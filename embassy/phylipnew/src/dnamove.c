
#include "phylip.h"
#include "moves.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1993-2002 by the University of Washington.
   Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

AjPSeqset seqset = NULL;
AjPPhyloProp phyloratecat = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloTree* phylotrees;

#define overr           4
#define which           1

typedef enum {
  horiz, vert, up, overt, upcorner, downcorner, aa, cc, gg, tt, question
  } chartype;

typedef enum {
  rearr, flipp, reroott, none
  } rearrtype;

typedef struct gbase2 {
  baseptr2 base2;
  struct gbase2 *next;
} gbase2;

typedef enum {
  arb, use, spec
  } howtree;

typedef node **pointptr;

#ifndef OLDC
/* function prototypes */
void dnamove_gnu(gbase2 **);
void dnamove_chuck(gbase2 *);
/*void getoptions(void);*/
void emboss_getoptions(char *pgm, int argc, char *argv[]);
void inputoptions(void);
void dnamove_inputdata(AjPSeqset);
void allocrest(void);
void doinput(void);
void configure(void);
void prefix(chartype);
void postfix(chartype);
       
void makechar(chartype);
void dnamove_add(node *, node *, node *);
void dnamove_re_move(node **, node **);
void dnamove_fillin(node *);
void dnamove_postorder(node *);
void evaluate(node *);
void dnamove_reroot(node *);
void dnamove_ancestset(long, long, long *);
void firstrav(node *, long);
void dnamove_hyptrav(node *, long *, long, boolean *);

void dnamove_hypstates(void);
void grwrite(chartype, long, long *);
void dnamove_drawline(long);
void dnamove_printree(void);
void arbitree(void);
void yourtree(void);
void initdnamovenode(node **, node **, node *, long, long, long *,
        long *, initops, pointarray, pointarray, Char *, Char *,
        char**);
void buildtree(void);
void setorder(void);
void mincomp(void);
       
void rearrange(void);
void dnamove_nextinc(void);
void dnamove_nextchar(void);
void dnamove_prevchar(void);
void dnamove_show(void);
void tryadd(node *, node **, node **, double *);
void addpreorder(node *, node *, node *, double *);
void try(void);
void undo(void);
void treewrite(boolean);
       
void clade(void);
void flip(void);
void changeoutgroup(void);
void redisplay(void);
void treeconstruct(void);
/* function prototypes */
#endif



char infilename[FNMLNGTH],intreename[FNMLNGTH],weightfilename[FNMLNGTH];
const char* outtreename;
node *root;
long chars, screenlines, col, treelines, leftedge, topedge, vmargin,
   hscroll, vscroll, scrollinc, screenwidth, farthest;
boolean weights, thresh, waswritten;
boolean usertree, goteof, firsttree, haslengths;  /*treeread variables*/
pointarray nodep;                                  /*treeread variables*/
node *grbg = NULL;                                  /*treeread variables*/
long *zeros;                                          /*treeread variables*/
pointptr treenode;   /* pointers to all nodes in tree */
double threshold;
double *threshwt;
boolean reversed[11];
boolean graphic[11];
unsigned char chh[11];
howtree how;
gbase2 *garbage;
char *progname;

/* Local variables for treeconstruct, propogated global for C version: */

long dispchar, atwhat, what, fromwhere, towhere, oldoutgrno, compatible;
double like, bestyet, gotlike;
boolean display, newtree, changed, subtree, written, oldwritten, restoring,
  wasleft, oldleft, earlytree;
steptr necsteps;
boolean *in_tree;
long sett[31];
steptr numsteps;
node *nuroot;
rearrtype lastop;
Char  ch;
boolean *names;


void dnamove_gnu(gbase2 **p)
{
  /* this and the following are do-it-yourself garbage collectors.
     Make a new node or pull one off the garbage list */
  if (garbage != NULL) {
    *p = garbage;
    garbage = garbage->next;
  } else {
    *p = (gbase2 *)Malloc(sizeof(gbase2));
    (*p)->base2 = (baseptr2)Malloc(chars*sizeof(long));
  }
  (*p)->next = NULL;
}  /* dnamove_gnu */


void dnamove_chuck(gbase2 *p)
{
  /* collect garbage on p -- put it on front of garbage list */
  p->next = garbage;
  garbage = p;
}  /* dnamove_chuck */


/************ EMBOSS GET OPTIONS ROUTINES ******************************/

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjStatus retval;
 
    /* initialize global variables */

    ajNamInit("emboss");
    retval =  ajAcdInitP (pgm, argc, argv, "PHYLIP");

    embossouttree = ajAcdGetOutfile("outtreefile");
    emboss_openfile(embossouttree, &outtree, &outtreename);
    /* ajAcdGet */

    /* init functions for standard ajAcdGet */

    /* cleanup for clashing options */

}

/************ END EMBOSS GET OPTIONS ROUTINES **************************/

