/***  ReadEntry.c  *************************************************************
**
**  ((THINK C 4.0 incompatablity read() and O_RDNLY))
**
**  This file contains a suite of related routines which find and retrieve
**  sequences from databases and user-entry files.
**
**  Database files:
**     The program first looks for PIR index (.INX) files.  If not found,
**     then it look for GCG names and offset files (.NAMES & .OFFSET).
**
**  User-entry files:
**     The specification of the format of user-entry files is obtained from
**     the VMS symbol "MITSeqFormat" or from a command line modifier attached
**     to sequence specification
**
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "ckittypes.h"
#include "seqspec.h"
#include "seqentry.h"
#include "seqsymbols.h"
#include "macros.h"

/*
** Global variables, also used by NextSeqEntry.c
*/

char headerLine[513];              /* Used by both ReadEntry and NextSeqEntry */
FILE *userFile;                    /* Currently open user file             */

ProcPtr SetDBPointers = NULL;
ProcPtr SeekSeq = NULL;
ProcPtr SeekRef = NULL;

extern FILE *seqFile, *refFile;           /* Core database files, Sequence and Text */
extern int dbSEQFile, dbREFFile;          /* files desc numbers of above file pointers */


/*
** Useful functions used by this file.
*/

extern     char *StrToUpper(char *);                       /* Strings.c  */
extern     char *StrToLower(char *);                       /* Strings.c  */
extern     char *StrCollapse(char *);                      /* Strings.c  */
extern     char *StrTrim(char *);                          /* Strings.c  */
extern     char *StrIndex(char *,char *);                  /* Strings.c  */
extern  Boolean  StrIsBlank(char *);                       /* Strings.c  */
extern     char *StrChange(char *, char, char);            /* Strings.c  */
extern     void  PostError(int, char *);                   /* Error.c    */
extern SeqEntry *NewSeqEntry(void);                        /* SeqEntry.c */
extern     char *MakeSeqDesc(SeqEntry *seq);               /* SeqEntry.c */
extern     char *DNAtoRNA(char *);                         /* Sequence.c */
extern     char *RNAtoDNA(char *);                         /* Sequence.c */
extern     char *NucToProtein(int, char *, long *);        /* Sequence.c */
extern      int  CheckSum(char *);                         /* Sequence.c */
extern      int  CompBase(char);                           /* Sequence.c */
extern     char *DePath(char *);                           /* VMS.c      */

/*
**  Function declarations and prototypes for this file.  This file has only
**  one public routines, "ReadEntry", all others are used only within this file.
*/

SeqEntry *ReadEntry(SeqSpec *spec);

/* private methods */
static Boolean ReadTxt(SeqSpec *spec, SeqEntry *seq);
static Boolean FindSeqSpec(SeqSpec *spec, SeqEntry *seq);
static Boolean Fragment(char *frag, SeqEntry *seq);

/******************************************************************************/
/***  ReadEntry  **************************************************************
**
**  Read the sequence from the current position in the file, we got to
**  the correct line in the file via FindSeqSpec.
**
**  This routine returns "true" if the sequence was properly read, otherwise
**  "false" is returned.
**
**  William Gilbert, Whitehead Institute
**
*****************************************************************************/

SeqEntry *ReadEntry(SeqSpec *spec)

