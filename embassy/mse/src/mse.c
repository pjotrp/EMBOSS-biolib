/**  MSE  *********************************************************************
**
**  A Multiple Sequence Screen Editor for Biological Sequences.
**  This is a copyrighted work.  (c) William A. Gilbert, Whitehead Insitutute
**
**  Modification History:
 * Last edited: May 16 09:14 2000 (pmr)
**  --------------------
**  February, 1986 Originally written in FORTRAN. (V1.0)
**  January 1987   Rewritten in VMS C using CKit library. (V2.0)
**                 Added GelAlign fragment assembly.
**  September 1989 Data Structures revised to match new CKit Library. (V2.1)
**  August 1990    Minor fixs to incorporate changes to CKit (V2.2)
**  October 1999   Now uses curses for Unix C (V3.0) (il@sanger.ac.uk)
**
*****************************************************************************/
#include "ajax.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "mse.h"         /* MSE global variables and definitions  */


/*FILE *debug;*/
int DEFAULTFORMAT;
/*AjPStr FORMATSTR=NULL;*/
AjPStr tmpdir = NULL;
char **envptr;


void LoadSeqWithseqset(AjPSeqset seqset){
  char *seq;
  int i;
  AjPStr tempname;

  DEFAULTFORMAT = seqset->Format;

  if(!DEFAULTFORMAT){
    DEFAULTFORMAT = 7;
  }

  strcpy(FOSSName,ajStrStr(seqset->Usa));
  ajDebug("FOSSName = %s \n",FOSSName);
  for(i=0;i<ajSeqsetSize(seqset);i++){
    seq = ajSeqsetSeq (seqset, i);
    Strand = i+1;

    OkToEdit[Strand] = true;

    Seq[Strand].Mem = CALLOC(strlen(seq)+1,char);
    strcpy(Seq[Strand].Mem,seq);
    Seq[Strand].Length = strlen(Seq[Strand].Mem);
    Seq[Strand].Size = strlen(seq)+1;
    Seq[Strand].Strand = Seq[Strand].Mem-1;

    if(ajSeqsetIsProt(seqset)){
      Seq[Strand].Type = PROTEIN;
      Seq[Strand].ReMap = PLAIN;
    }
    else {
      Seq[Strand].Type = DNA;
      Seq[Strand].ReMap = XFORM;
    }

    Seq[Strand].Circular = false;
    Seq[Strand].Format = DefFormat;

    tempname = ajSeqsetName(seqset,i);
    Seq[Strand].Name = CALLOC(ajStrLen(tempname)+1,char);
    strcpy(Seq[Strand].Name,ajStrStr(tempname));

    Seq[Strand].Code = CALLOC(1,char);
    strcpy(Seq[Strand].Code,"");
    Seq[Strand].File = CALLOC(1,char);
    strcpy(Seq[Strand].File,"");
    Seq[Strand].Title = CALLOC(1,char);
    strcpy(Seq[Strand].Title,"");
    Seq[Strand].Desc = CALLOC(1,char);
    strcpy(Seq[Strand].Desc,"");
    /*    Seq[Strand].ReMap = XFORM;*/
    Seq[Strand].Offset = 0;
    Seq[Strand].Modified = false;
    Seq[Strand].Reversed = false;
    Seq[Strand].Locked = false;
    Seq[Strand].Anchored = false;
  }
  OkToEdit[Strand+1] = false;
  Strand = 1;
}

void DoSave(char *FName)
{
  char OutLine[256], OutFName[256];
  int i=0;
  AjPSeq seqnew;
  AjPSeqout outseq;
  AjPStr ajname;
  
  if ( StrIsBlank(FName) ) {
    sprintf(OutLine,"Save as file [%s]",FOSSName);
    strcpy(OutFName,FOSSName);
    ShowText(OutLine);
    getstr(OutFName);
    if ( StrIsBlank(OutFName)) strcpy(OutFName,FOSSName);
  }
  else{
    strcpy(OutFName,FName);
  }    
  ajname = ajStrNewC(OutFName);
      
  outseq = ajSeqoutNew();
  outseq->Format = DEFAULTFORMAT;
  ajSeqFileNewOut(outseq, ajname);
  for(i=1;i<NOS && OkToEdit[i]; i++){
    seqnew = ajSeqNew();
    ajSeqAssNameC(seqnew,Seq[i].Name);
    ajSeqAssSeqC(seqnew,Seq[i].Mem);
    ajSeqAllWrite(outseq, seqnew);
    ajSeqDel(&seqnew);
  }
  
  sprintf(OutLine,"Sequences written to %s",OutFName);
  ShowText(OutLine);

}

void Initscr(){
  Display = initscr();
  nocbreak();
  nonl();
  intrflush(stdscr,TRUE);
  keypad(stdscr,TRUE);
  raw();
  /*  debug = fopen("./debug","w");*/
  __Cols=Display->_maxx;
  __Rows=Display->_maxy;
  if(__Cols == 0 || __Rows == 0){
    ajDebug("ERROR:Cols = %d Rows = %d\n",__Cols,__Rows);
    __Cols =30;
    __Rows = 80;
  }
  NROWS = (int)((__Rows-12)/2); 
  MARK1 = NROWS+3;
  MARK2 = (NROWS*2)+6;
  TEXTMARK = MARK2+4;
}

main(int argc, char *argv[], char **env)
{
char OneLine[132];
char ComStr[132], ArgStr[132];
int Com;
int Start=0, Finish=0;
AjPSeqset seqset;
/*
** Setup screen control package, keypad and MSE, check for in-line arguments
*/
        ajNamInit("emboss"); 
	ajAcdInitP("mse", argc, argv, "MSE");
  

	seqset = ajAcdGetSeqset("msf");

	tmpdir = ajAcdGetString("tmpdir");

	
	envptr = env;
	ajStrAppC(&tmpdir,"/"); /* does not matter if // at end */

	(void) Initscr();

	MSEInit();
	Notify((ProcPtr)ShowError,(ProcPtr)ShowError,(ProcPtr)ShowError);

	*OneLine = EOS;
	Start = 1;


	if ( argc > 1 )
	  strcpy(&OneLine[0],argv[1]);

	LoadSeqWithseqset(seqset);
/*
** Display one line help message then enter the "Screen" until "CTRL-Z" or
** "DO" are pressed then get the command from the user and use an "IF-THEN-
** ELSE" construct as a Case statement. When finished display one line help
** and go back to screen mode.
*/

Loop:
	ShowText("Press \"?\" for help");
	
	DoScreen();

	Com = GetCmd(&Start, &Finish, ComStr, ArgStr);

	switch (Com) {
	  case CSAVE:        DoSave(ArgStr);                    break;
	    /*	  case CALNED:       DoAlned(ArgStr);                   break;*/
	  case CANCHOR:      DoAnchor(Start, Finish);           break;
	  case CCANCEL:      DoCancel();                        break;
	  case CCREATE:      DoCreate(Start,ArgStr);            break;
	  case CDEGAP:       DoDegap(Start, Finish);            break;
	  case CDELETE:      DoDelete(Start, Finish);           break;
	  case CDIFFERENCES: DoDifferences(ArgStr);             break;
	  case CDNA:         DoDNA(Start);                      break;
	    /*	  case CEDIT:        DoEdit(ArgStr,Start);              break;*/
	  case CELIMINATE:   DoEliminate(Start, Finish);        break;
 	  case CEXIT:        DoExit(ArgStr);                    break;
	  case CFIND:        DoFind(Start, Finish, ArgStr);     break;
	  case CFORMAT:      DoFormat(ArgStr);                  break;
	  case CGO:          LinePos = Seq[Strand].Offset+Start;break;
	  case CGELALIGN:    DoGelAlign();                      break;
	  case CHARDCOPY:    DoHardcopy(Start, Finish, ArgStr); break;
	  case CHELP:        DoHelp( Start );                   break;
 	  case CINSERT:      DoInsert(Start);                   break;
  	  case CLOCK:        DoLock(Start,Finish);              break;
  	  case CLOWER:       DoLower(Start,Finish);             break;
	  case CMATCHES:     DoMatches(ArgStr);                 break;
	  case CMELD:        DoMeld(Start, Finish,ArgStr);      break;
	  case CMOVE:        DoMove(Start, Finish);             break;
	    /*	  case CMSF:         DoMSF(ArgStr);                     break;*/
	  case CNAME:        DoName(ArgStr);
	                     Seq[Strand].Modified = true;       break;
	  case CNEITHER:     DoNeither(); break;
	  case CNOANCHOR:    DoNoAnchor(Start, Finish);         break;
	  case COFFSET:      DoOffset(Start);                   break;
	  case COPEN:        DoOpen();                          break;
	  case CCQUIT:        DoQuit();                          break;
	  case CREDRAW:      DoRedraw();                        break;
	  case CREMOVE:      DoRemove();                        break;
	  case CREVERSE:     DoReverse(Start);                  break;
	  case CRNA:         DoRNA(Start);                      break;
	  case CSELECT:      DoSelect(Start,Finish);            break;
	  case CSEQ:         DoSeq();                           break;
	  case CSORT:        DoSort(Start, Finish, ArgStr);     break;
	  case CTITLE:       DoTitle(ArgStr);
	                     Seq[Strand].Modified = true;       break;
	  case CTYPE:        DoType(Start, Finish, ArgStr);     break;
	  case CUNLOCK:      DoUnLock(Start, Finish);           break;
	  case CUNIQUE:      DoUnique(ArgStr);                  break;
	  case CUPPER:       DoUpper(Start, Finish);            break;
	  case CWRITE:       DoWrite(Start, Finish, ArgStr);    break;
  	}

	UpDate();
	goto Loop;

}  /* End of MSE Main */

/******************************************************************************/
/******************************************************************************/
/****  MSEInit  ***************************************************************
**
**  performs the initialization for MSE, ed only once.
**
*****************************************************************************/

void MSEInit()

{
int i;
char OneLine[128];
char *temp;
/*----------------------------------------------------------------------
** Initialize each strand, make sure everything is set to NoAnchor
*/

 temp = getenv("MITSeqFormat");  
 if(temp)
   strcpy(OneLine,temp);
 if ( OneLine )
   strcpy(OneLine, "GCG");
 DefFormat = EncodeFormat(OneLine);
 if ( DefFormat == UNDEF ) DefFormat = GCG;

/* Initialize all strands */

	for ( i=0; i<=NOS; i++ )
	  OkToEdit[i] = false;
	  
	LinePos = 1;
	Strand = 1;
	FindSeq[0] = EOS;
	ScaleLen = 100;
	ScrSeqLen = 0;
	ScrEdge = 0;
	ScrScaleLen = 100;
	ScrBarLen = 0;

	DoNeither();

/* Draw the initial screen */

	MakeScreen();
	ReadInit();

}  /* End of MSEInit */

/** DoScreen  ***************************************************************
**
**  The main loop for MSE, this is an event-driven program.
**
**  There are two forms to most commands one is keyboard oriented i.e.
**  for VT100 type terminals, the other uses the VT220 editing keypad
**  i.e. DO, HELP, PREV SCREEN, etc.
**
***************************************************************************/

void DoScreen()

{

#define ADVANCE  1
#define BACKUP  -1

char OneLine[132];
int i;
unsigned int Keystroke;
static int Gold=false;
static int Dir=ADVANCE;
static int Rep=1;
static char LastChar=EOS;

/*------------------------------------------------*/
  
	Rep = 1;

	while ( true ) {

	  Strand = LIMIT(1, Strand, NOS);
	  if( LinePos < 1 ) LinePos = 1;
     	  Seq[Strand].Offset = AJMAX(0, Seq[Strand].Offset);
	  SeqPos = LinePos - Seq[Strand].Offset;

/* Update the screen */

    	  UpDate();

/* Get new character */

	  noecho();
    	  Keystroke = getch();
	  echo();
	  if( Dirty ) ShowError(" ");

/***************************************************************************/
/*****          Large case statement to determine action             *****/

/* Move the cursor with the arrow keys */

Parse:

	  switch ( Keystroke ) {

	    case KEY_RIGHT:
	      LinePos += Rep;
	      ReDoBar[Strand] = true;
	      Rep = 1;
	      break;

	    case KEY_LEFT:
	      LinePos -= Rep;
	      ReDoBar[Strand] = true;
	      Rep = 1;
	      break;

            case KEY_UP:
	      ReDoBar[Strand]=true;
	      Strand = LIMIT(1,Strand+Rep,NOS);
	      ReDoBar[Strand]=true;
	      Rep = 1;
	      break;

	    case KEY_DOWN:
	      ReDoBar[Strand]=true;
	      Strand = LIMIT(1,Strand-Rep,NOS);
	      ReDoBar[Strand]=true;
	      Rep = 1;
	      break;

	  case  KEY_F(17):
	      DoAnchor(Strand, LIMIT(1,Strand+(Rep-1),NOS));
	      Rep = 1;
	      break;

	    case KEY_F(18):
	      DoNoAnchor(Strand, LIMIT(1,Strand+(Rep-1),NOS));
	      Rep = 1;
	      break;

/* ... or move this strand to the left or right
*/
            case ' ':	/*  */
	      DoOffset(Seq[Strand].Offset+Rep);
	      LinePos += Rep;
	      Rep = 1;
	      break;

            case KEY_F(12):
     	    case KEY_BACKSPACE:
	      DoOffset(Seq[Strand].Offset-Rep);
	      LinePos -= Rep;
	      Rep = 1;
	      break;

/*
** ... or find a pattern in the current strand
*/
            case KEY_FIND:
     	    case '/':
              ShowText("Target Pattern: ");
              getstr(OneLine);
              if ( StrIsBlank(StrCollapse(OneLine)) )
	        strcpy(OneLine,FindSeq);
	      if ( StrIsBlank(OneLine) == false ) 
                DoFind( Strand, NOS, OneLine );
	      ShowText("");
	      Rep = 1;
	      break;

/*
** ... or start the Selecting of a region
*/
            case KEY_SELECT:
	      if ( OkToEdit[Strand] &&
	         LinePos>Seq[Strand].Offset &&
	         LinePos<=(Seq[Strand].Offset+Seq[Strand].Length)  ) {
	        SeqPos = LinePos - Seq[Strand].Offset;
	         DoSelect(SeqPos,SeqPos);
	      }
	      Rep = 1;
	      break;

/*
** ... or remove a selected region
*/

           case KEY_DC:
	     if ( OkToEdit[Strand] &&
	           LinePos>Seq[Strand].Offset &&
	           LinePos<=(Seq[Strand].Offset+Seq[Strand].Length+1)  ) {
	       SeqPos = LinePos - Seq[Strand].Offset;
               DoRemove();
	      }
	      Rep = 1;
	      break;

/*
** ... or insert a removed section at the current position
*/

            case KEY_IC:
	      for ( i=1; i<=Rep; i++)
                DoInsert( LinePos - Seq[Strand].Offset );
	      Rep = 1;
	      break;


/*
** ... or go to an absolute position with nnn<rtn>
*/

            case KEY_ENTER:
              if ( isdigit(LastChar) ) LinePos = Seq[Strand].Offset + Rep;
	      Rep = 1;
	      break;

/*
** ... or move to end of sequence 
*/
	    case '*':
	    case CNTRLE:
	      LinePos = Seq[Strand].Offset + Seq[Strand].Length + 1;
	      Rep = 1;
	      break;

/*
** ... or go to the right or left one screen-full
*/

            case KEY_NPAGE:
	    case '>':
	      LinePos += Rep*__Cols;
	      ReDoBar[Strand] = true;
	      Rep = 1;
	      break;

            case KEY_PPAGE:
	    case '<':
	      LinePos -= Rep*__Cols;
	      ReDoBar[Strand] = true;
	      Rep = 1;
	      break;
/*
** ... PF1, the EDT gold key
*/

	  case 'H' /*SMG$K_TRM_PF1*/:
	      Keystroke = getch();
	      Gold = true;
	      goto Parse;

/*
** ... or give Help; VT220 Help function, ? for VT100 (also EDT style)
*/

            case KEY_F(1) /*SMG$K_TRM_HELP*/:
	      /*	    case SMG$K_TRM_PF2:*/
	    case '?':
	      DoHelp( Rep);
	      Rep = 1;
	      break;

/*
** ... or toggle Anchor/UnAnchor for this strand
*/

	  case KEY_F(4) /*SMG$K_TRM_PF4*/:
	      for ( i=1; i<=Rep; i++) {
	        if ( Gold ) {
	          if ( Seq[Strand].Locked ) 
	            Seq[Strand].Locked = false;
	          else
	            Seq[Strand].Locked = OkToEdit[Strand];
	        } else {
	          if ( Seq[Strand].Anchored ) 
	            Seq[Strand].Anchored = false;
	          else
	            Seq[Strand].Anchored = OkToEdit[Strand];
	        }
	        ReDoBar[Strand] = true;
	        if (i !=Rep ) {
	         Strand = (Dir == ADVANCE) ?  Strand+1 : Strand-1;
	         Strand = LIMIT(1,Strand,NOS);
	        }
    	      }
	      Rep=1;
	      break;

/*
** ... Keypad 0 is EDT "Beginning of line"
*/

	  case KEY_BEG/*SMG$K_TRM_KP0*/:
	      ReDoBar[Strand] = true;
	      if ( LinePos > Seq[Strand].Offset+1 )
	        Strand = (Dir == ADVANCE) ?  Strand   : Strand-1  ;
	      else if (LinePos <= Seq[Strand].Offset)
	        Strand = (Dir == ADVANCE) ?  Strand+1 : Strand ;
	      Strand = LIMIT(1,Strand+(Rep*-Dir),NOS);
	      LinePos = Seq[Strand].Offset + 1;
	      ReDoBar[Strand] = true;
	      Rep=1;
	      break;

/*
** ... Keypad 1 is EDT "Next word", for sequences use 10
*/

	  case KEY_C1/*SMG$K_TRM_KP1*/:
	      ReDoBar[Strand] = true;
	      LinePos += 10*Rep*Dir;
	      Rep=1;
	      break;

/*
** ... Keypad 2 is EDT "End of line"
*/

	    case '@':
	      ReDoBar[Strand] = true;
	      if ( LinePos > Seq[Strand].Offset+Seq[Strand].Length+1 )
	        Strand = (Dir == ADVANCE) ?  Strand   : Strand-1  ;
	      else if (LinePos <= Seq[Strand].Offset+Seq[Strand].Length)
	        Strand = (Dir == ADVANCE) ?  Strand+1 : Strand ;
	      Strand = LIMIT(1,Strand+(Rep*-Dir),NOS);
	      LinePos = Seq[Strand].Offset + Seq[Strand].Length + 1;
	      ReDoBar[Strand] = true;
	      Rep=1;
	      break;

/*
** ... Keypad 3 is EDT "Next Char"
*/

	  case KEY_C3/*SMG$K_TRM_KP3*/:
	      ReDoBar[Strand] = true;
	      LinePos += 1*Dir;
	      Rep=1;
	      break;

/*
** ... Keypad 4 is EDT "Advance mode"
*/

	  case '#' /*SMG$K_TRM_KP4*/:
	      if ( Gold ) {
	        for ( Strand=1; OkToEdit[Strand] == false && Strand < NOS; )
	          Strand++;
	        ReDoBar[Strand]=true;
	      } else
	        Dir = ADVANCE;
	      Rep=1;
	      break;

/*
** ... Keypad 5 is EDT "Backup mode"
*/

	    case KEY_B2:
	      if ( Gold ) {
	        for ( Strand=NOS; OkToEdit[Strand] == false && Strand > 1; )
	          Strand--;
	        ReDoBar[Strand]=true;
	      } else
	        Dir = BACKUP;
	      Rep=1;
	      break;

/*
** ... Keypad 6 is EDT "Cut/Paste"
*/

	  case '=' /*SMG$K_TRM_KP6*/:
	    if ( Gold )
	      	Keystroke = KEY_IC;
	      else
	        Keystroke = KEY_DC;
	      goto Parse;

/*
** ... Keypad 7 is EDT "Page", in MSE do Left right shifting
*/

	  case KEY_A1/*SMG$K_TRM_KP7*/:
	      if ( Gold ) return;
	      if ( Dir == ADVANCE )
	        Keystroke = KEY_NPAGE;
	      else
	        Keystroke = KEY_PPAGE;
	      goto Parse;

/*
** ... Keypad 8 is EDT "Section", in MSE do up down shifting.  This is
**     a more natural extention of EDT behavior.  As "Page" is not used
**     that extensively in EDT and angle brackets are already familiar.
*/

	  case '%'/*SMG$K_TRM_KP8*/:
	      ReDoBar[Strand] = true;
	      Strand = LIMIT(1,Strand+(-1*7*Rep*Dir),NOS);
	      ReDoBar[Strand] = true;
	      Rep=1;
	      break;

/*
** ... Keypad "." is EDT "Select/Cancel"
*/

	  case KEY_CANCEL /*SMG$K_TRM_PERIOD*/:
	      if ( Gold )
	        DoCancel();
	      else {
	        Keystroke = KEY_SELECT;
	        goto Parse;
	      }
	      Rep = 1;
	      break;


/*
** ... Keypad "," is EDT "Stationary delete"
*/
	  case ',' /*SMG$K_TRM_COMMA*/:
	      if ( Gold )
	        ShowError("Sorry, no UnDelete Character yet");
	      else
	        for ( i=1; i<=Rep; i++)
	          if ( LinePos >= 1)
	            DeleteSymbol(Strand, SeqPos);
	      Rep = 1;
	      break;

/*
** ... or tell the use a little about the current strand i.e. format, type
**     circular, length, current position and checksum.
*/

            case '!':
	      DoSeq();
	      Rep = 1;
	      break;

/*
** ... or redraw the whole screen
*/

	    case CNTRLW:
	    case CNTRLR:
	      /*clear();*/
	      for(i=0;i<=NROWS;i++){
		ReDoSeq[i] = true;
		ReDoBar[i]= true;
	      }

	      ReDoBar[Strand] = true;
	      DoRedraw();
	      Rep = 1;
	      break;

/*
** ... go to command level, VT200 "Do" key, EDT-like PF1 or Cntrl-Z
*/

	      /*case SMG$K_TRM_DO:*/
	    case CNTRLZ:
	      return;

/*
** ... user typed a digit, accumulate it for later use
*/
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
              if ( isdigit(LastChar) == false ) Rep = 0;
	      Rep = 10*Rep + (Keystroke - '0');
	      sprintf(OneLine," %d",Rep);
	      ShowError(OneLine);
	      break;	    

	    default:

/* Anything else ...
**
**  Insert a sequence character at the current cursor position. If this
**  strand is a protein sequence don't remap the keyboard.  Call "InsertSymbol"
**  which will handle anchored groups and do memory resizing if necessary
*/
 
	      if ( Keystroke > 127 ) break;
	      ajDebug("before key = %d, after key = ",Keystroke);
	      Keystroke = CharMap[ Seq[Strand].ReMap ][ Keystroke ];
	      ajDebug("%d Rep=%d\n",Keystroke,Rep);


	      if ( Keystroke < 127 ) {
	        for ( i=1; i<=Rep; i++) {
                  if( InsertSymbol(Strand, SeqPos, Keystroke)){
		    SeqPos++;
		    printw("%c",(char)Keystroke);
		    refresh();
	            ajDebug("addch %c\n",Keystroke);
		  }
                }
	        LinePos = Seq[Strand].Offset + SeqPos;
	        Rep = 1;

/* ... or delete a base with the delete or backspace keys from either the
**     sequence for check sequence. Keep the terminal buffer flushed of
**     extra delete characters, user may over-run
*/

	      } else if ( Keystroke == DEL ) {
	        if ( Rep > 1 ) {
	          Rep /= 10;
	          sprintf(OneLine," %d",Rep);
	          ShowError(OneLine);
	        } else {
	        for ( i=1; i<=Rep; i++) {
	          if ( LinePos > 1) {
	            SeqPos--;
	            LinePos--;
	            DeleteSymbol(Strand, SeqPos);
	          }
	        }
	        }
	        Rep = 1;
	      }
	  }
	  LastChar = Keystroke;
	  Gold = false;
	}

} /* End of DoScreen */

