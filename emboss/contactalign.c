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

enum constant
    {
	enumDebugLevel        =  0,
	enumZeroContacts      =  0,
	enumFirstResType      =  0,
	enumFirstResTypeIndex =  0,
	enumNoCmapLine        =  0,
	enumTraceArrayOffset  =  1,
	enumLastContactIndex  =  1,
	enumHeaderCmapLine    =  1,
	enumBlankCmapLine     =  2,
	enumContactCmapLine   =  3,
	enumMaxContactsPerRes = 20,
	enumTotalResTypes     = 30,
	enumNoResTypeMatch    = 29,
	enumAsciiOffset       = 65,
	enumMaxCmapLineLen    = 80
    };

/* look-up array of three-letter codes */
/* XXXX THE PENULTIMATE THREE TLCs ARE DUMMIES */
static char *cArrayThreeLetterCodes[] =
    {
	"ALA\0","ASX\0","CYS\0","ASP\0","GLU\0","PHE\0","GLY\0","HIS\0",
	"ILE\0","---\0","LYS\0","LEU\0","MET\0","ASN\0","---\0","PRO\0",
	"GLN\0","ARG\0","SER\0","THR\0","---\0","VAL\0","TRP\0","XAA\0",
	"TYR\0","GLX\0","XXX\0","YYY\0","ZZZ\0","NO MATCH\0"
    };

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

typedef struct AjCmapHeader
{
    AjPStr ajpStrCmapId; 
    AjPStr ajpStrCmapDe;
    AjPStr ajpStrCmapEx;
    AjPStr ajpStrCmapMo;
    AjPStr ajpStrCmapCn;
    AjPStr ajpStrCmapIn;
    AjPStr ajpStrCmapSq;
} AjOCmapHeader;
#define AjPCmapHeader AjOCmapHeader*


static AjBool read_cmap_file (AjPFile ajpFileOriginalCmap,
			      ajint ajIntSeqLen,
			      AjPCmapHeader *pAjpFileOriginalCmapHeader,
			      AjPInt2d *pAjPInt2dCmapSummary,
			      AjPInt2d *pAjPInt2dCmapPositions,
			      AjPInt2d *pAjPInt2dCmapResTypes);

static AjPStr read_cmap_line (AjPFile ajpFileOriginalCmap);

static ajint type_cmap_line (AjPStr ajpStrCmapLine);

static AjBool load_header_line (AjPStr *pAjpStrCmapLine,
				AjPCmapHeader *pAjpHeaderToLoad);

static AjBool load_contact_line (AjPStr *pAjpStrCmapLine,
				 AjPContact *pAjpContactLoaded);

static AjBool write_cmap_file (AjPFile ajpFileUpdatedCmap,
			       ajint ajIntSeqLen,
			       AjPCmapHeader *pAjpFileUpdatedCmapHeader,
			       AjPInt2d *pAjPInt2dSummary,
			       AjPInt2d *pAjPInt2dCmapPositions,
			       AjPInt2d *pAjPInt2dCmapResTypes);

static AjPContact ajContactNew ();

static void ajContactDel (AjPContact *pAjpContactToDelete);

static AjBool print_contact (AjPContact ajpContactToPrint);

static AjBool write_cmap_header (AjPFile ajpFileUpdatedCmap,
				 AjPCmapHeader ajpHeaderToWrite);

static AjBool write_contact (AjPFile ajpFileUpdatedCmap,
			     AjPContact ajpContactToWrite);

static AjPCmapHeader ajCmapHeaderNew ();

static void ajCmapHeaderDel (AjPCmapHeader *pAjpCmapHeaderToDelete);

static AjBool print_cmap_header (AjPCmapHeader *pAjpCmapHeader);

static AjBool string3_to_ajint1 (AjPStr ajpStrThreeLetterCode);

static AjPStr ajint1_to_string3 (ajint ajIntCode);

static AjPInt2d get_int_map (ajint ajIntAcrossSeqLen);

static AjPInt2d get_cmap_summary (char *pcSeq);


/* DDDDEBUGGING FUNCTIONS */
static void debug_pair_score (AjPFloat2d ajpFloat2dPairScores,
			      AjPSeq ajpSeqDown,
			      AjPSeq ajpSeqAcross);

static void debug_Gotoh_score (AjPGotohCell **ajpGotohCellGotohScores,
			       ajint ajIntDownSeqLen,
			       ajint ajIntAcrossSeqLen);

static void debug_int_map (AjPInt2d *pAjpInt2dSummary,
			   AjPInt2d *ajpInt2dContacts,
			   char *pcSeq,
			   ajint ajIntSeqLen);

static void debug_int_summary (AjPInt2d *pAjpInt2dSummary,
			       ajint ajIntSeqLen);

