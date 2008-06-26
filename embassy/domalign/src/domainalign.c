/* @source domainalign application
**
** Generates DAF files (domain alignment files) of structure-based sequence 
** alignments for nodes in a DCF file (domain classification file).
**
** @author: Copyright (C) Ranjeeva Ranasinghe
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
**  DOMAINALIGN documentation
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
**  Implement futher alignment options, e.g. TCOFFEE.
**  Implement proper residue number positions in alignment ('0' is given on 
**  each side of the sequence currently).
******************************************************************************/






#include "emboss.h"
#define MODE_STAMP    1
#define MODE_TCOFFEE  2




/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static void domainalign_ProcessStampFile(AjPStr in, 
					 AjPStr out, 
					 AjPDomain domain, 
					 ajint noden, 
					 AjPFile logf);

static void domainalign_writelast(AjPDomain domain, ajint noden, 
                                  AjPStr *last_node, 
                                  ajint *last_nodeid);

static void domainalign_writeid(AjPDomain domain, ajint noden, 
                                AjPDirout daf, AjPDirout super, 
                                AjPStr *align, AjPStr *alignc);

static void domainalign_writesid(AjPDomain domain, ajint noden, 
                                 AjPStr *id);

static void domainalign_stamp(AjPDomain prevdomain, 
                              AjPDomain domain, 
                              AjPDirout daf, 
                              AjPDirout super,
                              AjPDirout singlets, 
                              AjPStr    align, 
                              AjPStr    alignc, 
                              AjPStr    dom, 
                              AjPStr    name, 
                              AjPStr    set, 
                              AjPStr    scan, 
                              AjPStr    sort, 
                              AjPStr    log, 
                              AjPStr    out, 
                              AjBool    keepsinglets, 
                              ajint     moden, 
                              ajint     noden,
                              ajint     nset, 
                              AjPFile   logf);


static void domainalign_keepsinglets(AjPDomain domain,
				     ajint     noden, 
				     AjPDirout singlets, 
				     AjPFile   logf);

static void domainalign_ProcessTcoffeeFile(AjPStr in, 
					   AjPStr out, 
					   AjPDomain domain,
					   ajint noden, 
					   AjPFile logf);

static void domainalign_tcoffee(AjPDomain domain, 
				AjPStr    in, 
				AjPStr    align, 
				AjPStr    alignc, 
				AjPStr    pdbnames, 
				ajint     noden, 
				AjPFile   logf);			 





/* @prog domainalign **********************************************************
**
** Generates DAF files (domain alignment files) structure-based sequence 
** alignments for nodes in a DCF file (domain classification file).
**
******************************************************************************/

