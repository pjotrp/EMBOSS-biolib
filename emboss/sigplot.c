/* @source sigplot application
**
** Reads a signature hits file and validation file and generates gnuplot data 
** files of signature performance
**
** @author: Copyright (C) Matt Blades (mblades@hgmp.mrc.ac.uk)
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
******************************************************************************
** 
** 
** 
******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE     
******************************************************************************
**
** Mon May 20 11:43:39 BST 2002
**
** The following documentation is out-of-date and should be disregarded.  It 
** will be updated shortly. 
** 
******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE     
******************************************************************************
** 
** 
** 
** Operation
** 
** Sigplot generates data files for the GNUPLOT graph plotting program, and 
** produces files of residue similarity between hit sequences and the
** sequences of the structural alignment from which the signature was derived.
** 
** 
** Graph plotting
** --------------
**
** Two graphs are generated:
**
** i.   A graph of signature performance indicating the proportion of hits
**      detected by the signature that are of type 'TRUE', 'FALSE', 'CROSS'
**      or 'UNKNOWN'.
**
** ii.  A graph of sensitivity and specificity of the signature, where:

**      
** 
** sensitivity = no. true positives / (no. true positives +
**                                              no. false negatives)
**
**
** specificity = no. true negatives / (no. true negatives + 
**                                              no. false positives)
**
** In generating the signature performance graph sigplot parses a 
** hits file from the sigscan application, from which the proportion of
** true, cross, false, and unknown hits are determined.  4 gnuplot type 
** data files are generated, specifying the proportion of each hit
** classification. Also generated is a GNUPLOT driver file which 
** contains the information specifying the type and format of the graph 
** to be plotted and the name of the four data files which contain the 
** information to be plotted. To plot the graph using GNUPLOT, 
** type the command:
**
**              load "test.dat"
**
** Where test.dat is the name of the driver file.
**
**
** Plotting sensitivity and specificity graphs require sigplot to  
** read the sigscan hits file, but also a validations file from the 
** seqsort application (see seqsort documentation).  Sensitivity and 
** specificity are calculated according to the above equations, where:
**      
** true positives = 'TRUE' hits from the sigscan hits file (i.e. actual
**                   members of the family from which the signature 
**                   was generated)
**
** false positives = 'FALSE' hits from the sigscan hits file (i.e. hits
**                    that belong to a family of a DIFFERENT fold to the
**                    family from whcih the signature was generated)
**
** true negatives  = (total number of sequences that are found to be
**                   homologous to a family of a different fold to that  
**                   from which the signature was generated (value obtained 
**                   from the validation file)) - the number of false 
**                   positives.
**
** false negatives = (total number of sequences found to be homologous to
**                   a family of the same fold to that from which the 
**                   signature was generated (value obtained from validation
**                   file)) - the number of true positives.
**
** Two GNUPLOT data files are produces specifying the values of sensitivity
** and specificity, and a driver file to plot the graph.
**
**
** Residue similarity determination
** --------------------------------
**
** Sigplot produces a matrix of percentage similarity between the sequences
** of the structural (seed) alignment (produced by the scopalign application), 
** from which signatures are generated (signature application).
**
** Needleman and Wunsch algorithm is used to determine % similarity.
**
** Sigplot also calculates and outputs the % similarity of every 'TRUE' 
** hit from the sigscan hits file compared to the sequences from the
** seed alignment.
**
** In producing this data sigplot reads the sigscan hits file, the 
** seed alignment and the sigscan alignment file.
**
**
******************************************************************************
** 
** Operation
** 
** Sigplot parses a signature hits output file generated by the EMBOSS 
** application sigscan, and produces data files suitable for use in the 
** graph plotting application GNUPLOT.  Sigplot parses the secondary 
** classification column of the signature hits output file (Figure 1).
** The data files produced by sigplot when plotted using GNUPLOT, display 
** a single graph indicating the performance of the signature.  The x axis 
** of the graph shows the number of hits and the y axis shows the proportion 
** of the hits which are 'TRUE', 'CROSS', 'FALSE' and 'UNKNOWN', see 
** classification of hits (Figure 1).
**
**
** Excerpts from a signature hits (Figure 1) are shown. The records used 
** are are as follows
** (1) DE - bibliographic information. The text 'Results of signature search'
** is always given.
** Four SCOP classification records are given:
** (2)  CL - Domain class.  It is identical to the text given after 'Class' in 
** the scop classification file (see documentation for the EMBOSS application 
** scope).
** (3)  FO - Domain fold.  It is identical to the text given after 'Fold' in 
** the scop classification file (see scope documentation).
** (4)  SF - Domain superfamily.  It is identical to the text given after 
** 'Superfamily' in the scop classification file (see scope documentation).
** (5)  FA - Domain family. It is identical to the text given after 'Family' in 
** the scop classification file (see scope documentation).
** (6) HI - hit data.  The data are as follows (column numbers are given in 
** parentheses). (i) HI is always given. (ii) Rank-order of the hit. (iii) 
** Database identifier code. (iv) The group number of the protein if a grouped 
** scop families file was provided or '.' otherwise. (v) The primary
** classification of the hit. For true hits (genuine relatives to the signature)
** one of 'TRAIN', 'PSIBLAST' or 'OTHER' is given. Otherwise, 'CROSS', 'FALSE'
** or 'UNKNOWN' is given ('.' is given if a scop families file was not 
** provided). (vi) The secondary classification of the hit, either 'FALSE', 
** 'TRUE' or 'UNKNOWN' ('.' is given if a scop families file was not 
** provided). The classes of hits are defined below.  
** (vii) Score of sequence-signature match.  (viii) E-value (see below).
** (7) XX - used for spacing.
** (8) // - The file ends with a line containing '//' only.
** 
** 
** Figure 1   Excerpt from a signature hits file
** 
** DE   Results of signature search
** XX
** CL   All alpha proteins
** XX
** FO   Globin-like
** XX
** SF   Globin-like
** XX
** FA   Globins
** XX
** XX
** HI   1    1RBPDFG   1    TRUE     TRUE    234  0.0001 
** HI   2    1GFT35J   3    TRUE     TRUE    234  0.0008 
** HI   3    1KJUFGH   1    TRUE     TRUE    224  0.0108 
** HI   4    1GYU15R   2    CLOSE    TRUE    220  0.1876 
** HI   5    1LKI89O   2    CLOSE    TRUE    203  0.6787 
** HI   6    1QRTY58   1    TRUE     TRUE    199  0.9978 
** HI   7    2IOM78G   1    FALSE    FALSE   198  1.0844
** HI   8    1SZR234   1    CLOSE    TRUE    198  1.4343 
** HI   9    3PONI57   1    FALSE    FALSE   197  2.8849 
** HI  10    1PHDJBS   3    CLOSE    TRUE    190  2.9872
** HI  11    1HIOHDW   1    UNKNOWN  UNKNOWN 160  5,8676 
** HI  12    199976T   1    CLOSE    TRUE    140  8.8346 
** XX
** //
** 
** 
**  Notes
**  MJB - ADD IN FORMAT OF GNUPLOT DATA FILES HERE!!!!
******************************************************************************/


#include "emboss.h"
#include <math.h>




typedef struct AjSScopdata
 {
    AjPStr   Class;
    AjPStr   Fold;
    AjPStr   Superfamily;
    AjPStr   Family;
    AjPStr   file_name;
    ajint    num_hits;            /* Total no. of hits */
    ajint    Sunid;

    ajint    num_true;            /* Number of hits classified 'TRUE' */
} AjOScopdata, *AjPScopdata;








AjBool  sigplot_CountHits(AjPFile hitsin, AjPScopdata *data, AjPInt *truehits,
                          AjPInt2d *range, AjPStr **codes, AjPInt *rank);
AjBool  sigplot_ValidatRead(AjPFile validatin, AjPScopdata *data, ajint *vali_TN,
                            ajint *vali_FN);
AjBool  sigplot_HitProportion(AjPFile hitsin, AjPScopdata *data, AjPFloat2d *prob_array,
                              AjPFloat *sensi_array, AjPFloat *speci_array,
                              ajint vali_TN, ajint vali_FN);
AjBool  sigplot_DataWrite(AjPFile datafile, AjPFile ssdatafile, AjPScopdata data, 
                          AjPFloat2d prob_array, AjPFloat sensi_array, AjPFloat speci_array);
AjBool  sigplot_SeedIdCalc(AjPList seedlist,  AjPScopalg alg, AjPFile matrixout,
                           AjPMatrixf submat, float gapopen, float gapextn); 
AjBool  sigplot_HitIdCalc(AjPList seedlist, AjPList hitlist,  AjPScopalg alg, 
                          AjPFile matrixout, AjPMatrixf submat, float gapopen, 
                          float gapextn, AjPStr **codes, AjPInt rank); 
