/* @source seqsearch application
**
** Generate files of hits for scop family alignments by using PSI-BLAST.
**
** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
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
** 
*******************************************************************************
**  Application name
**  seqsearch
**  
**  
**  
**  Summary
**  Generate files of hits for scop family alignments by using psiblast.
**  
**  
**  
**  Input and Output
**  seqsearch reads a directory of scop family alignments and generates for each
**  one a file in embl-like format of sequence relatives (hits) to the family,  
**  called a scop hits file.   The hits are found by using PSIBLAST and this 
**  application must be installed on the system that is running seqsearch (see
**  'Notes' below).  In cases where an alignment file contains scop 
**  classification data but no alignment, seqsearch will extract from a scop 
**  classification file a domain sequence for the family and find hits to that
**  instead.  Such cases will occur when the alignment file was generated by
**  using scopalign on a family with a single domain only.  The scop Sunid (an 
**  integer) of a family will be used as the base name for its output file. 
**  The paths and extensions for the scop family alignment files (input) and
**  scop hits files (output) are specified by the user.  The name of the scop
**  classification file and BLAST-indexed database to search are also user-
**  specified.  A log file is also written.
**  
**  
**
**  Sister applications
**  A 'scop family alignment file' contains a sequence alignment of domains 
**  belonging to the same scop family.  The file is in embl-like format and is
**  annotated with records describing the SCOP classification of the family.  A
**  scop family alignment file generatd by scopalign will contain a structure-
**  based sequence alignment of domains of known structure only.  Such 
**  alignments can be extended with sequence relatives (of unknown structure) to
**  the family in question by using seqalign.
**  A 'scop classification file' contains classification and other data for 
**  domains from the scop database.  The file is in embl-like format and is 
**  generated by scopparse.  Domain sequence information can be added to the 
**  file by using scopseqs.
**  scop hits files are used by fraggle and seqsort.
**  
**  
**  
**  Notes
**  When running seqsearch at the HGMP it is essential that the command 
**  'use blast_v2' (which runs the script /packages/menu/USE/blast_v2) is given 
**  before it is run. 
**  
**  seqsearch requires a blast-indexed database to be present, i.e. both the 
**  sequence and index file must be present on the system.  The name of the 
**  database to search specified in the acd file is that which is given as the 
**  -d parameter to blastpgp, e.g. (blastpgp -d swissprot).
**  
**  
**  
**  Known bugs & warnings
**  
**  
**  
**  Description
**  This program is part of a suite of EMBOSS applications that directly or 
**  indirectly make use of the protein structure databases pdb and scop.  
**  This program is part of an experimental analysis pipeline described in an
**  accompanying document.  We provide the software in the hope that it will
**  be useful.  The applications were designed for specific research purposes
**  and may not be useful or reliable in contexts other than the described 
**  pipeline.  The development of the suite was coordinated by Jon Ison to
**  whom enquiries and bug reports should be sent (email jison@hgmp.mrc.ac.uk).
**  
**  By using homology search tools such as blast it is possible to find 
**  relatives to a protein family, given one or more sequences belonging to 
**  the family of interest.  When using psiblast it is possible to use a 
**  sequence alignment as the seed with which to search a sequence database.
**  Performing such searches for large datasets such as all scop families 
**  potentially requires a lot of time for preparation of datasets, running
**  jobs and so on, in addition to the compute time required for the 
**  searches themselves.  seqsearch automatically performs a psiblast search
**  of a sequence database for every scop family represented in a directory
**  of sequence alignments; the alignments are used as seeds for the 
**  searches.
**  
**  
**  
**  Algorithm
**  A recent paper on psiblast:
**  Altschul et al, Nuc. Acids. Res. 25:3389-3402, 1997
**  
**  
**  
**  Usage 
**  An example of interactive use of seqsearch is shown below.
**  Unix % 
**  Unix % use blast_v2
**  Unix % 
**  Unix % seqsearch
**  Generate files of hits for families in a scop classification file by using
**  PSI-BLAST with seed alignments.
**  Name of scop classification file (embl format input): /test_data/all.scop
**  Location of scop alignment files (input) [./]: /test_data
**  Extension of scop alignment files [.salign]: 
**  Name of BLAST-indexed database to search [swissprot]: 
**  Number of PSIBLAST iterations [1]: 1
**  Threshold E-value for inclusion in family [0.001]: 0.0001
**  Maximum number of hits [1000]: 100
**  Location of scop hits files (output) [./]: /test_data
**  Extension of scop hits files [.hits]: 
**  Name of log file for the build [seqsearch.log]: /test_data/seqsearch.log
**  PROCESSING /test_data/55074.salign
**  blastpgp -i ./seqsearch-1031751107.24970.seqin -B ./seqsearch-1031751107.24970.seqsin -j 1 -e 0.000100 -b 100 -v 100 -d swissprot > ./seqsearch-1031751107.24970.psiout
**  
**  [blastpgp] WARNING: posFindAlignmentDimensions: Attempting to recover data from multiple alignment file
**  
**  [blastpgp] WARNING: posProcessAlignment: Alignment recovered successfully
**  
**  All scop alignment files with the file extension .salign were read from the
**  directory /test_data; in this case a single scop alignment file 
**  55074.salign was read.  As the alignment file contained sequences it was 
**  not necessary to extract sequence information from the scop classification
**  file /test_data/all.scop.  A set of sequence from the scop alignment file
**  was used to search the sequence database swissprot using psiblast.  
**  psiblast was configured to perform 1 iteration with a threshold E-value 
**  for acceptance of a hit of 0.0001 and no more than 100 hits were generated
**  from each iteration.  Hits file were specified to be written to /test_data
**  and have the file extension .hits; in this case a single hits file called
**  /test_data/55074.hits was written.  A log file called 
**  /test_data/seqsearch.log was also written.
**  
**  The following command line would achieve the same result.
**  seqsearch /test_data/all.scop /test_data .salign swissprot /test_data 
**  .hits -niter 1 -evalue 0.0001 -maxhits 100 -logf seqsearch.log
**  
**  
**  
**  Input file format
**  The format of the scop classification file is described in scopparse.c
**  The format of the scop alignment file is described in scopalign.c
**
**  
**  
**  Output file format
**  The output file (below) uses the following records 
**  The following four SCOP classification records are taken from the alignment 
**  input file: 
**  (1)  CL - Domain class.  It is identical to the text given after 'Class' in 
**  the scop classification file (see documentation for the EMBOSS application 
**  scope). 
**  (2)  FO - Domain fold.  It is identical to the text given after 'Fold' in 
**  the scop classification file (see scope documentation). 
**  (3)  SF - Domain superfamily.  It is identical to the text given after 
**  'Superfamily' in the scop classification file (see scope documentation). 
**  (4)  FA - Domain family. It is identical to the text given after 'Family' in 
**  the scop classification file (see scope documentation). 
**  (5)  SI - SCOP Sunid's of the family. This number uniquely identifies the 
**  family in the SCOP parsable files.
**  (6)  NS - Number in set. The number of hits for this family. The file will 
**  have a section containing an NN, AC, CL, RA and SQ records (see below) for 
**  each sequence in the set. 
**  (7) NN - Sequence number.  The number given in brackets after this record 
**  indicates the start of the data for the relevent sequence. 
**  (8) AC - Accession number of the hit. 
**  (9) TY - Classification of hit.  Has the value HIT (for sequences retrieved 
**  by psi-blast).
**  (Files of this type are also generated by the EMBOSS application swissparse 
**  and may be be hand edited with additional sequences, in either case, the 
**  value OTHER will be given for this record). Files of this type that have
**  been processed by seqnr may have 'SEED' given for this record.
**  (10) SC - Score of hit.  A floating point value that indicates a score, e.g.
**  from a database search algorithm.  
**  (This record is written by seqsearch but always has a value of 0.0. Other 
**  applications may write a non-zero value into this record).
**  (11) RA - Sequence range. The numbers before START and END give the start and 
**  end positions respectively of the hit relative to the full length sequence 
**  in the swissprot database (a '.' may be given for swissparse output files - 
**  see swissparse  documentation). 
**  (12)  SQ - protein sequence. The number of residues is given before AA on the 
**  first line. The protein sequence is given on subsequent lines. 
**  (13) MO - Model type.  The type of model that was used to generate the hit.
**  May have a value of "HMM" (hidden Markov model), "Gribskov" (Gribskov 
**  profile), "Henikoff" (Henikoff profile) or "Signature" (sparse protein 
**  signature).  This record is not written by seqsearch but scop hits file with
**  this record written may be generated by using dbscan.
**  (14) XX - used for spacing. 
**  (15) // - used to delimit data for each psiblast run (family).
** 
**  Excerpt from seqsearch output file 
**  CL   All alpha proteins 
**  XX 
**  FO   Globin-like 
**  XX 
**  SF   Globin-like 
**  XX 
**  FA   Globins 
**  XX
**  SI   14982
**  XX 
**  NS   2 
**  XX 
**  NN   [1] 
**  XX 
**  TY   HIT
**  XX 
**  SC   0.0
**  XX   
**  AC   HBDEX1 
**  XX 
**  RA   2 START; 79 END; 
**  XX 
**  SQ   SEQUENCE   141 AA;  15127 MW;  5EC7DB1E CRC32; 
**       VLSPADKTNV KAAWGKVGAH AGEYGAEALE RMFLSFPTTK TYFPHFDLSH GSAQVKGHGK 
**       KVADALTNAV AHVDDMPNAL SALSDLHAHK LRVDPVNFKL LSHCLLVTLA AHLPAEFTPA 
**       VHASLDKFLA SVSTVLTSKY R 
**  XX 
**  NN   [2] 
**  XX 
**  TY   HIT
**  XX 
**  SC   0.0
**  XX   
**  AC   HBDEX2 
**  XX 
**  RA   2 START; 79 END; 
**  XX 
**  SQ   SEQUENCE   141 AA;  15127 MW;  5EC7DB1E CRC32; 
**       VLSPADKTNV KAAWGKVGAH AGEYGAEALE RMFLSFPTTK TYFPHFDLSH GSAQVKGHGK 
**       KVADALTNAV AHVDDMPNAL SALSDLHAHK LRVDPVNFKL LSHCLLVTLA AHLPAEFTPA 
**       VHASLDKFLA SVSTVLTSKY R 
**  XX 
**  // 
**  CL   All alpha proteins 
**  XX 
**  FO   Globin-like 
**  XX 
**  SF   Globin-like 
**  XX 
**  FA   Phycocyanins 
**  XX 
**  XX
**  SI   14321
**  
**  
**  
**  Data files
**  seqsearch does not requires any data files.
**  
**  
**  
**  Diagnostic error messages
**  
**  
**  
**  Authors
**  Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
**  Jon Ison (jison@hgmp.mrc.ac.uk)
**  
**  
**  
**  References
**  Please cite the authors.
**  
**  A recent paper on psiblast:
**  Altschul et al, Nuc. Acids. Res. 25:3389-3402, 1997
**
** 
**  
******************************************************************************/ 
        

