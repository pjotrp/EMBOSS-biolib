#include "phylip.h"

/* version 3.6. (c) Copyright 1993-1997 by the University of Washington. 
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define epsilong         0.02 /* a small number                            */

AjPPhyloFreq phylofreq = NULL;		/* should be an array */

#ifndef OLDC
/* function prototypes */
/*void   getoptions(void);*/
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



Char infilename[FNMLNGTH];
const char* outfilename;
long loci, totalleles, df, datasets, ith;
long nonodes;
long *alleles;
phenotype3 *x;
double **d;
boolean all, cavalli, lower, nei, reynolds,  mulsets,
               firstset, progress;


/************ EMBOSS GET OPTIONS ROUTINES ******************************/

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjStatus retval;
 
    /* initialize global variables */

    ajNamInit("emboss");
    retval =  ajAcdInitP (pgm, argc, argv, "PHYLIP");

    /* ajAcdGet */

    /* init functions for standard ajAcdGet */

    /* cleanup for clashing options */

}

/************ END EMBOSS GET OPTIONS ROUTINES **************************/

/*
//void getoptions()
//{
//  /# interactively set options #/
//  long loopcount;
//  Char ch;
//
//  all = false;
//  cavalli = false;
//  lower = false;
//  nei = true;
//  reynolds = false;
//  lower = false;
//  progress = true;
//  loopcount = 0;
//  for (;;) {
//    cleerhome();
//    printf("\nGenetic Distance Matrix program, version %s\n\n",VERSION);
//    printf("Settings for this run:\n");
//    printf("  A   Input file contains all alleles at each locus?  %s\n",
//           all ? "Yes" : "One omitted at each locus");
//    printf("  N                        Use Nei genetic distance?  %s\n",
//           nei ? "Yes" : "No");
//    printf("  C                Use Cavalli-Sforza chord measure?  %s\n",
//           cavalli ? "Yes" : "No");
//    printf("  R                   Use Reynolds genetic distance?  %s\n",
//           reynolds ? "Yes" : "No");
//    printf("  L                         Form of distance matrix?  %s\n",
//           lower ? "Lower-triangular" : "Square");
//    printf("  M                      Analyze multiple data sets?");
//    if (mulsets)
//      printf("  Yes, %2ld sets\n", datasets);
//    else
//      printf("  No\n");
//    printf("  0              Terminal type (IBM PC, ANSI, none)?  %s\n",
//           ibmpc ? "IBM PC" : ansi  ? "ANSI" : "(none)");
//    printf("  1            Print indications of progress of run?  %s\n",
//           progress ? "Yes" : "No");
//    printf("\n  Y to accept these or type the letter for one to change\n");
//#ifdef WIN32
//    phyFillScreenColor();
//#endif
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    uppercase(&ch);
//    if (ch == 'Y')
//      break;
//    if (strchr("ACNMRL01", ch) != NULL) {
//      switch (ch) {
//        
//      case 'A':
//        all = !all;
//        break;
//        
//      case 'C':
//        cavalli = true;
//        nei = false;
//        reynolds = false;
//        break;
//        
//      case 'N':
//        cavalli = false;
//        nei = true;
//        reynolds = false;
//        break;
//        
//      case 'R':
//        reynolds = true;
//        cavalli = false;
//        nei = false;
//        break;
//        
//      case 'L':
//        lower = !lower;
//        break;
//        
//      case 'M':
//        mulsets = !mulsets;
//        if (mulsets)
//          initdatasets(&datasets);
//        break;
//        
//      case '0':
//        initterminal(&ibmpc, &ansi);
//        break;
//        
//      case '1':
//        progress = !progress;
//        break;
//      }
//    } else
//      printf("Not a possible option!\n");
//    countup(&loopcount, 100);
//  }
//  putchar('\n');
//}  /# getoptions #/
*/


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
/*  getoptions();*/
  allocrest();
}  /* doinit */


void getalleles()
{
  long i;

  if (!firstset)
    samenumspfreq(phylofreq, &loci, ith);
  totalleles = 0;
  for (i = 0; i < (loci); i++) {
    alleles[i] = phylofreq->Allele[i];
    totalleles += alleles[i];
  }
  df = totalleles - loci;
}  /* getalleles */


void inputdata()
{
  /* read allele frequencies */
  long i, j, k, m, n, p;
  double sum;
  ajint ipos = 0;

  for (i = 0; i < spp; i++)
    x[i] = (phenotype3)Malloc(totalleles*sizeof(double));
  for (i = 1; i <= (spp); i++) {
    initnamefreq(phylofreq,i-1);
    m = 1;
    p = 1;
    for (j = 1; j <= (loci); j++) {
      sum = 0.0;
      n = alleles[j - 1];
      for (k = 1; k <= n; k++) {
        x[i - 1][m - 1] = phylofreq->Data[ipos++];
        sum += x[i - 1][m - 1];
        if (x[i - 1][m - 1] < 0.0) {
          printf("\n\nERROR: Locus %ld in species %ld: an allele", j, i);
          printf(" frequency is negative\n\n");
          exxit(-1);
        }
        p++;
        m++;
      }
      if (all && fabs(sum - 1.0) > epsilong) {
        printf(
     "\n\nERROR: Locus %ld in species %ld: frequencies do not add up to 1\n\n",
               j, i);
        exxit(-1);
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
          else f = 0.0;
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
      fprintf(outfile, "%8.4f", d[i][j - 1]);
      if ((j + 1) % 9 == 0 && j < k)
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
  /*openfile(&infile,INFILE,"input file", "r",argv[0],infilename);*/
  openfile(&outfile,OUTFILE,"output file", "w",argv[0],&outfilename);

  ibmpc = IBMCRT;
  ansi = ANSICRT;
  mulsets = false;
  firstset = true;
  datasets = 1;
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
  return 0;
}
