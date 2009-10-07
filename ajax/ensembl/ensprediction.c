/******************************************************************************
** @source Ensembl Prediction functions.
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.3 $
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
******************************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensprediction.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisadaptor
ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba);

extern EnsPAssemblyMapperadaptor
ensRegistryGetAssemblyMapperadaptor(EnsPDatabaseadaptor dba);

extern EnsPCoordSystemadaptor
ensRegistryGetCoordSystemadaptor(EnsPDatabaseadaptor dba);

extern EnsPPredictionExonadaptor
ensRegistryGetPredictionExonadaptor(EnsPDatabaseadaptor dba);

extern EnsPPredictionTranscriptadaptor
ensRegistryGetPredictionTranscriptadaptor(EnsPDatabaseadaptor dba);

extern EnsPSliceadaptor
ensRegistryGetSliceadaptor(EnsPDatabaseadaptor dba);

static AjBool predictionExonadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                 const AjPStr statement,
                                                 EnsPAssemblyMapper am,
                                                 EnsPSlice slice,
                                                 AjPList pes);

static void *predictionExonadaptorCacheReference(void *value);

static void predictionExonadaptorCacheDelete(void **value);

static ajuint predictionExonadaptorCacheSize(const void *value);

static EnsPFeature predictionExonadaptorGetFeature(const void *value);

static AjBool predictionTranscriptadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                       const AjPStr statement,
                                                       EnsPAssemblyMapper am,
                                                       EnsPSlice slice,
                                                       AjPList pts);

static void *predictionTranscriptadaptorCacheReference(void *value);

static void predictionTranscriptadaptorCacheDelete(void **value);

static ajuint predictionTranscriptadaptorCacheSize(const void *value);

static EnsPFeature predictionTranscriptadaptorGetFeature(const void *value);

    


/* @filesection ensprediction *************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPPredictionExon] Prediction Exon **************************
**
** Functions for manipulating Ensembl Prediction Exon objects
**
** @cc Bio::EnsEMBL::PredictionExon CVS Revision: 1.6
**
** @nam2rule PredictionExon
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Prediction Exon by pointer.
** It is the responsibility of the user to first destroy any previous
** Prediction Exon. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPPredictionExon]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPPredictionExon] Ensembl Prediction Exon
** @argrule Ref object [EnsPPredictionExon] Ensembl Prediction Exon
**
** @valrule * [EnsPPredictionExon] Ensembl Prediction Exon
**
** @fcategory new
******************************************************************************/




/* @func ensPredictionExonNew *************************************************
**
** Default Ensembl Prediction Exon constructor.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPPredictionExonadaptor] Ensembl Prediction
**                                                Exon Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Feature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::PredictionExon::new
** @param [r] sphase [ajint] Start phase of Translation
** @param [r] score [double] Score
** @param [r] pvalue [double] P-value
**
** @return [EnsPPredictionExon] Ensembl Prediction Exon or NULL
** @@
******************************************************************************/

EnsPPredictionExon ensPredictionExonNew(EnsPPredictionExonadaptor adaptor,
                                        ajuint identifier,
                                        EnsPFeature feature,
                                        ajint sphase,
                                        double score,
                                        double pvalue)
{
    EnsPPredictionExon pe = NULL;
    
    if(!feature)
        return NULL;
    
    AJNEW0(pe);
    
    pe->Use = 1;
    
    pe->Identifier = identifier;
    
    pe->Adaptor = adaptor;
    
    pe->Feature = ensFeatureNewRef(feature);
    
    pe->StartPhase = sphase;
    
    pe->Score = score;
    
    pe->Pvalue = pvalue;
    
    return pe;
}




/* @func ensPredictionExonNewObj **********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [EnsPPredictionExon] Ensembl Prediction Exon or NULL
** @@
******************************************************************************/

EnsPPredictionExon ensPredictionExonNewObj(const EnsPPredictionExon object)
{
    EnsPPredictionExon pe = NULL;
    
    AJNEW0(pe);
    
    pe->Use = 1;
    
    pe->Identifier = object->Identifier;
    
    pe->Adaptor = object->Adaptor;
    
    pe->Feature = ensFeatureNewRef(object->Feature);
    
    pe->StartPhase = object->StartPhase;
    
    pe->Score = object->Score;
    
    pe->Pvalue = object->Pvalue;
    
    return pe;
}




/* @func ensPredictionExonNewRef **********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [EnsPPredictionExon] Ensembl Prediction Exon or NULL
** @@
******************************************************************************/

