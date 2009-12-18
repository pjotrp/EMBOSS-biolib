/* @source sites application
**
** Reads CCF files (clean coordinate files) and writes CON files (contact 
** files) of residue-ligand contact data for domains in a DCF file (domain
** classification file).
** 
** @author: Copyright (C) Waqas Awan
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
********************************************************************************
** 
**  SITES documentation
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
**  
**  
****************************************************************************/





#include "emboss.h"

#define ajWAPROTEIN  1
#define ajWADOMAIN   2



/******************************************************************************
**
** STRUCTURE DEFINITIONS
**
******************************************************************************/

/* @data AjPDomConts **********************************************************
**
** Ajax DomConts object.
**
** Holds the domain contact data
**
** AjPDomConts is implemented as a pointer to a C data structure.
**
** @alias AjSDomConts
** @alias AjODomConts
**
** @attr type [ajint]  Type of entry: ajWAPROTEIN or ajWADOMAIN.
** @attr het_name [AjPStr] 3-character code of heterogen. 
** @attr pdb_name  [AjPStr] Name of PDB file.
** @attr Nres [ajint] No. residues in chain. 
** @attr Seq [AjPStr] Polypeptide sequence. 
** @attr scop_name [AjPStr] 7-character scop id domain name. 
** @attr no_keyres [ajint] number of key binding residues.
** @attr *aa_code [AjPStr*] Array for 3-character amino acid codes. 
** @attr res_pos [AjPInt] Array of ints for residue positions in domain 
**                 clean coordinate file. 
** @attr *res_pos2 [AjPStr*] Array of residue positions in PDB file
**      	    - exist as strings.  No longer used.
** @attr chainid  [char] Chain identifier. 
** @@
******************************************************************************/

typedef struct AjSDomConts
{
  ajint  type;
  AjPStr het_name;
  AjPStr pdb_name;
  ajint  Nres;
  AjPStr Seq;
  AjPStr scop_name;
  ajint  no_keyres;
  AjPStr *aa_code;
  AjPInt res_pos;
  AjPStr *res_pos2;
  char   chainid;
}AjODomConts, *AjPDomConts;



/* @data AjPDbaseEnt **********************************************************
**
** Ajax DbaseEnt object.
**
** Holds the data required for the database of functional sites
**
** AjPDbaseDat is implemented as a pointer to a C data structure.
**
** @alias AjSDbaseEnt
** @alias AjODbaseEnt
**
** @attr abv [AjPStr] 3-letter abbreviation of heterogen
** @attr ful [AjPStr] Full name
** @attr no_sites [ajint] number of binding sites
** @attr cont_data [AjPDomConts] array of domain contact data
**                               (derived from tmp)
** @attr tmp [AjPList]  Temp. list of domain contact data
** @@
******************************************************************************/

typedef struct AjSDbaseEnt
{
  AjPStr      abv;
  AjPStr      ful;
  ajint       no_sites; 
  AjPDomConts *cont_data; 
  AjPList     tmp;        
} AjODbaseEnt, *AjPDbaseEnt;



/* @data AjPDbase ***********************************************************
**
** Ajax Dbase object.
**
** Holds a Database of functional residues.
**
** AjPDbase is implemented as a pointer to a C data structure.
**
** @alias AjSDbase
** @alias AjODbase  
**
** @attr n [ajint] Number of entries
** @attr entries [AjPDbaseEnt *] Array of entries
** @@
******************************************************************************/

typedef struct AjSDbase
{
  ajint         n;        
  AjPDbaseEnt *entries; 
} AjODbase, *AjPDbase;





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static   AjPDbase    sites_DbaseNew(ajint n);
static   void        sites_DbaseDel(AjPDbase *ptr);

static   AjPDbaseEnt sites_DbaseEntNew(ajint n);
static   void        sites_DbaseEntDel(AjPDbaseEnt *ptr);

static   AjPDomConts sites_DomContsNew(ajint n);
static   void        sites_DomContsDel(AjPDomConts *ptr);

static   AjBool      sites_HetTest(AjPPdb ptr);

static   AjBool      sites_DichetToDbase(AjPDbase *dbase, AjPHet hetDic);

static   AjBool      sites_HeterogenContacts(ajint entype, 
					     float dist_thresh, 
					     AjPList list_domains,
					     AjPChar arr_chains,
					     AjPInt arr_nres,
					     AjPList list_seqs,		
					     AjPList list_pdbscopids, 
					     AjPList list_heterogens, 
					     AjPInt siz_domains, 
					     AjPInt siz_heterogens, 
					     AjPVdwall vdw, AjPDbase *dbase);

static   AjBool      sites_HeterogenContactsWrite(AjPFile outf, 
						  AjPDbase dbase, 
						  float thresh); 

static AjBool    sites_HeterogenContactsWriteOld(AjPFile funky_out, 
						 AjPDbase dbase);

void sites_Dummy (void);


