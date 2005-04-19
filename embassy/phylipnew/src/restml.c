
/* version 3.6. (c) Copyright 1993-2002 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#include "phylip.h"
#include "seq.h"

#define initialv        0.1     /* starting value of branch length          */

#define over            60   /* maximum width of a tree on screen */

extern sequence y;

AjPPhyloProp phyloweights = NULL;
AjPPhyloTree* phylotrees;
AjPPhyloState* phylostates = NULL;

#ifndef OLDC
/* function prototypes */
void   restml_inputnumbers(AjPPhyloState);
//void   getoptions(void);
void   emboss_getoptions(char *pgm, int argc, char *argv[]);
void   allocrest(void);
void   setuppie(void);
void   doinit(void);
void   inputoptions(AjPPhyloState);
void   restml_inputdata(AjPPhyloState);
void   restml_sitesort(void);
void   restml_sitecombine(void);
void   makeweights(void);

void   restml_makevalues(void);
void   getinput(void);
void   copymatrix(transmatrix, transmatrix);
void   maketrans(double, boolean);
void   branchtrans(long, double);
double evaluate(tree *, node *);
void   nuview(node *);
void   makenewv(node *);
void   update(node *);
void   smooth(node *);

void   insert_(node *p, node *);
void   restml_re_move(node **, node **);
void   restml_copynode(node *, node *);
void   restml_copy_(tree *, tree *);
void   buildnewtip(long , tree *);
void   buildsimpletree(tree *);
void   addtraverse(node *, node *, boolean);
void   rearrange(node *, node *);
void   restml_coordinates(node *, double, long *,double *, double *);
void   restml_printree(void);

double sigma(node *, double *);
void   describe(node *);
void   summarize(void);
void   restml_treeout(node *);
void   inittravtree(node *);
void   treevaluate(void);
void   maketree(void);
void   globrearrange(void); 
/* function prototypes */
#endif


Char infilename[FNMLNGTH], intreename[FNMLNGTH];

const char* outfilename;
const char* outtreename;
AjPFile embossoutfile;
AjPFile embossouttree;


ajint numwts;

long nonodes2, sites, enzymes, weightsum, sitelength, datasets, 
        ith, njumble, jumb=0;
long inseed, inseed0;
boolean  global, jumble, lengths, weights, trout, trunc8, usertree,
           progress, mulsets, firstset, improve, smoothit;
double bestyet;
tree curtree, priortree, bestree, bestree2;
longer seed;
long *enterorder;
steptr aliasweight;
char *progname;
node *qwhere,*addwhere;

/* Local variables for maketree, propagated globally for C version: */
long       nextsp, numtrees, maxwhich, col;
double      maxlogl;
boolean     succeeded, smoothed;
transmatrix tempmatrix, temp2matrix, temp3matrix,
              temp4matrix, temp5matrix, tempslope, tempcurve;
sitelike2    pie;
double      *l0gl;
double     **l0gf;
Char ch;

/* variables added to keep treeread2() happy */
boolean goteof;
double trweight;
boolean haslengths;


void restml_inputnumbers(AjPPhyloState state)
{
  /* read and print out numbers of species and sites */

  spp = state->Size;
  sites = state->Len;
  enzymes = state->Count;

  nonodes2 = spp * 2 - 2;
}  /* restml_inputnumbers */


void   emboss_getoptions(char *pgm, int argc, char *argv[])
{

  AjStatus retval;
  ajint numseqs;

  boolean rough;
  
  sitelength = 6;
  trunc8 = true;
  global = false;
  improve = false;
  jumble = false;
  njumble = 1;
  lengths = false;
  outgrno = 1;
  outgropt = false;
  trout = true;
  usertree = false;
  weights = false;
  printdata = false;
  progress = true;
  treeprint = true;
  interleaved = true;
  mulsets = false;
  datasets = 1;

    ajNamInit("emboss");
    retval = ajAcdInitP (pgm, argc, argv, "PHYLIP");

    phylostates = ajAcdGetDiscretestates("data");

    numseqs = 0;
    while (phylostates[numseqs])
	numseqs++;

    printf("numseqs: %d\n", numseqs);

    if (numseqs > 1) {
	mulsets = true;
        datasets = numseqs;
    }

    phylotrees = ajAcdGetTree("intreefile");
    if (phylotrees)
    {
        numtrees = 0;
        while (phylotrees[numtrees])
            numtrees++;
        usertree = true;
    }

    numwts = 0;

    phyloweights = ajAcdGetProperties("weights");
    if (phyloweights) weights = true;




    trunc8 = ajAcdGetBool("allsites");
    
 
    if (!usertree) {
      rough = ajAcdGetBool("rough");
      if(!rough) improve = true;  
      global = ajAcdGetBool("global");
      njumble = ajAcdGetInt("njumble");
      if(njumble >0) {
        inseed = ajAcdGetInt("seed");
        jumble = true; 
        emboss_initseed(inseed, &inseed0, seed);
      }
      else njumble = 1;
    }

    sitelength = ajAcdGetInt("sitelength");
    outgrno = ajAcdGetInt("outgrno");
    if(outgrno != 0) outgropt = true;
    else outgrno = 1;

    printdata = ajAcdGetBool("printdata");
    progress = ajAcdGetBool("progress");
    treeprint = ajAcdGetBool("treeprint");
    trout = ajAcdGetToggle("trout");

     embossoutfile = ajAcdGetOutfile("outfile");   
     emboss_openfile(embossoutfile, &outfile, &outfilename);
     

     if(trout) {
     embossouttree = ajAcdGetOutfile("outtreefile");
     emboss_openfile(embossouttree, &outtree, &outtreename);
     }
 
  fprintf(outfile, "\nRestriction site Maximum Likelihood");
  fprintf(outfile, " method, version %s\n\n",VERSION);

}  /* emboss_getoptions */


void allocrest()
{
  long i;

  y = (Char **)Malloc(spp*sizeof(Char *));
  for (i = 0; i < spp; i++)
    y[i] = (Char *)Malloc(sites*sizeof(Char));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  enterorder = (long *)Malloc(spp*sizeof(long));
  weight = (steptr)Malloc((sites+1)*sizeof(long));
  alias = (steptr)Malloc((sites+1)*sizeof(long));
  aliasweight = (steptr)Malloc((sites+1)*sizeof(long));
}  /* allocrest */


void setuppie()
{
  /* set up equilibrium probabilities of being a given
     number of bases away from a restriction site */
  long i;
  double sum;

  pie[0] = 1.0;
  sum = pie[0];
  for (i = 1; i <= sitelength; i++) {
    pie[i] = 3 * pie[i - 1] * (sitelength - i + 1) / i;
    sum += pie[i];
  }
  for (i = 0; i <= sitelength; i++)
    pie[i] /= sum;
}  /* setuppie */


