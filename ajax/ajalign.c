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
  void (*Write) (AjPAlign thys, AjPSeqset seqset);
} AlignOFormat, *AlignPFormat;

static void alignDataDel (AlignPData* pthys);
static void alignDiff (AjPStr* pmark, AjPStr seq);
static void alignWriteMsf (AjPAlign thys, AjPSeqset seqset);

static void alignWriteTrace (AjPAlign thys, AjPSeqset seqset);
static void alignWriteBlast (AjPAlign thys, AjPSeqset seqset);
static void alignWriteSimple (AjPAlign thys, AjPSeqset seqset);

static AjPStr alignSeqName (AjPAlign thys, AjPSeqset seqset, ajint i);
static AjPStr alignSeqsetName (AjPAlign thys, AjPSeqset seqset);

/* @funclist alignFormat *****************************************************
**
** Functions to write alignments
**
******************************************************************************/

static AlignOFormat alignFormat[] = { 
  /* standard sequence formats */
  {"msf",       AJTRUE,  AJFALSE, alignWriteMsf},
  /* trace  for debug */
  {"trace",     AJTRUE,  AJTRUE,  alignWriteTrace},
  /* alignment formats */
  {"blast",     AJTRUE,  AJTRUE,  alignWriteBlast},
  {"simple",    AJTRUE,  AJTRUE,  alignWriteSimple},
  {NULL, 0, 0, NULL}
};

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
  int nali =ajListLength(thys->Data);
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

  if (thys->Width)
    iwidth = thys->Width;

  ajListToArray (thys->Data, (void***) &pdata);

  AJCNEW (seqs, nseq);
  for (i=0; i<nseq; i++) {
    seqs[i] = ajSeqStr(ajSeqsetGetSeq (seqset, i));
  }

  ajFmtPrintF (thys->File, "#Simple output\n");

  ajAlignWriteHeader (thys, seqset);

  for (iali=0; iali<nali; iali++) {
    data = pdata[iali];
    ilen = data[iali].Len;
    for (i=0; i < ilen; i += iwidth) {

      for (iseq=0; iseq < nseq; iseq++) {
	istart = i+data[iali].Start[iseq]-1;
	iend = AJMIN(data[iali].End[iseq]-1, istart+iwidth-1);
	ajStrAssSub(&tmpstr, seqs[iseq], istart, iend);
	if (!iseq)
	  ajStrAssS(&mrkstr, tmpstr);
	else {
	  alignDiff(&mrkstr, tmpstr);
	  if (iseq == (nseq-1))
	    ajFmtPrintF (outf,
			 "                       '%S'\n",
			 mrkstr);
	}

	ajFmtPrintF (outf,
		     "%-15.15S %6d '%S' %6d\n",
		     alignSeqName(thys, seqset, iseq),
		     istart+1, tmpstr, iend+1);
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
** Checks the format works with the number of tags.
** Checks the format works with the type (protein or nucleotide).
**
** @param [R] thys [AjPAlign] Alignment object
** @param [R] mintags [ajint] Minimum number of tags to use in align
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajAlignValid (AjPAlign thys) {

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

  ajFmtPrintF (outf, "########################################\n");
  ajFmtPrintF (outf, "# Program:  %s\n", ajAcdProgram());
  ajFmtPrintF (outf, "# Rundate:  %D\n", ajTimeTodayF("log"));

  ajFmtPrintF (outf, "# Sequence: %S     from: %d   to: %d\n",
	       alignSeqsetName(thys, seqset),
	       ajSeqsetBegin(seqset), ajSeqsetEnd(seqset));

  if (ajStrLen(thys->Header)) {
    ajStrAssS (&tmpstr, thys->Header);
    ajStrSubstituteCC (&tmpstr, "\n", "\1# ");
    ajStrSubstituteCC (&tmpstr, "\1", "\n");
    ajFmtPrintF (outf, "#\n");
    ajFmtPrintF (outf, "# %S\n", tmpstr);
    ajFmtPrintF (outf, "#\n");
  }
  ajFmtPrintF (outf, "########################################\n\n");

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

/* @funcstatic alignSeqsetName **************************************************
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

