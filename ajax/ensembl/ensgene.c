/******************************************************************************
** @source Ensembl Gene functions.
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.1 $
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

#include "ensgene.h"
#include "ensattribute.h"
#include "enstranscript.h"
#include "ensexon.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *geneStatus[] =
{
    NULL,
    "KNOWN",
    "NOVEL",
    "PUTATIVE",
    "PREDICTED",
    "KNOWN_BY_PROJECTION",
    "UNKNOWN",
    NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisAdaptor
ensRegistryGetAnalysisAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPAssemblyMapperAdaptor
ensRegistryGetAssemblyMapperAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPDatabaseEntryAdaptor
ensRegistryGetDatabaseEntryAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPExternalDatabaseAdaptor
ensRegistryGetExternalDatabaseAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPGeneAdaptor
ensRegistryGetGeneAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPSliceAdaptor
ensRegistryGetSliceAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPTranscriptAdaptor
ensRegistryGetTranscriptAdaptor(EnsPDatabaseAdaptor dba);

static int geneCompareExon(const void *P1, const void *P2);

static void geneDeleteExon(void **PP1, void *cl);

static AjBool geneAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                       const AjPStr statement,
                                       EnsPAssemblyMapper am,
                                       EnsPSlice slice,
                                       AjPList genes);

static void *geneAdaptorCacheReference(void *value);

static void geneAdaptorCacheDelete(void **value);

static ajuint geneAdaptorCacheSize(const void *value);

static EnsPFeature geneAdaptorGetFeature(const void *value);




/* @filesection ensgene *******************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPGene] Gene ***********************************************
**
** Functions for manipulating Ensembl Gene objects
**
** @cc Bio::EnsEMBL::Gene CVS Revision: 1.138
**
** @nam2rule Gene
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Gene by pointer.
** It is the responsibility of the user to first destroy any previous
** Gene. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGene]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGene] Ensembl Gene
** @argrule Ref object [EnsPGene] Ensembl Gene
**
** @valrule * [EnsPGene] Ensembl Gene
**
** @fcategory new
******************************************************************************/




/* @func ensGeneNew ***********************************************************
**
** Default Ensembl Gene constructor.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPGeneAdaptor] Ensembl Gene Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Feature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Gene::new
** @param [r] displaydbe [EnsPDatabaseEntry] Ensembl display Database Entry
** @param [u] description [AjPStr] Description
** @param [r] current [AjBool] Current attribute
** @param [u] stableid [AjPStr] Stable identifier
** @param [r] version [ajuint] Version
** @param [u] cdate [AjPStr] Creation date
** @param [u] mdate [AjPStr] Modification date
** @param [r] transcripts [AjPList] Ensembl Transcripts
**
** @return [EnsPGene] Ensembl Gene or NULL
** @@
******************************************************************************/

EnsPGene ensGeneNew(EnsPGeneAdaptor adaptor,
                    ajuint identifier,
                    EnsPFeature feature,
                    EnsPDatabaseEntry displaydbe,
                    AjPStr description,
                    AjPStr source,
                    AjPStr biotype,
                    AjEnum status,
                    AjBool current,
                    AjPStr stableid,
                    ajuint version,
                    AjPStr cdate,
                    AjPStr mdate,
                    AjPList transcripts)
{
    AjIList iter = NULL;
    
    EnsPGene gene = NULL;
    
    EnsPTranscript transcript = NULL;
    
    /*
     ajDebug("ensGeneNew\n"
	     "  adaptor %p\n"
	     "  identifier %u\n"
	     "  feature %p\n"
	     "  displaydbe %p\n"
	     "  description '%S'\n"
	     "  source '%S'\n"
	     "  biotype '%S'\n"
	     "  status %d\n"
	     "  current %b\n"
	     "  stableid '%S'\n"
	     "  version %u\n"
	     "  cdate '%S'\n"
	     "  mdate '%S'\n"
	     "  transcripts %p\n",
	     adaptor,
	     identifier,
	     feature,
	     displaydbe,
	     description,
	     source,
	     biotype,
	     status,
	     current,
	     stableid,
	     version,
	     cdate,
	     mdate,
	     transcripts);
     
     ensAnalysisTrace(analysis, 1);
     
     ensDatabaseEntryTrace(displaydbe, 1);
     */
    
    if(!feature)
	return NULL;
    
    AJNEW0(gene);
    
    gene->Use        = 1;
    gene->Identifier = identifier;
    gene->Adaptor    = adaptor;
    gene->Feature    = ensFeatureNewRef(feature);

    gene->DisplayReference = ensDatabaseEntryNewRef(displaydbe);
    
    if(description)
	gene->Description = ajStrNewRef(description);
    
    if(source)
	gene->Source = ajStrNewRef(source);
    
    if(biotype)
	gene->BioType = ajStrNewRef(biotype);
    
    gene->Status  = status;
    gene->Current = current;
    
    if(stableid)
	gene->StableIdentifier = ajStrNewRef(stableid);
    
    if(cdate)
	gene->CreationDate = ajStrNewRef(cdate);
    
    if(mdate)
	gene->ModificationDate = ajStrNewRef(mdate);
    
    gene->Version = version;
    
    gene->Attributes = NULL;
    
    gene->DatabaseEntries = NULL;
    
    if(transcripts && ajListGetLength(transcripts))
    {
	gene->Transcripts = ajListNew();
	
	iter = ajListIterNew(transcripts);
	
	while(!ajListIterDone(iter))
	{
	    transcript = (EnsPTranscript) ajListIterGet(iter);
	    
	    ajListPushAppend(gene->Transcripts,
			     (void *) ensTranscriptNewRef(transcript));
	}
	
	ajListIterDel(&iter);
    }
    else
	gene->Transcripts = NULL;
    
    return gene;
}




/* @func ensGeneNewObj ********************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGene] Ensembl Gene
**
** @return [EnsPGene] Ensembl Gene or NULL
** @@
******************************************************************************/

EnsPGene ensGeneNewObj(const EnsPGene object)
{
    AjIList iter = NULL;
    
    EnsPAttribute attribute = NULL;
    
    EnsPDatabaseEntry dbe = NULL;
    
    EnsPGene gene = NULL;
    
    EnsPTranscript transcript = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gene);
    
    gene->Use = 1;
    
    gene->Identifier = object->Identifier;
    gene->Adaptor    = object->Adaptor;
    gene->Feature    = ensFeatureNewRef(object->Feature);
    
    gene->DisplayReference = ensDatabaseEntryNewRef(object->DisplayReference);
    
    if(object->Description)
	gene->Description = ajStrNewRef(object->Description);
    
    if(object->Source)
	gene->Source = ajStrNewRef(object->Source);
    
    if(object->BioType)
	gene->BioType = ajStrNewRef(object->BioType);
    
    gene->Status = object->Status;
    
    gene->Current = object->Current;
    
    if(object->StableIdentifier)
	gene->StableIdentifier = ajStrNewRef(object->StableIdentifier);
    
    if(object->CreationDate)
        gene->CreationDate = ajStrNewRef(object->CreationDate);
    
    if(object->ModificationDate)
        gene->ModificationDate = ajStrNewRef(object->ModificationDate);
    
    gene->Version = object->Version;
    
    /* Copy the AJAX List of Ensembl Attributes. */
    
    if(object->Attributes && ajListGetLength(object->Attributes))
    {
	gene->Attributes = ajListNew();
	
	iter = ajListIterNew(object->Attributes);
	
	while(!ajListIterDone(iter))
	{
	    attribute = (EnsPAttribute) ajListIterGet(iter);
	    
	    ajListPushAppend(gene->Attributes,
			     (void *) ensAttributeNewRef(attribute));
	}
	
	ajListIterDel(&iter);
    }
    else
	gene->Attributes = NULL;
    
    /* Copy the AJAX List of Ensembl Database Entries. */
    
    if(object->DatabaseEntries && ajListGetLength(object->DatabaseEntries))
    {
	gene->DatabaseEntries = ajListNew();
	
	iter = ajListIterNew(object->DatabaseEntries);
	
	while(!ajListIterDone(iter))
	{
	    dbe = (EnsPDatabaseEntry) ajListIterGet(iter);
	    
	    ajListPushAppend(gene->DatabaseEntries,
			     (void *) ensDatabaseEntryNewRef(dbe));
	}
	
	ajListIterDel(&iter);
    }
    else
	gene->DatabaseEntries = NULL;
    
    /* Copy the AJAX List of Ensembl Transcripts. */
    
    if(object->Transcripts && ajListGetLength(object->Transcripts))
    {
	gene->Transcripts = ajListNew();
	
	iter = ajListIterNew(object->Transcripts);
	
	while(!ajListIterDone(iter))
	{
	    transcript = (EnsPTranscript) ajListIterGet(iter);
	    
	    ajListPushAppend(gene->Transcripts,
			     (void *) ensTranscriptNewRef(transcript));
	}
	
	ajListIterDel(&iter);
    }
    else
	gene->Transcripts = NULL;
    
    return gene;
}