/* @prog sites *************************************************************
**
** Reads CCF files (clean coordinate files) and writes CON files (contact 
** files) of residue-ligand contact data for domains in a DCF file (domain 
** classification file).
**
***************************************************************************/
int main(ajint argc, char **argv)
{
    /* ACD handling */
    AjPList   prot=NULL;	    /* Location of protein coordinate files. */
    AjPDir  dom=NULL;		    /* Location of domain coordinate files.  */
    AjPFile  dcf_fptr=NULL;	    /* Pointer to scop domain index file.    */
    AjPFile  het_fptr=NULL;         /* Pointer to dictionary of heterogens 
				       file (output from Dichet.c)           */
    AjPFile  outf=NULL;		    /* Pointer to output file                */
    AjPFile  vdwf=NULL;		    /* Pointer to van der Waals file         */
    float    dist_thresh;	    /* Threshold contact distance            */
    
    /* Data files */
    AjPList    list_allscop=NULL;   /* List for SCOP classification.         */
    AjPScop    scoptemp=NULL;       /* Temp. pointer for ajListPop function */
    AjPHet     hetDic=NULL;	    /* Dichet object to hold entry from 
				       heterogen dictionary */
    AjPVdwall  vdw=NULL;            /* Vdwall object to hold van der Waals 
				       radii data */
    
    /* Creating Dbase object */
    AjPDbase   dbase=NULL;	    /* Database of functional residues for whole build */
    ajint      i=0;		    /* Counter */
    
    
    /* Main application loop */
    AjPStr   cp_file=NULL;          /* Current protein coordinate file */
    AjPFile  prot_fptr=NULL;	    /* Protein coordinate file pointer */
    AjPStr   msg=NULL;              /* String for messaging */
    AjPPdb   pdb=NULL;		    /* Pdb object for protein */
    ajint    f_ctr=0;               /* Protein coordinate file counter */
    ajint    f_num=0;               /* Total number of protein coordinate files */
    
    
    /* List of heterogen atom arrays */
    AjPInt   siz_heterogens=NULL;       /* Integer array with numbers of 
					   elements in list_heterogens */
    AjPList  list_heterogens=NULL;      /* List of heterogen Atom-arrays 
					   (each for each heterogen in
					   current Pdb object */
    ajint    n_heterogens=0;            /* no. of heterogens/arrays in 
					   current Pdb object */
    
    /* List of scop identifiers for domains in pdb object */
    AjPList  list_pdbscopids=NULL;      /* List to hold SCOP domain identifiers 
					   for domains in pdb object */
    AjPStr   scopid_tmp=NULL;           /* Temprorary pointer for ajListPop to 
					   free memory for list_pdbscopid */
    
    /* List of domain coordinate filenames */
    AjIList  iter_scopids=NULL;         /* Iterator for list_pdbscopids */
    AjPStr   pdbscopid=NULL;            /* Pointer to current domain scop id */
    AjPStr   dom_co_fname=NULL;         /* Domain coordinate filename */
    AjPList  list_domfnames=NULL;       /* List of domain coordinate filenames 
					   for domains in pdb object */
    AjPStr   domfname_tmp=NULL;         /* Temprorary pointer for ajListPop 
					   to free memory for list_domfnames */
    
    /* Domain coordinate file and list of domain pdb objects */
    AjIList  iter_domfnames=NULL;        /* Iterator for list of domain 
					    coordinate filenames */
    AjPStr   pdbscop_fname=NULL;         /* Pointer to current domain 
					    coordinate filename */
    ajint    ndomain=0;                  /* Number of the current domain being
					    processed */
    AjPFile  dom_fptr=NULL;              /* Domain coordinate file pointer */
    AjPPdb   pdbdom=NULL;                /* Pdb object for domain */
    AjPList  pdbdomList=NULL;            /* List of domain pdb objects for 
					    domains in current protein 
					    coordinate file */
    
    /* List of domain atom arrays in current pdb object */
    ajint    siz_atomarr=0;	  /* Number of elements in current array of 
				     domain atoms */
    AjPAtom  *atomarr=NULL;       /* Array of domain atoms */
    AjPList  list_domains=NULL;   /* List of domain Atom arrays from pdb 
				     object */
    AjPInt   siz_domains=NULL;    /* Integer array with array sizes for 
				     list_domains */
    AjIList  iter_pdbdom=NULL;    /* Iterator for pdbdomList */
    AjPPdb   pdbdom_ptr=NULL;     /* Pointer to current domain pdb object 
				     in pdbdomList - also used for freeing memory. */
    
    /* Contacts between domain atoms and heterogen atoms */
    ajint    l=0;                 /* Lengths of atom arrays */
    
    /* Freeing memory for the list of atom arrays */
    AjIList  iter_loa=NULL;       /* Iterator for list of atom arrays*/
    AjPAtom  *l_atm=NULL;	  /* Current array of atom objects */
    
    /* Log file */
    AjPFile logf=NULL;
    

    AjPChar arr_chains  = NULL;   /* Array of chain identifiers */
    AjPInt  arr_nres    = NULL;   /* Array of numbers of residues */
    ajint   cnt_chains  = 0;
    AjPList list_seqs   = NULL;   /* List of sequences  */
    AjPStr  tmpseq      = NULL;   /* Temp. Seq for creating list_seqs */
    
    AjPStr pdbstr = NULL; 		/* for pushing on to the list */

    
    /* READ ACD FILE */
    embInitPV("sites",argc,argv,"STRUCTURE",VERSION);

    prot        = ajAcdGetDirlist("protpath");
    dom         = ajAcdGetDirectory("domaindir");
    dcf_fptr    = ajAcdGetInfile("dcffile");
    het_fptr    = ajAcdGetDatafile("dicfile");
    vdwf        = ajAcdGetDatafile("vdwfile");
    dist_thresh = ajAcdGetFloat("threshold");
    outf        = ajAcdGetOutfile("outfile");
    logf        = ajAcdGetOutfile("logfile");

    
   
    /* MEMORY ALLOCATION, READ DATA FILES. */
    msg      = ajStrNew();
    
   
    

    /* Read domain classification file & store it as list of SCOP objects */
    if(!(list_allscop = ajScopReadAllNew(dcf_fptr)))
    { 
	ajListFree(&prot);
	ajDirDel(&dom);
	ajStrDel(&msg); 
	ajFileClose(&dcf_fptr);
	ajFileClose(&het_fptr);
	ajFileClose(&outf);    
	ajFileClose(&vdwf);    
	ajFatal("Error reading SCOP classification file\n"); 
    }
    
    /* Read van der Waals data file (Evdw.dat) and create vdw object*/
    if(!(vdw=ajVdwallReadNew(vdwf)))
    { 
	ajListFree(&prot);
	ajDirDel(&dom);
	ajStrDel(&msg); 
	ajFileClose(&dcf_fptr);
	ajFileClose(&het_fptr);
	ajFileClose(&outf);    
	ajFileClose(&vdwf); 
	while(ajListPop(list_allscop,(void **)&scoptemp)) 
	    ajScopDel(&scoptemp); 
	ajListFree(&list_allscop);
	ajFatal("Error reading vdw radii file\n"); 
    }
    
    /* Read heterogen_dictionary.out and create Dichet object */
    if((!(hetDic = ajHetReadNew(het_fptr))))     
    {
	ajListFree(&prot);
	ajDirDel(&dom);
	ajStrDel(&msg); 
	ajFileClose(&dcf_fptr);
	ajFileClose(&het_fptr);
	ajFileClose(&outf);    
	ajFileClose(&vdwf);
	while(ajListPop(list_allscop,(void **)&scoptemp)) 
	    ajScopDel(&scoptemp); 
	ajListFree(&list_allscop);
	ajFatal("Error reading HET dictionary file\n"); 
    }
    
    ajFmtPrint("Entries in HetDic %d\n", hetDic->n);
    
    /* CREATE DBASE OBJECT */
    dbase=sites_DbaseNew(hetDic->n);
    for(i=0; i<hetDic->n; i++) 
	dbase->entries[i] = sites_DbaseEntNew(0); 
    
    /* WRITE HETEROGEN INFORMATION FROM DICHET OBJECT (ABBREVIATIONS 
       AND FULL NAMES) TO DBASE OBJECT */
    sites_DichetToDbase(&dbase, hetDic);
    
    ajFmtPrint("Entries in Dbase %d\n", dbase->n);
    
    /* CREATE LIST OF PROTEIN COORDINATE FILES IN PROTEIN COORDINATE 
       DIRECTORY */
    f_num=ajListGetLength(prot);
    




    /* START OF MAIN APPLICATION LOOP - PER PROTEIN COORDINATE FILE */
    while(ajListstrPop(prot, &cp_file))
    {
	ajFmtPrint("CCF FILE: %S (%d/%d)\n", cp_file, ++f_ctr, f_num);
	fflush(stdout);
	ajFmtPrintF(logf, "CCF: %S", cp_file);
	/*Reset counter for number of domains in this file*/
	ndomain=0;			
      
	/* OPEN FILE AND WRITE PDB OBJECT */
	/* Open file */
	if((prot_fptr=ajFileNewInNameS(cp_file))==NULL) 
	{
	    ajFmtPrintS(&msg, "Could not open for reading %S", cp_file );
	    ajWarn(ajStrGetPtr(msg));
	    ajStrDel(&cp_file);	
	    continue;	    
	}

	/* Write pdb object */
	if(!(pdb=ajPdbReadFirstModelNew(prot_fptr)))
	{
	    /* No need to free memory for pdb if CpdbRead fails */
	    ajFmtPrintS(&msg, "ERROR file read error %S", cp_file);
	    ajWarn(ajStrGetPtr(msg));
	    ajStrDel(&cp_file);	
	    ajFileClose(&prot_fptr);	     
	    continue;
	}
      
	/* Close protein coordinate file */
	ajFileClose(&prot_fptr);
    

	
  
	/* Check for heterogens in pdb object, if no heterogens skip to 
	   next file */
	if(!sites_HetTest(pdb))
	{
	    ajStrDel(&cp_file);	
	    ajFmtPrintF(logf, " HETS:NO\n");
	    fflush(stdout);
	    /* ajFmtPrint(" NO HETS - SKIPPING FILE...\n"); */
	    ajPdbDel(&pdb);
	    pdb=NULL;
	    continue;
	}

	/* Free string */
	ajStrDel(&cp_file);

	/* CREATE LIST OF ATOM ARRAYS FOR HETEROGENS IN CURRENT PDB OBJECT */
	list_heterogens=ajListNew();
	siz_heterogens=ajIntNew();
	n_heterogens=0;
	embPdbListHeterogens(pdb, &list_heterogens, &siz_heterogens, 
			     &n_heterogens, logf);	
      
	l=ajListGetLength(list_heterogens);
	ajFmtPrintF(logf, " HETS:YES", l);
	ajFmtPrintF(logf, " NHETS:%d", l);
	fflush(stdout);
      
	/* CREATE LIST OF SCOP IDENTIFIERS FOR SCOP DOMAINS OCCURRING 
	   IN CURRENT PDB OBJECT */
	list_pdbscopids=ajListstrNew();

	/* OPEN DOMAIN COORDINATE FILE AND WRITE LIST OF DOMAIN PDB OBJECTS */
	list_domains=ajListNew();
	arr_chains = ajChararrNew();
	arr_nres = ajIntNew();
	list_seqs=ajListNew(); 

	siz_domains=ajIntNew();
	pdbdomList=ajListNew();

	ajFmtPrintF(logf, " SCOP:");
      
	/* Check for domains in the pdb object, if no domains skip to 
	   next file */
	if(!embPdbidToScop(pdb, list_allscop, &list_pdbscopids))
	{
	    ajFmtPrintF(logf, "NO");
	    fflush(stdout);
	  
	    /*create a list of atom arrays*/
	    for(cnt_chains=0, i=0; i<pdb->Nchn; i++)
	    {
		/*pdb file is already open and the object exists*/
		pdbstr = NULL;
		ajStrAssignS(&pdbstr, pdb->Pdb);
		ajListPushAppend(list_pdbscopids, pdbstr);
		ajChararrPut(&arr_chains, cnt_chains, pdb->Chains[i]->Id);
		ajIntPut(&arr_nres, cnt_chains, pdb->Chains[i]->Nres);
		++cnt_chains;
		tmpseq = ajStrNew();
		ajStrAssignS(&tmpseq, pdb->Chains[i]->Seq);
		ajListPushAppend(list_seqs,   tmpseq); 

		siz_atomarr=ajListToarray(pdb->Chains[i]->Atoms, 
					  (void ***) &atomarr); 
		ajListPushAppend(list_domains, (AjPAtom *) atomarr);
		atomarr=NULL;
		ajIntPut(&siz_domains, ndomain, siz_atomarr);
		ndomain++;	
		fflush(stdout);
	    }
	    ajFmtPrintF(logf, " NCHN:%d\n", pdb->Nchn);
	    /* CALCULATE CONTACTS BETWEEN ALL COMBINATIONS OF CHAINS AND 
	       HETEROGENS IN CURRENT PDB OBJECT */
	    sites_HeterogenContacts(ajWAPROTEIN, 
				    dist_thresh, 
				    list_domains, 
				    arr_chains, 
				    arr_nres, 
				    list_seqs,
				    list_pdbscopids,
				    list_heterogens, 
				    siz_domains, 
				    siz_heterogens, 
				    vdw, 
				    &dbase);  
	}
	else
	{
	    ajFmtPrintF(logf, "YES");
	    /* CREATE LIST OF SCOP DOMAIN FILENAMES */
	    list_domfnames=ajListstrNew();
	  
	    /* Initialise iterator for list of scop ids */
	    iter_scopids=ajListIterNew(list_pdbscopids);	
	    while((pdbscopid=ajListIterGet(iter_scopids))) 
	    {	  
		dom_co_fname=ajStrNew();
		ajStrAssignS(&dom_co_fname, ajDirGetPath(dom));
		ajStrAppendS(&dom_co_fname, pdbscopid);
		ajStrAppendC(&dom_co_fname, ".");
		ajStrAppendS(&dom_co_fname, ajDirGetExt(dom));
		ajListstrPushAppend(list_domfnames, dom_co_fname);
	    }
	    ajListIterDel(&iter_scopids);
	  
	  
	    /* Initialise iterator */
	    iter_domfnames=ajListIterNew(list_domfnames);

	    /* START OF DOMAIN LOOP - PER DOMAIN IN list_scopids */
	    while((pdbscop_fname=ajListIterGet(iter_domfnames))) 
	    {
		if((dom_fptr=ajFileNewInNameS(pdbscop_fname))==NULL)
		{
		    ajFmtPrintS(&msg, "Could not open for reading: %S", 
				pdbscop_fname);
		    ajWarn(ajStrGetPtr(msg));
		    continue;
		}
		/* Write pdb object */
		if(!(pdbdom = ajPdbReadFirstModelNew(dom_fptr)))
		{
		    ajFmtPrintS(&msg, "ERROR file read error - ajPdbReadFirstModelNew");
		    ajWarn(ajStrGetPtr(msg));
		    ajFileClose(&dom_fptr);
		    continue;	
		}
	      
		/* Create list of domain pdb objects */
		ajListPushAppend(pdbdomList, pdbdom);
		/* Close domain coordinate file */
		ajFileClose(&dom_fptr);
	      
	    } /* END OF DOMAIN LOOP */
	  
	    /* CREATE LIST OF DOMAIN ATOM ARRAYS FOR DOMAINS IN CURRENT 
	       PROTEIN COORDINATE FILE */
	    iter_pdbdom=ajListIterNew(pdbdomList);
	    cnt_chains=0;
	    while((pdbdom_ptr=ajListIterGet(iter_pdbdom)))
	    {
		/* note: only one chain in domain coordinate file and 
		   so Chains[0] */
		siz_atomarr=ajListToarray(pdbdom_ptr->Chains[0]->Atoms, 
					  (void ***) &atomarr); 
		ajListPushAppend(list_domains, (AjPAtom *) atomarr);
		
		
		ajChararrPut(&arr_chains, cnt_chains, 
			     pdbdom_ptr->Chains[0]->Id);
		ajIntPut(&arr_nres, cnt_chains, pdbdom_ptr->Chains[0]->Nres);
		++cnt_chains;
		
		tmpseq = ajStrNew();
		ajStrAssignS(&tmpseq, pdbdom_ptr->Chains[0]->Seq);
		ajListPushAppend(list_seqs,   tmpseq);  

		atomarr=NULL;
		ajIntPut(&siz_domains, ndomain, siz_atomarr);
		ndomain++;	
	    }


	    ajListIterDel(&iter_pdbdom);
	    pdbdom_ptr=NULL;
	    l=ajListGetLength(list_domains);
	    ajFmtPrintF(logf, " NDOMS: %d\n", l);
	    fflush(stdout);
	  
	    /* CALCULATE CONTACTS BETWEEN ALL COMBINATIONS OF DOMAINS AND 
	       HETEROGENS IN CURRENT PDB OBJECT */
	    sites_HeterogenContacts(ajWADOMAIN, 
				    dist_thresh, 
				    list_domains, 
				    arr_chains, 
				    arr_nres,	
				    list_seqs, 
				    list_pdbscopids, 
				    list_heterogens, 
				    siz_domains, 
				    siz_heterogens, 
				    vdw, 
				    &dbase);  

	  
	    /* Free memory for domain pdb objects and pdbdomList */
	    while(ajListPop(pdbdomList, (void **) &pdbdom_ptr))
		ajPdbDel(&pdbdom_ptr);
	    ajListFree(&pdbdomList);
	  
	    /* Free list of domain coordinate filenames */
	    while(ajListstrPop(list_domfnames, &domfname_tmp))
		ajStrDel(&domfname_tmp); 
	    ajListstrFree(&list_domfnames);
	    ajListIterDel(&iter_domfnames);
	}
      
	/* Free siz_domains */
	ajIntDel(&siz_domains);
      
	/* Free siz_heterogens */
	ajIntDel(&siz_heterogens);
      
	/* Free list of domain atom arrays */     
	/* Free memory for the arrays themselves (but not the data that 
	   the array elements point to because this is free'd by the call to
	   ajPdbDel) */
	
	iter_loa=ajListIterNew(list_domains); 
	while((l_atm=(AjPAtom *)ajListIterGet(iter_loa)))
	    AJFREE(l_atm);
	ajListIterDel(&iter_loa);

	ajListFree(&list_domains);
	ajChararrDel(&arr_chains);
	ajIntDel(&arr_nres);
	while(ajListPop(list_seqs, (void *) &tmpseq))
	    ajStrDel(&tmpseq);
	ajListFree(&list_seqs); 

	
	/* Free list of heterogen atom arrays */      
	iter_loa=ajListIterNew(list_heterogens); 
	while((l_atm=(AjPAtom *)ajListIterGet(iter_loa)))
	    AJFREE(l_atm);
	ajListIterDel(&iter_loa);
	
	ajListFree(&list_heterogens);

	/* Free memory for protein pdb object */
	ajPdbDel(&pdb);
	pdb=NULL;

	/* Free list of scop ids in pdb object */
	while(ajListstrPop(list_pdbscopids, &scopid_tmp)) 
	    ajStrDel(&scopid_tmp); 
	ajListstrFree(&list_pdbscopids);
           
    } /*End of loop while(ajListstrPop(prot, &cp_file))*/
    

    /* COMPLETE THE DBASE OBJECT - WRITE CONTACT DATA ARRAY FOR EACH 
       HETEROGEN ENTRY IN THE OBJECT */
    for(i=0; i<(dbase)->n; i++)
    {
	(dbase)->entries[i]->no_sites
	    =ajListToarray((dbase)->entries[i]->tmp, 
			   (void ***) &(dbase)->entries[i]->cont_data);
    }
    
    /* WRITE OUTPUT FILE */
    sites_HeterogenContactsWrite(outf, dbase, dist_thresh);  
    





    /* MEMORY MANAGEMENT */
    ajListFree(&prot);
    ajListFree(&pdbdomList);
    ajDirDel(&dom);
    ajFileClose(&dcf_fptr);
    ajFileClose(&het_fptr);
    ajFileClose(&outf);    
    ajFileClose(&vdwf);    
    ajStrDel(&msg);	
    ajVdwallDel(&vdw);
    while(ajListPop(list_allscop,(void **)&scoptemp)) 
	ajScopDel(&scoptemp); 
    ajListFree(&list_allscop);
    ajHetDel(&hetDic);
    sites_DbaseDel(&dbase);
    ajFileClose(&logf);  

    

    ajExit();
    return 0;
}






