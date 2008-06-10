

#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1994-2004 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define initialv        0.1     /* starting value of branch length          */
#define iterationsr     20      /* how many Newton iterations per distance  */

extern sequence y;

AjPPhyloState* phylostates = NULL;


#ifndef OLDC
/* function prototypes */
void restdist_inputnumbers(AjPPhyloState);
//void getoptions(void);
void   emboss_getoptions(char *pgm, int argc, char *argv[]);
void allocrest(void);
void doinit(void);
void inputoptions(AjPPhyloState);
void restdist_inputdata(AjPPhyloState);
void restdist_sitesort(void);
void restdist_sitecombine(void);
void makeweights(void);
void makev(long, long, double *);
void makedists(void);
void writedists(void);
void getinput(void);
void reallocsites(void);
/* function prototypes */
#endif


Char infilename[FNMLNGTH];

const char* outfilename;
AjPFile embossoutfile;

long sites, weightsum, datasets, ith;
boolean  restsites, neili, gama, weights, lower,
           progress, mulsets, firstset;
double ttratio, fracchange, cvi, sitelength, xi, xv;
double **d;
steptr aliasweight;
char *progname;
Char ch;

void restdist_inputnumbers(AjPPhyloState state)
{
  /* read and print out numbers of species and sites */
  spp = state->Size;
  sites = state->Len;
}  /* restdist_inputnumbers */


void emboss_getoptions(char *pgm, int argc, char *argv[])
{

  ajint numseqs;

  sitelength = 6.0;
  neili = false;
  gama = false;
  cvi = 0.0;
  weights = false;
  lower = false;
  printdata = false;
  progress = true;
  restsites = true;
  ttratio = 2.0;
  mulsets = false;
  datasets = 1;
    printf("\nRestriction site or fragment distances, ");
    printf("version %s\n\n",VERSION);
 
    embInitP (pgm, argc, argv, "PHYLIPNEW");

    phylostates = ajAcdGetDiscretestates("data");

    numseqs = 0;
    while (phylostates[numseqs])
	numseqs++;

    if (numseqs > 1) {
	mulsets = true;
        datasets = numseqs;
    }

    restsites = ajAcdGetBoolean("restsites");
    neili = ajAcdGetBoolean("neili");
    if(!neili) gama = ajAcdGetBoolean("gamma");
    if(gama) {
      cvi = ajAcdGetFloat("gammacoefficient");
      cvi = 1.0 / (cvi * cvi);
    }
    ttratio = ajAcdGetFloat("ttratio");
    sitelength = ajAcdGetInt("sitelength");
    lower = ajAcdGetBoolean("lower");
    printdata = ajAcdGetBoolean("printdata");
    progress = ajAcdGetBoolean("progress");

    xi = (ttratio - 0.5)/(ttratio + 0.5);
    xv = 1.0 - xi;
    fracchange = xi*0.5 + xv*0.75;

     embossoutfile = ajAcdGetOutfile("outfile");   
     emboss_openfile(embossoutfile, &outfile, &outfilename);

}  /* emboss_getoptions */


void reallocsites() 
{
  long i;

  for (i = 0; i < spp; i++){
    free(y[i]);
    y[i] = (Char *)Malloc(sites*sizeof(Char));
  }
  
  free(weight);
  free(alias);
  free(aliasweight);

  weight = (steptr)Malloc((sites+1)*sizeof(long));
  alias = (steptr)Malloc((sites+1)*sizeof(long));
  aliasweight = (steptr)Malloc((sites+1)*sizeof(long));
  makeweights();
}

void allocrest()
{
  long i;

  y = (Char **)Malloc(spp*sizeof(Char *));
  for (i = 0; i < spp; i++)
    y[i] = (Char *)Malloc((sites+1)*sizeof(Char));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  weight = (steptr)Malloc((sites+1)*sizeof(long));
  alias = (steptr)Malloc((sites+1)*sizeof(long));
  aliasweight = (steptr)Malloc((sites+1)*sizeof(long));
  d = (double **)Malloc(spp*sizeof(double *));
  for (i = 0; i < spp; i++)
    d[i] = (double*)Malloc(spp*sizeof(double));
}  /* allocrest */


