/* @source scorerrunner application
**
** takes two directories of sequences, the first containing gold-standard
**  substituted protein sequences and the second containing sequences 
**  substituted with matches aligned with a test method and runs the scorer
**  program on order pairs of each, putting the scores obtained by
**  comparing them into a third output directory
**
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.2 $
** @modified $Date: 2004/11/22 18:21:17 $
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

/* @prog scorerrunner ********************************************************
** 
** runs scorer on two directories and writes scores to a new directory
**
******************************************************************************/

int main(int argc , char **argv)
{
    AjPStr ajpStrPathToCommand       = NULL;
    AjPStr ajpStrCommandName         = NULL; /* scoring program             */
    AjPStr ajpStrCommandLine         = NULL; /* scoring program             */
    const char *pcCommandLine        = NULL; /* for input to exec command   */


    /* parameters to substitution command */
    AjPList ajpListGoldSeqFiles      = NULL; /* list of gold std seq files  */
    AjPList ajpListTestSeqFiles      = NULL; /* list of test seq files      */
    AjPStr ajpStrInfileSuffix        = NULL; /* extension pair files        */
    AjPStr ajpStrRootOutfileName     = NULL; /* non-dir, non-extension name */
    AjPStr ajpStrGoldSeqFileName     = NULL;
    AjPStr ajpStrTestSeqFileName     = NULL;
    AjPStr ajpStrGoldSeqFileNameCopy = NULL;
    AjPStr ajpStrTestSeqFileNameCopy = NULL;
    AjPStr ajpStrPathToOutfile       = NULL; /* directory for scores        */
    AjPStr ajpStrOutfileSuffix       = NULL;

    ajint ajIntSeqFilesInGoldDir;
    ajint ajIntSeqFilesInTestDir;

/*     const char *pcFileNameSeparator  = "."; */

    embInit("scorerrunner", argc, argv);

    /* get command-line parameters from ACD */
    ajpStrPathToCommand  = ajAcdGetDirectoryName("pathtocommand");
    ajpListGoldSeqFiles  = ajAcdGetDirlist("goldseqsdirlist");
    ajpListTestSeqFiles  = ajAcdGetDirlist("testseqsdirlist");
    ajpStrPathToOutfile  = ajAcdGetOutdirName("scoresoutdir");

    /* assign other values */
    ajpStrCommandName    = ajStrNewC("scorer");
    ajpStrOutfileSuffix  = ajStrNewC("scored");
/*     ajpStrSeqoutFormat   = ajStrNewC("fasta"); */

    /* count the number of sequence pairs to be aligned */
    ajIntSeqFilesInGoldDir = ajListLength(ajpListGoldSeqFiles);
    ajIntSeqFilesInTestDir = ajListLength(ajpListTestSeqFiles);

    ajFmtPrint("\nlength of gold directory list: %d\n\n", ajIntSeqFilesInGoldDir);
    ajFmtPrint("\nlength of test directory list: %d\n\n", ajIntSeqFilesInTestDir);

    ajpStrInfileSuffix = ajStrNew();

    /* loop over pair files in alignment directory */
    while(ajListPop(ajpListGoldSeqFiles, (void **)&ajpStrGoldSeqFileName))
    {
	ajListPop(ajpListTestSeqFiles, (void **)&ajpStrTestSeqFileName);

	/* DDDDEBUG */
	if(enumDebugLevel)
	{
	    printf("\n%s\n", "START OF LOOP");
	    ajFmtPrint("ajpStrGoldSeqFileName:\t%S\n", ajpStrGoldSeqFileName);
	    ajFmtPrint("ajpStrTestSeqFileName:\t%S\n", ajpStrTestSeqFileName);
	    printf("%s\n", "START OF LOOP");
	}

	/* make modifiable copies of the current file names */
	ajStrCopy(&ajpStrGoldSeqFileNameCopy, ajpStrGoldSeqFileName);
	ajStrCopy(&ajpStrTestSeqFileNameCopy, ajpStrTestSeqFileName);

	/* get file name stems */
	ajFileNameTrim(&ajpStrGoldSeqFileNameCopy);
	ajFileNameTrim(&ajpStrTestSeqFileNameCopy);
	
	ajFileNameShorten(&ajpStrGoldSeqFileNameCopy);
	ajFileNameShorten(&ajpStrTestSeqFileNameCopy);

	/* create new command line strings */
	ajpStrCommandLine  = ajStrNew();

	/*
	 * assemble command line to score each pair of substituted sequences
	 */
	ajFmtPrintS(&ajpStrCommandLine,
		    "%S%S %S %S -auto >> /users/damian/EMBOSS/emboss/emboss/emboss/contacttest/test_scoring_files/22Nov04/scores.csv",
		    ajpStrPathToCommand, ajpStrCommandName,
		    ajpStrGoldSeqFileName,
		    ajpStrTestSeqFileName);

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
	ajStrDel(&ajpStrGoldSeqFileName);
	ajStrDel(&ajpStrTestSeqFileName);
	ajStrDel(&ajpStrCommandLine);
 
    }    

    /* clear up the pair file objects */
    while(ajListPop(ajpListGoldSeqFiles, (void **)&ajpStrGoldSeqFileName))
	AJFREE(ajpStrGoldSeqFileName);
    ajListDel(&ajpListGoldSeqFiles);
    while(ajListPop(ajpListTestSeqFiles, (void **)&ajpStrTestSeqFileName))
	AJFREE(ajpStrTestSeqFileName);
    ajListDel(&ajpListTestSeqFiles);

    /* clean up remaining strings */
    ajStrDel(&ajpStrGoldSeqFileName);
    ajStrDel(&ajpStrTestSeqFileName);
    ajStrDel(&ajpStrInfileSuffix);

    /* clear up the pair file list */
    ajListFree(&ajpListGoldSeqFiles);
    ajListFree(&ajpListTestSeqFiles);

    /* tidy up everything else */
    ajExit();

    /* exit properly (main() was declared as an int) */
    return 0;
}