/* @prog contactalign ********************************************************
** 
** align two protein sequence of unknown structure to protein sequence of
**  known structure, given residue contact information
**
******************************************************************************/

int main(int argc , char **argv)
{
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
    AjPStr ajpStrOriginalCmapFile = NULL;
    AjPFile ajpFileOriginalCmap = NULL;

    /* DDDDEBUG temporary contact map filename */
    AjPStr ajpStrUpdatedCmapFile = NULL;
    AjPFile ajpFileUpdatedCmap = NULL;
    
    /* contact map components */
    AjPCmapHeader ajpCmapHeader = NULL;
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    AjBool ajBoolOriginalCmapFileRead = ajFalse;
    AjBool ajBoolUpdatedCmapFileWritten = ajFalse;

    embInit( "contactalign" , argc , argv);
  
    /* DDDDEBUGGING: DEFAULT GAP AND EXTENSION PENALTIES SET BELOW */
    fGapPenalty       = -10.0;
    fExtensionPenalty =  -0.5;

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

    /* XXXX EVERYTHING FROM HERE IS TO DO WITH ALIGNMENT MODIFICATION */

    /* DDDD DEBUG DUMMY FILENAME BELOW */
    ajpStrOriginalCmapFile =
	ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/conts/d1aj3__.con");
    ajpFileOriginalCmap = ajFileNewIn(ajpStrOriginalCmapFile);

    pcSeqAcross = ajSeqCharCopy(ajpSeqAcross);

    /* reserve memory for objects representing contact map */
    ajpInt2dCmapSummary = get_cmap_summary(pcSeqAcross);
    ajpInt2dCmapResTypes = get_int_map(ajIntAcrossSeqLen);
    ajpInt2dCmapPositions = get_int_map(ajIntAcrossSeqLen);
    ajpCmapHeader = ajCmapHeaderNew();

    /* DDDDEBUGGING */
/*     debug_int_summary(&ajpInt2dCmapSummary, */
/* 		      ajIntAcrossSeqLen); */

/*     debug_int_map(&ajpInt2dCmapSummary, */
/* 		  &ajpInt2dCmapResTypes, */
/* 		  pcSeqAcross, */
/* 		  ajIntAcrossSeqLen); */

/*     debug_int_map(&ajpInt2dCmapSummary, */
/* 		  &ajpInt2dCmapPositions, */
/* 		  pcSeqAcross, */
/* 		  ajIntAcrossSeqLen); */

    /* read in original contact map */ 
    ajBoolOriginalCmapFileRead = read_cmap_file(ajpFileOriginalCmap,
						ajIntAcrossSeqLen,
						&ajpCmapHeader,
						&ajpInt2dCmapSummary,
						&ajpInt2dCmapResTypes,
						&ajpInt2dCmapPositions);

    /* DDDDEBUGGING */
    debug_int_summary(&ajpInt2dCmapSummary,
		      ajIntAcrossSeqLen);

    debug_int_map(&ajpInt2dCmapSummary,
		  &ajpInt2dCmapPositions,
		  pcSeqAcross,
		  ajIntAcrossSeqLen);

    print_cmap_header(&ajpCmapHeader);

    /* DDDD DEBUG DUMMY FILENAME BELOW */
    ajpStrUpdatedCmapFile =
	ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/conts/test.con");
    ajpFileUpdatedCmap = ajFileNewOut(ajpStrUpdatedCmapFile);

    /* write contact arrays to a new contact map file */
    ajBoolUpdatedCmapFileWritten =
	write_cmap_file (ajpFileUpdatedCmap,
			 ajIntAcrossSeqLen,
			 &ajpCmapHeader,
			 &ajpInt2dCmapSummary,
			 &ajpInt2dCmapPositions,
			 &ajpInt2dCmapResTypes);

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

    ajCmapHeaderDel(&ajpCmapHeader);

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

    for(ajIntCount = 0; ajIntCount < enumTotalResTypes; ajIntCount++)
	if(!ajStrCmpC(ajpStrThreeLetterCode,
		      cArrayThreeLetterCodes[ajIntCount]))
	{
	    ajIntCode = ajIntCount;
	}

    return ajIntCode;
}



/* @funcstatic ajint1_to_string3 *********************************************
**
** gives three-letter amino acid residue code for one-letter ASCII number
**
** @param [r] ajIntCode [ajint] one digit ASCII string A=0 29=NO_MATCH
** @return [AjPStr] code for three-letter equivalent amino acid residue
** @@
******************************************************************************/

