/* @source siggen application
**
** Generates a sparse protein signature from an alignment and residue contact 
** data.
** 
** @author: Copyright (C) Matt Blades 
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
*****************************************************************************
** 
**  SIGGEN documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Automatic generation and evaluation of sparse protein signatures for 
**  families of protein structural domains. MJ Blades, JC Ison, R Ranasinghe
**  and JBC Findlay. Protein Science. 2005 (accepted)
**  
**  Email Jon Ison (jison@ebi.ac.uk)
**  
****************************************************************************/






#include <math.h>

#include "emboss.h"




/******************************************************************************
**
** STRUCTURE DEFINITIONS
**
******************************************************************************/

/* @data AjPScorealg *******************************************************
**
** Ajax Scorealg object.
**
** Holds scores associated with the 5 scoring methods used in siggen
**
** AjPScorealg is implemented as a pointer to a C data structure.
**
** @attr  seqmat_score [AjPFloat]  Array of scores - residue convervation 
** @attr seqvar_score [AjPFloat]   Array of scores - residue variability 
** @attr post_similar [AjPUint]    Array of scores - stamp pij value      
** @attr positions [AjPUint]       Array of integers from 'Position' line in 
**  alignment, used for manual specification of signature positions.
** @attr ncon_score  [AjPFloat]    Array of scores based on number of contacts
** @attr ccon_score [AjPFloat]     Array of scores based on convervation of 
** 			           contacts. 
** @attr nccon_score [AjPUint]     Array of total score based on convervation 
				   and number of contacts.
** @attr combi_score  [AjPUint]    Array of total score based on users scoring 
				   criteria.
** @attr ncon_thresh [AjPUint]     Array of positions with > threshold number 
				   of contacts 
** @attr seqmat_do  [AjBool]       Whether to use score based on residue 
**                                 convervation.
** @attr seqvar_do  [AjBool]       Whether to use score based on residue 
**				   variablility. 
** @attr filterpsim  [AjBool]      Whether to filter on basis of post_similar 
** 				   line.      
** @attr filtercon [AjBool]        Whether to filter on basis of number of 
** 				   contacts.      
** @attr conthresh [ajint]         Threshold number of contacts for filtercon. 
** @attr ncon_do  [AjBool]         Whether to use score based on number of 
** 				   contacts.
** @attr  ccon_do [AjBool]         Whether to use score based on convervation 
**                                 of contacts.
** @attr   nccon_do [AjBool]       Whether to use score based on convervation 
**     			           and number of contacts.
** @attr random [AjBool]           Whether to generate a randomised signature. 
** @attr manual [AjBool]           Whether signature positions were taken from 
**                                 alignment file (manual selection).
**
** @alias AjSScorealg
** @alias AjOScorealg
**
** @@
******************************************************************************/
typedef struct AjSScorealg
{   
    AjPFloat  seqmat_score;
    AjPFloat  seqvar_score;
    AjPUint    post_similar;
    AjPUint    positions;
    AjPFloat  ncon_score;
    AjPFloat  ccon_score;
    AjPUint    nccon_score;
    AjPUint    combi_score;
    AjPUint    ncon_thresh;
    AjBool    seqmat_do;
    AjBool    seqvar_do;
    AjBool    filterpsim;
    AjBool    filtercon;
    ajint     conthresh;
    AjBool    ncon_do;  
    AjBool    ccon_do;
    AjBool    nccon_do;
    AjBool    random;
    AjBool    manual;
} AjOScorealg, *AjPScorealg;





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjPScorealg  siggen_ScorealgNew(ajint len);

static void  siggen_ScorealgDel(AjPScorealg *pthis);

static AjBool  siggen_ScoreSeqMat(AjPScopalg alg,
				  AjPScorealg *scores, 
				  AjPMatrixf mat, 
				  AjPUint2d seq_pos);

static AjBool  siggen_ScoreSeqVar(AjPScopalg alg, 
				  AjPScorealg *scores, 
				  AjPUint2d seq_pos);

static AjBool  siggen_ScoreNcon(AjPScopalg alg, 
				AjPScorealg *scores, 
				AjPCmap *cmaps, 
				AjPUint2d seq_pos, 
				AjPUint *atom_idx, 	
				AjBool *noca);

static AjBool  siggen_ScoreCcon(AjPScopalg alg, 
				AjPScorealg *scores, 
				AjPCmap *cmaps, 
				AjPUint2d seq_pos, 	
				AjPUint *atom_idx, 
				AjBool *noca);

static EmbPSignature  siggen_SigSelectSeq(AjPScopalg alg, 
					 AjPScorealg scores, 
					 AjPUint2d seq_pos, 
					 ajint sig_sparse,
					 float *spar_check, 
					 ajint wsiz);

static EmbPSignature  siggen_SigSelect(AjPScopalg alg, 
				      AjPScorealg scores, 
				      AjPUint2d seq_pos, 
				      ajint sig_sparse, 
				      AjPCmap *cmaps,
				      AjPUint *atom_idx, 
				      float *spar_check, 
				      ajint wsiz);

static AjBool siggen_CalcSeqpos(AjPScopalg alg, 
				AjPUint2d *seq_pos);

static AjBool siggen_ScoreAlignment(AjPScorealg *scores, 
				    AjPScopalg alg, 
				    AjPCmap *cmaps, 
				    AjPMatrixf  mat, 
				    AjBool *noca, 
				    AjPUint2d seq_pos, 
				    AjPUint *atom_idx);

static AjBool siggen_ScoreAlignmentSeq(AjPScorealg *scores, 
				       AjPScopalg alg, 
				       AjPMatrixf  mat, 
				       AjPUint2d seq_pos);

static EmbPSignature siggen_SigSelectManual(AjPScopalg alg, 
					   AjPScorealg *scores, 
					   AjPUint2d seq_pos, 
					   float *spar_check,
					   ajint wsiz);

static AjBool siggen_Con_Thresh(AjPScopalg alg, AjPScorealg *scores,
				AjPCmap *cmaps, ajint conthresh,
				AjBool *noca, AjPUint2d seq_pos, 
				AjPUint *atom_idx);








/* @prog siggen ***************************************************************
**
** Generates a sparse protein signature from an alignment and residue 
** contact data.
**
******************************************************************************/

int main(ajint argc, char **argv)
{
    /* Variables for stuff in acd. */
    AjPList     alg_path      =NULL;  /* Location of alignment files -input.*/
    AjPStr      alg_name      =NULL;  /* Name of alignment file. */
    AjPDir      cpdb_path     =NULL;  /* Location of coordinate files -input.*/
    AjPStr      cpdb_name     =NULL;  /* Name of coordinate file. */
    AjPDir      con_path      =NULL;  /* Location of contact files -input. */
    AjPStr      con_name      =NULL;  /* Name of contact file. */
    AjPDirout   sig_path      =NULL;  /* Location of signature files -output */
    AjPStr      sig_name      =NULL;  /* Name of signature files. */
    AjPStr      sig_name_sp   =NULL;  /* Sparsity extn for signature file. */
    AjPStr      pair_mat      =NULL;  /* Residue pair substitution matrix. */
    AjPStr      temp          =NULL;  /* Temp string. */
    AjPStr      temp1         =NULL;  /* Temp string. */
    AjPStr      temp2         =NULL;  /* Temp string. */
    AjPStr      temp3         =NULL;  /* Temp string. */
    

    AjPFile     fptr_alg      =NULL;   /* Pointer to alignment file. */
    AjPFile     fptr_con      =NULL;   /* Pointer to current contact file. */
    AjPFile     fptr_cpdb     =NULL;   /* Pointer to current coordinate file.*/
    AjPFile     sig_outf      =NULL;   /* File pointer for output file. */

    AjPList     list          =NULL;   /* List of files in align directory. */
    AjPMatrixf  mat           =NULL;
    AjPUint2d    seq_pos       =NULL;   /* Numbering of sequence according to 
					  alignment. */


    EmbPSignature sig=NULL;            /* Signature. */
    AjPScopalg  alg           =NULL;   /* Pointer to Scopalg structure. */
    AjPScorealg scores        =NULL;   /* Pointer to Scorealg structure. */
    AjPCmap    *cmaps         =NULL;   /* Array of pointers to Cmap structs */

    AjBool      score_seq_mat =ajFalse; /* Score by residue
					   conservation (Y/N). */
    AjBool      score_seq_var =ajFalse; /* Score by variability
					   function (Y/N). */
    AjBool      score_ncon    =ajFalse; /* Score by number of contacts
					   (Y/N). */
    AjBool      score_ccon    =ajFalse; /* Score by conservation of contacts  
					   (Y/N). */

    AjBool      score_both    =ajFalse; /* Score by combined measure of number 
					   and conservation (Y/N) ( not 
					   implemented at moment). */ 
    AjBool      filterpsim    =ajFalse; /* Filter on basis of post_similar data
					   line (Y/N). */ 
    AjBool      filtercon     =ajFalse; /* Filter on basis of number
					   of contacts (Y/N). */
    ajint       conthresh     =0;       /* Threshold number of contacts for
					   filtercon. */
    AjPStr      *mode         =NULL;    /* Holds mode options from acd*/
    AjPStr      *seqoption    =NULL;    /* Holds sequence scoring
					   options from acd*/
    AjPStr      *conoption    =NULL;    /* Holds contact scoring
					   options from acd*/

    char        id            ='.';     /* Chain identifier for a scop domain*/
    ajint       idn           =0;       /* Chain identifier as a number. */
    ajint       x             =0;       /* Loop counter. */

    ajint       sig_sparse    =0;       /* Sparsity of signature. */
    ajint       wsiz          =0;       /* Window size. */
    AjBool      idok          =ajFalse; /* Whether chain identifier could be 
					   determined ok. */
    AjPPdb      pdb           =NULL;    /* Pdb object pointer*/

    AjBool      *noca          =NULL;    /* Array whose elements are True if 
					    the relevant sequence in 
					    the alignment contained an NOCA 
					    grouyp in the original pdb
					    file. */
    AjPUint     *atom_idx=NULL;          /* Indices into the full
					   length sequences for
					   sequences in the alignment
					   (alignment sequences are
					   for structured residues
					   (residues with electron
					   density) only). */
    AjPStr      tempres       =NULL;


    float       spar_check    =0.0;
    
    ajint       nres =0;                /* number of structured residues for 
					   current sequence in alignment. */
             
    

    

    /* Allocate strings etc. */
    sig_name      = ajStrNew();
    sig_name_sp   = ajStrNew();
    alg_name      = ajStrNew();
    con_name      = ajStrNew();
    cpdb_name     = ajStrNew();
    pair_mat      = ajStrNew();
    temp          = ajStrNew();
    temp1         = ajStrNew();
    temp2         = ajStrNew();
    temp3         = ajStrNew();
    tempres       = ajStrNew();



    /* Read data from acd. */
    embInitPV("siggen",argc,argv,"SIGNATURE",VERSION);

    sig_path      = ajAcdGetOutdir("sigoutdir");
    alg_path      = ajAcdGetDirlist("algpath");
    sig_sparse    = ajAcdGetInt("sparsity");
    wsiz          = ajAcdGetInt("wsiz");
    mode          = ajAcdGetList("mode");
    seqoption     = ajAcdGetList("seqoption");
    mat           = ajAcdGetMatrixf("datafile");
    conoption     = ajAcdGetList("conoption");
    filtercon     = ajAcdGetToggle("filtercon");
    conthresh     = ajAcdGetInt("conthresh");
    con_path      = ajAcdGetDirectory("conpath");    
    cpdb_path     = ajAcdGetDirectory("cpdbpath");    
    filterpsim    = ajAcdGetBoolean("filterpsim");
    



    /* If random is chosen then just use one scoring method to go through, 
     but signature *will* be random. */
    if((ajStrGetCharFirst(*mode) == '3'))
	score_seq_var = ajTrue;
    
    
    
    if(ajStrGetCharFirst(*seqoption) == '1')
        score_seq_mat = ajTrue;
    else if(ajStrGetCharFirst(*seqoption) == '2')
        score_seq_var = ajTrue;
    /* Both methods are left as ajFalse if user selects option 3, 
       i.e. no sequence scoring. */


    if(ajStrGetCharFirst(*conoption) == '1')
        score_ncon=ajTrue;
    else if(ajStrGetCharFirst(*conoption) == '2')
        score_ccon=ajTrue;
    else if(ajStrGetCharFirst(*conoption) == '3')
    {
        score_ccon=ajTrue;
        score_ncon=ajTrue;
    }
    



    /* Check if a scoring method has been selected. */
    if((score_seq_mat == ajFalse) && (score_seq_var == ajFalse) && 
       (score_ncon == ajFalse) && (score_ccon == ajFalse))
    {
        ajFmtPrint("No scoring methods were selected from acd. Exiting\n");

        ajExit();

        return(0);
    }   


    /*Start of main application loop*/
    while(ajListPop(alg_path,(void **)&temp))
    {
      /*      ajFmtPrint("1\n");fflush(stdout); */

        /* Open alignment file. */
        if((fptr_alg=ajFileNewInNameS(temp))==NULL)
        {
            ajFileClose(&fptr_alg);
            ajFatal("Could not open alignment file");
        }


	/*      ajFmtPrint("2\n");fflush(stdout); */

        /* Read alignment file, write Scopalg structure,
	   close alignment file.*/
        ajDmxScopalgRead(fptr_alg, &alg);


	/* For non-STAMP alignments there will not be no Post_similar line 
	   present, however, the siggen algorithms depend on there being one.
	   Therefore write one here in which the values are all '1' (i.e. no 
	   positions will be included because of the
	   Post_similar assignment.) */
	if(!MAJSTRGETLEN(alg->Post_similar))
	    for(x=0;x<alg->width;x++)
		ajStrAppendK(&alg->Post_similar, '1');
	

	/*      ajFmtPrint("3\n");fflush(stdout); */

	if(alg->N==0)
	{
            ajFileClose(&fptr_alg);
            ajWarn("Alignment file does not contain sequences");
            ajStrDel(&temp);
	    ajDmxScopalgDel(&alg);
	    continue;       
	}
	
	/*      ajFmtPrint("4\n");fflush(stdout); */

        ajFileClose(&fptr_alg);
        

        /* Allocate array of pointers to Cmap structures for contact maps. */
	if(ajStrGetCharFirst(*conoption) != '5')
	    AJCNEW0(cmaps, alg->N);
        
	/*      ajFmtPrint("5\n");fflush(stdout); */


        /* Allocate array of bool's for noca array. */
	if(ajStrGetCharFirst(*conoption) != '5')
	    AJCNEW0(noca, alg->N);
        

	/*      ajFmtPrint("6\n");fflush(stdout); */

        /* Allocate array of AjPUint for indeces into sequences. */
	if(ajStrGetCharFirst(*conoption) != '5')
	{
	    AJCNEW0(atom_idx, alg->N);
	    
	    for(x=0; x<alg->N; ++x)
		atom_idx[x] = ajUintNew();
        }
	

	/*      ajFmtPrint("7\n");fflush(stdout); */

        /* Start of loop for reading contact data and coordinate files. 
           Only entered if contact data is to be used. */

	if(ajStrGetCharFirst(*conoption) != '5')
	{
            for(x=0; x<alg->N; ++x)

            {
                idok=ajFalse;
                
                /* Open contact data file. */
                if((fptr_con= ajFileNewListinDirPre(con_path, alg->Codes[x]))==NULL)
                    ajFatal("Could not open contact file!!");
                
                
                /* Read contact data file; hard-coded to read model 1
                   from file. */
                /* A scop identifier is presumed if the id is 7 characters 
                   long and the first character is a 'd' or 'D'. */
                if((ajStrGetLen(alg->Codes[x])==7)
                   &&(toupper((ajint) ajStrGetCharFirst(alg->Codes[x])) ==
		      (ajint) 'D'))

                { 
                    /*Read the chain id from the SCOP domain code and convert 
                      cases of '_' identifiers to '.' */
                    id=ajStrGetCharPos(alg->Codes[x], 5);
                    if(id=='_') id='.';
                    
                    cmaps[x] = ajCmapReadCNew(fptr_con, id, 1);
		    

                    idok=ajTrue;
                }
                else  
                {
                    /*Not sure of a chain identifier so read the first chain.*/
                    ajWarn("Uncertain of chain identifier so reading "
			   "first chain");
                    cmaps[x] = ajCmapReadINew(fptr_con, 1,1);
                }
                
		/*      ajFmtPrint("8\n");fflush(stdout); */

                
                /* Close contact data file. */
                ajFileClose(&fptr_con);

                
                /* Open coordinate file. */
                if((fptr_cpdb=ajFileNewListinDirPre(cpdb_path, alg->Codes[x]))==NULL)
                    ajFatal("Could not open coordinate file");

                /* Read coordinate data file. */ 
                pdb = ajPdbReadFirstModelNew(fptr_cpdb);
                

		/*      ajFmtPrint("9\n");fflush(stdout);*/

                /* Determine the chain number. */
                if(idok)
                {
                    if(!ajPdbChnidToNum(id, pdb, &idn))
                    {
                        idn=1;
                        ajWarn("Could not find chain in siggen x = %d. "
			       "Assuming chain 1\n", x);
                    }
                }
                else  
                    /* Not sure of a chain identifier so read the
                       first chain. */
                    idn=1;


		/*      ajFmtPrint("10\n");fflush(stdout); */

                /* To keep following condition tests happy. */
                noca[x]=ajFalse;


                if(!embPdbResidueIndexICA(pdb, idn, &atom_idx[x], &nres))
                {
                    ajWarn("Could not find chain in siggen "
			   "embPdbResidueIndexICA\n");
                    ajPdbDel(&pdb);
                    ajFileClose(&fptr_cpdb);
                    continue;
                }
                
		ajUintPut(&atom_idx[x], nres, -1);
		
    
                ajPdbDel(&pdb);
                ajFileClose(&fptr_cpdb);

		/*      ajFmtPrint("11\n");fflush(stdout); */
            }
	}
	

	/*      ajFmtPrint("12\n");fflush(stdout); */

        /* Allocate Scorealg structure and write values from acd. */
        scores = siggen_ScorealgNew((ajint)alg->width);
        scores->seqmat_do    = score_seq_mat;
        scores->seqvar_do    = score_seq_var;
        scores->filtercon    = filtercon;
        scores->filterpsim   = filterpsim;
        scores->ncon_do      = score_ncon;
        scores->ccon_do      = score_ccon;
        scores->nccon_do     = score_both;
	if((ajStrGetCharFirst(*mode) == '3'))
	    scores->random   = ajTrue;
	if((ajStrGetCharFirst(*mode) == '1'))
	    scores->manual   = ajTrue;



	/*      ajFmtPrint("13\n");fflush(stdout); */

        /* Calculate index for use by scoring functions. */
        siggen_CalcSeqpos(alg, &seq_pos);


        /* Determine positions with > conthresh no. of contacts. */
	if(filtercon == ajTrue)
	    siggen_Con_Thresh(alg, 
			      &scores, 
			      cmaps, 
			      conthresh, 
			      noca, 
			      seq_pos, 
			      atom_idx);        


	/*      ajFmtPrint("14\n");fflush(stdout); */

        /* Score alignement - write Scorealg structure and generate
           signature. */
	if(ajStrGetCharFirst(*mode) == '1')
	{
	    sig = siggen_SigSelectManual(alg, 
					 &scores, 
					 seq_pos, 
					 &spar_check, 
					 wsiz);
	}
	else
	{
	    if(ajStrGetCharFirst(*conoption) == '5')
		siggen_ScoreAlignmentSeq(&scores, alg, mat, seq_pos);
	    else
		siggen_ScoreAlignment(&scores, 
				      alg,
				      cmaps,
				      mat,
				      noca,
				      seq_pos,
				      atom_idx);


	    /*      ajFmtPrint("15\n");fflush(stdout); */

	    spar_check = 0.0;

	    if(ajStrGetCharFirst(*conoption) == '5')
	    {
		if( (sig = siggen_SigSelectSeq(alg, scores, seq_pos,
					       sig_sparse, 
					       &spar_check, wsiz))==NULL)
		{
		    if(ajStrGetCharFirst(*conoption) != '5')
			for(x=0; x<alg->N; ++x)
			    ajCmapDel(&cmaps[x]);
		    
		    if(ajStrGetCharFirst(*conoption) != '5')
		    {
			for(x=0; x<alg->N; ++x)
			    ajUintDel(&atom_idx[x]);
			AJFREE(atom_idx);
			AJFREE(noca);
			AJFREE(cmaps);
		    }
		    
		    siggen_ScorealgDel(&scores); 
		    ajDmxScopalgDel(&alg);
		    ajStrDel(&temp);
		    continue;
		}
	    }
	    
	    else 
	    {
		if( (sig = siggen_SigSelect(alg, scores, seq_pos, sig_sparse, 
					    cmaps, atom_idx, &spar_check, 
					    wsiz))==NULL)
		{
		    if(ajStrGetCharFirst(*conoption) != '5')
			for(x=0; x<alg->N; ++x)
			    ajCmapDel(&cmaps[x]);
		    
		    if(ajStrGetCharFirst(*conoption) != '5')
		    {
			for(x=0; x<alg->N; ++x)
			    ajUintDel(&atom_idx[x]);
			AJFREE(atom_idx);
			AJFREE(noca);
			AJFREE(cmaps);
		    }
		    
		    siggen_ScorealgDel(&scores); 
		    ajDmxScopalgDel(&alg);
		    ajStrDel(&temp);
		    continue;
		}
	    }
	}
	

	/*      ajFmtPrint("15\n");fflush(stdout); */

	ajUint2dDel(&seq_pos);    


        

        /* This code block can be used to produce file names which uses the 
	   Sunid only. If this is not available then the input file name is
	   used by default. */
	ajStrAssignS(&sig_name, temp);	
	if(alg)
	    if(alg->Sunid_Family)
	    {
		ajStrFromInt(&temp3, alg->Sunid_Family);
		ajStrAssignS(&sig_name, temp3);	
	    }	
	ajFilenameTrimPathExt(&sig_name);


	/*      ajFmtPrint("16\n");fflush(stdout); */
	       

        /* This code block can be used to produce file names which are the 
	   same as the SCOP family but with instances of ' ' and '&' replaced
	   by '_'. */
	/*        
	   ajStrAssignRef(&sig_name, alg->Family);       
	   ajStrExchangeCC(&sig_name, " ", "_");
	   ajStrExchangeCC(&sig_name, "&", "_");
	   */


        /* This code block can be used to produce file names in 
	   which sparsity and method of generation are indicated. */
	/*
        if(spar_check == 0.0)
            ajStrFromInt(&sig_name_sp, (int)sig_sparse);  
        else
        {
            ajStrFromInt(&sig_name_sp, (int)sig_sparse);            
            ajStrAppendC(&sig_name_sp, "_");
            ajStrFromInt(&temp3, (int)spar_check);          
            ajStrAppendS(&sig_name_sp, temp3);
        }
        
        


	ajStrFromInt(&sig_name, (int)alg->Sunid_Family);            
	ajStrExchangeCC(&sig_name, " ", "_");
        ajStrExchangeCC(&sig_name, "&", "+");
        ajStrAppendC(&sig_name, "_");                   

        if(filtercon == ajTrue)
        {
            ajStrAppendC(&sig_name, "F");             
            ajStrAppendC(&sig_name, "_");             
        }

        if(filterpsim == ajTrue)
        {
            ajStrAppendC(&sig_name, "P");             
            ajStrAppendC(&sig_name, "_");             
        }


        if((score_seq_mat == ajTrue) && 
	(ajStrMatchC(ajMatrixfGetName(mat), "./EBLOSUM62")))
        {
            ajStrAppendC(&sig_name, "sb");             
            ajStrAppendC(&sig_name, "_");             
        }

        if((score_seq_mat == ajTrue) && 
	(ajStrMatchC(ajMatrixfGetName(mat), "./EBC0030")))
        {
            ajStrAppendC(&sig_name, "ss");             
            ajStrAppendC(&sig_name, "_");             
        }

        if(score_seq_var == ajTrue)
        {
            if(scores->random != ajTrue)
            {
                ajStrAppendC(&sig_name, "v");             
                ajStrAppendC(&sig_name, "_");             
            }

            else if(scores->random == ajTrue)
                ajStrAppendC(&sig_name, "rand_");
        }


        if(score_ncon == ajTrue)
        {
            ajStrAppendC(&sig_name, "n");             
            ajStrAppendC(&sig_name, "_");             
        }

        if(score_ccon == ajTrue)

        {
            ajStrAppendC(&sig_name, "c");             
            ajStrAppendC(&sig_name, "_");             
        }
        ajStrAppendS(&sig_name, sig_name_sp);             
	*/


	  /*      ajFmtPrint("18\n");fflush(stdout); */


        /* If a file of that name exists, then append _1 or _2 etc 
           as necessary until a unique name is found. */
	/*
        ajStrAssignRef(&temp1, sig_name);     
        for(x=1;
            (ajFilenameExistsRead(temp1) ||
             ajFilenameExistsWrite(temp1) ||
             ajFilenameExistsExec(temp1));
            x++)
        {
            ajStrAssignRef(&temp1, sig_name); 
            ajStrAppendC(&temp1, "_");
            ajFmtPrintS(&temp2, "%d", x);
            ajStrAppendS(&temp1, temp2);
        }
        ajStrAssignRef(&sig_name, temp1);     
	*/


	if((sig_outf=ajFileNewOutNameDirS(sig_name, sig_path))==NULL)
        {
            ajFatal("Could not open signature file for output");
            continue;       
        }


	/*      ajFmtPrint("19\n");fflush(stdout); */

        /* Write and close signature file. */
	sig->Typesig = aj1D;
        if(!embSignatureWrite(sig_outf, sig))
            ajFatal("Error writing signature file");
        ajFileClose(&sig_outf);

        /* Free memory. */
        embSignatureDel(&sig);

	if(ajStrGetCharFirst(*conoption) != '5')
            for(x=0; x<alg->N; ++x)
                ajCmapDel(&cmaps[x]);

	if(ajStrGetCharFirst(*conoption) != '5')
	{
	    for(x=0; x<alg->N; ++x)
		ajUintDel(&atom_idx[x]);
	    AJFREE(atom_idx);
	    AJFREE(noca);
	    AJFREE(cmaps);
	}
	
	/*      ajFmtPrint("20\n");fflush(stdout);*/

        siggen_ScorealgDel(&scores); 
        ajDmxScopalgDel(&alg);
        ajStrDel(&temp);
    }
    
    
    
    /*      ajFmtPrint("21\n");fflush(stdout);*/


    /* Memory management. */
    ajStrDel(&conoption[0]);
    AJFREE(conoption);
    ajStrDel(&seqoption[0]);
    AJFREE(seqoption);
    ajStrDel(&mode[0]);
    AJFREE(mode);
    ajStrDel(&temp1);
    ajStrDel(&temp2);
    ajStrDel(&temp3);
    ajDiroutDel(&sig_path);
    ajStrDel(&sig_name);
    ajStrDel(&sig_name_sp);
    ajListFree(&alg_path);
    ajStrDel(&alg_name);
    ajDirDel(&con_path);
    ajStrDel(&con_name);
    ajDirDel(&cpdb_path);
    ajStrDel(&cpdb_name);
    ajStrDel(&pair_mat);
    ajStrDel(&tempres);

    ajListFree(&list);
    ajMatrixfDel(&mat);


    /*      ajFmtPrint("22\n");fflush(stdout);*/

    ajExit();
    return 0;
}





