/* @source seqretfeat application
**
** Read and write a sequence and feature table
**
** @author: Copyright (C) Peter Rice
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


static AjPList seqretfeat_buildListFromString(AjPStr string, AjPSeq seq,
					      AjBool feat);





/* @funcstatic seqretfeat_buildListFromString ********************************
**
** Undocumented.
**
** @param [?] string [AjPStr] Undocumented
** @param [?] seq [AjPSeq] Undocumented
** @param [?] feat [AjBool] Undocumented
** @return [AjPList] Undocumented
** @@
******************************************************************************/

static AjPList seqretfeat_buildListFromString(AjPStr string, AjPSeq seq,
					      AjBool feat)
{
    AjPRegexp commaseparated = NULL;
    AjPList newlist = NULL;
    AjPStr test=NULL;

    if(ajStrLen(string) <= 1)
	return NULL;

    commaseparated = ajRegCompC("([^, ]+)") ;

    while(ajRegExec(commaseparated,string))
    {
	ajRegSubI(commaseparated, 1, &test) ;
	/*    ajUser("test = *%S*\n",test);*/
	(void) ajRegPost(commaseparated,&string);    
	/*    ajUser("remainder = *%S*\n",string);*/

	if(feat)
	{
/*
	    item = CheckDictForFeature(seq->Fttable,test);

	    if(item)
	    {
		if(!newlist)
		    newlist = ajListNew();
		ajListPush(newlist,item);
	    }
	    else
		ajWarn("%S not a valid Feature hence not used\n",test);

	    ajStrDel(&test);
	}
	else
	{
	    item2 = CheckDictForTag(seq->Fttable,test);
      
	    if(item2)
	    {
		if(!newlist)
		    newlist = ajListNew();
		ajListPush(newlist,item2);
	    }
	    else
		ajWarn("%S not a valid Tag hence not used\n",test);
	    ajStrDel(&test);
*/
	}
    }

    ajRegFree(&commaseparated);
  
    return newlist;
}


/* @prog seqretfeat ***********************************************************
**
** Reads and writes (returns) a sequence and outputs the feature table for it
**
******************************************************************************/

int main(int argc, char **argv)
{

    AjPSeq seq;
    AjPSeqout seqout=NULL;
    AjPStr ignore=NULL,onlyallow=NULL;
    AjPStr ignore2=NULL,onlyallow2=NULL;
    AjPList newlist=NULL;
    AjBool sortbytype,sortbystart;

    embInit ("seqretfeat", argc, argv);

    ajFeatTest ();

    /*
       seq        = ajAcdGetSeq ("sequence");
       seqout     = ajAcdGetSeqout ("outseq");
       */
    ignore     = ajAcdGetString("featignore");
    onlyallow  = ajAcdGetString("featonlyallow");

    ignore2    = ajAcdGetString("tagignore");
    onlyallow2 = ajAcdGetString("tagonlyallow");

    sortbytype = ajAcdGetBool("sortbytype");
    sortbystart = ajAcdGetBool("sortbystart");

    ajExit();

    /* Process Features */
    newlist =  seqretfeat_buildListFromString(ignore, seq, ajTrue);
    if(newlist){
	ajDebug("Process featignore\n");
	/*
	   ajFeatIgnoreFeat(seq->Fttable,newlist);
	   */
	if(ajStrLen(onlyallow)> 1)
	    ajWarn("-featonlyallow option ignored as -featignore specified aswell");
    }
    else
    {
	newlist =  seqretfeat_buildListFromString(onlyallow, seq, ajTrue);
	/*
	   if(newlist)
	   ajFeatOnlyAllowFeat(seq->Fttable,newlist);
	   */
    }

    /* Process Tags */
    newlist =  seqretfeat_buildListFromString(ignore2, seq, ajFalse);
    if(newlist)
    {
	/*
	   ajFeatIgnoreTag(seq->Fttable,newlist);
	   */
	if(ajStrLen(onlyallow2)> 1)
	    ajWarn("-tagonlyallow option ignored as -tagignore specified aswell");
    }
    else
    {
	newlist =  seqretfeat_buildListFromString(onlyallow2, seq, ajFalse);
	/*
	   if(newlist)
	   ajFeatOnlyAllowTag(seq->Fttable,newlist);
	   */
    }

    if(sortbytype)
	ajFeatSortByType(seq->Fttable);
    else if(sortbystart)
	ajFeatSortByStart(seq->Fttable);

    ajSeqWrite (seqout, seq);
    ajSeqTrace (seq);
    ajSeqWriteClose (seqout);
    ajSeqDel (&seq);

    ajExit ();
    return 0;
}
