/* Last changed Time-stamp: <2008-03-31 17:06:17 ivo> */
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

static char UNUSED rcsid[] = "$Id: vrnasubopt.c,v 1.8 2008/06/26 08:40:00 rice Exp $";
#define PRIVATE static

extern double print_energy;
#define MAXDOS 1000
extern int    density_of_states[MAXDOS+1];
/*PRIVATE char *tokenize(char *line);*/
/*PRIVATE void usage(void);*/

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
    int noconv=0;
    int circ=0;
    int dos=0;
    
    AjPSeq  seq     = NULL;
    AjPFile confile = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;
    

    AjPStr constring = NULL;
  
    float eT = 0.;
    AjBool eGU;
  
    AjBool eclose;
    AjBool lonely;
    AjBool convert;
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
    circ          = !!ajAcdGetBoolean("circular");
    dos           = !!ajAcdGetBoolean("dos");
    eGU           = ajAcdGetBoolean("gu");
    eclose        = ajAcdGetBoolean("closegu");
    lonely        = ajAcdGetBoolean("lp");
    convert       = ajAcdGetBoolean("convert");
    ensbases      = ajAcdGetString("nsbases");
    etloop        = ajAcdGetBoolean("tetraloop");
    erange        = ajAcdGetFloat("erange");
    prange        = ajAcdGetFloat("prange");
    subopt_sorted = !!ajAcdGetBoolean("sort");
    logML         = !!ajAcdGetBoolean("logml");
    n_back        = ajAcdGetInt("nrandom");
   
    edangles      = ajAcdGetList("dangles");
    outf      = ajAcdGetOutfile("outfile");

    if(dos)
        print_energy = -999999;

    do_backtrack = 1;
   
    istty = 0;

    temperature   = (double) eT;
    noGU          = (eGU) ? 0 : 1;
    no_closingGU  = (eclose) ? 0 : 1;
    noLonelyPairs = (lonely) ? 0 : 1;
    noconv        = (convert) ? 0 : 1;
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
    

    if(n_back)
        init_rand();

    
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
      
    for (l = 0; l < length; l++)
    {
        sequence[l] = toupper(sequence[l]);
        if (!noconv && sequence[l] == 'T')
            sequence[l] = 'U';
    }
    
    if ((logML!=0 || dangles==1 || dangles==3) && dos==0)
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
	mfe = (circ) ? circfold(sequence, ss) : fold(sequence, ss);
	kT = (temperature+273.15)*1.98717/1000.; /* in Kcal */
	pf_scale = exp(-(1.03*mfe)/kT/length);
	strncpy(ss, structure, length);
        /*
        ** we are not interested in the free energy but in the bppm, so we
        ** drop free energy into the void
        */
        (circ) ? (void) pf_circ_fold(sequence, ss) :
            (void) pf_fold(sequence, ss);
	free(ss);
	for (i=0; i<n_back; i++)
	{
	    char *s;
            s = (circ) ? pbacktrack_circ(sequence) : pbacktrack(sequence);
	    ajFmtPrintF(outf,"%s\n", s);
	    free(s);
	}
	free_pf_arrays();
    }
    else
    {
	(circ) ? subopt_circ(sequence, structure, delta, ajFileGetFileptr(outf)) :
            subopt(sequence, structure, delta, ajFileGetFileptr(outf));
    }
      

    free(sequence);
    free(structure); 

    ajFileClose(&outf);

    embExit();

    return 0;
}
