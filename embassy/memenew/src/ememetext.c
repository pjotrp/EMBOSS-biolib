/* @source ememe application
**
** EMBOSS wrapper to meme from Timothy Bailey's MEME package version 3.0.14 
** Reads sequences and generates motifs. 
** 
** @author Copyright (C) Jon Ison (jison@ebi.ac.uk)
** @modified Alan Bleasby
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
    AjPStr     mod       = NULL;
    ajint      nmotifs   = 0;
    AjBool     text      = ajFalse;
    AjPStr     prior     = NULL;
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
    AjPStr     spmap     = NULL;
    AjPStr     cons      = NULL;
    ajint      maxsize   = 0;
    ajint      p         = 0;
    ajint      time      = 0;
    AjPStr     sf        = NULL;
    ajint      heapsize  = 64;
    AjBool     xbranch   = ajFalse;
    AjBool     wbranch   = ajFalse;
    ajint      bfactor   = 0;
    AjPFile    outtext   = NULL;
    
    /* Housekeeping variables */
    AjPStr     cmd       = NULL;
    AjPStr     ssname    = NULL;      
    AjPSeqout  outseq    = NULL;   
    AjPStr     tmp       = NULL;
    char       option;



    
    /* ACD file processing */
    embInitPV("ememetext",argc,argv,"MEMENEW",VERSION);
    dataset   = ajAcdGetSeqset("dataset");
    bfile     = ajAcdGetInfile("bfile");
    plib      = ajAcdGetInfile("plibfile");
    mod       = ajAcdGetSelectSingle("mod");
    nmotifs   = ajAcdGetInt("nmotifs");
    text      = ajAcdGetBoolean("text");
    prior     = ajAcdGetSelectSingle("prior");
    evt       = ajAcdGetFloat("evt");
    nsites    = ajAcdGetInt("nsites");
    minsites  = ajAcdGetInt("minsites");
    maxsites  = ajAcdGetInt("maxsites");
    wnsites   = ajAcdGetFloat("wnsites");
    w         = ajAcdGetInt("w");
    minw      = ajAcdGetInt("minw");
    maxw      = ajAcdGetInt("maxw");
    nomatrim  = ajAcdGetBoolean("nomatrim");
    wg        = ajAcdGetInt("wg");
    ws        = ajAcdGetInt("ws");
    noendgaps = ajAcdGetBoolean("noendgaps");
    revcomp   = ajAcdGetBoolean("revcomp");
    pal       = ajAcdGetBoolean("pal");
    nostatus  = ajAcdGetBoolean("nostatus");
    maxiter   = ajAcdGetInt("maxiter");
    distance  = ajAcdGetFloat("distance");
    b         = ajAcdGetFloat("b");
    spfuzz    = ajAcdGetFloat("spfuzz");
    spmap     = ajAcdGetSelectSingle("spmap");
    cons      = ajAcdGetString("cons");
    maxsize   = ajAcdGetInt("maxsize");
    p         = ajAcdGetInt("p");
    time      = ajAcdGetInt("time");
    sf        = ajAcdGetString("sf");
    heapsize  = ajAcdGetInt("heapsize");
    xbranch   = ajAcdGetBoolean("xbranch");
    wbranch   = ajAcdGetBoolean("wbranch");
    bfactor   = ajAcdGetInt("bfactor");    

    outtext   = ajAcdGetOutfile("outtext");
    outseq    = ajAcdGetSeqoutset("outseq");
    
    

    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd      = ajStrNew();
    tmp      = ajStrNew();
    
    /* 2. Re-write dataset to a temporary file in a format (fasta) MEME
    ** can understand.
    ** Can't just pass the name of dataset to MEME as the name provided
    ** might be a USA which MEME would not understand.
    */

    ssname = ajStrNewS(ajFileGetNameS(outseq->File));
    
    ajSeqoutSetFormatC(outseq, "fasta");
    ajSeqoutWriteSet(outseq, dataset);
    ajSeqoutClose(outseq);
    ajSeqoutDel(&outseq);


    /* 3. Build ememe command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. Original MEME options (in order they appear in ACD file)
       iii.Original MEME options (that don't appear in ACD file)
       iv. EMBASSY MEME new qualifiers and parameters.
       */
    ajStrAssignS(&cmd, ajAcdGetpathC("meme"));
    ajFmtPrintAppS(&cmd, " %S", ssname);

    if(bfile)
	ajFmtPrintAppS(&cmd, " -bfile %s ", ajFileGetNameC(bfile));

    if(plib)
	ajFmtPrintAppS(&cmd, " -plib %s ", ajFileGetNameC(plib));

    option = ajStrGetCharFirst(mod);
    if(option == 'o')
	ajStrAppendC(&cmd, " -mod oops ");
    else if(option == 'z')
	ajStrAppendC(&cmd, " -mod zoops ");
    else if(option == 'a')
	ajStrAppendC(&cmd, " -mod anr ");

    if(nmotifs != 1)
        ajFmtPrintAppS(&cmd, " -nmotifs %d ", nmotifs);

    if(text)
	ajFmtPrintAppS(&cmd, " -text ");

    ajFmtPrintAppS(&cmd,  " -prior %S ", prior);

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

    if(wnsites < 0.7999 || wnsites > .8001)
        ajFmtPrintAppS(&cmd, " -wnsites %f ", wnsites);

    if(w != -1)
	ajFmtPrintAppS(&cmd, " -w %d ", w);

    if(minw != 8)
        ajFmtPrintAppS(&cmd, " -minw %d ", minw);

    if(maxw != 50)
        ajFmtPrintAppS(&cmd, " -maxw %d ", maxw);

    if(nomatrim)
	ajFmtPrintAppS(&cmd, " -nomatrim ");


    if(wg != 11)
        ajFmtPrintAppS(&cmd, " -wg %d ", wg);

    if(ws != 1)
        ajFmtPrintAppS(&cmd, " -ws %d ", ws);

    if(noendgaps)
	ajFmtPrintAppS(&cmd, " -noendgaps ");

    if(revcomp)
	ajFmtPrintAppS(&cmd, " -revcomp ");

    if(pal && ajSeqsetIsNuc(dataset))
	ajFmtPrintAppS(&cmd, " -pal ");

    if(nostatus)
	ajFmtPrintAppS(&cmd, " -nostatus ");

    if(maxiter != 50)
        ajFmtPrintAppS(&cmd, " -maxiter %d ", maxiter);

    if(distance < 0.00099 || distance > 0.00101)
        ajFmtPrintAppS(&cmd, " -distance %f ", distance);

    if(b != -1)
	ajFmtPrintAppS(&cmd, " -b %f ", b);

    if(spfuzz != -1)
	ajFmtPrintAppS(&cmd, " -spfuzz %f ", spfuzz);

    
    if(!ajStrMatchC(spmap,"default"))
        ajFmtPrintAppS(&cmd,  " -spmap %S ", spmap);

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

    if(heapsize != 64)
 	ajFmtPrintAppS(&cmd, " -heapsize %d ", heapsize);

    if(xbranch)
        ajFmtPrintAppS(&cmd, " -x_branch");

    if(wbranch)
        ajFmtPrintAppS(&cmd, " -w_branch");
    
    if(bfactor != 3)
 	ajFmtPrintAppS(&cmd, " -bfactor %d ", bfactor);

    if(ajSeqsetIsProt(dataset))
	ajFmtPrintAppS(&cmd, "-protein ");
    else
	ajFmtPrintAppS(&cmd, "-dna ");

    ajFmtPrintAppS(&cmd, " -text");

    ajFmtPrintAppS(&cmd, " > %S ", ajFileGetNameS(outtext));


    /* 4. Close files from ACD before calling meme */	
    ajFileClose(&bfile);	
    ajFileClose(&plib);


    /* 5. Call meme */
    /* ajFmtPrint("\n%S\n", cmd); */
    system(ajStrGetPtr(cmd));    


    /* 6. Exit cleanly */

    ajSeqsetDel(&dataset);
    ajStrDel(&cons);
    ajStrDel(&sf);
    ajStrDel(&mod);
    ajStrDel(&prior);
    ajStrDel(&spmap);

    ajStrDel(&cmd);
    ajStrDel(&ssname);
    ajStrDel(&tmp);
    
    ajFileClose(&bfile);
    ajFileClose(&plib);
    ajFileClose(&outtext);
    ajSeqoutDel(&outseq);
    
    embExit();

    return 0;
}
