/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* init.c */
/*	
	Initialize meme.
*/

#include "meme.h"
#include "logs.h"
#include "general.h"
#include "banner.h"
#include "split.h"

#ifndef EXP
#define EXP 0
#else
#define EXP 1
#endif

#ifndef PARALLEL
#define PARAFLAG 0
#else
#define PARAFLAG 1
#endif

#define PLIB "Eprior30.plib"

static char *yesno[] = {"no", "yes"};

/* User input parameters */
AjPStr ajdatafile=NULL;
AjPStr ajnegfile=NULL;
AjPStr *ajntype=NULL;
AjPStr *ajmodel=NULL;
AjPStr ajmfile=NULL;
AjPStr *ajprior=NULL;

static char *datafile = NULL;	/* positive examples */
static char *negfile = NULL;	/* negative examples */
static char *ntype = "pair";	/* output two matrices per motif if negatives */
static char *plib_name = NULL;	/* use default library */
static char *mod = "zoops";	/* model type input string; default ZOOPS */
static char *adj = "root";	/* LRT adjustment type input string */
static char *alph = "PROTEIN";	/* default alphabet IUPAC protein 1-letter */
static BOOLEAN strands[] = {TRUE, FALSE, FALSE, FALSE};	/* strands to use */
static int pal = 0;		/* = 0, no palindromes
				   = 1, allow DNA palindromes,
				   = 2, force DNA palindromes
				*/
static BOOLEAN shortn = TRUE;	/* shorten motif if possible */
static double distance = 1e-3;	/* squared euclidean distance for convergence */
static char *prior = NULL;	/* prior type input string */
static double beta = -1;	/* scale factor for prior; defaults differ */
static double prob = 1.0;	/* try enough subsequences so P=prob */
static int nmotifs = 1;		/* number of motifs to find */
static char *mfile = NULL;	/* name of known .motifs file*/
static int maxiter = 50;        /* max number iterations of EM on best start */
static double nsites = 0;	/* try one value of nsites0 only if > 0 */
static double min_nsites = 0;	/* minimum nsites0 to try; set in get_nsites0 */
static double max_nsites = 0;	/* maximum nsites0 to try; set in get_nsites0 */
static int w = 0;		/* width of motifs */
static int min_w = MIN_W;	/* minimum W0 to try */
static int max_w = MAX_W;	/* maximum W0 to try */
static MAP_TYPE map_type; 	/* type of sequence to theta mapping */
static char *MAP = NULL;	/* map type input string */
static double map_scale=-1; 	/* scale of sequence to theta mapping:
					Uni - size of add-n prior (n)
					Pam - PAM distance (120) 
				   Default set in init_em.
				*/
int n_spcons = 1;		/* number of specified start points */
static char *spcons[MAXG];	/* starting point consensus strings */
static double chi_alpha = 1.0;	/* minimum significance level */
static int maxsize= 100000; 	/* dataset size limit */
static int seed = 0;		/* random number seed */
static double seqfrac = 1;	/* fraction of input dataset sequences to use */

