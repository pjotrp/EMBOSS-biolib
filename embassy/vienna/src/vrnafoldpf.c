/* Last changed Time-stamp: <2005-06-22 15:57:54 ivo> */
/*                
		  Ineractive Access to folding Routines

		  c Ivo L Hofacker
		  Vienna RNA package
*/

#include "emboss.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "fold.h"
#include "part_func.h"
#include "fold_vars.h"
#include "ePS_dot.h"
#include "utils.h"
extern void  read_parameter_file(AjPFile file);
extern float circfold(const char *string, char *structure);

extern AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);



static char *cwarn[] = 
{
    "# Depending on the origin of the circular sequence, some structures may",
    "# be missed when using -noLP. Try rotating your sequence a few times.",
    "#",
    NULL
};



/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char   *string;
    char   *structure=NULL;
    char   *cstruc=NULL;

    char   *ns_bases=NULL;
    char   *c;
    int    i;
    int    length;
    int    l;
    int    sym;

    double energy;
    double min_en;
    double kT;
    double sfact=1.07;
    int    pf=0;
    int    istty;
    int    noconv=0;
    int    circ=0;

    AjPSeq  seq     = NULL;
    AjPFile confile = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;
    AjPFile essfile = NULL;
    AjPFile dotfile = NULL;
    

    AjPStr seqstring = NULL;
    AjPStr constring = NULL;
    AjPStr seqname   = NULL;
  
    float eT = 0.;
    AjBool eGU;
    AjBool ecirc = ajFalse;
  
    AjBool eclose;
    AjBool lonely;
    AjBool convert;
    AjPStr ensbases = NULL;
    AjBool etloop;
    AjPStr *eenergy = NULL;
    char ewt = '\0';
    float escale = 0.;
    AjPStr *edangles = NULL;
    char edangle = '\0';

    ajint len;



    embInitP("vrnafoldpf",argc,argv,"VIENNA");
    
    
    seqstring = ajStrNew();
    constring = ajStrNew();
    seqname   = ajStrNew();
    
    
    seq       = ajAcdGetSeq("sequence");
    confile   = ajAcdGetInfile("constraintfile");
    paramfile = ajAcdGetInfile("paramfile");
    eT        = ajAcdGetFloat("temperature");
    ecirc     = ajAcdGetBool("circular");
    eGU       = ajAcdGetBool("gu");
    eclose    = ajAcdGetBool("closegu");
    lonely    = ajAcdGetBool("lp");
    convert   = ajAcdGetBool("convert");
    ensbases  = ajAcdGetString("nsbases");
    etloop    = ajAcdGetBool("tetraloop");
    eenergy   = ajAcdGetList("energy");
    escale    = ajAcdGetFloat("scale");
    edangles  = ajAcdGetList("dangles");
    outf      = ajAcdGetOutfile("outfile");
    essfile   = ajAcdGetOutfile("ssoutfile");
    dotfile   = ajAcdGetOutfile("dotoutfile");

    do_backtrack = 1; 
    pf = 1;
    string = NULL;
    istty = 0;

    temperature   = (double) eT;
    circ          = !!ecirc;
    noGU          = (eGU) ? 0 : 1;
    no_closingGU  = (eclose) ? 0 : 1;
    noLonelyPairs = (lonely) ? 0 : 1;
    noconv        = (convert) ? 0 : 1;
    ns_bases      = (ajStrGetLen(ensbases)) ? MAJSTRGETPTR(ensbases) : NULL;
    tetra_loop    = !!etloop;
    
    ewt = *ajStrGetPtr(*eenergy);
    if(ewt == '0')
	energy_set = 0;
    else if(ewt == '1')
	energy_set = 1;
    else if(ewt == '2')
	energy_set = 2;
    
    sfact = (double) escale;
    
    edangle = *ajStrGetPtr(*edangles);
    if(edangle == '0')
	dangles = 0;
    else if(edangle == '1')
	dangles = 1;
    else if(edangle == '2')
	dangles = 2;
    else if(edangle == '3')
	dangles = 3;


    if(circ&& noLonelyPairs)
    {
	i = 0;
	while(cwarn[i])
	{
	    ajFmtPrintF(outf,"%s\n",cwarn[i]);
	    ++i;
	}
    }


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


    if(confile)
	vienna_GetConstraints(confile,&constring);
    




    string = NULL;
    structure = NULL;

    length = ajSeqGetLen(seq);
    string = (char *) space(length+1);
    strcpy(string,ajSeqGetSeqC(seq));

    len = ajStrGetLen(constring);
    structure = (char *) space(length+1);
    if(len)
    {
	fold_constrained = 1;
	strcpy(structure,ajStrGetPtr(constring));
    }
    

    for (l = 0; l < length; l++)
    {
	string[l] = toupper(string[l]);
	if (!noconv && string[l] == 'T')
	    string[l] = 'U';
    }
    
    /* initialize_fold(length); */
    if (circ) 
	min_en = circfold(string, structure);
    else
	min_en = fold(string, structure);

    ajFmtPrintF(outf,"%s\n%s", string, structure);
    
    if (istty)
	printf("\n minimum free energy = %6.2f kcal/mol\n", min_en);
    else
	ajFmtPrintF(outf," (%6.2f)\n", min_en);
    

    if (length<2000)
	(void) PS_rna_plot(string, structure, essfile);
    else
    { 
	struct bond  *bp;
	ajWarn("Structure too long, not doing xy_plot\n");

	/* free mfe arrays but preserve base_pair for PS_dot_plot */
	bp = base_pair; base_pair = space(16);
	free_arrays();			/* free's base_pair */
	base_pair = bp;
    } 
    
    if (pf)
    {
	if (circ)
	    ajFatal("Currently no partition function for circular RNAs.\n");
	if (dangles==1)
	{
	    dangles=2;	  /* recompute with dangles as in pf_fold() */
	    min_en = energy_of_struct(string, structure);
	    dangles=1;
	}
	 
	kT = (temperature+273.15)*1.98717/1000.; /* in Kcal */
	pf_scale = exp(-(sfact*min_en)/kT/length);
	if (length>2000)
	    ajFmtPrintF(outf, "scaling factor %f\n", pf_scale);

	init_pf_fold(length);

	if (cstruc!=NULL)
	    strncpy(structure, cstruc, length+1);
	energy = pf_fold(string, structure);
	
	if (do_backtrack)
	{
	    ajFmtPrintF(outf,"%s", structure);
	    ajFmtPrintF(outf," [%6.2f]\n", energy);
	}

	if ((istty)||(!do_backtrack)) 
	    printf(" free energy of ensemble = %6.2f kcal/mol\n", energy);



	ajFmtPrintF(outf," frequency of mfe structure in ensemble %g\n"
		   " ensemble diversity %-6.2f\n", exp((energy-min_en)/kT),
		   mean_bp_dist(length));

	if (do_backtrack)
	{
	    (void) PS_dot_plot(string, dotfile);
	}
	free_pf_arrays();

    }

    if (cstruc!=NULL)
	free(cstruc);

    if (length>=2000)
	free(base_pair);

    free(string);
    free(structure); 


    ajStrDel(&seqstring);
    ajStrDel(&constring);
    ajStrDel(&seqname);
    ajSeqDel(&seq);

    ajFileClose(&outf);
    ajFileClose(&essfile);
    ajFileClose(&dotfile);
    
    ajExit();

    return 0;
}