/* @funcstatic  sites_DichetToDbase ******************************************
**
** Function to populate the Dbase object (database of functional residues)
** with abbreviations and full names from the heterogen dictionary.
** 
** @param [w] dbase  [AjPDbase*]   Functional residues.
** @param [r] hetDic [AjPHet]      Heterogen dictionary.
** 
** @return [AjBool] True on success
** @@
******************************************************************************/
static AjBool      sites_DichetToDbase(AjPDbase *dbase, AjPHet hetDic)
{
  ajint i=0;

  /* Check args */
  if(!hetDic || !dbase)
    {
      ajWarn("NULL arg passed to sites_DichetToDbase\n");
      return ajFalse;
    }

  if(*dbase==NULL) 
    {
      *dbase=sites_DbaseNew(hetDic->n);
      for(i=0; i< hetDic->n; i++)
	(*dbase)->entries[i] = sites_DbaseEntNew(0);
	
    }
  for(i=0;i<(hetDic)->n;++i)
    {
      ajStrAssignS(&(*dbase)->entries[i]->abv, hetDic->entries[i]->abv);
      ajStrAssignS(&(*dbase)->entries[i]->ful, hetDic->entries[i]->ful);
    }

  return ajTrue;
}





/* @funcstatic sites_HeterogenContacts ****************************************
**
** Function to calculate contacts between domains and ligands
**
** @param [r] entype             [ajint]     Type of contact, either 
**                                           ajWAPROTEIN (1) or ajWADOMAIN (2).
** @param [r] dist_thresh        [float]     The distant cut-off in angstroms 
**                                           that defines a contact.
** @param [r] list_domains       [AjPList]   List of domain Atom arrays.
** @param [r] arr_chains         [AjPChar]   Chains ids.
** @param [r] arr_nres           [AjPInt]    Number of residues in chains / 
**                                           domains. 
** @param [r] list_seqs          [AjPList]   List of sequences. 
** @param [r] list_pdbscopids    [AjPList]   List of scop domain ids in a pdb 
**                                           object.
** @param [r] list_heterogens    [AjPList]   Pointer to list of heterogen
**                                           Atom arrays
** @param [r] siz_domains        [AjPInt]    Size of each array in list of 
**                                           domain atom arrays.
** @param [r] siz_heterogens     [AjPInt]    Size of each array in list of
**                                           heterogen atom arrays.
** @param [r] vdw                [AjPVdwall] van der Waals radii data.
** @param [w] dbase              [AjPDbase*] Pointer to Dbase object.
**
**
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/
static AjBool      sites_HeterogenContacts(ajint entype, 
					   float dist_thresh, 
					   AjPList list_domains, 
					   AjPChar arr_chains,
					   AjPInt arr_nres,
					   AjPList list_seqs,
					   AjPList list_pdbscopids,
					   AjPList list_heterogens, 
					   AjPInt siz_domains, 
					   AjPInt siz_heterogens, 
					   AjPVdwall vdw, 
					   AjPDbase *dbase)
{

  AjIList iter_dom=NULL;      /* Iterator for domain atoms LOA*/
  AjIList iter_het=NULL;      /* Iterator for heterogen atoms LOA */
  AjPAtom *dom_atm=NULL;      /* Current array of atom objects (for domains) */
  AjPAtom *het_atm=NULL;      /* Current array of atom objects (for heterogens) */
  AjPStr  *scopidArr=NULL;    /* Array of scop ids */
  ajint   scopctr=0;          /* Loop counter for array of scop ids */
  ajint   hetctr=0;           /* Loop counter for array of hets*/
  AjPStr  *chainseqsArr=NULL; /* Array of chain sequences */

  
  AjPDomConts cont_dataTemp=NULL; /* Temporary pointer for current 
				     heterogen:domain contact data */
  AjPStr   tempaa;                /* Temp. pointer for amino acid code */
  AjPList aaTempList=NULL;	  /* Temporary list for amino acid code */
  AjPStr  tempres_pos2=NULL;      /* NEW Temporary pointer for the 
				     original pdb residue positions */
  AjPList res_pos2TempList=NULL;  /* NEW Temporary list for residue 
				     positions from original pdb file */
  AjPList cont_dataList=NULL;	  /* Temp. list of all AjPDomConts objects */
  

  AjPDomConts dom_cont=NULL;	  /* Current DomConts object */
  

  ajint   DomIdx=0;	/* For index into siz_domains array */
  ajint   HetIdx=0;	/* For index into siz_heterogens array */
  ajint   dom_max=0;	/* Size of current domain atoms array */
  ajint   het_max=0;	/* Size of current heterogen atoms array */
  ajint   idx_tmp=0;	/* Current residue identifier */
  ajint   i=0;          /* Counter for looping through domain 
			   atom array also used for looping 
			   through (*dbase)->entries */
  ajint   j=0;          /* Counter for looping through heterogen atom array */
    
  ajint res_ctr=0;      /* Counter for looping through aa_code */
  AjPStr temp=NULL;    	/* Temporary string for holding Hetrogen 
			   atom information - diagnostic */


  if((!dist_thresh) || (list_domains==NULL) || (list_pdbscopids==NULL) || 
     (list_heterogens==NULL) || (list_pdbscopids==NULL) || (siz_domains==NULL) 
     || (siz_heterogens==NULL) || (vdw==NULL) || (dbase==NULL)) 
    {
      ajWarn("sites_HeterogenContacts: Bad arguments passed to function\n");
      return ajFalse;
    }
  
  if(!(*dbase))
    {
      ajWarn("sites_HeterogenContacts: Bad arguments passed to function\n");
      return ajFalse;
    }
  
  
  ajListToarray(list_pdbscopids, (void ***) &scopidArr);
  ajListToarray(list_seqs, (void ***) &chainseqsArr);




  /* Create Temporary list for all AjPDomConts - check this i.e. all? */
  cont_dataList=ajListNew();
  
  iter_dom=ajListIterNew(list_domains); 
  while((dom_atm=(AjPAtom *)ajListIterGet(iter_dom)))
    {
      idx_tmp=0;

      scopctr++; /* increment scopidArr index */			        
      DomIdx++; /* increment siz_domain index */
      HetIdx=0; /* initialise siz_heterogens index */

/* initialise iterator for list_heterogens */
      iter_het=ajListIterNew(list_heterogens);     
/* Get size of current domain atom array */
      dom_max=ajIntGet(siz_domains, DomIdx-1);  

      while((het_atm=(AjPAtom *)ajListIterGet(iter_het)))
	{
	  hetctr++;
	  het_max=0;
	  fflush(stdout);
	  temp=ajStrNew();
	  aaTempList=ajListNew();
	  res_pos2TempList=ajListNew();

	  cont_dataTemp=sites_DomContsNew(0);
	  fflush(stdout);
	  HetIdx++; /* increment siz_heterogens index */
	   /* Get size of current heterogen atom array */
	  het_max=ajIntGet(siz_heterogens, HetIdx-1);
	  fflush(stdout);


	  /* loop through current domain atoms array */

	  /* (i) loop through domains*/
	  for(i=0;i<dom_max;++i)
	  {
	      /* Disregard any non-protein atoms */
	      if(dom_atm[i]->Type!='P') 
		  continue; 

	      /*Current residue check*/
	      if(dom_atm[i]->Idx==idx_tmp) 
		  continue; 

	      
	      /*Check that the coordinates are valid i.e. non zero values*/

		
	      if((ajStrMatchC(dom_atm[i]->Id3, "ALA"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "CYS"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "ASP"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "GLU"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "PHE"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "GLY"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "HIS"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "ILE"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "LYS"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "LEU"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "MET"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "ASN"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "PRO"))|| 
		 (ajStrMatchC(dom_atm[i]->Id3, "GLN"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "ARG"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "SER"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "THR"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "VAL"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "TRP"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "TYR"))||
		 (ajStrMatchC(dom_atm[i]->Id3, "GLX")))
		{		  
		  
		  /* (j) loop through current hetetrogen atoms array */
		  for(j=0;j<het_max;j++)
		    {
		      if(het_atm[j]->Id3 == NULL)
			continue;
		      
		      /*Check that the hetatm coordinates are valie i.e. 
			non zero values*/

		      cont_dataTemp->type=entype;
		      if(entype == ajWADOMAIN)
			{
			  embScopToPdbid(scopidArr[scopctr-1], 
					 &(cont_dataTemp)->pdb_name);
			  ajStrAssignS(&(cont_dataTemp)->scop_name, 
				    scopidArr[scopctr-1]);
			}
		      else
			{
			  ajStrAssignC(&(cont_dataTemp)->scop_name, ".");
			  ajStrAssignS(&(cont_dataTemp)->pdb_name, 
				    scopidArr[scopctr-1]);
			}

		      (cont_dataTemp)->chainid 
			  = ajChararrGet(arr_chains, scopctr-1);
		      


		      (cont_dataTemp)->Nres = ajIntGet(arr_nres, scopctr-1);
		      ajStrAssignS(&(cont_dataTemp)->Seq, 
				chainseqsArr[scopctr-1]); 


		      
		      /* Check that the group numbers have been 
			 assigned correctly here, temp can be used in place 
			 of het_atm[j]->Id3 for diagnostics */
		      ajStrAssignS(&(cont_dataTemp)->het_name, het_atm[j]->Id3);

		      
		      if(embAtomInContact(dom_atm[i], het_atm[j], dist_thresh,
					  vdw))
			{
			  (cont_dataTemp)->no_keyres++;
			  ajIntPut(&(cont_dataTemp)->res_pos,
				   ((cont_dataTemp)->no_keyres)-1, 
				   dom_atm[i]->Idx);
			  tempaa=ajStrNew();
			  ajStrAssignS(&tempaa, dom_atm[i]->Id3);		      
			  ajListPushAppend(aaTempList, (void *) tempaa);
			  tempres_pos2=ajStrNew();
			  /* ajStrAssignS(&tempres_pos2, dom_atm[i]->Pdb); */
			  ajListPushAppend(res_pos2TempList, tempres_pos2);
			  idx_tmp=dom_atm[i]->Idx;
			  break; /* heterogen atoms array loop */
			  
			} /* if embAtomInContact */		  
		      ajStrDel(&temp);
		    } /* Current heterogen atoms array loop */
		} /* Loop to test the current atom is a residue atom*/	      
	    } /* Current domain atoms array loop */
	  
	  
	  if(cont_dataTemp->no_keyres > 0)
	    {  
	      ajListToarray(aaTempList, (void ***) &(cont_dataTemp)->aa_code);
	      ajListToarray(res_pos2TempList, 
			    (void ***) &(cont_dataTemp)->res_pos2);
	      ajListPushAppend(cont_dataList, cont_dataTemp);
	      cont_dataTemp=NULL;
	    } 
	  else
	    sites_DomContsDel(&cont_dataTemp);
	  
	  ajListFree(&aaTempList);
	  ajListFree(&res_pos2TempList);
	  
	  /* cont_dataTemp=NULL; */
	  
	} /* while het_atm */
      
      ajListIterDel(&iter_het);
      
    } /* while dom_atm */
  
  res_ctr=0;
  
  while(ajListPop(cont_dataList,(void **)&dom_cont))
      for(i=0; i<(*dbase)->n; i++) 
	  if(ajStrMatchS(dom_cont->het_name, (*dbase)->entries[i]->abv))
	      ajListPushAppend((*dbase)->entries[i]->tmp, dom_cont);
  
  ajListFree(&cont_dataList);
  ajListIterDel(&iter_dom);
  AJFREE(scopidArr);
  AJFREE(chainseqsArr); 
  

  return ajTrue;
}


