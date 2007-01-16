#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajseq_h
#define ajseq_h

#include "ajseqdata.h"
#include "ajax.h"

/* @data AjPSeqCvt ************************************************************
**
** Sequence conversion data. Used to convert a sequence to binary.
**
** @new ajSeqCvtNew Creates from a character string of valid bases.
** @new ajSeqCvtNewText Creates from a character string of valid bases.
** @new ajSeqCvtNewZero Creates from a character string of valid bases.
** @new ajSeqCvtNewZeroS Creates from an array of strings of valid bases.
** @new ajSeqCvtNewZeroSS Creates from an array of strings of valid bases.
** @output ajSeqCvtTrace Reports on contents for debugging
*
** @alias AjSSeqCvt
** @alias AjOSeqCvt
**
** @attr size [ajint] Number of characters in table, usually
**                    all possible characters.
** @attr len [ajint] Number of characters defined
** @attr missing [ajint] Index of the missing character value
** @attr bases [AjPStr] The bases which can be converted
** @attr nrlabels [ajint] Number of row labels
** @attr rlabels [AjPStr*] Row labels 
** @attr nclabels [ajint] Number of column labels
** @attr clabels [AjPStr*] Column labels 
** @attr table [char*] Binary character value for each character in bases
** @@
******************************************************************************/

typedef struct AjSSeqCvt {
  ajint size;
  ajint len;
  ajint missing;
  AjPStr bases;
  ajint nrlabels;
  AjPStr* rlabels;
  ajint nclabels;
  AjPStr* clabels;
  char *table;
} AjOSeqCvt;

#define AjPSeqCvt AjOSeqCvt*


/*
** Prototype definitions
*/

AjBool       ajSeqtestIsAccession (const AjPStr accnum);
const AjPStr ajSeqtestIsSeqversion (const AjPStr sv);

AjBool       __deprecated ajIsAccession (const AjPStr accnum);
const AjPStr __deprecated ajIsSeqversion (const AjPStr sv);

void         ajSeqallDel(AjPSeqall *thys);
const AjPStr ajSeqallGetFilename(const AjPSeqall thys);
const AjPStr ajSeqallGetseqName (const AjPSeqall thys);
ajint        ajSeqallGetseqBegin (const AjPSeqall seq);
ajint        ajSeqallGetseqEnd (const AjPSeqall seq);
ajint        ajSeqallGetseqLen (const AjPSeqall seqall);
const AjPStr ajSeqallGetName(const AjPSeqall seq);
ajint        __deprecated ajSeqallLen (const AjPSeqall seqall);
const AjPStr __deprecated ajSeqallGetNameSeq (const AjPSeqall thys);
ajint        ajSeqallGetseqRange (const AjPSeqall thys, ajint* begin, ajint* end);
const AjPStr ajSeqallGetUsa (const AjPSeqall thys);
AjPSeqall    ajSeqallNew (void);
void         ajSeqallClear (AjPSeqall thys);
AjBool       ajSeqallNext (AjPSeqall seqall, AjPSeq* retseq);
void         ajSeqallSetRange (AjPSeqall seq, ajint ibegin, ajint iend);
void         ajSeqallSetRangeRev (AjPSeqall seq, ajint ibegin, ajint iend);
void         ajSeqAssignAccC(AjPSeq seq, const char* text);
void         ajSeqAssignAccS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignDescC(AjPSeq seq, const char* text);
void         ajSeqAssignDescS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignEntryC(AjPSeq seq, const char* text);
void         ajSeqAssignEntryS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignFileC(AjPSeq seq, const char* text);
void         ajSeqAssignFileS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignFullC(AjPSeq seq, const char* text);
void         ajSeqAssignFullS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignGiC(AjPSeq seq, const char* text);
void         ajSeqAssignGiS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignNameC(AjPSeq seq, const char* txt);
void         ajSeqAssignNameS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignSeqC(AjPSeq seq, const char* text);
void         ajSeqAssignSeqLenC(AjPSeq seq, const char* txt, ajint len);
void         ajSeqAssignSeqS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignSvC(AjPSeq seq, const char* text);
void         ajSeqAssignSvS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignUfoC(AjPSeq seq, const char* text);
void         ajSeqAssignUfoS(AjPSeq seq, const AjPStr str);
void         ajSeqAssignUsaC(AjPSeq seq, const char* text);
void         ajSeqAssignUsaS(AjPSeq seq, const AjPStr str);
ajint        ajSeqGetBegin (const AjPSeq seq);
const char*  ajSeqGetSeqC (const AjPSeq thys);
const AjPStr ajSeqGetSeqS(const AjPSeq seq);
char*        ajSeqGetSeqCopyC (const AjPSeq seq);
AjPStr       ajSeqGetSeqCopyS(const AjPSeq seq);
ajint        ajSeqCalcCheckgcg (const AjPSeq thys);
void         ajSeqClear (AjPSeq thys);
void         ajSeqComplement (AjPSeq thys);
void         __deprecated ajSeqComplementOnly(AjPSeq pthis);
void         ajSeqstrComplement (AjPStr* thys);
void         __deprecated ajSeqstrComplementOnly (AjPStr* thys);
AjBool       ajSeqstrConvertNum(const AjPStr seq, const AjPSeqCvt cvt,
				AjPStr* Pnumseq);
