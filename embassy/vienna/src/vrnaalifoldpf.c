/* Last changed Time-stamp: <2007-04-02 18:36:38 ivo> */
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
#include "PS_dot.h"
#include "utils.h"
#include "pair_mat.h"
#include "alifold.h"
#include "aln_util.h"

extern void  read_parameter_file(AjPFile fname);
extern float circalifold(const char **strings, char *structure);
extern float energy_of_circ_struct(const char *seq, const char *structure);
extern AjBool vienna_GetConstraints(AjPFile file, AjPStr *constring);

/*@unused@*/
static const char rcsid[] = "$Id: vrnaalifoldpf.c,v 1.11 2008/06/26 08:40:00 rice Exp $";

#define PRIVATE static

static const char scale[] = "....,....1....,....2....,....3....,....4"
			    "....,....5....,....6....,....7....,....8";

#if 0
PRIVATE void /*@exits@*/ usage(void);
#endif
PRIVATE char **annote(const char *structure, const char *AS[]);
PRIVATE void print_pi(const pair_info pi, FILE *file);
PRIVATE cpair *make_color_pinfo(const pair_info *pi);
PRIVATE void mark_endgaps(char *seq, char egap);

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
    AjPFile alifile   = NULL;
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

    int circ = 0;
    int doAlnPS = 0;
    int doColor = 0;
    

    embInitP("vrnaalifoldpf",argc,argv,"VIENNA");
    
    
    constring = ajStrNew();
    
    seq       = ajAcdGetSeqset("sequence");
    confile   = ajAcdGetInfile("constraintfile");
    paramfile = ajAcdGetInfile("paramfile");
    eT        = ajAcdGetFloat("temperature");
    eGU       = ajAcdGetBoolean("gu");
    eclose    = ajAcdGetBoolean("closegu");
    lonely    = ajAcdGetBoolean("lp");
    ensbases  = ajAcdGetString("nsbases");
    etloop    = ajAcdGetBoolean("tetraloop");
    eenergy   = ajAcdGetList("energy");
    escale    = ajAcdGetFloat("scale");
    edangles  = ajAcdGetList("dangles");
    mis       = !!ajAcdGetBoolean("most");
    endgaps   = !!ajAcdGetBoolean("endgaps");
    nc_fact   = (double) ajAcdGetFloat("nspenalty");
    cv_fact   = (double) ajAcdGetFloat("covariance");

    outf      = ajAcdGetOutfile("outfile");
    essfile   = ajAcdGetOutfile("ssoutfile");
    alifile   = ajAcdGetOutfile("alignoutfile");
    circ      = !!ajAcdGetBoolean("circular");
    doColor   = !!ajAcdGetBoolean("colour");

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

    if(alifile)
        doAlnPS = 1;

    
    if(confile)
	vienna_GetConstraints(confile,&constring);

    n_seq = ajSeqsetGetSize(seq);

    if(n_seq > MAX_NUM_NAMES - 1)
	ajFatal("[e]RNAalifold is restricted to %d sequences\n",
		MAX_NUM_NAMES - 1);

    if (n_seq==0)
	ajFatal("No sequences found");

    for(i=0;i<n_seq;++i)
    {
	tseq  = (AjPSeq) ajSeqsetGetseqSeq(seq,i);
	ajSeqGapStandard(tseq, '-');
	tname = (AjPStr) ajSeqsetGetseqNameS(seq,i);
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

    
  
  if (circ && noLonelyPairs)
    ajWarn(
	    "warning, depending on the origin of the circular sequence, "
	    "some structures may be missed when using -noLP\n"
	    "Try rotating your sequence a few times\n");

  if (circ)
      min_en = circalifold((const char **)AS, structure);
  else
    min_en = alifold(AS, structure);
  {
    int i;
    double s=0;
    extern int eos_debug;
    eos_debug=-1; /* shut off warnings about nonstandard pairs */
    for (i=0; AS[i]!=NULL; i++)
      if (circ)
	s += energy_of_circ_struct(AS[i], structure);
      else
	s += energy_of_struct(AS[i], structure);
    real_en = s/i;
  }
  string = (mis) ?
    consens_mis((const char **) AS) : consensus((const char **) AS);
  ajFmtPrintF(outf,"%s\n%s", string, structure);

  ajFmtPrintF(outf," (%6.2f = %6.2f + %6.2f) \n", min_en, real_en,
              min_en-real_en );

  if (length<=2500) {
    char **A;
    A = annote(structure, (const char**) AS);
    if (doColor)
      (void) PS_rna_plot_a(string, structure, essfile, A[0], A[1]);
    else
      (void) PS_rna_plot_a(string, structure, essfile, NULL, A[1]);
    free(A[0]); free(A[1]);free(A);
  } else
    ajWarn("INFO: structure too long, not doing xy_plot\n");

  if (doAlnPS)
    PS_color_aln(structure, alifile, AS,  names);

  { /* free mfe arrays but preserve base_pair for PS_dot_plot */
    struct bond  *bp;
    bp = base_pair; base_pair = space(16);
    free_alifold_arrays();  /* free's base_pair */
    base_pair = bp;
  }
  if (pf) {
    double energy, kT;
    pair_info *pi;
    char * mfe_struc;

    mfe_struc = strdup(structure);

    kT = (temperature+273.15)*1.98717/1000.; /* in Kcal */
    pf_scale = exp(-(sfact*min_en)/kT/length);
    if (length>2000)
        ajWarn("scaling factor %f\n", pf_scale);

    /* init_alipf_fold(length); */

    if (confile)
        strncpy(structure, ajStrGetPtr(constring), length+1);
    energy = (circ) ? alipf_circ_fold(AS, structure, &pi) : alipf_fold(AS, structure, &pi);

    if (do_backtrack) {
        ajFmtPrintF(outf,"%s", structure);
        ajFmtPrintF(outf," [%6.2f]\n", energy);
    }
    if ((istty)||(!do_backtrack))
        ajFmtPrintF(outf," free energy of ensemble = %6.2f kcal/mol\n", energy);
    ajFmtPrintF(outf," frequency of mfe structure in ensemble %g\n",
	   exp((energy-min_en)/kT));

    if (do_backtrack) {
      FILE *aliout;
      cpair *cp;
      short *ptable; int k;
	ptable = make_pair_table(mfe_struc);
        ajFmtPrintF(outf,"\n# Alignment section\n\n");
        aliout = ajFileGetFileptr(outf);
        
	fprintf(aliout, "%d sequences; length of alignment %d\n",
		n_seq, length);
	fprintf(aliout, "alifold output\n");
	for (k=0; pi[k].i>0; k++) {
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
  


  if (cstruc!=NULL) free(cstruc);
  free(base_pair);
  (void) fflush(stdout);
  free(string);
  free(structure);
  for (i=0; AS[i]; i++) {
    free(AS[i]); free(names[i]);
  }


    ajStrDel(&constring);
    ajFileClose(&outf);
    ajFileClose(&essfile);
    ajFileClose(&dotfile);

    embExit();  

return 0;
}
  


PRIVATE void mark_endgaps(char *seq, char egap) {
  int i,n;
  n = strlen(seq);
  for (i=0; i<n && (seq[i]=='-'); i++) {
    seq[i] = egap;
  }
  for (i=n-1; i>0 && (seq[i]=='-'); i--) {
    seq[i] = egap;
  }
}

PRIVATE void print_pi(const pair_info pi, FILE *file) {
  const char *pname[8] = {"","CG","GC","GU","UG","AU","UA", "--"};
  int i;

  /* numbering starts with 1 in output */
  fprintf(file, "%5d %5d %2d %5.1f%% %7.3f",
	  pi.i, pi.j, pi.bp[0], 100.*pi.p, pi.ent);
  for (i=1; i<=7; i++)
    if (pi.bp[i]) fprintf(file, " %s:%-4d", pname[i], pi.bp[i]);
  /* if ((!pi.sym)&&(pi.j>=0)) printf(" *"); */
  if (!pi.comp) fprintf(file, " +");
  fprintf(file, "\n");
}

#define MIN2(A, B)      ((A) < (B) ? (A) : (B))

PRIVATE cpair *make_color_pinfo(const pair_info *pi) {
  cpair *cp;
  int i, n;
  for (n=0; pi[n].i>0; n++);
  cp = (cpair *) space(sizeof(cpair)*(n+1));
  for (i=0; i<n; i++) {
    int j, ncomp;
    cp[i].i = pi[i].i;
    cp[i].j = pi[i].j;
    cp[i].p = pi[i].p;
    for (ncomp=0, j=1; j<=6; j++) if (pi[i].bp[j]) ncomp++;
    cp[i].hue = (ncomp-1.0)/6.2;   /* hue<6/6.9 (hue=1 ==  hue=0) */
    if (ncomp==0) cp[i].hue = 0; /* obscure case: only non-standard pairs */
    cp[i].sat = 1 - MIN2( 1.0, pi[i].bp[0]/2.5);
    cp[i].mfe = pi[i].comp;
  }
  return cp;
}

PRIVATE char **annote(const char *structure, const char *AS[]) {
  char *ps, *colorps, **A;
  int i, n, s, pairings, maxl;
  short *ptable;
  char * colorMatrix[6][3] = {
    {"0.0 1", "0.0 0.6",  "0.0 0.2"},  /* red    */
    {"0.16 1","0.16 0.6", "0.16 0.2"}, /* ochre  */
    {"0.32 1","0.32 0.6", "0.32 0.2"}, /* turquoise */
    {"0.48 1","0.48 0.6", "0.48 0.2"}, /* green  */
    {"0.65 1","0.65 0.6", "0.65 0.2"}, /* blue   */
    {"0.81 1","0.81 0.6", "0.81 0.2"} /* violet */
  };

  make_pair_matrix();
  n = strlen(AS[0]);
  maxl = 1024;

  A = (char **) space(sizeof(char *)*2);
  ps = (char *) space(maxl);
  colorps = (char *) space(maxl);
  ptable = make_pair_table(structure);
  for (i=1; i<=n; i++) {
    char pps[64], ci='\0', cj='\0';
    int j, type, pfreq[8] = {0,0,0,0,0,0,0,0}, vi=0, vj=0;
    if ((j=ptable[i])<i) continue;
    for (s=0; AS[s]!=NULL; s++) {
      type = pair[encode_char(AS[s][i-1])][encode_char(AS[s][j-1])];
      pfreq[type]++;
      if (type) {
	if (AS[s][i-1] != ci) { ci = AS[s][i-1]; vi++;}
	if (AS[s][j-1] != cj) { cj = AS[s][j-1]; vj++;}
      }
    }
    for (pairings=0,s=1; s<=7; s++) {
      if (pfreq[s]) pairings++;
    }

    if ((maxl - strlen(ps) < 192) || ((maxl - strlen(colorps)) < 64)) {
      maxl *= 2;
      ps = realloc(ps, maxl);
      colorps = realloc(colorps, maxl);
      if ((ps==NULL) || (colorps == NULL))
	  nrerror("out of memory in realloc");
    }

    if (pfreq[0]<=2) {
      snprintf(pps, 64, "%d %d %s colorpair\n",
	       i,j, colorMatrix[pairings-1][pfreq[0]]);
      strcat(colorps, pps);
    }

    if (pfreq[0]>0) {
      snprintf(pps, 64, "%d %d %d gmark\n", i, j, pfreq[0]);
      strcat(ps, pps);
    }
    if (vi>1) {
      snprintf(pps, 64, "%d cmark\n", i);
      strcat(ps, pps);
    }
    if (vj>1) {
      snprintf(pps, 64, "%d cmark\n", j);
      strcat(ps, pps);
    }
  }
  free(ptable);
  A[0]=colorps;
  A[1]=ps;
  return A;
}

/*-------------------------------------------------------------------------*/
#if 0
PRIVATE void usage(void)
{
  nrerror("usage:\n"
	  "RNAalifold [-cv float] [-nc float] [-E] [-mis] [-circ] [-color] [-aln]\n"
	  "        [-p[0]] [-C] [-T temp] [-4] [-d] [-noGU] [-noCloseGU]\n"
	  "        [-noLP] [-e e_set] [-P paramfile] [-nsp pairs] [-S scale]\n"
	  );
}
#endif
