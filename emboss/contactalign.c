/* @source contactalign application
**
** aligns two protein sequences, seqDownSequence and seqAcrossSequence,
**  by modifying a Needleman and Wunsch backtrace matrix according to
**  a contact-based scoring scheme
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.30 $
** @modified $Date: 2005/01/21 17:29:12 $
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
	enumDebugLevel        =  3,
	enumArrayOffset       =  1,
	enumTotalResTypes     = 30,
	enumAsciiOffset       = 65,
	enumMaxCmapLineLen    = 80
    };


/* @prog contactalign ********************************************************
** 
** align one protein sequence of unknown structure to another protein sequence
**  of known structure, using both a one-dimensional substitution scoring
**  scheme and on the basis of additional residue contact information
**
******************************************************************************/

int main(int argc , char **argv)
{
    /* sequence objects */
    AjPSeq ajpSeqDown = NULL;   /* query sequence---no structure             */
    AjPSeq ajpSeqAcross = NULL; /* template sequence---has structure         */
    AjPSeq ajpSeqDownCopy = NULL;   /* rewritable query sequence             */
    AjPSeq ajpSeqAcrossCopy = NULL; /* rewritable template sequence          */
    /* sequence characters */
    char cTempTraceDown;
    char cTempTraceAcross;
    /* sequence strings */
    char *pcSeqAcross;
    char *pcTraceDown;
    char *pcTraceAcross;
    char *pcUpdatedSeqAcross;
    /* sequence lengths */
    ajint ajIntDownSeqLen;
    ajint ajIntAcrossSeqLen;
    ajint ajIntAlignmentLen;
    /* sequence counts */    
    ajint ajIntSeqCount;
    ajint ajIntTraceCount;

    AjPSeqout ajpSeqoutAligned = NULL; /* output object to write alignment   */

    /* penalties */
    float fExtensionPenalty;
    float fGapPenalty;
    AjBool ajBoolZeroEndPenalty;

    /* prebuilt scoring matrix (e.g. BLOSUM62) */
    AjPMatrixf ajpMatrixfSubstitutionScoring = NULL;
    /* array for pair scores according to conventional scoring matrix  */
    AjPFloat2d ajpFloat2dPairScores = NULL;
    /* contact-based scoring matrix */
    AjPMatrixf ajpMatrixfContactScoring = NULL;
    /* array for pair scores according to contact-based scoring matrix */
    float **floatArrayContactScores = NULL;

    /* array for recursively summed scores in alignment backtrace */
    AjPGotohCell **ajpGotohCellGotohScores;
    AjPGotohCell ajpGotohCellForDeletion;
    /* stack of backtraced cells */
    AjPList ajpListGotohCellsMaxScoringTrace = NULL;

    /* DDDDEBUG input contact map filename */
    AjPStr ajpStrOriginalCmapFile = NULL;
    AjPFile ajpFileOriginalCmap = NULL;
    /* DDDDEBUG output contact maps */
    AjPStr ajpStrUpdatedCmapFile = NULL;
    AjPFile ajpFileUpdatedCmap = NULL;
    AjPStr ajpStrSubstitutedCmapFile = NULL;
    AjPFile ajpFileSubstitutedCmap = NULL;
    
    
    /* contact map components */
    EmbPCmapHeader embpCmapHeader = NULL;
    AjPInt2d ajpInt2dCmapSummary = NULL;
    AjPInt2d ajpInt2dCmapResTypes = NULL;
    AjPInt2d ajpInt2dCmapPositions = NULL;
    AjPInt2d ajpInt2dSubstitutedCmapSummary = NULL;
    AjPInt2d ajpInt2dSubstitutedCmapResTypes = NULL;
    AjPInt2d ajpInt2dSubstitutedCmapPositions = NULL;

    /* has template contact map been read successfully? */
    AjBool ajBoolOriginalCmapFileRead = ajFalse;
    /* has contact map been updated successfully? */
    AjBool ajBoolUpdatedCmapFileWritten = ajFalse;
    /* has updated contact map been read successfully? */
    AjBool ajBoolUpdatedCmapFileRead = ajFalse;
    /* has updated contact map been substituted successfully? */
    AjBool ajBoolSubstitutedCmapFileWritten = ajFalse;

    embInit("contactalign", argc, argv);
  
    /* DDDDEBUGGING: DEFAULT GAP AND EXTENSION PENALTIES SET BELOW */

    /* ajAcdGet functions access acd values embInit has put in memory */
    ajpSeqDown   = ajAcdGetSeq("down");
    ajpSeqAcross = ajAcdGetSeq("across");
    ajpMatrixfSubstitutionScoring = ajAcdGetMatrixf("substitutionscoringfile");
    ajpMatrixfContactScoring = ajAcdGetMatrixf("contactscoringfile");
    ajpSeqoutAligned  = ajAcdGetSeqout("aligned");
    fGapPenalty       = -ajAcdGetFloat("gapopen");
    fExtensionPenalty =  -ajAcdGetFloat("gapextend");
    ajBoolZeroEndPenalty =  ajAcdGetBool("zeroend");

    /* get sequence lengths: no. rows and columns in Gotoh sum array */
    ajIntDownSeqLen = ajSeqLen(ajpSeqDown);
    ajIntAcrossSeqLen = ajSeqLen(ajpSeqAcross);

    /* score the sequences to produce a scoring AjpMatrix */
    ajpFloat2dPairScores = embGotohPairScore(ajpMatrixfSubstitutionScoring,
					     ajpSeqDown,
					     ajpSeqAcross);
    
    /* initialize Gotoh score array */
    ajpGotohCellGotohScores = embGotohCellGetArray(ajIntDownSeqLen,
						   ajIntAcrossSeqLen);

    embGotohCellCalculateSumScore(ajpGotohCellGotohScores,
				  ajpFloat2dPairScores,
				  ajpSeqDown,
				  ajpSeqAcross,
				  fGapPenalty,
				  fExtensionPenalty,
				  ajBoolZeroEndPenalty);
    
    /* copy original sequences into new sequence objects */
    ajpSeqDownCopy = ajSeqNewS(ajpSeqDown);
    ajpSeqAcrossCopy = ajSeqNewS(ajpSeqAcross);

    /* pile up the backtraced elements onto a list */
    ajpListGotohCellsMaxScoringTrace = ajListNew();
    ajIntAlignmentLen = embGotohCellBacktrace(ajpGotohCellGotohScores,
					      ajpSeqDownCopy,
					      ajpSeqAcrossCopy,
					      ajpListGotohCellsMaxScoringTrace);
    
    /* read the backtraced elements off the list */
    embGotohReadOffBacktrace(ajpListGotohCellsMaxScoringTrace,
			     ajpSeqDownCopy,
			     ajpSeqAcrossCopy);

    /* substitute all matches into a new version of the template sequence */
    pcUpdatedSeqAcross = ajSeqCharCopy(ajpSeqAcross);
    pcTraceDown = ajSeqCharCopy(ajpSeqDownCopy);
    pcTraceAcross = ajSeqCharCopy(ajpSeqAcrossCopy);

    ajIntSeqCount = 0;
    
    for(ajIntTraceCount = 0;ajIntTraceCount < ajIntAlignmentLen; ajIntTraceCount++)
    {
	cTempTraceDown = pcTraceDown[ajIntTraceCount];
	cTempTraceAcross = pcTraceAcross[ajIntTraceCount];
	if(cTempTraceAcross == '-')
	{
	    ajIntTraceCount++;
	}
	else
	{
	    ajFmtPrint("%c\t", cTempTraceAcross);
	    ajFmtPrint("%c\t", cTempTraceDown);
	    if( cTempTraceDown != '-')
		pcUpdatedSeqAcross[ajIntSeqCount] = cTempTraceDown;
	    ajFmtPrint("%c\n", pcUpdatedSeqAcross[ajIntSeqCount]);
	}
    }

    /* DDDD DEBUG DUMMY FILENAME BELOW */
    ajpStrOriginalCmapFile =
	ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/SADtest/1hgx_a.con");
    ajpFileOriginalCmap = ajFileNewIn(ajpStrOriginalCmapFile);

    pcSeqAcross = ajSeqCharCopy(ajpSeqAcross);

    /* reserve memory for objects representing contact map */
    ajpInt2dCmapSummary = embGetCmapSummary(pcSeqAcross);
    ajpInt2dCmapResTypes = embGetIntMap(ajIntAcrossSeqLen +
					enumArrayOffset);
    ajpInt2dCmapPositions = embGetIntMap(ajIntAcrossSeqLen +
					 enumArrayOffset);
    embpCmapHeader = embCmapHeaderNew();

    /* DDDDEBUGGING */
    if( enumDebugLevel > 2 )
    {
	debug_cmap_summary(&ajpInt2dCmapSummary,
			   ajIntAcrossSeqLen);
    }
    if( enumDebugLevel > 2 )
    {
	debug_int_map(&ajpInt2dCmapSummary,
		      &ajpInt2dCmapResTypes,
		      &ajpInt2dCmapPositions,
		      pcSeqAcross,
		      ajIntAcrossSeqLen);
    }

    /* XXXX EVERYTHING FROM HERE IS TO DO WITH ALIGNMENT MODIFICATION */
    
    /* read in original contact map */ 
    ajBoolOriginalCmapFileRead =
	embReadCmapFile(ajpFileOriginalCmap,
			ajIntAcrossSeqLen,
			&ajpInt2dCmapSummary,
			&embpCmapHeader,
			&ajpInt2dCmapResTypes,
			&ajpInt2dCmapPositions);    

    ajFileClose(&ajpFileOriginalCmap);

    /* XXXX HARD-CODED FILENAME IS USED BELOW */
    ajpStrUpdatedCmapFile =
	ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/SADtest/test1.con");
    ajpFileUpdatedCmap = ajFileNewOut(ajpStrUpdatedCmapFile);

    /*
     * write unsubstituted contact arrays
     *  to a new contact map file that
     *  contains contact scores
     */
    ajBoolUpdatedCmapFileWritten =
	embWriteUpdatedCmapFile(ajpFileUpdatedCmap,
				ajIntAcrossSeqLen,
				&ajpInt2dCmapSummary,
				&embpCmapHeader,
				&ajpInt2dCmapResTypes,
				&ajpInt2dCmapPositions,
				ajpMatrixfContactScoring);

    ajFileClose(&ajpFileUpdatedCmap);

    /* XXXX HARD-CODED FILENAME IS USED BELOW */
    ajpFileUpdatedCmap = ajFileNewIn(ajpStrUpdatedCmapFile);
    
    /*
     * read in updated contact map and
     *  revise according to alignment
     *
     */ 
    ajBoolUpdatedCmapFileRead =
	embReadAndReviseCmapFile(ajpFileUpdatedCmap,
				 ajIntAcrossSeqLen,
				 &ajpInt2dCmapSummary,
				 &embpCmapHeader,
				 &ajpInt2dCmapResTypes,
				 &ajpInt2dCmapPositions,
				 pcUpdatedSeqAcross);    

    ajFileClose(&ajpFileUpdatedCmap);

    /* XXXX HARD-CODED FILENAME IS USED BELOW */
    ajpStrSubstitutedCmapFile =
	ajStrNewC("/users/damian/EMBOSS/emboss/emboss/emboss/SADtest/test2.con");
    ajpFileSubstitutedCmap = ajFileNewOut(ajpStrSubstitutedCmapFile);

    /* reserve memory for objects representing substituted contact map */
    ajpInt2dSubstitutedCmapSummary =
	embGetCmapSummary(pcUpdatedSeqAcross);
    ajpInt2dSubstitutedCmapResTypes =
	embGetIntMap(ajIntAcrossSeqLen + enumArrayOffset);
    ajpInt2dSubstitutedCmapPositions =
	embGetIntMap(ajIntAcrossSeqLen + enumArrayOffset);

    /*
     * write SUBSTITUTED contact arrays
     *  to a new contact map file that
     *  contains contact scores
     */
    ajBoolSubstitutedCmapFileWritten =
	embWriteUpdatedCmapFile(ajpFileSubstitutedCmap,
				ajIntAcrossSeqLen,
				&ajpInt2dCmapSummary,
				&embpCmapHeader,
				&ajpInt2dCmapResTypes,
				&ajpInt2dCmapPositions,
				ajpMatrixfContactScoring);

    /* XXXX FIND CORRESPONDING FIRST AND SECOND RESIDUES IN ORIGINAL TEMPLATE SEQUENCE */


    /* XXXX FIND CORRESPONDING FIRST AND SECOND RESIDUES IN ORIGINAL QUERY SEQUENCE */

    /* XXXX COUNT REAL RESIDUES IN FIRST SEQUENCE */

    /* XXXX LOOK UP FIRST CONTACT RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN QUERY SEQUENCE */

    /* XXXX LOOK UP SECOND CONTACT RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN QUERY SEQUENCE */

    /* XXXX LOOK UP FIRST RESIDUE POSITION AND COUNT THAT NUMBER OF REAL RESIDUES IN SECOND SEQUENCE */

    /* XXXX LOOK UP PROBABILITY SCORE FOR NEW PAIR */

    /* XXXX FIND RELEVANT CELL IN ALIGNMENT MATRIX AND PENALIZE IT */    

    /* XXXX YOU NEED SOME CODE HERE TO COMPARE THE PROBABILITIES OF EACH PAIR */

    embCmapHeaderDel(&embpCmapHeader);

    /* write out "aligned" sequences  */
    ajSeqWrite(ajpSeqoutAligned, ajpSeqDownCopy);
    ajSeqWrite(ajpSeqoutAligned, ajpSeqAcrossCopy);

    /* delete objects */
    ajSeqDel(&ajpSeqDown);
    ajSeqDel(&ajpSeqAcross);
    ajMatrixfDel(&ajpMatrixfSubstitutionScoring);
    ajFloat2dDel(&ajpFloat2dPairScores);
    ajSeqDel(&ajpSeqDownCopy);
    ajSeqDel(&ajpSeqAcrossCopy);
    while(ajListPop(ajpListGotohCellsMaxScoringTrace,
		    (void **)&ajpGotohCellForDeletion))
	AJFREE(ajpGotohCellForDeletion);
    ajListFree(&ajpListGotohCellsMaxScoringTrace);    
    ajFileClose(&ajpFileOriginalCmap);

    /* delete output sequence */
    ajSeqWriteClose(ajpSeqoutAligned);
    ajSeqoutDel(&ajpSeqoutAligned);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
