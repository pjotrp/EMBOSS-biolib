/***  seqentry.c    *********************************************************
**
**
**  William A. Gilbert, Whithead Institute
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "seqentry.h"
#include "seqspec.h"
#include "ckittypes.h"
#include "macros.h"

/*
** External routines used in this file
*/

extern char *StrToUpper(char *), *StrCollapse(char *);          /* Strings.c */
extern void DeleteSeqSpec(SeqSpec *spec);

/*
** Functions declarations and prototypes for this file
*/

SeqEntry *NewSeqEntry(void);
    void DeleteSeqEntry(SeqEntry *);
    char *MakeSeqDesc(SeqEntry *Seq);
     int EncodeType(char *String);
    char *DecodeType(int Type);

/*******************************  Methods  *********************************/
/***  NewSeqEntry  **********************************************************
**
**  Instance SeqEntry
**
**  William Gilbert, Whitehead Institute
**
****************************************************************************/

SeqEntry *NewSeqEntry(void)

{
SeqEntry *temp;	

	temp = new(SeqEntry);
	temp->spec = NULL;
	temp->name = NULL;
	temp->title = NULL;
	temp->desc = NULL;
	temp->type = UNDEF;
	temp->circular = 0;
	temp->checkSum = 0;
	temp->length = 0;
	temp->mem = NULL;
	temp->size = 0;
	temp->strand = NULL;
	temp->text = NULL;
	temp->tSize = 0;
	
	return(temp);
}
/***  DeleteSeqEntry  ******************************************************
**
**  Delete an instance of SeqEntry
**
**  William Gilbert, Whitehead Institute
**
****************************************************************************/

void DeleteSeqEntry(SeqEntry *seq)

{
	if ( seq == NULL ) return;

	DeleteSeqSpec(seq->spec);

	dispose(seq->name);
	dispose(seq->title);
	dispose(seq->desc);

	dispose(seq->mem);
	dispose(seq->text);

	dispose(seq);
}

/***  MakeSeqDesc  **********************************************************
**
**	Create a Sequence Description from sequence Type, and Strand
**
**  William Gilbert, Whitaker College, MIT
**  November 1986
****************************************************************************/

char *MakeSeqDesc(SeqEntry *seq)

{
/*--- Assemble Sequence Description line

[Circular ][DNA|RNA|Protein|Undefined][Fragment: fragspec]  Length:  Check:
*/
char temp[256];

	temp[0] = '\0';
	if ( seq->circular ) strcat(temp, "Circular ");

	switch ( seq->type ) {

	  case  PROTEIN: strcat(temp, "Protein"); break;
	  case FRAGMENT: strcat(temp, "Protein Fragment"); break;
	  case      DNA: strcat(temp, "DNA"); break;
	  case      RNA: strcat(temp, "RNA"); break;
	  case     RRNA: strcat(temp, "rRNA"); break;
	  case     TRNA: strcat(temp, "tRNA"); break;
	  case     URNA: strcat(temp, "uRNA"); break;
	  case     MRNA: strcat(temp, "mRNA"); break;
	        default: strcat(temp,"Undefined");
	}

	if( seq->spec->frag ) {
	  strcat(temp, "  Fragment: ");
	  strncat(temp, &seq->spec->frag[1], strlen(seq->spec->frag)-2);
	}

	sprintf(&temp[strlen(temp)],"  Length: %ld  Check: %d",
	        seq->length, seq->checkSum);

	seq->desc = CALLOC(strlen(temp)+1,char);
	strcpy(seq->desc,temp);

	return(seq->desc);

}  /* End of MakeSeqDesc */

/**  EncodeType ************************************************************
**
**  This function converts a string containing the Sequence Type
**  into an interger for the sequence Type.
**
******************************************************************************/

int EncodeType(char *string)

{
	StrToUpper(StrCollapse(string));

	if (      strcmp(string,"PROTEIN")==0 ) return(PROTEIN);
	else if ( strcmp(string,"DNA")==0 )     return(DNA);
	else if ( strcmp(string,"RNA")==0 )     return(RNA);
	else if ( strcmp(string,"RRNA")==0 )    return(RRNA);
	else if ( strcmp(string,"TRNA")==0 )    return(TRNA);
	else if ( strcmp(string,"URNA")==0 )    return(URNA);
	else if ( strcmp(string,"MRNA")==0 )    return(MRNA);
	else                                    return(UNDEF);

} /* End of EncodeType */

/***  DecodeType  *************************************************************
**
**  This function decode an integer for the sequence type and return an
**  address of a string corresponding to that code.
**
**  Usage includes:   strcpy(string,DecodeType(type))
**                    printf("%s",DecodeType(type))
**
******************************************************************************/

char *DecodeType(int type)

{
	switch ( type ) {
	  case PROTEIN: return((char *)"Protein");
	  case     DNA: return((char *)"DNA");
	  case     RNA: return((char *)"RNA");
	  case    RRNA: return((char *)"rRNA");
	  case    TRNA: return((char *)"tRNA");
	  case    URNA: return((char *)"uRNA");
	  case    MRNA: return((char *)"mRNA");
	       default: return((char *)"Undefined");
	}

} /* End of DecodeType */
