/* @source substitute application
**
** takes a directory of pairs of aligned sequences and substitutes match
**  residues from the second sequence of each pair into the first
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.4 $
** @modified $Date: 2004/07/19 13:14:21 $
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
    const AjPSeq ajpSeqTraceDown   = NULL; /* query---no structure         */
    const AjPSeq ajpSeqTraceAcross = NULL; /* template---structure known   */
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
    ajint ajIntAlignmentLen;
    
    /* file details for aligned sequence pairs */
    AjPStr ajpStrPairFileName            = NULL; /* input file name        */
    AjPStr ajpStrSingleFileName          = NULL; /* output file name       */
    AjPStr ajpStrInfileSuffix            = NULL;
    AjPFile ajpFilePairCurrent           = NULL; /* input file             */
    AjPFile ajpFileSingleCurrent         = NULL; /* output file            */
    AjPList ajpListPairFiles             = NULL; /* list of pair files     */
    AjPSeqset ajpSeqsetUncombinedPair    = NULL; /* obj holding both seqs  */

    /* alignment parameters */
    AjPDir ajpDirPairs                   = NULL; /* dir of seq pair files  */
    AjPStr ajpStrPairDir                 = NULL; /* name of that dir       */
    AjPStr ajpStrPairFileCurrent         = NULL; /* fullname combined seq  */
    AjPStr ajpStrPairFileCurrentBaseName = NULL; /* basename combined seq  */
    AjPStr ajpStrPathToOutfile           = NULL; /* dir combined seqs      */
    AjPStr ajpStrOutfileName             = NULL; /* fullname combined seqs */
    AjPStr ajpStrOutfileSuffix           = NULL; /* extension sub'd files  */
    AjPStr ajpStrPairFormat              = NULL; /* o/p format sub'd files */
    AjPStr ajpStrSingleSeq               = NULL; /* substituted seq string */
    AjPStr ajpStrSeqoutFormat            = NULL; /* format of sub'd seqfile*/
    AjPSeqout ajpSeqoutSingle            = NULL; /* to write sub'd seq to  */

    ajStrAssC(&ajpStrOutfileSuffix, ".substituted");

    embInit("substitute", argc, argv);

    /* get alignment file directory from ACD */
    ajpListPairFiles = ajAcdGetDirlist("alignedpairsdir");

    /* DDDDEBUG: FIRST DO THIS FOR ONE SEQUENCE PAIR ONLY */
    ajStrAssC(&ajpStrInfileSuffix, ".needle");
    ajStrAssC(&ajpStrPairDir,
	      "/users/damian/EMBOSS/emboss/emboss/emboss/contacttest/test_alignments/");

    /* make new directory object (second arg is file extension) */
    ajpDirPairs= ajDirNewS(ajpStrPairDir, ajpStrOutfileSuffix);

    ajStrAssC(&ajpStrPairFileCurrentBaseName,
	      "substitute_test.needle");

    ajStrAssC(&ajpStrSingleFileName,
	      "output_substituted_sequence.needle");

    ajpStrPairFileCurrent = ajStrNewS(ajpStrPairDir);
    
    ajStrApp(&ajpStrPairFileCurrent, ajpStrPairFileCurrentBaseName);
    
    ajpFilePairCurrent = ajFileNewIn(ajpStrPairFileCurrent);

    if(!ajpFilePairCurrent)
	ajDie("cannot open file %S",
	      ajpStrPairFileCurrentBaseName);

    /* reserve memory for new seqset */
    ajpSeqsetPairCurrent = ajSeqsetNew();
    
    /* get sequence objects from current alignment file */
    ajSeqsetGetFromUsa(ajpStrPairFileCurrent,
		       &ajpSeqsetPairCurrent);

    /* if there aren't two sequences in alignment, bail */ 
    if(ajpSeqsetPairCurrent->Size != enumPair)
	ajFatal("The file %S does not contain exactly one pair of sequence traces!\n",
		ajpStrPairFileCurrentBaseName);

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

    /* DDDDEBUG: JUST LOOP OVER ALIGNMENTS IN ALIGNMENT DIRECTORY */
    while(ajListPop(ajpListPairFiles, (void **)&ajpStrPairFileName))
    {
	ajpFilePairCurrent = ajFileNewIn(ajpStrPairFileName);

	if(!ajpFilePairCurrent)
	    ajDie("cannot open file %S",ajpStrPairFileName);

	/* DDDDEBUG: PRINT OUT FILENAMES IN ALIGNMENT DIRECTORY */
	ajFmtPrint("\n%S", ajpStrPairFileName);
    }
    
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

	/* DDDDEBUG */
	ajFmtPrint("AFTER trace count: %d\t", ajIntTraceCount);
	ajFmtPrint("AFTER sequence count: %d\t\n", ajIntSingleSeqCount);
	ajFmtPrint("AFTER down (query): %c\t", cTempTraceDown);
	ajFmtPrint("AFTER across (template): %c\t", cTempTraceAcross);
	ajFmtPrint("AFTER substitute: %c\t\n\n", cTempSingleSeq);
    }
    /* terminate substituted sequence string */
    ajIntTraceCount++;
    pcSingleSeq[ajIntSingleSeqCount] = '\0';

    ajpStrSingleSeq = ajStrNewC(pcSingleSeq);

    ajFmtPrint("\n%S\n", ajpStrSingleSeq);

    ajSeqAssSeq(ajpSeqSingle, (const AjPStr) ajpStrSingleSeq);

    /* write out "aligned" sequences  */
    ajpFileSingleCurrent = ajFileNewOut(ajpStrSingleFileName);    
    ajpSeqoutSingle = ajSeqoutNewF(ajpFileSingleCurrent);
    ajStrAssC(&ajpStrSeqoutFormat, "fasta");
    ajSeqOutSetFormat(ajpSeqoutSingle, ajpStrSeqoutFormat);
    ajSeqWrite(ajpSeqoutSingle, ajpSeqSingle);

    /* delete output sequence */
    ajSeqoutDel(&ajpSeqoutSingle);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
