
#include "phylip.h"

/* version 3.6. (c) Copyright 1988-2002 by the University of Washington.
   A program to factor multistate character trees.
   Originally version 29 May 1983 by C. A. Meacham, Botany Department,
     University of Georgia
   Additional code by Joe Felsenstein, 1988-1991
   C version code by Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */

#define maxstates       20   /* Maximum number of states in multi chars      */
#define maxoutput       80   /* Maximum length of output line                */
#define sizearray       5000 /* Size of symbarray; must be >= the sum of     */
                             /* squares of the number of states in each multi*/
                             /* char to be factored                          */
#define factchar        ':'  /* character to indicate state connections      */
#define unkchar         '?'  /* input character to indicate state unknown    */

typedef struct statenode {     /* Node of multifurcating tree */
  struct statenode *ancstr, *sibling, *descendant;
  Char state;             /* Symbol of character state   */
  long edge;             /* Number of subtending edge   */
} statenode;

AjPFile inputfile;
AjPStr rdline = NULL;

#ifndef OLDC
/* function prototypes */
/*void   getoptions(void);*/
void emboss_getoptions(char *pgm, int argc, char *argv[]);
void   nextch(Char *ch);
void   readtree(void);
void   attachnodes(statenode *, Char *);
void   maketree(statenode *, Char *);
void   construct(void);
void   numberedges(statenode *, long *);
void   factortree(void);
void   dotrees(void);
void   writech(Char ch, long *);
void   writefactors(long *);
void   writeancestor(long *);
void   doeu(long *, long);
void   dodatamatrix(void);
/* function prototypes */
#endif


Char infilename[FNMLNGTH];
const char* outfilename;
long neus, nchars, charindex, lastindex;
Char ch;
boolean ancstrrequest, factorrequest, rooted, progress;
Char symbarray[sizearray];
 /* Holds multi symbols and their factored equivs        */
long *charnum;     /* Multis           */
long *chstart;     /* Position of each */
long *numstates;   /* Number of states */
Char  *ancsymbol;   /* Ancestral state  */

/*  local variables for dotrees, propagated to global level. */
  long npairs, offset, charnumber, nstates;
  statenode *root;
  Char pair[maxstates][2];
  statenode *nodes[maxstates];



/************ EMBOSS GET OPTIONS ROUTINES ******************************/

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjStatus retval;
 
    /* initialize global variables */

    ajNamInit("emboss");
    retval =  ajAcdInitP (pgm, argc, argv, "PHYLIP");

    /* ajAcdGet */

    inputfile = ajAcdGetInfile("infile");


    /* init functions for standard ajAcdGet */

    /* cleanup for clashing options */

}

/************ END EMBOSS GET OPTIONS ROUTINES **************************/

/*
//void getoptions()
//{
//  /# interactively set options #/
//  Char ch;
//
//  ibmpc = IBMCRT;
//  ansi = ANSICRT;
//  progress = true;
//  factorrequest = false;
//  ancstrrequest = false;
//  putchar('\n');
//  for (;;){
//    printf(ansi ? "\033[2J\033[H" : "\n");
//    printf("\nFactor -- multistate to binary recoding program, version %s\n\n"
//           ,VERSION);
//    printf("Settings for this run:\n");
//    printf("  A      put ancestral states in output file?  %s\n",
//           ancstrrequest ? "Yes" : "No");
//    printf("  F   put factors information in output file?  %s\n",
//           factorrequest ? "Yes" : "No");
//    printf("  0       Terminal type (IBM PC, ANSI, none)?  %s\n",
//           ibmpc ? "IBM PC" : ansi  ? "ANSI" : "(none)");
//    printf("  1      Print indications of progress of run  %s\n",
//           (progress ? "Yes" : "No"));
//    printf("\nAre these settings correct? (type Y or the letter for one to change)\n");
//#ifdef WIN32
//    phyFillScreenColor();
//#endif
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    uppercase(&ch);
//    if (ch == 'Y')
//      break;
//    if (strchr("AF01", ch) != NULL) {
//      switch (ch) {
//        
//      case 'A':
//        ancstrrequest = !ancstrrequest;
//        break;
//        
//      case 'F':
//        factorrequest = !factorrequest;
//        break;
//        
//      case '0':
//        initterminal(&ibmpc, &ansi);
//        break;
//
//      case '1':
//        progress = !progress;
//      break;
//      }
//    } else
//      printf("Not a possible option!\n");
//  }
//}  /# getoptions #/
*/


