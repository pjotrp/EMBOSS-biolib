/* @source supermatcher application
**
** Local alignment of large sequences
**
** @author Copyright (C) Ian Longden
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

/* supermatcher
** Create a word table for the first sequence.
** Then go down second sequence checking to see if the word matches.
** If word matches then check to see if the position lines up with the last
** position if it does continue else stop.
** This gives us the start (offset) for the smith-waterman match by finding
** the biggest match and calculating start and ends for both sequences.
*/


/*
** possible speedup. The matching function is iterating through a
** list of hits just to find the one with the right offset. Could we
** use a table instead with the offset (as a string) as the key?
 */

#include "emboss.h"
#include <limits.h>
#include <math.h>




/* @datastatic concat *********************************************************
**
** supermatcher internals
**
** @alias concatS
**
** @attr offset [ajint] Undocumented
** @attr count [ajint] Undocumented
** @attr list [AjPList] Undocumented
** @attr total [ajint] Undocumented
** @attr Padding [char[4]] Padding to alignment boundary
******************************************************************************/

typedef struct concatS
{
    ajint offset;
    ajint count;
    AjPList list;
    ajint total;
    char  Padding[4];
} concat;




static void supermatcher_matchListOrder(void **x,void *cl);
static void supermatcher_orderandconcat(AjPList list,AjPList ordered);
static void supermatcher_removelists(void **x,void *cl);
static ajint supermatcher_findstartpoints(AjPTable seq1MatchTable,
					  const AjPSeq b, const AjPSeq a,
					  ajint *start1, ajint *start2,
					  ajint *end1, ajint *end2);
static void supermatcher_findmax(void **x,void *cl);




concat *conmax = NULL;
ajint maxgap   = 0;




