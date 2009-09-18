#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensmarker_h
#define ensmarker_h

#include "ensfeature.h"




/* @data EnsPMarkerSynonym ****************************************************
**
** Ensembl Marker Synonym
**
** @alias EnsSMarkerSynonym
** @alias EnsOMarkerSynonym
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Map::MarkerSynonym
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Source [AjPStr] Source
** @attr Name [AjPStr] Name
** @@
******************************************************************************/

typedef struct EnsSMarkerSynonym
{
    ajuint Use;
    ajuint Identifier;
    AjPStr Source;
    AjPStr Name;
} EnsOMarkerSynonym;

#define EnsPMarkerSynonym EnsOMarkerSynonym*




/* @data EnsPMarkerMapLocation ************************************************
**
** Ensembl Marker Map Location
**
** @alias EnsSMarkerMapLocation
** @alias EnsOMarkerMapLocation
**
** @cc Bio::EnsEMBL::Map::MapLocation
** @attr MarkerSynonym [EnsPMarkerSynonym] Ensembl Marker Synonym
** @attr MapName [AjPStr] Map name
** @attr ChromosomeName [AjPStr] Chromosome name
** @attr Position [AjPStr] Position
** @attr LODScore [float] LOD score
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct EnsSMarkerMapLocation
{
    EnsPMarkerSynonym MarkerSynonym;
    AjPStr MapName;
    AjPStr ChromosomeName;
    AjPStr Position;
    float LODScore;
    ajuint Use;
} EnsOMarkerMapLocation;

#define EnsPMarkerMapLocation EnsOMarkerMapLocation*




/* @data EnsPMarkerAdaptor ****************************************************
**
** Ensembl Marker Adaptor
**
** @alias EnsSMarkerAdaptor
** @alias EnsOMarkerAdaptor
**
** @attr Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

typedef struct EnsSMarkerAdaptor
{
    EnsPBaseAdaptor Adaptor;
} EnsOMarkerAdaptor;

#define EnsPMarkerAdaptor EnsOMarkerAdaptor*




/* @datatype EnsEMarkerType ***************************************************
**
** Ensembl Marker Type enumeration.
**
** @attr enum [EnsEMarkerType] Value
** @@
******************************************************************************/

enum EnsEMarkerType
{
    ensEMarkerTypeNULL,
    ensEMarkerTypeEST,
    ensEMarkerTypeMicroSatellite
};




/* @data EnsPMarker ***********************************************************
**
** Ensembl Marker
**
** @alias EnsSMarker
** @alias EnsOMarker
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @cc Bio::EnsEMBL::Map::Marker
** @attr DisplaySynonym [EnsPMarkerSynonym] Display Ensembl Marker Synonym
** @attr LeftPrimer [AjPStr] Left primer
** @attr RightPrimer [AjPStr] Right primer
** @attr MarkerSynonyms [AjPList] AJAX List of Ensembl Marker Synonyms
** @attr MarkerMapLocations [AjPList] AJAX List of Ensembl Marker Map Locations
** @attr MinimumDistance [ajuint] Minimum primer distance
** @attr MaximumDistance [ajuint] Maximum primer distance
** @attr Type [AjEnum] Type
** @attr Priority [ajint] Priority
** @@
******************************************************************************/

typedef struct EnsSMarker
{
    ajuint Use;
    ajuint Identifier;
    EnsPMarkerAdaptor Adaptor;
    EnsPMarkerSynonym DisplaySynonym;
    AjPStr LeftPrimer;
    AjPStr RightPrimer;
    AjPList MarkerSynonyms;
    AjPList MarkerMapLocations;
    ajuint MinimumDistance;
    ajuint MaximumDistance;
    AjEnum Type;
    ajint Priority;
} EnsOMarker;

#define EnsPMarker EnsOMarker*




