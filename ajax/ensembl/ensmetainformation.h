#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensmetainformation_h
#define ensmetainformation_h

#include "ensdatabaseadaptor.h"
#include "enstable.h"




/* @data EnsPMetaInformationadaptor *******************************************
**
** Ensembl Meta-Information Adaptor.
**
** @alias EnsSMetaInformationadaptor
** @alias EnsOMetaInformationadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor.
** @attr CacheByIdentifier [AjPTable] Cache by Meta-Information SQL identifier.
** @attr CacheByKey [AjPTable] Cache by Meta-Information key data.
** @@
******************************************************************************/

typedef struct EnsSMetaInformationadaptor
{
    EnsPDatabaseadaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByKey;
} EnsOMetaInformationadaptor;

#define EnsPMetaInformationadaptor EnsOMetaInformationadaptor*




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
** @attr Adaptor [EnsPMetaInformationadaptor] Ensembl Meta-Information Adaptor.
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
    EnsPMetaInformationadaptor Adaptor;
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

EnsPMetaInformation ensMetaInformationNew(EnsPMetaInformationadaptor adaptor,
                                          ajuint identifier,
                                          ajuint species,
                                          AjPStr key,
                                          AjPStr value);

EnsPMetaInformation ensMetaInformationNewObj(EnsPMetaInformation object);

EnsPMetaInformation ensMetaInformationNewRef(EnsPMetaInformation mi);

void ensMetaInformationDel(EnsPMetaInformation* Pmi);

EnsPMetaInformationadaptor ensMetaInformationGetadaptor(
    const EnsPMetaInformation mi);

ajuint ensMetaInformationGetIdentifier(const EnsPMetaInformation mi);

AjPStr ensMetaInformationGetKey(const EnsPMetaInformation mi);

AjPStr ensMetaInformationGetValue(const EnsPMetaInformation mi);

ajuint ensMetaInformationGetSpecies(const EnsPMetaInformation mi);

AjBool ensMetaInformationSetadaptor(EnsPMetaInformation mi,
                                    EnsPMetaInformationadaptor adaptor);

AjBool ensMetaInformationSetIdentifier(EnsPMetaInformation mi,
                                       ajuint identifier);

AjBool ensMetaInformationSetKey(EnsPMetaInformation mi, AjPStr key);

AjBool ensMetaInformationSetValue(EnsPMetaInformation mi, AjPStr value);

AjBool ensMetaInformationSetSpecies(EnsPMetaInformation mi, ajuint species);

AjBool ensMetaInformationTrace(const EnsPMetaInformation mi, ajuint level);

ajuint ensMetaInformationGetMemSize(const EnsPMetaInformation mi);

/* Ensembl Meta-Information Adaptor */

EnsPMetaInformationadaptor ensMetaInformationadaptorNew(
    EnsPDatabaseadaptor dba);

void ensMetaInformationadaptorDel(EnsPMetaInformationadaptor* Padaptor);

AjBool ensMetaInformationadaptorFetchAllByKey(
    const EnsPMetaInformationadaptor adaptor,
    const AjPStr key,
    AjPList mis);

AjBool ensMetaInformationadaptorGetValueByKey(
    const EnsPMetaInformationadaptor adaptor,
				       const AjPStr key,
				       AjPStr *Pvalue);

AjBool ensMetaInformationadaptorGetGenebuildVersion(
    const EnsPMetaInformationadaptor adaptor,
    AjPStr *Pvalue);

AjBool ensMetaInformationadaptorGetSchemaVersion(
    const EnsPMetaInformationadaptor adaptor,
    AjPStr *Pvalue);

AjBool ensMetaInformationadaptorGetTaxonomyIdentifier(
    const EnsPMetaInformationadaptor adaptor,
    AjPStr *Pvalue);

AjBool ensMetaInformationadaptorKeyValueExists(
    const EnsPMetaInformationadaptor adaptor,
    const AjPStr key,
    const AjPStr value);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
