#include <stdio.h>
#include <string.h>
#include "seqentry.h"
#include "ckittypes.h"
#include "macros.h"

char *SeqToSpecName(SeqEntry *, int);
char *WriteSeq(char *, SeqEntry *, int);

extern void PostError(int severity, char *errMsg);             /* Error.c */


/***  SeqToSpecName  *********************************************************
**
**  Return a user-entry output spec, based on the current SeqSpec.  Can be
**  used with database entries to provide defaults.
**
**  William Gilbert, Whitehead Institute
**
**************************************************************************/

char *SeqToSpecName(SeqEntry *seq, int format)

{
static char outSpec[256];

	strcpy(outSpec, seq->spec->code);

	switch (format) {

	  case PIR:
	    strcat(outSpec,"=");
	    (seq->spec->isUser) ? 
	           strcat(outSpec,seq->spec->file) : 
                   strcat(outSpec,seq->spec->code);
	    switch (seq->type ) {
	      case PROTEIN:
	      case FRAGMENT: NewFileType(outSpec,".PEP"); break;
	            default: NewFileType(outSpec,".SEQ");
	    }
	    break;
	  case     GCG: NewFileType(outSpec, ".GCG"); break;
	  case  STADEN: NewFileType(outSpec, ".SDN"); break;
	  case      IG: NewFileType(outSpec, ".IG");  break;
	  case     IBI: NewFileType(outSpec, ".IBI");  break;
	  case STRIDER: NewFileType(outSpec, ".STR");  break;
	  case     RAW:
	       default:  NewFileType(outSpec, ".RAW"); break;
	}

	return(outSpec);

}  /* End of SeqToSpecName */

/****  WriteSeq  ***********************************************************
**
**  This writes a sequence entry into a file using the output format.
**  Returns the full VMS filename, complete with pathname and version no.
**
**    William Gilbert, Whitehead Institute
**
****************************************************************************/

