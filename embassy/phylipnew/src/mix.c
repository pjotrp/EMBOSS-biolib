
#include "phylip.h"
#include "disc.h"
#include "wagner.h"

/* version 3.6. (c) Copyright 1993-2002 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define maxtrees        100  /* maximum number of tied trees stored     */


typedef long *placeptr;

AjPPhyloState* phylostates = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloProp phyloanc = NULL;
AjPPhyloProp phylomix = NULL;
AjPPhyloTree* phylotrees = NULL;

#ifndef OLDC
/* function prototypes */
//void   getoptions(void);
void emboss_getoptions(char *pgm, int argc, char *argv[]);
void   allocrest(void);
void   doinit(void);
void   inputoptions(void);
void   doinput(void);
void   evaluate(node2 *);
void   reroot(node2 *);
void   savetraverse(node2 *);
void   savetree(void);
void   mix_addtree(long *pos);

void   mix_findtree(boolean *, long *, long, long *, long **);
void   tryadd(node2 *, node2 **, node2 **);
void   addpreorder(node2 *, node2 *, node2 *);
void   tryrearr(node2 *, node2 **, boolean *);
void   repreorder(node2 *, node2 **, boolean *);
void   rearrange(node2 **r);
void   mix_addelement(node2 **, long *, long *, boolean *, char**);
void   mix_treeread(void);
void   describe(void);
void   maketree(void);
void   reallocchars(void);
/* function prototypes */
#endif



Char infilename[FNMLNGTH], intreename[FNMLNGTH], weightfilename[FNMLNGTH], ancfilename[FNMLNGTH], mixfilename[FNMLNGTH];
const char* outfilename;
const char* outtreename;
AjPFile embossoutfile;
AjPFile embossouttree;

node2 *root;
long outgrno, msets, ith, njumble, jumb, numtrees;
/*  outgrno indicates outgroup */
long inseed, inseed0;
boolean jumble, usertree, weights, ancvar, questions, allsokal,
               allwagner, mixture, trout, noroot, outgropt, didreroot,
               progress, treeprint, stepbox, ancseq, mulsets, firstset,
               justwts;
boolean *ancone, *anczero, *ancone0, *anczero0;
pointptr2 treenode;   /* pointers to all nodes in tree */
double threshold;
double *threshwt;
bitptr wagner, wagner0;
longer seed;
long *enterorder;
double **fsteps;
char *guess;
long **bestrees;
steptr numsteps, numsone, numszero;
gbit *garbage;
char ch;
char *progname;

/* Local variables for maketree: */
long minwhich;
double like, bestyet, bestlike, bstlike2, minsteps;
boolean lastrearr,full;
double nsteps[maxuser];
node2 *there;
long fullset;
bitptr steps, zeroanc, oneanc, fulzeroanc, empzeroanc;
long *place, col;

void emboss_getoptions(char *pgm, int argc, char *argv[])
{

  AjPStr method = NULL;
  ajint numseqs=0;
  ajint numwts=0;

  jumble = false;
  njumble = 1;
  outgrno = 1;
  outgropt = false;
  trout = true;
  usertree = false;
  weights = false;
  justwts = false;
  ancvar = false;
  allsokal = false;
  allwagner = false;
  mixture = false;
  printdata = false;
  progress = true;
  treeprint = true;
  stepbox = false;
  ancseq = false;
  mulsets = false;
  msets = 1;

    embInitP (pgm, argc, argv, "PHYLIPNEW");

    phylostates = ajAcdGetDiscretestates("infile");

    while (phylostates[numseqs])
	numseqs++;

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
    phyloanc = ajAcdGetProperties("ancfile");
    if(phyloanc) ancvar = true;


    phylotrees = ajAcdGetTree("intreefile");
    if (phylotrees)
    {
        while (phylotrees[numtrees])
            numtrees++;
        usertree = true;
    }

    method = ajAcdGetListSingle("method");

    if(ajStrMatchC(method, "w")) allwagner = true;
    else if(ajStrMatchC(method, "c")) allsokal = true;
    else if(ajStrMatchC(method, "m")) {
      mixture = allwagner = true; 
      phylomix = ajAcdGetProperties("mixfile");
    }

    if(!usertree) {  
      njumble = ajAcdGetInt("njumble");
      if(njumble >0) {
        inseed = ajAcdGetInt("seed");
        jumble = true; 
        emboss_initseed(inseed, &inseed0, seed);
      }
      else njumble = 1;
    }

    outgrno = ajAcdGetInt("outgrno");
    if(outgrno != 0) outgropt = true;
    else outgrno = 1;

    threshold = ajAcdGetFloat("threshold");

    printdata = ajAcdGetBool("printdata");
    progress = ajAcdGetBool("progress");
    treeprint = ajAcdGetBool("treeprint");
    trout = ajAcdGetToggle("trout");
    stepbox = ajAcdGetBool("stepbox");
    ancseq = ajAcdGetBool("ancseq");

     embossoutfile = ajAcdGetOutfile("outfile");   
     emboss_openfile(embossoutfile, &outfile, &outfilename);
     
     
     if(trout) {
       embossouttree = ajAcdGetOutfile("outtreefile");
       emboss_openfile(embossouttree, &outtree, &outtreename);
     }
 

  fprintf(outfile, "\nMixed parsimony algorithm, version %s\n\n",VERSION);
}  /* emboss_getoptions */

