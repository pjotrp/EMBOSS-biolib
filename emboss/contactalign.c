/* @source contactalign application
**
** aligns two sequences: seqDownSequence and seqAcrossSequence
**  (eventually using a structure-weighted alignment method)---at the
**   moment implementing Needleman and Wunsch 
**
**
** @author: Copyright (C) Damian Counsell
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

enum constant {enumDebugLevel       =  0,
	       enumLastContact      =  0,
	       enumZeroContacts     =  0,
	       enumTraceArrayOffset =  1,
	       enumTotalResTypes    = 28,
	       enumNoResTypeMatch   = 29};

#include "emboss.h"
#include <math.h>

typedef struct AjContact
{
    /* position of residues in contact */
    ajint ajIntFirstPosition;
    ajint ajIntSecondPosition;
    /* 3-letter codes for 1st residue in contact */
    AjPStr ajpStrFirstResType; 
    AjPStr ajpStrSecondResType;
} AjOContact;
#define AjPContact AjOContact*

static void debug_pair_score (AjPFloat2d ajpFloat2dPairScores,
			      AjPSeq ajpSeqDown,
			      AjPSeq ajpSeqAcross);

static void debug_Gotoh_score (AjPGotohCell **ajpGotohCellGotohScores,
			       ajint ajIntDownSeqLen,
			       ajint ajIntAcrossSeqLen);

static AjBool read_cmap_file (AjPFile ajpFileCmap,
			      AjPInt2d *pAjPInt2dCmapPositions,
			      AjPInt2d *pAjPInt2dCmapResTypes);

static AjBool read_cmap_line (AjPFile ajpFileCmap,
			      AjPContact ajpContactToRead);


static AjBool parse_cmap_line (AjPStr ajpStrCmapLine,
			       AjPContact ajpContactLoaded);

static AjPContact ajContactNew ();

static void ajContactDel (AjPContact *pAjpContactToDelete);

static AjBool print_contact (AjPContact ajpContactToPrint);

static AjBool string3_to_ajint1 (AjPStr ajpStrThreeLetterCode);

static void debug_int_map (AjPInt2d *ajpInt2dTemplateContacts,
			   char *pcSeq,
			   ajint ajIntSeqLen);

static AjPInt2d get_int_map (ajint ajIntAcrossSeqLen);



/* @prog contactalign ********************************************************
** 
** align two protein sequence of unknown structure to protein sequence of
**  known structure, given residue contact information
**
******************************************************************************/

