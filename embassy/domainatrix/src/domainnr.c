/* @source domainnr application
**
** Removes redundant domains from a DCF file (domain classification file). 
** The file must contain domain sequence information, which can be added by 
** using DOMAINSEQS.
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
**  DOMAINNR documentation
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
******************************************************************************/






#include "emboss.h"




/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static void domainnr_diagnostic(AjPFile errf,
				AjPDomain domain, 
				ajint noden);

static void domainnr_writelast(AjPDomain domain, 
			       ajint noden, 
			       AjPStr *last_node, 
			       ajint *last_nodeid);






/* @prog domainnr *************************************************************
**
** Removes redundant domains from a DCF file (domain classification file). 
** The file must contain domain sequence information, which can be added by 
** using DOMAINSEQS.
**
******************************************************************************/

int main(int argc, char **argv)
{
    ajint      x         = 0;
    ajint      nset      = 0;	 /* No. proteins in our  set.                */
    ajint      nsetnr    = 0;	 /* No. proteins in the non-redundant set.   */
    ajint      nfam      = 0;	 /* Counter of domain families.              */

    float      gapopen   = 0.0;	 /* Gap insertion penalty.                   */
    float      gapextend = 0.0;	 /* Gap extension penalty.                   */
    float      thresh    = 0.0;	 /* Threshold for non-redundancy (single value)*/
    float      threshlow = 0.0;	 /* Threshold for non-redundancy (lower limit)*/
    float      threshup  = 0.0;	 /* Threshold for non-redundancy (upper limit)*/

    ajint      last_nodeid=  0;  /* Domain id (e.g. SCOP Sunid) of last node 
				    that was processed.                      */
    AjPStr     last_node = NULL; /* Last node that was processed.            */
    AjPStr     msg       = NULL; /* String used for messages.                */

    AjPFile    domain_inf= NULL; /* File pointer for original domain file.   */
    AjPFile    domain_outf= NULL;/* File pointer for domain output file.     */
    AjPFile    red_outf  = NULL; /* File pointer for redundant sequences file.*/
    AjPFile    errf      = NULL;

    AjPList    list_seqs = NULL; /* List of sequences for a family.          */
    AjPList    list_domain = NULL;/* List of domain structures for a family. */
    AjIList    iter      = NULL; /* Iterator for domain list.                */    
    AjPUint    keep      = NULL; /*1: This sequence was kept after.          */
                                 /* redundancy removal, 0: it was discarded. */
    EmbPDmxNrseq nrseq   = NULL; /* A temporary pointer.                     */
    AjPMatrixf matrix    = NULL; /* Substitution matrix.                     */
    AjPDomain  domain    = NULL; /* Pointer to domain structure.             */
    AjPDomain  domain_tmp= NULL; /* Pointer to domain structure.             */
    AjPStr    *mode      = NULL; /* Mode of operation from acd.              */
    ajint      moden     = 0;    /* Program mode as int 1: use single threshold
				    value to remove redundancy, 2: use lower and
				    upper limit to remove redundancy */

    AjPStr      *node   =NULL;   /* Node of redundancy removal.              */
    ajint     noden      =0;     /*1: Class (SCOP), 2: Fold (SCOP) etc, see 
				   ACD file */
    char *nodes[]=
    {
	"Classes", "Folds", "Superfamilies", "Families", "Classes", 
	"Architectures", "Topologies", "Homologous Superfamilies", 
	"Families"
    };
    
    AjBool    ret=ajFalse;
    AjBool    retain = ajFalse; /*Whether to retain redundant sequences and
				  write them to an output file.              */
    ajint     type = 0;   /* Type of domain (ajSCOP or ajCATH) in the DCF file */





    
    /* Initialise strings etc & last_node with a value that is not in a DCF 
       file */
    msg       = ajStrNew();
    last_node  = ajStrNew();
    keep = ajUintNew();  	    
    ajStrAssignC(&last_node,"!!!!!");




    
    /* Read data from acd */
    embInitPV("domainnr",argc,argv,"DOMAINATRIX",VERSION);
    domain_inf  = ajAcdGetInfile("dcfinfile");
    domain_outf = ajAcdGetOutfile("dcfoutfile");
    matrix    = ajAcdGetMatrixf("datafile");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    errf      = ajAcdGetOutfile("logfile");
    mode      = ajAcdGetList("mode");
    node      = ajAcdGetList("node");
    retain    = ajAcdGetToggle("retain");

    if(ajStrGetCharFirst(*mode) == '1')
	{
	    moden=1;
	    thresh    = ajAcdGetFloat("threshold");
	}
    
    else if(ajStrGetCharFirst(*mode) == '2')
	{
	    moden=2;
	    threshlow    = ajAcdGetFloat("threshlow");
	    threshup     = ajAcdGetFloat("threshup");
	}
    if(retain)
	red_outf = ajAcdGetOutfile("redoutfile");


    
    /* Convert the selected node to an integer. */
    if(!(ajStrToInt(node[0], &noden)))
	ajFatal("Could not parse ACD node option");
      


    /* Write header of log file.
       Replace hard-coded 'FAMILIES' with appropriate string when. 
       redundancy removal is implemented for any node in DCF file. 
       */
    ajFmtPrintF(errf, "%s are non-redundant\n"
		"%.0f%% redundancy threshold\n", nodes[noden-1], thresh); 


    
    type = ajDomainDCFType(domain_inf);

 
    /* Start of main application loop. */
    while((domain=(ajDomainReadCNew(domain_inf, "*", type))))
    {
	/* If we are on to a new family*/
	if(((domain->Type == ajSCOP) &&
	    (((noden==1) && (last_nodeid !=  domain->Scop->Sunid_Class))      || 
	     ((noden==2) && (last_nodeid !=  domain->Scop->Sunid_Fold))        || 
	     ((noden==3) && (last_nodeid !=  domain->Scop->Sunid_Superfamily)) || 
	     ((noden==4) && (last_nodeid !=  domain->Scop->Sunid_Family))))       ||
	   ((domain->Type == ajCATH) &&
	    (((noden==5) && (last_nodeid !=  domain->Cath->Class_Id))         || 
	     ((noden==6) && (last_nodeid !=  domain->Cath->Arch_Id))          || 
	     ((noden==7) && (last_nodeid !=  domain->Cath->Topology_Id))      || 
	     ((noden==8) && (last_nodeid !=  domain->Cath->Superfamily_Id))   || 
	     ((noden==9) && (last_nodeid !=  domain->Cath->Family_Id)))))
	{
	    /* If we have done the first family. */
	    if(nfam)
	    {
		/* If the family does not have any members*/
		if(!nset)
		{
		    /* Write diagnostic. */
        	    ajWarn("Empty node - should never happen\n"); 
		}
		/* Last family had members, so remove redundancy. */
		else
		{
		    /* Remove redundancy from list_seqs. */
		    if(moden==1)
			ret=embDmxSeqNR(list_seqs, &keep, &nsetnr, matrix, 
					gapopen, gapextend,thresh, ajFalse);
		    else
			ret=embDmxSeqNRRange(list_seqs, &keep,
					     &nsetnr, matrix, 
					     gapopen, gapextend,threshlow, 
					     threshup, ajFalse);


		    if(!ret)
		    {
			ajStrDel(&last_node);
			ajStrDel(&msg);
			ajUintDel(&keep);	
			ajMatrixfDel(&matrix);
			ajFileClose(&errf);    
			ajFileClose(&domain_outf);	    
			if(retain)
			    ajFileClose(&red_outf);	    
			ajStrDel(&mode[0]);
			AJFREE(mode);
			ajStrDel(&node[0]);
			AJFREE(node);
			ajFatal("Unexpected embDmxSeqNR error");
		    }
		    

		    /* Write file with domain entries that are retained. */
		    for(iter=ajListIterNewread(list_domain), x=0;
			(domain_tmp=(AjPDomain)ajListIterGet(iter));
			x++)
			if(ajUintGet(keep,x))
			    ajDomainWrite(domain_outf, domain_tmp);
			else
			    ajDomainWrite(red_outf, domain_tmp);
		    ajListIterDel(&iter);	


		    /* Write diagnostic. */
 		    ajFmtPrintF(errf, "Retained\n");
		    for(iter=ajListIterNewread(list_domain), x=0;
			(domain_tmp=(AjPDomain)ajListIterGet(iter));
			x++)
			if(ajUintGet(keep,x))
			    ajFmtPrintF(errf, "%S\n", 
					ajDomainGetId(domain_tmp));
		    
		    

		    ajListIterDel(&iter);
		    ajFmtPrintF(errf, "Rejected\n");
		    for(iter=ajListIterNewread(list_domain), x=0;
			(domain_tmp=(AjPDomain)ajListIterGet(iter));
			x++)
			if(!(ajUintGet(keep,x)))
			    ajFmtPrintF(errf, "%S\n", 
					ajDomainGetId(domain_tmp));
		    ajListIterDel(&iter);	

		    
		    /* Write diagnostic. */
		    domainnr_diagnostic(errf, domain, noden);
		}
		/* Intitiliase counter for number in family. */
		nset=0;



		/* Free up the domain list and create a new one. */
		iter=ajListIterNewread(list_domain);
		while((domain_tmp=(AjPDomain)ajListIterGet(iter)))
		    ajDomainDel(&domain_tmp);
		ajListIterDel(&iter);	
		ajListFree(&list_domain);	    


		/* Free up the seqs list and create a new one. */
		iter=ajListIterNewread(list_seqs);
		while((nrseq=(EmbPDmxNrseq)ajListIterGet(iter)))
		{
		    ajSeqDel(&nrseq->Seq);
		    AJFREE(nrseq);
		}
		ajListIterDel(&iter);	
		ajListFree(&list_seqs);	    
	    }
	    else
	    {
		/* We have not done the first family. 
		   Write diagnostic. */
		domainnr_diagnostic(errf, domain, noden);
	    }
	    
	    
	    /* Write the current family to last_node. */
	    domainnr_writelast(domain, noden, &last_node, &last_nodeid);
	    
	    /* Create new list for domain and seqs. */
	    list_seqs = ajListNew();
	    list_domain = ajListNew();

	    
	    /* Increment counter for number of families. */
	    nfam++;
	}
	/* Write diagnostic. */
	ajFmtPrint("%S\n", ajDomainGetId(domain));
	


		
	/* Add a new sequence to the list - only ever one chain (chain 0) 
	   for domain files. */
	nset++;
	AJNEW0(nrseq);
	nrseq->Seq=ajSeqNew();

	/* pdb sequence has priority. */
        if((ajStrGetLen(ajDomainGetSeqPdb(domain)))==0)
            ajStrAssignS(&nrseq->Seq->Seq, ajDomainGetSeqSpr(domain));
        else
            ajStrAssignS(&nrseq->Seq->Seq, ajDomainGetSeqPdb(domain));

	ajStrAssignS(&nrseq->Seq->Name, ajDomainGetPdb(domain));
	ajListPushAppend(list_seqs,nrseq);	


	/* Add the current domain structure to the list. */
	ajListPushAppend(list_domain,domain);	
    }
    /* End of main application loop. */
    
    
    
    
    /* Close the domain input file. */
    ajFileClose(&domain_inf);	
    
    
    /* Process the last family. 
       If the family does not have any members */
    if(!nset)
    {
	/* Write diagnostic. */
	ajFmtPrint("WARN  Empty node\n");
	ajFmtPrintF(errf, "WARN  Empty node\n");
    }
    /* Last family had members, so remove redundancy. */
    else
    {
	/* Remove redundancy from list_seqs. */
	if(moden==1)
	    embDmxSeqNR(list_seqs, &keep, &nsetnr, matrix, gapopen, 
			gapextend,thresh, ajFalse);		
	else
	    embDmxSeqNRRange(list_seqs, &keep, &nsetnr, matrix, gapopen, 
			     gapextend,threshlow, threshup, ajFalse);

	/* Write file with domain entries that are retained. */
	for(iter=ajListIterNewread(list_domain), x=0;
	    (domain_tmp=(AjPDomain)ajListIterGet(iter));
	    x++)
	    if(ajUintGet(keep,x))
		ajDomainWrite(domain_outf, domain_tmp);
	    else
		ajDomainWrite(red_outf, domain_tmp);
	ajListIterDel(&iter);	
    


	/* Write diagnostic. */
	ajFmtPrintF(errf, "Retained\n");
	for(iter=ajListIterNewread(list_domain), x=0;
	    (domain_tmp=(AjPDomain)ajListIterGet(iter));
	    x++)
	    if(ajUintGet(keep,x))
		ajFmtPrintF(errf, "%S\n", ajDomainGetId(domain_tmp));
	ajListIterDel(&iter);	
	ajFmtPrintF(errf, "Rejected\n");
	for(iter=ajListIterNewread(list_domain), x=0;
	    (domain_tmp=(AjPDomain)ajListIterGet(iter));
	    x++)
	    if(!(ajUintGet(keep,x)))
		ajFmtPrintF(errf, "%S\n", ajDomainGetId(domain_tmp));
	ajListIterDel(&iter);	
    }
    
    /* Free up the domain list. */
    iter=ajListIterNewread(list_domain);
    while((domain_tmp=(AjPDomain)ajListIterGet(iter)))
	ajDomainDel(&domain_tmp);
    ajListIterDel(&iter);	
    ajListFree(&list_domain);	    
    
    
    /* Free up the seqs list. */
    iter=ajListIterNewread(list_seqs);
    while((nrseq=(EmbPDmxNrseq)ajListIterGet(iter)))
    {
	ajSeqDel(&nrseq->Seq);
	AJFREE(nrseq);
    }
    ajListIterDel(&iter);	
    ajListFree(&list_seqs);	    
    
    
    /* Tidy up. */
    ajMatrixfDel(&matrix);
    ajUintDel(&keep);	
    ajStrDel(&last_node);
    ajStrDel(&msg);
    ajFileClose(&errf);    
    ajFileClose(&domain_outf);	    
    if(retain)
	ajFileClose(&red_outf);	    
    ajStrDel(&mode[0]);
    AJFREE(mode);
    ajStrDel(&node[0]);
    AJFREE(node);
    
    /* Bye Bye. */
    ajExit();
    return 0;
}





