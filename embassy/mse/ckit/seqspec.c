/***  seqspec.c  ************************************************************
 **
 **
 **  William A. Gilbert, Whitehead Institute
 **
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "seqentry.h"
#include "ckittypes.h"
#include "macros.h"

/*
** External functions used
*/

extern Boolean *StrIsBlank(char *);                          /* Strings.c */
extern    char *StrToUpper(char *);                          /* Strings.c */
extern    char *StrCollapse(char *);                         /* Strings.c */
extern    char *DePath(char *);                              /* VMS.c     */
extern Boolean  GetOSSymbol(char *, char *);                /* VMS.c     */
extern char *StrIndex(char *Pattern, char *String );

/*
**  Function declarations and function prototypes for this file
*/

SeqSpec* NewSeqSpec(void);                           
   void  DeleteSeqSpec(SeqSpec*);
SeqSpec* CopySeqSpec(SeqSpec* to, SeqSpec* from);
   void  MakeSeqSpec(SeqSpec* spec, char* line, char* defaultDB);
 Choice  EncodeFormat(char*);
   char* DecodeFormat(int);

static Boolean IsUser(char*);
static char *CodeSpec(char*);
static char *FileSpec(char*, char*);
static char *FragSpec(char*);

/******************************  Methods  *********************************/
/***  NewSeqSpec  **********************************************************
**
**  Instance a SeqSpec.
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/

SeqSpec *NewSeqSpec(void)

{
SeqSpec *temp;	

 temp = new(SeqSpec);
	temp->code = NULL;
	temp->file = NULL;
	temp->frag = NULL;
	temp->options = NULL;
	temp->isUser = 0;
	temp->isWildCode = 0;
	temp->isWildFile = 0;
	temp->format = UNDEF;
	
	return(temp);
}

/***  DeleteSeqSpec  **********************************************************
**
**  Deallocate an instance of a SeqSpec.
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/

void DeleteSeqSpec(SeqSpec *spec)
{
	if ( spec == NULL ) return;

	if ( spec->code ) dispose(spec->code);
	if ( spec->file ) dispose(spec->file);
	if ( spec->frag ) dispose(spec->frag);
	if ( spec->options ) dispose(spec->options);
}

SeqSpec *CopySeqSpec(SeqSpec *to, SeqSpec *from)
{
	if ( to == NULL )
	  to = new(SeqSpec);

	if ( from->code ) {
	  to->code = CALLOC(strlen(from->code)+1,char);
	  strcpy(to->code,from->code);
	}

	if ( from->file ) {
	  to->file = CALLOC(strlen(from->file)+1,char);
	  strcpy(to->file,from->file);
	}

	if ( from->frag ) {
	  to->frag = CALLOC(strlen(from->frag)+1,char);
	  strcpy(to->frag,from->frag);
	}

	if ( from->options ) {
	  to->options = CALLOC(strlen(from->options)+1,char);
	  strcpy(to->options,from->options);
	}

	to->isUser = from->isUser;
	to->isWildCode = from->isWildCode;
	to->isWildFile = from->isWildFile;
	to->format = from->format;
	
	return(to);
}


/***  MakeSeqSpec  *************************************************************
**
**
**
**  William A. Gilbert, Whitehead Institute
**
*******************************************************************************/