int main(int argc , char **argv)
{
    /*DEBUG */
    ajint ajIntCount;
    char *pcSeqAcross;

    /* sequence objects and copies */
    AjPSeq ajpSeqDown = NULL;
    AjPSeq ajpSeqAcross = NULL;
    AjPSeq ajpSeqDownCopy = NULL;
    AjPSeq ajpSeqAcrossCopy = NULL;
    /* object to write out aligned sequence */
    AjPSeqout ajpSeqoutAligned = NULL;
    /* prebuilt scoring AjpMatrix (e.g. BLOSUM62) */
    AjPMatrixf ajpMatrixfSubstitutionScoring = NULL;
    /* contact-based scoring AjpMatrix  */
    AjPMatrixf ajpMatrixfContactScoring = NULL;

    /* array for pair scores according to conventional scoring matrix */
    AjPFloat2d ajpFloat2dPairScores = NULL;
    /* array for pair scores according to contact-based scoring matrix */
    float **floatArrayContactScores = NULL;

    /* array for recursively summed scores in backtrace */
    AjPGotohCell **ajpGotohCellGotohScores;

    /* lengths of strings of numbers */
    ajint ajIntDownSeqLen;
    ajint ajIntAcrossSeqLen;
    ajint ajIntAlignmentLen;

    /* stack of backtraced cells */
    AjPList ajpListGotohCellsMaxScoringTrace = NULL;
    float fExtensionPenalty;
    float fGapPenalty;

    /* DDDDEBUG temporary contact map filename */
    AjPStr ajpStrCmapFile = NULL;
    AjPFile ajpFileCmap = NULL;
    
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    AjBool ajBoolCmapFileRead = ajFalse;

    embInit( "contactalign" , argc , argv);
  
    /* DDDDEBUGGING: DEFAULT GAP AND EXTENSION PENALTIES SET BELOW */
    fGapPenalty       = -10.0;
    fExtensionPenalty = -0.5;

    /* ajAcdGet functions access acd values embInit has put in memory */
    ajpSeqDown   = ajAcdGetSeq("down");
    ajpSeqAcross = ajAcdGetSeq("across");
    ajpMatrixfSubstitutionScoring = ajAcdGetMatrixf("substitutionscoringfile");
    ajpMatrixfContactScoring = ajAcdGetMatrixf("contactscoringfile");
    ajpSeqoutAligned  = ajAcdGetSeqout("aligned");

    /* get sequence lengths: no. rows and columns in Gotoh sum array */
    ajIntDownSeqLen = ajSeqLen(ajpSeqDown);
    ajIntAcrossSeqLen = ajSeqLen(ajpSeqAcross);

    /* score the sequences to produce a scoring AjpMatrix */
    ajpFloat2dPairScores = embGotohPairScore(ajpMatrixfSubstitutionScoring,
					     ajpSeqDown,
					     ajpSeqAcross,
					     fExtensionPenalty);

    /* DDDDEBUG CHECK THAT WORKED */
    if(enumDebugLevel > 1)
    {
	debug_pair_score(ajpFloat2dPairScores, ajpSeqDown, ajpSeqAcross);
    }
    
    /* initialize Gotoh score array */
    ajpGotohCellGotohScores = embGotohCellGetArray(ajIntDownSeqLen +
						   enumTraceArrayOffset,
						   ajIntAcrossSeqLen +
						   enumTraceArrayOffset);
    
    /* DDDDEBUG CHECK THAT WORKED */
    if(enumDebugLevel > 1)
    {
	debug_Gotoh_score(ajpGotohCellGotohScores, ajIntDownSeqLen, ajIntAcrossSeqLen);
    }

    embGotohCellCalculateSumScore(ajpFloat2dPairScores,
				  ajpSeqDown,
				  ajpSeqAcross,
				  ajpGotohCellGotohScores,
				  fGapPenalty,
				  fExtensionPenalty);

    /* DDDDEBUG CHECK THAT WORKED */
    if(enumDebugLevel > 1)
    {
	debug_Gotoh_score(ajpGotohCellGotohScores, ajIntDownSeqLen, ajIntAcrossSeqLen);
    }
    
    /* copy original sequences into new sequence objects */
    ajpSeqDownCopy = ajSeqNewS(ajpSeqDown);
    ajpSeqAcrossCopy = ajSeqNewS(ajpSeqAcross);

    /* pile up the backtraced elements onto the stack */
    ajpListGotohCellsMaxScoringTrace = ajListNew();
    ajIntAlignmentLen = embGotohCellBacktrace(ajpGotohCellGotohScores,
					      ajpSeqDownCopy,
					      ajpSeqAcrossCopy,
					      ajpListGotohCellsMaxScoringTrace);
    
    /* read the backtraced elements off the stack */
    embGotohReadOffBacktrace(ajpListGotohCellsMaxScoringTrace, ajpSeqDownCopy, ajpSeqAcrossCopy);

    /* DDDD DEBUG DUMMY FILENAME BELOW */
    ajpStrCmapFile = ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/conts/d1aj3__.con");
    ajpFileCmap = ajFileNewIn(ajpStrCmapFile);

    ajpInt2dCmapResTypes = get_int_map(ajIntAcrossSeqLen);
    ajpInt2dCmapPositions = get_int_map(ajIntAcrossSeqLen);

    /* read in some contacts from a map */
    for(ajIntCount = 0;ajIntCount < 10;ajIntCount++)

    pcSeqAcross = ajSeqCharCopy(ajpSeqAcross);

    /* DDDDEBUGGING */
    debug_int_map(&ajpInt2dCmapResTypes, pcSeqAcross, ajIntAcrossSeqLen);
    debug_int_map(&ajpInt2dCmapPositions, pcSeqAcross, ajIntAcrossSeqLen);

    /*
     * read the 2-and-half-D data---the contacts
     *  from the Cmap file---into a 1-D array
     *  of AjPStrs, each containing the one-letter
     *  codes for the corresponding residue's
     *  contact partners
     */

    ajBoolCmapFileRead = read_cmap_file(ajpFileCmap,
					&ajpInt2dCmapResTypes,
					&ajpInt2dCmapPositions);


    /* DDDDEBUGGING */
    debug_int_map(&ajpInt2dCmapResTypes, pcSeqAcross, ajIntAcrossSeqLen);
    debug_int_map(&ajpInt2dCmapPositions, pcSeqAcross, ajIntAcrossSeqLen);

    /* XXXX LOOK UP PROBABILITY SCORE FOR PAIR */

    /* convert the input float AjpMatrix to a 2D array of scores */ 
    floatArrayContactScores = ajMatrixfArray(ajpMatrixfContactScoring);

    /* XXXX FIND CORRESPONDING FIRST AND SECOND RESIDUES IN ORIGINAL TEMPLATE SEQUENCE */
    


    /* XXXX FIND CORRESPONDING FIRST AND SECOND RESIDUES IN ORIGINAL QUERY SEQUENCE */

    /* XXXX COUNT REAL RESIDUES IN FIRST SEQUENCE */

    /* XXXX LOOK UP FIRST CONTACT RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN QUERY SEQUENCE */

    

    /* XXXX LOOK UP SECOND CONTACT RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN QUERY SEQUENCE */

    /* XXXX LOOK UP FIRST RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN SECOND SEQUENCE */

    /* XXXX LOOK UP PROBABILITY SCORE FOR NEW PAIR */

    /* XXXX FIND RELEVANT CELL IN ALIGNMENT MATRIX AND PENALIZE IT */    

    /* XXXX YOU NEED SOME CODE HERE TO COMPARE THE PROBABILITIES OF EACH PAIR */

    /* write out "aligned" sequences  */
    ajSeqWrite(ajpSeqoutAligned, ajpSeqDownCopy);
    ajSeqWrite(ajpSeqoutAligned, ajpSeqAcrossCopy);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}



