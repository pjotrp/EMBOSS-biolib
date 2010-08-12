#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1993-2004 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define iterationsd     100   /* number of iterates of EM for each distance */

typedef struct valrec {
  double rat, ratxv, z1, y1, z1zz, z1yy, z1xv;
} valrec;

AjPSeqset* seqsets = NULL;
AjPPhyloProp phyloratecat = NULL;
AjPPhyloProp phyloweights = NULL;


ajint numseqs;
ajint numwts;




extern sequence y;

Char infilename[FNMLNGTH], catfilename[FNMLNGTH], weightfilename[FNMLNGTH];
const char* outfilename;
AjPFile embossoutfile;

long sites, categs, weightsum, datasets, ith, rcategs;
boolean freqsfrom, jukes, kimura, logdet, gama, invar, similarity, lower, f84,
    weights, progress, ctgry, mulsets, justwts, firstset, baddists, human;
boolean matrix_flags;       /* Matrix output format */
node **nodep;
double xi, xv, ttratio, ttratio0, freqa, freqc, freqg, freqt, freqr, freqy,
        freqar, freqcy, freqgr, freqty, cvi, invarfrac, sumrates, fracchange;
steptr oldweight;
double rate[maxcategs];
double **d;
double sumweightrat;                  /* these values were propagated  */
double *weightrat;                    /* to global values from         */
valrec tbl[maxcategs];                /* function makedists.           */


#ifndef OLDC
/* function  prototypes */
//void   getoptions(void);
void   emboss_getoptions(char *pgm, int argc, char *argv[]);
void   allocrest(void);
void   reallocsites(void);
void   doinit(void);
void   printcategories(void);
void   inputoptions(void);
void   dnadist_sitesort(void);
void   dnadist_sitecombine(void);
void   dnadist_sitescrunch(void);
void   makeweights(void);
void   dnadist_makevalues(void);
void   dnadist_empiricalfreqs(void);
void   getinput(void);
void   inittable(void);
double lndet(double (*a)[4]);
void   makev(long, long, double *);
void   makedists(void);
void   writedists(void);
/* function  prototypes */
#endif


void   emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjPStr method = NULL;
    AjPStr gammamethod = NULL;
    AjPFloat basefreq;
    AjPFloat arrayval; 
  boolean ttr;

  ctgry = false;
  categs = 1;
  cvi = 1.0;
  rcategs = 1;
  rate[0] = 1.0;
  freqsfrom = true;
  gama = false;
  invar = false;
  invarfrac = 0.0;
  jukes = false;
  justwts = false;
  kimura = false;
  logdet = false;
  f84 = false;
  lower = false;
  human = false;
  similarity = false;
  ttratio = 2.0;
  ttr = false;
  weights = false;
  printdata = false;
  progress = true;
  mulsets = false;
  datasets = 1;
  matrix_flags = MAT_MACHINE;

  embInitPV(pgm, argc, argv, "PHYLIPNEW",VERSION);

    seqsets = ajAcdGetSeqsetall("sequence");

    numseqs = 0;
    while (seqsets[numseqs])
	numseqs++;

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

    method = ajAcdGetListSingle("method");

    if(ajStrMatchC(method, "f")) {
      f84 = true;
      ttratio = ajAcdGetFloat("ttratio");
      freqsfrom = ajAcdGetToggle("freqsfrom");
    }
    else if(ajStrMatchC(method, "k")) {
      kimura = true;
      ttratio = ajAcdGetFloat("ttratio");
    }
    else if(ajStrMatchC(method, "j")) jukes = true;
    else if(ajStrMatchC(method, "l")) logdet = true;
    else if(ajStrMatchC(method, "s")) similarity = true;



    if( (f84) || (kimura) || (jukes) ) {
      gammamethod = ajAcdGetListSingle("gamma");
      if(ajStrMatchC(gammamethod, "g")) { 
        gama = true;
        cvi = ajAcdGetFloat("gammacoefficient");
        cvi = 1.0 / (cvi * cvi);
      }
      else if(ajStrMatchC(gammamethod, "i")) { 
        invar = true;
        cvi = ajAcdGetFloat("gammacoefficient");
        cvi = 1.0 / (cvi * cvi);
        invarfrac = ajAcdGetFloat("invarfrac");
      }
      else if(ajStrMatchC(gammamethod, "n")) {
        categs = ajAcdGetInt("ncategories");        
      }
    }

    if (categs > 1) {
      ctgry = true;
      arrayval = ajAcdGetArray("rate");
      emboss_initcategs(arrayval, categs, rate);
    }


    printdata = ajAcdGetBoolean("printdata");
    progress = ajAcdGetBoolean("progress");
    lower = ajAcdGetBoolean("lower");
    if(lower)
        matrix_flags = MAT_LOWER;
    human = ajAcdGetBoolean("humanreadable");
    if(human)
        matrix_flags |= MAT_HUMAN;

    if(!freqsfrom) {
      basefreq = ajAcdGetArray("basefreq"); 
      freqa = ajFloatGet(basefreq, 0);
      freqc = ajFloatGet(basefreq, 1);
      freqg = ajFloatGet(basefreq, 2);
      freqt = ajFloatGet(basefreq, 3);
    }


     embossoutfile = ajAcdGetOutfile("outfile");   

     emboss_openfile(embossoutfile, &outfile, &outfilename); 


