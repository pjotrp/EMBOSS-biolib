/* @source substitute application
**
** takes a directory of pairs of aligned sequences and substitutes match
**  residues from the second sequence of each pair into the first
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.7 $
** @modified $Date: 2004/07/23 15:47:31 $
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
	enumDebugLevel       = 0,
	enumQuerySeqIndex    = 0,
	enumTemplateSeqIndex = 1,
	enumPair             = 2
    };

#include "emboss.h"
#include <math.h>

/* @prog substitute **********************************************************
** 
** substitutes match residues from second of two aligned sequences given into
**  corresponding positions in first of them, writing result as new sequence
**
******************************************************************************/

int main(int argc , char **argv)
{
    /* sequence objects and strings */
    const AjPSeq ajpSeqTraceAcross = NULL; /* template---structure known   */
    const AjPSeq ajpSeqTraceDown   = NULL; /* query---no structure         */
    AjPSeq ajpSeqSingle            = NULL; /* rewritable template seq      */
    AjPSeqset ajpSeqsetPairCurrent = NULL;
    char *pcTraceDown              = NULL;
    char *pcTraceAcross            = NULL;
    char *pcSingleSeq              = NULL;

    /* sequence characters */
    char cTempTraceDown   = '\0';
    char cTempTraceAcross = '\0';
    char cTempSingleSeq   = '\0';

    /* sequence lengths */
    ajint ajIntSingleSeqCount;
    ajint ajIntTraceCount;
    ajint ajIntAcrossTraceLen;
    
    /* file details for aligned sequence pairs */
    AjPStr ajpStrPairFileName            = NULL; /* input file name         */
    AjPStr ajpStrSingleFileName          = NULL; /* output file name        */
    AjPFile ajpFileSingleCurrent         = NULL; /* output file             */
    AjPList ajpListPairFiles             = NULL; /* list of pair files      */

    /* alignment parameters */
    AjPDir ajpDirPairs                   = NULL; /* dir of seq pair files   */
    AjPStr ajpStrInFileSuffix            = NULL; /* extension pair files    */
    AjPStr ajpStrOutFileSuffix           = NULL; /* extension sub'd files   */
    AjPStr ajpStrSingleSeq               = NULL; /* substituted seq string  */
    const AjPStr ajpStrSeqoutFormat      = NULL; /* format of sub'd seqfile */
    AjPSeqout ajpSeqoutSingle            = NULL; /* to write sub'd seq to   */

    embInit("substitute", argc, argv);

    /* get alignment file directory from ACD */
    ajpListPairFiles   = ajAcdGetDirlist("alignedpairsdirlist");
    ajpDirPairs        = ajAcdGetDirectory("pairsdir");
    ajpStrInFileSuffix = ajDirExt(ajpDirPairs);

    ajpStrOutFileSuffix = ajStrNewC("substituted");
    ajpStrSeqoutFormat  = ajStrNewC("fasta");

    /* loop over pair files in alignment directory */
    while(ajListPop(ajpListPairFiles, (void **)&ajpStrPairFileName))
    {

	/* DDDDEBUG: PRINT CURRENT FILENAME FROM ALIGNMENT DIRECTORY */
	ajFmtPrint("\n%S", ajpStrPairFileName);

	/* reserve memory for new seqset */
	ajpSeqsetPairCurrent = ajSeqsetNew();

	/* get sequence objects from current alignment file */
	ajSeqsetGetFromUsa(ajpStrPairFileName,
			   &ajpSeqsetPairCurrent);

	/* if there aren't two sequences in alignment, bail */ 
	if(ajpSeqsetPairCurrent->Size != enumPair)
	    ajFatal("The file %S does not contain exactly one pair of sequence traces!\n",ajpStrPairFileName);

	/* read seqset into seqs */
	ajpSeqTraceDown = ajSeqsetGetSeq(ajpSeqsetPairCurrent,
					 enumQuerySeqIndex);
	ajpSeqTraceAcross = ajSeqsetGetSeq(ajpSeqsetPairCurrent,
					   enumTemplateSeqIndex);
	/* get trace length */
	ajIntAcrossTraceLen = ajSeqLen(ajpSeqTraceAcross);

	/* substitute all matches into a new version of the template sequence */
	ajpSeqSingle = ajSeqNewS(ajpSeqTraceDown);
	pcSingleSeq  = ajSeqCharCopy(ajpSeqTraceAcross);

	/* copy the original traces into character strings */
	pcTraceDown   = ajSeqCharCopy(ajpSeqTraceDown);
	pcTraceAcross = ajSeqCharCopy(ajpSeqTraceAcross);

	ajIntSingleSeqCount = 0;

	/* loop over entire length of template trace */
	for(ajIntTraceCount = 0;
	    ajIntTraceCount < ajIntAcrossTraceLen;
	    ajIntTraceCount++)
	{
	    /* DDDDEBUG */
	    ajFmtPrint("BEFORE trace count: %d\t", ajIntTraceCount);
	    ajFmtPrint("BEFORE sequence count: %d\t\n", ajIntSingleSeqCount);
	    ajFmtPrint("BEFORE down (query): %c\t", cTempTraceDown);
	    ajFmtPrint("BEFORE across (template): %c\t", cTempTraceAcross);
	    ajFmtPrint("BEFORE substitute: %c\t\n\n", cTempSingleSeq);
	    
	    /* get current character in each trace string */
	    cTempTraceDown   = pcTraceDown[ajIntTraceCount];
	    cTempTraceAcross = pcTraceAcross[ajIntTraceCount];
	    
	    /* discard gaps in template trace */
	    if(isalpha(cTempTraceAcross))
	    {
		/* default to keeping current residue */
		cTempSingleSeq = pcTraceAcross[ajIntTraceCount];
		/* discard gaps in query trace */

		if(isalpha(cTempTraceDown))
		{
		    /* but substitute matches into output sequence */
		    cTempSingleSeq = pcTraceDown[ajIntTraceCount];
		}
		/* read either original or matched character into output */
		pcSingleSeq[ajIntSingleSeqCount] = cTempSingleSeq;
		ajIntSingleSeqCount++;
	    }

	    /* terminate substituted sequence string */
	    pcSingleSeq[ajIntSingleSeqCount] = '\0';

	    /* DDDDEBUG */
	    ajFmtPrint("AFTER trace count: %d\t", ajIntTraceCount);
	    ajFmtPrint("AFTER sequence count: %d\t\n", ajIntSingleSeqCount);
	    ajFmtPrint("AFTER down (query): %c\t", cTempTraceDown);
	    ajFmtPrint("AFTER across (template): %c\t", cTempTraceAcross);
	    ajFmtPrint("AFTER substitute: %c\t\n\n", cTempSingleSeq);
	}

	ajpStrSingleSeq = ajStrNewC(pcSingleSeq);
	
	ajFmtPrint("\n%S\n", ajpStrSingleSeq);
	
	ajSeqAssSeq(ajpSeqSingle, (const AjPStr) ajpStrSingleSeq);

	ajStrAss(&ajpStrSingleFileName, ajpStrPairFileName);
	
	/* write out "aligned" sequences  */
	ajStrSubstitute(&ajpStrSingleFileName,
			ajpStrInFileSuffix,
			ajpStrOutFileSuffix);

	ajpFileSingleCurrent = ajFileNewOut(ajpStrSingleFileName);    
	ajpSeqoutSingle = ajSeqoutNewF(ajpFileSingleCurrent);
	ajSeqOutSetFormat(ajpSeqoutSingle, ajpStrSeqoutFormat);
	ajSeqWrite(ajpSeqoutSingle, ajpSeqSingle);
	
	/* free output sequence */
	ajSeqoutDel(&ajpSeqoutSingle);

	/* free current pair set */
	ajSeqsetDel(&ajpSeqsetPairCurrent);

	/* free pair file name */
	ajStrDel(&ajpStrSingleFileName);

	/* free current substituted sequence object */
	ajSeqDel(&ajpSeqSingle);

	/* free current output file */
	ajFileClose(&ajpFileSingleCurrent);
    }    

    /* clear up the pair file objects */
    while(ajListPop(ajpListPairFiles, (void **)&ajpStrPairFileName))
	AJFREE(ajpStrPairFileName);

    ajDirDel(&ajpDirPairs);

    /* clean up strings */
    ajStrDel(&ajpStrPairFileName);
    ajStrDel(&ajpStrInFileSuffix);

    ajListFree(&ajpListPairFiles);


    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
