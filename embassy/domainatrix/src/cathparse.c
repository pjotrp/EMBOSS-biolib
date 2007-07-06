/* @source cathparse application
**
** Reads raw CATH classification files and writes DCF file (domain 
** classification file).
** 
**
** @author: Copyright (C) Mike Hurley
** @author: Copyright (C) Jon Ison (jison@ebi.ac.uk)
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
** 
**  
*******************************************************************************
** 
**  CATHPARSE documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@ebi.ac.uk.
**  
**  NOTES
**  na.
****************************************************************************/





#include "emboss.h"






/* @prog cathparse **********************************************************
**
** Reads raw CATH classification files and writes DCF file (domain 
** classification file).
**
*****************************************************************************/

int main(int argc, char **argv)
{
    AjPFile listf    = NULL; /* File to be read - caths.list.v2.4.      */
    AjPFile domf     = NULL; /* File to be read - domlist.v2.4.         */
    AjPFile namesf   = NULL; /* File to be read - CAT.names.all.v2.4.   */
    AjPFile outf     = NULL; /* Output file.                            */
    AjPFile logf     = NULL; /* Log file.                               */
    AjPList list     = NULL; /* List of CATH objects.                   */
    AjPCath tmp      = NULL; /* Temp. pointer.                          */
    
    
    embInitP("cathparse", argc, argv, "DOMAINATRIX");

    listf  =  ajAcdGetInfile("listfile");   
    domf   =  ajAcdGetInfile("domfile");    
    namesf =  ajAcdGetInfile("namesfile");  
    outf   =  ajAcdGetOutfile("outfile");   
    logf   =  ajAcdGetOutfile("logfile");   

    
    list = ajCathReadAllRawNew(listf, domf, namesf, logf);
    while(ajListPop(list, (void **) &tmp))
    {	
	ajCathWrite(outf, tmp);
	ajCathDel(&tmp);
    }



    /* Close CATH parsable files and output file */
    ajFileClose(&listf); 
    ajFileClose(&domf); 
    ajFileClose(&namesf); 
    ajFileClose(&outf); 
    ajFileClose(&logf); 
    ajListFree(&list);

    ajExit();
    return 0;
}