/* @funcstatic string3_to_ajint1 *********************************************
**
** gives one-letter ASCII number for three-letter amino acid residue code
**
** @param [r] ajpStrThreeLetterCode [AjPStr] three-letter code string
** @return [ajint] code for one-letter equivalent A=0 29=NO MATCH
** @@
******************************************************************************/

static ajint string3_to_ajint1 (AjPStr ajpStrThreeLetterCode)
{
    ajint ajIntCount;
    ajint ajIntCode = enumNoResTypeMatch;

    /* look-up array of three-letter codes */
    /* XXXX THE LAST TWO TLCs ARE DUMMIES FOR THE MOMENT */
    static char *cArrayThreeLetterCodes[] =
	{
	    "ALA\0","ASX\0","CYS\0","ASP\0","GLU\0","PHE\0","GLY\0","HIS\0",
	    "ILE\0","---\0","LYS\0","LEU\0","MET\0","ASN\0","---\0","PRO\0",
	    "GLN\0","ARG\0","SER\0","THR\0","---\0","VAL\0","TRP\0","XAA\0",
	    "TYR\0","GLX\0","XXX","YYY" 
	};

    for(ajIntCount = 0; ajIntCount < enumTotalResTypes; ajIntCount++)
	if(!ajStrCmpC(ajpStrThreeLetterCode,
		      cArrayThreeLetterCodes[ajIntCount]))
	{
	    ajIntCode = ajIntCount;
	}

    return ajIntCode;
}



