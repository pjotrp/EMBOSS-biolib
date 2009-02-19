/* Last changed Time-stamp: <2003-04-23 11:56:44 ivo> */
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
#include "utils.h"

extern float Lfold(char *string, char *structure, int maxdist,
		   AjPFile outf);
extern void  read_parameter_file(AjPFile file);


extern AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);



/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char *string;
    char *structure=NULL;

    char  *ns_bases=NULL;
    char  *c;
    int   i;
    int   length;
    int   l;
    int   sym;

    double min_en;
    double sfact=1.07;
    int pf=0;
    int istty;
    int noconv=0;
    int maxdist=150;
   

    AjPSeq seq = NULL;

    AjPFile outf = NULL;
    AjPFile paramfile = NULL;
    AjPFile confile = NULL;


    float eT = 0.;
    AjBool eGU;
    AjBool eclose;
    AjBool lonely;
    AjBool convert;
    AjPStr ensbases = NULL;
    AjBool etloop;
    AjPStr eenergy = NULL;
    char ewt = '\0';
    float escale = 0.;
    AjPStr edangles = NULL;
    char edangle = '\0';

    AjPStr seqstring = NULL;
    AjPStr constring = NULL;
    AjPStr seqname = NULL;
    ajint len;

    

    embInitP("vrnalfold",argc,argv,"VIENNA");


    do_backtrack = 1; 
    pf = 0;
    
    string    = NULL;
    structure = NULL;
    istty = 0;
    

    seqstring = ajStrNew();
    constring = ajStrNew();
    seqname   = ajStrNew();
  

    seq       = ajAcdGetSeq("sequence");
    confile   = ajAcdGetInfile("constraintfile");
    paramfile = ajAcdGetInfile("paramfile");
    maxdist   = ajAcdGetInt("separation");
    eT        = ajAcdGetFloat("temperature");
    eGU       = ajAcdGetBoolean("gu");
    eclose    = ajAcdGetBoolean("closegu");
    lonely    = ajAcdGetBoolean("lp");
    convert   = ajAcdGetBoolean("convert");
    ensbases  = ajAcdGetString("nsbases");
    etloop    = ajAcdGetBoolean("tetraloop");
    eenergy   = ajAcdGetListSingle("energy");
    escale    = ajAcdGetFloat("scale");
    edangles  = ajAcdGetListSingle("dangles");
    outf      = ajAcdGetOutfile("outfile");

    temperature   = (double) eT;
    noGU          = (eGU) ? 0 : 1;
    no_closingGU  = (eclose) ? 0 : 1;
    noLonelyPairs = (lonely) ? 0 : 1;
    noconv        = (convert) ? 0 : 1;
    ns_bases      = (ajStrGetLen(ensbases)) ? MAJSTRGETPTR(ensbases) : NULL;
    tetra_loop    = !!etloop;

    ewt = *ajStrGetPtr(eenergy);
    if(ewt == '0')
	energy_set = 0;
    else if(ewt == '1')
	energy_set = 1;
    else if(ewt == '2')
	energy_set = 2;
  
    sfact = (double) escale;
  
    edangle = *ajStrGetPtr(edangles);
    if(edangle == '0')
	dangles = 0;
    else if(edangle == '1')
	dangles = 1;
    else if(edangle == '2')
	dangles = 2;
    else if(edangle == '3')
	dangles = 3;



    if(paramfile != NULL)
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
    

    structure = NULL;
    string    = NULL;

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
	if (!noconv && string[l] == 'T') string[l] = 'U';
    }

    
    /* initialize_fold(length); */
    update_fold_params();
    min_en = Lfold(string, structure, maxdist, outf);
    ajFmtPrintF(outf, "%s\n%s", string, structure);

    if (istty)
	printf("\n minimum free energy = %6.2f kcal/mol\n", min_en);
    else
	ajFmtPrintF(outf," (%6.2f)\n", min_en);
    

    free(string);
    free(structure); 

    ajStrDel(&seqstring);
    ajStrDel(&constring);
    ajStrDel(&seqname);

    ajSeqDel(&seq);
    ajStrDel(&ensbases);
    ajStrDel(&eenergy);
    ajStrDel(&edangles);

    ajFileClose(&confile);
    ajFileClose(&paramfile);
    ajFileClose(&outf);

    embExit();

    return 0;
}
