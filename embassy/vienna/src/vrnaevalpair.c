/* Last changed Time-stamp: <2001-08-23 10:19:23 ivo> */
/*

	  Calculate Energy of given Sequences and Structures
			   c Ivo L Hofacker
			  Vienna RNA Pckage
*/
#include "emboss.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "fold_vars.h"
#include "fold.h"
#include "utils.h"
#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

/*@unused@*/
static char UNUSED rcsid[]="$Id: vrnaevalpair.c,v 1.4 2007/01/31 12:47:07 rice Exp $";

#define  PUBLIC
#define  PRIVATE   static

PRIVATE char *costring(char *string);
PRIVATE char *tokenize(char *line);

extern int logML;
extern int cut_point;
extern int eos_debug;
extern void  read_parameter_file(AjPFile file);

extern AjBool vienna_GetConstraints(AjPFile confile, AjPStr *constring);



int main(int argc, char *argv[])
{
    char *string;
    char  *structure;
    char  fname[12];

    int   l;
    int   length1;
    int   length2;
    float energy;
    int   istty;
    int   noconv=0;


    AjPSeq seq1 = NULL;
    AjPSeq seq2 = NULL;

    AjPFile outf = NULL;
    AjPFile paramfile = NULL;
    AjPFile confile1 = NULL;
    AjPFile confile2 = NULL;


    float eT = 0.;
    AjBool convert;
    AjBool etloop;
    AjPStr *eenergy = NULL;
    char ewt = '\0';
    AjPStr *edangles = NULL;
    char edangle = '\0';
    AjBool logml;
   
    AjPStr seqstring1 = NULL;
    AjPStr constring1 = NULL;
    AjPStr constring2 = NULL;
    

    embInitP("vrnaevalpair",argc,argv,"VIENNA");


    string    = NULL;
    structure = NULL;
    istty = 0;
    

    seqstring1 = ajStrNew();
    constring1 = ajStrNew();
    constring2 = ajStrNew();

    seq1      = ajAcdGetSeq("asequence");
    seq2      = ajAcdGetSeq("bsequence");

    confile1  = ajAcdGetInfile("aconstraintfile");
    confile2  = ajAcdGetInfile("bconstraintfile");
    paramfile = ajAcdGetInfile("paramfile");

    eT        = ajAcdGetFloat("temperature");

    convert   = ajAcdGetBool("convert");

    etloop    = ajAcdGetBool("tetraloop");
    eenergy   = ajAcdGetList("energy");

    edangles  = ajAcdGetList("dangles");
    logml     = ajAcdGetBool("logml");
    outf      = ajAcdGetOutfile("outfile");

    temperature   = (double) eT;
    noconv        = (convert) ? 0 : 1;
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

    logML = !!logml;
   

    if(paramfile)
	read_parameter_file(paramfile);

    update_fold_params();


    ajFmtPrintS(&seqstring1,"%s&%s",ajSeqGetSeqC(seq1),ajSeqGetSeqC(seq2));

    vienna_GetConstraints(confile1,&constring1);
    vienna_GetConstraints(confile2,&constring2);
    ajStrAppendK(&constring1,'&');
    ajStrAppendS(&constring1,constring2);
    

    cut_point = -1;

    strcpy(fname,ajSeqGetNameC(seq1));


    string = tokenize(MAJSTRGETPTR(seqstring1));
    length2 = (int) strlen(string);
      
    structure = tokenize(MAJSTRGETPTR(constring1));
    length1 = (int) strlen(structure);
      
    if(length1!=length2)
	ajFatal("Sequence and Structure have unequal length.");

    for(l = 0; l < length1; l++)
    {
        string[l] = toupper((int)string[l]);
        if (!noconv && string[l] == 'T')
	    string[l] = 'U';
    }

      
    energy = energy_of_struct(string, structure);
    if(cut_point == -1)
	ajFmtPrintF(outf,"%s\n%s", string, structure);
    else
    {
	char *pstring, *pstruct;
	pstring = costring(string);
	pstruct = costring(structure);
	ajFmtPrintF(outf,"%s\n%s", pstring,  pstruct);
	free(pstring);
	free(pstruct);
    }

    ajFmtPrintF(outf," (%6.2f)\n", energy);
      
    free(string);
    free(structure);

    ajStrDel(&seqstring1);
    ajStrDel(&constring1);
    ajStrDel(&constring2);
    ajSeqDel(&seq1);
    ajSeqDel(&seq2);
    
    ajFileClose(&outf);

    ajExit();
    

    return 0;
}




PRIVATE char *tokenize(char *line)
{
    char *token, *copy, *ctmp;
    int cut = -1;

    copy = (char *) space(strlen(line)+1);
    ctmp = (char *) space(strlen(line)+1);
    (void) sscanf(line, "%s", copy);
    ctmp[0] = '\0';
    token = strtok(copy, "&");
    cut = strlen(token)+1;
    while (token)
    {
	strcat(ctmp, token);
	token = strtok(NULL, "&");
    }

    if (cut > strlen(ctmp))
	cut = -1;

    if (cut > -1)
    {
	if (cut_point==-1)
	    cut_point = cut;
	else if (cut_point != cut)
	{
	    fprintf(stderr,"cut_point = %d cut = %d\n", cut_point, cut);
	    ajFatal("Sequence and Structure have different cut points.");
	}
    }
    free(copy);
  
    return ctmp;
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
