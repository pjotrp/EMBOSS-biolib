#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajseqalign_h
#define ajseqalign_h

/* @data AjPAlign *******************************************************
**
** Ajax Align Output object.
**
** Holds definition of feature align output.
**
** @new ajAlignNew Default constructor
** @delete ajAlignDel Default destructor
** @set ajAlignClear Resets ready for reuse.
** @use ajAlignWrite Master sequence output routine
** @use ajAlignNewOut Opens an output file for sequence writing.
** @other AjPSeqout Sequence output
** @other AjPFile Input and output files
** @@
******************************************************************************/

typedef struct AjSAlign {
  AjPStr Name;
  AjPStr Type;
  AjPStr Formatstr;
  AjEnum Format;
  AjPStr Filename;
  AjPStr Extension;
  AjPFile File;
  AjPStr Header;
  AjBool Showusa;
  AjBool Multi;
  AjPList Data;
  ajint Nseqs;
  ajint Nmin;
  ajint Nmax;
  ajint Width;
  ajint Count;
  AjPStr Matrix;
  AjPStr GapPen;
  AjPStr ExtPen;
} AjOAlign, *AjPAlign;

AjBool       ajAlignDefine (AjPAlign pthys, AjPSeqset seqset);
void         ajAlignDefineCC (AjPAlign pthys, AjPSeqset seqset,
			      char* seqa, char* seqb);
void         ajAlignDefineSS (AjPAlign pthys, AjPSeqset seqset,
			      AjPSeq seqa, AjPSeq seqb);
void         ajAlignDel (AjPAlign* pthys);
AjBool       ajAlignOpen (AjPAlign thys, AjPStr name);
AjBool       ajAlignFindFormat (AjPStr format, ajint* iformat);
AjBool       ajAlignFormatDefault (AjPStr* pformat);
AjPAlign     ajAlignNew (void);
void         ajAlignSetHeader (AjPAlign thys, AjPStr header);
void         ajAlignSetHeaderC (AjPAlign thys, char* header);
void         ajAlignSetGapI (AjPAlign thys, ajint gappen, ajint extpen);
void         ajAlignSetGapR (AjPAlign thys, float gappen, float extpen);
void         ajAlignSetMatrix (AjPAlign thys, AjPStr matrix);
void         ajAlignSetMatrixC (AjPAlign thys, const char* matrix);
void         ajAlignSetType (AjPAlign thys, AjPSeqset seqset);
void         ajAlignTrace (AjPAlign thys);
AjBool       ajAlignValid (AjPAlign thys);
void         ajAlignWrite (AjPAlign thys, AjPSeqset seqset);
void         ajAlignWriteClose (AjPAlign thys);
void         ajAlignWriteHeader (AjPAlign thys, AjPSeqset seqset);

#endif

#ifdef __cplusplus
}
#endif