/* @funcstatic read_cmap_file ************************************************
**
** reads ajxyz contact map file into two arrays: one of strings, one of ints  
**
** @param [r] ajpFileCmap [AjPFile]  input file stream of current cmap
** @param [r] pAjPInt2dCmapResTypes [AjPInt2d *] contacts as residue types
** @param [r] pAjPInt2dCmapPositions [AjPInt2d *] contacts as positions in chain
** @return [AjBool] ajTrue if file successfully read
** @@
******************************************************************************/

static AjBool read_cmap_file (AjPFile ajpFileCmap,
			      AjPInt2d *pAjpInt2dCmapResTypes,
			      AjPInt2d *pAjpInt2dCmapPositions)
{
    static AjPStr ajpStrCmapLine = NULL;  /* current line of Cmap file */
    AjBool ajBoolCmapFileRead = ajFalse;

    /* to store residue-specific contacts from template's contact map */
    ajint ajIntTempFirstResType;
    ajint ajIntTempSecondResType;
    ajint ajIntTempFirstPosition;
    ajint ajIntTempSecondPosition;
    ajint ajIntTempLastResRowIndex;

     /* object to hold single contact */
    AjPContact ajpContactTemp = NULL;

    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;
    
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* check file passed to function is usable */	
    if(!ajpFileCmap)
    {	
	ajWarn("function read_cmap_file cannot open passed filestream");	
	return ajFalse;
    }

    ajpStrCmapLine = ajStrNew();

    /* skip non-contact lines in contact map file */
    while(ajFileReadLine(ajpFileCmap, &ajpStrCmapLine) &&
	  !(ajStrPrefixC(ajpStrCmapLine, "SM")) );

    /* XXXX PERHAPS THERE SHOULD BE A SANITY CHECK HERE, */
    /*  USING THE DATA IN THE HEADER SECTION OF THE FILE */

    ajIntTempLastResRowIndex = enumZeroContacts;

    ajpContactTemp = ajContactNew();

    while(read_cmap_line(ajpFileCmap, ajpContactTemp))
    {

	/* load up temporary contact object with values read from line in Cmap file */ 
	ajIntTempFirstResType = string3_to_ajint1(ajpContactTemp->ajpStrFirstResType);
	ajIntTempSecondResType = string3_to_ajint1(ajpContactTemp->ajpStrSecondResType);
	ajIntTempFirstPosition = ajpContactTemp->ajIntFirstPosition;
	ajIntTempSecondPosition = ajpContactTemp->ajIntSecondPosition;

	/* get position of last contact in column */
	ajIntTempLastResRowIndex = ajInt2dGet(ajpInt2dCmapPositions,
					      enumLastContact,
					      ajIntTempFirstPosition);

	/* increment contact counter at zeroth position in column */
	ajIntTempLastResRowIndex++;

	/* insert second residue position in contact into column of ints in array of first residues */
	ajInt2dPut(&ajpInt2dCmapPositions,
		   ajIntTempLastResRowIndex,
		   ajIntTempFirstPosition,
		   ajIntTempSecondPosition);

	/* insert second residue type in contact into column of ints in array of first residues */
	ajInt2dPut(&ajpInt2dCmapResTypes,
		   ajIntTempLastResRowIndex,
		   ajIntTempFirstPosition,
		   ajIntTempSecondResType);

	/* insert incremented contact counter at zeroth cell of corresponding array columns */
	ajInt2dPut(&ajpInt2dCmapResTypes,
		   enumLastContact,
		   ajIntTempFirstPosition,
		   ajIntTempLastResRowIndex);

	ajInt2dPut(&ajpInt2dCmapPositions,
		   enumLastContact,
		   ajIntTempFirstPosition,
		   ajIntTempLastResRowIndex);

	/* DDDDEBUG: AND CHECK THAT'S WORKED */
	print_contact(ajpContactTemp);
    }

    /* free contact */
    ajContactDel(&ajpContactTemp);
    
    
    return ajBoolCmapFileRead;
}