/*
//void nextch(Char *ch)
//{
//  *ch = ' ';
//  while (*ch == ' ' && !eoln(infile))
//    *ch = gettc(infile);
//}  /# nextch #/
*/

void readtree()
{
  /* Reads a single character-state tree; puts adjacent symbol
     pairs into array 'pairs' */

  char* cp;
  cp = ajStrStr(rdline);
  while (*cp && isspace(*cp))
      cp++;
  while (*cp && isdigit(*cp))
      cp++;

  npairs = 0;
  while (*cp) {
    
      while (*cp && isspace(*cp))
	  cp++;
      ch = *cp++;
    npairs++;
    pair[npairs - 1][0] = ch;
    ch = *cp++;
    if (!*cp || (ch != factchar)) {
      printf("\n\nERROR: Character %d:  bad character state tree format\n\n",
             cp - ajStrStr(rdline));
      exxit(-1);}

    pair[npairs - 1][1] = *cp++;
    if (pair[npairs - 1][1] == ' '){
      printf("\n\nERROR: Character %d:  bad character state tree format\n\n",
             cp - ajStrStr(rdline));
      exxit(-1);}
  }
}  /* readtree */


void attachnodes(statenode *poynter, Char *otherone)
{
  /* Makes linked list of all nodes to which passed node is
     ancestral.  First such node is 'descendant'; second
     such node is 'sibling' of first; third such node is
     sibling of second; etc.  */
  statenode *linker, *ptr;
  long i, j, k;

  linker = poynter;
  for (i = 0; i < (npairs); i++) {
    for (j = 1; j <= 2; j++) {
      if (poynter->state == pair[i][j - 1]) {
            if (j == 1)
              *otherone = pair[i][1];
            else
              *otherone = pair[i][0];
            if (*otherone != '.' && *otherone != poynter->ancstr->state) {
              k = offset + 1;
              while (*otherone != symbarray[k - 1])
                k++;
              if (nodes[k - offset - 1] != NULL)
                exxit(-1);
              ptr = (statenode *)Malloc(sizeof(statenode));
              ptr->ancstr = poynter;
              ptr->descendant = NULL;
              ptr->sibling = NULL;
              ptr->state = *otherone;
              if (linker == poynter)   /* If not first */
                poynter->descendant = ptr;   /* If first */
              else
                linker->sibling = ptr;
              nodes[k - offset - 1] = ptr;
              /* Save pntr to node */
              linker = ptr;
            }
      }
    }
  }
}  /* attachnodes */


void maketree(statenode *poynter, Char *otherone)
{
  /* Recursively attach nodes */
  if (poynter == NULL)
    return;
  attachnodes(poynter, otherone);
  maketree(poynter->descendant, otherone);
  maketree(poynter->sibling, otherone);
}  /* maketree */


void construct()
{
  /* Puts tree together from array 'pairs' */
  Char rootstate;
  long i, j, k;
  boolean done;
  statenode *poynter;
  char otherone;

  rooted = false;
  ancsymbol[charindex - 1] = '?';
  rootstate = pair[0][0];
  nstates = 0;
  for (i = 0; i < (npairs); i++) {
    for (j = 1; j <= 2; j++) {
      k = 1;
      done = false;
      while (!done) {
        if (k > nstates) {
          done = true;
          break;
        }
        if (pair[i][j - 1] == symbarray[offset + k - 1])
          done = true;
        else
          k++;
      }
      if (k > nstates) {
        if (pair[i][j - 1] == '.') {
          if (rooted)
            exxit(-1);
          rooted = true;
          ancsymbol[charindex - 1] = '0';
          if (j == 1)
            rootstate = pair[i][1];
          else
            rootstate = pair[i][0];
        } else {
          nstates++;
          symbarray[offset + nstates - 1] = pair[i][j - 1];
        }
      }
    }
  }
  if ((rooted && nstates != npairs) ||
      (!rooted && nstates != npairs + 1))
    exxit(-1);
  root = (statenode *)Malloc(sizeof(statenode));
  root->state = ' ';
  root->descendant = (statenode *)Malloc(sizeof(statenode));
  root->descendant->ancstr = root;
  root = root->descendant;
  root->descendant = NULL;
  root->sibling = NULL;
  root->state = rootstate;
  for (i = 0; i < (nstates); i++)
    nodes[i] = NULL;
  i = 1;
  while (symbarray[offset + i - 1] != rootstate)
    i++;
  nodes[i - 1] = root;
  maketree(root, &otherone);
  for (i = 0; i < (nstates); i++) {
    if (nodes[i] != root) {
      if (nodes[i] == NULL){
        printf(
        "\n\nERROR: Character %ld: invalid character state tree description\n",
               charnumber);
        exxit(-1);}
      else {
        poynter = nodes[i]->ancstr;
        while (poynter != root && poynter != nodes[i])
          poynter = poynter->ancstr;
        if (poynter != root){
          printf(
          "ERROR: Character %ld: invalid character state tree description\n\n",
                 charnumber);
          exxit(-1);}
      }
    }
  }
}  /* construct */


