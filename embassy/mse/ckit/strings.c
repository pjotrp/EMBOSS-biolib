/***  strings.c  *********************************************************
** 
**  This file contains several string manipulation routines.
**
*************************************************************************/

#include <ctype.h>
#include "ckittypes.h"
#include <string.h>

/*
** Function declaration and prototypes for this file
*/

   char  *BooleanToStr(Boolean Value, int Style);
   char  *NextToken(char *String, char *cPos, char *Token, char *Sep);
   char  *StrCollapse(char *String);
   char  *StrCompress(char *String);
   char  *StrIndex(char *Pattern, char *String);
Boolean  StrIsBlank(char *String);
   char  *StrToLower(char *String );
   char  *StrToUpper(char *String );
   char  *StrChange(char *String, char Before, char After);
   char  *StrTrim(char *String);
   char  *StrTruncate(char *String);
   char  *StrUnComment(char *String);
Boolean StrWildMatch(char *Pattern, char *String);


/***  BooleanToStr  ******************************************************
**
**  Converts a Boolean variable to a string using one of three systems.
**
**     "YN"  - "Yes", "No" system
**     "TF"  - "True", "False" system
**     "OZ"  - "One", "Zero" system
**
**  This function returns a pointer to the appropriate string. Which has
**  been previously assigned to a static variable.
**
**  William A. Gilbert, Whitehead Institute
**
**************************************************************************/

char *BooleanToStr(Boolean value, int style)

{

#define YN 0
#define OZ 1
#define TF 2

/* Switch and return pointer to the correct string */

	switch ( style ) {
	  case YN:
	    return(value ? (char *)"Yes" : (char *)"No");
	  case OZ:
	    return(value ? (char *)"1" : (char *)"0");
	  case TF:
	    return(value ? (char *)"True" : (char *)"False");
	  default:
	    return(value ? (char *)"Da" : (char *)"Nyet");    /* For Glasnost */
	}

} /*  End of BooleanToStr */

/***  StrCollapse  *******************************************************
**
**  Remove all of the white space, if any, out of "String". Return a pointer
**  to "String".
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/

char *StrCollapse(char *string)

{
char *out, *ptr=string;
	
	out = string;
	while ( *string ) {
	  if ( !isspace((int)*string) ) *out++ = *string;
	  string++;
	}
	*out = '\0';

	return(ptr);

}  /* End of StrCollapse */

/***  StrCompress  ********************************************************
**
**  Replace all multiple spaces and tabs with a single space.  Tabs are
**  converted to spaces.  This function returns a pointer to "String".
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/

char *StrCompress(char *String)

{
#define SPACE '\040'

char *Out, *ptr=String;
	
	Out = String;
	while ( (*Out = *String++) ) {
	  if ( isspace((int)*Out) ) {
	   *Out = SPACE;
	   while ( isspace((int)*String) )
	     String++;
	  }
	  Out++;
	}

	return(ptr);

}  /* End of StrCompress */

/***  StrIndex  *************************************************************
**
**  This function returns the address of "Pattern" within "String".  It is
**  case sensitive so incoming strings should be made the same case by the
**  calling program if case does not matter.
**  
**  A NULL pointer is returned if no match is foundor if Pattern was empty.
**
**  William A. Gilbert, Whitehead Institute
**
*****************************************************************************/

char *StrIndex(char *Pattern, char *String )
{

char *pPos, *sPos;

	if ( String == NULL ) return(NULL);
	if ( Pattern == NULL ) return(NULL);
	if ( *Pattern == 0 ) return(NULL);

	for ( ; *String; String++ )
	  for ( pPos=Pattern, sPos=String; *pPos == *sPos; pPos++,sPos++ )
	    if ( *(pPos+1) == 0 ) return(String);

	return ((char *)NULL);

} /* End of StrIndex */

/***  StrIsBlank  ***********************************************************
**
**  Returns true if every charater in string is non-printing.
**
**  William A. Gilbert, Whitehead Institute
**
*****************************************************************************/

Boolean StrIsBlank(char *String )
{
	while ( *String )
	  if ( isspace((int)*String++) == 0 ) return(0);

	return(1);

} /* End of StrIsBlank */

/***  StrToLower  **********************************************************
**
**  This converts an entire string of characters to lowercase.
**  Non-alphabetics are not changed (of course).  StrToLower returns
**  a pointer to the new string.
**
**  William A. Gilbert, Whitehead Institute
**
**************************************************************************/

char *StrToLower(char *String )
{
char *ptr = String;

	while ( (*String = (char) tolower((int)*String)) ) 
	  String++;

	return(ptr);

} /*  End of StrToLower */

/***  StrToUpper  ********************************************************
**
**  This converts an entire string of characters to uppercase.
**  Non-alphabetics are not changed (of course).  StrToUpper returns
**  a pointer to "String".
**
**  William A. Gilbert, Whitehead Institute
**
**************************************************************************/

char *StrToUpper(char *string )
{
char *ptr=string;

	while ( (*string = (char)toupper((int)*string)) ) 
	  string++;

	return(ptr);
} /*  End of StrToUpper */

/***  StrChange *******************************************************
**
**  This function returns at pointer to "String" after replacing all
**  occurences of the character "before" with the character "after".
**
**  Restiction: Only single character replacements are possible.
**
**  William A. Gilbert, Whitehead Institute
**
***********************************************************************/

char *StrChange(char *string, char before, char after )
{

	while( *string ) {
	  if ( *string == before ) *string = after;
	  *string++;
	}

	return(string);

} /* End of StrChange */

