/* @source domainsse application
**
** Adds secondary structure records to a DCF file (domain classification file).
**
** @author: Copyright (C) Amanda O'Reilly (aoreilly@hgmp.mrc.ac.uk)
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
*******************************************************************************
** 
**  ALLVERSUSALL Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  NOTES
**  SS:
**  Secondary structure string - string of secondary structure assignments
**  for all residues, calculated by the Stride programme.
**  STRIDE-assigned secondary structure: 'H' (Alpha helix), 
**     'G' (3-10 helix), 'I' (PI-helix), 
**     'E' (Extended conformation), 'B' or 'b' (Isolated bridge),
**     'T' (Turn), 'C' (Coil, none of the above)  
**  SE:
**  Secondary structure element map- string of element assignments 
**  derived from Stridewrap output.  Element length is set when running 
**  Stridewrap (default is set to 4).  Elements are made of residues with 
**  H, G, I, E structure only.
**
******************************************************************************/






#include "emboss.h"






/* @prog domainsse ************************************************************
**
** Adds secondary structure records to a DCF file (domain classification file).
** 
******************************************************************************/

int main(int argc, char **argv)
{
    ajint    num       = 0;
    AjPStr   msg       = NULL; /* Pointer to String used for messages.        */
    AjPDir   dccf      = NULL; /* Pointer to Path of dccf (clean domain) files*/
    AjPStr   dccf_name = NULL; /* Pointer to Name of dccf (clean domain) file */
   
    AjPStr   sse       = NULL; /* Pointer to Secondary structure element map. */
    AjPStr   sss       = NULL; /* Pointer to Secondary structure string.      */
    char     ss; 

    AjPFile  dccf_inf  = NULL; /* File pointer for input domain files.        */
    AjPFile  dcf_inf   = NULL; /* File pointer to input scop file.            */
    AjPFile  dcf_outf  = NULL; /* File pointer for scop output file.          */
    AjPFile  errf      = NULL; /* File pointer for output log file.           */
    
    AjPScop  scop      = NULL; /* Pointer to scopstride object.               */
    AjPPdb   pdb       = NULL; /* Pointer to pdb object.                      */
    AjPAtom  temp_atom = NULL; /* Pointer to Atom object.                     */
    AjIList  iter      = NULL;


    
    /* Initialise strings, lists,  etc */
    msg        = ajStrNew();
    dccf_name  = ajStrNew();



    /* Read data from acd */
    ajNamInit("emboss");
    ajAcdInitP("domainsse",argc,argv,"DOMAINATRIX");
    dcf_inf    = ajAcdGetInfile("dcfinfile");
    dcf_outf   = ajAcdGetOutfile("dcfoutfile");
    dccf        = ajAcdGetDirectory("dccfdir");
    errf        = ajAcdGetOutfile("errfile");



    /* Start of main application loop.
       ajScopReadC creates a Scop object from a scop file in embl-like format. */
    while((scop=(ajScopReadCNew(dcf_inf, "*"))))
    {
        /* construct name of domain coordinate file*/
	ajStrAssS(&dccf_name,scop->Entry);
	ajStrToLower(&dccf_name);

       	ajFmtPrint("Processing %S\n", scop->Entry);
        fflush(stdout);
	
	/*ajFileNewDF opens domain files dir, looks for named domain file, 
	  returns new file object */
	if(!(dccf_inf=ajFileNewDirF(dccf, dccf_name)))
	{
	    ajFmtPrintF(errf, "%-15s\n", "FILE_OPEN");
            ajFmtPrintF(errf, "Could not open dccf file %S\n", dccf_name);  
	    ajFmtPrintS(&msg, "Could not open dccf file %S\n", dccf_name);
      	    ajWarn(ajStrStr(msg));
	    ajScopDel(&scop);
	    continue; /* move on to next item in scopfile*/
	}
	
	/* Read the coordinate file for the domain. */
	if(!(pdb=ajPdbReadNew(dccf_inf)))
	{
	    ajFmtPrintF(errf, "%-15s\n", "FILE_READ"); 
            ajFmtPrintF(errf, "Could not read dccf file %S", dccf_name); 
	    ajFmtPrintS(&msg, "Could not read dccf file %S", dccf_name);
	    ajWarn(ajStrStr(msg));
	    ajFileClose(&dccf_inf);
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
	    continue; /* Move on to next item in scopfile*/
	}
       		
        /* Construct SSE string from data in pdb object. */

	/* Initialise an sse string object. */
        sse =ajStrNew();

        /* Initialise the iterator. */
        iter=ajListIter(pdb->Chains[0]->Atoms);

        /* Iiterate through the list of atoms in the Pdb object,
	   temp_atom is an AjPAtom used to point to the current atom. */
        num=0;
        while((temp_atom=(AjPAtom)ajListIterNext(iter)))
	{
	    /* If this atom is part of a new element.
	       eNum is the Serial number of the STRIDE-assigned element. */
	    if (temp_atom->eNum > num)
            {
                /* Append one instance of eType for each new element to make 
		   sse map. */
	        ajStrAppK(&sse, temp_atom->eType);
                num++;
            }
	}
	/* Add the completed sse to the scop structure. */
        ajStrAssS(&scop->Sse, sse);
	
        ajListIterFree(&iter);

        /* Construct SSS string from data in pdb object. */
       
        /* Initialise an sss string object. */
        sss =ajStrNew();

        /* Initialise the iterator. */
        iter=ajListIter(pdb->Chains[0]->Atoms);

        /* Iiterate through the list of atoms in the Pdb object. */
     	num=0;
        while((temp_atom=(AjPAtom)ajListIterNext(iter)))
	{
            /* If this atom is part of a new residue.
	       Idx is the Residue number - index into sequence. */
	    if (temp_atom->Idx > num)
            {
                /* Append one instance of eType for each new residue to make ss
		   string. */
                ss = temp_atom->eType;
                if (ss == '.')
                    ss = 'L';
	        ajStrAppK(&sss, ss);
                num = temp_atom->Idx;
            }
	}
	/* Add the completed sss to the scop structure. */
        ajStrAssS(&scop->Sss, sss);
        ajListIterFree(&iter);

	/* Write out the scop structure to the scop output file. */
	if(!ajScopWrite(dcf_outf, scop))
	{
            ajFmtPrintF(errf, "%-15s\n", "FILE_WRITE"); 
            ajFmtPrintF(errf, "Could not write scop structure for %S to scop "
			"output file\n ", dccf_name); 
	    ajFmtPrintS(&msg, "Could not write scop structure for %S to scop "
			"output file ", dccf_name);
	    ajWarn(ajStrStr(msg));
            ajFileClose(&dccf_inf);
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
            ajStrDel(&sse);
            ajStrDel(&sss);
            continue; /* Move on to next item in scopfile. */
        }
	
	ajFileClose(&dccf_inf);
       	ajPdbDel(&pdb);
	ajScopDel(&scop);
	ajStrDel(&sse);
        ajStrDel(&sss);
    }
    
    /* Print message that DOMAINSSE has finished, giving name of
       output & error files. */
    ajFmtPrint("\nDone!\n output file name: %S\n log file name: %S\n", 
	       dcf_outf->Name, errf->Name);



    
    /* Memory management. */
    ajStrDel(&msg);
    ajStrDel(&dccf_name);
    ajFileClose(&errf);   
    ajFileClose(&dcf_inf);
    ajFileClose(&dcf_outf);	    
    

    ajExit();
    return 0;
}