/*
//void getoptions()
//{
//  /# interactively set options #/
//  Char ch;
//  boolean done, gotopt;
//  long loopcount;
//
//  how = arb;
//  usertree = false;
//  goteof = false;
//  outgrno = 1;
//  outgropt = false;
//  thresh = false;
//  weights = false;
//  interleaved = true;
//  loopcount = 0;
//  do {
//    cleerhome();
//    printf("\nInteractive DNA parsimony, version %s\n\n",VERSION);
//    printf("Settings for this run:\n");
//    printf("  O                             Outgroup root?");
//    if (outgropt)
//      printf("  Yes, at sequence number%3ld\n", outgrno);
//    else
//      printf("  No, use as outgroup species%3ld\n", outgrno);
//    printf("  W                            Sites weighted?  %s\n",
//           (weights ? "Yes" : "No"));
//    printf("  T                   Use Threshold parsimony?");
//    if (thresh)
//      printf("  Yes, count up to%4.1f per site\n", threshold);
//    else
//      printf("  No, use ordinary parsimony\n");
//    printf("  I               Input sequences interleaved?  %s\n",
//           (interleaved ? "Yes" : "No, sequential"));
//
//    printf("  U   Initial tree (arbitrary, user, specify)?  %s\n",
//           (how == arb) ? "Arbitrary"                :
//           (how == use) ? "User tree from tree file" : "Tree you specify");
//    printf("  0        Graphics type (IBM PC, ANSI, none)?  %s\n",
//           ibmpc ? "IBM PC\n" : ansi  ? "ANSI"     : "(none)");
//    printf("  S                  Width of terminal screen?");
//    printf("%4ld\n", screenwidth);
//    printf("  L                 Number of lines on screen?%4ld\n",screenlines);
//    printf("\nAre these settings correct? ");
//    printf("(type Y or the letter for one to change)\n");
//#ifdef WIN32
//    phyFillScreenColor();
//#endif
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    uppercase(&ch);
//    done = (ch == 'Y');
//    gotopt = (strchr("SOTIU0WL",ch) != NULL) ? true : false;
//    if (gotopt) {
//      switch (ch) {
//        
//      case 'O':
//        outgropt = !outgropt;
//        if (outgropt)
//          initoutgroup(&outgrno, spp);
//        break;
//        
//        case 'T':
//          thresh = !thresh;
//          if (thresh)
//            initthreshold(&threshold);
//          break;
//        
//        case 'I':
//          interleaved = !interleaved;
//          break;
//
//        case 'W':
//          weights = !weights;
//          break;
//
//        case 'U':
//          if (how == arb){
//            how = use;
//            usertree = 1;}
//          else if (how == use){
//            how = spec;
//            usertree = 0;}
//          else
//            how = arb;
//          break;
//        
//        case '0':
//          initterminal(&ibmpc, &ansi);
//          break;
//        
//        case 'S':
//          screenwidth= readlong("Width of terminal screen (in characters)?\n");
//          break;
//
//        case 'L':
//          initnumlines(&screenlines);
//          break;
//        }
//      }
//    if (!(gotopt || done))
//      printf("Not a possible option!\n");
//    countup(&loopcount, 100);   
//  } while (!done);
//  if (scrollinc < screenwidth / 2.0)
//    hscroll = scrollinc;
//  else
//    hscroll = screenwidth / 2;
//  if (scrollinc < screenlines / 2.0)
//    vscroll = scrollinc;
//  else
//    vscroll = screenlines / 2;
//}  /# getoptions #/
*/


void inputoptions()
{
  /* input the information on the options */
  long i;

  for (i = 0; i < (chars); i++)
    weight[i] = 1;
  if (weights){
      inputweightsstr(phyloweights->Str[0], chars, weight, &weights);
      printweights(stdout, 0, chars, weight, "Sites");
  }
  if (!thresh)
    threshold = spp;
  for (i = 0; i < (chars); i++)
    threshwt[i] = threshold * weight[i];
}  /* inputoptions */


void dnamove_inputdata(AjPSeqset seqset)
{
  /* input the names and sequences for each species */
  long i,j, basesread, basesnew=0;
  Char charstate;
  boolean allread;
  /*boolean done;*/
  long ns = 0;   /* temporary base set for input */
  const char* myseq;
  long jlast;

  basesread = 0;
  allread = false;
  while (!(allread)) {
      /*
       if (eoln(infile)) 
       scan_eoln(infile);
       */
    i = 1;
    while (i <= spp ) {
      if ((interleaved && basesread == 0) || !interleaved)
        initnameseq(seqset, i - 1);
      myseq = ajSeqsetSeq(seqset, i);
      jlast = ajSeqLen(ajSeqsetGetSeq(seqset, i));
      j = 0;
      while (j < jlast) {
          charstate = myseq[j];
          uppercase(&charstate);
          if ((strchr("ABCDGHKMNRSTUVWXY?O-",charstate)) == NULL){
            printf("ERROR: bad base: %c at site %5ld of species %3ld\n",
                   charstate, j+1, i);
            if (charstate == '.') {
              printf("       Periods (.) may not be used as gap characters.\n");
              printf("       The correct gap character is (-)\n");
            }
            exxit(-1);
          }
          j++;
          switch (charstate) {
        
          case 'A':
            ns = 1L << ((long)A);
            break;
        
          case 'C':
            ns = 1L << ((long)C);
            break;
        
          case 'G':
            ns = 1L << ((long)G);
            break;
        
          case 'U':
            ns = 1L << ((long)T);
            break;
        
          case 'T':
            ns = 1L << ((long)T);
            break;
        
          case 'M':
            ns = (1L << ((long)A)) | (1L << ((long)C));
            break;
        
          case 'R':
            ns = (1L << ((long)A)) | (1L << ((long)G));
            break;
        
          case 'W':
            ns = (1L << ((long)A)) | (1L << ((long)T));
            break;
        
          case 'S':
            ns = (1L << ((long)C)) | (1L << ((long)G));
            break;
        
          case 'Y':
            ns = (1L << ((long)C)) | (1L << ((long)T));
            break;
        
          case 'K':
            ns = (1L << ((long)G)) | (1L << ((long)T));
            break;
        
          case 'B':
            ns = (1L << ((long)C)) | (1L << ((long)G)) | (1L << ((long)T));
            break;
        
          case 'D':
            ns = (1L << ((long)A)) | (1L << ((long)G)) | (1L << ((long)T));
            break;
        
          case 'H':
            ns = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)T));
            break;
        
          case 'V':
            ns = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G));
            break;
        
          case 'N':
            ns = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G)) |
              (1L << ((long)T));
            break;
        
          case 'X':
            ns = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G)) |
              (1L << ((long)T));
            break;
        
          case '?':
            ns = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G)) |
                    (1L << ((long)T)) | (1L << ((long)O));
            break;
        
          case 'O':
            ns = 1L << ((long)O);
            break;
        
          case '.':
            ns = treenode[0]->base2[j - 1];
            break;
        
          case '-':
            ns = 1L << ((long)O);
            break;
          }
          treenode[i - 1]->base2[j - 1] = ns;
        }
      i++;
    }
    if (interleaved) {
      basesread = basesnew;
      allread = (basesread == chars);
    } else
      allread = (i > spp);
  }
  root = NULL;
  printf("\n\n");
}  /* dnamove_inputdata */


void allocrest()
{
  nayme = (naym *)Malloc(spp*sizeof(naym));
  in_tree = (boolean *)Malloc(nonodes*sizeof(boolean));
  weight = (steptr)Malloc(chars*sizeof(long));
  numsteps = (steptr)Malloc(chars*sizeof(long));
  necsteps = (steptr)Malloc(chars*sizeof(long));
  threshwt = (double *)Malloc(chars*sizeof(double));
}  /* allocrest */


