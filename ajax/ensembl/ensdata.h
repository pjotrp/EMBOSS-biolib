#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensdata_h
#define ensdata_h

#include "enscache.h"
#include "enscoordsystem.h"
#include "ensmapper.h"




/* @data EnsPAttribute ********************************************************
**
** Ensembl Attribute.
**
** @alias EnsSAttribute
** @alias EnsOAttribute
**
** @cc Bio::EnsEMBL::Attribute
** @attr Code [AjPStr] Ensembl Code
** @attr Name [AjPStr] Ensembl Name
** @attr Description [AjPStr] Description
** @attr Value [AjPStr] Value
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSAttribute {
    AjPStr Code;
    AjPStr Name;
    AjPStr Description;
    AjPStr Value;
    ajuint Use;
    ajuint Padding;
} EnsOAttribute;

#define EnsPAttribute EnsOAttribute*




/* @data EnsPSeqRegionAdaptor *************************************************
**
** Ensembl Sequence Region Adaptor.
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr CacheByIdentifier [EnsPCache] Ensembl Cache to store Ensembl Sequence
**                              Regions with SQL-database identifiers as index
** @attr CacheByName [AjPTable] AJAX Table to cache Ensembl Sequence Regions
**                              with 'SequenceRegionName:CoordinatesystemId'
**                              AJAX Strings as index
** @@
******************************************************************************/

typedef struct EnsSSeqRegionAdaptor {
    EnsPDatabaseAdaptor Adaptor;
    EnsPCache CacheByIdentifier;
    AjPTable CacheByName;
} EnsOSeqRegionAdaptor;

#define EnsPSeqRegionAdaptor EnsOSeqRegionAdaptor*




/* @data EnsPSeqRegion ********************************************************
**
** Ensembl Sequence Region.
**
** @attr Use [ajuint] Usage counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPSeqRegionAdaptor] Ensembl Sequence Region Adaptor
** @cc Bio::EnsEMBL::???
** @attr CoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @attr Attributes [AjPList] AJAX List of Ensembl Attributes
** @attr Name [AjPStr] Name
** @attr Length [ajint] Length
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

/*
** FIXME: The SQL table definition for seq_region.length specifies SIGNED.
** Why? This should be safe to be UNSIGNED.
*/

typedef struct EnsSSeqRegion {
    ajuint Use;
    ajuint Identifier;
    EnsPSeqRegionAdaptor Adaptor;
    EnsPCoordSystem CoordSystem;
    AjPStr Name;
    AjPList Attributes;
    ajint Length;
    ajuint Padding;
} EnsOSeqRegion;

#define EnsPSeqRegion EnsOSeqRegion*




/* @data EnsPSliceAdaptor *****************************************************
**
** Ensembl Slice Adaptor.
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr CacheByIdentifier [EnsPCache] Cache Ensembl Slice by their
**                                     Ensembl Sequence Region identifier,
**                                     start, end and strand coordinate.
** @attr CacheByName [AjPTable] Cache Ensembl Slices by their
**                              Ensembl Sequence Region name, start, end and
**                              strand coordinate.
** @@
******************************************************************************/

typedef struct EnsSSliceAdaptor {
    EnsPDatabaseAdaptor Adaptor;
    EnsPCache CacheByIdentifier;
    AjPTable CacheByName;
} EnsOSliceAdaptor;

#define EnsPSliceAdaptor EnsOSliceAdaptor*




/* @data EnsPSlice ************************************************************
**
** Ensembl Slice.
**
** Holds information about a genome sequence slice.
**
** @alias EnsSSlice
** @alias EnsOSlice
**
** @attr Adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @attr SeqRegion [EnsPSeqRegion] Ensembl Sequence Region
** @attr Sequence [AjPStr] Sequence
** @attr Start [ajint] Start coordinate
** @attr End [ajint] End coordinate
** @attr Strand [ajint] Strand information (+1|-1)
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct EnsSSlice {
    EnsPSliceAdaptor Adaptor;
    EnsPSeqRegion SeqRegion;
    AjPStr Sequence;
    ajint Start;
    ajint End;
    ajint Strand;
    ajuint Use;
} EnsOSlice;

#define EnsPSlice EnsOSlice*




/* @data EnsPAssemblyMapperAdaptor ********************************************
**
** Ensembl Assembly Mapper Adaptor.
**
** The Ensembl Assembly Mapper Adaptor is used to retrieve Ensembl Mappers
** between any two Ensembl Coordinate Systems whose makeup is described by
** the 'assembly' table. Currently, one-step (explicit) and two-step (implict)
** pairwise mapping is supported. In one-step mapping an explicit relationship
** between the Coordinate Systems is defined in the 'assembly' table.
** In two-step 'chained' mapping no explicit mapping is present, but the
** Coordinate Systems must share a common mapping to an intermediate
** Coordinate System.
**
** @alias EnsSAssemblyMapperAdaptor
** @alias EnsOAssemblyMapperAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr AsmMapperCache [AjPTable] Ensembl Assembly Mapper cache
** @attr MultipleMappings [AjPTable] Sequence Regions that have more than one
**                                   entry in the Ensembl Core 'assembly'
**                                   table, i. e. they map to more than one
**                                   location.
** @@
** The Ensembl Assembly Mapper cache is implemented as an AJAX Table with
** Ensembl Coordinate System identifiers as AJAX String keys and
** Ensembl Assembly Mapper objects as values.
******************************************************************************/