/*    fprintf(outfile, "\nNucleic acid sequence Distance Matrix program,");*/
/*    fprintf(outfile, " version %s\n\n",VERSION);*/
}  /* emboss_getoptions */


void allocrest(void)
{
  long i;

  y = (Char **)Malloc(spp*sizeof(Char *));
  nodep = (node **)Malloc(spp*sizeof(node *));
  for (i = 0; i < spp; i++) {
    y[i] = (Char *)Malloc(sites*sizeof(Char));
    nodep[i] = (node *)Malloc(sizeof(node));
  }
  d = (double **)Malloc(spp*sizeof(double *));
  for (i = 0; i < spp; i++)
    d[i] = (double*)Malloc(spp*sizeof(double));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  category = (steptr)Malloc(sites*sizeof(long));
  oldweight = (steptr)Malloc(sites*sizeof(long));
  weight = (steptr)Malloc(sites*sizeof(long));
  alias = (steptr)Malloc(sites*sizeof(long));
  ally = (steptr)Malloc(sites*sizeof(long));
  location = (steptr)Malloc(sites*sizeof(long));
  weightrat = (double *)Malloc(sites*sizeof(double));
} /* allocrest */


void reallocsites(void) 
{/* The amount of sites can change between runs 
     this function reallocates all the variables 
     whose size depends on the amount of sites */
  long i;

  for (i = 0; i < spp; i++) {
    free(y[i]);  
    y[i] = (Char *)Malloc(sites*sizeof(Char));
  }
  free(category);
  free(oldweight);
  free(weight);
  free(alias);
  free(ally);
  free(location);
  free(weightrat);
  
  category = (steptr)Malloc(sites*sizeof(long));
  oldweight = (steptr)Malloc(sites*sizeof(long));
  weight = (steptr)Malloc(sites*sizeof(long));
  alias = (steptr)Malloc(sites*sizeof(long));
  ally = (steptr)Malloc(sites*sizeof(long));
  location = (steptr)Malloc(sites*sizeof(long));
  weightrat = (double *)Malloc(sites*sizeof(double));  
} /* reallocsites */


void doinit(void)
{
  /* initializes variables */

  inputnumbersseq(seqsets[0], &spp, &sites, &nonodes, 1);
  if (printdata)
    fprintf(outfile, "%2ld species, %3ld  sites\n", spp, sites);
  allocrest();
}  /* doinit */




void printcategories(void)
{ /* print out list of categories of sites */
  long i, j;

  fprintf(outfile, "Rate categories\n\n");
  for (i = 1; i <= nmlngth + 3; i++)
    putc(' ', outfile);
  for (i = 1; i <= sites; i++) {
    fprintf(outfile, "%ld", category[i - 1]);
    if (i % 60 == 0) {
      putc('\n', outfile);
      for (j = 1; j <= nmlngth + 3; j++)
        putc(' ', outfile);
    } else if (i % 10 == 0)
      putc(' ', outfile);
  }
  fprintf(outfile, "\n\n");
}  /* printcategories */


