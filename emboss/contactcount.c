/* @source contactcount application
**
** Calculates the relative frequencies (the meaning of the term "relative
**  frequencies" is defined in various ways below in more detail) of
**  residue-residue contacts in domains from domainatrix contact map files 
**
** Outputs plain text files containing EMBOSS format scoring matrices
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

#include "emboss.h"
#include <math.h>

enum constant
    {
	enumDebugLevel        =  0,
	enumTotalResTypes     = 28,
	enumZeroCounts        =  0,
    };

static AjPFeature write_count(AjPFeattable ajpFeattableCounts,
			      ajint ajIntFeatureResType,
			      ajint ajIntSpecificPairs,
			      ajint ajIntTotalPairs);


/* @prog contactcount ********************************************************
** 
** aggregate residue-specific contacts from contact files in local dir
**
******************************************************************************/

int main( int argc , char **argv )
{
    /* position counters and limits */
    ajint ajIntResTypeCurrent = 0;
    ajint ajIntContactNumber  = 0;
    ajint ajIntNumberOfContactFiles = 0;
    ajint ajIntRow    = 0;
    ajint ajIntColumn = 0;
    ajint ajIntRowMax    = 0;
    ajint ajIntColumnMax = 0;

    /* contact maps */
    AjPStr ajpStrCmapFileName   = NULL; /* name                      */
    AjPFile ajpFileCmapCurrent  = NULL; /* file                      */
    AjPList ajpListCmapFiles    = NULL; /* list of contact map files */
    AjPCmap ajpCmapCurrent      = NULL; /* object                    */
    AjPInt2d ajpInt2dContactMap = NULL; /* Cmap matrix               */
    AjPStr ajpStrChainId        = NULL; /* chain id attribute        */
    AjPStr ajpStrChainSeq       = NULL; /* seq string attribute      */
    AjPSeq ajpSeqChain          = NULL; /* seq AjPStr as seq object  */

    char cFirstResType       = '\0'; /* first residue letter in contact  */
    char cSecondResType      = '\0'; /* second residue letter in contact */
    ajint ajIntFirstResType  = 0;    /* first residue number in contact  */
    ajint ajIntSecondResType = 0;    /* second residue number in contact */

    /* count matrix */
    AjPStr ajpStrResTypeAlphabet   = NULL; /* ordered letters of residues  */
    AjPStr ajpStrFirstResType      = NULL;
    AjPStr ajpStrSecondResType     = NULL;
    AjIStr ajpStrIterFirstResType  = NULL;
    AjIStr ajpStrIterSecondResType = NULL;

    /* scoring matrix */
    AjPInt2d ajpInt2dCounts = NULL;
    ajint ajIntCount = 0;
    AjPFile ajpFile2dScoringMatrix = NULL;

    /* number of contacts between first and second residue types */
    ajint ajIntSpecificPairs = 0;
    /* total number of contacts of first residue type */
    ajint ajIntTotalPairs = 0;

    /* report objects */
    AjPReport ajpReportCounts       = NULL;
    AjPStr ajpStrReportHead         = NULL;
    AjPFeattable ajpFeattableCounts = NULL;
    AjPFeature ajpFeatCurrent       = NULL;
    AjPStr ajpStrReportTail         = NULL; /* DDDDEBUGGING ONLY */

    embInit( "contactcount", argc, argv );

    /* get contact file directory from ACD */
    ajpListCmapFiles = ajAcdGetDirlist("cmapdir");

    /* get test output file from ACD */
    ajpFile2dScoringMatrix = ajAcdGetOutfile("dummyfile");

    /* get contact count output file from ACD */
    ajpReportCounts = ajAcdGetReport("outfile");

    ajIntNumberOfContactFiles = ajListLength(ajpListCmapFiles);

    /* create a 2-D array (count array) to store the scores */
    ajpStrResTypeAlphabet = ajStrNewC("ARNDCQEGHILKMFPSTWYVBZX*");

    ajpInt2dCounts = ajInt2dNewL(enumTotalResTypes);

    /* empty count array */
    for(ajIntRow = 0;ajIntRow < enumTotalResTypes;ajIntRow++)
    {
	for(ajIntColumn = 0;ajIntColumn < enumTotalResTypes;ajIntColumn++)
	{
	    ajInt2dPut(&ajpInt2dCounts, ajIntRow, ajIntColumn, enumZeroCounts);
	}
    }

    /* reserve memory for objects in loop */
    ajpSeqChain = ajSeqNewStr(ajpStrChainSeq);
    ajpStrReportHead = ajStrNew();
    ajpFeattableCounts = ajFeattableNewSeq(ajpSeqChain);

    /* loop over contact map files in current directory */
    while(ajListPop(ajpListCmapFiles, (void **)&ajpStrCmapFileName))
    {
	ajpFileCmapCurrent = ajFileNewIn(ajpStrCmapFileName);

	if(!ajpFileCmapCurrent)
	    ajDie("cannot open file %S",ajpStrCmapFileName);

	/* get Cmap object from current Cmap file */
	if(!(ajpCmapCurrent = ajCmapReadINew(ajpFileCmapCurrent, 1, 1 )))
	{
	    ajWarn("no valid contact map in %S",ajpStrCmapFileName);
	    break;   
	}

	/* obtain chain name from Cmap object */
	ajpStrChainId = ajpCmapCurrent->Id;

	ajFmtPrint("\n%S\n\n", ajpStrChainId);

	/* obtain chain sequence from Cmap object */
	ajpStrChainSeq = ajpCmapCurrent->Seq;

	ajFmtPrint("\n%S\n\n", ajpStrChainSeq);

	/* obtain sequence object from string */
	ajpStrChainSeq = ajpCmapCurrent->Seq;

	/* obtain contact matrix from Cmap object */
	ajpInt2dContactMap = ajpCmapCurrent->Mat;

	/* get dimensions of contact map */
	ajInt2dLen(ajpInt2dContactMap, &ajIntRowMax, &ajIntColumnMax);

	/* outer loop over sequence of chain */
	for(ajIntRow = 0;ajIntRow < ajIntRowMax;ajIntRow++)
	{
	    /* get type of residue at current position in chain */
	    cFirstResType = ajStrChar(ajpStrChainSeq, ajIntRow);

	    /* inner loop over sequence of chain (these are self-self contacts) */
	    for(ajIntColumn = 0;ajIntColumn < ajIntColumnMax;ajIntColumn++)
	    {
		/* get type of residue at current position in chain */
		cSecondResType = ajStrChar(ajpStrChainSeq, ajIntColumn);

		/* convert residue type to position on axis of scoring matrix */
		ajIntFirstResType = embCharToScoringMatrixIndex(cFirstResType);
		ajIntSecondResType = embCharToScoringMatrixIndex(cSecondResType);

		/* DDDDEBUGGING */
		if((ajIntFirstResType < 0) || (ajIntFirstResType > 27))
		{
		    ajFmtPrint("\n ======================\n");
		    ajFmtPrint("\n OUT OF RANGE. FIRST RESIDUE = %d", ajIntFirstResType);
		}
		else if((ajIntSecondResType < 0) || (ajIntSecondResType > 27))
		{
		    ajFmtPrint("\n ======================\n");
		    ajFmtPrint("\n OUT OF RANGE. SECOND RESIDUE = %d", ajIntSecondResType);
		}
		/* NOT DEBUGGING */
		/* increment count for that restype-restype pairing */
		else if(ajInt2dGet(ajpInt2dContactMap, ajIntRow, ajIntColumn))
		{
		    ajIntCount = ajInt2dGet(ajpInt2dCounts,
					    ajIntFirstResType,
					    ajIntSecondResType);
		    ajInt2dPut(&ajpInt2dCounts,
			       ajIntFirstResType,
			       ajIntSecondResType,
			       ajIntCount+1);
		}	    
	    }
	}

	/* DDDDEBUGGING */
	ajIntCount = ajInt2dGet(ajpInt2dCounts, 6, 11);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS FIRST:\t%d\n", ajIntCount);
	ajFmtPrint("=======================\n");
	ajIntCount = ajInt2dGet(ajpInt2dCounts, 11, 6);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS AGAIN:\t%d\n", ajIntCount);
	ajFmtPrint("=======================\n");

	/* get dimensions of count array */
	ajInt2dLen(ajpInt2dCounts, &ajIntRowMax, &ajIntColumnMax);

	/* info about chain for head of report */
	ajFmtPrintS(&ajpStrReportHead, "Chain: %S", (ajpStrChainId));
	ajReportSetHeader(ajpReportCounts, ajpStrReportHead);

	/* DDDDEBUG: DUMMY VALUE FOR CONTACT NUMBER */
	ajIntContactNumber= ajSeqLen(ajpSeqChain)-5;

	/* DDDDEBUGGING */
	ajIntSpecificPairs = 4;
	ajIntTotalPairs = 7;

	ajpFeatCurrent = write_count(ajpFeattableCounts,
				     ajIntContactNumber,
				     ajIntSpecificPairs,
				     ajIntTotalPairs);

	/* write the report to the output file */
	ajReportWrite(ajpReportCounts,
		      ajpFeattableCounts,
		      ajpSeqChain);

	/* close the input file */
	ajFileClose(&ajpFileCmapCurrent);
    }

    /* END LOOP OVER CONTACT MAP FILES IN CURRENT DIRECTORY */
    
    /* DDDDEBUGGING: read elements in count array */
    for(ajIntRow = 0;ajIntRow < enumTotalResTypes;ajIntRow++)
    {
	for(ajIntColumn = 0;ajIntColumn < enumTotalResTypes;ajIntColumn++)
	{
	    cFirstResType = embScoringMatrixIndexToChar(ajIntRow);
	    cSecondResType = embScoringMatrixIndexToChar(ajIntColumn);
	    ajIntCount = ajInt2dGet(ajpInt2dCounts, ajIntRow, ajIntColumn);
	    ajFmtPrint("%d\t", ajIntCount);
	}
	ajFmtPrint("\n");
    }
    
    /* write scores to scoring matrix data file */
    embContactWriteScoringMatrix(ajpInt2dCounts, ajpFile2dScoringMatrix);

    /* DDDDEBUG TEST INFO FOR TAIL OF REPORT */
    /*     ajFmtPrintS(&ajpStrReportTail, "This is some tail text"); */
    /*     ajReportSetTail(ajpReportCounts, ajpStrReportTail); */

    /* clear up report objects */
    ajStrDel(&ajpStrReportHead);
    ajFeatDel(&ajpFeatCurrent);
    ajFeattableDel(&ajpFeattableCounts);
    ajReportDel(&ajpReportCounts);

    /*
     * clear up the contact map objects,
     * starting with the file objects
     */
    while(ajListPop(ajpListCmapFiles, (void **)&ajpFileCmapCurrent))
	AJFREE(ajpFileCmapCurrent);
    ajListFree(&ajpListCmapFiles);
    ajSeqDel(&ajpSeqChain);
    ajInt2dDel(&ajpInt2dCounts);

    /* tidy up scoring output objects */
    ajStrIterFree(&ajpStrIterFirstResType);
    ajStrIterFree(&ajpStrIterSecondResType);
    ajStrDel(&ajpStrFirstResType);
    ajStrDel(&ajpStrSecondResType);
    ajStrDel(&ajpStrResTypeAlphabet);    
    
    ajFileClose(&ajpFile2dScoringMatrix);

    /*  tidy up everything else... */
    ajExit();

    return 0;
}




