/******************************************************************************
** @source Ensembl Registry functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.2 $
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
** @attr DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr DnaAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr AnalysisAdaptor [EnsPAnalysisAdaptor]
** @attr AssemblyExceptionAdaptor [EnsPAssemblyExceptionAdaptor]
** @attr AssemblyExceptionFeatureAdaptor [EnsPAssemblyExceptionFeatureAdaptor]
** @attr AssemblyMapperAdaptor [EnsPAssemblyMapperAdaptor]
** @attr CoordSystemAdaptor [EnsPCoordSystemAdaptor]
** @attr DatabaseEntryAdaptor [EnsPDatabaseEntryAdaptor]
** @attr DiTagFeatureAdaptor [EnsPDiTagFeatureAdaptor]
** @attr DNAAlignFeatureAdaptor [EnsPDNAAlignFeatureAdaptor]
** @attr ExonAdaptor [EnsPExonAdaptor]
** @attr ExternalDatabaseAdaptor [EnsPExternalDatabaseAdaptor]
** @attr GeneAdaptor [EnsPGeneAdaptor]
** @attr KaryotypeBandAdaptor [EnsPKaryotypeBandAdaptor]
** @attr MarkerAdaptor [EnsPMarkerAdaptor]
** @attr MarkerFeatureAdaptor [EnsPMarkerFeatureAdaptor]
** @attr MetaInformationAdaptor [EnsPMetaInformationAdaptor]
** @attr MetaCoordinateAdaptor [EnsPMetaCoordinateAdaptor]
** @attr PredictionTranscriptAdaptor [EnsPPredictionTranscriptAdaptor]
** @attr ProteinAlignFeatureAdaptor [EnsPProteinAlignFeatureAdaptor]
** @attr ProteinFeatureAdaptor [EnsPProteinFeatureAdaptor]
** @attr RepeatFeatureAdaptor [EnsPRepeatFeatureAdaptor]
** @attr SeqRegionAdaptor [EnsPSeqRegionAdaptor]
** @attr SequenceAdaptor [EnsPSequenceAdaptor]
** @attr SliceAdaptor [EnsPSliceAdaptor]
** @attr TranscriptAdaptor [EnsPTranscriptAdaptor]
** @attr TranslationAdaptor [EnsPTranslationAdaptor]
** @@
******************************************************************************/