/* @funcstatic  siggen_ScoreSeqMat ********************************************
**
** Reada a Scopalg object and writes a Scorealg object. Each residue in 
** the alignment is scored on the basis of conservation of residue type.
**
**
** @param [r] alg     [AjPScopalg]   Alignment
** @param [w] scores  [AjPScorealg*] Scores for alignment
** @param [r] mat     [AjPMatrixf]   Subsitution matrix
** @param [r] seq_pos [AjPUint2d]     Index for alignment
**
** @return [AjBool] True on succcess
** @@
******************************************************************************/

static AjBool  siggen_ScoreSeqMat(AjPScopalg alg, 
				  AjPScorealg *scores, 
				  AjPMatrixf mat, 
				  AjPUint2d seq_pos)
{
    ajint       memb_cnt     =0;  /* Counter for members of the family
				     (alignment). */
    ajint       res_cnt      =0;  /* Counter for residue in the alignment.   */
    ajint       post_cnt     =0;  /* Counter for post_similar line.          */
    float       **sub        =0;  /* Array of floats for sub matrix.         */
    float       val          =0;  /* Current value for res sub score.        */
    float       pos_score    =0;  /* Total sub score for all res at position.*/
    float       temp         =0;  /* Temp variable for score.                */
    AjPSeqCvt   cvt          =0;  /* Sequence character conversion table.    */


    cvt = ajMatrixfGetCvt(mat);    /* Create sequence character
				   conversion table. */
    sub = ajMatrixfGetMatrix(mat);  /* Create matrix as array of floats. */
    

    /* Counter for positions in alignment. */
    for(post_cnt = 0; post_cnt < alg->width; post_cnt++)
    {
        /* Filter on basis of post_similar line. */
        if(((ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') && 
	    ((*scores)->filterpsim == ajTrue) &&
	    ((*scores)->filtercon == ajTrue)  && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
	   ||
           (((*scores)->filterpsim == ajTrue) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') &&
	    ((*scores)->filtercon == ajFalse))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
	    ((*scores)->filtercon == ajTrue) && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
	    ((*scores)->filtercon == ajFalse)))
        {
            /* Initialise variable to zero. */
            val = 0;
            pos_score = 0;
            temp = 0;

            /* Iterate through member of family. */
            for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
            {   
	      if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='X') /* Andy */
                    continue;


                
                /* Iterate throught every combination of residues at the 
		   current position. */
                for(res_cnt = (memb_cnt+1); res_cnt < alg->N; res_cnt++)
                    {
		      if(ajStrGetCharPos(alg->Seqs[res_cnt], post_cnt)=='X') /* Andy */
                            continue;


                        /* Assign score form matrix to variable val. */
                        val = (sub[ajSeqcvtGetCodeK(cvt, (ajStrGetCharPos(alg->Seqs[memb_cnt], 
                            post_cnt)))][ajSeqcvtGetCodeK(cvt, 
                            (ajStrGetCharPos(alg->Seqs[res_cnt], post_cnt)))]);



                        /* Add score form matrix to temp. */
                        temp += val;

                    }
            }
            /* Perform last part of calculation. */
            /* Divide total score by members in family. */
            pos_score = (temp/alg->N);


            /* Assign total score for substitution of all the residues at a 
	       position into scoring array. */
            ajFloatPut(&(*scores)->seqmat_score, post_cnt, pos_score);
        }
    }

 
    return ajTrue;
}