#include "emboss.h"




static AjPHitlist seqsearch_ajDmxHitlistPsiblast(AjPScopalg scopalg, AjPFile psif);
static AjPFile seqsearch_ajDmxScopalgPsiblast(AjPScopalg scopalg, AjPFile alignf, 
                                       AjPStr *psiname, ajint niter, 
                                       ajint maxhits, float evalue, 
                                       AjPStr database);
static AjPFile seqsearch_ajDmxScopPsiblast(AjPStr singlet, AjPStr *psiname, 
                                           ajint niter, ajint maxhits, 
                                           float evalue,  AjPStr database);




/* @prog seqsearch *******************************************************
**
** Generate files of hits for scop family alignments by using PSI-BLAST.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr     align     = NULL;   /* Location of alignment files for input */
    AjPStr     alignextn = NULL;   /* File extension of alignment files */
    AjPStr     alignname = NULL;   /* Name of alignment file */
    
    AjPStr     hits      = NULL;   /* Location of hits files for output */
    AjPStr     hitsextn  = NULL;   /* File extension of hits files */
    AjPStr     hitsname  = NULL;   /* Name of hits file */
    AjPStr     singlet   = NULL;   /* sequence of a particular sunid */ 
    
    AjPStr     msg       = NULL;   /* Error message */
    AjPStr     temp      = NULL;   /* Temp string */
    AjPStr     psiname   = NULL;   /* Name of psiblast output file */
    AjPStr     database  = NULL;   /* Name of blast-indexed database to search */
    
    AjPFile    families  = NULL;   /* Pointer to families file for output */
    AjPFile    logf      = NULL;   /* Log file pointer */
    AjPFile    psif      = NULL;   /* Pointer to psiblast output file*/
    AjPFile    alignf    = NULL;   /* Alignment file pointer */
    AjPFile    escop     = NULL;   /* File pointer to Escop_seqs.dat */
    
    ajint      maxhits   = 0;      /* Maximum number of hits reported by PSIBLAST */          
    ajint      niter     = 0;      /* Number of PSIBLAST iterations */          
    float      evalue    = 0.0;    /* Threshold E-value for inclusion in family */
    
    AjPList    list      = NULL;   /* Used to hold list of names of files in a directory */
    AjPList    scoplist  = NULL;   /* a list of scop objects */
    AjPScop    scoptmp   = NULL;   /* Scop pointer for housekeeping*/
        
    AjPList    listin     = NULL;   /* a list of hitlist for eleminating the identical hits */
    AjPList    listout    = NULL;   /* a list of scophit for eleminating the identical hits */
    
    AjPScopalg scopalg   = NULL;   /*Scop alignment from input file */
    
    AjPHitlist tmphitlist = NULL;
    AjPHitlist hitlist   = NULL;   /* Hitlist object for holding results of 
                                      PSIBLAST hits*/  
    
    AjIList    iter       = NULL;
    AjPScophit hit        = NULL;   /* for eleminating identical hits */
    AjPScophit nexthit    = NULL;   /* for eleminating identical hits */
    
    
    /* Initialise strings etc */
    alignname = ajStrNew();
    hitsname = ajStrNew();
    msg       = ajStrNew();
    temp      = ajStrNew();
    psiname   = ajStrNew();
    singlet   = ajStrNew();
    
    list     = ajListNew();


    /* Read data from acd */
    ajNamInit("emboss");
    ajAcdInitP("seqsearch",argc,argv,"DOMAINATRIX"); 
    align      = ajAcdGetString("align");
    alignextn  = ajAcdGetString("alignextn");
    hits       = ajAcdGetString("hits");
    hitsextn   = ajAcdGetString("hitsextn");
    maxhits    = ajAcdGetInt("maxhits");
    niter      = ajAcdGetInt("niter");
    evalue     = ajAcdGetFloat("evalue");
    logf       = ajAcdGetOutfile("logf");
    escop      = ajAcdGetInfile("escop");
    database   = ajAcdGetString("database");    
    
    /* Check directories */
    if(!ajFileDir(&align))
        ajFatal("Could not open alignments directory");
    if(!ajFileDir(&hits))
        ajFatal("Could not open hits directory");
    
    
    /* Create list of files in align directory */
    ajStrAssC(&temp, "*");      
    if((ajStrChar(alignextn, 0)=='.'))
        ajStrApp(&temp, alignextn);    
    else
    {
        ajStrAppC(&temp, ".");    
        ajStrApp(&temp, alignextn);    
    }
    
    ajFileScan(align,temp,&list,ajFalse,ajFalse,NULL,NULL,ajFalse,NULL); 
    
    /* create a list of scop objects */
    scoplist = embScopReadAllNew(escop);
    ajFileClose(&escop);
    
    /* need to sort the list before binary search otherwise does not work */
    ajListSort(scoplist,ajScopMatchSunid);
    
    /*Start of main application loop*/   
    while(ajListPop(list,(void **)&alignname))
    { 
	listin    = ajListNew();
        listout   = ajListNew();

	ajFmtPrint("PROCESSING %S\n", alignname);
	ajFmtPrintF(logf, "//\n%S\n", alignname);

        /* Open alignment file*/
        if((alignf = ajFileNewIn(alignname)) == NULL)
        {
            ajFmtPrintS(&msg, "Could not open for reading %S", 
                        alignname);
            ajWarn(ajStrStr(msg));
            ajFmtPrintF(logf, "WARN  Could not open for reading %S\n", 
                        alignname);
	    ajListDel(&listin);
	    ajListDel(&listout);
            continue;       
        }
        
        /* Read alignment file */
        ajDmxScopalgRead(alignf, &scopalg);

        /* seqsearch is hard-coded to give to scan swissprot, (-d swissprot  option 
           to blastpgp. This is probably specific to use on the HGMP server. Option
           to run seqsearch on any blast-indexed database will be implemented in the 
           future */
        /* ajStrAssC(&database, "swissprot"); */

        /* check if the file contains a singlet or an alignment */
        if(scopalg->N==0)
        {
	    ajFmtPrintF(logf, "No sequences in alignment (singlet)\n");
            if(!(ajDmxScopSeqFromSunid(scopalg->Sunid_Family,&singlet,scoplist)))
                ajFatal("The bin search does not work! email rranasin@hgmp.mrc.ac.uk\n");
            
            /* Generate input files for psiblast from a singlet and callpsiblast */
            if(!(psif = seqsearch_ajDmxScopPsiblast(singlet,&psiname,niter,maxhits,
						    evalue,database)))
                ajFatal("Error creating psiblast file"); 
        }

        else
            /* Generate input files from an alignment for psiblast and callpsiblast */
            if(!(psif = seqsearch_ajDmxScopalgPsiblast(scopalg,alignf,&psiname,niter,
						       maxhits,evalue,database)))
                ajFatal("Error creating psiblast file");

        /*  Parse the Psi-Blast output file and write a Hitlist object */
        tmphitlist = seqsearch_ajDmxHitlistPsiblast(scopalg, psif);
        
        /* Close alignment file and delete psiblast output file*/
        ajFileClose(&alignf);
	
	 
	ajFmtPrintS(&temp, "rm %S", psiname);
	system(ajStrStr(temp));
	


	/*
	   ajFmtPrint("***NAME OF PSIBLAST OUTPUT FILE ---> %S", psiname); 
	   system(ajStrStr(temp));
	   */


	/* START NEW STUFF */

	/* RAND's new mods on 30.07.2002 */
        /* Create hits output file name - the name will
           be the same as the Sunid for the SCOP family */
        ajStrAssS(&hitsname, hits);
        ajStrFromInt(&temp, scopalg->Sunid_Family);
        ajStrApp(&hitsname, temp);
        ajStrApp(&hitsname, hitsextn);

        if(tmphitlist->N == 0)
        {
	    /* create an ouput stream */
	    families=ajFileNewOut(hitsname);

	    if(MAJSTRLEN(tmphitlist->Class))
		ajFmtPrintF(families,"CL   %S\n",tmphitlist->Class);
            
	    if(MAJSTRLEN(tmphitlist->Fold))
		ajFmtPrintSplit(families,tmphitlist->Fold,"XX\nFO   ",75," \t\n\r");
          
	    if(MAJSTRLEN(tmphitlist->Superfamily))
		ajFmtPrintSplit(families,tmphitlist->Superfamily,"XX\nSF   ",75," \t\n\r");
          
	    if(MAJSTRLEN(tmphitlist->Family))
		ajFmtPrintSplit(families,tmphitlist->Family,"XX\nFA   ",75," \t\n\r");
          
	    if(MAJSTRLEN(tmphitlist->Family))
		ajFmtPrintF(families,"XX\nSI   %d\n", tmphitlist->Sunid_Family);
          
	    ajFmtPrintF(families,"XX\nNS   %d\nXX\n",tmphitlist->N);
          
	    ajFileClose(&families);


	    embHitlistDel(&tmphitlist);
	    ajDmxScopalgDel(&scopalg);  
	    ajFileClose(&psif); 

	    ajListDel(&listin);
	    ajListDel(&listout);

	    continue;
        }

        else
        {
	    /* this list will only ever contain a single hitlist */
	    ajListPushApp(listin,tmphitlist);
          
	    /* create a list of scophits to eleminate identical hits */
	    embXyzHitlistToScophits(listin, &listout);
          
	    /* sort this list by accession number, then by start, then by end */
	    ajListSort3(listout,ajDmxScophitCompAcc, ajDmxScophitCompStart, ajDmxScophitCompEnd);
          
	    /* eleminate identical hits */
	    iter=ajListIter(listout); 
          
	    /* get the first node in the list, only once */
	    hit = (AjPScophit)ajListIterNext(iter);
          
	    /* Loop while we can get another hit */
	    while((nexthit=(AjPScophit)ajListIterNext(iter)))
	    {
		/* check if the accession numbers are the same and if the
		   the start and end are identical */
		if(ajStrMatch(hit->Acc,nexthit->Acc) && hit->Start == nexthit->Start && hit->End == nexthit->End)
		{
		    ajDmxScophitTarget(&hit);
		    hit = nexthit;
		}
            
		else
		    hit = nexthit;
	    }             

	    ajListIterFree(iter);
	    iter =  NULL;
     
	    /* The end of the list been reached */
	    /* Delete hits in the list that are targetted for removal */
	    ajListGarbageCollect(listout, ajDmxScophitDelWrap, 
				 (const void *) ajDmxScophitCheckTarget);
          
	    /* recreate the hitlist for printing */
	    embXyzScophitsToHitlist(listout,&hitlist,&iter);
	    ajListIterFree(iter);
	    iter =  NULL;
	    /* END NEW STUFF */





	    /* Create hits output file name - the name will
	       be the same as the Sunid for the SCOP family */
	    /*
	       ajStrAssS(&hitsname, hits);
	       ajStrFromInt(&temp, scopalg->Sunid_Family);
	       ajStrApp(&hitsname, temp);
	       ajStrApp(&hitsname, hitsextn);
	       */


	    if(!(families=ajFileNewOut(hitsname)))
	    {
		ajFmtPrintS(&msg, "Could not open for writing %S", 
			    hitsname);
		ajWarn(ajStrStr(msg));
		ajFmtPrintF(logf, "WARN  Could not open for writing %S\n", 
			    hitsname);
		embHitlistDel(&hitlist);
		ajDmxScopalgDel(&scopalg);  
		ajFileClose(&psif); 

		/* free up listin */
		iter=ajListIter(listin); 
		while((tmphitlist=(AjPHitlist)ajListIterNext(iter)))
		    embHitlistDel(&tmphitlist);
		ajListDel(&listin);
		ajListIterFree(iter);
		
		/* free up listout */
		iter=ajListIter(listout); 
		while((hit=(AjPScophit)ajListIterNext(iter)))
		    ajDmxScophitDel(&hit);
		ajListDel(&listout);
		ajListIterFree(iter);

		continue;       
	    }       
        
	    /* Write the Hitlist object to file */         
	    embHitlistWrite(families, hitlist);
        
	    /* Close families file */
	    ajFileClose(&families);

	    /* free up listin */
	    iter=ajListIter(listin); 
	    while((tmphitlist=(AjPHitlist)ajListIterNext(iter)))
		embHitlistDel(&tmphitlist);
	    ajListDel(&listin);
	    ajListIterFree(iter);
          
	    /* free up listout */
	    iter=ajListIter(listout); 
	    while((hit=(AjPScophit)ajListIterNext(iter)))
		ajDmxScophitDel(&hit);
	    ajListDel(&listout);
	    ajListIterFree(iter);

	    /* Free memory etc*/
	    embHitlistDel(&hitlist);
	    ajDmxScopalgDel(&scopalg);
	    ajFileClose(&psif);     
	    ajStrDel(&alignname);
	}
    }

    
    /*Tidy up and return */
    ajStrDel(&align);
    ajStrDel(&alignextn);
    ajStrDel(&hits);
    ajStrDel(&hitsextn);
    ajStrDel(&hitsname);
    ajStrDel(&psiname);
    ajStrDel(&database);
    ajStrDel(&msg);
    ajStrDel(&temp);
    ajStrDel(&singlet);
    
    ajFileClose(&logf);
    
    ajListDel(&list);

    while(ajListPop(scoplist,(void **)&scoptmp))
	ajScopDel(&scoptmp);
    ajListDel(&scoplist);
    
    ajExit();
    return 0;
}

