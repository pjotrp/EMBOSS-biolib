#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1993-2009 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define maxtrees        100   /* maximum number of trees to be printed out   */
#define often           1000  /* how often to notify how many trees examined */
#define many            10000 /* how many multiples of howoften before stop  */

typedef node **pointptr;
typedef long *treenumbers;
typedef double *valptr;
typedef long *placeptr;

AjPSeqset* seqsets = NULL;
AjPPhyloProp phyloweights = NULL;

ajint numseqs;
ajint numwts;

#ifndef OLDC
/* function prototypes */
void   emboss_getoptions(char *pgm, int argc, char *argv[]);
//void   getoptions(void);
void   allocrest(void);
void   doinit(void);
void   makeweights(void);
void   doinput(void);
void   supplement(node *);
void   evaluate(node *);
void   addtraverse(node *, node *, node *, long *, long *, valptr, placeptr);
void   addit(long );
void   dnapenny_reroot(node *);
void   describe(void);
void   maketree(void);
void reallocchars(void);
/* function prototypes */
#endif


extern sequence y;

Char infilename[FNMLNGTH], weightfilename[FNMLNGTH];

const char* outfilename;
const char* outtreename;
AjPFile embossoutfile;
AjPFile embossouttree;
node *root, *p;
long *zeros=NULL;
long chars, howmany, howoften, col, msets, ith;
boolean weights, thresh, simple, trout, progress, stepbox, ancseq,
               mulsets, firstset, justwts;
double threshold;
steptr oldweight;
pointptr treenode;   /* pointers to all nodes in tree */
double fracdone, fracinc;
boolean *added;
gbases *garbage;
node **grbg;
Char basechar[]="ACMGRSVTWYHKDBNO???????????????";

/* Variables for maketree, propagated globally for C version: */
long examined, mults;
boolean firsttime, done, recompute;
double like, bestyet;
treenumbers *bestorders, *bestrees;
treenumbers current, order;
long *threshwt;
baseptr nothing;
node *temp, *temp1;
long suppno[] =
  { 0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,4};

long suppset[] =          /* this was previously a function. */
{                                  /* in C, it doesn't need to be.    */
   1 << ((long)A),
   1 << ((long)C),
   1 << ((long)G),
   1 << ((long)T),
   1 << ((long)O),
   (1 << ((long)A)) | (1 << ((long)C)),
   (1 << ((long)A)) | (1 << ((long)G)),
   (1 << ((long)A)) | (1 << ((long)T)),
   (1 << ((long)A)) | (1 << ((long)O)),
   (1 << ((long)C)) | (1 << ((long)G)),
   (1 << ((long)C)) | (1 << ((long)T)),
   (1 << ((long)C)) | (1 << ((long)O)),
   (1 << ((long)G)) | (1 << ((long)T)),
   (1 << ((long)G)) | (1 << ((long)O)),
   (1 << ((long)T)) | (1 << ((long)O)),
   (1 << ((long)A)) | (1 << ((long)C)) | (1 << ((long)G)),
   (1 << ((long)A)) | (1 << ((long)C)) | (1 << ((long)T)),
   (1 << ((long)A)) | (1 << ((long)C)) | (1 << ((long)O)),
   (1 << ((long)A)) | (1 << ((long)G)) | (1 << ((long)T)),
   (1 << ((long)A)) | (1 << ((long)G)) | (1 << ((long)O)),
   (1 << ((long)A)) | (1 << ((long)T)) | (1 << ((long)O)),
   (1 << ((long)C)) | (1 << ((long)G)) | (1 << ((long)T)),
   (1 << ((long)C)) | (1 << ((long)G)) | (1 << ((long)O)),
   (1 << ((long)C)) | (1 << ((long)T)) | (1 << ((long)O)),
   (1 << ((long)G)) | (1 << ((long)T)) | (1 << ((long)O)),
   (1 << ((long)A))|(1 << ((long)C))|(1 << ((long)G))|(1 << ((long)T)),
   (1 << ((long)A))|(1 << ((long)C))|(1 << ((long)G))|(1 << ((long)O)),
   (1 << ((long)A))|(1 << ((long)C))|(1 << ((long)T))|(1 << ((long)O)),
   (1 << ((long)A))|(1 << ((long)G))|(1 << ((long)T))|(1 << ((long)O)),
   (1 << ((long)C))|(1 << ((long)G))|(1 << ((long)T)) | (1 << ((long)O)),
   (1 << ((long)A))|(1 << ((long)C))|(1 << ((long)G)) | (1 << ((long)T)) | (1 << ((long)O))};

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
   
  howoften = often;
  howmany = many;
  outgrno = 1;
  outgropt = false;
  simple = true;
  thresh = false;
  threshold = 0.0;
  trout = true;
  weights = false;
  justwts = false;
  printdata = false;
  progress = true;
  treeprint = true;
  stepbox = false;
  ancseq = false;
  mulsets = false;
  msets = 1;

  embInitPV(pgm, argc, argv, "PHYLIPNEW",VERSION);

    seqsets = ajAcdGetSeqsetall("sequence");

    howmany = ajAcdGetInt("howmany");
    howoften = ajAcdGetInt("howoften");
    simple = ajAcdGetBoolean("simple");  
    outgrno = ajAcdGetInt("outgrno");
    if(outgrno != 0) outgropt = true;
    else outgrno = 1;

    thresh = ajAcdGetToggle("thresh");
    if(thresh) ajAcdGetFloat("threshold");


    numwts = 0;

    phyloweights = ajAcdGetProperties("weights");
    if (phyloweights)
    {
      weights = true;
      numwts = ajPhyloPropGetSize(phyloweights);
    }

    if (numseqs > 1) {
	mulsets = true;
        msets = numseqs;
    }
    else if (numwts > 1) {
      mulsets = true;
      msets = numwts;
      justwts = true;
    }

    progress = ajAcdGetBoolean("progress");
    printdata = ajAcdGetBoolean("printdata");
    treeprint = ajAcdGetBoolean("treeprint");
    trout = ajAcdGetToggle("trout");
    stepbox = ajAcdGetBoolean("stepbox");
    ancseq = ajAcdGetBoolean("ancseq");
   

     embossoutfile = ajAcdGetOutfile("outfile");   
     emboss_openfile(embossoutfile, &outfile, &outfilename);
     
     if(trout) {
       embossouttree = ajAcdGetOutfile("outtreefile");
       emboss_openfile(embossouttree, &outtree, &outtreename);
     }

    fprintf(outfile, "\nPenny algorithm for DNA, version %s\n",VERSION);
    fprintf(outfile, " branch-and-bound to find all");
    fprintf(outfile, " most parsimonious trees\n\n");


    printf("justweights: %s\n", (justwts ?  "true" : "false"));
    printf("numwts: %d\n", numwts); 
} /* emboss_getoptions */