void doinit()
{
  /* initializes variables */
  restdist_inputnumbers(phylostates[0]);
  if (printdata)
    fprintf(outfile, "\n %4ld Species, %4ld Sites\n", spp, sites);
  allocrest();
}  /* doinit */


void inputoptions(AjPPhyloState state)
{
  /* read the options information */
 
  long i, extranum, cursp, curst;

  if (!firstset) {
    cursp = state->Size;
    curst = state->Len; 
    if (cursp != spp) {
      printf("\nERROR: INCONSISTENT NUMBER OF SPECIES IN DATA SET %4ld\n",
             ith);
      embExitBad();
    }
    sites = curst;
    reallocsites();
  }
  for (i = 1; i <= sites; i++)
    weight[i] = 1;
  weightsum = sites;
  extranum = 0;

  /*  fscanf(infile, "%*[ 0-9]");
  readoptions(&extranum, "W");
  for (i = 1; i <= extranum; i++) {
      matchoptions(&ch, "W");
      inputweights2(1, sites+1, &weightsum, weight, &weights, "RESTDIST");
      }
    */

}  /* inputoptions */


void restdist_inputdata(AjPPhyloState state)
{
  /* read the species and sites data */
  long i, j, k, l, sitesread = 0;
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
        initnamestate(state, i - 1);
        str = state->Str[i-1];
        j = 0;
      done = false;
      while (!done) {
        while (j < sites) {
          ch = ajStrGetCharPos(str, j);
          if (ch != '1' && ch != '0' && ch != '+' && ch != '-' && ch != '?') {
            printf(" ERROR -- Bad symbol %c",ch);
            printf(" at position %ld of species %ld\n", j+1, i);
            embExitBad();
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
}  /* restdist_inputdata */


void restdist_sitesort()
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
}  /* restdist_sitesort */


void restdist_sitecombine()
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
}  /* restdist_sitecombine */


void makeweights()
{
  /* make up weights vector to avoid duplicate computations */
  long i;

  for (i = 1; i <= sites; i++) {
    alias[i] = i;
    aliasweight[i] = weight[i];
  }
  restdist_sitesort();
  restdist_sitecombine();
  sitescrunch2(sites + 1, 2, 3, aliasweight);
  for (i = 1; i <= sites; i++) {
    weight[i] = aliasweight[i];
    if (weight[i] > 0)
      endsite = i;
  }
  weight[0] = 1;
}  /* makeweights */


void makev(long m, long n, double *v)
{
  /* compute one distance */
  long i, ii, it, numerator, denominator;
  double f, g=0, h, p1, p2, p3, q1, pp, tt, delta, vv;

  numerator = 0;
  denominator = 0;
  for (i = 0; i < endsite; i++) {
    ii = alias[i + 1];
    if ((y[m-1][ii-1] == '+') ||
	(y[n-1][ii-1] == '+')) {
      denominator += weight[i+1];
      if ((y[m-1][ii-1] == '+') &&
	  (y[n-1][ii-1] == '+')) {
        numerator += weight[i + 1];
      }
    }
  }
  f = 2*numerator/(double)(denominator+numerator);
  if (restsites) {
    if (exp(-sitelength*1.38629436) > f) {
      printf("\nERROR: Infinite distance between ");
      printf(" species %3ld and %3ld\n", m, n);
      embExitBad();
    }
  }
  if (!restsites) {
    if (!neili) {
      f = (sqrt(f*(f+8.0))-f)/2.0;
    }
    else {
      g = initialv;
      delta = g;
      it = 0;
      while (fabs(delta) > 0.00002 && it < iterationsr) {
        it++;
        h = g;
        g = exp(0.25*log(f * (3-2*g)));
        delta = g - h;
      }
    }
  }
  if ((!restsites) && neili)
    vv = - (2.0/sitelength) * log(g);
  else {
    if(neili && restsites){
      pp = exp(log(f)/(2*sitelength));
      vv = -(3.0/2.0)*log((4.0/3.0)*pp - (1.0/3.0));      
    } else {
      pp = exp(log(f)/sitelength); 
      delta = initialv;
      tt = delta;
      it = 0;
      while (fabs(delta) > 0.00002 && it < iterationsr) {
        it++;
        if (gama) {
          p1 = exp(-cvi * log(1 + tt / cvi));
          p2 = exp(-cvi * log(1 + xv * tt / cvi))
            - exp(-cvi * log(1 + tt / cvi));
          p3 = 1.0 - exp(-cvi * log(1 + xv * tt / cvi));
        } else {
          p1 = exp(-tt);
          p2 = exp(-xv * tt) - exp(-tt);
          p3 = 1.0 - exp(-xv * tt);
        }
        q1 = p1 + p2 / 2.0 + p3 / 4.0;
        g = q1 - pp;
        if (g < 0.0)
          delta = fabs(delta) / -2.0;
        else
          delta = fabs(delta);
        tt += delta;
      }
      vv = fracchange * tt;
    }
  }
  *v = vv;
}  /* makev */


void makedists()
{
  /* compute distance matrix */
  long i, j;
  double v;

  if (progress)
    printf("Distances calculated for species\n");
  for (i = 0; i < spp; i++)
    d[i][i] = 0.0;
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
      if (progress)
        putchar('.');
    }
    if (progress)
      putchar('\n');
  }
  if (progress) {
    printf("    ");
    for (j = 0; j < nmlngth; j++)
      putchar(nayme[spp - 1][j]);
    putchar('\n');
  }
}  /* makedists */