void reallocchars() 
{
  long i;

  if (usertree) {
    for (i = 0; i < maxuser; i++) {
      free (fsteps[i]);
      fsteps[i] = (double *)Malloc(chars*sizeof(double));
    }
  }
  free(extras);
  free(weight);
  free(threshwt);
  free(numsteps);
  free(numszero);
  free(numsone);
  free(guess);
  free(ancone);
  free(anczero);
  free(ancone0);
  free(anczero0);

  extras = (steptr)Malloc(chars*sizeof(long));
  weight = (steptr)Malloc(chars*sizeof(long));
  threshwt = (double *)Malloc(chars*sizeof(double));
  numsteps = (steptr)Malloc(chars*sizeof(long));
  numszero = (steptr)Malloc(chars*sizeof(long));
  numsone = (steptr)Malloc(chars*sizeof(long));
  guess = (Char *)Malloc(chars*sizeof(Char));
  ancone = (boolean *)Malloc(chars*sizeof(boolean));
  anczero = (boolean *)Malloc(chars*sizeof(boolean));
  ancone0 = (boolean *)Malloc(chars*sizeof(boolean));
  anczero0 = (boolean *)Malloc(chars*sizeof(boolean));
}

void allocrest()
{
  long i;

  if (usertree) {
    fsteps = (double **)Malloc(maxuser*sizeof(double *));
    for (i = 0; i < maxuser; i++)
      fsteps[i] = (double *)Malloc(chars*sizeof(double));
  }
  bestrees = (long **)Malloc(maxtrees*sizeof(long *));
  for (i = 1; i <= maxtrees; i++)
    bestrees[i - 1] = (long *)Malloc((spp+1)*sizeof(long));
  extras = (steptr)Malloc(chars*sizeof(long));
  weight = (steptr)Malloc(chars*sizeof(long));
  threshwt = (double *)Malloc(chars*sizeof(double));
  numsteps = (steptr)Malloc(chars*sizeof(long));
  numszero = (steptr)Malloc(chars*sizeof(long));
  numsone = (steptr)Malloc(chars*sizeof(long));
  guess = (Char *)Malloc(chars*sizeof(Char));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  enterorder = (long *)Malloc(spp*sizeof(long));
  ancone = (boolean *)Malloc(chars*sizeof(boolean));
  anczero = (boolean *)Malloc(chars*sizeof(boolean));
  ancone0 = (boolean *)Malloc(chars*sizeof(boolean));
  anczero0 = (boolean *)Malloc(chars*sizeof(boolean));
  wagner = (bitptr)Malloc(words*sizeof(long));
  wagner0 = (bitptr)Malloc(words*sizeof(long));
  place = (long *)Malloc(nonodes*sizeof(long));
  steps = (bitptr)Malloc(words*sizeof(long));
  zeroanc = (bitptr)Malloc(words*sizeof(long));
  oneanc = (bitptr)Malloc(words*sizeof(long));
  fulzeroanc = (bitptr)Malloc(words*sizeof(long));
  empzeroanc = (bitptr)Malloc(words*sizeof(long));
}  /* allocrest */


void doinit()
{
  /* initializes variables */

  inputnumbersstate(phylostates[0], &spp, &chars, &nonodes, 1);
  words = chars / bits + 1;
  if (printdata)
      fprintf(outfile, "%ld species, %ld characters\n\n", spp, chars);
  alloctree2(&treenode);
  setuptree2(treenode);
  allocrest();
}  /* doinit */


