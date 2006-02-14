/* @source ehmmalign application
**
** EMBOSS wrapper to hmmalign from Sean Eddy's HMMER package v.2.3.2
** Align sequences to an HMM profile.
** 
** @author Copyright (C) Jon Ison (jison@ebi.ac.uk)
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




/* @prog ehmmalign ***********************************************************
**
** EMBOSS wrapper to hmmalign from Sean Eddy's HMMER package v.2.3.2
** Align sequences to an HMM profile.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile    hmmfile = NULL;     
    AjPSeqset  seqfile = NULL;     
    AjPFile     mapali = NULL;     
    AjPFile    withali = NULL;     
    AjPAlign         o = NULL;     
    AjBool           m = ajFalse;
    AjBool           q = ajFalse;

    /* Housekeeping variables */
    AjPStr        cmd = NULL;
    AjPStr        tmp = NULL;
    AjPStr        fmt = NULL;
    AjBool      fmtok = ajFalse;





    /* ACD file processing */
    ajNamInit("emboss");
    ajAcdInitP("ehmmalign",argc,argv,"HMMER_NEW");

    hmmfile = ajAcdGetInfile("hmmfile");
    seqfile = ajAcdGetSeqset("seqfile");
    mapali  = ajAcdGetInfile("mapali");
    withali = ajAcdGetInfile("withali");
    o       = ajAcdGetAlign("o");
    m       = ajAcdGetBool("m");
    q       = ajAcdGetBool("q");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd = ajStrNew();
    tmp = ajStrNew();
    fmt = ajStrNew();


    /* 2. Ensure seqfile is in format HMMER can understand.  These include
       FASTA, GENBANK,EMBL, GCG, PIR, STOCKHOLM, SELEX, MSF,CLUSTAL and PHYLIP.
       EMBOSS name definitions are taken from seqInFormatDef in ajseqread.c and
       seqOutFormat in ajseqwrite.c */
    fmtok=ajFalse;
    ajStrAssignS(&fmt, ajSeqsetGetFormat(seqfile));
    if(ajStrMatchC(fmt, "fasta")    ||
       ajStrMatchC(fmt, "genbank")  ||
       ajStrMatchC(fmt, "embl")     ||
       ajStrMatchC(fmt, "gcg")      ||
       ajStrMatchC(fmt, "pir")      ||
       ajStrMatchC(fmt, "stockholm")||
       ajStrMatchC(fmt, "selex")    ||
       ajStrMatchC(fmt, "msf")      ||
       ajStrMatchC(fmt, "clustal")  ||
       ajStrMatchC(fmt, "phylip"))
	fmtok = ajTrue;
    /* This could be replaced with code to reformat the file. */
    if(!fmtok)
	ajFatal("Input sequence set ('seqfile' ACD option) is not in format "
		"HMMER understands. Please use a a file in FASTA, GENBANK, "
		"EMBL, GCG, PIR, STOCKHOLM, SELEX, MSF,CLUSTAL or PHYLIP format.");

    /* 3. Build hmmalign command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajFmtPrintS(&cmd, "hmmalign ");
    if(mapali)
	ajFmtPrintAppS(&cmd, " --mapali %s ", ajFileName(mapali));
    if(withali)
	ajFmtPrintAppS(&cmd, " --withali %s ", ajFileName(withali));
    if(m)
	ajStrAppendC(&cmd, " -m ");
    if(q)
	ajStrAppendC(&cmd, " -q ");


    /* Ensure output alignment is in user-specified format. */
    fmtok=ajTrue;
    ajStrAssignS(&fmt, ajAlignGetFormat(o));
    /* fasta and a2m are identical formats. */
    if(ajStrMatchC(fmt, "fasta"))
	ajStrAssignC(&fmt, "A2M");
    else if(ajStrMatchC(fmt, "a2m"))
	ajStrAssignC(&fmt, "A2M");
    else if(ajStrMatchC(fmt, "msf"))
	ajStrAssignC(&fmt, "MSF");
    else if(ajStrMatchC(fmt, "phylip"))
	ajStrAssignC(&fmt, "PHYLIP");
    /* hmmer also supports stockholm, SELEX & Clustal output, EMBOSS does not.
       EMBOSS supports unknown/multiple/simple and srs output, hmmer does not. */ 
    else
	fmtok = ajFalse;

    if(!fmtok)
    {
	/* This could be replaced with code to reformat the file. */
	ajWarn("Specified output alignment format ('o' ACD option) is "
	       "not understood by HMMER.  Using stockholm format instead.");
	ajStrAssignC(&fmt, "Stockholm");
    }
       
    ajFmtPrintAppS(&cmd, " --outformat %S  -o %s %s %S", 
		   fmt,
		   ajAlignGetFilename(o),
		   ajFileName(hmmfile),
		   ajSeqsetGetFilename(seqfile));
            
    /* 4. Close ACD files */
    ajFileClose(&hmmfile);    
    ajSeqsetDel(&seqfile);
    ajFileClose(&mapali);
    ajFileClose(&withali);
    ajAlignDel(&o);

    
    /* 5. Call hmmalign */
    ajFmtPrint("\n%S\n\n", cmd);
    system(ajStrGetPtr(cmd));
    




    /* Exit cleanly */
    ajStrDel(&cmd);
    ajStrDel(&tmp);
    ajStrDel(&fmt);

    ajExit();

    return 0;
}
