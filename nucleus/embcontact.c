/******************************************************************************
** @source NUCLEUS contact functions for alignment
**
** functions used to handle contact map files for refining sequence-structure
**                                                                 alignment
**
**
** @author Copyright (C) 2004 Damian Counsell
** @version $Revision: 1.1 $
** @modified $Date: 2004/06/16 17:08:23 $
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/




/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ajax.h"
#include "embcontact.h"

/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

enum constant
    {
	enumDebugLevel        =  0,
	enumFirstResType      =  0,
	enumFirstResTypeIndex =  0,
	enumNoCmapLine        =  0,
	enumZeroContacts      =  0,
	enumArrayOffset       =  1,
	enumHeaderCmapLine    =  1,
	enumLastContactIndex  =  1,
	enumBlankCmapLine     =  2,
	enumContactCmapLine   =  3,
	enumSeqCmapLine       =  4,
	enumMaxContactsPerRes = 20,
	enumTotalResTypes     = 30,
	enumNoResTypeMatch    = 29,
 	enumAsciiOffset       = 65,
	enumAsciiAsterisk     = 91,
	enumAsciiQuestionMark = 92
    };

/* look-up array of three-letter codes */
/* XXXX THE PENULTIMATE THREE TLCs ARE DUMMIES */
static char *cArrayThreeLetterCodes[enumTotalResTypes] =
    {
	"ALA\0","ASX\0","CYS\0","ASP\0","GLU\0","PHE\0","GLY\0","HIS\0",
	"ILE\0","---\0","LYS\0","LEU\0","MET\0","ASN\0","---\0","PRO\0",
	"GLN\0","ARG\0","SER\0","THR\0","SEC\0","VAL\0","TRP\0","XAA\0",
	"TYR\0","GLX\0","XXX\0","YYY\0","ZZZ\0","NO MATCH\0"
    };
static char cOneLetterCodes[enumTotalResTypes] =
    {
	'A','B','C','D','E','F','G','H',
	'I','\0','K','L','M','N','\0','P',
	'Q','R','S','T','U','V','W','X',
	'Y','Z','\0','\0','\0','\0'
    };

/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

/* ==================================================================== */
/* ========================= constructors ============================= */
/* ==================================================================== */

/* @section AjPContact Constructors ******************************************
**
** All constructors return a new AjPContact by pointer. It is the
** responsibility of the user to first destroy any previous cell. The target
** pointer does not need to be initialised to NULL, but it is good programming
** practice to do so anyway.
**
******************************************************************************/

/* @func embContactNew *****************************************************
**
** Default constructor for NUCLEUS Cmap contact objects
**
** @return [AjPContact] pointer to an AjPContact
** @category new [AjPContact] default constructor
** @@
******************************************************************************/

AjPContact ajContactNew(void)
{
    AjPContact ajpContactReturned = NULL;

    AJNEW0(ajpContactReturned);

    return ajpContactReturned;
}


/* @section AjPHeader Constructors ******************************************
**
** All constructors return a new AjPCmapHeader by pointer. It is the
** responsibility of the user to first destroy any previous contact map file
** header object. The target pointer does not need to be initialised to NULL,
** but it is good programming practice to do so anyway.
**
******************************************************************************/



/* @func ajCmapHeaderNew *****************************************************
**
** Default constructor for a contact map header object
**
** @return [AjPCmapHeader] Pointer to an AjPCmapHeader
** @@
******************************************************************************/

AjPCmapHeader ajCmapHeaderNew(void)
{
    AjPCmapHeader ajpCmapHeaderReturned = NULL;

    AJNEW0(ajpCmapHeaderReturned);

    return ajpCmapHeaderReturned;
}

/* ==================================================================== */
/* =========================== destructor ============================= */
/* ==================================================================== */


/* @section Contact Destructors **********************************************
**
** Destruction is achieved by deleting the pointer to the contact object and
**  freeing the associated memory
**
******************************************************************************/

/* @func embContactDel *******************************************************
**
** Default destructor for NUCLEUS Cmap contact objects
**
** If the given pointer is NULL, or a NULL pointer, simply returns.
**
** @param  [w] pthis [AjPContact*] Pointer to the to be deleted.
**         The pointer is always deleted.
** @return [void]
** @category delete [AjPGotohCell] default destructor
** @@
******************************************************************************/

void embContactDel(AjPContact* pthis)
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


/* @section Cmap header Destructors ******************************************
**
** Destruction is achieved by deleting the pointer to the contact map header
**  object and freeing the associated memory
**
******************************************************************************/
/* @func ajCmapHeaderDel ************************************************
**
** Default destructor for a contact map header object
**
** @param [r] pthis [AjPCmapHeader*] header to be deleted
** @return [void]
** @@
******************************************************************************/

