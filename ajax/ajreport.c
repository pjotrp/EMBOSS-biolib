/********************************************************************
** @source AJAX REPORT (ajax feature reporting) functions
**
** These functions report AJAX sequence feature data in a variety
** of formats.
**
** @author Copyright (C) 2000 Peter Rice, LION Bioscience Ltd.
** @version 1.0 
** @modified Nov 10 First version
** @@
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
********************************************************************/

#include <stddef.h>
#include <stdarg.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#include "ajax.h"

typedef struct ReportSFormat {
  char *Name;
  ajint Mintags;
  AjBool Showseq;
  AjBool Nuc;
  AjBool Prot;
  void (*Write) (AjPReport outrpt, AjPFeattable ftable, AjPSeq seq);
} ReportOFormat, *ReportPFormat;

static void reportWriteEmbl (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWriteGenbank (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWriteGff (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWritePir (AjPReport outrpt, AjPFeattable ftable,
			    AjPSeq seq);
static void reportWriteSwiss (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);

static void reportWriteTrace (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);

static void reportWriteListFile (AjPReport outrpt, AjPFeattable ftable,
				 AjPSeq seq);

static void reportWriteDbMotif (AjPReport outrpt, AjPFeattable ftable,
				AjPSeq seq);
static void reportWriteDiffseq (AjPReport outrpt, AjPFeattable ftable,
				AjPSeq seq);
static void reportWriteExcel (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWriteFeatTable (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWriteMotif (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWriteRegions (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWriteSeqTable (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);
static void reportWriteTable (AjPReport outrpt, AjPFeattable ftable,
			      AjPSeq seq);

static char* reportCharname (AjPReport thys);

/* @funclist reportFormat *****************************************************
**
** Functions to write feature reports
**
******************************************************************************/

static ReportOFormat reportFormat[] = { 
  /* standard feature formats */
  {"embl",      0, AJFALSE, AJTRUE,  AJFALSE, reportWriteEmbl},
  {"genbank",   0, AJFALSE, AJTRUE,  AJFALSE, reportWriteGenbank},
  {"gff",       0, AJFALSE, AJTRUE,  AJTRUE,  reportWriteGff},
  {"pir",       0, AJFALSE, AJFALSE, AJTRUE,  reportWritePir},
  {"swiss",     0, AJFALSE, AJFALSE, AJTRUE,  reportWriteSwiss},
  /* trace  for debug */
  {"trace",     0, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteTrace},
  /* list file for input to other programs */
  {"listfile",  0, AJFALSE, AJTRUE,  AJTRUE,  reportWriteListFile},
  /* feature reports */
  {"dbmotif",   0, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteDbMotif},
  {"diffseq",   0, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteDiffseq},
  {"excel",     0, AJFALSE, AJTRUE,  AJTRUE,  reportWriteExcel},
  {"feattable", 0, AJFALSE, AJTRUE,  AJTRUE,  reportWriteFeatTable},
  {"motif",     0, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteMotif},
  {"regions",   0, AJFALSE, AJTRUE,  AJTRUE,  reportWriteRegions},
  {"seqtable",  0, AJTRUE,  AJTRUE,  AJTRUE,  reportWriteSeqTable},
  {"table",     0, AJFALSE, AJTRUE,  AJTRUE,  reportWriteTable},
  {NULL, 0, 0, 0, 0, NULL}
};

/* @funcstatic reportWriteTrace ***********************************************
**
** Writes a report in Trace format
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteTrace (AjPReport thys,
			      AjPFeattable ftable, AjPSeq seq) {

  ajReportWriteHeader (thys, ftable, seq);

  ajFmtPrintF (thys->File, "Trace output\n");

  ajReportWriteTail (thys, ftable, seq);

  return;
}

/* @funcstatic reportWriteEmbl ************************************************
**
** Writes a report in EMBL format
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteEmbl (AjPReport thys,
			     AjPFeattable ftable, AjPSeq seq) {

  static AjPStr ftfmt = NULL;

  if (!ftfmt)
    ajStrAssC (&ftfmt, "embl");

  ajFmtPrintF (thys->File, "#EMBL output\n");

  thys->Ftquery = ajFeattabOutNewSSF (ftfmt, thys->Name,
				      ajStrStr(thys->Type),
				      thys->File);
  if (!ajFeatWrite (thys->Ftquery, ftable)) {
    ajWarn ("ajReportWriteEmbl features output failed format: '%S'",
	    ftfmt);
  }
  return;
}

/* @funcstatic reportWriteGenbank *********************************************
**
** Writes a report in Genbank format
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteGenbank (AjPReport thys,
				AjPFeattable ftable, AjPSeq seq) {

  static AjPStr ftfmt = NULL;

  if (!ftfmt)
    ajStrAssC (&ftfmt, "genbank");

  ajFmtPrintF (thys->File, "#Genbank output\n");

  thys->Ftquery = ajFeattabOutNewSSF (ftfmt, thys->Name,
				      ajStrStr(thys->Type),
				      thys->File);
  if (!ajFeatWrite (thys->Ftquery, ftable)) {
    ajWarn ("ajReportWriteGenbank features output failed format: '%S'",
	    ftfmt);
  }
  return;
}
/* @funcstatic reportWriteGff *************************************************
**
** Writes a report in GFF format
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteGff (AjPReport thys,
			    AjPFeattable ftable, AjPSeq seq) {

  static AjPStr ftfmt = NULL;

  if (!ftfmt)
    ajStrAssC (&ftfmt, "gff");

  thys->Ftquery = ajFeattabOutNewSSF (ftfmt, thys->Name,
				      ajStrStr(thys->Type),
				      thys->File);
  if (!ajFeatWrite (thys->Ftquery, ftable)) {
    ajWarn ("ajReportWriteGff features output failed format: '%S'",
	    ftfmt);
  }
  return;
}

/* @funcstatic reportWritePir *************************************************
**
** Writes a report in PIR format
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWritePir (AjPReport thys,
			    AjPFeattable ftable, AjPSeq seq) {

  static AjPStr ftfmt = NULL;

  if (!ftfmt)
    ajStrAssC (&ftfmt, "pir");

  thys->Ftquery = ajFeattabOutNewSSF (ftfmt, thys->Name,
				      ajStrStr(thys->Type),
				      thys->File);
  if (!ajFeatWrite (thys->Ftquery, ftable)) {
    ajWarn ("ajReportWritePir features output failed format: '%S'",
	    ftfmt);
  }
  return;
}

/* @funcstatic reportWriteSwiss ***********************************************
**
** Writes a report in SwissProt format
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteSwiss (AjPReport thys,
			      AjPFeattable ftable, AjPSeq seq) {

  static AjPStr ftfmt = NULL;

  if (!ftfmt)
    ajStrAssC (&ftfmt, "swissprot");

  thys->Ftquery = ajFeattabOutNewSSF (ftfmt, thys->Name,
				      ajStrStr(thys->Type),
				      thys->File);
  if (!ajFeatWrite (thys->Ftquery, ftable)) {
    ajWarn ("ajReportWriteSwiss features output failed format: '%S'",
	    ftfmt);
  }
  return;
}

/* @funcstatic reportWriteDbMotif *******************************************
**
** Writes a report in DbMotif format
**
** Format:<br>
** Length = [length] <br>
** Start = position [start] of sequence <br>
** End = position [start] of sequence <br>
** ... other tags ... <br>
** [sequence] <br>
** [start and end numbered below sequence] <br>
**
** Data reported: Length, Start, End, Sequence (5 bases around feature)
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: <br>
**   all tags reported as name = value
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteDbMotif (AjPReport thys,
				AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  ajint ilen=0;
  AjPStr subseq = NULL;
  AjPStr tmpstr = NULL;

  ajint ntags;
  static AjPStr* tagtypes;
  static AjPStr* tagnames;
  static AjPStr* tagprints;
  ajint j=0;
  AjPStr tagval = NULL;
  ajint jstart;
  ajint jend;

  ajReportWriteHeader (thys, ftable, seq);

  ntags = ajReportLists (thys, &tagtypes, &tagnames, &tagprints);

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    ilen = iend - istart + 1;

    jstart = AJMAX (0, istart-6);
    jend = AJMIN(ajSeqLen(seq), iend+4);

    ajStrAssCL (&tmpstr, "", ilen+10);
    for (j=istart+1; j<iend; j++)
      ajStrAppK (&tmpstr, ' ');

    ajStrAssSub(&subseq, ajSeqStr(seq), jstart, jend);
    ajFmtPrintF (outf, "Length = %d\n", ilen);
    ajFmtPrintF (outf, "Start = position %d of sequence\n", istart);
    ajFmtPrintF (outf, "End = position %d of sequence\n\n", iend);
    for (j=0; j < ntags; j++) {
      if (ajFeatGetNote(feature, tagnames[j], &tagval)) {
	ajFmtPrintF (outf, "%S = %S\n", tagprints[j], tagval);
      }
    }
    ajFmtPrintF (outf, "\n");

    ajFmtPrintF (outf, "%S\n", subseq);
    if (istart == iend) {
      ajFmtPrintF (outf, "%*s|\n", (istart-jstart-1), " ");
      ajFmtPrintF (outf, "%*d\n\n", AJMIN(6, istart), istart);
    }
    else {
    ajFmtPrintF (outf, "%*s|%S|\n", (istart-jstart-1), " ", tmpstr);
      ajFmtPrintF (outf, "%*d%S%d\n\n",
		   AJMIN(6, istart),
		   istart, tmpstr, iend);
    }
    ajStrDelReuse(&tmpstr);
  }

  ajStrDel(&subseq);
  ajStrDel(&tagval);

  ajListIterFree(iterft);

  ajReportWriteTail (thys, ftable, seq);

  return;
}

/* @funcstatic reportWriteDiffseq *******************************************
**
** Writes a report in Diffseq format, based on the output from the
** diffseq application. The report describes matches, usually short,
** between two sequences and features which overlap them.
**
** A number of tags are used. The rpeort makes little sense without them.
** These tags are used to replicate features in a second sequence.
**
** Format:<br>
**   [Name] [start]-[end] Length: [length] <br>
**   Feature: [special first_feature tag] <br>
**   Sequence: [sequence] <br>
**   Sequence: [special sequence tag] <br>
**   Feature: [special second_feature tag] <br>
**   [as first line, using special tags name, start, end, length] <br>
**
** Data reported: Name, Start, End, Length, Sequence
**
** Tags required: None
**
** Tags used: <br>
**   start : start position in second sequence <br>
**   end : end in second sequence <br>
**   length : length of match in second sequence <br>
**   name : name of second sequence (set by ajReportSeqName) <br>
**   sequence : sequence of match in second sequence <br>
**   first_feature : feature(s) in first sequence <br>
**   second_feature : feature(s) in second sequence <br>
**
** Tags reported: None
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteDiffseq (AjPReport thys,
				AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0, jstart=0;
  ajint iend=0, jend=0;
  ajint ilen=0, jlen=0;
  AjPStr subseq = NULL;
  ajint i=0;
  ajint ifeat=0;
  AjPStr tmpstr = NULL;
  AjPStr jname=NULL;
  AjPStr firstfeat=NULL;
  AjPStr secondfeat=NULL;
  AjPStr tagval = NULL;

  static AjPStr tagfirst=NULL, tagsecond=NULL,
    tagstart=NULL, tagend=NULL, taglength=NULL,
    tagname=NULL, tagsequence=NULL;

  if (!tagstart) {
    tagstart = ajStrNewC("start");
    tagend = ajStrNewC("end");
    taglength = ajStrNewC("length");
    tagname = ajStrNewC("name");
    tagsequence = ajStrNewC("sequence");
    tagfirst = ajStrNewC("first_feature");
    tagsecond = ajStrNewC("second_feature");
  }

  ajReportWriteHeader (thys, ftable, seq);

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    ilen = iend - istart + 1;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);
    i++;

    if (!ajFeatGetNote(feature, tagstart, &tagval)) {
      jstart = 0;
    }
    else {
      ajStrToInt(tagval, &jstart);
    }

    if (!ajFeatGetNote(feature, tagend, &tagval)) {
      jend = 0;
    }
    else {
      ajStrToInt(tagval, &jend);
    }

    if (!ajFeatGetNote(feature, taglength, &tagval)) {
      jlen = 0;
    }
    else {
      ajStrToInt(tagval, &jlen);
    }

    if (!ajFeatGetNote(feature, tagname, &jname)) {
      ajStrAssC (&jname, "");
    }

    if (ilen > 0) {
      ajFmtPrintF (outf, "\n%S %d-%d Length: %d\n",
		   ajReportSeqName(thys, seq), istart, iend, ilen);
      ifeat = 1;
      while (ajFeatGetNoteI(feature, tagfirst, ifeat++, &firstfeat)) {
	ajFmtPrintF (outf, "Feature: %S\n", firstfeat);
      }
      ajFmtPrintF (outf, "Sequence: %S\n", subseq);
    }
    else {
      ajFmtPrintF (outf, "\n%S %d Length: %d\n",
		   ajReportSeqName(thys, seq), istart, ilen);
      ifeat = 1;
      while (ajFeatGetNoteI(feature, tagfirst, ifeat++, &firstfeat)) {
	ajFmtPrintF (outf, "Feature: %S\n", firstfeat);
      }
      ajFmtPrintF (outf, "Sequence: \n");
    }

    if (!ajFeatGetNote(feature, tagsequence, &subseq)) {
      ajStrAssC (&subseq, "");
    }

    if (jlen > 0) {
      ajFmtPrintF (outf, "Sequence: %S\n", subseq);
      ifeat = 1;
      while (ajFeatGetNoteI(feature, tagsecond, ifeat++, &secondfeat)) {
	ajFmtPrintF (outf, "Feature: %S\n", secondfeat);
      }
      ajFmtPrintF (outf, "%S %d-%d Length: %d\n",
		   jname, jstart, jend, jlen);
    }
    else {
      ajFmtPrintF (outf, "Sequence: \n");
      ifeat = 1;
      while (ajFeatGetNoteI(feature, tagsecond, ifeat++, &secondfeat)) {
	ajFmtPrintF (outf, "Feature: %S\n", secondfeat);
      }
      ajFmtPrintF (outf, "%S %d Length: %d\n",
		   jname, jstart, jlen);
    }

    ajStrDelReuse(&tmpstr);
  }

  ajReportWriteTail (thys, ftable, seq);

  ajStrDel(&subseq);
  ajStrDel(&tmpstr);
  ajStrDel(&tagval);

  ajListIterFree(iterft);
  return;
}

/* @funcstatic reportWriteExcel *******************************************
**
** Writes a report in Excel (tab delimited) format. Name, start, end
** and score are always reported. Other tags in the report definition
** are added as extra columns.
**
** All values are (for now) unquoted. Missing values are reported as '.'
**
** Format:<br>
**   "SeqName Start End Score" (tab delimited) <br>
**   [extra tag names added to first line] <br>
**   Name Start End Score [extra tag values] (tab delimited) <br>
**
** Data reported: Name Start End Score
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All defined tags
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteExcel (AjPReport thys,
			      AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  float score=0.0;
  AjPStr subseq = NULL;
  ajint i=0;
  AjPStr tmpstr = NULL;

  ajint ntags;
  static AjPStr* tagtypes;
  static AjPStr* tagnames;
  static AjPStr* tagprints;
  ajint j=0;
  AjPStr tagval = NULL;

  ntags = ajReportLists (thys, &tagtypes, &tagnames, &tagprints);

  ajFmtPrintF (outf, "SeqName\tStart\tEnd\tScore");	/* then extra tags */
  for (j=0; j < ntags; j++) {
    ajFmtPrintF (outf, "\t%S", tagprints[j]);
  }
  ajFmtPrintF (outf, "\n");

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    score = feature->Score;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);
    i++;
    ajFmtPrintF (outf, "%S\t%d\t%d\t%f",
		 ajReportSeqName(thys, seq),
		 istart, iend, score);  /* then extra tags */
    for (j=0; j < ntags; j++) {
      if (ajFeatGetNote(feature, tagnames[j], &tagval)) {
	ajFmtPrintF (outf, "\t%S", tagval);
      }
      else {
	ajFmtPrintF (outf, "\t."); /* missing value '.' for now */
      }
    }
    ajFmtPrintF (outf, "\n");
    ajStrDelReuse(&tmpstr);
  }

  ajStrDel(&subseq);
  ajStrDel(&tmpstr);
  ajStrDel(&tagval);

  ajListIterFree(iterft);
  return;
}

/* @funcstatic reportWriteFeatTable *******************************************
**
** Writes a report in FeatTable format. The report is an EMBL feature
** table using only the tags in the report definition. There is no
** requirement for tag names to match standards for the EMBL feature
** table.
**
** The original EMBOSS application for this format was cpgreport.
**
** Format:<br>
**   FT [type] [start]..[end] <br>
**                            /[tagname]=[tagvalue] <br>
**
** Data reported: Type, Start, End
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteFeatTable (AjPReport thys,
				  AjPFeattable ftable, AjPSeq seq) {
  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  AjPStr subseq = NULL;

  ajint ntags;
  static AjPStr* tagtypes;
  static AjPStr* tagnames;
  static AjPStr* tagprints;
  ajint j=0;
  AjPStr tagval = NULL;

  ntags = ajReportLists (thys, &tagtypes, &tagnames, &tagprints);

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);
    if (feature->Strand == '-') {
      ajFmtPrintF (outf, "FT   %-15.15S complement(%d..%d)\n",
		   ajFeatGetType(feature),
		   iend, istart);
    }
    else {
      ajFmtPrintF (outf, "FT   %-15.15S %d..%d\n",
		   ajFeatGetType(feature),
		   istart, iend);
    }
    for (j=0; j < ntags; j++) {
      if (ajFeatGetNote(feature, tagnames[j], &tagval)) {
	ajFmtPrintF (outf, "FT                   /%S=\"%S\"\n",
		     tagprints[j], tagval);
       }
      else {			/* skip the missing ones for now */
	/*ajFmtPrintF (outf, "FT                   /%S=\"<unknown>\"\n",
	  tagprints[j]);*/
      }
    }
    ajFmtPrintF (outf, "\n");
  }

  ajStrDel(&subseq);
  ajStrDel(&tagval);

  ajListIterFree(iterft);
  return;
}

