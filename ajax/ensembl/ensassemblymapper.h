#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensassemblymapper_h
#define ensassemblymapper_h

#include "ensassembly.h"
#include "ensdata.h"
#include "enstable.h"




/*
** Prototype definitions
*/

/* Ensembl Generic Assembly Mapper */

EnsPGenericAssemblyMapper ensGenericAssemblyMapperNew(
    EnsPAssemblyMapperadaptor ama,
    EnsPCoordSystem asmcs,
    EnsPCoordSystem cmpcs);

EnsPGenericAssemblyMapper ensGenericAssemblyMapperNewRef(
    EnsPGenericAssemblyMapper gam);

AjBool ensGenericAssemblyMapperClear(EnsPGenericAssemblyMapper gam);

void ensGenericAssemblyMapperDel(EnsPGenericAssemblyMapper* Pgam);

EnsPAssemblyMapperadaptor ensGenericAssemblyMapperGetadaptor(
    const EnsPGenericAssemblyMapper gam);

const EnsPCoordSystem ensGenericAssemblyMapperGetAssembledCoordSystem(
    const EnsPGenericAssemblyMapper gam);

const EnsPCoordSystem ensGenericAssemblyMapperGetComponentCoordSystem(
    const EnsPGenericAssemblyMapper gam);

EnsPMapper ensGenericAssemblyMapperGetMapper(
    const EnsPGenericAssemblyMapper gam);

ajuint ensGenericAssemblyMapperGetMaxPairCount(
    const EnsPGenericAssemblyMapper gam);

ajuint ensGenericAssemblyMapperGetPairCount(
    const EnsPGenericAssemblyMapper gam);

AjBool ensGenericAssemblyMapperSetadaptor(EnsPGenericAssemblyMapper gam,
                                          EnsPAssemblyMapperadaptor ama);

AjBool ensGenericAssemblyMapperSetMaxPairCount(EnsPGenericAssemblyMapper gam,
                                               ajuint count);

AjBool ensGenericAssemblyMapperRegisterAssembled(EnsPGenericAssemblyMapper gam,
                                                 ajuint asmsrid,
                                                 ajuint chunkid);

AjBool ensGenericAssemblyMapperRegisterComponent(EnsPGenericAssemblyMapper gam,
                                                 ajuint cmpsrid);

AjBool ensGenericAssemblyMapperHaveRegisteredAssembled(
    const EnsPGenericAssemblyMapper gam,
    ajuint asmsrid,
    ajuint chunkid);

AjBool ensGenericAssemblyMapperHaveRegisteredComponent(
    const EnsPGenericAssemblyMapper gam,
    ajuint cmpsrid);

AjBool ensGenericAssemblyMapperMap(EnsPGenericAssemblyMapper gam,
                                   const EnsPSeqRegion sr,
                                   ajint srstart,
                                   ajint srend,
                                   ajint srstrand,
                                   AjPList mrs);

AjBool ensGenericAssemblyMapperFastMap(EnsPGenericAssemblyMapper gam,
                                       const EnsPSeqRegion sr,
                                       ajint srstart,
                                       ajint srend,
                                       ajint srstrand,
                                       AjPList mrs);

AjBool ensGenericAssemblyMapperRegisterAll(EnsPGenericAssemblyMapper gam);

/* Ensembl Chained Assembly Mapper */

EnsPChainedAssemblyMapper ensChainedAssemblyMapperNew(
    EnsPAssemblyMapperadaptor ama,
    EnsPCoordSystem srccs,
    EnsPCoordSystem midcs,
    EnsPCoordSystem trgcs);

EnsPChainedAssemblyMapper ensChainedAssemblyMapperNewRef(
    EnsPChainedAssemblyMapper cam);

void ensChainedAssemblyMapperDel(EnsPChainedAssemblyMapper *Pcam);

AjBool ensChainedAssemblyMapperClear(EnsPChainedAssemblyMapper cam);

EnsPAssemblyMapperadaptor ensChainedAssemblyMapperGetadaptor(
    const EnsPChainedAssemblyMapper cam);

