/*  Last edited: May 16 09:33 2000 (pmr) */
/*
Example GIVEN.

 ./topo sw:aqp1_human -signa -prol=UM -cys=cg -gly=sr -acid=dr -basic=db -ohaa=da -araa=dg
Draws an image of a transmembrane protein
Display on device [xwin]: 
Transmembrane Sections: 18 35 49 67 94 115 136 156 165 184 211 232
First membrane from the outside [Y]: n
Signature Sections: 74 82 sy 189 189 hm 239 239 sa 157 157 sa 262 262 sa
*/

#include "emboss.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h" 
#include "ajgraph.h"

void  truncdown(const int num,const int istart,const int iend,
		float *off,const float wide,char *strand,int *syms);
void  truncup(const int num,const int istart,const int iend,
	      float *off,const float wide,char *strand,int *syms)  ;
/*void  selectsym(int ndev,int symx);*/
void  checkcys(AjBool outstart,const int symcys,const int tmcount,
	       const int length,ajuint *istart,
	       ajuint *iend,char *strand,int *syms) ;
void  checklca(const int symlcharge,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms);
void  checklcb(const int symlcharge,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms);
void  checkla(const int symlcharge,const int tmcount,
	      ajuint *istart,ajuint *iend,char *strand,int *syms);
void  checkloh(const int symloh,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms);
void  checklar(const int symlar,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms);
void  checklpro(const int symlpro,const int tmcount,
		ajuint *istart,ajuint *iend,char *strand,int *syms);
void  checkgly(AjBool outstart,const int symgly,const int tmcount,int length,
	       ajuint *istart,ajuint *iend,char *strand,int *syms);
void  topoMembrane(void);
void  topoMembrane2(void);
void  topoMembrane3(void);
void  topoSymbol(const float x,float y,char stran,int sym);
void  starttop7(AjPGraph graph, const int tmcount,const int istart,float *off,
		char *strand,int *syms);
void  starttop7full(AjPGraph graph, const int tmcount,
		    const int istart,char *strand,int *syms);
void  endtop7(AjPGraph graph, const int tmcount,
	      const int iend,const int length,
	      float *off,char *strand,int *syms)  ;
void  endtop7full(AjPGraph graph, const int tmcount,
		  const int iend,const int length,
		  char *strand,int *syms) ;
void  topoTerm(const float x,const float y);
void  startbot7(AjPGraph graph, const int tmcount,
		const int istart,float *off,
		char *strand,int *syms);
void  startbot7full(AjPGraph graph, const int tmcount,
		    int istart,char *strand,int *syms)  ;
void  endbot7(AjPGraph graph, const int tmcount,
	      const int iend,const int length,
	      float *off,char *strand,int *syms);
void  endbot7full(AjPGraph graph, const int tmcount,
		  const int ie,const int length,
		  char *strand,int *syms);
void  chaindown(const int i,const int istart,const int nchain,
		float *off,const int page,const float wide,
		float *hold,char *strand,int *syms);
void  chain0down(const int page,const int i,const int istart,
		 const int nchain,float *off,float *hold,
		 const float wide,char *strand,int *syms);
void  sheetup(const int page,const int i,const int istart,
	      const int nchain,float *off,float *hold,
	      const float wide,char *strand,int *syms);
void  chain0up(const int page,const int i,const int istart,
	       const int nchain,float *off,float *hold,
		       const float wide,char *strand,int *syms);
void  chain1down(const int page,const int i,const int istart,
		 const int nchain,float *off,float *hold,
		 const float wide,char *strand,int *syms);
void  chain1up(const int page,const int i,const int istart,
	       const int nchain,float *off,float *hold,
	       const float wide,char *strand,int *syms);
void  chain2down(const int page,const int i,const int istart,
		 const int nchain,float *off,float *hold,
		 const float wide,char *strand,int *syms);
void  chainup(const int i,const int istart,const int nchain,
	      float *off,const int page,const float wide,
	      float *hold,char *strand,int *syms);
void  chain2up(const int page,const int i,const int istart,
	       const int nchain,float *off,float *hold,
	       const float wide,char *strand,int *syms) ;
void  chain3down(const int page,const int i,const int istart,
		 const int nchain,float *off,
		 float *hold,const float wide,char *strand,int *syms) ;
void  chain3up(const int page,const int i,const int istart,
	       const int nchain,float *off,
	       float *hold,const float wide,char *strand,int *syms) ;
void  ball1s(const int istart,const int j,const float x,
	     char *strand,int *syms);
void  ball1su(const int istart,const int j,const float x,
	      char *strand,int *syms) ;
void  ball1u(const int istart,const int j,const float x,
	     char *strand,int *syms) ;
void  ball2topdown(const int istart,const int j,const float x,
		   char *strand,int *syms)  ;
