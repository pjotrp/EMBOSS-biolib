/* @source substitute application
**
** takes a directory of pairs of aligned sequences and substitutes match
**  residues from the second sequence of each pair into the first
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.1 $
** @modified $Date: 2004/07/13 16:48:37 $
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

    /* sequence lengths */
    ajint ajIntSingleSeqCount;
    ajint ajIntTraceCount;
    ajint ajIntAcrossTraceLen;
    ajint ajIntAlignmentLen;
    
    /* file details for aligned sequence pairs */
    AjPStr ajpStrPairFileName            = NULL;
    AjPStr ajpStrInfileSuffix            = NULL;
    AjPFile ajpFilePairCurrent           = NULL;
    AjPList ajpListSeqPairFiles          = NULL; /* list of pair files     */
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
    AjPSeqout ajpSeqoutSingle            = NULL; /* to write sub'd seq to  */

    ajStrAssC(&ajpStrOutfileSuffix, ".substituted");

    embInit("substitute", argc, argv);

    /* DDDDEBUG: FIRST DO THIS FOR ONE SEQUENCE PAIR ONLY */
    ajStrAssC(&ajpStrInfileSuffix, ".needle");
    ajStrAssC(&ajpStrPairDir,
	      "/users/damian/EMBOSS/emboss/emboss/emboss/contacttest/test_alignments/");

    /* make new directory object (second arg is file extension) */
    ajpDirPairs= ajDirNewS(ajpStrPairDir, ajpStrOutfileSuffix);

    ajStrAssC(&ajpStrPairFileCurrentBaseName,
	      "1hlb__-2lhb__.needle");

    ajpStrPairFileCurrent = ajStrNewS(ajpStrPairDir);
    
    ajStrApp(&ajpStrPairFileCurrent, ajpStrPairFileCurrentBaseName);
    
    ajpFilePairCurrent = ajFileNewIn(ajpStrPairFileCurrent);

    if(!ajpFilePairCurrent)
	ajDie("cannot open file %S",
	      ajpStrPairFileCurrentBaseName);

    /* reserve memory for new seqset */
    ajpSeqsetPairCurrent = ajSeqsetNew();
    
    /* get sequence objects from current alignment file */
    ajSeqsetGetFromUsa(ajpStrPairFileName,
		       &ajpSeqsetPairCurrent);
    /* if there aren't two sequences in alignment, bail */ 
    if(ajpSeqsetPairCurrent->Size != enumPair)
	ajFatal("file does not contain pair of sequence traces!\n");

    /* read seqset into seqs */
    ajpSeqTraceAcross = ajSeqsetGetSeq(ajpSeqsetPairCurrent,
				       enumQuerySeqIndex);
    ajpSeqTraceDown = ajSeqsetGetSeq(ajpSeqsetPairCurrent,
				     enumTemplateSeqIndex);

    /* get trace length */
    ajIntAcrossTraceLen = ajSeqLen(ajpSeqTraceAcross);

    /* substitute all matches into a new version of the template sequence */
    ajpSeqSingle = ajSeqNewS(ajpSeqTraceAcross);
    pcTraceDown = ajSeqCharCopy(ajpSeqTraceDown);
    pcTraceAcross = ajSeqCharCopy(ajpSeqTraceAcross);

    ajIntSingleSeqCount = 0;
    
    for(ajIntTraceCount = 0;ajIntTraceCount < ajIntAcrossTraceLen; ajIntTraceCount++)
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
		pcSingleSeq[ajIntSingleSeqCount] = cTempTraceDown;
	    ajFmtPrint("%c\n", pcSingleSeq[ajIntSingleSeqCount]);
	}
    }

    /* write out "aligned" sequences  */
    ajSeqWrite(ajpSeqoutSingle, ajpSeqSingle);

    /* delete output sequence */
    ajSeqoutDel(&ajpSeqoutSingle);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
