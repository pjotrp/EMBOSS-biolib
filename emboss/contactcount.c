/* @source contactcount application
**
** Calculates the relative frequencies (the meaning of the term "relative
**  frequencies" is defined in various ways below in more detail) of
**  residue-residue contacts in domains from domainatrix contact map files 
**
** Outputs EMBOSS format matrix files
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

/* [XXXX ajIntCount1, ajIntCount2 SHOULD BE REPLACED WITH MORE MEANINGFUL NAMES] */
static AjPFeature write_count(AjPFeattable ajpFeattableCounts,
				ajint ajIntFeatureResidueType,
				ajint ajIntCount1,
				ajint ajIntCount2);




/* @prog contactcount ********************************************************
** 
** aggregate residue-specific contacts from contact files in local dir
**
******************************************************************************/

int main( int argc , char **argv )
{
    /* DDDDEBUG: string for test prints */
    AjPStr ajpStrDebug = NULL;

    AjPStr ajpStrCmapFileName   = NULL;
    AjPList ajpListCmapFiles    = NULL; /* list of contact map files */

    /* position counters and limits */
    ajint ajIntResidueTypeCurrent = 0;
    ajint ajIntContactNumber      = 0;
    ajint ajIntNumberOfContactFiles = 0;
    ajint ajIntRow = 0;
    ajint ajIntColumn = 0;
    ajint ajIntRowMax = 0;
    ajint ajIntColumnMax = 0;

    /* contact map */
    AjPStr ajpStrAlphabet        = ajStrNewC("ABCDEFGHIJKLMNOPQRSTUVWZYZ");
    AjPFile ajpFileCmapCurrent   = NULL; /* file                      */
    AjPCmap ajpCmapCurrent       = NULL; /* object                    */
    AjPInt2d ajpInt2dContactMap  = NULL; /* Cmap matrix               */
    AjPStr ajpStrChainId         = NULL; /* chain id attribute        */
    AjPStr ajpStrChainSeq        = NULL; /* seq string attribute      */
    AjPSeq ajpSeqChain           = NULL; /* seq AjPStr as seq object  */
    char cFirstResidueType       = '\0'; /* first residue in contact  */
    char cSecondResidueType      = '\0'; /* second residue in contact */
    ajint ajIntFirstResidueType  = 0;    /* first residue in contact  */
    ajint ajIntSecondResidueType = 0;    /* second residue in contact */

    /* scoring matrix */
    ajint ajIntNumberOfResidueTypes = 0;
    AjPInt2d ajpInt2dCounts         = NULL;
    ajint ajIntTempCount            = 0;

    /* number of contacts between first and second residue types */
    ajint ajIntCount1 = 0;
    /* total number of contacts of first residue type */
    ajint ajIntCount2 = 0;

    /* output report file for counts */
    AjPReport ajpReportCounts = NULL;
    AjPStr ajpStrReportHeader = NULL;

    /* DDDDEBUG: string for report footer */
    AjPStr ajpStrReportTail          = NULL;
    AjPFeattable ajpFeattableCounts  = NULL;
    AjPFeature ajpFeatCurrent        = NULL;

    embInit( "contactcount", argc ,argv );

    /* get contact file directory from ACD */
    ajpListCmapFiles = ajAcdGetDirlist("cmapdir");

    /* get contact count output file from ACD */
    ajpReportCounts = ajAcdGetReport("outfile");

    ajIntNumberOfContactFiles = ajListLength(ajpListCmapFiles);

    /* create a 2-D array (count array) to store the scores */
    ajpStrAlphabet = ajStrNewC("ABCDEFGHIJKLMNOPQRSTUVWZYZ");
    ajIntNumberOfResidueTypes = ajStrLen(ajpStrAlphabet);
    ajpInt2dCounts = ajInt2dNewL(ajIntNumberOfResidueTypes);

    /* empty count array */
    for(ajIntRow = 0;ajIntRow < ajIntNumberOfResidueTypes;ajIntRow++)
    {
	for(ajIntColumn = 0;ajIntColumn < ajIntNumberOfResidueTypes;ajIntColumn++)
	{
	    ajInt2dPut(&ajpInt2dCounts,ajIntRow,ajIntColumn,0);
	}
    }

    /* BEGIN LOOP OVER CONTACT MAP FILES IN CURRENT DIRECTORY */
    while(ajListPop(ajpListCmapFiles, (void **)&ajpStrCmapFileName))
    {
	ajpFileCmapCurrent = ajFileNewIn(ajpStrCmapFileName);

	if(!ajpFileCmapCurrent)
	    ajDie("cannot open file %S",ajpStrCmapFileName);

	/* get Cmap object from current Cmap file */
	ajXyzCmapReadI(ajpFileCmapCurrent, 1, 1, &ajpCmapCurrent);

	/* obtain chain name from Cmap object */
	ajpStrChainId = ajpCmapCurrent->Id;

	ajFmtPrint("\n%S\n\n", ajpStrChainId);

	/* obtain chain sequence from Cmap object */
	ajpStrChainSeq = ajpCmapCurrent->Seq;

	ajFmtPrint("\n%S\n\n", ajpStrChainSeq);

	/* obtain sequence object from string */
	ajpStrChainSeq = ajpCmapCurrent->Seq;
	ajpSeqChain = ajSeqNewStr(ajpStrChainSeq);

	/* obtain contact matrix from Cmap object */
	ajpInt2dContactMap = ajpCmapCurrent->Mat;

	/* get dimensions of contact map */
	ajInt2dLen(ajpInt2dContactMap,&ajIntRowMax,&ajIntColumnMax);

	/* loop through all cells in map */
	for(ajIntRow = 0;ajIntRow<ajIntRowMax;ajIntRow++)
	{
	    cFirstResidueType = ajStrChar(ajpStrChainSeq,ajIntRow);
	    for(ajIntColumn = 0;ajIntColumn < ajIntColumnMax;ajIntColumn++)
	    {
		cSecondResidueType = ajStrChar(ajpStrChainSeq,ajIntColumn);
		ajIntFirstResidueType = ajAZToInt(cFirstResidueType);
		ajIntSecondResidueType = ajAZToInt(cSecondResidueType);
		if(ajInt2dGet(ajpInt2dContactMap,ajIntRow,ajIntColumn))
		{
		    ajIntTempCount = ajInt2dGet(ajpInt2dCounts, ajIntFirstResidueType, ajIntSecondResidueType);
		    ajInt2dPut(&ajpInt2dCounts, ajIntFirstResidueType, ajIntSecondResidueType, ajIntTempCount+1);
		}	    
	    }
	}

	ajIntTempCount = ajInt2dGet(ajpInt2dCounts, 6, 11);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS FIRST:\t%d\n", ajIntTempCount);
	ajFmtPrint("=======================\n");
	ajIntTempCount = ajInt2dGet(ajpInt2dCounts, 11, 6);
	ajFmtPrint("=======================\n");
	ajFmtPrint("HERE IT IS AGAIN:\t%d\n", ajIntTempCount);
	ajFmtPrint("=======================\n");

	/* get dimensions of count array */
	ajInt2dLen(ajpInt2dCounts,&ajIntRowMax,&ajIntColumnMax);

	/* chain info for head of report */
	ajFmtPrintS(&ajpStrReportHeader, "Chain: %S", (ajpStrChainId));
	ajReportSetHeader(ajpReportCounts, ajpStrReportHeader);

	/* create feature table for count output */
	ajpFeattableCounts = ajFeattableNewSeq(ajpSeqChain);

	/* DDDDEBUG: DUMMY VALUE FOR CONTACT NUMBER */
	ajIntContactNumber= ajSeqLen(ajpSeqChain)-5;

	/* DDDDEBUGGING */
	ajIntCount1 = 4;
	ajIntCount2 = 7;

	ajpFeatCurrent = write_count(ajpFeattableCounts,
				     ajIntContactNumber,
				     ajIntCount1,
				     ajIntCount2);

	/* write the report to the output file */
	ajReportWrite(ajpReportCounts,
		      ajpFeattableCounts,
		      ajpSeqChain);

	/* close the input file */
	ajFileClose(&ajpFileCmapCurrent);
    }
    /* END LOOP OVER CONTACT MAP FILES IN CURRENT DIRECTORY */

    /* DEBUGGING */
    ajFmtPrint("AFTER AND OUTSIDE LOOP: ajIntNumberOfResidueTypes:\t%d\n", ajIntNumberOfResidueTypes);
    
    /* read elements in count array */
    for(ajIntRow = 0;ajIntRow < ajIntNumberOfResidueTypes;ajIntRow++)
    {
	for(ajIntColumn = 0;ajIntColumn < ajIntNumberOfResidueTypes;ajIntColumn++)
	{
	    cFirstResidueType = ajIntToAZ(ajIntRow);
	    cSecondResidueType = ajIntToAZ(ajIntColumn);
	    ajIntTempCount = ajInt2dGet(ajpInt2dCounts, ajIntRow, ajIntColumn);
	    ajFmtPrint("%d\t", ajIntTempCount);
	}
	ajFmtPrint("\n");
    }

    /* DDDDEBUG TEST INFO FOR TAIL OF REPORT */
    /*     ajFmtPrintS(&ajpStrReportTail, "This is some tail text"); */
    /*     ajReportSetTail(ajpReportCounts, ajpStrReportTail); */


    /* close the report file */
    ajReportDel(&ajpReportCounts);

    /* clear up report objects */
    ajFeattableDel(&ajpFeattableCounts);
    
    /* clean up the list iterator */

    /* clean up the list of files */
    while(ajListPop(ajpListCmapFiles, (void **)&ajpFileCmapCurrent))
	AJFREE(ajpFileCmapCurrent);
    ajListFree(&ajpListCmapFiles);
    
    /* clear up the contact map object */
    ajXyzCmapDel(&ajpCmapCurrent);
    
    /* clean up the derived objects */
    ajStrDel(&ajpStrChainId);
    ajSeqDel(&ajpSeqChain);
    ajStrDel(&ajpStrChainSeq);


    /*  tidy up everything else... */
    ajExit();

    return 0;
}




