/********************************************************************
** @source AJAX ALIGN (ajax alignment output) functions
**
** These functions align AJAX sequences and report them in a variety
** of formats.
**
** @author Copyright (C) 2001 Peter Rice, LION Bioscience Ltd.
** @version 1.0 
** @modified Aug 21 2001 First version
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

typedef struct AlignSData {
  ajint* Start;
  ajint* End;
  AjBool* Rev;
  ajint Len;
} AlignOData, *AlignPData;

typedef struct AlignSFormat {
  char *Name;
  AjBool Nuc;
  AjBool Prot;
  ajint Minseq;
  ajint Maxseq;
  void (*Write) (AjPAlign thys, AjPSeqset seqset);
} AlignOFormat, *AlignPFormat;

static void alignConsStats(AjPSeqset seqset,AjPMatrix cmpmatrix, AjPStr *cons,
		  ajint* retident, ajint* retsim, ajint* retgap,
		  ajint* retlen);
static void alignDataDel (AlignPData* pthys);
static void alignDiff (AjPStr* pmark, AjPStr seq);
static void alignWriteMsf (AjPAlign thys, AjPSeqset seqset);

static void alignWriteTrace (AjPAlign thys, AjPSeqset seqset);
static void alignWriteBlast (AjPAlign thys, AjPSeqset seqset);
static void alignWriteSimple (AjPAlign thys, AjPSeqset seqset);

static AjPStr alignSeqName (AjPAlign thys, AjPSeqset seqset, ajint i);
static AjPStr alignSeqsetName (AjPAlign thys, AjPSeqset seqset);
static void alignSim (AjPStr* pmark, const char idch, const char simch,
		      const char misch, const char gapch);

/* @funclist alignFormat *****************************************************
**
** Functions to write alignments
**
******************************************************************************/

static AlignOFormat alignFormat[] = { 
  /* standard sequence formats */
  {"unknown",   AJFALSE, AJFALSE, 0, 0, alignWriteSimple},
  {"msf",       AJTRUE,  AJFALSE, 0, 0, alignWriteMsf},
  /* trace  for debug */
  {"trace",     AJTRUE,  AJTRUE,  0, 0, alignWriteTrace},
  /* alignment formats */
  {"blast",     AJTRUE,  AJTRUE,  0, 0, alignWriteBlast},
  {"multiple",  AJTRUE,  AJTRUE,  0, 0, alignWriteSimple},
  {"pair",      AJTRUE,  AJTRUE,  2, 2, alignWriteSimple},
  {"simple",    AJTRUE,  AJTRUE,  0, 0, alignWriteSimple},
  {NULL, 0, 0, 0, 0, NULL}
};

/* pair only works if the alignment is defined as 2 sequences */

/* other formats to be defined:
**
** markx1 .. markx10 as for FASTA (code in matcher.c)
** blast to be implemented
*/

/* @funcstatic alignWriteTrace ***********************************************
**
** Writes an alignment in Trace format
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

static void alignWriteTrace (AjPAlign thys, AjPSeqset seqset) {

  ajFmtPrintF (thys->File, "Trace output\n");

  return;
}

/* @funcstatic alignWriteMsf ************************************************
**
** Writes an alignment in MSF format
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seqset [AjPSeqset] Sequence set object
** @return [void]
** @@
******************************************************************************/

static void alignWriteMsf (AjPAlign thys, AjPSeqset seqset) {

  static AjPStr sqfmt = NULL;
  AjPSeqout seqout = ajSeqoutNewF(thys->File);

  if (!sqfmt)
    ajStrAssC (&sqfmt, "msf");

  ajStrAssS (&seqout->Formatstr, sqfmt);
  seqout->File = thys->File;

  ajSeqoutOpen(seqout);

  ajSeqsetWrite (seqout, seqset);
  ajSeqWriteClose (seqout);
  seqout->File = NULL;

  return;
}

/* @funcstatic alignWriteBlast ************************************************
**
** Writes an alignment in Blast format
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seqset [AjPSeqset] Sequence set object
** @return [void]
** @@
******************************************************************************/

static void alignWriteBlast (AjPAlign thys, AjPSeqset seqset) {

  ajFmtPrintF (thys->File, "#Blast output\n");

  return;
}