char *WriteSeq(    char *outSpec,  /* Output sequence spec              */
               SeqEntry *seq,      /* Seq Entry data structure          */
                    int  format)   /* Output format                     */
{

#define SIZE   255    /* String size                         */

char line[SIZE], header[SIZE], code[SIZE], testCode[SIZE];
char outFName[SIZE];

char *cPos, *pStrand;

int lineSize, blockSize;
int count, nOut;

Boolean twoFiles;
Boolean doText;
char errMsg[SIZE];

static char outFileName[SIZE];    /* Static because it's the return value */

FILE *outFile, *inFile;

/*
** External function declarations and prototypes
*/

extern char *StrIndex(), *StrCollapse(), *StrToUpper(), *GetTime();
extern char *StrChange(), *DecodeType(int), *GetTime();

/*
** Process available text for formats which allow freely formated text.
*/

	switch ( format ) {
	  case PIR:
	  case GCG:
	    doText = (seq->text != NULL); break;
	  default:
	    doText = false; break;
	}

/*
** Set LineSize and BlockSize for variou formats
*/
	switch ( seq->spec->format ) {
	  case GCG:
	    lineSize = 50; blockSize= 10; break;
	  case PIR:
	  case IBI:
	    lineSize = 60; blockSize = 10; break;
	  case  IG:
	  case STRIDER:
	  default:
	    lineSize = blockSize= 80;
	}
/*
** Extract the the code from OutSpec for multi-entry files. If OutSpec just 
** has a filename then strip off the extension and use the filename as the code.
** Filenames are to the right of the equals.  
** N.B. does not yet support pathnames.
**
**    Outspec will look like either:
**
**                code=filename.ext     OR    filename.ext
*/

	if ( cPos = strchr(outSpec,';') ) *cPos = '\0'; /* Remove version */

	strcpy(code,outSpec);
	if ( cPos = strchr(code,'=') ) *cPos = '\0'; /* code=filename.ext */
	if ( cPos = strchr(code,'.') ) *cPos = '\0'; /* filename.ext */
	StrToUpper(code);
	strcpy(outFName,outSpec);
	if ( cPos = strchr(outFName,'=') ) strcpy(outFName, ++cPos);

/*
**  Are we dealing with a new or old sequence file?  Test for an existing
**  file set "twoFiles" flag accordingly.
*/

	if ( inFile = fopen(outFName, "r") ) {
	  if ( !(outFile = fopen(outFName, "w")) ) goto Error;
	  twoFiles = true;
	} else {
	  if ( !(outFile = fopen(outFName, "w")) ) goto Error;
	  twoFiles = false;
	}

/*
** Do what needs to be done to prepare the file for writing or over-writing
** the new sequence data.
**
**  PIR    - create header, read into filename until we find the entry to
**           replace or reach EOF. Save existing text if there is no new text.
**
**  IG     - Save any lines beginning with ";" these are comment characters.
**           Write out codeword.  Generate circular/linear flag.
**
**  STADEN - Translate symbols from IUPAC to STADEN, append end of sequence
**           character, "@".
**
**  GCG    - Save all text at the top of the file up to but not including the
**           line with ".." in it, unless we have new text. Convert "-"'s to
**           "."'s.  Recalculate CheckSum.  Write a new GCG descriptor line.
**
**  IBI    - Rewrite the LOCUS line and ORIGIN line.
**
**  STRIDER- Write out strider header line and sequnce title.
**
**  RAW    - Do nothing.
*/

	switch ( format ) {

	  case PIR:
	    strcpy(header, ">");
	    switch (seq->type ) {
	      case  PROTEIN: strcat(header,"P1;"); break;
	      case FRAGMENT: strcat(header,"F1;"); break;
	      case      DNA: 
	        seq->circular ? strcat(header,"DC;"):strcat(header,"DL;") ; break;
	      case      RNA:
	        seq->circular ? strcat(header,"RC;"):strcat(header,"RL;") ; break;
	      case     RRNA: strcat(header,"N1;"); break;
	      case     TRNA: strcat(header,"N3;"); break;
	            default: strcat(header,"XX;"); break;
	    }
	    strcat(header, code);

	    /* 
	    ** Read in/out until the beginning of entry to be overwritten
	    **  or End of File.
	    */
	    
	    if ( twoFiles ) {
	      testCode[0] = '\0';
	      while ( fgets(line, 255, inFile) ) {
	        if( line[0] == '>') {
	          strcpy(testCode, &line[4]);
	          if ( cPos = strchr(testCode, '\n') ) *cPos = '\0';
	          StrToUpper(StrCollapse(testCode));
	          if( strcmp(testCode,code) == 0 ) break;
	        }  
                fputs(line,outFile);
	      }
	    }

	    /* 
	    **  Write out the header and title lines.
	    */

	    fprintf(outFile,"%s\n", header);
	    fprintf(outFile,"%s\n", seq->title);
	    break;

	/****************************************************************/

	  case IG:
	  /*
	  **  For IG format save any lines beginning with ";" these
	  **  are comment characters
	  */
	    if ( twoFiles && !doText ) {
	      while ( fgets(line, 255, inFile) ) {
	        if ( line[0] != ';' )  break;
                fputs(line,outFile);
	      }
	    } else if ( doText ) {
	      if ( seq->text ) fputs(seq->text,outFile);
	    } else
	      fprintf(outFile, "; %s\n; %s\n", seq->title,seq->desc);

	    fprintf(outFile, "%s\n", code);
	    break;

	/****************************************************************/

	   case STADEN:
	    if (seq->type >= DNA ) {
	      StrChange(seq->mem,'M','5');
	      StrChange(seq->mem,'K','6');
	      StrChange(seq->mem,'W','7');
	      StrChange(seq->mem,'S','8');
	      StrChange(seq->mem,'m','5');
	      StrChange(seq->mem,'k','6');
	      StrChange(seq->mem,'w','7');
	      StrChange(seq->mem,'s','8');
	    }
	    break;

	/****************************************************************/

	  case GCG:
	    if ( twoFiles && !doText ) {
	      while ( fgets(line, 255, inFile) ) {
	        if ( StrIndex("..",line) )  break;
                fputs(line,outFile);
	      }
	    } else if ( doText ) {
	        if ( seq->text ) fputs(seq->text,outFile);
	    } else
	      fprintf(outFile, "  %s\n  %s\n\n", seq->title,seq->desc);

	    StrChange(seq->mem,'-','.');
	    seq->checkSum = CheckSum(seq->mem);
	    fprintf(outFile, "  %s  Length: %d  %s  Check: %d ..\n",
	            outFName, seq->length, GetTime(3), seq->checkSum);
	    fprintf(outFile,"        1 ");
	    break;
	  
	/****************************************************************/

	  case IBI:
	    fprintf(outFile,
	      "LOCUS       %s         %d BP           UPDATED  %s\n",
	       code, seq->length, GetTime(0));
	    if ( twoFiles && !doText ) {
	      while ( fgets(line, 255, inFile) ) {
	        if ( StrIndex("LOCUS",line) ) continue;
	        if ( StrIndex("ORIGIN",line) )  break;
                fputs(line,outFile);
	      }
	    } else if ( doText && seq->text ) {
	      if ( cPos = StrIndex("LOCUS     ",seq->text) )
	        strcpy(seq->text,strchr(cPos,'\n'));
	      if ( cPos = StrIndex("\nORIGIN",seq->text) )
	        *cPos = '\0';
	      fputs(seq->text,outFile);
	    }
	    fprintf(outFile, "ORIGIN      %s\n", seq->title);
	    fprintf(outFile,"        1 ");
	    break;

	/****************************************************************/

	   case STRIDER:
	    fprintf(outFile,"; ### from DNA Strider  %s\n",GetTime(3));
	    fprintf(outFile,"; %s sequence  %s length %d",
	      DecodeType(seq->type),seq->name,seq->length);
	    if ( seq->type <= PROTEIN )
	      fprintf(outFile," a.a. complete sequence\n; %s\n",seq->title);
	    else
	      fprintf(outFile," n.a. complete sequence\n; %s\n",seq->title);
	    break;

	/****************************************************************/

	  case RAW:
	  default:
	    break;

	}
/*
**  Write the sequence in Block/Line size.  "NOut" counts the number 
**  of symbols written in each line.  Support GCG and IBI line numbering
**  schemes.
*/

	nOut = 0;
	count = 1;
	pStrand = seq->mem;
	while( *pStrand ) {
	  fprintf(outFile, "%c", *pStrand);
	  nOut++;
	  count++;
	  if ( nOut == lineSize ) {
	    switch ( format ) {
	      case IBI:
	      case GCG:
	       fprintf(outFile,"\n%9d ",count);
	       break;
	      default:
	       fprintf(outFile, "\n");
	    }
	    nOut=0;
	  } else 
	    if ( nOut%blockSize == 0 ) fprintf(outFile, " ");
	  pStrand++;
	}

/*
** Depending on the format we have some finishing up to do.
**
** PIR     - Add the End of sequence character. Read past the old sequence
**           form the original file. If "DoText" is true, write out the new 
**           text comments, skip over the comments in the old file, if any.
**           Finally, copy the balance of the old file into the new file.
**
** IBI     - Add sequence terminator "//"
**
** IG      - Append linear/circular flag at end of sequence.
**
** STRIDER - Add sequence terminator "//"
**
** STADEN  - Add end of sequence character. Convert sequence symbols back 
**           to IUPAC.
**
** GCG     - Convert "."'s back to "-"'s. Recalc CheckSum.
*/
	
	switch ( format ) {

	  case PIR:

	    fprintf(outFile,"*\n");

	    /* Skip over sequence in the old file */

	    if ( twoFiles )
	      while ( fgets(line, 255, inFile) )
	        if( strchr(line,'*' ) ) break;

	    /* Skip over old comments, if any. */

	    if ( doText ) {
	      if ( seq->text ) fputs(seq->text,outFile);
	      if ( twoFiles )
	        while ( fgets(line, 255, inFile) )
	          if( *line == '>' ) {
	            fputs(line,outFile);
	            break;
	          }
	    }

	    /* Copy the remainder of the file */

	    if ( twoFiles ) {
	      while ( fgets(line, 255, inFile) )
	        fputs(line, outFile);
	    }
	    break;

	/****************************************************************/

	  case IBI:
	    fprintf(outFile,"\n//");  
	    break;

	/****************************************************************/

	  case IG:
	    fprintf(outFile,"%c",seq->circular ? '2' : '1');
	    break;

	/****************************************************************/

	  case STRIDER:
	    fprintf(outFile,"\n//");  
	    break;

	/****************************************************************/

	  case STADEN:
	    fprintf(outFile,"@");  
	    if (seq->type >= DNA ) {
	      StrChange(seq->mem,'5','M');
	      StrChange(seq->mem,'6','K');
	      StrChange(seq->mem,'7','W');
	      StrChange(seq->mem,'8','S');
	    }
	    break;

	/****************************************************************/

	  case GCG:
	    StrChange(seq->mem,'.','-');
	    seq->checkSum = CheckSum(seq->mem);
	    break;
	}	

	/*	fgetname(outFile, outFileName); NO IDEA PUT strcopy to compile il*/
	strcpy(outFileName,outFName);

	fclose(outFile);
	if ( twoFiles ) fclose(inFile);

	return(outFileName);

/*
** The output file could not be created.  Set error message and return
**  NULL
*/

Error:
	sprintf(errMsg, "Output file \"%s\" could not be created.", outFName);
	PostError(2,errMsg);
	return(NULL);

} /* End of WriteSeq */

