/***  GCG.c  *************************************************************
**
**  This file contains a suite of related routines which find and retrieve
**  sequences from GCG formated databases.
**
**  Database files:
**     GCG names and offset files (.NAMES & .OFFSET).
**
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "ckittypes.h"
#include "seqspec.h"
#include "seqentry.h"
#include "macros.h"

/*
** Global variables used by this file and externally.
*/


FILE *offsetFile;           /* GCG sorted offset file    */
FILE *namesFile;            /* GCG sorted name files     */
int gcgOffset;
int gcgNames;

/*
** External Global variables
*/

extern CharPtrFunc NextCode;                  /* NextSeqEntry.c */
extern ProcPtr SetDBPointers;             /* ReadEntry.c    */

extern int codeLength;
extern int dbINXFile;

extern FILE *seqFile;           /* Core database files, Sequence and Text */
extern FILE *refFile;           /* Core database files, Sequence and Text */
extern int dbSEQFile;          /* files desc numbers of above file pointers */
extern int dbREFFile;          /* files desc numbers of above file pointers */


/*
** Useful functions used by this file.
*/

extern     void  PostError(int, char *);                   /* Error.c    */

/*
**  Function declarations and prototypes for this file.
*/

Boolean OpenGCGDatabase(char *dbName);

/* private: */
static Boolean LookupGCG(SeqSpec *Spec);
static char *NextGCGCode(void);


/***  OpenDBFiles  *********************************************************
**
**      Function OpenDBFiles opens the appropriate PIR formatted
**	database files.  This function may tend to be considered
**	VMS specific due the name of the file name syntax and the
**	binary repesentation of integers as 4 bytes.
**
**	Arguments:
**	----------
**	vmsDBName - The VMS name of the database fileswith out the file
**	            extension i.e. For the GenBank database this 
**	            would be SEQ_DATABASE:GENBANK
**
**      OpenDBFiles returns TRUE for opening completion.
**
**  Will Gilbert, Whitehead Instite
**  October 1985
**
**************************************************************************/
Boolean OpenGCGDatabase(char *dbPathName)

{
#define ERROR -1
#define MODE 0

char tempLine[80];
char errMsg[80];

/*
** Open Database Index (.Offset), Sequence (.SEQ) and Text (.REF) files.
** If any errors occur then return OpenGCGDatabase as false.
*/

	if ( dbSEQFile ) close(dbSEQFile);
	if ( dbREFFile ) close(dbREFFile);
	if ( dbINXFile ) close(dbINXFile);
	if ( gcgOffset ) close(gcgOffset);
	if ( gcgNames  ) close(gcgNames);

/*
** Open core database sequence and text files
*/
	strcpy(tempLine,dbPathName);
	strcat(tempLine,".seq");
	if ( (seqFile = fopen(tempLine, "r")) == NULL) {
	  sprintf(errMsg,"Failed to open database file: %s",tempLine);
	  PostError(1,errMsg);
	  return(0);
	}

	strcpy(tempLine,dbPathName);
	strcat(tempLine,".ref");
	if ( (refFile = fopen(tempLine, "r")) == NULL) {
	  sprintf(errMsg,"Failed to open database file: %s",tempLine);
	  PostError(1,errMsg);
	  return(0);
	}

	dbSEQFile = fileno(seqFile);
	dbREFFile = fileno(refFile);

/*
** Open GCG indices, .NAMES and .OFFSET files
*/

	strcpy(tempLine,dbPathName);
	strcat(tempLine,".offset"); 

	if ( (offsetFile = fopen(tempLine, "r")) == NULL) {
	  sprintf(errMsg,"Failed to open GCG database file: %s",tempLine);
	  PostError(1,errMsg);
	  return(0);
	}

	strcpy(tempLine,dbPathName);
	strcat(tempLine,".names");
	if ( (namesFile = fopen(tempLine, "r")) == NULL) {
	  sprintf(errMsg,"Failed to open GCG database file: %s",tempLine);
	  PostError(1,errMsg);
	  return(0);
	}
	codeLength = 12;
	gcgOffset = fileno(offsetFile);
	gcgNames = fileno(namesFile);

	NextCode = NextGCGCode;
	SetDBPointers = LookupGCG;

	return(1);

} /* End of OpenDBFiles */


/****  NextGCGCode  ***********************************************************
**
**  This is a sequential read through the GCG .NAMES files.  Codewords will
**  be returned in alphabetical order.
**
****************************************************************************/

static char *NextGCGCode(void)
{
static Boolean initialized = 0;
static char code[20];

	if (!initialized) {
	  initialized = 1;
	  rewind(namesFile);
	}

	if ( fgets(code, 39, namesFile) ) {
	  code[codeLength+1] = EOS;
	  return(code);
	}

	initialized = 0;
	return(NULL);

}  /* End of NextGCGCode */



/*** LookupGCG  ***********************************************************
**
**  Function LookupGCG looks for a valid code in the current database
**  followed by an access to the .NAMES file.
**
**  The functions does a linear search through the .NAMES file it is not
**  very efficient, it could use a binary search of the alphabetized
**  names.  This function will be written with GCG release 7 which will
**  have redesigned .NAMES and .OFFSET files.
**
**  LookupGCG returns true for successful Code find and read pointer set.
**
**   William A. Gilbert
**
***********************************************************************/

static Boolean LookupGCG(SeqSpec *spec)

{
  char code[40];
  char line[40];
  char errMsg[80];
  int isn;
  
  long seqOffset, refOffset;
  rewind(offsetFile);
  
  while ( fgets(line, 39, offsetFile) ) {
    sscanf(line,"%s %d %ld %ld", code, &isn, &seqOffset, &refOffset);
    if ( strcmp(spec->code,code) ) continue;
    
    fseek(seqFile, seqOffset-512, SEEK_SET);
    fseek(refFile, refOffset-512, SEEK_SET);
    
    return(1);
  }
  
  sprintf(errMsg, "Code \"%s\" not found in %s index.",
	  spec->code,spec->file);
  PostError(1,errMsg);
  return(0);
  
} /* End of LookupGCG */

