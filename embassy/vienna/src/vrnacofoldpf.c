/* Last changed Time-stamp: <2005-02-16 14:58:00 ivo> */

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
#include "ePS_dot.h"
#include "cofold.h"
#include "fold.h"
#include "eco_part_func.h"
#include "part_func.h"
#include "fold_vars.h"
#include "utils.h"

extern void  read_parameter_file(AjPFile file);
extern AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);

#define PRIVATE static

#define SAME_STRAND(I,J) (((I)>=cut_point)||((J)<cut_point))
PRIVATE char *costring(char *string);
PRIVATE char *tokenize(char *line);

PRIVATE double do_partfunc(char *string, int length, int Switch,
			   struct plist **tpr, struct plist **mf);

PRIVATE void free_franz(char *Astring, char *Bstring, plist *prAB,
			plist *prAA, plist *prBB, plist *prA, plist *prB,
			struct plist *mfAB, struct plist *mfAA,
			struct plist *mfBB, struct plist *mfA,
			struct plist *mfB);

PRIVATE int read_concentrationfile(AjPFile concfile, double *startc);

PRIVATE struct ConcEnt *do_the_concenratinger(AjPFile Conc_file, double FEAB,
					      double FEAA, double FEBB,
					      double FEA, double FEB,
					      double *startconces,
					      AjPFile outf);

/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char *string;
    char *structure = NULL;
    char *cstruc = NULL;
    char  fname[53];
    char  *ns_bases = NULL;
    char  *c;
    char  *Concfile;

    int   i;
    int   length;
    int   l;
    int   sym;

    double energy;
    double min_en;
    double kT;
    double sfact=1.07;

    int pf=0;
    int istty;
    int noconv=0;
    int doT=0;			 /*compute dimere free energies etc.*/
    int doC=0;			 /*toggle to compute concentrations*/ 
    int doQ=0;	       /*toggle to compute prob of base being paired*/
    int cofi=0;			/*toggle concentrations stdin / file*/

    double FEAB;			/*free energy  of AB dimer*/
    double FEAA;			/*free energy  of AA dimer*/
    double FEBB;			/*free energy  of BB dimer*/
    double FEA;
    double FEB;

    struct plist *prAB = NULL;
    struct plist *prAA;		    /*pair probabilities of AA dimer*/
    struct plist *prBB;
    struct plist *prA;
    struct plist *prB;
    struct plist *mfAB;
    struct plist *mfAA;		    /*pair mfobabilities of AA dimer*/
    struct plist *mfBB;
    struct plist *mfA;
    struct plist *mfB;
    double *ConcAandB = NULL;


    AjPSeq  seq1    = NULL;
    AjPFile confile1 = NULL;
    AjPSeq  seq2    = NULL;
    AjPFile confile2 = NULL;
    AjPFile concfile = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;
    AjPFile essfile = NULL;
    AjPFile dotfile = NULL;
    AjPFile aoutf  = NULL;
    AjPFile aaoutf = NULL;
    AjPFile boutf  = NULL;
    AjPFile bboutf = NULL;
    AjPFile aboutf = NULL;

    

    AjPStr seqstring1 = NULL;
    AjPStr constring1 = NULL;
    AjPStr constring2 = NULL;
  
    float eT = 0.;
    AjBool eGU;
  
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

/*    AjBool dimers; */
    AjBool paired;
  

    embInitP("vrnacofoldpf",argc,argv,"VIENNA");
    
    
    seqstring1 = ajStrNew();
    constring1 = ajStrNew();
    constring2 = ajStrNew();
    
    
    seq1      = ajAcdGetSeq("asequence");
    confile1  = ajAcdGetInfile("aconstraintfile");
    seq2      = ajAcdGetSeq("bsequence");
    confile2  = ajAcdGetInfile("bconstraintfile");
    paramfile = ajAcdGetInfile("paramfile");

    eT        = ajAcdGetFloat("temperature");
    eGU       = ajAcdGetBool("gu");
    eclose    = ajAcdGetBool("closegu");
    lonely    = ajAcdGetBool("lp");
    convert   = ajAcdGetBool("convert");
    ensbases  = ajAcdGetString("nsbases");
    etloop    = ajAcdGetBool("tetraloop");
    eenergy   = ajAcdGetList("energy");
    escale    = ajAcdGetFloat("scale");
    edangles  = ajAcdGetList("dangles");