/* @funcstatic  siggen_ScoreSeqVar*********************************************
**
** Reada a Scopalg object and writes a Scorealg object. Each residue in 
** the alignment is scored on the basis of a variability function.
**
**
** @param [r] alg     [AjPScopalg]   Alignment
** @param [w] scores  [AjPScorealg*] Scores for alignment
** @param [r] seq_pos [AjPUint2d]     Index for alignment
**
** @return [AjBool] True on succcess
** @@
*****************************************************************************/
static AjBool  siggen_ScoreSeqVar(AjPScopalg alg, AjPScorealg *scores, 
				  AjPUint2d seq_pos)
{
    ajint       memb_cnt     =0;    /* Counter for members of the family (alignment). */

    ajint       post_cnt     =0;    /* Counter for post_similar line.       */
    ajint       aliphatic    =0;    /* Counter for aliphatic residue group. */    
    ajint       aromatic     =0;    /* Counter for aromatic residue group.  */
    ajint       polar        =0;    /* Counter for polar residue group.     */  
    ajint       basic        =0;    /* Counter for basic residue group.     */  
    ajint       acidic       =0;    /* Counter for acidic residue group.    */
    ajint       special      =0;    /* Counter for special residue group.   */
    ajint       x            =0;    /* Loop counter.                        */
    ajint       total        =0;    /* Total.                               */
    float       val          =0;    /* Current value for res sub score.     */        
    float       temp         =0;    /* Temp value for res sub score.        */        
    float       temp2        =0;    /* Temp value for res sub score.        */        
    float       pos_score    =0;    /* Total sub score for all res at position. */
    AjPFloat    class_freq   =NULL; /* Array for frequencies for each residue group. */




    /* Create the class frequencey array. */
    class_freq = ajFloatNewRes((float)6);
    


    /* Counter for positions in alignment. */
    for(post_cnt = 0; post_cnt < alg->width; post_cnt++)
    {
        /* Filter on basis of post_similar line. */
	/*if(((ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') && 
	  ((*scores)->filterpsim == ajTrue)) || 
	  (((*scores)->filterpsim == ajFalse) && 
	  ((ajStrGetCharPos(alg->Post_similar, post_cnt) == '1')
	  ||  (ajStrGetCharPos(alg->Post_similar, post_cnt) == '0'))))*/

        if(((ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') && 
	    ((*scores)->filterpsim == ajTrue) &&
           ((*scores)->filtercon == ajTrue) && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
	   ||
           (((*scores)->filterpsim == ajTrue) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') &&
	    ((*scores)->filtercon == ajFalse))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
           ((*scores)->filtercon == ajTrue) && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
	    ((*scores)->filtercon == ajFalse)))

        {

            /* Initialise variable to zero. */
            aliphatic    =0;

            aromatic     =0;
            polar        =0;    
            basic        =0;   
            acidic       =0;
            special      =0;
            val          =0;
            pos_score    =0;

            total        =0;
            ajFloatPut(&class_freq, 0, (float) 0.0);
            ajFloatPut(&class_freq, 1, (float) 0.0);
            ajFloatPut(&class_freq, 2, (float) 0.0);
            ajFloatPut(&class_freq, 3, (float) 0.0);
            ajFloatPut(&class_freq, 4, (float) 0.0);
            ajFloatPut(&class_freq, 5, (float) 0.0);

            /* Iterate through member of family. */
            /* Calculate frequency of each residue group. */

            for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
            {   
	      if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='X') /* Andy */
                    continue;
                    
                /* Check if residues are in group 1  */
                /* Then increment aliphatic counter . */
                if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='A')
                    aliphatic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='V')
                    aliphatic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='I')
                    aliphatic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='L')
                    aliphatic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='M')
                    aliphatic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='C')
                    aliphatic++;
                                    
                /* Check if residues are in group 2  */
                /* Then increment aromatic counter.  */
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='F')
                    aromatic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='W')
                    aromatic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='Y')
                    aromatic++;
                


                /* Check if residues are in group 3  */
                /* Then increment polar counter.     */
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='S')
                    polar++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='T')
                    polar++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='N')
                    polar++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='Q')
                    polar++;
                
                /* Check if residues are in group 4  */
                /* Then increment basic counter.     */
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='K')
                    basic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='R')
                    basic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='H')
                    basic++;

                /* Check if residues are in group 5  */
                /* Then increment acidic counter.    */
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='D')
                    acidic++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='E')
                    acidic++;

                /* Check if residues are in group 6  */
                /* Then increment special counter.   */
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='G')
                    special++;
                else if(ajStrGetCharPos(alg->Seqs[memb_cnt], post_cnt)=='P')
                    special++;
            }

            /* Sum all counts of each residue class. */
            total =  (aliphatic + aromatic + polar + basic + acidic + special); 

            /* Check to ensure total residues counted at position is not 
	       greater than number of seqs in set. */
            if(total != alg->N)
            {
                if(total > alg->N)
                {
                    printf("Error in Siggen_ScoreSeqVar.... total"
			   " = greater than number of sequences\n");
                    printf("Exiting....   email jison@ebi.ac.uk\n");
                    ajExit();

                    return 0;
                }

                else
                {
                    /* print warning. */
                    printf("Unknown residue type found in siggen_ScoreSeqVar.... "
			   "total residues counted does not = number of sequences\n");
                    /* Perform frequency calculation divide by total, 
		       NOT alg->N  */
                    ajFloatPut(&class_freq, 0, ((float)aliphatic/(float)total));
                    ajFloatPut(&class_freq, 1, ((float)aromatic/(float)total));
                    ajFloatPut(&class_freq, 2, ((float)polar/(float)total));
                    ajFloatPut(&class_freq, 3, ((float)basic/(float)total));
                    ajFloatPut(&class_freq, 4, ((float)acidic/(float)total));
                    ajFloatPut(&class_freq, 5, ((float)special/(float)total));
                }

            }

            else
            {
                /* Perform frequency calculation. */
                ajFloatPut(&class_freq, 0, ((float)aliphatic/(float)alg->N));
                ajFloatPut(&class_freq, 1, ((float)aromatic/(float)alg->N));
                ajFloatPut(&class_freq, 2, ((float)polar/(float)alg->N));
                ajFloatPut(&class_freq, 3, ((float)basic/(float)alg->N));
                ajFloatPut(&class_freq, 4, ((float)acidic/(float)alg->N));
                ajFloatPut(&class_freq, 5, ((float)special/(float)alg->N));
            }
            
            

            /* Perform calculation of residue variability. */
            for(x=0; x<6; x++)
            {   
                /* Check if frequency = 0, i.e. no residues of */
                /* that class in the position                  */
                if(ajFloatGet(class_freq, x) == 0 )
                    continue;

                
                /* Else Perform calculation. */
                else
                {                   
                    val = 0;
                    /* Assign value of (freq * (ln(freq)) to val. */
                    val = ((ajFloatGet(class_freq, x)) 
			   * ((log(ajFloatGet(class_freq, x)))));


                    /* Assign value of val to pos_score, which holds */
                    /* total score for all residues at the position. */
                    pos_score += val;
                }
                
            }
            /* Determine absolute value of pos_score. */
            temp = (fabs(pos_score));


            /* Invert the score, by taking away from 1. */
            temp2 = (1 - temp);


            /* Assign total score for substitution of all the residues */
            /* at a position into scoring array.                       */      
            ajFloatPut(&(*scores)->seqvar_score, post_cnt, temp2);
        }
        
        else
            continue;
    }
    
           
    return ajTrue;
}








