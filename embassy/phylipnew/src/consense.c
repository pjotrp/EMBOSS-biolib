#include "phylip.h"
#include "cons.h"

/* version 3.6. (c) Copyright 1993-2002 by the University of Washington.
   Written by Joseph Felsenstein, Hisashi Horino,
   Akiko Fuseki, Dan Fineman, Sean Lamont, and Andrew Keeffe.
   Permission is granted
   to copy and use this program provided no fee is charged for it and
   provided that this copyright notice is not removed. */


/* The following extern's refer to things declared in cons.c */

AjPPhyloTree* phylotrees = NULL;

extern int tree_pairing;

extern Char intreename[FNMLNGTH], intree2name[FNMLNGTH];

const char* outfilename;
const char* outtreename;
AjPFile embossoutfile;
AjPFile embossouttree;

long trees_in;

extern node *root;

extern long numopts, outgrno, col, setsz;
extern long maxgrp;               /* max. no. of groups in all trees found  */

extern boolean trout, firsttree, noroot, outgropt, didreroot, prntsets,
          progress, treeprint, goteof, strict, mr, mre, ml;
extern pointarray nodep;                 /* pointers to all nodes in tree */
extern group_type **grouping, **grping2, **group2;/* to store groups found  */
extern long **order, **order2, lasti;
extern group_type *fullset;
extern long tipy;

extern double trweight, ntrees, mlfrac;

#ifndef OLDC
/* function prototypes */
//void   getoptions(void);
void   emboss_getoptions(char *pgm, int argc, char *argv[]);
void   count_siblings(node **p);
void   treeout(node *);
/* function prototypes */
#endif


void   emboss_getoptions(char *pgm, int argc, char *argv[])
{

  AjPStr method;

  /* Initial settings */
  ibmpc          = IBMCRT;
  ansi           = ANSICRT;
  didreroot      = false;
  firsttree      = true;
  spp            = 0 ;
  col            = 0 ;

  /* This is needed so functions in cons.c work */
  tree_pairing   = NO_PAIRING ;  

  strict = false;
  mr = false;
  mre = false;
  ml = false;
  mlfrac = 0.5;
  noroot = true;
  numopts = 0;
  outgrno = 1;
  outgropt = false;
  trout = true;
  prntsets = true;
  progress = true;
  treeprint = true;

  embInitP (pgm, argc, argv,"PHYLIPNEW");

    phylotrees = ajAcdGetTree("intreefile");
    trees_in = 0;
    while (phylotrees[trees_in])
	trees_in++;

    method = ajAcdGetListI("method", 1);
    if (ajStrMatchC(method, "strict")) strict = true;
    else if(ajStrMatchC(method, "mr")) mr = true;
    else if(ajStrMatchC(method, "mre")) mre = true;
    else if(ajStrMatchC(method, "ml")) {
      ml = true;
      mlfrac = ajAcdGetFloat("mlfrac");
    }

    prntsets = ajAcdGetBool("prntsets");
    progress = ajAcdGetBool("progress");
    treeprint = ajAcdGetBool("treeprint");
    trout = ajAcdGetToggle("trout");
    outgrno = ajAcdGetInt("outgrno");
    if(outgrno != 0) outgropt = true;
    else outgrno = 1;

    noroot =  !ajAcdGetToggle("root");

  embossoutfile = ajAcdGetOutfile("outfile");   
  emboss_openfile(embossoutfile, &outfile, &outfilename);
     
  
  if(trout) {
    embossouttree = ajAcdGetOutfile("outtreefile");
    emboss_openfile(embossouttree, &outtree, &outtreename);
  }


  fprintf(outfile, "\nConsensus tree");
  fprintf(outfile, " program, version %s\n\n", VERSION);

}  /* emboss_getoptions */


void count_siblings(node **p)
{
  node *tmp_node;
  int i;

  if (!(*p)) {
    /* This is a leaf, */
    return;
  } else {
    tmp_node = (*p)->next;
  }

  for (i = 0 ; i < 1000; i++) {
    if (tmp_node == (*p)) {
      /* When we've gone through all the siblings, */
      break;
    } else if (tmp_node) {
      tmp_node = tmp_node->next;
    } else  {
      /* Should this be executed? */
      return ;
    }
  }
} /* count_siblings */