void  ball2topup(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball2botdown(const int istart,const int j,const float x,
		   char *strand,int *syms) ;
void  ball2botup(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball1(const int istart,const int j,const float x,
	    char *strand,int *syms) ;
void  ball1down7(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball1up7(const int istart,const int j,const float x,
	       char *strand,int *syms)  ;
void  ball1down(const int istart,const int j,const float x,
		char *strand,int *syms)  ;
void  ball1up(const int istart,const int j,const float x,
	      char *strand,int *syms)  ;
void  ball1bdown(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball1bup(const int istart,const int j,const float x,
	       char *strand,int *syms)  ;
void  ball2sdown(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball2sup(const int istart,const int j,const float x,
	       char *strand,int *syms)  ;
void  ball2down(const int istart,const int j,const float x,
		char *strand,int *syms)  ;
void  ball2up(const int istart,const int j,const float x,
	      char *strand,int *syms)  ;
void  ball2adown(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball2aup(const int istart,const int j,const float x,
	       char *strand,int *syms)  ;
void  ball3sdown(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball3sup(const int istart,const int j,const float x,
	       char *strand,int *syms)  ;
void  ball3down(const int istart,const int j,const float x,
		char *strand,int *syms)  ;
void  ball3up(const int istart,const int j,const float x,
	      char *strand,int *syms)  ;
void  ball3down2(const int istart,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball3up2(const int istart,const int j,const float x,
	       char *strand,int *syms)  ;
void  ball4up(const int istart,const int j,const float x,
	      char *strand,int *syms)  ;
void  ball4down(const int i,const int j,const float x,
		char *strand,int *syms)  ;
void  ball4bdown(const int i,const int j,const float x,
		 char *strand,int *syms)  ;
void  ball4bup(const int i,const int j,const float x,
	       char *strand,int *syms)  ;
void  ball5down(const int i,const int j,const float x,
		char *strand,int *syms)  ;
void  ball5up(const int i,const int j,const float x,
	      char *strand,int *syms)  ;
void  conup(float *hold,const int i,const ajuint is,ajuint *ie,
	    char *strand,int *syms)  ;
void  condown(float *hold1,const int i,const ajuint is,ajuint *ie,
	      char *strand,int *syms) ;
void  condownsmall(const int is,int *ie,char *strand,int *syms)  ;
void  conupbig2(const int is,int *ie,char *strand,int *syms)  ;
void  condownbig2(const int is,int *ie,char *strand,int *syms)  ;
void  condownbig3(const int is,int *ie,char *strand,int *syms)  ;
void  sheetdown(const int page,const int i,const int istart,
		const int nchain,float *off,
		float *hold,const float wide,char *strand,int *syms) ;
int getValFromStr(AjPStr str);
void topoMoveTo(const float x, float y);
void topoDraw(const float x, float y);
void topoCentre(const float x, float y);
void topoCurve(float rad, float junk, float junk2);
void topoPlotText(char *text);
void topoNewColour(int col);
 


float tempxpos=0.0;
float tempypos=0.0;
float circle1=0.0;
float circle2=0.0;
AjPGraph graph; /* change to ajPGraph later*/

int getValFromStr(AjPStr str)
{
    static AjPStr tmpstr = NULL;
    const char *colstr;
    int symval;
    int shape = 0;
    int col = 0;

    tmpstr = ajStrNewS(str);
    ajStrFmtUpper(&tmpstr);
    colstr = ajStrGetPtr(tmpstr);


    if(strstr(colstr,"N")!=NULL)          /* NONE overrides everything else */
	return 0;
    if(strstr(colstr,"C")!=NULL)
	shape = 1;
    else if(strstr(colstr,"S")!=NULL)
	shape = 2;
    else if(strstr(colstr,"H")!=NULL)
	shape = 3;
    else if(strstr(colstr,"U")!=NULL)
	shape = 4;
    else if(strstr(colstr,"D")!=NULL)
	shape = 5;

    if(!shape)                     /* if no shape return */
    {
	ajStrDel(&tmpstr);
	return 0;
    }

    if(strstr(colstr,"G")!=NULL)
	col = 1;
    else if(strstr(colstr,"R")!=NULL)
	col = 2;
    else if(strstr(colstr,"A")!=NULL)
	col = 4;
    else if(strstr(colstr,"B")!=NULL)   
	col = 3;
    else if(strstr(colstr,"M")!=NULL)
	col = 5;
    else if(strstr(colstr,"Y")!=NULL)
	col = 6;
    else if(strstr(colstr,"F")!=NULL)    /* NO FILL */
    {
	ajStrDel(&tmpstr);
	return shape;
    }

    symval = ((shape*8)-2)+col;

    ajStrDel(&tmpstr);
    return symval;
}


void topoMoveTo(const float x, float y)
{
    tempxpos=x;
    tempypos=y;
}

void topoDraw(const float x, float y)
{  
    ajGraphicsDrawposLine(tempxpos,tempypos,x,y);
    tempxpos=x;
    tempypos=y;
}

void topoCentre(const float x, float y)
{
    circle1 = x;
    circle2 = y;
}

void topoCurve(float rad, float junk, float junk2)
{
    ajGraphicsDrawposCircle(circle1,circle2,rad);
}

void topoPlotText(char *text)
{
    ajGraphicsDrawposTextAtstart(tempxpos-0.4,tempypos,text);
}

void topoNewColour(int col)
{
    int old;
    if(col == 1)       /*black */
	col =0;
    else if(col == 2)  /* green */
	col = 3;
    else if (col == 4)  /* red */
	col = 1;
    else if (col == 3 )  /* blue */
	col = 9;
    else if (col == 5)  /* aqua */
	col = 4;
    else if (col == 6)   /* magenta */
	col = 13;
    else if (col == 7)   /* yellow */
	col = 2;

    old = ajGraphicsSetFgcolour(col);
}

int main(int argc, char * argv[])
{
    /*c topographical model program */ 
    /*c this program is for the drawing of topographical */ 
    /*c maps of membrane spanning sequences */ 
    /*c by sj2 10/94 */ 
 
 
    topoSequence sq;
 
    char  *strand = NULL;	/*  */
    int   i,tmcount,tmcount2,nsecs,nsece,nchain,length;
    int   j,iex,iex2;
    int   gap,page,pages,stsp;
    ajuint   *istart = NULL;
    ajuint   *iend = NULL;
    int   *syms = NULL;
    int   gaps ;
    int   symcys,symgly,symlchargea,symlchargeb ;
    int   symlamine,symloh,symlar,symlpro,symsign ;
    int   endsp = 0;
    float off=0.0,wide=0.0,hold=0.0,off0=0.0,offi = 0.0;
    /*        int   sqread ;*/
    /* new */
    int ii;
    int minstart;
    int minend;
    int maxtm;

    /* acd defs */
    AjPSeq sequence;
    AjPRange regions;
    AjPRange sigregions;
    AjBool draft ;
    AjBool outstart;
    AjBool ajspnum;
    AjPStr listcys;
    AjPStr listgly;
    AjPStr listacid;
    AjPStr listbase;
    AjPStr listamine;
    AjPStr listoh;
    AjPStr listaro;
    AjPStr listpro;
    AjBool doend = ajTrue;
    ajuint isigstart, isigend;

    embInitPV("topo", argc, argv, "TOPO", VERSION);
  
    sequence = ajAcdGetSeq("sequence");
    graph  = ajAcdGetGraph("graph");

    ajGraphAppendTitleS(graph, ajSeqGetUsaS(sequence));

    ajGraphOpenWin(graph,0.0,150.0,0.0,100.0);
    ajGraphicsSetCharscale(0.5);

    sq.seq[0] = ' '; /* hopefully sort out the fortran to c offset worrys :) */

    strcpy(&sq.seq[1],ajSeqGetSeqC(sequence));
    length = strlen(sq.seq);

    if(sq.seq[length] == '*')
	length=length-2 ;
    else 
	length=length-1 ;

    strand = (char*) AJALLOC((length+1)*sizeof(char)); 
    syms = (int *) AJALLOC((length+1)*sizeof(int)); 

    for(i=1;i<=length ;i++)
    {
	strand[i]=sq.seq[i] ;
    }  /*end for*/
    /*c create default symbols for the sequence */ 
    for(i=1;i<=length ;i++)
    {
	syms[i]=1 ;
    }  /*end for*/
  
    /*c section to ask about the number of transmembrane sections wanted */ 
    /*c and how long they are */ 
    tmcount=0 ;
    nsecs=0 ;
    nsece=0 ;

    regions = ajAcdGetRange ("sections");
    tmcount = ajRangeGetSize(regions);
    istart = (ajuint *) AJALLOC((tmcount+1)*sizeof(ajuint)); 
    iend = (ajuint *) AJALLOC((tmcount+1)*sizeof(ajuint));  
    ajRangeElementGetValues(regions,tmcount,
                            &istart[0],&iend[0]); /* again offset by 1*/
    for(i=0;i<=tmcount;i++){
	ajRangeElementGetValues(regions,i,
                                &istart[i+1],&iend[i+1]); /* ... offset by 1 */
    }
  
    outstart = ajAcdGetBoolean("membrane");
  
    wide=16.8 ;

    off0=24.6 ;
    offi = 83.4 - wide/2.0*tmcount;
    minstart = 117 - 9*tmcount;
    tmcount2 = tmcount;
    if(tmcount2 > 7)
	tmcount2 -= 7;
    minend = 117 - 9*tmcount2;
    if (offi < off0)
	offi = off0;
    if(tmcount > 5)
	minstart -= 9;
    if(tmcount > 6)
	minstart -= 9;
    if(tmcount2 > 5)
	minend -= 9;
    if(tmcount2 > 6)
	minend -= 9;

    if(minstart < 36)
	minstart = 36;
    off = offi;

    /* symbols. 1 is "no symbol". All use the same list in the ACD file. */

    /*c get external cys instructions */ 
    listcys = ajAcdGetListSingle("cyssymbol");
    symcys = getValFromStr(listcys);
    if(symcys != 1)
	(void) checkcys(outstart,symcys,tmcount,length,
			istart,iend,strand,syms) ;
  
    /*c ask about glycosylation sites */ 
    listgly = ajAcdGetListSingle("glysymbol");
    symgly = getValFromStr(listgly);
    if(symgly != 1)
	(void) checkgly(outstart,symgly,tmcount,length,
			istart,iend,strand,syms) ;

    /*c get special information about the membrane portion of the drawing */ 

    listacid = ajAcdGetListSingle("acidicaa");
    symlchargea = getValFromStr(listacid);
    if(symlchargea != 1)
	(void) checklca(symlchargea,tmcount,istart,iend,strand,syms) ;

    listbase = ajAcdGetListSingle("basicaa");
    symlchargeb = getValFromStr(listbase);
    if(symlchargeb != 1)
	(void) checklcb(symlchargeb,tmcount,istart,iend,strand,syms) ;

    /*c get special informatio about the membrane portion of the drawing */ 
    /*c ask about amine amino acids within the membrane */ 

    listamine = ajAcdGetListSingle("amineaa");
    symlamine = getValFromStr(listamine);
    if(symlamine != 1)
	(void) checkla(symlamine,tmcount,istart,iend,strand,syms) ;
  
    listoh = ajAcdGetListSingle("ohaa");
    symloh = getValFromStr(listoh);
    if(symloh != 1)
	(void) checkloh(symloh,tmcount,istart,iend,strand,syms) ;
  
    listaro = ajAcdGetListSingle("araa");
    symlar = getValFromStr(listaro);
    if(symlar != 1)
	(void) checklar(symlar,tmcount,istart,iend,strand,syms) ;

    listpro = ajAcdGetListSingle("proline");
    symlpro = getValFromStr(listpro);
    if(symlpro != 1)
	(void) checklpro(symlpro,tmcount,istart,iend,strand,syms) ;
  /*c ask about aromatic amino acids  */ 

    ajspnum = ajAcdGetToggle("signature");
    if(ajspnum){
	/* num num code sets */
	sigregions = ajAcdGetRange ("sigrange");
	ii = ajRangeGetSize(sigregions);
	for(i=0;i<ii;i++){
	    AjPStr val = NULL;
	    ajRangeElementGetValues(sigregions,i,&isigstart,&isigend);
	    if(ajRangeElementGetText(sigregions,i,&val)){
		symsign = getValFromStr(val);
	    }
	    else{
		symsign = 1;
	    }
	    for(j=isigstart;j<=isigend ;j++)
	    {
		syms[j]=symsign ;
	    }  /*end for*/
	    ajStrDel(&val);
	}
    }
    
/*c determine if this is a draft plot */ 
    draft = ajAcdGetBoolean("draft");

    /*c determine the number of possible pages this will take */ 
    pages=1 ;
    page=1 ;
  
    /*c section to set the parameters for sets of transmembrane sections */ 
    if(!draft)
    {
  	/*c see if there needs to be a special starting page */ 
	stsp=istart[1]-1 ;

	if(stsp > minstart)
	{
	    if(outstart)
		starttop7full(graph, tmcount,istart[1],strand,syms) ;
	    else
		startbot7full(graph, tmcount,istart[1],strand,syms) ;
	    off = offi;
	    ++pages;
	}
  
	/*c see if there needs to be a special ending page */ 
	endsp=length-iend[tmcount]-1 ;

	if(endsp > minend)
	{
	    if(outstart)
	    {
		if(tmcount%2)
		    endbot7full(graph, tmcount,
				iend[tmcount],length,strand,syms) ;
		else
		    endtop7full(graph, tmcount,
				iend[tmcount],length,strand,syms);
	    }
	    else
	    {
		if(tmcount%2)
		    endtop7full(graph, tmcount,
				iend[tmcount],length,strand,syms);
		else
		    endbot7full(graph, tmcount,
				iend[tmcount],length,strand,syms) ;
	    }
	    ++pages;
	}

	/*c check for extra gapping pages */ 

	for(i=1;i<tmcount ;i++)
	{
	    gap=istart[i+1]-iend[i]-1 ;
	    iex=iend[i] ;

	    if(outstart)
	    {
		if(i%2)				/* odd number */
		{
		    if(gap > 74)
		    {
			conupbig2(istart[i+1],&iex,strand,syms);
			++pages;
		    }
		}
		else				/* even number */
		{
		    if(gap > 49)
		    {
			if(gap <= 74)
			{
			    condownsmall(istart[i+1],&iex,strand,syms);
			    ++pages;
			}
			else if (74 < gap && gap < 314)
			{
			    condownbig2(istart[i+1],&iex,strand,syms);
			    ++pages;
			}
			else if (gap >= 314)
			{
			    condownbig2(istart[i+1],&iex,strand,syms);
			    iex2=iend[i];
			    condownbig3(istart[i+1],&iex2,strand,syms);
			    pages += 2;
			}
		    }
		}
	    }
	    else
	    {
		if(i%2)
		{
		    if(gap > 49)
		    {
			if(gap <= 74)
			{
			    condownsmall(istart[i+1],&iex,strand,syms);
			    ++pages;
			}
			else if (74 < gap && gap < 314)
			{
			    condownbig2(istart[i+1],&iex,strand,syms);
			    ++pages;
			}
			else if (gap >= 314)
			{
			    condownbig2(istart[i+1],&iex,strand,syms);
			    iex2=iend[i];
			    condownbig3(istart[i+1],&iex2,strand,syms);
			    pages += 2;
			}
		    }
		}
		else
		{
		    if(gap > 74)
		    {
			conupbig2(istart[i+1],&iex,strand,syms);
			++pages;
		    }
		}
	    }
	}
    }

    /* non-draft-specific stuff done */

    /*c do the regular plot */ 

    /*c start out the plot by putting in the membrane lines */ 

    topoMembrane();
    
    if(outstart)
    {
	if(draft)
	{
	    starttop7(graph, tmcount,istart[1],&off,strand,syms) ;
	}
	else
	{
	    stsp=istart[1]-1;
	    if(stsp > minstart)
		off=offi;
	    else
		starttop7(graph, tmcount,istart[1],&off,strand,syms);
	}
    }
    else
    {
	if(draft)
	{
	    startbot7(graph, tmcount,istart[1],&off,strand,syms) ;
	}
	else
	{
	    stsp=istart[1]-1 ;
	    if(stsp > minstart)
		off=offi;
	    else
		startbot7(graph, tmcount,istart[1],&off,strand,syms) ;
	}
    }
  
  /*c cycle through the chains and put in the data */ 
    nchain=0 ;
    i=0 ;
    maxtm = 7;
    for(i=1;i<=tmcount ;i++)
    {
	nchain=iend[i]-istart[i]+1 ;
	if(outstart)
	{
	    if(i%2)
	    {
		chaindown(i,istart[i],nchain,&off,page,wide, 
			  &hold,strand,syms) ;
	    }
	    else
	    {
		chainup(i,istart[i],nchain,&off,page,wide, 
			&hold,strand,syms) ;
	    }
	}
	else
	{
	    if(i%2)
	    {
		chainup(i,istart[i],nchain,&off,page,wide, 
			&hold,strand,syms) ;
	    }
	    else
	    {
		chaindown(i,istart[i],nchain,&off,page,wide, 
			  &hold,strand,syms) ;
	    }
	}

	/* new page for 7th TM */

	if (tmcount > maxtm && i == maxtm)
	{
	    maxtm += 7;
	    /*c finish off the first page and get ready
	      for the second */ 
	    /*c	      call closef(fn) */ 
	    /*c start up the second page */ 
	    ajGraphNewpage(graph,AJFALSE);
	    page = 2;
	    /*c start a new membrane */ 
	    topoMembrane();
	    hold = off0-wide;
	}

	if(outstart)
	{
	    if(i%2)
	    {
		if(tmcount > i)
		{
		    j=i+1 ;
		    gaps=istart[j]-iend[i]-1 ;
		    if(draft)
		    {
			if(gaps >= 75)
			    truncup(j,istart[j],iend[i],&off,wide, 
				    strand,syms);
			else if (gaps <= 74)
			    conup(&hold,j,istart[j],&iend[i], 
				  strand,syms);
		    }
		    else
		    {
			if(gaps <= 74)
			    conup(&hold,j,istart[j],&iend[i],strand, 
				  syms);
		    }  /*end if*/
		}  /*end if*/
	    }
	    else
	    {
		if(tmcount > i)
		{
		    j=i+1 ;
		    gaps=istart[j]-iend[i]-1 ;
		    if(gaps > 49 && draft)
		    {
			truncdown(j,istart[j],iend[i],&off,wide, 
				  strand,syms) ;
		    }
		    else if (gaps > 49 && !draft)
		    {
			continue;
		    }
		    else
		    {
			condown(&hold,j,istart[j],&iend[i], 
				strand,syms) ;
		    }   /*end if*/
		}  /*end if*/
	    }
	}
	else
	{
	    if(i%2)
	    {
		if(tmcount > i)
		{
		    j=i+1 ;
		    gaps=istart[j]-iend[i]-1 ;
		    if(gaps > 49 && draft)
		    {
			truncdown(j,istart[j],iend[i],&off,wide, 
				  strand,syms);
			hold=off+wide;
		    }
		    else if (gaps > 49 && !draft)
		    {
			continue;
		    }
		    else
		    {
			condown(&hold,j,istart[j],&iend[i], 
				strand,syms);
		    }  /*end if*/
		}  /*end if*/
	    }
	    else
	    {
		if(tmcount > i)
		{
		    j=i+1 ;
		    gaps=istart[j]-iend[i]-1 ;
		    if(draft)
		    {
			if(gaps >= 75)
			{
			    truncup(j,istart[j],iend[i],&off,wide, 
				    strand,syms) ;
			}
			else if (gaps <= 74)
			{
			    conup(&hold,j,istart[j],&iend[i], 
				  strand,syms) ;
			}  /*end if*/
		    }
		    else
		    {
			if(gaps <= 74)
			{
			    conup(&hold,j,istart[j],&iend[i],strand, 
				  syms) ;
			}  /*end if*/
		    }  /*end if*/
		}
	    }
	}
    }



  
  /*c check for ending requirements */ 
    if (!draft)
    {
	endsp=length-iend[tmcount]-1 ;
	if(endsp > minend)
	{
	    doend = ajFalse;;
	}
    }
    if(doend)
    {
	if(outstart)
	{
	    if(tmcount%2)
		endbot7(graph, tmcount,iend[tmcount],length,&off,strand,syms) ;
	    else
		endtop7(graph, tmcount,iend[tmcount],length,&off,strand,syms) ;
	}

	else
	{
	    if(tmcount%2)
		endtop7(graph, tmcount,iend[tmcount],length,&off,strand,syms) ;
	    else
		endbot7(graph, tmcount,iend[tmcount],length,&off,strand,syms) ;
	}
    }

    ajGraphicsCloseWin() ;

    ajGraphxyDel(&graph);
    AJFREE(istart);
    AJFREE(iend);
    ajSeqDel(&sequence);
    ajRangeDel(&regions);
    if (ajspnum)
      ajRangeDel(&sigregions);
    ajStrDel(&listcys);
    ajStrDel(&listgly);
    ajStrDel(&listacid);
    ajStrDel(&listbase);
    ajStrDel(&listamine);
    ajStrDel(&listoh);
    ajStrDel(&listaro);
    ajStrDel(&listpro);
    AJFREE(strand);
    AJFREE(syms);

    embExit();
    return 0;
}	/*end ;*/

/* @func truncdown ************************************************************
**
******************************************************************************/

void  truncdown(const int num,const int istart,const int iend,
		float *off,const float wide,char *strand,int *syms)
{
    float  x,y ;
    int   j,sym ;
    int   gap ;
    char stran,string[39] ;
    int jnum;

    jnum = num-2;
    x=*off+(jnum%7)*wide;
    for(j=1;j<=10 ;j++)
    {
	y=71.6+2.8*(j-1) ;
	stran=strand[iend+j] ;
	sym=syms[iend+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    gap=istart-iend-1 ;
    /*  (void) inttostr(gap,string) ;*/
    sprintf(string,"%d",gap);

    topoMoveTo(x+8.4,y) ;
    topoPlotText("loop of") ;

    topoMoveTo(x+8.4,y-5.0) ;
    topoPlotText(string) ;

    x += wide;
    for(j=1;j<=10 ;j++)
    {
	y=71.6+2.8*(10-j) ;
	stran=strand[istart-11+j] ;
	sym=syms[istart-11+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    return ;
}
  
/* @func truncup **************************************************************
**
******************************************************************************/

void  truncup(const int num,const int istart,const int iend,
	      float *off,const float wide,char *strand,int *syms)
{
    float x,y ;
    int   j,sym ;
    int   gap ;
    char stran,string[39] ;
    int jnum;

    jnum = num-2;
    x=*off+(jnum%7)*wide;
    
    for(j=1;j<=10 ;j++)
    {
	y=37.2-2.8*(j-1) ;
	stran=strand[iend+j] ;
	sym=syms[iend+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    gap=istart-iend-1 ;
    /*    (void) inttostr(gap,string) ;*/
    sprintf(string,"%d",gap);
    topoMoveTo(x+8.4,y+5.0) ;
    topoPlotText("loop of") ;
    topoMoveTo(x+8.4,y) ;
    topoPlotText(string) ;

    x += wide;

    for(j=1;j<=10 ;j++)
    {
	y=37.2-2.8*(10-j) ;
	stran=strand[istart-11+j] ;
	sym=syms[istart-11+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    return ;
}

 
/* @func checkcys *************************************************************
**
******************************************************************************/

void  checkcys(AjBool outstart,const int symcys,const int tmcount,
	       const int length,ajuint *istart,ajuint *iend,
	       char *strand,int *syms)
{
 
 
/*#include "common.inc" */ 
 
  int    i,k,count,pass = 0;
  float       rcount ;
 
/*c determine the number of passes */ 
  count=tmcount/2 ;
  rcount=(float)(tmcount)/2.0 ;
  if(!outstart)
{
    if(count == rcount)pass=count ;
  }
  else
{
    if(count == rcount)pass=count+1 ;
  }  /*end if*/
  if(count < rcount)pass=count+1 ;
  
  for(k=1;k<=pass	 ;k++)
{
    if(outstart && k == 1)
{
      for(i=1;i<=istart[1]-1 ;i++)
{
	if(strand[i] == 'c' || strand[i] == 'C')syms[i]=symcys ;
      }  /*end for*/
    }
    else if (outstart && k == 2)
{
      if(tmcount == 2)
{
	for(i=iend[2]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 2)
{
	for(i=iend[2]+1;i<=istart[3]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 3)
{
      if(tmcount == 4)
{
	for(i=iend[4]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 4)
{
	for(i=iend[4]+1;i<=istart[5]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 4)
{
      if(tmcount == 6)
{
	for(i=iend[6]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 6)
{
	for(i=iend[6]+1;i<=istart[7]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 5)
{
      if(tmcount == 8)
{
	for(i=iend[8]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 8)
{
	for(i=iend[8]+1;i<=istart[9]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 6)
{
      if(tmcount == 10)
{
	for(i=iend[10]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 10)
{
	for(i=iend[10]+1;i<=istart[11]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 7)
{
      if(tmcount == 12)
{
	for(i=iend[12]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 12)
{
	for(i=iend[12]+1;i<=istart[13]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 1)
{
      if(tmcount == 1)
{
	for(i=iend[1]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 1)
{
	for(i=iend[1]+1;i<=istart[2]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 2)
{
      if(tmcount == 3)
{
	for(i=iend[3]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 3)
{
	for(i=iend[3]+1;i<=istart[4]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 3)
{
      if(tmcount == 5)
{
	for(i=iend[5]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 5)
{
	for(i=iend[5]+1;i<=istart[6]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 4)
{
      if(tmcount == 7)
{
	for(i=iend[7]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 7)
{
	for(i=iend[7]+1;i<=istart[8]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 5)
{
      if(tmcount == 9)
{
	for(i=iend[9]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 9)
{
	for(i=iend[9]+1;i<=istart[10]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 6)
{
      if(tmcount == 11)
{
	for(i=iend[11]+1;i<=length ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }
      else if (tmcount > 11)
{
	for(i=iend[11]+1;i<=istart[12]-1 ;i++)
{
	  if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 7)
{
      for(i=iend[13]+1;i<=length ;i++)
{
	if(strand[i] == 'c'|| strand[i] == 'C')syms[i]=symcys ;
      }  /*end for*/
    }  /*end if*/
  }  /*end for*/
  
  return ;
}
 
/* @func checklca *************************************************************
**
******************************************************************************/

void  checklca(const int symlcharge,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms)  {
/*#include "common.inc" */ 
  int    i,k ;
 
  for(k=1;k<=tmcount ;k++)
{
    for(i=istart[k];i<=iend[k] ;i++)
{
      if(strand[i] == 'd' || strand[i] == 'D')syms[i]=symlcharge 	 ;
      if(strand[i] == 'e' || strand[i] == 'E')syms[i]=symlcharge 	 ;
    }  /*end for*/
  }  	 /*end for*/
  return ;
}
 
/* @func checklcb *************************************************************
**
******************************************************************************/

void  checklcb(const int symlcharge,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms)  {
/*#include "common.inc" */ 
  int    i,k ;
 
  for(k=1;k<=tmcount ;k++)
{
    for(i=istart[k];i<=iend[k] ;i++)
{
      if(strand[i] == 'h' || strand[i] == 'H' )syms[i]=symlcharge 	 ;
      if(strand[i] == 'k' || strand[i] == 'K' )syms[i]=symlcharge 	 ;
      if(strand[i] == 'r' || strand[i] == 'R' )syms[i]=symlcharge ;
    }  /*end for*/
  }  	 /*end for*/
  return ;
}
 
/* @func checkla **************************************************************
**
******************************************************************************/

void  checkla(const int symlamine,const int tmcount,
	      ajuint *istart,ajuint *iend,char *strand,int *syms)  {
/*#include "common.inc" */ 
  int    i,k ;
 
  for(k=1;k<=tmcount ;k++)
{
    for(i=istart[k];i<=iend[k] ;i++)
{
      if(strand[i] == 'n' || strand[i] == 'N' )syms[i]=symlamine 	 ;
      if(strand[i] == 'q' || strand[i] == 'Q' )syms[i]=symlamine 	 ;
    }  /*end for*/
  }  	 /*end for*/
  return ;
}
 
/* @func checkloh *************************************************************
**
******************************************************************************/

void  checkloh(const int symloh,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms)
{
/*#include "common.inc" */ 
  int    i,k ;
 
  for(k=1;k<=tmcount ;k++)
{
    for(i=istart[k];i<=iend[k] ;i++)
{
      if(strand[i] == 's' || strand[i] == 'S' )syms[i]=symloh 	 ;
      if(strand[i] == 't' || strand[i] == 'T' )syms[i]=symloh 	 ;
      if(strand[i] == 'y' || strand[i] == 'Y' )syms[i]=symloh 	 ;
    }  /*end for*/
  }  	 /*end for*/
  return ;
}
 
/* @func checklar *************************************************************
**
******************************************************************************/

void  checklar(const int symlar,const int tmcount,
	       ajuint *istart,ajuint *iend,char *strand,int *syms)
{
/*#include "common.inc" */ 
  int    i,k ;
 
  for(k=1;k<=tmcount ;k++)
{
    for(i=istart[k];i<=iend[k] ;i++)
{
      if(strand[i] == 'f' || strand[i] == 'F' )syms[i]=symlar 	 ;
      if(strand[i] == 'y' || strand[i] == 'Y' )syms[i]=symlar 	 ;
      if(strand[i] == 'w' || strand[i] == 'W' )syms[i]=symlar 	 ;
    }  /*end for*/
  }  	 /*end for*/
  return ;
}
 
 
/* @func checklpro ************************************************************
**
******************************************************************************/

void  checklpro(const int symlpro,const int tmcount,
		ajuint *istart,ajuint *iend,char *strand,int *syms){
  int    i,k ;
 
  for(k=1;k<=tmcount ;k++)
{
    for(i=istart[k];i<=iend[k] ;i++)
{
      if(strand[i] == 'p' || strand[i] == 'P')syms[i]=symlpro 	 ;
    }  /*end for*/
  }  	 /*end for*/
  return ;
}
 
/* @func checkgly *************************************************************
**
******************************************************************************/

void  checkgly(AjBool outstart,const int symgly,const int tmcount,int length,
	       ajuint *istart,ajuint *iend,char *strand,int *syms)
{
  int    i,k,count,pass = 0 ;
  float       rcount ;
 
/*c determine the number of passes */ 
  count=tmcount/2 ;
  rcount=(float)tmcount/2.0 ;
  if(count == rcount)pass=count ;
  if(count < rcount)pass=count+1 ;
  
  for(k=1;k<=pass	 ;k++)
{
    if(outstart && k == 1)
{
      for(i=1;i<=istart[1]-1 ;i++)
{
	if(strand[i] == 'n'  || strand[i] == 'N' )
{
	  if(strand[i+2] == 's' || strand[i+2] == 't' || strand[i+2] == 'S' || strand[i+2] == 'T'  )
{
	    syms[i]=symgly ;
	  }  /*end if*/
	}  /*end if*/
      }  /*end for*/
    }
    else if (outstart && k == 2)
{
      if(tmcount == 2)
{
	for(i=iend[2]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N' )
{
	    if(strand[i+2] == 's' || strand[i+2] == 't'|| strand[i+2] == 'S' || strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 2)
{
	for(i=iend[2]+1;i<=istart[3]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N' )
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 3)
{
      if(tmcount == 4)
{
	for(i=iend[4]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N' )
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't'||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 4)
{
	for(i=iend[4]+1;i<=istart[5]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 4)
{
      if(tmcount == 6)
{
	for(i=iend[6]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 6)
{
	for(i=iend[6]+1;i<=istart[6]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 5)
{
      if(tmcount == 8)
{
	for(i=iend[8]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 8)
{
	for(i=iend[8]+1;i<=istart[9]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N' )
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 6)
{
      if(tmcount == 10)
{
	for(i=iend[10]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 10)
{
	for(i=iend[10]+1;i<=istart[11]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (outstart && k == 7)
{
      if(tmcount == 12)
{
	for(i=iend[12]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 12)
{
	for(i=iend[12]+1;i<=istart[13]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 1)
{
      if(tmcount == 1)
{
	for(i=iend[1]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 1)
{
	for(i=iend[1]+1;i<=istart[2]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 2)
{
      if(tmcount == 3)
{
	for(i=iend[3]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 3)
{
	for(i=iend[3]+1;i<=istart[4]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 3)
{
      if(tmcount == 5)
{
	for(i=iend[5]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 5)
{
	for(i=iend[5]+1;i<=istart[6]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 4)
{
      if(tmcount == 7)
{
	for(i=iend[7]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 7)
{
	for(i=iend[7]+1;i<=istart[8]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 5)
{
      if(tmcount == 9)
{
	for(i=iend[9]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 9)
{
	for(i=iend[9]+1;i<=istart[10]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 6)
{
      if(tmcount == 11)
{
	for(i=iend[11]+1;i<=length ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }
      else if (tmcount > 11)
{
	for(i=iend[11]+1;i<=istart[12]-1 ;i++)
{
	  if(strand[i] == 'n' || strand[i] == 'N')
{
	    if(strand[i+2] == 's' ||
	       strand[i+2] == 't' ||
	       strand[i+2] == 'S' ||
	       strand[i+2] == 'T')
{
	      syms[i]=symgly ;
	    }  /*end if*/
	  }  /*end if*/
	}  /*end for*/
      }  /*end if*/
    }
    else if (!outstart && k == 7)
{
      for(i=iend[13]+1;i<=length ;i++)
{
	if(strand[i] == 'n' || strand[i] == 'N')
{
	  if(strand[i+2] == 's' ||
	     strand[i+2] == 't' ||
	     strand[i+2] == 'S' ||
	     strand[i+2] == 'T')
{
	    syms[i]=symgly ;
	  }  /*end if*/
	}  /*end if*/
      }  /*end for*/
    }  /*end if*/
  }  /*end for*/
 
  return ;
}
 
/* @func topoMembrane *********************************************************
**
******************************************************************************/

void  topoMembrane(void)
{
    topoMembrane2();
    topoMembrane3();
    return             ;
}
 
/* @func topoMembrane2 ********************************************************
**
******************************************************************************/

void  topoMembrane2(void)
{ 
  topoMoveTo(10.0,70.0) ;
  topoDraw(140.0,70.0) ;
  topoMoveTo(10.0,70.1) ;
  topoDraw(140.0,70.1) ;
  topoMoveTo(10.0,70.2) ;
  topoDraw(140.0,70.2) ;
  return ;
}
 
/* @func topoMembrane3 ********************************************************
**
******************************************************************************/

void  topoMembrane3 (void)
{

  topoMoveTo(10.0,39.0) ;
  topoDraw(140.0,39.0) ;
  topoMoveTo(10.0,38.9) ;
  topoDraw(140.0,38.9) ;
  topoMoveTo(10.0,38.8) ;
  topoDraw(140.0,38.8) ;
  return ;
}
 
 
/* @func topoSymbol ***********************************************************
**
******************************************************************************/

void  topoSymbol(const float x,float y,char stran,int sym) 
{
    char  str[40] ;
    float xhex[7],yhex[7]; /* for drawing hexagons */


    str[1]='\0' ;
    /*c draw the symbols and put in the letter */ 
    /*c just the letter */ 
    if(sym == 0)
{
	topoMoveTo(x,y) ;
	str[0]=stran ;
	topoPlotText(str) ;
    }
    else if (sym == 1)
{
	/*c draw a circle and the letter */ 
	topoNewColour(0);
	topoMoveTo(x,y) ;
	topoCentre(x,y) ;
	topoCurve(1.4,0.0,360.0) ;
	topoMoveTo(x,y) ;
	str[0]=stran ;
	topoPlotText(str) ;
    }
    else if (sym == 2)
{
	/*c draw a square and the letter */ 
	topoMoveTo(x,y+1.4) ;
	topoDraw(x-1.4,y+1.4) ;
	topoDraw(x-1.4,y-1.4) ;
	topoDraw(x+1.4,y-1.4) ;
	topoDraw(x+1.4,y+1.4) ;
	topoDraw(x,y+1.4) ;
	topoMoveTo(x,y) ;
	str[0]=stran ;
	topoPlotText(str) ;
    }
    else if (sym == 3)
{
	/*c draw a hexagon and a letter */ 
	xhex[0] = x; yhex[0]=y+1.4;
	xhex[1] = x-1.4; yhex[1]=y+0.7;
	xhex[2] = x-1.4; yhex[2]=y-0.7;
	xhex[3] = x; yhex[3]=y-1.4;
	xhex[4] = x+1.4; yhex[4]=y-0.7;
	xhex[5] = x+1.4; yhex[5]=y+0.7;
	xhex[6] = x; yhex[6]=y+1.4;

	ajGraphicsDrawsetPoly(7,xhex,yhex);
	topoMoveTo(x,y) ;
	str[0]=stran ;
	topoPlotText(str) ;
    }
    else if (sym == 4)
{
	/*c draw a up arrow and a letter */ 

	xhex[0]=x; yhex[0]=y-1.4;
	xhex[1]=x+1.4; yhex[1]=y-1.05;
	xhex[2]=x; yhex[2]=y+1.4;
	xhex[3]=x-1.4; yhex[3]=y-1.05;
	xhex[4]=x; yhex[4]=y-1.4;
	ajGraphicsDrawsetPoly(5,xhex,yhex);

	topoMoveTo(x,y-.45) ;
	str[0]=stran ;
	topoPlotText(str) ;
    }
    else if (sym == 5)
{
	/*c draw a down arrow and a letter */ 
	
	xhex[0]=x; yhex[0]=y+1.4;
	xhex[1]=x-1.4; yhex[1]=y+1.05;
	xhex[2]=x; yhex[2]=y-1.4;
	xhex[3]=x+1.4; yhex[3]=y+1.05;
	xhex[4]=x; yhex[4]=y+1.4;

	ajGraphicsDrawsetPoly(5,xhex,yhex);

	topoMoveTo(x,y+.45) ;
	str[0]=stran ;
	topoPlotText(str) ;
    }
    /*c circle section */ 
    else if (6 <= sym && sym <= 13)
{
	/*c black circle - no letter */ 
	/*c black circle */ 
	if(sym == 6)topoNewColour(1) ;
	/*c green circle */ 
	if(sym == 7)topoNewColour(2) ;
	/*c red circle */ 
	if(sym == 8)topoNewColour(4) ;
	/*c blue circle */ 
	if(sym == 9)topoNewColour(3) ;
	/*c aqua circle */ 
	if(sym == 10)topoNewColour(5) ;
	/*c light-purple circle */ 
	if(sym == 11)topoNewColour(6) ;
	/*c yellow circle */ 
	if(sym == 12)topoNewColour(7) ;
	/*c unknown color circle */ 
	if(sym == 13)topoNewColour(8) ;
	topoMoveTo(x,y) ;
	topoCentre(x,y) ;
	topoCurve(.1,0.0,360.0) ;
	topoCurve(.2,0.0,360.0) ;
	topoCurve(.3,0.0,360.0) ;
	topoCurve(.4,0.0,360.0) ;
	topoCurve(.5,0.0,360.0) ;
	topoCurve(.6,0.0,360.0) ;
	topoCurve(.7,0.0,360.0) ;
	topoCurve(.8,0.0,360.0) ;
	topoCurve(.9,0.0,360.0) ;
	topoCurve(1.0,0.0,360.0) ;
	topoCurve(1.1,0.0,360.0) ;
	topoCurve(1.2,0.0,360.0) ;
	topoCurve(1.3,0.0,360.0) ;
	topoCurve(1.4,0.0,360.0) ;
	topoNewColour(1) ;
	topoMoveTo(x,y) ;
	str[0]=stran ;
	topoPlotText(str) ;
	/*c square section */ 
	/*c square with a letter */ 
    }
    else if (14 <= sym && sym <= 21)
{
	/*c black square */ 
	if(sym == 14)topoNewColour(1) ;
	/*c green square */ 
	if(sym == 15)topoNewColour(2) ;
	/*c red square */ 
	if(sym == 16)topoNewColour(4) ;
	/*c blue square */ 
	if(sym == 17)topoNewColour(3) ;
	/*c aqua square */ 
	if(sym == 18)topoNewColour(5) ;
	/*c light-purple square */ 
	if(sym == 19)topoNewColour(6) ;
	/*c yellow square */ 
	if(sym == 20)topoNewColour(7) ;
	/*c unknown color square */ 
	if(sym == 21)topoNewColour(8) ;

	ajGraphicsDrawposBoxFill(x-1.4,y-1.4,2.8);

	topoNewColour(1) ;
	topoMoveTo(x,y) ;
	str[0]=stran ;
	topoPlotText(str) ;
	/*c hexagon with a letter */ 
    }
    else if (22 <= sym && sym <= 29)
{
	/*c black hexagon */ 
	if(sym == 22)topoNewColour(1) ;
	/*c green hexagon */ 
	if(sym == 23)topoNewColour(2) ;
	/*c red hexagon */ 
	if(sym == 24)topoNewColour(4) ;
	/*c blue hexagon */ 
	if(sym == 25)topoNewColour(3) ;
	/*c aqua hexagon */ 
	if(sym == 26)topoNewColour(5) ;
	/*c light-purple hexagon */ 
	if(sym == 27)topoNewColour(6) ;
	/*c yellow hexagon */ 
	if(sym == 28)topoNewColour(7) ;
	/*c unknown color hexagon */ 
	if(sym == 29)topoNewColour(8) ;

	xhex[0] = x; yhex[0]=y+1.4;
	xhex[1] = x-1.4; yhex[1]=y+0.7;
	xhex[2] = x-1.4; yhex[2]=y-0.7;
	xhex[3] = x; yhex[3]=y-1.4;
	xhex[4] = x+1.4; yhex[4]=y-0.7;
	xhex[5] = x+1.4; yhex[5]=y+0.7;
	xhex[6] = x; yhex[6]=y+1.4;


	ajGraphicsDrawsetPolyFill(7,xhex,yhex);

	topoNewColour(1) ;
	topoMoveTo(x,y) ;
	str[0]=stran ;
	topoPlotText(str) ;
	/*c up arrow section */ 
	/*c up arrow with a letter */ 
    }
    else if (30 <= sym && sym <= 37)
{
	/*c black up arrow */ 
	if(sym == 30)topoNewColour(1) ;
	/*c green up arrow */ 
	if(sym == 31)topoNewColour(2) ;
	/*c red up arrow */ 
	if(sym == 32)topoNewColour(4) ;
	/*c blue up arrow */ 
	if(sym == 33)topoNewColour(3) ;
	/*c aqua up arrow */ 
	if(sym == 34)topoNewColour(5) ;
	/*c light-purple up arrow */ 
	if(sym == 35)topoNewColour(6) ;
	/*c yellow hexagon */ 
	if(sym == 36)topoNewColour(7) ;
	/*c unknown color up arrow */ 
	if(sym == 37)topoNewColour(8) ;

	xhex[0]=x; yhex[0]=y-1.4;
	xhex[1]=x+1.4; yhex[1]=y-1.05;
	xhex[2]=x; yhex[2]=y+1.4;
	xhex[3]=x-1.4; yhex[3]=y-1.05;
	xhex[4]=x; yhex[4]=y-1.4;
	ajGraphicsDrawsetPolyFill(5,xhex,yhex);

	topoNewColour(1) ;
	topoMoveTo(x,y-.45) ;
	str[0]=stran ;
	topoPlotText(str) ;
	/*c down arrow section */ 
	/*c down arrow with a letter */ 
    }
    else if (38 <= sym && sym <= 45)
{
	/*c black down arrow */ 
	if(sym == 38)topoNewColour(1) ;
	/*c green down arrow */ 
	if(sym == 39)topoNewColour(2) ;
	/*c red down arrow */ 
	if(sym == 40)topoNewColour(4) ;
	/*c blue down arrow */ 
	if(sym == 41)topoNewColour(3) ;
	/*c aqua down arrow */ 
	if(sym == 42)topoNewColour(5) ;
	/*c light-purple down arrow */ 
	if(sym == 43)topoNewColour(6) ;
	/*c yellow down arrow */ 
	if(sym == 44)topoNewColour(7) ;
	/*c unknown color down arrow */ 
	if(sym == 45)topoNewColour(8) ;
	xhex[0]=x; yhex[0]=y+1.4;
	xhex[1]=x-1.4; yhex[1]=y+1.05;
	xhex[2]=x; yhex[2]=y-1.4;
	xhex[3]=x+1.4; yhex[3]=y+1.05;
	xhex[4]=x; yhex[4]=y+1.4;

	ajGraphicsDrawsetPolyFill(5,xhex,yhex);

	topoNewColour(1) ;
	topoMoveTo(x,y+.45) ;
	str[0]=stran ;
	topoPlotText(str) ;
    }  /*end if*/
    return    ;
}
 
/* @func starttop7 ************************************************************
**
******************************************************************************/

void  starttop7(AjPGraph graph, const int tmcount,const int istart,float *off,
		char *strand,int *syms) 
{
    char  stran ;
    int   j,sym ;
    float x = 0.0,y,wide,shift ;

    /*c this assumes 7 transmembrane sections */ 
    if(istart <= 9)shift=0.0 ;
    if(istart > 9)shift=1.4 ;

    wide=16.8 ;
    if(tmcount == 1)
{
	*off=75.0 ;
    }
    else if (tmcount == 2)
{
	*off=66.6 ;
    }
    else if (tmcount == 3)
{
	*off=58.2 ;
    }
    else if (tmcount == 4)
{
	*off=49.8 ;
    }
    else if (tmcount == 5)
{
	*off=41.4 ;
    }
    else if (tmcount == 6)
{
	*off=33.0 ;
    }
    else if (tmcount == 7)
{
	*off=24.6 ;
    }
    else if (tmcount > 7)
{
	*off=24.6 ;
    }  /*end if*/
  
    /*c draw the chain back from the attachment point */ 
  for(j=1;j<=istart-1 ;j++)
{
    if(j <= 9)
{
      x=*off ;
      y=71.6+2.8*(j-1) ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (10 <= j && j <= 11)
{
      if(j == 10)x=*off-shift    ;
      if(j == 11)x=*off-shift-2.8    ;
      y=71.6+25.2 ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (12 <= j && j <= 18)
{
      x=*off-2.0*shift-2.8 ;
      y=96.8-2.8*(j-11) ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (19 <= j && j <= 20)
{
      if(j == 19)x=*off-3.0*shift-2.8    ;
      if(j == 20)x=*off-3.0*shift-5.6    ;
      y=74.4 ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (21 <= j && j <= 27)
{
      x=*off-5.6-4.0*shift ;
      y=71.6+2.8*(j-19) ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (28 <= j && j <= 29)
{
      if(j == 28)x=*off-5.6-5.0*shift    ;
      if(j == 29)x=*off-5.0*shift-8.4    ;
      y=96.8 ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (30 <= j && j <= 36)
{
      x=*off-6.0*shift-8.4 ;
      y=96.8-2.8*(j-29) ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (istart > 36 && *off == 24.6)
{
      x=*off-8.4-7.0*shift    ;
      y=74.4 ;
      topoTerm(x,y) ;
    }
    else if (37 <= j && j <= 38)
{
      if(j == 37)x=*off-8.4-7.0*shift    ;
      if(j == 38)x=*off-7.0*shift-11.2    ;
      y=74.4 ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (39 <= j && j <= 45)
{
      x=*off-8.0*shift-11.2 ;
      y=71.6+2.8*(j-37) ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (46 <= j && j <= 47)
{
      if(j == 46)x=*off-11.2-9.0*shift    ;
      if(j == 47)x=*off-9.0*shift-14.0    ;
      y=96.8 ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (48 <= j && j <= 54)
{
      x=*off-10.0*shift-14.0 ;
      y=96.8-2.8*(j-47) ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (istart > 54 && *off == 33.0)
{
      y=74.4 ;
      x=*off-11.0*shift-14.0 ;
      topoTerm(x,y)	 ;
    }
    else if (55 <= j && j <= 56)
{
      if(j == 55)x=*off-14.0-11.0*shift    ;
      if(j == 56)x=*off-11.0*shift-16.8    ;
      y=74.4 ;
      stran=strand[istart-j] ;
      sym=syms[istart-j] ;
      topoSymbol(x,y,stran,sym) ;
    }
    else if (57 <= j && j <= 63)
{
      x=*off-12.0*shift-16.8 ;
      y=71.6+2.8*(j-55) ;
      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (64 <= j && j <= 65)
{
	      if(j == 64)x=*off-16.8-13.0*shift    ;
	      if(j == 65)x=*off-13.0*shift-19.6    ;
              y=96.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (66 <= j && j <= 72)
{
              x=*off-14.0*shift-19.6 ;
	      y=96.8-2.8*(j-65) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 72 && *off == 41.4)
{
              y=74.4 ;
	      x=*off-15.0*shift-19.6 ;
	      topoTerm(x,y)	       ;
	}
	else if (73 <= j && j <= 74)
{
	      if(j == 73)x=*off-19.6-15.0*shift    ;
	      if(j == 74)x=*off-15.0*shift-22.4    ;
              y=74.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (75 <= j && j <= 81)
{
              x=*off-16.0*shift-22.4 ;
	      y=71.6+2.8*(j-73) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 81 && *off == 49.8)
{
              y=96.8 ;
	      x=*off-17.0*shift-22.4 ;
	      topoTerm(x,y)	       ;
	}
	else if (82 <= j && j <= 83)
{
	      if(j == 82)x=*off-22.4-17.0*shift    ;
	      if(j == 83)x=*off-17.0*shift-25.2    ;
              y=96.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (84 <= j && j <= 90)
{
              x=*off-18.0*shift-25.2 ;
	      y=96.8-2.8*(j-83) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (j > 90 && *off == 58.2)
{
	      x=*off-25.2-19.0*shift ;
	      y=74.4 ;
	      topoTerm(x,y) ;
	      goto CAA ;
	}
	else if (91 <= j && j <= 92)
{
	      if(j == 91)x=*off-25.2-19.0*shift    ;
	      if(j == 92)x=*off-19.0*shift-28.0    ;
              y=74.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (93 <= j && j <= 99)
{
              x=*off-20.0*shift-28.0 ;
	      y=71.6+2.8*(j-91) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 99 && *off == 66.6)
{
              y=96.8 ;
	      x=*off-21.0*shift-28.0 ;
	      topoTerm(x,y)	       ;
	}
	else if (100 <= j && j <= 101)
{
	      if(j == 100)x=*off-28.0-21.0*shift    ;
	      if(j == 101)x=*off-21.0*shift-30.8    ;
              y=96.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (102 <= j && j <= 108)
{
              x=*off-22.0*shift-30.8 ;
	      y=96.8-2.8*(j-101) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 108 && *off == 75.0)
{
	      x=*off-23.0*shift-30.8    ;
              y=74.4 ;
	      topoTerm(x,y) ;
	   }  /*end if*/
	}  /*end for*/
CAA:
	return ;
}
 
/* @func starttop7full ********************************************************
**
******************************************************************************/

void  starttop7full(AjPGraph graph, const int tmcount,
		    const int istart,char *strand,int *syms)
{
 
/*#include "common.inc" */ 
 
  char  stran ;
  int    j,sym;
  float       x,y,shift ;

/*c draw the membrane layer at the bottom of the page */ 
	topoMoveTo(10.0,10.0) ;
	topoDraw(149.0,10.0) ;
	topoMoveTo(10.0,9.9) ;
	topoDraw(149.0,9.9) ;
	topoMoveTo(10.0,9.8) ;
	topoDraw(149.0,9.8) ;
 
/*c set shift value */ 
	shift=1.4 ;
 
/*c put in the first residue of the transmembrane helix for alignment */ 
/*c purposes */ 
	x=140.0 ;
	y=8.4 ;
	stran=strand[istart] ;
	sym=syms[istart] ;
	topoSymbol(x,y,stran,sym) ;
 
/*c draw the chain back from the attachment point */ 
	for(j=1;j<=istart-1 ;j++)
{
	   if(j <= 31)
{
	      x=140.0 ;
	      y=11.6+2.8*(j-1) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (32 <= j && j <= 33)
{
	      if(j == 32)x=140.0-shift    ;
	      if(j == 33)x=140.0-shift-2.8    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (34 <= j && j <= 62)
{
              x=140.0-2.0*shift-2.8 ;
	      y=98.4-2.8*(j-33) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (63 <= j && j <= 64)
{
	      if(j == 63)x=140.0-3.0*shift-2.8    ;
	      if(j == 64)x=140.0-3.0*shift-5.6    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (65 <= j && j <= 93)
{
	      x=140.0-5.6-4.0*shift ;
	      y=14.2+2.8*(j-64) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (94 <= j && j <= 95)
{
	      if(j == 94)x=140.0-5.6-5.0*shift    ;
	      if(j == 95)x=140.0-5.0*shift-8.4    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (96 <= j && j <= 124)
{
              x=140.0-6.0*shift-8.4 ;
	      y=98.4-2.8*(j-95) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (125 <= j && j <= 126)
{
	      if(j == 125)x=140.0-8.4-7.0*shift    ;
	      if(j == 126)x=140.0-7.0*shift-11.2    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (127 <= j && j <= 155)
{
              x=140.0-8.0*shift-11.2 ;
	      y=14.2+2.8*(j-126) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (156 <= j && j <= 157)
{
	      if(j == 156)x=140.0-11.2-9.0*shift    ;
	      if(j == 157)x=140.0-9.0*shift-14.0    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (158 <= j && j <= 186)
{
              x=140.0-10.0*shift-14.0 ;
	      y=98.4-2.8*(j-157) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (187 <= j && j <= 188)
{
	      if(j == 187)x=140.0-14.0-11.0*shift    ;
	      if(j == 188)x=140.00-11.0*shift-16.8    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (189 <= j && j <= 217)
{
              x=140.0-12.0*shift-16.8 ;
	      y=14.2+2.8*(j-188) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (218 <= j && j <= 219)
{
	      if(j == 218)x=140.0-16.8-13.0*shift    ;
	      if(j == 219)x=140.0-13.0*shift-19.6    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (220 <= j && j <= 248)
{
              x=140-14.0*shift-19.6 ;
	      y=98.4-2.8*(j-219) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (249 <= j && j <= 250)
{
	      if(j == 249)x=140.0-19.6-15.0*shift    ;
	      if(j == 250)x=140.0-15.0*shift-22.4    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (251 <= j && j <= 279)
{
              x=140.0-16.0*shift-22.4 ;
	      y=14.2+2.8*(j-250) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (280 <= j && j <= 281)
{
	      if(j == 280)x=140.0-22.4-17.0*shift    ;
	      if(j == 281)x=140.0-17.0*shift-25.2    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (282 <= j && j <= 310)
{
              x=140.0-18.0*shift-25.2 ;
	      y=98.4-2.8*(j-281) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (311 <= j && j <= 312)
{
	      if(j == 311)x=140.0-25.2-19.0*shift    ;
	      if(j == 312)x=140.0-19.0*shift-28.0    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (313 <= j && j <= 341)
{
              x=140.0-20.0*shift-28.0 ;
	      y=14.2+2.8*(j-312) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (342 <= j && j <= 343)
{
	      if(j == 342)x=140.0-28.0-21.0*shift    ;
	      if(j == 343)x=140.0-21.0*shift-30.8    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (344 <= j && j <= 372)
{
              x=140.0-22.0*shift-30.8 ;
	      y=98.4-2.8*(j-343) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (373 <= j && j <= 374)
{
	      if(j == 373)x=140.0-30.8-23.0*shift    ;
	      if(j == 374)x=140.0-23.0*shift-33.6    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (375 <= j && j <= 403)
{
              x=140.0-24.0*shift-33.6 ;
	      y=14.2+2.8*(j-374) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (404 <= j && j <= 405)
{
	      if(j == 404)x=140.0-33.6-25.0*shift    ;
	      if(j == 405)x=140.0-25.0*shift-36.4    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (406 <= j && j <= 434)
{
              x=140.0-26.0*shift-36.4 ;
	      y=98.4-2.8*(j-405) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (435 <= j && j <= 436)
{
	      if(j == 435)x=140.0-36.4-27.0*shift    ;
	      if(j == 436)x=140.0-27.0*shift-39.2    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (437 <= j && j <= 465)
{
              x=140.0-28.0*shift-39.2 ;
	      y=14.2+2.8*(j-436) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (466 <= j && j <= 467)
{
	      if(j == 466)x=140.0-39.2-29.0*shift    ;
	      if(j == 467)x=140.0-29.0*shift-42.0    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (468 <= j && j <= 496)
{
              x=140.0-30.0*shift-42.0 ;
	      y=98.4-2.8*(j-467) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (497 <= j && j <= 498)
{
	      if(j == 497)x=140.0-42.0-31.0*shift    ;
	      if(j == 498)x=140.0-31.0*shift-44.8    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (499 <= j && j <= 527)
{
              x=140.0-32.0*shift-44.8 ;
	      y=14.2+2.8*(j-498) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (528 <= j && j <= 529)
{
	      if(j == 528)x=140.0-44.8-33.0*shift    ;
	      if(j == 529)x=140.0-33.0*shift-47.6    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (530 <= j && j <= 558)
{
              x=140.0-34.0*shift-47.6 ;
	      y=98.4-2.8*(j-529) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (559 <= j && j <= 560)
{
	      if(j == 559)x=140.0-47.6-35.0*shift    ;
	      if(j == 560)x=140.0-35.0*shift-50.4    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (561 <= j && j <= 589)
{
              x=140.0-36.0*shift-50.4 ;
	      y=14.2+2.8*(j-560) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (590 <= j && j <= 591)
{
	      if(j == 590)x=140.0-50.4-37.0*shift    ;
	      if(j == 591)x=140.0-37.0*shift-53.2    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (592 <= j && j <= 620)
{
              x=140.0-38.0*shift-53.2 ;
	      y=98.4-2.8*(j-591) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (621 <= j && j <= 622)
{
	      if(j == 621)x=140.0-53.2-39.0*shift    ;
	      if(j == 622)x=140.0-39.0*shift-56.0    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (623 <= j && j <= 651)
{
              x=140.0-40.0*shift-56.0 ;
	      y=14.2+2.8*(j-622) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (652 <= j && j <= 653)
{
	      if(j == 652)x=140.0-56.0-41.0*shift    ;
	      if(j == 653)x=140.0-41.0*shift-58.8    ;
              y=98.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (654 <= j && j <= 682)
{
              x=140.0-42.0*shift-58.8 ;
	      y=98.4-2.8*(j-653) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (683 <= j && j <= 684)
{
	      if(j == 683)x=140.0-58.8-43.0*shift    ;
	      if(j == 684)x=140.0-43.0*shift-61.6    ;
              y=14.2 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (685 <= j && j <= 713)
{
              x=140.0-44.0*shift-61.6 ;
	      y=14.2+2.8*(j-684) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end if*/
	}  /*end for*/
  ajGraphNewpage(graph,AJFALSE);
return ;
}
 
/* @func endtop7full **********************************************************
**
******************************************************************************/

void  endtop7full(AjPGraph graph, const int tmcount,
		  const int iend,const int length,
		  char *strand,int *syms)  {
 
/*#include "common.inc" */ 
 
  char  stran ;
  int    j,remain,sym ;
  float       x,y,shift ;
	
/*c draw the membrane layer at the bottom of the page */ 
	topoMoveTo(1.0,10.0) ;
	topoDraw(140.0,10.0) ;
	topoMoveTo(1.0,9.9) ;
	topoDraw(140.0,9.9) ;
	topoMoveTo(1.0,9.8) ;
	topoDraw(140.0,9.8) ;
 
/*c set shift value */ 
	shift=1.4 ;
 
/*c put in the first residue of the transmembrane helix for alignment */ 
/*c purposes */ 
	x=10.0 ;
	y=8.4 ;
	stran=strand[iend] ;
	sym=syms[iend] ;
	topoSymbol(x,y,stran,sym) ;
 
	remain=length-iend ;
 
/*c draw the chain back from the attachment point */ 
	for(j=1;j<=remain ;j++)
{
	   if(j <= 31)
{
	      x=10.0 ;
	      y=11.6+2.8*(j-1) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (32 <= j && j <= 33)
{
	      if(j == 32)x=10.0+shift    ;
	      if(j == 33)x=10.0+shift+2.8    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (34 <= j && j <= 62)
{
              x=10.0+2.0*shift+2.8 ;
	      y=98.4-2.8*(j-33) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (63 <= j && j <= 64)
{
	      if(j == 63)x=10.0+3.0*shift+2.8    ;
	      if(j == 64)x=10.0+3.0*shift+5.6    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (65 <= j && j <= 93)
{
	      x=10.0+5.6+4.0*shift ;
	      y=14.2+2.8*(j-64) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (94 <= j && j <= 95)
{
	      if(j == 94)x=10.0+5.6+5.0*shift    ;
	      if(j == 95)x=10.0+5.0*shift+8.4    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (96 <= j && j <= 124)
{
              x=10.0+6.0*shift+8.4 ;
	      y=98.4-2.8*(j-95) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (125 <= j && j <= 126)
{
	      if(j == 125)x=10.0+8.4+7.0*shift    ;
	      if(j == 126)x=10.0+7.0*shift+11.2    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (127 <= j && j <= 155)
{
              x=10.0+8.0*shift+11.2 ;
	      y=14.2+2.8*(j-126) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (156 <= j && j <= 157)
{
	      if(j == 156)x=10.0+11.2+9.0*shift    ;
	      if(j == 157)x=14.0+9.0*shift+14.0    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (158 <= j && j <= 186)
{
              x=10.0+10.0*shift+14.0 ;
	      y=98.4-2.8*(j-157) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (187 <= j && j <= 188)
{
	      if(j == 187)x=10.0+14.0+11.0*shift    ;
	      if(j == 188)x=10.00+11.0*shift+16.8    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (189 <= j && j <= 217)
{
              x=10.0+12.0*shift+16.8 ;
	      y=14.2+2.8*(j-188) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (218 <= j && j <= 219)
{
	      if(j == 218)x=10.0+16.8+13.0*shift    ;
	      if(j == 219)x=10.0+13.0*shift+19.6    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (220 <= j && j <= 248)
{
              x=10.0+14.0*shift+19.6 ;
	      y=98.4-2.8*(j-219) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (249 <= j && j <= 250)
{
	      if(j == 249)x=10.0+19.6+15.0*shift    ;
	      if(j == 250)x=10.0+15.0*shift+22.4    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (251 <= j && j <= 279)
{
              x=10.0+16.0*shift+22.4 ;
	      y=14.2+2.8*(j-250) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (280 <= j && j <= 281)
{
	      if(j == 280)x=10.0+22.4+17.0*shift    ;
	      if(j == 281)x=10.0+17.0*shift+25.2    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (282 <= j && j <= 310)
{
              x=10.0+18.0*shift+25.2 ;
	      y=98.4-2.8*(j-281) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (311 <= j && j <= 312)
{
	      if(j == 311)x=10.0+25.2+19.0*shift    ;
	      if(j == 312)x=10.0+19.0*shift+28.0    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (313 <= j && j <= 341)
{
              x=10.0+20.0*shift+28.0 ;
	      y=14.2+2.8*(j-312) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (342 <= j && j <= 343)
{
	      if(j == 342)x=10.0+28.0+21.0*shift    ;
	      if(j == 343)x=10.0+21.0*shift+30.8    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (344 <= j && j <= 372)
{
              x=10.0+22.0*shift+30.8 ;
	      y=98.4-2.8*(j-343) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (373 <= j && j <= 374)
{
	      if(j == 373)x=10.0+30.8+23.0*shift    ;
	      if(j == 374)x=10.0+23.0*shift+33.6    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (375 <= j && j <= 403)
{
              x=10.0+24.0*shift+33.6 ;
	      y=14.2+2.8*(j-374) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (404 <= j && j <= 405)
{
	      if(j == 404)x=10.0+33.6+25.0*shift    ;
	      if(j == 405)x=10.0+25.0*shift+36.4    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (406 <= j && j <= 434)
{
              x=10.0+26.0*shift+36.4 ;
	      y=98.4-2.8*(j-405) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (435 <= j && j <= 436)
{
	      if(j == 435)x=10.0+36.4+27.0*shift    ;
	      if(j == 436)x=10.0+27.0*shift+39.2    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (437 <= j && j <= 465)
{
              x=10.0+28.0*shift+39.2 ;
	      y=14.2+2.8*(j-436) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (466 <= j && j <= 467)
{
	      if(j == 466)x=10.0+39.2+29.0*shift    ;
	      if(j == 467)x=10.0+29.0*shift+42.0    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (468 <= j && j <= 496)
{
              x=10.0+30.0*shift+42.0 ;
	      y=98.4-2.8*(j-467) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (497 <= j && j <= 498)
{
	      if(j == 497)x=10.0+42.0+31.0*shift    ;
	      if(j == 498)x=10.0+31.0*shift+44.8    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (499 <= j && j <= 527)
{
              x=10.0+32.0*shift+44.8 ;
	      y=14.2+2.8*(j-498) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (528 <= j && j <= 529)
{
	      if(j == 528)x=10.0+44.8+33.0*shift    ;
	      if(j == 529)x=10.0+33.0*shift+47.6    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (530 <= j && j <= 558)
{
              x=10.0+34.0*shift+47.6 ;
	      y=98.4-2.8*(j-529) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (559 <= j && j <= 560)
{
	      if(j == 559)x=10.0+47.6+35.0*shift    ;
	      if(j == 560)x=10.0+35.0*shift+50.4    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (561 <= j && j <= 589)
{
              x=10.0+36.0*shift+50.4 ;
	      y=14.2+2.8*(j-560) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (590 <= j && j <= 591)
{
	      if(j == 590)x=10.0+50.4+37.0*shift    ;
	      if(j == 591)x=10.0+37.0*shift+53.2    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (592 <= j && j <= 620)
{
              x=10.0+38.0*shift+53.2 ;
	      y=98.4-2.8*(j-591) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (621 <= j && j <= 622)
{
	      if(j == 621)x=10.0+53.2+39.0*shift    ;
	      if(j == 622)x=10.0+39.0*shift+56.0    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (623 <= j && j <= 651)
{
              x=10.0+40.0*shift+56.0 ;
	      y=14.2+2.8*(j-622) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (652 <= j && j <= 653)
{
	      if(j == 652)x=10.0+56.0+41.0*shift    ;
	      if(j == 653)x=10.0+41.0*shift+58.8    ;
              y=98.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (654 <= j && j <= 682)
{
              x=10.0+42.0*shift+58.8 ;
	      y=98.4-2.8*(j-653) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (683 <= j && j <= 684)
{
	      if(j == 683)x=10.0+58.8+43.0*shift    ;
	      if(j == 684)x=10.0+43.0*shift+61.6    ;
              y=14.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (685 <= j && j <= 713)
{
              x=10.0+44.0*shift+61.6 ;
	      y=14.2+2.8*(j-684) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end if*/
	}  /*end for*/
  ajGraphNewpage(graph,AJFALSE);
	return ;
}
 
/* @func startbot7full ********************************************************
**
******************************************************************************/

void  startbot7full(AjPGraph graph, const int tmcount,
		    int istart,char *strand,int *syms)  {
 
/*#include "common.inc" */ 
 
	char  stran ;
	int    j,sym;
	float       x,y,shift ;
 
/*c draw the membrane layer at the bottom of the page */ 
	topoMoveTo(10.0,90.0) ;
	topoDraw(149.0,90.0) ;
	topoMoveTo(10.0,90.1) ;
	topoDraw(149.0,90.1) ;
	topoMoveTo(10.0,90.2) ;
	topoDraw(149.0,90.2) ;
 
/*c set shift value */ 
	shift=1.4 ;
 
/*c put in the first residue of the transmembrane helix for alignment */ 
/*c purposes */ 
	x=140.0 ;
	y=91.6 ;
	stran=strand[istart] ;
	sym=syms[istart] ;
	topoSymbol(x,y,stran,sym) ;
 
/*c draw the chain back from the attachment point */ 
	for(j=1;j<=istart-1 ;j++)
{
	   if(j <= 31)
{
	      x=140.0 ;
	      y=88.6-2.8*(j-1) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (32 <= j && j <= 33)
{
	      if(j == 32)x=140.0-shift    ;
	      if(j == 33)x=140.0-shift-2.8    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (34 <= j && j <= 62)
{
              x=140.0-2.0*shift-2.8 ;
	      y=1.8+2.8*(j-33) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (63 <= j && j <= 64)
{
	      if(j == 63)x=140.0-3.0*shift-2.8    ;
	      if(j == 64)x=140.0-3.0*shift-5.6    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (65 <= j && j <= 93)
{
	      x=140.0-5.6-4.0*shift ;
	      y=85.8-2.8*(j-64) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (94 <= j && j <= 95)
{
	      if(j == 94)x=140.0-5.6-5.0*shift    ;
	      if(j == 95)x=140.0-5.0*shift-8.4    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (96 <= j && j <= 124)
{
              x=140.0-6.0*shift-8.4 ;
	      y=1.8+2.8*(j-95) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (125 <= j && j <= 126)
{
	      if(j == 125)x=140.0-8.4-7.0*shift    ;
	      if(j == 126)x=140.0-7.0*shift-11.2    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (127 <= j && j <= 155)
{
              x=140.0-8.0*shift-11.2 ;
	      y=85.8-2.8*(j-126) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (156 <= j && j <= 157)
{
	      if(j == 156)x=140.0-11.2-9.0*shift    ;
	      if(j == 157)x=140.0-9.0*shift-14.0    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (158 <= j && j <= 186)
{
              x=140.0-10.0*shift-14.0 ;
	      y=1.8+2.8*(j-157) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (187 <= j && j <= 188)
{
	      if(j == 187)x=140.0-14.0-11.0*shift    ;
	      if(j == 188)x=140.00-11.0*shift-16.8    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (189 <= j && j <= 217)
{
              x=140.0-12.0*shift-16.8 ;
	      y=85.8-2.8*(j-188) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (218 <= j && j <= 219)
{
	      if(j == 218)x=140.0-16.8-13.0*shift    ;
	      if(j == 219)x=140.0-13.0*shift-19.6    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (220 <= j && j <= 248)
{
              x=140-14.0*shift-19.6 ;
	      y=1.8+2.8*(j-219) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (249 <= j && j <= 250)
{
	      if(j == 249)x=140.0-19.6-15.0*shift    ;
	      if(j == 250)x=140.0-15.0*shift-22.4    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (251 <= j && j <= 279)
{
              x=140.0-16.0*shift-22.4 ;
	      y=85.8-2.8*(j-250) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (280 <= j && j <= 281)
{
	      if(j == 280)x=140.0-22.4-17.0*shift    ;
	      if(j == 281)x=140.0-17.0*shift-25.2    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (282 <= j && j <= 310)
{
              x=140.0-18.0*shift-25.2 ;
	      y=1.8+2.8*(j-281) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (311 <= j && j <= 312)
{
	      if(j == 311)x=140.0-25.2-19.0*shift    ;
	      if(j == 312)x=140.0-19.0*shift-28.0    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (313 <= j && j <= 341)
{
              x=140.0-20.0*shift-28.0 ;
	      y=85.8-2.8*(j-312) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (342 <= j && j <= 343)
{
	      if(j == 342)x=140.0-28.0-21.0*shift    ;
	      if(j == 343)x=140.0-21.0*shift-30.8    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (344 <= j && j <= 372)
{
              x=140.0-22.0*shift-30.8 ;
	      y=1.8+2.8*(j-343) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (373 <= j && j <= 374)
{
	      if(j == 373)x=140.0-30.8-23.0*shift    ;
	      if(j == 374)x=140.0-23.0*shift-33.6    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (375 <= j && j <= 403)
{
              x=140.0-24.0*shift-33.6 ;
	      y=85.8-2.8*(j-374) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (404 <= j && j <= 405)
{
	      if(j == 404)x=140.0-33.6-25.0*shift    ;
	      if(j == 405)x=140.0-25.0*shift-36.4    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (406 <= j && j <= 434)
{
              x=140.0-26.0*shift-36.4 ;
	      y=1.8+2.8*(j-405) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (435 <= j && j <= 436)
{
	      if(j == 435)x=140.0-36.4-27.0*shift    ;
	      if(j == 436)x=140.0-27.0*shift-39.2    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (437 <= j && j <= 465)
{
              x=140.0-28.0*shift-39.2 ;
	      y=85.8-2.8*(j-436) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (466 <= j && j <= 467)
{
	      if(j == 466)x=140.0-39.2-29.0*shift    ;
	      if(j == 467)x=140.0-29.0*shift-42.0    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (468 <= j && j <= 496)
{
              x=140.0-30.0*shift-42.0 ;
	      y=1.8+2.8*(j-467) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (497 <= j && j <= 498)
{
	      if(j == 497)x=140.0-42.0-31.0*shift    ;
	      if(j == 498)x=140.0-31.0*shift-44.8    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (499 <= j && j <= 527)
{
              x=140.0-32.0*shift-44.8 ;
	      y=85.8-2.8*(j-498) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (528 <= j && j <= 529)
{
	      if(j == 528)x=140.0-44.8-33.0*shift    ;
	      if(j == 529)x=140.0-33.0*shift-47.6    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (530 <= j && j <= 558)
{
              x=140.0-34.0*shift-47.6 ;
	      y=1.8+2.8*(j-529) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (559 <= j && j <= 560)
{
	      if(j == 559)x=140.0-47.6-35.0*shift    ;
	      if(j == 560)x=140.0-35.0*shift-50.4    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (561 <= j && j <= 589)
{
              x=140.0-36.0*shift-50.4 ;
	      y=85.8-2.8*(j-560) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (590 <= j && j <= 591)
{
	      if(j == 590)x=140.0-50.4-37.0*shift    ;
	      if(j == 591)x=140.0-37.0*shift-53.2    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (592 <= j && j <= 620)
{
              x=140.0-38.0*shift-53.2 ;
	      y=1.8+2.8*(j-591) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (621 <= j && j <= 622)
{
	      if(j == 621)x=140.0-53.2-39.0*shift    ;
	      if(j == 622)x=140.0-39.0*shift-56.0    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (623 <= j && j <= 651)
{
              x=140.0-40.0*shift-56.0 ;
	      y=85.8-2.8*(j-622) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (652 <= j && j <= 653)
{
	      if(j == 652)x=140.0-56.0-41.0*shift    ;
	      if(j == 653)x=140.0-41.0*shift-58.8    ;
              y=1.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (654 <= j && j <= 682)
{
              x=140.0-42.0*shift-58.8 ;
	      y=1.8+2.8*(j-653) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (683 <= j && j <= 684)
{
	      if(j == 683)x=140.0-58.8-43.0*shift    ;
	      if(j == 684)x=140.0-43.0*shift-61.6    ;
              y=85.8 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (685 <= j && j <= 713)
{
              x=140.0-44.0*shift-61.6 ;
	      y=85.8-2.8*(j-684) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end if*/
	}  /*end for*/
  ajGraphNewpage(graph, AJFALSE);
	return ;
}
 
/* @func endbot7full **********************************************************
**
******************************************************************************/

void  endbot7full(AjPGraph graph, const int tmcount,
		  const int ie,const int length,
		  char *strand,int *syms)  {
 
/*#include "common.inc" */ 
 	char  stran ;
	int    j,remain,sym ;
	float       x,y,shift ;
 
/*c draw the membrane layer at the bottom of the page */ 
	topoMoveTo(1.0,90.0) ;
	topoDraw(140.0,90.0) ;
	topoMoveTo(1.0,90.1) ;
	topoDraw(140.0,90.1) ;
	topoMoveTo(1.0,90.2) ;
	topoDraw(140.0,90.2) ;
 
/*c set shift value */ 
	shift=1.4 ;
 
/*c put in the first residue of the transmembrane helix for alignment */ 
/*c purposes */ 
	x=10.0 ;
	y=91.6 ;
	stran=strand[ie] ;
	sym=syms[ie] ;
	topoSymbol(x,y,stran,sym) ;
 
	remain=length-ie ;
 
/*c draw the chain back from the attachment point */ 
	for(j=1;j<=remain ;j++)
{
	   if(j <= 31)
{
	      x=10.0 ;
	      y=88.6-2.8*(j-1) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (32 <= j && j <= 33)
{
	      if(j == 32)x=10.0+shift    ;
	      if(j == 33)x=10.0+shift+2.8    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (34 <= j && j <= 62)
{
              x=10.0+2.0*shift+2.8 ;
	      y=1.8+2.8*(j-33) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (63 <= j && j <= 64)
{
	      if(j == 63)x=10.0+3.0*shift+2.8    ;
	      if(j == 64)x=10.0+3.0*shift+5.6    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (65 <= j && j <= 93)
{
	      x=10.0+5.6+4.0*shift ;
	      y=85.8-2.8*(j-64) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (94 <= j && j <= 95)
{
	      if(j == 94)x=10.0+5.6+5.0*shift    ;
	      if(j == 95)x=10.0+5.0*shift+8.4    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (96 <= j && j <= 124)
{
              x=10.0+6.0*shift+8.4 ;
	      y=1.8+2.8*(j-95) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (125 <= j && j <= 126)
{
	      if(j == 125)x=10.0+8.4+7.0*shift    ;
	      if(j == 126)x=10.0+7.0*shift+11.2    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (127 <= j && j <= 155)
{
              x=10.0+8.0*shift+11.2 ;
	      y=85.8-2.8*(j-126) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (156 <= j && j <= 157)
{
	      if(j == 156)x=10.0+11.2+9.0*shift    ;
	      if(j == 157)x=10.0+9.0*shift+14.0    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (158 <= j && j <= 186)
{
              x=10.0+10.0*shift+14.0 ;
	      y=1.8+2.8*(j-157) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (187 <= j && j <= 188)
{
	      if(j == 187)x=10.0+14.0+11.0*shift    ;
	      if(j == 188)x=10.00+11.0*shift+16.8    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (189 <= j && j <= 217)
{
              x=10.0+12.0*shift+16.8 ;
	      y=85.8-2.8*(j-188) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (218 <= j && j <= 219)
{
	      if(j == 218)x=10.0+16.8+13.0*shift    ;
	      if(j == 219)x=10.0+13.0*shift+19.6    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (220 <= j && j <= 248)
{
              x=10.0+14.0*shift+19.6 ;
	      y=1.8+2.8*(j-219) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (249 <= j && j <= 250)
{
	      if(j == 249)x=10.0+19.6+15.0*shift    ;
	      if(j == 250)x=10.0+15.0*shift+22.4    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (251 <= j && j <= 279)
{
              x=10.0+16.0*shift+22.4 ;
	      y=85.8-2.8*(j-250) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (280 <= j && j <= 281)
{
	      if(j == 280)x=10.0+22.4+17.0*shift    ;
	      if(j == 281)x=10.0+17.0*shift+25.2    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (282 <= j && j <= 310)
{
              x=10.0+18.0*shift+25.2 ;
	      y=1.8+2.8*(j-281) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (311 <= j && j <= 312)
{
	      if(j == 311)x=10.0+25.2+19.0*shift    ;
	      if(j == 312)x=10.0+19.0*shift+28.0    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (313 <= j && j <= 341)
{
              x=10.0+20.0*shift+28.0 ;
	      y=85.8-2.8*(j-312) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (342 <= j && j <= 343)
{
	      if(j == 342)x=10.0+28.0+21.0*shift    ;
	      if(j == 343)x=10.0+21.0*shift+30.8    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (344 <= j && j <= 372)
{
              x=10.0+22.0*shift+30.8 ;
	      y=1.8+2.8*(j-343) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (373 <= j && j <= 374)
{
	      if(j == 373)x=10.0+30.8+23.0*shift    ;
	      if(j == 374)x=10.0+23.0*shift+33.6    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (375 <= j && j <= 403)
{
              x=10.0+24.0*shift+33.6 ;
	      y=85.8-2.8*(j-374) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (404 <= j && j <= 405)
{
	      if(j == 404)x=10.0+33.6+25.0*shift    ;
	      if(j == 405)x=10.0+25.0*shift+36.4    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (406 <= j && j <= 434)
{
              x=10.0+26.0*shift+36.4 ;
	      y=1.8+2.8*(j-405) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (435 <= j && j <= 436)
{
	      if(j == 435)x=10.0+36.4+27.0*shift    ;
	      if(j == 436)x=10.0+27.0*shift+39.2    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (437 <= j && j <= 465)
{
              x=10.0+28.0*shift+39.2 ;
	      y=85.8-2.8*(j-436) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (466 <= j && j <= 467)
{
	      if(j == 466)x=10.0+39.2+29.0*shift    ;
	      if(j == 467)x=10.0+29.0*shift+42.0    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (468 <= j && j <= 496)
{
              x=10.0+30.0*shift+42.0 ;
	      y=1.8+2.8*(j-467) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (497 <= j && j <= 498)
{
	      if(j == 497)x=10.0+42.0+31.0*shift    ;
	      if(j == 498)x=10.0+31.0*shift+44.8    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (499 <= j && j <= 527)
{
              x=10.0+32.0*shift+44.8 ;
	      y=85.8-2.8*(j-498) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (528 <= j && j <= 529)
{
	      if(j == 528)x=10.0+44.8+33.0*shift    ;
	      if(j == 529)x=10.0+33.0*shift+47.6    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (530 <= j && j <= 558)
{
              x=10.0+34.0*shift+47.6 ;
	      y=1.8+2.8*(j-529) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (559 <= j && j <= 560)
{
	      if(j == 559)x=10.0+47.6+35.0*shift    ;
	      if(j == 560)x=10.0+35.0*shift+50.4    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (561 <= j && j <= 589)
{
              x=10.0+36.0*shift+50.4 ;
	      y=85.8-2.8*(j-560) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (590 <= j && j <= 591)
{
	      if(j == 590)x=10.0+50.4+37.0*shift    ;
	      if(j == 591)x=10.0+37.0*shift+53.2    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (592 <= j && j <= 620)
{
              x=10.0+38.0*shift+53.2 ;
	      y=1.8+2.8*(j-591) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (621 <= j && j <= 622)
{
	      if(j == 621)x=10.0+53.2+39.0*shift    ;
	      if(j == 622)x=10.0+39.0*shift+56.0    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (623 <= j && j <= 651)
{
              x=10.0+40.0*shift+56.0 ;
	      y=85.8-2.8*(j-622) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (652 <= j && j <= 653)
{
	      if(j == 652)x=10.0+56.0+41.0*shift    ;
	      if(j == 653)x=10.0+41.0*shift+58.8    ;
              y=1.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (654 <= j && j <= 682)
{
              x=10.0+42.0*shift+58.8 ;
	      y=1.8+2.8*(j-653) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (683 <= j && j <= 684)
{
	      if(j == 683)x=10.0+58.8+43.0*shift    ;
	      if(j == 684)x=10.0+43.0*shift+61.6    ;
              y=85.8 ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (685 <= j && j <= 713)
{
              x=10.0+44.0*shift+61.6 ;
	      y=85.8-2.8*(j-684) ;
	      stran=strand[ie+j] ;
	      sym=syms[ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end if*/
	}  /*end for*/
  ajGraphNewpage(graph, AJFALSE);
	return ;
}
 
/* @func endtop7 **************************************************************
**
******************************************************************************/

void  endtop7(AjPGraph graph, const int tmcount,
	      const int iend,const int length,
	      float *off,char *strand,int *syms)  {
 
/*#include "common.inc" */ 
 
	char  stran ;
	int    j,sym,remain;
	float       x = 0.0,y,wide,shift,offref ;
 
/*c this assumes 7 transmembrane sections */ 
	 
	remain=length-iend ;
	if(remain <= 9)shift=0.0 ;
	if(remain > 9)shift=1.4 ;
 
	wide=16.8 ;
	if(tmcount == 1)
{
	   *off=75.0 ;
	}
	else if (tmcount == 2)
{
	   *off=66.6 ;
	}
	else if (tmcount == 3)
{
	   *off=58.2 ;
	}
	else if (tmcount == 4)
{
	   *off=49.8 ;
	}
	else if (tmcount == 5)
{
	   *off=41.4 ;
	}
	else if (tmcount == 6)
{
	   *off=33.0 ;
	}
	else if (tmcount == 7)
{
	   *off=24.6 ;
	}
	else if (tmcount > 7)
{
	   *off=24.6 ;
	}  /*end if*/
 
	offref=*off ;
 
	if(tmcount <= 7)
{
	   *off=*off+(tmcount-1)*16.8 ;
	}
	else if (tmcount > 7)
{
	   *off=*off+(tmcount-7)*16.8 ;
	}  /*end if*/
 
/*c draw the chain back from the attachment point */ 
	for(j=1;j<=remain ;j++)
{
	   if(j <= 9)
{
	      x=*off ;
	      y=71.6+2.8*(j-1) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (10 <= j && j <= 11)
{
	      if(j == 10)x=*off+shift    ;
	      if(j == 11)x=*off+shift+2.8    ;
              y=71.6+25.2 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (12 <= j && j <= 18)
{
              x=*off+2.0*shift+2.8 ;
	      y=96.8-2.8*(j-11) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (19 <= j && j <= 20)
{
	      if(j == 19)x=*off+3.0*shift+2.8    ;
	      if(j == 20)x=*off+3.0*shift+5.6    ;
              y=74.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (21 <= j && j <= 27)
{
	      x=*off+5.6+4.0*shift ;
	      y=71.6+2.8*(j-19) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (28 <= j && j <= 29)
{
	      if(j == 28)x=*off+5.6+5.0*shift    ;
	      if(j == 29)x=*off+5.0*shift+8.4    ;
              y=96.8 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (30 <= j && j <= 36)
{
              x=*off+6.0*shift+8.4 ;
	      y=96.8-2.8*(j-29) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 36 && offref == 24.6)
{
	      x=*off+8.4+7.0*shift    ;
              y=74.4 ;
	      topoTerm(x,y) ;
	}
	else if (37 <= j && j <= 38)
{
	      if(j == 37)x=*off+8.4+7.0*shift    ;
	      if(j == 38)x=*off+7.0*shift+11.2    ;
              y=74.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (39 <= j && j <= 45)
{
              x=*off+8.0*shift+11.2 ;
	      y=71.6+2.8*(j-37) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (46 <= j && j <= 47)
{
	      if(j == 46)x=*off+11.2+9.0*shift    ;
	      if(j == 47)x=*off+9.0*shift+14.0    ;
              y=96.8 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (48 <= j && j <= 54)
{
              x=*off+10.0*shift+14.0 ;
	      y=96.8-2.8*(j-47) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 54 && offref == 33.0)
{
	      x=*off+14.0+11.0*shift    ;
              y=74.4 ;
	      topoTerm(x,y) ;
	}
	else if (55 <= j && j <= 56)
{
	      if(j == 55)x=*off+14.0+11.0*shift    ;
	      if(j == 56)x=*off+11.0*shift+16.8    ;
              y=74.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (57 <= j && j <= 63)
{
              x=*off+12.0*shift+16.8 ;
	      y=71.6+2.8*(j-55) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (64 <= j && j <= 65)
{
	      if(j == 64)x=*off+16.8+13.0*shift    ;
	      if(j == 65)x=*off+13.0*shift+19.6    ;
              y=96.8 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (66 <= j && j <= 72)
{
              x=*off+14.0*shift+19.6 ;
	      y=96.8-2.8*(j-65) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 72 && offref == 41.4)
{
	      x=*off+19.6+15.0*shift    ;
              y=74.4 ;
	      topoTerm(x,y) ;
	}
	else if (73 <= j && j <= 74)
{
	      if(j == 73)x=*off+19.6+15.0*shift    ;
	      if(j == 74)x=*off+15.0*shift+22.4    ;
              y=74.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (75 <= j && j <= 81)
{
              x=*off+16.0*shift+22.4 ;
	      y=71.6+2.8*(j-73) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 81 && offref == 49.8)
{
	      x=*off+22.4+17.0*shift    ;
              y=96.8 ;
	      topoTerm(x,y) ;
	}
	else if (82 <= j && j <= 83)
{
	      if(j == 82)x=*off+22.4+17.0*shift    ;
	      if(j == 83)x=*off+17.0*shift+25.2    ;
              y=96.8 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (84 <= j && j <= 90)
{
              x=*off+18.0*shift+25.2 ;
	      y=96.8-2.8*(j-83) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (j > 90 && offref == 58.2)
{
	      x=*off+25.2+19.0*shift ;
	      y=74.4 ;
	      topoTerm(x,y) ;
	      goto CAA ;
	}
	else if (remain > 90 && offref == 58.2)
{
	      x=*off+25.2+19.0*shift    ;
              y=74.4 ;
	      topoTerm(x,y) ;
	}
	else if (91 <= j && j <= 92)
{
	      if(j == 91)x=*off+25.2+19.0*shift    ;
	      if(j == 92)x=*off+19.0*shift+28.0    ;
              y=74.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (93 <= j && j <= 99)
{
              x=*off+20.0*shift+28.0 ;
	      y=71.6+2.8*(j-91) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 99 && offref == 66.6)
{
	      x=*off+28.0+21.0*shift    ;
              y=96.8 ;
	      topoTerm(x,y) ;
	}
	else if (100 <= j && j <= 101)
{
	      if(j == 100)x=*off+28.0+21.0*shift    ;
	      if(j == 101)x=*off+21.0*shift+30.8    ;
              y=96.8 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (102 <= j && j <= 108)
{
              x=*off+22.0*shift+30.8 ;
	      y=96.8-2.8*(j-101) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 108 && offref == 75.0)
{
	      x=*off+23.0*shift+30.8    ;
              y=74.4 ;
	      topoTerm(x,y) ;
	   }  /*end if*/
 
	}  /*end for*/
CAA:   
	return ;
	}
 
/* @func topoTerm *************************************************************
**
******************************************************************************/

void  topoTerm(const float x,const float y)  {
 
	topoMoveTo(x,y+.7) ;
	topoDraw(x,y-.7) ;
	topoMoveTo(x-.7,y+.7) ;
	topoDraw(x+.7,y-.7) ;
	topoMoveTo(x-.7,y) ;
	topoDraw(x+.7,y) ;
	topoMoveTo(x-.7,y-.7) ;
	topoDraw(x+.7,y+.7) ;
	return ;
	}
 
/* @func startbot7 ************************************************************
**
******************************************************************************/

void  startbot7(AjPGraph graph, const int tmcount,const int istart,float *off,
		char *strand,int *syms)  {
 
/*#include "common.inc" */ 
 
	char  stran ;
	int    j,sym ;
	float       x = 0.0,y ;
	float       wide,shift ;
 
/*c this assumes 7 transmembrane sections */ 
	if(istart <= 9)shift=0.0 ;
	if(istart > 9)shift=1.4 ;
 
	wide=16.8 ;
	if(tmcount == 1)
{
	   *off=75.0 ;
	}
	else if (tmcount == 2)
{
	   *off=66.6 ;
	}
	else if (tmcount == 3)
{
	   *off=58.2 ;
	}
	else if (tmcount == 4)
{
	   *off=49.8 ;
	}
	else if (tmcount == 5)
{
	   *off=41.4 ;
	}
	else if (tmcount == 6)
{
	   *off=33.0 ;
	}
	else if (tmcount == 7)
{
	   *off=24.6 ;
	}
	else if (tmcount > 7)
{
	   *off=24.6 ;
	} 	 /*end if*/
 
/*c draw the chain back from the attachment point */ 
	for(j=1;j<=istart-1 ;j++)
{
	   if(j <= 12)
{
	      x=*off ;
	      y=37.2-2.8*(j-1) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (13 <= j && j <= 14)
{
	      if(j == 13)x=*off-shift    ;
	      if(j == 14)x=*off-shift-2.8    ;
              y=3.6 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (15 <= j && j <= 24)
{
              x=*off-2.0*shift-2.8 ;
	      y=3.6+2.8*(j-14) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (25 <= j && j <= 26)
{
	      if(j == 25)x=*off-3.0*shift-2.8    ;
	      if(j == 26)x=*off-3.0*shift-5.6    ;
              y=34.6 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (27 <= j && j <= 36)
{
	      x=*off-4.0*shift-5.6 ;
	      y=34.6-2.8*(j-26) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (37 <= j && j <= 38)
{
	      if(j == 37)x=*off-5.0*shift-5.6    ;
	      if(j == 38)x=*off-5.0*shift-8.4    ;
              y=3.6 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (39 <= j && j <= 48)
{
              x=*off-6.0*shift-8.4 ;
	      y=3.6+2.8*(j-38) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (istart > 48 && tmcount >= 7)
{
	      x=*off-7.0*shift-8.4 ;
	      y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (49 <= j && j <= 50)
{
	      if(j == 49)x=*off-8.4-7.0*shift    ;
	      if(j == 50)x=*off-7.0*shift-11.2    ;
              y=34.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (51 <= j && j <= 60)
{
              x=*off-8.0*shift-11.2 ;
	      y=34.4-2.8*(j-50) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (61 <= j && j <= 62)
{
	      if(j == 61)x=*off-11.2-9.0*shift    ;
	      if(j == 62)x=*off-9.0*shift-14.0    ;
              y=3.6 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (63 <= j && j <= 72)
{
              x=*off-10.0*shift-14.0 ;
	      y=3.6+2.8*(j-62) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 72 && *off == 33.0)
{
	      x=*off-11.0*shift-14.0 ;
	      y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (73 <= j && j <= 74)
{
	      if(j == 73)x=*off-14.0-11.0*shift    ;
	      if(j == 74)x=*off-11.0*shift-16.8    ;
              y=34.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (75 <= j && j <= 84)
{
              x=*off-12.0*shift-16.8 ;
	      y=34.4-2.8*(j-74) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (85 <= j && j <= 86)
{
	      if(j == 85)x=*off-16.8-13.0*shift    ;
	      if(j == 86)x=*off-13.0*shift-19.6    ;
              y=3.6 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (87 <= j && j <= 96)
{
              x=*off-14.0*shift-19.6 ;
	      y=3.6+2.8*(j-86) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 96 && *off == 41.4)
{
	      x=*off-15.0*shift-19.6 ;
	      y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (97 <= j && j <= 98)
{
	      if(j == 97)x=*off-19.6-15.0*shift    ;
	      if(j == 98)x=*off-15.0*shift-22.4    ;
              y=34.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (99 <= j && j <= 108)
{
              x=*off-16.0*shift-22.4 ;
	      y=34.4-2.8*(j-98) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 108 && *off == 49.8)
{
	      x=*off-17.0*shift-22.4 ;
	      y=3.6 ;
	      topoTerm(x,y) ;
	}
	else if (109 <= j && j <= 110)
{
	      if(j == 109)x=*off-22.4-17.0*shift    ;
	      if(j == 110)x=*off-17.0*shift-25.2    ;
              y=3.6 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (111 <= j && j <= 120)
{
              x=*off-18.0*shift-25.2 ;
	      y=3.6+2.8*(j-110) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 120 && *off == 58.2)
{
	      x=*off-19.0*shift-25.2 ;
	      y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (121 <= j && j <= 122)
{
	      if(j == 121)x=*off-25.2-19.0*shift    ;
	      if(j == 122)x=*off-19.0*shift-28.0    ;
              y=34.4 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (123 <= j && j <= 132)
{
              x=*off-20.0*shift-28.0 ;
	      y=34.4-2.8*(j-122) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (133 <= j && j <= 134)
{
	      if(j == 133)x=*off-28.0-21.0*shift    ;
	      if(j == 134)x=*off-21.0*shift-30.8    ;
              y=3.6 ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym)	       ;
	}
	else if (135 <= j && j <= 144)
{
              x=*off-22.0*shift-30.8 ;
	      y=3.6+2.8*(j-134) ;
	      stran=strand[istart-j] ;
	      sym=syms[istart-j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (istart > 144 && *off == 66.6)
{
	      x=*off-23.0*shift-30.8 ;
	      y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (istart > 144 && *off == 75.0)
{
	      x=*off-23.0*shift-30.8 ;
	      y=34.4 ;
	      topoTerm(x,y) ;
	   }  /*end if*/
	}  /*end for*/
	return ;
	}
 
/* @func endbot7 **************************************************************
**
******************************************************************************/

void  endbot7(AjPGraph graph, const int tmcount,
	      const int iend,const int length,
	      float *off,char *strand,int *syms)  {
 
/*#include "common.inc" */ 
 
	char stran ;
	int    j,remain ;
	int    sym ;
	float       x = 0.0,y,off0 ;
	float       shift ;
 
        remain=length-iend ;
	if(remain <= 12)shift=0.0 ;
	if(remain > 12)shift=1.4 ;
 
/*c draw the chain back from the attachment point */ 
	if(tmcount <= 7)
{
	   off0=*off+(tmcount-1)*16.8 ;
	}
	else if (tmcount > 7)
{
	   off0=*off+(tmcount-7)*16.8 ;
	}  /*end if*/
	for(j=1;j<=remain ;j++)
{
	   if(j <= 12)
{
	      x=off0 ;
	      y=37.2-2.8*(j-1) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (13 <= j && j <= 14)
{
	      if(j == 13)x=off0+shift    ;
	      if(j == 14)x=off0+2.8+shift    ;
              y=3.6 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (15 <= j && j <= 24)
{
              x=off0+2.0*shift+2.8 ;
	      y=3.6+2.8*(j-14) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (25 <= j && j <= 26)
{
	      if(j == 25)x=off0+3.0*shift+2.8 ;
	      if(j == 26)x=off0+3.0*shift+5.6    ;
              y=34.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (27 <= j && j <= 36)
{
	      x=off0+4.0*shift+5.6 ;
	      y=34.4-2.8*(j-26) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (37 <= j && j <= 38)
{
	      if(j == 37)x=off0+5.0*shift+5.6 ;
	      if(j == 38)x=off0+5.0*shift+8.4 ;
              y=3.6 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (39 <= j && j <= 48)
{
             x=off0+6.0*shift+8.4 ;
	      y=3.6+2.8*(j-38) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 48 && tmcount == 7)
{
	      x=off0+7.0*shift+8.4 ;
              y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (remain > 48 && tmcount == 13)
{
	      x=off0+7.0*shift+8.4 ;
              y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (49 <= j && j <= 50)
{
	      if(j == 49)x=off0+7.0*shift+8.4    ;
	      if(j == 50)x=off0+7.0*shift+11.2    ;
             y=34.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (51 <= j && j <= 60)
{
              x=off0+8.0*shift+11.2 ;
	      y=34.4-2.8*(j-50) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (61 <= j && j <= 62)
{
	      if(j == 61)x=off0+9.0*shift+11.2    ;
	      if(j == 62)x=off0+9.0*shift+14.0    ;
             y=3.6 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (63 <= j && j <= 72)
{
              x=off0+10.0*shift+14.0 ;
	      y=3.6+2.8*(j-62) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 72 && *off == 66.6)
{
	      x=off0+11.0*shift+14.0 ;
              y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (73 <= j && j <= 74)
{
	      if(j == 73)x=off0+11.0*shift+14.0    ;
	      if(j == 74)x=off0+11.0*shift+16.8    ;
             y=34.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (75 <= j && j <= 84)
{
              x=off0+12.0*shift+16.8 ;
	      y=34.4-2.8*(j-74) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (85 <= j && j <= 86)
{
	      if(j == 85)x=off0+13.0*shift+16.8    ;
	      if(j == 86)x=off0+13.0*shift+19.6    ;
              y=3.6 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (87 <= j && j <= 96)
{
              x=off0+14.0*shift+19.6 ;
	      y=3.6+2.8*(j-86) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 96 && *off == 41.4)
{
	      x=off0+15.0*shift+19.6 ;
              y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (97 <= j && j <= 98)
{
	      if(j == 97)x=off0+15.0*shift+19.6 ;
	      if(j == 98)x=off0+15.0*shift+22.4    ;
              y=34.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (99 <= j && j <= 108)
{
	      x=off0+16.0*shift+22.4 ;
	      y=34.4-2.8*(j-98) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 108 && *off == 49.8)
{
	      x=off0+5.0*shift+5.6 ;
              y=3.6 ;
	      topoTerm(x,y) ;
	}
	else if (109 <= j && j <= 110)
{
	      if(j == 109)x=off0+17.0*shift+22.4 ;
	      if(j == 110)x=off0+17.0*shift+25.2 ;
              y=3.6 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (111 <= j && j <= 120)
{
             x=off0+18.0*shift+25.2 ;
	      y=3.6+2.8*(j-110) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 120 && *off == 58.2)
{
	      x=off0+19.0*shift+25.2 ;
              y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (121 <= j && j <= 122)
{
	      if(j == 121)x=off0+19.0*shift+25.2    ;
	      if(j == 122)x=off0+19.0*shift+28.0    ;
             y=34.4 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (123 <= j && j <= 132)
{
              x=off0+20.0*shift+28.0 ;
	      y=34.4-2.8*(j-122) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (133 <= j && j <= 134)
{
	      if(j == 133)x=off0+21.0*shift+28.0    ;
	      if(j == 134)x=off0+21.0*shift+30.8    ;
             y=3.6 ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (135 <= j && j <= 144)
{
              x=off0+22.0*shift+30.8 ;
	      y=3.6+2.8*(j-134) ;
	      stran=strand[iend+j] ;
	      sym=syms[iend+j] ;
	      topoSymbol(x,y,stran,sym) ;
	}
	else if (remain > 144 && *off == 66.6)
{
	      x=off0+23.0*shift+30.8 ;
              y=34.4 ;
	      topoTerm(x,y) ;
	}
	else if (remain > 144 && *off == 75.0)
{
	      x=off0+23.0*shift+30.8 ;
              y=34.4 ;
	      topoTerm(x,y) ;
	   }  /*end if*/
	}  /*end for*/
/*c	off=store */ 
	return ;
	}
 
/* @func chaindown ************************************************************
**
******************************************************************************/

void  chaindown(const int i,const int istart,const int nchain,
		float *off,const int page,const float wide,
		float *hold,char *strand,int *syms)
{
 
    float       x3 = 0.0 ;
    ajint j;

    j = i-1;
    /*c draw down the membrane section */ 
    x3=*off+(j%7)*wide;
    if(7 <= nchain && nchain <= 11)
    {
	(void) sheetdown(page,i,istart,nchain,off,hold, 
			 wide,strand,syms) ;
    }  /*end if*/
    if(12 <= nchain && nchain <= 16)
    {
	(void) chain0down(page,i,istart,nchain,off,hold, 
			  wide,strand,syms) ;
    }  /*end if*/
    if(17 <= nchain && nchain <= 19)
    {
	(void) chain1down(page,i,istart,nchain,off,hold, 
			  wide,strand,syms) ;
    }  /*end if*/
    if(20 <= nchain && nchain <= 24)
    {
	(void) chain2down(page,i,istart,nchain,off,hold, 
			  wide,strand,syms) ;
    }  /*end if*/
    if(25 <= nchain && nchain <= 29)
    {
	(void) chain3down(page,i,istart,nchain,off,hold, 
			  wide,strand,syms) ;
    }  /*end if*/
    *hold=x3 ;
    return ;
}

/* @func chainup **************************************************************
**
******************************************************************************/

void  chainup(const int i,const int istart,const int nchain,
	      float *off,const int page,const float wide,
	      float *hold,char *strand,int *syms)
{
    float       x3 = 0.0;
    ajint j;

    j = i-1;
/*c draw up the membrane section */ 
    x3=*off+(j%7)*wide;
    if(7 <= nchain && nchain <= 11)
    {
	(void) sheetup(page,i,istart,nchain,off,hold,wide, 
		       strand,syms) ;
    }  /*end if*/
    if(12 <= nchain && nchain <= 16)
    {
	(void) chain0up(page,i,istart,nchain,off,hold,wide, 
			strand,syms) ;
    }  /*end if*/
    if(17 <= nchain && nchain <= 19)
    {
	(void) chain1up(page,i,istart,nchain,off,hold,wide, 
			strand,syms) ;
    }  /*end if*/
    if(20 <= nchain && nchain <= 24)
    {
	(void) chain2up(page,i,istart,nchain,off,hold,wide, 
			strand,syms) ;
    }  /*end if*/
    if(25 <= nchain && nchain <= 29)
    {
	(void) chain3up(page,i,istart,nchain,off,hold,wide, 
			strand,syms) ;
    }  /*end if*/  
    *hold=x3 ;
    return ;
}
 
/* @func chain0down ***********************************************************
**
******************************************************************************/

void  chain0down(const int page,const int i,const int istart,
		 const int nchain,float *off,float *hold,
		 const float wide,char *strand,int *syms)
{
    int   k;
    float      x = 0.0 ;
    ajint j;

    j = i-1;
    x=*off+(j%7)*wide;
    k=1 ;
    /*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1s(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain == 12)
    {
	(void) ball2down(istart,2,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 2 && nchain >= 13)
    {
	(void) ball3sdown(istart,2,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 4 && nchain == 12)
    {
	(void) ball2down(istart,4,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 5 && nchain < 16)
    {
	(void) ball2down(istart,5,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 5 && nchain == 16)
    {
	(void) ball3sdown(istart,5,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 6 && nchain == 12)
    {
	(void) ball2down(istart,6,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 7 && nchain == 13)
    {
	(void) ball2down(istart,7,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 7 && nchain > 13)
    {
	(void) ball3sdown(istart,7,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 8 && nchain == 12)
    {
	(void) ball2down(istart,8,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 8 && nchain == 16)
    {
	(void) ball3sdown(istart,8,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 9)
    {
	(void) ball2down(istart,9,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 10 && nchain == 12)
    {
	(void) ball2down(istart,10,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 10 && nchain > 13)
    {
	(void) ball2adown(istart,10,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 11 && nchain == 13)
    {
	(void) ball2down(istart,11,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 11 && nchain == 16)
    {
	(void) ball2adown(istart,11,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 12 && nchain == 12)
    {
	(void) ball1s(istart,12,x,strand,syms) ;
	goto DAA ;
    }  /*end if*/
    if(k == 12 && nchain == 14)
    {
	(void) ball2down(istart,12,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 12 && nchain == 15)
    {
	(void) ball3sdown(istart,12,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 13 && nchain == 13)
    {
	(void) ball1s(istart,13,x,strand,syms) ;
	goto DAA ;
    }  /*end if*/
    if(k == 13 && nchain == 16)
    {
	(void) ball3sdown(istart,13,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 14 && nchain == 14)(void) ball1s(istart,14,x,strand,syms) ;
    if(k == 15 && nchain == 15)(void) ball1s(istart,15,x,strand,syms) ;
    if(k == 16 && nchain == 16)(void) ball1s(istart,16,x,strand,syms) ;
 DAA:	*hold=x ;
    return ;
}
 
/* @func sheetdown ************************************************************
**
******************************************************************************/

void  sheetdown(const int page,const int i,const int istart,
		const int nchain,float *off,
		float *hold,const float wide,char *strand,int *syms)
{
 
    int   k ;
    float      x = 0.0;
    ajint j;

    j = i-1;
    x=*off+(j%7)*wide ;

    k=1 ;
    /*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1s(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain == 7)
    {
	(void) ball1down7(istart,2,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain >= 8)
    {
	(void) ball2sdown(istart,2,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 3 && nchain == 7)
    {
	(void) ball1down7(istart,3,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 4 && nchain == 7)
    {
	(void) ball1down7(istart,4,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 4 && nchain <= 10)
    {
	(void) ball1down(istart,4,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 4 && nchain == 11)
    {
	(void) ball2sdown(istart,4,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 5 && nchain == 7)
    {
	(void) ball1down7(istart,5,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 5 && nchain == 8)
    {
	(void) ball1down(istart,5,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 5 && nchain > 8)
    {
	(void) ball2sdown(istart,5,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 6 && nchain == 7)
    {
	(void) ball1down7(istart,6,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 6 && nchain == 8)
    {
	(void) ball1down(istart,6,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 6 && nchain > 8)
    {
	(void) ball2sdown(istart,6,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 7 && nchain == 7)
    {
	(void) ball1s(istart,7,x,strand,syms) ;
	goto DAA ;
    }  /*end if*/
    if(k == 7 && nchain == 8)
    {
	(void) ball1bdown(istart,7,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 7 && nchain > 8)
    {
	(void) ball1down(istart,7,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 8 && nchain == 8)
    {
	(void) ball1s(istart,8,x,strand,syms) ;
	goto DAA ;
    }  /*end if*/
    if(k == 8 && nchain == 9)
    {
	(void) ball1bdown(istart,8,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 8 && nchain == 10)
    {
	(void) ball2sdown(istart,8,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 8 && nchain == 11)
    {
	(void) ball1down(istart,8,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 9 && nchain == 9)
    {
	(void) ball1s(istart,9,x,strand,syms) ;
	goto DAA ;
    }  /*end if*/
    if(k == 9 && nchain == 11)
    {
	(void) ball2sdown(istart,9,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 10 && nchain == 10)(void) ball1s(istart,10,x,strand,syms) ;
    if(k == 11 && nchain == 11)(void) ball1s(istart,11,x,strand,syms) ;
 DAA:	*hold=x ;
    return ;
}
 
/* @func sheetup **************************************************************
**
******************************************************************************/

void  sheetup(const int page,const int i,const int istart,
	      const int nchain,float *off,float *hold,
	      const float wide,char *strand,int *syms)
{
    int   k ;
    float      x = 0.0;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1su(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain == 7)
    {
	(void) ball1up7(istart,2,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain >= 8)
    {
	(void) ball2sup(istart,2,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 3 && nchain == 7)
    {
	(void) ball1up7(istart,3,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 4 && nchain == 7)
    {
	(void) ball1up7(istart,4,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 4 && nchain <= 10)
    {
	(void) ball1up(istart,4,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 4 && nchain == 11)
    {
	(void) ball2sup(istart,4,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 5 && nchain == 7)
    {
	(void) ball1up7(istart,5,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 5 && nchain == 8)
    {
	(void) ball1up(istart,5,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 5 && nchain > 8)
    {
	(void) ball2sup(istart,5,x,strand,syms) ;
	   k=k+2 ;
    }  /*end if*/
    if(k == 6 && nchain == 7)
    {
	(void) ball1up7(istart,6,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 6 && nchain == 8)
    {
	(void) ball1up(istart,6,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 6 && nchain > 8)
    {
	(void) ball2sup(istart,6,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 7 && nchain == 7)(void) ball1su(istart,7,x,strand,syms) ;
    if(k == 7 && nchain == 8)
    {
	(void) ball1bup(istart,7,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 7 && nchain > 8)
    {
	(void) ball1up(istart,7,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 8 && nchain == 8)(void) ball1su(istart,8,x,strand,syms) ;
    if(k == 8 && nchain == 9)
    {
	(void) ball1bup(istart,8,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 8 && nchain == 10)
    {
	(void) ball2sup(istart,8,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 8 && nchain == 11)
    {
	(void) ball1up(istart,8,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 9 && nchain == 9)(void) ball1su(istart,9,x,strand,syms) ;
    if(k == 9 && nchain == 11)
    {
	(void) ball2sup(istart,9,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 10 && nchain == 10)(void) ball1su(istart,10,x,strand,syms) ;
    if(k == 11 && nchain == 11)(void) ball1su(istart,11,x,strand,syms) ;
    *hold=x ;
    return ;
}
 
/* @func chain0up *************************************************************
**
******************************************************************************/

void  chain0up(const int page,const int i,const int istart,
	       const int nchain,float *off,float *hold,
	       const float wide,char *strand,int *syms)
{
 
    int   k ;
    float      x = 0.0;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1su(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain == 12)
    {
	(void) ball2up(istart,2,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 2 && nchain >= 13)
    {
	(void) ball3sup(istart,2,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 4 && nchain == 12)
    {
	(void) ball2up(istart,4,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 5 && nchain < 16)
    {
	(void) ball2up(istart,5,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 5 && nchain == 16)
    {
	(void) ball3sup(istart,5,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 6 && nchain == 12)
    {
	(void) ball2up(istart,6,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 7 && nchain == 13)
    {
	(void) ball2up(istart,7,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 7 && nchain > 13)
    {
	(void) ball3sup(istart,7,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 8 && nchain < 16)
    {
	(void) ball2up(istart,8,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 8 && nchain == 16)
    {
	(void) ball3sup(istart,8,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 9)
    {
	(void) ball2up(istart,9,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 10 && nchain == 12)
    {
	(void) ball2up(istart,10,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 10 && nchain > 13)
    {
	(void) ball2aup(istart,10,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 11 && nchain == 13)
    {
	(void) ball2up(istart,11,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 11 && nchain == 16)
    {
	(void) ball2aup(istart,11,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 12 && nchain == 12)
    {
	(void) ball1su(istart,12,x,strand,syms) ;
	goto DAA ;
    }  /*end if*/
    if(k == 12 && nchain == 14)
    {
	(void) ball2up(istart,12,x,strand,syms) ;
	k=k+2 ;
    }  /*end if*/
    if(k == 12 && nchain == 15)
    {
	(void) ball3sup(istart,12,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 13 && nchain == 13)
    {
	(void) ball1su(istart,13,x,strand,syms) ;
	goto DAA ;
    }  /*end if*/
    if(k == 13 && nchain == 16)
    {
	(void) ball3sup(istart,13,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 14 && nchain == 14)(void) ball1su(istart,14,x,strand,syms) ;
    if(k == 15 && nchain == 15)(void) ball1su(istart,15,x,strand,syms) ;
    if(k == 16 && nchain == 16)(void) ball1su(istart,16,x,strand,syms) ;
 DAA:	*hold=x ;
    return ;
}
 
/* @func chain1down ***********************************************************
**
******************************************************************************/

void  chain1down(const int page,const int i,const int istart,
		 const int nchain,float *off,float *hold,
		 const float wide,char *strand,int *syms)
{
    int   k ;
    float      x = 0.0 ;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain == 17)
    {
	(void) ball3down(istart,2,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 2 && nchain > 17)
    {
	(void) ball2topdown(istart,2,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 3 && nchain > 17)
    {
	(void) ball3down(istart,3,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 5 && nchain == 17)
    {
	(void) ball3down(istart,5,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 6 && nchain > 17)
    {
	(void) ball3down(istart,6,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 8 && nchain == 17)
    {
	(void) ball3down(istart,8,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 9 && nchain > 17)
    {
	(void) ball3down(istart,9,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 11 && nchain == 17)
    {
	(void) ball3down(istart,11,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 12 && nchain > 17)
    {
	(void) ball3down(istart,12,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 14 && nchain == 17)
    {
	(void) ball3down(istart,14,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 15 && nchain > 17)
    {
	(void) ball3down(istart,15,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 17 && nchain == 17)(void) ball1(istart,17,x,strand,syms) ;
    if(k == 18 && nchain == 18)(void) ball1(istart,18,x,strand,syms) ;
    if(k == 18 && nchain > 18)
    {
	(void) ball2botdown(istart,18,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 19 && nchain == 19)(void) ball1(istart,19,x,strand,syms) ;
    *hold=x ;
    return ;
}
 
/* @func chain1up *************************************************************
**
******************************************************************************/

void  chain1up(const int page,const int i,const int istart,
	       const int nchain,float *off,float *hold,
	       const float wide,char *strand,int *syms)
{
    int   k;
    float      x = 0.0;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1u(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain == 17)
    {
	(void) ball3up(istart,2,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 2 && nchain > 17)
    {
	(void) ball2topup(istart,2,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 3 && nchain > 17)
    {
	(void) ball3up(istart,3,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 5 && nchain == 17)
    {
	(void) ball3up(istart,5,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 6 && nchain > 17)
    {
	(void) ball3up(istart,6,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 8 && nchain == 17)
    {
	(void) ball3up(istart,8,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 9 && nchain > 17)
    {
	(void) ball3up(istart,9,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 11 && nchain == 17)
    {
	(void) ball3up(istart,11,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 12 && nchain > 17)
    {
	(void) ball3up(istart,12,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 14 && nchain == 17)
    {
	(void) ball3up(istart,14,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 15 && nchain > 17)
    {
	(void) ball3up(istart,15,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 17 && nchain == 17)
    {
	(void) ball1u(istart,17,x,strand,syms) ;
    }  /*end if*/
    if(k == 18 && nchain == 18)
    {
	(void) ball1u(istart,18,x,strand,syms) ;
    }  /*end if*/
    if(k == 18 && nchain > 18)
    {
	(void) ball2botup(istart,18,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 19 && nchain == 19)
    {
	(void) ball1u(istart,19,x,strand,syms) ;
    }  /*end if*/
    *hold=x ;
    return ;
}
 
/* @func chain2down ***********************************************************
**
******************************************************************************/

void  chain2down(const int page,const int i,const int istart,
		 const int nchain,float *off,float *hold,
		 const float wide,char *strand,int *syms)
{
    int   k ;
    float      x = 0.0;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
/*c	if(i == 1)x=x+2.8 */ 
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain > 22)
    {
	(void) ball2topdown(istart,2,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 2)
    {
	(void) ball4down(istart,2,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 3)
    {
	(void) ball4down(istart,3,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 6 && nchain == 20)
    {
	(void) ball3down(istart,6,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 6 && nchain > 20)
    {
	(void) ball4down(istart,6,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 7)
    {
	(void) ball4down(istart,7,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 9 && nchain == 20)
    {
	(void) ball4down(istart,9,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 10)
    {
	(void) ball4down(istart,10,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 11)
    {
	(void) ball4down(istart,11,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 13)
    {
	(void) ball3down(istart,13,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 14 && nchain == 21)
    {
	(void) ball3down2(istart,14,x,strand,syms) ;
	k=k+3 ;
    }
    else if (k == 14)
    {
	(void) ball4down(istart,14,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 15)
    {
	(void) ball4down(istart,15,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 16)
    {
	(void) ball4down(istart,16,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 17)
    {
	(void) ball4down(istart,17,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 18)
    {
	(void) ball4down(istart,18,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 19)
    {
	(void) ball4down(istart,19,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 20)(void) ball1(istart,20,x,strand,syms) ;
    if(k == 21)(void) ball1(istart,21,x,strand,syms) ;
    if(k == 22)(void) ball1(istart,22,x,strand,syms) ;
    if(k == 23 && nchain == 24)
    {
	(void) ball2botdown(istart,23,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 23)
    {
	(void) ball1(istart,23,x,strand,syms) ;
    }  /*end if*/
    if(k == 24 && nchain == 24)
    {
	(void) ball1(istart,24,x,strand,syms) ;
    }  /*end if*/
    *hold=x ;
    return ;
}
 
/* @func chain2up *************************************************************
**
******************************************************************************/

void  chain2up(const int page,const int i,const int istart,
	       const int nchain,float *off,float *hold,
	       const float wide,char *strand,int *syms)
{
    int   k ;
    float      x = 0.0;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1u(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain > 22)
    {
	(void) ball2topup(istart,2,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 2)
    {
	(void) ball4up(istart,2,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 3)
    {
	(void) ball4up(istart,3,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 6 && nchain == 20)
    {
	(void) ball3up(istart,6,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 6 && nchain > 20)
    {
	(void) ball4up(istart,6,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 7)
    {
	(void) ball4up(istart,7,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 9 && nchain == 20)
    {
	(void) ball4up(istart,9,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 10)
    {
	(void) ball4up(istart,10,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 11)
    {
	(void) ball4up(istart,11,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 13)
    {
	(void) ball3up(istart,13,x,strand,syms) ;
	k=k+3 ;
    }  /*end if*/
    if(k == 14 && nchain == 21)
    {
	(void) ball3up2(istart,14,x,strand,syms) ;
	k=k+3 ;
    }
    else if (k == 14)
    {
	(void) ball4up(istart,14,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 15)
    {
	(void) ball4up(istart,15,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 16)
    {
	(void) ball4up(istart,16,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 17)
    {
	(void) ball4up(istart,17,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 18)
    {
	(void) ball4up(istart,18,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 19)
    {
	(void) ball4up(istart,19,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 20)(void) ball1u(istart,20,x,strand,syms) ;
    if(k == 21)(void) ball1u(istart,21,x,strand,syms) ;
    if(k == 22)(void) ball1u(istart,22,x,strand,syms) ;
    if(k == 23 && nchain == 24)
    {
	(void) ball2botup(istart,23,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 23)
    {
	(void) ball1u(istart,23,x,strand,syms) ;
    }  /*end if*/
    if(k == 24 && nchain == 24)
    {
	(void) ball1u(istart,24,x,strand,syms) ;
    }  /*end if*/
    *hold=x ;
    return ;
}
 
/* @func chain3down ***********************************************************
**
******************************************************************************/

void  chain3down(const int page,const int i,const int istart,
		 const int nchain,float *off,
		 float *hold,const float wide,char *strand,int *syms)
{

    int   k;
    float      x = 0.0;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain > 27)
    {
	(void) ball2topdown(istart,2,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 2)
    {
	(void) ball5down(istart,2,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 3)
    {
	(void) ball5down(istart,3,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 7 && nchain == 25)
    {
	(void) ball4down(istart,7,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 7 && nchain > 25)
    {
	(void) ball5down(istart,7,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 8)
    {
	(void) ball5down(istart,8,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 11)
    {
	(void) ball5down(istart,11,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 12)
    {
	(void) ball5down(istart,12,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 13)
    {
	(void) ball5down(istart,13,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 16)
    {
	(void) ball4bdown(istart,16,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 17 && nchain == 26)
    {
	(void) ball4bdown(istart,17,x,strand,syms) ;
	k=k+4 ;
    }
    else if (k == 17)
    {
	(void) ball5down(istart,17,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 18)
    {
	(void) ball5down(istart,18,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 20)
    {
	(void) ball5down(istart,20,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 21)
    {
	(void) ball5down(istart,21,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 22)
    {
	(void) ball5down(istart,22,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 23)
    {
	(void) ball5down(istart,23,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 25)(void) ball1(istart,25,x,strand,syms) ;
    if(k == 26)(void) ball1(istart,26,x,strand,syms) ;
    if(k == 27)(void) ball1(istart,27,x,strand,syms) ;
    if(k == 28 && nchain == 29)
    {
	(void) ball2botdown(istart,28,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 28)
    {
	(void) ball1(istart,28,x,strand,syms) ;
    }  /*end if*/
    if(k == 29 && nchain == 29)
    {
	(void) ball1(istart,29,x,strand,syms) ;
    }  /*end if*/
    *hold=x ;
    return ;
}
 
/* @func chain3up *************************************************************
**
******************************************************************************/

void  chain3up(const int page,const int i,const int istart,
	       const int nchain,float *off,
	       float *hold,const float wide,char *strand,int *syms)
{
    int   k;
    float      x = 0.0;
    int j;

    j = i-1;
    x=*off+(j%7)*wide ;
/*c	if(i == 1)x=x+2.8 */ 
    k=1 ;
/*c put in the structure */ 
    if(k == 1)
    {
	(void) ball1u(istart,1,x,strand,syms) ;
	k=k+1 ;
    }  /*end if*/
    if(k == 2 && nchain > 27)
    {
	(void) ball2topup(istart,2,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 2)
    {
	(void) ball5up(istart,2,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 3)
    {
	(void) ball5up(istart,3,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 7 && nchain == 25)
    {
	(void) ball4up(istart,7,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 7 && nchain > 25)
    {
	(void) ball5up(istart,7,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 8)
    {
	(void) ball5up(istart,8,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 11)
    {
	(void) ball5up(istart,11,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 12)
    {
	(void) ball5up(istart,12,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 13)
    {
	(void) ball5up(istart,13,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 16)
    {
	(void) ball4bup(istart,16,x,strand,syms) ;
	k=k+4 ;
    }  /*end if*/
    if(k == 17 && nchain == 26)
    {
	(void) ball4bup(istart,17,x,strand,syms) ;
	k=k+4 ;
    }
    else if (k == 17)
    {
	(void) ball5up(istart,17,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 18)
    {
	(void) ball5up(istart,18,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 20)
    {
	(void) ball5up(istart,20,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 21)
    {
	(void) ball5up(istart,21,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 22)
    {
	(void) ball5up(istart,22,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 23)
    {
	(void) ball5up(istart,23,x,strand,syms) ;
	k=k+5 ;
    }  /*end if*/
    if(k == 25)(void) ball1u(istart,25,x,strand,syms) ;
    if(k == 26)(void) ball1u(istart,26,x,strand,syms) ;
    if(k == 27)(void) ball1u(istart,27,x,strand,syms) ;
    if(k == 28 && nchain == 29)
    {
	(void) ball2botup(istart,28,x,strand,syms) ;
	k=k+1 ;
    }
    else if (k == 28)
    {
	(void) ball1u(istart,28,x,strand,syms) ;
    }  /*end if*/
    if(k == 29 && nchain == 29)
    {
	(void) ball1u(istart,29,x,strand,syms) ;
    }  /*end if*/
    *hold=x ;
    return ;
}
 
/* @func ball1s ***************************************************************
**
******************************************************************************/

void  ball1s(const int istart,const int j,const float x,
	     char *strand,int *syms)
{
 
    char stran = '\0';
	int   sym = 0;
	float     y = 0.0;
 
	if(j == 1)
{
	   y=68.6 ;
	   stran=strand[istart] ;
	   sym=syms[istart] ;
	}
	else if (j >= 7)
{
	   y=40.4 ;
	   stran=strand[istart+j-1] ;
	   sym=syms[istart+j-1] ;
	}  /*end if*/
	topoSymbol(x,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball1su **************************************************************
**
******************************************************************************/

void  ball1su(const int istart,const int j,const float x,
	      char *strand,int *syms)
{

	char stran = '\0';
	int  sym = 0;
	float y = 0.0;
 
 	if(j == 1)
{
	   y=40.4 ;
	   stran=strand[istart] ;
	   sym=syms[istart] ;
	}
	else if (j >= 7)
{
	   y=68.6 ;
	   stran=strand[istart+j-1] ;
	   sym=syms[istart+j-1] ;
	}  /*end if*/
	topoSymbol(x,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball1 ****************************************************************
**
******************************************************************************/

void  ball1(const int istart,const int j,const float x,
	    char *strand,int *syms)
{

	char stran = '\0';
	int   sym = 0;
	float y = 0.0;
 
	if(j == 1)
{
	   y=68.6 ;
	   stran=strand[istart] ;
	   sym=syms[istart] ;
	}
	else if (j > 16)
{
	   y=40.4 ;
	   stran=strand[istart+j-1] ;
	   sym=syms[istart+j-1] ;
	}  /*end if*/
	topoSymbol(x,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball1u ***************************************************************
**
******************************************************************************/

void  ball1u(const int istart,const int j,const float x,
	     char *strand,int *syms)
{

	char stran = '\0';
	int   sym = 0;
	float y = 0.0;
 
	if(j == 1)
{
	   y=40.4 ;
	   stran=strand[istart] ;
	   sym=syms[istart] ;
	}
	else if (j > 16)
{
	   y=68.6 ;
	   stran=strand[istart+j-1] ;
	   sym=syms[istart+j-1] ;
	}  /*end if*/
	topoSymbol(x,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball2topdown *********************************************************
**
******************************************************************************/

void  ball2topdown(const int istart,const int j,const float x,
		   char *strand,int *syms)  {
 
	char stran;
	int   sym;
	float y,x2 ;
 
	x2=x+2.8 ;
	y=67.6 ;
 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball2topup ***********************************************************
**
******************************************************************************/

void  ball2topup(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int  sym ;
	float y,x2 ;
 
	x2=x-2.8 ;
	y=41.4 ;
 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball2botdown *********************************************************
**
******************************************************************************/

void  ball2botdown(const int istart,const int j,const float x,
		   char *strand,int *syms)
{
 
	char stran ;
	int   sym ;
	float y = 0.0,x2 ;
 
	x2=x-2.8 ;
 
	if(j > 17)y=41.4 ;
 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball2botup ***********************************************************
**
******************************************************************************/

void  ball2botup(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float y = 0.0,x2 ;
 
	x2=x+2.8 ;
 
	if(j > 17)y=67.6 ;
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y,stran,sym) ;
 
	return ;
	}
 
/* @func ball1down7 ***********************************************************
**
******************************************************************************/

void  ball1down7(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,y = 0.0,y2 ;
 
	x0=x ;
	x2=x0 ;
	if(j == 2)y=63.9 ;
	if(j == 3)y=59.2 ;
	if(j == 4)y=54.5 ;
	if(j == 5)y=49.8 ;
	if(j == 6)y=45.1 ;
	y2=y ;
 
/*c single ball  */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
	}
 
/* @func ball1up7 *************************************************************
**
******************************************************************************/

void  ball1up7(const int istart,const int j,const float x,
	       char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,y = 0.0,y2 ;
 
	x0=x ;
	x2=x0 ;
	if(j == 2)y=45.1 ;
	if(j == 3)y=49.8 ;
	if(j == 4)y=54.5 ;
	if(j == 5)y=59.2 ;
	if(j == 6)y=63.9 ;
	y2=y ;
 
/*c single ball  */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
	}
 
/* @func ball1down ************************************************************
**
******************************************************************************/

void  ball1down(const int istart,const int j,const float x,
		char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,y = 0.0,y2 ;
 
	x0=x ;
	x2=x0 ;
	if(j == 4)y=59.2 ;
	if(j == 5)y=54.5 ;
	if(j == 6)y=49.8 ;
	if(j == 7)y=49.8 ;
	if(j == 8)y=49.8 ;
	y2=y ;
 
/*c single ball */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
	}
 
/* @func ball1up **************************************************************
**
******************************************************************************/

void  ball1up(const int istart,const int j,const float x,
	      char *strand,int *syms)  {
 
	char stran ;
	int  sym ;
	float x0,x2,y = 0.0,y2 ;
 
	x0=x ;
	x2=x0 ;
	if(j == 4)y=49.8 ;
	if(j == 5)y=54.5 ;
	if(j == 6)y=59.2 ;
	if(j == 7)y=59.2 ;
	if(j == 8)y=59.2 ;
	y2=y ;
 
/*c single ball */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
	}
 
/* @func ball1bdown ***********************************************************
**
******************************************************************************/

void  ball1bdown(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,y = 0.0,y2 ;
 
	x0=x ;
	x2=x0 ;
	if(j == 7)y=45.1 ;
	if(j == 8)y=45.1 ;
	y2=y ;
 
/*c single ball */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
	}
 
/* @func ball1bup *************************************************************
**
******************************************************************************/

void  ball1bup(const int istart,const int j,const float x,
	       char *strand,int *syms)  {
 
	char stran ;
	int  sym ;
	float x0,x2,y = 0.0,y2 ;
 
	x0=x ;
	x2=x0 ;
	if(j == 7)y=63.9 ;
	if(j == 8)y=63.9 ;
	y2=y ;
 
/*c single ball */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
}
 
/* @func ball2sdown ***********************************************************
**
******************************************************************************/

void  ball2sdown(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-1.4 ;
	x3=x0+1.4 ;
	if(j == 2)y=63.9 ;
	if(j == 4)y=59.2 ;
	if(j == 5)y=54.5 ;
	if(j == 6)y=54.5 ;
	if(j == 8)y=45.1 ;
	if(j == 9)y=45.1 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
/*c first ball in the duet */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
/*c 2nd ball in the duet */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball2sup *************************************************************
**
******************************************************************************/

void  ball2sup(const int istart,const int j,const float x,
	       char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0+1.4 ;
	x3=x0-1.4 ;
	if(j == 2)y=45.1 ;
	if(j == 4)y=49.8 ;
	if(j == 5)y=54.5 ;
	if(j == 6)y=54.5 ;
	if(j == 8)y=63.9 ;
	if(j == 9)y=63.9 ;
	y2=y-.7 ;
	y3=y+.7 ;
 
/*c first ball in the duet */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
/*c 2nd ball in the duet */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball2down ************************************************************
**
******************************************************************************/

void  ball2down(const int istart,const int j,const float x,
		char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-1.4 ;
	x3=x0+1.4 ;
	if(j == 2)y=63.9 ;
	if(j == 4)y=59.2 ;
	if(j == 5)y=59.2 ;
	if(j == 6)y=54.5 ;
	if(j == 7)y=54.5 ;
	if(j == 8)y=49.8 ;
	if(j == 9)y=49.8 ;
	if(j == 10)y=45.1 ;
	if(j == 11)y=45.1 ;
	if(j == 12)y=45.1 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
/*c first ball in the duet */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
/*c 2nd ball in the duet */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball2up **************************************************************
**
******************************************************************************/

void  ball2up(const int istart,const int j,const float x,
	      char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float  x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0+1.4 ;
	x3=x0-1.4 ;
	if(j == 2)y=45.1 ;
	if(j == 4)y=49.8 ;
	if(j == 5)y=49.8 ;
	if(j == 6)y=54.5 ;
	if(j == 7)y=54.5 ;
	if(j == 8)y=59.2 ;
	if(j == 9)y=59.2 ;
	if(j == 10)y=63.9 ;
	if(j == 11)y=63.9 ;
	if(j == 12)y=63.9 ;
	y2=y-.7 ;
	y3=y+.7 ;
 
/*c first ball in the duet */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 2nd ball in the duet */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball2adown ***********************************************************
**
******************************************************************************/

void  ball2adown(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-1.4 ;
	x3=x0+1.3 ;
	if(j == 10)y=49.8 ;
	if(j == 11)y=49.8 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
/*c first ball in the duet */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 2nd ball in the duet */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball2aup *************************************************************
**
******************************************************************************/

void  ball2aup(const int istart,const int j,const float x,
	       char *strand,int *syms)  {
 
	char stran ;
	int  sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0+1.4 ;
	x3=x0-1.4 ;
	if(j == 10)y=59.2 ;
	if(j == 11)y=59.2 ;
	y2=y-.7 ;
	y3=y+.7 ;
 
/*c first ball in the duet */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 2nd ball in the duet */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball3sdown ***********************************************************
**
******************************************************************************/

void  ball3sdown(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-2.8 ;
	x3=x0+2.8 ;
	if(j == 2)y=63.9 ;
	if(j == 5)y=59.2 ;
	if(j == 7)y=54.5 ;
	if(j == 8)y=54.5 ;
	if(j == 9)y=54.5 ;
	if(j == 12)y=45.1 ;
	if(j == 13)y=45.1 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
/*c first ball in the trio */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 2nd ball in the trio */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x0,y,stran,sym) ;
/*c 3rd ball in the trio */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball3sup *************************************************************
**
******************************************************************************/

void  ball3sup(const int istart,const int j,const float x,
	       char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0+2.8 ;
	x3=x0-2.8 ;
	if(j == 2)y=45.1 ;
	if(j == 5)y=49.8 ;
	if(j == 7)y=54.5 ;
	if(j == 8)y=54.5 ;
	if(j == 9)y=54.5 ;
	if(j == 12)y=63.9 ;
	if(j == 13)y=63.9 ;
	y2=y-.7 ;
	y3=y+.7 ;
 
/*c first ball in the trio */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 2nd ball in the trio */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x0,y,stran,sym) ;
/*c 3rd ball in the trio */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball3down ************************************************************
**
******************************************************************************/

void  ball3down(const int istart,const int j,const float x,
		char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-2.8 ;
	x3=x0+2.8 ;
	if(j == 2)y=63.9 ;
	if(j == 3)y=63.9 ;
	if(j == 5)y=59.2 ;
	if(j == 6)y=59.2 ;
	if(j == 8)y=54.5 ;
	if(j == 9)y=54.5 ;
	if(j == 11)y=49.8 ;
	if(j == 12)y=49.8 ;
	if(j == 13)y=49.8 ;
	if(j == 14)y=45.1 ;
	if(j == 15)y=45.1 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
 
/*c first ball in the trio */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 2nd ball in the trio */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x0,y,stran,sym) ;
/*c 3rd ball in the trio */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball3up **************************************************************
**
******************************************************************************/

void  ball3up(const int istart,const int j,const float x,
	      char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-2.8 ;
	x3=x0+2.8 ;
	if(j == 2)y=45.1 ;
	if(j == 3)y=45.1 ;
	if(j == 5)y=49.8 ;
	if(j == 6)y=49.8 ;
	if(j == 8)y=54.5 ;
	if(j == 9)y=54.5 ;
	if(j == 11)y=59.2 ;
	if(j == 12)y=59.2 ;
	if(j == 13)y=59.2 ;
	if(j == 14)y=63.9 ;
	if(j == 15)y=63.9 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
/*c first ball in the trio */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 2nd ball in the trio */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x0,y,stran,sym) ;
/*c 3rd ball in the trio */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
	}
 
/* @func ball3down2 ***********************************************************
**
******************************************************************************/

void  ball3down2(const int istart,const int j,const float x,
		 char *strand,int *syms)  {
 
	char stran ;
	int  sym ;
	float x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-2.8 ;
	x3=x0+2.8 ;
	if(j == 14)y=49.8 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
/*c first ball in the trio */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 2nd ball in the trio */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x0,y,stran,sym) ;
/*c 3rd ball in the trio */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
 
	return ;
	}
 
/* @func ball3up2 *************************************************************
**
******************************************************************************/

void  ball3up2(const int istart,const int j,const float x,
	       char *strand,int *syms)  {
 
	char stran ;
	int  sym ;
	float  x0,x2,x3,y = 0.0,y2,y3 ;
 
	x0=x ;
	x2=x0-2.8 ;
	x3=x0+2.8 ;
	if(j == 14)y=59.2 ;
	y2=y+.7 ;
	y3=y-.7 ;
 
/*c first ball in the trio */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 2nd ball in the trio */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x0,y,stran,sym) ;
/*c 3rd ball in the trio */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x2,y2,stran,sym) ;
 
	return ;
	}
 
/* @func ball4down ************************************************************
**
******************************************************************************/

void  ball4down(const int istart,const int j,const float x,
		char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,x4,y = 0.0,y0,y2,y3,y4 ;
 
	x0=x-4.2 ;
	x2=x-1.4 ;
	x3=x+1.4 ;
	x4=x+4.2 ;
	if(j == 2)y=63.9 ;
	if(j == 3)y=63.9 ;
	if(j == 6)y=59.2 ;
	if(j == 7)y=59.2 ;
	if(j == 9)y=54.5 ;
	if(j == 10)y=54.5 ;
	if(j == 11)y=54.5 ;
	if(j == 13)y=49.8 ;
	if(j == 14)y=49.8 ;
	if(j == 15)y=49.8 ;
	if(j == 16)y=45.1 ;
	if(j == 17)y=45.1 ;
	if(j == 18)y=45.1 ;
	if(j == 19)y=45.1 ;
	y0=y+.7 ;
	y2=y+.35 ;
	y3=y-.35 ;
	y4=y-.7 ;
 
/*c first ball in the foursome */ 
	stran =strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x0,y0,stran,sym) ;
/*c 2nd ball in the foursome */ 
	stran =strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 3rd ball in the foursome */ 
	stran =strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 4th ball in the foursome */ 
	stran =strand[istart+j+2] ;
	sym=syms[istart+j+2] ;
	topoSymbol(x4,y4,stran,sym) ;
 
	return ;
}
 
/* @func ball4up **************************************************************
**
******************************************************************************/

void  ball4up(const int istart,const int j,const float x,
	      char *strand,int *syms)  {
 
	char stran ;
	int sym ;
	float  x0,x2,x3,x4,y = 0.0,y0,y2,y3,y4 ;
 
	x0=x-4.2 ;
	x2=x-1.4 ;
	x3=x+1.4 ;
	x4=x+4.2 ;
	if(j == 2)y=45.1 ;
	if(j == 3)y=45.1 ;
	if(j == 6)y=49.8 ;
	if(j == 7)y=49.8 ;
	if(j == 9)y=54.5 ;
	if(j == 10)y=54.5 ;
	if(j == 11)y=54.5 ;
	if(j == 13)y=59.2 ;
	if(j == 14)y=59.2 ;
	if(j == 15)y=59.2 ;
	if(j == 16)y=63.9 ;
	if(j == 17)y=63.9 ;
	if(j == 18)y=63.9 ;
	if(j == 19)y=63.9 ;
	y0=y+.7 ;
	y2=y+.35 ;
	y3=y-.35 ;
	y4=y-.7 ;
 
/*c first ball in the foursome */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x4,y4,stran,sym) ;
/*c 2nd ball in the foursome */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 3rd ball in the foursome */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 4th ball in the foursome */ 
	stran=strand[istart+j+2] ;
	sym=syms[istart+j+2] ;
	topoSymbol(x0,y0,stran,sym) ;
 
	return ;
}
 
/* @func ball4bdown ***********************************************************
**
******************************************************************************/

void  ball4bdown(const int istart,const int j,const float x,
		  char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,x4,y = 0.0,y0,y2,y3,y4 ;
 
	x0=x-4.2 ;
	x2=x-1.4 ;
	x3=x+1.4 ;
	x4=x+4.2 ;
	if(j == 16)y=49.8 ;
	if(j == 17)y=49.8 ;
	y0=y+.7 ;
	y2=y+.35 ;
	y3=y-.35 ;
	y4=y-.7 ;
 
/*c first ball in the foursome */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x0,y0,stran,sym) ;
/*c 2nd ball in the foursome */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 3rd ball in the foursome */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 4th ball in the foursome */ 
	stran=strand[istart+j+2] ;
	sym=syms[istart+j+2] ;
	topoSymbol(x4,y4,stran,sym) ;
 
	return ;
	}
 
/* @func ball4bup *************************************************************
**
******************************************************************************/

void  ball4bup(const int istart,const int j,const float x,
	       char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,x4,y = 0.0,y0,y2,y3,y4 ;
 
	x0=x-4.2 ;
	x2=x-1.4 ;
	x3=x+1.4 ;
	x4=x+4.2 ;
	if(j == 16)y=59.2 ;
	if(j == 17)y=59.2 ;
	y0=y+.7 ;
	y2=y+.35 ;
	y3=y-.35 ;
	y4=y-.7 ;
 
/*c first ball in the foursome */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x4,y4,stran,sym) ;
/*c 2nd ball in the foursome */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 3rd ball in the foursome */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 4th ball in the foursome */ 
	stran=strand[istart+j+2] ;
	sym=syms[istart+j+2] ;
	topoSymbol(x0,y0,stran,sym) ;
 
	return ;
}
 
/* @func ball5down ************************************************************
**
******************************************************************************/

void  ball5down(const int istart,const int j,const float x,
		char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,x4,x5,y = 0.0,y0,y2,y3,y4,y5 ;
 
	x0=x-5.6 ;
	x2=x-2.8 ;
	x3=x ;
	x4=x+2.8 ;
	x5=x+5.6 ;
	if(j == 2)y=63.9 ;
	if(j == 3)y=63.9 ;
	if(j == 7)y=59.2 ;
	if(j == 8)y=59.2 ;
	if(j == 11)y=54.5 ;
	if(j == 12)y=54.5 ;
	if(j == 13)y=54.5 ;
	if(j == 16)y=49.8 ;
	if(j == 17)y=49.8 ;
	if(j == 18)y=49.8 ;
	if(j == 20)y=45.1 ;
	if(j == 21)y=45.1 ;
	if(j == 22)y=45.1 ;
	if(j == 23)y=45.1 ;
	y0=y+.7 ;
	y2=y+.35 ;
	y3=y ;
	y4=y-.35 ;
	y5=y-.7 ;
 
/*c first ball in the fivesome */ 
	stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x0,y0,stran,sym) ;
/*c 2nd ball in the fivesome */ 
	stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 3rd ball in the fivesome */ 
	stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 4th ball in the fivesome */ 
	stran=strand[istart+j+2] ;
	sym=syms[istart+j+2] ;
	topoSymbol(x4,y4,stran,sym) ;
/*c 5th ball in the fivesome */ 
	stran=strand[istart+j+3] ;
	sym=syms[istart+j+3] ;
	topoSymbol(x5,y5,stran,sym) ;
 
	return ;
}
 
/* @func ball5up **************************************************************
**
******************************************************************************/

void  ball5up(const int istart,const int j,const float x,
	      char *strand,int *syms)  {
 
	char stran ;
	int   sym ;
	float x0,x2,x3,x4,x5,y = 0.0,y0,y2,y3,y4,y5 ;
 
	x0=x-5.6 ;
	x2=x-2.8 ;
	x3=x ;
	x4=x+2.8 ;
	x5=x+5.6 ;
	if(j == 2)y=45.1 ;
	if(j == 3)y=45.1 ;
	if(j == 7)y=49.8 ;
	if(j == 8)y=49.8 ;
	if(j == 11)y=54.5 ;
	if(j == 12)y=54.5 ;
	if(j == 13)y=54.5 ;
	if(j == 16)y=59.2 ;
	if(j == 17)y=59.2 ;
	if(j == 18)y=59.2 ;
	if(j == 20)y=63.9 ;
	if(j == 21)y=63.9 ;
	if(j == 22)y=63.9 ;
	if(j == 23)y=63.9 ;
	y0=y+.7 ;
	y2=y+.35 ;
	y3=y ;
	y4=y-.35 ;
	y5=y-.7 ;
 
/*c first ball in the fivesome */ 
        stran=strand[istart+j-1] ;
	sym=syms[istart+j-1] ;
	topoSymbol(x5,y5,stran,sym) ;
/*c 2nd ball in the fivesome */ 
        stran=strand[istart+j] ;
	sym=syms[istart+j] ;
	topoSymbol(x4,y4,stran,sym) ;
/*c 3rd ball in the fivesome */ 
        stran=strand[istart+j+1] ;
	sym=syms[istart+j+1] ;
	topoSymbol(x3,y3,stran,sym) ;
/*c 4th ball in the fivesome */ 
        stran=strand[istart+j+2] ;
	sym=syms[istart+j+2] ;
	topoSymbol(x2,y2,stran,sym) ;
/*c 5th ball in the fivesome */ 
        stran=strand[istart+j+3] ;
	sym=syms[istart+j+3] ;
	topoSymbol(x0,y0,stran,sym) ;
 
	return ;
}
	 
/* @func conup ****************************************************************
**
******************************************************************************/

void  conup(float *hold,const int i,const ajuint is,ajuint *ie,
	    char *strand,int *syms)
{
 
    char   stran ;
    int     j,length,hole,hol2,hold3=0,ibridge,ibrid2 ;
    int     ihole,sym ;
    float        x = 0.0,y,x2 ;
    float        spacer = 0.0,rhol2,diff,rhole ;
  
    /*c check to see the size of the connecting part */ 
    length=is-*ie-1 ;
    /*c	if(length > 74)return */
    if(length <= 7)
    {
	if(length == 7)spacer=0.0 ;
	if(length == 6)spacer=0.0 ;
	if(length == 5)spacer=.46 ;
	if(length == 4)spacer=1.12 ;
	if(length == 3)spacer=2.1 ;
	if(length == 2)spacer=3.73 ;
	if(length == 1)spacer=7.0 ;
	if(length == 0)
	{
	    x=*hold ;
	    y=37.2 ;
	    topoMoveTo(x,y) ;
	    topoDraw(x,34.4) ;
	    topoDraw(x+6.0*2.8,34.4) ;
	    topoDraw(x+6.0*2.8,37.2) ;
	    *hold=x+6.0*2.8 ;
	}  /*end if*/
	if(length == 7)*hold=*hold-1.4 ;
	for(j=1;j<=length ;j++)
	{
	    x=*hold+spacer+1.4+spacer*(j-1)+2.8*(j-1) ;
	    y=37.2 ;
	    stran=strand[*ie+j] ;
	    sym=syms[*ie+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	*hold=x ;
	if(length == 7)*hold=*hold-1.4 ;
    }
    else if (8 <= length && length <= 22)
    {
	/*c form a simple bridge */ 
	x=*hold ;
	hole=length-5 ;
	hol2=hole/2 ;
	rhol2=(float)hole/2.0 ;
	if(hol2 == rhol2)hold3=hol2 ;
	if(hol2 != rhol2)hold3=hol2+1;
	ibridge=length-2*hold3 ;
	for(j=1;j<=hold3 ;j++)
	{
	    y=37.2-2.8*(j-1) ;
	    stran=strand[*ie+j] ;
	    sym=syms[*ie+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	ibrid2=5-ibridge ;
	if(ibrid2 == 0)spacer=0.0 ;
	if(ibrid2 == 1)spacer=.56 ;
	if(ibrid2 == 2)spacer=1.4 ;
	if(ibrid2 == 3)spacer=2.8 ;
	if(ibrid2 == 4)spacer=5.6 ;
	for(j=1;j<=ibridge ;j++)
	{
	    x=*hold+spacer*j+2.8*j ;
	    y=37.2-2.8*(hold3-1) ;
	    stran=strand[*ie+hold3+j] ;
	    sym=syms[*ie+hold3+j] ;
	    topoSymbol(x,y,stran,sym) ;
	    if(j == ibridge)*hold=x+spacer ;
	}  /*end for*/
	x=*hold+2.8 ;
	for(j=1;j<=hold3 ;j++)
	{
	    y=37.2-2.8*(hold3-j) ;
	    stran=strand[is-(hold3-j)-1] ;
	    sym=syms[is-(hold3-j)-1] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	*hold=x ;
    }
    else if (23 <= length && length <= 38)
    {
	hole=length-10 ;
	rhole=(float)hole/4.0 ;
	ihole=hole/4 ;
	diff=rhole-ihole ;
	if(rhole == ihole)
	{
	    /*c standard 10 extra loop configuration */ 
	    spacer=0.0 ;
	    /*c	      if(nchain == 22)spacer=.45 */ 
	    x=*hold ;
	    for(j=1;j<=2 ;j++)
	    {
		y=37.2-2.8*(j-1) ;
		stran=strand[*ie+j] ;
		sym=syms[*ie+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*j ;
		stran=strand[*ie+j+2] ;
		sym=syms[*ie+j+2] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    /*c put in end caps */ 
	    x=*hold+1.4+spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+3+ihole] ;
	    sym=syms[*ie+3+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+4.2+spacer ;
	    stran=strand[*ie+4+ihole] ;
	    sym=syms[*ie+4+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+5.6+2.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*(1+ihole-j) ;
		stran=strand[*ie+4+ihole+j] ;
		sym=syms[*ie+4+ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    /*c put in end caps near surface */ 
	    x=*hold+7.0+3.0*spacer ;
	    y=34.6 ;
	    stran=strand[*ie+5+2*ihole] ;
	    sym=syms[*ie+5+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+9.8+3.0*spacer ;
	    stran=strand[*ie+6+2*ihole] ;
	    sym=syms[*ie+6+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw up from the surface */ 
	    x=*hold+11.2+4.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*j ;
		stran=strand[*ie+6+2*ihole+j] ;
		sym=syms[*ie+6+2*ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    /*c put in end caps */ 
	    x=*hold+12.6+5.0*spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+7+3*ihole] ;
	    sym=syms[*ie+7+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+15.4+5.0*spacer ;
	    stran=strand[*ie+8+3*ihole] ;
	    sym=syms[*ie+8+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+16.8+6.0*spacer ;
	    for(j=1;j<=ihole+2 ;j++)
	    {
		y=34.6-2.8*(1+ihole-j) ;
		stran=strand[*ie+8+3*ihole+j] ;
		sym=syms[*ie+8+3*ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    *hold=x ;
	}
	else if (diff == .25)
	{
	    /*c standard 7 extra loop configuration */ 
	    spacer=0.0 ;
	    /*c	      if(nchain == 22)spacer=.45 */ 
	    x=*hold ;
	    ihole=ihole+1 ;
	    for(j=1;j<=2 ;j++)
	    {
		y=37.2-2.8*(j-1) ;
		stran=strand[*ie+j] ;
		sym=syms[*ie+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*j ;
		stran=strand[*ie+2+j] ;
		sym=syms[*ie+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    /*c put in end cap */ 
	    x=*hold+2.8+spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+3+ihole] ;
	    sym=syms[*ie+3+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+5.6+2.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*(1+ihole-j) ;
		stran=strand[*ie+3+ihole+j] ;
		sym=syms[*ie+3+ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    /*c put in end cap near surface */ 
	    x=*hold+8.4+3.0*spacer ;
	    y=34.6 ;
	    stran=strand[*ie+4+2*ihole] ;
	    sym=syms[*ie+4+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw up from the surface */ 
	    x=*hold+11.2+4.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
	         y=34.6-2.8*j ;
 	         stran=strand[*ie+4+2*ihole+j] ;
	         sym=syms[*ie+4+2*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	     }     /*end for*/
	    /*c put in end cap */ 
	    x=*hold+14.0+5.0*spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+5+3*ihole] ;
	    sym=syms[*ie+5+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+16.8+6.0*spacer ;
	    for(j=1;j<=ihole+2 ;j++)
	    {
		y=34.6-2.8*(1+ihole-j) ;
		stran=strand[*ie+5+3*ihole+j] ;
		sym=syms[*ie+5+3*ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    *hold=x ;
	}
	else if (diff == .5)
	{
	    /*c standard 8 extra loop configuration */ 
	    spacer=0.0 ;
	    /*c	      if(nchain == 22)spacer=.45 */ 
	    ihole=ihole+1 ;
	    x=*hold ;
	    for(j=1;j<=2 ;j++)
	    {
		y=37.2-+2.8*(j-1) ;
		stran=strand[*ie+j] ;
		sym=syms[*ie+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*j ;
		stran=strand[*ie+2+j] ;
		sym=syms[*ie+2+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    /*c put in end cap */ 
	    x=*hold+2.8+spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+3+ihole] ;
	    sym=syms[*ie+3+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+5.6+2.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-+2.8*(1+ihole-j) ;
		stran=strand[*ie+3+ihole+j] ;
		sym=syms[*ie+3+ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    /*c put in end caps near surface */ 
	    x=*hold+7.0+3.0*spacer ;
	    y=34.6 ;
	    stran=strand[*ie+4+2*ihole] ;
	    sym=syms[*ie+4+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+9.8+3.0*spacer ;
	    stran=strand[*ie+5+2*ihole] ;
	    sym=syms[*ie+5+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw up from the surface */ 
	    x=*hold+11.2+4.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*j ;
		stran=strand[*ie+5+2*ihole+j] ;
		sym=syms[*ie+5+2*ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    /*c put in end caps */ 
	    x=*hold+14.0+5.0*spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+6+3*ihole] ;
	    sym=syms[*ie+6+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+16.8+6.0*spacer ;
	    for(j=1;j<=ihole+2 ;j++)
	    {
		y=34.6-2.8*(1+ihole-j) ;
		stran=strand[*ie+6+3*ihole+j] ;
		sym=syms[*ie+6+3*ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    *hold=x ;
	}
	else if (diff == .75)
	{
	    /*c standard 9 extra loop configuration */ 
	    spacer=0.0 ;
	    /*c	      if(nchain == 22)spacer=.45 */ 
	    ihole=ihole+1 ;
	    x=*hold ;
	    for(j=1;j<=2 ;j++)
	    {
		y=37.2-2.8*(j-1) ;
		stran=strand[*ie+j] ;
		sym=syms[*ie+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*j ;
		stran=strand[*ie+2+j] ;
		sym=syms[*ie+2+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    /*c put in end caps */ 
	    x=*hold+1.4+spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+3+ihole] ;
	    sym=syms[*ie+3+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+4.2+spacer ;
	    stran=strand[*ie+4+ihole] ;
	    sym=syms[*ie+4+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+5.6+2.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*(1+ihole-j) ;
		stran=strand[*ie+4+ihole+j] ;
		sym=syms[*ie+4+ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    /*c put in end caps near surface */ 
	    x=*hold+7.0+3.0*spacer ;
	    y=34.6 ;
	    stran=strand[*ie+5+2*ihole] ;
	    sym=syms[*ie+5+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+9.8+3.0*spacer ;
	    stran=strand[*ie+6+2*ihole] ;
	    sym=syms[*ie+6+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw up from the surface */ 
	    x=*hold+11.2+4.0*spacer ;
	    for(j=1;j<=ihole ;j++)
	    {
		y=34.6-2.8*j ;
		stran=strand[*ie+6+2*ihole+j] ;
		sym=syms[*ie+6+2*ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }     /*end for*/
	    /*c put in end cap */ 
	    x=*hold+14.0+5.0*spacer ;
	    y=34.6-2.8*(1+ihole) ;
	    stran=strand[*ie+7+3*ihole] ;
	    sym=syms[*ie+7+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    /*c draw back towards the surface */ 
	    x=*hold+16.8+6.0*spacer ;
	    for(j=1;j<=ihole+2 ;j++)
	    {
		y=34.6-2.8*(1+ihole-j) ;
		stran=strand[*ie+7+3*ihole+j] ;
		sym=syms[*ie+7+3*ihole+j] ;
		topoSymbol(x,y,stran,sym) ;
	    }  /*end for*/
	    *hold=x ;
	}     /*end if*/
    }
    else if (39 <= length && length <= 74)
    {
	x2=.45 ;
	hole=length-16 ;
	rhole=(float)hole/6.0 ;
	ihole=hole/6 ;
	diff=rhole-ihole ;
	diff=6.0*diff ;
	/*c standard start for the structure */ 
	x=*hold  ;
	for(j=1;j<=2 ;j++)
	{
	    y=37.2-2.8*(j-1) ;
	    stran=strand[*ie+j] ;
	    sym=syms[*ie+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*cx over to the left */ 
	for(j=1;j<=3 ;j++)
	{
	    x=*hold-2.8*j ;
	    stran=strand[*ie+2+j] ;
	    sym=syms[*ie+2+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*c down in downward spacer */ 
	x=*hold-8.4 ;
	y=31.6 ;
	stran=strand[*ie+6] ;
	sym=syms[*ie+6] ;
	topoSymbol(x,y,stran,sym) ;
	y=28.8 ;
	stran=strand[*ie+7] ;
	sym=syms[*ie+7] ;
	topoSymbol(x,y,stran,sym) ;
	/*!	   if(rhole == ihole)then */ 
	/*c standard 10 extra loop configuration */ 
	spacer=0.0 ;
	/*!	      if(nchain == 22)spacer=.45 */ 
	x=*hold-8.4 ;
	for(j=1;j<=ihole ;j++)
	{
	    y=28.8-2.8*j ;
	    stran=strand[*ie+7+j] ;
	    sym=syms[*ie+7+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*c put in end cap(s) */ 
	if(diff >= .99)
	{
	    x=*hold-7.28 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+8+ihole] ;
	    sym=syms[*ie+8+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold-4.48 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+9+ihole] ;
	    sym=syms[*ie+9+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    *ie=*ie+1 ;
	}
	else
	{
	    x=*hold-5.88 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+8+ihole] ;
	    sym=syms[*ie+8+ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end if*/
	/*c draw back towards the surface */ 
	x=*hold-3.36 ;
	for(j=1;j<=ihole ;j++)
	{
	    y=28.8-2.8*(1+ihole-j) ;
	    stran=strand[*ie+8+ihole+j] ;
	    sym=syms[*ie+8+ihole+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*c put in end cap(s) */ 
	if(diff >= 1.99)
	{
	    x=*hold-2.24 ;
	    y=28.8 ;
	    stran=strand[*ie+9+2*ihole] ;
	    sym=syms[*ie+9+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+.56 ;
	    y=28.8 ;
	    stran=strand[*ie+10+2*ihole] ;
	    sym=syms[*ie+10+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    *ie=*ie+1 ;
	}
	else
	{
	    x=*hold-.84 ;
	    y=28.8 ;
	    stran=strand[*ie+9+2*ihole] ;
	    sym=syms[*ie+9+2*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end if*/
	/*c draw back away from the surface */ 
	x=*hold+1.68 ;
	for(j=1;j<=ihole ;j++)
	{
	    y=28.8-2.8*j ;
	    stran=strand[*ie+9+2*ihole+j] ;
	    sym=syms[*ie+9+2*ihole+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*c put in end cap(s) */ 
	if(diff >= 2.99)
	{
	    x=*hold+2.8 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+10+3*ihole] ;
	    sym=syms[*ie+10+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+5.6 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+11+3*ihole] ;
	    sym=syms[*ie+11+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    *ie=*ie+1 ;
	}
	else
	{
	    x=*hold+4.2 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+10+3*ihole] ;
	    sym=syms[*ie+10+3*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end if*/
	/*c draw back towards the surface */ 
	x=*hold+6.72 ;
	for(j=1;j<=ihole ;j++)
	{
	    y=28.8-2.8*(1+ihole-j) ;
	    stran=strand[*ie+10+3*ihole+j] ;
	    sym=syms[*ie+10+3*ihole+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*c put in end cap(s) */ 
	if(diff >= 3.99)
	{
	    x=*hold+7.84 ;
	    y=28.8 ;
	    stran=strand[*ie+11+4*ihole] ;
	    sym=syms[*ie+11+4*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+10.64 ;
	    y=28.8 ;
	    stran=strand[*ie+12+4*ihole] ;
	    sym=syms[*ie+12+4*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    *ie=*ie+1 ;
	}
	else
	{
	    x=*hold+9.24 ;
	    y=28.8 ;
	    stran=strand[*ie+11+4*ihole] ;
	    sym=syms[*ie+11+4*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end if*/
	/*c draw back away from the surface */ 
	x=*hold+11.76 ;
	for(j=1;j<=ihole ;j++)
	{
	    y=28.8-2.8*j ;
	    stran=strand[*ie+11+4*ihole+j] ;
	    sym=syms[*ie+11+4*ihole+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*c put in end cap(s) */ 
	if(diff >= 4.99)
	{
	    x=*hold+12.68 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+12+5*ihole] ;
	    sym=syms[*ie+12+5*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    x=*hold+15.68 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+13+5*ihole] ;
	    sym=syms[*ie+13+5*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	    *ie=*ie+1 ;
	}
	else
	{
	    x=*hold+14.28 ;
	    y=28.8-2.8*(1+ihole) ;
	    stran=strand[*ie+12+5*ihole] ;
	    sym=syms[*ie+12+5*ihole] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end if*/
	/*c draw back towards the surface */ 
	x=*hold+16.8 ;
	for(j=1;j<=ihole ;j++)
	{
	    y=28.8-2.8*(1+ihole-j) ;
	    stran=strand[*ie+12+5*ihole+j] ;
	    sym=syms[*ie+12+5*ihole+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	/*c standard ending */ 
	x=*hold+16.8 ;
	for(j=1;j<=4 ;j++)
	{
	    y=28.8+2.8*(j-1) ;
	    stran=strand[*ie+12+6*ihole+j] ;
	    sym=syms[*ie+12+6*ihole+j] ;
	    topoSymbol(x,y,stran,sym) ;
	}  /*end for*/
	*hold=x ;
	/*!	   end if */ 
    }  /*end if*/
    return ;
}
 
/* @func condown **************************************************************
**
******************************************************************************/

void  condown(float *hold1,const int i,const ajuint is,ajuint *ie,
	      char *strand,int *syms)
{
 
 
/*#include "common.inc" */ 
 
	char   stran ;
	int     j,length,hole,hol2,hold3=0,ibridge,ibrid2 ;
	int     ihole,sym ;
	float        x = 0.0,y,x2 ;
        float        spacer = 0.0,rhol2,diff,rhole ;
 
/*c check to see the size of the connecting part */ 
	length=is-*ie-1 ;
	if(length >= 75)return ;
	if(length <= 7)
{
           if(length == 7)spacer=0.0 ;
           if(length == 6)spacer=0.0 ;
           if(length == 5)spacer=.46 ;
	   if(length == 4)spacer=1.12 ;
	   if(length == 3)spacer=2.1 ;
	   if(length == 2)spacer=3.73 ;
	   if(length == 1)spacer=7.0 ;
	   if(length == 0)
{
	      x=*hold1 ;
	      y=71.8 ;
	      topoMoveTo(x,y) ;
	      topoDraw(x,74.6) ;
	      topoDraw(x+6.0*2.8,74.6) ;
	      topoDraw(x+6.0*2.8,71.8) ;
	      *hold1=x+6.0*2.8 ;
	   }  /*end if*/
	   if(length == 7)*hold1=*hold1-1.4 ;
	for(j=1;j<=length ;j++)
{
              x=*hold1+spacer+1.4+spacer*(j-1)+2.8*(j-1) ;
	      y=71.6 ;
	      stran=strand[*ie+j] ;
	      sym=syms[*ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
           }  /*end for*/
	   *hold1=x ;
	   if(length == 7)*hold1=*hold1-1.4 ;
	}
	else if (8 <= length && length <= 22)
{
/*c form a simple bridge */ 
	  x=*hold1 ;
	   hole=length-5 ;
/*c	   if(nchain == 22)hole=length-4 */ 
           hol2=hole/2 ;
           rhol2=(float)((float)hole/2.0) ;
           if(hol2 == rhol2)hold3=hol2 ;
	   if(hol2 != rhol2)hold3=hol2+1;
	   ibridge=length-2*hold3 ;
	for(j=1;j<=hold3 ;j++)
{
	      y=71.6+2.8*(j-1) ;
	      stran=strand[*ie+j] ;
	      sym=syms[*ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end for*/
           ibrid2=5-ibridge ;
	   if(ibrid2 == 0)spacer=0.0 ;
/*c	   if(ibrid2 == 0 && nchain == 24)spacer=0.23 */ 
	   if(ibrid2 == 1)
{
	      spacer=.56 ;
/*c	      if(nchain == 22)spacer=0.0 */ 
	   }  /*end if*/
	   if(ibrid2 == 2)spacer=1.4 ;
	   if(ibrid2 == 3)spacer=2.8 ;
	   if(ibrid2 == 4)spacer=5.6 ;
	for(j=1;j<=ibridge ;j++)
{
	      x=*hold1+spacer*j+2.8*j ;
              y=71.6+2.8*(hold3-1) ;
	      stran=strand[*ie+j+hold3] ;
	      sym=syms[*ie+j+hold3] ;
	      topoSymbol(x,y,stran,sym) ;
	      if(j == ibridge)*hold1=x+spacer ;
	   }  /*end for*/
	   x=*hold1+2.8 ;
	for(j=1;j<=hold3 ;j++)
{
	      y=71.6+2.8*(hold3-j) ;
	      stran=strand[is-(hold3-j)-1] ;
	      sym=syms[is-(hold3-j)-1] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end for*/
	   *hold1=x ;
	}
	else if (23 <= length && length <= 38)
{
	   hole=length-10 ;
	   rhole=(float)hole/4.0 ;
	   ihole=hole/4 ;
	   diff=rhole-ihole ;
	   if(rhole == ihole)
{
/*c standard 10 extra loop configuration */ 
	      spacer=0.0 ;
/*c	      if(nchain == 22)spacer=.45 */ 
	      x=*hold1 ;
	for(j=1;j<=2 ;j++)
{
	         y=71.6+2.8*(j-1) ;
	         stran=strand[*ie+j] ;
	         sym=syms[*ie+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+2+j] ;
	         sym=syms[*ie+2+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end caps */ 
	      x=*hold1+1.4+spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+3+ihole] ;
	      sym=syms[*ie+3+ihole] ;
	      topoSymbol(x,y,stran,sym) ;
	      x=*hold1+4.2+spacer ;
	      stran=strand[*ie+4+ihole] ;
	      sym=syms[*ie+4+ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+5.6+2.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+4+ihole+j] ;
	         sym=syms[*ie+4+ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end caps near surface */ 
	      x=*hold1+7.0+3.0*spacer ;
	      y=74.4 ;
	      stran=strand[*ie+5+2*ihole] ;
	      sym=syms[*ie+5+2*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
	      x=*hold1+9.8+3.0*spacer ;
	      stran=strand[*ie+6+2*ihole] ;
	      sym=syms[*ie+6+2*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw up from the surface */ 
	      x=*hold1+11.2+4.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+6+2*ihole+j] ;
	         sym=syms[*ie+6+2*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end caps */ 
	      x=*hold1+12.6+5.0*spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+7+3*ihole] ;
	      sym=syms[*ie+7+3*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
	      x=*hold1+15.4+5.0*spacer ;
	      stran=strand[*ie+8+3*ihole] ;
	      sym=syms[*ie+8+3*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+16.8+6.0*spacer ;
	for(j=1;j<=ihole+2 ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+8+3*ihole+j] ;
	         sym=syms[*ie+8+3*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
	      *hold1=x    ;
	}
	else if (diff == .25)
{
/*c standard 7 extra loop configuration */ 
	      spacer=0.0 ;
/*c	      if(nchain == 22)spacer=.45 */ 
	      x=*hold1 ;
	      ihole=ihole+1 ;
	for(j=1;j<=2 ;j++)
{
	         y=71.6+2.8*(j-1) ;
	         stran=strand[*ie+j] ;
	         sym=syms[*ie+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+j+2] ;
	         sym=syms[*ie+j+2] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end cap */ 
	      x=*hold1+2.8+spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+3+ihole] ;
	      sym=syms[*ie+3+ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+5.6+2.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+3+ihole+j] ;
	         sym=syms[*ie+3+ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end cap near surface */ 
	      x=*hold1+8.4+3.0*spacer ;
	      y=74.4 ;
	      stran=strand[*ie+4+2*ihole] ;
	      sym=syms[*ie+4+2*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw up from the surface */ 
	      x=*hold1+11.2+4.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+4+2*ihole+j] ;
	         sym=syms[*ie+4+2*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end cap */ 
	      x=*hold1+14.0+5.0*spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+5+3*ihole] ;
	      sym=syms[*ie+5+3*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+16.8+6.0*spacer ;
	for(j=1;j<=ihole+2 ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+5+3*ihole+j] ;
	         sym=syms[*ie+5+3*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
	      *hold1=x ;
	}
	else if (diff == .5)
{
/*c standard 8 extra loop configuration */ 
	      spacer=0.0 ;
/*c	      if(nchain == 22)spacer=.45 */ 
	      ihole=ihole+1 ;
	      x=*hold1 ;
	for(j=1;j<=2 ;j++)
{
	         y=71.6+2.8*(j-1) ;
	         stran=strand[*ie+j] ;
	         sym=syms[*ie+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+2+j] ;
	         sym=syms[*ie+2+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end cap */ 
	      x=*hold1+2.8+spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+3+ihole] ;
	      sym=syms[*ie+3+ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+5.6+2.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+3+ihole+j] ;
	         sym=syms[*ie+3+ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end caps near surface */ 
	      x=*hold1+7.0+3.0*spacer ;
	      y=74.4 ;
	      stran=strand[*ie+4+2*ihole] ;
	      sym=syms[*ie+4+2*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
	      x=*hold1+9.8+3.0*spacer ;
	      stran=strand[*ie+5+2*ihole] ;
	      sym=syms[*ie+5+2*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw up from the surface */ 
	      x=*hold1+11.2+4.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+5+2*ihole+j] ;
	         sym=syms[*ie+5+2*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end caps */ 
	      x=*hold1+14.0+5.0*spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+6+3*ihole] ;
	      sym=syms[*ie+6+3*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+16.8+6.0*spacer ;
	for(j=1;j<=ihole+2 ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+6+3*ihole+j] ;
	         sym=syms[*ie+6+3*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
	      *hold1=x ;
	}
	else if (diff == .75)
{
/*c standard 9 extra loop configuration */ 
	      spacer=0.0 ;
/*c	      if(nchain == 22)spacer=.45 */ 
	      ihole=ihole+1 ;
	      x=*hold1 ;
	for(j=1;j<=2 ;j++)
{
	         y=71.6+2.8*(j-1) ;
	         stran=strand[*ie+j] ;
	         sym=syms[*ie+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+2+j] ;
	         sym=syms[*ie+2+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end caps */ 
	      x=*hold1+1.4+spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+3+ihole] ;
	      sym=syms[*ie+3+ihole] ;
	      topoSymbol(x,y,stran,sym) ;
	      x=*hold1+4.2+spacer ;
	      stran=strand[*ie+4+ihole] ;
	      sym=syms[*ie+4+ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+5.6+2.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+4+ihole+j] ;
	         sym=syms[*ie+4+ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end caps near surface */ 
	      x=*hold1+7.0+3.0*spacer ;
	      y=74.4 ;
	      stran=strand[*ie+5+2*ihole] ;
	      sym=syms[*ie+5+2*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
	      x=*hold1+9.8+3.0*spacer ;
	      stran=strand[*ie+6+2*ihole] ;
	      sym=syms[*ie+6+2*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw up from the surface */ 
	      x=*hold1+11.2+4.0*spacer ;
	for(j=1;j<=ihole ;j++)
{
	         y=74.4+2.8*j ;
	         stran=strand[*ie+6+2*ihole+j] ;
	         sym=syms[*ie+6+2*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }     /*end for*/
/*c put in end cap */ 
	      x=*hold1+14.0+5.0*spacer ;
	      y=74.4+2.8*(1+ihole) ;
	      stran=strand[*ie+7+3*ihole] ;
	      sym=syms[*ie+7+3*ihole] ;
	      topoSymbol(x,y,stran,sym) ;
/*c draw back towards the surface */ 
	      x=*hold1+16.8+6.0*spacer ;
	for(j=1;j<=ihole+2 ;j++)
{
	         y=74.4+2.8*(1+ihole-j) ;
	         stran=strand[*ie+7+3*ihole+j] ;
	         sym=syms[*ie+7+3*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
	   }     /*end if*/
	   *hold1=x ;
	}
	else if (39 <= length && length <= 74)
{
	   x2=.45 ;
           hole=length-16 ;
	   rhole=(float)hole/6.0 ;
	   ihole=hole/6 ;
	   diff=rhole-ihole ;
	   diff=6.0*diff ;
/*c standard start for the structure */ 
	   x=*hold1  ;
	for(j=1;j<=2 ;j++)
{
	      y=71.6+2.8*(j-1) ;
	      stran=strand[*ie+j] ;
	      sym=syms[*ie+j] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end for*/
/*cx over to the left */ 
	for(j=1;j<=3 ;j++)
{
	      x=*hold1-2.8*j ;
	      stran=strand[*ie+2+j] ;
	      sym=syms[*ie+2+j] ;
	      topoSymbol(x,y,stran,sym) ;
	   }  /*end for*/
/*c down in downward spacer */ 
	   x=*hold1-8.4 ;
	   y=77.2 ;
	   stran=strand[*ie+6] ;
	   sym=syms[*ie+6] ;
	   topoSymbol(x,y,stran,sym) ;
	   x=*hold1-8.4	    ;
	   y=80.0 ;
	   stran=strand[*ie+7] ;
	   sym=syms[*ie+7] ;
	   topoSymbol(x,y,stran,sym) ;
	   x=*hold1-8.4	    ;
/*!	   if(rhole == ihole)then */ 
/*c standard 10 extra loop configuration */ 
	      spacer=0.0 ;
/*!	      if(nchain == 22)spacer=.45 */ 
	      x=*hold1-8.4 ;
	for(j=1;j<=ihole ;j++)
{
	         y=80.0+2.8*j ;
	         stran=strand[*ie+7+j] ;
	         sym=syms[*ie+7+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end cap(s) */ 
	      if(diff >= .99)
{
	         x=*hold1-7.28 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+8+ihole] ;
	         sym=syms[*ie+8+ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         x=*hold1-4.48 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+9+ihole] ;
	         sym=syms[*ie+9+ihole] ;
	         topoSymbol(x,y,stran,sym) ;
		 *ie=*ie+1 ;
	}
	else
{
	         x=*hold1-5.88 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+8+ihole] ;
	         sym=syms[*ie+8+ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end if*/
/*c draw back towards the surface */ 
	      x=*hold1-3.36 ;
	for(j=1;j<=ihole ;j++)
{
	         y=80.0+2.8*(1+ihole-j) ;
	         stran=strand[*ie+8+ihole+j] ;
	         sym=syms[*ie+8+ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end cap(s) */ 
	      if(diff >= 1.99)
{
	         x=*hold1-2.24 ;
	         y=80.0 ;
	         stran=strand[*ie+9+2*ihole] ;
	         sym=syms[*ie+9+2*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         x=*hold1+.56 ;
	         y=80.0 ;
	         stran=strand[*ie+10+2*ihole] ;
	         sym=syms[*ie+10+2*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
		 *ie=*ie+1 ;
	}
	else
{
	         x=*hold1-.84 ;
	         y=80.0 ;
	         stran=strand[*ie+9+2*ihole] ;
	         sym=syms[*ie+9+2*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end if*/
/*c draw back away from the surface */ 
	      x=*hold1+1.68 ;
	for(j=1;j<=ihole ;j++)
{
	         y=80.0+2.8*j ;
	         stran=strand[*ie+9+2*ihole+j] ;
	         sym=syms[*ie+9+2*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end cap(s) */ 
	      if(diff >= 2.99)
{
	         x=*hold1+2.8 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+10+3*ihole] ;
	         sym=syms[*ie+10+3*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         x=*hold1+5.6 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+11+3*ihole] ;
	         sym=syms[*ie+11+3*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         *ie=*ie+1 ;
	}
	else
{
	         x=*hold1+4.2 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+10+3*ihole] ;
	         sym=syms[*ie+10+3*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end if*/
/*c draw back towards the surface */ 
	      x=*hold1+6.72 ;
	for(j=1;j<=ihole ;j++)
{
	         y=80.0+2.8*(1+ihole-j) ;
	         stran=strand[*ie+10+3*ihole+j] ;
	         sym=syms[*ie+10+3*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end cap(s) */ 
	      if(diff >= 3.99)
{
	         x=*hold1+7.84 ;
	         y=80.0 ;
	         stran=strand[*ie+11+4*ihole] ;
	         sym=syms[*ie+11+4*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         x=*hold1+10.64 ;
	         y=80.0 ;
	         stran=strand[*ie+12+4*ihole] ;
	         sym=syms[*ie+12+4*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         *ie=*ie+1 ;
	}
	else
{
	         x=*hold1+9.24 ;
	         y=80.0 ;
	         stran=strand[*ie+11+4*ihole] ;
	         sym=syms[*ie+11+4*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end if*/
/*c draw back away from the surface */ 
	      x=*hold1+11.76 ;
	for(j=1;j<=ihole ;j++)
{
	         y=80.0+2.8*j ;
	         stran=strand[*ie+11+4*ihole+j] ;
	         sym=syms[*ie+11+4*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c put in end cap(s) */ 
	      if(diff >= 4.99)
{
	         x=*hold1+12.68 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+12+5*ihole] ;
	         sym=syms[*ie+12+5*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         x=*hold1+15.68 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+13+5*ihole] ;
	         sym=syms[*ie+13+5*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	         *ie=*ie+1 ;
	}
	else
{
	         x=*hold1+14.28 ;
	         y=80.0+2.8*(1+ihole) ;
	         stran=strand[*ie+12+5*ihole] ;
	         sym=syms[*ie+12+5*ihole] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end if*/
/*c draw back towards the surface */ 
	      x=*hold1+16.8 ;
	for(j=1;j<=ihole ;j++)
{
	         y=80.0+2.8*(1+ihole-j) ;
	         stran=strand[*ie+12+5*ihole+j] ;
	         sym=syms[*ie+12+5*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*c standard ending */ 
	      x=*hold1+16.8 ;
	for(j=1;j<=4 ;j++)
{
		 y=80.0-2.8*(j-1) ;
	         stran=strand[*ie+12+6*ihole+j] ;
	         sym=syms[*ie+12+6*ihole+j] ;
	         topoSymbol(x,y,stran,sym) ;
	      }  /*end for*/
/*!	   end if */ 
	}  /*end if*/
	return ;
}
 
/* @func condownsmall *********************************************************
**
******************************************************************************/

void  condownsmall(const int is,int *ie,char *strand,int *syms) 
{
    char   stran ;
    int     j,length,hole ;
    int     ihole,sym ;
    float        x,y ;
    float        spacer,diff,rhole ;
 
    /*c draw the membrane */ 
    topoMoveTo(10.0,20.0) ;
    topoDraw(140.0,20.0) ;
    topoMoveTo(10.0,19.9) ;
    topoDraw(140.0,19.9) ;
    topoMoveTo(10.0,19.8) ;
    topoDraw(140.0,19.8) ;
 
    x=40.0 ;
    y=20.0 ;
    /*c check to see the size of the connecting part */ 
    length=is-*ie-1 ;
    if(length < 48)return ;
    if(length > 75)return ;
    hole=length-16 ;
    rhole=(float)hole/6.0 ;
    ihole=hole/6 ;
    diff=rhole-ihole ;
    diff=6.0*diff ;
    /*c standard start for the structure */ 
    for(j=1;j<=2 ;j++)
    {
	y=21.4+2.8*(j-1) ;
	stran=strand[*ie+j] ;
	sym=syms[*ie+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*cx over to the left */ 
    for(j=1;j<=3 ;j++)
    {
	x=40.0-2.8*j ;
	stran=strand[*ie+2+j] ;
	sym=syms[*ie+2+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*c down in downward spacer */ 
    x=40.0-8.4 ;
    y=27.0 ;
    stran=strand[*ie+6] ;
    sym=syms[*ie+6] ;
    topoSymbol(x,y,stran,sym) ;
    x=40.0-8.4	    ;
    y=29.8 ;
    stran=strand[*ie+7] ;
    sym=syms[*ie+7] ;
    topoSymbol(x,y,stran,sym) ;
    x=40.0-8.4	    ;
    /*!	   if(rhole == ihole)then */ 
    /*c standard 10 extra loop configuration */ 
    spacer=0.0 ;
    /*!	      if(nchain == 22)spacer=.45 */ 
    x=40.0-8.4 ;
    for(j=1;j<=ihole ;j++)
    {
	y=29.8+2.8*j ;
	stran=strand[*ie+7+j] ;
	sym=syms[*ie+7+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*c put in end cap(s) */ 
    if(diff >= .99)
    {
	x=40.0-7.28 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+8+ihole] ;
	sym=syms[*ie+8+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=40.0-4.48 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+9+ihole] ;
	sym=syms[*ie+9+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	x=40.0-5.88 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+8+ihole] ;
	sym=syms[*ie+8+ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
    /*c draw back towards the surface */ 
    x=40.0-3.36 ;
    for(j=1;j<=ihole ;j++)
    {
	y=29.8+2.8*(1+ihole-j) ;
	stran=strand[*ie+8+ihole+j] ;
	sym=syms[*ie+8+ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*c put in end cap(s) */ 
    if(diff >= 1.99)
    {
	x=40.0-2.24 ;
	y=29.8 ;
	stran=strand[*ie+9+2*ihole] ;
	sym=syms[*ie+9+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=40.56 ;
	y=29.8 ;
	stran=strand[*ie+10+2*ihole] ;
	sym=syms[*ie+10+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	x=40.0-.84 ;
	y=29.8 ;
	stran=strand[*ie+9+2*ihole] ;
	sym=syms[*ie+9+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
    /*c draw back away from the surface */ 
    x=41.68 ;
    for(j=1;j<=ihole ;j++)
    {
	y=29.8+2.8*j ;
	stran=strand[*ie+9+2*ihole+j] ;
	sym=syms[*ie+9+2*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*c put in end cap(s) */ 
    if(diff >= 2.99)
    {
	x=42.8 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+10+3*ihole] ;
	sym=syms[*ie+10+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=45.6 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+11+3*ihole] ;
	sym=syms[*ie+11+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	x=44.2 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+10+3*ihole] ;
	sym=syms[*ie+10+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
    /*c draw back towards the surface */ 
    x=46.72 ;
    for(j=1;j<=ihole ;j++)
    {
	y=29.8+2.8*(1+ihole-j) ;
	stran=strand[*ie+10+3*ihole+j] ;
	sym=syms[*ie+10+3*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*c put in end cap(s) */ 
    if(diff >= 3.99)
    {
	x=47.84 ;
	y=29.8 ;
	stran=strand[*ie+11+4*ihole] ;
	sym=syms[*ie+11+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=50.64 ;
	y=29.8 ;
	stran=strand[*ie+12+4*ihole] ;
	sym=syms[*ie+12+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	x=49.24 ;
	y=29.8 ;
	stran=strand[*ie+11+4*ihole] ;
	sym=syms[*ie+11+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
    /*c draw back away from the surface */ 
    x=51.76 ;
    for(j=1;j<=ihole ;j++)
    {
	y=29.8+2.8*j ;
	stran=strand[*ie+11+4*ihole+j] ;
	sym=syms[*ie+11+4*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*c put in end cap(s) */ 
    if(diff >= 4.99)
    {
	x=52.68 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+12+5*ihole] ;
	sym=syms[*ie+12+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=55.68 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+13+5*ihole] ;
	sym=syms[*ie+13+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	x=54.28 ;
	y=29.8+2.8*(1+ihole) ;
	stran=strand[*ie+12+5*ihole] ;
	sym=syms[*ie+12+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
    /*c draw back towards the surface */ 
    x=56.8 ;
    for(j=1;j<=ihole ;j++)
    {
	y=29.8+2.8*(1+ihole-j) ;
	stran=strand[*ie+12+5*ihole+j] ;
	sym=syms[*ie+12+5*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    /*c standard ending */ 
    x=56.8 ;
    for(j=1;j<=4 ;j++)
    {
	y=29.8-2.8*(j-1) ;
	stran=strand[*ie+12+6*ihole+j] ;
	sym=syms[*ie+12+6*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    ajGraphNewpage(graph,AJFALSE);
    return ;
}
 
/* @func conupbig2 ************************************************************
**
******************************************************************************/

void  conupbig2(const int is,int *ie,char *strand,int *syms)
{
    char    stran ;
    int     j,length,hole ;
    int     ihole,sym ;
    float   hold,x,y ;
    float   spacer,diff,rhole ;

/*c comment everything in this image is rotated 90 degrees from usual */ 
/*c and the image starts at the right side of the page and goes to the left */ 

/*c start out the plot by putting in the membrane lines */ 
 
/*c put in membrane line */ 
    topoMoveTo(10.0,99.0) ;
    topoDraw(140.0,99.0) ;
    topoMoveTo(10.0,99.1) ;
    topoDraw(140.0,99.1) ;
    topoMoveTo(10.0,99.2) ;
    topoDraw(140.0,99.2) ;
 
    hold=40.0 ;
/*c check to see the size of the connecting part */ 
    length=is-*ie-1 ;
    hole=length-61 ;
    rhole=(float)hole/14.0 ;
    ihole=hole/14 ;
    diff=rhole-ihole ;
    diff=14.0*diff ;
/*c standard start for the structure */ 
/*c down from the default starting point at the top */ 
    x=hold  ;
    for(j=1;j<=15 ;j++)
    {
	y=97.6-2.8*(j-1) ;
	stran=strand[*ie+j] ;
	sym=syms[*ie+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c over to the left */ 
    y=97.6-2.8*14.0 ;
    for(j=1;j<=7 ;j++)
    {
	x=hold-2.8*j ;
	stran=strand[*ie+j+15] ;
	sym=syms[*ie+j+15] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in the turning spacers */ 
    x=20.4 ;
    for(j=1;j<=2 ;j++)
    {
	y=97.6-2.8*(14+j) ;
	stran=strand[*ie+j+22] ;
	sym=syms[*ie+j+22] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c start the 14 loops for the data */ 
/*c down for loop 1 */ 
    spacer=0.0 ;
/*!	      if(nchain == 22)spacer=.45 */ 
    x=20.4 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(16+j) ;
	stran=strand[*ie+j+24] ;
	sym=syms[*ie+j+24] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= .99)
    {
	y=97.6-2.8*(17+ihole) ;
	x=21.15 ;
	stran=strand[*ie+25+ihole] ;
	sym=syms[*ie+25+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=23.95 ;
	stran=strand[*ie+26+ihole] ;
	sym=syms[*ie+26+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*(17+ihole) ;
	x=22.55 ;
	stran=strand[*ie+25+ihole] ;
	sym=syms[*ie+25+ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 2) */ 
    x=24.7 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(17+ihole-j) ;
	stran=strand[*ie+25+ihole+j] ;
	sym=syms[*ie+25+ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 1.99)
    {
	x=25.45 ;
	y=97.6-2.8*16 ;
	stran=strand[*ie+26+2*ihole] ;
	sym=syms[*ie+26+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=28.25 ;
	stran=strand[*ie+27+2*ihole] ;
	sym=syms[*ie+27+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*16 ;
	x=26.85 ;
	stran=strand[*ie+26+2*ihole] ;
	sym=syms[*ie+26+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 3 */ 
    x=29.0 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(16+j) ;
	stran=strand[*ie+26+2*ihole+j] ;
	sym=syms[*ie+26+2*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 2.99)
    {
	y=97.6-2.8*(17+ihole) ;
	x=29.75 ;
	stran=strand[*ie+27+3*ihole] ;
	sym=syms[*ie+27+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=32.55 ;
	stran=strand[*ie+28+3*ihole] ;
	sym=syms[*ie+28+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*(17+ihole) ;
	x=31.15 ;
	stran=strand[*ie+27+3*ihole] ;
	sym=syms[*ie+27+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 4) */ 
    x=33.3 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(17+ihole-j) ;
	stran=strand[*ie+27+3*ihole+j] ;
	sym=syms[*ie+27+3*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 3.99)
    {
	x=34.05 ;
	y=97.6-2.8*16 ;
	stran=strand[*ie+28+4*ihole] ;
	sym=syms[*ie+28+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=36.85 ;
	stran=strand[*ie+29+4*ihole] ;
	sym=syms[*ie+29+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*16 ;
	x=35.45 ;
	stran=strand[*ie+28+4*ihole] ;
	sym=syms[*ie+28+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 5 */ 
    x=37.6 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(16+j) ;
	stran=strand[*ie+28+4*ihole+j] ;
	sym=syms[*ie+28+4*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 4.99)
    {
	y=97.6-2.8*(17+ihole) ;
	x=38.35 ;
	stran=strand[*ie+29+5*ihole] ;
	sym=syms[*ie+29+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=41.15 ;
	stran=strand[*ie+30+5*ihole] ;
	sym=syms[*ie+30+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*(17+ihole) ;
	x=39.75 ;
	stran=strand[*ie+29+5*ihole] ;
	sym=syms[*ie+29+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 6) */ 
    x=41.9 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(17+ihole-j) ;
	stran=strand[*ie+29+5*ihole+j] ;
	sym=syms[*ie+29+5*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 5.99)
    {
	x=42.65 ;
	y=97.6-2.8*16 ;
	stran=strand[*ie+30+5*ihole] ;
	sym=syms[*ie+30+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=45.45 ;
	stran=strand[*ie+31+6*ihole] ;
	sym=syms[*ie+31+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*16 ;
	x=44.05 ;
	stran=strand[*ie+30+6*ihole] ;
	sym=syms[*ie+30+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 7 */ 
    x=46.2 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(16+j) ;
	stran=strand[*ie+30+6*ihole+j] ;
	sym=syms[*ie+30+6*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 6.99)
    {
	y=97.6-2.8*(17+ihole) ;
	x=46.95 ;
	stran=strand[*ie+31+7*ihole] ;
	sym=syms[*ie+31+7*ihole] ;
	   topoSymbol(x,y,stran,sym) ;
	x=49.75 ;
	stran=strand[*ie+32+7*ihole] ;
	sym=syms[*ie+32+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*(17+ihole) ;
	x=48.35 ;
	stran=strand[*ie+31+7*ihole] ;
	sym=syms[*ie+31+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 8) */ 
    x=50.5 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(17+ihole-j) ;
	stran=strand[*ie+31+7*ihole+j] ;
	sym=syms[*ie+31+7*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 7.99)
    {
	x=51.25 ;
	y=97.6-2.8*16 ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=54.05 ;
	stran=strand[*ie+33+8*ihole] ;
	sym=syms[*ie+33+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*16 ;
	x=52.65 ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 9 */ 
    x=54.8 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(16+j) ;
	stran=strand[*ie+32+8*ihole+j] ;
	sym=syms[*ie+32+8*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 8.99)
    {
	y=97.6-2.8*(17+ihole) ;
	x=55.55 ;
	stran=strand[*ie+33+9*ihole] ;
	sym=syms[*ie+33+9*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=58.35 ;
	stran=strand[*ie+34+9*ihole] ;
	sym=syms[*ie+34+9*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*(17+ihole) ;
	x=56.95 ;
	stran=strand[*ie+33+9*ihole] ;
	sym=syms[*ie+33+9*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 10) */ 
    x=59.1 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(17+ihole-j) ;
	stran=strand[*ie+33+9*ihole+j] ;
	sym=syms[*ie+33+9*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 9.99)
    {
	x=59.85 ;
	y=97.6-2.8*16 ;
	stran=strand[*ie+34+10*ihole] ;
	sym=syms[*ie+34+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=62.65 ;
	stran=strand[*ie+35+10*ihole] ;
	sym=syms[*ie+35+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*16 ;
	x=61.25 ;
	stran=strand[*ie+34+10*ihole] ;
	sym=syms[*ie+34+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 11 */ 
    x=63.4 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(16+j) ;
	stran=strand[*ie+34+10*ihole+j] ;
	sym=syms[*ie+34+10*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 10.99)
    {
	y=97.6-2.8*(17+ihole) ;
	x=64.15 ;
	stran=strand[*ie+35+11*ihole] ;
	sym=syms[*ie+35+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=66.95 ;
	stran=strand[*ie+36+11*ihole] ;
	sym=syms[*ie+36+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*(17+ihole) ;
	x=65.55 ;
	stran=strand[*ie+35+11*ihole] ;
	sym=syms[*ie+35+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 12) */ 
    x=67.7 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(17+ihole-j) ;
	stran=strand[*ie+35+11*ihole+j] ;
	sym=syms[*ie+35+11*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 11.99)
    {
	x=68.45 ;
	y=97.6-2.8*16 ;
	stran=strand[*ie+36+12*ihole] ;
	sym=syms[*ie+36+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=71.25 ;
	stran=strand[*ie+37+12*ihole] ;
	sym=syms[*ie+37+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*16 ;
	x=69.85 ;
	stran=strand[*ie+36+12*ihole] ;
	sym=syms[*ie+36+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 13 */ 
    x=72.0 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(16+j) ;
	stran=strand[*ie+36+12*ihole+j] ;
	sym=syms[*ie+36+12*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 12.99)
    {
	y=97.6-2.8*(17+ihole) ;
	x=72.75 ;
	stran=strand[*ie+37+13*ihole] ;
	sym=syms[*ie+37+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=75.55 ;
	stran=strand[*ie+38+13*ihole] ;
	sym=syms[*ie+38+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=97.6-2.8*(17+ihole) ;
	x=74.15 ;
	stran=strand[*ie+37+13*ihole] ;
	sym=syms[*ie+37+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 14) */ 
    x=76.3 ;
    for(j=1;j<=ihole ;j++)
    {
	y=97.6-2.8*(17+ihole-j) ;
	stran=strand[*ie+37+13*ihole+j] ;
	sym=syms[*ie+37+13*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in the spacers */ 
    for(j=1;j<=2 ;j++)
    {
	y=97.6-2.8*(17-j) ;
	stran=strand[*ie+37+14*ihole+j] ;
	sym=syms[*ie+37+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c move back to the stem area */ 
    y=97.6-2.8*14.0 ;
    for(j=1;j<=7 ;j++)
    {
	x=76.3-2.8*(j-1) ;
	stran=strand[*ie+39+14*ihole+j] ;
	sym=syms[*ie+39+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c back up to the top */ 
    y=97.6-2.8*14.0 ;
    x=76.3-2.8*7.0 ;
    for(j=1;j<=15 ;j++)
    {
	y=97.6-2.8*(15-j) ;
	stran=strand[*ie+46+14*ihole+j] ;
	sym=syms[*ie+46+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
 
    ajGraphicsSetCharscale(0.5) ;
    topoMoveTo(105.0,5.0) ;
/*c	call topoPlotText(title)    */ 
/*!	call closef(fn) */ 
    ajGraphNewpage(graph,AJFALSE);
    return ;
}
 
/* @func condownbig2 **********************************************************
**
******************************************************************************/

void  condownbig2(const int is,int *ie,char *strand,int *syms)
{
    char    stran ;
    int     j,length,hole ;
    int     ihole,sym ;
    float   hold,x,y ;
    float   spacer,diff,rhole ;

/*c comment everything in this image is rotated 90 degrees from usual */ 
/*c and the image starts at the right side of the page and goes to the left */ 
 
/*c start out the plot by putting in the membrane lines */ 
 
/*c put in membrane line */ 
    topoMoveTo(10.0,1.0) ;
    topoDraw(140.0,1.0) ;
    topoMoveTo(10.0,1.1) ;
    topoDraw(140.0,1.1) ;
    topoMoveTo(10.0,1.2) ;
    topoDraw(10.0,1.2) ;
 
    hold=40.0 ;
/*c check to see the size of the connecting part */ 
    length=is-*ie-1 ;
    hole=length-61 ;
    rhole=(float)hole/14.0 ;
    ihole=hole/14 ;
    diff=rhole-ihole ;
    diff=14.0*diff ;
/*c	call printf("\n rhole and ihole are %f %d",rhole,ihole) */ 
/*c standard start for the structure */ 
/*c down from the default starting point at the top */ 
    x=hold ;
    for(j=1;j<=15 ;j++)
    {
	y=2.4+2.8*(j-1) ;
	stran=strand[*ie+j] ;
	sym=syms[*ie+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c over to the left */ 
    y=2.4+2.8*14.0 ;
    for(j=1;j<=7 ;j++)
    {
	x=hold-2.8*j ;
	stran=strand[*ie+15+j] ;
	sym=syms[*ie+15+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in the turning spacers */ 
    x=20.4 ;
    for(j=1;j<=2 ;j++)
    {
	y=2.4+2.8*(14+j) ;
	stran=strand[*ie+22+j] ;
	sym=syms[*ie+22+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c start the 14 loops for the data */ 
/*c down for loop 1 */ 
    spacer=0.0 ;
    x=20.4 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(16+j) ;
	stran=strand[*ie+24+j] ;
	sym=syms[*ie+24+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= .99)
    {
	y=2.4+2.8*(17+ihole) ;
	x=21.15 ;
	stran=strand[*ie+25+ihole] ;
	sym=syms[*ie+25+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=23.95 ;
	stran=strand[*ie+26+ihole] ;
	sym=syms[*ie+26+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(17+ihole) ;
	x=22.55 ;
	stran=strand[*ie+25+ihole] ;
	sym=syms[*ie+25+ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 2) */ 
    x=24.7 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(17+ihole-j) ;
	stran=strand[*ie+25+ihole+j] ;
	sym=syms[*ie+25+ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 1.99)
    {
	x=25.45 ;
	y=2.4+2.8*16 ;
	stran=strand[*ie+26+2*ihole] ;
	sym=syms[*ie+26+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=28.25 ;
	stran=strand[*ie+27+2*ihole] ;
	sym=syms[*ie+27+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*16 ;
	x=26.85 ;
	stran=strand[*ie+26+2*ihole] ;
	sym=syms[*ie+26+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 3 */ 
    x=29.0 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(16+j) ;
	stran=strand[*ie+26+2*ihole+j] ;
	sym=syms[*ie+26+2*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 2.99)
    {
	y=2.4+2.8*(17+ihole) ;
	x=29.75 ;
	stran=strand[*ie+27+3*ihole] ;
	sym=syms[*ie+27+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=32.55 ;
	stran=strand[*ie+28+3*ihole] ;
	sym=syms[*ie+28+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(17+ihole) ;
	x=31.15 ;
	stran=strand[*ie+27+3*ihole] ;
	sym=syms[*ie+27+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 4) */ 
    x=33.3 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(17+ihole-j) ;
	stran=strand[*ie+27+3*ihole+j] ;
	sym=syms[*ie+27+3*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 3.99)
    {
	x=34.05 ;
	y=2.4+2.8*16 ;
	stran=strand[*ie+28+4*ihole] ;
	sym=syms[*ie+28+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=36.85 ;
	stran=strand[*ie+29+4*ihole] ;
	sym=syms[*ie+29+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*16 ;
	x=35.45 ;
	stran=strand[*ie+28+4*ihole] ;
	sym=syms[*ie+28+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 5 */ 
    x=37.6 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(16+j) ;
	stran=strand[*ie+28+4*ihole+j] ;
	sym=syms[*ie+28+4*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 4.99)
    {
	y=2.4+2.8*(17+ihole) ;
	x=38.35 ;
	stran=strand[*ie+29+5*ihole] ;
	sym=syms[*ie+29+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=41.15 ;
	stran=strand[*ie+30+5*ihole] ;
	sym=syms[*ie+30+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(17+ihole) ;
	x=39.75 ;
	stran=strand[*ie+29+5*ihole] ;
	sym=syms[*ie+29+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 6) */ 
    x=41.9 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(17+ihole-j) ;
	stran=strand[*ie+29+5*ihole+j] ;
	sym=syms[*ie+29+5*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 5.99)
    {
	x=42.65 ;
	y=2.4+2.8*16 ;
	stran=strand[*ie+30+6*ihole] ;
	sym=syms[*ie+30+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=45.45 ;
	stran=strand[*ie+31+6*ihole] ;
	sym=syms[*ie+31+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*16 ;
	x=44.05 ;
	stran=strand[*ie+30+6*ihole] ;
	sym=syms[*ie+30+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 7 */ 
    x=46.2 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(16+j) ;
	stran=strand[*ie+30+6*ihole+j] ;
	sym=syms[*ie+30+6*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 6.99)
    {
	y=2.4+2.8*(17+ihole) ;
	x=46.95 ;
	stran=strand[*ie+31+7*ihole] ;
	sym=syms[*ie+31+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=49.75 ;
	stran=strand[*ie+32+7*ihole] ;
	sym=syms[*ie+32+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(17+ihole) ;
	x=48.35 ;
	stran=strand[*ie+31+7*ihole] ;
	sym=syms[*ie+31+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 8) */ 
    x=50.5 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(17+ihole-j) ;
	stran=strand[*ie+31+7*ihole+j] ;
	sym=syms[*ie+31+7*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 7.99)
    {
	x=51.25 ;
	y=2.4+2.8*16.0 ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=54.05 ;
	stran=strand[*ie+33+8*ihole] ;
	sym=syms[*ie+33+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*16.0 ;
	x=52.65 ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
    /*c down for loop 9 */ 
    x=54.8 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(16+j) ;
	stran=strand[*ie+32+8*ihole+j] ;
	sym=syms[*ie+32+8*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c pu in end cap(s) */ 
    if(diff >= 8.99)
    {
	x=55.55 ;
	y=2.4+2.8*(17+ihole) ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=58.35 ;
	stran=strand[*ie+33+9*ihole] ;
	sym=syms[*ie+33+9*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(17+ihole) ;
	x=56.95 ;
	stran=strand[*ie+33+9*ihole] ;
	sym=syms[*ie+33+9*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 10) */ 
    x=59.1 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(17+ihole-j) ;
	stran=strand[*ie+33+9*ihole+j] ;
	sym=syms[*ie+33+9*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }                  /*end for*/
    if(diff >= 9.99)
    {
	x=59.85 ;
	y=2.4+2.8*16 ;
	stran=strand[*ie+34+10*ihole] ;
	sym=syms[*ie+34+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=62.65 ;
	stran=strand[*ie+35+10*ihole] ;
	sym=syms[*ie+35+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*16 ;
	x=61.25 ;
	stran=strand[*ie+34+10*ihole] ;
	sym=syms[*ie+34+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 11 */ 
    x=63.4 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(16+j) ;
	stran=strand[*ie+34+10*ihole+j] ;
	sym=syms[*ie+34+10*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    if(diff >= 10.99)
    {
	x=64.15 ;
	y=2.4+2.8*(17+ihole) ;
	stran=strand[*ie+35+11*ihole] ;
	sym=syms[*ie+35+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=66.95 ;
	stran=strand[*ie+36+11*ihole] ;
	sym=syms[*ie+36+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(17+ihole) ;
	x=65.55 ;
	stran=strand[*ie+35+11*ihole] ;
	sym=syms[*ie+35+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 12) */ 
    x=67.77 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(17+ihole-j) ;
	stran=strand[*ie+35+11*ihole+j] ;
	sym=syms[*ie+35+11*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    if(diff >= 11.99)
    {
	x=68.45 ;
	y=2.4+2.8*16 ;
	stran=strand[*ie+36+12*ihole] ;
	sym=syms[*ie+36+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=71.25 ;
	stran=strand[*ie+37+12*ihole] ;
	sym=syms[*ie+37+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*16 ;
	x=69.85 ;
	stran=strand[*ie+36+12*ihole] ;
	sym=syms[*ie+36+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 13 */ 
    x=72.0 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(16+j) ;
	stran=strand[*ie+36+12*ihole+j] ;
	sym=syms[*ie+36+12*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    if(diff >= 12.99)
    {
	x=72.75 ;
	y=2.4+2.8*(17+ihole) ;
	stran=strand[*ie+37+13*ihole] ;
	sym=syms[*ie+37+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=75.55 ;
	stran=strand[*ie+38+13*ihole] ;
	sym=syms[*ie+38+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(17+ihole) ;
	x=74.15 ;
	stran=strand[*ie+37+13*ihole] ;
	sym=syms[*ie+37+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 14) */ 
    x=76.3 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(17+ihole-j) ;
	stran=strand[*ie+37+13*ihole+j] ;
	sym=syms[*ie+37+13*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }     /*end for*/
/*c put in the spacers */ 
    for(j=1;j<=2 ;j++)
    {
	y=2.4+2.8*(17-j) ;
	stran=strand[*ie+37+14*ihole+j] ;
	sym=syms[*ie+37+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c move back to the stem area */ 
    y=2.4+2.8*14.0 ;
    for(j=1;j<=7 ;j++)
    {
	x=76.3-2.8*(j-1) ;
	stran=strand[*ie+39+14*ihole+j] ;
	sym=syms[*ie+39+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c back up to the top */ 
    y=2.4+2.8*14.0 ;
    x=76.3-2.8*7.0 ;
    for(j=1;j<=15 ;j++)
    {
	y=2.4+2.8*(15-j) ;
	stran=strand[*ie+46+14*ihole+j] ;
	sym=syms[*ie+46+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
 
    ajGraphicsSetCharscale(0.5) ;
    topoMoveTo(105.0,5.0) ;
/*c        call topoPlotText(title) */ 
/*!       call closef(fn) */ 
    ajGraphNewpage(graph,AJFALSE);
    return ;
}    /*end subroutine*/


/* @func condownbig3 **********************************************************
**
******************************************************************************/

void  condownbig3(const int is,int *ie,char *strand,int *syms)
{
    char    stran ;
    int     j,length,hole ;
    int     ihole,sym ;
    float   hold,x,y ;
    float   spacer,diff,rhole ;
 
/*c comment everything in this image is rotated 90 degrees from usual */ 
/*c and the image starts at the right side of the page and goes to the left */ 
 
/*c start out the plot by putting in the membrane lines */ 
 
    hold=40.0 ;
/*c check to see the size of the connecting part */ 
    length=is-*ie-1 ;
    hole=length-61 ;
    rhole=(float)hole/14.0 ;
    ihole=hole/14 ;
    diff=rhole-ihole ;
    diff=14.0*diff ;
/*c	call printf("\n rhole and ihole are %f %d",rhole,ihole) */ 
       
/*c standard start for the structure */ 
/*c down from the default starting point at the top */ 
    x=hold ;
    for(j=1;j<=2 ;j++)
    {
	y=2.4+2.8*(j-1) ;
	stran=strand[*ie+j+13] ;
	sym=syms[*ie+j+13] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c over to the left */ 
    y=2.4+2.8*1.0 ;
    for(j=1;j<=7 ;j++)
    {
	x=hold-2.8*j ;
	stran=strand[*ie+15+j] ;
	sym=syms[*ie+15+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in the turning spacers */ 
    x=20.4 ;
    for(j=1;j<=2 ;j++)
    {
	y=2.4+2.8*(1+j) ;
	stran=strand[*ie+22+j] ;
	sym=syms[*ie+22+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c start the 14 loops for the data */ 
/*c down for loop 1 */ 
    spacer=0.0 ;
    x=20.4 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(3+j) ;
	stran=strand[*ie+24+j] ;
	sym=syms[*ie+24+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= .99)
    {
	y=2.4+2.8*(4+ihole) ;
	x=21.15 ;
	stran=strand[*ie+25+ihole] ;
	sym=syms[*ie+25+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=23.95 ;
	stran=strand[*ie+26+ihole] ;
	sym=syms[*ie+26+ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(4+ihole) ;
	x=22.55 ;
	stran=strand[*ie+25+ihole] ;
	sym=syms[*ie+25+ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 2) */ 
    x=24.7 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(4+ihole-j) ;
	stran=strand[*ie+25+ihole+j] ;
	sym=syms[*ie+25+ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 1.99)
    {
	x=25.45 ;
	y=2.4+2.8*3.0 ;
	stran=strand[*ie+26+2*ihole] ;
	sym=syms[*ie+26+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=28.25 ;
	stran=strand[*ie+27+2*ihole] ;
	sym=syms[*ie+27+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*3.0 ;
	x=26.85 ;
	stran=strand[*ie+26+2*ihole] ;
	sym=syms[*ie+26+2*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 3 */ 
    x=29.0 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(3+j) ;
	stran=strand[*ie+26+2*ihole+j] ;
	sym=syms[*ie+26+2*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 2.99)
    {
	y=2.4+2.8*(4+ihole) ;
	x=29.75 ;
	stran=strand[*ie+27+3*ihole] ;
	sym=syms[*ie+27+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=32.55 ;
	stran=strand[*ie+28+3*ihole] ;
	sym=syms[*ie+28+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(4+ihole) ;
	x=31.15 ;
	stran=strand[*ie+27+3*ihole] ;
	sym=syms[*ie+27+3*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 4) */ 
    x=33.3 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(4+ihole-j) ;
	stran=strand[*ie+27+3*ihole+j] ;
	sym=syms[*ie+27+3*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 3.99)
    {
	x=34.05 ;
	y=2.4+2.8*3.0 ;
	stran=strand[*ie+28+4*ihole] ;
	sym=syms[*ie+28+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=36.85 ;
	stran=strand[*ie+29+4*ihole] ;
	sym=syms[*ie+29+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*3.0 ;
	x=35.45 ;
	stran=strand[*ie+28+4*ihole] ;
	sym=syms[*ie+28+4*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 5 */ 
    x=37.6 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(3+j) ;
	stran=strand[*ie+28+4*ihole+j] ;
	sym=syms[*ie+28+4*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 4.99)
    {
	y=2.4+2.8*(4+ihole) ;
	x=38.35 ;
	stran=strand[*ie+29+5*ihole] ;
	sym=syms[*ie+29+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=41.15 ;
	stran=strand[*ie+30+5*ihole] ;
	sym=syms[*ie+30+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(4+ihole) ;
	x=39.75 ;
	stran=strand[*ie+29+5*ihole] ;
	sym=syms[*ie+29+5*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 6) */ 
    x=41.9 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(4+ihole-j) ;
	stran=strand[*ie+29+5*ihole+j] ;
	sym=syms[*ie+29+5*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 5.99)
    {
	x=42.65 ;
	y=2.4+2.8*3.0 ;
	stran=strand[*ie+30+6*ihole] ;
	sym=syms[*ie+30+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=45.45 ;
	stran=strand[*ie+31+6*ihole] ;
	sym=syms[*ie+31+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*3.0 ;
	x=44.05 ;
	stran=strand[*ie+30+6*ihole] ;
	sym=syms[*ie+30+6*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 7 */ 
    x=46.2 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(3+j) ;
	stran=strand[*ie+30+6*ihole+j] ;
	sym=syms[*ie+30+6*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 6.99)
    {
	y=2.4+2.8*(4+ihole) ;
	x=46.95 ;
	stran=strand[*ie+31+7*ihole] ;
	sym=syms[*ie+31+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=49.75 ;
	stran=strand[*ie+32+7*ihole] ;
	sym=syms[*ie+32+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(4+ihole) ;
	x=48.35 ;
	stran=strand[*ie+31+7*ihole] ;
	sym=syms[*ie+31+7*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 8) */ 
    x=50.5 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(4+ihole-j) ;
	stran=strand[*ie+31+7*ihole+j] ;
	sym=syms[*ie+31+7*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c put in end cap(s) */ 
    if(diff >= 7.99)
    {
	x=51.25 ;
	y=2.4+2.8*3.0 ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=54.05 ;
	stran=strand[*ie+33+8*ihole] ;
	sym=syms[*ie+33+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*3.0 ;
	x=52.65 ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 9 */ 
    x=54.8 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(3+j) ;
	stran=strand[*ie+32+8*ihole+j] ;
	sym=syms[*ie+32+8*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c pu in end cap(s) */ 
    if(diff >= 8.99)
    {
	x=55.55 ;
	y=2.4+2.8*(4+ihole) ;
	stran=strand[*ie+32+8*ihole] ;
	sym=syms[*ie+32+8*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=58.35 ;
	stran=strand[*ie+33+9*ihole] ;
	sym=syms[*ie+33+9*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(4+ihole) ;
	x=56.95 ;
	stran=strand[*ie+33+9*ihole] ;
	sym=syms[*ie+33+9*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 10) */ 
    x=59.1 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(4+ihole-j) ;
	stran=strand[*ie+33+9*ihole+j] ;
	sym=syms[*ie+33+9*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }                  /*end for*/
    if(diff >= 9.99)
    {
	x=59.85 ;
	y=2.4+2.8*3.0 ;
	stran=strand[*ie+34+10*ihole] ;
	sym=syms[*ie+34+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=62.65 ;
	stran=strand[*ie+35+10*ihole] ;
	sym=syms[*ie+35+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*3.0 ;
	x=61.25 ;
	stran=strand[*ie+34+10*ihole] ;
	sym=syms[*ie+34+10*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 11 */ 
    x=63.4 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(3+j) ;
	stran=strand[*ie+34+10*ihole+j] ;
	sym=syms[*ie+34+10*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    if(diff >= 10.99)
    {
	x=64.15 ;
	y=2.4+2.8*(4+ihole) ;
	stran=strand[*ie+35+11*ihole] ;
	sym=syms[*ie+35+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=66.95 ;
	stran=strand[*ie+36+11*ihole] ;
	sym=syms[*ie+36+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(4+ihole) ;
	x=65.55 ;
	stran=strand[*ie+35+11*ihole] ;
	sym=syms[*ie+35+11*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 12) */ 
    x=67.77 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(4+ihole-j) ;
	stran=strand[*ie+35+11*ihole+j] ;
	sym=syms[*ie+35+11*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    if(diff >= 11.99)
    {
	x=68.45 ;
	y=2.4+2.8*3.0 ;
	stran=strand[*ie+36+12*ihole] ;
	sym=syms[*ie+36+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=71.25 ;
	   stran=strand[*ie+37+12*ihole] ;
	sym=syms[*ie+37+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*3.0 ;
	x=69.85 ;
	stran=strand[*ie+36+12*ihole] ;
	sym=syms[*ie+36+12*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c down for loop 13 */ 
    x=72.0 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(3+j) ;
	stran=strand[*ie+36+12*ihole+j] ;
	sym=syms[*ie+36+12*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
    if(diff >= 12.99)
    {
	x=72.75 ;
	y=2.4+2.8*(4+ihole) ;
	stran=strand[*ie+37+13*ihole] ;
	sym=syms[*ie+37+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	x=75.55 ;
	stran=strand[*ie+38+13*ihole] ;
	sym=syms[*ie+38+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
	*ie=*ie+1 ;
    }
    else
    {
	y=2.4+2.8*(4+ihole) ;
	x=74.15 ;
	stran=strand[*ie+37+13*ihole] ;
	sym=syms[*ie+37+13*ihole] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end if*/
/*c draw back towards the top (loop 14) */ 
    x=76.3 ;
    for(j=1;j<=ihole ;j++)
    {
	y=2.4+2.8*(4+ihole-j) ;
	stran=strand[*ie+37+13*ihole+j] ;
	sym=syms[*ie+37+13*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }     /*end for*/
/*c put in the spacers */ 
    for(j=1;j<=2 ;j++)
    {
	y=2.4+2.8*(4-j) ;
	stran=strand[*ie+37+14*ihole+j] ;
	sym=syms[*ie+37+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c move back to the stem area */ 
    y=2.4+2.8*1.0 ;
    for(j=1;j<=7 ;j++)
    {
	x=76.3-2.8*(j-1) ;
	stran=strand[*ie+39+14*ihole+j] ;
	sym=syms[*ie+39+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
/*c back up to the top */ 
    y=2.4+2.8*1.0 ;
    x=76.3-2.8*7.0 ;
    for(j=1;j<=2 ;j++)
    {
	y=2.4+2.8*(2-j) ;
	stran=strand[*ie+46+14*ihole+j] ;
	sym=syms[*ie+46+14*ihole+j] ;
	topoSymbol(x,y,stran,sym) ;
    }  /*end for*/
 
    ajGraphicsSetCharscale(0.5) ;
    topoMoveTo(105.0,5.0) ;
/*c        call topoPlotText(title) */ 
/*!       call closef(fn) */ 
    ajGraphNewpage(graph,AJFALSE);
    return ;
}    /*end subroutine*/
