/******************************************************************************
** @source NUCLEUS Gotoh dynamic programming functions
**
** sequence alignment by the method of Gotoh uses cells (here, AjPGotohCells)
**  containing pointers to preceding cells in sequence alignment backtrace
**  tables:
**
**  Gotoh,  O., "An Improved algorithm for matching biological sequences."
**  Journal of Molecular Biology 162:705-708
**
** @author Copyright (C) 2003--2004 Damian Counsell
** @version $Revision: 1.5 $
** @modified $Date: 2004/02/18 19:21:47 $
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
#include "embgotoh.h"

static const ajint cAjIntOffset = 1;

/* DDDDEBUG */
static ajint DEBUG_LEVEL = 0;

/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

/* ==================================================================== */
/* ========================= constructors ============================= */
/* ==================================================================== */

/* @section AjPGotohCell Constructors ****************************************
**
** All constructors return a new AjPGotohCell by pointer. It is the
** responsibility of the user to first destroy any previous cell. The target
** pointer does not need to be initialised to NULL, but it is good programming
** practice to do so anyway.
**
******************************************************************************/

/* @func embGotohCellNew *****************************************************
**
** Default constructor for NUCLEUS Gotoh alignment table cell.
**
** @return [AjPGotohCell] Pointer to an AjPGotohCell
** @@
******************************************************************************/

AjPGotohCell embGotohCellNew(void)
{
    AjPGotohCell ajpGotohCellReturned = NULL;

    AJNEW0(ajpGotohCellReturned);

    return ajpGotohCellReturned;
}




/* @func embGotohCellCreate **************************************************
**
** Constructor for initialized NUCLEUS Gotoh cell
** @param [r] ajIntRow [ajint] row in backtrace table
** @param [r] ajIntColumn [ajint] column in backtrace table
** @param [r] fSubScore [float] intermediate cumulative alignment score
** @param [r] cDownResidue [char] residue in template indexed by row 
** @param [r] cAcrossResidue [char] residue in query indexed by column
** @param [r] ajBoolIsIndel [AjBool] does cell correspond to an indel?
**
** @return [AjPGotohCell] pointer to an initialized Gotoh cell
** @@
******************************************************************************/

AjPGotohCell embGotohCellCreate(ajint ajIntRow, ajint ajIntColumn,
				float fSubScore,
				char cDownResidue,
				char cAcrossResidue,
				AjBool ajBoolIsIndel)
{
    AjPGotohCell ajpGotohCellReturned;

    AJNEW0(ajpGotohCellReturned);
    
    ajpGotohCellReturned->ajIntRowPointer = ajIntRow;
    ajpGotohCellReturned->ajIntColumnPointer = ajIntColumn;
    ajpGotohCellReturned->fSubScore = fSubScore;
    ajpGotohCellReturned->cDownResidue = cDownResidue;
    ajpGotohCellReturned->cAcrossResidue = cAcrossResidue;
    ajpGotohCellReturned->ajBoolIsIndel = ajBoolIsIndel;

    return ajpGotohCellReturned;  
}




/* ==================================================================== */
/* =========================== destructor ============================= */
/* ==================================================================== */


/* @section Gotoh cell Destructors *******************************************
**
** Destruction is achieved by deleting the pointer to the 3-D vector and
**  freeing the associated memory
**
******************************************************************************/

/* @func embGotohCellDel *****************************************************
**
** Default destructor for NUCLEUS Gotoh cells
**
** If the given pointer is NULL, or a NULL pointer, simply returns.
**
** @param  [w] pthis [AjPGotohCell*] Pointer to the Gotoh cell to be deleted.
**         The pointer is always deleted.
** @return [void]
** @@
******************************************************************************/

void embGotohCellDel(AjPGotohCell* pthis)
{
    AjPGotohCell thys = NULL;

    thys = pthis ? *pthis :0;

    if(!pthis)
	return;

    if(!*pthis)
	return;

    thys->ajIntRowPointer = 0;
    thys->ajIntColumnPointer = 0;
    thys->fSubScore = 0.0;
    thys->cDownResidue = '\0';
    thys->cAcrossResidue = '\0';
    thys->ajBoolIsIndel = AJFALSE;

    AJFREE(thys);
    *pthis = NULL;

    return;
}




