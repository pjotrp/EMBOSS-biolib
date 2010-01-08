/* @source ehmmcalibrate application
**
** EMBOSS wrapper to hmmcalibrate from Sean Eddy's HMMER package v.2.3.2
** Calibrate HMM search statistics.
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





/* @prog ehmmcalibrate *******************************************************
**
** EMBOSS wrapper to hmmcalibrate from Sean Eddy's HMMER package v.2.3.2
** Calibrate HMM search statistics.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPFile hmmfilein = NULL;
    ajint         cpu  = 0;
    ajint       fixed  = 0;
    float      mean  = 0.0;
    ajint         num  = 0;
    AjBool        pvm  = ajFalse;
    float        sd  = 0.0;
    ajint        seed  = 0;
    AjPFile hmmfileout = NULL;
    AjPFile   histfile = NULL;

    /* Housekeeping variables */
    AjPStr         cmd = NULL;
    AjPStr         tmp = NULL;
    AjPStr         rnd = NULL;
    AjPStr      inname = NULL;
    AjPStr     outname = NULL;





    /* ACD file processing */
    embInitPV("ehmmcalibrate",argc,argv,"HMMERNEW",VERSION);

    hmmfilein  = ajAcdGetInfile("inhmmfile");
    cpu        = ajAcdGetInt("cpu");
    fixed      = ajAcdGetInt("fixed");
    mean       = ajAcdGetFloat("mean");
    num        = ajAcdGetInt("num");
    pvm        = ajAcdGetBoolean("pvm");
    sd         = ajAcdGetFloat("sd");
    seed       = ajAcdGetInt("seed");
    hmmfileout = ajAcdGetOutfile("outhmmfile");
    histfile   = ajAcdGetOutfile("histfile");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd     = ajStrNew();
    tmp     = ajStrNew();
    rnd     = ajStrNew();
    inname  = ajStrNew();
    outname = ajStrNew();
    ajStrAssignC(&inname, ajFileGetNameC(hmmfilein));
    ajStrAssignC(&outname, ajFileGetNameC(hmmfileout));


    /* 2. Copy the input file (HMMER will overwrite it) */
    ajFilenameSetTempname(&rnd);
    ajFmtPrintS(&cmd, "cp %S %S", inname, rnd);
    /* ajFmtPrint("\n%S\n", cmd); */
    system(ajStrGetPtr(cmd));


    /* 3. Build hmmcalibrate command-line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajStrAssignS(&cmd, ajAcdGetpathC("hmmcalibrate"));
    if(cpu)
	ajFmtPrintAppS(&cmd, " --cpu %d ", cpu);
    if(fixed)
	ajFmtPrintAppS(&cmd, " --fixed %d ", fixed);
    ajFmtPrintAppS(&cmd, " --mean %f ", mean);
    ajFmtPrintAppS(&cmd, " --num %d ", num);
    if(pvm)
	ajStrAppendC(&cmd, " --pvm ");    
    ajFmtPrintAppS(&cmd, " --sd %f ", sd);
    ajFmtPrintAppS(&cmd, " --seed %d ", seed);
    if(histfile)
	ajFmtPrintAppS(&cmd, " --histfile %s ", ajFileGetNameC(histfile));
    ajStrAppendS(&cmd, inname);


    /* 4. Close ACD files */
    ajFileClose(&hmmfilein);
    ajFileClose(&hmmfileout);
    ajFileClose(&histfile);


    /* 5. Call hmmcalibrate */
    ajFmtPrint("\n%S\n\n", cmd);
    ajSysSystem(cmd);
    

    /* 6. Copy HMMER output (overwritten input file) to output file. 
       Restore original input file. */
    ajFmtPrintS(&cmd, "mv %S %S", inname, outname);
    /*    ajFmtPrint("\n%S\n\n", cmd);  */
    ajSysSystem(cmd);
    ajFmtPrintS(&cmd, "mv %S %S", rnd, inname);
    /*    ajFmtPrint("\n%S\n\n", cmd);  */
    ajSysSystem(cmd);
    

    /* 7. Exit cleanly */
    ajStrDel(&cmd);
    ajStrDel(&tmp);
    ajStrDel(&rnd);
    ajStrDel(&inname);
    ajStrDel(&outname);

    embExit();

    return 0;
}