int main(int argc, char **argv)
{
    ajint     famn      = 0;	 /* Counter for the families.                */
    ajint     nset      = 0;	 /* No. entries in family.                   */
    
    ajint     last_nodeid = 0;   /* SCOP Sunid of last family that was 
				    processed.                               */
    AjPStr    last_node  = NULL; /* Last family that was processed.          */
    AjPStr    exec       = NULL; /* The UNIX command line to be executed.    */
    AjPStr    out        = NULL; /* Name of stamp alignment file.            */
    AjPStr    align      = NULL; /* Name of sequence alignment file.         */
    AjPStr    alignc     = NULL; /* Name of structure alignment file.        */
    AjPStr    log        = NULL; /* Name of STAMP log file.                  */
    AjPStr    dom        = NULL; /* Name of file containing single domain.   */
    AjPStr    set        = NULL; /* Name of file containing set of domains.  */
    AjPStr    scan       = NULL; /* Name of temp. file used by STAMP.        */
    AjPStr    sort       = NULL; /* Name of temp. file used by STAMP.        */
    AjPStr    name       = NULL; /* Base name of STAMP temp files.           */
    AjPStr    pdbnames   = NULL; /* Names of domain pdb files to be passed to
				    TCOFFEEE.                                */
    AjPDir    pdb        = NULL; /* Path of domain coordinate files (pdb 
				    format input).                           */
    AjPDirout daf        = NULL; /* Path of sequence alignment files for output. */
    AjPDirout super      = NULL; /* Path of structure alignment files for output. */
    AjPDirout singlets   = NULL; /* Path of FASTA singlet sequence files for output. */
    AjPStr    temp1      = NULL; /* A temporary string.                      */

    AjPFile   dcfin      = NULL; /* File pointer for original Escop.dat file.*/
    AjPFile   domf       = NULL; /* File pointer for single domain file.     */
    AjPFile   setf       = NULL; /* File pointer for domain set file.        */
    AjPFile   logf       = NULL; /* Log file. */

    AjPDomain domain     = NULL; /* Pointer to domain structure.             */
    AjPDomain prevdomain = NULL; /* Pointer to previous domain structure.    */

    ajint     type       = 0;    /* Type of domain (ajSCOP or ajCATH) in the 
				    DCF file.                                */

    AjPStr   *node       = NULL; /* Node of alignment         .              */
    ajint     noden      = 0;    /*1: Class (SCOP), 2: Fold (SCOP) etc, see 
				   ACD file.                                 */

    AjPStr   *mode       = NULL; /* Mode of operation from acd*/
    ajint     moden      = 0;    /* Program mode, 1: MODE_STAMP, 2: MODE_TCOFFEE (not
				    yet implemented). */
    AjBool    keepsinglets= ajFalse; /*Whether to retain sequences of singlet families
				       and write them to an output file.         */

    AjPStr    temp      = NULL;	/* A temporary string.                       */
    AjPStr    cmd       = NULL; /* The command line to execute t-coffee.     */





    /* Initialise strings etc*/
    last_node = ajStrNew();
    exec     = ajStrNew();
    out      = ajStrNew();
    align    = ajStrNew();
    alignc   = ajStrNew();
    log      = ajStrNew();
    dom      = ajStrNew();
    set      = ajStrNew();
    scan     = ajStrNew();
    sort     = ajStrNew();
    name     = ajStrNew();
    temp     = ajStrNew();
    temp1    = ajStrNew();
    cmd      = ajStrNew();
    pdbnames = ajStrNew();




    /* Read data from acd. */
    embInitP("domainalign",argc,argv,"DOMALIGN");

    dcfin       = ajAcdGetInfile("dcfinfile");
    pdb           = ajAcdGetDirectory("pdbdir");
    daf          = ajAcdGetOutdir("dafoutdir");
    super         = ajAcdGetOutdir("superoutdir");
    singlets      = ajAcdGetOutdir("singletsoutdir");
    node          = ajAcdGetList("node");
    mode          = ajAcdGetList("mode");    
    keepsinglets  = ajAcdGetToggle("keepsinglets");
    logf          = ajAcdGetOutfile("logfile");
   

    /* Convert the selected node and mode to an integer. */
    if(!(ajStrToInt(node[0], &noden)))
	ajFatal("Could not parse ACD node option");
    if(!(ajStrToInt(mode[0], &moden)))
	ajFatal("Could not parse ACD node option");


    /* Initialise random number generator for naming of temp. files. */
    ajRandomSeed();
    ajFilenameSetTempname(&name);


    /* Create names for temp. files. */
    ajStrAssignS(&log, name);	
    ajStrAppendC(&log, ".log");
    ajStrAssignS(&dom, name);	
    ajStrAppendC(&dom, ".dom");
    ajStrAssignS(&set, name);	
    ajStrAppendC(&set, ".set");
    ajStrAssignS(&scan, name);	
    ajStrAppendC(&scan, ".scan");
    ajStrAssignS(&sort, name);
    ajStrAppendC(&sort, ".sort");
    ajStrAssignS(&out, name);	
    ajStrAppendC(&out, ".out");


    /* Initialise last_node with something that is not in SCOP. */
    ajStrAssignC(&last_node,"!!!!!");
    
    

    /* Open STAMP domain set file. */
    if(moden == MODE_STAMP)
    {
	if(!(setf=ajFileNewOutNameS(set)))
	    ajFatal("Could not open domain set file\n");
    }
    

    /* Get domain type. */
    type = ajDomainDCFType(dcfin);


    /* Start of main application loop. */
    while((domain=(ajDomainReadCNew(dcfin, "*", type))))
    {
	/* A new family. */
	if(((domain->Type == ajSCOP) &&
	    (((noden==1) && (last_nodeid != domain->Scop->Sunid_Class))      ||
	     ((noden==2) && (last_nodeid != domain->Scop->Sunid_Fold))       ||
	     ((noden==3) && (last_nodeid != domain->Scop->Sunid_Superfamily))||
	     ((noden==4) && (last_nodeid != domain->Scop->Sunid_Family))))   ||
	   ((domain->Type == ajCATH) &&
	    (((noden==5) && (last_nodeid != domain->Cath->Class_Id))         ||
	     ((noden==6) && (last_nodeid != domain->Cath->Arch_Id))          ||
	     ((noden==7) && (last_nodeid != domain->Cath->Topology_Id))      ||
	     ((noden==8) && (last_nodeid != domain->Cath->Superfamily_Id))   ||
	     ((noden==9) && (last_nodeid != domain->Cath->Family_Id)))))
	{
	    /* If we have done the first family. */
	    if(famn)
	    {

		/* Create the output file for the alignment - the name will
		   be the same as the Sunid for the DOMAIN family. */
		domainalign_writeid(prevdomain, noden, daf, super,
				    &align, &alignc);

		if(moden == MODE_STAMP)
		{
		    /* Close domain set file. */
		    ajFileClose(&setf);	

		    /* Call STAMP. */
		    
		    /* Family with 2 or more entries. */
		    if(nset > 1)
		    {
			domainalign_stamp(prevdomain, 
					  domain, 
					  daf, 
					  super,
					  singlets, 
					  align, 
					  alignc, 
					  dom, 
					  name, 
					  set, 
					  scan, 
					  sort, 
					  log, 
					  out, 
					  keepsinglets, 
					  moden, 
					  noden,
					  nset, 
					  logf);
		    }
		    
		    else if(keepsinglets) /* Singlet family. */	
			domainalign_keepsinglets(prevdomain, noden,
						 singlets, logf);
			

		    /* Open STAMP domain set file. */
		    if(!(setf=ajFileNewOutNameS(set)))
			ajFatal("Could not open domain set file\n");
		}
		else
		{
		    /* Call TCOFEE. */
		    if(nset > 1)
			domainalign_tcoffee(prevdomain, out, align,
					    alignc, pdbnames, noden, logf);
		    else if(keepsinglets) /* Singlet family. */	
			domainalign_keepsinglets(prevdomain, noden,
						 singlets, logf);
		}

		/* Set the number of members of the new family to zero. */
		nset = 0;

		/* Clear TCOFFEE argument. */    
		ajStrSetClear(&pdbnames);
	    }	
	    
	    
	    /* Open, write and close STAMP domain file. */
	    if(moden == MODE_STAMP)
	    {
		if(!(domf=ajFileNewOutNameS(dom)))
		    ajFatal("Could not open domain file\n");
		ajStrAssignS(&temp, ajDomainGetId(domain));
		ajStrFmtLower(&temp);
		ajFmtPrintF(domf, "%S %S { ALL }\n", temp, temp);
		ajFileClose(&domf);	
	    }
	    
	    
	    /* Copy current family name to last_node. */
	    domainalign_writelast(domain, noden, &last_node, &last_nodeid);
	    
	    /* Copy current domain pointer to prevdomain. */
	    ajDomainDel(&prevdomain);
	    prevdomain=NULL;
	    ajDomainCopy(&prevdomain, domain);

	    /* Increment family counter. */
	    famn++;
	}
	
						
	ajStrAssignS(&temp, ajDomainGetId(domain));
	ajStrFmtLower(&temp);

	/* Write STAMP domain set file. */
	if(moden == MODE_STAMP)
	    ajFmtPrintF(setf, "%S %S { ALL }\n", temp, temp);
	/* Write TCOFFEE argument. */    
	else
	{
	    ajStrAppendS(&pdbnames, ajDirGetPath(pdb));
	    ajStrAppendS(&pdbnames, temp);
	    ajStrAppendC(&pdbnames, ".");
	    ajStrAppendS(&pdbnames, ajDirGetExt(pdb));
	    ajStrAppendC(&pdbnames, " ");
	}
	
	ajDomainDel(&domain);

	/* Increment number of members in family. */
	nset++;
    }
    
    /* End of main application loop. */
    domain=prevdomain;
    

    ajFmtPrint("\nProcessing node %d\n", last_nodeid);
    


    /* Create the output file for the alignment - the name will
       be the same as the Sunid for the DOMAIN family. */
    domainalign_writeid(prevdomain, noden, daf, super, &align, &alignc);



    /* Code to process last family. */
    if(moden == MODE_STAMP)
    {
	/*Close domain set file. */
	ajFileClose(&setf);	

		
	/*    ajFmtPrint("\n***** SECOND CALL\n");. */
	if(nset > 1)
	{
	    domainalign_stamp(prevdomain, 
			      domain, 
			      daf, 
			      super,
			      singlets, 
			      align, 
			      alignc, 
			      dom, 
			      name, 
			      set, 
			      scan, 
			      sort, 
			      log, 
			      out, 
			      keepsinglets, 
			      moden, 
			      noden,
			      nset, 
			      logf);
	}
	
	else if(keepsinglets) /* Singlet family. */	
	    domainalign_keepsinglets(prevdomain, noden, singlets, logf);
			
    }
    else
    {
	/* Call TCOFEE. */
	if(nset > 1)
	    domainalign_tcoffee(prevdomain, out, align, alignc, 
				pdbnames, noden, logf);
	else if(keepsinglets) /* Singlet family. */	
	    domainalign_keepsinglets(prevdomain, noden, singlets, logf);
    }


    /* Remove all temporary files. */

    ajSysFileUnlink(log);
    ajSysFileUnlink(dom);
    ajSysFileUnlink(set);
    ajSysFileUnlink(scan);
    ajSysFileUnlink(sort);
    ajSysFileUnlink(out);
    ajStrAssignS(&temp, name);	
    ajStrAppendC(&temp, ".mat");
    ajSysFileUnlink(temp);
    
    

    /* Tidy up*/
    ajDomainDel(&domain);
    ajFileClose(&dcfin);	
    ajStrDel(&last_node);
    ajStrDel(&exec);
    ajStrDel(&log);
    ajStrDel(&dom);
    ajStrDel(&set);
    ajStrDel(&scan);
    ajStrDel(&sort);
    ajStrDel(&name);
    ajStrDel(&out);
    ajStrDel(&align);
    ajStrDel(&alignc);
    ajStrDel(&pdbnames);
    ajDirDel(&pdb); 
    ajDiroutDel(&daf); 
    ajDiroutDel(&super); 
    ajDiroutDel(&singlets); 
    ajStrDel(&temp); 
    ajStrDel(&temp1); 
    ajStrDel(&node[0]);
    AJFREE(node);
    ajStrDel(&mode[0]);
    AJFREE(mode);
    ajFileClose(&logf);
    
    ajExit();
    return 0;
}







