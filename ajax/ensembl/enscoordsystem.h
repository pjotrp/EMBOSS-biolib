#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enscoordsystem_h
#define enscoordsystem_h

#include "ensdatabaseadaptor.h"
#include "enstable.h"




/* @data EnsPCoordSystemAdaptor ***********************************************
**
** Ensembl Coordinate System Adaptor
**
** @alias EnsSCoordSystemAdaptor
** @alias EnsOCoordSystemAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr CacheByIdentifier [AjPTable] Database identifier cache
** @attr CacheByName [AjPTable] Name cache
** @attr CacheByRank [AjPTable] Rank cache
** @attr CacheByDefault [AjPTable] Default Ensembl Coordinate Systems
** @attr MappingPaths [AjPTable] Mapping paths between coordinate systems
** @attr ExternalToInternal [AjPTable] External to internal Sequence Regions
** @attr InternalToExternal [AjPTable] Internal to external Sequence Regions
** @attr SeqLevel [void *] Sequence-level Ensembl Coordinate System
** @attr TopLevel [void *] Top-level Ensembl Coordinate System
** @@
******************************************************************************/

typedef struct EnsSCoordSystemAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByName;
    AjPTable CacheByRank;
    AjPTable CacheByDefault;
    AjPTable MappingPaths;
    AjPTable ExternalToInternal;
    AjPTable InternalToExternal;
    void *SeqLevel;
    void *TopLevel;
} EnsOCoordSystemAdaptor;

#define EnsPCoordSystemAdaptor EnsOCoordSystemAdaptor*




/* @data EnsPCoordSystem ******************************************************
**
** Ensembl Coordinate System
**
** @alias EnsSCoordSystem
** @alias EnsOCoordSystem
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Adaptor [EnsPCoordSystemAdaptor] Ensembl Coordinate System Adaptor
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @cc Bio::EnsEMBL::CoordSystem
** @attr Name [AjPStr] Coordinate System name
** @attr Version [AjPStr] Coordinate System version
** @attr Default [AjBool] Default Coordinate System version of this name
** @attr SequenceLevel [AjBool] Sequence-level attribute
** @attr TopLevel [AjBool] Top-level attribute
** @attr Rank [ajuint] Coordinate System rank
** @@
******************************************************************************/

typedef struct EnsSCoordSystem
{
    ajuint Use;
    ajuint Identifier;
    EnsPCoordSystemAdaptor Adaptor;
    AjPStr Name;
    AjPStr Version;
    AjBool Default;
    AjBool SequenceLevel;
    AjBool TopLevel;
    ajuint Rank;
} EnsOCoordSystem;

#define EnsPCoordSystem EnsOCoordSystem*




/*
** Prototype definitions
*/

/* Ensembl Coordinate System */

EnsPCoordSystem ensCoordSystemNew(EnsPCoordSystemAdaptor adaptor,
                                  ajuint identifier,
                                  AjPStr name,
                                  AjPStr version,
                                  ajuint rank,
                                  AjBool deflt,
                                  AjBool toplvl,
                                  AjBool seqlvl);

EnsPCoordSystem ensCoordSystemNewObj(EnsPCoordSystem object);

EnsPCoordSystem ensCoordSystemNewRef(EnsPCoordSystem cs);

void ensCoordSystemDel(EnsPCoordSystem* Pcs);

EnsPCoordSystemAdaptor ensCoordSystemGetAdaptor(const EnsPCoordSystem cs);

ajuint ensCoordSystemGetIdentifier(const EnsPCoordSystem cs);

const AjPStr ensCoordSystemGetName(const EnsPCoordSystem cs);

const AjPStr ensCoordSystemGetVersion(const EnsPCoordSystem cs);

AjBool ensCoordSystemGetDefault(const EnsPCoordSystem cs);

#define ensCoordSystemIsDefault ensCoordSystemGetDefault

AjBool ensCoordSystemGetSeqLevel(const EnsPCoordSystem cs);

#define ensCoordSystemIsSeqLevel ensCoordSystemGetSeqLevel

AjBool ensCoordSystemGetTopLevel(const EnsPCoordSystem cs);

#define ensCoordSystemIsTopLevel ensCoordSystemGetTopLevel

ajuint ensCoordSystemGetRank(const EnsPCoordSystem cs);

AjBool ensCoordSystemSetAdaptor(EnsPCoordSystem cs, EnsPCoordSystemAdaptor csa);

AjBool ensCoordSystemSetIdentifier(EnsPCoordSystem cs, ajuint identifier);

AjBool ensCoordSystemTrace(const EnsPCoordSystem cs, ajuint level);

AjBool ensCoordSystemMatch(const EnsPCoordSystem cs1,
                           const EnsPCoordSystem cs2);

ajuint ensCoordSystemGetMemSize(const EnsPCoordSystem cs);

AjPStr ensCoordSystemGetSpecies(EnsPCoordSystem cs);

/* Ensembl Coordinate System Adaptor */

EnsPCoordSystemAdaptor ensCoordSystemAdaptorNew(EnsPDatabaseAdaptor dba);

void ensCoordSystemAdaptorDel(EnsPCoordSystemAdaptor* Pcsa);

EnsPDatabaseAdaptor ensCoordSystemAdaptorGetDatabaseAdaptor(
    const EnsPCoordSystemAdaptor adaptor);

AjBool ensCoordSystemAdaptorFetchAll(const EnsPCoordSystemAdaptor adaptor,
                                     AjPList cslist);

AjBool ensCoordSystemAdaptorFetchAllByName(const EnsPCoordSystemAdaptor adaptor,
                                           const AjPStr name,
                                           AjPList cslist);

AjBool ensCoordSystemAdaptorFetchByIdentifier(
    const EnsPCoordSystemAdaptor adaptor,
    ajuint identifier,
    EnsPCoordSystem *Pcs);

AjBool ensCoordSystemAdaptorFetchByName(const EnsPCoordSystemAdaptor adaptor,
                                        const AjPStr name,
                                        const AjPStr version,
                                        EnsPCoordSystem *Pcs);

AjBool ensCoordSystemAdaptorFetchByRank(const EnsPCoordSystemAdaptor adaptor,
                                        ajuint rank,
                                        EnsPCoordSystem *Pcs);

AjBool ensCoordSystemAdaptorFetchSeqLevel(const EnsPCoordSystemAdaptor adaptor,
                                          EnsPCoordSystem *Pcs);

AjBool ensCoordSystemAdaptorFetchTopLevel(const EnsPCoordSystemAdaptor adaptor,
                                          EnsPCoordSystem *Pcs);

const AjPList ensCoordSystemAdaptorGetMappingPath(
    const EnsPCoordSystemAdaptor adaptor,
    EnsPCoordSystem cs1,
    EnsPCoordSystem cs2);

ajuint ensCoordSystemAdaptorGetExternalSeqRegionIdentifier(
    const EnsPCoordSystemAdaptor adaptor,
    ajuint srid);

ajuint ensCoordSystemAdaptorGetInternalSeqRegionIdentifier(
    const EnsPCoordSystemAdaptor adaptor,
    ajuint srid);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
