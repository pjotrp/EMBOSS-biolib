#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensseqregion_h
#define ensseqregion_h

#include "enscache.h"
#include "enscoordsystem.h"
#include "ensdata.h"




/*
** Prototype definitions
*/

/* Ensembl Sequence Region */

EnsPSeqRegion ensSeqRegionNew(EnsPSeqRegionadaptor adaptor,
                              ajuint identifier,
                              EnsPCoordSystem cs,
                              AjPStr name,
                              ajint length);

EnsPSeqRegion ensSeqRegionNewObj(EnsPSeqRegion object);

EnsPSeqRegion ensSeqRegionNewRef(EnsPSeqRegion sr);

void ensSeqRegionDel(EnsPSeqRegion *Psr);

const EnsPSeqRegionadaptor ensSeqRegionGetadaptor(const EnsPSeqRegion sr);

ajuint ensSeqRegionGetIdentifier(const EnsPSeqRegion sr);

EnsPCoordSystem ensSeqRegionGetCoordSystem(const EnsPSeqRegion sr);

AjPStr ensSeqRegionGetName(const EnsPSeqRegion sr);

ajint ensSeqRegionGetLength(const EnsPSeqRegion sr);

const AjPList ensSeqRegionGetAttributes(EnsPSeqRegion sr);

AjBool ensSeqRegionSetadaptor(EnsPSeqRegion sr, EnsPSeqRegionadaptor adaptor);

AjBool ensSeqRegionSetIdentifier(EnsPSeqRegion sr, ajuint identifier);

AjBool ensSeqRegionSetCoordSystem(EnsPSeqRegion sr, EnsPCoordSystem cs);

AjBool ensSeqRegionSetName(EnsPSeqRegion sr, AjPStr name);

AjBool ensSeqRegionSetLength(EnsPSeqRegion sr, ajint length);

AjBool ensSeqRegionAddAttribute(EnsPSeqRegion sr, EnsPAttribute attribute);

AjBool ensSeqRegionTrace(const EnsPSeqRegion sr, ajuint level);

ajuint ensSeqRegionGetMemSize(const EnsPSeqRegion sr);

AjBool ensSeqRegionMatch(const EnsPSeqRegion sr1, const EnsPSeqRegion sr2);

AjBool ensSeqRegionFetchAllAttributes(EnsPSeqRegion sr,
                                      const AjPStr code,
                                      AjPList attributes);

AjBool ensSeqRegionIsTopLevel(EnsPSeqRegion sr);

/* Ensembl Sequence Region Adaptor */

EnsPSeqRegionadaptor ensSeqRegionadaptorNew(EnsPDatabaseadaptor dba);

void ensSeqRegionadaptorDel(EnsPSeqRegionadaptor *Padaptor);

EnsPDatabaseadaptor ensSeqRegionadaptorGetDatabaseadaptor(
    EnsPSeqRegionadaptor adaptor);

AjBool ensSeqRegionadaptorCacheInsert(EnsPSeqRegionadaptor adaptor,
                                      EnsPSeqRegion *Psr);

AjBool ensSeqRegionadaptorCacheRemove(EnsPSeqRegionadaptor adaptor,
                                      const EnsPSeqRegion sr);

AjBool ensSeqRegionadaptorFetchByIdentifier(EnsPSeqRegionadaptor adaptor,
                                            ajuint identifier,
                                            EnsPSeqRegion *Psr);

AjBool ensSeqRegionadaptorFetchByName(EnsPSeqRegionadaptor adaptor,
                                      const EnsPCoordSystem cs,
                                      const AjPStr name,
                                      EnsPSeqRegion *Psr);

AjBool ensSeqRegionadaptorFetchByNameFuzzy(EnsPSeqRegionadaptor adaptor,
                                           const EnsPCoordSystem cs,
                                           const AjPStr name,
                                           EnsPSeqRegion *Psr);

AjBool ensSeqRegionadaptorFetchAllByCoordSystem(EnsPSeqRegionadaptor adaptor,
                                                const EnsPCoordSystem cs,
                                                AjPList srlist);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