/* @funcstatic domainalign_ProcessStampFile ***********************************
**
** This function is very specific to domainalign, hence it is not library code.
** This function reads the output of ver2hor, i.e. a stamp alignment (Figure 
** 1 below) and creates an output file which is annotated with DOMAIN records 
** (Figure 2 below).
** 
** 
** Figure 1
** 
** VER2HOR R.B. Russell, 1995
**  Prints STAMP alignments in horizontal format
**   for quick viewing
**  Reading Alignment...
**  Getting STAMP information...
**  6 STAMP fields read in for 547 positions 
**  Processing the alignment...
**  Output:
**  Very reliable => Pij' >=6 for stretches of >=3
**  Less reliable => Pij' >=4.5 for stretches of >=3
**  Post reliable => All Pij' > stamp_post parameter for stretches >=3
** 
** Number               10        20        30        40        50    
**   d1maac_    egrEDPQLLVRVRGGQLRGIRLKAPGGPVSAFLGIPFAEPPVGSRRFMPPEPKRPWS
** d1maad__1       EDPQLLVRVRGGQLRGIRLKAPGGPVSAFLGIPFAEPPVGSRRFMPPEPKRPWS
** 
** d1maac__ds   ?????????????????????????????????????????????????????????
** d1maad__1_      ??????????????????????????????????????????????????????
** 
** Very similar ---111111111111111111111111111111111111111111111111111111
** Less similar ---111111111111111111111111111111111111111111111111111111
** Post similar ---111111111111111111111111111111111111111111111111111111
** 
** Number        60        70        80        90       100       110 
**   d1maac_    GVLDATTFQNVCYQYVDTLYPGFEGTEMWNPNRELSEDCLYLNVWTPYPRPASPTPV
** d1maad__1    GVLDATTFQNVCYQYVDTLYPGFEGTEMWNPNRELSEDCLYLNVWTPYPRPASPTPV
** 
** d1maac__ds   ?????????????????????????????????????????????????????????
** d1maad__1_   ?????????????????????????????????????????????????????????
** 
** Very similar 111111111111111111111111111111111111111111111111111111111
** Less similar 111111111111111111111111111111111111111111111111111111111
** Post similar 111111111111111111111111111111111111111111111111111111111
** 
** Number          120       130       140       150       160       170
**   d1maac_    LIWIYGGGFYSGAASLDVYDGRFLAQVEGAVLVSMNYRVGTFGFLALPGSREAPGNV
** d1maad__1    LIWIYGGGFYSGAASLDVYDGRFLAQVEGAVLVSMNYRVGTFGFLALPGSREAPGNV
** 
** d1maac__ds   ?????????????????????????????????????????????????????????
** d1maad__1_   ?????????????????????????????????????????????????????????
** 
** Very similar 111111111111111111111111111111111111111111111111111111111
** Less similar 111111111111111111111111111111111111111111111111111111111
** Post similar 111111111111111111111111111111111111111111111111111111111
** 
** 
** 
** Figure 2
**
** # CL   Alpha and beta proteins (a/b)
** # XX
** # FO   alpha/beta-Hydrolases
** # XX
** # SF   alpha/beta-Hydrolases
** # XX
** # FA   Acetylcholinesterase-like
** # XX
** # SI   1321321
** # XX
** # Number               10        20        30        40        50    
**   d1maac_    egrEDPQLLVRVRGGQLRGIRLKAPGGPVSAFLGIPFAEPPVGSRRFMPPEPKRPWS
**   d1maad_       EDPQLLVRVRGGQLRGIRLKAPGGPVSAFLGIPFAEPPVGSRRFMPPEPKRPWS
** # Post similar ---111111111111111111111111111111111111111111111111111111
** 
** # Number        60        70        80        90       100       110 
**   d1maac_    GVLDATTFQNVCYQYVDTLYPGFEGTEMWNPNRELSEDCLYLNVWTPYPRPASPTPV
**   d1maad_    GVLDATTFQNVCYQYVDTLYPGFEGTEMWNPNRELSEDCLYLNVWTPYPRPASPTPV
** # Post similar 111111111111111111111111111111111111111111111111111111111
** 
** # Number          120       130       140       150       160       170
**   d1maac_    LIWIYGGGFYSGAASLDVYDGRFLAQVEGAVLVSMNYRVGTFGFLALPGSREAPGNV
**   d1maad_    LIWIYGGGFYSGAASLDVYDGRFLAQVEGAVLVSMNYRVGTFGFLALPGSREAPGNV
** # Post similar 111111111111111111111111111111111111111111111111111111111
** 
**
** @param [r] in  [AjPStr] Name of input file
** @param [r] out [AjPStr] Name of output file
** @param [r] domain [AjPDomain] DOMAIN structure with DOMAIN classification
**                               records
** @param [r] noden [ajint] Node number.
** @param [r] logf [AjPFile] Log file.
**
** @return [void]
** @@
******************************************************************************/