/* @func ensGeneNewRef ********************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gene [EnsPGene] Ensembl Gene
**
** @return [EnsPGene] Ensembl Gene or NULL
** @@
******************************************************************************/

EnsPGene ensGeneNewRef(EnsPGene gene)
{
    if(!gene)
	return NULL;
    
    gene->Use++;
    
    return gene;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Gene.
**
** @fdata [EnsPGene]
** @fnote None
**
** @nam3rule Del Destroy (free) a Gene object
**
** @argrule * Pgene [EnsPGene*] Gene object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGeneDel ***********************************************************
**
** Default destructor for an Ensembl Gene.
**
** @param [d] Pgene [EnsPGene*] Ensembl Gene address
**
** @return [void]
** @@
******************************************************************************/

void ensGeneDel(EnsPGene *Pgene)
{
    EnsPAttribute attribute = NULL;
    
    EnsPDatabaseEntry dbe = NULL;
    
    EnsPTranscript transcript = NULL;

    EnsPGene pthis = NULL;
    
    if(!Pgene)
        return;
    
    if(!*Pgene)
        return;
    
    /*
     ajDebug("ensGeneDel\n"
	     "  *Pgene %p\n",
	     *Pgene);
     
     ensGeneTrace(*Pgene, 1);
     */

    pthis = *Pgene;
    
    pthis->Use--;
    
    if((*Pgene)->Use)
    {
	*Pgene = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    ensDatabaseEntryDel(&pthis->DisplayReference);
    
    ajStrDel(&pthis->Description);
    ajStrDel(&pthis->Source);
    ajStrDel(&pthis->BioType);
    ajStrDel(&pthis->StableIdentifier);
    ajStrDel(&pthis->CreationDate);
    ajStrDel(&pthis->ModificationDate);
    
    /* Clear and delete the AJAX List of Ensembl Attributes. */
    
    while(ajListPop((*Pgene)->Attributes, (void **) &attribute))
	ensAttributeDel(&attribute);
    
    ajListFree(&pthis->DatabaseEntries);
    
    /* Clear and delete the AJAX List of Ensembl Database Entries. */
    
    while(ajListPop((*Pgene)->DatabaseEntries, (void **) &dbe))
	ensDatabaseEntryDel(&dbe);
    
    ajListFree(&pthis->DatabaseEntries);
    
    /* Clear and delete the AJAX List of Ensembl Transcripts. */
    
    while(ajListPop((*Pgene)->Transcripts, (void **) &transcript))
	ensTranscriptDel(&transcript);
    
    ajListFree(&pthis->Transcripts);
    
    AJFREE(pthis);

    *Pgene = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Gene object.
**
** @fdata [EnsPGene]
** @fnote None
**
** @nam3rule Get Return Gene attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Gene Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetDisplayReference Return the display External Reference
** @nam4rule GetDescription Return the description
** @nam4rule GetSource Return the source
** @nam4rule GetBioType Return the biological type
** @nam4rule GetStatus Return the status
** @nam4rule GetStableIdentifier Return the stable identifier
** @nam4rule GetVersion Return the version
** @nam4rule GetCreationDate Return the creation date
** @nam4rule GetModificationDate Return the modification date
** @nam4rule GetAttributes Return all Ensembl Attributes
** @nam4rule GetDatabaseEntries Return all Ensembl Database Entries
** @nam4rule GetTranscripts Return all Ensembl Transcripts
**
** @argrule * gene [const EnsPGene] Gene
**
** @valrule Adaptor [EnsPGeneAdaptor] Ensembl Gene Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule DisplayReference [EnsPDatabaseEntry] Ensembl Database Entry
** @valrule Description [AjPStr] Description
** @valrule Source [AjPStr] Source
** @valrule BioType [AjPStr] Biological type
** @valrule Status [AjEnum] Status
** @valrule StableIdentifier [AjPStr] Stable identifier
** @valrule Version [ajuint] Version
** @valrule GetCreationDate [AjPStr] Creation date
** @valrule GetModificationDate [AjPStr] Modification date
** @valrule GetAttributes [const AjPList] AJAX List of Ensembl Attributes
** @valrule GetDatabaseEntries [const AjPList] AJAX List of
**                                             Ensembl Database Entries
** @valrule GetTranscripts [const AjPList] AJAX List of Ensembl Transcripts
**
** @fcategory use
******************************************************************************/




/* @func ensGeneGetAdaptor ****************************************************
**
** Get the Ensembl Gene Adaptor element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [EnsPGeneAdaptor] Ensembl Gene Adaptor
** @@
******************************************************************************/

EnsPGeneAdaptor ensGeneGetAdaptor(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->Adaptor;
}




/* @func ensGeneGetIdentifier *************************************************
**
** Get the SQL database-internal identifier element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensGeneGetIdentifier(const EnsPGene gene)
{
    if(!gene)
        return 0;
    
    return gene->Identifier;
}




/* @func ensGeneGetFeature ****************************************************
**
** Get the Ensembl Feature element of an Ensembl Gene.
**
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensGeneGetFeature(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->Feature;
}




/* @func ensGeneGetDisplayReference *******************************************
**
** Get the display reference element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::display_xref
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [EnsPDatabaseEntry] Ensembl Database Entry
** @@
******************************************************************************/

EnsPDatabaseEntry ensGeneGetDisplayReference(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->DisplayReference;
}




/* @func ensGeneGetDescription ************************************************
**
** Get the description element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::description
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjPStr] Description
** @@
******************************************************************************/

AjPStr ensGeneGetDescription(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->Description;
}




/* @func ensGeneGetSource *****************************************************
**
** Get the source element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::source
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjPStr] Source
** @@
******************************************************************************/

AjPStr ensGeneGetSource(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->Source;
}




/* @func ensGeneGetBioType ****************************************************
**
** Get the biological type element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::biotype
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjPStr] Biological type
** @@
******************************************************************************/

AjPStr ensGeneGetBioType(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->BioType;
}




/* @func ensGeneGetStatus *****************************************************
**
** Get the status element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::status
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjEnum] Status
** @@
******************************************************************************/

AjEnum ensGeneGetStatus(const EnsPGene gene)
{
    if(!gene)
        return ensEGeneStatusNULL;
    
    return gene->Status;
}




/* @func ensGeneGetCurrent ****************************************************
**
** Get the current element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::is_current
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjBool] ajTrue if this Gene reflects the current state of
**                  annotation
** @@
******************************************************************************/

AjBool ensGeneGetCurrent(const EnsPGene gene)
{
    if(!gene)
        return ajFalse;
    
    return gene->Current;
}




/* @func ensGeneGetStableIdentifier *******************************************
**
** Get the stable identifier element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::stable_id
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjPStr] Stable identifier
** @@
******************************************************************************/

AjPStr ensGeneGetStableIdentifier(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->StableIdentifier;
}




/* @func ensGeneGetVersion ****************************************************
**
** Get the version element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::version
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [ajuint] Version
** @@
******************************************************************************/

ajuint ensGeneGetVersion(const EnsPGene gene)
{
    if(!gene)
        return 0;
    
    return gene->Version;
}




/* @func ensGeneGetCreationDate ***********************************************
**
** Get the creation date element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::created_date
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjPStr] Creation date
** @@
******************************************************************************/

AjPStr ensGeneGetCreationDate(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->CreationDate;
}




/* @func ensGeneGetModificationDate *******************************************
**
** Get the modification date element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::modified_date
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [AjPStr] Modification date
** @@
******************************************************************************/

AjPStr ensGeneGetModificationDate(const EnsPGene gene)
{
    if(!gene)
        return NULL;
    
    return gene->ModificationDate;
}




/* @func ensGeneGetAttributes *************************************************
**
** Get all Ensembl Attributes of an Ensembl Gene.
**
** This is not a simple accessor function, it will fetch Ensembl Attributes
** from the Ensembl Core database in case the AJAX List is empty.
**
** @cc Bio::EnsEMBL::Gene::get_all_Attributes
** @param [u] gene [EnsPGene] Ensembl Gene
**
** @return [const AjPList] AJAX List of Ensembl Attributes
** @@
******************************************************************************/

const AjPList ensGeneGetAttributes(EnsPGene gene)
{
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gene)
	return NULL;
    
    if(gene->Attributes)
	return gene->Attributes;
    
    if(!gene->Adaptor)
    {
	ajDebug("ensGeneGetAttributes cannot fetch "
		"Ensembl Attributes for a Gene without a "
		"Gene Adaptor.\n");
	
	return NULL;
    }
    
    dba = ensGeneAdaptorGetDatabaseAdaptor(gene->Adaptor);
    
    if(!dba)
    {
	ajDebug("ensGeneGetAttributes cannot fetch "
		"Ensembl Attributes for a Gene without a "
		"Database Adaptor set in the Gene Adaptor.\n");
	
	return NULL;
    }
    
    gene->Attributes = ajListNew();
    
    ensAttributeAdaptorFetchAllByGene(dba,
				      gene,
				      (AjPStr) NULL,
				      gene->Attributes);
    
    return gene->Attributes;
}




/* @func ensGeneGetDatabaseEntries ********************************************
**
** Get all Ensembl Database Entries of an Ensembl Gene.
**
** This is not a simple accessor function, it will fetch
** Ensembl Database Entries from the Ensembl Core database in case the
** AJAX List is empty.
**
** @cc Bio::EnsEMBL::Gene::get_all_DBEntries
** @param [u] gene [EnsPGene] Ensembl Gene
**
** @return [const AjPList] AJAX List of Ensembl Database Entries
** @@
******************************************************************************/

const AjPList ensGeneGetDatabaseEntries(EnsPGene gene)
{
    AjPStr objtype = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    EnsPDatabaseEntryAdaptor dbea = NULL;
    
    if(!gene)
	return NULL;
    
    if(gene->DatabaseEntries)
	return gene->DatabaseEntries;
    
    if(!gene->Adaptor)
    {
	ajDebug("ensGeneGetDatabaseEntries cannot fetch "
		"Ensembl Database Entries for a Gene without a "
		"Gene Adaptor.\n");
	
	return NULL;
    }
    
    dba = ensGeneAdaptorGetDatabaseAdaptor(gene->Adaptor);
    
    if(!dba)
    {
	ajDebug("ensGeneGetDatabaseEntries cannot fetch "
		"Ensembl Database Entries for a Gene without a "
		"Database Adaptor set in the Gene Adaptor.\n");
	
	return NULL;
    }
    
    dbea = ensRegistryGetDatabaseEntryAdaptor(dba);
    
    objtype = ajStrNewC("Gene");
    
    gene->DatabaseEntries = ajListNew();
    
    ensDatabaseEntryAdaptorFetchAllByObjectType(dbea,
						gene->Identifier,
						objtype,
						(AjPStr) NULL,
						ensEExternalDatabaseTypeNULL,
						gene->DatabaseEntries);
    
    ajStrDel(&objtype);
    
    return gene->DatabaseEntries;
}




/* @func ensGeneGetTranscripts ************************************************
**
** Get all Ensembl Transcripts of an Ensembl Gene.
**
** This is not a simple accessor function, it will fetch
** Ensembl Transcripts from the Ensembl Core database in case the
** AJAX List is empty.
**
** @cc Bio::EnsEMBL::Gene::get_all_Transcripts
** @param [u] gene [EnsPGene] Ensembl Gene
**
** @return [const AjPList] AJAX List of Ensembl Transcripts
** @@
******************************************************************************/

const AjPList ensGeneGetTranscripts(EnsPGene gene)
{
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPTranscriptAdaptor ta = NULL;
    
    if(!gene)
	return NULL;
    
    if(gene->Transcripts)
	return gene->Transcripts;
    
    if(!gene->Adaptor)
    {
	ajDebug("ensGeneGetTranscripts cannot fetch "
		"Ensembl Transcripts for a Gene without a "
		"Gene Adaptor.\n");
	
	return NULL;
    }
    
    dba = ensGeneAdaptorGetDatabaseAdaptor(gene->Adaptor);
    
    ta = ensRegistryGetTranscriptAdaptor(dba);
    
    gene->Transcripts = ajListNew();
    
    ensTranscriptAdaptorFetchAllByGene(ta, gene, gene->Transcripts);
    
    return gene->Transcripts;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Gene object.
**
** @fdata [EnsPGene]
** @fnote None
**
** @nam3rule Set Set one element of a Gene
** @nam4rule SetAdaptor Set the Ensembl Gene Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetDisplayRefernece Set the Ensembl Database Entry
** @nam4rule SetDescription Set the description
** @nam4rule SetSource Set the source
** @nam4rule SetBioType Set the biological type
** @nam4rule SetStatus Set the status
** @nam4rule SetCurrent Set the current element
** @nam4rule SetStableIdentifier Set the stable identifier
** @nam4rule SetVersion Set the version
** @nam4rule SetCreationDate Set the creation date
** @nam4rule SetModificationDate Set the modification date
**
** @argrule * gene [EnsPGene] Ensembl Gene object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGeneSetAdaptor ****************************************************
**
** Set the Ensembl Gene Adaptor element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] adaptor [EnsPGeneAdaptor] Ensembl Gene Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetAdaptor(EnsPGene gene, EnsPGeneAdaptor adaptor)
{
    if(!gene)
        return ajFalse;
    
    gene->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensGeneSetIdentifier *************************************************
**
** Set the SQL database-internal identifier element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetIdentifier(EnsPGene gene, ajuint identifier)
{
    if(!gene)
        return ajFalse;
    
    gene->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGeneSetFeature ****************************************************
**
** Set the Ensembl Feature element of an Ensembl Gene.
**
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetFeature(EnsPGene gene, EnsPFeature feature)
{
    AjIList iter = NULL;
    
    EnsPSlice slice = NULL;
    
    EnsPTranscript oldtranscript = NULL;
    EnsPTranscript newtranscript = NULL;
    
    ajDebug("ensGeneSetFeature\n"
	    "  gene %p\n"
	    "  feature %p\n",
	    gene,
	    feature);
    
    if(!gene)
	return ajFalse;
    
    if(!feature)
	return ajFalse;
    
    /* Replace the current Feature. */
    
    if(gene->Feature)
	ensFeatureDel(&(gene->Feature));
    
    gene->Feature = ensFeatureNewRef(feature);
    
    slice = ensFeatureGetSlice(gene->Feature);
    
    /* Transfer the Transcripts onto the new Slice. */
    
    iter = ajListIterNew(gene->Transcripts);
    
    while(!ajListIterDone(iter))
    {
	oldtranscript = (EnsPTranscript) ajListIterGet(iter);
	
	ajListIterRemove(iter);
	
	newtranscript = ensTranscriptTransfer(oldtranscript, slice);
	
	ajDebug("  oldtranscript %p\n"
		"  newtranscript %p\n",
		oldtranscript,
		newtranscript);
	
	if(!newtranscript)
	{
	    ajDebug("ensGeneSetFeature could not transfer "
		    "Transcript onto new Ensembl Feature Slice.");
	    
	    ensTranscriptTrace(oldtranscript, 1);
	}
	
	ajListIterInsert(iter, (void *) newtranscript);
	
	/* Advance the AJAX List Iterator after the insert. */
	
	/*
	** TODO: Report this to EMBOSS.
	** Shouldn't the Iterator remain at the position?
	** It looks like remove happens at the Iterator position, but inserts
	** happen before the iterator so that the inserted object is fetched in
	** the nnext round. This seems counter intuitive?
	*/
	
	(void) ajListIterGet(iter);
	
	ensTranscriptDel(&oldtranscript);
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensGeneSetDisplayReference *******************************************
**
** Set the Ensembl Database Entry element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::display_xref
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] dbe [EnsPDatabaseEntry] Ensembl Database Entry
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetDisplayReference(EnsPGene gene, EnsPDatabaseEntry dbe)
{
    if(!gene)
        return ajFalse;
    
    ensDatabaseEntryDel(&gene->DisplayReference);
    
    gene->DisplayReference = ensDatabaseEntryNewRef(dbe);
    
    return ajTrue;
}




/* @func ensGeneSetDescription ************************************************
**
** Set the description element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::description
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] description [AjPStr] Description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetDescription(EnsPGene gene, AjPStr description)
{
    if(!gene)
        return ajFalse;
    
    ajStrDel(&gene->Description);
    
    gene->Description = ajStrNewRef(description);
    
    return ajTrue;
}




/* @func ensGeneSetSource ************************************************
**
** Set the source element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::source
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] source [AjPStr] Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetSource(EnsPGene gene, AjPStr source)
{
    if(!gene)
        return ajFalse;
    
    ajStrDel(&gene->Source);
    
    gene->Source = ajStrNewRef(source);
    
    return ajTrue;
}




/* @func ensGeneSetBioType ****************************************************
**
** Set the biological type element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::biotype
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] biotype [AjPStr] Biological type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetBioType(EnsPGene gene, AjPStr biotype)
{
    if(!gene)
        return ajFalse;
    
    ajStrDel(&gene->BioType);
    
    gene->BioType = ajStrNewRef(biotype);
    
    return ajTrue;
}




/* @func ensGeneSetStatus *****************************************************
**
** Set the status element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::status
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] status [AjEnum] Status
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetStatus(EnsPGene gene, AjEnum status)
{
    if(!gene)
        return ajFalse;
    
    gene->Status = status;
    
    return ajTrue;
}




/* @func ensGeneSetCurrent ****************************************************
**
** Set the current element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::is_current
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] current [AjBool] Current attribute
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetCurrent(EnsPGene gene, AjBool current)
{
    if(!gene)
        return ajFalse;
    
    gene->Current = current;
    
    return ajTrue;
}




/* @func ensGeneSetStableIdentifier *******************************************
**
** Set the stable identifier element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::stable_id
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] stableid [AjPStr] Stable identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetStableIdentifier(EnsPGene gene, AjPStr stableid)
{
    if(!gene)
        return ajFalse;
    
    ajStrDel(&gene->StableIdentifier);
    
    gene->StableIdentifier = ajStrNewRef(stableid);
    
    return ajTrue;
}




/* @func ensGeneSetVersion ****************************************************
**
** Set the version element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::version
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] version [ajuint] Version
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetVersion(EnsPGene gene, ajuint version)
{
    if(!gene)
        return ajFalse;
    
    gene->Version = version;
    
    return ajTrue;
}




/* @func ensGeneSetCreationDate ***********************************************
**
** Set the creation date element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::created_date
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] cdate [AjPStr] Creation date
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetCreationDate(EnsPGene gene, AjPStr cdate)
{
    if(!gene)
        return ajFalse;
    
    ajStrDel(&gene->CreationDate);
    
    gene->CreationDate = ajStrNewRef(cdate);
    
    return ajTrue;
}




/* @func ensGeneSetModificationDate *******************************************
**
** Set the modification date element of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::modified_date
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] mdate [AjPStr] Modification date
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneSetModificationDate(EnsPGene gene, AjPStr mdate)
{
    if(!gene)
        return ajFalse;
    
    ajStrDel(&gene->ModificationDate);
    
    gene->ModificationDate = ajStrNewRef(mdate);
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Gene object.
**
** @fdata [EnsPGene]
** @nam3rule Trace Report Ensembl Gene elements to debug file
**
** @argrule Trace gene [const EnsPGene] Ensembl Gene
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGeneTrace *********************************************************
**
** Trace an Ensembl Gene.
**
** @param [r] gene [const EnsPGene] Ensembl Gene
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneTrace(const EnsPGene gene, ajuint level)
{
    AjIList iter = NULL;
    
    AjPStr indent = NULL;
    
    EnsPAttribute attribute = NULL;
    
    EnsPDatabaseEntry dbe = NULL;
    
    EnsPTranscript transcript = NULL;
    
    if(!gene)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGeneTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  DisplayReference %p\n"
	    "%S  Description '%S'\n"
	    "%S  Source '%S'\n"
	    "%S  BioType '%S'\n"
	    "%S  Status %d\n"
	    "%S  Current '%B'\n"
	    "%S  StableIdentifier '%S'\n"
	    "%S  CreationDate '%S'\n"
	    "%S  ModificationDate '%S'\n"
	    "%S  Version %u\n"
	    "%S  Attributes %p\n"
	    "%S  DatabaseEntries %p\n"
	    "%S  Transcripts %p\n",
	    indent, gene,
	    indent, gene->Use,
	    indent, gene->Identifier,
	    indent, gene->Adaptor,
	    indent, gene->Feature,
	    indent, gene->DisplayReference,
	    indent, gene->Description,
	    indent, gene->Source,
	    indent, gene->BioType,
	    indent, gene->Status,
	    indent, gene->Current,
	    indent, gene->StableIdentifier,
	    indent, gene->CreationDate,
	    indent, gene->ModificationDate,
	    indent, gene->Version,
	    indent, gene->Attributes,
	    indent, gene->DatabaseEntries,
	    indent, gene->Transcripts);
    
    ensFeatureTrace(gene->Feature, level + 1);
    
    ensDatabaseEntryTrace(gene->DisplayReference, level + 1);
    
    /* Trace the AJAX List of Ensembl Attributes. */
    
    if(gene->Attributes)
    {
	ajDebug("%S    AJAX List %p of Ensembl Attributes\n",
		indent, gene->Attributes);
	
	iter = ajListIterNewread(gene->Attributes);
	
	while(!ajListIterDone(iter))
	{
	    attribute = (EnsPAttribute) ajListIterGet(iter);
	    
	    ensAttributeTrace(attribute, level + 2);
	}
	
	ajListIterDel(&iter);
    }
    
    /* Trace the AJAX List of Ensembl Database Entries. */
    
    if(gene->DatabaseEntries)
    {
	ajDebug("%S    AJAX List %p of Ensembl Database Entries\n",
		indent, gene->DatabaseEntries);
	
	iter = ajListIterNewread(gene->DatabaseEntries);
	
	while(!ajListIterDone(iter))
	{
	    dbe = (EnsPDatabaseEntry) ajListIterGet(iter);
	    
	    ensDatabaseEntryTrace(dbe, level + 2);
	}
	
	ajListIterDel(&iter);
    }
    
    /* Trace the AJAX List of Ensembl Transcripts. */
    
    if(gene->Transcripts)
    {
	ajDebug("%S    AJAX List %p of Ensembl Transcripts\n",
		indent, gene->Transcripts);
	
	iter = ajListIterNewread(gene->Transcripts);
	
	while(!ajListIterDone(iter))
	{
	    transcript = (EnsPTranscript) ajListIterGet(iter);
	    
	    ensTranscriptTrace(transcript, level + 2);
	}
	
	ajListIterDel(&iter);
    }
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensGeneGetMemSize ****************************************************
**
** Get the memory size in bytes of an Ensembl Gene.
**
** @param [r] gene [const EnsPGene] Ensembl Gene
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGeneGetMemSize(const EnsPGene gene)
{
    ajuint size = 0;
    
    AjIList iter = NULL;
    
    EnsPAttribute attribute = NULL;
    
    EnsPDatabaseEntry dbe = NULL;
    
    EnsPTranscript transcript = NULL;
    
    if(!gene)
	return 0;
    
    size += (ajuint) sizeof (EnsOGene);
    
    size += ensFeatureGetMemSize(gene->Feature);
    
    size += ensDatabaseEntryGetMemSize(gene->DisplayReference);
    
    if(gene->Description)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gene->Description);
    }
    
    if(gene->Source)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gene->Source);
    }
    
    if(gene->BioType)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gene->BioType);
    }
    
    if(gene->StableIdentifier)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gene->StableIdentifier);
    }
    
    if(gene->CreationDate)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gene->CreationDate);
    }
    
    if(gene->ModificationDate)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gene->ModificationDate);
    }
    
    /* Summarise the AJAX List of Ensembl Attributes. */
    
    if(gene->Attributes)
    {
	size += (ajuint) sizeof (AjOList);
	
	iter = ajListIterNewread(gene->Attributes);
	
	while(!ajListIterDone(iter))
	{
	    attribute = (EnsPAttribute) ajListIterGet(iter);
	    
	    size += ensAttributeGetMemSize(attribute);
	}
	
	ajListIterDel(&iter);
    }
    
    /* Summarise the AJAX List of Ensembl Database Entries. */
    
    if(gene->DatabaseEntries)
    {
	size += (ajuint) sizeof (AjOList);
	
	iter = ajListIterNewread(gene->DatabaseEntries);
	
	while(!ajListIterDone(iter))
	{
	    dbe = (EnsPDatabaseEntry) ajListIterGet(iter);
	    
	    size += ensDatabaseEntryGetMemSize(dbe);
	}
	
	ajListIterDel(&iter);
    }
    
    /* Summarise the AJAX List of Ensembl Transcripts. */
    
    if(gene->Transcripts)
    {
	size += (ajuint) sizeof (AjOList);
	
	iter = ajListIterNewread(gene->Transcripts);
	
	while(!ajListIterDone(iter))
	{
	    transcript = (EnsPTranscript) ajListIterGet(iter);
	    
	    size += ensTranscriptGetMemSize(transcript);
	}
	
	ajListIterDel(&iter);
    }
    
    return size;
}




