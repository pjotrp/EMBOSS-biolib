/* @source substitute application
**
** takes a pair of aligned sequence traces and substitutes match residues
**  from the second sequence of each pair into the first, removing any gaps
**  
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.9 $
** @modified $Date: 2004/09/28 11:53:19 $
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
	enumDebugLevel       = 2,
	enumTemplateSeqIndex = 0,
	enumQuerySeqIndex    = 1,
	enumPair             = 2
    };

#include "emboss.h"
#include <math.h>

/* @prog substitute **********************************************************
** 
** substitutes match residues from second of two given traces into
**   corresponding positions in first given trace, writing result as new,
**   ungapped sequence
**
******************************************************************************/

int main(int argc , char **argv)
{
    /* sequence objects and strings */
    const AjPSeq ajpSeqTraceAcross = NULL; /* template sequence---has structure */
    const AjPSeq ajpSeqTraceDown   = NULL; /* query sequence---no structure     */
    AjPSeq ajpSeqSubstituted       = NULL; /* rewritable template seq           */
    AjPSeqset ajpSeqsetPair        = NULL; /* current pair of sequences         */
    char *pcTraceAcross            = NULL; /* C string of template sequence     */
    char *pcTraceDown              = NULL; /* C string of query sequence        */
    char *pcSubstitutedSeq         = NULL;
    AjPSeqout ajpSeqoutSubstituted = NULL; /* substituted output seq object     */

    /* sequence characters */
    char cTempTraceDown      = '\0';
    char cTempTraceAcross    = '\0';
    char cTempSubstitutedSeq = '\0';

    /* sequence lengths */
    ajint ajIntSubstitutedSeqCount;
    ajint ajIntTraceCount;
    ajint ajIntAcrossTraceLen;

    /* alignment parameters */
    AjPStr ajpStrSubstitutedSeq     = NULL; /* substituted seq string           */

    embInit("substitute", argc, argv);

    /* get alignment from ACD */
    ajpSeqsetPair = ajAcdGetSeqset("alignedpair");
    ajpSeqoutSubstituted  = ajAcdGetSeqout("substitutedseq");
    
    /* if there aren't two sequences in alignment, bail */ 
    if(ajpSeqsetPair->Size != enumPair)
	ajFatal("\nThe file %S does not contain exactly one pair of sequence traces!\n",
		ajpSeqsetPair->Filename);

    /* read seqset into seqs */
    ajpSeqTraceAcross = ajSeqsetGetSeq(ajpSeqsetPair,
				       enumTemplateSeqIndex);
    ajpSeqTraceDown = ajSeqsetGetSeq(ajpSeqsetPair,
				     enumQuerySeqIndex);

    /* substitute all matches into a new version of the template sequence */
    ajpSeqSubstituted = ajSeqNewS(ajpSeqTraceDown);
    pcSubstitutedSeq  = ajSeqCharCopy(ajpSeqTraceAcross);

    /* copy the original traces into character strings */
    pcTraceDown   = ajSeqCharCopy(ajpSeqTraceDown);
    pcTraceAcross = ajSeqCharCopy(ajpSeqTraceAcross);

    ajIntSubstitutedSeqCount = 0;

    /* get trace length */
    ajIntAcrossTraceLen = ajSeqLen(ajpSeqTraceAcross);

    /* loop over entire length of template trace */
    for(ajIntTraceCount = 0;
	ajIntTraceCount < ajIntAcrossTraceLen;
	ajIntTraceCount++)
    {    
	/* DDDDEBUG */
	if(enumDebugLevel > 1)
	{
	    ajFmtPrint("BEFORE trace count: %d\t", ajIntTraceCount);
	    ajFmtPrint("BEFORE sequence count: %d\t\n", ajIntSubstitutedSeqCount);
	    ajFmtPrint("BEFORE down (query): %c\t", cTempTraceDown);
	    ajFmtPrint("BEFORE across (template): %c\t", cTempTraceAcross);
	    ajFmtPrint("BEFORE substitute: %c\t\n\n", cTempSubstitutedSeq);
	}
	
	/* get current character in each trace string */
	cTempTraceDown   = pcTraceDown[ajIntTraceCount];
	cTempTraceAcross = pcTraceAcross[ajIntTraceCount];
	    
	/* discard gaps in template trace */
	if(isalpha(cTempTraceAcross))
	{
	    /* default to keeping current residue */
	    cTempSubstitutedSeq = pcTraceAcross[ajIntTraceCount];
	    /* discard gaps in query trace */

	    if(isalpha(cTempTraceDown))
	    {
		/* but substitute matches into output sequence */
		cTempSubstitutedSeq = pcTraceDown[ajIntTraceCount];
	    }
	    /* read either original or matched character into output */
	    pcSubstitutedSeq[ajIntSubstitutedSeqCount] = cTempSubstitutedSeq;
	    ajIntSubstitutedSeqCount++;
	}

	/* terminate substituted sequence string */
	pcSubstitutedSeq[ajIntSubstitutedSeqCount] = '\0';

	/* DDDDEBUG */
	if(enumDebugLevel > 1)
	{
	    ajFmtPrint("AFTER trace count: %d\t", ajIntTraceCount);
	    ajFmtPrint("AFTER sequence count: %d\t\n", ajIntSubstitutedSeqCount);
	    ajFmtPrint("AFTER down (query): %c\t", cTempTraceDown);
	    ajFmtPrint("AFTER across (template): %c\t", cTempTraceAcross);
	    ajFmtPrint("AFTER substitute: %c\t\n\n", cTempSubstitutedSeq);
	}
    }

    /* DDDDEBUG */
    if(enumDebugLevel)
	ajFmtPrint("!!!!Trace Length: %d\t!!!!\n", ajIntAcrossTraceLen);

    ajpStrSubstitutedSeq = ajStrNewC(pcSubstitutedSeq);

    /* DDDDEBUG */
    if(enumDebugLevel)
	ajFmtPrint("\n%S\n", ajpStrSubstitutedSeq);

    ajSeqAssSeq(ajpSeqSubstituted, (const AjPStr)ajpStrSubstitutedSeq);
	
    /* write out "aligned" sequences  */
    ajSeqWrite(ajpSeqoutSubstituted, ajpSeqSubstituted);
	
    /* free objects */
    ajSeqoutDel(&ajpSeqoutSubstituted);
    ajSeqsetDel(&ajpSeqsetPair);
    ajSeqDel(&ajpSeqSubstituted);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
