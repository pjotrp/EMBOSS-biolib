/* @source showfeat application
**
** Show features of a sequence
**
** @author: Copyright (C) Gary Williams (gwilliam@hgmp.mrc.ac.uk)
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


static void ShowFeatSeq (AjPFile outfile, AjPSeq seq, ajint beg, ajint end, AjPStr
	matchsource, AjPStr matchtype, AjPStr matchtag, AjPStr matchvalue,
	AjPStr *sortlist, ajint width, AjBool collapse, AjBool forward, AjBool
	reverse, AjBool unknown, AjBool strand, AjBool source, AjBool
	position, AjBool type, AjBool tags, AjBool values);
    	
static void WriteFeat(AjPStr line, char strand, ajint fstart, ajint fend,
	ajint width, ajint beg, ajint end);

static void FeatOut(AjPFile outfile, AjPStr lineout, char strandout,
	AjPStr sourceout, AjPStr posout, AjPStr typeout, AjPStr tagsout,
	ajint width, AjBool strand, AjBool source, AjBool type, AjBool
	tags, AjBool position);

static ajint CompareFeatSource (const void * a, const void * b);

static ajint CompareFeatType (const void * a, const void * b);

static ajint CompareFeatPos (const void * a, const void * b);

static AjBool showfeatMatchPatternTags (AjPFeature feat, AjPStr tpattern,
					AjPStr vpattern);

static void AddPos(AjPStr *posout, ajint start, ajint end);

static void AddTagsStr(AjPStr *tagout, AjPFeature feat, AjBool values);





int main(int argc, char **argv)
{

  AjPSeqall seqall;
  AjPFile outfile;
  AjPSeq seq;
  AjPStr matchsource;
  AjPStr matchtype;
  AjPStr matchtag;
  AjPStr matchvalue;
  AjPStr * sortlist;
  AjBool html;
  AjBool id;
  AjBool description;
  AjBool scale;
  ajint width;
  AjBool collapse;
  AjBool forward;
  AjBool reverse;
  AjBool unknown;
  AjBool strand;
  AjBool source;
  AjBool position;
  AjBool type;
  AjBool tags;
  AjBool values;

  
  ajint i;  
  ajint beg, end;
  AjPStr descriptionline;

  (void) embInit ("showfeat", argc, argv);

  seqall = ajAcdGetSeqall ("sequence");
  outfile = ajAcdGetOutfile ("outfile");
  matchsource = ajAcdGetString ("matchsource");
  matchtype = ajAcdGetString ("matchtype");
  matchtag = ajAcdGetString ("matchtag");
  matchvalue = ajAcdGetString ("matchvalue");
  sortlist = ajAcdGetList ("sort");
  html = ajAcdGetBool ("html");
  id = ajAcdGetBool ("id");
  description = ajAcdGetBool ("description");
  scale = ajAcdGetBool ("scale");
  width = ajAcdGetInt ("width");    
  collapse = ajAcdGetBool ("collapse");
  forward = ajAcdGetBool ("forward");
  reverse = ajAcdGetBool ("reverse");
  unknown = ajAcdGetBool ("unknown");
  strand = ajAcdGetBool ("strand");
  source = ajAcdGetBool ("source");
  position = ajAcdGetBool ("position");
  type = ajAcdGetBool ("type");
  tags = ajAcdGetBool ("tags");
  values = ajAcdGetBool ("values");
 
  while (ajSeqallNext(seqall, &seq)) {


/* get begin and end positions */
  beg = ajSeqBegin(seq)-1;  
  end = ajSeqEnd(seq)-1;

/* do the ID name and description */
    if (id) {
      if (html) {
        (void) ajFmtPrintF(outfile, "<H2>%S</H2>\n", ajSeqGetName(seq));
      } else {
        (void) ajFmtPrintF(outfile, "%S\n", ajSeqGetName(seq));
      }
    }
    if (description) {
      if (html) {
        (void) ajFmtPrintF(outfile, "<H3>%S</H3>\n", ajSeqGetDesc(seq));
      } else {
        descriptionline = ajStrNew();
        (void) ajStrAss(&descriptionline, ajSeqGetDesc(seq));
        (void) ajStrWrap(&descriptionline, 80);
        (void) ajFmtPrintF(outfile, "%S\n", descriptionline);
        (void) ajStrDel(&descriptionline);
      }
    }