void inputoptions()
{
  /* input the information on the options */
  long i;

  if(justwts){
    if(firstset){
      if (ancvar)
        inputancestorsstr(phyloanc->Str[0], anczero0, ancone0);
      if (mixture)
        inputmixturestr(phylomix->Str[0], wagner0);
    }
    for (i = 0; i < (chars); i++)
      weight[i] = 1;
    inputweightsstr(phyloweights->Str[0], chars, weight, &weights);
    for (i = 0; i < (words); i++) {
      if (mixture)
        wagner[i] = wagner0[i];
      else if (allsokal)
        wagner[i] = 0;
      else
        wagner[i] = (1L << (bits + 1)) - (1L << 1);
    }
  }
  else {
    if (!firstset) {
      samenumspstate(phylostates[ith-1], &chars, ith);
      reallocchars();
    }
    for (i = 0; i < (chars); i++)
      weight[i] = 1;
    if (ancvar)
      inputancestorsstr(phyloanc->Str[0], anczero0, ancone0);
    if (mixture)
      inputmixturestr(phylomix->Str[0], wagner0);
    if (weights)
      inputweightsstr(phyloweights->Str[0], chars, weight, &weights);
    for (i = 0; i < (words); i++) {
      if (mixture)
        wagner[i] = wagner0[i];
      else if (allsokal)
        wagner[i] = 0;
      else
        wagner[i] = (1L << (bits + 1)) - (1L << 1);
    }
  }
  for (i = 0; i < (chars); i++) {
    if (!ancvar) {
      anczero[i] = true;
      ancone[i] = (((1L << (i % bits + 1)) & wagner[i / bits]) != 0);
    } else {
      anczero[i] = anczero0[i];
      ancone[i] = ancone0[i];
    }
  }
  noroot = true;
  questions = false;
  for (i = 0; i < (chars); i++) {
    if (weight[i] > 0) {
      noroot = (noroot && ancone[i] && anczero[i] &&
                ((((1L << (i % bits + 1)) & wagner[i / bits]) != 0)
                 || threshold <= 2.0));
    }
    questions = (questions || (ancone[i] && anczero[i]));
    threshwt[i] = threshold * weight[i];
  }
}  /* inputoptions */


void doinput()
{
  /* reads the input data */
  inputoptions();
  if(!justwts || firstset)
      disc_inputdata2(phylostates[ith-1], treenode);
}  /* doinput */


void evaluate(node2 *r)
{
  /* Determines the number of steps needed for a tree.
    This is the minimum number needed to evolve chars on
    this tree */
  long i, stepnum, smaller;
  double sum, term;

  sum = 0.0;
  for (i = 0; i < (chars); i++) {
    numszero[i] = 0;
    numsone[i] = 0;
  }
  full = true;
  for (i = 0; i < (words); i++)
    zeroanc[i] = fullset;
  postorder(r, fullset, full, wagner, zeroanc);
  cpostorder(r, full, zeroanc, numszero, numsone);
  count(r->fulstte1, zeroanc, numszero, numsone);
  for (i = 0; i < (words); i++)
    zeroanc[i] = 0;
  full = false;
  postorder(r, fullset, full, wagner, zeroanc);
  cpostorder(r, full, zeroanc, numszero, numsone);
  count(r->empstte0, zeroanc, numszero, numsone);
  for (i = 0; i < (chars); i++) {
    smaller = spp * weight[i];
    numsteps[i] = smaller;
    if (anczero[i]) {
      numsteps[i] = numszero[i];
      smaller = numszero[i];
    }
    if (ancone[i] && numsone[i] < smaller)
      numsteps[i] = numsone[i];
    stepnum = numsteps[i] + extras[i];
    if (stepnum <= threshwt[i])
      term = stepnum;
    else
      term = threshwt[i];
    sum += term;
    if (usertree && which <= maxuser)
      fsteps[which - 1][i] = term;
    guess[i] = '?';
    if (!ancone[i] || (anczero[i] && numszero[i] < numsone[i]))
      guess[i] = '0';
    else if (!anczero[i] || (ancone[i] && numsone[i] < numszero[i]))
      guess[i] = '1';
  }
  if (usertree && which <= maxuser) {
    nsteps[which - 1] = sum;
    if (which == 1) {
      minwhich = 1;
      minsteps = sum;
    } else if (sum < minsteps) {
      minwhich = which;
      minsteps = sum;
    }
  }
  like = -sum;
}  /* evaluate */


