/* @Source pdbplus application
**
** Add records for secondary structure and residue accessibility to a
** coordinate file.
** 
**
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
** @author: Copyright (C) Amanda O'Reilly (aoreilly@hgmp.mrc.ac.uk)
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
**  PDBPLUS documentation
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
****************************************************************************/





#include "emboss.h"





/* Function prototypes. */
void     pdbplus_writeElement(int start, int end, int eNum, AjPAtom *arr);
void     pdbplus_sort(AjPPdb pdb, int tS);
AjBool   pdbplus_CpdbReadOldTwo(AjPFile inf, AjPPdb *thys);





/* @prog pdbplus *************************************************************
**
** pdbplus reads a directory of domain or protein coordinate files and writes
** a directory containing the same files as the input directory but in which 
** values for records for secondary structure and residue accessibility are
** given. These data are calculated by using the STRIDE and NACCESS programs.
**
*****************************************************************************/

int main(ajint argc, char **argv)
{
    AjPStr   pdb_path        = NULL;  /* Path of pdb input files.           */
    AjPStr   pdb_extn        = NULL;  /* Extn. of pdb input files.          */
    AjPStr   pdb_prefix      = NULL;  /* Prefix of pdb input files.         */
    AjPStr   in_EMBLpdbpath  = NULL;  /* Path of coordinate input file.     */
    AjPStr   in_EMBLpdbextn  = NULL;  /* Extn. of coordinate input file.    */
    AjPStr   out_EMBLpdbpath = NULL;  /* Path of coordinate output file.    */
    AjPStr   out_EMBLpdbextn = NULL;  /* Extn. of coordinate output file.   */
    AjPStr   pdb_name        = NULL;  /* Full name (path/name/extension) of 
					 pdb format input file.             */
    AjPStr   out_name        = NULL;  /* Full name (path/name/extension) of 
					 clean format output file.          */
    AjPStr   randomname      = NULL;  /* Name for temp file tempf.          */
    AjPStr   tempstr         = NULL; 
    AjPStr   exec            = NULL; 
    AjPStr   naccess_str     = NULL; 
    AjPStr   line            = NULL;
    AjPStr   syscmd          = NULL;  /* Command line arguments.            */
    AjPStr  *mode            = NULL;  /* Mode of operation from acd.        */

    AjPFile  errf        = NULL;  /* pdbplus error file pointer.            */
    AjPFile  serrf       = NULL;  /* stride error file pointer.             */
    AjPFile  nerrf       = NULL;  /* stride error file pointer.             */
    AjPFile  tempf       = NULL;  /* Temp file for holding STRIDE output.   */
    AjPFile  cpdb_inf    = NULL;  /* Protein coordinate input file.         */
    AjPFile  cpdb_outf   = NULL;  /* Protein coordinate output file.        */
 
    AjPList  cpdblist    = NULL;  /* List of filenames in in_EMBLpdbpath.   */
    AjIList  iter        = NULL;

    AjBool   done_naccess= ajFalse;
    AjBool   done_stride = ajFalse;
    AjBool   found       = ajFalse;
    AjPAtom  temp_atom   = NULL;  /* Pointer to Atom object.                */
    AjPPdb   pdb_old     = NULL;  /* Pointer to PDB object - without new
				     stride elements. */
    AjPPdb   pdb         = NULL;  /* Pointer to PDB object. */
    ajint    idn         = 0;     /* Chain identifier as a number (1,2,...) */
    ajint    chain_num   = 0;     /* Chain identifier index (0,1,...).      */
    ajint    tS          = 0;     /* User-defined threshold size for SSEs.  */
    ajint    nostride    = 0;     /* No. times stride failed */
    ajint    nonaccess   = 0;     /* No. times naccess failed */
    ajint    nofile      = 0;     /* No. times of file error */

    /* Variables for each item that will be parsed from the ASG line. */
    AjPStr   res      = NULL;  /* Residue id from STRIDE ASG line (ALA etc). */
    AjPStr   res_num  = NULL;  /* PDB residue number from STRIDE ASG line.   */
    char     pcid     = ' ';   /* Protein chain identifier from STRIDE or 
				  NACESS output (A,B, etc).                  */
    char     ss       = ' ';   /* One-letter secondary structure code from 
				  STRIDE ASG line.                           */
    float    ph       = 0.0;   /* Phi angle from STRIDE ASG line.            */
    float    ps       = 0.0;   /* Psi angle from STRIDE ASG line.            */
    float    sa       = 0.0;   /* Residue solvent accessible area from STRIDE 
				  ASG line.                                  */
    float    f1       = 0;
    float    f2       = 0;
    float    f3       = 0;
    float    f4       = 0;
    float    f5       = 0;
    float    f6       = 0;
    float    f7       = 0;
    float    f8       = 0;
    float    f9       = 0;
    float    f10      = 0;





    /* Allocate strings; this section is used for variables that are 
       allocated once only. */
    pdb_name       = ajStrNew();
    out_name       = ajStrNew();
    res            = ajStrNew();
    res_num        = ajStrNew();
    randomname     = ajStrNew();
    syscmd         = ajStrNew();
    line           = ajStrNew();  
    naccess_str    = ajStrNew();
    exec           = ajStrNew();





    /* Read data from acd. */
    ajNamInit("emboss");
    ajAcdInitP("pdbplus",argc,argv,"DOMAINATRIX");

    pdb_path        = ajAcdGetString("pdbpath"); 
    pdb_extn        = ajAcdGetString("pdbextn");
    pdb_prefix      = ajAcdGetString("pdbprefix");
    in_EMBLpdbpath  = ajAcdGetString("inputemblpdbpath");  
    in_EMBLpdbextn  = ajAcdGetString("inputemblpdbextn");
    out_EMBLpdbpath = ajAcdGetString("outputemblpdbpath");
    out_EMBLpdbextn = ajAcdGetString("outputemblpdbextn");
    mode            = ajAcdGetList("mode");
    errf            = ajAcdGetOutfile("errfile");
    if(ajStrChar(*mode, 0) != '2')
      serrf         = ajAcdGetOutfile("serrfile");
    if(ajStrChar(*mode, 0) != '1')
      nerrf         = ajAcdGetOutfile("nerrfile");
    tS              = ajAcdGetInt("thresholdsize");
 
    /* 
    ** Check validity of paths.  ajFileDir() checks string is valid 
    ** directory & appends trailing '/' if necessary.
    ** Check file extensions. 
    */
    if(!ajFileDir(&pdb_path))
      ajFatal("Could not open %S\n", pdb_path);
    if(!ajFileDir(&in_EMBLpdbpath))
      ajFatal("Could not open %S\n", in_EMBLpdbpath);      
    if(!ajFileDir(&out_EMBLpdbpath))
      ajFatal("Could not open %S\n", out_EMBLpdbpath);      
    if(!(pdb_extn)) 
      ajFatal("File extension was NULL\n");
    if(!(in_EMBLpdbextn))
      ajFatal("File extension was NULL\n");
    if(!(out_EMBLpdbextn))
      ajFatal("File extension was NULL\n");

    
    if((!ajFileDir(&pdb_path))        || 
       (!ajFileDir(&in_EMBLpdbpath))  || 
       (!ajFileDir(&out_EMBLpdbpath)) || 
       (!(pdb_extn))                  || 
       (!(in_EMBLpdbextn))            || 
       (!(out_EMBLpdbextn)))
    {
      ajStrDel(&pdb_path);
      ajStrDel(&pdb_extn);
      ajStrDel(&pdb_prefix);
      ajStrDel(&in_EMBLpdbpath);
        ajStrDel(&in_EMBLpdbextn);
        ajStrDel(&out_EMBLpdbpath);
        ajStrDel(&out_EMBLpdbextn);
        ajStrDel(&pdb_name);
        ajStrDel(&out_name);
        ajStrDel(&res);
        ajStrDel(&randomname);
        ajStrDel(&syscmd);
        ajStrDel(&line);
        ajStrDel(&naccess_str);
        ajFileClose(&errf);
	if(ajStrChar(*mode, 0) != '2')
	  ajFileClose(&serrf);
	if(ajStrChar(*mode, 0) != '1')
	  ajFileClose(&nerrf);
  
        ajWarn("Could not open one of the specified directories or file "
               "extension NULL");
        ajExit();
        return 1;
    }

    /* NACCESS does *not* generate an output file if the path is './' e.g. 
       naccess ./1rbp.ent , therefore replace './' with null. */
    if(ajStrMatchC(pdb_path, "./") ||
       ajStrMatchC(pdb_path, "."))
	ajStrAssC(&pdb_path, "");
    
    /* Add a '.' to extensions if one does not already exist. */
    if((ajStrChar(pdb_extn, 0) != '.'))
        ajStrInsertC(&pdb_extn, 0, ".");
    if((ajStrChar(in_EMBLpdbextn, 0) != '.'))        
        ajStrInsertC(&in_EMBLpdbextn, 0, ".");
    if((ajStrChar(out_EMBLpdbextn, 0) != '.'))        
        ajStrInsertC(&out_EMBLpdbextn, 0, ".");

    
    /*Create list of file names (AjPStr's) in in_EMBLpdbpath  directory. */   
    cpdblist = ajListNew();
    tempstr = ajStrNew();
    ajStrAssC(&tempstr, "*"); 
    ajStrApp(&tempstr, in_EMBLpdbextn); 
    ajFileScan(in_EMBLpdbpath, tempstr, &cpdblist, ajFalse, ajFalse, NULL, 
               NULL, ajFalse, NULL);
    ajStrDel(&tempstr);

    ajRandomSeed();
    ajStrAssC(&randomname, ajFileTempName(NULL)); 




    
    /* 
    **  Start of main application loop. 
    **  Process each PDB/ protein coordinate file (EMBL format) in turn. 
    */ 
    
    while(ajListPop(cpdblist,(void **)&tempstr))
    {
        /* Open protein coordinate file.  If it cannot be opened, write a 
           message to the error file, delete tempstr and continue. */

        if((cpdb_inf = ajFileNewIn(tempstr)) == NULL)   
	{
	    ajWarn("%s%S\n//\n", 
		   "clean coordinate file not found: ", tempstr);
	    
	    ajFmtPrintF(errf, "%s%S\n//\n", 
                        "clean coordinate file not found: ", tempstr); 
            ajStrDel(&tempstr); 
	    nofile++;
	    continue; 
        }       

        ajFmtPrint("Processing %S\n", tempstr);
	fflush(stdout);

        /* Parse protein coordinate data (from clean format file) into 
	   AjPPdb object.  ajPdbRead will create the AjPPdb object. */
      if(!(pdb_old=ajPdbReadNew(cpdb_inf)))
        {
	    ajWarn("ERROR Clean coordinate file read" 
		   "error: %S\n//\n", tempstr);
            ajFmtPrintF(errf, "ERROR Clean coordinate file read" 
			"error: %S\n//\n", tempstr);
            ajFileClose(&cpdb_inf);
            ajStrDel(&tempstr); 
	    nofile++;
            continue;
        }

        ajFileClose(&cpdb_inf);
        ajPdbCopy(&pdb, pdb_old);
        ajPdbDel(&pdb_old); 

        /* Construct name of corresponding PDB file. */
	ajStrAssS(&pdb_name, pdb_path);
        ajStrApp(&pdb_name, pdb_prefix);
	ajStrToLower(&pdb->Pdb);
        ajStrApp(&pdb_name, pdb->Pdb);
	ajStrApp(&pdb_name, pdb_extn);

        /* Check corresponding PDB file exists for reading using ajFileStat. */
	if(!(ajFileStat(pdb_name, AJ_FILE_R )))
        {
            ajFmtPrintF(errf, "%s%S\n//\n", "PDB file not found: ", pdb_name);
            ajWarn("%s%S\n//\n", "PDB file not found: ", pdb_name);
            ajStrDel(&tempstr); 
            ajPdbDel(&pdb);
	    nofile++;
            continue;
        }
        
	if(ajStrChar(*mode, 0) != '2')
        {        
	    /* 
	    **  Create a string containing the STRIDE command line (it needs
	    **  PDB file name & name of temp output file).
	    **  Call STRIDE by using ajSystem.
	    */
	    
	    ajFmtPrintS(&syscmd, "stride %S -f%S >> %s 2>&1",  
			pdb_name, randomname, ajFileName(serrf));
	    ajFmtPrint("stride %S -f%S >> %s 2>&1\n",  
		       pdb_name, randomname,ajFileName(serrf));
	    system(ajStrStr(syscmd));
	    
	    /* Open the stride output file */
	    if (((tempf = ajFileNewIn(randomname)) == NULL))
	    {
		ajWarn("%s%S\n//\n", 
		       "no stride output for: ", pdb_name); 
		ajFmtPrintF(errf, "%s%S\n//\n", 
			    "no stride output for: ", pdb_name); 
		nostride++;
		ajStrDel(&tempstr);
		ajPdbDel(&pdb); 
		continue; 
	    } 
	    else
	      ajFmtPrintF(errf, "%s%S\n//\n", 
			  "stride output for: ", pdb_name); 

	    
	    done_stride = ajFalse;

	    /* Parse STRIDE output from temp output file a line at a time. */
	    while(ajFileReadLine(tempf,&line))
	    {       
		if(ajStrPrefixC(line,"ASG"))    
		{
		    ajFmtScanS(line, "%*S %S  %c %S %*d %c %*S %f %f %f %*S", 
			       &res, &pcid, &res_num, &ss, &ph, &ps, &sa);
                
		    /* 
		    **  Populate pdbplus object with the data from this parsed
		    **  line. This means first identifying the chain, then 
		    **  finding all the atoms corresponding to the residue. 
		    */
                
		    /* Determine the chain number. ajDmxPdbplusChain does not 
		       recognise '-', so change '-' to '.'  */
		    if (pcid == '-')
			pcid = '.'; 

		    /* Get chain number from the chain identifier. */
		    if(!ajPdbChnidToNum(pcid, pdb, &idn)) 
		    {
			ajWarn("Could not convert chain id %c to chain"
			       " number in pdb file %S\n//\n", 
			       pcid, pdb_name);
			ajFmtPrintF(errf, "Could not convert chain id %c "
				    "to chain number in pdb file %S\n//\n", 
				    pcid, pdb_name);
			continue;
		    }
                    
		    /* 
		    **  The chain number that will get written starts at 1, but
		    **  we want an index into an array which must start at 0, 
		    **  so subtract 1 from the chain number to get the index. 
		    */
		    chain_num = idn-1; 
                  
		    /* 
		    **   Iiterate through the list of atoms in the Pdb object,
		    **   found switches to true when first atom corresponding 
		    **   to the line is found. 
		    */

		    iter = ajListIterRead(pdb->Chains[chain_num]->Atoms);
		    found = ajFalse; 
		    while((temp_atom = (AjPAtom)ajListIterNext(iter)))
		    {
		        /* If we have found the atom we want */
			if((ajStrMatch(res_num, temp_atom->Pdb) && 
			    ajStrMatch(res, temp_atom->Id3)))
			{
                       	    done_stride = ajTrue;
			    found = ajTrue;
			    temp_atom->eStrideType = ss;
			    temp_atom->Phi  = ph;
			    temp_atom->Psi  = ps;
			    temp_atom->Area = sa;
			}                 
			/* If the matching atoms have all been processed
			  move on to next ASG line, next residue. */
			else if(found == ajTrue) 
			    break;	
			else 
			/* Matching atoms not found yet. */       
			    continue;	
		    }
		    ajListIterFree(&iter);
		} /* End of if ASG loop. */ 
	    } /* End of while line loop. */
	    

	    if(done_stride)
	      ajFmtPrintF(errf, "%s%S\n//\n", 
			  "stride data for: ", pdb_name); 
	    else
	      {
		ajFmtPrintF(errf, "%s%S\n//\n", 
			    "no stride data for: ", pdb_name); 
		ajWarn("%s%S\n//\n", "no stride data for: ", pdb_name);
		nostride++;
	      }


	    /* Close STRIDE temp file. & tidy up. */
	    ajFileClose(&tempf);

	    /* Remove temporary file (stride output file). */
	    ajFmtPrintS(&exec, "rm %S", randomname); 
	    ajSystem(exec); 
	    
	    /* 
	    **  Calculate element serial numbers (eStrideNum)& ammend atom
	    **  objects, count no's of elements and ammend chain object 
	    **  (numHelices, num Strands). 
	    */
	    pdbplus_sort(pdb, tS);
	}
	

	if(ajStrChar(*mode, 0) != '1')
        {        
	    /* 
	    **   Create a string containing the NACCESS command line (it needs
	    **   PDB file name & name of temp output file) & call NACCESS.
	    **   If e.g. /data/structure/pdbfred.ent was parsed and the program
	    **   was run from /stuff, then /stuff/fred.asa and /stuff/fred.rsa
	    **   would be written.  These must be deleted once parsed (only
	    **   use the .rsa file here). 
	    */
	    
	    ajFmtPrintS(&syscmd, "naccess %S  >> %s 2>&1",  
			pdb_name, 
			ajFileName(nerrf));
	    ajFmtPrint("naccess %S  >> %s 2>&1\n",  
		       pdb_name, 
		       ajFileName(nerrf));
	    system(ajStrStr(syscmd));
	    
	    ajStrAssS(&naccess_str, pdb_prefix);
	    ajStrApp(&naccess_str, pdb->Pdb);
	    ajStrAppC(&naccess_str, ".rsa");
	    
	    /* Open the NACCESS output file. */
	    if (((tempf = ajFileNewIn(naccess_str)) == NULL))
	    {
		ajFmtPrintF(errf, "%s%S\n//\n", 
			    "no naccess output for: ", pdb_name); 
		ajWarn("%s%S\n//\n", "no naccess output for: ", pdb_name);
		nonaccess++;
		ajStrDel(&tempstr);
		ajPdbDel(&pdb); 
		continue; 
	    }	 
	    else
	      ajFmtPrintF(errf, "%s%S\n//\n", 
			  "naccess output for: ", pdb_name); 


	    done_naccess = ajFalse;
	    /* Parse NACCESS output from temp output file a line at a time. */	    
	    while(ajFileReadLine(tempf,&line))
	    {       
		if(ajStrPrefixC(line,"RES"))    
		{
		    /* Read data from lines. */
		    if((pcid = line->Ptr[8]) == ' ')
		      ajFmtScanS(line, "%*S %S %S %f %f %f "
				 "%f %f %f %f %f %f %f", 
				 &res, &res_num, &f1, &f2, &f3, &f4, &f5, 
				 &f6, &f7, &f8, &f9, &f10);
		    else
		      ajFmtScanS(line, "%*S %S %*c %S %f %f "
				 "%f %f %f %f %f %f %f %f", 
				 &res, &res_num, &f1, &f2, &f3, &f4, &f5, 
				 &f6, &f7, &f8, &f9, &f10);

		    /* Identify the chain, then finding all 
		       the atoms corresponding to the residue. */
                
		    /* Get the chain number from the chain identifier. */
		    if(!ajPdbChnidToNum(pcid, pdb, &idn))
		    {
                        ajWarn("Could not convert chain id %c to chain"
				    " number in pdb file %S\n//\n", 
			       pcid, pdb_name);	
			ajFmtPrintF(errf, "Could not convert chain id"
				    " %c to chain number in pdb file %S\n//\n",
				    pcid, pdb_name);
			continue;
		    }
                    

                  
		    /* 
		    **  Chain number will start at 1, but we want an index 
		    **  into an array which must start at 0, so subtract 1 
		    **  from the chain number to get the index.
		    */
		    chain_num = idn-1; 



		    /* 
		    **   Iiterate through the list of atoms in the Pdb object,
		    **   temp_atom is an AjPAtom used to point to the current
		    **   atom.
		    **   ajBool found switches to true when first atom 
		    **   corresponding to the line is found. 
		    */
		    iter = ajListIterRead(pdb->Chains[chain_num]->Atoms);

		    found = ajFalse; 
		    while((temp_atom = (AjPAtom)ajListIterNext(iter)))
		    {
			/* If we have found the atom we want, write the atom 
			   object. */
			if((ajStrMatch(res_num, temp_atom->Pdb) && 
			    ajStrMatch(res, temp_atom->Id3)))
                        {
			    found = ajTrue;
			    done_naccess = ajTrue;
			    temp_atom->all_abs  = f1;
			    temp_atom->all_rel  = f2;
			    temp_atom->side_abs = f3;
			    temp_atom->side_rel = f4;
			    temp_atom->main_abs = f5;
			    temp_atom->main_rel = f6;
			    temp_atom->npol_abs = f7;
			    temp_atom->npol_rel = f8;
			    temp_atom->pol_abs  = f9;
			    temp_atom->pol_rel  = f10;

			}      
			/* If the matching atoms have all been processed. 
			   move on to next ASG line, next residue. */
			else if(found == ajTrue) 
			    break;	
			else 
			    /* Matching atoms not found yet,      
			       move on to next atom. */
			    continue;	 
		    }
		    ajListIterFree(&iter);
		} 
	    } 
	    
	    if(done_naccess)
	      ajFmtPrintF(errf, "%s%S\n//\n", 
			  "naccess data for: ", pdb_name); 
	    else
	      {
		ajFmtPrintF(errf, "%s%S\n//\n", 
			    "no naccess data for: ", pdb_name); 
		ajWarn("%s%S\n//\n", "no naccess data for: ", pdb_name);
		nonaccess++;
	      }

	    /* Remove temporary file (naccess output files). */
	    ajFileClose(&tempf);
	    
	    ajFmtPrintS(&exec, "rm %S", naccess_str); 
	    ajSystem(exec); 

	    ajStrAssS(&naccess_str, pdb_prefix);
	    ajStrApp(&naccess_str, pdb->Pdb);
	    ajStrAppC(&naccess_str, ".asa");
	    ajFmtPrintS(&exec, "rm %S", naccess_str);
	    ajSystem(exec); 

	    ajStrAssS(&naccess_str, pdb_prefix);
	    ajStrApp(&naccess_str, pdb->Pdb);
	    ajStrAppC(&naccess_str, ".log");
	    ajFmtPrintS(&exec, "rm %S", naccess_str);
	    ajSystem(exec); 
	}

        /* Construct name of corresp. output file, out_name, & allocate it. */
        ajStrAssS(&out_name, out_EMBLpdbpath);
        ajStrApp(&out_name, pdb->Pdb);
        ajStrApp(&out_name, out_EMBLpdbextn);
        cpdb_outf = ajFileNewOut(out_name);
        
        /* Write AjPPdb object to the output file in clean format. */
        if(!ajPdbWriteAll(cpdb_outf, pdb))
        {               
	    ajWarn("%s%S\n//\n","Could not write results file: ", 
                        out_name);  
	    
	    ajFmtPrintF(errf,"%s%S\n//\n","Could not write results file: ", 
                        out_name);

        }	
	ajFileClose(&cpdb_outf);
        ajPdbDel(&pdb);
        ajStrDel(&tempstr);
    } /* End of main application loop. */


    ajFmtPrint("STRIDE  failures: %d\n", nostride);
    ajFmtPrint("NACCESS failures: %d\n", nonaccess);
    ajFmtPrintF(errf, "\n\nSTRIDE  failures: %d\nNACCESS failures: %d\n", nostride, nonaccess);
    

    

    ajStrDel(&pdb_name);
    ajStrDel(&pdb_path);
    ajStrDel(&pdb_extn);
    ajStrDel(&pdb_prefix);
    ajStrDel(&in_EMBLpdbpath);
    ajStrDel(&in_EMBLpdbextn);
    ajStrDel(&out_EMBLpdbpath);
    ajStrDel(&out_EMBLpdbextn);
    ajStrDel(&out_name);
    ajStrDel(&res);
    ajStrDel(&res_num);
    ajStrDel(&randomname);
    ajStrDel(&line);
    ajStrDel(&naccess_str);
    ajStrDel(&exec);
    ajStrDel(&syscmd);
  
    ajFileClose(&errf);
    if(ajStrChar(*mode, 0) != '2')
	ajFileClose(&serrf);
    if(ajStrChar(*mode, 0) != '1')
	ajFileClose(&nerrf);
    ajListDel(&cpdblist);

    ajStrDel(&mode[0]);
    AJFREE(mode);




    
    ajExit();
    return 0;
} 