/****  NewFileType  *********************************************************
**
**  Given an existing "Filename" change the file type (extension) to "Type"
**
**  "Type" may contain a leading "." if desired.
**
**  William A. Gilbert, Whitehead Institute
**
****************************************************************************/

Boolean NewFileType(char *fileName, char *type)

{
char *pChr;

	if ( pChr = strrchr(fileName, '.') ) *pChr = '\0';

	if ( *type == '.' )
	  strcat(fileName, type);
	else {
	  strcat(fileName, ".");
	  strcat(fileName, type);
	}

	return(true);

}  /* End of NewFileType */
/****  SetOSSymbol  *********************************************************
**
**  Sets the contents of a DCL global symbol to value and returns true if 
**  successful.  Symbol is the name of the new or existing symbol. Value is
**  not upcased or truncated.
**
**  This routine may be implemented on non-VMS machines by returning false.
**
**  William A. Gilbert, Whitehead Institute
**
***************************************************************************/

Boolean SetOSSymbol(char *symbol, char *value)

{
  return 0;
}
/****  GetOSSymbol  ********************************************************
**
**  Determines the contents of a DCL Symbol in String.
**  Returns true if the symbol was defined by DCL
**
**  William A. Gilbert, Whitehead Institute
**
*****************************************************************************/
Boolean GetOSSymbol(char *symbol, char *value)
{
  return(false);
	

} /* End of GetOSSymbol */


