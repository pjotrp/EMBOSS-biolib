/* @source scoprep application
**
** Reorder scop classificaiton file so that the representative structure of 
** each family is given first
**
** @author: Copyright (C) Mathew Blades (mblades@hgmp.mrc.ac.uk)
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
******************************************************************************
** 
** 
** 
** 
******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE
******************************************************************************
**
** Mon May 20 11:43:39 BST 2002
**
** The following documentation is out-of-date and should be disregarded.  It 
** will be updated shortly. 
** 
******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE
******************************************************************************
** 
** 
** Operation
** 
** scoprep parses a scop classification file in embl-like format generated by 
** the EMBOSS applications scope or nrscope, and domain coordinate files 
** generated by the EMBOSS application domainer, and calls stamp to generate 
** structural alignments for each SCOP family in turn.
** 
** VERY IMPORTANT NOTE 
** scoprep will only run with with a version of stamp which has been modified
** so that pdb id codes of length greater than 4 characters are acceptable.
** This involves a trivial change to the stamp module getdomain.c (around line
** number 155), a 4 must be changed to a 7 as follows:
** temp=getfile(domain[0].id,dirfile,4,OUTPUT); 
** temp=getfile(domain[0].id,dirfile,7,OUTPUT); 
** 
** The modified code is kept on the HGMP file system in /packages/stamp/src2
** WHEN RUNNING SCOPREP AT THE HGMP IT IS ESSENTIAL THAT THE COMMAND 
** 'use stamp2' (which runs the script /packages/menu/USE/stamp2) IS GIVEN 
** BEFORE SCOPREP IS RUN.  This will ensure that the modified version of 
** stamp is used.
** 
** stamp searches for pdb files with a certain prefix, extension and path as 
** specified in the stamp "pdb.directories" file.  For the HGMP, this file is
** /packages/stamp/defs/pdb.directories and should look like :
**
** /data/pdb - -
** /data/pdb _ .ent
** /data/pdb _ .pdb
** /data/pdb pdb .ent
** /data/pdbscop _ _
** /data/pdbscop _ .ent
** /data/pdbscop _ .pdb
** /data/pdbscop pdb .ent
** ./ _ _
** ./ _ .ent
** ./ _ .ent.z
** ./ _ .ent.gz
** ./ _ .pdb
** ./ _ .pdb.Z
** ./ _ .pdb.gz
** ./ pdb .ent
** ./ pdb .ent.Z
** ./ pdb .ent.gz
** /data/CASS1/pdb/coords/ _ .pdb
** /data/CASS1/pdb/coords/ _ .pdb.Z
** /data/CASS1/pdb/coords/ _ .pdb.gz

** 
** The names of the output files are identical to the names of the families
** given in the SCOP classification records, except that if a file of a 
** certain name already exists, then an "_1", "_2" etc will be added as 
** appropriate.
**
** The format of the scoprep output file (Figure 1) is similar to the output
** file generated by stamp when issued with the following three types of 
** command:
** 
** (1) stamp -l ./stamps_file.dom -s -n 2 -slide 5 -prefix ./stamps_file -d 
** ./stamps_file.set;sorttrans -f ./stamps_file.scan -s Sc 2.5 > 
** ./stamps_file.sort;stamp -l ./stamps_file.sort -prefix ./stamps_file > 
** ./stamps_file.log
**
** (2) poststamp -f ./stamps_file.3 -min 0.5
** 
** (3) ver2hor -f ./stamps_file.3.post > ./stamps_file.out
**
** However, the SCOP classification records for the appopriate family are 
** written above the alignment, no dssp assignments are given, and only the
** 'Post similar' line is given. Also, 7 character domain identifier codes 
** taken from the scop classificaiton file are given.
** 
**
** 
** Figure 1 Example of scoprep output file
**
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Globins
**  XX
**  SI   1321321
**  XX
**  Number               10        20        30        40        50    
**  d1vrea_              LSAAQRQVVASTWKDIAgsdngAGVGKECFTKFLSAHHDMAAV f gFS
**  d3sdhb_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  d3hbia_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  d3sdha_      svydaaaqLTADVKKDLRDSWKVIG sd kKGNGVALMTTLFADNQETIGYfkrlGN
**  Post_similar --------11111111111111111-00-1111111111111111111111-0-111
**  
**  Number        60        70        80        90       100       110 
**  d1vrea_      GAS   dpGVADLGAKVLAQIGVAVSHLgDEGKMVAEMKAVGVRHKgygnkhIKAEY
**  d3sdhb_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKFAVNHI  t rkISAAE
**  d3hbia_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKLAVNHI  t rkISAAE
**  d3sdha_      VSQgmandKLRGHSITLMYALQNFIDQLdNPDDLVCVVEKFAVNHI  t rkISAAE
**  Post_similar 111---0011111111111111111111011111111111111111--0-0011111
**  
**  Number          120       130       140       150       160
**  d1vrea_      FEPlGASL LSAMEhriggkMNAAAKDAWAAAYADisgalisglqs
**  d3sdhb_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al     
**  d3hbia_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al     
**  d3sdha_      FGK INGPiKKVLA s k nFGDKYANAWAKLVAVvqa al     
**  Post_similar 111-1111-11111-0-0-1111111111111111100-00-----
**  
**
** Important Notes
** stamp will ignore (omit from the alignment and *not* replace with '-' or any
** other symbol) ANY residues or groups in a pdb file that
** (i) are not structured (i.e. do not appear in the ATOM records) or
** (ii) lack a CA atom, regardless of whether it is a known amino acid or not.
** 
** This means that the position (column) in the alignment cannot reliably be 
** used as the basis for an index into arrays representing the full length 
** sequences.
**
** stamp will however include in the alignment residues with a single atom
** only, so long as it is the CA atom.
*/