/**********************************************************************/
/*
        init_meme

        Set up all the stuff for meme.
*/
/**********************************************************************/
extern void init_meme(
  int argc,		/* number of input arguments */
  char **argv,		/* input arguments */
  MODEL **model_p,      /* the model */
  MODEL **best_model_p, /* the model */
  DATASET **dataset_p,  /* the dataset */
  THETA *map_p,         /* letter to frequency vector mapping */
  PRIORS **priors_p,    /* the prior probabilities model */
  P_POINT **p_point_p,	/* previously learned starting points */
  double *min_nsites_p,	/* minimum value of nsites */
  double *max_nsites_p,	/* maximum value of nsites */
  int *min_w_p,		/* minimum width for motifs */
  int *max_w_p,		/* maximum width for motifs */
  BOOLEAN *shortn_p,	/* shorten motif flag */
  double *distance_p,   /* convergence radius */
  double *prob_p,	/* sampling probability for subsequence starts */
  int *nmotifs_p,	/* number of motifs to find */
  int *maxiter_p,	/* maximum number of iterations for EM */ 
  double *chi_alpha_p,	/* width significance level */
  FILE **debug_file_p,	/* holds debug info */
  MODEL **neg_model_p,	/* model of negative examples */
  DATASET **neg_dataset_p /* dataset of negative examples */
)
{
  int i, j, cc=0, pcol;
  char *DNA = DNA0;
  char *PROTEIN = PROTEIN0;
  char *meme_directory; 
  MTYPE mtype;
  PTYPE ptype;

  char *alphabet;
  int alength;
  DATASET *dataset;
  P_POINT *p_point;
  BOOLEAN use_comp;			/* use complementary strand */
  FILE *debug_file = NULL;
  AjBool ajprotein;
  AjBool ajnucleic;
  AjBool ajpal;
  float ajnsites;
  float ajmaxsites;
  float ajminsites;
  AjBool brief;
  AjPStr *ajspmap=NULL;
  AjPStr ajcons=NULL;
  AjBool ajstatus;
  AjBool ajalign;
  

  ajNamInit("emboss");
  ajAcdInitP("meme",argc,argv,"MEME");


  seqall = ajAcdGetSeqall("sequence");

  datnam = ajStrNewC(ajStrStr(seqall->Seqin->Usa));

/*  seqalln  = ajAcdGetSeqall("negfile");*/

  ajntype = ajAcdGetList("ntype");
  ntype = ajStrStr(*ajntype);

  ajmodel = ajAcdGetList("model");
  mod = ajStrStr(*ajmodel);

  ajprotein = ajAcdGetBool("protein");
  if(ajprotein)
      alph = "PROTEIN";
  ajnucleic = ajAcdGetBool("nucleic");
  if(ajnucleic)
      alph = "DNA";

  pal=0;
  ajpal = ajAcdGetBool("palindromes");
  if(ajpal)
      pal=1;
  ajpal = ajAcdGetBool("ponly");
  if(ajpal)
      pal=2;

  shortn = ajAcdGetBool("shorten");
  
  ajnsites = ajAcdGetFloat("nsites");
  nsites = (double) ajnsites;
  
  ajmaxsites = ajAcdGetFloat("maxsites");
  max_nsites = (double) ajmaxsites;

  ajminsites = ajAcdGetFloat("minsites");
  min_nsites = (double) ajminsites;
  
  w = ajAcdGetInt("w");
  min_w = ajAcdGetInt("minw");
  max_w = ajAcdGetInt("maxw");
  nmotifs = ajAcdGetInt("nmotifs");
  
/*  seqallm = ajAcdGetSeqall("mfile");*/


  ajprior = ajAcdGetList("prior");
  prior = ajStrStr(*ajprior);

  brief = ajAcdGetBool("brief");
  if(brief)
      DOC=FALSE;
  else
      DOC=TRUE;
  
  beta = (double) ajAcdGetFloat("b");

  ajspmap = ajAcdGetList("spmap");
  MAP = ajStrStr(*ajspmap);
  
  map_scale = (double) ajAcdGetFloat("spfuzz");
  
  maxiter = ajAcdGetInt("maxiter");
  
  distance = (double)ajAcdGetFloat("distance");
  
  ajcons = ajAcdGetString("cons");
  spcons[n_spcons++] = ajStrStr(ajcons);

  chi_alpha = (double)ajAcdGetFloat("chi");

  maxsize = ajAcdGetInt("maxsize");
  
  PAGEWIDTH = ajAcdGetInt("page");

  ajstatus = ajAcdGetBool("status");
  if(ajstatus)
      NO_STATUS = FALSE;
  else
      NO_STATUS = TRUE;

  VERBOSE = ajAcdGetBool("V");

  prob = (double) ajAcdGetFloat("prob");

  strands[2] = ajAcdGetBool("cfive");
  strands[3] = ajAcdGetBool("cthree");
  strands[1] = ajAcdGetBool("wthree");

  seed = ajAcdGetInt("seed");

  seqfrac = (double) ajAcdGetFloat("seqfrac");

  ajalign = ajAcdGetBool("align");
  if(ajalign)
      NOALIGN = FALSE;
  else
      NOALIGN = TRUE;

  TRACE = ajAcdGetBool("trace");

  PRINTALL = ajAcdGetBool("allprint");

  PRINT_W  = ajAcdGetBool("wprint");

  PRINT_Z  = ajAcdGetBool("zprint");

  PRINT_LL = ajAcdGetBool("llprint");

  PRINT_STARTS = ajAcdGetBool("startsprint");

  PRINT_FASTA  = ajAcdGetBool("fastaprint");

  TIMER = ajAcdGetInt("timer");
  
  outf = ajAcdGetOutfile("outfile");

  /* get the command line arguments */
/*  i = 1; */
#ifndef lint
  /* print the command line */
/*  argv[0] = "";
  DO_STANDARD_COMMAND_LINE(1,
    NON_SWITCH(1, <datafile> \t\tfile containing sequences in FASTA format\n,
      switch (i++) {
        case 1: datafile = _OPTION_; break;
        default: COMMAND_LINE_ERROR;
      });
     DATA_OPTN(1, neg, <negdataset>, \tfile containing negative example sequences, 
       negfile = _OPTION_);
     DATA_OPTN(1, ntype, pair|blend, \thow to use negative examples, 
       ntype = _OPTION_);
     FLAG_OPTN(1, protein, \t\tassume sequences use IUPAC protein alphabet, 
       alph = "PROTEIN");
     FLAG_OPTN(1, dna, \t\t\tassume sequences use DNA alphabet, alph = "DNA");
     DATA_OPTN(1, alph, <alphabet>, \ta string of letters in quotes,
       alph = _OPTION_);
     DATA_OPTN(1, mod, oops|zoops|tcm, \tmotif distribution, mod = _OPTION_);
     FLAG_OPTN(1, pal, \t\t\t(DNA) allow palindromes, pal = 1);
     FLAG_OPTN(1, pal_only, \t\t(DNA) force palindromes, pal = 2);
     FLAG_OPTN(1, noshorten, \t\tdo not allow motifs shorter than <minw>, 
       shortn = FALSE);
     DATA_OPTN(1, nsites, <nsites>, \texpected number of sites for each motif, 
       nsites=atof(_OPTION_));
     DATA_OPTN(1, 
        minsites, <minsites>, 
        \tminimum number of sites for each motif, min_nsites=atof(_OPTION_));
     DATA_OPTN(1, 
        maxsites, <maxsites>, \tmaximum number of sites for each motif,
	max_nsites=atof(_OPTION_));
     DATA_OPTN(1, w, <w>, \t\tstarting motif width to try, w = atoi(_OPTION_));
     DATA_OPTN(1, minw, <minw>, \t\tminumum starting motif width to try,
       min_w = atoi(_OPTION_));
     DATA_OPTN(1, maxw, <maxw>, \t\tmaximum starting motif width to try,
       max_w = atoi(_OPTION_));
     DATA_OPTN(1, nmotifs, <nmotifs>, \tmaximum number of motifs to find, 
       nmotifs = atoi(_OPTION_));
     DATA_OPTN(EXP, mfile, <mfile>, \tfile of known motifs, mfile = _OPTION_);
     DATA_OPTN(1, 
       prior, dirichlet|dmix|mega|megap|addone, \n\t\t\t\ttype of prior to use, 
       prior = _OPTION_);
     FLAG_OPTN(1,
      brief, \t\tbrief output--do not print documentation, DOC = FALSE);
     DATA_OPTN(1, b, <b>, \t\tstrength of the prior, beta = atof(_OPTION_));
     DATA_OPTN(1, spmap, ic|pam, \t\tstarting point seq to theta mapping type,
       MAP = _OPTION_);
     DATA_OPTN(1, spfuzz, <spfuzz>, \tfuzziness of sequence to theta mapping, 
	map_scale = atof(_OPTION_));
     DATA_OPTN(1, maxiter, <maxiter>, \tmaximum EM iterations to run, 
	maxiter = atoi(_OPTION_));
     DATA_OPTN(1, distance, <distance>, \tEM convergence criterion, 
       distance = atof(_OPTION_));
     DATA_OPTN(1, cons, <cons>, \t\tconsensus sequence to start EM from, 
       spcons[n_spcons++] = _OPTION_);
     DATA_OPTN(1, chi, <p>, \t\tp-value cutoff, chi_alpha=atof(_OPTION_));
     DATA_OPTN(1, adj, none|bon|root, \ttype of LRT adjustment, 
       adj = _OPTION_);
     DATA_OPTN(1, maxsize, <maxsize>, \tmaximum dataset size in characters,
       maxsize = atoi(_OPTION_));
     DATA_OPTN(0, page, <page>, width of page, PAGEWIDTH = atoi(_OPTION_));
     FLAG_OPTN(1, nostatus, \t\tdo not print progress reports, 
       NO_STATUS = TRUE);
     DATA_OPTN(PARAFLAG, parallel, <level>, type of parallelization,
       PARA = atoi(_OPTION_));
     FLAG_OPTN(EXP, V, \t\t\tverbose mode, VERBOSE = TRUE);
     DATA_OPTN(EXP, plib, <plib>, \t\tname of dirichlet prior file,
       plib_name = _OPTION_);
     DATA_OPTN(EXP, prob, <prob>, \t\tstarting point confidence level, 
       prob = atof(_OPTION_));
     FLAG_OPTN(1, c53, \t\t\t(DNA) use 5' to 3' complementary strand as well,
       strands[2] = TRUE);
     FLAG_OPTN(1, c35, \t\t\t(DNA) use 3' to 5' complementary strand as well,
       strands[3] = TRUE);
     FLAG_OPTN(1, w35, \t\t\t(DNA) use 3' to 5' main strand as well,
      strands[1] = TRUE);
     FLAG_OPTN(EXP, invcomp, \t\tuse inverse complement as strand as well,
       strands[2] = TRUE);
     DATA_OPTN(EXP, seed, <seed>, \t\tseed for random numbers in sampling, 
       seed = atoi(_OPTION_));
     DATA_OPTN(EXP, seqfrac, <seqfrac>, \tfraction of sequences to use,
	seqfrac= atof(_OPTION_));
     FLAG_OPTN(EXP, noalign, \t\tdo not print aligned motif occurrences, 
       NOALIGN = TRUE);
     FLAG_OPTN(EXP, trace, \t\ttrace starting points, TRACE = TRUE);
     FLAG_OPTN(EXP, print_all, \t\tprint all debug information, 
       PRINTALL = TRUE);
     FLAG_OPTN(EXP, print_w, \t\tprint erasure matrix, PRINT_W = TRUE);
     FLAG_OPTN(EXP, print_z, \t\tprint missing information matrix, 
       PRINT_Z = TRUE);
     FLAG_OPTN(EXP, print_ll, \t\tprint log-likelihood during EM, 
       PRINT_LL = TRUE);
     FLAG_OPTN(EXP, print_starts, \t\tprint starting points, 
       PRINT_STARTS = TRUE);
     FLAG_OPTN(1, print_fasta, \t\tprint sites in FASTA format (default BLOCKS),
       PRINT_FASTA = TRUE);
     DATA_OPTN(EXP, timer, <type>, \t\tshow loop times, TIMER = atoi(_OPTION_));
  )
*/
#endif

  /* announce meme */
  banner("MEME");

  /* set random number generator */
  srand48(seed);

  /* set all the print flags appropriately */
  if (PRINTALL) {
    PRINT_W = TRUE;
    PRINT_Z = TRUE;
    PRINT_LL = TRUE;
    PRINT_STARTS = TRUE;
  }

  /* check input arguments */
  if (prob < 0 || prob > 1) {
    printf("-prob <p>, <p> must be between 0 and 1\n");
    exit(1);
  }

  if (nmotifs >= MAXG) {
    printf("-nmotifs larger than MAXG-1.  Use smaller -nmotifs or recompile with larger MAXG.\n");
   exit(1);
  }

  /* get the name of the MEME directory */
/*  meme_directory = getenv("MEME_DIRECTORY");
  if (meme_directory == NULL) {
      printf("You must define environment variable MEME_DIRECTORY\n");
      exit(1);
  }*/

  /* get the model type */
  if (!strcmp(mod, "tcm")) {
    mtype = Tcm;
  } else if (!strcmp(mod, "oops")) {
    mtype = Oops;
  } else if (!strcmp(mod, "zoops")) {
    mtype = Zoops;
  } else {
    printf("Unknown model type %s. \n", mod);
    exit(1);
  }

  /* see if alphabet is one of standards; otherwise use
	the string alph as the set of letters in the alphabet
  */
  if (!strcmp(alph, "DNA")) {
    alphabet = DNA;
    if (!MAP) MAP = "uni";			/* uniform prior mapping */
    if (!prior) prior = "dirichlet";		/* simple dirichlet prior */
  } else if (!strcmp(alph, "PROTEIN")) {
    alphabet = PROTEIN;
    if (!MAP) MAP = "pam";			/* PAM mapping */
    if (!prior) {
      switch (mtype) {
	case Oops:
	  prior = "dmix";	break;
	case Zoops:
	  prior = "megap"; break;
	case Tcm:
	  prior = "mega"; break;
	default:
	  prior = "dirichlet"; break;
      }
    }
  } else {
    alphabet = alph;
    if (!MAP) MAP = "pam";			/* PAM mapping */
    if (!prior) prior = "dirichlet";		/* simple dirichlet prior */
  }

#define ILLEGAL_PRIOR(type) {\
  printf("\nThe %s prior is only supported for the protein alphabet!\n", type);\
  exit(1); \
}

  /* find out type of prior */
  if (!strcmp(prior, "dirichlet")) {
    ptype = Dirichlet;
    if (beta < 0) beta = 1;			/* default b = 1 */
  } else if (!strcmp(prior, "dmix")) { 
    ptype = Dmix;
    if (alphabet != PROTEIN) ILLEGAL_PRIOR(prior);
    if (beta < 0) beta = 0;			/* default b = 0 for dmix */
  } else if (!strcmp(prior, "megadmix") || !strcmp(prior, "mega")) { 
    ptype = Mega;				/* use mega prior heuristic */
    if (alphabet != PROTEIN) ILLEGAL_PRIOR(prior);
  } else if (!strcmp(prior, "megap")) { 
    ptype = MegaP;				/* last iteration use b=0 */
    if (alphabet != PROTEIN) ILLEGAL_PRIOR(prior);
  } else if (!strcmp(prior, "addone")) {
    ptype = Addone;
  } else {
    printf("Unknown type of prior: %s!\n", prior);
    exit(1);
  }

  /* get the alphabet from the mixture prior library file if using */ 
  if (ptype == Dmix || ptype == Mega || ptype == MegaP) {
    int s1, s2;
    char *tmp;
    /* make the name of the prior library */
    if (!plib_name) plib_name = PLIB;		/* default mixture prior */
/*    s1 = strlen(meme_directory);
    s2 = strlen(plib_name);
    tmp = (char *) malloc((s1 + s2 + 2) * sizeof(char));
    strcpy(tmp, meme_directory);*/
/*    tmp[s1] = '/';	*/	/* separate directory name from file name */
/*    strcpy(tmp+s1+1, plib_name);
    plib_name = tmp;*/ 
    *priors_p = create_priors(Dmix, -1, NULL, plib_name);
    alphabet = (*priors_p)->plib->alphabet;
  }

  /* convert the alphabet to upper case */
  for (i=0; alphabet && (cc = alphabet[i]) != (int)NULL; i++)
    if (islower(cc)) alphabet[i]=toupper(cc);

  /* setup hashing function for encoding strings as integers */
  alength = setup_hash_alph(alphabet);

  /* read the samples and set up globals */
  use_comp = strands[2] | strands[3];
  if (seqfrac > 1 || seqfrac <=0) {
    printf("seqfrac must be in (0, 1]\n");
    exit(1);
  }
  *dataset_p = read_seq_file(seqall, alphabet, use_comp, seqfrac);
  dataset = *dataset_p;

  /* prevent too long jobs */
  if (dataset->total_res > maxsize) {
    printf("Dataset too large (> %d).  Rerun with larger -maxsize.\n",
      maxsize);
    exit(1);
  }

