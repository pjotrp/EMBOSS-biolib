#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensqcdatabase_h
#define ensqcdatabase_h

#include "ensanalysis.h"




/* @data EnsPQCDatabaseAdaptor ************************************************
**
** Ensembl QC Database Adaptor
**
** @alias EnsSQCDatabaseAdaptor
** @alias EnsOQCDatabaseAdaptor
**
** @attr Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
** @attr CacheByIdentifier [AjPTable] Identifier cache
** @attr CacheByName [AjPTable] Name cache
** @@
******************************************************************************/

typedef struct EnsSQCDatabaseAdaptor
{
    EnsPBaseAdaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByName;
} EnsOQCDatabaseAdaptor;

#define EnsPQCDatabaseAdaptor EnsOQCDatabaseAdaptor*




/* @datatype EnsEQCDatabaseClass **********************************************
**
** Ensembl QC Database Class enumeration
**
** @attr enum [EnsEQCDatabaseClass] Value
** @@
******************************************************************************/

enum EnsEQCDatabaseClass
{
    ensEQCDatabaseClassNULL,
    ensEQCDatabaseClassUnknown,
    ensEQCDatabaseClassReference,
    ensEQCDatabaseClassTest,
    ensEQCDatabaseClassBoth,
    ensEQCDatabaseClassGenome
};




/* @datatype EnsEQCDatabaseType ***********************************************
**
** Ensembl QC Database Type enumeration
**
** @attr enum [EnsEQCDatabaseType] Value
** @@
******************************************************************************/

enum EnsEQCDatabaseType
{
    ensEQCDatabaseTypeNULL,
    ensEQCDatabaseTypeUnknown,
    ensEQCDatabaseTypeDNA,
    ensEQCDatabaseTypeProtein
};




/* @data EnsPQCDatabase *******************************************************
**
** Ensembl QC Database
**
** @alias EnsSQCDatabase
** @alias EnsOQCDatabase
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQCDatabaseAdaptor] Ensembl Database Adaptor
** @cc Bio::EnsEMBL::QC::SequenceDB
** @cc 'sequence_db' SQL table
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr Name [AjPStr] Name
** @attr Release [AjPStr] Release
** @attr Date [AjPStr] Date
** @attr Format [AjPStr] Format
** @attr Class [AjEnum] Class
** @attr Type [AjEnum] Type
** @attr Species [AjPStr] Registry species
** @attr Group [AjEnum] Ensembl Database Adaptor group
** @attr Padding [ajuint] Padding to alignment boundary
** @attr Host [AjPStr] Host
** @attr Directory [AjPStr] Directory
** @attr File [AjPStr] File
** @attr ExternalURL [AjPStr] External URL
** @attr InternalURL [AjPStr] Internal URL
** @@
******************************************************************************/

typedef struct EnsSQCDatabase
{
    ajuint Use;
    ajuint Identifier;
    EnsPQCDatabaseAdaptor Adaptor;
    EnsPAnalysis Analysis;
    AjPStr Name;
    AjPStr Release;
    AjPStr Date;
    AjPStr Format;
    AjEnum Class;
    AjEnum Type;
    AjPStr Species;
    AjEnum Group;
    ajuint Padding;
    AjPStr Host;
    AjPStr Directory;
    AjPStr File;
    AjPStr ExternalURL;
    AjPStr InternalURL;
} EnsOQCDatabase;

#define EnsPQCDatabase EnsOQCDatabase*




/*
** Prototype definitions
*/

/* Ensembl Quality Check Database */

EnsPQCDatabase ensQCDatabaseNew(EnsPQCDatabaseAdaptor adaptor,
                                ajuint identifier,
                                EnsPAnalysis analysis,
                                AjPStr name,
                                AjPStr release,
                                AjPStr date,
                                AjPStr format,
                                AjEnum class,
                                AjEnum type,
                                AjPStr species,
                                AjEnum group,
                                AjPStr host,
                                AjPStr directory,
                                AjPStr file,
                                AjPStr externalurl);

EnsPQCDatabase ensQCDatabaseNewObj(const EnsPQCDatabase object);

EnsPQCDatabase ensQCDatabaseNewRef(EnsPQCDatabase qcdb);

void ensQCDatabaseDel(EnsPQCDatabase* Pqcdb);

EnsPQCDatabaseAdaptor ensQCDatabaseGetAdaptor(const EnsPQCDatabase qcdb);

ajuint ensQCDatabaseGetIdentifier(const EnsPQCDatabase qcdb);