/* @funcstatic  siggen_ScoreNcon **********************************************
**
** Reads a Scopalg object and writes a Scorealg object. Each residue in 
** the alignment is scored on the basis of the number of phsyical contacts
** (residue-residue) it makes in the structure. Contacts are read from a
** Cmaps object.
**
** @param [r] alg      [AjPScopalg]     Alignment
** @param [w] scores   [AjPScorealg*]   Scores for alignment
** @param [r] cmaps    [AjPCmap*]       Residue contacts
** @param [r] seq_pos  [AjPUint2d]       Index for alignment
** @param [r] atom_idx [AjPUint*]        Index residue positions.
** @param [r] noca     [AjBool*]        Whether residues possess a CA atom.
**
** @return [AjBool] True on succcess
** @@
******************************************************************************/
static AjBool  siggen_ScoreNcon(AjPScopalg alg, 
				AjPScorealg *scores, 
				AjPCmap *cmaps, 
				AjPUint2d seq_pos, 
				AjPUint *atom_idx, 
				AjBool *noca)
{

    ajint       memb_cnt     =0;    /* Counter for members of the family (alignment). */
    ajint       xmat_cnt     =0;    /* Counter for x axis of contact matrix. */
    ajint       ymat_cnt     =0;    /* Counter for y axis of contact matrix. */
    ajint       nconcount    =0;    /* Counter for number of contacts at position*/
    ajint       x            =0;    /* Counter for initializing arrays to zero. */
    ajint       post_cnt     =0;    /* Counter for post_similar line.        */
    ajint       nconpos_cnt  =0;    /* Counter to hold ncon.                 */
    ajint       idx_seqpos   =0;    /* Index into seq_pos array.             */
    ajint       idx_atomidx  =0;    /* Index into atom_idx array.            */
    float       av_ncon      =0;    /* Counter to hold average no. of contacts. */
    AjIStr      iter         =NULL; /* Iterator for post_similar string.     */
    AjPUint2d    align_ncon   =NULL; /* Matrix of number of contacts for every
				       residue.                              */

    
    

    /* Allocate memory for the align_ncon array . */
    align_ncon = ajUint2dNewRes((ajint)alg->width);        


    /* Assign iterator for post_similar line. */
    iter = ajStrIterNew(alg->Post_similar);


    /* Create arrays of size width. */
    for(x = 0; x < alg->N; x++)
        ajUint2dPut(&align_ncon, x, alg->width-1, (ajint) 0);


    
    /* Determine ncon value for every residue. */
    /* Counter for sequences in alignment. */
    for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
    {   
        /* Counter for x-axis of contact map. */
        for(xmat_cnt = 0; xmat_cnt < cmaps[memb_cnt]->Dim; xmat_cnt++)
        {
            /* Counter for y-axis of contact map. */
            for(ymat_cnt = 0; ymat_cnt < cmaps[memb_cnt]->Dim; ymat_cnt++)
            {
	      /*	      ajFmtPrint(">");fflush(stdout); */

                /* Check if position in contact map is 1 (i.e. contact). */
                if(ajUint2dGet(cmaps[memb_cnt]->Mat, xmat_cnt, ymat_cnt) == 1)
                    nconcount++;
            }
            /* Put nconcounter value into array. */
            ajUint2dPut(&align_ncon, memb_cnt, xmat_cnt, nconcount);
            nconcount = 0;
        }
    }



    /* Counter for positions in alignment. */
    for(post_cnt = 0; post_cnt < alg->width; post_cnt++)
    {
        if(((ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') && 
	    ((*scores)->filterpsim == ajTrue) &&
            ((*scores)->filtercon == ajTrue) && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
           ||   
           (((*scores)->filterpsim == ajTrue) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') &&
            ((*scores)->filtercon == ajFalse))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
            ((*scores)->filtercon == ajTrue) && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
            ((*scores)->filtercon == ajFalse)))
        {
            /* Extract ncon for residues in alignment at that position. */
            for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
            {
                /* Check to see if alignment position is a gap.    */
                /* Assign position of seq_pos array to idx_seqpos. */

	      /*	      ajFmtPrint("post_cnt:%d", post_cnt);
	      ajFmtPrint(".");
	      fflush(stdout); */

                if((idx_seqpos = ajUint2dGet(seq_pos, memb_cnt, post_cnt))==-1)
                    continue;
                
                /* Assign position of atom_idx array to idx_atomidx. */
                else
		  {
                    if((idx_atomidx = ajUintGet(atom_idx[memb_cnt], idx_seqpos))==-1)
		      ajFatal("Oh no! Gap not detectd in seq_pos but -1 found in idx_atomidx");
		  }

		/*		ajFmtPrint("memb_cnt:%d idx_seqpos:%d", 
				memb_cnt, idx_seqpos);
				fflush(stdout); */


                /* If noca is true, increment counter into align_ncon array by 1 */
                /* to take into account extra 'residue' not present in alignment. */
                if(noca[memb_cnt] == ajTrue)
                {
		  /*		  ajFmtPrint("!");		fflush(stdout); */
                    nconpos_cnt += ajUint2dGet(align_ncon, memb_cnt, idx_atomidx);
                }
                
                /* Else continue as normal. */
                else
                {
		  /*		  ajFmtPrint("*");		fflush(stdout); */
                    nconpos_cnt += ajUint2dGet(align_ncon, memb_cnt, (idx_atomidx - 1));
                }
            }


            /* Divide total ncon at the position by no. of sequences in 
	       alignment. */
            av_ncon = (float)nconpos_cnt/(float)alg->N;
            nconpos_cnt = 0;


            /* Assign av. ncon value for residues at that position into */
            /* ncon_score array of scores structure at position corresponding
	       to alignment. */
            ajFloatPut(&(*scores)->ncon_score, post_cnt, av_ncon);
        }
    }


    /* Free memory for matrix and iterator. */
    ajUint2dDel(&align_ncon);
    ajStrIterDel(&iter);


    /* Return */
    return ajTrue;
}





/* @funcstatic  siggen_ScoreCcon **********************************************
**
** Read  a Scopalg object and writes a Scorealg object. Each residue in 
** the alignment is scored on the basis of the conservation of phsyical 
** contacts (residue-residue) it makes in the structures. Contacts are 
** read from a Cmaps object.
**
** @param [r] alg     [AjPScopalg ]    Alignment
** @param [w] scores  [AjPScorealg*]   Scores for alignment

** @param [r] cmaps   [AjPCmap*]       Residue contacts
** @param [r] seq_pos [AjPUint2d]       Index for alignment
**
** @return [AjBool] True on succcess
** @@
*****************************************************************************/
static AjBool  siggen_ScoreCcon(AjPScopalg alg, AjPScorealg *scores, 
				AjPCmap *cmaps, 
				AjPUint2d seq_pos, AjPUint *atom_idx, 
				AjBool *noca)
{

    ajint       memb_cnt     =0;    /* Counter for members of the family (alignment). */
    ajint       post_cnt     =0;    /* Counter for post_similar line. */
    ajint       y_cnt        =0;    /* Counter for y axis of contact matrix. */
    ajint       seqpos_cnt   =0;    /* Counter for position in alignment. */
    ajint       atomidx_cnt  =0;    /* Counter for position in actual sequence. */
    ajint       nsite        =0;    /* Number of sites with oarticular contact. */
    ajint       x            =0;    /* Counter. */
    ajint       temp         =0;    /* Current position of seq_pos array. */
    ajint       num          =0;    /* Assign to each element of con_contacts. */
    ajint       p            =0;
    ajint       idx_seqpos   =0;    /* Index. */
    ajint       idx_atomidx  =0;    /* Index. */
    ajint       size         =0;    /* length of atom_idx arrays. */

    float       sum          =0;    /* Variable to hold nsite calculation. */
    AjIStr      iter         =NULL; /* Iterator for post_similar string. */
    AjPUint2d    con_contact  =NULL; /* Matrix of conserv of contacts for every residue. */
    AjPUint      con_line     =NULL; /* Temp storage of line. */
    AjPUint      atomidx_size =NULL; /* Array of sizes of atom_idx arrays. */
    


    
    /*Allocate memory for arrays . */
    con_contact    = ajUint2dNewRes((ajint)alg->width);    
    con_line       = ajUintNewRes((ajint)alg->width);
    atomidx_size   = ajUintNewRes((ajint)alg->N);

    
    for(memb_cnt = 0; memb_cnt<alg->N;memb_cnt++)
    {
        if(alg->width == cmaps[memb_cnt]->Dim)
        {
            for(x=0; x<=alg->width;x++)
            {
                if(ajUintGet(atom_idx[memb_cnt], x) == -1)
                {
                    ajUintPut(&atomidx_size, memb_cnt, x);
                    break;
                }
                else
                    continue;
            }
        }

        else
        {
            for(x=0; x<alg->width;x++)
            {
                if(ajUintGet(atom_idx[memb_cnt], x) == -1)
                {
                    ajUintPut(&atomidx_size, memb_cnt, x);
                    break;
                }
                else
                    continue;
            }
        }
    }
    

    
    /* Assign iterator for post_similar line. */
    iter = ajStrIterNew(alg->Post_similar);
    

    
    /* Create arrays of size width. */
    for(x = 0; x < alg->width; x++)
        ajUint2dPut(&con_contact, x, alg->width-1, 0);


    /* Create arrays of size width. */
    ajUintPut(&con_line, alg->width-1, 0);



    /* Start of main loop. */
    /* Iterate through Post_similar line. */
    for(post_cnt = 0; post_cnt < alg->width; post_cnt++)
    {
        if(((ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') && 
	    ((*scores)->filterpsim == ajTrue) &&
           ((*scores)->filtercon == ajTrue) && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
          ||
           (((*scores)->filterpsim == ajTrue) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') &&
           ((*scores)->filtercon == ajFalse))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
           ((*scores)->filtercon == ajTrue) && 
	    (ajUintGet((*scores)->ncon_thresh, post_cnt) == 1))
           ||
           (((*scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, post_cnt) != '-') && 
           ((*scores)->filtercon == ajFalse)))
        {
            /* Create array of size width. */
            for(x = 0; x < alg->width; x++)
                ajUintPut(&con_line, x, 0); 
            nsite = 0;
            num = 0;


            /* Perform calculation for each member of family. */

            for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
            {   
                /* Go through each element of cmap column. */
                for(y_cnt = 0; y_cnt < cmaps[memb_cnt]->Dim; y_cnt++)
                {
                    /*Check to see if alignment position is a gap*/
                    if((idx_seqpos=ajUint2dGet(seq_pos,memb_cnt,post_cnt))==-1)
                        {
                            printf("Error! found a '-' in siggen_ScoreCcon function\n");
                            continue; 
                        }
                    
                    /* Assign position of atom_idx array to idx_atomidx. */
                    else
                        idx_atomidx = ajUintGet(atom_idx[memb_cnt], idx_seqpos);

                    if(noca[memb_cnt] == ajTrue)
                    {
                        /*Check if position in contact map is 1 (i.e. contact). */
                        if(ajUint2dGet(cmaps[memb_cnt]->Mat, (idx_atomidx), 
				      y_cnt)==1)
                        {
                            /* Assign number of elements in atom_idx array to 
			       size. */
                            size = ajUintGet(atomidx_size, memb_cnt);
                            /* determine position of ymat_cnt in atom_idx array. */
                            for(atomidx_cnt=0; atomidx_cnt<size; atomidx_cnt++)
                            {
                                /* Assign value from atom_idx to variable. */
                                idx_atomidx = ajUintGet(atom_idx[memb_cnt], 
						       atomidx_cnt);


                                /* Find position where value of y_cnt appears. */
                                if(idx_atomidx == y_cnt)
                                {
                                    for(seqpos_cnt=0; seqpos_cnt<alg->width; 
					seqpos_cnt++)
                                    {
                                        /* Assign value from seq_pos to variable. */
                                        idx_seqpos=ajUint2dGet(seq_pos, memb_cnt,
							      seqpos_cnt);
                                    
                                        /* Check if seq_pos element contains the
					   (atomidx_cnt)th structured residue. */
                                        if((idx_seqpos == atomidx_cnt) 
					   && (idx_seqpos != '-'))  /* Matt */
                                        {
                                            /* Assign current value of seqpos_cnt
					       to temp, i.e. the element of 
					       seq_pos (hence position in 
					       alignment) where the residue from 
					       atom_idx array occurs. */
                                            temp = seqpos_cnt; /* !!idx_seqpos*/
                                    
                                            /* increment the number if sites counter. */
                                            if(ajUintGet(con_line, temp) == 0)
                                                nsite++;

                                            /* Increment element of con_line. */
                                            ajUintInc(&con_line, (ajint) temp);

                                            /* !!idx_seqpos*/
                                            ajUint2dPut(&con_contact, post_cnt, 
						       seqpos_cnt, 
                                                       (ajUintGet(con_line, seqpos_cnt)));
                                            break;
                                        }
                                
                                        else
                                            continue;
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    else if(noca[memb_cnt] == ajFalse)
                    {
                        /*Check if position in contact map is 1 (i.e. contact). */
                        /* -1 as atom_idx counts from 1 NOT ZERO!!. */
                        if(ajUint2dGet(cmaps[memb_cnt]->Mat, (idx_atomidx-1), 
				      y_cnt)==1)
                        {
                            /* Assign number of elements in atom_idx array to size. */
                            size = ajUintGet(atomidx_size, memb_cnt);

                            /* determine position of ymat_cnt in atom_idx array. */
                            for(atomidx_cnt=0; atomidx_cnt<size; atomidx_cnt++)
                            {
                                /* Assign value from atom_idx to variable. */
                                idx_atomidx = ajUintGet(atom_idx[memb_cnt], 
						       atomidx_cnt);
				
                                /* Find position where value of y_cnt appears. */
                                /* +1 as atom_idx array counts from 1. */
                                if(idx_atomidx == (y_cnt + 1))
                                {
                                    for(seqpos_cnt=0; seqpos_cnt<alg->width; 
					seqpos_cnt++)
                                    {
                                        /* Assign value from seq_pos to variable. */
                                        idx_seqpos=ajUint2dGet(seq_pos, memb_cnt,
							      seqpos_cnt);
                                    
                                        /* Check if seq_pos element contains the 
					   (atomidx_cnt)th structured residue. */
                                        if((idx_seqpos == atomidx_cnt)
					   && (idx_seqpos != '-')) /* Matt */
                                        {
                                            /* Assign current value of seqpos_cnt
					       to temp, i.e. the element of seq_pos
					       (hence position in alignment) where
					       the residue from atom_idx array 
					       occurs. */
                                            temp = seqpos_cnt; /* !!idx_seqpos*/
                                    
                                            /* Increment the number if sites counter. */
                                            if(ajUintGet(con_line, temp) == 0)
                                                nsite++;

                                            /* Increment element of con_line. */
                                            ajUintInc(&con_line, (ajint) temp);
					    
                                            ajUint2dPut(&con_contact, post_cnt,
						       seqpos_cnt, 
                                                       (ajUintGet(con_line, seqpos_cnt)));
                                            break;
                                        }
                                
                                        else
                                            continue;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            sum = 0;
            for(p = 0; p < alg->width; p++)
            {
                /* Divide number of sequences making a contact with a particular */
                /* residue by the total no. of sequences.                        */
                if(ajUintGet(con_line, p)!=0)
                    sum += ((float)(ajUintGet(con_line, p)/((float)(ajint)alg->N)));

            }

            

            /* Write total sum for all residues at position post_cnt into scores
	       structure. */ 
            ajFloatPut(&(*scores)->ccon_score, post_cnt, (sum/(float)nsite));
        }
    }


    

    /* Free memory for arrays and iterator. */
    ajUintDel(&con_line);
    ajUintDel(&atomidx_size);
    ajUint2dDel(&con_contact);
    ajStrIterDel(&iter);


    /* Return. */
    return ajTrue;
}







/* @funcstatic  siggen_SigSelect **********************************************
**
** Read Scopalg and Scorealg objects and returns a pointer to a Signature
** object. The signature generated is derived from a number, determined
** by the sparsity of the signature, of the top-scoring positions in the 
** alignment
**
** @param [r] alg        [AjPScopalg]  Alignment 
** @param [r] scores     [AjPScorealg] Scores for alignment
** @param [r] seq_pos    [AjPUint2d]    Index for alignment
** @param [r] sig_sparse [ajint]       Sparsity of signature
** @param [r] cmaps      [AjPCmap *]   Contact maps.  
** @param [r] atom_idx   [AjPUint *]    Index residue positions.
** @param [r] spar_check [float *]     Sparsity check array.
** @param [r] wsiz       [ajint]       Window size 
** 
** @return [EmbPSignature] Pointer to Signature structure or NULL on failure
** @@
*****************************************************************************/
static EmbPSignature  siggen_SigSelect(AjPScopalg alg, 
				       AjPScorealg scores, 
				       AjPUint2d seq_pos, 
				       ajint sig_sparse, 
				       AjPCmap *cmaps, 
				       AjPUint *atom_idx, 
				       float *spar_check, 
				       ajint wsiz)
{
    ajint       nseqs=0;                /* Number of sequences. */
    ajint       idx              =0;    /* Index.               */
    ajint       atomidx          =0;    /* Index.               */
    ajint       cnt              =0;    /* Counter for post_similar line. */
    ajint       first            =0;    /* Counter. */
    ajint       x                =0;    /* Counter. */
    ajint       memb_cnt         =0;    /* Counter. */
    ajint       rand_cnt         =0;    /* Counter. */
    ajint       j                =0;    /* Counter. */
    ajint       i                =0;    /* Counter. */
    ajint       av_temp          =0;    /* Counter. */
    float       total_temp       =0;    /* Counter. */
    ajint       imax             =0;    /* Loop exit condition. */
    ajint       hold_pos         =0;    /* Counter to hold values whilst sorting. */

    ajint       res_count        =0;    /* Counter for total number of residues. */
    ajint       num_aligned      =0;    /* Number of positions scoring '1' in alignment. */
    ajint       single_rescount  =0;
    ajint       gsiz             =0;    /* Temp. size of gap*/
    ajint       npos             =0;    /* Counter of signature positions*/
    ajint       sig_npos         =0;    /* Number of positions in signature. */
    float       val              =0.0;  /*Temp. val*/
    float       seqmat_min       =0;    /* Variable for minimum seq subn score. */
    float       seqmat_max       =0;    /* Variable for maximum seq subn score. */
    float       seqvar_min       =0;    /* Variable for minimum seq var score. */
    float       seqvar_max       =0;    /* Variable for maximum seq var score. */
    float       ncon_min         =0;    /* Variable for minimum ncon score. */
    float       ncon_max         =0;    /* Variable for maximum ncon score. */
    float       ncon_temp        =0;    /* Variable for temp. ncon score. */
    float       ccon_min         =0;    /* Variable for minimum ncon score. */
    float       ccon_max         =0;    /* Variable for maximum ncon score. */
    float       ccon_temp        =0;    /* Variable for temp. ncon score. */
    float       hold             =0;    /* Counter to hold values whilst sorting. */
    AjBool      done             =ajFalse;  /*Flag*/
    AjBool      finish           =ajFalse;  /*Flag*/
    AjBool      already_done     =ajFalse;  /*Flag*/
    AjPFloat    seqmat_normal    =NULL; /* Array of floats for normalised seqmat_score. */
    AjPFloat    seqvar_normal    =NULL; /* Array of floats for normalised seqvar_score. */
    AjPFloat    ncon_normal      =NULL; /* Array of floats for normalised ncon_score. */
    AjPFloat    ccon_normal      =NULL; /* Array of floats for normalised ccon_score. */
    AjPFloat    total_score      =NULL; /* Array of floats for total normalised scores. */
    AjPUint      keyres_pos       =NULL; /* Total_score array, sorted in ascending order. */
    AjPUint      post_sim         =NULL; /* Array for positions in post_similar line. */
    AjPUint      seq_len          =NULL; /* Array of length of each seq, as in alignment . */
    AjPUint      fullseq_len      =NULL; /* Array of length of each seq, as in original seq . */
    AjPUint      rand_pos         =NULL; /* Array of positions for random selection . */
    AjPUint      temp_rand        =NULL; /* Array of positions for random selection . */
    AjPUint2d    keyres_seq       =NULL; /* Total_score array, sorted in ascending order. */
    AjPUint2d    atomres_seq      =NULL; /* Total_score array, sorted in ascending order. */
    AjPStr      *seq_array       =NULL; /* Arrays of sequence (w/o gaps) from alignment. */
    EmbPSignature sig=NULL;              /* Signature. */
    float       spar             =0.0;
    
    double      rn               = 0;   
    int         irn              = 0; 
    

    /* Check args. */
    if(!alg || !scores || !seq_pos || !sig_sparse || !atom_idx)
    {
        ajWarn("Null args passed to siggen_SigSelect");
        return NULL;
    }
    
    /* Seed the number generator. */
    ajRandomSeed();
    
    /* Create arrays. */
    seqmat_normal       =  ajFloatNewRes(alg->width);
    seqvar_normal       =  ajFloatNewRes(alg->width);
    ncon_normal         =  ajFloatNewRes(alg->width);
    ccon_normal         =  ajFloatNewRes(alg->width);
    total_score         =  ajFloatNewRes(alg->width);
    keyres_pos          =  ajUintNewRes(alg->width);    


    post_sim            =  ajUintNewRes(alg->width);
    rand_pos            =  ajUintNewRes(alg->width);
    keyres_seq          =  ajUint2dNew();    
    atomres_seq         =  ajUint2dNew();    
    seq_len             =  ajUintNewRes(alg->N);    
    fullseq_len         =  ajUintNewRes(alg->N);    
    

    /* Initialise array elements to zero. */
    ajFloatPut(&seqmat_normal, alg->width-1, (float) 0.0);

    ajFloatPut(&seqvar_normal, alg->width-1, (float) 0.0);
    ajFloatPut(&ncon_normal, alg->width-1, (float) 0.0);
    ajFloatPut(&ccon_normal, alg->width-1, (float) 0.0);
    ajFloatPut(&total_score, alg->width-1, (float) 0.0);

    ajUintPut(&keyres_pos, alg->width-1, (ajint) 0);
    ajUintPut(&post_sim, alg->width-1, (ajint) 0);
    ajUintPut(&rand_pos, alg->width-1, (ajint) 0);

    nseqs=ajDmxScopalgGetseqs(alg, &seq_array);

    /* Start of main application loop. */
    /* Determine min and max values of seqmat_score */
    /* Iterate through (scores)->seqmat_score array. */

    if((scores)->seqmat_do == ajTrue)
    {
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Check if value is not zero. */
            if(ajFloatGet((scores)->seqmat_score, cnt) != 0)
            {

                /* If in loop for first time assign min and max. */

                if(first == 0)
                {
                    seqmat_min = ajFloatGet((scores)->seqmat_score, cnt);
                    seqmat_max = ajFloatGet((scores)->seqmat_score, cnt);
                    first = 1;
                }


                /* Check if value is >, <, = to min/max. */
                /* If true assign value to min/max variable. */
                if(ajFloatGet((scores)->seqmat_score, cnt) < seqmat_min)
                    seqmat_min = ajFloatGet((scores)->seqmat_score, cnt);
                if(ajFloatGet((scores)->seqmat_score, cnt) == seqmat_min)
                    continue;
                if(ajFloatGet((scores)->seqmat_score, cnt) == seqmat_max)
                    continue;
                if(ajFloatGet((scores)->seqmat_score, cnt) > seqmat_max)
                    seqmat_max = ajFloatGet((scores)->seqmat_score, cnt);
            }
        }
    }

    /* Start of main application loop */
    /* Determine min and max values of seqvar_score */
    /* Iterate through (scores)->seqvar_score array. */
    if((scores)->seqvar_do == ajTrue)
    {
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Check if value is not zero. */
            if(ajFloatGet((scores)->seqvar_score, cnt) != 0)
            {
                /* If in loop for first time assign min and max. */
                if(first == 0)
                {
                    seqvar_min = ajFloatGet((scores)->seqvar_score, cnt);
                    seqvar_max = ajFloatGet((scores)->seqvar_score, cnt);

                    first = 1;
                }



                /* Check if value is >, <, = to min/max */
                /* If true assign value to min/max variable. */
                if(ajFloatGet((scores)->seqvar_score, cnt) < seqvar_min)
                    seqvar_min = ajFloatGet((scores)->seqvar_score, cnt);
                if(ajFloatGet((scores)->seqvar_score, cnt) == seqvar_min)
                    continue;
                if(ajFloatGet((scores)->seqvar_score, cnt) == seqvar_max)
                    continue;

                if(ajFloatGet((scores)->seqvar_score, cnt) > seqvar_max)
                    seqvar_max = ajFloatGet((scores)->seqvar_score, cnt);
            }
        }
    }




    /* Determine min and max values of ncon_score */
    /* Iterate through (scores)->ncon_score array. */
    if((scores)->ncon_do == ajTrue)
    {
        first = 0;
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Check if value is not zero. */
            if(ajFloatGet((scores)->ncon_score, cnt) != 0)
            {
                /* If in loop for first time assign min and max. */
                if(first == 0)

                {
                    ncon_min = ncon_max = ajFloatGet((scores)->ncon_score, cnt);

                    first = 1;

                }


                ncon_temp = ajFloatGet((scores)->ncon_score, cnt);
                if(ncon_temp < ncon_min)


                    ncon_min = ncon_temp;
                else if(ncon_temp > ncon_max)

                    ncon_max = ncon_temp;
            }
        }
    }

    /* Determine min and max values of ccon_score */
    /* Iterate through (scores)->ccon_score array. */
    if((scores)->ccon_do == ajTrue)
    {

        first = 0;
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Check if value is not zero. */
            if(ajFloatGet((scores)->ccon_score, cnt) != 0)
            {
                /* If in loop for first time assign min and max. */
                if(first == 0)

                {
                    ccon_min = ccon_max = ajFloatGet((scores)->ccon_score, cnt);

                    first = 1;

                }


                ccon_temp = ajFloatGet((scores)->ccon_score, cnt);
                if(ccon_temp < ccon_min)
                    ccon_min = ccon_temp;

                else if(ccon_temp > ccon_max)
                    ccon_max = ccon_temp;
            }
        }
    }



    /* Perform normalising calculation on seqmat_score array */
    if((scores)->seqmat_do == ajTrue)
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Only perform nornalisation on Post_similar. */
            /* positions with value '1' or '0', if filter = ajFalse */
	    /*  if((((scores)->filterpsim) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) == '1')) ||
               ((scores)->filterpsim == ajFalse))*/
	    
	    if(((ajStrGetCharPos(alg->Post_similar, cnt) == '1') && 
		((scores)->filterpsim == ajTrue) &&
		((scores)->filtercon == ajTrue) && 	
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajTrue) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) == '1') &&
		((scores)->filtercon == ajFalse))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajFalse)))
            {   
                /* Put normalised score into array at eqiuvalent position. */
                ajFloatPut(&seqmat_normal, cnt,((ajFloatGet((scores)->seqmat_score, cnt) 
                                                 - seqmat_min)/(seqmat_max - seqmat_min)));
            }	
            else
                ajFloatPut(&seqmat_normal, cnt, 0);
        }


    /* Perform normalising calculation on seqvar_score array. */
    if((scores)->seqvar_do == ajTrue)
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Only perform nornalisation on Post_similar */
            /* positions with value '1'                  . */

	    if(((ajStrGetCharPos(alg->Post_similar, cnt) == '1') && 
		((scores)->filterpsim == ajTrue) &&
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajTrue) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) == '1') &&
		((scores)->filtercon == ajFalse))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajFalse)))
            {   
                /* Put normalised score into array at eqiuvalent position. */
                ajFloatPut(&seqvar_normal, cnt,
			   ((ajFloatGet((scores)->seqvar_score, cnt) 
			     - seqvar_min)/(seqvar_max - seqvar_min)));

            }

            else

                ajFloatPut(&seqvar_normal, cnt, 0);
        }
    
 

    /* Perform normalising calculation on ncon_score array. */
    if((scores)->ncon_do == ajTrue)
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Only perform nornalisation on Post_similar */
            /* positions with value '1'                  . */

	    if(((ajStrGetCharPos(alg->Post_similar, cnt) == '1') && 
		((scores)->filterpsim == ajTrue) &&
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajTrue) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) == '1') &&
		((scores)->filtercon == ajFalse))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajFalse)))
            {
                /* Put normalised score into array at eqiuvalent position. */
                ajFloatPut(&ncon_normal, cnt,((ajFloatGet((scores)->ncon_score, cnt) 
                                           - ncon_min)/(ncon_max - ncon_min)));  
            }   
            
            else
                ajFloatPut(&ncon_normal, cnt, 0);           
            
        }

    
    /* Perform normalising calculation on ccon_score array. */
    if((scores)->ccon_do == ajTrue)
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Only perform nornalisation on Post_similar */
            /* positions with value '1'                   */

        if(((ajStrGetCharPos(alg->Post_similar, cnt) == '1') && 
	    ((scores)->filterpsim == ajTrue) &&
           ((scores)->filtercon == ajTrue) && 	
	    (ajUintGet((scores)->ncon_thresh, cnt) == 1))
	   ||
           (((scores)->filterpsim == ajTrue) && 
	    (ajStrGetCharPos(alg->Post_similar, cnt) == '1') &&
           ((scores)->filtercon == ajFalse))
           ||
           (((scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
           ((scores)->filtercon == ajTrue) && 
	    (ajUintGet((scores)->ncon_thresh, cnt) == 1))
           ||
           (((scores)->filterpsim == ajFalse) && 
	    (ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
           ((scores)->filtercon == ajFalse)))
            {
                /* Put normalised score into array at eqiuvalent position. */
                ajFloatPut(&ccon_normal, cnt,
			   ((ajFloatGet((scores)->ccon_score, cnt) 
			     - ccon_min)/(ccon_max - ccon_min)));  
            }   
            
            else
                ajFloatPut(&ccon_normal, cnt, 0);           
        }

    /* Add total scores for each position depending on the */
    /* scoring options selected.  Enter value in total_score array. */

    for(x=0; x<alg->width; x++)
    {
        val=0.0;
        if((scores)->seqmat_do == ajTrue) 
            val+=ajFloatGet(seqmat_normal, x);

        if((scores)->seqvar_do == ajTrue) 
            val+=ajFloatGet(seqvar_normal, x);

        if((scores)->ncon_do == ajTrue) 
            val+=ajFloatGet(ncon_normal, x);

        if((scores)->ccon_do == ajTrue)
            val+=ajFloatGet(ccon_normal, x);


        ajFloatPut(&total_score, x, val);

    }
    
    /* Initialise array elements from 0 to alg->width. Count number of 
       potential signature positions depending on the scoring methods
       selected. */
    num_aligned = -1;
    
    for(x=0; x<alg->width; x++)
    {
        /* Initialise post_sim array to zero. */
        ajUintPut(&post_sim, x, x);
        
        /* Check if filtersim and filtercon are true and that ncon_thresh and 
	   post_similar = 1, if so incrememnt num_aligned. */
        if((ajStrGetCharPos(alg->Post_similar, x) == '1') && 
	   ((scores)->filterpsim == ajTrue) &&
           ((scores)->filtercon == ajTrue) && 
	   (ajUintGet((scores)->ncon_thresh, x) == 1))
            {
		num_aligned++;
		ajUintPut(&rand_pos, j++, x);
	    }
	
        else if(((scores)->filterpsim == ajTrue) && 
		(ajStrGetCharPos(alg->Post_similar, x) == '1') &&
           ((scores)->filtercon == ajFalse))
	    {
		num_aligned++;
		ajUintPut(&rand_pos, j++, x);
	    }
	
        else if(((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, x) != '-') && 
           ((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, x) == 1))
            {
		num_aligned++;
		ajUintPut(&rand_pos, j++, x);
	    }
	
        else if(((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, x) != '-') && 

           ((scores)->filtercon == ajFalse))
	{
            num_aligned++;
	    ajUintPut(&rand_pos, j++, x);
	}
    }    



    /* Perform bubble sort of total_score array. */ 
    for(i=1; i<=(alg->width-1); i++)
        for(j=0; j<=(alg->width-2); j++)
        { 
            /* Check if element j is > than element j+1. */
            if((ajFloatGet(total_score, j)) > 
               (ajFloatGet(total_score, (j + 1))))
            {
                /* Swap elements j and j+1 via hold/hold_pos variables. */
                hold = ajFloatGet(total_score, j);

                hold_pos = ajUintGet(post_sim, j);
                ajFloatPut(&total_score, j, (ajFloatGet(total_score, (j+1))));

                ajUintPut(&post_sim, j, (ajUintGet(post_sim, (j+1))));
                ajFloatPut(&total_score, (j+1), hold);
                ajUintPut(&post_sim, (j+1), hold_pos);
            }           

        }

    /* Determine average number of residues for each sequence from ALIGNMENT . */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
    {
        single_rescount = 0;
        for(i=0; i<alg->width; i++)
	  if(ajStrGetCharPos(alg->Seqs[memb_cnt], i) != 'X')  /* Andy */
            {
                single_rescount++;
            }

        ajUintPut(&seq_len, memb_cnt, single_rescount);
    }

    /* Determinine actual length of each sequence from dimensions of cmap, NOT
       the STAMP alignment. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
    {
        ajUintPut(&fullseq_len, memb_cnt, cmaps[memb_cnt]->Dim);
        res_count += cmaps[memb_cnt]->Dim;
    }
    

    /* Initialise keyres_seq array to zero. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
        for(i=0; i<ajUintGet(seq_len, memb_cnt); i++)
            ajUint2dPut(&keyres_seq, memb_cnt, i, 0);

    /* Initialise atomres_seq array to zero. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
        for(i=0; i<ajUintGet(fullseq_len, memb_cnt); i++)
            ajUint2dPut(&atomres_seq, memb_cnt, i, 0);    
    
    /* Calculate the number of positions the signature should have. Determine 
       no. of positions the signature will have. */
    sig_npos = (ajint) ceil((double)  ((res_count/alg->N) * 
                                       ( (float)sig_sparse/(float)100)));


    
    for(i=((alg->width)-1); i>((alg->width)-sig_npos); i--)
    {
        if(ajFloatGet(total_score, i) == 0)

            printf("Total_score array = 0.00 at position = %d\n", i);
    }


    /* Do random number generating here and fill post_sim array from the end 
       with the random numbers which equate to the putative signature positions
       identified above. */

    /* Assign temp array and initialise. */
    temp_rand =  ajUintNewRes(num_aligned);
    for(i=0;i<num_aligned;i++)
        ajUintPut(&temp_rand, i, -1);
    
    if(scores->random == ajTrue)
    {    
        while(finish == ajFalse)
        {
            /* Generate random number. */
            rn = ajRandomDouble();
            irn = (int) (rn * (double)num_aligned);
            already_done = ajFalse;
            
            /* Check number hasn't already been put into array. */
            for(i=0;i<num_aligned;i++)
            {
                if(ajUintGet(temp_rand, i) == irn)
                {
                    already_done = ajTrue;
                    break;
                }       
            
                else
                    continue;
            }

            /* If number not already in array, then put it in. */
            if(already_done == ajFalse)
                ajUintPut(&temp_rand, rand_cnt++, irn);

            /* Number already in array so continue and choose another. */
            else
                continue;

            /* If all array elements filled then finish while loop. */
            if(rand_cnt == num_aligned)

            {   
                finish = ajTrue;
            }
        }
    
        /* Copy the random numbers to the last 'num_aligned' positions of the 
	   post_sim array. */
        for(i=0;i<num_aligned;i++)
            ajUintPut(&post_sim, ((alg->width) - i), 
		     ajUintGet(rand_pos, ajUintGet(temp_rand, i)));
        
    }



    /* Check if the user entered sparsity exceeds the number of positions which
       are aligned in the alignment. */
    if(num_aligned < sig_npos)
    {
        /* Calculate sparsity for number of aligned positions. */
        av_temp = (ajint) (res_count/alg->N);
        total_temp = (((float)num_aligned/(float)av_temp) * (float)100);

        /* If so print message. */
        ajFmtPrint("There are not sufficient aligned positions to generate a "
		   "signature of the specified sparsity.\nSignature sparsity = %.2f\n", 
		   total_temp);

	if(total_temp < 1)
	{
	    /* Tidy up. */
	    ajFloatDel(&seqmat_normal);
	    ajFloatDel(&seqvar_normal);
	    ajFloatDel(&ncon_normal);
	    ajFloatDel(&ccon_normal);
	    ajFloatDel(&total_score);
	    ajUintDel(&keyres_pos);   
	    ajUintDel(&post_sim);     
	    ajUintDel(&rand_pos);     
	    ajUintDel(&temp_rand);     
	    ajUint2dDel(&keyres_seq);
	    ajUintDel(&seq_len);
	    for(x=0;x<nseqs;x++)
		ajStrDel(&seq_array[x]);
	    AJFREE(seq_array);
	    ajUint2dDel(&atomres_seq);
	    ajUintDel(&fullseq_len);
	    return NULL;
	}
	
        spar = total_temp;
        (*spar_check) = (ceil((double)spar));

        /* 
	 **  Generate signature using all of the aligned positions scoring '1'.
	 **  Begin at end i.e. highest scoring positions of post_sim array.
	 **  Count down from end until all '1' positions are entered in
	 **  keyres_pos array. 
	 */
        for(i=alg->width-1; i>=alg->width-(num_aligned); i--)
        {
            /* Put a '1' in keyres_pos array at position corresponding to 
	       position in post_sim array. */
            ajUintPut(&keyres_pos, ajUintGet(post_sim, i), 1);

        }
    }
    
    /* Else output signature of sparsity specified by user */
    else
    {
        /* Begin at end i.e. highest scoring positions of post_sim array.
	   Count down from end until sparsity is fulfilled. */
        for(i=alg->width-1; i>=alg->width-(sig_npos); i--)
        {
            /* Put a '1' in keyres_pos array at position corresponding to 
	       position in post_sim array. */
            ajUintPut(&keyres_pos, ajUintGet(post_sim, i), 1);
        }
    }    




    /* Fill 2d array with positions of each key res w.r.t. alignment seq. */
    for(i=0; i<alg->width; i++)
    {
        if(ajUintGet(keyres_pos, i) == 1)
        {
            for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)            
            {
                /* Check to see if alignment position is a gap. */
                if((idx=ajUint2dGet(seq_pos, memb_cnt, i))==-1)
                    continue;
                else
                    ajUint2dPut(&keyres_seq, memb_cnt, idx, 1); 
            }
        }
    }


    /* Convert keyres_seq array into atomidx_seq, i.e. the signature posions 
       w.r.t. the original sequence . */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
    {            
        for(x=0; x<ajUintGet(seq_len, memb_cnt); x++)
        {
            if(ajUint2dGet(keyres_seq, memb_cnt, x)==1)
            {
                atomidx = ajUintGet(atom_idx[memb_cnt], x);
                ajUint2dPut(&atomres_seq, memb_cnt, (atomidx-1), 1);
            }
        }
    }



    /* Allocate signature structure and write SCOP classification records. 
       If not enough aligned positions then make structure size num_aligned. */
    if(num_aligned < sig_npos)
            sig =  embSignatureNew(num_aligned);
    else
        sig =  embSignatureNew(sig_npos);

    sig->Type = alg->Type;
    ajStrAssignRef(&sig->Class, alg->Class);
    ajStrAssignRef(&sig->Architecture, alg->Architecture);
    ajStrAssignRef(&sig->Topology, alg->Topology);
    ajStrAssignRef(&sig->Fold, alg->Fold);
    ajStrAssignRef(&sig->Superfamily, alg->Superfamily);
    ajStrAssignRef(&sig->Family, alg->Family);
    sig->Sunid_Family =  alg->Sunid_Family;

    /* Allocate sigdat structures, don't know the number of residue or gaps in
       advance, these are set below, but set the window size here. */
    for(x=0; x<sig->npos; x++)
    {
	sig->dat[x]=embSigdatNew(0, 0);
	sig->dat[x]->wsiz=wsiz;
    }
    


    /* Write signature structure. */
    for(j=0; j<alg->N; j++)
    {   
        for(npos=0, gsiz=0, i=0, imax=ajUintGet(fullseq_len, j); 
            i<imax; 
            i++)       
        {
            /* The position is a signature position. */
            if(ajUint2dGet(atomres_seq, j, i) == 1)
            {
                /*Process gap. */
                for(done=ajFalse,x=0;x<sig->dat[npos]->ngap;x++)
                {
                    /*The gap length is NOT new*/
                    if((ajUintGet(sig->dat[npos]->gsiz, x)==gsiz))
                    {
                        ajUintInc(&sig->dat[npos]->gfrq, x);
                        done=ajTrue;
                        break;

                    }   
                }

                

                /*The gap length is new*/
                if(!done)
                {
                    sig->dat[npos]->ngap++;
                    ajUintPut(&sig->dat[npos]->gsiz, sig->dat[npos]->ngap-1, 
                             gsiz);
                    ajUintPut(&sig->dat[npos]->gfrq, sig->dat[npos]->ngap-1, 
                             (ajint) 1);
                }    


                gsiz=0;
                npos++;
            }
            else
                gsiz++;
        }

    }





    /* Write residue id into signature structure. */
    for(j=0; j<alg->N; j++)
    {
        for(npos=0, i=0, imax=ajUintGet(seq_len, j); 
            i<imax; 
            i++)       
        {
            /* The position is a signature position. */
            if(ajUint2dGet(keyres_seq, j, i) == 1)
            {
                /*Process residue*/
                for(done=ajFalse,x=0;x<sig->dat[npos]->nres;x++)
                    /*The residue id is NOT new*/
                {
                    if((ajChararrGet(sig->dat[npos]->rids, x))
                       ==ajStrGetCharPos(seq_array[j], i))
                    {
                        ajUintInc(&sig->dat[npos]->rfrq, x);
                        done=ajTrue;
                        break;
                    }
                }


                /*The residue id is new*/
                if(!done)
                {
                    sig->dat[npos]->nres++;
                    ajChararrPut(&sig->dat[npos]->rids, sig->dat[npos]->nres-1,
                                 ajStrGetCharPos(seq_array[j], i));

                    ajUintPut(&sig->dat[npos]->rfrq, sig->dat[npos]->nres-1, 
                            (ajint) 1);
                }    
                

                npos++;
            }
            else
                continue;
        }
    }
    

  
    /* Perform bubble sort of gap sizes and frequencies array . */ 
    for(x=0; x<sig->npos;x++) 
    { 
        for(cnt=1; cnt<(sig->dat[x]->ngap);cnt++)
            for(i=0; i<(sig->dat[x]->ngap-1);i++)
            {    
                /* Check if element i is > than element i+1. */
                if(ajUintGet(sig->dat[x]->gsiz, i) > ajUintGet(sig->dat[x]->gsiz, i+1))
                {       
                    /* Swap elements i and i+1 via y/hold_pos variables. */
                    j        = ajUintGet(sig->dat[x]->gsiz, i);
                    hold_pos = ajUintGet(sig->dat[x]->gfrq, i);

                    ajUintPut(&sig->dat[x]->gsiz, i, 
			     ajUintGet(sig->dat[x]->gsiz, i+1));
                    ajUintPut(&sig->dat[x]->gfrq, i,
			     ajUintGet(sig->dat[x]->gfrq, i+1));
                
                    ajUintPut(&sig->dat[x]->gsiz, i+1, j);
                    ajUintPut(&sig->dat[x]->gfrq, i+1, hold_pos);
                }               
            }
    }
    


      
    /* Tidy up. */
    ajFloatDel(&seqmat_normal);
    ajFloatDel(&seqvar_normal);
    ajFloatDel(&ncon_normal);
    ajFloatDel(&ccon_normal);
    ajFloatDel(&total_score);
    ajUintDel(&keyres_pos);   
    ajUintDel(&post_sim);     
    ajUintDel(&rand_pos);     
    ajUintDel(&temp_rand);     
    ajUint2dDel(&keyres_seq);
    ajUintDel(&seq_len);
    for(x=0;x<nseqs;x++)
        ajStrDel(&seq_array[x]);
    AJFREE(seq_array);
    ajUint2dDel(&atomres_seq);
    ajUintDel(&fullseq_len);

    return sig;
}





