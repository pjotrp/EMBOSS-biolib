/* @source seqnr application
**
** Removes redundant hits from a scop families file.
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
  SEQNR DOCUMENTATION
                        ----------------------

* Modify ACD to take i) Escop.dat ii) directory of .align files.doc1
* Modify to take hits AND validation file.
* Go through each family in the hits file in turn:

  -> Open the corresponding .align file.
  -> Extract SCOP domain codes (scopalg->Id)
  -> Query Escop.dat & retrieve swissprot sequences (+Start/End/Acc)
     corresponding to the domains 
  -> Mark up for removal i) The domain sequences
     ii) All other sequences in the hits file (for family) that show 
     significant overlap to any of the domain sequences.
  -> Calculate redundancy (some will be removed).
  -> Garbage collection (remove all domain sequences).
  -> Write new hits file.

* In the validation file:
  -> Remove from the validation file all targetted sequences.
  -> Add domain sequences back into the validation file.
  -> Mark up as "SEED" in validation file i) all hits in new his file
     ii) all domain sequences.
 */


#include "emboss.h"

static AjBool seqnr_GetSeqsFromScop(AjPScopalg align, AjPFile scop_inf, 
                                   AjPList* list);
static AjBool seqnr_ProcValidFile(AjPFile validf, AjPList domains, 
                                 AjPList hits, AjPStr famname, AjPFile voutf);