/* @funcstatic seqsearch_ajDmxScopalgPsiblast *******************************
 **
 ** Reads a Scopalg object and the corresponding alignment file. Calls psiblast
 ** to do a search for the SCOP family over a specified database. The psiblast 
 ** output file is created and a pointer to it provided.
 **
 ** @param [r] scopalg    [AjPScopalg]  Alignment    
 ** @param [r] alignf     [AjPFile]     Alignment file
 ** @param [r] psiname    [AjPStr *]    Name of psiblast output file created
 ** @param [r] niter      [ajint]       No. psiblast iterations
 ** @param [r] maxhits    [ajint]       Maximum number of hits to generate
 ** @param [r] evalue     [float]       Threshold E-value for psiblast
 ** @param [r] database   [AjPStr]      Database name
 **
 ** @return [AjPFile] Pointer to  psiblast output file for reading or NULL for error.
 ** @@
 ** 
 ** Note
 ** When the library code function ScopalgWrite is written, we will no longer
 ** need to pass in a pointer to the alignment file itself. Write ScopalgWrite
 ** and modify this function accordingly - not urgent.
 ******************************************************************************/
static AjPFile seqsearch_ajDmxScopalgPsiblast(AjPScopalg scopalg, AjPFile alignf, 
                                              AjPStr *psiname, ajint niter, ajint maxhits, 
                                              float evalue,  AjPStr database)
{
    AjPStr    line      = NULL;         /* Temp string for reading alignment file */
    AjPStr    name      = NULL;         /* Base name of STAMP temp files */
    AjPStr    temp      = NULL;         /* Temp. string */
    AjPStr    seqin     = NULL;         /* Name of temp. file for PSIBLAST input file 
                                           (single sequence in FASTA format from alignment */
    AjPStr    seqsin    = NULL;         /* Name of temp. file for PSIBLAST input file 
                                           (sequences alignment w/o scop records, 
                                           'Post_similar' or 'Number' lines*/
    AjPStr   *seqs      = NULL;         /* Sequences from alignment */    

    AjPFile   seqsinf   = NULL;         /* File pointer for  PSIBLAST input file 
                                           (multiple sequences)*/
    AjPFile   seqinf    = NULL;         /* File pointer for  PSIBLAST input file 
                                           (single sequence)*/
    AjPFile   psif      = NULL;         /* Pointer to psiblast output file*/
    
    ajint     nseqs     = 0;            /* No. of sequences in alignment */
    ajint     x         = 0;            /* Loop counter */
    

    

    
    /* Rewind alignment file */
    ajFileSeek(alignf,0,SEEK_SET);



    /* Allocate strings */
    line      = ajStrNew();
    name      = ajStrNew();
    temp      = ajStrNew();
    seqin     = ajStrNew();
    seqsin    = ajStrNew();
    


    /* Read scopalg structure and extract sequences only */
    if(!(nseqs=ajDmxScopalgGetseqs(scopalg, &seqs)))
        ajFatal("ajDmxScopalgGetseqs returned 0 sequences in "
                "seqsearch_ajDmxScopalgPsiblast. Email jison@hgmp.mrc.ac.uk\n");


    /* Initialise random number generator for naming of temp. files
       and create  psiblast input files */
    ajRandomSeed();
    ajStrAssC(&name, ajFileTempName(NULL));
    ajStrAss(&seqsin, name);
    ajStrAppC(&seqsin, ".seqsin");
    ajStrAss(&seqin, name);
    ajStrAppC(&seqin, ".seqin");
    ajStrAss(psiname, name);
    ajStrAppC(psiname, ".psiout");


    seqsinf = ajFileNewOut(seqsin);
    seqinf = ajFileNewOut(seqin);
    

    /* Read alignment file and write psiblast input file
       of multiple sequences */
    while(ajFileReadLine(alignf,&line))
    {
        /* Ignore 'Number', 'Post_similar' and Scop classification lines */
        if((ajStrPrefixC(line,"Number")))
            continue;
        else if (ajStrPrefixC(line,"Post_similar"))
            continue;
        else if(ajStrPrefixC(line,"CL"))
        {
            /*Print blank line at top of output file*/
            ajFmtPrintF(seqsinf,"\n");  
            continue;
        }
        else if(ajStrPrefixC(line,"FO"))
            continue;
        else if(ajStrPrefixC(line,"SF"))
            continue;
        else if(ajStrPrefixC(line,"FA"))
            continue;
        else if(ajStrPrefixC(line,"XX"))
            continue;
        else if(ajStrPrefixC(line,"SI"))
            continue;
        else if(ajStrChar(line,1)=='\0')
        { 
            /* If we are on a blank line
               Print blank line at top of output file*/
            ajFmtPrintF(seqsinf,"\n");  
            continue;
        }
        else
            ajFmtPrintF(seqsinf,"%S\n",line);
    }
    
    

    /* Write psiblast input file of single sequence */    
    ajFmtPrintF(seqinf,">\n%S\n",seqs[0]);
    

    /* Close psiblast input files before psiblast opens them */
    ajFileClose(&seqinf);
    ajFileClose(&seqsinf);

    
    /* Run PSI-BLAST */
    ajFmtPrintS(&temp,"blastpgp -i %S -B %S -j %d -e %f -b %d -v %d -d %S > %S\n",
                seqin, seqsin, niter,evalue, maxhits, maxhits, database, *psiname);
    ajFmtPrint("%S\n", temp);
    system(ajStrStr(temp));
    
    

    /* Remove temp. files */
    ajFmtPrintS(&temp, "rm %S", seqin);
    system(ajStrStr(temp));
    ajFmtPrintS(&temp, "rm %S", seqsin);
    system(ajStrStr(temp));


    /* Tidy up */
    ajStrDel(&line);
    ajStrDel(&name);
    ajStrDel(&temp);
    ajStrDel(&seqin);   
    ajStrDel(&seqsin);
    for(x=0;x<nseqs;x++)
        ajStrDel(&seqs[x]);     
    AJFREE(seqs);
        

    /* Open psiblast output file and return pointer */
    psif = ajFileNewIn(*psiname);
    return psif;
}

