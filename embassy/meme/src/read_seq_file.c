/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* read_seq_file.c */
/*
	Supports FASTA and SWISSPROT formats.

Note:
	Assumes SWISSPROT format if "ID" appears at beginning of first line.
	Ignores duplicate (same <id>) sequences.

All formats:
	<id>		:= <alpha>+
			   The *unique* name of the sequence; may
			   be no longer than 2*MSN characters; may *not*
			   contain whitespace!

			   NOTE: if <id> is "WEIGHTS", the <description>
			   field is parsed as a string of sequence weights 
			   and any <sequence> field (if any) is ignored.
			   All weights are assigned in order to the 
			   sequences in the file. If there are more sequences
			   than weights, the remainder are given weight one.
			   Weights must be greater than zero and less than
			   or equal to one.  Weights may be specified by
			   more than one "WEIGHT" entry which may appear
			   anywhere in the file.

	<description>	:= <alpha>+
			   A description of the sequence;
		           may contain whitespace.

	<sequence>	:= <alpha>+
			   The DNA or protein sequence data; 
                           may contain whitespace.

	<text>		:= <alpha>*
			   Text which is ignored.


Pearson FASTA format:
	INPUT DATA FILE FORMAT:
		[
		<header>
		<sequence>+
		]*

	<header>	:= ">"<id>" "<description>
			    Everything up to the first space is 
			    assumed to be the unique name of the sequence.
	
SWISSPROT Format
	[
	<ID>
	<DE>+
	<SQ>
	<sequence>+
	//
	]*

	<ID>		:= "ID   "<id>" "<text>
	<DE>		:= "DE   "<description>
	<SQ>		:= "SQ   "<text>

*/

#include "meme.h"

/* size of memory chunks to allocate for sequence data */
#define RCHUNK 100

/* maximum size of sequence description text string */
#define MAXDELEN 10000

/* local types */
typedef enum {FASTA, SWISSPROT} FORMAT_TYPE;

/* local functions */
static SAMPLE *create_sample(
  char *alpha,		/* the alphabet */
  int length,		/* length of sequence */
  char *name,		/* name of sample */
  char *sequence  	/* the sequence */
);

/* local functions */
static int read_sequence_data(
  AjPSeq eseq,		/* data file of sequences */
  char **sequence, 		/* sequence data */
  char *name			/* name of sequence */
);
static int read_sequence_de(
  char *desc,			/* data file of sequences */
  char **descriptor		/* sequence descriptor */
);

/* local variables */
#define DATA_HASH_SIZE 100000
/* 
   The hash table will be appended to if read_seq_file is called more
   than once.  This is so that positive sequences the negative sequence file
   will be ignored.
*/ 
static HASH_TABLE ht_seq_names = NULL;	/* hash of dataset seq names */

