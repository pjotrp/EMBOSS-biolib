/* @source hmmgen application
**
** Generates a hidden Markov model for each alignment in a directory
** by using the HMMER package.
** 
** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
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
**  Application name
**  hmmgen 
**  
**  
**  
**  Summary
**  Generates a hidden Markov model for each alignment in a directory by using 
**  the HMMER package.
**
**  
**  
**  Input and Output
**  hmmgen reads a directory of scop family alignments file and generates a 
**  hidden Markov model for each alignment by using the HMMER package.
**  The scop Sunid (an integer) of a family will be used as the base name for
**  its hmm file. The paths and extensions for the alignment (input) and
**  hmm (output) files are provided by the user. 
**  
**  
**   
**  Sister applications
**  A 'scop family alignment file' contains a sequence alignment of domains 
**  belonging to the same scop family.  The file is in embl-like format and is
**  annotated with records describing the SCOP classification of the family.  A
**  scop family alignment file generatd by scopalign will contain a structure-
**  based sequence alignment of domains of known structure only.  Such 
**  alignments can be extended with sequence relatives (of unknown structure)
**  to the family in question by using seqalign.
**  The hmm output file is used by modelscan.
**  
**  
**  
**  Notes
**  When running hmmgen at the HGMP it is essential that the command 'use 
**  hmmer' (which runs the script /packages/menu/USE/hmmer) is given 
**  before it is run.  If run elsewhere, hmmgen requires a working version
**  of hmmer.
**  
**  
**  
**  Known bugs & warnings
**  
**  
**  
**  Description
**  This program is part of a suite of EMBOSS applications that directly or 
**  indirectly make use of the protein structure databases pdb and scop.  
**  This program is part of an experimental analysis pipeline described in an
**  accompanying document.  We provide the software in the hope that it will
**  be useful.  The applications were designed for specific research purposes
**  and may not be useful or reliable in contexts other than the described 
**  pipeline.  The development of the suite was coordinated by Jon Ison to
**  whom enquiries and bug reports should be sent (email jison@hgmp.mrc.ac.uk).
**  
**  hmmgen provides a convenient means to generate hidden Markov models
**  for scop families.  The HMMs are descriptive of the family and can be
**  used to identify sequence relatives of the family. 
**  
**  
**  
**  Algorithm
**  
**  
**  
**  Usage 
**  An example of interactive use of hmmgen is shown below.
**  
**  Unix % 
**  Unix % use hmmer
**  Unix % hmmgen
**  Generates a hidden Markov model for each alignment in a directory by using
**  the HMMER package.
**  Location of sequence alignment files (input) [./]: /test_data
**  Extension of sequence alignment files [.ealign]: 
**  Location of HMM files (output) [./]: /test_data
**  Extention of HMM files [.hmm]: 
**  scopalg structure read ok
**  ScopalgWriteClustal2 called ok
**  hmmbuild -g /test_data/55074.hmm ./hmmgen-1032428453.28708.seqsin
**  hmmbuild - build a hidden Markov model from an alignment
**  HMMER 2.1.1 (Dec 1998)
**  Copyright (C) 1992-1998 Washington University School of Medicine
**  HMMER is freely distributed under the GNU General Public License (GPL).
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**  Training alignment:                ./hmmgen-1032428453.28708.seqsin
**  Number of sequences:               108
**  Search algorithm configuration:    Global alignment (hmms)
**  Model construction strategy:       MAP (gapmax hint: 0.50)
**  Null model used:                   (default)
**  Prior used:                        (default)
**  Prior strategy:                    Dirichlet
**  Sequence weighting method:         G/S/C tree weights
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**  Determining effective sequence number    ... done. [32]
**  Weighting sequences heuristically        ... done.
**  Constructing model architecture          ... done.
**  Converting counts to probabilities       ... done.
**  Setting model name, etc.                 ... done. [hmmgen-1032428453.28708]
**  
**  Constructed a profile HMM (length 204)
**  Average score:      128.80 bits
**  Minimum score:      -15.12 bits
**  Maximum score:      184.79 bits
**  Std. deviation:      51.88 bits
**  
**  Finalizing model configuration           ... done.
**  Saving model to file                     ... done. [/test_data/55074.hmm]
**  scopalg structure read ok
**  ScopalgWriteClustal2 called ok
**  hmmbuild -g /test_data/54894.hmm ./hmmgen-1032428455.23721.seqsin
**  hmmbuild - build a hidden Markov model from an alignment
**  HMMER 2.1.1 (Dec 1998)
**  Copyright (C) 1992-1998 Washington University School of Medicine
**  HMMER is freely distributed under the GNU General Public License (GPL).
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**  Training alignment:                ./hmmgen-1032428455.23721.seqsin
**  Number of sequences:               28
**  Search algorithm configuration:    Global alignment (hmms)
**  Model construction strategy:       MAP (gapmax hint: 0.50)
**  Null model used:                   (default)
**  Prior used:                        (default)
**  Prior strategy:                    Dirichlet
**  Sequence weighting method:         G/S/C tree weights
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**  Determining effective sequence number    ... done. [15]
**  Weighting sequences heuristically        ... done.
**  Constructing model architecture          ... done.
**  Converting counts to probabilities       ... done.
**  Setting model name, etc.                 ... done. [hmmgen-1032428455.23721]
**  
**  Constructed a profile HMM (length 96)
**  Average score:       97.86 bits
**  Minimum score:       48.80 bits
**  Maximum score:      148.57 bits
**  Std. deviation:      28.67 bits
**  
**  Finalizing model configuration           ... done.
**  Saving model to file                     ... done. [/test_data/54894.hmm]
**  Unix % 
**  
**  A hidden Markov model was generated for every scop alignment file with the
**  file extension .ealign in the directory /test_data.  Files
**  with the file extension .hmm were written to /test_data and
**  containing the HMMs.
**  
**  The following command line would achieve the same result.
**  hmmgen /test_data .ealign /test_data .hmm
**  
**
**  
**  Input file format
**  The format of the scop alignment file is described in scopalign.c
**  
**  
**  
**  Output file format
**  The format of the hmm file is described in the HMMER documentation.
**  
**  
**  
**  Data files
**  hmmgen does not use a data file.
**  
**  
**  
**  Diagnostic error messages
**  
**  
**  
**  Authors
**  Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
**  Jon Ison (jison@hgmp.mrc.ac.uk)
**  
**  
**  
**  References
**  Please cite the authors.
**  
**  
**  
******************************************************************************/





