#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1986-2002 by the University of Washington
  and by Joseph Felsenstein.  Written by Joseph Felsenstein.  Permission is
  granted to copy and use this program provided no fee is charged for it
  and provided that this copyright notice is not removed. */

#define epsilon         0.0001   /* used in makenewv, getthree, update */
#define over            60

typedef struct valrec {
  double rat, ratxi, ratxv, orig_zz, z1, y1, z1zz, z1yy, xiz1,
         xiy1xv;
  double *ww, *zz, *wwzz, *vvzz; 
} valrec;

typedef double contribarr[maxcategs];

extern sequence y;
valrec ***tbl;

AjPSeqset* seqsets = NULL;
AjPPhyloProp phyloratecat = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloTree* phylotrees;

ajint numseqs;
ajint numwts;

#ifndef OLDC
/* function prototypes */
//void   getoptions(void);
void   emboss_getoptions(char *pgm, int argc, char *argv[]);
void   initmemrates(); 
void   allocrest(void);
void   doinit(void);
void   inputoptions(void);
void   makeweights(void);
void   getinput(void);
void   inittable_for_usertree (char *);
void   inittable(void);
void   exmake(double, long);
void   alloc_nvd(long, nuview_data *);
void   free_nvd(nuview_data *);
void   nuview(node *);
double evaluate(node *);
void   getthree(node *p, double thigh, double tlow);
void   makenewv(node *);
void   update(node *);
void   smooth(node *);
void   restoradd(node *, node *, node *, double);
void   dnamlk_add(node *, node *, node *);
void   dnamlk_re_move(node **, node **, boolean);
void   tryadd(node *, node **, node **);
void   addpreorder(node *, node *, node *, boolean, boolean);
void   tryrearr(node *, boolean *);
void   repreorder(node *, boolean *);
void   rearrange(node **);
void   initdnamlnode(node **, node **, node *, long, long, long *, long *,
                initops, pointarray, pointarray, Char *, Char *, char **);
void   tymetrav(node *, double *);
void   dnamlk_coordinates(node *, long *);
void   dnamlk_drawline(long, double);
void   dnamlk_printree(void);
void   describe(node *);
void   reconstr(node *, long);
void   rectrav(node *, long, long);
void   summarize(void);
void   dnamlk_treeout(node *);
void   nodeinit(node *);
void   initrav(node *);
void   travinit(node *);
void   travsp(node *);
void   treevaluate(void);
void   maketree(void);
void reallocsites(void);
void save_tree_tyme(tree* save_tree, double tymes[]);
void restore_saved_tyme(tree *load_tree, double tymes[]);
/* function prototypes */
#endif


Char infilename[FNMLNGTH],  intreename[FNMLNGTH], catfilename[FNMLNGTH], weightfilename[FNMLNGTH];

const char* outfilename;
const char* outtreename;
AjPFile embossoutfile;
AjPFile embossouttree;
double *rrate;
long sites, weightsum, categs, datasets, ith, njumble, jumb, numtrees, shimotrees;
/*  sites = number of sites in actual sequences
  numtrees = number of user-defined trees */
long inseed, inseed0, mx, mx0, mx1;
boolean freqsfrom, global, global2=0, jumble, lngths, trout, usertree, weights, 
          rctgry, ctgry, ttr, auto_, progress, mulsets, firstset, hypstate, 
          smoothit, polishing, justwts, gama, invar;
tree curtree, bestree, bestree2, priortree;
node *qwhere, *grbg;
double *tymes;
double xi, xv, ttratio, ttratio0, freqa, freqc, freqg, freqt, freqr,
              freqy, freqar, freqcy, freqgr, freqty, fracchange, sumrates,
              cv, alpha, lambda, lambda1, invarfrac;
long *enterorder;
steptr aliasweight;
double *rate;
double **term, **slopeterm, **curveterm;
longer seed;
double *probcat;
long iprobcat;
contribarr *contribution;
char *progname;
long rcategs, nonodes2;
long **mp;
char basechar[16]="acmgrsvtwyhkdbn";


/* Local variables for maketree, propagated globally for C version: */
long    k, maxwhich, col;
double  like, bestyet, tdelta, lnlike, slope, curv, maxlogl;
boolean lastsp, smoothed, succeeded;
double  *l0gl;
double  x[3], lnl[3];
double  expon1i[maxcategs], expon1v[maxcategs],
        expon2i[maxcategs], expon2v[maxcategs];
node   *there;
double  **l0gf;
Char ch, ch2;


void save_tree_tyme(tree* save_tree, double tymes[])
{
  int i;
  for ( i = spp ; i < nonodes ; i++) {
    tymes[i - spp] = save_tree->nodep[i]->tyme;
  }
}

void restore_saved_tyme(tree *load_tree, double tymes[])
{
  int i;
  for ( i = spp ; i < nonodes ; i++) {
    load_tree->nodep[i]->tyme = tymes[i - spp];
  }
}

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjPStr gammamethod = NULL;
    ajint i;
    AjPFloat basefreq;
    AjPFloat hmmrates;
    AjPFloat hmmprob;
    AjPFloat arrayval;

  double probsum=0.0;
  auto_ = false;
  ctgry = false;
  rctgry = false;
  categs = 1;
  rcategs = 1;
  freqsfrom = true;
  gama = false;
  invar = false;
  global = false;
  hypstate = false;
  jumble = false;
  njumble = 1;
  lambda = 1.0;
  lambda1 = 0.0;
  lngths = false;
  trout = true;
  ttratio = 2.0;
  ttr = false;
  usertree = false;
  weights = false;
  printdata = false;
  progress = true;
  treeprint = true;
  interleaved = true;
  datasets = 1;
  mulsets = false;

    embInitP (pgm, argc, argv, "PHYLIPNEW");

    seqsets = ajAcdGetSeqsetall("sequence");

    numseqs = 0;
    while (seqsets[numseqs])
	numseqs++;

    phylotrees = ajAcdGetTree("intreefile");
    if (phylotrees)
    {
        numtrees = 0;
        while (phylotrees[numtrees])
            numtrees++;
        usertree = true;
        lngths = ajAcdGetBool("lengths");
    }
    numwts = 0;

    phyloweights = ajAcdGetProperties("weights");
    if (phyloweights)
    {
      weights = true;
      numwts = ajPhyloPropGetSize(phyloweights);
    }

    if (numseqs > 1) {
      mulsets = true;
      datasets = numseqs;
    }
    else if (numwts > 1) {
      mulsets = true;
      datasets = numwts;
      justwts = true;
    }

   categs = ajAcdGetInt("ncategories"); 

    if (categs > 1) {
      ctgry = true;
      rate = (double *) Malloc(categs * sizeof(double));
      arrayval = ajAcdGetArray("rate");
      emboss_initcategs(arrayval, categs, rate);
    }
    else{
      rate    = (double *) Malloc(categs*sizeof(double));
      rate[0] = 1.0;
    }     



    phyloratecat = ajAcdGetProperties("categories");      

    gammamethod = ajAcdGetListI("gamma", 1);

    if(ajStrMatchC(gammamethod, "n")) {
      rrate      = (double *) Malloc(rcategs*sizeof(double));
      probcat    = (double *) Malloc(rcategs*sizeof(double));
      iprobcat = rcategs;
      rrate[0]   = 1.0;
      probcat[0] = 1.0;
    }     
    else {
      rctgry = true;
      auto_ = ajAcdGetBool("adjsite"); 
      if(auto_)  {
         lambda = ajAcdGetFloat("lambda");
         lambda = 1 / lambda;
         lambda1 = 1.0 - lambda;
      }
    }        



    if(ajStrMatchC(gammamethod, "g")) {
      gama = true; 
      rcategs = ajAcdGetInt("ngammacat");
      cv = ajAcdGetFloat("gammacoefficient");
      alpha = 1.0 / (cv*cv);
      initmemrates();
      initgammacat(rcategs, alpha, rrate, probcat);
    }
    else if(ajStrMatchC(gammamethod, "i")) {
      invar = true;
      rcategs = ajAcdGetInt("ninvarcat");
      cv = ajAcdGetFloat("invarcoefficient");
      alpha = 1.0 / (cv*cv);
      invarfrac = ajAcdGetFloat("invarfrac");
      initmemrates();
      initgammacat(rcategs-1, alpha, rrate, probcat);
      for (i=0; i < rcategs-1 ; i++)
         probcat[i] = probcat[i]*(1.0-invarfrac);
      probcat[rcategs-1] = invarfrac;
      rrate[rcategs-1] = 0.0;
    }
    else if(ajStrMatchC(gammamethod, "h")) {
      rcategs = ajAcdGetInt("nhmmcategories"); 
      initmemrates();
      hmmrates = ajAcdGetArray("hmmrates");
      emboss_initcategs(hmmrates, rcategs,rrate);      
      hmmprob = ajAcdGetArray("hmmprobabilities");
      for (i=0; i < rcategs; i++){
	 probcat[i] = ajFloatGet(hmmprob, i);
         probsum += probcat[i];
      }
    }    

    ttratio = ajAcdGetFloat("ttratio");
 
    if(!usertree) {
      global = ajAcdGetBool("global"); 
      njumble = ajAcdGetInt("njumble");
      if(njumble >0) {
        inseed = ajAcdGetInt("seed");
        jumble = true; 
        emboss_initseed(inseed, &inseed0, seed);
      }
      else njumble = 1;
    }

    if((mulsets) && (!jumble)) {
      jumble = true;
      inseed = ajAcdGetInt("seed");
      emboss_initseed(inseed, &inseed0, seed);
    }

    printdata = ajAcdGetBool("printdata");
    progress = ajAcdGetBool("progress");
    treeprint = ajAcdGetBool("treeprint");
    trout = ajAcdGetToggle("trout");
    hypstate = ajAcdGetBool("hypstate");

    freqsfrom = ajAcdGetToggle("freqsfrom");
    if(!freqsfrom) {
      basefreq = ajAcdGetArray("basefreq"); 
      freqa = ajFloatGet(basefreq, 0);
      freqc = ajFloatGet(basefreq, 1);
      freqg = ajFloatGet(basefreq, 2);
      freqt = ajFloatGet(basefreq, 3);
    }

     embossoutfile = ajAcdGetOutfile("outfile");   
     emboss_openfile(embossoutfile, &outfile, &outfilename);
     
     
     if(trout) {
       embossouttree = ajAcdGetOutfile("outtreefile");
       emboss_openfile(embossouttree, &outtree, &outtreename);
     }

    fprintf(outfile, "\nNucleic acid sequence Maximum Likelihood");
    fprintf(outfile, " method, version %s\n\n",VERSION);


    printf("\n mulsets: %s",(mulsets ? "true" : "false"));
    printf("\n datasets : %ld",(datasets));
    printf("\n rctgry : %s",(rctgry ? "true" : "false"));
    printf("\n gama : %s",(gama ? "true" : "false"));
    printf("\n invar : %s",(invar ? "true" : "false"));
    printf("\n\n ctgry: %s",(ctgry ? "true" : "false"));
    printf("\n categs : %ld",(categs));
    printf("\n rcategs : %ld",(rcategs));    
    printf("\n auto_: %s",(auto_ ? "true" : "false"));
    printf("\n freqsfrom : %s",(freqsfrom ? "true" : "false"));
    printf("\n global : %s",(global ? "true" : "false"));
    printf("\n hypstate : %s",(hypstate ? "true" : "false"));

    printf("\n invar : %s",(invar ? "true" : "false"));
    printf("\n jumble : %s",(jumble ? "true" : "false"));
    printf("\n njumble : %ld",(njumble));
    printf("\n lngths : %s",(lngths ? "true" : "false"));
    printf("\n lambda : %f",(lambda));
    printf("\n lambda1 : %f",(lambda1));
    printf("\n cv : %f",(cv));
    printf("\n freqa : %f",(freqa));
    printf("\n freqc : %f",(freqc));
    printf("\n freqg : %f",(freqg));
    printf("\n freqt : %f",(freqt));
    printf("\n trout : %s",(trout ? "true" : "false"));    
    printf("\n ttratio : %f",(ttratio));
    printf("\n probsum : %f",(probsum));
    printf("\n ttr : %s",(ttr ? "true" : "false"));
    printf("\n usertree : %s",(usertree ? "true" : "false"));
    printf("\n weights: %s",(weights ? "true" : "false"));
    printf("\n printdata : %s",(printdata ? "true" : "false"));
    printf("\n progress : %s",(progress ? "true" : "false"));
    printf("\n treeprint: %s",(treeprint ? "true" : "false"));
    printf("\n interleaved : %s \n\n",(interleaved ? "true" : "false"));
    for (i=0;i<iprobcat;i++)
      printf("probcat[%d] %f\n", i, probcat[i]);

}  /* emboss_getoptions */

