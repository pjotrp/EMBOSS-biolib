/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1996, The Regents of the University of California    *
*	Author: Bill Grundy					       *
*								       *
***********************************************************************/
/* message.c */

#ifdef PARALLEL
 
#include <macros.h>
#include <mpi.h>
#include <mp.h>
#include <message.h>

/**********************************************************************
 *
 * void balance_loop1
 *
 * This function does load-balancing on the search for starting
 * points. It divides the dataset by the number of processors at the
 * character level. The work assigned to each processor is stored in a
 * global struct.
 *
 * IN: SAMPLE **samples             the dataset
 *     int n_samples                size of the dataset
 * OUT: SEQ_PARAMS *start_n_end     start and end points for this node
 *
 * This function is called in 'init.c'.
 **********************************************************************/
void balance_loop1(SAMPLE **samples, SEQ_PARAMS *start_n_end, int n_samples)
{
  int n_residues;
  float residues_per_node, target;
  int iseq, inode, seq_start, seq_end;
  
#ifdef NOBALANCE   /* No load balancing. */
  /* Each node gets a certain number of sequences. */
  float seqs_per_node = (float)n_samples / (float)mpNodes();
  start_n_end->start_seq = (int)(seqs_per_node * (float)mpMyID());
  start_n_end->start_off = 0;
  start_n_end->end_seq = start_n_end->start_seq + (int)ceil(seqs_per_node) - 1;
  if (start_n_end->end_seq >= n_samples)
    start_n_end->end_seq = n_samples - 1;
  start_n_end->end_off = samples[start_n_end->end_seq]->length - 1;
  /* fprintf(stderr, "%d: %d, %d -> %d, %d\n", mpMyID(), start_n_end->start_seq,
          start_n_end->start_off, start_n_end->end_seq, start_n_end->end_off);*/
  return;
#endif

  /* First count how many residues we have. */
  for (iseq = 0, n_residues = 0; iseq < n_samples; iseq++)
    n_residues += samples[iseq]->length;
  
  /* Calculate the number of residues per node. */
  residues_per_node = n_residues / mpNodes();
  
  /*
     if (mpMyID() == 0) {
       fprintf(stderr, "n_residues=%d residues_per_node=%g\n", n_residues,
	       residues_per_node);
     }
     */
  
  /* Set the starting points for node 0. */
  if (mpMyID() == 0) {
    start_n_end->start_seq = 0;
    start_n_end->start_off = 0;
  }
  
  /* Calculate per-node starting and ending points. */
  for (iseq = 0, seq_start = 0, seq_end = samples[0]->length - 1,
       inode = 0, target = residues_per_node;
       (target < n_residues) || (iseq < n_samples);
       /* Incrementing is done in the loop */) {
    
    /*
       if (mpMyID() == 0)
       fprintf(stderr, "seq=%d start=%d end=%d node=%d target=%g\n",
	       iseq, seq_start, seq_end, inode, target);
	       */
    
    /* See whether we've moved past the target. */      
    if (seq_end >= target) {
      if (mpMyID() == inode) {
	start_n_end->end_seq = iseq;
	start_n_end->end_off = (int)target - seq_start - 1;
      }
      if (mpMyID() == inode + 1) {
	start_n_end->start_seq = iseq;
	start_n_end->start_off = (int)target - seq_start;
      }

      /* Move to a new node. */
      inode++;
      /* Calculate the target for this node. */
      target = residues_per_node * (inode+1);

    } else {
      
      /* Move to a new sequence. */
      if (++iseq < n_samples) {
	/* Calculate the starting and ending points of this sequence. */
	seq_start = seq_end;
	seq_end += samples[iseq]->length; /* 1 more than the index. */
      }
    }
  }
  
  /* Set the ending points for the last node. */
  if (mpMyID() == mpNodes() - 1) {
    start_n_end->end_seq = n_samples - 1;
    start_n_end->end_off = samples[n_samples - 1]->length - 1;
  }
  
  /* Search for (and correct) fencepost problems. */
  if (start_n_end->start_off >= samples[start_n_end->start_seq]->length) {
    start_n_end->start_seq++;
    start_n_end->start_off = 0;
  } else if (start_n_end->start_off <= -1) {
    start_n_end->start_seq--;
    start_n_end->start_off = samples[start_n_end->start_seq]->length - 1;
  }
  
  if (start_n_end->end_off >= samples[start_n_end->end_seq]->length) {
    start_n_end->end_seq++;
    start_n_end->end_off = 0;
  } else if (start_n_end->end_off <= -1) {
    start_n_end->end_seq--;
    start_n_end->end_off = samples[start_n_end->end_seq]->length - 1;
  }
  
#ifdef DEBUG
  fprintf(stderr, "%d: (%d,%d) -> (%d,%d)\n", mpMyID(), 
	  start_n_end->start_seq, start_n_end->start_off,
	  start_n_end->end_seq, start_n_end->end_off);
#endif /* DEBUG */
  
  /* Do some error checking. */
  /*
     if (inode != mpNodes())
     fprintf(stderr, "Warning! inode(%d) != mpNodes(%d)\n",
	     inode, mpNodes());
	     */
  if (iseq != n_samples)
    fprintf(stderr, "Warning! iseq(%d) != n_samples(%d)\n",
	    iseq, n_samples);
}

