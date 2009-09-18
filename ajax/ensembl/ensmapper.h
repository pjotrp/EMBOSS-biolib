#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensmapper_h
#define ensmapper_h

#include "enscoordsystem.h"
#include "enstable.h"




/* @data EnsPMapperUnit *******************************************************
**
** Ensembl Mapper Unit.
**
** One unit of an Ensembl Mapper Pair.
**
** @alias EnsSMapperUnit
** @alias EnsOMapperUnit
**
** @cc Bio::EnsEMBL::Mapper::Unit
** @attr ObjectIdentifier [ajuint] Ensembl Object identifier
** @attr Start [ajint] Start coordinate
** @attr End [ajint] End coordinate
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct EnsSMapperUnit
{
    ajuint ObjectIdentifier;
    ajint Start;
    ajint End;
    ajuint Use;
} EnsOMapperUnit;

#define EnsPMapperUnit EnsOMapperUnit*




/* @datatype EnsEMapperUnitType ***********************************************
**
** Ensembl Mapper Unit Type enumeration.
**
** @attr enum [EnsEMapperUnitType] Value
** @@
******************************************************************************/

enum EnsEMapperUnitType
{
    ensEMapperUnitTypeNULL,
    ensEMapperUnitTypeSource,
    ensEMapperUnitTypeTarget
};




/* @data EnsPMapperPair *******************************************************
**
** Ensembl Mapper Pair.
**
** An Ensembl Mapper Pair consists of two Ensembl Mapper Units.
**
** @alias EnsSMapperPair
** @alias EnsOMapperPair
**
** @cc Bio::EnsEMBL::Mapper::Pair
** @attr Source [EnsPMapperUnit] Source Ensembl Mapper Unit
** @attr Target [EnsPMapperUnit] Target Ensembl Mapper Unit
** @attr Orientation [ajint] Relative orientation of the Ensembl Mapper Units
** @cc Bio::EnsEMBL::Mapper::IndelPair
** @attr InsertionDeletion [AjBool] Insertion-deletion attribute
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSMapperPair
{
    EnsPMapperUnit Source;
    EnsPMapperUnit Target;
    ajint Orientation;
    AjBool InsertionDeletion;
    ajuint Use;
    ajuint Padding;
} EnsOMapperPair;

#define EnsPMapperPair EnsOMapperPair*




/* @datatype EnsEMapperResultType *********************************************
**
** Ensembl Mapper Result Type enumeration.
**
** @attr enum [EnsEMapperResultType] Value
** @@
******************************************************************************/

enum EnsEMapperResultType
{
    ensEMapperResultNULL,
    ensEMapperResultCoordinate,
    ensEMapperResultGap,
    ensEMapperResultInDel
};




/* @data EnsPMapperResult *****************************************************
**
** Ensembl Mapper Result.
**
** Representation of a mapped Ensembl Object returned by an Ensembl Mapper when
** the source region maps to valid sequence. Depending on the type element
** this can also represent a gap.
**
** @alias EnsSMapperResult
** @alias EnsOMapperResult
**
** @attr Type [AjEnum] Result type (ensEMapperResultCoordinate, ...)
** @cc Bio::EnsEMBL::Mapper::Coordinate
** @cc Bio::EnsEMBL::Mapper::IndelCoordinate
** @attr ObjectIdentifier [ajuint] Ensembl Object identifier
** @attr Start [ajint] Start coordinate
** @attr End [ajint] End coordinate
** @attr Strand [ajint] Strand information
** @attr CoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @cc Bio::EnsEMBL::Mapper::Gap
** @cc Bio::EnsEMBL::Mapper::IndelCoordinate
** @attr GapStart [ajint] Start coordinate of a gap
** @attr GapEnd [ajint] End coordinate of a gap
** @attr Use [ajuint] Use counter
** @@
**
** This object subsumes the following Perl objects all returned by the
** Bio::EnsEMBL::Mapper
**
** Bio::EnsEMBL::Mapper::Coordinate
** Bio::EnsEMBL::Mapper::Gap
** Bio::EnsEMBL::Mapper::IndelCoordinate
**
** The objects can be distinguished by their type element.
******************************************************************************/

