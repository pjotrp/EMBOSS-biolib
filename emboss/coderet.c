/* @source coderet application
**
** Retrieves CDS and mRNA sequences from feature tables
**
** @author: Copyright (C) Alan Bleasby (ableasby@hgmp.mrc.ac.uk)
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
******************************************************************************/

#include "emboss.h"

void put_seq(AjPSeq seq, AjPStr strseq, int n, char *name, int type);


int main(int argc, char **argv)
{
    AjPSeq seq=NULL;
    
    int ncds=0;
    int nmrna=0;
    int i=0;

    AjPStr cds=NULL;
    AjPStr mrna=NULL;
    
    AjBool ret=ajFalse;
    AjPStr *cdslines=NULL;
    AjPStr *mrnalines=NULL;
    AjBool docds=ajFalse;
    AjBool domrna=ajFalse;
    
    
    embInit("coderet",argc,argv);
    
    seq  = ajAcdGetSeq("sequence");
    
    domrna = ajAcdGetBool("mrna");
    docds  = ajAcdGetBool("cds");
    

    cds = ajStrNew();
    mrna = ajStrNew();
    

    if(docds)
    {
	ncds = ajFeatGetLocs(seq->TextPtr, &cdslines, "CDS");
    
	for(i=0;i<ncds;++i)
	{
	    ret = ajFeatLocToSeq(ajSeqStr(seq),cdslines[i],&cds);
	    if(!ret)
	    {
		ajWarn("Cannot extract");
		continue;
	    }
	    put_seq(seq,cds,i,"cds",0);
	    ajStrDel(&cdslines[i]);
	}
	if(ncds)
	    AJFREE(cdslines);
    }
    
    if(domrna)
    {
	nmrna = ajFeatGetLocs(seq->TextPtr, &mrnalines, "mRNA");
    
	for(i=0;i<nmrna;++i)
	{
	    ret = ajFeatLocToSeq(ajSeqStr(seq),mrnalines[i],&mrna);
	    if(!ret)
	    {
		ajWarn("Cannot extract");
		continue;
	    }
	    put_seq(seq,mrna,i,"mrna",0);
	    ajStrDel(&mrnalines[i]);
	}

	if(nmrna)
	    AJFREE(mrnalines);
    }
    


    ajExit();
    return 0;
}



void put_seq(AjPSeq seq, AjPStr strseq, int n, char *name, int type)
{
    AjPSeq nseq=NULL;
    AjPStr fn=NULL;
    AjPSeqout seqout=NULL;
    
    fn = ajStrNew();
    

    ajFmtPrintS(&fn,"%S_%s_%d",ajSeqGetAcc(seq),name,n+1);
    ajStrToLower(&fn);

    nseq = ajSeqNewL(ajStrLen(strseq));
    ajSeqAssName(nseq, fn);
    ajSeqAssEntry(nseq, fn);

    if(!type)
	ajSeqSetNuc(nseq);
    else
	ajSeqSetProt(nseq);
    
    ajSeqReplace(nseq,strseq);

    seqout = ajSeqoutNew();

    ajStrAppC(&fn,".seq");

    ajSeqOutSetFormatC(seqout,"fasta");
    ajSeqFileNewOut(seqout,fn);	
    ajSeqWrite (seqout,nseq);  
    ajSeqWriteClose(seqout);

    ajSeqoutDel(&seqout);
    ajSeqDel(&nseq);
    ajStrDel(&fn);
    
    return;
}
