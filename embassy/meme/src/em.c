/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1994, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/
/* em.c */

#include "meme.h"
 
static BOOLEAN check_convergence(
  THETA	old_theta,			/* before EM iteration */
  THETA	new_theta,			/* after EM iteration */
  int w,				/* width of motif */
  double distance,			/* convergence radius */
  int alength, 				/* alphabet length */
  int iter,				/* current iteration number */
  int maxiter				/* maximum iterations */
);

/**********************************************************************/
/*
	em

	Uses a version of the EM algorithm (expectation maximization).

*/
/**********************************************************************/
extern void em(
  MODEL *model,			/* the model */
  DATASET *dataset,		/* the dataset */
  PRIORS *priors,		/* the priors */
  int maxiter,			/* maximum number of iterations */
  double distance		/* stopping criterion */
)
{
  int alength = dataset->alength;
  THETA theta_save;
  int iter;			/* iteration number */
  double (*E_STEP)(MODEL *, DATASET *); /* expectation step function */
  double (*E_STEP0)(MODEL *, DATASET *); /* expectation step function */
  /* maximization step function */
  void (*M_STEP)(MODEL *, DATASET *, PRIORS *, int);	
  int nc = model->c;		/* number of components of model */
  int max_w = model->w[nc-1];	/* width of last component */
  BOOLEAN converged = FALSE;	/* EM has converged */

  /* create a place to save old value of theta */
  create_2array(theta_save, double, max_w, alength);

  /* set up the correct type of EM to run */
  M_STEP = m_step;
  E_STEP = e_step;
  E_STEP0 = e_step;
  switch (model->mtype) {
    case Oops:
      E_STEP = e_step;
      break;
    case Zoops:
      E_STEP = zoops_e_step;
      break;
    case Tcm:
      E_STEP = tcm_e_step;
      break;
    default:
      fprintf(stderr,"Unknown model type in em()! \n");
      exit(1);
      break;
  }
  /* use like_e_step to set z matrix on iteration 0 if motifs were given */
  if (dataset->nmotifs > 0) {E_STEP0 = E_STEP; E_STEP = like_e_step;}

  /* get the probability that a site starting at position x_ij would
     NOT overlap a previously found motif; used in E_STEP.
  */
  get_not_o(dataset, model->w[1], FALSE);

  /* Perform EM for number of iterations or until no improvement */
  for (iter=0; iter < maxiter; iter++) {
    int w = model->w[nc-1];	/* width of model */
    THETA theta = model->theta[nc-1];	/* final theta of last component */

    if (iter > 0 && dataset->nmotifs > 0) E_STEP = E_STEP0;

    if (PRINTALL) ajFmtPrintF(outf,"\niter %d\n", iter);
#ifdef PARALLEL
    /* If we're running in parallel, only print from one node. */
    if (mpMyID() == 0)
#endif
    if ((!NO_STATUS) && ((iter % 10) == 0))
      fprintf(stderr, "\rem: w=%4d, iter=%4d                       ", w, iter);

    /* fix this later: save current contents of theta */
    copy_theta(theta, theta_save, w, alength);

    /* expectation step */
    model->ll = E_STEP(model, dataset);

    /* maximization step */
    M_STEP(model, dataset, priors, iter);

    /* print status if requested */
    if (PRINT_LL) {
      double m1, e1;
      double nsites = model->lambda[1] * ps(dataset, model->w[1]);
      calc_like(model, dataset);
      exp10_logx(model->sig, m1, e1);
      ajFmtPrintF(outf,"iter=%d w=%d ll=%8.2f e_ll=%8.2f nsites=%6.1f sig=%5.3fe%+04.0f",
      iter, model->w[1], model->ll, model->e_ll, nsites, m1, e1);
    }
    if (PRINTALL) {
      int c;
      for (c=0; c<nc; c++) {
        ajFmtPrintF(outf,"component %2d: lambda= %8.6f\n", c,
		    model->lambda[c]);
        print_theta(2, model->theta[c], model->w[c], "", dataset, NULL); 
        print_theta(2, model->obs[c], model->w[c], "", dataset, NULL); 
      }
    }
    if (PRINT_Z) print_zij(dataset, model);

    /* see if EM has converged */
    converged = check_convergence(theta_save, theta, w, distance, alength,
      iter, maxiter);

    if (converged) {iter++; break;}		/* done */
  }

  /* save the number of iterations (counting from zero)*/
  model->iter += iter;

  /* get the consensus of each component of the model */
  {
    THETA theta = model->theta[1];
    int w = model->w[1];
    char *cons = model->cons;
    cons = get_consensus(theta, w, dataset, 1, MINCONS); 
  }

  /* calculate the expected likelihood of the model */
  calc_like(model, dataset);

  free_2array(theta_save, max_w);
} 

/**********************************************************************/
/*
	check_convergence
*/
/**********************************************************************/
static BOOLEAN check_convergence(
  THETA	old_theta,			/* before EM iteration */
  THETA	new_theta,			/* after EM iteration */
  int w,				/* width of motif */
  double distance,			/* convergence radius */
  int alength, 				/* alphabet length */
  int iter,				/* current iteration number */
  int maxiter				/* maximum iterations */
)
{
  int i, j;
  double euclid;		/* distance between old_theta and new_theta */
  BOOLEAN converged;

  /* calculate the euclidean change in theta */
  euclid = 0;
  for(i=0; i<w; i++) {
    for(j=0; j<alength; j++) {
      double diff = theta_ref(old_theta, i, j) - theta_ref(new_theta, i, j);
      euclid += diff * diff;
    }
  }
  euclid = sqrt(euclid);
  if (PRINTALL || PRINT_LL) {ajFmtPrintF(outf," d_theta = %f\n", euclid);}

  if (euclid < distance) {		/* converged? */
    if (TRACE) printf("Converged to motif (< %g change) after %d iterations\n",
      distance, iter+1);
    converged = TRUE;
  } else if (maxiter > 1 && iter == maxiter - 1) {
    /* Use fprintf to print from all nodes in parallel. */
    if (TRACE) 
      fprintf(stdout, "Failed to converge after %d iterations!\n", maxiter);
    converged = FALSE;
  } else {
    converged = FALSE;
  }

  return converged;
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/em.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