static void domainalign_ProcessStampFile(AjPStr in, 
					 AjPStr out,
					 AjPDomain domain, 
					 ajint noden, 
					 AjPFile logf)
{
    AjPFile  outf = NULL;  /* Output file pointer.          */
    AjPFile   inf = NULL;  /* Input file pointer.           */
    AjPStr  temp1 = NULL;  /* Temporary string.             */
    AjPStr  temp2 = NULL;  /* Temporary string.             */
    AjPStr  temp3 = NULL;  /* Temporary string.             */
    AjPStr   line = NULL;  /* Line of text from input file. */
    ajint     blk = 1;     /* Count of the current block in the input file.
			      Block 1 is the numbering and protein sequences, 
			      Block 2 is the secondary structure, 
			      Block 3 is the Very/Less/Post similar records*/
    AjBool     ok = ajFalse;
    
    
    /* Initialise strings. */
    line    = ajStrNew();
    temp1    = ajStrNew();
    temp2    = ajStrNew();
    temp3    = ajStrNew();


    /* Open input and output files. */
    if(!(inf=ajFileNewInNameS(in)))
	ajFatal("Could not open input file in domainalign_ProcessStampFile");
    



    /* Start of code for reading input file. 
       Ignore everything up to first line beginning with 'Number'. */
    while((ajReadlineTrim(inf,&line)))
    {
	/* ajFileReadLine will trim the tailing \n. */
	if((ajStrGetCharPos(line, 1)=='\0'))
	{
	    ok = ajTrue;
	    break;
	}
    }
    
    
    
    /* Read rest of input file. */
    if(ok)
    {
	/* Write DOMAIN classification records to file. */
	if(!(outf=ajFileNewOutNameS(out)))
	 ajFatal("Could not open output file in domainalign_ProcessStampFile");

	
	if((domain->Type == ajSCOP))
	{
	    ajFmtPrintF(outf,"# TY   SCOP\n# XX\n");
	    ajFmtPrintF(outf,"# CL   %S",domain->Scop->Class);
	    ajFmtPrintSplit(outf,domain->Scop->Fold,"\n# XX\n# FO   ",
			    75," \t\n\r");
	    ajFmtPrintSplit(outf,domain->Scop->Superfamily,"# XX\n# SF   ",
			    75," \t\n\r");
	    ajFmtPrintSplit(outf,domain->Scop->Family,"# XX\n# FA   ",
			    75," \t\n\r");
	    ajFmtPrintF(outf,"# XX\n");
	}
	else
	{
	    ajFmtPrintF(outf,"# TY   CATH\n# XX\n");
	    ajFmtPrintF(outf,"# CL   %S",domain->Cath->Class);
	    ajFmtPrintSplit(outf,domain->Cath->Architecture,"\n# XX\n# AR   ",
			    75," \t\n\r");
	    ajFmtPrintSplit(outf,domain->Cath->Topology,"# XX\n# TP   ",75,
			    " \t\n\r");
	    ajFmtPrintSplit(outf,domain->Cath->Superfamily,"# XX\n# SF   ",75,
			    " \t\n\r");
	    ajFmtPrintF(outf,"# XX\n");
	}
	if((domain->Type == ajSCOP))
	{
	    if(noden==1) 
		ajFmtPrintF(outf,"# SI   %d\n# XX",domain->Scop->Sunid_Class);
	    else if(noden==2)
		ajFmtPrintF(outf,"# SI   %d\n# XX",domain->Scop->Sunid_Fold);
	    else if(noden==3)
		ajFmtPrintF(outf,"# SI   %d\n# XX",
			    domain->Scop->Sunid_Superfamily);
	    else if(noden==4) 	
		ajFmtPrintF(outf,"# SI   %d\n# XX",
			    domain->Scop->Sunid_Family);
	    else
		ajFatal("Node number error in domainalign_ProcessStampFile");
	}
	else
	{
	    if(noden==5) 
		ajFmtPrintF(outf,"# SI   %d\n# XX", domain->Cath->Class_Id);
	    else if(noden==6)
		ajFmtPrintF(outf,"# SI   %d\n# XX", domain->Cath->Arch_Id);
	    else if(noden==7)
		ajFmtPrintF(outf,"# SI   %d\n# XX",domain->Cath->Topology_Id);
	    else if(noden==8)
		ajFmtPrintF(outf,"# SI   %d\n# XX",
			    domain->Cath->Superfamily_Id);
	    else if(noden==9)  
		ajFmtPrintF(outf,"# SI   %d\n# XX",domain->Cath->Family_Id);
	    else
		ajFatal("Node number error in domainalign_ProcessStampFile");
	}   



	while((ajReadlineTrim(inf,&line)))
	{
	    /* Increment counter for block of file. */
	    if((ajStrGetCharPos(line, 1)=='\0'))
	    {
		blk++;
		if(blk==4)
		    blk=1;
	    
		continue;
	    }



	    /* Block of numbering line and protein sequences. */
	    if(blk==1)
	    {
		/* Print the number line out as it is. */
		if(ajStrPrefixC(line,"Number"))
		    ajFmtPrintF(outf,"\n# %7s %S\n"," ", line);
		else
		{
		    /* Read only the 7 characters
		       of the domain identifier
		       code in. */
		    ajFmtScanS(line, "%S", &temp1);
		    ajStrAssignSubS(&temp2, temp1, 0, 6);


		    /* Read the sequence. */
		    ajStrAssignSubS(&temp3, line, 13, 69);
		    ajStrExchangeSetCC(&temp3, " ", "X");
		    ajFmtPrintF(logf, "Replaced ' ' in STAMP alignment "
				"with 'X'\n");
		    ajStrFmtUpper(&temp3);
		

		    /* Write domain id code and sequence out. */
		    ajFmtPrintF(outf,"%-15S%7d %S%7d\n",
				temp2, 0, temp3, 0);
		}
	    }
	    /* Secondary structure filled with '????' (unwanted). */
	    else if(blk==2)
	    {
		continue;
	    }
	    /* Similarity lines. */
	    else
	    {
		if(ajStrPrefixC(line,"Post"))
		{
		    /* Read the sequence. */
		    ajStrAssignSubS(&temp3, line, 13, 69);

		    /* Write post similar line out. */
		    ajFmtPrintF(outf,"%-15s%7s %S\n","# Post_similar", " ",
				temp3);
		}
		/* Ignore Very and Less similar lines. */
		else continue;
	    }
	}
    }
    else /* ok == ajFalse. */
    {
	ajWarn("\n***********************************************\n"
	       "* STAMP was called but output file was EMPTY! *\n"
	       "*   NO OUTPUT FILE GENERATED FOR THIS NODE.   *\n"
	       "***********************************************\n");
	ajFmtPrintF(logf, "STAMP called but output file empty.  "
		    "No output file for this node!");
    }
    


    /* Clean up and close input and output files. */
    ajFileClose(&outf);
    ajFileClose(&inf);
    ajStrDel(&line);
    ajStrDel(&temp1);
    ajStrDel(&temp2);
    ajStrDel(&temp3);
    

    /* All done. */
    return;
}