/* @funcstatic read_cmap_line ************************************************
**
** reads single contact from ajxyz contact map file  
**
** @param [r] ajpFileCmap [AjPFile]  input file stream of current cmap
** @param [r] pAjpContactToRead [AjPContact] to residue contact object
** @return [AjPStr] contents of line read
** @@
******************************************************************************/

static AjBool read_cmap_line (AjPFile ajpFileCmap, AjPContact ajpContactToRead)
{
    AjPStr ajpStrTempCmapLine = NULL;  /* current line of Cmap file */
    AjBool ajBoolContactRead = ajFalse;

    ajpStrTempCmapLine = ajStrNew();

    ajFileReadLine(ajpFileCmap, &ajpStrTempCmapLine);

    ajBoolContactRead = parse_cmap_line(ajpStrTempCmapLine, ajpContactToRead);

    ajStrDel(&ajpStrTempCmapLine);
    
    return ajBoolContactRead;
}



/* @funcstatic parse_cmap_line ************************************************
**
** loads single contact from AjPStr of single line of ajxyz contact map file
**
** @param [r] ajpStrCmapLine [AjPStr] to contact line from Cmap file
** @param [r] ajpContactParsed [AjPContact] to contact object from line
** @return [AjBool] ajTrue if contact successfully parsed
** @@
******************************************************************************/

static AjBool parse_cmap_line (AjPStr ajpStrCmapLine,
			       AjPContact ajpContactParsed)
{
    AjPStr ajpStrPrefix = NULL;  /* first two characters on current line */
    char cPunctuation  = '\0';  /* separator on current line */
    ajint ajIntFirstPosition = 0; /* position of 1st residue in contact */
    ajint ajIntSecondPosition = 0;/* position of 2nd residue in contact */
    ajint ajIntConversionsDone = 0; /* n(successful conversions) by AjFmtScansS */
    AjPStr ajpStrFirstResType = NULL; /* 3-letter code for 1st residue in contact */
    AjPStr ajpStrSecondResType = NULL; /* 3-letter code for 2nd residue in contact */
    AjBool ajBoolContactParsed = ajFalse;

    ajpStrFirstResType = ajStrNew();
    ajpStrSecondResType = ajStrNew();
    ajpStrPrefix = ajStrNew();

    /* check file passed to function is usable */	
    if(!ajpStrCmapLine)
    {	
	ajWarn("NULL string passed to parse_cmap_line");	
	return ajFalse;
    }

    ajIntConversionsDone = ajFmtScanS(ajpStrCmapLine, "%S %S %d %c %S %d",
				      &ajpStrPrefix,
				      &ajpStrFirstResType, &ajIntFirstPosition,
				      &cPunctuation,
				      &ajpStrSecondResType, &ajIntSecondPosition);

    if(ajIntConversionsDone == 6)
    {
	ajpContactParsed->ajpStrFirstResType = ajpStrFirstResType;
	ajpContactParsed->ajIntFirstPosition = ajIntFirstPosition;
	ajpContactParsed->ajpStrSecondResType = ajpStrSecondResType;
	ajpContactParsed->ajIntSecondPosition = ajIntSecondPosition;
	ajBoolContactParsed = ajTrue;
    }

    ajStrDel(&ajpStrPrefix);
    
    return ajBoolContactParsed;
}



/* @funcstatic ajContactNew **************************************************
**
** Default constructor for a Cmap contact object
**
** @return [AjpContact] Pointer to an AjPContact
** @@
******************************************************************************/