/**  UpDate  ****************************************************************
**
**	Updates and refreshes the screen consistent with SeqLen, Seq, 
**	SeqPos, etc., using its current ideas of the screen ScrSeqLen, 
**	ScrEdge, ScrScaleLen, and ScrBarLen
**
*****************************************************************************/
void UpDate()

{
char OneNum[5];
static char Consensus[255];
int Temp=0;
int i=0, ii=0, NewEdge=0, MaxPos=0, MaxLen=0;
int NScroll=0;
int ScrBarPos=0, SeqBarLen=0, SeqBarOffset=0;
int Dir=0;
static int LastSeq=1, BotStrand=1;

Boolean ReDoSeqs, Scroll;


 ajDebug("Update called\n"); 
 ajDebug("%d %d\n",__Cols,__Rows);
 ajDebug("NROWS = %d BotStrand = %d\n",NROWS,BotStrand);
/*-----------------------------------------------------------------*/
/* (1) See if we are within 5 symbols of the edge of the screen    */

	if ( LinePos < ScrEdge+5 )
	  NewEdge = AJMAX(0, 10*((LinePos-(__Cols-20))/10) );
	else if ( LinePos > ScrEdge+(__Cols-10) )
	    NewEdge = 10*((LinePos-5)/10);
	else
	    NewEdge = ScrEdge;

/* (2) If close to the edge, readjust number line and redraw sequences */

	if ( NewEdge != ScrEdge ) {
	  NumberScale(MARK1+2, 0, __Cols, 10, 4, NewEdge, 10);
	  ScrEdge = NewEdge;
	  for ( i=BotStrand; i<=NROWS/*BotStrand+6*/; i++)
	    ReDoSeq[i] = true;
	}

/* (3) Generate consensus sequence based on plurality */

	ReDoSeqs = false;
	for ( i=BotStrand; i<=/*BotStrand+6*/NROWS; i++)
	  ReDoSeqs |= ReDoSeq[i];
	
	if ( ReDoSeqs && DoPlurality ) {
	  Plurality(ScrEdge-4, ScrEdge+__Cols-5, Consensus);

	  setattr(_REVERSE);
	  mvaddstr(2,0,Consensus);
	  setattr(_NORMAL);
	  refresh();
	  
	  for (i=BotStrand; i<BotStrand+NROWS; i++)
	    ReDoSeq[i] = true;
	}

	if ( DoPlurality == false ) {
	  move(2,0);
	  clrtoeol();
	}
	refresh();

/* (4) Have we moved out of the current window, i.e. up or down? */

	Scroll = false;
	if ( Strand < BotStrand ) {                   /* Moved off the bottom */
	  if ( (BotStrand - Strand) < NROWS ) {
	   NScroll = BotStrand-Strand;
	   Dir = /*SMG$M_UP*/1;
	   Scroll = true;
	  }
	  for( i=Strand; i<BotStrand; i++ ) {
	    ReDoSeq[i] = true;
	    ReDoBar[i] = true;
	  }
	  BotStrand = Strand;
	} else if ( Strand > BotStrand+(NROWS-1) ) {          /* Moved of the top */
	  if ( (Strand - BotStrand) <= NROWS+5 ) {
	    NScroll = Strand-BotStrand-(NROWS-1);
	    Dir = /*SMG$M_DOWN*/0;
	    Scroll = true;
	  }
	  for ( i=Strand; i>BotStrand+(NROWS-1); i--) {
	    ReDoSeq[i] = true;
	    ReDoBar[i] = true;
	  }
	  BotStrand = Strand - (NROWS-1);
	}

/* (5) Redraw any sequences which need it  */

	if(Scroll)
	  ajDebug("NScroll = %d\n",NScroll);
	if ( Scroll ){
	  for ( i=BotStrand; i<=BotStrand+(NROWS-1); i++ ) {
	    ReDoSeq[i]= true;
	    ReDoBar[i]= true;
	  }
	  if(Dir){
	    scrl(NScroll);
	    refresh();
	  }
	  else{
	    scrl(-NScroll);
	    refresh();
	  }
	}
	if ( Select ) ReDoSeq[SelectStr] = true;

	for ( i=BotStrand; i<=BotStrand+(NROWS-1); i++ ) {
	  if ( ReDoSeq[i] ) {
	    move(MARK1-(i-BotStrand),0);
	    clrtoeol();
	    refresh();
	    MaxPos = AJMIN(ScrEdge+(__Cols-5), Seq[i].Offset+Seq[i].Length);
	    for ( ii=ScrEdge-4; ii<=MaxPos; ii++ ) {
	      if ( ii <= Seq[i].Offset ) {
	         addch(' ');
	      } else {
	        if ( DoPlurality ) {
	          Temp = toupper(Seq[i].Strand[ii-Seq[i].Offset]);
	          if ( Temp == Consensus[ii-ScrEdge+4] ) 
     	             setattr(MatchAttr);
	          else
	             setattr(DiffAttr);
	        }
                if ( SelectStr==i )
	          if ( (ii<=SelectPos+Seq[i].Offset &&
	                ii>SeqPos+Seq[i].Offset       ) ||
	               (ii>=SelectPos+Seq[i].Offset &&
	                ii<SeqPos+Seq[i].Offset  )      )
	             setattr(_REVERSE);
	        addch(Seq[i].Strand[ii-Seq[i].Offset]);
	        if ( Select || DoPlurality )  setattr(_NORMAL);
	      }
	    }
	  }
	  ReDoSeq[i] = false;
	}

	if ( Scroll ) refresh();

/* (6) Do we need to expand the bar scale? */

	if ( ReDoSeqs ) {
	  MaxLen = 100;
	  for ( i=1; i<=NOS; i++)
	    MaxLen = AJMAX(MaxLen, Seq[i].Length+Seq[i].Offset);

	  if ( MaxLen  > ScrScaleLen )
	    ScaleLen = GoodScaleLen(MaxLen);
	  else if ( MaxLen <= ScrScaleLen/2 )
	    ScaleLen = GoodScaleLen(MaxLen);

	  if ( ScaleLen != ScrScaleLen ) {
	    NumberScale(MARK2+2, 5, (__Cols-9), 7, 0, 0, ScaleLen/10);
	    ScrScaleLen = ScaleLen;
	    for ( i = BotStrand; i<=BotStrand+(NROWS-1); i++ )
	      ReDoBar[i] = true;
	  }
	}

/*
** (7) Redraw the Sequence bars, postion the global cursor if need be.
*/

	if ( Scroll ) {
	  if(Dir){
	    scrl(NScroll);
	  }
	  else
	    scrl(-NScroll);
	  refresh();
	} 

	for ( i=BotStrand; i<=BotStrand+(NROWS-1); i++) {
	  if ( ReDoBar[i] ) {
	    SeqBarLen = (Seq[i].Length*(__Cols-9))/ScaleLen;
	    SeqBarOffset = (Seq[i].Offset*(__Cols-9))/ScaleLen;
	    move(MARK2-(i-BotStrand),0);
	    clrtoeol();
	    sprintf(OneNum,"%3.3d",i);
	    addstr(OneNum);
	    if (  OkToEdit[i] == true ) {
	      if ( Seq[i].Anchored ) mvaddch(MARK2-(i-BotStrand),3, 'A');
	      if ( Seq[i].Locked )   mvaddch(MARK2-(i-BotStrand),4, 'L');
	      if ( Seq[i].Modified ) mvaddch(MARK2-(i-BotStrand),5, 'M');
	      if ( Seq[i].Anchored ) setattr(_REVERSE);
	      DrawBar( MARK2-(i-BotStrand), SeqBarOffset+7, SeqBarLen, 
                Seq[i].Type==PROTEIN, Seq[i].Reversed);
	      if ( Seq[i].Anchored ) setattr(_NORMAL);
	    }
	    refresh();

	    if ( i == Strand) {
	      ScrBarPos = (LinePos*(__Cols-9))/ScaleLen;
	      if ( ScrBarPos < 1 ) ScrBarPos = 1;
	      mvaddch((int)(MARK2-(Strand-BotStrand)), (int)(6+ScrBarPos), '#' );
	    }
	    ReDoBar[i] = false;
	  }
	}
	refresh();
/*
** (9) Display SeqName and SeqTitle of Current strand if it's changed.
*/

	if ( Strand != LastSeq ) {
	  move(0, 9);
	  /*	  clrtocol(__Cols-5);*/
	  clrtoeol();
	  mvaddstr(0,(__Cols-4), "WIBR");
	  move(1,0);
	  clrtoeol();
	  if ( OkToEdit[Strand] ) {
	    mvaddstr(0, (__Cols-strlen(Seq[Strand].Name))/2, Seq[Strand].Name);
	    mvaddstr(1, (__Cols-strlen(Seq[Strand].Title))/2, Seq[Strand].Title);
	  }
	  LastSeq = Strand;
	}

/*
** (10) Place the cursor in the current strand at the current position
*/
	move(MARK1-(Strand-BotStrand),4+LinePos-ScrEdge);
	refresh();

}  /* End of UpDate */

/****  GoodScaleLen  ********************************************************
**
**	return a good scale length for the given sequence length
**
***************************************************************************/
int GoodScaleLen(int Length )

{
int NewLen;
int Incr;

	Incr = (int) log10((double)Length);
	Incr = (int) pow( (double) 10, (double) Incr ) ;
	NewLen= AJMIN(100, Incr);

	while ( NewLen <= Length )
	  NewLen += Incr;

	return(NewLen);

} /* End of GoodScaleLen */

/****  DrawBar  **************************************************************
**
**  Draw a horizontal bar toward the right from the specified col 
**  and row.  This returns the newlength of the bar.
**
****************************************************************************/

void DrawBar(int Row, int Col, int Length, Boolean Protein, Boolean Reversed)

{
static char Bar[255];
char LeftChar, RightChar;
int i;
static int Init=true;
/*--------------------------------------*/
ajDebug("Draw Bar called  - R=%d C=%d  Len-%d\n",Row,Col,Length); 

	if (Init) {
	  for ( i=0; i<=253; i++)
	    Bar[i] = '-';
	  Bar[254] = EOS;
	  /*  Init = false;*/
	}

	if ( Protein ) {
	  LeftChar = '+';
	  RightChar = '*';
	} else {
	  if ( Reversed ) {
	    LeftChar = '<';
	    RightChar = '+';
	  } else {
	    LeftChar = '+';
	    RightChar = '>';
	  }
	}

	move(Row, Col);
	addch(LeftChar);
	Length -= 2;
	if ( Length>0 ) addstr(&Bar[255-Length]);
	addch(RightChar);

}  /* End of DrawBar */

/****  GetCmd  *************************************************************
**
**  Parse a command line and return the command number
**
***************************************************************************/

int GetCmd(int *Start, int *Finish, char *ComStr, char *ArgStr )

