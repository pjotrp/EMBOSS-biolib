/* @source degapseqrunner application
**
** Runs EMBOSS gap removing program on all sequence pairs in given directory and
**  writes the resulting ungapped sequence pairs into a new directory
**
** @author: Copyright (C) Damian Counsell
** @version $Revision: 1.2 $
** @modified $Date: 2004/10/14 18:55:53 $
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
	enumCountZero         =  0,
    };


/* @prog alignrunner.c *******************************************************
** 
** run degapseq program on a directory full of sequence pairs 
**
******************************************************************************/

int main( int argc , char **argv )
{
    ajint ajIntNumberOfSeqPairFiles  = 0;
    ajint ajIntSeparatorPos          = 0;    /* where does the filename root meet its suffix */

    /* all unaligned input sequences in directory */
    AjPList ajpListSeqPairFiles      = NULL;
    /* current pair of unaligned input sequences */
    AjPStr ajpStrSeqPairFileName     = NULL; 
    AjPStr ajpStrSeqPairFile         = NULL; 
    AjPStr ajpStrSeqPairFileNameRoot = NULL; 
    AjPStr ajpStrSeqOutFileNameRoot  = NULL; 

    /* command line parameters */
    const char *pcCommandLine        = NULL;
    AjPStr ajpStrPathToCommand       = NULL;
    AjPStr ajpStrCommandName         = NULL;
    AjPStr ajpStrPathToOutFile       = NULL; /*   ...from two alignment progs    */
    AjPStr ajpStrOutFileSuffix       = NULL; /* filename extension for alignment file         */
    AjPStr ajpStrCommandLine         = NULL; /* command line given to alignment program       */

    const char *pcFileNameSeparator  = ".";

    ajpStrPathToCommand = ajStrNew();
    ajpStrCommandName   = ajStrNew();
    ajpStrPathToOutFile = ajStrNew();

    embInit( "degapseqrunner", argc, argv );

    /*
     * get command-line parameters from ACD
     */
    ajpListSeqPairFiles = ajAcdGetDirlist("gappedseqsdir");
    ajpStrPathToCommand = ajAcdGetString("pathtocommand");
    ajpStrPathToOutFile = ajAcdGetOutdirName("degappedseqsoutdir");
    ajpStrCommandName   = ajAcdGetString("commandname");
    ajpStrOutFileSuffix = ajAcdGetString("outfilesuffix");

    /* count the number of sequence pairs to be aligned */
    ajIntNumberOfSeqPairFiles = ajListLength(ajpListSeqPairFiles);

    /* loop over sequence files in current directory */
    /* new string for current input file name */
    ajpStrSeqPairFileName     = ajStrNew();
    ajpStrSeqPairFileNameRoot = ajStrNew();
    ajpStrSeqOutFileNameRoot  = ajStrNew();
    ajpStrCommandLine         = ajStrNew();

    while(ajListPop(ajpListSeqPairFiles, (void **)&ajpStrSeqPairFileName))
    {
	ajFmtPrint("%S\n", ajpStrSeqPairFileName);
	ajStrCopy(&ajpStrSeqPairFileNameRoot, ajpStrSeqPairFileName);
	ajFileNameShorten(&ajpStrSeqPairFileNameRoot);
	ajFmtPrint("\nSHORTENED:\t");
	ajFmtPrint("%S\n", ajpStrSeqPairFileNameRoot);
	ajFmtPrint("\nFULL:\t");
	ajFmtPrint("%S%S\n",
		   ajpStrSeqPairFileName);

	/* find last dot in file name */
	ajIntSeparatorPos = ajStrRFindC(ajpStrSeqPairFileName,
					pcFileNameSeparator);
	
	/* replace everything after it with new substituted suffix */
	ajFmtPrint("\n\najpStrOutFileSuffix:\t%S\n", ajpStrOutFileSuffix);
	ajStrAssSub(&ajpStrSeqOutFileNameRoot,
		    ajpStrSeqPairFileName,
		    enumCountZero,
		    ajIntSeparatorPos);
	
	/* DDDDEBUG: PRINT CURRENT OUTPUT FILENAME FOR SCORING DIRECTORY */
	ajFmtPrint("\najpStrSeqOutFileNameRoot:\t%S",
		   ajpStrSeqOutFileNameRoot);
	
	ajFmtPrintS(&ajpStrCommandLine,
		    "%S/%S %S -outseq=%S/%S%S",
		    ajpStrPathToCommand, ajpStrCommandName,
		    ajpStrSeqPairFileName,
		    ajpStrPathToOutFile, ajpStrSeqPairFileNameRoot, ajpStrOutFileSuffix);
	pcCommandLine = ajStrStr(ajpStrCommandLine);
	system(pcCommandLine);
	printf("\n%s", pcCommandLine);
    }

    /*
     * clear up the alignment objects,
     * starting with the file names
     */
    ajpStrSeqPairFile = ajStrNew();
    while(ajListPop(ajpListSeqPairFiles,
		    (void **)&ajpStrSeqPairFile))
	AJFREE(ajpStrSeqPairFile);
    ajListFree(&ajpListSeqPairFiles);

    ajStrDel(&ajpStrSeqPairFileName);
    ajStrDel(&ajpStrSeqPairFileNameRoot);
    ajStrDel(&ajpStrPathToCommand);
    ajStrDel(&ajpStrCommandName);
    ajStrDel(&ajpStrPathToOutFile);
    ajStrDel(&ajpStrOutFileSuffix);

    ajStrDel(&ajpStrCommandLine);

    /*  tidy up everything else... */
    ajExit();

    return 0;
}
