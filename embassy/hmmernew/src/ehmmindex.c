/* @source ehmmindex application
**
** EMBOSS wrapper to ehmmindex from Sean Eddy's HMMER package v.2.3.2
** Create a binary SSI index for an HMM database.
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





/* @prog ehmmindex ***********************************************************
**
** EMBOSS wrapper to ehmmindex from Sean Eddy's HMMER package v.2.3.2
** Create a binary SSI index for an HMM database.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile database = NULL;

    /* Housekeeping variables */
    AjPStr        cmd = NULL;
    AjPStr        tmp = NULL;





    /* ACD file processing */
    embInitP("ehmmindex",argc,argv,"HMMERNEW");

    database   = ajAcdGetInfile("database");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd = ajStrNew();
    tmp = ajStrNew();


    /* 2. Build hmmindex command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajFmtPrintS(&cmd, "hmmindex ");
    ajStrAppendC(&cmd, ajFileName(database));

    
    /* 3. Close ACD files. */
    ajFileClose(&database);


    /* 4. Call hmmindex */
    ajFmtPrint("\n%S\n\n", cmd);
    system(ajStrGetPtr(cmd));

    /* 5. Exit cleanly */
    ajStrDel(&cmd);
    ajStrDel(&tmp);
    
    ajExit();

    return 0;
}