void inputoptions(void)
{
  /* read options information */
  long i;

  if (!firstset && !justwts) {
    samenumspseq(seqsets[ith-1], &sites, ith);
    reallocsites();
  }
  for (i = 0; i < sites; i++) {
    category[i] = 1;
    oldweight[i] = 1;
  }
  if (justwts || weights)
    inputweightsstr(phyloweights->Str[ith-1], sites, oldweight, &weights);
  if (printdata)
    putc('\n', outfile);
  if (jukes && printdata)
    fprintf(outfile, "  Jukes-Cantor Distance\n");
  if (kimura && printdata)
    fprintf(outfile, "  Kimura 2-parameter Distance\n");
  if (f84 && printdata)
    fprintf(outfile, "  F84 Distance\n");
  if (similarity)
    fprintf(outfile, "  \n  Table of similarity between sequences\n");
  if (firstset && printdata && (kimura || f84))
    fprintf(outfile, "\nTransition/transversion ratio = %10.6f\n", ttratio);
  if (ctgry && categs > 1) {
    inputcategsstr(phyloratecat->Str[0], 0, sites, category, categs, "DnaDist");
    if (printdata)
      printcategs(outfile, sites, category, "Site categories");
  } else if (printdata && (categs > 1)) {
    fprintf(outfile, "\nSite category   Rate of change\n\n");
    for (i = 1; i <= categs; i++)
      fprintf(outfile, "%12ld%13.3f\n", i, rate[i - 1]);
    putc('\n', outfile);
    printcategories();
  }

  if (jukes)
    ttratio = 0.5000001;
  if (weights && printdata)
    printweights(outfile, 0, sites, oldweight, "Sites");
}  /* inputoptions */


void dnadist_sitesort(void)
{
  /* Shell sort of sites lexicographically */
  long gap, i, j, jj, jg, k, itemp;
  boolean flip, tied;

  gap = sites / 2;
  while (gap > 0) {
    for (i = gap + 1; i <= sites; i++) {
      j = i - gap;
      flip = true;
      while (j > 0 && flip) {
        jj = alias[j - 1];
        jg = alias[j + gap - 1];
        tied = (oldweight[jj - 1] == oldweight[jg - 1]);
        flip = (oldweight[jj - 1] < oldweight[jg - 1] ||
                (tied && category[jj - 1] > category[jg - 1]));
        tied = (tied && category[jj - 1] == category[jg - 1]);
        k = 1;
        while (k <= spp && tied) {
          flip = (y[k - 1][jj - 1] > y[k - 1][jg - 1]);
          tied = (tied && y[k - 1][jj - 1] == y[k - 1][jg - 1]);
          k++;
        }
        if (!flip)
          break;
        itemp = alias[j - 1];
        alias[j - 1] = alias[j + gap - 1];
        alias[j + gap - 1] = itemp;
        j -= gap;
      }
    }
    gap /= 2;
  }
}  /* dnadist_sitesort */


void dnadist_sitecombine(void)
{
  /* combine sites that have identical patterns */
  long i, j, k;
  boolean tied;

  i = 1;
  while (i < sites) {
    j = i + 1;
    tied = true;
    while (j <= sites && tied) {
      tied = (oldweight[alias[i - 1] - 1] == oldweight[alias[j - 1] - 1] &&
              category[alias[i - 1] - 1] == category[alias[j - 1] - 1]);
      k = 1;
      while (k <= spp && tied) {
        tied = (tied &&
            y[k - 1][alias[i - 1] - 1] == y[k - 1][alias[j - 1] - 1]);
        k++;
      }
      if (!tied)
        break;
      ally[alias[j - 1] - 1] = alias[i - 1];
      j++;
    }
    i = j;
  }
}  /* dnadist_sitecombine */


void dnadist_sitescrunch(void)
{
  /* move so one representative of each pattern of
     sites comes first */
  long i, j, itemp;
  boolean done, found, completed;

  done = false;
  i = 1;
  j = 2;
  while (!done) {
    if (ally[alias[i - 1] - 1] != alias[i - 1]) {
      if (j <= i)
        j = i + 1;
      if (j <= sites) {
        do {
          found = (ally[alias[j - 1] - 1] == alias[j - 1]);
          j++;
          completed = (j > sites);
          if (j <= sites)
            completed = (oldweight[alias[j - 1] - 1] == 0);
        } while (!(found || completed));
        if (found) {
          j--;
          itemp = alias[i - 1];
          alias[i - 1] = alias[j - 1];
          alias[j - 1] = itemp;
        } else
          done = true;
      } else
        done = true;
    }
    i++;
    done = (done || i >= sites);
  }
}  /* dnadist_sitescrunch */


