/* @source scopparse application
**
** Converts raw scop classification files to a file in embl-like format.
**
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
** @author: Copyright (C) Alan Bleasby (ableasby@hgmp.mrc.ac.uk)
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
**  SCOPPARSE documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Manuscript in preparation.
**  XXX (in preparation). 
**  Jon C. Ison & Alan J. Bleasby  
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
****************************************************************************/

#include "emboss.h"





/* @prog scopparse **********************************************************
**
** Converts raw scop classification files to a file in embl-like format.
**
*****************************************************************************/
int main(int argc, char **argv)
{
    AjPFile inf1      = NULL;
    AjPFile inf2      = NULL;
    AjPFile outf      = NULL;
    AjBool  outputall = ajFalse;
    AjPList list      = NULL;    
    AjPScop tmp       = NULL;
    
       




    /* Read data from acd*/
    ajNamInit("emboss");
    ajAcdInitP("scopparse", argc, argv, "DOMAINATRIX");
    inf1      =  ajAcdGetInfile("infilea");
    inf2      =  ajAcdGetInfile("infileb");
    outf      = ajAcdGetOutfile("outfile");
    outputall =  ajAcdGetBool("outputall");





    /* Main body of code */
    list = ajScopReadAllRawNew(inf1, inf2, outputall);
    while(ajListPop(list, (void **) &tmp))
    {
	ajScopWrite(outf, tmp);
	ajScopDel(&tmp);
    }
    

    /* Clean up */
    ajFileClose(&outf);
    ajFileClose(&inf1);
    ajFileClose(&inf2);
    ajListDel(&list);

    ajExit();
    return 0;
}