void doinit()
{
  /* initializes variables */
  long i;

  restml_inputnumbers(phylostates[0]);
  if (printdata)
    fprintf(outfile, "%4ld Species, %4ld Sites,%4ld Enzymes\n",
            spp, sites, enzymes);
  tempmatrix = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; i++)
    tempmatrix[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  temp2matrix = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; i++)
    temp2matrix[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  temp3matrix = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; i++)
    temp3matrix[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  temp4matrix = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; i++)
    temp4matrix[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  temp5matrix = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; i++)
    temp5matrix[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  tempslope = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; i++)
    tempslope[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  tempcurve = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; i++)
    tempcurve[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  setuppie();
  alloctrans(&curtree.trans, nonodes2, sitelength);
  alloctree(&curtree.nodep, nonodes2, false);
  allocrest();
  if (usertree)
    return;
  alloctrans(&bestree.trans, nonodes2, sitelength);
  alloctree(&bestree.nodep, nonodes2, 0);
  alloctrans(&priortree.trans, nonodes2, sitelength);
  alloctree(&priortree.nodep, nonodes2, 0);
  if (njumble == 1) return;
  alloctrans(&bestree2.trans, nonodes2, sitelength);
  alloctree(&bestree2.nodep, nonodes2, 0);
}  /* doinit */


void inputoptions(AjPPhyloState state)
{
  /* read the options information */
  
  long i, extranum, cursp, curst, curenz;

  if (!firstset) {
    cursp = state->Size;
    curst = state->Len;
    curenz = state->Count;

    if (cursp != spp) {
      printf("\nERROR: INCONSISTENT NUMBER OF SPECIES IN DATA SET %4ld\n",
             ith);
      exxit(-1);
    }
    if (curenz != enzymes) {
      printf("\nERROR: INCONSISTENT NUMBER OF ENZYMES IN DATA SET %4ld\n",
             ith);
      exxit(-1);
    }
    sites = curst;
  }
  for (i = 1; i <= sites; i++)
    weight[i] = 1;
  weightsum = sites;
  extranum = numwts;
  for (i = 1; i <= numwts; i++) {
      inputweightsstr2(phyloweights->Str[i-1], 1, sites+1, &weightsum, weight, &weights, "RESTML");
  }
  fprintf(outfile, "\n  Recognition sequences all%2ld bases long\n",
          sitelength);
  if (trunc8)
    fprintf(outfile,
      "\nSites absent from all species are assumed to have been omitted\n\n");
  if (weights)
    printweights(outfile, 1, sites, weight, "Sites");
}  /* inputoptions */