/* @funcstatic write_count *************************************************
**
** writes frequency features to a feature table and returns new feature  
**
** @param [u] ajpFeattableCounts [AjPFeattable] table to write frequency to
** @param [r] ajIntFeatureResType [ajint] residue type that a count
**                                            belongs to
** @param [r] ajIntSpecificPairs [ajint] contacts specific to both residues
** @param [r] ajIntTotalPairs [ajint] contacts specific to one residue
** @return [AjPFeature] New feature added to feature table
** @@
******************************************************************************/

static AjPFeature write_count(AjPFeattable ajpFeattableCounts,
			      ajint ajIntFeatureResType,
			      ajint ajIntSpecificPairs,
			      ajint ajIntTotalPairs)
{
    AjPFeature ajpFeatCounts = NULL;
    AjPStr ajpStrFeatTemp = NULL;

    ajpStrFeatTemp = ajStrNew();

    /* create feature for count and write per residue and per type frequency */
    ajpFeatCounts = ajFeatNewII(ajpFeattableCounts,
				ajIntFeatureResType,
				ajIntFeatureResType);
    ajFmtPrintS(&ajpStrFeatTemp, "*count1 %d", ajIntSpecificPairs);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);
    ajFmtPrintS(&ajpStrFeatTemp, "*count2 %d", ajIntTotalPairs);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);

    ajStrDel(&ajpStrFeatTemp);
    
    return ajpFeatCounts;
}