/* @funcstatic alignWriteSimple ***********************************************
**
** Writes an alignment in Simple format
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seqset [AjPSeqset] Sequence set object
** @return [void]
** @@
******************************************************************************/

static void alignWriteSimple (AjPAlign thys, AjPSeqset seqset) {

  AjPFile outf = thys->File;
  int nseq = ajSeqsetSize(seqset);
  int nali;
  AjPStr *seqs = NULL;
  AlignPData* pdata;
  AlignPData data;
  ajint iali;
  ajint iseq;
  ajint i;
  ajint istart;
  ajint iend;
  ajint ilen;
  ajint iwidth = 50;
  AjPStr tmpstr = NULL;
  AjPStr mrkstr = NULL;
  AjPStr mrkcons = NULL;
  AjPStr cons = NULL;
  ajint identity=0;
  ajint similarity=0;
  ajint gaps=0;
  ajint seqlen=0;
  float pctid;
  float pctsim;
  float pctgap;

  alignConsStats (seqset, NULL, &cons,
		  &identity, &similarity, &gaps, &seqlen);

  if (thys->Width)
    iwidth = thys->Width;

  nali = ajListToArray (thys->Data, (void***) &pdata);

  AJCNEW (seqs, nseq);
  for (i=0; i<nseq; i++) {
    seqs[i] = ajSeqStr(ajSeqsetGetSeq (seqset, i));
  }

  ajAlignWriteHeader (thys, seqset);

  pctid  = 100. * (float) identity / (float) seqlen;
  pctsim = 100. * (float) similarity / (float) seqlen;
  pctgap = 100. * (float) gaps / (float) seqlen;
  ajFmtPrintF (outf, "# identity:   %5d/%d (%4.1f%%)\n",
	       identity, seqlen, pctid);
  ajFmtPrintF (outf, "# similarity: %5d/%d (%4.1f%%)\n",
	       similarity, seqlen, pctsim);
  ajFmtPrintF (outf, "# gaps:       %5d/%d (%4.1f%%)\n", gaps,
	       seqlen, pctgap);
  ajFmtPrintF (outf, "# len: %d\n", seqlen);
  ajFmtPrintF (outf, "# consens: '%S'\n\n", cons);

  alignSim (&cons, '|', ':', '.', ' ');

  ajFmtPrintF (outf, "# modcons: '%S'\n\n", cons);

  for (iali=0; iali<nali; iali++) {
    data = pdata[iali];
    ilen = data[iali].Len;
    for (i=0; i < ilen; i += iwidth) {

      for (iseq=0; iseq < nseq; iseq++) {
	istart = i+data[iali].Start[iseq]-1;
	iend = AJMIN(data[iali].End[iseq]-1, istart+iwidth-1);
	ajStrAssSub(&tmpstr, seqs[iseq], istart, iend);
	ajStrAssSub(&mrkcons, cons, istart, iend);

	if (!iseq)
	  ajStrAssS(&mrkstr, tmpstr);
	else {
	  alignDiff(&mrkstr, tmpstr);
	}

	if (nseq==2 && iseq==1) /* 2 seqs, markup between them */
	  ajFmtPrintF (outf,
		       "                       '%S'\n",
		       mrkcons);

	ajFmtPrintF (outf,
		     "%-15.15S %6d '%S' %6d\n",
		     alignSeqName(thys, seqset, iseq),
		     istart+1, tmpstr, iend+1);
      }
      if (nseq > 2) {		/* 3 or more seqs, markup under */
	ajFmtPrintF (outf,
		     "                       '%S'\n",
		     mrkcons);
      }
      ajFmtPrintF (outf, "\n");
    }
  }
  AJFREE (pdata);
  AJFREE (seqs);

  return;
}

/* @func ajAlignDefine ********************************************************
**
** Defines a sequence set as an alignment
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seqset [AjPSeqset] Sequence set object
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajAlignDefine (AjPAlign thys, AjPSeqset seqset) {

  AlignPData data = NULL;
  ajint i;

  AJNEW0 (data);

  thys->Nseqs = ajSeqsetSize(seqset);
  AJCNEW0 (data->Start, thys->Nseqs);
  AJCNEW0 (data->End, thys->Nseqs);
  AJCNEW0 (data->Rev, thys->Nseqs);

  for (i=0; i < thys->Nseqs; i++) {
    data->Start[i] = 1;
    data->End[i] = ajSeqLen(ajSeqsetGetSeq(seqset, i));
    data->Rev[i] = ajFalse;
  }

  data->Len = ajSeqsetLen(seqset);

  ajListPushApp (thys->Data, data);

  return ajTrue;
}

/* @func ajAlignDel **********************************************************
**
** Destructor for Alignment objects
**
** @param [D] pthys [AjPAlign*] Alignment object reference
** @return [void]
** @@
******************************************************************************/