AjBool  sigplot_AlignSeqExtract(AjPFile sigalignfile, AjPInt truehits, AjPInt2d range, 
                                AjPStr **sig_seqs, AjPStr **temp_seqs, ajint num,
                                ajint num_true); 
void sigplot_ScopdataDel(AjPScopdata *pthis);
AjPScopdata  sigplot_ScopdataNew();









/* @prog sigplot *************************************************************
**
** Signature performance plotting program
**
******************************************************************************/
int main(int argc, char **argv)
{

    AjPFile     hitsin      = NULL;      /* Signature hits input file                      */
    AjPFile     validatin   = NULL;      /* Validation input file                          */
    AjPFile     datafile    = NULL;      /* GNUPLOT class output data file                 */
    AjPFile     ssdatafile  = NULL;      /* GNUPLOT sensi/speci output data file           */
    AjPFile     sigalignfile = NULL;     /* sigscan alignment file                         */
    AjPFile     alignfile   = NULL;      /* Alignment file for input                       */
    AjPFile     matrixout   = NULL;      /* output file for id matrix                      */

    ajint       vali_TN     = NULL;      /* ajint for storing validation information       */
    ajint       vali_FN     = NULL;      /* ajint for storing validation information       */
    ajint       nseqs       = 0;         /* No. of sequences                               */
    ajint       x           = 0;         /* Counter                                        */
    AjPInt      truehits    = NULL;      /* Array of hits that are 'TRUE'                  */
    AjPInt      rank        = NULL;      /* Array of the rank of the 'TRUE' hits           */
    AjPInt2d    range       = NULL;      /* Array of range of seq-sig matches of 'True' hits */

    AjPFloat2d  prob_array  = NULL;      /* Array for probabilities of each classification */    
    AjPFloat    sensi_array = NULL;      /* Array for specificity data                     */ 
    AjPFloat    speci_array = NULL;      /* Array for sensitivity data                     */ 
    float       gapopen     = 0;         /* Gap opening penalty                            */   
    float       gapextn     = 0;         /* Gap extension penalty                          */           

    AjPScopdata data        = NULL;      /* Pointer to Scopdata structure                  */
    AjPScopalg  alg         = NULL;      /* Pointer to Scopalg structure                   */

    AjBool      seedid      = ajFalse;   /* bool for generating seed id's                  */
    AjPMatrixf  submat      = NULL;      /* subsitiution matirx for use in seed id         */
    AjPStr      *seed_array = NULL;      /* Arrays of seqs from seed alignment             */
    AjPList     seedlist    = NULL;      /* List to hold seed sequences                    */
    AjPList     hitlist     = NULL;      /* List to hold hit sequences                     */
    AjIList     iter        = NULL;      /* A list iterator                                */
    AjIList     iter2       = NULL;      /* A list iterator                                */
    AjPSeq      seed_seq    = NULL;      /* A sequence object to hold the constructed sequence */
    AjPSeq      hit_seq     = NULL;      /* A sequence object to hold the constructed sequence */

    
    AjPStr      *codes      = NULL;
    AjPStr      *sig_seqs   = NULL;
    AjPStr      *temp_seqs  = NULL;
    

    embInit("sigplot", argc, argv);

    
    /* Assign variables */
    seedlist = ajListstrNew();
    hitlist  = ajListstrNew();
       
    /* GET VALUES FROM ACD */
    hitsin      = ajAcdGetInfile("hitsin");
    validatin   = ajAcdGetInfile("validatin");
    datafile    = ajAcdGetOutfile("datafile");
    ssdatafile  = ajAcdGetOutfile("ssdatafile");
    seedid      = ajAcdGetBool("seedid");
    if(seedid)
    {
	alignfile   = ajAcdGetInfile("alignfile");
	sigalignfile= ajAcdGetInfile("sigalignfile");
	submat      = ajAcdGetMatrixf("submat");
	gapopen     = ajAcdGetFloat("gapopen");
	gapextn     = ajAcdGetFloat("gapextn");
	matrixout   = ajAcdGetOutfile("matrixout");
    }
    

    /* Check signature hits file */
    if(!hitsin)
        ajFatal("Could not open signature hits file\n");
    else
        ajFmtPrint("Signature hits file read ok\n");

    /* Check validation file */
    if(!validatin)
        ajFatal("Could not open validation file\n");
    else
        ajFmtPrint("Validation file read ok\n");
    

    /* Check seed id requirements */

    if(seedid == ajTrue)
    {
        if(!alignfile)
            ajFatal("Could not open alignments directory");

        else
            ajFmtPrint("Alignment file read ok\n");
    }


        
    


    /* Call sigplot_CountHits */
    sigplot_CountHits(hitsin, &data, &truehits, &range, &codes, &rank);

    ajFmtPrint("Number of hits in hits file = %d\n", (data)->num_hits);


    /* Call sigplot_ValidatRead */

    sigplot_ValidatRead(validatin, &data, &vali_TN, &vali_FN);


    /* Call sigplot_HitProportion */
    sigplot_HitProportion(hitsin, &data, &prob_array, &sensi_array, &speci_array,
                          vali_TN, vali_FN);
    

    /* Call sigplot_DataWrite */
    sigplot_DataWrite(datafile, ssdatafile, data, prob_array, sensi_array, speci_array);


    /* Call sigplot_AlignSeqExtract */
    sigplot_AlignSeqExtract(sigalignfile, truehits, range, &sig_seqs, &temp_seqs, 
                            data->num_hits, data->num_true); 


    /* Section for generating AjPSeq lists     */
    /* to pass to seq id calculating functions */

    /* Read alignment file, write Scopalgn structure */ 
    ajXyzScopalgRead(alignfile, &alg);

    /* Remove gaps from Seqs array and assign to seed_array */
    nseqs=ajXyzScopalgGetseqs(alg, &seed_array);
    

    /* Push seed sequences onto list */
    for(x=0;x<nseqs;x++)
    {
        seed_seq = ajSeqNew();  
        ajStrAss(&seed_seq->Seq, seed_array[x]);
        ajListPushApp(seedlist, seed_seq); 
    }

    /* Push hit sequences onto list */
    for(x=0;x<data->num_true;x++)
    {
        hit_seq = ajSeqNew();   
        ajStrAss(&hit_seq->Seq, sig_seqs[x]);
        ajListPushApp(hitlist, hit_seq); 
    }

    /* Call sigplot_SeedIdCalc */
    sigplot_SeedIdCalc(seedlist, alg, matrixout, submat, gapopen, gapextn); 

    /* Call sigplot_HitIdCalc */
    sigplot_HitIdCalc(seedlist, hitlist, alg, matrixout, submat, gapopen, gapextn,
                      &codes, rank);



    /* Tidy up */

    /* Delete string arrays and pointers */
    for(x=0;x<alg->N;x++)
        ajStrDel(&seed_array[x]);       
    AJFREE(seed_array);
    
    for(x=0;x<data->num_hits;x++)
        ajStrDel(&codes[x]);    
    AJFREE(codes);

    for(x=0;x<data->num_true;x++)
        ajStrDel(&sig_seqs[x]); 
    AJFREE(sig_seqs);    

    for(x=0;x<data->num_hits;x++)
        ajStrDel(&temp_seqs[x]);        
    AJFREE(temp_seqs);    



    sigplot_ScopdataDel(&data);
    ajFileClose(&hitsin);
    ajFileClose(&datafile);
    ajFileClose(&ssdatafile);
    ajFileClose(&validatin);
    ajFileClose(&alignfile);
    ajFileClose(&sigalignfile);
    ajFileClose(&matrixout);
    ajFloat2dDel(&prob_array);
    ajFloatDel(&sensi_array);
    ajFloatDel(&speci_array);
    ajMatrixfDel(&submat);
    ajXyzScopalgDel(&alg);
    ajListstrDel(&seedlist);
    ajIntDel(&truehits);
    ajIntDel(&rank);
    ajInt2dDel(&range);


    /*delete and clean up seedlist */
    iter=ajListIter(seedlist);
    iter2=ajListIter(hitlist);
    while((seed_seq=(AjPSeq)ajListIterNext(iter)))
        ajSeqDel(&seed_seq);
    while((hit_seq=(AjPSeq)ajListIterNext(iter2)))
        ajSeqDel(&hit_seq);
    ajListIterFree(iter);
    ajListIterFree(iter2);
    ajListDel(&seedlist);

    ajListDel(&hitlist);
    ajSeqDel(&seed_seq);
    ajSeqDel(&hit_seq);
    


    printf("Sigplot finished successfully\n");


    /* Return */
    ajExit();
    return 0;


}


/* @funcstatic sigplot_CountHits  *****************************************
 **
 ** Read signature hits file and count the number of hits, by reading the 
 ** HI line and storing the value for the rank of the hit.
 **
 ** @param [r] hitsin   [AjPFile]      File pointer to signature hits file

 ** @param [w] data     [AjPScopdata*] Scopdata object pointer
 **
 ** @return [AjBool] True on succcess
 ** @@
 ******************************************************************************/