void reroot(node2 *outgroup)
{
  /* reorients tree, putting outgroup in desired position. */
  node2 *p, *q;

  if (outgroup->back->index == root->index)
    return;
  p = root->next;
  q = root->next->next;
  p->back->back = q->back;
  q->back->back = p->back;
  p->back = outgroup;
  q->back = outgroup->back;
  outgroup->back->back = q;
  outgroup->back = p;
}  /* reroot */


void savetraverse(node2 *p)
{
  /* sets BOOLEANs that indicate which way is down */
  p->bottom = true;
  if (p->tip)
    return;
  p->next->bottom = false;
  savetraverse(p->next->back);
  p->next->next->bottom = false;
  savetraverse(p->next->next->back);
}  /* savetraverse */


void savetree()
{
  /* record in place where each species has to be
    added to reconstruct this tree */
  long i, j;
  node2 *p;
  boolean done;

  if (noroot)
    reroot(treenode[outgrno - 1]);
  savetraverse(root);
  for (i = 0; i < (nonodes); i++)
   place[i] = 0;
  place[root->index - 1] = 1;
  for (i = 1; i <= (spp); i++) {
    p = treenode[i - 1];
    while (place[p->index - 1] == 0) {
      place[p->index - 1] = i;
      while (!p->bottom)
        p = p->next;
      p = p->back;
    }
    if (i > 1) {
      place[i - 1] = place[p->index - 1];
      j = place[p->index - 1];
      done = false;
      while (!done) {
        place[p->index - 1] = spp + i - 1;
        while (!p->bottom)
          p = p->next;
        p = p->back;
        done = (p == NULL);
        if (!done)
          done = (place[p->index - 1] != j);
      }
    }
  }
}  /* savetree */


void mix_addtree(long *pos)
{
  /* puts tree from ARRAY place in its proper position
    in ARRAY bestrees */
  long i;
  for (i =nextree - 1; i >= (*pos); i--)
    memcpy(bestrees[i], bestrees[i - 1], spp*sizeof(long));
  for (i = 0; i < (spp); i++)
    bestrees[(*pos) - 1][i] = place[i];
  nextree++;
}  /* mix_addtree */


void mix_findtree(boolean *found, long *pos, long nextree,
                        long *place, long **bestrees)
{
  /* finds tree given by ARRAY place in ARRAY bestrees by binary search */
  /* used by dnacomp, dnapars, dollop, mix, & protpars */
  long i, lower, upper;
  boolean below, done;

  below = false;
  lower = 1;
  upper = nextree - 1;
  (*found) = false;
  while (!(*found) && lower <= upper) {
    (*pos) = (lower + upper) / 2;
    i = 3;
    done = false;
    while (!done) {
      done = (i > spp);
      if (!done)
        done = (place[i - 1] != bestrees[(*pos) - 1][i - 1]);
      if (!done)
        i++;
    }
    (*found) = (i > spp);
    below = (place[i - 1] <  bestrees[(*pos )- 1][i - 1]);
    if (*found)
      break;
    if (below)
      upper = (*pos) - 1;
    else
      lower = (*pos) + 1;
  }
  if (!(*found) && !below)
    (*pos)++;
}  /* mix_findtree */


void tryadd(node2 *p, node2 **item, node2 **nufork)
{
  /* temporarily adds one fork and one tip to the tree.
    if the location where they are added yields greater
    "likelihood" than other locations tested up to that
    time, then keeps that location as there */

  long pos;
  boolean found; 
  node2 *rute;

  add3(p, *item, *nufork, &root, treenode);
  evaluate(root);
  if (lastrearr) {
    if (like >= bstlike2) {
      rute = root->next->back;
      savetree();
      reroot(rute);
      if (like > bstlike2) {
        bestlike = bstlike2 = like;
        pos = 1;
        nextree = 1;
        mix_addtree(&pos);
      } else {
        pos = 0;
        mix_findtree(&found, &pos, nextree, place, bestrees);
        if (!found) { 
          if (nextree <= maxtrees)
            mix_addtree(&pos);
        }
      }
    }
  }
  if (like > bestyet) {
    bestyet = like;
    there = p;
  }
  re_move3(item, nufork, &root, treenode);
}  /* tryadd */