void ajAlignDel (AjPAlign* pthys) {

  AlignPData data;
  AjPAlign thys = *pthys;

  ajStrDel (&thys->Name);
  ajStrDel (&thys->Formatstr);
  ajStrDel (&thys->Filename);
  ajStrDel (&thys->Extension);

  while (ajListPop(thys->Data, (void**) &data)) {
    alignDataDel(&data);
  }

  ajListFree (&thys->Data);

  AJFREE(*pthys);

  return;
}

/* @func ajAlignOpen *********************************************************
**
** Opens a new align file
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] name [AjPStr] File name
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajAlignOpen (AjPAlign thys, AjPStr name) {

  if (!ajAlignValid(thys)) return ajFalse;

  thys->File = ajFileNewOut(name);
  if (thys->File)
    return ajTrue;

  return ajFalse;
}

/* @func ajAlignFormatDefault ************************************************
**
** Sets the default format for an alignment
**
** @param [W] pformat [AjPStr*] Default format returned
** @return [AjBool] ajTrue is format was returned
** @@
******************************************************************************/

AjBool ajAlignFormatDefault (AjPStr* pformat) {

  if (ajStrLen(*pformat)) {
    ajDebug ("... output format '%S'\n", *pformat);
  }
  else {
    /* ajStrSetC (pformat, alignFormat[0].Name);*/
    (void) ajStrSetC (pformat, "gff"); /* use the real name */
    ajDebug ("... output format not set, default to '%S'\n", *pformat);
  }

  return ajTrue;
}

/* @func ajAlignFindFormat ***********************************************
**
** Looks for the specified align format in the internal definitions and
** returns the index.
**
** @param [P] format [AjPStr] Format required.
** @param [w] iformat [ajint*] Index
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajAlignFindFormat (AjPStr format, ajint* iformat) {

  AjPStr tmpformat = NULL;
  ajint i = 0;

  if (!ajStrLen(format))
    return ajFalse;

  (void) ajStrAss (&tmpformat, format);
  (void) ajStrToLower(&tmpformat);

  while (alignFormat[i].Name) {
    if (ajStrMatchCaseC(tmpformat, alignFormat[i].Name)) {
      *iformat = i;
      ajStrDel(&tmpformat);
      return ajTrue;
    }
    i++;
  }

  ajStrDel(&tmpformat);
  return ajFalse;
}

/* @func ajAlignValid ************************************************
**
** Test for an alignment object.
**
** Checks the format works with the number of sequences.
** Checks the format works with the type (protein or nucleotide).
**
** @param [R] thys [AjPAlign] Alignment object
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajAlignValid (AjPAlign thys) {

  if (!thys->Format) {
    if (!ajAlignFindFormat(thys->Formatstr, &thys->Format)) {
      ajWarn("Unknown alignment format '%S'", thys->Formatstr);
      return ajFalse;
    }
  }

  if ( alignFormat[thys->Format].Minseq  &&
       thys->Nmin < alignFormat[thys->Format].Minseq) {
    ajWarn("Alignment format specifies at least %d sequences, alignment has only %d",
	   alignFormat[thys->Format].Minseq, thys->Nmin);
    return ajFalse;
  }

  if ( alignFormat[thys->Format].Maxseq  &&
       thys->Nmax > alignFormat[thys->Format].Maxseq) {
    ajWarn("Alignment format specifies at most %d sequences, alignment has  %d",
	   alignFormat[thys->Format].Minseq, thys->Nmin);
    return ajFalse;
  }

  return ajTrue;
}

/* @func ajAlignNew ************************************************
**
** Constructor for an alignment object
**
** @return [AjPAlign] New Alignment object
** @@
******************************************************************************/