AjBool   sigplot_CountHits(AjPFile hitsin, AjPScopdata *data, AjPInt *truehits,
                           AjPInt2d *range, AjPStr **codes, AjPInt *rank)
{
    
    AjPStr   line           = NULL;     /* Line of text */    
    AjPStr   name           = NULL;     /* String for file name */
    AjPStr   second         = NULL;     /* String for secondary clasification */
    AjPStr   str            = NULL;     /* String */
    AjPStr   code           = NULL;     /* String */
    ajint    num_hits       = 0;        /* Variable to hold no. of hits */
    static   AjPStr class   = NULL;     /* string to hold class             */
    static   AjPStr fold    = NULL;     /* string to hold fold              */
    static   AjPStr super   = NULL;     /* string to hold super             */
    static   AjPStr family  = NULL;     /* string to hold family            */
    ajint    sunid          = 0;
    ajint    hi_cnt         = 0;
    ajint       x = 0;
    ajint       y = 0;
    ajint       start       = 0;
    ajint       end         = 0;
    ajint       true_cnt    = 0;
    ajint       temp        = 0;
    

    /* initialize strings */
    name     = ajStrNew();
    line     = ajStrNew();    
    class    = ajStrNew();
    fold     = ajStrNew();
    super    = ajStrNew();
    family   = ajStrNew();
    second   = ajStrNew();
    str      = ajStrNew();
    code     = ajStrNew();
    
    
    ajStrAssC(&str, "TRUE");

    /* Check signature hits file */
    if(!hitsin)
        ajFatal("Could not open signature hits file\n");


    /* Read files once and determine num_hits         */
    /* To make assigning arrays and strings lengths   */
    /* easier later on                                */
    while(ajFileReadLine(hitsin, &line) && !ajStrPrefixC(line,"//"))
    {
        if(ajStrPrefixC(line,"DE"))
            continue;

        else if(ajStrPrefixC(line,"SI"))
        {
            ajFmtScanS(line, "%*s %d\n", &sunid);
        }
        
        else if(ajStrPrefixC(line,"CL"))
            {
                ajStrAssC(&class,ajStrStr(line)+3);
                ajStrClean(&class);
            }
        else if(ajStrPrefixC(line,"FO"))
        {
            ajStrAssC(&fold,ajStrStr(line)+3);
            while((ajFileReadLine(hitsin,&line)))
            {
                if(ajStrPrefixC(line,"XX"))
                    break;
                ajStrAppC(&fold,ajStrStr(line)+3);
            }
            ajStrClean(&fold);
        }
        else if(ajStrPrefixC(line,"SF"))
        {
            ajStrAssC(&super,ajStrStr(line)+3);
            while((ajFileReadLine(hitsin,&line)))
            {
                if(ajStrPrefixC(line,"XX"))
                    break;
                ajStrAppC(&super,ajStrStr(line)+3);
            }
            ajStrClean(&super);
        }
        else if(ajStrPrefixC(line,"FA"))
        {
            ajStrAssC(&family,ajStrStr(line)+3);
            while((ajFileReadLine(hitsin,&line)))
            {
                if(ajStrPrefixC(line,"XX"))
                    break;
                ajStrAppC(&family,ajStrStr(line)+3);
            }
            ajStrClean(&family);
        }
        else if(ajStrPrefixC(line,"XX"))
            continue;

        /* Start of loop to count number of hits */
        else if(ajStrPrefixC(line,"HI")) 
        {
            ajFmtScanS(line, "%*s %d %*s %*s %*s %*s %*s %S", &num_hits, &second);
                if(ajStrMatch(second, str))
                    true_cnt++;

            /* loop through HI lines */
            /* Read in and store the rank (no. of hits) */
            /* from the HI line */
            while(ajFileReadLine(hitsin, &line) && !ajStrPrefixC(line,"//"))        
            {
                ajFmtScanS(line, "%*s %d %*s %*s %*s %*s %*s %S", &num_hits, &second);
                if(ajStrMatch(second, str))
                    true_cnt++;
            }
        }
    }


    /* Assign arrays and strings */
    *truehits = ajIntNewL(num_hits);
    *rank     = ajIntNewL(true_cnt);
    *range    = ajInt2dNewL(num_hits);
    *codes    = (AjPStr *) AJCALLOC0(num_hits, sizeof(AjPStr));


    /* Fill truehits array */
    for(x=0;x<num_hits;x++)
        ajIntPut(truehits,x,0);

    /* Assign strings in codes array */
    for(x=0;x<true_cnt;x++)    
        (*codes)[x] = ajStrNew();

    
    /* Fill range array */
    for(x=0;x<num_hits;x++)
        for(y=0;y<2;y++)
            ajInt2dPut(range, x, y, 0);


    /* reset posinter to start of file */
    ajFileSeek(hitsin, 0, 0);


    
    /* read file again and fill arrays and strings */
    while(ajFileReadLine(hitsin, &line) && !ajStrPrefixC(line,"//"))
    {
        if(ajStrPrefixC(line,"DE"))
            continue;
        else if(ajStrPrefixC(line,"CL"))
            continue;
        else if(ajStrPrefixC(line,"XX"))
            continue;
        else if(ajStrPrefixC(line,"FO"))
            continue;
        else if(ajStrPrefixC(line,"SF"))
            continue;
        else if(ajStrPrefixC(line,"FA"))
            continue;
        else if(ajStrPrefixC(line,"SI"))
            continue;

        /* Start of loop to parse HI data */
        else if(ajStrPrefixC(line,"HI")) 
        {
            ajFmtScanS(line, "%*s %*d %S %d %d %*s %*s %S", 
                       &code, &start, &end, &second);
            if(ajStrMatch(second, str))
            {
                ajIntPut(truehits, hi_cnt, 1);
                ajInt2dPut(range, hi_cnt, 0, start);
                ajInt2dPut(range, hi_cnt, 1, end);              

                ajStrAssS(&(*codes[temp]), code);
                temp++;
            }
            
            /* loop through HI lines */
            /* Read in and store the rank (no. of hits) */
            /* from the HI line */
            while(ajFileReadLine(hitsin, &line) && !ajStrPrefixC(line,"//"))        
            {
                hi_cnt++;
                ajFmtScanS(line, "%*s %*d %S %d %d %*s %*s %S", 
                       &code, &start, &end, &second);
                if(ajStrMatch(second, str))
                {       
                    ajIntPut(truehits, hi_cnt, 1);              
                    ajInt2dPut(range, hi_cnt, 0, start);
                    ajInt2dPut(range, hi_cnt, 1, end);          
                    ajStrAssS(&(*codes)[temp], code);
                    temp++;
                }
            }
        }
    }


    /* Allocate memory for Scopdata structure */
    (*data) = sigplot_ScopdataNew();

    /* Copy name of hits file to string name  */
    ajStrAss(&name, ajFileGetName(hitsin));

    /* remove trailing '.hits' */
    ajStrSubstituteCC(&name, ".hits", "");



    /* Assign SCOP records */
    (*data)->num_hits = num_hits;
    ajStrAss(&(*data)->file_name, name);
    ajStrAss(&(*data)->Class,class);
    ajStrAss(&(*data)->Fold,fold);
    ajStrAss(&(*data)->Superfamily,super);
    ajStrAss(&(*data)->Family,family); 
    (*data)->Sunid = sunid;
    (*data)->num_true = true_cnt;
    

    y=0;
    /* Process truehits array */
    for(x=0;x<num_hits;x++)
    {
        if(ajIntGet((*truehits),x) == 1)
        {
            ajIntPut(rank,y,x);
            y++;
        }
        
        else
            continue;
    }



    /* tidy up */
    ajStrDel(&line);
    ajStrDel(&name);
    ajStrDel(&class);
    ajStrDel(&fold);
    ajStrDel(&super);
    ajStrDel(&second);
    ajStrDel(&family);
    ajStrDel(&str);
    ajStrDel(&code);


    /* return */
    return ajTrue;
}



/* @funcstatic sigplot_ValidatRead  *****************************************
 **
 ** Read validation file and count the number of sequences classified as
 ** seed, other and hit for the family of the signature in question and 
 ** also the number in different folds. 
 **

 ** @param [r] validatin     [AjPFile]      File pointer to signature hits file
 ** @param [r] data          [AjPScopdata*] Scopdata object pointer
 ** @param [w] vali_TN       [ajint]        ajint for number in different fold
 ** @param [w] vali_FN       [ajint]        ajint for number in family
 ** 
 ** @return [AjBool] True on succcess
 ** @@
 ******************************************************************************/