/*    dimers    = ajAcdGetBool("dimers"); */
    paired    = ajAcdGetBool("paired");
    outf      = ajAcdGetOutfile("outfile");
    essfile   = ajAcdGetOutfile("ssoutfile");
    dotfile   = ajAcdGetOutfile("dotoutfile");
    
/*
    aoutf     = ajAcdGetOutfile("aoutfile");
    aaoutf    = ajAcdGetOutfile("aaoutfile");
    boutf     = ajAcdGetOutfile("boutfile");
    bboutf    = ajAcdGetOutfile("bboutfile");
    aboutf    = ajAcdGetOutfile("aboutfile");
*/


    do_backtrack = 1; 
    pf   = 1;
    doT  = 0;
    doC  = 0;
    cofi = 0;
    
    string   = NULL;
    Concfile = NULL;
    istty = 0;

    temperature   = (double) eT;
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

    doQ = !!paired;
    
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

    

    cut_point = -1;
    if (doC)
    {
	ConcAandB=(double *)space(20*sizeof(double));
    }

    ajFmtPrintS(&seqstring1,"%s&%s",ajSeqGetSeqC(seq1),ajSeqGetSeqC(seq2));
    string = tokenize(MAJSTRGETPTR(seqstring1));
    length = (int) strlen(string);


    structure = (char *) space((unsigned) length+1);
    if(confile1)
    {
	vienna_GetConstraints(confile1,&constring1);
	vienna_GetConstraints(confile2,&constring2);
	ajStrAppendK(&constring1,'&');
	ajStrAppendS(&constring1,constring2);

	cstruc = tokenize(MAJSTRGETPTR(constring1));
	if (cstruc!=NULL)
	    strncpy(structure, cstruc, length);
	else
	    ajFatal("Constraints missing\n");
    }
    
    for (l = 0; l < length; l++)
    {
	string[l] = toupper(string[l]);
	if (!noconv && string[l] == 'T')
	    string[l] = 'U';
    }

    /*compute mfe of AB dimer*/
    min_en = cofold(string, structure);
    mfAB=(struct plist *) space(sizeof(struct plist) * (length+1));
    mfAB=get_mfe_plist(mfAB);
    
    /*     for (l=0; mfAB[l].i!=0; l++) { */
    /*       if (!SAME_STRAND(mfAB[l].i,mfAB[l].j)) { */
    /* 	min_en += 4.1; /\*can i use DuplexInit there? if not: Ivooo??*\/ */
    /* 	break; */
    /*       } */
    /*     } */
    
    if (cut_point == -1)
	ajFmtPrintF(outf,"%s\n%s", string, structure); /*no cofold*/
    else
    {
	char *pstring, *pstruct;
	pstring = costring(string);
	pstruct = costring(structure);
	ajFmtPrintF(outf,"%s\n%s", pstring,  pstruct);
	free(pstring);
	free(pstruct);
    }
    
    ajFmtPrintF(outf," (%6.2f)\n", min_en);
    
    
    if (length<2000)
	(void) PS_rna_plot(string, structure, essfile);
    else
    { 
	ajWarn("Structure too long, not doing xy_plot\n");
	free_co_arrays();  
    }
    
    /*compute partition function*/
    if (pf)
    {
	if (dangles==1)
	{
	    dangles=2;	  /* recompute with dangles as in pf_fold() */
	    min_en = energy_of_struct(string, structure);
	    dangles=1;
	}
      	 
	kT = (temperature+273.15)*1.98717/1000.; /* in Kcal */
	pf_scale = exp(-(sfact*min_en)/kT/length);
	if (length>2000)
	    ajWarn("Scaling factor %f\n", pf_scale);

	init_co_pf_fold(length);

	if (cstruc!=NULL)
	    strncpy(structure, cstruc, length+1);
	energy = co_pf_fold(string, structure);
	prAB=(struct plist *) space(sizeof(struct plist) * (2*length));
	prAB=get_plist(prAB, length,0.00001); 
	FEAB=energy;
	if (doQ)
	    make_probsum(length,fname,outf); /*compute prob of base paired*/
	free_co_arrays();

	if (doT)
	{			    /*cofold of all dimers, monomers*/
	    int Blength, Alength;
	    char  *Astring, *Bstring;
	    char *Newstring;

	    char *comment;
	    struct ConcEnt *Conc;
	    if (cut_point<0)
	    {
		free(mfAB);
		free(prAB);
		ajFatal("This program requires two molecules\n");
	    }

	    if (dangles==1)
		dangles=2;		/*merkmas??*/
	    Alength=cut_point-1;	/*length of first molecule*/
	    Blength=length-cut_point+1; /*length of 2nd molecule*/
	
	    /*Sequence of first molecule*/
	    Astring=(char *)space(sizeof(char)*(Alength+1));
	    /*Sequence of second molecule*/
	    Bstring=(char *)space(sizeof(char)*(Blength+1));
	    strncat(Astring,string,Alength);      
	    strncat(Bstring,string+Alength,Blength);
	
	
	
	    /*compute AA dimer*/
	    prAA=(struct plist *) space(sizeof(struct plist) * (4*Alength));
	    mfAA=(struct plist *) space(sizeof(struct plist) * (Alength+1));
	    FEAA=do_partfunc(Astring, Alength, 2, &prAA, &mfAA);
	    /*compute BB dimer*/
	    prBB=(struct plist *) space(sizeof(struct plist) * (4*Blength));
	    mfBB=(struct plist *) space(sizeof(struct plist) * (Blength+1));
	    FEBB=do_partfunc(Bstring, Blength, 2, &prBB, &mfBB);
	    /*free_co_pf_arrays();*/
	
	    /*compute A monomer*/
	    prA=(struct plist *) space(sizeof(struct plist) * (2*Alength));
	    mfA=(struct plist *) space(sizeof(struct plist) * (Alength+1));
	    if(Alength>4)
	    {			     /*only if sec_struc is possible*/
		FEA=do_partfunc(Astring, Alength, 1, &prA, &mfA);
		/*	  free_pf_arrays();*/

	    }
	    else
	    {				/*no secondary structure*/
		FEA=0.;
		prA=(struct plist *)xrealloc(prA,sizeof(struct plist));
		mfA=(struct plist *)xrealloc(mfA,sizeof(struct plist));
		prA[0].i=mfA[0].i=0;
		prA[0].j=mfA[0].j=0;
		prA[0].p=mfA[0].p=0.;
	    }
	    /*compute B monomer*/
	    prB=(struct plist *) space(sizeof(struct plist) * (2*Blength));
	    mfB=(struct plist *) space(sizeof(struct plist) * (Blength+1));
	
	    if (Blength>4)
	    {
	  
		FEB=do_partfunc(Bstring, Blength, 1, &prB, &mfB); 
		/*	  free_pf_arrays();*/
	  
	  
	    }
	    else
	    {
		FEB=0.;
		prB=(struct plist *)xrealloc(prB,sizeof(struct plist));
		mfB=(struct plist *)xrealloc(mfB,sizeof(struct plist));
		prB[0].i=mfB[0].i=0;
		prB[0].j=mfB[0].j=0;
		prB[0].p=mfB[0].p=0.;
	 
	    } 

	    compute_probabilities(&FEAB,&FEAA,&FEBB,&FEA,&FEB,
				  prAB,prAA,prBB,prA,prB,
				  Alength,Blength,outf);
	    ajFmtPrintF(outf,"\nFree Energies:\n    AB\t\tAA\t\tBB\t\tA\t\t"
			"B\n%.6f\t%6f\t%6f\t%6f\t%6f\n",FEAB,FEAA,FEBB,FEA,
			FEB);
	
	    if (doC)
	    {
		Conc=do_the_concenratinger(concfile,FEAB, FEAA, FEBB, FEA,
					   FEB, ConcAandB, outf);
		free(Conc);		/*freeen*/
	    }
	
	    /*output of the 5 dot plots*/
	
	    /*AB dot_plot*/
	    /*write Free Energy into comment*/ 
	    comment=(char *)space(80*sizeof(char));
	    sprintf(comment,"\n%%FreeEnergy= %.9f\n",FEAB);
	    /*reset cut_point*/
	    cut_point=Alength+1;
	    (void)PS_dot_plot_list(string, aboutf, prAB, mfAB, comment);
	    free(comment);
	
	    /*AA dot_plot*/
	    comment=(char *)space(80*sizeof(char));
	    sprintf(comment,"\n%%FreeEnergy= %.9f\n",FEAA);
	    /*write AA sequence*/
	    Newstring=(char*)space((2*Alength+1)*sizeof(char));
	    strcat(Newstring,Astring);
	    strcat(Newstring,Astring);
	    (void)PS_dot_plot_list(Newstring, aaoutf, prAA, mfAA, comment);
	    free(comment);
	    free(Newstring);

	    /*BB dot_plot*/
	    comment=(char *)space(80*sizeof(char));
	    sprintf(comment,"\n%%FreeEnergy= %.9f\n",FEBB);
	    /*write BB sequence*/
	    Newstring=(char*)space((2*Blength+1)*sizeof(char));
	    strcat(Newstring,Bstring);
	    strcat(Newstring,Bstring);
	    /*reset cut_point*/
	    cut_point=Blength+1;
	    (void)PS_dot_plot_list(Newstring, bboutf, prBB, mfBB, comment);
	    free(comment);
	    free(Newstring);

	    /*A dot plot*/
	    /*reset cut_point*/
	    cut_point=-1;
	    comment=(char *)space(80*sizeof(char));
	    sprintf(comment,"\n%%FreeEnergy= %.9f\n",FEA);
	    (void)PS_dot_plot_list(Astring, aoutf, prA, mfA, comment);
	    free(comment);


	    /*B monomer dot plot*/
	    comment=(char *)space(80*sizeof(char));
	    sprintf(comment,"\n%%FreeEnergy= %.9f\n",FEB);
	    (void)PS_dot_plot_list(Bstring, boutf, prB, mfB, comment);
	    free(comment);
		
	    free_franz(Astring, Bstring,  prAB, prAA, prBB, prA, prB,  mfAB,
		       mfAA, mfBB, mfA, mfB);
	
	
	} /*end if(doT)*/

	if (do_backtrack)
	{
	    ajFmtPrintF(outf,"%s", structure);
	    if (!istty)
		ajFmtPrintF(outf," [%6.2f]\n", energy);
	    else
		printf("\n");		/*8.6.04*/
	}

	if ((istty)||(!do_backtrack)) 
	    ajFmtPrintF(outf," free energy of ensemble = %6.2f kcal/mol\n",
			energy);
	ajFmtPrintF(outf," frequency of mfe structure in ensemble %g\n",
	       exp((energy-min_en)/kT));
     
	
    } /*end if(pf)*/
    
    
    if (do_backtrack)
    {
	if (!doT)
	{
	    if(pf)
	    {
		(void) PS_dot_plot_list(string, dotfile, prAB, mfAB, "doof");
		free(prAB);
	    }
	    free(mfAB);
	}
    }
    if(!doT)
	free_co_pf_arrays();
    
    
    if (cstruc!=NULL)
	free(cstruc);

    free(string);

    free(structure); 
    
    
    
    ajSeqDel(&seq1);
    ajSeqDel(&seq2);

    ajStrDel(&constring1);
    ajStrDel(&constring2);


    ajFileClose(&outf);

    embExit();

    return 0;
}



 
PRIVATE char *tokenize(char *line)
{
    char *pos, *copy;
    int cut = -1;

    copy = (char *) space(strlen(line)+1);
    (void) sscanf(line, "%s", copy);
    pos = strchr(copy, '&');
    if (pos)
    {
	cut = (int) (pos-copy)+1;
	if (cut >= strlen(copy))
	    cut = -1;
	if (strchr(pos+1, '&'))
	    ajFatal("More than one cut-point in input\n");
	for (;*pos;pos++)
	    *pos = *(pos+1);		/* splice out the & */
    }

    if(cut > -1)
    {
	if (cut_point==-1)
	    cut_point = cut;
	else if(cut_point != cut)
	{
	    ajFatal("cut_point = %d cut = %d\n"
		    "Sequence and Structure have different cut points.\n",
		    cut_point,cut);
	}
    }

    free(line);

    return copy;
}