AjPAlign ajAlignNew (void) {

  AjPAlign pthis;

  AJNEW0(pthis);

  pthis->Count = 0;
  pthis->Name = ajStrNew();
  pthis->Formatstr = ajStrNew();
  pthis->Format = 0;
  pthis->Filename = ajStrNew();
  pthis->Extension = ajStrNew();
  pthis->File = NULL;
  pthis->Data = ajListNew();

  return pthis;
}

/* @func ajAlignWrite ************************************************
**
** Writes an alignment file
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seq [AjPSeq] Sequence object
** @return [void]
** @@
******************************************************************************/

void ajAlignWrite (AjPAlign thys, AjPSeqset seqset) {

  ajDebug ("ajAlignWrite\n");

  if (!thys->Format) {
    if (!ajAlignFindFormat(thys->Formatstr, &thys->Format)) {
      ajErr ("unknown align format '%S'", thys->Formatstr);
    }
  }

  ajDebug ("ajAlignWrite %d '%s'\n",
	   thys->Format, alignFormat[thys->Format].Name);

  ajAlignSetType (thys, seqset);
  ajAlignSetHeaderC (thys, "Testing\n1..2..3");

  /* EFUNC NOTE: next line Write calls funclist alignFormat() */

  alignFormat[thys->Format].Write (thys, seqset);

  return;
}

/* @func ajAlignClose ********************************************************
**
** Closes an alignment
**
** @param [R] thys [AjPAlign] Alignment object
** @return [void]
** @@
******************************************************************************/

void ajAlignClose (AjPAlign thys) {

  ajDebug ("ajAlignClose '%F'\n", thys->File);

  ajFileClose (&thys->File);

  return;
}

/* @func ajAlignWriteHeader ************************************************
**
** Writes an alignment header
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seqet [AjPSeqset] Sequence set object
** @return [void]
** @@
******************************************************************************/

void ajAlignWriteHeader (AjPAlign thys, AjPSeqset seqset) {

  AjPFile outf = thys->File;
  AjPStr tmpstr=NULL;
  AjBool doSingle=ajFalse;	/* turned off for now - always multi format */
  ajint i;

  if (!thys->Count) {
    ajFmtPrintF (outf, "########################################\n");
    ajFmtPrintF (outf, "# Program:  %s\n", ajAcdProgram());
    ajFmtPrintF (outf, "# Rundate:  %D\n", ajTimeTodayF("log"));
    ajFmtPrintF (outf, "# Report file: %F\n", outf);
    if (!doSingle || thys->Multi) {
      ajFmtPrintF (outf, "########################################\n\n");
    }
    else {
      ajFmtPrintF (outf, "#\n");
    }
  }

  if (!doSingle || thys->Multi) {
    ajFmtPrintF (outf, "#=======================================\n\n");
  }

  ajFmtPrintF (outf, "# Alignment: %S     from: %d   to: %d\n",
	       alignSeqsetName(thys, seqset),
	       ajSeqsetBegin(seqset), ajSeqsetEnd(seqset));
  for (i=0; i < ajSeqsetSize(seqset); i++) {
    ajFmtPrintF (outf, "# Sequence: %S\n",
	       alignSeqName(thys, seqset, i));
  }

  if (ajStrLen(thys->Matrix))
    ajFmtPrintF (outf, "# Matrix: %S\n", thys->Matrix);

  if (ajStrLen(thys->GapPen))
    ajFmtPrintF (outf, "# Gap penalty: %S\n", thys->GapPen);

  if (ajStrLen(thys->ExtPen))
    ajFmtPrintF (outf, "# Extend penalty: %S\n", thys->ExtPen);

  if (ajStrLen(thys->Header)) {
    ajStrAssS (&tmpstr, thys->Header);
    ajStrSubstituteCC (&tmpstr, "\n", "\1# ");
    ajStrSubstituteCC (&tmpstr, "\1", "\n");
    ajFmtPrintF (outf, "#\n");
    ajFmtPrintF (outf, "# %S\n", tmpstr);
    ajFmtPrintF (outf, "#\n");
  }

  if (!doSingle || thys->Multi) {
    ajFmtPrintF (outf, "#=======================================\n\n");
    }
  else {
    ajFmtPrintF (outf, "########################################\n\n");
  }

  ++thys->Count;

  return;
}