/* @funcstatic  siggen_ScoreAlignment *****************************************
**
** Convenience routine - calls several functions to score an alignment.
**
** @param [w] scores   [AjPScorealg*]   Scores for alignment
** @param [r] alg      [AjPScopalg ]    Alignment
** @param [r] cmaps    [AjPCmap*]       Residue contacts
** @param [r] mat      [AjPMatrixf]     Subsitution matrix
** @param [r] noca     [AjBool *]       Bool array for NOCA groups
** @param [r] seq_pos  [AjPUint2d]       Index for alignment
** @param [r] atom_idx [AjPUint *]       Index residue positions.
**
** @return [AjBool] True on succcess
** @@
*****************************************************************************/
static AjBool siggen_ScoreAlignment(AjPScorealg *scores, 
				    AjPScopalg alg, 
				    AjPCmap *cmaps, 
				    AjPMatrixf  mat, 
				    AjBool *noca,
				    AjPUint2d seq_pos,
				    AjPUint *atom_idx)
{
    /*Check args. */
    if( !(*scores) || !alg || !mat || !seq_pos || !atom_idx || !noca)
    {
        printf("Bad args passed to siggen_ScoreAlignment\n");
        return ajFalse;
    }
    
    

    /* Call scoring functions as appropriate. */
    if((*scores)->seqmat_do)
    {
        siggen_ScoreSeqMat(alg, scores, mat, seq_pos);
    }
    

    if((*scores)->seqvar_do)      
        siggen_ScoreSeqVar(alg, scores, seq_pos);
    

    if((*scores)->ncon_do)     
    {
        if(!(*cmaps))
            return ajFalse;
        siggen_ScoreNcon(alg, scores, cmaps, seq_pos, atom_idx, noca);
    }
    

    if((*scores)->ccon_do)
    {
        if(!(*cmaps))
            return ajFalse;
        siggen_ScoreCcon(alg, scores, cmaps, seq_pos, atom_idx, noca); 
    }
    

    return ajTrue;
}





/* @funcstatic  siggen_CalcSeqpos *********************************************
**
** Reads a Scopalg object and calculates an index for the alignment.
**
** @param [r] alg     [AjPScopalg]   Alignment
** @param [w] seq_pos [AjPUint2d*]    Index for alignment
**
** @return [AjBool] True on succcess
** @@
******************************************************************************/
static AjBool siggen_CalcSeqpos(AjPScopalg alg, 
				AjPUint2d *seq_pos)
{
    ajint  z        =0;     /* Loop counter.                                 */
    ajint  memb_cnt =0;     /* Counter for members of the family alignment). */   
    ajint  wid_cnt  =0;     /* Counter for position in the sequence.         */
    ajint  seq_cnt  =0;     /* Counter for position in seq w.r.t alignment.  */
    AjIStr iter     =NULL;  /* Iterator for sequence string.                 */


    /*Check args. */
    if(!alg)
        return ajFalse;


    /* This section determines the position of each aligned residue 
       in its original protein sequence. Allocate memory for the seq_pos 
       array. */
    *seq_pos    = ajUint2dNewRes((ajint)alg->N);

    
    /*Set reserved size. */
    for(z = 0; z < alg->N; z++)
        ajUint2dPut(seq_pos, z, alg->width, (ajint) 0);



    /* Determine position of each residue in alignment. */
    for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
    {
        /* Assign iterator for post_similar line. */
        iter = ajStrIterNew((alg->Seqs[memb_cnt]));

        
        /* Counter of sequence, reset for each member. */
        seq_cnt = 0;
        
        
        for(wid_cnt = 0; wid_cnt < alg->width; wid_cnt++)
        {
            /* Check if sequence line is not '-'. */     
	  if(ajStrIterGetK(iter) != 'X')   /* Andy */
            {
                /* For every position in alignment assign position of residue 
		   from its respective sequence. */
                ajUint2dPut(seq_pos, memb_cnt, wid_cnt, seq_cnt);
                seq_cnt++;
            }
            else
                ajUint2dPut(seq_pos, memb_cnt, wid_cnt, -1);
            
            
            ajStrIterNext(iter);                                   
        }
        ajStrIterDel(&iter);
    }
        

    /* Tidy up and return. */

    return ajTrue;
}