/* @funcstatic reportWriteListFile *******************************************
**
** Writes a report in ListFile format for use as input to another application.
**
** Format:<br>
**   Name[start:end] <br>
**
** Data reported: Name, Start, End, Strand
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: None
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteListFile (AjPReport thys,
			      AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  AjPStr subseq = NULL;
  ajint i=0;
  AjPStr tmpstr = NULL;

  ajReportWriteHeader (thys, ftable, seq);

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);
    i++;

    ajFmtPrintS (&tmpstr, "[");

    if (istart)
      ajFmtPrintAppS (&tmpstr, "%d", istart);

    ajFmtPrintAppS (&tmpstr, ":");

    if (iend)
      ajFmtPrintAppS (&tmpstr, "%d", iend);

    if (feature->Strand == '-')
      ajFmtPrintAppS (&tmpstr, ":r");
    ajFmtPrintAppS (&tmpstr, "]");

    if (ajStrLen(tmpstr) > 3)
      ajFmtPrintF (outf, "%S%S\n",
		 ajReportSeqName(thys, seq), tmpstr);
    else
      ajFmtPrintF (outf, "%S\n",
		   ajReportSeqName(thys, seq));

    ajStrDelReuse(&tmpstr);
  }

  ajReportWriteTail (thys, ftable, seq);

  ajListIterFree(iterft);
  ajStrDel(&subseq);
  ajStrDel(&tmpstr);

  return;
}

/* @funcstatic reportWriteMotif *******************************************
**
** Writes a report in Motif format.  Based on the original output
** format of antigenic, helixturnhelix and sigcleave.
**
** Format:<br>
**   (1) Score [score] length [length] at [name] [start->[end] <br>
**               *  (marked at position pos) <br>
**             [sequence] <br>
**             |        | <br>
**       [start]        [end] <br>
**   [tagname]: tagvalue
**
** Data reported: Name, Start, End, Length, Score, Sequence
**
** Tags required: None
**
** Tags used: <br>
**   pos (integer, maximum score position)
**
** Tags reported: All
**   
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteMotif (AjPReport thys,
			      AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  float score=0.0;
  ajint ilen=0;
  AjPStr subseq = NULL;
  ajint i=0;
  AjPStr tmpstr = NULL;

  ajint ntags;
  static AjPStr* tagtypes;
  static AjPStr* tagnames;
  static AjPStr* tagprints;
  ajint j=0;
  AjPStr tagval = NULL;

  ajint jpos = 0;
  ajint jmax = -1;

  ajReportWriteHeader (thys, ftable, seq);

  ntags = ajReportLists (thys, &tagtypes, &tagnames, &tagprints);

  for (j=0; j < ntags; j++) {
    if (ajStrMatchCaseC (tagnames[j], "pos")) {
      jmax = j;
      ajFmtPrintF (outf, "%S at \"*\"\n\n", tagprints[jmax]);
      break;
    }
  }

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    score = feature->Score;
    ilen = iend - istart + 1;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);
    i++;
    ajFmtPrintF (outf, "(%d) Score %.3f length %d at %s %d->%d\n",
		 i, score, ilen, reportCharname(thys), istart, iend);
    if (jmax >= 0) {
      if (ajFeatGetNote(feature, tagnames[jmax], &tagval)) {
	ajStrToInt(tagval, &jpos);
      }
      else {
	jpos = iend+1;
      }
      
      ajStrAssCL (&tmpstr, "",ilen);
      for (j=istart; j<jpos; j++)
	ajStrAppK (&tmpstr, ' ');

      ajFmtPrintF (outf, "           %S*\n", tmpstr);
    }

    ajFmtPrintF (outf, " Sequence: %S\n", subseq);
    ajStrAssCL (&tmpstr, "",ilen);
    for (j=istart+1; j<iend; j++)
      ajStrAppK (&tmpstr, ' ');
    if (istart == iend) {
      ajFmtPrintF (outf, "           |\n");
      ajFmtPrintF (outf, "%12d\n", istart);
     }
    else {
      ajFmtPrintF (outf, "           |%S|\n", tmpstr);
      ajFmtPrintF (outf, "%12d%S%d\n", istart, tmpstr, iend);
    }
    if (ntags) {
      for (j=0; j < ntags; j++) {
	if (j == jmax) continue;
	if (ajFeatGetNote(feature, tagnames[j], &tagval)) {
	  ajFmtPrintF (outf, " %S: %S\n", tagprints[j], tagval);
	}
      }
    }

    ajFmtPrintF (outf, "\n");
    ajStrDelReuse(&tmpstr);
  }

  ajReportWriteTail (thys, ftable, seq);

  ajStrDel(&subseq);
  ajStrDel(&tmpstr);
  ajStrDel(&tagval);

  ajListIterFree(iterft);
  return;
}

/* @funcstatic reportWriteRegions *******************************************
**
** Writes a report in Regions format. The report (unusually for the current
** report formats) includes the feature type.
**
** Format: <br>
**   [type] from [start] to [end] ([length] [name]) <br>
**   ([tagname]: [tagvalue], [tagname]: [tagvalue]  ...) <br>
**
** Data reported: Type, Start, End, Length, Name
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteRegions (AjPReport thys,
				AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  float score=0.0;
  ajint ilen=0;
  AjPStr subseq = NULL;
  AjPStr tagstr = NULL;

  ajint ntags;
  static AjPStr* tagtypes;
  static AjPStr* tagnames;
  static AjPStr* tagprints;
  ajint j=0;
  AjPStr tagval = NULL;

  ajReportWriteHeader (thys, ftable, seq);

  ntags = ajReportLists (thys, &tagtypes, &tagnames, &tagprints);

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    score = feature->Score;
    ilen = iend - istart + 1;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);
    ajFmtPrintF (outf, "%S from %d to %d (%d %s)\n",
		 ajFeatGetType(feature), istart, iend, ilen,
		 reportCharname(thys));
    ajFmtPrintF (outf, "   Max score: %.3f", score);
    if (ntags) {
      ajFmtPrintF(outf, " (");
      for (j=0; j < ntags; j++) {
	if (ajFeatGetNote(feature, tagnames[j], &tagval)) {
	  if (j) {
	    ajFmtPrintF (outf, ", ");
	  }
	  ajFmtPrintF (outf, "%S: %S", tagprints[j], tagval);
	}
      }

      ajFmtPrintF(outf, ")");

    }
    ajFmtPrintF (outf, "\n\n");
  }

  ajReportWriteTail (thys, ftable, seq);

  ajStrDel(&subseq);
  ajStrDel(&tagstr);
  ajStrDel(&tagval);

  ajListIterFree(iterft);
  return;
}

/* @funcstatic reportWriteSeqTable *******************************************
**
** Writes a report in SeqTable format Thie is a simple table format that
** includes the feature sequence. See reportWriteTable for a version
** without the sequence. Missing tag values are reported as '.'
** The column width is 6, or longer if the name is longer.
**
** Format:<br>
**   Start   End   [tagnames]  Sequence
**   [start] [end] [tagvalues] [sequence]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported: All
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteSeqTable (AjPReport thys,
				 AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  AjPStr subseq = NULL;

  ajint ntags;
  static AjPStr* tagtypes;
  static AjPStr* tagnames;
  static AjPStr* tagprints;
  ajint j=0;
  AjPStr tagval = NULL;
  ajint jwid=6;
  ajint jmin=6;			/* minimum width for printing special tags */

  ajReportWriteHeader (thys, ftable, seq);

  ntags = ajReportLists (thys, &tagtypes, &tagnames, &tagprints);

  ajFmtPrintF (outf, "%7s %7s", "Start", "End");
  for (j=0; j < ntags; j++) {
    jwid = AJMAX(jmin, ajStrLen(tagprints[j]));
    ajFmtPrintF (outf, " %-*S", jwid, tagprints[j]);
  }
  ajFmtPrintF (outf, " Sequence\n");

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);
    ajFmtPrintF (outf, "%7d %7d", istart, iend);
    for (j=0; j < ntags; j++) {
      jwid = AJMAX(jmin, ajStrLen(tagprints[j]));
      if (ajFeatGetNote(feature, tagnames[j], &tagval)) {
	ajFmtPrintF (outf, " %-*S", jwid, tagval);
      }
      else {
	ajFmtPrintF (outf, " %-*s", jwid, "."); /* missing value '.' for now */
      }
    }
    ajFmtPrintF (outf, " %S\n", subseq);
  }

  ajReportWriteTail (thys, ftable, seq);

  ajStrDel(&subseq);
  ajStrDel(&tagval);

  ajListIterFree(iterft);
  return;
}