/* @func embGotohCellGetArray ************************************************
**
** reserves memory for an array of Gotoh cells for summing pair score array
**
** @param [r] ajIntDownSeqLen [ajint] size down
** @param [r] ajIntAcrossSeqLen [ajint] size across
** @return [AjPGotohCell**] Gotoh cell array address
** @@
******************************************************************************/


AjPGotohCell** embGotohCellGetArray(ajint ajIntDownSeqLen,
				    ajint ajIntAcrossSeqLen)
{
  ajint ajIntRow;
  ajint ajIntColumn;

  AjPGotohCell **ajpGotohCellGotohScores;

  AJCNEW0(ajpGotohCellGotohScores,(ajIntDownSeqLen + cAjIntOffset));

  for(ajIntRow = 0;
      ajIntRow < (ajIntDownSeqLen + cAjIntOffset);
      ajIntRow++)
    {
      AJCNEW0(ajpGotohCellGotohScores[ajIntRow],(ajIntAcrossSeqLen + cAjIntOffset));
    }

  for(ajIntRow = 0;
      ajIntRow < (ajIntDownSeqLen + cAjIntOffset);
      ajIntRow++)
    {
      for(ajIntColumn = 0;
	  ajIntColumn < (ajIntAcrossSeqLen + cAjIntOffset);
	  ajIntColumn++)
	{
	  AJNEW0(ajpGotohCellGotohScores[ajIntRow][ajIntColumn]);
	}
    }

  /* set default values   */
  for(ajIntRow = 0;
      ajIntRow < (ajIntDownSeqLen + cAjIntOffset);
      ajIntRow++)
    {
    for(ajIntColumn = 0;
	ajIntColumn < (ajIntAcrossSeqLen + cAjIntOffset);
	ajIntColumn++)
      {
  	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->fSubScore = 0.0;
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntRowPointer = 0;
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntColumnPointer = 0;
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajBoolIsIndel = AJFALSE;
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cDownResidue = '$';
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cAcrossResidue = '#';
      }
    }

  return ajpGotohCellGotohScores;
}




/* @func embGotohCellCalculateSumScore ***************************************
**
** Calculates Gotoh cells for summing pair score array
**
** @param [r] ajpFloat2dPairScores [AjPFloat2d] Pair score array
** @param [r] ajpSeqDown [AjPSeq] Sequence down
** @param [r] ajpSeqAcross [AjPSeq] Seqeunce across
** @param [w] ajpGotohCellGotohScores [AjPGotohCell**] Gotoh cell array
** @param [r] fGapPenalty [float] Gap penalty
** @param [r] fExtensionPenalty [float] Gap extension penalty
** @return [void]
** @@
******************************************************************************/


