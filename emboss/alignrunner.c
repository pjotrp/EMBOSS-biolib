/* @source alignrunner application
**
** Runs EMBOSS alignment programs on all sequence pairs in given directory and
**  writes the resulting alignments into a new directory
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.4 $
** @modified $Date: 2004/09/20 18:18:01 $
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

enum constant
    {
	enumDebugLevel        =  0,
	enumQuerySeqIndex     =  0,
	enumTemplateSeqIndex  =  1,
	enumCountZero         =  0,
    };


/* @prog alignrunner *******************************************************
** 
** run alignment programs on a directory full of sequence pairs 
**
******************************************************************************/

int main( int argc , char **argv )
{
    /* position counters and limits */
    ajint ajIntCount;
    ajint ajIntNumberOfSeqPairFiles  = enumCountZero;
    AjPSeqin ajpSeqinUnalignedPair   = NULL;
    float fExtensionPenalty;
    float fGapPenalty;
    const AjPSeq ajpSeqQuery         = NULL; /* first unaligned sequence in file as string    */
    const AjPSeq ajpSeqTemplate      = NULL; /* second unaligned sequence in file as string   */
    const AjPStr ajpStrQuerySeq      = NULL; /* first unaligned sequence in file as object    */
    const AjPStr ajpStrTemplateSeq   = NULL; /* second unaligned sequence in file as object   */
    const char *pcFirstCommandLine   = NULL;
    const char *pcSecondCommandLine  = NULL;

    /* all unaligned input sequences in directory */
    AjPList ajpListSeqPairFiles      = NULL;
    /* current pair of unaligned input sequences */
    AjPStr ajpStrSeqPairFileName     = NULL; 
    AjPFile ajpFileSeqPair           = NULL; 
    AjPSeqset ajpSeqsetUnalignedPair = NULL;

    /* alignment parameters */
    AjPStr ajpStrPathToCommands       = NULL;
    AjPStr ajpStrFirstCommandName     = NULL;
    AjPStr ajpStrSecondCommandName    = NULL;
    AjPStr ajpStrPathToScoringMatrix  = NULL;
    AjPStr ajpStrScoringMatrixName    = NULL;
    AjPStr ajpStrFirstPathToOutfile   = NULL; /* directories for aligned files... */
    AjPStr ajpStrSecondPathToOutfile  = NULL; /*   ...from two alignment progs    */
    AjPStr ajpStrRootOutfileName      = NULL; /* stem of name of both aligned output files     */
    AjPStr ajpStrJoinOutfileName      = NULL; /* connects first part of outfile name to second */
    AjPStr ajpStrFirstOutfileName     = NULL; /* full name of output file of aligned sequences */
    AjPStr ajpStrSecondOutfileName    = NULL; /* full name of output file of aligned sequences */
    AjPStr ajpStrFirstOutfileSuffix   = NULL; /* filename extension for alignment file         */
    AjPStr ajpStrSecondOutfileSuffix  = NULL; /* filename extension for alignment file         */
    AjPStr ajpStrAlignmentFormat      = NULL; /* format for output alignment file              */
    AjPStr ajpStrFirstCommandLine     = NULL; /* command line given to alignment program       */
    AjPStr ajpStrSecondCommandLine    = NULL; /* command line given to alignment program       */
    embInit("alignrunner", argc, argv);


    /* get command-line parameters from ACD */     
    ajpStrPathToCommands = ajAcdGetDirectoryName("pathtocommands");
    ajpStrFirstCommandName = ajAcdGetString("firstcommandname");
    ajpStrSecondCommandName = ajAcdGetString("secondcommandname");
    ajpStrPathToScoringMatrix = ajAcdGetDirectoryName("scoringmatrixdirname");
    ajpStrScoringMatrixName = ajAcdGetString("scoringmatrixname");
    fGapPenalty = ajAcdGetFloat("gapopen");
    fExtensionPenalty = ajAcdGetFloat("gapextend");
    ajpStrFirstPathToOutfile = ajAcdGetOutdirName("firstalignedseqsoutdir");
    ajpStrSecondPathToOutfile = ajAcdGetOutdirName("secondalignedseqsoutdir");
    ajpListSeqPairFiles = ajAcdGetDirlist("seqpairsdir");
    ajpStrFirstOutfileSuffix = ajAcdGetString("firstoutfilesuffix");
    ajpStrSecondOutfileSuffix = ajAcdGetString("secondoutfilesuffix");
    ajpStrAlignmentFormat = ajAcdGetString("outfileformat");

    /* count the number of sequence pairs to be aligned */
    ajIntNumberOfSeqPairFiles = ajListLength(ajpListSeqPairFiles);

    /* new string for current input file name */
    ajpStrSeqPairFileName  = ajStrNew();

    /* loop over sequence files in current directory */
    /* new sequence input object */
    ajpSeqinUnalignedPair = ajSeqinNew();
    /* new seqset to store input unaligned sequences */
    ajpSeqsetUnalignedPair = ajSeqsetNew();
    /* new string for alignment command and options to it */
    ajpStrJoinOutfileName = ajStrNewC("_vs_");
    ajpSeqQuery = ajSeqNew();    
    ajpSeqTemplate = ajSeqNew();
    ajpStrQuerySeq = ajStrNew();    
    ajpStrTemplateSeq = ajStrNew();
    
    /*     while(ajListPop(ajpListSeqPairFiles, (void **)&ajpStrSeqPairFileName)) */
    /* DDDDEBUG */
    for(ajIntCount = 0;ajIntCount < 10;ajIntCount++)
    {
	ajpStrRootOutfileName = ajStrNew();
	ajpStrFirstOutfileName = ajStrNew();
	ajpStrSecondOutfileName = ajStrNew();
	ajpStrFirstCommandLine = ajStrNew();
	ajpStrSecondCommandLine = ajStrNew();

	/* DDDDEBUG */
	ajListPop(ajpListSeqPairFiles, (void **)&ajpStrSeqPairFileName);
	ajFmtPrint( "%S\n", ajpStrSeqPairFileName);

	/* read two unaligned sequences into memory */
	ajSeqinUsa(&ajpSeqinUnalignedPair, ajpStrSeqPairFileName);
	ajSeqsetRead(ajpSeqsetUnalignedPair, ajpSeqinUnalignedPair);

	ajpSeqQuery = ajSeqsetGetSeq(ajpSeqsetUnalignedPair, enumQuerySeqIndex);
	ajpSeqTemplate = ajSeqsetGetSeq(ajpSeqsetUnalignedPair, enumTemplateSeqIndex);

	ajpStrQuerySeq = ajSeqStr(ajpSeqQuery);
	ajpStrTemplateSeq = ajSeqStr(ajpSeqTemplate);

	ajStrCopy(&ajpStrRootOutfileName, ajpSeqQuery->Name);
	ajStrApp(&ajpStrRootOutfileName, ajpStrJoinOutfileName);
	ajStrApp(&ajpStrRootOutfileName, ajpSeqTemplate->Name);
/* 	ajFileNameTrim(&ajpStrFirstOutfileName); */
/* 	ajFileNameTrim(&ajpStrSecondOutfileName); */
/* 	ajFileNameShorten(&ajpStrFirstOutfileName); */
/* 	ajFileNameShorten(&ajpStrSecondOutfileName); */
	ajStrCopy(&ajpStrFirstOutfileName, ajpStrRootOutfileName);
	ajStrCopy(&ajpStrSecondOutfileName, ajpStrRootOutfileName);	
	ajStrApp(&ajpStrFirstOutfileName, ajpStrFirstOutfileSuffix);
	ajStrApp(&ajpStrSecondOutfileName, ajpStrSecondOutfileSuffix);

	/* obtain sequences as modifiable strings */
	ajFmtPrintS(&ajpStrFirstCommandLine,
		    "%S%S -asequence asis::%S  -bsequence asis::%S -datafile %S%S -gapopen %2.1f -gapextend %2.1f -outfile %S%S -aformat3 %S",
		    ajpStrPathToCommands, ajpStrFirstCommandName,
		    ajpStrQuerySeq, ajpStrTemplateSeq,
		    ajpStrPathToScoringMatrix, ajpStrScoringMatrixName,
		    fGapPenalty, fExtensionPenalty,
		    ajpStrFirstPathToOutfile, ajpStrFirstOutfileName,
		    ajpStrAlignmentFormat);

	ajFmtPrintS(&ajpStrSecondCommandLine,
		    "%S%S -down asis::%S -across asis::%S -substitution %S%S -gapo %2.1f -gape %2.1f -aligned %S%S",
		    ajpStrPathToCommands, ajpStrSecondCommandName,
		    ajpStrQuerySeq, ajpStrTemplateSeq,
		    ajpStrPathToScoringMatrix, ajpStrScoringMatrixName,
		    fGapPenalty, fExtensionPenalty,
		    ajpStrSecondPathToOutfile, ajpStrSecondOutfileName);

	pcFirstCommandLine = ajStrStr(ajpStrFirstCommandLine);
	pcSecondCommandLine = ajStrStr(ajpStrSecondCommandLine);
	system(pcFirstCommandLine);
	system(pcSecondCommandLine);
	printf("\n%s", pcFirstCommandLine);
 	printf("\n%s", pcSecondCommandLine);
	ajStrDel(&ajpStrRootOutfileName);
	ajStrDel(&ajpStrFirstOutfileName);
	ajStrDel(&ajpStrSecondOutfileName);
	ajStrDel(&ajpStrFirstCommandLine);
	ajStrDel(&ajpStrSecondCommandLine);
   }

    /*
     * clear up the alignment objects,
     * starting with the file objects
     */
    while(ajListPop(ajpListSeqPairFiles,
		    (void **)&ajpFileSeqPair))
	AJFREE(ajpFileSeqPair);
    ajListFree(&ajpListSeqPairFiles);

    ajStrDel(&ajpStrSeqPairFileName);
    ajSeqinDel(&ajpSeqinUnalignedPair);
    ajSeqsetDel(&ajpSeqsetUnalignedPair);
    ajStrDel(&ajpStrFirstCommandLine);
    ajStrDel(&ajpStrSecondCommandLine);

    /*  tidy up everything else... */
    ajExit();

    return 0;
}







