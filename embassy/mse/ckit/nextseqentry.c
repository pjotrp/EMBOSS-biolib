/***  NextSeqEntry.c ********************************************************
**
**
**
**    William A. Gilbert, Whitehead Institute
**
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ckittypes.h"
#include "seqspec.h"
#include "seqentry.h"
#include "macros.h"

/*
** External variables used to manage the databases
*/

extern FILE *namesFile;                                   /* ReadEntry.c */
extern FILE *userFile;                                    /* ReadEntry.c */
extern int codeLength;                                    /* ReadEntry.c */
extern long hashOffset;                                   /* ReadEntry.c */
extern char headerLine[];                                 /* ReadEntry.c */
extern int dbIndex;                                       /* ReadEntry.c */

/*
** Globals for this file
*/

Boolean lookUpError;
CharPtrFunc NextCode = NULL;

/*
** External Functions used in this file
*/

extern char     *StrToUpper(char *);                       /* Strings.c      */
extern char     *StrIndex(char *,char *);                  /* Strings.c      */
extern Boolean   StrIsBlank(char *);                       /* Strings.c      */
extern Boolean   StrWildMatch(char *, char *);             /* Strings.c      */
extern SeqEntry *ReadEntry(SeqSpec *);                     /* ReadEntry.c    */
extern SeqSpec  *NewSeqSpec(void);                         /* SeqSpec.c      */
extern void      MakeSeqSpec(SeqSpec *,char *);            /* SeqSpec.c      */
extern Boolean   NextVMSFile(char *, char *);              /* VMS.c          */
extern void      PostError(int, char *);                   /* Error.c        */
extern char *NextGCGCode(void);
extern char *NextPIRCode(void);

/*
** Delcarations and function prototypes for this file.  This file
** has only one public routine, "NextSeqEntry".  All other routines are
** known only to this file.
*/

SeqEntry *NextSeqEntry(SeqSpec *);
/* private: */
static Boolean  NextIndSpec(SeqSpec *, SeqSpec *);
static Boolean  NextDBSpec(SeqSpec *, SeqSpec *);
static Boolean  NextUserSpec(SeqSpec *, SeqSpec *);
static Boolean  NextUserCode(char *,char *, int, Boolean, SeqSpec *);

/***  NextSeqEntry  **********************************************************
**
**  1) Allocate memory for the next Sequence Spec. structure. For a wildcard
**     spec many may potentially be allocated as in a multiple sequence
**     alignment or editor.
**
**  2) Look at the Wildcard Spec. spec and determine if it is:
**      a) an indirect file of sequence specs.
**      b) a user file
**      c) a database entry
**
**  3) If an error occured then report it quit now.
**
**  4) Invoke ReadEntry to retrieve the entry.
**
***************************************************************************/

SeqEntry *NextSeqEntry(SeqSpec *wildName)
{
SeqSpec *thisSpec;

	thisSpec = NewSeqSpec();

	if ( wildName->file[0] == '@' ) {
	  if ( NextIndSpec(wildName, thisSpec) == false ) return(NULL);
	} else {
	  if ( wildName->isUser ) {
	    if ( NextUserSpec(wildName, thisSpec) == false ) return(NULL);
	  } else {
	    if ( NextDBSpec(wildName, thisSpec) == false ) return(NULL);
	  }
	}

	return( ReadEntry(thisSpec) );
}
/* End of NextSeqEntry */

/***  NextIndSpec  *********************************************************
**
**  Finds the next sequence specification in a File of SeqSpecs.
**  Indirect files can be nested 5 deep.
**
**    William A. Gilbert, Whitehead Institute
**
***************************************************************************/

static Boolean NextIndSpec(SeqSpec *indSpec, SeqSpec *thisSpec)