typedef struct EnsSAssemblyMapperAdaptor {
    EnsPDatabaseAdaptor Adaptor;
    AjPTable AsmMapperCache;
    AjPTable MultipleMappings;
} EnsOAssemblyMapperAdaptor;

#define EnsPAssemblyMapperAdaptor EnsOAssemblyMapperAdaptor*




/* @data EnsPGenericAssemblyMapper ********************************************
**
** Ensembl Generic Assembly Mapper.
**
** The Ensembl Generic Assembly Mapper handles mappings between two Ensembl
** Coordinate Systems using the information stored in the 'assembly' table of
** an Ensembl Core database.
**
** The Ensembl Generic Assembly Mapper is a database aware mapper, which
** facilitates conversion of coordinates between any two coordinate systems
** with a relationship explicitly defined in the assembly table. In future,
** it may be possible to perform multiple step (implicit) mapping between
** coordinate systems.
**
** It is implemented using the Ensembl Mapper object, which is a generic mapper
** object between disjoint coordinate systems.
**
** @alias EnsSGenericAssemblyMapper
** @alias EnsOGenericAssemblyMapper
**
** @attr Adaptor [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @attr AsmCoordSystem [EnsPCoordSystem] Assembled Ensembl Coordinate System
** @attr CmpCoordSystem [EnsPCoordSystem] Component Ensembl Coordinate System
** @attr AsmRegister [AjPTable] Assembled Register
** @attr CmpRegister [AjPTable] Component Register
** @attr Mapper [EnsPMapper] Ensembl Mapper
** @attr MaxPairCount [ajuint] Maximum Ensembl Mapper Pair count
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct EnsSGenericAssemblyMapper {
    EnsPAssemblyMapperAdaptor Adaptor;
    EnsPCoordSystem AsmCoordSystem;
    EnsPCoordSystem CmpCoordSystem;
    AjPTable AsmRegister;
    AjPTable CmpRegister;
    EnsPMapper Mapper;
    ajuint MaxPairCount;
    ajuint Use;
} EnsOGenericAssemblyMapper;

#define EnsPGenericAssemblyMapper EnsOGenericAssemblyMapper*




/* @data EnsPChainedAssemblyMapper ********************************************
**
** Ensembl Chained Assembly Mapper.
**
** The Ensembl Chained Assembly Mapper is an extension of the regular
** Ensembl Generic Assembly Mapper that allows for mappings between
** Ensembl Coordinate Systems that require multi-step mapping.
** For example if explicit mappings are defined between the following
** Coordinate Systems,
**   chromosome <-> contig
**   contig     <-> clone
** the Ensembl Chained Assembly Mapper would be able to perform implicit
** mapping between the chromosome and clone coordinate systems. This should be
** transparent to the user of this module, and users should not even realise
** that they are using a Chained Assembly Mapper as opposed to a
** Generic Assembly Mapper.
**
** @alias EnsSChainedAssemblyMapper
** @alias EnsOChainedAssemblyMapper
**
** @attr Adaptor [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @attr SourceCoordSystem [EnsPCoordSystem] Source Ensembl Coordinate System
** @attr MiddleCoordSystem [EnsPCoordSystem] Middle Ensembl Coordinate System
** @attr TargetCoordSystem [EnsPCoordSystem] Target Ensembl Coordinate System
** @attr SourceMiddleMapper [EnsPMapper] Source to middle Ensembl Mapper
** @attr TargetMiddleMapper [EnsPMapper] Target to middle Ensembl Mapper
** @attr SourceTargetMapper [EnsPMapper] Source to target Ensembl Mapper
** @attr SourceRegistry [EnsPMapperRangeRegistry] Source Ensembl Mapper
**                                                Range Registry
** @attr TargetRegistry [EnsPMapperRangeRegistry] Target Ensembl Mapper
**                                                Range Registry
** @attr MaxPairCount [ajuint] Maximum Ensembl Mapper Pair count
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct EnsSChainedAssemblyMapper {
    EnsPAssemblyMapperAdaptor Adaptor;
    EnsPCoordSystem SourceCoordSystem;
    EnsPCoordSystem MiddleCoordSystem;
    EnsPCoordSystem TargetCoordSystem;
    EnsPMapper SourceMiddleMapper;
    EnsPMapper TargetMiddleMapper;
    EnsPMapper SourceTargetMapper;
    EnsPMapperRangeRegistry SourceRegistry;
    EnsPMapperRangeRegistry TargetRegistry;
    ajuint MaxPairCount;
    ajuint Use;
} EnsOChainedAssemblyMapper;

#define EnsPChainedAssemblyMapper EnsOChainedAssemblyMapper*




/* @data EnsPTopLevelAssemblyMapper *******************************************
**
** Ensembl Top-Level Assembly Mapper.
**
** The Ensembl Top-Level Assembly Mapper performs mapping between a provided
** Ensembl Coordinate System and the top-level pseudo Cooordinate System.
** The top-level Coordinate System is not a real Coordinate System, but
** represents the highest Coordinate System that can be mapped to in a given
** Sequence Region. It is only possible to perform unidirectional mapping
** using this Mapper, because it does not make sense to map from the top-level
** Coordinate System to another Coordinate System.
**
** @alias EnsSTopLevelAssemblyMapper
** @alias EnsOTopLevelAssemblyMapper
**
** @attr Adaptor [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor.
** @attr CoordSystems [AjPList] AJAX List of Ensembl Coordinate Systems.
** @attr TopLevelCoordSystem [EnsPCoordSystem]
**                            Top-level Ensembl Coordinate System.
** @attr OtherCoordSystem [EnsPCoordSystem] Other Ensembl Coordinate System.
** @attr Use [ajuint] Use counter.
** @attr Padding [ajuint] Padding to alignment boundary.
** @@
******************************************************************************/

