/* @source substituterunner application
**
** takes two directories of pairs of aligned sequences and runs substitute
**  on them, outputting the substituted sequences into two other directories
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.3 $
** @modified $Date: 2004/10/26 14:16:33 $
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
    AjPStr ajpStrCommandLine         = NULL; /* for running "substitute"     */
    const char *pcCommandLine        = NULL; /* for input to exec command    */

    /* parameters to substitution command */
    AjPList ajpListPairFiles            = NULL; /* list of pair files           */
    AjPStr ajpStrInfileSuffix           = NULL; /* extension pair files         */
    AjPStr ajpStrRootOutfileName        = NULL; /* non-dir, non-extension name  */
    AjPStr ajpStrPairFileName           = NULL;
    AjPStr ajpStrPairFileNameCopy       = NULL;
    AjPStr ajpStrPathToOutfile          = NULL; /* dir for sub'd files          */
    AjPStr ajpStrOutfileName            = NULL;
    AjPStr ajpStrOutfileSuffix          = NULL;
    const AjPStr ajpStrSeqoutFormat     = NULL; /* format of sub'd seqfile      */

    ajint ajIntSeqPairFilesInPairDir;

    const char *pcFileNameSeparator  = ".";

    embInit("substituterunner", argc, argv);

    /* get command-line parameters from ACD */
    ajpStrPathToCommands = ajAcdGetDirectoryName("pathtocommands");
    ajpStrPathToOutfile  = ajAcdGetOutdirName("substitutedseqsoutdir");
    ajpListPairFiles     = ajAcdGetDirlist("alignedpairsdirlist");

    /* assign other values */
    ajpStrCommandName   = ajStrNewC("substitute");
    ajpStrOutfileSuffix = ajStrNewC("sbs");
    ajpStrSeqoutFormat  = ajStrNewC("fasta");

    /* count the number of sequence pairs to be aligned */
    ajIntSeqPairFilesInPairDir = ajListLength(ajpListPairFiles);

    ajFmtPrint("\nfirst: %d\tsecond: %d\n\n", ajIntSeqPairFilesInPairDir);

    /* loop over pair files in alignment directory */
    while(ajListPop(ajpListPairFiles, (void **)&ajpStrPairFileName))
    {
	/* DDDDEBUG */
	if(enumDebugLevel)
	{
	    printf("\n%s\n", "START OF LOOP");
	    ajFmtPrint("ajpStrPairFileName:\t%S\n", ajpStrPairFileName);
	    printf("%s\n", "START OF LOOP");
	}

	/* make modifiable copies of the current file names */
	ajStrCopy(&ajpStrPairFileNameCopy, ajpStrPairFileName);

	/* get file name stems */
	ajFileNameTrim(&ajpStrPairFileNameCopy);
	ajFileNameShorten(&ajpStrPairFileNameCopy);

	/* assemble each file name */
	ajStrApp(&ajpStrOutfileName, ajpStrPathToOutfile);
	ajStrApp(&ajpStrOutfileName, ajpStrPairFileNameCopy);
	ajStrAppC(&ajpStrOutfileName, pcFileNameSeparator);
	ajStrApp(&ajpStrOutfileName, ajpStrOutfileSuffix);

	/* create new command line string */
	ajpStrCommandLine = ajStrNew();

	/*
	 * assemble command line to substitute each current query trace
	 * into each template trace
	 */
	ajFmtPrintS(&ajpStrCommandLine,
		    "%S%S -alignedpair %S -substitutedseq %S",
		    ajpStrPathToCommands, ajpStrCommandName,
		    ajpStrPairFileName,
		    ajpStrOutfileName);

	pcCommandLine  = ajStrStr(ajpStrCommandLine);

	system(pcCommandLine);

	printf("\n%s", "Hello?!");

	/* DDDDEBUG */
	if(enumDebugLevel)
	{
	    printf("\npcCommandLine:\t%s", pcCommandLine);
	}
	
	/* free command strings */
	ajStrDel(&ajpStrRootOutfileName);
	ajStrDel(&ajpStrPairFileName);
	ajStrDel(&ajpStrOutfileName);
	ajStrDel(&ajpStrCommandLine);
   }    

    /* XXXX YOU NEED TO REWIND THE PAIR FILE LISTS HERE */

    /* clear up the pair file objects */
    while(ajListPop(ajpListPairFiles, (void **)&ajpStrPairFileName))
	AJFREE(ajpStrPairFileName);
    ajListDel(&ajpListPairFiles);

    /* clean up remaining strings */
    ajStrDel(&ajpStrPairFileName);
    ajStrDel(&ajpStrInfileSuffix);

    /* clear up the pair file list */
    ajListFree(&ajpListPairFiles);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}


