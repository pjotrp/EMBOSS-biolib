#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajseqalign_h
#define ajseqalign_h

/* @data AjPAlign *************************************************************
**
** Ajax Align Output object.
**
** Holds definition of feature align output.
**
** @new ajAlignNew Default constructor
** @delete ajAlignDel Default destructor
** @set ajAlignReset Resets ready for reuse.
** @use ajAlignWrite Master alignment output routine
** @use ajAlignWriteHeader Master header output routine
** @use ajAlignWriteTail Master footer output routine
** @other AjPSeqout Sequence output
** @other AjPFeatout Feature output
** @other AjPReport Report output
** @other AjPFile Input and output files
**
** @attr Name [AjPStr] As "Source" for features, usually empty
** @attr Usa [AjPStr] Name built from input USA, usually empty
** @attr Type [AjPStr] "P" Protein or "N" Nucleotide
** @attr Formatstr [AjPStr] Report format (-aformat qualifier)
** @attr Format [AjEnum] Report format (index number)
** @attr Extension [AjPStr] Output file extension
** @attr File [AjPFile] Output file object
** @attr Header [AjPStr] Text to add to header with newlines
** @attr SubHeader [AjPStr] Text to add to align subheader with newlines
** @attr Tail [AjPStr] Text to add to tail with newlines
** @attr Showacc [AjBool] Report accession number if ajTrue
** @attr Showdes [AjBool] Report sequence description if ajTrue
** @attr Showusa [AjBool] Report full USA (-ausaqualifier) if ajTrue
**                        or only seqname if ajFalse
** @attr Multi [AjBool] if true, assume >1 alignment
** @attr Global [AjBool] if true, show full sequence beyond match
** @attr Data [AjPList] Alignment specific data - see ajalign.c
** @attr Nseqs [ajint] Number of sequences in all alignments
** @attr Nmin [ajint] Minimum number of sequences e.g. 2
** @attr Nmax [ajint] Maximum number of sequences e.g. 2
** @attr Width [ajint] Output width (minimum 10)
** @attr Count [ajint] Use count
** @attr IMatrix [AjPMatrix] Integer matrix (see also FMatrix)
** @attr FMatrix [AjPMatrixf] Floating Pt matrix (see also IMatrix)
** @attr Matrix [AjPStr] Matrix name
** @attr GapPen [AjPStr] Gap penalty (converted to string)
** @attr ExtPen [AjPStr] Gap extend penalty (converted to string)
** @attr SeqOnly [AjBool] Sequence output only, no head or tail
** @attr SeqExternal [AjBool] Sequence is non-local, do not delete
** @@
******************************************************************************/

typedef struct AjSAlign {
  AjPStr Name;
  AjPStr Usa;
  AjPStr Type;
  AjPStr Formatstr;
  AjEnum Format;
  AjPStr Extension;
  AjPFile File;
  AjPStr Header;
  AjPStr SubHeader;
  AjPStr Tail;
  AjBool Showacc;
  AjBool Showdes;
  AjBool Showusa;
  AjBool Multi;
  AjBool Global;
  AjPList Data;
  ajint Nseqs;
  ajint Nmin;
  ajint Nmax;
  ajint Width;
  ajint Count;
  AjPMatrix  IMatrix;
  AjPMatrixf FMatrix;
  AjPStr Matrix;
  AjPStr GapPen;
  AjPStr ExtPen;
  AjBool SeqOnly;
  AjBool SeqExternal;
} AjOAlign;

#define AjPAlign AjOAlign*

void         ajAlignClose (AjPAlign thys);
AjBool       ajAlignDefine (AjPAlign pthys, AjPSeqset seqset);
AjBool       ajAlignDefineSS (AjPAlign pthys,
			      AjPSeq seqa, AjPSeq seqb);
AjBool       ajAlignDefineCC (AjPAlign pthys,
			      const char* seqa, const char* seqb,
			      const char* namea,const  char* nameb);
void         ajAlignDel (AjPAlign* pthys);
AjBool       ajAlignOpen (AjPAlign thys, const AjPStr name);
AjBool       ajAlignFindFormat (const AjPStr format, ajint* iformat);
AjBool       ajAlignFormatDefault (AjPStr* pformat);
AjPAlign     ajAlignNew (void);
void         ajAlignPrintFormat (const AjPFile outf, AjBool full);
void         ajAlignReset (AjPAlign thys);
void         ajAlignSetExternal (AjPAlign thys, AjBool external);
void         ajAlignSetHeader (AjPAlign thys, const AjPStr header);
void         ajAlignSetHeaderApp (AjPAlign thys, const AjPStr header);
void         ajAlignSetHeaderC (AjPAlign thys, const char* header);
void         ajAlignSetGapI (AjPAlign thys, ajint gappen, ajint extpen);
void         ajAlignSetGapR (AjPAlign thys, float gappen, float extpen);
void         ajAlignSetMatrixName (AjPAlign thys, const AjPStr matrix);
void         ajAlignSetMatrixNameC (AjPAlign thys, const char* matrix);
void         ajAlignSetMatrixInt (AjPAlign thys, const AjPMatrix matrix);
void         ajAlignSetMatrixFloat (AjPAlign thys, const AjPMatrixf matrix);
AjBool       ajAlignSetRange (AjPAlign thys,
			      ajint start1, ajint end1,
			      ajint start2, ajint end2);
void         ajAlignSetScoreI (AjPAlign thys, ajint score);
void         ajAlignSetScoreR (AjPAlign thys, float score);
void         ajAlignSetSubHeader (AjPAlign thys, const AjPStr subheader);
void         ajAlignSetSubHeaderApp (AjPAlign thys, const AjPStr subheader);
void         ajAlignSetSubHeaderC (AjPAlign thys, const char* subheader);
void         ajAlignSetSubHeaderPre (AjPAlign thys, const AjPStr subheader);
void         ajAlignSetStats (AjPAlign thys, ajint iali, ajint len,
				    ajint ident, ajint sim, ajint gaps,
				    const AjPStr score);
AjBool       ajAlignSetSubRange (AjPAlign thys, ajint substart1, ajint start1,
				 ajint end1, ajint substart2, ajint start2,
				 ajint end2);
void         ajAlignSetSubStandard (AjPAlign thys, ajint iali);
void         ajAlignSetTail (AjPAlign thys, const AjPStr tail);
void         ajAlignSetTailApp (AjPAlign thys, const AjPStr tail);
void         ajAlignSetTailC (AjPAlign thys, const char* tail);
void         ajAlignSetType (AjPAlign thys);
void         ajAlignTrace (const AjPAlign thys);
AjBool       ajAlignValid (AjPAlign thys);
void         ajAlignWrite (AjPAlign thys);
void         ajAlignWriteClose (AjPAlign thys);
void         ajAlignWriteHeader (AjPAlign thys);
void         ajAlignWriteTail (AjPAlign thys);

#endif

#ifdef __cplusplus
}
#endif