/* the simplest way of formatting for HTML is just to PRE it all :-) */
   if (html) { (void) ajFmtPrintF(outfile, "<PRE>");}

/* show the scale */
    if (scale && width > 3) {
      (void) ajFmtPrintF(outfile, "|");
      for (i=0; i<width-2; i++) {
      	(void) ajFmtPrintF(outfile, "=");
      }
      (void) ajFmtPrintF(outfile, "| ");
      if (beg != 0 || end+1 != ajSeqLen(seq)) {
        (void) ajFmtPrintF(outfile, "%d-%d of %d\n", beg+1, end+1, ajSeqLen(seq));
      } else {
        (void) ajFmtPrintF(outfile, "%d\n", ajSeqLen(seq));
      }
    }


/* show the features */
    (void) ShowFeatSeq (outfile, seq, beg, end, matchsource, matchtype,
	matchtag, matchvalue, sortlist, width, collapse, forward, reverse,
	unknown, strand, source, position, type, tags, values);

/* end the HTML PRE block */
    if (html) { (void) ajFmtPrintF(outfile, "</PRE>");}

/* gratuitous blank line after the sequence details */
    (void) ajFmtPrintF(outfile, "\n");

  }

  ajFileClose(&outfile);

  (void) ajExit ();
  return 0;
}

/* @funcstatic ShowFeatSeq ********************************************
**
** Show the sequence features using clunky ascii graphics
**
** @param [u] outfile [AjPFile] output file
** @param [r] seq [AjPSeq] sequence
** @param [r] beg [ajint] sequence start position
** @param [r] end [ajint] sequence end position
** @param [r] matchsource [AjPStr] source pattern to display
** @param [r] matchtype [AjPStr] type pattern to display
** @param [r] matchtag [AjPStr] tag pattern to display
** @param [r] matchvalue [AjPStr] tag's value pattern to display
** @param [r] sortlist [AjPStr *] type of sorting of features to do
** @param [r] width [ajint] width of line of features
** @param [r] collapse [AjBool] show all features on separate lines
** @param [r] forward [AjBool] show forward sense features
** @param [r] reverse [AjBool] show reverse sense features
** @param [r] unknown [AjBool] show unknown sense features
** @param [r] strand [AjBool] show strand of feature
** @param [r] source [AjBool] show source of feature
** @param [r] position [AjBool] show position of feature
** @param [r] type [AjBool] show type of feature
** @param [r] tags [AjBool] show tags and values of feature
** @param [r] values [AjBool] show tag values of feature
** @return [void] 
** @@
******************************************************************************/


