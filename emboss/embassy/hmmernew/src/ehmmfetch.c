/* @source ehmmfetch application
**
** EMBOSS wrapper to ehmmfetch from Sean Eddy's HMMER package v.2.3.2
** Retrieve an HMM from an HMM database.
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





/* @prog ehmmfetch ***********************************************************
**
** EMBOSS wrapper to ehmmfetch from Sean Eddy's HMMER package v.2.3.2
** Retrieve an HMM from an HMM database.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile database = NULL;
    AjPStr      name = NULL;
    AjBool      nhmm = ajFalse;
    AjPStr  outfname = NULL;

    /* Housekeeping variables */
    AjPStr        cmd = NULL;





    /* ACD file processing */
    embInitPV("ehmmfetch",argc,argv,"HMMERNEW",VERSION);

    database   = ajAcdGetInfile("database");
    name       = ajAcdGetString("name");
    nhmm       = ajAcdGetBoolean("nhmm");
    outfname   = ajAcdGetOutfileName("outfile");
    

    


    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd = ajStrNew();


    /* 2. Build hmmfetch command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajStrAssignS(&cmd, ajAcdGetpathC("hmmfetch"));
    if(nhmm)
	ajStrAppendC(&cmd, " -n ");
    /* Note the output redirected to outfname  */
    ajFmtPrintAppS(&cmd, " %S %S", 
		   ajFileGetNameS(database),
		   name);


    /* 3. Close ACD files. */
    ajFileClose(&database);


    /* 4. Call hmmfetch.  Use C system call instead of ajSystem
       so that redirect in cmd works ok. */
    ajFmtPrint("\n%S > %S\n\n", cmd, outfname);
    ajSysExecOutnameAppendS(cmd, outfname);
    
    /* 5. Exit cleanly */
    ajStrDel(&name);
    ajStrDel(&cmd);
    ajStrDel(&outfname);

    embExit();

    return 0;
}