typedef struct EnsSTopLevelAssemblyMapper {
    EnsPAssemblyMapperAdaptor Adaptor;
    AjPList CoordSystems;
    EnsPCoordSystem TopLevelCoordSystem;
    EnsPCoordSystem OtherCoordSystem;
    ajuint Use;
    ajuint Padding;
} EnsOTopLevelAssemblyMapper;

#define EnsPTopLevelAssemblyMapper EnsOTopLevelAssemblyMapper*




/* @data EnsPAssemblyMapper ***************************************************
**
** Ensembl Assembly Mapper.
**
** The Ensembl Assembly Mapper is a wrapper for specialised Assembly Mappers.
**
** @alias EnsSAssemblyMapper
** @alias EnsOAssemblyMapper
**
** @attr Adaptor [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor.
** @attr Generic [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper.
** @attr Chained [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper.
** @attr Generic [EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                            Assembly Mapper.
** @attr Use [ajuint] Use counter.
** @attr Padding [ajuint] Padding to alignment boundary.
** @@
** This object subsumes the following Perl objects all returned by the
** Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor
**
** Bio::EnsEMBL::AssemblyMapper (here Ensembl Generic Assembly Mapper)
** Bio::EnsEMBL::ChainedAssemblyMapper
** Bio::EnsEMBL::TopLevelAssemblyMapper
**
** The objects can be distinguished by their Type element.
******************************************************************************/

typedef struct EnsSAssemblyMapper {
    EnsPAssemblyMapperAdaptor Adaptor;
    EnsPGenericAssemblyMapper Generic;
    EnsPChainedAssemblyMapper Chained;
    EnsPTopLevelAssemblyMapper TopLevel;
    ajuint Use;
    ajuint Padding;
} EnsOAssemblyMapper;

#define EnsPAssemblyMapper EnsOAssemblyMapper*




/* @data EnsPBaseAdaptorLeftJoin **********************************************
**
** Ensembl Base Adaptor Left Join Statement.
**
** @alias EnsSBaseAdaptorLeftJoin
** @alias EnsOBaseAdaptorLeftJoin
**
** @attr Table [const char*] SQL Table
** @attr Condition [const char*] SQL Condition for a LEFT JOIN statement
** @@
******************************************************************************/

typedef struct EnsSBaseAdaptorLeftJoin {
    const char *Table;
    const char *Condition;
} EnsOBaseAdaptorLeftJoin;

#define EnsPBaseAdaptorLeftJoin EnsOBaseAdaptorLeftJoin*




/* @data EnsPBaseAdaptor ******************************************************
**
** Ensembl Base Adaptor.
**
** @alias EnsSBaseAdaptor
** @alias EnsOBaseAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor.
** @attr Tables [const char **] One dimensional array of table name character
**                              strings
** @attr Columns [const char **] One dimensional array of column name character
**                               strings
** @attr LeftJoin [EnsOBaseAdaptorLeftJoin*] Address of an array of
**                                   Ensembl Base Adaptor Left Join Statements
** @attr DefaultCondition [const char *] Default condition in a
**                                       SQL SELECT statement
** @attr FinalCondition [const char *] Final condition in a
**                                     SQL SELECT statement
** @attr StraightJoin [AjBool] Set the MySQL-specific STRAIGHT_JOIN option in a
**                             SQL SELECT statement
** @attr Padding [ajuint] Padding to alignment boundary
** @attr Query [(AjBool)] Function pointer to an Ensembl object-spcific Query
**                        function, consolidating SQL results into Ensembl
**                        objects
** @@
** NOTE: In constrast to the Perl API, this implementation does not have a
** species_id element, which is already available from the Database Adaptor.
******************************************************************************/

typedef struct EnsSBaseAdaptor {
    EnsPDatabaseAdaptor Adaptor;
    const char **Tables;
    const char **Columns;
    EnsOBaseAdaptorLeftJoin *LeftJoin;
    const char *DefaultCondition;
    const char *FinalCondition;
    AjBool StraightJoin;
    ajuint Padding;
    AjBool (*Query)(EnsPDatabaseAdaptor dba,
		    const AjPStr sql,
		    EnsPAssemblyMapper mapper,
		    EnsPSlice slice,
		    AjPList objects);
} EnsOBaseAdaptor;

#define EnsPBaseAdaptor EnsOBaseAdaptor*




/* @data EnsPAnalysisAdaptor **************************************************
**
** Ensembl Analysis Adaptor.
**
** @alias EnsSAnalysisAdaptor
** @alias EnsOAnalysisAdaptor
**
** @attr Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
** @attr CacheByIdentifier [AjPTable] Identifier cache
** @attr CacheByName [AjPTable] Name cache
** @@
******************************************************************************/