/* @func sites_Dummy **********************************************************
**
** Dummy function to catch all unused functions defined in the sites
** source file.
**
** @return [void]
** @@
******************************************************************************/

void sites_Dummy (void)
{
    AjPFile outfile = NULL;
    AjPDbase dbase = NULL;
    sites_HeterogenContactsWriteOld(outfile, dbase);
    return;
}



/* @funcstatic  sites_HeterogenContactsWriteOld ******************************
**
** Write Dbase object to file i.e. the database of functional residues. The
** OLD ligand-centric format is used (*NOT* CON format).
**
** @param [w] funky_out    [AjPFile]   Pointer to output file
** @param [r] dbase        [AjPDbase*] Pointer to Dbase object
** 
** 
** @return [AjBool] True on success
** @@
******************************************************************************/

static AjBool    sites_HeterogenContactsWriteOld(AjPFile funky_out, 
						 AjPDbase dbase)
{

  ajint i=0;  /* loop counter for dbase->entries[i] */
  ajint j=0;  /* loop counter for dbase->entries[i]->cont_data[j] */
  ajint k=0;  /* loop counter for dbase->entries[i]->cont_data[j]->aa_code[j]
		 and dbase->entries[i]->cont_data[j]->res_pos[j] */

  /* Check arguments */
  if((funky_out==NULL) || (dbase==NULL)) 
  {
      ajWarn("Pointer error in funky_HeterogenContactsWriteOld\n");
      return ajFalse;
  }
  

  for(i=0;i<dbase->n;i++)
    {
      if((dbase)->entries[i]->no_sites >0)
	{
	  ajFmtPrintF(funky_out, "ID   %S\n", dbase->entries[i]->abv);
	  ajFmtPrintF(funky_out, "DE   %S\n", dbase->entries[i]->ful);
	  ajFmtPrintF(funky_out, "NS   %d\n", dbase->entries[i]->no_sites);
	  ajFmtPrintF(funky_out, "XX\n");
	  for(j=0;j<(dbase)->entries[i]->no_sites; j++)
	    {
	      ajFmtPrintF(funky_out, "SN   %d\n", j+1);
	      ajFmtPrintF(funky_out, "XX\n");
	      ajFmtPrintF(funky_out, "EN   %S\n", 
			  dbase->entries[i]->cont_data[j]->pdb_name);
	      ajFmtPrintF(funky_out, "XX\n");
	      ajFmtPrintF(funky_out, "CH   %c\n", 
			  dbase->entries[i]->cont_data[j]->chainid);
	      ajFmtPrintF(funky_out, "XX\n");

	      ajFmtPrintF(funky_out, "SC   %S\n", 
			  dbase->entries[i]->cont_data[j]->scop_name);
	      ajFmtPrintF(funky_out, "XX\n");
	      if(dbase->entries[i]->cont_data[j]->no_keyres > 0)
		{
		  ajFmtPrintF(funky_out, "NR   %d\n", 
			      dbase->entries[i]->cont_data[j]->no_keyres);
		  ajFmtPrintF(funky_out, "XX\n");
		  
		  for(k=0; k< dbase->entries[i]->cont_data[j]->no_keyres; k++)
		    {
		      if((ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "ALA"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "CYS"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "ASP"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "GLU"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "PHE"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "GLY"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "HIS"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "ILE"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "LYS"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "LEU"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "MET"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "ASN"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "PRO"))|| 
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "GLN"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "ARG"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "SER"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "THR"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "VAL"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "TRP"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "TYR"))||
			 (ajStrMatchC(dbase->entries[i]->cont_data[j]->aa_code[k], "GLX")))
			{
			  ajFmtPrintF(funky_out, "RE   %S %d %S\n",  
				      dbase->entries[i]->cont_data[j]->aa_code[k], 
				      ajIntGet(dbase->entries[i]->cont_data[j]->res_pos, k), 
				      dbase->entries[i]->cont_data[j]->res_pos2[k]); 
			}
		    }
		  ajFmtPrintF(funky_out, "XX\n");
		  ajFmtPrintF(funky_out, "//\n");
		}
	    }
	}
    }

  return ajTrue;
}