/* @funcstatic reportWriteTable *******************************************
**
** Writes a report in Table format. See reportWriteSeqTable for a version
** with the sequence. Missing tag values are reported as '.'
** The column width is 6, or longer if the name is longer.
**
** Format:<br>
**   USA    Start   End   Score   [tagnames]
**   [name] [start] [end] [score] [tagvalues]
**
** Data reported:
**
** Tags required: None
**
** Tags used: None
**
** Tags reported:
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void reportWriteTable (AjPReport thys,
			      AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjIList iterft = NULL;
  AjPFeature feature = NULL;
  ajint istart=0;
  ajint iend=0;
  float score=0.0;
  AjPStr subseq = NULL;
  ajint ntags;
  static AjPStr* tagtypes;
  static AjPStr* tagnames;
  static AjPStr* tagprints;
  ajint j=0;
  AjPStr tagval = NULL;
  ajint jwid=6;
  ajint jmin=6;			/* minimum width for printing special tags */

  ajReportWriteHeader (thys, ftable, seq);

  ntags = ajReportLists (thys, &tagtypes, &tagnames, &tagprints);

  ajFmtPrintF (outf, "%-15s %7s %7s %7s", "USA", "Start", "End", "Score");
  for (j=0; j < ntags; j++) {
    jwid = AJMAX(jmin, ajStrLen(tagprints[j]));
    ajFmtPrintF (outf, " %-*S", jwid, tagprints[j]);
  }
  ajFmtPrintF (outf, "\n");

  iterft = ajListIter(ftable->Features) ;
  while(ajListIterMore(iterft)) {
    feature = (AjPFeature)ajListIterNext (iterft) ;
    istart = feature->Start;
    iend = feature->End;
    ajStrAssSub(&subseq, ajSeqStr(seq), istart-1, iend-1);

    ajFmtPrintF (outf, "%-15.15S %7d %7d %7.3f",
		 ajReportSeqName(thys, seq),
		 istart, iend, score);
    for (j=0; j < ntags; j++) {
      jwid = AJMAX(jmin, ajStrLen(tagprints[j]));
      if (ajFeatGetNote(feature, tagnames[j], &tagval)) {
	  ajFmtPrintF (outf, " %-*S", jwid, tagval);
      }
      else {
	  ajFmtPrintF (outf, " %-*s", jwid, "."); /* missing value '.' for now */
      }
    }
    ajFmtPrintF (outf, "\n");
  }

  ajReportWriteTail (thys, ftable, seq);

  ajStrDel(&subseq);
  ajStrDel(&tagval);

  ajListIterFree(iterft);
  return;
}