typedef struct EnsSAnalysisAdaptor {
    EnsPBaseAdaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByName;
} EnsOAnalysisAdaptor;

#define EnsPAnalysisAdaptor EnsOAnalysisAdaptor*




/* @data EnsPAnalysis *********************************************************
**
** Ensembl Analysis.
** An Ensembl Analysis object stores details of an analysis within the
** Ensembl genome analysis and annotation pipeline.
**
** @alias EnsSAnalysis
** @alias EnsOAnalysis
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPAnalysisAdaptor] Ensembl Analysis Adaptor
** @cc Bio::EnsEMBL::Analysis
** @attr CreationDate [AjPStr] Creation date
** @attr Name [AjPStr] Name
** @attr DatabaseName [AjPStr] Database name
** @attr DatabaseVersion [AjPStr] Database version
** @attr DatabaseFile [AjPStr] Database file
** @attr ProgramName [AjPStr] Program name
** @attr ProgramVersion [AjPStr] Program version
** @attr ProgramFile [AjPStr] Program file
** @attr Parameters [AjPStr] Parameters
** @attr ModuleName [AjPStr] Module name
** @attr ModuleVersion [AjPStr] Module version
** @attr GFFSource [AjPStr] GFF source
** @attr GFFFeature [AjPStr] GFF feature
** @cc 'analysis_description' SQL table
** @attr Description [AjPStr] Description
** @attr DisplayLabel [AjPStr] Display label for the Ensembl website
** @attr WebData [AjPStr] Configuration information for the Ensembl web site
** @attr Displayable [AjBool] Displayable on the Ensembl web site
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSAnalysis {
    ajuint Use;
    ajuint Identifier;
    EnsPAnalysisAdaptor Adaptor;
    AjPStr CreationDate;
    AjPStr Name;
    AjPStr DatabaseName;
    AjPStr DatabaseVersion;
    AjPStr DatabaseFile;
    AjPStr ProgramName;
    AjPStr ProgramVersion;
    AjPStr ProgramFile;
    AjPStr Parameters;
    AjPStr ModuleName;
    AjPStr ModuleVersion;
    AjPStr GFFSource;
    AjPStr GFFFeature;
    AjPStr Description;
    AjPStr DisplayLabel;
    AjPStr WebData;
    AjBool Displayable;
    ajuint Padding;
} EnsOAnalysis;

#define EnsPAnalysis EnsOAnalysis*




/* @data EnsPExternalDatabaseAdaptor ******************************************
**
** Ensembl External Database Adaptor.
**
** @alias EnsSExternalDatabaseAdaptor
** @alias EnsOExternalDatabaseAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr CacheByIdentifier [AjPTable] Identifier cache
** @attr CacheByName [AjPTable] Name cache
** @@
******************************************************************************/

typedef struct EnsSExternalDatabaseAdaptor {
    EnsPDatabaseAdaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByName;
} EnsOExternalDatabaseAdaptor;

#define EnsPExternalDatabaseAdaptor EnsOExternalDatabaseAdaptor*




/* @datatype EnsEExternalDatabaseStatus ***************************************
**
** Ensembl External Database Status enumeration.
**
** @attr enum [EnsEExternalDatabaseStatus] Value
** @@
******************************************************************************/

enum EnsEExternalDatabaseStatus
{
    ensEExternalDatabaseStatusNULL,
    ensEExternalDatabaseStatusKnownXref,
    ensEExternalDatabaseStatusKnown,
    ensEExternalDatabaseStatusXref,
    ensEExternalDatabaseStatusPred,
    ensEExternalDatabaseStatusOrth,
    ensEExternalDatabaseStatusPseudo
};




/* @datatype EnsEExternalDatabaseType *****************************************
**
** Ensembl External Database Type enumeration.
**
** @attr enum [EnsEExternalDatabaseType] Value
** @@
******************************************************************************/

enum EnsEExternalDatabaseType
{
    ensEExternalDatabaseTypeNULL,
    ensEExternalDatabaseTypeArray,
    ensEExternalDatabaseTypeAltTrans,
    ensEExternalDatabaseTypeMisc,
    ensEExternalDatabaseTypeLit,
    ensEExternalDatabaseTypePrimaryDbSynonym,
    ensEExternalDatabaseTypeEnsembl
};