/* @data EnsPMarkerFeatureAdaptor *********************************************
**
** Ensembl Marker Feature Adaptor.
**
** @alias EnsSMarkerFeatureAdaptor
** @alias EnsOMarkerFeatureAdaptor
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerFeatureAdaptor
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSMarkerFeatureAdaptor
{
    EnsPFeatureAdaptor Adaptor;
} EnsOMarkerFeatureAdaptor;

#define EnsPMarkerFeatureAdaptor EnsOMarkerFeatureAdaptor*




/* @data EnsPMarkerFeature ****************************************************
**
** Ensembl Marker Feature
**
** @alias EnsSMarkerFeature
** @alias EnsOMarkerFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Map::MarkerFeature
** @attr Marker [EnsPMarker] Ensembl Marker
** @attr MapWeight [ajint] Map weight
** @attr Padding [ajint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSMarkerFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPMarkerFeatureAdaptor Adaptor;
    EnsPFeature Feature;
    EnsPMarker Marker;
    ajint MapWeight;
    ajint Padding;
} EnsOMarkerFeature;

#define EnsPMarkerFeature EnsOMarkerFeature*




/*
** Prototype definitions
*/

/* Ensembl Marker Synonym */

EnsPMarkerSynonym ensMarkerSynonymNew(ajuint identifier,
                                      AjPStr source,
                                      AjPStr name);

EnsPMarkerSynonym ensMarkerSynonymNewObj(const EnsPMarkerSynonym object);

EnsPMarkerSynonym ensMarkerSynonymNewRef(EnsPMarkerSynonym ms);

void ensMarkerSynonymDel(EnsPMarkerSynonym* Pms);

ajuint ensMarkerSynonymGetIdentifier(const EnsPMarkerSynonym ms);

AjPStr ensMarkerSynonymGetSource(const EnsPMarkerSynonym ms);

AjPStr ensMarkerSynonymGetName(const EnsPMarkerSynonym ms);

AjBool ensMarkerSynonymSetIdentifier(EnsPMarkerSynonym ms, ajuint identifier);

AjBool ensMarkerSynonymSetSource(EnsPMarkerSynonym ms, AjPStr source);

AjBool ensMarkerSynonymSetName(EnsPMarkerSynonym ms, AjPStr name);

AjBool ensMarkerSynonymTrace(const EnsPMarkerSynonym ms, ajuint level);

ajuint ensMarkerSynonymGetMemSize(const EnsPMarkerSynonym ms);

/* Ensembl Marker Synonym Adaptor */

AjBool ensMarkerSynonymAdaptorFetchByIdentifier(EnsPDatabaseAdaptor dba,
                                                ajuint identifier,
                                                EnsPMarkerSynonym *Pms);

AjBool ensMarkerSynonymAdaptorFetchAllByMarkerIdentifier(
    EnsPDatabaseAdaptor dba,
    ajuint markerid,
    AjPList mslist);

/* Ensembl Marker Map Location */

EnsPMarkerMapLocation ensMarkerMapLocationNew(EnsPMarkerSynonym ms,
                                              AjPStr mapname,
                                              AjPStr chrname,
                                              AjPStr position,
                                              float lodscore);

EnsPMarkerMapLocation ensMarkerMapLocationNewObj(
    const EnsPMarkerMapLocation object);

EnsPMarkerMapLocation ensMarkerMapLocationNewRef(EnsPMarkerMapLocation mml);

void ensMarkerMapLocationDel(EnsPMarkerMapLocation* Pmml);

EnsPMarkerSynonym ensMarkerMapLocationGetMarkerSynonym(
    const EnsPMarkerMapLocation mml);

AjPStr ensMarkerMapLocationGetMapName(const EnsPMarkerMapLocation mml);

AjPStr ensMarkerMapLocationGetChromosomeName(const EnsPMarkerMapLocation mml);

AjPStr ensMarkerMapLocationGetPosition(const EnsPMarkerMapLocation mml);

float ensMarkerMapLocationGetLODScore(const EnsPMarkerMapLocation mml);