/**********************************************************************
 *
 * void store_consensus
 *
 * Because the reduction across models does not include the
 * candidates, we must transfer the human-readable consensus string
 * into the model so that it can be printed later.
 *
 * This function is called by 'meme.c'.
 **********************************************************************/
void store_consensus(MODEL *model, CANDIDATE *candidates)
{
    int nc = model->c;                        /* no. of components in model */
    int w = model->w[nc-1];                   /* width of last component */
    S_POINT *s_point = candidates[w].s_point; /* starting point for model */

    /* Make sure we have at least one candidate. */
    if (s_point == NULL) {
      /* Guarantee that this model won't get chosen. */
      model->sig = BIG;
    } else {
      /*fprintf(stderr, "%d: BEFORE consensus=%s\n", mpMyID(),
	s_point->cons0);*/
      strcpy(model->cons0, s_point->cons0);
    }
}


/**********************************************************************
 *
 * void save_theta_ptrs
 *
 * When we broadcast the winning model, the array of pointers in that
 * model gets sent to everyone. Since those pointers are nonsense to
 * other nodes, those nodes have to store their own pointers and then
 * restore them after the broadcast.
 *
 **********************************************************************/

void save_theta_ptrs(MODEL *model, int save_or_restore)
{
  static THETA saved_thetas[MAXG];
  static THETA saved_obs[MAXG];
  int i_component;

  /* Find out how many components the model has. */
  int num_components = model->c;

  /* Iterate through the components. */
  for (i_component = 0; i_component < num_components; i_component++) {

    if (save_or_restore == 1) {
      /* Save the theta matrix pointer for this component. */
      saved_thetas[i_component] = model->theta[i_component];
      saved_obs[i_component] = model->obs[i_component];
    } else {
      model->theta[i_component] = saved_thetas[i_component];
      model->obs[i_component] = saved_obs[i_component];
    }
  }
}

/**********************************************************************
 *
 * void theta_packer
 *
 * This function packs (or unpacks) the 2-D theta matrix into a 1-D
 * array so that it can be sent in a single broadcast.
 * 
 **********************************************************************/
void theta_packer(
   MODEL *model,
   double *theta_matrix,
   double *obs_matrix,
   int pack_or_unpack,
   int alength
)
{
  THETA theta, obs;

  /* Find out how many components the model has. */
  int num_components = model->c;
  int i_component, width, i, j;

  /* An index for stepping through the linear theta matrix. */
  int i_theta = 0;

  /* Iterate through the components. */
  for (i_component = 0; i_component < num_components; i_component++) {

    /* Get the theta matrix for this component. */
    theta = model->theta[i_component];
    obs = model->obs[i_component];

    /* Find the width of this component. */
    width = model->w[i_component];
    
    /*
    fprintf(stderr, "%d: alength=%d num_components=%d width=%d\n",
	    mpMyID(), alength, num_components, width);
	    */

    /* Iterate across positions. */
    for (i = 0; i < width; i++) {
      /* Iterate through the letters in the dataset. */
      for (j = 0; j < alength; j++) {

	/* Copy from theta into the theta matrix or vice versa. */
	if (pack_or_unpack == 1) {
	  theta_matrix[i_theta] = theta(i, j);
	  obs_matrix[i_theta++] = obs[i][j];
	} else {
	  theta(i, j) = theta_matrix[i_theta];
	  obs[i][j] = theta_matrix[i_theta++];
	}
      }
    }
  }
}

/**********************************************************************
 *
 * void max_packets
 *
 * Given two packets, select the best.
 * This function is used in the reduction across models.
 *
 **********************************************************************/