{
SeqEntry *seq;
char oneLine[512];
char comChar, *cPos, *cPtr;
char *temp, *pTemp;
char *pPos;
int seqChar, tempSize;
int count,n, sgCode;
int a,c,g,t,u;
FILE *file;

/*
** Instance a new SeqEntry.
*/
	seq = NewSeqEntry();
	seq->spec = spec;

/*
** Position read pointer in either the Database file(s) or in the
** user-entry file. If "FindSeqSpec" has an error i.e. invalid format
** the deallocate memory for the SeqEntry and return a null pointer.
*/

	if ( FindSeqSpec(spec, seq) == false ) {
	  seq->spec = NULL;
	  DeleteSeqEntry(seq);
	  return(NULL);
	}

/*
** Read lines from sequence file, testing for End of sequence, the
** beginning of a new sequence, or a comment character.  Use the appropriate
** remapping table.
*/

	if ( spec->isUser )
	  file = userFile;
	else             
	  file = seqFile;  /* and set read pointer */

	n = 0;
	count = 0;
	tempSize = 4096;
	if ( (temp = CALLOC(tempSize,char)) == NULL ) {
	  PostError(2,"ReadEntry - Failed to allocate memory."); 
	  return(NULL);
	}
	pTemp = temp;

GetLine:
	while ( fgets(oneLine, 512, file) != NULL ) {
	  if ( n+512 > tempSize ) {
	    tempSize *= 2;
	    if ( (temp = REALLOC(temp,tempSize,char)) == NULL ) {
	      PostError(2,"ReadEntry - Failed to allocate memory."); 
	      return(NULL);
	    }
	    pTemp = temp+n;
	  }

	  cPtr = oneLine;
	  while( seqChar=seqCharSet[(spec->format)-1][(*cPtr)&0x7F] ) {
	    cPtr++;
	    switch (seqChar) {
	      case ENDSEQ:
	        if (spec->format == IG) 
	          seq->circular = (*(cPtr-1)=='2') ? true : false;
	        goto Done;
	      case NEXTENTRY:
	        PostError(1,"End of sequence marker not found.");
	        return(NULL);
	      case NOTSEQ:
	        break;
	      case IGNORE:
	        goto GetLine;
	      case COMMENT:
	        if ( spec->format == GCG ) {
	          comChar = *(cPtr-1);
	          while ( (cPos = strchr(cPtr,comChar)) == NULL ) {
	            if ( fgets(oneLine, 512, file) == NULL ) {
	              PostError(1,"Unclosed comment in GCG format");
	              return(NULL);
	            }
	            cPtr = oneLine;
	          }
	          cPtr = cPos+1;
	        }
	        break;
	      default:  {
	        *pTemp++ = seqChar;
	        n++;
	        seq->checkSum += ++count * toupper(seqChar);
	        if (count == 57) count = 0;
	      }
	    }
	  }  
	}

/*
** PIR, IG, IBI and STRIDER have "End of Sequence" markers.  
** These formats should branch to Done: and not pass through here unless 
** there is an error
*/

	switch (spec->format) {
	  case PIR:
	  case IG:
	  case IBI:
	  case STRIDER:
	    sprintf(oneLine, "End of File while reading \"%s\"", spec->file);
	    PostError(1,oneLine);
	    return(NULL);
	}

Done:
	*pTemp = '\0';
	seq->size = n+512;
	if ( (seq->mem = REALLOC(temp,seq->size,char)) == NULL ) {
	  PostError(2,"ReadEntry - Failed to allocate memory."); 
	  return(NULL);
	}
	seq->length = n;
	seq->strand = seq->mem-1;
 	seq->checkSum %= 10000;

/*
**  Get Text for this entry after you read the sequence for PIR format 
*/

	if (spec->format == PIR )
	  if ( ReadTxt(spec,seq) == false )
	    return(NULL);

/*
** Process any fragment spec, if the sequence was Circular, set it to Linear 
*/

	if ( spec->frag )
     	  if( Fragment(spec->frag, seq) ) {
	    seq->circular = false;
	    seq->checkSum = CheckSum(seq->mem);
	  } else
	    return (NULL);

/*
** read and process Options flags here 
*/

	if ( spec->options ) {
	  if ( StrIndex("/DEGAP", spec->options) ) {
	    StrChange(seq->mem, '-', ' ');
	    StrCollapse(seq->mem);
	    seq->length = strlen(seq->mem);
	  }
	  if ( StrIndex("/PRO",spec->options) ) {
	    seq->type = PROTEIN;
	  }
	  if ( StrIndex("/DNA", spec->options) ) {
	    RNAtoDNA(seq->mem);
	    seq->type = DNA;
	  }
	  if ( StrIndex("/RNA", spec->options) ) {
	    DNAtoRNA(seq->mem);
	    seq->type = RNA;
	  }
	  if ( StrIndex("/CIR",spec->options) ) {
	    seq->circular = true;
	  }
	  if ( StrIndex("/LIN",spec->options) ) {
	    seq->circular = false;
	  }
	  if ( StrIndex("/TRA",spec->options) ) {
	    StrChange(seq->mem, '-', ' ');
	    seq->length = strlen(StrCollapse(seq->mem));
	    strcpy(oneLine, StrIndex("/TRA",spec->options)+1);
	    if ( pPos = strchr(oneLine,'/') ) *pPos = '\0';
	    if ( pPos = StrIndex("=SGC",oneLine) )
	      sgCode = (*(pPos+4))-48;
	    else if ( pPos = StrIndex("(SGC", seq->title) )
	      sgCode = (*(pPos+4))-48;
	    else 
	      sgCode = 0;
	    NucToProtein(sgCode, seq->mem, &(seq->length));
	    seq->type = PROTEIN;
	  }
	  if ( StrIndex("/UPP", spec->options) ) {
	    StrToUpper(seq->mem);
	  }
	  if ( StrIndex("/LOW", spec->options) ) {
	    StrToLower(seq->mem);
	  }
	  seq->checkSum = CheckSum(seq->mem);
	}

/*
**  If the sequence type is still undefined.  Try to nail it down using
**  Bill Pearson's rule of "85% ACGT/U then it's DNA".
*/

	if ( seq->type == UNDEF ) {
	  a = c = g = t = u = n = 0;
	  for (cPos = seq->mem; *cPos; cPos++) {
	    n++;
	    switch (toupper(*cPos)) {
	      case 'A': a++; break;
	      case 'C': c++; break;
	      case 'G': g++; break;
	      case 'T': t++; break;
	      case 'U': u++; break;
	      case '-': n--; break;  /* Don't count gaps */
	    }
	  }
	  if ( ((a+c+g+t+u)*100)/n > 85) 
	    if ( t > u )
	      seq->type = DNA;
	    else
	      seq->type = RNA;
	  else
	    seq->type = PROTEIN;
	}


	MakeSeqDesc(seq); 

/*
** Application or user does not want the sequence or text. Deallocate them here
** The sequence must be read however in order to determine the sequence
** description and title etc. In most cases, the text also must be read in order
** to position the read pointer for the sequence in a user-entry file. 
**
** N.B. --  Using /NOSEQUENCE or /NOTEXT will not speed up the retreival
** of a sequence entry!!
*/

	if ( StrIndex("/NOSEQ",spec->options) ) {
	  dispose(seq->mem);
	  seq->strand = seq->mem = NULL;
	  seq->length = seq->size = 0;
	}

	if ( StrIndex("/NOTEX",spec->options) ) {
	  dispose(seq->text);
	   seq->tSize = 0;
	}

	return (seq);

} /* End of ReadEntry */