void numberedges(statenode *poynter, long *edgenum)
{
  /* Assign to each node a number for the edge below it.
     The root is zero */
  if (poynter == NULL)
    return;
  poynter->edge = *edgenum;
  (*edgenum)++;
  numberedges(poynter->descendant, edgenum);
  numberedges(poynter->sibling, edgenum);
}  /* numberedges */


void factortree()
{
  /* Generate the string of 0's and 1's that will be
     substituted for each symbol of the multistate char. */
  long i, j, place, factoroffset;
  statenode *poynter;
  long edgenum=0;

  numberedges(root, &edgenum);
  factoroffset = offset + nstates;
  for (i = 0; i < (nstates); i++) {
    place = factoroffset + (nstates - 1) * i;
    for (j = place; j <= (place + nstates - 2); j++)
      symbarray[j] = '0';
    poynter = nodes[i];
    while (poynter != root) {
      symbarray[place + poynter->edge - 1] = '1';
      poynter = poynter->ancstr;
    }
  }
}  /* factortree */


void dotrees()
{
  /* Process character-state trees */
  long lastchar;
  ajint ival;

  charindex = 0;
  lastchar = 0;
  offset = 0;
  charnumber = 0;
  ajFileGetsTrim(inputfile, &rdline);
  ajFmtScanS(rdline, "%d", &ival);
  charnumber = ival;
  while (charnumber < 999) {
    if (charnumber < lastchar) {
      printf("\n\nERROR: Character state tree");
      printf(" for character %ld: out of order\n\n", charnumber);
      exxit(-1);
    }
    charindex++;
    lastindex = charindex;
    readtree();   /* Process character-state tree  */
    if (npairs > 0) {
      construct();   /* Link tree together  */
      factortree();
    } else {
      nstates = 0;
      ancsymbol[charindex - 1] = '?';
    }
    lastchar = charnumber;
    charnum[charindex - 1] = charnumber;
    chstart[charindex - 1] = offset;
    numstates[charindex - 1] = nstates;
    offset += nstates * nstates;
    fscanf(infile, "%ld", &charnumber);
  }
  /*    each multistate character */
  /*    symbol  */
}  /* dotrees */


void writech(Char ch, long *chposition)
{
  /* Writes a single character to output */
  if (*chposition > maxoutput) {
    putc('\n', outfile);
    *chposition = 1;
  }
  putc(ch, outfile);
  (*chposition)++;
}  /* writech */


void writefactors(long *chposition)
{  /* Writes 'FACTORS' line */

  long i, charindex;
  Char symbol;

  fprintf(outfile, "FACTORS   ");
  *chposition = 11;
  symbol = '-';
  for (charindex = 0; charindex < (lastindex); charindex++) {
    if (symbol == '-')
      symbol = '+';
    else
      symbol = '-';
    if (numstates[charindex] == 0)
      writech(symbol, chposition);
    else {
      for (i = 1; i < (numstates[charindex]); i++)
        writech(symbol, chposition);
    }
  }
  putc('\n', outfile);
}  /* writefactors */


void writeancestor(long *chposition)
{
  /* Writes 'ANCESTOR' line */
  long i, charindex;

  charindex = 1;
  while (ancsymbol[charindex - 1] == '?')
    charindex++;
  if (charindex > lastindex)
    return;
  fprintf(outfile, "ANCESTOR  ");
  *chposition = 11;
  for (charindex = 0; charindex < (lastindex); charindex++) {
    if (numstates[charindex] == 0)
      writech(ancsymbol[charindex], chposition);
    else {
      for (i = 1; i < (numstates[charindex]); i++)
        writech(ancsymbol[charindex], chposition);
    }
  }
  putc('\n', outfile);
}  /* writeancestor */


