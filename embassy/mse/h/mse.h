/*  Last edited: Sep 30 10:49 1999 (pmr) */
/****  MSE include file  ******************************************************
**
**  MSE, Multiple Sequence Editor
**
**  William Gilbert, Copyright (c) Whitehead Insitute
**
******************************************************************************/
#include "boolean.h"
#include "seqspec.h"
#include "seqentry.h"
/*#include "smgmacros.h"*/
#include "macros.h"
#include "ckit.h"
#include <curses.h>
/*---------------------------------------*/
/* Match "Curses" attributes
*/

#define _NORMAL    A_NORMAL
#define _BOLD      A_BOLD   
#define _UNDERLINE A_UNDERLINE
#define _BLINK     A_BLINK
#define _REVERSE   A_REVERSE
#define setattr    attrset
#define clrtocol(x)   printf("%d\n",x);
/*#define GetDCLSymbol(x,y) y=getenv(x)*/

int __Cols=80, __Rows=28;
int MARK1,MARK2,TEXTMARK,NROWS;
WINDOW *Display;

typedef struct MSEinternal MSEinternal;
struct MSEinternal {
     char* Code;      /* Code word, if used by the foramt               */
     char* File;      /* File with pathname or database name            */
  Boolean IsUser;         /* TRUE if this is a user-entry sequence          */
      int Format;         /* Sequence format for user-entry file            */
     char* Name;      /* Sequence Name, varies with format              */
     char* Title;     /* Sequence title, may be blank for some formats  */
     char* Desc;      /* Sequence description                           */
      int Type;           /* Sequence type, Protein, DNA, RNA, etc,         */
  Boolean Circular;       /* "true" if this is a circular molecule          */
     long Length;         /* Length of sequence                             */
     char *Mem;           /* Memory pointer to start of strand,used by cfree*/
      int Size;           /* Size of memory block                           */
     char *Strand;        /* Pointer to *Mem-1 such that Strand[1] = &Mem   */
      int ReMap;          /* How to remap the keystrokes to characters      */
      int Anchored;       /* Is the sequence part of an anchored group?     */
     long Offset;         /* Offset for each strand from "Absolute Left     */
  Boolean Modified;       /* Has the sequence been modified?                */
  Boolean Reversed;       /* Has the strand been reversed?                  */
  Boolean Locked;         /* Is the strand lock against changes             */
};


#define NOS   250            /* Number of Strands, use 0 for temp work */

MSEinternal Seq[NOS+1];

char FOSSName[128];          /* File of Sequence Specs name */

char DefaultDB[]="Nucleic";  /* Default database for codewords */

char IncSeq[10000];          /* Use by cut and paste */
char FindSeq[256];           /* Target search pattern, remember it */

unsigned char CharMap[2][128]=   /* Remap keystrokes to nucleic acid symbols */
                        /* Fill the PLAIN array with IUPAC symbols  */
{
{255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
 255,255,255,255,255,255,255,255,255,255,255,255,255,'-',255,255,
 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
 255,'A','B','C','D','E','F','G','H','I',255,'K','L','M','N',255,
 'P','Q','R','S','T','U','V','W','X','Y','Z',255,255,255,255,255,
 255,'a','b','c','d','e','f','g','h','i',255,'k','l','m','n',255,
 'p','q','r','s','t','u','v','w','x','y','z',255,255,255,255,127 },

{255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
 255,255,255,255,255,255,255,255,255,255,255,255,255,'-',255,255,
 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
 255,'A',255,'C',255,255,255,'G',255,255,255,255,255,255,'N',255,
 255,255,255,255,'T',255,255,255,'N',255,255,255,255,255,255,255,
 255,'a',255,'c',255,255,255,'g',255,255,255,255,255,255,'N',255,
 255,255,255,255,'t',255,255,255,'N',255,255,255,255,255,255,127 }
};

long LinePos,            /* Position from "Absolute Left"              */
     SeqPos;             /* Current symbol number in current strand    */

int  MatchAttr,          /* Attribute to display symbols matching consensus   */
     DiffAttr;           /* Attribute to display symbols differing from cons. */

int  BlockSize=10,       /* Defaults for writing out Sequence or hardcopy */
     LineSize=50,
     PageSize=63;

long ScaleLen,          /* Used to draw number scale and test out of bounds */
     ScrSeqLen,
     ScrEdge,
     ScrScaleLen,
     ScrBarLen;

int  Strand,              /* Strand number of the current strand              */
     SelectStr;           /* Number of the strand if Selection is on          */
long SelectPos;           /* Position in the select stran where select starts */

int  Threshold;           /* Threshold below which to not count plurality     */

int  DefFormat;           /* The format when using user-entry files          */

Boolean Select,           /* Is the "Select" function on or off              */
        Dirty;            /* If TRUE the error message are has text in it    */


Boolean OkToEdit[NOS+1],  /* Is there a sequence on this line?           */
        ReDoSeq[NOS+1],   /* During Update, refresh this strand?         */
        ReDoBar[NOS+1];   /* During Update, refresh this bar?            */

