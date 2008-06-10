/* @source scopparse application
**
** Reads raw SCOP classification files and writes a DCF file (domain 
** classification file).
**
** @author: Copyright (C) Jon Ison (jison@ebi.ac.uk)
** @author: Copyright (C) Alan Bleasby (ableasby@ebi.ac.uk)
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
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@ebi.ac.uk.
**  
**  NOTES
**  na.
******************************************************************************/





#include "emboss.h"






/* @prog scopparse ************************************************************
**
** Converts raw scop classification files to a file in embl-like format.
**
******************************************************************************/
int main(int argc, char **argv)
{
    AjPFile inf1         = NULL;
    AjPFile inf2         = NULL;
    AjPFile outf         = NULL;
    AjPList list         = NULL;    
    AjPScop tmp          = NULL;
    AjBool  nosegments   = ajFalse;
    AjBool  nomultichain = ajFalse;
    AjBool  nominor      = ajFalse;
       




    /* Read data from acd. */
    embInitP("scopparse", argc, argv, "DOMAINATRIX");
    inf1         =  ajAcdGetInfile("classfile");
    inf2         =  ajAcdGetInfile("desinfile");
    outf         = ajAcdGetOutfile("dcffile");
    nosegments   =  ajAcdGetBoolean("nosegments");
    nomultichain =  ajAcdGetBoolean("nomultichain");
    nominor      =  ajAcdGetBoolean("nominor");


    /*
    ajFmtPrint("nosegments: %B\n", nosegments);
    ajFmtPrint("nomultichain: %B\n", nomultichain);
    ajFmtPrint("nominor: %B\n", nominor);
    */

    /* Main body of code. */
    list = ajScopReadAllRawNew(inf1, inf2, nomultichain);
    while(ajListPop(list, (void **) &tmp))
    {
	if(((!nosegments) || (tmp->N == 1)) &&
	   ((!nominor) || ((tmp->Sunid_Class == 46456) ||  /* All alpha*/
			   (tmp->Sunid_Class == 48724) ||  /* All beta */
			   (tmp->Sunid_Class == 51349) ||  /* a/b      */
			   (tmp->Sunid_Class == 53931))))  /* a+b      */
	    ajScopWrite(outf, tmp);
	ajScopDel(&tmp);
    }
    

    /* Memory management. */
    ajFileClose(&outf);
    ajFileClose(&inf1);
    ajFileClose(&inf2);
    ajListFree(&list);

    ajExit();
    return 0;
}