AjPFeattable ajSeqGetFeatCopy (const AjPSeq thys);
void         ajSeqCalcCount (const AjPSeq thys, ajint *b);
ajuint       __deprecated ajSeqCalcCrc(const AjPSeq seq );
ajuint       ajSeqstrCalcCrc(const AjPStr seq );
void         ajSeqcvtDel (AjPSeqCvt* thys);
ajint        ajSeqcvtGetCodeK (const AjPSeqCvt thys, char ch);
ajint        ajSeqcvtGetCodeS (const AjPSeqCvt thys, const AjPStr ch);
ajint        ajSeqcvtGetCodeAsymS (const AjPSeqCvt cvt, const AjPStr str);
ajint        ajSeqcvtGetLen (const AjPSeqCvt thys);
AjPSeqCvt    ajSeqcvtNewC (const char* bases);
AjPSeqCvt    ajSeqcvtNewNumberC (const char* bases);
AjPSeqCvt    ajSeqcvtNewEndC (const char* bases);
AjPSeqCvt    ajSeqcvtNewStr (const AjPPStr bases, ajint n);
AjPSeqCvt    ajSeqcvtNewStrAsym (const AjPPStr bases, ajint n, 
				const AjPPStr rbases, ajint rn);
void         ajSeqcvtTrace (const AjPSeqCvt cvt);
void         __deprecated ajSeqCvtDel (AjPSeqCvt* thys);
ajint        __deprecated ajSeqCvtK (const AjPSeqCvt thys, char ch);
ajint        __deprecated ajSeqCvtKS (const AjPSeqCvt thys, const AjPStr ch);
ajint        __deprecated ajSeqCvtKSRow (const AjPSeqCvt thys,
					 const AjPStr ch);
ajint        __deprecated ajSeqCvtKSColumn (const AjPSeqCvt thys,
					    const AjPStr ch);
ajint        __deprecated ajSeqCvtLen (const AjPSeqCvt thys);
AjPSeqCvt    __deprecated ajSeqCvtNew (const char* bases);
AjPSeqCvt    __deprecated ajSeqCvtNewText (const char* bases);
AjPSeqCvt    __deprecated ajSeqCvtNewZero (const char* bases);
AjPSeqCvt    __deprecated ajSeqCvtNewZeroS (const AjPPStr bases, ajint n);
AjPSeqCvt    __deprecated ajSeqCvtNewZeroSS (const AjPPStr bases, ajint n, 
				const AjPPStr rbases, ajint rn);