/******************* P R I V A T E    M E T H O D S  ************************/
/****************************************************************************/
/****  FindSeqSpec  *********************************************************
**
**  Opens a sequence file for reading, and sets the read pointer at the
**  the beginning of the entry.  Returns true if successful.
**
**   Will Gilbert, Whitaker College, MIT
**   Copyright (C) October 1986
****************************************************************************/

static Boolean FindSeqSpec( SeqSpec *spec, SeqEntry *seq)
{
char line[512];
static char name[256] = "";
static char title[256] = "";
char errMsg[80];
char *cPos;


/*-----------------------------------------------------------------------
*/
                                                    
	if ( spec->isUser ) {  /*----- USER sequence files --------*/

	  if( userFile == 0 ) {
	    PostError(1,"User File not found");
	    return(false);
	  }

/*
** Assign a title to the structure. For PIR it's easy.  For the other's
** Make up something based on the Format and user-file name.
*/

	  switch (spec->format) {
	    case  PIR:
	      if ( fgets(title, 256, userFile) ) {
	        if ( cPos = strchr(title, '\n') ) *cPos = '\0';
	      } else {
	        strcpy(errMsg,"Code not found in user-entry file");
	        PostError(1,errMsg);
	        return (false);
	      }
	      break;
	    case IG:
	      if ( ReadTxt(spec,seq) == false ) return(false);
	      if ( fgets(title, 256, userFile) == NULL ) {
	        PostError(1,"EOF reached in IG format file.");
	        return(false);
	      }
	      sprintf(title, "IG format file: %s", DePath(spec->file));
	      break;
	    case RAW:
	      if ( ReadTxt(spec,seq) == false ) return(false);
	      sprintf(title,"Raw format file: %s", DePath(spec->file));
	      break;
	    case STADEN:
	      ReadTxt(spec,seq);
	      sprintf(title,"Staden format file: %s", DePath(spec->file));
	      break;
	    case STRIDER:
	      ReadTxt(spec,seq);
	      sprintf(title,"DNA Strider format file: %s", DePath(spec->file));
	      break;
	    case GCG:
	      if ( ReadTxt(spec,seq) == false ) return(false);
	      sprintf(title,"GCG format file: %s", DePath(spec->file));
	      break; 
	    case IBI:
	      if ( ReadTxt(spec,seq) == false ) return(false);
	      fgets(line, 256, userFile);
	      strcpy(title,&line[6]);
	      StrTrim(title);
	      break; 
	  }

	} else {   /*----------------  DATABASE Entries  ------------*/

	  if( (*SetDBPointers)(spec) ) {
	      fgets(headerLine, 512, seqFile);
	      if ( cPos = strchr(headerLine, '\n') ) *cPos = '\0';
	      fgets(title, 512, seqFile);
	      if ( cPos = strchr(title, '\n') ) *cPos = '\0';
	  } else
	    return (false);

	}

/*  Make up a codeword based on the filename for those formats
**  which don't use codewords.  Do it now so all other program
**  won't have to.
*/

	switch ( spec->format ) {
	  case RAW:
	  case GCG:
	  case STADEN:
	  case IBI:
	  case STRIDER:
	   strcpy(spec->code, spec->file);
	   DePath(spec->code);
	   if ( cPos = strchr(spec->code,'.') ) *cPos = '\0';
	}

/*
** Set the flags in the "Seq" structure
*/

	seq->type = UNDEF;
	switch ( spec->format ) {
	  case PIR:
	    if ( spec->isUser )
	      sprintf(name,"%s=%s",spec->code,DePath(spec->file));
	    else
	      sprintf(name,"%s:%s",spec->file,spec->code);

	    seq->circular = ( headerLine[2] == 'C' );
	    if ( headerLine[1] == 'D' ) seq->type = DNA;
	    if ( headerLine[1] == 'R' ) seq->type = RNA;
	    if ( seq->type ) break;

	    if ( strncmp(&headerLine[1], "P1", 2) == 0 ) 
	      seq->type = PROTEIN;
	    else if ( strncmp(&headerLine[1], "F1", 2) == 0 ) 
	      seq->type = FRAGMENT;
	    else if ( strncmp(&headerLine[1], "N1", 2) == 0 )
	      seq->type = RRNA;
	    else if ( strncmp(&headerLine[1], "N3", 2) == 0 )
	      seq->type = TRNA;
	    break;

	  case IG:
	  case RAW:
	  case STADEN:
	  case GCG:
	  case IBI:
	  case STRIDER:
	  default:
	    seq->type = UNDEF;
	    seq->circular = false;
	    strcpy(name,DePath(spec->file));
	}

/*
** Catch-all on the Sequence TITLE in case a blank title is read
*/

	seq->name = CALLOC(strlen(name)+1,char);
	strcpy(seq->name,name);

	seq->title = CALLOC(strlen(title)+1,char);
	strcpy(seq->title,title);

	return(true);

} /* End of FindSeqSpec */