void doinput()
{
  /* reads the input data */
  long i, j;
  node *p;

  inputnumbersseq(seqset, &spp, &chars, &nonodes, 1);
  printf("%2ld species, %3ld  sites\n", spp, chars);
/*  getoptions();*/
  printf("\nReading input file ...\n\n");
  /*
     if (weights)
    openfile(&weightfile,WEIGHTFILE,"weights file","r",progname,weightfilename);
*/
  allocrest();
  inputoptions();
  alloctree(&treenode, nonodes, usertree);
  for (i = 0; i < (spp); i++)
    treenode[i]->base2 = (baseptr2)Malloc(chars*sizeof(long));
  if(!usertree) {
        for (i = spp; i < (nonodes); i++) {
          p = treenode[i];
          for (j = 1; j <= 3; j++) {
            p->base2 = (baseptr2)Malloc(chars*sizeof(long));
            p = p->next;
          }
        }
  } 
  setuptree(treenode, nonodes, usertree);
  dnamove_inputdata(seqset);
}  /* doinput */


void configure()
{
  /* configure to machine -- set up special characters */
  chartype a;

  for (a = horiz; (long)a <= (long)question; a = (chartype)((long)a + 1))
    reversed[(long)a] = false;
  for (a = horiz; (long)a <= (long)question; a = (chartype)((long)a + 1))
    graphic[(long)a] = false;
  if (ibmpc) {
    chh[(long)horiz] = 205;
    graphic[(long)horiz] = true;
    chh[(long)vert] = 186;
    graphic[(long)vert] = true;
    chh[(long)up] = 186;
    graphic[(long)up] = true;
    chh[(long)overt] = 205;
    graphic[(long)overt] = true;
    chh[(long)upcorner] = 200;
    graphic[(long)upcorner] = true;
    chh[(long)downcorner] = 201;
    graphic[(long)downcorner] = true;
    chh[(long)aa] = 176;
    chh[(long)cc] = 178;
    chh[(long)gg] = 177;
    chh[(long)tt] = 219;
    chh[(long)question] = '\001';
    return;
  }
  if (ansi) {
    chh[(long)horiz] = ' ';
    reversed[(long)horiz] = true;
    chh[(long)vert] = chh[(long)horiz];
    reversed[(long)vert] = true;
    chh[(long)up] = 'x';
    graphic[(long)up] = true;
    chh[(long)overt] = 'q';
    graphic[(long)overt] = true;
    chh[(long)upcorner] = 'm';
    graphic[(long)upcorner] = true;
    chh[(long)downcorner] = 'l';
    graphic[(long)downcorner] = true;
    chh[(long)aa] = 'a';
    reversed[(long)aa] = true;
    chh[(long)cc] = 'c';
    reversed[(long)cc] = true;
    chh[(long)gg] = 'g';
    reversed[(long)gg] = true;
    chh[(long)tt] = 't';
    reversed[(long)tt] = true;
    chh[(long)question] = '?';
    reversed[(long)question] = true;
    return;
  }
  chh[(long)horiz] = '=';
  chh[(long)vert] = ' ';
  chh[(long)up] = '!';
  chh[(long)upcorner] = '`';
  chh[(long)downcorner] = ',';
  chh[(long)overt] = '-';
  chh[(long)aa] = 'a';
  chh[(long)cc] = 'c';
  chh[(long)gg] = 'g';
  chh[(long)tt] = 't';
  chh[(long)question] = '.';
}  /* configure */


void prefix(chartype a)
{
  /* give prefix appropriate for this character */
  if (reversed[(long)a])
    prereverse(ansi);
  if (graphic[(long)a])
    pregraph2(ansi);
}  /* prefix */


void postfix(chartype a)
{
  /* give postfix appropriate for this character */
  if (reversed[(long)a])
    postreverse(ansi);
  if (graphic[(long)a])
    postgraph2(ansi);
}  /* postfix */


void makechar(chartype a)
{
  /* print out a character with appropriate prefix or postfix */
  prefix(a);
  putchar(chh[(long)a]);
  postfix(a);
}  /* makechar */


void dnamove_add(node *below, node *newtip, node *newfork)
{
  /* inserts the nodes newfork and its left descendant, newtip,
     to the tree.  below becomes newfork's right descendant */
  boolean putleft;
  node *leftdesc, *rtdesc;

  if (below != treenode[below->index - 1])
    below = treenode[below->index - 1];
  if (below->back != NULL)
    below->back->back = newfork;
  newfork->back = below->back;
  putleft = true;
  if (restoring)
    putleft = wasleft;
  if (putleft) {
    leftdesc = newtip;
    rtdesc = below;
  } else {
    leftdesc = below;
    rtdesc = newtip;
  }
  rtdesc->back = newfork->next->next;
  newfork->next->next->back = rtdesc;
  newfork->next->back = leftdesc;
  leftdesc->back = newfork->next;
  if (root == below)
    root = newfork;
  root->back = NULL;
}  /* dnamove_add */


