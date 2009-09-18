#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensregistry_h
#define ensregistry_h

#include "ajax.h"
#include "ensdatabaseadaptor.h"
#include "ensanalysis.h"
#include "ensassemblyexception.h"
#include "ensassemblymapper.h"
#include "enscoordsystem.h"
#include "ensdatabaseentry.h"
#include "ensdensity.h"
#include "ensditag.h"
#include "ensexon.h"
#include "ensexternaldatabase.h"
#include "ensgene.h"
#include "enskaryotype.h"
#include "ensmarker.h"
#include "ensmetacoordinate.h"
#include "ensmetainformation.h"
#include "ensmiscellaneous.h"
#include "ensprediction.h"
#include "ensqc.h"
#include "ensrepeat.h"
#include "enssequence.h"
#include "ensseqregion.h"
#include "ensslice.h"
#include "enstranscript.h"
#include "enstranslation.h"
#include "ensvariation.h"




/*
** Prototype definitions
*/

/* Ensembl Registry */

void ensRegistryInit(void);

void ensRegistryClear(void);

void ensRegistryExit(void);

/* Ensembl Registry Aliases */

AjBool ensRegistryAddAlias(const AjPStr species, const AjPStr alias);

AjBool ensRegistryRemoveAlias(const AjPStr alias);

AjPStr ensRegistryGetSpecies(const AjPStr alias);

AjBool ensRegistryLoadAliases(void);

AjBool ensRegistryTraceAliases(ajint level);

AjBool ensRegistryTraceEntries(ajint level);

/* Ensembl Database Adaptors */

AjBool ensRegistryAddDatabaseAdaptor(EnsPDatabaseConnection dbc,
                                     AjPStr database,
                                     AjPStr alias,
                                     AjEnum group,
                                     AjBool multi,
                                     ajuint identifier);

AjBool ensRegistryRemoveDatabaseAdaptor(EnsPDatabaseAdaptor *Pdba);

AjBool ensRegistryGetAllDatabaseAdaptors(AjEnum group,
                                         const AjPStr alias,
                                         AjPList list);

EnsPDatabaseAdaptor ensRegistryGetDatabaseAdaptor(AjEnum group,
                                                  const AjPStr alias);

/* Ensembl Core-style Adaptors */

EnsPDatabaseAdaptor ensRegistryGetDnaAdaptor(EnsPDatabaseAdaptor dba);

EnsPAnalysisAdaptor ensRegistryGetAnalysisAdaptor(EnsPDatabaseAdaptor dba);

EnsPAssemblyExceptionAdaptor ensRegistryGetAssemblyExceptionAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPAssemblyExceptionFeatureAdaptor ensRegistryGetAssemblyExceptionFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPAssemblyMapperAdaptor ensRegistryGetAssemblyMapperAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPCoordSystemAdaptor ensRegistryGetCoordSystemAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPDatabaseEntryAdaptor ensRegistryGetDatabaseEntryAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPDensityFeatureAdaptor ensRegistryGetDensityFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPDensityTypeAdaptor ensRegistryGetDensityTypeAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPDiTagFeatureAdaptor ensRegistryGetDiTagFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPDNAAlignFeatureAdaptor ensRegistryGetDNAAlignFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPExonAdaptor ensRegistryGetExonAdaptor(EnsPDatabaseAdaptor dba);

EnsPExternalDatabaseAdaptor ensRegistryGetExternalDatabaseAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPGeneAdaptor ensRegistryGetGeneAdaptor(EnsPDatabaseAdaptor dba);

EnsPKaryotypeBandAdaptor ensRegistryGetKaryotypeBandAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPMarkerAdaptor ensRegistryGetMarkerAdaptor(EnsPDatabaseAdaptor dba);

EnsPMarkerFeatureAdaptor ensRegistryGetMarkerFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPMetaInformationAdaptor ensRegistryGetMetaInformationAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPMetaCoordinateAdaptor ensRegistryGetMetaCoordinateAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPMiscellaneousFeatureAdaptor ensRegistryGetMiscellaneousFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPMiscellaneousSetAdaptor ensRegistryGetMiscellaneousSetAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPPredictionExonAdaptor ensRegistryGetPredictionExonAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPPredictionTranscriptAdaptor ensRegistryGetPredictionTranscriptAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPProteinAlignFeatureAdaptor ensRegistryGetProteinAlignFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPProteinFeatureAdaptor ensRegistryGetProteinFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPRepeatFeatureAdaptor ensRegistryGetRepeatFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPSeqRegionAdaptor ensRegistryGetSeqRegionAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPSequenceAdaptor ensRegistryGetSequenceAdaptor(EnsPDatabaseAdaptor dba);

EnsPSimpleFeatureAdaptor ensRegistryGetSimpleFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPSliceAdaptor ensRegistryGetSliceAdaptor(EnsPDatabaseAdaptor dba);

EnsPTranscriptAdaptor ensRegistryGetTranscriptAdaptor(EnsPDatabaseAdaptor dba);

EnsPTranslationAdaptor ensRegistryGetTranslationAdaptor(
    EnsPDatabaseAdaptor dba);

/* Ensembl Quality Check Adaptors */

EnsPQCDatabaseAdaptor ensRegistryGetQCDatabaseAdaptor(EnsPDatabaseAdaptor dba);

EnsPQCSequenceAdaptor ensRegistryGetQCSequenceAdaptor(EnsPDatabaseAdaptor dba);

EnsPQCAlignmentAdaptor ensRegistryGetQCAlignmentAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPQCDASFeatureAdaptor ensRegistryGetQCDASFeatureAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPQCVariationAdaptor ensRegistryGetQCVariationAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPQCSubmissionAdaptor ensRegistryGetQCSubmissionAdaptor(
    EnsPDatabaseAdaptor dba);

/* Ensembl Genetic Variation Adaptors */

EnsPGVIndividualAdaptor ensRegistryGetGVIndividualAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPGVPopulationAdaptor ensRegistryGetGVPopulationAdaptor(
    EnsPDatabaseAdaptor dba);

EnsPGVVariationAdaptor ensRegistryGetGVVariationAdaptor(
    EnsPDatabaseAdaptor dba);

AjBool ensRegistryLoadFromServer(EnsPDatabaseConnection dbc);

AjBool ensRegistryLoadIdentifiers(void);

AjBool ensRegistryGetSpeciesGroup(const AjPStr identifier,
                                  AjPStr *Pspecies,
                                  AjBool *Pgroup);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