void max_packets (void *f_data, void *f_result, int *f_length,
		  MPI_Datatype *datatype)
{
  /* Compare the data in the two packets. */
  if ((((REDUCE_PACKET *)f_data)->data < ((REDUCE_PACKET *)f_result)->data) ||

      /* If they're equal, discriminate by ID. */
      ((((REDUCE_PACKET *)f_data)->data == ((REDUCE_PACKET *)f_result)->data)
       && (((REDUCE_PACKET *)f_data)->ID < ((REDUCE_PACKET *)f_result)->ID))) {

    /* Put the new data into the result position. */
    ((REDUCE_PACKET *)f_result)->data = ((REDUCE_PACKET *)f_data)->data;
    ((REDUCE_PACKET *)f_result)->ID = ((REDUCE_PACKET *)f_data)->ID;
  }
}

/**********************************************************************
 *
 * void reduce_across_models
 *
 * Do a reduction across an entire model. The model with the lowest
 * score in the 'sig' field gets propagated to every node.
 *
 * This function is called by 'meme.c'.
 **********************************************************************/
void reduce_across_models(
   MODEL *model,
   int alength
)
{
  static int init;
  static MPI_Datatype reduction_packet_type;
  static MPI_Op max_packets_op;
  REDUCE_PACKET a_packet, best_packet;
  double theta_matrix[MAXALPH * (MAXSITE + 1)];
  double obs_matrix[MAXALPH * (MAXSITE + 1)];

  /* NOTE: sizeof(MODEL) = 7040 bytes */

  /* Add up the number of EM iterations from each node. */
  mpReduceAdd(&(model->iter));

  /* Package the best model's sig together with the processor ID. */
  a_packet.data = model->sig;
  a_packet.ID = (double)mpMyID();
    
  /*fprintf(stdout, "%d: Prior score=%g\n", mpMyID(), a_packet.data);*/

  /* Reduce across all the sig-ID pairs. */
  /* Make sure we have a handle for the reduction function. */
  if (init == 0) {
    init = 1;
    MPI_Type_contiguous(2, MPI_DOUBLE, &reduction_packet_type);
    MPI_Type_commit(&reduction_packet_type);
    MPI_Op_create(max_packets, TRUE, &max_packets_op);
  }

  /* Do the reduction. */
  MPI_Allreduce((void *)&a_packet, (void *)&best_packet, 1,
		reduction_packet_type, max_packets_op, MPI_COMM_WORLD);

  /*fprintf(stdout, "%d: Winner=%d score=%g\n", mpMyID(), (int)best_packet.ID,
	  best_packet.data);*/

  /* The losers store their theta matrix pointers so they don't get
     overwritten in the upcoming broadcast. */
  if (mpMyID() != (int)best_packet.ID)
    save_theta_ptrs(model, 1);

  /* Broadcast the best model from the winning ID. */
  mpBroadcast((void *)model, sizeof(MODEL), (int)best_packet.ID);

  /*fprintf(stderr, "%d: Past model broadcast.\n", mpMyID());
  fflush(stderr);*/

  /* After the broadcast, the losers restore their original pointers. */
  if (mpMyID() != (int)best_packet.ID)
    save_theta_ptrs(model, 0);

  /* The winner packages the theta matrix up into a linear array. */
  if (mpMyID() == (int)best_packet.ID)
    theta_packer(model, theta_matrix, obs_matrix, 1, alength);

  /* Broadcast the theta and obs matrix arrays. */
  mpBroadcast((void *)theta_matrix, sizeof(double) * MAXALPH * (MAXSITE + 1),
	      (int)best_packet.ID);
  mpBroadcast((void *)obs_matrix, sizeof(double) * MAXALPH * (MAXSITE + 1),
	      (int)best_packet.ID);

  /*fprintf(stderr, "%d: Past theta broadcast.\n", mpMyID());
  fflush(stderr);*/

  /* Everyone else unpacks the theta matrix. */
  if (mpMyID() != (int)best_packet.ID)
    theta_packer(model, theta_matrix, obs_matrix, 0, alength);
}

/**********************************************************************
 *
 * void max_s_packets
 *
 * Find the maximum from two s_point packets.
 * This function is used in the reduction across starting points.
 *
 **********************************************************************/
void max_s_packets(void *f_data, void *f_result, int *f_length,
		   MPI_Datatype *datatype)
{
  int i_nsites0;
  double f_result_score, f_data_score;

  /* Compare each s_point in the array. */
  for (i_nsites0 = 0; i_nsites0 < *f_length; i_nsites0++) {

    /* Get the two scores. */
    f_result_score = ((S_POINT_PACKET *)f_result + i_nsites0)->score;
    f_data_score = ((S_POINT_PACKET *)f_data + i_nsites0)->score;
    /*fprintf(stdout, "f_result_score=%g f_data_score=%g\n", f_result_score,
	    f_data_score);*/

    /* Compare the two scores. */
    if ((f_result_score < f_data_score) || 

	/* If the two scores are ==, take the one from the earlier sequence. */
	((f_result_score == f_data_score) &&
	 (((S_POINT_PACKET *)f_data + i_nsites0)->iseq <
	  ((S_POINT_PACKET *)f_result + i_nsites0)->iseq))) {


      /* Copy the new data into the result location. */
      ((S_POINT_PACKET *)f_result + i_nsites0)->score =
	((S_POINT_PACKET *)f_data + i_nsites0)->score;
      ((S_POINT_PACKET *)f_result + i_nsites0)->iseq =
	((S_POINT_PACKET *)f_data + i_nsites0)->iseq;
      ((S_POINT_PACKET *)f_result + i_nsites0)->ioff =
	((S_POINT_PACKET *)f_data + i_nsites0)->ioff;
    }
  }
}