void dnamove_re_move(node **item, node **fork)
{
  /* removes nodes item and its ancestor, fork, from the tree.
     the new descendant of fork's ancestor is made to be
     fork's second descendant (other than item).  Also
     returns pointers to the deleted nodes, item and fork */
  node *p, *q;

  if ((*item)->back == NULL) {
    *fork = NULL;
    return;
  }
  *fork = treenode[(*item)->back->index - 1];
  if (*item == (*fork)->next->back) {
    if (root == *fork)
      root = (*fork)->next->next->back;
    wasleft = true;
  } else {
    if (root == *fork)
      root = (*fork)->next->back;
    wasleft = false;
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
}  /* dnamove_re_move */


void dnamove_fillin(node *p)
{
  /* sets up for each node in the tree the base sequence
     at that point and counts the changes.  The program
     spends much of its time in this PROCEDURE */
  long i;
  long ns, rs, ls;

  for (i = 0; i < (chars); i++) {
    ls = p->next->back->base2[i];
    rs = p->next->next->back->base2[i];
    ns = ls & rs;
    if (ns == 0) {
      ns = ls | rs;
      numsteps[i] += weight[i];
    }
    p->base2[i] = ns;
  }
}  /* dnamove_fillin */


void dnamove_postorder(node *p)
{
  /* traverses a binary tree, calling PROCEDURE fillin at a
     node's descendants before calling fillin at the node */

  if (p->tip)
    return;
  dnamove_postorder(p->next->back);
  dnamove_postorder(p->next->next->back);
  dnamove_fillin(p);
}  /* dnamove_postorder */


void evaluate(node *r)
{
  /* determines the number of steps needed for a tree. this is
     the minimum number of steps needed to evolve sequences on
     this tree */
  long i, steps;
  double sum;

  compatible = 0;
  sum = 0.0;
  for (i = 0; i < (chars); i++)
    numsteps[i] = 0;
  dnamove_postorder(r);
  for (i = 0; i < (chars); i++) {
    steps = numsteps[i];
    if (steps <= threshwt[i])
      sum += steps;
    else
      sum += threshwt[i];
    if (steps <= necsteps[i] && !earlytree)
      compatible += weight[i];
  }
  like = -sum;
  /*printf("like: %f\n",like);*/
}  /* evaluate */


void dnamove_reroot(node *outgroup)
{
  /* reorients tree, putting outgroup in desired position. */
  node *p, *q, *newbottom, *oldbottom;
  boolean onleft;

  if (outgroup->back->index == root->index)
    return;
  newbottom = outgroup->back;
  p = treenode[newbottom->index - 1]->back;
  while (p->index != root->index) {
    oldbottom = treenode[p->index - 1];
    treenode[p->index - 1] = p;
    p = oldbottom->back;
  }
  onleft = (p == root->next);
  if (restoring)
    if (!onleft && wasleft){
      p = root->next->next;
      q = root->next;
    } else {
      p = root->next;
      q = root->next->next;
    }
  else {
    if (onleft)
      oldoutgrno = root->next->next->back->index;
    else
      oldoutgrno = root->next->back->index;
    wasleft = onleft;
    p = root->next;
    q = root->next->next;
  }
  p->back->back = q->back;
  q->back->back = p->back;
  p->back = outgroup;
  q->back = outgroup->back;
  if (restoring) {
    if (!onleft && wasleft) {
      outgroup->back->back = root->next;
      outgroup->back = root->next->next;
    } else {
      outgroup->back->back = root->next->next;
      outgroup->back = root->next;
    }
  } else {
    outgroup->back->back = root->next->next;
    outgroup->back = root->next;
  }
  treenode[newbottom->index - 1] = newbottom;
}  /* dnamove_reroot */


void dnamove_ancestset(long a, long b, long *c)
{
  /* make the set of ancestral states below nodes
     whose base sets are a and b */
  *c = a & b;
  if (*c == 0)
    *c = a | b;
}  /* dnamove_ancestset */


void firstrav(node *r, long i)
{
  /* initial traverse for hypothetical states */
  if (r->tip)
    return;
  firstrav(r->next->back, i);
  firstrav(r->next->next->back, i);
  dnamove_ancestset(r->next->back->base2[i - 1],
            r->next->next->back->base2[i - 1], &r->base2[i - 1]);
}  /* firstrav */


void dnamove_hyptrav(node *r, long *hypset, long i, boolean *bottom)
{
  /*  compute, print out state at one interior node */
  long tempset, left, rt, anc;
  gbase2 *temparray, *ancset;

  dnamove_gnu(&ancset);
  dnamove_gnu(&temparray);
  anc = hypset[i - 1];
  if (!r->tip) {
    left = r->next->back->base2[i - 1];
    rt = r->next->next->back->base2[i - 1];
    tempset = left & rt & anc;
    if (tempset == 0) {
      tempset = (left & rt) | (left & anc) | (rt & anc);
      if (tempset == 0)
        tempset = left | rt | anc;
    }
    r->base2[i - 1] = tempset;
  }
  r->state = '?';
  if (r->base2[dispchar - 1] == 1L << ((long)A))
    r->state = 'A';
  if (r->base2[dispchar - 1] == 1L << ((long)C))
    r->state = 'C';
  if (r->base2[dispchar - 1] == 1L << ((long)G))
    r->state = 'G';
  if (r->base2[dispchar - 1] == 1L << ((long)T))
    r->state = 'T';
  *bottom = false;
  if (!r->tip) {
    memcpy(temparray->base2, r->next->back->base2, chars*sizeof(long));
    dnamove_ancestset(hypset[i - 1], r->next->next->back->base2[i - 1],
              &ancset->base2[i - 1]);
    dnamove_hyptrav(r->next->back, ancset->base2, i,bottom);
    dnamove_ancestset(hypset[i - 1], temparray->base2[i - 1],
              &ancset->base2[i - 1]);
    dnamove_hyptrav(r->next->next->back, ancset->base2, i,bottom);
  }
  dnamove_chuck(temparray);
  dnamove_chuck(ancset);
}  /* dnamove_hyptrav */

void dnamove_hypstates()
{
  /* fill in and describe states at interior nodes */
  long i;
  boolean bottom;
  baseptr2 nothing;

  i = dispchar;
  nothing = (baseptr2)Malloc(chars*sizeof(long));
  nothing[i - 1] = 0;
  bottom = true;
  firstrav(root, i);
  dnamove_hyptrav(root, nothing, i,&bottom);
  free(nothing);
}  /* dnamove_hypstates */


void grwrite(chartype c, long num, long *pos)
{
  long i;

  prefix(c);
  for (i = 1; i <= num; i++) {
    if ((*pos) >= leftedge && (*pos) - leftedge + 1 < screenwidth)
      putchar(chh[(long)c]);
    (*pos)++;
  }
  postfix(c);
}  /* grwrite */


void dnamove_drawline(long i)
{
  /* draws one row of the tree diagram by moving up tree */
  node *p, *q, *r, *first =NULL, *last =NULL;
  long n, j, pos;
  boolean extra, done;
  Char st;
  chartype c, d;

  pos = 1;
  p = nuroot;
  q = nuroot;
  extra = false;
  if (i == p->ycoord && (p == root || subtree)) {
    extra = true;
    c = overt;
    if (display) {
      switch (p->state) {
        
      case 'A':
        c = aa;
        break;
        
      case 'C':
        c = cc;
        break;
        
      case 'G':
        c = gg;
        break;
        
      case 'T':
        c = tt;
        break;
        
      case '?':
        c = question;
        break;
      }
    }
    if ((subtree))
      stwrite("Subtree:", 8, &pos, leftedge, screenwidth);
    if (p->index >= 100)
      nnwrite(p->index, 3, &pos, leftedge, screenwidth);
    else if (p->index >= 10) {
      grwrite(c, 1, &pos);
      nnwrite(p->index, 2, &pos, leftedge, screenwidth);
    } else {
      grwrite(c, 2, &pos);
      nnwrite(p->index, 1, &pos, leftedge, screenwidth);
    }
  } else {
    if ((subtree))
      stwrite("          ", 10, &pos, leftedge, screenwidth);
    else
      stwrite("  ", 2, &pos, leftedge, screenwidth);
  }
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
    n = p->xcoord - q->xcoord;
    if (n < 3 && !q->tip)
      n = 3;
    if (extra) {
      n--;
      extra = false;
    }
    if (q->ycoord == i && !done) {
      if (q->ycoord > p->ycoord)
        d = upcorner;
      else
        d = downcorner;
      c = overt;
      if (display) {
        switch (q->state) {
        
        case 'A':
          c = aa;
          break;
        
        case 'C':
          c = cc;
          break;
        
        case 'G':
          c = gg;
          break;
        
        case 'T':
          c = tt;
          break;
        
        case '?':
          c = question;
          break;
        }
        d = c;
      }
      if (n > 1) {
        grwrite(d, 1, &pos);
        grwrite(c, n - 3, &pos);
      }
      if (q->index >= 100)
        nnwrite(q->index, 3, &pos, leftedge, screenwidth);
      else if (q->index >= 10) {
        grwrite(c, 1, &pos);
        nnwrite(q->index, 2, &pos, leftedge, screenwidth);
      } else {
        grwrite(c, 2, &pos);
        nnwrite(q->index, 1, &pos, leftedge, screenwidth);
      }
      extra = true;
    } else if (!q->tip) {
      if (last->ycoord > i && first->ycoord < i && i != p->ycoord) {
        c = up;
        if (i < p->ycoord)
          st = p->next->back->state;
        else
          st = p->next->next->back->state;
        if (display) {
          switch (st) {
        
          case 'A':
            c = aa;
            break;
        
          case 'C':
            c = cc;
            break;
        
          case 'G':
            c = gg;
            break;
        
          case 'T':
            c = tt;
            break;
        
          case '?':
            c = question;
            break;
          }
        }
        grwrite(c, 1, &pos);
        chwrite(' ', n - 1, &pos, leftedge, screenwidth);
      } else
        chwrite(' ', n, &pos, leftedge, screenwidth);
    } else
      chwrite(' ', n, &pos, leftedge, screenwidth);
    if (p != q)
      p = q;
  } while (!done);
  if (p->ycoord == i && p->tip) {
    n = 0;
    for (j = 1; j <= nmlngth; j++) {
      if (nayme[p->index - 1][j - 1] != '\0')
        n = j;
    }
    chwrite(':', 1, &pos, leftedge, screenwidth);
    for (j = 0; j < n; j++)
      chwrite(nayme[p->index - 1][j], 1, &pos, leftedge, screenwidth);
  }
  putchar('\n');
}  /* dnamove_drawline */

