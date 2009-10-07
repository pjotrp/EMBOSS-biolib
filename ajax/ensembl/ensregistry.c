/******************************************************************************
** @Source Ensembl Registry functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.3 $
** @@
**
** Bio::EnsEMBL::Registry CVS Revision:
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
******************************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensregistry.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

/* @datastatic RegistryPIdentifier ********************************************
**
** Ensembl Registry Identifier.
**
** Holds regular expressions for matching Ensembl Gene, Transcript,
** Translation and Exon stable identifiers, as well as
** Ensembl Database Adaptor species and group information.
**
** @alias RegistrySIdentifier
** @alias RegistryOIdentifier
**
** @attr RegularExpression [AjPStr] Regular expression
** @attr SpeciesName [AjPStr] Ensembl Database Adaptor species element
** @attr Group [AjEnum] Ensembl Database Adaptor group element
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct RegistrySIdentifier
{
    AjPStr RegularExpression;
    AjPStr SpeciesName;
    AjEnum Group;
    ajuint Padding;
} RegistryOIdentifier;

#define RegistryPIdentifier RegistryOIdentifier*




/* @datastatic RegistryPCoreStyle *********************************************
**
** Ensembl Core-Style Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Core-Style databases.
**
** @alias RegistrySCoreStyle
** @alias RegistryOCoreStyle
**
** @attr Databaseadaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @attr Dnaadaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @attr Analysisadaptor [EnsPAnalysisadaptor] Ensembl Analysis Adaptor
** @attr AssemblyExceptionadaptor [EnsPAssemblyExceptionadaptor]
**                   Ensembl Assembly Exception Adaptor
** @attr AssemblyExceptionFeatureadaptor [EnsPAssemblyExceptionFeatureadaptor]
**                   Ensembl Assembly Exception Feature Adaptor
** @attr AssemblyMapperadaptor [EnsPAssemblyMapperadaptor]
**                   Ensembl Assembly Mapper Adaptor
** @attr CoordSystemadaptor [EnsPCoordSystemadaptor]
**                   Ensembl Coordjnate System Adaptor
** @attr DatabaseEntryadaptor [EnsPDatabaseEntryadaptor]
**                   Ensembl Database Entry Adaptor
** @attr DensityFeatureadaptor [EnsPDensityFeatureadaptor]
**                   Ensembl Density Feature Adaptor
** @attr DensityTypeadaptor [EnsPDensityTypeadaptor]
**                   Ensembl Density Type Adaptor
** @attr DiTagFeatureadaptor [EnsPDiTagFeatureadaptor]
**                   Ensembl Di-Tag Feature Adaptor
** @attr DNAAlignFeatureadaptor [EnsPDNAAlignFeatureadaptor]
**                   Ensembl DNA Alignment Feature Adaptor
** @attr Exonadaptor [EnsPExonadaptor]
**                   Ensembl Exon Adaptor
** @attr ExternalDatabaseadaptor [EnsPExternalDatabaseadaptor]
**                   Ensembl External Database Adaptor
** @attr Geneadaptor [EnsPGeneadaptor] Ensembl Gene Adaptor
** @attr KaryotypeBandadaptor [EnsPKaryotypeBandadaptor]
**                   Ensembl Karyotype Band Adaptor
** @attr Markeradaptor [EnsPMarkeradaptor]
**                   Ensembl Marker Adaptor
** @attr MarkerFeatureadaptor [EnsPMarkerFeatureadaptor]
**                   Ensembl Marker Feature Adaptor
** @attr MetaInformationadaptor [EnsPMetaInformationadaptor]
**                   Ensembl Meta-Information Adaptor
** @attr MetaCoordinateadaptor [EnsPMetaCoordinateadaptor]
**                   Ensembl Meta Coordinate Adaptor
** @attr MiscellaneousFeatureadaptor [EnsPMiscellaneousFeatureadaptor]
**                   Ensembl MIscellaneous Feature Adaptor
** @attr MiscellaneousSetadaptor [EnsPMiscellaneousSetadaptor]
**                   Ensembl MIscellaneous Set Adaptor
** @attr PredictionExonadaptor [EnsPPredictionExonadaptor]
**                   Ensembl Prediction Exon Adaptor
** @attr PredictionTranscriptadaptor [EnsPPredictionTranscriptadaptor]
**                   Ensembl Prediction Transcript Adaptor
** @attr ProteinAlignFeatureadaptor [EnsPProteinAlignFeatureadaptor]
**                   Ensembl Protein Alignment Feature Adaptor
** @attr ProteinFeatureadaptor [EnsPProteinFeatureadaptor]
**                   Ensembl Protein Feature Adaptor
** @attr RepeatFeatureadaptor [EnsPRepeatFeatureadaptor]
**                   Ensembl Repeat Feature Adaptor
** @attr SeqRegionadaptor [EnsPSeqRegionadaptor]
**                   Ensembl Sequence Region Adaptor
** @attr Sequenceadaptor [EnsPSequenceadaptor]
**                   Ensembl Sequence Adaptor
** @attr SimpleFeatureadaptor [EnsPSimpleFeatureadaptor]
**                   Ensembl Simple Feature Adaptor
** @attr Sliceadaptor [EnsPSliceadaptor]
**                   Ensembl Slice Adaptor
** @attr Transcriptadaptor [EnsPTranscriptadaptor]
**                   Ensembl Transcript Adaptor
** @attr Translationadaptor [EnsPTranslationadaptor]
**                   Ensembl Translation Adaptor
** @@
******************************************************************************/

typedef struct RegistrySCoreStyle
{
    EnsPDatabaseadaptor Databaseadaptor;
    EnsPDatabaseadaptor Dnaadaptor;
    EnsPAnalysisadaptor Analysisadaptor;
    EnsPAssemblyExceptionadaptor AssemblyExceptionadaptor;
    EnsPAssemblyExceptionFeatureadaptor AssemblyExceptionFeatureadaptor;
    EnsPAssemblyMapperadaptor AssemblyMapperadaptor;
    EnsPCoordSystemadaptor CoordSystemadaptor;
    EnsPDatabaseEntryadaptor DatabaseEntryadaptor;
    EnsPDensityFeatureadaptor DensityFeatureadaptor;
    EnsPDensityTypeadaptor DensityTypeadaptor;
    EnsPDiTagFeatureadaptor DiTagFeatureadaptor;
    EnsPDNAAlignFeatureadaptor DNAAlignFeatureadaptor;
    EnsPExonadaptor Exonadaptor;
    EnsPExternalDatabaseadaptor ExternalDatabaseadaptor;
    EnsPGeneadaptor Geneadaptor;
    EnsPKaryotypeBandadaptor KaryotypeBandadaptor;
    EnsPMarkeradaptor Markeradaptor;
    EnsPMarkerFeatureadaptor MarkerFeatureadaptor;
    EnsPMetaInformationadaptor MetaInformationadaptor;
    EnsPMetaCoordinateadaptor MetaCoordinateadaptor;
    EnsPMiscellaneousFeatureadaptor MiscellaneousFeatureadaptor;
    EnsPMiscellaneousSetadaptor MiscellaneousSetadaptor;
    EnsPPredictionExonadaptor PredictionExonadaptor;
    EnsPPredictionTranscriptadaptor PredictionTranscriptadaptor;
    EnsPProteinAlignFeatureadaptor ProteinAlignFeatureadaptor;
    EnsPProteinFeatureadaptor ProteinFeatureadaptor;
    EnsPRepeatFeatureadaptor RepeatFeatureadaptor;
    EnsPSeqRegionadaptor SeqRegionadaptor;
    EnsPSequenceadaptor Sequenceadaptor;
    EnsPSimpleFeatureadaptor SimpleFeatureadaptor;
    EnsPSliceadaptor Sliceadaptor;
    EnsPTranscriptadaptor Transcriptadaptor;
    EnsPTranslationadaptor Translationadaptor;
} RegistryOCoreStyle;

#define RegistryPCoreStyle RegistryOCoreStyle*




/* @datastatic RegistryPGeneticVariation **************************************
**
** Ensembl Genetic Variation Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Genetic Variation databases.
**
** @alias RegistrySGeneticVariation
** @alias RegistryOGeneticVariation
**
** @attr Databaseadaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @attr Alleleadaptor [EnsPGVAlleleadaptor] Ensembl Allele Adaptor
** @attr Genotypeadaptor [EnsPGVGenotypeadaptor] Ensembl Genotype Adaptor
** @attr Individualadaptor [EnsPGVIndividualadaptor] Ensembl Individual Adaptor
** @attr Populationadaptor [EnsPGVPopulationadaptor] Ensembl Population Adaptor
** @attr Sampleadaptor [EnsPGVSampleadaptor] Ensembl Sample Adaptor
** @attr Variationadaptor [EnsPGVVariationadaptor] Ensembl Variation Adaptor
** @@
******************************************************************************/

typedef struct RegistrySGeneticVariation
{
    EnsPDatabaseadaptor Databaseadaptor;
    EnsPGVAlleleadaptor Alleleadaptor;
    EnsPGVGenotypeadaptor Genotypeadaptor;
    EnsPGVIndividualadaptor Individualadaptor;
    EnsPGVPopulationadaptor Populationadaptor;
    EnsPGVSampleadaptor Sampleadaptor;
    EnsPGVVariationadaptor Variationadaptor;
} RegistryOGeneticVariation;

#define RegistryPGeneticVariation RegistryOGeneticVariation*




