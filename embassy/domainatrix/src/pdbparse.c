/*  @source pdbparse application
**
**  Parses pdb files and writes cleaned-up protein coordinate files.
**
**  @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
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
*****************************************************************************
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
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  FEATURES
**  1. Look into why d1ygpa_.pxyz residue numbers run backwards (domains from 
**     two chains).
**  2. Check "Parser and ACE groups" behaviour is as advertised (or improve 
**     the documentation if necessary) for d2fam__.pxyz.  Masking on/off for 
**     ACE group.  When masking is OFF the ACE group appears in the CO record 
**     (with a residue number of 0) but not in the SQ record.  This might 
**     cause problems !!!!!
**     These cases will arise when a non-amino acid group (e.g. ACE) appears 
**     in the CO record of a clean coordinate file with a residue number of 
**     0.  This should never really happen, but is happening when masking of 
**     non-amino acid groups w/o CA atoms is set OFF. There should never be 
**     any protein atom in a CO line with a residue number of 0.  So we need 
**     an alternative way of dealing with ACE groups when they are not 
**     masked out, either treat them as proper residues and give them a 
**     residue number, or treat them as a different type of atom than 'P', 
**     'H' or 'L'. 
**  3. Check files which produce "ATOM/SEQRES show no correspondence" 
**     message. For 1hhk the genuine sequence is given correctly when the
**     ATOM sequence is used by default, but do we need any new functionality 
**     in coorde_AtomSequences for other cases?    Check cases the parser 
**     fails on and see if we can get around them (might need to derive a 
**     sequence with and without masking of residues for which there is no 
**     C-alpha atom present. --> Need to do a rebuild for this.
****************************************************************************/





#include "emboss.h"










  


/* @prog pdbparse ***********************************************************
**
** Parses pdb files and writes cleaned-up protein coordinate files.
**
****************************************************************************/