/* @func ajAlignSetHeader ************************************************
**
** Defines an alignment header
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] header [AjPStr] Align header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajAlignSetHeader (AjPAlign thys, AjPStr header) {

  ajStrAssS (&thys->Header, header);

  return;
}

/* @func ajAlignSetHeaderC ************************************************
**
** Defines an alignment header
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] header [char*] Align header with embedded newlines
** @return [void]
** @@
******************************************************************************/

void ajAlignSetHeaderC (AjPAlign thys, char* header) {

  ajStrAssC (&thys->Header, header);

  return;
}

/* @func ajAlignSetMatrixC ************************************************
**
** Defines an alignment matrix
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] matrix [const char*] Matrix name
** @return [void]
** @@
******************************************************************************/

void ajAlignSetMatrixC (AjPAlign thys, const char* matrix) {

  ajStrAssC (&thys->Matrix, matrix);

  return;
}

/* @func ajAlignSetMatrix ************************************************
**
** Defines an alignment matrix
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] matrix [AjPStr] Matrix name
** @return [void]
** @@
******************************************************************************/

void ajAlignSetMatrix (AjPAlign thys, AjPStr matrix) {

  ajAlignSetMatrixC(thys, ajStrStr(matrix));

  return;
}

/* @func ajAlignSetGapI ************************************************
**
** Defines alignment gap penalties
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] gappen [ajint] Gap penalty
** @param [R] extpen [ajint] Gap extension penalty
** @return [void]
** @@
******************************************************************************/

void ajAlignSetGapI (AjPAlign thys, ajint gappen, ajint extpen) {

  static AjPStr tmpstr=NULL;

  ajFmtPrintS (&tmpstr, "%d", gappen);
  ajStrAssS (&thys->GapPen, tmpstr);

  ajFmtPrintS (&tmpstr, "%d", extpen);
  ajStrAssS (&thys->ExtPen, tmpstr);

  return;
}

/* @func ajAlignSetGapR ************************************************
**
** Defines alignment gap penalties
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] gappen [float] Gap penalty
** @param [R] extpen [float] Gap extension penalty
** @return [void]
** @@
******************************************************************************/

void ajAlignSetGapR (AjPAlign thys, float gappen, float extpen) {

  static AjPStr tmpstr=NULL;
  ajint precision=3;
  ajint i;

  ajFmtPrintS (&tmpstr, "%.3f", gappen);
  for (i=1; i<precision; i++) {
    if (ajStrChar(tmpstr, -1) != '0') break;
    ajStrTrim(&tmpstr, -1);
  }
  ajStrAssS (&thys->GapPen, tmpstr);

  ajFmtPrintS (&tmpstr, "%.3f", extpen);
  for (i=1; i<precision; i++) {
    if (ajStrChar(tmpstr, -1) != '0') break;
    ajStrTrim(&tmpstr, -1);
  }
  ajStrAssS (&thys->ExtPen, tmpstr);

  return;
}

/* @func ajAlignSetType ************************************************
**
** Sets the align type (if it is not set already)
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] seqset [AjPSeqset] Sequence set object
** @return [void]
** @@
******************************************************************************/

void ajAlignSetType (AjPAlign thys, AjPSeqset seqset) {

  ajDebug("ajAlignSetType '%S' sq: '%S'\n",
	  thys->Type, seqset->Type);

  if (ajStrLen(thys->Type))
    return;

  if (seqset && ajStrLen(seqset->Type)) {
    ajStrAssS(&thys->Type, seqset->Type);
    return;
  }

  return;
}

/* @funcstatic alignSeqName **************************************************
**
** Returns the sequence name or USA depending on the setting in the
** Alignment object (derived from the ACD and command line -ausa option)
**
** @param [r] thys [AjPAlign] Alignment object
** @param [r] seqset [AjPSeqset] Sequence set object
** @param [r] i [ajint] Sequence number in set
** @return [AjPStr] Sequence name for this alignment
******************************************************************************/

static AjPStr alignSeqName (AjPAlign thys, AjPSeqset seqset, ajint i) {

  AjPSeq seq = ajSeqsetGetSeq (seqset, i);

  if (thys->Showusa)
    return ajSeqGetUsa(seq);

  return ajSeqGetName(seq);
}

