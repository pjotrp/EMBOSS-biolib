/* @source nawalign application
**
** aligns two protein sequences: seqDownSequence and seqAcrossSequence
**  (eventually using a structure-weighted alignment method)---at the
**   moment implementing Needleman and Wunsch 
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.1 $
** @modified $Date: 2004/09/06 19:55:13 $
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
	enumDebugLevel        =  2,
	enumArrayOffset       =  1,
	enumTotalResTypes     = 30,
	enumAsciiOffset       = 65,
	enumMaxCmapLineLen    = 80
    };


/* @prog nawalign ************************************************************
** 
** align two protein sequences according to the method of Needleman and Wunsch
**
******************************************************************************/

int main(int argc , char **argv)
{
    /* sequence objects */
    AjPSeq ajpSeqDown = NULL;   /* query sequence---no structure             */
    AjPSeq ajpSeqAcross = NULL; /* template sequence---has structure         */
    AjPSeq ajpSeqDownCopy = NULL;   /* rewritable query sequence             */
    AjPSeq ajpSeqAcrossCopy = NULL; /* rewritable template sequence          */
    /* sequence lengths */
    ajint ajIntDownSeqLen;
    ajint ajIntAcrossSeqLen;
    ajint ajIntAlignmentLen;
    
    AjPSeqout ajpSeqoutAligned = NULL; /* output object to write alignment   */

    /* alignment gap extension and gap penalty scores */
    float fExtensionPenalty;
    float fGapPenalty;

    /* prebuilt scoring matrix (e.g. BLOSUM62) */
    AjPMatrixf ajpMatrixfSubstitutionScoring = NULL;
    /* array for pair scores according to scoring matrix  */
    AjPFloat2d ajpFloat2dPairScores = NULL;

    /* array for recursively summed scores in alignment backtrace */
    AjPGotohCell **ajpGotohCellGotohScores;
    AjPGotohCell ajpGotohCellForDeletion;
    /* stack of backtraced cells */
    AjPList ajpListGotohCellsMaxScoringTrace = NULL;

    embInit("nawalign", argc, argv);

    /* ajAcdGet functions access acd values embInit has put in memory */
    ajpSeqDown   = ajAcdGetSeq("down");
    ajpSeqAcross = ajAcdGetSeq("across");
    ajpMatrixfSubstitutionScoring = ajAcdGetMatrixf("substitutionscoringfile");
    ajpSeqoutAligned  = ajAcdGetSeqout("aligned");
    fGapPenalty       = -ajAcdGetFloat("gapopen");
    fExtensionPenalty =  -ajAcdGetFloat("gapextend");

    /* get sequence lengths: no. rows and columns in Gotoh sum array */
    ajIntDownSeqLen = ajSeqLen(ajpSeqDown);
    ajIntAcrossSeqLen = ajSeqLen(ajpSeqAcross);

    /* score the sequences to produce a scoring AjpMatrix */
    ajpFloat2dPairScores = embGotohPairScore(ajpMatrixfSubstitutionScoring,
					     ajpSeqDown,
					     ajpSeqAcross,
					     fExtensionPenalty);
    
    /* initialize Gotoh score array */
    ajpGotohCellGotohScores = embGotohCellGetArray(ajIntDownSeqLen,
						   ajIntAcrossSeqLen);

    embGotohCellCalculateSumScore(ajpFloat2dPairScores,
				  ajpSeqDown,
				  ajpSeqAcross,
				  ajpGotohCellGotohScores,
				  fGapPenalty,
				  fExtensionPenalty);
    
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

    /* write out aligned sequences  */
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

    /* delete output sequence */
    ajSeqWriteClose(ajpSeqoutAligned);
    ajSeqoutDel(&ajpSeqoutAligned);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