void embGotohCellCalculateSumScore(AjPFloat2d ajpFloat2dPairScores,
				   AjPSeq ajpSeqDown,
				   AjPSeq ajpSeqAcross,
				   AjPGotohCell **ajpGotohCellGotohScores,
				   float fGapPenalty,
				   float fExtensionPenalty)
{
  ajint ajIntRowMax;
  ajint ajIntColumnMax;
  AjPStr ajpStrDownSeq = NULL;
  AjPStr ajpStrAcrossSeq = NULL;
  ajint ajIntRow;
  ajint ajIntColumn;
  ajint ajIntMaxRowPosition;
  ajint ajIntMaxColumnPosition;
  float fUpperLeftSum;
  float fUpperPenalty;
  float fLeftPenalty;
  float fUpperSum;
  float fLeftSum;
  float fMaxSum;
  AjBool ajbIsCurrentIndel;

  /* obtain proper scores for all cells                       */

  /* deal with boundary conditions                            */
  /* first set top left cell to zero...                       */
  ajpGotohCellGotohScores[0][0]->fSubScore= 0.0;
  ajpGotohCellGotohScores[0][0]->ajIntRowPointer = 0;
  ajpGotohCellGotohScores[0][0]->ajIntColumnPointer = 0;
  ajpGotohCellGotohScores[0][0]->cDownResidue = '|';
  ajpGotohCellGotohScores[0][0]->cAcrossResidue = '_';
  ajpGotohCellGotohScores[0][0]->ajBoolIsIndel = AJFALSE;

  /* get dimensions of matrix */
  ajIntRowMax = ajSeqLen(ajpSeqDown);
  ajIntColumnMax = ajSeqLen(ajpSeqAcross);

  /* get sequence strings */
  ajpStrDownSeq = ajSeqStr(ajpSeqDown);
  ajpStrAcrossSeq = ajSeqStr(ajpSeqAcross);

  ajIntRow = 0;
  /* ...then set topmost row to multiples of extension penalty */
  /*  (-1 multiple for later recursive score)                  */
  for (ajIntColumn = cAjIntOffset;ajIntColumn <= ajIntColumnMax;ajIntColumn++)
    {
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->fSubScore
	    = ((float)ajIntColumn * fExtensionPenalty);
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntRowPointer
	    = ajIntRow;
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntColumnPointer
	    = (ajIntColumn - 1);
	ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajBoolIsIndel
	    = AJTRUE;
	    ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cDownResidue
		= '|';
	    ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cAcrossResidue
		= ajStrChar(ajpStrAcrossSeq, ajIntColumn - cAjIntOffset);
    }

  /* ...then set leftmost column to multiples of gap penalty */
  /*  (-1 multiple for later recursive score)                */
  ajIntColumn = 0;
  for (ajIntRow = cAjIntOffset;ajIntRow <= ajIntRowMax;ajIntRow++)
    {
      ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->fSubScore
	  = ((float)ajIntRow * fExtensionPenalty);
      ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntRowPointer
	  = (ajIntRow - 1);
      ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntColumnPointer
	  = ajIntColumn ;
      ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajBoolIsIndel
	  = AJTRUE;
      ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cDownResidue
	  = ajStrChar(ajpStrDownSeq, ajIntRow - cAjIntOffset);
      ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cAcrossResidue
	  = '_';
    }

  ajbIsCurrentIndel = AJFALSE;

  /* start one row from top-left cell                         */
  /* and finish one row from bottom-right cell   */
  for(ajIntRow = cAjIntOffset;ajIntRow <= ajIntRowMax;ajIntRow++)
    {
    /* start one column from top-left cell                   */
    /*  and finish one column from bottom-right cell         */
    for(ajIntColumn = cAjIntOffset;ajIntColumn <= ajIntColumnMax;ajIntColumn++)
      {
	  /* THIS IS THE CORE DYNAMIC PROGRAMMING ALGORITHM */

	  /* calculate cumulative score for north-west cell */
	  fUpperLeftSum =
	      ajpGotohCellGotohScores[ajIntRow-1][ajIntColumn-1]->fSubScore +
	      (ajFloat2dGet(ajpFloat2dPairScores, ajIntRow, ajIntColumn));

	  /* calculate cumulative (affine gap) score for north cell */  
	  if(ajpGotohCellGotohScores[ajIntRow-1][ajIntColumn]->ajBoolIsIndel)
	  {
	      fUpperPenalty = fExtensionPenalty;
	  }
	  else fUpperPenalty = fGapPenalty;
	  fUpperSum =
	      ajpGotohCellGotohScores[ajIntRow-1][ajIntColumn]->fSubScore +
	      fUpperPenalty;
	  
	  /* calculate cumulative (affine gap) score for west cell */
	  if(ajpGotohCellGotohScores[ajIntRow][ajIntColumn-1]->ajBoolIsIndel)
	  {
	      fLeftPenalty = fExtensionPenalty;
	  }
	  else fLeftPenalty = fGapPenalty;
	  fLeftSum =
	      ajpGotohCellGotohScores[ajIntRow][ajIntColumn-1]->fSubScore +
	      fLeftPenalty;

	  /* FIND MAXIMUM SCORE OF THE THREE */

	  /* default to north-west as max    */
	  fMaxSum = fUpperLeftSum;
	  ajIntMaxRowPosition = ajIntRow-1;
	  ajIntMaxColumnPosition = ajIntColumn-1;
	  ajbIsCurrentIndel = AJFALSE;

	  /* north max if greater */
	  if(fUpperSum > fMaxSum)
	  {
	    fMaxSum = fUpperSum;
	    ajbIsCurrentIndel = AJTRUE;
	    ajIntMaxColumnPosition = ajIntColumn;
	    ajIntMaxRowPosition = ajIntRow-1;
	  }

	  /* west max if greater */
	  if(fLeftSum > fMaxSum)
	  {
	      fMaxSum = fLeftSum;
	      ajbIsCurrentIndel = AJTRUE;
	      ajIntMaxRowPosition = ajIntRow;
	      ajIntMaxColumnPosition = ajIntColumn-1;
	  }
	  
	  /* set resulting current cell score */
	  ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntRowPointer =
	      ajIntMaxRowPosition;
	  ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajIntColumnPointer =
	      ajIntMaxColumnPosition;
	  ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->fSubScore =
	      fMaxSum;
	  ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cDownResidue =
	      ajStrChar(ajpStrDownSeq , ajIntRow - cAjIntOffset);
	  ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->cAcrossResidue =
	      ajStrChar(ajpStrAcrossSeq , ajIntColumn - cAjIntOffset);
	  ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->ajBoolIsIndel =
	      ajbIsCurrentIndel;
	  
	  /* DDDDEBUGGING */
	  if (DEBUG_LEVEL)
	  {    
	      ajFmtPrint("====================================================================================================\n");
	      ajFmtPrint("IN embGotohCellCalculateSumScore() (%4d , %4d):\nfUpperLeftSum: %f fUpperSum: %f fLeftSum:\t%f\n" , \
			 ajIntRow , ajIntColumn , fUpperLeftSum , fUpperSum , fLeftSum);
	      ajFmtPrint("sub score:\t%f fMaxSum: %f\nipointer :%d jpointer :%d \n" , \
			 ajpGotohCellGotohScores[ajIntRow][ajIntColumn]->fSubScore , fMaxSum , ajIntMaxRowPosition , ajIntMaxColumnPosition);
	      ajFmtPrint("====================================================================================================\n");
	  }
	  
      }
    }
}



