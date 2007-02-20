/* @source ememe application
**
** EMBOSS wrapper to meme from Timothy Bailey's MEME package version 3.0.14 
** Reads sequences and generates motifs. 
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





/* @prog ememe ****************************************************************
**
** EMBOSS wrapper to meme from Timothy Bailey's MEME package version 3.0.14 
** Reads sequences and generates motifs. 
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPSeqset  dataset   = NULL;
    AjPFile    bfile     = NULL;
    AjPFile    plib      = NULL;
    AjPStr*    mod       = NULL;
    ajint      nmotifs   = 0;
    AjBool     text      = ajFalse;
    AjPStr*    prior     = NULL;
    float      evt       = 0.0;
    ajint      nsites    = 0;
    ajint      minsites  = 0;
    ajint      maxsites  = 0;
    float      wnsites   = 0.0;
    ajint      w         = 0;
    ajint      minw      = 0;
    ajint      maxw      = 0;
    AjBool     nomatrim  = ajFalse;
    ajint      wg        = 0;
    ajint      ws        = 0;
    AjBool     noendgaps = ajFalse;
    AjBool     revcomp   = ajFalse;
    AjBool     pal       = ajFalse;
    AjBool     nostatus  = ajFalse;
    ajint      maxiter   = 0;
    float      distance  = 0.0;
    float      b         = 0.0;
    float      spfuzz    = 0.0;
    AjPStr*    spmap     = NULL;
    AjPStr     cons      = NULL;
    ajint      maxsize   = 0;
    ajint      p         = 0;
    ajint      time      = 0;
    AjPStr     sf        = NULL;
    AjPFile    outfile   = NULL;

    /* Housekeeping variables */
    AjPStr     cmd       = NULL;
    AjPStr     rnd       = NULL;      
    AjPSeqout  rndo      = NULL;   
    AjPStr     tmp       = NULL;
    char       option;


    


    /* ACD file processing */
    embInitP("ememe",argc,argv,"MEMENEW");
    dataset   = ajAcdGetSeqset("dataset");
    bfile     = ajAcdGetInfile("bfile");
    plib      = ajAcdGetInfile("plib");
    mod       = ajAcdGetSelect("mod");
    nmotifs   = ajAcdGetInt("nmotifs");
    text      = ajAcdGetBool("text");
    prior     = ajAcdGetSelect("prior");
    evt       = ajAcdGetFloat("evt");
    nsites    = ajAcdGetInt("nsites");
    minsites  = ajAcdGetInt("minsites");
    maxsites  = ajAcdGetInt("maxsites");
    wnsites   = ajAcdGetFloat("wnsites");
    w         = ajAcdGetInt("w");
    minw      = ajAcdGetInt("minw");
    maxw      = ajAcdGetInt("maxw");
    nomatrim  = ajAcdGetBool("nomatrim");
    wg        = ajAcdGetInt("wg");
    ws        = ajAcdGetInt("ws");
    noendgaps = ajAcdGetBool("noendgaps");
    revcomp   = ajAcdGetBool("revcomp");
    pal       = ajAcdGetBool("pal");
    nostatus  = ajAcdGetBool("nostatus");
    maxiter   = ajAcdGetInt("maxiter");
    distance  = ajAcdGetFloat("distance");
    b         = ajAcdGetFloat("b");
    spfuzz    = ajAcdGetFloat("spfuzz");
    spmap     = ajAcdGetSelect("spmap");
    cons      = ajAcdGetString("cons");
    maxsize   = ajAcdGetInt("maxsize");
    p         = ajAcdGetInt("p");
    time      = ajAcdGetInt("time");
    sf        = ajAcdGetString("sf");
    outfile   = ajAcdGetOutfile("outfile");

    

    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd  = ajStrNew();
    rnd  = ajStrNew();
    tmp  = ajStrNew();

    
    /* 2. Re-write dataset to a temporary file in a format (fasta) MEME can understand.
       We cannot just pass the name of dataset to MEME as the name provided might be a 
       USA which MEME would not understand. */
    ajStrAssignC(&rnd, ajFileTempName(NULL));
    rndo = ajSeqoutNew();
    if(!ajSeqoutOpenFilename(rndo, rnd))
	ajFatal("Terminal ajSeqFileNewOut failure. Email EMBOSS helpdesk!\n");
    ajSeqOutSetFormatC(rndo, "fasta");
    ajSeqoutWriteSet(rndo, dataset);
    ajSeqoutClose(rndo);
    ajSeqoutDel(&rndo);


    /* 3. Build ememe command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. Original MEME options (in order they appear in ACD file)
       iii.Original MEME options (that don't appear in ACD file)
       iv. EMBASSY MEME new qualifiers and parameters.
       */
    if(!ajNamGetValueC("meme", &cmd))
	ajStrAssignC(&cmd, "meme");

    ajFmtPrintAppS(&cmd, " %S", rnd);
    if(bfile)
	ajFmtPrintAppS(&cmd, " -bfile %s ", ajFileName(bfile));
    if(plib)
	ajFmtPrintAppS(&cmd, " -plib %s ", ajFileName(plib));
    option = ajStrGetCharFirst(mod[0]);
    if(option == 'o')
	ajStrAppendC(&cmd, " -mod oops ");
    else if(option == 'z')
	ajStrAppendC(&cmd, " -mod zoops ");
    else if(option == 'a')
	ajStrAppendC(&cmd, " -mod anr ");
    ajFmtPrintAppS(&cmd, " -nmotifs %d ", nmotifs);
    if(text)
	ajFmtPrintAppS(&cmd, " -text ");
    ajFmtPrintAppS(&cmd,  " -prior %S ", prior[0]);
    if(evt != -1)
	ajFmtPrintAppS(&cmd, " -evt %f ", evt);
    if(nsites != -1)
	ajFmtPrintAppS(&cmd, " -nsites %d ", nsites);
    else
    {
	if(minsites != -1)
	    ajFmtPrintAppS(&cmd, " -minsites %d ", minsites);
	if(maxsites != -1)
	    ajFmtPrintAppS(&cmd, " -maxsites %d ", maxsites);
    }
    ajFmtPrintAppS(&cmd, " -wnsites %f ", wnsites);
    if(w != -1)
	ajFmtPrintAppS(&cmd, " -w %d ", w);
    ajFmtPrintAppS(&cmd, " -minw %d ", minw);
    ajFmtPrintAppS(&cmd, " -maxw %d ", maxw);
    if(nomatrim)
	ajFmtPrintAppS(&cmd, " -nomatrim ");
    ajFmtPrintAppS(&cmd, " -wg %d ", wg);
    ajFmtPrintAppS(&cmd, " -ws %d ", ws);
    if(noendgaps)
	ajFmtPrintAppS(&cmd, " -noendgaps ");
    if(revcomp)
	ajFmtPrintAppS(&cmd, " -revcomp ");

    if(pal && ajSeqsetIsNuc(dataset))
	ajFmtPrintAppS(&cmd, " -pal ");
    if(nostatus)
	ajFmtPrintAppS(&cmd, " -nostatus ");
    ajFmtPrintAppS(&cmd, " -maxiter %d ", maxiter);
    ajFmtPrintAppS(&cmd, " -distance %f ", distance);
    if(b != -1)
	ajFmtPrintAppS(&cmd, " -b %f ", b);
    if(spfuzz != -1)
	ajFmtPrintAppS(&cmd, " -spfuzz %f ", spfuzz);
    ajFmtPrintAppS(&cmd,  " -spmap %S ", spmap[0]);

    if(MAJSTRGETLEN(cons))
	ajFmtPrintAppS(&cmd, "-cons %S", cons);
    if(maxsize != -1)
	ajFmtPrintAppS(&cmd, " -maxsize %d ", maxsize);
    if(p > 0)
	ajFmtPrintAppS(&cmd, " -p %d ", p);
    if(time > 0)
	ajFmtPrintAppS(&cmd, " -time %d ", time);
    if(MAJSTRGETLEN(sf))
	ajFmtPrintAppS(&cmd, " -sf %S", sf);

    if(ajSeqsetIsProt(dataset))
	ajFmtPrintAppS(&cmd, "-protein ");
    else
	ajFmtPrintAppS(&cmd, "-dna ");
    ajFmtPrintAppS(&cmd, " > %S ", ajFileGetName(outfile));


    /* 4. Close files from ACD before calling meme */	
    ajFileClose(&bfile);	
    ajFileClose(&plib);
    ajFileClose(&outfile);


    /* 5. Call meme */
    /*    ajFmtPrint("\n%S\n", cmd);  */
    system(ajStrGetPtr(cmd));    

    /* 6. Exit cleanly */
    /* Delete temporary file */
    ajFmtPrintS(&tmp, "rm %S", rnd);
    system(ajStrGetPtr(tmp)); 

    ajSeqsetDel(&dataset);
    ajStrDel(&cons);
    ajStrDel(&sf);
    /* Should delete mod, prior and spmap here, but no convenient way 
       so leave it to the o.s. */
    
    ajStrDel(&cmd);
    ajStrDel(&rnd);
    ajStrDel(&tmp);

    embExit();

    return 0;
}