typedef struct RegistrySCoreStyle
{
    EnsPDatabaseAdaptor DatabaseAdaptor;
    EnsPDatabaseAdaptor DnaAdaptor;
    EnsPAnalysisAdaptor AnalysisAdaptor;
    EnsPAssemblyExceptionAdaptor AssemblyExceptionAdaptor;
    EnsPAssemblyExceptionFeatureAdaptor AssemblyExceptionFeatureAdaptor;
    EnsPAssemblyMapperAdaptor AssemblyMapperAdaptor;
    EnsPCoordSystemAdaptor CoordSystemAdaptor;
    EnsPDatabaseEntryAdaptor DatabaseEntryAdaptor;
    EnsPDensityFeatureAdaptor DensityFeatureAdaptor;
    EnsPDensityTypeAdaptor DensityTypeAdaptor;
    EnsPDiTagFeatureAdaptor DiTagFeatureAdaptor;
    EnsPDNAAlignFeatureAdaptor DNAAlignFeatureAdaptor;
    EnsPExonAdaptor ExonAdaptor;
    EnsPExternalDatabaseAdaptor ExternalDatabaseAdaptor;
    EnsPGeneAdaptor GeneAdaptor;
    EnsPKaryotypeBandAdaptor KaryotypeBandAdaptor;
    EnsPMarkerAdaptor MarkerAdaptor;
    EnsPMarkerFeatureAdaptor MarkerFeatureAdaptor;
    EnsPMetaInformationAdaptor MetaInformationAdaptor;
    EnsPMetaCoordinateAdaptor MetaCoordinateAdaptor;
    EnsPMiscellaneousFeatureAdaptor MiscellaneousFeatureAdaptor;
    EnsPMiscellaneousSetAdaptor MiscellaneousSetAdaptor;
    EnsPPredictionExonAdaptor PredictionExonAdaptor;
    EnsPPredictionTranscriptAdaptor PredictionTranscriptAdaptor;
    EnsPProteinAlignFeatureAdaptor ProteinAlignFeatureAdaptor;
    EnsPProteinFeatureAdaptor ProteinFeatureAdaptor;
    EnsPRepeatFeatureAdaptor RepeatFeatureAdaptor;
    EnsPSeqRegionAdaptor SeqRegionAdaptor;
    EnsPSequenceAdaptor SequenceAdaptor;
    EnsPSimpleFeatureAdaptor SimpleFeatureAdaptor;
    EnsPSliceAdaptor SliceAdaptor;
    EnsPTranscriptAdaptor TranscriptAdaptor;
    EnsPTranslationAdaptor TranslationAdaptor;
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
** @attr DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr AlleleAdaptor [EnsPGVAlleleAdaptor]
** @attr GenotypeAdaptor [EnsPGVGenotypeAdaptor]
** @attr IndividualAdaptor [EnsPGVIndividualAdaptor]
** @attr PopulationAdaptor [EnsPGVPopulationAdaptor]
** @attr SampleAdaptor [EnsPGVSampleAdaptor]
** @attr VariationAdaptor [EnsPGVVariationAdaptor]
** @@
******************************************************************************/

typedef struct RegistrySGeneticVariation
{
    EnsPDatabaseAdaptor DatabaseAdaptor;
    EnsPGVAlleleAdaptor AlleleAdaptor;
    EnsPGVGenotypeAdaptor GenotypeAdaptor;
    EnsPGVIndividualAdaptor IndividualAdaptor;
    EnsPGVPopulationAdaptor PopulationAdaptor;
    EnsPGVSampleAdaptor SampleAdaptor;
    EnsPGVVariationAdaptor VariationAdaptor;
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
** @attr DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySFunctionalGenomics
{
    EnsPDatabaseAdaptor DatabaseAdaptor;
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
** @attr DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySComparativeGenomics
{
    EnsPDatabaseAdaptor DatabaseAdaptor;
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
** @attr DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct RegistrySGeneOntology
{
    EnsPDatabaseAdaptor DatabaseAdaptor;
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
** @attr DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr QCDatabaseAdaptor [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @attr SequenceAdaptor [EnsPQCSequenceAdaptor] Ensembl QC Sequence Adaptor
** @attr AlignmentAdaptor [EnsPQCAlignmentAdaptor] Ensembl QC Alignment Adaptor
** @attr DASFeatureAdaptor [EnsPQCDASFeatureAdaptor] Ensembl QC DAS Feature
**                                                   Adaptor
** @attr VariationAdaptor [EnsPQCVariationAdaptor] Ensembl QC Variation Adaptor
** @attr SubmissionAdaptor [EnsPQCSubmissionAdaptor] Ensembl QC Submission
**                                                   Adaptor
** @@
******************************************************************************/

typedef struct RegistrySQualityCheck
{
    EnsPDatabaseAdaptor DatabaseAdaptor;
    EnsPQCDatabaseAdaptor QCDatabaseAdaptor;
    EnsPQCSequenceAdaptor SequenceAdaptor;
    EnsPQCAlignmentAdaptor AlignmentAdaptor;
    EnsPQCDASFeatureAdaptor DASFeatureAdaptor;
    EnsPQCVariationAdaptor VariationAdaptor;
    EnsPQCSubmissionAdaptor SubmissionAdaptor;
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
** @attr Registry [void* [16]] Array of database-specific Registry Entries,
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
	    "%S  DatabaseAdaptor %p\n"
	    "%S  DnaAdaptor %p\n"
	    "%S  AnalysisAdaptor %p\n"
	    "%S  AssemblyExceptionAdaptor %p\n"
	    "%S  AssemblyExceptionFeatureAdaptor %p\n"
	    "%S  AssemblyMapperAdaptor %p\n"
	    "%S  CoordSystemAdaptor %p\n"
	    "%S  DatabaseEntryAdaptor %p\n"
	    "%S  DNAAlignFeatureAdaptor %p\n"
	    "%S  ExonAdaptor %p\n"
	    "%S  ExternalDatabaseAdaptor %p\n"
	    "%S  GeneAdaptor %p\n"
	    "%S  KaryotypeBandAdaptor %p\n"
	    "%S  MetaInformationAdaptor %p\n"
	    "%S  MetaCoordinateAdaptor %p\n"
	    "%S  MiscellaneousFeatureAdaptor %p\n"
	    "%S  MiscellaneousSetAdaptor %p\n"
	    "%S  ProteinAlignFeatureAdaptor %p\n"
	    "%S  ProteinFeatureAdaptor %p\n"
	    "%S  RepeatFeatureAdaptor %p\n"
	    "%S  SeqRegionAdaptor %p\n"
	    "%S  SequenceAdaptor %p\n"
	    "%S  SimpleFeatureAdaptor %p\n"
	    "%S  SliceAdaptor %p\n"
	    "%S  TranscriptAdaptor %p\n"
	    "%S  TranslationAdaptor %p\n",
	    indent, cs,
	    indent, cs->DatabaseAdaptor,
	    indent, cs->DnaAdaptor,
	    indent, cs->AnalysisAdaptor,
	    indent, cs->AssemblyExceptionAdaptor,
	    indent, cs->AssemblyExceptionFeatureAdaptor,
	    indent, cs->AssemblyMapperAdaptor,
	    indent, cs->CoordSystemAdaptor,
	    indent, cs->DatabaseEntryAdaptor,
	    indent, cs->DNAAlignFeatureAdaptor,
	    indent, cs->ExonAdaptor,
	    indent, cs->ExternalDatabaseAdaptor,
	    indent, cs->GeneAdaptor,
	    indent, cs->KaryotypeBandAdaptor,
	    indent, cs->MetaInformationAdaptor,
	    indent, cs->MetaCoordinateAdaptor,
	    indent, cs->MiscellaneousFeatureAdaptor,
	    indent, cs->MiscellaneousSetAdaptor,
	    indent, cs->ProteinAlignFeatureAdaptor,
	    indent, cs->ProteinFeatureAdaptor,
	    indent, cs->RepeatFeatureAdaptor,
	    indent, cs->SeqRegionAdaptor,
	    indent, cs->SequenceAdaptor,
	    indent, cs->SimpleFeatureAdaptor,
	    indent, cs->SliceAdaptor,
	    indent, cs->TranscriptAdaptor,
	    indent, cs->TranslationAdaptor);
    
    ensDatabaseAdaptorTrace(cs->DatabaseAdaptor, level + 1);
    
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
    
    ensDatabaseAdaptorDel(&pthis->DnaAdaptor);
    
    ensAnalysisAdaptorDel(&pthis->AnalysisAdaptor);
    
    ensAssemblyExceptionAdaptorDel(&pthis->AssemblyExceptionAdaptor);
    
    ensAssemblyExceptionFeatureAdaptorDel(
        &pthis->AssemblyExceptionFeatureAdaptor);
    
    ensAssemblyMapperAdaptorDel(&pthis->AssemblyMapperAdaptor);
    
    ensCoordSystemAdaptorDel(&pthis->CoordSystemAdaptor);
    
    ensDatabaseEntryAdaptorDel(&pthis->DatabaseEntryAdaptor);
    
    ensDensityFeatureAdaptorDel(&pthis->DensityFeatureAdaptor);
    
    ensDensityTypeAdaptorDel(&pthis->DensityTypeAdaptor);
    
    ensDiTagFeatureAdaptorDel(&pthis->DiTagFeatureAdaptor);
    
    ensDNAAlignFeatureAdaptorDel(&pthis->DNAAlignFeatureAdaptor);
    
    ensExonAdaptorDel(&pthis->ExonAdaptor);
    
    ensExternalDatabaseAdaptorDel(&pthis->ExternalDatabaseAdaptor);
    
    ensGeneAdaptorDel(&pthis->GeneAdaptor);
    
    ensKaryotypeBandAdaptorDel(&pthis->KaryotypeBandAdaptor);
    
    ensMarkerAdaptorDel(&pthis->MarkerAdaptor);
    
    ensMarkerFeatureAdaptorDel(&pthis->MarkerFeatureAdaptor);
    
    ensMetaInformationAdaptorDel(&pthis->MetaInformationAdaptor);
    
    ensMetaCoordinateAdaptorDel(&pthis->MetaCoordinateAdaptor);
    
    ensMiscellaneousFeatureAdaptorDel(&pthis->MiscellaneousFeatureAdaptor);
    
    ensMiscellaneousSetAdaptorDel(&pthis->MiscellaneousSetAdaptor);
    
    ensPredictionExonAdaptorDel(&pthis->PredictionExonAdaptor);
    
    ensPredictionTranscriptAdaptorDel(&pthis->PredictionTranscriptAdaptor);
    
    ensProteinAlignFeatureAdaptorDel(&pthis->ProteinAlignFeatureAdaptor);
    
    ensProteinFeatureAdaptorDel(&pthis->ProteinFeatureAdaptor);
    
    ensRepeatFeatureAdaptorDel(&pthis->RepeatFeatureAdaptor);
    
    ensSeqRegionAdaptorDel(&pthis->SeqRegionAdaptor);
    
    ensSequenceAdaptorDel(&pthis->SequenceAdaptor);
    
    ensSimpleFeatureAdaptorDel(&pthis->SimpleFeatureAdaptor);
    
    ensSliceAdaptorDel(&pthis->SliceAdaptor);
    
    ensTranscriptAdaptorDel(&pthis->TranscriptAdaptor);
    
    ensTranslationAdaptorDel(&pthis->TranslationAdaptor);
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseAdaptorDel(&pthis->DatabaseAdaptor);
    
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
    
    /* ensGVAlleleAdaptorDel(&pthis->AlleleAdaptor); */
    
    /* ensGVGenotypeAdaptorDel(&pthis->GenotypeAdaptor); */
    
    ensGVIndividualAdaptorDel(&pthis->IndividualAdaptor);
    
    ensGVPopulationAdaptorDel(&pthis->PopulationAdaptor);
    
    ensGVSampleAdaptorDel(&pthis->SampleAdaptor);
    
    /* ensGVVariationAdaptorDel(&pthis->VariationAdaptor); */
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseAdaptorDel(&pthis->DatabaseAdaptor);
    
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
    
    ensDatabaseAdaptorDel(&pthis->DatabaseAdaptor);
    
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
    
    ensDatabaseAdaptorDel(&pthis->DatabaseAdaptor);
    
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
    
    ensDatabaseAdaptorDel(&pthis->DatabaseAdaptor);
    
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
    
    ensQCDatabaseAdaptorDel(&pthis->QCDatabaseAdaptor);
    
    ensQCSequenceAdaptorDel(&pthis->SequenceAdaptor);
    
    ensQCAlignmentAdaptorDel(&pthis->AlignmentAdaptor);
    
    ensQCDASFeatureAdaptorDel(&pthis->DASFeatureAdaptor);
    
    ensQCVariationAdaptorDel(&pthis->VariationAdaptor);
    
    ensQCSubmissionAdaptorDel(&pthis->SubmissionAdaptor);
    
    /* Finally, delete the Ensembl Database Adaptor. */
    
    ensDatabaseAdaptorDel(&pthis->DatabaseAdaptor);
    
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




/* @func ensRegistryAddDatabaseAdaptor ****************************************
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

AjBool ensRegistryAddDatabaseAdaptor(EnsPDatabaseConnection dbc,
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
     ajDebug("ensRegistryAddDatabaseAdaptor\n"
	     "  dbc %p\n"
	     "  database '%S'\n"
	     "  alias '%S'\n"
	     "  group '%s'\n"
	     "  multi %B\n"
	     "  identifier %u\n",
	     dbc,
	     database,
	     alias,
	     ensDatabaseAdaptorGroupToChar(group),
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
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    if(entry->Registry[group])
	    {
		ecs = (RegistryPCoreStyle) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(ecs);
		
		entry->Registry[group] = (void *) ecs;
	    }
	    
	    if(ecs->DatabaseAdaptor)
	    {
		if(!ensDatabaseAdaptorMatchComponents(ecs->DatabaseAdaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseAdaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseAdaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		ecs->DatabaseAdaptor =
		ensDatabaseAdaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupGeneticVariation:
	    
	    if(entry->Registry[group])
	    {
		egv = (RegistryPGeneticVariation) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(egv);
		
		entry->Registry[group] = (void *) egv;
	    }
	    
	    if(egv->DatabaseAdaptor)
	    {
		if(!ensDatabaseAdaptorMatchComponents(egv->DatabaseAdaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseAdaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseAdaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		egv->DatabaseAdaptor =
		ensDatabaseAdaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupFunctionalGenomics:
	    
	    if(entry->Registry[group])
	    {
		efg = (RegistryPFunctionalGenomics) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(efg);
		
		entry->Registry[group] = (void *) efg;
	    }
	    
	    if(efg->DatabaseAdaptor)
	    {
		if(!ensDatabaseAdaptorMatchComponents(efg->DatabaseAdaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseAdaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseAdaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		efg->DatabaseAdaptor =
		ensDatabaseAdaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupComparativeGenomics:
	    
	    if(entry->Registry[group])
	    {
		ecg = (RegistryPComparativeGenomics) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(ecg);
		
		entry->Registry[group] = (void *) ecg;
	    }
	    
	    if(ecg->DatabaseAdaptor)
	    {
		if(!ensDatabaseAdaptorMatchComponents(ecg->DatabaseAdaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseAdaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseAdaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		ecg->DatabaseAdaptor =
		ensDatabaseAdaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupGeneOntology:
	    
	    if(entry->Registry[group])
	    {
		ego = (RegistryPGeneOntology) entry->Registry[group];
	    }
	    else
	    {
		AJNEW0(ego);
		
		entry->Registry[group] = (void *) ego;
	    }
	    
	    if(ego->DatabaseAdaptor)
	    {
		if(!ensDatabaseAdaptorMatchComponents(ego->DatabaseAdaptor,
							dbc,
							species,
							group,
							multi,
							identifier))
		    ajWarn("ensRegistryAddDatabaseAdaptor already has an "
			   "Ensembl Database Adaptor of the same "
			   "species '%S' and group '%s' registered.",
			   species,
			   ensDatabaseAdaptorGroupToChar(group));
		
		registered = ajFalse;
	    }
	    else
	    {
		ego->DatabaseAdaptor =
		ensDatabaseAdaptorNew(dbc,
				      database,
				      species,
				      group,
				      multi,
				      identifier);
		
		registered = ajTrue;
	    }
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupQualityCheck:
	    
	case ensEDatabaseAdaptorGroupPipeline:
	    
	case ensEDatabaseAdaptorGroupHive:
	    
	case ensEDatabaseAdaptorGroupCoreExpressionEST:
	    
	case ensEDatabaseAdaptorGroupCoreExpressionGNF:
	    
	case ensEDatabaseAdaptorGroupAncestral:
	    
	case ensEDatabaseAdaptorGroupWebsite:
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryAddDatabaseAdaptor got a request for an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n", group);
	    
	    registered = ajFalse;
    }
    
    return registered;
}




/* @func ensRegistryRemoveDatabaseAdaptor *************************************
**
** Remove an Ensembl Database Adaptor from the Ensembl Registry and delete it.
**
** @param [d] Pdba [EnsPDatabaseAdaptor*] Ensembl Database Adaptor address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRegistryRemoveDatabaseAdaptor(EnsPDatabaseAdaptor *Pdba)
{
    AjEnum group = ensEDatabaseAdaptorGroupNULL;
    
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
		     (const void *) ensDatabaseAdaptorGetSpecies(*Pdba));
    
    if(!entry)
    {
	ajWarn("ensRegistryRemoveDatabaseAdaptor could not get "
	       "a Registry Entry for species '%S'.\n",
	       ensDatabaseAdaptorGetSpecies(*Pdba));
	
	*Pdba = (EnsPDatabaseAdaptor) NULL;
	
	return ajTrue;
    }
    
    group = ensDatabaseAdaptorGetGroup(*Pdba);
    
    switch(group)
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle) entry->Registry[group];
	    
	    if(ecs)
	    {
		if(ecs->DatabaseAdaptor == *Pdba)
		    registryCoreStyleDel((RegistryPCoreStyle*)
					 &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseAdaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation) entry->Registry[group];
	    
	    if(egv)
	    {
		if(egv->DatabaseAdaptor == *Pdba)
		    registryGeneticVariationDel((RegistryPGeneticVariation *)
						&(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseAdaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupFunctionalGenomics:
	    
	    efg = (RegistryPFunctionalGenomics) entry->Registry[group];
	    
	    if(efg)
	    {
		if(efg->DatabaseAdaptor == *Pdba)
		    registryFunctionalGenomicsDel(
                        (RegistryPFunctionalGenomics *)
                        &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseAdaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupComparativeGenomics:
	    
	    ecg = (RegistryPComparativeGenomics) entry->Registry[group];
	    
	    if(ecg)
	    {
		if(ecg->DatabaseAdaptor == *Pdba)
		    registryComparativeGenomicsDel(
                        (RegistryPComparativeGenomics *)
                        &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseAdaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	case ensEDatabaseAdaptorGroupGeneOntology:
	    
	    ego = (RegistryPGeneOntology) entry->Registry[group];
	    
	    if(ego)
	    {
		if(ego->DatabaseAdaptor == *Pdba)
		    registryGeneOntologyDel((RegistryPGeneOntology *)
					    &(entry->Registry[group]));
	    }
            else
                ajWarn("ensRegistryRemoveDatabaseAdaptor got "
                       "an Ensembl Database Adaptor, which is not "
                       "registered?\n");
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryRemoveDatabaseAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n", group);
    }
    
    /*
    ** TODO: Test if this entry remains completely empty.
    ** If this was the case it could be removed.
    ** Should we also automatically delete all DatabaseAdaptors that get the
    ** DNA via the Core adaptor in case the Core adaptor gets deleted?
    */
    
    /* Clear the Ensembl Database Adaptor pointer. */
    
    *Pdba = (EnsPDatabaseAdaptor) NULL;
    
    return ajTrue;
}




/* @func ensRegistryGetAllDatabaseAdaptors ************************************
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

AjBool ensRegistryGetAllDatabaseAdaptors(AjEnum group,
                                         const AjPStr alias,
                                         AjPList list)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    register ajint j = 0;
    
    AjPStr species = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
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
		case ensEDatabaseAdaptorGroupCore:
		    
		case ensEDatabaseAdaptorGroupVega:
		    
		case ensEDatabaseAdaptorGroupOtherFeatures:
		    
		case ensEDatabaseAdaptorGroupCopyDNA:
		    
		    ecs = (RegistryPCoreStyle)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(ecs && ecs->DatabaseAdaptor)
		    {
			dba = ecs->DatabaseAdaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseAdaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseAdaptorGroupGeneticVariation:
		    
		    egv = (RegistryPGeneticVariation)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(egv && egv->DatabaseAdaptor)
		    {
			dba = egv->DatabaseAdaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseAdaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseAdaptorGroupFunctionalGenomics:
		    
		    efg = (RegistryPFunctionalGenomics)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(efg && efg->DatabaseAdaptor)
		    {
			dba = efg->DatabaseAdaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseAdaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseAdaptorGroupComparativeGenomics:
		    
		    ecg = (RegistryPComparativeGenomics)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(ecg && ecg->DatabaseAdaptor)
		    {
			dba = ecg->DatabaseAdaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseAdaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		case ensEDatabaseAdaptorGroupGeneOntology:
		    
		    ego = (RegistryPGeneOntology)
		    (((RegistryPEntry) valarray[i])->Registry[j]);
		    
		    if(ego && ego->DatabaseAdaptor)
		    {
			dba = ego->DatabaseAdaptor;
			
			if(alias)
			{
			    if(ajStrMatchS(species,
					    ensDatabaseAdaptorGetSpecies(dba)))
				ajListPushAppend(list, (void *) dba);
			}
			else
			    ajListPushAppend(list, (void *) dba);
		    }
			
			break;
		    
		default:
		    
		    ajWarn("ensRegistryGetAllDatabaseAdaptors got an"
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
** @fdata [EnsPDatabaseAdaptor]
** @fnote None
**
** @nam3rule Get Return Registry attribute(s)
** @nam4rule GetAnalysisAdaptor Return the Ensembl Analysis Adaptor
** @nam4rule GetAssemblyExceptionAdaptor Return the Ensembl Assembly
**                                       Exception Adaptor
** @nam4rule GetAssemblyExceptionFeatureAdaptor Return the Ensembl Assembly
**                                              Exception Feature Adaptor
** @nam4rule GetAssemblyMapperAdaptor Return the Ensembl Assembly
**                                    Mapper Adaptor
** @nam4rule GetCoordSystemAdaptor Return the Ensembl Coordinate System Adaptor
** @nam4rule GetDatabaseEntryAdaptor Return the Ensembl Database Entry Adaptor
** @nam4rule GetDNAAlignFeatureAdaptor Return the Ensembl DNA Align Feature
**                                     Adaptor
** @nam4rule GetExonAdaptor Return the Ensembl Exon Adaptor
** @nam4rule GetExternalDatabaseAdaptor Return the Ensembl External Database
**                                      Adaptor
** @nam4rule GetGeneAdaptor Return the Ensembl Gene Adaptor
** @nam4rule GetKaryotypeBandAdaptor Return the Ensembl Karyotype Band Adaptor
** @nam4rule GetMetaInformationAdaptor Return the Ensembl Meta-Information
**                                     Adaptor
** @nam4rule GetMetaCoordinateAdaptor Return the Ensembl Meta-Coordinate
**                                    Adaptor
**
** @argrule * dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @valrule AnalysisAdaptor [EnsPAnalysisAdaptor]
** @valrule AssemblyException [EnsPAssemblyException]
** @valrule AssemblyExceptionFeatureAdaptor [EnsPAssemblyExceptionFeatureAdaptor]
** @valrule AssemblyMapperAdaptor [EnsPAssemblyMapperAdaptor]
** @valrule CoordSystemAdaptor [EnsPCoordSystemAdaptor]
** @valrule DatabaseEntryAdaptor [EnsPDatabaseEntryAdaptor]
** @valrule DNAAlignFeatureAdaptor [EnsPDNAAlignFeatureAdaptor]
** @valrule ExonAdaptor [EnsPExonAdaptor]
** @valrule ExternalDatabaseAdaptor [EnsPExternalDatabaseAdaptor]
** @valrule GeneAdaptor [EnsPGeneAdaptor]
** @valrule KaryotypeBandAdaptor [EnsPKaryotypeBandAdaptor]
** @valrule MetaInformationAdaptor [EnsPMetaInformationAdaptor]
**
** @fcategory use
******************************************************************************/




/* @func ensRegistryGetDatabaseAdaptor ****************************************
**
** Get an Ensembl Database Adaptor from the Ensembl Registry.
**
** @param [r] group [AjEnum] Ensembl Database Adaptor group
** @param [r] alias [const AjPStr] Scientific species name or alias name
**
** @return [EnsPDatabaseAdaptor] Ensembl Database Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseAdaptor ensRegistryGetDatabaseAdaptor(AjEnum group,
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
     ajDebug("ensRegistryGetDatabaseAdaptor alias '%S' -> species '%S'\n",
	     alias, species);
     */
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries, (const void *) species);
    
    if(!entry)
    {
	ajDebug("ensRegistryGetDatabaseAdaptor could not get a "
		"Registry Entry for group '%s' and species '%S'.\n",
		ensDatabaseAdaptorGroupToChar(group), species);
	
	return NULL;
    }
    
    switch(group)
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle) entry->Registry[group];
	    
	    if(ecs)
		return ecs->DatabaseAdaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseAdaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseAdaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseAdaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation) entry->Registry[group];
	    
	    if(egv)
		return egv->DatabaseAdaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseAdaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseAdaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseAdaptorGroupFunctionalGenomics:
	    
	    efg = (RegistryPFunctionalGenomics) entry->Registry[group];
	    
	    if(efg)
		return efg->DatabaseAdaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseAdaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseAdaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseAdaptorGroupComparativeGenomics:
	    
	    ecg = (RegistryPComparativeGenomics) entry->Registry[group];
	    
	    if(ecg)
		return ecg->DatabaseAdaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseAdaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseAdaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	case ensEDatabaseAdaptorGroupGeneOntology:
	    
	    ego = (RegistryPGeneOntology) entry->Registry[group];
	    
	    if(ego)
		return ego->DatabaseAdaptor;
	    else
	    {
		ajDebug("ensRegistryGetDatabaseAdaptor could not get an "
			"Ensembl Database Adaptor for group '%s' and "
			"species '%S'.\n",
			ensDatabaseAdaptorGroupToChar(group), species);
		
		return NULL;
	    }
		
		break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDatabaseAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n", group);
    }
    
    return NULL;
}




