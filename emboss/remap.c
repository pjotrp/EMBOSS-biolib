/* @source remap application
**
** Display a sequence with restriction cut sites
**
** @author: Copyright (C) Gary Williams (gwilliam@hgmp.mrc.ac.uk)
** 18 Jan 2000 - GWW - written
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "emboss.h"


#define ENZDATA "REBASE/embossre.enz"
#define EQUDATA "embossre.equ"
#define EQUGUESS 3500     /* Estimate of number of equivalent names */

#define TABLEGUESS 200

/* declare functions */
static void remap_read_equiv(AjPFile *equfile, AjPTable *table);
static void remap_CutList (AjPFile outfile, AjPList restrictlist, AjBool isos,
			   AjBool html);
static void remap_NoCutList (AjPFile outfile, AjPList restrictlist,
			     AjBool html, AjPStr enzymes, AjBool blunt,
			     AjBool sticky, ajint sitelen);
static void remap_read_file_of_enzyme_names(AjPStr *enzymes);

/* structure for counts and isoschizomers of a restriction enzyme hit */
typedef struct SValue
{
    ajint    count;
    AjPStr iso;
} OValue, *PValue;



/* @prog remap ***************************************************************
**
** Display a sequence with restriction cut sites, translation etc
**
******************************************************************************/

