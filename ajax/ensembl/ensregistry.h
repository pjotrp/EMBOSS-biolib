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

AjBool ensRegistryAddDatabaseadaptor(EnsPDatabaseConnection dbc,
                                     AjPStr database,
                                     AjPStr alias,
                                     AjEnum group,
                                     AjBool multi,
                                     ajuint identifier);

AjBool ensRegistryRemoveDatabaseadaptor(EnsPDatabaseadaptor *Pdba);

AjBool ensRegistryGetAllDatabaseadaptors(AjEnum group,
                                         const AjPStr alias,
                                         AjPList list);

EnsPDatabaseadaptor ensRegistryGetDatabaseadaptor(AjEnum group,
                                                  const AjPStr alias);



/* Excluded in C file */
/* EnsPDatabaseadaptor ensRegistryGetDnaadaptor(EnsPDatabaseadaptor dba); */

/* Ensembl Core-style Adaptors */
    
EnsPAnalysisadaptor ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba);

EnsPAssemblyExceptionadaptor ensRegistryGetAssemblyExceptionadaptor(
    EnsPDatabaseadaptor dba);

EnsPAssemblyExceptionFeatureadaptor ensRegistryGetAssemblyExceptionFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPAssemblyMapperadaptor ensRegistryGetAssemblyMapperadaptor(
    EnsPDatabaseadaptor dba);

EnsPCoordSystemadaptor ensRegistryGetCoordSystemadaptor(
    EnsPDatabaseadaptor dba);

EnsPDatabaseEntryadaptor ensRegistryGetDatabaseEntryadaptor(
    EnsPDatabaseadaptor dba);

EnsPDensityFeatureadaptor ensRegistryGetDensityFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPDensityTypeadaptor ensRegistryGetDensityTypeadaptor(
    EnsPDatabaseadaptor dba);

EnsPDiTagFeatureadaptor ensRegistryGetDiTagFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPDNAAlignFeatureadaptor ensRegistryGetDNAAlignFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPExonadaptor ensRegistryGetExonadaptor(EnsPDatabaseadaptor dba);

EnsPExternalDatabaseadaptor ensRegistryGetExternalDatabaseadaptor(
    EnsPDatabaseadaptor dba);

EnsPGeneadaptor ensRegistryGetGeneadaptor(EnsPDatabaseadaptor dba);

EnsPKaryotypeBandadaptor ensRegistryGetKaryotypeBandadaptor(
    EnsPDatabaseadaptor dba);

EnsPMarkeradaptor ensRegistryGetMarkeradaptor(EnsPDatabaseadaptor dba);

EnsPMarkerFeatureadaptor ensRegistryGetMarkerFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPMetaInformationadaptor ensRegistryGetMetaInformationadaptor(
    EnsPDatabaseadaptor dba);

EnsPMetaCoordinateadaptor ensRegistryGetMetaCoordinateadaptor(
    EnsPDatabaseadaptor dba);

EnsPMiscellaneousFeatureadaptor ensRegistryGetMiscellaneousFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPMiscellaneousSetadaptor ensRegistryGetMiscellaneousSetadaptor(
    EnsPDatabaseadaptor dba);

EnsPPredictionExonadaptor ensRegistryGetPredictionExonadaptor(
    EnsPDatabaseadaptor dba);

EnsPPredictionTranscriptadaptor ensRegistryGetPredictionTranscriptadaptor(
    EnsPDatabaseadaptor dba);

EnsPProteinAlignFeatureadaptor ensRegistryGetProteinAlignFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPProteinFeatureadaptor ensRegistryGetProteinFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPRepeatFeatureadaptor ensRegistryGetRepeatFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPSeqRegionadaptor ensRegistryGetSeqRegionadaptor(
    EnsPDatabaseadaptor dba);

EnsPSequenceadaptor ensRegistryGetSequenceadaptor(EnsPDatabaseadaptor dba);

EnsPSimpleFeatureadaptor ensRegistryGetSimpleFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPSliceadaptor ensRegistryGetSliceadaptor(EnsPDatabaseadaptor dba);

EnsPTranscriptadaptor ensRegistryGetTranscriptadaptor(EnsPDatabaseadaptor dba);

EnsPTranslationadaptor ensRegistryGetTranslationadaptor(
    EnsPDatabaseadaptor dba);

/* Ensembl Quality Check Adaptors */

EnsPQCDatabaseadaptor ensRegistryGetQCDatabaseadaptor(EnsPDatabaseadaptor dba);

EnsPQCSequenceadaptor ensRegistryGetQCSequenceadaptor(EnsPDatabaseadaptor dba);

EnsPQCAlignmentadaptor ensRegistryGetQCAlignmentadaptor(
    EnsPDatabaseadaptor dba);

EnsPQCDASFeatureadaptor ensRegistryGetQCDASFeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPQCVariationadaptor ensRegistryGetQCVariationadaptor(
    EnsPDatabaseadaptor dba);

EnsPQCSubmissionadaptor ensRegistryGetQCSubmissionadaptor(
    EnsPDatabaseadaptor dba);

/* Ensembl Genetic Variation Adaptors */

EnsPGVIndividualadaptor ensRegistryGetGVIndividualadaptor(
    EnsPDatabaseadaptor dba);

EnsPGVPopulationadaptor ensRegistryGetGVPopulationadaptor(
    EnsPDatabaseadaptor dba);

EnsPGVVariationadaptor ensRegistryGetGVVariationadaptor(
    EnsPDatabaseadaptor dba);

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