/* @func ensRegistryGetDnaAdaptor *********************************************
**
** Get an Ensembl Dna Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPDatabaseAdaptor] Ensembl Database Adaptor or NULL
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

EnsPDatabaseAdaptor ensRegistryGetDnaAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry = NULL;
    
    if(!dba)
	return NULL;
    
    entry = registryGetEntryByDatabaseAdaptor(dba);
    
    if(entry)
	return entry->DnaAdaptor;

    return NULL;
}

#endif




/* @func ensRegistryGetAnalysisAdaptor ****************************************
**
** Get an Ensembl Analysis Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPAnalysisAdaptor] Ensembl Analysis Adaptor or NULL
** @@
******************************************************************************/

EnsPAnalysisAdaptor ensRegistryGetAnalysisAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->AnalysisAdaptor)
		ecs->AnalysisAdaptor = ensAnalysisAdaptorNew(dba);
		
		return ecs->AnalysisAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAnalysisAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetAssemblyExceptionAdaptor *******************************
**
** Get an Ensembl Assembly Exception Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyExceptionAdaptor] Ensembl Assembly Exception Adaptor
**                                        or NULL
** @@
******************************************************************************/

EnsPAssemblyExceptionAdaptor ensRegistryGetAssemblyExceptionAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->AssemblyExceptionAdaptor)
		ecs->AssemblyExceptionAdaptor =
		    ensAssemblyExceptionAdaptorNew(dba);
		
		return ecs->AssemblyExceptionAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAssemblyExceptionAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetAssemblyExceptionFeatureAdaptor ************************
