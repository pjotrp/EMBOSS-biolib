/* @source sixpack application
**
** Display a DNA sequence in both direction with its translation
**
** @author: Copyright (C) Thomas Laurent (thomas.laurent@uk.lionbioscience.com)
** 30 Sept 2002
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


/* declare functions */

static int sixpack_ajStrCmpCase (const void* str1, const void* str2);

/* structure for counts and isoschizomers of a restriction enzyme hit */
typedef struct SValue
{
    ajint  count;
    AjPStr iso;
} OValue, *PValue;



/* @prog sixpack ****************************************************************
**
** Display a DNA sequence in both direction with its translation
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
    AjBool numberseq;
    AjBool nameseq;
    ajint width;
    ajint length;
    ajint margin;
    AjBool description;
    ajint offset;
    AjBool html;
    AjPStr descriptionline;
    ajint orfminsize;
    AjPTrn trnTable;
    AjBool translation;
    AjBool reverse;



    (void) embInit ("sixpack", argc, argv);

    seqall = ajAcdGetSeqall ("sequence");
    outfile = ajAcdGetOutfile ("outfile");
    tablelist = ajAcdGetList ("table");
    uppercase = ajAcdGetRange ("uppercase");
    highlight = ajAcdGetRange ("highlight");
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


    /* get the number of the genetic code used */
    (void) ajStrToInt(tablelist[0], &table);
    trnTable = ajTrnNewI(table);

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


	/* make the Show Object */
	ss = embShowNew(seq, begin, end, width, length, margin, html, offset);

	if (html)
	    (void) ajFmtPrintF(outfile, "<PRE>");

	/* create the format to display */

	if (translation)
	{
	  if (reverse) 
	    (void) embShowAddBlank(ss);
	  (void) embShowAddBlank(ss);
	  

	    (void) embShowAddTran (ss, trnTable, 1, FALSE, numberseq,
				   NULL, orfminsize);
	    (void) embShowAddTran (ss, trnTable, 2, FALSE, numberseq,
				   NULL, orfminsize);
	    (void) embShowAddTran (ss, trnTable, 3, FALSE, numberseq,
				   NULL, orfminsize);
	}

	/*	(void) embShowAddBlank(ss);*/
	(void) embShowAddSeq(ss, numberseq, FALSE, uppercase, highlight);

	if (!numberseq)
	    (void) embShowAddTicknum(ss);
	(void) embShowAddTicks(ss);

	if (reverse)
	{
	    (void) embShowAddComp(ss, numberseq);
	}

	if (translation)
	{
	    if (reverse)
	    {
		(void) embShowAddTran (ss, trnTable, -3, FALSE,
				       numberseq, NULL, orfminsize);
		(void) embShowAddTran (ss, trnTable, -2, FALSE,
				       numberseq, NULL, orfminsize);
		(void) embShowAddTran (ss, trnTable, -1, FALSE,
				       numberseq, NULL, orfminsize);
	    }
	}

	(void) embShowPrint(outfile, ss);


	/* add a gratuitous newline at the end of the sequence */
	(void) ajFmtPrintF(outfile, "\n");

	/* tidy up */
	(void) embShowDel(&ss);

    }

    /* tidy up */
    ajTrnDel (&trnTable);

    ajExit ();
    return 0;
}


/* @funcstatic sixpack_ajStrCmpCase *********************************************
**
** Compares the value of two strings for use in sorting (e.g. ajListSort)
** Case Independent!
**
** @param [r] str1 [const void*] First string
** @param [r] str2 [const void*] Second string
** @return [int] -1 if first string should sort before second, +1 if the
**         second string should sort first. 0 if they are identical
**         in length and content.
** @@
******************************************************************************/

static int sixpack_ajStrCmpCase (const void* str1, const void* str2) {
  const char* cp;
  const char* cq;

  for (cp = (*(AjPStr*)str1)->Ptr, cq = (*(AjPStr*)str2)->Ptr;
       *cp && *cq;
       cp++, cq++) {
    if (toupper((ajint) *cp) != toupper((ajint) *cq)) {
      if (toupper((ajint) *cp) > toupper((ajint) *cq)) return 1;
      else return -1;
    }
  }

  if (*cp) return 1;
  if (*cq) return -1;
  return 0;

}