/* @prog seqnr **************************************************************
**
** Test 
**
******************************************************************************/
int main(int argc, char **argv)
{
    float gapopen;                 /* Gap insertion penalty */
    float gapextend;               /* Gap extension penalty */
    float thresh;                  /* Threshold for non-redundancy */
    AjPMatrixf matrix;             /* Substitution matrix */
    
    AjPStr tmp           = NULL;   /* temparary string */
    AjPStr logf          = NULL;   /* log file pointer */

    AjPList seqlist      = NULL;   /* a list of sequences to be processed */
    AjPList scopseqlist  = NULL;   /* list of scop sequence seq objects */
    AjPList hits         = NULL;   /* a list of scophits from each hitlist iteration that has not been processed */
    AjPList proslist     = NULL;   /* processed hits from which the redundancy has been removed */
    AjPList hitlistin    = NULL;   /* list of hitlists containing hits that are unprocessed */
    
    AjPHitlist tmphitlist= NULL;   /* temparary hitlist structure */
    AjPHitlist hitlistout= NULL;   /* hitlist containing processed hits */
    
    AjPFile inf          = NULL;   /* file containing the unprocessed hits */
    AjPFile validf       = NULL;   /* file pointer for the validation file */
    AjPFile scop_inf     = NULL;   /* file pointer for Escop.dat */
    AjPFile aln_inf      = NULL;   /* file pointer to alignment file */
    AjPFile outf         = NULL;   /* the outfile for the processed families */
    AjPFile voutf        = NULL;   /* the validation outfile */

    AjPStr filename      = NULL;   /* name of the input file */
    AjPStr alignextn     = NULL;   /* extension for seed alignment files */
    AjPStr outfilename   = NULL;   /* the name of the output file containing the processed hits */
    AjPStr alignpath     = NULL;   /* path to the seed alignment files */
    AjPStr aln_filename  = NULL;   /* name of alignment file */
    AjPStr fam_name      = NULL;   /* name of the family */

    AjPSeq seq           = NULL;   /* A sequence object to hold the constructed sequence */
    
    AjIList iter         = NULL;   /* A list iterator */
    
    AjPScophit hit       = NULL;   /* temperary scophit */
    AjPScophit domhit    = NULL;   /* recently converted scoptoscophit object of a domain */



    AjPScop  scop        = NULL;   /* a scop object */

    AjPScopalg align     = NULL;   /* scopalg object to hold the seed alignment */
   
    AjPInt keep          = NULL;   /* 1: This sequence was kept after redundancy removal,
                                      0: it was discarded */
    ajint nsetnr         = 0;      /* No. proteins in the non-redundant set */
    ajint i              = 0;      /* loop counter */


    /* Read data from acd */
    embInit("seqnr",argc,argv);
    
    inf       = ajAcdGetInfile("inf");
    outf       = ajAcdGetOutfile("outf");
    validf    = ajAcdGetInfile("validf");
    voutf     = ajAcdGetOutfile("voutf");
    scop_inf  = ajAcdGetInfile("scopin");
    alignpath = ajAcdGetString("alignpath");
    alignextn = ajAcdGetString("alignextn");
    thresh    = ajAcdGetFloat("thresh");
    matrix    = ajAcdGetMatrixf("datafile");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    
    outfilename  = ajStrNew();
    aln_filename = ajStrNew();
    tmp          = ajStrNew();
    logf         = ajStrNew();
    fam_name     = ajStrNew();
    
    scop     = ajXyzScopNew(1);
    
    /* Check directories */
    if((!inf))
        ajFatal("Could not open Hitsfile\n");    
    
    
    filename =  ajStrNewC(ajFileName(inf));


    
    /* read in each entry delimited by a "//" */
    while(ajXyzHitlistRead(inf,"//",&tmphitlist)) 
    {
        keep       = ajIntNew();
        hitlistin  = ajListNew();
        hits       = ajListNew();
        seqlist    = ajListNew();
        proslist   = ajListNew();
        
/*        ajStrAssS(&aln_filename,tmphitlist->Family);
	  ajStrAssS(&fam_name,aln_filename);
	  ajStrToLower(&aln_filename);
	  ajStrApp(&aln_filename,alignextn);*/

	/* New code for using SCOP Sunid's for names of input files */
	ajStrAssS(&fam_name, tmphitlist->Family);
	ajStrFromInt(&aln_filename, tmphitlist->Sunid_Family);
	ajStrApp(&aln_filename,alignextn);

        /* open corresponding "seed" alignment file. If seed alignment is absent no point in continuing */
        if(!(aln_inf=ajFileNewDF(alignpath,aln_filename)))
        {
            ajWarn("The alignment file %S%S could not be found\n",alignpath,aln_filename);
            continue;
        }

        /* read seed alignment into scopalg structure */
        ajXyzScopalgRead(aln_inf,&align);

        /* get scop sequences from Escop.dat */
        scopseqlist = ajListNew();
        
        if(!seqnr_GetSeqsFromScop(align,scop_inf,&scopseqlist))
	    ajFatal("Domain not found in scop classification file");
	

        /* Create list of a single Hitlist from tmphitlist */
        ajListPushApp(hitlistin,tmphitlist);

        /* Convert this hitlist to a list of Scophit objects */
        ajXyzHitlistToScophits(hitlistin,&hits);

        /* Add the domain sequences to this list */
        while(ajListPop(scopseqlist,(void **)&scop))
        {
            domhit = ajXyzScophitNew(); /* allocate some space, need to free this up later*/
            ajXyzScopToScophit(scop,&domhit);
            ajXyzScophitTarget(&domhit);
            ajListPushApp(hits,domhit);
        }

        /* Delete original Hitlist and the derived list
           We now just have a list of Scophit's called <hits> */
        ajXyzHitlistDel(&tmphitlist);
        ajListDel(&hitlistin);

        /* sort the list and target identical or overlapping hits*/
        ajListSort(hits,ajXyzScophitCompAcc);
        
        /* No longer check for overlapping hits because accession number
	   and start/end point not available for all sequences */
	/*
        iter = ajListIter(hits);
        hit1 = (AjPScophit)ajListIterNext(iter);
	
        target hits sequences that are "the same" as the "seed" sequences  
        while((hit2=(AjPScophit)ajListIterNext(iter)))
        { 
	any overlaps arise only form seed proteins, rest have been processed by seqsort 
            if(ajXyzScophitsOverlapAcc(hit1,hit2,10))
            {
	    target both hits 
                ajXyzScophitTarget(&hit1);
                ajXyzScophitTarget(&hit2);
            }   
            else
	     move one node along 
                hit1 = hit2;
        }
                    
        ajListIterFree(iter);
        */

        iter = ajListIter(hits);        
        while((hit=(AjPScophit)ajListIterNext(iter)))
        {
            seq = ajSeqNew();
            ajStrAss(&seq->Name,hit->Id);
            ajStrAss(&seq->Seq,hit->Seq);
            ajListPushApp(seqlist,seq);
        }
        ajListIterFree(iter);
        
        /*remove the redundancy from the sequence set */
        embXyzSeqsetNR(seqlist, &keep, &nsetnr, matrix, gapopen, gapextend, thresh);

        /* create a list of processed hits */
        for(iter = ajListIter(hits), i = 0;(hit = (AjPScophit)ajListIterNext(iter));i++)
        {
            if(ajIntGet(keep,i))
                ajListPushApp(proslist,hit);
            else
                ajXyzScophitDel(&hit);
            }   
        ajListDel(&hits);
        ajListIterFree(iter);
        iter = NULL;

        /* remove targeted hits */
        ajListGarbageCollect(proslist,ajXyzScophitDelWrap,(const void*)ajXyzScophitCheckTarget);

        /* process the validation file */
        seqnr_ProcValidFile(validf,scopseqlist,proslist,fam_name,voutf);
        ajFileClose(&voutf);

        /* write a hitlist of the processed scophits */
        ajXyzScophitsToHitlist(proslist,&hitlistout,&iter);

        /* delete and clean up proslist  */
        iter=ajListIter(proslist);
        while((hit=(AjPScophit)ajListIterNext(iter)))
            ajXyzScophitDel(&hit);
        ajListIterFree(iter);
        ajListDel(&proslist);

        /* write the processed hitlist in a file in EMBL format */
        ajXyzHitlistWrite(outf,hitlistout);
        ajXyzHitlistDel(&hitlistout);
        hitlistout = NULL;

        /*delete and clean up seqlist */
        iter=ajListIter(seqlist);
        while((seq=(AjPSeq)ajListIterNext(iter)))
            ajSeqDel(&seq);
        ajListIterFree(iter);
        ajListDel(&seqlist);

        /* clean up */
        ajIntDel(&keep);
        ajXyzScopalgDel(&align);
        ajFileClose(&aln_inf);
        ajListDel(&scopseqlist);
        
    }
    
    /* clean up */
    ajMatrixfDel(&matrix);
    ajStrDel(&alignpath);
    ajStrDel(&alignextn);
    ajStrDel(&tmp);
    ajStrDel(&filename);
    ajStrDel(&outfilename);
    ajStrDel(&aln_filename);
    ajStrDel(&logf);
    ajStrDel(&fam_name);
    ajFileClose(&scop_inf);
    ajXyzScopDel(&scop);
    ajFileClose(&inf);
    ajFileClose(&validf);
    ajFileClose(&outf);

    ajExit();
    return 0;

}