/* @func pdbplus_sort ********************************************************
**
** Identifies and indexes secondary structure elements in a Pdb object
** assigns eNum to Atom objects 
** assigns numHelices & numStrands to ChainStride objects
**
** @param [r] pdb [AjPPdb] Pdb object
** @param [r] tS  [ajint]  threshold size for secondary structure elements
** 
** @return [void]
** @@
*****************************************************************************/

void pdbplus_sort(AjPPdb pdb, int tS)
{
    
    AjPAtom *arr = NULL;  /* Array of AjPAtom objects from list
			     of AjPAtom objects in Pdb chain object. */
    ajint n      = 0;     /* Current position in array of atoms. */
    ajint x      = 0;     /* Loop counter. */
    ajint z      = 0;     /* Loop counter. */
    ajint siz    = 0;     /* Size of array of atoms. */
    ajint start  = 0;     /* Start position of element. */
    ajint end    = 0;     /* End position of element. */
    ajint esiz   = 0;     /* Size of current element. */
    ajint eNum   = 0;     /* Sequential count of elements. */
    ajint resnum = 0;     /* Residue number of last atom, Idx value. */
    char   etype = ' ';   /* Element type. */
    AjBool foundStart =ajFalse; /* True if we have found the start 
				   of an element of any size*/
    int numHelices = 0;
    int numStrands = 0;
	
 



    for(z=0; z < pdb->Nchn; z++)
    {
        /* Use ajListToArray to convert the list of atoms for the current
	   chain to an array.  Returns size of array of pointers */

        siz = ajListToArray((AjPList)pdb->Chains[z]->Atoms,
			  (void ***)&arr);

	/*loop through the array to identify, index  &
              then write SSE data to atoms in the array*/
        for(eNum=1, foundStart=ajFalse, n=0; 
	    n<siz; 
	    resnum = arr[n]->Idx, n++)
        {
            /* if atom is def. not in an element */
            if((arr[n]->eStrideType == 'C')  ||
	       (arr[n]->eStrideType == 'B')  ||
	       (arr[n]->eStrideType == 'b')  ||
	       (arr[n]->eStrideType == 'T')  ||
	       (arr[n]->eStrideType == '.'))
            {
	        /* If element start already found, this atom defines
		   the end of an element. */
                if (foundStart)
                {
		  /* Check if element size  >= threshold size. */
                    if(esiz >= tS)  
                    {
                        end = n-1;
                        /* Write element data. */
                        pdbplus_writeElement(start, end, eNum, arr);
                        eNum++;
                    }
                    /* Element written or element < threshold size */
                    foundStart = ajFalse;
                    esiz = 0;
                    continue;   /* Next atom in array. */		
                }
		continue;       /* Next atom in array. */
            }
	    /* This atom might be in an element of tS or greater. */
            else 
            {
                if(foundStart)
                {
                    if(arr[n]->eStrideType != etype)
                    {
		        /* Found the end of one & the start of next element.
			   Check element size (esiz) >= threshold size. */
                        if(esiz >= tS) 
                        {
                            end = n-1;
                            /* Write element data. */
                            pdbplus_writeElement(start, end, eNum, arr);
                            eNum++;
                        }
                        /* foundStart remains ajTrue. */
                        start = n;
			/*  atom is first residue of next element. */
                        esiz = 1;	 
                        etype = arr[n]->eStrideType;
                        continue;	
                    }
		    else
                    {
		        /* 
			** Residue type is same as first residue.
                        ** Increase size of element if residue number has
			** increased by 1 since the last atom but the residue
			** identity is the same as that for the first residue.
			** 'by 1' accounts for 'gaps' in the residue numbering,
                        ** e.g.caused by missing electron density. 
			*/
                        if(arr[n]->Idx == resnum+1)
			    esiz++;

                        /* Cope with cases of jumps in residue numbering -
			   such cases define the end of an element. */

                        /* If residue number has increased by more than 1,
			   end of element is found. */
                        if(arr[n]->Idx > resnum+1)
                        {
                            if (esiz >= tS)
                            {
                                /* Write element data. */
                                end = n-1;
                                pdbplus_writeElement(start, end, eNum, arr);
                                eNum++;
                            }
                            /* Current atom is the start of the next 
			       element. */
                            esiz = 1;
                            /* note-foundStart remains true. 
			       eType remains the same. */
                            start = n;
                            continue;	
                        }
                        /* If end of array is reached. */
                        if ((n == (siz-1)) && (esiz >= tS))
                        {
                            end = n;
                            pdbplus_writeElement(start, end, eNum, arr);
                        }
                    }
                }
                else
                    /* We've not found the start yet */
                {
                    start = n;
                    etype = arr[n]->eStrideType;
                    foundStart = ajTrue;
                    esiz = 1;
                    continue;		
                }
            }
	}

        /* Count numHelices, num Strands. */
	numHelices = 0;
        numStrands = 0;

        /* Loop through array of atoms again. */
        for(n=0, x=0; n<siz; n++)   
        {
	  /* eStrideNum starts at 1. */
            if(!(arr[n]->eStrideNum))		
                continue;
            else if (arr[n]->eStrideNum > x)
                { 
		    if((arr[n]->eStrideType == 'H')  ||
		       (arr[n]->eStrideType == 'G')  ||
		       (arr[n]->eStrideType == 'I'))
		    {
			numHelices++;
                        x++;
                        continue;
		    }
		    else
		    {
			/* eNum will always be 0 for eStrideType's of 
			   C, T, B & b. */
                        numStrands++;
                        x++;
                        continue;
		    }
		}
	}	
	
        pdb->Chains[z]->numHelices = numHelices;
        pdb->Chains[z]->numStrands = numStrands;
	AJFREE(arr); 
    }
}