typedef struct EnsSMapperResult
{
    EnsPCoordSystem CoordSystem;
    AjEnum Type;
    ajuint ObjectIdentifier;
    ajint Start;
    ajint End;
    ajint Strand;
    ajint GapStart;
    ajint GapEnd;
    ajuint Use;
} EnsOMapperResult;

#define EnsPMapperResult EnsOMapperResult*




/* @data EnsPMapperRange ******************************************************
**
** Ensembl Mapper Range.
**
** An Ensembl Mapper Range defines start and end ccordinates of genome ranges.
**
** @alias EnsSMapperRange
** @alias EnsOMapperRange
**
** @attr Start [ajint] Start coordinate
** @attr End [ajint] End coordinate
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSMapperRange
{
    ajint Start;
    ajint End;
    ajuint Use;
    ajuint Padding;
} EnsOMapperRange;

#define EnsPMapperRange EnsOMapperRange*




/* @data EnsPMapperRangeRegistry **********************************************
**
** Ensembl Mapper Range Registry.
**
** An Ensembl Mapper Range Registry maintains a table of registered regions.
**
** @alias EnsSMapperRangeRegistry
** @alias EnsOMapperRangeRegistry
**
** @attr Registry [AjPTable] Registry Table
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @@
** The Ensembl Mapper Range Registry maintains an internal list of registered
** regions and is used to quickly ascertain if and what regions of a provided
** range need registration. It is implemented as a first-level AJAX table with
** Ensembl Object identifers as keys and second-level AJAX Lists of
** Ensembl Mapper Ranges as values.
******************************************************************************/

typedef struct EnsSMapperRangeRegistry
{
    AjPTable Registry;
    ajuint Use;
    ajuint Padding;
} EnsOMapperRangeRegistry;

#define EnsPMapperRangeRegistry EnsOMapperRangeRegistry*




/* @data EnsPMapper ***********************************************************
**
** Ensembl Mapper.
**
** Generic mapper to provide coordinate transforms between two
** disjoint Ensembl Coordinate Systems.
**
** @alias EnsSMapper
** @alias EnsOMapper
**
** @cc Bio::EnsEMBL::Mapper
** @attr SourceType [AjPStr] Source mapping type
** @attr TargetType [AjPStr] Target mapping type
** @attr SourceCoordSystem [EnsPCoordSystem] Source Ensembl Coordinate System
** @attr TargetCoordSystem [EnsPCoordSystem] Target Ensembl Coordinate System
** @attr Pairs [AjPTable] AJAX Table of AJAX Tables with Ensembl Mapper Pairs
** @attr IsSorted [AjBool] Ensembl Mapper Pairs in the Mapper are sorted
** @attr PairCount [ajuint] Number of Ensembl Mapper Pairs
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @@
** The AJAX Table Pairs forms the top hierarchy of an Ensembl Mapper Pair
** cache. The Table uses the contents of the SourceType and TargetType strings
** as index and holds a second hierarchy of Tables, which use Ensembl Object
** identifiers as index. Those second-level AJAX Tables then hold a
** third-level of AJAX Lists of Ensembl Mapper Pairs.
******************************************************************************/

typedef struct EnsSMapper
{
    AjPStr SourceType;
    AjPStr TargetType;
    EnsPCoordSystem SourceCoordSystem;
    EnsPCoordSystem TargetCoordSystem;
    AjPTable Pairs;
    AjBool IsSorted;
    ajuint PairCount;
    ajuint Use;
    ajuint Padding;
} EnsOMapper;

#define EnsPMapper EnsOMapper*




/*
** Prototype definitions
*/

/* Ensembl Mapper Unit */

EnsPMapperUnit ensMapperUnitNew(ajuint oid, ajint start, ajint end);

EnsPMapperUnit ensMapperUnitNewObj(const EnsPMapperUnit object);

EnsPMapperUnit ensMapperUnitNewRef(EnsPMapperUnit mu);

void ensMapperUnitDel(EnsPMapperUnit* Pmu);

ajuint ensMapperUnitGetObjectIdentifier(const EnsPMapperUnit mu);

ajint ensMapperUnitGetStart(const EnsPMapperUnit mu);

ajint ensMapperUnitGetEnd(const EnsPMapperUnit mu);