void ajCmapHeaderDel (AjPCmapHeader* pthis)
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






/* @func embContactWriteScoringMatrix ****************************************
**
** writes normalized substitution scores to a scoring matrix file format
**
** @param [r] ajpFileScoringMatrix [AjPFile] file to write scoring table to
** @param [r] ajpInt2dScoringMatrix [AjPInt2d] 2-D matrix of signed integer scores
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

AjBool embContactWriteScoringMatrix(AjPInt2d ajpInt2dCounts,
				    AjPFile ajpFileScoringMatrix)
{
    ajint ajIntRow    = 0;
    ajint ajIntColumn = 0;
    ajint ajIntCount  = 0;
    char cRowResType    = '\0';
    char cColumnResType = '\0';
    AjBool ajBoolSuccess = AJFALSE;
    AjPStr ajpStrResTypeAlphabet     = NULL; /* alphabet strings */
    AjPStr ajpStrResTypeAlphabetCopy = NULL;
    AjIStr ajpStrIterRowResType      = NULL;
    AjIStr ajpStrIterColumnResType   = NULL;

    ajIntRow = 0;
    ajIntColumn =0;

    /* write header */
    
    /*
     * write scores to data file
     * iterating over string of
     * scoring_matrix residue types
     */

    /* start by writing top line */
    ajpStrResTypeAlphabet = ajStrNewC("ARNDCQEGHILKMFPSTWYVBZX*");
    ajpStrResTypeAlphabetCopy = ajStrDup(ajpStrResTypeAlphabet);

    ajFmtPrintF(ajpFileScoringMatrix, " ");

    ajpStrIterColumnResType = ajStrIter(ajpStrResTypeAlphabetCopy);
    do
    {
	cColumnResType = ajStrIterGetK(ajpStrIterColumnResType);
	ajFmtPrintF(ajpFileScoringMatrix, "    %c", cColumnResType);
    }    
    while(ajStrIterNext(ajpStrIterColumnResType));

    /* rewind string iterator */
    ajStrIterBegin(ajpStrIterColumnResType);

    ajFmtPrintF(ajpFileScoringMatrix, "\n");

    /* write rest of score block */
    ajpStrIterRowResType = ajStrIter(ajpStrResTypeAlphabet);
   do
    {
	cRowResType = ajStrIterGetK(ajpStrIterRowResType);
	ajIntRow = embCharToScoringMatrixIndex(cRowResType);

	/* iterate over string of scoring_matrix residue types */
	ajFmtPrintF(ajpFileScoringMatrix, "%c ", cRowResType);
	do
	{
	    cColumnResType = ajStrIterGetK(ajpStrIterColumnResType);
	    ajIntColumn = embCharToScoringMatrixIndex(cColumnResType);
	    ajIntCount = ajInt2dGet(ajpInt2dCounts, ajIntRow, ajIntColumn);
	    /* DDDDEBUG */
	    ajFmtPrint("(%4c, %4c) ", cRowResType, cColumnResType);
	    ajFmtPrint("(%4d, %4d) :\t", ajIntRow, ajIntColumn);
	    ajFmtPrint("%4d", ajIntCount);
	    ajFmtPrintF(ajpFileScoringMatrix, "%4d ", ajIntCount);
	}
	while(ajStrIterNext(ajpStrIterColumnResType));
	ajFmtPrintF(ajpFileScoringMatrix, "\n");
	ajFmtPrint("\n");
	/* rewind string iterator */
	ajStrIterBegin(ajpStrIterColumnResType);
    }    
    while(ajStrIterNext(ajpStrIterRowResType));

    ajStrIterFree(&ajpStrIterRowResType);
    ajStrIterFree(&ajpStrIterColumnResType);
    ajStrDel(&ajpStrResTypeAlphabet);
  
    return ajBoolSuccess;
}



/* @func embCharToScoringMatrixIndex *****************************************
**
** converts residue type character to scoring_matrix index
**
** @param [r] cResType [char] original single-letter residue type
** @return [ajint] index to corresponding character in data dir scoring matrix
** @@
******************************************************************************/

ajint embCharToScoringMatrixIndex(char cResType)
{
    ajint ajIntScoringMatrixIndex;

    /* look up array for residue type order
     *  found in scoring matrix data files:
     *   ARNDCQEGHILKMFPSTWYVBZX*
     */
    const ajint ajIntArrayLookUp[enumTotalResTypes] =
	{0,20,4,3,6,13,7,8,9,24,11,10,12,2,25,14,5,1,15,16,26,19,17,22,18,21,23,27};
    if (cResType == '*')
	cResType = enumAsciiAsterisk;
    if (cResType == '?')
	cResType = enumAsciiQuestionMark;
  
    cResType = cResType - enumAsciiOffset;
    
    /* default to out-of-range value */
    if ((cResType < 0) || (cResType > 27))
	ajIntScoringMatrixIndex = 28;
    else
	ajIntScoringMatrixIndex = ajIntArrayLookUp[(ajint) cResType];

    return ajIntScoringMatrixIndex;
}