/* @func ensGeneAddAttribute **************************************************
**
** Add an Ensembl Attribute to an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::add_Attributes
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] attribute [EnsPAttribute] Ensembl Attribute
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneAddAttribute(EnsPGene gene, EnsPAttribute attribute)
{
    if(!gene)
	return ajFalse;
    
    if(!attribute)
	return ajFalse;
    
    if(!gene->Attributes)
	gene->Attributes = ajListNew();
    
    ajListPushAppend(gene->Attributes, (void *) ensAttributeNewRef(attribute));
    
    return ajTrue;
}




/* @func ensGeneAddDatabaseEntry **********************************************
**
** Add an Ensembl Database Entry to an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::add_DBEntry
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] dbe [EnsPDatabaseEntry] Ensembl Database Entry
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneAddDatabaseEntry(EnsPGene gene, EnsPDatabaseEntry dbe)
{
    if(!gene)
	return ajFalse;
    
    if(!dbe)
	return ajFalse;
    
    if(!gene->DatabaseEntries)
	gene->DatabaseEntries = ajListNew();
    
    ajListPushAppend(gene->DatabaseEntries,
		     (void *) ensDatabaseEntryNewRef(dbe));
    
    return ajTrue;
}




/* @func ensGeneAddTranscript *************************************************
**
** Add an Ensembl Transcripts to an Ensembl Gene.
** This will also re-calculate gene coordinates.
**
** @cc Bio::EnsEMBL::Gene::add_Transcript
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] transcript [EnsPTranscript] Ensembl Transcript
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneAddTranscript(EnsPGene gene, EnsPTranscript transcript)
{
    if(!gene)
	return ajFalse;
    
    if(!transcript)
	return ajFalse;
    
    if(!gene->Transcripts)
	gene->Transcripts = ajListNew();
    
    ajListPushAppend(gene->Transcripts,
		     (void *) ensTranscriptNewRef(transcript));
    
    ensGeneCalculateCoordinates(gene);
    
    return ajTrue;
}




/* @func ensGeneStatusFromStr *************************************************
**
** Convert an AJAX String into an Ensembl Gene status element.
**
** @param [r] status [const AjPStr] Status string
**
** @return [AjEnum] Ensembl Gene status element or
**                  ensEGeneStatusNULL
** @@
******************************************************************************/

