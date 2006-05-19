/* Last changed Time-stamp: <2004-08-12 12:45:17 ivo> */
/*                
		Ineractive access to suboptimal folding

			   c Ivo L Hofacker
			  Vienna RNA package
*/
#include "emboss.h"
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "part_func.h"
#include "fold.h"
#include "fold_vars.h"
#include "utils.h"
#include "subopt.h"
extern void  read_parameter_file(AjPFile fname);
extern int   st_back;

#define PRIVATE static

extern double print_energy;

extern char *pbacktrack(char *sequence);

extern AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);

/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char *sequence;
    char *structure = NULL;
    char  *ns_bases = NULL, *c;
    int   i, length, l, sym;
    int   istty;
    double deltap=0.;
    int delta=100;
    int n_back = 0;

    AjPSeq  seq     = NULL;
    AjPFile confile = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;
    

    AjPStr constring = NULL;
  
    float eT = 0.;
    AjBool eGU;
  
    AjBool eclose;
    AjBool lonely;
    AjPStr ensbases = NULL;
    AjBool etloop;
    AjPStr *edangles = NULL;
    char edangle = '\0';

    ajint len;
    float erange;
    float prange;
   

    embInitP("vrnasubopt",argc,argv,"VIENNA");
    
    
    constring = ajStrNew();
    
    seq           = ajAcdGetSeq("sequence");
    confile       = ajAcdGetInfile("constraintfile");
    paramfile     = ajAcdGetInfile("paramfile");
    eT            = ajAcdGetFloat("temperature");
    eGU           = ajAcdGetBool("gu");
    eclose        = ajAcdGetBool("closegu");
    lonely        = ajAcdGetBool("lp");
    ensbases      = ajAcdGetString("nsbases");
    etloop        = ajAcdGetBool("tetraloop");
    erange        = ajAcdGetFloat("erange");
    prange        = ajAcdGetFloat("prange");
    subopt_sorted = !!ajAcdGetBool("sort");
    logML         = !!ajAcdGetBool("logml");
    n_back        = ajAcdGetInt("nrandom");
   
    edangles      = ajAcdGetList("dangles");
    outf      = ajAcdGetOutfile("outfile");


    do_backtrack = 1;
   
    istty = 0;

    temperature   = (double) eT;
    noGU          = (eGU) ? 0 : 1;
    no_closingGU  = (eclose) ? 0 : 1;
    noLonelyPairs = (lonely) ? 0 : 1;
    ns_bases      = (ajStrGetLen(ensbases)) ? MAJSTRGETPTR(ensbases) : NULL;
    tetra_loop    = !!etloop;

    delta = (int) (0.1 + erange * 100);
    deltap = prange;
    
    edangle = *ajStrGetPtr(*edangles);
    if(edangle == '0')
	dangles = 0;
    else if(edangle == '1')
	dangles = 1;
    else if(edangle == '2')
	dangles = 2;
    else if(edangle == '3')
	dangles = 3;


    if(paramfile)
	read_parameter_file(paramfile);


   
    if (ns_bases != NULL)
    {
	nonstandards = space(33);
	c=ns_bases;
	i=sym=0;
	if (*c=='-')
	{
	    sym=1;
	    c++;
	}
	while (*c)
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


    if(confile)
	vienna_GetConstraints(confile,&constring);
    




    sequence  = NULL;
    structure = NULL;

    length = ajSeqGetLen(seq);
    sequence = (char *) space(length+1);
    strcpy(sequence,ajSeqGetSeqC(seq));

    len = ajStrGetLen(constring);
    structure = (char *) space(length+1);
    if(len)
    {
	fold_constrained = 1;
	strcpy(structure,ajStrGetPtr(constring));
    }
    
    istty = 0;

    if (fold_constrained)
    {
	for (i=0; i<length; i++)
	    if (structure[i]=='|')
		ajFatal("Constraints of type '|' are not allowed\n");
    }      
      
    for (l = 0; l < length; l++) sequence[l] = toupper(sequence[l]);

    if (logML!=0 || dangles==1 || dangles==3) 
	if (deltap<=0) deltap=delta/100. +0.001;
    if (deltap>0)
	print_energy = deltap;

    /* first lines of output (suitable  for sort +1n) */

    ajFmtPrintF(outf,"> %s [%d]\n", ajSeqGetNameC(seq), delta);

    if(n_back>0)
    {
	int i;
	double mfe, kT;
	char *ss;
	st_back=1;
	ss = (char *) space(strlen(sequence)+1);
	strncpy(ss, structure, length);
	mfe = fold(sequence, ss);
	kT = (temperature+273.15)*1.98717/1000.; /* in Kcal */
	pf_scale = exp(-(1.03*mfe)/kT/length);
	strncpy(ss, structure, length);
	(void) pf_fold(sequence, ss);
	init_rand();
	free(ss);
	for (i=0; i<n_back; i++)
	{
	    char *s;
	    s = pbacktrack(sequence);
	    ajFmtPrintF(outf,"%s\n", s);
	    free(s);
	}
	free_pf_arrays();
    }
    else
    {
	subopt(sequence, structure, delta, ajFileFp(outf));
    }
      

    free(sequence);
    free(structure); 

    ajFileClose(&outf);

    ajExit();

    return 0;
}