void initmemrates() 
{
   probcat = (double *) Malloc(rcategs * sizeof(double));
   rrate = (double *) Malloc(rcategs * sizeof(double));
   iprobcat = rcategs;
}

void reallocsites(void) 
{
  long i;

  for (i = 0; i < spp; i++) {
    free(y[i]);
    y[i] = (char *)Malloc(sites * sizeof(char));
  }
  free(weight);
  free(category);
  free(alias);
  free(aliasweight);
  free(ally);
  free(location);
  
  weight      = (long *)Malloc(sites*sizeof(long));
  category    = (long *)Malloc(sites*sizeof(long));
  alias       = (long *)Malloc(sites*sizeof(long));
  aliasweight = (long *)Malloc(sites*sizeof(long));
  ally        = (long *)Malloc(sites*sizeof(long));
  location    = (long *)Malloc(sites*sizeof(long));
}

void allocrest()
{
  long i;

  y     = (Char **)Malloc(spp*sizeof(Char *));
  nayme  = (naym *)Malloc(spp*sizeof(naym));
  for (i = 0; i < spp; i++)
    y[i] = (char *)Malloc(sites * sizeof(char));
  enterorder  = (long *)Malloc(spp*sizeof(long));
  weight      = (long *)Malloc(sites*sizeof(long));
  category    = (long *)Malloc(sites*sizeof(long));
  alias       = (long *)Malloc(sites*sizeof(long));
  aliasweight = (long *)Malloc(sites*sizeof(long));
  ally        = (long *)Malloc(sites*sizeof(long));
  location    = (long *)Malloc(sites*sizeof(long));
  tymes       = (double *)Malloc((nonodes - spp) * sizeof(double));
}  /* allocrest */


void doinit()
{
  /* initializes variables */

  inputnumbersseq(seqsets[0], &spp, &sites, &nonodes, 1);
  if (printdata)
    fprintf(outfile, "%2ld species, %3ld  sites\n", spp, sites);
  alloctree(&curtree.nodep, nonodes, usertree);
  allocrest();
  if (usertree)
    return;
  alloctree(&bestree.nodep, nonodes, 0);
  if (njumble <= 1)
    return;
  alloctree(&bestree2.nodep, nonodes, 0);
}  /* doinit */


void inputoptions()
{
  long i;

  if (!firstset && !justwts) {
    samenumspseq(seqsets[ith-1], &sites, ith);
    reallocsites();
  }

  for (i = 0; i < sites; i++)
    category[i] = 1;
  for (i = 0; i < sites; i++)
    weight[i] = 1;
  
  if (justwts || weights)
    inputweightsstr(phyloweights->Str[ith-1], sites, weight, &weights);
  weightsum = 0;
  for (i = 0; i < sites; i++)
    weightsum += weight[i];
  if (ctgry && categs > 1) {
    inputcategsstr(phyloratecat->Str[0], 0, sites, category, categs, "DnaMLK");
    if (printdata)
      printcategs(outfile, sites, category, "Site categories");
  }
  if (weights && printdata)
    printweights(outfile, 0, sites, weight, "Sites");
}  /* inputoptions */


void makeweights()
{
  /* make up weights vector to avoid duplicate computations */
  long i;

   for (i = 1; i <= sites; i++) {
    alias[i - 1] = i;
    ally[i - 1] = 0;
    aliasweight[i - 1] = weight[i - 1];
    location[i - 1] = 0;
  }
  sitesort2(sites, aliasweight);
  sitecombine2(sites, aliasweight);
  sitescrunch2(sites, 1, 2, aliasweight);
  for (i = 1; i <= sites; i++) {
    if (aliasweight[i - 1] > 0)
      endsite = i;
  }
  for (i = 1; i <= endsite; i++) {
    ally[alias[i - 1] - 1] = alias[i - 1];
    location[alias[i - 1] - 1] = i;
  }
  contribution = (contribarr *) Malloc( endsite*sizeof(contribarr));
}  /* makeweights */


void getinput()
{
  
  /* reads the input data */
  inputoptions();
  if (!freqsfrom)
    getbasefreqs(freqa, freqc, freqg, freqt, &freqr, &freqy, &freqar, &freqcy,
                   &freqgr, &freqty, &ttratio, &xi, &xv, &fracchange,
                   freqsfrom, true);
  if (!justwts || firstset)
    seq_inputdata(seqsets[ith-1], sites);
  makeweights();
  setuptree2(curtree);
  if (!usertree) {
    setuptree2(bestree);
    if (njumble > 1)
      setuptree2(bestree2);
  }
  allocx(nonodes, rcategs, curtree.nodep, usertree);
  if (!usertree) {
    allocx(nonodes, rcategs, bestree.nodep, 0);
    if (njumble > 1)
      allocx(nonodes, rcategs, bestree2.nodep, 0);
  }
  makevalues2(rcategs, curtree.nodep, endsite, spp, y, alias);
  if (freqsfrom) {
    empiricalfreqs(&freqa, &freqc, &freqg, &freqt, aliasweight, curtree.nodep);
    getbasefreqs(freqa, freqc, freqg, freqt, &freqr, &freqy, &freqar, &freqcy,
                   &freqgr, &freqty, &ttratio, &xi, &xv, &fracchange,
                   freqsfrom, true);
  }
  if (!justwts || firstset)
    fprintf(outfile, "\nTransition/transversion ratio = %10.6f\n\n", ttratio);
}  /* getinput */


void inittable_for_usertree (char* treestr)
{
  /* If there's a user tree, then the ww/zz/wwzz/vvzz elements need
     to be allocated appropriately. */
  long num_comma;
  long i, j;

  /* First, figure out the largest possible furcation, i.e. the number
     of commas plus one */
  countcomma (treestr, &num_comma);
  num_comma++;
  
  for (i = 0; i < rcategs; i++) {
    for (j = 0; j < categs; j++) {
      /* Free the stuff allocated assuming bifurcations */
      free (tbl[i][j]->ww);
      free (tbl[i][j]->zz);
      free (tbl[i][j]->wwzz);
      free (tbl[i][j]->vvzz);

      /* Then allocate for worst-case multifurcations */
      tbl[i][j]->ww   = (double *) Malloc( num_comma * sizeof (double));
      tbl[i][j]->zz   = (double *) Malloc( num_comma * sizeof (double));
      tbl[i][j]->wwzz = (double *) Malloc( num_comma * sizeof (double));
      tbl[i][j]->vvzz = (double *) Malloc( num_comma * sizeof (double));
    }
  }
}  /* inittable_for_usertree */