int main(int argc, char **argv)
{

    ajint begin, end;
    AjPSeqall seqall;
    AjPSeq seq;
    EmbPShow ss;
    AjPFile outfile;
    AjPStr * tablelist;
    ajint table;
    AjPRange uppercase;
    AjPRange highlight;
    AjBool threeletter;
    AjBool numberseq;
    AjBool nameseq;
    ajint width;
    ajint length;
    ajint margin;
    AjBool description;
    ajint offset;
    AjBool html;
    AjPStr descriptionline;
    AjPFeattable feat;
    ajint orfminsize;
    AjPTrn trnTable;
    AjBool translation;
    AjBool reverse;
    AjBool cutlist;
    AjBool flat;
    EmbPMatMatch mm=NULL;
    
    /* stuff lifted from Alan's 'restrict.c' */
    AjPStr    enzymes=NULL;
    ajint mincuts;
    ajint maxcuts;
    ajint sitelen;
    AjBool single;
    AjBool blunt;
    AjBool sticky;
    AjBool ambiguity;
    AjBool plasmid;
    AjBool commercial;
    AjBool limit;
    AjBool equiv;
    AjPFile   enzfile=NULL;
    AjPFile   equfile=NULL;
    AjPTable  retable=NULL;
    ajint       hits;
    AjPList     restrictlist=NULL;



    (void) embInit ("remap", argc, argv);

    seqall = ajAcdGetSeqall ("sequence");
    outfile = ajAcdGetOutfile ("outfile");
    tablelist = ajAcdGetList ("table");
    uppercase = ajAcdGetRange ("uppercase");
    highlight = ajAcdGetRange ("highlight");
    threeletter = ajAcdGetBool ("threeletter");
    numberseq = ajAcdGetBool ("number");
    width = ajAcdGetInt ("width");
    length = ajAcdGetInt ("length");
    margin = ajAcdGetInt ("margin");
    nameseq = ajAcdGetBool ("name");
    description = ajAcdGetBool ("description");
    offset = ajAcdGetInt ("offset");
    html = ajAcdGetBool ("html");
    orfminsize = ajAcdGetInt ("orfminsize");
    translation = ajAcdGetBool ("translation");
    reverse = ajAcdGetBool ("reverse");
    cutlist = ajAcdGetBool ("cutlist");
    flat = ajAcdGetBool ("flatreformat");

    /*  restriction enzyme stuff */
    mincuts = ajAcdGetInt ("mincuts");
    maxcuts = ajAcdGetInt ("maxcuts");
    sitelen  = ajAcdGetInt ("sitelen");
    single = ajAcdGetBool ("single");
    blunt = ajAcdGetBool ("blunt");
    sticky = ajAcdGetBool ("sticky");
    ambiguity = ajAcdGetBool ("ambiguity");
    plasmid = ajAcdGetBool ("plasmid");
    commercial = ajAcdGetBool ("commercial");
    limit = ajAcdGetBool ("limit");
    enzymes = ajAcdGetString ("enzymes");
    equiv = ajAcdGetBool("preferred");

    if (!blunt  && !sticky)
	ajFatal("Blunt/Sticky end cutters shouldn't both be disabled.");    

    /* get the number of the genetic code used */
    (void) ajStrToInt(tablelist[0], &table);
    trnTable = ajTrnNewI(table);

    /* read the local file of enzymes names */
    remap_read_file_of_enzyme_names(&enzymes);

    while (ajSeqallNext(seqall, &seq))
    {
	/* get begin and end positions */
	begin = ajSeqBegin(seq)-1;
	end = ajSeqEnd(seq)-1;

	/* do the name and description */
	if (nameseq)
	{
	    if (html)
		(void) ajFmtPrintF(outfile, "<H2>%S</H2>\n",
				   ajSeqGetName(seq));
	    else
		(void) ajFmtPrintF(outfile, "%S\n", ajSeqGetName(seq));
	}

	if (description)
	{
	    /*
	     *  wrap the description line at the width of the sequence
	     *  plus margin
	     */
	    if (html)
		(void) ajFmtPrintF(outfile, "<H3>%S</H3>\n",
				   ajSeqGetDesc(seq));
	    else
	    {
		descriptionline = ajStrNew();
		(void) ajStrAss(&descriptionline, ajSeqGetDesc(seq));
		(void) ajStrWrap(&descriptionline, width+margin);
		(void) ajFmtPrintF(outfile, "%S\n", descriptionline);
		(void) ajStrDel(&descriptionline);
	    }
	}

	/* get the feature table of the sequence */
	feat = ajSeqCopyFeat(seq);

	/* get the restriction cut sites */
	/*
	 *  most of this is lifted from the program 'restrict.c' by Alan
	 *  Bleasby
	 */    
	if (single)
	    maxcuts=mincuts=1;

	retable = ajStrTableNew(EQUGUESS);
	ajFileDataNewC(ENZDATA, &enzfile);
	if(!enzfile)
	    ajFatal("Cannot locate enzyme file. Run REBASEEXTRACT");
	if (equiv)
	{
	    ajFileDataNewC(EQUDATA,&equfile);
	    if (!equfile)
		equiv=ajFalse;
	    else
		remap_read_equiv(&equfile, &retable);   
	}    

	ajFileSeek(enzfile, 0L, 0);
	hits = embPatRestrictMatch(seq, begin+1, end+1, enzfile, enzymes,
				   sitelen,plasmid, ambiguity, mincuts,
				   maxcuts, blunt, sticky, commercial,
				   &restrictlist);
	if (hits)
	    /* this bit is lifted from printHits */
	    (void) embPatRestrictRestrict(&restrictlist, hits, !limit,
					  ajFalse);

	/* tidy up */
	ajFileClose(&enzfile);      


	/* make the Show Object */
	ss = embShowNew(seq, begin, end, width, length, margin, html, offset);

	if (html)
	    (void) ajFmtPrintF(outfile, "<PRE>");

	/* create the format to display */
	(void) embShowAddBlank(ss);
	(void) embShowAddRE (ss, 1, restrictlist, flat);
	(void) embShowAddSeq(ss, numberseq, threeletter, uppercase, highlight);

	if (!numberseq)
	    (void) embShowAddTicknum(ss);
	(void) embShowAddTicks(ss);

	if (reverse)
	{
	    (void) embShowAddComp(ss, numberseq);
	    (void) embShowAddRE (ss, -1, restrictlist, flat);
	}

	if (translation)
	{
	    if (reverse)
		(void) embShowAddBlank(ss);

	    (void) embShowAddTran (ss, trnTable, 1, threeletter, numberseq,
				   NULL, orfminsize);
	    (void) embShowAddTran (ss, trnTable, 2, threeletter, numberseq,
				   NULL, orfminsize);
	    (void) embShowAddTran (ss, trnTable, 3, threeletter, numberseq,
				   NULL, orfminsize);

	    if (reverse)
	    {
		(void) embShowAddTicks(ss);
		(void) embShowAddTran (ss, trnTable, -3, threeletter,
				       numberseq, NULL, orfminsize);
		(void) embShowAddTran (ss, trnTable, -2, threeletter,
				       numberseq, NULL, orfminsize);
		(void) embShowAddTran (ss, trnTable, -1, threeletter,
				       numberseq, NULL, orfminsize);
	    }
	}

	(void) embShowPrint(outfile, ss);

	/* display a list of the Enzymes that cut and don't cut */
	if (cutlist)
	{
	    remap_CutList(outfile, restrictlist, limit, html);
	    remap_NoCutList(outfile, restrictlist, html, enzymes, blunt,
			    sticky, sitelen);
	}

	/* tidy up */
	(void) embShowDel(&ss);
	(void) ajFeattableDel(&feat);
	while(ajListPop(restrictlist,(void **)&mm))
	    embMatMatchDel(&mm);
	(void) ajListDel(&restrictlist);



	/* add a gratuitous newline at the end of the sequence */
	(void) ajFmtPrintF(outfile, "\n");

	if (html)
	    (void) ajFmtPrintF(outfile, "<PRE>");

	ajStrTableFree(&retable);
    }

    /* tidy up */
    ajTrnDel (&trnTable);

    ajExit ();
    return 0;
}