/* @func pdbplus_writeElement ************************************************
**
** Writes eNum (element number) to Atoms in Pdb object
** 
** @param [start]  [ajint]          start position of SSE
** @param [end]    [ajint]          end position of SSE
** @param [eNum]   [ajint]          SSE number
** @param [arr]    [AjPAtom*] pointer to array of AjPAtom objects
**
** @return [void]
** @@
*****************************************************************************/

void pdbplus_writeElement(int start, int end, int eNum, AjPAtom *arr)
{
    int x = 0;

    for(x=start; x<=end; x++)
    {
	arr[x]->eStrideNum = eNum;
    }    

    return;
}





/* @func pdbplus_CpdbReadOldTwo **********************************************
**
** Reads a Cpdb file  (new format) and writes a filled Pdb object.
** Needs modifying to return ajFalse in case of bad format etc
**
** The following types of lines can be parsed (the CO lines are wrapped):
**
** EX   METHOD xray; RESO 2.00; NMOD 1; NCHA 1; NGRP 0;
** XX
** IN   ID .; NR 182; NL 1; NH 2; NE 9; NS 0; NT 8;
** CO   1    1    .    P    1     1     .    .    .    .    E    GLU    N    
**   5.199   35.047   56.202     1.00    73.68
** CO   1    1    .    P    1     1     .    .    .    .    E    GLU    CA  
**   3.949   34.440   55.713     1.00    73.22
**  
** @param [r] inf  [AjPFile] Pointer to cpdb file
** @param [w] thys [AjPPdb*] Pdb object pointer
**
** @return [AjBool] True on success
** @@
*****************************************************************************/