void MakeSeqSpec(SeqSpec *spec, char *line, char * defaultDB)
{
char *cPos;
char tempStr[256];
int i;
static int defSeqFormat = UNDEF;



/* ajDebug("MakeSeqSpec line= %s, defaultDB = %s\n",line,defaultDB); */
/*
** Cleanup and leftover data in case this structure is being reused
*/
	if ( spec->code ) dispose(spec->code);
	if ( spec->file ) dispose(spec->file);
	if ( spec->frag ) dispose(spec->frag);
	if ( spec->options ) dispose(spec->options);
/*
** Find out what the User has define as his/her default format for
** his/her user-entry sequences.  If it's not defined, set it to
** GCG. RAWSEQ makes a more theoretical choice but GCG a more practical one.
*/

	if ( defSeqFormat == UNDEF ) {          
	  if ( GetOSSymbol("CKitSeqFormat",tempStr) == 0 )
	    strcpy(tempStr, "GCG");

	  defSeqFormat = EncodeFormat(StrToUpper(tempStr));
        }

/*
** First save any OptionSpec to the SeqSpec structure and then
** remove them as they get in the way
*/

	if ( (cPos = strchr(line, '!')) ) {
	  spec->options = CALLOC(strlen(cPos)+1,char);
	  strcpy(spec->options, cPos);
	  *cPos = '\0'; 
	  StrToUpper(spec->options);
	}

/*
** Did user type a just a filename? Then set it up to bring back the
** first code in that file by putting an equals sign at the beginning
*/

/* ajDebug("MakeSeqSpec line= %s, defaultDB = %s\n",line,defaultDB); */
	if( strchr(line, '.') && line[0] != '@' )
	  if( strchr(line, '=') == NULL ) {
	    for( i = strlen(line); i>=0; i--)
	      line[i+1] = line[i];
	    line[0] = '=';
	  }

/* ajDebug("MakeSeqSpec line= %s, defaultDB = %s\n",line,defaultDB); */
/*
** If this is an indirect file spec, save the filename with the "@"
** character.  Set both wild flags to true, as we really don't know
** Options have already been saved.
**
** If not a FOSS, break out all of the parts of this SeqSpec for future use
** go ahead and invoke individual functions as this is not a rate
** determining function.  If the Filename is blank for a database
** spec then set it to the default as passed.
*/
	if ( line[0] == '@' ) {
	  spec->file = CALLOC(strlen(line)+1,char);
	  strcpy(spec->file, line);
	  spec->isWildCode = 1;
	  spec->isWildFile = 1;
	} else {
	  spec->code = CodeSpec(line);
	  spec->file =  FileSpec(line,defaultDB);
	  spec->frag =  FragSpec(line);
	  spec->isUser = IsUser(line);

	  spec->isWildCode = 0;
	  if ( strchr(spec->code, '*') ) spec->isWildCode = 1;
	  if ( strchr(spec->code, '%') ) spec->isWildCode = 1;

	  spec->isWildFile = 0;
	  if ( strchr(spec->file, '*') ) spec->isWildFile = 1;
	  if ( strchr(spec->file, '%') ) spec->isWildFile = 1;
	}
/* ajDebug("MakeSeqSpec line= %s, defaultDB = %s\n",line,defaultDB); */
/*
** If the Spec is a database entry don't waste time, assign format as 
** PIR, exit now.
*/

	if ( spec->isUser == 0 ) {
	  spec->format = PIR;
	  StrToUpper(spec->code);
/*	  ajDebug("spec->format = PIR\n"); */
	  return;
	}
	
/*
** Check any Option Spec to see if this sequences format is different
** from the default setting
*/

	spec->format = defSeqFormat;
	if ( spec->options ) {
	  if ( StrIndex("/NBRF",spec->options) ) 
	    spec->format = NBRF;
	  else if ( StrIndex("/PIR",spec->options) )  
            spec->format = PIR;
	  else if ( StrIndex("/IG",spec->options) )
	    spec->format = IG;
	  else if ( StrIndex("/IBI",spec->options) )
	    spec->format = IBI;
	  else if ( StrIndex("/RAW",spec->options) )
	    spec->format = RAWSEQ;
	  else if ( StrIndex("/STA",spec->options) )
	    spec->format = STADEN;
	  else if ( StrIndex("/GCG",spec->options) )
	    spec->format = GCG;
	  else if ( StrIndex("/STR",spec->options) )
	    spec->format = STRIDER;
	}

/*
** Here we have failed to assign the user-entry file format so
** set it he/she must mean the default format
*/

	if ( spec->format == UNDEF ) spec->format = defSeqFormat;

}

/***  EncodeFormat  ***********************************************************
**
**  This function converts a string containing the Sequence format
**  into an integer for the sequence format
**
******************************************************************************/

Choice EncodeFormat(char *string)

{
	StrToUpper(StrCollapse(string));

	if (      strcmp(string,"PIR")==0 )      return(PIR);
	else if ( strcmp(string,"NBRF")==0 )     return(PIR);
	else if ( strcmp(string,"RAW")==0 )      return(RAWSEQ);
	else if ( strcmp(string,"STADEN")==0 )   return(STADEN);
	else if ( strcmp(string,"GCG")==0 )      return(GCG);
	else if ( strcmp(string,"IG")==0 )       return(IG);
	else if ( strcmp(string,"IBI")==0 )      return(IBI);
	else if ( strcmp(string,"STRIDER")==0 )  return(STRIDER);
	else                                     return(UNDEF);

} /* End of EncodeFormat */