/**********************************************************************/
/*
	read_seq_file

	Open a sequence file and read in the sequences. 
	Returns a dataset->

	setup_hash_alphabet must have been called prior to first call to this.
*/
/**********************************************************************/
extern DATASET *read_seq_file(
  AjPSeqall seqa,		/* name of file to open */
  char *alpha,			/* alphabet used in sequences */
  BOOLEAN use_comp,		/* use complementary strands, too */
  double seqfrac		/* fraction of input sequences to use */
)
{
  int i, j, pcol;
  FILE *data_file;		/* file with samples to read */
  char *sample_name;		/* name of sample read */
  char *sample_de;		/* descriptor text for sample */
  char *sequence;		/* sequence read */
  int length;			/* length of sequence */
  BOOLEAN error=FALSE;		/* none yet */
  SAMPLE *sample;		/* sample created */
  DATASET *dataset;		/* dataset created */
  int n_samples=0;		/* number of samples read */
  double *seq_weights=NULL;	/* sequence weights */
  int n_wgts=0;			/* number of sequence weights given */

  /* create a hash table of sequence names */
  if (!ht_seq_names) ht_seq_names = hash_create(DATA_HASH_SIZE);

  /* create a dataset */
  dataset = (DATASET *) malloc(sizeof(DATASET));
  dataset->alength = strlen(alpha);
  dataset->alphabet = alpha;
  dataset->pal = 0;		/* not looking for palindromes */


  /* initialize maximum length of sequences */
  dataset->max_slength = 0;
  dataset->min_slength = 10000000;

  dataset->n_samples = 0;	/* no samples yet */
  dataset->samples = NULL;	/* no samples */

  while (read_sequence(seqa, &sample_name, &sample_de, &sequence, 
    &length)) {

    /* skip sequence if an error occurred */
    if (length < 0) continue;

    /* parse weights if given; make (more than enough) room in array */
    if (strcmp(sample_name, "WEIGHTS")==0) {
      double wgt; 
      char *wgt_str = sample_de;
      Resize(seq_weights, n_wgts+strlen(wgt_str), double);
      while (sscanf(wgt_str, "%lf", &wgt) == 1) {
        if (wgt <= 0 || wgt > 1) {
	  fprintf(stderr, 
            "Weights must be larger than zero and no greater than 1.\n");
	  exit(1);
        }
        seq_weights[n_wgts++] = wgt;			/* save weight */
        wgt_str += strspn(wgt_str, "      ");		/* skip white */
        wgt_str += strcspn(wgt_str, "     ");		/* skip token */
      }
      myfree(sample_name);
      myfree(sample_de);
      myfree(sequence);
      continue;
    }

    /* ignore duplicate (same sample name) sequences */ 
    if (hash_lookup(sample_name, 0, ht_seq_names)) {
      fprintf(stderr, "Skipping %s\n", sample_name);
      myfree(sample_name);
      myfree(sample_de);
      myfree(sequence);
      continue;
    }
    hash_insert(sample_name, 0, ht_seq_names);  /* put name in hash table */

    n_samples++;

    /* see if sequence will be used in random sample; store it if yes */
    if (drand48() >= 1 - seqfrac) {

      /* create the sample */
      sample = create_sample(alpha, length, sample_name, sequence);
      if (sample == NULL) {error = TRUE; continue;}

      /* record maximum length of actual sequences */
      dataset->max_slength = MAX(sample->length, dataset->max_slength);
      dataset->min_slength = MIN(sample->length, dataset->min_slength);

      /* put the sample in the array of samples */
      if ((dataset->n_samples % RCHUNK) == 0) {
        Resize(dataset->samples, dataset->n_samples + RCHUNK, SAMPLE *);
      }
      dataset->samples[dataset->n_samples++] = sample;

    }

  } /* sequences */
/*  printf("\n\n");*/

  /* resize the array of samples */
  if (dataset->n_samples) Resize(dataset->samples, dataset->n_samples, SAMPLE*);

  /* check that datafile contained at least one sample */
  if (!error) {
    if (n_samples == 0) {
      fprintf(stderr, "No sequences found.  Check file format.\n");
      error = TRUE;
    } else if (dataset->n_samples == 0) {
      fprintf(stderr, 
        "No sequences sampled.  Use different seed or higher seqfrac.\n");
      error = TRUE;
    }
  }

  /* exit if there was an error */
  if (error) exit(1);

  /* calculate the prior residue frequencies and entropies 
     and |D|, size of the dataset */
/* tlb; 5/9/97 wgt_total_res */
  dataset->res_freq = dataset->adj_freq = NULL; 
  Resize(dataset->res_freq, dataset->alength, double);
  Resize(dataset->adj_freq, dataset->alength, double);
  for (i=0; i<dataset->alength; i++) { dataset->res_freq[i] = 0; }
  dataset->total_res = 0;
  dataset->wgt_total_res = 0;
  for (i=0; i<dataset->n_samples; i++) {		/* sequence */
    int slen = dataset->samples[i]->length;
    double sw = dataset->samples[i]->sw = (n_wgts > i ? seq_weights[i] : 1);
    dataset->total_res += slen;
    dataset->wgt_total_res += slen*sw;
    for (j=0; j<dataset->alength; j++) {
      if (use_comp) { 	/* using complementary strand as well */
        dataset->res_freq[j] += sw * dataset->samples[i]->counts[j]/2.0;
        dataset->res_freq[dna_comp(j)] += sw*dataset->samples[i]->counts[j]/2.0;
      } else {		/* not using complementary strands */
        dataset->res_freq[j] += sw * dataset->samples[i]->counts[j];
      }
    }
  }

  dataset->entropy = 0;
  /* number of non-zero letter freqs */
  dataset->effective_alength = dataset->alength;
  pcol = 0;
  for (i=0; i<dataset->alength; i++) {
    if (dataset->res_freq[i] == 0) dataset->effective_alength--; 
    dataset->res_freq[i] /= dataset->wgt_total_res;
    dataset->entropy += dataset->res_freq[i] * LOG2(dataset->res_freq[i]);
  }

  return dataset;
}