/* @funcstatic  siggen_Con_thresh *********************************************
**
** Reads Cmaps object, and determines whether each position in the 
** structural alignment displays greater than a threshold (conthresh)
** number of (residue-residue) contacts.  A value of '1' is entered
** in the corresponding element of the ncon_thresh array (within 
** the scores structure), otherwise the element = '0'
**
** @param [r] alg         [AjPScopalg ]  Alignment
** @param [w] scores      [AjPScorealg*] Scores for alignment
** @param [r] cmaps       [AjPCmap*]     Residue contacts
** @param [r] conthresh   [ajint]        contact threshold
** @param [r] noca        [AjBool *]     Bool array for NOCA groups
** @param [r] seq_pos     [AjPUint2d]     Index for alignment
** @param [r] atom_idx    [AjPUint *]     Index residue positions.
**
** @return [AjBool] True on succcess
** @@
*****************************************************************************/
static AjBool siggen_Con_Thresh(AjPScopalg alg, 
				AjPScorealg *scores, 
				AjPCmap *cmaps, 
				ajint conthresh, 
				AjBool *noca, 
				AjPUint2d seq_pos,
				AjPUint *atom_idx)
{

    ajint       memb_cnt     =0;    /* Counter for members of the family (alignment). */
    ajint       xmat_cnt     =0;    /* Counter for x axis of contact matrix. */
    ajint       ymat_cnt     =0;    /* Counter for y axis of contact matrix. */
    ajint       nconcount    =0;    /* Counter for number of contacts at position*/
    ajint       x            =0;    /* Counter for initializing arrays to zero. */
    ajint       post_cnt     =0;    /* Counter for post_similar line. */
    ajint       nconpos_cnt  =0;    /* Counter to hold ncon. */
    ajint       idx_seqpos   =0;    /* Index into seq_pos array. */
    ajint       idx_atomidx  =0;    /* Index into atom_idx array. */
    ajint       con_counter  =0;    /* Counter. */
    float       av_ncon      =0;    /* Counter to hold average no. of contacts. */
    AjIStr      iter         =NULL; /* Iterator for post_similar string. */
    AjPUint2d    align_ncon   =NULL; /* Matrix of number of contacts for every residue. */



    /* Allocate memory for the align_ncon array . */
    align_ncon = ajUint2dNewRes((ajint)alg->width);        


    /* Assign iterator for post_similar line. */
    iter = ajStrIterNew(alg->Post_similar);


    /* Create arrays of size width. */
    for(x = 0; x < alg->N; x++)
        ajUint2dPut(&align_ncon, x, alg->width-1, (ajint) 0);


    
    /* Determine ncon value for every residue counter for sequences in alignment. */
    for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
    {   
        /* Counter for x-axis of contact map. */
        for(xmat_cnt = 0; xmat_cnt < cmaps[memb_cnt]->Dim; xmat_cnt++)
        {
            /* Counter for y-axis of contact map. */
            for(ymat_cnt = 0; ymat_cnt < cmaps[memb_cnt]->Dim; ymat_cnt++)
            {
                /* Check if position in contact map is 1 (i.e. contact). */
                if(ajUint2dGet(cmaps[memb_cnt]->Mat, xmat_cnt, ymat_cnt) == 1)
                {
                    /* Increment ncon counter. */
                    nconcount++;
                }
            }
            /* Put nconcounter value into array. */
            ajUint2dPut(&align_ncon, memb_cnt, xmat_cnt, nconcount);
            nconcount = 0;
        }
    }





    /* Counter for positions in alignment. */
    for(post_cnt = 0; post_cnt < alg->width; post_cnt++)
    {
        if(((ajStrGetCharPos(alg->Post_similar, post_cnt) == '1') 
	    && ((*scores)->filterpsim == ajTrue)) || 
           (((*scores)->filterpsim == ajFalse) 
	    && ((ajStrGetCharPos(alg->Post_similar, post_cnt) != '-'))))
        {
            /* Extract ncon for residues in alignment at that position. */
            for(memb_cnt = 0; memb_cnt < alg->N; memb_cnt++)
            {
                /*Check to see if alignment position is a gap*/
                if((idx_seqpos=ajUint2dGet(seq_pos, memb_cnt, post_cnt))==-1)
                    continue;
                
                /* Assign position of atom_idx array to idx_atomidx. */
                else
                    idx_atomidx = ajUintGet(atom_idx[memb_cnt], idx_seqpos);
                                
                /* If noca is true, increment counter into align_ncon array by 1  
		   to take into account extra 'residue' not present in alignment. */
                if(noca[memb_cnt] == ajTrue)
                {
                    nconpos_cnt += ajUint2dGet(align_ncon, memb_cnt, idx_atomidx);
                }
                
                /* Else continue as normal. */
                else
                {
                    nconpos_cnt += ajUint2dGet(align_ncon, memb_cnt, 
					      (idx_atomidx - 1));
                }
            }


            /* Divide total ncon at the position by no. of sequences in alignment. */
            av_ncon = (float)nconpos_cnt/(float)alg->N;

            nconpos_cnt = 0;

            
            /* Check if av_ncon value is greater the conthresh. */
            if((av_ncon > (float)conthresh) || (av_ncon == (float)conthresh))
            {
                /* Assign '1' into position post_cnt of ncon_thresh array. */
                ajUintPut(&(*scores)->ncon_thresh, post_cnt, 1);
                con_counter++;
            }

            /* Else put '0' into array at position post_cnt. */
            else
                ajUintPut(&(*scores)->ncon_thresh, post_cnt, 0);
        }
    }


    /* Code to test that sufficient number of positions have > conthresh to
       enable a signature to be generated.  If not return ajFalse. */
    /* Check 3 or more residues are above conthresh or exit. */
    if(con_counter < 3)
    {
        ajFmtPrint("There are not sufficient residues making greater than "
		   "threshold number of contacts.\nExiting......signature "
		   "not generated\n");
        ajExit();
        return(0);
    }
    


    /* Free memory for matrix and iterator. */
    ajUint2dDel(&align_ncon);
    ajStrIterDel(&iter);


    /* Return. */
    return ajTrue;
}