/* @prog supermatcher *********************************************************
**
** Finds a match of a large sequence against one or more sequences
**
** Create a word table for the first sequence.
** Then go down second sequence checking to see if the word matches.
** If word matches then check to see if the position lines up with the last
** position if it does continue else stop.
** This gives us the start (offset) for the smith-waterman match by finding
** the biggest match and calculating start and ends for both sequences.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPSeqall queryseqs;
    AjPSeqset targetseqs;
    AjPSeq queryseq;
    const AjPSeq targetseq;
    AjPStr queryaln = 0;
    AjPStr targetaln = 0;

    AjPFile errorf;
    AjBool show = ajFalse;

    const char   *queryseqc;
    const char   *targetseqc;

    AjPMatrixf matrix;
    AjPSeqCvt cvt = 0;
    float **sub;
    ajint *compass = NULL;
    float *path = NULL;

    float gapopen;
    float gapextend;
    float score;
    float minscore;

    ajuint k;
    ajint targetbegin;
    ajint querystart = 0;
    ajint targetstart = 0;
    ajint queryend   = 0;
    ajint targetend   = 0;
    ajint width  = 0;
    AjPTable seq1MatchTable = 0;
    ajint wordlen = 6;
    ajint oldmax = 0;
    ajint newmax = 0;

    AjPAlign align = NULL;

    embInit("supermatcher", argc, argv);

    matrix    = ajAcdGetMatrixf("datafile");
    queryseqs      = ajAcdGetSeqall("asequence");
    targetseqs      = ajAcdGetSeqset("bsequence");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    wordlen   = ajAcdGetInt("wordlen");
    align     = ajAcdGetAlign("outfile");
    errorf    = ajAcdGetOutfile("errorfile");
    width     = ajAcdGetInt("width");	/* width for banded Smith-Waterman */
    minscore  = ajAcdGetFloat("minscore");

    gapopen   = ajRoundFloat(gapopen, 8);
    gapextend = ajRoundFloat(gapextend, 8);

    sub = ajMatrixfGetMatrix(matrix);
    cvt = ajMatrixfGetCvt(matrix);

    embWordLength(wordlen);

    ajSeqsetTrim(targetseqs);

    while(ajSeqallNext(queryseqs,&queryseq))
    {
        ajSeqTrim(queryseq);

	queryaln = ajStrNewRes(1+ajSeqGetLen(queryseq));

	ajDebug("Read '%S'\n", ajSeqGetNameS(queryseq));

	if(!embWordGetTable(&seq1MatchTable, queryseq)) /* get table of words */
	    ajErr("Could not generate table for %s\n",
		  ajSeqGetNameC(queryseq));

	for(k=0;k<ajSeqsetGetSize(targetseqs);k++)
	{
	    targetseq      = ajSeqsetGetseqSeq(targetseqs, k);
	    targetbegin = 1 + ajSeqGetOffset(targetseq);

	    ajDebug("Processing '%S'\n", ajSeqGetNameS(targetseq));

	    if(!supermatcher_findstartpoints(seq1MatchTable,targetseq,queryseq,
	                                     &querystart, &targetstart,
	                                     &queryend, &targetend))
	    {
		ajFmtPrintF(errorf,
			    "No wordmatch start points for "
			    "%s vs %s. No alignment\n",
			    ajSeqGetNameC(queryseq),ajSeqGetNameC(targetseq));
		continue;
	    }
	    
	    targetaln=ajStrNewRes(1+ajSeqGetLen(targetseq));
	    queryseqc = ajSeqGetSeqC(queryseq);
	    targetseqc = ajSeqGetSeqC(targetseq);

	    ajStrAssignC(&queryaln,"");
	    ajStrAssignC(&targetaln,"");

	    ajDebug("++ %S v %S start:%d %d end:%d %d\n",
			ajSeqGetNameS(targetseq), ajSeqGetNameS(queryseq),
			targetstart, querystart, targetend, queryend);

	    newmax = (targetend-targetstart+1)*width;
	    if(newmax > oldmax)
	    {
		AJCRESIZE0(path,oldmax,newmax);
		AJCRESIZE0(compass,oldmax,newmax);
		oldmax=newmax;
		ajDebug("++ memory re/allocation for path/compass arrays"
			" to size: %d\n", newmax);
	    }

	    ajDebug("Calling embAlignPathCalcSWFast "
		    "%d..%d [%d/%d] %d..%d [%d/%d] width:%d\n",
		    querystart, queryend, (queryend - querystart + 1),
		    ajSeqGetLen(queryseq),
		    targetstart, targetend, (targetend - targetstart + 1),
		    ajSeqGetLen(targetseq),
		    width);

		score = embAlignPathCalcSWFast(&targetseqc[targetstart],
		                               &queryseqc[querystart],
		                               targetend-targetstart+1,
		                               queryend-querystart+1,
		                               0,width,
		                               gapopen,gapextend,
		                               path,sub,cvt,
		                               compass,show);
		if(score>minscore)
		{
		    embAlignWalkSWMatrixFast(path,compass,gapopen,gapextend,
		                             targetseq,queryseq,
		                             &targetaln,&queryaln,
		                             targetend-targetstart+1,
		                             queryend-querystart+1,
		                             0,width,
		                             &targetstart,&querystart);

		if(!ajAlignFormatShowsSequences(align))
		{
		    ajAlignDefineCC(align, ajStrGetPtr(targetaln),
		                    ajStrGetPtr(queryaln),
		                    ajSeqGetNameC(targetseq),
		                    ajSeqGetNameC(queryseq));
		    ajAlignSetScoreR(align, score);
		}
		else
		{
		    embAlignReportLocal(align,
                        queryseq, targetseq,
                        queryaln,targetaln,
                        querystart,targetstart,
                        gapopen, gapextend,
                        score,matrix,
                        1 + ajSeqGetOffset(queryseq),
                        targetbegin);
		}
		ajAlignWrite(align);
		ajAlignReset(align);
	    }
	    ajStrDel(&targetaln);
	}

	embWordFreeTable(&seq1MatchTable); /* free table of words */
	seq1MatchTable=0;

	ajStrDel(&queryaln);

    }

    if(!ajAlignFormatShowsSequences(align))
    {
        ajMatrixfDel(&matrix);        
    }
    
    AJFREE(path);
    AJFREE(compass);

    ajAlignClose(align);
    ajAlignDel(&align);
    ajSeqallDel(&queryseqs);
    ajSeqDel(&queryseq);
    ajSeqsetDel(&targetseqs);
    ajFileClose(&errorf);

    embExit();

    return 0;
}




/* @funcstatic supermatcher_matchListOrder ************************************
**
** Calculates the offset for the current match.
**
** Steps through the ordered output list to find one item with the same offset.
** Adds to it if found, otherwise creates a new item at the end.
**
** @param [r] x [void**] Word match item
** @param [r] cl [void*] Ordered output lists
** @return [void]
** @@
******************************************************************************/

static void supermatcher_matchListOrder(void **x,void *cl)
{
    EmbPWordMatch p;
    AjPList ordered;
    ajint offset;
    AjIList listIter;
    concat *con;
    concat *c=NULL;

    p = (EmbPWordMatch)*x;
    ordered = (AjPList) cl;

    offset = (*p).seq1start-(*p).seq2start;

    /* iterate through ordered list to find if it exists already*/
    listIter = ajListIterNewread(ordered);

    while(!ajListIterDone( listIter))
    {
	con = ajListIterGet(listIter);
	if(con->offset == offset)
	{
	    /* found so add count and set offset to the new value */
	    con->offset = offset;
	    con->total+= (*p).length;
	    con->count++;
	    ajListPushAppend(con->list,p);
	    ajListIterDel(&listIter);
	    return;
	}
    }
    ajListIterDel(&listIter);

    /* not found so add it */
    AJNEW(c);
    c->offset = offset;
    c->total  = (*p).length;
    c->count  = 1;
    c->list   = ajListNew();
    ajListPushAppend(c->list,p);
    ajListPushAppend(ordered, c);

    return;
}