/* @funcstatic domainalign_writelast ******************************************
**
** House-keeping function.
**
** @param [r] domain [AjPDomain]    Domain.
** @param [r] noden  [ajint]        Node number.
** @param [r] last_node [AjPStr *]  Last node.  
** @param [r] last_nodeid [ajint *] Id of last node.
** 
** @return [void]
** 
** @@
****************************************************************************/
static void domainalign_writelast(AjPDomain domain, 
				  ajint noden, 
				  AjPStr *last_node, 
				  ajint *last_nodeid)
{
    if(noden==1) 
    {
	ajStrAssignS(last_node, domain->Scop->Class);
	*last_nodeid = domain->Scop->Sunid_Class;
    }		       
    else if (noden==2)
    {
	ajStrAssignS(last_node, domain->Scop->Fold);
	*last_nodeid = domain->Scop->Sunid_Fold;
    }
    else if (noden==3)
    {
	ajStrAssignS(last_node, domain->Scop->Superfamily);
	*last_nodeid = domain->Scop->Sunid_Superfamily;
    }
    else if (noden==4)
    {
	ajStrAssignS(last_node, domain->Scop->Family);
	*last_nodeid = domain->Scop->Sunid_Family;
    } 	
    else if (noden==5)
    {
	ajStrAssignS(last_node, domain->Cath->Class);
	*last_nodeid = domain->Cath->Class_Id ;
    } 
    else if (noden==6)
    {
	ajStrAssignS(last_node, domain->Cath->Architecture);
	*last_nodeid = domain->Cath->Arch_Id;
    } 
    else if (noden==7)
    {
	ajStrAssignS(last_node, domain->Cath->Topology);
	*last_nodeid = domain->Cath->Topology_Id;
    } 
    else if (noden==8)
    {
	ajStrAssignS(last_node, domain->Cath->Superfamily);
	*last_nodeid = domain->Cath->Superfamily_Id;
    } 
    else if (noden==9)
    {
	/* There is no text describing the CATH families. */
	ajFmtPrintS(last_node, "%d", domain->Cath->Family_Id);
	*last_nodeid = domain->Cath->Family_Id;
    } 
}



/* @funcstatic domainalign_writeid ********************************************
**
** House-keeping function.
**
** @param [r] domain [AjPDomain]   Domain.
** @param [r] noden  [ajint]       Node number.
** @param [r] daf    [AjPDirout]   Domain alignment files.
** @param [r] super  [AjPDirout]   Structural superimposition files.
** @param [r] align  [AjPStr *]    Align.
** @param [r] alignc [AjPStr *]    Alignc.
** 
** @return [void]
** 
** @@
****************************************************************************/
static void domainalign_writeid(AjPDomain domain, 
				ajint noden, 
				AjPDirout daf, 
				AjPDirout super, 
				AjPStr *align, 
				AjPStr *alignc) 
{
    AjPStr temp=NULL;
    
    temp=ajStrNew();
    
    if(noden==1) 
	ajStrFromInt(&temp, domain->Scop->Sunid_Class);
    else if (noden==2)
	ajStrFromInt(&temp, domain->Scop->Sunid_Fold);
    else if (noden==3)
	ajStrFromInt(&temp, domain->Scop->Sunid_Superfamily);
    else if (noden==4)
	ajStrFromInt(&temp, domain->Scop->Sunid_Family);
    else if (noden==5)
	ajStrFromInt(&temp, domain->Cath->Class_Id );
    else if (noden==6)
	ajStrFromInt(&temp, domain->Cath->Arch_Id);
    else if (noden==7)
	ajStrFromInt(&temp, domain->Cath->Topology_Id);
    else if (noden==8)
	ajStrFromInt(&temp, domain->Cath->Superfamily_Id);
    else if (noden==9)
	ajStrFromInt(&temp, domain->Cath->Family_Id);

    ajStrAssignS(align, temp);	
    ajStrInsertS(align, 0, ajDiroutGetPath(daf));	
    ajStrAppendC(align, ".");
    ajStrAppendS(align, ajDiroutGetExt(daf));

    ajStrAssignS(alignc, temp);	
    ajStrInsertS(alignc, 0, ajDiroutGetPath(super));	
    ajStrAppendC(alignc, ".");
    ajStrAppendS(alignc, ajDiroutGetExt(super));

    ajDebug("daf file '%S' super file '%S'\n", *align, *alignc);
    ajStrDel(&temp);
    return;
}