/* @funcstatic seqnr_GetSeqsFromScop *************************************
**
** Takes a Scopalg object (scop alignment) and a pointer to the Escop database. 
** Extracts the pdb domain codes from the alignment and compiles a list of 
** corresponding sequence objects from the scop database (EScop.dat). 
**
** @param [r] align     [AjPScopalgn]   Contains a seed alignment.
** @param [r] scop_inf  [AjPFile]       Points to Ecop.dat.
** @param [w] list      [AjPList*]      List of Seq objects.
** 
** @return [AjBool] A populated list has been returned (a file has been written)
** @@
********************************************************************************/
static AjBool seqnr_GetSeqsFromScop(AjPScopalg align, AjPFile scop_inf, 
                                   AjPList* list)
{

    AjPScop  scop     = NULL;  /* scop object to hold a scop entry */
    AjPScop  *entry   = NULL;  /* Array of Scop objects */
    AjPStr   entry_up = NULL;  /* Current entry, upper case */
    
    AjPList  tmp_list = NULL;  /* list to hold entire scop */
    
    ajint  dim        = 0;     /* size of array */
    ajint  idx        = 0;     /* Index into array for the Pdb code */
    
    ajint  i          = 0;     /* Simple loop counter */

    AjIList iter      = NULL;

    tmp_list  = ajListNew();
    entry_up  = ajStrNew();
    

    /* check for bad arguments */
    if(!align || !scop_inf)
    {
        ajWarn("Bad args passed to seqnr_GetSeqsFromScop");
        return ajFalse;
    }

    /* create a list of all the scop entries in Escop.dat */
    while((ajXyzScopReadC(scop_inf,"*",&scop)))
        ajListPushApp(tmp_list,scop);

    /* Order the list of Scop objects by Domain Id and create an array */
    ajListSort(tmp_list, ajXyzScopCompId);
    dim = ajListToArray(tmp_list,(void ***)&entry);
    if(!dim)
    {
        ajWarn("Empty list in seqnr_GetSeqsFromScop\n");
        return ajFalse;
    }

    /* write to list the scop structures matching a particular family of domains */
    for(i=0;i<align->N;i++)
    {
	ajStrAssS(&entry_up, align->Codes[i]);
	ajStrToUpper(&entry_up);
	
	ajFmtPrint("Trying to find %S\n", entry_up);
	
	
        if((idx = ajXyzScopBinSearch(entry_up,entry,dim))==-1)
            return ajFalse;
        else
            ajListPushApp(*list,entry[i]);
    }
    
    /* clean up */
    iter=ajListIter(tmp_list);
    while((scop=(AjPScop)ajListIterNext(iter)))
        ajXyzScopDel(&scop);
    ajListIterFree(iter);
    ajListDel(&tmp_list);
    ajStrDel(&entry_up);
    

    return ajTrue;
}