void inittable()
{
  /* Define a lookup table. Precompute values and print them out in tables */
  long i, j;
  double sumrates;
  
  tbl = (valrec ***) Malloc( rcategs * sizeof(valrec **));
  for (i = 0; i < rcategs; i++) {
    tbl[i] = (valrec **) Malloc( categs*sizeof(valrec *));
    for (j = 0; j < categs; j++)
      tbl[i][j] = (valrec *) Malloc( sizeof(valrec));
  }

  for (i = 0; i < rcategs; i++) {
    for (j = 0; j < categs; j++) {
      tbl[i][j]->rat = rrate[i]*rate[j];
      tbl[i][j]->ratxi = tbl[i][j]->rat * xi;
      tbl[i][j]->ratxv = tbl[i][j]->rat * xv;

      /* Allocate assuming bifurcations, will be changed later if
         neccesarry (i.e. there's a user tree) */
      tbl[i][j]->ww   = (double *) Malloc( 2 * sizeof (double));
      tbl[i][j]->zz   = (double *) Malloc( 2 * sizeof (double));
      tbl[i][j]->wwzz = (double *) Malloc( 2 * sizeof (double));
      tbl[i][j]->vvzz = (double *) Malloc( 2 * sizeof (double));
    }
  }
  sumrates = 0.0;
  for (i = 0; i < endsite; i++) {
    for (j = 0; j < rcategs; j++)
      sumrates += aliasweight[i] * probcat[j] 
        * tbl[j][category[alias[i] - 1] - 1]->rat;
  }
  sumrates /= (double)sites;
  for (i = 0; i < rcategs; i++)
    for (j = 0; j < categs; j++) {
      tbl[i][j]->rat /= sumrates;
      tbl[i][j]->ratxi /= sumrates;
      tbl[i][j]->ratxv /= sumrates;
    }

  if(jumb > 1)
    return;

  if (gama || invar) {
    fprintf(outfile, "\nDiscrete approximation to gamma distributed rates\n");
    fprintf(outfile,
    " Coefficient of variation of rates = %f  (alpha = %f)\n", cv, alpha);
  }
  if (rcategs > 1) {
    fprintf(outfile, "\nState in HMM    Rate of change    Probability\n\n");
    for (i = 0; i < rcategs; i++)
      if (probcat[i] < 0.0001)
        fprintf(outfile, "%9ld%16.3f%20.6f\n", i+1, rrate[i], probcat[i]);
      else if (probcat[i] < 0.001)
          fprintf(outfile, "%9ld%16.3f%19.5f\n", i+1, rrate[i], probcat[i]);
        else if (probcat[i] < 0.01)
            fprintf(outfile, "%9ld%16.3f%18.4f\n", i+1, rrate[i], probcat[i]);
          else
            fprintf(outfile, "%9ld%16.3f%17.3f\n", i+1, rrate[i], probcat[i]);
    putc('\n', outfile);
    if (auto_) {
      fprintf(outfile,
     "Expected length of a patch of sites having the same rate = %8.3f\n",
             1/lambda);
      putc('\n', outfile);
    }
  }
  if (categs > 1) {
    fprintf(outfile, "\nSite category   Rate of change\n\n");
    for (i = 0; i < categs; i++)
      fprintf(outfile, "%9ld%16.3f\n", i+1, rate[i]);
    fprintf(outfile, "\n\n");
  }
}  /* inittable */


void exmake(double lz, long n)
{
  /* pretabulate tables of exponentials so need not do for each site */
  long i;
  double rat;

  for (i = 0; i < categs; i++) {
    rat = rate[i];
    switch (n) {

    case 1:
      expon1i[i] = exp(rat * xi * lz);
      expon1v[i] = exp(rat * xv * lz);
      break;

    case 2:
      expon2i[i] = exp(rat * xi * lz);
      expon2v[i] = exp(rat * xv * lz);
      break;
    }
  }
}  /* exmake */


void alloc_nvd(long num_sibs, nuview_data *local_nvd)
{
  /* Allocate blocks of memory appropriate for the number of siblings
     a given node has */
  local_nvd->yy     = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->wwzz   = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->vvzz   = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->vzsumr = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->vzsumy = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->sum    = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->sumr   = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->sumy   = (double *) Malloc( num_sibs * sizeof (double));
  local_nvd->xx     = (sitelike *) Malloc( num_sibs * sizeof (sitelike));
}  /* alloc_nvd */


void free_nvd(nuview_data *local_nvd)
{
  /* The natural complement to the alloc version */
  free (local_nvd->yy);
  free (local_nvd->wwzz);
  free (local_nvd->vvzz);
  free (local_nvd->vzsumr);
  free (local_nvd->vzsumy);
  free (local_nvd->sum);
  free (local_nvd->sumr);
  free (local_nvd->sumy);
  free (local_nvd->xx);
}  /* free_nvd */


void nuview(node *p)
/* current (modified dnaml) nuview */
{
  long i, j, k, num_sibs, sib_index;
  nuview_data *local_nvd;
  node *sib_ptr, *sib_back_ptr;
  sitelike p_xx;
  double lw;

  /* Figure out how many siblings the current node has */
  num_sibs    = count_sibs (p);
  /* Recursive calls, should be called for all children */
  sib_ptr = p;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;

    if (!(sib_back_ptr == NULL))
      if (!sib_back_ptr->tip && !sib_back_ptr->initialized)
        nuview (sib_back_ptr);
  }
  /* Allocate the structure and blocks therein for variables used in
     this function */
  local_nvd = (nuview_data *) Malloc( sizeof (nuview_data));
  alloc_nvd (num_sibs, local_nvd);

  /* Loop 1: makes assignments to tbl based on some combination of
     what's already in tbl and the children's value of v */
  sib_ptr = p;
  for (sib_index=0; sib_index < num_sibs; sib_index++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    
    if (sib_back_ptr != NULL)
      lw =  -fabs(p->tyme - sib_back_ptr->tyme);
    else
      lw = 0.0;

    for (i = 0; i < rcategs; i++)
      for (j = 0; j < categs; j++) {
        tbl[i][j]->ww[sib_index]   = exp(tbl[i][j]->ratxi * lw);
        tbl[i][j]->zz[sib_index]   = exp(tbl[i][j]->ratxv * lw);
        tbl[i][j]->wwzz[sib_index] = tbl[i][j]->ww[sib_index] * tbl[i][j]->zz[sib_index];
        tbl[i][j]->vvzz[sib_index] = (1.0 - tbl[i][j]->ww[sib_index]) *
          tbl[i][j]->zz[sib_index];
      }
  }

  /* Loop 2: */
  for (i = 0; i < endsite; i++) {
    k = category[alias[i]-1] - 1;
    for (j = 0; j < rcategs; j++) {

      /* Loop 2.1 */
      sib_ptr = p;
      for (sib_index=0; sib_index < num_sibs; sib_index++) {
        sib_ptr         = sib_ptr->next;
        sib_back_ptr    = sib_ptr->back;

        local_nvd->wwzz[sib_index] = tbl[j][k]->wwzz[sib_index];
        local_nvd->vvzz[sib_index] = tbl[j][k]->vvzz[sib_index];
        local_nvd->yy[sib_index]   = 1.0 - tbl[j][k]->zz[sib_index];
        if (sib_back_ptr != NULL)
          memcpy(local_nvd->xx[sib_index],
               sib_back_ptr->x[i][j],
               sizeof(sitelike));
        else {
          local_nvd->xx[sib_index][0] = 1.0;
          local_nvd->xx[sib_index][(long)C - (long)A] = 1.0;
          local_nvd->xx[sib_index][(long)G - (long)A] = 1.0;
          local_nvd->xx[sib_index][(long)T - (long)A] = 1.0;
        }
      }

      /* Loop 2.2 */
      for (sib_index=0; sib_index < num_sibs; sib_index++) {
        local_nvd->sum[sib_index] =
          local_nvd->yy[sib_index] *
          (freqa * local_nvd->xx[sib_index][(long)A] +
           freqc * local_nvd->xx[sib_index][(long)C] +
           freqg * local_nvd->xx[sib_index][(long)G] +
           freqt * local_nvd->xx[sib_index][(long)T]);
        local_nvd->sumr[sib_index] =
          freqar * local_nvd->xx[sib_index][(long)A] +
          freqgr * local_nvd->xx[sib_index][(long)G];
        local_nvd->sumy[sib_index] =
          freqcy * local_nvd->xx[sib_index][(long)C] +
          freqty * local_nvd->xx[sib_index][(long)T];
        local_nvd->vzsumr[sib_index] =
          local_nvd->vvzz[sib_index] * local_nvd->sumr[sib_index];
        local_nvd->vzsumy[sib_index] =
          local_nvd->vvzz[sib_index] * local_nvd->sumy[sib_index];
      }

      /* Initialize to one, multiply incremental values for every
         sibling a node has */
      p_xx[(long)A] = 1 ;
      p_xx[(long)C] = 1 ; 
      p_xx[(long)G] = 1 ;
      p_xx[(long)T] = 1 ;

      for (sib_index=0; sib_index < num_sibs; sib_index++) {
        p_xx[(long)A] *=
          local_nvd->sum[sib_index] +
          local_nvd->wwzz[sib_index] *
          local_nvd->xx[sib_index][(long)A] +
          local_nvd->vzsumr[sib_index];
        p_xx[(long)C] *=
          local_nvd->sum[sib_index] +
          local_nvd->wwzz[sib_index] *
          local_nvd->xx[sib_index][(long)C] +
          local_nvd->vzsumy[sib_index];
        p_xx[(long)G] *=
          local_nvd->sum[sib_index] +
          local_nvd->wwzz[sib_index] *
          local_nvd->xx[sib_index][(long)G] +
          local_nvd->vzsumr[sib_index];
        p_xx[(long)T] *=
          local_nvd->sum[sib_index] +
          local_nvd->wwzz[sib_index] *
          local_nvd->xx[sib_index][(long)T] +
          local_nvd->vzsumy[sib_index];
      }

      /* And the final point of this whole function: */
      memcpy(p->x[i][j], p_xx, sizeof(sitelike));
    }
  }

  p->initialized = true;

  free_nvd (local_nvd);
  free (local_nvd);
}  /* nuview */