static AjPStr ajint1_to_string3 (ajint ajIntCode)
{
    AjPStr ajpStrThreeLetterCode = NULL;
    
    if(ajIntCode < 0 || ajIntCode > 29)
    {
	ajpStrThreeLetterCode = NULL;
    }
    else
    {
	ajpStrThreeLetterCode = 
	    ajStrNewC(cArrayThreeLetterCodes[ajIntCode]);
    }

    return ajpStrThreeLetterCode;
}



/* @funcstatic read_cmap_file ************************************************
**
** reads ajxyz contact map file into two arrays of ints  
**
** @param [r] ajpFileCmap [AjPFile]  input file stream of current cmap
** @param [r] pAjPInt2dCmapResTypes [AjPInt2d *] contacts as residue types
** @param [r] pAjPInt2dCmapPositions [AjPInt2d *] contacts as positions in chain
** @return [AjBool] ajTrue if file successfully read
** @@
******************************************************************************/

static AjBool read_cmap_file (AjPFile ajpFileCmap,
			      ajint ajIntSeqLen,
			      AjPCmapHeader *pAjpCmapHeader,
			      AjPInt2d *pAjpInt2dCmapSummary,
			      AjPInt2d *pAjpInt2dCmapResTypes,
			      AjPInt2d *pAjpInt2dCmapPositions)
{
    AjBool ajBoolCmapFileRead = ajFalse; /* has the file been read? */

    ajint ajIntColumnCount;

    /* to store contact attributes from contact map */
    ajint ajIntTempFirstResType;
    ajint ajIntTempSecondResType;
    ajint ajIntTempFirstPosition;
    ajint ajIntTempSecondPosition;
    ajint ajIntTempContacts;

     /* object to hold single contact */
    AjPContact ajpContactTemp = NULL;

    /* structure to hold header text */
    AjPCmapHeader ajpCmapHeader = NULL;

    /* arrays to hold contacts */
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    /* what kind of contact map line has been read in? */
    ajint ajIntCmapLineType = enumNoCmapLine;

    /* string to store that contact map line */
    AjPStr ajpStrCmapLine = NULL;

    /* dereference pointers to same */
    ajpCmapHeader = *pAjpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* check file passed to function is usable */	
    if(!ajpFileCmap)
    {	
	ajWarn("function read_cmap_file cannot open passed filestream");	
	return ajFalse;
    }

    /* there are no contacts in any of the columns of the contact arrays yet */
    ajIntTempContacts = enumZeroContacts;

    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
	ajInt2dPut(&ajpInt2dCmapSummary,
		   enumLastContactIndex,
		   ajIntColumnCount,
		   ajIntTempContacts);

    /* reserve memory for current contact string */
    ajpStrCmapLine = ajStrNew();
    
    /* reserve memory for current contact object */
    ajpContactTemp = ajContactNew();

    /* reserve memory for current contact map header object */
    ajpCmapHeader = ajCmapHeaderNew();    

    /* read through contact map file until there are no more contacts */
    while( ( ajpStrCmapLine = read_cmap_line(ajpFileCmap) ) )
    {
	ajIntCmapLineType = type_cmap_line(ajpStrCmapLine);
	
	if(ajIntCmapLineType == enumContactCmapLine)
	{
	    load_contact_line(&ajpStrCmapLine, &ajpContactTemp);

	    /* load up temporary contact object with values read from line in Cmap file */ 
	    ajIntTempFirstResType = 
		string3_to_ajint1(ajpContactTemp->ajpStrFirstResType);
	    ajIntTempSecondResType = 
		string3_to_ajint1(ajpContactTemp->ajpStrSecondResType);
	    ajIntTempFirstPosition = 
		ajpContactTemp->ajIntFirstPosition;
	    ajIntTempSecondPosition = 
		ajpContactTemp->ajIntSecondPosition;
	    
	    /* get position of last contact in column */
	    ajIntTempContacts = ajInt2dGet(ajpInt2dCmapSummary,
					   enumLastContactIndex,
					   ajIntTempFirstPosition);
	    
	    /* increment contact counter at zeroth position in column */
	    ajIntTempContacts++;
	    
	    /*
	     * insert position of second residue in contact into
	     * column of ints in array of first residues
	     */
	    ajInt2dPut(pAjpInt2dCmapPositions,
		       ajIntTempContacts,
		       ajIntTempFirstPosition,
		       ajIntTempSecondPosition);
	    
	    /*
	     * insert type of second residue in contact into
	     * column of ints in array of first residues
	     */
	    ajInt2dPut(pAjpInt2dCmapResTypes,
		       ajIntTempContacts,
		       ajIntTempFirstPosition,
		       ajIntTempSecondResType);
	    
	    /*
	     * insert incremented contact counter
	     * into summary array
	     */
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumLastContactIndex,
		       ajIntTempFirstPosition,
		       ajIntTempContacts);
	    
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumLastContactIndex,
		       ajIntTempFirstPosition,
		       ajIntTempContacts);

	    /* DDDDEBUG: AND CHECK THAT'S WORKED */
	    print_contact(ajpContactTemp);
	}
	else if(ajIntCmapLineType == enumHeaderCmapLine)
	{
	    load_header_line(&ajpStrCmapLine,
			     &ajpCmapHeader);
	    
	}
    }

    /* free contact */
    ajContactDel(&ajpContactTemp);
    
    
    return ajBoolCmapFileRead;
}



