/* @source ehmmconvert application
**
** EMBOSS wrapper to ehmmconvert from Sean Eddy's HMMER package v.2.3.2
** Convert between profile HMM file formats.
** 
** @author Copyright (C) Jon Ison (jison@ebi.ac.uk)
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





/* @prog ehmmconvert *********************************************************
**
** EMBOSS wrapper to ehmmconvert from Sean Eddy's HMMER package v.2.3.2
** Convert between profile HMM file formats.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile oldhmmfile = NULL;
    AjPStr      format = NULL;
    AjPFile newhmmfile = NULL;

    /* Housekeeping variables */
    AjPStr        cmd = NULL;
    AjPStr        tmp = NULL;
    char       option;
    




    /* ACD file processing */
    embInitP("ehmmconvert",argc,argv,"HMMERNEW");

    oldhmmfile = ajAcdGetInfile("oldhmmfile");
    format     = ajAcdGetListSingle("format");
    newhmmfile = ajAcdGetOutfile("newhmmfile");





    /* MAIN APPLICATION CODE */
    cmd = ajStrNew();
    tmp = ajStrNew();


    /* 1. Build hmmconvert command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajFmtPrintS(&cmd, "hmmconvert");

    /* ACD option only allows one selection */
    option = ajStrGetCharFirst(format);
    if(option == 'A')
	ajStrAppendC(&cmd, " -a ");
    else if(option == 'B')
	ajStrAppendC(&cmd, " -b ");
    else if(option == 'G')
	ajStrAppendC(&cmd, " -p ");
    else if(option == 'X')
	ajStrAppendC(&cmd, " -P ");
    /* -A (append) always set but file will be wiped by EMBOSS first unless 
       append: "Y" is set for "newhmmfile" in the ACD file. */

    /* hmmer cannot append to empty file, so only set the -A (append) option
       if file was opened for appending to (and therefore was not wiped by
       EMBOSS) and is not zero size. */
    if(ajFileGetApp(newhmmfile) && (ajFileLength(ajFileNameS(newhmmfile))!=-1))
	ajStrAppendC(&cmd, " -A ");
    ajStrAppendC(&cmd, " -F ");
    ajFmtPrintAppS(&cmd, " %s %s", ajFileName(oldhmmfile), ajFileName(newhmmfile));


    /* 2. Close ACD files. */
    ajFileClose(&oldhmmfile);
    ajFileClose(&newhmmfile);


    /* 3. Call hmmconvert */
    ajFmtPrint("\n%S\n\n", cmd);
    system(ajStrGetPtr(cmd));


    /* 4. Exit cleanly */
    ajStrDel(&cmd);
    ajStrDel(&tmp);
    ajStrDel(&format);

    embExit();

    return 0;
}