/* @func ajReportDel **********************************************************
**
** Destructor for report objects
**
** @param [D] pthys [AjPReport*] Report object reference
** @return [void]
** @@
******************************************************************************/

void ajReportDel (AjPReport* pthys) {

  AjPReport thys = *pthys;

  ajStrDel (&thys->Name);
  ajStrDel (&thys->Formatstr);
  ajStrDel (&thys->Filename);
  ajStrDel (&thys->Extension);

  AJFREE(*pthys);

  return;
}

/* @func ajReportOpen *********************************************************
**
** Opens a new report file
**
** @param [R] thys [AjPReport] Report object
** @param [R] name [AjPStr] File name
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajReportOpen (AjPReport thys, AjPStr name) {

  thys->File = ajFileNewOut(name);
  if (thys->File)
    return ajTrue;

  return ajFalse;
}

/* @func ajReportFormatDefault ************************************************
**
** Sets the default format for a feature report
**
** @param [W] pformat [AjPStr*] Default format returned
** @return [AjBool] ajTrue is format was returned
** @@
******************************************************************************/

AjBool ajReportFormatDefault (AjPStr* pformat) {

  if (ajStrLen(*pformat)) {
    ajDebug ("... output format '%S'\n", *pformat);
  }
  else {
    /* ajStrSetC (pformat, reportFormat[0].Name);*/
    (void) ajStrSetC (pformat, "gff"); /* use the real name */
    ajDebug ("... output format not set, default to '%S'\n", *pformat);
  }

  return ajTrue;
}

