/* @Source rocon application
**
** Reads a DHF file (domain hits file) of hits (sequences of unknown structural
** classification) and a domain families file (validation sequences of known 
** classification) and writes a "hits file" for the hits, which are classified 
** and rank-ordered on the basis of score.
**
** @author: Copyright (C) Jon Ison (jison@ebi.ac.uk)
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
** 
**  ROCON Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@ebi.ac.uk.
**  
**  NOTES
**  
******************************************************************************/

#include "emboss.h"





/* @prog rocon ****************************************************************
**
** Reads a DHF file (domain hits file) of hits (sequences of unknown structural
** classification) and a domain families file (validation sequences of known 
** classification) and writes a "hits file" for the hits, which are classified 
** and rank-ordered on the basis of score.
**
****************************************************************************/
int main(int argc, char **argv)
{
    /* Variable declarations */
    AjPFile      hitsin      = NULL;  /* (ACD) DHF file (hits input).       */
    EmbPHitlist   hitsin_l    = NULL;  /* For contents of hitsin.            */
    AjPList      hitsin_tmp  = NULL;  /* For managing hitsin_l              */
    ajint        nhitsin     = 0;     /* No. of hits in hitsin.             */
    

    AjPFile      validin     = NULL;  /* (ACD) DHF file (validation input). */
    AjPList      validin_l   = NULL;  /* For contents of validin.           */
    EmbPHitlist   validin_tmp = NULL; /* For managing validin_l.            */ 

    AjPFile      hitsout     = NULL;  /* (ACD) Hits file (output).          */
    AjPStr      *mode        = NULL;  /* (ACD) Mode of operation            */
    ajint        moden       = 0;     /* mode as an integer.
					      1: Family classification scheme.
					      2: (Not yet available).       */
    ajint        thresh      = 0;     /* (ACD) threshold for hit overlap.   */


    /* The total number of known true hits in the validation file for the 
       family given in <hitsin>. This is the maximum number of TRUE tokens 
       that could ever appear in the hits file (output).                    */
    ajint        nrelated    = 0;     

    /*  The maximum permissible ROC value that could be calculated for the  
	hits file (output). This is equal to the number of FALSE tokens that 
	occur in the hits file.                                             */
    ajint        maxroc      = 0;
    
    ajint        x           = 0;     /* Housekeeping                       */
    


    
    /* ACD processing. */
    embInitP("rocon",argc,argv,"SIGNATURE");

    hitsin   = ajAcdGetInfile("hitsinfile");
    validin  = ajAcdGetInfile("validinfile");
    thresh   = ajAcdGetInt("thresh");
    mode     = ajAcdGetList("mode");  
    hitsout  = ajAcdGetOutfile("hitsoutfile");


    
    /* Memory allocation & housekeeping. */
    if(!(ajStrToInt(mode[0], &moden)))
	ajFatal("Could not parse ACD node option");
    if(moden!=1)
	ajFatal("Unrecognised mode");
    validin_l   = ajListNew();
    hitsin_tmp  = ajListNew();




    /* Read input files. */
    hitsin_l  = embHitlistReadFasta(hitsin);

    while((validin_tmp = embHitlistReadFasta(validin)))
    {
	ajListPushAppend(validin_l, (void *) validin_tmp);

	if(moden==1)
	    if(ajStrMatchS(validin_tmp->Family, hitsin_l->Family)           &&
	       ajStrMatchS(validin_tmp->Superfamily, hitsin_l->Superfamily) &&
	       ajStrMatchS(validin_tmp->Fold, hitsin_l->Fold)               &&
	       ajStrMatchS(validin_tmp->Class, hitsin_l->Class))
		nrelated += validin_tmp->N;
    }

    /* Classify hits & sort results for output. */
    if(moden==1)
    {
	if(!(embHitlistClassify(hitsin_l, validin_l, thresh)))
	    ajFatal("embHitlistClassify failed\n");
    }



    for(x=0; x<hitsin_l->N; x++)
	ajListPushAppend(hitsin_tmp, (void *) hitsin_l->hits[x]);
    ajListSort(hitsin_tmp, embMatchinvScore);
    AJFREE(hitsin_l->hits);
    hitsin_l->hits = NULL;
    nhitsin = ajListToarray(hitsin_tmp, (void ***) &hitsin_l->hits);
    
    for(x=0; x<nhitsin; x++)
	if(ajStrMatchC(hitsin_l->hits[x]->Typeobj, "FALSE"))
	    maxroc++;
    


    /* Write output file. */
    ajFmtPrintF(hitsout, "> RELATED %d ; ROC %d\n", nrelated, maxroc);
    for(x=0; x<nhitsin; x++)
	ajFmtPrintF(hitsout, "%-13S%-10S%-6d%-6d\n", 
		    hitsin_l->hits[x]->Typeobj, 
		    hitsin_l->hits[x]->Acc, 
		    hitsin_l->hits[x]->Start, 
		    hitsin_l->hits[x]->End);
    


    /* Free memory & exit cleanly. */
    ajFileClose(&hitsin);
    embHitlistDel(&hitsin_l);
    ajListFree(&hitsin_tmp);

    ajFileClose(&validin);
    while(ajListPop(validin_l, (void **) &validin_tmp))
	embHitlistDel(&validin_tmp);
    ajListFree(&validin_l);

    ajFileClose(&hitsout);

    ajStrDel(&mode[0]);
    AJFREE(mode);

    ajExit();
    return 0;
}