AjBool ensMarkerMapLocationSetMarkerSynonym(EnsPMarkerMapLocation mml,
				     EnsPMarkerSynonym ms);

AjBool ensMarkerMapLocationSetMapName(EnsPMarkerMapLocation mml,
                                      AjPStr mapname);

AjBool ensMarkerMapLocationSetChromosomeName(EnsPMarkerMapLocation mml,
                                             AjPStr chrname);

AjBool ensMarkerMapLocationSetPosition(EnsPMarkerMapLocation mml,
                                       AjPStr position);

AjBool ensMarkerMapLocationSetLODScore(EnsPMarkerMapLocation mml,
                                       float lodscore);

AjBool ensMarkerMapLocationTrace(const EnsPMarkerMapLocation mml, ajuint level);

ajuint ensMarkerMapLocationGetMemSize(const EnsPMarkerMapLocation mml);

/* Ensembl Marker Map Location Adaptor */

AjBool ensMarkerMapLocationAdaptorFetchAllByMarkerIdentifier(
    EnsPDatabaseAdaptor dba,
    ajuint markerid,
    AjPList mmlist);

/* Ensembl Marker */

EnsPMarker ensMarkerNew(EnsPMarkerAdaptor adaptor,
                        ajuint identifier,
                        EnsPMarkerSynonym display,
                        AjPStr lprimer,
                        AjPStr rprimer,
                        AjPList synonyms,
                        AjPList locations,
                        ajuint mindistance,
                        ajuint maxdistance,
                        AjEnum type,
                        ajint priority);

EnsPMarker ensMarkerNewObj(const EnsPMarker object);

EnsPMarker ensMarkerNewRef(EnsPMarker marker);

void ensMarkerDel(EnsPMarker* Pmarker);

EnsPMarkerAdaptor ensMarkerGetAdaptor(const EnsPMarker marker);

ajuint ensMarkerGetIdentifier(const EnsPMarker marker);

EnsPMarkerSynonym ensMarkerGetDisplaySynonym(const EnsPMarker marker);

AjPStr ensMarkerGetLeftPrimer(const EnsPMarker marker);

AjPStr ensMarkerGetRightPrimer(const EnsPMarker marker);

const AjPList ensMarkerGetMarkerSynonyms(EnsPMarker marker);

const AjPList ensMarkerGetMarkerMapLocations(EnsPMarker marker);

ajuint ensMarkerGetMinimumDistance(const EnsPMarker marker);

ajuint ensMarkerGetMaximumDistance(const EnsPMarker marker);

AjEnum ensMarkerGetType(const EnsPMarker marker);

ajint ensMarkerGetPriority(const EnsPMarker marker);

AjBool ensMarkerSetAdaptor(EnsPMarker marker, EnsPMarkerAdaptor adaptor);

AjBool ensMarkerSetIdentifier(EnsPMarker marker, ajuint identifier);

AjBool ensMarkerSetLeftPrimer(EnsPMarker marker, AjPStr lprimer);

AjBool ensMarkerSetRightPrimer(EnsPMarker marker, AjPStr rprimer);

AjBool ensMarkerSetDisplaySynonym(EnsPMarker marker, EnsPMarkerSynonym display);

AjBool ensMarkerSetMinimumDistance(EnsPMarker marker, ajuint mindistance);

AjBool ensMarkerSetMaximumDistance(EnsPMarker marker, ajuint maxdistance);

AjBool ensMarkerSetType(EnsPMarker marker, AjEnum type);

AjBool ensMarkerSetPriority(EnsPMarker marker, ajint priority);

AjBool ensMarkerTrace(const EnsPMarker marker, ajuint level);

ajuint ensMarkerGetMemSize(const EnsPMarker marker);

AjBool ensMarkerAddMarkerSynonym(EnsPMarker marker, EnsPMarkerSynonym ms);

AjBool ensMarkerAddMarkerMapLocation(EnsPMarker marker,
                                     EnsPMarkerMapLocation mml);

AjEnum ensMarkerTypeFromStr(const AjPStr type);