/* @func ajReportFindFormat ***********************************************
**
** Looks for the specified report format in the internal definitions and
** returns the index.
**
** @param [P] format [AjPStr] Format required.
** @param [w] iformat [ajint*] Index
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajReportFindFormat (AjPStr format, ajint* iformat) {

  AjPStr tmpformat = NULL;
  ajint i = 0;

  if (!ajStrLen(format))
    return ajFalse;

  (void) ajStrAss (&tmpformat, format);
  (void) ajStrToLower(&tmpformat);

  while (reportFormat[i].Name) {
    if (ajStrMatchCaseC(tmpformat, reportFormat[i].Name)) {
      *iformat = i;
      ajStrDel(&tmpformat);
      return ajTrue;
    }
    i++;
  }

  ajStrDel(&tmpformat);
  return ajFalse;
}

/* @func ajReportSetTags ************************************************
**
** Constructor for a report object
**
** @param [R] thys [AjPReport] Report object
** @param [R] taglist [AjPStr] Tag names list
** @param [R] mintags [ajint] Minimum number of tags to use in report
**                            (used to check there are enough tags listed)
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajReportSetTags (AjPReport thys, AjPStr taglist, ajint mintags) {

  static AjPRegexp tagexp = NULL;
  static AjPStr tmplist = NULL;
  static AjPStr tmpstr = NULL;
  AjPStr tagtype = NULL;
  AjPStr tagname = NULL;
  AjPStr tagprint = NULL;

  /* assume the tags are a simple list in this format:
  ** type: name[=printname]
  **
  ** spaces are not allowd in names (for ease of parsing the results)
  */

  if (!tagexp)
    tagexp = ajRegCompC(" *([^:]+):([^= ]+)(=([^ ]+))?");

  ajStrAssS(&tmplist, taglist);
  while (ajRegExec(tagexp, tmplist)) {
    tagtype = NULL;
    tagname = NULL;
    tagprint = NULL;
    ajRegSubI (tagexp, 1, &tagtype);
    ajRegSubI (tagexp, 2, &tagname);
    ajRegSubI (tagexp, 4, &tagprint);
    if (!ajStrLen(tagprint))
      ajStrAssS(&tagprint, tagname);

    ajDebug ("Tag '%S' : '%S' print '%S'\n", tagtype, tagname, tagprint);
    ajRegPost(tagexp, &tmpstr);
    ajStrAssS(&tmplist, tmpstr);

    if (!ajListLength(thys->Tagtypes)) {
      thys->Tagtypes  = ajListNew();
      thys->Tagnames  = ajListNew();
      thys->Tagprints = ajListNew();
    }

    ajListPushApp (thys->Tagtypes,  tagtype);
    ajListPushApp (thys->Tagnames,  tagname);
    ajListPushApp (thys->Tagprints, tagprint);
  }

  return ajTrue;
}