/* @funcstatic read_cmap_line ************************************************
**
** reads single line from ajxyz contact map file and returns it as a string
**
** @param [r] ajpFileCmap [AjPFile]  input file stream of current cmap
** @return [AjPStr] contents of line read
** @@
******************************************************************************/

static AjPStr read_cmap_line (AjPFile ajpFileCmap)
{
    AjPStr ajpStrCmapLine = NULL;

    ajFileReadLine(ajpFileCmap, &ajpStrCmapLine);

    return ajpStrCmapLine;
}



/* @funcstatic type_cmap_line ************************************************
**
** based on first two chars, returns type of line from contact map file:
**
**
**
** @param [r] ajpFileCmap [AjPFile]  input file stream of current cmap
** @return [AjPStr] contents of line read
** @@
******************************************************************************/

static ajint type_cmap_line (AjPStr ajpStrCmapLine)
{
    ajint ajIntLineCode = enumNoCmapLine;

    if(!ajpStrCmapLine)
	ajWarn("NULL string passed to type_cmap_line");

    /* flag contact line as such */
    if(ajStrPrefixC(ajpStrCmapLine, "SM"))
    {
	ajIntLineCode = enumContactCmapLine;
    }
    /* flag blank Cmap lines */
    else if(ajStrPrefixC(ajpStrCmapLine, "XX"))
    {
	ajIntLineCode = enumBlankCmapLine;
    }
    /* flag Cmap header lines */
    else if( ajStrPrefixC(ajpStrCmapLine, "ID") ||
	     ajStrPrefixC(ajpStrCmapLine, "DE") ||
	     ajStrPrefixC(ajpStrCmapLine, "EX") ||
	     ajStrPrefixC(ajpStrCmapLine, "MO") ||
	     ajStrPrefixC(ajpStrCmapLine, "CN") ||
	     ajStrPrefixC(ajpStrCmapLine, "IN") ||
	     ajStrPrefixC(ajpStrCmapLine, "SQ") )
    {
	ajIntLineCode = enumHeaderCmapLine;	
    }
    else 
    {
	ajIntLineCode = enumNoCmapLine;
    }

    return ajIntLineCode;
}



/* @funcstatic load_contact_line ************************************************
**
** loads single contact from single line of ajxyz contact map file
**
** @param [r] pAjpStrCmapLine [AjPStr *] to contact line from Cmap file
** @param [r] pAjpContactParsed [AjPContact *] to contact object from line
** @return [AjBool] ajTrue if contact successfully parsed
** @@
******************************************************************************/

static AjBool load_contact_line (AjPStr *pAjpStrCmapLine,
				 AjPContact *pAjpContactLoaded)
{
    AjPStr ajpStrCmapLine = NULL;
    AjPContact ajpContactLoaded = NULL;

    AjPStr ajpStrPrefix = NULL;  /* first two characters on current line */
    char cPunctuation  = '\0';  /* separator on current line */
    ajint ajIntFirstPosition = 0; /* position of 1st residue in contact */
    ajint ajIntSecondPosition = 0;/* position of 2nd residue in contact */
    ajint ajIntConversionsDone = 0; /* n(successful conversions) by AjFmtScansS */
    AjPStr ajpStrFirstResType = NULL; /* 3-letter code for 1st residue in contact */
    AjPStr ajpStrSecondResType = NULL; /* 3-letter code for 2nd residue in contact */
    AjBool ajBoolContactLoaded = ajFalse;

    ajpContactLoaded = *pAjpContactLoaded;
    ajpStrCmapLine = *pAjpStrCmapLine;

    if(!ajpStrFirstResType)
	ajpStrFirstResType = ajStrNew();
    if(!ajpStrSecondResType)
	ajpStrSecondResType = ajStrNew();
    if(!ajpStrPrefix)
	ajpStrPrefix = ajStrNew();

    /* check string passed to function is usable */	
    if(!ajpStrCmapLine)
    {	
	ajWarn("NULL string passed to load_cmap_line");	
	return ajFalse;
    }

    ajIntConversionsDone = ajFmtScanS(ajpStrCmapLine, "%S %S %d %c %S %d",
				      &ajpStrPrefix,
				      &ajpStrFirstResType, &ajIntFirstPosition,
				      &cPunctuation,
				      &ajpStrSecondResType, &ajIntSecondPosition);

    if(ajIntConversionsDone == 6)
    {
	ajpContactLoaded->ajpStrFirstResType = ajpStrFirstResType;
	ajpContactLoaded->ajIntFirstPosition = ajIntFirstPosition;
	ajpContactLoaded->ajpStrSecondResType = ajpStrSecondResType;
	ajpContactLoaded->ajIntSecondPosition = ajIntSecondPosition;
	ajBoolContactLoaded = ajTrue;
    }

    ajStrDel(&ajpStrPrefix);
    
    return ajBoolContactLoaded;
}



