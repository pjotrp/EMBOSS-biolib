/* @Source allversusall application
**
** Does an all-versus-all global alignment for each set of sequences in an 
** input directory and writes files of sequence similarity values. 
**
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
**  ALLVERSUSALL Documentation
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
****************************************************************************/





#include "emboss.h"





/* @prog allversusall *******************************************************
**
** Does an all-versus-all global alignment for each set of sequences in an 
** input directory and writes files of sequence similarity values. 
**
****************************************************************************/
int main(int argc, char **argv)
{
    /* Variable declarations */
    AjPList    in        = NULL;    /* Names of domain hits files (input).    */    
    AjPStr     inname    = NULL;    /* Full name of the current DHF file.     */
    AjPFile    inf       = NULL;    /* Current DHF file.                      */
    AjPMatrixf matrix    = NULL;    /* Substitution matrix.                   */
    float      gapopen   = 0.0;     /* Gap insertion penalty.                 */
    float      gapextend = 0.0;     /* Gap extension penalty.                 */
    AjPDir     out       = NULL;    /* Domain hits files (output).            */    
    AjPFile    outf      = NULL;    /* Current DHF file (output).             */
    AjPStr     outname   = NULL;    /* Name of output file (re-used).         */    
    AjPFile    logf      = NULL;    /* log file pointer.                      */
 
    AjBool     ok        = ajFalse; /* Housekeeping                           */
    AjPSeqset  seqset    = NULL;    /* Seqset (re-used)                       */
    AjPSeqin   seqin     = NULL;    /* Seqin (re-used)                        */    
    AjPList    seq_list  = NULL;    /* Main list for redundancy removal.      */
    ajint      seq_list_siz = 0;    /* Size of seq_list                       */
    AjPSeq     seq_tmp   = NULL;    /* Temp. pointer for making seq_list.     */
    AjPFloat2d scores      = NULL;  /* 1: Sequence in seq_list was classed as
				       non-redundant, 0: redundant.           */
    ajint      x         = 0;       /* Housekeeping.                          */
    ajint      y         = 0;       /* Housekeeping.                          */

    


    /* Read data from acd */
    ajNamInit("emboss");
    ajAcdInitP("allversusall",argc,argv,"DOMAINATRIX");
    in        = ajAcdGetDirlist("seqinpath");
    matrix    = ajAcdGetMatrixf("matrix");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    out       = ajAcdGetOutdir("datoutdir");
    logf      = ajAcdGetOutfile("logfile");



    /* Housekeeping */
    outname     = ajStrNew();


       
    /* Process each DHF (input) in turn */
    while(ajListPop(in,(void **)&inname))
    {
	ajFmtPrint("Processing %S\n", inname);
	ajFmtPrintF(logf, "//\n%S\n", inname);


	seq_list    = ajListNew();
	
	/* Open & read sequence file. */
	if((inf = ajFileNewIn(inname)) == NULL)
	    ajFatal("Could not open DHF file %S", inname);

	seqset = ajSeqsetNew();
	seqin  = ajSeqinNew();
	ajSeqinUsa(&seqin, inname);
	if(!(ajSeqsetRead(seqset, seqin)))
	    ajFatal("SeqsetRead failed in seqsearch_psialigned");
	
	ok = ajFalse;
	if(ajSeqsetSize(seqset))
	    ok = ajTrue;
	
	ajFileClose(&inf);
	
	
	/* Process empty sequence files (should never occur). */
	if(!ok)
	{		
	    ajWarn("Empty input file %S\n", inname);
	    ajFmtPrintF(logf, "Empty input file %S\n", inname);
	    if(seqset)
		ajSeqsetDel(&seqset);
	    if(seqin)
		ajSeqinDel(&seqin);
	    continue;
	}	

	
	/* 1.  Create list of sequences from the main input directory. */
	for(x=0;x<ajSeqsetSize(seqset);x++)
	{
	    seq_tmp = ajSeqNew();
	    ajStrAssS(&seq_tmp->Acc, ajSeqsetAcc(seqset, x));
	    ajStrAssC(&seq_tmp->Seq, ajSeqsetSeq(seqset, x));
	    ajListPushApp(seq_list,seq_tmp);		
	}
    

	/* 4. Identify redundant domains. */
	if((!embDmxSeqCompall(seq_list, &scores, matrix, gapopen, gapextend)))
	    ajFatal("embDmxSeqCompall failure in allversusall");



	/* Create output files. */
	ajStrAssS(&outname, inname);
	ajFileDirExtnTrim(&outname);
	outf = ajFileNewOutDir(out, outname);
	

	/* 5. Write sequence similarity values to output directory.   */
	seq_list_siz = ajListLength(seq_list);
	for(x=0; x<seq_list_siz; x++)
	    for(y=x+1; y<seq_list_siz; y++)
		ajFmtPrintF(outf, "%d %S : %d %S : %.2f\n", 
			    x+1, ajSeqsetName(seqset, x), 
			    y+1, ajSeqsetName(seqset, y), 
			    ajFloat2dGet(scores, x, y));
	
	ajSeqsetDel(&seqset);
	ajSeqinDel(&seqin);
	ajFileClose(&outf);
	ajStrDel(&inname);

	while(ajListPop(seq_list, (void **) &seq_tmp))
	    ajSeqDel(&seq_tmp);
	ajListDel(&seq_list);
	ajFloat2dDel(&scores);	
    }	    


    /* Memory management. */
    ajListDel(&in);
    ajDirDel(&out);
    ajFileClose(&logf);
    ajStrDel(&outname);

    ajExit();
    return 0;
}



