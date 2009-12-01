#include "phylip.h"

/* version 3.6. (c) Copyright 1993-1997 by the University of Washington. 
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define epsilong         0.02 /* a small number */    

AjPPhyloFreq phylofreq;

#ifndef OLDC
/* function prototypes */
//void   getoptions(void);
void emboss_getoptions(char *pgm, int argc, char *argv[]);
void   allocrest(void);
void   doinit(void);
void   getalleles(void);
void   inputdata(void);
void   getinput(void);
void   makedists(void);
void   writedists(void);
/* function prototypes */
#endif

const char* outfilename;
AjPFile embossoutfile;

long loci, totalleles, df, datasets, ith;
long nonodes;
long *alleles;
phenotype3 *x;
double **d;
boolean all, cavalli, lower, nei, reynolds,  mulsets,
               firstset, progress;

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
  AjPStr method = NULL;
  all = true;
  cavalli = false;
  lower = false;
  nei = false;
  reynolds = false;
  lower = false;
  progress = true;
  mulsets = false;
  datasets = 1;

  embInitPV(pgm, argc, argv,"PHYLIPNEW",VERSION);

    phylofreq = ajAcdGetFrequencies("infile");

    method = ajAcdGetListSingle("method");
    if(ajStrMatchC(method, "n")) nei = true;
    else if(ajStrMatchC(method, "c")) cavalli = true;
    else if(ajStrMatchC(method, "r")) reynolds = true;

    lower = ajAcdGetBoolean("lower");

    progress = ajAcdGetBoolean("progress");

     embossoutfile = ajAcdGetOutfile("outfile");   
     emboss_openfile(embossoutfile, &outfile, &outfilename);

}  /* emboss_getoptions */