PRIVATE char *costring(char *string)
{
    char *ctmp;
    int len;
  
    len = strlen(string);
    ctmp = (char *)space((len+2) * sizeof(char));
    /* first sequence */
    (void) strncpy(ctmp, string, cut_point-1);
    /* spacer */
    ctmp[cut_point-1] = '&';
    /* second sequence */
    (void) strcat(ctmp, string+cut_point-1);

    return ctmp;
}




PRIVATE double do_partfunc(char *string, int length, int Switch,
			   struct plist **tpr, struct plist **mfpl)
{
    /*compute mfe and partition function of dimere or  monomer*/  
    double En = 0.;
    char *Newstring;
    char *tempstruc;
    double min_en;
    double sfact=1.07;
    double kT;
    kT = (temperature+273.15)*1.98717/1000.;
    switch (Switch)
    {
    case 1:				/*monomer*/
	cut_point=-1;
	tempstruc = (char *) space((unsigned)length+1);
	min_en = fold(string, tempstruc);
	pf_scale = exp(-(sfact*min_en)/kT/(length));
	*mfpl=get_mfe_plist(*mfpl);
	free_arrays();
	/*En=pf_fold(string, tempstruc);*/
	init_co_pf_fold(length);
	En=co_pf_fold(string, tempstruc);
      
	*tpr=get_plist(*tpr, length,0.00001);
	free_co_pf_arrays();
	free(tempstruc);
	break;

    case 2:				/*dimer*/
	Newstring=(char *)space(sizeof(char)*(length*2+1)); 
	strcat(Newstring,string);
	strcat(Newstring,string);
	cut_point=length+1;
	tempstruc = (char *) space((unsigned)length*2+1);
	min_en = cofold(Newstring, tempstruc);
	pf_scale =exp(-(sfact*min_en)/kT/(2*length));
	*mfpl=get_mfe_plist(*mfpl);
	free_co_arrays(); 
	init_co_pf_fold(2*length);
	En=co_pf_fold(Newstring, tempstruc);
	*tpr=get_plist(*tpr, 2*length,0.00001);
	free_co_pf_arrays();
	free(Newstring);
	free(tempstruc);
	break;

    default:
	ajFatal("Error in get_partfunc\n, computing neither mono- nor dimer!\n");
    }