/* @datastatic RegistryPFunctionalGenomics ************************************
**
** Ensembl Registry Functional Genomics.
**
** Holds an Ensembl Database Adaptors and associated Ensembl Object Adaptors
** specific for Ensembl Functional Genomics databases.
**
** @alias RegistrySFunctionalGenomics
** @alias RegistryOFunctionalGenomics
**
** @attr Databaseadaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySFunctionalGenomics
{
    EnsPDatabaseadaptor Databaseadaptor;
} RegistryOFunctionalGenomics;

#define RegistryPFunctionalGenomics RegistryOFunctionalGenomics*




/* @datastatic RegistryPComparativeGenomics ***********************************
**
** Ensembl Comparative Genomics Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Comparative Genomics databases.
**
** @alias RegistrySComparativeGenomics
** @alias RegistryOComparativeGenomics
**
** @attr Databaseadaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySComparativeGenomics
{
    EnsPDatabaseadaptor Databaseadaptor;
} RegistryOComparativeGenomics;

#define RegistryPComparativeGenomics RegistryOComparativeGenomics*




/* @datastatic RegistryPGeneOntology ******************************************
**
** Ensembl Gene Ontology Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Gene Ontology databases.
**
** @alias RegistrySGeneOntology
** @alias RegistryOGeneOntology
**
** @attr Databaseadaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySGeneOntology
{
    EnsPDatabaseadaptor Databaseadaptor;
} RegistryOGeneOntology;

#define RegistryPGeneOntology RegistryOGeneOntology*




/* @datastatic RegistryPQualityCheck ******************************************
**
** Ensembl Quality Check Registry.
**
** Holds an Ensembl Database Adaptor and associated Ensembl Object Adaptors
** specific for Ensembl Quality Check databases.
**
** @alias RegistrySQualityCheck
** @alias RegistryOQualityCheck
**
** @attr Databaseadaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @attr QCDatabaseadaptor [EnsPQCDatabaseadaptor] Ensembl QC Database Adaptor
** @attr Sequenceadaptor [EnsPQCSequenceadaptor] Ensembl QC Sequence Adaptor
** @attr Alignmentadaptor [EnsPQCAlignmentadaptor] Ensembl QC Alignment Adaptor
** @attr DASFeatureadaptor [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature
**                                                   Adaptor
** @attr Variationadaptor [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @attr Submissionadaptor [EnsPQCSubmissionadaptor] Ensembl QC Submission
**                                                   Adaptor
** @@
******************************************************************************/

typedef struct RegistrySQualityCheck
{
    EnsPDatabaseadaptor Databaseadaptor;
    EnsPQCDatabaseadaptor QCDatabaseadaptor;
    EnsPQCSequenceadaptor Sequenceadaptor;
    EnsPQCAlignmentadaptor Alignmentadaptor;
    EnsPQCDASFeatureadaptor DASFeatureadaptor;
    EnsPQCVariationadaptor Variationadaptor;
    EnsPQCSubmissionadaptor Submissionadaptor;
} RegistryOQualityCheck;

#define RegistryPQualityCheck RegistryOQualityCheck*




/* @datastatic RegistryPEntry *************************************************
**
** Ensembl Registry Entry.
**
** Holds Ensembl database-specific Registry objects.
**
** @alias RegistrySEntry
** @alias RegistryOEntry
**
** @attr Registry [void*[16]] Array of database-specific Registry Entries,
**                             indexed by the Ensembl Database Adaptor group
**                             element
** @@
******************************************************************************/

typedef struct RegistrySEntry
{
    void *Registry[16];
} RegistryOEntry;

#define RegistryPEntry RegistryOEntry*




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

/*
** TODO: Could the registrySoftwareVersion be taken from the build system,
** i.e. the last number in the library software version?
** libensembl_la_LDFLAGS = -version-info 6:1:47
*/

static const char *registrySoftwareVersion = "54";

static AjPTable registryAliases = NULL;

static AjPTable registryEntries = NULL;

static AjPList registryIdentifiers = NULL;

static RegistryPIdentifier registryIdentifierNew(AjPStr expression,
                                                 AjPStr species,
                                                 AjEnum group);

static void registryIdentifierDel(RegistryPIdentifier *Pri);

#if AJFALSE
static AjBool registryCoreStyleTrace(const RegistryPCoreStyle cs, ajuint level);
#endif

static void registryCoreStyleDel(RegistryPCoreStyle *Pcs);

static void registryGeneticVariationDel(RegistryPGeneticVariation *Pgv);

static void registryFunctionalGenomicsDel(RegistryPFunctionalGenomics *Pfg);

static void registryComparativeGenomicsDel(RegistryPComparativeGenomics *Pcg);

static void registryGeneOntologyDel(RegistryPGeneOntology *Pgo);

static void registryQualityCheckDel(RegistryPQualityCheck *Pqc);

static AjBool registryEntryTrace(const RegistryPEntry entry, ajuint level);

static void registryEntryDel(RegistryPEntry *Pentry);

static AjBool registryLoadCollection(EnsPDatabaseConnection dbc, AjPStr dbname);




/* @funcstatic registryIdentifierNew ******************************************
**
** Default constructor for an Ensembl Registry Identifier.
**
** @param [r] expression [AjPStr] Regular expression
** @param [r] species [AjPStr] Ensembl Database Adaptor species element
** @param [r] group [AjEnum] Ensembl Database Adaptor group element
**
** @return [RegistryPIdentifier] Ensembl Registry Identifier or NULL
** @@
******************************************************************************/

static RegistryPIdentifier registryIdentifierNew(AjPStr expression,
                                                 AjPStr species,
                                                 AjEnum group)
{
    RegistryPIdentifier ri = NULL;
    
    if(!expression)
	return NULL;
    
    if(!species)
	return NULL;
    
    if(!group)
	return NULL;
    
    AJNEW0(ri);
    
    ri->RegularExpression = ajStrNewS(expression);
    
    ri->SpeciesName = ajStrNewS(species);
    
    ri->Group = group;
    
    return ri;
}




/* @funcstatic registryIdentifierDel ******************************************
**
** Default destructor for an Ensembl Registry Identifier.
**
** @param [d] Pri [RegistryPIdentifier*] Registry Regular Identifier address
**
** @return [void]
** @@
******************************************************************************/

static void registryIdentifierDel(RegistryPIdentifier *Pri)
{
    RegistryPIdentifier pthis = NULL;
    
    if(!Pri)
	return;
    
    if(!*Pri)
	return;

    pthis = *Pri;
    
    ajStrDel(&pthis->RegularExpression);
    ajStrDel(&pthis->SpeciesName);
    
    AJFREE(pthis);

    *Pri = NULL;

    return;
}




/* @funcstatic registryCoreStyleTrace *****************************************
**
** Trace a Core-Style Registry.
**
** @param [r] cs [const RegistryPCoreStyle] Core-Style Registry
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

#if AJFALSE

static AjBool registryCoreStyleTrace(const RegistryPCoreStyle cs, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!cs)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SregistryCoreStyleTrace %p\n"
	    "%S  Databaseadaptor %p\n"
	    "%S  Dnaadaptor %p\n"
	    "%S  Analysisadaptor %p\n"
	    "%S  AssemblyExceptionadaptor %p\n"
	    "%S  AssemblyExceptionFeatureadaptor %p\n"
	    "%S  AssemblyMapperadaptor %p\n"
	    "%S  CoordSystemadaptor %p\n"
	    "%S  DatabaseEntryadaptor %p\n"
	    "%S  DNAAlignFeatureadaptor %p\n"
	    "%S  Exonadaptor %p\n"
	    "%S  ExternalDatabaseadaptor %p\n"
	    "%S  Geneadaptor %p\n"
	    "%S  KaryotypeBandadaptor %p\n"
	    "%S  MetaInformationadaptor %p\n"
	    "%S  MetaCoordinateadaptor %p\n"
	    "%S  MiscellaneousFeatureadaptor %p\n"
	    "%S  MiscellaneousSetadaptor %p\n"
	    "%S  ProteinAlignFeatureadaptor %p\n"
	    "%S  ProteinFeatureadaptor %p\n"
	    "%S  RepeatFeatureadaptor %p\n"
	    "%S  SeqRegionadaptor %p\n"
	    "%S  Sequenceadaptor %p\n"
	    "%S  SimpleFeatureadaptor %p\n"
	    "%S  Sliceadaptor %p\n"
	    "%S  Transcriptadaptor %p\n"
	    "%S  Translationadaptor %p\n",
	    indent, cs,
	    indent, cs->Databaseadaptor,
	    indent, cs->Dnaadaptor,
	    indent, cs->Analysisadaptor,
	    indent, cs->AssemblyExceptionadaptor,
	    indent, cs->AssemblyExceptionFeatureadaptor,
	    indent, cs->AssemblyMapperadaptor,
	    indent, cs->CoordSystemadaptor,
	    indent, cs->DatabaseEntryadaptor,
	    indent, cs->DNAAlignFeatureadaptor,
	    indent, cs->Exonadaptor,
	    indent, cs->ExternalDatabaseadaptor,
	    indent, cs->Geneadaptor,
	    indent, cs->KaryotypeBandadaptor,
	    indent, cs->MetaInformationadaptor,
	    indent, cs->MetaCoordinateadaptor,
	    indent, cs->MiscellaneousFeatureadaptor,
	    indent, cs->MiscellaneousSetadaptor,
	    indent, cs->ProteinAlignFeatureadaptor,
	    indent, cs->ProteinFeatureadaptor,
	    indent, cs->RepeatFeatureadaptor,
	    indent, cs->SeqRegionadaptor,
	    indent, cs->Sequenceadaptor,
	    indent, cs->SimpleFeatureadaptor,
	    indent, cs->Sliceadaptor,
	    indent, cs->Transcriptadaptor,
	    indent, cs->Translationadaptor);
    
    ensDatabaseadaptorTrace(cs->Databaseadaptor, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}

#endif




/* @funcstatic registryCoreStyleDel *******************************************
**
** Default destructor for an Ensembl Core-Style Registry.
**
** @param [d] Pcs [RegistryPCoreStyle*] Core-Style Registry address
**
** @return [void]
** @@
******************************************************************************/

