/* @source substituterunner application
**
** takes two directories of pairs of aligned sequences and runs substitute
**  on them, outputting the substituted sequences into two other directories
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.1 $
** @modified $Date: 2004/10/14 18:49:17 $
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
	enumCountZero  = 0,
	enumDebugLevel = 2
    };

#include "emboss.h"

/* @prog substituterunner ****************************************************
** 
** runs substitute on two directories and writes substituted sequences to two
**  new directories
**
******************************************************************************/

int main(int argc , char **argv)
{
    AjPStr ajpStrPathToCommands      = NULL;
    AjPStr ajpStrCommandName         = NULL; /* substitition program         */
    AjPStr ajpStrFirstCommandLine    = NULL; /* 1st input to substitute prog */
    AjPStr ajpStrSecondCommandLine   = NULL; /* 2nd input to substitute prog */
    const char *pcFirstCommandLine   = NULL; /* for input to exec command    */
    const char *pcSecondCommandLine  = NULL; /* for input to exec command    */


    /* parameters to substitution command */
    AjPList ajpListFirstPairFiles       = NULL; /* first list of pair files     */
    AjPList ajpListSecondPairFiles      = NULL; /* first list of pair files     */
    AjPStr ajpStrInfileSuffix           = NULL; /* extension pair files         */
    AjPStr ajpStrRootOutfileName        = NULL; /* non-dir, non-extension name  */
    AjPStr ajpStrFirstPairFileName      = NULL;
    AjPStr ajpStrSecondPairFileName     = NULL;
    AjPStr ajpStrFirstPairFileNameCopy  = NULL;
    AjPStr ajpStrSecondPairFileNameCopy = NULL;
    AjPStr ajpStrFirstPathToOutfile     = NULL; /* first dir for sub'd files    */
    AjPStr ajpStrSecondPathToOutfile    = NULL; /* second dir for sub'd files   */
    AjPStr ajpStrFirstOutfileName       = NULL;
    AjPStr ajpStrSecondOutfileName      = NULL;
    AjPStr ajpStrFirstOutfileSuffix     = NULL;
    AjPStr ajpStrSecondOutfileSuffix    = NULL;
    const AjPStr ajpStrSeqoutFormat     = NULL; /* format of sub'd seqfile      */

    ajint ajIntSeqPairFilesInFirstDir;
    ajint ajIntSeqPairFilesInSecondDir;

    const char *pcFileNameSeparator  = ".";

    embInit("substituterunner", argc, argv);

    /* get command-line parameters from ACD */
    ajpStrPathToCommands      = ajAcdGetDirectoryName("pathtocommands");
    ajpStrFirstPathToOutfile  = ajAcdGetOutdirName("firstsubstitutedseqsoutdir");
    ajpStrSecondPathToOutfile = ajAcdGetOutdirName("secondsubstitutedseqsoutdir");
    ajpListFirstPairFiles     = ajAcdGetDirlist("firstalignedpairsdirlist");
    ajpListSecondPairFiles    = ajAcdGetDirlist("secondalignedpairsdirlist");

    /* assign other values */
    ajpStrCommandName         = ajStrNewC("substitute");
    ajpStrFirstOutfileSuffix  = ajStrNewC("sbs");
    ajpStrSecondOutfileSuffix = ajStrNewC("sbs");
    ajpStrSeqoutFormat        = ajStrNewC("fasta");

    /* count the number of sequence pairs to be aligned */
    ajIntSeqPairFilesInFirstDir  = ajListLength(ajpListFirstPairFiles);
    ajIntSeqPairFilesInSecondDir = ajListLength(ajpListSecondPairFiles);

    ajFmtPrint("\nfirst: %d\tsecond: %d\n\n", ajIntSeqPairFilesInFirstDir, ajIntSeqPairFilesInSecondDir);

    /* loop over pair files in alignment directory */
    while(ajListPop(ajpListFirstPairFiles, (void **)&ajpStrFirstPairFileName))
    {
	ajListPop(ajpListSecondPairFiles, (void **)&ajpStrSecondPairFileName);

	/* DDDDEBUG */
	if(enumDebugLevel)
	{
	    printf("\n%s\n", "START OF LOOP");
	    ajFmtPrint("ajpStrFirstPairFileName:\t%S\n", ajpStrFirstPairFileName);
	    ajFmtPrint("ajpStrSecondPairFileName:\t%S\n", ajpStrSecondPairFileName);
	    printf("%s\n", "START OF LOOP");
	}

	/* make modifiable copies of the current file names */
	ajStrCopy(&ajpStrFirstPairFileNameCopy, ajpStrFirstPairFileName);
	ajStrCopy(&ajpStrSecondPairFileNameCopy, ajpStrSecondPairFileName);

	/* get file name stems */
	ajFileNameTrim(&ajpStrFirstPairFileNameCopy);
	ajFileNameTrim(&ajpStrSecondPairFileNameCopy);
	
	ajFileNameShorten(&ajpStrFirstPairFileNameCopy);
	ajFileNameShorten(&ajpStrSecondPairFileNameCopy);


	/* assemble each output file name */
	ajStrApp(&ajpStrFirstOutfileName, ajpStrFirstPathToOutfile);
	ajStrApp(&ajpStrSecondOutfileName, ajpStrSecondPathToOutfile);
	ajStrApp(&ajpStrFirstOutfileName, ajpStrFirstPairFileNameCopy);
	ajStrApp(&ajpStrSecondOutfileName, ajpStrSecondPairFileNameCopy);
	ajStrAppC(&ajpStrFirstOutfileName, pcFileNameSeparator);
	ajStrAppC(&ajpStrSecondOutfileName, pcFileNameSeparator);
	ajStrApp(&ajpStrFirstOutfileName, ajpStrFirstOutfileSuffix);
	ajStrApp(&ajpStrSecondOutfileName, ajpStrSecondOutfileSuffix);

	/* find last dot in file name */
/* 	ajIntSeparatorPos = ajStrRFindC(ajpStrPairFileNameCopy, */
/* 					pcFileNameSeparator); */
	
	/*
	 * copy everything after it into second pair file name
	 */
/* 	ajStrAssSub(&ajpStrSecondPairFileName, */
/* 		    ajpStrPairFileNameCopy, */
/* 		    enumCountZero, */
/* 		    ajIntSeparatorPos); */

	/* assemble each input file name */
/* 	ajStrCopy(&ajpStrFirstPairFileName, */
/* 		   ajpStrPairFileNameCopy); */
/* 	ajStrApp(&ajpStrSecondPairFileName, ajpStrSecondInfileSuffix); */


	/* create new command line strings */
	ajpStrFirstCommandLine  = ajStrNew();
	ajpStrSecondCommandLine = ajStrNew();

	/*
	 * assemble command lines to substitute each current query trace
	 * into each template trace
	 */
	ajFmtPrintS(&ajpStrFirstCommandLine,
		    "%S%S -alignedpair %S -substitutedseq %S",
		    ajpStrPathToCommands, ajpStrCommandName,
		    ajpStrFirstPairFileName,
		    ajpStrFirstOutfileName);

	ajFmtPrintS(&ajpStrSecondCommandLine,
		    "%S%S -alignedpair %S -substitutedseq %S",
		    ajpStrPathToCommands, ajpStrCommandName,
		    ajpStrSecondPairFileName,
		    ajpStrSecondOutfileName);

	pcFirstCommandLine  = ajStrStr(ajpStrFirstCommandLine);
	pcSecondCommandLine = ajStrStr(ajpStrSecondCommandLine);

	system(pcFirstCommandLine);
	system(pcSecondCommandLine);

	printf("\n%s", "Hello?!");

	/* DDDDEBUG */
	if(enumDebugLevel)
	{
	    printf("\npcFirstCommandLine:\t%s", pcFirstCommandLine);
	    printf("\npcSecondCommandLine:\t%s", pcSecondCommandLine);
	}
	
	/* free command strings */
	ajStrDel(&ajpStrRootOutfileName);
	ajStrDel(&ajpStrFirstPairFileName);
	ajStrDel(&ajpStrSecondPairFileName);
	ajStrDel(&ajpStrFirstOutfileName);
	ajStrDel(&ajpStrSecondOutfileName);
	ajStrDel(&ajpStrFirstCommandLine);
	ajStrDel(&ajpStrSecondCommandLine);
 	/* DDDDEBUG */
/* 	if(enumDebugLevel) */
/* 	{ */
/* 	    printf("\n%s\n", "END OF LOOP"); */
/* 	    ajFmtPrint("%S\n", ajpStrPairFileName); */
/* 	    printf("%s\n", "END OF LOOP"); */
/* 	} */
   }    

    /* XXXX YOU NEED TO REWIND THE PAIR FILE LISTS HERE */

    /* clear up the pair file objects */
    while(ajListPop(ajpListFirstPairFiles, (void **)&ajpStrFirstPairFileName))
	AJFREE(ajpStrFirstPairFileName);
    ajListDel(&ajpListFirstPairFiles);
    while(ajListPop(ajpListSecondPairFiles, (void **)&ajpStrSecondPairFileName))
	AJFREE(ajpStrSecondPairFileName);
    ajListDel(&ajpListSecondPairFiles);

    /* clean up remaining strings */
    ajStrDel(&ajpStrFirstPairFileName);
    ajStrDel(&ajpStrSecondPairFileName);
    ajStrDel(&ajpStrInfileSuffix);

    /* clear up the pair file list */
    ajListFree(&ajpListFirstPairFiles);
    ajListFree(&ajpListSecondPairFiles);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}