void addpreorder(node2 *p, node2 *item, node2 *nufork)
{
  /* traverses a binary tree, calling PROCEDURE tryadd
    at a node before calling tryadd at its descendants */

  if (p == NULL)
    return;
  tryadd(p, &item, &nufork);
  if (!p->tip) {
    addpreorder(p->next->back, item, nufork);
    addpreorder(p->next->next->back, item, nufork);
  }
}  /* addpreorder */


void tryrearr(node2 *p, node2 **r, boolean *success)
{
  /* evaluates one rearrangement of the tree.
    if the new tree has greater "likelihood" than the old
    one sets success := TRUE and keeps the new tree.
    otherwise, restores the old tree */
  node2 *frombelow, *whereto, *forknode;
  double oldlike;

  if (p->back == NULL)
    return;
  forknode = treenode[p->back->index - 1];
  if (forknode->back == NULL)
    return;
  oldlike = bestyet;
  if (p->back->next->next == forknode)
    frombelow = forknode->next->next->back;
  else
    frombelow = forknode->next->back;
  whereto = treenode[forknode->back->index - 1];
  re_move3(&p, &forknode, &root, treenode);
  add3(whereto, p, forknode, &root, treenode);
  evaluate(*r);
  if (like <= oldlike) {
    re_move3(&p, &forknode, &root, treenode);
    add3(frombelow, p, forknode, &root, treenode);
  } else {
    *success = true;
    bestyet = like;
  }
}  /* tryrearr */


void repreorder(node2 *p, node2 **r, boolean *success)
{
  /* traverses a binary tree, calling PROCEDURE tryrearr
    at a node before calling tryrearr at its descendants */
  if (p == NULL)
    return;
  tryrearr(p, r, success);
  if (!p->tip) {
    repreorder(p->next->back, r,success);
    repreorder(p->next->next->back, r,success);
  }
}  /* repreorder */


void rearrange(node2 **r)
{
  /* traverses the tree (preorder), finding any local
    rearrangement which decreases the number of steps.
    if traversal succeeds in increasing the tree's
    "likelihood", PROCEDURE rearrange runs traversal again */
  boolean success=true;
  while (success) {
    success = false;
    repreorder(*r,r,&success);
  }
}  /* rearrange */


void mix_addelement(node2 **p, long *nextnode, long *lparens,
                        boolean *names, char** treestr)
{
  /* recursive procedure adds nodes to user-defined tree */
  node2 *q;
  long i, n;
  boolean found;
  Char str[nmlngth];

  sgetch(&ch, lparens, treestr);
  if (ch == '(' ) {
    if ((*lparens) >= spp) {
      printf("\n\nERROR IN USER TREE: Too many left parentheses\n\n");
      embExitBad();
    }
    (*nextnode)++;
    q = treenode[(*nextnode) - 1];
    mix_addelement(&q->next->back, nextnode, lparens, names, treestr);
    q->next->back->back = q->next;
    do {
	ch = *(*treestr)++;
    } while (ch && ch != ',');
    mix_addelement(&q->next->next->back, nextnode, lparens, names, treestr);
    q->next->next->back->back = q->next->next;
    do {
	ch = *(*treestr)++;
    } while (ch && ch != ')');
    *p = q;
    return;
  }
  for (i = 0; i < nmlngth; i++)
    str[i] = ' ';
  n = 1;
  do {
    if (ch == '_')
      ch = ' ';
    str[n - 1] =ch;
    ch = *(*treestr)++ ;
    n++;
  } while (ch != ',' && ch != ')' && ch != ':' && n <= nmlngth);
  n = 1;
  do {
    found = true;
    for (i = 0; i < nmlngth; i++)
      found = (found && ((str[i] == nayme[n - 1][i]) ||
                         ((nayme[n - 1][i] == '_') && (str[i] == ' '))));
    if (found) {
      if (names[n - 1] == false) {
        *p = treenode[n - 1];
        names[n - 1] = true;
      } else {
        printf("\n\nERROR IN USER TREE: Duplicate name found: ");
        for (i = 0; i < nmlngth; i++)
          putchar(nayme[n - 1][i]);
        printf("\n\n");
        embExitBad();
      }
    } else
      n++;
  } while (!(n > spp || found ));
  if (n <= spp)
    return;
  printf("CANNOT FIND SPECIES: ");
  for (i = 0; i < nmlngth; i++)
    putchar(str[i]);
  putchar('\n');
}  /* mix_addelement */