/**********************************************************************/
/*
	create_sample

	Create a sample.

	Returns the sample or NULL on error.

*/
/**********************************************************************/
static SAMPLE *create_sample(
  char *alpha,		/* the alphabet */
  int length,		/* length of sequence */
  char *name,		/* name of sample */
  char *sequence  	/* the sequence */
)
{
  SAMPLE *new1;
  int i; 

  /* disallow zero length samples */
  if (length == 0) {
    fprintf(stderr, "\nZero length sequences not allowed. (Sequence `%s').\n",
      name);
    return NULL;
  }

  /* create the record to hold the sample and its associated data */
  new1 = (SAMPLE *) malloc(sizeof(SAMPLE));

  /* assign the name and sequence data */
  new1->sample_name = name;

  /* set up encoded version of sequence and weights */
  new1->res = (char *) malloc(length * (int) sizeof(char));
  new1->resc = (char *) malloc(length * (int) sizeof(char));
  new1->weights = (double *) malloc(length * (int) sizeof(double));
  new1->not_o = (double *) malloc(length * (int) sizeof(double));
  new1->log_not_o = (int *) malloc(length * (int) sizeof(int));
  for (i=0; i<length; i++) { 
    int c = (int) sequence[i];
    int e = hash(c);
    new1->res[i] = e;
    new1->resc[i] = dna_comp(e);
    new1->weights[i] = 1.0;
  }

  /* set up arrays to hold posterior probabilities for group 1 */
  create_2array(new1->pY, int, 1, length);

  /* set up array expected value of missing information */
  create_2array(new1->z, double, 2, length);
  myfree(new1->z[0]);			/* only one z used-- motif */

  /* set up array to hold character counts */
  new1->counts = (int *) calloc(strlen(alpha), (int) sizeof(int));

  /* get character counts */
  for (i=0; i<length; i++) (new1->counts[new1->res[i]])++;

  /* record length of sequence */
  new1->length = length;

  return new1;
} /* create_sample */

/**********************************************************************/
/*
	read_sequence

	Read a single sequence from the data file.
	Returns FALSE on EOF or bad sequence data.

	Supports FASTA and SWISSPROT formats.

	Note:
	setup_hash_alphabet must have been called prior to first call to this.
*/
/**********************************************************************/
extern BOOLEAN read_sequence(
  AjPSeqall seqa,		/* file containing sequences */
  char **sample_name,		/* unique identifier of sequence */
  char **sample_de,		/* descriptor of sequence */
  char **sequence,		/* protein or DNA letters */
  int *length			/* length of sequence */
)
{
  int i, c;
  int msn = 2 * MSN;			/* define maximum internal name size */
  FORMAT_TYPE format = FASTA;		/* assume FASTA format */
  char *name = NULL;
  AjPSeq eseq=NULL;
  char *p=NULL;

  if(!ajSeqallNext(seqa,&eseq))
      return FALSE;

  /* get the sample name; truncate to msn characters */
  Resize(name, msn+1, char);

  if(strlen(ajSeqName(eseq))>msn)
      strncpy(name,ajSeqName(eseq),msn);
  else
      strcpy(name,ajSeqName(eseq));
  name[msn]='\0';

  p = ajStrStr(ajSeqGetDesc(eseq));
  
  /* read in description */
  *sample_de = NULL;			/* in case no description found */
  read_sequence_de(p,sample_de);



  /* read in the actual sequence data */
  *length = read_sequence_data(eseq, sequence, name);

  /* sequence had bad data */
  if (*length < 0) {
    myfree(name);
    myfree(*sample_de);
    return FALSE;
  }

  *sample_name = name;
  /* insure that the description string exists */
  if (*sample_de == NULL) {
    Resize(*sample_de, 1, char);
    (*sample_de)[0] = '\0';
  }

  return TRUE;
}

/**********************************************************************/
/*
	read_sequence_data

	Read the sequence data into a dynamic array.
	Converts to upper case.
	Checks for illegal sequence characters.

	Returns the length of the sequence or -1 on error.
*/
/**********************************************************************/
static int read_sequence_data(
  AjPSeq eseq,			/* data file of sequences */
  char **sequence, 		/* sequence data */
  char *name			/* name of sequence */
)
{
  int length, c;
  char *seq = NULL;

  /* 
    read sample sequence 
  */

  length = ajSeqLen(eseq);
  
  Resize(seq, ajSeqLen(eseq)+1, char);
  strcpy(seq,ajSeqChar(eseq));
  seq[length] = '\0';

  *sequence = seq;

  return length;
}

/**********************************************************************/
/*
	read_sequence_de

	Read the sequence descriptor into a dynamic array.
*/
/**********************************************************************/
static int read_sequence_de(
  char *desc,			/* data file of sequences */
  char **descriptor		/* sequence descriptor; if not NULL,
				   string will be appended */
)
{
  int length, c;
  char *de = *descriptor;


  length = strlen(desc);
  
  Resize(de, length+1, char);
  strcpy(de,desc);
  de[length] = '\0';

  *descriptor = de;

  return length;
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/read_seq_file.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