/* @funcstatic seqnr_ProcValidFile ***********************************************
**
** Remove from validation file all targetted sequences. Add domain sequences
** back into the validation file. Mark up as "SEED" all hit and domain
** sequences.
**
** @param [r] validf  [AjPFile]   The Validation file
** @param [r] domains [AjPList]   A list of domains
** @param [r] hits    [AjPList]   A list of hits
** @param [r] famname [AjPStr ]   The name of the domain family in question
** @param [w] voutf   [AjFile ]   The new validation file
**
** @return [ajTrue] on completion. 
** @@
******************************************************************************/
static AjBool seqnr_ProcValidFile(AjPFile validf, AjPList domains, 
                                 AjPList hits, AjPStr famname, AjPFile voutf)
{
    AjPHitlist tmphitlist  = NULL;      /* a temparary hit list */
    AjPHitlist apphitlist  = NULL;      /* a hitlist contain the domain sequences */
    
    AjPList  hitlists      = NULL;      /* a list of hitlists */
    AjPList  vhits         = NULL;      /* a list of validation hits */

    AjPScophit domhit      = NULL;
    AjPScophit hit1        = NULL;
    AjPScophit hit2        = NULL;
  
    AjPScop  scop          = NULL;

    AjIList iter1          = NULL;      /* A list iterator */
    AjIList iter2          = NULL;      /* Another list iterator */

    ajint nodes            = 0;         /* number of nodes in vhits */

    hitlists  = ajListNew();
    vhits     = ajListNew();

    if(!validf || !domains || !hits)
    {
        ajWarn("Bad args passed to seqnr_ProcValidFile\n");
        return ajFalse;
    }
    

    /* read the validation file */
    while(ajXyzHitlistRead(validf,"//",&tmphitlist))
    {   
        /* add the domains back into the validation file */
        if(ajStrMatch(famname,tmphitlist->Family))
        {
            /* there is only ever going to be one hitlist in this list */
            ajListPushApp(hitlists,tmphitlist);
            ajXyzHitlistToScophits(hitlists,&vhits);
        
            while(ajListPop(domains,(void **)&scop))
            {
                domhit = ajXyzScophitNew();     
                ajXyzScopToScophit(scop,&domhit);
                ajStrAssC(&domhit->Typeobj,"SEED");
                ajListPushApp(vhits,domhit);
            }
        
            /* sort the list and target identical or overlapping hits*/
            ajListSort(vhits,ajXyzScophitCompAcc);
        
            /* check for overlaps */
            iter1 = ajListIter(vhits);
            hit1  = (AjPScophit)ajListIterNext(iter1);
        
            /* target hits sequences that are "the same" as the "seed" sequences */ 
            while((hit2=(AjPScophit)ajListIterNext(iter1)))
            {
                /* any overlaps arise only form seed proteins, rest have been processed by seqsort */
                if(ajXyzScophitsOverlapAcc(hit1,hit2,10))
                {
                    /*target both hits */
                    ajXyzScophitTarget(&hit1);
                    ajXyzScophitTarget(&hit2);
                }       
                else
                    /* move one node along */
                    hit1 = hit2;
            }   
            ajListIterFree(iter1);
           
            /* remove targeted hits */
            ajListGarbageCollect(vhits,ajXyzScophitDelWrap,(const void*)ajXyzScophitCheckTarget);

            /* mark up hits as "seed" */
            iter1 = ajListIter(vhits);
            while((hit1=(AjPScophit)ajListIterNext(iter1)))
            {
                iter2 = ajListIter(hits);
                while((hit2=(AjPScophit)ajListIterNext(iter2)))
                { 

                    if(ajStrMatch(hit1->Id,hit2->Id))
                        ajStrAssC(&hit1->Typeobj,"SEED");
                    else
                        continue;
                }
                ajListIterFree(iter2);
            }
            ajListIterFree(iter1);

            nodes = ajListLength(vhits);
            apphitlist = ajXyzHitlistNew(nodes);
            iter1 = NULL;
            ajXyzScophitsToHitlist(vhits,&apphitlist,&iter1);
            iter1 = NULL;

            ajXyzHitlistWrite(voutf,apphitlist);
        }
        else
            ajXyzHitlistWrite(voutf,tmphitlist); 
    }

    /*delete and clean up vhits */
    iter1=ajListIter(vhits);
    while((hit1=(AjPScophit)ajListIterNext(iter1)))
        ajXyzScophitDel(&hit1);
    ajListIterFree(iter1);
    ajListDel(&vhits);

    /* clean up */
    ajListDel(&hitlists);
    ajXyzHitlistDel(&apphitlist);
    
    return ajTrue;
}



