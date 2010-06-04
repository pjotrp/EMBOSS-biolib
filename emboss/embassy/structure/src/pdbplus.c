/* @Source pdbplus application
**
** Add residue solvent accessibility and secondary structure data to
** a CCF file (clean coordinate file) for a protein or domain.
** 
**
** @author: Copyright (C) Jon Ison (jison@ebi.ac.uk)
** @author: Copyright (C) Amanda O'Reilly
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
**  Email jison@ebi.ac.uk.
**  
******************************************************************************/





#include "emboss.h"





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static void     pdbplus_writeElement(int start, 
			      int end,
			      int eNum,
			      AjPResidue *arr);

static void     pdbplus_sort(AjPPdb pdb,
		      int tS);





/* @prog pdbplus *************************************************************
**
** Add residue solvent accessibility and secondary structure data to
** a CCF file (clean coordinate file) for a protein or domain.
** 
*****************************************************************************/

int main(ajint argc, char **argv)
{
    AjPList  ccfin        = NULL;  /* List of CCF (input) files.             */

    AjPDir   pdbin        = NULL;  /* Path of pdb input files.               */
    AjPStr   pdbprefix    = NULL;  /* Prefix of pdb input files.             */
    AjPStr   pdb_name     = NULL;  /* Full name (path/name/extension) of 
					 pdb format input file.              */

    AjPDirout ccfout     = NULL;   /* Path of coordinate output file.        */
    AjPStr   randomname  = NULL;   /* Name for temp file tempf.              */
    AjPStr   ccf_this    = NULL; 
    AjPStr   exec        = NULL; 
    AjPStr   naccess_str = NULL; 
    AjPStr   line        = NULL;
    AjPStr   syscmd      = NULL;   /* Command line arguments.                */
    AjPStr  *mode        = NULL;   /* Mode of operation from acd.            */

    AjPFile  errf        = NULL;   /* pdbplus error file pointer.            */
    AjPFile  tempf       = NULL;   /* Temp file for holding STRIDE output.   */
    AjPFile  ccf_inf     = NULL;   /* Protein coordinate input file.         */
    AjPFile  ccf_outf    = NULL;   /* Protein coordinate output file.        */

    AjPStr   nerrname    = NULL;   /* naccess error file name */
    AjPStr   serrname    = NULL;   /* stride error file name */
    AjIList  iter        = NULL; 

    AjBool   done_naccess= ajFalse;
    AjBool   done_stride = ajFalse;
    AjBool   found       = ajFalse;
    AjPResidue temp_res  = NULL;  /* Pointer to Residue object.                */
    AjPPdb   pdb_old     = NULL;  /* Pointer to PDB object - without new
				     stride elements.                       */
    AjPPdb   pdb         = NULL;  /* Pointer to PDB object.                 */
    ajint    idn         = 0;     /* Chain identifier as a number (1,2,...) */
    ajint    chain_num   = 0;     /* Chain identifier index (0,1,...).      */
    ajint    tS          = 0;     /* User-defined threshold size for SSEs.  */
    ajint    nostride    = 0;     /* No. times stride failed                */
    ajint    nonaccess   = 0;     /* No. times naccess failed               */
    ajint    nofile      = 0;     /* No. times of file error                */

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
    res            = ajStrNew();
    res_num        = ajStrNew();
    randomname     = ajStrNew();
    syscmd         = ajStrNew();
    line           = ajStrNew();  
    naccess_str    = ajStrNew();
    exec           = ajStrNew();





    /* Read data from acd. */
    embInitPV("pdbplus",argc,argv,"STRUCTURE",VERSION);

    ccfin        = ajAcdGetDirlist("ccfinpath");  
    pdbin        = ajAcdGetDirectory("pdbindir"); 
    pdbprefix    = ajAcdGetString("pdbprefix");
    ccfout       = ajAcdGetOutdir("ccfoutdir");
    mode         = ajAcdGetList("mode");
    errf         = ajAcdGetOutfile("logfile");
    if(ajStrGetCharFirst(*mode) != '2')
	serrname = ajAcdGetOutfileName("slogfile");
    if(ajStrGetCharFirst(*mode) != '1')
	nerrname = ajAcdGetOutfileName("nlogfile");

    tS           = ajAcdGetInt("thresholdsize");
 
    


    
    ajRandomSeed();
    ajFilenameSetTempname(&randomname); 




    
    /* 
     **  Start of main application loop. 
     **  Process each PDB/ protein coordinate file (EMBL format) in turn. 
     */ 
    
    while(ajListPop(ccfin,(void **)&ccf_this))
    {
        /* Open protein coordinate file.  If it cannot be opened, write a 
           message to the error file, delete ccf_this and continue. */

        if((ccf_inf = ajFileNewInNameS(ccf_this)) == NULL)   
	{
	    ajWarn("%s%S\n//\n", 
		   "clean coordinate file not found: ", ccf_this);
	    
	    ajFmtPrintF(errf, "%s%S\n//\n", 
                        "clean coordinate file not found: ", ccf_this); 
            ajStrDel(&ccf_this); 
	    nofile++;
	    continue; 
        }       

        ajFmtPrint("Processing %S\n", ccf_this);
	fflush(stdout);

        /* Parse protein coordinate data (from clean format file) into 
	   AjPPdb object.  ajPdbReadAllModelsNew will create the AjPPdb object. */
      if(!(pdb_old=ajPdbReadAllModelsNew(ccf_inf)))
        {
	    ajWarn("ERROR Clean coordinate file read" 
		   "error: %S\n//\n", ccf_this);
            ajFmtPrintF(errf, "ERROR Clean coordinate file read" 
			"error: %S\n//\n", ccf_this);
            ajFileClose(&ccf_inf);
            ajStrDel(&ccf_this); 
	    nofile++;
            continue;
        }

        ajFileClose(&ccf_inf);
        ajPdbCopy(&pdb, pdb_old); 
        ajPdbDel(&pdb_old); 

        /* Construct name of corresponding PDB file.
	    NACCESS does *not* generate an output file if the path is './' e.g. 
	    naccess ./1rbp.ent , therefore replace './' with null. */
	ajStrAssignS(&pdb_name, ajDirGetPath(pdbin));
	if(ajStrMatchC(pdb_name, "./") || ajStrMatchC(pdb_name, "."))
	    ajStrAssignC(&pdb_name, "");
	
        ajStrAppendS(&pdb_name, pdbprefix);
	ajStrFmtLower(&pdb->Pdb);
        ajStrAppendS(&pdb_name, pdb->Pdb);
        ajStrAppendC(&pdb_name, ".");
	ajStrAppendS(&pdb_name, ajDirGetExt(pdbin));
	

        /* Check corresponding PDB file exists for reading using ajFileStat. */
	if(!(ajFilenameExistsRead(pdb_name)))
        {
            ajFmtPrintF(errf, "%s%S\n//\n", "PDB file not found: ", pdb_name);
            ajWarn("%s%S\n//\n", "PDB file not found: ", pdb_name);
            ajStrDel(&ccf_this); 
            ajPdbDel(&pdb);
	    nofile++;
            continue;
        }
        
	if(ajStrGetCharFirst(*mode) != '2')
        {        
	    /* 
	     **  Create a string containing the STRIDE command line (it needs
	     **  PDB file name & name of temp output file).
	     **  Call STRIDE by using ajSysExecOutnameErrS.
	     */
	    
	    ajFmtPrintS(&syscmd, "%S %S -f%S",  
			ajAcdGetpathC("stride"),
                        pdb_name, randomname);
	    ajFmtPrint("%S\n", syscmd);
	    ajSysExecOutnameErrAppendS(syscmd, serrname);

	    
	    /* Open the stride output file */
	    if (((tempf = ajFileNewInNameS(randomname)) == NULL))
	    {
		ajWarn("%s%S\n//\n", 
		       "no stride output for: ", pdb_name); 
		ajFmtPrintF(errf, "%s%S\n//\n", 
			    "no stride output for: ", pdb_name); 
		nostride++;
		ajStrDel(&ccf_this);
		ajPdbDel(&pdb); 
		continue; 
	    } 
	    else
	      ajFmtPrintF(errf, "%s%S\n//\n", 
			  "stride output for: ", pdb_name); 

	    
	    done_stride = ajFalse;

	    /* Parse STRIDE output from temp output file a line at a time. */
	    while(ajReadlineTrim(tempf,&line))
	    {       
		if(ajStrPrefixC(line,"ASG"))    
		{
		    ajFmtScanS(line, "%*S %S  %c %S %*d %c %*S %f %f %f %*S", 
			       &res, &pcid, &res_num, &ss, &ph, &ps, &sa);
                
		    /* 
		     **  Populate pdbplus object with the data from this parsed
		     **  line. This means first identifying the chain, then 
		     **  finding the residue. 
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
		     **   Iiterate through the list of residues in the Pdb object,
		     **   found switches to true when first residue corresponding 
		     **   to the line is found. 
		     */

		    /* iter = ajListIterNewread(pdb->Chains[chain_num]->Atoms); */
		    iter = ajListIterNewread(pdb->Chains[chain_num]->Residues);
		    found = ajFalse; 

		    while((temp_res = (AjPResidue)ajListIterGet(iter)))
		    {
		        /* If we have found the residue we want */
			if((ajStrMatchS(res_num, temp_res->Pdb) && 
			    ajStrMatchS(res, temp_res->Id3)))
			{
                       	    done_stride = ajTrue;
			    found = ajTrue;
			    temp_res->eStrideType = ss;
			    temp_res->Phi  = ph;
			    temp_res->Psi  = ps;
			    temp_res->Area = sa;
			}                 
			/* If the matching residue has been processed
			   move on to next ASG line, next residue. */
			else if(found == ajTrue) 
			    break;	
			else 
			/* Matching residue not found yet. */       
			    continue;	
		    }
		    ajListIterDel(&iter);
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
	    ajSysCommandRemoveS(randomname); 
	    
	    /* 
	     **  Calculate element serial numbers (eStrideNum)& ammend residue
	     **  objects, count no's of elements and ammend chain object 
	     **  (numHelices, num Strands). 
	     */
	    pdbplus_sort(pdb, tS);
	}
	

	if(ajStrGetCharFirst(*mode) != '1')
        {        
	    /* 
	     **   Create a string containing the NACCESS command line (it needs
	     **   PDB file name & name of temp output file) & call NACCESS.
	     **   If e.g. /data/structure/pdbfred.ent was parsed and the program
	     **   was run from /stuff, then /stuff/fred.asa and /stuff/fred.rsa
	     **   would be written.  These must be deleted once parsed (only
	     **   use the .rsa file here). 
	     */
	    
	    ajFmtPrintS(&syscmd, "%S %S",  
			ajAcdGetpathC("naccess"), pdb_name);
	    ajFmtPrint("%S\n", syscmd);
	    ajSysExecOutnameErrAppendS(syscmd, nerrname);


	    
	    ajStrAssignS(&naccess_str, pdbprefix);
	    ajStrAppendS(&naccess_str, pdb->Pdb);
	    ajStrAppendC(&naccess_str, ".rsa");
	    
	    /* Open the NACCESS output file. */
	    if (((tempf = ajFileNewInNameS(naccess_str)) == NULL))
	    {
		ajFmtPrintF(errf, "%s%S\n//\n", 
			    "no naccess output for: ", pdb_name); 
		ajWarn("%s%S\n//\n", "no naccess output for: ", pdb_name);
		nonaccess++;
		ajStrDel(&ccf_this);
		ajPdbDel(&pdb); 
		continue; 
	    }	 
	    else
	      ajFmtPrintF(errf, "%s%S\n//\n", 
			  "naccess output for: ", pdb_name); 


	    done_naccess = ajFalse;
	    /* Parse NACCESS output from temp output file a line at a time. */	    
	    while(ajReadlineTrim(tempf,&line))
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

		    /* Identify the chain, then finding all the residues 
		       corresponding to the residue. */
                
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
		     **   Iiterate through the list of residues in the Pdb object,
		     **   temp_res is an AjPResidue used to point to the current
		     **   residue.
		     **   ajBool found switches to true when first residue 
		     **   corresponding to the line is found. 
		     */
		    iter = ajListIterNewread(pdb->Chains[chain_num]->Residues);

		    found = ajFalse; 
		    while((temp_res = (AjPResidue)ajListIterGet(iter)))
		    {
			/* If we have found the residue we want, write the residue 
			   object. */
			if((ajStrMatchS(res_num, temp_res->Pdb) && 
			    ajStrMatchS(res, temp_res->Id3)))
                        {
			    found = ajTrue;
			    done_naccess = ajTrue;
			    temp_res->all_abs  = f1;
			    temp_res->all_rel  = f2;
			    temp_res->side_abs = f3;
			    temp_res->side_rel = f4;
			    temp_res->main_abs = f5;
			    temp_res->main_rel = f6;
			    temp_res->npol_abs = f7;
			    temp_res->npol_rel = f8;
			    temp_res->pol_abs  = f9;
			    temp_res->pol_rel  = f10;

			}      
			/* If the matching residues have all been processed. 
			   move on to next ASG line, next residue. */
			else if(found == ajTrue) 
			    break;	
			else 
			    /* Matching residues not found yet, move on to next 
			       residue. */
			    continue;	 
		    }
		    ajListIterDel(&iter);
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
	    
	    ajSysCommandRemoveS(naccess_str); 

	    ajStrAssignS(&naccess_str, pdbprefix);
	    ajStrAppendS(&naccess_str, pdb->Pdb);
	    ajStrAppendC(&naccess_str, ".asa");
	    ajSysCommandRemoveS(naccess_str); 

	    ajStrAssignS(&naccess_str, pdbprefix);
	    ajStrAppendS(&naccess_str, pdb->Pdb);
	    ajStrAppendC(&naccess_str, ".log");
	    ajFmtPrintS(&exec, "rm %S", naccess_str);
	    ajSysCommandRemoveS(naccess_str); 
	}

        /* Open CCF (output) file. */
        ccf_outf = ajFileNewOutNameDirS(pdb->Pdb, ccfout);
	
        
        /* Write AjPPdb object to the output file in clean format. */
        if(!ajPdbWriteAll(ccf_outf, pdb))
        {               
	    ajWarn("%s%S\n//\n","Could not write results file for: ", 
                        pdb->Pdb);  
	    
	    ajFmtPrintF(errf,"%s%S\n//\n","Could not write results file for ", 
                        pdb->Pdb);

        }	
	ajFileClose(&ccf_outf);
        ajPdbDel(&pdb);
        ajStrDel(&ccf_this);
    } /* End of main application loop. */


    ajFmtPrint("STRIDE  failures: %d\n", nostride);
    ajFmtPrint("NACCESS failures: %d\n", nonaccess);
    ajFmtPrintF(errf, "\n\nSTRIDE  failures: %d\nNACCESS failures: %d\n",
		nostride, nonaccess);
    

    

    ajListFree(&ccfin);
    ajDirDel(&pdbin);
    ajStrDel(&pdbprefix);
    ajStrDel(&pdb_name);
    ajDiroutDel(&ccfout);
    ajStrDel(&res);
    ajStrDel(&res_num);
    ajStrDel(&randomname);
    ajStrDel(&line);
    ajStrDel(&naccess_str);
    ajStrDel(&exec);
    ajStrDel(&syscmd);
  
    ajFileClose(&errf);

    ajStrDel(&mode[0]);
    AJFREE(mode);




    
    ajExit();
    return 0;
} 





/* @funcstatic pdbplus_sort ***************************************************
**
** Identifies and indexes secondary structure elements in a Pdb object
** assigns eNum to Residue objects 
** assigns numHelices & numStrands to ChainStride objects
**
** @param [r] pdb [AjPPdb] Pdb object
** @param [r] tS  [ajint]  threshold size for secondary structure elements
** 
** @return [void]
** @@
*****************************************************************************/

static void pdbplus_sort(AjPPdb pdb, int tS)
{
    
    AjPResidue *arr = NULL;  /* Array of Residue objects from list
			     of Residue objects in Pdb chain object.        */
    ajint n      = 0;     /* Current position in array of residues.         */
    ajint x      = 0;     /* Loop counter.                                  */
    ajint z      = 0;     /* Loop counter.                                  */
    ajint siz    = 0;     /* Size of array of residues.                     */
    ajint start  = 0;     /* Start position of element.                     */
    ajint end    = 0;     /* End position of element.                       */
    ajint esiz   = 0;     /* Size of current element.                       */
    ajint eNum   = 0;     /* Sequential count of elements.                  */
    ajint resnum = 0;     /* Residue number of last residue, Idx value.     */
    char   etype = ' ';   /* Element type.                                  */
    AjBool foundStart =ajFalse; /* True if we have found the start 
				   of an element of any size*/
    int numHelices = 0;
    int numStrands = 0;
	
 



    for(z=0; z < pdb->Nchn; z++)
    {
        /* Use ajListToArray to convert the list of residues for the current
	   chain to an array.  Returns size of array of pointers */

        siz = ajListToarray((AjPList)pdb->Chains[z]->Residues,
			  (void ***)&arr);

	/* Loop through the array to identify, index  &
	   then write SSE data to residues in the array. */
        for(eNum=1, foundStart=ajFalse, n=0; 
	    n<siz; 
	    resnum = arr[n]->Idx, n++)
        {
            /* If residue is def. not in an element. */
            if((arr[n]->eStrideType == 'C')  ||
	       (arr[n]->eStrideType == 'B')  ||
	       (arr[n]->eStrideType == 'b')  ||
	       (arr[n]->eStrideType == 'T')  ||
	       (arr[n]->eStrideType == '.'))
            {
	        /* If element start already found, this residue defines
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
                    /* Element written or element < threshold size. */
                    foundStart = ajFalse;
                    esiz = 0;
                    continue;   /* Next residue in array. */		
                }
		continue;       /* Next residue in array. */
            }
	    /* This residue might be in an element of tS or greater. */
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
			/*  residue is first residue of next element. */
                        esiz = 1;	 
                        etype = arr[n]->eStrideType;
                        continue;	
                    }
		    else
                    {
		        /* 
			** Residue type is same as first residue.
                        ** Increase size of element if residue number has
			** increased by 1 since the last residue but the residue
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
                            /* Current residue is the start of the next element. */
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
                    /* We've not found the start yet. */
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

        /* Loop through array of residues again. */
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





/* @funcstatic pdbplus_writeElement *******************************************
**
** Writes eNum (element number) to Residues in Pdb object
** 
** @param [start]  [ajint]     Start position of SSE
** @param [end]    [ajint]     End position of SSE
** @param [eNum]   [ajint]     SSE number
** @param [arr]    [AjPResidue*]  Pointer to array of AjPResidue objects
**
** @return [void]
** @@
******************************************************************************/

static void pdbplus_writeElement(int start, 
				 int end,
				 int eNum,
				 AjPResidue *arr)
{
    int x = 0;

    for(x=start; x<=end; x++)
    {
	arr[x]->eStrideNum = eNum;
    }    

    return;
}