#include "emboss.h"

static AjBool scoprep_WriteRmsd(ajint x, ajint y, AjPFloat2d *scores,
				AjPFile fptr);




/* @prog scoprep *******************************************************
**
** Generate alignments for SCOP families
**
******************************************************************************/

int main(int argc, char **argv)
{
    ajint     nfam      = 0;	/* Counter for the families */

    ajint     x         = 0;  /* Counter */
    ajint     y         = 0;  /* Counter */
    

/* ajint last_fam = NULL; */ /* SCOP Sunid of last family that was
      processed */
    AjPStr    last_fam  = NULL; /* Last family that was processed */
    AjPStr    exec      = NULL;	/* The UNIX command line to be executed*/
    AjPStr    dom       = NULL;	/* Name of file containing single domain*/
    AjPStr    set       = NULL;	/* Name of file containing set of domains*/
    AjPStr    out       = NULL;	/* Name of file containing pairwise
                                   rmsd values */
    AjPStr    name      = NULL;	/* Base name of STAMP temp files */
    AjPStr    temp      = NULL;	/* A temporary string */

    AjPFile   scopin    = NULL;	/* File pointer for Escop.dat file (input)*/
    AjPFile   scopout   = NULL;	/* File pointer for Escop.dat file (output)*/
    AjPFile   domf      = NULL;	/* File pointer for single domain file */
    AjPFile   setf      = NULL;	/* File pointer for domain set file */
    AjPFile   outf      = NULL;	/* File pointer for file containing
                                   pairwise rmsd values */
    

    AjPScop   scop      = NULL;	/* Pointer to scop structure */

    AjPList famlist     = NULL; /* List of Scopobjects for current family */
    AjPScop *arr        = NULL; /* Array of pointers to the famlist data */
    
    AjPScop  tmp     =NULL;  /* Temp. pointer for freeing famlist*/
    ajint   famsize     =0;     /* No. domains in the family */
    
    AjPFloat2d      scores        = NULL;    /* Array of pairwise rmsd
                                                values */
    AjPFloat        means         = NULL;    /* Array of average
                                                pairwise rmsd values */
    float      reso_cnt =0.0;   /* Used to calculate means array */
    ajint      div      =0;     /* Used to calculate means array */
    ajint      ignore   =0;     /* Used for writing ordered scop output file */
    float      min=0.0;     /* Used for writing ordered scop output file */    

   
    


    /* Initialise strings etc*/
    last_fam = ajStrNew();
    exec     = ajStrNew();

    dom      = ajStrNew();
    set      = ajStrNew();
    name     = ajStrNew();
    temp     = ajStrNew();

    famlist    = ajListNew();


    /* Read data from acd */
    embInit("scoprep",argc,argv);
    scopin     = ajAcdGetInfile("scopin");
    scopout    = ajAcdGetOutfile("scopout");


    
    /* Initialise random number generator for naming of temp. files*/
    ajRandomSeed();
    ajStrAssC(&name, ajFileTempName(NULL));


    /* Create names for temp. files*/
    ajStrAssS(&dom, name);	
    ajStrAppC(&dom, ".dom");
    ajStrAssS(&set, name);	
    ajStrAppC(&set, ".set");
    ajStrAssS(&out, name);	
    ajStrAppC(&out, ".out");



    /* Initialise last_fam with something that is not in SCOP*/
    ajStrAssC(&last_fam,"!!!!!");
    
    

    /* Start of main application loop*/
    while((ajXyzScopReadC(scopin, "*", &scop)))
    {
	/* A new family */
	if(ajStrMatch(last_fam, scop->Family)==ajFalse)
	{
	    /* If we have done the first family*/
	    if(nfam)
	    {
		/* Get family size */
		famsize = ajListLength(famlist);
		

		if(famsize>2)
		{
		    /* Create an array of pointers to the famlist data */
		    ajListToArray(famlist, (void ***) &(arr));
		    
		    /* Allocate & initialise scores and mean arrays */
		    scores   = ajFloat2dNewL(famsize);
		    for(x=0;x<famsize;x++)
			for(y=0;y<famsize;y++)
			    ajFloat2dPut(&scores, x, y, 0);
		    
		    means = ajFloatNewL(famsize);
		    for(x=0;x<famsize;x++)
			ajFloatPut(&means, x, 0);


		    /* Do pairwise alignments for the family */
		    for(x=0; x<famsize-1; x++)
		    {
			/* Open, write and close domain file*/
			if(!(domf=ajFileNewOut(dom)))
			    ajFatal("Could not open domain file\n");
			ajStrAssS(&temp, arr[x]->Entry);
			ajStrToLower(&temp);
			ajFmtPrintF(domf, "%S %S { ALL }\n", temp, temp);
			ajFileClose(&domf);	


			for(y=x+1; y<famsize; y++)
			{
			    /* Open domain set file */
			    if(!(setf=ajFileNewOut(set)))
				ajFatal("Could not open domain set file\n");

			    
			    /* Write to domain set file*/
			    ajStrAssS(&temp, arr[x]->Entry);
			    ajStrToLower(&temp);
			    ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
			    ajStrAssS(&temp, arr[y]->Entry);
			    ajStrToLower(&temp);
			    ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
			    

			    /*Close domain set file*/
			    ajFileClose(&setf);	
			    
			    
			    /* Call STAMP */
			    ajFmtPrintS(&exec,
					"stamp -l %S -s -n 2 -slide 5 -prefix "
					"%S -d %S > %S\n",
					dom, name, set, out);
			    ajFmtPrint("%S\n", exec);
			    system(ajStrStr(exec));  

			    
			    /* Open stamp output file */
			    outf = ajFileNewIn(out);
			    

			    /* Parse stamp output file and write
                               scores array */
			    scoprep_WriteRmsd(x, y, &scores, outf);
			    
			    
			    /* Close stamp output file */
			    ajFileClose(&outf);	

			}
		    }
		    
		    /* Calculate the means array 

		       In this code, the mean calculation does NOT
		       include (obviously) the comparison of the
		       domain against itself.  And in cases where the
		       array value = 100 (i.e. no rmsd given in
		       scopalign.out file) the calculation again does
		       not involve this domain.
		       
		       The important thing is that the mean is only
		       calculated from the domains against which the
		       scan domain was scanned and an rmsd
		       obtaioned. the mean is calculated by dividing
		       by the number of domains for which acceptable
		       rmsd valuse were available.  */
		 
		    for(x=0;x<famsize;x++)
		    {
			reso_cnt = 0;
			div      = 0;
			for(y=0;y<famsize;y++)    
			{       
			    if((y != x) && (ajFloat2dGet(scores, x, y) != 100))
			    {
				reso_cnt += ajFloat2dGet(scores, x, y);
				div++;
			    }           
                        
			    else
				continue;
			}
        
			ajFloatPut(&means, x, (reso_cnt/(float)(div)));
		    }
		    
		    
		    /* Write the scop classificaiton file (output)
		       giving the structure with the lowest mean rmsd
		       first */
		    for(ignore=0, min = 1000000, x=0;x<famsize;x++)
		    {
			if(ajFloatGet(means, x) < min)
			{
			    min = ajFloatGet(means, x);
			    ignore = x;
			}
		    }
		    ajXyzScopWrite(scopout, arr[ignore]);
		    for(x=0;x<famsize;x++)
		    {
			if(x==ignore)
			    continue;
			else
			    ajXyzScopWrite(scopout, arr[x]);
		    }


		    /* Delete family array amd family list, create new
                       family list*/
		    AJFREE(arr);
		    arr=NULL;
		    
		    while(ajListPop(famlist,(void**)&tmp))
			ajXyzScopDel(&tmp);
		    ajListDel(&famlist);
		    famlist    = ajListNew();
		
    
		    /* Delete the scores and means array */
		    ajFloat2dDel(&scores);  	    	    
		    ajFloatDel(&means);
		}
		/* family only has 1 or 2 members, Write output file
		   with domain or two domains in family list, delete
		   family list and continue */
		else
		{
		    while(ajListPop(famlist,(void**)&tmp))
		    {
			ajXyzScopWrite(scopout, tmp);
			ajXyzScopDel(&tmp);
		    }
		    ajListDel(&famlist);
		    famlist    = ajListNew();
		}
	    }
	    	    	    		
		
	    /* Increment family counter*/
	    nfam++;


	    /* Copy current family name to last_fam*/
	    ajStrAssS(&last_fam,scop->Family);
	}
		
	/* Add the Scop object to the list for the family */
	ajListPushApp(famlist, scop);
    }
    /* End of main application loop*/



    /* Start of code to process last family */
    /* Get family size */
    famsize = ajListLength(famlist);
		

    if(famsize>2)
    {
	/* Create an array of pointers to the famlist data */
	ajListToArray(famlist, (void ***) &(arr));
	
	/* Allocate & initialise scores and mean arrays */
	scores   = ajFloat2dNewL(famsize);
	for(x=0;x<famsize;x++)
	    for(y=0;y<famsize;y++)
		ajFloat2dPut(&scores, x, y, 0);
	
	means = ajFloatNewL(famsize);
	for(x=0;x<famsize;x++)
	    ajFloatPut(&means, x, 0);
	
	
	/* Do pairwise alignments for the family */
	for(x=0; x<famsize-1; x++)
	{
	    /* Open, write and close domain file*/
	    if(!(domf=ajFileNewOut(dom)))
		ajFatal("Could not open domain file\n");
	    ajStrAssS(&temp, arr[x]->Entry);
	    ajStrToLower(&temp);
	    ajFmtPrintF(domf, "%S %S { ALL }\n", temp, temp);
	    ajFileClose(&domf);	


	    for(y=x+1; y<famsize; y++)
	    {
		/* Open domain set file */
		if(!(setf=ajFileNewOut(set)))
		    ajFatal("Could not open domain set file\n");

			    
		/* Write to domain set file*/
		ajStrAssS(&temp, arr[x]->Entry);
		ajStrToLower(&temp);
		ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
		ajStrAssS(&temp, arr[y]->Entry);
		ajStrToLower(&temp);
		ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
			    

		/*Close domain set file*/
		ajFileClose(&setf);	
			    
			    
		/* Call STAMP */
		ajFmtPrintS(&exec,"stamp -l %S -s -n 2 -slide 5 -prefix "
			    "%S -d %S > %S\n", dom, name, set, out);
		ajFmtPrint("%S\n", exec);
		system(ajStrStr(exec));  

			    
		/* Open stamp output file */
		outf = ajFileNewIn(out);
			    

		/* Parse stamp output file and write scores array */ 
		scoprep_WriteRmsd(x, y, &scores, outf);
			    
			    
		/* Close stamp output file */
		ajFileClose(&outf);	
	    }
	}
	
	/* Calculate the means array 
	   
	   In this code, the mean calculation does NOT include (obviously)
	   the comparison of the domain against itself.  And in cases where
	   the array value = 100 (i.e. no rmsd given in scopalign.out file)
	   the calculation again does not involve this domain.
	   
	   The important thing is that the mean is only calculated from the
	   domains against which the scan domain was scanned and an rmsd 
	   obtaioned. the mean is calculated by dividing by the number of 
	   domains for which acceptable rmsd valuse were available.
	   */
	
	for(x=0;x<famsize;x++)
	{
	    reso_cnt = 0;
	    div      = 0;
	    for(y=0;y<famsize;y++)    
	    {       
		if((y != x) && (ajFloat2dGet(scores, x, y) != 100))
		{
		    reso_cnt += ajFloat2dGet(scores, x, y);
		    div++;
		}           
                        
		else
		    continue;
	    }
        
	    ajFloatPut(&means, x, (reso_cnt/(float)(div)));
	}
	
	
	/* Write the scop classificaiton file (output) giving the structure
	   with the lowest mean rmsd first */
	for(ignore=0, min = 1000000, x=0;x<famsize;x++)
	{
	    if(ajFloatGet(means, x) < min)
	    {
		min = ajFloatGet(means, x);
		ignore = x;
	    }
	}
	ajXyzScopWrite(scopout, arr[ignore]);
	for(x=0;x<famsize;x++)
	{
	    if(x==ignore)
		continue;
	    else
		ajXyzScopWrite(scopout, arr[x]);
	}
	
	
	/* Delete family array amd family list, create new family list*/
	AJFREE(arr);
	arr=NULL;
	
	while(ajListPop(famlist,(void**)&tmp))
	    ajXyzScopDel(&tmp);
	ajListDel(&famlist);
	famlist    = ajListNew();
	
	
	/* Delete the scores and means array */
	ajFloat2dDel(&scores);  	    	    
	ajFloatDel(&means);
    }
    /* family only has 1 or 2 members, Write output file with domain 
       or two domains in family list, delete family list and continue */
    else
    {
	while(ajListPop(famlist,(void**)&tmp))
	{
	    ajXyzScopWrite(scopout, tmp);
	    ajXyzScopDel(&tmp);
	}
	ajListDel(&famlist);
	famlist    = ajListNew();
    }



    /* Remove all temporary files */
    ajFmtPrintS(&temp, "rm %S", dom);
    ajSystem(&temp);
    ajFmtPrintS(&temp, "rm %S", set);
    ajSystem(&temp);
    ajFmtPrintS(&temp, "rm %S", out);
    ajSystem(&temp);
    
    
    
    /* Tidy up*/

    ajFileClose(&scopin);	
    ajFileClose(&scopout);	
    ajStrDel(&last_fam);
    ajStrDel(&exec);
    ajStrDel(&dom);
    ajStrDel(&set);
    ajStrDel(&out);
    ajStrDel(&name);
    ajStrDel(&temp); 

    
    /* All done */
    ajExit();
    return 0;
}











