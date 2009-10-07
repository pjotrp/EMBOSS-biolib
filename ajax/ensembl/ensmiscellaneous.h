#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensmiscellaneous_h
#define ensmiscellaneous_h

#include "ensfeature.h"
#include "ensattribute.h"




/* @data EnsPMiscellaneousSetadaptor ******************************************
**
** Ensembl Miscellaneous Set Adaptor.
**
** @alias EnsSMiscellaneousSetadaptor
** @alias EnsOMiscellaneousSetadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor.
** @attr CacheByIdentifier [AjPTable] Identifier cache.
** @attr CacheByCode [AjPTable] Code cache.
** @@
******************************************************************************/

typedef struct EnsSMiscellaneousSetadaptor
{
    EnsPDatabaseadaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByCode;
} EnsOMiscellaneousSetadaptor;

#define EnsPMiscellaneousSetadaptor EnsOMiscellaneousSetadaptor*




/* @data EnsPMiscellaneousSet *************************************************
**
** Ensembl Miscellaneous Set.
**
** @alias EnsSMiscellaneousSet
** @alias EnsOMiscellaneousSet
**
** @attr Use [ajuint] Use counter.
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier.
** @attr Adaptor [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                             Set Adaptor.
** @cc Bio::EnsEMBL::MiscSet
** @cc 'misc_set' SQL table
** @attr Code [AjPStr] Code.
** @attr Name [AjPStr] Name.
** @attr Description [AjPStr] Description.
** @attr MaximumLength [ajuint] Maximum Feature length.
** @attr Padding [ajuint] Padding to alignment boundary.
** @@
******************************************************************************/

typedef struct EnsSMiscellaneousSet
{
    ajuint Use;
    ajuint Identifier;
    EnsPMiscellaneousSetadaptor Adaptor;
    AjPStr Code;
    AjPStr Name;
    AjPStr Description;
    ajuint MaximumLength;
    ajuint Padding;
} EnsOMiscellaneousSet;

#define EnsPMiscellaneousSet EnsOMiscellaneousSet*




/* @data EnsPMiscellaneousFeatureadaptor **************************************
**
** Ensembl Miscellaneous Feature Adaptor.
**
** @alias EnsSMiscellaneousFeatureadaptor
** @alias EnsOMiscellaneousFeatureadaptor
**
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor.
** @@
******************************************************************************/

typedef struct EnsSMiscellaneousFeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsOMiscellaneousFeatureadaptor;

#define EnsPMiscellaneousFeatureadaptor EnsOMiscellaneousFeatureadaptor*




/* @data EnsPMiscellaneousFeature *********************************************
**
** Ensembl Miscellaneous Feature.
**
** @alias EnsSMiscellaneousFeature
** @alias EnsOMiscellaneousFeature
**
** @attr Use [ajuint] Use counter.
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier.
** @attr Adaptor [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                                 Feature Adaptor.
** @cc Bio::EnsEMBL::Feature
** @cc 'misc_feature' SQL table
** @attr Feature [EnsPFeature] Ensembl Feature.
** @cc Bio::EnsEMBL::MiscFeature
** @attr Attributes [AjPList] AJAX List of Ensembl Attributes.
** @attr MiscellaneousSets [AjPList] AJAX List of Ensembl Miscellaneous Sets.
** @@
******************************************************************************/

typedef struct EnsSMiscellaneousFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPMiscellaneousFeatureadaptor Adaptor;
    EnsPFeature Feature;
    AjPList Attributes;
    AjPList MiscellaneousSets;
} EnsOMiscellaneousFeature;

#define EnsPMiscellaneousFeature EnsOMiscellaneousFeature*




/*
** Prototype definitions
*/

/* Ensembl Miscellaneous Set */

EnsPMiscellaneousSet ensMiscellaneousSetNew(EnsPMiscellaneousSetadaptor adaptor,
                                            ajuint identifier,
                                            AjPStr code,
                                            AjPStr name,
                                            AjPStr description,
                                            ajuint maxlen);

EnsPMiscellaneousSet ensMiscellaneousSetNewObj(
    const EnsPMiscellaneousSet object);

EnsPMiscellaneousSet ensMiscellaneousSetNewRef(EnsPMiscellaneousSet ms);

void ensMiscellaneousSetDel(EnsPMiscellaneousSet* Pms);

EnsPMiscellaneousSetadaptor ensMiscellaneousSetGetadaptor(
    const EnsPMiscellaneousSet ms);

ajuint ensMiscellaneousSetGetIdentifier(const EnsPMiscellaneousSet ms);

AjPStr ensMiscellaneousSetGetCode(const EnsPMiscellaneousSet ms);

AjPStr ensMiscellaneousSetGetName(const EnsPMiscellaneousSet ms);

AjPStr ensMiscellaneousSetGetDescription(const EnsPMiscellaneousSet ms);

ajuint ensMiscellaneousSetGetMaximumLength(const EnsPMiscellaneousSet ms);

AjBool ensMiscellaneousSetSetadaptor(EnsPMiscellaneousSet ms,
			      EnsPMiscellaneousSetadaptor adaptor);

