/* @source sigscan application
**
** Generates a DHF (domain hits file) of hits (sequences) from scanning a 
** signature against a sequence database.
**
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
**  SIGSCAN documentation
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
**  NOTES
** 
******************************************************************************/





#include "emboss.h"
#include <math.h>




/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjBool sigscan_SignatureAlignWrite(AjPFile outf, 
					  EmbPSignature sig, 
					  EmbPHitlist hits);

static AjBool sigscan_SignatureAlignWriteBlock(AjPFile outf,
					       EmbPSignature sig,
					       EmbPHitlist hits);

void sigscan_dummy_function(void);




/* @prog sigscan **************************************************************
**
**  Generates a DHF (domain hits file) of hits (sequences) from scanning a 
**  signature against a sequence database.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPFile      sigin =NULL;     /* Signature input file.                   */
    AjPFile      hitsf =NULL;     /* Hits output file.                       */
    AjPFile      alignf =NULL;    /* Alignment output file.                  */

    AjPSeqall    database=NULL;   /* Protein sequences to match signature 
				     against.                                */
    AjPMatrixf   sub  =NULL;      /* Residue substitution matrix.            */
    float        gapo =0.0;       /* Gap insertion penalty.                  */
    float        gape =0.0;       /* Gap extension penalty.                  */

    ajint        nhits=0;         /* Max. no. of hits to write to
				     output files*/
    EmbPHitlist   hits=NULL;      /* Hitlist to stores hits from signature-
				     sequence matches.                       */

    EmbPSignature sig=NULL;       /* Signature data structure.               */
    AjPStr        nterm=NULL;     /* Holds N-terminal matching options from 
				     acd.                                    */
    ajint         ntopt=0;        /* N-terminal option as int. */



    embInitPV("sigscan", argc, argv, "SIGNATURE",VERSION);
    

    /* GET VALUES FROM ACD */
    sigin      = ajAcdGetInfile("siginfile");
    database   = ajAcdGetSeqall("dbsequence");
    sub        = ajAcdGetMatrixf("sub");
    gapo       = ajAcdGetFloat("gapo");
    gape       = ajAcdGetFloat("gape");
    nhits      = ajAcdGetInt("nhits");
    hitsf      = ajAcdGetOutfile("hitsfile");
    alignf     = ajAcdGetOutfile("alignfile"); 
    nterm      = ajAcdGetListSingle("nterm");


    /*Assign N-terminal matching option */
    ajFmtScanS(nterm, "%d", &ntopt);
    

    /* READ SIGNATURE FILE */
    if(!(sig = embSignatureReadNew(sigin)))
    {	
	ajMatrixfDel(&sub);
	ajFileClose(&sigin);
	ajFileClose(&hitsf);
	ajFileClose(&alignf);
	ajFatal("Error reading signature file");
    }   
    else
	ajFmtPrint("Signature file read ok\n");

    
    
    /* COMPILE SIGNATURE */
    if(!embSignatureCompile(&sig, gapo, gape, sub))
    {	
	embSignatureDel(&sig);
	ajMatrixfDel(&sub);
	ajFileClose(&sigin);
	ajFileClose(&hitsf);
	ajFileClose(&alignf);
	ajFatal("Error compiling signature");
    }  
    else
	ajFmtPrint("Signature compiled ok\n");



    /* THIS CODE BLOCK IS FOR DEBUG ONLY */
    /*    printf("no. pos.: %d\n", sig->npos);
	  for(y=0;y<sig->npos;y++)
	  {
	  printf("POSITION %d   no. gaps: %d\n",y, sig->pos[y]->ngaps);
	  for(x=0;x<sig->pos[y]->ngaps; x++)
	  printf("GAP %d  SIZ %d  PEN %f\n", x, sig->pos[y]->gsiz[x], 
	  sig->pos[y]->gpen[x]);
	  printf("\n\nRES.SUB.VALS\n");
	  for(x=0; x<26;x++)
	  printf("%c %f\n", (char)('A'+x), sig->pos[y]->subs[x]);
	  }
	  */
	
    

    /* ALIGN THE SIGNATURE TO THE SEQUENCES & FILL Hitlist object */
    hits = embHitlistNew(0);
    embSignatureAlignSeqall(sig, database, nhits, &hits, ntopt);
    ajFmtPrint("Signature aligned to db ok\n");


    /*WRITE HITS OUTPUT FILE */
    /* embSignatureHitsWrite(hitsf, sig, hits, nfalse); */
    embHitlistWriteFasta(hitsf, hits);
    ajFmtPrint("Hits file written ok\n");


    /*WRITE SIGNATURE ALIGNMENT OUTPUT FILE */
    sigscan_SignatureAlignWriteBlock(alignf, sig, hits);
    ajFmtPrint("Alignments file written ok\n");


    /* TIDY UP AND EXIT */
    embHitlistDel(&hits);
    embSignatureDel(&sig);
    ajSeqallDel(&database);
    ajMatrixfDel(&sub);
    ajFileClose(&sigin);
    ajFileClose(&hitsf);
    ajFileClose(&alignf);
    ajStrDel(&nterm);

    embExit();
    return 0;    
}