void makeweights(void)
{
  /* make up weights vector to avoid duplicate computations */
  long i;

  for (i = 1; i <= sites; i++) {
    alias[i - 1] = i;
    ally[i - 1] = i;
    weight[i - 1] = 0;
  }
  dnadist_sitesort();
  dnadist_sitecombine();
  dnadist_sitescrunch();
  endsite = 0;
  for (i = 1; i <= sites; i++) {
    if (ally[i - 1] == i && oldweight[i - 1] > 0)
      endsite++;
  }
  for (i = 1; i <= endsite; i++)
    location[alias[i - 1] - 1] = i;
  weightsum = 0;
  for (i = 0; i < sites; i++)
    weightsum += oldweight[i];
  sumrates = 0.0;
  for (i = 0; i < sites; i++)
    sumrates += oldweight[i] * rate[category[i] - 1];
  for (i = 0; i < categs; i++)
    rate[i] *= weightsum / sumrates;
  for (i = 0; i < sites; i++)
    weight[location[ally[i] - 1] - 1] += oldweight[i];
}  /* makeweights */


void dnadist_makevalues(void)
{
  /* set up fractional likelihoods at tips */
  long i, j, k;
  bases b;

  for (i = 0; i < spp; i++) {
    nodep[i]->x = (phenotype)Malloc(endsite*sizeof(ratelike));
    for (j = 0; j < endsite; j++)
      nodep[i]->x[j]  = (ratelike)Malloc(rcategs*sizeof(sitelike));
  }
  for (k = 0; k < endsite; k++) {
    j = alias[k];
    for (i = 0; i < spp; i++) {
      for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
        nodep[i]->x[k][0][(long)b - (long)A] = 0.0;
      switch (y[i][j - 1]) {

      case 'A':
        nodep[i]->x[k][0][0] = 1.0;
        break;

      case 'C':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        break;

      case 'G':
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'T':
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'U':
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'M':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        break;

      case 'R':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'W':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'S':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'Y':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'K':
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'B':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'D':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'H':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'V':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'N':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case 'X':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case '?':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case 'O':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case '-':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;
      }
    }
  }
}  /* dnadist_makevalues */


void dnadist_empiricalfreqs(void)
{
  /* Get empirical base frequencies from the data */
  long i, j, k;
  double sum, suma, sumc, sumg, sumt, w;

  freqa = 0.25;
  freqc = 0.25;
  freqg = 0.25;
  freqt = 0.25;
  for (k = 1; k <= 8; k++) {
    suma = 0.0;
    sumc = 0.0;
    sumg = 0.0;
    sumt = 0.0;
    for (i = 0; i < spp; i++) {
      for (j = 0; j < endsite; j++) {
        w = weight[j];
        sum = freqa * nodep[i]->x[j][0][0];
        sum += freqc * nodep[i]->x[j][0][(long)C - (long)A];
        sum += freqg * nodep[i]->x[j][0][(long)G - (long)A];
        sum += freqt * nodep[i]->x[j][0][(long)T - (long)A];
        suma += w * freqa * nodep[i]->x[j][0][0] / sum;
        sumc += w * freqc * nodep[i]->x[j][0][(long)C - (long)A] / sum;
        sumg += w * freqg * nodep[i]->x[j][0][(long)G - (long)A] / sum;
        sumt += w * freqt * nodep[i]->x[j][0][(long)T - (long)A] / sum;
      }
    }
    sum = suma + sumc + sumg + sumt;
    freqa = suma / sum;
    freqc = sumc / sum;
    freqg = sumg / sum;
    freqt = sumt / sum;
  }
}  /* dnadist_empiricalfreqs */


void getinput(void)
{
  /* reads the input data */
  inputoptions();
  if ((!freqsfrom) && !logdet && !similarity) {
    if (kimura || jukes) {
      freqa = 0.25;
      freqc = 0.25;
      freqg = 0.25;
      freqt = 0.25;
    }
    getbasefreqs(freqa, freqc, freqg, freqt, &freqr, &freqy, &freqar, &freqcy,
                   &freqgr, &freqty, &ttratio, &xi, &xv, &fracchange,
                   freqsfrom, printdata);
    if (freqa < 0.00000001) {
      freqa = 0.000001;
      freqc = 0.999999*freqc;
      freqg = 0.999999*freqg;
      freqt = 0.999999*freqt;
    }
    if (freqc < 0.00000001) {
      freqa = 0.999999*freqa;
      freqc = 0.000001;
      freqg = 0.999999*freqg;
      freqt = 0.999999*freqt;
    }
    if (freqg < 0.00000001) {
      freqa = 0.999999*freqa;
      freqc = 0.999999*freqc;
      freqg = 0.000001;
      freqt = 0.999999*freqt;
    }
    if (freqt < 0.00000001) {
      freqa = 0.999999*freqa;
      freqc = 0.999999*freqc;
      freqg = 0.999999*freqg;
      freqt = 0.000001;
    }
  }
  if (!justwts || firstset)
    seq_inputdata(seqsets[ith-1],sites);
  makeweights();
  dnadist_makevalues();
  if (freqsfrom) {
    dnadist_empiricalfreqs();
    getbasefreqs(freqa, freqc, freqg, freqt, &freqr, &freqy, &freqar, &freqcy,
                   &freqgr, &freqty, &ttratio, &xi, &xv, &fracchange,
                   freqsfrom, printdata);
  }
}  /* getinput */