{
	char *cPos, line[256];
	int i;

	static SeqSpec tempSpec = {NULL,NULL,NULL,NULL,false,false,false,UNDEF};

	static char options[5][80], currIndFName[256];
	static int depth;
	static FILE *file[5];

/*
**  Is this the same indirect command file or a new one?  If so,
**  copy the filename to list at the zero level. Open the file and
**  see if it exists. Scan it for ".." to see there are any header comments
**  if ".." cannot be found start back at the top.  Read the first line
**  of the file by jumping to the "NextLine" label.  This seem a little
**  convoluted but remeber that this peice of code get executed once, from
**  then on it's a nice little dance between the "NextLine" and "NextSeqSpec"
**  labels. Read the comments, it does make sence.
*/

	if ( strcmp(currIndFName, indSpec->file) != 0 ) {
	  depth = 0;
	  if ( indSpec->file) strcpy(currIndFName, indSpec->file);
	  if ( indSpec->options) strcpy(options[depth],indSpec->options);
	  if ( (file[depth] = fopen(&indSpec->file[1], "r")) == NULL )
	    return(false);
	  while ( fgets(line, 255, file[depth]) )
	    if ( StrIndex("..", line) ) break;
	  if ( feof(file[depth]) ) rewind(file[depth]);
	  goto NextLine;
	}

/*
** At this point a Spec is either from a User file or a database.
*/

NextSeqSpec:

	if ( tempSpec.isUser ) {
	  if ( NextUserSpec(&tempSpec, thisSpec) )
	    return(true);
	} else {
	  if ( NextDBSpec(&tempSpec, thisSpec) )
	    return(true);
	}
/*
** Get a line from the indirect file(s), goto NextSeqSpec until it
** the current spec exhausts, i.e. returns false.  At which point you will
** fall through to here in order to read the next line. 
*/

NextLine:
	while ( depth >= 0 ) {
	  while ( fgets(line, 255, file[depth]) ) {
	    if ( cPos = strchr(line, ' ') ) *cPos = '\0';  /* First token */
	    if ( cPos = strchr(line, '!') ) *cPos = '\0';  /* Uncomment   */
	    if ( cPos = strchr(line, '\n') )*cPos = '\0';  /* String-ize  */
	    if ( line[0] == '\0' ) continue;               /* Blank line  */
	    if ( line[0] == '@' ) {                        /* Another FOSS*/
	      if ( depth < 5 ) {
	        depth++;
	        if ( cPos = strchr(line,'/') ) {
	          strcpy(options[depth],cPos);
	          *cPos = '\0';
	        }
	        if ( file[depth] = fopen(&line[1], "r") ) {
	          while ( fgets(line, 255, file[depth]) )
	            if ( StrIndex("..", line) ) break;
	          if ( feof(file[depth]) ) rewind(file[depth]);
	        } else {
	          depth--;
	        }
	      } else {
	        PostError(2,"SeqSpec lists are too deeply nested!!");
	      }
	    } else {                         /* A Sequence Spec              */
	      for ( i=depth; i>=0; i-- )     /* Append the options from every*/
	        strcat(line, options[i]);    /*   FOSS file which came before*/
	      MakeSeqSpec(&tempSpec, line);  /* Assign the SeqSpec structure */
	      goto NextSeqSpec;              /* Dispatch and run down.        */
	    }
	  }
	  fclose(file[depth]);
	  depth--;
	}
/*
**  Fall through to here when all of the Sequence Specification from all
**  of the FOSS files are finished.
*/
	currIndFName[0] = '\0';
	return(false);
}
/* End of NextIndSpec */

/***  NextDBSpec  *********************************************************
**
**  Returns individual database entries from a possibly ambiguous code 
**  specification.  Returns "false" when exhausted.
**
**  William Gilbert, Whitehead Institute
**
*************************************************************************/

static Boolean NextDBSpec(SeqSpec *wildName, SeqSpec *thisSpec)

{
char *cPos;
char errMsg[256];
static char currCode[256], currDB[256];

extern Boolean SetDataBase(char *, char *);              /* ReadEntry.c */

/*-----------------------------------------------------------------*/

	if ( strcmp(wildName->code, currCode) ||
	     strcmp(wildName->file, currDB) )  {    /* New WildName */
	  if ( SetDataBase(wildName->file, errMsg) == false ) return(false);
	  strcpy(currCode,wildName->code);
	  strcpy(currDB,wildName->file);
	} else {
	  if ( wildName->isWildCode == false ) {
	    currCode[0] = '\0';
	    currDB[0] = '\0';
	    return(false);
	  }
	}

/*
** Return the next codeword in the database. Use "StrWildMatch" to see
** if it is a member of the ambiguous set.  If it's not ambiguous use 
** it as is.
*/
	if ( wildName->isWildCode ) {
	  while ( cPos = (char*)(*NextCode)() ) {
	    if ( StrWildMatch(wildName->code, cPos) ) goto Match;
	  }
	  currCode[0] = '\0';
	  currDB[0] = '\0';
	  return(false);
	} else 
	  cPos = wildName->code;

/*
**  Allocate storage for the codeword, "code", the database name, "file",
**  fragment spec, "frag", options qualifiers "options".
**  We know this SeqSpec is not ambiguous and that all databases are in
**  PIR format, so set these attributes accordingly.
*/


Match:

	if ( cPos ) {
	  thisSpec->code = CALLOC(strlen(cPos)+1,char);
	  strcpy(thisSpec->code, cPos);
	  if ( cPos = strchr(thisSpec->code, ' ') ) *cPos = EOS;
	}
	if ( wildName->file ) {
	  thisSpec->file = CALLOC(strlen(wildName->file)+1,char);
	  strcpy(thisSpec->file, wildName->file);
	}
	if ( wildName->frag ) {
	  thisSpec->frag = CALLOC(strlen(wildName->frag)+1,char);
	  strcpy(thisSpec->frag, wildName->frag);
	}
	if ( wildName->options ) {
	  thisSpec->options = CALLOC(strlen(wildName->options)+1,char);
	  strcpy(thisSpec->options, wildName->options);
	}

	thisSpec->isUser = false;
	thisSpec->isWildCode = false;
	thisSpec->isWildFile = false;
	thisSpec->format = PIR;
	return(true);
}
/* End of NextDBSpec */