double evaluate(node *p)
{
  contribarr tterm;
  static contribarr like, nulike, clai;
  double sum, sum2, sumc=0, y, lz, y1, z1zz, z1yy, 
                prod12, prod1, prod2, prod3, sumterm, lterm;
  long i, j, k, lai;
  node *q, *r;
  sitelike x1, x2;
  sum = 0.0;

  if (p == curtree.root && (count_sibs(p) == 2)) {
    r = p->next->back;
    q = p->next->next->back;
    y = r->tyme + q->tyme - 2 * p->tyme;
    if (!r->tip && !r->initialized) nuview (r);
    if (!q->tip && !q->initialized) nuview (q);
  } else if (p == curtree.root) {
    /* the next two lines copy tyme and x to p->next.  Normally they are 
       not initialized for an internal node. */
/* assumes bifurcation */
    p->next->tyme = p->tyme;
    nuview(p->next);
    r = p->next;
    q = p->next->back;
    y = fabs(p->next->tyme - q->tyme);
  } else {
    r = p;
    q = p->back;
    if (!r->tip && !r->initialized) nuview (r);
    if (!q->tip && !q->initialized) nuview (q);
    y = fabs(r->tyme - q->tyme);
  }

  lz = -y;
  for (i = 0; i < rcategs; i++)
    for (j = 0; j < categs; j++) {
    tbl[i][j]->orig_zz = exp(tbl[i][j]->ratxi * lz);
    tbl[i][j]->z1 = exp(tbl[i][j]->ratxv * lz);
    tbl[i][j]->z1zz = tbl[i][j]->z1 * tbl[i][j]->orig_zz;
    tbl[i][j]->z1yy = tbl[i][j]->z1 - tbl[i][j]->z1zz;
  }
  for (i = 0; i < endsite; i++) {
    k = category[alias[i]-1] - 1;
    for (j = 0; j < rcategs; j++) {
      if (y > 0.0) {
        y1 = 1.0 - tbl[j][k]->z1;
        z1zz = tbl[j][k]->z1zz;
        z1yy = tbl[j][k]->z1yy;
      } else {
        y1 = 0.0;
        z1zz = 1.0;
        z1yy = 0.0;
      }
      memcpy(x1, r->x[i][j], sizeof(sitelike));
      prod1 = freqa * x1[0] + freqc * x1[(long)C - (long)A] +
             freqg * x1[(long)G - (long)A] + freqt * x1[(long)T - (long)A];
      memcpy(x2, q->x[i][j], sizeof(sitelike));
      prod2 = freqa * x2[0] + freqc * x2[(long)C - (long)A] +
             freqg * x2[(long)G - (long)A] + freqt * x2[(long)T - (long)A];
      prod3 = (x1[0] * freqa + x1[(long)G - (long)A] * freqg) *
              (x2[0] * freqar + x2[(long)G - (long)A] * freqgr) +
          (x1[(long)C - (long)A] * freqc + x1[(long)T - (long)A] * freqt) *
         (x2[(long)C - (long)A] * freqcy + x2[(long)T - (long)A] * freqty);
      prod12 = freqa * x1[0] * x2[0] +
               freqc * x1[(long)C - (long)A] * x2[(long)C - (long)A] +
               freqg * x1[(long)G - (long)A] * x2[(long)G - (long)A] +
               freqt * x1[(long)T - (long)A] * x2[(long)T - (long)A];
      tterm[j] = z1zz * prod12 + z1yy * prod3 + y1 * prod1 * prod2;
    }
    sumterm = 0.0;
    for (j = 0; j < rcategs; j++)
      sumterm += probcat[j] * tterm[j];
    lterm = log(sumterm);
    for (j = 0; j < rcategs; j++)
      clai[j] = tterm[j] / sumterm;
    memcpy(contribution[i], clai, sizeof(contribarr));
    if (!auto_ && usertree && (which <= shimotrees))
      l0gf[which - 1][i] = lterm;
    sum += aliasweight[i] * lterm;
  }
  if (auto_) {
    for (j = 0; j < rcategs; j++)
      like[j] = 1.0;
    for (i = 0; i < sites; i++) {
      if ((ally[i] > 0) && (location[ally[i]-1] > 0)) {
        sumc = 0.0;
        for (k = 0; k < rcategs; k++)
          sumc += probcat[k] * like[k];
        sumc *= lambda;
        lai = location[ally[i] - 1];
        memcpy(clai, contribution[lai - 1], sizeof(contribarr));
        for (j = 0; j < rcategs; j++)
          nulike[j] = ((1.0 - lambda) * like[j] + sumc) * clai[j];
      } else {
        for (j = 0; j < rcategs; j++)
          nulike[j] = ((1.0 - lambda) * like[j] + sumc);
      }
      memcpy(like, nulike, sizeof(contribarr));
    }
    sum2 = 0.0;
    for (i = 0; i < rcategs; i++)
      sum2 += probcat[i] * like[i];
    sum += log(sum2);
  }
  curtree.likelihood = sum;
  if (auto_ || !usertree)
    return sum;
  if(which <= shimotrees)
    l0gl[which - 1] = sum;
  if (which == 1) {
    maxwhich = 1;
    maxlogl = sum;
    return sum;
  }
  if (sum > maxlogl) {
    maxwhich = which;
    maxlogl = sum;
  }
  return sum;
}  /* evaluate */


void getthree(node *p, double thigh, double tlow)
{
  /* compute likelihood at a new triple of points */
  int i;
  double tt = p->tyme;
  double td = fabs(tdelta);

  x[0] = tt - td;
  x[1] = tt;
  x[2] = tt + td;

  if ( x[0] < tlow + epsilon ) {
    x[0] = tlow + epsilon;
    x[1] = ( x[0] + x[2] ) / 2;
  }

  if ( x[2] > thigh - epsilon ) {
    x[2] = thigh - epsilon;
    x[1] = ( x[0] + x[2] ) / 2;
  }
  
  for ( i = 0 ; i < 3 ; i++ ) {
    p->tyme = x[i];
    nuview(p);
    lnl[i] = evaluate(p);
  }
}  /* getthree */


void makenewv(node *p)
{
  /* improve a node time */
  long it, imin, imax, i, num_sibs;
  double tt, tfactor, tlow, thigh, oldlike, oldx, ymin, ymax, s32, s21, yold;
  boolean done, already;
  node *s, *sdown, *sib_ptr, *sib_back_ptr;

  s = curtree.nodep[p->index - 1];
  oldx = s->tyme;
  oldlike = curtree.likelihood;
  sdown = s->back;
  if (s == curtree.root)
    tlow = -10.0;
  else
    tlow = sdown->tyme;

  sib_ptr = s;
  num_sibs = count_sibs(p);

  thigh = s->next->back->tyme;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
      if (sib_back_ptr->tyme < thigh)
        thigh = sib_back_ptr->tyme;
  }
  done = (thigh - tlow < 4.0*epsilon);
  it = 1;
  if (s != curtree.root)
    tdelta = (thigh - tlow) / 10.0;
  else
    tdelta = (thigh - s->tyme) / 5.0;
  tfactor = 1.0;
  if (!done)
    getthree(s, thigh, tlow); /* get three points for interpolation */
  while (it < iterations && !done) {
    ymax = lnl[0];
    imax = 1;
    for (i = 2; i <= 3; i++) { /*figure out which point has the largest */
      if (lnl[i - 1] > ymax) { /*ln likelihood value */
        ymax = lnl[i - 1];  
        imax = i;
      }
    }
    if (imax != 2) { /* make sure that the second point has the largest */
      ymax = x[1];   /* ln likelihood value */
      x[1] = x[imax - 1];
      x[imax - 1] = ymax;
      ymax = lnl[1];
      lnl[1] = lnl[imax - 1];
      lnl[imax - 1] = ymax;
    }
    tt = x[1];
    yold = tt;
    s32 = (lnl[2] - lnl[1]) / (x[2] - x[1]); /* compute slopes */
    s21 = (lnl[1] - lnl[0]) / (x[1] - x[0]);
    if (fabs(x[2] - x[0]) > epsilon)
      curv = (s32 - s21) / ((x[2] - x[0]) / 2);
    else
      curv = 0.0;
    slope = (s32 + s21) / 2 - curv * (x[2] - 2 * x[1] + x[0]) / 4;
    if (curv >= 0.0) {
      if (slope < 0)
        tdelta = -fabs(tdelta);
      else
        tdelta = fabs(tdelta);
    } else
      tdelta = -(tfactor * slope / curv);
    if (tt + tdelta <= tlow + epsilon)
      tdelta = tlow + epsilon - tt;
    if (tt + tdelta >= thigh - epsilon)
      tdelta = thigh - epsilon - tt;
    tt += tdelta;
    done = (fabs(yold - tt) < epsilon || fabs(tdelta) < epsilon);
    s->tyme = tt;
    nuview(s);
    lnlike = evaluate(s);
    ymin = lnl[0];
    imin = 1;
    for (i = 2; i <= 3; i++) { /* figure out which of the three original */
      if (lnl[i - 1] < ymin) { /* points has the lowest ln likelihood */
        ymin = lnl[i - 1];
        imin = i;
      }
    }
    already = (tt == x[0]) || (tt == x[1]) || (tt == x[2]);
    if (!already && ymin < lnlike) { /* if the minimum point is lower than   */
      x[imin - 1] = tt;              /* our new interpolated point than take */
      lnl[imin - 1] = lnlike;        /* that point and put it where the*/
    }                                /* interpolated point is */
    if (already || lnlike < oldlike) { 
      tt = x[2];                     /* if either our interpolated point has */
      s->tyme = x[2];                /* a lower score or is equivalent to    */
      tfactor /= 2;                  /* our original three reinterpolate this*/
      tdelta /= 2;                   /* time go only half as far             */
      curtree.likelihood = lnl[2];
      lnlike = lnl[2];
    } else
      tfactor = 1.0;

    if (!done) {  /* apply it to the sibs */
      sib_ptr = p;
      num_sibs = count_sibs(p);
      p->tyme = tt;
      for (i=0 ; i < num_sibs; i++) {
        sib_ptr      = sib_ptr->next;
        sib_ptr->tyme = tt;
      }
  
      sib_ptr = p;
      nuview(p);
      for (i=0 ; i < num_sibs; i++) {
        sib_ptr      = sib_ptr->next;
        nuview(sib_ptr);
      }
    }
    
    it++;
  }
  sib_ptr = p;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    inittrav (sib_ptr);
  }
  smoothed = smoothed && done;
}  /* makenewv */


