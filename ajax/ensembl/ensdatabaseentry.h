#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensdatabaseentry_h
#define ensdatabaseentry_h

#include "ensanalysis.h"
#include "ensexternaldatabase.h"




/*
** Prototype definitions
*/

/* Ensembl External Reference */

EnsPExternalReference ensExternalReferenceNew(ajuint identifier,
                                              EnsPAnalysis analysis,
                                              EnsPExternalDatabase edb,
                                              AjPStr primaryid,
                                              AjPStr displayid,
                                              AjPStr version,
                                              AjPStr description,
                                              AjPStr linkageannotation,
                                              AjPStr infotext,
                                              AjEnum infotype);

EnsPExternalReference ensExternalReferenceNewObj(
    const EnsPExternalReference object);

EnsPExternalReference ensExternalReferenceNewRef(EnsPExternalReference er);

void ensExternalReferenceDel(EnsPExternalReference *Per);

ajuint ensExternalReferenceGetIdentifier(const EnsPExternalReference er);

EnsPAnalysis ensExternalReferenceGetAnalysis(const EnsPExternalReference er);

EnsPExternalDatabase ensExternalReferenceGetExternalDatabase(
    const EnsPExternalReference er);

AjPStr ensExternalReferenceGetPrimaryIdentifier(const EnsPExternalReference er);

AjPStr ensExternalReferenceGetDisplayIdentifier(const EnsPExternalReference er);

AjPStr ensExternalReferenceGetVersion(const EnsPExternalReference er);

AjPStr ensExternalReferenceGetDescription(const EnsPExternalReference er);

AjPStr ensExternalReferenceGetLinkageAnnotation(const EnsPExternalReference er);

AjPStr ensExternalReferenceGetInfoText(const EnsPExternalReference er);

AjEnum ensExternalReferenceGetInfoType(const EnsPExternalReference er);

AjBool ensExternalReferenceSetIdentifier(EnsPExternalReference er,
                                         ajuint identifier);

AjBool ensExternalReferenceSetAnalysis(EnsPExternalReference er,
                                       EnsPAnalysis analysis);

AjBool ensExternalReferenceSetExternalDatabase(EnsPExternalReference er,
                                               EnsPExternalDatabase edb);

AjBool ensExternalReferenceSetPrimaryIdentifier(EnsPExternalReference er,
                                                AjPStr primaryid);

AjBool ensExternalReferenceSetDisplayIdentifier(EnsPExternalReference er,
                                                AjPStr displayid);

AjBool ensExternalReferenceSetVersion(EnsPExternalReference er, AjPStr version);

AjBool ensExternalReferenceSetDescription(EnsPExternalReference er,
                                          AjPStr description);

AjBool ensExternalReferenceSetLinkageAnnotation(EnsPExternalReference er,
                                                AjPStr linkageannotation);

AjBool ensExternalReferenceSetInfoText(EnsPExternalReference er,
                                       AjPStr infotext);

AjBool ensExternalReferenceSetInfoType(EnsPExternalReference er,
                                       AjEnum infotype);

AjBool ensExternalReferenceTrace(const EnsPExternalReference er, ajuint level);

AjEnum ensExternalReferenceInfoTypeFromStr(const AjPStr infotype);

const char *ensExternalReferenceInfoTypeToChar(const AjEnum type);

ajuint ensExternalReferenceGetMemSize(const EnsPExternalReference er);

/* Ensembl Identity Reference */

EnsPIdentityReference ensIdentityReferenceNew(AjPStr cigar,
                                              ajint qstart,
                                              ajint qend,
                                              ajint qidentity,
                                              ajint tstart,
                                              ajint tend,
                                              ajint tidentity,
                                              double score,
                                              double evalue);

EnsPIdentityReference ensIdentityReferenceNewObj(
    const EnsPIdentityReference object);

void ensIdentityReferenceDel(EnsPIdentityReference *Pir);

AjPStr ensIdentityReferenceGetCigar(const EnsPIdentityReference ir);

ajint ensIdentityReferenceGetQueryStart(const EnsPIdentityReference ir);

ajint ensIdentityReferenceGetQueryEnd(const EnsPIdentityReference ir);

ajint ensIdentityReferenceGetQueryIdentity(const EnsPIdentityReference ir);

ajint ensIdentityReferenceGetTargetStart(const EnsPIdentityReference ir);

ajint ensIdentityReferenceGetTargetEnd(const EnsPIdentityReference ir);

ajint ensIdentityReferenceGetTargetIdentity(const EnsPIdentityReference ir);

double ensIdentityReferenceGetEvalue(const EnsPIdentityReference ir);

double ensIdentityReferenceGetScore(const EnsPIdentityReference ir);

AjBool ensIdentityReferenceSetCigar(EnsPIdentityReference ir, AjPStr cigar);

AjBool ensIdentityReferenceSetQueryStart(EnsPIdentityReference ir,
                                         ajint qstart);

AjBool ensIdentityReferenceSetQueryEnd(EnsPIdentityReference ir, ajint qend);

AjBool ensIdentityReferenceSetQueryIdentity(EnsPIdentityReference ir,
                                            ajint qidentity);

AjBool ensIdentityReferenceSetTargetStart(EnsPIdentityReference ir,
                                          ajint tstart);

AjBool ensIdentityReferenceSetTargetEnd(EnsPIdentityReference ir, ajint tend);

AjBool ensIdentityReferenceSetTargetIdentity(EnsPIdentityReference ir,
                                             ajint tidentity);

AjBool ensIdentityReferenceSetEvalue(EnsPIdentityReference ir, double evalue);

AjBool ensIdentityReferenceSetScore(EnsPIdentityReference ir, double score);