const EnsPCoordSystem ensChainedAssemblyMapperGetSourceCoordSystem(
    const EnsPChainedAssemblyMapper cam);

const EnsPCoordSystem ensChainedAssemblyMapperGetMiddleCoordSystem(
    const EnsPChainedAssemblyMapper cam);

const EnsPCoordSystem ensChainedAssemblyMapperGetTargetCoordSystem(
    const EnsPChainedAssemblyMapper cam);

EnsPMapper ensChainedAssemblyMapperGetSourceMiddleMapper(
    const EnsPChainedAssemblyMapper cam);

EnsPMapper ensChainedAssemblyMapperGetTargetMiddleMapper(
    const EnsPChainedAssemblyMapper cam);

EnsPMapper ensChainedAssemblyMapperGetSourceTargetMapper(
    const EnsPChainedAssemblyMapper cam);

EnsPMapperRangeRegistry ensChainedAssemblyMapperGetSourceRegistry(
    const EnsPChainedAssemblyMapper cam);

EnsPMapperRangeRegistry ensChainedAssemblyMapperGetTargetRegistry(
    const EnsPChainedAssemblyMapper cam);

ajuint ensChainedAssemblyMapperGetMaxPairCount(
    const EnsPChainedAssemblyMapper cam);

AjBool ensChainedAssemblyMapperSetadaptor(EnsPChainedAssemblyMapper cam,
                                          EnsPAssemblyMapperadaptor ama);

AjBool ensChainedAssemblyMapperSetMaxPairCount(EnsPChainedAssemblyMapper cam,
                                               ajuint count);

ajuint ensChainedAssemblyMapperGetPairCount(
    const EnsPChainedAssemblyMapper cam);

AjBool ensChainedAssemblyMapperMap(EnsPChainedAssemblyMapper cam,
                                   const EnsPSeqRegion sr,
                                   ajint srstart,
                                   ajint srend,
                                   ajint srstrand,
                                   const EnsPSeqRegion optsr,
                                   AjBool fastmap,
                                   AjPList mrs);

AjBool ensChainedAssemblyMapperRegisterAll(EnsPChainedAssemblyMapper cam);

/* Ensembl Top-level Assembly Mapper */

EnsPTopLevelAssemblyMapper ensTopLevelAssemblyMapperNew(
    EnsPAssemblyMapperadaptor ama,
    EnsPCoordSystem cs1,
    EnsPCoordSystem cs2);

EnsPTopLevelAssemblyMapper ensTopLevelAssemblyMapperNewRef(
    EnsPTopLevelAssemblyMapper tlam);

void ensTopLevelAssemblyMapperDel(EnsPTopLevelAssemblyMapper* Ptlam);

const EnsPAssemblyMapperadaptor ensTopLevelAssemblyMapperGetadaptor(
    const EnsPTopLevelAssemblyMapper tlam);

const EnsPCoordSystem ensTopLevelAssemblyMapperGetAssembledCoordSystem(
    const EnsPTopLevelAssemblyMapper tlam);

const EnsPCoordSystem ensTopLevelAssemblyMapperGetComponentCoordSystem(
    const EnsPTopLevelAssemblyMapper tlam);

AjBool ensTopLevelAssemblyMapperSetadaptor(
    EnsPTopLevelAssemblyMapper tlam,
    EnsPAssemblyMapperadaptor ama);

AjBool ensTopLevelAssemblyMapperMap(EnsPTopLevelAssemblyMapper tlam,
                                    const EnsPSeqRegion sr,
                                    ajint srstart,
                                    ajint srend,
                                    ajint srstrand,
                                    AjBool fastmap,
                                    AjPList mrs);

/* Ensembl Assembly Mapper */

EnsPAssemblyMapper ensAssemblyMapperNew(EnsPAssemblyMapperadaptor ama,
                                        EnsPGenericAssemblyMapper gam,
                                        EnsPChainedAssemblyMapper cam,
                                        EnsPTopLevelAssemblyMapper tlam);

EnsPAssemblyMapper ensAssemblyMapperNewRef(EnsPAssemblyMapper am);

