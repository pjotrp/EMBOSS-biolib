#ifdef __cplusplus
extern "C"
{
#endif

/*
**
** ajfeat.h - AJAX Sequence Feature include file
**            Version 1.0 - June 1999
**
*/

#ifndef ajfeat_h
#define ajfeat_h

#include <stdlib.h>
#include <stdio.h>

#include "ajdefine.h"
#include "ajexcept.h"
#include "ajmem.h"
#include "ajreg.h"
#include "ajstr.h"
#include "ajfile.h"
#include "ajtime.h"
#include "ajfmt.h"
#include "ajfeatdata.h"
#include "ajseqdata.h"

/* ========================================================================= */
/* ========== All functions in (more or less) alphabetical order =========== */
/* ========================================================================= */

/*
 * Some global functions, not in order
 */

  /* Feature Table Creation */

AjPFeattabIn         ajFeattabInNew (void);
AjPFeattabIn         ajFeattabInNewSS (AjPStr fmt, AjPStr name, char* type);
AjPFeattabIn         ajFeattabInNewSSF (AjPStr fmt, AjPStr name, char* type,
				       AjPFileBuff buff);
AjPFeattabOut        ajFeattabOutNew (void);
AjPFeattabOut        ajFeattabOutNewSSF (AjPStr fmt, AjPStr name, char* type,
					 AjPFile buff);
AjBool               ajFeattabOutOpen (AjPFeattabOut thys, AjPStr ufo);
AjPFeattable         ajFeaturesRead  ( AjPFeattabIn  ftin ) ; 

  /* Feature Creation */

AjPFeature            ajFeatureNew (AjPFeattable thys,
				    AjPStr source, AjPStr type,
				    ajint Start, ajint End,  float score,
				    char strand, ajint frame,
				    AjPStr desc , ajint Start2, ajint End2) ;

  /* Features Delete */

void *               ajFeatClearTag(AjPFeature thys, AjPFeattable table,
				    AjPStr tag) ;
void                 ajFeatDel(AjPFeature *pthis) ;
void                 ajFeatDictDel();
void                 ajFeattabDel (AjPFeattable *pthis) ;
void                 ajFeattabInDel( AjPFeattabIn* pthis);
void                 ajFeattabOutDel( AjPFeattabOut* pthis);

  /* Feature Object Operators */

AjBool               ajFeaturesWrite ( AjPFeattabOut ftout, AjPFeattable ft) ; 

AjBool               ajFeatIsProt (AjPFeattable thys);
ajint                  ajFeatLen (AjPFeattable thys);

void                 ajFeattabAdd (AjPFeattable thys, AjPFeature feature) ;
AjBool               ajFeatRead (AjPFeattable* pthis,
				  AjPFeattabIn tabin, AjPStr Ufo);
AjBool               ajFeatWrite (AjPFeattable thys,
				   AjPFeattabOut tabout, AjPStr Ufo);
void                 ajFeatTraceOld (AjPFeattable thys);
AjPStr               ajFeatGetName (AjPFeattable thys);
ajint                  ajFeatLen (AjPFeattable thys);
ajint                  ajFeatSize (AjPFeattable thys);
void                 ajFeatSortByType(AjPFeattable Feattab);
void                 ajFeatSortByStart(AjPFeattable Feattab);
void                 ajFeatSortByEnd(AjPFeattable Feattab);

void                 ajFeattabInClear (AjPFeattabIn thys);

AjBool               ajFeattableWriteDdbj (AjPFeattable features,
					   AjPFile file);
AjBool               ajFeattableWriteEmbl (AjPFeattable features,
					   AjPFile file);
AjBool               ajFeattableWriteGenbank (AjPFeattable features,
					      AjPFile file);
AjBool               ajFeattableWriteGff (AjPFeattable features,
					  AjPFile file);
AjBool               ajFeattableWriteSwiss (AjPFeattable features,
					    AjPFile file);

  /*
AjPFeatVocabFeat       CheckDictForFeature(AjPFeattable table, AjPStr feature);
AjPFeatVocabTag        CheckDictForTag(AjPFeattable table, AjPStr tag);
void                 ajFeatDickTracy(AjPFeatLexicon dictionary);
AjPFeatLexicon       ajFeattableDict(AjPFeattable thys);
  */

AjBool ajFeatLocToSeq(AjPStr seq, AjPStr line, AjPStr *res, AjPStr usa);
ajint    ajFeatGetLocs(AjPStr str, AjPStr **cds, char *type);
ajint    ajFeatGetTrans(AjPStr str, AjPStr **cds);

void ajFeatTest (void);

#define              ajFeatObjVerify(p,c) ajFeatObjAssert((p), (c), \
      __FILE__, __LINE__)
#define               ajFeatSetScore(p,s) (((AjPFeature)(p))->Score=(s))
#define               ajFeatSource(p)   ((p)->Source)
#define               ajFeatType(p)     ((p)->Type)
#define               ajFeatScore(p)    ((p)->Score)
#define               ajFeattabSetVersion(p,v) ((p)->Version=(v))
#define               ajFeattabSetDate(p,d)    ((p)->Date=(d))
#define               ajFeattabDefFormat(p,f)  ((p)->DefFormat=(f))
#define               ajFeattabFormat(p)       ((p)->Format)
#define               ajFeattabVersion(p)      ((p)->Version)
#define               ajFeattabDate(p)         ((p)->Date)
#define               ajFeattabDictionary(p)   ((p)->Dictionary)
#define               ajFeatVocabReadOnly(p,f) ((p)?(p)->ReadOnly=(f):AjTrue)



AjPFeature   ajFeatAdd (AjPFeattable thys, AjPStr type,
		      ajint start, ajint end, float score,
		      char strand, ajint frame, AjPStr desc);
AjPFeature   ajFeatAddC (AjPFeattable thys, char* type,
		       ajint start, ajint end, float score,
		       char strand, ajint frame, AjPStr desc);
void         ajFeatExit (void);
AjPFeature   ajFeatNew (AjPFeattable thys,
			     AjPStr       source, 
			     AjPStr       type,
			     ajint start, ajint end,
			     float        score,
			     char         strand,
			     ajint          frame, 
			     AjPStr       desc);
AjBool       ajFeatSetDesc (AjPFeature thys, AjPStr desc);
AjPStr       ajFeatTagSet (AjPFeature thys, AjPStr tag, AjPStr value);
AjPStr       ajFeatTagSetC (AjPFeature thys, char* tag, AjPStr value);
AjIList      ajFeatTagIter (AjPFeature thys);
AjBool       ajFeatTagval (AjIList iter, AjPStr* tagnam, AjPStr* tagval);

void         ajFeatTrace (AjPFeature thys);
void         ajFeatTagTrace (AjPFeature thys);
void         ajFeattableTrace (AjPFeattable thys);
AjPFeattable ajFeattableNew( AjPStr name );
AjPFeattable ajFeattableNewDna( AjPStr name );
AjPFeattable ajFeattableNewProt( AjPStr name );
AjPFeattable ajFeattableNewSeq( AjPSeq seq );

#endif /* ajfeat_h */

#ifdef __cplusplus
}
#endif