/* @funcstatic supermatcher_orderandconcat ************************************
**
** Undocumented.
**
** @param [u] list [AjPList] unordered input list - elements added to the
**                           ordered list, but apparently not deleted.
** @param [w] ordered [AjPList] ordered output list
** @return [void]
** @@
******************************************************************************/

static void supermatcher_orderandconcat(AjPList list,AjPList ordered)
{
    ajListMap(list,supermatcher_matchListOrder, ordered);

    return;
}




/* @funcstatic supermatcher_removelists ***************************************
**
** Undocumented.
**
** @param [r] x [void**] Undocumented
** @param [r] cl [void*] Undocumented
** @return [void]
** @@
******************************************************************************/

static void supermatcher_removelists(void **x,void *cl)
{
    concat *p;

    (void) cl;				/* make it used */

    p = (concat *)*x;

    ajListFree(&(p)->list);
    AJFREE(p);

    return;
}




/* @funcstatic supermatcher_findmax *******************************************
**
** Undocumented.
**
** @param [r] x [void**] Undocumented
** @param [r] cl [void*] Undocumented
** @return [void]
** @@
******************************************************************************/

static void supermatcher_findmax(void **x,void *cl)
{
    concat *p;
    ajint *max;

    p   = (concat *)*x;
    max = (ajint *) cl;

    if(p->total > *max)
    {
	*max = p->total;
	conmax = p;
    }

    return;
}




/* @funcstatic supermatcher_findstartpoints ***********************************
**
** Undocumented.
**
** @param [w] seq1MatchTable [AjPTable] match table
** @param [r] b [const AjPSeq] second sequence
** @param [r] a [const AjPSeq] first sequence
** @param [w] start1 [ajint*] start in sequence 1
** @param [w] start2 [ajint*] start in sequence 2
** @param [w] end1 [ajint*] end in sequence 1
** @param [w] end2 [ajint*] end in sequence 2
** @return [ajint] Undocumented
** @@
******************************************************************************/

static ajint supermatcher_findstartpoints(AjPTable seq1MatchTable,
					  const AjPSeq b,
					  const AjPSeq a, ajint *start1,
					  ajint *start2, ajint *end1,
					  ajint *end2)
{
    ajint max = -10;
    ajint offset = 0;
    AjPList matchlist = NULL;
    AjPList ordered = NULL;
    ajint amax;
    ajint bmax;
    ajint bega;
    ajint begb;

    amax = ajSeqGetLen(a)-1;
    bmax = ajSeqGetLen(b)-1;
    bega = ajSeqGetOffset(a);
    begb = ajSeqGetOffset(b);


    ajDebug("supermatcher_findstartpoints len %d %d off %d %d\n",
	     amax, bmax, bega, begb);
    matchlist = embWordBuildMatchTable(seq1MatchTable, b, ajTrue);

    if(!matchlist)
	return 0;
    else if(!matchlist->Count)
    {
        embWordMatchListDelete(&matchlist);
	return 0;
    }


    /* order and add if the gap is gapmax or less */

    /* create list header bit*/
    ordered = ajListNew();

    supermatcher_orderandconcat(matchlist, ordered);

    /* this sets global structure conmax to point to a matchlist element */
    ajListMap(ordered,supermatcher_findmax, &max);

    ajDebug("findstart conmax off:%d count:%d total:%d\n",
	    conmax->offset, conmax->count, conmax->total,
	    ajListGetLength(conmax->list));
    offset = conmax->offset;

    /* the offset is all we needed! we can delete everything */

    ajListMap(ordered,supermatcher_removelists, NULL);
    ajListFree(&ordered);
    embWordMatchListDelete(&matchlist);	/* free the match structures */


    if(offset > 0)
    {
	*start1 = offset;
	*start2 = 0;
    }
    else
    {
	*start2 = 0-offset;
	*start1 = 0;
    }
    *end1 = *start1;
    *end2 = *start2;

    ajDebug("++ end1 %d -> %d end2 %d -> %d\n", *end1, amax, *end2, bmax);
    while(*end1<amax && *end2<bmax)
    {
	(*end1)++;
	(*end2)++;
    }

    ajDebug("++ end1 %d end2 %d\n", *end1, *end2);
    
    
    ajDebug("supermatcher_findstartpoints has %d..%d [%d] %d..%d [%d]\n",
	    *start1, *end1, ajSeqGetLen(a), *start2, *end2, ajSeqGetLen(b));

    return 1;
}
