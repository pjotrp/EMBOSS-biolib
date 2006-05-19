/* Last changed Time-stamp: <2005-07-23 16:50:24 ivo> */
/*                
	     Compute duplex structure of two RNA strands

			   c Ivo L Hofacker
			  Vienna RNA package
*/

#include "emboss.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "duplex.h"
#include "fold.h"
#include "fold_vars.h"
#include "utils.h"

extern void  read_parameter_file(AjPFile fname);
extern int subopt_sorted;
static void  print_struc(duplexT const *dup, AjPFile outf);


/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char *s1;
    char *s2;
    char *ns_bases=NULL;
    char *c;
    int  i;
    int  l;
    int  sym;
    int  pf=0;
    int  istty;
    int  delta=-1;
    int  noconv=0;
   
    AjPSeq  seq1 = NULL;
    AjPSeq  seq2 = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;

    float edelta;
  
    float eT = 0.;
    AjBool eGU;
  
    AjBool eclose;
    AjBool lonely;
    AjBool convert;
    AjBool dosort;
  
    AjPStr ensbases = NULL;
    AjBool etloop;
    AjPStr *edangles = NULL;
    char edangle = '\0';

    ajint s1len;
    ajint s2len;
    
    duplexT mfe;
    duplexT *subopt;

    embInitP("vrnaduplex",argc,argv,"VIENNA");
    
    seq1      = ajAcdGetSeq("asequence");
    seq2      = ajAcdGetSeq("bsequence");
    paramfile = ajAcdGetInfile("paramfile");
    eT        = ajAcdGetFloat("temperature");
    eGU       = ajAcdGetBool("gu");
    eclose    = ajAcdGetBool("closegu");
    lonely    = ajAcdGetBool("lp");
    convert   = ajAcdGetBool("convert");
    ensbases  = ajAcdGetString("nsbases");
    etloop    = ajAcdGetBool("tetraloop");
    dosort    = ajAcdGetBool("sort");
    edelta    = ajAcdGetFloat("delta");
  
    edangles  = ajAcdGetList("dangles");
    outf      = ajAcdGetOutfile("outfile");

    pf = 0;
    istty = 0;

    temperature   = (double) eT;
    noGU          = (eGU) ? 0 : 1;
    no_closingGU  = (eclose) ? 0 : 1;
    noLonelyPairs = (lonely) ? 0 : 1;
    noconv        = (convert) ? 0 : 1;
    ns_bases      = (ajStrGetLen(ensbases)) ? MAJSTRGETPTR(ensbases) : NULL;
    tetra_loop    = !!etloop;
    
    edangle = *ajStrGetPtr(*edangles);
    if(edangle == '0')
	dangles = 0;
    else if(edangle == '1')
	dangles = 1;
    else if(edangle == '2')
	dangles = 2;
    else if(edangle == '3')
	dangles = 3;

    if(edelta < 0.)
	delta = -1;
    else
	delta = (int) (0.1 + (edelta * 100.));

    subopt_sorted = !!dosort;
  

    if(paramfile)
	read_parameter_file(paramfile);
   
    if (ns_bases != NULL)
    {
	nonstandards = space(33);
	c=ns_bases;
	i=sym=0;
	if (*c=='-')
	{
	    sym=1; c++;
	}
	while (*c!='\0')
	{
	    if (*c!=',')
	    {
		nonstandards[i++]=*c++;
		nonstandards[i++]=*c;
		if ((sym)&&(*c!=*(c-1)))
		{
		    nonstandards[i++]=*c;
		    nonstandards[i++]=*(c-1);
		}
	    }
	    c++;
	}
    }


    s1len = ajSeqGetLen(seq1);
    s2len = ajSeqGetLen(seq2);

    s1 = (char *) space(s1len+1);
    s2 = (char *) space(s2len+1);

    strcpy(s1,ajSeqGetSeqC(seq1));
    strcpy(s2,ajSeqGetSeqC(seq2));
    
    for (l = 0; l < s1len; l++)
    {
	s1[l] = toupper(s1[l]);
	if (!noconv && s1[l] == 'T')
	    s1[l] = 'U';
    }
    
    for (l = 0; l < s2len; l++)
    {
	s2[l] = toupper(s2[l]);
	if (!noconv && s2[l] == 'T') s2[l] = 'U';
    }
    
    
    /* initialize_fold(length); */
    update_fold_params();
    if (delta>=0)
    {
	duplexT *sub;
	subopt = duplex_subopt(s1, s2, delta, 0);
	for (sub=subopt; sub->i >0; sub++)
	{
	    print_struc(sub,outf);
	    free(sub->structure);
	}
	free(subopt);
    }
    else
    {
	mfe = duplexfold(s1, s2);
	print_struc(&mfe,outf);
	free(mfe.structure);
    }
    
    free(s1);
    free(s2);



    ajExit();

    return 0;
}




static void print_struc(duplexT const *dup, AjPFile outf)
{
    int l1;

    l1 = strchr(dup->structure, '&')-dup->structure;
    ajFmtPrintF(outf,"%s %3d,%-3d : %3d,%-3d (%5.2f)\n",
		dup->structure, dup->i+1-l1, dup->i, dup->j,
		dup->j+strlen(dup->structure)-l1-2, dup->energy);

    return;
}