/* @func embScoringMatrixIndexToChar *****************************************
**
** converts scoring_matrix index number to char
**
** @param [r] ajIntScoringMatrixIndex [ajint] BLOSUM index number
** @return [ajint]  original single-letter residue type
** @@
******************************************************************************/

ajint embScoringMatrixIndexToChar(ajint ajIntScoringMatrixIndex)
{
    char cResType;

    /* look up array for residue type order
     *  found in scoring matrix data files:
     *   ARNDCQEGHILKMFPSTWYVBZX*
     */
    const char cArrayLookUp[enumTotalResTypes] =
	{'A','R','N','D','C','Q','E','G','H','I','L','K','M','F','P','S','T','W','Y','V','B','Z','X','*','J','O','U','?'};
  
    /* XXXX SHOULD THIS BE AN OFFICIAL AMBIGUITY CODE INSTEAD? */
    /* default to out-of-range value */
    if ((cResType < 0) || (cResType > 27))
	cResType = '?';
    else
	cResType = cArrayLookUp[ajIntScoringMatrixIndex];

    return cResType;
}






/* @func embAjint1ToString3 **************************************************
**
** gives three-letter amino acid residue code for one-letter ASCII number
**
** @param [r] ajIntCode [ajint] one digit ASCII string A=0 29=NO_MATCH
** @return [AjPStr] code for three-letter equivalent amino acid residue
** @@
******************************************************************************/

AjPStr embAjint1ToString3 (ajint ajIntCode)
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



/* @func embReadCmapFile ******************************************************
**
** reads ajpdb contact map file into three arrays of ints  
**
** @param [r] ajpFileCmap [AjPFile] input file stream of current cmap
** @param [r] ajIntSeqLen [ajint] Sequence length
** @param [r] pAjpCmapHeader [AjPCmapHeader*] Contact map header
** @param [r] pAjpInt2dCmapSummary [AjPInt2d*] contacts summary
** @param [r] pAjpInt2dCmapResTypes [AjPInt2d*] contacts as residue types
** @param [r] pAjpInt2dCmapPositions [AjPInt2d*] contacts as positions
**                                               in chain
** @return [AjBool] ajTrue if file successfully read
** @@
******************************************************************************/