/* @funcstatic  sites_HeterogenContactsWrite *********************************
**
** Write Dbase object to file i.e. the database of functional residues.
**
** @param [w] outf    [AjPFile]   Pointer to output file
** @param [r] dbase   [AjPDbase]  Pointer to Dbase object
** @param [r] thresh  [float]     Contact distance threshold
** 
** 
** @return [AjBool] True on success
** @@
******************************************************************************/

static AjBool    sites_HeterogenContactsWrite(AjPFile outf, 
					      AjPDbase dbase, 
					      float thresh)
{

  ajint i=0;   /* loop counter for dbase->entries[i] */
  ajint j=0;   /* loop counter for dbase->entries[i]->cont_data[j] */
  ajint k=0;   /* loop counter for dbase->entries[i]->cont_data[j]->aa_code[j] 
		      and dbase->entries[i]->cont_data[j]->res_pos[j] */
  ajint entry=0;
  ajint total_entry=0;
  AjPStr ptr=NULL;
  AjPSeqout outseq = NULL;

  /* Check arguments */
  if((outf==NULL) || (dbase==NULL)) 
  {
      ajWarn("Pointer error in sites_HeterogenContactsWrite\n");
      return ajFalse;
  }
  
  /* Print out header data */
  ajFmtPrintF(outf, "%-5s%s\n", 
	      "XX", "Residue-ligand contact data (for domains).");
  ajFmtPrintF(outf, "XX\n");


  /* TY */
  ajFmtPrintF(outf, "%-5s%s\n", 
	      "TY", "LIGAND");
  ajFmtPrintF(outf, "XX\n");


  /* EX */
  ajFmtPrintF(outf, "%-5s%s%.1f; %s.; NMOD .; NCHA .;\n", 
	      "EX", "THRESH ", thresh, "IGNORE ");
  ajFmtPrintF(outf, "XX\n");  

  
  /* NE */
  for(i=0;i<dbase->n;i++)
      if((dbase)->entries[i]->no_sites >0)
	  for(j=0;j<(dbase)->entries[i]->no_sites; j++)
	      ++total_entry;
  /*  ajFmtPrintF(outf, "%-5s%d\n", "NE", dbase->n); */
  ajFmtPrintF(outf, "%-5s%d\n", "NE", total_entry);
  ajFmtPrintF(outf, "XX\n");   
  

  /* Start of loop to print out data for each entry (ligand:domain pair) */
  for(i=0;i<dbase->n;i++)
  {
    if((dbase)->entries[i]->no_sites >0)
      {
	  
	  for(j=0;j<(dbase)->entries[i]->no_sites; j++)
	  {
	      /* EN */
	      ajFmtPrintF(outf, "%-5s[%d]\n", "EN", ++entry);
	      ajFmtPrintF(outf, "XX\n");  
		
	      /* ID */
	      ajFmtPrintF(outf, "%-5sPDB %S; DOM %S; LIG %S;\n", 
			  "ID", 
			  dbase->entries[i]->cont_data[j]->pdb_name, 
			  dbase->entries[i]->cont_data[j]->scop_name,
			  dbase->entries[i]->abv);
	      ajFmtPrintF(outf, "XX\n");  


	      /* DE */
	      ajFmtPrintF(outf, "DE   %S\n", dbase->entries[i]->ful);
	      ajFmtPrintF(outf, "XX\n");

	      
	      /* SI */
	      ajFmtPrintF(outf, "%-5sSN %d; NS %d\n", "SI", j+1, 
			  (dbase)->entries[i]->no_sites);		
	      ajFmtPrintF(outf, "XX\n");


	      /* CN */
	      ajFmtPrintF(outf, "%-5sMO .; CN1 1; CN2 .; ID1 %c; ID2 .; "
			  "NRES1 %d; NRES2 .\n",
			  "CN", 
			  dbase->entries[i]->cont_data[j]->chainid, 
			  dbase->entries[i]->cont_data[j]->Nres);
	      ajFmtPrintF(outf, "XX\n");  

	      
	      /* S1 */
	      if(dbase->entries[i]->cont_data[j]->Nres != 0)
	      {
		  outseq = ajSeqoutNewFile(outf);
		  ajSeqoutDumpSwisslike(outseq,
					dbase->entries[i]->cont_data[j]->Seq, 
					"S1");
		  ajSeqoutDel(&outseq);
		  ajFmtPrintF(outf, "XX\n");	
	      }
	      
	      
	      /* NC */
	      if(dbase->entries[i]->cont_data[j]->Nres == 0)
	      {
		  ajFmtPrintF(outf, "%-5sSM .; LI 0\n", "NC");
		  ajFmtPrintF(outf, "XX\n");  
	      }
	      else
	      {
		  ajFmtPrintF(outf, "%-5sSM .; LI %d\n", 
			      "NC", dbase->entries[i]->cont_data[j]->no_keyres);
		  ajFmtPrintF(outf, "XX\n");  
	      }
	      

	      /* SM */
	      if(dbase->entries[i]->cont_data[j]->no_keyres > 0)
	      {
		  for(k=0; k< dbase->entries[i]->cont_data[j]->no_keyres; k++)
		  {
		      ptr=dbase->entries[i]->cont_data[j]->aa_code[k];
		      
		      if((ajStrMatchC(ptr, "ALA"))||
			 (ajStrMatchC(ptr, "CYS"))||
			 (ajStrMatchC(ptr, "ASP"))||
			 (ajStrMatchC(ptr, "GLU"))||
			 (ajStrMatchC(ptr, "PHE"))||
			 (ajStrMatchC(ptr, "GLY"))||
			 (ajStrMatchC(ptr, "HIS"))||
			 (ajStrMatchC(ptr, "ILE"))||
			 (ajStrMatchC(ptr, "LYS"))||
			 (ajStrMatchC(ptr, "LEU"))||
			 (ajStrMatchC(ptr, "MET"))||
			 (ajStrMatchC(ptr, "ASN"))||
			 (ajStrMatchC(ptr, "PRO"))|| 
			 (ajStrMatchC(ptr, "GLN"))||
			 (ajStrMatchC(ptr, "ARG"))||
			 (ajStrMatchC(ptr, "SER"))||
			 (ajStrMatchC(ptr, "THR"))||
			 (ajStrMatchC(ptr, "VAL"))||
			 (ajStrMatchC(ptr, "TRP"))||
			 (ajStrMatchC(ptr, "TYR"))||
			 (ajStrMatchC(ptr, "GLX")))
		      {
			  ajFmtPrintF(outf, "LI   %S %d\n",  
				      ptr, 
				      ajIntGet(dbase->entries[i]->cont_data[j]->res_pos, k));
		      }
		  }
		  ajFmtPrintF(outf, "XX\n");
	      }
	      ajFmtPrintF(outf, "//\n");
	  }
      }
  }	

  return ajTrue;
}






