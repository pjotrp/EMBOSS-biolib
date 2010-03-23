/* @source ehmmbuild application
**
** EMBOSS wrapper to hmmbuild from Sean Eddy's HMMER package v.2.3.2
** Build a profle HMM from an alignment.
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





/* @prog ehmmbuild ***********************************************************
**
** EMBOSS wrapper to hmmbuild from Sean Eddy's HMMER package v.2.3.2
** Build a profle HMM from an alignment.
**
******************************************************************************/

int main(int argc, char **argv)
{
    /* ACD data item variables */
    AjPSeqset alignfile = NULL; 
    AjPFile       prior = NULL;
    AjPFile        null = NULL;
    AjPFile         pam = NULL;
    float        pamwgt = 0.0;
    AjPStr         nhmm = NULL;
    AjPStr     strategy = NULL;
    ajint      pbswitch = 0;
    float       archpri = 0.0;
    AjBool      binary  = ajFalse;
    AjBool         fast = ajFalse;
    float        gapmax = 0.0;
    AjBool         hand = ajFalse;
    float       idlevel = 0.0;
    AjBool        noeff = ajFalse;
    float       swentry = 0.0;
    float        swexit = 0.0;
    AjBool    verbosity = ajFalse;
    AjPStr    weighting = NULL;
    AjPFile     hmmfile = NULL;
    AjPFile           o = NULL;
    AjPFile       cfile = NULL;

    /* Housekeeping variables */
    AjPStr          cmd = NULL;
    AjPStr         rnd1 = NULL;
    AjPStr         rnd2 = NULL;
    AjPStr          fmt = NULL;
    char         option;
    AjBool        fmtok = ajFalse;
    AjPStr  hmmfilename = NULL;
    




    /* ACD file processing */
    embInitPV("ehmmbuild",argc,argv,"HMMERNEW",VERSION);

    alignfile = ajAcdGetSeqset("alignfile");
    prior     = ajAcdGetInfile("prior");
    null      = ajAcdGetInfile("null");
    pam       = ajAcdGetInfile("pam");
    pamwgt    = ajAcdGetFloat("pamwgt");
    nhmm      = ajAcdGetString("nhmm");
    strategy  = ajAcdGetListSingle("strategy");
    pbswitch  = ajAcdGetInt("pbswitch");
    archpri   = ajAcdGetFloat("archpri");
    binary    = ajAcdGetBoolean("binary");
    fast      = ajAcdGetBoolean("fast");
    gapmax    = ajAcdGetFloat("gapmax");
    hand      = ajAcdGetBoolean("hand");
    idlevel   = ajAcdGetFloat("sidlevel");
    noeff     = ajAcdGetBoolean("noeff");
    swentry   = ajAcdGetFloat("swentry");
    swexit    = ajAcdGetFloat("swexit");
    verbosity = ajAcdGetBoolean("verbosity");
    weighting = ajAcdGetListSingle("weighting");
    hmmfile   = ajAcdGetOutfile("hmmfile");
    o         = ajAcdGetOutfile("o");
    cfile     = ajAcdGetOutfile("cfile");





    /* MAIN APPLICATION CODE */
    /* 1. Housekeeping */
    cmd  = ajStrNew();
    rnd1 = ajStrNew();
    rnd2 = ajStrNew();
    fmt  = ajStrNew();
    hmmfilename = ajStrNew();

    ajStrAssignC(&hmmfilename, ajFileGetNameC(hmmfile));


    /* 2. Ensure alignfile is in format HMMER can understand.  These include
       FASTA, GENBANK,EMBL, GCG, PIR, STOCKHOLM, SELEX, MSF,CLUSTAL and PHYLIP.
       EMBOSS name definitions are taken from seqInFormatDef in ajseqread.c and
       seqOutFormat in ajseqwrite.c */
    fmtok=ajFalse;
    ajStrAssignS(&fmt, ajSeqsetGetFormat(alignfile));
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
	ajFatal("Input alignment ('alignfile' ACD option) is not in format "
		"HMMER understands. Please use a a file in FASTA, GENBANK, "
		"EMBL, GCG, PIR, STOCKHOLM, SELEX, MSF,CLUSTAL or PHYLIP format.");
    

    /* 3. Build hmmbuild command line */
    /* Command line is built in this order: 
       i.  Application name.
       ii. HMMER 'options' (in order they appear in ACD file)
       iii.HMMER 'options' (that don't appear in ACD file)
       iv. HMMER & new parameters.
       */
    ajStrAssignS(&cmd, ajAcdGetpathC("hmmbuild"));
    if(prior)
	ajFmtPrintAppS(&cmd, " --prior %s ", ajFileGetNameC(prior));
    if(null)
	ajFmtPrintS(&cmd, " --null %s ", ajFileGetNameC(null));
    if(pam)
	ajFmtPrintAppS(&cmd, " --pam %s  --pamwgt %f ", ajFileGetNameC(pam), pamwgt);
    ajFmtPrintAppS(&cmd, " -n %S ", nhmm);

    /* ACD option only allows one selection */
    option = ajStrGetCharFirst(strategy);
    if(option == 'F')
	ajStrAppendC(&cmd, " -f ");
    else if(option == 'G')
	ajStrAppendC(&cmd, " -g ");
    else if(option == 'S')
	ajStrAppendC(&cmd, " -s ");
    /* else go with default ('D' option in ACD file) */
    ajFmtPrintAppS(&cmd, " --pbswitch %d ", pbswitch);
    ajFmtPrintAppS(&cmd, " --archpri %f ", archpri);
    if(binary)
	ajStrAppendC(&cmd, " --binary ");
    if(fast)
	ajFmtPrintAppS(&cmd, " --fast --gapmax %f ", gapmax);
    if(hand)
	ajStrAppendC(&cmd, " --hand ");
    ajFmtPrintAppS(&cmd, " --idlevel %f ", idlevel);
    if(noeff)
	ajStrAppendC(&cmd, " --noeff ");
    ajFmtPrintAppS(&cmd, " --swentry %f ", swentry);
    ajFmtPrintAppS(&cmd, " --swexit %f ", swexit);
    if(verbosity)
	ajStrAppendC(&cmd, " --verbose ");

    /* ACD option only allows one selection */
    option = ajStrGetCharFirst(weighting);
    if(option == 'B')
	ajStrAppendC(&cmd, " --wblosum ");
    else if(option == 'G')
	ajStrAppendC(&cmd, " --wgsc ");
    else if(option == 'K')
	ajStrAppendC(&cmd, " --wme ");
    else if(option == 'W')
	ajStrAppendC(&cmd, " --wpb ");
    else if(option == 'V')
	ajStrAppendC(&cmd, " --wvoronoi ");
    else if(option == 'N')
	ajStrAppendC(&cmd, " --wnone ");
    if(o)
	ajFmtPrintAppS(&cmd, " -o %s ", ajFileGetNameC(o));
    if(cfile)
	ajFmtPrintAppS(&cmd, " --cfile %s ", ajFileGetNameC(cfile));
    /* -A (append) always set but file will be wiped by EMBOSS first unless 
       append: "Y" is set for "hmmfile" in the ACD file. */
    ajStrAppendC(&cmd, " -A -F ");
    ajFmtPrintAppS(&cmd, " %S %S", hmmfilename, ajSeqsetGetFilename(alignfile));


    /* 4. Close ACD files */
    ajSeqsetDel(&alignfile);
    ajFileClose(&prior);
    ajFileClose(&null);
    ajFileClose(&pam);
    ajFileClose(&hmmfile);
    ajFileClose(&o);
    ajFileClose(&cfile);


    /* 5. Call hmmbuild */
    ajFmtPrint("\n%S\n", cmd); 
    ajSysExecS(cmd);    


    /* 6. Exit cleanly */
    ajStrDel(&nhmm);
    ajStrDel(&cmd);
    ajStrDel(&rnd1);
    ajStrDel(&rnd2);
    ajStrDel(&fmt);
    ajStrDel(&hmmfilename);
    ajStrDel(&strategy);
    ajStrDel(&weighting);
    
    embExit();

    return 0;
}