/* @funcstatic domainalign_writesid *******************************************
**
** House-keeping function.
**
** @param [r] domain [AjPDomain]   Domain.
** @param [r] noden  [ajint]       Node number.
** @param [r] id     [AjPStr *]    Id.
** 
** @return [void]
** 
** @@
****************************************************************************/
static 	void domainalign_writesid(AjPDomain domain, 
				  ajint noden, 
				  AjPStr *id)
{
    if(noden==1) 
	ajStrFromInt(id, domain->Scop->Sunid_Class);
    else if (noden==2)
	ajStrFromInt(id, domain->Scop->Sunid_Fold);
    else if (noden==3)
	ajStrFromInt(id, domain->Scop->Sunid_Superfamily);
    else if (noden==4)
	ajStrFromInt(id, domain->Scop->Sunid_Family);
    else if (noden==5)
	ajStrFromInt(id, domain->Cath->Class_Id );
    else if (noden==6)
	ajStrFromInt(id, domain->Cath->Arch_Id);
    else if (noden==7)
	ajStrFromInt(id, domain->Cath->Topology_Id);
    else if (noden==8)
	ajStrFromInt(id, domain->Cath->Superfamily_Id);
    else if (noden==9)
	ajStrFromInt(id, domain->Cath->Family_Id);

    return;
}



/* @funcstatic domainalign_stamp **********************************************
**
** Call STAMP and process files.
**
** @param [r] prevdomain [AjPDomain] Previous domain.
** @param [r] domain [AjPDomain] This domain.
** @param [r] daf [AjPDirout] Domain alignment files.
** @param [r] super [AjPDirout] Superimposition files.
** @param [r] singlets [AjPDirout]  Singlet files.
** @param [r] align [AjPStr]   Align.
** @param [r] alignc [AjPStr] Alignc.
** @param [r] dom [AjPStr]   Dom.
** @param [r] name [AjPStr] Name.
** @param [r] set [AjPStr] Name of set file.
** @param [r] scan [AjPStr] Name of scan file.
** @param [r] sort [AjPStr] Name of sort file.
** @param [r] log [AjPStr] Lof file name.
** @param [r] out [AjPStr] Out file name.
** @param [r] keepsinglets [AjBool] Keep singlet sequences or not.
** @param [r] moden [ajint] Mode number.
** @param [r] noden [ajint] Node number.
** @param [r] nset [ajint] Number in set.
** @param [r] logf [AjPFile] Lof file.
** 
**
** @return [void] True on success
** @@
****************************************************************************/
static void domainalign_stamp(AjPDomain prevdomain,
			      AjPDomain domain, 
			      AjPDirout daf, 
			      AjPDirout super,
			      AjPDirout singlets, 
			      AjPStr    align, 
			      AjPStr    alignc, 
			      AjPStr    dom, 
			      AjPStr    name, 
			      AjPStr    set, 
			      AjPStr    scan, 
			      AjPStr    sort, 
			      AjPStr    log, 
			      AjPStr    out, 
			      AjBool    keepsinglets, 
			      ajint     moden, 
			      ajint     noden,
			      ajint     nset, 
			      AjPFile   logf)
{
    AjPStr    exec      = NULL;	/* The UNIX command line to be executed.   */
    AjPFile   clusterf  = NULL;	/* File pointer for log file.              */
    ajint     ncluster  = 0;	/* Counter for the number of clusters.     */
    AjPStr    line      = NULL;	/* Holds a line from the log file.         */
    AjPRegexp rexp      = NULL;	/* For parsing no. of clusters in log file */
    AjPStr    temp      = NULL;	/* A temporary string.                     */
    ajint     x         = 0;    /* Loop counter.                           */
    

    exec     = ajStrNew();
    line     = ajStrNew();
    temp     = ajStrNew();



    rexp     = ajRegCompC("^(Cluster:)");

    ajDebug("domainalign_stamp name: '%S'\n", name);
    
    /* Call STAMP. */
    ajFmtPrintS(&exec,	"stamp -l %S -s -n 2 -slide 5 -prefix %S -d %S",
		dom, name, set);
    ajFmtPrint("\n%S\n\n", exec);
    ajSysSystem(exec);  

    ajFmtPrintS(&exec, "sorttrans -f %S -s Sc 2.5",
		scan);
    ajFmtPrint("\n%S > %S\n\n", exec, sort);

    ajSysSystemOut(exec, sort);

    ajFmtPrintS(&exec, "stamp -l %S -prefix %S",
		sort, name);
    ajFmtPrint("\n%S > %S\n\n", exec, log);
    ajSysSystemOut(exec, log);
	
    ajFmtPrintS(&exec, "transform -f %S -g  -o %S",
		sort, alignc);
    ajFmtPrint("\n%S\n\n", exec);
    ajSysSystem(exec);
    
    
    /* Count the number of clusters in the log file. */
    if(!(clusterf=ajFileNewInNameS(log)))
	ajFatal("Could not open log file '%S'\n", log);
    ncluster=0;
    while(ajReadlineTrim(clusterf,&line))
	if(ajRegExec(rexp,line))
	    ncluster++;
    ajFileClose(&clusterf);	

    ajDebug("ncluster: %d\n", ncluster);
    
    /* Call STAMP ... calculate two fields for structural equivalence using 
       threshold Pij value of 0.5, see stamp manual v4.1 pg 27. */
    ajFmtPrintS(&exec,"poststamp -f %S.%d -min 0.5",
		name, ncluster);
    ajFmtPrint("%S\n\n", exec);
    ajSysSystem(exec);
    
    
    /* Call STAMP ... convert block format alignment into clustal format. */
    ajFmtPrintS(&exec,"ver2hor -f %S.%d.post",
		name, ncluster); 
    ajFmtPrint("%S > %S\n\n", exec, out);
    ajSysSystemOut(exec, out);
    
    
    /* Process STAMP alignment file and generate alignment file for output. */
    domainalign_ProcessStampFile(out, align, prevdomain, noden, logf);
    
    
    /* Remove all temporary files. */
    
    for(x=1;x<ncluster+1;x++)
    {
	ajFmtPrintS(&temp, "%S.%d", name, x);
	ajSysFileUnlink(temp); 
    }
    
    ajFmtPrintS(&temp, "%S.%d.post", name, ncluster);
    ajSysFileUnlink(temp); 

    ajStrDel(&exec);
    ajStrDel(&line);
    ajStrDel(&temp);
    ajRegFree(&rexp);

    return;
}   