void update(node *p)
{
  node *sib_ptr, *sib_back_ptr;
  long i, num_sibs;
  
  /* improve time and recompute views at a node */
  if (p == NULL)
    return;
  if (p->back != NULL) {
    if (!p->back->tip && !p->back->initialized)
      nuview(p->back);
  }

  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    if (sib_back_ptr != NULL) {
      if (!sib_back_ptr->tip && !sib_back_ptr->initialized)
        nuview(sib_back_ptr);
    }
  }

  if ((!usertree) || (usertree && !lngths) || p->iter) {
    makenewv(p);
    return;
  }
  nuview(p);

  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    nuview(sib_ptr);  
  }
}  /* update */


void smooth(node *p)
{
  node *sib_ptr;
  long i, num_sibs;

  if (p == NULL)
    return;
  if (p->tip)
    return;

  update(p);

  smoothed = false;
  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0; i < num_sibs; i++) {
    sib_ptr = sib_ptr->next;
    if (polishing || (smoothit && !smoothed)) {
      smooth(sib_ptr->back);
      p->initialized = false;
      sib_ptr->initialized = false;
    }
    update(p);  
  }
}  /* smooth */


void restoradd(node *below, node *newtip, node *newfork, double prevtyme)
{
/* restore "new" tip and fork to place "below".  restore tymes */
/* assumes bifurcation */
  hookup(newfork, below->back);
  hookup(newfork->next, below);
  hookup(newtip, newfork->next->next);
  curtree.nodep[newfork->index-1] = newfork;
  newfork->tyme = prevtyme;
/* assumes bifurcations */
  newfork->next->tyme = prevtyme;
  newfork->next->next->tyme = prevtyme;
} /* restoradd */


void dnamlk_add(node *below, node *newtip, node *newfork)
{
  /* inserts the nodes newfork and its descendant, newtip, into the tree. */
  long i;
  boolean done;
  node *p;

  below = curtree.nodep[below->index - 1];
  newfork = curtree.nodep[newfork->index-1];
  newtip = curtree.nodep[newtip->index-1];
  if (below->back != NULL)
    below->back->back = newfork;
  newfork->back = below->back;
  below->back = newfork->next->next;
  newfork->next->next->back = below;
  newfork->next->back = newtip;
  newtip->back = newfork->next;
  if (newtip->tyme < below->tyme)
    p = newtip;
  else p = below;
  newfork->tyme = p->tyme;
  if (curtree.root == below)
    curtree.root = newfork;
  if (newfork->back != NULL) {
    if (p->tyme > newfork->back->tyme)
      newfork->tyme = (p->tyme + newfork->back->tyme) / 2.0;
    else newfork->tyme = p->tyme - epsilon;
    newfork->next->tyme = newfork->tyme;
    newfork->next->next->tyme = newfork->tyme;
    do {
      p = curtree.nodep[p->back->index - 1];
      done = (p == curtree.root);
      if (!done)
        done = (curtree.nodep[p->back->index - 1]->tyme < p->tyme - epsilon);
      if (!done) {
        curtree.nodep[p->back->index - 1]->tyme = p->tyme - epsilon;
        curtree.nodep[p->back->index - 1]->next->tyme = p->tyme - epsilon;
        curtree.nodep[p->back->index - 1]->next->next->tyme = p->tyme - epsilon;
      }
    } while (!done);
  } else {
      newfork->tyme = newfork->tyme - 2*epsilon;
      newfork->next->tyme = newfork->tyme;
      newfork->next->next->tyme = newfork->tyme;
    }
  inittrav(newtip);
  inittrav(newtip->back);
  smoothed = false;
  i = 1;
  while (i < smoothings && !smoothed) {
    smoothed = true;
    smooth(newfork);
    smooth(newfork->back);
    i++;
  }
}  /* dnamlk_add */


void dnamlk_re_move(node **item, node **fork, boolean tempadd)
{
  /* removes nodes item and its ancestor, fork, from the tree.
    the new descendant of fork's ancestor is made to be
    fork's second descendant (other than item).  Also
    returns pointers to the deleted nodes, item and fork */
  node *p, *q;
  long i;

  if ((*item)->back == NULL) {
    *fork = NULL;
    return;
  }
  *item = curtree.nodep[(*item)->index-1];
  *fork = curtree.nodep[(*item)->back->index - 1];
  if (curtree.root == *fork) {
    if (*item == (*fork)->next->back)
      curtree.root = (*fork)->next->next->back;
    else
      curtree.root = (*fork)->next->back;
  }
  p = (*item)->back->next->back;
  q = (*item)->back->next->next->back;
  if (p != NULL)
    p->back = q;
  if (q != NULL)
    q->back = p;
  (*fork)->back = NULL;
  p = (*fork)->next;
  while (p != *fork) {
    p->back = NULL;
    p = p->next;
  }
  (*item)->back = NULL;
  inittrav(p);
  inittrav(q);
  if (tempadd)
    return;
  i = 1;
  while (i <= smoothings) {
    smooth(q);
    if (smoothit)
      smooth(q->back);
    i++;
  }
}  /* dnamlk_re_move */


void tryadd(node *p, node **item, node **nufork)
{  /* temporarily adds one fork and one tip to the tree.
    if the location where they are added yields greater
    likelihood than other locations tested up to that
    time, then keeps that location as there */

  dnamlk_add(p, *item, *nufork);
  like = evaluate(p);
  if (lastsp) {
      if (like >= bestree.likelihood || bestree.likelihood == UNDEFINED)  {
            copy_(&curtree, &bestree, nonodes, rcategs);
            if (global2) 
                save_tree_tyme(&curtree,tymes);
      }
  }
  if (like > bestyet || bestyet == UNDEFINED) {
    bestyet = like;
    there = p;
  }
  dnamlk_re_move(item, nufork, true);
  if ( global2 ) {
      restore_saved_tyme(&curtree,tymes);
  }
}  /* tryadd */


void addpreorder(node *p, node *item_, node *nufork_, boolean contin,
                        boolean continagain)
{
  /* traverses a binary tree, calling function tryadd
    at a node before calling tryadd at its descendants */
  node *item, *nufork;

  item = item_;
  nufork = nufork_;
  if (p == NULL)
    return;
  tryadd(p, &item, &nufork);
  contin = continagain;
  if ((!p->tip) && contin) {
    addpreorder(p->next->back, item, nufork, contin, continagain);
    addpreorder(p->next->next->back, item, nufork, contin, continagain);
  }
}  /* addpreorder */


void tryrearr(node *p, boolean *success)
{
  /* evaluates one rearrangement of the tree.
    if the new tree has greater likelihood than the old
    one sets success = TRUE and keeps the new tree.
    otherwise, restores the old tree */
  node *frombelow, *whereto, *forknode;
  double oldlike, prevtyme;
  boolean wasonleft;

  if (p == curtree.root)
    return;
  forknode = curtree.nodep[p->back->index - 1];
  if (forknode == curtree.root)
    return;
  oldlike = bestyet;
  prevtyme = forknode->tyme;
/* the following statement presumes bifurcating tree */
  if (forknode->next->back == p) {
    frombelow = forknode->next->next->back;
    wasonleft = true;
  }
  else {
    frombelow = forknode->next->back;
    wasonleft = false;
  }
  whereto = curtree.nodep[forknode->back->index - 1];
  dnamlk_re_move(&p, &forknode, true);
  dnamlk_add(whereto, p, forknode);
  like = evaluate(p);
  if (like <= oldlike && oldlike != UNDEFINED) {
    dnamlk_re_move(&p, &forknode, true);
    restoradd(frombelow, p, forknode, prevtyme);
    if (wasonleft && (forknode->next->next->back == p)) {
       hookup (forknode->next->back, forknode->next->next);
       hookup (forknode->next, p);
    }
    curtree.likelihood = oldlike;
    inittrav(forknode);
    inittrav(forknode->next);
    inittrav(forknode->next->next);
  } else {
    (*success) = true;
    bestyet = like;
  }
}  /* tryrearr */


void repreorder(node *p, boolean *success)
{
  /* traverses a binary tree, calling function tryrearr
    at a node before calling tryrearr at its descendants */
  if (p == NULL)
    return;
  tryrearr(p, success);
  if (p->tip)
    return;
  if (!(*success))
    repreorder(p->next->back, success);
  if (!(*success))
    repreorder(p->next->next->back, success);
}  /* repreorder */