/* @data EnsPExternalDatabase *************************************************
**
** Ensembl External Database.
**
** @alias EnsSExternalDatabase
** @alias EnsOExternalDatabase
**
** @attr Use [ajuint] Use counter.
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key).
** @attr Adaptor [EnsPExternalDatabaseAdaptor] Ensembl External
**                                             Database Adaptor
** @cc 'external_db' table
** @attr Name [AjPStr] Database name
** @attr Release [AjPStr] Database release
** @attr DisplayName [AjPStr] Database display name
** @attr SecondaryName [AjPStr] Secondary database name
** @attr SecondaryTable [AjPStr] Secondary database table
** @attr Description [AjPStr] Description
** @attr PrimaryIdIsLinkable [AjBool] Primary identifier is linkable
** @attr DisplayIdIsLinkable [AjBool] Display identifier is linkable
** @attr Status [AjEnum] Status
** @attr Type [AjEnum] Type
** @attr Priority [ajint] Priority
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSExternalDatabase {
    ajuint Use;
    ajuint Identifier;
    EnsPExternalDatabaseAdaptor Adaptor;
    AjPStr Name;
    AjPStr Release;
    AjPStr DisplayName;
    AjPStr SecondaryName;
    AjPStr SecondaryTable;
    AjPStr Description;
    AjBool PrimaryIdIsLinkable;
    AjBool DisplayIdIsLinkable;
    AjEnum Status;
    AjEnum Type;
    ajint Priority;
    ajuint Padding;
} EnsOExternalDatabase;

#define EnsPExternalDatabase EnsOExternalDatabase*




/* @datatype EnsEExternalReferenceInfoType ************************************
**
** Ensembl External Reference InfoType enumeration.
**
** @attr enum [EnsEExternalReferenceInfoType] Value
** @@
******************************************************************************/

enum EnsEExternalReferenceInfoType
{
    ensEExternalReferenceInfoTypeNULL,
    ensEExternalReferenceInfoTypeProjection,
    ensEExternalReferenceInfoTypeMisc,
    ensEExternalReferenceInfoTypeDependent,
    ensEExternalReferenceInfoTypeDirect,
    ensEExternalReferenceInfoTypeSequenceMatch,
    ensEExternalReferenceInfoTypeInferrredPair,
    ensEExternalReferenceInfoTypeProbe,
    ensEExternalReferenceInfoTypeUnmapped,
    ensEExternalReferenceInfoTypeCoordinateOverlap
};




/* @data EnsPExternalReference ************************************************
**
** Ensembl External Reference.
**
** @alias EnsSExternalReference
** @alias EnsOExternalReference
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPExternalReferenceAdaptor] Ensembl External
**                                              Reference Adaptor
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @cc 'xref' table
** @attr ExternalDatabase [EnsPExternalDatabase] Ensembl External Database
** @attr PrimaryIdentifier [AjPStr] Primary identifier
** @attr DisplayIdentifier [AjPstr] Display identifier
** @attr Version [AjPStr] Version
** @attr Description [AjPStr] Description
** @attr LinkageAnnotation [AjPStr] Linkage annotation
** @attr InfoText [AjPStr] Information text
** @attr InfoType [AjEnum] Information type
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSExternalReference {
    ajuint Use;
    ajuint Identifier;
/*  EnsPExternalReferenceAdaptor Adaptor; */
    EnsPAnalysis Analysis;
    EnsPExternalDatabase ExternalDatabase;
    AjPStr PrimaryIdentifier;
    AjPStr DisplayIdentifier;
    AjPStr Version;
    AjPStr Description;
    AjPStr LinkageAnnotation;
    AjPStr InfoText;
    AjEnum InfoType;
    ajuint Padding;
} EnsOExternalReference;

#define EnsPExternalReference EnsOExternalReference*




/* @data EnsPIdentityReference ************************************************
**
** Ensembl Identity Reference.
**
** @alias EnsSIdentityReference
** @alias EnsOIdentityReference
**
** @cc Bio::EnsEMBL::IdentityXref
** @cc 'identity_xref' table
** @attr Cigar [AjPStr] Cigar line (See exonerate(1))
** @attr QueryStart [ajint] Query start
** @attr QueryEnd [ajint] Query end
** @attr QueryIdentity [ajint] Query sequence identity
** @attr TargetStart [ajint] Target start
** @attr TargetEnd [ajint] Target end
** @attr TargetIdentity [ajint] Target sequence identity
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @attr Evalue [double] e-Value
** @attr Score [double] Score
** @@
******************************************************************************/

typedef struct EnsSIdentityReference {
    AjPStr Cigar;
    ajint QueryStart;
    ajint QueryEnd;
    ajint QueryIdentity;
    ajint TargetStart;
    ajint TargetEnd;
    ajint TargetIdentity;
    ajuint Use;
    ajuint Padding;
    double Evalue;
    double Score;
} EnsOIdentityReference;

#define EnsPIdentityReference EnsOIdentityReference*




/* @data EnsPDatabaseEntryAdaptor *********************************************
**
** Ensembl Database Entry Adaptor.
**
** @alias EnsSDatabaseEntryAdaptor
** @alias EnsODatabaseEntryAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct EnsSDatabaseEntryAdaptor {
    EnsPDatabaseAdaptor Adaptor;
} EnsODatabaseEntryAdaptor;

#define EnsPDatabaseEntryAdaptor EnsODatabaseEntryAdaptor*




/* @data EnsPDatabaseEntry ****************************************************
**
** Ensembl Database Entry.
**
** @alias EnsSDatabaseEntry
** @alias EnsODatabaseEntry
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPDatabaseEntryAdaptor] Ensembl Database Entry Adaptor
** @cc Bio::EnsEMBL::DbEntry
** @cc 'xref' table
** @attr ExternalReference [EnsPExternalReference] Ensembl External Reference
** @cc Bio::EnsEMBL::IdentityXref
** @attr IdentityReference [EnsPIdentityReference] Ensembl Identity Reference
** @cc 'external_synonym' table
** @attr Synonyms [AjPList] Synonyms
** @attr GoLinkageTypes [AjPList] Gene Ontology linkage types
** @@
******************************************************************************/