int main(ajint argc, char **argv)
{
    AjPStr     pdb_path     =NULL; /* Path of pdb files */
    AjPStr     pdb_extn     =NULL; /* Extn. of pdb files */
    AjPStr     pdb_name     =NULL; /* Name of pdb file */
    AjPStr     cpdb_path    =NULL; /* Path of cpdb files */
    AjPStr     cpdb_extn    =NULL; /* Extn. of cpdb files */
    AjPStr     cpdb_name    =NULL; /* Name of cpdb file */
    AjPStr     pdbid        =NULL; /* PDB code */
    
    AjBool     cpdbnaming   =ajFalse;   
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
    AjPFile    cpdb_outf    =NULL; /* cpdb output file pointer */
    AjPFile    logf         =NULL; /* log file pointer*/ 
    
    AjPList    list         =NULL; /* For list of files in pdb 
				      directory */
    
    AjPPdb     pdb          =NULL; /* Pdb structure (for parsed data) */
    
    ajint      min_chain_size=0; /* Minimum length of a SEQRES chain 
				    for it to be parsed */
    ajint      max_mismatch=0; /* Maximum number of permissible 
				  mismatches between the ATOM and 
				  SEQRES sequences */
    ajint      pos          =0; /* Location of the file extension in 
				   the pdb file name */
    
    
    
    
    /*     THIS_DIAGNOSTIC  
	   tempstr=ajStrNew();    
	   ajStrAssC(&tempstr,     "diagnostics");
	   tempfile=ajFileNewOut(tempstr);
	   ajStrDel(&tempstr);*/
    
    
    /* Initialise strings */
    cpdb_path     = ajStrNew();
    cpdb_extn     = ajStrNew();
    cpdb_name     = ajStrNew();
    pdb_path      = ajStrNew();
    pdb_extn      = ajStrNew();
    pdb_name      = ajStrNew();
    temp          = ajStrNew();
    msg           = ajStrNew();
    base_name     = ajStrNew();
    pdbid         = ajStrNew();    
    
    
    /* Read data from acd */
    ajNamInit("emboss");
    ajAcdInitP("pdbparse",argc,argv,"DOMAINATRIX"); 
    pdb_path     = ajAcdGetString("pdb");
    pdb_extn     = ajAcdGetString("pdbextn");
    cpdb_path    = ajAcdGetString("cpdb");
    cpdb_extn    = ajAcdGetString("cpdbextn");
    logf         = ajAcdGetOutfile("errf");
    min_chain_size=ajAcdGetInt("chnsiz");
    max_mismatch  =ajAcdGetInt("maxmis");
    cpdbnaming   = ajAcdGetBool("cpdbnaming");
    camask     = ajAcdGetBool("camask");
    camask1    = ajAcdGetBool("camaska");
    atommask     = ajAcdGetBool("atommask");
    
    
    /* Check directories*/
    if(!ajFileDir(&pdb_path))
	ajFatal("Could not open pdb directory");
    if(!ajFileDir(&cpdb_path))
	ajFatal("Could not open cpdb directory");
    
    
    /* Create list of files in pdb directory */
    list = ajListNew();
    ajStrAssC(&temp, "*");	
    
    
    if((ajStrChar(pdb_extn, 0)!='.'))
	ajStrInsertC(&pdb_extn, 0, ".");
    ajStrApp(&temp, pdb_extn);    
    
    
    ajFileScan(pdb_path, temp, &list, ajFalse, ajFalse, NULL, NULL, 
	       ajFalse, NULL); 
    ajStrDel(&temp);
    
    
    /*Start of main application loop*/
    while(ajListPop(list,(void **)&temp))
    {
	ajFmtPrint("Processing %S\n", temp);   
	ajFmtPrintF(logf, "%S\n", temp);    
	
	
	
	
	
	/* Read pdb file*/
	if((pdb_inf=ajFileNewIn(temp))==NULL)
	{
	    ajFmtPrintS(&msg, "Could not open for reading %S ", 
			temp);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "FILE_OPEN", temp); 
	    ajStrDel(&temp);	
	    continue;	    
	}
	
	
	
	/* Assign pdb id code from file name - take the 4 characters preceeding 
	   the file extension */
	if((pos=ajStrRFindC(temp, ajStrStr(pdb_extn)))<4)
	{
	    /* The file name is too short to contain the extension and a 4 character 
	       pdbid code. Take everything to the left of the extension to be the 
	       pdbid code */
	    ajWarn("Could not determine pdbid code from file name.");
	    if(pos)
		ajStrAssS(&pdbid, temp);
	    else
		ajStrAssC(&pdbid, ".");
	}	
	/* The file name is prob. of the form 1rbp.ent */
	else 
	    ajStrAssSub(&pdbid, temp, pos-4, pos-1);
	
	
	
	/* Parse pdb file and write pdb structure */
	if(!(pdb=ajPdbReadRawNew(pdb_inf, pdbid, min_chain_size, max_mismatch, 
				 camask, camask1, atommask, logf)))
	{	
	    ajFmtPrintS(&msg, "Clean coordinate file not generated for %S", temp);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "NO_OUTPUT", temp); 
	    
	    ajFileClose(&pdb_inf);
	    ajStrDel(&temp);
	    continue;
	}
	
	
	/* Open clean coordinate file for writing*/
	if(cpdbnaming)
	{
	    ajStrAssS(&cpdb_name, cpdb_path);
	    ajStrApp(&cpdb_name, pdb->Pdb);
	    ajStrAppC(&cpdb_name, ajStrStr(cpdb_extn));
	    ajStrToLower(&cpdb_name);

	}
	else
	{
	    ajStrAssS(&base_name, temp);
	    ajStrTrim(&base_name, MAJSTRLEN(pdb_path));
	    ajStrTrim(&base_name, (-1 * MAJSTRLEN(pdb_extn)));
	    ajStrAssS(&cpdb_name, cpdb_path);
	    ajStrApp(&cpdb_name, base_name);
	    ajStrToLower(&cpdb_name);
	    ajStrAppC(&cpdb_name, ajStrStr(cpdb_extn));
	}
	
	if(!(cpdb_outf=ajFileNewOut(cpdb_name)))
	{
	    ajFmtPrintS(&msg, "Could not open %S for writing", 
			cpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "FILE_OPEN", cpdb_name); 
	    ajFileClose(&pdb_inf);
	    ajPdbDel(&pdb);
	    ajStrDel(&temp);	
	    continue;
	}  
	
	
	
	
	
	/* Write pdb file */
	if(!ajPdbWriteAll(cpdb_outf, pdb))
	{
	    ajFmtPrintS(&msg, "Could not write file %S", cpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajFmtPrintF(logf, "%-15s%S\n//\n", "FILE_WRITE", cpdb_name); 
	    
	    ajFmtPrintS(&temp, "rm %S", cpdb_name);
	    ajFmtPrint("%S", temp);
	    ajSystem(temp);

	    ajFileClose(&pdb_inf);
	    ajFileClose(&cpdb_outf);
	    ajPdbDel(&pdb);
	    ajStrDel(&temp);	
	    continue;
	}
	
	
	
	/* Tidy up*/
	ajFileClose(&pdb_inf);
	ajFileClose(&cpdb_outf);
	ajPdbDel(&pdb);   
	ajStrDel(&temp);	
	
	
	ajFmtPrintF(logf, "//\n");    
    }

    /*End of main application loop*/        





    /*Tidy up */
    ajStrDel(&pdb_path);
    ajStrDel(&pdb_extn);
    ajStrDel(&pdb_name);
    ajStrDel(&cpdb_path);
    ajStrDel(&cpdb_extn);
    ajStrDel(&cpdb_name);
    ajStrDel(&base_name);
    ajStrDel(&pdbid);
    ajStrDel(&msg);
    
    ajFileClose(&logf);
    
    ajListDel(&list);
    
    
    
    /* DIAGNOSTIC
       ajFileClose(&tempfile);
       */
    
    /* Return */
    ajExit();
    return 0;
}	    

