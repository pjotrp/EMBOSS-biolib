/***  VDB.c  *************************************************************
**
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "seqspec.h"
#include "ckittypes.h"
#include "macros.h"

/*
** Global variables, also used by NextSeqEntry.c
*/

int dbIndex;                       /* Database indices: 1=PIR, 2=GCG     */
int dbType, dbFormat, entries;
int codeLength;

FILE *seqFile;           /* Core database file, Sequence */
FILE *refFile;           /* Core database file, Reference Text */

int dbSEQFile;          /* files desc numbers of above file pointers */
int dbREFFile;          /* files desc numbers of above file pointers */


/*
** External functions used by this file.
*/

extern     void  PostError(int, char *);                   /* Error.c    */
extern  Boolean  LookupGCG(SeqSpec *spec);                 /* GCG.c      */
extern  Boolean  OpenGCGDatabase(char *dbPathName);        /* GCG.c      */
extern  Boolean  LookupPIR(SeqSpec *spec);                 /* PIR.c      */
extern  Boolean  OpenPIRDatabase(char *dbPathName);        /* PIR.c      */
extern  Boolean  TransName(char *, char *);                /* VMS.c      */


/*
**  Function declarations and prototypes for this file.
*/

Boolean SetDataBase(char *dbName);

/** SetDataBase ***********************************************************
**
**      Function SetDataBase makes the VMS Logical name for the database
**	files from the database name, DBName.  It then checks to see if
**	it has been defined as a valid system logical and if so then opens
**	the appropriate Database files.  If the files open successfully then
**      this database becomes the new default database hence user does not
**      have to type so much.
**
**	Assumption: Users will tend to work with entries from only one 
**	database at a time so make the last opened one the default.
**
**       Input parameter:
**         dbName - Database name from Sequence Specification
**	           i.e. GENBANK, EMBL, NUCLEIC, etc.
**
**       SetDataBase is True for successful completion.
**
**   Will Gilbert, Whitehead Institute
**     October, 1986 - Created
**     August, 1989  - Added GCG Name and Offset file support
**
****************************************************************************/

Boolean SetDataBase(char *dbName)
{

char translation[256], dbLogical[256];
char errMsg[80];
static char currentDB[256];
static Boolean dbIsOpen=0;

/*
** If this Database is the Current one and it is open, quit now, doing
** logical lookups and opening files is very time consuming.   
*/

	if ( (strcmp(dbName, currentDB)==0) && dbIsOpen )
          return(1);

/*
**  VMS Implementation
**  ------------------
**  Does this database have a logical pointer? If so then open the Database
**  files and set "DefaultDB" to this Database name.
**
** Database names map to Database logical variables in the following way.
**  Note: The database name does not have to be the same as the datase
**        file names. However it usually is. (Required syntax is up-case)
**
**    $ DEFINE NA$databasename mydisk:[myroot.subdir]databasefiles
**
** If DBLogical is valid then open a new database and make this database
**  the new default. Otherwise, leave everything alone.
**
** Non-VMS implementations don't check the NA$ and PR$ stuff.
**  
*/

/*
** Look for ATLAS databases from the PIR first.
*/

	strcpy(dbLogical, "DB$");
	strcat(dbLogical, dbName);

/*	ajDebug("dblogical = %s\n",dbLogical); */
	if( TransName(translation, dbLogical) ) {
	  dbIndex = PIR;
	  if( OpenPIRDatabase(translation) ) {
	    strcpy(currentDB, dbName);
	    dbIsOpen = 1;
	    return(1);
	  } else return(0);
	}

/*
** Look for nucleic acid databases from the PIR next.
*/

	strcpy(dbLogical, "NA$");
	strcat(dbLogical, dbName);

	if( TransName(translation, dbLogical) ) {
	  dbIndex = PIR;
	  if( OpenPIRDatabase(translation) ) {
	    strcpy(currentDB, dbName);
	    dbIsOpen = 1;
	    return(1);
	  } else return(0);
	}

/*
** Then look for protein databases from the PIR
*/

	strcpy(dbLogical, "PR$");
	strcat(dbLogical, dbName);

	if( TransName(translation, dbLogical) ) {
	  dbIndex = PIR;
	  if( OpenPIRDatabase(translation) ) {
	    strcpy(currentDB, dbName);
	    dbIsOpen = 1;
	    return(1);
	  } else return(0);
	}
/*
** Finally, look for GCG Names & Offset files.
*/
	strcpy(dbLogical, dbName);

	if( TransName(translation, dbLogical) ) {
	  dbIndex = GCG;
	  if( OpenGCGDatabase(translation) ) {
	    strcpy(currentDB, dbName);
	    dbIsOpen = 1;
	    return(1);
	  } else return(0);
	}

	sprintf(errMsg, "Logical pointer for database \"%s\" is not defined",dbName);
	PostError(1,errMsg);
	return(0);

} /* End of SetDatabase */

