/* @source scorer application
**
** Takes two sequences, each obtained by substituting corresponding residues
**  from a query sequence (of unknown structure) into a templates sequence (of
**  known structure).  The first sequence is a gold-standard substituted sequence.
**  The second is substituted according the alignment method being evaluated.
**
**  Outputs plain text file containing basic measures of accuracy, including
**  Hamming score: each residue which matches with the gold standard scores one
**  and each mismatch scores zero.
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.4 $
** @modified $Date: 2004/10/14 19:08:19 $
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

static ajint match_count(const AjPSeq ajpSeqGold,
			 const AjPSeq ajpSeqScored);


enum constant
    {
	enumDebugLevel =  1,
	enumZeroCount  =  0,
    };


/* @prog scorer ************************************************************
** 
** compare substituted alignment with gold standard and score
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* counts */
    ajint ajIntTempMatchScore;
    ajint ajIntSeqLen;

    /* substituted sequences */
    AjPSeq ajpSeqGold               = NULL; /* ideal substituted seq       */
    AjPSeq ajpSeqTest               = NULL; /* test substituted seq        */
    AjPSeq ajpSeqScored             = NULL; /* score sequence              */
    char *pcGoldSeq                 = NULL; /* ideal seq string            */
    char *pcTestSeq                 = NULL; /* test seq string             */
    
    embInit("scorer", argc, argv);

    /* get gold standard aligned and substituted sequence*/
    ajpSeqGold = ajAcdGetSeq("goldsubstituted");
    /* get test aligned and substituted sequence*/
    ajpSeqTest = ajAcdGetSeq("testsubstituted");

    pcGoldSeq = ajSeqCharCopy(ajpSeqGold);
    pcTestSeq = ajSeqCharCopy(ajpSeqTest);

    /* DDDDEBUG: PRINT OUT GOLD STANDARD SEQUENCE */
    if( enumDebugLevel > 1)
    {
	ajFmtPrint("\n%s\n", pcGoldSeq);
	ajFmtPrint("\n%s\n", pcTestSeq);
    }
    
    /*
     * compare current alignment with gold standard and write result
     * to file with above current output file name
     */
    ajIntTempMatchScore = 0;
    ajIntSeqLen = ajSeqLen(ajpSeqGold);
    ajIntTempMatchScore = match_count(ajpSeqGold, ajpSeqTest);
    if( enumDebugLevel )
    {
	ajFmtPrint("\n%S:\t%d", ajpSeqGold->Name,
		   (ajIntTempMatchScore/ajIntSeqLen));
    }    

    /* tidy up */
    ajSeqDel(&ajpSeqGold);
    ajSeqDel(&ajpSeqTest);
    ajExit();

    return 0;
}



ajint match_count(const AjPSeq ajpSeqGold,
		  const AjPSeq ajpSeqTest)
{
    ajint ajIntMatchCount;    
    AjPStr ajpStrGoldSeq       = NULL;
    AjPStr ajpStrScoredSeq     = NULL;
    AjIStr ajpStrIterGoldSeq   = NULL;
    AjIStr ajpStrIterScoredSeq = NULL;
    char cCurrentGoldRes   = '\0';
    char cCurrentScoredRes = '\0';
    AjBool ajBoolMatch = AJFALSE;

    /* get string from sequence */
    ajStrAssS(&ajpStrGoldSeq, ajSeqStr(ajpSeqGold));
    ajStrAssS(&ajpStrScoredSeq, ajSeqStr(ajpSeqTest));

    /* iterate through string */
    ajpStrIterGoldSeq = ajStrIter(ajpStrGoldSeq);
    ajpStrIterScoredSeq = ajStrIter(ajpStrScoredSeq);

    ajIntMatchCount = 0;

    while(!ajStrIterDone(ajpStrIterGoldSeq))
    {
	cCurrentGoldRes   = ajStrIterGetK(ajpStrIterGoldSeq);
	cCurrentScoredRes = ajStrIterGetK(ajpStrIterScoredSeq);
	ajStrIterNext(ajpStrIterGoldSeq);
	ajStrIterNext(ajpStrIterScoredSeq);
	if(cCurrentGoldRes == cCurrentScoredRes)
	{
	    ajBoolMatch = AJTRUE;
	    ajIntMatchCount++;
	}
	else
	{
	    ajBoolMatch = AJFALSE;   
	}
	if(enumDebugLevel > 1)
	{
	    ajFmtPrint("%c\t", cCurrentGoldRes);
	    ajFmtPrint("%c\t", cCurrentScoredRes);
	    ajFmtPrint("%B\n", ajBoolMatch);
	}
    }

    /* destroy all objects */
    ajStrDel(&ajpStrGoldSeq);
    ajStrDel(&ajpStrScoredSeq);
    ajStrIterFree(&ajpStrIterGoldSeq);
    ajStrIterFree(&ajpStrIterScoredSeq);    

    return ajIntMatchCount;
}




