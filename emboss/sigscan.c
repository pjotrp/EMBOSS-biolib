/* @source sigscan application
**
** Scans a signature against swissprot and writes a signature hits files.
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
** sigscan scans a signature such as that generated by the EMBOSS application 
** siggen against a protein sequence database and generates files of scored 
** hits and corresponding alignments.  Optionally, sigscan can use a file of 
** residue pair substitution frequencies as generated by the EMBOSS application
** pairsub duringa signature:sequence match.  An (optionally grouped) scop 
** families file can be provided in which case a classification of hits is 
** provided in the signature hits output file. See documentation for the EMBOSS
** application psiblasts for an explanation of the scop families file and groups 
** for information on how to group it.
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
** (6)  SI - SCOP Sunid's of the family. This number uniquely identifies the 
** family in the SCOP parsable files.
** (7) HI - hit data.  The data are as follows (column numbers are given in 
** parentheses). (i) HI is always given. (ii) Rank-order of the hit. (iii) 
** Database identifier code. (iv) The group number of the protein if a grouped 
** scop families file was provided or '.' otherwise. (v) The primary (objective)
** classification of the hit. For true hits (genuine relatives to the signature
** as listed in the scop families file)
** one of 'SEED', 'HIT' or 'OTHER') is given. Otherwise, 'CROSS', 'FALSE'
** or 'UNKNOWN' is given ('.' is given if a scop families file was not 
** provided). (vi) The secondary classification of the hit, either 'TRUE', 
** 'CROSS', 'FALSE' or 'UNKNOWN' ('.' is given if a scop families file was not 
** provided). The classes of hits are defined below.  
** (vii) Score of sequence-signature match.  (viii) E-value (see below).
** (8) XX - used for spacing.
** (9) // - The file ends with a line containing '//' only.
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
** SI   14982
** XX
** HI   1    1RBPDFG   1    TRUE     TRUE    234  0.0001 
** HI   2    1GFT35J   3    TRUE     TRUE    234  0.0008 
** HI   3    1KJUFGH   1    TRUE     TRUE    224  0.0108 
** HI   4    1GYU15R   2    CLOSE    TRUE    220  0.1876 
** HI   5    1LKI89O   2    CLOSE    TRUE    203  0.6787 
** HI   6    1QRTY58   1    TRUE     TRUE    199  0.9978 
** HI   7    2IOM78G   1    FALSE    FALSE   198  1.0844
** HI   8    1SZR234   1    CLOSE    TRUE    198  1.4343 
** HI   9    3PONI57   1    DISTANT  FALSE   197  2.8849 
** HI  10    1PHDJBS   3    CLOSE    TRUE    190  2.9872
** HI  11    1HIOHDW   1    UNKNOWN  UNKNOWN 160  5,8676 
** HI  12    199976T   1    CLOSE    TRUE    140  8.8346 
** XX
** //
** 
**
** Excerpts from an alignment file are shown (Figure 2). The records used 
** are are as follows:
** (1) The DE, CL, FO, SF, FA, SI, XX and // records have the same meaning as 
** in the hits file (above).
** (2) Other lines contain either a fragment of protein sequence preceeded
** by an accession number, or a fragment of an alignment of a signature
** to the protein sequence (signature positions are marked with a '*').
** The two numbers on either side of the sequence are begin and end residue
** numbers for that line.
** 
** 
** Figure 2   Excerpt from a signature alignment file
**
** DE   Results of signature search
** XX
** CL   Alpha and beta proteins (a/b)
** XX
** FO   alpha/beta-Hydrolases
** XX
** SF   alpha/beta-Hydrolases
** XX
** FA   Acetylcholinesterase-like
** XX
** SI   14982
** XX
** OPSD_HUMAN      1        MNGTEGPNFYVPFSNATGVVRSPFEYPQYYLAEPWQFSMLAAYMF 45      
** SIGNATURE       -        ---------*------------*---------------*------   
** OPSD_XENLA      1        MNGTEGPNFYVPMSNKTGVVRSPFDYPQYYLAEPWQYSALAAYMF 45      
** SIGNATURE       -        --------*-------------*----------------*-----   
** XX
** OPSD_HUMAN      46       LLIVLGFPINFLTLYVTVQHKKLRTPLNYILLNLAVADLFMVLGG 90
** SIGNATURE       -        --------------*--------------*------------*--         
** OPSD_XENLA      46       LLILLGLPINFMTLFVTIQHKKLRTPLNYILLNLVFANHFMVLCG 90      
** SIGNATURE       -        --------------*--------------*------------*--         
** XX
** OPSD_HUMAN      91       FTSTLYTSLHGYFVFGPTGCNLEGFFATLGGEIALWSLVVLAIER 135     
** SIGNATURE       -        ---------*--*--------------------------**----         
** OPSD_XENLA      91       FTVTMYTSMHGYFIFGPTGCYIEGFFATLGGEVALWSLVVLAVER 135     
** SIGNATURE       -        ---------*----*-------------------------**---         
** XX1
** //
**
** 
** Definition of classes of hit 
** The primary classification is an objective definition of the hit and has 
** one of the following values:
** SEED - the sequence was included in the original alignment from which the 
** signature was generated.
** HIT - A protein which was detected by psiblast  (see psiblasts.c) to 
** be a homologue to at least one of the proteins in the family from which 
** the signature was derived. Such proteins are identified by the 'HIT' 
** record in the scop families file.
** OTHER - A true member of the family but not a homologue as detected by 
** psi-blast. Such proteins may have been found from the literature and 
** manually added to the scop families file or may have been detected by the 
** EMBOSS program swissparse (see swissparse.c). They are identified in the 
** SCOP families file by the 'OTHER' record.
** CROSS - A protein which is homologous to a protein of the same fold,
** but differnt family, of the proteins from which the signature was
** derived.
** FALSE - A homologue to a protein with a different fold to the family
** of the signature.
** UNKNOWN - The protein is not known to be CROSS, FALSE or a true hit (a 
** SEED, HIT or OTHER).
** The secondary classification is provided for convenience and a value as 
** follows:
** Hits of SEED, HIT and OTHER classification are all listed as TRUE.
** Hits of CROSS, FALSE or UNKNOWN objective classification are listed as CROSS, 
** FALSE or UNKNOWN respectively.
** 
** The subjective column allows for hand-annotation of the hits files so that 
** proteins of UNKNOWN objective classification can re-classified by a human 
** expert as TRUE, FALSE, CROSS or otherwise left as UNKNOWN for the purpose of 
** generating signature performance plots with the EMBOSS application sigplot.
**
** Notes
** Important - sigscan presumes that SCOP family names are unique. If 
** this were not the case, changes to ajXyzClassifyHits would have to
** be made.
** Important
** In the case a signature file is generated by hand, it is essential that the 
** gap data given is listed in order of increasing gap size.  
** 
** 
Add the following fragment if pair scoring is implemented:

bool: usepairsub
[
	param: Y
	info: "Use residue pair substitution data for scoring"
	def: Y
]

infile: pairsub
[ 
	param: Y
	info: "Name of residue pair substitution matrix for input"
	def: "pair.dat"
]

Residue pair substitution data file
AjPFile      pairsub =NULL;      
pairsub    = ajAcdGetInfile("pairsub");
ajFileClose(&pairsub);

Flag to control use of residue pair substitution data file
AjBool       usepairsub=ajTrue;  
usepairsub = ajAcdGetBool("usepairsub");

******************************************************************************/