void rearrange(node **r)
{
  /* traverses the tree (preorder), finding any local
    rearrangement which increases the likelihood.
    if traversal succeeds in increasing the tree's
    likelihood, function rearrange runs traversal again */
  boolean success;
  success = true;
  while (success) {
    success = false;
    repreorder(*r, &success);
  }
}  /* rearrange */


void initdnamlnode(node **p, node **grbg, node *q, long len, long nodei,
                     long *ntips, long *parens, initops whichinit,
                     pointarray treenode, pointarray nodep, Char *str, Char *ch,
                     char** treestr)
{
  /* initializes a node */
  boolean minusread;
  double valyew, divisor;

  switch (whichinit) {
  case bottom:
    gnu(grbg, p);
    (*p)->index = nodei;
    (*p)->tip = false;
    malloc_pheno((*p), endsite, rcategs);
    nodep[(*p)->index - 1] = (*p);
    break;
  case nonbottom:
    gnu(grbg, p);
    malloc_pheno(*p, endsite, rcategs);
    (*p)->index = nodei;
    break;
  case tip:
    match_names_to_data (str, nodep, p, spp);
    break;
  case iter:
    (*p)->initialized = false;
    (*p)->v = initialv;
    (*p)->iter = true;
    if ((*p)->back != NULL)
      (*p)->back->iter = true;
    break;
  case length:
    processlength(&valyew, &divisor, ch, &minusread, treestr, parens);
    (*p)->v = valyew / divisor / fracchange;
    (*p)->iter = false;
    if ((*p)->back != NULL) {
      (*p)->back->v = (*p)->v;
      (*p)->back->iter = false;
    }
    break;
  case hslength:
    break;
  case hsnolength:
    break;
  case treewt:
    break;
  case unittrwt:
    curtree.nodep[spp]->iter = false; 
    break;
  }
} /* initdnamlnode */


void tymetrav(node *p, double *x)
{
  /* set up times of nodes */
  node *sib_ptr, *q;
  long i, num_sibs;
  double xmax;

  xmax = 0.0;
  if (!p->tip) {
    sib_ptr  = p;
    num_sibs = count_sibs(p);
    for (i=0; i < num_sibs; i++) {
      sib_ptr = sib_ptr->next;
      tymetrav(sib_ptr->back, x);
      if (xmax > (*x))
        xmax = (*x);
    }
  } else
    (*x)     = 0.0;
  p->tyme  = xmax;
  if (!p->tip) {
    q = p;
    while (q->next != p) {
      q = q->next;
      q->tyme = p->tyme;
    }
  }
  (*x) = p->tyme - p->v;
}  /* tymetrav */


void dnamlk_coordinates(node *p, long *tipy)
{
  /* establishes coordinates of nodes */
  node *q, *first, *last, *pp1 =NULL, *pp2 =NULL;
  long num_sibs, p1, p2, i;

  if (p->tip) {
    p->xcoord = 0;
    p->ycoord = (*tipy);
    p->ymin   = (*tipy);
    p->ymax   = (*tipy);
    (*tipy)  += down;
    return;
  }
  q = p->next;
  do {
    dnamlk_coordinates(q->back, tipy);
    q = q->next;
  } while (p != q);
  num_sibs = count_sibs(p);
  p1 = (long)((num_sibs+1)/2.0);
  p2 = (long)((num_sibs+2)/2.0);
  i = 1;
  q = p->next;
  first  = q->back;
  do {
    if (i == p1) pp1 = q->back;
    if (i == p2) pp2 = q->back;
    last = q->back;
    q = q->next;
    i++;
  } while (q != p);
  p->xcoord = (long)(0.5 - over * p->tyme);
  p->ycoord = (pp1->ycoord + pp2->ycoord) / 2;
  p->ymin = first->ymin;
  p->ymax = last->ymax;
}  /* dnamlk_coordinates */


void dnamlk_drawline(long i, double scale)
{
  /* draws one row of the tree diagram by moving up tree */
  node *p, *q, *r, *first =NULL, *last =NULL;
  long n, j;
  boolean extra, done;

  p = curtree.root;
  q = curtree.root;
  extra = false;
  if ((long)(p->ycoord) == i) {
    if (p->index - spp >= 10)
      fprintf(outfile, "-%2ld", p->index - spp);
    else
      fprintf(outfile, "--%ld", p->index - spp);
    extra = true;
  } else
    fprintf(outfile, "  ");
  do {
    if (!p->tip) {
      r = p->next;
      done = false;
      do {
        if (i >= r->back->ymin && i <= r->back->ymax) {
          q = r->back;
          done = true;
        }
        r = r->next;
      } while (!(done || r == p));
      first = p->next->back;
      r = p->next;
      while (r->next != p)
        r = r->next;
      last = r->back;
    }
    done = (p == q);
    n = (long)(scale * ((long)(p->xcoord) - (long)(q->xcoord)) + 0.5);
    if (n < 3 && !q->tip)
      n = 3;
    if (extra) {
      n--;
      extra = false;
    }
    if ((long)(q->ycoord) == i && !done) {
      if (p->ycoord != q->ycoord)
        putc('+', outfile);
      else
        putc('-', outfile);
      if (!q->tip) {
        for (j = 1; j <= n - 2; j++)
          putc('-', outfile);
        if (q->index - spp >= 10)
          fprintf(outfile, "%2ld", q->index - spp);
        else
          fprintf(outfile, "-%ld", q->index - spp);
        extra = true;
      } else {
        for (j = 1; j < n; j++)
          putc('-', outfile);
      }
    } else if (!p->tip) {
      if ((long)(last->ycoord) > i && (long)(first->ycoord) < i && 
           i != (long)(p->ycoord)) {
        putc('!', outfile);
        for (j = 1; j < n; j++)
          putc(' ', outfile);
      } else {
        for (j = 1; j <= n; j++)
          putc(' ', outfile);
      }
    } else {
      for (j = 1; j <= n; j++)
        putc(' ', outfile);
    }
    if (p != q)
      p = q;
  } while (!done);
  if ((long)(p->ycoord) == i && p->tip) {
    for (j = 0; j < nmlngth; j++)
      putc(nayme[p->index - 1][j], outfile);
  }
  putc('\n', outfile);
}  /* dnamlk_drawline */


void dnamlk_printree()
{
 /* prints out diagram of the tree */
  long tipy;
  double scale;
  long i;
  node *p;

  if (!treeprint)
    return;
  putc('\n', outfile);
  tipy = 1;
  dnamlk_coordinates(curtree.root, &tipy);
  p = curtree.root;
  while (!p->tip)
    p = p->next->back;
  scale = 1.0 / (long)(p->tyme - curtree.root->tyme + 1.000);
  putc('\n', outfile);
  for (i = 1; i <= tipy - down; i++)
    dnamlk_drawline(i, scale);
  putc('\n', outfile);
}  /* dnamlk_printree */


void describe(node *p)
{
  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;
  double v;

  if (p == curtree.root)
    fprintf(outfile, " root         ");
  else
    fprintf(outfile, "%4ld          ", p->back->index - spp);
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index - 1][i], outfile);
  } else
    fprintf(outfile, "%4ld      ", p->index - spp);
  if (p != curtree.root) {
    fprintf(outfile, "%11.5f", fracchange * (p->tyme - curtree.root->tyme));
    v = fracchange * (p->tyme - curtree.nodep[p->back->index - 1]->tyme);
    fprintf(outfile, "%13.5f", v);
  }
  putc('\n', outfile);
  if (!p->tip) {

    sib_ptr = p;
    num_sibs = count_sibs(p);
    for (i=0 ; i < num_sibs; i++) {
      sib_ptr      = sib_ptr->next;
      sib_back_ptr = sib_ptr->back;
      describe(sib_back_ptr);
    }
  }
}  /* describe */


void reconstr(node *p, long n)
{
  /* reconstruct and print out base at site n+1 at node p */
  long i, j, k, m, first, second, num_sibs;
  double f, sum, xx[4];
  node *q;

  if ((ally[n] == 0) || (location[ally[n]-1] == 0))
    putc('.', outfile);
  else {
    j = location[ally[n]-1] - 1;
    for (i = 0; i < 4; i++) {
      f = p->x[j][mx-1][i];
      num_sibs = count_sibs(p);
      q = p;
      for (k = 0; k < num_sibs; k++) {
        q = q->next;
        f *= q->x[j][mx-1][i];
      }
      f = sqrt(f);
      xx[i] = f;
    }
    xx[0] *= freqa;
    xx[1] *= freqc;
    xx[2] *= freqg;
    xx[3] *= freqt;
    sum = xx[0]+xx[1]+xx[2]+xx[3];
    for (i = 0; i < 4; i++)
      xx[i] /= sum;
    first = 0;
    for (i = 1; i < 4; i++)
      if (xx [i] > xx[first])
        first = i;
    if (first == 0)
      second = 1;
    else
      second = 0;
    for (i = 0; i < 4; i++)
      if ((i != first) && (xx[i] > xx[second]))
        second = i;
    m = 1 << first;
    if (xx[first] < 0.4999995)
      m = m + (1 << second);
    if (xx[first] > 0.95)
      putc(toupper((int)basechar[m - 1]), outfile);
    else
      putc(basechar[m - 1], outfile);
    if (rctgry && rcategs > 1)
      mx = mp[n][mx - 1];    
    else
      mx = 1;
  }
} /* reconstr */


