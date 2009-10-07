/******************************************************************************
** @source Ensembl Quality Check functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.2 $
** @@
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
*****************************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensqc.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *qcDASFeatureCategory[] =
{
    (const char *) NULL,
    "unknown",
    "transcript:perfect",
    "transcript:tolerance",
    "transcript:partial'",
    "transcript:missing",
    "transcript",
    "translation:perfect",
    "translation:tolerance",
    "translation:partial",
    "translation:missing",
    "translation",
    (const char *) NULL
};




static const char *qcDASFeatureType[] =
{
    (const char *) NULL,
    "unknown",
    "exon:perfect",
    "exon:partial",
    "exon:missing",
    "exon:frameshift",
    "exon:gap",
    "exon",
    (const char *) NULL
};




static const char *qcVariationClass[] =
{
    (const char *) NULL,
    "none",
    "simple",
    "splice",
    "exon",
    (const char *) NULL
};




static const char *qcVariationType[] =
{
    (const char *) NULL,
    "none",
    "single",
    "multi",
    (const char *) NULL
};




static const char *qcVariationState[] =
{
    (const char *) NULL,
    "none",
    "match",
    "gap",
    "frameshift",
    "5'ss",
    "3'ss",
    "split",
    (const char *) NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisadaptor
ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba);

extern EnsPQCAlignmentadaptor
ensRegistryGetQCAlignmentadaptor(EnsPDatabaseadaptor dba);

extern EnsPQCDASFeatureadaptor
ensRegistryGetQCDASFeatureadaptor(EnsPDatabaseadaptor dba);

extern EnsPQCSequenceadaptor
ensRegistryGetQCSequenceadaptor(EnsPDatabaseadaptor dba);

extern EnsPQCSubmissionadaptor
ensRegistryGetQCSubmissionadaptor(EnsPDatabaseadaptor dba);

extern EnsPQCVariationadaptor
ensRegistryGetQCVariationadaptor(EnsPDatabaseadaptor dba);

static AjBool qcDASFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList qcdasfs);

static AjBool qcVariationadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblyMapper am,
                                              EnsPSlice slice,
                                              AjPList qcvs);

static AjBool qcSubmissionadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList qcss);




/* @filesection ensqc *********************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPQCDASFeature] QC DAS Feature *****************************
**
** Functions for manipulating Ensembl QC DAS Feature objects
**
** Bio::EnsEMBL::QC::DASFeature CVS Revision:
**
** @nam2rule QCDASFeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC DAS Feature by pointer.
** It is the responsibility of the user to first destroy any previous
** QC DAS Feature. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCDASFeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPQCDASFeature] Ensembl QC DAS Feature
** @argrule Ref object [EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @valrule * [EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @fcategory new
******************************************************************************/




/* @func ensQCDASFeatureNew ***************************************************
**
** Default constructor for an Ensembl QC DAS Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::QC::DASFeature::new
** @param [u] qca [EnsPQCAlignment] Ensembl QC Alignment
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [u] ssequence [EnsPQCSequence] Segment Ensembl QC Sequence
** @param [r] sstart [ajuint] Segment start
** @param [r] send [ajuint] Segment end
** @param [r] sstrand [ajint] Segment strand
** @param [u] fsequence [EnsPQCSequence] Feature Ensembl QC Sequence
** @param [r] tstart [ajuint] Feature start
** @param [r] tend [ajuint] Feature end
** @param [r] phase [ajint] Phase
** @param [r] category [AjEnum] Category
** @param [r] type [AjEnum] Type
**
** @return [EnsPQCDASFeature] Ensembl QC DAS Feature or NULL
** @@
******************************************************************************/

EnsPQCDASFeature ensQCDASFeatureNew(EnsPQCDASFeatureadaptor adaptor,
                                    ajuint identifier,
                                    EnsPQCAlignment qca,
                                    EnsPAnalysis analysis,
                                    EnsPQCSequence segment,
                                    ajuint sstart,
                                    ajuint send,
                                    ajint sstrand,
                                    EnsPQCSequence feature,
                                    ajuint fstart,
                                    ajuint fend,
                                    ajint phase,
                                    AjEnum category,
                                    AjEnum type)
{
    EnsPQCDASFeature qcdasf = NULL;
    
    if(!qca)
	return NULL;
    
    if(!analysis)
	return NULL;
    
    if(!segment)
	return NULL;
    
    if(!feature)
	return NULL;
    
    AJNEW0(qcdasf);
    
    qcdasf->Use = 1;
    
    qcdasf->Identifier = identifier;
    
    qcdasf->Adaptor = adaptor;
    
    qcdasf->QCAlignment = ensQCAlignmentNewRef(qca);
    
    qcdasf->Analysis = ensAnalysisNewRef(analysis);
    
    qcdasf->SegmentSequence = ensQCSequenceNewRef(segment);
    
    qcdasf->SegmentStart = sstart;
    
    qcdasf->SegmentEnd = send;
    
    qcdasf->SegmentStrand = sstrand;
    
    qcdasf->FeatureSequence = ensQCSequenceNewRef(feature);
    
    qcdasf->FeatureStart = fstart;
    
    qcdasf->FeatureEnd = fend;
    
    qcdasf->Phase = phase;
    
    qcdasf->Category = category;
    
    qcdasf->Type = type;
    
    return qcdasf;
}




/* @func ensQCDASFeatureNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [EnsPQCDASFeature] Ensembl QC DAS Feature or NULL
** @@
******************************************************************************/

EnsPQCDASFeature ensQCDASFeatureNewObj(const EnsPQCDASFeature object)
{
    EnsPQCDASFeature qcdasf = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(qcdasf);
    
    qcdasf->Use = 1;
    
    qcdasf->Identifier = object->Identifier;
    
    qcdasf->Adaptor = object->Adaptor;
    
    if(object->QCAlignment)
	qcdasf->QCAlignment = ensQCAlignmentNewRef(object->QCAlignment);
    
    if(object->Analysis)
	qcdasf->Analysis = ensAnalysisNewRef(object->Analysis);
    
    if(object->SegmentSequence)
	qcdasf->SegmentSequence = ensQCSequenceNewRef(object->SegmentSequence);
    
    if(object->FeatureSequence)
	qcdasf->FeatureSequence = ensQCSequenceNewRef(object->FeatureSequence);
    
    qcdasf->SegmentStart = object->SegmentStart;
    
    qcdasf->SegmentEnd = object->SegmentEnd;
    
    qcdasf->SegmentStrand = object->SegmentStrand;
    
    qcdasf->FeatureStart = object->FeatureStart;
    
    qcdasf->FeatureEnd = object->FeatureEnd;
    
    qcdasf->Phase = object->Phase;
    
    qcdasf->Category = object->Category;
    
    qcdasf->Type = object->Type;
    
    return qcdasf;
}




/* @func ensQCDASFeatureNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl DAS Feature
**
** @return [EnsPQCDASFeature] Ensembl QC DAS Feature or NULL
** @@
******************************************************************************/