void dnamove_printree()
{
  /* prints out diagram of the tree */
  long tipy;
  long i, dow;

  if (!subtree)
    nuroot = root;
  if (changed || newtree)
    evaluate(root);
  if (display)
    dnamove_hypstates();
  printf((ansi || ibmpc) ? "\033[2J\033[H" : "\n");
  tipy = 1;
  dow = down;
  if (spp * dow > screenlines && !subtree)
    dow--;

  printf("(unrooted)");
  if (display) {
    printf(" ");
    makechar(aa);
    printf(":A ");
    makechar(cc);
    printf(":C ");
    makechar(gg);
    printf(":G ");
    makechar(tt);
    printf(":T ");
    makechar(question);
    printf(":?");
  } else
    printf("                    ");
  if (!earlytree) {
    printf("%10.1f Steps", -like);
  }
  if (display)
    printf("  SITE%4ld", dispchar);
  else
    printf("         ");
  if (!earlytree) {
    printf("  %3ld sites compatible\n", compatible);
  }

  printf("                            ");
  if (changed && !earlytree) {
    if (-like < bestyet) {
      printf("     BEST YET!");
      bestyet = -like;
    } else if (fabs(-like - bestyet) < 0.000001)
      printf("     (as good as best)");
    else {
      if (-like < gotlike)
        printf("     better");
      else if (-like > gotlike)
        printf("     worse!");
    }
  }
  printf("\n");

  farthest = 0;
  coordinates(nuroot, &tipy, 1.5, &farthest);
  vmargin = 4;
  treelines = tipy - dow;
  if (topedge != 1) {
    printf("** %ld lines above screen **\n", topedge - 1);
    vmargin++;
  }
  if ((treelines - topedge + 1) > (screenlines - vmargin))
    vmargin++;
  for (i = 1; i <= treelines; i++) {
    if (i >= topedge && i < topedge + screenlines - vmargin)
      dnamove_drawline(i);
  }
  if ((treelines - topedge + 1) > (screenlines - vmargin)) {
    printf("** %ld", treelines - (topedge - 1 + screenlines - vmargin));
    printf(" lines below screen **\n");
  }
  if (treelines - topedge + vmargin + 1 < screenlines)
    putchar('\n');
  gotlike = -like;
  changed = false;
}  /* dnamove_printree */


void arbitree()
{
  long i;

  root = treenode[0];
  dnamove_add(treenode[0], treenode[1], treenode[spp]);
  for (i = 3; i <= (spp); i++)
    dnamove_add(treenode[spp + i - 3], treenode[i - 1], treenode[spp + i - 2]);
  for (i = 0; i < (nonodes); i++)
    in_tree[i] = true;
}  /* arbitree */


void yourtree()
{
  long i, j;
  boolean ok;

  root = treenode[0];
  dnamove_add(treenode[0], treenode[1], treenode[spp]);
  i = 2;
  do {
    i++;
    dnamove_printree();
    printf("Add species%3ld: ", i);
    for (j = 0; j < nmlngth; j++)
      putchar(nayme[i - 1][j]);
    do {
      printf("\n before node (type number): ");
      inpnum(&j, &ok);
      ok = (ok && ((j >= 1 && j < i) || (j > spp && j < spp + i - 1)));
      if (!ok)
        printf("Impossible number. Please try again:\n");
    } while (!ok);
    dnamove_add(treenode[j - 1], treenode[i - 1], treenode[spp + i - 2]);
  } while (i != spp);
  for (i = 0; i < (nonodes); i++)
    in_tree[i] = true;
}  /* yourtree */


void initdnamovenode(node **p, node **grbg, node *q, long len, long nodei,
                        long *ntips, long *parens, initops whichinit,
                        pointarray treenode, pointarray nodep, Char *str, Char *ch,
                        char** treestr)
{
  /* initializes a node */
  /* LM 7/27  I added this function and the commented lines around */
  /* treeread() to get the program running, but all 4 move programs*/
  /* are improperly integrated into the v4.0 support files.  As is */
  /* endsite = chars and this is a patchwork function                   */
  boolean minusread;
  double valyew, divisor;

  switch (whichinit) {
  case bottom:
    gnutreenode(grbg, p, nodei, endsite, zeros);
    treenode[nodei - 1] = *p;
    break;
  case nonbottom:
    gnutreenode(grbg, p, nodei, endsite, zeros);
    break;
  case tip:
    match_names_to_data (str, treenode, p, spp);
    break;
  case length:
    processlength(&valyew, &divisor, ch, &minusread, treestr, parens);
    /* process lengths and discard */
  default:      /*cases hslength,hsnolength,treewt,unittrwt,iter,*/
    break;
  }
} /* initdnamovenode */


