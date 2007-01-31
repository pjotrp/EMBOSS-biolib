/***  PIR.c  *************************************************************
**
**  This file contains a suite of related routines which find and retrieve
**  sequences from PIR formated databases.
**
**  Database files:
**     PIR index    (.INX) file.
**     PIR sequence (.SEQ) file.
**     PIR text     (.REF) file.
**
**  William Gilbert, copyright 1990
**  
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "seqspec.h"
#include "seqentry.h"
#include "ckittypes.h"
#include "macros.h"

/*
** Global variables, declared by ReadEntry.c
** PIR globals are declared in this file.
*/

extern CharPtrFunc NextCode;              /* NextSeqEntry.c */
extern ProcPtrSeq SetDBPointers;             /* ReadEntry.c    */

extern int dbIndex;                       /* VDB.c       */
extern int dbType, dbFormat, entries;     /* VDB.c       */
extern int codeLength;                    /* VDB.c       */



/*
** PIR Globals
*/

FILE *indexFile;                   /* PIR index file                       */
int dbINXFile;

extern FILE *seqFile;           /* Core database file, Sequence */
extern FILE *refFile;           /* Core database file, Reference Text */
extern int dbSEQFile;          /* files desc numbers of above file pointers */
extern int dbREFFile;          /* files desc numbers of above file pointers */

static int prime;
static int seqOffset;
static int hashOffset;

/*
** External functions used by this file.
*/

extern void  PostError(int, char *);                   /* Error.c    */

/*
**  Function declarations and prototypes for this file.
*/

Boolean OpenPIRDatabase(char *dbPathName);

/* private: */
static Boolean  SeekISN(int isn);
static     int  HashEntry(char *code, int *prime);
static    char *NextPIRCode(void);
static Boolean LookupPIR(SeqSpec *spec);

/***  OpenPIRDatabase  *********************************************************
**
**      Function OpenPIRDatabase opens the appropriate PIR formatted
**	database files.  This function may tend to be considered
**	VMS specific due the name of the file name syntax and the
**	binary repesentation of integers as 4 bytes.
**
**	Arguments:
**	----------
**	dbPathName - The VMS name of the database files with out the file
**	             extension i.e. For the GenBank database this 
**	             would be SEQ_DATABASE:GENBANK
**
**      OpenPIRDatabase returns TRUE for opening completion.
**
**  Will Gilbert, Whitehead Instite
**  October 1985
**
**************************************************************************/
Boolean OpenPIRDatabase(char *dbPathName)

{
#define ERROR -1
#define MODE 0

char tempLine[80];
char errMsg[80];
long line[8];


/*  ajDebug(" OpenPIRDatabase entered\n"); */
/*
** Open Database Index (.inx), Sequence (.seq) and Text (.ref) files.
** If any errors occur then return DBOpenFiles as false.
*/

	if ( dbSEQFile ) close(dbSEQFile);
	if ( dbREFFile ) close(dbREFFile);
	if ( dbINXFile ) close(dbINXFile);

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
	if ( (refFile = fopen(tempLine, "r")) == NULL)
	  return(0);

	dbSEQFile = fileno(seqFile);
	dbREFFile = fileno(refFile);

	NextCode = NextPIRCode;
	SetDBPointers = LookupPIR;

/*
**  Open and read PIR index file (.INX) and read header line variables
**
** Read the Index File header block. If an error occurs return false.
** The header block is record number 1 of the .INX file; the layout is:
**
**  Item  Description
**  ----  -----------
**    1   Database type:   1=Protein Sequence, 2=Nucleic Acid Sequence
**    2   Database format: 0=Unknown, 1=PIR, 2=GenBank, 3=EMBL
**    3   Number of entries in database
**    4   Length of codewords: 6 to 10
**    5   Prime number for code-to-ISN hash function
**    6   Record offset to first TTL index record (Obsolete, not used by CKit)
**    7   Record offset to first SEQ/REF index record
**    8   Record offset to first hash table record
**
** Each item is a 4 byte Integer read high byte to low.  Watch this on
**  non-VMS implemetations.
*/

        strcpy(tempLine,dbPathName);
        strcat(tempLine,".inx");
        if ( (indexFile = fopen(tempLine, "r")) == NULL)
          return(0);


        dbINXFile = fileno(indexFile);

	if ( fread(line, sizeof(long), 8,indexFile) == 0) return(0);

	dbType     = line[0];
	dbFormat   = line[1];
	entries    = line[2];
	codeLength = line[3];
	prime      = line[4];
	seqOffset  = line[6];
	hashOffset = line[7];

/*	ajDebug("OpenPIRDatabase %d %d %d %d %d %d %d\n",dbType,dbFormat,
	entries,codeLength,prime,seqOffset,hashOffset);
*/
	return(1);

} /* End of OpenPIRDatabase */