**
** Get an Ensembl Assembly Exception Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyExceptionFeatureAdaptor] Ensembl Assembly Exception
**                                               Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyExceptionFeatureAdaptor ensRegistryGetAssemblyExceptionFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->AssemblyExceptionFeatureAdaptor)
		ecs->AssemblyExceptionFeatureAdaptor =
		    ensAssemblyExceptionFeatureAdaptorNew(dba);
		
		return ecs->AssemblyExceptionFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAssemblyExceptionFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetAssemblyMapperAdaptor **********************************
**
** Get an Ensembl Assembly Mapper Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyMapperAdaptor ensRegistryGetAssemblyMapperAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->AssemblyMapperAdaptor)
		ecs->AssemblyMapperAdaptor = ensAssemblyMapperAdaptorNew(dba);
		
		return ecs->AssemblyMapperAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetAssemblyMapperAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetCoordSystemAdaptor *************************************
**
** Get an Ensembl Coordinate System Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPCoordSystemAdaptor] Ensembl Coordinate System Adaptor or NULL
** @@
******************************************************************************/

EnsPCoordSystemAdaptor ensRegistryGetCoordSystemAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->CoordSystemAdaptor)
		ecs->CoordSystemAdaptor =
		    ensCoordSystemAdaptorNew(dba);
		
		return ecs->CoordSystemAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetCoordSystemAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDatabaseEntryAdaptor ***********************************
**
** Get an Ensembl Database Entry Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPDatabaseEntryAdaptor] Ensembl Database Entry Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseEntryAdaptor ensRegistryGetDatabaseEntryAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->DatabaseEntryAdaptor)
		ecs->DatabaseEntryAdaptor = ensDatabaseEntryAdaptorNew(dba);
		
		return ecs->DatabaseEntryAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDatabaseEntryAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDensityFeatureAdaptor **********************************
**
** Get an Ensembl Density Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPDensityFeatureAdaptor] Ensembl Density Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDensityFeatureAdaptor ensRegistryGetDensityFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->DensityFeatureAdaptor)
		ecs->DensityFeatureAdaptor = ensDensityFeatureAdaptorNew(dba);
		
		return ecs->DensityFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDensityFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDensityTypeAdaptor *************************************
**
** Get an Ensembl Density Type Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPDensityTypeAdaptor] Ensembl Density Type Adaptor or NULL
** @@
******************************************************************************/

EnsPDensityTypeAdaptor ensRegistryGetDensityTypeAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->DensityTypeAdaptor)
		ecs->DensityTypeAdaptor = ensDensityTypeAdaptorNew(dba);
		
		return ecs->DensityTypeAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDensityTypeAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDiTagFeatureAdaptor ************************************
**
** Get an Ensembl DiTag Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPDiTagFeatureAdaptor] Ensembl DiTag Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDiTagFeatureAdaptor ensRegistryGetDiTagFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->DiTagFeatureAdaptor)
		ecs->DiTagFeatureAdaptor = ensDiTagFeatureAdaptorNew(dba);
		
		return ecs->DiTagFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDiTagFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetDNAAlignFeatureAdaptor *********************************
**
** Get an Ensembl DNA Align Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPDNAAlignFeatureAdaptor] Ensembl DNA Align Feature Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPDNAAlignFeatureAdaptor ensRegistryGetDNAAlignFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->DNAAlignFeatureAdaptor)
		ecs->DNAAlignFeatureAdaptor =
		    ensDNAAlignFeatureAdaptorNew(dba);
		
		return ecs->DNAAlignFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetDNAAlignFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetExonAdaptor ********************************************
**
** Get an Ensembl Exon Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPExonAdaptor] Ensembl Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPExonAdaptor ensRegistryGetExonAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->ExonAdaptor)
		ecs->ExonAdaptor = ensExonAdaptorNew(dba);
		
		return ecs->ExonAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetExonAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetExternalDatabaseAdaptor ********************************
**
** Get an Ensembl External Database Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPExternalDatabaseAdaptor] Ensembl External Database Adaptor or
**                                       NULL
** @@
******************************************************************************/

EnsPExternalDatabaseAdaptor ensRegistryGetExternalDatabaseAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->ExternalDatabaseAdaptor)
		ecs->ExternalDatabaseAdaptor =
		    ensExternalDatabaseAdaptorNew(dba);
		
		return ecs->ExternalDatabaseAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetExternalDatabaseAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGeneAdaptor ********************************************
**
** Get an Ensembl Gene Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGeneAdaptor] Ensembl Gene Adaptor or NULL
** @@
******************************************************************************/

EnsPGeneAdaptor ensRegistryGetGeneAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->GeneAdaptor)
		ecs->GeneAdaptor = ensGeneAdaptorNew(dba);
		
		return ecs->GeneAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGeneAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetKaryotypeBandAdaptor ***********************************
**
** Get an Ensembl Karyotype Band Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPKaryotypeBandAdaptor] Ensembl Karyotype Band Adaptor or NULL
** @@
******************************************************************************/

EnsPKaryotypeBandAdaptor ensRegistryGetKaryotypeBandAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->KaryotypeBandAdaptor)
		ecs->KaryotypeBandAdaptor = ensKaryotypeBandAdaptorNew(dba);
		
		return ecs->KaryotypeBandAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetKaryotypeBandAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMarkerAdaptor ******************************************
**
** Get an Ensembl Marker Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkerAdaptor] Ensembl Marker Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkerAdaptor ensRegistryGetMarkerAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->MarkerAdaptor)
		ecs->MarkerAdaptor =
		    ensMarkerAdaptorNew(dba);
		
		return ecs->MarkerAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMarkerAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMarkerFeatureAdaptor ***********************************
**
** Get an Ensembl Marker Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkerFeatureAdaptor ensRegistryGetMarkerFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->MarkerFeatureAdaptor)
		ecs->MarkerFeatureAdaptor =
		    ensMarkerFeatureAdaptorNew(dba);
		
		return ecs->MarkerFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMarkerFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMetaInformationAdaptor *********************************
**
** Get an Ensembl Meta-Information Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMetaInformationAdaptor] Ensembl Meta-Information Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPMetaInformationAdaptor ensRegistryGetMetaInformationAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->MetaInformationAdaptor)
		ecs->MetaInformationAdaptor =
		    ensMetaInformationAdaptorNew(dba);
		
		return ecs->MetaInformationAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMetaInformationAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMetaCoordinateAdaptor **********************************
**
** Get an Ensembl Meta-Coordinate Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate Adaptor or NULL
** @@
******************************************************************************/

EnsPMetaCoordinateAdaptor ensRegistryGetMetaCoordinateAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->MetaCoordinateAdaptor)
		ecs->MetaCoordinateAdaptor = ensMetaCoordinateAdaptorNew(dba);
		
		return ecs->MetaCoordinateAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMetaCoordinateAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMiscellaneousFeatureAdaptor ****************************
**
** Get an Ensembl Miscellaneous Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMiscellaneousFeatureAdaptor] Ensembl Miscellaneous
**                                           Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPMiscellaneousFeatureAdaptor ensRegistryGetMiscellaneousFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->MiscellaneousFeatureAdaptor)
		ecs->MiscellaneousFeatureAdaptor =
		    ensMiscellaneousFeatureAdaptorNew(dba);
		
		return ecs->MiscellaneousFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMiscellaneousFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetMiscellaneousSetAdaptor ********************************
**
** Get an Ensembl Miscellaneous Set Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMiscellaneousSetAdaptor] Ensembl Miscellaneous Set Adaptor
**                                       or NULL
** @@
******************************************************************************/

