/* @source substitute application
**
** takes a pair of aligned sequence traces and substitutes match residues
**  from the second sequence of each pair into the first, removing any gaps
**  
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.13 $
** @modified $Date: 2004/11/25 20:23:57 $
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

/*
 * note that the template sequence always comes first
 * in gold standard alignment pairs, but always comes
 * second in my programs
 */

enum constant
    {
	enumDebugLevel       = 2,
	enumTemplateSeqIndex = 0,
	enumQuerySeqIndex    = 1,
	enumPair             = 2
    };

/* name spacer and gap characters */
const char cHyphen = '-';
const char cDot = '.';

#include "emboss.h"
#include <math.h>

/* @prog substitute **********************************************************
** 
** substitutes match residues from second of two given traces into
**   corresponding positions in first given trace, writing result as new,
**   ungapped sequence
**
******************************************************************************/

static AjBool is_a_gap(char cTraceElement);


int main(int argc , char **argv)
{
    /* sequence objects and strings */
    const AjPSeq ajpSeqTraceDown   = NULL; /* query sequence---no structure     */
    const AjPSeq ajpSeqTraceAcross = NULL; /* template sequence---has structure */
    AjPStr ajpStrSeqFileName       = NULL; /* name of file containing input seq */
    AjPSeq ajpSeqSubstituted       = NULL; /* rewritable template seq           */
    AjPSeqset ajpSeqsetPair        = NULL; /* current pair of sequences         */
    char *pcTraceDown              = NULL; /* C string of query sequence        */
    char *pcTraceAcross            = NULL; /* C string of template sequence     */
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
    AjPStr ajpStrSubstitutedSeq = NULL; /* substituted seq string           */

    /* variables for constructing sequence name */
    AjPStr ajpStrTempQuerySeqName    = NULL;
    AjPStr ajpStrQuerySeqName        = NULL;
    AjIStr ajpStrIterQuerySeqName    = NULL;
    char cQuerySeqName               = '\0';
    AjBool ajBoolQueryNameRead       = AJFALSE;
    AjPStr ajpStrTempTemplateSeqName = NULL;
    AjPStr ajpStrTemplateSeqName     = NULL;
    AjIStr ajpStrIterTemplateSeqName = NULL;
    char cTemplateSeqName            = '\0';
    AjBool ajBoolTemplateNameRead    = AJFALSE;
    AjPStr ajpStrJoinOutfileName     = NULL;

    embInit("substitute", argc, argv);

    /* get alignment from ACD */
    ajpSeqsetPair = ajAcdGetSeqset("alignedpair");
    ajpSeqoutSubstituted  = ajAcdGetSeqout("substitutedseq");
    
    /* if there aren't two sequences in alignment, bail */ 
    if(ajpSeqsetPair->Size != enumPair)
	ajFatal("\nThe file %S does not contain exactly one pair of sequence traces!\n",
		ajpSeqsetPair->Filename);

    /* read seqset into seqs */
    ajpSeqTraceDown = ajSeqsetGetSeq(ajpSeqsetPair,
				     enumQuerySeqIndex);
    ajpSeqTraceAcross = ajSeqsetGetSeq(ajpSeqsetPair,
				       enumTemplateSeqIndex);

    /* get name from sequence properties */
    ajpStrTempQuerySeqName = ajStrNewS(ajpSeqTraceDown->Name);

    /* iterate over the characters in the name */
    ajpStrIterQuerySeqName = ajStrIter(ajpStrTempQuerySeqName);

    ajBoolQueryNameRead = AJFALSE;	    
    while(!ajStrIterDone(ajpStrIterQuerySeqName) &&
	  !ajBoolQueryNameRead)
    {
	/* read in current character */
	cQuerySeqName = ajStrIterGetK(ajpStrIterQuerySeqName);
	ajStrIterNext(ajpStrIterQuerySeqName);
	/*
	 * if there is a dot in the sequence name, truncate 
	 * the name there and append the first character
	 * after the dot to this leading string
	 */
	if(cQuerySeqName == cDot)
	{
	    ajBoolQueryNameRead = AJTRUE;
	    cQuerySeqName = ajStrIterGetK(ajpStrIterQuerySeqName);
	    /* 
	     * if the next character is not a number
	     * append a zero otherwise keep same digit
	     */
	    if(!isdigit(cQuerySeqName))
		cQuerySeqName = '0';
	}
	/* if there is no dot, use the whole sequence name */
	ajStrAppK(&ajpStrQuerySeqName, cQuerySeqName);
    }

    /* get name from sequence properties */
    ajpStrTempTemplateSeqName = ajStrNewS(ajpSeqTraceAcross->Name);
    /* iterate over the characters in the name */
    ajpStrIterTemplateSeqName = ajStrIter(ajpStrTempTemplateSeqName);
    ajBoolTemplateNameRead = AJFALSE;	    
    while(!ajStrIterDone(ajpStrIterTemplateSeqName) &&
	  !ajBoolTemplateNameRead)
    {
	/* read in current character */
	cTemplateSeqName = ajStrIterGetK(ajpStrIterTemplateSeqName);
	ajStrIterNext(ajpStrIterTemplateSeqName);
	/*
	 * if there is a dot in the sequence name, truncate 
	 * the name there and append the first character
	 * after the dot to this leading string
	 */
	if(cTemplateSeqName == cDot)
	{
	    ajBoolTemplateNameRead = AJTRUE;
	    cTemplateSeqName = ajStrIterGetK(ajpStrIterTemplateSeqName);
	    /* 
	     * if the next character is not a number
	     * append a zero otherwise keep same digit
	     */
	    if(!isdigit(cTemplateSeqName))
		cTemplateSeqName = '0';
	}
	/* if there is no dot, use the whole sequence name */
	ajStrAppK(&ajpStrTemplateSeqName, cTemplateSeqName);
    }

    /* delimiter between halves of sequence filename */
    ajpStrJoinOutfileName = ajStrNewC("_");    
    /*
     * note that the template sequence always comes first in
     * the gold standard alignment pair files but always comes
     * second in the names of those files and in my programs
     */
    ajStrCopy(&ajpStrSeqFileName, ajpStrQuerySeqName);
    ajStrApp(&ajpStrSeqFileName, ajpStrJoinOutfileName);
    ajStrApp(&ajpStrSeqFileName, ajpStrTemplateSeqName);
    ajFileNameShorten(&ajpStrSeqFileName);
    ajFileNameTrim(&ajpStrSeqFileName);

    /* substitute all matches into a new version of the template sequence */
    ajpSeqSubstituted = ajSeqNewS(ajpSeqTraceAcross);
    pcSubstitutedSeq  = ajSeqCharCopy(ajpSeqTraceAcross);

    /* copy the original traces into character strings */
    pcTraceAcross = ajSeqCharCopy(ajpSeqTraceAcross);
    pcTraceDown   = ajSeqCharCopy(ajpSeqTraceDown);

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
	    /* default to keeping current template residue */
	    cTempSubstitutedSeq = pcTraceAcross[ajIntTraceCount];
	    /* but substitute corresponding matches or gaps from query trace */
	    if(isalpha(cTempTraceDown) || is_a_gap(cTempTraceDown))
	    {
		cTempSubstitutedSeq = pcTraceDown[ajIntTraceCount];
	    }
	    
	    /* read substituted symbol into output */
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

    /* DDDDEBUG */
    if(enumDebugLevel)
	ajFmtPrint("FFFFileName: %S\tFFFF\n", ajpStrSeqFileName);

    ajSeqAssName(ajpSeqSubstituted, (const AjPStr)ajpStrSeqFileName);
    ajSeqAssSeq(ajpSeqSubstituted, (const AjPStr)ajpStrSubstitutedSeq);

    /* DDDDEBUG */
    if(enumDebugLevel)
    {
	ajFmtPrint("FFFSeqName: %S\tFFFF\n", ajpSeqSubstituted->Name);
	ajFmtPrint("FFFSeqSeq: %S\tFFFF\n", ajpSeqSubstituted->Seq);
    }
    
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



static AjBool is_a_gap(char cTraceElement)
{
    AjBool ajBoolIsAGap = AJFALSE;

    if ((cTraceElement == cHyphen) || (cTraceElement == cDot))
    {
	ajBoolIsAGap = AJTRUE;
    }

    return ajBoolIsAGap;
}
