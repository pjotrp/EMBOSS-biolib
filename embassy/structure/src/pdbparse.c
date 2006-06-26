/*  @source pdbparse application
**
**  Parses PDB files and writes CCF files (clean coordinate files) for 
**  proteins.
**
**  @author: Copyright (C) Jon Ison (jison@ebi.ac.uk)
**  @@
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
**
*******************************************************************************
** 
**  PDBPARSE documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Clean text files for protein coordinate and derived data. 
**  Bioinformatics (submitted).  
**  Jon C. Ison, Amanda O'Reilly and Alan J. Bleasby
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@ebi.ac.uk.
**  
**  NOTES
**  1. Check files which produce "ATOM/SEQRES show no correspondence" 
**     message. For 1hhk the genuine sequence is given correctly when the
**     ATOM sequence is used by default, but do we need any new functionality 
**     in coorde_AtomSequences for other cases?    Check cases the parser 
**     fails on and see if we can get around them (might need to derive a 
**     sequence with and without masking of residues for which there is no 
**     C-alpha atom present. --> Need to do a rebuild for this.
******************************************************************************/



#include "emboss.h"

  


/* @prog pdbparse ***********************************************************
**
** Parses pdb files and writes cleaned-up protein coordinate files.
**
****************************************************************************/

int main(ajint argc, char **argv)
{
    AjPList    pdb_path     =NULL;  /* Path of pdb files */
    AjPStr     pdb_name     =NULL;  /* Name of pdb file  */
    AjPDir     ccf_path    =NULL;   /* Path of ccf files */
    AjPStr     ccf_name    =NULL;   /* Name of ccf file  */
    AjPStr     pdbid        =NULL;  /* PDB code          */
    AjPStr     pdbid_temp   =NULL;  /* PDB code          */
    
    AjBool     ccfnaming   =ajFalse;   
    /* True == use the pdbid code to name the output file, 
       False== use the name of the original pdb file*/
    
    /* Mask non-amino acid groups in protein chains that do not 
       contain a C-alpha atom. The group will not appear in either 
       the CO or SQ records of the clean coordinate file */
    AjBool     camask     =ajFalse;  
    
    
    /* Mask amino acids in protein chains that do not contain a 
       C-alpha atom. The amino acid will appear not appear in the 
       CO record but will still be present in the SQ record of the 
       clean coordinate file */
    AjBool     camask1    =ajFalse;   
    
    /* Mask residues or groups in protein chains with a single atom only */
    AjBool     atommask     =ajFalse;  
    
    AjPStr     temp         =NULL; /* Temp string */   
    AjPStr     msg          =NULL; /* Error message */
   AjPStr     base_name    =NULL; /* Name of pdb file w/o path or 
				      extension */
    
    
    AjPFile    pdb_inf      =NULL; /* pdb input file pointer */
    AjPFile    ccf_outf    =NULL; /* ccf output file pointer */
    AjPFile    logf         =NULL; /* log file pointer*/ 
    
    AjPPdb     pdb          =NULL; /* Pdb structure (for parsed data) */
    
    ajint      min_chain_size=0; /* Minimum length of a SEQRES chain 
				    for it to be parsed */
    ajint      max_mismatch=0; /* Max. no. residues to trim when checking
				  for missing C-terminal SEQRES residues. */
    ajint      max_trim=0;   /* Maximum number of permissible 
				  mismatches between the ATOM and 
				  SEQRES sequences */
    ajint      pos          =0; /* Location of the file extension in 
				   the pdb file name */
    
    
    
    
    /*     THIS_DIAGNOSTIC  
	   tempstr=ajStrNew();    
	   ajStrAssignC(&tempstr,     "diagnostics");
	   tempfile=ajFileNewOut(tempstr);
	   ajStrDel(&tempstr);*/
    
    
    /* Initialise strings */
    ccf_name     = ajStrNew();
    pdb_name      = ajStrNew();
    temp          = ajStrNew();
    msg           = ajStrNew();
    base_name     = ajStrNew();
    pdbid         = ajStrNew();    
    pdbid_temp    = ajStrNew();    
    
    
    /* Read data from acd */
    embInitP("pdbparse",argc,argv,"STRUCTURE");

    pdb_path     = ajAcdGetDirlist("pdbpath");
    ccf_path    = ajAcdGetOutdir("ccfoutdir");
    logf         = ajAcdGetOutfile("logfile");
    min_chain_size=ajAcdGetInt("chnsiz");
    max_mismatch  =ajAcdGetInt("maxmis");
    max_trim      =ajAcdGetInt("maxtrim");
    ccfnaming   = ajAcdGetBool("ccfnaming");
    camask     = ajAcdGetBool("camask");
    camask1    = ajAcdGetBool("camaska");
    atommask     = ajAcdGetBool("atommask");
    
    
    /* Check directories*/
    
    /*Start of main application loop*/
    while(ajListPop(pdb_path,(void **)&temp))
    {
	ajFmtPrint("Processing %S\n", temp);   
	ajFmtPrintF(logf, "%S\n", temp);    
	
	
		
	
	/* Read pdb file*/
	if((pdb_inf=ajFileNewIn(temp))==NULL)
	{
	    ajFmtPrintS(&msg, "Could not open for reading %S ", 
			temp);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "FILE_OPEN", temp); 
	    ajStrDel(&temp);	
	    continue;	    
	}
	
	
	
	/* Assign pdb id code from file name */
	ajStrAssignS(&pdbid, temp);
	ajFileDirExtnTrim(&pdbid);

	if(MAJSTRGETLEN(pdbid)>4)
	{
	    /* The file name is longer than expected (and probably contains a 
	       prefix). Take the last four characters to be the pdbid code */
	    ajStrAssignSubS(&pdbid_temp, pdbid, pos-4, pos-1);
	    ajStrAssignS(&pdbid, pdbid_temp);
	}
	else if(MAJSTRGETLEN(pdbid)<4)
	    ajFatal("Could not determine pdbid code from file name (%S)", pdbid);
	

	/* Parse pdb file and write pdb structure */
	if(!(pdb=ajPdbReadRawNew(pdb_inf, pdbid, min_chain_size, max_mismatch, 
				 max_trim, camask, camask1, atommask, logf)))
	{	
	    ajFmtPrintS(&msg, "Clean coordinate file not generated for %S", temp);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "NO_OUTPUT", temp); 
	    
	    ajFileClose(&pdb_inf);
	    ajStrDel(&temp);
	    continue;
	}
	
	
	/* Open clean coordinate file for writing*/
	if(ccfnaming)
	    ajStrAssignS(&ccf_name, pdb->Pdb);
	else
	    ajStrAssignS(&ccf_name, temp);
	ajStrFmtLower(&ccf_name);

	
	if(!(ccf_outf=ajFileNewOutDir(ccf_path, ccf_name)))
	{
	    ajFmtPrintS(&msg, "Could not open %S for writing", 
			ccf_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "FILE_OPEN", ccf_name); 
	    ajFileClose(&pdb_inf);
	    ajPdbDel(&pdb);
	    ajStrDel(&temp);	
	    continue;
	}  
	
	
	
	
	
	/* Write pdb file */
	if(!ajPdbWriteAll(ccf_outf, pdb))
	{
	    ajFmtPrintS(&msg, "Could not write file %S", ccf_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "FILE_WRITE", ccf_name); 
	    
	    ajFmtPrintS(&temp, "rm %S", ccf_name);
	    ajFmtPrint("%S", temp);
	    ajSystem(temp);

	    ajFileClose(&pdb_inf);
	    ajFileClose(&ccf_outf);
	    ajPdbDel(&pdb);
	    ajStrDel(&temp);	
	    continue;
	}
	
	
	
	/* Tidy up*/
	ajFileClose(&pdb_inf);
	ajFileClose(&ccf_outf);
	ajPdbDel(&pdb);   
	ajStrDel(&temp);	
	
	
	ajFmtPrintF(logf, "//\n");    
    }

    /*End of main application loop*/        





    /*Tidy up */
    ajListDel(&pdb_path);
    ajStrDel(&pdb_name);
    ajDirDel(&ccf_path);
    ajStrDel(&ccf_name);
    ajStrDel(&base_name);
    ajStrDel(&pdbid);
    ajStrDel(&pdbid_temp);
    ajStrDel(&msg);
    
    ajFileClose(&logf);
    
    
    
    /* DIAGNOSTIC
       ajFileClose(&tempfile);
       */
    
    /* Return */
    ajExit();
    return 0;
}	    