void allocrest()
{
  long i;

  x = (phenotype3 *)Malloc(spp*sizeof(phenotype3));
  d = (double **)Malloc(spp*sizeof(double *));
  for (i = 0; i < (spp); i++)
    d[i] = (double *)Malloc(spp*sizeof(double));
  alleles = (long *)Malloc(loci*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
}  /* allocrest */


void doinit()
{
  /* initializes variables */

  inputnumbersfreq(phylofreq, &spp, &loci, &nonodes, 1);
  allocrest();
}  /* doinit */


void getalleles()
{
  long i;

  if (!firstset)
    samenumspfreq(phylofreq, &loci, ith);
  totalleles = 0;
  for (i = 0; i < loci; i++) {
    alleles[i] = phylofreq->Allele[i];
    totalleles += alleles[i];
  }
  df = totalleles - loci;
}  /* getalleles */


void inputdata()
{
  /* read allele frequencies */
  long i, j, k, m, m1, n;
  double sum;
  ajint ipos = 0;
 
  for (i = 0; i < spp; i++)
    x[i] = (phenotype3)Malloc(totalleles*sizeof(double));
  for (i = 1; i <= (spp); i++) {
    initnamefreq(phylofreq,i-1);
    m = 1;
    for (j = 1; j <= (loci); j++) {
      sum = 0.0;
      n = alleles[j - 1];
      for (k = 1; k <= n; k++) {
        x[i - 1][m - 1] = phylofreq->Data[ipos++];
        sum += x[i - 1][m - 1];
        if (x[i - 1][m - 1] < 0.0) {
          printf("\n\nERROR: Locus %ld in species %ld: an allele", j, i);
          printf(" frequency is negative\n\n");
          embExitBad();
        }
 
        m++;
      }
      if (all && fabs(sum - 1.0) > epsilong) {
        printf(
     "\n\nERROR: Locus %ld in species %ld: frequencies do not add up to 1\n\n",
               j, i);
        for (m1 = 1; m1 <= n; m1 += 1) {
          if (m1 == 1)
            printf("%f", x[i-1][m-n+m1-2]);
          else {
            if ((m1 % 8) == 1)
              printf("\n");
            printf("+%f", x[i-1][m-n+m1-2]);
          }
        }
        printf(" = %f\n\n", sum);
        embExitBad();
      }
      if (!all) {
        x[i - 1][m - 1] = 1.0 - sum;
        if (x[i-1][m-1] < -epsilong) {
          printf("\n\nERROR: Locus %ld in species %ld: ",j,i);
          printf("frequencies add up to more than 1\n\n");
          for (m1 = 1; m1 <= n; m1 += 1) {
            if (m1 == 1)
              printf("%f", x[i-1][m-n+m1-2]);
            else {
              if ((m1 % 8) == 1)
                printf("\n");
              printf("+%f", x[i-1][m-n+m1-2]);
              }
            }
          printf(" = %f\n\n", sum);
          embExitBad();
        }
        m++;
      }
    }
  }

}  /* inputdata */


void getinput()
{
  /* read the input data */
  getalleles();
  inputdata();
}  /* getinput */


void makedists()
{
  long i, j, k;
  double s, s1, s2, s3, f;
  double TEMP;

  if (progress)
    printf("Distances calculated for species\n");
  for (i = 0; i < spp; i++)
    d[i][i] = 0.0;
  for (i = 1; i <= spp; i++) {
    if (progress) {
#ifdef WIN32
      phyFillScreenColor();
#endif
      printf("    ");
      for (j = 0; j < nmlngth; j++)
        putchar(nayme[i - 1][j]);
      printf("   ");
    }
    for (j = 0; j <= i - 1; j++) {
      if (cavalli) {
        s = 0.0;
        for (k = 0; k < (totalleles); k++) {
          f = x[i - 1][k] * x[j][k];
          if (f > 0.0)
            s += sqrt(f);
        }
        d[i - 1][j] = 4 * (loci - s) / df;
      }
      if (nei) {
        s1 = 0.0;
        s2 = 0.0;
        s3 = 0.0;
        for (k = 0; k < (totalleles); k++) {
          s1 += x[i - 1][k] * x[j][k];
          TEMP = x[i - 1][k];
          s2 += TEMP * TEMP;
          TEMP = x[j][k];
          s3 += TEMP * TEMP;
        }
        if (s1 <= 1.0e-20) {
          d[i - 1][j] = -1.0;
          printf("\nWARNING: INFINITE DISTANCE BETWEEN SPECIES ");
          printf("%ld AND %ld; -1.0 WAS WRITTEN\n", i, j);
        } else
          d[i - 1][j] = fabs(-log(s1 / sqrt(s2 * s3)));
      }
      if (reynolds) {
        s1 = 0.0;
        s2 = 0.0;
        for (k = 0; k < (totalleles); k++) {
          TEMP = x[i - 1][k] - x[j][k];
          s1 += TEMP * TEMP;
          s2 += x[i - 1][k] * x[j][k];
        }
        d[i - 1][j] = s1 / (loci * 2 - 2 * s2);
      }
      if (progress) {
        putchar('.');
        fflush(stdout);
      }
      d[j][i - 1] = d[i - 1][j];
    }
    if (progress) {
      putchar('\n');
      fflush(stdout);
    }
  }
  if (progress) {
    putchar('\n');
    fflush(stdout);
  }
}  /* makedists */


void writedists()
{
  long i, j, k;

  fprintf(outfile, "%5ld\n", spp);
  for (i = 0; i < (spp); i++) {
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
    printf("Distances written to file \"%s\"\n\n", outfilename);
}  /* writedists */


int main(int argc, Char *argv[])
{  /* main program */
#ifdef MAC
  argc = 1;                /* macsetup("Gendist","");                */
  argv[0] = "Gendist";
#endif
  init(argc, argv);
  emboss_getoptions("fgendist",argc,argv);
  ibmpc = IBMCRT;
  ansi = ANSICRT;
  firstset = true;
  doinit();
  for (ith = 1; ith <= (datasets); ith++) {
    getinput();
    firstset = false;
    if ((datasets > 1) && progress)
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
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  embExit();
  return 0;
}