const EnsPAnalysis ensQCDatabaseGetAnalysis(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetName(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetRelease(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetDate(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetFormat(const EnsPQCDatabase qcdb);

AjEnum ensQCDatabaseGetClass(const EnsPQCDatabase qcdb);

AjEnum ensQCDatabaseGetType(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetSpecies(const EnsPQCDatabase qcdb);

AjEnum ensQCDatabaseGetGroup(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetHost(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetDirectory(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetFile(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetExternalURL(const EnsPQCDatabase qcdb);

AjPStr ensQCDatabaseGetInternalURL(const EnsPQCDatabase qcdb);

AjBool ensQCDatabaseSetAdaptor(EnsPQCDatabase qcdb,
                               EnsPQCDatabaseAdaptor qcdba);

AjBool ensQCDatabaseSetIdentifier(EnsPQCDatabase qcdb, ajuint identifier);

AjBool ensQCDatabaseSetAnalysis(EnsPQCDatabase qcdb, EnsPAnalysis analysis);

AjBool ensQCDatabaseSetName(EnsPQCDatabase qcdb, AjPStr name);

AjBool ensQCDatabaseSetRelease(EnsPQCDatabase qcdb, AjPStr release);

AjBool ensQCDatabaseSetDate(EnsPQCDatabase qcdb, AjPStr date);

AjBool ensQCDatabaseSetFormat(EnsPQCDatabase qcdb, AjPStr format);

AjBool ensQCDatabaseSetClass(EnsPQCDatabase qcdb, AjEnum class);

AjBool ensQCDatabaseSetType(EnsPQCDatabase qcdb, AjEnum type);

AjBool ensQCDatabaseSetSpecies(EnsPQCDatabase qcdb, AjPStr species);

AjBool ensQCDatabaseSetGroup(EnsPQCDatabase qcdb, AjEnum group);

AjBool ensQCDatabaseSetHost(EnsPQCDatabase qcdb, AjPStr host);

AjBool ensQCDatabaseSetDirectory(EnsPQCDatabase qcdb, AjPStr directory);

AjBool ensQCDatabaseSetFile(EnsPQCDatabase qcdb, AjPStr file);

AjBool ensQCDatabaseSetExternalURL(EnsPQCDatabase qcdb, AjPStr url);

AjBool ensQCDatabaseSetInternalURL(EnsPQCDatabase qcdb, AjPStr url);

ajuint ensQCDatabaseGetMemSize(const EnsPQCDatabase qcdb);

AjBool ensQCDatabaseTrace(const EnsPQCDatabase qcdb, ajuint level);

AjEnum ensQCDatabaseClassFromStr(const AjPStr class);

AjEnum ensQCDatabaseTypeFromStr(const AjPStr type);

const char *ensQCDatabaseClassToChar(const AjEnum class);

const char *ensQCDatabaseTypeToChar(const AjEnum type);

AjBool ensQCDatabaseMatch(const EnsPQCDatabase qcdb1,
                          const EnsPQCDatabase qcdb2);

/* Ensembl Quality Check Database Adaptor */

EnsPQCDatabaseAdaptor ensQCDatabaseAdaptorNew(EnsPDatabaseAdaptor dba);

void ensQCDatabaseAdaptorDel(EnsPQCDatabaseAdaptor* Pqcdba);

AjBool ensQCDatabaseAdaptorFetchByIdentifier(EnsPQCDatabaseAdaptor qcdba,
                                             ajuint identifier,
                                             EnsPQCDatabase *Pqcdb);

AjBool ensQCDatabaseAdaptorFetchByName(EnsPQCDatabaseAdaptor qcdba,
                                       const AjPStr name,
                                       const AjPStr release,
                                       EnsPQCDatabase *Pqcdb);

AjBool ensQCDatabaseAdaptorFetchAllByClassType(EnsPQCDatabaseAdaptor qcdba,
                                               AjEnum class,
                                               AjEnum type,
                                               AjPList qcdbs);

AjBool ensQCDatabaseAdaptorStore(EnsPQCDatabaseAdaptor qcdba,
                                 EnsPQCDatabase qcdb);

AjBool ensQCDatabaseAdaptorUpdate(EnsPQCDatabaseAdaptor qcdba,
                                  const EnsPQCDatabase qcdb);

AjBool ensQCDatabaseAdaptorDelete(EnsPQCDatabaseAdaptor qcdba,
                                  const EnsPQCDatabase qcdb);


/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