/* @funcstatic domainnr_diagnostic   ******************************************
**
** Writes diagnostics messages to file.
**
** @@
******************************************************************************/
static void domainnr_diagnostic(AjPFile errf, AjPDomain domain, ajint noden)
{
    if(noden==1) 
    {
	ajFmtPrintF(errf, "// %S\n", domain->Scop->Class);
	ajFmtPrint("// %S\n", domain->Scop->Class);
    }		       
    else if (noden==2)
    {
	ajFmtPrintF(errf, "// %S\n", domain->Scop->Fold);
	ajFmtPrint("// %S\n",domain->Scop->Fold);
    }
    else if (noden==3)
    {
	ajFmtPrintF(errf, "// %S\n",  
		    domain->Scop->Superfamily);
	ajFmtPrint("// %S\n", domain->Scop->Superfamily);
    }
    else if (noden==4)
    {
	ajFmtPrintF(errf, "// %S\n", domain->Scop->Family);
	ajFmtPrint("// %S\n",domain->Scop->Family);
    } 	
    else if (noden==5)
    {
	ajFmtPrintF(errf, "// %S\n", domain->Cath->Class);
	ajFmtPrint("// %S\n",domain->Cath->Class);
    } 
    else if (noden==6)
    {
	ajFmtPrintF(errf, "// %S\n", domain->Cath->Architecture);
	ajFmtPrint("// %S\n",domain->Cath->Architecture);
    } 
    else if (noden==7)
    {
	ajFmtPrintF(errf, "// %S\n", domain->Cath->Topology);
	ajFmtPrint("// %S\n",domain->Cath->Topology);
    } 
    else if (noden==8)
    {
	ajFmtPrintF(errf, "// %S\n", domain->Cath->Superfamily);
	ajFmtPrint("// %S\n",domain->Cath->Superfamily);
    } 
    else if (noden==9)
    {
	/* There is no text describing the CATH families. */
	ajFmtPrintF(errf, "// %d\n", domain->Cath->Family_Id);
	ajFmtPrint("// %d\n",domain->Cath->Family_Id);
    } 
}


/* @funcstatic domainnr_writelast *********************************************
**
** House-keeping function.
**
** @@
******************************************************************************/
static void domainnr_writelast(AjPDomain domain, ajint noden, AjPStr *last_node, 
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



