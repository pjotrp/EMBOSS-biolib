#ifdef __cplusplus
extern "C"
{
#endif

#ifndef embshow_h
#define embshow_h


/********* Descriptor object types *****************/

enum ShowEValtype
{
    SH_SEQ,
    SH_BLANK,
    SH_TICK,
    SH_TICKNUM,
    SH_COMP,
    SH_TRAN,
    SH_RE,
    SH_FT,
    SH_NOTE
};

/* @data EmbPShow *************************************************************
**
** NUCLEUS data structure for EmbPShow object for a sequence
**
** @attr list [AjPList] list of EmbPShowInfo structures
**
** @cc information about the sequence
**
** @attr seq [AjPSeq] the sequence
** @attr nucleic [AjBool] ajTrue = the sequence is nucleic
** @attr offset [ajint] offset to start numbering at
** @attr start [ajint] sequence position to start printing at
** @attr end [ajint] sequence position to stop printing at
**
** @cc information about the page layout
**
** @attr width [ajint] width of sequence to display on each line
** @attr length [ajint] length of a page (0 = indefinite)
** @attr margin [ajint] margin for numbers
** @attr html [AjBool] ajTrue = format page for HTML
** @@
******************************************************************************/

typedef struct EmbSShow {
  AjPList list;
  AjPSeq seq;
  AjBool nucleic;
  ajint offset;
  ajint start;
  ajint end;
  ajint width;
  ajint length;
  ajint margin;
  AjBool html;
} EmbOShow, *EmbPShow;


/* @data EmbPShowInfo *********************************************************
**
**
** The sequence and associated things to show are held in an ordered list
** of type EmbPShowInfo. This list is held in the structure EmbPShow.
**
** The things to show are displayed around the sequence in the order that
** they are held on the list.
**
** EmbPShowInfo holds the descriptor (one of EmbPShowBlank, EmbPShowTicks,
** EmbPShowSeq, EmbPShowComp, etc.) and the type of the descriptor (one of
** SH_BLANK, SH_TICKS, SH_SEQ, SH_COMP, etc.  )
**
** Each descriptor (EmbPShowSeq, EmbPShowBlank, EmbPShowTicks, etc.) holds
** information that could be useful in displaying its type of information.
**
** So, for example:
**
** EmbPShow could have a list of:
** ----------------------------
**
** EmbPShowInfo->type=SH_BLANK
**    |       ->info=EmbPShowBlank
**    |
** EmbPShowInfo->type=SH_TICKS
**    |       ->info=EmbPShowTicks
**    |
** EmbPShowInfo->type=SH_SEQ
**    |       ->info=EmbPShowSeq
**    |
** EmbPShowInfo->type=SH_COMP
**    |       ->info=EmbPShowComp
**    |
** EmbPShowInfo->type=etc.
**    |       ->info=etc.
**    |
**   etc.
**
** @attr type [enum ShowEValtype] Type of information (enumerated list)
** @attr info [void*] Information descriptor (set of available descriptors)
** @@
******************************************************************************/

typedef struct EmbSShowInfo {
  enum ShowEValtype type;
  void * info;
} EmbOShowInfo, *EmbPShowInfo;




/* @data EmbPShowSeq **********************************************************
**
** NUCLEUS data structure for sequence information, type = SH_SEQ
**
** @attr number [AjBool] ajTrue = number the sequence
** @attr threeletter [AjBool] ajTrue = display proteins in three letter code
** @attr upperrange [AjPRange] range of sequence to uppercase
** @attr highlight [AjPRange] range of sequence to colour in HTML
** @@
******************************************************************************/

typedef struct EmbSShowSeq {
  AjBool number;
  AjBool threeletter;
  AjPRange upperrange;
  AjPRange highlight;
} EmbOShowSeq, *EmbPShowSeq;

/* blank line information, type = SH_BLANK */

/* @data EmbPShowBlank ********************************************************
**
** NUCLEUS data structure for  blank line information, type = SH_BLANK
**
** @attr dummy [AjBool] Dummy attribute - no specific information needed
**                      AJNEW0() falls over if 0 bytes are allocated, so
**                      put in this dummy to pad the structure out
** @@
******************************************************************************/

typedef struct EmbSShowBlank {
  AjBool dummy;
} EmbOShowBlank, *EmbPShowBlank;

/* @data EmbPShowTicks ********************************************************
**
** NUCLEUS data structure for tick line information, type = SH_TICK
**
** @attr dummy [AjBool] Dummy attribute - no specific information needed
**                      AJNEW0() falls over if 0 bytes are allocated, so
**                      put in this dummy to pad the structure out
** @@
******************************************************************************/

typedef struct EmbSShowTicks {
  AjBool dummy;
} EmbOShowTicks, *EmbPShowTicks;

/* @data EmbPShowTicknum ******************************************************
**
** NUCLEUS data structure for tick number line information, type = SH_TICKNUM
**
** @attr dummy [AjBool] Dummy attribute - no specific information needed
**                      AJNEW0() falls over if 0 bytes are allocated, so
**                      put in this dummy to pad the structure out
** @@
******************************************************************************/

typedef struct EmbSShowTicknum {
  AjBool dummy;
} EmbOShowTicknum, *EmbPShowTicknum;

/* @data EmbPShowTran *********************************************************
**
** NUCLEUS data structure for translation information, type = SH_TRAN
**
** @attr transeq [AjPSeq] our stored translation
** @attr trnTable [AjPTrn] translation table
** @attr frame [ajint] 1,2,3,-1,-2 or -3 = frame to translate
** @attr threeletter [AjBool] ajTrue = display in three letter code
** @attr number [AjBool] ajTrue = number the translation
** @attr tranpos [ajint] store of translation position for numbering
** @attr regions [AjPRange] only translate in these regions, NULL = do all
** @attr orfminsize [ajint] minimum size of ORF to display
** @attr lcinterorf [AjBool] ajTrue = put the inter-orf regions in lower case
** @attr firstorf [AjBool] ajTrue = beginning of the seq is a possible ORF
** @attr lastorf [AjBool] ajTrue = end of the seq is a possible ORF
** @attr showframe [AjBool] ajTrue = write the frame number
** @@
******************************************************************************/

typedef struct EmbSShowTran {
  AjPSeq transeq;
  AjPTrn trnTable;
  ajint frame;
  AjBool threeletter;
  AjBool number;
  ajint tranpos;
  AjPRange regions;
  ajint orfminsize;
  AjBool lcinterorf;
  AjBool firstorf;
  AjBool lastorf;
  AjBool showframe;
} EmbOShowTran, *EmbPShowTran;

/* @data EmbPShowComp *********************************************************
**
** NUCLEUS data structure for sequence complement information, type = SH_COMP
**
** @attr number [AjBool] ajTrue = number the complement
** @@
******************************************************************************/

typedef struct EmbSShowComp {
  AjBool number;
} EmbOShowComp, *EmbPShowComp;

/* @data EmbPShowRE ***********************************************************
**
** NUCLEUS data structure for RE cut site information, type = SH_RE
**
** @attr sense [ajint]  1 or -1 = sense to display
** @attr flat [AjBool] ajTrue = display in flat format with recognition sites
** @attr matches [AjPList] list of AjPMatmatch matches
** @attr hits [ajint]  number of hits in list
** @attr sitelist [AjPList] list of EmbSShowREsite
** @@
******************************************************************************/

typedef struct EmbSShowRE {
  ajint sense;
  AjBool flat;
  AjPList matches;
  ajint hits;
  AjPList sitelist;
} EmbOShowRE, *EmbPShowRE;

/* @data EmbPShowFT ***********************************************************
**
** NUCLEUS data structure for  Feature information, type = SH_FT
**
** @attr feat [AjPFeattable] Feature table
** @@
******************************************************************************/

typedef struct EmbSShowFT {
  AjPFeattable feat;
} EmbOShowFT, *EmbPShowFT;

/* @data EmbPShowNote *********************************************************
**
** NUCLEUS data structure for annotation information, type = SH_NOTE
**
** @attr regions [AjPRange] regions to annotate, NULL = no regions
** @@
******************************************************************************/

typedef struct EmbSShowNote {
  AjPRange regions;
} EmbOShowNote, *EmbPShowNote;


/********* assorted structures ***********/

/* @data EmbPShowREsite *******************************************************
**
** NUCLEUS data structure for Restriction Enzyme cut site position list node
**
** @attr pos [ajint] cut site position
** @attr name [AjPStr] name of Restriction Enzyme
** @@
******************************************************************************/

typedef struct EmbSShowREsite {
  ajint pos;
  AjPStr name;
} EmbOShowREsite, *EmbPShowREsite;


/* declare functions **********************************************/
EmbPShow embShowNew (AjPSeq seq, ajint begin, ajint end, ajint width,
		     ajint length, ajint margin, AjBool html, ajint offset);
void     embShowDel (EmbPShow* pthis);

void     embShowAddSeq (EmbPShow thys, AjBool number, AjBool threeletter,
			AjPRange upperrange, AjPRange colour);
void     embShowAddBlank (EmbPShow thys);
void     embShowAddTicks (EmbPShow thys);
void     embShowAddTicknum (EmbPShow thys);
void     embShowAddComp (EmbPShow thys, AjBool number);
void     embShowAddTran (EmbPShow thys, AjPTrn trnTable, ajint frame,
			 AjBool threeletter, AjBool number, AjPRange regions,
			 ajint orfminsize, AjBool lcinterorf,
			 AjBool firstorf, AjBool lastorf, AjBool showframe);
void     embShowAddRE (EmbPShow thys, ajint sense, AjPList restrictlist,
		       AjBool flat);
void     embShowAddFT (EmbPShow thys, AjPFeattable feat);
void     embShowAddNote (EmbPShow thys, AjPRange regions);
void     embShowPrint (AjPFile out, EmbPShow thys);
void     embShowUpperRange (AjPStr *line, AjPRange upperrange, ajint pos);
void     embShowColourRange (AjPStr *line, AjPRange colour, ajint pos);

#endif

#ifdef __cplusplus
}
#endif