typedef struct EnsSDatabaseEntry {
    ajuint Use;
    ajuint Identifier;
    EnsPDatabaseEntryAdaptor Adaptor;
    EnsPExternalReference ExternalReference;
    EnsPIdentityReference IdentityReference;
    AjPList Synonyms;
    AjPList GoLinkageTypes;
} EnsODatabaseEntry;

#define EnsPDatabaseEntry EnsODatabaseEntry*




/* @data EnsPGeneOntologyLinkage **********************************************
**
** Ensembl Gene Ontology Linkage.
**
** @alias EnsSGeneOntologyLinkage
** @alias EnsOGeneOntologyLinkage
**
** @cc 'go_xref' table
** @attr LinkageType [AjPStr] Likage type (Gene Ontology Evidence Code)
** @attr Source [EnsPDatabaseEntry] Source Ensembl Database Entry
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSGeneOntologyLinkage {
    AjPStr LinkageType;
    EnsPDatabaseEntry Source;
    ajuint Use;
    ajuint Padding;
} EnsOGeneOntologyLinkage;

#define EnsPGeneOntologyLinkage EnsOGeneOntologyLinkage*




/* @data EnsPFeature **********************************************************
**
** Ensembl Feature.
**
** @alias EnsSFeature
** @alias EnsOFeature
**
** @cc Bio::EnsEMBL::Feature
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr Slice [EnsPSlice] Ensembl Slice
** @attr SequenceName [AjPStr] Sequence name
** @attr Start [ajint] Start coordinate
** @attr End [ajint] End coordinate
** @attr Strand [ajint] Strand orientation
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct EnsSFeature {
    EnsPAnalysis Analysis;
    EnsPSlice Slice;
    AjPStr SequenceName;
    ajint Start;
    ajint End;
    ajint Strand;
    ajuint Use;
} EnsOFeature;

#define EnsPFeature EnsOFeature*




/* @data EnsPFeatureAdaptor ***************************************************
**
** Ensembl Feature Adaptor.
**
** @alias EnsSFeatureAdaptor
** @alias EnsOFeatureAdaptor
**
** @attr Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
** @attr Cache [EnsPCache] Ensembl LRU Cache
** @attr Tables [char**] Tables
** @attr Condition [char*] Default SQL condition
** @attr GetFeature [(EnsPFeature)] Ensembl Object-specific 'GetFeature'
**                                  element function
** @attr Reference [(void*)] Ensembl Object-specific referencing function
** @attr Delete [(void)] Ensembl Object-specific deletion function
** @attr MaxFeatureLength [ajuint] Maximum length of a particular Feature type
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSFeatureAdaptor {
    EnsPBaseAdaptor Adaptor;
    EnsPCache Cache;
    char **Tables;
    char *Condition;
    EnsPFeature (*GetFeature)(const void *object);
    void* (*Reference)(void* value);
    void (*Delete)(void** value);
    ajuint MaxFeatureLength;
    ajuint Padding;
} EnsOFeatureAdaptor;

#define EnsPFeatureAdaptor EnsOFeatureAdaptor*




/* @data EnsPExonAdaptor ******************************************************
**
** Ensembl Exon Adaptor.
**
** @alias EnsSExonAdaptor
** @alias EnsOExonAdaptor
**
** @cc Bio::EnsEMBL::DBSQL::BaseFeatureAdaptor
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSExonAdaptor {
    EnsPFeatureAdaptor Adaptor;
} EnsOExonAdaptor;

#define EnsPExonAdaptor EnsOExonAdaptor*




/* @data EnsPExon *************************************************************
**
** Ensembl Exon.
**
** @alias EnsSExon
** @alias EnsOExon
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPExonAdaptor] Ensembl Exon Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Exon
** @cc 'exon' SQL table
** @attr StartPhase [ajint] Exon start phase
** @attr EndPhase [ajint] Exon end phase
** @attr Current [AjBool] Current
** @attr Consitutive [AjBool] Consitutive
** @cc 'exon_stable_id' SQL table
** @attr StableIdentifier [AjPStr] Stable identifier
** @attr Version [ajuint] Version
** @attr Padding [ajuint] Padding to alignment boundary
** @attr CreationDate [AjPStr] Creation date
** @attr ModificationDate [AjPStr] Modification date
** @cc Additional elements
** @attr SequenceCache [AjPStr] Sequence Cache
** @attr SupportingFeatures [AjList] AJAX List of Ensembl Base Align Features
** @attr Coordinates [AjPTable] Exon Coordinates indexed by
**                              Ensembl Transcript identifiers
** @@
******************************************************************************/

typedef struct EnsSExon {
    ajuint Use;
    ajuint Identifier;
    EnsPExonAdaptor Adaptor;
    EnsPFeature Feature;
    ajint StartPhase;
    ajint EndPhase;
    AjBool Current;
    AjBool Constitutive;
    AjPStr StableIdentifier;
    ajuint Version;
    ajuint Padding;
    AjPStr CreationDate;
    AjPStr ModificationDate;
    AjPStr SequenceCache;
    AjPList SupportingFeatures;
    AjPTable Coordinates;
} EnsOExon;

#define EnsPExon EnsOExon*




