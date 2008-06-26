/*
	    Interactive access to inverse folding routines
		    c Ivo Hofacker, Peter Stadler
			  Vienna RNA Package
*/
/* Last changed Time-stamp: <2000-09-28 14:58:05 ivo> */

#include "emboss.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "inverse.h"
#include "fold_vars.h"
#include "fold.h"
#include "part_func.h"
#include "utils.h"
#ifdef dmalloc
#include  "/usr/local/include/dmalloc.h"
#define space(X) calloc(1,(X))
#endif

#define  PUBLIC
#define  PRIVATE   static

#define  REPEAT_DEFAULT  100
#define  INFINITY        100000

extern void  read_parameter_file(AjPFile fname);

extern int inv_verbose;

int main(int argc, char *argv[])
{
    char *start;
    char *structure;
    char *rstart;
    char *str2;
    char *line;
    int i;
    int length;
    int l;
    int hd;
    double energy = 0.;
    double kT;
    int   pf = 0;
    int   mfe = 0;
    int   istty;
    int   repeat; 
    int   found;
    
    AjPFile inf     = NULL;
    AjPSeq  seq = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;
    
    float eT = 0.;
    AjBool eGU;
    
    AjBool eclose;
    AjBool lonely;
    AjBool etloop;
    AjPStr *eenergy = NULL;
    char ewt = '\0';
    AjPStr *edangles = NULL;
    AjPStr *method   = NULL;
    AjPStr ealpha    = NULL;
    AjBool showfails = ajFalse;
    AjBool succeed = ajFalse;
    
    char edangle = '\0';
    
    ajint len;
    FILE *fp;
    
    
    
    embInitP("vrnainverse",argc,argv,"VIENNA");
    
    
    inf        = ajAcdGetInfile("structuresfile");
    seq        = ajAcdGetSeq("sequence");
    paramfile  = ajAcdGetInfile("paramfile");
    eT         = ajAcdGetFloat("temperature");
    eGU        = ajAcdGetBoolean("gu");
    eclose     = ajAcdGetBoolean("closegu");
    lonely     = ajAcdGetBoolean("lp");
    etloop     = ajAcdGetBoolean("tetraloop");
    eenergy    = ajAcdGetList("energy");
    edangles   = ajAcdGetList("dangles");
    method     = ajAcdGetList("folding");
    ealpha     = ajAcdGetString("alphabet");
    final_cost = ajAcdGetFloat("final");
    repeat     = ajAcdGetInt("repeats");
    showfails  = ajAcdGetBoolean("showfails");
    succeed    = ajAcdGetBoolean("succeed");
    outf       = ajAcdGetOutfile("outfile");
    
    
    do_backtrack = 0; 
    structure = NULL;
    istty = 0;
    
    temperature   = (double) eT;
    noGU          = (eGU) ? 0 : 1;
    no_closingGU  = (eclose) ? 0 : 1;
    noLonelyPairs = (lonely) ? 0 : 1;
    tetra_loop    = !!etloop;
    
    ewt = *ajStrGetPtr(*eenergy);
    if(ewt == '0')
	energy_set = 0;
    else if(ewt == '1')
	energy_set = 1;
    else if(ewt == '2')
	energy_set = 2;
    
    edangle = *ajStrGetPtr(*edangles);
    if(edangle == '0')
	dangles = 0;
    else if(edangle == '1')
	dangles = 1;
    else if(edangle == '2')
	dangles = 2;
    else if(edangle == '3')
	dangles = 3;
    
    if(ajStrMatchC(method[0],"mp"))
    {
	mfe = 1;
	pf  = 1;
    }
    else if(ajStrMatchC(method[0],"m"))
    {
	mfe = 1;
	pf  = 0;
    }
    else if(ajStrMatchC(method[0],"p"))
    {
	mfe = 0;
	pf  = 1;
    }
    
    len = ajStrGetLen(ealpha);
    symbolset = (char *) space(len + 1);
    strcpy(symbolset, ajStrGetPtr(ealpha));
    for (l = 0; l < len; l++)
	symbolset[l] = toupper(symbolset[l]);
    
    inv_verbose = !!showfails;
    fp = ajFileGetFileptr(inf);
    
    init_rand();
    kT = (temperature+273.15)*1.98717/1000.0;
    
    istty = (isatty(fileno(stdout))&&isatty(fileno(stdin)));
    
    if (paramfile)
	read_parameter_file(paramfile);
    
    give_up = succeed;
    
    do {
      
	if ((line = get_line(fp))==NULL) break;

	/* read structure, skipping over comment lines */
	while ((*line=='*')||(*line=='\0')||(*line=='>'))
	{
	    free(line);
	    if ((line = get_line(fp))==NULL)
		break;
	} 
	/* stop at eof or '@' */
	if (line==NULL) break;
	if (strcmp(line, "@") == 0)
	{
	    free(line);
	    break;
	}

	structure = (char *) space(strlen(line)+1);
	/* scanf gets rid of trailing junk */
	(void) sscanf(line,"%s",structure);
	free(line);
      
	length = (int) strlen(structure);
	str2 = (char *) space((unsigned)length+1);

/* now look for a sequence to match the structure */

/*
	if ((line = get_line(fp))!=NULL)
	    if (strcmp(line, "@") == 0)
	    {
		free(line);
		break;
	    }
*/

	start = (char *) space((unsigned) length+1);
	if(seq)
	    (void) strncpy(start, ajSeqGetSeqC(seq), length);

	if (repeat!=0)
	    found = repeat;
	else
	    found = 1;
      
	initialize_fold(length);

	rstart = (char *) space((unsigned)length+1);
	while(found>0)
	{
	    char *string;
	    string = (char *) space((unsigned)length+1);
	    strcpy(string, start);
	    for (i=0; i<length; i++)
	    {
		/* lower case characters are kept fixed, any other character
		   not in symbolset is replaced by a random character */
		if (islower(string[i]))
		    continue;

		if (string[i]=='\0' || (strchr(symbolset,string[i])==NULL))
		    string[i]=symbolset[int_urn(0,strlen(symbolset)-1)];
	    }
	    strcpy(rstart, string);	/* remember start string */
	
	    if (mfe)
	    {
		energy = inverse_fold(string, structure);
		if( (!succeed) || (energy<=0.0) ) {
		    found--;
		    hd = hamming(rstart, string);
		    ajFmtPrintF(outf,"%s  %3d", string, hd);
		    if (energy>0)
		    {			/* no solution found */
			ajFmtPrintF(outf,"   d = %f\n", energy);
		    }
		    else
			ajFmtPrintF(outf,"\n");
		}
	    }

	    if (pf)
	    {
		if (!(mfe && give_up && (energy>0)))
		{
		    /* unless we gave up in the mfe part */
		    double prob, min_en, sfact=1.07;
	    
		    /* get a reasonable pf_scale */
		    min_en = fold(string,str2); 
		    pf_scale = exp(-(sfact*min_en)/kT/length);
		    init_pf_fold(length);
	    
		    energy = inverse_pf_fold(string, structure);
		    prob = exp(-energy/kT);
		    hd = hamming(rstart, string);
		    ajFmtPrintF(outf,"%s  %3d  (%f)\n", string, hd, prob);
		    free_pf_arrays();
		}
		if (!mfe)
		    found--;
	    }

	    free(string);
	}
	free(rstart);
	free_arrays();
      
	free(structure);
	free(str2);
	free(start);

    } while (1);
    
    embExit();
    return 0;
}