/* @funcstatic remap_CutList *************************************************
**
** display a list of the enzymes that cut
**
** @param [r] outfile [AjPFile] file to print to.
** @param [r] restrictlist [AjPList] List to print.
** @param [r] isos [AjBool] True if allow isoschizomers
** @param [r] html [AjBool] dump out html if true.
** @return [void]
** @@
******************************************************************************/

static void remap_CutList (AjPFile outfile, AjPList restrictlist, AjBool isos,
			   AjBool html)
{
    AjPTable table = ajStrTableNewCase (TABLEGUESS);
    PValue value;
    AjPStr key;
    AjIList miter;		/* iterator for matches list */
    EmbPMatMatch m=NULL;	/* restriction enzyme match structure */
    void **array;		/* array for table */  
    ajint i;

    /* print title */
    if (html)
	(void) ajFmtPrintF(outfile, "<H2>");  
    (void) ajFmtPrintF(outfile, "\n\n# Enzymes that cut  Frequency");
    if (isos)
	(void) ajFmtPrintF(outfile, "\tIsoschizomers\n");
    else
	(void) ajFmtPrintF(outfile, "\n");

    if (html)
	(void) ajFmtPrintF(outfile, "</H2>");  

    /* if no hits then ignore much of this routine */
    if (ajListLength(restrictlist))
    {
	miter = ajListIter(restrictlist);
	while ((m = ajListIterNext(miter)) != NULL)
	{
	    key = m->cod;

	    /* increment the count of key */
	    value = (PValue) ajTableGet(table, (const void *)key);
	    if (value == NULL)
	    {
		AJNEW0(value);
		value->count = 1;          	
		value->iso = ajStrNew();
		ajStrAss(&(value->iso), m->iso);
	    }
	    else
		value->count++;

	    ajTablePut(table, (const void *)key, (void *)value);
	}
	(void) ajListIterFree(miter);

	/* print out results */
	if (html)
	    (void) ajFmtPrintF(outfile, "<PRE>");

	if (ajTableLength(table))
	{
	    array = ajTableToarray(table, NULL);
	    qsort(array, ajTableLength(table), 2*sizeof (*array), ajStrCmp);
	    for (i = 0; array[i]; i += 2)
	    {
		value = (PValue) array[i+1];
		(void) ajFmtPrintF (outfile, "%10S\t    %d\t%S\n",
				    (AjPStr) array[i], value->count,
				    value->iso);
		ajStrDel(&(value->iso));
		AJFREE(array[i+1]);	/* free the ajint* value */
	    }  
	    AJFREE(array);
	}
    }
    (void) ajFmtPrintF (outfile, "\n");
    if (html)
	(void) ajFmtPrintF(outfile, "</PRE>");

    /* tidy up */
    ajTableFree (&table);

    return;
}


/* @funcstatic remap_NoCutList **********************************************
**
** display a list of the enzymes that do NOT cut
**
** @param [r] outfile [AjPFile] file to print to.
** @param [r] restrictlist [AjPList] List to print.
** @param [r] html [AjBool] dump out html if true.
** @param [r] enzymes [AjPStr] names of enzymes to search for or 'all'
** @param [r] blunt [AjBool] Allow blunt cutters
** @param [r] sticky [AjBool] Allow sticky cutters
** @param [r] sitelen [ajint] minimum length of recognition site

** @return [void]
** @@
******************************************************************************/