/* @funcstatic write_count *************************************************
**
** writes frequency features to a feature table and returns new feature  
**
** @param [r] ajpFeattable [AjpFeat] table to write frequency to
** @param [r] ajIntFeatureResidueType [ajint] residue type that count belongs to
** @param [r] fPhiTorsionAngle [float] phi torsion angle for residue
** @param [r] fPhiTorsionAngle [float] psi torsion angle for residue
** @return [AjPFeature]
** @@
******************************************************************************/

static AjPFeature write_count (AjPFeattable ajpFeattableCounts,
				 ajint ajIntFeatureResidueType,
				 ajint ajIntCount1,
				 ajint ajIntCount2)
{
    AjPFeature ajpFeatCounts;
    AjPStr ajpStrFeatTemp;

    ajpStrFeatTemp = ajStrNew();

    /* create feature for count and write per residue and per type frequency */
    ajpFeatCounts = ajFeatNewII(ajpFeattableCounts,
				       ajIntFeatureResidueType,
				       ajIntFeatureResidueType);
    ajFmtPrintS(&ajpStrFeatTemp, "*count1 %d", ajIntCount1);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);
    ajFmtPrintS(&ajpStrFeatTemp, "*count2 %d", ajIntCount2);
    ajFeatTagAdd(ajpFeatCounts, NULL, ajpStrFeatTemp);

    ajStrDel(&ajpStrFeatTemp);
    
    return ajpFeatCounts;
}