/* @funcstatic seqsearch_ajDmxScopPsiblast *******************************
 **
 ** Calls psiblast to do a search for the SCOP family over a specified 
 ** database. The psiblast output file is created and a pointer to it provided.
 **
 ** @param [r] singlet    [AjPStr]      Sequence from scop
 ** @param [r] psiname    [AjPStr *]    Name of psiblast output file created
 ** @param [r] niter      [ajint]       No. psiblast iterations
 ** @param [r] maxhits    [ajint]       Maximum number of hits to generate
 ** @param [r] evalue     [float]       Threshold E-value for psiblast
 ** @param [r] database   [AjPStr]      Database name
 **
 ** @return [AjPFile] Pointer to  psiblast output file for reading or NULL for error.
 ** @@
 ** 
 ** Note
 ** When the library code function ScopalgWrite is written, we will no longer
 ** need to pass in a pointer to the alignment file itself. Write ScopalgWrite
 ** and modify this function accordingly - not urgent.
 ******************************************************************************/
static AjPFile seqsearch_ajDmxScopPsiblast(AjPStr singlet, AjPStr *psiname, 
                                           ajint niter, ajint maxhits, 
                                           float evalue,  AjPStr database)
{
    AjPStr    name      = NULL;         /* Base name of STAMP temp files */
    AjPStr    temp      = NULL;         /* Temp. string */
    AjPStr    seqin     = NULL;         /* Name of temp. file for PSIBLAST input file 
                                           (single sequence in FASTA format from alignment */

    AjPFile   seqinf    = NULL;         /* File pointer for  PSIBLAST input file (single sequence)*/
    AjPFile   psif      = NULL;         /* Pointer to psiblast output file*/

    /* Allocate strings */
    name      = ajStrNew();
    temp      = ajStrNew();
    seqin     = ajStrNew();

    /* Initialise random number generator for naming of temp. files
       and create  psiblast input files */
    ajStrAssC(&name, ajFileTempName(NULL));
    ajStrAss(&seqin, name);
    ajStrAppC(&seqin, ".seqin");
    ajStrAss(psiname, name);
    ajStrAppC(psiname, ".psiout");

    /* create output file for psi-blast input file */
    seqinf = ajFileNewOut(seqin);
    
    /* Write psiblast input file of single sequence */    
    ajFmtPrintF(seqinf,">\n%S\n",singlet);
    
    /* Close psiblast input files before psiblast opens them */
    ajFileClose(&seqinf);

    /* Run PSI-BLAST */
    ajFmtPrintS(&temp,"blastpgp -i %S -j %d -e %f -b %d -v %d -d %S > %S\n",
                seqin, niter,evalue, maxhits, maxhits, database, *psiname);
    ajFmtPrint("%S\n", temp);
    system(ajStrStr(temp));

    /* Remove temp. files */
    ajFmtPrintS(&temp, "rm %S", seqin);
    system(ajStrStr(temp));

    /* Tidy up */
    ajStrDel(&name);
    ajStrDel(&temp);
    ajStrDel(&seqin);   

    /* Open psiblast output file and return pointer */
    psif = ajFileNewIn(*psiname);
    return psif;
}