void mix_treeread()
{
  /* read in user-defined tree and set it up */
  long nextnode, lparens, i;
  boolean *names;
  char* treestr;

  root = treenode[spp];
  nextnode = spp;
  root->back = NULL;
  names = (boolean *)Malloc(spp*sizeof(boolean));
  for (i = 0; i < (spp); i++)
    names[i] = false;
  lparens = 0;
  treestr = ajStrGetuniquePtr(&phylotrees[0]->Tree);

  mix_addelement(&root, &nextnode, &lparens, names, &treestr);
  if (ch == '[') {
    do
      ch = *treestr++;
    while (ch != ']');
    ch = *treestr++;
  }
  do {
      ch = *treestr++;
  } while (ch && ch != ';');
  if (progress)
    printf(".");
  free(names);
}  /* mix_treeread */


void describe()
{
  /* prints ancestors, steps and table of numbers of steps in
    each character */

  if (treeprint)
    fprintf(outfile, "\nrequires a total of %10.3f\n", -like);
  putc('\n', outfile);
  if (stepbox)
    writesteps(weights, numsteps);
  if (questions && (!noroot || didreroot))
    guesstates(guess);
  if (ancseq) {
    hypstates(fullset, full, noroot, didreroot, root, wagner,
                zeroanc, oneanc, treenode, guess, garbage);
    putc('\n', outfile);
  }
  putc('\n', outfile);
  if (trout) {
    col = 0;
    treeout2(root, &col, root);
  }
}  /* describe */


