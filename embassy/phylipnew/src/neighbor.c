
#include "phylip.h"
#include "dist.h"

/* version 3.6. (c) Copyright 1993-2002 by the University of Washington.
   Written by Mary Kuhner, Jon Yamato, Joseph Felsenstein, Akiko Fuseki,
   Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */


AjPPhyloDist phylodist = NULL;
AjPPhyloTree* phylotrees;

#ifndef OLDC
/* function prototypes */
//void getoptions(void);
void   emboss_getoptions(char *pgm, int argc, char *argv[]);
void allocrest(void);
void doinit(void);
void inputoptions(void);
void getinput(void);
void describe(node *, double);
void summarize(void);
void nodelabel(boolean);
void jointree(void);
void maketree(void);
/* function prototypes */
#endif

const char* outfilename;
const char* outtreename;
AjPFile embossoutfile;
AjPFile embossouttree;

Char infilename[FNMLNGTH];
long nonodes2, outgrno, col, datasets, ith;
long inseed;
vector *x;
intvector *reps;
boolean jumble, lower, upper, outgropt, replicates, trout,
               printdata, progress, treeprint, mulsets, njoin;
tree curtree;
longer seed;
long *enterorder;
Char progname[20];

/* variables for maketree, propagated globally for C version: */
node **cluster;


void   emboss_getoptions(char *pgm, int argc, char *argv[])
{
  AjPStr matrixtype = NULL;
  AjPStr treetype=NULL;

  long inseed0 = 0;
  putchar('\n');
  jumble = false;
  lower = false;
  outgrno = 1;
  outgropt = false;
  replicates = false;
  trout = true;
  upper = false;
  printdata = false;
  progress = true;
  treeprint = true;
  njoin = true;
  mulsets = false;
  datasets = 1;

    embInitP (pgm, argc, argv, "PHYLIPNEW");
   
    matrixtype = ajAcdGetListSingle("matrixtype");
    if(ajStrMatchC(matrixtype, "l")) lower = true;
    else if(ajStrMatchC(matrixtype, "u")) upper = true;

    phylodist = ajAcdGetDistances("datafile");

    treetype = ajAcdGetListSingle("treetype");
    if(ajStrMatchC(treetype, "n")) njoin = true;
    else if(ajStrMatchC(treetype, "u")) njoin = false;

    if(njoin) {
      outgrno = ajAcdGetInt("outgrno");
      if(outgrno != 0) outgropt = true;
      else outgrno = 1;
    }

    replicates = ajAcdGetBool("replicates");

    jumble = ajAcdGetToggle("jumble");
    if(jumble) {
        inseed = ajAcdGetInt("seed"); 
        emboss_initseed(inseed, &inseed0, seed);
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

    embossoutfile = ajAcdGetOutfile("outfile");   
    embossouttree = ajAcdGetOutfile("outtreefile");

    emboss_openfile(embossoutfile, &outfile, &outfilename);
    if(trout) emboss_openfile(embossouttree, &outtree, &outtreename);

  fprintf(outfile, "\nNeighbor-Joining/UPGMA method version %s\n\n",VERSION);


}  /* emboss_getoptions */


void allocrest()
{
  long i;

  x = (vector *)Malloc(spp*sizeof(vector));
  for (i = 0; i < spp; i++)
    x[i] = (vector)Malloc(spp*sizeof(double));
  reps = (intvector *)Malloc(spp*sizeof(intvector));
  for (i = 0; i < spp; i++)
    reps[i] = (intvector)Malloc(spp*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  enterorder = (long *)Malloc(spp*sizeof(long));
  cluster = (node **)Malloc(spp*sizeof(node *));
}  /* allocrest */


void doinit()
{
  /* initializes variables */
  node *p;

  inputnumbers2seq(phylodist, &spp, &nonodes2, 2);
  nonodes2 += (njoin ? 0 : 1);
  alloctree(&curtree.nodep, nonodes2+1);
  p = curtree.nodep[nonodes2]->next->next;
  curtree.nodep[nonodes2]->next = curtree.nodep[nonodes2];
  free(p);
  allocrest();
}  /* doinit */


void inputoptions()
{
  /* read options information */

  if (ith != 1)
    samenumspseq2(phylodist, ith);
  putc('\n', outfile);
  if (njoin)
    fprintf(outfile, " Neighbor-joining method\n");
  else
    fprintf(outfile, " UPGMA method\n");
  fprintf(outfile, "\n Negative branch lengths allowed\n\n");
}  /* inputoptions */


void describe(node *p, double height)
{
  /* print out information for one branch */
  long i;
  node *q;

  q = p->back;
  if (njoin)
    fprintf(outfile, "%4ld          ", q->index - spp);
  else
    fprintf(outfile, "%4ld     ", q->index - spp);
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index - 1][i], outfile);
    putc(' ', outfile);
  } else {
    if (njoin)
      fprintf(outfile, "%4ld       ", p->index - spp);
    else {
      fprintf(outfile, "%4ld       ", p->index - spp);
    }
  }
  if (njoin)
    fprintf(outfile, "%12.5f\n", q->v);
  else
    fprintf(outfile, "%10.5f      %10.5f\n", q->v, q->v+height);
  if (!p->tip) {
    describe(p->next->back, height+q->v);
    describe(p->next->next->back, height+q->v);
  }
}  /* describe */