AjBool embReadCmapFile (AjPFile ajpFileCmap,
			ajint ajIntSeqLen,
			AjPInt2d *pAjpInt2dCmapSummary,
			AjPCmapHeader *pAjpCmapHeader,
			AjPInt2d *pAjpInt2dCmapResTypes,
			AjPInt2d *pAjpInt2dCmapPositions)
{
    AjBool ajBoolCmapFileRead; /* has the file been read? */
    /* DDDD DEBUG: ARE NULL HEADERS BEING RETURNED? */
    AjBool ajBoolCmapHeaderLoaded; /* has the Cmap header been read? */

    ajint ajIntColumnCount;

    /* what kind of contact map line has been read in? */
    ajint ajIntCmapLineType;

    /* to store contact attributes from contact map */
    ajint ajIntFirstResType;
    ajint ajIntSecondResType;
    ajint ajIntFirstPosition;
    ajint ajIntSecondPosition;
    ajint ajIntLastContact;

     /* object to hold single contact */
    AjPContact ajpContactTemp = NULL;

    /* structure to hold header text */
    AjPCmapHeader ajpCmapHeader = NULL;

    /* arrays to hold contacts */
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;

    /* string to store that contact map line */
    AjPStr ajpStrCmapLine = NULL;

    /* default to no line */
    ajIntCmapLineType = enumNoCmapLine;

    ajBoolCmapFileRead = ajFalse;
    /* DDDD DEBUG: ARE NULL HEADERS BEING RETURNED? */
    ajBoolCmapHeaderLoaded = ajFalse;

    /* check file passed to function is usable */	
    if(!ajpFileCmap)
    {	
	ajWarn("function embReadCmapFile cannot open passed filestream");	
	return ajFalse;
    }

    /* there are no contacts in any of the columns of the contact arrays yet */
    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
	ajInt2dPut(pAjpInt2dCmapSummary,
		   enumLastContactIndex,
		   ajIntColumnCount,
		   enumZeroContacts);

    /* reserve memory for current contact string */
    ajpStrCmapLine = ajStrNew();
    
    /* reserve memory for current contact object */
    ajpContactTemp = ajContactNew();

    /* dereference pointers to passed object pointers */
    ajpCmapHeader = *pAjpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* read through contact map file until there are no more contacts */
    while( ( ajpStrCmapLine = embReadCmapLine(ajpFileCmap) ) )
    {
	ajIntCmapLineType = embTypeCmapLine(ajpStrCmapLine);

	ajIntLastContact = enumZeroContacts;
	
	if(ajIntCmapLineType == enumContactCmapLine)
	{
	    embLoadContactLine(&ajpStrCmapLine, &ajpContactTemp);

	    /* load up temporary contact object with values read from line in Cmap file */ 
	    ajIntFirstResType = 
		embString3ToAjint1(ajpContactTemp->ajpStrFirstResType);
	    ajIntSecondResType = 
		embString3ToAjint1(ajpContactTemp->ajpStrSecondResType);
	    ajIntFirstPosition = 
		ajpContactTemp->ajIntFirstPosition;
	    ajIntSecondPosition = 
		ajpContactTemp->ajIntSecondPosition;
	    
	    /* get position of last contact in column */
	    ajIntLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntFirstPosition);
	    /* XXXX DEBUG */
	    if( enumDebugLevel > 2 )
	    {
		ajFmtPrint("ajIntFirstPosition (col.): %d\t",
			   ajIntFirstPosition);
		ajFmtPrint("ajIntSecondPosition: %d\t",
			   ajIntSecondPosition);
		ajFmtPrint("ajIntLastContact: %d\n",
			   ajIntLastContact);
		ajFmtPrint("# ajpStrFirstResType: %S\tajIntFirstResType: %d\tand back to type: %S #\n",
			   ajpContactTemp->ajpStrFirstResType, ajIntFirstResType, embAjint1ToString3(ajIntFirstResType));
		ajFmtPrint("# ajIntSecondResType: %S\tajIntSecondResType: %d\tand back to type: %S #\n",
			   ajpContactTemp->ajpStrSecondResType, ajIntSecondResType, embAjint1ToString3(ajIntSecondResType));
	    }
	    
	    /*
	     * insert type of first residue in contact into
	     * column of ints in summary array
	     */
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumFirstResType,
		       ajIntFirstPosition,
		       ajIntFirstResType);

	    /*
	     * insert position of second residue in contact into
	     * column of ints in array of second residue positions
	     */
	    ajInt2dPut(pAjpInt2dCmapPositions,
		       ajIntLastContact,
		       ajIntFirstPosition,
		       ajIntSecondPosition);
	    
	    /*
	     * insert type of second residue in contact into
	     * column of ints in array of second residues
	     */
	    ajInt2dPut(pAjpInt2dCmapResTypes,
		       ajIntLastContact,
		       ajIntFirstPosition,
		       ajIntSecondResType);
	    
	    /* increment contact counter at zeroth position in column */
	    ajIntLastContact++;
	    
	    /*
	     * insert incremented contact counter
	     * into summary array
	     */
	    ajInt2dPut(pAjpInt2dCmapSummary,
		       enumLastContactIndex,
		       ajIntFirstPosition,
		       ajIntLastContact);	    

	}
	/*
	 * XXXX IDEALLY THERE SHOULD BE A LOOP HERE SO THAT, IF THE
	 *  LINE IS THE FIRST SEQUENCE ("SQ") LINE, THEN ALL THE
	 *  ALL THE LINES OF SEQUENCE BEFORE THE NEXT TRUE BLANK ("XX")
	 *  LINE ARE READ IN AS A BLOCK
	 */

	else if(ajIntCmapLineType == enumHeaderCmapLine ||
		ajIntCmapLineType == enumSeqCmapLine)
	{
	    /* DDDDEBUG: DEBUGGGING VERSION OF THIS CLAUSE USED HERE */
	    ajBoolCmapHeaderLoaded = embLoadHeaderLine(&ajpStrCmapLine,
						       pAjpCmapHeader);
	    if(!ajBoolCmapHeaderLoaded)
		    {	
			ajWarn("HEADER NOT READ!");	
			return ajFalse;
		    }
	}
    }

    /* free contact */
    embContactDel(&ajpContactTemp);
    
    return ajBoolCmapFileRead;
}



/* @func embReadCmapLine *****************************************************
**
** reads single line from ajpdb contact map file and returns it as a string
**
** @param [r] ajpFileCmap [AjPFile]  input file stream of current cmap
** @return [AjPStr] contents of line read
** @@
******************************************************************************/

AjPStr embReadCmapLine (AjPFile ajpFileCmap)
{
    AjPStr ajpStrCmapLine = NULL;

    ajFileReadLine(ajpFileCmap, &ajpStrCmapLine);

    return ajpStrCmapLine;
}



/* @func embTypeCmapLine ******************************************************
**
** based on first two chars, returns type of line from contact map file:
**
** @param [r] ajpStrCmapLine [AjPStr] Contact map file line  
** @return [ajint] Enumerated line code
** @@
******************************************************************************/

