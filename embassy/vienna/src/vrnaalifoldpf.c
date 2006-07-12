/* Last changed Time-stamp: <2005-03-21 13:49:55 ivo> */
/*                
		  Access to alifold Routines

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
#include "pair_mat.h"
#include "alifold.h"
#include "aln_util.h"
extern void  read_parameter_file(AjPFile fname);
/*@unused@*/
static const char rcsid[] = "$Id: vrnaalifoldpf.c,v 1.5 2006/07/12 15:50:46 rice Exp $";

#define PRIVATE static

static const char scale[] = "....,....1....,....2....,....3....,....4"
                            "....,....5....,....6....,....7....,....8";

PRIVATE char *annote(const char *structure, const char *AS[]);
PRIVATE void print_pi(const pair_info pi, FILE *file);
PRIVATE cpair *make_color_pinfo(const pair_info *pi);
PRIVATE void mark_endgaps(char *seq, char egap);

extern AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);

/*--------------------------------------------------------------------------*/
#define MAX_NUM_NAMES    500
int main(int argc, char *argv[])
{
    char *string;
    char *structure=NULL;
    char *cstruc=NULL;
    char *ns_bases=NULL;
    char *c;
    int  n_seq;
    int  i;
    int  length;
    int  sym;
    int  endgaps = 0;
    int  mis = 0;
    double min_en;
    double real_en;
    double sfact = 1.07;
    int  pf = 0;
    int  istty;
    char *AS[MAX_NUM_NAMES];	/* aligned sequences */
    char *names[MAX_NUM_NAMES];	/* sequence names */

    AjPSeqset  seq       = NULL;

    AjPFile confile   = NULL;
    AjPFile paramfile = NULL;
    AjPFile outf      = NULL;
    AjPFile essfile   = NULL;
    AjPFile dotfile   = NULL;
    

    AjPStr constring = NULL;
  
    float eT = 0.;
    AjBool eGU;
  
    AjBool eclose;
    AjBool lonely;
    AjPStr ensbases = NULL;
    AjBool etloop;
    AjPStr *eenergy = NULL;
    char ewt = '\0';
    float escale = 0.;
    AjPStr *edangles = NULL;
    char edangle = '\0';

    ajint len;

    AjPSeq tseq = NULL;
    AjPStr tname = NULL;
    

    embInitP("vrnaalifoldpf",argc,argv,"VIENNA");
    
    
    constring = ajStrNew();
    
    seq       = ajAcdGetSeqset("sequence");
    confile   = ajAcdGetInfile("constraintfile");
    paramfile = ajAcdGetInfile("paramfile");
    eT        = ajAcdGetFloat("temperature");
    eGU       = ajAcdGetBool("gu");
    eclose    = ajAcdGetBool("closegu");
    lonely    = ajAcdGetBool("lp");
    ensbases  = ajAcdGetString("nsbases");
    etloop    = ajAcdGetBool("tetraloop");
    eenergy   = ajAcdGetList("energy");
    escale    = ajAcdGetFloat("scale");
    edangles  = ajAcdGetList("dangles");
    mis       = !!ajAcdGetBool("most");
    endgaps   = !!ajAcdGetBool("endgaps");
    nc_fact   = (double) ajAcdGetFloat("nspenalty");
    cv_fact   = (double) ajAcdGetFloat("covariance");

    outf      = ajAcdGetOutfile("outfile");
    essfile   = ajAcdGetOutfile("ssoutfile");
    dotfile   = ajAcdGetOutfile("dotoutfile");
    
    do_backtrack = 1; 
    pf = 1;
    string = NULL;
    istty = 0;
    dangles = 2;

    temperature   = (double) eT;
    noGU          = (eGU) ? 0 : 1;
    no_closingGU  = (eclose) ? 0 : 1;
    noLonelyPairs = (lonely) ? 0 : 1;
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

    n_seq = ajSeqsetSize(seq);

    if(n_seq > MAX_NUM_NAMES - 1)
	ajFatal("[e]RNAalifold is restricted to %d sequences\n",
		MAX_NUM_NAMES - 1);

    if (n_seq==0)
	ajFatal("No sequences found");

    for(i=0;i<n_seq;++i)
    {
	tseq  = (AjPSeq) ajSeqsetGetSeq(seq,i);
	tname = (AjPStr) ajSeqsetName(seq,i);
	len   = ajSeqGetLen(tseq);
	AS[i] = (char *) space(len+1);
	names[i] = (char *) space(ajStrGetLen(tname)+1);
	strcpy(AS[i],ajSeqGetSeqC(tseq));
	strcpy(names[i],ajStrGetPtr(tname));
    }
    AS[n_seq] = NULL;
    names[n_seq] = NULL;
    
    if (endgaps)
	for (i=0; i<n_seq; i++)
	    mark_endgaps(AS[i], '~');


    length = (int) strlen(AS[0]);
    structure = (char *) space((unsigned) length+1);
    if(confile)
    {
	fold_constrained = 1;
	strcpy(structure,ajStrGetPtr(constring));
    }
  
    min_en = alifold(AS, structure);
    {
	int i;
	double s=0;
	extern int eos_debug;
	eos_debug=-1;  /* shut off warnings about nonstandard pairs */

	for (i=0; AS[i]!=NULL; i++) 
	    s += energy_of_struct(AS[i], structure);
	real_en = s/i;
    }

    string = (mis) ?
	consens_mis((const char **) AS) : consensus((const char **) AS);
    ajFmtPrintF(outf,"%s\n%s", string, structure);
    ajFmtPrintF(outf," (%6.2f = %6.2f + %6.2f) \n", min_en, real_en,
		min_en-real_en );
  
    if (length<=2500)
    {
	char *A;
	A = annote(structure, (const char**) AS);
	(void) PS_rna_plot_a(string, structure, essfile, NULL, A);
	free(A);
    }
    else 
	ajWarn("Structure too long, not doing xy_plot\n");
  
    {
	/* free mfe arrays but preserve base_pair for PS_dot_plot */
	struct bond  *bp;
	bp = base_pair; base_pair = space(16);
	free_alifold_arrays();		/* free's base_pair */
	base_pair = bp;
    }

    if (pf)
    {
	double energy, kT;
	pair_info *pi;
	char * mfe_struc;
	mfe_struc = strdup(structure);
    	 
	kT = (temperature+273.15)*1.98717/1000.; /* in Kcal */
	pf_scale = exp(-(sfact*min_en)/kT/length);
	if (length>2000)
	    ajWarn("scaling factor %f\n", pf_scale);
    
	/* init_alipf_fold(length); */
    
	if(confile)
	    strncpy(structure, ajStrGetPtr(constring), length+1);
	energy = alipf_fold(AS, structure, &pi);
    
	if (do_backtrack)
	{
	    ajFmtPrintF(outf,"%s", structure);
	    ajFmtPrintF(outf," [%6.2f]\n", energy);
	}

	if ((istty)||(!do_backtrack)) 
	    ajFmtPrintF(outf," free energy of ensemble = %6.2f kcal/mol\n",
			energy);
	ajFmtPrintF(outf," frequency of mfe structure in ensemble %g\n",
	       exp((energy-min_en)/kT));
    
	if(do_backtrack)
	{
	    FILE *aliout;
	    cpair *cp;
	    short *ptable; int k;

	    ptable = make_pair_table(mfe_struc);
	    ajFmtPrintF(outf,"\n#Alignment section\n\n");
	    aliout = ajFileFp(outf);
	    fprintf(aliout, "%d sequences; length of alignment %d\n", 
		    n_seq, length);
	    fprintf(aliout, "alifold output\n");
	    for (k=0; pi[k].i>0; k++)
	    {
		pi[k].comp = (ptable[pi[k].i] == pi[k].j) ? 1:0;
		print_pi(pi[k], aliout);
	    }
	    fprintf(aliout, "%s\n", structure);
	    free(ptable);

	    cp = make_color_pinfo(pi);
	    (void) PS_color_dot_plot(string, cp, dotfile);
	    free(cp);
	    free(mfe_struc);
	    free(pi);
	}
    }

    if (cstruc!=NULL)
	free(cstruc);
    free(base_pair);
    (void) fflush(stdout);

    free(string);
    free(structure);
    for (i=0; AS[i]; i++)
    {
	free(AS[i]);
	free(names[i]);
    }

    ajStrDel(&constring);
    ajFileClose(&outf);
    ajFileClose(&essfile);
    ajFileClose(&dotfile);

    embExit();

    return 1;
}