EnsPMiscellaneousSetAdaptor ensRegistryGetMiscellaneousSetAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->MiscellaneousSetAdaptor)
		ecs->MiscellaneousSetAdaptor =
		    ensMiscellaneousSetAdaptorNew(dba);
		
		return ecs->MiscellaneousSetAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetMiscellaneousSetAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetPredictionExonAdaptor **********************************
**
** Get an Ensembl Prediction Exon Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictionExonAdaptor] Ensembl Prediction Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionExonAdaptor ensRegistryGetPredictionExonAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->PredictionExonAdaptor)
		ecs->PredictionExonAdaptor =
		    ensPredictionExonAdaptorNew(dba);
		
		return ecs->PredictionExonAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetPredictionExonAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetPredictionTranscriptAdaptor ****************************
**
** Get an Ensembl Prediction Transcript Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictionTranscriptAdaptor] Ensembl Prediction
**                                           Transcript Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionTranscriptAdaptor ensRegistryGetPredictionTranscriptAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->PredictionTranscriptAdaptor)
		ecs->PredictionTranscriptAdaptor =
		    ensPredictionTranscriptAdaptorNew(dba);
		
		return ecs->PredictionTranscriptAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetPredictionTranscriptAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetProteinAlignFeatureAdaptor *****************************
**
** Get an Ensembl Protein Align Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPProteinAlignFeatureAdaptor] Ensembl Protein Align Feature
**                                          Adaptor or NULL
** @@
******************************************************************************/

EnsPProteinAlignFeatureAdaptor ensRegistryGetProteinAlignFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->ProteinAlignFeatureAdaptor)
		ecs->ProteinAlignFeatureAdaptor =
		    ensProteinAlignFeatureAdaptorNew(dba);
		
		return ecs->ProteinAlignFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetProteinAlignFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetProteinFeatureAdaptor **********************************
**
** Get an Ensembl Protein Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPProteinFeatureAdaptor] Ensembl Protein Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPProteinFeatureAdaptor ensRegistryGetProteinFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->ProteinFeatureAdaptor)
		ecs->ProteinFeatureAdaptor = ensProteinFeatureAdaptorNew(dba);
		
		return ecs->ProteinFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetProteinFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetRepeatFeatureAdaptor ***********************************
**
** Get an Ensembl Repeat Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPRepeatFeatureAdaptor] Ensembl Repeat Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPRepeatFeatureAdaptor ensRegistryGetRepeatFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->RepeatFeatureAdaptor)
		ecs->RepeatFeatureAdaptor = ensRepeatFeatureAdaptorNew(dba);
		
		return ecs->RepeatFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetRepeatFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSeqRegionAdaptor ***************************************
**
** Get an Ensembl Sequence Region Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPSeqRegionAdaptor] Ensembl Sequence Region Adaptor or NULL
** @@
******************************************************************************/

EnsPSeqRegionAdaptor ensRegistryGetSeqRegionAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->SeqRegionAdaptor)
		ecs->SeqRegionAdaptor = ensSeqRegionAdaptorNew(dba);
		
		return ecs->SeqRegionAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSeqRegionAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSimpleFeatureAdaptor ***********************************
**
** Get an Ensembl Simple Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPSimpleFeatureAdaptor] Ensembl Simple Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPSimpleFeatureAdaptor ensRegistryGetSimpleFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->SimpleFeatureAdaptor)
		ecs->SimpleFeatureAdaptor = ensSimpleFeatureAdaptorNew(dba);
		
		return ecs->SimpleFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSimpleFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSequenceAdaptor ****************************************
**
** Get an Ensembl Sequence Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPSequenceAdaptor] Ensembl Sequence Adaptor or NULL
** @@
******************************************************************************/

EnsPSequenceAdaptor ensRegistryGetSequenceAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->SequenceAdaptor)
		ecs->SequenceAdaptor = ensSequenceAdaptorNew(dba);
		
		return ecs->SequenceAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSequenceAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetSliceAdaptor *******************************************
**
** Get an Ensembl Slice Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPSliceAdaptor] Ensembl Slice Adaptor or NULL
** @@
******************************************************************************/

EnsPSliceAdaptor ensRegistryGetSliceAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->SliceAdaptor)
		ecs->SliceAdaptor = ensSliceAdaptorNew(dba);
		
		return ecs->SliceAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetSliceAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetTranscriptAdaptor **************************************
**
** Get an Ensembl Transcript Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPTranscriptAdaptor] Ensembl Transcript Adaptor or NULL
** @@
******************************************************************************/

EnsPTranscriptAdaptor ensRegistryGetTranscriptAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->TranscriptAdaptor)
		ecs->TranscriptAdaptor = ensTranscriptAdaptorNew(dba);
		
		return ecs->TranscriptAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetTranscriptAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetTranslationAdaptor *************************************
**
** Get an Ensembl Translation Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPTranslationAdaptor] Ensembl Translation Adaptor or NULL
** @@
******************************************************************************/

EnsPTranslationAdaptor ensRegistryGetTranslationAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry   = NULL;
    RegistryPCoreStyle ecs = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupCore:
	    
	case ensEDatabaseAdaptorGroupVega:
	    
	case ensEDatabaseAdaptorGroupOtherFeatures:
	    
	case ensEDatabaseAdaptorGroupCopyDNA:
	    
	    ecs = (RegistryPCoreStyle)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!ecs->TranslationAdaptor)
		ecs->TranslationAdaptor = ensTranslationAdaptorNew(dba);
		
		return ecs->TranslationAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetTranslationAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCDatabaseAdaptor **************************************
**
** Get an Ensembl QC Database Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor or NULL
** @@
******************************************************************************/

EnsPQCDatabaseAdaptor ensRegistryGetQCDatabaseAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!eqc->DatabaseAdaptor)
		eqc->QCDatabaseAdaptor =
		    ensQCDatabaseAdaptorNew(dba);
		
		return eqc->QCDatabaseAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCDatabaseAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCSequenceAdaptor **************************************
**
** Get an Ensembl QC Sequence Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPQCSequenceAdaptor] Ensembl QC Sequence Adaptor or NULL
** @@
******************************************************************************/

EnsPQCSequenceAdaptor ensRegistryGetQCSequenceAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!eqc->SequenceAdaptor)
		eqc->SequenceAdaptor =
		    ensQCSequenceAdaptorNew(dba);
		
		return eqc->SequenceAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCSequenceAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCAlignmentAdaptor *************************************
**
** Get an Ensembl QC Alignment Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPQCAlignmentAdaptor] Ensembl QC Alignment Adaptor or NULL
** @@
******************************************************************************/