static void registryCoreStyleDel(RegistryPCoreStyle *Pcs)
{
    RegistryPCoreStyle pthis = NULL;
    
    if(!Pcs)
        return;
    
    if(!*Pcs)
        return;

    pthis = *Pcs;
    
    /*
     ajDebug("registryCoreStyleDel\n"
	     "  Pcs %p\n"
	     "  *Pcs %p\n",
	     Pcs,
	     *Pcs);
     
     registryCoreStyleTrace(*Pcs, 1);
     */
    
    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */
    
    ensDatabaseadaptorDel(&pthis->Dnaadaptor);
    
    ensAnalysisadaptorDel(&pthis->Analysisadaptor);
    
    ensAssemblyExceptionadaptorDel(&pthis->AssemblyExceptionadaptor);
    
    ensAssemblyExceptionFeatureadaptorDel(
        &pthis->AssemblyExceptionFeatureadaptor);
    
    ensAssemblyMapperadaptorDel(&pthis->AssemblyMapperadaptor);
    
    ensCoordSystemadaptorDel(&pthis->CoordSystemadaptor);
    
    ensDatabaseEntryadaptorDel(&pthis->DatabaseEntryadaptor);
    
    ensDensityFeatureadaptorDel(&pthis->DensityFeatureadaptor);
    
    ensDensityTypeadaptorDel(&pthis->DensityTypeadaptor);
    
    ensDiTagFeatureadaptorDel(&pthis->DiTagFeatureadaptor);
    
    ensDNAAlignFeatureadaptorDel(&pthis->DNAAlignFeatureadaptor);
    
    ensExonadaptorDel(&pthis->Exonadaptor);
    
    ensExternalDatabaseadaptorDel(&pthis->ExternalDatabaseadaptor);
    
    ensGeneadaptorDel(&pthis->Geneadaptor);
    
    ensKaryotypeBandadaptorDel(&pthis->KaryotypeBandadaptor);
    
    ensMarkeradaptorDel(&pthis->Markeradaptor);
    
    ensMarkerFeatureadaptorDel(&pthis->MarkerFeatureadaptor);
    
    ensMetaInformationadaptorDel(&pthis->MetaInformationadaptor);
    
    ensMetaCoordinateadaptorDel(&pthis->MetaCoordinateadaptor);
    
    ensMiscellaneousFeatureadaptorDel(&pthis->MiscellaneousFeatureadaptor);
    
    ensMiscellaneousSetadaptorDel(&pthis->MiscellaneousSetadaptor);
    
    ensPredictionExonadaptorDel(&pthis->PredictionExonadaptor);
    
    ensPredictionTranscriptadaptorDel(&pthis->PredictionTranscriptadaptor);
    
    ensProteinAlignFeatureadaptorDel(&pthis->ProteinAlignFeatureadaptor);
    
    ensProteinFeatureadaptorDel(&pthis->ProteinFeatureadaptor);
    
    ensRepeatFeatureadaptorDel(&pthis->RepeatFeatureadaptor);
    
    ensSeqRegionadaptorDel(&pthis->SeqRegionadaptor);
    
    ensSequenceadaptorDel(&pthis->Sequenceadaptor);
    
    ensSimpleFeatureadaptorDel(&pthis->SimpleFeatureadaptor);
    
    ensSliceadaptorDel(&pthis->Sliceadaptor);
    
    ensTranscriptadaptorDel(&pthis->Transcriptadaptor);
    
    ensTranslationadaptorDel(&pthis->Translationadaptor);
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseadaptorDel(&pthis->Databaseadaptor);
    
    AJFREE(pthis);

    *Pcs = NULL;
    
    return;
}




/* @funcstatic registryGeneticVariationDel ************************************
**
** Default destructor for an Ensembl Genetic Variation Registry.
**
** @param [d] Pgv [RegistryPGeneticVariation*] Genetic Variation Registry
**                                             address
**
** @return [void]
** @@
******************************************************************************/

static void registryGeneticVariationDel(RegistryPGeneticVariation *Pgv)
{
    RegistryPGeneticVariation pthis = NULL;
    
    if(!Pgv)
        return;
    
    if(!*Pgv)
        return;

    pthis = *Pgv;
    
    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */
    
    /* ensGVAlleleadaptorDel(&pthis->Alleleadaptor); */
    
    /* ensGVGenotypeadaptorDel(&pthis->Genotypeadaptor); */
    
    ensGVIndividualadaptorDel(&pthis->Individualadaptor);
    
    ensGVPopulationadaptorDel(&pthis->Populationadaptor);
    
    ensGVSampleadaptorDel(&pthis->Sampleadaptor);
    
    /* ensGVVariationadaptorDel(&pthis->Variationadaptor); */
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseadaptorDel(&pthis->Databaseadaptor);
    
    AJFREE(pthis);

    *Pgv = NULL;
    
    return;
}




/* @funcstatic registryFunctionalGenomicsDel **********************************
**
** Default destructor for an Ensembl Functional Genomics Registry.
**
** @param [d] Pfg [RegistryPFunctionalGenomics*] Functional Genomics Registry
**                                               address
**
** @return [void]
** @@
******************************************************************************/

static void registryFunctionalGenomicsDel(RegistryPFunctionalGenomics *Pfg)
{
    RegistryPFunctionalGenomics pthis = NULL;
    
    if(!Pfg)
        return;
    
    if(!*Pfg)
        return;

    pthis = *Pfg;
    
    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseadaptorDel(&pthis->Databaseadaptor);
    
    AJFREE(pthis);

    *Pfg = NULL;
    
    return;
}




/* @funcstatic registryComparativeGenomicsDel *********************************
**
** Default destructor for an Ensembl Comparative Genomics Registry.
**
** @param [d] Pcg [RegistryPComparativeGenomics*] Comparative Genomics Registry
**                                                address
**
** @return [void]
** @@
******************************************************************************/

static void registryComparativeGenomicsDel(RegistryPComparativeGenomics *Pcg)
{
    RegistryPComparativeGenomics pthis = NULL;
    
    if(!Pcg)
        return;
    
    if(!*Pcg)
        return;

    pthis = *Pcg;
    
    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseadaptorDel(&pthis->Databaseadaptor);
    
    AJFREE(pthis);

    *Pcg = NULL;
    
    return;
}




/* @funcstatic registryGeneOntologyDel ****************************************
**
** Default destructor for an Ensembl Gene Ontology Registry.
**
** @param [d] Pgo [RegistryPGeneOntology*] Gene Ontology Registry address
**
** @return [void]
** @@
******************************************************************************/

static void registryGeneOntologyDel(RegistryPGeneOntology *Pgo)
{
    RegistryPGeneOntology pthis = NULL;
    
    if(!Pgo)
        return;
    
    if(!*Pgo)
        return;

    pthis = *Pgo;
    
    /*
     ajDebug("registryGeneOntologyDel\n"
	     "  Pgo %p\n"
	     "  *Pgo %p\n",
	     Pgo,
	     *Pgo);
     */
    
    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseadaptorDel(&pthis->Databaseadaptor);
    
    AJFREE(pthis);

    *Pgo = NULL;
    
    return;
}




/* @funcstatic registryQualityCheckDel ****************************************
**
** Default destructor for an Ensembl Quality Check Registry.
**
** @param [d] Pqc [RegistryPQualityCheck*] Quality Check Registry address
**
** @return [void]
** @@
******************************************************************************/

static void registryQualityCheckDel(RegistryPQualityCheck *Pqc)
{
    RegistryPQualityCheck pthis = NULL;
    
    if(!Pqc)
        return;
    
    if(!*Pqc)
        return;

    pthis = *Pqc;
    
    /* Delete all Ensembl Object Adaptors based on the Database Adaptor. */
    
    ensQCDatabaseadaptorDel(&pthis->QCDatabaseadaptor);
    
    ensQCSequenceadaptorDel(&pthis->Sequenceadaptor);
    
    ensQCAlignmentadaptorDel(&pthis->Alignmentadaptor);
    
    ensQCDASFeatureadaptorDel(&pthis->DASFeatureadaptor);
    
    ensQCVariationadaptorDel(&pthis->Variationadaptor);
    
    ensQCSubmissionadaptorDel(&pthis->Submissionadaptor);
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseadaptorDel(&pthis->Databaseadaptor);
    
    AJFREE(pthis);

    *Pqc = NULL;
    
    return;
}




/* @funcstatic registryEntryTrace *********************************************
**
** Trace a Registry Entry.
**
** @param [r] entry [const RegistryPEntry] Registry Entry
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool registryEntryTrace(const RegistryPEntry entry, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!entry)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SregistryEntryTrace %p\n"
	    "%S  Registry[0] NULL %p\n"
	    "%S  Registry[1] Core %p\n"
	    "%S  Registry[2] Vega %p\n"
	    "%S  Registry[3] Other Features %p\n"
	    "%S  Registry[4] Copy DNA %p\n"
	    "%S  Registry[5] Genetic Variation %p\n"
	    "%S  Registry[6] Functional Genomics %p\n"
	    "%S  Registry[7] Comparative Genomics %p\n"
	    "%S  Registry[8] Gene Ontology %p\n"
	    "%S  Registry[9] Quality Check %p\n",
	    indent, entry,
	    indent, entry->Registry[0],
	    indent, entry->Registry[1],
	    indent, entry->Registry[2],
	    indent, entry->Registry[3],
	    indent, entry->Registry[4],
	    indent, entry->Registry[5],
	    indent, entry->Registry[6],
	    indent, entry->Registry[7],
	    indent, entry->Registry[8],
	    indent, entry->Registry[9]);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @funcstatic registryEntryDel ***********************************************
**
** Default destructor for an Ensembl Registry Entry.
**
** @param [d] Pentry [RegistryPEntry*] Ensembl Registry Entry address
**
** @return [void]
** @@
******************************************************************************/

static void registryEntryDel(RegistryPEntry *Pentry)
{
    RegistryPEntry pthis = NULL;
    
    if(!Pentry)
        return;
    
    if(!*Pentry)
        return;

    pthis = *Pentry;
    
    /*
     ajDebug("registryEntryDel\n"
	     "  Pentry %p\n"
	     "  *Pentry %p\n",
	     Pentry,
	     *Pentry);
     
     registryEntryTrace(*Pentry, 1);
     */
    
    /* Ensembl Core */
    
    registryCoreStyleDel((RegistryPCoreStyle *) &pthis->Registry[1]);
    
    /* Vega Core */
    
    registryCoreStyleDel((RegistryPCoreStyle *) &pthis->Registry[2]);
    
    /* Other Features Core */
    
    registryCoreStyleDel((RegistryPCoreStyle *) &pthis->Registry[3]);
    
    /* Copy DNA Core */
    
    registryCoreStyleDel((RegistryPCoreStyle *) &pthis->Registry[4]);
    
    /* Ensembl Genetic Variation */
    
    registryGeneticVariationDel((RegistryPGeneticVariation *)
				&pthis->Registry[5]);
    
    /* Ensembl Functional Genomics */
    
    registryFunctionalGenomicsDel((RegistryPFunctionalGenomics *)
				  &pthis->Registry[6]);
    
    /* Ensembl Comparative Genomics */
    
    registryComparativeGenomicsDel((RegistryPComparativeGenomics *)
				   &pthis->Registry[7]);
    
    /* Ensembl Gene Ontology */
    
    registryGeneOntologyDel((RegistryPGeneOntology *)
			    &pthis->Registry[8]);
    
    /* Ensembl Quality Check */
    
    registryQualityCheckDel((RegistryPQualityCheck *)
			    &pthis->Registry[9]);
    
    AJFREE(pthis);

    *Pentry = NULL;
    
    return;
}