    return En;
}




PRIVATE void free_franz(char *Astring, char *Bstring, struct plist *prAB,
			struct plist *prAA, struct plist *prBB,
			struct plist *prA, struct plist *prB,
			struct plist *mfAB, struct plist *mfAA,
			struct plist *mfBB, struct plist *mfA,
			struct plist *mfB)
{

    /*free arrays for dimer/monomer computations*/
    free(Astring);
    free(Bstring);
    free(prAB);
    free(prAA);
    free(prBB);
    free(prB);
    free(prA);
    free(mfAB);
    free(mfAA);
    free(mfBB);
    free(mfA);
    free(mfB);

    return;
}




PRIVATE struct ConcEnt *do_the_concenratinger(AjPFile Conc_file,double FEAB,
					      double FEAA, double FEBB,
					      double FEA, double FEB,
					      double *startconces,
					      AjPFile outf)
{
    /*compute concentrations out of  free energies, calls get_concentrations*/
    struct ConcEnt *result;
    int i;
    char *line;
    double temp;
    int n=1;
 
    i=0;

    if(Conc_file)
	i=read_concentrationfile(Conc_file,startconces); /*??*/
    else if(startconces[0]==0)
    {
	printf("Please enter concentrations, alternating A,B, '*'to stop\n");
	do
	{
	    if ((line = get_line(stdin))==NULL)
		break;
	    if ((line ==NULL) || (strcmp(line, "*") == 0))
		break;
	    sscanf(line,"%lf", &temp);
	    startconces[i++]=temp;
	    free(line);
	    if (i==n*20-1)
	    {
		n*=2;
		startconces=(double *)xrealloc(startconces,(2+n*20)*
					       sizeof(double));
	    }
	}while (1);
    
	if (!i%2)
	{
	    printf("Warning! number of concentrations is not "
		   "a multiple of 2!!\n");
	}
	startconces[i++]=0;
	startconces[i]=0;
    }
   
    result=get_concentrations(FEAB, FEAA, FEBB, FEA, FEB, startconces, outf);
  
    free(startconces);

    return result;
}




PRIVATE int read_concentrationfile(AjPFile concfile, double *startc)
{
    /*reads file of concentrations*/
    FILE *fp;
    char *line;
    int i,n;
    double tmp1, tmp2;
    n=1;
    i=0;

    fp = ajFileFp(concfile);

    line=get_line(fp);
    while(line!=NULL)
    {
	sscanf(line,"%lf %lf",&tmp1,&tmp2);
	startc[i++]=tmp1;
	startc[i++]=tmp2;
	free(line);
	if(i==n*20)
	{
	    n*=2;
	    startc=(double *)xrealloc(startc,(2+20*n)*sizeof(double));
	}
	line=get_line(fp);
    }

    free(line);
    startc[i++]=0;
    startc[i--]=0;

    return --i;
}