AjBool ensMapperUnitSetObjectIdentifier(EnsPMapperUnit mu, ajuint oid);

AjBool ensMapperUnitSetStart(EnsPMapperUnit mu, ajint start);

AjBool ensMapperUnitSetEnd(EnsPMapperUnit mu, ajint end);

ajuint ensMapperUnitGetMemSize(const EnsPMapperUnit mu);

AjBool ensMapperUnitTrace(const EnsPMapperUnit mu, ajuint level);

/* Ensembl Mapper Pair */

EnsPMapperPair ensMapperPairNew(EnsPMapperUnit source,
                                EnsPMapperUnit target,
                                ajint ori,
                                AjBool indel);

#define MENSMAPPERINDELPAIRNEW(source, target, ori) \
ensMapperPairNew(source, target, ori, AJTRUE);

EnsPMapperPair ensMapperPairNewC(ajuint srcoid,
                                 ajint srcstart,
                                 ajint srcend,
                                 ajuint trgoid,
                                 ajint trgstart,
                                 ajint trgend,
                                 ajint ori,
                                 AjBool indel);

EnsPMapperPair ensMapperPairNewObj(const EnsPMapperPair object);

EnsPMapperPair ensMapperPairNewRef(EnsPMapperPair mp);

void ensMapperPairDel(EnsPMapperPair* Pmp);

#define MENSMAPPERINDELPAIRDEL(Pmp) \
ensMapperPairDel(Pmp);

EnsPMapperUnit ensMapperPairGetSource(const EnsPMapperPair mp);

EnsPMapperUnit ensMapperPairGetTarget(const EnsPMapperPair mp);

ajint ensMapperPairGetOrientation(const EnsPMapperPair mp);

AjBool ensMapperPairGetInsertionDeletion(const EnsPMapperPair mp);

EnsPMapperUnit ensMapperPairGetUnit(const EnsPMapperPair mp, AjEnum type);

ajuint ensMapperPairGetMemSize(const EnsPMapperPair mp);

AjBool ensMapperPairTrace(const EnsPMapperPair mp, ajuint level);

/* Ensembl Mapper Result */

EnsPMapperResult ensMapperResultNew(AjEnum type,
                                    ajuint oid,
                                    ajint start,
                                    ajint end,
                                    ajint strand,
                                    EnsPCoordSystem cs,
                                    ajint gapstart,
                                    ajint gapend);

#define MENSMAPPERCOORDINATENEW(oid, start, end, strand, cs) \
ensMapperResultNew(ensEMapperResultCoordinate, oid, start, end, strand, cs, \
		   0, 0)

#define MENSMAPPERGAPNEW(start, end) \
ensMapperResultNew(ensEMapperResultGap, 0, 0, 0, 0, NULL, start, end)

#define MENSMAPPERINDELNEW(oid, start, end, strand, cs, gstart, gend) \
ensMapperResultNew(ensEMapperResultInDel, oid, start, end, strand, cs, \
		   gstart, gend)

EnsPMapperResult ensMapperResultNewObj(const EnsPMapperResult object);

EnsPMapperResult ensMapperResultNewRef(EnsPMapperResult mr);

void ensMapperResultDel(EnsPMapperResult* Pmr);

AjEnum ensMapperResultGetType(const EnsPMapperResult mr);

ajuint ensMapperResultGetObjectIdentifier(const EnsPMapperResult mr);

ajint ensMapperResultGetStart(const EnsPMapperResult mr);

ajint ensMapperResultGetEnd(const EnsPMapperResult mr);

ajint ensMapperResultGetStrand(const EnsPMapperResult mr);

EnsPCoordSystem ensMapperResultGetCoordSystem(const EnsPMapperResult mr);

ajint ensMapperResultGetGapStart(const EnsPMapperResult mr);

ajint ensMapperResultGetGapEnd(const EnsPMapperResult mr);

ajuint ensMapperResultGetCoordinateLength(const EnsPMapperResult mr);

ajuint ensMapperResultGetGapLength(const EnsPMapperResult mr);

ajuint ensMapperResultGetLength(const EnsPMapperResult mr);

AjBool ensMapperResultTrace(const EnsPMapperResult mr, ajuint level);