static void ShowFeatSeq (AjPFile outfile, AjPSeq seq, ajint beg, ajint end, AjPStr
	matchsource, AjPStr matchtype, AjPStr matchtag, AjPStr matchvalue,
	AjPStr *sortlist, ajint width, AjBool collapse, AjBool forward, AjBool
	reverse, AjBool unknown, AjBool strand, AjBool source, AjBool
	position, AjBool type, AjBool tags, AjBool values) {
    	
  AjIList    iter = NULL ;
  AjPFeature gf   = NULL ;
  AjPFeattable feat;

  /*  ajint len = ajSeqLen(seq);*/

  AjPStr lineout   = ajStrNew();
  char strandout = '\0';
  AjBool first = ajTrue;
  AjPStr sourceout = NULL;
  AjPStr typeout   = NULL;
  AjPStr tagsout   = NULL;
  AjPStr posout    = NULL;

  AjBool gotoutput = ajFalse;	/* have a line to output */
      
/* get the feature table of the sequence */
  feat = ajSeqGetFeat(seq);
  if(!feat)
    return;

  lineout = ajStrNew();
  tagsout = ajStrNewC("");
  posout  = ajStrNewC("");
  
  if (feat->Features) {

    if (!ajStrCmpC(sortlist[0], "source")) {
/* sort by: sense, source, type, start */
      ajListSort(feat->Features, CompareFeatSource);
    } else if (!ajStrCmpC(sortlist[0], "start")) {
/* sort by: sense, source, type, start */
      ajListSort(feat->Features, CompareFeatPos);
    } else { /* type */
/* sort by: sense, type, source, start */
      ajListSort(feat->Features, CompareFeatType);
    }
  
    iter = ajListIter(feat->Features) ;

    while(ajListIterMore(iter)) {
      gf = ajListIterNext (iter) ;

/* check that we want to output this sense */
      if (!forward && gf->Strand == '+') continue;
      if (!reverse && gf->Strand == '-') continue;
      if (!unknown && gf->Strand == '\0') continue;

/* check that we want to output this match of source, type */
      if (!embMiscMatchPattern (gf->Source, matchsource) || 
          !embMiscMatchPattern (gf->Type, matchtype) ||
          !showfeatMatchPatternTags(gf, matchtag, matchvalue)) continue;

/* check that the feature is within the range we wish to display */
      if (beg+1 > gf->End || end+1 < gf->Start) continue;

/* see if we are starting a new line */
      if (!collapse || first ||
          gf->Strand != strandout ||
          (source && ajStrCmpCase(gf->Source, sourceout)) ||
          ajStrCmpCase(gf->Type, typeout)) {
      	if (gotoutput) {
      	  FeatOut(outfile, lineout, strandout, sourceout, posout, typeout, tagsout, width, strand, source, type, tags, position);
      	}
/* reset the strings for the new line */
        ajStrClear(&lineout);
      	(void) ajStrAppKI(&lineout, ' ', width);
      	ajStrAss(&sourceout, gf->Source);
      	ajStrAss(&typeout, gf->Type);
        strandout = gf->Strand;
        ajStrClear(&tagsout);
        ajStrClear(&posout);

/* note that we have something to output */
        gotoutput = ajTrue;
      }
/* add tags to tagout */
      AddTagsStr(&tagsout, gf, values);
/* add positions to posout */
      AddPos(&posout, gf->Start, gf->End);

/* write the feature on the line */
      WriteFeat(lineout, strandout, gf->Start, gf->End, width, beg, end);
      
      first = ajFalse;
    }

/* print out any last line */
    if (gotoutput) {
      FeatOut(outfile, lineout, strandout, sourceout, posout, typeout, tagsout, width, strand, source, type, tags, position);
    }

    ajListIterFree(iter) ;

  }


/* tidy up */
  (void) ajFeattabDel(&feat);

  ajStrDel(&tagsout);
  ajStrDel(&posout);
  ajStrDel(&lineout);
  
  return;
}

  
/* @funcstatic WriteFeat********************************************
**
** Show the sequence features using clunky ascii graphics
**
** @param [w] line [AjPStr] Line of ASCII graphics to write in
** @param [r] strand [char] strand
** @param [r] fstart [ajint] start of feature
** @param [r] fend [ajint] end of feature
** @param [r] width [ajint] width of line of features
** @param [r] beg [ajint] sequence start position
** @param [r] end [ajint] sequence end position
** @return [void] 
** @@
******************************************************************************/

static void WriteFeat(AjPStr line, char strand, ajint fstart, ajint fend,
	ajint width, ajint beg, ajint end) {

  ajint i;
  ajint len = end-beg+1;
  ajint pos1 = ((float)(fstart-beg)/(float)len)*width-1;
  ajint pos2 = ((float)(fend-beg)/(float)len)*width-1;

/* write the '-'s */
  for (i=pos1; i<pos2; i++) {
    if (i >= 0 && i < width) {
/* don't overwrite any characters except space */
      if (*(ajStrStr(line)+i) == ' ') {
        *(ajStrStr(line)+i) = '-';
      }
    }
  }
  
/* write the end characters */
  if (pos1 >= 0 && pos1 < width) {
    if (strand == '+') {
      *(ajStrStr(line)+pos1) = '|';
    
    } else if (strand == '-') {
      *(ajStrStr(line)+pos1) = '<';
    
    } else {
      *(ajStrStr(line)+pos1) = '|';
    
    }
  }
  
  if (pos2 >= 0 && pos2 < width) {
    if (strand == '+') {
      *(ajStrStr(line)+pos2) = '>';
    
    } else if (strand == '-') {
      *(ajStrStr(line)+pos2) = '|';
    
    } else {
      *(ajStrStr(line)+pos2) = '|';
    
    }
  } 
}