{

char OneLine[132], TempStr[132];
char *cPos, *tPos;

/*------------------------------------------------------------------------
* Get the command line, clear the error area after new command is entered.
*/
	ShowText("Command: ");

	getstr(OneLine);
	ShowError(" ");

	*Start = NOADDR;
	*Finish =NOADDR;
	if ( StrIsBlank(OneLine) ) return(CNULL);

/* skip leading white space */

	cPos = OneLine;
	while( isspace(*cPos) )
	   cPos++;

/* scan a number from the start of the command if it is there */
	tPos = TempStr;
	while( *cPos == '-' || isdigit(*cPos) ) {
	  *tPos = *cPos;
	  cPos++; tPos++;
	}
	*tPos = EOS;
	if ( TempStr[0] != EOS ) *Start = atoi(TempStr);

/* skip white space, comma and more white space */

	while( isspace(*cPos) || *cPos == ',' )
	    cPos++;

/* scan another number off the command line */	

	tPos = TempStr;
	while( *cPos == '-' || isdigit(*cPos) ) {
	  *tPos = *cPos;
	  cPos++; tPos++;
	}
	*tPos = EOS;
	if ( TempStr[0] != EOS ) 
	  *Finish = atoi(TempStr);
	else
	  *Finish = *Start;

/* skip white space */

	while( isspace(*cPos) )
	   cPos++;

/* get the command */

	tPos = ComStr;
	while( isalnum(*cPos) ) {
	   *tPos = *cPos;
	   cPos++; tPos++;
	}
	*tPos = EOS;

/* skip more white space */
	
	while( isspace(*cPos) )
	   cPos++;

/* save any text argument */

	strcpy(ArgStr, cPos);

/* Convert the command to a choice, return as soon as have it  */

	if ( CmdMatch(" ", ComStr) )                  return(CNULL);
	else if ( CmdMatch("SAve",ComStr) )           return(CSAVE);
	/*	else if ( CmdMatch("ALned",ComStr) )          return(CALNED);*/
	else if ( CmdMatch("ANchor",ComStr) )         return(CANCHOR);
	else if ( CmdMatch("CAncel",ComStr) )         return(CCANCEL);
	else if ( CmdMatch("CReate",ComStr) )         return(CCREATE);
	else if ( CmdMatch("DIfferences",ComStr) )    return(CDIFFERENCES);
	else if ( CmdMatch("DEGap",ComStr) )          return(CDEGAP);
	else if ( CmdMatch("DELete",ComStr) )         return(CDELETE);
	else if ( CmdMatch("DNa",ComStr) )            return(CDNA);
	/*	else if ( CmdMatch("EDIt",ComStr) )           return(CEDIT);*/
	else if ( CmdMatch("ELiminate",ComStr) )      return(CELIMINATE);
	else if ( CmdMatch("EXit",ComStr) )           return(CEXIT);
	else if ( CmdMatch("FInd",ComStr) )           return(CFIND);
	else if ( CmdMatch("FOrmat",ComStr) )         return(CFORMAT);
	else if ( CmdMatch("GO",ComStr) )             return(CGO);
	else if ( CmdMatch("GElalign",ComStr) )       return(CGELALIGN);
	else if ( CmdMatch("HArdcopy",ComStr) )       return(CHARDCOPY);
	else if ( CmdMatch("HELp",ComStr) )           return(CHELP);
	else if ( CmdMatch("INSert",ComStr) )         return(CINSERT);
	else if ( CmdMatch("LOCk",ComStr) )           return(CLOCK);
	else if ( CmdMatch("LOWer",ComStr) )          return(CLOWER);
	else if ( CmdMatch("MAtches",ComStr) )        return(CMATCHES);
	else if ( CmdMatch("MEld",ComStr) )           return(CMELD);
	else if ( CmdMatch("MOve",ComStr) )           return(CMOVE);
	/*	else if ( CmdMatch("MSf",ComStr) )            return(CMSF);*/
	else if ( CmdMatch("NAme",ComStr) )           return(CNAME);
	else if ( CmdMatch("NEither",ComStr) )        return(CNEITHER);
	else if ( CmdMatch("NOANchor",ComStr) )       return(CNOANCHOR);
	else if ( CmdMatch("Offset",ComStr) )         return(COFFSET);
	else if ( CmdMatch("OPen",ComStr) )           return(COPEN);
	else if ( CmdMatch("Quit",ComStr) )           return(CCQUIT);
	else if ( CmdMatch("REAd",ComStr) )           return(CCREAD);
	else if ( CmdMatch("REDraw",ComStr) )         return(CREDRAW);
	else if ( CmdMatch("REMove",ComStr) )         return(CREMOVE);
	else if ( CmdMatch("REVerse",ComStr) )        return(CREVERSE);
	else if ( CmdMatch("RNa",ComStr) )            return(CRNA);
	else if ( CmdMatch("SELect",ComStr) )         return(CSELECT);
	else if ( CmdMatch("SEQ",ComStr) )            return(CSEQ);
	else if ( CmdMatch("SOrt",ComStr) )           return(CSORT);
	else if ( CmdMatch("TItle",ComStr) )          return(CTITLE);
	else if ( CmdMatch("TYpe",ComStr) )           return(CTYPE);
	else if ( CmdMatch("UNlock",ComStr) )         return(CUNLOCK);
	else if ( CmdMatch("UPper",ComStr) )          return(CUPPER);
	else if ( CmdMatch("Write",ComStr) )          return(CWRITE);
	else if ( (*Start != NOADDR || *Finish != NOADDR) &&
		  ComStr[0]==EOS  && ArgStr[0]==EOS  ) {
	    if ( *Start == NOADDR ) *Start = *Finish;
	      return(CGO);
	} else {
	 sprintf(OneLine, "\"%s\" is not an MSE command!", ComStr);
	 ShowError(OneLine);
	 return(CNULL);
	}

} /* End of GetCmd */

/***  EdInit  ***************************************************************
*
*  Performs the initialization for seqed, may be ed many times
*
****************************************************************************/

void EdInit(int Start)

{
int s;
/*--------------------------------------------------------*/

	if ( Start == NOADDR ) Start = Strand;
	s = LIMIT(0, Start, NOS);
	
	dispose(Seq[s].File);
	dispose(Seq[s].Name);
	dispose(Seq[s].Title);
	dispose(Seq[s].Desc);
	dispose(Seq[s].Mem);

	Seq[s].Size = 0;
	Seq[s].Strand = NULL;
	Seq[s].Length = 0;
	Seq[s].Offset = 0;
	Seq[s].Modified = false;
	OkToEdit[s] = false;
	Seq[s].Reversed = false;
	Seq[s].Anchored = false;
	Seq[s].Locked   = false;
	ReDoSeq[s] = true;
	ReDoBar[s] = true;
	Seq[s].ReMap = PLAIN;

}  /* End of EdInit */

/****  DoDelete  ***********************************************************
**
**  	This deletes a range of the sequence.
**
****************************************************************************/

void DoDelete(int Start, int Finish)

{
int s, f;
/*---------------------------*/

	if ( OkToEdit[Strand] == false ) {
	  ShowError("Can't DELETE symbols. No strand on this line.");
	  return;
	}

	if( Seq[Strand].Locked ) {
	  ShowError("Can't DELETE symbols. This strand is currently locked.");
	  return;
	}

	SeqPos = LinePos - Seq[Strand].Offset;
	if ( Start == NOADDR )
	  Start = Get_Integer( " From", SeqPos, 1, Seq[Strand].Length); 
	if ( Finish == NOADDR )
	  Finish = Get_Integer(" To", Seq[Strand].Length, Start,Seq[Strand].Length); 

        if ( Start>Finish ) 
           ShowError("Start address exceeds finish.");
        else if ( Start<1 || Finish<1 )
           ShowError("Negative address");
        else if ( Start<=Seq[Strand].Length ) {
          s = Start;
          f = AJMIN(Seq[Strand].Length, Finish);
	  for ( ; f >= s; f--)
             DeleteSymbol(Strand, f);
	  if ( Start == 1)
	    Seq[Strand].Offset += Finish-Start+1;
	  LinePos = Seq[Strand].Offset+Start;
	}

}  /* End of DoDelete */

/****  DoDegap  *************************************************************
**
**  	This degaps a range of the sequence.
**
****************************************************************************/

void DoDegap(int Start, int Finish)

{
int s, f;


	if ( Start == NOADDR && Finish == NOADDR ) Start = Finish = Strand;

	s = LIMIT(0,AJMIN(Start,Finish), NOS);
	f = LIMIT(0,AJMAX(Start,Finish), NOS);

	for( ;s <= f; s++ ) {
	  if ( OkToEdit[s] == false ) continue;
	  DeGap(Seq[s].Mem);
	  ReDoSeq[s] = true;
	  ReDoBar[s] = true;
	}

}  /* End of DoDegap */

/****  DoReverse  ************************************************************
**
**  This reverse-complements a sequence.
**
*****************************************************************************/

void DoReverse(int Start)

{
int l, r, i, Shift;
/*-----------------------*/

	if ( Start == NOADDR ) Start = Strand;

	if ( Seq[Strand].Type <= PROTEIN ) {
	  ShowError("Can't REVERSE proteins.");
	   return;
	}

	if ( OkToEdit[Strand] == false ) {
	  ShowError("Can't REVERSE complement. No strand on this line.");
	  return;
	}

	if ( Seq[Start].Anchored ) {
	  l = Start;
	  r = Start;
	  for ( i=1; i<=NOS; i++) {
	    if (Seq[i].Anchored ) {
	      if (Seq[i].Offset+Seq[i].Length > Seq[l].Offset+Seq[l].Length) l = i;
	      if (Seq[i].Offset < Seq[r].Offset) r = i;
	    }
	  }
	  Shift = Seq[l].Offset - Seq[r].Offset;

	  for ( i=1; i<=NOS; i++) {
	    if (Seq[i].Anchored) {
	      Seq[i].Offset = Seq[i].Offset
	                + (Seq[l].Offset+Seq[l].Length) - (Seq[i].Offset+Seq[i].Length)
	                + (Seq[l].Offset - Seq[i].Offset);
	      RevComp(Seq[i].Mem, Seq[i].Type);
	      ReDoSeq[i] = true;
	      ReDoBar[i] = true;
	      Seq[i].Reversed= !Seq[i].Reversed;
	    }
	  }

	  for ( i=1; i<=NOS; i++)
	    if (Seq[i].Anchored)
	      Seq[i].Offset = Seq[i].Offset - Shift;
	} else {
	  i = Start;
/*	  RevComp(&Seq[i].Strand[1], 1, Seq[i].Length, Seq[i].Type>DNA);*/
	  RevComp(Seq[i].Mem, Seq[i].Type);
	  ReDoSeq[i] = true;
	  ReDoBar[i] = true;
	  Seq[i].Reversed = !Seq[i].Reversed;
	}

} /* End of DoReverse */

/***  DoDNA  ******************************************************************
**
** This Changes all U's to T's
**
*****************************************************************************/

void DoDNA(int s)

{
	if ( s == NOADDR ) s = Strand;
	if ( Seq[s].Type == PROTEIN ) return;
	if ( OkToEdit[s] == false ) {
	  ShowError("Can't convert symbols to DNA. No strand on this line.");
	  return;
	}

	RNAtoDNA(&Seq[s].Strand[1]);
	Seq[s].Type = DNA;
	Seq[s].Modified = true;

	ReDoSeq[s] = true;

} /* End of DoDNA */

/***  DoRNA  *****************************************************************
**
**  	This Converts all T's to U's
**
******************************************************************************/

void DoRNA(int s)

{
	if ( s == NOADDR ) s= Strand;
	if ( Seq[s].Type == PROTEIN ) return;
	if ( OkToEdit[s] == false ) {
	  ShowError("Can't convert symbols to RNA. No strand on this line.");
	  return;
	}

	DNAtoRNA(&Seq[s].Strand[1]);
	Seq[s].Type = RNA;
	Seq[s].Modified = true;

	ReDoSeq[s] = true;

}/* End of DoRNA */

/****  DoLower  *****************************************************************
**
**  Change all sequence symbols to lower case.
**
******************************************************************************/

void DoLower(int s,int f)

{
int i,j;

	if ( OkToEdit[Strand] == false ) {
	  ShowError("Can't do lower case. No strand on this line.");
	  return;
	}

	if ( s == NOADDR ) 
	  if ( SelectStr == Strand ) {
	    s = SelectPos;
	    f = SeqPos;
	  } else
	    s = 1;

	i = LIMIT(1,AJMIN(s,f),Seq[Strand].Length);
	j = LIMIT(1,AJMAX(s,f),Seq[Strand].Length);

	while( i<=j ) {
	  Seq[Strand].Strand[i] = tolower(Seq[Strand].Strand[i]);
	  i++;
	}

	DoCancel();
	ReDoSeq[Strand] = true;	

}/* End of DoLower */

/***  DoUpper  *****************************************************************
**
**  Change all sequence symbols to Upper case.
**
******************************************************************************/

void DoUpper(int s, int f)

{
int i,j;

	if ( OkToEdit[Strand] == false ) {
	  ShowError("Can't do upper case. No strand on this line.");
	  return;
	}

	if ( s == NOADDR ) 
	  if ( SelectStr == Strand ) {
	    s = SelectPos;
	    f = SeqPos;
	  } else
	    s = 1;

	i = LIMIT(1,AJMIN(s,f),Seq[Strand].Length);
	j = LIMIT(1,AJMAX(s,f),Seq[Strand].Length);

	while( i<=j ) {
	  Seq[Strand].Strand[i] = toupper(Seq[Strand].Strand[i]);
	  i++;
	}

	DoCancel();
	ReDoSeq[Strand] = true;	

}/* End of DoUpper */

/***  DoSelect  ***************************************************************
**
**  Set the Select flags for the current strand.  The passed parameter
**  START is the position in the strand relative to it's position one.
**
******************************************************************************/

void DoSelect(int Start, int Finish)
{

/*-----------------------------*/

	if ( Select ) {
	  ShowError("Select already active.");
	  return;
	}

	if ( OkToEdit[Strand] == false ) {
	  ShowError("Can't SELECT symbols. No strand on this line.");
	  return;
	}
	
	SeqPos = LinePos - Seq[Strand].Offset;
	if ( Start  == NOADDR ) Start = SeqPos;
	if ( Finish == NOADDR ) Finish = SeqPos;

	SelectPos = Start;
	SelectStr = Strand;
	Select = true;
	if ( Start < Finish)
	  LinePos = Seq[Strand].Offset + Finish + 1;
	else if ( Start > Finish )
	  LinePos = Seq[Strand].Offset + Finish - 1;

}
/* End of DoSelect */

/****  DoRemove  ************************************************************
**
**  Cuts the select region into the buffer IncSeq, Removes it from the
**  selected sequence
**
*****************************************************************************/

void DoRemove()

{
char OneLine[132];
int Start, Finish;
	
/*---------------------------------------*/
	if ( Select == false ) {
	  ShowError("No Select Range");
	  return;
	}

	if ( SelectStr != Strand ) {
	  sprintf(OneLine,
	    "You must be in line number %d in order to remove",
	     SelectStr);
	  ShowError(OneLine);
	  return;
	}

	if ( Seq[Strand].Anchored || Seq[Strand].Locked ) {
	  ShowError(
	    "Cannot remove a piece from an anchored group or locked strand.");
	  return;
	}

	SeqPos = LIMIT(1,LinePos - Seq[Strand].Offset,Seq[Strand].Length);
	if ( SeqPos < SelectPos ) {
	  Start = SeqPos + 1;
	  Finish = SelectPos;
	} else if ( SeqPos > SelectPos ) {
	  Start = SelectPos;
	  Finish = SeqPos - 1;
	} else {
	  DoCancel();
	  IncSeq[0] = EOS;
	  return;
	}

	if ( Start < 1 ) {
	  ShowError("Strand position is negative");
	  DoCancel();
	  return;
	} else {
	  strncpy(IncSeq, &Seq[Strand].Strand[Start], Finish-Start+1);
	  IncSeq[Finish-Start+1] = EOS;
	}

	SelectStr = -1;
	Select = false;
	DoDelete(Start, Finish);
	ReDoSeq[Strand] = true;
	ReDoBar[Strand] = true;

} /* End of DoRemove */

/****  DoInsert  *********************************************************
**
**  Insert the buffer IncSeq into the current position. Start is the
**  position in the recieving strand.
**
**************************************************************************/

void DoInsert(int Start)

{
int f;
int i;
/*--------------------------------*/

	if ( OkToEdit[Strand] == false ) {
	  ShowError ("Can't INSERT symbols here. No Strand on line.");
	  return;
	}

	if( Seq[Strand].Locked ) {
	  ShowError("Can't INSERT symbols. This Line is currently locked.");
	  return;
	}

	if ( Start == NOADDR ) Start = LinePos-Seq[Strand].Offset;

	if ( Start < 1  ||
	     Start > Seq[Strand].Length+1 ) {
	  ShowError("Can't insert symbols outside of strand.");
	  return;
	}

	if ( StrIsBlank(IncSeq) ) {
	   ShowError("Paste buffer is empty.");
	   return;
	}

	f = strlen(IncSeq);
	for (i=0; i<f; i++)
	  InsertSymbol(Strand, Start+i, IncSeq[i]);

	LinePos += f;
	Seq[Strand].Modified = true;
	ReDoSeq[Strand] = true;
	ReDoBar[Strand] = true;

} /* End of DoInsert */

/****  DoCancel  *************************************************************
**
**  Cancel the Selecting process
**
*****************************************************************************/

void DoCancel()

{
	Select = false;
	SelectStr = -1;
	ReDoSeq[Strand] = true;

}  /* End of DoCancel */

/****  DoMatches  ***********************************************************
**
**  Set the attribute such that matching symbols are bolded
**
****************************************************************************/

void DoMatches(char *ArgStr)

{

	switch ( toupper(*ArgStr) ) {
	  case 'B': MatchAttr = _BLINK; break;
	  case 'R': MatchAttr = _REVERSE; break;
	  case 'U': MatchAttr = _UNDERLINE; break;
	  default:  MatchAttr = _BOLD;
	}

	DiffAttr  = _NORMAL;
	DoPlurality = true;
	unique = false;
	ReDoSeq[Strand] = true;

} /* End of DoMatches */

/****  DoDifferences  ********************************************************
**
**  Set the attribute such that non-matching symbols are bolded
**
*****************************************************************************/

void DoDifferences(char *ArgStr)

{
	switch ( toupper(*ArgStr) ) {
	  case 'B': DiffAttr = _BLINK; break;
	  case 'R': DiffAttr = _REVERSE; break;
	  case 'U': DiffAttr = _UNDERLINE; break;
	  default:  DiffAttr = _BOLD;
	}

	MatchAttr = _NORMAL;
	DoPlurality = true;
	unique = false;
	ReDoSeq[Strand] = true;

} /* End of DoDifferences */

/****  DoUnique  ************************************************************
**
**  Hilight the symbol in line 1 not found elsewhere
**
*****************************************************************************/

void DoUnique(char *ArgStr)

{
	DoNeither();

	switch ( toupper(*ArgStr) ) {
	  case 'B': DiffAttr = _BLINK; break;
	  case 'R': DiffAttr = _REVERSE; break;
	  case 'U': DiffAttr = _UNDERLINE; break;
	  default:  DiffAttr = _BOLD;
	}

	MatchAttr = _NORMAL;
	unique = true;
	ReDoSeq[Strand] = true;

} /* End of DoUnique */

/****  DoNeither  ********************************************************
**
**  Turn off Plurality calculation and highlighting of sequence
**
*************************************************************************/

void DoNeither()

{
int i;

	MatchAttr = _NORMAL;
	DiffAttr  = _NORMAL;
	DoPlurality = false;
	unique = false;
	for ( i=0; i<=NOS; i++)
	  ReDoSeq[i] = true;

} /* End of DoNeither */