/**  Fragment ****************************************************************
**
** Assemble a working strand from a Fragment spec and the original strand
**
** This routine makes two passed through the fragement spec, the first to 
** determine how large to make the working strand. The second to actually
** build the strand.
**
** Modelled after the "EXTRACT" command of PSQ from the PIR.
**
**  William A. Gilbert, Whitehead Institute
**
***************************************************************************/

static Boolean Fragment(char *frag, SeqEntry *seq)

{
char *workStrand, *seqPos;
long workSize;
char fSpec[256], fSave[256], startTok[20], stopTok[20];
char *cPos, *pPos, *pFSpec;
int tokens, numTokens;
long workLength, start, stop, k;
Boolean reverse;


	if ( frag == NULL ) return(true);

/*
** Make a copy of the fragment spec. Remove surrounding paren's
*/
	strcpy(fSpec, frag);
	StrToUpper(StrCollapse(fSpec));
	if ( fSpec[0] == '(') {
	  fSpec[0] = ' ';
	  fSpec[strlen(fSpec)-1] = '\0';
	}
/*
** Look for the complemetation operator, set reverse flag and remove
** the (C) operator using blanks
*/

	reverse = false;
	if ( cPos = StrIndex("(C)", fSpec) ) {  /* Test for rev-comp */
	  for ( k=0; k<3; k++ )
	   *(cPos+k) = ' ';
	  reverse = true;
	}
/*
** Handle the special case of ((C)) by setting the range to the entire
** sequence. While we're here also handle the case ().
*/

	if ( StrIsBlank(StrCollapse(fSpec)) ) {
	  if ( reverse ) strcpy(fSpec, "*-1");
	    else         strcpy(fSpec, "1-*");
	}

/* 
** Count the number of tokens i.e. literals, points and ranges in the
** frag spec by counting commas. Bump up the token count by one to take
** into account that the commas are seperators.
*/

	for( numTokens=0, pFSpec=fSpec; *pFSpec; pFSpec++)
	  if ( *pFSpec == ',' ) numTokens++;
	numTokens++;

/* 
** Process all token, first to determine how large to make the
** final fragment will be.  This may seem redundant with the code
** which copies the characters but it is nesscary in order to get
** the size of each range and literal.
*/

	strcpy(fSave, fSpec);

	workSize = 0;
	tokens = numTokens;
	for (pFSpec = fSpec;  tokens; tokens--) {
	  if ( cPos = strchr(pFSpec, ',') ) *cPos = '\0';
	  if ( *pFSpec == '\"' || *pFSpec == '\'' ) { /* Literal */
	    for (k=1; *(pFSpec+k+1) ; k++)
	      workSize++;
	    pFSpec = cPos+1;
	    continue;
	  }

	  if ( pPos = strchr(pFSpec, '-') ) {           /* Range */
            strcpy(stopTok,pPos+1);
	    *pPos = '\0';
	    strcpy(startTok,pFSpec);
	  } else {                                      /* Point */
	    strcpy(startTok,pFSpec);
	    strcpy(stopTok,pFSpec);
	  }

/* 
** Check for the use of the "*" character to mean end of strand
*/

	    start = ( strcmp(startTok, "*") == 0 ) ?
	             seq->length : LIMIT(1,atol(startTok),seq->length);

	    stop = ( strcmp(stopTok, "*") == 0 ) ?
	             seq->length : LIMIT(1,atol(stopTok),seq->length);

	    if ( reverse ) {
	      if ( start < stop ) {
	        workSize += start;
	        start = seq->length;
	      }
	      workSize += (start-stop)+1;
	    } else {
	      if ( stop < start ) {
	        workSize += (seq->length-start)+1;
	        start = 1;
	      }
              workSize += (stop-start)+1;
	    }
	    pFSpec = cPos+1;

	} /* end of while loop for processing tokens*/

/*
** Get the amount of memory which you will need based.
*/

	workLength = workSize;
	workSize += 512;

	if ( (workStrand = CALLOC(workSize,char)) == NULL) {
	  PostError(2,"Fragment - Memory allocation failure");
	  return(false);
	}
	seqPos = workStrand;

/* 
** Reprocess the tokens and move the characters into WorkStrand
*/

	tokens = numTokens;
	for (pFSpec = fSave;  tokens; tokens--) {
	  if ( cPos = strchr(pFSpec, ',') ) *cPos = '\0';
	  if ( *pFSpec == '\"' || *pFSpec == '\'' ) { /* Literal */
	    for (k=1; *(pFSpec+k+1) ; k++)
	      *seqPos++ = *(pFSpec+k);
	    pFSpec = cPos+1;
	    continue;
	  }

	  if ( pPos = strchr(pFSpec, '-') ) {           /* Range */
            strcpy(stopTok,pPos+1);
	    *pPos = '\0';
	    strcpy(startTok,pFSpec);
	  } else {                                      /* Point */
	    strcpy(startTok,pFSpec);
	    strcpy(stopTok,pFSpec);
	  }

/*
** Check for the use of the "*" character to mean end of strand 
*/

	  start = ( strcmp(startTok, "*") == 0 ) ?
	           seq->length : LIMIT(1, atol(startTok), seq->length);

	  stop = ( strcmp(stopTok, "*") == 0 ) ?
	           seq->length : LIMIT(1, atol(stopTok), seq->length);

/* 
** Copy sequence characters from Strand to WorkStrand starting at "Start" and
** going to "Stop".  First check the reverse-complement flag to see how we
** have defined "Start" and "Stop" and then in each case check to see if we've
** gone around the end of a circular genome.  i.e. 200-100
*/

	  if ( reverse ) {
	    if ( start < stop ) {
	      for(k=start; k>=1; k--)
	        *seqPos = ( seq->type >= DNA ) ?
	                    CompBase(seq->strand[k]) : seq->strand[k];
	      start = seq->length;
	    }
	    for (k=start; k>=stop; k--)
	      *seqPos++ = ( seq->type >= DNA ) ?
	                    CompBase(seq->strand[k]) : seq->strand[k];
	  } else {
	    if ( stop < start ) {
	      for(k=start; k<=seq->length; k++)
	        *seqPos++ = seq->strand[k];
	      start = 1;
	    }
	    for (k=start; k<=stop; k++)
	      *seqPos++ = seq->strand[k];
	  }
	  pFSpec = cPos+1;
	} /* end of while */

	*seqPos = '\0';
	seq->length = workLength;
	seq->size = workSize;
	dispose(seq->mem);
	seq->mem = workStrand;
	seq->strand = seq->mem-1;
	return(true);

} /* End of Fragment */