AjBool  sigplot_ValidatRead(AjPFile validatin, AjPScopdata *data, ajint *vali_TN,
                              ajint *vali_FN)
{

    ajint       tn_temp         = 0;
    ajint       fn_temp         = 0;
    
    AjPStr      line            = NULL;
    AjPStr      string          = NULL;
    AjPStr      fold            = NULL;
    


    line   = ajStrNew();
    string = ajStrNew();
    fold   = ajStrNew();
    

    /* Check args */    
    if(!validatin)
        return ajFalse;

    /* reset posinter to start of file */
    ajFileSeek(validatin, 0, 0);



    /* Determine vali_FN */
    /* i.e. count no. of hits in SAME family */
/*
    while(ajFileReadLine(validatin, &line))
    {
        if(ajStrPrefixC(line,"XX"))
            continue;
        else if(ajStrPrefixC(line,"CL"))
            continue;
        else if(ajStrPrefixC(line,"FO"))
            continue;
        else if(ajStrPrefixC(line,"SF"))
            continue;
        else if(ajStrPrefixC(line,"FA"))
            continue;
        else if(ajStrPrefixC(line,"NS"))
            continue;
        else if(ajStrPrefixC(line,"NN"))
            continue;
        else if(ajStrPrefixC(line,"AC"))
            continue;
        else if(ajStrPrefixC(line,"TY"))
            continue;
        else if(ajStrPrefixC(line,"RA"))
            continue;
        else if(ajStrPrefixC(line,"SQ"))
            continue;

        else if(ajStrPrefixC(line,"SI"))
        {*/
            /* copy SI value to si_temp */
         /*   ajFmtScanS(line, "%*s %d\n", &si_temp);
            ajFmtPrint("si_temp = %d Sunid = %d\n", si_temp, (*data)->Sunid);
            if(si_temp == (*data)->Sunid)
            {
                printf("matched SI %d = %d\n", si_temp, (*data)->Sunid);
                while(ajFileReadLine(validatin, &line) && !ajStrPrefixC(line,"//"))
                {
                    if(ajStrPrefixC(line,"NS"))
                    {
                        printf("found NS\n");*/
                        /* copy NS value to vali_TN */
                        /*ajFmtScanS(line, "%*s %d\n", &ns_temp);
                        (*vali_FN) = ns_temp;
                    }
                    else continue;
                }
            }
            else continue;
        }
    }
    */



    /* Determine vali_TN and vali_FN */
    /* i.e. count no. of hits in DIFFERENT fold */
    while(ajFileReadLine(validatin, &line))
    {
        if(ajStrPrefixC(line,"XX"))
            continue;
        else if(ajStrPrefixC(line,"CL"))
            continue;
        else if(ajStrPrefixC(line,"SF"))
            continue;
        else if(ajStrPrefixC(line,"FA"))
            continue;
        else if(ajStrPrefixC(line,"NS"))
            continue;
        else if(ajStrPrefixC(line,"NN"))
            continue;
        else if(ajStrPrefixC(line,"AC"))
            continue;
        else if(ajStrPrefixC(line,"TY"))
            continue;
        else if(ajStrPrefixC(line,"RA"))
            continue;

        else if(ajStrPrefixC(line,"SQ"))
            continue;
        else if(ajStrPrefixC(line,"SI"))
            continue;

        else if(ajStrPrefixC(line,"FO"))
        {
            ajStrAss(&fold,line);
            ajStrTrim(&fold, +5);

            if(!ajStrMatch(fold, (*data)->Fold))
            {
                while(ajFileReadLine(validatin, &line) && !ajStrPrefixC(line,"//"))
                {
                    if(ajStrPrefixC(line,"NS"))
                    {
                        ajFmtScanS(line, "%*s %d\n", &tn_temp);
                        (*vali_TN) += tn_temp;
                    }
                }
            }
            else if(ajStrMatch(fold, (*data)->Fold))
            {
                while(ajFileReadLine(validatin, &line) && !ajStrPrefixC(line,"//"))
                {
                    if(ajStrPrefixC(line,"NS"))
                    {
                        ajFmtScanS(line, "%*s %d\n", &fn_temp);
                        (*vali_FN) += fn_temp;
                    }   
                }
            }    
        }
    }
    


    ajStrDel(&line);
    ajStrDel(&string);
    ajStrDel(&fold);


    /* return */
    return ajTrue;
    
}



/* @funcstatic sigplot_HitProportion  *****************************************
 **
 ** Read signature hits file and count the number of hits, by reading the 
 ** HI line and storing the value for the rank of the hit.
 **
 ** @param [r] hitsin     [AjPFile]      File pointer to signature hits file
 ** @param [r] data       [AjPScopdata*] Scopdata object pointer
 ** @param [w] prob_array [AjPInt2d]     Probability of four classifications
 ** 
 ** @return [AjBool] True on succcess
 ** @@
 ******************************************************************************/
AjBool  sigplot_HitProportion(AjPFile hitsin, AjPScopdata *data, AjPFloat2d *prob_array,
                              AjPFloat *sensi_array, AjPFloat *speci_array, ajint vali_TN,
                              ajint vali_FN)
{

    AjPStr   line           = NULL;     /* Line of text                     */
    AjPStr   temp           = NULL;     /* temp variable for classification */


    ajint    x              = 0;
    ajint    y              = 0;
    ajint    true           = 0;        /* Counter for no. of true classifications    */
    ajint    false          = 0;        /* Counter for no. of false classifications   */
    ajint    cross          = 0;        /* Counter for no. of cross classifications   */
    ajint    unknown        = 0;        /* Counter for no. of unknown classifications */
    ajint    rank           = 0;

    ajint    FP             = 0;   /* No. of FALSE hits from hits file                      */
    ajint    TP             = 0;   /* No. of CROSS and TRUE hits from hits file             */
    ajint    TN             = 0;   /* (vali_TN - FP)                                        */
    ajint    FN             = 0;   /* (vali_FN - TP)                                        */
    float    sensi_temp     = 0;
    float    speci_temp     = 0;
    


    /* Check args */    
    if(!hitsin)
        return ajFalse;
    

    /* reset posinter to start of file */
    ajFileSeek(hitsin, 0, 0);


    /* Allocate memory for the probability array */
    *prob_array = ajFloat2dNewL((*data)->num_hits);
    *sensi_array = ajFloatNewL((*data)->num_hits);
    *speci_array = ajFloatNewL((*data)->num_hits);
    

    /* Set reserved size */
    for(x=0; x<4; x++)
        for(y=0;y<(*data)->num_hits;y++)
            ajFloat2dPut(prob_array, x, y, (ajint)0);

    /* assign arrays to zero */
    for(x=0; x<(*data)->num_hits; x++)
    {
        ajFloatPut(sensi_array, x, (ajint)0);
        ajFloatPut(speci_array, x, (ajint)0);
    }
    

    /* assign strings */
    line    = ajStrNew();
    temp    = ajStrNew();



    /* Read through the file */
    while(ajFileReadLine(hitsin, &line) && !ajStrPrefixC(line,"//"))
    {
        if(ajStrPrefixC(line,"XX"))
            continue;

        else if(ajStrPrefixC(line,"CL"))
            continue;

        else if(ajStrPrefixC(line,"FO"))
            continue;
        
        else if(ajStrPrefixC(line,"SF"))
            continue;


        else if(ajStrPrefixC(line,"FA"))
            continue;

        else if(ajStrPrefixC(line,"DE"))
            continue;

        else if(ajStrPrefixC(line,"SI"))
            continue;

        else if(ajStrPrefixC(line,"HI"))
        {
            /* perform calculation on first line of HI */
            ajFmtScanS(line, "%*s %d %*s %*s %*s %*s %*s %S", &rank, &temp);          
            
            /* If classification = true, determine the                */
            /* proportion of each classification and write into array */
            if(ajStrPrefixC(temp, "TRUE"))
            {
                true++;
                ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
            }
            
            /* If classification = false, determine the */
            /* proportion of each classification and write value into array */
            else if(ajStrPrefixC(temp, "FALSE"))
            {
                false++;
                ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
            }
            
            /* If classification = cross, determine the */
            /* proportion of each classification and write value into array */
            else if(ajStrPrefixC(temp, "CROSS"))
            {
                cross++;
                ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
            }
            
            /* If classification = unknown, determine the */
            /* proportion of each classification and write value into array */
            else if(ajStrPrefixC(temp, "UNKNOWN"))
            {
                unknown++;
                ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
            }
            /* calculate specificity and sensitivity values */
            
            /* Determine and assign true positives */
            TP = (true + cross);
            /*printf("TP = %d\n", TP);*/
            
            /* Determine and assign false positives */
            FP = false;
            /*printf("FP = %d\n", FP);*/
            
            /* Determine and assign true negatives */
            TN = (vali_TN - FP);
            /*printf("TN = %d\n", TN);*/
            
            /* Determine and assign false negatives */
            FN = (vali_FN - TP);
            /*printf("FN = %d\n", FN);*/
            

            /* Perform calculations and assign to arrays */
            /* Sensitivity */
            sensi_temp =  (float)(((float)TP)/((float)TP+(float)FN));
            /*printf("sensi = %f\n", sensi_temp);*/
            
            ajFloatPut(sensi_array, rank-1, sensi_temp); 
            
            /* Specificity */
            speci_temp =  (float)(((float)TN)/((float)TN+(float)FP));
            /*printf("speci = %f\n", speci_temp);*/
            
            ajFloatPut(speci_array, rank-1, speci_temp); 
        



            /* Start of loop for calculating      */
            /* proportion of each classification  */
            /* and sensitivity/specificity values */
            while(ajFileReadLine(hitsin, &line) && !ajStrPrefixC(line,"XX"))        
            {
                /* Scan rank and secondary classification into variables */
                ajFmtScanS(line, "%*s %d %*s %*s %*s %*s %*s %S", &rank, &temp);              


                /* If classification = true, determine the */
                /* proportion of each classification and write value into array */
                if(ajStrPrefixC(temp, "TRUE"))
                {
                    true++;
                    ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                    ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                    ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                    ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
                }

                /* If classification = false, determine the */
                /* proportion of each classification and write value into array */
                else if(ajStrPrefixC(temp, "FALSE"))
                {
                    false++;
                    ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                    ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                    ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                    ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
                }

                /* If classification = cross, determine the */
                /* proportion of each classification and write value into array */
                else if(ajStrPrefixC(temp, "CROSS"))
                {
                    cross++;
                    ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                    ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                    ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                    ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
                }       

                /* If classification = unknown, determine the */
                /* proportion of each classification and write value into array */
                else if(ajStrPrefixC(temp, "UNKNOWN"))
                {
                    unknown++;  
                    ajFloat2dPut(prob_array, 0, rank-1, (float)((float)true/(float)rank));
                    ajFloat2dPut(prob_array, 1, rank-1, (float)((float)false/(float)rank));
                    ajFloat2dPut(prob_array, 2, rank-1, (float)((float)cross/(float)rank));
                    ajFloat2dPut(prob_array, 3, rank-1, (float)((float)unknown/(float)rank));
                }
                
                /* calculate specificity and sensitivity values */

                /* Determine and assign true positives */
                TP = (true + cross);
                /*printf("rank = %4d TP = %4d\n", rank, TP);*/
                
                /* Determine and assign false positives */
                FP = false;
                /*printf("rank = %4d FP = %4d\n", rank,FP);*/
                
                /* Determine and assign true negatives */
                TN = (vali_TN - FP);
                /*printf("rank = %4d TN = %4d\n", rank,TN);*/
                
                /* Determine and assign false negatives */
                FN = (vali_FN - TP);
                /*printf("rank = %4d FN = %4d\n", rank,FN);*/

                /* Perform calculations and assign to arrays */
                /* Sensitivity */
                sensi_temp =  (float)(((float)TP)/((float)TP+(float)FN));
                /*printf("sensi = %f\n", sensi_temp);*/

                
                ajFloatPut(sensi_array, rank-1, sensi_temp); 

                /* Specificity */
                speci_temp =  (float)(((float)TN)/((float)TN+(float)FP));
                /*printf("speci = %f\n", speci_temp);*/


                ajFloatPut(speci_array, rank-1, speci_temp); 
            }
        }
    }

/*      for(x=0;x<(*data)->num_hits;x++)
            {
                ajFmtPrint("%f ", ajFloat2dGet((*prob_array), 0, x));
                ajFmtPrint("%f ", ajFloat2dGet((*prob_array), 1, x));
                ajFmtPrint("%f ", ajFloat2dGet((*prob_array), 2, x));
                ajFmtPrint("%f \n", ajFloat2dGet((*prob_array), 3, x));
            }
*/
    
    /* Tidy up */
    ajStrDel(&line);
    ajStrDel(&temp);


    /* return */

    return ajTrue;
    
}