/***  DecodeFormat  ***********************************************************
**
**  This function decode a integer for the sequence format and returns an
**  address of string corresponding to that code.
**
**  Usage includes:   strcpy(string,DecodeFormat(format))
**                    printf("%s",DecodeFormat(format))
**
******************************************************************************/

char *DecodeFormat(int format)

{
	switch ( format ) {
	  case     PIR: return((char *)"PIR");
	  case  RAWSEQ: return((char *)"Raw");
	  case  STADEN: return((char *)"Staden");
	  case     GCG: return((char *)"GCG");
	  case      IG: return((char *)"IG");
	  case     IBI: return((char *)"IBI");
	  case STRIDER: return((char *)"Strider");
	       default: return((char *)"Undefined");
	}

} /* End of DecodeFormat */

/**********************  Private routines  ******************************/
/***  IsUser  **********************************************************
**
**  Determines if a SeqSpec is a user-entry by looking for the
**  equals sign "=" in the sequence seqspec.
**
**  William A. Gilbert, Whitehead Institute
**
*************************************************************************/

static Boolean IsUser(char *spec)

{
	while ( *spec && *spec != '/' )
	  if ( *spec++ == '=' ) return(1);

	return(0);

}  /* End of IsUser */

/***  CodeSpec  **************************************************************
**
**  Extracts the code from a sequence specification by stripping off the 
**  options spec, filename and equals if a user entry, the database name
**  if a database code, and finally any fragment spec.
**
**  William A. Gilbert, Whitehead Institute
**
******************************************************************************/

static char *CodeSpec(char *spec)

{
char *cPos;
char temp[256];

	strcpy(temp,spec);
	if( (cPos = strchr(temp, '/')) ) *cPos = '\0';

	if( IsUser(temp) ) {
	  if( (cPos = strchr(temp, '=')) ) *cPos = '\0';
        } else {
	  if( (cPos = strchr(temp, ':')) ) strcpy(temp, cPos+1);
	}
/* 
**Remove FragSpec 
*/
	if( (cPos = strchr(temp, '(')) ) *cPos = '\0';

	cPos = CALLOC(strlen(temp)+1,char);
	strcpy(cPos,temp);

	return(cPos);

} /* End of CodeSpec */

/***  FileSpec  **************************************************************
**
**  Extracts the filename from a sequence specification by stripping off 
**  any options beyond a '/' character.
**  Depending on the type of SeqSpec, User or Database, the filename or
**  database name is copied to "File".
**
**  William A. Gilbert, Whitehead Institute
**
******************************************************************************/

static char *FileSpec(char *spec, char *defaultDB)

{
char *cPos;
char temp[256];

	strcpy(temp,spec);
	if( (cPos = strchr(temp,'/'))) *cPos = '\0';
	
	if ( IsUser(temp) ) {
	  if ( (cPos = strchr(temp,'=')) ) strcpy(temp, cPos+1);
	} else {
	  if ( (cPos = strchr(temp,':')) ) {
	    *cPos = '\0';
	  } else {
	    if ( StrIsBlank(defaultDB) )
	      strcpy(temp,"GENBANK");
	    else
	      strcpy(temp,defaultDB);
	  }
	}

	StrToUpper(temp);
	cPos = CALLOC(strlen(temp)+1,char);
	strcpy(cPos,temp);

	return(cPos);

}  /* End of FileSpec */

/***  FragSpec  ***********************************************************
**
**  Extracts the fragment specification from a sequence specification by 
**  first removing the OptionSpec, finding the first "(" and then finding
**  the last ")".  If a matching closing ")" cannot be found Frag is
**  returned empty.
**
**  William A. Gilbert, Whitehead Institute
**
***************************************************************************/

static char *FragSpec(char *spec)

{
char *cPos;
char temp[256];

	strcpy(temp, spec);
	if ( (cPos = strchr(temp, '/')) ) *cPos = '\0';

	if ( (cPos = strchr(temp, '(')) ) {
	  strcpy(temp, cPos);
	  if ( (cPos = strrchr(temp,')')) )
	    *(cPos+1) = '\0';
	  else
	   return(NULL);
	} else 
	   return(NULL);

	cPos = CALLOC(strlen(temp)+1,char);

	return(cPos);

} /* End of FragSpec */