/* @funcstatic  siggen_SigSelectSeq *******************************************
**
** Read Scopalg and Scorealg objects and returns a pointer to a Signature
** object. The signature generated is derived from a number, determined
** by the sparsity of the signature, of the top-scoring positions in the 
** alignment
**
** @param [r] alg        [AjPScopalg]  Alignment 
** @param [r] scores     [AjPScorealg] Scores for alignment
** @param [r] seq_pos    [AjPUint2d]    Index for alignment
** @param [r] sig_sparse [ajint]       Sparsity of signature
** @param [r] spar_check [float *]     Sparsity check array.
** @param [r] wsiz       [ajint]       Window size 
** @return [EmbPSignature] Pointer to Signature structure or NULL on failure
** @@
******************************************************************************/
static EmbPSignature  siggen_SigSelectSeq(AjPScopalg alg, 
					 AjPScorealg scores, 
					 AjPUint2d seq_pos,
					 ajint sig_sparse,
					 float *spar_check, 
					 ajint wsiz)
{
    ajint       nseqs=0;                /* Number of sequences */
    ajint       idx              =0;    /* Index */
    ajint       cnt              =0;    /* Counter for post_similar line. */
    ajint       first            =0;    /* Counter. */
    ajint       x                =0;    /* Counter. */
    ajint       memb_cnt         =0;    /* Counter. */
    ajint       rand_cnt         =0;    /* Counter. */
    ajint       j                =0;    /* Counter. */
    ajint       i                =0;    /* Counter. */
    ajint       av_temp          =0;    /* Counter. */
    float       total_temp       =0;    /* Counter. */
    ajint       imax             =0;    /* Loop exit condition. */
    ajint       hold_pos         =0;    /* Counter to hold values whilst sorting. */

    ajint       res_count        =0;    /* Counter for total number of residues. */
    ajint       num_aligned      =0;    /* Number of positions scoring '1' in 
					   alignment. */
    ajint       single_rescount  =0;
    ajint       gsiz             =0;    /* Temp. size of gap*/
    ajint       npos             =0;    /* Counter of signature positions*/
    ajint       sig_npos         =0;    /* Number of positions in signature. */
    float       val              =0.0;  /*Temp. val*/
    float       seqmat_min       =0;    /* Variable for minimum seq subn score. */
    float       seqmat_max       =0;    /* Variable for maximum seq subn score. */
    float       seqvar_min       =0;    /* Variable for minimum seq var score. */
    float       seqvar_max       =0;    /* Variable for maximum seq var score. */
    float       hold             =0;    /* Counter to hold values whilst sorting. */
    AjBool      done             =ajFalse;  /*Flag*/
    AjBool      finish           =ajFalse;  /*Flag*/
    AjBool      already_done     =ajFalse;  /*Flag*/
    AjPFloat    seqmat_normal    =NULL; /* Array of floats for normalised seqmat_score. */
    AjPFloat    seqvar_normal    =NULL; /* Array of floats for normalised seqvar_score. */
    AjPFloat    ncon_normal      =NULL; /* Array of floats for normalised ncon_score. */
    AjPFloat    ccon_normal      =NULL; /* Array of floats for normalised ccon_score. */
    AjPFloat    total_score      =NULL; /* Array of floats for total normalised scores. */
    AjPUint      keyres_pos       =NULL; /* Total_score array, sorted in ascending order. */
    AjPUint      post_sim         =NULL; /* Array for positions in post_similar line. */
    AjPUint      seq_len          =NULL; /* Array of length of each seq, as in alignment . */
    AjPUint      rand_pos         =NULL; /* Array of positions for random selection . */
    AjPUint      temp_rand        =NULL; /* Array of positions for random selection . */
    AjPUint2d    keyres_seq       =NULL; /* Total_score array, sorted in ascending order. */
    AjPStr      *seq_array       =NULL; /* Arrays of sequence (w/o gaps) from alignment. */
    EmbPSignature sig=NULL;              /* Signature. */
    float       spar             =0.0;
    
    double      rn               = 0;   
    int         irn              = 0; 
    ajint       len              = 0; 

    /* New stuff for randomising. */
    ajint   rand_num=0;
    ajint   randpos_cnt=0;
    ajint   rand_total=0;
    ajint   p=0;
    ajint   new_gsiz=0;
    


    /* Check args. */
    if(!alg || !scores || !seq_pos || !sig_sparse)
    {
        ajWarn("Null args passed to siggen_SigSelect");
        return NULL;
    }
    
    /* Seed the number generator. */
    ajRandomSeed();
    
    /* Create arrays. */
    seqmat_normal       =  ajFloatNewRes(alg->width);
    seqvar_normal       =  ajFloatNewRes(alg->width);
    ncon_normal         =  ajFloatNewRes(alg->width);
    ccon_normal         =  ajFloatNewRes(alg->width);
    total_score         =  ajFloatNewRes(alg->width);
    keyres_pos          =  ajUintNewRes(alg->width);    


    post_sim            =  ajUintNewRes(alg->width);
    rand_pos            =  ajUintNewRes(alg->width);
    keyres_seq          =  ajUint2dNew();    
    seq_len             =  ajUintNewRes(alg->N);    
    

    /* Initialise array elements to zero. */
    ajFloatPut(&seqmat_normal, alg->width-1, (float) 0.0);

    ajFloatPut(&seqvar_normal, alg->width-1, (float) 0.0);
    ajFloatPut(&ncon_normal, alg->width-1, (float) 0.0);
    ajFloatPut(&ccon_normal, alg->width-1, (float) 0.0);
    ajFloatPut(&total_score, alg->width-1, (float) 0.0);

    ajUintPut(&keyres_pos, alg->width-1, (ajint) 0);
    ajUintPut(&post_sim, alg->width-1, (ajint) 0);
    ajUintPut(&rand_pos, alg->width-1, (ajint) 0);

    nseqs=ajDmxScopalgGetseqs(alg, &seq_array);

    /* Start of main application loop. */
    /* Determine min and max values of seqmat_score. 
       Iterate through (scores)->seqmat_score array. */

    if((scores)->seqmat_do == ajTrue)
    {
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Check if value is not zero. */
            if(ajFloatGet((scores)->seqmat_score, cnt) != 0)
            {

                /* If in loop for first time assign min and max. */

                if(first == 0)
                {
                    seqmat_min = ajFloatGet((scores)->seqmat_score, cnt);
                    seqmat_max = ajFloatGet((scores)->seqmat_score, cnt);
                    first = 1;
                }


                /* Check if value is >, <, = to min/max */
                /* If true assign value to min/max variable. */
                if(ajFloatGet((scores)->seqmat_score, cnt) < seqmat_min)
                    seqmat_min = ajFloatGet((scores)->seqmat_score, cnt);
                if(ajFloatGet((scores)->seqmat_score, cnt) == seqmat_min)
                    continue;
                if(ajFloatGet((scores)->seqmat_score, cnt) == seqmat_max)
                    continue;
                if(ajFloatGet((scores)->seqmat_score, cnt) > seqmat_max)
                    seqmat_max = ajFloatGet((scores)->seqmat_score, cnt);
            }
        }
    }

    /* Start of main application loop */
    /* Determine min and max values of seqvar_score 
       Iterate through (scores)->seqvar_score array. */
    if((scores)->seqvar_do == ajTrue)
    {
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Check if value is not zero. */
            if(ajFloatGet((scores)->seqvar_score, cnt) != 0)
            {
                /* If in loop for first time assign min and max. */
                if(first == 0)
                {
                    seqvar_min = ajFloatGet((scores)->seqvar_score, cnt);
                    seqvar_max = ajFloatGet((scores)->seqvar_score, cnt);

                    first = 1;
                }



                /* Check if value is >, <, = to min/max */
                /* If true assign value to min/max variable. */
                if(ajFloatGet((scores)->seqvar_score, cnt) < seqvar_min)
                    seqvar_min = ajFloatGet((scores)->seqvar_score, cnt);
                if(ajFloatGet((scores)->seqvar_score, cnt) == seqvar_min)
                    continue;
                if(ajFloatGet((scores)->seqvar_score, cnt) == seqvar_max)
                    continue;

                if(ajFloatGet((scores)->seqvar_score, cnt) > seqvar_max)
                    seqvar_max = ajFloatGet((scores)->seqvar_score, cnt);
            }
        }
    }





    /* Perform normalising calculation on seqmat_score array. */
    if((scores)->seqmat_do == ajTrue)
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Only perform nornalisation on Post_similar */
            /* positions with value '1' or '0', if filter = ajFalse. */

	    if(((ajStrGetCharPos(alg->Post_similar, cnt) == '1') && 
		((scores)->filterpsim == ajTrue) &&
		((scores)->filtercon == ajTrue) && 	
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajTrue) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) == '1') &&
		((scores)->filtercon == ajFalse))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajFalse)))
            {   
                /* Put normalised score into array at eqiuvalent position. */
                ajFloatPut(&seqmat_normal, cnt,
			   ((ajFloatGet((scores)->seqmat_score, cnt) 
			     - seqmat_min)/(seqmat_max - seqmat_min)));
            }	
            else
                ajFloatPut(&seqmat_normal, cnt, 0);
        }


    /* Perform normalising calculation on seqvar_score array. */
    if((scores)->seqvar_do == ajTrue)
        for(cnt =0; cnt <alg->width; cnt++)
        {
            /* Only perform nornalisation on Post_similar positions with 
	       value '1'. */


	    if(((ajStrGetCharPos(alg->Post_similar, cnt) == '1') && 
		((scores)->filterpsim == ajTrue) &&
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajTrue) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) == '1') &&
		((scores)->filtercon == ajFalse))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajTrue) && 
		(ajUintGet((scores)->ncon_thresh, cnt) == 1))
	       ||
	       (((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, cnt) != '-') && 
		((scores)->filtercon == ajFalse)))
            {   
                /* Put normalised score into array at eqiuvalent position. */
                ajFloatPut(&seqvar_normal, cnt,
			   ((ajFloatGet((scores)->seqvar_score, cnt) 
			     - seqvar_min)/(seqvar_max - seqvar_min)));

            }

            else

                ajFloatPut(&seqvar_normal, cnt, 0);
        }
    
 


    
    
    /* Add total scores for each position depending on the scoring options 
       selected.  Enter value in total_score array. */

    for(x=0; x<alg->width; x++)
    {
        val=0.0;
        if((scores)->seqmat_do == ajTrue) 
            val+=ajFloatGet(seqmat_normal, x);

        if((scores)->seqvar_do == ajTrue) 
            val+=ajFloatGet(seqvar_normal, x);


        ajFloatPut(&total_score, x, val);

    }
    
    /* Initialise array elements from 0 to alg->width.  Count number of 
       potential signature positions depending on the scoring methods 
       selected. */
    num_aligned = -1;
    
    for(x=0; x<alg->width; x++)
    {
        /* initialise post_sim array to zero. */
        ajUintPut(&post_sim, x, x);
        
	/* New stuff for randomising. */
        ajUintPut(&rand_pos, j++, x);
	rand_num++;
	

        if(((scores)->filterpsim == ajTrue) && 
		(ajStrGetCharPos(alg->Post_similar, x) == '1') &&
           ((scores)->filtercon == ajFalse))
	    {
		num_aligned++;
	    }
        else if(((scores)->filterpsim == ajFalse) && 
		(ajStrGetCharPos(alg->Post_similar, x) != '-') && 
		((scores)->filtercon == ajFalse))
	{
            num_aligned++;
	}
    }    



    /* Perform bubble sort of total_score array . */ 
    for(i=1; i<=(alg->width-1); i++)
        for(j=0; j<=(alg->width-2); j++)
        { 
            /* Check if element j is > than element j+1. */
            if((ajFloatGet(total_score, j)) > 
               (ajFloatGet(total_score, (j + 1))))
            {
                /* Swap elements j and j+1 via hold/hold_pos variables. */
                hold = ajFloatGet(total_score, j);

                hold_pos = ajUintGet(post_sim, j);
                ajFloatPut(&total_score, j, (ajFloatGet(total_score, (j+1))));

                ajUintPut(&post_sim, j, (ajUintGet(post_sim, (j+1))));
                ajFloatPut(&total_score, (j+1), hold);
                ajUintPut(&post_sim, (j+1), hold_pos);
            }           

        }

    /* Determine average number of residues for each sequence from ALIGNMENT. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
    {
        single_rescount = 0;
        for(i=0; i<alg->width; i++)
	  if(ajStrGetCharPos(alg->Seqs[memb_cnt], i) != '-')  /* Andy */
            {
                single_rescount++;
            }

        ajUintPut(&seq_len, memb_cnt, single_rescount);
    }

    /* Determinine actual length of each sequence from dimensions of cmap, NOT 
       the STAMP alignment. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
    {
	len = ajUintGet(seq_len, memb_cnt);
	
	res_count += len;
    }
    

    /* Initialise keyres_seq array to zero. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
        for(i=0; i<ajUintGet(seq_len, memb_cnt); i++)
            ajUint2dPut(&keyres_seq, memb_cnt, i, 0);


    
    /* Calculate the number of positions the signature should have. 
       Determine no. of positions the signature will have. */
    sig_npos = (ajint) ceil((double)  ((res_count/alg->N) * 
                                       ( (float)sig_sparse/(float)100)));


    for(i=((alg->width)-1); i>((alg->width)-sig_npos); i--)
    {
        if(ajFloatGet(total_score, i) == 0)
            printf("Total_score array = 0.00 at position = %d\n", i);
    }


    /* Do random number generating here and fill post_sim array from the end 
       with the random numbers which equate to the putative signature 
       positions identified above. */

    /* Assign temp array and initialise. */
    temp_rand =  ajUintNewRes(num_aligned);
    for(i=0;i<num_aligned;i++)
        ajUintPut(&temp_rand, i, -1);
    
    if(scores->random == ajTrue)
    {    
        while(finish == ajFalse)
        {
            /* Generate random number. */
            rn = ajRandomDouble();


	    irn = (int) (rn * (double)rand_num);

            already_done = ajFalse;
            
            /* Check number hasn't already been put into array. */
            for(i=0;i<rand_num;i++) 
            {
                if(ajUintGet(temp_rand, i) == irn)
                {
                    already_done = ajTrue;
                    break;
                }       
            
                else
                    continue;
            }

            /* If number not already in array, then put it in. */
            if(already_done == ajFalse)
                ajUintPut(&temp_rand, rand_cnt++, irn);

            /* Number already in array so continue and choose another. */
            else
                continue;

            /* If all array elements filled then finish while loop. */
            if(rand_cnt == rand_num) 
                finish = ajTrue;
        }
    
        /* Diagnostic to sort temp_rand and check all number are present. */
        /*for(i=0;i<num_aligned;i++)
          ajSortIntInc((ajint *) ajUintInt(temp_rand), num_aligned);
          
          printf("sorted temp_rand\n");
          for(i=0;i<num_aligned;i++)
          ajFmtPrint("%4d\n", ajUintGet(temp_rand, i));*/
        
        
        /* Copy the random numbers to the last 'num_aligned' positions */
        /* of the post_sim array. */
        for(i=0;i<num_aligned;i++)
            ajUintPut(&post_sim, ((alg->width) - i), 
		     ajUintGet(rand_pos, ajUintGet(temp_rand, i)));
        
    }



    /* Check if the user entered sparsity exceeds the number of positions 
       which are aligned in the alignment. */
    if(num_aligned < sig_npos)
    {
        /* Calculate sparsity for number of aligned positions. */
        av_temp = (ajint) (res_count/alg->N);
        total_temp = (((float)num_aligned/(float)av_temp) * (float)100);

        ajFmtPrint("There are not sufficient aligned positions to generate a "
		   "signature of the specified sparsity.\nSignature sparsity = %.2f\n", 
		   total_temp);

	if(total_temp < 1)
	{
	    /* Tidy up. */
	    ajFloatDel(&seqmat_normal);
	    ajFloatDel(&seqvar_normal);
	    ajFloatDel(&ncon_normal);
	    ajFloatDel(&ccon_normal);
	    ajFloatDel(&total_score);
	    ajUintDel(&keyres_pos);   
	    ajUintDel(&post_sim);     
	    ajUintDel(&rand_pos);     
	    ajUintDel(&temp_rand);     
	    ajUint2dDel(&keyres_seq);
	    ajUintDel(&seq_len);
	    for(x=0;x<nseqs;x++)
		ajStrDel(&seq_array[x]);
	    AJFREE(seq_array);

	    return NULL;
	}

        spar = total_temp;
        (*spar_check) = (ceil((double)spar));

        /* 
	 ** Generate signature using all of the aligned positions scoring '1'
	 ** Begin at end i.e. highest scoring positions of post_sim array 
	 ** Count down from end until all '1' positions are entered in
	 ** keyres_pos array. 
	 */
        for(i=alg->width-1; i>=alg->width-(num_aligned); i--)
        {
            /* Put a '1' in keyres_pos array at position corresponding to 
	       position in post_sim array. */
            ajUintPut(&keyres_pos, ajUintGet(post_sim, i), 1);

        }
    }
    
    /* Else output signature of sparsity specified by user. */
    else
    {
        /* Begin at end i.e. highest scoring positions of post_sim array. */
        /* Count down from end until sparsity is fulfilled. */
        for(i=alg->width-1; i>=alg->width-(sig_npos); i--)
        {
            /* Put a '1' in keyres_pos array at position corresponding to 
	       position in post_sim array. */
            ajUintPut(&keyres_pos, ajUintGet(post_sim, i), 1);
        }
    }    


    /* Fill 2d array with positions of each key res w.r.t. alignment seq. */
    if(scores->random ==ajTrue)
    {
	for(i=0; i<alg->width; i++)
	{
	    if(ajUintGet(keyres_pos, i) == 1)
	    {
		randpos_cnt++;
		
		for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)            
		{
		    /*Check to see if alignment position is a gap*/
		    if((idx=ajUint2dGet(seq_pos, memb_cnt, i))==-1)
			continue;
		    else
			ajUint2dPut(&keyres_seq, randpos_cnt, idx, 1); 
		}
	    }
	}
    }
    else
    {
    	/* Fill 2d array with positions of each key res w.r.t. alignment seq. */
	for(i=0; i<alg->width; i++)
	{
	    if(ajUintGet(keyres_pos, i) == 1)
	    {
		for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)            
		{
		    /* Check to see if alignment position is a gap. */
		    if((idx=ajUint2dGet(seq_pos, memb_cnt, i))==-1)
			continue;
		    else
			ajUint2dPut(&keyres_seq, memb_cnt, idx, 1); 
		}
	    }
	}
    }

    rand_total=randpos_cnt;
    

    /* Allocate signature structure and write SCOP classification records. 
       If not enough aligned positions then make structure size num_aligned. */
    if(num_aligned < sig_npos)
            sig =  embSignatureNew(num_aligned);
    else
        sig =  embSignatureNew(sig_npos);

    sig->Type = alg->Type;
    ajStrAssignRef(&sig->Class, alg->Class);
    ajStrAssignRef(&sig->Architecture, alg->Architecture);
    ajStrAssignRef(&sig->Topology, alg->Topology);
    ajStrAssignRef(&sig->Fold, alg->Fold);
    ajStrAssignRef(&sig->Superfamily, alg->Superfamily);
    ajStrAssignRef(&sig->Family, alg->Family);
    sig->Sunid_Family =  alg->Sunid_Family;

    /* Allocate sigdat structures, don't know the number of residue or gaps in 
       advance, these are set below, but set the window size here. */
    for(x=0; x<sig->npos; x++)
    {
	sig->dat[x]=embSigdatNew(0, 0);
	sig->dat[x]->wsiz=wsiz;
    }
    

    /* Write signature structure */
    if(scores->random==ajTrue)
    {
    
	/* Write signature structure. */
	for(j=0; j<alg->N; j++)
	{			
	    randpos_cnt = 1;
	    for(npos=0, gsiz=0, i=0, imax=ajUintGet(seq_len, j); 
		i<imax; 
		i++)       
	    {

		/* The position is a signature position. */
		if(ajUint2dGet(keyres_seq, j, i) == randpos_cnt)
		{
		    /*Process gap. */
		    for(done=ajFalse,x=0;x<sig->dat[randpos_cnt-1]->ngap;x++)
		    {
			/*The gap length is NOT new*/
			if((ajUintGet(sig->dat[randpos_cnt-1]->gsiz, x)==gsiz))
			{
			    ajUintInc(&sig->dat[randpos_cnt-1]->gfrq, x);
			    done=ajTrue;
			    break;

			}   
		    }

                

		    /*The gap length is new*/
		    if(!done)
		    {
			sig->dat[randpos_cnt-1]->ngap++;
			ajUintPut(&sig->dat[randpos_cnt-1]->gsiz, 
				 sig->dat[randpos_cnt-1]->ngap-1, 
				 gsiz);
			ajUintPut(&sig->dat[randpos_cnt-1]->gfrq, 
				 sig->dat[randpos_cnt-1]->ngap-1, 
				 (ajint) 1);
		    }    


		    gsiz=0;
		    npos++;
		    randpos_cnt++;
		}



		else if(ajUint2dGet(keyres_seq, j, i) > randpos_cnt)
		{
		    rand_total = ajUint2dGet(keyres_seq, j, i);
		    

		    for(p=1;p<100;p++)
		    {
			printf("randpos_cnt = %d p = %d\n", randpos_cnt, p);
			
			if((randpos_cnt + p) == rand_total)
			{	
			    new_gsiz = (int)(gsiz/(p+1));
			    printf("old_ gsiz = %d new_gsiz = %d\n", 
				   gsiz, new_gsiz);
			    break;
			}
		    }
		    

		    /*Process gap. */
		    for(done=ajFalse,x=0;x<sig->dat[rand_total-1]->ngap;x++)
		    {
			/*The gap length is NOT new*/
			if((ajUintGet(sig->dat[rand_total-1]->gsiz, x)==new_gsiz))
			{
			    ajUintInc(&sig->dat[rand_total-1]->gfrq, x);
			    done=ajTrue;
			    break;

			}   
		    }

                

		    /*The gap length is new*/
		    if(!done)
		    {
			sig->dat[rand_total-1]->ngap++;
			ajUintPut(&sig->dat[rand_total-1]->gsiz, 
				 sig->dat[rand_total-1]->ngap-1, 
				 new_gsiz);
			ajUintPut(&sig->dat[rand_total-1]->gfrq, 
				 sig->dat[rand_total-1]->ngap-1, 
				 (ajint) 1);
		    }    


		    /* Set variables*/
		    gsiz=0;
		    npos++;
		    randpos_cnt = ajUint2dGet(keyres_seq, j, i);
		    randpos_cnt++;
		}


		else
		{
		    gsiz++;
		}
	    }
	}




	/* Write residue id into signature structure. */
	for(j=0; j<alg->N; j++)
	{
	    randpos_cnt = 1;
	    
	    for(npos=0, i=0, imax=ajUintGet(seq_len, j); 
		i<imax; 
		i++)       
	    {
		/* The position is a signature position. */
		if(ajUint2dGet(keyres_seq, j, i) == randpos_cnt)
		{
		    /* Process residue. */
		    for(done=ajFalse,x=0;x<sig->dat[randpos_cnt-1]->nres;x++)
			/* The residue id is NOT new. */
		    {
			if((ajChararrGet(sig->dat[randpos_cnt-1]->rids, x))
			   ==ajStrGetCharPos(seq_array[j], i))
			{
			    ajUintInc(&sig->dat[randpos_cnt-1]->rfrq, x);
			    done=ajTrue;
			    break;
			}
		    }


		    /* The residue id is new. */
		    if(!done)
		    {
			sig->dat[randpos_cnt-1]->nres++;
			ajChararrPut(&sig->dat[randpos_cnt-1]->rids, 
				     sig->dat[randpos_cnt-1]->nres-1, 	
				     ajStrGetCharPos(seq_array[j], i));

			ajUintPut(&sig->dat[randpos_cnt-1]->rfrq, 
				 sig->dat[randpos_cnt-1]->nres-1, 
				 (ajint) 1);
		    }    
                

		    npos++;
		    randpos_cnt++;
		}

		else if(ajUint2dGet(keyres_seq, j, i) > randpos_cnt)
		{
		    rand_total = ajUint2dGet(keyres_seq, j, i);
		    /*Process residue*/
		    for(done=ajFalse,x=0;x<sig->dat[rand_total-1]->nres;x++)
			/*The residue id is NOT new*/
		    {
			if((ajChararrGet(sig->dat[rand_total-1]->rids, x))
			   ==ajStrGetCharPos(seq_array[j], i))
			{
			    ajUintInc(&sig->dat[rand_total-1]->rfrq, x);
			    done=ajTrue;
			    break;
			}
		    }


		    /*The residue id is new*/
		    if(!done)
		    {
			sig->dat[rand_total-1]->nres++;
			ajChararrPut(&sig->dat[rand_total-1]->rids, 
				     sig->dat[rand_total-1]->nres-1, 	
				     ajStrGetCharPos(seq_array[j], i));

			ajUintPut(&sig->dat[rand_total-1]->rfrq, 
				 sig->dat[rand_total-1]->nres-1, 
				 (ajint) 1);
		    }    
                

		    npos++;
		    randpos_cnt = ajUint2dGet(keyres_seq, j, i);
		    randpos_cnt++;
		}

		else
		{
		    continue;
		}
	    }
	}
    }
    else
    {
	/* Write signature structure. */
	for(j=0; j<alg->N; j++)
	{				
	    for(npos=0, gsiz=0, i=0, imax=ajUintGet(seq_len, j); 
		i<imax; 
		i++)       
	    {

		/* The position is a signature position. */
		if(ajUint2dGet(keyres_seq, j, i) == 1)
		{
		    /*Process gap. */
		    for(done=ajFalse,x=0;x<sig->dat[npos]->ngap;x++)
		    {
			/*The gap length is NOT new*/
			if((ajUintGet(sig->dat[npos]->gsiz, x)==gsiz))
			{
			    ajUintInc(&sig->dat[npos]->gfrq, x);
			    done=ajTrue;
			    break;

			}   
		    }

                

		    /*The gap length is new*/
		    if(!done)
		    {
			sig->dat[npos]->ngap++;
			ajUintPut(&sig->dat[npos]->gsiz, sig->dat[npos]->ngap-1, 
				 gsiz);
			ajUintPut(&sig->dat[npos]->gfrq, sig->dat[npos]->ngap-1, 
				 (ajint) 1);
		    }    


		    gsiz=0;
		    npos++;
		}
		else
		{
		    gsiz++;
		}
	    }

	}



	/* Write residue id into signature structure. */
	for(j=0; j<alg->N; j++)
	{
	    /* seq_len changed to fullseq_len. */
	    for(npos=0, i=0, imax=ajUintGet(seq_len, j); 
		i<imax; 
		i++)       
	    {
		/* The position is a signature position. */
		if(ajUint2dGet(keyres_seq, j, i) == 1)
		{
		    /*Process residue*/
		    for(done=ajFalse,x=0;x<sig->dat[npos]->nres;x++)
			/*The residue id is NOT new*/
		    {
			if((ajChararrGet(sig->dat[npos]->rids, x))
			   ==ajStrGetCharPos(seq_array[j], i))
			{
			    ajUintInc(&sig->dat[npos]->rfrq, x);
			    done=ajTrue;
			    break;
			}
		    }


		    /*The residue id is new*/
		    if(!done)
		    {
			sig->dat[npos]->nres++;
			ajChararrPut(&sig->dat[npos]->rids, sig->dat[npos]->nres-1,         
				     ajStrGetCharPos(seq_array[j], i));

			ajUintPut(&sig->dat[npos]->rfrq, sig->dat[npos]->nres-1, 
				 (ajint) 1);
		    }    
                

		    npos++;
		}
		else
		    continue;
	    }
	}

    }   




    

  
    /* Perform bubble sort of gap sizes and frequencies array . */ 
    for(x=0; x<sig->npos;x++) 
    { 
        for(cnt=1; cnt<(sig->dat[x]->ngap);cnt++)
            for(i=0; i<(sig->dat[x]->ngap-1);i++)
            {    
                /* Check if element i is > than element i+1. */
                if(ajUintGet(sig->dat[x]->gsiz, i) > ajUintGet(sig->dat[x]->gsiz, i+1))
                {       
                    /* Swap elements i and i+1 via y/hold_pos variables. */
                    j        = ajUintGet(sig->dat[x]->gsiz, i);
                    hold_pos = ajUintGet(sig->dat[x]->gfrq, i);

                    ajUintPut(&sig->dat[x]->gsiz, i, 
			     ajUintGet(sig->dat[x]->gsiz, i+1));
                    ajUintPut(&sig->dat[x]->gfrq, i, 
			     ajUintGet(sig->dat[x]->gfrq, i+1));
                
                    ajUintPut(&sig->dat[x]->gsiz, i+1, j);
                    ajUintPut(&sig->dat[x]->gfrq, i+1, hold_pos);
                }               
            }
    }
    


    /* Memory management. */
    ajFloatDel(&seqmat_normal);
    ajFloatDel(&seqvar_normal);
    ajFloatDel(&ncon_normal);
    ajFloatDel(&ccon_normal);
    ajFloatDel(&total_score);
    ajUintDel(&keyres_pos);   
    ajUintDel(&post_sim);     
    ajUintDel(&rand_pos);     
    ajUintDel(&temp_rand);     
    ajUint2dDel(&keyres_seq);
    ajUintDel(&seq_len);
    for(x=0;x<nseqs;x++)
        ajStrDel(&seq_array[x]);
    AJFREE(seq_array);

    return sig;
}




