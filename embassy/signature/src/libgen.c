/* @source libgen application
**
** Generates various types of discriminating elements for each alignment in a 
** directory.
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
*******************************************************************************
** 
**  LIBGEN Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  NOTES
**  
******************************************************************************/





#include "emboss.h"

#define LIBGEN_FREQUENCY 1
#define LIBGEN_GRIBSKOV  2
#define LIBGEN_HENIKOFF  3
#define LIBGEN_HMMER     4
#define LIBGEN_SAM       5
#define LIBGEN_PSSM      6
#define LIBGEN_AZ        27
#define GRIBSKOV_LENGTH  27
#define HENIKOFF_LENGTH  27





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static void libgen_simple_matrix(AjPSeqset seqset, 
				 AjPFile outf,
				 AjPStr name,
				 ajint threshold);

static void libgen_gribskov_profile(AjPSeqset seqset,
				    AjPFile outf, 
				    AjPStr name,
				    ajint threshold, 
				    float gapopen,
				    float gapextend, 
				    AjPStr *cons);

static void libgen_henikoff_profile(AjPSeqset seqset,
				    AjPMatrixf imtx, 
				    ajint threshold, 
				    AjPFile outf,
				    AjPStr name, 
				    float gapopen, 
				    float gapextend,
				    AjPStr *cons);





/* @prog libgen *************************************************************
**
** Generates various types of discriminating elements for each alignment in 
** a directory.
**
****************************************************************************/

