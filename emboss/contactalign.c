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

#define TRACE_BACK_ARRAY_SEQ_OFFSET 1
#define DEBUG_LEVEL 0

#include "emboss.h"
#include <math.h>



static void debug_pair_score(AjPFloat2d ajpFloat2dPairScores,
			     AjPSeq ajpSeqDown,
			     AjPSeq ajpSeqAcross);

static void debug_Gotoh_score(AjPGotohCell **ajpGotohCellGotohScores,
			      ajint ajIntDownSeqLen,
			      ajint ajIntAcrossSeqLen);



/* @prog contactalign ********************************************************
** 
** align two protein sequence of unknown structure to protein sequence of
**  known structure, given residue contact information
**
** @param [r] arrayOfAjpatomsWindow [AjPAtom*] window of nine mainchain atoms
** @return [float] 
** @@ 
******************************************************************************/

int main( int argc , char **argv)
{
    /* sequence objects and copies */
    AjPSeq ajpSeqDown = NULL;
    AjPSeq ajpSeqAcross = NULL;
    AjPSeq ajpSeqDownCopy = NULL;
    AjPSeq ajpSeqAcrossCopy = NULL;
    /* object to write out aligned sequence */
    AjPSeqout ajpSeqoutAligned = NULL;
    /* prebuilt scoring AjpMatrix (e.g. BLOSUM62) */
    AjPMatrixf ajpMatrixFscoring = NULL;

    /* array for pair scores according to scoring matrix */
    AjPFloat2d ajpFloat2dPairScores = NULL;

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

    embInit( "contactalign" , argc , argv);
  
    /* XXXX DEFAULT GAP AND EXTENSION PENALTIES SET BELOW */
    fGapPenalty       = -10.0;
    fExtensionPenalty = -0.5;

    /* ajAcdGet functions access acd values embInit has put in memory */
    ajpSeqDown   = ajAcdGetSeq("down");
    ajpSeqAcross = ajAcdGetSeq("across");
    ajpMatrixFscoring = ajAcdGetMatrixf("datafile");
    ajpSeqoutAligned  = ajAcdGetSeqout("aligned");

    /* get sequence lengths: no. rows and columns in Gotoh sum array */
    ajIntDownSeqLen = ajSeqLen(ajpSeqDown);
    ajIntAcrossSeqLen = ajSeqLen(ajpSeqAcross);

    /* score the sequences to produce a scoring AjpMatrix */
    ajpFloat2dPairScores = embGotohPairScore(ajpMatrixFscoring,
					     ajpSeqDown,
					     ajpSeqAcross,
					     fExtensionPenalty);

    /* DDDDEBUG CHECK THAT WORKED */
    if(DEBUG_LEVEL > 1)
    {
	debug_pair_score(ajpFloat2dPairScores, ajpSeqDown, ajpSeqAcross);
    }

    /* initialize Gotoh score array */
    ajpGotohCellGotohScores = embGotohCellGetArray(ajIntDownSeqLen +
						   TRACE_BACK_ARRAY_SEQ_OFFSET,
						   ajIntAcrossSeqLen +
						   TRACE_BACK_ARRAY_SEQ_OFFSET);
    
    /* DDDDEBUG CHECK THAT WORKED */
    if(DEBUG_LEVEL > 1)
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
    if(DEBUG_LEVEL > 0)
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
    
    embGotohReadOffBacktrace(ajpListGotohCellsMaxScoringTrace, ajpSeqDownCopy, ajpSeqAcrossCopy);

    /* write out "aligned" sequences  */
    ajSeqWrite(ajpSeqoutAligned, ajpSeqDownCopy);
    ajSeqWrite(ajpSeqoutAligned, ajpSeqAcrossCopy);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}

/* 16Jan02                debug_Gotoh_score()                  Damian Counsell  */
/*                                                                              */
/* prints out result of Gotoh scoring                                           */

void debug_Gotoh_score(AjPGotohCell **ajpGotohCellGotohScores, ajint ajIntDownSeqLen, ajint ajIntAcrossSeqLen)
{
  ajint ajIntRowCount;
  ajint ajIntColumnCount;

  for(ajIntRowCount = 0 ; ajIntRowCount < ajIntDownSeqLen + TRACE_BACK_ARRAY_SEQ_OFFSET ; ajIntRowCount++)
    {
      for(ajIntColumnCount = 0 ; ajIntColumnCount < ajIntAcrossSeqLen + TRACE_BACK_ARRAY_SEQ_OFFSET ; ajIntColumnCount++)
	{
	  ajFmtPrint("====================================================================================================\n");
	  ajFmtPrint("GOTOH (%4d, %4d):\ti pointer: %4d j pointer: %4d\tsubscore: %3.3f\ttemplate residue:\t%c\tquery residue:\t%c\tisIndel?: %B\n", \
		       ajIntRowCount, ajIntColumnCount, \
		       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> ajIntRow, \
		       ajpGotohCellGotohScores[ ajIntRowCount ][ ajIntColumnCount ] -> ajIntColumn, \
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
/* prints out result of pairscoring                                           */

void debug_pair_score( AjPFloat2d ajpFloat2dPairScores , AjPSeq ajpSeqDown , AjPSeq ajpSeqAcross )
{
  ajint ajIntMaxRow;
  ajint ajIntMaxColumn;
  ajint ajIntRow;
  ajint ajIntColumn;

  /* get size of sequences---and therefore good array values */
  ajIntMaxRow = ajSeqLen(ajpSeqDown) + TRACE_BACK_ARRAY_SEQ_OFFSET;
  ajIntMaxColumn = ajSeqLen(ajpSeqAcross) + TRACE_BACK_ARRAY_SEQ_OFFSET;

  if(DEBUG_LEVEL > 0)
  {
      ajFmtPrint( "ROWS:\t%d\tCOLUMNS:\t%d\n" , ajIntMaxRow , ajIntMaxColumn );
  }
  
  if(DEBUG_LEVEL > 1)
  {
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
  }

  return;
}