static void remap_NoCutList (AjPFile outfile, AjPList restrictlist,
			     AjBool html, AjPStr enzymes, AjBool blunt,
			     AjBool sticky, ajint sitelen)
{
    AjPFile enzfile=NULL;
    AjPStr  *ea;
    ajint ne;			/* number of enzymes */
    AjBool isall=ajTrue;
    ajint i;
    AjIList miter;		/* iterator for matches list */
    EmbPMatMatch m=NULL;	/* restriction enzyme match structure */
    EmbPPatRestrict enz;
    char *p;
    AjPStrTok tok;
    char tokens[] = " ,";
    AjPStr code = NULL;
    AjIList riter;		/* iterator for restrictlist */
    
    /* list of enzymes that cut */
    AjPList cutlist=ajListstrNew();
    AjIList citer;		/* iterator for cutlist */
    AjPStr cutname = NULL;
    
    /* list of enzymes that don't cut */
    AjPList nocutlist = ajListstrNew();
    AjIList niter;		/* iterator for nocutlist */
    AjPStr nocutname = NULL;
    
    
    /*** Read in a list of all input enzyme names */
    ajDebug("Read in a list of all input enzyme names\n");
    
    ne = 0;
    if (!enzymes)
	isall = ajTrue;
    else
    {
	/* get input list of enzymes into ea[] */
	ne = ajArrCommaList(enzymes,&ea);
	if (ajStrMatchCaseC(ea[0],"all"))
	    isall = ajTrue;
	else
	{
	    isall = ajFalse;

	    /* push explicitly input enzymes on nocutlist */
	    for (i=0;i<ne;++i)
	    {
		ajStrCleanWhite(&ea[i]);
		code = ajStrNew();
		ajStrAss(&code, ea[i]);
		ajListstrPushApp(nocutlist, code);
	    }
	}
    }

    /* tidy up ea[] */  
    for (i=0; i<ne; ++i) 
	if (ea[i]) 
	    ajStrDel(&ea[i]);

    if (ne)
	AJFREE (ea);
    
    /*
     *  if user entered 'all' then read in list of all enzymes, with
     *  restrictions
     */
    if (isall)
    {
	/* list all files in REBASE that don't cut */
	ajFileDataNewC(ENZDATA, &enzfile);

	/* push all enzyme names onto nocutlist */
	enz = embPatRestrictNew();
	/*    (void) ajFileSeek(enzfile,0L,0); */
	while(embPatRestrictReadEntry(&enz,&enzfile))
	{
	    if(!enz->ncuts)
		continue;
	    if(enz->len < sitelen)
		continue;
	    if(!blunt && enz->blunt)
		continue;
	    if(!sticky && !enz->blunt)
		continue;
	    p = ajStrStr(enz->pat);
	    if(*p < 'A' || *p > 'Z')
		continue;
	    code = ajStrNew();
	    ajStrAssS(&code, enz->cod);
	    ajListstrPushApp(nocutlist, code);
	}

	embPatRestrictDel(&enz);
	ajFileClose(&enzfile);
    }
    
    /*** Make a list of all enzymes that cut */
    ajDebug("Make a list of all enzymes that cut\n");
    
    miter = ajListIter(restrictlist);
    while ((m = ajListIterNext(miter)) != NULL)
    {
	cutname = ajStrNew();
	ajStrAss(&cutname, m->cod);
	ajListstrPushApp(cutlist, cutname);
    }
    ajListIterFree(miter);
    
    
    /*** Add to cutlist all isoschizomers of enzymes that cut */
    ajDebug("Add to cutlist all isoschizomers of enzymes that cut\n");
    
    /* make list of isoschizomers that cut */

    /*
     *  to stop problems with nocutlist still using this instance of
     *  'code' in its last element
     */
    code = ajStrNew();
    riter = ajListIter(restrictlist);
    while ((m = ajListIterNext(riter)) != NULL)
    {
	/* start token to parse isoschizomers names */
	tok = ajStrTokenInit(m->iso,  tokens);
	while (ajStrToken (&code, &tok, tokens))
	{
	    cutname = ajStrNew();
	    ajStrAss(&cutname, code);
	    ajListstrPushApp(cutlist, cutname);
	}
	ajStrTokenClear(&tok);
    }
    (void) ajListIterFree(riter);
    ajStrDel(&code);
    
    /*** Remove from the nocutlist all enzymes and isoschizomers that cut */
    ajDebug("Remove from the nocutlist all enzymes and isoschizomers "
	    "that cut\n");
    
    /*
     *  This steps down both lists at the same time, comparing names and
     *  iterating to the next name in whichever list whose name compares
     *  alphabetically before the other.  Where a match is found, the
     *  nocutlist item is deleted.
     */
    
    ajListSort(nocutlist, ajStrCmp);
    ajListSort(cutlist, ajStrCmp);
    
    citer = ajListIter(cutlist);
    niter = ajListIter(nocutlist);
    
    /* debug */
    /*
    while((cutname = (AjPStr)ajListIterNext(citer)) != NULL)
        ajDebug("dbg cutname = %S\n", cutname);
       */
    
    nocutname = (AjPStr)ajListIterNext(niter);
    cutname = (AjPStr)ajListIterNext(citer);
    
    ajDebug("initial cutname, nocutname: '%S' '%S'\n", cutname, nocutname);
    
    while (nocutname != NULL && cutname != NULL)
    {
	ajDebug("compare cutname, nocutname: %S %S\n", cutname, nocutname);
	i = ajStrCmpCase(cutname, nocutname);
	if (i == 0)
	{			/* match - so remove from nocutlist */
	    ajDebug("ajListstrRemove %S\n", nocutname);
	    ajListstrRemove(niter);
	    nocutname = (AjPStr)ajListIterNext(niter);
	    cutname = (AjPStr)ajListIterNext(citer);
	}
	else if (i == -1)
	    /* cutlist name sorts before nocutlist name */
	    cutname = (AjPStr)ajListIterNext(citer);
	else if (i == 1)
	    /* nocutlist name sorts before cutlist name */
	    nocutname = (AjPStr)ajListIterNext(niter);
    }

    ajListIterFree(citer);
    ajListIterFree(niter);
    ajListstrFree(&cutlist);
    
    
    
    /*** Print out the list */
    ajDebug("Print out the list\n");
    
    /* print the title */
    if (html)
	(void) ajFmtPrintF(outfile, "<H2>");  
    (void) ajFmtPrintF(outfile, "\n\n# Enzymes that do not cut\n\n");
    if (html)
	(void) ajFmtPrintF(outfile, "</H2>");  
    
    if (html)
	(void) ajFmtPrintF(outfile, "<PRE>");
    
    /*  ajListSort(nocutlist, ajStrCmp);*/
    niter = ajListIter(nocutlist);
    i = 0;
    while ((nocutname = (AjPStr)ajListIterNext(niter)) != NULL)
    {
	(void) ajFmtPrintF (outfile, "%-10S", nocutname);
	/* new line after every 7 names printed */
	if (i++ == 7)
	{
	    (void) ajFmtPrintF (outfile, "\n");
	    i = 0;
	}
    }
    ajListIterFree(niter);
    
    
    /* end the output */
    (void) ajFmtPrintF (outfile, "\n");
    if (html) {(void) ajFmtPrintF(outfile, "</PRE>");} 
    
    /*** Tidy up */
    ajDebug("Tidy up\n");
    ajListstrFree(&nocutlist);
    
    return;
}