int main(int argc, char **argv)
{
    AjPStr    *mode      = NULL;  /* Mode of operation 
				     1:Frequency, 2:Gribskov,3:Henikoff,
				     4:HMM (HMMER)                         */
    ajint      modei     = 0;     /* Mode as an integer.                   */
     
    AjPList    indir     = NULL;  /* Directory of input alignments.        */
    AjPStr     inname    = NULL;  /* Name of current input alignments.     */
    AjPFile    inf       = NULL;  /* File pointer for alignments (input).  */

    AjPDir     outdir    = NULL;  /* Directory of output discriminators.   */    
    AjPStr     outname   = NULL;  /* Name of output file.                  */
    AjPFile    outf      = NULL;  /* File for discriminator (output).      */

    AjPStr     seqsfname = NULL;  /* Random name of temp. files.           */
    AjPFile    seqsf     = NULL;  /* File for temp. file.                  */
    AjPStr     seqfname  = NULL;  /* Random name of temp. files.           */
    AjPFile    seqf      = NULL;  /* File for temp. file.                  */
    AjPStr     tmpfname  = NULL;  /* Random name of temp. files.           */

    AjPMatrixf  matrixf  = NULL;  /* Substitution matrix.                  */
    float      gapopen   = 0.0;   /* Gap open penalty.                     */
    float      gapextend = 0.0;   /* Gap extention  penalty.               */
    ajint      threshold = 0;     /* Threshold for frequency matrices.     */
    
    AjPScopalg scopalg   = NULL;  /* Scopalg object for input alignments.  */

    AjPSeqin   seqin     = NULL;  /* For defining seqset input file.       */
    AjPSeqset  seqset    = NULL;  /* Input for profile generating functions*/
    AjPSeq     seq       = NULL;  /* Sequence object.                      */
    AjPStr     cmd       = NULL;  /* Command line for external apps.       */
    ajint      i         = 0;     /* Loop counter.                         */

    AjPStr     cons      = NULL;  /* Housekeeping.                         */

    /* For PSSM generation via PSIBLAST: all have dummy values */
    ajint      niter     = 0;     /* Number of iterations                  */          
    float      evalue    = 0.0;   /* Threshold E-value                     */
    AjPStr     database  = NULL;  /* Blast-indexed database to search      */

    ajNamInit("emboss");
    ajAcdInitP("libgen",argc,argv,"SIGNATURE");
    
    
    /* ACD processing. */
    mode       = ajAcdGetList("mode");
    indir      = ajAcdGetDirlist("dafinpath");
    outdir     = ajAcdGetOutdir("outdir");
    threshold  = ajAcdGetInt("threshold");
    matrixf    = ajAcdGetMatrixf("matrixfile");
    gapopen    = ajAcdGetFloat("gapopen");
    gapextend  = ajAcdGetFloat("gapextend");



    /* Housekeeping. */
    modei      = (ajint) ajStrChar(*mode,0)-48;
    gapopen    = ajRoundF(gapopen,8);
    gapextend  = ajRoundF(gapextend,8);
    seqsfname  = ajStrNew();
    cmd        = ajStrNew();
    ajStrAssC(&database, "swissprot"); /* Dummy value. */



    /* Main application loop. */
    while(ajListPop(indir,(void **)&inname))
    {  
	/* Open domain alignment file. */
        if((inf = ajFileNewIn(inname)) == NULL)
        {
            ajWarn("Could not open file %S\n",inname);
	    ajStrDel(&inname);
            continue;
        }
	
	/* Read alignment into Scopalg object and create seqset from it. */
	seqset = ajSeqsetNew();
	if((!ajDmxScopalgRead(inf,&scopalg)))
	{
	    ajWarn("ajDmxScopalgRead call failed in libgen");
	
	    /* Read sequence set instead. */ 
	    seqin  = ajSeqinNew();
	    /* Set the filename via the USA. ajSeqsetRead interprets it to 
	       find the filename. */
	    ajSeqinUsa(&seqin, inname);
	
	    if(!(ajSeqsetRead(seqset, seqin)))
		ajFatal("SeqsetRead failed in seqsearch_psialigned");
	    ajSeqinDel(&seqin);
	}
	/* Otherwise read seqset directly. */
	else
	{
	    /* Create seqset with input sequences. */
	    for(i=0; i<scopalg->N; i++)
            {
		seq = ajSeqNewStr(scopalg->Seqs[i]);
		ajStrAssS(&seq->Acc,scopalg->Codes[i]);
		
                ajSeqsetApp(seqset,seq);
		ajSeqDel(&seq);

            }	
	}
	ajFileClose(&inf);

	/* Create name for output file. */
	ajStrAssS(&outname, inname);
	ajFileDirExtnTrim(&outname);	
	ajStrInsert(&outname, 0, ajDirName(outdir));
	ajStrAppC(&outname, ".");
	ajStrApp(&outname, ajDirExt(outdir));


	if((modei==LIBGEN_HMMER) || (modei==LIBGEN_PSSM) || (modei==LIBGEN_SAM))
	{
	    /* Write alignment in CLUSTAL format to temp. file. */
	    ajRandomSeed();
	    ajStrAssC(&seqsfname, ajFileTempName(NULL));
	    if(modei==LIBGEN_SAM)
		ajStrAppC(&seqsfname, ".a2m"); 
	    seqsf = ajFileNewOut(seqsfname);

	    if(scopalg)
		ajDmxScopalgWriteClustal2(scopalg,seqsf);
	    else
	    {
		ajFmtPrintF(seqsf, "\n"); 
		for(i=0;i<ajSeqsetSize(seqset);++i)
		    ajFmtPrintF(seqsf,"%S_%d   %s\n", 
				ajSeqsetName(seqset, i),	
				ajSeqsetSeq(seqset, i));
		ajFmtPrintF(seqsf,"\n");
	    }
	    ajFileClose(&seqsf);


	    /* Write single sequence to temp. file. */
	    ajStrAssC(&seqfname, ajFileTempName(NULL));
	    seqf = ajFileNewOut(seqfname);
	    
	    if(scopalg)
		ajFmtPrintF(seqf,">\n%S\n", scopalg->Seqs[0]);
	    else
		ajFmtPrintF(seqf,"\n%s\n",  ajSeqsetSeq(seqset, 0));
	    ajFileClose(&seqf);


	    if(modei==LIBGEN_HMMER)
		ajFmtPrintS(&cmd,"hmmbuild -g %S %S",outname,seqsfname);
	    else if(modei==LIBGEN_PSSM)
		/* niter, evalue and database arg's have dummy values. */
		ajFmtPrintS(&cmd,"blastpgp -i %S -B %S -j %d -e %f -d %S -C %S\n",
			    seqfname, seqsfname, niter,evalue, database, 
			    outname);
	    else if(modei==LIBGEN_SAM)
	    {
		ajStrAssC(&tmpfname, ajFileTempName(NULL));
		ajStrAppC(&tmpfname, ".mod");


		/* Run modelfromalign. */
		ajFmtPrintS(&cmd,"modelfromalign %S -alignfile %S",tmpfname,
			    seqsfname);
		ajFmtPrint("%S\n", cmd);
		system(ajStrStr(cmd));

		/* Could run buildmodel to refine the model 
		   ajFmtPrintS(&cmd,"buildmodel %S -train %S -alignfile %S",
		   outname,tmpfname,seqsfname);
		   ajFmtPrint("%S\n", cmd);
		   system(ajStrStr(cmd));
		   */

		ajFmtPrintS(&cmd,"rm %S",tmpfname);
		system(ajStrStr(cmd));
	    }
	    
	    
	    ajFmtPrint("%S\n", cmd);
	    ajSystem(cmd);
	    ajFmtPrintS(&cmd,"rm %S",seqsfname);
	    ajSystem(cmd);
	    ajFmtPrintS(&cmd,"rm %S",seqfname);
	    ajSystem(cmd);

	}
	else
	{
	    if((!(outf = ajFileNewOut(outname))))
		ajFatal("Could not create output file\n");
	    
	    if(modei==LIBGEN_FREQUENCY)
                libgen_simple_matrix(seqset,outf,outname,threshold);
	    else if(modei==LIBGEN_GRIBSKOV)
		libgen_gribskov_profile(seqset,outf,outname,
					 threshold,gapopen,gapextend,&cons);
	    else if(modei==LIBGEN_HENIKOFF)
		libgen_henikoff_profile(seqset,matrixf,threshold,outf,
					outname,gapopen,gapextend, &cons);
	    
	    ajFileClose(&outf);	    
	}

	/* Tidy up. */
	ajStrDel(&inname);
	if(scopalg)
	    ajDmxScopalgDel(&scopalg);
	ajSeqsetDel(&seqset);

    }
    

 
    /* Tidy up. */
    ajStrDel(&mode[0]);
    AJFREE(mode);
    ajListDel(&indir);
    ajDirDel(&outdir);
    ajStrDel(&seqsfname);
    ajStrDel(&cmd);
    
    ajExit();
    return 0;
}






