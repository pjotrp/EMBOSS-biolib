/*
  Plot RNA structures using different layout algorithms
  Last changed Time-stamp: <2003-09-10 13:55:01 ivo> 
*/

#include "emboss.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "PS_dot.h"

#define PRIVATE static



/*--------------------------------------------------------------------------*/

extern int svg_rna_plot(char *string, char *structure, AjPFile outf);

int main(int argc, char *argv[])
{
   
    char *string=NULL;
    char *structure=NULL, *pre=NULL, *post=NULL;
    float energy;
    int   istty;
    char  format[5]="ps";
    AjPFile inf = NULL;
    AjPFile outf = NULL;
    AjPStr layout = NULL;
    AjPStr optype = NULL;
    AjPStr epre = NULL;
    AjPStr epost = NULL;
    AjPStr eline = NULL;
   

    embInitP("vrnaplot",argc,argv,"VIENNA");


    inf    = ajAcdGetInfile("structuresfile");
    layout = ajAcdGetListSingle("layout");
    optype = ajAcdGetListSingle("optype");
    epre   = ajAcdGetString("pre");
    epost  = ajAcdGetString("post");
    outf   = ajAcdGetOutfile("outfile");
   
    if(ajStrMatchC(layout,"radial"))
	rna_plot_type = 0;
    else if (ajStrMatchC(layout,"naview"))
	rna_plot_type = 1;
   
    strcpy(format,ajStrGetPtr(optype));
   
    if(ajStrGetLen(epre))
	pre = MAJSTRGETPTR(epre);

    if(ajStrGetLen(epost))
	post = MAJSTRGETPTR(epost);
   
    istty = 0;

    eline = ajStrNew();

    if(!ajReadlineTrim(inf,&eline))
	ajFatal("Empty input file\n");

    while(!ajStrGetLen(eline))
    {
	if(!ajReadlineTrim(inf,&eline))
	    ajFatal("Empty input file\n");
    }
    if(*ajStrGetPtr(eline) == '>')
	if(!ajReadlineTrim(inf,&eline))
	    ajFatal("Missing sequence line\n");

    string = (char *) space(ajStrGetLen(eline) + 1);
    sscanf(ajStrGetPtr(eline),"%s",string);

    if(!ajReadlineTrim(inf,&eline))
	ajFatal("Missing structure line\n");

    structure = (char *) space(ajStrGetLen(eline) + 1);
    sscanf(ajStrGetPtr(eline),"%s (%f)", structure, &energy);

    if (strlen(string)!=strlen(structure)) 
	ajFatal("Sequence and structure have unequal length\n");


    switch (format[0])
    {
    case 'p':
	PS_rna_plot_a(string, structure, outf, pre, post);
	break;
    case 'g':
	gmlRNA(string, structure, outf, 'x');
	break;
    case 'x':
	xrna_plot(string, structure, outf);
	break;
    case 's':
	svg_rna_plot(string, structure, outf);
	break;
    default:
	break;
    }

    free(string);
    free(structure); 

    ajStrDel(&eline);
    ajStrDel(&epre);
    ajStrDel(&epost);
    ajStrDel(&layout);
    ajStrDel(&optype);
    ajFileClose(&inf);
    ajFileClose(&outf);
    
    embExit();
    return 0;
}
