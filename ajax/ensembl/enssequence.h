#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enssequence_h
#define enssequence_h

#include "enscache.h"
#include "ensslice.h"




/* @data EnsPSequenceadaptor **************************************************
**
** Ensembl Sequence Adaptor.
**
** @alias EnsSSequenceadaptor
** @alias EnsOSequenceadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor.
** @attr Cache [EnsPCache] Ensembl LRU Cache.
** @@
******************************************************************************/

typedef struct EnsSSequenceadaptor
{
    EnsPDatabaseadaptor Adaptor;
    EnsPCache Cache;
} EnsOSequenceadaptor;

#define EnsPSequenceadaptor EnsOSequenceadaptor*




/*
** Prototype definitions
*/

/* Ensembl Sequence Adaptor */

EnsPSequenceadaptor ensSequenceadaptorNew(EnsPDatabaseadaptor dba);

void ensSequenceadaptorDel(EnsPSequenceadaptor* Psa);

AjBool ensSequenceadaptorFetchSubStrBySeqRegion(EnsPSequenceadaptor sa,
                                                EnsPSeqRegion sr,
                                                ajuint start,
                                                ajuint length,
                                                AjPStr *Psequence);

AjBool ensSequenceadaptorFetchStrBySeqRegion(EnsPSequenceadaptor sa,
                                             EnsPSeqRegion sr,
                                             AjPStr *Psequence);

AjBool ensSequenceadaptorFetchSubSeqBySeqRegion(EnsPSequenceadaptor sa,
                                                EnsPSeqRegion sr,
                                                ajuint start,
                                                ajuint length,
                                                AjPSeq *Psequence);

AjBool ensSequenceadaptorFetchSeqBySeqRegion(EnsPSequenceadaptor sa,
                                             EnsPSeqRegion sr,
                                             AjPSeq *Psequence);

AjBool ensSequenceadaptorFetchSubStrBySlice(EnsPSequenceadaptor sa,
                                            EnsPSlice slice,
                                            ajint start,
                                            ajint end,
                                            ajint strand,
                                            AjPStr *Psequence);

AjBool ensSequenceadaptorFetchStrBySlice(EnsPSequenceadaptor sa,
                                         EnsPSlice slice,
                                         AjPStr *Psequence);

AjBool ensSequenceadaptorFetchSubSeqBySlice(EnsPSequenceadaptor sa,
                                            EnsPSlice slice,
                                            ajint start,
                                            ajint end,
                                            ajint strand,
                                            AjPSeq *Psequence);

AjBool ensSequenceadaptorFetchSeqBySlice(EnsPSequenceadaptor sa,
                                         EnsPSlice slice,
                                         AjPSeq *Psequence);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