void         __deprecated ajSeqCvtTrace (const AjPSeqCvt cvt);
void         ajSeqSetName(AjPSeq thys, const AjPStr setname);
void         ajSeqSetNameMulti(AjPSeq thys, const AjPStr setname);
void         __deprecated ajSeqDefName(AjPSeq thys, const AjPStr setname,
				       AjBool multi);
void         ajSeqDel (AjPSeq* pthis);
ajint        ajSeqGetEnd (const AjPSeq seq);
void         ajSeqExit(void);
void         ajSeqGapFill (AjPSeq seq, ajint len);
ajint        __deprecated ajSeqFill (AjPSeq seq, ajint len);
ajint        ajSeqCountGaps(const AjPSeq seq);
ajint        __deprecated ajSeqGapCount (const AjPSeq thys);
ajint        __deprecated ajSeqGapCountS (const AjPStr str);
void         ajSeqGapStandard (AjPSeq thys, char gapch);
void         __deprecated ajSeqGapStandardS(AjPStr thys, char gapch);
const char*  ajSeqGetAccC (const AjPSeq thys);
const AjPStr ajSeqGetAccS (const AjPSeq thys);
const char*  ajSeqGetDescC (const AjPSeq thys);
const AjPStr ajSeqGetDescS (const AjPSeq thys);
const char*  ajSeqGetEntryC (const AjPSeq thys);
const AjPStr ajSeqGetEntryS (const AjPSeq thys);
const AjPFeattable ajSeqGetFeat (const AjPSeq thys);
const char*  ajSeqGetGiC (const AjPSeq thys);
const AjPStr ajSeqGetGiS (const AjPSeq thys);
const char*  ajSeqGetNameC (const AjPSeq thys);
const AjPStr ajSeqGetNameS (const AjPSeq thys);
ajint        ajSeqGetOffend(const AjPSeq seq);
ajint        ajSeqGetOffset(const AjPSeq seq);
ajint        ajSeqGetRange (const AjPSeq thys, ajint* begin, ajint* end);
AjBool       ajSeqGetRev (const AjPSeq thys);
const char*  ajSeqGetSvC (const AjPSeq thys);
const AjPStr ajSeqGetSvS (const AjPSeq thys);
const char*  ajSeqGetTaxC (const AjPSeq thys);
const AjPStr ajSeqGetTaxS (const AjPSeq thys);
const char*  ajSeqGetUsaC (const AjPSeq thys);
const AjPStr ajSeqGetUsaS (const AjPSeq thys);
AjBool       ajSeqIsNuc (const AjPSeq thys);
AjBool       ajSeqIsProt (const AjPSeq thys);
ajint        ajSeqGetLen (const AjPSeq seq);
void         ajSeqSetUnique(AjPSeq seq);
void         __deprecated ajSeqMod (AjPSeq thys);
float        ajSeqCalcMolwt (const AjPSeq seq);
float        ajSeqstrCalcMolwt (const AjPStr seq);
AjPSeq       ajSeqNew (void);
AjPSeq       ajSeqNewNameC(const char* txt, const char* name);
AjPSeq       ajSeqNewNameS(const AjPStr str, const AjPStr name);
AjPSeq       ajSeqNewRes (size_t size);
AjPSeq       ajSeqNewSeq (const AjPSeq seq);
AjPSeq       ajSeqNewRangeC(const char* txt,
			   ajint offset, ajint offend, AjBool rev);
AjPSeq       ajSeqNewRangeS(const AjPStr str,
			   ajint offset, ajint offend, AjBool rev);
AjBool       ajSeqConvertNum (const AjPSeq thys, const AjPSeqCvt cvt,
		       AjPStr *numseq);
AjBool       __deprecated ajSeqNum (const AjPSeq thys, const AjPSeqCvt cvt,
		       AjPStr *numseq);