/*****  DoSort  **********************************************************
**
**  Sort the range of lines from "Start" to "Finish" based on 
**  increasing Offset size.
**
**************************************************************************/

void DoSort( int Start, int Finish, char *ArgStr)

{
char OneLine[256];
int i, Scope, s, f, Dir;
Boolean Switch;

/*------------------------------*/
	s = Start;
	f = Finish;
	if (s == NOADDR) s = Strand;
	if (f == NOADDR) f = NOS;

	Dir = 1;
	strcpy(OneLine,ArgStr);
	StrToUpper(OneLine);
	if ( StrIndex("DES",OneLine) ) Dir = -1;

/*-- Use strand 0 as temporary  */

	Switch = true;
	if (s >= f) Switch = false;

/* Push empty strand off to the right */

	for ( i=1; i<=NOS; i++)
	  if ( OkToEdit[i] == false ) Seq[i].Offset = 100000*Dir;

	Scope = f - 1;
	while ( Switch ) {
	  Switch = false;
	  for ( i=s; i<=Scope; i++) {
	    if ( Seq[i].Offset*Dir > Seq[i+1].Offset*Dir ) {
	      DoMove(i,0);
	      DoMove(i+1,i);
	      DoMove(0,i+1);
	      Switch = true;
	    }
	  }
	  Scope--;
	}

/* restore empty strands */

	for ( i=1; i<=NOS; i++) {
	  if ( OkToEdit[i] == false ) Seq[i].Offset = 0;
	  ReDoSeq[i] = true;
	  ReDoBar[i] = true;
	}


} /* End of DoSort */

/****  DoAnchor  ************************************************************
**
**  Make lines s through f move together with space and backspace
**
***************************************************************************/

void DoAnchor(int Start, int Finish)

{
int i,j;

	if ( Start == NOADDR && Finish == NOADDR) Start = Finish = Strand;
	
	i = LIMIT(1,AJMIN(Start, Finish),NOS);
        j = LIMIT(1,AJMAX(Start, Finish),NOS);

	while( i <= j ) {
	  Seq[i].Anchored = ReDoBar[i] = OkToEdit[i];
	  i++;
	}

} /* End of DoAnchor */

/****  DoNoAnchor  **********************************************************
**
**  Remove the anchor designation for a strand
**
****************************************************************************/

void DoNoAnchor(int Start, int Finish)

{
int i,j;

	if ( Start == NOADDR && Finish == NOADDR) Start = Finish = Strand;
	
	i = LIMIT(1,AJMIN(Start, Finish),NOS);
        j = LIMIT(1,AJMAX(Start, Finish),NOS);

	while( i <= j ) {
	  Seq[i].Anchored = false;
	  ReDoBar[i] = true;
	  i++;
	}

} /* End of NoDoAnchor */

/****  DoLock  *************************************************************
**
**  Locks lines s to f such that the user cannot add or delete symbols
**
***************************************************************************/

void DoLock(int Start, int Finish)

{
int i,j;

	if ( Start == NOADDR && Finish == NOADDR) Start = Finish = Strand;
	
	i = LIMIT(1,AJMIN(Start, Finish),NOS);
        j = LIMIT(1,AJMAX(Start, Finish),NOS);

	while( i <= j ) {
	  Seq[i].Locked = ReDoBar[i] = OkToEdit[i];
	  i++;
	}

} /* End of DoLock */

/****  DoUnLock  ************************************************************
**
**  Unlock lines s to f, which have been locked-up
**
***************************************************************************/

void DoUnLock(int Start, int Finish)

{
int i,j;

	if ( Start == NOADDR && Finish == NOADDR) Start = Finish = Strand;
	
	i = LIMIT(1,AJMIN(Start, Finish),NOS);
        j = LIMIT(1,AJMAX(Start, Finish),NOS);

	while( i <= j ) {
	  Seq[i].Locked = false;
	  ReDoBar[i] = true;
	  i++;
	}

} /* End of NoUnLock */

/***  DoQuit  ***************************************************************
**
**  Quit the editor
**
****************************************************************************/

void DoQuit()

{
char OutFName[256]="No";
int i;
Boolean Mod = 0;


 for ( i=1; i<=NOS && !Mod; i++ ) {
   if(Seq[i].Modified){
     Mod = 1;
   }
 }

 if(Mod){ /* if modified as user if they want to save */
   ShowText("Sequences modified do you wish to continue exiting [N]");
   getstr(OutFName);
   if(OutFName[0]!='Y' &&  OutFName[0]!='y') 
     return;
 }


  CleanUp();   /* never returns */

} /* End of DoQuit */

/****  DoSeq  ****************************************************************
**
** tell about the sequence being editted.
**
****************************************************************************/

void DoSeq()

{
char OneLine[132];

/*---------------------------------------*/

	if ( !OkToEdit[Strand] ) {
	  ShowError("No strand on this line.");
	  return;
	}

	
	SeqPos = LinePos - Seq[Strand].Offset;
	sprintf( OneLine, 
	  " \"%s\", %s in %s format.  Now at symbol %d of %d",
	  Seq[Strand].Name, DecodeType(Seq[Strand].Type),
          DecodeFormat(Seq[Strand].Format), SeqPos, Seq[Strand].Length);

	ShowError( OneLine );
	
} /* End of DoSeq */

/****  DoFormat  *********************************************************
**
** Set the output i.e. NBRF, Staden, Raw etc. for the output files
**
*************************************************************************/
/*
  void DoFormat(int Start, int Finish, char *ArgStr)

  {
  int s,f;
  int Format;
  char Prompt[132], InLine[132];
  
  char name[255];
  
  ??--------------------------------------------------------??
  
  if ( Start == NOADDR ) Start = Strand;	
  if ( Finish == NOADDR ) Finish = Strand;	
  
  s = LIMIT(1,MIN(Start,Finish),NOS);
  f = LIMIT(1,MAX(Start,Finish),NOS);
  
  if ( StrIsBlank(ArgStr) ) {
  sprintf(Prompt,"Sequence file format [%s]? ",DecodeFormat(DefFormat));
  ShowText(Prompt);
  getstr(InLine);
  if ( StrIsBlank(InLine) ) strcpy(InLine,DecodeFormat(DefFormat));
  } else
  strcpy(InLine, ArgStr);
  
  Format = EncodeFormat(InLine);
  
  for ( ;s <= f; s++ ) {
  if ( OkToEdit[s] == false ) continue;
  Seq[s].Format = Format;
  
  switch (Seq[s].Format) {
  case NBRF:
  sprintf(name,"%s=%s.SEQ",Seq[s].Code,Seq[s].Code);
  break;
  case GCG:
  sprintf(name,"%s.GCG",Seq[s].Code); break;
  case IG: 
  sprintf(name,"%s.IG",Seq[s].Code);  break;
  case IBI: 
  sprintf(name,"%s.IBI",Seq[s].Code);  break;
  case STADEN:
  sprintf(name,"%s.SDN",Seq[s].Code); break;
  case STRIDER:
  sprintf(name,"%s.STR",Seq[s].Code); break;
  default:
  sprintf(name,"%s.RAW",Seq[s].Code); 
  }
  
  Seq[s].IsUser = true;
  Seq[s].Name = REALLOC(Seq[s].Name, strlen(name)+1, char);
  strcpy(Seq[s].Name,name);
  if ( s == Strand ) DoName(name);   ?? Redisplay current ??
  Seq[s].Modified = true;
  ReDoBar[s] = true;
  }
  
  }?? End of DoFormat ??
*/
/****  DoType  ***********************************************************
**
**  Set the sequence type i.e. Protein, DNA, RNA etc.
**
*************************************************************************/

void DoType(int Start, int Finish, char *ArgStr)

{
int s,f;
int Type;
char InLine[132], Prompt[128];

	if ( Start == NOADDR ) Start = Strand;	
	if ( Finish == NOADDR ) Finish = Strand;	

	s = LIMIT(1,AJMIN(Start,Finish),NOS);
	f = LIMIT(1,AJMAX(Start,Finish),NOS);

	if ( StrIsBlank(ArgStr) ) {
	  ;
	  sprintf(Prompt,"Sequence type [%s]? ", DecodeType(Seq[Strand].Type));
	  ShowText(Prompt);
	  getstr(InLine);
	  if ( StrIsBlank(InLine) ) strcpy(InLine,DecodeType(Seq[Strand].Type));
	} else
	  strcpy(InLine,ArgStr);

	Type = EncodeType(InLine);
	
	for ( ;s <= f; s++ ) {
	  if ( OkToEdit[s] == false ) continue;
	  switch ( Type ) {
	    case PROTEIN:
	    case FRAGMENT:
	      Seq[s].ReMap = PLAIN; break;
	    default:
	      Seq[s].ReMap = XFORM;
	  }
	  Seq[s].Type = Type;
	  ReDoBar[s] = true;
	  Seq[s].Modified = true;
	}

}/* End of DoType */

/************************************************************************/
/****  DoGelAlign  ******************************************************
**
**
*************************************************************************/

void DoGelAlign ()

{
int Frag, i;
int Tuple, Shift;
int NewMeld;
/*-------------------------------------------------------------------*/

	if ( OkToEdit[Strand] == false ) {
	  for (i=1; i<=NOS; i++ )
	    if ( OkToEdit[i] ) break;
	  Strand = i;
	}

	Tuple = 12;
	Tuple = Get_Integer( "Search word size", Tuple, 3, 50 );
	Shift = Get_Integer( "Search word shift", 1, 1, Tuple );

	DoNoAnchor(1,NOS);
	for (i=1; i<=NOS; i++ )
	  Seq[i].Offset = 0;

	Frag = 1;
	NewMeld = -49;
	while( NextPool(&Frag) ) {
	  Seq[Frag].Offset = 1000000;
	  DoAnchor(Frag,Frag);
	  for (i=1; i<=NOS; i++ )
	    if ( Seq[i].Offset>0 && Seq[i].Anchored == false )
	      NewMeld = AJMAX(NewMeld, Seq[i].Offset+Seq[i].Length);
	  NewMeld += 50;
	  GelFind(Frag, Tuple, Shift);

	  for (i=1; i<=NOS; i++) {
	    if ( Seq[i].Anchored == false ) continue;
	    if ( Seq[i].Offset <= Seq[Frag].Offset ) Strand=i;	    	    
	  }

	  DoOffset(NewMeld);
	  DoNoAnchor(1,NOS);
	}

	DoSort(1,NOS," ");
	DoLock(1,NOS);
	DoDifferences(" ");
	for (i=1; i<=NOS; i++ )
	  ReDoSeq[i] = ReDoBar[i] = true;

	UpDate();

	return;

}  /* End of DoGelAlign */

/*****  GelFind  ***********************************************************
**
**
**
**************************************************************************/

void GelFind(int Frag, int Tuple, int Shift)

{
char OneLine[80];
char Target[80], RCTarget[80];
char *Loc;
int Pos, Pool;

/*--------------*/
	sprintf(OneLine," Now scanning strand %d, %s",Frag,Seq[Frag].Name);
	ShowError(OneLine);

	DoAnchor(Frag,Frag);
	Pos = 1;
	while ( Pos <= Seq[Frag].Length-Tuple ) {
	  strncpy(Target,&Seq[Frag].Strand[Pos],Tuple);
	  Target[Tuple] = EOS;
	  strcpy(RCTarget,Target);
	  RevComp(RCTarget,Seq[1].Type);
	  Pool = 1;
	  while ( NextPool(&Pool) ) {
	    if ( Loc = StrIndex(Target,Seq[Pool].Mem) ) {
	      Seq[Pool].Offset = Seq[Frag].Offset + Pos - (Loc - Seq[Pool].Mem+1);
	      GelFind(Pool,Tuple,Shift);
	    } else if ( Loc = StrIndex(RCTarget,Seq[Pool].Mem) ) {
	      DoReverse(Pool);
	      Seq[Pool].Offset = Seq[Frag].Offset + Pos - ((Seq[Pool].Length - (Loc-Seq[Pool].Mem)-Tuple)+1);
	      GelFind(Pool,Tuple,Shift);
	    }
	    Pool++;
	  }
	Pos += Shift;
	}

} /* End of GelFind */

/*****  NextPool  *************************************************************
**
**  Are there more strands at Offset equal 0;
**
**************************************************************************/

Boolean NextPool(int *Frag)

{
int i;

	for ( i = *Frag; i <= NOS+1; i++ ) {
	  if ( i == NOS+1 ) return(false);
	  if ( OkToEdit[i] == false )   continue;  /* Empty Frags */
	  if ( Seq[i].Offset != 0 )         continue;  /* Aligned Frags */
	  *Frag = i;
	  return(true);
	}
	
	return(false);
} /* End of NextPool */
/************************************************************************/
/************************************************************************/

/****   DoMeld  **************************************************************
**
**  Meld together strands start to finish or if Start = Finish and if that
**  strand is part of an anchor group meld the anchored group.
**
**  Melding should write out any modified strand before elimination them.
**  Use ArgStr as the new Sequence Name or create one based on the left
**  most strand with use a file extention of .CON to avoid potential conflict
**  If a plurality is not achieved then use the X symbol. Use Seq[0] for
**  melding.
**
**  Don't meld proteins. At least for now. Skip them if they are part of the
**  meld group.
**
***************************************************************************/

void DoMeld(int Start, int Finish, char *ArgStr )

{
char MeldName[132];
char OneLine[132], OutLine[132];
char *ptr;
int Left, Right, Name;
int s,f,i, NumBad;
int save;
Boolean Bad, NotContig, AGroup;

/*--------------------------------------------------------*/

	if ( Start == NOADDR ) Start = Strand;
	if ( Finish == NOADDR ) Finish = Strand;

	strcpy(MeldName, ArgStr);

	/*
	** Move the affected strands, anchored or specified range to
	** a safe place i.e. Offset > 10,000,000
	** Fix any unused lines to make sure that their Offset is defined
	** as zero.  Determine whether you have an anchored group or
	** a range of strands.
	*/

	for (i=1; i<=NOS; i++ )
	  if ( OkToEdit[i] == false ) Seq[i].Offset = 0;

	if ( Start == Strand && Finish == Strand && Seq[Strand].Anchored ){
	  s = 1;
	  f = NOS;
	  AGroup = true;
	} else {
	  s = LIMIT(1,AJMIN(Start,Finish),NOS);
	  f = LIMIT(1,AJMAX(Start,Finish),NOS);
	  AGroup = false;
	}

	/*
	** Check the strands for any Protein sequences. Ignore it if you
	** find any. Move the candidate strands to the safe region.
	*/

	for (i=s; i<=f; i++ ) {
	  if (OkToEdit[i] == false) continue;
	  if (Seq[i].Anchored == false && AGroup == true) continue;
	  if ( Seq[i].Type == PROTEIN)
	    ShowError("Can't MELD protein sequences! Ignoring it in the Meld.");
	  Seq[i].Offset += 10000000;
	  Seq[i].Anchored = false;
	}

	/*
	** Collect the vital information i.e. to, from, left most strand number
	** to be used to name/Place the meld.
	** If we don't have any condidates then just return.
	*/

	Left = 20000000;
	Right = 0;
	for (i=f; i>=s; i--) {
	  if ( Seq[i].Offset < 10000000 ) continue;	
	  Right = AJMAX(Seq[i].Offset+Seq[i].Length,Right);
	  Left  = AJMIN(Seq[i].Offset+1,Left);
	  if ( Left == Seq[i].Offset+1 ) Name = i;
	}

	if ( Right <= Left ) return;

	/* 
	** Use strand "Name" as a model for the new meld. Get some memory for 
	** the new sequence. Collect the plurality. "Stringize" it with a EOS.
	*/

	DoCopy(Name,0);
	dispose(Seq[0].Mem);

	Seq[0].Reversed = false;

	Seq[0].Size = Right-Left+512;
	if ( (Seq[0].Mem = CALLOC(Seq[0].Size,char)) == NULL ) {
	  ShowError("Failed to allocate memory.");
	  return;
	}

	Plurality(Left, Right, Seq[0].Mem);
	Seq[0].Length = Right-Left+1;
	Seq[0].Strand = Seq[0].Mem-1;
	Seq[0].Strand[Seq[0].Length+1] = EOS;
	
	/*
	** Check for positions which had no plurality, set then to "N"
	** Warn end user but keep going. If, however, the fragments were
	** not contiguous then put everything back and delete the meld.
	*/

	NotContig = false;
	Bad = false;
	NumBad = 0;
	for ( ptr=Seq[0].Mem; *ptr; ptr++ ) {
	  if ( *ptr == SPACE ) NotContig = true;
	  if ( *ptr == '.'|| *ptr == 'N' ) {
	    *ptr = 'N';
            NumBad++;
	    Bad = true;
	  }
	}

	if ( NotContig ) {
	  for ( i=1; i<=NOS; i++ )
	    if ( Seq[i].Offset >= 10000000 ) Seq[i].Offset -= 10000000;
	  ShowError("Fragments are not contiguous. Meld not formed.");
	  DoEliminate(0,0);
	  return;
	}

	if ( Bad ) {
	  sprintf(OneLine,
	   "The meld contains %d undefined positions. Continue [N] ? ",NumBad);
	  ShowText(OneLine);
	  getstr(OneLine);
	  StrToUpper(OneLine);
	  if (StrIsBlank(OneLine) || *OneLine == 'N' ) {
	    for ( i=1; i<=NOS; i++ )
	      if ( Seq[i].Offset >= 10000000 ) Seq[i].Offset -= 10000000;
	    DoEliminate(0,0);
	    return;
	  }
	}
	/*
	** Remove any strands used in making the meld.  Write out modified
	** ones before eliminating them.  Move the meld into the position
	** occupied by "Name" in the pre-meld days.
	*/

	Seq[0].Title = CALLOC(255,char);
	strcpy(Seq[0].Title,"Meld of: ");

	for (i=s; i<=f; i++) {
	  if ( Seq[i].Offset < 10000000 ) continue;
	  Strand = i;
	  if ( strlen(Seq[0].Title) < 70 ) {
	    strcat(Seq[0].Title,Seq[i].Code);
	    strcat(Seq[0].Title," ");
	  } else
	    if ( StrIndex("...",Seq[0].Title) == NULL ) 
	      strcat(Seq[0].Title,"...");
	  if ( Seq[i].Modified ) {
	   save = Strand;
	   Strand = i;
	   if (Seq[i].IsUser)   
	     DoWrite(1,Seq[i].Length,Seq[i].Name); 
	   else
	     DoWrite(1,Seq[i].Length,""); 
	   Strand = save;
/*WriteSeq(Seq[i].Name, &Seq[i], Seq[i].Format); */

	  }
	  DoEliminate(i,i);
	}

	DoCopy(0, Name);
	DoEliminate(0,0);

	/*
	** Assign the name to the meld.
	*/

	if ( StrIsBlank(MeldName) ) {
	  if(strlen(Seq[Name].Code)+5 > strlen(Seq[Name].File)){
	    dispose(Seq[Name].File);
	    Seq[Name].File = CALLOC(strlen(Seq[Name].Code)+5,char);
	  }
	  sprintf(Seq[Name].File,"%s.MLD",Seq[Name].Code);
	  switch ( Seq[Name].Format ) {
	    case NBRF:
	         sprintf(Seq[Name].Name,"%s=%s",
	           Seq[Name].Code,Seq[Name].File);
	      break;
	    default:	   
	         strcpy(Seq[Name].Name,Seq[Name].File);
	  }
	  strcpy(MeldName, Seq[Name].Name);
	  sprintf(OutLine, "Write meld into [%s] ? ",MeldName);
	  ShowText(OutLine);
	  getstr(OneLine);
	  if ( StrIsBlank(OneLine) == false ) strcpy(MeldName,OneLine);
	}
	Strand = Name;
	DoName(MeldName);

	Seq[Name].Offset = Left - 10000001;
	Seq[Name].Modified = true;
	OkToEdit[Name] = true;
	ReDoSeq[Name] = true;
	ReDoBar[Name] = true;

} /* End of DoMeld */