/* @funcstatic  libgen_simple_matrix ****************************************
**
** Generate simple matrix.
**
** @param [r] seqset [AjPSeqset] Set of sequences. 
** @param [r] outf [AjPFile] Output file.
** @param [r] name [AjPStr] Name.
** @param [r] threshold [ajint] Threshold.
** 
** @return [void]
** @@
****************************************************************************/
static void libgen_simple_matrix(AjPSeqset seqset, 
				 AjPFile outf, 
				 AjPStr name,
				 ajint threshold)
{
    const char *p;
    ajint nseqs;
    ajint mlen;
    ajint len;
    ajint i;
    ajint j;
    ajint x;
    ajint px;
    
    ajint maxscore;
    ajint score;
    ajint *matrix[LIBGEN_AZ+2];
    AjPStr cons=NULL;
    
    nseqs = ajSeqsetSize(seqset);
    if(nseqs<2)
        ajFatal("Insufficient sequences (%d) to create a matrix",nseqs);

    mlen = ajSeqsetLen(seqset);
    
    /* Check sequences are the same length. Warn if not. */
    for(i=0;i<nseqs;++i)
    {
        p = ajSeqsetSeq(seqset,i);
        if(strlen(p)!=mlen)
            ajWarn("Sequence lengths are not equal!");
    }
    
    for(i=0;i<LIBGEN_AZ+2;++i)
        AJCNEW0(matrix[i], mlen);

    /* Load matrix. */
    for(i=0;i<nseqs;++i)
    {
        p = ajSeqsetSeq(seqset,i);      
        len = strlen(p);
        for(j=0;j<len;++j)
        {
            x = toupper((ajint)*p++);
            ++matrix[ajAZToInt(x)][j];
        }
    }

    /* Get consensus sequence. */
    cons = ajStrNew();
    for(i=0;i<mlen;++i)
    {
        px=x=-INT_MAX;
        for(j=0;j<LIBGEN_AZ-1;++j)
            if(matrix[j][i]>x)
            {
                x=matrix[j][i];
                px=j;
            }
        ajStrAppK(&cons,(char)(px+'A'));
    }
    
    /* Find maximum score for matrix. */
    maxscore=0;
    for(i=0;i<mlen;++i)
    {
        for(j=score=0;j<LIBGEN_AZ;++j)
            score = AJMAX(score,matrix[j][i]);
        maxscore += score;
    }

    ajFmtPrintF(outf,"# Pure Frequency Matrix\n");
    ajFmtPrintF(outf,"# Columns are amino acid counts A->Z\n");
    ajFmtPrintF(outf,"# Rows are alignment positions 1->n\n");
    ajFmtPrintF(outf,"Simple\n");
    ajFmtPrintF(outf,"Name\t\t%s\n",ajStrStr(name));
    ajFmtPrintF(outf,"Length\t\t%d\n",mlen);
    ajFmtPrintF(outf,"Maximum score\t%d\n",maxscore);
    ajFmtPrintF(outf,"Thresh\t\t%d\n",threshold);
    ajFmtPrintF(outf,"Consensus\t%s\n",ajStrStr(cons));


    for(i=0;i<mlen;++i)
    {
        for(j=0;j<LIBGEN_AZ;++j)
            ajFmtPrintF(outf,"%-2d ",matrix[j][i]);
        ajFmtPrintF(outf,"\n");
    }
    
    for(i=0;i<LIBGEN_AZ+2;++i)
        AJFREE (matrix[i]);

    ajStrDel(&cons);

    return;
}