void doeu(long *chposition, long eu)
{
  /* Writes factored data for a single species  */
  long i, charindex, place;
  Char *multichar;
  char* cp;

  cp = ajStrStr(rdline);
  
  for (i = 1; i <= nmlngth; i++) {
    ch = *cp++;
    putc(ch, outfile);
    if ((ch == '(') || (ch == ')') || (ch == ':')
        || (ch == ',') || (ch == ';') || (ch == '[')
        || (ch == ']')) {
      printf(
        "\n\nERROR: Species name may not contain characters ( ) : ; , [ ] \n");
      printf("       In name of species number %ld there is character %c\n\n",
              i+1, ch);
      exxit(-1);
    }
  }
  multichar = (Char *)Malloc(nchars*sizeof(Char));
  *chposition = 11;
  for (i = 0; i < (nchars); i++) {
    do {
      ch = *cp++;
      if (!*cp)
      {
	  ajFileGetsTrim(inputfile, &rdline);
	  cp = ajStrStr(rdline);
	  ch = *cp++;
      }
    } while (isspace(ch));
    multichar[i] = ch;
  }
  for (charindex = 0; charindex < (lastindex); charindex++) {
    if (numstates[charindex] == 0)
      writech(multichar[charnum[charindex] - 1], chposition);
    else {
      i = 1;
      while (symbarray[chstart[charindex] + i - 1] !=
             multichar[charnum[charindex] - 1] && i <= numstates[charindex])
        i++;
      if (i > numstates[charindex]) {
        if( multichar[charnum[charindex] - 1] == unkchar){
          for (i = 1; i < (numstates[charindex]); i++)
            writech('?', chposition);
        } else {
          putc('\n', outfile);
          printf("\n\nERROR: In species %ld, multistate character %ld:  ",
                 eu, charnum[charindex]);
          printf("'%c' is not a documented state\n\n",
                 multichar[charnum[charindex] - 1]);
          exxit(-1);
        }
      } else {
        place = chstart[charindex] + numstates[charindex] +
                (numstates[charindex] - 1) * (i - 1);
        for (i = 0; i <= (numstates[charindex] - 2); i++)
          writech(symbarray[place + i], chposition);
      }
    }
  }
  putc('\n', outfile);
  free(multichar);
}  /* doeu */


void dodatamatrix()
{
  /* Reads species information and write factored data set */
  long charindex, totalfactors, eu, chposition;

  totalfactors = 0;
  for (charindex = 0; charindex < (lastindex); charindex++) {
    if (numstates[charindex] == 0)
      totalfactors++;
    else
      totalfactors += numstates[charindex] - 1;
  }
  if (rooted && ancstrrequest)
    fprintf(outfile, "%5ld %4ld    A\n", neus + 1, totalfactors);
  else
    fprintf(outfile, "%5ld %4ld\n", neus, totalfactors);
  if (factorrequest)
    writefactors(&chposition);
  if (ancstrrequest)
    writeancestor(&chposition);
  eu = 1;
  while (eu <= neus) {
    eu++;
    doeu(&chposition, eu);
  }
  if (progress)
    printf("\nData matrix written on file \"%s\"\n\n", outfilename);
}  /* dodatamatrix */


int main(int argc, Char *argv[])
{
#ifdef MAC
  argc = 1;                /* macsetup("Factor","");        */
  argv[0] = "Factor";
#endif
  init(argc,argv);
  emboss_getoptions("ffactor",argc,argv);
  /*openfile(&infile,INFILE,"input file", "r",argv[0],infilename);*/
  embossoutfile = ajAcdGetOutfile("outfile");
  emboss_openfile(embossoutfile,&outfile,&outfilename);

/*  getoptions();*/
  ajFileGetsTrim(inputfile, &rdline);
  ajFmtScanS(rdline, "%ld%ld%*[^\n]", &neus, &nchars); /* ignore A - see ACD */
  charnum = (long *)Malloc(nchars*sizeof(long));
  chstart = (long *)Malloc(nchars*sizeof(long));
  numstates = (long *)Malloc(nchars*sizeof(long));
  ancsymbol = (Char *)Malloc(nchars*sizeof(Char));
  dotrees();   /* Read and factor character-state trees */
  dodatamatrix();
  ajFileClose(&inputfile);
  FClose(outfile);
#ifdef MAC
  fixmacfile(outfilename);
#endif
  /*printf("Done.\n\n");*/
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  ajExit();
  return 0;
}  /* factor */
