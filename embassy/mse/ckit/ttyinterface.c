/***  TTYInterface.c  *******************************************************
**
**  The routines in this file are used to implement a TTY or command driven
**  interface to the program.
**
**  All input flows through the "GetInput" routine, in this way bad input
**  into GetInteger and GetFloat do not cause the program to dump. GetInput
**  also allows a user to use a dash at the end of a line to indicate a
**  continuation of input on the next line.
**
***************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "seqspec.h"
#include "seqentry.h"
#include "ckittypes.h"
#include "macros.h"
#include <unistd.h>
#include <stdlib.h>

/*
**  Declaration and prototypes for functions within this file
*/

     char *GetInput(char *prompt, char *def);
  Boolean  GetBoolean(char *prompt,Boolean def);
     long  GetInteger(char *prompt, long def, long low, long high);
   double  GetFloat(char *prompt, char *format, double def, double low, double high);
  Boolean  GetRange(long *Begin, long *End, SeqEntry *);
  SeqSpec *GetSeqSpec(char *, char *,char *);
     void  TTYError(char *ErrMsg);

/*
** External Declarations
**/

extern    char *StrToUpper(char *);                        /* Strings.c */
extern    char *StrCollapse(char *);                       /* Strings.c */
extern Boolean  StrIsBlank(char *);                        /* Strings.c */
extern    char *BooleanToStr(Boolean value, Choice style); /* Strings.c */
extern Boolean GetOSSymbol(char* , char*);           /* <os>.c */
extern    char  *StrTrim(char *String);


/**  GetInput  ************************************************************
**
**  This function obtains a string from STDIO ( null terminated ).
**  If the string read has length greater than zero it copies 
**  the value to the string passed.  Otherwise it leaves it unchanged.
**
**  Returns a pointer to the input string.
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/

char *GetInput(char *prompt, char *string )

{
char newString[512];
int length;
Boolean start;

	start = 1;

/* 
** Read one line
*/
	if ( StrIsBlank(string) )
	  printf(" %s? ",prompt);
	else
	  printf(" %s [ %s ] ? ",prompt, string);

Get:	if ( gets(newString) == NULL ) { printf("\n"); exit(0);}


	length = strlen(newString);
	if ( length > 0 ) {
	  if ( start ) *string = '\0';            /* Write over existing */
	  start = 0;
	  if( newString[length-1] == '-' ) {      /* Continuation character */
	    newString[length-1] = '\0';           /* Process and get more   */
	    strcat(string, newString);
	    printf(" continue: ");
	    goto Get;
	  } else
	    strcat(string, newString);
	}

	return(string);
	
} /* End of GetInput */

/**  GetBoolean  ***********************************************************
**
**  Accept a Boolean response.  The response may be Y, Yes, N, No, T, True
**  F, False, 0 or 1.  Upper or lower case.  
**
**  William A. Gilbert, Whitehead Institute
**  
***************************************************************************/

Boolean GetBoolean(char *prompt, Boolean def)

{
char string[256];

	while(1) {
	  strcpy(string,BooleanToStr(def,0));
	  StrToUpper(StrCollapse(GetInput(prompt,string)));
	  if ( *string == 'Y' ) return(1);
	  if ( *string == 'N' ) return(0);
	  if ( *string == 'T' ) return(1);
	  if ( *string == 'F' ) return(0);
	  if ( *string == '1' ) return(1);
	  if ( *string == '0' ) return(0);
	  printf("\n You must answer YES or NO. Try again: ");

	}

}  /* End of GetBoolean */

/**  GetFloat  *********************************************************
**
**  This routine performs input of floating point (real) values.  The number
**  is truncated if it is read and must be in the range of Low to
**  High. The current value, "val" is passed so that the user may press a
**  return to accept it as the default.
**
**  William A. Gilbert, Whitehead Institute
**
*************************************************************************/

double GetFloat(char *prompt, char *format, double def, double low, double high )

{
char string[256];
char mask[256];
double value;
double power;
int i, sign;

/*----------------------------------------------------*/

	sprintf(mask,"%%%sf", format);
	sprintf(string, mask, def);

	StrTrim(string);
Try:	if ( StrIsBlank(StrCollapse(GetInput(prompt,string))) )
	  return(def);
	 
	sign = 1;
	if ( string[0] == '+' || string[0] == '-')
      	  sign = (string[0]=='+') ? 1: -1;

	value = 0;
	for ( i=0; string[i]; i++ ) {
	  if ( string[i] == '.' ) break;
	  if ( isdigit((int)string[i]) )
	    value = (value * 10.) + (string[i] -'0');
	}

	power = 1.0;
	for (; string[i]; i++ )
	  if ( isdigit((int)string[i]) ) {
	    value = (value * 10.) + (string[i] -'0');
	    power *= 10.0;
	  }

	value *= sign/power;

	if ( (value < low ) || (value > high) ) {
	  sprintf(mask,"\n Input is out of the range %%%sf to %%%sf, try again: ", format, format);
	  printf(mask, low, high);
	  goto Try;
	}

	return(value);

}  /*  End of GetFloat */