/* @funcstatic domainalign_tcoffee ********************************************
**
** Call TCOFFEE and process files.
**
** @param [r] domain [AjPDomain] Domain being aligned
** @param [r] in [AjPStr] Name of TCOFFEE input file
** @param [r] align [AjPStr] Name of sequence alignment file for output
** @param [r] alignc [AjPStr] Name of structure alignment file for output
** @param [r] pdbnames [AjPStr] Names of pdb files to be passed to TCOFFEEE 
** @param [r] noden [ajint] Node-level of alignment
** @param [r] logf [AjPFile] Log file.
**
** @return [void] True on success
** @@
****************************************************************************/
static void domainalign_tcoffee(AjPDomain domain, 
				AjPStr    in, 
				AjPStr    align, 
				AjPStr    alignc, 
				AjPStr    pdbnames, 
				ajint     noden, 
				AjPFile   logf)
			 
{
    AjPStr    exec      = NULL;	/* The UNIX command line to be executed*/

    exec     = ajStrNew();


    /* '-in' is indeed a file sepecifier. 
       The 'pdb1 pdb2 pdb3' string contains the file (including the path) of the 
       clean domain cordinate files (pdb format) for the structural alifnment 
       e.g. d1vsc_2.ent. 
       '-outfile' is the clustal format alignment output file. */

    ajFmtPrintS(&exec,"t_coffee -in %S sap_pair", pdbnames);
    ajFmtPrint("%S > %S\n", exec, in);
    ajSysSystemOut(exec, in);

    /* Process tcoffee alignment file and generate alignment file 
       for output. */
    domainalign_ProcessTcoffeeFile(in, align, domain, noden, logf);

    ajStrDel(&exec);

    return;
}





/* @funcstatic domainalign_ProcessTcoffeeFile *********************************
**
** Parses tcoffee output.
**
** @param [r] in [AjPStr] Name of TCOFFEE input file
** @param [r] align [AjPStr] Name of sequence alignment file for output
** @param [r] domain [AjPDomain] Domain being aligned
** @param [r] noden [ajint] Node-level of alignment** 
** @param [r] logf [AjPFile] Log file
**
** @return [void] True on success
** @@
****************************************************************************/
static void domainalign_ProcessTcoffeeFile(AjPStr in, 
					   AjPStr align, 
					   AjPDomain domain,
					   ajint noden, 
					   AjPFile logf)
{
    AjPFile  outf = NULL;  /* Output file pointer. */
    AjPFile   inf = NULL;  /* Input file pointer. */
    AjPStr  temp1 = NULL;  /* Temporary string. */
    AjPStr  temp2 = NULL;  /* Temporary string. */
    AjPStr  temp3 = NULL;  /* Temporary string. */
    AjPStr   line = NULL;  /* Line of text from input file. */
    
    
    /* Initialise strings. */
    line    = ajStrNew();
    temp1   = ajStrNew();
    temp2   = ajStrNew();
    temp3   = ajStrNew();



    /* Open input and output files. */
    if(!(inf=ajFileNewInNameS(in)))
        ajFatal("Could not open input file in domainalign_ProcessTcoffeeFile");
    if(!(outf=ajFileNewOutNameS(align)))
        ajFatal("Could not open output file in domainalign_ProcessTcoffeeFile");
    

    /*Write DOMAIN classification records to file*/
    if((domain->Type == ajSCOP))
    {
	ajFmtPrintF(outf,"# TY   SCOP\n# XX\n");
	ajFmtPrintF(outf,"# CL   %S",domain->Scop->Class);
	ajFmtPrintSplit(outf,domain->Scop->Fold,"\n# XX\n# FO   ",75," \t\n\r");
	ajFmtPrintSplit(outf,domain->Scop->Superfamily,"# XX\n# SF   ",75," \t\n\r");
	ajFmtPrintSplit(outf,domain->Scop->Family,"# XX\n# FA   ",75," \t\n\r");
	ajFmtPrintF(outf,"# XX\n");
    }
    else
    {
	ajFmtPrintF(outf,"# TY   CATH\n# XX\n");
	ajFmtPrintF(outf,"# CL   %S",domain->Cath->Class);
	ajFmtPrintSplit(outf,domain->Cath->Architecture,"\n# XX\n# AR   ",75," \t\n\r");
	ajFmtPrintSplit(outf,domain->Cath->Topology,"# XX\n# TP   ",75," \t\n\r");
	ajFmtPrintSplit(outf,domain->Cath->Superfamily,"# XX\n# SF   ",75," \t\n\r");
	ajFmtPrintF(outf,"# XX\n");
    }
    
    if((domain->Type == ajSCOP))
    {
	if(noden==1) 
	    ajFmtPrintF(outf,"# SI   %d\n# XX\n",domain->Scop->Sunid_Class);
	else if(noden==2)
	    ajFmtPrintF(outf,"# SI   %d\n# XX\n",domain->Scop->Sunid_Fold);
	else if(noden==3)
	    ajFmtPrintF(outf,"# SI   %d\n# XX\n",domain->Scop->Sunid_Superfamily);
	else if(noden==4) 	
	    ajFmtPrintF(outf,"# SI   %d\n# XX\n", domain->Scop->Sunid_Family);
	else
	    ajFatal("Node number error in domainalign_ProcessStampFile");
    }	
    else
    {
	    if(noden==5) 
		ajFmtPrintF(outf,"# SI   %d\n# XX\n", domain->Cath->Class_Id);
	    else if(noden==6)
		ajFmtPrintF(outf,"# SI   %d\n# XX\n", domain->Cath->Arch_Id);
	    else if(noden==7)
		ajFmtPrintF(outf,"# SI   %d\n# XX\n",domain->Cath->Topology_Id);
	    else if(noden==8)
		ajFmtPrintF(outf,"# SI   %d\n# XX\n",domain->Cath->Superfamily_Id);
	    else if(noden==9)  
		ajFmtPrintF(outf,"# SI   %d\n# XX\n",domain->Cath->Family_Id);
	    else
		ajFatal("Node number error in domainalign_ProcessStampFile");
    }   


    
    /* Start of code for reading input file. */
    /*Ignore everything up to first line beginning with 'Number'*/
    while((ajReadlineTrim(inf,&line)))
        /* ajFileReadLine will trim the tailing \n. */
        if((ajStrGetCharPos(line, 1)=='\0'))
            break;

    
    /* Read rest of input file. */
    while((ajReadlineTrim(inf,&line)))
    {
      if((ajStrGetCharPos(line, 1)=='\0'))
        continue; 
        
       /* Print the number line out as it is. */
            else if(ajStrPrefixC(line,"CLUSTAL"))
              continue;
	    else if(ajStrPrefixC(line," "))
                ajFmtPrintF(outf,"\n");
        /* write out a block of protein sequences. */
        else
        {
               /* Read only the 7 characters of the domain identifier code in. */
               ajFmtScanS(line, "%S %S", &temp1,&temp3);
                 ajStrAssignSubS(&temp2, temp1, 0, 6);
  
  
         /* Read the sequence
                   ajStrAssignSubS(&temp3, line, 13, 69);
                   ajStrExchangeSetCC(&temp3, " ", "X");
                   ajStrFmtUpper(&temp3);*/
                
  
                   /* Write domain id code and sequence out. */
                   ajFmtPrintF(outf,"%-13S%S\n",temp2, temp3);              
        }
    }
    

    /* Clean up and close input and output files. */
    ajFileClose(&outf);
    ajFileClose(&inf);
    ajStrDel(&line);
    ajStrDel(&temp1);
    ajStrDel(&temp2);
    ajStrDel(&temp3);
    

    /* All done. */
    return;
}