/****  NextPIRCode  ***********************************************************
**
**  Return the codewod in the hash list. Not sorted by ISN!!
**
**  Read through the Hash table, reading records as required.
**  This is poorly coded but get the job done for now.
**
****************************************************************************/

static char *NextPIRCode(void)
{
#define ERROR -1
static Boolean initialized=0;
static unsigned char buffer[512];
static char *cPos;
static char *recordEnd;
static int record=0;
	
	if (!initialized) {
	  record = 0;
	  initialized = 1;
	  cPos = recordEnd = (char*)buffer;
	}

loop:	cPos += codeLength+2;

	if ( cPos < recordEnd ) {
	  *(cPos + codeLength) = EOS;
	  return(cPos);
	}

	if ( record == prime) goto end;

	fseek(indexFile, (hashOffset+record)*512, SEEK_SET);
	if ( fread(buffer,sizeof(char), 512, indexFile) == 0 ) goto end;
	record++;
	recordEnd = (char*)buffer + buffer[1]*256 + buffer[0];
	cPos = (char*)buffer - codeLength;
	goto loop;

end:	initialized = 0;
	return(NULL);

}  /* End of NextPIRCode */


/*** LookupPIR  ***********************************************************
**
**   Search the Index (INX) file for the CODE:
**    (1) Hash the codeword in order determine the bucket number.
**    (2) Locate codeword in the bucket.
**    (3) Get the internal sequence number (ISN), next two bytes.
**    (4) Call SeekISN with the ISN to set the read pointers.
**
**  LookupPIR returns true for successful Code find and read pointer set.
**
**   William A. Gilbert, Whitehead Institute
**
***********************************************************************/

static Boolean LookupPIR(SeqSpec* spec)
{
	char code[256];
	int isn;
	int record, recordLength, i, j;
	unsigned char kode[512];

	strcpy(code,spec->code);
	strcat(code, "         ");
	record = hashOffset + HashEntry(code, &prime);

/*
**  Read the hash table bucket and search for the CODE
*/

	fseek(indexFile, (record-1)*512, SEEK_SET);
	fread (kode, sizeof(char), 512, indexFile);
	recordLength = kode[1]*256 + kode[0] - (codeLength+2);

	for (j=2; j<=recordLength; j += codeLength+2 ) {
	  for ( i=0; i<codeLength; i++)
	    if ( code[i] != kode[j+i] ) break;
	  if ( i == codeLength ) {
	    isn = kode[j+i+1]*256 + kode[j+i];
	    return(SeekISN(isn));
	  }
	}

/*
** If we get to here, the code was not found in the INX bucket.
*/

	return(0);

} /* End of LookupPIR */


/*** SeekISN **************************************************************
**
**	Given a sequence's Internal Sequence Number (ISN), SeekISN will
**	look up the byte offsets .REF file and .SEQ file and move the file
**	pointers to the start of desired entry.
**
**       Input parameter:
**         isn = Internal Sequence Number
**
**       SeekISN is true for successful pointer setting in both files.
**
**   William Gilbert, October 1985
**
***************************************************************************/

static Boolean SeekISN(int isn)

{
int locs[64][2];

	fseek(indexFile, (seqOffset+((isn-1)/64))*512, SEEK_SET);
	if ( fread(locs,sizeof(char), 512, indexFile) == 0 ) return(0);

	fseek(seqFile, (long)locs[(isn-1)%64][0]-512, SEEK_SET);
	fseek(refFile, (long)locs[(isn-1)%64][1]-512, SEEK_SET);

	return(1);

} /* End of SeekISN */

/****  HashEntry  *******************************************************
**
** Using a database code word. Determine which record (aka bucket) it is
** in the ISN section of the .INX file.  Look in that record to determine
** it's ISN.
**
**   William Gilbert, October 1985
**
*************************************************************************/
static int HashEntry(char *code, int *prime)
{
union {
   unsigned short int hash[5];
   char kode[10];
   } align ;

int i, hashEntry;

	for ( i=0; i<=9; i++)
	   align.kode[i]=code[i];

	hashEntry = ( ((int)align.hash[4] ^ align.hash[2]) << 6) +
	            ( ((int)align.hash[1] ^ align.hash[3]) << 3) +
	                    align.hash[0];

 return ( 1 + (hashEntry % *prime) );

} /* End of HashEntry */