/* @func ensRegistryInit ******************************************************
**
** Initialises the Ensembl Registry.
**
** @return [void]
** @@
******************************************************************************/

void ensRegistryInit(void)
{
    registryAliases = ajTablestrNewCaseLen(0);
    
    registryEntries = ajTablestrNewLen(0);
    
    registryIdentifiers = ajListNew();
    
    return;
}




/* @func ensRegistryClear *****************************************************
**
** Clear the Ensembl Registry.
**
** @return [void]
** @@
******************************************************************************/

void ensRegistryClear(void)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    ajTableToarray(registryEntries, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	ajTableRemove(registryEntries, (const void *) keyarray[i]);
	
	ajStrDel((AjPStr *) &keyarray[i]);
	
	registryEntryDel((RegistryPEntry *) &valarray[i]);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return;
}




/* @func ensRegistryExit ******************************************************
**
** Frees the Ensembl Registry.
**
** @return [void]
** @@
******************************************************************************/

void ensRegistryExit(void)
{
    RegistryPIdentifier ri = NULL;
    
    /* Free the AJAX Table of aliases. */
    
    ajTablestrFree(&registryAliases);
    
    /* Clear and free the AJAX Table of Registry Entries. */
    
    ensRegistryClear();
    
    ajTableFree(&registryEntries);
    
    /* Clear and free the AJAX List of Registry Identifiers. */
    
    while(ajListPop(registryIdentifiers, (void **) &ri))
	registryIdentifierDel(&ri);
    
    ajListFree(&registryIdentifiers);
    
    return;
}




/* @func ensRegistryAddAlias **************************************************
**
** Add an alias for a (scientific) species name to the Ensembl Registry.
**
** @param [r] species [const AjPStr] Species
** @param [r] alias [const AjPStr] Alias
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryAddAlias(const AjPStr species, const AjPStr alias)
{
    if(!species)
	return ajFalse;
    
    if(!alias)
	return ajFalse;
    
    /*
     ajDebug("ensRegistryAddAlias\n"
	     "  species '%S'\n"
	     "  alias '%S'\n",
	     species,
	     alias);
     */
    
    if(ajTableFetch(registryAliases, (const void *) alias))
    {
	ajDebug("ensRegistryAddAlias has already added alias '%S'.\n", alias);
	
	return ajFalse;
    }
    
    ajTablePut(registryAliases,
	       (void *) ajStrNewS(alias),
	       (void *) ajStrNewS(species));
    
    return ajTrue;
}




/* @func ensRegistryRemoveAlias ***********************************************
**
** Remove an alias from the Ensembl Registry.
**
** @param [r] alias [const AjPStr] Alias
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryRemoveAlias(const AjPStr alias)
{
    AjPStr key = NULL;
    AjPStr val = NULL;
    
    if(!alias)
	return ajFalse;
    
    val = (AjPStr) ajTableRemoveKey(registryAliases,
				    (const void *) alias,
				    (void **) &key);
    
    ajStrDel(&key);
    ajStrDel(&val);
    
    return ajTrue;
}




/* @func ensRegistryGetSpecies ************************************************
**
** Get the species name to an alias from the Ensembl Registry.
**
** @param [r] alias [const AjPStr] Alias
**
** @return [AjPStr] Species name or NULL
** @@
******************************************************************************/

AjPStr ensRegistryGetSpecies(const AjPStr alias)
{
    if(!alias)
	return NULL;
    
    return (AjPStr) ajTableFetch(registryAliases, (const void *) alias);
}




/* @func ensRegistryLoadAliases ***********************************************
**
** Read aliases for(scientific) species names from an "EnsemblAliases.dat"
** data file and add them to the Ensembl Registry.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryLoadAliases(void)
{
    AjBool block = AJFALSE;
    
    AjPFile infile = NULL;
    
    AjPStr line    = NULL;
    AjPStr species = NULL;
    
    infile = ajDatafileNewInNameC("EnsemblAliases.dat");
    
    if(!infile)
    {
	ajWarn("ensRegistryLoadAliases could no load EnsemblAliases.dat file.");
	
	return ajFalse;
    }
    
    line = ajStrNew();
    
    species = ajStrNew();
    
    while(ajReadlineTrim(infile, &line))
    {
	/*
	** Remove excess white space and ignore all lines that start with
	** a '#' sign. Commented out lines, however, should not break blocks.
	*/
	
	ajStrRemoveWhiteSpaces(&line);
	
	if(ajStrGetCharFirst(line) == '#')
	    continue;
	
	/*
	** Remove comments on the same line as an alias, before again removing
	** excess white space. Finally, remove the quotes, if any, and
	** register the remaining string as alias.
	*/
	
	ajStrCutComments(&line);
	
	ajStrQuoteStrip(&line);
	
	ajStrRemoveWhiteSpaces(&line);
	
	if(!ajStrGetLen(line))
	{
	    /* An empty line indicates the end of a block. */
	    
	    block = ajFalse;
	    
	    continue;
	}
	
	if(block)
	{
	    /* This is a subsequent line of a block, which is an alias. */
	    
	    ensRegistryAddAlias(species, line);
	}
	else
	{
	    /* This is the first line of a block, which is the species. */
	    
	    ajStrAssignS(&species, line);
	    
	    ensRegistryAddAlias(species, species);
	    
	    block = ajTrue;
	}
    }
    
    ajStrDel(&species);
    ajStrDel(&line);
    
    ajFileClose(&infile);
    
    return ajTrue;
}




/* @func ensRegistryTraceAliases **********************************************
**
** Trace Ensembl Registry Aliases.
**
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryTraceAliases(ajint level)
{
    AjPStr indent = NULL;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensRegistryTraceAliases %p\n",
	    indent, registryAliases);
    
    /* FIXME: It is not clear whether that will work satisfactorily. */
    
    /*
    ** FIXME: There is a new line character missing after the (string)
    ** table trace line.
    ** TODO: Report this to EMBOSS.
    */
    
    ajTablestrTrace(registryAliases);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensRegistryTraceEntries **********************************************