/* @funcstatic domainalign_keepsinglets ***************************************
**
** Write singlet sequences to file.
**
** @param [r] domain [AjPDomain] 
** @param [r] noden [ajint] 
** @param [r] singlets [AjPDirout] 
** @param [r] logf [AjPFile] 
**
** @return [void] True on success
** @@
****************************************************************************/
static void domainalign_keepsinglets(AjPDomain domain,
				     ajint     noden, 
				     AjPDirout singlets, 	
				     AjPFile   logf)
{
    AjPStr      temp2     = NULL;   /* A temporary string. */
    AjPFile     singf     = NULL;   /* File pointer for singlets file. */
    EmbPHitlist  hitlist   = NULL;   /* Hitlist object for output of data. */
    
    temp2    = ajStrNew();


    domainalign_writesid(domain, noden, &temp2);
    
    if(MAJSTRGETLEN(ajDomainGetSeqPdb(domain)))
    {
	/* Write Hit object. */
	hitlist = embHitlistNew(1);
	hitlist->Type = domain->Type;
	ajStrAssignS(&hitlist->hits[0]->Seq, ajDomainGetSeqPdb(domain));
	ajStrAssignS(&hitlist->hits[0]->Acc, ajDomainGetAcc(domain));
	ajStrAssignS(&hitlist->hits[0]->Spr, ajDomainGetSpr(domain));
	ajStrAssignS(&hitlist->hits[0]->Dom, ajDomainGetId(domain));
	

	if((domain->Type == ajSCOP))
	{
	    ajStrAssignS(&hitlist->Class, domain->Scop->Class);
	    ajStrAssignS(&hitlist->Fold, domain->Scop->Fold);
	    ajStrAssignS(&hitlist->Superfamily, domain->Scop->Superfamily);
	    ajStrAssignS(&hitlist->Family, domain->Scop->Family);
	    if(noden==4)
		hitlist->Sunid_Family = domain->Scop->Sunid_Family;
	    else if(noden==3)
		hitlist->Sunid_Family = domain->Scop->Sunid_Superfamily;
	    else if(noden==2)
		hitlist->Sunid_Family = domain->Scop->Sunid_Fold;
	    else if(noden==1)
		hitlist->Sunid_Family = domain->Scop->Sunid_Class;
	}
	else
	{	
	    ajStrAssignS(&hitlist->Class, domain->Cath->Class);
	    ajStrAssignS(&hitlist->Architecture, domain->Cath->Architecture);
	    ajStrAssignS(&hitlist->Topology, domain->Cath->Topology);
	    ajStrAssignS(&hitlist->Superfamily, domain->Cath->Superfamily);

	    if(noden==9)
		hitlist->Sunid_Family = domain->Cath->Family_Id;
	    if(noden==8)
		hitlist->Sunid_Family = domain->Cath->Superfamily_Id;
	    else if(noden==7)
		hitlist->Sunid_Family = domain->Cath->Topology_Id;
	    else if(noden==6)
		hitlist->Sunid_Family = domain->Cath->Arch_Id;
	    else if(noden==5)
		hitlist->Sunid_Family = domain->Cath->Class_Id;

	}

	singf = ajFileNewOutNameDirS(temp2, singlets);
	embHitlistWriteFasta(singf, hitlist);
	/* ajFmtPrintF(singf, ">%S\n%S\n", temp2, ajDomainGetSeqPdb(domain)); */
	ajFileClose(&singf);
    }
    else
    {
	ajWarn("No sequence found (no 'DS' record in DCF file) for singlet %S "
	       "despite configuring to save singlets. "
	       "NO file written!", ajDomainGetId(domain)); 
	ajFmtPrintF(logf, "No sequence found (no 'DS' record in DCF file) for "
		    "singlet %S despite configuring to save singlets. "
		    "NO file written!", ajDomainGetId(domain));
    }		

    ajStrDel(&temp2); 

    return;
}    

