#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enssequence_h
#define enssequence_h

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "enscache.h"
#include "ensslice.h"




/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ========================== public data ============================= */
/* ==================================================================== */

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




/* ==================================================================== */
/* ======================= public functions =========================== */
/* ==================================================================== */

/*
** Prototype definitions
*/

/* Ensembl Sequence Adaptor */

EnsPSequenceadaptor ensRegistryGetSequenceadaptor(
    EnsPDatabaseadaptor dba);

EnsPSequenceadaptor ensSequenceadaptorNew(
    EnsPDatabaseadaptor dba);

void ensSequenceadaptorDel(EnsPSequenceadaptor* Psa);

AjBool ensSequenceadaptorFetchSubStrBySeqregion(const EnsPSequenceadaptor sa,
                                                EnsPSeqregion sr,
                                                ajuint start,
                                                ajuint length,
                                                AjPStr *Psequence);

AjBool ensSequenceadaptorFetchStrBySeqregion(const EnsPSequenceadaptor sa,
                                             EnsPSeqregion sr,
                                             AjPStr *Psequence);

AjBool ensSequenceadaptorFetchSubSeqBySeqregion(const EnsPSequenceadaptor sa,
                                                EnsPSeqregion sr,
                                                ajuint start,
                                                ajuint length,
                                                AjPSeq *Psequence);

AjBool ensSequenceadaptorFetchSeqBySeqregion(const EnsPSequenceadaptor sa,
                                             EnsPSeqregion sr,
                                             AjPSeq *Psequence);

AjBool ensSequenceadaptorFetchSubStrBySlice(const EnsPSequenceadaptor sa,
                                            EnsPSlice slice,
                                            ajint start,
                                            ajint end,
                                            ajint strand,
                                            AjPStr *Psequence);

AjBool ensSequenceadaptorFetchStrBySlice(const EnsPSequenceadaptor sa,
                                         EnsPSlice slice,
                                         AjPStr *Psequence);

AjBool ensSequenceadaptorFetchSubSeqBySlice(const EnsPSequenceadaptor sa,
                                            EnsPSlice slice,
                                            ajint start,
                                            ajint end,
                                            ajint strand,
                                            AjPSeq *Psequence);

AjBool ensSequenceadaptorFetchSeqBySlice(const EnsPSequenceadaptor sa,
                                         EnsPSlice slice,
                                         AjPSeq *Psequence);

/*
** End of prototype definitions
*/




#endif /* enssequence_h */

#ifdef __cplusplus
}
#endif
