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

EnsPSeqRegion ensSeqRegionNew(EnsPSeqRegionAdaptor adaptor,
                              ajuint identifier,
                              EnsPCoordSystem cs,
                              AjPStr name,
                              ajint length);

EnsPSeqRegion ensSeqRegionNewObj(EnsPSeqRegion object);

EnsPSeqRegion ensSeqRegionNewRef(EnsPSeqRegion sr);

void ensSeqRegionDel(EnsPSeqRegion *Psr);

const EnsPSeqRegionAdaptor ensSeqRegionGetAdaptor(const EnsPSeqRegion sr);

ajuint ensSeqRegionGetIdentifier(const EnsPSeqRegion sr);

EnsPCoordSystem ensSeqRegionGetCoordSystem(const EnsPSeqRegion sr);

AjPStr ensSeqRegionGetName(const EnsPSeqRegion sr);

ajint ensSeqRegionGetLength(const EnsPSeqRegion sr);

const AjPList ensSeqRegionGetAttributes(EnsPSeqRegion sr);

AjBool ensSeqRegionSetAdaptor(EnsPSeqRegion sr, EnsPSeqRegionAdaptor adaptor);

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

EnsPSeqRegionAdaptor ensSeqRegionAdaptorNew(EnsPDatabaseAdaptor dba);

void ensSeqRegionAdaptorDel(EnsPSeqRegionAdaptor *Padaptor);

EnsPDatabaseAdaptor ensSeqRegionAdaptorGetDatabaseAdaptor(
    EnsPSeqRegionAdaptor adaptor);

AjBool ensSeqRegionAdaptorCacheInsert(EnsPSeqRegionAdaptor adaptor,
                                      EnsPSeqRegion *Psr);

AjBool ensSeqRegionAdaptorCacheRemove(EnsPSeqRegionAdaptor adaptor,
                                      const EnsPSeqRegion sr);

AjBool ensSeqRegionAdaptorFetchByIdentifier(EnsPSeqRegionAdaptor adaptor,
                                            ajuint identifier,
                                            EnsPSeqRegion *Psr);

AjBool ensSeqRegionAdaptorFetchByName(EnsPSeqRegionAdaptor adaptor,
                                      const EnsPCoordSystem cs,
                                      const AjPStr name,
                                      EnsPSeqRegion *Psr);

AjBool ensSeqRegionAdaptorFetchByNameFuzzy(EnsPSeqRegionAdaptor adaptor,
                                           const EnsPCoordSystem cs,
                                           const AjPStr name,
                                           EnsPSeqRegion *Psr);

AjBool ensSeqRegionAdaptorFetchAllByCoordSystem(EnsPSeqRegionAdaptor adaptor,
                                                const EnsPCoordSystem cs,
                                                AjPList srlist);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