AjBool       __deprecated ajSeqNumS (const AjPStr thys, const AjPSeqCvt cvt,
			AjPStr *numseq);
AjBool       ajSeqIsGarbage(const AjPSeq thys);
AjBool       ajSeqIsReversedTrue(const AjPSeq seq);
AjBool       ajSeqIsReversed (const AjPSeq thys);
AjBool       ajSeqIsTrimmed (const AjPSeq thys);
void         ajSeqReverseDo (AjPSeq thys);
void         ajSeqReverseForce (AjPSeq thys);
void         ajSeqstrReverse (AjPStr* thys);
void         ajSeqReverseOnly (AjPSeq thys);
ajint        ajSeqsetGetBegin (const AjPSeqset seq);
ajint        __deprecated ajSeqsetBegin (const AjPSeqset seq);
void         ajSeqsetDel(AjPSeqset *thys);
void         ajSeqsetDelarray(AjPSeqset **thys);
ajint        ajSeqsetGetEnd (const AjPSeqset seq);
ajint        __deprecated ajSeqsetEnd (const AjPSeqset seq);
ajint        ajSeqsetFill (AjPSeqset seq);
const AjPStr ajSeqsetGetFormat (const AjPSeqset thys);
const char*  ajSeqsetGetNameC (const AjPSeqset thys);
const AjPStr ajSeqsetGetNameS (const AjPSeqset thys);
const AjPStr __deprecated ajSeqsetGetName (const AjPSeqset thys);
ajint        ajSeqsetGetOffend(const AjPSeqset seq);
ajint        ajSeqsetGetOffset(const AjPSeqset seq);
ajint        ajSeqsetGetRange (const AjPSeqset thys, ajint* begin, ajint* end);
const AjPSeq ajSeqsetGetseqSeq (const AjPSeqset thys, ajint i);
const char*  ajSeqsetGetseqSeqC (const AjPSeqset thys, ajint i);
const AjPStr ajSeqsetGetseqSeqS (const AjPSeqset thys, ajint i);
const AjPSeq __deprecated ajSeqsetGetSeq (const AjPSeqset thys, ajint i);
AjPSeq*      __deprecated ajSeqsetGetSeqArray(const AjPSeqset thys);
AjPSeq*      ajSeqsetGetSeqarray(const AjPSeqset thys);
const AjPStr ajSeqsetGetUsa (const AjPSeqset thys);
const AjPStr ajSeqsetGetFilename(const AjPSeqset thys);
AjBool       ajSeqsetIsDna (const AjPSeqset thys);
AjBool       ajSeqsetIsNuc (const AjPSeqset thys);
AjBool       ajSeqsetIsProt (const AjPSeqset thys);
AjBool       ajSeqsetIsRna (const AjPSeqset thys);
ajint        ajSeqsetGetLen (const AjPSeqset seq);
ajint        __deprecated ajSeqsetLen (const AjPSeqset seq);
const char*  ajSeqsetGetseqAccC (const AjPSeqset seq, ajint i);
const AjPStr ajSeqsetGetseqAccS (const AjPSeqset seq, ajint i);
const char*  ajSeqsetGetseqNameC (const AjPSeqset seq, ajint i);
const AjPStr ajSeqsetGetseqNameS (const AjPSeqset seq, ajint i);
const AjPStr __deprecated ajSeqsetName (const AjPSeqset seq, ajint i);
const AjPStr __deprecated ajSeqsetAcc (const AjPSeqset seq, ajint i);
AjPSeqset    ajSeqsetNew (void);
void         ajSeqSetOffsets(AjPSeq seq, ajint ioff, ajint ioriglen);
void         ajSeqSetRange (AjPSeq seq, ajint ibegin, ajint iend);
void         ajSeqSetRangeRev (AjPSeq seq, ajint ibegin, ajint iend);
void         ajSeqsetReverse (AjPSeqset thys);
const char*  __deprecated ajSeqsetSeq (const AjPSeqset seq, ajint i);
void         ajSeqsetSetRange (AjPSeqset seq, ajint ibegin, ajint iend);
ajint        ajSeqsetGetSize (const AjPSeqset seq);
ajint        __deprecated ajSeqsetSize (const AjPSeqset seq);
void         ajSeqsetFmtLower (AjPSeqset seq);
void         ajSeqsetFmtUpper (AjPSeqset seq);
float        ajSeqsetGetTotweight (const AjPSeqset seq);
float        __deprecated ajSeqsetTotweight (const AjPSeqset seq);
void         ajSeqsetTrim(AjPSeqset thys);
float        ajSeqsetGetseqWeight (const AjPSeqset seq, ajint i) ;
float        __deprecated ajSeqsetWeight (const AjPSeqset seq, ajint i) ;
void         ajSeqFmtLower (AjPSeq thys);
void         ajSeqFmtUpper (AjPSeq thys);
void         ajSeqTrace (const AjPSeq seq);
void         ajSeqTraceTitle (const AjPSeq seq, const char* title);
void         __deprecated ajSeqTraceT (const AjPSeq seq, const char* title);
AjBool       ajSeqFindOutFormat (const AjPStr format, ajint* iformat);
void         ajSeqTrim(AjPSeq thys);
ajint        ajSeqGetBeginTrue (const AjPSeq seq);
ajint        ajSeqGetEndTrue (const AjPSeq seq);
ajint        ajSeqGetLenTrue (const AjPSeq seq);
ajint        ajSeqCalcTruepos (const AjPSeq thys, ajint ipos);
ajint        ajSeqCalcTrueposMin (const AjPSeq thys, ajint imin, ajint ipos);