void buildtree()
{
  long i, nextnode;
  node *p;
  long j;
  char* treestr;

  changed = false;
  newtree = false;
  switch (how) {

  case arb:
    arbitree();
    break;

  case use:
    /*openfile(&intree,intreename,"input tree file", "r",progname,intreename);*/
    names = (boolean *)Malloc(spp*sizeof(boolean));
    firsttree = true;                                        /**/
    nodep = NULL;                                        /**/
    nextnode = 0;                                        /**/
    haslengths = 0;                                        /**/
    endsite = chars;                        /*debug*/
    zeros = (long *)Malloc(endsite*sizeof(long));        /**/
    for (i = 0; i < endsite; i++)                        /**/
      zeros[i] = 0;
                                       /**/
    treestr = ajStrStrMod(&phylotrees[0]->Tree);
    treeread(&treestr, &root, treenode, &goteof, &firsttree,
                nodep, &nextnode, &haslengths,
                &grbg, initdnamovenode); /*debug*/
    for (i = spp; i < (nonodes); i++) {
      p = treenode[i];
      for (j = 1; j <= 3; j++) {
        p->base2 = (baseptr2)Malloc(chars*sizeof(long));
        p = p->next;
      } 
    } /* debug: see comment at initdnamovenode() */

    for (i = 0; i < (spp); i++)
      in_tree[i] = names[i];
    free(names);
    /*FClose(intree);*/
    break;

  case spec:
    yourtree();
    break;
  }
  if (!outgropt)
    outgrno = root->next->back->index;
  if (outgropt && in_tree[outgrno - 1])
    dnamove_reroot(treenode[outgrno - 1]);
}  /* buildtree */


void setorder()
{
  /* sets in order of number of members */
  sett[0] = 1L << ((long)A);
  sett[1] = 1L << ((long)C);
  sett[2] = 1L << ((long)G);
  sett[3] = 1L << ((long)T);
  sett[4] = 1L << ((long)O);
  sett[5] = (1L << ((long)A)) | (1L << ((long)C));
  sett[6] = (1L << ((long)A)) | (1L << ((long)G));
  sett[7] = (1L << ((long)A)) | (1L << ((long)T));
  sett[8] = (1L << ((long)A)) | (1L << ((long)O));
  sett[9] = (1L << ((long)C)) | (1L << ((long)G));
  sett[10] = (1L << ((long)C)) | (1L << ((long)T));
  sett[11] = (1L << ((long)C)) | (1L << ((long)O));
  sett[12] = (1L << ((long)G)) | (1L << ((long)T));
  sett[13] = (1L << ((long)G)) | (1L << ((long)O));
  sett[14] = (1L << ((long)T)) | (1L << ((long)O));
  sett[15] = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G));
  sett[16] = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)T));
  sett[17] = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)O));
  sett[18] = (1L << ((long)A)) | (1L << ((long)G)) | (1L << ((long)T));
  sett[19] = (1L << ((long)A)) | (1L << ((long)G)) | (1L << ((long)O));
  sett[20] = (1L << ((long)A)) | (1L << ((long)T)) | (1L << ((long)O));
  sett[21] = (1L << ((long)C)) | (1L << ((long)G)) | (1L << ((long)T));
  sett[22] = (1L << ((long)C)) | (1L << ((long)G)) | (1L << ((long)O));
  sett[23] = (1L << ((long)C)) | (1L << ((long)T)) | (1L << ((long)O));
  sett[24] = (1L << ((long)G)) | (1L << ((long)T)) | (1L << ((long)O));
  sett[25] = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G)) |
    (1L << ((long)T));
  sett[26] = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G)) |
    (1L << ((long)O));
  sett[27] = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)T)) |
    (1L << ((long)O));
  sett[28] = (1L << ((long)A)) | (1L << ((long)G)) | (1L << ((long)T)) |
    (1L << ((long)O));
  sett[29] = (1L << ((long)C)) | (1L << ((long)G)) | (1L << ((long)T)) |
    (1L << ((long)O));
  sett[30] = (1L << ((long)A)) | (1L << ((long)C)) | (1L << ((long)G)) |
    (1L << ((long)T)) | (1L << ((long)O));
}  /* setorder */


void mincomp()
{
  /* computes for each site the minimum number of steps
     necessary to accomodate those species already
     in the analysis */
  long i, j, k;
  boolean done;

  for (i = 0; i < (chars); i++) {
    done = false;
    j = 0;
    while (!done) {
      j++;
      done = true;
      k = 1;
      do {
        if (in_tree[k - 1])
          done = (done && (treenode[k - 1]->base2[i] & sett[j - 1]) != 0);
        k++;
      } while (k <= spp && done);
    }
    if (j == 31)
      necsteps[i] = 4;
    if (j <= 30)
      necsteps[i] = 3;
    if (j <= 25)
      necsteps[i] = 2;
    if (j <= 15)
      necsteps[i] = 1;
    if (j <= 5)
      necsteps[i] = 0;
    necsteps[i] *= weight[i];
  }
}  /* mincomp */


void rearrange()
{
  long i, j;
  boolean ok1, ok2;
  node *p, *q;

  printf("Remove everything to the right of which node? ");
  inpnum(&i, &ok1);
  ok1 = (ok1 && i >= 1 && i < spp * 2 && i != root->index);
  if (ok1) {
    printf("Add before which node? ");
    inpnum(&j, &ok2);
    ok2 = (ok2 && j >= 1 && j < spp * 2);
    if (ok2) {
      ok2 = (treenode[j - 1] != treenode[treenode[i - 1]->back->index - 1]);
      p = treenode[j - 1];
      while (p != root) {
        ok2 = (ok2 && p != treenode[i - 1]);
        p = treenode[p->back->index - 1];
      }
      if (ok1 && ok2) {
        what = i;
        q = treenode[treenode[i - 1]->back->index - 1];
        if (q->next->back->index == i)
          fromwhere = q->next->next->back->index;
        else
          fromwhere = q->next->back->index;
        towhere = j;
        dnamove_re_move(&treenode[i - 1], &q);
        dnamove_add(treenode[j - 1], treenode[i - 1], q);
      }
      lastop = rearr;
    }
  }
  changed = (ok1 && ok2);
  dnamove_printree();
  if (!(ok1 && ok2))
    printf("Not a possible rearrangement.  Try again: \n");
  else {
    oldwritten = written;
    written = false;
  }
}  /* rearrange */

