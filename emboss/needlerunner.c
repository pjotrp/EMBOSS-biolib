/* @source needlerunner application
**
** Runs EMBOSS alignment program needle on all sequence pairs in given directory and
**  writes the resulting alignments into a new directory
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.1 $
** @modified $Date: 2004/10/27 17:55:20 $
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
	enumTemplateSeqIndex  =  0,
	enumQuerySeqIndex     =  1,
	enumCountZero         =  0,
    };


/* @prog needlerunner ********************************************************
** 
** runs needle alignment program on a directory full of sequence pairs 
**
******************************************************************************/

int main( int argc , char **argv )
{
    /* position counters and limits */
    ajint ajIntNumberOfSeqPairFiles  = enumCountZero;
    AjPSeqin ajpSeqinUnalignedPair   = NULL;
    float fExtensionPenalty;
    float fGapPenalty;
    const AjPSeq ajpSeqTemplate      = NULL; /* first unaligned sequence in file as string    */
    const AjPSeq ajpSeqQuery         = NULL; /* second unaligned sequence in file as string   */
    const AjPStr ajpStrTemplateSeq   = NULL; /* first unaligned sequence in file as object    */
    const AjPStr ajpStrQuerySeq      = NULL; /* second unaligned sequence in file as object   */
    const char *pcCommandLine        = NULL;

    /* all unaligned input sequences in directory */
    AjPList ajpListSeqPairFiles      = NULL;
    /* current pair of unaligned input sequences */
    AjPStr ajpStrSeqPairFileName     = NULL; 
    AjPFile ajpFileSeqPair           = NULL; 
    AjPSeqset ajpSeqsetUnalignedPair = NULL;

    /* alignment parameters */
    AjPStr ajpStrPathToCommand        = NULL;
    AjPStr ajpStrCommandName          = NULL;
    AjPStr ajpStrPathToScoringMatrix  = NULL;
    AjPStr ajpStrScoringMatrixName    = NULL;
    AjPStr ajpStrPathToOutfile        = NULL; 
    AjPStr ajpStrRootOutfileName      = NULL; /* stem of name of both aligned output files     */
    AjPStr ajpStrJoinOutfileName      = NULL; /* connects first part of outfile name to second */
    AjPStr ajpStrOutfileName          = NULL; /* full name of output file of aligned sequences */
    AjPStr ajpStrOutfileSuffix        = NULL; /* filename extension for alignment file         */
    AjPStr ajpStrAlignmentFormat      = NULL; /* format for output alignment file              */
    AjPStr ajpStrCommandLine          = NULL; /* command line given to alignment program       */

    embInit("needlerunner", argc, argv);

    /* get command-line parameters from ACD */     
    ajpStrPathToCommand       = ajAcdGetDirectoryName("pathtocommand");
    ajpStrCommandName         = ajAcdGetString("commandname");
    ajpStrPathToScoringMatrix = ajAcdGetDirectoryName("scoringmatrixdirname");
    ajpStrScoringMatrixName   = ajAcdGetString("scoringmatrixname");
    fGapPenalty               = ajAcdGetFloat("gapopen");
    fExtensionPenalty         = ajAcdGetFloat("gapextend");
    ajpStrPathToOutfile       = ajAcdGetOutdirName("alignedseqsoutdir");
    ajpListSeqPairFiles       = ajAcdGetDirlist("seqpairsdir");
    ajpStrOutfileSuffix       = ajAcdGetString("outfilesuffix");
    ajpStrAlignmentFormat     = ajAcdGetString("outfileformat");

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
    ajpStrJoinOutfileName = ajStrNewC("_");

    ajpSeqTemplate = ajSeqNew();    
    ajpSeqQuery = ajSeqNew();
    ajpStrTemplateSeq = ajStrNew();    
    ajpStrQuerySeq = ajStrNew();
    
    while(ajListPop(ajpListSeqPairFiles, (void **)&ajpStrSeqPairFileName))
    {
	ajpStrRootOutfileName = ajStrNew();
	ajpStrOutfileName = ajStrNew();
	ajpStrCommandLine = ajStrNew();

	/* read two unaligned sequences into memory */
	ajSeqinUsa(&ajpSeqinUnalignedPair, ajpStrSeqPairFileName);
	ajSeqsetRead(ajpSeqsetUnalignedPair, ajpSeqinUnalignedPair);

	if(ajpSeqsetUnalignedPair->Size > 1)
	{
	    ajpSeqTemplate = ajSeqsetGetSeq(ajpSeqsetUnalignedPair,
					 enumTemplateSeqIndex);
	    ajpSeqQuery = ajSeqsetGetSeq(ajpSeqsetUnalignedPair, 
					    enumQuerySeqIndex);
	    
	    ajpStrTemplateSeq = ajSeqStr(ajpSeqTemplate);
	    ajpStrQuerySeq = ajSeqStr(ajpSeqQuery);

	    /* construct output file name from IDs of each seq to be aligned */
	    ajStrCopy(&ajpStrRootOutfileName, ajpSeqTemplate->Name);
	    ajStrApp(&ajpStrRootOutfileName, ajpStrJoinOutfileName);
	    ajStrApp(&ajpStrRootOutfileName, ajpSeqQuery->Name);

	    ajStrCopy(&ajpStrOutfileName, ajpStrRootOutfileName);
	    ajStrApp(&ajpStrOutfileName, ajpStrOutfileSuffix);

	    /* assemble command line as ajpString */
	    ajFmtPrintS(&ajpStrCommandLine,
			"%S%S -asequence asis::%S  -bsequence asis::%S -datafile %S%S -gapopen %2.1f -gapextend %2.1f -outfile %S%S -aformat3 %S",
			ajpStrPathToCommand, ajpStrCommandName,
			ajpStrTemplateSeq, ajpStrQuerySeq,
			ajpStrPathToScoringMatrix, ajpStrScoringMatrixName,
			fGapPenalty, fExtensionPenalty,
			ajpStrPathToOutfile, ajpStrOutfileName,
			ajpStrAlignmentFormat);

	    /* execute commands */
	    pcCommandLine = ajStrStr(ajpStrCommandLine);
	    system(pcCommandLine);

	    /* DDDDEBUG */
	    if(enumDebugLevel)
	    {
		printf("\n%s", pcCommandLine);
	    }
	}
	else
	{
	    ajFmtPrint("\nSINGLE ENTRY IN FILE\n");
	}

	ajStrDel(&ajpStrRootOutfileName);
	ajStrDel(&ajpStrOutfileName);
	ajStrDel(&ajpStrCommandLine);
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
    ajStrDel(&ajpStrCommandLine);

    /*  tidy up everything else... */
    ajExit();

    return 0;
}