/* @funcstatic scoprep_WriteRmsd **********************************************
 **
 ** Reads a file containing the redirected stdout output from the scopalign
 ** application and extracts the pairwise rmsd data.  The rmsd values are 
 ** entered into a 2d float array  
 **
 ** @param  [r] x        [ajint]      Index into scoring array
 ** @param  [r] y        [ajint]      Index into scoring array
 ** @param  [w] scores   [AjPFloat2d*] 2d float score array 
 ** @param  [r] fptr     [AjPFile]    File pointer to scopalign file
 ** @return              [AjBool]     True on success
 ** @@
 *****************************************************************************/
static AjBool scoprep_WriteRmsd(ajint x, ajint y, AjPFloat2d *scores,
				AjPFile fptr)
{

    AjPStr      scan          = NULL;    /* scan domain */
    AjPStr      dom           = NULL;    /* domain scanned against
                                            scan domian */
    AjPStr      line          = NULL;    /* string to hold file line */
    AjPStr      skip          = NULL;    /* string */
    float       rmsd          = 0.0;     /* rmsd for this pairwise
                                            comparison */
    


    /* Assign line string */
    line = ajStrNew();


    /* Read through file until line starting with "See" is found */
    while(ajFileReadLine(fptr, &line) && !(ajStrPrefixC(line, "See")))
    {
        /* Look for line starting Scan */
        if(ajStrPrefixC(line, "Scan"))
        {
            /* Assign strings */
            skip = ajStrNew();
            dom  = ajStrNew();
            scan = ajStrNew();
            
            /* Read in important bits */
            ajFmtScanS(line, "%*s%S%S%S", &scan, &dom, &skip);
            
            /* If line does NOT represent domain scanned against itself  */
            /* then process rmsd value                                   */
            if(!ajStrMatch(scan, dom))
            {
                /* Check if domain was skipped i.e. no rmsd given */
                if(ajStrMatchC(skip, "skipped"))
                {
                    /* Assign silly value to array to ensure value is */
                    /* ignored in later averaging calculations        */
                    ajFloat2dPut(scores, x, y, 100);
                    ajFloat2dPut(scores, y, x, 100);
                    
                    /* Delete strings */
                    ajStrDel(&scan);
                    ajStrDel(&skip);
                    ajStrDel(&dom);             
                }               
                
                /* Else process rmsd value */
                else
                {
                    /* Read rmsd */
                    ajFmtScanS(line, "%*s %*s %*s %*d %*f %f", &rmsd);
                    
                    /* Assign rmsd to array */
                    ajFloat2dPut(scores, x, y, rmsd);
                    ajFloat2dPut(scores, y, x, rmsd);
                    
                    /* Delete strings */
                    ajStrDel(&scan);
                    ajStrDel(&skip);
                    ajStrDel(&dom);             
                }               
            }
            
            /* Line is domain scanned against itself */
            /* so delete strings and proceed         */
            else
            {
                ajStrDel(&scan);
                ajStrDel(&skip);
                ajStrDel(&dom); 
                continue;
            }
            
        }       
            
        /* Line doesn't start with 'Scan' so continue */
        else
            continue;
    }

    
    /* Tidy up */
    ajStrDel(&line);

    /* Return */
    return ajTrue;
}