/* @func ajReportValid ************************************************
**
** Test for a report object.
**
** Checks the format works with the number of tags.
** Checks the format works with the type (protein or nucleotide).
**
** @param [R] thys [AjPReport] Report object
** @param [R] mintags [ajint] Minimum number of tags to use in report
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajReportValid (AjPReport thys) {

  if (!thys->Format) {
    if (!ajReportFindFormat(thys->Formatstr, &thys->Format)) {
      ajWarn("Unknown report format '%S'", thys->Formatstr);
      return ajFalse;
    }
  }

  if (thys->Mintags > ajListLength(thys->Tagnames)) {
      ajWarn("Report specifies %d tags, has only %d",
	     thys->Mintags, ajListLength(thys->Tagnames));
      return ajFalse;
  }

  if ( reportFormat[thys->Format].Mintags > ajListLength(thys->Tagnames)) {
      ajWarn("Report format '%S' needs %d tags, has only %d",
	     reportFormat[thys->Format].Mintags,
	     ajListLength(thys->Tagnames));
      return ajFalse;
  }

  return ajTrue;
}

/* @func ajReportNew ************************************************
**
** Constructor for a report object
**
** @return [AjPReport] New report object
** @@
******************************************************************************/

AjPReport ajReportNew (void) {

  AjPReport pthis;

  AJNEW0(pthis);

  pthis->Count = 0;
  pthis->Name = ajStrNew();
  pthis->Formatstr = ajStrNew();
  pthis->Format = 0;
  pthis->Fttable = NULL;
  pthis->Ftquery = ajFeattabOutNew();
  pthis->Filename = ajStrNew();
  pthis->Extension = ajStrNew();
  pthis->File = NULL;

  return pthis;
}