/* @funcstatic siggen_SigSelectManual ***************************************
**
** Convenience routine - calls several functions to score an alignment.
**
** @param [r] alg        [AjPScopalg ]   Alignment
** @param [w] scores     [AjPScorealg*]  Scores for alignment
** @param [r] seq_pos    [AjPUint2d]      Index for alignment
** @param [w] spar_check [float *]       Sparsity of signature
** @param [r] wsiz       [ajint]         Window size 
** 
** @return [AjBool] True on succcess
** @@
****************************************************************************/
static EmbPSignature siggen_SigSelectManual(AjPScopalg alg, 
					   AjPScorealg *scores, 
					   AjPUint2d seq_pos,
					   float *spar_check,
					   ajint wsiz)
{ 
    ajint       nseqs=0;            /* Number of sequences.                */
    ajint       idx          =0;    /* Index.                              */
    ajint       cnt          =0;    /* Counter for post_similar line.      */
    ajint       x            =0;    /* Counter.                            */
    ajint       memb_cnt     =0;    /* Counter.                            */
    ajint       j            =0;    /* Counter.                            */
    ajint       i            =0;    /* Counter.                            */
    ajint       imax         =0;    /* Loop exit condition.                */
    ajint       hold_pos     =0;    /* Counter to hold values whilst sorting. */

    ajint   single_rescount  =0;
    ajint       gsiz         =0;    /* Temp. size of gap.                  */
    ajint       npos         =0;    /* Counter of signature positions.     */
    ajint       sig_npos     =0;    /* Number of positions in signature.   */
    float       val          =0.0;  /* Temp. val.                          */
    AjBool      done         =ajFalse;  /*Flag.                            */
    AjPFloat    total_score  =NULL; /* Total normalised scores.            */
    AjPUint      keyres_pos   =NULL; /* Total_score array, sorted in ascending order.*/
    AjPUint      post_sim     =NULL; /* Positions in post_similar line.     */
    AjPUint      seq_len      =NULL; /* Length of each seq, as in alignment.*/
    AjPUint      fullseq_len  =NULL; /* Length of each seq, as in original seq . */
    AjPUint2d    keyres_seq   =NULL; /* Total_score array, sorted in ascending order. */
    AjPStr      *seq_array   =NULL; /* Sequence (w/o gaps) from alignment. */
    EmbPSignature sig=NULL;          /* Signature.                          */

    




    /*Check args. */
    if( !(*scores) || !alg || !seq_pos)
    {
        printf("Bad args passed to siggen_SigSelectManual\n");
        return NULL;
    }


    
    /* Create arrays. */
    total_score         =  ajFloatNewRes(alg->width);
    keyres_pos          =  ajUintNewRes(alg->width);    


    post_sim            =  ajUintNewRes(alg->width);
    keyres_seq          =  ajUint2dNew();    
    seq_len             =  ajUintNewRes(alg->N);    
    fullseq_len         =  ajUintNewRes(alg->N);    
    

    /* Initialise array elements to zero. */
    ajFloatPut(&total_score, alg->width-1, (float) 0.0);

    ajUintPut(&keyres_pos, alg->width-1, (ajint) 0);
    ajUintPut(&post_sim, alg->width-1, (ajint) 0);

    nseqs=ajDmxScopalgGetseqs(alg, &seq_array);

    /* Start of main application loop. */
    /* Determine min and max values of seqmat_score. Iterate through 
       (scores)->seqmat_score array. Add total scores for each position 
       depending on the scoring options selected.  Enter value in 
       total_score array. */

    for(x=0; x<alg->width; x++)
    {
        val=0.0;
        ajFloatPut(&total_score, x, val);
    }
    


    /* Perform bubble sort of total_score array . */ 
    for(i=0; i<alg->width; i++)
	/* Check manual position line is 1. */	
	if(ajStrGetCharPos(alg->Positions, i)=='1')
	    ajUintPut(&post_sim, alg->width-i, i);


    /* Determine average number of residues for each sequence from ALIGNMENT. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
    {
        single_rescount = 0;
        for(i=0; i<alg->width; i++)
	  if(ajStrGetCharPos(alg->Seqs[memb_cnt], i) != 'X')  /* Andy */
                single_rescount++;

        ajUintPut(&seq_len, memb_cnt, single_rescount);
    }


    /* Initialise keyres_seq array to zero. */
    for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)
        for(i=0; i<ajUintGet(seq_len, memb_cnt); i++)
            ajUint2dPut(&keyres_seq, memb_cnt, i, 0);

    
    /* Calculate the number of positions the signature has. */
    for(sig_npos=0,i=0; i<alg->width; i++)
	/* Check manual position line is 1. */	
	if(ajStrGetCharPos(alg->Positions, i)=='1')
	    sig_npos++;
 
    for(i=((alg->width)-1); i>((alg->width)-sig_npos); i--)
        if(ajFloatGet(total_score, i) == 0)
            printf("Total_score array = 0.00 at position = %d\n", i);


    /* Output signature of sparsity specified by user. */
 
    /* Begin at end i.e. highest scoring positions of post_sim array. Count 
       down from end until sparsity is fulfilled. */
    for(i=alg->width-1; i>=alg->width-(sig_npos); i--)
    {
	/* Put a '1' in keyres_pos array at position corresponding to position 
	   in post_sim array. */
	ajUintPut(&keyres_pos, ajUintGet(post_sim, i), 1);
    }
     

    /* Fill 2d array with positions of each key res w.r.t. alignment seq. */
    for(i=0; i<alg->width; i++)
    {
        if(ajUintGet(keyres_pos, i) == 1)
        {
            for(memb_cnt=0; memb_cnt<alg->N; memb_cnt++)            
            {
                /* Check to see if alignment position is a gap. */
                if((idx=ajUint2dGet(seq_pos, memb_cnt, i))==-1)
                    continue;
                else
                    ajUint2dPut(&keyres_seq, memb_cnt, idx, 1); 
            }
        }
    }




    /* Allocate signature structure and write SCOP classification records. If 
       not enough aligned positions then make structure size num_aligned. */
    sig =  embSignatureNew(sig_npos);

    sig->Type = alg->Type;
    ajStrAssignRef(&sig->Class, alg->Class);
    ajStrAssignRef(&sig->Architecture, alg->Architecture);
    ajStrAssignRef(&sig->Topology, alg->Topology);
    ajStrAssignRef(&sig->Fold, alg->Fold);
    ajStrAssignRef(&sig->Superfamily, alg->Superfamily);
    ajStrAssignRef(&sig->Family, alg->Family);
    sig->Sunid_Family =  alg->Sunid_Family;

    /* Allocate sigdat structures, don't know the number of residue or gaps in 
       advance, these are set below, but set the window size here. */
    for(x=0; x<sig->npos; x++)
    {
	sig->dat[x]=embSigdatNew(0, 0);
	sig->dat[x]->wsiz=wsiz;
    }
    


    /* Write signature structure. */
    for(j=0; j<alg->N; j++)
    {   
        for(npos=0, gsiz=0, i=0, imax=ajUintGet(fullseq_len, j); 
            i<imax; 
            i++)       
        {
            /* The position is a signature position. */
            if(ajUint2dGet(keyres_seq, j, i) == 1)
            {
                /*Process gap. */
                for(done=ajFalse,x=0;x<sig->dat[npos]->ngap;x++)
                {
                    /* The gap length is NOT new. */
                    if((ajUintGet(sig->dat[npos]->gsiz, x)==gsiz))
                    {
                        ajUintInc(&sig->dat[npos]->gfrq, x);
                        done=ajTrue;
                        break;

                    }   
                }

                
                /* The gap length is new. */
                if(!done)
                {
                    sig->dat[npos]->ngap++;
                    ajUintPut(&sig->dat[npos]->gsiz, sig->dat[npos]->ngap-1, 
                             gsiz);
                    ajUintPut(&sig->dat[npos]->gfrq, sig->dat[npos]->ngap-1, 
                             (ajint) 1);
                }    


                /* Set variables*/
                gsiz=0;
                npos++;
            }
            else
                gsiz++;
        }

    }





    /* Write residue id into signature structure. */
    for(j=0; j<alg->N; j++)
    {
        for(npos=0, i=0, imax=ajUintGet(seq_len, j); 
            i<imax; 
            i++)       
        {
            /* The position is a signature position. */
            if(ajUint2dGet(keyres_seq, j, i) == 1)
            {
                /* Process residue. */
                for(done=ajFalse,x=0;x<sig->dat[npos]->nres;x++)
                    /*The residue id is NOT new*/
                {
                    if((ajChararrGet(sig->dat[npos]->rids, x))
                       ==ajStrGetCharPos(seq_array[j], i))
                    {
                        ajUintInc(&sig->dat[npos]->rfrq, x);
                        done=ajTrue;
                        break;
                    }
                }


                /* The residue id is new. */
                if(!done)
                {
                    sig->dat[npos]->nres++;
                    ajChararrPut(&sig->dat[npos]->rids, sig->dat[npos]->nres-1,         
                                 ajStrGetCharPos(seq_array[j], i));

                    ajUintPut(&sig->dat[npos]->rfrq, sig->dat[npos]->nres-1, 
                            (ajint) 1);
                }    
                

                /* Set variables. */
                npos++;
            }
            else
                continue;
        }
    }
    
    


  
    /* Perform bubble sort of gap sizes and frequencies array. */ 
    for(x=0; x<sig->npos;x++) 
    { 
        for(cnt=1; cnt<(sig->dat[x]->ngap);cnt++)
            for(i=0; i<(sig->dat[x]->ngap-1);i++)
            {    
                /* Check if element i is > than element i+1. */
                if(ajUintGet(sig->dat[x]->gsiz, i) 
		   > ajUintGet(sig->dat[x]->gsiz, i+1))
                {       
                    /* Swap elements i and i+1 via y/hold_pos variables. */
                    j        = ajUintGet(sig->dat[x]->gsiz, i);
                    hold_pos = ajUintGet(sig->dat[x]->gfrq, i);

                    ajUintPut(&sig->dat[x]->gsiz, i, 
			     ajUintGet(sig->dat[x]->gsiz, i+1));
                    ajUintPut(&sig->dat[x]->gfrq, i, 
			     ajUintGet(sig->dat[x]->gfrq, i+1));
                
                    ajUintPut(&sig->dat[x]->gsiz, i+1, j);
                    ajUintPut(&sig->dat[x]->gfrq, i+1, hold_pos);
                }               
            }         
    }
    


      

    /* Tidy up. */
    ajFloatDel(&total_score);
    ajUintDel(&keyres_pos);   
    ajUintDel(&post_sim);     
    ajUint2dDel(&keyres_seq);
    ajUintDel(&seq_len);
    for(x=0;x<nseqs;x++)
        ajStrDel(&seq_array[x]);
    AJFREE(seq_array);

    return sig;
}





/* @funcstatic  siggen_ScoreAlignmentSeq **************************************
**
** Convenience routine - calls several functions to score an alignment.
**
** @param [w] scores  [AjPScorealg*]   Scores for alignment
** @param [r] alg     [AjPScopalg ]    Alignment
** @param [r] mat     [AjPMatrixf]     Subsitution matrix
** @param [r] seq_pos [AjPUint2d]       Index for alignment
**
** @return [AjBool] True on succcess
** @@
******************************************************************************/
static AjBool siggen_ScoreAlignmentSeq(AjPScorealg *scores, 
				       AjPScopalg alg, 
				       AjPMatrixf  mat, 
				       AjPUint2d seq_pos)
{
    /*Check args. */
    if( !(*scores) || !alg || !mat || !seq_pos)
    {
        printf("Bad args passed to siggen_ScoreAlignment\n");
        return ajFalse;
    }
    
    

    /* Call scoring functions as appropriate. */
    if((*scores)->seqmat_do)
        siggen_ScoreSeqMat(alg, scores, mat, seq_pos);
    

    if((*scores)->seqvar_do)      
        siggen_ScoreSeqVar(alg, scores, seq_pos);
    


    return ajTrue;
}






/* @funcstatic siggen_ScorealgNew *********************************************
**
** Scorealg object constructor. 
** Fore-knowledge of the length of the alignment is required.
**
** @param  [r] len [ajint]   Length of alignment
**
** @return [AjPScorealg] Pointer to a Scorealg object
** 
** @@
******************************************************************************/

static AjPScorealg siggen_ScorealgNew(ajint len)
{
    AjPScorealg ret = NULL;

    AJNEW0(ret);

    /* Create the scoring arrays. */
    if(len)
    {
	ret->seqmat_score    = ajFloatNewRes((ajint)len);
	ajFloatPut(&ret->seqmat_score, len-1, (float)0.0);
	ret->seqvar_score    = ajFloatNewRes((ajint)len);
	ajFloatPut(&ret->seqvar_score, len-1, (float)0.0);


        ret->ncon_thresh = ajUintNewRes((ajint)len);
	ajUintPut(&ret->ncon_thresh , len-1, (ajint)0);

	ret->post_similar = ajUintNewRes((ajint)len);
	ajUintPut(&ret->post_similar , len-1, (ajint)0);
	ret->positions = ajUintNewRes((ajint)len);
	ajUintPut(&ret->positions, len-1, (ajint)0);
	ret->ncon_score   = ajFloatNewRes((ajint)len);
	ajFloatPut(&ret->ncon_score , len-1, (float)0.0);
	ret->ccon_score   = ajFloatNewRes((ajint)len);
	ajFloatPut(&ret->ccon_score  , len-1, (float)0.0);
	ret->nccon_score = ajUintNewRes((ajint)len);
	ajUintPut(&ret->nccon_score, len-1, (ajint)0);
	ret->combi_score  = ajUintNewRes((ajint)len);
	ajUintPut(&ret->combi_score, len-1, (ajint)0);
    }

    
    ret->seqmat_do = ajFalse;
    ret->seqvar_do = ajFalse;


    ret->filtercon  = ajFalse;
    ret->filterpsim = ajFalse;
    ret->ncon_do    = ajFalse;
    ret->ccon_do    = ajFalse;
    ret->nccon_do   = ajFalse;
    ret->conthresh  = 0;
    ret->random     = ajFalse;
    ret->manual     = ajFalse;
    
    return ret;
}




/* @funcstatic siggen_ScorealgDel *********************************************
**
** Destructor for Scorealg object.
**
** @param [w] pthis [AjPScorealg*] Scorealg object pointer
**
** @return [void]
** @@
******************************************************************************/

static void siggen_ScorealgDel(AjPScorealg *pthis)
{
    ajFloatDel(&(*pthis)->seqmat_score);
    ajFloatDel(&(*pthis)->seqvar_score);

    ajUintDel(&(*pthis)->post_similar);
    ajUintDel(&(*pthis)->positions);
    ajFloatDel(&(*pthis)->ncon_score);
    ajFloatDel(&(*pthis)->ccon_score);
    ajUintDel(&(*pthis)->nccon_score);
    ajUintDel(&(*pthis)->combi_score);
    ajUintDel(&(*pthis)->ncon_thresh);

    AJFREE(*pthis);    
    *pthis = NULL;

    return;
}	
