/* @source domainer application
**
** Reads protein coordinate files and writes domain coordinate files.
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
*****************************************************************************
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
**  Email jison@rfcgr.mrc.ac.uk.
**  
****************************************************************************/





#include "emboss.h"







/* @prog domainer *************************************************************
**
** Reads protein coordinate files and writes domain coordinate files.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr   cpdb_name     =NULL;	/* Name of cpdb file */
    AjPStr   pdbscop_name  =NULL;	/* Name of pdbscop file */
    AjPStr   cpdbscop_name =NULL;	/* Name of cpdbscop file */
    AjPStr   scop_name =NULL;	/* Name of cpdbscop file */
    AjPStr   msg           =NULL;	/* Error message */
    AjPStr   temp          =NULL;	/* Error message */

    AjPDir   cpdbscop_dir  =NULL;
    AjPDir   pdbscop_dir   =NULL;
    AjPDir   cpdb_dir      =NULL;
    AjPFile  scop_inf      =NULL;
    AjPFile  cpdb_inf      =NULL;
    AjPFile  pdbscop_outf  =NULL;
    AjPFile  cpdbscop_outf =NULL;
    AjPFile  errf1         =NULL;
    AjPFile  errf2         =NULL;

    AjPScop  scop=NULL;
    AjPPdb   pdb=NULL;
    AjPStr  *mode       = NULL;		/* Mode of operation from acd*/
    ajint    moden      = 0;            /* Mode as an integer */

    
    /* Initialise strings */
    msg           = ajStrNew();
    cpdb_name     = ajStrNew();
    pdbscop_name  = ajStrNew();
    cpdbscop_name = ajStrNew();
    scop_name = ajStrNew();
    temp          = ajStrNew();


    /* Read data from acd */
    ajNamInit("emboss");
    ajAcdInitP("domainer",argc,argv,"DOMAINATRIX"); 
    cpdb_dir     = ajAcdGetDirectory("cpdb");
/*
    cpdb_extn     = ajAcdGetString("cpdbextn");
    cpdbscop_extn = ajAcdGetString("cpdbscopextn");
    pdb_extn      = ajAcdGetString("pdbextn");
*/
    mode          = ajAcdGetList("mode");
    cpdbscop_dir = ajAcdGetOutdir("cpdboutdir");
    pdbscop_dir  = ajAcdGetOutdir("pdboutdir");
    scop_inf      = ajAcdGetInfile("scopfile");
    errf1         = ajAcdGetOutfile("pdberrfile");
    errf2         = ajAcdGetOutfile("cpdberrfile");
    
    if(ajStrChar(*mode,0)=='1')
	moden = ajPDB;
    else
	moden = ajIDX;


    /*Start of main application loop*/
    while((scop = (ajScopReadCNew(scop_inf, "*"))))
    {
	/* Write diagnostic */
	ajFmtPrint("%S\n", scop->Entry);   

     
	/* Read clean coordinate file*/
	ajStrAss(&scop_name, scop->Pdb);
	ajStrToLower(&scop_name);
	if(!(cpdb_inf=ajFileNewDirF(cpdb_dir, scop_name)))
	{
	    ajFileNameDir(&cpdb_name, cpdb_dir, scop_name);
	    ajFmtPrintS(&msg, "Could not open for reading cpdb file %S", 
			cpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(errf1, "//\n%S\nWARN  %S not found\n", 
			scop->Entry, cpdb_name);
	    ajFmtPrintF(errf2, "//\n%S\nWARN  %S not found\n", 
			scop->Entry, cpdb_name);
	    /* ajFileClose(&cpdb_inf); */
	    ajScopDel(&scop);
	    continue;	    
	}
	
	
	/* Write pdb structure */
	if(!(pdb=ajPdbReadNew(cpdb_inf)))
	{
	    ajFmtPrintS(&msg, "Error reading cpdb file %S", cpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(errf1, "//\n%S\nERROR %S file read error\n", 
			scop->Entry, cpdb_name);
	    ajFmtPrintF(errf2, "//\n%S\nERROR %S file read error\n", 
			scop->Entry, cpdb_name);
	    ajFileClose(&cpdb_inf);
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
	    continue;
	}
	


	/* Open pdb format file for writing*/
	ajStrAss(&scop_name, scop->Entry);
	ajStrToLower(&scop_name);

	ajFileNameDir(&pdbscop_name, pdbscop_dir, scop_name);
	if(!(pdbscop_outf=ajFileNewOutDir(pdbscop_dir,scop_name)))
	{
	    ajFmtPrintS(&msg, "Could not open for writing pdbscop file %S", 
			pdbscop_name);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(errf1, "//\n%S\nERROR %S file write error\n", 
			scop->Entry, pdbscop_name);
	    ajFileClose(&cpdb_inf);
/*	    ajFileClose(&pdbscop_outf); */
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
	    continue;
	}   	


	/* Open embl-like format file for writing*/
	ajStrAssS(&scop_name, scop->Entry);
	ajStrToLower(&scop_name);
	ajFileNameDir(&cpdbscop_name, cpdbscop_dir, scop_name);

	if(!(cpdbscop_outf=ajFileNewOutDir(cpdbscop_dir, scop_name)))
	{
	    ajFmtPrintS(&msg, "Could not open for writing cpdbscop file %S", 
			cpdbscop_name);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(errf2, "//\n%S\nERROR %S file write error\n", 
			scop->Entry, cpdbscop_name);
	    ajFileClose(&cpdb_inf);
	    ajFileClose(&pdbscop_outf);
	    ajScopDel(&scop);
	    ajPdbDel(&pdb);
	    continue;
	}   	
	
	
	/* Write domain coordinate file in pdb format */
	if(!ajPdbWriteDomainRaw(moden, pdb, scop, pdbscop_outf, errf1))
	{
	    ajFmtPrintS(&msg, "Error writing pdbscop file %S", pdbscop_name);
	    ajWarn(ajStrStr(msg));

	    ajFileClose(&pdbscop_outf);
	    ajFmtPrintS(&temp, "rm %S\n", pdbscop_name);
	    ajFmtPrint("%S", temp);
	    ajSystem(temp);

	}


	/* Write domain coordinate file in embl-like format */
	if(!ajPdbWriteDomain(cpdbscop_outf, pdb, scop, errf2))
	{
	    ajFmtPrintS(&msg, "Error writing cpdbscop file %S", 
			cpdbscop_name);
	    ajWarn(ajStrStr(msg));

	    ajFileClose(&cpdbscop_outf);
	    ajFmtPrintS(&temp, "rm %S\n", cpdbscop_name);
	    ajFmtPrint("%S", temp);
	    
	    ajSystem(temp);
	}


	/* Tidy up*/
	ajFileClose(&cpdb_inf);
	ajFileClose(&pdbscop_outf);
	ajFileClose(&cpdbscop_outf);
	ajScopDel(&scop);
	ajPdbDel(&pdb);
    }
    /*End of main application loop*/    
    




    /*Tidy up */
    ajStrDel(&cpdb_name);
    ajStrDel(&pdbscop_name);
    ajStrDel(&cpdbscop_name);
    ajStrDel(&scop_name);
    ajStrDel(&msg);
    ajStrDel(&temp);
    ajFileClose(&scop_inf);
    ajFileClose(&errf1);
    ajFileClose(&errf2);


    /* Bye Bye */
    ajExit();
    return 0;
}	