/* @func embGotohCellBacktrace ************************************************
**
**  backtraces through the elements of a pair-scoring array according
**  to the method of Gotoh,  O., "An Improved algorithm for matching
**  biological sequences." Journal of Molecular Biology 162:705-708
**
** @param [r] ajpGotohCellGotohScores [AjPGotohCell**] Gotoh cell array
** @param [r] ajpSeqDown [AjPSeq] Sequence down
** @param [r] ajpSeqAcross [AjPSeq] Sequence across
** @param [r] ajpListGotohCellsMaxScoringTrace [AjPList] Trace
** @return [ajint] length of alignment
** @@
******************************************************************************/

ajint embGotohCellBacktrace(AjPGotohCell **ajpGotohCellGotohScores,
			    AjPSeq ajpSeqDown,
			    AjPSeq ajpSeqAcross,
			    AjPList ajpListGotohCellsMaxScoringTrace)
{
  AjPGotohCell ajpGotohCellBackTrace;
  ajint ajIntCurrentRow;
  ajint ajIntCurrentColumn;
  ajint ajIntNextRow;
  ajint ajIntNextColumn;
  ajint ajIntAlignmentLen;

  AJNEW0(ajpGotohCellBackTrace);

  /* start search for starting AjOGotohCell from bottom right hand corner of sum matrix */
  ajIntCurrentRow = ajSeqLen(ajpSeqDown);
  ajIntCurrentColumn = ajSeqLen(ajpSeqAcross);

  /* DDDDEBUGGING */
  if(DEBUG_LEVEL)
    {
      ajFmtPrint("\nSTARTING VALUE OF iCurrentRow (iRowMax - 1):\t%d\n" , ajIntCurrentRow);
      ajFmtPrint("\nSTARTING VALUE OF iCurrentColumn (iColumnMax - 1):\t%d\n\n" , ajIntCurrentColumn);
    }

  ajIntNextRow = 0;
  ajIntNextColumn = 0;
  ajIntAlignmentLen = 0;

  /* continue back-trace until you're in the top left-hand corner */

  do
    {
	if(DEBUG_LEVEL > 1)
	{
	    ajFmtPrint("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	    ajFmtPrint("\najIntCurrentRow:\t%d\t" , ajIntCurrentRow);
	    ajFmtPrint("ajIntCurrentColumn:\t%d\n" , ajIntCurrentColumn);
	    ajFmtPrint("i pointer:\t%d",
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntRowPointer);
	    ajFmtPrint("\tj pointer:\t%d\n",
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntColumnPointer);
	    ajFmtPrint("\nfloatSubScore:\t%f\n" ,
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn] ->fSubScore);
	    ajFmtPrint("ajIntAlignmentLen:\t%d\n",
		       ajIntAlignmentLen);
	    ajFmtPrint("\tajBoolIsIndel:\t%B\n",
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajBoolIsIndel);
	    ajFmtPrint("cDownResidue: %c\t",
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->cDownResidue);
	    ajFmtPrint("cAcrossResidue: %c\n",
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->cAcrossResidue);
	    ajFmtPrint("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
	
	ajpGotohCellBackTrace->ajIntRowPointer = 
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntRowPointer;
	ajpGotohCellBackTrace->ajIntColumnPointer =
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntColumnPointer;
	ajpGotohCellBackTrace->fSubScore =
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn] ->fSubScore;
	ajpGotohCellBackTrace->ajBoolIsIndel =
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajBoolIsIndel;
	ajpGotohCellBackTrace->cDownResidue =
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->cDownResidue;
	ajpGotohCellBackTrace->cAcrossResidue =
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->cAcrossResidue;
	
	/* DDDDEBUGGING */
	if(DEBUG_LEVEL > 1)
	{
	    ajFmtPrint("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	    ajFmtPrint("\n NEXT CURRENT ROW:\t%d\t",
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntRowPointer);
	    ajFmtPrint(" NEXT CURRENT COLUMN:\t%d\n",
		       ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntColumnPointer);
	    ajFmtPrint("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
	
	ajIntNextRow = 
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntRowPointer;
	ajIntNextColumn =
	    ajpGotohCellGotohScores[ajIntCurrentRow][ajIntCurrentColumn]->ajIntColumnPointer;	
	if (ajIntNextRow == ajIntCurrentRow)
	{
	    ajpGotohCellBackTrace->cDownResidue = '-';
	}
	if (ajIntNextColumn == ajIntCurrentColumn)
	{
	    ajpGotohCellBackTrace->cAcrossResidue = '-';
	}
	
	ajIntAlignmentLen++;      

	/* put ajpGotohCell onto the stack */
	ajListPush(ajpListGotohCellsMaxScoringTrace, (void *)(ajpGotohCellBackTrace));
	
	ajIntCurrentRow = ajIntNextRow;
	ajIntCurrentColumn = ajIntNextColumn;
	
	/* get co-ordinates of next cell */
	ajIntCurrentRow = ajpGotohCellBackTrace->ajIntRowPointer;
	ajIntCurrentColumn = ajpGotohCellBackTrace->ajIntColumnPointer;
	
	AJNEW0(ajpGotohCellBackTrace);
	
	/* DDDDEBUGGING */
	if(DEBUG_LEVEL > 1)
	{
	    ajFmtPrint("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	    ajFmtPrint("\niCurrentRow:\t%d\t" , ajIntCurrentRow);
	    ajFmtPrint("iCurrentColumn:\t%d\n" , ajIntCurrentColumn);
	    ajFmtPrint("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	    
	    ajFmtPrint("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	    ajFmtPrint("\n Next Current Row:\t%d\t" , ajIntCurrentRow);
	    ajFmtPrint(" Next Current Column:\t%d\n" , ajIntCurrentColumn);
	    ajFmtPrint("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	    
	    ajFmtPrint("\n######################################\n\n");
	}
    }
  while(ajIntCurrentRow > 0 || ajIntCurrentColumn > 0);
  
  return ajIntAlignmentLen;    
}



/* @func embGotohPairScore ***************************************************
**
** scores the residues of two protein sequences against a scoring matrix
**
** @param [r] ajpMatrixFscoring [AjPMatrixf] stack of AjGotohCells
** @param [r] ajpSeqDown [AjPSeq] first sequence
** @param [r] ajpSeqAcross [AjPSeq] second sequence
** @param [r] fExtensionPenalty [float] alignment extension penalty
**
** @return [AjPFloat2d] 2D matrix of pair scores
** @@
******************************************************************************/


AjPFloat2d embGotohPairScore(AjPMatrixf ajpMatrixFscoring,
			     AjPSeq ajpSeqDown,
			     AjPSeq ajpSeqAcross,
			     float fExtensionPenalty)
{
  ajint ajIntDownSeqLen;
  ajint ajIntAcrossSeqLen;
  ajint ajIntRow;
  ajint ajIntColumn;

  /* strings containing the numbers corresponding to the residues */
  AjPStr ajpStrDownNumerical = NULL; 
  AjPStr ajpStrAcrossNumerical = NULL;

  AjPStr *pAjpStrDownNumerical = &ajpStrDownNumerical; 
  AjPStr *pAjpStrAcrossNumerical = &ajpStrAcrossNumerical;

  AjPFloat2d ajpFloat2dPairScores = NULL;
  float** floatArray2dScoring = NULL;

  /* convert alphabetical sequences to numerical sequences */
  ajMatrixfSeqNum(ajpMatrixFscoring, ajpSeqDown, pAjpStrDownNumerical);
  ajMatrixfSeqNum(ajpMatrixFscoring, ajpSeqAcross, pAjpStrAcrossNumerical);

  /* convert the input float AjpMatrix to a 2D array of scores */ 
  floatArray2dScoring = ajMatrixfArray(ajpMatrixFscoring);

  /* the dimensions of the AjpMatrix are the lengths of the two strings */
  ajIntDownSeqLen = ajStrLen(ajpStrDownNumerical);
  ajIntAcrossSeqLen = ajStrLen(ajpStrAcrossNumerical);

  /* we're going to start in the top left hand corner of AjPMatrix */
  ajIntRow = 0;
  ajIntColumn = 0;

  /* OBTAIN PROPER SCORES FOR ALL CELLS */

  /* first let's have an array for the scores */
  if (ajIntDownSeqLen > ajIntAcrossSeqLen)
    {
	ajpFloat2dPairScores = ajFloat2dNewL(ajIntDownSeqLen + cAjIntOffset);
    }
  else
    {
	ajpFloat2dPairScores = ajFloat2dNewL(ajIntAcrossSeqLen + cAjIntOffset);
    }

  /* deal with boundary conditions      */
  /* first set top left cell to zero... */
  ajFloat2dPut(&ajpFloat2dPairScores , ajIntRow , ajIntColumn , 0.0);

  /* zero counter */
  ajIntColumn = 0;
  /* ...then set leftmost column to multiples of extension penalty... */
  for (ajIntRow = 0;ajIntRow <= (ajIntDownSeqLen + cAjIntOffset);ajIntRow++)
    {
      ajFloat2dPut(&ajpFloat2dPairScores, ajIntRow, ajIntColumn,
		   (float)ajIntRow*fExtensionPenalty);
    }

  /* zero counter */
  ajIntRow = 0;

  /* ...then set topmost row to multiples of extension penalty */
  for (ajIntColumn = 0;
       ajIntColumn <= (ajIntAcrossSeqLen + cAjIntOffset);
       ajIntColumn++)
    {
      ajFloat2dPut(&ajpFloat2dPairScores,
		   ajIntRow,
		   ajIntColumn,
		   (float)ajIntColumn*fExtensionPenalty);
    }

  /* score two numerical sequences */
  for(ajIntRow = cAjIntOffset;
      ajIntRow <= (ajIntDownSeqLen + cAjIntOffset);
      ajIntRow++)
    {
      for(ajIntColumn = cAjIntOffset;
	  ajIntColumn <= ( ajIntAcrossSeqLen + cAjIntOffset );
	  ajIntColumn++)
	{
	  ajFloat2dPut(&ajpFloat2dPairScores,
		       ajIntRow,
		       ajIntColumn,
		       (float)(floatArray2dScoring
			       [(ajint)ajStrChar(ajpStrDownNumerical,
						 (ajIntRow - cAjIntOffset))]
			       [(ajint)ajStrChar(ajpStrAcrossNumerical,
						 (ajIntColumn - cAjIntOffset))]));
	}
    }

  return ajpFloat2dPairScores;
}




/* @func embGotohReadOffBacktrace ********************************************
**
** unloads the elements of a stack of backtraced AjOGotohCells and reads the
**  corresponding residues from the aligned sequences
**
** @param [r] ajpListGotohCellsMaxScoringTrace [AjPList] stack of AjGotohCells
** @param [r] ajpSeqDown [AjPSeq] first sequence
** @param [r] ajpSeqAcross [AjPSeq] second sequence
**
** @return [ajint] length of alignment trace
** @@
******************************************************************************/

ajint embGotohReadOffBacktrace(AjPList ajpListGotohCellsMaxScoringTrace,
			       AjPSeq ajpSeqDown,
			       AjPSeq ajpSeqAcross)
{
  char cBufferCurrentDown = '£';
  char cBufferCurrentAcross = '$';
  ajint ajIntAlignment;
  ajint ajIntCurrentRow;
  ajint ajIntCurrentColumn;
  AjIList ajListIterBacktrace = NULL;
  AjPGotohCell ajpGotohCellCurrentInBackTrace;
  AjBool ajbIsDownGap;
  AjBool ajbIsAcrossGap;
  AjPStr strDownTrace = NULL;
  AjPStr strAcrossTrace = NULL;

  ajListIterBacktrace = ajListIter(ajpListGotohCellsMaxScoringTrace);

  ajIntAlignment = 0;

  /* get embGotohCells off the stack (and print out the values) */ 

  ajIntCurrentRow = 0;
  ajIntCurrentColumn = 0;

  ajbIsDownGap = AJFALSE;
  ajbIsAcrossGap = AJFALSE;

  do
    {
      ajpGotohCellCurrentInBackTrace = ajListIterNext(ajListIterBacktrace);

      cBufferCurrentDown = ajpGotohCellCurrentInBackTrace->cDownResidue;
      cBufferCurrentAcross = ajpGotohCellCurrentInBackTrace->cAcrossResidue;

      ajStrAppK(&strDownTrace, cBufferCurrentDown);
      ajStrAppK(&strAcrossTrace, cBufferCurrentAcross);
      
      /* DDDDEBUG */
      if(DEBUG_LEVEL)
	  ajFmtPrint("\nthis template residue: %c , this query residue: %c\n",
		     cBufferCurrentDown , cBufferCurrentAcross); 

    }
  while(ajListIterMore(ajListIterBacktrace));

  /* DDDDEBUG */
      if(DEBUG_LEVEL)
	  ajFmtPrint("\n FULL TEMPLATE STRING: %S , FULL QUERY STRING: %S\n",
		     strDownTrace , strAcrossTrace); 

  ajSeqReplace(ajpSeqDown, strDownTrace); 
  ajSeqReplace(ajpSeqAcross, strAcrossTrace); 

  return 0;
}