#include "emboss.h"


/* @prog hmmgen****** *******************************************************
**
** Generates a hidden Markov model for each alignment in a directory
** by using the HMMER package.
**
******************************************************************************/

int main(int argc, char **argv)
{

    AjPList infpath    = NULL;      /* path to directory containing extended alignments */
    AjPDir outfpath   = NULL;      /* output directory for the HMM profiles */
    AjPStr filename   = NULL;      /* name of extended alignment file */
    AjPStr outfile    = NULL;      /* name of output file a HMM */
    AjPStr tmp        = NULL;      /* temparary string variable */
    AjPStr cmd        = NULL;      /* the unix command line to execute hmmer */
    AjPStr tmpname    = NULL;      /* randomly generated name for hmmer input */
    AjPStr seqsin     = NULL;      /* name of sequence file for input into 
				      hmmer in CLUSTAL format */

    AjPFile inf       = NULL;      /* file pointer for extn alignments */
    AjPFile seqsinf   = NULL;      /* file pointer of seqsin */

    AjPScopalg scopalg= NULL;      /* scopalg object for read in extn alignment files */


    ajNamInit("emboss");
    ajAcdInitP("hmmgen",argc,argv, "DOMAINATRIX");

    infpath  = ajAcdGetDirlist("infpath");
    outfpath = ajAcdGetDirectory("outfpath");

    filename = ajStrNew();
    outfile  = ajStrNew();
    tmp      = ajStrNew();
    cmd      = ajStrNew();
    tmpname  = ajStrNew();
   


    /* read each each extended alignment file and run prophecy to generate profile */
    while(ajListPop(infpath,(void **)&filename))
    {  
        if((inf = ajFileNewIn(filename)) == NULL)
        {
            ajWarn("Could not open file %S\n",filename);
	    ajStrDel(&filename);
            continue;
        }

        else
        {
            /* Initialise random number generator for naming of temp. files
               and create  psiblast input files */
            ajRandomSeed();
            ajStrAssC(&tmpname, ajFileTempName(NULL));
            ajStrAss(&seqsin, tmpname);
            ajStrAppC(&seqsin, ".seqsin");

            /* create an output filename */
	    ajStrAssS(&outfile, ajDirName(outfpath));
	    ajFileDirExtnTrim(&filename);
	    ajStrAssS(&outfile,filename);
	    ajStrAssS(&outfile, ajDirExt(outfpath));


	
    
            /* read alignment file into a scopalg structure */
            ajDmxScopalgRead(inf,&scopalg);
                    
	    printf("scopalg structure read ok\n");
	    fflush(stdout);
	    

            /* open up a file and write out the alignment in CLUSTAL format */
            seqsinf = ajFileNewOut(seqsin);
          
            ajDmxScopalgWriteClustal2(scopalg,seqsinf);
          
	    ajDmxScopalgDel(&scopalg);
	    

	    printf("ScopalgWriteClustal2 called ok\n");
	    fflush(stdout);

            ajFileClose(&seqsinf);
          
            /* construct command line to run hmmer to build model */
            ajFmtPrintS(&cmd,"hmmbuild -g %S %S",outfile,seqsin);

            /* run hmmer */
	    ajFmtPrint("%S\n", cmd);
	    fflush(stdout);
	    system(ajStrStr(cmd));

            /* clean up temperary files */
            ajFmtPrintS(&cmd,"rm %S",seqsin);
            system(ajStrStr(cmd));

	    ajStrDel(&filename);
	    ajFileClose(&inf);
        }
    }
    
    /* clean up */
    ajListDel(&infpath);
    ajDirDel(&outfpath);
    ajStrDel(&outfile);
    ajStrDel(&tmp);
    ajStrDel(&cmd);
    ajStrDel(&tmpname);
    

    /* exit */ 
    ajExit();
    return 0;
}
