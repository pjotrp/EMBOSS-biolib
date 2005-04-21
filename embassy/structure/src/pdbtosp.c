/* @source pdbtosp application
**
** Convert raw swissprot:pdb equivalence file to embl-like format.
**
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
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
******************************************************************************
** 
**  PDBTOSP Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email Jon Ison (jison@rfcgr.mrc.ac.uk)
**  
**  NOTES
**  
******************************************************************************/





#include "emboss.h"







/* @prog pdbtosp *************************************************************
**
** Convert raw swissprot:pdb equivalence file to embl-like format
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPFile    inf1    =NULL;
    AjPFile    outf    =NULL;
    AjPList    list    =NULL;  /* List of AjPPdbtosp objects */
    AjPPdbtosp  tmp    =NULL;
          

    /* Read data from acd*/
    ajNamInit("emboss");
    ajAcdInitP("pdbtosp", argc, argv, "STRUCTURE");
    inf1  =  ajAcdGetInfile("infile");
    outf  =  ajAcdGetOutfile("outfile");


    list = ajPdbtospReadAllRawNew(inf1);
    ajPdbtospWrite(outf, list);
        

    /* Clean up */
    while(ajListPop(list, (void **) &tmp))
	ajPdbtospDel(&tmp);
    ajListDel(&list);
    ajFileClose(&inf1);
    ajFileClose(&outf);

    ajExit();
    return 0;
}