void inittable(void)
{
  /* Define a lookup table. Precompute values and store in a table */
  long i;

  for (i = 0; i < categs; i++) {
    tbl[i].rat = rate[i];
    tbl[i].ratxv = rate[i] * xv;
  }
}  /* inittable */


double lndet(double (*a)[4])
{
  long i, j, k;
  double temp, ld;

  /*Gauss-Jordan reduction -- invert matrix a in place,
         overwriting previous contents of a.  On exit, matrix a
         contains the inverse, lndet contains the log of the determinant */
  ld = 1.0;
  for (i = 0; i < 4; i++) {
    ld *= a[i][i];
    temp = 1.0 / a[i][i];
    a[i][i] = 1.0;
    for (j = 0; j < 4; j++)
      a[i][j] *= temp;
    for (j = 0; j < 4; j++) {
      if (j != i) {
        temp = a[j][i];
        a[j][i] = 0.0;
        for (k = 0; k < 4; k++)
          a[j][k] -= temp * a[i][k];
      }
    }
  }
  if (ld <= 0.0)
    return(99.0);
  else
    return(log(ld));
}  /* lndet */


void makev(long m, long n, double *v)
{
  /* compute one distance */
  long i, j, k, l, it, num1, num2, idx;
  long numerator = 0, denominator = 0;
  double sum, sum1, sum2, sumyr, lz, aa, bb, cc, vv=0,
         p1, p2, p3, q1, q2, q3, tt, delta=0.0, slope,
         xx1freqa, xx1freqc, xx1freqg, xx1freqt;
  double *prod, *prod2, *prod3;
  boolean quick, jukesquick, kimquick, logdetquick, overlap;
  bases b;
  node *p, *q;
  sitelike xx1, xx2;
  double basetable[4][4];  /* for quick logdet */
  double basefreq1[4], basefreq2[4];

  p = nodep[m - 1];
  q = nodep[n - 1];

  /* check for overlap between sequences */
  overlap = false;
  for(i=0 ; i < sites ; i++){
    if((strchr("NX?O-",y[m-1][i])==NULL) && 
       (strchr("NX?O-",y[n-1][i])==NULL)){
      overlap = true;
      break;
    }
  }
  if(!overlap){
    printf("\nWARNING: NO OVERLAP BETWEEN SEQUENCES %ld AND %ld; -1.0 WAS WRITTEN\n", m, n);
    baddists = true;
    return;
  }

  quick = (!ctgry || categs == 1);
  if (jukes || kimura || logdet || similarity) {
    numerator = 0;
    denominator = 0;
    for (i = 0; i < endsite; i++) {
      memcpy(xx1, p->x[i][0], sizeof(sitelike));
      memcpy(xx2, q->x[i][0], sizeof(sitelike));
      sum = 0.0;
      sum1 = 0.0;
      sum2 = 0.0;
      for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1)) {
        sum1 += xx1[(long)b - (long)A];
        sum2 += xx2[(long)b - (long)A];
        sum += xx1[(long)b - (long)A] * xx2[(long)b - (long)A];
      }
      quick = (quick && (sum1 == 1.0 || sum1 == 4.0) &&
               (sum2 == 1.0 || sum2 == 4.0));
      if (sum1 == 1.0 && sum2 == 1.0) {
        numerator += (long)(weight[i] * sum);
        denominator += weight[i];
      }
    }
  }
  jukesquick = ((jukes || similarity) && quick);
  kimquick = (kimura && quick);
  logdetquick = (logdet && quick);
  if (logdet && !quick) {
    printf(" WARNING: CANNOT CALCULATE LOGDET DISTANCE\n");
    printf("  WITH PRESENT PROGRAM IF PARTIALLY AMBIGUOUS NUCLEOTIDES\n");
    printf("  -1.0 WAS WRITTEN\n");
    baddists = true;
  }
  if (jukesquick && jukes && (numerator * 4 <= denominator)) {
    printf("\nWARNING: INFINITE DISTANCE BETWEEN ");
    printf(" SPECIES %3ld AND %3ld\n", m, n);
    printf("  -1.0 WAS WRITTEN\n");
    baddists = true;
  }
  if (jukesquick && invar
      && (4 * (((double)numerator / denominator) - invarfrac)
          <= (1.0 - invarfrac))) {
    printf("\nWARNING: DIFFERENCE BETWEEN SPECIES %3ld AND %3ld", m, n);
    printf(" TOO LARGE FOR INVARIABLE SITES\n");
    printf("  -1.0 WAS WRITTEN\n");
    baddists = true;
  }
  if (jukesquick) {
    if (!gama && !invar)
      vv = -0.75 * log((4.0*((double)numerator / denominator) - 1.0) / 3.0);
    else if (!invar)
        vv = 0.75 * cvi * (exp(-(1/cvi)*
           log((4.0 * ((double)numerator / denominator) - 1.0) / 3.0)) - 1.0);
      else
        vv = 0.75 * cvi * (exp(-(1/cvi)*
           log((4.0 * ((double)numerator / denominator - invarfrac)/
                        (1.0-invarfrac) - 1.0) / 3.0)) - 1.0);
  }
  if (kimquick) {
    num1 = 0;
    num2 = 0;
    denominator = 0;
    for (i = 0; i < endsite; i++) {
      memcpy(xx1, p->x[i][0], sizeof(sitelike));
      memcpy(xx2, q->x[i][0], sizeof(sitelike));
      sum = 0.0;
      sum1 = 0.0;
      sum2 = 0.0;
      for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1)) {
        sum1 += xx1[(long)b - (long)A];
        sum2 += xx2[(long)b - (long)A];
        sum += xx1[(long)b - (long)A] * xx2[(long)b - (long)A];
      }
      sumyr = (xx1[0] + xx1[(long)G - (long)A])
            * (xx2[0] + xx2[(long)G - (long)A]) +
              (xx1[(long)C - (long)A] + xx1[(long)T - (long)A]) *
              (xx2[(long)C - (long)A] + xx2[(long)T - (long)A]);
      if (sum1 == 1.0 && sum2 == 1.0) {
        num1 += (long)(weight[i] * sum);
        num2 += (long)(weight[i] * (sumyr - sum));
        denominator += weight[i];
      }
    }
    tt = ((1.0 - (double)num1 / denominator)-invarfrac)/(1.0-invarfrac);
    if (tt > 0.0) {
      delta = 0.1;
      tt = delta;
      it = 0;
      while (fabs(delta) > 0.0000002 && it < iterationsd) {
        it++;
        if (!gama) {
          p1 = exp(-tt);
          p2 = exp(-xv * tt) - exp(-tt);
          p3 = 1.0 - exp(-xv * tt);
        } else {
          p1 = exp(-cvi * log(1 + tt / cvi));
          p2 = exp(-cvi * log(1 + xv * tt / cvi))
              - exp(-cvi * log(1 + tt / cvi));
          p3 = 1.0 - exp(-cvi * log(1 + xv * tt / cvi));
        }
        q1 = p1 + p2 / 2.0 + p3 / 4.0;
        q2 = p2 / 2.0 + p3 / 4.0;
        q3 = p3 / 2.0;
        q1 = q1 * (1.0-invarfrac) + invarfrac;
        q2 *= (1.0 - invarfrac);
        q3 *= (1.0 - invarfrac);
        if (!gama && !invar)
          slope = 0.5 * exp(-tt) * (num2 / q2 - num1 / q1) +
                  0.25 * xv * exp(-xv * tt) *
                 ((denominator - num1 - num2) * 2 / q3 - num2 / q2 - num1 / q1);
        else
          slope = 0.5 * (1 / (1 + tt / cvi)) * exp(-cvi * log(1 + tt / cvi)) *
                  (num2 / q2 - num1 / q1) + 0.25 * (xv / (1 + xv * tt / cvi)) *
                    exp(-cvi * log(1 + xv * tt / cvi)) *
                 ((denominator - num1 - num2) * 2 / q3 - num2 / q2 - num1 / q1);
        slope *= (1.0-invarfrac);
        if (slope < 0.0)
          delta = fabs(delta) / -2.0;
        else
          delta = fabs(delta);
        tt += delta;
      }
    }
    if ((delta >= 0.1) && (!similarity)) {
      printf("\nWARNING: DIFFERENCE BETWEEN SPECIES %3ld AND %3ld", m, n);
      if (invar)
        printf(" TOO LARGE FOR INVARIABLE SITES\n");
      else
        printf(" TOO LARGE TO ESTIMATE DISTANCE\n");
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
    }
    vv = fracchange * tt;
  }
  if (!(jukesquick || kimquick || logdet)) {
    prod = (double *)Malloc(sites*sizeof(double));
    prod2 = (double *)Malloc(sites*sizeof(double));
    prod3 = (double *)Malloc(sites*sizeof(double));
    for (i = 0; i < endsite; i++) {
      memcpy(xx1, p->x[i][0], sizeof(sitelike));
      memcpy(xx2, q->x[i][0], sizeof(sitelike));
      xx1freqa = xx1[0] * freqa;
      xx1freqc = xx1[(long)C - (long)A] * freqc;
      xx1freqg = xx1[(long)G - (long)A] * freqg;
      xx1freqt = xx1[(long)T - (long)A] * freqt;
      sum1 = xx1freqa + xx1freqc + xx1freqg + xx1freqt;
      sum2 = freqa * xx2[0] + freqc * xx2[(long)C - (long)A] +
             freqg * xx2[(long)G - (long)A] + freqt * xx2[(long)T - (long)A];
      prod[i] = sum1 * sum2;
      prod2[i] = (xx1freqa + xx1freqg) *
                 (xx2[0] * freqar + xx2[(long)G - (long)A] * freqgr) +
          (xx1freqc + xx1freqt) *
          (xx2[(long)C - (long)A] * freqcy + xx2[(long)T - (long)A] * freqty);
      prod3[i] = xx1freqa * xx2[0] + xx1freqc * xx2[(long)C - (long)A] +
         xx1freqg * xx2[(long)G - (long)A] + xx1freqt * xx2[(long)T - (long)A];
    }
    tt = 0.1;
    delta = 0.1;
    it = 1;
    while (it < iterationsd && fabs(delta) > 0.0000002) {
      slope = 0.0;
      if (tt > 0.0) {
        lz = -tt;
        for (i = 0; i < categs; i++) {
          if (!gama) {
            tbl[i].z1 = exp(tbl[i].ratxv * lz);
            tbl[i].z1zz = exp(tbl[i].rat * lz);
          }
          else {
            tbl[i].z1 = exp(-cvi*log(1.0-tbl[i].ratxv * lz/cvi));
            tbl[i].z1zz = exp(-cvi*log(1.0-tbl[i].rat * lz/cvi));
          }
          tbl[i].y1 = 1.0 - tbl[i].z1;
          tbl[i].z1yy = tbl[i].z1 - tbl[i].z1zz;
          tbl[i].z1xv = tbl[i].z1 * xv;
        }
        for (i = 0; i < endsite; i++) {
          idx = category[alias[i] - 1];
          cc = prod[i];
          bb = prod2[i];
          aa = prod3[i];
          if (!gama && !invar)
            slope += weightrat[i] * (tbl[idx - 1].z1zz * (bb - aa) +
                                     tbl[idx - 1].z1xv * (cc - bb)) /
                         (aa * tbl[idx - 1].z1zz + bb * tbl[idx - 1].z1yy +
                          cc * tbl[idx - 1].y1);
          else
            slope += (1.0-invarfrac) * weightrat[i] * (
                    ((tbl[idx-1].rat)/(1.0-tbl[idx-1].rat * lz/cvi))
                       * tbl[idx - 1].z1zz * (bb - aa) +
                    ((tbl[idx-1].ratxv)/(1.0-tbl[idx-1].ratxv * lz/cvi))
                       * tbl[idx - 1].z1 * (cc - bb)) /
                (aa * ((1.0-invarfrac)*tbl[idx - 1].z1zz + invarfrac)
                  + bb * (1.0-invarfrac)*tbl[idx - 1].z1yy
                  + cc * (1.0-invarfrac)*tbl[idx - 1].y1);
        }
      }
      if (slope < 0.0)
        delta = fabs(delta) / -2.0;
      else
        delta = fabs(delta);
      tt += delta;
      it++;
    }
    if ((delta >= 0.1) && (!similarity)) {
      printf("\nWARNING: DIFFERENCE BETWEEN SPECIES %3ld AND %3ld", m, n);
      if (invar)
        printf(" TOO LARGE FOR INVARIABLE SITES\n");
      else
        printf(" TOO LARGE TO ESTIMATE DISTANCE\n");
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
    }
    vv = tt * fracchange;
    free(prod);
    free(prod2);
    free(prod3);
  }
  if (logdetquick) {  /* compute logdet when no ambiguous nucleotides */
    for (i = 0; i < 4; i++) {
      basefreq1[i] = 0.0;
      basefreq2[i] = 0.0;
      for (j = 0; j < 4; j++)
        basetable[i][j] = 0.0;
    }
    for (i = 0; i < endsite; i++) {
      k = 0;
      while (p->x[i][0][k] == 0.0)
        k++;
      basefreq1[k] += weight[i];
      l = 0;
      while (q->x[i][0][l] == 0.0)
        l++;
      basefreq2[l] += weight[i];
      basetable[k][l] += weight[i];
    }
    vv = lndet(basetable);
    if (vv == 99.0) {
      printf("\nNegative or zero determinant for distance between species");
      printf(" %ld and %ld\n", m, n);
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
    }
    vv = -0.25*(vv - 0.5*(log(basefreq1[0])+log(basefreq1[1])
                                        +log(basefreq1[2])+log(basefreq1[3])
                                        +log(basefreq2[0])+log(basefreq2[1])
                                        +log(basefreq2[2])+log(basefreq2[3])));
  }
  if (similarity) {
    if (denominator < 1.0) {
      printf("\nWARNING: SPECIES %3ld AND %3ld HAVE NO BASES THAT", m, n);
      printf(" CAN BE COMPARED\n");
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
  }
    vv = (double)numerator / denominator;
  }
  *v = vv;
}  /* makev */