/***  NextUserSpec  ********************************************************
**
**  Generates individual user-entry file entries from an ambiguous code
**  specification.
**
**  Explanation:
**  -----------
**  If we have a new Wild SeqSpec, then save it's name, extract out
**  the code and filename, do a dummy call to "NextVMSFile" in order
**  initialize it if the last call was a valid file with an invalid
**  code.
**
**  Now, get the next valid VMS file and open it. Set "badFileName" to
**  to false.  If a bad filename is given to "NextVMSFile" it will return
**  false so you have to test "badFileName" to see if this was really
**  the cause of the return status as opposed to an exhausted wildcard
**  stream.  Note that if a bad filename returns true this is because a false
**  does not mean an error but an exhausted wildcard stream.  The error
**  flags are set in the External and the error will be processed by
**  "ReadEntry". You now fall through to "NextUserCode".
**
**  On subsequent calls to this routine you execute "NextUserCode" immediatly
**  as long as you're working on the same WildName.  If and when "NextUserCode"
**  run out of valid codes it will return false and then you branch back to
**  "NextVMSFile" for the next file.
**
**    William A. Gilbert, Whitehead Institute
**
***************************************************************************/

static Boolean NextUserSpec(SeqSpec *wildName, SeqSpec *thisSpec)

{
	return false;    
#if COMMENTED_OUT
static char code[256];
static char file[256];
static Boolean isWildCode;
static int format;

static char currCode[256], currFile[256], currFName[256];
static char wildCode[256], wildFName[256];
static Boolean badFileName;

char fName[256], line[256];
char temp[256];
int seqFormat;

/*
** New possibly ambiguous user spec.  Save it in currCode and currFile.
** Flush the "NextVMSFile" routine by passing it a blank file name.
** Get the real next file using "NextVMSFile" and test to see if the file
** exists and can be open by this users.
*/


	if ( strcmp(wildName->code,currCode) ||
	     strcmp(wildName->file,currFile)  )  {
	  strcpy(currCode,wildName->code);
	  strcpy(currFile,wildName->file);
	  strcpy(code, wildName->code);
	  isWildCode = wildName->isWildCode;
	  format = wildName->format;
	  badFileName = true;
	  NextVMSFile("", file);  /* Flush NextVMSFile */

        NextFile:
	  if ( NextVMSFile(wildName->file, file) ) {
	    badFileName = false;
	    if ( strcmp(file, currFName) != 0 ) {
	      strcpy(currFName, file);	  
	      if ( userFile ) fclose(userFile);
	      if ( (userFile = fopen(file, "r")) == 0 )
	        badFileName = true;  /* Cannot open */
	    }
	  } else {
	    if ( badFileName ) {
	        sprintf(temp, "File \"%s\" not found.", wildName->file);
	        PostError(1,temp);
	        return(false);
	    }
	    currCode[0]='\0';
	    currFile[0]='\0';
	    return(false);
	  }
	}
	if ( badFileName ) return(false);

/*
** Using the current file name obtained in the section above.  Run down
** all of the codes which match the wild code spec.
** Append the fragment and options specs to each SeqSpec derived from
** the wild spec.
**
** As in the routine "NextDBCode" we know that this is not an ambiguous
** spec and set it as such.  We can get the format format from the 
** input specification.
**
** When the code exhausts branch to the "NextFile" section and process any
** more files which may be in the ambigous filename set.
*/

	if ( NextUserCode(code, file, format, isWildCode, thisSpec) ) {
	  if ( wildName->frag ) {
	    thisSpec->frag = CALLOC(strlen(wildName->frag)+1,char);
	    strcpy(thisSpec->frag, wildName->frag);
	  }
	  if ( wildName->options ) {
	    thisSpec->options = CALLOC(strlen(wildName->options)+1,char);
	    strcpy(thisSpec->options, wildName->options);
	  }
	  thisSpec->format = wildName->format;
	  thisSpec->isUser = true;
	  thisSpec->isWildCode = false;
	  thisSpec->isWildFile = false;
	  return(true);
	}
	goto NextFile;
#endif
}
/* End of NextUserSpec */