AjEnum ensGeneStatusFromStr(const AjPStr status)
{
    register ajint i = 0;
    
    AjEnum estatus = ensEGeneStatusNULL;
    
    for(i = 1; geneStatus[i]; i++)
	if(ajStrMatchC(status, geneStatus[i]))
	    estatus = i;
    
    if(!estatus)
	ajDebug("ensGeneStatusFromStr encountered "
		"unexpected string '%S'.\n", status);
    
    return estatus;
}




/* @func ensGeneStatusToChar **************************************************
**
** Convert an Ensembl Gene status element into a C-type (char*) string.
**
** @param [r] status [const AjEnum] Gene status enumerator
**
** @return [const char*] Gene status C-type (char*) string
** @@
******************************************************************************/

const char* ensGeneStatusToChar(const AjEnum status)
{
    register ajint i = 0;
    
    if(!status)
	return NULL;
    
    for(i = 1; geneStatus[i] && (i < status); i++);
    
    if(!geneStatus[i])
	ajDebug("ensGeneStatusToChar encountered an "
		"out of boundary error on status %d.\n", status);
    
    return geneStatus[i];
}




/* @func ensGeneCalculateCoordinates ******************************************
**
** Calculate coordinates of an Ensembl Gene.
** This function should be called, whenever a Transcript of this Gene has
** been changed.
**
** @cc Bio::EnsEMBL::Gene::recalculate_coordinates
** @param [u] gene [EnsPGene] Ensembl Gene
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneCalculateCoordinates(EnsPGene gene)
{
    ajint start = 0;
    ajint end = 0;
    ajint strand = 0;
    
    AjBool transsplicing = AJFALSE;
    
    AjIList iter = NULL;
    const AjPList list = NULL;
    
    EnsPFeature gfeature = NULL;
    EnsPFeature tfeature = NULL;
    
    EnsPSlice slice = NULL;
    
    EnsPTranscript transcript = NULL;
    
    if(!gene)
	return ajFalse;
    
    list = ensGeneGetTranscripts(gene);
    
    if(!ajListGetLength(list))
	return ajTrue;
    
    /* Get the first Transcript with coordinates. */
    
    iter = ajListIterNewread(list);
    
    while(!ajListIterDone(iter))
    {
	transcript = (EnsPTranscript) ajListIterGet(iter);
	
	/* Skip missing Transcripts. */
	
	if(!transcript)
	    continue;
	
	tfeature = ensTranscriptGetFeature(transcript);
	
	/* Skip un-mapped Transcripts. */
	
	if(!ensFeatureGetStart(tfeature))
	    continue;
	
	slice = ensFeatureGetSlice(tfeature);
	
	start = ensFeatureGetStart(tfeature);
	
	end = ensFeatureGetEnd(tfeature);
	
	strand = ensFeatureGetStrand(tfeature);
	
	break;
    }
    
    ajListIterDel(&iter);
    
    /* Start loop after the first Transcript with coordinates. */
    
    iter = ajListIterNewread(list);
    
    while(!ajListIterDone(iter))
    {
	transcript = (EnsPTranscript) ajListIterGet(iter);
	
	/* Skip missing Transcripts. */
	
	if(!transcript)
	    continue;
	
	tfeature = ensTranscriptGetFeature(transcript);
	
	/* Skip un-mapped Transcripts. */
	
	if(!ensFeatureGetStart(tfeature))
	    continue;
	
	if(!ensSliceMatch(ensFeatureGetSlice(tfeature), slice))
	    ajFatal("ensGeneCalculateCoordinates got Transcripts of one "
		    "Gene on different Slices.\n");
	
	start =
	    (ensFeatureGetStart(tfeature) < start) ?
	    ensFeatureGetStart(tfeature) :
	    start;
	
	end =
	    (ensFeatureGetEnd(tfeature) > end) ?
	    ensFeatureGetEnd(tfeature) :
	    end;
	
	if(ensFeatureGetStrand(tfeature) != strand)
	    transsplicing = ajTrue;
    }
    
    ajListIterDel(&iter);
    
    if(transsplicing)
	ajWarn("ensGeneCalculateCoordinates got Gene with "
	       "trans-splicing event.\n");
    
    gfeature = ensGeneGetFeature(gene);
    
    ensFeatureSetStart(gfeature, start);
    
    ensFeatureSetEnd(gfeature, end);
    
    ensFeatureSetStrand(gfeature, strand);
    
    ensFeatureSetSlice(gfeature, slice);
    
    /* Clear internal values that depend on Transcript coordinates. None! */
    
    return ajTrue;
}