/* @data EnsPIntron ***********************************************************
**
** Ensembl Intron.
**
** @alias EnsSIntron
** @alias EnsOIntron
**
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Intron
** @attr PreviousExon [EnsPExon] Previous Ensembl Exon
** @attr NextExon [EnsPExon] Next Ensembl Exon
** @attr Use [ajuint] Use counter
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSIntron {
    EnsPFeature Feature;
    EnsPExon PreviousExon;
    EnsPExon NextExon;
    ajuint Use;
    ajuint Padding;
} EnsOIntron;

#define EnsPIntron EnsOIntron*




/* @data EnsPTranscriptAdaptor ************************************************
**
** Ensembl Transcript Adaptor.
**
** @alias EnsSTranscriptAdaptor
** @alias EnsOTranscriptAdaptor
**
** @cc Bio::EnsEMBL::DBSQL::BaseFeatureAdaptor
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSTranscriptAdaptor {
    EnsPFeatureAdaptor Adaptor;
} EnsOTranscriptAdaptor;

#define EnsPTranscriptAdaptor EnsOTranscriptAdaptor*




/* @datatype EnsETranscriptStatus *********************************************
**
** Ensembl Transcript Status enumeration.
**
** @attr enum [EnsETranscriptStatus] Value
** @@
******************************************************************************/

enum EnsETranscriptStatus
{
    ensETranscriptStatusNULL,
    ensETranscriptStatusKnown,
    ensETranscriptStatusNovel,
    ensETranscriptStatusPutative,
    ensETranscriptStatusPredicted,
    ensETranscriptStatusKnownByProjection,
    ensETranscriptStatusUnknown
};




/* @data EnsPTranscript *******************************************************
**
** Ensembl Transcript.
**
** @alias EnsSTranscript
** @alias EnsOTranscript
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPTranscriptAdaptor] Ensembl Transcript Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Transcript
** @cc 'transcript' SQL table
** @attr DisplayReference [EnsPDatabaseEntry] Display External Reference
** @attr Description [AjPStr] Description
** @attr BioType [AjPStr] Biological type
** @attr Status [AjEnum] Status
** @attr Current [AjBool] Current attribute
** @cc 'transcript_stable_id' SQL table
** @attr Version [ajuint] Version
** @attr StableIdentifier [AjPStr] Stable identifier
** @attr CreationDate [AjPStr] Creation date
** @attr ModificationDate [AjPStr] Modification date
** @cc Additional elements
** @attr GeneIdentifier [ajuint] Ensembl Gene identifier
** @attr Attributes [AjPList] AJAX List of Ensembl Attributes
** @attr DatabaseEntries [AjPList] AJAX List of Ensembl Database Entries
** @attr Exons [AjPList] AJAX List of Ensembl Exons
** @attr SupportingFeatures [AjPList] AJAX List of Ensembl Base Align Features
** @attr Translations [AjPList] Ensembl Translations (weak references)
** @attr SliceCodingStart [ajuint] Coding start in Slice coordinates
** @attr SliceCodingEnd [ajuint] Coding end in Slice coordinates
** @attr TranscriptCodingStart [ajuint] Coding start in Transcript coordinates
** @attr TranscriptCodingEnd [ajuint] Coding end in Transcript coordinates
** @attr EditsEnabled [AjBool] Sequence Edits are enabled
** @attr StartPhase [ajint] Start phase
** @cc Bio::EnsEMBL::TranscriptMapper
** @attr ExonCoordMapper [EnsPMapper] Ensembl Mapper
** @@
** The Attribute AJAX Table is a first-level Table of Ensembl Attribute code
** AJAX String keys and AjPTable second-level AJAX Lists as value.
******************************************************************************/

typedef struct EnsSTranscript {
    ajuint Use;
    ajuint Identifier;
    EnsPTranscriptAdaptor Adaptor;
    EnsPFeature Feature;
    EnsPDatabaseEntry DisplayReference;
    AjPStr Description;
    AjPStr BioType;
    AjEnum Status;
    AjBool Current;
    AjPStr StableIdentifier;
    AjPStr CreationDate;
    AjPStr ModificationDate;
    ajuint Version;
    ajuint GeneIdentifier;
    AjPList Attributes;
    AjPList DatabaseEntries;
    AjPList Exons;
    AjPList SupportingFeatures;
    AjPList Translations;
    ajuint SliceCodingStart;
    ajuint SliceCodingEnd;
    ajuint TranscriptCodingStart;
    ajuint TranscriptCodingEnd;
    AjBool EditsEnabled;
    ajint StartPhase;
    EnsPMapper ExonCoordMapper;
} EnsOTranscript;

#define EnsPTranscript EnsOTranscript*