/* @funcstatic sigscan_SignatureAlignWrite ************************************
**
** Writes the alignments of a Signature to a list of AjOHit objects to an
** output file.
** This is intended for displaying the results from scans of a signature 
** against a protein sequence database.  Segments of sequences / alignment
** are given for all sequences in the same block.
**
** @param [w] outf [AjPFile]      Output file stream
** @param [r] sig  [EmbPSignature] Signature object pointer
** @param [r] hits [EmbPHitlist]   Hitlist objects with hits from scan
**
** @return [AjBool] True if file was written
** @@
******************************************************************************/

static AjBool sigscan_SignatureAlignWrite(AjPFile outf, 
					  EmbPSignature sig, 
					  EmbPHitlist hits)
{
    /*
    ** A line of the alignment (including accession number, a space and the 
    ** sequence) in the output file is 70 characters long. An index number is 
    ** also printed after this 70 character field.
    */
    ajint  wid1  = 0;    /* Temp. width of Accession Number.                */
    ajint  mwid1 = 0;    /* Max. width of Accession Number or the string 
			    "Number". This is the field width the accession 
			    numbers will be printed into.                   */

    ajint  mwid2 = 0;    /* Width of region to print sequence into.         */
    ajint  len   = 0;    /* Temp. length of sequence.                       */
    ajint  mlen  = 0;    /* Max. length of sequence.                        */
    const char   *ptrp = NULL;  /* Pointer to sequence string.              */ 
    const char   *ptrs = NULL;  /* Pointer to alignment string.             */ 
    ajint  idx   = 0;    /* Start position for printing.                    */
    ajint  niter = 0;    /* No. iterations of loop for printing out
			    sequence blocks.                                */
    ajint  fwid1 = 70;   /* Including accession number, a space, 7 characters 
			    for the first index number, and the sequence.   */
    ajint  fwid2 = 7;    /* Field width for the first index number */
    ajint  num   = 0;    /* Index number for alignment. */
    ajint  y     = 0;    /* Loop counter. */
    ajint  x     = 0;    /* Loop counter. */
    

    /* Check args */
    if(!outf || !hits || !sig)
	return ajFalse;

    /* Cycle through hits to find longest width of accession number. */
    for(len=0, mlen=0, wid1=0, mwid1=0, x=0;
	x<hits->N; 
	x++)
    {
	if((wid1=MAJSTRGETLEN(hits->hits[x]->Acc))>mwid1)
	    mwid1 = wid1; 
	if((len=MAJSTRGETLEN(hits->hits[x]->Seq))>mlen)
	    mlen = len;
    }

    /*Assign field widths and number of iterations for printing. */
    if((wid1 = strlen("SIGNATURE"))>mwid1)
	mwid1 = wid1;
    mwid1++;   /* A space */
    mwid2 = fwid1-fwid2-mwid1;
    niter = (ajint)ceil( ((double)mlen/(double)mwid2));
    

    /* Print header info and SCOP classification records of signature. */
    ajFmtPrintF(outf, "# DE   Results of signature search\n# XX\n");
    if((sig->Type == ajSCOP))  
	ajFmtPrintF(outf, "# TY   SCOP\n# XX\n");
    else if ((sig->Type == ajCATH))
	ajFmtPrintF(outf, "# TY   CATH\n# XX\n");
    ajFmtPrintF(outf,"# CL   %S",sig->Class);
    ajFmtPrintSplit(outf,sig->Fold,"\n# XX\n# FO   ",75," \t\n\r");
    ajFmtPrintSplit(outf,sig->Superfamily,"# XX\n# SF   ",75," \t\n\r");
    ajFmtPrintSplit(outf,sig->Family,"# XX\n# FA   ",75," \t\n\r");
    ajFmtPrintF(outf,"# XX\n# SI   %d\n", sig->Sunid_Family);
    


    /* Main loop for printing alignment. */
    for(num=0, idx=0, y=0;y<niter;y++)
    {
	num += mwid2;
		
	/* Print spacer */
	ajFmtPrintF(outf, "# XX\n");

	
	/* Loop for each protein in Hitlist. */
	for(x=0;x<hits->N; x++)
	{
	    /* Get pointer to sequence & alignment string. */
	    ptrp = ajStrGetPtr(hits->hits[x]->Seq);
	    ptrs = ajStrGetPtr(hits->hits[x]->Alg);

	    /* There is some of the sequence left to print. */
	    if(idx<MAJSTRGETLEN(hits->hits[x]->Seq))
	    {
		ajFmtPrintF(outf,"%-*S%-*d%-*.*s %d\n", 
			    mwid1, hits->hits[x]->Acc, fwid2, 
			    (num-mwid2+1), mwid2, mwid2, ptrp+idx, num);
		ajFmtPrintF(outf,"%-*s%-*c%-*.*s\n", 
			    mwid1, "SIGNATURE", fwid2, '-', mwid2, 
			    mwid2, ptrs+idx);
	    }	
      	    /* We have printed all the sequence already. */
	    else
	    {
		ajFmtPrintF(outf,"%-*S%-*d%-*.*s %d\n", 
			    mwid1, hits->hits[x]->Acc, fwid2,  
			    (num-mwid2+1), mwid2, mwid2, ".", num);
		ajFmtPrintF(outf,"%-*s%-*c%-*.*s\n", 
			    mwid1, "SIGNATURE", fwid2, '-', mwid2, 
			    mwid2, "." );
	    }
	}
	idx += mwid2;
    }	 

    

    return ajTrue;
}