Boolean DoPlurality,      /* Show diff/matches from the plurality seq.   */
        unique;         /* Hilight any symbols in line 1 not found elsewhere*/

/* Parameters */

#define NOADDR  10000000  /* Used to signal a default condition */

/* Command definitions, used to talk between modules */

enum keydef {
 CNULL,CGO,CFIND,CHELP,CWRITE,CEXIT,CEDIT,CCREAD,CDELETE,CCHECK,CREDRAW,CCQUIT,
 CSEQ,CCOM,CFORMAT,CTEXT,CREVERSE,CDNA,CRNA,CNAME,CTITLE,CTYPE,CSELECT,CREMOVE,
 CINSERT,CCANCEL,CMOVE,CELIMINATE,CCOPY,COFFSET,CMATCHES,CDIFFERENCES,CNEITHER, 
 CHARDCOPY,CANCHOR,CNOANCHOR,CSORT,CLOCK,CUNLOCK,COPEN,CMSF,CCONSENSUS,CUPPER,
 CLOWER,CDEGAP,CMELD,CALNED,CGELALIGN,CUNIQUE,CCREATE,CSAVE
};


#define BELL      '\007'   /* Terminal bell            */
#define BACKSPACE '\010'   /* Backspace or Control-H   */
#define CNTRLL    '\014'   /* Control-L i.e. Form Feed */
#define CARRET    '\n'   /* Carriage return          */
#define CNTRLE    '\005'   /* Control-E, go to EOL     */
#define CNTRLR    '\022'   /* Control-R, repaint       */
#define CNTRLW    '\027'   /* Control-W, repaint       */
#define CNTRLZ    '\032'   /* Control-Z, command mode  */
#define SPACE     '\040'   /* Space character          */
#define DEL       '\177'   /* Delete character         */
#define GAP       '-'      /* Gap character            */

/*
** Definition for remapping keys
*/

enum {PLAIN,             /* Do no character remapping i.e. Proteins */
      XFORM};            /* Remap keys according to "SeqEdInit.TXT  */


/*
** Fuction declarations and prototypes for MSE.
*/

void MSEInit(void);
void DoScreen(void);
void UpDate(void);
int  GoodScaleLen(int Length);
void DrawBar(int Row, int Col, int Length, Boolean Protein, Boolean Reversed);
int  GetCmd(int *Start, int *Finish, char *ComStr, char *ArgStr);
void EdInit(int Start);
void DoCreate(int Start, char *ArgStr);
void DoDelete(int Start, int Finish);
void DoDegap(int Start, int Finish);
void DoReverse(int Start);
void DoDNA(int Start);
void DoRNA(int Start);
void DoLower(int Start, int Finish);
void DoUpper(int Start, int Finish);
void DoSelect(int Start, int Finish);
void DoRemove(void);
void DoInsert(int Start);
void DoCancel(void);
void DoMatches(char *ArgStr);
void DoDifferences(char *ArgStr);
void DoUnique(char *ArgStr);
void DoNeither(void);
void DoSort(int Start, int Finish, char *ArgStr);
void DoAnchor(int Start, int Finish);
void DoNoAnchor(int Start, int Finish);
void DoLock(int Start, int Finish);
void DoUnLock(int Start, int Finish);
void DoQuit(void);
void DoSeq(void);
void DoFormat(char *ArgStr);
void DoType(int Start, int Finish, char *ArgStr);
void DoGelAlign(void);
void GelFind(int Frag, int Tuple, int Shift);
Boolean NextPool(int *Frag);
void DoMeld(int Start, int Finish, char *ArgStr);
void DoWrite(int Start, int Finish, char *FName);
void DoExit(char *FName);
int CmdMatch(char *cmdstr, char *string);
int InsertSymbol(int Line, int Pos, char Base);
void DeleteSymbol(int Line, int Pos);
void DoEliminate(int Start, int Finish);
void DoCopy(int Start, int Finish);
void DoMove(int Start, int Finish);
void DoOpen(void);
void DoOffset(int Start);
void DoEdit(char *InLine, int Start);
void DoFOSS(char *FOSSName);
void DoAlned(char *ArgStr);
void DoName(char *ArgStr);
void DoTitle(char *InLine);
void DoMSF(char *ArgStr);
void DoHardcopy(int Start, int Finish, char *InLine);
void Plurality(int From, int To, char *PlurSeq);
void ReadInit(void);
void DoFind(int Start, int Finish, char *Pattern);
void DoHelp(int Start);
void DoRedraw(void);
void CleanUp(void);
void ShowText(char *OneLine);
void DrawScale(int Row, int Col, int Width, int Divs, int Offset);
void NumberScale(int Row, int Col, int Width, int Divs, int Offset, int Start, int Step);
SeqSpec *Get_SeqSpec(char *Line, SeqSpec *Spec, char *Prompt, char *DCLSymbol);
int Get_Integer(char *Prompt, int Val, int Low, int High);
void MakeScreen(void);
void ShowError(char *errMsg);
void ToInternal(int strand, SeqEntry *SeqIn);
void FromInternal(int strand, SeqEntry *SeqOut);
/* End of Include File for MSE */