/* @funcstatic sigplot_DataWrite  *****************************************
 **
 ** Read prob_array and Scopdata structure, and write a data file suitable
 ** for plotting using the application GNUPLOT
 **
 ** @param [w] datafile    [AjPFile]     File pointer to output datafile
 ** @param [r] data        [AjPScopdata] Scopdata object pointer
 ** @param [r] prob_array  [AjPInt2d]    Array of probability of four classifications
 ** @param [r] sensi_array [AjPInt]      Array of sensitivity values
 ** @param [r] speci_array [AjPInt]      Array of specificity values
 **
 ** @return [AjBool] True on succcess
 ** @@
 ******************************************************************************/
AjBool  sigplot_DataWrite(AjPFile datafile, AjPFile ssdatafile, AjPScopdata data, 
                          AjPFloat2d prob_array, AjPFloat sensi_array, AjPFloat speci_array)
{
    
    ajint       x          = 0;     /* Loop counter                                */
    AjPFile     truePtr    = NULL;  /* Pointer to true data file                   */
    AjPFile     crossPtr   = NULL;  /* Pointer to cross data file                  */
    AjPFile     falsePtr   = NULL;  /* Pointer to false data file                  */
    AjPFile     unknownPtr = NULL;  /* Pointer to unknown data file                */
    AjPFile     sensiPtr   = NULL;  /* Pointer to sensitivity data file            */
    AjPFile     speciPtr   = NULL;  /* Pointer to specificity data file            */
    AjPStr      true       = NULL;  /* String of output data file name             */
    AjPStr      cross      = NULL;  /* String of output data file name             */
    AjPStr      false      = NULL;  /* String of output data file name             */
    AjPStr      unknown    = NULL;  /* String of output data file name             */
    AjPStr      sensi      = NULL;  /* String of sensitivity output data file name */
    AjPStr      speci      = NULL;  /* String of specificity output data file name */

    

    
    /* initialize strings */
    true    = ajStrNew();
    cross   = ajStrNew();
    false   = ajStrNew(); 
    unknown = ajStrNew();   
    sensi   = ajStrNew();
    speci   = ajStrNew();

    

    /* Assign name to filename string               */
    /* Append 'classification'.dat to end of string */ 
    ajStrAss(&true, data->file_name);
    ajStrAppC(&true, "_true.dat");
    ajStrAss(&cross, data->file_name);
    ajStrAppC(&cross, "_cross.dat");
    ajStrAss(&false, data->file_name);
    ajStrAppC(&false, "_false.dat");
    ajStrAss(&unknown, data->file_name);
    ajStrAppC(&unknown, "_unknown.dat");

    ajStrAss(&sensi, data->file_name);
    ajStrAppC(&sensi, "_sensi.dat");
    ajStrAss(&speci, data->file_name);
    ajStrAppC(&speci, "_speci.dat");

    
    /* Assign file pointers */
    truePtr    = ajFileNewOut(true);
    crossPtr   = ajFileNewOut(cross);
    falsePtr   = ajFileNewOut(false);
    unknownPtr = ajFileNewOut(unknown);

    sensiPtr = ajFileNewOut(sensi);
    speciPtr = ajFileNewOut(speci);


    /* Print probabilities from array to data files */
    for(x = 0; x <(data)->num_hits; x++)
    {
        ajFmtPrintF(truePtr,    "%d    %4f\n", x, ajFloat2dGet(prob_array, 0, x));
        ajFmtPrintF(falsePtr,   "%d    %4f\n", x, ajFloat2dGet(prob_array, 1, x));
        ajFmtPrintF(crossPtr,   "%d    %4f\n", x, ajFloat2dGet(prob_array, 2, x));
        ajFmtPrintF(unknownPtr, "%d    %4f\n", x, ajFloat2dGet(prob_array, 3, x));
        ajFmtPrintF(sensiPtr, "%d    %4f\n", x, ajFloatGet(sensi_array, x));
        ajFmtPrintF(speciPtr, "%d    %4f\n", x, ajFloatGet(speci_array, x));
    }


    /* Write main .dat file */
    ajFmtPrintF(datafile,    "#  GNUPLOT data file suitable for plotting signature performance\n");
    ajFmtPrintF(datafile,    "#\n");
    ajFmtPrintF(datafile,    "#\n");
    ajFmtPrintF(datafile,    "#  %S\n", data->Class);
    ajFmtPrintF(datafile,    "#  XX\n");
    ajFmtPrintF(datafile,    "#  %S\n", data->Fold);
    ajFmtPrintF(datafile,    "#  XX\n");
    ajFmtPrintF(datafile,    "#  %S\n", data->Superfamily);
    ajFmtPrintF(datafile,    "#  XX\n");
    ajFmtPrintF(datafile,    "#  %S\n", data->Family);
    ajFmtPrintF(datafile,    "#  XX\n");
    ajFmtPrintF(datafile,    "#  %d\n", data->Sunid);
    ajFmtPrintF(datafile,    "#  XX\n");
    ajFmtPrintF(datafile,    "set title \"Graph of Signature Performance\"\n");
    ajFmtPrintF(datafile,    "set xlabel \"Number of Hits\"\n");
    ajFmtPrintF(datafile,    "set ylabel \"Proportion of total hits\"\n");
    ajFmtPrintF(datafile,    "set nokey\n");
    ajFmtPrintF(datafile,    "set key top outside title \"Legend\" box 3 \n");
    ajFmtPrintF(datafile,    "set data style points\n");
    ajFmtPrintF(datafile,    "set pointsize 0.45\n");
    /* Execute plot command to plot data files */
    ajFmtPrintF(datafile,    "plot \"%S\" smooth bezier title \"True Hits\", \"%S\" smooth bezier title \"Cross Hits\", \"%S\" smooth bezier title \"False Hits\", \"%S\" smooth bezier title \"Unknown Hits\"\n", true, cross, false, unknown);
 
   
    /* Write specificity and sensitivity .dat file */
    ajFmtPrintF(ssdatafile,    "#  GNUPLOT data file suitable for plotting signature sensitivity and specificity\n");
    ajFmtPrintF(ssdatafile,    "#\n");
    ajFmtPrintF(ssdatafile,    "#\n");
    ajFmtPrintF(ssdatafile,    "#  %S\n", data->Class);
    ajFmtPrintF(ssdatafile,    "#  XX\n");
    ajFmtPrintF(ssdatafile,    "#  %S\n", data->Fold);
    ajFmtPrintF(ssdatafile,    "#  XX\n");
    ajFmtPrintF(ssdatafile,    "#  %S\n", data->Superfamily);
    ajFmtPrintF(ssdatafile,    "#  XX\n");
    ajFmtPrintF(ssdatafile,    "#  %S\n", data->Family);
    ajFmtPrintF(ssdatafile,    "#  XX\n");
    ajFmtPrintF(ssdatafile,    "#  %d\n", data->Sunid);
    ajFmtPrintF(ssdatafile,    "#  XX\n");
    ajFmtPrintF(ssdatafile,    "set title \"Graph of Signature Sensitivity/specificity\"\n");
    ajFmtPrintF(ssdatafile,    "set xlabel \"Number of Hits\"\n");
    ajFmtPrintF(ssdatafile,    "set ylabel \"Proportion of total hits\"\n");
    ajFmtPrintF(ssdatafile,    "set nokey\n");
    ajFmtPrintF(ssdatafile,    "set key top outside title \"Legend\" box 3 \n");
    ajFmtPrintF(ssdatafile,    "set data style points\n");
    ajFmtPrintF(ssdatafile,    "set pointsize 0.45\n");
    /* Execute plot command to plot data files */
    ajFmtPrintF(ssdatafile,    "plot \"%S\" smooth bezier title \"Sensitivity\", \"%S\" smooth bezier title \"Specificity\"\n", sensi, speci);
 
    /* Close files */
    ajFileClose(&truePtr);
    ajFileClose(&crossPtr);
    ajFileClose(&falsePtr);
    ajFileClose(&unknownPtr);
    ajFileClose(&sensiPtr);
    ajFileClose(&speciPtr);


    /* Tidy up */
    ajStrDel(&true);
    ajStrDel(&cross);
    ajStrDel(&false);
    ajStrDel(&unknown);
    ajStrDel(&sensi);
    ajStrDel(&speci);
    

    /* Return */
    return ajTrue;
    

}







