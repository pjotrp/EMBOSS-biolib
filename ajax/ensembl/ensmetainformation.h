#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensmetainformation_h
#define ensmetainformation_h

#include "ensdatabaseadaptor.h"
#include "enstable.h"




/* @data EnsPMetaInformationAdaptor *******************************************
**
** Ensembl Meta-Information Adaptor.
**
** @alias EnsSMetaInformationAdaptor
** @alias EnsOMetaInformationAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor.
** @attr CacheByIdentifier [AjPTable] Cache by Meta-Information SQL identifier.
** @attr CacheByKey [AjPTable] Cache by Meta-Information key data.
** @@
******************************************************************************/

typedef struct EnsSMetaInformationAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByKey;
} EnsOMetaInformationAdaptor;

#define EnsPMetaInformationAdaptor EnsOMetaInformationAdaptor*




/* @data EnsPMetaInformation **************************************************
**
** Ensembl Meta-Information.
**
** @alias EnsSMetaInformation
** @alias EnsOMetaInformation
**
** @attr Use [ajuint] Use counter.
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL Database-internal identifier.
** @attr Adaptor [EnsPMetaInformationAdaptor] Ensembl Meta-Information Adaptor.
** @cc Bio::EnsEMBL::MetaContainer
** @attr Key [AjPStr] Key.
** @attr Value [AjPStr] Value.
** @attr Species [ajuint] Species identifier.
** @attr Padding [ajuint] Padding to alignment boundary.
** @@
******************************************************************************/

typedef struct EnsSMetaInformation
{
    ajuint Use;
    ajuint Identifier;
    EnsPMetaInformationAdaptor Adaptor;
    AjPStr Key;
    AjPStr Value;
    ajuint Species;
    ajuint Padding;
} EnsOMetaInformation;

#define EnsPMetaInformation EnsOMetaInformation*




/*
** Prototype definitions
*/

/* Ensembl Meta-Information */

EnsPMetaInformation ensMetaInformationNew(EnsPMetaInformationAdaptor adaptor,
                                          ajuint identifier,
                                          ajuint species,
                                          AjPStr key,
                                          AjPStr value);

EnsPMetaInformation ensMetaInformationNewObj(EnsPMetaInformation object);

EnsPMetaInformation ensMetaInformationNewRef(EnsPMetaInformation mi);

void ensMetaInformationDel(EnsPMetaInformation* Pmi);

EnsPMetaInformationAdaptor ensMetaInformationGetAdaptor(
    const EnsPMetaInformation mi);

ajuint ensMetaInformationGetIdentifier(const EnsPMetaInformation mi);

AjPStr ensMetaInformationGetKey(const EnsPMetaInformation mi);

AjPStr ensMetaInformationGetValue(const EnsPMetaInformation mi);

ajuint ensMetaInformationGetSpecies(const EnsPMetaInformation mi);

AjBool ensMetaInformationSetAdaptor(EnsPMetaInformation mi,
                                    EnsPMetaInformationAdaptor adaptor);

AjBool ensMetaInformationSetIdentifier(EnsPMetaInformation mi,
                                       ajuint identifier);

AjBool ensMetaInformationSetKey(EnsPMetaInformation mi, AjPStr key);

AjBool ensMetaInformationSetValue(EnsPMetaInformation mi, AjPStr value);

AjBool ensMetaInformationSetSpecies(EnsPMetaInformation mi, ajuint species);

AjBool ensMetaInformationTrace(const EnsPMetaInformation mi, ajuint level);

ajuint ensMetaInformationGetMemSize(const EnsPMetaInformation mi);

/* Ensembl Meta-Information Adaptor */

EnsPMetaInformationAdaptor ensMetaInformationAdaptorNew(
    EnsPDatabaseAdaptor dba);

void ensMetaInformationAdaptorDel(EnsPMetaInformationAdaptor* Padaptor);

AjBool ensMetaInformationAdaptorFetchAllByKey(
    const EnsPMetaInformationAdaptor adaptor,
    const AjPStr key,
    AjPList mis);

AjBool ensMetaInformationAdaptorGetValueByKey(
    const EnsPMetaInformationAdaptor adaptor,
				       const AjPStr key,
				       AjPStr *Pvalue);

AjBool ensMetaInformationAdaptorGetGenebuildVersion(
    const EnsPMetaInformationAdaptor adaptor,
    AjPStr *Pvalue);

AjBool ensMetaInformationAdaptorGetSchemaVersion(
    const EnsPMetaInformationAdaptor adaptor,
    AjPStr *Pvalue);

AjBool ensMetaInformationAdaptorGetTaxonomyIdentifier(
    const EnsPMetaInformationAdaptor adaptor,
    AjPStr *Pvalue);

AjBool ensMetaInformationAdaptorKeyValueExists(
    const EnsPMetaInformationAdaptor adaptor,
    const AjPStr key,
    const AjPStr value);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