AjBool ensMiscellaneousSetSetIdentifier(EnsPMiscellaneousSet ms,
                                        ajuint identifier);

AjBool ensMiscellaneousSetSetCode(EnsPMiscellaneousSet ms, AjPStr code);

AjBool ensMiscellaneousSetSetName(EnsPMiscellaneousSet ms, AjPStr name);

AjBool ensMiscellaneousSetSetDescription(EnsPMiscellaneousSet ms,
                                         AjPStr description);

AjBool ensMiscellaneousSetSetMaximumLength(EnsPMiscellaneousSet ms,
                                           ajuint maxlen);

ajuint ensMiscellaneousSetGetMemSize(const EnsPMiscellaneousSet ms);

AjBool ensMiscellaneousSetTrace(const EnsPMiscellaneousSet ms, ajuint level);

/* Ensembl Miscellaneous Set Adaptor */

EnsPMiscellaneousSetadaptor ensMiscellaneousSetadaptorNew(
    EnsPDatabaseadaptor dba);

void ensMiscellaneousSetadaptorDel(EnsPMiscellaneousSetadaptor* Padaptor);

const EnsPDatabaseadaptor ensMiscellaneousSetadaptorGetDatabaseadaptor(
    const EnsPMiscellaneousSetadaptor adaptor);

AjBool ensMiscellaneousSetadaptorFetchAll(
    const EnsPMiscellaneousSetadaptor adaptor,
				   AjPList mslist);

AjBool ensMiscellaneousSetadaptorFetchByIdentifier(
    EnsPMiscellaneousSetadaptor adaptor,
    ajuint identifier,
    EnsPMiscellaneousSet *Pms);

AjBool ensMiscellaneousSetadaptorFetchByCode(
    EnsPMiscellaneousSetadaptor adaptor,
    const AjPStr code,
    EnsPMiscellaneousSet *Pms);

/* Ensembl Miscellaneous Feature */

EnsPMiscellaneousFeature ensMiscellaneousFeatureNew(
    EnsPMiscellaneousFeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature);

EnsPMiscellaneousFeature ensMiscellaneousFeatureNewObj(
    const EnsPMiscellaneousFeature object);

EnsPMiscellaneousFeature ensMiscellaneousFeatureNewRef(
    EnsPMiscellaneousFeature mf);

void ensMiscellaneousFeatureDel(EnsPMiscellaneousFeature* Pmf);

EnsPMiscellaneousFeatureadaptor ensMiscellaneousFeatureGetadaptor(
    const EnsPMiscellaneousFeature mf);

ajuint ensMiscellaneousFeatureGetIdentifier(const EnsPMiscellaneousFeature mf);

EnsPFeature ensMiscellaneousFeatureGetFeature(
    const EnsPMiscellaneousFeature mf);

const AjPList ensMiscellaneousFeatureGetAttributes(
    const EnsPMiscellaneousFeature mf);

const AjPList ensMiscellaneousFeatureGetMiscellaneousSets(
    const EnsPMiscellaneousFeature mf);

AjBool ensMiscellaneousFeatureSetadaptor(
    EnsPMiscellaneousFeature mf,
    EnsPMiscellaneousFeatureadaptor adaptor);

AjBool ensMiscellaneousFeatureSetIdentifier(EnsPMiscellaneousFeature mf,
                                            ajuint identifier);

AjBool ensMiscellaneousFeatureSetFeature(EnsPMiscellaneousFeature mf,
                                         EnsPFeature feature);

AjBool ensMiscellaneousFeatureAddAttribute(EnsPMiscellaneousFeature mf,
                                           EnsPAttribute attribute);

AjBool ensMiscellaneousFeatureAddMiscellaneousSet(EnsPMiscellaneousFeature mf,
                                                  EnsPMiscellaneousSet ms);

AjBool ensMiscellaneousFeatureTrace(const EnsPMiscellaneousFeature mf,
                                    ajuint level);

ajuint ensMiscellaneousFeatureGetMemSize(const EnsPMiscellaneousFeature mf);

AjBool ensMiscellaneousFeatureFetchAllAttributes(EnsPMiscellaneousFeature mf,
                                                 const AjPStr code,
                                                 AjPList attributes);

AjBool ensMiscellaneousFeatureFetchAllMiscellaneousSets(
    EnsPMiscellaneousFeature mf,
    const AjPStr code,
    AjPList mslist);

/* Ensembl Miscellaneous Feature Adaptor */

EnsPMiscellaneousFeatureadaptor ensMiscellaneousFeatureadaptorNew(
    EnsPDatabaseadaptor dba);

void ensMiscellaneousFeatureadaptorDel(EnsPMiscellaneousFeatureadaptor *Pmfa);

AjBool ensMiscellaneousFeatureadaptorFetchAllBySliceAndSetCode(
    EnsPMiscellaneousFeatureadaptor adaptor,
    EnsPSlice slice,
    const AjPList codes,
    AjPList mflist);

AjBool ensMiscellaneousFeatureadaptorFetchAllByAttributeCodeValue(
    EnsPMiscellaneousFeatureadaptor adaptor,
    const AjPStr code,
    const AjPStr value,
    AjPList mflist);

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
