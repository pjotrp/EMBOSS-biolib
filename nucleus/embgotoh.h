#ifdef __cplusplus
extern "C"
{
#endif




#ifndef embgotoh_h
#define embgotoh_h

/* @data AjPGotohCell *********************************************************
**
** NUCLEUS Gotoh path matrix cell for pairwise dynamic programming
**
** Holds integer co-ordinates of preceding cell in backtrace, cumulative trace
**  score, indexing down and across residue characters and flag for indel status
**
** AjPGotohCell is implemented as a pointer to a C data structure.
**
** @alias AjSGotohCell
** @alias AjOGotohCell
**
** @new ajGotohNew default constructor
** @new ajGotohCreate constructor initializing values of attributes
**
** @delete ajGotohDel default destructor
**
** @ass ajGotohNull set all components to zero
**
** @use embGotohCellCalculateSumScore fill in alignment array of ajGotohCells
** @use embGotohCellBacktrace find highest scoring path through array of
** ajGotohCells
**
** @attr ajIntRow [ajint] Row number in backtrace table
** @attr ajIntColumn [ajint] Column number in backtrace table
** @attr fSubScore [float] Intermediate cumulative alignment score
** @attr cDownResidue [char] Residue in template indexed by row
** @attr cAcrossResidue [char] Residue in query indexed by column
** @attr ajBoolIsIndel [AjBool] Does cell correspond to an indel
** @@
******************************************************************************/

typedef struct AjGotohCell
{
    ajint ajIntRow;
    ajint ajIntColumn;
    float fSubScore;
    char cDownResidue;
    char cAcrossResidue;
    AjBool ajBoolIsIndel;
} AjOGotohCell;
#define AjPGotohCell AjOGotohCell*




/* ========================================================================= */
/* =================== All functions in alphabetical order ================= */
/* ========================================================================= */

/* ajgotoh.h() $Date: 2004/02/13 11:08:46 $                        DJC Oct03 */

AjPGotohCell **embGotohCellGetArray(ajint ajIntDownSeqLen,
				    ajint ajIntCrossSeqLen);

AjPFloat2d     embGotohPairScore(AjPMatrixf ajpMatrixFscoring,
				 AjPSeq ajpSeqDown,
				 AjPSeq ajpSeqAcross,
				 float fExtensionPenalty);

void           embGotohCellCalculateSumScore(AjPFloat2d ajpFloat2dPairScores,
					     AjPSeq ajpSeqDown,
					     AjPSeq ajpSeqAcross,
					     AjPGotohCell **ajpGotohCellGotohScores,
					     float fGapPenalty,
					     float fExtensionPenalty);

ajint          embGotohCellBacktrace(AjPGotohCell **ajpGotohCellGotohScores,
				     AjPSeq ajpSeqDown,
				     AjPSeq ajpSeqAcross,
				     AjPList ajpListGotohCellsMaxScoringTrace);

ajint          embGotohReadOffBacktrace(AjPList ajpListGotohCellsMaxScoringTrace,
					AjPSeq ajpSeqDown,
					AjPSeq ajpSeqAcross);

#endif




#ifdef __cplusplus
}
#endif