/* @func ajReportWrite ************************************************
**
** Writes a feature report
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajReportWrite (AjPReport thys, AjPFeattable ftable, AjPSeq seq) {

  ajDebug ("ajReportWrite\n");	/* add ftable name and size */

  if (!thys->Format) {
    if (!ajReportFindFormat(thys->Formatstr, &thys->Format)) {
      ajErr ("unknown report format '%S'", thys->Formatstr);
    }
  }

  ajDebug ("ajReportWrite %d '%s'\n",
	   thys->Format, reportFormat[thys->Format].Name);

  ajReportSetType (thys, ftable, seq);

  /* EFUNC NOTE: next line Write calls funclist reportFormat() */

  reportFormat[thys->Format].Write (thys, ftable, seq);

  return;
}

/* @func ajReportClose ********************************************************
**
** Closes a feature report
**
** @param [R] thys [AjPReport] Report object
** @return [void]
** @@
******************************************************************************/

void ajReportClose (AjPReport thys) {

  ajDebug ("ajReportClose '%F'\n", thys->File);

  ajFileClose (&thys->File);

  return;
}

/* @func ajReportLists ********************************************************
**
** Converts a report tagtypes definition (ACD taglist attribute)
** into arrays of tag types, names and printnames.
**
** @param [r] thys [AjPReport] Report object
** @param [w] types [AjPStr**] Address of array of types generated
** @param [w] names [AjPStr**] Address of array of names generated
** @param [w] prints [AjPStr**] Address of array of print names generated
** @return [ajint] Number of tagtypes (size of arrays created)
******************************************************************************/

ajint ajReportLists (AjPReport thys, AjPStr** types, AjPStr** names,
		     AjPStr** prints) {

  ajint ntags;

  if (!ajListLength(thys->Tagtypes))
      return 0;

  ntags = ajListToArray (thys->Tagnames,  (void***) names);
  ntags = ajListToArray (thys->Tagprints, (void***) prints);
  ntags = ajListToArray (thys->Tagtypes,  (void***) types);

  return ntags;
}