/* Ensembl Mapper Range */

EnsPMapperRange ensMapperRangeNew(ajint start, ajint end);

EnsPMapperRange ensMapperRangeNewObj(const EnsPMapperRange object);

EnsPMapperRange ensMapperRangeNewRef(EnsPMapperRange mr);

void ensMapperRangeDel(EnsPMapperRange* Pmr);

ajint ensMapperRangeGetStart(const EnsPMapperRange mr);

ajint ensMapperRangeGetEnd(const EnsPMapperRange mr);

/* Ensembl Mapper Range Registry */

EnsPMapperRangeRegistry ensMapperRangeRegistryNew(void);

EnsPMapperRangeRegistry ensMapperRangeRegistryNewRef(
    EnsPMapperRangeRegistry mrr);

AjBool ensMapperRangeRegistryClear(EnsPMapperRangeRegistry mrr);

void ensMapperRangeRegistryDel(EnsPMapperRangeRegistry* Pmrr);

AjBool ensMapperRangeRegistryCheckAndRegister(EnsPMapperRangeRegistry mrr,
                                              ajuint oid,
                                              ajint chkstart,
                                              ajint chkend,
                                              ajint regstart,
                                              ajint regend,
                                              AjPList ranges);

const AjPList ensMapperRangeRegistryGetRanges(
    const EnsPMapperRangeRegistry mrr,
    ajuint oid);

ajuint ensMapperRangeRegistryOverlapSize(const EnsPMapperRangeRegistry mrr,
                                         ajuint oid,
                                         ajint start,
                                         ajint end);

/* Ensembl Mapper */

EnsPMapper ensMapperNew(AjPStr srctype,
                        AjPStr trgtype,
                        EnsPCoordSystem srccs,
                        EnsPCoordSystem trgcs);

EnsPMapper ensMapperNewRef(EnsPMapper mapper);

AjBool ensMapperClear(EnsPMapper mapper);

void ensMapperDel(EnsPMapper* Pmapper);

AjPStr ensMapperGetSourceType(const EnsPMapper mapper);

AjPStr ensMapperGetTargetType(const EnsPMapper mapper);

ajuint ensMapperGetPairCount(const EnsPMapper mapper);

AjBool ensMapperIsSorted(const EnsPMapper mapper);

AjBool ensMapperAddCoordinates(EnsPMapper mapper,
                               ajuint srcoid,
                               ajint srcstart,
                               ajint srcend,
                               ajint ori,
                               ajuint trgoid,
                               ajint trgstart,
                               ajint trgend);

AjBool ensMapperAddMapperUnits(EnsPMapper mapper,
                               EnsPMapperUnit srcmu,
                               EnsPMapperUnit trgmu,
                               ajint ori,
                               AjBool indel);

AjBool ensMapperAddMapperPair(EnsPMapper mapper, EnsPMapperPair mp);

AjBool ensMapperListPairs(EnsPMapper mapper,
                          ajuint oid,
                          ajint start,
                          ajint end,
                          const AjPStr type,
                          AjPList mps);

AjBool ensMapperMapCoordinates(EnsPMapper mapper,
                               ajuint oid,
                               ajint start,
                               ajint end,
                               ajint strand,
                               const AjPStr type,
                               AjPList mrs);

AjBool ensMapperFastMap(EnsPMapper mapper,
                        ajuint oid,
                        ajint start,
                        ajint end,
                        ajint strand,
                        const AjPStr type,
                        AjPList mrs);

AjBool ensMapperAddInDelCoordinates(EnsPMapper mapper,
                                    ajuint srcoid,
                                    ajint srcstart,
                                    ajint srcend,
                                    ajint ori,
                                    ajuint trgoid,
                                    ajint trgstart,
                                    ajint trgend);

AjBool ensMapperMapInDel(EnsPMapper mapper,
                         ajuint oid,
                         ajint start,
                         ajint end,
                         ajint strand,
                         const AjPStr type,
                         AjPList mrs);

AjBool ensMapperAddMapper(EnsPMapper mapper1, EnsPMapper mapper2);

ajuint ensMapperGetMemSize(const EnsPMapper mapper);

AjBool ensMapperTrace(const EnsPMapper mapper, ajuint level);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