/***  StrTrim  ************************************************************
**
**  Remove leading and trailing space characters. space is defined by <ctype.h>
**  Returns a pointer to "String".
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/


char *StrTrim(char *String)
{

char *Out, *ptr=String;
	
	while ( isspace((int)*String) )          /* Find first non-space character */
	  String++;

	if ( *String == '\0' ) {            /* End reached with all "space"*/
	  *ptr = '\0';
	  return(ptr);
	}

	Out = ptr;
	while ( (*Out = *String++) )        /* Copy what's left */
	  Out++;

	while ( isspace((int)*--Out) )         /* Back up over blanks  */
	  *Out = '\0';

	return(ptr);

}  /* End of StrTrim */

/***  StrTrunate  *********************************************************
**
**  Remove trailing space. Returns a pointer to "String".
**
**  William A. Gilbert, Whitehead Institute
**
**************************************************************************/


char *StrTruncate(char *String)

{
char *Out, *ptr=String;
	
	for ( ; *String; String++ )
	  ;

	Out = String;
	while ( isspace((int)*--Out) )         /* Back up over space chars */
	  *Out = '\0';

	return(ptr);

}  /* End of StrTruncate */

/***  StrUnComment  ********************************************************
**
**  Remove the comments from a string at the "!" character. Return a pointer
**  to "String".
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/

char *StrUnComment(char *String)

{

#define COMMENT '!'

char *ptr=String;
	
	while ( *String )
	  if ( *String == COMMENT ) 
	    *String = '\0';
	  else
	    String++;

	return(ptr);

}  /* End of StrUnComment */

/***  StrWildMatch  *******************************************************
**
**  Determines if "String" is in the set a member of the ambiguous string
**  "Pattern" using the VMS ambiguous file characters "*" and "%".
**
**  Bug in non-VMS version:  Does not properly translate a wildcard pattern
**                           contining "*%"
**
**  William A. Gilbert, Whitehead Institute
**
**************************************************************************/

Boolean StrWildMatch(char *Pattern, char *String)

{

#ifdef VMS

#include <ssdef.h>
#include <descrip.h>
#define STR$_MATCH 2393113

#endif

#ifdef VMS

unsigned int Status;
short int Length;
struct dsc$descriptor_s PatternDesc;
struct dsc$descriptor_s StringDesc;
extern unsigned int STR$MATCH_WILD();  

#else

  char SubStr[256];

#endif

char *cPos;

/*
** Strip off trailing spaces
*/

	if ( (cPos = strchr(Pattern, ' ')) ) *cPos = '\0';
	if ( (cPos = strchr(String, ' ')) ) *cPos = '\0';

/*
** Deal with special case of "*" where everything matches
** by returning true.
*/

	if ( strcmp(Pattern, "*") == 0 ) return(1);

#ifdef VMS

/* 
** Setup the structure and let VMS do the work.
*/

	PatternDesc.dsc$a_pointer = Pattern;
	PatternDesc.dsc$w_length = strlen(Pattern);
	PatternDesc.dsc$b_class = DSC$K_CLASS_S;
	PatternDesc.dsc$b_dtype = DSC$K_DTYPE_T;

	StringDesc.dsc$a_pointer = String;
	StringDesc.dsc$w_length = strlen(String);
	StringDesc.dsc$b_class = DSC$K_CLASS_S;
	StringDesc.dsc$b_dtype = DSC$K_DTYPE_T;

	Status = STR$MATCH_WILD(&StringDesc, &PatternDesc);

	if ( Status == STR$_MATCH ) return( 1 );
	return( 0 );

#else

/*
** Restiction: Non-VMS version has known deficiency, cannot handle a
** pattern containing "*%"
**
**  For each character in "Pattern" consider three cases:
**
**    (1) Next character is null or multiple wildcard match,"*".
**        (a) Condense multiple "*", test for end of "Pattern"
**        (b) Extract the nonambigous substring from "Pattern"
**        (c) If it's found in "String" adjust pointer to "String" and go on
**    (2) Next character is single wildcard match, "%".
**    (3) Next character is discreet and must match.
*/
	while ( *Pattern != '\0' || *String != '\0'  ) {

	  if ( *Pattern == '*' ) {
	    for(Pattern++; *Pattern == '*'; Pattern++)
	      ;
	    if ( *Pattern == '\0' ) return(1);

	    strcpy(SubStr, Pattern);
	    if ( (cPos = strchr(SubStr, '%')) ) *cPos = '\0';
	    if ( (cPos = strchr(SubStr, '*')) ) *cPos = '\0';

	    if ( (cPos = StrIndex(SubStr, String)) == 0 ) return(0);
	    String = cPos;

	  } else if ( *Pattern == '%' ) {
	    if ( *String == '\0') return(0);
	    Pattern++;
	    String++;

	  } else {
	    if ( *Pattern != *String ) return(0);
	    Pattern++;
	    String++;
	  }
	}

	return(1);
#endif

}
/* End of StrWildMatch */

/**  NextToken ****************************************************************
**
**  Given a string and a place to start.  This routine will find the next
**  occurence of the seperator character and return the token between the
**  two.
**
**  This routine can be used to parse command lines and input files.
**
******************************************************************************/

char *NextToken(char *String, char *cPos, char *Token, char *Sep)

{
char *pPos,*tPos;

	if ( (pPos = strpbrk(cPos,Sep)) == NULL )
        {
	  if ( *cPos == '\0' ) 
	    return(NULL);
	  else {
	    strcpy(Token, cPos);
	    return(strchr(cPos,'\0'));
	  }
        }

	for ( tPos=Token; cPos < pPos; tPos++, cPos++)
	  *tPos = *cPos;
	
	*tPos = '\0';

	return(cPos+1);

}