/**  ReadTxt  ***************************************************************
**
** Read the Text from the current position in the file, we got to
** the correct line in the file via FindSeqSpec.
**
** William Gilbert, July, 1988
**
*****************************************************************************/

static Boolean ReadTxt(SeqSpec *spec, SeqEntry *seq)
{
char oneLine[512];
char errMsg[80];
char *temp;
int tempSize;
int n;
int backRec;
FILE *file;


/*
** Read lines from sequence file, testing for then end of the comments or
** beginning of the sequence or beginning of another entry.
*/

	if ( spec->isUser )
	  file = userFile;
	else {
	  file = refFile;  /* and set read Pointer */
	  fgets(oneLine, 512, file);  /* Skip codeword line in .REF file */
	  fgets(oneLine, 512, file);  /* Skip title line in .REF file    */
	}

	tempSize = 512;

	if ( seq->text != NULL ) dispose(seq->text);      /* Anti-bugging */

	if ( (temp = CALLOC(tempSize,char)) == NULL ) {
      	  PostError(2,"ReadTxt - Failed to allocate memory."); 
	  return(false);
	}
	seq->text = temp;
	seq->tSize = tempSize;

	n = 0;
	strcpy(seq->text,"");
	backRec = ftell(file);

GetLine:
	while ( fgets(oneLine, 512, file) != NULL ) {
	  n += strlen(oneLine);
	  if ( n > seq->tSize ) {
	    tempSize = seq->tSize + 512;
	    if ( (temp = REALLOC(seq->text,tempSize,char)) == NULL ) {
	      PostError(2,"ReadTxt - Failed to allocate memory."); 
	      return(false);
	    }
	    seq->text = temp;
	    seq->tSize= tempSize;
	  }

	  switch (spec->format) {
	    case PIR:
	      if (oneLine[0] == '>') goto Done;
	      strcat(seq->text,oneLine);
	      break;
	    case IG:
	      if (oneLine[0] != ';') goto Done;
	      strcat(seq->text,oneLine);
	      break;
	    case IBI:
	      if (strncmp("ORIGIN",oneLine,6) == 0) {
	        strcat(seq->text,oneLine);
	        goto Done;
	      }
	      strcat(seq->text,oneLine);
	      break;
	    case GCG:
	      if ( StrIndex("..",oneLine) ) return(true);
	      strcat(seq->text,oneLine);
	      break;
	    case RAW:
	    case STRIDER:
	    case STADEN:
 	      goto Done;
	  }
	  backRec = ftell(file);
	}  

/*
**  If we read to here then we've hit the bottom of the file.  Sometimes this
**  is OK, usually it mean there was something wrong with the sequence entry.
*/

	switch (spec->format) {
	  case PIR:
	    return(true);
	  case GCG:
	    sprintf(errMsg,"Missing \"..\" between text and sequence in file \"%s\".", spec->file);
	    PostError(1,errMsg);
	    return(false);
	  case IBI:
	    sprintf(errMsg, "Missing keyword \"ORIGIN\" in file \"%s\".", spec->file);
	    PostError(1,errMsg);
	    return(false);
	  case RAW:
	    return(true);
	  default:
	    sprintf(errMsg, "End of File while reading Text: \"%s\"", spec->file);
	    PostError(1,errMsg);
	    return(false);
	}

/*
**  Fix any read pointers here before going back.
*/

Done:
	fseek(file, backRec, SEEK_SET);

	return (true);

} /* End of ReadTxt */