void mark_endgaps(char *seq, char egap)
{
    int i,n;

    n = strlen(seq);
    for (i=0; i<n && (seq[i]=='-'); i++)
    {
	seq[i] = egap;
    }

    for (i=n-1; i>0 && (seq[i]=='-'); i--)
    {
	seq[i] = egap;
    }

    return;
}



 
void print_pi(const pair_info pi, FILE *file)
{
    const char *pname[8] = {"","CG","GC","GU","UG","AU","UA", "--"};
    int i;
  
    /* numbering starts with 1 in output */
    fprintf(file, "%5d %5d %2d %5.1f%% %7.3f",
	    pi.i, pi.j, pi.bp[0], 100.*pi.p, pi.ent);
    for (i=1; i<=7; i++) 
	if (pi.bp[i])
	    fprintf(file, " %s:%-4d", pname[i], pi.bp[i]);
    /* if ((!pi.sym)&&(pi.j>=0)) printf(" *"); */
    if (!pi.comp)
	fprintf(file, " +");

    fprintf(file, "\n");

    return;
}




#define MIN2(A, B)      ((A) < (B) ? (A) : (B))

PRIVATE cpair *make_color_pinfo(const pair_info *pi)
{
    cpair *cp;
    int i, n;
    for (n=0; pi[n].i>0; n++);
    cp = (cpair *) space(sizeof(cpair)*(n+1));
    for (i=0; i<n; i++)
    {
	int j, ncomp;
	cp[i].i = pi[i].i;
	cp[i].j = pi[i].j;
	cp[i].p = pi[i].p;
	for (ncomp=0, j=1; j<=6; j++) if (pi[i].bp[j]) ncomp++;
	cp[i].hue = (ncomp-1.0)/6.2; /* hue<6/6.9 (hue=1 ==  hue=0) */
	cp[i].sat = 1 - MIN2( 1.0, pi[i].bp[0]/2.5);
	cp[i].mfe = pi[i].comp;
    }

    return cp;
}