**
** Trace Ensembl Registry Aliases.
**
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryTraceEntries(ajint level)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjPStr indent = NULL;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensRegistryTraceEntries %p\n",
	    indent, registryEntries);
    
    ajTableToarray(registryEntries, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	ajDebug("%S  Species '%S'\n", indent, (AjPStr) keyarray[i]);
	
	registryEntryTrace((RegistryPEntry) valarray[i], level + 2);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensRegistryAddDatabaseadaptor ****************************************
**
** Add an Ensembl Database Adaptor to the Ensembl Registry.
**
** @param [r] dbc [EnsPDatabaseConnection] Ensembl Database Connection
** @param [u] database [AjPStr] Database name (optional). If not provided, the
**                              database name in the Database Connection will
**                              be used.
** @param [u] alias [AjPStr] Species name or alias
** @param [r] group [AjEnum] Group
** @param [r] multi [AjBool] Multiple species
** @param [r] identifier [ajuint] Species identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** This function will aim to resolve an eventual alias to a valid species name.
** If the alias has not been registered before, it will strip underscores from
** the alias to directly permit an Ensembl database name prefix, such as
** "homo_sapiens". If an alias without underscores has also not been registered
** before, it will register the alias without underscores as species and the
** the prefix with underscores as species.
** Ideally aliases have been loaded from the corresponding Ensembl data file
** (EnsemblAliases.dat) before.
******************************************************************************/

AjBool ensRegistryAddDatabaseadaptor(EnsPDatabaseConnection dbc,
                                     AjPStr database,
                                     AjPStr alias,
                                     AjEnum group,
                                     AjBool multi,
                                     ajuint identifier)
{
    AjBool registered = AJFALSE;
    
    AjPStr species = NULL;
    AjPStr temp    = NULL;
    
    RegistryPEntry entry             = NULL;
    RegistryPCoreStyle ecs           = NULL;
    RegistryPGeneticVariation egv    = NULL;
    RegistryPFunctionalGenomics efg  = NULL;
    RegistryPComparativeGenomics ecg = NULL;
    RegistryPGeneOntology ego        = NULL;
    
    if(!dbc)
	return ajFalse;
    
    if(!database)
	return ajFalse;
    
    if(!alias)
	return ajFalse;
    
    /*
     ajDebug("ensRegistryAddDatabaseadaptor\n"
	     "  dbc %p\n"
	     "  database '%S'\n"
	     "  alias '%S'\n"
	     "  group '%s'\n"
	     "  multi %B\n"
	     "  identifier %u\n",
	     dbc,
	     database,
	     alias,
	     ensDatabaseadaptorGroupToChar(group),
	     multi,
	     identifier);
     
     ensDatabaseConnectionTrace(dbc, 1);
     */
    
    /*
    ** Resolve an eventual species alias to the species name.
    ** Check if the alias eventually an Ensembl database name prefix with
    ** underscores (e. g. 'homo_sapiens') has already been registered as an
    ** alias before.
    */
    
    species = ensRegistryGetSpecies(alias);
    
    if(!species)
    {
	/*
	** If this was not the case check if the alias without
	** underscores has been registered as an alias before.
	*/
	
	temp = ajStrNewS(alias);
	
	ajStrExchangeCC(&temp, "_", " ");
	
	species = ensRegistryGetSpecies(temp);
	
	if(!species)
	{
	    /*
	    ** If the alias without underscores has also not been registered
	    ** as an alias before, register it as species and alias before
	    ** registering it as species with the underscores as an alias.
	    */
	    
	    ensRegistryAddAlias(temp, temp);
	    ensRegistryAddAlias(temp, alias);
	    
	    species = ensRegistryGetSpecies(alias);
	}
	
	ajStrDel(&temp);
    }
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries, (const void *) species);
    
    if(!entry)
    {
	AJNEW0(entry);
	
        ajTablePut(registryEntries,
		   (void *) ajStrNewRef(species),
		   (void *) entry);
    }
    
    switch(group)
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    if(entry->Registry[group])
	    {
		ecs = (RegistryPCoreStyle) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(ecs);
		
		entry->Registry[group] = (void *) ecs;
	    }
	    
	    if(ecs->Databaseadaptor)
	    {
		if(!ensDatabaseadaptorMatchComponents(ecs->Databaseadaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseadaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseadaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		ecs->Databaseadaptor =
		ensDatabaseadaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseadaptorGroupGeneticVariation:
	    
	    if(entry->Registry[group])
	    {
		egv = (RegistryPGeneticVariation) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(egv);
		
		entry->Registry[group] = (void *) egv;
	    }
	    
	    if(egv->Databaseadaptor)
	    {
		if(!ensDatabaseadaptorMatchComponents(egv->Databaseadaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseadaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseadaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		egv->Databaseadaptor =
		ensDatabaseadaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseadaptorGroupFunctionalGenomics:
	    
	    if(entry->Registry[group])
	    {
		efg = (RegistryPFunctionalGenomics) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(efg);
		
		entry->Registry[group] = (void *) efg;
	    }
	    
	    if(efg->Databaseadaptor)
	    {
		if(!ensDatabaseadaptorMatchComponents(efg->Databaseadaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseadaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseadaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		efg->Databaseadaptor =
		ensDatabaseadaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseadaptorGroupComparativeGenomics:
	    
	    if(entry->Registry[group])
	    {
		ecg = (RegistryPComparativeGenomics) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(ecg);
		
		entry->Registry[group] = (void *) ecg;
	    }
	    
	    if(ecg->Databaseadaptor)
	    {
		if(!ensDatabaseadaptorMatchComponents(ecg->Databaseadaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseadaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseadaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		ecg->Databaseadaptor =
		ensDatabaseadaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseadaptorGroupGeneOntology:
	    
	    if(entry->Registry[group])
	    {
		ego = (RegistryPGeneOntology) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(ego);
		
		entry->Registry[group] = (void *) ego;
	    }
	    
	    if(ego->Databaseadaptor)
	    {
		if(!ensDatabaseadaptorMatchComponents(ego->Databaseadaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseadaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseadaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		ego->Databaseadaptor =
		ensDatabaseadaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseadaptorGroupQualityCheck:
	    
	case ensEDatabaseadaptorGroupPipeline:
	    
	case ensEDatabaseadaptorGroupHive:
	    
	case ensEDatabaseadaptorGroupCoreExpressionEST:
	    
	case ensEDatabaseadaptorGroupCoreExpressionGNF:
	    
	case ensEDatabaseadaptorGroupAncestral:
	    
	case ensEDatabaseadaptorGroupWebsite:
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryAddDatabaseadaptor got a request for an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n", group);
	    
	    registered = ajFalse;
    }
    
    return registered;
}




/* @func ensRegistryRemoveDatabaseadaptor *************************************
**
** Remove an Ensembl Database Adaptor from the Ensembl Registry and delete it.
**
** @param [d] Pdba [EnsPDatabaseadaptor*] Ensembl Database Adaptor address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryRemoveDatabaseadaptor(EnsPDatabaseadaptor *Pdba)
{
    AjEnum group = ensEDatabaseadaptorGroupNULL;
    
    RegistryPEntry entry              = NULL;
    RegistryPCoreStyle ecs           = NULL;
    RegistryPGeneticVariation egv    = NULL;
    RegistryPFunctionalGenomics efg  = NULL;
    RegistryPComparativeGenomics ecg = NULL;
    RegistryPGeneOntology ego        = NULL;
    
    if(!Pdba)
        return ajFalse;
    
    if(!*Pdba)
	return ajFalse;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(*Pdba));
    
    if(!entry)
    {
	ajWarn("ensRegistryRemoveDatabaseadaptor could not get "
	       "a Registry Entry for species '%S'.\n",
	       ensDatabaseadaptorGetSpecies(*Pdba));
	
	*Pdba = (EnsPDatabaseadaptor) NULL;
	
	return ajTrue;
    }
    
    group = ensDatabaseadaptorGetGroup(*Pdba);
    
    switch(group)
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle) entry->Registry[group];
	    
	    if(ecs)
	    {
		if(ecs->Databaseadaptor == *Pdba)
		    registryCoreStyleDel((RegistryPCoreStyle*)
					 &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseadaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation) entry->Registry[group];
	    
	    if(egv)
	    {
		if(egv->Databaseadaptor == *Pdba)
		    registryGeneticVariationDel((RegistryPGeneticVariation *)
						&(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseadaptorGroupFunctionalGenomics:
	    
	    efg = (RegistryPFunctionalGenomics) entry->Registry[group];
	    
	    if(efg)
	    {
		if(efg->Databaseadaptor == *Pdba)
		    registryFunctionalGenomicsDel(
                        (RegistryPFunctionalGenomics *)
                        &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseadaptorGroupComparativeGenomics:
	    
	    ecg = (RegistryPComparativeGenomics) entry->Registry[group];
	    
	    if(ecg)
	    {
		if(ecg->Databaseadaptor == *Pdba)
		    registryComparativeGenomicsDel(
                        (RegistryPComparativeGenomics *)
                        &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseadaptorGroupGeneOntology:
	    
	    ego = (RegistryPGeneOntology) entry->Registry[group];
	    
	    if(ego)
	    {
		if(ego->Databaseadaptor == *Pdba)
		    registryGeneOntologyDel((RegistryPGeneOntology *)
					    &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseadaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryRemoveDatabaseadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n", group);
    }
    
    /*
    ** TODO: Test if this entry remains completely empty.
    ** If this was the case it could be removed.
    ** Should we also automatically delete all Databaseadaptors that get the
    ** DNA via the Core adaptor in case the Core adaptor gets deleted?
    */
    
    /* Clear the Ensembl Database Adaptor pointer. */
    
    *Pdba = (EnsPDatabaseadaptor) NULL;
    
    return ajTrue;
}




/* @func ensRegistryGetAllDatabaseadaptors ************************************
**
** Get all Ensembl Database Adaptors from the Ensembl Registry
** and optionally filter them by species or group elements.
**
** @param [rN] group [AjEnum] Ensembl Database Adaptor group element
** @param [rN] alias [const AjPStr] Ensembl Database Adaptor species element
** @param [w] list [AjPList] AJAX List of Ensembl Database Adaptors
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryGetAllDatabaseadaptors(AjEnum group,
                                         const AjPStr alias,
                                         AjPList list)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    register ajint j = 0;
    
    AjPStr species = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    RegistryPCoreStyle ecs           = NULL;
    RegistryPGeneticVariation egv    = NULL;
    RegistryPFunctionalGenomics efg  = NULL;
    RegistryPComparativeGenomics ecg = NULL;
    RegistryPGeneOntology ego        = NULL;
    
    if(!list)
	return ajFalse;
    
    species = ensRegistryGetSpecies(alias);
    
    ajTableToarray(registryEntries, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	for(j = 1; j <= 9; j++)
	{
	    if(group && (group != j))
		continue;
	    
	    switch(j)
	    {
		case ensEDatabaseadaptorGroupCore:
		    
		case ensEDatabaseadaptorGroupVega:
		    
		case ensEDatabaseadaptorGroupOtherFeatures:
		    
		case ensEDatabaseadaptorGroupCopyDNA:
		    
		    ecs = (RegistryPCoreStyle)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(ecs && ecs->Databaseadaptor)
		    {
			dba = ecs->Databaseadaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseadaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseadaptorGroupGeneticVariation:
		    
		    egv = (RegistryPGeneticVariation)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(egv && egv->Databaseadaptor)
		    {
			dba = egv->Databaseadaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseadaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseadaptorGroupFunctionalGenomics:
		    
		    efg = (RegistryPFunctionalGenomics)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(efg && efg->Databaseadaptor)
		    {
			dba = efg->Databaseadaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseadaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseadaptorGroupComparativeGenomics:
		    
		    ecg = (RegistryPComparativeGenomics)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(ecg && ecg->Databaseadaptor)
		    {
			dba = ecg->Databaseadaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseadaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseadaptorGroupGeneOntology:
		    
		    ego = (RegistryPGeneOntology)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(ego && ego->Databaseadaptor)
		    {
			dba = ego->Databaseadaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseadaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		default:
		    
		    ajWarn("ensRegistryGetAllDatabaseadaptors got an"
			   "unexpected group %d.\n", j);
	    }
	}
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return ajTrue;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Registry object.
**
** @fdata [EnsPDatabaseadaptor]
** @fnote None
**
** @nam3rule Get Return Registry attribute(s)
** @nam4rule GetAnalysisadaptor Return the Ensembl Analysis Adaptor
** @nam4rule GetAssemblyExceptionadaptor Return the Ensembl Assembly
**                                       Exception Adaptor
** @nam4rule GetAssemblyExceptionFeatureadaptor Return the Ensembl Assembly
**                                              Exception Feature Adaptor
** @nam4rule GetAssemblyMapperadaptor Return the Ensembl Assembly
**                                    Mapper Adaptor
** @nam4rule GetCoordSystemadaptor Return the Ensembl Coordinate System Adaptor
** @nam4rule GetDatabaseEntryadaptor Return the Ensembl Database Entry Adaptor
** @nam4rule GetDNAAlignFeatureadaptor Return the Ensembl DNA Align Feature
**                                     Adaptor
** @nam4rule GetExonadaptor Return the Ensembl Exon Adaptor
** @nam4rule GetExternalDatabaseadaptor Return the Ensembl External Database
**                                      Adaptor
** @nam4rule GetGeneadaptor Return the Ensembl Gene Adaptor
** @nam4rule GetKaryotypeBandadaptor Return the Ensembl Karyotype Band Adaptor
** @nam4rule GetMetaInformationadaptor Return the Ensembl Meta-Information
**                                     Adaptor
** @nam4rule GetMetaCoordinateadaptor Return the Ensembl Meta-Coordinate
**                                    Adaptor
**
** @argrule * dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule Analysisadaptor [EnsPAnalysisadaptor]
** @valrule AssemblyException [EnsPAssemblyException]
** @valrule AssemblyExceptionFeatureadaptor [EnsPAssemblyExceptionFeatureadaptor]
** @valrule AssemblyMapperadaptor [EnsPAssemblyMapperadaptor]
** @valrule CoordSystemadaptor [EnsPCoordSystemadaptor]
** @valrule DatabaseEntryadaptor [EnsPDatabaseEntryadaptor]
** @valrule DNAAlignFeatureadaptor [EnsPDNAAlignFeatureadaptor]
** @valrule Exonadaptor [EnsPExonadaptor]
** @valrule ExternalDatabaseadaptor [EnsPExternalDatabaseadaptor]
** @valrule Geneadaptor [EnsPGeneadaptor]
** @valrule KaryotypeBandadaptor [EnsPKaryotypeBandadaptor]
** @valrule MetaInformationadaptor [EnsPMetaInformationadaptor]
**
** @fcategory use
******************************************************************************/




/* @func ensRegistryGetDatabaseadaptor ****************************************
**
** Get an Ensembl Database Adaptor from the Ensembl Registry.
**
** @param [r] group [AjEnum] Ensembl Database Adaptor group
** @param [r] alias [const AjPStr] Scientific species name or alias name
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseadaptor ensRegistryGetDatabaseadaptor(AjEnum group,
                                                  const AjPStr alias)
{
    AjPStr species = NULL;
    
    RegistryPEntry entry             = NULL;
    RegistryPCoreStyle ecs           = NULL;
    RegistryPGeneticVariation egv    = NULL;
    RegistryPFunctionalGenomics efg  = NULL;
    RegistryPComparativeGenomics ecg = NULL;
    RegistryPGeneOntology ego        = NULL;
    
    if(!group)
	return NULL;
    
    if(!alias)
	return NULL;
    
    species = ensRegistryGetSpecies(alias);
    
    /*
     ajDebug("ensRegistryGetDatabaseadaptor alias '%S' -> species '%S'\n",
	     alias, species);
     */
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries, (const void *) species);
    
    if(!entry)
    {
	ajDebug("ensRegistryGetDatabaseadaptor could not get a "
		"Registry Entry for group '%s' and species '%S'.\n",
		ensDatabaseadaptorGroupToChar(group), species);
	
	return NULL;
    }
    
    switch(group)
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle) entry->Registry[group];
	    
	    if(ecs)
		return ecs->Databaseadaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseadaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseadaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseadaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation) entry->Registry[group];
	    
	    if(egv)
		return egv->Databaseadaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseadaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseadaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseadaptorGroupFunctionalGenomics:
	    
	    efg = (RegistryPFunctionalGenomics) entry->Registry[group];
	    
	    if(efg)
		return efg->Databaseadaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseadaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseadaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseadaptorGroupComparativeGenomics:
	    
	    ecg = (RegistryPComparativeGenomics) entry->Registry[group];
	    
	    if(ecg)
		return ecg->Databaseadaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseadaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseadaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseadaptorGroupGeneOntology:
	    
	    ego = (RegistryPGeneOntology) entry->Registry[group];
	    
	    if(ego)
		return ego->Databaseadaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseadaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseadaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDatabaseadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n", group);
    }
    
    return NULL;
}




/* @func ensRegistryGetDnaadaptor *********************************************
**
** Get an Ensembl Dna Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor or NULL
** @@
******************************************************************************/

#if AJFALSE

/*
** FIXME: Is a separate DNA Adaptor really neccessary or could just the
** generic Ensembl Core Adaptor of the same species be used?
** There could be a problem in the gene-build procedure though where one core
** database has the DNA and another core database has the gene set.
** OtherFeatures may not be sufficient here?
*/

EnsPDatabaseadaptor ensRegistryGetDnaadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry = NULL;
    
    if(!dba)
	return NULL;
    
    entry = registryGetEntryByDatabaseadaptor(dba);
    
    if(entry)
	return entry->Dnaadaptor;

    return NULL;
}

#endif




/* @func ensRegistryGetAnalysisadaptor ****************************************
**
** Get an Ensembl Analysis Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAnalysisadaptor] Ensembl Analysis Adaptor or NULL
** @@
******************************************************************************/

EnsPAnalysisadaptor ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Analysisadaptor)
		ecs->Analysisadaptor = ensAnalysisadaptorNew(dba);
		
		return ecs->Analysisadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAnalysisadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetAssemblyExceptionadaptor *******************************
**
** Get an Ensembl Assembly Exception Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyExceptionadaptor] Ensembl Assembly Exception Adaptor
**                                        or NULL
** @@
******************************************************************************/

EnsPAssemblyExceptionadaptor ensRegistryGetAssemblyExceptionadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->AssemblyExceptionadaptor)
		ecs->AssemblyExceptionadaptor =
		    ensAssemblyExceptionadaptorNew(dba);
		
		return ecs->AssemblyExceptionadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAssemblyExceptionadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetAssemblyExceptionFeatureadaptor ************************
**
** Get an Ensembl Assembly Exception Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyExceptionFeatureadaptor] Ensembl Assembly Exception
**                                               Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyExceptionFeatureadaptor ensRegistryGetAssemblyExceptionFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->AssemblyExceptionFeatureadaptor)
		ecs->AssemblyExceptionFeatureadaptor =
		    ensAssemblyExceptionFeatureadaptorNew(dba);
		
		return ecs->AssemblyExceptionFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAssemblyExceptionFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetAssemblyMapperadaptor **********************************
**
** Get an Ensembl Assembly Mapper Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyMapperadaptor] Ensembl Assembly Mapper Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyMapperadaptor ensRegistryGetAssemblyMapperadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->AssemblyMapperadaptor)
		ecs->AssemblyMapperadaptor = ensAssemblyMapperadaptorNew(dba);
		
		return ecs->AssemblyMapperadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAssemblyMapperadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetCoordSystemadaptor *************************************