/***  NextUserCode   *******************************************************
**
**  Returns the sequence entry from a user entry file. For multiple entry
**  formats like PIR and IG we have to search through the file to find
**  codeword that match the wildcode.  For single entry formats like GCG,
**  RAW, etc. we return the sequence entry, there is no ambiguity below
**  the filename level.
**
**  NB: Multiple entries for IG format files has not been implemented.
**
**    William A. Gilbert, Whitehead Institute
**
***************************************************************************/

static Boolean NextUserCode(char *code, char *file, int format,
                            Boolean isWildCode, SeqSpec *thisSpec)
{
char kode[256], line[256];
char temp[80];
char *cPos;
static char currCode[512], currFile[512];
static Boolean codeIsBlank;

/*
**   If we have a New WildName...
**    (1) Set the read pointer to the beginning of the file
**    (2) Get the Codeword portion of the wildname and see if it too is wild.
**
**   If it's the old wildname then we are still returning codes from the current
**    file which statisfy the code in WildName. If, however, the code is not
**    wild then we are done.
*/

	if ( strcmp(code, currCode) ||
	     strcmp(file, currFile) )  {    /* New Wild name */
	  rewind(userFile);                 /* maybe same file, new codeword */
	  strcpy(currCode,code);
	  strcpy(currFile,file);
	} else {
	  if ( isWildCode == false ) {
	    currCode[0] = '\0';
	    currFile[0] = '\0';
	    return(false);
	  }
	}

/*
**  Read lines from the user file until you hit bottom. If you happen to
**  find a header line, upcase the codeword and test against the possibly
**  ambiguous code from WildName.
**
**  For multiple entry formats like PIR, we will do the user a favor and
**  return the first entry in the file is the codeword is blank.
*/

	switch (format) {
	  case PIR:
	    codeIsBlank = StrIsBlank(code);
	    while ( fgets(line, 512, userFile) != NULL ) {
	      if ( line[0] != '>' ) continue;                /* Not a header */
	      if ( cPos = strchr(line, '\n') ) *cPos = '\0'; /* Stringize */
	      if ( cPos = strchr(line, ' ') ) *cPos = '\0';  /* first token */
	      StrToUpper(strcpy(kode, &line[4]));            /* Upcase for match*/
	      if ( isWildCode ) {
	        if ( StrWildMatch(code, kode) ) goto CodeMatch;
	      } else {
	        if ( codeIsBlank || strcmp(code,kode) == 0 ) /* user 1st codeword */
	          goto CodeMatch;
	      }
            }

/*
** End of file falls through to here, Reset the current code/file flags 
** because we are done.
** Returning false means we've exhausted the list and are done.
** On the other hand, if we were looking for a specific code, i.e. isWildCode
** was not wild, then we get to here we have failed to find it and must
** report this as an error.
*/

	    currCode[0] = '\0';
	    currFile[0] = '\0';
	    if ( isWildCode ) return(false);

	    sprintf(temp, "Code \"%s\" not found in file \"%s\"", 
	      code, file);
	    PostError(1,temp);
	    return(false);

/*
**  All other formats consist of single entries, so return true.
*/
	  default:   
	    line[0] = '\0';
	    kode[0] = '\0';
	    goto CodeMatch;
	}

/*
** If we get to here we have either, found the correct codword or found a
** member of the ambiguous set of codewords for PIR format.  For all other 
** formats we were simple able to open the file.
**
** Allocate storage for the the code and filename and put them into the
** current SeqSpec which we are preparing.
*/

CodeMatch:

	strcpy(headerLine, line);
	StrToUpper(headerLine);

	thisSpec->code = CALLOC(strlen(kode)+1,char);
	strcpy(thisSpec->code, kode);

	thisSpec->file = CALLOC(strlen(file)+1,char);
	strcpy(thisSpec->file, file);

	return(true);
}

/* End of NextUserCode */