void writedists()
{
  /* write out distances */
  long i, j, k;

  if (!printdata)
    fprintf(outfile, "%5ld\n", spp);
  for (i = 0; i < spp; i++) {
    for (j = 0; j < nmlngth; j++)
      putc(nayme[i][j], outfile);
    if (lower)
      k = i;
    else
      k = spp;
    for (j = 1; j <= k; j++) {
      if (d[i][j-1] < 100.0)
	fprintf(outfile, "%10.6f", d[i][j-1]);
      else if (d[i][j-1] < 1000.0)
        fprintf(outfile, " %10.6f", d[i][j-1]);
        else
          fprintf(outfile, " %11.6f", d[i][j-1]);
      if ((j + 1) % 7 == 0 && j < k)
        putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  if (progress)
    printf("\nDistances written to file \"%s\"\n\n", outfilename);
}  /* writedists */


void getinput()
{
  /* reads the input data */
  inputoptions(phylostates[ith-1]);
  restdist_inputdata(phylostates[ith-1]);
  makeweights();
}  /* getinput */


int main(int argc, Char *argv[])
{  /* distances from restriction sites or fragments */
#ifdef MAC
  argc = 1;                /* macsetup("Restdist",""); */
  argv[0] = "Restdist";
#endif
  init(argc,argv);
  emboss_getoptions("frestdist", argc, argv);
  progname = argv[0];

  ibmpc = IBMCRT;
  ansi = ANSICRT;
  firstset = true;
  doinit();
  for (ith = 1; ith <= datasets; ith++) {
    getinput();
    if (ith == 1)
      firstset = false;
    if (datasets > 1 && progress)
      printf("\nData set # %ld:\n\n",ith);
    makedists();
    writedists();
  }
  FClose(infile);
  FClose(outfile);
#ifdef MAC
  fixmacfile(outfilename);
#endif
  printf("Done.\n\n");
  embExit();
  return 0;
}  /* distances from restriction sites or fragments */
