/*
		      Heat Capacity of RNA molecule

		    c Ivo Hofacker and Peter Stadler
			  Vienna RNA package


	    calculates specific heat using C = - T d^2/dT^2 G(T)
*/

#include "emboss.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include "utils.h"
#include "fold_vars.h"
#include "fold.h"
#include "part_func.h"
extern void  read_parameter_file(AjPFile fname);

#define PRIVATE      static
#define PUBLIC
#define MAXWIDTH     201

PRIVATE float F[MAXWIDTH];
PRIVATE float ddiff(float f[], float h, int m);

#define GASCONST 1.98717  /* in [cal/K] */
#define K0 273.15




PRIVATE void heat_capacity(char *string, float T_min, float T_max,
			   float h, int m, AjPFile outf)
{
    int length, i;
    char *structure;
    float hc, kT, min_en;
   
    length = (int) strlen(string);
   
    do_backtrack = 0;   

    temperature = T_min -m*h;
    initialize_fold(length);
    structure = (char *) space((unsigned) length+1);
    min_en = fold(string, structure);
    free(structure); free_arrays();
    kT = (temperature+K0)*GASCONST/1000; /* in kcal */
    pf_scale = exp(-(1.07*min_en)/kT/length );
    init_pf_fold(length);
   
    for (i=0; i<2*m+1; i++)
    {
	F[i] = pf_fold(string, NULL);   /* T_min -2h */
	temperature += h;
	kT = (temperature+K0)*GASCONST/1000;
	pf_scale=exp(-(F[i]/length +h*0.00727)/kT); /* try to extrapolate F */
	update_pf_params(length); 
    }

    while (temperature <= (T_max+m*h+h))
    {
      
	hc = - ddiff(F,h,m)* (temperature +K0 - m*h -h); 
	ajFmtPrintF(outf,"%-5.1f   %g\n", (temperature-m*h-h), hc);  
      
	for (i=0; i<2*m; i++)
	    F[i] = F[i+1];
	F[2*m] = pf_fold(string, NULL); 
	temperature += h;
	kT = (temperature+K0)*GASCONST/1000;
	pf_scale=exp(-(F[i]/length +h*0.00727)/kT);
	update_pf_params(length); 
    }
    free_pf_arrays();

    return;
}




/* ------------------------------------------------------------------------- */

PRIVATE float ddiff(float f[], float h, int m)
{
    float fp;
    int i;
    float A;
    float B;

    A = (float)(m*(m+1)*(2*m+1)/3 );	/* 2*sum(x^2) */
    B = (float)(m*(m+1)*(2*m+1) ) * (float)(3*m*m+3*m-1) /15.; /* 2*sum(x^4) */
   
    fp=0.;
    for (i=0; i<2*m+1; i++)
	fp += f[i]*( A - (float) ( (2*m+1)*(i-m)*(i-m)) );
   
    fp /= ( ( A*A - B*( (float)(2*m+1) ) )*h*h/2. );

    return (float)fp;
}




int main(int argc, char *argv[])
{
    char *string;
    char  *ns_bases=NULL;
    char  *c;
    int  i;
    int  length;
    int  l;
    int  sym;
    float T_min;
    float T_max;
    float h;
    int mpoints;
    int istty;
    int noconv = 0;


    AjPSeq  seq     = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;
  
    AjBool eGU;
    AjBool eclose;
    AjBool lonely;
    AjBool convert;
    AjPStr ensbases = NULL;
    AjBool etloop;
    AjPStr eenergy = NULL;
    char ewt = '\0';
    AjPStr edangles = NULL;
    char edangle = '\0';


    embInitP("vrnaheat",argc,argv,"VIENNA");
    
    
    seq       = ajAcdGetSeq("sequence");
    paramfile = ajAcdGetInfile("paramfile");
    T_min     = ajAcdGetFloat("mintemp");
    T_max     = ajAcdGetFloat("maxtemp");
    eGU       = ajAcdGetBoolean("gu");
    eclose    = ajAcdGetBoolean("closegu");
    lonely    = ajAcdGetBoolean("lp");
    convert   = ajAcdGetBoolean("convert");
    ensbases  = ajAcdGetString("nsbases");
    etloop    = ajAcdGetBoolean("tetraloop");
    eenergy   = ajAcdGetListSingle("energy");
    edangles  = ajAcdGetListSingle("dangles");
    h         = ajAcdGetFloat("step");
    mpoints   = ajAcdGetInt("smoothing");
   
    outf      = ajAcdGetOutfile("outfile");


    do_backtrack = 0; 
    string = NULL;
    istty = 0;

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
    
    edangle = *ajStrGetPtr(edangles);
    if(edangle == '0')
	dangles = 0;
    else if(edangle == '2')
	dangles = 2;


    if(paramfile)
	read_parameter_file(paramfile);
   
    if (ns_bases!=NULL)
    {
	nonstandards = space(33);
	c=ns_bases;
	i=sym=0;
	if (*c=='-')
	{
	    sym=1;
	    c++;
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
   

    length = ajSeqGetLen(seq);
    string = (char *) space(length+1);   
    strcpy(string,ajSeqGetSeqC(seq));
   
       
    for (l = 0; l < length; l++)
    {
        string[l] = toupper(string[l]);
        if (!noconv && string[l] == 'T')
	    string[l] = 'U';
    }

      
    heat_capacity(string, T_min, T_max, h, mpoints, outf);
    free(string);

    ajSeqDel(&seq);
    ajStrDel(&ensbases);
    ajStrDel(&eenergy);
    ajStrDel(&edangles);

    ajFileClose(&paramfile);
    ajFileClose(&outf);

    embExit();

    return 0;
}