**
** Get an Ensembl Coordinate System Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPCoordSystemadaptor] Ensembl Coordinate System Adaptor or NULL
** @@
******************************************************************************/

EnsPCoordSystemadaptor ensRegistryGetCoordSystemadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->CoordSystemadaptor)
		ecs->CoordSystemadaptor =
		    ensCoordSystemadaptorNew(dba);
		
		return ecs->CoordSystemadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetCoordSystemadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDatabaseEntryadaptor ***********************************
**
** Get an Ensembl Database Entry Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDatabaseEntryadaptor] Ensembl Database Entry Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseEntryadaptor ensRegistryGetDatabaseEntryadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->DatabaseEntryadaptor)
		ecs->DatabaseEntryadaptor = ensDatabaseEntryadaptorNew(dba);
		
		return ecs->DatabaseEntryadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDatabaseEntryadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDensityFeatureadaptor **********************************
**
** Get an Ensembl Density Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDensityFeatureadaptor] Ensembl Density Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDensityFeatureadaptor ensRegistryGetDensityFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->DensityFeatureadaptor)
		ecs->DensityFeatureadaptor = ensDensityFeatureadaptorNew(dba);
		
		return ecs->DensityFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDensityFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDensityTypeadaptor *************************************
**
** Get an Ensembl Density Type Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor or NULL
** @@
******************************************************************************/

EnsPDensityTypeadaptor ensRegistryGetDensityTypeadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->DensityTypeadaptor)
		ecs->DensityTypeadaptor = ensDensityTypeadaptorNew(dba);
		
		return ecs->DensityTypeadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDensityTypeadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDiTagFeatureadaptor ************************************
**
** Get an Ensembl DiTag Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDiTagFeatureadaptor ensRegistryGetDiTagFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->DiTagFeatureadaptor)
		ecs->DiTagFeatureadaptor = ensDiTagFeatureadaptorNew(dba);
		
		return ecs->DiTagFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDiTagFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDNAAlignFeatureadaptor *********************************
**
** Get an Ensembl DNA Align Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDNAAlignFeatureadaptor] Ensembl DNA Align Feature Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPDNAAlignFeatureadaptor ensRegistryGetDNAAlignFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->DNAAlignFeatureadaptor)
		ecs->DNAAlignFeatureadaptor =
		    ensDNAAlignFeatureadaptorNew(dba);
		
		return ecs->DNAAlignFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDNAAlignFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetExonadaptor ********************************************
**
** Get an Ensembl Exon Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPExonadaptor] Ensembl Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPExonadaptor ensRegistryGetExonadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Exonadaptor)
		ecs->Exonadaptor = ensExonadaptorNew(dba);
		
		return ecs->Exonadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetExonadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetExternalDatabaseadaptor ********************************
**
** Get an Ensembl External Database Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPExternalDatabaseadaptor] Ensembl External Database Adaptor or
**                                       NULL
** @@
******************************************************************************/

EnsPExternalDatabaseadaptor ensRegistryGetExternalDatabaseadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->ExternalDatabaseadaptor)
		ecs->ExternalDatabaseadaptor =
		    ensExternalDatabaseadaptorNew(dba);
		
		return ecs->ExternalDatabaseadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetExternalDatabaseadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGeneadaptor ********************************************
**
** Get an Ensembl Gene Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGeneadaptor] Ensembl Gene Adaptor or NULL
** @@
******************************************************************************/

EnsPGeneadaptor ensRegistryGetGeneadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Geneadaptor)
		ecs->Geneadaptor = ensGeneadaptorNew(dba);
		
		return ecs->Geneadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGeneadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetKaryotypeBandadaptor ***********************************
**
** Get an Ensembl Karyotype Band Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPKaryotypeBandadaptor] Ensembl Karyotype Band Adaptor or NULL
** @@
******************************************************************************/

EnsPKaryotypeBandadaptor ensRegistryGetKaryotypeBandadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->KaryotypeBandadaptor)
		ecs->KaryotypeBandadaptor = ensKaryotypeBandadaptorNew(dba);
		
		return ecs->KaryotypeBandadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetKaryotypeBandadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMarkeradaptor ******************************************
**
** Get an Ensembl Marker Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkeradaptor] Ensembl Marker Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkeradaptor ensRegistryGetMarkeradaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Markeradaptor)
		ecs->Markeradaptor =
		    ensMarkeradaptorNew(dba);
		
		return ecs->Markeradaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMarkeradaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMarkerFeatureadaptor ***********************************
**
** Get an Ensembl Marker Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkerFeatureadaptor] Ensembl Marker Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkerFeatureadaptor ensRegistryGetMarkerFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->MarkerFeatureadaptor)
		ecs->MarkerFeatureadaptor =
		    ensMarkerFeatureadaptorNew(dba);
		
		return ecs->MarkerFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMarkerFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMetaInformationadaptor *********************************
**
** Get an Ensembl Meta-Information Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMetaInformationadaptor] Ensembl Meta-Information Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPMetaInformationadaptor ensRegistryGetMetaInformationadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->MetaInformationadaptor)
		ecs->MetaInformationadaptor =
		    ensMetaInformationadaptorNew(dba);
		
		return ecs->MetaInformationadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMetaInformationadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMetaCoordinateadaptor **********************************
**
** Get an Ensembl Meta-Coordinate Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMetaCoordinateadaptor] Ensembl Meta-Coordinate Adaptor or NULL
** @@
******************************************************************************/

EnsPMetaCoordinateadaptor ensRegistryGetMetaCoordinateadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->MetaCoordinateadaptor)
		ecs->MetaCoordinateadaptor = ensMetaCoordinateadaptorNew(dba);
		
		return ecs->MetaCoordinateadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMetaCoordinateadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMiscellaneousFeatureadaptor ****************************
**
** Get an Ensembl Miscellaneous Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                           Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPMiscellaneousFeatureadaptor ensRegistryGetMiscellaneousFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->MiscellaneousFeatureadaptor)
		ecs->MiscellaneousFeatureadaptor =
		    ensMiscellaneousFeatureadaptorNew(dba);
		
		return ecs->MiscellaneousFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMiscellaneousFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMiscellaneousSetadaptor ********************************
**
** Get an Ensembl Miscellaneous Set Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous Set Adaptor
**                                       or NULL
** @@
******************************************************************************/

EnsPMiscellaneousSetadaptor ensRegistryGetMiscellaneousSetadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->MiscellaneousSetadaptor)
		ecs->MiscellaneousSetadaptor =
		    ensMiscellaneousSetadaptorNew(dba);
		
		return ecs->MiscellaneousSetadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMiscellaneousSetadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetPredictionExonadaptor **********************************
**
** Get an Ensembl Prediction Exon Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictionExonadaptor] Ensembl Prediction Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionExonadaptor ensRegistryGetPredictionExonadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->PredictionExonadaptor)
		ecs->PredictionExonadaptor =
		    ensPredictionExonadaptorNew(dba);
		
		return ecs->PredictionExonadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetPredictionExonadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetPredictionTranscriptadaptor ****************************
**
** Get an Ensembl Prediction Transcript Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                           Transcript Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionTranscriptadaptor ensRegistryGetPredictionTranscriptadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->PredictionTranscriptadaptor)
		ecs->PredictionTranscriptadaptor =
		    ensPredictionTranscriptadaptorNew(dba);
		
		return ecs->PredictionTranscriptadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetPredictionTranscriptadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetProteinAlignFeatureadaptor *****************************
**
** Get an Ensembl Protein Align Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPProteinAlignFeatureadaptor] Ensembl Protein Align Feature
**                                          Adaptor or NULL
** @@
******************************************************************************/

EnsPProteinAlignFeatureadaptor ensRegistryGetProteinAlignFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->ProteinAlignFeatureadaptor)
		ecs->ProteinAlignFeatureadaptor =
		    ensProteinAlignFeatureadaptorNew(dba);
		
		return ecs->ProteinAlignFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetProteinAlignFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetProteinFeatureadaptor **********************************
**
** Get an Ensembl Protein Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPProteinFeatureadaptor] Ensembl Protein Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPProteinFeatureadaptor ensRegistryGetProteinFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->ProteinFeatureadaptor)
		ecs->ProteinFeatureadaptor = ensProteinFeatureadaptorNew(dba);
		
		return ecs->ProteinFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetProteinFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetRepeatFeatureadaptor ***********************************
**
** Get an Ensembl Repeat Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPRepeatFeatureadaptor] Ensembl Repeat Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPRepeatFeatureadaptor ensRegistryGetRepeatFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->RepeatFeatureadaptor)
		ecs->RepeatFeatureadaptor = ensRepeatFeatureadaptorNew(dba);
		
		return ecs->RepeatFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetRepeatFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSeqRegionadaptor ***************************************
**
** Get an Ensembl Sequence Region Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSeqRegionadaptor] Ensembl Sequence Region Adaptor or NULL
** @@
******************************************************************************/

EnsPSeqRegionadaptor ensRegistryGetSeqRegionadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->SeqRegionadaptor)
		ecs->SeqRegionadaptor = ensSeqRegionadaptorNew(dba);
		
		return ecs->SeqRegionadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSeqRegionadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSimpleFeatureadaptor ***********************************
**
** Get an Ensembl Simple Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSimpleFeatureadaptor] Ensembl Simple Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPSimpleFeatureadaptor ensRegistryGetSimpleFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->SimpleFeatureadaptor)
		ecs->SimpleFeatureadaptor = ensSimpleFeatureadaptorNew(dba);
		
		return ecs->SimpleFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSimpleFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSequenceadaptor ****************************************
**
** Get an Ensembl Sequence Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSequenceadaptor] Ensembl Sequence Adaptor or NULL
** @@
******************************************************************************/

EnsPSequenceadaptor ensRegistryGetSequenceadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Sequenceadaptor)
		ecs->Sequenceadaptor = ensSequenceadaptorNew(dba);
		
		return ecs->Sequenceadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSequenceadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSliceadaptor *******************************************
**
** Get an Ensembl Slice Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPSliceadaptor] Ensembl Slice Adaptor or NULL
** @@
******************************************************************************/

EnsPSliceadaptor ensRegistryGetSliceadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Sliceadaptor)
		ecs->Sliceadaptor = ensSliceadaptorNew(dba);
		
		return ecs->Sliceadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSliceadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetTranscriptadaptor **************************************
**
** Get an Ensembl Transcript Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPTranscriptadaptor] Ensembl Transcript Adaptor or NULL
** @@
******************************************************************************/

EnsPTranscriptadaptor ensRegistryGetTranscriptadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Transcriptadaptor)
		ecs->Transcriptadaptor = ensTranscriptadaptorNew(dba);
		
		return ecs->Transcriptadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetTranscriptadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetTranslationadaptor *************************************
**
** Get an Ensembl Translation Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPTranslationadaptor] Ensembl Translation Adaptor or NULL
** @@
******************************************************************************/

EnsPTranslationadaptor ensRegistryGetTranslationadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupCore:
	    
	case ensEDatabaseadaptorGroupVega:
	    
	case ensEDatabaseadaptorGroupOtherFeatures:
	    
	case ensEDatabaseadaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!ecs->Translationadaptor)
		ecs->Translationadaptor = ensTranslationadaptorNew(dba);
		
		return ecs->Translationadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetTranslationadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCDatabaseadaptor **************************************
**
** Get an Ensembl QC Database Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCDatabaseadaptor] Ensembl QC Database Adaptor or NULL
** @@
******************************************************************************/

EnsPQCDatabaseadaptor ensRegistryGetQCDatabaseadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!eqc->Databaseadaptor)
		eqc->QCDatabaseadaptor =
		    ensQCDatabaseadaptorNew(dba);
		
		return eqc->QCDatabaseadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCDatabaseadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCSequenceadaptor **************************************
**
** Get an Ensembl QC Sequence Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCSequenceadaptor] Ensembl QC Sequence Adaptor or NULL
** @@
******************************************************************************/

EnsPQCSequenceadaptor ensRegistryGetQCSequenceadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!eqc->Sequenceadaptor)
		eqc->Sequenceadaptor =
		    ensQCSequenceadaptorNew(dba);
		
		return eqc->Sequenceadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCSequenceadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCAlignmentadaptor *************************************
**
** Get an Ensembl QC Alignment Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCAlignmentadaptor] Ensembl QC Alignment Adaptor or NULL
** @@
******************************************************************************/

EnsPQCAlignmentadaptor ensRegistryGetQCAlignmentadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!eqc->Alignmentadaptor)
		eqc->Alignmentadaptor =
		    ensQCAlignmentadaptorNew(dba);
		
		return eqc->Alignmentadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCAlignmentadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCDASFeatureadaptor ************************************
**
** Get an Ensembl QC DAS Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPQCDASFeatureadaptor ensRegistryGetQCDASFeatureadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!eqc->DASFeatureadaptor)
		eqc->DASFeatureadaptor =
		    ensQCDASFeatureadaptorNew(dba);
		
		return eqc->DASFeatureadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCDASFeatureadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCVariationadaptor *************************************
**
** Get an Ensembl QC Variation Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor or NULL
** @@
******************************************************************************/

EnsPQCVariationadaptor ensRegistryGetQCVariationadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!eqc->Variationadaptor)
		eqc->Variationadaptor =
		    ensQCVariationadaptorNew(dba);
		
		return eqc->Variationadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCVariationadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCSubmissionadaptor ************************************
**
** Get an Ensembl QC Submission Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor or NULL
** @@
******************************************************************************/

EnsPQCSubmissionadaptor ensRegistryGetQCSubmissionadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!eqc->Submissionadaptor)
		eqc->Submissionadaptor =
		    ensQCSubmissionadaptorNew(dba);
		
		return eqc->Submissionadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCSubmissionadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGVIndividualadaptor ************************************
**
** Get an Ensembl Genetic Variation Individual Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGVIndividualadaptor] Ensembl Genetic Variation
**                                   Individual Adaptor or NULL
** @@
******************************************************************************/

EnsPGVIndividualadaptor ensRegistryGetGVIndividualadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation egv = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!egv->Individualadaptor)
		egv->Individualadaptor =
		    ensGVIndividualadaptorNew(dba);
		
		return egv->Individualadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGVIndividualadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGVPopulationadaptor ************************************
**
** Get an Ensembl Genetic Variation Population Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGVPopulationadaptor] Ensembl Genetic Variation
**                                   Population Adaptor or NULL
** @@
******************************************************************************/

EnsPGVPopulationadaptor ensRegistryGetGVPopulationadaptor(
    EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation egv = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!egv->Populationadaptor)
		egv->Populationadaptor =
		    ensGVPopulationadaptorNew(dba);
		
		return egv->Populationadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGVPopulationadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGVVariationadaptor *************************************
**
** Get an Ensembl Genetic Variation Variation Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGVVariationadaptor] Ensembl Genetic Variation
**                                  Variation Adaptor or NULL
** @@
******************************************************************************/

EnsPGVVariationadaptor ensRegistryGetGVVariationadaptor(EnsPDatabaseadaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation egv = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseadaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseadaptorGetGroup(dba))
    {
	case ensEDatabaseadaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation)
	    entry->Registry[ensDatabaseadaptorGetGroup(dba)];
	    
	    if(!egv->Variationadaptor)
		egv->Variationadaptor =
		    ensGVVariationadaptorNew(dba);
		
		return egv->Variationadaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGVVariationadaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseadaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @funcstatic registryLoadCollection *****************************************