/****  DoWrite  ***********************************************************
**
**  Write out the sequence from position "Start" to "Finish" into file
**  "FName".  If "FName" is blank, ask user.
**
**************************************************************************/

void DoWrite( int Start, int Finish, char *FName )

{

int s, f;
char OneLine[132], OutSpec[132];
char *sChar, fChar;

SeqEntry *SeqOut;

/*-------------------------------------------*/

	if ( !OkToEdit[Strand] ) {
	  ShowError("Can't WRITE out this sequence. No strand on this line.");
	  return;
	}

	DoName(FName);
	strcpy(OutSpec,Seq[Strand].Name);

	s = Start;
	f = Finish;
	if ( s == NOADDR ) s = 1;
	if ( f == NOADDR ) f = Seq[Strand].Length;
	if ( f > Seq[Strand].Length ) f = Seq[Strand].Length;
	if ( s > f ) {
	  ShowError("Start position exceeds finish position.");
	  return;
	}
	if ( s<1 || f<1 ) {
	   ShowError("Negative sequence postition given");
	   return;
	}

	fChar = Seq[Strand].Mem[f];
	Seq[Strand].Mem[f] = EOS;

Seq[Strand].Length = f-s+1;


	sChar = Seq[Strand].Mem;
	Seq[Strand].Mem  = &(Seq[Strand].Mem[s-1]);

	SeqOut = NewSeqEntry();
	FromInternal(Strand, SeqOut);

	WriteSeq(OutSpec, SeqOut, Seq[Strand].Format);

	Seq[Strand].Mem  = sChar;
	Seq[Strand].Mem[f] = fChar;

	Seq[Strand].IsUser = true;
	Seq[Strand].Modified = false;
	ReDoBar[Strand] = true;

	/* delete SeqOut */

	sprintf(OneLine," \"%s\",  %d residues written.", OutSpec, f-s+1);
	ShowError(OneLine);

} /* End of DoWrite */

/**  DoExit  ***************************************************************
**
**  Write out any modified sequence and write-out a new .FOS file if
**  requested.
**
**************************************************************************/

void DoExit( char *FName )

{
  /*char OutLine[256], OutFName[256];
SeqSpec SeqOut;
int i;
Boolean IsUser=0;
int DefFormat;
FILE *OutFile;
Boolean Mod = 0;
char *cPos;
int p,n;*/
/*------------------------------------------*/
/*
 for ( i=1; i<=NOS && !Mod; i++ ) {
   if(Seq[i].Modified){
     Mod = 1;
   }
 }

 if(Mod){ ?? if modified as user if they want to save ??
   ShowText("Sequences modified do you wish to continue [N]");
   getstr(OutFName);
   if ( StrIsBlank(OutFName)==false) return;
   if(OutFName[0]!='Y' || OutFName[0]!='y') 
     return;
     }
*/
/*
	if ( StrIsBlank(FName) ) {
	 if ( StrIsBlank(FOSSName) ) {
	   for ( i=1; OkToEdit[i]==false && i < NOS; i++ )
	     ;
	   strcpy(FOSSName, Seq[1].Code);
	   NewFileType(FOSSName, ".FOSS");
	 }
	 sprintf(OutLine, "Name of the output FOSS file [%s] ? ", FOSSName);
	 ShowText(OutLine);
	 getstr(OutFName);
	 if ( StrIsBlank(OutFName)==false) strcpy(FOSSName, OutFName);
	} else
	  strcpy(FOSSName,FName);

	?? Need to test for success of file open ??

	OutFile = fopen(FOSSName, "w");
	DoNoAnchor(1,NOS);               ?? If not, then you may reverse all ??

	for ( i=1; i<=NOS; i++ ) {
	    ajDebug("%s isUSer = %d, Modified = %d, Format = %d, OkToEdit=%d\n",
		    Seq[i].Name,Seq[i].IsUser,Seq[i].Modified,Seq[i].Format,OkToEdit[i]);
	  if ( OkToEdit[i] ) {
	    if ( !Seq[i].IsUser && Seq[i].Modified ) { 
	      if ( Seq[i].Format != NBRF ) Seq[i].Format = DefFormat;
	      sprintf(Seq[i].File,"%s.SEQ",Seq[i].Name);

	      switch ( Seq[i].Format ) {
	        case NBRF:
	         sprintf(Seq[i].Name,"%s=%s",Seq[i].Code,Seq[i].File);
	         break;
	        default:
	         strcpy(Seq[i].Name,Seq[i].File);
	      }
	      Seq[i].IsUser = true;
	    }

	    SeqOut.options =  CALLOC(100,char);
	    SeqOut.options[0] = '\0';
	    if ( (Seq[i].Format != DefFormat) && Seq[i].IsUser ) {
	      strcat(SeqOut.options,"/");
	      ;
	      strcat(SeqOut.options,DecodeFormat(Seq[i].Format));
	    }
	    if ( Seq[i].Locked ) strcat(SeqOut.options,"/Lock");
	    if ( Seq[i].Reversed ) strcat(SeqOut.options,"/Reverse");
	    if ( Seq[i].Offset > 0 )
	      sprintf(&SeqOut.options[strlen(SeqOut.options)],
	        "/Offset=%d",Seq[i].Offset);

	    fprintf(OutFile,"%s%s\n", Seq[i].Name,SeqOut.options);

	    cPos = Seq[i].Mem;
	    while ( cPos = strchr(cPos,(int)('-')) ) {
	      p = cPos - Seq[i].Mem; n = 0;
	      while (*cPos++ == '-')
	       n++;
	      fprintf(OutFile,"  !Gaps %d,%d\n",p,n);
	    }

	    DeGap(Seq[i].Mem);

	    if ( Seq[i].Modified ) {
	      Strand = i;
	      if ( Seq[i].Reversed ) DoReverse(i);
	      DoWrite(1,Seq[i].Length,Seq[i].Name);
	    }
	  }
	}

	fclose(OutFile);

*/
	DoQuit();

} /* End of DoExit */

/****  CmdMatch  ********************************************************
**
**  this compares a string with a command and returns true if they 
**  match.  The string can be shorter than the command but it must
**  match all capital letters in ccomstr.
**
**
**************************************************************************/

int CmdMatch( char *cmdstr, char *string )

{
int pos;

/*-------------------------------------*/
	if ( strlen(string) > strlen(cmdstr) ) return(false);

	pos = 0;
	while ( (toupper(cmdstr[pos]) == toupper(string[pos]) ) && string[pos] )
  	     pos++;

	if ( string[pos] ) return(false);

	if ( islower(cmdstr[pos]) || cmdstr[pos] == EOS ) 
	   return(true);
	else
	   return(false);

} /* End of cmdmatch */

/****  InsertSymol  **********************************************************
**
**  insert a sequence symbol before the specified position in a sequence. 
**
****************************************************************************/

int InsertSymbol(int Line, int Pos, char Base)

{
int Start, Finish, Size;
int RefPos;
register char *sPos;
char *ptr, *fPos;
Boolean DoAll;

/* First check to see if we are dealing with only one strand or if
** we have to scan all of the strand's anchored Flags.
OkToEdit[i]*/

        ajDebug("1) base = %c Line= %d %s\n",Base,Line,Seq[Line].Mem);
	if ( OkToEdit[Line] == false ) return 0;
	ajDebug("2) base = %c Line= %d %s\n",Base,Line,Seq[Line].Mem);

	if ( Seq[Line].Anchored ) {
	   Start = 1;
	   Finish = NOS;
	   DoAll = false;
	} else {
	   Start = Line;
	   Finish = Line;
	   DoAll = true;
	}
	
	RefPos = Seq[Line].Offset + SeqPos;

/*
** For all seqeunce which need to be touched i.e. Anchored groups
** Adjust the Offset if We're to left of the start position.
** Otherwize add the symbol if we're in the sequence or at the end.
** Beyond that do nothing.
*/

	for ( Line=Start; Line <= Finish; Line++ ) {
	  if ( Seq[Line].Anchored || DoAll ) {
	    if ( RefPos <= Seq[Line].Offset )
	      Seq[Line].Offset++;
	    else if ( RefPos <= Seq[Line].Offset+Seq[Line].Length+1 ) {
	      if ( Seq[Line].Locked && Base != GAP ) continue;
	      if ( Seq[Line].Size < Seq[Line].Length+10 ) {
	        Size = Seq[Line].Size + 512;
	        if ( (ptr = REALLOC(Seq[Line].Mem,Size, char)) == NULL ) {
	          ShowError("Unable to expand memory, EXIT and reload FOS file to continue.");
	          return 0;
	        }
	        Seq[Line].Size = Size;
	        Seq[Line].Mem = ptr;
	        Seq[Line].Strand = ptr-1;
	      }
	      
	      
	      ajDebug("base = %c Line= %d %s\n",Base,Line,Seq[Line].Mem);
	      sPos = &Seq[Line].Strand[Seq[Line].Length+1];
	      fPos = &Seq[Line].Strand[RefPos-Seq[Line].Offset];
	      for ( ; sPos>=fPos; sPos--) 
	        *(sPos+1) = *sPos;
	      *fPos = Base;
	      Seq[Line].Length++;
	      if ( Base != '-' ) Seq[Line].Modified = true;
	    }
	    ReDoSeq[Line] = true;
	    ReDoBar[Line] = true;
	  }
	}

	return 1;
}  /* End of InsertSymbol */

/*****  DeleteSymbol  ********************************************************
**
**  Delete the symbols at position "Pos" in strand "Line".  If "Line" is a
**  member of an anchored group. Modify those accordingly.
**
******************************************************************************/

void DeleteSymbol(int Line, int Pos)

{
int Start, Finish, i, Size;
int RefPos;
Boolean DoAll;
char *ptr;

/* First check to see if we are dealing with only one strand or if
** we have to scan all of the strand's anchored Flags.
*/

	if ( OkToEdit[Line] == false ) return;

	if ( Seq[Line].Anchored ) {
	   Start = 1;
	   Finish = NOS;
	   DoAll = false;
	} else {
	   Start = Line;
	   Finish = Line;
	   DoAll = true;
	}
	
/*
** For all seqeunce which need to be touched i.e. Anchored groups
**   Left of sequence  - decrease Offset.
**         Position 1  - Do nothing
**   Right of sequence - Do nothing
**
**   Otherwise, remove symbols by shifting sequence left.
*/

	RefPos = Seq[Line].Offset + Pos;

	for ( Line=Start; Line <= Finish; Line++ ) {
	  if ( Seq[Line].Anchored || DoAll ) {
	    if ( RefPos <= Seq[Line].Offset ) {
	      Seq[Line].Offset--;
	    } else if ( RefPos <= Seq[Line].Offset+Seq[Line].Length ) {
	      i = RefPos - Seq[Line].Offset;
	      if ( Seq[Line].Locked && Seq[Line].Strand[i] != GAP ) continue;
	      if ( Seq[Line].Strand[i] != '-') Seq[Line].Modified = true;
	      for ( ; Seq[Line].Strand[i]; i++)
	        Seq[Line].Strand[i] = Seq[Line].Strand[i+1];
	      Seq[Line].Length--;

	      if ( Seq[Line].Size > Seq[Line].Length+1024 ) {
	        Size  = Seq[Line].Length+512;
	        ptr = REALLOC(Seq[Line].Mem, Size, char);
	        Seq[Line].Size  = Size;
	        Seq[Line].Mem = ptr;
	        Seq[Line].Strand = ptr-1;
	      }
	    }
	    ReDoSeq[Line] = true;
	    ReDoBar[Line] = true;
	  }
	}

} /* End of DeleteSymbol */

/*****  DoEliminate  ********************************************************
**
**  Eliminate strand(s) from the line start to line finish.
**
****************************************************************************/

void DoEliminate( int Start, int Finish )

{
int s, f;

/*-------------------------------------------------*/

	if ( Start == NOADDR && Finish == NOADDR ) Start = Finish = Strand;

	s = LIMIT(0,AJMIN(Start,Finish), NOS);
	f = LIMIT(0,AJMAX(Start,Finish), NOS);

	for( ;s <= f; s++ ) {
	  if ( OkToEdit[s] == false ) continue;
	  EdInit(s);
	  ReDoSeq[s] = true;
	  ReDoBar[s] = true;
	}

} /* End of DoEliminate */

/****  DoCopy  *************************************************************
**
**  Make a copy from line start to line finish
**
**************************************************************************/

void DoCopy( int from, int to )

{
/*----------------------------------*/

	if ( from == NOADDR ) from = Strand;
	if ( to == NOADDR ) {
	  to = Strand;
	  while( OkToEdit[to] )
	    if ( ++to > NOS ) return;
	}

	from = LIMIT(0, from, NOS);
	to = LIMIT(0, to, NOS);

	if ( from == to ) return;

	EdInit(to);

	Seq[to].Code = CALLOC(strlen(Seq[from].Code)+1,char);
	strcpy(Seq[to].Code,Seq[from].Code);
	
	Seq[to].File = CALLOC(strlen(Seq[from].File)+1,char);
	strcpy(Seq[to].File,Seq[from].File);

	Seq[to].Name = CALLOC(strlen(Seq[from].Name)+1,char);
	strcpy(Seq[to].Name,Seq[from].Name);
	
	Seq[to].Title = CALLOC(strlen(Seq[from].Title)+1,char);
	strcpy(Seq[to].Title,Seq[from].Title);
	
	Seq[to].Desc = CALLOC(strlen(Seq[from].Desc)+1,char);
	strcpy(Seq[to].Desc,Seq[from].Desc);
	
	Seq[to].Type     = Seq[from].Type;
	Seq[to].Circular = Seq[from].Circular;
	Seq[to].Length   = Seq[from].Length;

	Seq[to].Mem = CALLOC(Seq[from].Size,char);
	strcpy(Seq[to].Mem,Seq[from].Mem);
	Seq[to].Size     = Seq[from].Size;
	Seq[to].Strand   = Seq[to].Mem-1;

	Seq[to].Offset = Seq[from].Offset;
	Seq[to].Modified = Seq[from].Modified;
	Seq[to].Anchored = Seq[from].Anchored;
	Seq[to].Locked = Seq[from].Locked;
	Seq[to].Reversed = Seq[from].Reversed;
	Seq[to].ReMap = Seq[from].ReMap;

	OkToEdit[to] = OkToEdit[from];

	ReDoSeq[from]  = ReDoBar[from] = true;
	ReDoSeq[to] = ReDoBar[from] = true;

} /* End of DoCopy */

/****  DoMove  *************************************************************
**
**  Move a sequence to a new line,  (From, To)
**
***************************************************************************/

void DoMove( int from, int to )

{

	if ( from == NOADDR ) from = Strand;
	if ( to == NOADDR ) {
	  to = Strand;
	  while( OkToEdit[to] )
	    if ( ++to > NOS ) return;
	}

	from = LIMIT(0, from, NOS-1);
	to = LIMIT(0, to, NOS-1);

	if ( from == to ) return;

	Seq[to].Code = Seq[from].Code;
	Seq[from].Code = NULL;
	
	Seq[to].File = Seq[from].File;
	Seq[from].File = NULL;

	Seq[to].Name = Seq[from].Name;
	Seq[from].Name = NULL;
	
	Seq[to].Title = Seq[from].Title;
	Seq[from].Title = NULL;
	
	Seq[to].Desc = Seq[from].Desc;
	Seq[from].Desc = NULL;
	
	Seq[to].Type     = Seq[from].Type;
	Seq[to].Circular = Seq[from].Circular;
	Seq[to].Length   = Seq[from].Length;

	Seq[to].Mem = Seq[from].Mem;
	Seq[from].Mem = NULL;
	Seq[to].Size     = Seq[from].Size;
	Seq[to].Strand   = Seq[from].Strand;

	Seq[to].Offset = Seq[from].Offset;
	Seq[to].Modified = Seq[from].Modified;
	Seq[to].Anchored = Seq[from].Anchored;
	Seq[to].Locked = Seq[from].Locked;
	Seq[to].Reversed = Seq[from].Reversed;
	Seq[to].ReMap = Seq[from].ReMap;

	OkToEdit[to] = OkToEdit[from];
	ReDoSeq[to] = ReDoBar[from] = true;

	EdInit(from);

}
/* End of DoMove */

/****  DoOpen  *************************************************************
**
**  Open up a line at Strand by moving all above sequence to higher
**  available line numbers. 
**
***************************************************************************/

void DoOpen()