/* @funcstatic sites_HetTest **************************************************
**
** Tests for presence of heterogens in a pdb object. Returns True if a 
** heterogen is found.
**
** @param [r] pdb_ptr [AjPPdb] Pointer to pdb object
**
** @return [AjBool] True on success
** @@
******************************************************************************/

static AjBool sites_HetTest(AjPPdb pdb_ptr)
{
  /* False = no hets found */
  /* True = hets found */
  ajint i=0;
  
  if(pdb_ptr->Nchn>0)
      for(i=0;i<pdb_ptr->Nchn;++i) 
	  if(pdb_ptr->Chains[i]->Nlig>0)
	      return ajTrue;

  if(pdb_ptr->Ngp>0)
      return ajTrue;

  
  return ajFalse;
}







/* @funcstatic sites_DbaseNew *************************************************
**
** Constructor for Dbase object.
** 
** @param [r] n [ajint] Number of entries in database.
** 
** @return [AjPDbase] Dbase object pointer.
** @@
*************************************************************************/
static AjPDbase  sites_DbaseNew(ajint n)
{
    AjPDbase ret = NULL;
    
    AJNEW0(ret);
    
    ret->n = n;
    
    if(n)
	AJCNEW0(ret->entries, n);
    else
    {
	ajWarn("Arg with value zero passed to sites_DbaseNew");
	ret->entries=NULL;
    }

    return ret;
}