ajint embTypeCmapLine (AjPStr ajpStrCmapLine)
{
    ajint ajIntLineCode = enumNoCmapLine;

    if(!ajpStrCmapLine)
	ajWarn("NULL string passed to embTypeCmapLine");

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
	     ajStrPrefixC(ajpStrCmapLine, "IN") )
    {
	ajIntLineCode = enumHeaderCmapLine;	
    }     
    else if(ajStrPrefixC(ajpStrCmapLine, "SQ"))
    {
	ajIntLineCode = enumSeqCmapLine;
    }
    else 
    {
	ajIntLineCode = enumNoCmapLine;
    }

    return ajIntLineCode;
}



/* @func embLoadContactLine ***************************************************
**
** loads single contact from single line of ajpdb contact map file
**
** @param [r] pAjpStrCmapLine [AjPStr*] to contact line from Cmap file
** @param [r] pAjpContactLoaded [AjPContact*] to contact object from line
** @return [AjBool] ajTrue if contact successfully parsed
** @@
******************************************************************************/

AjBool embLoadContactLine (AjPStr *pAjpStrCmapLine,
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



/* @func embLoadHeaderLine ****************************************************
**
** loads single line from ajpdb contact map file into header object
**
** @param [r] pAjpStrCmapLine [AjPStr*] to header line from Cmap file
** @param [r] pAjpCmapHeaderToLoad [AjPCmapHeader*] to header object from line
** @return [AjBool] ajTrue if line successfully loaded
** @@
******************************************************************************/

AjBool embLoadHeaderLine (AjPStr *pAjpStrCmapLine,
			  AjPCmapHeader *pAjpCmapHeaderToLoad)
{
    AjPStr ajpStrCmapLine = NULL;
    AjPCmapHeader ajpCmapHeaderToLoad = NULL;
    AjBool ajBoolCmapHeaderToLoad;

    ajBoolCmapHeaderToLoad = ajFalse;

    ajpCmapHeaderToLoad = *pAjpCmapHeaderToLoad;
    ajpStrCmapLine = *pAjpStrCmapLine;

    if(!ajpStrCmapLine)
    {	
	ajWarn("NULL string passed to embReadCmapLine()");	
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



/* @func embWriteCmapFile ****************************************************
**
** writes ajpdb contact map file from header object and two arrays of ints  
**
** @param [r] ajpFileUpdatedCmap [AjPFile] output file stream of current cmap
** @param [r] ajIntSeqLen [ajint] number of residues in chain sequence
** @param [r] pAjpInt2dCmapSummary [AjPInt2d*] summary of contact map
** @param [r] pAjpCmapHeader [AjPCmapHeader*] Contact map header
** @param [r] pAjpInt2dCmapResTypes [AjPInt2d*] contacts as residue types
** @param [r] pAjpInt2dCmapPositions [AjPInt2d*] contacts as positions in
**                                               chain
** @return [AjBool] ajTrue if file successfully written
** @@
******************************************************************************/

AjBool embWriteCmapFile (AjPFile ajpFileUpdatedCmap,
			ajint ajIntSeqLen,
			AjPInt2d *pAjpInt2dCmapSummary,
			AjPCmapHeader *pAjpCmapHeader,
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
    AjPStr ajpStrTempFirstResType;
    AjPStr ajpStrTempSecondResType;
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

    /* dereference pointers to original pointers */
    ajpCmapHeader = *pAjpCmapHeader;
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;    

    /* check file passed to function is usable */	
    if(!ajpFileUpdatedCmap)
    {	
	ajWarn("function embWriteCmapFile cannot open passed filestream");	
	return ajFalse;
    }

    /* write header to file */
    ajBoolUpdatedCmapFileWritten = embWriteCmapHeader(ajpFileUpdatedCmap,
						     ajpCmapHeader);

    /* reserve memory for current contact object */
    ajpContactTemp = ajContactNew();

    /* XXXX OUTER LOOP OVER FIRST RESIDUES IN CONTACTS */
    for(ajIntColumn = enumArrayOffset;ajIntColumn < ajIntSeqLen;ajIntColumn++)
    {
	/* find end of each column */
	ajIntColumnLen = ajInt2dGet(ajpInt2dCmapSummary,
				    enumLastContactIndex,
				    ajIntColumn);

/* 	ajFmtPrint("££££££££££££££££££££££££££££££££\n"); */
/* 	ajFmtPrint("££££££ COLUMN LENGTH: %d ££££££\n", ajIntColumnLen); */
/* 	ajFmtPrint("££££££££££££££££££££££££££££££££\n\n"); */

	ajIntTempFirstResType = ajInt2dGet(ajpInt2dCmapSummary,
					   enumFirstResTypeIndex,
					   ajIntColumn);
	ajpStrTempFirstResType = embAjint1ToString3(ajIntTempFirstResType);

	if (ajIntColumnLen > enumZeroContacts)
	{

	    /* XXXX INNER LOOP OVER CONTACTS IN COLUMN */
	    for(ajIntRow = 0;ajIntRow < ajIntColumnLen;ajIntRow++)
	    {
		ajIntTempSecondResType = ajInt2dGet(ajpInt2dCmapResTypes,
						    ajIntRow,
						    ajIntColumn);	    

		/* load up temporary contact object with values from residue type array */ 
		ajpStrTempSecondResType = embAjint1ToString3(ajIntTempSecondResType);

		if( enumDebugLevel > 1 )
		{
		    
		    ajFmtPrint("££££££££££££££££££££££££££££££££\n");
		    ajFmtPrint("££££ first residue type: %S ££££\n",
			       ajpStrTempFirstResType);
		    ajFmtPrint("£££ second residue type: %S ££££\n",
			       ajpStrTempSecondResType);
		    ajFmtPrint("££££££££££££££££££££££££££££££££\n\n");
		}
		
		/* read temporary contact object with values from position array */
		ajIntTempFirstPosition = ajIntColumn;
		ajIntTempSecondPosition = ajInt2dGet(ajpInt2dCmapPositions,
						     ajIntRow,
						     ajIntColumn);

		/* load up temporary contact object with values from residue position array */
		ajpContactTemp->ajIntFirstPosition = ajIntTempFirstPosition;
		ajpContactTemp->ajIntSecondPosition = ajIntTempSecondPosition;
		ajpContactTemp->ajpStrFirstResType = ajpStrTempFirstResType;
		ajpContactTemp->ajpStrSecondResType = ajpStrTempSecondResType;
		
		embWriteContact(ajpFileUpdatedCmap,
			      ajpContactTemp);
	    }
	}
    }

    /* free contact */
    embContactDel(&ajpContactTemp);
    
    return ajBoolUpdatedCmapFileWritten;
}


/* @func embString3ToAjint1 ***************************************************
**
** gives one-letter ASCII number for three-letter amino acid residue code
**
** @param [r] ajpStrThreeLetterCode [AjPStr] three-letter code string
** @return [ajint] code for one-letter equivalent A=0 29=NO MATCH
** @@
******************************************************************************/

ajint embString3ToAjint1 (AjPStr ajpStrThreeLetterCode)
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





/* @func embGetCmapSummary ***************************************************
**
** reserves memory for a 2-D array of ints to summarise a contact map 
**
** @param [r] pcSeq [char*] Contact map
** @return [AjPInt2d] array of ajints containing number of contacts and
** types of first residue in each contact
** @@
******************************************************************************/

AjPInt2d embGetCmapSummary (char *pcSeq)
{
    ajint ajIntAcrossSeqLen;
    ajint ajIntColumnCount;
    ajint ajIntResType;
    
    AjPInt2d ajpInt2dSummary = NULL;

    ajIntAcrossSeqLen = strlen(pcSeq);

    ajpInt2dSummary = ajInt2dNewL(ajIntAcrossSeqLen);

    for(ajIntColumnCount = 0;ajIntColumnCount < ajIntAcrossSeqLen;ajIntColumnCount++)
    {
	ajIntResType = enumNoResTypeMatch;

	ajInt2dPut(&ajpInt2dSummary,
		   enumFirstResTypeIndex,
		   ajIntColumnCount,
		   ajIntResType);

	ajInt2dPut(&ajpInt2dSummary,
		   enumLastContactIndex,
		   ajIntColumnCount,
		   enumZeroContacts);
    }
	
    return ajpInt2dSummary;
}




/* @func embGetIntMap ********************************************************
**
** reserves memory for a 2-D array of ints to store a contact map 
**
** @param [r] ajIntAcrossSeqLen [ajint] number of residues in template
** @return [AjPInt2d] array of ajints containing position IDs for each contact
** @@
******************************************************************************/

AjPInt2d embGetIntMap (ajint ajIntAcrossSeqLen)
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


/* @func embWriteContact *******************************************************
**
** writes contact object to open contact map file
**
** @param [r] ajpFileUpdatedCmap [AjPFile] file to write contact map to
** @param [r] ajpContactToWrite [AjPContact] contact to be written
** @return [void]
** @@
******************************************************************************/

void embWriteContact (AjPFile ajpFileUpdatedCmap,
		    AjPContact ajpContactToWrite)
{
    /* write contact object attributes in EMBL-like format */
    ajFmtPrintF(ajpFileUpdatedCmap,
		"SM   %S %d ; %S %d\n",
		ajpContactToWrite->ajpStrFirstResType,
		ajpContactToWrite->ajIntFirstPosition,
		ajpContactToWrite->ajpStrSecondResType,
		ajpContactToWrite->ajIntSecondPosition);
}


/* @func embWriteCmapHeader *************************************************
**
** writes header object to open contact map file
**
** @param [r] ajpFileUpdatedCmap [AjPFile] file to write contact map to
** @param [r] ajpHeaderToWrite [AjPCmapHeader] contact to be written
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

AjBool embWriteCmapHeader (AjPFile ajpFileUpdatedCmap,
				 AjPCmapHeader ajpHeaderToWrite)
{
    AjBool ajBoolSuccess;
    AjPStr ajpStrBlankLine = NULL;
    AjPStr *pAjpStrBlankLine = NULL;
    AjPStr ajpStrTemp = NULL;

    ajBoolSuccess = ajFalse;

    pAjpStrBlankLine = &ajpStrBlankLine;
    ajpStrBlankLine = ajStrNewC("XX");
    
    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    &(ajpHeaderToWrite->ajpStrCmapId));

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    pAjpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    &(ajpHeaderToWrite->ajpStrCmapDe));

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    pAjpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    &(ajpHeaderToWrite->ajpStrCmapEx));

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    pAjpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    &(ajpHeaderToWrite->ajpStrCmapMo));

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    pAjpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    &(ajpHeaderToWrite->ajpStrCmapIn));

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    pAjpStrBlankLine);

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    &(ajpHeaderToWrite->ajpStrCmapSq));

    ajBoolSuccess = embWriteCmapLine(ajpFileUpdatedCmap,
				    pAjpStrBlankLine);

    ajStrDel(&ajpStrTemp);
    ajStrDel(&ajpStrBlankLine);
    

    return ajBoolSuccess;

}



/* @func embWriteCmapLine ****************************************
**
** writes single AjPStr to open contact map file
**
** @param [r] ajpFileUpdatedCmap [AjPFile] file to write AjPStr to
** @param [r] ajpStrLineToWrite [AjPStr *] pointer to line to be written
** @return [AjBool] did it work? ajTrue for success
** @@
******************************************************************************/

AjBool embWriteCmapLine (AjPFile ajpFileUpdatedCmap,
			AjPStr *pAjpStrLineToWrite)
{
    AjBool ajBoolSuccess;
    AjPStr ajpStrTemp = NULL;
    ajint ajIntStrTempLen;
    AjPStr ajpStrLineToWrite;

    ajBoolSuccess = ajFalse;
    ajpStrLineToWrite = *pAjpStrLineToWrite;

    ajStrCopy(&ajpStrTemp, ajpStrLineToWrite);
    ajStrAppK(&ajpStrTemp, '\n');
    ajStrAppK(&ajpStrTemp, '\0');

    ajIntStrTempLen = ajStrLen(ajpStrTemp);

    ajBoolSuccess = ajFileWriteStr(ajpFileUpdatedCmap,
				   ajpStrTemp,
				   ajIntStrTempLen-1);
    ajStrDel(&ajpStrTemp);

    return ajBoolSuccess;

}








/* DEBUGGING FUNCTIONS ONLY FROM HERE DOWNWARDS*/


/* @func debug_cmap_header ***************************************************
**
** prints out a contact map header
**
** @param [r] pAjpCmapHeaderToPrint [AjPCmapHeader*] to header to be printed
** @return [AjBool] did it work?
** @@
******************************************************************************/

AjBool debug_cmap_header (AjPCmapHeader *pAjpCmapHeaderToPrint)
{

    AjPCmapHeader ajpCmapHeaderToPrint = NULL;

    AjBool ajBoolSuccess; /* did we print it? */

    ajpCmapHeaderToPrint = *pAjpCmapHeaderToPrint;

    ajBoolSuccess = ajFalse;

    /* print out each line in contact map header in order */
    ajFmtPrint("ID   %S\n", ajpCmapHeaderToPrint->ajpStrCmapId);
    ajFmtPrint("XX\n");
    ajFmtPrint("DE   %S\n", ajpCmapHeaderToPrint->ajpStrCmapDe);
    ajFmtPrint("XX\n");
    ajFmtPrint("MO   %S\n", ajpCmapHeaderToPrint->ajpStrCmapMo);
    ajFmtPrint("XX\n");
    ajFmtPrint("CN   %S\n", ajpCmapHeaderToPrint->ajpStrCmapCn);
    ajFmtPrint("XX\n");
    ajFmtPrint("IN   %S\n", ajpCmapHeaderToPrint->ajpStrCmapIn);
    ajFmtPrint("XX\n");
    ajFmtPrint("SQ   %S\n", ajpCmapHeaderToPrint->ajpStrCmapSq);
    ajFmtPrint("XX\n");
	
    ajBoolSuccess = ajTrue;
    
    return ajBoolSuccess; 
}




/* @func embPrintContact ******************************************************
**
** prints out contact object 
**
** @param [r] ajpContactToPrint [AjPContact] contact to be printed
** @return [AjBool] did it work?
** @@
******************************************************************************/

AjBool embPrintContact (AjPContact ajpContactToPrint)
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



/* 17Mar04              debug_int_map()                  Damian Counsell  */
/*                                                                              */
/* checks that array of strings of one-letter contacts is properly built        */

void debug_int_map(AjPInt2d *pAjpInt2dCmapSummary,
			  AjPInt2d *pAjpInt2dCmapResTypes,
			  AjPInt2d *pAjpInt2dCmapPositions,
			  char *pcSeq,
			  ajint ajIntSeqLen)
{
    AjPStr ajpStrResTypeTemp;
    
    ajint ajIntRowCount;
    ajint ajIntColumnCount;
    ajint ajIntTempLastContact;
    ajint ajIntTempFirstPosition;
    ajint ajIntTempSecondPosition;
    
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    
    /* dereference pointers */
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;
    ajpInt2dCmapPositions = *pAjpInt2dCmapPositions;
    ajpInt2dCmapResTypes = *pAjpInt2dCmapResTypes;

    ajFmtPrint("====================================================================================================\n");
    ajFmtPrint("====================================================================================================\n");
    for(ajIntColumnCount = 0; ajIntColumnCount < ajIntSeqLen; ajIntColumnCount++)
    {
	ajIntTempLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntColumnCount);
	ajFmtPrint("current position:\t%4d current residue type: %c\tnumber of contacts: %4d",
		   ajIntColumnCount,
		   pcSeq[ajIntColumnCount],
		   ajIntTempLastContact);
	
	ajpStrResTypeTemp = embAjint1ToString3( ajInt2dGet(ajpInt2dCmapSummary,
							  enumFirstResTypeIndex,
							  ajIntColumnCount));

	ajFmtPrint("\tcurrent int residue type: %S:  ", ajpStrResTypeTemp);
	
	ajIntTempFirstPosition = ajInt2dGet(ajpInt2dCmapSummary,
					    enumFirstResTypeIndex,
					    ajIntColumnCount);
	
	if(ajIntTempLastContact)
	    for (ajIntRowCount = 0;ajIntRowCount < ajIntTempLastContact;ajIntRowCount++)
	    {
		ajIntTempSecondPosition = ajInt2dGet(ajpInt2dCmapPositions,
						     ajIntRowCount,
						     ajIntColumnCount);
		ajpStrResTypeTemp = embAjint1ToString3( ajInt2dGet(ajpInt2dCmapResTypes,
								  ajIntRowCount,
								  ajIntColumnCount));
		ajFmtPrint("%S", ajpStrResTypeTemp);
		ajFmtPrint("%3d  ", ajIntTempSecondPosition);
	    }
	ajFmtPrint("\n");
	
    }

  ajFmtPrint("====================================================================================================\n");
  ajFmtPrint("====================================================================================================\n");

  return;
}

/* 24Mar04              debug_cmap_summary()                  Damian Counsell  */
/*                                                                            */
/* checks that contact summary array is properly built                        */

void debug_cmap_summary(AjPInt2d *pAjpInt2dCmapSummary,
			       ajint ajIntSeqLen)
{
    ajint ajIntColumn;
    ajint ajIntFirstResType;
    ajint ajIntLastContact;
    
    AjPStr ajpStrFirstResType;

    AjPInt2d ajpInt2dCmapSummary = NULL;
    
    /* dereference pointer to summary array */
    ajpInt2dCmapSummary = *pAjpInt2dCmapSummary;

    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");
    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");

    for(ajIntColumn = 0; ajIntColumn < ajIntSeqLen; ajIntColumn++)
    {
	    ajIntFirstResType = ajInt2dGet(ajpInt2dCmapSummary,
					   enumFirstResTypeIndex,
					   ajIntColumn);
	    
	    ajIntLastContact = ajInt2dGet(ajpInt2dCmapSummary,
					  enumLastContactIndex,
					  ajIntColumn);

	    ajpStrFirstResType = embAjint1ToString3(ajIntFirstResType);

	    ajFmtPrint("COLUMN %d TYPE OF FIRST RESIDUE IN CONTACT: %S\t LAST CONTACT NUMBER: %d\n",
		       ajIntColumn,
		       ajpStrFirstResType,
		       ajIntLastContact);
    }

    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");
    ajFmtPrint("[][][][][][][][][][][][][][]][][][][][][][][][][][][][][][][][][][][][][][][]][][][][][][]\n");

    return;
}