void rectrav(node *p, long m, long n)
{
  /* print out segment of reconstructed sequence for one branch */
  long num_sibs, i;
  node *sib_ptr;

  putc(' ', outfile);
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index-1][i], outfile);
  } else
    fprintf(outfile, "%4ld      ", p->index - spp);
  fprintf(outfile, "  ");
  mx = mx0;
  for (i = m; i <= n; i++) {
    if ((i % 10 == 0) && (i != m))
      putc(' ', outfile);
    if (p->tip)
      putc(y[p->index-1][i], outfile);
    else
      reconstr(p, i);
  }
  putc('\n', outfile);
  if (!p->tip) {
    num_sibs = count_sibs(p);
    sib_ptr = p;
    for (i = 0; i < num_sibs; i++) {
      sib_ptr = sib_ptr->next;
      rectrav(sib_ptr->back, m, n);
    }
  }
  mx1 = mx;
}  /* rectrav */


void summarize()
{
  long i, j, mm=0;
  double mode, sum;
  double like[maxcategs], nulike[maxcategs];
  double **marginal;

  mp = (long **)Malloc(sites * sizeof(long *));
  for (i = 0; i <= sites-1; ++i)
    mp[i] = (long *)Malloc(sizeof(long)*rcategs);
  fprintf(outfile, "\nLn Likelihood = %11.5f\n\n", curtree.likelihood);
  fprintf(outfile, " Ancestor      Node      Node Height     Length\n");
  fprintf(outfile, " --------      ----      ---- ------     ------\n");
  describe(curtree.root);
  putc('\n', outfile);
  if (rctgry && rcategs > 1) {
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = sites - 1; i >= 0; i--) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (lambda1 + lambda * probcat[j]) * like[j];
        mp[i][j] = j + 1;
        for (k = 1; k <= rcategs; k++) {
          if (k != j + 1) {
            if (lambda * probcat[k - 1] * like[k - 1] > nulike[j]) {
              nulike[j] = lambda * probcat[k - 1] * like[k - 1];
              mp[i][j] = k;
            }
          }
        }
        if ((ally[i] > 0) && (location[ally[i]-1] > 0))
          nulike[j] *= contribution[location[ally[i] - 1] - 1][j];
        sum += nulike[j];
      }
      for (j = 0; j < rcategs; j++)
        nulike[j] /= sum;
      memcpy(like, nulike, rcategs * sizeof(double));
    }
    mode = 0.0;
    mx = 1;
    for (i = 1; i <= rcategs; i++) {
      if (probcat[i - 1] * like[i - 1] > mode) {
        mx = i;
        mode = probcat[i - 1] * like[i - 1];
      }
    }
    mx0 = mx;
    fprintf(outfile,
 "Combination of categories that contributes the most to the likelihood:\n\n");
    for (i = 1; i <= nmlngth + 3; i++)
      putc(' ', outfile);
    for (i = 1; i <= sites; i++) {
      fprintf(outfile, "%ld", mx);
      if (i % 10 == 0)
        putc(' ', outfile);
      if (i % 60 == 0 && i != sites) {
        putc('\n', outfile);
        for (j = 1; j <= nmlngth + 3; j++)
          putc(' ', outfile);
      }
      mx = mp[i - 1][mx - 1];
    }
    fprintf(outfile, "\n\n");
    marginal = (double **) Malloc( sites*sizeof(double *));
    for (i = 0; i < sites; i++)
      marginal[i] = (double *) Malloc( rcategs*sizeof(double));
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = sites - 1; i >= 0; i--) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (lambda1 + lambda * probcat[j]) * like[j];
        for (k = 1; k <= rcategs; k++) {
          if (k != j + 1)
              nulike[j] += lambda * probcat[k - 1] * like[k - 1];
        }
        if ((ally[i] > 0) && (location[ally[i]-1] > 0))
          nulike[j] *= contribution[location[ally[i] - 1] - 1][j];
        sum += nulike[j];
      }
      for (j = 0; j < rcategs; j++) {
        nulike[j] /= sum;
        marginal[i][j] = nulike[j];
      }
      memcpy(like, nulike, rcategs * sizeof(double));
    }
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = 0; i < sites; i++) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (lambda1 + lambda * probcat[j]) * like[j];
        for (k = 1; k <= rcategs; k++) {
          if (k != j + 1)
              nulike[j] += lambda * probcat[k - 1] * like[k - 1];
        }
        marginal[i][j] *= like[j] * probcat[j];
        sum += nulike[j];
      }
      for (j = 0; j < rcategs; j++)
        nulike[j] /= sum;
      memcpy(like, nulike, rcategs * sizeof(double));
      sum = 0.0;
      for (j = 0; j < rcategs; j++)
        sum += marginal[i][j];
      for (j = 0; j < rcategs; j++)
        marginal[i][j] /= sum;
    }
    fprintf(outfile, "Most probable category at each site if > 0.95 probability (\".\" otherwise)\n\n");
    for (i = 1; i <= nmlngth + 3; i++)
      putc(' ', outfile);
    for (i = 0; i < sites; i++) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++)
        if (marginal[i][j] > sum) {
          sum = marginal[i][j];
          mm = j;
        }
        if (sum >= 0.95)
        fprintf(outfile, "%ld", mm+1);
      else
        putc('.', outfile);
      if ((i+1) % 60 == 0) {
        if (i != 0) {
          putc('\n', outfile);
          for (j = 1; j <= nmlngth + 3; j++)
            putc(' ', outfile);
        }
      }
      else if ((i+1) % 10 == 0)
        putc(' ', outfile);
    }
    putc('\n', outfile);
    for (i = 0; i < sites; i++)
      free(marginal[i]);
    free(marginal);
  }
  putc('\n', outfile);
  putc('\n', outfile);
  if (hypstate) {
    fprintf(outfile, "Probable sequences at interior nodes:\n\n");
    fprintf(outfile, "  node       ");
    for (i = 0; (i < 13) && (i < ((sites + (sites-1)/10 - 39) / 2)); i++)
      putc(' ', outfile);
    fprintf(outfile, "Reconstructed sequence (caps if > 0.95)\n\n");
    if (!rctgry || (rcategs == 1))
      mx0 = 1;
    for (i = 0; i < sites; i += 60) {
      k = i + 59;
      if (k >= sites)
        k = sites - 1;
      rectrav(curtree.root, i, k);
      putc('\n', outfile);
      mx0 = mx1;
    }
  }
  for (i = 0; i < sites; ++i)
    free(mp[i]);
  free(mp);
}  /* summarize */


void dnamlk_treeout(node *p)
{
  /* write out file with representation of final tree */
  node *sib_ptr;
  long i, n, w, num_sibs;
  Char c;
  double x;

  if (p->tip) {
    n = 0;
    for (i = 1; i <= nmlngth; i++) {
      if (nayme[p->index - 1][i - 1] != ' ')
        n = i;
    }
    for (i = 0; i < n; i++) {
      c = nayme[p->index - 1][i];
      if (c == ' ')
        c = '_';
      putc(c, outtree);
    }
    col += n;
  } else {
    sib_ptr = p;
    num_sibs = count_sibs(p);
    putc('(', outtree);
    col++;

    for (i=0; i < (num_sibs - 1); i++) {
      sib_ptr = sib_ptr->next;
      dnamlk_treeout(sib_ptr->back);
      putc(',', outtree);
      col++;
      if (col > 55) {
        putc('\n', outtree);
        col = 0;
      }
    }
    sib_ptr = sib_ptr->next;
    dnamlk_treeout(sib_ptr->back);
    putc(')', outtree);
    col++;
  }
  if (p == curtree.root) {
    fprintf(outtree, ";\n");
    return;
  }
  x = fracchange * (p->tyme - curtree.nodep[p->back->index - 1]->tyme);
  if (x > 0.0)
    w = (long)(0.4342944822 * log(x));
  else if (x == 0.0)
    w = 0;
  else
    w = (long)(0.4342944822 * log(-x)) + 1;
  if (w < 0)
    w = 0;
  fprintf(outtree, ":%*.5f", (int)(w + 7), x);
  col += w + 8;
}  /* dnamlk_treeout */


void nodeinit(node *p)
{
  /* set up times at one node */
  node *sib_ptr, *sib_back_ptr;
  long i, num_sibs;
  double lowertyme;

  sib_ptr = p;
  num_sibs = count_sibs(p);

  /* lowertyme = lowest of children's times */
  lowertyme = p->next->back->tyme;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    if (sib_back_ptr->tyme < lowertyme)
      lowertyme = sib_back_ptr->tyme;
  }

  p->tyme = lowertyme - 0.1;

  sib_ptr = p;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;

    sib_ptr->tyme = p->tyme;
    sib_back_ptr->v = sib_back_ptr->tyme - p->tyme;
    sib_ptr->v = sib_back_ptr->v;
  }
}  /* nodeinit */


void initrav(node *p)
{

  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;

  /* traverse to set up times throughout tree */
  if (p->tip)
    return;

  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    initrav(sib_back_ptr);
  }

  nodeinit(p);
}  /* initrav */


void travinit(node *p)
{
  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;

  /* traverse to set up initial values */
  if (p == NULL)
    return;
  if (p->tip)
    return;
  if (p->initialized)
    return;

  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    travinit(sib_back_ptr);
  }

  nuview(p);
  p->initialized = true;
}  /* travinit */


void travsp(node *p)
{
  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;

  /* traverse to find tips */
  if (p == curtree.root)
    travinit(p);
  if (p->tip)
    travinit(p->back);
  else {
    sib_ptr = p;
    num_sibs = count_sibs(p);
    for (i=0 ; i < num_sibs; i++) {
      sib_ptr      = sib_ptr->next;
      sib_back_ptr = sib_ptr->back;
      travsp(sib_back_ptr);
    }
  }
}  /* travsp */