/* @func sigplot_AlignSeqExtract **********************************************
**
** @param [r] alignfile   [AjPFile]     File pointer to alignment file
** @param [r] submat      [AjPMatrixf]  Pointer to subsitiution matrix
** @param [r] gapopen     [float]       Gap opening penalty
** @param [r] gapextn     [float]       Gap extension penalty
** @param [r] matrixout   [AjPFile]     Pointer to output file for %id matrix
** 
** @return [AjBool] 
** @@
******************************************************************************/
AjBool  sigplot_AlignSeqExtract(AjPFile sigalignfile, AjPInt truehits, AjPInt2d range, 
                                AjPStr **sig_seqs, AjPStr **temp_seqs, ajint num, 
                                ajint num_true) 
{

    AjPStr              seq             = NULL;
    AjPStr              line            = NULL;
    AjPStr              check           = NULL;


    ajint               x               = 0;
    ajint               cnt             = 0;
    ajint               sigcnt          = 0;
    
    

    /* check args */
    if(!sigalignfile || !truehits || !range || !sig_seqs)
    {
        printf("Bad arguments passed to sigplot_AlignSeqExtract...... exiting\n");
        return ajFalse; 
    }    
    

    /* Assign Strings */
    seq = ajStrNew();
    line = ajStrNew();
    check = ajStrNew();

    


    /* Allocate memory and Strings */
    *temp_seqs = (AjPStr *) AJCALLOC0(num, sizeof(AjPStr));
    *sig_seqs = (AjPStr *) AJCALLOC0(num_true, sizeof(AjPStr));

    for(x=0;x<num;x++)
        (*temp_seqs)[x] = ajStrNew();


    for(x=0;x<num_true;x++)
        (*sig_seqs)[x] = ajStrNew();



    
    /* Start of main application loop */
    while(ajFileReadLine(sigalignfile,&line) && (!ajStrPrefixC(line,"//")))
    {
        /* Ignore 'Number' lines */
        if(ajStrPrefixC(line,"CL"))
            continue;
        else if(ajStrPrefixC(line,"FO"))
            continue;
        else if(ajStrPrefixC(line,"SF"))
            continue;
        else if(ajStrPrefixC(line,"FA"))
            continue;
        else if(ajStrPrefixC(line,"XX"))
            continue;
        else if(ajStrPrefixC(line,"SI"))
        {
            /* Read in XX line after SI line */
            ajFileReadLine(sigalignfile,&line);

            /* read first block of data */
            while(ajFileReadLine(sigalignfile,&line) && (!ajStrPrefixC(line,"XX")))

            {
                ajFmtScanS(line, "%S%*s%S", &check, &seq);
                if(!ajStrMatchC(check, "SIGNATURE"))
                    {
                        ajStrAssS(&(*temp_seqs)[cnt], seq);
                        cnt++;
                    }
                else
                    continue;
            }


            /* First block read, reset block line counter */
            cnt = 0;



            /* Read rest of sig/seq data lines */
            while(ajFileReadLine(sigalignfile,&line) && (!ajStrPrefixC(line,"//")))         
            {
                /* If line = XX, i.e. end of block, reset cnt */
                if(ajStrPrefixC(line,"XX"))
                    cnt = 0;

                /* Else still in block so deal with line */
                else
                {
                    ajFmtScanS(line, "%S%*s%S", &check, &seq);
                    /* Check if sig/seq data for that line is completed */
                    if(!ajStrMatchC(check, "SIGNATURE"))
                    {
                        if(!ajStrMatchC(seq, "."))
                        {
                            /*ajFmtPrint("\nprev %S\n", (*temp_seqs)[cnt]);*/
                            ajStrApp(&(*temp_seqs)[cnt], seq);
                            /*ajFmtPrint("now  %S\n\n", (*temp_seqs)[cnt]);*/
                            cnt++;
                        }
                        else
                        {
                            cnt++;
                            continue;
                        }
                        
                    }
                    
                    else
                        continue;
                }
            }
        }                   
    }
    



    /* Code section to extract range of sequence corresponding */
    /* to the sequence-signature alignment                     */
    for(x=0;x<num;x++)
    {
        /* Check if hit is a 'TRUE' hit */
        if(ajIntGet(truehits, x) == 1)
        {
/*          ajFmtPrint("\nFound true hit at pos %3d (%d) range = %3d-%3d\n", 
                       x, ajIntGet(truehits,x), ajInt2dGet(range,x, 0), ajInt2dGet(range, x, 1));
            ajFmtPrint("Sequence at rank   =  %S\n", (*temp_seqs)[x]);*/

            /* Assign sub-sequence to array */
            ajStrAssSub(&(*sig_seqs)[sigcnt], (*temp_seqs)[x],
                        (ajInt2dGet(range, x, 0)-1), (ajInt2dGet(range, x, 1)-1));
            /*ajFmtPrint("range              =  %S\n", (*sig_seqs)[sigcnt]);*/
            sigcnt++;
        }
        else
        {
            /*printf("\nNo hit found at pos %3d (%d)\n", x, ajIntGet(truehits,x)); */
            continue;
        }
    }
    
    /*for(x=0;x<num_true;x++)
          ajFmtPrint("Extracted range =  %S\n", (*sig_seqs)[x]);*/






    ajStrDel(&seq);    
    ajStrDel(&line);    
    ajStrDel(&check);    
    


    /* return */
    return ajTrue;
    


}