EnsPPredictionExon ensPredictionExonNewRef(EnsPPredictionExon pe)
{
    if(!pe)
	return NULL;
    
    pe->Use++;
    
    return pe;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Prediction Exon.
**
** @fdata [EnsPPredictionExon]
** @fnote None
**
** @nam3rule Del Destroy (free) a Prediction Exon object
**
** @argrule * Ppe [EnsPPredictionExon*] Ensembl Prediction Exon object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensPredictionExonDel *************************************************
**
** Default destructor for an Ensembl Prediction Exon.
**
** @param [d] Ppe [EnsPPredictionExon*] Ensembl Prediction Exon address
**
** @return [void]
** @@
******************************************************************************/

void ensPredictionExonDel(EnsPPredictionExon *Ppe)
{
    EnsPPredictionExon pthis = NULL;
    
    if(!Ppe)
        return;
    
    if(!*Ppe)
        return;

    pthis = *Ppe;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Ppe = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    AJFREE(pthis);

    *Ppe = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Prediction Exon object.
**
** @fdata [EnsPPredictionExon]
** @fnote None
**
** @nam3rule Get Return Prediction Exon attribute(s)
** @nam4rule Getadaptor Return the Ensembl Prediction Exon Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetStartPhase Return the start phase
** @nam4rule GetScore Return the score
** @nam4rule GetPvalue Return the p-value
**
** @argrule * pe [const EnsPPredictionExon] Prediction Exon
**
** @valrule Adaptor [EnsPPredictionExonadaptor] Ensembl Prediction Exon Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule StartPhase [ajint] Start phase
** @valrule Score [double] Score
** @valrule Pvalue [double] P-value
**
** @fcategory use
******************************************************************************/




/* @func ensPredictionExonGetadaptor ******************************************
**
** Get the Ensembl Prediction Exon Adaptor element of an
** Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [EnsPPredictionExonadaptor] Ensembl Prediction Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionExonadaptor ensPredictionExonGetadaptor(
    const EnsPPredictionExon pe)
{
    if(!pe)
	return NULL;
    
    return pe->Adaptor;
}




/* @func ensPredictionExonGetIdentifier ***************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensPredictionExonGetIdentifier(const EnsPPredictionExon pe)
{
    if(!pe)
	return 0;
    
    return pe->Identifier;
}




/* @func ensPredictionExonGetFeature ******************************************
**
** Get the Ensembl Feature element of an Ensembl Prediction Exon.
**
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensPredictionExonGetFeature(const EnsPPredictionExon pe)
{
    if(!pe)
	return NULL;
    
    return pe->Feature;
}




/* @func ensPredictionExonGetStartPhase ***************************************
**
** Get the start phase element of an Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::PredictionExon::phase
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [ajint] Start phase
** @@
**
** Get or set the phase of the Exon, which tells the translation machinery,
** which makes a peptide from the DNA, where to start.
**
** The Ensembl phase convention can be thought of as "the number of bases of
** the first codon which are on the previous exon". It is therefore 0, 1 or 2
** or -1 if the exon is non-coding. In ASCII art, with alternate codons
** represented by '###' and '+++':
**
**       Previous Exon   Intron   This Exon
**    ...-------------            -------------...
**
**    5'                    Phase                3'
**    ...#+++###+++###          0 +++###+++###+...
**    ...+++###+++###+          1 ++###+++###++...
**    ...++###+++###++          2 +###+++###+++...
**
** Here is another explanation from Ewan:
**
** Phase means the place where the intron lands inside the codon - 0 between
** codons, 1 between the 1st and second base, 2 between the second and 3rd
** base. Exons therefore have a start phase and an end phase, but introns have
** just one phase.
******************************************************************************/

ajint ensPredictionExonGetStartPhase(const EnsPPredictionExon pe)
{
    if(!pe)
        return 0;
    
    return pe->StartPhase;
}




/* @func ensPredictionExonGetScore ********************************************
**
** Get the score element of an Ensembl Prediction Exon.
**
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [double] Score
** @@
******************************************************************************/

double ensPredictionExonGetScore(const EnsPPredictionExon pe)
{
    if(!pe)
	return 0.0;
    
    return pe->Score;
}




/* @func ensPredictionExonGetPvalue *******************************************
**
** Get the p-value element of an Ensembl Prediction Exon.
**
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [double] P-value
** @@
******************************************************************************/

double ensPredictionExonGetPvalue(const EnsPPredictionExon pe)
{
    if(!pe)
	return 0.0;
    
    return pe->Pvalue;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Prediction Exon object.
**
** @fdata [EnsPPredictionExon]
** @fnote None
**
** @nam3rule Set Set one element of a Prediction Exon
** @nam4rule Setadaptor Set the Ensembl Prediction Exon Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetStartPhase Set the start phase
** @nam4rule SetScore Set the score
** @nam4rule SetPvalue Set the p-value
**
** @argrule * pe [EnsPPredictionExon] Ensembl Prediction Exon object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensPrediction<ExonSetadaptor *****************************************
**
** Set the Ensembl Prediction Exon Adaptor element of an
** Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [r] adaptor [EnsPPredictionExonadaptor] Ensembl Prediction
**                                                Exon Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonSetadaptor(EnsPPredictionExon pe,
                                   EnsPPredictionExonadaptor adaptor)
{
    if(!pe)
        return ajFalse;
    
    pe->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensPredictionExonSetIdentifier ***************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonSetIdentifier(EnsPPredictionExon pe, ajuint identifier)
{
    if(!pe)
        return ajFalse;
    
    pe->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensPredictionExonSetFeature ******************************************
**
** Set the Ensembl Feature element of an Ensembl Prediction Exon.
**
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonSetFeature(EnsPPredictionExon pe, EnsPFeature feature)
{
#if AJFALSE
    AjIList iter = NULL;
    
    EnsPBaseAlignFeature baf  = NULL;
    EnsPBaseAlignFeature nbaf = NULL;
    
    EnsPSlice eslice = NULL;
#endif

    /*
     ajDebug("ensPredictionExonSetFeature\n"
	     "  pe %p\n"
	     "  feature %p\n",
	     pe,
	     feature);
     
     ensPredictionExonTrace(pe, 1);
     
     ensFeatureTrace(feature, 1);
     */
    
    if(!pe)
	return ajFalse;
    
    if(!feature)
	return ajFalse;
    
    /* Replace the current Feature. */
    
    if(pe->Feature)
	ensFeatureDel(&(pe->Feature));
    
    pe->Feature = ensFeatureNewRef(feature);
    
#if AJFALSE
    
    /* TODO: Resolve this! */
    
    /* Clear the sequence cache. */
    
    ajStrDel(&(pe->SequenceCache));
    
    /* Clear the Exon coordinate cache. */
    
    exonCoordinatesClear(exon);
    
    /*
     ** FIXME: Setting a Feature, which may be based on another Slice requires
     ** transferring of Supporting Features to the new Feature Slice.
     ** The Perl API does not implement this, it only deletes the Sequence
     ** cache, but it should.
     */
    
    /* Transfer Base Align Features onto the new Feature Slice. */
    
    if(!exon->SupportingFeatures)
	return ajTrue;
    
    eslice = ensFeatureGetSlice(exon->Feature);
    
    iter = ajListIterNew(exon->SupportingFeatures);
    
    while(!ajListIterDone(iter))
    {
	baf = (EnsPBaseAlignFeature) ajListIterGet(iter);
	
	ajListIterRemove(iter);
	
	nbaf = ensBaseAlignFeatureTransfer(baf, eslice);
	
	if(!nbaf)
	{
	    ajDebug("ensExonSetFeature could not transfer Base Align Feature "
		    "onto new Ensembl Feature Slice.");
	    
	    ensBaseAlignFeatureTrace(baf, 1);
	}
	
	ajListIterInsert(iter, (void *) nbaf);
	
	/* Advance the AJAX List Iterator after the insert. */
	
	(void) ajListIterGet(iter);
	
	ensBaseAlignFeatureDel(&baf);
    }
    
    ajListIterDel(&iter);
    
#endif
    
    return ajTrue;
}




/* @func ensPredictionExonSetStartPhase ***************************************
**
** Set the start phase element of an Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::PredictionExon::phase
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [r] sphase [ajint] Start phase
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonSetStartPhase(EnsPPredictionExon pe, ajint sphase)
{
    if(!pe)
        return ajFalse;
    
    pe->StartPhase = sphase;
    
    return ajTrue;
}




/* @func ensPredictionExonSetScore ********************************************
**
** Set the score element of an Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::PredictionExon::score
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [r] score [double] Score
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonSetScore(EnsPPredictionExon pe, double score)
{
    if(!pe)
        return ajFalse;
    
    pe->Score = score;
    
    return ajTrue;
}




/* @func ensPredictionExonSetPvalue *******************************************
**
** Set the p-value element of an Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::PredictionExon::p_value
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [r] pvalue [double] P-value
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonSetPvalue(EnsPPredictionExon pe, double pvalue)
{
    if(!pe)
        return ajFalse;
    
    pe->Pvalue = pvalue;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Prediction Exon object.
**
** @fdata [EnsPPredictionExon]
** @nam3rule Trace Report Ensembl Prediction Exon elements to debug file
**
** @argrule Trace pe [const EnsPPredictionExon] Ensembl Prediction Exon
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensPredictionExonTrace ***********************************************
**
** Trace an Ensembl Prediction Exon.
**
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonTrace(const EnsPPredictionExon pe, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!pe)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensPredictionExonTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  StartPhase %d\n"
	    "%S  Score %f\n"
	    "%S  P-value %f\n",
	    indent, pe,
	    indent, pe->Use,
	    indent, pe->Identifier,
	    indent, pe->Adaptor,
	    indent, pe->Feature,
	    indent, pe->StartPhase,
	    indent, pe->Score,
	    indent, pe->Pvalue);
    
    ensFeatureTrace(pe->Feature, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensPredictionExonGetMemSize ******************************************
**
** Get the memory size in bytes of an Ensembl Prediction Exon.
**
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensPredictionExonGetMemSize(const EnsPPredictionExon pe)
{
    ajuint size = 0;
    
    if(!pe)
	return 0;
    
    size += (ajuint) sizeof (EnsOPredictionExon);
    
    size += ensFeatureGetMemSize(pe->Feature);
    
    return size;
}




/* @func ensPredictionExonGetEndPhase *****************************************
**
** Get the end phase element of an Ensembl Prediction Exon.
**
** @cc Bio::EnsEMBL::PredictionExon::end_phase
** @param [r] pe [const EnsPPredictionExon] Ensembl Prediction Exon
**
** @return [ajint] End phase
** @@
******************************************************************************/

ajint ensPredictionExonGetEndPhase(const EnsPPredictionExon pe)
{
    if(!pe)
        return 0;
    
    return (pe->StartPhase + ensFeatureGetLength(pe->Feature)) % 3;
}




/* @func ensPredictionExonTransform *******************************************
**
** Transform an Ensembl Prediction Exon into another Ensembl Coordinate System.
**
** @cc Bio::EnsEMBL::PredictionExon::transform
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [r] csname [const AjPStr] Ensembl Coordinate System name
** @param [r] csversion [const AjPStr] Ensembl Coordinate System version
** @see ensFeatureTransform
**
** @return [EnsPPredictionExon] Ensembl Prediction Exon in the defined
**                              Ensembl Coordinate System or NULL
** @@
******************************************************************************/

EnsPPredictionExon ensPredictionExonTransform(EnsPPredictionExon pe,
                                              const AjPStr csname,
                                              const AjPStr csversion)
{
    EnsPPredictionExon npe = NULL;
    EnsPFeature nfeature   = NULL;
    
    if(!pe)
	return NULL;
    
    if(!csname)
	return NULL;
    
    if(!csversion)
	return NULL;
    
    nfeature = ensFeatureTransform(pe->Feature, csname, csversion);
    
    if(!nfeature)
	return NULL;
    
    npe = ensPredictionExonNewObj(pe);
    
    ensPredictionExonSetFeature(npe, nfeature);
    
    /*
    ** NOTE: The Exon-internal supporting features have already been
    ** transfered to the new Slice and the sequence cache has already been
    ** cleared by the ensExonSetFeature function.
    */
    
    /*
    ** FIXME: Clear the SequenceCache, which does not exist yet, in
    ** ensPredictionExonSetFeature.
    */
    
    return npe;
}




/* @func ensPredictionExonTransfer ********************************************
**
** Transfer an Ensembl Prediction Exon onto another Ensembl Slice.
**
** @cc Bio::EnsEMBL::PredictionExon::transfer
** @param [u] pe [EnsPPredictionExon] Ensembl Prediction Exon
** @param [u] slice [EnsPSlice] Ensembl Slice
** @see ensFeatureTransfer
**
** @return [EnsPPredictionExon] Ensembl Prediction Exon on the defined
**                              Ensembl Slice or NULL
** @@
******************************************************************************/

EnsPPredictionExon ensPredictionExonTransfer(EnsPPredictionExon pe,
                                             EnsPSlice slice)
{
    EnsPPredictionExon npe = NULL;
    EnsPFeature newfeature = NULL;
    
    if(!pe)
	return NULL;
    
    if(!slice)
	return NULL;
    
    newfeature = ensFeatureTransfer(pe->Feature, slice);
    
    if(!newfeature)
	return NULL;
    
    npe = ensPredictionExonNewObj(pe);
    
    ensPredictionExonSetFeature(npe, newfeature);
    
    ensFeatureDel(&newfeature);
    
    return npe;
}




/* @datasection [EnsPPredictionExonadaptor] Prediction Exon Adaptor ***********
**
** Functions for manipulating Ensembl Prediction Exon Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::PredictionExonadaptor CVS Revision: 1.16
**
** @nam2rule PredictionExonadaptor
**
******************************************************************************/

static const char *predictionExonadaptorTables[] =
{
    "prediction_exon",
    NULL
};




static const char *predictionExonadaptorColumns[] =
{
    "prediction_exon.prediction_exon_id",
    "prediction_exon.seq_region_id",
    "prediction_exon.seq_region_start",
    "prediction_exon.seq_region_end",
    "prediction_exon.seq_region_strand",
    "prediction_exon.start_phase",
    "prediction_exon.score",
    "prediction_exon.p_value",
    NULL
};




static EnsOBaseadaptorLeftJoin predictionExonadaptorLeftJoin[] =
{
    {NULL, NULL}
};




static const char *predictionExonadaptorDefaultCondition = NULL;

static const char *predictionExonadaptorFinalCondition =
"ORDER BY "
"prediction_exon.prediction_transcript_id, prediction_exon.exon_rank";




/* @funcstatic predictionExonadaptorFetchAllBySQL *****************************
**
** Fetch all Ensembl Prediction Exon objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] pes [AjPList] AJAX List of Ensembl Prediction Exon objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool predictionExonadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                 const AjPStr statement,
                                                 EnsPAssemblyMapper am,
                                                 EnsPSlice slice,
                                                 AjPList pes)
{
    double score  = 0.0;
    double pvalue = 0.0;
    
    ajint sphase = 0;
    /* ajint ephase = 0; */ /* unused */
    
    ajuint identifier = 0;
    
    ajint slstart  = 0;
    ajint slend    = 0;
    ajint slstrand = 0;
    ajint sllength = 0;
    
    ajuint srid    = 0;
    ajuint srstart = 0;
    ajuint srend   = 0;
    ajint srstrand = 0;
    
    AjPList mrs = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    EnsPAssemblyMapperadaptor ama = NULL;
    
    EnsPCoordSystemadaptor csa = NULL;
    
    EnsPPredictionExon pe         = NULL;
    EnsPPredictionExonadaptor pea = NULL;
    
    EnsPFeature feature = NULL;
    EnsPMapperResult mr = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceadaptor sa = NULL;
    
    /*
     ajDebug("predictionExonadaptorFetchAllBySQL\n"
	     "  dba %p\n"
	     "  statement %p\n"
	     "  am %p\n"
	     "  slice %p\n"
	     "  pes %p\n",
	     dba,
	     statement,
	     am,
	     slice,
	     pes);
     */
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!pes)
	return ajFalse;
    
    csa = ensRegistryGetCoordSystemadaptor(dba);
    
    pea = ensRegistryGetPredictionExonadaptor(dba);
    
    sa = ensRegistryGetSliceadaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperadaptor(dba);
    
    mrs = ajListNew();
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	srid       = 0;
	srstart    = 0;
	srend      = 0;
	srstrand   = 0;
	sphase     = 0;
	score      = 0.0;
	pvalue     = 0.0;
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToUint(sqlr, &srid);
	ajSqlcolumnToUint(sqlr, &srstart);
	ajSqlcolumnToUint(sqlr, &srend);
	ajSqlcolumnToInt(sqlr, &srstrand);
	ajSqlcolumnToInt(sqlr, &sphase);
	ajSqlcolumnToDouble(sqlr, &score);
        ajSqlcolumnToDouble(sqlr, &pvalue);
	
	/* Need to get the internal Ensembl Sequence Region identifier. */
	
	srid = ensCoordSystemadaptorGetInternalSeqRegionIdentifier(csa, srid);
	
	/*
	** Since the Ensembl SQL schema defines Sequence Region start and end
	** coordinates as unsigned integers for all Features, the range needs
	** checking.
	*/
	
	if(srstart <= INT_MAX)
	    slstart = (ajint) srstart;
	else
	    ajFatal("predictionExonadaptorFetchAllBySQL got a "
		    "Sequence Region start coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srstart, INT_MAX);
	
	if(srend <= INT_MAX)
	    slend = (ajint) srend;
	else
	    ajFatal("predictionExonadaptorFetchAllBySQL got a "
		    "Sequence Region end coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srend, INT_MAX);
	
	slstrand = srstrand;
	
	/* Fetch a Slice spanning the entire Sequence Region. */
	
	ensSliceadaptorFetchBySeqRegionIdentifier(sa, srid, 0, 0, 0, &srslice);
	
	/*
	** Increase the reference counter of the Ensembl Assembly Mapper if
	** one has been specified, otherwise fetch it from the database if a
	** destination Slice has been specified.
	*/
	
	if(am)
	    am = ensAssemblyMapperNewRef(am);
	
	if((!am) &&
	    slice &&
	    (!ensCoordSystemMatch(ensSliceGetCoordSystem(slice),
				   ensSliceGetCoordSystem(srslice))))
	    am = ensAssemblyMapperadaptorFetchByCoordSystems(
                ama,
                ensSliceGetCoordSystem(slice),
                ensSliceGetCoordSystem(srslice));
	
	/*
        ** FIXME: Should we have an ensAsemblyMapperadaptorFetchBySlices
        ** function?
        */
	
	/*
	** Remap the Feature coordinates to another Ensembl Coordinate System
	** if an Ensembl Assembly Mapper is defined at this point.
	*/
	
	if(am)
	{
	    ensAssemblyMapperFastMap(am,
				     ensSliceGetSeqRegion(srslice),
				     slstart,
				     slend,
				     slstrand,
				     mrs);
	    
	    /*
	    ** The ensAssemblyMapperFastMap function returns at best one
	    ** Ensembl Mapper Result.
	    */
	    
	    ajListPop(mrs, (void **) &mr);
	    
	    /*
	    ** Skip Features that map to gaps or
	    ** Coordinate System boundaries.
	    */
	    
	    if(ensMapperResultGetType(mr) != ensEMapperResultCoordinate)
	    {
		/* Load the next Feature but destroy first! */
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		ensMapperResultDel(&mr);
		
		continue;
	    }
	    
	    srid = ensMapperResultGetObjectIdentifier(mr);
	    
	    slstart = ensMapperResultGetStart(mr);
	    
	    slend = ensMapperResultGetEnd(mr);
	    
	    slstrand = ensMapperResultGetStrand(mr);
	    
	    /*
	    ** Delete the Sequence Region Slice and fetch a Slice in the
	    ** Coordinate System we just mapped to.
	    */
	    
	    ensSliceDel(&srslice);
	    
	    ensSliceadaptorFetchBySeqRegionIdentifier(sa,
						      srid,
						      0,
						      0,
						      0,
						      &srslice);
	    
	    ensMapperResultDel(&mr);
	}
	
	/*
	** Convert Sequence Region Slice coordinates to destination Slice
	** coordinates, if a destination Slice has been provided.
	*/
	
	if(slice)
	{
	    /* Check that the length of the Slice is within range. */
	    
	    if(ensSliceGetLength(slice) <= INT_MAX)
		sllength = (ajint) ensSliceGetLength(slice);
	    else
		ajFatal("exonAdaptorFetchAllBySQL got a Slice, "
			"which length (%u) exceeds the "
			"maximum integer limit (%d).",
			ensSliceGetLength(slice), INT_MAX);
	    
	    /*
	    ** Nothing needs to be done if the destination Slice starts at 1
	    ** and is on the forward strand.
	    */
	    
	    if((ensSliceGetStart(slice) != 1) ||
		(ensSliceGetStrand(slice) < 0))
	    {
		if(ensSliceGetStrand(slice) >= 0)
		{
		    slstart = slstart - ensSliceGetStart(slice) + 1;
		    
		    slend = slend - ensSliceGetStart(slice) + 1;
		}
		else
		{
		    slend = ensSliceGetEnd(slice) - slstart + 1;
		    
		    slstart = ensSliceGetEnd(slice) - slend + 1;
		    
		    slstrand *= -1;
		}
	    }
	    
	    /*
	    ** Throw away Features off the end of the requested Slice or on
	    ** any other than the requested Slice.
	    */
	    
	    if((slend < 1) ||
		(slstart > sllength) ||
		(srid != ensSliceGetSeqRegionIdentifier(slice)))
	    {
		/* Load the next Feature but destroy first! */
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		continue;
	    }
	    
	    /* Delete the Sequence Region Slice and set the requested Slice. */
	    
	    ensSliceDel(&srslice);
	    
	    srslice = ensSliceNewRef(slice);
	}
	
	/* Finally, create a new Ensembl Exon. */
	
	feature = ensFeatureNewS((EnsPAnalysis) NULL,
				 srslice,
				 slstart,
				 slend,
				 slstrand);
	
	pe = ensPredictionExonNew(pea,
				  identifier,
				  feature,
				  sphase,
				  score,
				  pvalue);
	
	ajListPushAppend(pes, (void *) pe);
	
	ensFeatureDel(&feature);
	
	ensAssemblyMapperDel(&am);
	
	ensSliceDel(&srslice);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajListFree(&mrs);
    
    return ajTrue;
}




/* @funcstatic predictionExonadaptorCacheReference ****************************
**
** Wrapper function to reference an Ensembl Prediction Exon from an
** Ensembl Cache.
**
** @param [r] value [void *] Ensembl Prediction Exon
**
** @return [void *] Ensembl Prediction Exon or NULL
** @@
******************************************************************************/

static void *predictionExonadaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *) ensPredictionExonNewRef((EnsPPredictionExon) value);
}




/* @funcstatic predictionExonadaptorCacheDelete *******************************
**
** Wrapper function to delete an Ensembl Prediction Exon from an Ensembl Cache.
**
** @param [r] value [void**] Ensembl Prediction Exon address
**
** @return [void]
** @@
******************************************************************************/

static void predictionExonadaptorCacheDelete(void **value)
{
    if(!value)
	return;
    
    ensPredictionExonDel((EnsPPredictionExon *) value);
    
    return;
}




/* @funcstatic predictionExonadaptorCacheSize *********************************
**
** Wrapper function to determine the memory size of an Ensembl Prediction Exon
** from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Prediction Exon
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint predictionExonadaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensPredictionExonGetMemSize((const EnsPPredictionExon) value);
}




/* @funcstatic predictionExonadaptorGetFeature ********************************
**
** Wrapper function to get the Ensembl Feature of an Ensembl Prediction Exon
** from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl Prediction Exon
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature predictionExonadaptorGetFeature(const void *value)
{
    if(!value)
	return NULL;
    
    return ensPredictionExonGetFeature((const EnsPPredictionExon) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Prediction Exon Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Prediction Exon Adaptor. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPPredictionExonadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPPredictionExonadaptor] Ensembl Prediction
**                                                 Exon Adaptor
** @argrule Ref object [EnsPPredictionExonadaptor] Ensembl Prediction
**                                                 Exon Adaptor
**
** @valrule * [EnsPPredictionExonadaptor] Ensembl Prediction Exon Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensPredictionExonadaptorNew ******************************************
**
** Default Ensembl Prediction Exon Adaptor constructor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictionExonadaptor] Ensembl Prediction Exon Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionExonadaptor ensPredictionExonadaptorNew(EnsPDatabaseadaptor dba)
{
    return ensFeatureadaptorNew(dba,
				predictionExonadaptorTables,
				predictionExonadaptorColumns,
				predictionExonadaptorLeftJoin,
				predictionExonadaptorDefaultCondition,
				predictionExonadaptorFinalCondition,
				predictionExonadaptorFetchAllBySQL,
                                 /* Fread */
				(void * (*)(const void *key)) NULL,
				predictionExonadaptorCacheReference,
                                 /* Fwrite */
				(AjBool (*)(const void* value)) NULL,
				predictionExonadaptorCacheDelete,
				predictionExonadaptorCacheSize,
				predictionExonadaptorGetFeature,
				"PredictionExon");
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Prediction Exon Adaptor.
**
** @fdata [EnsPPredictionExonadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Prediction Exon Adaptor object
**
** @argrule * Padaptor [EnsPPredictionExonadaptor*] Ensembl Prediction
**                                                  Exon Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensPredictionExonadaptorDel ******************************************
**
** Default destructor for an Ensembl Prediction Exon Adaptor.
**
** @param [d] Padaptor [EnsPPredictionExonadaptor*] Ensembl Prediction
**                                                  Exon Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensPredictionExonadaptorDel(EnsPPredictionExonadaptor *Padaptor)
{
    if(!Padaptor)
	return;
    
    ensFeatureadaptorDel(Padaptor);
    
    return;
}




/* @func ensPredictionExonadaptorFetchAllByPredictionTranscript ***************
**
** Fetch all Ensembl Prediction Exons via an Ensembl Prediction Transcript.
**
** The caller is responsible for deleting the Ensembl Prediction Exons before
** deleting the AJAX List.
**
** @param [r] pea [EnsPPredictionExonadaptor] Ensembl Prediction Exon Adaptor
** @param [r] pt [const EnsPPredictionTranscript] Ensembl Prediction Transcript
** @param [u] pes [AjPList] AJAX List of Ensembl Prediction Exons
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionExonadaptorFetchAllByPredictionTranscript(
    EnsPPredictionExonadaptor pea,
    const EnsPPredictionTranscript pt,
    AjPList pes)
{
    AjIList iter = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    EnsPPredictionExon pe = NULL;
    
    EnsPFeature efeature = NULL;
    EnsPFeature tfeature = NULL;
    
    EnsPSlice eslice = NULL;
    EnsPSlice tslice = NULL;

    EnsPSliceadaptor sa = NULL;
    
    /*
     ajDebug("ensPredictionExonadaptorFetchAllByPredictionTranscript\n"
	     "  pea %p\n"
	     "  pt %p\n"
	     "  pes %p\n",
	     pea,
	     pt,
	     pes);
     
     ensPredictionTranscriptTrace(pt, 1);
     */
    
    if(!pea)
	return ajFalse;
    
    if(!pt)
	return ajFalse;
    
    if(!pes)
	return ajFalse;
    
    tfeature = ensPredictionTranscriptGetFeature(pt);
    
    tslice = ensFeatureGetSlice(tfeature);
    
    if(!tslice)
    {
	ajDebug("ensPredictionExonadaptorFetchAllByPredictionTranscript "
		"cannot fetch Prediction Exons for Prediction Transcript "
		"without Slice.\n");
	
	return ajFalse;
    }
    
    dba = ensFeatureadaptorGetDatabaseadaptor(pea);
    
    sa = ensRegistryGetSliceadaptor(dba);
    
    /*
    ** Get a Slice that spans just this Prediction Transcript to place
    ** Prediction Exons on them.
    */
    
    ensSliceadaptorFetchByFeature(sa, tfeature, 0, &eslice);
    
    constraint =
	ajFmtStr("prediction_exon.prediction_transcript_id = %u",
		 ensPredictionTranscriptGetIdentifier(pt));
    
    ensFeatureadaptorFetchAllBySliceConstraint(pea,
					       eslice,
					       constraint,
					       (const AjPStr) NULL,
					       pes);
    
    /* Remap Exon coordinates if neccessary. */
    
    /*
    ** FIXME: What is the point of retrieving Exons first on a Slice that
    ** spans the Transcript only and then map them to the Slice the Transcript
    ** is on? Could this have something to do with haplotypes or PAR regions?
    */
    
    if(!ensSliceMatch(eslice, tslice))
    {
	iter = ajListIterNew(pes);
	
	while(!ajListIterDone(iter))
	{
	    pe = (EnsPPredictionExon) ajListIterGet(iter);
	    
	    efeature = ensFeatureTransfer(pe->Feature, tslice);
	    
	    ensPredictionExonSetFeature(pe, efeature);
	    
	    ensFeatureDel(&efeature);
	}
	
	ajListIterDel(&iter);	
    }
    
    ajStrDel(&constraint);
    
    ensSliceDel(&eslice);
    
    return ajTrue;
}




/* @datasection [EnsPPredictionTranscript] Prediction Transcript **************
**
** Functions for manipulating Ensembl Prediction Transcript objects
**
** @cc Bio::EnsEMBL::PredictionTranscript CVS Revision: 1.43
**
** @nam2rule PredictionTranscript
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Prediction Transcript by pointer.
** It is the responsibility of the user to first destroy any previous
** Prediction Transcript. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPPredictionTranscript]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPPredictionTranscript] Ensembl Prediction Transcript
** @argrule Ref object [EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @valrule * [EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @fcategory new
******************************************************************************/




/* @func ensPredictionTranscriptNew *******************************************
**
** Default Ensembl Prediction Transcript constructor.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                      Transcript Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Feature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::PredictionTranscript::new
** @param [u] label [AjPStr] Display label
**
** @return [EnsPPredictionTranscript] Ensembl Prediction Transcript or NULL
** @@
******************************************************************************/

EnsPPredictionTranscript ensPredictionTranscriptNew(
    EnsPPredictionTranscriptadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    AjPStr label)
{
    EnsPPredictionTranscript pt = NULL;
    
    if(!feature)
        return NULL;
    
    AJNEW0(pt);
    
    pt->Use = 1;
    
    pt->Identifier = identifier;
    
    pt->Adaptor = adaptor;
    
    pt->Feature = ensFeatureNewRef(feature);
    
    if(label)
	pt->DisplayLabel = ajStrNewRef(label);
    
    return pt;
}




/* @func ensPredictionTranscriptNewObj ****************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPPredictionTranscript] Ensembl Prediction
**                                                    Transcript
**
** @return [EnsPPredictionTranscript] Ensembl Prediction Transcript or NULL
** @@
******************************************************************************/

EnsPPredictionTranscript ensPredictionTranscriptNewObj(
    const EnsPPredictionTranscript object)
{
    AjIList iter = NULL;
    
    EnsPPredictionExon pe = NULL;
    
    EnsPPredictionTranscript pt = NULL;
    
    AJNEW0(pt);
    
    pt->Use = 1;
    
    pt->Identifier = object->Identifier;
    
    pt->Adaptor = object->Adaptor;
    
    pt->Feature = ensFeatureNewRef(object->Feature);
    
    if(object->DisplayLabel)
	pt->DisplayLabel = ajStrNewRef(object->DisplayLabel);
    
    /* Copy the AJAX List of Ensembl Prediction Exons. */
    
    if(object->PredictionExons && ajListGetLength(object->PredictionExons))
    {
	pt->PredictionExons = ajListNew();
	
	iter = ajListIterNew(object->PredictionExons);
	
	while(!ajListIterDone(iter))
	{
	    pe = (EnsPPredictionExon) ajListIterGet(iter);
	    
	    ajListPushAppend(pt->PredictionExons,
			     (void *) ensPredictionExonNewRef(pe));
	}
	
	ajListIterDel(&iter);
    }
    else
	pt->PredictionExons = NULL;
    
    return pt;
}




/* @func ensPredictionTranscriptNewRef ****************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] pt [EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @return [EnsPPredictionTranscript] Ensembl Prediction Transcript or NULL
** @@
******************************************************************************/

EnsPPredictionTranscript ensPredictionTranscriptNewRef(
    EnsPPredictionTranscript pt)
{
    if(!pt)
	return NULL;
    
    pt->Use++;
    
    return pt;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Prediction Transcript.
**
** @fdata [EnsPPredictionTranscript]
** @fnote None
**
** @nam3rule Del Destroy (free) a Prediction Transcript object
**
** @argrule * Ppt [EnsPPredictionTranscript*] Ensembl Prediction Transcript
**                                            object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensPredictionTranscriptDel *******************************************
**
** Default destructor for an Ensembl Prediction Transcript.
**
** @param [d] Ppt [EnsPPredictionTranscript*] Ensembl Prediction Transcript
**                                            address
**
** @return [void]
** @@
******************************************************************************/

void ensPredictionTranscriptDel(EnsPPredictionTranscript *Ppt)
{
    EnsPPredictionExon pe = NULL;
    EnsPPredictionTranscript pthis = NULL;
    
    if(!Ppt)
        return;
    
    if(!*Ppt)
        return;

    pthis = *Ppt;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Ppt = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    ajStrDel(&pthis->DisplayLabel);
    
    /* Clear and delete the AJAX List of Ensembl Prediction Exons. */
    
    while(ajListPop(pthis->PredictionExons, (void **) &pe))
	ensPredictionExonDel(&pe);
    
    ajListFree(&pthis->PredictionExons);
    
    AJFREE(pthis);

    *Ppt = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Prediction Transcript object.
**
** @fdata [EnsPPredictionTranscript]
** @fnote None
**
** @nam3rule Get Return Prediction Transcript attribute(s)
** @nam4rule Getadaptor Return the Ensembl Prediction Transcript Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetDisplayLabel Return the display label
**
** @argrule * pt [const EnsPPredictionTranscript] Prediction Transcript
**
** @valrule Adaptor [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                    Transcript Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule DisplayLabel [AjPStr] Display label
**
** @fcategory use
******************************************************************************/




/* @func ensPredictionTranscriptGetadaptor ************************************
**
** Get the Ensembl Prediction Transcript Adaptor element of an
** Ensembl Prediction Transcript.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] pt [const EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @return [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                           Transcript Adaptor
** @@
******************************************************************************/

EnsPPredictionTranscriptadaptor ensPredictionTranscriptGetadaptor(
    const EnsPPredictionTranscript pt)
{
    if(!pt)
        return NULL;
    
    return pt->Adaptor;
}




/* @func ensPredictionTranscriptGetIdentifier *********************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Prediction Transcript.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] pt [const EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensPredictionTranscriptGetIdentifier(const EnsPPredictionTranscript pt)
{
    if(!pt)
        return 0;
    
    return pt->Identifier;
}




/* @func ensPredictionTranscriptGetFeature ************************************
**
** Get the Ensembl Feature element of an Ensembl Prediction Transcript.
**
** @param [r] pt [const EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensPredictionTranscriptGetFeature(const EnsPPredictionTranscript pt)
{
    if(!pt)
        return NULL;
    
    return pt->Feature;
}




/* @func ensPredictionTranscriptGetDisplayLabel *******************************
**
** Get the display label element of an Ensembl Prediction Transcript.
**
** @cc Bio::EnsEMBL::PredictionTranscript::display_label
** @param [r] pt [const EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @return [AjPStr] Display label
** @@
******************************************************************************/

AjPStr ensPredictionTranscriptGetDisplayLabel(const EnsPPredictionTranscript pt)
{
    if(!pt)
        return NULL;
    
    return pt->DisplayLabel;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Prediction Transcript object.
**
** @fdata [EnsPPredictionTranscript]
** @fnote None
**
** @nam3rule Set Set one element of a Prediction Transcript
** @nam4rule Setadaptor Set the Ensembl Prediction Transcript Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetDisplayLabel Set the display label
**
** @argrule * pt [EnsPPredictionTranscript] Ensembl Prediction Transcript
**                                          object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensPredictionTranscriptSetadaptor ************************************
**
** Set the Ensembl Prediction Transcript Adaptor element of an
** Ensembl Prediction Transcript.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] pt [EnsPPredictionTranscript] Ensembl Prediction Transcript
** @param [r] adaptor [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                      Transcript Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionTranscriptSetadaptor(
    EnsPPredictionTranscript pt,
    EnsPPredictionTranscriptadaptor adaptor)
{
    if(!pt)
        return ajFalse;
    
    pt->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensPredictionTranscriptSetIdentifier *********************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Prediction Transcript.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] pt [EnsPPredictionTranscript] Ensembl Prediction Transcript
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionTranscriptSetIdentifier(EnsPPredictionTranscript pt,
                                            ajuint identifier)
{
    if(!pt)
        return ajFalse;
    
    pt->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensPredictionTranscriptSetFeature ************************************
**
** Set the Ensembl Feature element of an Ensembl Prediction Transcript.
**
** @param [u] pt [EnsPPredictionTranscript] Ensembl Prediction Transcript
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionTranscriptSetFeature(EnsPPredictionTranscript pt,
                                         EnsPFeature feature)
{
    if(!pt)
        return ajFalse;
    
    ensFeatureDel(&pt->Feature);
    
    pt->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensPredictionTranscriptSetDisplayLabel *******************************
**
** Set the display label element of an Ensembl Prediction Transcript.
**
** @cc Bio::EnsEMBL::PredictionTranscript::display_label
** @param [u] pt [EnsPPredictionTranscript] Ensembl Prediction Transcript
** @param [u] label [AjPStr] Display label
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionTranscriptSetDisplayLabel(EnsPPredictionTranscript pt,
                                              AjPStr label)
{
    if(!pt)
        return ajFalse;
    
    ajStrDel(&pt->DisplayLabel);
    
    pt->DisplayLabel = ajStrNewRef(label);
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Prediction Transcript object.
**
** @fdata [EnsPPredictionTranscript]
** @nam3rule Trace Report Ensembl Prediction Transcript elements to debug file
**
** @argrule Trace pt [const EnsPPredictionTranscript] Ensembl Prediction
**                                                    Transcript
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensPredictionTranscriptTrace *****************************************
**
** Trace an Ensembl Prediction Transcript.
**
** @param [r] pt [const EnsPPredictionTranscript] Ensembl Prediction Transcript
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionTranscriptTrace(const EnsPPredictionTranscript pt,
                                    ajuint level)
{
    AjIList iter = NULL;
    
    AjPStr indent = NULL;
    
    EnsPPredictionExon pe = NULL;
    
    if(!pt)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensPredictionTranscriptTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  DisplayLabel '%S'\n"
	    "%S  PredictionExons %p\n",
	    indent, pt,
	    indent, pt->Use,
	    indent, pt->Identifier,
	    indent, pt->Adaptor,
	    indent, pt->Feature,
	    indent, pt->DisplayLabel,
	    indent, pt->PredictionExons);
    
    ensFeatureTrace(pt->Feature, level + 1);
    
    /* Trace the AJAX List of Ensembl Prediction Exons. */
    
    if(pt->PredictionExons)
    {
	ajDebug("%S    AJAX List %p of Ensembl Prediction Exons\n",
		indent, pt->PredictionExons);
	
	iter = ajListIterNewread(pt->PredictionExons);
	
	while(!ajListIterDone(iter))
	{
	    pe = (EnsPPredictionExon) ajListIterGet(iter);
	    
	    ensPredictionExonTrace(pe, level + 2);
	}
	
	ajListIterDel(&iter);
    }
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensPredictionTranscriptGetMemSize ************************************
**
** Get the memory size in bytes of an Ensembl Prediction Transcript.
**
** @param [r] pt [const EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensPredictionTranscriptGetMemSize(const EnsPPredictionTranscript pt)
{
    ajuint size = 0;
    
    AjIList iter = NULL;
    
    EnsPPredictionExon pe = NULL;
    
    if(!pt)
	return 0;
    
    size += (ajuint) sizeof (EnsOPredictionTranscript);
    
    size += ensFeatureGetMemSize(pt->Feature);
    
    if(pt->DisplayLabel)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(pt->DisplayLabel);
    }
    
    /* Summarise the AJAX List of Ensembl Prediction Exons. */
    
    if(pt->PredictionExons)
    {
	size += (ajuint) sizeof (AjOList);
	
	iter = ajListIterNewread(pt->PredictionExons);
	
	while(!ajListIterDone(iter))
	{
	    pe = (EnsPPredictionExon) ajListIterGet(iter);
	    
	    size += ensPredictionExonGetMemSize(pe);
	}
	
	ajListIterDel(&iter);
    }
    
    return size;
}




/* @func ensPredictionTranscriptGetPredictionExons ****************************
**
** Get all Ensembl Prediction Exons of an Ensembl Prediction Transcript.
**
** This is not a simple accessor function, since it will attempt fetching the
** Prediction Exons from the Ensembl Core database associated with the
** Prediction Transcript Adaptor.
**
** @cc Bio::EnsEMBL::PredictionTranscript::get_all_Exons
** @param [u] pt [EnsPPredictionTranscript] Ensembl Prediction Transcript
**
** @return [const AjPList] AJAX List of Ensembl Prediction Exons
** @@
******************************************************************************/

const AjPList ensPredictionTranscriptGetExons(EnsPPredictionTranscript pt)
{
    EnsPDatabaseadaptor dba = NULL;
    
    EnsPPredictionExonadaptor pea = NULL;
    
    if(!pt)
	return NULL;
    
    if(pt->PredictionExons)
	return pt->PredictionExons;
    
    if(!pt->Adaptor)
    {
	ajDebug("ensPredictionTranscriptGetPredictionExons cannot fetch "
		"Ensembl Prediction Exons for a Prediction Transcript without "
		"a Prediction Transcript Adaptor.\n");
	
	return NULL;
    }
    
    dba = ensFeatureadaptorGetDatabaseadaptor(pt->Adaptor);
    
    pea = ensRegistryGetPredictionExonadaptor(dba);
    
    pt->PredictionExons = ajListNew();
    
    ensPredictionExonadaptorFetchAllByPredictionTranscript(pea,
							   pt,
							   pt->PredictionExons);
    
    return pt->PredictionExons;
}




/* TODO: Some Prediction Transcript methods are missing! */

/* @datasection [EnsPPredictionTranscriptadaptor] Prediction Transcript Adaptor
**
** Functions for manipulating Ensembl Prediction Transcript Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::PredictionTranscriptadaptor CVS Revision: 1.48
**
** @nam2rule PredictionTranscriptadaptor
**
******************************************************************************/

static const char *predictionTranscriptadaptorTables[] =
{
    "prediction_transcript",
    NULL
};




static const char *predictionTranscriptadaptorColumns[] =
{
    "prediction_transcript.prediction_transcript_id",
    "prediction_transcript.seq_region_id",
    "prediction_transcript.seq_region_start",
    "prediction_transcript.seq_region_end",
    "prediction_transcript.seq_region_strand",
    "prediction_transcript.analysis_id",
    "prediction_transcript.display_label",
    NULL
};




static EnsOBaseadaptorLeftJoin predictionTranscriptadaptorLeftJoin[] =
{
    {NULL, NULL}
};




static const char *predictionTranscriptadaptorDefaultCondition = NULL;

static const char *predictionTranscriptadaptorFinalCondition = NULL;




/* @funcstatic predictionTranscriptadaptorFetchAllBySQL ***********************
**
** Fetch all Ensembl Prediction Transcript objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] pts [AjPList] AJAX List of Ensembl Prediction Transcripts
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool predictionTranscriptadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                       const AjPStr statement,
                                                       EnsPAssemblyMapper am,
                                                       EnsPSlice slice,
                                                       AjPList pts)
{
    ajuint identifier = 0;
    ajuint analysisid = 0;
    
    ajuint srid    = 0;
    ajuint srstart = 0;
    ajuint srend   = 0;
    ajint srstrand = 0;
    
    ajint slstart  = 0;
    ajint slend    = 0;
    ajint slstrand = 0;
    ajint sllength = 0;
    
    AjPList mrs = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr label = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;
    
    EnsPAssemblyMapperadaptor ama = NULL;
    
    EnsPCoordSystemadaptor csa = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPPredictionTranscript pt = NULL;
    EnsPPredictionTranscriptadaptor pta = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceadaptor sa = NULL;
    
    /*
     ajDebug("predictionTranscriptadaptorFetchAllBySQL\n"
	     "  dba %p\n"
	     "  statement %p\n"
	     "  am %p\n"
	     "  slice %p\n"
	     "  pts %p\n",
	     dba,
	     statement,
	     am,
	     slice,
	     pts);
     */
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!pts)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisadaptor(dba);
    
    csa = ensRegistryGetCoordSystemadaptor(dba);
    
    sa = ensRegistryGetSliceadaptor(dba);
    
    pta = ensRegistryGetPredictionTranscriptadaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperadaptor(dba);
    
    mrs = ajListNew();
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	srid       = 0;
	srstart    = 0;
	srend      = 0;
	srstrand   = 0;
	analysisid = 0;
	label      = ajStrNew();
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToUint(sqlr, &srid);
	ajSqlcolumnToUint(sqlr, &srstart);
	ajSqlcolumnToUint(sqlr, &srend);
	ajSqlcolumnToInt(sqlr, &srstrand);
	ajSqlcolumnToUint(sqlr, &analysisid);
	ajSqlcolumnToStr(sqlr, &label);
	
	/* Need to get the internal Ensembl Sequence Region identifier. */
	
	srid = ensCoordSystemadaptorGetInternalSeqRegionIdentifier(csa, srid);
	
	/*
	** Since the Ensembl SQL schema defines Sequence Region start and end
	** coordinates as unsigned integers for all Features, the range needs
	** checking.
	*/
	
	if(srstart <= INT_MAX)
	    slstart = (ajint) srstart;
	else
	    ajFatal("predictionTranscriptadaptorFetchAllBySQL got a "
		    "Sequence Region start coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srstart, INT_MAX);
	
	if(srend <= INT_MAX)
	    slend = (ajint) srend;
	else
	    ajFatal("predictionTranscriptadaptorFetchAllBySQL got a "
		    "Sequence Region end coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srend, INT_MAX);
	
	slstrand = srstrand;
	
	/* Fetch a Slice spanning the entire Sequence Region. */
	
	ensSliceadaptorFetchBySeqRegionIdentifier(sa, srid, 0, 0, 0, &srslice);
	
	/*
	** Increase the reference counter of the Ensembl Assembly Mapper if
	** one has been specified, otherwise fetch it from the database if a
	** destination Slice has been specified.
	*/
	
	if(am)
	    am = ensAssemblyMapperNewRef(am);
	
	if((!am) &&
	    slice &&
	    (!ensCoordSystemMatch(ensSliceGetCoordSystem(slice),
				   ensSliceGetCoordSystem(srslice))))
	    am =
		ensAssemblyMapperadaptorFetchByCoordSystems(
                    ama,
                    ensSliceGetCoordSystem(slice),
                    ensSliceGetCoordSystem(srslice));
	
	/*
	** Remap the Feature coordinates to another Ensembl Coordinate System
	** if an Ensembl Assembly Mapper is defined at this point.
	*/
	
	if(am)
	{
	    ensAssemblyMapperFastMap(am,
				     ensSliceGetSeqRegion(srslice),
				     slstart,
				     slend,
				     slstrand,
				     mrs);
	    
	    /*
	    ** The ensAssemblyMapperFastMap function returns at best one
	    ** Ensembl Mapper Result.
	    */
	    
	    ajListPop(mrs, (void **) &mr);
	    
	    /*
	    ** Skip Features that map to gaps or
	    ** Coordinate System boundaries.
	    */
	    
	    if(ensMapperResultGetType(mr) != ensEMapperResultCoordinate)
	    {
		/* Load the next Feature but destroy first! */
		
		ajStrDel(&label);
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		ensMapperResultDel(&mr);
		
		continue;
	    }
	    
	    srid = ensMapperResultGetObjectIdentifier(mr);
	    
	    slstart = ensMapperResultGetStart(mr);
	    
	    slend = ensMapperResultGetEnd(mr);
	    
	    slstrand = ensMapperResultGetStrand(mr);
	    
	    /*
	    ** Delete the Sequence Region Slice and fetch a Slice in the
	    ** Coordinate System we just mapped to.
	    */
	    
	    ensSliceDel(&srslice);
	    
	    ensSliceadaptorFetchBySeqRegionIdentifier(sa,
						      srid,
						      0,
						      0,
						      0,
						      &srslice);
	    
	    ensMapperResultDel(&mr);
	}
	
	/*
	** Convert Sequence Region Slice coordinates to destination Slice
	** coordinates, if a destination Slice has been provided.
	*/
	
	if(slice)
	{
	    /* Check that the length of the Slice is within range. */
	    
	    if(ensSliceGetLength(slice) <= INT_MAX)
		sllength = (ajint) ensSliceGetLength(slice);
	    else
		ajFatal("transcriptAdaptorFetchAllBySQL got a Slice, "
			"which length (%u) exceeds the "
			"maximum integer limit (%d).",
			ensSliceGetLength(slice), INT_MAX);
	    
	    /*
	    ** Nothing needs to be done if the destination Slice starts at 1
	    ** and is on the forward strand.
	    */
	    
	    if((ensSliceGetStart(slice) != 1) ||
		(ensSliceGetStrand(slice) < 0))
	    {
		if(ensSliceGetStrand(slice) >= 0)
		{
		    slstart = slstart - ensSliceGetStart(slice) + 1;
		    
		    slend = slend - ensSliceGetStart(slice) + 1;
		}
		else
		{
		    slend = ensSliceGetEnd(slice) - slstart + 1;
		    
		    slstart = ensSliceGetEnd(slice) - slend + 1;
		    
		    slstrand *= -1;
		}
	    }
	    
	    /*
	    ** Throw away Features off the end of the requested Slice or on
	    ** any other than the requested Slice.
	    */
	    
	    if((slend < 1) ||
		(slstart > sllength) ||
		(srid != ensSliceGetSeqRegionIdentifier(slice)))
	    {
		/* Next feature but destroy first! */
		
		ajStrDel(&label);
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		continue;
	    }
	    
	    /* Delete the Sequence Region Slice and set the requested Slice. */
	    
	    ensSliceDel(&srslice);
	    
	    srslice = ensSliceNewRef(slice);
	}
	
	ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	/* Finally, create a new Ensembl Prediction Transcript. */
	
	feature = ensFeatureNewS(analysis,
				 srslice,
				 slstart,
				 slend,
				 slstrand);
	
	pt = ensPredictionTranscriptNew(pta,
					identifier,
					feature,
					label);
	
	ajListPushAppend(pts, (void *) pt);	
	
	ensFeatureDel(&feature);
	
	ajStrDel(&label);
	
	ensAnalysisDel(&analysis);
	
	ensSliceDel(&srslice);
	
	ensAssemblyMapperDel(&am);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajListFree(&mrs);
    
    return ajTrue;
}




/* @funcstatic predictionTranscriptadaptorCacheReference **********************
**
** Wrapper function to reference an Ensembl Prediction Transcript
** from an Ensembl Cache.
**
** @param [r] value [void *] Ensembl Prediction Transcript
**
** @return [void *] Ensembl Transcript or NULL
** @@
******************************************************************************/

static void *predictionTranscriptadaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *) ensPredictionTranscriptNewRef((EnsPPredictionTranscript)
						  value);
}




/* @funcstatic predictionTranscriptadaptorCacheDelete *************************
**
** Wrapper function to delete an Ensembl Prediction Transcript
** from an Ensembl Cache.
**
** @param [r] value [void**] Ensembl Prediction Transcript address
**
** @return [void]
** @@
******************************************************************************/

static void predictionTranscriptadaptorCacheDelete(void **value)
{
    if(!value)
	return;
    
    ensPredictionTranscriptDel((EnsPPredictionTranscript *) value);
    
    return;
}




/* @funcstatic predictionTranscriptadaptorCacheSize ***************************
**
** Wrapper function to determine the memory size of an
** Ensembl Prediction Transcript via an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Prediction Transcript
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint predictionTranscriptadaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensPredictionTranscriptGetMemSize((const EnsPPredictionTranscript)
					     value);
}




/* @funcstatic predictionTranscriptadaptorGetFeature **************************
**
** Wrapper function to get the Ensembl Feature of an
** Ensembl Prediction Transcript from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl Prediction Transcript
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature predictionTranscriptadaptorGetFeature(const void *value)
{
    if(!value)
	return NULL;
    
    return ensPredictionTranscriptGetFeature((const EnsPPredictionTranscript)
					     value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Prediction Transcript Adaptor by
** pointer.
** It is the responsibility of the user to first destroy any previous
** Prediction Transcript Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPPredictionTranscriptadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                       Transcript Adaptor
** @argrule Ref object [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                       Transcript Adaptor
**
** @valrule * [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                              Transcript Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensPredictionTranscriptadaptorNew ************************************
**
** Default Ensembl Prediction Transcript Adaptor constructor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                           Transcript Adaptor or NULL
** @@
******************************************************************************/

EnsPPredictionTranscriptadaptor ensPredictionTranscriptadaptorNew(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
	return NULL;
    
    return
	ensFeatureadaptorNew(dba,
			     predictionTranscriptadaptorTables,
			     predictionTranscriptadaptorColumns,
			     predictionTranscriptadaptorLeftJoin,
			     predictionTranscriptadaptorDefaultCondition,
			     predictionTranscriptadaptorFinalCondition,
			     predictionTranscriptadaptorFetchAllBySQL,
			     (void* (*)(const void* key)) NULL, /* Fread */
			     predictionTranscriptadaptorCacheReference,
			     (AjBool (*)(const void* value)) NULL, /* Fwrite */
			     predictionTranscriptadaptorCacheDelete,
			     predictionTranscriptadaptorCacheSize,
			     predictionTranscriptadaptorGetFeature,
			     "PredictionTranscript");
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Prediction Transcript Adaptor.
**
** @fdata [EnsPPredictionTranscriptadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Prediction Transcript Adaptor
**               object.
**
** @argrule * Padaptor [EnsPPredictionTranscriptadaptor*] Ensembl Prediction
**                                                        Transcript Adaptor
**                                                        object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensPredictionTranscriptadaptorDel ************************************
**
** Default destructor for an Ensembl Prediction Transcript Adaptor.
**
** @param [d] Padaptor [EnsPPredictionTranscriptadaptor*] Ensembl Prediction
**                                                        Transcript Adaptor
**                                                        address
**
** @return [void]
** @@
******************************************************************************/

void ensPredictionTranscriptadaptorDel(
    EnsPPredictionTranscriptadaptor *Padaptor)
{
    if(!Padaptor)
	return;
    
    ensFeatureadaptorDel(Padaptor);
    
    return;
}




/* @func ensPredictionTranscriptadaptorFetchByIdentifier **********************
**
** Fetch an Ensembl Prediction Transcript via its SQL database-internal
** identifier.
** The caller is responsible for deleting the Ensembl Prediction Transcript.
**
** @param [r] adaptor [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                      Transcript Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Ppt [EnsPPredictionTranscript*] Ensembl Prediction
**                                             Transcript address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionTranscriptadaptorFetchByIdentifier(
    EnsPPredictionTranscriptadaptor adaptor,
    ajuint identifier,
    EnsPPredictionTranscript *Ppt)
{
    EnsPBaseadaptor ba = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Ppt)
	return ajFalse;
    
    ba = ensFeatureadaptorGetBaseadaptor(adaptor);
    
    *Ppt = (EnsPPredictionTranscript)
	ensBaseadaptorFetchByIdentifier(ba, identifier);
    
    return ajTrue;
}




/* @func ensPredictionTranscriptadaptorFetchByStableIdentifier ****************
**
** Fetch an Ensembl Prediction Transcript via its stable identifier and
** version. This method is called FetchByStableIdentifier for polymorphism with
** the Ensembl Transcript Adaptor. Prediction Transcript display labels are
** not necessarily stable in that the same identifier may be reused for a
** completely different Prediction Transcript in a subsequent database release.
**
** The caller is responsible for deleting the Ensembl Prediction Transcript.
**
** @param [r] adaptor [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                      Transcript Adaptor
** @param [r] stableid [const AjPStr] Stable identifier
** @param [wP] Ppt [EnsPPredictionTranscript*] Ensembl Prediction
**                                             Transcript address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPredictionTranscriptadaptorFetchByStableIdentifier(
    EnsPPredictionTranscriptadaptor adaptor,
    const AjPStr stableid,
    EnsPPredictionTranscript *Ppt)
{
    char *txtstableid = NULL;
    
    AjPList pts = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPPredictionTranscript pt = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!stableid)
	return ajFalse;
    
    if(!Ppt)
	return ajFalse;
    
    ba = ensFeatureadaptorGetBaseadaptor(adaptor);
    
    ensBaseadaptorEscapeC(ba, &txtstableid, stableid);
    
    constraint =
	ajFmtStr("prediction_transcript.display_label = '%s'", txtstableid);
    
    ajCharDel(&txtstableid);
    
    pts = ajListNew();
    
    ensBaseadaptorGenericFetch(ba,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       pts);
    
    if(ajListGetLength(pts) > 1)
	ajDebug("ensPredictionTranscriptadaptorFetchByStableIdentifier got "
		"more than one Prediction Transcript for stable identifier "
		"'%S'.\n",
		stableid);
    
    ajListPop(pts, (void **) Ppt);
    
    while(ajListPop(pts, (void **) &pt))
	ensPredictionTranscriptDel(&pt);
    
    ajListFree(&pts);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}