{
int i, s;
/*----------------------------------------*/
ajDebug("DoOpen entered\n");

	s = Strand;
	i = s + 1;
	while( OkToEdit[i] )
          if ( ++i > NOS ) return;

	while ( i > s) {
	  DoMove(i-1,i);
	  i--;
	}

   OkToEdit[Strand] = true;
   Seq[Strand].Mem = CALLOC(1,char);
   strcpy(Seq[Strand].Mem,"");
   Seq[Strand].Length = 0;
   Seq[Strand].Size = 1;
   Seq[Strand].Strand = Seq[Strand].Mem-1;
   if(Seq[Strand-1].Type == PROTEIN || Seq[Strand+1].Type == PROTEIN){  
     Seq[Strand].Type = PROTEIN;
     Seq[Strand].ReMap = PLAIN;
   }
   else {
     Seq[Strand].Type = DNA;
     Seq[Strand].ReMap = XFORM;
   }
   Seq[Strand].Circular = false;
   Seq[Strand].Format = DefFormat;
   
   Seq[Strand].Name = CALLOC(1,char);
   strcpy(Seq[Strand].Name,"");
    Seq[Strand].Code = CALLOC(1,char);
    strcpy(Seq[Strand].Code,"");
    Seq[Strand].File = CALLOC(1,char);
    strcpy(Seq[Strand].File,"");
    Seq[Strand].Title = CALLOC(1,char);
    strcpy(Seq[Strand].Title,"");
    Seq[Strand].Desc = CALLOC(1,char);
    strcpy(Seq[Strand].Desc,"");
    /*    Seq[Strand].ReMap = XFORM;*/
    Seq[Strand].Offset = 0;
    Seq[Strand].Modified = false;
    Seq[Strand].Reversed = false;
    Seq[Strand].Locked = false;
    Seq[Strand].Anchored = false;

} /* End of DoOpen */

/****  DoOffset  **********************************************************
**
**  Command line way to set the Offset to a strand or anchored set
**  Argument is absolute but is converted to difference
**
**************************************************************************/

void DoOffset(int Start)

{
int i, s, diff;
/*---------------------------------------*/
	s = Start;
	if ( Start == NOADDR ) s = LinePos-1;

	diff = s - Seq[Strand].Offset;
	if ( diff < 0 ) {
	  if ( Seq[Strand].Anchored ) {
	    for ( i=1; i<=NOS; i++)
	      if ( Seq[i].Anchored )
	        if (-Seq[i].Offset>diff) diff = -Seq[i].Offset;
	    if ( s-Seq[Strand].Offset > diff ) diff = s-Seq[Strand].Offset;
	  }
	}
	
	if ( Seq[Strand].Anchored ) {
	  for ( i=1; i<=NOS; i++) {
	    if ( Seq[i].Anchored ){
    	      Seq[i].Offset = Seq[i].Offset + diff;
	      if ( Seq[i].Offset<0 ) Seq[i].Offset = 0;
	      ReDoSeq[i] = true;
	      ReDoBar[i] = true;
	    }
	  }
	} else {
    	  Seq[Strand].Offset =  Seq[Strand].Offset + diff;
	  if ( Seq[Strand].Offset<0 ) Seq[Strand].Offset = 0;
	  ReDoSeq[Strand] = true;
	  ReDoBar[Strand] = true;
	}

} /* End of DoOffset  */

/****  DoEdit  *************************************************************
**
**  Starts the session with a new or used sequence file.
**
***************************************************************************/
/*
void DoEdit( char *InLine, int Start )

{

#define ALLOWEDTRYS 3

SeqSpec *WildSpec;
SeqEntry *SeqIn;

char WildName[132], DefSeqSpec[132], *cPos;
char OneLine[132], Prompt[132], ErrMsg[256], Options[256];
int Trys, TooManyErrors;
int LineLen, i, j, k, Row;
Boolean error=false;
static Boolean FirstTime=true, OneSpec;

	WildSpec = NewSeqSpec();

	if ( Start == NOADDR ) Start = Strand;
	Start = LIMIT(1, Start, NOS);

	strcpy(WildName, InLine);
	strcpy(Prompt, "Edit which sequence(s)");

	if ( FirstTime )
	  strcpy(DefSeqSpec,"MSEDEFAULT");
	else
	  DefSeqSpec[0]='\0';

	Trys = 0;
	TooManyErrors = 0;

TryAgain:

	Trys++;
	if ( Trys > ALLOWEDTRYS ) return;
	
	ajDebug("WildName = %s (%d)\n",WildName,strlen(WildName));
	Get_SeqSpec(&WildName[0], WildSpec, Prompt, DefSeqSpec);
	ajDebug("WildName = %s (%d)\n",WildName,strlen(WildName));

	if ( StrIsBlank(WildName) ) return;
	OneSpec = WildSpec->isWildCode || WildSpec->isWildFile;
	if ( *(WildSpec->file) == '@' ) {
	  strcpy(FOSSName, WildSpec->file+1);
	  DoFOSS(FOSSName);
	  return;
	}

	while ( OkToEdit[Start] ) {
	  Start++;
	  if ( Start > NOS ) {
	    sprintf(ErrMsg, "No more Strands, %d maximum.",NOS);
	    ShowError(ErrMsg);
	    return;
	  }
	}

	while ( SeqIn = NextSeqEntry(WildSpec) ) {
	  if ( error == false ) {
	    ToInternal(Start,SeqIn);
	    Seq[Start].Offset = LinePos-1;
	  } else {
	    if (OneSpec) {
	      WildName[0] = EOS;
	      goto TryAgain;
	    }
	    if ( ++TooManyErrors > 10 ) goto TryAgain;
	    goto LoopBot;
	  }

	  OkToEdit[Start] = true;
	  Strand = Start;
	  strcpy(Options,StrToUpper(SeqIn->spec->options));
	  if ( StrIndex("/LOC",Options) ) DoLock(Start,Start);
	  if ( StrIndex("/REV",Options) ) DoReverse(Start);
	  if (cPos = StrIndex("/OFF",Options) ) {
	    strcpy(Options,cPos+1);
	    if ( cPos = strchr(Options,'/') ) *cPos = '\0';
	    if ( cPos = strchr(Options,'=') ) DoOffset(atoi(cPos+1));
	  }

	  SeqIn->mem = NULL;
	  SeqIn->text = NULL;
	  DeleteSeqEntry(SeqIn);
	  SeqIn = NULL;

	  switch ( Seq[Start].Type ) { 
	    case PROTEIN:
	    case FRAGMENT:
	      Seq[Start].ReMap = PLAIN;
	      break;
	    case UNDEF:
	    default:
	      Seq[Start].ReMap = XFORM;
	  }

	  DoName( Seq[Start].Name );
	  DoTitle( Seq[Start].Title );
	  ReDoSeq[Start] = true;
	  ReDoBar[Start] = true;
	  Strand = Start;

LoopBot:
	  while ( OkToEdit[Start] ) {
	    Start++;
	    if ( Start > NOS ) {
	      sprintf(ErrMsg, "No more Strands, %d maximum.",NOS);
	      ShowError(ErrMsg);
	      return;
	    }
	  }
	}

	UpDate();
	refresh();
	ajDebug("DoEdit finished\n");

}*/
  /* End of DoEdit */


/****  DoCreate  *************************************************************
**
**  Create a new sequence entry from within MSE.
**
***************************************************************************/
void DoCreate(int Start, char *InLine)
{
char ErrMsg[256];

	if ( Start == NOADDR ) Start = Strand;
	Start = LIMIT(1, Start, NOS);

	while ( OkToEdit[Start] ) {
	  Start++;
	  if ( Start > NOS ) {
	    sprintf(ErrMsg, "No more Strands, %d maximum.",NOS);
	    ShowError(ErrMsg);
	    return;
	  }
	}

	Strand = Start;
	Seq[Strand].Mem = CALLOC(512,char);
	Seq[Strand].Length = 0;
	Seq[Strand].Size = 512;
	Seq[Strand].Strand = Seq[Strand].Mem-1;
	Seq[Strand].Type = DNA;
	Seq[Strand].Circular = false;
	Seq[Strand].Format = DefFormat;
	Seq[Strand].Name = CALLOC(1,char);
	Seq[Strand].Code = CALLOC(1,char);
	Seq[Strand].File = CALLOC(1,char);
	Seq[Strand].Title = CALLOC(1,char);
	Seq[Strand].Desc = CALLOC(1,char);
	Seq[Strand].ReMap = XFORM;
	Seq[Strand].Offset = SeqPos;
	Seq[Strand].Modified = true;
	Seq[Strand].Reversed = false;
	Seq[Strand].Locked = false;
	Seq[Strand].Anchored = false;
	

	DoName("");
	DoTitle("");
	DoType(Strand,Strand,"");
	ReDoSeq[Start] = true;
	ReDoBar[Start] = true;

}

/****  DoFOSS  **********************************************************
**
**
**
*************************************************************************/
/*
void DoFOSS(char *FOSSName)

{
char OneLine[132], Gap[20];
char Temp[20];
char SaveDB[132];
char *cPos,*Comma;
FILE *InFile;
int Number, Position;
int i;
long before;


	if ( (InFile = fopen(FOSSName,"r")) == NULL ) {
	  ShowError("File not found.");
	  ajDebug("Error %s not found\n",FOSSName);
 	  return;
	}

	while (before = ftell(InFile), fgets(OneLine,131,InFile) != NULL ) {
	  if (cPos = strchr(OneLine,'\n')) *cPos = EOS;
	  DoEdit(OneLine,Strand);
	ajDebug("End of DoEdit\n");
 	  DoDegap(Strand,Strand);
	ajDebug("End of Degap\n");
	  while (before = ftell(InFile), fgets(OneLine,131,InFile) != NULL ) {
	    StrTrim(OneLine);
	    if ( *OneLine != '!' ) {
	      fseek(InFile,before,SEEK_SET);
	      break;
	    }

	    if ( (cPos = StrIndex("!Gaps ", OneLine)) == NULL ) continue;
	    cPos += 5;
	    while ( cPos = NextToken(OneLine, cPos, Gap, " \n") ) {
	      if ( Comma = strchr(Gap,',') ) {
	       strcpy(Temp,Comma+1);
	       Number = atoi(Temp);
	       *Comma = EOS;
	       strcpy(Temp,Gap);
	       Position = atoi(Temp)+1;

	       for (; Number; Number--)
	         InsertSymbol(Strand,Position,'-');
	      }
	    }
	  }
	  UpDate();
	  refresh();
	}
                      
	ajDebug("End of DoFOSS\n");


	



	fclose(InFile);

}
*/
/****  DoAlned  **********************************************************
**
**
**
*************************************************************************/
/*
void DoAlned(char *ArgStr)

{
char OneLine[132], Gap[20];
char Temp[20];
char SaveDB[132];
char *cPos,*Comma;
FILE *InFile;
int Number, Position;
int i;

      
ajDebug("DoAlned entered\n");
	if ( StrIsBlank(ArgStr) ) {
	  ShowText("Read which ALNED alignment file? ");
	  getstr(OneLine);
	}

	if ( (InFile = fopen(OneLine,"r")) == NULL ) {
	  ShowError("File not found.");
	  return;
	}

	fgets(OneLine,131,InFile);  ?? Title, skip it ??
	fgets(OneLine,131,InFile);  ?? Total length, skip it ??

	while ( fgets(OneLine,131,InFile) != NULL ) {
	  if (cPos = strchr(OneLine,'\n')) *cPos = EOS;
	  if (OneLine[0] == '*' ) continue;      ?? Stuff for DISP ??
	  if (OneLine[0] == '[' ) continue;      ?? Stuff for DISP ??
	  if (StrIsBlank(OneLine) ) break;  ?? Stop reading at first blank ??
	  if ( cPos = StrIndex("/T", OneLine) ) {    ?? Skip Special title ??
	    *cPos = EOS;
	    DoEdit(OneLine,Strand);
	    fgets(OneLine,131,InFile); 
	  } else
	    DoEdit(OneLine,Strand);

 	  DoDegap(Strand,Strand);

	  while ( fgets(OneLine,131,InFile) != NULL ) {
	    StrCompress(OneLine);
	    cPos = OneLine;
	    while ( cPos = NextToken(OneLine, cPos, Gap, " \n") ) {
	      if ( Comma = strchr(Gap,',') ) {
	       strcpy(Temp,Comma+1);
	       Number = atoi(Temp);
	       *Comma = EOS;
	       strcpy(Temp,Gap);
	       Position = atoi(Temp)+1;

	       for (; Number; Number--)
	         InsertSymbol(Strand,Position,'-');
	      }
	    }
	    if (cPos = strchr(OneLine,'/') ) break;
	  }
	}
                      
	fclose(InFile);

}
*/

/****  DoName  **************************************************************
**
**  Assign a new code to a sequence. Also used to change output file 
**  name.  If passed a valid database spec then just display it.  If
**  a new filename is not passed used the current one, if one does not
**  exist then make one from the codeword.
**
*****************************************************************************/

void DoName(char *ArgStr)

{
char Prompt[128];
char name[128];

 
        if ( StrIsBlank(ArgStr) ) {
	  strcpy(name,Seq[Strand].Name);
	  sprintf(Prompt,"Name for this entry [%s]? ",name);
	  ShowText(Prompt);
	  getstr(ArgStr);
	}
        if ( ! StrIsBlank(ArgStr) ) {
	  strcpy(name,ArgStr);
	}
/*
** Stop here if we have nothing from the user, i.e Cancel. Otherwise,
** assign the new variables and repaint the screen.
*/

	if ( StrIsBlank(name) ) return;

	if(strlen(Seq[Strand].Name) < strlen(name)){
	  free(Seq[Strand].Name);
	  Seq[Strand].Name = CALLOC(strlen(name)+1,char);
	}
	strcpy(Seq[Strand].Name,name);
	move(0, 9);
	/*	clrtocol(__Cols-5);*/
	clrtoeol();
	mvaddstr(0,(__Cols-4), "WIBR");
	mvaddstr(0, (__Cols-strlen(Seq[Strand].Name))/2, Seq[Strand].Name);

	OkToEdit[Strand] = true;
	ShowText(" ");

} /* End of DoName */

/****  DoTitle  ************************************************************
**
** Get a new title for this sequence and display it.
**
***************************************************************************/

void DoTitle(char *InLine)

{
char OneLine[256];

	if ( !OkToEdit[Strand] ) {
	  ShowError("Can't change TITLE.  No strand on this line.");
	  return;
	}

	strcpy(OneLine, InLine);

	if ( StrIsBlank(OneLine) ) {
	  ShowText("Title: ");
	  getstr(OneLine);
	}

	if ( StrIsBlank(OneLine) ) return;

	Seq[Strand].Title = REALLOC(Seq[Strand].Title, strlen(OneLine)+1, char);
	strcpy(Seq[Strand].Title,OneLine);

	move(1, 0);
	clrtoeol();
	mvaddstr(1, (__Cols-strlen(Seq[Strand].Title))/2, Seq[Strand].Title);

	ShowText(" ");

} /* End of DoTitle */

void DoFormat(char *format){
int formatnumber;
 AjPStr tmpformat = NULL;

 tmpformat = ajStrNewC(format);
 
 if (!ajSeqFindOutFormat(tmpformat, &formatnumber)) {
   ShowError("format not recognised");
 }
 else{
   DEFAULTFORMAT = formatnumber;
 }
  ajStrDel(&tmpformat);
}


/****  DoMSF  **************************************************************
**
**  Rename all of the sequence spec's such that they will be written
**  to the same file upon exiting. Does not check for duplicate name
**  as a result of duplicate codewords.
**
**************************************************************************/
/*
void DoMSF(char *ArgStr)

{
char OutFName[256], Code[256];
int i;
??--------------------------------------------??

	strcpy(OutFName, ArgStr);
	if ( StrIsBlank(OutFName) ) {
	  ShowText("Write all sequence entries to which file? ");
	  getstr(OutFName);
	}
	StrCollapse(StrToUpper(OutFName));

	if ( StrIsBlank(OutFName) ) return;

	for (i=1; i <= NOS; i++ )
	  if ( OkToEdit[i] && (Seq[i].Format == NBRF) ) {
	    sprintf(Seq[i].Name,"%s=%s", Seq[i].Code, OutFName);
	    if ( i == Strand) DoName(Seq[i].Name);
	    Seq[i].IsUser = true;
	    Seq[i].Modified = true;
	    ReDoSeq[i] = true;
	    ReDoBar[i] = true;
	  }

} ?? End of DoMSF ??
*/

/****  DoHardCopy  ********************************************************
**
**  Create an output file similar to that produced by PRETTY showing the
**  Generated alignments
**
*************************************************************************/

void DoHardcopy(int Start, int Finish, char *InLine )