EnsPQCDASFeature ensQCDASFeatureNewRef(EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
	return NULL;
    
    qcdasf->Use++;
    
    return qcdasf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC DAS Feature.
**
** @fdata [EnsPQCDASFeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC DAS Feature object
**
** @argrule * Pqcdasf [EnsPQCDASFeature*] QC DAS Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCDASFeatureDel ***************************************************
**
** Default destructor for an Ensembl QC DAS Feature.
**
** @param [d] Pqcdasf [EnsPQCDASFeature*] Ensembl QC DAS Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensQCDASFeatureDel(EnsPQCDASFeature *Pqcdasf)
{
    EnsPQCDASFeature pthis = NULL;
    
    /*
     ajDebug("ensQCDASFeatureDel\n"
	     "  *Pqcdasf %p\n",
	     *Pqcdasf);
     
     ensQCDASFeatureTrace(*Pqcdasf, 1);
     */
    
    if(!Pqcdasf)
	return;
    
    if(!*Pqcdasf)
	return;

    pthis = *Pqcdasf;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pqcdasf = NULL;
	
	return;
    }
    
    ensQCAlignmentDel(&pthis->QCAlignment);
    
    ensAnalysisDel(&pthis->Analysis);
    
    ensQCSequenceDel(&pthis->SegmentSequence);
    
    ensQCSequenceDel(&pthis->FeatureSequence);
    
    AJFREE(pthis);

    *Pqcdasf = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl QC DASFeature object.
**
** @fdata [EnsPQCDASFeature]
** @fnote None
**
** @nam3rule Get Return QC DAS Feature attribute(s)
** @nam4rule Getadaptor Return the Ensembl DAS Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetQCAlignment Return the Ensembl QC Alignment
** @nam4rule GetAnalysis Return the Ensembl Analysis
** @nam4rule GetSegmentSequence Return the segment Ensembl QC Sequence
** @nam4rule GetFeatureSequence Return the feature Ensembl QC Sequence
** @nam4rule GetSegmentStart Return the segment start
** @nam4rule GetSegmentEnd Return the segment end
** @nam4rule GetSegmentStrand Return the segment strand
** @nam4rule GetFeatureStart Return the feature start
** @nam4rule GetFeatureEnd Return the feature end
** @nam4rule GetPhase Return the phase
** @nam4rule GetCategory Return the category
** @nam4rule GetType Return the type
**
** @argrule * qcdasf [const EnsPQCDASFeature] QC DAS Feature
**
** @valrule Adaptor [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule QCAlignment [EnsPQCAlignment] Ensembl QC Alignment
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule SegmentSequence [EnsPQCSequence] Segment Ensembl QC Sequence
** @valrule FeatureSequence [EnsPQCSequence] Feature Ensembl QC Sequence
** @valrule SegmentStart [ajuint] Segment start
** @valrule SegmentEnd [ajuint] Segment end
** @valrule SegmentStrand [ajint] Segment strand
** @valrule FeatureStart [ajuint] Feature start
** @valrule FeatureEnd [ajuint] Feature end
** @valrule Phase [ajint] Phase
** @valrule Category [AjEnum] Category
** @valrule Type [AjEnum] Type
**
** @fcategory use
******************************************************************************/




/* @func ensQCDASFeatureGetadaptor ********************************************
**
** Get the Ensembl DAS Feature Adaptor element of an Ensembl DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @@
******************************************************************************/

EnsPQCDASFeatureadaptor ensQCDASFeatureGetadaptor(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return NULL;
    
    return qcdasf->Adaptor;
}




/* @func ensQCDASFeatureGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensQCDASFeatureGetIdentifier(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return 0;
    
    return qcdasf->Identifier;
}




/* @func ensQCDASFeatureGetQCAlignment ****************************************
**
** Get the Ensembl QC Alignment element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [EnsPQCAlignment] Ensembl QC Alignment
** @@
******************************************************************************/

EnsPQCAlignment ensQCDASFeatureGetQCAlignment(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return NULL;
    
    return qcdasf->QCAlignment;
}




/* @func ensQCDASFeatureGetAnalysis *******************************************
**
** Get the Ensembl Analysis element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [EnsPAnalysis] Ensembl Analysis
** @@
******************************************************************************/

EnsPAnalysis ensQCDASFeatureGetAnalysis(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return NULL;
    
    return qcdasf->Analysis;
}




/* @func ensQCDASFeatureGetSegmentSequence ************************************
**
** Get the segment Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [EnsPQCSequence] Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQCSequence ensQCDASFeatureGetSegmentSequence(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return NULL;
    
    return qcdasf->SegmentSequence;
}




/* @func ensQCDASFeatureGetFeatureSequence ************************************
**
** Get the feature Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [EnsPQCSequence] Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQCSequence ensQCDASFeatureGetFeatureSequence(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return NULL;
    
    return qcdasf->FeatureSequence;
}




/* @func ensQCDASFeatureGetSegmentStart ***************************************
**
** Get the segment start element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajuint] Segment start
** @@
******************************************************************************/

ajuint ensQCDASFeatureGetSegmentStart(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return 0;
    
    return qcdasf->SegmentStart;
}




/* @func ensQCDASFeatureGetSegmentEnd *****************************************
**
** Get the segment end element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajuint] Segment end
** @@
******************************************************************************/

ajuint ensQCDASFeatureGetSegmentEnd(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return 0;
    
    return qcdasf->SegmentEnd;
}




/* @func ensQCDASFeatureGetSegmentStrand **************************************
**
** Get the segment strand element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajint] Segment strand
** @@
******************************************************************************/

ajint ensQCDASFeatureGetSegmentStrand(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return 0;
    
    return qcdasf->SegmentStrand;
}




/* @func ensQCDASFeatureGetFeatureStart ***************************************
**
** Get the feature start element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajuint] Feature start
** @@
******************************************************************************/

ajuint ensQCDASFeatureGetFeatureStart(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return 0;
    
    return qcdasf->FeatureStart;
}




/* @func ensQCDASFeatureGetFeatureEnd ***************************************
**
** Get the feature end element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajuint] Feature end
** @@
******************************************************************************/

ajuint ensQCDASFeatureGetFeatureEnd(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return 0;
    
    return qcdasf->FeatureEnd;
}




/* @func ensQCDASFeatureGetPhase **********************************************
**
** Get the phase element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajint] Phase
** @@
******************************************************************************/

ajint ensQCDASFeatureGetPhase(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return 0;
    
    return qcdasf->Phase;
}




/* @func ensQCDASFeatureGetCategory *******************************************
**
** Get the category element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [AjEnum] Category
** @@
******************************************************************************/

AjEnum ensQCDASFeatureGetCategory(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return ensEQCDASFeatureCategoryNULL;
    
    return qcdasf->Category;
}




/* @func ensQCDASFeatureGetType ***********************************************
**
** Get the type element of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [AjEnum] Type
** @@
******************************************************************************/

AjEnum ensQCDASFeatureGetType(const EnsPQCDASFeature qcdasf)
{
    if(!qcdasf)
        return ensEQCDASFeatureTypeNULL;
    
    return qcdasf->Type;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl QC DAS Feature object.
**
** @fdata [EnsPQCDASFeature]
** @fnote None
**
** @nam3rule Set Set one element of a QC DAS Feature
** @nam4rule Setadaptor Set the Ensembl QC DAS Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetQCAlignment Set the Ensembl QC Alignment
** @nam4rule SetAnalysis Set the Ensembl Analysis
** @nam4rule SetSegmentSequence Set the segment Ensembl QC Sequence
** @nam4rule SetFeatureSequence Set the feature Ensembl QC Sequence
** @nam4rule SetSegmentStart Set the segment start
** @nam4rule SetSegmentEnd Set the segment end
** @nam4rule SetSegmentStrand Set the segment strand
** @nam4rule SetFeatureStart Set the feature start
** @nam4rule SetFeatureEnd Set the feature end
** @nam4rule SetPhase Set the phase
** @nam4rule SetCategory Set the category
** @nam4rule SetType Set the type
**
** @argrule * qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensQCDASFeatureSetadaptor ********************************************
**
** Set the Ensembl QC DAS Feature Adaptor element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] adaptor [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetadaptor(EnsPQCDASFeature qcdasf,
                                 EnsPQCDASFeatureadaptor adaptor)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetIdentifier(EnsPQCDASFeature qcdasf, ajuint identifier)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetQCAlignment ****************************************
**
** Set the Ensembl QC Alignment element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [u] qca [EnsPQCAlignment] Ensembl QC Alignment
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetQCAlignment(EnsPQCDASFeature qcdasf,
                                     EnsPQCAlignment qca)
{
    if(!qcdasf)
	return ajFalse;
    
    ensQCAlignmentDel(&qcdasf->QCAlignment);
    
    qcdasf->QCAlignment = ensQCAlignmentNewRef(qca);
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetAnalysis *******************************************
**
** Set the Ensembl Analysis element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetAnalysis(EnsPQCDASFeature qcdasf,
                                  EnsPAnalysis analysis)
{
    if(!qcdasf)
	return ajFalse;
    
    ensAnalysisDel(&qcdasf->Analysis);
    
    qcdasf->Analysis = ensAnalysisNewRef(analysis);
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetSegmentSequence ************************************
**
** Set the segment Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [u] qcs [EnsPQCSequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetSegmentSequence(EnsPQCDASFeature qcdasf,
                                         EnsPQCSequence qcs)
{
    if(!qcdasf)
	return ajFalse;
    
    ensQCSequenceDel(&qcdasf->SegmentSequence);
    
    qcdasf->SegmentSequence = ensQCSequenceNewRef(qcs);
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetFeatureSequence ************************************
**
** Set the feature Ensembl QC Sequence element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [u] qcs [EnsPQCSequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetFeatureSequence(EnsPQCDASFeature qcdasf,
                                         EnsPQCSequence qcs)
{
    if(!qcdasf)
	return ajFalse;
    
    ensQCSequenceDel(&qcdasf->FeatureSequence);
    
    qcdasf->FeatureSequence = ensQCSequenceNewRef(qcs);
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetSegmentStart ***************************************
**
** Set the segment start element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] start [ajuint] Segment start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetSegmentStart(EnsPQCDASFeature qcdasf, ajuint start)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->SegmentStart = start;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetSegmentEnd *****************************************
**
** Set the segment end element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] end [ajuint] Segment end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetSegmentEnd(EnsPQCDASFeature qcdasf, ajuint end)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->SegmentEnd = end;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetSegmentStrand **************************************
**
** Set the segment strand element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] strand [ajint] Segment strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetSegmentStrand(EnsPQCDASFeature qcdasf, ajint strand)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->SegmentStrand = strand;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetFeatureStart ***************************************
**
** Set the feature start element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] start [ajuint] Feature start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetFeatureStart(EnsPQCDASFeature qcdasf, ajuint start)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->FeatureStart = start;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetFeatureEnd *****************************************
**
** Set the feature end element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] end [ajuint] Feature end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetFeatureEnd(EnsPQCDASFeature qcdasf, ajuint end)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->FeatureEnd = end;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetPhase **********************************************
**
** Set the phase element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] phase [ajint] Phase
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetPhase(EnsPQCDASFeature qcdasf, ajint phase)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->Phase = phase;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetCategory *******************************************
**
** Set the category element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] category [AjEnum] Category
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetCategory(EnsPQCDASFeature qcdasf, AjEnum category)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->Category = category;
    
    return ajTrue;
}




/* @func ensQCDASFeatureSetType ***********************************************
**
** Set the type element of an Ensembl QC DAS Feature.
**
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureSetType(EnsPQCDASFeature qcdasf, AjEnum type)
{
    if(!qcdasf)
	return ajFalse;
    
    qcdasf->Type = type;
    
    return ajTrue;
}




/* @func ensQCDASFeatureGetMemSize ********************************************
**
** Get the memory size in bytes of an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensQCDASFeatureGetMemSize(const EnsPQCDASFeature qcdasf)
{
    ajuint size = 0;
    
    if(!qcdasf)
	return 0;
    
    size += (ajuint) sizeof (EnsOQCDASFeature);
    
    size += ensAnalysisGetMemSize(qcdasf->Analysis);
    
    size += ensQCAlignmentGetMemSize(qcdasf->QCAlignment);
    
    size += ensQCSequenceGetMemSize(qcdasf->SegmentSequence);
    
    size += ensQCSequenceGetMemSize(qcdasf->FeatureSequence);
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl QC DAS Feature object.
**
** @fdata [EnsPQCDASFeature]
** @nam3rule Trace Report Ensembl QC DAS Feature elements to debug file
**
** @argrule Trace qcdasf [const EnsPQCSequence] Ensembl QC DAS Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensQCDASFeatureTrace *************************************************
**
** Trace an Ensembl QC DAS Feature.
**
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureTrace(const EnsPQCDASFeature qcdasf, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!qcdasf)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensQCDASFeatureTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  QCAlignment %p\n"
	    "%S  Analysis %p\n"
	    "%S  SegmentSequence %p\n"
	    "%S  FeatureSequence %p\n"
	    "%S  SegmentStart %u\n"
	    "%S  SegmentEnd %u\n"
	    "%S  SegmentStrand %d\n"
	    "%S  FeatureStart %u\n"
	    "%S  FeatureEnd %u\n"
	    "%S  Phase %d\n"
	    "%S  Category '%s'\n"
	    "%S  Type '%s'\n",
	    indent, qcdasf,
	    indent, qcdasf->Use,
	    indent, qcdasf->Identifier,
	    indent, qcdasf->Adaptor,
	    indent, qcdasf->QCAlignment,
	    indent, qcdasf->Analysis,
	    indent, qcdasf->SegmentSequence,
	    indent, qcdasf->FeatureSequence,
	    indent, qcdasf->SegmentStart,
	    indent, qcdasf->SegmentEnd,
	    indent, qcdasf->SegmentStrand,
	    indent, qcdasf->FeatureStart,
	    indent, qcdasf->FeatureEnd,
	    indent, qcdasf->Phase,
	    indent, ensQCDASFeatureCategoryToChar(qcdasf->Category),
	    indent, ensQCDASFeatureTypeToChar(qcdasf->Type));
    
    ensAnalysisTrace(qcdasf->Analysis, 1);
    
    ensQCAlignmentTrace(qcdasf->QCAlignment, 1);
    
    ensQCSequenceTrace(qcdasf->SegmentSequence, 1);
    
    ensQCSequenceTrace(qcdasf->FeatureSequence, 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensQCDASFeatureCategoryFromStr ***************************************
**
** Convert an AJAX String into an Ensembl QC DAS Feature category element.
**
** @param [r] category [const AjPStr] Category string
**
** @return [AjEnum] Ensembl QC DAS Feature category element or
**                  ensEQCDASFeatureCategoryNULL
** @@
******************************************************************************/

AjEnum ensQCDASFeatureCategoryFromStr(const AjPStr category)
{
    register ajint i = 0;
    
    AjEnum ecategory = ensEQCDASFeatureCategoryNULL;
    
    for(i = 1; qcDASFeatureCategory[i]; i++)
	if(ajStrMatchCaseC(category, qcDASFeatureCategory[i]))
	    ecategory = i;
    
    if(!ecategory)
	ajDebug("ensQCDASFeatureCategoryFromStr encountered "
		"unexpected string '%S'.\n", category);
    
    return ecategory;
}




/* @func ensQCDASFeatureTypeFromStr *******************************************
**
** Convert an AJAX String into an Ensembl QC DAS Feature type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [AjEnum] Ensembl QC DAS Feature type element or
**                  ensEQCDASFeatureTypeNULL
** @@
******************************************************************************/

AjEnum ensQCDASFeatureTypeFromStr(const AjPStr type)
{
    register ajint i = 0;
    
    AjEnum etype = ensEQCDASFeatureTypeNULL;
    
    for(i = 1; qcDASFeatureType[i]; i++)
	if(ajStrMatchCaseC(type, qcDASFeatureType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensQCDASFeatureTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensQCDASFeatureCategoryToChar ****************************************
**
** Convert an Ensembl QC DAS Feature category element into a
** C-type (char*) string.
**
** @param [r] category [const AjEnum] Ensembl QC DAS Feature category
**                                    enumerator
**
** @return [const char*] Ensembl QC DAS Feature category
**                       C-type (char*) string
** @@
******************************************************************************/

const char* ensQCDASFeatureCategoryToChar(const AjEnum category)
{
    register ajint i = 0;
    
    if(!category)
	return NULL;
    
    for(i = 1; qcDASFeatureCategory[i] && (i < category); i++);
    
    if(!qcDASFeatureCategory[i])
	ajDebug("ensQCDASFeatureCategoryToChar encountered an "
		"out of boundary error on group %d.\n", category);
    
    return qcDASFeatureCategory[i];
}




/* @func ensQCDASFeatureTypeToChar ********************************************
**
** Convert an Ensembl QC DAS Feature type element into a
** C-type (char*) string.
**
** @param [r] type [const AjEnum] Ensembl QC DAS Feature type enumerator
**
** @return [const char*] Ensembl QC DAS Feature type
**                       C-type (char*) string
** @@
******************************************************************************/

const char *ensQCDASFeatureTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; qcDASFeatureType[i] && (i < type); i++);
    
    if(!qcDASFeatureType[i])
	ajDebug("ensQCDASFeatureTypeToChar encountered an "
		"out of boundary error on group %d.\n", type);
    
    return qcDASFeatureType[i];
}




/* @datasection [EnsPQCDASFeatureadaptor] QC DAS Feature Adaptor **************
**
** Functions for manipulating Ensembl QC DAS Feature Adaptor objects
**
** Bio::EnsEMBL::QC::DBSQL::DASFeatureadaptor CVS Revision:
**
** @nam2rule QCDASFeatureadaptor
**
******************************************************************************/

static const char *qcDASFeatureadaptorTables[] =
{
    "das_feature",
    (const char *) NULL
};




static const char *qcDASFeatureadaptorColumns[] =
{
    "das_feature.das_feature_id",
    "das_feature.analysis_id",
    "das_feature.alignment_id",
    "das_feature.feature_id",
    "das_feature.feature_start",
    "das_feature.feature_end",
    "das_feature.segment_id",
    "das_feature.segment_start",
    "das_feature.segment_end",
    "das_feature.segment_strand",
    "das_feature.phase",
    "das_feature.category",
    "das_feature.type",
    (const char *) NULL
};




static EnsOBaseadaptorLeftJoin qcDASFeatureadaptorLeftJoin[] =
{
    {(const char*) NULL, (const char*) NULL}
};




static const char *qcDASFeatureadaptorDefaultCondition =
(const char*) NULL;




static const char *qcDASFeatureadaptorFinalCondition =
(const char *) NULL;




/* @funcstatic qcDASFeatureadaptorFetchAllBySQL *******************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl QC DAS Feature objects.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcDASFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList qcdasfs)
{
    ajint tstrand = 0;
    ajint phase   = 0;
    
    ajuint identifier  = 0;
    ajuint analysisid  = 0;
    ajuint alignmentid = 0;
    ajuint qsid   = 0;
    ajuint qstart = 0;
    ajuint qend   = 0;
    ajuint tsid   = 0;
    ajuint tstart = 0;
    ajuint tend   = 0;
    
    AjEnum ecategory = ensEQCDASFeatureCategoryNULL;
    AjEnum etype     = ensEQCDASFeatureTypeNULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr category = NULL;
    AjPStr type     = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;
    
    EnsPQCAlignment qca         = NULL;
    EnsPQCAlignmentadaptor qcaa = NULL;
    
    EnsPQCDASFeature qcdasf         = NULL;
    EnsPQCDASFeatureadaptor qcdasfa = NULL;
    
    EnsPQCSequence qsequence   = NULL;
    EnsPQCSequence tsequence   = NULL;
    EnsPQCSequenceadaptor qcsa = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!qcdasfs)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisadaptor(dba);
    
    qcaa = ensRegistryGetQCAlignmentadaptor(dba);
    
    qcdasfa = ensRegistryGetQCDASFeatureadaptor(dba);
    
    qcsa = ensRegistryGetQCSequenceadaptor(dba);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier  = 0;
	analysisid  = 0;
	alignmentid = 0;
	qsid        = 0;
	qstart      = 0;
	qend        = 0;
	tsid        = 0;
	tstart      = 0;
	tend        = 0;
	tstrand     = 0;
	phase       = 0;
	category    = ajStrNew();
	type        = ajStrNew();
	ecategory   = ensEQCDASFeatureCategoryNULL;
	etype       = ensEQCDASFeatureTypeNULL;
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &analysisid);
	ajSqlcolumnToUint(sqlr, &alignmentid);
	ajSqlcolumnToUint(sqlr, &qsid);
	ajSqlcolumnToUint(sqlr, &qstart);
	ajSqlcolumnToUint(sqlr, &qend);
	ajSqlcolumnToUint(sqlr, &tsid);
	ajSqlcolumnToUint(sqlr, &tstart);
	ajSqlcolumnToUint(sqlr, &tend);
	ajSqlcolumnToInt(sqlr, &tstrand);
	ajSqlcolumnToInt(sqlr, &phase);
	ajSqlcolumnToStr(sqlr, &category);
	ajSqlcolumnToStr(sqlr, &type);
	
	ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	ensQCAlignmentadaptorFetchByIdentifier(qcaa, alignmentid, &qca);
	
	ensQCSequenceadaptorFetchByIdentifier(qcsa, qsid, &qsequence);
	
	ensQCSequenceadaptorFetchByIdentifier(qcsa, tsid, &tsequence);
	
	ecategory = ensQCDASFeatureCategoryFromStr(category);
	
	etype = ensQCDASFeatureTypeFromStr(type);
	
	qcdasf = ensQCDASFeatureNew(qcdasfa,
				    identifier,
				    qca,
				    analysis,
				    qsequence,
				    qstart,
				    qend,
				    tstrand,
				    tsequence,
				    tstart,
				    tend,
				    phase,
				    ecategory,
				    etype);
	
	ajListPushAppend(qcdasfs, (void *) qcdasf);
	
	ensQCSequenceDel(&qsequence);
	ensQCSequenceDel(&tsequence);
	
	ensAnalysisDel(&analysis);
	
	ensQCAlignmentDel(&qca);
	
	ajStrDel(&category);
	ajStrDel(&type);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC DAS Feature Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** QC DAS Feature Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCDASFeatureadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @argrule Ref object [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
**
** @valrule * [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensQCDASFeatureadaptorNew ********************************************
**
** Default constructor for an Ensembl QC DAS Feature Adaptor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPQCDASFeatureadaptor ensQCDASFeatureadaptorNew(EnsPDatabaseadaptor dba)
{
    if(!dba)
	return NULL;
    
    return ensBaseadaptorNew(dba,
			     qcDASFeatureadaptorTables,
			     qcDASFeatureadaptorColumns,
			     qcDASFeatureadaptorLeftJoin,
			     qcDASFeatureadaptorDefaultCondition,
			     qcDASFeatureadaptorFinalCondition,
			     qcDASFeatureadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC DAS Feature Adaptor.
**
** @fdata [EnsPQCDASFeatureadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC DAS Feature Adaptor object
**
** @argrule * Pqcdasfa [EnsPQCDASFeatureadaptor*] QC DAS Feature Adaptor
**                                                object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCDASFeatureadaptorDel ********************************************
**
** Default destructor for an Ensembl QC DAS Feature Adaptor.
**
** @param [d] Pqcfasfa [EnsPQCDASFeatureadaptor*] Ensembl QC DAS Feature
**                                                Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensQCDASFeatureadaptorDel(EnsPQCDASFeatureadaptor *Pqcdasfa)
{
    /*
     ajDebug("ensQCDASFeatureadaptorDel\n"
	     "  *Pqcdasfa %p\n",
	     *Pqcdasfa);
     */
    
    if(!Pqcdasfa)
	return;
    
    if(!*Pqcdasfa)
	return;
    
    ensBaseadaptorDel(Pqcdasfa);

    return;
}




/* @func ensQCDASFeatureadaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl QC DAS Feature via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl QC DAS Feature.
**
** @param [r] adaptor [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal QC DAS Feature
**                                identifier
** @param [wP] Pqcdasf [EnsPQCDASFeature*] Ensembl QC DAS Feature address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorFetchByIdentifier(EnsPQCDASFeatureadaptor adaptor,
                                               ajuint identifier,
                                               EnsPQCDASFeature *Pqcdasf)
{
    if(!adaptor)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pqcdasf)
	return ajFalse;
    
    *Pqcdasf = (EnsPQCDASFeature)
	ensBaseadaptorFetchByIdentifier(adaptor, identifier);
    
    return ajTrue;
}




/* @func ensQCDASFeatureadaptorFetchAllByQCAlignment **************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl QC Alignment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] qca [const EnsPQCAlignment] Ensembl QC Alignment
** @param [w] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorFetchAllByQCAlignment(
    EnsPQCDASFeatureadaptor qcdasfa,
    const EnsPQCAlignment qca,
    AjPList qcdasfs)
{
    AjPStr constraint = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!qca)
	return ajFalse;
    
    if(!qcdasfs)
	return ajFalse;
    
    constraint = ajFmtStr("das_feature.alignment_id = %u",
			  ensQCAlignmentGetIdentifier(qca));
    
    ensBaseadaptorGenericFetch(qcdasfa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcdasfs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCDASFeatureadaptorFetchAllByFeature ******************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl QC Sequence representing an
** Ensembl QC DAS Feature feature.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] feature [const EnsPQCSequence] Feature Ensembl QC Sequence
** @param [w] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorFetchAllByFeature(EnsPQCDASFeatureadaptor qcdasfa,
                                               const EnsPAnalysis analysis,
                                               const EnsPQCSequence feature,
                                               AjPList qcdasfs)
{
    AjPStr constraint = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!feature)
	return ajFalse;
    
    if(!qcdasfs)
	return ajFalse;
    
    constraint = ajFmtStr("das_feature.feature_id = %u",
			  ensQCSequenceGetIdentifier(feature));
    
    if(analysis)
	ajFmtPrintAppS(&constraint,
		       " AND das_feature.analysis_id = %u",
		       ensAnalysisGetIdentifier(analysis));
    
    ensBaseadaptorGenericFetch(qcdasfa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcdasfs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCDASFeatureadaptorFetchAllBySegment ******************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl QC Sequence representing an
** Ensembl QC DAS Feature segment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] segment [const EnsPQCSequence] Segment Ensembl QC Sequence
** @param [w] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorFetchAllBySegment( EnsPQCDASFeatureadaptor qcdasfa,
                                                const EnsPAnalysis analysis,
                                                const EnsPQCSequence segment,
                                                AjPList qcdasfs)
{
    AjPStr constraint = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!segment)
	return ajFalse;
    
    if(!qcdasfs)
	return ajFalse;
    
    constraint = ajFmtStr("das_feature.segment_id = %u",
			  ensQCSequenceGetIdentifier(segment));
    
    if(analysis)
	ajFmtPrintAppS(&constraint,
		       " AND das_feature.analysis_id = %u",
		       ensAnalysisGetIdentifier(analysis));
    
    ensBaseadaptorGenericFetch(qcdasfa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcdasfs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCDASFeatureadaptorFetchAllByAFS **********************************
**
** Fetch all Ensembl QC DAS Features by an Ensembl Analysis and
** Ensembl QC Sequences representing Ensembl QC DAS Feature feature and
** segment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] feature [const EnsPQCSequence] Feature Ensembl QC Sequence
** @param [r] segment [const EnsPQCSequence] Segment Ensembl QC Sequence
** @param [w] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorFetchAllByAFS(EnsPQCDASFeatureadaptor qcdasfa,
                                           const EnsPAnalysis analysis,
                                           const EnsPQCSequence feature,
                                           const EnsPQCSequence segment,
                                           AjPList qcdasfs)
{
    AjPStr constraint = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!analysis)
	return ajFalse;
    
    if(!feature)
	return ajFalse;
    
    if(!segment)
	return ajFalse;
    
    constraint = ajFmtStr("das_feature.analysis_id = %u "
			  "AND "
			  "das_feature.feature_id = %u "
			  "AND "
			  "das_feature.segment_id = %u ",
			  ensAnalysisGetIdentifier(analysis),
			  ensQCSequenceGetIdentifier(feature),
			  ensQCSequenceGetIdentifier(segment));
    
    ensBaseadaptorGenericFetch(qcdasfa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcdasfs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCDASFeatureadaptorFetchAllByRegion *******************************
**
** Fetch all Ensembl QC DAS Features that fall into a region on the segment.
** The caller is responsible for deleting the Ensembl QC DAS Features
** before deleting the AJAX List.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] segment [const EnsPQCSequence] Segment Ensembl QC Sequence
** @param [r] start [ajuint] Start
** @param [r] end [ajuint] End
** @param [w] qcdasfs [AjPList] AJAX List of Ensembl QC DAS Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorFetchAllByRegion(EnsPQCDASFeatureadaptor qcdasfa,
                                              const EnsPAnalysis analysis,
                                              const EnsPQCSequence segment,
                                              ajuint start,
                                              ajuint end,
                                              AjPList qcdasfs)
{
    AjPStr constraint = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!analysis)
	return ajFalse;
    
    if(!segment)
	return ajFalse;
    
    if(!qcdasfs)
	return ajFalse;
    
    constraint = ajFmtStr("das.analysis_id = %u AND "
			  "das.segment_id = %u AND (("
			  "das.segment_start >= %u AND "
			  "das.segment_start <= %u) OR ("
			  "das.segment_end >= %u AND "
			  "das.segment_end <= %u) OR ("
			  "das.segment_start <= %u AND "
			  "das.segment_end >= %u))",
			  ensAnalysisGetIdentifier(analysis),
			  ensQCSequenceGetIdentifier(segment),
			  start,
			  end,
			  start,
			  end,
			  start,
			  end);
    
    ensBaseadaptorGenericFetch(qcdasfa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcdasfs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCDASFeatureadaptorStore ******************************************
**
** Store an Ensembl QC DAS Feature.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [u] qcdasf [EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorStore(EnsPQCDASFeatureadaptor qcdasfa,
                                   EnsPQCDASFeature qcdasf)
{
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!qcdasf)
	return ajFalse;
    
    if(ensQCDASFeatureGetadaptor(qcdasf) &&
	ensQCDASFeatureGetIdentifier(qcdasf))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcdasfa);
    
    statement = ajFmtStr("INSERT IGNORE INTO "
			 "das_feature "
			 "SET "
			 "das_feature.alignment_id = %u, "
			 "das_feature.analysis_id = %u, "
			 "das_feature.feature_id = %u, "
			 "das_feature.feature_start = %u, "
			 "das_feature.feature_end = %u, "
			 "das_feature.segment_id = %u, "
			 "das_feature.segment_start = %u, "
			 "das_feature.segment_end = %u, "
			 "das_feature.segment_strand = %d, "
			 "das_feature.phase = %d, "
			 "das_feature.category = '%s', "
			 "das_feature.type = '%s'",
			 ensQCAlignmentGetIdentity(qcdasf->QCAlignment),
			 ensAnalysisGetIdentifier(qcdasf->Analysis),
			 ensQCSequenceGetIdentifier(qcdasf->FeatureSequence),
			 qcdasf->FeatureStart,
			 qcdasf->FeatureEnd,
			 ensQCSequenceGetIdentifier(qcdasf->SegmentSequence),
			 qcdasf->SegmentStart,
			 qcdasf->SegmentEnd,
			 qcdasf->SegmentStrand,
			 qcdasf->Phase,
			 ensQCDASFeatureCategoryToChar(qcdasf->Category),
			 ensQCDASFeatureTypeToChar(qcdasf->Type));
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
    {
	ensQCDASFeatureSetIdentifier(qcdasf,
				     ajSqlstatementGetIdentifier(sqls));
	
	ensQCDASFeatureSetadaptor(qcdasf, qcdasfa);
	
	value = ajTrue;
    }
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCDASFeatureadaptorUpdate *****************************************
**
** Update an Ensembl QC DAS Feature.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorUpdate(EnsPQCDASFeatureadaptor qcdasfa,
                                    const EnsPQCDASFeature qcdasf)
{
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!qcdasf)
	return ajFalse;
    
    if(!ensQCDASFeatureGetIdentifier(qcdasf))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcdasfa);
    
    statement = ajFmtStr("UPDATE IGNORE "
			 "das_feature "
			 "SET "
			 "das_feature.alignment_id = %u, "
			 "das_feature.analysis_id = %u, "
			 "das_feature.feature_id = %u, "
			 "das_feature.feature_start = %u, "
			 "das_feature.feature_end = %u, "
			 "das_feature.segment_id = %u, "
			 "das_feature.segment_start = %u, "
			 "das_feature.segment_end = %u, "
			 "das_feature.segment_strand = %d, "
			 "das_feature.phase = %d, "
			 "das_feature.category = '%s', "
			 "das_feature.type = '%s' "
			 "WHERE "
			 "das_feature.das_feature_id = %u",
			 ensQCAlignmentGetIdentity(qcdasf->QCAlignment),
			 ensAnalysisGetIdentifier(qcdasf->Analysis),
			 ensQCSequenceGetIdentifier(qcdasf->FeatureSequence),
			 qcdasf->FeatureStart,
			 qcdasf->FeatureEnd,
			 ensQCSequenceGetIdentifier(qcdasf->SegmentSequence),
			 qcdasf->SegmentStart,
			 qcdasf->SegmentEnd,
			 qcdasf->SegmentStrand,
			 qcdasf->Phase,
			 ensQCDASFeatureCategoryToChar(qcdasf->Category),
			 ensQCDASFeatureTypeToChar(qcdasf->Type),
			 ensQCDASFeatureGetIdentifier(qcdasf));
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
	value = ajTrue;
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCDASFeatureadaptorDelete *****************************************
**
** Delete an Ensembl QC DAS Feature.
**
** @param [r] qcdasfa [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @param [r] qcdasf [const EnsPQCDASFeature] Ensembl QC DAS Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDASFeatureadaptorDelete(EnsPQCDASFeatureadaptor qcdasfa,
                                    const EnsPQCDASFeature qcdasf)
{
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcdasfa)
	return ajFalse;
    
    if(!qcdasf)
	return ajFalse;
    
    if(!ensQCDASFeatureGetIdentifier(qcdasf))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcdasfa);
    
    statement = ajFmtStr("DELETE FROM "
			 "das_feature "
			 "WHERE "
			 "das_feature.das_feature_id = %u",
			 qcdasf->Identifier);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
	value = ajTrue;
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @datasection [EnsPQCVariation] QC Variation ********************************
**
** Functions for manipulating Ensembl QC Variation objects
**
** Bio::EnsEMBL::QC::Variation CVS Revision:
**
** @nam2rule QCVariation
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Variation by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Variation. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCVariation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPQCVariation] Ensembl QC Variation
** @argrule Ref object [EnsPQCVariation] Ensembl QC Variation
**
** @valrule * [EnsPQCVariation] Ensembl QC Variation
**
** @fcategory new
******************************************************************************/




/* @func ensQCVariationNew ****************************************************
**
** Default constructor for an Ensembl QC Variation.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::QC::Variation::new
** @param [u] qca [EnsPQCAlignment] Ensembl QC Alignment
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [u] qsequence [EnsPQCSequence] Query Ensembl QC Sequence
** @param [r] qstart [ajuint] Query start
** @param [r] qend [ajuint] Query end
** @param [r] qstring [AjPStr] Query string
** @param [u] tsequence [EnsPQCSequence] Target Ensembl QC Sequence
** @param [r] tstart [ajuint] Target start
** @param [r] tend [ajuint] Target end
** @param [r] tstring [AjPStr] Target string
** @param [r] class [AjEnum] Class
** @param [r] type [AjEnum] Type
** @param [r] state [AjEnum] State
**
** @return [EnsPQCVariation] Ensembl QC Variation or NULL
** @@
******************************************************************************/

EnsPQCVariation ensQCVariationNew(EnsPQCVariationadaptor adaptor,
                                  ajuint identifier,
                                  EnsPQCAlignment qca,
                                  EnsPAnalysis analysis,
                                  EnsPQCSequence qsequence,
                                  ajuint qstart,
                                  ajuint qend,
                                  AjPStr qstring,
                                  EnsPQCSequence tsequence,
                                  ajuint tstart,
                                  ajuint tend,
                                  AjPStr tstring,
                                  AjEnum class,
                                  AjEnum type,
                                  AjEnum state)
{
    EnsPQCVariation qcv = NULL;
    
    if(!qca)
	return NULL;
    
    if(!analysis)
	return NULL;
    
    if(!qsequence)
	return NULL;
    
    if(!tsequence)
	return NULL;
    
    AJNEW0(qcv);
    
    qcv->Use = 1;
    
    qcv->Identifier = identifier;
    
    qcv->Adaptor = adaptor;
    
    qcv->QCAlignment = ensQCAlignmentNewRef(qca);
    
    qcv->Analysis = ensAnalysisNewRef(analysis);
    
    qcv->QuerySequence = ensQCSequenceNewRef(qsequence);
    
    qcv->QueryStart = qstart;
    
    qcv->QueryEnd = qend;
    
    if(qstring)
	qcv->QueryString = ajStrNewRef(qstring);
    
    qcv->TargetSequence = ensQCSequenceNewRef(tsequence);
    
    qcv->TargetStart = tstart;
    
    qcv->TargetEnd = tend;
    
    if(tstring)
	qcv->TargetString = ajStrNewRef(tstring);
    
    qcv->Class = class;
    
    qcv->Type = type;
    
    qcv->State = state;
    
    return qcv;
}




/* @func ensQCVariationNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPQCVariation] Ensembl QC Variation
**
** @return [EnsPQCVariation] Ensembl QC Variation or NULL
** @@
******************************************************************************/

EnsPQCVariation ensQCVariationNewObj(const EnsPQCVariation object)
{
    EnsPQCVariation qcv= NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(qcv);
    
    qcv->Use = 1;
    
    qcv->Identifier = object->Identifier;
    
    qcv->Adaptor = object->Adaptor;
    
    qcv->QCAlignment = ensQCAlignmentNewRef(object->QCAlignment);
    
    qcv->Analysis = ensAnalysisNewRef(object->Analysis);
    
    qcv->QuerySequence = ensQCSequenceNewRef(object->QuerySequence);
    
    qcv->QueryStart = object->QueryStart;
    
    qcv->QueryEnd = object->QueryEnd;
    
    if(object->QueryString)
	qcv->QueryString = ajStrNewRef(object->QueryString);
    
    qcv->TargetSequence = ensQCSequenceNewRef(object->TargetSequence);
    
    qcv->TargetStart = object->TargetStart;
    
    qcv->TargetEnd = object->TargetEnd;
    
    if(object->TargetString)
	qcv->TargetString = ajStrNewRef(object->TargetString);
    
    qcv->Class = object->Class;
    
    qcv->Type = object->Type;
    
    qcv->State = object->State;
    
    return qcv;
}




/* @func ensQCVariationNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] qca [EnsPQCVariation] Ensembl Variation
**
** @return [EnsPQCVariation] Ensembl QC Variation
** @@
******************************************************************************/

EnsPQCVariation ensQCVariationNewRef(EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    qcv->Use++;
    
    return qcv;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Variation.
**
** @fdata [EnsPQCVariation]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Variation object
**
** @argrule * Pqcv [EnsPQCVariation*] QC Variation object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCVariationDel ****************************************************
**
** Default destructor for an Ensembl QC Variation.
**
** @param [d] Pqcv [EnsPQCVariation*] Ensembl QC Variation address
**
** @return [void]
** @@
******************************************************************************/

void ensQCVariationDel(EnsPQCVariation *Pqcv)
{
    EnsPQCVariation pthis = NULL;
    
    /*
     ajDebug("ensQCVariationDel\n"
	     "  *Pqcv %p\n",
	     *Pqcv);
     
     ensQCVariationTrace(*Pqcv, 1);
     */
    
    if(!Pqcv)
	return;
    
    if(!*Pqcv)
	return;

    pthis = *Pqcv;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pqcv = NULL;
	
	return;
    }
    
    ensQCAlignmentDel(&pthis->QCAlignment);
    
    ensAnalysisDel(&pthis->Analysis);
    
    ensQCSequenceDel(&pthis->QuerySequence);
    
    ensQCSequenceDel(&pthis->TargetSequence);
    
    ajStrDel(&pthis->QueryString);
    ajStrDel(&pthis->TargetString);
    
    AJFREE(pthis);

    *Pqcv = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl QC Variation object.
**
** @fdata [EnsPQCVariation]
** @fnote None
**
** @nam3rule Get Return QC Variation attribute(s)
** @nam4rule Getadaptor Return the Ensembl QC Variation Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetQCAlignment Return the Ensembl QC Alignment
** @nam4rule GetAnalysis Return the Ensembl Analysis
** @nam4rule GetQuerySequence Return the query Ensembl QC Sequence
** @nam4rule GetQueryStart Return the query start
** @nam4rule GetQueryEnd Return the query end
** @nam4rule GetQueryString Return the query string
** @nam4rule GetTargetSequence Return the target Ensembl QC Sequence
** @nam4rule GetTargetStart Return the target start
** @nam4rule GetTargetEnd Return the target end
** @nam4rule GetTargetString Return the target string
** @nam4rule GetClass Return the class
** @nam4rule GetType Return the type
** @nam4rule GetState Return the state
**
** @argrule * qcv [const EnsPQCVariation] QC Variation
**
** @valrule Adaptor [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule QCAlignment [EnsPQCAlignment] Ensembl QC Alignment
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule QuerySequence [AjPStr] Query Ensembl QC Sequence
** @valrule QueryStart [ajuint] Query start
** @valrule QueryEnd [ajuint] Query end
** @valrule QueryString [AjPStr] Query string
** @valrule TargetSequence [AjPStr] Target Ensembl QC Sequence
** @valrule TargetStart [ajuint] Target start
** @valrule TargetEnd [ajuint] Target end
** @valrule TargetString [AjPStr] Target string
** @valrule Class [AjEnum] Class
** @valrule Type [AjEnum] Type
** @valrule State [AjEnum] State
**
** @fcategory use
******************************************************************************/




/* @func ensQCVariationGetadaptor *********************************************
**
** Get the Ensembl QC Variation Adaptor element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @@
******************************************************************************/

EnsPQCVariationadaptor ensQCVariationGetadaptor(const EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    return qcv->Adaptor;
}




/* @func ensQCVariationGetIdentifier ******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensQCVariationGetIdentifier(const EnsPQCVariation qcv)
{
    if(!qcv)
	return 0;
    
    return qcv->Identifier;
}




/* @func ensQCVariationGetQCAlignment *****************************************
**
** Get the Ensembl QC Alignment element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [EnsPQCAlignment] Ensembl QC Alignment
** @@
******************************************************************************/

EnsPQCAlignment ensQCVariationGetQCAlignment(const EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    return qcv->QCAlignment;
}




/* @func ensQCVariationGetAnalysis ********************************************
**
** Get the Ensembl Analysis element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [EnsPAnalysis] Ensembl Analysis
** @@
******************************************************************************/

EnsPAnalysis ensQCVariationGetAnalysis(const EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    return qcv->Analysis;
}




/* @func ensQCVariationGetQuerySequence ***************************************
**
** Get the query Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [EnsPQCSequence] Query Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQCSequence ensQCVariationGetQuerySequence(const EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    return qcv->QuerySequence;
}




/* @func ensQCVariationGetQueryStart ******************************************
**
** Get the query start element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [ajuint] Query start
** @@
******************************************************************************/

ajuint ensQCVariationGetQueryStart(const EnsPQCVariation qcv)
{
    if(!qcv)
	return 0;
    
    return qcv->QueryStart;
}




/* @func ensQCVariationGetQueryEnd ********************************************
**
** Get the query end element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [ajuint] Query end
** @@
******************************************************************************/

ajuint ensQCVariationGetQueryEnd(const EnsPQCVariation qcv)
{
    if(!qcv)
	return 0;
    
    return qcv->QueryEnd;
}




/* @func ensQCVariationGetQueryString *****************************************
**
** Get the query string element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [AjPStr] Query string
** @@
******************************************************************************/

AjPStr ensQCVariationGetQueryString(const EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    return qcv->QueryString;
}




/* @func ensQCVariationGetTargetSequence **************************************
**
** Get the target Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [EnsPQCSequence] Target Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQCSequence ensQCVariationGetTargetSequence(const EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    return qcv->TargetSequence;
}




/* @func ensQCVariationGetTargetStart *****************************************
**
** Get the target start element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [ajuint] Target start
** @@
******************************************************************************/

ajuint ensQCVariationGetTargetStart(const EnsPQCVariation qcv)
{
    if(!qcv)
	return 0;
    
    return qcv->TargetStart;
}




/* @func ensQCVariationGetTargetEnd *******************************************
**
** Get the target end element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [ajuint] Target end
** @@
******************************************************************************/

ajuint ensQCVariationGetTargetEnd(const EnsPQCVariation qcv)
{
    if(!qcv)
	return 0;
    
    return qcv->TargetEnd;
}




/* @func ensQCVariationGetTargetString ****************************************
**
** Get the target string element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [AjPStr] Target string
** @@
******************************************************************************/

AjPStr ensQCVariationGetTargetString(const EnsPQCVariation qcv)
{
    if(!qcv)
	return NULL;
    
    return qcv->TargetString;
}




/* @func ensQCVariationGetClass ***********************************************
**
** Get the class element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [AjEnum] Class
** @@
******************************************************************************/

AjEnum ensQCVariationGetClass(const EnsPQCVariation qcv)
{
    if(!qcv)
	return ensEQCVariationClassNULL;
    
    return qcv->Class;
}




/* @func ensQCVariationGetType ************************************************
**
** Get the type element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [AjEnum] Type
** @@
******************************************************************************/

AjEnum ensQCVariationGetType(const EnsPQCVariation qcv)
{
    if(!qcv)
	return ensEQCVariationTypeNULL;
    
    return qcv->Type;
}




/* @func ensQCVariationGetState ***********************************************
**
** Get the state element of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [AjEnum] State
** @@
******************************************************************************/

AjEnum ensQCVariationGetState(const EnsPQCVariation qcv)
{
    if(!qcv)
	return ensEQCVariationStateNULL;
    
    return qcv->State;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl QC Variation object.
**
** @fdata [EnsPQCVariation]
** @fnote None
**
** @nam3rule Set Set one element of a QC Variation
** @nam4rule Setadaptor Set the Ensembl QC Variation Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetQCAlignment Set the Ensembl QC Alignment
** @nam4rule SetAnalysis Set the Ensembl Analysis
** @nam4rule SetQuerySequence Set the query Ensembl QC Sequence
** @nam4rule SetQueryStart Set the query start
** @nam4rule SetQueryEnd Set the query end
** @nam4rule SetQueryString Set the query string
** @nam4rule SetTargetSequence Set the target Ensembl QC Sequence
** @nam4rule SetTargetStart Set the target start
** @nam4rule SetTargetEnd Set the target end
** @nam4rule SetTargetString Set the target string
** @nam4rule SetClass Set the class
** @nam4rule SetType Set the type
** @nam4rule SetState Set the state
**
** @argrule * qcv [EnsPQCVariation] Ensembl QC Variation
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensQCVariationSetadaptor *********************************************
**
** Set the Ensembl Database Adaptor element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetadaptor(EnsPQCVariation qcv,
                                EnsPQCVariationadaptor qcva)
{
    if(!qcv)
	return ajFalse;
    
    qcv->Adaptor = qcva;
    
    return ajTrue;
}




/* @func ensQCVariationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetIdentifier(EnsPQCVariation qcv, ajuint identifier)
{
    if(!qcv)
	return ajFalse;
    
    qcv->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensQCVariationSetAnalysis ********************************************
**
** Set the Ensembl Analysis element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetAnalysis(EnsPQCVariation qcv, EnsPAnalysis analysis)
{
    if(!qcv)
	return ajFalse;
    
    ensAnalysisDel(&qcv->Analysis);
    
    qcv->Analysis = ensAnalysisNewRef(analysis);
    
    return ajTrue;
}




/* @func ensQCVariationSetQCAlignment *****************************************
**
** Set the Ensembl QC Alignment element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [u] qca [EnsPQCAlignment] Ensembl QC Alignment
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetQCAlignment(EnsPQCVariation qcv, EnsPQCAlignment qca)
{
    if(!qcv)
	return ajFalse;
    
    ensQCAlignmentDel(&qcv->QCAlignment);
    
    qcv->QCAlignment = ensQCAlignmentNewRef(qca);
    
    return ajTrue;
}




/* @func ensQCVariationSetQuerySequence ***************************************
**
** Set the query Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [u] qsequence [EnsPQCSequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetQuerySequence(EnsPQCVariation qcv,
                                      EnsPQCSequence qsequence)
{
    if(!qcv)
	return ajFalse;
    
    ensQCSequenceDel(&qcv->QuerySequence);
    
    qcv->QuerySequence = ensQCSequenceNewRef(qsequence);
    
    return ajTrue;
}




/* @func ensQCVariationSetQueryStart ******************************************
**
** Set the query start element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] qstart [ajuint] Query start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetQueryStart(EnsPQCVariation qcv, ajuint qstart)
{
    if(!qcv)
	return ajFalse;
    
    qcv->QueryStart = qstart;
    
    return ajTrue;
}




/* @func ensQCVariationSetQueryEnd ********************************************
**
** Set the query end element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] qend [ajuint] Query end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetQueryEnd(EnsPQCVariation qcv, ajuint qend)
{
    if(!qcv)
	return ajFalse;
    
    qcv->QueryEnd = qend;
    
    return ajTrue;
}




/* @func ensQCVariationSetQueryString *****************************************
**
** Set the query string element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [u] qstring [AjPStr] Query string
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetQueryString(EnsPQCVariation qcv, AjPStr qstring)
{
    if(!qcv)
	return ajFalse;
    
    ajStrDel(&qcv->QueryString);
    
    if(qcv->QueryString)
	qcv->QueryString = ajStrNewRef(qstring);
    
    return ajTrue;
}




/* @func ensQCVariationSetTargetSequence **************************************
**
** Set the target Ensembl QC Sequence element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [u] tsequence [EnsPQCSequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetTargetSequence(EnsPQCVariation qcv,
                                       EnsPQCSequence tsequence)
{
    if(!qcv)
	return ajFalse;
    
    ensQCSequenceDel(&qcv->TargetSequence);
    
    qcv->TargetSequence = ensQCSequenceNewRef(tsequence);
    
    return ajTrue;
}




/* @func ensQCVariationSetTargetStart *****************************************
**
** Set the target start element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] tstart [ajuint] Target start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetTargetStart(EnsPQCVariation qcv, ajuint tstart)
{
    if(!qcv)
	return ajFalse;
    
    qcv->TargetStart = tstart;
    
    return ajTrue;
}




/* @func ensQCVariationSetTargetEnd *******************************************
**
** Set the target end element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] tend [ajuint] Target end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetTargetEnd(EnsPQCVariation qcv, ajuint tend)
{
    if(!qcv)
	return ajFalse;
    
    qcv->TargetEnd = tend;
    
    return ajTrue;
}




/* @func ensQCVariationSetTargetString ****************************************
**
** Set the target string element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [u] tstring [AjPStr] Target string
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetTargetString(EnsPQCVariation qcv, AjPStr tstring)
{
    if(!qcv)
	return ajFalse;
    
    ajStrDel(&qcv->TargetString);
    
    if(tstring)
	qcv->TargetString = ajStrNewRef(tstring);
    
    return ajTrue;
}




/* @func ensQCVariationSetClass ***********************************************
**
** Set the class element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] class [AjEnum] Class
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetClass(EnsPQCVariation qcv, AjEnum class)
{
    if(!qcv)
	return ajFalse;
    
    qcv->Class = class;
    
    return ajTrue;
}




/* @func ensQCVariationSetType ************************************************
**
** Set the type element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetType(EnsPQCVariation qcv, AjEnum type)
{
    if(!qcv)
	return ajFalse;
    
    qcv->Type = type;
    
    return ajTrue;
}




/* @func ensQCVariationSetState ***********************************************
**
** Set the state element of an Ensembl QC Variation.
**
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
** @param [r] state [AjEnum] State
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationSetState(EnsPQCVariation qcv, AjEnum state)
{
    if(!qcv)
	return ajFalse;
    
    qcv->State = state;
    
    return ajTrue;
}




/* @func ensQCVariationGetMemSize *********************************************
**
** Get the memory size in bytes of an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensQCVariationGetMemSize(const EnsPQCVariation qcv)
{
    ajuint size = 0;
    
    if(!qcv)
	return 0;
    
    size += (ajuint) sizeof (EnsOQCVariation);
    
    size += ensQCAlignmentGetMemSize(qcv->QCAlignment);
    
    size += ensAnalysisGetMemSize(qcv->Analysis);
    
    size += ensQCSequenceGetMemSize(qcv->QuerySequence);
    size += ensQCSequenceGetMemSize(qcv->TargetSequence);
    
    if(qcv->QueryString)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcv->QueryString);
    }
    
    if(qcv->TargetString)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcv->TargetString);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl QC Variation object.
**
** @fdata [EnsPQCVariation]
** @nam3rule Trace Report QC Variation elements to debug file
**
** @argrule Trace qcv [const EnsPQCVariation] Ensembl QC Variation
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensQCVariationTrace **************************************************
**
** Trace an Ensembl QC Variation.
**
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationTrace(const EnsPQCVariation qcv, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!qcv)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensQCVariationTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  QCAlignment %p\n"
	    "%S  Analysis %p\n"
	    "%S  QuerySequence %p\n"
	    "%S  QueryStart %u\n"
	    "%S  QueryEnd %u\n"
	    "%S  QueryString '%S'\n"
	    "%S  TargetSequence %p\n"
	    "%S  TargetStart %u\n"
	    "%S  TargetEnd %u\n"
	    "%S  TargetString '%S'\n"
	    "%S  Class '%s'\n"
	    "%S  Type '%s'\n"
	    "%S  State '%s'\n",
	    indent, qcv,
	    indent, qcv->Use,
	    indent, qcv->Identifier,
	    indent, qcv->Adaptor,
	    indent, qcv->QCAlignment,
	    indent, qcv->Analysis,
	    indent, qcv->QuerySequence,
	    indent, qcv->QueryStart,
	    indent, qcv->QueryEnd,
	    indent, qcv->QueryString,
	    indent, qcv->TargetSequence,
	    indent, qcv->TargetStart,
	    indent, qcv->TargetEnd,
	    indent, qcv->TargetString,
	    indent, ensQCVariationClassToChar(qcv->Class),
	    indent, ensQCVariationTypeToChar(qcv->Type),
	    indent, ensQCVariationStateToChar(qcv->State));
    
    ensQCAlignmentTrace(qcv->QCAlignment, level + 1);
    
    ensAnalysisTrace(qcv->Analysis, level + 1);
    
    ensQCSequenceTrace(qcv->QuerySequence, level + 1);
    
    ensQCSequenceTrace(qcv->TargetSequence, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensQCVariationClassFromStr *******************************************
**
** Convert an AJAX String into an Ensembl QC Variation class element.
**
** @param [r] vclass [const AjPStr] Class string
**
** @return [AjEnum] Ensembl QC Variation class element or
**                  ensEQCVariationClassNULL
** @@
******************************************************************************/

AjEnum ensQCVariationClassFromStr(const AjPStr vclass)
{
    register ajint i = 0;
    
    AjEnum eclass = ensEQCVariationClassNULL;
    
    for(i = 1; qcVariationClass[i]; i++)
	if(ajStrMatchCaseC(vclass, qcVariationClass[i]))
	    eclass = i;
    
    if(!eclass)
	ajDebug("ensQCVariationClassFromStr encountered "
		"unexpected string '%S'.\n", vclass);
    
    return eclass;
}




/* @func ensQCVariationTypeFromStr ********************************************
**
** Convert an AJAX String into an Ensembl QC Variation type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [AjEnum] Ensembl QC Variation type element or
**                  ensEQCVariationTypeNULL
** @@
******************************************************************************/

AjEnum ensQCVariationTypeFromStr(const AjPStr type)
{
    register ajint i = 0;
    
    AjEnum etype = ensEQCVariationTypeNULL;
    
    for(i = 1; qcVariationType[i]; i++)
	if(ajStrMatchCaseC(type, qcVariationType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensQCVariationTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensQCVariationStateFromStr *******************************************
**
** Convert an AJAX String into an Ensembl QC Variation state element.
**
** @param [r] state [const AjPStr] State string
**
** @return [AjEnum] Ensembl QC Variation state element or
**                  ensEQCVariationStateNULL
** @@
******************************************************************************/

AjEnum ensQCVariationStateFromStr(const AjPStr state)
{
    register ajint i = 0;
    
    AjEnum estate = ensEQCVariationStateNULL;
    
    for(i = 1; qcVariationState[i]; i++)
	if(ajStrMatchCaseC(state, qcVariationState[i]))
	    estate = i;
    
    if(!estate)
	ajDebug("ensQCVariationStateFromStr encountered "
		"unexpected string '%S'.\n", state);
    
    return estate;
}




/* @func ensQCVariationClassToChar ********************************************
**
** Convert an Ensembl QC Variation class element into a C-type (char*) string.
**
** @param [r] class [const AjEnum] Ensembl QC Variation class enumerator
**
** @return [const char*] Ensembl QC Variation class C-type (char*) string
** @@
******************************************************************************/

const char *ensQCVariationClassToChar(const AjEnum vclass)
{
    register ajint i = 0;
    
    if(!vclass)
	return NULL;
    
    for(i = 1; qcVariationClass[i] && (i < vclass); i++);
    
    if(!qcVariationClass[i])
	ajDebug("ensQCVariationClassToChar encountered an "
		"out of boundary error on group %d.\n", vclass);
    
    return qcVariationClass[i];
}




/* @func ensQCVariationTypeToChar *********************************************
**
** Convert an Ensembl QC Variation type element into a C-type (char*) string.
**
** @param [r] type [const AjEnum] Ensembl QC Variation type enumerator
**
** @return [const char*] Ensembl QC Variation type C-type (char*) string
** @@
******************************************************************************/

const char *ensQCVariationTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; qcVariationType[i] && (i < type); i++);
    
    if(!qcVariationType[i])
	ajDebug("ensQCVariationTypeToChar encountered an "
		"out of boundary error on group %d.\n", type);
    
    return qcVariationType[i];
}




/* @func ensQCVariationStateToChar ********************************************
**
** Convert an Ensembl QC Variation state element into a C-type (char*) string.
**
** @param [r] state [const AjEnum] Ensembl QC Variation state enumerator
**
** @return [const char*] Ensembl QC Variation state C-type (char*) string
** @@
******************************************************************************/

const char *ensQCVariationStateToChar(const AjEnum state)
{
    register ajint i = 0;
    
    if(!state)
	return NULL;
    
    for(i = 1; qcVariationState[i] && (i < state); i++);
    
    if(!qcVariationState[i])
	ajDebug("ensQCVariationStateToChar encountered an "
		"out of boundary error on group %d.\n", state);
    
    return qcVariationState[i];
}




/* @datasection [EnsPQCVariationadaptor] QC Variation Adaptor *****************
**
** Functions for manipulating Ensembl QC Variation Adaptor objects
**
** Bio::EnsEMBL::QC::DBSQL::Variationadaptor CVS Revision:
**
** @nam2rule QCVariationadaptor
**
******************************************************************************/

static const char *qcVariationadaptorTables[] =
{
    "variation",
    (const char *) NULL
};




static const char *qcVariationadaptorColumns[] =
{
    "variation.variation_id",
    "variation.analysis_id",
    "variation.alignment_id",
    "variation.query_db_id",
    "variation.query_id"
    "variation.query_start",
    "variation.query_end",
    "variation.query_seq",
    "variation.target_db_id"
    "variation.target_id",
    "variation.target_start",
    "variation.target_end",
    "variation.target_seq",
    "variation.class",
    "variation.type",
    "variation.state",
    (const char *) NULL
};




static EnsOBaseadaptorLeftJoin qcVariationadaptorLeftJoin[] =
{
    {(const char*) NULL, (const char*) NULL}
};




static const char *qcVariationadaptorDefaultCondition =
(const char*) NULL;




static const char *qcVariationadaptorFinalCondition =
(const char *) NULL;




/* @funcstatic qcVariationadaptorFetchAllBySQL ********************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl QC Variation objects.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcVariationadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                              const AjPStr statement,
                                              EnsPAssemblyMapper am,
                                              EnsPSlice slice,
                                              AjPList qcvs)
{
    ajuint identifier  = 0;
    ajuint analysisid  = 0;
    ajuint alignmentid = 0;

    ajuint qdbid  = 0;
    ajuint qsid   = 0;
    ajuint qstart = 0;
    ajuint qend   = 0;
    ajuint tdbid  = 0;
    ajuint tsid   = 0;
    ajuint tstart = 0;
    ajuint tend   = 0;
    
    AjEnum eclass = ensEQCVariationClassNULL;
    AjEnum etype  = ensEQCVariationTypeNULL;
    AjEnum estate = ensEQCVariationStateNULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr qstring = NULL;
    AjPStr tstring = NULL;
    AjPStr class   = NULL;
    AjPStr type    = NULL;
    AjPStr state   = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;
    
    EnsPQCAlignment qca         = NULL;
    EnsPQCAlignmentadaptor qcaa = NULL;
    
    EnsPQCSequence qsequence   = NULL;
    EnsPQCSequence tsequence   = NULL;
    EnsPQCSequenceadaptor qcsa = NULL;
    
    EnsPQCVariation qcv         = NULL;
    EnsPQCVariationadaptor qcva = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!qcvs)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisadaptor(dba);
    
    qcaa = ensRegistryGetQCAlignmentadaptor(dba);
    
    qcva = ensRegistryGetQCVariationadaptor(dba);
    
    qcsa = ensRegistryGetQCSequenceadaptor(dba);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier  = 0;
	analysisid  = 0;
	alignmentid = 0;
	qdbid       = 0;
	qsid        = 0;
	qstart      = 0;
	qend        = 0;
	qstring     = ajStrNew();
	tdbid       = 0;
	tsid        = 0;
	tstart      = 0;
	tend        = 0;
	tstring     = ajStrNew();
	class       = ajStrNew();
	type        = ajStrNew();
	state       = ajStrNew();
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &analysisid);
	ajSqlcolumnToUint(sqlr, &alignmentid);
	ajSqlcolumnToUint(sqlr, &qdbid);
	ajSqlcolumnToUint(sqlr, &qsid);
	ajSqlcolumnToUint(sqlr, &qstart);
	ajSqlcolumnToUint(sqlr, &qend);
	ajSqlcolumnToStr(sqlr, &qstring);
	ajSqlcolumnToUint(sqlr, &tdbid);
	ajSqlcolumnToUint(sqlr, &tsid);
	ajSqlcolumnToUint(sqlr, &tstart);
	ajSqlcolumnToUint(sqlr, &tend);
	ajSqlcolumnToStr(sqlr, &tstring);
	ajSqlcolumnToStr(sqlr, &class);
	ajSqlcolumnToStr(sqlr, &type);
	ajSqlcolumnToStr(sqlr, &state);
	
	ensQCAlignmentadaptorFetchByIdentifier(qcaa, alignmentid, &qca);
	
	ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	ensQCSequenceadaptorFetchByIdentifier(qcsa, qsid, &qsequence);
	
	ensQCSequenceadaptorFetchByIdentifier(qcsa, tsid, &tsequence);
	
	eclass = ensQCVariationClassFromStr(class);
	
	etype = ensQCVariationTypeFromStr(type);
	
	estate = ensQCVariationClassFromStr(class);
	
	qcv = ensQCVariationNew(qcva,
				identifier,
				qca,
				analysis,
				qsequence,
				qstart,
				qend,
				qstring,
				tsequence,
				tstart,
				tend,
				tstring,
				eclass,
				etype,
				estate);
	
	ajListPushAppend(qcvs, (void *) qcv);
	
	ensQCSequenceDel(&qsequence);
	ensQCSequenceDel(&tsequence);
	
	ensAnalysisDel(&analysis);
	
	ensQCAlignmentDel(&qca);
	
	ajStrDel(&qstring);
	ajStrDel(&tstring);
	ajStrDel(&class);
	ajStrDel(&type);
	ajStrDel(&state);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Variation Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Variation Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCVariationadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @argrule Ref object [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
**
** @valrule * [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensQCVariationadaptorNew *********************************************
**
** Default constructor for an Ensembl QC Variation Adaptor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor or NULL
** @@
******************************************************************************/

EnsPQCVariationadaptor ensQCVariationadaptorNew(EnsPDatabaseadaptor dba)
{
    if(!dba)
	return NULL;
    
    return ensBaseadaptorNew(dba,
			     qcVariationadaptorTables,
			     qcVariationadaptorColumns,
			     qcVariationadaptorLeftJoin,
			     qcVariationadaptorDefaultCondition,
			     qcVariationadaptorFinalCondition,
			     qcVariationadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Variation Adaptor.
**
** @fdata [EnsPQCVariationadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Variation Adaptor object
**
** @argrule * Pqcva [EnsPQCVariationadaptor*] QC Variation Adaptor
**                                            object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCVariationadaptorDel *********************************************
**
** Default destructor for an Ensembl QC Variation Adaptor.
**
** @param [d] Pqcva [EnsPQCVariationadaptor*] Ensembl QC Variation Adaptor
**                                            address
**
** @return [void]
** @@
******************************************************************************/

void ensQCVariationadaptorDel(EnsPQCVariationadaptor* Pqcva)
{
    /*
     ajDebug("ensQCVariationadaptorDel\n"
	     "  *Pqcva %p\n",
	     *Pqcva);
     */
    
    if(!Pqcva)
	return;
    
    if(!*Pqcva)
	return;
    
    ensBaseadaptorDel(Pqcva);
    
    return;
}




/* @func ensQCVariationadaptorFetchByIdentifier *******************************
**
** Fetch an Ensembl QC Variation via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl QC Variation.
**
** @param [r] adaptor [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal QC Variation identifier
** @param [wP] Pqcv [EnsPQCVariation*] Ensembl QC Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorFetchByIdentifier(EnsPQCVariationadaptor adaptor,
                                              ajuint identifier,
                                              EnsPQCVariation *Pqcv)
{
    if(!adaptor)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pqcv)
	return ajFalse;
    
    *Pqcv = (EnsPQCVariation)
	ensBaseadaptorFetchByIdentifier(adaptor, identifier);
    
    return ajTrue;
}




/* @func ensQCVariationadaptorFetchAllByQCAlignment ***************************
**
** Fetch all Ensembl QC Variations by an Ensembl QC Alignment.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [r] qca [const EnsPQCAlignment] Ensembl QC Alignment
** @param [w] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorFetchAllByQCAlignment(EnsPQCVariationadaptor qcva,
                                                  const EnsPQCAlignment qca,
                                                  AjPList qcvs)
{
    AjPStr constraint = NULL;
    
    if(!qcva)
	return ajFalse;
    
    if(!qca)
	return ajFalse;
    
    if(!qcvs)
	return ajFalse;
    
    constraint = ajFmtStr("variation.alignment_id = %u",
			  ensQCAlignmentGetIdentifier(qca));
    
    ensBaseadaptorGenericFetch(qcva,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcvs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCVariationadaptorFetchAllByAnalysisQueryTarget *******************
**
** Fetch all Ensembl QC Variations by Ensembl an Analysis, as well as
** Query and Target Ensembl Sequences.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQCDatabase] Query Ensembl QC Database
** @param [r] tdb [const EnsPQCDatabase] Target Ensembl QC Database
** @param [w] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorFetchAllByAnalysisQueryTarget(
    EnsPQCVariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQCDatabase qdb,
    const EnsPQCDatabase tdb,
    AjPList qcvs)
{
    AjPStr constraint = NULL;
    
    if(!qcva)
	return ajFalse;
    
    if(!analysis)
	return ajFalse;
    
    if(!qdb)
	return ajFalse;
    
    if(!tdb)
	return ajFalse;
    
    if(!qcvs)
	return ajFalse;
    
    constraint = ajFmtStr("variation.analysis_id = %u "
			  "AND "
			  "variation.query_db_id = %u "
			  "AND "
			  "variation.target_db_id = %u",
			  ensAnalysisGetIdentifier(analysis),
			  ensQCDatabaseGetIdentifier(qdb),
			  ensQCDatabaseGetIdentifier(tdb));
    
    ensBaseadaptorGenericFetch(qcva,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcvs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCVariationadaptorFetchAllByQuery *********************************
**
** Fetch all Ensembl QC Variations by a Query Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQCDatabase] Query Ensembl QC Database
** @param [w] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorFetchAllByQuery(EnsPQCVariationadaptor qcva,
                                            const EnsPAnalysis analysis,
                                            const EnsPQCDatabase qdb,
                                            AjPList qcvs)
{
    AjPStr constraint = NULL;
    
    if(!qcva)
	return ajFalse;
    
    if(!qdb)
	return ajFalse;
    
    if(!qcvs)
	return ajFalse;
    
    constraint = ajFmtStr("variation.query_db_id = %u",
			  ensQCDatabaseGetIdentifier(qdb));
    
    if(analysis)
	ajFmtPrintAppS(&constraint,
		       " AND variation.analysis_id = %u",
		       ensAnalysisGetIdentifier(analysis));
    
    ensBaseadaptorGenericFetch(qcva,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcvs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCVariationadaptorFetchAllByTarget ********************************
**
** Fetch all Ensembl QC Variations by a Target Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Variations
** before deleting the AJAX List.
**
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] tdb [const EnsPQCDatabase] Target Ensembl QC Database
** @param [w] qcvs [AjPList] AJAX List of Ensembl QC Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorFetchAllByTarget(EnsPQCVariationadaptor qcva,
                                             const EnsPAnalysis analysis,
                                             const EnsPQCDatabase tdb,
                                             AjPList qcvs)
{
    AjPStr constraint = NULL;
    
    if(!qcva)
	return ajFalse;
    
    if(!tdb)
	return ajFalse;
    
    if(!qcvs)
	return ajFalse;
    
    constraint = ajFmtStr("variation.target_db_id = %u",
			  ensQCDatabaseGetIdentifier(tdb));
    
    if(analysis)
	ajFmtPrintAppS(&constraint,
		       " AND variation.analysis_id = %u",
		       ensAnalysisGetIdentifier(analysis));
    
    ensBaseadaptorGenericFetch(qcva,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcvs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCVariationadaptorStore *******************************************
**
** Store an Ensembl QC Variation.
**
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [u] qcv [EnsPQCVariation] Ensembl QC Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorStore(EnsPQCVariationadaptor qcva,
                                  EnsPQCVariation qcv)
{
    char *txtqstr = NULL;
    char *txttstr = NULL;
    
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcva)
	return ajFalse;
    
    if(!qcv)
	return ajFalse;
    
    if(ensQCVariationGetadaptor(qcv) && ensQCVariationGetIdentifier(qcv))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcva);
    
    ensDatabaseadaptorEscapeC(dba, &txtqstr, qcv->QueryString);
    
    ensDatabaseadaptorEscapeC(dba, &txttstr, qcv->TargetString);
    
    statement = ajFmtStr("INSERT IGNORE INTO "
			 "variation "
			 "SET "
			 "variation.analysis_id = %u, "
			 "variation.alignment_id = %u, "
			 "variation.query_db_id = %u, "
			 "variation.query_id = %u, "
			 "variation.query_start = %u, "
			 "variation.query_end = %u, "
			 "variation.query_seq = '%s', "
			 "variation.target_db_id = %u, "
			 "variation.target_id = %u, "
			 "variation.target_start = %u, "
			 "variation.target_end = %u, "
			 "variation.target_seq = '%s', "
			 "variation.class = '%s', "
			 "variation.type = '%s', "
			 "variation.state = '%s'",
			 ensAnalysisGetIdentifier(qcv->Analysis),
			 ensQCAlignmentGetIdentity(qcv->QCAlignment),
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcv->QuerySequence),
			 ensQCSequenceGetIdentifier(qcv->QuerySequence),
			 qcv->QueryStart,
			 qcv->QueryEnd,
			 txtqstr,
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcv->TargetSequence),
			 ensQCSequenceGetIdentifier(qcv->TargetSequence),
			 qcv->TargetStart,
			 qcv->TargetEnd,
			 txttstr,
			 ensQCVariationClassToChar(qcv->Class),
			 ensQCVariationTypeToChar(qcv->Type),
			 ensQCVariationStateToChar(qcv->State));
    
    ajCharDel(&txtqstr);
    ajCharDel(&txttstr);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
    {
	ensQCVariationSetIdentifier(qcv, ajSqlstatementGetIdentifier(sqls));
	
	ensQCVariationSetadaptor(qcv, qcva);
	
	value = ajTrue;
    }
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCVariationadaptorUpdate ******************************************
**
** Update an Ensembl QC Variation.
**
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorUpdate(EnsPQCVariationadaptor qcva,
                                   const EnsPQCVariation qcv)
{
    char *txtqstr = NULL;
    char *txttstr = NULL;
    
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcva)
	return ajFalse;
    
    if(!qcv)
	return ajFalse;
    
    if(!ensQCVariationGetIdentifier(qcv))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcva);
    
    ensDatabaseadaptorEscapeC(dba, &txtqstr, qcv->QueryString);
    
    ensDatabaseadaptorEscapeC(dba, &txttstr, qcv->TargetString);
    
    statement = ajFmtStr("UPDATE IGNORE "
			 "variation "
			 "SET "
			 "variation.analysis_id = %u, "
			 "variation.alignment_id = %u, "
			 "variation.query_db_id = %u, "
			 "variation.query_id = %u, "
			 "variation.query_start = %u, "
			 "variation.query_end = %u, "
			 "variation.query_seq = '%s', "
			 "variation.target_db_id = %u, "
			 "variation.target_id = %u, "
			 "variation.target_start = %u, "
			 "variation.target_end = %u, "
			 "variation.target_seq = '%s', "
			 "variation.class = '%s', "
			 "variation.type = '%s', "
			 "variation.state = '%s' "
			 "WHERE "
			 "variation.variation_id = %u",
			 ensAnalysisGetIdentifier(qcv->Analysis),
			 ensQCAlignmentGetIdentity(qcv->QCAlignment),
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcv->QuerySequence),
			 ensQCSequenceGetIdentifier(qcv->QuerySequence),
			 qcv->QueryStart,
			 qcv->QueryEnd,
			 txtqstr,
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcv->TargetSequence),
			 ensQCSequenceGetIdentifier(qcv->TargetSequence),
			 qcv->TargetStart,
			 qcv->TargetEnd,
			 txttstr,
			 ensQCVariationClassToChar(qcv->Class),
			 ensQCVariationTypeToChar(qcv->Type),
			 ensQCVariationStateToChar(qcv->State),
			 qcv->Identifier);
    
    ajCharDel(&txtqstr);
    ajCharDel(&txttstr);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
	value = ajTrue;
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCVariationadaptorDelete ******************************************
**
** Delete an Ensembl QC Variation.
**
** @param [r] qcva [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @param [r] qcv [const EnsPQCVariation] Ensembl QC Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCVariationadaptorDelete(EnsPQCVariationadaptor qcva,
                                   const EnsPQCVariation qcv)
{
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcva)
	return ajFalse;
    
    if(!qcv)
	return ajFalse;
    
    if(!ensQCVariationGetIdentifier(qcv))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcva);
    
    statement = ajFmtStr("DELETE FROM "
			 "variation "
			 "WHERE "
			 "variation.variation_id = %u",
			 qcv->Identifier);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
	value = ajTrue;
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @datasection [EnsPQCSubmission] QC Submission ******************************
**
** Functions for manipulating Ensembl QC Submission objects
**
** Bio::EnsEMBL::QC::Submission CVS Revision:
**
** @nam2rule QCSubmission
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Submission by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Submission. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCSubmission]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPQCSubmission] Ensembl QC Submission
** @argrule Ref object [EnsPQCSubmission] Ensembl QC Submission
**
** @valrule * [EnsPQCSubmission] Ensembl QC Submission
**
** @fcategory new
******************************************************************************/




/* @func ensQCSubmissionNew ***************************************************
**
** Default constructor for an Ensembl QC Submission.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::QC::Submission::new
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [u] qsequence [EnsPQCSequence] Query Ensembl QC Sequence
** @param [u] tsequence [EnsPQCSequence] Target Ensembl QC Sequence
** @param [r] tstart [ajuint] Target start
** @param [r] tend [ajuint] Target end
** @param [r] tstrand [ajint] Target strand
** @param [r] analysisjobid [ajuint] Ensembl Hive Analysis Job identifier
**
** @return [EnsPQCSubmission] Ensembl QC Submission or NULL
** @@
******************************************************************************/

EnsPQCSubmission ensQCSubmissionNew(EnsPQCSubmissionadaptor adaptor,
                                    ajuint identifier,
                                    EnsPAnalysis analysis,
                                    EnsPQCSequence qsequence,
                                    EnsPQCSequence tsequence,
                                    ajuint tstart,
                                    ajuint tend,
                                    ajint tstrand,
                                    ajuint analysisjobid)
{
    EnsPQCSubmission qcs = NULL;
    
    if(!analysis)
	return NULL;
    
    if(!qsequence)
	return NULL;
    
    if(!tsequence)
	return NULL;
    
    AJNEW0(qcs);
    
    qcs->Use = 1;
    
    qcs->Identifier = identifier;
    
    qcs->Adaptor = adaptor;
    
    qcs->Analysis = ensAnalysisNewRef(analysis);
    
    qcs->QuerySequence  = ensQCSequenceNewRef(qsequence);
    qcs->TargetSequence = ensQCSequenceNewRef(tsequence);
    
    qcs->TargetStart = tstart;
    qcs->TargetEnd   = tend;
    
    qcs->TargetStrand = tstrand;
    
    qcs->AnalysisJobIdentifier = analysisjobid;
    
    return qcs;
}




/* @func ensQCSubmissionNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [EnsPQCSubmission] Ensembl QC Submission or NULL
** @@
******************************************************************************/

EnsPQCSubmission ensQCSubmissionNewObj(const EnsPQCSubmission object)
{
    EnsPQCSubmission qcs= NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(qcs);
    
    qcs->Use = 1;
    
    qcs->Identifier = object->Identifier;
    
    qcs->Adaptor = object->Adaptor;
    
    if(object->Analysis)
	qcs->Analysis = ensAnalysisNewRef(object->Analysis);
    
    qcs->QuerySequence = ensQCSequenceNewRef(object->QuerySequence);
    
    qcs->TargetSequence = ensQCSequenceNewRef(object->TargetSequence);
    
    qcs->TargetStart = object->TargetStart;
    
    qcs->TargetEnd = object->TargetEnd;
    
    qcs->TargetStrand = object->TargetStrand;
    
    qcs->AnalysisJobIdentifier = object->AnalysisJobIdentifier;
    
    return qcs;
}




/* @func ensQCSubmissionNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] qca [EnsPQCSubmission] Ensembl Submission
**
** @return [EnsPQCSubmission] Ensembl QC Submission
** @@
******************************************************************************/

EnsPQCSubmission ensQCSubmissionNewRef(EnsPQCSubmission qcs)
{
    if(!qcs)
	return NULL;
    
    qcs->Use++;
    
    return qcs;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Submission.
**
** @fdata [EnsPQCSubmission]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Submission object
**
** @argrule * Pqcs [EnsPQCSubmission*] QC Submission object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCSubmissionDel ***************************************************
**
** Default destructor for an Ensembl QC Submission.
**
** @param [d] Pqcs [EnsPQCSubmission*] Ensembl QC Submission address
**
** @return [void]
** @@
******************************************************************************/

void ensQCSubmissionDel(EnsPQCSubmission *Pqcs)
{
    EnsPQCSubmission pthis = NULL;
    
    /*
     ajDebug("ensQCSubmissionDel\n"
	     "  *Pqcs %p\n",
	     *Pqcs);
     
     ensQCSubmissionTrace(*Pqcs, 1);
     */
    
    if(!Pqcs)
	return;
    
    if(!*Pqcs)
	return;

    pthis = *Pqcs;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pqcs = NULL;
	
	return;
    }
    
    ensAnalysisDel(&pthis->Analysis);
    
    ensQCSequenceDel(&pthis->QuerySequence);
    
    ensQCSequenceDel(&pthis->TargetSequence);
    
    AJFREE(pthis);

    *Pqcs = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl QC Submission object.
**
** @fdata [EnsPQCSubmission]
** @fnote None
**
** @nam3rule Get Return QC Submission attribute(s)
** @nam4rule Getadaptor Return the Ensembl QC Submission Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetAnalysis Return the Ensembl Analysis
** @nam4rule GetQuerySequence Return the query Ensembl QC Sequence
** @nam4rule GetQueryStart Return the query start
** @nam4rule GetQueryEnd Return the query end
** @nam4rule GetQueryStrand Return the query strand
** @nam4rule GetTargetSequence Return the target Ensembl QC Sequence
** @nam4rule GetTargetStart Return the target start
** @nam4rule GetTargetEnd Return the target end
** @nam4rule GetTargetStrand Return the target strand
** @nam4rule GetAnalysisJobIdentifier Return the Hive Analysis Job Identifier
**
** @argrule * qcs [const EnsPQCSubmission] QC Submission
**
** @valrule Adaptor [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule QuerySequence [AjPStr] Query Ensembl QC Sequence
** @valrule QueryStart [ajuint] Query start
** @valrule QueryEnd [ajuint] Query end
** @valrule QueryStrand [ajint] Query strand
** @valrule TargetSequence [AjPStr] Target Ensembl QC Sequence
** @valrule TargetStart [ajuint] Target start
** @valrule TargetEnd [ajuint] Target end
** @valrule TargetStrand [ajint] Target strand
** @valrule AnalysisJobIdentifier [ajuint] Hive Analysis Job Identifier
**
** @fcategory use
******************************************************************************/




/* @func ensQCSubmissionGetadaptor ********************************************
**
** Get the Ensembl QC Submission Adaptor element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @@
******************************************************************************/

EnsPQCSubmissionadaptor ensQCSubmissionGetadaptor(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return NULL;
    
    return qcs->Adaptor;
}




/* @func ensQCSubmissionGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensQCSubmissionGetIdentifier(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->Identifier;
}




/* @func ensQCSubmissionGetAnalysis *******************************************
**
** Get the Ensembl Analysis element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [EnsPAnalysis] Ensembl Analysis
** @@
******************************************************************************/

EnsPAnalysis ensQCSubmissionGetAnalysis(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return NULL;
    
    return qcs->Analysis;
}




/* @func ensQCSubmissionGetQuerySequence **************************************
**
** Get the query Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [EnsPQCSequence] Query Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQCSequence ensQCSubmissionGetQuerySequence(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return NULL;
    
    return qcs->QuerySequence;
}




/* @func ensQCSubmissionGetQueryStart *****************************************
**
** Get the query start element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajuint] Query start
** @@
******************************************************************************/

ajuint ensQCSubmissionGetQueryStart(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->QueryStart;
}




/* @func ensQCSubmissionGetQueryEnd *******************************************
**
** Get the query end element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajuint] Query end
** @@
******************************************************************************/

ajuint ensQCSubmissionGetQueryEnd(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->QueryEnd;
}




/* @func ensQCSubmissionGetQueryStrand ****************************************
**
** Get the query strand element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajint] Query strand
** @@
******************************************************************************/

ajint ensQCSubmissionGetQueryStrand(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->QueryStrand;
}




/* @func ensQCSubmissionGetTargetSequence *************************************
**
** Get the target Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [EnsPQCSequence] Target Ensembl QC Sequence
** @@
******************************************************************************/

EnsPQCSequence ensQCSubmissionGetTargetSequence(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return NULL;
    
    return qcs->TargetSequence;
}




/* @func ensQCSubmissionGetTargetStart ****************************************
**
** Get the target start element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajuint] Target start
** @@
******************************************************************************/

ajuint ensQCSubmissionGetTargetStart(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->TargetStart;
}




/* @func ensQCSubmissionGetTargetEnd ******************************************
**
** Get the target end element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajuint] Target end
** @@
******************************************************************************/

ajuint ensQCSubmissionGetTargetEnd(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->TargetEnd;
}




/* @func ensQCSubmissionGetTargetStrand ***************************************
**
** Get the target strand element of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajint] Target strand
** @@
******************************************************************************/

ajint ensQCSubmissionGetTargetStrand(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->TargetStrand;
}




/* @func ensQCSubmissionGetAnalysisJobIdentifier ******************************
**
** Get the Hive Analysis Job identifier element of an Ensembl QC Variation.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Variation
**
** @return [ajuint] Hive Analysis Job identifier
** @@
******************************************************************************/

ajuint ensQCSubmissionGetAnalysisJobIdentifier(const EnsPQCSubmission qcs)
{
    if(!qcs)
	return 0;
    
    return qcs->AnalysisJobIdentifier;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl QC Submission object.
**
** @fdata [EnsPQCSubmission]
** @fnote None
**
** @nam3rule Set Set one element of a QC Submission
** @nam4rule Setadaptor Set the Ensembl QC Submission Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetQuerySequence Set the query Ensembl QC Sequence
** @nam4rule SetQueryStart Set the query start
** @nam4rule SetQueryEnd Set the query end
** @nam4rule SetQueryStrand Set the query strand
** @nam4rule SetTargetSequence Set the target Ensembl QC Sequence
** @nam4rule SetTargetStart Set the target start
** @nam4rule SetTargetEnd Set the target end
** @nam4rule SetTargetStrand Set the target strand
** @nam4rule SetAnalysisJobIdentifier Set the Analysis Job Identifier
**
** @argrule * qcs [EnsPQCSubmission] Ensembl QC Submission
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensQCSubmissionSetadaptor ********************************************
**
** Set the Ensembl Database Adaptor element of an QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetadaptor(EnsPQCSubmission qcs,
                                 EnsPQCSubmissionadaptor qcsa)
{
    if(!qcs)
	return ajFalse;
    
    qcs->Adaptor = qcsa;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetIdentifier(EnsPQCSubmission qcs, ajuint identifier)
{
    if(!qcs)
	return ajFalse;
    
    qcs->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetAnalysis *******************************************
**
** Set the Ensembl Analysis element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetAnalysis(EnsPQCSubmission qcs, EnsPAnalysis analysis)
{
    if(!qcs)
	return ajFalse;
    
    ensAnalysisDel(&qcs->Analysis);
    
    qcs->Analysis = ensAnalysisNewRef(analysis);
    
    return ajTrue;
}




/* @func ensQCSubmissionSetQuerySequence **************************************
**
** Set the query Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [u] qcseq [EnsPQCSequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetQuerySequence(EnsPQCSubmission qcs,
                                       EnsPQCSequence qcseq)
{
    if(!qcs)
	return ajFalse;
    
    ensQCSequenceDel(&qcs->QuerySequence);
    
    qcs->QuerySequence = ensQCSequenceNewRef(qcseq);
    
    return ajTrue;
}




/* @func ensQCSubmissionSetQueryStart *****************************************
**
** Set the query start element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [r] qstart [ajuint] Query start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetQueryStart(EnsPQCSubmission qcs, ajuint qstart)
{
    if(!qcs)
	return ajFalse;
    
    qcs->QueryStart = qstart;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetQueryEnd *******************************************
**
** Set the query end element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [r] qend [ajuint] Query end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetQueryEnd(EnsPQCSubmission qcs, ajuint qend)
{
    if(!qcs)
	return ajFalse;
    
    qcs->QueryEnd = qend;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetQueryStrand ****************************************
**
** Set the query strand element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [u] qstrand [ajint] Query strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetQueryStrand(EnsPQCSubmission qcs, ajint qstrand)
{
    if(!qcs)
	return ajFalse;
    
    qcs->QueryStrand = qstrand;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetTargetSequence *************************************
**
** Set the target Ensembl QC Sequence element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [u] qcseq [EnsPQCSequence] Ensembl QC Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetTargetSequence(EnsPQCSubmission qcs,
                                        EnsPQCSequence qcseq)
{
    if(!qcs)
	return ajFalse;
    
    ensQCSequenceDel(&qcs->TargetSequence);
    
    qcs->TargetSequence = ensQCSequenceNewRef(qcseq);
    
    return ajTrue;
}




/* @func ensQCSubmissionSetTargetStart ****************************************
**
** Set the target start element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [r] tstart [ajuint] Target start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetTargetStart(EnsPQCSubmission qcs, ajuint tstart)
{
    if(!qcs)
	return ajFalse;
    
    qcs->TargetStart = tstart;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetTargetEnd ******************************************
**
** Set the target end element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [r] tend [ajuint] Target end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetTargetEnd(EnsPQCSubmission qcs, ajuint tend)
{
    if(!qcs)
	return ajFalse;
    
    qcs->TargetEnd = tend;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetTargetStrand ***************************************
**
** Set the target strand element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [u] tstrand [ajint] Target strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetTargetStrand(EnsPQCSubmission qcs, ajint tstrand)
{
    if(!qcs)
	return ajFalse;
    
    qcs->TargetStrand = tstrand;
    
    return ajTrue;
}




/* @func ensQCSubmissionSetAnalysisJobIdentifier ******************************
**
** Set the Hive Analysis Job identifier element of an Ensembl QC Submission.
**
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
** @param [r] jobid [ajuint] Hive Analysis Job identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionSetAnalysisJobIdentifier(EnsPQCSubmission qcs,
                                               ajuint jobid)
{
    if(!qcs)
	return ajFalse;
    
    qcs->AnalysisJobIdentifier = jobid;
    
    return ajTrue;
}




/* @func ensQCSubmissionGetMemSize *********************************************
**
** Get the memory size in bytes of an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensQCSubmissionGetMemSize(const EnsPQCSubmission qcs)
{
    ajuint size = 0;
    
    if(!qcs)
	return 0;
    
    size += (ajuint) sizeof (EnsOQCSubmission);
    
    size += ensAnalysisGetMemSize(qcs->Analysis);
    
    size += ensQCSequenceGetMemSize(qcs->QuerySequence);
    size += ensQCSequenceGetMemSize(qcs->TargetSequence);
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl QC Submission object.
**
** @fdata [EnsPQCSubmission]
** @nam3rule Trace Report Ensembl QC Submission elements to debug file
**
** @argrule Trace qcs [const EnsPQCSubmission] Ensembl QC Submission
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensQCSubmissionTrace *************************************************
**
** Trace an Ensembl QC Submission.
**
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionTrace(const EnsPQCSubmission qcs, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!qcs)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensQCSubmissionTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Analysis %p\n"
	    "%S  QuerySequence %p\n"
	    /*
	     "%S  QueryStart %u\n"
	     "%S  QueryEnd %u\n"
	     "%S  QueryStrand %d\n"
	     */
	    "%S  TargetSequence %p\n"
	    "%S  TargetStart %u\n"
	    "%S  TargetEnd %u\n"
	    "%S  TargetStrand %d\n"
	    "%S  AnalysisJobIdentifier %u\n",
	    indent, qcs,
	    indent, qcs->Use,
	    indent, qcs->Identifier,
	    indent, qcs->Adaptor,
	    indent, qcs->Analysis,
	    indent, qcs->QuerySequence,
	    /*
	     indent, qcs->QueryStart,
	     indent, qcs->QueryEnd,
	     indent, qcs->QueryStrand,
	     */
	    indent, qcs->TargetSequence,
	    indent, qcs->TargetStart,
	    indent, qcs->TargetEnd,
	    indent, qcs->TargetStrand,
	    indent, qcs->AnalysisJobIdentifier);
    
    ensAnalysisTrace(qcs->Analysis, 1);
    
    ensQCSequenceTrace(qcs->QuerySequence, 1);
    ensQCSequenceTrace(qcs->TargetSequence, 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPQCSubmissionadaptor] QC Submission Adaptor ***************
**
** Functions for manipulating Ensembl QC Submission Adaptor objects
**
** Bio::EnsEMBL::QC::DBSQL::Submissionadaptor CVS Revision:
**
** @nam2rule QCSubmissionadaptor
**
******************************************************************************/

static const char *qcSubmissionadaptorTables[] =
{
    "submission",
    (const char *) NULL
};




static const char *qcSubmissionadaptorColumns[] =
{
    "submission.submission_id",
    "submission.analysis_id",
    "submission.query_db_id",
    "submission.query_id"
    "submission.target_db_id"
    "submission.target_id",
    "submission.target_start",
    "submission.target_end",
    "submission.target_strand",
    "submission.analysis_job_id",
    (const char *) NULL
};




static EnsOBaseadaptorLeftJoin qcSubmissionadaptorLeftJoin[] =
{
    {(const char*) NULL, (const char*) NULL}
};




static const char *qcSubmissionadaptorDefaultCondition =
(const char*) NULL;




static const char *qcSubmissionadaptorFinalCondition =
(const char *) NULL;




/* @funcstatic qcSubmissionadaptorFetchAllBySQL *******************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl QC Submission objects.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] qcss [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcSubmissionadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList qcss)
{
    ajint tstrand = 0;
    
    ajuint identifier = 0;
    ajuint analysisid = 0;
    ajuint qdbid      = 0;
    ajuint qsid       = 0;
    ajuint tdbid      = 0;
    ajuint tsid       = 0;
    ajuint tstart     = 0;
    ajuint tend       = 0;

    ajuint analysisjobid = 0;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;
    
    EnsPQCSequence qsequence   = NULL;
    EnsPQCSequence tsequence   = NULL;
    EnsPQCSequenceadaptor qcsa = NULL;
    
    EnsPQCSubmission qcb         = NULL;
    EnsPQCSubmissionadaptor qcba = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!qcss)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisadaptor(dba);
    
    qcsa = ensRegistryGetQCSequenceadaptor(dba);
    
    qcba = ensRegistryGetQCSubmissionadaptor(dba);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier    = 0;
	analysisid    = 0;
	qdbid         = 0;
	qsid          = 0;
	tdbid         = 0;
	tsid          = 0;
	tstart        = 0;
	tend          = 0;
	tstrand       = 0;
	analysisjobid = 0;
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &analysisid);
	ajSqlcolumnToUint(sqlr, &qdbid);
	ajSqlcolumnToUint(sqlr, &qsid);
	ajSqlcolumnToUint(sqlr, &tdbid);
	ajSqlcolumnToUint(sqlr, &tsid);
	ajSqlcolumnToUint(sqlr, &tstart);
	ajSqlcolumnToUint(sqlr, &tend);
	ajSqlcolumnToInt(sqlr, &tstrand);
	ajSqlcolumnToUint(sqlr, &analysisjobid);
	
	ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	ensQCSequenceadaptorFetchByIdentifier(qcsa, qsid, &qsequence);
	
	ensQCSequenceadaptorFetchByIdentifier(qcsa, tsid, &tsequence);
	
	qcb = ensQCSubmissionNew(qcba,
				 identifier,
				 analysis,
				 qsequence,
				 tsequence,
				 tstart,
				 tend,
				 tstrand,
				 analysisjobid);
	
	ajListPushAppend(qcss, (void *) qcb);
	
	ensAnalysisDel(&analysis);
	
	ensQCSequenceDel(&qsequence);
	ensQCSequenceDel(&tsequence);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Submission Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Submission Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCSubmissionadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @argrule Ref object [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
**
** @valrule * [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensQCSubmissionadaptorNew ********************************************
**
** Default constructor for an Ensembl QC Submission Adaptor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor or NULL
** @@
******************************************************************************/

EnsPQCSubmissionadaptor ensQCSubmissionadaptorNew(EnsPDatabaseadaptor dba)
{
    if(!dba)
	return NULL;
    
    return ensBaseadaptorNew(dba,
			     qcSubmissionadaptorTables,
			     qcSubmissionadaptorColumns,
			     qcSubmissionadaptorLeftJoin,
			     qcSubmissionadaptorDefaultCondition,
			     qcSubmissionadaptorFinalCondition,
			     qcSubmissionadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Submission Adaptor.
**
** @fdata [EnsPQCSubmissionadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Submission Adaptor object
**
** @argrule * Pqcsa [EnsPQCSubmissionadaptor*] QC Submission Adaptor
**                                             object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCSubmissionadaptorDel ********************************************
**
** Default destructor for an Ensembl QC Submission Adaptor.
**
** @param [d] Pqcsa [EnsPQCSubmissionadaptor*] Ensembl QC Submission Adaptor
**                                             address
**
** @return [void]
** @@
******************************************************************************/

void ensQCSubmissionadaptorDel(EnsPQCSubmissionadaptor* Pqcsa)
{
    /*
     ajDebug("ensQCSubmissionadaptorDel\n"
	     "  *Pqcsa %p\n",
	     *Pqcsa);
     */
    
    if(!Pqcsa)
	return;
    
    if(!*Pqcsa)
	return;
    
    ensBaseadaptorDel(Pqcsa);
    
    return;
}




/* @func ensQCSubmissionadaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl QC Submission via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl QC Submission.
**
** @param [r] adaptor [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] identifier [ajuint] SQL database-internal QC Submission
**                                identifier
** @param [wP] Pqcs [EnsPQCSubmission*] Ensembl QC Submission address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorFetchByIdentifier(EnsPQCSubmissionadaptor adaptor,
                                               ajuint identifier,
                                               EnsPQCSubmission *Pqcs)
{
    if(!adaptor)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pqcs)
	return ajFalse;
    
    *Pqcs = (EnsPQCSubmission)
	ensBaseadaptorFetchByIdentifier(adaptor, identifier);
    
    return ajTrue;
}




/* @func ensQCSubmissionadaptorFetchAllByAnalysisQueryTarget ******************
**
** Fetch all Ensembl QC Submissions by an Ensembl Analysis, as well as
** Query and Target Ensembl Sequences.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQCDatabase] Query Ensembl QC Database
** @param [r] tdb [const EnsPQCDatabase] Target Ensembl QC Database
** @param [w] qcss [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorFetchAllByAnalysisQueryTarget(
    EnsPQCSubmissionadaptor qcsa,
    const EnsPAnalysis analysis,
    const EnsPQCDatabase qdb,
    const EnsPQCDatabase tdb,
    AjPList qcss)
{
    AjPStr constraint = NULL;
    
    if(!qcsa)
	return ajFalse;
    
    if(!analysis)
	return ajFalse;
    
    if(!qdb)
	return ajFalse;
    
    if(!tdb)
	return ajFalse;
    
    if(!qcss)
	return ajFalse;
    
    constraint = ajFmtStr("submission.analysis_id = %u "
			  "AND "
			  "submission.query_db_id = %u "
			  "AND "
			  "submission.target_db_id = %u",
			  ensAnalysisGetIdentifier(analysis),
			  ensQCDatabaseGetIdentifier(qdb),
			  ensQCDatabaseGetIdentifier(tdb));
    
    ensBaseadaptorGenericFetch(qcsa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcss);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCSubmissionadaptorFetchAllByANQIDTDB *****************************
**
** Fetch all Ensembl QC Submissions by an Ensembl Analysis, as well as
** Query and Target Ensembl Sequences.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qsequence [const EnsPQCSequence] Query Ensembl QC Sequence
** @param [r] tdb [const EnsPQCDatabase] Target Ensembl QC Database
** @param [r] tsequence [const EnsPQCSequence] Target Ensembl QC Sequence
** @param [r] tstart [ajuint] Target start
** @param [r] tend [ajuint] Target end
** @param [r] tstrand [ajint] Target strand
** @param [w] qcss [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorFetchAllByANQIDTDB(EnsPQCSubmissionadaptor qcsa,
                                                const EnsPAnalysis analysis,
                                                const EnsPQCSequence qsequence,
                                                const EnsPQCDatabase tdb,
                                                const EnsPQCSequence tsequence,
                                                ajuint tstart,
                                                ajuint tend,
                                                ajint tstrand,
                                                AjPList qcss)
{
    AjPStr constraint = NULL;
    
    if(!qcsa)
	return ajFalse;
    
    if(!analysis)
	return ajFalse;
    
    if(!qsequence)
	return ajFalse;
    
    if(!tdb)
	return ajFalse;
    
    if(!qcss)
	return ajFalse;
    
    constraint = ajFmtStr("submission.analysis_id = %u "
			  "AND "
			  "submission.query_db_id = %u "
			  "AND "
			  "submission.target_db_id = %u "
			  "AND "
			  "submission.query_id = %u",
			  ensAnalysisGetIdentifier(analysis),
			  ensQCSequenceGetQCDatabaseIdentifier(qsequence),
			  ensQCDatabaseGetIdentifier(tdb),
			  ensQCSequenceGetIdentifier(qsequence));
    
    if(tsequence)
	ajFmtPrintAppS(&constraint,
		       " AND submission.target_id = %u",
		       ensQCSequenceGetIdentifier(tsequence));
    
    if(tstart && tend)
	ajFmtPrintAppS(&constraint,
		       " AND"
		       " submission.target_start >= %u"
		       " AND"
		       " submission.target_end <= %u"
		       " AND"
		       " submission.target_strand = %d",
		       tstart,
		       tend,
		       tstrand);
    
    ensBaseadaptorGenericFetch(qcsa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcss);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCSubmissionadaptorFetchAllByQuery ********************************
**
** Fetch all Ensembl QC Submissions by a Query Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] qdb [const EnsPQCDatabase] Query Ensembl QC Database
** @param [w] qcss [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorFetchAllByQuery(EnsPQCSubmissionadaptor qcsa,
                                             const EnsPAnalysis analysis,
                                             const EnsPQCDatabase qdb,
                                             AjPList qcss)
{
    AjPStr constraint = NULL;
    
    if(!qcsa)
	return ajFalse;
    
    if(!qdb)
	return ajFalse;
    
    if(!qcss)
	return ajFalse;
    
    constraint = ajFmtStr("submission.query_db_id = %u",
			  ensQCDatabaseGetIdentifier(qdb));
    
    if(analysis)
	ajFmtPrintAppS(&constraint,
		       " AND submission.analysis_id = %u",
		       ensAnalysisGetIdentifier(analysis));
    
    ensBaseadaptorGenericFetch(qcsa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcss);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCSubmissionadaptorFetchAllByTarget *******************************
**
** Fetch all Ensembl QC Submissions by a Target Ensembl QC Sequence.
** The caller is responsible for deleting the Ensembl QC Submissions
** before deleting the AJAX List.
**
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [rN] analysis [const EnsPAnalysis] Ensembl Analysis
** @param [r] tdb [const EnsPQCDatabase] Target Ensembl QC Database
** @param [w] qcss [AjPList] AJAX List of Ensembl QC Submissions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorFetchAllByTarget(EnsPQCSubmissionadaptor qcsa,
                                              const EnsPAnalysis analysis,
                                              const EnsPQCDatabase tdb,
                                              AjPList qcss)
{
    AjPStr constraint = NULL;
    
    if(!qcsa)
	return ajFalse;
    
    if(!tdb)
	return ajFalse;
    
    if(!qcss)
	return ajFalse;
    
    constraint = ajFmtStr("submission.target_db_id = %u",
			  ensQCDatabaseGetIdentifier(tdb));
    
    if(analysis)
	ajFmtPrintAppS(&constraint,
		       " AND submission.analysis_id = %u",
		       ensAnalysisGetIdentifier(analysis));
    
    ensBaseadaptorGenericFetch(qcsa,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcss);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCSubmissionadaptorStore ******************************************
**
** Store an Ensembl QC Submission.
**
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [u] qcs [EnsPQCSubmission] Ensembl QC Submission
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorStore(EnsPQCSubmissionadaptor qcsa,
                                   EnsPQCSubmission qcs)
{
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcsa)
	return ajFalse;
    
    if(!qcs)
	return ajFalse;
    
    if(ensQCSubmissionGetadaptor(qcs) && ensQCSubmissionGetIdentifier(qcs))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcsa);
    
    statement = ajFmtStr("INSERT IGNORE INTO "
			 "submission "
			 "SET "
			 "submission.analysis_id = %u, "
			 "submission.query_db_id = %u, "
			 "submission.query_id = %u, "
			 "submission.target_db_id = %u, "
			 "submission.target_id = %u, "
			 "submission.target_start = %u, "
			 "submission.target_end = %u, "
			 "submission.target_strand = %d, "
			 "submission.analysis_job_id = %u",
			 ensAnalysisGetIdentifier(qcs->Analysis),
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcs->QuerySequence),
			 ensQCSequenceGetIdentifier(qcs->QuerySequence),
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcs->TargetSequence),
			 ensQCSequenceGetIdentifier(qcs->TargetSequence),
			 qcs->TargetStart,
			 qcs->TargetEnd,
			 qcs->TargetStrand,
			 qcs->AnalysisJobIdentifier);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
    {
	ensQCSubmissionSetIdentifier(qcs, ajSqlstatementGetIdentifier(sqls));
	
	ensQCSubmissionSetadaptor(qcs, qcsa);
	
	value = ajTrue;
    }
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCSubmissionadaptorUpdate *****************************************
**
** Update an Ensembl QC Submission.
**
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorUpdate(EnsPQCSubmissionadaptor qcsa,
                                    const EnsPQCSubmission qcs)
{
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcsa)
	return ajFalse;
    
    if(!qcs)
	return ajFalse;
    
    if(!ensQCSubmissionGetIdentifier(qcs))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcsa);
    
    statement = ajFmtStr("UPDATE IGNORE "
			 "submission "
			 "SET "
			 "submission.analysis_id = %u, "
			 "submission.query_db_id = %u, "
			 "submission.query_id = %u, "
			 "submission.target_db_id = %u, "
			 "submission.target_id = %u, "
			 "submission.target_start = %u, "
			 "submission.target_end = %u, "
			 "submission.target_strand = %d, "
			 "submission.analysis_job_id = %u "
			 "WHERE "
			 "submission.submission_id = %u",
			 ensAnalysisGetIdentifier(qcs->Analysis),
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcs->QuerySequence),
			 ensQCSequenceGetIdentifier(qcs->QuerySequence),
			 ensQCSequenceGetQCDatabaseIdentifier(
                             qcs->TargetSequence),
			 ensQCSequenceGetIdentifier(qcs->TargetSequence),
			 qcs->TargetStart,
			 qcs->TargetEnd,
			 qcs->TargetStrand,
			 qcs->AnalysisJobIdentifier);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
	value = ajTrue;
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCSubmissionadaptorDelete *****************************************
**
** Delete an Ensembl QC Submission.
**
** @param [r] qcsa [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @param [r] qcs [const EnsPQCSubmission] Ensembl QC Submission
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCSubmissionadaptorDelete(EnsPQCSubmissionadaptor qcsa,
                                    const EnsPQCSubmission qcs)
{
    AjBool value = ajFalse;
    
    AjPSqlstatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!qcsa)
	return ajFalse;
    
    if(!qcs)
	return ajFalse;
    
    if(!ensQCSubmissionGetIdentifier(qcs))
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(qcsa);
    
    statement = ajFmtStr("DELETE FROM "
			 "submission "
			 "WHERE "
			 "submission.submission_id = %u",
			 qcs->Identifier);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    if(ajSqlstatementGetAffectedrows(sqls))
	value = ajTrue;
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}