/***  DePath **************************************************************
**
**  Removes the VMS pathname from a filename
**
**  Will Gilbert, Whitehead Institute
**
**************************************************************************/

char *DePath(char *fileName)

{
char *cPos;

	if( cPos = strrchr(fileName, '/') ) strcpy(fileName, cPos+1);

	return(fileName);

} /* End of DePath */

/****  GetTime  **********************************************************
**  Creates a string giving the current data/time according to a set
**  of pre-defined formats.
**
**    0) VMS style data/time   DD-MMM-YYYY HH:MM
**
**    1) Time                                             HH:MM  
**    2) Full date                      Mmmmmmm, DD, YYYY
**    3) Full date, Time                Mmmmmmm, DD, YYYY HH:MM  (GCG style)
**    6) Weekday, Date         Weekday, Mmmmmmm, DD, YYYY
**
**  This routine relies on the VMS operatin system for the call to "time".
**  It should not be difficult to rewrite this routine for non-VMS systems.
**
**  William A. Gilbert, Whitehead Institute
**
*************************************************************************/
char *GetTime(int style)

{
#include <time.h>

struct tm *timeStrc;
time_t timeVal;

static char *weekday[7] = { "Sunday","Monday","Tuesday","Wednesday","Thursday",
                            "Friday","Saturday"};

static char *month[12]  = {"January","February","March","April","May","June",
                           "July","August","September","October","November",
                           "December"};

static int century  = 19;

static char string[80];


	time(&timeVal);
       	timeStrc = localtime(&timeVal);
	if ( timeStrc->tm_year < 88 ) century = 20;

	switch (style) {

	case 0:
	default:   /****** "DD-MMM-19YY HH:MM ", VMS style ******/

	  sprintf(string,"%02d-%3.3s-%02d%02d %02d:%02d",
	          timeStrc->tm_mday,
	          month[timeStrc->tm_mon],
	          century,
	          timeStrc->tm_year,
	          timeStrc->tm_hour,
	          timeStrc->tm_min);
	  break;

	case 1: /****** "HH:MM" ******/

	  sprintf(string,"%02d:%02d",
	          timeStrc->tm_hour,
	          timeStrc->tm_min);
	  break;

	case 2: /****** "Month day, year" ******/

	  sprintf(string,"%s %d, %02d%02d",
                  month[timeStrc->tm_mon],
	          timeStrc->tm_mday,
	          century,
	          timeStrc->tm_year);
	  break;

	case 3: /****** "Month day, year HH:MM" ******/

	  sprintf(string,"%s %d, %02d%02d %02d:%02d",
                  month[timeStrc->tm_mon],
	          timeStrc->tm_mday,
	          century,
	          timeStrc->tm_year,
	          timeStrc->tm_hour,
	          timeStrc->tm_min);
	  break;

	case 6: /****** "Weekday, Month day, year" ******/

	  sprintf(string,"%s, %s %d, %02d%02d",          
	          weekday[timeStrc->tm_wday],
                  month[timeStrc->tm_mon],
	          timeStrc->tm_mday,
	          century,
	          timeStrc->tm_year);
	  break;
	}

	return(&string[0]);

} /* End of GetTime */

/***  TransName  ************************************************************
**
**  Returns the operating system translation of Logicalname 
**  TransName returns false if there is no translation
**  When no translation can be made Translation will be of length zero.
**
**  For non-VMS systems the logical name is copied to the translation
**  and true is returned.
**
**  William A. Gilbert, Whitehead Institute
*
***************************************************************************/

Boolean TransName(char* translation, char* logicalName)

{

        strcpy(translation,logicalName);
	return(true);

}
  