const char *ensMarkerTypeToChar(const AjEnum type);

AjBool ensMarkerClearMarkerSynonyms(EnsPMarker marker);

AjBool ensMarkerClearMarkerMapLocations(EnsPMarker marker);

/* Ensembl Marker Adaptor */

EnsPMarkerAdaptor ensMarkerAdaptorNew(EnsPDatabaseAdaptor dba);

void ensMarkerAdaptorDel(EnsPMarkerAdaptor* Padaptor);

EnsPBaseAdaptor ensMarkerAdaptorGetBaseAdaptor(const EnsPMarkerAdaptor adaptor);

AjBool ensMarkerAdaptorFetchAll(EnsPMarkerAdaptor adaptor, AjPList markers);

AjBool ensMarkerAdaptorFetchByIdentifier(EnsPMarkerAdaptor adaptor,
                                         ajuint identifier,
                                         EnsPMarker *Pmarker);

AjBool ensMarkerAdaptorFetchAllBySynonym(EnsPMarkerAdaptor ma,
                                         const AjPStr name,
                                         const AjPStr source,
                                         AjPList markers);

__deprecated AjBool ensMarkerAdaptorFetchAttributes(EnsPMarkerAdaptor adaptor,
                                                    EnsPMarker marker);

/* Ensembl Marker Feature */

EnsPMarkerFeature ensMarkerFeatureNew(EnsPMarkerFeatureAdaptor adaptor,
                                      ajuint identifier,
                                      EnsPFeature feature,
                                      EnsPMarker marker,
                                      ajint mapweight);

EnsPMarkerFeature ensMarkerFeatureNewObj(const EnsPMarkerFeature object);

EnsPMarkerFeature ensMarkerFeatureNewRef(EnsPMarkerFeature mf);

void ensMarkerFeatureDel(EnsPMarkerFeature* Pmf);

EnsPMarkerFeatureAdaptor ensMarkerFeatureGetAdaptor(const EnsPMarkerFeature mf);

ajuint ensMarkerFeatureGetIdentifier(const EnsPMarkerFeature mf);

EnsPFeature ensMarkerFeatureGetFeature(const EnsPMarkerFeature mf);

EnsPMarker ensMarkerFeatureGetMarker(const EnsPMarkerFeature mf);

ajint ensMarkerFeatureGetMapWeight(const EnsPMarkerFeature mf);

AjBool ensMarkerFeatureSetAdaptor(EnsPMarkerFeature mf,
                                  EnsPMarkerFeatureAdaptor adaptor);

AjBool ensMarkerFeatureSetIdentifier(EnsPMarkerFeature mf, ajuint identifier);

AjBool ensMarkerFeatureSetFeature(EnsPMarkerFeature mf, EnsPFeature feature);

AjBool ensMarkerFeatureSetMarker(EnsPMarkerFeature mf, EnsPMarker marker);

AjBool ensMarkerFeatureSetMapWeight(EnsPMarkerFeature mf, ajint mapweight);

AjBool ensMarkerFeatureTrace(const EnsPMarkerFeature mf, ajuint level);

ajuint ensMarkerFeatureGetMemSize(const EnsPMarkerFeature mf);

/* Ensembl Marker Feature Adaptor */

EnsPMarkerFeatureAdaptor ensMarkerFeatureAdaptorNew(EnsPDatabaseAdaptor dba);

void ensMarkerFeatureAdaptorDel(EnsPMarkerFeatureAdaptor *Padaptor);

AjBool ensMarkerFeatureAdaptorFetchAllByMarker(EnsPMarkerFeatureAdaptor adaptor,
                                               const EnsPMarker marker,
                                               AjPList mflist);

AjBool ensMarkerFeatureAdaptorFetchAllBySlice(EnsPMarkerFeatureAdaptor adaptor,
                                              EnsPSlice slice,
                                              ajint priority,
                                              ajuint mapweight,
                                              const AjPStr anname,
                                              AjPList mflist);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