void restml_inputdata(AjPPhyloState state)
{
  /* read the species and sites data */
  long i, j, k, l, sitesread;
  Char ch;
  boolean allread, done;
  AjPStr str;

  if (printdata)
    putc('\n', outfile);
  j = nmlngth + (sites + (sites - 1) / 10) / 2 - 5;
  if (j < nmlngth - 1)
    j = nmlngth - 1;
  if (j > 39)
    j = 39;
  if (printdata) {
    fprintf(outfile, "Name");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "Sites\n");
    fprintf(outfile, "----");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "-----\n\n");
  }
  sitesread = 0;
  allread = false;
  while (!(allread)) {
    i = 1;
    while (i <= spp ) {
      str = state->Str[i-1];
      initnamestate(state, i - 1);     
      j = 0;
      done = false;
      while (!done) {
        while (j < sites) {
          ch = ajStrChar(str, j);
          uppercase(&ch);
          if (ch != '1' && ch != '0' && ch != '+' && ch != '-' && ch != '?') {
            printf(" ERROR: Bad symbol %c", ch);
            printf(" at position %ld of species %ld\n", j+1, i);
            exxit(-1);
          }
          if (ch == '1')
            ch = '+';
          if (ch == '0')
            ch = '-';
          j++;
          y[i - 1][j - 1] = ch;
        }
        if (j == sites)
          done = true;
      }
      i++;
    }
    allread = (i > spp);
  }
  if (printdata) {
    for (i = 1; i <= ((sites - 1) / 60 + 1); i++) {
      for (j = 0; j < spp; j++) {
        for (k = 0; k < nmlngth; k++)
          putc(nayme[j][k], outfile);
        fprintf(outfile, "   ");
        l = i * 60;
        if (l > sites)
          l = sites;
        for (k = (i - 1) * 60 + 1; k <= l; k++) {
          putc(y[j][k - 1], outfile);
          if (k % 10 == 0 && k % 60 != 0)
            putc(' ', outfile);
        }
        putc('\n', outfile);
      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  putc('\n', outfile);
}  /* restml_inputdata */


void restml_sitesort()
{
  /* Shell sort keeping alias, aliasweight in same order */
  long gap, i, j, jj, jg, k, itemp;
  boolean flip, tied;

  gap = sites / 2;
  while (gap > 0) {
    for (i = gap + 1; i <= sites; i++) {
      j = i - gap;
      flip = true;
      while (j > 0 && flip) {
        jj = alias[j];
        jg = alias[j + gap];
        flip = false;
        tied = true;
        k = 1;
        while (k <= spp && tied) {
          flip = (y[k - 1][jj - 1] > y[k - 1][jg - 1]);
          tied = (tied && y[k - 1][jj - 1] == y[k - 1][jg - 1]);
          k++;
        }
        if (tied) {
          aliasweight[j] += aliasweight[j + gap];
          aliasweight[j + gap] = 0;
        }
        if (!flip)
          break;
        itemp = alias[j];
        alias[j] = alias[j + gap];
        alias[j + gap] = itemp;
        itemp = aliasweight[j];
        aliasweight[j] = aliasweight[j + gap];
        aliasweight[j + gap] = itemp;
        j -= gap;
      }
    }
    gap /= 2;
  }
}  /* restml_sitesort */


void restml_sitecombine()
{
  /* combine sites that have identical patterns */
  long i, j, k;
  boolean tied;

  i = 1;
  while (i < sites) {
    j = i + 1;
    tied = true;
    while (j <= sites && tied) {
      k = 1;
      while (k <= spp && tied) {
        tied = (tied && y[k - 1][alias[i] - 1] == y[k - 1][alias[j] - 1]);
        k++;
      }
      if (tied && aliasweight[j] > 0) {
        aliasweight[i] += aliasweight[j];
        aliasweight[j] = 0;
        alias[j] = alias[i];
      }
      j++;
    }
    i = j - 1;
  }
}  /* restml_sitecombine */


void makeweights()
{
  /* make up weights vector to avoid duplicate computations */
  long i;

  for (i = 1; i <= sites; i++) {
    alias[i] = i;
    aliasweight[i] = weight[i];
  }
  restml_sitesort();
  restml_sitecombine();
  sitescrunch2(sites + 1, 2, 3, aliasweight);
  for (i = 1; i <= sites; i++) {
    weight[i] = aliasweight[i];
    if (weight[i] > 0)
      endsite = i;
  }
  weight[0] = 1;
}  /* makeweights */


void restml_makevalues()
{
  /* set up fractional likelihoods at tips */
  long i, j, k, l, m;
  boolean found;

  for (k = 1; k <= endsite; k++) {
    j = alias[k];
    for (i = 0; i < spp; i++) {
      for (l = 0; l <= sitelength; l++)
        curtree.nodep[i]->x2[k][l] = 1.0;
      switch (y[i][j - 1]) {

      case '+':
        for (m = 1; m <= sitelength; m++)
          curtree.nodep[i]->x2[k][m] = 0.0;
        break;

      case '-':
        curtree.nodep[i]->x2[k][0] = 0.0;
        break;

      case '?':
        /* blank case */
        break;
      }
    }
  }
  for (i = 0; i < spp; i++) {
    for (k = 1; k <= sitelength; k++)
      curtree.nodep[i]->x2[0][k] = 1.0;
    curtree.nodep[i]->x2[0][0] = 0.0;
  }
  if (trunc8)
    return;
  found = false;
  i = 1;
  while (!found && i <= endsite) {
    found = true;
    for (k = 0; k < spp; k++)
      found = (found && y[k][alias[i] - 1] == '-');
    if (!found)
      i++;
  }
  if (found) {
    weightsum += (enzymes - 1) * weight[i];
    weight[i] *= enzymes;
  }
}  /* restml_makevalues */


void getinput()
{
  /* reads the input data */
  inputoptions(phylostates[ith-1]);
  restml_inputdata(phylostates[ith-1]);
  makeweights();
  setuptree2(curtree);
  if (!usertree) {
    setuptree2(priortree);
    setuptree2(bestree);
    if (njumble > 1) 
      setuptree2(bestree2);
  }
  allocx2(nonodes2, endsite+1, sitelength, curtree.nodep, false);
  if (!usertree) {
    allocx2(nonodes2, endsite+1, sitelength, priortree.nodep, 0);
    allocx2(nonodes2, endsite+1, sitelength, bestree.nodep, 0);
    if (njumble > 1)
      allocx2(nonodes2, endsite+1, sitelength, bestree2.nodep, 0);
  }
  restml_makevalues();
}  /* getinput */


void copymatrix(transmatrix tomat, transmatrix frommat)
{
  /* copy a matrix the size of transition matrix */
  int i,j;

  for (i=0;i<=sitelength;++i){
    for (j=0;j<=sitelength;++j)
      tomat[i][j] = frommat[i][j];
  }
} /* copymatrix */


void maketrans(double p, boolean nr)
{
  /* make transition matrix, product matrix with change
     probability p.  Put the results in tempmatrix, tempslope, tempcurve */
  long i, j, k, m1, m2;
  double sump, sums=0, sumc=0, pover3, pijk, term;
  double binom1[maxcutter + 1], binom2[maxcutter + 1];

  pover3 = p / 3;
  for (i = 0; i <= sitelength; i++) {
    if (p > 1.0 - epsilon)
      p = 1.0 - epsilon;
    if (p < epsilon)
      p = epsilon;
    binom1[0] = exp((sitelength - i) * log(1 - p));
    for (k = 1; k <= sitelength - i; k++)
      binom1[k] = binom1[k - 1] * (p / (1 - p)) * (sitelength - i - k + 1) / k;
    binom2[0] = exp(i * log(1 - pover3));
    for (k = 1; k <= i; k++)
      binom2[k] = binom2[k - 1] * (pover3 / (1 - pover3)) * (i - k + 1) / k;
    for (j = 0; j <= sitelength; ++j) {
      sump = 0.0;
      if (nr) {
        sums = 0.0;
        sumc = 0.0;
      }
      if (i - j > 0)
        m1 = i - j;
      else
        m1 = 0;
      if (sitelength - j < i)
        m2 = sitelength - j;
      else
        m2 = i;
      for (k = m1; k <= m2; k++) {
        pijk = binom1[j - i + k] * binom2[k];
        sump += pijk;
        if (nr) {
          term = (j-i+2*k)/p - (sitelength-j-k)/(1.0-p) - (i-k)/(3.0-p);
          sums += pijk * term;
          sumc += pijk * (term * term
                            - (j-i+2*k)/(p*p)
                            - (sitelength-j-k)/((1.0-p)*(1.0-p))
                            - (i-k)/((3.0-p)*(3.0-p)) );
        }
      }
      tempmatrix[i][j] = sump;
      if (nr) {
        tempslope[i][j] = sums;
        tempcurve[i][j] = sumc;
      }
    }
  }
}  /* maketrans */


void branchtrans(long i, double p)
{
  /* make branch transition matrix for branch i with probability of change p */
  boolean nr;

  nr = false;
  maketrans(p, nr);
  copymatrix(curtree.trans[i - 1], tempmatrix);
}  /* branchtrans */


double evaluate(tree *tr, node *p)
{
  /* evaluates the likelihood, using info. at one branch */
  double sum, sum2, y, liketerm, like0, lnlike0=0, term;
  long i, j, k;
  node *q;
  sitelike2 x1, x2;
  boolean nr;

  sum = 0.0;
  q = p->back;
  y = p->v;
  nr = false;
  maketrans(y, nr);
  memcpy(x1, p->x2[0], sizeof(sitelike2));
  memcpy(x2, q->x2[0], sizeof(sitelike2));
  if (trunc8) {
    like0 = 0.0;
    for (j = 0; j <= sitelength; j++) {
      liketerm = pie[j] * x1[j];
      for (k = 0; k <= sitelength; k++)
        like0 += liketerm * tempmatrix[j][k] * x2[k];
    }
    lnlike0 = log(enzymes * (1.0 - like0));
  }
  for (i = 1; i <= endsite; i++) {
    memcpy(x1, p->x2[i], sizeof(sitelike2));
    memcpy(x2, q->x2[i], sizeof(sitelike2));
    sum2 = 0.0;
    for (j = 0; j <= sitelength; j++) {
      liketerm = pie[j] * x1[j];
      for (k = 0; k <= sitelength; k++)
        sum2 += liketerm * tempmatrix[j][k] * x2[k];
    }
    term = log(sum2);
    if (trunc8)
      term -= lnlike0;
    if (usertree)
      l0gf[which - 1][i - 1] = term;
    sum += weight[i] * term;
  }
/* *** debug  put a variable "saveit" in evaluate as third argument as to
   whether to save the KHT suff */
  if (usertree) {
      l0gl[which - 1] = sum;
    if (which == 1) {
      maxwhich = 1;
      maxlogl = sum;
    } else if (sum > maxlogl) {
      maxwhich = which;
      maxlogl = sum;
    }
  }
  tr->likelihood = sum;
  return sum;
}  /* evaluate */


void nuview(node *p)
{
  /* recompute fractional likelihoods for one part of tree */
  long i, j, k, lowlim;
  double sumq, sumr;
  node *q, *r;
  sitelike2 xq, xr, xp;
  transmatrix tempq, tempr;
  double *tq, *tr;

  if (!p->next->back->tip && !p->next->back->initialized)
    nuview (p->next->back);
  if (!p->next->next->back->tip && !p->next->next->back->initialized)
    nuview (p->next->next->back);
  tempq = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  tempr = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0;i<=sitelength;++i){
    tempq[i] = (double *)Malloc((sitelength+1) * sizeof (double));
    tempr[i] = (double *)Malloc((sitelength+1) * sizeof (double));
  }
  if (trunc8)
    lowlim = 0;
  else
    lowlim = 1;
  q = p->next->back;
  r = p->next->next->back;
  copymatrix(tempq,curtree.trans[q->branchnum - 1]);
  copymatrix(tempr,curtree.trans[r->branchnum - 1]);
  for (i = lowlim; i <= endsite; i++) {
    memcpy (xq, q->x2[i], sizeof(sitelike2));
    memcpy (xr, r->x2[i], sizeof(sitelike2));
    for (j = 0; j <= sitelength; j++) {
      sumq = 0.0;
      sumr = 0.0;
      tq = tempq[j];
      tr = tempr[j];
      for (k = 0; k <= sitelength; k++) {
        sumq += tq[k] * xq[k];
        sumr += tr[k] * xr[k];
      }
      xp[j] = sumq * sumr;
    }
    memcpy(p->x2[i], xp, sizeof(sitelike2));
  }
  for (i=0;i<=sitelength;++i){
    free(tempq[i]);
    free(tempr[i]);
  }
  free(tempq);
  free(tempr);
  p->initialized = true;
}  /* nuview */


void makenewv(node *p)
{
  /* Newton-Raphson algorithm improvement of a branch length */
  long i, j, k, lowlim, it, ite;
  double sum, sums, sumc, like, slope, curve, liketerm, liket,
         y, yold=0, yorig, like0=0, slope0=0, curve0=0, oldlike=0, temp;
  boolean done, nr, firsttime, better;
  node *q;
  sitelike2 xx1, xx2;
  double *tm, *ts, *tc;

  q = p->back;
  y = p->v;
  yorig = y;
  if (trunc8)
    lowlim = 0;
  else
    lowlim = 1;
  done = false;
  nr = true;
  firsttime = true;
  it = 1;
  ite = 0;
  while ((it < iterations) && (ite < 20) && (!done)) {
    like = 0.0;
    slope = 0.0;
    curve = 0.0;
    maketrans(y, nr);
    for (i = lowlim; i <= endsite; i++) {
      memcpy(xx1, p->x2[i], sizeof(sitelike2));
      memcpy(xx2, q->x2[i], sizeof(sitelike2));
      sum = 0.0;
      sums = 0.0;
      sumc = 0.0;
      for (j = 0; j <= sitelength; j++) {
        liket = xx1[j] * pie[j];
        tm = tempmatrix[j];
        ts = tempslope[j];
        tc = tempcurve[j];
        for (k = 0; k <= sitelength; k++) {
          liketerm = liket * xx2[k];
          sum += tm[k] * liketerm;
          sums += ts[k] * liketerm;
          sumc += tc[k] * liketerm;
        }
      }
      if (i == 0) {
        like0 = sum;
        slope0 = sums;
        curve0 = sumc;
      } else {
        like += weight[i] * log(sum);
        slope += weight[i] * sums/sum;
        temp = sums/sum;
        curve += weight[i] * (sumc/sum-temp*temp);
      }
    }
    if (trunc8 && fabs(like0 - 1.0) > 1.0e-10) {
      like -= weightsum * log(enzymes * (1.0 - like0));
      slope += weightsum * slope0 /(1.0 - like0);
      curve += weightsum * (curve0 /(1.0 - like0)
                            + slope0*slope0/((1.0 - like0)*(1.0 - like0)));
    }
    better = false;
    if (firsttime) {
      yold = y;
      oldlike = like;
      firsttime = false;
      better = true;
    } else {
      if (like > oldlike) {
        yold = y;
        oldlike = like;
        better = true;
        it++;
      }
    }
    if (better) {
      y = y + slope/fabs(curve);
      if (y < epsilon)
        y = 10.0 * epsilon;
      if (y > 0.75)
        y = 0.75;
    } else {
        if (fabs(y - yold) < epsilon)
          ite = 20;
        y = (y + yold) / 2.0;
    }
    ite++;
    done = fabs(y-yold) < epsilon;
  }
  smoothed = (fabs(yold-yorig) < epsilon) && (yorig > 1000.0*epsilon);
  p->v = yold;
  q->v = yold;
  branchtrans(p->branchnum, yold);
  curtree.likelihood = oldlike;
}  /* makenewv */


void update(node *p)
{
  /* improve branch length and views for one branch */

  if (!p->tip && !p->initialized)
    nuview(p);
  if (!p->back->tip && !p->back->initialized)
    nuview(p->back);
  if (p->iter) {
    makenewv(p);
    if (!p->tip) {
      p->next->initialized = false;
      p->next->next->initialized = false;
    }
    if (!p->back->tip) {
      p->back->next->initialized = false;
      p->back->next->next->initialized = false;
    }
  }
}  /* update */


void smooth(node *p)
{
  /* update nodes throughout the tree, recursively */

  smoothed = false;
  update(p);
  if (!p->tip) {
    if (smoothit && !smoothed) {
      smooth(p->next->back);
      p->initialized = false;
      p->next->next->initialized = false;
    }
    if (smoothit && !smoothed) {
      smooth(p->next->next->back);
      p->initialized = false;
      p->next->initialized = false;
    }
  }
}  /* smooth */


void insert_(node *p, node *q)
{
  /* insert a subtree into a branch, improve lengths in tree */
  long i, m, n;
  node *r;

  r = p->next->next;
  hookup(r, q->back);
  hookup(p->next, q);
  if (q->v >= 0.75)
    q->v = 0.75;
  else
    q->v = 0.75 * (1 - sqrt(1 - 1.333333 * q->v));
  q->back->v = q->v;
  r->v = q->v;
  r->back->v = r->v;
  if (q->branchnum == q->index) {
    m = q->branchnum;
    n = r->index;
  } else {
    m = r->index;
    n = q->branchnum;
  }
  q->branchnum = m;
  q->back->branchnum = m;
  r->branchnum = n;
  r->back->branchnum = n;
  branchtrans(q->branchnum, q->v);
  branchtrans(r->branchnum, r->v);
  p->initialized = false;
  p->next->initialized = false;
  p->next->next->initialized = false;
  i = 1;
  while (i <= smoothings) {
    smooth(p);
    if (!smoothit) {
      if (!p->tip) {
        smooth (p->next->back);
        smooth (p->next->next->back);
      }
    }
    else 
      smooth(p->back);
    i++;
  }
}  /* insert */


void restml_re_move(node **p, node **q)
{
  /* remove p and record in q where it was */
  long i;

  *q = (*p)->next->back;
  hookup(*q, (*p)->next->next->back);
  (*q)->back->branchnum = (*q)->branchnum;
  branchtrans((*q)->branchnum, 0.75*(1 - (1 - 1.333333*(*q)->v)
                                        * (1 - 1.333333*(*p)->next->v)));
  (*p)->next->back = NULL;
  (*p)->next->next->back = NULL;
  if (!(*q)->tip) {
    (*q)->next->initialized = false;
    (*q)->next->next->initialized = false;
  }
  if (!(*q)->back->tip) {
    (*q)->back->next->initialized = false;
    (*q)->back->next->next->initialized = false;
  }
  i = 1;
  while (i <= smoothings) {
    smooth(*q);
    if (smoothit)
      smooth((*q)->back);
    i++;
  }
}  /* restml_re_move */


void restml_copynode(node *c, node *d)
{
  /* copy a node */

  d->branchnum = c->branchnum;
  memcpy(d->x2, c->x2, (endsite+1)*sizeof(sitelike2));
  d->v = c->v;
  d->iter = c->iter;
  d->xcoord = c->xcoord;
  d->ycoord = c->ycoord;
  d->ymin = c->ymin;
  d->ymax = c->ymax;
  d->initialized = c->initialized;
}  /* restml_copynode */


void restml_copy_(tree *a, tree *b)
{
  /* copy tree a to tree b */
  long i,j;
  node *p, *q;

  for (i = 0; i < spp; i++) {
    restml_copynode(a->nodep[i], b->nodep[i]);
    if (a->nodep[i]->back) {
      if (a->nodep[i]->back == a->nodep[a->nodep[i]->back->index - 1])
        b->nodep[i]->back = b->nodep[a->nodep[i]->back->index - 1];
      else if (a->nodep[i]->back
                == a->nodep[a->nodep[i]->back->index - 1]->next)
          b->nodep[i]->back = b->nodep[a->nodep[i]->back->index - 1]->next;
        else
          b->nodep[i]->back
                         = b->nodep[a->nodep[i]->back->index - 1]->next->next;
    }
    else b->nodep[i]->back = NULL;
  }
  for (i = spp; i < nonodes2; i++) {
    p = a->nodep[i];
    q = b->nodep[i];
    for (j = 1; j <= 3; j++) {
      restml_copynode(p, q);
      if (p->back) {
        if (p->back == a->nodep[p->back->index - 1])
          q->back = b->nodep[p->back->index - 1];
        else if (p->back == a->nodep[p->back->index - 1]->next)
          q->back = b->nodep[p->back->index - 1]->next;
        else
          q->back = b->nodep[p->back->index - 1]->next->next;
      }
      else
        q->back = NULL;
      p = p->next;
      q = q->next;
    }
  }
  b->likelihood = a->likelihood;
  for (i=0;i<nonodes2;++i)
    copymatrix(b->trans[i],a->trans[i]);
  b->start = a->start;
}  /* restml_copy */


void buildnewtip(long m, tree *tr)
{
  /* set up a new tip and interior node it is connected to */
  node *p;
  long i, j;

  p = tr->nodep[nextsp + spp - 3];
  for (i = 0; i < endsite; i++) {
    for (j = 0; j < sitelength; j++) {
      p->x2[i][j] = 1.0; 
      p->next->x2[i][j] = 1.0; 
      p->next->next->x2[i][j] = 1.0; 
    }
  }
  hookup(tr->nodep[m - 1], p);
  p->v = initialv;
  p->back->v = initialv;
  branchtrans(m, initialv);
  p->branchnum = m;
  p->next->branchnum = p->index;
  p->next->next->branchnum = p->index;
  p->back->branchnum = m;
}  /* buildnewtip */


void buildsimpletree(tree *tr)
{
  /* set up and adjust branch lengths of a three-species tree */

  hookup(tr->nodep[enterorder[0] - 1], tr->nodep[enterorder[1] - 1]);
  tr->nodep[enterorder[0] - 1]->v = initialv;
  tr->nodep[enterorder[1] - 1]->v = initialv;
  branchtrans(enterorder[1], initialv);
  tr->nodep[enterorder[0] - 1]->branchnum = 2;
  tr->nodep[enterorder[1] - 1]->branchnum = 2;
  buildnewtip(enterorder[2], tr);
  insert_(tr->nodep[enterorder[2] - 1]->back, tr->nodep[enterorder[1] - 1]);
  tr->start = tr->nodep[enterorder[2]-1]->back;
}  /* buildsimpletree */


void addtraverse(node *p, node *q, boolean contin)
{
  /* try adding p at q, proceed recursively through tree */
  long oldnum = 0;
  double like, vsave = 0;
  node *qback =NULL;

  if (!smoothit) {
    oldnum = q->branchnum;
    copymatrix (temp2matrix, curtree.trans[oldnum-1]);
    vsave = q->v;
    qback = q->back;
  }
  insert_(p, q);
  like = evaluate(&curtree, p);
  if (like > bestyet) {
    bestyet = like;
    if (smoothit) {
      restml_copy_(&curtree, &bestree);
      addwhere = q;
    }
    else
      qwhere = q;
    succeeded = true;
  }
  if (smoothit)
    restml_copy_(&priortree, &curtree);
  else {
    hookup (q, qback);
    q->v = vsave;
    q->back->v = vsave;
    q->branchnum = oldnum;
    q->back->branchnum = oldnum;
    copymatrix (curtree.trans[oldnum-1], temp2matrix);
    curtree.likelihood = bestyet;
  }
  if (!q->tip && contin) {
    addtraverse(p, q->next->back, contin);
    addtraverse(p, q->next->next->back, contin);
  }
}  /* addtraverse */

void globrearrange() 
{
  /* does global rearrangements */
  tree globtree;
  tree oldtree;
  int i,j,k,l,num_sibs,num_sibs2;
  node *where,*sib_ptr,*sib_ptr2;
  double oldbestyet = curtree.likelihood;
  int success = false;
  printf("\n   ");
  alloctree(&globtree.nodep,nonodes2,0);
  alloctree(&oldtree.nodep,nonodes2,0);
  alloctrans(&globtree.trans, nonodes2, sitelength);
  alloctrans(&oldtree.trans, nonodes2, sitelength);
  setuptree2(globtree);
  setuptree2(oldtree);
  allocx2(nonodes2, endsite + 1, sitelength,globtree.nodep, 0);
  allocx2(nonodes2, endsite + 1, sitelength,oldtree.nodep, 0);
  restml_copy_(&curtree,&globtree);
  restml_copy_(&curtree,&oldtree);
  bestyet = curtree.likelihood;
  for ( i = spp ; i < nonodes2 ; i++ ) {
    num_sibs = count_sibs(curtree.nodep[i]);
    sib_ptr  = curtree.nodep[i];
    if ( (i - spp) % (( nonodes2 / 72 ) + 1 ) == 0 )
      putchar('.');
    fflush(stdout);
    for ( j = 0 ; j <= num_sibs ; j++ ) {
      restml_re_move(&sib_ptr,&where);
      restml_copy_(&curtree,&priortree);
      qwhere = where;
      
      if (where->tip) {
        restml_copy_(&oldtree,&curtree);
        restml_copy_(&oldtree,&bestree);
        sib_ptr=sib_ptr->next;
        continue;
      }
      else num_sibs2 = count_sibs(where);
      sib_ptr2 = where;
      for ( k = 0 ; k < num_sibs2 ; k++ ) {
        addwhere = NULL;
        addtraverse(sib_ptr,sib_ptr2->back,true);
        if ( !smoothit ) {
          if (succeeded && qwhere != where && qwhere != where->back) {
            insert_(sib_ptr,qwhere);
            smoothit = true;
            for (l = 1; l<=smoothings; l++) {
              smooth (where);
              smooth (where->back);
            }
            smoothit = false;
            success = true;
            restml_copy_(&curtree,&globtree);
          }
          restml_copy_(&priortree,&curtree);
        }
        else if ( addwhere && where != addwhere && where->back != addwhere
              && bestyet > globtree.likelihood) {
            restml_copy_(&bestree,&globtree);
            success = true;
        }
        sib_ptr2 = sib_ptr2->next;
      } 
      restml_copy_(&oldtree,&curtree);
      restml_copy_(&oldtree,&bestree);
      sib_ptr = sib_ptr->next;
    }
  }
  restml_copy_(&globtree,&curtree);
  restml_copy_(&globtree,&bestree);
  if (success && globtree.likelihood > oldbestyet)  {
    succeeded = true;
  }
  else  {
    succeeded = false;
  }
  bestyet = globtree.likelihood;
  freetrans(&globtree.trans, nonodes2, sitelength);
  freetrans(&oldtree.trans, nonodes2, sitelength);
  freetree2(globtree.nodep,nonodes2);
  freetree2(oldtree.nodep,nonodes2);
}

void rearrange(node *p, node *pp)
{
  /* rearranges the tree locally */
  long i, oldnum3=0, oldnum4=0, oldnum5=0;
  double v3=0, v4=0, v5=0;
  node *q, *r;

  if (!p->tip && !p->back->tip) {
    bestyet = curtree.likelihood;
    if (p->back->next != pp)
      r = p->back->next;
    else
      r = p->back->next->next;
    if (!smoothit) {
      oldnum3 = r->branchnum;
      copymatrix (temp3matrix, curtree.trans[oldnum3-1]);
      v3 = r->v;
      oldnum4 = r->next->branchnum;
      copymatrix (temp4matrix, curtree.trans[oldnum4-1]);
      v4 = r->next->v;
      oldnum5 = r->next->next->branchnum;
      copymatrix (temp5matrix, curtree.trans[oldnum5-1]);
      v5 = r->next->next->v;
    }
    else
      restml_copy_(&curtree, &bestree);
    restml_re_move(&r, &q);
    if (smoothit)
      restml_copy_(&curtree, &priortree);
    else
      qwhere = q;
    addtraverse(r, p->next->back, false);
    addtraverse(r, p->next->next->back, false);
    if (smoothit)
      restml_copy_(&bestree, &curtree);
    else {
      insert_(r, qwhere);
      if (qwhere == q) {
        r->v = v3;
        r->back->v = v3;
        r->branchnum = oldnum3;
        r->back->branchnum = oldnum3;
        copymatrix (curtree.trans[oldnum3-1], temp3matrix);
        r->next->v = v4;
        r->next->back->v = v4;
        r->next->branchnum = oldnum4;
        r->next->back->branchnum = oldnum4;
        copymatrix (curtree.trans[oldnum4-1], temp4matrix);
        r->next->next->v = v5;
        r->next->next->back->v = v5;
        r->next->next->branchnum = oldnum5;
        r->next->next->back->branchnum = oldnum5;
        copymatrix (curtree.trans[oldnum5-1], temp5matrix);
        curtree.likelihood = bestyet;
      }
      else {
        smoothit = true;
        for (i = 1; i<=smoothings; i++) {
          smooth (r);
          smooth (r->back);
        }
        smoothit = false;
        restml_copy_(&curtree, &bestree);
      }
    }
  }
  if (!p->tip) {
    rearrange(p->next->back, p);
    rearrange(p->next->next->back, p);
  }
}  /* rearrange */


void restml_coordinates(node *p, double lengthsum, long *tipy,
                double *tipmax, double *x)
{
  /* establishes coordinates of nodes */
  node *q, *first, *last;

  if (p->tip) {
    p->xcoord = (long)(over * lengthsum + 0.5);
    p->ycoord = (*tipy);
    p->ymin = (*tipy);
    p->ymax = (*tipy);
    (*tipy) += down;
    if (lengthsum > (*tipmax))
      (*tipmax) = lengthsum;
    return;
  }
  q = p->next;
  do {
    (*x) = -0.75 * log(1.0 - 1.333333 * q->v);
    restml_coordinates(q->back, lengthsum + (*x),tipy,tipmax,x);
    q = q->next;
  } while ((p == curtree.start || p != q) &&
           (p != curtree.start || p->next != q));
  first = p->next->back;
  q = p;
  while (q->next != p)
    q = q->next;
  last = q->back;
  p->xcoord = (long)(over * lengthsum + 0.5);
  if (p == curtree.start)
    p->ycoord = p->next->next->back->ycoord;
  else
    p->ycoord = (first->ycoord + last->ycoord) / 2;
  p->ymin = first->ymin;
  p->ymax = last->ymax;
}  /* restml_coordinates */


void restml_printree()
{
  /* prints out diagram of the tree */
  long tipy,i;
  double scale, tipmax, x;

  putc('\n', outfile);
  if (!treeprint)
    return;
  putc('\n', outfile);
  tipy = 1;
  tipmax = 0.0;
  restml_coordinates(curtree.start, 0.0, &tipy,&tipmax,&x);
  scale = 1.0 / (tipmax + 1.000);
  for (i = 1; i <= tipy - down; i++)
    drawline2(i, scale, curtree);
  putc('\n', outfile);
}  /* restml_printree */


double sigma(node *q, double *sumlr)
{
  /* get 1.95996 * approximate standard error of branch length */
  double sump, sumr, sums, sumc, p, pover3, pijk, Qjk, liketerm, f;
  double  slopef,curvef;
  long i, j, k, m1, m2;
  double binom1[maxcutter + 1], binom2[maxcutter + 1];
  transmatrix Prob, slopeP, curveP;
  node *r;
  sitelike2 x1, x2;
  double term, TEMP;

  Prob   = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  slopeP = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  curveP = (transmatrix)Malloc((sitelength+1) * sizeof(double *));
  for (i=0; i<=sitelength; ++i) {
    Prob[i]   = (double *)Malloc((sitelength+1) * sizeof(double));
    slopeP[i] = (double *)Malloc((sitelength+1) * sizeof(double));
    curveP[i] = (double *)Malloc((sitelength+1) * sizeof(double));
  }
  p = q->v;
  pover3 = p / 3;
  for (i = 0; i <= sitelength; i++) {
    binom1[0] = exp((sitelength - i) * log(1 - p));
    for (k = 1; k <= (sitelength - i); k++)
      binom1[k] = binom1[k - 1] * (p / (1 - p)) * (sitelength - i - k + 1) / k;
    binom2[0] = exp(i * log(1 - pover3));
    for (k = 1; k <= i; k++)
      binom2[k] = binom2[k - 1] * (pover3 / (1 - pover3)) * (i - k + 1) / k;
    for (j = 0; j <= sitelength; j++) {
      sump = 0.0;
      sums = 0.0;
      sumc = 0.0;
      if (i - j > 0)
        m1 = i - j;
      else
        m1 = 0;
      if (sitelength - j < i)
        m2 = sitelength - j;
      else
        m2 = i;
      for (k = m1; k <= m2; k++) {
        pijk = binom1[j - i + k] * binom2[k];
        sump += pijk;
        term = (j-i+2*k)/p - (sitelength-j-k)/(1.0-p) - (i-k)/(3.0-p);
        sums += pijk * term;
        sumc += pijk * (term * term
                          - (j-i+2*k)/(p*p)
                          - (sitelength-j-k)/((1.0-p)*(1.0-p))
                          - (i-k)/((3.0-p)*(3.0-p)) );
      }
      Prob[i][j] = sump;
      slopeP[i][j] = sums;
      curveP[i][j] = sumc;
    }
  }
  (*sumlr) = 0.0;
  sumc = 0.0;
  sums = 0.0;
  r = q->back;
  for (i = 1; i <= endsite; i++) {
    f = 0.0;
    slopef = 0.0;
    curvef = 0.0;
    sumr = 0.0;
    memcpy(x1, q->x2[i], sizeof(sitelike2));
    memcpy(x2, r->x2[i], sizeof(sitelike2));
    for (j = 0; j <= sitelength; j++) {
      liketerm = pie[j] * x1[j];
      sumr += liketerm * x2[j];
      for (k = 0; k <= sitelength; k++) {
        Qjk = liketerm * x2[k];
        f += Qjk * Prob[j][k];
        slopef += Qjk * slopeP[j][k];
        curvef += Qjk * curveP[j][k];
      }
    }
    (*sumlr) += weight[i] * log(f / sumr);
    sums += weight[i] * slopef / f;
    TEMP = slopef / f;
    sumc += weight[i] * (curvef / f - TEMP * TEMP);
  }
  if (trunc8) {
    f = 0.0;
    slopef = 0.0;
    curvef = 0.0;
    sumr = 0.0;
    memcpy(x1, q->x2[0], sizeof(sitelike2));
    memcpy(x2, r->x2[0], sizeof(sitelike2));
    for (j = 0; j <= sitelength; j++) {
      liketerm = pie[j] * x1[j];
      sumr += liketerm * x2[j];
      for (k = 0; k <= sitelength; k++) {
        Qjk = liketerm * x2[k];
        f += Qjk * Prob[j][k];
        slopef += Qjk * slopeP[j][k];
        curvef += Qjk * curveP[j][k];
      }
    }
    (*sumlr) += weightsum * log((1.0 - sumr) / (1.0 - f));
    sums += weightsum * slopef / (1.0 - f);
    TEMP = slopef / (1.0 - f);
    sumc += weightsum * (curvef / (1.0 - f) + TEMP * TEMP);
  }
  for (i=0;i<=sitelength;++i){
    free(Prob[i]);
    free(slopeP[i]);
    free(curveP[i]);
  }
  free(Prob);
  free(slopeP);
  free(curveP);
  if (sumc < -1.0e-6)
    return ((-sums - sqrt(sums * sums - 3.841 * sumc)) / sumc);
  else
    return -1.0;
}  /* sigma */


void describe(node *p)
{
  /* print out information on one branch */
  double sumlr;
  long i;
  node *q;
  double s;

  q = p->back;
  fprintf(outfile, "%4ld      ", q->index - spp);
  fprintf(outfile, "    ");
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index - 1][i], outfile);
  } else
    fprintf(outfile, "%4ld      ", p->index - spp);
  if (q->v >= 0.75)
    fprintf(outfile, "     infinity");
  else
    fprintf(outfile, "%13.5f", -0.75 * log(1 - 1.333333 * q->v));
  if (p->iter) {
    s = sigma(q, &sumlr);
    if (s < 0.0)
      fprintf(outfile, "     (     zero,    infinity)");
    else {
      fprintf(outfile, "     (");
      if (q->v - s <= 0.0)
        fprintf(outfile, "     zero");
      else
        fprintf(outfile, "%9.5f", -0.75 * log(1 - 1.333333 * (q->v - s)));
      putc(',', outfile);
      if (q->v + s >= 0.75)
        fprintf(outfile, "    infinity");
      else
        fprintf(outfile, "%12.5f", -0.75 * log(1 - 1.333333 * (q->v + s)));
      putc(')', outfile);
      }
    if (sumlr > 1.9205)
      fprintf(outfile, " *");
    if (sumlr > 2.995)
      putc('*', outfile);
    }
  else
    fprintf(outfile, "            (not varied)");
  putc('\n', outfile);
  if (!p->tip) {
    describe(p->next->back);
    describe(p->next->next->back);
  }
}  /* describe */