{
char OutFName[256], OutLine[256], OneNum[64];
char Consensus[256], LeftChar, RightChar;
int MaxLen, NOut, Pos, CharPos;
int i, ii, n, Step, SeqBarOffset, SeqBarLen;
int TotalSeqs, BlocksPerPage, Block, Lines;
Boolean Diff;
FILE *OutFile;

/*-------------------------------------------------------------------*/

	if ( Start == NOADDR ) Start = 1;
	if ( Finish == NOADDR ) Finish = 1000000;

/* Ask for an output filename based on FOS filenam if available else
** use the first code you come to
*/

	strcpy(OutFName, InLine);
	if ( StrIsBlank(OutFName) ) {
	  if ( StrIsBlank(FOSSName) ) { 
	    for ( i=1; OkToEdit[i]==false; i++ )
	     ;
	    strcpy(OutFName, Seq[i].Code);
	    NewFileType(OutFName, ".lis");
	  } else {
	    strcpy(OutFName, FOSSName);
	    NewFileType(OutFName,".lis");
	  }
	  sprintf(OutLine,"Write hardcopy to which file [%s] ? ",OutFName);
	  ShowText(OutLine);	
	  getstr(OutLine);
	  if ( StrIsBlank(OutLine) == false ) strcpy(OutFName, OutLine);
	}

	if ( (OutFile = fopen(OutFName, "w")) == NULL ) {
	  ShowError("Cannot of output file for HARDCOPY command");
	  return;
	}

/*
** Ask about layout Blocksize, Linesize.
** Ask about plurality as well.
*/

	LineSize =  Get_Integer("How many symbols per line", LineSize, 1, 120);
	BlockSize = Get_Integer("written in blocks of", BlockSize, 1, 120);

	ShowText("Show differences from the Plurality [Yes]?");
	getstr(OutLine);
	if ( StrIsBlank(OutLine) == false )
	  if ( toupper(OutLine[0]) == 'N' ) Diff = false;
	else
	 Diff = true;

/*
** Write out the file, Header, graph and then the sequences.
*/

	fprintf(OutFile,"No.  Sequence Spec, Title and Description\n");
	fprintf(OutFile,"---  ------------------------------------\n");
	Lines = 2;

	TotalSeqs = 0;
	for ( i=NOS; i>=1; i--)
	  if ( OkToEdit[i] ) {
	    fprintf(OutFile,"%3d  %s\n       %s\n       %s\n", 
	      i, Seq[i].Name, Seq[i].Title, Seq[i].Desc);
	    TotalSeqs++;
	  }
	Lines += TotalSeqs*3;

/*
**  Move to a new page if the graph is not going to fit.
*/

	if ( PageSize-Lines < TotalSeqs + 5 ) fprintf(OutFile,"%c\n",CNTRLL);

	fprintf(OutFile,"\n\n");
	for ( i=NOS; i>=1; i--) {
	  if ( OkToEdit[i] ) {
	    sprintf(OutLine, "%3d", i);
	    for ( ii=3; ii<80;ii++)
	      OutLine[ii]=SPACE;
	    OutLine[80]=EOS;
	    if ( Seq[i].Anchored ) OutLine[3] = 'A';
	    if ( Seq[i].Modified ) OutLine[4] = 'M';
	    if ( Seq[i].Locked   ) OutLine[5] = 'L';
	    SeqBarLen = ((Seq[i].Length*71)/ScaleLen) - 2;
	    SeqBarOffset = (Seq[i].Offset*71)/ScaleLen;
	    for (ii=SeqBarOffset; ii<=SeqBarOffset+SeqBarLen;ii++)
	      OutLine[ii+7] ='-';
	    if ( Seq[i].Type == PROTEIN ) {
	      LeftChar = '+';
	      RightChar = '*';
	    } else {
	      if ( Seq[i].Reversed ) {
	        LeftChar = '<';
	        RightChar = '+';
	      } else {
	        LeftChar = '+';
	        RightChar = '>';
	      }
	    }
	    OutLine[7+SeqBarOffset] = LeftChar;
	    OutLine[7+SeqBarOffset+SeqBarLen] = RightChar;
	    fprintf(OutFile, "%s\n", OutLine);
	  }
	}

/* write out the scale */

	fprintf(OutFile,"      |");
	for (ii=1;ii<=71;ii++)
	  if ( ii%7 )
	    fprintf(OutFile,".");
	  else
	    fprintf(OutFile,"|");

	fprintf(OutFile, "\n");

/* ... and the number line */

	n = 0;
	Step = ScaleLen/10;
	for (i=0; i<=10; i++){
	  fprintf(OutFile,"%7.7d",n);
	  n += Step;
	}
	fprintf(OutFile, "%s\n", OutLine);


/*
** ... find the longest strand
*/

	MaxLen = 0;
	for (i=1; i<=NOS; i++)
	  if ( OkToEdit[i] ) MaxLen = AJMAX(MaxLen, Seq[i].Length+Seq[i].Offset);
	Finish = AJMIN(MaxLen, Finish);

/* 
** Make sure that the user didn't ask for a printout go over 132 wide 
*/

	LineSize = LineSize - (LineSize%BlockSize);
	while ( LineSize+(LineSize/BlockSize)+11 > 132 )
	  if ( LineSize==BlockSize ) {
	    LineSize = 120;
	    BlockSize = 120;
	  } else
	    LineSize = LineSize - BlockSize;

/*
** Write out the strands in groups of NOut.
** Start this section on a new page.
*/
	fprintf(OutFile,"%c\n",CNTRLL);

	if ( PageSize > TotalSeqs+5 )
	 BlocksPerPage = PageSize/(TotalSeqs+5);
	else 
	 BlocksPerPage = 1;

	Block=0;
	while ( Start<=Finish ) {
	  fprintf(OutFile,"\n\n");
	  NOut = AJMIN(Finish-Start+1, LineSize);

/*
**  Generate the Plurality for the block
*/

	  for (i=0; i<256; i++)
	    Consensus[i] = EOS;

	  if ( true /*Diff*/ ) {
	    Plurality(Start, Start+NOut-1, Consensus);
	    StrTruncate(Consensus);
	    fprintf(OutFile,"Plurality  ");

	    for ( i=0; Consensus[i]; i++ ) {
	      if ( i%BlockSize == 0 ) fprintf(OutFile," ");
	      fprintf(OutFile,"%c",Consensus[i]);
	    }
	  }

	  fprintf(OutFile, "\n");
	  for ( i=NOS; i>=1; i--) {
	    if ( OkToEdit[i] ) {
	      fprintf(OutFile, "%-10s  ", Seq[i].Code );
	      CharPos = 12;
	      Pos = 0;
	      while (Pos<NOut) {
	        if ( (CharPos-11)%(BlockSize+1) == 0 )
	          fprintf(OutFile,"%c",SPACE);
	        else if ( (Start+Pos)>(Seq[i].Length+Seq[i].Offset) )
	          Pos++;
	        else if ( (Start+Pos)<=Seq[i].Offset ) {
	          fprintf(OutFile,"%c", SPACE);
	          Pos++;
	        } else {
	          if ( Diff && (Consensus[Pos] == Seq[i].Strand[Start+Pos-Seq[i].Offset]) ) 
	            fprintf(OutFile,"%c",'.');
	          else
	            fprintf(OutFile,"%c",Seq[i].Strand[Start+Pos-Seq[i].Offset]);
	          Pos++;
	        }
	        CharPos++;
	      }
	      fprintf(OutFile, "\n");
	    }
	  }

	  fprintf(OutFile, "            ");
	  CharPos = 12;
	  Pos = 0;
	  while (Pos<LineSize) {
	    if ( (CharPos-11)%(BlockSize+1)==0 )
	      fprintf(OutFile,"%c",SPACE);
	    else if ( (Start+Pos)%10 == 0 ) {
	      fprintf(OutFile,"%c",'|');
	      Pos++;
	    } else {
	      fprintf(OutFile,"%c",'.');
	      Pos++;
	    }
	    CharPos++;
	  }
  	  fprintf(OutFile, "\n");

/* 
** Write the number line. First write EOS into OutLine, then write the
** number at the appropriate place. Then change any leftover EOS's to 
** spaces.  Finally, truncate the line.
*/
	  for (i=0; i<=255; i++)
	    OutLine[i] = EOS;

	  n = ((Start+9)/10)*10;
	  Step = 10;

	  while( n<Start+LineSize ) {
	    sprintf(OneNum,"%d",n);
	    CharPos = n - Start + (n-Start)/BlockSize - strlen(OneNum);
	    strcpy(&OutLine[CharPos+13], OneNum);
	    n += Step;
	  }

	  for (i=0; i<=254; i++)
	    if ( OutLine[i] == EOS) OutLine[i] = SPACE;

	  StrTruncate(OutLine);
	  fprintf(OutFile, "%s\n",OutLine);

	  Block++;
	  if ( Block >= BlocksPerPage ) {
	    fprintf(OutFile,"%c\n",CNTRLL);
	    Block = 0;
	  }

	  Start += LineSize;
	}

	fclose(OutFile);
	sprintf(OutLine," Aligned sequences written to \"%s\".",OutFName);
	ShowError(OutLine);

}  /* End of DoHardCopy */

/****  Plurality  ********************************************************
**
**  Given a range generate the plurality sequence.
**
*************************************************************************/

void Plurality(int From, int To, char *PlurSeq )

{
char PlurBase;
int NumOfSeqs, Unique, MaxCount, Count[122], Temp;
int Pos, i, t, ii;

/*-------------------------------------------------------------------
**
**  Generate the Plurality for the block
*/

	    i = 0;
	    t = To - From;
	    while ( i <= t ) {
	      for (ii=65; ii<=90; ii++)
	        Count[ii] = 0;
	      Count[45]=0;

	      Pos = From + i;
	      NumOfSeqs = 0;
	      for (ii=1; ii<=NOS; ii++) {
	        if ( OkToEdit[ii] ) {
	          if ( Pos  > Seq[ii].Offset &&
	               Pos <= Seq[ii].Length+Seq[ii].Offset ) {
	            Temp = toupper(Seq[ii].Strand[Pos-Seq[ii].Offset]);
	            Count[Temp]++;
	            NumOfSeqs++;
	          }
	        }
	      }

	      PlurSeq[i] = ' ';
	      if ( NumOfSeqs == 0 ) goto _999;  /* No sequences, we're done */
	      PlurSeq[i] = '.';
	      if ( NumOfSeqs < Threshold ) goto _999;   /* Less than 2, no plurality */

	      MaxCount = Count[45];
	      PlurBase = 45;
	      Unique = MaxCount ? 1 : 0;
	      for ( ii=65; ii<=90; ii++) {
	        if ( Count[ii] > MaxCount ) {
	          MaxCount = Count[ii];
	          PlurBase = ii;
	          Unique = 1;
	        } else if ( Count[ii] == MaxCount ) {
	          Unique++;
	        }
	      }

	      if ( Unique == 1 ) PlurSeq[i] = PlurBase;
_999:	      i++;
	    }

}  /* End of Plurality */

/****  ReadInit  ************************************************************
**
**  This reads the MSE.INI file which allows the user to customize
**  the translation of keystrokes when sequences are entered.
**
**  Here is a simple MSE.INI file:
**
**		this is the seqinit file format.  Coments can go here!
**
**		..
**		Change a into g    ! these define key mapping for entry
**		Change f into c
**		Change t into -
**		Change g into ^H   ! control chars look like this
**
**************************************************************************/

void ReadInit()
	
{      

char OneLine[256], InChar, OutChar;
char *cPos, *tPos;
int i;
FILE *InFile;

/* 
** If no remapping file is found use the "PLAIN" array for protein and/or
** no remapping.  PLAIN has been defined in HLIB:MSE.H
*/

	if ( (InFile = FindDataFile("MSE.INI")) == NULL )
	  return;

/* 
**  Remap the keys in array "XFORM" according to the contents of SeqEdInit.TXT
**  First set everything to invalid.
*/

	for (i = 0; i<=127; i++)
	   CharMap[XFORM][i] = 255;

/*
** Use UWGCG-like file i.e. header comments seperated from data by
** using double dots.  If missing, don't cry, just start reading 
** data from the top.
*/
	
	while ( fgets(OneLine, 255, InFile) )
	  if ( StrIndex("..", OneLine) ) break;

	if ( feof(InFile) ) rewind(InFile);

/*
** Parse remapping lines, check to make sure all the parts are correct.
*/

	while( fgets(OneLine, 255, InFile) ) {
	  if ( StrIsBlank(StrUnComment(OneLine)) ) continue;
	  StrCompress(OneLine);

	 /* Get the first word which should be "CHANGE". */

	  tPos = OneLine;
	  if ( cPos = strchr(tPos, ' ') ) *cPos = '\0';	else continue;
	  StrToUpper(tPos);
	  if ( strcmp(tPos, "CHANGE") != 0 ) continue;

	 /* Get the next character which will be the keystroke to change */

	  tPos = cPos + 1;
	  if ( cPos = strchr(tPos, ' ') ) *cPos = '\0';	else continue;
	  InChar = *tPos;
	
	 /* Skip the next word, should be "INTO" */

	  tPos = cPos + 1;
	  if ( cPos = strchr(tPos, ' ') ) *cPos = '\0';	else continue;
	  StrToUpper(tPos);
	  if ( strcmp(tPos, "INTO") != 0 ) continue;

	 /* Get the fourth and final word */

	  tPos = cPos + 1;
	  if ( *tPos == '^' )
	    OutChar = *(tPos+1) - 64;
	  else
	    OutChar = *tPos;

	  if ( OutChar == BACKSPACE ) OutChar = DEL;
	  CharMap[XFORM][InChar] = OutChar;

	}
	CharMap[XFORM][127] = DEL;    /* Always map <DEL> to Delete key*/
	CharMap[XFORM][45]  = '-';   /*  and gap to dash key.          */

	fclose(InFile);

}  /* End of ReadInit */

/****  DoFind  ***************************************************************
**
**  This locates the specified pattern in the sequence. The current 
**  search pattern is set to this string so find next can be used.
**  If string is of zero length then we use the last search pattern.
**  moves the cursor to a specific position in the sequence.
**
******************************************************************************/

void DoFind(int Start, int Finish, char *Pattern )

{
  char *Loc;
 char OneLine[132]/*, ErrMsg[132]*/;
 int s,f,i;

	if ( StrIsBlank(Pattern) ) {
          ShowText("Target pattern: ");
	  getstr(OneLine);
	  if (StrIsBlank(OneLine) ) strcpy(OneLine,FindSeq);
	} else
	  strcpy(OneLine, Pattern);

	if ( StrIsBlank(OneLine) ) return;
	strcpy(FindSeq, OneLine);

	if ( Start == NOADDR ) Start = Strand;
	if ( Finish == NOADDR ) Finish = Strand;

	s = LIMIT(1,AJMIN(Start,Finish),NOS);
	f = LIMIT(1,AJMAX(Start,Finish),NOS);

	for ( i=s; i<=f; i++ ) {
	  if ( OkToEdit[i] == false ) continue;
	  if ( i == s && s == Strand ) {
	    if ( Loc = StrIndex(OneLine, &Seq[i].Strand[SeqPos+1]) ) {
	      LinePos = Seq[i].Offset + (Loc - &Seq[i].Strand[1]) + 1;
	      return;
	    }
	  } else if ( Loc = StrIndex(OneLine, &Seq[i].Strand[1]) ) {
	    LinePos = Seq[i].Offset + (Loc - &Seq[i].Strand[1]) + 1;
	    Strand = i;
	    return;
          }
	}

	/* Generate the revserse-complement of the Pattern and try again */	

	RevComp(OneLine, Seq[1].Type);

	for ( i=s; i<=f; i++ ) {
	  if ( OkToEdit[i] == false ) continue;
	  if ( Seq[i].Type < DNA ) continue;
	  if ( i == s && s == Strand ) {
	    if ( Loc = StrIndex(OneLine, &Seq[i].Strand[SeqPos+1]) ) {
	      LinePos = Seq[i].Offset + (Loc - &Seq[i].Strand[1]) + 1;
	      return;
	    }
	  } else if ( Loc = StrIndex(OneLine, &Seq[i].Strand[1]) ) {
	    LinePos = Seq[i].Offset + (Loc - &Seq[i].Strand[1]) + 1;
	    Strand = i;
	    ShowError(" Rev-comp of pattern found.");
	    return;
          }
	}

	ShowError("Pattern not found.");

} /* End of DoFind */

/****  DoHelp  **************************************************************
**
**  for "Help" type out in a page-wise fashion.  Help messages moved from
**  files in MITDATA to the program in order to make the distribution of
**  MSE easier.
**
**  Each page has 22 lines and has been declared to be 85 characters wide.
**  I sure that there is a much better way to implement this.
**
****************************************************************************/

