#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensslice_h
#define ensslice_h

#include "ensassemblyexception.h"
#include "ensassemblymapper.h"
#include "enscache.h"
#include "ensmapper.h"
#include "ensseqregion.h"
#include "enstable.h"




/* @data EnsPRepeatMaskedSlice ************************************************
**
** Ensembl Repeat Masked Slice.
**
** Holds information about a genome sequence slice.
**
** @alias EnsSRepeatMaskedSlice
** @alias EnsORepeatMaskedSlice
**
** @attr Slice [EnsPSlice] Ensembl Slice.
** @attr AnalysisNames [AjPList] AJAX List of Ensembl Analysis name
**                               AJAX Strings.
** @attr Masking [AjPTable] AJAX Table of Repeat Consensus types, classes or
**                          names and sequence masking types.
** @attr Use [ajuint] Use counter.
** @attr Padding [ajuint] Padding to alignment boundary.
** @@
******************************************************************************/

typedef struct EnsSRepeatMaskedSlice
{
    EnsPSlice Slice;
    AjPList AnalysisNames;
    AjPTable Masking;
    ajuint Use;
    ajuint Padding;
} EnsORepeatMaskedSlice;

#define EnsPRepeatMaskedSlice EnsORepeatMaskedSlice*




#include "ensprojectionsegment.h"




/*
** Prototype definitions
*/

/* Ensembl Slice */

EnsPSlice ensSliceNewS(EnsPSliceadaptor adaptor,
                       EnsPSeqRegion sr,
                       ajint start,
                       ajint end,
                       ajint strand,
                       AjPStr sequence);
	     
EnsPSlice ensSliceNew(EnsPSliceadaptor adaptor,
                      EnsPSeqRegion sr,
                      ajint start,
                      ajint end,
                      ajint strand);

EnsPSlice ensSliceNewObj(EnsPSlice object);

EnsPSlice ensSliceNewRef(EnsPSlice slice);

void ensSliceDel(EnsPSlice* Pslice);

EnsPSliceadaptor ensSliceGetadaptor(const EnsPSlice slice);

EnsPSeqRegion ensSliceGetSeqRegion(const EnsPSlice slice);

const AjPStr ensSliceGetSequence(const EnsPSlice slice);

ajint ensSliceGetStart(const EnsPSlice slice);

ajint ensSliceGetEnd(const EnsPSlice slice);

ajint ensSliceGetStrand(const EnsPSlice slice);

AjBool ensSliceSetadaptor(EnsPSlice slice, EnsPSliceadaptor adaptor);

AjBool ensSliceSetSequence(EnsPSlice slice, AjPStr sequence);

AjBool ensSliceTrace(const EnsPSlice slice, ajuint level);

ajuint ensSliceGetSeqRegionIdentifier(const EnsPSlice slice);

const AjPStr ensSliceGetSeqRegionName(const EnsPSlice slice);

ajint ensSliceGetSeqRegionLength(const EnsPSlice slice);

EnsPCoordSystem ensSliceGetCoordSystem(const EnsPSlice slice);

const AjPStr ensSliceGetCoordSystemName(const EnsPSlice slice);

const AjPStr ensSliceGetCoordSystemVersion(const EnsPSlice slice);

ajint ensSliceGetCentrePoint(const EnsPSlice slice);

ajuint ensSliceGetLength(const EnsPSlice slice);

ajuint ensSliceGetMemSize(const EnsPSlice slice);

AjBool ensSliceFetchName(const EnsPSlice slice, AjPStr* Pname);

AjBool ensSliceMatch(const EnsPSlice slice1, const EnsPSlice slice2);

AjBool ensSliceIsTopLevel(EnsPSlice slice);

AjBool ensSliceFetchSequenceSeq(EnsPSlice slice, AjPSeq *Psequence);

AjBool ensSliceFetchSequenceStr(EnsPSlice slice, AjPStr *Psequence);

AjBool ensSliceFetchSubSequenceSeq(EnsPSlice slice,
                                   ajint start,
                                   ajint end,
                                   ajint strand,
                                   AjPSeq *Psequence);

AjBool ensSliceFetchSubSequenceStr(EnsPSlice slice,
                                   ajint start,
                                   ajint end,
                                   ajint strand,
                                   AjPStr *Psequence);