void allocrest()
{
  long i;

  y = (Char **)Malloc(spp*sizeof(Char *));
  for (i = 0; i < spp; i++)
    y[i] = (Char *)Malloc(chars*sizeof(Char));
  weight = (long *)Malloc(chars*sizeof(long));
  oldweight = (long *)Malloc(chars*sizeof(long));
  alias = (steptr)Malloc(chars*sizeof(long));
  ally = (steptr)Malloc(chars*sizeof(long));
  location = (steptr)Malloc(chars*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  bestorders = (treenumbers *)Malloc(maxtrees*sizeof(treenumbers));
  for (i = 1; i <= maxtrees; i++)
    bestorders[i - 1] = (treenumbers)Malloc(spp*sizeof(long));
  bestrees = (treenumbers *)Malloc(maxtrees*sizeof(treenumbers));
  for (i = 1; i <= maxtrees; i++)
    bestrees[i - 1] = (treenumbers)Malloc(spp*sizeof(long));
  current = (treenumbers)Malloc(spp*sizeof(long));
  order = (treenumbers)Malloc(spp*sizeof(long));
  added = (boolean *)Malloc(nonodes*sizeof(boolean));
}  /* allocrest */

void reallocchars(void) 
{/* The amount of chars can change between runs 
    this function reallocates all the variables 
    whose size depends on the amount of chars */
  long i;

  for (i = 0; i < spp; i++) {
    free(y[i]);
    y[i] = (Char *)Malloc(chars*sizeof(Char));
  }
  
  free(weight);
  free(oldweight);
  free(alias);
  free(ally);
  free(location);
  
  weight = (long *)Malloc(chars*sizeof(long));
  oldweight = (long *)Malloc(chars*sizeof(long));
  alias = (steptr)Malloc(chars*sizeof(long));
  ally = (steptr)Malloc(chars*sizeof(long));
  location = (steptr)Malloc(chars*sizeof(long));
} /* reallocchars */

void doinit()
{
  /* initializes variables */
  inputnumbersseq(seqsets[0], &spp, &chars, &nonodes, 1);
  if(!threshold) threshold = spp;
  if (printdata)
    fprintf(outfile, "%2ld species, %3ld  sites\n", spp, chars);
  alloctree(&treenode, nonodes, false);
  allocrest();
}  /* doinit */

void makeweights()
{
  /* make up weights vector to avoid duplicate computations */
  long i;

  for (i = 1; i <= chars; i++) {
    alias[i - 1] = i;
    oldweight[i - 1] = weight[i - 1];
    ally[i - 1] = i;
  }
  sitesort(chars, weight);
  sitecombine(chars);
  sitescrunch(chars);
  endsite = 0;
  for (i = 1; i <= chars; i++) {
    if (ally[i - 1] == i)
      endsite++;
  }
  for (i = 1; i <= endsite; i++)
    location[alias[i - 1] - 1] = i;
  if (!thresh)
    threshold = spp;
  threshwt = (long *)Malloc(endsite*sizeof(long));
  for (i = 0; i < endsite; i++) {
    weight[i] *= 10;
    threshwt[i] = (long)(threshold * weight[i] + 0.5);
  }
  if ( zeros  != NULL )
    free(zeros);
  zeros = (long *)Malloc(endsite*sizeof(long)); /*in makeweights()*/
  for (i = 0; i < endsite; i++)
    zeros[i] = 0;
}  /* makeweights */


void doinput()
{
  /* reads the input data */
  long i;

  if (justwts) {
    if (firstset)
      seq_inputdata(seqsets[ith-1], chars);
    for (i = 0; i < chars; i++)
      weight[i] = 1;
    inputweightsstr(phyloweights->Str[ith-1], chars, weight, &weights);
    if (justwts) {
      fprintf(outfile, "\n\nWeights set # %ld:\n\n", ith);
      if (progress)
        printf("\nWeights set # %ld:\n\n", ith);
    }
    if (printdata)
      printweights(outfile, 0, chars, weight, "Sites");
  } else {
    if (!firstset){
      samenumspseq(seqsets[ith-1],&chars, ith);
      reallocchars();
    }
    seq_inputdata(seqsets[ith-1], chars);
    for (i = 0; i < chars; i++)
      weight[i] = 1;
    if (weights) {
      inputweightsstr(phyloweights->Str[0], chars, weight, &weights);
      if (printdata)
        printweights(outfile, 0, chars, weight, "Sites");
    }
  }

  makeweights();
  makevalues(treenode, zeros, false);
  alloctemp(&temp, zeros, endsite);
  alloctemp(&temp1, zeros, endsite);
}  /* doinput */


void supplement(node *r)
{
  /* determine minimum number of steps more which will
     be added when rest of species are put in tree */
  long i, j, k, has, sum;
  boolean addedmayhave, nonaddedhave;

  for (i = 0; i < endsite; i++) {
    nonaddedhave = 0;;
    addedmayhave = 0;
    for (k = 0; k < spp; k++) {
      has = treenode[k]->base[i];
      if (has != 31) {
        if (added[k])
          addedmayhave |= has;
        else {
          if ((has == 1) || (has == 2) || (has == 4)
              || (has == 8) || (has == 16))
            nonaddedhave |= has;
        }
      }
    }
    sum = 0;
    j = 1;
    for (k = 1; k <= 5; k++) {
      if ((j & nonaddedhave) != 0)
        if ((j & addedmayhave) == 0)
          sum++;
      j += j;
    }
    r->numsteps[i] += sum * weight[i];
  }
}  /* supplement */


void evaluate(node *r)
{
  /* determines the number of steps needed for a tree. this is
     the minimum number of steps needed to evolve sequences on
     this tree */
  long i, steps;
  double sum;

  sum = 0.0;
  supplement(r);
  for (i = 0; i < endsite; i++) {
    steps = r->numsteps[i];
    if ((long)steps <= threshwt[i])
      sum += steps;
    else
      sum += threshwt[i];
  }
  if (examined == 0 && mults == 0)
    bestyet = -1.0;
  like = sum;
}  /* evaluate */


void addtraverse(node *p, node *item, node *fork, long *m, long *n,
                        valptr valyew, placeptr place)
{
  /* traverse all places to add item */
  if (done)
    return;
  if (*m <= 2 || (p != root && p != root->next->back)) {
    if (p == root)
      fillin(temp, item, p);
    else {
      fillin(temp1, item, p);
      fillin(temp, temp1, p->back);
    }
    (*n)++;
    evaluate(temp);
    examined++;
    if (examined == howoften) {
      examined = 0;
      mults++;
      if (mults == howmany)
        done = true;
      if (progress) {
        printf("%7ld", mults);
        if (bestyet >= 0)
          printf("%16.1f", bestyet / 10.0);
        else
          printf("            -   ");
        printf("%17ld%20.2f\n", nextree - 1, fracdone * 100);
#ifdef WIN32
        phyFillScreenColor();
#endif
      }
    }
    valyew[(*n) - 1] = like;
    place[(*n) - 1] = p->index;
  }
  if (!p->tip) {
    addtraverse(p->next->back, item, fork, m,n,valyew,place);
    addtraverse(p->next->next->back, item, fork,m,n,valyew,place);
  }
}  /* addtraverse */


void addit(long m)
{
  /* adds the species one by one, recursively */
  long n;
  valptr valyew;
  placeptr place;
  long i, j, n1, besttoadd=0;
  valptr bestval;
  placeptr bestplace;
  double oldfrac, oldfdone, sum, bestsum;

  valyew = (valptr)Malloc(nonodes*sizeof(double));
  bestval = (valptr)Malloc(nonodes*sizeof(double));
  place = (placeptr)Malloc(nonodes*sizeof(long));
  bestplace = (placeptr)Malloc(nonodes*sizeof(long));
  if (simple && !firsttime) {
    n = 0;
    added[order[m - 1] - 1] = true;
    addtraverse(root, treenode[order[m - 1] - 1],
                treenode[spp + m - 2], &m,&n,valyew,place);
    besttoadd = order[m - 1];
    memcpy(bestplace, place, nonodes*sizeof(long));
    memcpy(bestval, valyew, nonodes*sizeof(double));
  } else {
    bestsum = -1.0;
    for (i = 1; i <= spp; i++) {
      if (!added[i - 1]) {
        n = 0;
        added[i - 1] = true;
        addtraverse(root, treenode[i - 1], treenode[spp + m - 2],
                    &m,&n,valyew,place);
        added[i - 1] = false;
        sum = 0.0;
        for (j = 0; j < n; j++)
          sum += valyew[j];
        if (sum > bestsum) {
          bestsum = sum;
          besttoadd = i;
          memcpy(bestplace, place, nonodes*sizeof(long));
          memcpy(bestval, valyew, nonodes*sizeof(double));
        }
      }
    }
  }
  order[m - 1] = besttoadd;
  memcpy(place, bestplace, nonodes*sizeof(long));
  memcpy(valyew, bestval, nonodes*sizeof(double));
  shellsort(valyew, place, n);
  oldfrac = fracinc;
  oldfdone = fracdone;
  n1 = 0;
  for (i = 0; i < n; i++) {
    if (valyew[i] <= bestyet || bestyet < 0.0)
      n1++;
  }
  if (n1 > 0)
    fracinc /= n1;
  for (i = 0; i < n; i++) {
    if (valyew[i] <= bestyet || bestyet < 0.0) {
      current[m - 1] = place[i];
      recompute = (m < spp);
      add(treenode[place[i] - 1], treenode[besttoadd - 1],
          treenode[spp + m - 2], &root, recompute, treenode, grbg, zeros);
      added[besttoadd - 1] = true;
      if (m < spp)
        addit(m + 1);
      else {
        if (valyew[i] < bestyet || bestyet < 0.0) {
          nextree = 1;
          bestyet = valyew[i];
        }
        if (nextree <= maxtrees) {
          memcpy(bestorders[nextree - 1], order,
                 spp*sizeof(long));
          memcpy(bestrees[nextree - 1], current,
                 spp*sizeof(long));
        }
        nextree++;
        firsttime = false;
      }
      recompute = (m < spp);
      re_move(treenode[besttoadd - 1], &treenode[spp + m - 2], &root,
                recompute, treenode, grbg, zeros);
      added[besttoadd - 1] = false;
    }
    fracdone += fracinc;
  }
  fracinc = oldfrac;
  fracdone = oldfdone;
  free(valyew);
  free(bestval);
  free(place);
  free(bestplace);
}  /* addit */


void dnapenny_reroot(node *outgroup)
{
  /* reorients tree, putting outgroup in desired position. */
  node *p, *q, *newbottom, *oldbottom;

  if (outgroup->back->index == root->index)
    return;
  newbottom = outgroup->back;
  p = treenode[newbottom->index - 1]->back;
  while (p->index != root->index) {
    oldbottom = treenode[p->index - 1];
    treenode[p->index - 1] = p;
    p = oldbottom->back;
  }
  p = root->next;
  q = root->next->next;
  p->back->back = q->back;
  q->back->back = p->back;
  p->back = outgroup;
  q->back = outgroup->back;
  outgroup->back->back = root->next->next;
  outgroup->back = root->next;
  treenode[newbottom->index - 1] = newbottom;
}  /* dnapenny_reroot */


void describe()
{
  /* prints ancestors, steps and table of numbers of steps in
     each site */

  if (stepbox)
    writesteps(chars, weights, oldweight, root);
  if (ancseq) {
    hypstates(chars, root, treenode, &garbage, basechar);
    putc('\n', outfile);
  }
  putc('\n', outfile);
  if (trout) {
    col = 0;
    treeout(root, nextree, &col, root);
  }
}  /* describe */


void maketree()
{
  /* tree construction recursively by branch and bound */
  long i, j, k;
  node *dummy;

  if (progress) {
    printf("\nHow many\n");
    printf("trees looked                                       Approximate\n");
    printf("at so far      Length of        How many           percentage\n");
    printf("(multiples     shortest tree    trees this short   searched\n");
    printf("of %4ld):      found so far     found so far       so far\n",
           howoften);
    printf("----------     ------------     ------------       ------------\n");
  }
#ifdef WIN32
  phyFillScreenColor();
#endif
  done = false;
  mults = 0;
  examined = 0;
  nextree = 1;
  root = treenode[0];
  firsttime = true;
  for (i = 0; i < spp; i++)
    added[i] = false;
  added[0] = true;
  order[0] = 1;
  k = 2;
  fracdone = 0.0;
  fracinc = 1.0;
  bestyet = -1.0;
  recompute = true;
  addit(k);
  if (done) {
    if (progress) {
      printf("Search broken off!  Not guaranteed to\n");
      printf(" have found the most parsimonious trees.\n");
    }
    if (treeprint) {
      fprintf(outfile, "Search broken off!  Not guaranteed to\n");
      fprintf(outfile, " have found the most parsimonious\n");
      fprintf(outfile, " trees, but here is what we found:\n");
    }
  }
  if (treeprint) {
    fprintf(outfile, "\nrequires a total of %18.3f\n\n", bestyet / 10.0);
    if (nextree == 2)
      fprintf(outfile, "One most parsimonious tree found:\n");
    else
      fprintf(outfile, "%6ld trees in all found\n", nextree - 1);
  }
  if (nextree > maxtrees + 1) {
    if (treeprint)
      fprintf(outfile, "here are the first%4ld of them\n", (long)maxtrees);
    nextree = maxtrees + 1;
  }
  if (treeprint)
    putc('\n', outfile);
  for (i = 0; i < spp; i++)
    added[i] = true;
  for (i = 0; i <= nextree - 2; i++) {
    root = treenode[0];
    for (j = k; j <= spp; j++)
      add(treenode[bestrees[i][j - 1] - 1],
          treenode[bestorders[i][j - 1] - 1], treenode[spp + j - 2],
          &root, recompute, treenode, grbg, zeros);
    dnapenny_reroot(treenode[outgrno - 1]);
    postorder(root);
    evaluate(root);
    printree(root, 1.0);
    describe();
    for (j = k - 1; j < spp; j++)
      re_move(treenode[bestorders[i][j] - 1], &dummy, &root,
                recompute, treenode, grbg, zeros);
  }
  if (progress) {
    printf("\nOutput written to file \"%s\"\n\n", outfilename);
    if (trout)
      printf("Trees also written onto file \"%s\"\n\n", outtreename);
  }
  freetemp(&temp);
  freetemp(&temp1);
  if (ancseq)
    freegarbage(&garbage);
}  /* maketree */


int main(int argc, Char *argv[])
{  /* Penny's branch-and-bound method for DNA sequences */
#ifdef MAC
   argc = 1;                /* macsetup("Dnapenny","");        */
   argv[0] = "Dnapenny";
#endif
  init(argc, argv);
  
  emboss_getoptions("fdnapenny", argc, argv);


  /* Reads in the number of species, number of characters,
     options and data.  Then finds all most parsimonious trees */

  ibmpc = IBMCRT;
  ansi = ANSICRT;
  mulsets = false;
  garbage = NULL;
  msets = 1;
  firstset = true;
  doinit();


  for (ith = 1; ith <= msets; ith++) {
    doinput();
    if (ith == 1)
      firstset = false;
    if (msets > 1 && !justwts) {
      fprintf(outfile, "\nData set # %ld:\n",ith);
      if (progress)
        printf("\nData set # %ld:\n",ith);
    }
    maketree();
    free(threshwt);
    freenodes(nonodes,treenode);
  }
  FClose(infile);
  FClose(outfile);
  FClose(outtree);
#ifdef MAC
  fixmacfile(outfilename);
  fixmacfile(outtreename);
#endif
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
   embExit();
  return 0;
}  /* Penny's branch-and-bound method for DNA sequences */