/* @funcstatic  libgen_gribskov_profile ***************************************
**
** Generate Gribskov profile.
**
** @param [r] seqset [AjPSeqset] Set of sequences. 
** @param [r] outf [AjPFile] Output file.
** @param [r] name [AjPStr] Name.
** @param [r] threshold [ajint] Threshold.
** @param [r] gapopen [float] Gap open penalty. 
** @param [r] gapextend [float] Gap extend penalty.
** @param [w] cons [AjPStr*] Conservation.
** 
** @return [void]
** @@
******************************************************************************/
static void libgen_gribskov_profile(AjPSeqset seqset, 
				    AjPFile outf, 
				    AjPStr name, 
				    ajint threshold,
				    float gapopen, 
				    float gapextend, 
				    AjPStr *cons)
{
    AjPMatrixf matrix=0;
    AjPSeqCvt cvt=0;
    AjPStr mname=NULL;
    float **mat;
    ajint nseqs;
    ajint mlen;
    ajint i;
    ajint j;
    static char *valid="ACDEFGHIKLMNPQRSTVWY";
    const char *p;
    char *q;
    float score;
    float sum;
    ajint   gsum;
    float   mmax;
    float   pmax;
    float   psum;
    ajint  start;
    ajint  end;
    ajint  pos;
    float  x;
    ajint  px;
    
    float **weights;
    ajint *gaps;
    float **sub       = NULL;

    
    
    mname=ajStrNewC("Epprofile");
    ajMatrixfRead(&matrix,mname);
    ajStrDel(&mname);
                 
    nseqs = ajSeqsetSize(seqset);
    mlen  = ajSeqsetLen(seqset);

    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);



    /* Set gaps to be maximum length of gap that can occur including that 
       position. */
    AJCNEW(gaps, mlen);

    for(i=0;i<mlen;++i)
    {
        gsum=0;
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            if(ajAZToInt(p[i])!=27)  /* If not a gap. */
                continue;
            pos = i;
            while(pos>-1 && ajAZToInt(p[pos])==27)
                --pos;
            start = ++pos;
            pos=i;
            while(pos<mlen && ajAZToInt(p[pos])==27)
                ++pos;
            end = pos-1;
            gsum = AJMAX(gsum, (end-start)+1);
        }
        gaps[i]=gsum;
    }


    /* Get maximum score in scoring matrix. */
    mmax=0.0;
    p=valid;
    while(*p)
    {
        q=valid;
        while(*q)
        {
            mmax=(mmax>sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)]) ? mmax :
                sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)];
            ++q;
        }
        ++p;
    }


    /* Create the weight matrix and zero it. */
    AJCNEW(weights, mlen);
    for(i=0;i<mlen;++i)
      AJCNEW0(weights[i], GRIBSKOV_LENGTH+1);

    /*
     **  Count the number of times each residue occurs at each
     **  position in the alignment
     */
    for(i=0;i<mlen;++i)
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            weights[i][ajAZToInt(p[i])] += ajSeqsetWeight(seqset,j);
        }


    px = -INT_MAX;
    for(i=0;i<mlen;++i)
    {
        x = (float)-INT_MAX;
        for(j=0;j<LIBGEN_AZ-1;++j)
            if(weights[i][j]>x)
            {
                x=weights[i][j];
                px=j;
            }
        ajStrAppK(cons,(char)(px+'A'));
    }
    
    
    /* Normalise the weights. */
    for(i=0;i<mlen;++i)
        for(j=0;j<GRIBSKOV_LENGTH;++j)
            weights[i][j] /= (float)nseqs;


    /* Create the profile matrix n*GRIBSKOV_LENGTH and zero it. */
    AJCNEW(mat, mlen);
    for(i=0;i<mlen;++i)
        AJCNEW0(mat[i],GRIBSKOV_LENGTH);

    /* Fill the profile with aa scores. */
    for(i=0;i<mlen;++i)
        for(p=valid;*p;++p)
        {
            sum = 0.0;
            q = valid;
            while(*q)
            {
                score = weights[i][ajAZToInt(*q)];
                score *= (float)(sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)]);
                sum += score;
                ++q;
            }
            mat[i][ajAZToInt(*p)] = sum;
        }

    /* Calculate gap penalties. */
    for(i=0;i<mlen;++i)
        mat[i][GRIBSKOV_LENGTH-1]=
	    (mmax / (gapopen+gapextend+(float)gaps[i]));


    /* Get maximum matrix score. */
    psum=0.0;
    for(i=0;i<mlen;++i)
    {
        pmax= (float)-INT_MAX;
        for(j=0;j<LIBGEN_AZ;++j)
            pmax=(pmax>mat[i][j]) ? pmax : mat[i][j];
        psum+=pmax;
    }
    
    /* Print matrix. */

    ajFmtPrintF(outf,"# Gribskov Protein Profile\n");
    ajFmtPrintF(outf,"# Columns are amino acids A->Z\n");
    ajFmtPrintF(outf,"# Last column is indel penalty\n");
    ajFmtPrintF(outf,"# Rows are alignment positions 1->n\n");
    ajFmtPrintF(outf,"Gribskov\n");
    ajFmtPrintF(outf,"Name\t\t%s\n",ajStrStr(name));
    ajFmtPrintF(outf,"Matrix\t\tpprofile\n");
    ajFmtPrintF(outf,"Length\t\t%d\n",mlen);
    ajFmtPrintF(outf,"Max_score\t%.2f\n",psum);
    ajFmtPrintF(outf,"Threshold\t%d\n",threshold);
    ajFmtPrintF(outf,"Gap_open\t%.2f\n",gapopen);
    ajFmtPrintF(outf,"Gap_extend\t%.2f\n",gapextend);
    ajFmtPrintF(outf,"Consensus\t%s\n",ajStrStr(*cons));
    
    for(i=0;i<mlen;++i)
    {
        for(j=0;j<GRIBSKOV_LENGTH;++j)
            ajFmtPrintF(outf,"%.2f ",mat[i][j]);
        ajFmtPrintF(outf,"%.2f\n",mat[i][GRIBSKOV_LENGTH-1]);
    }


    for(i=0;i<mlen;++i)
    {
        AJFREE (mat[i]);
        AJFREE (weights[i]);
    }
    AJFREE (mat);
    AJFREE (weights);

    AJFREE (gaps);

    ajMatrixfDel(&matrix);
    

    return;
}