/* @data EnsPTranslationAdaptor ***********************************************
**
** Ensembl Translation Adaptor.
**
** @alias EnsSTranslationAdaptor
** @alias EnsOTranslationAdaptor
**
** @cc Bio::EnsEMBL::DBSQL::BaseAdaptor
** @attr Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

typedef struct EnsSTranslationAdaptor {
    EnsPBaseAdaptor Adaptor;
} EnsOTranslationAdaptor;

#define EnsPTranslationAdaptor EnsOTranslationAdaptor*




/* @data EnsPTranslation ******************************************************
**
** Ensembl Translation.
**
** @alias EnsSTranslation
** @alias EnsOTranslation
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPTranscriptAdaptor] Ensembl Transcript Adaptor
** @cc Bio::EnsEMBL::Translation
** @cc SQL table 'translation' elements
** @attr Transcript [EnsPTranscript] Ensembl Transcript
** @attr StartExon [EnsPExon] Ensembl Exon in which the Translation
**                            start coordinate is annotated
** @attr EndExon [EnsPExon] Ensembl Exon in which the Translation
**                          end coordinate is annotated
** @attr Start [ajuint] Start coordinate relative to start Ensembl Exon
** @attr End [ajuint] End coordinate relative to end Ensembl Exon
** @cc  SQL table 'translation_stable_id' elements
** @attr StableIdentifier [AjPStr] Stable identifier
** @attr CreationDate [AjPStr] Creation date
** @attr ModificationDate [AjPStr] Modification date
** @attr Version [ajuint] Version
** @attr Padding [ajuint] Padding to alignment boundary
** @cc Additional elements
** @attr Attributes [AjPList] AJAX List of Ensembl Attributes
** @attr DatabaseEntries [AjPList] AJAX List of Ensembl Database Entries
** @attr ProteinFeatures [AjPList] AJAX List of Ensembl Protein Features
** @attr Sequence [AjPStr] Translation sequence
** @attr TranscriptStart [ajuint] Start coordinate relative to
**                                Ensembl Transcript (cDNA)
** @attr TranscriptEnd [ajuint] End coordinate relative to
**                              Ensembl Transcript (cDNA)
** @attr SliceStart [ajuint] Start coordinate relative to the Slice
** @attr SliceEnd [ajuint] End coordinate relative to the Slice
** @@
******************************************************************************/

typedef struct EnsSTranslation {
    ajuint Use;
    ajuint Identifier;
    EnsPTranslationAdaptor Adaptor;
    EnsPTranscript Transcript;
    EnsPExon StartExon;
    EnsPExon EndExon;
    ajuint Start;
    ajuint End;
    AjPStr StableIdentifier;
    AjPStr CreationDate;
    AjPStr ModificationDate;
    ajuint Version;
    ajuint Padding;
    AjPList Attributes;
    AjPList DatabaseEntries;
    AjPList ProteinFeatures;
    AjPStr Sequence;
    ajuint TranscriptStart;
    ajuint TranscriptEnd;
    ajuint SliceStart;
    ajuint SliceEnd;
} EnsOTranslation;

#define EnsPTranslation EnsOTranslation*




/* @data EnsPGeneAdaptor ******************************************************
**
** Ensembl Gene Adaptor.
**
** @alias EnsSGeneAdaptor
** @alias EnsOGeneAdaptor
**
** @cc Bio::EnsEMBL::DBSQL::BaseFeatureAdaptor
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSGeneAdaptor {
    EnsPFeatureAdaptor Adaptor;
} EnsOGeneAdaptor;

#define EnsPGeneAdaptor EnsOGeneAdaptor*




/* @datatype EnsEGeneStatus ***************************************************
**
** Ensembl Gene Status enumeration.
**
** @attr enum [EnsEGeneStatus] Value
** @@
******************************************************************************/

enum EnsEGeneStatus
{
    ensEGeneStatusNULL,
    ensEGeneStatusKnown,
    ensEGeneStatusNovel,
    ensEGeneStatusPutative,
    ensEGeneStatusPredicted,
    ensEGeneStatusKnownByProjection,
    ensEGeneStatusUnknown
};




/* @data EnsPGene *************************************************************
**
** Ensembl Gene.
**
** @alias EnsSGene
** @alias EnsOGene
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPGeneAdaptor] Ensembl Gene Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Gene
** @cc 'gene' SQL table
** @attr DisplayReference [EnsPDatabaseEntry] Display External Reference
** @attr Description [AjPStr] Description
** @attr Source [AjPStr] Source
** @attr BioType [AjPStr] Biological type
** @attr Status [AjEnum] Status
** @attr Current [AjBool] Current attribute
** @cc 'gene_stable_id' SQL table
** @attr StableIdentifier [AjPStr] Stable identifier
** @attr CreationDate [AjPStr] Creation date
** @attr ModificationDate [AjPStr] Modification date
** @attr Version [ajuint] Version
** @attr Padding [ajuint] Padding to alignment boundary
** @cc
** @attr Attributes [AjPList] AJAX List of Ensembl Attributes
** @attr DatabaseEntries [AjPList] AJAX List of Ensembl Database Entries
** @attr Transcripts [AjPList] AJAX List of Ensembl Transcripts
** @@
******************************************************************************/

typedef struct EnsSGene {
    ajuint Use;
    ajuint Identifier;
    EnsPGeneAdaptor Adaptor;
    EnsPFeature Feature;
    EnsPDatabaseEntry DisplayReference;
    AjPStr Description;
    AjPStr Source;
    AjPStr BioType;
    AjEnum Status;
    AjBool Current;
    AjPStr StableIdentifier;
    AjPStr CreationDate;
    AjPStr ModificationDate;
    ajuint Version;
    ajuint Padding;
    AjPList Attributes;
    AjPList DatabaseEntries;
    AjPList Transcripts;
} EnsOGene;

#define EnsPGene EnsOGene*




#endif

#ifdef __cplusplus
}
#endif
