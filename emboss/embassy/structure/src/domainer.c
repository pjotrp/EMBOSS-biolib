/* @source domainer application
**
** Reads CCF files (clean coordinate files) for proteins and writes CCF 
** files for domains, taken from a DCF file (domain classification file).
** 
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
*******************************************************************************
** 
**  DOMAINER documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Manuscript in preparation.
**  XXX (in preparation). 
**  Jon C. Ison  
**  
**  Email jison@ebi.ac.uk.
**  
******************************************************************************/





#include "emboss.h"







/* @prog domainer *************************************************************
**
** Reads CCF files (clean coordinate files) for proteins and writes CCF files 
** for domains, taken from a DCF file (domain classification file).
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr   cpdb_name     =NULL;	/* Name of cpdb file */
    AjPStr   pdbscop_name  =NULL;	/* Name of pdbscop file */
    AjPStr   cpdbscop_name =NULL;	/* Name of cpdbscop file */
    AjPStr   scop_name =NULL;	        /* Name of cpdbscop file */
    AjPStr   msg           =NULL;	/* Error message */
    AjPStr   temp          =NULL;	/* Error message */

    AjPDirout cpdbscop_dir =NULL;
    AjPDirout pdbscop_dir  =NULL;
    AjPDir   cpdb_dir      =NULL;
    AjPFile  scop_inf      =NULL;
    AjPFile  cpdb_inf      =NULL;
    AjPFile  pdbscop_outf  =NULL;
    AjPFile  cpdbscop_outf =NULL;
    AjPFile  errf1         =NULL;
    AjPFile  errf2         =NULL;

    AjPScop  scop=NULL;
    AjPPdb   pdb=NULL;
    AjPStr  mode       = NULL;		/* Mode of operation from acd */
    ajint    moden      = 0;            /* Mode as an integer */

    
    /* Initialise strings */
    msg           = ajStrNew();
    cpdb_name     = ajStrNew();
    pdbscop_name  = ajStrNew();
    cpdbscop_name = ajStrNew();
    scop_name = ajStrNew();
    temp          = ajStrNew();


    /* Read data from acd */
    embInitPV("domainer",argc,argv,"STRUCTURE",VERSION);

    cpdb_dir     = ajAcdGetDirectory("ccfpdir");
    mode          = ajAcdGetListSingle("mode");
    cpdbscop_dir = ajAcdGetOutdir("ccfoutdir");
    pdbscop_dir  = ajAcdGetOutdir("pdboutdir");
    scop_inf      = ajAcdGetInfile("scopfile");
    errf1         = ajAcdGetOutfile("pdblogfile");
    errf2         = ajAcdGetOutfile("cpdblogfile");
    
    if(ajStrGetCharFirst(mode)=='1')
	moden = ajPDB;
    else
	moden = ajIDX;


    /* Start of main application loop. */
    while((scop = (ajScopReadCNew(scop_inf, "*"))))
    {
	/* Write diagnostic. */
	ajFmtPrint("%S\n", scop->Entry);   

     
	/* Read clean coordinate file*/
	ajStrAssignRef(&scop_name, scop->Pdb);
	ajStrFmtLower(&scop_name);
	if(!(cpdb_inf=ajFileNewListinDirPre(cpdb_dir, scop_name)))
	{
	    ajStrAssignS(&cpdb_name, ajDirGetPath(cpdb_dir));
	    ajStrAppendS(&cpdb_name, scop_name);
	    ajStrAppendC(&cpdb_name, ".");
	    ajStrAppendS(&cpdb_name, ajDirGetExt(cpdb_dir));

	    ajFmtPrintS(&msg, "Could not open for reading cpdb file %S", 
			cpdb_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(errf1, "//\n%S\nWARN  %S not found\n", 
			scop->Entry, cpdb_name);
	    ajFmtPrintF(errf2, "//\n%S\nWARN  %S not found\n", 
			scop->Entry, cpdb_name);
	    ajScopDel(&scop);
	    continue;	    
	}
	

	/* Write pdb structure. */
	if(!(pdb=ajPdbReadFirstModelNew(cpdb_inf)))
	{
	    ajFmtPrintS(&msg, "Error reading cpdb file %S", cpdb_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(errf1, "//\n%S\nERROR %S file read error\n", 
			scop->Entry, cpdb_name);
	    ajFmtPrintF(errf2, "//\n%S\nERROR %S file read error\n", 
			scop->Entry, cpdb_name);
	    ajFileClose(&cpdb_inf);
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
	    continue;
	}
	


	/* Open pdb format file for writing. */
	ajStrAssignRef(&scop_name, scop->Entry);
	ajStrFmtLower(&scop_name);


	ajStrAssignS(&pdbscop_name, ajDiroutGetPath(pdbscop_dir));
	ajStrAppendS(&pdbscop_name, scop_name);
	ajStrAppendC(&pdbscop_name, ".");
	ajStrAppendS(&pdbscop_name, ajDiroutGetExt(pdbscop_dir));



	if(!(pdbscop_outf=ajFileNewOutNameDirS(scop_name,pdbscop_dir)))
	{
	    ajFmtPrintS(&msg, "Could not open for writing pdbscop file %S", 
			pdbscop_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(errf1, "//\n%S\nERROR %S file write error\n", 
			scop->Entry, pdbscop_name);
	    ajFileClose(&cpdb_inf);
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
	    continue;
	}   	



	/* Open embl-like format file for writing. */
	ajStrAssignS(&scop_name, scop->Entry);
	ajStrFmtLower(&scop_name);

	ajStrAssignS(&cpdbscop_name, ajDiroutGetPath(cpdbscop_dir));
	ajStrAppendS(&cpdbscop_name, scop_name);
	ajStrAppendC(&cpdbscop_name, ".");
	ajStrAppendS(&cpdbscop_name, ajDiroutGetExt(cpdbscop_dir));


	if(!(cpdbscop_outf=ajFileNewOutNameDirS(scop_name, cpdbscop_dir)))
	{
	    ajFmtPrintS(&msg, "Could not open for writing cpdbscop file %S", 
			cpdbscop_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(errf2, "//\n%S\nERROR %S file write error\n", 
			scop->Entry, cpdbscop_name);
	    ajFileClose(&pdbscop_outf);

	    ajFileClose(&cpdb_inf);
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
	    continue;
	}   	
	
	
	/* Write domain coordinate file in pdb format.  */
	if(!ajPdbWriteDomainRaw(moden, pdb, scop, pdbscop_outf, errf1))
	{
	    ajFmtPrintS(&msg, "Error writing pdbscop file %S", pdbscop_name);
	    ajWarn(ajStrGetPtr(msg));

	    ajFileClose(&pdbscop_outf);
	    ajSysFileUnlinkS(pdbscop_name);
	}



	/* Write domain coordinate file in embl-like format. */
	if(!ajPdbWriteDomain(cpdbscop_outf, pdb, scop, errf2))
	{
	    ajFmtPrintS(&msg, "Error writing cpdbscop file %S", 
			cpdbscop_name);
	    ajWarn(ajStrGetPtr(msg));

	    ajFileClose(&cpdbscop_outf);
	    ajSysFileUnlinkS(cpdbscop_name);
	}



	/* Memory management. */
	ajFileClose(&cpdb_inf);
	ajFileClose(&pdbscop_outf);
	ajFileClose(&cpdbscop_outf);
	ajScopDel(&scop);
	ajPdbDel(&pdb);

    }
    /* End of main application loop. */    
    



    /* Tidy up. */
    ajStrDel(&cpdb_name);
    ajStrDel(&pdbscop_name);
    ajStrDel(&cpdbscop_name);
    ajStrDel(&scop_name);
    ajStrDel(&msg);
    ajStrDel(&temp);
    ajFileClose(&scop_inf);
    ajFileClose(&errf1);
    ajFileClose(&errf2);

    ajDirDel(&cpdb_dir);
    ajDiroutDel(&cpdbscop_dir);
    ajDiroutDel(&pdbscop_dir);
    ajStrDel(&mode);

    embExit();
    return 0;
}	