/* @funcstatic load_header_line ************************************************
**
** loads single line from ajxyz contact map file into header object
**
** @param [r] pAjpStrCmapLine [AjPStr *] to header line from Cmap file
** @param [r] pAjpHeaderToLoad [AjPCmapHeader *] to header object from line
** @return [AjBool] ajTrue if line successfully loaded
** @@
******************************************************************************/

static AjBool load_header_line (AjPStr *pAjpStrCmapLine,
				 AjPCmapHeader *pAjpCmapHeaderToLoad)
{
    AjPStr ajpStrCmapLine = NULL;
    AjPCmapHeader ajpCmapHeaderToLoad = NULL;

    ajpCmapHeaderToLoad = *pAjpCmapHeaderToLoad;
    ajpStrCmapLine = *pAjpStrCmapLine;

    AjBool ajBoolCmapHeaderToLoad = ajFalse;

    /* check string passed to function is usable */	
    if(!ajpStrCmapLine)
    {	
	ajWarn("NULL string passed to load_header_line()");	
	return ajFalse;
    }

    if(ajStrPrefixC(ajpStrCmapLine, "ID"))
    {
	ajStrCopy(&ajpCmapHeaderToLoad->ajpStrCmapId, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    }
    /* read in other types of line */
    else if(ajStrPrefixC(ajpStrCmapLine, "DE"))
    {
	ajStrCopy(&ajpCmapHeaderToLoad->ajpStrCmapDe, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    }
    else if(ajStrPrefixC(ajpStrCmapLine, "EX"))
    {
	ajStrCopy(&ajpCmapHeaderToLoad->ajpStrCmapEx, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "MO"))
    {
	ajStrCopy(&ajpCmapHeaderToLoad->ajpStrCmapMo, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "CN"))
    {
	ajStrCopy(&ajpCmapHeaderToLoad->ajpStrCmapCn, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "IN"))
    {
	ajStrCopy(&ajpCmapHeaderToLoad->ajpStrCmapIn, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else if(ajStrPrefixC(ajpStrCmapLine, "SQ"))
    {
	ajStrCopy(&ajpCmapHeaderToLoad->ajpStrCmapSq, ajpStrCmapLine);
	ajBoolCmapHeaderToLoad = ajTrue;
    } 
    else 
    {
	ajBoolCmapHeaderToLoad = ajFalse;
    } 
    
    return ajBoolCmapHeaderToLoad;
}



/* @funcstatic write_cmap_file ************************************************
**
** writes ajxyz contact map file from header object and two arrays of ints  
**
** @param [r] ajpFileUpdatedCmap [AjPFile] output file stream of current cmap
** @param [r] ajIntSeqLen [ajint] number of residues in chain sequence
** @param [r] pAjPInt2dCmapSummary [AjPInt2d *] summary of contact map
** @param [r] pAjPInt2dCmapResTypes [AjPInt2d *] contacts as residue types
** @param [r] pAjPInt2dCmapPositions [AjPInt2d *] contacts as positions in chain
** @return [AjBool] ajTrue if file successfully written
** @@
******************************************************************************/

static AjBool write_cmap_file (AjPFile ajpFileUpdatedCmap,
			       ajint ajIntSeqLen,
			       AjPCmapHeader *pAjpCmapHeader,
			       AjPInt2d *pAjpInt2dCmapSummary,
			       AjPInt2d *pAjpInt2dCmapResTypes,
			       AjPInt2d *pAjpInt2dCmapPositions)
{
    AjBool ajBoolUpdatedCmapFileWritten = ajFalse; /* has the file been written? */

    /* counters and limits */
    ajint ajIntRow;
    ajint ajIntColumn;
    ajint ajIntColumnLen;

    /* to store contact attributes from contact map */
    ajint ajIntTempFirstResType;
    ajint ajIntTempSecondResType;
    AjPStr ajStrTempFirstResType;
    AjPStr ajStrTempSecondResType;
    ajint ajIntTempFirstPosition;
    ajint ajIntTempSecondPosition;

     /* object to hold single contact */
    AjPContact ajpContactTemp = NULL;
    /* structure to hold header text */
    AjPCmapHeader ajpCmapHeader = NULL;
    /* arrays to hold contacts */
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;
    /* dereference pointers to same */
    ajpCmapHeader = *pAjpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* check file passed to function is usable */	
    if(!ajpFileUpdatedCmap)
    {	
	ajWarn("function write_cmap_file cannot open passed filestream");	
	return ajFalse;
    }

    /* write header to file */
    ajBoolUpdatedCmapFileWritten = write_cmap_header(ajpFileUpdatedCmap,
						     ajpCmapHeader);

    /* reserve memory for current contact object */
    ajpContactTemp = ajContactNew();

    /* XXXX OUTER LOOP OVER FIRST RESIDUES IN CONTACTS */
    for(ajIntColumn= 0;ajIntColumn <= ajIntSeqLen;ajIntColumn++)
    {
	/* find end of each column */
	ajIntColumnLen = ajInt2dGet(ajpInt2dCmapSummary,
				    enumLastContactIndex,
				    ajIntColumn);

	/* XXXX INNER LOOP OVER CONTACTS IN COLUMN */
	for(ajIntRow = enumFirstResTypeIndex;ajIntRow <= ajIntColumnLen;ajIntRow++)
	{
	    /* read contact values from residue position array */
	    ajIntTempFirstResType = ajInt2dGet(ajpInt2dCmapResTypes,
					       ajIntRow,
					       ajIntColumn);
	    ajIntTempSecondResType = ajInt2dGet(ajpInt2dCmapResTypes,
						ajIntRow,
						ajIntColumn);
	    
	    /* load up temporary contact object with values from residue type array */ 
	    ajStrTempFirstResType = ajint1_to_string3(ajIntTempFirstResType);
	    ajStrTempSecondResType = ajint1_to_string3(ajIntTempSecondResType);

	    /* read temporary contact object with values from position array */
	    ajIntTempFirstPosition = ajInt2dGet(ajpInt2dCmapPositions,
						ajIntRow,
						ajIntColumn);
	    ajIntTempSecondPosition = ajInt2dGet(ajpInt2dCmapPositions,
						 ajIntRow,
						 ajIntColumn);

	    /* load up temporary contact object with values from residue position array */
	    ajpContactTemp->ajIntFirstPosition = ajIntTempFirstPosition;
	    ajpContactTemp->ajIntSecondPosition = ajIntTempSecondPosition;

	    /* DDDDEBUG: AND CHECK THAT'S WORKED */
	    print_contact(ajpContactTemp);

	    write_contact(ajpFileUpdatedCmap,
			  ajpContactTemp);

	}
    }

    /* free contact */
    ajContactDel(&ajpContactTemp);
    
    return ajBoolUpdatedCmapFileWritten;
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



/* @funcstatic ajCmapHeaderNew ***********************************************
**
** Default constructor for a contact map header object
**
** @return [AjpCmapHeader] Pointer to an AjPCmapHeader
** @@
******************************************************************************/

static AjPCmapHeader ajCmapHeaderNew(void)
{
    AjPCmapHeader ajpCmapHeaderReturned = NULL;

    AJNEW0(ajpCmapHeaderReturned);

    return ajpCmapHeaderReturned;
}



/* @funcstatic ajCmapHeaderDel **************************************************
**
** Default destructor for a contact map header object
**
** @param [r] pAjpCmapHeaderToDel [AjPCmapHeader *] header to be deleted
** @return [void]
** @@
******************************************************************************/

static void ajCmapHeaderDel (AjPCmapHeader* pthis)
{
    AjPCmapHeader thys = NULL;

    thys = pthis ? *pthis :0;
 
    if(!pthis)
	return;

    thys->ajpStrCmapId = NULL; 
    thys->ajpStrCmapDe = NULL;    
    thys->ajpStrCmapEx = NULL; 
    thys->ajpStrCmapMo = NULL;    
    thys->ajpStrCmapCn = NULL; 
    thys->ajpStrCmapIn = NULL;    
    thys->ajpStrCmapSq = NULL;    

    AJFREE(thys);
    *pthis = NULL;

    return;
}



/* @funcstatic get_cmap_summary *************************************************
**
** reserves memory for a 2-D array of ints to summarise a contact map 
**
** @param [r] ajIntAcrossSeqLen [AjPContact] number of residues in template
** @return [ajint **] array of ajints containing number of contacts and
** types of first residue in each contact
** @@
******************************************************************************/

static AjPInt2d get_cmap_summary (char *pcSeq)
{
    ajint ajIntAcrossSeqLen;
    ajint ajIntColumnCount;
    ajint ajIntTempResType;
    
    AjPInt2d ajpInt2dSummary = NULL;

    ajIntAcrossSeqLen = strlen(pcSeq);

    ajpInt2dSummary = ajInt2dNewL(ajIntAcrossSeqLen);

    for(ajIntColumnCount = 0;ajIntColumnCount < ajIntAcrossSeqLen;ajIntColumnCount++)
    {
	ajIntTempResType = (ajint) (*(pcSeq+ajIntColumnCount) - enumAsciiOffset);

	ajInt2dPut(&ajpInt2dSummary,
		   enumFirstResTypeIndex,
		   ajIntColumnCount,
		   ajIntTempResType);
	ajInt2dPut(&ajpInt2dSummary,
		   enumLastContactIndex,
		   ajIntColumnCount,
		   enumZeroContacts);
    }
	
    return ajpInt2dSummary;
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
    ajint ajIntRowCount;
    ajint ajIntColumnCount;
    
    AjPInt2d ajpInt2dContacts = NULL;

    ajpInt2dContacts = ajInt2dNewL(ajIntAcrossSeqLen);

    for(ajIntRowCount = 0;ajIntRowCount < enumMaxContactsPerRes;ajIntRowCount++)
    {
	for(ajIntColumnCount = 0;ajIntColumnCount < ajIntAcrossSeqLen;ajIntColumnCount++)
	{
	    ajInt2dPut(&ajpInt2dContacts,
		       ajIntRowCount,
		       ajIntColumnCount,
		       enumZeroContacts);
	}
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
** prints out contact object 
**
** @param [r] ajpContactToPrint [AjPContact] contact to be printed
** @return [AjBool] did it work?
** @@
******************************************************************************/

static AjBool print_contact (AjPContact ajpContactToPrint)
{
    AjBool ajBoolSuccess;
    ajBoolSuccess= ajFalse;

    /* print contact object attributes in EMBL-like format */
    ajFmtPrint("SM   ");
    ajFmtPrint("%S %d ; %S %d\n",
	       ajpContactToPrint->ajpStrFirstResType,
	       ajpContactToPrint->ajIntFirstPosition,
	       ajpContactToPrint->ajpStrSecondResType,
	       ajpContactToPrint->ajIntSecondPosition);

    ajBoolSuccess= ajTrue;
    
    return ajBoolSuccess;
}




/* @funcstatic write_contact *************************************************
**
** writes contact object to open contact map file
**
** @param [r] ajpFileUpdatedCmap [AjPFile] file to write contact map to
** @param [r] ajpContactToWrite [AjPContact] contact to be written
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

static AjBool write_contact (AjPFile ajpFileUpdatedCmap,
			     AjPContact ajpContactToWrite)
{
    AjBool ajBoolSuccess;
    AjPStr ajpStrContactToWrite;

    ajBoolSuccess = ajFalse;

    /* write contact object attributes in EMBL-like format */
    ajpStrContactToWrite = ajFmtStr("SM   %S %d ; %S %d\n",
				    ajpContactToWrite->ajpStrFirstResType,
				    ajpContactToWrite->ajIntFirstPosition,
				    ajpContactToWrite->ajpStrSecondResType,
				    ajpContactToWrite->ajIntSecondPosition);

    ajBoolSuccess = ajFileWriteStr(ajpFileUpdatedCmap,
				   ajpStrContactToWrite,
				   enumMaxCmapLineLen);
    
    
    return ajBoolSuccess;
}


/* @funcstatic write_cmap_header *************************************************
**
** writes header object to open contact map file
**
** @param [r] ajpFileUpdatedCmap [AjPFile] file to write contact map to
** @param [r] ajpHeaderToWrite [AjPCmapHeader] contact to be written
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

static AjBool write_cmap_header (AjPFile ajpFileUpdatedCmap,
				 AjPCmapHeader ajpHeaderToWrite)
{
    static AjBool write_line();

    AjBool ajBoolSuccess;
    AjPStr ajpStrTemp = NULL;

    ajBoolSuccess = ajFalse;

    ajpStrTemp = ajpHeaderToWrite->ajpStrCmapId;
    ajBoolSuccess = write_line(ajpStrTemp);

    ajpStrTemp = ajStrNewC("XX\n");
    ajBoolSuccess = write_line(ajpStrTemp);
    
    static AjBool write_line()
	{
	    ajBoolSuccess = (ajFileWriteStr(ajpFileUpdatedCmap,
					    ajpStrTemp,
					    enumMaxCmapLineLen));
	    return ajBoolSuccess;
	}
    
    return ajBoolSuccess;
}


/* @funcstatic print_cmap_header *********************************************
**
** prints out a contact map header
**
** @param [r] ajpCmapHeaderToPrint [AjPCmapHeader *] to header to be printed
** @return [AjBool] did it work?
** @@
******************************************************************************/

static AjBool print_cmap_header (AjPCmapHeader *pAjpCmapHeaderToPrint)
{
    static void empty_line();

    AjPCmapHeader ajpCmapHeaderToPrint;

    AjBool ajBoolSuccess;
    ajBoolSuccess= ajFalse;

    ajpCmapHeaderToPrint = *pAjpCmapHeaderToPrint;

    /* print out each line in contact map header in order */
    ajFmtPrint("ID   %S\n", ajpCmapHeaderToPrint->ajpStrCmapId);
    empty_line();
    ajFmtPrint("DE   %S\n", ajpCmapHeaderToPrint->ajpStrCmapDe);
    empty_line();
    ajFmtPrint("MO   %S\n", ajpCmapHeaderToPrint->ajpStrCmapMo);
    empty_line();
    ajFmtPrint("CN   %S\n", ajpCmapHeaderToPrint->ajpStrCmapCn);
    empty_line();
    ajFmtPrint("IN   %S\n", ajpCmapHeaderToPrint->ajpStrCmapIn);
    empty_line();
    ajFmtPrint("SQ   %S\n", ajpCmapHeaderToPrint->ajpStrCmapSq);
    empty_line();
	
    ajBoolSuccess= ajTrue;

    
    return ajBoolSuccess;

    static void empty_line()
	{
	    ajFmtPrint("XX\n");
	}
    
}


/* 17Mar04              debug_int_map()                  Damian Counsell  */
/*                                                                              */
/* checks that array of strings of one-letter contacts is properly built        */

static void debug_int_map(AjPInt2d *pAjpInt2dCmapSummary,
			  AjPInt2d *pAjpInt2dCmapContacts,
			  char *pcSeq,
			  ajint ajIntSeqLen)
{
    char cTemp;
    
    ajint ajIntRowCount;
    ajint ajIntColumnCount;
    ajint ajIntTempLastContact;
    ajint ajIntTempFirstRes;
    ajint ajIntTempSecondRes;
    
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapContacts = NULL;
    
    /* dereference pointers */
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapContacts = *pAjpInt2dCmapContacts;
    
    ajFmtPrint("====================================================================================================\n");
    ajFmtPrint("====================================================================================================\n");
    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
    {
	ajIntTempLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntColumnCount);
	ajFmtPrint("current count:\t%4d current residue type: %c\tnumber of contacts: %4d",
		   ajIntColumnCount,
		   pcSeq[ajIntColumnCount],
		   ajIntTempLastContact);
	
	cTemp = (char) ( ajInt2dGet(ajpInt2dCmapSummary,
				    enumFirstResTypeIndex,
				    ajIntColumnCount) + enumAsciiOffset);

	ajFmtPrint("\tcurrent int residue type: %c", cTemp);
	
	ajIntTempFirstRes = ajInt2dGet(ajpInt2dCmapSummary,
				       enumFirstResTypeIndex,
				       ajIntColumnCount);
	
	if(ajIntTempLastContact)
	    for (ajIntRowCount = 0;ajIntRowCount < ajIntTempLastContact;ajIntRowCount++)
	    {
		ajIntTempSecondRes = ajInt2dGet(ajpInt2dCmapContacts,
						ajIntRowCount,
						ajIntColumnCount);
		ajFmtPrint("%3d ", ajIntTempSecondRes);
	    }
	ajFmtPrint("\n");
	
    }

  ajFmtPrint("====================================================================================================\n");
  ajFmtPrint("====================================================================================================\n");

  return;
}

/* 24Mar04              debug_int_summary()                  Damian Counsell  */
/*                                                                            */
/* checks that array contacts is properly built                               */

static void debug_int_summary(AjPInt2d *pAjpInt2dCmapSummary,
			      ajint ajIntSeqLen)
{
    ajint ajIntColumnCount;
    
    char cFirstContactResTemp;
    ajint ajIntTempLastContact;
    
    AjPInt2d ajpInt2dCmapSummary = NULL;
    
    /* dereference pointer */
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    
    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");
    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");
    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
    {
	    cFirstContactResTemp = (char) ( ajInt2dGet(ajpInt2dCmapSummary,
						       enumFirstResTypeIndex,
						       ajIntColumnCount) + enumAsciiOffset);
	
	    ajIntTempLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					      enumLastContactIndex,
					      ajIntColumnCount);

	    ajFmtPrint("COLUMN %d FIRST CONTACT TYPE: %c\t LAST CONTACT NUMBER: %d\n",
		       ajIntColumnCount,
		       cFirstContactResTemp,
		       ajIntTempLastContact);
    }

    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");
    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");

  return;
}