void summarize()
{
  /* print out branch lengths etc. */
  putc('\n', outfile);
  if (njoin) {
    fprintf(outfile, "remember:");
    if (outgropt)
      fprintf(outfile, " (although rooted by outgroup)");
    fprintf(outfile, " this is an unrooted tree!\n");
  }
  if (njoin) {
    fprintf(outfile, "\nBetween        And            Length\n");
    fprintf(outfile, "-------        ---            ------\n");
  } else {
    fprintf(outfile, "From     To            Length          Height\n");
    fprintf(outfile, "----     --            ------          ------\n");
  }
  describe(curtree.start->next->back, 0.0);
  describe(curtree.start->next->next->back, 0.0);
  if (njoin)
    describe(curtree.start->back, 0.0);
  fprintf(outfile, "\n\n");
}  /* summarize */


void nodelabel(boolean isnode)
{
  if (isnode)
    printf("node");
  else
    printf("species");
}  /* nodelabel */


void jointree()
{
  /* calculate the tree */
  long nc, nextnode, mini=0, minj=0, i, j, ia, ja, ii, jj, nude, iter;
  double fotu2, total, tmin, dio, djo, bi, bj, bk, dmin=0, da;
  long el[3];
  vector av;
  intvector oc;

  double *R;   /* added in revisions by Y. Ina */
  R = (double *)Malloc(spp * sizeof(double));

  for (i = 0; i <= spp - 2; i++) {
    for (j = i + 1; j < spp; j++) {
      da = (x[i][j] + x[j][i]) / 2.0;
      x[i][j] = da;
      x[j][i] = da;
    }
  }
  /* First initialization */
  fotu2 = spp - 2.0;
  nextnode = spp + 1;
  av = (vector)Malloc(spp*sizeof(double));
  oc = (intvector)Malloc(spp*sizeof(long));
  for (i = 0; i < spp; i++) {
    av[i] = 0.0;
    oc[i] = 1;
  }
  /* Enter the main cycle */
  if (njoin)
    iter = spp - 3;
  else
    iter = spp - 1;
  for (nc = 1; nc <= iter; nc++) {
    for (j = 2; j <= spp; j++) {
      for (i = 0; i <= j - 2; i++)
        x[j - 1][i] = x[i][j - 1];
    }
    tmin = 99999.0;
    /* Compute sij and minimize */
    if (njoin) {     /* many revisions by Y. Ina from here ... */
      for (i = 0; i < spp; i++)
        R[i] = 0.0;
      for (ja = 2; ja <= spp; ja++) {
        jj = enterorder[ja - 1];
        if (cluster[jj - 1] != NULL) {
          for (ia = 0; ia <= ja - 2; ia++) {
            ii = enterorder[ia];
            if (cluster[ii - 1] != NULL) {
              R[ii - 1] += x[ii - 1][jj - 1];
              R[jj - 1] += x[ii - 1][jj - 1];
            }
          }
        }
      }
    } /* ... to here */
    for (ja = 2; ja <= spp; ja++) {
      jj = enterorder[ja - 1];
      if (cluster[jj - 1] != NULL) {
        for (ia = 0; ia <= ja - 2; ia++) {
          ii = enterorder[ia];
          if (cluster[ii - 1] != NULL) {
            if (njoin) {
              total = fotu2 * x[ii - 1][jj - 1] - R[ii - 1] - R[jj - 1];
               /* this statement part of revisions by Y. Ina */
            } else
              total = x[ii - 1][jj - 1];
            if (total < tmin) {
              tmin = total;
              mini = ii;
              minj = jj;
            }
          }
        }
      }
    }
    /* compute lengths and print */
    if (njoin) {
      dio = 0.0;
      djo = 0.0;
      for (i = 0; i < spp; i++) {
        dio += x[i][mini - 1];
        djo += x[i][minj - 1];
      }
      dmin = x[mini - 1][minj - 1];
      dio = (dio - dmin) / fotu2;
      djo = (djo - dmin) / fotu2;
      bi = (dmin + dio - djo) * 0.5;
      bj = dmin - bi;
      bi -= av[mini - 1];
      bj -= av[minj - 1];
    } else {
      bi = x[mini - 1][minj - 1] / 2.0 - av[mini - 1];
      bj = x[mini - 1][minj - 1] / 2.0 - av[minj - 1];
      av[mini - 1] += bi;
    }
    if (progress) {
      printf("Cycle %3ld: ", iter - nc + 1);
      if (njoin)
        nodelabel((boolean)(av[mini - 1] > 0.0));
      else
        nodelabel((boolean)(oc[mini - 1] > 1.0));
      printf(" %ld (%10.5f) joins ", mini, bi);
      if (njoin)
        nodelabel((boolean)(av[minj - 1] > 0.0));
      else
        nodelabel((boolean)(oc[minj - 1] > 1.0));
      printf(" %ld (%10.5f)\n", minj, bj);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
    hookup(curtree.nodep[nextnode - 1]->next, cluster[mini - 1]);
    hookup(curtree.nodep[nextnode - 1]->next->next, cluster[minj - 1]);
    cluster[mini - 1]->v = bi;
    cluster[minj - 1]->v = bj;
    cluster[mini - 1]->back->v = bi;
    cluster[minj - 1]->back->v = bj;
    cluster[mini - 1] = curtree.nodep[nextnode - 1];
    cluster[minj - 1] = NULL;
    nextnode++;
    if (njoin)
      av[mini - 1] = dmin * 0.5;
    /* re-initialization */
    fotu2 -= 1.0;
    for (j = 0; j < spp; j++) {
      if (cluster[j] != NULL) {
        if (njoin) {
          da = (x[mini - 1][j] + x[minj - 1][j]) * 0.5;
          if (mini - j - 1 < 0)
            x[mini - 1][j] = da;
          if (mini - j - 1 > 0)
            x[j][mini - 1] = da;
        } else {
          da = x[mini - 1][j] * oc[mini - 1] + x[minj - 1][j] * oc[minj - 1];
          da /= oc[mini - 1] + oc[minj - 1];
          x[mini - 1][j] = da;
          x[j][mini - 1] = da;
        }
      }
    }
    for (j = 0; j < spp; j++) {
      x[minj - 1][j] = 0.0;
      x[j][minj - 1] = 0.0;
    }
    oc[mini - 1] += oc[minj - 1];
  }
  /* the last cycle */
  nude = 1;
  for (i = 1; i <= spp; i++) {
    if (cluster[i - 1] != NULL) {
      el[nude - 1] = i;
      nude++;
    }
  }
  if (!njoin) {
    curtree.start = cluster[el[0] - 1];
    curtree.start->back = NULL;
    free(av);
    free(oc);
    return;
  }
  bi = (x[el[0] - 1][el[1] - 1] + x[el[0] - 1][el[2] - 1] - x[el[1] - 1]
        [el[2] - 1]) * 0.5;
  bj = x[el[0] - 1][el[1] - 1] - bi;
  bk = x[el[0] - 1][el[2] - 1] - bi;
  bi -= av[el[0] - 1];
  bj -= av[el[1] - 1];
  bk -= av[el[2] - 1];
  if (progress) {
    printf("last cycle:\n");
    putchar(' ');
    nodelabel((boolean)(av[el[0] - 1] > 0.0));
    printf(" %ld  (%10.5f) joins ", el[0], bi);
    nodelabel((boolean)(av[el[1] - 1] > 0.0));
    printf(" %ld  (%10.5f) joins ", el[1], bj);
    nodelabel((boolean)(av[el[2] - 1] > 0.0));
    printf(" %ld  (%10.5f)\n", el[2], bk);
#ifdef WIN32
    phyFillScreenColor();
#endif
  }
  hookup(curtree.nodep[nextnode - 1], cluster[el[0] - 1]);
  hookup(curtree.nodep[nextnode - 1]->next, cluster[el[1] - 1]);
  hookup(curtree.nodep[nextnode - 1]->next->next, cluster[el[2] - 1]);
  cluster[el[0] - 1]->v = bi;
  cluster[el[1] - 1]->v = bj;
  cluster[el[2] - 1]->v = bk;
  cluster[el[0] - 1]->back->v = bi;
  cluster[el[1] - 1]->back->v = bj;
  cluster[el[2] - 1]->back->v = bk;
  curtree.start = cluster[el[0] - 1]->back;
  free(av);
  free(oc);
}  /* jointree */


void maketree()
{
  /* construct the tree */
  long i ;

  dist_inputdata(phylodist, replicates, printdata, lower, upper, x, reps);
  if (njoin && (spp < 3)) {
    printf("\nERROR: Neighbor-Joining runs must have at least 3 species\n\n");
    embExitBad();
  }
  if (progress)
    putchar('\n');
  if (ith == 1)
    setuptree(&curtree, nonodes2 + 1);
  for (i = 1; i <= spp; i++)
    enterorder[i - 1] = i;
  if (jumble)
    randumize(seed, enterorder);
  for (i = 0; i < spp; i++)
    cluster[i] = curtree.nodep[i];
  jointree();
  if (njoin)
    curtree.start = curtree.nodep[outgrno - 1]->back;
  printree(curtree.start, treeprint, njoin, (boolean)(!njoin));
  if (treeprint)
    summarize();
  if (trout) {
    col = 0;
    if (njoin)
      treeout(curtree.start, &col, 0.43429448222, njoin, curtree.start);
    else
      curtree.root = curtree.start,
      treeoutr(curtree.start,&col,&curtree);
  }
  if (progress) {
    printf("\nOutput written on file \"%s\"\n\n", outfilename);
    if (trout)
      printf("Tree written on file \"%s\"\n\n", outtreename);
  }
}  /* maketree */


int main(int argc, Char *argv[])
{  /* main program */
#ifdef MAC
  argc = 1;                /* macsetup("Neighbor","");                */
  argv[0] = "Neighbor";
#endif
  init(argc, argv);
  emboss_getoptions("fneighbor",argc,argv);

  ibmpc = IBMCRT;
  ansi = ANSICRT;

  doinit();

  ith = 1;
  while (ith <= datasets) {
    if (datasets > 1) {
      fprintf(outfile, "Data set # %ld:\n",ith);
      if (progress)
        printf("Data set # %ld:\n",ith);
    }
    inputoptions();
    maketree();
    ith++;
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
  embExit();
  return 0;
}