/* @funcstatic seqsearch_ajDmxHitlistPsiblast ********************************
 **
 ** Reads a psiblast output file and writes a Hitlist object containing the 
 ** hits.
 **
 ** @param [r] scopalg   [AjPScopalg]  Alignment    
 ** @param [r] psif      [AjPFile]     psiblast output file 
 **
 ** @return [AjPHitlist] Pointer to Hitlist object (or NULL on failure)
 ** @@
 ** 
 ******************************************************************************/
static AjPHitlist seqsearch_ajDmxHitlistPsiblast(AjPScopalg scopalg, AjPFile psif)
{
    /* The hits are organised into blocks, each block contains hits to 
       a protein with a unique accession number.  
       Each hit in a block mnay be spread over multiple lines. nlines 
       is the number of the line (sequence fragment) we are currently on */
    
    
    AjPStr  line       = NULL;  /* Temp string */ 
    AjPStr  acc        = NULL;  /* Acc. number of current hit*/ 
    AjPStr  prevacc    = NULL;  /* Acc. number of previous hit*/ 
    AjPStr  fragseq    = NULL;  /* Sequence fragment */ 
    AjPStr  fullseq    = NULL;  /* Sequence of entire hit */ 

    
    ajint   start      = 0;     /* Start of hit */
    ajint   fragstart  = 0;     /* Start of sequence fragment */
    ajint   fragend    = 0;     /* End of sequence fragment */
    ajint   hitn       = 0;     /* The number of the hit we are on */
    ajint   nhits      = 0;     /* No. of hits in alignment file */
    ajint   fseekr     = 0;

    ajlong  offset     = 0;
    
    AjPHitlist hitlist = NULL;  /* Hitlist object for holding results 
                                   of PSIBLAST hits*/
    
    /* Allocate strings etc */
    line      = ajStrNew();
    acc       = ajStrNew();
    prevacc   = ajStrNew();
    fragseq   = ajStrNew();
    fullseq   = ajStrNew();
    
    /* Calculate the number of hits */
    while(ajFileReadLine(psif,&line))
        if(ajStrFindCaseC(line,"score = ")>=0)
            nhits++;
    fseekr = ajFileSeek(psif,offset,SEEK_SET);
    
    
    /* Allocate memory for Hitlist object */
    if(!(hitlist = embHitlistNew(nhits)))
        return(hitlist);
    
    
    /* Copy SCOP classification records*/
    /* Assign scop classification records from hitlist structure */
    ajStrAss(&hitlist->Class, scopalg->Class);
    ajStrAss(&hitlist->Fold, scopalg->Fold);
    ajStrAss(&hitlist->Superfamily, scopalg->Superfamily);
    ajStrAss(&hitlist->Family, scopalg->Family);
    hitlist->Sunid_Family = scopalg->Sunid_Family;
    
    /* Loop for whole of input file*/
    while(ajFileReadLine(psif,&line))
    {
        /* We've found a line beginning with > i.e. the start 
           of a block of hits to a single protein*/
        if(ajStrPrefixC(line,">SW:"))
        {
            /* Parse the accession number */
            ajFmtScanS(line, "%*s %S", &prevacc);
        }
        /* We've found a line beginning with ' Score = ' i.e. the
           start of data for a hit */
        else if(ajStrPrefixC(line," Score = "))
        {
            /* Write hit structure, we've parsed  */
            if(hitn!=0)
            {
                hitlist->hits[hitn-1]->Start = start;
                hitlist->hits[hitn-1]->End = fragend;
                ajStrAss(&hitlist->hits[hitn-1]->Acc, acc);
                ajStrAss(&hitlist->hits[hitn-1]->Seq, fullseq);
                ajStrDegap(&hitlist->hits[hitn-1]->Seq);
                ajStrAssC(&hitlist->hits[hitn-1]->Typeobj, "HIT");
                
                /*
                   for(x=0;x<scopalg->N; x++)
                   if((ajStrFindCase(seqs[x], hitlist->hits[hitn-1]->Seq))>=0)
                   {
                   ajStrAssC(&hitlist->hits[hitn-1]->Typeobj, "SEED");
                   break;
                   } 
                   */
            }
            
            /* Reset the sequence of the full hit */
            ajStrAssC(&fullseq, "");

            /* Incremenet hit counter */
            hitn++;

            /* Copy accession number */
            ajStrAss(&acc, prevacc);

        }
        /* Line containing sequence segment of the hit */
        else if(ajStrPrefixC(line,"Sbjct: "))
        {
            /* Parse the start, end and sequence of the fragment */
            ajFmtScanS(line, "%*s %d %S %d", &fragstart, &fragseq, &fragend);

            /* If this is the first fragment, get the start point */
            if(!ajStrCmpC(fullseq, ""))
                start=fragstart;
   
            /* Add fragment to end of sequence of full hit */
            ajStrApp(&fullseq, fragseq);
        }
    }

    /* Write hit structure for last hit */
    if(hitn!=0)
    {
        hitlist->hits[hitn-1]->Start = start;
        hitlist->hits[hitn-1]->End = fragend;
        ajStrAss(&hitlist->hits[hitn-1]->Acc, acc);
        ajStrAss(&hitlist->hits[hitn-1]->Seq, fullseq);
        ajStrDegap(&hitlist->hits[hitn-1]->Seq);
        ajStrAssC(&hitlist->hits[hitn-1]->Typeobj, "HIT");

    }

    /*Tidy up and return */
    ajStrDel(&line);
    ajStrDel(&acc);
    ajStrDel(&prevacc);
    ajStrDel(&fragseq);
    ajStrDel(&fullseq);

    return hitlist;
}