/* @funcstatic sites_DbaseDel *************************************************
**
** Destructor for Dbase object.
** 
** @param [r] ptr [AjPDbase*] Pointer to the Dbase object.
**
** @return [void] 
** @@
******************************************************************************/

static void sites_DbaseDel(AjPDbase *ptr)
{
    ajint i = 0;
  
    /* Check arg's */
    if(ptr==NULL) 
    {	
	ajWarn("Attemp to free NULL pointer in sites_DbaseDel");
	return; 
    }

    if(*ptr==NULL) 
    {	
	ajWarn("Attemp to free NULL pointer in sites_DbaseDel");
	return; 
    }

    if((*ptr)->entries)
    {
	for(i=0;i<(*ptr)->n;i++)
	    if((*ptr)->entries[i])
		sites_DbaseEntDel(&((*ptr)->entries[i]));
	AJFREE((*ptr)->entries);
    }

    AJFREE((*ptr));
    *ptr = NULL;

    return;
}





/* @funcstatic sites_DbaseEntNew **********************************************
**
** Constructor for DbaseEnt object.
** 
** @param [r] n [ajint] number of entries in array of domain contact residues.
**
** @return [AjPDbaseEnt] DbaseEnt object pointer.
** @@
******************************************************************************/

static AjPDbaseEnt sites_DbaseEntNew(ajint n)
{
    AjPDbaseEnt ret = NULL;
    AJNEW0(ret);


    ret->abv = ajStrNew();
    ret->ful = ajStrNew();
    ret->tmp = ajListNew();
    ret->no_sites = n;
  

    if(n)
	AJCNEW0(ret->cont_data, n);
    else
	ret->cont_data = NULL;

    return ret;
}