#ifdef PARALLEL
  if ((PARA == 1) || (PARA == 3)) {
    /* Allocate storage for the sequence parameters. */
    start_n_end = (SEQ_PARAMS *)malloc(sizeof(SEQ_PARAMS));
    /* Calculate optimal split points for this dataset. */
    balance_loop1(dataset->samples, start_n_end, dataset->n_samples);
  }
#endif

  /* read in known motifs file */
  if (mfile) {
/*    dataset->nmotifs = read_motifs (seqallm, "motifs", dataset->motifs, FALSE, NULL);*/
    nmotifs = dataset->nmotifs;
    shortn = FALSE;			/* don't shorten the motif */
    prob = 0;				/* don't sample starts */
    /*maxiter = 1;*/			/* only one iteration to set \theta */
    dataset->pal = 0;			/* no palindrome testing */
  } else {
    dataset->nmotifs = 0;
  }

  /* get the LRT adjustment type */
  if (!strcmp(adj, "none")) {		/* no adjustment */
    dataset->adj = None;
  } else if (!strcmp(adj, "bon")) {	/* Bonferroni adjustment */
    dataset->adj = Bon;
  } else if (!strcmp(adj, "root")) {	/* n-th root adjustment */
    dataset->adj = Root;
  } else {
    printf("Unknown LRT adjustment type %s. \n", adj);
    exit(1);
  }

  /*
     Non-redundant database letter frequencies for logodds matrices
     and dirichlet prior.
  */
  {
    double tot = 0;
    /* create the array of nrdb frequencies */
    dataset->nrdb_freq = NULL;  
    Resize(dataset->nrdb_freq, alength, double);
    if (!strcmp(alphabet, DNA)) {		/* standard 4-letter DNA */
      /* frequencies for NT 9/22/96 */
      dataset->nrdb_freq[ 0] =  0.281475655; /* A */
      dataset->nrdb_freq[ 1] =  0.221785822; /* C */
      dataset->nrdb_freq[ 2] =  0.228634607; /* G */
      dataset->nrdb_freq[ 3] =  0.267048106; /* T */
    } else if (!strcmp(alphabet, PROTEIN)) {	/* standard 20-letter protein */
      /* frequencies for NR 9/22/96 */
      dataset->nrdb_freq[ 0] =  0.073091885; /* A */
      dataset->nrdb_freq[ 1] =  0.018145453; /* C */
      dataset->nrdb_freq[ 2] =  0.051687956; /* D */
      dataset->nrdb_freq[ 3] =  0.062278511; /* E */
      dataset->nrdb_freq[ 4] =  0.040243411; /* F */
      dataset->nrdb_freq[ 5] =  0.069259642; /* G */
      dataset->nrdb_freq[ 6] =  0.022405456; /* H */
      dataset->nrdb_freq[ 7] =  0.056227000; /* I */
      dataset->nrdb_freq[ 8] =  0.058435042; /* K */
      dataset->nrdb_freq[ 9] =  0.091621836; /* L */
      dataset->nrdb_freq[10] =  0.023044274; /* M */
      dataset->nrdb_freq[11] =  0.046032137; /* N */
      dataset->nrdb_freq[12] =  0.050623807; /* P */
      dataset->nrdb_freq[13] =  0.040715284; /* Q */
      dataset->nrdb_freq[14] =  0.051846246; /* R */
      dataset->nrdb_freq[15] =  0.073729031; /* S */
      dataset->nrdb_freq[16] =  0.059352333; /* T */
      dataset->nrdb_freq[17] =  0.064298546; /* V */
      dataset->nrdb_freq[18] =  0.013328158; /* W */
      dataset->nrdb_freq[19] =  0.032649745; /* Y */
    } else {					/* unknown alphabet */
      for (i=0; i<alength; i++) dataset->nrdb_freq[i] = 1.0/alength;
    }
    /* make sure background frequencies sum to 1.0 */
    for (i=0; i<alength; i++) tot += dataset->nrdb_freq[i];
    for (i=0; i<alength; i++) dataset->nrdb_freq[i] /= tot; 
  }

  /* create the priors */
  if (ptype == Mega || ptype == MegaP) {
    /* tlb 5-9-97; wgt_total_res */
    beta = 10.0 * dataset->wgt_total_res;	/* size of mega prior */
  }
  *priors_p = create_priors(ptype, beta, dataset, plib_name);

  /* adjust the default model frequencies by the prior */
  {
    double *p_count = (*priors_p)->prior_count; 	/* pseudo counts */
    double *res_freq = dataset->res_freq;
    double *adj_freq = dataset->adj_freq;
    /* tlb 5-9-97; wgt_total_res */
    double wgt_total_res = dataset->wgt_total_res;
    double total = 0;

    /* calculate the counts of each letter */
    for (i=0; i<alength; i++) {
      adj_freq[i] = wgt_total_res * res_freq[i];
    }
      
    /* adjust by the prior */
    if (ptype == Dmix) {
      mixture_regularizer(adj_freq, (*priors_p)->plib, p_count); 
    } else if (ptype == Mega || ptype == MegaP) {
      mixture_regularizer(adj_freq, (*priors_p)->plib0, p_count); 
    }
    for (i=0; i<alength; i++) {
      total += adj_freq[i] += p_count[i];
    }
    dataset->adj_entropy = 0;
    for (i=0; i<alength; i++) {
      adj_freq[i] /= total;
      dataset->adj_entropy += adj_freq[i] * LOG2(adj_freq[i]);
    }

    if (VERBOSE) {
      printf("\n\n");
      printf ("\nAdjusted Letter frequencies:\n");
      for (i=0, pcol=0; i<dataset->alength; i++) {
	pcol += 8;          /* start of next printed thing */ 
        if (pcol >= PAGEWIDTH) {pcol=8; printf("\n");} 
        printf("%c %5.3f ", alphabet[i], dataset->res_freq[i]); 
      } 
      printf("\n\n"); 
      printf("Adjusted entropy of dataset (bits) = %f\n\n", 
        dataset->adj_entropy); 
    }
  }

  /* set search range for nsites */
  if (mtype == Oops) {
    min_nsites = max_nsites = dataset->n_samples; 
  } else if (mtype == Zoops) {
    max_nsites = dataset->n_samples;
  }

  /* check that no sequence too short */
  if (dataset->min_slength <= MIN_W) {
    printf("All sequences must be at least %d characters long.  Remove ",
      MIN_W+1);
    printf("shorter sequences\nand rerun.\n");
    exit(1);
  }
 
  /* set up globals */
  if (w != 0) {				/* w specified; set min_w and max_w */
    max_w = min_w = w;
  }

  if (max_w >= dataset->min_slength) {
    max_w = dataset->min_slength-1;
    printf("maxw >= length of shortest sequence (%d).", dataset->min_slength);
    printf("  Setting maxw to %d.\n", max_w);
  }
  if (max_w > MAXSITE) {
    printf("maxw too large (> %d).  Recompile with larger MAXSITE.\n", MAXSITE);
    exit(1);
  }
  if (max_w < 0) {					/* use default */
    max_w = MIN(MAXSITE, dataset->min_slength-1);	/* maximum W0 */
  }

  /* check that min_w <= max_w */
  if (min_w > max_w) {
     printf("minw > maxw.  Setting minw to %d.\n", max_w);
     min_w = max_w;
  }
 
  /* check that min_w and max_w are at least MIN_W */
  if (min_w < MIN_W) {
    printf("Minimum width must be >= %d.  Respecify larger -w or -minw.\n", 
      MIN_W);
    exit(1);
  } else if (max_w < MIN_W) {
    printf("Maximum width must be >= %d.  Respecify larger -w or -maxw.\n", 
      MIN_W);
    exit(1);
  }

  /* set number of occurrences of sites */
  if (nsites != 0) {
    min_nsites = max_nsites = nsites;
  }

  /* other strands only supported for oops model */
  if (mtype != Oops && (strands[1] | strands[2] | strands[3])) {
    printf("Strands w35, c53 and c35 are only supported for oops model!\n");
    exit(1);
  }

  /* must use TCM if only one sequence */
  if (mtype != Tcm && dataset->n_samples==1) {
    printf("You must specify '-mod tcm' since your dataset contains only one sequence.\n");
    printf("Alternatively, you might wish to break your sequence into several sequences.\n");
    exit(1);
  }
  
  /* check that using right alphabet if model type requires it */
  if (strcmp(alph, "DNA")) {
    if (use_comp) {
      printf(
"You must use default DNA alphabet if using complementary strands!\n");
      exit(1);
    }
    if (pal) {
      printf("You must use default DNA alphabet");
      printf(" if using -pal or -pal_only!\n");
      exit(1);
    }
  }

  /* flag search for palindromes */
  dataset->pal = pal;

  /* get the type of mapping between sequences and thetas */
  if (!strcmp(MAP, "uni")) {
    map_type = Uni; 
    if (map_scale == -1) map_scale = .5;		/* default add .5 */
  } else if (!strcmp(MAP, "pam")) {
    map_type = Pam; 
    if (map_scale == -1) map_scale = 120;		/* default PAM 120 */
  } else {
    printf("Unknown mapping type %s. \n", MAP);
    exit(1);
  }

  /* check that IUPAC alphabet if using PAM mapping */
  if (MAP == "pam" && strcmp(alphabet, PROTEIN)) {
    printf("Setting sequence to theta mapping type to `Uni' since alphabet not IUPAC\n");
    MAP = "uni";
  }

  /* set up the sequence to theta mapping matrix for starts */
  dataset->map = init_map(map_type, map_scale, alength);
  *map_p = dataset->map;

  /* set up p_point: previously learned components start points */
  *p_point_p = (P_POINT *) malloc(sizeof(P_POINT));
  p_point = *p_point_p;
  p_point->c = n_spcons;		/* number of specified starts */
  /* the default starting points */
  for (i=0; i<MAXG; i++) {
    p_point->e_cons0[i] = NULL;
    p_point->w[i] = max_w;
    p_point->nsites[i] = nsites;
  }
  /* the user specified starting points */
  p_point->w[0] = 1;			 /* background start point */ 
  /* set up background and user-specified start points */
  for (i=1; i<n_spcons; i++) {
    char *e_cons = (char *) malloc(MAXSITE);		/* encoded version */
    if (i >= MAXG) {
      printf("Too many starting points.  Increase MAXG and recompile.");
      exit(1);
    }
    p_point->e_cons0[i] = e_cons;
    /* convert the consensus sequence to upper case and encode as integer */
    for (j=0; (cc = spcons[i][j]) != (int)NULL; j++) {
      char uc = (islower(cc) ? toupper(cc) : cc); 
      e_cons[j] = (uc == 'X') ? alength : hash(uc);
      if (e_cons[j] > alength) {
	printf("Illegal letter %c in consensus string!\n", cc);
	exit(1);
      }
    }
    /* set width to length of consensus unless -w specified */
    if (w == 0) p_point->w[i] = j;
    /* pad out the consensus sequence with A's */
    for ( ; j < MAXSITE; j++) e_cons[j] = 0;
  }

  /* make sure at least one motif for each specified starting point */
  if (nmotifs < n_spcons - 1) {
    nmotifs = n_spcons - 1;
    printf("Setting nmotifs to %d\n", nmotifs);
  }

  /* create the model */
  *model_p = create_model(mtype, strands, p_point, nmotifs, alength);
  *best_model_p = create_model(mtype, strands, p_point, nmotifs, alength);

  /* create the negative dataset and negative model if negative file given */
  if (negfile) {
    *neg_dataset_p = read_seq_file(seqalln, dataset->alphabet, 
      use_comp, seqfrac);
    if (!strcmp(ntype, "pair")) {
      (*neg_dataset_p)->negtype = Pair; 
    } else if (!strcmp(ntype, "blend")) {
      (*neg_dataset_p)->negtype = Blend;
    } else {
      printf("Unknown ntype %s. \n", ntype);
      exit(1);
    }
    (*neg_dataset_p)->pal = dataset->pal;
    (*neg_dataset_p)->nrdb_freq = dataset->nrdb_freq;
    *neg_model_p = create_model(mtype, strands, p_point, nmotifs, alength);
  } else {
    *neg_dataset_p = NULL;
    *neg_model_p = NULL;
  }

  /* initialize log table */
  init_log();

  /*printf("Consensus: all letters with at least %2.1f probability\n", MINCONS);
  */

  /* create a temporary file to hold this info */
  debug_file = split(0, debug_file, i);

  /* print the debug info to the file */
  fprintf(debug_file,
"This information can also be useful in the event you wish to report a\n"
"problem with the MEME software.\n\n"
"model:  mod=      %8s    nmotifs=  %8d    chi=      %8g\n"
"width:  minw=     %8d    maxw=     %8d    shorten=  %8s\n"
"lambda: minsites= %8g    maxsites= %8g\n"
"theta:  prob=     %8g    spmap=    %8s    spfuzz=   %8g\n" 
"em:     prior=   %9s    b=        %8g    maxiter=  %8d\n"
"        distance= %8g\n"
"data:   n=        %8d    N=        %8d\n", 
    mod, nmotifs, chi_alpha,
    min_w, max_w, yesno[shortn],
    min_nsites, max_nsites,
    prob, MAP, map_scale,
    prior, beta, maxiter,
    distance,
    dataset->total_res, dataset->n_samples
  );
  fprintf(debug_file, "strands:");
  for (i=0; i<4; i++) if (strands[i]) fprintf(debug_file, "%4s", dir[i]);
  fprintf(debug_file,
"\n"
"sample: seed=     %8d    seqfrac=  %8g\n"
"LRT:    adj=      %8s\n",
    seed, seqfrac,
    adj
  );
  if (plib_name) {
    char *name = plib_name + strlen(plib_name);
    while (*name != '/') name--; name++;	/* strip off directory */
    fprintf(debug_file, "Dirichlet mixture priors file: %s\n", name);
  }
  /* print dataset frequencies of letters in alphabet */
  fprintf(debug_file, "Letter frequencies:\n");
  for (i=0, pcol=0; i<dataset->alength; i++) {
    pcol += 8;          /* start of next printed thing */
    if (pcol >= PAGEWIDTH) {pcol=8; fprintf(debug_file, "\n");}
    fprintf(debug_file, "%c %5.3f ", alphabet[i], dataset->res_freq[i]);
  }
  /* print non-redundant database frequencies of letters in alphabet */
  fprintf(debug_file, "\nNon-redundant database letter frequencies:\n");
  for (i=0, pcol=0; i<dataset->alength; i++) {
    pcol += 8;          /* start of next printed thing */
    if (pcol >= PAGEWIDTH) {pcol=8; fprintf(debug_file, "\n");}
    fprintf(debug_file, "%c %5.3f ", alphabet[i], dataset->nrdb_freq[i]);
  }
  fprintf(debug_file, 
"\nEffective length of alphabet = %d\n"
"Entropy of dataset (bits) = %8.3g\n\n",
    dataset->effective_alength, dataset->entropy);
  /* print the command line */
  argv[0] = "meme";
  for (i=0; i<argc; i++) fprintf(debug_file, "%s ", argv[i]);
  fprintf(debug_file, "\n");

  /* describe the dataset */
  print_dataset_summary (datafile, alphabet, dataset, negfile);

  /* print explanation of how to interpret the results */
  if (DOC) print_meme_doc();

  /* set up return values */
  *min_nsites_p = min_nsites;
  *max_nsites_p = max_nsites;
  *min_w_p = min_w;
  *max_w_p = max_w;
  *shortn_p = shortn;
  *distance_p = distance;
  *prob_p = prob;
  *nmotifs_p = nmotifs;
  *maxiter_p = maxiter;
  *chi_alpha_p = chi_alpha;
  *debug_file_p = debug_file;
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/init.c,v 1.2 2003/10/06 09:03:21 rice Exp $ */