/* @funcstatic alignSeqsetName ************************************************
**
** Returns the sequence name or USA depending on the setting in the
** Alignment object (derived from the ACD and command line -ausa option)
**
** @param [r] thys [AjPAlign] Alignment object
** @param [r] seq [AjPSeqset] Sequence set object
** @return [AjPStr] Sequence name for this alignment
******************************************************************************/

static AjPStr alignSeqsetName (AjPAlign thys, AjPSeqset seqset) {

  if (thys->Showusa)
    return ajSeqsetGetUsa(seqset);

  return ajSeqsetGetName(seqset);
}

/* @funcstatic alignDataDel **************************************************
**
** Deletes an alignment data structure
**
** @param [D] pthys [AlignPData*] Alignment data structure
** @return [void]
******************************************************************************/

static void alignDataDel (AlignPData* pthys) {

  AlignPData thys = *pthys;

  AJFREE (thys->Start);
  AJFREE (thys->End);
  AJFREE (thys->Rev);
  AJFREE (*pthys);

  return;
}

/* @funcstatic alignDiff ******************************************************
**
** Blank out differences between two strings
**
** @param [W] pmark [AjPStr*] Mark string with spaces for differences
** @return [void]
******************************************************************************/

static void alignDiff (AjPStr* pmark, AjPStr seq) {

  ajint i;
  ajint ilen = ajStrLen(seq);
  char c;
  char d;
  char* s = " ";

  ajStrMod (pmark);
  
  if (ajStrLen(*pmark) < ilen)
    ilen = ajStrLen(*pmark);

  for (i=0; i < ilen; i++) {
    c = ajStrChar(*pmark, i);
    if (c == ' ') continue;
    d = ajStrChar(seq, i);
    if (toupper(c) == toupper(d)) continue;
    ajStrReplaceC (pmark, i, s, 1);
  }

  return;
}

/* @funcstatic alignSim ******************************************************
**
** Convert upper case (identical) positions to an identity character,
** and lower case (similar) positions to a similarity character
**
** @param [W] pmark [AjPStr*] Mark string with spaces for differences
** @param [r] idch [const char] Identity character
** @param [r] simch [const char] Similarity character
** @param [r] misch [const char] Mismatch character
** @param [r] gapch [const char] Gap character
** @return [void]
******************************************************************************/

static void alignSim (AjPStr* pmark, const char idch, const char simch,
		      const char misch, const char gapch) {

  ajint i;
  ajint ilen;
  char c;

  ajDebug ("alignSim '%S'\n", *pmark);

  ajStrMod (pmark);
  
  ilen = ajStrLen(*pmark);

  for (i=0; i < ilen; i++) {
    c = ajStrChar(*pmark, i);
    if (tolower(c) == 'x')
      ajStrReplaceK (pmark, i, misch, 1);
    else if (isupper((int)c))
      ajStrReplaceK (pmark, i, idch, 1);
    else if (islower((int)c))
      ajStrReplaceK (pmark, i, simch, 1);
    else
      ajStrReplaceK (pmark, i,gapch, 1);
  }

  ajDebug ("  result '%S'\n", *pmark);
  return;
}

/* @funcstatic alignConsStats *************************************************
**
** Calculates alignment statistics (and a consensus).
**
** @param [r] seqset [AjPSeqset] multiple alignment
** @param [r] cmpmatrix [AjPMatrix] comparison scoring matrix
** @param [w] cons [AjPStr*] the created consensus sequence
** @param [r] retident [ajint*] number of residues identical in all sequences
** @param [r] retsim   [ajint*] number of residues similar in all sequences
** @param [r] retgap   [ajint*] number of residues with a gap in 1 sequence
** @param [r] retlen [ajint*] length of the alignment
** @return [void]
******************************************************************************/