void DoHelp(int Start) 
{

  int i=0, Page=0, Pages=10;
  unsigned int c;
  AjPStr whereto = NULL;
  AjBool okay = ajTrue;

  static char HelpMsg[10][22][85] = {
{  /* Page One */ 
"                          Screen Mode Commands,",
"             ([n] is an optional numeric (keyboard) parameter)",
"",
"[n]s            - Inserts symbol \"s\" [n] times",
"[n]<delete>     - Delete [n] sequence character(s)",
"[n]<space bar>  - Move the sequence(s) to the right [n] spaces",
"[n]<backspace>  - Move the sequence(s) to the left [n] spaces, VT200's",
"                   use F12 key or Control-H",
"",
"[n]<arrow>      - Go forward, backward, up or down [n] positions or lines",
"[n]<return>     - Go to position [n], also use keypad ENTER key",
"   *            - Go to end of current sequence (Also Control-E)",
"[n]<            - Go [n]x50 back, VT200's also use PREV SCREEN key",
"[n]>            - Go [n]x50 ahead, VT200's also use NEXT SCREEN key",
"",
"   /string<rtn> - Find \"string\", VT200's also use FIND key",
"[n]?            - Display help starting at page [n], VT200's also use HELP key",
"   !            - Display status of current sequence, same as \"SEQ\" command",
"   Control-W    - Redraw the screen (also Control-R)",
"   Control-Z    - Switch to Command Mode",

"",
""
}/*,{ 
"                          Screen Mode Commands, Keypad",
"              Keypad keys are very similar to those used by VMS EDT. ",
"                 ([n] is an optional numeric (keyboard) parameter)",
"",
"  [n] KP0   -  Move to beginning of next strand",
"  [n] KP1   -  Move by 10 symbols.",
"  [n] KP2   -  Move to the next end of sequence.",
"  [n] KP3   -  Move to the next symbol.",
"      KP4   -  Set movement to forward.",
"      KP5   -  Set movement to backward.",
"  PF1 KP5   -  Move to highest numbered avaliable strand.",
"      KP6   -  \"Cut\" selection portion of strand.",
"  PF1 KP6   -  \"Paste\" previously cut portion of strand.",
"      KP7   -  \"Page\" left and right by screenfulls.",
"  PF1 KP7   -  Enter command mode, same as ^Z and DO key",
"      KP8   -  \"Section\" up and down by screenfulls.",
"      KP.   -  Start Selection.",
"  PF1 KP.   -  Cancel selection.",
"",
"      PF1   - Gold key.   PF2   - Help key.  PF3   - Find key.",
"      PF4   - Toggle Anchored/NoAnchored for current strand.",
"  PF1 PF4   - Toggle Locked/NoLocked for current strand."
}*/,{  /* Page Three */
"     Commands end with <ret>, [ ] indicates an optional parameter.",
"     \"s\" and \"f\" represent sequence positions for start and finish.  ",
"     \"attr\" may be Blink, Reverse, Underline.  Bold is the Default.",
"     Only the capitalized part of the command is necessary",
"",
"      EDIt [SeqSpec(s)] - edit another sequence(s) or indirect FOSS file",
"",
"      FInd string       - find the next occurrence of \"string\"",
"[s,f] DELete            - delete a range of symbols or answer prompts",
"                          for a range, default will be: current to *",
"      [n]               - go to position [n], same as raw mode [n]",
"",
"  [s] HELp              - display help starting at page [s]",
"      REDraw            - redraw the screen",
"      MATches [attr]    - Display matches with plurality sequence",
"      DIFferences [attr]- Display differences with plurality sequence",
"      NEIther           - Turn off plurality sequence calculation",
"                           Saves CPU time!!",
"",
"      ALNED [filename]  - Read in an alignment file from the NBRF/PIR program",
"                           ALNED (A multiple sequence alignment program)."
},{  /* Page Four */
"     Commands end with <ret>, [ ] indicates an optional parameter.",
"     \"s\" and \"f\" are line numbers for to and from or start and finish.",
"     Only the capitalized part of the command is necessary",
"",
"  [s,f] MOVe       - Move from line [s] to line [f].",
"                      Default is from current to next available line.",
"  [s,f] ELIminate  - Delete lines [s] to [f]. Default is current line.",
"",
"  [s,f] ANChor     - Force lines [s] to [f] to move as one unit.",
"                      Default is to add the current strand to the anchored",
"                      group. PF4 key toggles anchoring.",
"        NOAnchor   - Turn off anchoring. Default is the current strand.",
"                      PF4 toggles anchoring",
"",
"  [s,f] SORt [dir] - Sort lines [s] to [f] according to their \"Offsets\".",
"                      Default is from current to highest numbered line.",
"                      If \"dir\" is DEScending, the sort is reversed.",
"",
"  [s,f] DEGap      - Remove gaps from lines [s] to [f].",
"",
""
},{  /* Page Five */
"     Commands end with <ret>, [ ] indicates an optional parameter.",
"     \"s\" and \"f\" are line numbers for to and from or start and finish.",
"     Only the capitalized part of the command is necessary",
"",
"        Open    - Open up a new line by moving all the line above the current",
"                   line up by one",
"",
"  [s,f] Lock    - Lock lines \"s\" to \"f\" such that you cannot add or delete",
"                   sequence symbols. You may still shift and/or reverse lines.",
"                   You may also add/delete gap characters \"-\".  Default is the",
"                   current line.",
"",
"  [s,f] Unlock  - Unlocks locked lines.",
"",
"    [s] Offset  - Set the Offset of the current line to position [s].",
"                   Default is set the Offset of the current line to the",
"                   current line position. Also works with anchored groups.",
"",
"    [s] REVerse - Reverse/complement line \"s\".  The current line is the defualt",
"                   If the reversed line is part of an anchored group then the",
"                   entire group is reversed.",
""
},{  /* Page Six */
"     Commands end with <ret>, [ ] indicates an optional parameter.",
"     Only the capitalized part of the command is necessary",
"",
"  SEQ                  - Show current position. Same as \"!\" in raw mode.",
"",
"  NAMe [code=filename] - Change the code and/or filename.",
"",
"  TItle [New Title]    - Change the sequence title.",
"  TYpe [type]          - Change the sequence type, values for TYPE:",
"",
"                             DNA     PROTEIN     RNA",
"                            rRNA    tRNA        uRNA        mRNA",
"",
"  FORmat [New format]  - Change the sequence format, for FORMAT:",
"",
"                          GCG PIR FASTA SWISS NCBI etc..",
"",
"",
"",
"",
"",
""
},{  /* Page Seven */
"",
"     Commands end with <ret>, [ ] indicates an optional parameter.",
"     s and f represent numbers for start and finish.  Only the",
"     capitalized part of the command is necessary",
"",
"[s,f] SELect          - Mark a range [s] to [f] to be selected, or start",
"                        selection at current cursor position.",
"                        VT200's also use SELECT key.",
"",
"      REMove          - Remove a selected, (Reverse video), region. Does not",
"                        include current cursor position.",
"                        VT200's also use REMOVE key.",
"",
"  [s] INSert          - Insert cut region at [s] or current cursor position.",
"                        VT200's also use INSERT HERE key.",
"",
"      CANcel          - Cancel SELECT function. Clears PASTE buffer.",
"",
"",
"",
"",
""
},{  /* Page Eight */
"     Commands end with <ret>, [ ] indicates an optional parameter.",
"     \"s\" and \"f\" represent sequence positions. Only the capitalized part ",
"     of the command is necessary",
"",
"      GelAlign       - Will take all of the strands currently loaded into MSE",
"                        and produce the best alignment based on exact matches.",
"                        GelAlign does not add gaps but it will get the strands",
"                        started for you",
"",
"[s,f] Meld [seqname] - Merges strands \"s\" to \"f\" to create a new entry",
"                        named \"seqname\".  If the current strand is part of an",
"                        anchored group then that group will be melded.  You",
"                        will be prompted to continue if undetermined postitions",
"                        are found in the melded sequence.",
"",
"",
"",
"",
"",
"",
"",
""
},{  /* Page Nine */
"     Commands end with <ret>, [ ] indicates an optional parameter.",
"     \"s\" and \"f\" represent sequence positions. Only the capitalized part ",
"     of the command is necessary",
"",
"[s,f] HARdcopy [filename] - write [a part of] aligned sequences to a file.",
"[s,f] WRIte [seqname]     - write [a part of] the sequence to a file. You",
"                             may rename it to \"seqname\" in the process",
"      EXIt  [FOSSname]    - write any modified sequences, a new FOSS file,",
"                             and then quit.",
"      QUIt                - quit the editor, do not write sequences.",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
""
},{  /* Page Ten */
"                        Release Notes for MSE V2.1",
"",
" 1.  Read/Writes Staden, Raw, GCG, NBRF and IG user sequence formats. Default",
"     is set with system logical \"MITSeqFormat\".",
"",
" 2.  Dropped the INLCUDE command, use EDIT and then Cut/Paste what you need.",
"",
" 3.  Added the MELD command for collapsing groups into a new sequence.  The",
"     group may either be specified as a range or the currently anhored group.",
"",
" 4.  The keypad now functions in a very similar way as VMS EDT.  Note that",
"     \"PAGE\" was slightly redefined to move left/right by screenfulls. \"SECTION\"",
"     still moves up and down.",
"",
" 5.  PF4 in VMS EDT deletes the current line.  This key was not implemented in",
"     that manner as it would be a little used feature would cause lots of ",
"     problems.  Hence, it acts a toggle for the Anchor/NoAnchor making the",
"     use of anchored group easier.",
"",
" 6.  Some keypad keys, KP6, KP- and KP, have not yet been implemented.",
"",
""
},{  /* Page Eleven */
"                   Release Notes for MSE V2.1, continued",
"",
" 7. When using MELD, MSE will suggest a sequence name based on the left",
"    most strand.  This may not always be the best name to use, especialy",
"    when doing sequencing projects.  Try things like MELD1.MLD.",
"    ",
" 8. FIND was implemented very quickly and only does exact matches, that is,",
"    upper case and lower case are considered to be different.",
"",
" 9. On the brighter side, FIND will now search all of the strands and, if no",
"    match is found, will search for the reverse complement of the pattern. ",
"    Proteins are skipped during reverse searching.",
"",
"",
"",
"",
"",
"",
"",
"",
"",
""
}};
/*--------------------------------------------------*/

 if ( Start == NOADDR ) Start = 1;

 Page = LIMIT(1, Start, Pages);

 /*	savescreen(); il NEED still */
 ajStrAssS(&whereto,tmpdir);
 ajStrAppC(&whereto,"virtual.dump");
 if(scr_dump(ajStrStr(whereto))==ERR) {
   ajDebug("scr_dump to %S ERR \n",whereto);
   /* problem storing so try ./ */
   ajStrClear(&whereto);
   ajStrAppC(&whereto,"/tmp/virtual.dump");
   if(scr_dump(ajStrStr(whereto))==ERR) {
     ajDebug("scr_dump to %S ERR \n",whereto);
     ajStrClear(&whereto);
     ajStrAppC(&whereto,"/tmp/virtual.dump");
     if(scr_dump(ajStrStr(whereto))==ERR) {
       ajDebug("scr_dump to %S ERR \n",whereto);
       okay = ajFalse;
     }
     else 
       ajDebug("scr_dump to %S 0kay \n",whereto);
   }
   else
     ajDebug("scr_dump to %S 0kay \n",whereto);
 }
 else 
   ajDebug("scr_dump to %S 0kay \n",whereto);

 ajDebug("Start = %d Page = %d\n",Start,Page);
 while( Page <= Pages ) {
   clear();
   for (i=0; i<22; i++)
     printw("%s\n",HelpMsg[Page-1][i]);
     printw("\n     [Help page %d of %d.  Press RETURN for more help. SPACE to quit.] ",
	  Page, Pages);
   c = getch();;
   if ( (char)c != '\n' ) break;
   Page++;
 }
 
 /* restorescreen(); il NEED still */

 /*clear(); NO IDEA WHY CLEAR MESSES UP RESTORE BEFORE IT'S CALLED ???? */
 if(okay){
   if(scr_restore(ajStrStr(whereto))==ERR)
     ajDebug("scr_restore ERR \n");
   else
     ajDebug("scr_restore 0kay \n");
 
   ajStrInsertC(&whereto,0,"rm ");
   ajSystemEnv(&whereto, envptr);
 }
 ajStrDel(&whereto);

 doupdate();
 /* refresh();*/
 /*    c = getch();*/
 /*DoRedraw();*/
 
} /* End of DoHelp */

/****  DoRedraw  *************************************************************
**
**  redraws the screen for in the event of foreign I/O.
**
****************************************************************************/

void DoRedraw()
{
  /*repaint();*/
  /*  redrawwin(Display);*/
  doupdate();
  refresh();
} /* end of DoRedraw */

/****  CleanUp  **************************************************************
**
**  Common exit point for program, clears screen and stops.
**
*****************************************************************************/

void CleanUp()
{
	endwin();
	exit(0);

} /* End of cleanup */

/****  ShowText  ************************************************************
**
** put text on the next-to-the-bottom line of the screen.
**
***************************************************************************/

void ShowText(char *OneLine)

{
	move(MARK2+4,0);
	clrtoeol();
	refresh();
	addstr(OneLine);
	refresh();

} /* End of ShowText */

/****  ShowError  ***********************************************************
**
**  puts text at the bottom of the screen, if first character of the text
**  is not blank then the bell is also rung.  Use blank string to clear
**  the area.  The global flag "Dirty" is used to indicate that the Error
**  line contains text.
**
*****************************************************************************/

void ShowError(char *OneLine)

{
	move(MARK2+5, 0);
	clrtoeol();	addstr(OneLine);
	refresh();
	/*	if ( OneLine[0] != ' ' ) addch(BELL);*/
	Dirty = !StrIsBlank( OneLine );
	ajDebug("SHOWERROR: %s\n",OneLine);

}  /* End of ShowError */

/****  MakeScreen  **********************************************************
**
**  Makes sequence and global scales, label screen
**
***************************************************************************/

void MakeScreen(void)

{
	mvaddstr(0,0,"MSE V3.0");
	mvaddstr(0,(__Cols-4), "WIBR");

	DrawScale(  MARK1+1, 0, __Cols-1, 10, 4);
	NumberScale(MARK1+2, 0, __Cols,   10, 4, 0, 10);
	DrawScale(  MARK2+1, 6, __Cols-9,  7, 0);
	NumberScale(MARK2+2, 6, __Cols-9,  7, 0, 0, ScaleLen/10);
	refresh();

}  /* End of MakeScreen */

/****  DrawScale  ***********************************************************
**
**  Draws a simple scale on the screen for use with on screen editors
**
***************************************************************************/

void DrawScale(int Row, int Col, int Width, int Divs, int Offset)

{
int Count;

	move(Row, Col);
	clrtoeol();
	
	for ( Count=0; Count < Width; Count++)
	  if ( (Count-Offset)%Divs ) {
	    addch('.');
	  } else 
	    addch('|');

}  /* End of drawscale */

/****  NumberScale  ********************************************************
**
**  number a horizontal scale on the screen. Position the cursor before
**  a call to this function.
**
***************************************************************************/
	
void NumberScale(int Row, int Col, int Width, int Divs,
                 int Offset, int Start, int Step)

{
int n, pos;
char OneNum[20];
/*-------------------------------------------*/
        sprintf(OneNum,"%d",0);
	n = Start;
	pos = Offset;
	move(Row, Col);
	clrtoeol();
	while (pos+strlen(OneNum) < Col+Width) {
	  sprintf(OneNum,"%d",n);
	  mvaddstr(Row, Col+pos-(strlen(OneNum))/2, OneNum);
	  n += Step;
	  pos += Divs;
	}

}  /* End of NumberScale */

/*** Get_SeqSpec  **************************************************************
**
**  Special version of "Get_SeqSpec" from the general library for use wiht
**  the Curses package.
**
**  Returns a pointer to the String SeqSpec
**
*******************************************************************************/
/*
SeqSpec *Get_SeqSpec(char *Line, SeqSpec *Spec, char *Prompt, char *DCLSymbol)

{
char Temp[132];
char Temp2[132];
char *temp3;
Boolean NoPrompt, NoSymbol;

??-----------------------------------------------------------------
** First check to see if we have not exceeded out allowed tries.
** Then determine if we have a prompt and if there is a global
**  symbol to be used as a default
??
 ajDebug("Line = %s (%d)\n",Line,strlen(Line));
 if ( StrIsBlank(Line) ) {
   temp3 = getenv(DCLSymbol);
   if(temp3){
     strcpy(Line,temp3);
     sprintf(Temp, "%s [%s] ?  ", Prompt, Line);
   }
   else
     sprintf(Temp,"%s ?  ", Prompt);
   
   ShowText(Temp);
   getstr(Temp);
   if ( StrIsBlank(StrCollapse(Temp)) ) strcpy(Temp, Line);
 } 
 else
   strcpy(Temp,Line);

 sprintf(Temp2,"%s=%s",DCLSymbol,Temp);
 ?? SetDCLSymbol(DCLSymbol, Temp);??
 putenv(Temp2);
 strcat(Temp,"!NoText");
 ajDebug("Temp = %s DEfaultDB=%s\n",Temp,DefaultDB);
 MakeSeqSpec(Spec, Temp, DefaultDB);
 ajDebug("Temp = %s DEfaultDB=%s\n",Temp,DefaultDB);
 
 if ( Spec->isUser == false && StrIsBlank(Spec->file) )
   strcpy(Spec->file, DefaultDB);
 
 strcpy(Line,Temp);
 ajDebug("Line = %s (%d)\n",Line,strlen(Line));
 return(Spec);
}
*/
/**  Get_Integer  *********************************************************
**
**  This subroutine performs input of integer values.  The integer
**  is truncated if it is read and must be in the range of Low to
**  High.
**
*************************************************************************/

int Get_Integer(char *Prompt, int Val, int Low, int High )

{
char PStr[255], String[80], *cPos;
int i, Sign;

/*----------------------------------------------------*/

	sprintf(PStr,"%s [%d] ? ",Prompt, Val);

Try:	ShowText(PStr);
	getstr(String);

	if (StrIsBlank(StrCollapse(String))) return(Val);

	Sign = 1;
	if ( String[0] == '+' || String[0] == '-')
	  Sign = (String[0]=='+') ? 1: -1;

	if ( cPos = strchr(String,'.') ) *cPos = '\0';

	Val = 0;
	for ( i=0; String[i]; i++ )
	  if ( isdigit(String[i]) )
	    Val = (Val * 10) + (String[i] -'0');

	Val *= Sign;

	if ( (Val < Low ) || (Val > High) ) {
	  ShowText("");
	  sprintf(PStr,"Out of range [%d:%d]. Try again; %s ? ",
	    Low, High, Prompt);
	  goto Try;
	}

	return(Val);

}  /*  End of Get_Integer */


void ToInternal(int i, SeqEntry *SeqIn)
{
	EdInit(i);

	Seq[i].Code = CALLOC(strlen(SeqIn->spec->code)+1,char);
	strcpy(Seq[i].Code,SeqIn->spec->code);
	Seq[i].File = CALLOC(strlen(SeqIn->spec->file)+1,char);
	strcpy(Seq[i].File,SeqIn->spec->file);
	Seq[i].IsUser = SeqIn->spec->isUser;
	Seq[i].Format = SeqIn->spec->format;
	Seq[i].Name = CALLOC(strlen(SeqIn->name)+1,char);
	strcpy(Seq[i].Name,SeqIn->name);
	Seq[i].Title = CALLOC(strlen(SeqIn->title)+1,char);
	strcpy(Seq[i].Title,SeqIn->title);
	Seq[i].Desc = CALLOC(strlen(SeqIn->desc)+1,char);
	strcpy(Seq[i].Desc,SeqIn->desc);
	Seq[i].Type = SeqIn->type;
	Seq[i].Circular = SeqIn->circular;
	Seq[i].Length = SeqIn->length;
	Seq[i].Mem = SeqIn->mem;
	Seq[i].Size = SeqIn->size;
	Seq[i].Strand = SeqIn->strand;
	Seq[i].ReMap = PLAIN;
	Seq[i].Anchored = false;
	Seq[i].Offset = 0;
	Seq[i].Modified = false;
	Seq[i].Reversed = false;
	Seq[i].Locked = false;

} /* End of ToInternal */

void FromInternal(int i, SeqEntry *SeqOut)
{
SeqSpec *Spec;

	Spec = MALLOC(SeqSpec);

	Spec->code = Seq[i].Code;
	Spec->file = Seq[i].File;
	Spec->isUser = Seq[i].IsUser;
	Spec->format = Seq[i].Format;

	SeqOut->name = Seq[i].Name;
	SeqOut->title = Seq[i].Title;
	SeqOut->desc = Seq[i].Desc;
	SeqOut->type = Seq[i].Type;
	SeqOut->circular = Seq[i].Circular;
	SeqOut->length = Seq[i].Length;
	SeqOut->mem = Seq[i].Mem;
	SeqOut->size = Seq[i].Size;
	SeqOut->strand = Seq[i].Strand;
	SeqOut->text = NULL;
	SeqOut->tSize = 0;
	SeqOut->spec = Spec;

} /* End of FromInternal */

/* End of MSE.c */