void dnamove_nextinc()
{
  /* show next incompatible site */
  long disp0;
  boolean done;

  display = true;
  disp0 = dispchar;
  done = false;
  do {
    dispchar++;
    if (dispchar > chars) {
      dispchar = 1;
      done = (disp0 == 0);
    }
  } while (!(necsteps[dispchar - 1] != numsteps[dispchar - 1] ||
             dispchar == disp0 || done));
  dnamove_printree();
}  /* dnamove_nextinc */

void dnamove_nextchar()
{
  /* show next site */
  display = true;
  dispchar++;
  if (dispchar > chars)
    dispchar = 1;
  dnamove_printree();
}  /* dnamove_nextchar */

void dnamove_prevchar()
{
  /* show previous site */
  display = true;
  dispchar--;
  if (dispchar < 1)
    dispchar = chars;
  dnamove_printree();
}  /* dnamove_prevchar */

void dnamove_show()
{
  long i;
  boolean ok;

  do {
    printf("SHOW: (Character number or 0 to see none)? ");
    inpnum(&i, &ok);
    ok = (ok && (i == 0 || (i >= 1 && i <= chars)));
    if (ok && i != 0) {
      display = true;
      dispchar = i;
    }
    if (ok && i == 0)
      display = false;
  } while (!ok);
  dnamove_printree();
}  /* dnamove_show */


void tryadd(node *p, node **item, node **nufork, double *place)
{
  /* temporarily adds one fork and one tip to the tree.
     Records scores in ARRAY place */
  dnamove_add(p, *item, *nufork);
  evaluate(root);
  place[p->index - 1] = -like;
  dnamove_re_move(item, nufork);
}  /* tryadd */


void addpreorder(node *p, node *item_, node *nufork_, double *place)
{
  /* traverses a binary tree, calling PROCEDURE tryadd
     at a node before calling tryadd at its descendants */
  node *item, *nufork;

  item = item_;
  nufork = nufork_;
  if (p == NULL)
    return;
  tryadd(p,&item,&nufork,place);
  if (!p->tip) {
    addpreorder(p->next->back, item,nufork,place);
    addpreorder(p->next->next->back,item,nufork,place);
  }
}  /* addpreorder */


void try()
{
  /* Remove node, try it in all possible places */
  double *place;
  long i, j, oldcompat;
  double current;
  node *q, *dummy, *rute;
  boolean tied, better, ok;

  printf("Try other positions for which node? ");
  inpnum(&i, &ok);
  if (!(ok && i >= 1 && i <= nonodes && i != root->index)) {
    printf("Not a possible choice! ");
    return;
  }
  printf("WAIT ...\n");
  place = (double *)Malloc(nonodes*sizeof(double));
  for (j = 0; j < (nonodes); j++)
    place[j] = -1.0;
  evaluate(root);
  current = -like;
  oldcompat = compatible;
  what = i;
  q = treenode[treenode[i - 1]->back->index - 1];
  if (q->next->back->index == i)
    fromwhere = q->next->next->back->index;
  else
    fromwhere = q->next->back->index;
  rute = root;
  if (root == treenode[treenode[i - 1]->back->index - 1]) {
    if (treenode[treenode[i - 1]->back->index - 1]->next->back == treenode[i - 1])
      rute = treenode[treenode[i - 1]->back->index - 1]->next->next->back;
    else
      rute = treenode[treenode[i - 1]->back->index - 1]->next->back;
  }
  dnamove_re_move(&treenode[i - 1], &dummy);
  oldleft = wasleft;
  root = rute;
  addpreorder(root, treenode[i - 1], dummy, place);
  wasleft = oldleft;
  restoring = true;
  dnamove_add(treenode[fromwhere - 1], treenode[what - 1], q);
  like = -current;
  compatible = oldcompat;
  restoring = false;
  better = false;
  printf("       BETTER: ");
  for (j = 1; j <= (nonodes); j++) {
    if (place[j - 1] < current && place[j - 1] >= 0.0) {
      printf("%3ld:%6.2f", j, place[j - 1]);
      better = true;
    }
  }
  if (!better)
    printf(" NONE");
  printf("\n       TIED:    ");
  tied = false;
  for (j = 1; j <= (nonodes); j++) {
    if (fabs(place[j - 1] - current) < 1.0e-6 && j != fromwhere) {
      if (j < 10)
        printf("%2ld", j);
      else
        printf("%3ld", j);
      tied = true;
    }
  }
  if (tied)
    printf(":%6.2f\n", current);
  else
    printf("NONE\n");
  changed = true;
  free(place);
}  /* try */


void undo()
{
  /* restore to tree before last rearrangement */
  long temp;
  boolean btemp;
  node *q;

  switch (lastop) {

  case rearr:
    restoring = true;
    oldleft = wasleft;
    dnamove_re_move(&treenode[what - 1], &q);
    btemp = wasleft;
    wasleft = oldleft;
    dnamove_add(treenode[fromwhere - 1], treenode[what - 1],q);
    wasleft = btemp;
    restoring = false;
    temp = fromwhere;
    fromwhere = towhere;
    towhere = temp;
    changed = true;
    break;

  case flipp:
    q = treenode[atwhat - 1]->next->back;
    treenode[atwhat - 1]->next->back =treenode[atwhat - 1]->next->next->back;
    treenode[atwhat - 1]->next->next->back = q;
    treenode[atwhat - 1]->next->back->back = treenode[atwhat - 1]->next;
    treenode[atwhat - 1]->next->next->back->back =
      treenode[atwhat - 1]->next->next;
    break;

  case reroott:
    restoring = true;
    temp =oldoutgrno;
    oldoutgrno = outgrno;
    outgrno = temp;
    dnamove_reroot(treenode[outgrno - 1]);
    restoring = false;
    break;

  case none:
    /* blank case */
    break;
  }
  dnamove_printree();
  if (lastop == none) {
    printf("No operation to undo! ");
    return;
  }
  btemp = oldwritten;
  oldwritten = written;
  written = btemp;
}  /* undo */


void treewrite(boolean done)
{
  /* write out tree to a file */
  /*Char ch;*/

  /*treeoptions(waswritten, &ch, &outtree, outtreename, progname);*/
  if (!done)
    dnamove_printree();
  if (waswritten && ch != 'A' && ch != 'R')
    return;
  col = 0;
  treeout(root, 1, &col, root);
  printf("\nTree written to file \"%s\"\n\n", outtreename);
  waswritten = true;
  written = true;
  FClose(outtree);
#ifdef MAC
  fixmacfile(outtreename);
#endif
}
/* treewrite */