void treeout(node *p)
{
  /* write out file with representation of final tree */
  long i, n = 0;
  Char c;
  node *q;
  double x;

  count_siblings (&p);  

  if (p->tip) {
    /* If we're at a node which is a leaf, figure out how long the
       name is and print it out. */
    for (i = 1; i <= MAXNCH; i++) {
      if (p->nayme[i - 1] != '\0')
        n = i;
    }
    for (i = 0; i < n; i++) {
      c = p->nayme[i];
      if (c == ' ')
        c = '_';
      putc(c, outtree);
    }
    col += n;
  } else {
    /* If we're at a furcation, print out the proper formatting, loop
       through all the children, calling the procedure recursively. */
    putc('(', outtree);
    col++;
    q = p->next;
    while (q != p) {
      /* This should terminate when we've gone through all the
         siblings, */
      treeout(q->back);
      q = q->next;
      if (q == p)
        break;
      putc(',', outtree);
      col++;
      if (col > 60) {
        putc('\n', outtree);
        col = 0;
      }
    }
    putc(')', outtree);
    col++;
  }

  if (p->tip)
    x = ntrees;
  else
    x = (double)p->deltav;

  if (p == root) {
    /* When we're all done with this tree, */
    fprintf(outtree, ";\n");
    return;
  }

  /* Figure out how many characters the branch length requires: */
  else {
    if (!strict) {
      if (x >= 100.0) { 
        fprintf(outtree, ":%5.1f", x);
        col += 4;
      } else if (x >= 10.0) {
          fprintf(outtree, ":%4.1f", x); 
          col += 3;
        } else if (x >= 0.99) {
            fprintf(outtree, ":%3.1f", x);
            col += 2;
          } else {
            fprintf(outtree, ":%4.2f", x); 
            col += 3;
          }
    }
  }
}  /* treeout */


int main(int argc, Char *argv[])
{  
  /* Local variables added by Dan F. */
  pattern_elm  ***pattern_array;
  double *timesseen_changes = NULL;

  long i, j;
  node *p, *q;

#ifdef MAC
  argc = 1;                /* macsetup("Consense", "");        */
  argv[0] = "Consense";
#endif
  init(argc, argv);
  emboss_getoptions("fconsense", argc, argv);

  ntrees = 0.0;
  maxgrp = 32767;   /* initial size of set hash table */
  lasti  = -1;

  if (prntsets)
    fprintf(outfile, "Species in order: \n\n");

  /* Read the tree file and put together grouping, order, and timesseen */
  read_groups (&pattern_array, timesseen_changes, trees_in, trees_in, phylotrees);
  /* Compute the consensus tree. */
  putc('\n', outfile);
  nodep      = (pointarray)Malloc(2*(1+spp)*sizeof(node *));
  for (i = 0; i < spp; i++) {
    nodep[i] = (node *)Malloc(sizeof(node));
    for (j = 0; j < MAXNCH; j++)
      nodep[i]->nayme[j] = '\0';
    strncpy(nodep[i]->nayme, nayme[i], MAXNCH);
  }
  for (i = spp; i < 2*(1+spp); i++)
    nodep[i] = NULL;
  consensus(pattern_array, trees_in);
  printf("\n");
  if (trout) {
    treeout(root);
    if (progress)
      printf("Consensus tree written to file \"%s\"\n\n", outtreename);
  }
  if (progress)
    printf("Output written to file \"%s\"\n\n", outfilename);
  for (i = 0; i < spp; i++)
    free(nodep[i]);
  for (i = spp; i < 2*(1 + spp); i++) {
    if (nodep[i] != NULL) {
      p = nodep[i]->next;
      do {
        q = p->next;
        free(p);
        p = q;
      } while (p != nodep[i]);
      free(p);
    }
  }
  free(nodep);
  FClose(outtree);
  FClose(intree);
  FClose(outfile);

#ifdef MAC
  fixmacfile(outfilename);
  fixmacfile(outtreename);
#endif
printf("Done.\n\n");

#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif

return 0;
}  /* main */