/* @funcstatic FeatOut********************************************
**
** Show the sequence features  and source, type, etc.
** We guarantee to not have trailing whitespace at the end of a line.
**
** @param [u] outfile [AjPFile] output file
** @param [r] lineout [AjPStr] ASCII graphics line
** @param [r] strandout [char] strand of feature 
** @param [r] sourceout [AjPStr] source of feature 
** @param [r] posout [AjPStr] positions of feature 
** @param [r] typeout [AjPStr] type of feature 
** @param [r] tagsout [AjPStr] tags string
** @param [r] width [ajint] width of graphics lines
** @param [r] strand [AjBool] show strand of feature
** @param [r] source [AjBool] show source of feature
** @param [r] type [AjBool] show type of feature
** @param [r] tags [AjBool] show tags and values of feature
** @param [r] position [AjBool] show positions of feature
** @return [void] 
** @@
******************************************************************************/

static void FeatOut(AjPFile outfile, AjPStr lineout, char strandout,
	AjPStr sourceout, AjPStr posout, AjPStr typeout, AjPStr tagsout,
	ajint width, AjBool strand, AjBool source, AjBool type, AjBool
	tags, AjBool position) {


/* don't display the graphics lines if they have a width less than 4 */
  if (width > 3) {
    (void) ajFmtPrintF(outfile, "%S", lineout);
    if (strand || source || type || tags || position) {
      (void) ajFmtPrintF(outfile, " ");
    }
  }

  if (strand) {
    if (strandout == '+') {
      (void) ajFmtPrintF(outfile, "+");
    } else if (strandout == '-') {
      (void) ajFmtPrintF(outfile, "-");
    } else {
      (void) ajFmtPrintF(outfile, "0");
    }
    if (source || type || tags || position) {
      (void) ajFmtPrintF(outfile, " ");
    }
  }

  if (source) {
    (void) ajFmtPrintF(outfile, "%S", sourceout);
    if (type || tags || position) {
      (void) ajFmtPrintF(outfile, " ");
    }
  }

  if (position) {
    (void) ajFmtPrintF(outfile, "%S", posout);
    if (type || tags) {
      (void) ajFmtPrintF(outfile, " ");
    }
  }

  if (type) {
    (void) ajFmtPrintF(outfile, "%S", typeout);
    if (tags) {
      (void) ajFmtPrintF(outfile, " ");
    }
  }


  if (tags) { 
    (void) ajFmtPrintF(outfile, "%S", tagsout);
  }


  (void) ajFmtPrintF(outfile, "\n");

}

/** @funcstatic CompareFeatSource ***********************************************
**
** Compare two feature node to sort as: sense, source, type, start
**
** @param [r] a [const void *] First node
** @param [r] b [const void *] Second node
**
** @return [ajint] Compare value (-1, 0, +1)
** @@
******************************************************************************/

static ajint CompareFeatSource (const void * a, const void * b) {

  ajint val;

  AjPFeature c = *(AjPFeature *)a;
  AjPFeature d = *(AjPFeature *)b;
  

/* sort by strand */	
  if (c->Strand == d->Strand) {

/* stands are the same, sort by source */
    val = ajStrCmpCase(c->Source, d->Source);
    if (val != 0) return val;
  
 
/* source is the same, sort by type */
    val = ajStrCmpCase(c->Type, d->Type);
    if (val != 0) return val;
  
 
/* type is the same, sort by start */
    return (c->Start - d->Start);

/* sort by strand */	
  } else if (c->Strand == '+') {
    return -1;

  } else if (c->Strand == '\0' && d->Strand == '-') {
    return -1;

  } else {
    return 1;
  }

}

/** @funcstatic CompareFeatType ***********************************************
**
** Compare two feature node to sort as: sense, type, source, start
**
** @param [r] a [const void *] First node
** @param [r] b [const void *] Second node
**
** @return [ajint] Compare value (-1, 0, +1)
** @@
******************************************************************************/

static ajint CompareFeatType (const void * a, const void * b) {

  ajint val;

  AjPFeature c = *(AjPFeature *)a;
  AjPFeature d = *(AjPFeature *)b;
  

/* sort by strand */	
  if (c->Strand == d->Strand) {

/* stands are the same, sort by type */
    val = ajStrCmpCase(c->Type, d->Type);
    if (val != 0) return val;
  
/* type is the same, sort by source */
    val = ajStrCmpCase(c->Source, d->Source);
    if (val != 0) return val;
  
/* source is the same, sort by start */
    return (c->Start - d->Start);

/* sort by strand */	
  } else if (c->Strand == '+') {
    return -1;

  } else if (c->Strand == '\0' && d->Strand == '-') {
    return -1;

  } else {
    return 1;
  }

}