/* @funcstatic sites_DbaseEntDel **********************************************
**
** Destructor for DbaseEnt object.
** 
** @param [w] ptr [AjPDbaseEnt *] DbaseEnt object pointer.
**
** @return [void] 
** @@
******************************************************************************/

static void sites_DbaseEntDel(AjPDbaseEnt *ptr)
{
    ajint x = 0;
    
    /* Check arg's */
    if(*ptr==NULL) 
    {	
	ajWarn("Attemp to free NULL pointer in sites_DbaseEntDel");
	return; 
    }

    ajStrDel(&(*ptr)->abv);
    ajStrDel(&(*ptr)->ful);
    ajListFree(&(*ptr)->tmp);
  
    if((*ptr)->cont_data)
    {
	for(x=0;x<(*ptr)->no_sites;x++)
	    sites_DomContsDel(&((*ptr)->cont_data[x]));

	AJFREE((*ptr)->cont_data);
    }
  
    
    AJFREE((*ptr));
    *ptr = NULL;

    return;
}







/* @funcstatic sites_DomContsNew **********************************************
**
** Constructor for DomConts object
** 
** @param [r] n [ajint] Number of amino acids that make contact with the ligand.
** 
** @return [AjPDomConts] DomConts object pointer. 
** @@
******************************************************************************/

static AjPDomConts sites_DomContsNew(ajint n)
{
    AjPDomConts ret = NULL;
    ajint i = 0;

    AJNEW0(ret);

    ret->pdb_name  = ajStrNew();
    ret->chainid   = '.';
    ret->Nres      = 0;
    ret->scop_name = ajStrNew();
    ret->het_name  = ajStrNew();
    ret->Seq       = ajStrNew();
    ret->no_keyres = n;
  
    if(n)
    {
	AJCNEW0(ret->aa_code, n);      
	AJCNEW0(ret->res_pos2, n);      
	for(i=0;i<n;i++)
	{
	    ret->aa_code[i]=ajStrNew();
	    ret->res_pos2[i]=ajStrNew(); 
	}
	ret->res_pos=ajIntNewRes(n);    
    }
    else
    {
	/* ajWarn("Zero sized arg passed to sites_DomContsNew.\n"); */
	ret->res_pos=ajIntNew();    
	ret->aa_code  = NULL;
	ret->res_pos2 = NULL;		
    }	

    return ret;
}





         
/* @funcstatic sites_DomContsDel **********************************************
**
** Destructor for DomConts object.
** 
** @param [r]  ptr [AjPDomConts*] Pointer to AjPDomConts object.
** 
** @return [void]
** @@
******************************************************************************/

static void sites_DomContsDel(AjPDomConts *ptr) 
{
    ajint i = 0;

    /* Check arg's */
    if(*ptr==NULL) 
    {	
	ajWarn("Attemp to free NULL pointer in sites_DomContsDel");
	return; 
    }
    ajStrDel(&(*ptr)->scop_name);
    ajStrDel(&(*ptr)->het_name);
    ajStrDel(&(*ptr)->pdb_name);
    ajStrDel(&(*ptr)->Seq);

    for(i=0;i<(*ptr)->no_keyres;++i)
    {
	ajStrDel(&(*ptr)->aa_code[i]);
	ajStrDel(&(*ptr)->res_pos2[i]); 
    }

    if((*ptr)->aa_code)
	AJFREE((*ptr)->aa_code);

    if((*ptr)->res_pos2)
	AJFREE((*ptr)->res_pos2);


    ajIntDel(&(*ptr)->res_pos);
    AJFREE(*ptr);

    *ptr = NULL;

    return;
}

