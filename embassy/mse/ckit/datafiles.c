/***  DataFiles.c  ***********************************************************
**
**  Used in locating and reading auxiliary data files.
**
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "boolean.h"
#include "gcgdefines.h"
#include <ctype.h>
#include <math.h>

/*
** External function declarations
*/

extern Boolean StrIsBlank(char *);                          /* Strings.c */
extern char *GetInput(char *), *StrUnComment(char *);      /* Strings.c */

/*
** Function declarations and prototypes
*/

FILE *FindDataFile(char *fileName);
FILE *FindPIRFile(char *fileName);
FILE *FindGCGFile(char *fileName, int *where);
void ShowGCGEnzymes(char *enzFName,Boolean doSchiz,int *where);

/***  FindDataFile  ***********************************************************
**
**  Look for a datafile to be used by the calling program.  First check where
**  "FName" tells you to, using the current directory as a default.  Then use
**  the user's root directory, then look in a logical called "GrpData" for
**  Group-wide data files. Finally, check the system-wide data files in
**  "CKITData".
**
**  This routine returns a null file pointer if the file was not found,
**  otherwise a file pointer is returned.
**
**  William A. Gilbert, Whitehead Institute
**
******************************************************************************/

FILE *FindDataFile(char *fName)

{
char fileName[256];
char trialName[256];
FILE *file;


/* 
** Try to find the data file by first looking where "FName" points, defaulting
** to the local directory.
*/

	if ( file = fopen(fName, "r") ) return(file);

/*
** Strip off the pathname and file name
*/

	strcpy(fileName, fName);
	DePath(fileName);

/*
** Next look in the users root directory "SYS$LOGIN", similar to "MyData" 
** in UWGCG programs
*/

	strcpy(trialName, "SYS$LOGIN:");
	strcat(trialName, fileName);
	if ( file = fopen(trialName, "r") ) return(file);

/*
** Next look in the logical "GrpData", or GroupWide data
*/

	strcpy(trialName, "GrpData:");
	strcat(trialName, fileName);
	if ( file = fopen(trialName, "r") ) return(file);

/*
** Finally, look in "CKitData", system data files, similar to "GenRunData"
**  in UWGCG files.
*/

	strcpy(trialName, "CKitData:");
	strcat(trialName, fileName);
	if ( file = fopen(trialName, "r") ) return(file);

	return(NULL);

} /* End of GetDataFile */

/***  FindPIRFile  ***********************************************************
**
**  Look for a datafile to be used by the calling program.  First check where
**  "FName" tells you to, using the current directory as a default.  Then use
**  the system-wide data files in "PIRSYSTEM".
**
**  This routine returns a null file pointer if the file was not found,
**  otherwise a file pointer is returned.
**
**  William A. Gilbert, Whitehead Institute
**
******************************************************************************/

FILE *FindPIRFile(char *fName)
{
char fileName[256];
char trialName[256];
FILE *file;


/* 
** Try to find the data file by first looking where "FName" points, defaulting
** to the local directory.
*/

	if ( file = fopen(fName, "r") ) return(file);

/*
** Strip off the pathname and file name
*/

	strcpy(fileName, fName);
	DePath(fileName);

/*
** Finally, look in "PIRSYSTEM", system data files, similar to "GenRunData"
**  in the GCG package.
*/

	strcpy(trialName, "PIRSYSTEM:");
	strcat(trialName, fileName);
	if ( file = fopen(trialName, "r") ) return(file);

	return(NULL);

} /* End of GetPIRFile */

/***  FindGCGFile  ***********************************************************
**
**  Look for a datafile to be used by the calling program.  First check where
**  "FName" tells you to, using the current directory as a default.  Then use
**  the logical called "MyData".  Finally, check the system-wide data files in
**  "GenRunData".
**
**  William A. Gilbert, Whithead Institute
**
******************************************************************************/


FILE *FindGCGFile(char *fName, int *where)

{
char fileName[256];
char trialName[256];
FILE *file;


/* 
** Try to find the data file by first looking where "fName" points, defaulting
** to the local directory.
*/

	*where = LOCAL;
	if ( file = fopen(fName, "r") ) return(file);

/*
** Strip off the pathname and file name
*/

	strcpy(fileName, fName);
	DePath(fileName);

/*
** Next look in the logical "MyData".
*/

	*where = MYDATA;
	strcpy(trialName, "MyData:");
	strcat(trialName, fileName);
	if ( file = fopen(trialName, "r") ) return(file);

/*
** Finally, look in "GenRunData".
*/

	*where = GENRUNDATA;
	strcpy(trialName, "GenRunData:");
	strcat(trialName, fileName);
	if ( file = fopen(trialName, "r") ) return(file);

	return(NULL);

} /* End of FindGCGFile */

/***  ShowGCGEnzymes  *******************************************************
**
**
**
****************************************************************************/

void ShowGCGEnzymes(char *enzFName,Boolean doSchiz,int *where)

{
#define PERLINE 8
#define PERSCREEN 21

char line[256];
int i,j;

char *cPos;
FILE *enzFile;

                 
	if ( (enzFile = FindGCGFile(enzFName,where)) == NULL ) {
	  printf("\n\n Enzyme file: %s does not exist",enzFName);
	  return;
	}

	while ( fgets(line,511,enzFile) != NULL)
	  if ( StrIndex("..",line) ) break;

	i = 0;
	j = 0;
	printf("\n\n");
	while ( fgets(line,511,enzFile) != NULL ) {
	  StrUnComment(line);
	  if ( cPos = strchr(line,' ') ) *cPos = '\0';
	  if ( cPos = strchr(line,';') ) 
	    if ( doSchiz )
	      strcpy(line,cPos+1);
	    else
	      continue;
	  if ( StrIsBlank(line) ) continue;
	  printf("%-10s",line);
	  i++;
	  if ( i >= PERLINE ) {
	    printf("\n");
	    i=0;
	    j++;
	  }
	  if ( j >= PERSCREEN ) {
	    printf("\nPress the <RETURN> key to continue ...");
	    GetInput(line);
	    printf("\n\n");
	    j=0;
	  }
	}

	fclose(enzFile);


#undef PERLINE
#undef PERSCREEN

} /* End of Show GCGEnzymes */

#include "ckittypes.h"

static int errors = 0;
static ProcPtr procNotify=NULL;
static ProcPtr procError=NULL;
static ProcPtr procFatal=NULL;

void Notify(ProcPtr pNotify, ProcPtr pError, ProcPtr pFatal);
 int ClearErrors(void);
 int Errors(void);
void PostError(Choice severity, char *errMsg);

void Notify(ProcPtr pNotify, ProcPtr pError, ProcPtr pFatal)
{
	procNotify = pNotify;
	procError = pError;
	procFatal = pFatal;
}

int ClearErrors() 
{
int temp;
	temp = errors;
	errors = 0;

	return(temp);
}

int Errors() 
{
	return(errors);
}

void PostError(Choice severity, char *errMsg)
{
	errors++;

	switch ( severity ) {

	  default:
	  case 1:
	    if ( procNotify)
	      (*procNotify)(errMsg);
	    break;

	  case 2:
	    if ( procError)
	      (*procError)(errMsg);
	    break;

	  case 3:
	    if ( procFatal)
	      (*procFatal)(errMsg);
	    break;

	}

}

