#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enssequence_h
#define enssequence_h

#include "enscache.h"
#include "ensslice.h"




/* @data EnsPSequenceAdaptor **************************************************
**
** Ensembl Sequence Adaptor.
**
** @alias EnsSSequenceAdaptor
** @alias EnsOSequenceAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor.
** @attr Cache [EnsPCache] Ensembl LRU Cache.
** @attr CacheByName [AjPTable] Name cache.
** @@
******************************************************************************/

typedef struct EnsSSequenceAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
    EnsPCache Cache;
} EnsOSequenceAdaptor;

#define EnsPSequenceAdaptor EnsOSequenceAdaptor*




/*
** Prototype definitions
*/

/* Ensembl Sequence Adaptor */

EnsPSequenceAdaptor ensSequenceAdaptorNew(EnsPDatabaseAdaptor dba);

void ensSequenceAdaptorDel(EnsPSequenceAdaptor* Psa);

AjBool ensSequenceAdaptorFetchSubStrBySeqRegion(EnsPSequenceAdaptor sa,
                                                EnsPSeqRegion sr,
                                                ajuint start,
                                                ajuint length,
                                                AjPStr *Psequence);

AjBool ensSequenceAdaptorFetchStrBySeqRegion(EnsPSequenceAdaptor sa,
                                             EnsPSeqRegion sr,
                                             AjPStr *Psequence);

AjBool ensSequenceAdaptorFetchSubSeqBySeqRegion(EnsPSequenceAdaptor sa,
                                                EnsPSeqRegion sr,
                                                ajuint start,
                                                ajuint length,
                                                AjPSeq *Psequence);

AjBool ensSequenceAdaptorFetchSeqBySeqRegion(EnsPSequenceAdaptor sa,
                                             EnsPSeqRegion sr,
                                             AjPSeq *Psequence);

AjBool ensSequenceAdaptorFetchSubStrBySlice(EnsPSequenceAdaptor sa,
                                            EnsPSlice slice,
                                            ajint start,
                                            ajint end,
                                            ajint strand,
                                            AjPStr *Psequence);

AjBool ensSequenceAdaptorFetchStrBySlice(EnsPSequenceAdaptor sa,
                                         EnsPSlice slice,
                                         AjPStr *Psequence);

AjBool ensSequenceAdaptorFetchSubSeqBySlice(EnsPSequenceAdaptor sa,
                                            EnsPSlice slice,
                                            ajint start,
                                            ajint end,
                                            ajint strand,
                                            AjPSeq *Psequence);

AjBool ensSequenceAdaptorFetchSeqBySlice(EnsPSequenceAdaptor sa,
                                         EnsPSlice slice,
                                         AjPSeq *Psequence);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