void clade()
{
  /* pick a subtree and show only that on screen */
  long i;
  boolean ok;

  printf("Select subtree rooted at which node (0 for whole tree)? ");
  inpnum(&i, &ok);
  ok = (ok && ((unsigned)i) <= ((unsigned)nonodes));
  if (ok) {
    subtree = (i > 0);
    if (subtree)
      nuroot = treenode[i - 1];
    else
      nuroot = root;
  }
  dnamove_printree();
  if (!ok)
    printf("Not possible to use this node. ");
}  /* clade */


void flip()
{
  /* flip at a node left-right */
  long i;
  boolean ok;
  node *p;

  printf("Flip branches at which node? ");
  inpnum(&i, &ok);
  ok = (ok && i > spp && i <= nonodes);
  if (ok) {
    p = treenode[i - 1]->next->back;
    treenode[i - 1]->next->back = treenode[i - 1]->next->next->back;
    treenode[i - 1]->next->next->back = p;
    treenode[i - 1]->next->back->back = treenode[i - 1]->next;
    treenode[i - 1]->next->next->back->back = treenode[i - 1]->next->next;
    atwhat = i;
    lastop = flipp;
  }
  dnamove_printree();
  if (ok) {
    oldwritten = written;
    written = false;
    return;
  }
  if (i >= 1 && i <= spp)
    printf("Can't flip there. ");
  else
    printf("No such node. ");
}  /* flip */

void changeoutgroup()
{
  long i;
  boolean ok;

  oldoutgrno = outgrno;
  do {
    printf("Which node should be the new outgroup? ");
    inpnum(&i, &ok);
    ok = (ok && in_tree[i - 1] && i >= 1 && i <= nonodes &&
          i != root->index);
    if (ok)
      outgrno = i;
  } while (!ok);
  if (in_tree[outgrno - 1])
    dnamove_reroot(treenode[outgrno - 1]);
  changed = true;
  lastop = reroott;
  dnamove_printree();
  oldwritten = written;
  written = false;
}  /* changeoutgroup */


void redisplay()
{
  boolean done = false;
  waswritten = false;
  do {
    printf("NEXT? (Options: R # + - S . T U W O F H J K L C ? X Q) ");
    printf("(? for Help) ");
#ifdef WIN32
    phyFillScreenColor();
#endif
    scanf("%c%*[^\n]", &ch);
    getchar();
    uppercase(&ch); 
    if (strchr("HJKLCFORSTUXQ+#-.W?",ch) != NULL){
      switch (ch) {
        
      case 'R':
        rearrange();
        break;
        
      case '#':
        dnamove_nextinc();
        break;
        
      case '+':
        dnamove_nextchar();
        break;
        
      case '-':
        dnamove_prevchar();
        break;
        
      case 'S':
        dnamove_show();
        break;
        
      case '.':
        dnamove_printree();
        break;
        
      case 'T':
        try();
        break;
        
      case 'U':
        undo();
        break;
        
      case 'W':
        treewrite(done);
        break;
        
      case 'O':
        changeoutgroup();
        break;
        
      case 'F':
        flip();
        break;
        
      case 'H':
        window(left, &leftedge, &topedge, hscroll, vscroll, treelines,
                 screenlines, screenwidth, farthest, subtree);
        dnamove_printree();
        break;

      case 'J':
        window(downn, &leftedge, &topedge, hscroll, vscroll, treelines,
                 screenlines, screenwidth, farthest, subtree);
        dnamove_printree();
        break;

      case 'K':
        window(upp, &leftedge, &topedge, hscroll, vscroll, treelines,
                 screenlines, screenwidth, farthest, subtree);
        dnamove_printree();
        break;

      case 'L':
        window(right, &leftedge, &topedge, hscroll, vscroll, treelines,
                 screenlines, screenwidth, farthest, subtree);
        dnamove_printree();
        break;

      case 'C':
        clade();
        break;
        
      case '?':
        help("site");
        dnamove_printree();
        break;
        
      case 'X':
        done = true;
        break;
        
      case 'Q':
        done = true;
        break;
      }
    }
  } while (!done);
  if (written)
    return;
  do {
    printf("Do you want to write out the tree to a file? (Y or N) ");
#ifdef WIN32
    phyFillScreenColor();
#endif
    scanf("%c%*[^\n]", &ch);
    getchar();
    if (ch == 'Y' || ch == 'y')
      treewrite(done);
  } while (ch != 'Y' && ch != 'y' && ch != 'N' && ch != 'n');
}  /* redisplay */


void treeconstruct()
{
  /* constructs a binary tree from the pointers in treenode. */

  restoring = false;
  subtree = false;
  display = false;
  dispchar = 0;
  earlytree = true;
  waswritten = false;
  buildtree();
  printf("\nComputing steps needed for compatibility in sites ...\n\n");
  setorder();
  mincomp();
  newtree = true;
  earlytree = false;
  dnamove_printree();
  bestyet = -like;
  gotlike = -like;
  lastop = none;
  newtree = false;
  written = false;
  redisplay();
}  /* treeconstruct */


int main(int argc, Char *argv[])
{ /* Interactive DNA parsimony */
  /* reads in spp, chars, and the data. Then calls treeconstruct to
     construct the tree and query the user */
#ifdef MAC
  argc = 1;                /* macsetup("Dnamove","");        */
  argv[0] = "Dnamove";
#endif
  init(argc, argv);
  emboss_getoptions("fdnamove",argc,argv);
  progname = argv[0];
  /*
  strcpy(infilename,INFILE);
  strcpy(intreename,INTREE);
  strcpy(outtreename,OUTTREE);
*/
  /*openfile(&infile,infilename,"input file", "r",argv[0],infilename);*/
  embossouttree = ajAcdGetOutfile("outtreefile");
  emboss_openfile(embossouttree,&outtree,&outtreename);

  garbage = NULL;
  screenlines = 24;
  scrollinc = 20;
  screenwidth = 80;
  topedge = 1;
  leftedge = 1;
  ibmpc = IBMCRT;
  ansi = ANSICRT;
  doinput();
  configure();
  treeconstruct();
  FClose(infile);
  FClose(outtree);
#ifdef MAC
  fixmacfile(outtreename);
#endif
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  ajExit();
  return 0;
}  /* Interactive DNA parsimony */