/* @func ensGeneFetchAllAttributes ********************************************
**
** Fetch all Ensembl Attributes of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::get_all_Attributes
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] code [const AjPStr] Ensembl Attribute code
** @param [u] attributes [AjPList] AJAX List of Ensembl Attributes
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneFetchAllAttributes(EnsPGene gene, const AjPStr code,
                                 AjPList attributes)
{
    AjBool match = AJFALSE;
    
    const AjPList list = NULL;
    AjIList iter = NULL;
    
    EnsPAttribute attribute = NULL;
    
    if(!gene)
	return ajFalse;
    
    if(!attributes)
	return ajFalse;
    
    list = ensGeneGetAttributes(gene);
    
    iter = ajListIterNewread(list);
    
    while(!ajListIterDone(iter))
    {
	attribute = (EnsPAttribute) ajListIterGet(iter);
	
	if(code)
	{
	    if(ajStrMatchCaseS(code, ensAttributeGetCode(attribute)))
		match = ajTrue;
	    else
		match = ajFalse;
	}
	else
	    match = ajTrue;
	
	if(match)
	    ajListPushAppend(attributes,
			     (void *) ensAttributeNewRef(attribute));
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensGeneFetchAllDatabaseEntries ***************************************
**
** Fetch all Ensembl Database Entries of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::get_all_DBEntries
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] name [const AjPStr] Ensembl External Database name
** @param [r] type [AjEnum] Ensembl External Database type
** @param [u] dbes [AjPList] AJAX List of Ensembl Database Entries
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: The Perl API documentation does not document the two
** additional arguments to this function.
*/