void treevaluate()
{
  /* evaluate likelihood of tree, after iterating branch lengths */
  long i, j,  num_sibs;
  node *sib_ptr;

  polishing = true;
  smoothit = true;
  if (lngths == 0 && usertree == 1) {
    for (i = 0; i < spp; i++)
        curtree.nodep[i]->initialized = false;
    for (i = spp; i < nonodes; i++) {
        sib_ptr = curtree.nodep[i];
        sib_ptr->initialized = false;
        num_sibs = count_sibs(sib_ptr);
        for (j=0 ; j < num_sibs; j++) {
        sib_ptr      = sib_ptr->next;
        sib_ptr->initialized = false;
        }
    }
    initrav(curtree.root);
    travsp(curtree.root);
  }
  for (i = 1; i <= smoothings * 4; i++)
    smooth(curtree.root);
  evaluate(curtree.root);
}  /* treevaluate */


void maketree()
{
  /* constructs a binary tree from the pointers in curtree.nodep,
     adds each node at location which yields highest likelihood
     then rearranges the tree for greatest likelihood */

  long i, j;
  double x;
  node *item, *nufork, *dummy, *q, *root=NULL;
  boolean succeded, dummy_haslengths, dummy_first, goteof;
  long nextnode;
  pointarray dummy_treenode=NULL;
  double oldbest;
  node *tmp;
  char* treestr;

  inittable();  

  if (!usertree) {
    for (i = 1; i <= spp; i++)
      enterorder[i - 1] = i;
    if (jumble)
      randumize(seed, enterorder);
    curtree.root = curtree.nodep[spp];
    curtree.root->back = NULL;
    for (i = 0; i < spp; i++)
       curtree.nodep[i]->back = NULL;
    for (i = spp; i < nonodes; i++) {
       q = curtree.nodep[i];
       q->back = NULL;
       while ((q = q->next) != curtree.nodep[i])
         q->back = NULL;
    }
    polishing = false;
    dnamlk_add(curtree.nodep[enterorder[0] - 1], curtree.nodep[enterorder[1] - 1],
        curtree.nodep[spp]);
    if (progress) {
      printf("\nAdding species:\n");
      writename(0, 2, enterorder);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
    lastsp = false;
    smoothit = false;
    for (i = 3; i <= spp; i++) {
      bestree.likelihood = UNDEFINED;
      bestyet = UNDEFINED;
      there = curtree.root;
      item = curtree.nodep[enterorder[i - 1] - 1];
      nufork = curtree.nodep[spp + i - 2];
      lastsp = (i == spp);
      addpreorder(curtree.root, item, nufork, true, true);
      dnamlk_add(there, item, nufork);
      like = evaluate(curtree.root);
      copy_(&curtree, &bestree, nonodes, rcategs);
      rearrange(&curtree.root);
      if (curtree.likelihood > bestree.likelihood) {
        copy_(&curtree, &bestree, nonodes, rcategs);
      }
      if (progress) {
        writename(i - 1, 1, enterorder);
#ifdef WIN32
        phyFillScreenColor();
#endif
      }
      if (lastsp && global) {
        if (progress) {
          printf("Doing global rearrangements\n");
          printf("  !");
          for (j = 1; j <= nonodes; j++)
            if ( j % (( nonodes / 72 ) + 1 ) == 0 )
              putchar('-');
          printf("!\n");
        }
        global2 = true;
        do {
          succeded = false;
          if (progress)
            printf("   ");
          save_tree_tyme(&curtree, tymes);
          for (j = 0; j < nonodes; j++) {
            oldbest = bestree.likelihood;
            bestyet = UNDEFINED;
            item = curtree.nodep[j];
            if (item != curtree.root) {
              nufork = curtree.nodep[curtree.nodep[j]->back->index - 1];
              
              if (nufork != curtree.root) {  
                tmp = nufork->next->back;
                if (tmp == item) 
                    tmp = nufork->next->next->back; 
                    /* can't figure out why we never get here */
              }
              else {
                  if (nufork->next->back != item)
                      tmp  = nufork->next->back;
                  else tmp = nufork->next->next->back;
              } /* if we add item at tmp we have done nothing */
              dnamlk_re_move(&item, &nufork, false);
              there = curtree.root;
              addpreorder(curtree.root, item, nufork, true, true);
              if ( tmp != there && bestree.likelihood > oldbest)
                succeded = true;
              dnamlk_add(there, item, nufork);
              restore_saved_tyme(&curtree,tymes);
            }
            if (progress) {
              if ( j % (( nonodes / 72 ) + 1 ) == 0 )
                putchar('.');
              fflush(stdout);
            }
          } 
          if (progress)
            putchar('\n');
        } while ( succeded );
      }
    }
    if (njumble > 1 && lastsp) {
      for (i = 0; i < spp; i++ )
        dnamlk_re_move(&curtree.nodep[i], &dummy, false);
      if (jumb == 1 || bestree2.likelihood < bestree.likelihood)
        copy_(&bestree, &bestree2, nonodes, rcategs);
    }
    if (jumb == njumble) {
      if (njumble > 1)
        copy_(&bestree2, &curtree, nonodes, rcategs);
      else copy_(&bestree, &curtree, nonodes, rcategs);
      fprintf(outfile, "\n\n");
      treevaluate();
      curtree.likelihood = evaluate(curtree.root);
      dnamlk_printree();
      summarize();
      if (trout) {
        col = 0;
        dnamlk_treeout(curtree.root);
      }
    } 
  } else {
    treestr = ajStrGetuniquePtr(&phylotrees[0]->Tree);
    inittable_for_usertree (treestr);
    if(numtrees > MAXSHIMOTREES)
      shimotrees = MAXSHIMOTREES;
    else
      shimotrees = numtrees;
    if (numtrees > 2)
      emboss_initseed(inseed, &inseed0, seed);
    l0gl = (double *)Malloc(shimotrees * sizeof(double));
    l0gf = (double **)Malloc(shimotrees * sizeof(double *));
    for (i=0; i < shimotrees; ++i)
      l0gf[i] = (double *)Malloc(endsite * sizeof(double));
    if (treeprint) {
      fprintf(outfile, "User-defined tree");
      if (numtrees > 1)
        putc('s', outfile);
      fprintf(outfile, ":\n\n");
    }
    fprintf(outfile, "\n\n");
    which = 1;
    while (which <= numtrees) {
      
      /* These initializations required each time through the loop
         since multiple trees require re-initialization */
      dummy_haslengths = true;
      nextnode         = 0;
      dummy_first      = true;
      goteof           = false;

      treestr = ajStrGetuniquePtr(&phylotrees[which-1]->Tree);
      treeread(&treestr, &root, dummy_treenode, &goteof, &dummy_first,
               curtree.nodep, &nextnode,
               &dummy_haslengths, &grbg, initdnamlnode);

      nonodes = nextnode;
      
      root = curtree.nodep[root->index - 1];
      curtree.root = root;

      if (lngths)
        tymetrav(curtree.root, &x);

      if (goteof && (which <= numtrees)) {
        /* if we hit the end of the file prematurely */
        printf ("\n");
        printf ("ERROR: trees missing at end of file.\n");
        printf ("\tExpected number of trees:\t\t%ld\n", numtrees);
        printf ("\tNumber of trees actually in file:\t%ld.\n\n", which - 1);
        exxit(-1);
      }
      curtree.start = curtree.nodep[0]->back;
      treevaluate();
      dnamlk_printree();
      summarize();
      if (trout) {
        col = 0;
        dnamlk_treeout(curtree.root);
      }
      if(which < numtrees){
        freex_notip(nonodes, curtree.nodep);
        gdispose(curtree.root, &grbg, curtree.nodep);
      }
      which++;
    }      

    FClose(intree);
    if (!auto_ && numtrees > 1 && weightsum > 1 )
      standev2(numtrees, maxwhich, 0, endsite, maxlogl, l0gl, l0gf,
               aliasweight, seed);
  }
  
  if (jumb == njumble) {
    if (progress) {
      printf("\nOutput written to file \"%s\"\n\n", outfilename);
      if (trout)
        printf("Tree also written onto file \"%s\"\n\n", outtreename);
    }
    free(contribution);
    freex(nonodes, curtree.nodep);
    if (!usertree) {
      freex(nonodes, bestree.nodep);
      if (njumble > 1)
        freex(nonodes, bestree2.nodep);
    }
  }
  free(root);
} /* maketree */

/*?? Dnaml has a clean-up function for freeing memory, closing files, etc.
     Put one here too? */

int main(int argc, Char *argv[])
{  /* DNA Maximum Likelihood with molecular clock */

#ifdef MAC
  argc = 1;                /* macsetup("Dnamlk", "Dnamlk");        */
  argv[0] = "Dnamlk";
#endif
  init(argc,argv);
  emboss_getoptions("fdnamlk", argc, argv);
  progname = argv[0];

  ibmpc = IBMCRT;
  ansi = ANSICRT;
  firstset = true;
  doinit();

  ttratio0    = ttratio;

  for (ith = 1; ith <= datasets; ith++) {
    ttratio = ttratio0;
    if (datasets > 1) {
      fprintf(outfile, "Data set # %ld:\n\n", ith);
      if (progress)
        printf("\nData set # %ld:\n", ith);
    }
    getinput();
    if (ith == 1)
      firstset = false;
    for (jumb = 1; jumb <= njumble; jumb++)
      maketree();
  }
  FClose(infile);  
  FClose(outfile);
  FClose(outtree);
#ifdef MAC
  fixmacfile(outfilename);
  fixmacfile(outtreename);
#endif
  printf("Done.\n\n");
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  return 0;
}  /* DNA Maximum Likelihood with molecular clock */