AjBool ensSliceFetchInvertedSlice(EnsPSlice slice, EnsPSlice *Pslice);

AjBool ensSliceFetchSubSlice(EnsPSlice slice,
                             ajint start,
                             ajint end,
                             ajint strand,
                             EnsPSlice *Pslice);

AjBool ensSliceFetchExpandedSlice(const EnsPSlice slice,
                                  ajint five,
                                  ajint three,
                                  AjBool force,
                                  ajint *Pfive,
                                  ajint *Pthree,
                                  EnsPSlice *Pslice);

AjBool ensSliceProject(EnsPSlice slice,
                       const AjPStr csname,
                       const AjPStr csversion,
                       AjPList pslist);

AjBool ensPSliceProjectToSlice(EnsPSlice srcslice,
                               EnsPSlice trgslice,
                               AjPList pslist);
			
AjBool ensSliceFetchAllAttributes(EnsPSlice slice,
                                  const AjPStr code,
                                  AjPList attributes);

AjBool ensSliceFetchAllRepeatFeatures(EnsPSlice slice,
                                      AjPStr anname,
                                      AjPStr rctype,
                                      AjPStr rcclass,
                                      AjPStr rcname,
                                      AjPList rfs);

/* Ensembl Slice Adaptor */

EnsPSliceadaptor ensSliceadaptorNew(EnsPDatabaseadaptor dba);

void ensSliceadaptorDel(EnsPSliceadaptor* Padaptor);

EnsPDatabaseadaptor ensSliceadaptorGetDatabaseadaptor(
    const EnsPSliceadaptor adaptor);

AjBool ensSliceadaptorCacheInsert(EnsPSliceadaptor adaptor, EnsPSlice* Pslice);

AjBool ensSliceadaptorFetchBySeqRegionIdentifier(EnsPSliceadaptor adaptor,
                                                 ajuint srid,
                                                 ajint srstart,
                                                 ajint srend,
                                                 ajint srstrand,
                                                 EnsPSlice *Pslice);

AjBool ensSliceadaptorFetchByRegion(EnsPSliceadaptor adaptor,
                                    const AjPStr csname,
                                    const AjPStr csversion,
                                    const AjPStr srname,
                                    ajint srstart,
                                    ajint srend,
                                    ajint srstrand,
                                    EnsPSlice *Pslice);

AjBool ensSliceadaptorFetchByName(EnsPSliceadaptor adaptor,
                                  const AjPStr name,
                                  EnsPSlice *Pslice);

AjBool ensSliceadaptorFetchBySlice(EnsPSliceadaptor adaptor,
                                   EnsPSlice slice,
                                   ajint start,
                                   ajint end,
                                   ajint strand,
                                   EnsPSlice *Pslice);

AjBool ensSliceadaptorFetchByFeature(EnsPSliceadaptor adaptor,
                                     const EnsPFeature feature,
                                     ajint flank,
                                     EnsPSlice *Pslice);

AjBool ensSliceadaptorFetchNormalisedSliceProjection(EnsPSliceadaptor adaptor,
                                                     EnsPSlice slice,
                                                     AjPList pslist);

AjBool ensSliceadaptorFetchAll(EnsPSliceadaptor adaptor,
                               const AjPStr csname,
                               const AjPStr csversion,
                               AjBool nonref,
                               AjBool duplicates,
                               AjPList slices);

/* Ensembl Repeat Masked Slice */

EnsPRepeatMaskedSlice ensRepeatMaskedSliceNew(EnsPSlice slice, AjPList annames,
                                              AjPTable masking);

EnsPRepeatMaskedSlice ensRepeatMaskedSliceNewObj(EnsPRepeatMaskedSlice object);

EnsPRepeatMaskedSlice ensRepeatMaskedSliceNewRef(EnsPRepeatMaskedSlice rmslice);

void ensRepeatMaskedSliceDel(EnsPRepeatMaskedSlice* Prmslice);

AjBool ensRepeatMaskedSliceFetchSequenceSeq(EnsPRepeatMaskedSlice rmslice,
                                            AjEnum mtype,
                                            AjPSeq *Psequence);

AjBool ensRepeatMaskedSliceFetchSequenceStr(EnsPRepeatMaskedSlice rmslice,
                                            AjEnum mtype,
                                            AjPStr *Psequence);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