/* @func sigplot_SeedIdCalc***************************************************
**
** @param [r] alignfile   [AjPFile]     File pointer to alignment file
** @param [r] submat      [AjPMatrixf]  Pointer to subsitiutionmatrix
** @param [r] gapopen     [float]       Gap opening penalty
** @param [r] gapextn     [float]       Gap extension penalty
** @param [r] matrixout   [AjPFile]     Pointer to output file for %id matrix
** 
** @return [AjBool] 
** @@
******************************************************************************/
AjBool  sigplot_SeedIdCalc(AjPList seedlist,  AjPScopalg alg, AjPFile matrixout,
                           AjPMatrixf submat, float gapopen, float gapextn) 
{
    ajint       start1          = 0;    /*Start of seq 1, passed as arg but not used*/
    ajint       start2          = 0;    /*Start of seq 2, passed as arg but not used*/
    ajint       maxarr          = 300;  /*Initial size for matrix*/
    ajint       len;
    ajint       x;                      /*Counter for seq 1*/
    ajint       y;                      /*Counter for seq 2*/ 
    ajint       nin;                    /*Number of sequences in input list*/
    ajint       *compass;

    char        *p;
    char        *q;

    float     **sub;
    float       id              = 0.;   /*Passed as arg but not used here*/
    float       sim             = 0.;   
    float       idx             = 0.;   /*Passed as arg but not used here*/
    float       simx            = 0.;   /*Passed as arg but not used here*/
    float      *path;
    float       av_temp         = 0.0;

    AjPStr      m               = NULL; /*Passed as arg but not used here*/
    AjPStr      n               = NULL; /*Passed as arg but not used here*/

    AjPSeq      *inseqs         = NULL; /*Array containing input sequences*/
    AjPInt      lens            = NULL; /*1: Lengths of sequences* in input list*/
    AjPFloat2d  scores          = NULL;
    AjPSeqCvt   cvt             = 0;
    AjBool      show            = ajFalse; /*Passed as arg but not used here*/





    /*Intitialise variables*/
    AJCNEW(path, maxarr);
    AJCNEW(compass, maxarr);
    m = ajStrNew();    
    n = ajStrNew();    
    gapopen   = ajRoundF(gapopen,8);
    gapextn = ajRoundF(gapextn,8);
    sub = ajMatrixfArray(submat);
    cvt = ajMatrixfCvt(submat);

    
    /*Convert the AjPList to an array of AjPseq*/
    if(!(nin=ajListToArray(seedlist,(void ***)&inseqs)))
    {
        ajWarn("Zero sized list of sequences passed into SeqsetNR");
        AJFREE(compass);
        AJFREE(path);
        ajStrDel(&m);
        ajStrDel(&n);
        return ajFalse;
    }

                               
    /*Create an ajint array to hold lengths of sequences*/
    lens = ajIntNewL(nin);     
    for(x=0; x<nin; x++)
        ajIntPut(&lens,x,ajSeqLen(inseqs[x]));


    /*Create a 2d float array to hold the similarity scores*/
    scores = ajFloat2dNew();   

    /* Assign array values to zero */
    for(x=0;x<nin;x++)
        for(y=0;y<nin;y++)
            ajFloat2dPut(&scores, x, y, 0.0);



    /*Start of main application loop*/
    for(x=0; x<nin; x++)       
    { 
        for(y=x+1; y<nin; y++) 
        {
            /*Process w/o alignment identical sequences*/       
            if(ajStrMatch(inseqs[x]->Seq, inseqs[y]->Seq))
            {
                ajFloat2dPut(&scores,x,y,(float)100.0);
                ajFloat2dPut(&scores,y,x,(float)100.0);
                continue;
            } 

                                       

            /* Intitialise variables for use by alignment functions*/       
            len = ajIntGet(lens,x)*ajIntGet(lens,y);

            if(len>maxarr)
            {
                AJCRESIZE(path,len);
                AJCRESIZE(compass,len);
                maxarr=len;
            }
                                       
            p = ajSeqChar(inseqs[x]); 
            q = ajSeqChar(inseqs[y]); 

            ajStrAssC(&m,"");
            ajStrAssC(&n,"");


            /* Check that no sequence length is 0 */
            if((ajIntGet(lens,x)==0)||(ajIntGet(lens,y)==0))
            {
                ajWarn("Zero length sequence in SeqsetNR");
                AJFREE(compass);
                AJFREE(path);
                ajStrDel(&m);
                ajStrDel(&n);
                ajFloat2dDel(&scores);
                ajIntDel(&lens);
                AJFREE(inseqs);

                                           

                return ajFalse;
            }


            /* Call alignment functions */
            embAlignPathCalc(p,q,ajIntGet(lens,x),ajIntGet(lens,y), gapopen,
                             gapextn,path,sub,cvt,compass,show);

            embAlignScoreNWMatrix(path,inseqs[x],inseqs[y],sub,cvt,
                                  ajIntGet(lens,x), ajIntGet(lens,y),gapopen,
                                  compass,gapextn,&start1,&start2);

            embAlignWalkNWMatrix(path,inseqs[x],inseqs[y],&m,&n,
                                 ajIntGet(lens,x),ajIntGet(lens,y),
                                 &start1,&start2,gapopen,gapextn,cvt,
                                 compass,sub);

            embAlignCalcSimilarity(m,n,sub,cvt,ajIntGet(lens,x),
                                   ajIntGet(lens,y),&id,&sim,&idx, &simx);
            
            
            /* Write array with score*/
            ajFloat2dPut(&scores,x,y,sim);
            ajFloat2dPut(&scores,y,x,sim);
            /*ajFmtPrint("%8.3f%8.3f%8.3f%8.3f\n", id, idx, sim, simx);*/
        }
    }

    /* Write output file */
    ajFmtPrintF(matrixout, "CL   %S\n", alg->Class);

    ajFmtPrintF(matrixout, "XX\n");
    ajFmtPrintF(matrixout, "FO   %S\n", alg->Fold);

    ajFmtPrintF(matrixout, "XX\n");
    ajFmtPrintF(matrixout, "SF   %S\n", alg->Superfamily);
    ajFmtPrintF(matrixout, "XX\n");
    ajFmtPrintF(matrixout, "FA   %S\n", alg->Family);
    ajFmtPrintF(matrixout, "XX\n");
    ajFmtPrintF(matrixout, "SI   %d\n", alg->Sunid_Family);
    ajFmtPrintF(matrixout, "XX\n");
    ajFmtPrintF(matrixout, "------------\n");
    ajFmtPrintF(matrixout, "Seed Id Data\n");
    ajFmtPrintF(matrixout, "------------\n");
    

    ajFmtPrintF(matrixout, "      ");

    for(x=0;x<nin;x++)
        ajFmtPrintF(matrixout, "%8S", alg->Codes[x]);

    ajFmtPrintF(matrixout, "\n");
    
    for(x=0;x<nin;x++)
    {
        ajFmtPrintF(matrixout, "%7S", alg->Codes[x]);
        for(y=0;y<nin;y++)
        {
            if(y == 0)
                ajFmtPrintF(matrixout,"%6.2f", ajFloat2dGet(scores, x, y));
            else
                ajFmtPrintF(matrixout,"%8.2f", ajFloat2dGet(scores, x, y));
        }
        ajFmtPrintF(matrixout, "\n");
    }

    /* Write line to file */
    for(x=0;x<(6+(nin * 8));x++)
        ajFmtPrintF(matrixout, "-");

    ajFmtPrintF(matrixout, "\n");


    /* Calculate individual average */
    for(x=0;x<nin;x++)
    {
        av_temp = 0.0;
        for(y=0;y<nin;y++)
        {
            av_temp += ajFloat2dGet(scores, x, y); 
        }
        if(x == 0)
        {
            ajFmtPrintF(matrixout, "av.id");
            ajFmtPrintF(matrixout, "%8.2f", (av_temp/(float)(nin - 1)));            
        }
        else
            ajFmtPrintF(matrixout, "%8.2f", (av_temp/(float)(nin - 1))); 
    }    

    ajFmtPrintF(matrixout, "\n");

    /* Write line to file */    
    for(x=0;x<(6+(nin * 8));x++)
        ajFmtPrintF(matrixout, "-");

    ajFmtPrintF(matrixout, "\n");

    
    av_temp = 0.0;
    
    /* Calculate overall average */
    for(x=0;x<nin;x++)
        for(y=0;y<nin;y++)
            av_temp += ajFloat2dGet(scores, x, y); 

    ajFmtPrintF(matrixout, "overall average id = %.2f\n", (av_temp/(float)((nin * nin) - nin)));            

  

                               
    /* Tidy up */
    AJFREE(compass);
    AJFREE(path);
    ajStrDel(&m);
    ajStrDel(&n);
    ajFloat2dDel(&scores);
    ajIntDel(&lens);
    AJFREE(inseqs);
    
                               
    /* Bye Bye */
    return ajTrue;
}    




/* @func sigplot_HitIdCalc***************************************************
**
** @param [r] alignfile   [AjPFile]     File pointer to alignment file
** @param [r] submat      [AjPMatrixf]  Pointer to subsitiutionmatrix
** @param [r] gapopen     [float]       Gap opening penalty
** @param [r] gapextn     [float]       Gap extension penalty
** @param [r] matrixout   [AjPFile]     Pointer to output file for %id matrix
** 
** @return [AjBool] 
** @@
******************************************************************************/
AjBool  sigplot_HitIdCalc(AjPList seedlist, AjPList hitlist, AjPScopalg alg, 
                          AjPFile matrixout, AjPMatrixf submat, float gapopen, 
                          float gapextn, AjPStr **codes, AjPInt rank) 