static void alignConsStats(AjPSeqset seqset,AjPMatrix cmpmatrix, AjPStr *cons,
		  ajint* retident, ajint* retsim, ajint* retgap,
		  ajint* retlen)
{
  ajint   imat;		      /* iterate over identical and matching arrays */
  ajint   iseq;			/* iterate over sequences (outer loop) */
  ajint   jseq;			/* iterate over sequences (inner loop) */
  ajint   kpos;
  ajint   **matrix;
  ajint   m1=0;
  ajint   m2=0;
  ajint   matsize;
  ajint   matchingmaxindex;
  ajint   identicalmaxindex;
  ajint   nseqs;
  ajint   mlen;

  float   max;
  float   contri=0;
  float   contrj=0;

  AjPSeqCvt cvt=0;

  AjPFloat  posScore=NULL;	/* cumulative similarity scores by sequence */
				/* for matching all other sequences */
  float   *identical;		/* cum. weight for each valid character */
  float   *matching;		/* cum. weight for matching this character */
  ajint   highindex;		/* position of highest score in posScore */

  char **seqcharptr;
  char res;
  char nocon;
  char gapch;
  float fplural;
  float fplurality = 50.0;
  float setcase = 0.0;
  float ident;
  AjPStr matfname=NULL;
  AjBool isident;
  AjBool issim;
  AjBool isgap;

  if (!cmpmatrix) {
    if(ajSeqsetIsNuc(seqset))
      ajStrAssC (&matfname, "EDNAFULL");
    else
      ajStrAssC (&matfname, "EBLOSUM62");
    ajMatrixRead(&cmpmatrix, matfname);
  }

  *retident=0;
  *retsim=0;
  *retgap=0;

  nseqs = ajSeqsetSize(seqset);
  mlen = ajSeqsetLen(seqset);
  fplural = ajSeqsetTotweight(seqset) * fplurality / 100.;
  ident = ajSeqsetTotweight(seqset);
  setcase = ident;

  ajDebug("fplural:%.1f ident:%.1f setcase:%.1f\n", fplural, ident, setcase);

  matrix  = ajMatrixArray(cmpmatrix);
  cvt     = ajMatrixCvt(cmpmatrix);    /* return conversion table */
  matsize = ajMatrixSize(cmpmatrix);

  AJCNEW(seqcharptr,nseqs);
  AJCNEW(identical,matsize);
  AJCNEW(matching,matsize);

  posScore = ajFloatNew();

  gapch = '-';
  nocon = 'x';

  for(iseq=0;iseq<nseqs;iseq++)	   /* get sequence as string */
    seqcharptr[iseq] =  ajSeqsetSeq(seqset, iseq);  

  /* For each position in the alignment, calculate consensus character */

  for(kpos=0; kpos< mlen; kpos++)
  {
    res = gapch;

    isident=ajFalse;
    issim=ajFalse;
    isgap=ajFalse;

    /*
    ** reset identities and +ve matches
    */

    for(imat=0;imat<matsize;imat++) 
    {
      identical[imat] = 0.0;	/* weights of all sequence chars in column */
      matching[imat] = 0.0;
    }

    /*
    ** reset the posScore array
    */

    for(iseq=0;iseq<nseqs;iseq++) 
      ajFloatPut(&posScore,iseq,0.);
 
    /*
    ** generate scores (identical, posScore) for columns
    */

    for(iseq=0;iseq<nseqs;iseq++)
    {
      m1 = ajSeqCvtK(cvt,seqcharptr[iseq][kpos]);
      if(m1)
	identical[m1] += ajSeqsetWeight(seqset,iseq);

      for(jseq=iseq+1;jseq<nseqs;jseq++) 
      {
	m2 = ajSeqCvtK(cvt,seqcharptr[jseq][kpos]);
	if(m1 && m2)
        {
	  contri = (float)matrix[m1][m2]*ajSeqsetWeight(seqset,jseq) 
	    +ajFloatGet(posScore,iseq);
	  contrj = (float)matrix[m1][m2]*ajSeqsetWeight(seqset,iseq)
	    +ajFloatGet(posScore,jseq);

	  ajFloatPut(&posScore,iseq,contri);
	  ajFloatPut(&posScore,jseq,contrj);
	}
      }
    }

    /*
    ** highindex is the highest scoring position (seq no.) in posScore
    ** for 2 sequences this appears to be usually 0
    */

    highindex = -1;
    max = -FLT_MAX;
    for(iseq=0;iseq<nseqs;iseq++)
    {
      if(ajFloatGet(posScore,iseq) > max) 
      {
	highindex = iseq;
	max       = ajFloatGet(posScore,iseq);
      }
    }

    /*
    ** find +ve matches in the column
    ** m1 is non-zero for a valid character in iseq
    ** m2 is non-zero for a valid character in jseq
    */

    for(iseq=0;iseq<nseqs;iseq++)
    {
      m1 = ajSeqCvtK (cvt, seqcharptr[iseq][kpos]);
      if(!matching[m1])		/* first time we have met this character */
      {
	for(jseq=0;jseq<nseqs;jseq++) /* all (other) sequences */
        {
	  m2 = ajSeqCvtK (cvt, seqcharptr[jseq][kpos]);
	  if(m1 && m2 && matrix[m1][m2] > 0) { /* 'matching' if positive */
	    matching[m1] += ajSeqsetWeight(seqset, jseq);
	  }

/*
//	  if( iseq != jseq)	/# skip the sequence we are on #/
//          {
//	    m2 = ajSeqCvtK (cvt, seqcharptr[jseq][kpos]);
//	    if(m1 && m2 && matrix[m1][m2] > 0) { /# 'matching' if positive #/
//	      matching[m1] += ajSeqsetWeight(seqset, jseq);
//	    }
//	  }
*/
	}
      }
    }

    matchingmaxindex  = 0;      /* get max matching and identical */
    identicalmaxindex = 0;
    for(iseq=0;iseq<nseqs;iseq++)
    {
      m1 = ajSeqCvtK(cvt,seqcharptr[iseq][kpos]);
      if(identical[m1] > identical[identicalmaxindex])
	identicalmaxindex= m1;
    }

    for(iseq=0;iseq<nseqs;iseq++)
    {
      m1 = ajSeqCvtK(cvt,seqcharptr[iseq][kpos]);
      if(matching[m1] > matching[matchingmaxindex])
      {
	matchingmaxindex= m1;
      }
      else if(matching[m1] ==  matching[matchingmaxindex])
      {
	if(identical[m1] > identical[matchingmaxindex])
	{
	  matchingmaxindex= m1;
	}
      }
      if (seqcharptr[iseq][kpos] == '-')
      {
	isgap=ajTrue;
      }
    }
/*
//    ajDebug("index[%d] ident:%d matching:%d high:%d\n",
//	    kpos,
//	    identicalmaxindex, 
//	    matchingmaxindex, highindex);
*/

    ajDebug("index[%d] ident:%d '%c' %.1f matching:%d '%c' %.1f %.1f high:%d '%c' %.1f\n",
	    kpos,
	    identicalmaxindex, ajMatrixChar(cmpmatrix, identicalmaxindex-1),
	    identical[identicalmaxindex],
	    matchingmaxindex,  ajMatrixChar(cmpmatrix, matchingmaxindex-1),
	    matching[matchingmaxindex],
	    matching[ajSeqCvtK(cvt,seqcharptr[highindex][kpos])],
	    highindex, seqcharptr[highindex][kpos],
	    ajSeqsetWeight(seqset, highindex));

    if (identical[identicalmaxindex] >= ident) isident=ajTrue;
    if (matching[matchingmaxindex] >= fplural) issim=ajTrue;

    /* plurality check */
    if(matching[ajSeqCvtK(cvt,seqcharptr[highindex][kpos])] >= fplural) {
      if (seqcharptr[highindex][kpos] != '-')
      {
	res = toupper(seqcharptr[highindex][kpos]);
      }
    }

    if(matching[ajSeqCvtK(cvt,seqcharptr[highindex][kpos])] < setcase)
    {
      res = tolower(res);
    }

    if(matching[ajSeqCvtK(cvt,seqcharptr[highindex][kpos])] == ajSeqsetWeight(seqset, highindex))
      {
      res = nocon;
    }

    ajStrAppK(cons,res);
    if (isident) ++*retident;
    if (issim) ++*retsim;
    if (isgap) ++*retgap;

    ajDebug ("id:%b sim:%b gap:%b res:%c '", isident, issim, isgap, res);
    for (iseq=0; iseq<nseqs; iseq++) {
      ajDebug("%c", seqcharptr[iseq][kpos]);
    }
    ajDebug ("'\n");

  }

  *retlen = ajSeqsetLen(seqset);

  ajDebug ("ret ident:%d sim:%d gap:%d len:%d\n",
	   *retident, *retsim, *retgap, *retlen);

  AJFREE(seqcharptr);
  AJFREE(matching);
  AJFREE(identical);
  ajFloatDel(&posScore);

  return;

}