ajint __deprecated ajSeqallBegin(const AjPSeqall seq);
ajint __deprecated ajSeqallEnd(const AjPSeqall seq);
ajint __deprecated ajSeqallGetRange(const AjPSeqall seq,
				    ajint* begin, ajint* end);
void __deprecated ajSeqsetToLower(AjPSeqset seqset);
void __deprecated ajSeqsetToUpper(AjPSeqset seqset);

AjPSeq       __deprecated ajSeqNewC (const char* seq, const char* name);
AjPSeq       __deprecated ajSeqNewStr (const AjPStr str);
AjPSeq       __deprecated ajSeqNewRange(const AjPStr seq,
					ajint offset, ajint offend,
					AjBool rev);
AjPSeq       __deprecated ajSeqNewRangeCI(const char* seq, ajint len,
					  ajint offset, ajint offend,
					  AjBool rev);
AjPSeq       __deprecated ajSeqNewL (size_t size);
AjPSeq       __deprecated ajSeqNewS  (const AjPSeq seq);

void         __deprecated ajSeqAssAcc (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssAccC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssDesc (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssDescC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssEntry (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssEntryC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssFile (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssFileC(AjPSeq thys, const char* text);
void         __deprecated ajSeqAssFull (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssFullC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssGi (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssGiC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssName (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssNameC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssSeq(AjPSeq seq, const AjPStr str);
void         __deprecated ajSeqAssSeqC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssSeqCI (AjPSeq thys, const char* text,
					 ajint ilen);
void         __deprecated ajSeqAssSvC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssSv (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssUfo (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssUfoC (AjPSeq thys, const char* text);
void         __deprecated ajSeqAssUsa (AjPSeq thys, const AjPStr str);
void         __deprecated ajSeqAssUsaC (AjPSeq thys, const char* text);

void         __deprecated ajSeqSetRangeDir (AjPSeq seq,
					    ajint ibegin, ajint iend,
					    AjBool rev);
void         __deprecated ajSeqReplace (AjPSeq thys, const AjPStr seq);
void         __deprecated ajSeqReplaceC (AjPSeq thys, const char* seq);
void         __deprecated ajSeqMakeUsa (AjPSeq thys, const AjPSeqin seqin);
void         __deprecated ajSeqMakeUsaS(const AjPSeq thys,
					const AjPSeqin seqin, AjPStr* usa);
void         __deprecated ajSeqCompOnly (AjPSeq thys);
void         __deprecated ajSeqToLower (AjPSeq thys);
void         __deprecated ajSeqToUpper (AjPSeq thys);
void         __deprecated ajSeqRevOnly (AjPSeq thys);
AjBool       __deprecated ajSeqReverse (AjPSeq thys);

const AjPStr __deprecated ajSeqGetAcc (const AjPSeq thys);
ajint        __deprecated ajSeqBegin (const AjPSeq seq);
ajint        __deprecated ajSeqTrueBegin (const AjPSeq seq);
const AjPStr __deprecated ajSeqGetDesc (const AjPSeq thys);
ajint        __deprecated ajSeqEnd (const AjPSeq seq);
ajint        __deprecated ajSeqTrueEnd (const AjPSeq seq);
const AjPStr __deprecated ajSeqGetEntry (const AjPSeq thys);
AjPFeattable __deprecated ajSeqCopyFeat (const AjPSeq thys);
const AjPStr __deprecated ajSeqGetGi (const AjPSeq thys);
ajint        __deprecated ajSeqLen (const AjPSeq seq);
ajint        __deprecated ajSeqTrueLen (const AjPSeq seq);
const char*  __deprecated ajSeqName (const AjPSeq seq);
const AjPStr __deprecated ajSeqGetName (const AjPSeq thys);
ajint        __deprecated ajSeqOffend (const AjPSeq seq);
ajint        __deprecated ajSeqOffset (const AjPSeq seq);
AjBool       __deprecated ajSeqGetReverse (const AjPSeq thys);
AjBool       __deprecated ajSeqGetReversed (const AjPSeq thys);

const AjPStr __deprecated ajSeqStr (const AjPSeq thys);
const char*  __deprecated ajSeqChar (const AjPSeq thys);
AjPStr       __deprecated ajSeqStrCopy (const AjPSeq thys);
char*        __deprecated ajSeqCharCopy (const AjPSeq seq);
char*        __deprecated ajSeqCharCopyL (const AjPSeq seq, size_t size);
const AjPStr __deprecated ajSeqGetSv (const AjPSeq thys);
const AjPStr __deprecated ajSeqGetTax (const AjPSeq thys);
const AjPStr __deprecated ajSeqGetUsa (const AjPSeq thys);

AjBool       __deprecated ajSeqRev (const AjPSeq thys);
ajint        __deprecated ajSeqCheckGcg (const AjPSeq thys);
void         __deprecated ajSeqCount (const AjPSeq thys, ajint *b);

ajint        __deprecated ajSeqPos (const AjPSeq thys, ajint ipos);
ajint        __deprecated ajSeqPosI (const AjPSeq thys,
				     ajint imin, ajint ipos);
ajint        __deprecated ajSeqPosII (ajint ilen, ajint imin, ajint ipos);

ajint        __deprecated ajSeqTruePos (const AjPSeq thys, ajint ipos);
ajint        __deprecated ajSeqTruePosI (const AjPSeq thys, ajint imin,
					 ajint ipos);
ajint        __deprecated ajSeqTruePosII (ajint ilen, ajint imin, ajint ipos);

void         __deprecated ajSeqallReverse (AjPSeqall thys);
void         __deprecated ajSeqallToLower (AjPSeqall seqall);
void         __deprecated ajSeqallToUpper (AjPSeqall seqall);
void         __deprecated ajSeqReverseStr (AjPStr* thys);
void         __deprecated ajSeqCompOnlyStr (AjPStr* thys);

float        __deprecated ajSeqMW (const AjPStr seq);
ajuint       __deprecated ajSeqCrc(const AjPStr seq );

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