void summarize()
{
  /* print out information on branches of tree */

  fprintf(outfile, "\nremember: ");
  if (outgropt)
    fprintf(outfile, "(although rooted by outgroup) ");
  fprintf(outfile, "this is an unrooted tree!\n\n");
  fprintf(outfile, "Ln Likelihood = %11.5f\n\n", curtree.likelihood);
  fprintf(outfile, " \n");
  fprintf(outfile, " Between        And            Length");
  fprintf(outfile, "      Approx. Confidence Limits\n");
  fprintf(outfile, " -------        ---            ------");
  fprintf(outfile, "      ------- ---------- ------\n");
  describe(curtree.start->next->back);
  describe(curtree.start->next->next->back);
  describe(curtree.start->back);
  fprintf(outfile, "\n     *  = significantly positive, P < 0.05\n");
  fprintf(outfile, "     ** = significantly positive, P < 0.01\n\n\n");
}  /* summarize */


void restml_treeout(node *p)
{
  /* write out file with representation of final tree */
  long i, n, w;
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
    putc('(', outtree);
    col++;
    restml_treeout(p->next->back);
    putc(',', outtree);
    col++;
    if (col > 45) {
      putc('\n', outtree);
      col = 0;
    }
    restml_treeout(p->next->next->back);
    if (p == curtree.start) {
      putc(',', outtree);
      col++;
      if (col > 45) {
        putc('\n', outtree);
        col = 0;
      }
      restml_treeout(p->back);
    }
    putc(')', outtree);
    col++;
  }
  if (p->v >= 0.75)
    x = -1.0;
  else
    x = -0.75 * log(1 - 1.333333 * p->v);
  if (x > 0.0)
    w = (long)(0.43429448222 * log(x));
  else if (x == 0.0)
    w = 0;
  else
    w = (long)(0.43429448222 * log(-x)) + 1;
  if (w < 0)
    w = 0;
  if (p == curtree.start)
    fprintf(outtree, ";\n");
  else {
    fprintf(outtree, ":%*.5f", (int)(w + 7), x);
    col += w + 8;
  }
}  /* restml_treeout */