static AjPContact ajContactNew(void)
{
    AjPContact ajpContactReturned = NULL;

    AJNEW0(ajpContactReturned);

    return ajpContactReturned;
}



/* @funcstatic ajContactDel **************************************************
**
** Default destructor for a Cmap contact object
**
** @param [r] pAjpContactToDel [AjPContact *] contact to be deleted
** @return [void]
** @@
******************************************************************************/

static void ajContactDel(AjPContact* pthis)
{
    AjPContact thys = NULL;

    thys = pthis ? *pthis :0;
 
    if(!pthis)
	return;

    thys->ajIntFirstPosition = 0;
    thys->ajIntSecondPosition = 0;
    thys->ajpStrFirstResType = NULL; 
    thys->ajpStrSecondResType = NULL;    

    AJFREE(thys);
    *pthis = NULL;

    return;
}


/* @funcstatic get_int_map *************************************************
**
** reserves memory for a 2-D array of ints to store a contact map 
**
** @param [r] ajIntAcrossSeqLen [AjPContact] number of residues in template
** @return [ajint **] array of ajints containing position IDs for each contact
** @@
******************************************************************************/

static AjPInt2d get_int_map (ajint ajIntAcrossSeqLen)
{
    ajint ajIntCount;
    
    AjPInt2d ajpInt2dContacts = NULL;

    ajpInt2dContacts = ajInt2dNewL(ajIntAcrossSeqLen);

    for(ajIntCount = 0;ajIntCount < ajIntAcrossSeqLen;ajIntCount++)
    {
	ajInt2dPut(&ajpInt2dContacts,
		   enumLastContact,
		   ajIntCount,
		   enumZeroContacts);
    }
	
    return ajpInt2dContacts;
}




/*   EVERYTHING IS DDDDEBUGGING CODE FROM HERE ONWARDS */
/*   EVERYTHING IS DDDDEBUGGING CODE FROM HERE ONWARDS */
/*   EVERYTHING IS DDDDEBUGGING CODE FROM HERE ONWARDS */
/*   EVERYTHING IS DDDDEBUGGING CODE FROM HERE ONWARDS */

/* 16Jan02                debug_Gotoh_score()                  Damian Counsell  */
/*                                                                              */
/* prints out result of Gotoh scoring                                           */

void debug_Gotoh_score (AjPGotohCell **ajpGotohCellGotohScores, ajint ajIntDownSeqLen, ajint ajIntAcrossSeqLen)
{
  ajint ajIntRowCount;
  ajint ajIntColumnCount;

  for(ajIntRowCount = 0 ; ajIntRowCount < ajIntDownSeqLen + enumTraceArrayOffset ; ajIntRowCount++)
    {
      for(ajIntColumnCount = 0 ; ajIntColumnCount < ajIntAcrossSeqLen + enumTraceArrayOffset ; ajIntColumnCount++)
	{
	  ajFmtPrint("====================================================================================================\n");
	  ajFmtPrint("GOTOH (%4d, %4d):\ti pointer: %4d j pointer: %4d\tsubscore: %3.3f\ttemplate residue:\t%c\tquery residue:\t%c\tisIndel?: %B\n", \
		       ajIntRowCount, ajIntColumnCount, \
		       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> ajIntRowPointer, \
		       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> ajIntColumnPointer, \
		       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> fSubScore, \
                       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> cDownResidue ,\
                       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> cAcrossResidue, \
		       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> ajBoolIsIndel);
	  ajFmtPrint("====================================================================================================\n");
	}
    }

  return;
}



/* 04May01                debug_pair_score()                 Damian Counsell  */
/*                                                                            */
/* prints out result of pair-scoring                                          */

