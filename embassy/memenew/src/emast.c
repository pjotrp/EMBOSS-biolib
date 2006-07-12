/* @source emast application
**
** EMBOSS wrapper to meme from Timothy Bailey's MEME package version 3.0.14 
** Searches sequences for motifs. 
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





/* @prog emast ****************************************************************
**
** EMBOSS wrapper to meme from Timothy Bailey's MEME package version 3.0.14 
** Searches sequences for motifs. 
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile mfile    = NULL;
    AjPFile d        = NULL;
    AjPFile a        = NULL;
    AjPFile bfile    = NULL;
    ajint   smax     = 0;
    float   ev       = 0.0;
    float   mt       = 0.0;
    AjBool  isstdin    = ajFalse;
    AjBool  text     = ajFalse;
    AjBool  dna      = ajFalse;
    AjBool  comp     = ajFalse;
    ajint   rank     = 0;
    AjBool  best     = ajFalse;
    AjBool  remcorr  = ajFalse;
    AjBool  brief    = ajFalse;
    AjBool  b        = ajFalse;
    AjBool  nostatus = ajFalse;
    AjBool  hitlist  = ajFalse;
    ajint   c        = 0;
    AjBool  sep      = ajFalse;
    AjBool  norc     = ajFalse;
    AjBool  w        = ajFalse;
    AjBool  seqp     = ajFalse;
    AjPStr  mf       = NULL;
    AjPStr  df       = NULL;
    ajint   minseqs  = 0;
    float   mev      = 0.0;
    ajint   m        = 0;
    AjPStr  diag     = NULL;
    AjPFile outfile  = NULL;

    /* Housekeeping variables */
    AjPStr          cmd = NULL;
    AjPStr  outfilename = NULL;   /* Name of outfile */
    AjPStr  stdoutname  = NULL;   /* Name of temp. file for holding data written to stdout */
    AjPStr  stdouttemp  = NULL;   /* Temp. string */
    AjPFile stdoutf     = NULL;

    /* ACD file processing */
    embInitP("emast",argc,argv,"MEMENEW");
    mfile    = ajAcdGetInfile("mfile");
    d        = ajAcdGetInfile("d");
    a        = ajAcdGetInfile("a");
    bfile    = ajAcdGetInfile("bfile");
    smax     = ajAcdGetInt("smax");
    ev       = ajAcdGetFloat("ev");
    mt       = ajAcdGetFloat("mt");
    isstdin    = ajAcdGetBool("stdin");
    text     = ajAcdGetBool("text");
    dna      = ajAcdGetBool("dna");
    comp     = ajAcdGetBool("comp");
    rank     = ajAcdGetInt("rank");
    best     = ajAcdGetBool("best");
    remcorr  = ajAcdGetBool("remcorr");
    brief    = ajAcdGetBool("brief");
    b        = ajAcdGetBool("b");
    nostatus = ajAcdGetBool("nostatus");
    hitlist  = ajAcdGetBool("hitlist");
    c        = ajAcdGetInt("c");
    sep      = ajAcdGetBool("sep");
    norc     = ajAcdGetBool("norc");
    w        = ajAcdGetBool("w");
    seqp     = ajAcdGetBool("seqp");
    mf       = ajAcdGetString("mf");
    df       = ajAcdGetString("df");
    minseqs  = ajAcdGetInt("minseqs");
    mev      = ajAcdGetFloat("mev");
    m        = ajAcdGetInt("m");
    diag     = ajAcdGetString("diag");
    outfile  = ajAcdGetOutfile("outfile");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd         = ajStrNew();
    outfilename = ajStrNew();
    stdoutname  = ajStrNew();
    stdouttemp  = ajStrNew();
    ajStrAssignC(&stdoutname, ajFileTempName(NULL));


    /* 2. Build emast command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. Original MAST options (in order they appear in ACD file)
       iii.Original MAST options (that don't appear in ACD file)
       iv. EMBASSY MAST new qualifiers and parameters.
       */
    if(!ajNamGetValueC("mast", &cmd))
	ajStrAssignC(&cmd, "mast");
    ajFmtPrintAppS(&cmd, " %s ", ajFileName(mfile));

    /* Warn user if both d and isstdin are specified and use only d */
    if(d && isstdin)
	ajWarn("Database options < -d > and < -stdin > were both set, only < -d > will be used!");
    if(d)
	ajFmtPrintAppS(&cmd, " -d %s ", ajFileName(d));
    else if(isstdin)
	ajFmtPrintAppS(&cmd, " -stdin ");
    /* Presume 'a' is a file .. it doesn't say in the MAST docs ! */
    if((d || isstdin) && a)
	ajFmtPrintAppS(&cmd, " -a %s ", ajFileName(a));
    if(b)
	ajFmtPrintAppS(&cmd, " -b ");
    if(bfile)
	ajFmtPrintAppS(&cmd, " -bfile %s ", ajFileName(bfile));
    if(smax != -1)
	ajFmtPrintAppS(&cmd, " -smax %d ", smax);
    ajFmtPrintAppS(&cmd, " -ev %f ", ev);
    ajFmtPrintAppS(&cmd, " -mt %f ", mt);
    if(text)
	ajFmtPrintAppS(&cmd, " -text ");
    if(dna)
	ajFmtPrintAppS(&cmd, " -dna ");
    if(comp)
	ajFmtPrintAppS(&cmd, " -comp ");
    if(rank != -1)
	ajFmtPrintAppS(&cmd, " -rank %d ", rank);
    if(best)
	ajFmtPrintAppS(&cmd, " -best ");
    if(remcorr)
	ajFmtPrintAppS(&cmd, " -remcorr ");
    if(brief)
	ajFmtPrintAppS(&cmd, " -brief ");
    if(b)
	ajFmtPrintAppS(&cmd, " -b ");
    if(nostatus)
	ajFmtPrintAppS(&cmd, " -nostatus ");
    /* Disregard hitlist if text==ajFalse */
    if(text && hitlist)
	ajFmtPrintAppS(&cmd, " -hitlist ");
    if(c != -1)
	ajFmtPrintAppS(&cmd, " -c %d ", c);
    if(sep)
	ajFmtPrintAppS(&cmd, " -sep ");
    if(norc)
	ajFmtPrintAppS(&cmd, " -norc ");
    if(w)
	ajFmtPrintAppS(&cmd, " -w ");
    if(seqp)
	ajFmtPrintAppS(&cmd, " -seqp ");
    if(MAJSTRGETLEN(mf))
	ajFmtPrintAppS(&cmd, " -mf %S ", mf);
    if(MAJSTRGETLEN(df))
	ajFmtPrintAppS(&cmd, " -df %S ", df);
    if(minseqs != -1)
	ajFmtPrintAppS(&cmd, " -minseqs %d ", minseqs);
    if(mev != -1)
	ajFmtPrintAppS(&cmd, " -mev %f ", mev);
    if(m != -1)
	ajFmtPrintAppS(&cmd, " -m %d ", m);
    /* Not certain diag is really a string - it's not clear in the documentation ! */
    if(MAJSTRGETLEN(diag))
	ajFmtPrintAppS(&cmd, " -diag %S ", diag);
    ajFmtPrintAppS(&cmd, " > %S ", stdoutname);
    
    
    /* 3. Close files from ACD before calling mast */
    ajFileClose(&mfile);
    ajFileClose(&d);
    ajFileClose(&a);
    ajFileClose(&bfile);


    /* 4. Call mast */
    /* ajFmtPrint("\n%S\n", cmd);  */
    system(ajStrGetPtr(cmd));    


    /* 5. The mast output file name is hard-coded to a name derived from the 
       mast command-line (white-space removed, ".html" appended and other
       undocumented subtle differences). The name of this file is written
       to stdout, caught in the file stdoutf above.  This contains a 
       single line, e.g. 

       Writing to file mast.fred.evs10.000000.mt0.000100.html

       Rename the mast output file to that specified in the ACD file.
       Remove stdoutf */
    stdoutf = ajFileNewIn(stdoutname);
    if(!ajFileReadLine(stdoutf, &stdouttemp))
	ajFatal("File read error on mast output file (stdout)");
    if(!ajFmtScanS(stdouttemp, "%*s %*s %*s %S", &outfilename))
    	ajFatal("File read error on mast output file (stdout)");
    ajFmtPrintS(&cmd, "mv %S %S", outfilename , ajFileGetName(outfile));
    system(ajStrGetPtr(cmd));    
    ajFmtPrintS(&cmd, "rm %S", stdoutname);
    system(ajStrGetPtr(cmd));


    /* 6. Exit cleanly */
    /* Close file from ACD */
    ajFileClose(&outfile);
    /* Free housekeeping variables */
    ajStrDel(&cmd);
    ajStrDel(&stdoutname);
    ajStrDel(&stdouttemp);
    ajStrDel(&outfilename);
    ajFileClose(&stdoutf);
    
    ajExit();

    return 0;
}