void makedists(void)
{
  /* compute distance matrix */
  long i, j;
  double v;

  inittable();
  for (i = 0; i < endsite; i++)
    weightrat[i] = weight[i] * rate[category[alias[i] - 1] - 1];
  if (progress) {
    printf("Distances calculated for species\n");
#ifdef WIN32
    phyFillScreenColor();
#endif
  }
  for (i = 0; i < spp; i++)
    if (similarity)
      d[i][i] = 1.0;
    else
      d[i][i] = 0.0;
  baddists = false;
  for (i = 1; i < spp; i++) {
    if (progress) {
      printf("    ");
      for (j = 0; j < nmlngth; j++)
        putchar(nayme[i - 1][j]);
      printf("   ");
    }
    for (j = i + 1; j <= spp; j++) {
      makev(i, j, &v);
      d[i - 1][j - 1] = v;
      d[j - 1][i - 1] = v;
      if (progress) {
        putchar('.');
        fflush(stdout);
      }
    }
    if (progress) {
      putchar('\n');
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
  }
  if (progress) {
    printf("    ");
    for (j = 0; j < nmlngth; j++)
      putchar(nayme[spp - 1][j]);
    putchar('\n');
  }
  for (i = 0; i < spp; i++) {
    for (j = 0; j < endsite; j++)
      free(nodep[i]->x[j]);
    free(nodep[i]->x);
  }
}  /* makedists */


void writedists(void)
{
  /* write out distances */
  char **names;

  names = stringnames_new();
  output_matrix_d(outfile, d, spp, spp, names, names, matrix_flags);
  stringnames_delete(names);

  if (progress)
    printf("\nDistances written to file \"%s\"\n\n", outfilename);
}  /* writedists */


int main(int argc, Char *argv[])
{  /* DNA Distances by Maximum Likelihood */
#ifdef MAC
  argc = 1;                /* macsetup("Dnadist","");        */
  argv[0] = "Dnadist";
#endif
  init(argc, argv);
  emboss_getoptions("fdnadist", argc, argv);

  ibmpc = IBMCRT;
  ansi = ANSICRT;
  firstset = true;
  doinit();
  ttratio0 = ttratio;

  for (ith = 1; ith <= datasets; ith++) {
    ttratio = ttratio0;
    getinput();
    if (ith == 1)
      firstset = false;
    if (datasets > 1 && progress)
      printf("Data set # %ld:\n\n",ith);
    makedists();
    writedists();
  }
  FClose(infile);
  FClose(outfile);
#ifdef MAC
  fixmacfile(outfilename);
#endif
  printf("Done.\n\n");
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  embExit();
  return 0;
}  /* DNA Distances by Maximum Likelihood */