/* @funcstatic sigscan_SignatureAlignWriteBlock *******************************
**
** Writes the alignments of a Signature to a list of AjOHit objects to an
** output file.
** This is intended for displaying the results from scans of a signature 
** against a protein sequence database. The full sequence / alignment is 
** printed out for each sequence in its own block.
**
** @param [w] outf [AjPFile]      Output file stream
** @param [r] sig  [EmbPSignature] Signature object pointer
** @param [r] hits [EmbPHitlist]   Hitlist objects with hits from scan
**
** @return [AjBool] True if file was written
** @@
******************************************************************************/

static AjBool sigscan_SignatureAlignWriteBlock(AjPFile outf,
					       EmbPSignature sig, 
					       EmbPHitlist hits)
{
    /*
    ** A line of the alignment (including accession number, a space and the 
    ** sequence) in the output file is 70 characters long. An index number is 
    ** also printed after this 70 character field.
    */
    ajint  wid1  = 0;    /*Temp. width of Accession Number */
    ajint  mwid1 = 0;    /*
			 ** Max. width of Accession Number or the string
			 ** "Number". 
			 ** This is the field width the accession numbers
			 ** will be  printed into
			 */
    ajint  mwid2 = 0;    /* Width of region to print sequence into */
    ajint  len   = 0;    /* Temp. length of sequence */
    ajint  mlen  = 0;    /* Max. length of sequence */
    const char   *ptrp = NULL; /* Pointer to sequence string */ 
    const char   *ptrs = NULL; /* Pointer to alignment string */ 
    ajint  idx   = 0;    /* Start position for printing */
    ajint  niter = 0;    /*
			 ** No. iterations of loop for printing out
			 ** sequence blocks
			 */
    ajint  fwid1 = 70;   /*
			 ** Including accession number, a space, 7 characters 
			 ** for the first index number, and the sequence
			 */
    ajint  fwid2 = 7;    /* Field width for the first index number */
    ajint  num   = 0;    /* Index number for alignment */
    ajint  y     = 0;    /* Loop counter */
    ajint  x     = 0;    /* Loop counter */
    

    /* Check args */
    if(!outf || !hits || !sig)
	return ajFalse;

    /* Cycle through hits to find longest width of accession number. */
    for(len=0, mlen=0, wid1=0, mwid1=0, x=0;
	x<hits->N; 
	x++)
    {
	if((wid1=MAJSTRGETLEN(hits->hits[x]->Acc))>mwid1)
	    mwid1 = wid1; 
	if((len=MAJSTRGETLEN(hits->hits[x]->Seq))>mlen)
	    mlen = len;
    }

    /* Assign field widths and number of iterations for printing. */
    if((wid1 = strlen("SIGNATURE"))>mwid1)
	mwid1 = wid1;
    mwid1++;   /* A space */
    mwid2 = fwid1-fwid2-mwid1;
    niter = (ajint)ceil( ((double)mlen/(double)mwid2));
    

    /* Print header info and SCOP classification records of signature */
    ajFmtPrintF(outf, "# DE   Results of signature search\n# XX\n");
    if((sig->Type == ajSCOP))  
	ajFmtPrintF(outf, "# TY   SCOP\n# XX\n");
    else if ((sig->Type == ajCATH))
	ajFmtPrintF(outf, "# TY   CATH\n# XX\n");
    ajFmtPrintF(outf,"# CL   %S",sig->Class);
    ajFmtPrintSplit(outf,sig->Fold,"\n# XX\n# FO   ",75," \t\n\r");
    ajFmtPrintSplit(outf,sig->Superfamily,"# XX\n# SF   ",75," \t\n\r");
    ajFmtPrintSplit(outf,sig->Family,"# XX\n# FA   ",75," \t\n\r");
    ajFmtPrintF(outf,"# XX\n# SI   %d\n", sig->Sunid_Family);


    
    /* Main loop for printing alignment. */
    for(x=0;x<hits->N; x++)
    {
	/* Get pointer to sequence & alignment string. */
	ptrp = ajStrGetPtr(hits->hits[x]->Seq);
	ptrs = ajStrGetPtr(hits->hits[x]->Alg);

	/* Print spacer */
	ajFmtPrintF(outf, "# XX\n");

	/* Loop for each protein in Hitlist. */
	for(num=0, idx=0, y=0;y<niter;y++)
	{
	    num+=mwid2;

	    /* There is some of the sequence left to print. */
	    if(idx<MAJSTRGETLEN(hits->hits[x]->Seq))
	    {
		ajFmtPrintF(outf,"%-*S%-*d%-*.*s %d\n", 
			    mwid1, hits->hits[x]->Acc, fwid2, 
			    (num-mwid2+1), mwid2, mwid2, ptrp+idx, num);
		ajFmtPrintF(outf,"%-*s%-*c%-*.*s\n", 
			    mwid1, "SIGNATURE", fwid2, '-', mwid2, 
			    mwid2, ptrs+idx);
	    }	
	    /* Printed all the sequence already. */
	    else
	    {
		ajFmtPrintF(outf,"%-*S%-*d%-*.*s %d\n", 
			    mwid1, hits->hits[x]->Acc, fwid2,  
			    (num-mwid2+1), mwid2, mwid2, ".", num);
		ajFmtPrintF(outf,"%-*s%-*c%-*.*s\n", 
			    mwid1, "SIGNATURE", fwid2, '-', mwid2, 
			    mwid2, "." );
	    }
	    idx += mwid2;
	}
    }	 

    return ajTrue;
}


/* To prevent compiler grumbling */
void sigscan_dummy_function(void)
{
    sigscan_SignatureAlignWrite(NULL, NULL, NULL);
}