/* @func ajReportWriteHeader ************************************************
**
** Writes a feature report header
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajReportWriteHeader (AjPReport thys, AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjPStr tmpstr=NULL;

  /* Header for the top of the file (first call for report only) */

  if (!thys->Count) {
    ajFmtPrintF (outf, "########################################\n");
    ajFmtPrintF (outf, "# Program: %s\n", ajAcdProgram());
    ajFmtPrintF (outf, "# Rundate: %D\n", ajTimeTodayF("log"));
    if (thys->Multi) {
      ajFmtPrintF (outf, "########################################\n\n");
    }
    else {
      ajFmtPrintF (outf, "#\n");
    }
  }

  /* Sequence header (can be part of top header) */

  if (thys->Multi) {
    ajFmtPrintF (outf, "#=======================================\n#\n");
  }

  ajFmtPrintF (outf, "# Sequence: %S     from: %d   to: %d\n",
		 ajReportSeqName(thys, seq), ajSeqBegin(seq), ajSeqEnd(seq));

  if (ajStrLen(thys->Header)) {
    ajStrAssS (&tmpstr, thys->Header);
    ajStrSubstituteCC (&tmpstr, "\n", "\1# ");
    ajStrSubstituteCC (&tmpstr, "\1", "\n");
    ajFmtPrintF (outf, "#\n");
    ajFmtPrintF (outf, "# %S\n", tmpstr);
    ajFmtPrintF (outf, "#\n");
  }

  if (thys->Multi) {
    ajFmtPrintF (outf, "#=======================================\n\n");
    }
  else {
    ajFmtPrintF (outf, "########################################\n\n");
  }

  ++thys->Count;

  return;
}

/* @func ajReportWriteTail ************************************************
**
** Writes a feature report tail
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajReportWriteTail (AjPReport thys, AjPFeattable ftable, AjPSeq seq) {

  AjPFile outf = thys->File;
  AjPStr tmpstr=NULL;

  if (thys->Multi) {
    ajFmtPrintF (outf, "\n#-------------------------------------\n");
  }
  else {
    ajFmtPrintF (outf, "\n######################################\n");
  }

  if (ajStrLen(thys->Tail)) {
    ajStrAssS (&tmpstr, thys->Tail);
    ajStrSubstituteCC (&tmpstr, "\n", "\1# ");
    ajStrSubstituteCC (&tmpstr, "\1", "\n");
    ajFmtPrintF (outf, "#\n");
    ajFmtPrintF (outf, "# %S\n", tmpstr);
    ajFmtPrintF (outf, "#\n");
  }

  if (thys->Multi) {
    ajFmtPrintF (outf, "#---------------------------------------\n\n");
  }
  else {
    ajFmtPrintF (outf, "########################################\n\n");
  }

  return;
}

/* @func ajReportSetHeader ************************************************
**
** Defines a feature report header
**
** @param [R] thys [AjPReport] Report object
** @param [R] header [AjPStr] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetHeader (AjPReport thys, AjPStr header) {

  ajStrAssS (&thys->Header, header);

  return;
}

/* @func ajReportSetHeaderC ************************************************
**
** Defines a feature report header
**
** @param [R] thys [AjPReport] Report object
** @param [R] header [char*] Report header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetHeaderC (AjPReport thys, char* header) {

  ajStrAssC (&thys->Header, header);

  return;
}

/* @func ajReportSetTail ************************************************
**
** Defines a feature report tail
**
** @param [R] thys [AjPReport] Report object
** @param [R] tail [AjPStr] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetTail (AjPReport thys, AjPStr tail) {

  ajStrAssS (&thys->Tail, tail);

  return;
}

/* @func ajReportSetTailC ************************************************
**
** Defines a feature report tail
**
** @param [R] thys [AjPReport] Report object
** @param [R] tail [char*] Report tail with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajReportSetTailC (AjPReport thys, char* tail) {

  ajStrAssC (&thys->Tail, tail);

  return;
}

/* @func ajReportSetType ************************************************
**
** Sets the report type (if it is not set already)
**
** @param [R] thys [AjPReport] Report object
** @param [R] ftable [AjPFeattable] Feature table object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajReportSetType (AjPReport thys, AjPFeattable ftable, AjPSeq seq) {

  ajDebug("ajReportSetType '%S' ft: '%S' sq: '%S'\n",
	  thys->Type, ftable->Type, seq->Type);

  if (ajStrLen(thys->Type))
    return;

  if (ajStrLen(ftable->Type)) {
    ajStrAssS(&thys->Type, ftable->Type);
    return;
  }

  if (seq && ajStrLen(seq->Type)) {
    ajStrAssS(&thys->Type, seq->Type);
    return;
  }

  return;
}

/* @funcstatic reportCharname *************************************************
**
** Returns 'residues' for a protein report, 'bases' for a nucleotide report.
**
** @param [r] thys [AjPReport] Report object
** @return [char*] String to print the sequence character type
******************************************************************************/

static char* reportCharname (AjPReport thys) {

  static char* protstr = "residues";
  static char* nucstr = "bases";

  if (!ajStrLen(thys->Type))
    return protstr;

  switch (ajStrChar(thys->Type, 0)) {
  case 'n':
  case 'N':
    return nucstr;
  default:
    return protstr;
  }

  return protstr;
}

/* @funcstatic ajReportSeqName ************************************************
**
** Returns the sequence name or USA depending on the setting in the
** report object (derived from the ACD and command line -rusa option)
**
** @param [r] thys [AjPReport] Report object
** @param [r] seq [AjPSeq] Sequence object
** @return [AjPStr] Sequence name for this report
******************************************************************************/

AjPStr ajReportSeqName (AjPReport thys, AjPSeq seq) {

  if (thys->Showusa)
    return ajSeqGetUsa(seq);

  return ajSeqGetName(seq);
}