AjBool ensIdentityReferenceTrace(const EnsPIdentityReference ir, ajuint level);

ajuint ensIdentityReferenceGetMemSize(const EnsPIdentityReference ir);

/* Ensembl Gene Ontology Linkage */

EnsPGeneOntologyLinkage ensGeneOntologyLinkageNew(AjPStr linkagetype,
                                                  EnsPDatabaseEntry source);

EnsPGeneOntologyLinkage ensGeneOntologyLinkageNewObj(
    const EnsPGeneOntologyLinkage object);

EnsPGeneOntologyLinkage ensGeneOntologyLinkageNewRef(
    EnsPGeneOntologyLinkage gol);

void ensGeneOntologyLinkageDel(EnsPGeneOntologyLinkage *Pgol);

/* Ensembl Database Entry */

EnsPDatabaseEntry ensDatabaseEntryNew(EnsPDatabaseEntryAdaptor adaptor,
                                      ajuint identifier,
                                      EnsPAnalysis analysis,
                                      EnsPExternalDatabase edb,
                                      AjPStr primaryid,
                                      AjPStr displayid,
                                      AjPStr version,
                                      AjPStr description,
                                      AjPStr linkageannotation,
                                      AjEnum infotype,
                                      AjPStr infotext);

EnsPDatabaseEntry ensDatabaseEntryNewObj(const EnsPDatabaseEntry object);

EnsPDatabaseEntry ensDatabaseEntryNewRef(EnsPDatabaseEntry dbe);

void ensDatabaseEntryDel(EnsPDatabaseEntry *Pdbe);

EnsPDatabaseEntryAdaptor ensDatabaseEntryGetAdaptor(
    const EnsPDatabaseEntry dbe);

ajuint ensDatabaseEntryGetIdentifier(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetPrimaryIdentifier(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetDisplayIdentifier(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetVersion(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetDescription(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetInfoText(const EnsPDatabaseEntry dbe);

AjEnum ensDatabaseEntryGetInfoType(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetDbName(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetDbRelease(const EnsPDatabaseEntry dbe);

AjPStr ensDatabaseEntryGetDbDisplayName(const EnsPDatabaseEntry dbe);

AjBool ensDatabaseEntryGetPrimaryIdIsLinkable(const EnsPDatabaseEntry dbe);

AjBool ensDatabaseEntryGetDisplayIdIsLinkable(const EnsPDatabaseEntry dbe);

ajint ensDatabaseEntryGetPriority(const EnsPDatabaseEntry dbe);

AjEnum ensDatabaseEntryGetStatus(const EnsPDatabaseEntry dbe);

AjEnum ensDatabaseEntryGetType(const EnsPDatabaseEntry dbe);

AjPList ensDatabaseEntryGetSynonyms(const EnsPDatabaseEntry dbe);

AjBool ensDatabaseEntryTrace(const EnsPDatabaseEntry dbe, ajuint level);

ajuint ensDatabaseEntryGetMemSize(const EnsPDatabaseEntry dbe);

AjBool ensDatabaseEntryAddGeneOntologyLinkage(EnsPDatabaseEntry dbe,
                                              AjPStr linkagetype,
                                              EnsPDatabaseEntry source);

const AjPList ensDatabaseEntryGetGeneOntologyLinkages(EnsPDatabaseEntry dbe);

AjBool ensDatabaseEntryGetAllLinkageTypes(EnsPDatabaseEntry dbe, AjPList types);

AjBool EnsDatabaseEntryClearGeneOntologyLinkageTypes(EnsPDatabaseEntry dbe);

/* Ensembl Database Entry Adaptor */

EnsPDatabaseEntryAdaptor ensDatabaseEntryAdaptorNew(EnsPDatabaseAdaptor dba);

void ensDatabaseEntryAdaptorDel(EnsPDatabaseEntryAdaptor *Pdbea);

EnsPDatabaseAdaptor ensDatabaseEntryAdaptorGetDatabaseAdaptor(
    const EnsPDatabaseEntryAdaptor dbea);

AjBool ensDatabaseEntryAdaptorFetchByIdentifier(EnsPDatabaseEntryAdaptor dbea,
                                                ajuint identifier,
                                                EnsPDatabaseEntry *Pdbe);

AjBool ensDatabaseEntryAdaptorFetchByDbNameAccession(
    EnsPDatabaseEntryAdaptor dbea,
    const AjPStr dbname,
    const AjPStr accession,
    EnsPDatabaseEntry *Pdbe);

AjBool ensDatabaseEntryAdaptorFetchAllByObjectType(
    EnsPDatabaseEntryAdaptor dbea,
    ajuint objid,
    AjPStr objtype,
    AjPStr dbname,
    AjEnum dbtype,
    AjPList dbes);

AjBool ensDatabaseEntryAdaptorFetchAllGeneIdentifiersByExternalName(
    EnsPDatabaseEntryAdaptor dbea,
    AjPStr name,
    AjPStr dbname,
    AjPList idlist);

AjBool ensDatabaseEntryAdaptorFetchAllTranscriptIdentifiersByExternalName(
    EnsPDatabaseEntryAdaptor dbea,
    AjPStr name,
    AjPStr dbname,
    AjPList idlist);

AjBool ensDatabaseEntryAdaptorFetchAllTranslationIdentifiersByExternalName(
    EnsPDatabaseEntryAdaptor dbea,
    AjPStr name,
    AjPStr dbname,
    AjPList idlist);

AjBool ensDatabaseEntryAdaptorFetchAllGeneIdentifiersByExternalDatabaseName(
    EnsPDatabaseEntryAdaptor dbea,
    AjPStr dbname,
    AjPList idlist);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