#include "emboss.h"
#include <math.h>




















/* @prog sigscan *************************************************************
**
** Signature-sequence alignment
**
******************************************************************************/


int main(int argc, char **argv)
{
    AjPFile      sigin =NULL;        /*Signature input file*/
    AjPFile      targetf =NULL;      /*SCOP families file*/
    AjPFile      hitsf =NULL;        /*Hits output file*/
    AjPFile      alignf =NULL;       /*Alignment output file*/

    AjPSeqall    database=NULL;      /*Protein sequences to match signature 
				       against*/
    AjPMatrixf   sub  =NULL;         /*Residue substitution matrix*/
    float        gapo =0.0;          /*Gap insertion penalty*/
    float        gape =0.0;          /*Gap extension penalty*/

    ajint        nhits=0;            /*No. of hits to write to output files*/
    AjPHitlist   hits=NULL;          /*Hitlist to stores hits from 
				       signature-sequence matches */
    ajint        thresh=0;           /*Minimum length (residues) of overlap 
				       required for two hits with the same 
				       code to be counted as the same hit. */
    

    AjPSignature sig=NULL;           /*Signature data structure*/

    AjPHitlist   targets =NULL;	     /*Pointer to AjOHitlist structure */    
    AjPList      listtargets = NULL; /*List of AjOHitlist structures */    
    AjPStr       *nterm=NULL;        /*Holds N-terminal matching options from acd*/
    ajint         ntopt=0;           /*N-terminal option as int */
    



    embInit("sigscan", argc, argv);
    

    /* GET VALUES FROM ACD */
    sigin      = ajAcdGetInfile("sigin");
    database   = ajAcdGetSeqall("database");
    targetf   = ajAcdGetInfile("targetf");   
    sub        = ajAcdGetMatrixf("sub");
    gapo       = ajAcdGetFloat("gapo");
    gape       = ajAcdGetFloat("gape");
    nhits      = ajAcdGetInt("nhits");
    thresh     = ajAcdGetInt("thresh");
    hitsf      = ajAcdGetOutfile("hitsf");
    alignf     = ajAcdGetOutfile("alignf"); 
    nterm      = ajAcdGetList("nterm");


    /*Assign N-terminal matching option */
    ajFmtScanS(nterm[0], "%d", &ntopt);
    
    
    /* READ SIGNATURE FILE */
    if(!ajXyzSignatureRead(sigin, &sig))
    {	
	ajMatrixfDel(&sub);
	ajFileClose(&sigin);
	ajFileClose(&targetf);
	ajFileClose(&hitsf);
	ajFileClose(&alignf);
	ajFatal("Error reading signature file");
    }   
    else
	ajFmtPrint("Signature file read ok\n");

    
    
    /* COMPILE SIGNATURE */
    if(!ajXyzSignatureCompile(&sig, gapo, gape, sub))
    {	
	ajXyzSignatureDel(&sig);
	ajMatrixfDel(&sub);
	ajFileClose(&sigin);
	ajFileClose(&targetf);
	ajFileClose(&hitsf);
	ajFileClose(&alignf);
	ajFatal("Error compiling signature");
    }  
    else
	ajFmtPrint("Signature compiled ok\n");


    /* READ SCOP FAMILIES FILE & FILL THE LIST OF TARGETS*/
    listtargets = ajListNew();
    while(ajXyzHitlistRead(targetf,"//",&targets))
	ajListPush(listtargets, (AjPHitlist) targets);
    ajFmtPrint("Targets read ok\n");

    

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
    hits = ajXyzHitlistNew(0);
    ajXyzSignatureAlignSeqall(sig, database, nhits, &hits, ntopt);
    ajFmtPrint("Signature aligned to db ok\n");


    /* CLASSIFY THE HITS ACCORDING TO THE SCOP FAMILIES FILE */
    ajXyzHitlistClassify(&hits, listtargets, thresh);
    ajFmtPrint("Hit classified ok\n");


    
    /*WRITE HITS OUTPUT FILE */
    ajXyzSignatureHitsWrite(hitsf, sig, hits);
    ajFmtPrint("Hits file written ok\n");


    /*WRITE HITS OUTPUT FILE */
  
    ajXyzSignatureAlignWrite(alignf, sig, hits);
    ajFmtPrint("Alignments file written ok\n");


    /* TIDY UP AND EXIT */
    ajXyzHitlistDel(&hits);
    while(ajListPop(listtargets, (void *) &targets))
	ajXyzHitlistDel(&targets);
    ajListDel(&listtargets);
    ajXyzSignatureDel(&sig);
    ajSeqallDel(&database);
    ajMatrixfDel(&sub);
    ajFileClose(&sigin);
    ajFileClose(&targetf);
    ajFileClose(&hitsf);
    ajFileClose(&alignf);

    ajExit();
    return 0;    
}
