void debug_pair_score(AjPFloat2d ajpFloat2dPairScores , AjPSeq ajpSeqDown , AjPSeq ajpSeqAcross)
{
  ajint ajIntMaxRow;
  ajint ajIntMaxColumn;
  ajint ajIntRow;
  ajint ajIntColumn;

  /* get size of sequences---and therefore good array values */
  ajIntMaxRow = ajSeqLen(ajpSeqDown) + enumTraceArrayOffset;
  ajIntMaxColumn = ajSeqLen(ajpSeqAcross) + enumTraceArrayOffset;

  ajFmtPrint( "ROWS:\t%d\tCOLUMNS:\t%d\n" , ajIntMaxRow , ajIntMaxColumn );

  /* print out various results for debugging purposes */
  for( ajIntRow = 0 ; ajIntRow < ajIntMaxRow ; ajIntRow++ )
  {
      for( ajIntColumn = 0 ; ajIntColumn < ajIntMaxColumn ; ajIntColumn++ )
      {
	  ajFmtPrint( "==================================================\n" );
	  ajFmtPrint( "ARRAY ENTRY: Row:\t%d\tColumn:\t%d\tScore:\t%f\n" , \
		      ajIntRow , ajIntColumn , \
		      ajFloat2dGet( ajpFloat2dPairScores , ajIntRow , ajIntColumn ) );
	  ajFmtPrint( "==================================================\n" );
      }
  }

  return;
}



/* @funcstatic print_contact *************************************************
**
** prints a contact object out for debugging 
**
** @param [r] ajpContactToPrint [AjPContact] contact to be printed
** @return [AjBool] did it work?
** @@
******************************************************************************/

static AjBool print_contact (AjPContact ajpContactToPrint)
{
    AjBool ajBoolSuccess;
    ajBoolSuccess= AJFALSE;

    /* print out each residue in contact object in order */
    ajFmtPrint("\n\nFIRST RESIDUE residue type:\t%S\tresidue number:\t%d\n",
	       ajpContactToPrint->ajpStrFirstResType,
	       ajpContactToPrint->ajIntFirstPosition);
    ajFmtPrint("\n\nSECOND RESIDUE residue type:\t%S\tresidue number:\t%d\n",    
	       ajpContactToPrint->ajpStrSecondResType,
	       ajpContactToPrint->ajIntSecondPosition);

    ajBoolSuccess= AJTRUE;
    
    return ajBoolSuccess;
}



/* 17Mar04              debug_int_map()                  Damian Counsell  */
/*                                                                              */
/* checks that array of strings of one-letter contacts is properly built        */

static void debug_int_map(AjPInt2d *ajpInt2dTemplateContacts,
			  char *pcSeq,
			  ajint ajIntSeqLen)
{
  ajint ajIntCount;
  ajint ajIntContactCount;
  ajint ajIntTempLastContact;
  ajint ajIntTempFirstRes;
  ajint ajIntTempSecondRes;

  ajFmtPrint("====================================================================================================\n");
  ajFmtPrint("====================================================================================================\n");
  for(ajIntCount = 0; ajIntCount < ajIntSeqLen; ajIntCount++)
    {
	ajIntTempLastContact = ajInt2dGet(*ajpInt2dTemplateContacts,
					  enumLastContact,
					  ajIntCount);
	ajFmtPrint("current count:\t%4d current residue type: %c\tnumber of contacts: %4d",
		   ajIntCount,
		   pcSeq[ajIntCount],
		   ajIntTempLastContact);
	ajFmtPrint("\tcurrent contact ints: ");
	
	ajIntTempFirstRes = ajIntCount;
	
	if(ajIntTempLastContact)
	    for (ajIntContactCount = 1;ajIntContactCount <= ajIntTempLastContact;ajIntContactCount++)
	    {
		ajIntTempSecondRes = ajInt2dGet(*ajpInt2dTemplateContacts,
						ajIntContactCount,
						ajIntCount);
		ajFmtPrint("%3d ", ajIntTempSecondRes);
	    }
	ajFmtPrint("\n");
	
    }

  ajFmtPrint("====================================================================================================\n");
  ajFmtPrint("====================================================================================================\n");

  return;
}

