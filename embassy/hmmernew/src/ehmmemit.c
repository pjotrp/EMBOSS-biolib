/* @source ehmmemit application
**
** EMBOSS wrapper to ehmmemit from Sean Eddy's HMMER package v.2.3.2
** Generate sequences from a profile HMM.
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




/* @prog ehmmemit ************************************************************
**
** EMBOSS wrapper to ehmmemit from Sean Eddy's HMMER package v.2.3.2
** Generate sequences from a profile HMM.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile    hmmfile = NULL;
    ajint        seed  = 0;
    AjBool           a = ajFalse;
    AjBool           c = ajFalse;
    ajint         nseq = 0;
    AjBool           q = 0;
    AjPFile          o = NULL;

    /* Housekeeping variables */
    AjPStr        cmd = NULL;
    AjPStr        tmp = NULL;





    /* ACD file processing */
    embInitP("ehmmemit",argc,argv,"HMMERNEW");

    hmmfile = ajAcdGetInfile("hmmfile");
    seed    = ajAcdGetInt("seed");
    a       = ajAcdGetBool("a");
    c       = ajAcdGetBool("c");
    nseq    = ajAcdGetInt("nseq");
    q       = ajAcdGetBool("q");
    o       = ajAcdGetOutfile("o");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd = ajStrNew();
    tmp = ajStrNew();


    /* 2. Build hmmemit command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajFmtPrintS(&cmd, "hmmemit");
    ajFmtPrintAppS(&cmd, " --seed %d ", seed);
    if(a)
	ajStrAppendC(&cmd, " -a ");
    if(c)
	ajStrAppendC(&cmd, " -c ");
    ajFmtPrintAppS(&cmd, " -n %d ", nseq);
    if(q)
	ajStrAppendC(&cmd, " -q ");
    ajFmtPrintAppS(&cmd, " -o %s ", ajFileName(o));
    ajStrAppendS(&cmd, ajFileNameS(hmmfile));

    
    /* 3. Close ACD files. */
    ajFileClose(&hmmfile);
    ajFileClose(&o);


    /* 4. Call hmmemit */
    ajFmtPrint("\n%S\n\n", cmd);
    system(ajStrGetPtr(cmd));


    /* 5. Exit cleanly */
    ajStrDel(&cmd);
    ajStrDel(&tmp);

    embExit();

    return 0;
}