**
** Load species form a collection database, register species names in the
** alias table and instatiate Ensembl Database Adaptor objects.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
** @param [r] dbname [AjPStr] Database name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool registryLoadCollection(EnsPDatabaseConnection dbc, AjPStr dbname)
{
    char *txtdbname = NULL;
    
    ajuint identifier = 0;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr alias     = NULL;
    AjPStr species   = NULL;
    AjPStr statement = NULL;
    
    if(!dbc)
	return ajFalse;
    
    ensDatabaseConnectionEscapeC(dbc, &txtdbname, dbname);
    
    statement = ajFmtStr("SELECT "
			 "%s.meta.species_id, "
			 "%s.meta.meta_value "
			 "FROM "
			 "%s.meta "
			 "WHERE "
			 "%s.meta.meta_key = 'species.db_name'",
			 txtdbname,
			 txtdbname,
			 txtdbname,
			 txtdbname);
    
    ajCharDel(&txtdbname);
    
    sqls = ensDatabaseConnectionSqlstatementNew(dbc, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	alias = ajStrNew();
	species = ajStrNew();
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToStr(sqlr, &alias);
	
	ajStrAssignS(&species, alias);
	
	ajStrExchangeCC(&species, "_", " ");
	
	ensRegistryAddAlias(species, alias);
	
	ensRegistryAddDatabaseadaptor(dbc,
				      dbname,
				      species,
				      ensEDatabaseadaptorGroupCore,
				      ajTrue,
				      identifier);
	
	ajStrDel(&alias);
	
	ajStrDel(&species);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensRegistryLoadFromServer ********************************************
**
** Automatically register databases on an SQL server with the Ensembl Registry.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryLoadFromServer(EnsPDatabaseConnection dbc)
{
    AjEnum egroup = ensEDatabaseadaptorGroupNULL;
    
    AjPRegexp multire      = NULL;
    AjPRegexp speciesre    = NULL;
    AjPRegexp collectionre = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr statement = NULL;
    AjPStr dbname    = NULL;
    AjPStr group     = NULL;
    AjPStr prefix    = NULL;
    AjPStr dbversion = NULL;
    AjPStr swversion = NULL;
    AjPStr multi     = NULL;
    
    if(!dbc)
	return ajFalse;
    
    /*
    ** FIXME: Experimentally, load aliases for species names from a data file.
    ** This should probably go into an ensInit function, although it may not
    ** be neccessary to load this every time?
    */
    
    ensRegistryLoadAliases();
    
    multi = ajStrNewC("DEFAULT");
    
    multire = ajRegCompC("^ensembl_([a-z]+)_([0-9]+)");
    
    collectionre =
	ajRegCompC("^(\\w+)_(collection_core_(?:\\d+_)?(\\d+)_(\\w+))");
    
    speciesre =
	ajRegCompC("^([a-z]+_[a-z]+)_([a-z]+)_([0-9]+)_([0-9]+[a-z]*)");
    
    statement = ajStrNewC("SHOW DATABASES");
    
    sqls = ensDatabaseConnectionSqlstatementNew(dbc, statement);
    
    if(!sqls)
	ajFatal("ensRegistryLoadFromServer SQL statement failed.\n"
		"Please check the SQL server address '%S', "
		"your network connection or that any firewalls "
		"permitt outgong TCP/IP connections on port '%S'.\n",
		ensDatabaseConnectionGetHostName(dbc),
		ensDatabaseConnectionGetHostPort(dbc));
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	dbname = ajStrNew();
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToStr(sqlr, &dbname);
	
	/* Core Collection databases */
	
	if(ajRegExec(collectionre, dbname))
	{
	    prefix = ajStrNew();
	    swversion = ajStrNew();
	    dbversion = ajStrNew();
	    
	    ajRegSubI(collectionre, 1, &prefix);
	    ajRegSubI(collectionre, 2, &dbversion);
	    ajRegSubI(collectionre, 3, &swversion);
	    
	    registryLoadCollection(dbc, dbname);
	    
	    /*
	    ** TODO: Finish this and test the regular expression above whether
	    ** it really works!
	    */
	}
	
	/* Species-specific databases */
	
	if(ajRegExec(speciesre, dbname))
	{
	    prefix = ajStrNew();
	    group = ajStrNew();
	    swversion = ajStrNew();
	    dbversion = ajStrNew();
	    
	    ajRegSubI(speciesre, 1, &prefix);
	    ajRegSubI(speciesre, 2, &group);
	    ajRegSubI(speciesre, 3, &swversion);
	    ajRegSubI(speciesre, 4, &dbversion);
	    
	    if(ajStrMatchCaseC(swversion, registrySoftwareVersion))
	    {
		egroup = ensDatabaseadaptorGroupFromStr(group);
		
		if(egroup)
		    ensRegistryAddDatabaseadaptor(dbc,
						  dbname,
						  prefix,
						  egroup,
						  ajFalse,
						  0);
		else
		    ajWarn("ensRegistryLoadFromServer got unexpected group "
			   "string '%S' for database name '%S'.\n",
			   group, dbname);
	    }
	    
	    ajStrDel(&prefix);
	    ajStrDel(&group);
	    ajStrDel(&swversion);
	    ajStrDel(&dbversion);
	}
	
	/* Multi-species databases */
	
	if(ajRegExec(multire, dbname))
	{
	    group = ajStrNew();
	    swversion = ajStrNew();
	    
	    ajRegSubI(multire, 1, &group);
	    ajRegSubI(multire, 2, &swversion);
	    
	    if(ajStrMatchCaseC(swversion, registrySoftwareVersion))
	    {
		egroup = ensDatabaseadaptorGroupFromStr(group);
		
		if(egroup)
		    ensRegistryAddDatabaseadaptor(dbc,
						  dbname,
						  multi,
						  egroup,
						  ajFalse,
						  0);
		
		else
		    ajWarn("ensRegistryLoadFromServer got unexpected group "
			   "string '%S' for database name '%S'.\n",
			   group, dbname);
	    }
	    
	    ajStrDel(&group);
	    ajStrDel(&swversion);
	}
	
	ajStrDel(&dbname);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajRegFree(&collectionre);
    ajRegFree(&multire);
    ajRegFree(&speciesre);
    
    ajStrDel(&statement);
    ajStrDel(&multi);
    
    return ajTrue;
}




/* @func ensRegistryLoadIdentifiers *******************************************
**
** Read regular expressions for Ensembl Gene, Transcript, Translation and Exon
** stable identifiers, as well as Ensembl Database species and group
** information from an "EnsemblIdentifiers.dat" data file and add them to the
** Ensembl Registry.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryLoadIdentifiers(void)
{
    AjEnum egroup = ensEDatabaseadaptorGroupNULL;
    
    AjPFile infile = NULL;
    
    AjPStr line       = NULL;
    AjPStr expression = NULL;
    AjPStr alias      = NULL;
    AjPStr group      = NULL;
    AjPStr space      = NULL;
    AjPStr species    = NULL;
    
    AjPStrTok token = NULL;
    
    RegistryPIdentifier ri = NULL;
    
    infile = ajDatafileNewInNameC("EnsemblIdentifiers.dat");
    
    if(!infile)
    {
	ajWarn("ensRegistryLoadIdentifiers could no load "
	       "EnsemblIdentifiers.dat file.");
	
	return ajFalse;
    }
    
    line = ajStrNew();
    
    expression = ajStrNew();
    
    alias = ajStrNew();
    group = ajStrNew();
    space = ajStrNew();
    
    while(ajReadlineTrim(infile, &line))
    {
	if(ajStrCutComments(&line))
	{
	    ajDebug("ensRegistryLoadIdentifiers original line '%S'\n", line);
	    
	    ajStrTokenAssignC(&token, line, "\"");
	    
	    if(!ajStrTokenNextFind(&token, &expression))
		ajWarn("ensRegistryLoadIdentifiers could not parse "
		       "regular expression from line '%S'.\n", line);
	    
	    if(!ajStrTokenNextFind(&token, &space))
		ajWarn("ensRegistryLoadIdentifiers could not parse "
		       "begin of species from line '%S'.\n", line);
	    
	    if(!ajStrTokenNextFind(&token, &alias))
		ajWarn("ensRegistryLoadIdentifiers could not parse "
		       "species from line '%S'.\n", line);
	    
	    if(!ajStrTokenNextFind(&token, &space))
		ajWarn("ensRegistryLoadIdentifiers could not parse "
		       "begin of group from line '%S'.\n", line);
	    
	    if(!ajStrTokenNextFind(&token, &group))
		ajWarn("ensRegistryLoadIdentifiers could not parse "
		       "group from line '%S'.\n", line);
	    
	    ajStrTokenDel(&token);
	    
	    ajDebug("ensRegistryLoadIdentifiers "
		    "regular expression '%S' "
		    "alias '%S' "
		    "group '%S'\n",
		    expression,
		    alias,
		    group);
	    
	    species = ensRegistryGetSpecies(alias);
	    
	    if(!species)
	    {
		ajWarn("ensRegistryLoadIdentifiers could not resolve '%S' "
		       "to valid species name.\n", alias);
		
		continue;
	    }
	    
	    egroup = ensDatabaseadaptorGroupFromStr(group);
	    
	    if(!egroup)
	    {
		ajWarn("ensRegistryLoadIdentifiers could not get group for "
		       "string '%S'.\n", group);
		
		continue;
	    }
	    
	    /*
	     ** FIXME: Eventually we could directly compile the regular
	     ** expressions here, but that could take up quite some memory.
	     */
	    
	    ri = registryIdentifierNew(expression, species , egroup);
	    
	    ajListPushAppend(registryIdentifiers, (void *) ri);
	}
    }
    
    ajStrDel(&expression);
    ajStrDel(&alias);
    ajStrDel(&group);
    ajStrDel(&space);
    ajStrDel(&line);
    
    ajFileClose(&infile);
    
    return ajTrue;
}




/* @func ensRegistryGetSpeciesGroup *******************************************
**
** Evaluate Ensembl Database Adaptor species and group elements on the basis
** of an Ensembl stable identifier.
**
** @param [r] identifier [const AjPStr] Ensembl stable identifier
** @param [u] Pspecies [AjPStr*] Ensembl Database Adaptor species
** @param [w] Pgroup [AjEnum*] Ensembl Database Adaptor group
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryGetSpeciesGroup(const AjPStr identifier,
                                  AjPStr *Pspecies,
                                  AjBool *Pgroup)
{
    AjIList iter = NULL;
    
    AjPRegexp re = NULL;
    
    RegistryPIdentifier ri = NULL;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pspecies)
	return ajFalse;
    
    if(!Pgroup)
	return ajFalse;
    
    ajStrAssignClear(Pspecies);
    
    *Pgroup = ensEDatabaseadaptorGroupNULL;
    
    iter = ajListIterNew(registryIdentifiers);
    
    while(!ajListIterDone(iter))
    {
	ri = (RegistryPIdentifier) ajListIterGet(iter);
	
	re = ajRegComp(ri->RegularExpression);
	
	if(ajRegExec(re, identifier))
	{
	    ajStrAssignS(Pspecies, ri->SpeciesName);
	    
	    *Pgroup = ri->Group;
	    
	    ajRegFree(&re);
	    
	    break;
	}
	
	ajRegFree(&re);
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}