PRIVATE char *annote(const char *structure, const char *AS[])
{
    char *ps;
    int i, n, s, maxl;
    short *ptable;
    make_pair_matrix();
    n = strlen(AS[0]);
    maxl = 1024;
    ps = (char *) space(maxl);
    ptable = make_pair_table(structure);
    for (i=1; i<=n; i++)
    {
	char pps[64], ci='\0', cj='\0';
	int j, type, pfreq[8] = {0,0,0,0,0,0,0,0}, vi=0, vj=0;
	if ((j=ptable[i])<i) continue;
	for (s=0; AS[s]!=NULL; s++)
	{
	    type = pair[encode_char(AS[s][i-1])][encode_char(AS[s][j-1])];
	    pfreq[type]++;
	    if (type)
	    {
		if (AS[s][i-1] != ci) { ci = AS[s][i-1]; vi++;}
		if (AS[s][j-1] != cj) { cj = AS[s][j-1]; vj++;}
	    }
	}

	if (maxl - strlen(ps) < 128)
	{ 
	    maxl *= 2;
	    ps = realloc(ps, maxl);
	    if (ps==NULL) nrerror("out of memory in realloc");
	}

	if (pfreq[0]>0)
	{
	    snprintf(pps, 64, "%d %d %d gmark\n", i, j, pfreq[0]);
	    strcat(ps, pps);
	}

	if (vi>1)
	{
	    snprintf(pps, 64, "%d cmark\n", i);
	    strcat(ps, pps);
	}

	if (vj>1)
	{
	    snprintf(pps, 64, "%d cmark\n", j);
	    strcat(ps, pps);
	}
    }

    free(ptable);
    return ps;
}