/* @funcstatic remap_read_equiv **********************************************
**
** Lifted from Alan's restrict.c but reads the equ file.
**
** @param [r] equfile [AjPFile*] file to read then close.
** @param [wP] table [AjPTable*] table to write to.
** @return [void]
** @@
******************************************************************************/

static void remap_read_equiv(AjPFile *equfile, AjPTable *table)
{
    AjPStr line;
    AjPStr key;
    AjPStr value;

    char *p;

    line = ajStrNew();

    while(ajFileReadLine(*equfile,&line))
    {
        p=ajStrStr(line);
        if(!*p || *p=='#' || *p=='!')
            continue;
        p=strtok(p," \t\n");
        key=ajStrNewC(p);
        p=strtok(NULL," \t\n");
        value=ajStrNewC(p);
        ajTablePut(*table,(const void *)key, (void *)value);
    }
         
    ajFileClose(equfile);
}


/* @funcstatic remap_read_file_of_enzyme_names *******************************
**
** If the list of enzymes starts with a '@' if opens that file, reads in
** the list of enzyme names and replaces the input string with the enzyme names
**
** @param [r] enzymes [AjPStr*] names of enzymes to search for or 'all' or
**                              '@file'
** @return [void]
** @@
******************************************************************************/

static void remap_read_file_of_enzyme_names(AjPStr *enzymes)
{
    AjPFile file=NULL;
    AjPStr line;
    char   *p=NULL;

    if (ajStrFindC(*enzymes, "@") == 0)
    {
	ajStrTrimC(enzymes, "@");	/* remove the @ */
	file = ajFileNewIn(*enzymes);
	if (file == NULL)
	    ajDie("Cannot open the file of enzyme names: '%S'", enzymes);

	/* blank off the enzyme file name and replace with the enzyme names */
	ajStrClear(enzymes);
	line = ajStrNew();
	while(ajFileReadLine(file, &line))
	{
	    p = ajStrStr(line);
	    if (!*p || *p == '#' || *p == '!')
		continue;
	    ajStrApp(enzymes, line);
	    ajStrAppC(enzymes, ",");
	}
	ajStrDel(&line);
   
	ajFileClose(&file);  
    }

    return;
}