/**  GetInteger  *********************************************************
**
**  This subroutine performs input of integer values.  The integer
**  is truncated if it is read and must be in the range of Low to
**  High.
**
**  William A. Gilbert, Whitehead Institute
**
*************************************************************************/


long GetInteger(char *prompt, long def, long low, long high )
{
char string[256], *cPos;
long value;
int i, sign;

/*----------------------------------------------------*/

	sprintf(string,"%ld", def);
	
Try:	if ( StrIsBlank(StrCollapse(GetInput(prompt,string))) )
	  return(def);
	 
	sign = 1;
	if ( string[0] == '+' || string[0] == '-')
	  sign = (string[0]=='+') ? 1: -1;

	if ( (cPos = strchr(string,'.')) ) *cPos = '\0';

	value = 0;
	for ( i=0; string[i]; i++ )
	  if ( isdigit((int)string[i]) )
	    value = (value * 10) + (string[i] -'0');

	value *= sign;

	if ( (value < low ) || (value > high) ) {
	  printf("\n Input is out of the range %ld to %ld, try again: ",
	   low, high);
	  goto Try;
	}

	return(value);

}  /*  End of GetInteger */

/**  GetRange  *************************************************************
**
**  This routine prompts for begin and end positions in a sequence max 
**  in length.  if begin is greater than end, or either is outside the
**  range 1...Max, an error message is printed.
**
**  William A. Gilbert, Whitehead Institute
**
***************************************************************************/

Boolean GetRange(long *begin, long *end, SeqEntry *seq)

{
char temp[256];
char format[256];
char *cPos;
Boolean reverse;
/*------------------------------------------------------*/

	*begin = ( seq->length ) ? 1 : 0;
	*end = seq->length;
	reverse = 0;

	*begin = GetInteger("\n                  Begin", *begin, *begin, seq->length);

	sprintf(temp,"End [%ld] ?  ", *end);
	sprintf(format,"%30s",temp);
	if ( (cPos = strrchr(format,'[')) ) *(cPos-1) = EOS;

	*end = GetInteger(format, *end, *begin, seq->length);

	if ((*begin != 1) || *end != seq->length) seq->circular = 0;

	if ( seq->type >= DNA ) {
	  if (GetBoolean("  From reverse strand",0)) {
	    reverse = 1;
	  }
	}

	return(reverse);
}
/* End of GetRange */

/*** GetSeqSpec  **************************************************************
**
**  Gets a Sequence Specification from STDIN.  Ask for the spec using prompt.
**  Look up the current value of DCLSymbol and use it as the defualt. Set this
**  symbol to the user's response.  "DefaultDB" is the database to use if the
**  the user enters a codeword without specifing a database.
**
**  Returns a pointer to the stucture "SeqSpec"
**
**  William A. Gilbert, Whitehead Institute
**
*******************************************************************************/

SeqSpec *GetSeqSpec(char *prompt, char *dclSymbol, char *defaultDB)
{
char tempSpec[128], line[128];
SeqSpec *spec;

extern Boolean GetOSSymbol(), SetOSSymbol();            /* VMS.c     */
extern SeqSpec *NewSeqSpec(void);                         /* SeqSpec.c */
extern void  MakeSeqSpec();                               /* SeqSpec.c */

/*
** First check to see if we have not exceeded our allowed tries.
** Then determine if we have a prompt and if there is a global
** symbol to be used as a default input.
*/

	if ( GetOSSymbol(dclSymbol, line) ) 
	  strcpy(tempSpec,line);
	else
	  strcpy(tempSpec, "");

	strcpy(line,"\n ");
	strcat(line, prompt);
	StrCollapse(GetInput(line,tempSpec));

	if ( *dclSymbol )
	  SetOSSymbol(dclSymbol, tempSpec);

	spec = NewSeqSpec();
	MakeSeqSpec(spec, tempSpec,defaultDB);

	return(spec);

} /* End of GetSeqSpec */

/***  TTYError  **************************************************************
**
**  User printf to display an error.
**
******************************************************************************/

void TTYError(char *errMsg)
{
	printf("\n\n Error: %s\n", errMsg);
}