/* @funcstatic  libgen_henikoff_profile *************************************
**
** Generate Henikoff profile.
**
** @param [r] seqset [AjPSeqset] Set of sequences. 
** @param [r] matrixf [AjPMatrixf] Substitution matrix.
** @param [r] threshold [ajint] Threshold.
** @param [r] outf [AjPFile] Output file.
** @param [r] name [AjPStr] Name.
** @param [r] gapopen [float] Gap open penalty. 
** @param [r] gapextend [float] Gap extend penalty.
** @param [w] cons [AjPStr*] Conservation.
** 
** @return [void]
** @@
****************************************************************************/
static void libgen_henikoff_profile(AjPSeqset seqset, 
				    AjPMatrixf matrix, 
				    ajint threshold,  
				    AjPFile outf, 
				    AjPStr name, 
				    float gapopen, 
				    float gapextend, 
				    AjPStr *cons)
{
    float **mat;
    ajint nseqs;
    ajint mlen;
    ajint i;
    ajint j;
    static char *valid="ACDEFGHIKLMNPQRSTVWY";
    const char *p;
    char *q;
    float score;
    float sum;
    float psum;
    float pmax;
    ajint   gsum;
    ajint   mmax;
    ajint  start;
    ajint  end;
    ajint  pos;
    
    float **weights;
    ajint *gaps;
    ajint *pcnt;
    float **sub       = NULL;

    float x;
    ajint px;

    AjPSeqCvt cvt=NULL;
    

    nseqs = ajSeqsetSize(seqset);
    mlen  = ajSeqsetLen(seqset);

    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);


    /* Set gaps to be maximum length of gap that can occur
     * including that position
     */
    AJCNEW(gaps, mlen);
    for(i=0;i<mlen;++i)
    {
        gsum=0;
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            if(ajAZToInt(p[i])!=27)
                continue; /* If not a gap. */
            pos = i;
            while(pos>-1 && ajAZToInt(p[pos])==27)
                --pos;
            start = ++pos;
            pos=i;
            while(pos<mlen && ajAZToInt(p[pos])==27)
                ++pos;
            end = pos-1;
            gsum = AJMAX(gsum, (end-start)+1);
        }
        gaps[i]=gsum;
    }

    /* Get maximum score in scoring matrix. */
    mmax=0;
    p=valid;
    while(*p)
    {
        q=valid;
        while(*q)
        {
            mmax=(mmax>sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)]) ? mmax :
                sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)];
            ++q;
        }
        ++p;
    }


    /* Create the weight matrix and zero it. */
    AJCNEW(weights, mlen);
    for(i=0;i<mlen;++i)
        AJCNEW0(weights[i],HENIKOFF_LENGTH+1);

    /*
     **  Count the number of times each residue occurs at each
     **  position in the alignment
     */
    for(i=0;i<mlen;++i)
        for(j=0;j<nseqs;++j)
        {
            p=ajSeqsetSeq(seqset,j);
            if(i>=strlen(p))
                continue;
            weights[i][ajAZToInt(p[i])] += ajSeqsetWeight(seqset,j);
        }

    px = -INT_MAX;
    for(i=0;i<mlen;++i)
    {
        x=(float)-INT_MAX;
        for(j=0;j<LIBGEN_AZ-1;++j)
            if(weights[i][j]>x)
            {
                x = weights[i][j];
                px=j;
            }
        ajStrAppK(cons,(char)(px+'A'));
    }
    


    /* Count the number of different residues at each position. */

    AJCNEW0(pcnt, mlen);

    for(i=0;i<mlen;++i)
        for(j=0;j<HENIKOFF_LENGTH-1;++j)
            if(weights[i][j])
                ++pcnt[i];
    
    /* weights = 1/(num diff res * count of residues at that position. */
    for(i=0;i<mlen;++i)
        for(j=0;j<HENIKOFF_LENGTH-1;++j)
            if(weights[i][j])
                weights[i][j] = 1.0/(weights[i][j]*(float)pcnt[i]);


    /* Create the profile matrix n*HENIKOFF_LENGTH. */
    AJCNEW(mat, mlen);
    for(i=0;i<mlen;++i)
      AJCNEW0(mat[i],HENIKOFF_LENGTH);
    
    /* Fill the profile with aa scores. */
    for(i=0;i<mlen;++i)
        for(p=valid;*p;++p)
        {
            sum = 0.0;
            q = valid;
            while(*q)
            {
                score = weights[i][ajAZToInt(*q)];
                score *= sub[ajSeqCvtK(cvt,*p)][ajSeqCvtK(cvt,*q)];
                sum += score;
                ++q;
            }
            mat[i][ajAZToInt(*p)] = sum;
        }

    /* Calculate gap penalties. */
    for(i=0;i<mlen;++i)
        mat[i][HENIKOFF_LENGTH-1]=(mmax / (gapopen+gapextend+
                                          (float)gaps[i]));


    /* Get maximum matrix score. */
    psum=0.0;
    for(i=0;i<mlen;++i)
    {
        pmax= (float)-INT_MAX;
        for(j=0;j<HENIKOFF_LENGTH-1;++j)
            pmax=(pmax>mat[i][j]) ? pmax : mat[i][j];
        psum+=pmax;
    }

    /* Print matrix. */

    ajFmtPrintF(outf,"# Henikoff Protein Profile\n");
    ajFmtPrintF(outf,"# Columns are amino acids A->Z\n");
    ajFmtPrintF(outf,"# Last column is indel penalty\n");
    ajFmtPrintF(outf,"# Rows are alignment positions 1->n\n");
    ajFmtPrintF(outf,"Henikoff\n");
    ajFmtPrintF(outf,"Name\t\t%s\n",ajStrStr(name));
    ajFmtPrintF(outf,"Matrix\t\t%s\n",ajStrStr(ajMatrixfName(matrix)));
    ajFmtPrintF(outf,"Length\t\t%d\n",mlen);
    ajFmtPrintF(outf,"Max_score\t%.2f\n",psum);
    ajFmtPrintF(outf,"Threshold\t%d\n",threshold);
    ajFmtPrintF(outf,"Gap_open\t%.2f\n",gapopen);
    ajFmtPrintF(outf,"Gap_extend\t%.2f\n",gapextend);
    ajFmtPrintF(outf,"Consensus\t%s\n",ajStrStr(*cons));

    for(i=0;i<mlen;++i)
    {
        for(j=0;j<HENIKOFF_LENGTH;++j)
            ajFmtPrintF(outf,"%.2f ",mat[i][j]);
        ajFmtPrintF(outf,"%.2f\n",mat[i][j-1]);
    }
    

    for(i=0;i<mlen;++i)
    {
        AJFREE (mat[i]);
        AJFREE (weights[i]);
    }
    AJFREE (mat);
    AJFREE (weights);
    AJFREE (gaps);
    AJFREE (pcnt);
    return;
}

