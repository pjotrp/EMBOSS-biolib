/* @source ehmmsearch application
**
** EMBOSS wrapper to hmmsearch from Sean Eddy's HMMER package v.2.3.2
** Search a sequence database with a profile HMM.
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





/* @prog ehmmsearch **********************************************************
**
** EMBOSS wrapper to hmmsearch from Sean Eddy's HMMER package v.2.3.2
** Search a sequence database with a profile HMM.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile   hmmfile = NULL;
    AjPSeqall seqfile = NULL;
    ajint           A = 0;
    float           E = 0.0;
    float           T = 0.0;
    ajint           Z = 0;
    AjBool     compat = ajFalse;
    ajint         cpu = 0;
    AjBool      cutga = ajFalse;
    AjBool      cuttc = ajFalse;
    AjBool      cutnc = ajFalse;
    float        dome = 0.0;
    float        domt = 0.0;
    AjBool    forward = ajFalse;
    AjBool    nulltwo = ajFalse;
    AjBool        pvm = ajFalse;
    AjBool        xnu = ajFalse;
    AjPStr    outname = NULL;

    /* Housekeeping variables */
    AjPStr        cmd = NULL;
       




    /* ACD file processing */
    embInitPV("ehmmsearch",argc,argv,"HMMERNEW",VERSION);

    hmmfile = ajAcdGetInfile("hmmfile");
    seqfile = ajAcdGetSeqall("seqfile");
    A       = ajAcdGetInt("A");
    E       = ajAcdGetFloat("E");
    T       = ajAcdGetFloat("T");
    Z       = ajAcdGetInt("Z");
    compat  = ajAcdGetBoolean("compat");
    cpu     = ajAcdGetInt("cpu");
    cutga   = ajAcdGetBoolean("cutga");
    cuttc   = ajAcdGetBoolean("cuttc");
    cutnc   = ajAcdGetBoolean("cutnc");
    dome    = ajAcdGetFloat("dome");
    domt    = ajAcdGetFloat("domt");
    forward = ajAcdGetBoolean("forward");
    nulltwo = ajAcdGetBoolean("nulltwo");
        pvm = ajAcdGetBoolean("pvm");
        xnu = ajAcdGetBoolean("xnu");
    outname = ajAcdGetOutfileName("outfile");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd = ajStrNew();


    /* 2. Build hmmsearch command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajStrAssignS(&cmd, ajAcdGetpathC("hmmsearch"));
    ajFmtPrintAppS(&cmd, " -A %d -E %f -T %f -Z %d", A, E, T, Z);
    if(compat)
	ajStrAppendC(&cmd, " --compat ");
    if(cpu)
      ajFmtPrintAppS(&cmd, " --cpu %d ", cpu);
    if(cutga)
	ajStrAppendC(&cmd, " --cutga ");
    if(cuttc)
	ajStrAppendC(&cmd, " --cuttc ");
    if(cutnc)
	ajStrAppendC(&cmd, " --cutnc ");
    ajFmtPrintAppS(&cmd, " --domE %f --domT %f ", dome, domt);
    if(forward)
	ajStrAppendC(&cmd, " --forward ");
    if(nulltwo)
	ajStrAppendC(&cmd, " --null2 ");
    if(pvm)
	ajStrAppendC(&cmd, " --pvm ");
    if(xnu)
	ajStrAppendC(&cmd, " --xnu ");
    ajFmtPrintAppS(&cmd, " %s %S", 
		   ajFileGetNameC(hmmfile),
		   ajSeqallGetFilename(seqfile));


    /* 3. Close ACD files. */
    ajFileClose(&hmmfile);
    ajSeqallDel(&seqfile);


    /* 4. Call hmmsearch.  Use C system call instead of ajSystem
       so that redirect in cmd works ok. */
    ajFmtPrint("\n%S > %S\n\n", cmd, outname);
    ajSysExecOutnameAppendS(cmd, outname);

    /* 5. Exit cleanly */
    ajStrDel(&cmd);
    ajStrDel(&outname);
    
    embExit();

    return 0;
}
