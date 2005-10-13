/*
  Plot RNA structures using different layout algorithms
  Last changed Time-stamp: <2003-09-10 13:55:01 ivo> 
*/

#include "ajax.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "ePS_dot.h"

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
    AjPStr *layout = NULL;
    AjPStr *optype = NULL;
    AjPStr epre = NULL;
    AjPStr epost = NULL;
    AjPStr eline = NULL;
   

    ajNamInit("emboss");
    ajAcdInitP("vrnaplot",argc,argv,"VIENNA");


    inf    = ajAcdGetInfile("structure");
    layout = ajAcdGetList("layout");
    optype = ajAcdGetList("optype");
    epre   = ajAcdGetString("pre");
    epost  = ajAcdGetString("post");
    outf   = ajAcdGetOutfile("outfile");
   
    if(ajStrMatchC(layout[0],"radial"))
	rna_plot_type = 0;
    else if (ajStrMatchC(layout[0],"naview"))
	rna_plot_type = 1;
   
    strcpy(format,ajStrStr(optype[0]));
   
    if(ajStrLen(epre))
	pre = MAJSTRSTR(epre);

    if(ajStrLen(epost))
	post = MAJSTRSTR(epost);
   
    istty = 0;

    eline = ajStrNew();

    if(!ajFileReadLine(inf,&eline))
	ajFatal("Empty input file\n");

    while(!ajStrLen(eline))
    {
	if(!ajFileReadLine(inf,&eline))
	    ajFatal("Empty input file\n");
    }
    if(*ajStrStr(eline) == '>')
	if(!ajFileReadLine(inf,&eline))
	    ajFatal("Missing sequence line\n");

    string = (char *) space(ajStrLen(eline) + 1);
    sscanf(ajStrStr(eline),"%s",string);

    if(!ajFileReadLine(inf,&eline))
	ajFatal("Missing structure line\n");

    structure = (char *) space(ajStrLen(eline) + 1);
    sscanf(ajStrStr(eline),"%s (%f)", structure, &energy);

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
   

    return 0;
}