AjBool ensGeneFetchAllDatabaseEntries(EnsPGene gene,
                                      const AjPStr name,
                                      AjEnum type,
                                      AjPList dbes)
{
    AjBool namematch = AJFALSE;
    AjBool typematch = AJFALSE;
    
    const AjPList list = NULL;
    AjIList iter       = NULL;
    
    EnsPDatabaseEntry dbe = NULL;
    
    if(!gene)
	return ajFalse;
    
    if(!dbes)
	return ajFalse;
    
    list = ensGeneGetDatabaseEntries(gene);
    
    iter = ajListIterNewread(list);
    
    while(!ajListIterDone(iter))
    {
	dbe = (EnsPDatabaseEntry) ajListIterGet(iter);
	
	if(name)
	{
	    if(ajStrMatchCaseS(name, ensDatabaseEntryGetDbName(dbe)))
		namematch = ajTrue;
	    else
		namematch = ajFalse;
	}
	else
	    namematch = ajTrue;
	
	if(type)
	{
	    if(type == ensDatabaseEntryGetType(dbe))
		typematch = ajTrue;
	    else
		typematch = ajFalse;
	}
	else
	    typematch = ajTrue;
	
	if(namematch && typematch)
	    ajListPushAppend(dbes, (void *) ensDatabaseEntryNewRef(dbe));
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @funcstatic geneCompareExon ************************************************
**
** Comparison function to sort Ensembl Exons (addresses) in ascending order.
**
** @param [r] P1 [const void*] Ensembl Exon 1
** @param [r] P2 [const void*] Ensembl Exon 2
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int geneCompareExon(const void *P1, const void *P2)
{
    int value = 0;
    
    const EnsPExon exon1 = NULL;
    const EnsPExon exon2 = NULL;
    
    if(!P1)
	return 0;
    
    if(!P2)
	return 0;
    
    exon1 = *(EnsPExon const *) P1;
    
    exon2 = *(EnsPExon const *) P2;
    
    /*
     ajDebug("geneCompareExon exon1 %p exon2 %p\n", exon1, exon2);
     */
    
    /*
     ** FIXME: Currently this function evaluates only Exon memory addresses.
     ** In theory, it could be used to order Exons on the Slice.
     */
    
    if(exon1 < exon2)
	value = -1;
    
    if(exon1 == exon2)
	value = 0;
    
    if(exon1 > exon2)
	value = +1;
    
    return value;
}




/* @funcstatic geneDeleteExon *************************************************
**
** ajListSortUnique nodedelete function to delete Ensembl Exons that are
** redundant.
**
** @param [r] PP1 [void**] Ensembl Exon address 1
** @param [r] cl [void*] Standard, passed in from ajListSortUnique
**
** @return [void]
** @@
******************************************************************************/

static void geneDeleteExon(void **PP1, void *cl)
{
    if(!PP1)
	return;
    
    (void) cl;
    
    ensExonDel((EnsPExon *) PP1);
    
    return;
}




/* @func ensGeneFetchAllExons *************************************************
**
** Fetch all Ensembl Exons of an Ensembl Gene.
**
** @cc Bio::EnsEMBL::Gene::get_all_Exons
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] exons [AjPList] AJAX List of Ensembl Exons
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneFetchAllExons(EnsPGene gene, AjPList exons)
{
    const AjPList elist = NULL;
    const AjPList tlist = NULL;
    
    AjIList eiter = NULL;
    AjIList titer = NULL;
    
    EnsPExon exon = NULL;
    
    EnsPTranscript transcript = NULL;
    
    if(!gene)
	return ajFalse;
    
    if(!exons)
	return ajFalse;
    
    tlist = ensGeneGetTranscripts(gene);
    
    titer = ajListIterNewread(tlist);
    
    while(!ajListIterDone(titer))
    {
	transcript = (EnsPTranscript) ajListIterGet(titer);
	
	elist = ensTranscriptGetExons(transcript);
	
	eiter = ajListIterNewread(elist);
	
	while(!ajListIterDone(eiter))
	{
	    exon = (EnsPExon) ajListIterGet(eiter);
	    
	    ajListPushAppend(exons, (void *) ensExonNewRef(exon));
	}
	
	ajListIterDel(&eiter);
    }
    
    ajListIterDel(&titer);
    
    ajListSortUnique(exons, geneCompareExon, geneDeleteExon);
    
    return ajTrue;
}




/* @func ensGeneTransform *****************************************************
**
** Transform an Ensembl Gene into another Ensembl Coordinate System.
**
** @cc Bio::EnsEMBL::Gene::transform
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [r] csname [const AjPStr] Ensembl Coordinate System name
** @param [r] csversion [const AjPStr] Ensembl Coordinate System version
** @see ensFeatureTransform
**
** @return [EnsPGene] Ensembl Gene or NULL
** @@
******************************************************************************/

EnsPGene ensGeneTransform(EnsPGene gene, const AjPStr csname,
                          const AjPStr csversion)
{
    ajint failures = 0;
    ajint minstart = INT_MAX;
    ajint maxend   = INT_MIN;
    ajint strand   = 0;
    
    ajuint pslength = 0;
    
    AjIList iter        = NULL;
    AjPList pslist      = NULL;
    AjPList transcripts = NULL;
    
    EnsPAnalysis analysis = NULL;
    
    EnsPFeature nfeature = NULL;
    EnsPFeature tfeature = NULL;
    
    EnsPGene newgene = NULL;
    
    EnsPProjectionSegment ps = NULL;
    
    EnsPTranscript newtranscript = NULL;
    EnsPTranscript oldtranscript = NULL;
    
    EnsPSlice slice = NULL;
    
    if(!gene)
	return NULL;
    
    if(!csname)
	return NULL;
    
    if(ajStrGetLen(csname) == 0)
	return NULL;
    
    nfeature = ensFeatureTransform(gene->Feature, csname, csversion);
    
    if(!nfeature)
    {
	/*
	** Check if this Gene projects at all to the requested
	** Coordinate System.
	*/
	
	pslist = ajListNew();
	
	ensFeatureProject(gene->Feature, csname, csversion, pslist);
	
	pslength = ajListGetLength(pslist);
	
	while(ajListPop(pslist, (void **) &ps))
	    ensProjectionSegmentDel(&ps);
	
	ajListFree(&pslist);
	
	if(pslength == 0)
	    return NULL;
	else
	    ensGeneGetTranscripts(gene);
    }
    
    if(gene->Transcripts)
    {
	transcripts = ajListNew();
	
	iter = ajListIterNew(gene->Transcripts);
	
	while(!ajListIterDone(iter))
	{
	    oldtranscript = (EnsPTranscript) ajListIterGet(iter);
	    
	    newtranscript =
		ensTranscriptTransform(oldtranscript, csname, csversion);
	    
	    if(!newtranscript)
	    {
		failures++;
		
		continue;
	    }
	    
	    if(!nfeature)
	    {
		tfeature = ensTranscriptGetFeature(newtranscript);
		
		if(ensFeatureGetStart(tfeature) < minstart)
		    minstart = ensFeatureGetStart(tfeature);
		
		if(ensFeatureGetEnd(tfeature) > maxend)
		    maxend = ensFeatureGetEnd(tfeature);
		
		strand = ensFeatureGetStrand(tfeature);
		
		slice = ensFeatureGetSlice(tfeature);
	    }
	    
	    ajListPushAppend(transcripts, (void *) newtranscript);
	}
	
	ajListIterDel(&iter);
    }
    
    if(failures)
    {
	while(ajListPop(transcripts, (void **) &newtranscript))
	    ensTranscriptDel(&newtranscript);
	
	ajListFree(&transcripts);
	
	return NULL;
    }
    
    if(!nfeature)
    {
	analysis = ensFeatureGetAnalysis(gene->Feature);
	
	nfeature = ensFeatureNewS(analysis, slice, minstart, maxend, strand);
    }
    
    newgene = ensGeneNewObj(gene);
    
    /* Set the new Feature. */
    
    ensFeatureDel(&(newgene->Feature));
    
    newgene->Feature = nfeature;
    
    /* Set the new Transcripts. */
    
    while(ajListPop(newgene->Transcripts, (void **) &oldtranscript))
	ensTranscriptDel(&oldtranscript);
    
    ajListFree(&(newgene->Transcripts));
    
    newgene->Transcripts = transcripts;
    
    return newgene;
}




/* @func ensGeneTransfer ******************************************************
**
** Transfer an Ensembl Gene onto another Ensembl Slice.
**
** @cc Bio::EnsEMBL::Gene::transfer
** @param [u] gene [EnsPGene] Ensembl Gene
** @param [u] slice [EnsPSlice] Ensembl Slice
** @see ensFeatureTransfer
**
** @return [EnsPGene] Ensembl Gene or NULL
** @@
******************************************************************************/

EnsPGene ensGeneTransfer(EnsPGene gene, EnsPSlice slice)
{
    EnsPFeature newfeature = NULL;
    
    EnsPGene newgene = NULL;
    
    ajDebug("ensGeneTransfer\n"
	    "  gene %p\n"
	    "  slice %p\n",
	    gene,
	    slice);
    
    if(!gene)
	return NULL;
    
    if(!slice)
	return NULL;
    
    newfeature = ensFeatureTransfer(gene->Feature, slice);
    
    if(!newfeature)
    {
	ajDebug("ensGeneTransfer could not transfer the Ensembl Feature %p "
		"onto the new Slice %p.\n", gene->Feature, slice);
	
	ensFeatureTrace(gene->Feature, 1);
	
	ensSliceTrace(slice, 1);
	
	return NULL;
    }
    
    newgene = ensGeneNewObj(gene);
    
    ensGeneSetFeature(newgene, newfeature);
    
    ensFeatureDel(&newfeature);
    
    return newgene;
}




/* @datasection [EnsPGeneAdaptor] Gene Adaptor ********************************
**
** Functions for manipulating Ensembl Gene Adaptor objects
**
** @nam2rule GeneAdaptor
**
******************************************************************************/

static const char *geneAdaptorTables[] =
{
    "gene",
    "gene_stable_id",
    "xref",
    NULL
};




static const char *geneAdaptorColumns[] =
{
    "gene.gene_id",
    "gene.seq_region_id",
    "gene.seq_region_start",
    "gene.seq_region_end",
    "gene.seq_region_strand",
    "gene.analysis_id",
    "gene.display_xref_id",
    "gene.description",
    "gene.source",
    "gene.biotype",
    "gene.status",
    "gene.is_current",
    "gene_stable_id.stable_id",
    "gene_stable_id.version",
    "gene_stable_id.created_date",
    "gene_stable_id.modified_date",
    "xref.external_db_id",
    "xref.dbprimary_acc",
    "xref.display_label",
    "xref.version",
    "xref.description",
    "xref.info_type",
    "xref.info_text",
    NULL
};




static EnsOBaseAdaptorLeftJoin geneAdaptorLeftJoin[] =
{
    {"gene_stable_id", "gene.gene_id = gene_stable_id.gene_id"},
    {"xref", "gene.display_xref_id = xref.xref_id"},
    {NULL, NULL}
};




static const char *geneAdaptorDefaultCondition = NULL;

static const char *geneAdaptorFinalCondition = NULL;




/* @funcstatic geneAdaptorFetchAllBySQL ***************************************
**
** Fetch all Ensembl Gene objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] genes [AjPList] AJAX List of Ensembl Gene objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool geneAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                       const AjPStr statement,
                                       EnsPAssemblyMapper am,
                                       EnsPSlice slice,
                                       AjPList genes)
{
    ajuint identifier = 0;
    ajuint analysisid = 0;
    ajuint erid       = 0;
    ajuint edbid      = 0;
    
    ajuint srid    = 0;
    ajuint srstart = 0;
    ajuint srend   = 0;
    ajint srstrand = 0;
    
    ajint slstart  = 0;
    ajint slend    = 0;
    ajint slstrand = 0;
    ajint sllength = 0;
    
    ajuint version = 0;
    
    AjBool current = AJFALSE;
    
    AjEnum estatus   = ensEGeneStatusNULL;
    AjEnum einfotype = ensEExternalReferenceInfoTypeNULL;
    
    AjPList mrs = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr description   = NULL;
    AjPStr source        = NULL;
    AjPStr biotype       = NULL;
    AjPStr status        = NULL;
    AjPStr stableid      = NULL;
    AjPStr cdate         = NULL;
    AjPStr mdate         = NULL;
    AjPStr erprimaryid   = NULL;
    AjPStr erdisplayid   = NULL;
    AjPStr erversion     = NULL;
    AjPStr erdescription = NULL;
    AjPStr erinfotype    = NULL;
    AjPStr erinfotext    = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisAdaptor aa = NULL;
    
    EnsPAssemblyMapperAdaptor ama = NULL;
    
    EnsPDatabaseEntry dbe = NULL;
    
    EnsPExternalDatabase edb         = NULL;
    EnsPExternalDatabaseAdaptor edba = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPGene gene      = NULL;
    EnsPGeneAdaptor ga = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceAdaptor sa = NULL;
    
    /*
     ajDebug("geneAdaptorFetchAllBySQL\n"
	     "  dba %p\n"
	     "  statement %p\n"
	     "  am %p\n"
	     "  slice %p\n"
	     "  genes %p\n",
	     dba,
	     statement,
	     am,
	     slice,
	     genes);
     */
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!genes)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisAdaptor(dba);
    
    edba = ensRegistryGetExternalDatabaseAdaptor(dba);
    
    sa = ensRegistryGetSliceAdaptor(dba);
    
    ga = ensRegistryGetGeneAdaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperAdaptor(dba);
    
    mrs = ajListNew();
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier    = 0;
	srid          = 0;
	srstart       = 0;
	srend         = 0;
	srstrand      = 0;
	analysisid    = 0;
	erid          = 0;
	description   = ajStrNew();
	source        = ajStrNew();
	biotype       = ajStrNew();
	status        = ajStrNew();
	current       = ajFalse;
	stableid      = ajStrNew();
	version       = 0;
	cdate         = ajStrNew();
	mdate         = ajStrNew();
	edbid         = 0;
	erprimaryid   = ajStrNew();
	erdisplayid   = ajStrNew();
	erversion     = ajStrNew();
	erdescription = ajStrNew();
	erinfotype    = ajStrNew();
	erinfotext    = ajStrNew();
	
	estatus   = ensEGeneStatusNULL;
	einfotype = ensEExternalReferenceInfoTypeNULL;
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToUint(sqlr, &identifier);
	ajSqlColumnToUint(sqlr, &srid);
	ajSqlColumnToUint(sqlr, &srstart);
	ajSqlColumnToUint(sqlr, &srend);
	ajSqlColumnToInt(sqlr, &srstrand);
	ajSqlColumnToUint(sqlr, &analysisid);
	ajSqlColumnToUint(sqlr, &erid);
        ajSqlColumnToStr(sqlr, &description);
	ajSqlColumnToStr(sqlr, &source);
        ajSqlColumnToStr(sqlr, &biotype);
        ajSqlColumnToStr(sqlr, &status);
        ajSqlColumnToBool(sqlr, &current);
        ajSqlColumnToStr(sqlr, &stableid);
	ajSqlColumnToUint(sqlr, &version);
        ajSqlColumnToStr(sqlr, &cdate);
        ajSqlColumnToStr(sqlr, &mdate);
	ajSqlColumnToUint(sqlr, &edbid);
        ajSqlColumnToStr(sqlr, &erprimaryid);
	ajSqlColumnToStr(sqlr, &erdisplayid);
	ajSqlColumnToStr(sqlr, &erversion);
	ajSqlColumnToStr(sqlr, &erdescription);
        ajSqlColumnToStr(sqlr, &erinfotype);
	ajSqlColumnToStr(sqlr, &erinfotext);
	
	/*
	** Since the Ensembl SQL schema defines Sequence Region start and end
	** coordinates as unsigned integers for all Features, the range needs
	** checking.
	*/
	
	if(srstart <= INT_MAX)
	    slstart = (ajint) srstart;
	else
	    ajFatal("geneAdaptorFetchAllBySQL got a "
		    "Sequence Region start coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srstart, INT_MAX);
	
	if(srend <= INT_MAX)
	    slend = (ajint) srend;
	else
	    ajFatal("geneAdaptorFetchAllBySQL got a "
		    "Sequence Region end coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srend, INT_MAX);
	
	slstrand = srstrand;
	
	/* Fetch a Slice spanning the entire Sequence Region. */
	
	ensSliceAdaptorFetchBySeqRegionIdentifier(sa, srid, 0, 0, 0, &srslice);
	
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
		ensAssemblyMapperAdaptorFetchByCoordSystems(
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
		
		ajStrDel(&description);
		ajStrDel(&source);
		ajStrDel(&biotype);
		ajStrDel(&status);
		ajStrDel(&stableid);
		ajStrDel(&cdate);
		ajStrDel(&mdate);
		ajStrDel(&erprimaryid);
		ajStrDel(&erdisplayid);
		ajStrDel(&erversion);
		ajStrDel(&erdescription);
		ajStrDel(&erinfotype);
		ajStrDel(&erinfotext);
		
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
	    
	    ensSliceAdaptorFetchBySeqRegionIdentifier(sa,
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
		ajFatal("geneAdaptorFetchAllBySQL got a Slice, "
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
		
		ajStrDel(&description);
		ajStrDel(&source);
		ajStrDel(&biotype);
		ajStrDel(&status);
		ajStrDel(&stableid);
		ajStrDel(&cdate);
		ajStrDel(&mdate);
		ajStrDel(&erprimaryid);
		ajStrDel(&erdisplayid);
		ajStrDel(&erversion);
		ajStrDel(&erdescription);
		ajStrDel(&erinfotype);
		ajStrDel(&erinfotext);
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		continue;
	    }
	    
	    /* Delete the Sequence Region Slice and set the requested Slice. */
	    
	    ensSliceDel(&srslice);
	    
	    srslice = ensSliceNewRef(slice);
	}
	
	if(erid)
	{
	    ensExternalDatabaseAdaptorFetchByIdentifier(edba, edbid, &edb);
	    
	    einfotype = ensExternalReferenceInfoTypeFromStr(erinfotype);
	    
	    /* FIXME: The homo_sapiens_core_51_36m database has NULL here.
		if(!einfotype)
		ajFatal("geneAdaptorFetchAllBySQL encountered "
			"unexpected string '%S' in the "
			"'xref.infotype' field.\n", erinfotype);
	    */
	    
	    dbe = ensDatabaseEntryNew((EnsPDatabaseEntryAdaptor) NULL,
				      erid,
				      (EnsPAnalysis) NULL,
				      edb,
				      erprimaryid,
				      erdisplayid,
				      erversion,
				      erdescription,
				      (AjPStr) NULL,
				      einfotype,
				      erinfotext);
	    
	    ensExternalDatabaseDel(&edb);	
	}
	
	ensAnalysisAdaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	/* Set the Gene status. */
	
	estatus = ensGeneStatusFromStr(status);
	
	if(!estatus)
	    ajFatal("geneAdaptorFetchAllBySQL encountered "
		    "unexpected string '%S' in the "
		    "'gene.status' field.\n", status);
	
	/* Finally, create a new Ensembl Gene. */
	
	feature = ensFeatureNewS(analysis,
				 srslice,
				 slstart,
				 slend,
				 slstrand);
	
	gene = ensGeneNew(ga,
			  identifier,
			  feature,
			  dbe,
			  description,
			  source,
			  biotype,
			  estatus,
			  current,
			  stableid,
			  version,
			  cdate,
			  mdate,
			  (AjPList) NULL);
	
	ensFeatureDel(&feature);
	
	ajListPushAppend(genes, (void *) gene);	
	
	ajStrDel(&description);
	ajStrDel(&source);
	ajStrDel(&biotype);
	ajStrDel(&status);
	ajStrDel(&stableid);
	ajStrDel(&cdate);
	ajStrDel(&mdate);
	ajStrDel(&erprimaryid);
	ajStrDel(&erdisplayid);
	ajStrDel(&erversion);
	ajStrDel(&erdescription);
	ajStrDel(&erinfotype);
	ajStrDel(&erinfotext);
	
	ensAnalysisDel(&analysis);
	
	ensDatabaseEntryDel(&dbe);
	
	ensSliceDel(&srslice);
	
	ensAssemblyMapperDel(&am);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajListFree(&mrs);
    
    return ajTrue;
}




/* @funcstatic geneAdaptorCacheReference **************************************
**
** Wrapper function to reference an Ensembl Gene from an Ensembl Cache.
**
** @param [r] value [void *] Ensembl Gene
**
** @return [void *] Ensembl Gene or NULL
** @@
******************************************************************************/

static void *geneAdaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *) ensGeneNewRef((EnsPGene) value);
}




/* @funcstatic geneAdaptorCacheDelete *****************************************
**
** Wrapper function to delete an Ensembl Gene from an Ensembl Cache.
**
** @param [r] value [void **] Ensembl Gene address
**
** @return [void]
** @@
******************************************************************************/

static void geneAdaptorCacheDelete(void **value)
{
    if(!value)
	return;
    
    ensGeneDel((EnsPGene *) value);
    
    return;
}




/* @funcstatic geneAdaptorCacheSize *******************************************
**
** Wrapper function to determine the memory size of an Ensembl Gene
** from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Gene
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint geneAdaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensGeneGetMemSize((const EnsPGene) value);
}




/* @funcstatic geneAdaptorGetFeature ******************************************
**
** Wrapper function to get the Ensembl Feature of an Ensembl Gene
** from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl Gene
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature geneAdaptorGetFeature(const void *value)
{
    if(!value)
	return NULL;
    
    return ensGeneGetFeature((const EnsPGene) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Gene Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Gene Adaptor. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGeneAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPGeneAdaptor] Ensembl Gene Adaptor
** @argrule Ref object [EnsPGeneAdaptor] Ensembl Gene Adaptor
**
** @valrule * [EnsPGeneAdaptor] Ensembl Gene Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensGeneAdaptorNew ****************************************************
**
** Default Ensembl Gene Adaptor constructor.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGeneAdaptor] Ensembl Gene Adaptor or NULL
** @@
******************************************************************************/

EnsPGeneAdaptor ensGeneAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPGeneAdaptor ga = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(ga);
    
    ga->Adaptor = ensFeatureAdaptorNew(dba,
				       geneAdaptorTables,
				       geneAdaptorColumns,
				       geneAdaptorLeftJoin,
				       geneAdaptorDefaultCondition,
				       geneAdaptorFinalCondition,
				       geneAdaptorFetchAllBySQL,
                                        /* Fread */
				       (void* (*)(const void* key)) NULL,
				       geneAdaptorCacheReference,
                                        /* Fwrite */
				       (AjBool (*)(const void* value)) NULL,
				       geneAdaptorCacheDelete,
				       geneAdaptorCacheSize,
				       geneAdaptorGetFeature,
				       "Gene");
    
    return ga;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Gene Adaptor.
**
** @fdata [EnsPGeneAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Gene Adaptor object
**
** @argrule * Padaptor [EnsPGeneAdaptor*] Ensembl Gene Adaptor
**                                        object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGeneAdaptorDel ****************************************************
**
** Default destructor for an Ensembl Gene Adaptor.
**
** @param [d] Padaptor [EnsPGeneAdaptor*] Ensembl Gene Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGeneAdaptorDel(EnsPGeneAdaptor *Padaptor)
{
    EnsPGeneAdaptor pthis = NULL;
    
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;

    pthis = *Padaptor;
    
    ensFeatureAdaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Gene Adaptor object.
**
** @fdata [EnsPGeneAdaptor]
** @fnote None
**
** @nam3rule Get Return Ensembl Gene Adaptor attribute(s)
** @nam4rule GetFeatureAdaptor Return the Ensembl Feature Adaptor
** @nam4rule GetDatabaseAdaptor Return the Ensembl Database Adaptor
**
** @argrule * adaptor [const EnsPGeneAdaptor] Ensembl Gene Adaptor
**
** @valrule FeatureAdaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @valrule DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensGeneAdaptorGetFeatureAdaptor **************************************
**
** Get the Ensembl Feature Adaptor element of an Ensembl Gene Adaptor.
**
** @param [r] adaptor [const EnsPGeneAdaptor] Ensembl Gene Adaptor
**
** @return [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

EnsPFeatureAdaptor ensGeneAdaptorGetFeatureAdaptor(
    const EnsPGeneAdaptor adaptor)
{
    if(!adaptor)
	return NULL;
    
    return adaptor->Adaptor;
}




/* @func ensGeneAdaptorGetDatabaseAdaptor *************************************
**
** Get the Ensembl Database Adaptor element of the
** Ensembl Feature Adaptor element of an Ensembl Gene Adaptor.
**
** @param [r] adaptor [const EnsPGeneAdaptor] Ensembl Gene Adaptor
**
** @return [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

EnsPDatabaseAdaptor ensGeneAdaptorGetDatabaseAdaptor(
    const EnsPGeneAdaptor adaptor)
{
    if(!adaptor)
	return NULL;
    
    return ensFeatureAdaptorGetDatabaseAdaptor(adaptor->Adaptor);
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Gene objects from an
** Ensembl Core database.
**
** @fdata [EnsPGeneAdaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Gene object(s)
** @nam4rule FetchAll Retrieve all Ensembl Gene objects
** @nam5rule FetchAllBy Retrieve all Ensembl Gene objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Gene object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPGeneAdaptor] Ensembl Gene Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Gene objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ensGeneAdaptorFetchAll ***********************************************
**
** Fetch all Ensembl Genes.
**
** @param [r] adaptor [const EnsPGeneAdaptor] Ensembl Gene Adaptor
** @param [u] genes [AjPList] AJAX List of Ensembl Genes
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneAdaptorFetchAll(EnsPGeneAdaptor adaptor, AjPList genes)
{
    EnsPBaseAdaptor ba = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!genes)
	return ajFalse;
    
    ba = ensFeatureAdaptorGetBaseAdaptor(adaptor->Adaptor);
    
    return ensBaseAdaptorFetchAll(ba, genes);
}




/* @func ensGeneAdaptorFetchByIdentifier **************************************
**
** Fetch an Ensembl Gene via its SQL database-internal identifier.
** The caller is responsible for deleting the Ensembl Gene.
**
** @param [r] adaptor [EnsPGeneAdaptor] Ensembl Gene Adaptor
** @param [r] identifier [ajuint] SQL database-internal Gene identifier
** @param [wP] Pgene [EnsPGene*] Ensembl Gene address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneAdaptorFetchByIdentifier(EnsPGeneAdaptor adaptor,
                                       ajuint identifier,
                                       EnsPGene *Pgene)
{
    EnsPBaseAdaptor ba = NULL;
    
    EnsPCache cache = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pgene)
	return ajFalse;
    
    cache = ensFeatureAdaptorGetCache(adaptor->Adaptor);
    
    *Pgene = (EnsPGene) ensCacheFetch(cache, (void *) &identifier);
    
    if(*Pgene)
	return ajTrue;
    
    ba = ensFeatureAdaptorGetBaseAdaptor(adaptor->Adaptor);
    
    *Pgene = (EnsPGene) ensBaseAdaptorFetchByIdentifier(ba, identifier);
    
    ensCacheStore(cache, (void *) &identifier, (void **) Pgene);
    
    return ajTrue;
}




/* @func ensGeneAdaptorFetchByStableIdentifier ********************************
**
** Fetch an Ensembl Gene via its stable identifier and version.
** In case a particular version is not specified,
** the current Ensembl Gene will be returned.
** The caller is responsible for deleting the Ensembl Gene.
**
** @param [r] adaptor [EnsPGeneAdaptor] Ensembl Gene Adaptor
** @param [r] stableid [const AjPStr] Stable identifier
** @param [r] version [ajuint] Version
** @param [wP] Pgene [EnsPGene*] Ensembl Gene address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGeneAdaptorFetchByStableIdentifier(EnsPGeneAdaptor adaptor,
                                             const AjPStr stableid,
                                             ajuint version,
                                             EnsPGene *Pgene)
{
    char *txtstableid = NULL;
    
    AjPList genes = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPGene gene = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!stableid)
	return ajFalse;
    
    if(!Pgene)
	return ajFalse;
    
    ba = ensFeatureAdaptorGetBaseAdaptor(adaptor->Adaptor);
    
    ensBaseAdaptorEscapeCS(ba, &txtstableid, stableid);
    
    if(version)
	constraint =
	    ajFmtStr("gene_stable_id.stable_id = '%s' "
		     "AND "
		     "gene_stable_id.version = %u",
		     txtstableid,
		     version);
    else
	constraint =
	    ajFmtStr("gene_stable_id.stable_id = '%s' "
		     "AND "
		     "gene.is_current = 1",
		     txtstableid);
    
    ajCharDel(&txtstableid);
    
    genes = ajListNew();
    
    ensBaseAdaptorGenericFetch(ba,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       genes);
    
    if(ajListGetLength(genes) > 1)
	ajDebug("ensGeneAdaptorFetchByStableIdentifier got more than one "
		"Gene for stable identifier '%S' and version %u.\n",
		stableid,
		version);
    
    ajListPop(genes, (void **) Pgene);
    
    /*
     ajDebug("ensGeneAdaptorFetchByStableIdentifier got Gene %p\n", *Pgene);
     
     ensGeneTrace(*Pgene, 1);
     */
    
    while(ajListPop(genes, (void **) &gene))
	ensGeneDel(&gene);
    
    ajListFree(&genes);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}