void inittravtree(node *p)
{
  /* traverse tree to set initialized and v to initial values */

  if (p->index < p->back->index)
    p->branchnum = p->index;
  else
    p->branchnum = p->back->index;
  p->initialized = false;
  p->back->initialized = false;
  if ((!lengths) || p->iter) {
    branchtrans(p->branchnum, initialv);
    p->v = initialv;
    p->back->v = initialv;
  }
  if (!p->tip) {
    inittravtree(p->next->back);
    inittravtree(p->next->next->back);
  }
} /* inittravtree */


void treevaluate()
{
  /* find maximum likelihood branch lengths of user tree */
  long i;

  inittravtree(curtree.start);
  smoothit = true;
  for (i = 1; i <= smoothings * 4; i++)
    smooth (curtree.start);
  evaluate(&curtree, curtree.start);
}  /* treevaluate */


void maketree()
{
  /* construct and rearrange tree */
  long i,j;
  char* treestr;

  if (usertree) {
      if (numtrees > 2)
      emboss_initseed(inseed, &inseed0, seed);
    l0gl = (double *) Malloc(numtrees * sizeof(double));
    l0gf = (double **) Malloc(numtrees * sizeof(double *));
    for (i=0; i < numtrees; ++i)
      l0gf[i] = (double *)Malloc(endsite * sizeof(double));
    if (treeprint) {
      fprintf(outfile, "User-defined tree");
      if (numtrees > 1)
        putc('s', outfile);
      fprintf(outfile, ":\n\n");
    }
    which = 1;
    while (which <= numtrees) {
      treestr = ajStrStrMod(&phylotrees[which-1]->Tree);
      treeread2 (&treestr, &curtree.start, curtree.nodep,
        lengths, &trweight, &goteof, &haslengths, &spp);
      treevaluate();
      restml_printree();
      summarize();
      if (trout) {
        col = 0;
        restml_treeout(curtree.start);
      }
      which++;
    }
    FClose(intree);
    if (numtrees > 1 && weightsum > 1 )
    standev2(numtrees, maxwhich, 1, endsite, maxlogl, l0gl, l0gf,
              aliasweight, seed);
  } else {
    for (i = 1; i <= spp; i++)
      enterorder[i - 1] = i;
    if (jumble)
      randumize(seed, enterorder);
    if (progress) {
      printf("\nAdding species:\n");
      writename(0, 3, enterorder);
    }
    nextsp = 3;
    buildsimpletree(&curtree);
    curtree.start = curtree.nodep[enterorder[0] - 1]->back;
    smoothit = improve;
    nextsp = 4;
    while (nextsp <= spp) {
      buildnewtip(enterorder[nextsp - 1], &curtree);
      bestyet = - nextsp*sites*sitelength*log(4.0);
      if (smoothit)
        restml_copy_(&curtree, &priortree);
      addtraverse(curtree.nodep[enterorder[nextsp - 1] - 1]->back,
                  curtree.start, true);
      if (smoothit)
        restml_copy_(&bestree, &curtree);
      else {
        insert_(curtree.nodep[enterorder[nextsp - 1] - 1]->back, qwhere);
        smoothit = true;
        for (i = 1; i<=smoothings; i++) {
          smooth (curtree.start);
          smooth (curtree.start->back);
        }
        smoothit = false;
        restml_copy_(&curtree, &bestree);
        bestyet = curtree.likelihood;
      }
      if (progress)
        writename(nextsp - 1, 1, enterorder);
      if (global && nextsp == spp) {
        if (progress) {
          printf("Doing global rearrangements\n");
          printf("  !");
          for (j = spp ; j < nonodes2 ; j++)
            if ( (j - spp) % (( nonodes2 / 72 ) + 1 ) == 0 )
              putchar('-');
          putchar('!');
        }
      }
      succeeded = true;
      while (succeeded) {
        succeeded = false;
        if (global && nextsp == spp)
          globrearrange();
        else 
          rearrange(curtree.start, curtree.start->back);
      }
      for (i = spp; i < nonodes2; i++) {
        curtree.nodep[i]->initialized = false;
        curtree.nodep[i]->next->initialized = false;
        curtree.nodep[i]->next->next->initialized = false;
      }
      if (!smoothit) {
        smoothit = true;
        for (i = 1; i<=smoothings; i++) {
          smooth (curtree.start);
          smooth (curtree.start->back);
        }
        smoothit = false;
        restml_copy_(&curtree, &bestree);
      }
      nextsp++;
    }
    if (global && progress) {
      putchar('\n');
      fflush(stdout);
    }
    if (njumble > 1) {
      if (jumb == 1)
        restml_copy_(&bestree, &bestree2);
      else
        if (bestree2.likelihood < bestree.likelihood)
          restml_copy_(&bestree, &bestree2);
    }
    if (jumb == njumble) {
      if (njumble > 1)
        restml_copy_(&bestree2, &curtree);
      curtree.start = curtree.nodep[outgrno - 1]->back;
      restml_printree();
      summarize();
      if (trout) {
        col = 0;
        restml_treeout(curtree.start);
      }
    }
  }
  freex2(nonodes2, curtree.nodep);
   if (!usertree) {
     freex2(nonodes2, priortree.nodep);
     freex2(nonodes2, bestree.nodep);
     if (njumble > 1)
       freex2(nonodes2, bestree2.nodep);
   } else {
     free(l0gl);
     for (i=0;i<numtrees;++i)
       free(l0gf[i]);
     free(l0gf);
   }
  if (jumb == njumble) {
    if (progress) {
      printf("\nOutput written to file \"%s\"\n\n", outfilename);
      if (trout)
        printf("Tree also written onto file \"%s\"\n", outtreename);
      putchar('\n');
    }
  }
}  /* maketree */


int main(int argc, Char *argv[])
{  /* maximum likelihood phylogenies from restriction sites */
#ifdef MAC
  argc = 1;                /* macsetup("Restml","");        */
  argv[0] = "RestML";
#endif
  init(argc, argv);
  emboss_getoptions("frestml",argc,argv);
  progname = argv[0];

  ibmpc = IBMCRT;
  ansi = ANSICRT;

  firstset = true;
  doinit();
  for (ith = 1; ith <= datasets; ith++) {
    if (datasets > 1) {
      fprintf(outfile, "Data set # %ld:\n",ith);
      if (progress)
        printf("\nData set # %ld:\n",ith);
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
  return 0;
}  /* maximum likelihood phylogenies from restriction sites */