AjBool pdbplus_CpdbReadOldTwo(AjPFile inf, AjPPdb *thys)
{
    ajint         nmod = 0;
    ajint         ncha = 0;
    ajint         ngrp = 0;
    ajint           nc = 0;
    ajint          mod = 0;
    ajint          chn = 0;
    ajint          gpn = 0;

    float       reso   = 0.0;

    AjPStr      line   = NULL;
    AjPStr     token   = NULL;
    AjPStr     idstr   = NULL;
    AjPStr     destr   = NULL;
    AjPStr     osstr   = NULL;
    AjPStr      xstr   = NULL;
    AjPStrTok handle   = NULL;
     
    AjPAtom     atom   = NULL;


    


    /* Intitialise strings. */
    line  = ajStrNew();
    token = ajStrNew();
    idstr = ajStrNew();
    destr = ajStrNew();
    osstr = ajStrNew();
    xstr  = ajStrNew();





    /* Start of main loop. */
    while(ajFileReadLine(inf,&line))
    {
	if(ajStrPrefixC(line,"XX"))
	    continue;

	/* Parse ID. */
	if(ajStrPrefixC(line,"ID"))
	{
	    ajStrTokenAss(&handle,line," \n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&idstr,&handle,NULL);
	    continue;
	}
	
	/* Parse number of chains. */
	if(ajStrPrefixC(line,"CN"))
	{
	    ajStrTokenAss(&handle,line," []\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&nc);
	    continue;
	}

	/* Parse description text. */
	if(ajStrPrefixC(line,"DE"))
	{
	    (void) ajStrTokenAss (&handle, line, " ");
	    (void) ajStrToken (&token, &handle, NULL);
	    /* 'DE' */
	    (void) ajStrToken (&token, &handle, "\n\r");
	    /* desc */
	    if (ajStrLen(destr))
	    {
		(void) ajStrAppC (&destr, " ");
		(void) ajStrApp (&destr, token);
	    }
	    else
		(void) ajStrAss (&destr, token);
	    continue;
	}

	/* Parse source text. */
	if(ajStrPrefixC(line,"OS"))
	{
	    (void) ajStrTokenAss (&handle, line, " ");
	    (void) ajStrToken (&token, &handle, NULL);
	    /* 'OS' */
	    (void) ajStrToken (&token, &handle, "\n\r");
	    /* source */
	    if (ajStrLen(osstr))
	    {
		(void) ajStrAppC (&osstr, " ");
		(void) ajStrApp (&osstr, token);
	    }
	    else
		(void) ajStrAss (&osstr, token);
	    continue;
	}

	/* Parse experimental line. */
	if(ajStrPrefixC(line,"EX"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&xstr,&handle,NULL); /* method */
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* reso */
	    ajStrToFloat(token,&reso);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* nmod */
	    ajStrToInt(token,&nmod);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* ncha */
	    ajStrToInt(token,&ncha);

	    ajStrToken(&token,&handle,NULL); /* ngrp */
	    ajStrToInt(token,&ngrp);

	    *thys = ajPdbNew(ncha);

	    ajStrAssS(&(*thys)->Pdb,idstr);
	    ajStrAssS(&(*thys)->Compnd,destr);
	    ajStrAssS(&(*thys)->Source,osstr);
	    if(ajStrMatchC(xstr,"xray"))
		(*thys)->Method = ajXRAY;
	    else
		(*thys)->Method = ajNMR;

	    (*thys)->Reso = reso;
	    (*thys)->Nmod = nmod;
	    (*thys)->Nchn = ncha;
	    (*thys)->Ngp  = ngrp;
	}

	/* Parse information line. */
	if(ajStrPrefixC(line,"IN"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* id value */
	    (*thys)->Chains[nc-1]->Id = *ajStrStr(token);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* residues */
	    ajStrToInt(token,&(*thys)->Chains[nc-1]->Nres);
	    ajStrToken(&token,&handle,NULL);
	    /* hetatm */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(*thys)->Chains[nc-1]->Nlig);
	    /* helices */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(*thys)->Chains[nc-1]->numHelices);
	    /* strands */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(*thys)->Chains[nc-1]->numStrands);
	    /* sheets */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(*thys)->Chains[nc-1]->numSheets);
	    */
	    /* turns */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(*thys)->Chains[nc-1]->numTurns);
	    */
	    continue;
	}

	/* Parse sequence line. */
	if(ajStrPrefixC(line,"SQ"))
	{
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&(*thys)->Chains[nc-1]->Seq,ajStrStr(line));
	    ajStrCleanWhite(&(*thys)->Chains[nc-1]->Seq);
	    continue;
	}

	/* Parse coordinate line. */
	if(ajStrPrefixC(line,"CO"))
	{
	    mod = chn = gpn = 0;
	    
	    ajStrTokenAss(&handle,line," \t\n\r");
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&mod);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&chn);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&gpn);
	    
	    atom = ajAtomNew();
	    
	    atom->Mod = mod;
	    atom->Chn = chn;
	    atom->Gpn = gpn;
	    

	    ajStrToken(&token,&handle,NULL);
	    atom->Type = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->Idx);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Pdb,token);

	    ajStrToken(&token,&handle,NULL);
	    atom->eType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eNum);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->eId,token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eClass);

	    ajStrToken(&token,&handle,NULL);
	    atom->Id1 = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Id3,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Atm,token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->X);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Y);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Z);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->O);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->B);


	    /* Check for coordinates for water or groups that could not
	       be uniquely assigned to a chain. */
	    if(chn == 0)
	    {
		/* Heterogen */
		if(atom->Type == 'H')
		    ajListPushApp((*thys)->Groups,(void *)atom);
		else if(atom->Type == 'W')
		    ajListPushApp((*thys)->Water,(void *)atom);
		else
		    ajFatal("Unexpected parse error in ajPdbRead. "
			    "Email jison@hgmp.mrc.ac.uk");
	    }
	    else
		ajListPushApp((*thys)->Chains[chn-1]->Atoms,(void *)atom);
	}
    }
    /* End of main loop. */
    


    /* Tidy up */
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&idstr);
    ajStrDel(&destr);
    ajStrDel(&osstr);
    ajStrDel(&xstr);


    return ajTrue;
}