void ensAssemblyMapperDel(EnsPAssemblyMapper* Pam);

AjBool ensAssemblyMapperMap(EnsPAssemblyMapper am,
                            const EnsPSeqRegion sr,
                            ajint srstart,
                            ajint srend,
                            ajint srstrand,
                            AjPList mrs);

AjBool ensAssemblyMapperFastMap(EnsPAssemblyMapper am,
                                const EnsPSeqRegion sr,
                                ajint srstart,
                                ajint srend,
                                ajint srstrand,
                                AjPList mrs);

AjBool ensAssemblyMapperMapToSeqRegion(EnsPAssemblyMapper am,
                                       const EnsPSeqRegion sr,
                                       ajint srstart,
                                       ajint srend,
                                       ajint srstrand,
                                       const EnsPSeqRegion optsr,
                                       AjPList mrs);
		     
/* Ensembl Assembly Mapper Adaptor */

EnsPAssemblyMapperadaptor ensAssemblyMapperadaptorNew(EnsPDatabaseadaptor dba);

AjBool ensAssemblyMapperadaptorCacheClear(EnsPAssemblyMapperadaptor ama);

void ensAssemblyMapperadaptorDel(EnsPAssemblyMapperadaptor* Pama);

EnsPDatabaseadaptor ensAssemblyMapperadaptorGetDatabaseadaptor(
    const EnsPAssemblyMapperadaptor ama);

EnsPAssemblyMapper ensAssemblyMapperadaptorFetchByCoordSystems(
    EnsPAssemblyMapperadaptor ama,
    EnsPCoordSystem cs1,
    EnsPCoordSystem cs2);

AjBool ensAssemblyMapperadaptorSeqRegionIdsToNames(
    EnsPAssemblyMapperadaptor ama,
    const AjPList ids,
    AjPList names);

AjBool ensAssemblyMapperadaptorSeqRegionNamesToIds(
    EnsPAssemblyMapperadaptor ama,
    EnsPCoordSystem cs,
    const AjPList names,
    AjPList identifiers);

AjBool ensAssemblyMapperadaptorRegisterAssembled(
    EnsPAssemblyMapperadaptor ama,
    EnsPGenericAssemblyMapper gam,
    ajuint asmsrid,
    ajint regstart,
    ajint regend);

AjBool ensAssemblyMapperadaptorRegisterComponent(
    EnsPAssemblyMapperadaptor ama,
    EnsPGenericAssemblyMapper gam,
    ajuint cmpsrid);

AjBool ensAssemblyMapperadaptorRegisterAll(EnsPAssemblyMapperadaptor ama,
                                           EnsPGenericAssemblyMapper gam);

AjBool ensAssemblyMapperadaptorRegisterChained(
    EnsPAssemblyMapperadaptor ama,
    EnsPChainedAssemblyMapper cam,
    const AjPStr source,
    ajuint srcsrid,
    ajuint optsrid,
    AjPList ranges);

AjBool ensAssemblyMapperadaptorRegisterChainedSpecial(
    EnsPAssemblyMapperadaptor ama,
    EnsPChainedAssemblyMapper cam,
    const AjPStr source,
    ajuint srcsrid,
    ajuint optsrid,
    AjPList ranges);

AjBool ensAssemblyMapperadaptorRegisterAllChained(
    EnsPAssemblyMapperadaptor ama,
    EnsPChainedAssemblyMapper cam);

/*
** End of prototype definitions
*/

#define MENSCHAINEDASSEMBLYMAPPERADAPTORGETASSEMBLEDCOORDSYSTEM(cam) \
ensChainedAssemblyMapperGetSourceCoordSystem(cam)

#define MENSCHAINEDASSEMBLYMAPPERADAPTORGETCOMPONENTCOORDSYSTEM(cam) \
ensChainedAssemblyMapperGetTargetCoordSystem(cam)

#define MENSCHAINEDASSEMBLYMAPPERGETMAPPER(cam) \
ensChainedAssemblyMapperGetSourceTargetMapper(cam)

#endif

#ifdef __cplusplus
}
#endif