/**********************************************************************
 *
 * void reduce_across_s_points
 *
 * Do a reduction across an array of starting points. For each
 * position in the array, the starting point with the lowest value in
 * the 'score' field gets propagated to every node.
 *
 * This function is called by subseq7.c.
 **********************************************************************/
void reduce_across_s_points(
  S_POINT *s_points, 
  SAMPLE **samples, 
  int n_nsites0,
  int n_starts
)
{
  static int init;
  static MPI_Datatype s_point_packet_type;
  static MPI_Op max_s_packets_op;
  int i_packet;
  S_POINT_PACKET packets[1000], best_packets[1000];
  
  /* 
     NOTE: sizeof(S_POINT) = 344 bytes
     sizeof(S_POINT_PACKET) = 16 bytes
     */
  
  /* Package the scores into an array of packets. */
  /* Don't need w0 & nsites0 because they're invariant across processors.*/
  /* Don't want e_cons0 & cons0 because they are pointers
     (We recalculate e_cons0 and contents of cons0 later). */
  for (i_packet = 0; i_packet < n_nsites0; i_packet++) {
    packets[i_packet].score = s_points[i_packet].score;
    packets[i_packet].iseq = (double)s_points[i_packet].iseq;
    packets[i_packet].ioff = (double)s_points[i_packet].ioff;
  }

  /*
  printf("BEFORE\n");
  for (i_packet = 0; i_packet < n_nsites0; i_packet++)
    fprintf(stdout, "node %d, packet %d: score=%g iseq=%d ioff=%d\n",
	    mpMyID(), i_packet, packets[i_packet].score,
	    (int)packets[i_packet].iseq, (int)packets[i_packet].ioff);
  fflush(stdout);
  */

  /* MPI */
  /* Reduce across the list of packets. */
  if (init == 0) {
    init = 1;
    MPI_Type_contiguous(3, MPI_DOUBLE, &s_point_packet_type);
    MPI_Type_commit(&s_point_packet_type);
    MPI_Op_create(max_s_packets, TRUE, &max_s_packets_op);
  }

  /* Do the reduction. */
  MPI_Allreduce((void *)&packets, (void *)&best_packets, n_nsites0,
		s_point_packet_type, max_s_packets_op, MPI_COMM_WORLD);

  /*
  printf("AFTER\n");
  for (i_packet = 0; i_packet < n_nsites0; i_packet++)
    fprintf(stdout, "node %d, packet %d: score=%g iseq=%d ioff=%d\n",
	    mpMyID(), i_packet, best_packets[i_packet].score,
	    (int)best_packets[i_packet].iseq,
	    (int)best_packets[i_packet].ioff);
  fflush(stdout);
  */
  
  /* Unpack the data. */
  for (i_packet = 0; i_packet < n_nsites0; i_packet++) {
    s_points[i_packet].score = best_packets[i_packet].score;
    s_points[i_packet].iseq = (int)best_packets[i_packet].iseq;
    s_points[i_packet].ioff = (int)best_packets[i_packet].ioff;

    /* Set the e_cons0 fields to the proper memory locations. */
    s_points[i_packet].e_cons0 =
      samples[s_points[i_packet].iseq]->res + s_points[i_packet].ioff;
  }
  
  /* Add up all of the values of n_starts. */
  mpReduceAdd(&n_starts);
}

/**********************************************************************
 * void print_model
 *
 * Print some of the contents of a model.
 * (Debugging only.)
 **********************************************************************/
extern void print_model(
   char *label,
   MODEL *model
)
{		       
  fprintf(stderr, "%d: %s -- sig=%1.2e e_ll_0=%1.2e e_ll=%2.1e",
	  mpMyID(), label, model->sig, model->e_ll_0, model->e_ll);
  fprintf(stderr, "ll=%1.2e lrt=%1.2e bon=%1.2e root=%1.2e\n",
	  model->ll, model->lrt, model->bon, model->root);
  fflush(stdout);
}


#endif /* PARALLEL */