/** @funcstatic CompareFeatPos ***********************************************
**
** Compare two feature node to sort as: sense, start, type, source
**
** @param [r] a [const void *] First node
** @param [r] b [const void *] Second node
**
** @return [ajint] Compare value (-1, 0, +1)
** @@
******************************************************************************/

static ajint CompareFeatPos (const void * a, const void * b) {

  ajint val;

  AjPFeature c = *(AjPFeature *)a;
  AjPFeature d = *(AjPFeature *)b;
  

/* sort by strand */	
  if (c->Strand == d->Strand) {

/* strands are the same, sort by start */
    val = c->Start - d->Start;
    if (val != 0) return val;

/* starts are the same, sort by type */
    val = ajStrCmpCase(c->Type, d->Type);
    if (val != 0) return val;
  
/* type is the same, sort by source */
    val = ajStrCmpCase(c->Source, d->Source);
    return val;
  
/* sort by strand */	
  } else if (c->Strand == '+') {
    return -1;

  } else if (c->Strand == '\0' && d->Strand == '-') {
    return -1;

  } else {
    return 1;
  }

}

/** @funcstatic showfeatMatchPatternTags **************************************
**
** Checks for a match of the tagpattern and valuepattern to at least one
** tag=value pair
**
** @param [r] feat [AjPFeature] Feature to process
** @param [r] tpattern [AjPStr] tags pattern to match with
** @param [r] vpattern [AjPStr] values pattern to match with
**
** @return [AjBool] ajTrue = found a match
** @@
******************************************************************************/

static AjBool showfeatMatchPatternTags (AjPFeature feat, AjPStr tpattern,
				AjPStr vpattern) {
    
  AjIList titer;                /* iterator for feat */
  AjPStr tagnam;		/* tag structure */
  AjPStr tagval;		/* tag structure */
  AjBool val = ajFalse;		/* returned value */
  AjBool tval;			/* tags result */
  AjBool vval;			/* value result */


/* if there are no tags to match, but the patterns are both '*', then
allow this as a match */
  if (!ajStrCmpC(tpattern, "*") &&
      !ajStrCmpC(vpattern, "*")) 
      return ajTrue;

/* iterate through the tags and test for match to patterns */
  titer = ajFeatTagIter(feat);
  while (ajFeatTagval(titer, &tagnam, &tagval)) {
    tval = embMiscMatchPattern(tagnam, tpattern);
    if(!ajStrLen(tagval)) /* if tag has no value */ 
      return val;
    vval = embMiscMatchPattern(tagval, vpattern);
    if (tval && vval) {
      val = ajTrue;
      break;
    }
  }
  (void) ajListIterFree(titer);

  return val;

}
/** @funcstatic AddPos ***********************************************
**
** writes the positions to the positions string
**
** @param [r] posout [AjPStr *] position string
** @param [r] start [ajint] start position
** @param [r] end [ajint] end position
**
** @return [void] 
** @@
******************************************************************************/

static void AddPos(AjPStr *posout, ajint start, ajint end) {

/* if the string already has positions in, separate them with commas */
  if (ajStrLen(*posout) > 0) {
    ajFmtPrintAppS(posout, ",");
  }
  ajFmtPrintAppS(posout, "%d-%d", start, end);

}
/** @funcstatic AddTagsStr ***********************************************
**
** writes the tags to the tagsout string
**
** @param [r] tagsout [AjPStr *] tags out string
** @param [r] feat [AjPFeature] Feature to use
** @param [r] values [AjBool] display values of tags
**
** @return [void] 
** @@
******************************************************************************/

static void AddTagsStr(AjPStr *tagsout, AjPFeature feat, AjBool values) {

  AjIList titer;                /* iterator for taglist */
  AjPStr tagnam = NULL;
  AjPStr tagval = NULL;

/* iterate through the tags and test for match to patterns */
/* debug - there is something wrong with the list */

  titer = ajFeatTagIter(feat);
  while (ajFeatTagval(titer, &tagnam, &tagval)) {
/* don't display the translation tag - it is far too long :-) */
    if (ajStrCmpC(tagnam, "translation")) {
      if (values == ajTrue) {
        (void) ajFmtPrintAppS(tagsout, " %S=\"%S\"", tagnam, tagval);
      } else {
        (void) ajFmtPrintAppS(tagsout, " %S", tagnam);
      }
    }
  }
  (void) ajListIterFree(titer);
}