void maketree()
{
  /* constructs a binary tree from the pointers in treenode.
    adds each node at location which yields highest "likelihood"
    then rearranges the tree for greatest "likelihood" */
  long i, j;
  double gotlike;
  node2 *item, *nufork, *dummy;

  fullset = (1L << (bits + 1)) - (1L << 1);
  for (i=0 ; i<words ; ++i){
    fulzeroanc[i]=fullset;
    empzeroanc[i]= 0;}
  if (!usertree) {
    for (i = 1; i <= (spp); i++)
      enterorder[i - 1] = i;
    if (jumble)
      randumize(seed, enterorder);
    root = treenode[enterorder[0] - 1];
    add3(treenode[enterorder[0] - 1], treenode[enterorder[1] - 1],
        treenode[spp], &root, treenode);
    if (progress) {
      printf("Adding species:\n");
      writename(0, 2, enterorder);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
    lastrearr = false;
    for (i = 3; i <= (spp); i++) {
      bestyet = -350.0 * spp * chars;
      item = treenode[enterorder[i - 1] - 1];
      nufork = treenode[spp + i - 2];
      addpreorder(root, item, nufork);
      add3(there, item, nufork, &root, treenode);
      like = bestyet;
      rearrange(&root);
      if (progress) {
        writename(i - 1, 1, enterorder);
#ifdef WIN32
        phyFillScreenColor();
#endif
      }
      lastrearr = (i == spp);
      if (lastrearr) {
        if (progress) {
          printf("\nDoing global rearrangements\n");
          printf("  !");
          for (j = 1; j <= (nonodes); j++)
            if ( j % (( nonodes / 72 ) + 1 ) == 0 )
              putchar('-');
          printf("!\n");
#ifdef WIN32
          phyFillScreenColor();
#endif
        }
        bestlike = bestyet;
        if (jumb == 1) {
          bstlike2 = bestlike;
          nextree = 1;
        }
        do {
          if (progress)
            printf("   ");
          gotlike = bestlike;
          for (j = 0; j < (nonodes); j++) {
            bestyet = -350.0  * spp * chars;
            item = treenode[j];
            if (item != root) {
              re_move3(&item, &nufork, &root, treenode);
              there = root;
              addpreorder(root, item, nufork);
              add3(there, item, nufork, &root, treenode);
            }
            if (progress) {
              if ( j % (( nonodes / 72 ) + 1 ) == 0 )
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
        } while (bestlike > gotlike);
      }
    }
    if (progress)
      putchar('\n');
    for (i = spp - 1; i >= 1; i--)
      re_move3(&treenode[i], &dummy, &root, treenode);
    if (jumb == njumble) {
      if (treeprint) {
        putc('\n', outfile);
        if (nextree == 2)
          fprintf(outfile, "One most parsimonious tree found:\n");
        else
          fprintf(outfile, "%6ld trees in all found\n", nextree - 1);
      }
      if (nextree > maxtrees + 1) {
        if (treeprint)
          fprintf(outfile, "here are the first%4ld of them\n",(long)maxtrees);
        nextree = maxtrees + 1;
      }
      if (treeprint)
        putc('\n', outfile);
      for (i = 0; i <= (nextree - 2); i++) {
        root = treenode[0];
        add3(treenode[0], treenode[1], treenode[spp], &root, treenode);
        for (j = 3; j <= (spp); j++)
            add3(treenode[bestrees[i][j - 1] - 1], treenode[j - 1],
            treenode[spp + j - 2], &root, treenode);
        if (noroot)
          reroot(treenode[outgrno - 1]);
        didreroot = (outgropt && noroot);
        evaluate(root);
        printree(treeprint, noroot, didreroot, root);
        describe();
        for (j = 1; j < (spp); j++)
          re_move3(&treenode[j], &dummy, &root, treenode);
      }
    }
  } else {
    if (numtrees > 2)
      emboss_initseed(inseed, &inseed0, seed);
    if (treeprint) {
      fprintf(outfile, "User-defined tree");
      if (numtrees > 1)
        putc('s', outfile);
      fprintf(outfile, ":\n\n");
    }
    which = 1;
    if (progress)
      printf("   ");
    while (which <= numtrees ) {
      mix_treeread();
      didreroot = (outgropt && noroot);
      if (noroot)
        reroot(treenode[outgrno - 1]);
      evaluate(root);
      printree(treeprint, noroot, didreroot, root);
      describe();
      which++;
    }
    if (progress)
      printf("\n");
    FClose(intree);
    fprintf(outfile, "\n\n");
    if (numtrees > 2 && chars > 1 ) {
      if (progress)
        printf("   sampling for SH test\n");
      standev(numtrees, minwhich, minsteps, nsteps, fsteps, seed);
    }
  }
  if (jumb == njumble) {
    if (progress) {
      printf("\nOutput written to file \"%s\"\n\n", outfilename);
      if (trout)
        printf("Trees also written onto file \"%s\"\n", outtreename);
      putchar('\n');
    }
  }
  if (ancseq)
    freegarbage(&garbage);
}  /* maketree */

int main(int argc, Char *argv[])
{  /* Mixed parsimony by uphill search */
#ifdef MAC
  argc = 1;         /* macsetup("Mix","");                */
  argv[0] = "Mix";
#endif
  init(argc, argv);
  emboss_getoptions("fmix",argc,argv);
  progname = argv[0];


  ibmpc = IBMCRT;
  ansi = ANSICRT;
  firstset = true;
  garbage = NULL;
  bits = 8*sizeof(long) - 1;
  doinit();

  for (ith = 1; ith <= msets; ith++) {
    if(firstset){
        if (allsokal && !mixture)
            fprintf(outfile, "Camin-Sokal parsimony method\n\n");
        if (allwagner && !mixture)
            fprintf(outfile, "Wagner parsimony method\n\n");
        if (mixture)
            fprintf(outfile, "Mixture of Wagner and Camin-Sokal parsimony methods\n\n");
    }
    doinput();
    if (msets > 1 && !justwts) {
      fprintf(outfile, "Data set # %ld:\n\n",ith);
      if (progress)
        printf("\nData set # %ld:\n",ith);
    }
    if (justwts){
        if(firstset && mixture && (printdata || stepbox || ancseq))
            printmixture(outfile, wagner);
        fprintf(outfile, "Weights set # %ld:\n\n", ith);
        if (progress)
            printf("\nWeights set # %ld:\n\n", ith);
    }
    else if (mixture && (printdata || stepbox || ancseq))
        printmixture(outfile, wagner);
    if (printdata){
        if (weights || justwts)
            printweights(outfile, 0, chars, weight, "Characters");
        if (ancvar)
            printancestors(outfile, anczero, ancone);
    }
    

    if (ith == 1)
      firstset = false;
    for (jumb = 1; jumb <= njumble; jumb++)
      maketree();
  }
  free(place);
  free(steps);
  free(zeroanc);
  free(oneanc);
  free(fulzeroanc);
  free(empzeroanc);
  FClose(outfile);
  FClose(infile);
  FClose(outtree);
#ifdef MAC
  fixmacfile(outtreename);
  fixmacfile(outfilename);
#endif
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  embExit();
  return 0;
}  /* Mixed parsimony by uphill search */