{
    ajint       start1          = 0;    /*Start of seq 1, passed as arg but not used*/
    ajint       start2          = 0;    /*Start of seq 2, passed as arg but not used*/
    ajint       maxarr          = 300;  /*Initial size for matrix*/
    ajint       len;
    ajint       x;                      /*Counter for seq 1*/
    ajint       y;                      /*Counter for seq 2*/ 
    ajint       nin;                    /*Number of sequences in input list*/
    ajint       hitn;
    ajint       *compass;
    ajint       num             = 0;
    

    char        *p;
    char        *q;

    float     **sub;
    float       id              = 0.;   /*Passed as arg but not used here*/
    float       sim             = 0.;   
    float       idx             = 0.;   /*Passed as arg but not used here*/
    float       simx            = 0.;   /*Passed as arg but not used here*/
    float      *path;
    float       av_temp         = 0.0;

    AjPStr      m               = NULL; /*Passed as arg but not used here*/
    AjPStr      n               = NULL; /*Passed as arg but not used here*/

    AjPSeq      *seedseqs       = NULL; /*Array containing input sequences*/
    AjPSeq      *hitseqs        = NULL; /*Array containing input sequences*/
    AjPInt      seedlens        = NULL; /*1: Lengths of sequences* in input list*/
    AjPInt      hitlens         = NULL; /*1: Lengths of sequences* in input list*/
    AjPFloat2d  scores          = NULL;
    AjPSeqCvt   cvt             = 0;
    AjBool      show            = ajFalse; /*Passed as arg but not used here*/




    /*Intitialise variables*/
    AJCNEW(path, maxarr);
    AJCNEW(compass, maxarr);
    m = ajStrNew();    

    n = ajStrNew();    
    gapopen   = ajRoundF(gapopen,8);
    gapextn = ajRoundF(gapextn,8);
    sub = ajMatrixfArray(submat);
    cvt = ajMatrixfCvt(submat);

    
    /*Convert the AjPList to an array of AjPseq*/
    if(!(nin=ajListToArray(seedlist,(void ***)&seedseqs)))
    {
        ajWarn("Zero sized list of sequences passed into SeqsetNR");
        AJFREE(compass);
        AJFREE(path);
        ajStrDel(&m);
        ajStrDel(&n);
        return ajFalse;
    }


    /*Convert the AjPList to an array of AjPseq*/
    if(!(hitn=ajListToArray(hitlist,(void ***)&hitseqs)))
    {
        ajWarn("Zero sized list of sequences passed into SeqsetNR");

        AJFREE(compass);
        AJFREE(path);
        ajStrDel(&m);
        ajStrDel(&n);
        return ajFalse;
    }

                               
    /*Create an ajint array to hold lengths of sequences*/
    seedlens = ajIntNewL(nin);     
    for(x=0; x<nin; x++)
        ajIntPut(&seedlens,x,ajSeqLen(seedseqs[x]));

    /*Create an ajint array to hold lengths of sequences*/
    hitlens = ajIntNewL(hitn);     
    for(x=0; x<hitn; x++)
        ajIntPut(&hitlens,x,ajSeqLen(hitseqs[x]));


    /*Create a 2d float array to hold the similarity scores*/
    scores = ajFloat2dNew();   

    /* Assign array values to zero */
    for(x=0;x<hitn;x++)
        for(y=0;y<nin;y++)
            ajFloat2dPut(&scores, x, y, 0.0);

    


    /*Start of main application loop*/
    for(x=0; x<hitn; x++)       
    { 
        num++;
        for(y=0; y<nin; y++) 
        {
            /* Process identical sequences */       
            if(ajStrMatch(hitseqs[x]->Seq, seedseqs[y]->Seq))
            {
                ajFloat2dPut(&scores,x,y,(float)100.0);
                /*ajFloat2dPut(&scores,y,x,(float)100.0);*/
                continue;
            } 

            /* Intitialise variables for use by alignment functions*/       
            len = ajIntGet(hitlens,x)*ajIntGet(seedlens,y);

            if(len>maxarr)
            {
                AJCRESIZE(path,len);
                AJCRESIZE(compass,len);
                maxarr=len;
            }
                                       
            p = ajSeqChar(hitseqs[x]); 
            q = ajSeqChar(seedseqs[y]); 

            ajStrAssC(&m,"");
            ajStrAssC(&n,"");


            /* Check that no sequence length is 0 */
            if((ajIntGet(hitlens,x)==0)||(ajIntGet(seedlens,y)==0))
            {
                ajWarn("Zero length sequence in SeqsetNR hit");
                AJFREE(compass);
                AJFREE(path);
                ajStrDel(&m);
                ajStrDel(&n);
                ajFloat2dDel(&scores);

                ajIntDel(&seedlens);
                ajIntDel(&hitlens);
                AJFREE(seedseqs);
                AJFREE(hitseqs);
                                           
                return ajFalse;
            }


            /* Call alignment functions */
            embAlignPathCalc(p,q,ajIntGet(hitlens,x),ajIntGet(seedlens,y), gapopen,
                             gapextn,path,sub,cvt,compass,show);

            embAlignScoreNWMatrix(path,hitseqs[x],seedseqs[y],sub,cvt,
                                  ajIntGet(hitlens,x), ajIntGet(seedlens,y),gapopen,
                                  compass,gapextn,&start1,&start2);

            embAlignWalkNWMatrix(path,hitseqs[x],seedseqs[y],&m,&n,
                                 ajIntGet(hitlens,x),ajIntGet(seedlens,y),
                                 &start1,&start2,gapopen,gapextn,cvt,
                                 compass,sub);

            embAlignCalcSimilarity(m,n,sub,cvt,ajIntGet(hitlens,x),
                                   ajIntGet(seedlens,y),&id,&sim,&idx, &simx);
            
            
            /* Write score to array */
            ajFloat2dPut(&scores,x,y,sim);
            /*ajFmtPrint("%8.3f%8.3f%8.3f%8.3f\n", id, idx, sim, simx);*/
        }
    }


    /* Write output file */

    ajFmtPrintF(matrixout, "//\n-------------------------\n");
    ajFmtPrintF(matrixout, "True Hit and Seed Id Data\n");
    ajFmtPrintF(matrixout, "-------------------------\n");
    ajFmtPrintF(matrixout, "            ");
    for(x=0;x<nin;x++)
        ajFmtPrintF(matrixout, "%8S", alg->Codes[x]);
    ajFmtPrintF(matrixout, "  av. id\n");
    

    for(x=0;x<hitn;x++)
    {
        av_temp = 0.0;
        
        ajFmtPrintF(matrixout, "%S%5d", (*codes)[x], (ajIntGet(rank,x)+1));
        for(y=0;y<nin;y++)
        {
            ajFmtPrintF(matrixout,"%8.2f", ajFloat2dGet(scores, x, y));
            av_temp += ajFloat2dGet(scores, x, y); 
        }
        
        /* Print average id of hit to all seeds to output file */
        ajFmtPrintF(matrixout, "%8.2f\n", (av_temp) / (float)nin);
    }

    ajFmtPrintF(matrixout, "//");
    
                               
    /* Tidy up */
    AJFREE(compass);
    AJFREE(path);
    ajStrDel(&m);
    ajStrDel(&n);
    ajFloat2dDel(&scores);
    ajIntDel(&seedlens);
    ajIntDel(&hitlens);


    AJFREE(seedseqs);
    AJFREE(hitseqs);

                               
    /* Bye Bye */
    return ajTrue;
}    




/* @func sigplot_ScopdataNew ***********************************************************
**
** Scopdata object constructor.
**
** 
** @return [AjPScopdata] Pointer to a Scopdata object
** @@
******************************************************************************/
AjPScopdata  sigplot_ScopdataNew()
{

    AjPScopdata ret = NULL;
    

    AJNEW0(ret);
    ret->Class=ajStrNew();
    ret->Fold=ajStrNew();
    ret->Superfamily=ajStrNew();
    ret->Family=ajStrNew();
    ret->file_name=ajStrNew();
    ret->Sunid = 0;
    ret->num_true = 0;

    return ret;
}


/* @func sigplot_ScopdataDel *******************************************************
**
** Destructor for Scopdata object.
**
** @param [w] pthis [AjPScopdata*] Scopdata object pointer
**
** @return [void]
** @@
******************************************************************************/
void sigplot_ScopdataDel(AjPScopdata *pthis)
{

    ajStrDel(&(*pthis)->Class);
    ajStrDel(&(*pthis)->Fold);
    ajStrDel(&(*pthis)->Superfamily);
    ajStrDel(&(*pthis)->Family);
    ajStrDel(&(*pthis)->file_name);

    
    AJFREE(*pthis);
    *pthis=NULL;
    
    /* return */
    return;
}



 















