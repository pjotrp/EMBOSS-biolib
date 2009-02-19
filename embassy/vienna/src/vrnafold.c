/* Last changed Time-stamp: <2007-12-05 13:55:42 ronny> */
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
#include "PS_dot.h"
#include "utils.h"
extern void  read_parameter_file(AjPFile file);
extern AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);
extern float circfold(const char *string, char *structure);
extern plist * stackProb(double cutoff);
extern void init_pf_circ_fold(int length);

/*@unused@*/
#if 0
static char UNUSED rcsid[] = "$Id: vrnafold.c,v 1.10 2009/02/19 13:11:56 rice Exp $";
#endif

#define PRIVATE static

#if 0
static char  scale1[] = "....,....1....,....2....,....3....,....4";
static char  scale2[] = "....,....5....,....6....,....7....,....8";
#endif

PRIVATE struct plist *b2plist(const char *struc);
PRIVATE struct plist *make_plist(int length, double pmin);
#if 0
PRIVATE void usage(void);
#endif

/*--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char *string/*, *line*/;
    char *structure=NULL, *cstruc=NULL;
    /*char  fname[13], ffname[20], gfname[20];*/
    /*char  *ParamFile=NULL;*/
    char  *ns_bases=NULL, *c;
    int   i, length, l, sym/*, r*/;
    double energy, min_en;
    double kT, sfact=1.07;
    int   pf=0, noPS=0, istty;
    int noconv=0;
    int circ=0;

    AjPSeq  seq     = NULL;
    AjPFile confile = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf = NULL;
    AjPFile essfile = NULL;
    AjPFile dotfilea = NULL;
    AjPFile dotfileb = NULL;
    

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
    AjPStr eenergy = NULL;
    char ewt = '\0';
    float escale = 0.;
    AjPStr edangles = NULL;
    char edangle = '\0';

    ajint len;



    embInitP("vrnafold",argc,argv,"VIENNA");
    
    
    seqstring = ajStrNew();
    constring = ajStrNew();
    seqname   = ajStrNew();
    
    
    seq       = ajAcdGetSeq("sequence");
    confile   = ajAcdGetInfile("constraintfile");
    paramfile = ajAcdGetInfile("paramfile");
    eT        = ajAcdGetFloat("temperature");
    ecirc     = ajAcdGetBoolean("circular");
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
    essfile   = ajAcdGetOutfile("ssoutfile");
    /*
      dotfilea  = ajAcdGetOutfile("adotoutfile");
      dotfileb  = ajAcdGetOutfile("bdotoutfile");
    */
    
    do_backtrack = 2; 
    pf = 0;
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


    if(circ && noLonelyPairs)
    {

        ajWarn("Depending on the origin of the circular sequence\n"
               "some structures may be missed when using -noLP\nTry "
               "rotating your sequence a few times\n");        
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
    

    for (l = 0; l < length; l++) {
        string[l] = toupper(string[l]);
        if (!noconv && string[l] == 'T') string[l] = 'U';
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

    if (!noPS)
    {
        if (length<2000)
            (void) PS_rna_plot(string, structure, essfile);
        else
            ajWarn("Structure too long, not doing xy_plot\n");
    }
    if (length>=2000) free_arrays(); 

    if (pf)
    {
        char *pf_struc;
        pf_struc = (char *) space((unsigned) length+1);
	if (dangles==1)
        {
            dangles=2;   /* recompute with dangles as in pf_fold() */
            min_en = (circ) ? energy_of_circ_struct(string, structure) :
                energy_of_struct(string, structure);
            dangles=1;
        }

        kT = (temperature+273.15)*1.98717/1000.; /* in Kcal */
        pf_scale = exp(-(sfact*min_en)/kT/length);

        if (length>2000)
            ajWarn("scaling factor %f\n", pf_scale);

        (circ) ? init_pf_circ_fold(length) : init_pf_fold(length);

        if (cstruc!=NULL)
            strncpy(pf_struc, cstruc, length+1);

        energy = (circ) ? pf_circ_fold(string, pf_struc) :
            pf_fold(string, pf_struc);

        if (do_backtrack)
        {
            ajFmtPrintF(outf,"%s", pf_struc);
            ajFmtPrintF(outf," [%6.2f]\n", energy);
        }

        if ((istty)||(!do_backtrack))
            ajFmtPrintF(outf," free energy of ensemble = %6.2f kcal/mol\n",
                        energy);

        if (do_backtrack)
        {
            plist *pl1,*pl2;
            char *cent;
            double dist, cent_en;
            cent = centroid(length, &dist);
            cent_en = (circ) ? energy_of_circ_struct(string, cent) :
                energy_of_struct(string, cent);
            ajFmtPrintF(outf,"%s {%6.2f d=%.2f}\n", cent, cent_en, dist);
            free(cent);

            pl1 = make_plist(length, 1e-5);
            pl2 = b2plist(structure);
            (void) PS_dot_plot_list(string, dotfilea, pl1, pl2, "");
            free(pl2);
            if (do_backtrack==2)
            {
                pl2 = stackProb(1e-5);
                PS_dot_plot_list(string, dotfileb, pl1, pl2,
                                 "Probabilities for stacked pairs (i,j)(i+1,j-1)");
                free(pl2);
            }
            free(pl1);
            free(pf_struc);
        }

        ajFmtPrintF(outf," frequency of mfe structure in ensemble %g; ",
                    exp((energy-min_en)/kT));

        if (do_backtrack)
            ajFmtPrintF(outf,"ensemble diversity %-6.2f", mean_bp_dist(length));

        ajFmtPrintF(outf,"\n");
        free_pf_arrays();

    }

    if (cstruc!=NULL)
        free(cstruc);

    free(string);
    free(structure);

    ajStrDel(&seqstring);
    ajStrDel(&constring);
    ajStrDel(&seqname);

    ajStrDel(&ensbases);
    ajStrDel(&eenergy);
    ajStrDel(&edangles);

    ajSeqDel(&seq);

    ajFileClose(&confile);
    ajFileClose(&paramfile);
    ajFileClose(&outf);
    ajFileClose(&essfile);

/*
  ajFileClose(&dotfilea);
  ajFileClose(&dotfileb);
*/  
    if (length<2000) free_arrays(); 
    embExit();
    
    return 0;
}




PRIVATE struct plist *b2plist(const char *struc) {
  /* convert bracket string to plist */
  short *pt;
  struct plist *pl;
  int i,k=0;
  pt = make_pair_table(struc);
  pl = (struct plist *)space(strlen(struc)/2*sizeof(struct plist));
  for (i=1; i<strlen(struc); i++) {
    if (pt[i]>i) {
      pl[k].i = i;
      pl[k].j = pt[i];
      pl[k++].p = 0.95*0.95;
    }
  }
  free(pt);
  pl[k].i=0;
  pl[k].j=0;
  pl[k++].p=0.;
  return pl;
}




PRIVATE struct plist *make_plist(int length, double pmin) {
  /* convert matrix of pair probs to plist */
  struct plist *pl;
  int i,j,k=0,maxl;
  maxl = 2*length;
  pl = (struct plist *)space(maxl*sizeof(struct plist));
  k=0;
  for (i=1; i<length; i++)
    for (j=i+1; j<=length; j++) {
      if (pr[iindx[i]-j]<pmin) continue;
      if (k>=maxl-1) {
	maxl *= 2;
	pl = (struct plist *)xrealloc(pl,maxl*sizeof(struct plist));
      }
      pl[k].i = i;
      pl[k].j = j;
      pl[k++].p = pr[iindx[i]-j];
    }
  pl[k].i=0;
  pl[k].j=0;
  pl[k++].p=0.;
  return pl;
}



#if 0
PRIVATE void usage(void)
{
  nrerror("usage:\n"
	  "RNAfold [-p[0|2]] [-C] [-T temp] [-4] [-d[2|3]] [-noGU] [-noCloseGU]\n"
	  "        [-noLP] [-e e_set] [-P paramfile] [-nsp pairs] [-S scale]\n"
	  "        [-noconv] [-noPS] [-circ] \n");
}
#endif