EnsPQCAlignmentAdaptor ensRegistryGetQCAlignmentAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!eqc->AlignmentAdaptor)
		eqc->AlignmentAdaptor =
		    ensQCAlignmentAdaptorNew(dba);
		
		return eqc->AlignmentAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCAlignmentAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCDASFeatureAdaptor ************************************
**
** Get an Ensembl QC DAS Feature Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPQCDASFeatureAdaptor] Ensembl QC DAS Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPQCDASFeatureAdaptor ensRegistryGetQCDASFeatureAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!eqc->DASFeatureAdaptor)
		eqc->DASFeatureAdaptor =
		    ensQCDASFeatureAdaptorNew(dba);
		
		return eqc->DASFeatureAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCDASFeatureAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCVariationAdaptor *************************************
**
** Get an Ensembl QC Variation Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPQCVariationAdaptor] Ensembl QC Variation Adaptor or NULL
** @@
******************************************************************************/

EnsPQCVariationAdaptor ensRegistryGetQCVariationAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!eqc->VariationAdaptor)
		eqc->VariationAdaptor =
		    ensQCVariationAdaptorNew(dba);
		
		return eqc->VariationAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCVariationAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetQCSubmissionAdaptor ************************************
**
** Get an Ensembl QC Submission Adaptor from the Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPQCSubmissionAdaptor] Ensembl QC Submission Adaptor or NULL
** @@
******************************************************************************/

EnsPQCSubmissionAdaptor ensRegistryGetQCSubmissionAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry      = NULL;
    RegistryPQualityCheck eqc = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupQualityCheck:
	    
	    eqc = (RegistryPQualityCheck)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!eqc->SubmissionAdaptor)
		eqc->SubmissionAdaptor =
		    ensQCSubmissionAdaptorNew(dba);
		
		return eqc->SubmissionAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetQCSubmissionAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGVIndividualAdaptor ************************************
**
** Get an Ensembl Genetic Variation Individual Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                   Individual Adaptor or NULL
** @@
******************************************************************************/

EnsPGVIndividualAdaptor ensRegistryGetGVIndividualAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation egv = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!egv->IndividualAdaptor)
		egv->IndividualAdaptor =
		    ensGVIndividualAdaptorNew(dba);
		
		return egv->IndividualAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGVIndividualAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGVPopulationAdaptor ************************************
**
** Get an Ensembl Genetic Variation Population Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                   Population Adaptor or NULL
** @@
******************************************************************************/

EnsPGVPopulationAdaptor ensRegistryGetGVPopulationAdaptor(
    EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation egv = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!egv->PopulationAdaptor)
		egv->PopulationAdaptor =
		    ensGVPopulationAdaptorNew(dba);
		
		return egv->PopulationAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGVPopulationAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
    }
    
    return NULL;
}




/* @func ensRegistryGetGVVariationAdaptor *************************************
**
** Get an Ensembl Genetic Variation Variation Adaptor from the
** Ensembl Registry.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                  Variation Adaptor or NULL
** @@
******************************************************************************/

EnsPGVVariationAdaptor ensRegistryGetGVVariationAdaptor(EnsPDatabaseAdaptor dba)
{
    RegistryPEntry entry          = NULL;
    RegistryPGeneticVariation egv = NULL;
    
    if(!dba)
	return NULL;
    
    entry = (RegistryPEntry)
	ajTableFetch(registryEntries,
		     (const void *) ensDatabaseAdaptorGetSpecies(dba));
    
    if(!entry)
	return NULL;
    
    switch(ensDatabaseAdaptorGetGroup(dba))
    {
	case ensEDatabaseAdaptorGroupGeneticVariation:
	    
	    egv = (RegistryPGeneticVariation)
	    entry->Registry[ensDatabaseAdaptorGetGroup(dba)];
	    
	    if(!egv->VariationAdaptor)
		egv->VariationAdaptor =
		    ensGVVariationAdaptorNew(dba);
		
		return egv->VariationAdaptor;
	    
	    break;
	    
	default:
	    
	    ajWarn("ensRegistryGetGVVariationAdaptor got an "
		   "Ensembl Database Adaptor "
		   "with an unexpected group %d.\n",
		   ensDatabaseAdaptorGetGroup(dba));
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
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr alias     = NULL;
    AjPStr species   = NULL;
    AjPStr statement = NULL;
    
    if(!dbc)
	return ajFalse;
    
    ensDatabaseConnectionEscapeCS(dbc, &txtdbname, dbname);
    
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
    
    sqls = ensDatabaseConnectionSqlStatementNew(dbc, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier = 0;
	alias = ajStrNew();
	species = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToUint(sqlr, &identifier);
	ajSqlColumnToStr(sqlr, &alias);
	
	ajStrAssignS(&species, alias);
	
	ajStrExchangeCC(&species, "_", " ");
	
	ensRegistryAddAlias(species, alias);
	
	ensRegistryAddDatabaseAdaptor(dbc,
				      dbname,
				      species,
				      ensEDatabaseAdaptorGroupCore,
				      ajTrue,
				      identifier);
	
	ajStrDel(&alias);
	
	ajStrDel(&species);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
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
    AjEnum egroup = ensEDatabaseAdaptorGroupNULL;
    
    AjPRegexp multire      = NULL;
    AjPRegexp speciesre    = NULL;
    AjPRegexp collectionre = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
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
    
    sqls = ensDatabaseConnectionSqlStatementNew(dbc, statement);
    
    if(!sqls)
	ajFatal("ensRegistryLoadFromServer SQL statement failed.\n"
		"Please check the SQL server address '%S', "
		"your network connection or that any firewalls "
		"permitt outgong TCP/IP connections on port '%S'.\n",
		ensDatabaseConnectionGetHostName(dbc),
		ensDatabaseConnectionGetHostPort(dbc));
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	dbname = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToStr(sqlr, &dbname);
	
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
		egroup = ensDatabaseAdaptorGroupFromStr(group);
		
		if(egroup)
		    ensRegistryAddDatabaseAdaptor(dbc,
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
		egroup = ensDatabaseAdaptorGroupFromStr(group);
		
		if(egroup)
		    ensRegistryAddDatabaseAdaptor(dbc,
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
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
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
    AjEnum egroup = ensEDatabaseAdaptorGroupNULL;
    
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
	    
	    egroup = ensDatabaseAdaptorGroupFromStr(group);
	    
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
    
    *Pgroup = ensEDatabaseAdaptorGroupNULL;
    
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
