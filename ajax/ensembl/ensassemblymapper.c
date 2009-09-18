/******************************************************************************
** @source Ensembl Assembly Mapper functions
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

#include "ensassemblymapper.h"
#include "ensseqregion.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

const ajuint assemblyMapperMaxPairCount = 1000;

/*
** The Assembly Mapper registers Sequence Regions in chunks of 2 ^ 20,
** which corresponds to approximately 10 ^ 6 or 1 M bps.
*/

const ajuint assemblyMapperChunkFactor = 20;




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPCoordSystemAdaptor
ensRegistryGetCoordSystemAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPSeqRegionAdaptor
ensRegistryGetSeqRegionAdaptor(EnsPDatabaseAdaptor dba);

static AjBool assemblyMapperAdaptorHasMultipleMappings(
    const EnsPAssemblyMapperAdaptor ama,
    ajuint srid);

static AjBool assemblyMapperAdaptorMultipleMappingsInit(
    EnsPAssemblyMapperAdaptor ama);

static AjBool assemblyMapperAdaptorMultipleMappingsExit(
    EnsPAssemblyMapperAdaptor ama);

static AjBool assemblyMapperAdaptorBuildCombinedMapper(
    EnsPAssemblyMapperAdaptor ama,
    AjPList ranges,
    EnsPMapper srcmidmapper,
    EnsPMapper trgmidmapper,
    EnsPMapper srctrgmapper,
    const AjPStr srctype);




/* @filesection ensassemblymapper *********************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPGenericAssemblyMapper] Generic Assembly Mapper ***********
**
** Functions for manipulating Ensembl Generic Assembly Mapper objects
**
** @cc Bio::EnsEMBL::AssemblyMapper CVS Revision: 1.42
**
** @nam2rule GenericAssemblyMapper
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Generic Assembly Mapper by pointer.
** It is the responsibility of the user to first destroy any previous
** Generic Assembly Mapper. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGenericAssemblyMapper]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGenericAssemblyMapper] Ensembl Generic
**                                                 Assembly Mapper
** @argrule Ref object [EnsPGenericAssemblyMapper] Ensembl Generic
**                                                 Assembly Mapper
**
** @valrule * [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
**
** @fcategory new
******************************************************************************/




/* @func ensGenericAssemblyMapperNew ******************************************
**
** Default constructor for an Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::new
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] asmcs [EnsPCoordSystem] Assembled Ensembl Coordinate System
** @param [u] cmpcs [EnsPCoordSystem] Component Ensembl Coordinate System
**
** @return [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper or NULL
** @@
******************************************************************************/

EnsPGenericAssemblyMapper ensGenericAssemblyMapperNew(
    EnsPAssemblyMapperAdaptor ama,
    EnsPCoordSystem asmcs,
    EnsPCoordSystem cmpcs)
{
    AjPStr srcname = NULL;
    AjPStr trgname = NULL;
    
    EnsPGenericAssemblyMapper gam = NULL;
    
    /*
     ajDebug("ensGenericAssemblyMapperNew\n"
	     "  ama %p\n"
	     "  asmcs %p\n"
	     "  cmpcs %p\n",
	     ama,
	     asmcs,
	     cmpcs);
     
     ensCoordSystemTrace(asmcs, 1);
     
     ensCoordSystemTrace(cmpcs, 1);
     */
    
    if(!ama)
	return NULL;
    
    if(!asmcs)
	return NULL;
    
    if(!cmpcs)
	return NULL;
    
    srcname = ajStrNewC("assembled");
    
    trgname = ajStrNewC("component");
    
    AJNEW0(gam);
    
    gam->Adaptor        = ama;
    gam->AsmCoordSystem = ensCoordSystemNewRef(asmcs);
    gam->CmpCoordSystem = ensCoordSystemNewRef(cmpcs);
    
    gam->AsmRegister =
	ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    gam->CmpRegister =
	ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    gam->Mapper       = ensMapperNew(srcname, trgname, asmcs, cmpcs);
    gam->MaxPairCount = assemblyMapperMaxPairCount;
    
    gam->Use = 1;
    
    ajStrDel(&srcname);
    
    ajStrDel(&trgname);
    
    return gam;
}




/* @func ensGenericAssemblyMapperNewRef ***************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
**
** @return [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper or NULL
** @@
******************************************************************************/

EnsPGenericAssemblyMapper ensGenericAssemblyMapperNewRef(
    EnsPGenericAssemblyMapper gam)
{
    if(!gam)
	return NULL;
    
    gam->Use++;
    
    return gam;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Generic Assembly Mapper.
**
** @fdata [EnsPGenericAssemblyMapper]
** @fnote None
**
** @nam3rule Del Destroy (free) an EnsPGenericAssemblyMapper object
**
** @argrule * Pgam [EnsPGenericAssemblyMapper*] Generic Assembly Mapper
**                                              object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGenericAssemblyMapperClear ****************************************
**
** Clear an Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::flush
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperClear(EnsPGenericAssemblyMapper gam)
{
    void **keyarray1 = NULL;
    void **valarray1 = NULL;
    void **keyarray2 = NULL;
    void **valarray2 = NULL;
    
    register ajint i = 0;
    register ajint j = 0;
    
    /*
     ajDebug("ensGenericAssemblyMapperClear\n"
	     "  gam %p\n",
	     gam);
     */
    
    if(!gam)
	return ajFalse;
    
    ajTableToarray(gam->AsmRegister, &keyarray1, &valarray1);
    
    /*
     ** Free the Assembled Sequence Region identifiers and the second-level
     ** AJAX Tables from the first-level Table. The first-level Table is not
     ** deleted here but in the ensGenericAssemblyMapperDel function.
     */
    
    for(i = 0; keyarray1[i]; i++)
    {
	ajTableRemove(gam->AsmRegister, (const void *) keyarray1[i]);
	AJFREE(keyarray1[i]);
	ajTableToarray((AjPTable) valarray1[i], &keyarray2, &valarray2);
	
	/*
	 ** Free the Assembled Sequence Region chunk identifiers and the
	 ** flags from the second-level Table.
	 */
	
	for(j = 0; keyarray2[j]; j++)
	{
	    AJFREE(keyarray2[j]);
	    AJFREE(valarray2[j]);
	}
	
	AJFREE(keyarray2);
	AJFREE(valarray2);
	
	/* Free the second-level AJAX Tables. */
	
	ajTableFree((AjPTable *) &valarray1[i]);
    }
    
    AJFREE(keyarray1);
    AJFREE(valarray1);
    
    ajTableToarray(gam->CmpRegister, &keyarray1, &valarray1);
    
    /* Free the Component Sequence Region identifiers and the AjBool flags. */
    
    for(i = 0; keyarray1[i]; i++)
    {
	ajTableRemove(gam->CmpRegister, (const void *) keyarray1[i]);
	
	AJFREE(keyarray1[i]);
	AJFREE(valarray1[i]);
    }
    
    AJFREE(keyarray1);
    AJFREE(valarray1);
    
    ensMapperClear(gam->Mapper);
    
    return ajTrue;
}




/* @func ensGenericAssemblyMapperDel ******************************************
**
** Default destructor for an Ensembl Generic Assembly Mapper.
**
** @param [d] Pgam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
**                                             address
**
** @return [void]
** @@
******************************************************************************/

void ensGenericAssemblyMapperDel(EnsPGenericAssemblyMapper* Pgam)
{
    EnsPGenericAssemblyMapper pthis = NULL;
    
    if(!Pgam)
	return;
    
    if(!*Pgam)
	return;
    
    /*
     ajDebug("ensGenericAssemblyMapperDel\n"
	     "  *Pgam %p\n",
	     *Pgam);
     */

    pthis = *Pgam;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgam = NULL;
	
	return;
    }
    
    ensGenericAssemblyMapperClear(pthis);
    
    ensCoordSystemDel(&pthis->AsmCoordSystem);
    
    ensCoordSystemDel(&pthis->CmpCoordSystem);
    
    ajTableFree(&pthis->AsmRegister);
    
    ajTableFree(&pthis->CmpRegister);
    
    ensMapperDel(&pthis->Mapper);
    
    AJFREE(*Pgam);

    *Pgam = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Generic Assembly Mapper object.
**
** @fdata [EnsPGenericAssemblyMapper]
** @fnote None
**
** @nam3rule Get Return Generic Assembly Mapper attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Assembly Mapper Adaptor
** @nam4rule GetAssembledCoordSystem Return the assembled Coordinate System
** @nam4rule GetComponentCoordSystem Return the component Coordinate System
** @nam4rule GetMapper Return the Ensembl Mapper
** @nam4rule GetMaxPairCount Return the maximum Ensembl Mapper Pair count
**
** @argrule * gam [const EnsPGenericAssemblyMapper] Generic Assembly Mapper
**
** @valrule Adaptor [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @valrule AssembledCoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @valrule ComponentCoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @valrule Mapper [EnsPMapper] Ensembl Mapper
** @valrule MaxPairCount [ajuint] Maximum Ensembl Mapper Pair count
**
** @fcategory use
******************************************************************************/




/* @func ensGenericAssemblyMapperGetAdaptor ***********************************
**
** Get the Ensembl Assembly Mapper Adaptor element of an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::adaptor
** @param [r] gam [const EnsPGenericAssemblyMapper] Ensembl Generic
**                                                  Assembly Mapper
**
** @return [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyMapperAdaptor ensGenericAssemblyMapperGetAdaptor(
    const EnsPGenericAssemblyMapper gam)
{
    if(!gam)
        return NULL;
    
    return gam->Adaptor;
}




/* @func ensGenericAssemblyMapperGetAssembledCoordinateSystem *****************
**
** Get the assembled Ensembl Coordinate System element of an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::assembled_CoordSystem
** @param [r] gam [const EnsPGenericAssemblyMapper] Ensembl Generic
**                                                  Assembly Mapper
**
** @return [const EnsPCoordSystem] Ensembl Coordinate System or NULL
** @@
******************************************************************************/

const EnsPCoordSystem ensGenericAssemblyMapperGetAssembledCoordSystem(
    const EnsPGenericAssemblyMapper gam)
{
    if(!gam)
	return NULL;
    
    return gam->AsmCoordSystem;
}




/* @func ensAssemblyGenericMapperGetComponentCoordinateSystem *****************
**
** Get the component Ensembl Coordinate System element of an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::component_CoordSystem
** @param [r] gam [const EnsPAssemblyMapper] Ensembl Generic Assembly Mapper
**
** @return [const EnsPCoordSystem] Ensembl Coordinate System or NULL
** @@
******************************************************************************/

const EnsPCoordSystem ensGenericAssemblyMapperGetComponentCoordSystem(
    const EnsPGenericAssemblyMapper gam)
{
    if(!gam)
	return NULL;
    
    return gam->CmpCoordSystem;
}




/* @func ensGenericAssemblyMapperGetMapper ************************************
**
** Get the Ensembl Mapper element of an Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::mapper
** @param [r] gam [const EnsPGenericAssemblyMapper] Ensembl Generic
**                                                  Assembly Mapper
**
** @return [EnsPMapper] Ensembl Mapper or NULL
** @@
******************************************************************************/

EnsPMapper ensGenericAssemblyMapperGetMapper(
    const EnsPGenericAssemblyMapper gam)
{
    if(!gam)
	return NULL;
    
    return gam->Mapper;
}




/* @func ensGenericAssemblyMapperGetMaxPairCount ******************************
**
** Get the maximum Ensembl Mapper Pair count element of an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::max_pair_count
** @param [r] gam [const EnsPGenericAssemblyMapper] Ensembl Generic
**                                                  Assembly Mapper
**
** @return [ajuint] Maximum Ensembl Mapper Pair count or 0
** @@
******************************************************************************/

ajuint ensGenericAssemblyMapperGetMaxPairCount(
    const EnsPGenericAssemblyMapper gam)
{
    if(!gam)
	return 0;
    
    return gam->MaxPairCount;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Generic Assembly Mapper object.
**
** @fdata [EnsPGenericAssemblyMapper]
** @fnote None
**
** @nam3rule Set Set one element of a Generic Assembly Mapper
** @nam4rule SetAdaptor Set the Ensembl Assembly Mapper
** @nam4rule SetMaxPairCount Set the maximum Ensembl Mapper Pair count
**
** @argrule * gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly
**                                            Mapper object
**
** @valrule * [AjBool]
**
** @fcategory modify
******************************************************************************/




/* @func ensGenericAssemblyMapperSetAdaptor ***********************************
**
** Set the Adaptor element of an Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::adaptor
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperSetAdaptor(EnsPGenericAssemblyMapper gam,
                                          EnsPAssemblyMapperAdaptor ama)
{
    if(!gam)
        return ajFalse;
    
    if(!ama)
        return ajFalse;
    
    gam->Adaptor = ama;
    
    return ajTrue;
}




/* @func ensGenericAssemblyMapperSetMaxPairCount ******************************
**
** Set the maximum Ensembl Mapper Pair count element of an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::max_pair_count
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] count [ajuint] Maximum Ensembl Mapper Pair count
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperSetMaxPairCount(EnsPGenericAssemblyMapper gam,
                                               ajuint count)
{
    if(!gam)
	return ajFalse;
    
    gam->MaxPairCount = count;
    
    return ajTrue;
}




/* @func ensGenericAssemblyMapperGetPairCount *********************************
**
** Get the number of Ensembl Mapper Pairs stored in the Ensembl Mapper that is
** associated with an Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::size
** @param [r] gam [const EnsPGenericAssemblyMapper] Ensembl Generic
**                                                  Assembly Mapper
**
** @return [ajuint] Number of Ensembl Mapper Pairs or 0
** @@
******************************************************************************/

ajuint ensGenericAssemblyMapperGetPairCount(const EnsPGenericAssemblyMapper gam)
{
    if(!gam)
	return 0;
    
    return ensMapperGetPairCount(gam->Mapper);
}




/* @func ensGenericAssemblyMapperRegisterAssembled ****************************
**
** Register an Assembled Ensembl Sequence Region in an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::register_assembled
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] asmsrid [ajuint] Assembled Ensembl Sequence Region identifier
** @param [r] chunkid [ajuint] Chunk identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
 ** FIXME: The Perl API
 ** Bio::EnsEMBL::AssemblyMapper::register_assembled method requires a
 ** sequence region identifier not a sequence region name as in the POD.
 */

AjBool ensGenericAssemblyMapperRegisterAssembled(EnsPGenericAssemblyMapper gam,
                                                 ajuint asmsrid,
                                                 ajuint chunkid)
{
    ajuint *Pchunkid = NULL;
    ajuint *Pasmsrid = NULL;
    
    AjBool *Pflag  = NULL;
    
    AjPTable table = NULL;
    
    /*
     ajDebug("ensGenericAssemblyMapperRegisterAssembled\n"
	     "  gam %p\n"
	     "  asmsrid %u\n"
	     "  chunkid %u\n",
	     gam,
	     asmsrid,
	     chunkid);
     */
    
    if(!gam)
	return ajFalse;
    
    if(!asmsrid)
	return ajFalse;
    
    table = (AjPTable) ajTableFetch(gam->AsmRegister, (const void *) &asmsrid);
    
    if(!table)
    {
	AJNEW0(Pasmsrid);
	
	*Pasmsrid = asmsrid;
	
	table = ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
	
	ajTablePut(gam->AsmRegister, (void *) Pasmsrid, (void *) table);
    }
    
    if (ajTableFetch(table, (const void *) &chunkid))
	return ajTrue;
    
    else
    {
	AJNEW0(Pchunkid);
	
	*Pchunkid = chunkid;
	
	AJNEW0(Pflag);
	
	*Pflag = ajTrue;
	
	ajTablePut(table, (void *) Pchunkid, (void *) Pflag);
    }
    
    return ajTrue;
}




/* @func ensGenericAssemblyMapperRegisterComponent ****************************
**
** Register a Component Ensembl Sequence Region in an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::AssemblyMapper::register_component
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] cmpsrid [ajuint] Component Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
 ** FIXME: The Perl API
 ** Bio::EnsEMBL::AssemblyMapper::register_component method requires a
 ** sequence region identifier not a sequence region name as in the POD.
 */

AjBool ensGenericAssemblyMapperRegisterComponent(EnsPGenericAssemblyMapper gam,
                                                 ajuint cmpsrid)
{
    ajuint *Pcmpsrid = NULL;
    
    AjBool *Pold = NULL;
    AjBool *Pflag = NULL;
    
    /*
     ajDebug("ensGenericAssemblyMapperRegisterComponent\n"
	     "  gam %p\n"
	     "  cmpsrid %u\n",
	     gam,
	     cmpsrid);
     */
    
    if(!gam)
	return ajFalse;
    
    if(!cmpsrid)
	return ajFalse;
    
    Pold = (AjBool *) ajTableFetch(gam->CmpRegister, (const void *) &cmpsrid);
    
    if(Pold)
	return ajTrue;
    
    else
    {
	AJNEW0(Pcmpsrid);
	
	*Pcmpsrid = cmpsrid;
	
	AJNEW0(Pflag);
	
	*Pflag = ajTrue;
	
	ajTablePut(gam->CmpRegister, (void *) Pcmpsrid, (void *) Pflag);
    }
    
    return ajTrue;
}




/* @func ensGenericAssemblyMapperHaveRegisteredAssembled **********************
**
** Test whether an Ensembl Generic Assembly Mapper has an
** Ensembl Sequence Region in the cache of assembled sequence regions.
**
** @cc Bio::EnsEMBL::AssemblyMapper::have_registered_assembled
** @param [r] gam [const EnsPGenericAssemblyMapper] Ensembl Generic
**                                                  Assembly Mapper
** @param [r] asmsrid [ajuint] Assembled Ensembl Sequence Region identifier
** @param [r] chunkid [ajuint] Sequence Region chunk identifier
**
** @return [AjBool] ajTrue if the Sequence Region chunk is already registered
**                  in the assembled cache, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperHaveRegisteredAssembled(
    const EnsPGenericAssemblyMapper gam,
    ajuint asmsrid,
    ajuint chunkid)
{
    AjPTable table = NULL;
    
    /*
     ajDebug("ensGenericAssemblyMapperHaveRegisteredAssembled\n"
	     "  gam %p\n"
	     "  asmsrid %u\n"
	     "  chunkid %u\n",
	     gam,
	     asmsrid,
	     chunkid);
     */
    
    if(!gam)
	return ajFalse;
    
    if(!asmsrid)
	return ajFalse;
    
    /*
     ** Get the second-level AJAX table keyed on chunk identifers from the
     ** first-level table keyed on Ensembl Sequence Region identifiers.
     */
    
    table = (AjPTable) ajTableFetch(gam->AsmRegister, (const void *) &asmsrid);
    
    if (table)
    {
	if (ajTableFetch(table, (const void *) &chunkid))
	    return ajTrue;
	else
	    return ajFalse;
    }
    else
	return ajFalse;
}




/* @func ensGenericAssemblyMapperHaveRegisteredComponent **********************
**
** Test whether an Ensembl Enseric Assembly Mapper has an
** Ensembl Sequence Region in the cache of component sequence regions.
**
** @cc Bio::EnsEMBL::AssemblyMapper::have_registered_component
** @param [r] gam [const EnsPGenericAssemblyMapper] Ensembl Generic
**                                                  Assembly Mapper
** @param [r] cmpsrid [ajuint] Component Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue if the Sequence Region chunk is already registered
**                  in the component cache, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperHaveRegisteredComponent(
    const EnsPGenericAssemblyMapper gam,
    ajuint cmpsrid)
{
    /*
     ajDebug("ensGenericAssemblyMapperHaveRegisteredComponent\n"
	     "  gam %p\n"
	     "  cmpsrid %u\n",
	     gam,
	     cmpsrid);
     */
    
    if(!gam)
	return ajFalse;
    
    if(!cmpsrid)
	return ajFalse;
    
    if(ajTableFetch(gam->CmpRegister, (const void *) &cmpsrid))
	return ajTrue;

    return ajFalse;
}




/* @func ensGenericAssemblyMapperMap ******************************************
**
** Transform coordinates from one Ensembl Coordinate System to another by means
** of an Ensembl Generic Assembly Mapper.
**
** The caller is responsible for deleting the
** Ensembl Mapper Results before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::AssemblyMapper::map
** @param [r] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] sr [const EnsPSeqRegion] Ensembl Sequence Region
** @param [r] srstart [ajint] Sequence Region Start coordinate
** @param [r] srend [ajint] Sequence Region End coordinate
** @param [r] srstrand [ajint] Sequence Region Strand information
** @param [u] mrs [AjList] AJAX List of Ensembl Mapper Results
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperMap(EnsPGenericAssemblyMapper gam,
                                   const EnsPSeqRegion sr,
                                   ajint srstart,
                                   ajint srend,
                                   ajint srstrand,
                                   AjPList mrs)
{
    ajuint srid = 0;
    
    AjBool *Pflag = NULL;
    AjPStr type = NULL;
    
    /* TODO: Remove this once it works!
	EnsPDatabaseAdaptor dba = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    */
    
    /*
     ajDebug("ensGenericAssemblyMapperMap\n"
	     "  gam %p\n"
	     "  sr %p\n"
	     "  srstart %d\n"
	     "  srend %d\n"
	     "  srstrand %d\n"
	     "  mrs %p\n",
	     gam,
	     sr,
	     srstart,
	     srend,
	     srstrand,
	     mrs);
     
     ensSeqRegionTrace(sr, 1);
     */
    
    if(!gam)
	return ajFalse;
    
    if(!sr)
	return ajFalse;
    
    if(!mrs)
	return ajFalse;
    
    /* TODO: Remove this once it works!
	dba = ensAssemblyMapperAdaptorGetDatabaseAdaptor(gam->Adaptor);
    
    sra = ensRegistryGetSeqRegionAdaptor(dba);
    
    ensSeqRegionAdaptorFetchByName(sra, cs, srname, &sr);
    */
    
    srid = ensSeqRegionGetIdentifier(sr);
    
    /* TODO: Remove this once it works!
	ensSeqRegionDel(&sr);
    */
    
    /* TODO: Remove this once it works!
	if (ensCoordSystemMatch(cs, gam->CmpCoordSystem))
	*/
    if(ensCoordSystemMatch(ensSeqRegionGetCoordSystem(sr),
                           gam->CmpCoordSystem))
    {
	Pflag = (AjBool *)
	ajTableFetch(gam->CmpRegister, (const void *) &srid);
	
	if(!*Pflag)
	    ensAssemblyMapperAdaptorRegisterComponent(gam->Adaptor,
						      gam,
						      srid);
	
	type = ajStrNewC("component");
    }
    /* TODO: Remove this once it works!
	else if (ensCoordSystemMatch(cs, gam->AsmCoordSystem))
	*/
    else if (ensCoordSystemMatch(ensSeqRegionGetCoordSystem(sr),
				 gam->AsmCoordSystem))
    {
        ensAssemblyMapperAdaptorRegisterAssembled(gam->Adaptor,
						  gam,
						  srid,
						  srstart,
						  srend);
	
	type = ajStrNewC("assembled");
    }
    else
	ajFatal("ensGenericAssemblyMapperMap got an Ensembl Sequence Region, "
		"which Coordinate System '%S:%S' "
		"is neiter the assembled nor the component Coordinate System "
		"of this Ensembl Generic Assembly Mapper.",
		ensCoordSystemGetName(ensSeqRegionGetCoordSystem(sr)),
		ensCoordSystemGetVersion(ensSeqRegionGetCoordSystem(sr)));
    
    ensMapperMapCoordinates(gam->Mapper,
			    srid,
			    srstart,
			    srend,
			    srstrand,
			    type,
			    mrs);
    
    ajStrDel(&type);
    
    return ajTrue;
}




/* @func ensChainedAssemblyMapperFastMap **************************************
**
** Transform coordinates from one Ensembl Coordinate System to another by means
** of an Ensembl Generic Assembly Mapper.
**
** The caller is responsible for deleting the
** Ensembl Mapper Results before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::AssemblyMapper::fastmap
** @param [r] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] sr [const EnsPSeqRegion] Ensembl Sequence Region
** @param [r] srstart [ajint] Sequence Region Start coordinate
** @param [r] srend [ajint] Sequence Region End coordinate
** @param [r] srstrand [ajint] Sequence Region Strand information
** @param [u] mrs [AjList] AJAX List of Ensembl Mapper Results
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperFastMap(EnsPGenericAssemblyMapper gam,
                                       const EnsPSeqRegion sr,
                                       ajint srstart,
                                       ajint srend,
                                       ajint srstrand,
                                       AjPList mrs)
{
    ajuint srid = 0;
    
    AjBool *Pflag = NULL;
    
    AjPStr type = NULL;
    
    /* TODO: Remove once this works.
	EnsPDatabaseAdaptor dba = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    */
    
    /*
     ajDebug("ensGenericAssemblyMapperFastMap\n"
	     "  gam %p\n"
	     "  sr %p\n"
	     "  srstart %d\n"
	     "  srend %d\n"
	     "  srstrand %d\n"
	     "  mrs %p\n",
	     gam,
	     sr,
	     srstart,
	     srend,
	     srstrand,
	     mrs);
     */
    
    if(!gam)
	return ajFalse;
    
    if(!sr)
	return ajFalse;
    
    if(!mrs)
	return ajFalse;
    
    /*
     ** TODO: Remove this once it works!
     dba = ensAssemblyMapperAdaptorGetDatabaseAdaptor(gam->Adaptor);
     
     sra = ensRegistryGetSeqRegionAdaptor(dba);
     
     ensSeqRegionAdaptorFetchByName(sra, cs, srname, &sr);
     */
    
    srid = ensSeqRegionGetIdentifier(sr);
    
    /*
     ** TODO: Remove this once it works!
     if (ensCoordSystemMatch(cs, gam->CmpCoordSystem))
     */
    
    if(ensCoordSystemMatch(ensSeqRegionGetCoordSystem(sr),
                           gam->CmpCoordSystem))
    {
	Pflag = (AjBool *)
	ajTableFetch(gam->CmpRegister, (const void *) &srid);
	
	if(!*Pflag)
	    ensAssemblyMapperAdaptorRegisterComponent(gam->Adaptor, gam, srid);
	
	type = ajStrNewC("component");
    }
    /*
     ** TODO: Remove this once it works!
     else if (ensCoordSystemMatch(cs, gam->AsmCoordSystem))
     */
    else if (ensCoordSystemMatch(ensSeqRegionGetCoordSystem(sr),
				 gam->AsmCoordSystem))
    {
	/*
	 ** TODO: The following note from the Perl API is probably not relevant
	 ** for the C implementation.
	 **
	 ** This can be probably be sped up some by only calling registered
	 ** assembled if needed.
	 */
	
	ensAssemblyMapperAdaptorRegisterAssembled(gam->Adaptor,
						  gam,
						  srid,
						  srstart,
						  srend);
						  
	type = ajStrNewC("assembled");
    }
    else
	ajFatal("ensGenericAssemblyMapperFastMap got an "
		"Ensembl Sequence Region, which Coordinate System '%S:%S' "
		"is neiter the assembled nor the component Coordinate System "
		"of this Ensembl Generic Assembly Mapper.\n",
		ensCoordSystemGetName(ensSeqRegionGetCoordSystem(sr)),
		ensCoordSystemGetVersion(ensSeqRegionGetCoordSystem(sr)));
    
    ensMapperFastMap(gam->Mapper,
		     srid,
		     srstart,
		     srend,
		     srstrand,
		     type,
		     mrs);
    
    ajStrDel(&type);
    
    /*
     ** TODO: Remove this once it works!
     ensSeqRegionDel(&sr);
     */
    
    return ajTrue;
}




/* @func ensGenericAssemblyMapperRegisterAll **********************************
**
** Register all mappings between two Ensembl Coordinate Systems in an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::GenericAssemblyMapper::register_all
** @param [r] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGenericAssemblyMapperRegisterAll(EnsPGenericAssemblyMapper gam)
{
    if(!gam)
        return ajFalse;
    
    return ensAssemblyMapperAdaptorRegisterAll(gam->Adaptor, gam);
}




/* @datasection [EnsPChainedAssemblyMapper] Chained Assembly Mapper ***********
**
** Functions for manipulating Ensembl Chained Assembly Mapper objects
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper CVS Revision: 1.14
**
** @nam2rule ChainedAssemblyMapper
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Chained Assembly Mapper by pointer.
** It is the responsibility of the user to first destroy any previous
** Chained Assembly Mapper. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPChainedAssemblyMapper]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPChainedAssemblyMapper] Ensembl Chained
**                                                 Assembly Mapper
** @argrule Ref object [EnsPChainedAssemblyMapper] Ensembl Chained
**                                                 Assembly Mapper
**
** @valrule * [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
**
** @fcategory new
******************************************************************************/




/* @func ensChainedAssemblyMapperNew ******************************************
**
** Default constructor for an Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::new
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [r] srccs [EnsPCoordSystem] Source Ensembl Coordinate System
** @param [r] midcs [EnsPCoordSystem] Middle Ensembl Coordinate System
** @param [r] trgcs [EnsPCoordSystem] Target Ensembl Coordinate System
**
** @return [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper or NULL
** @@
******************************************************************************/

EnsPChainedAssemblyMapper ensChainedAssemblyMapperNew(
    EnsPAssemblyMapperAdaptor ama,
    EnsPCoordSystem srccs,
    EnsPCoordSystem midcs,
    EnsPCoordSystem trgcs)
{
    AjPStr srctype = NULL;
    AjPStr midtype = NULL;
    AjPStr trgtype = NULL;
    
    EnsPChainedAssemblyMapper cam = NULL;
    
    /*
     ajDebug("ensChainedAssemblyMapperNew\n"
	     "  ama %p\n"
	     "  srccs %p\n"
	     "  midcs %p\n"
	     "  trgcs %p\n",
	     ama,
	     midcs,
	     trgcs,
	     trgcs);
     
     ensCoordSystemTrace(srccs, 1);
     
     ensCoordSystemTrace(midcs, 1);
     
     ensCoordSystemTrace(trgcs, 1);
     */
    
    if(!ama)
	return NULL;
    
    if(!srccs)
	return NULL;
    
    /*
     if (! midcs)
     ajDebug("ensChainedAssemblyMapperNew got no middle Coordinate System. "
	     "Multiple parts of a component map to more than one "
	     "assembled part.\n");
     */
    
    if(!trgcs)
	return NULL;
    
    srctype = ajStrNewC("source");
    midtype = ajStrNewC("middle");
    trgtype = ajStrNewC("target");
    
    AJNEW0(cam);
    
    cam->Adaptor = ama;
    
    cam->SourceCoordSystem  = ensCoordSystemNewRef(srccs);
    cam->MiddleCoordSystem  = ensCoordSystemNewRef(midcs);
    cam->TargetCoordSystem  = ensCoordSystemNewRef(trgcs);
    cam->SourceMiddleMapper = ensMapperNew(srctype, midtype, srccs, midcs);
    cam->TargetMiddleMapper = ensMapperNew(trgtype, midtype, trgcs, midcs);
    cam->SourceTargetMapper = ensMapperNew(srctype, trgtype, srccs, trgcs);
    cam->SourceRegistry     = ensMapperRangeRegistryNew();
    cam->TargetRegistry     = ensMapperRangeRegistryNew();
    cam->MaxPairCount       = assemblyMapperMaxPairCount;
    
    cam->Use = 1;
    
    ajStrDel(&srctype);
    ajStrDel(&midtype);
    ajStrDel(&trgtype);
    
    return cam;
}




/* @func ensChainedAssemblyMapperNewRef ***************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
**
** @return [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper or NULL
** @@
******************************************************************************/

EnsPChainedAssemblyMapper ensChainedAssemblyMapperNewRef(
    EnsPChainedAssemblyMapper cam)
{
    if(!cam)
	return NULL;
    
    cam->Use++;
    
    return cam;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Chained Assembly Mapper.
**
** @fdata [EnsPChainedAssemblyMapper]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Chained Assembly Mapper object
**
** @argrule * Pcam [EnsPChainedAssemblyMapper*] Ensembl Chained Assembly Mapper
**                                              object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensChainedAssemblyMapperClear ****************************************
**
** Clear all Ensembl Mappers and Ensembl Mapper Range Registries in an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::flush
** @param [u] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensChainedAssemblyMapperClear(EnsPChainedAssemblyMapper cam)
{
    /*
     ajDebug("ensChainedAssemblyMapperClear\n"
	     "  cam %p\n",
	     cam);
     */
    
    if(!cam)
	return ajFalse;
    
    ensMapperClear(cam->SourceMiddleMapper);
    ensMapperClear(cam->TargetMiddleMapper);
    ensMapperClear(cam->SourceTargetMapper);

    ensMapperRangeRegistryClear(cam->SourceRegistry);
    ensMapperRangeRegistryClear(cam->TargetRegistry);
    
    return ajTrue;
}




/* @func ensChainedAssemblyMapperDel ******************************************
**
** Default destructor for an Ensembl Chained Assembly Mapper.
**
** @param [d] Pcam [EnsPChainedAssemblyMapper*] Ensembl Chained
**                                              Assembly Mapper
**
** @return [void]
** @@
******************************************************************************/

void ensChainedAssemblyMapperDel(EnsPChainedAssemblyMapper *Pcam)
{
    EnsPChainedAssemblyMapper pthis = NULL;
    
    if(!Pcam)
	return;
    
    if(!*Pcam)
	return;

    pthis = *Pcam;
    
    /*
     ajDebug("ensChainedAssemblyMapperDel\n"
	     "  *Pcam %p\n",
	     *Pcam);
     */
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pcam = NULL;
	
	return;
    }
    
    ensCoordSystemDel(&pthis->SourceCoordSystem);
    
    ensCoordSystemDel(&pthis->MiddleCoordSystem);
    
    ensCoordSystemDel(&pthis->TargetCoordSystem);
    
    ensMapperDel(&pthis->SourceMiddleMapper);
    
    ensMapperDel(&pthis->TargetMiddleMapper);
    
    ensMapperDel(&pthis->SourceTargetMapper);
    
    ensMapperRangeRegistryDel(&pthis->SourceRegistry);
    
    ensMapperRangeRegistryDel(&pthis->TargetRegistry);
    
    AJFREE(*Pcam);
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Chained Assembly Mapper object.
**
** @fdata [EnsPChainedAssemblyMapper]
** @fnote None
**
** @nam3rule Get Return Chained Assembly Mapper attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Assembly Mapper Adaptor
** @nam4rule GetSourceCoordSystem Return the source Ensembl Coordinate System
** @nam4rule GetMiddleCoordSystem Return the middle Ensembl Coordinate System
** @nam4rule GetTargetCoordSystem Return the target Ensembl Coordinate System
** @nam4rule GetSourceMiddleMapper Return the source <-> middle Ensembl Mapper
** @nam4rule GetTargetMiddleMapper Return the target <-> middle Ensembl Mapper
** @nam4rule GetSourceTargetMapper Return the source <-> target Ensembl Mapper
** @nam4rule GetSourceRegistry Return the source Ensembl Range Registry
** @nam4rule GetTargetRegistry Return the target Ensembl Range Registry
** @nam4rule GetMaxPairCount Return the maximum Ensembl Mapper Pair count
**
** @argrule * gam [const EnsPGenericAssemblyMapper] Generic Assembly Mapper
**
** @valrule Adaptor [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @valrule SourceCoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @valrule MiddleCoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @valrule TargetCoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @valrule SourceMiddleMapper [EnsPMapper] Ensembl Mapper
** @valrule TargetMiddleMapper [EnsPMapper] Ensembl Mapper
** @valrule SourceTargetMapper [EnsPMapper] Ensembl Mapper
** @valrule SourceRegistry [EnsPMapperRangeRegistry] Ensembl Range Registry
** @valrule TargetRegistry [EnsPMapperRangeRegistry] Ensembl Range Registry
** @valrule MaxPairCount [ajuint] Maximum Ensembl Mapper Pair count
**
** @fcategory use
******************************************************************************/




/* @func ensChainedAssemblyMapperGetAdaptor ***********************************
**
** Get the Ensembl Assembly Mapper Adaptor element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::adaptor
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyMapperAdaptor ensChainedAssemblyMapperGetAdaptor(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->Adaptor;
}




/* @func ensChainedAssemblyMapperGetSourceCoordSystem *************************
**
** Get the source Coordinate System element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::first_CoordSystem
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [const EnsPCoordSystem] Source Ensembl Coordinate System or NULL
** @@
******************************************************************************/

const EnsPCoordSystem ensChainedAssemblyMapperGetSourceCoordSystem(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->SourceCoordSystem;
}




/* @func ensChainedAssemblyMapperGetMiddleCoordSystem *************************
**
** Get the middle Coordinate System element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::middle_CoordSystem
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [const EnsPCoordSystem] Middle Ensembl Coordinate System or NULL
** @@
******************************************************************************/

const EnsPCoordSystem ensChainedAssemblyMapperGetMiddleCoordSystem(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->MiddleCoordSystem;
}




/* @func ensChainedAssemblyMapperGetTargetCoordSystem *************************
**
** Get the target Coordinate System element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::last_CoordSystem
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [const EnsPCoordSystem] Target Ensembl Coordinate System or NULL
** @@
******************************************************************************/

const EnsPCoordSystem ensChainedAssemblyMapperGetTargetCoordSystem(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->TargetCoordSystem;
}




/* @func ensChainedAssemblyMapperGetSourceMiddleMapper ************************
**
** Get the source to middle Ensembl Mapper element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::first_middle_mapper
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [EnsPMapper] Source to middle Ensembl Mapper or NULL
** @@
******************************************************************************/

EnsPMapper ensChainedAssemblyMapperGetSourceMiddleMapper(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->SourceMiddleMapper;
}




/* @func ensChainedAssemblyMapperGetTargetMiddleMapper ************************
**
** Get the target to middle Ensembl Mapper element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::last_middle_mapper
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [EnsPMapper] Target to middle Ensembl Mapper or NULL
** @@
******************************************************************************/

EnsPMapper ensChainedAssemblyMapperGetTargetMiddleMapper(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->TargetMiddleMapper;
}




/* @func ensChainedAssemblyMapperGetSourceTargetMapper ************************
**
** Get the source to target Ensembl Mapper element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::first_last_mapper
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [EnsPMapper] Source to target Ensembl Mapper or NULL
** @@
******************************************************************************/

EnsPMapper ensChainedAssemblyMapperGetSourceTargetMapper(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->SourceTargetMapper;
}




/* @func ensChainedAssemblyMapperGetSourceRegistry ****************************
**
** Get the source Ensembl Mapper Range Registry element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::first_registry
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [EnsPMapperRangeRegistry] Source Ensembl Mapper Range Registry
**                                   or NULL.
** @@
******************************************************************************/

EnsPMapperRangeRegistry ensChainedAssemblyMapperGetSourceRegistry(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->SourceRegistry;
}




/* @func ensChainedAssemblyMapperGetTargetRegistry ****************************
**
** Get the target Ensembl Mapper Range Registry element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::last_registry
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [EnsPMapperRangeRegistry] Target Ensembl Mapper Range Registry
**                                   or NULL
** @@
******************************************************************************/

/*
** FIXME: The Perl POD has a typo:
**  =head2 lsst_registry
*/

EnsPMapperRangeRegistry ensChainedAssemblyMapperGetTargetRegistry(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return NULL;
    
    return cam->TargetRegistry;
}




/* @func ensChainedAssemblyMapperGetMaxPairCount ******************************
**
** Get the maximum Ensembl Mapper Pair count element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::max_pair_count
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [ajuint] Maximum Ensembl Mapper Pair count or 0
** @@
******************************************************************************/

ajuint ensChainedAssemblyMapperGetMaxPairCount(
    const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return 0;
    
    return cam->MaxPairCount;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Chained Assembly Mapper object.
**
** @fdata [EnsPChainedAssemblyMapper]
** @fnote None
**
** @nam3rule Set Set one element of a Chained Assembly Mapper
** @nam4rule SetAdaptor Set the Ensembl Assembly Mapper
** @nam4rule SetMaxPairCount Set the maximum Ensembl Mapper Pair count
**
** @argrule * cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly
**                                            Mapper object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensChainedAssemblyMapperSetAdaptor ***********************************
**
** Set the Adaptor element of an Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::adaptor
** @param [u] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensChainedAssemblyMapperSetAdaptor(EnsPChainedAssemblyMapper cam,
                                          EnsPAssemblyMapperAdaptor ama)
{
    if(!cam)
        return ajFalse;
    
    if(!ama)
        return ajFalse;
    
    cam->Adaptor = ama;
    
    return ajTrue;
}




/* @func ensChainedAssemblyMapperSetMaxPairCount ******************************
**
** Set the maximum Ensembl Mapper Pair count element of an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::max_pair_count
** @param [u] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
** @param [r] count [ajuint] Maximum Ensembl Mapper Pair count
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
** This sets the number of Ensembl Mapper Pairs allowed in the internal cache.
** This can be used to override the default value (6000) to tune the
** performance and memory usage for certain scenarios.
** A higher value means a bigger cache and thus higher memory requirements.
******************************************************************************/

AjBool ensChainedAssemblyMapperSetMaxPairCount(EnsPChainedAssemblyMapper cam,
                                               ajuint count)
{
    if(!cam)
        return ajFalse;
    
    if(!count)
        return ajFalse;
    
    cam->MaxPairCount = count;
    
    return ajTrue;
}




/* @func ensChainedAssemblyMapperGetPairCount *********************************
**
** Get the number of Ensembl Mapper Pairs in an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::size
** @param [r] cam [const EnsPChainedAssemblyMapper] Ensembl Chained
**                                                  Assembly Mapper
**
** @return [ajuint] Number of Ensembl Mapper Pairs or 0
** @@
******************************************************************************/

ajuint ensChainedAssemblyMapperGetPairCount(const EnsPChainedAssemblyMapper cam)
{
    if(!cam)
	return 0;
    
    return (ensMapperGetPairCount(cam->SourceMiddleMapper) +
	    ensMapperGetPairCount(cam->TargetMiddleMapper) +
	    ensMapperGetPairCount(cam->SourceTargetMapper));
}




/* @func ensChainedAssemblyMapperMap ******************************************
**
** Transform coordinates from one Ensembl Coordinate System to another by means
** of an Ensembl Chained Assembly Mapper.
**
** The caller is responsible for deleting the
** Ensembl Mapper Results before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::map
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::fastmap
** @param [r] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
** @param [r] sr [const EnsPSeqRegion] Ensembl Sequence Region
** @param [r] srstart [ajint] Sequence Region Start coordinate
** @param [r] srend [ajint] Sequence Region End coordinate
** @param [r] srstrand [ajint] Sequence Region Strand information
** @param [r] optsr [const EnsPSeqRegion] Optional Ensembl Sequence Region
** @param [r] fastmap [AjBool] Fast-mapping attribute
** @param [u] mrs [AjList] AJAX List of Ensembl Mapper Results
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensChainedAssemblyMapperMap(EnsPChainedAssemblyMapper cam,
                                   const EnsPSeqRegion sr,
                                   ajint srstart,
                                   ajint srend,
                                   ajint srstrand,
                                   const EnsPSeqRegion optsr,
                                   AjBool fastmap,
                                   AjPList mrs)
{
    ajint regstart = 0;
    ajint regend   = 0;
    
    ajuint srid    = 0;
    ajuint optsrid = 0;
    
    AjBool isinsert = AJFALSE;
    
    AjPList ranges = NULL;
    
    AjPStr type = NULL;
    
    /*
    ** TODO: Remove once this works.
    **	EnsPDatabaseAdaptor dba = NULL;
    */
    
    EnsPMapperRange mr = NULL;
    EnsPMapperRangeRegistry registry = NULL;
    
    /*
    ** TODO: Remove once this works.
    ** EnsPSeqRegionAdaptor sra = NULL;
    */
    
    /*
     ajDebug("ensChainedAssemblyMapperMap\n"
	     "  cam %p\n"
	     "  sr %p\n"
	     "  srstart %d\n"
	     "  srend %d\n"
	     "  srstrand %d\n"
	     "  optsr %p\n"
	     "  fastmap '%B'\n"
	     "  mrs %p\n",
	     cam,
	     sr,
	     srstart,
	     srend,
	     srstrand,
	     optsr,
	     fastmap,
	     mrs);
     
     ensSeqRegionTrace(sr, 1);
     
     ensSeqRegionTrace(optsr, 1);
     */
    
    if(!cam)
	return ajFalse;
    
    if(!sr)
	return ajFalse;
    
    if(!mrs)
	return ajFalse;
    
    isinsert = (srstart == (srend + 1));
    
    /* TODO: Remove once this works.
	dba = ensAssemblyMapperAdaptorGetDatabaseAdaptor(cam->Adaptor);
    
    sra = ensRegistryGetSeqRegionAdaptor(dba);
    
    ensSeqRegionAdaptorFetchByName(sra, cs, srname, &sr);
    
    if(!sr)
    {
	ajWarn("ensChainedAssemblyMapperMap could not get "
	       "Ensembl Sequence Region for name '%S' and "
	       "Ensembl Coordinate System %p.\n",
	       srname,
	       cs);
	
	ensCoordSystemTrace(cs, 1);
    }
    */
    
    srid = ensSeqRegionGetIdentifier(sr);
    
    optsrid = ensSeqRegionGetIdentifier(optsr);
    
    /* TODO: Remove once this works.
	ensSeqRegionDel(&sr);
    */
    
    /* TODO: Remove once this works.
	if (ensCoordSystemMatch(cs, cam->SourceCoordSystem))
	*/
    if(ensCoordSystemMatch(ensSeqRegionGetCoordSystem(sr),
			    cam->SourceCoordSystem))
    {
	type = ajStrNewC("source");
	
	registry = cam->SourceRegistry;
    }
    /* TODO: Remove once this works.
	else if (ensCoordSystemMatch(cs, cam->TargetCoordSystem))
	*/
    else if (ensCoordSystemMatch(ensSeqRegionGetCoordSystem(sr),
				 cam->TargetCoordSystem))
    {
	type = ajStrNewC("target");
	
	registry = cam->TargetRegistry;
    }
    else
	ajFatal("ensChainedAssemblyMapperMap got an Ensembl Sequence Region, "
		"which Coordinate System '%S:%S' is "
	        "neither the source nor the target Coordinate System of this "
		"Chained Assembly Mapper.",
		ensCoordSystemGetName(ensSeqRegionGetCoordSystem(sr)),
		ensCoordSystemGetVersion(ensSeqRegionGetCoordSystem(sr)));
    
    
    /*
     ** The minimum area we want to register if registration is necessary is
     ** about 1 Mb. Break requested ranges into chunks of 1 Mb and then
     ** register this larger region if we have a registry miss.
     **
     ** Use bitwise shift for fast and easy integer multiplication and
     ** division.
     */
    
    if (isinsert)
    {
	regstart = ((srend >> assemblyMapperChunkFactor)
		    << assemblyMapperChunkFactor);
	
	regend = (((srstart >> assemblyMapperChunkFactor) + 1)
		  << assemblyMapperChunkFactor) - 1 ;
    }
    else
    {
	regstart = ((srstart >> assemblyMapperChunkFactor)
		    << assemblyMapperChunkFactor);
	
	regend = (((srend >> assemblyMapperChunkFactor) + 1)
		  << assemblyMapperChunkFactor) - 1 ;
    }
    
    /*
     ** Get a list of Mapper Ranges in the requested region that have not been
     ** registered, and register them at the same time.
     */
    
    ranges = ajListNew();
    
    if(isinsert)
	ensMapperRangeRegistryCheckAndRegister(registry,
					       srid,
					       srend,
					       srstart,
					       regstart,
					       regend,
					       ranges);
    else
	ensMapperRangeRegistryCheckAndRegister(registry,
					       srid,
					       srstart,
					       srend,
					       regstart,
					       regend,
					       ranges);
    
    if(ajListGetLength(ranges))
    {
	if(ensChainedAssemblyMapperGetPairCount(cam) > cam->MaxPairCount)
	{
	    /*
	     ** If the Chained Assembly Mapper contains already more
	     ** Ensembl Mapper Pairs than allowed, clear the internal cache and
	     ** re-register the lot. The AJAX List of Ensembl Mapper Ranges
	     ** needs clearing as well.
	     */
	    
	    ensChainedAssemblyMapperClear(cam);
	    
	    while(ajListPop(ranges, (void **) &mr))
		ensMapperRangeDel(&mr);
	    
	    if(isinsert)
		ensMapperRangeRegistryCheckAndRegister(registry,
						       srid,
						       srend,
						       srstart,
						       regstart,
						       regend,
						       ranges);
	    else
		ensMapperRangeRegistryCheckAndRegister(registry,
						       srid,
						       srstart,
						       srend,
						       regstart,
						       regend,
						       ranges);
	}
	
	ensAssemblyMapperAdaptorRegisterChained(cam->Adaptor,
						cam,
						type,
						srid,
						optsrid,
						ranges);
    }
    
    if(fastmap)
	ensMapperFastMap(cam->SourceTargetMapper,
			 srid,
			 srstart,
			 srend,
			 srstrand,
			 type,
			 mrs);
    else
	ensMapperMapCoordinates(cam->SourceTargetMapper,
				srid,
				srstart,
				srend,
				srstrand,
				type,
				mrs);
    
    /*
     ** Delete all Ensembl Mapper Ranges from the AJAX List before
     ** deleting the List.
     */
    
    while(ajListPop(ranges, (void **) &mr))
	ensMapperRangeDel(&mr);
    
    ajListFree(&ranges);
    
    ajStrDel(&type);
    
    return ajTrue;
}




/* @func ensChainedAssemblyMapperRegisterAll **********************************
**
** Register all mappings between two Ensembl Coordinate Systems in an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::ChainedAssemblyMapper::register_all
** @param [r] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensChainedAssemblyMapperRegisterAll(EnsPChainedAssemblyMapper cam)
{
    if(!cam)
        return ajFalse;
    
    return ensAssemblyMapperAdaptorRegisterAllChained(cam->Adaptor, cam);
}




/* @datasection [EnsPTopLevelAssemblyMapper] Top-Level Assembly Mapper ********
**
** Functions for manipulating Ensembl Top-Level Assembly Mapper objects
**
** @cc Bio::EnsEMBL::TopLevelAssemblyMapper CVS Revision: 1.7
**
** @nam2rule TopLevelAssemblyMapper
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Top-Level Assembly Mapper by pointer.
** It is the responsibility of the user to first destroy any previous
** Top-Level Assembly Mapper. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPTopLevelAssemblyMapper]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                                  Assembly Mapper
** @argrule Ref object [EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                                  Assembly Mapper
**
** @valrule * [EnsPTopLevelAssemblyMapper] Ensembl Top-Level Assembly Mapper
**
** @fcategory new
******************************************************************************/




/* @func ensTopLevelAssemblyMapperNew *****************************************
**
** Default constructor for an Ensembl TopLevel Assembly Mapper.
**
** @cc Bio::EnsEMBL::TopLevelAssemblyMapper::new
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] cs1 [EnsPCoordSystem] Top-level Ensembl Coordinate System
** @param [u] cs2 [EnsPCoordSystem] Other Ensembl Coordinate System
**
** @return [EnsPTopLevelAssemblyMapper] Ensembl Top-Level Assembly Mapper
**                                      or NULL
** @@
******************************************************************************/

/*
 ** FIXME: The name section in the Perl API POD lists this as
 ** Bio::EnsEMBL::TopLevelMapper not Bio::EnsEMBL::TopLevelAssemblyMapper.
 */

EnsPTopLevelAssemblyMapper ensTopLevelAssemblyMapperNew(
    EnsPAssemblyMapperAdaptor ama,
    EnsPCoordSystem cs1,
    EnsPCoordSystem cs2)
{
    EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPTopLevelAssemblyMapper tlam = NULL;
    
    if(!ama)
	return NULL;
    
    if(!cs1)
	return NULL;
    
    if(!cs2)
	return NULL;
    
    if(!ensCoordSystemIsTopLevel(cs1))
	ajFatal("ensTopLevelMapperNew cs1 '%S:%S' should be a top-level "
		"Coordinate System.",
		ensCoordSystemGetName(cs1),
		ensCoordSystemGetVersion(cs1));
    
    if(ensCoordSystemIsTopLevel(cs2))
	ajFatal("ensTopLevelMapperNew cs2 '%S:%S' should be no top-level "
		"Coordinate System.",
                ensCoordSystemGetName(cs2),
		ensCoordSystemGetVersion(cs2));
    
    csa = ensRegistryGetCoordSystemAdaptor(ama->Adaptor);
    
    AJNEW0(tlam);
    
    tlam->Adaptor = ama;
    
    tlam->CoordSystems = ajListNew();
    
    ensCoordSystemAdaptorFetchAll(csa, tlam->CoordSystems);
    
    tlam->TopLevelCoordSystem = ensCoordSystemNewRef(cs1);
    tlam->OtherCoordSystem    = ensCoordSystemNewRef(cs2);
    
    tlam->Use = 1;
    
    return tlam;
}




/* @func ensTopLevelAssemblyMapperNewRef **************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] tlam [EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                              Assembly Mapper
**
** @return [EnsPTopLevelAssemblyMapper] Ensembl Top-Level Assembly Mapper
**                                      or NULL
** @@
******************************************************************************/

EnsPTopLevelAssemblyMapper ensTopLevelAssemblyMapperNewRef(
    EnsPTopLevelAssemblyMapper tlam)
{
    if(!tlam)
	return NULL;
    
    tlam->Use++;
    
    return tlam;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Top-Level Assembly Mapper.
**
** @fdata [EnsPTopLevelAssemblyMapper]
** @fnote None
**
** @nam3rule Del Destroy (free) an EnsPTopLevelAssemblyMapper object
**
** @argrule * Ptlam [EnsPTopLevelAssemblyMapper*] Top-Level Assembly Mapper
**                                                object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensTopLevelAssemblyMapperDel *****************************************
**
** Default destructor for an Ensembl TopLevel Assembly Mapper.
**
** @param [d] Ptlam [EnsPTopLevelAssemblyMapper*]  Ensembl Top-Level Assembly
**                                                 Mapper address
** @return [void]
** @@
******************************************************************************/

void ensTopLevelAssemblyMapperDel(EnsPTopLevelAssemblyMapper* Ptlam)
{
    EnsPCoordSystem cs = NULL;
    EnsPTopLevelAssemblyMapper pthis = NULL;
    
    if(!Ptlam)
	return;
    
    if(!*Ptlam)
	return;

    pthis = *Ptlam;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Ptlam = NULL;
	
	return;
    }
    
    ensCoordSystemDel(&pthis->TopLevelCoordSystem);
    
    ensCoordSystemDel(&pthis->OtherCoordSystem);
    
    /*
     ** Delete the Ensembl Coordinate System list elements before deleting
     ** the AJAX List.
     */
    
    while (ajListPop(pthis->CoordSystems, (void **) &cs))
	ensCoordSystemDel(&cs);
	   
    ajListFree(&pthis->CoordSystems);
	   
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Top-Level Assembly Mapper object.
**
** @fdata [EnsPTopLevelAssemblyMapper]
** @fnote None
**
** @nam3rule Get Return Top-Level Assembly Mapper attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Assembly Mapper Adaptor
** @nam4rule GetAssembledCoordSystem Return the assembled Coordinate System
** @nam4rule GetComponentCoordSystem Return the component Coordinate System
**
** @argrule * tlam [const EnsPTopLevelAssemblyMapper] Top-Level Assembly Mapper
**
** @valrule Adaptor [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @valrule AssembledCoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @valrule ComponentCoordSystem [EnsPCoordSystem] Ensembl Coordinate System
**
** @fcategory use
******************************************************************************/




/* @func ensTopLevelAssemblyMapperGetAdaptor **********************************
**
** Get the Ensembl Assembly Mapper Adaptor element of an
** Ensembl Top-Level Assembly Mapper.
**
** @cc Bio::EnsEMBL::TopLevelAssemblyMapper::adaptor
** @param [r] tlam [const EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                                    Assembly Mapper
**
** @return [const EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
**                                           or NULL
** @@
******************************************************************************/

const EnsPAssemblyMapperAdaptor ensTopLevelAssemblyMapperGetAdaptor(
    const EnsPTopLevelAssemblyMapper tlam)
{
    if(!tlam)
	return NULL;
    
    return tlam->Adaptor;
}




/* @func ensTopLevelAssemblyMapperGetAssembledCoordSystem *********************
**
** Get the Assembled Ensembl Coordinate System element of an
** Ensembl Top-Level Assembly Mapper.
**
** @cc Bio::EnsEMBL::TopLevelAssemblyMapper::assembled_CoordSystem
** @param [r] tlam [const EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                                    Assembly Mapper
**
** @return [const EnsPCoordSystem] Assembled Ensembl Coordinate System or NULL
** @@
******************************************************************************/

const EnsPCoordSystem ensTopLevelAssemblyMapperGetAssembledCoordSystem(
    const EnsPTopLevelAssemblyMapper tlam)
{
    if(!tlam)
	return NULL;
    
    return tlam->TopLevelCoordSystem;
}




/* @func ensTopLevelAssemblyMapperGetComponentCoordSystem *********************
**
** Get the component Ensembl Coordinate System element of an
** Ensembl Top-Level Assembly Mapper.
**
** @cc Bio::EnsEMBL::TopLevelAssemblyMapper::component_CoordSystem
** @param [r] tlam [const EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                                    Assembly Mapper
**
** @return [const EnsPCoordSystem] Component Ensembl Coordinate System or NULL
** @@
******************************************************************************/

const EnsPCoordSystem ensTopLevelAssemblyMapperGetComponentCoordSystem(
    const EnsPTopLevelAssemblyMapper tlam)
{
    if(!tlam)
	return NULL;
    
    return tlam->OtherCoordSystem;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Top-Level Assembly Mapper object.
**
** @fdata [EnsPTopLevelAssemblyMapper]
** @fnote None
**
** @nam3rule Set Set one element of a Top-Level Assembly Mapper
** @nam4rule SetAdaptor Set the Ensembl Assembly Mapper
**
** @argrule * tlam [EnsPTopLevelAssemblyMapper] Ensembl Top-Level Assembly
**                                              Mapper object
**
** @valrule * [AjBool]
**
** @fcategory modify
******************************************************************************/




/* @func ensTopLevelAssemblyMapperSetAdaptor **********************************
**
** Set the Ensembl Assembly Mapper Adaptor element of an
** Ensembl Top-Level Assembly Mapper.
**
** @param [u] cam [EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                             Assembly Mapper
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly
**                                            Mapper Adaptor
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTopLevelAssemblyMapperSetAdaptor(EnsPTopLevelAssemblyMapper tlam,
                                           EnsPAssemblyMapperAdaptor ama)
{
    if(!tlam)
	return ajFalse;
    
    if(!ama)
	return ajFalse;
    
    tlam->Adaptor = ama;
    
    return ajTrue;
}




/* @func ensTopLevelAssemblyMapperMap *****************************************
**
** Transform coordinates from one Ensembl Coordinate System to another by means
** of an Ensembl Top-Level Assembly Mapper.
**
** The caller is responsible for deleting the
** Ensembl Mapper Results before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::TopLevelAssemblyMapper::map
** @cc Bio::EnsEMBL::TopLevelAssemblyMapper::fastmap
** @param [r] tlam [EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                              Assembly Mapper
** @param [r] sr [const EnsPSeqRegion] Sequence Region
** @param [r] srstart [ajint] Sequence Region start coordinate
** @param [r] srend [ajint] Sequence Region end coordinate
** @param [r] srstrand [ajint] Sequence Region strand information
** @param [r] fastmap [AjBool] Fast-mapping attribute
** @param [u] mrs [AjPList] AJAX List of Ensembl Mapper Results
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTopLevelAssemblyMapperMap(EnsPTopLevelAssemblyMapper tlam,
                                    const EnsPSeqRegion sr,
                                    ajint srstart,
                                    ajint srend,
                                    ajint srstrand,
                                    AjBool fastmap,
                                    AjPList mrs)
{
    ajuint srid = 0;
    
    AjBool done = AJFALSE;
    
    AjIList iter = NULL;
    const AjPList mappath = NULL;
    
    EnsPAssemblyMapper am = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPCoordSystem tstcs = NULL;
    const EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPMapperResult mr = NULL;
    
    /*
     ** TODO: Remove this once it works!
     EnsPSeqRegion sr = NULL;
     EnsPSeqRegionAdaptor sra = NULL;
     */
    
    if(!tlam)
	return ajFalse;
    
    if(!sr)
	return ajFalse;
    
    if(!mrs)
	return ajFalse;
    
    /*
    ** FIXME: This should not be the case anyway. No SeqRegion can be
    ** associated with the top-level Coordinate System. This test was only
    ** neccessary for Coordinate Systems passed in separately.
    ** Now Sequence Regions with an attached Coordinate System are passed in .
    */
    if(ensCoordSystemIsTopLevel(ensSeqRegionGetCoordSystem(sr)))
	ajFatal("The top-level Coordinate System can only be mapped TO, "
	        "not FROM.");
    
    if(!ensCoordSystemMatch(ensSeqRegionGetCoordSystem(sr),
			    tlam->OtherCoordSystem))
	ajFatal("ensTopLevelAssemblyMapperMap got an Ensembl Sequence Region, "
		"which Coordinate System '%S:%S' "
		"is neither the assembled nor the component Coordinate System "
		"of this Top-Level Assembly Mapper.",
		ensCoordSystemGetName(ensSeqRegionGetCoordSystem(sr)),
		ensCoordSystemGetVersion(ensSeqRegionGetCoordSystem(sr)));
    
    dba = ensAssemblyMapperAdaptorGetDatabaseAdaptor(tlam->Adaptor);
    csa = ensRegistryGetCoordSystemAdaptor(dba);
    
    /*
    ** TODO: Remove once it works!
    ** sra = ensRegistryGetSeqRegionAdaptor(dba);
    ** 
    ** ensSeqRegionAdaptorFetchByName(sra, cs, srname, &sr);
    */
    
    srid = ensSeqRegionGetIdentifier(sr);
    
    /*
    ** TODO: Remove once it works!
    ** ensSeqRegionDel(&sr);
    */
    
    iter = ajListIterNew(tlam->CoordSystems);
    
    while(!ajListIterDone(iter))
    {
	tstcs = (EnsPCoordSystem) ajListIterGet(iter);
	
	if(ensCoordSystemGetRank(tstcs) >=
	   ensCoordSystemGetRank(tlam->OtherCoordSystem))
	    break;
	
	/* Check if a mapping path even exists to this Coordinate System. */
	
	mappath =
	    ensCoordSystemAdaptorGetMappingPath(csa,
						tstcs,
						tlam->OtherCoordSystem);
	
	if(ajListGetLength(mappath))
	{
	    /*
	     ** Try to map to this Coordinate System.
	     ** If we get back any coordinates then it is our 'toplevel' that
	     ** we were looking for.
	     */
	    
	    am =
	    ensAssemblyMapperAdaptorFetchByCoordSystems(tlam->Adaptor,
							tlam->OtherCoordSystem,
							tstcs);
	    
	    if(fastmap)
	    {
		ensAssemblyMapperFastMap(am,
					 sr,
					 srstart,
					 srend,
					 srstrand,
					 mrs);
		
		if(ajListGetLength(mrs))
		{
		    done = ajTrue;
		    
		    break;
		}
	    }
	    else
	    {
		ensAssemblyMapperMap(am,
				     sr,
				     srstart,
				     srend,
				     srstrand,
				     mrs);
		
		if(ajListGetLength(mrs))
		{
		    if(ajListGetLength(mrs) > 1)
		    {
			done = ajTrue;
			
			break;
		    }
		    
		    ajListPeekFirst(mrs, (void **) &mr);
		    
		    if(ensMapperResultGetType(mr) != ensEMapperResultGap)
		    {
			done = ajTrue;
			
			break;
		    }
		    
		    while(ajListPop(mrs, (void **) &mr))
			ensMapperResultDel(&mr);
		}
	    }
	    
	    ensAssemblyMapperDel(&am);
	}
    }
    
    ajListIterDel(&iter);
    
    if (done)
    {
	ensAssemblyMapperDel(&am);
	
	return ajTrue;
    }
    
    /*
     ** The top-level coordinate system for the region requested *is* the
     ** requested region.
     */
    
    mr = MENSMAPPERCOORDINATENEW(srid,
				 srstart,
				 srend,
				 srstrand,
				 tlam->OtherCoordSystem);
    
    ajListPushAppend(mrs, (void *) mr);
    
    return ajTrue;
}




/* @datasection [EnsPAssemblyMapper] Assembly Mapper **************************
**
** Functions for manipulating Ensembl Assembly Mapper objects
**
** @nam2rule AssemblyMapper
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Assembly Mapper by pointer.
** It is the responsibility of the user to first destroy any previous
** Assembly Mapper. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPAssemblyMapper]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @argrule Ref object [EnsPAssemblyMapper] Ensembl Assembly Mapper
**
** @valrule * [EnsPAssemblyMapper] Ensembl Assembly Mapper
**
** @fcategory new
******************************************************************************/




/* @func ensAssemblyMapperNew *************************************************
**
** Default constructor for an Ensembl Assembly Mapper.
**
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [r] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
** @param [r] gam [EnsPTopLevelAssemblyMapper] Ensembl Top-Level
**                                             Assembly Mapper
**
** @return [EnsPAssemblyMapper] Ensembl Assembly Mapper or NULL
** @@
******************************************************************************/

EnsPAssemblyMapper ensAssemblyMapperNew(EnsPAssemblyMapperAdaptor ama,
                                        EnsPGenericAssemblyMapper gam,
                                        EnsPChainedAssemblyMapper cam,
                                        EnsPTopLevelAssemblyMapper tlam)
{
    EnsPAssemblyMapper am = NULL;
    
    if(!ama)
	return NULL;
    
    if(!(gam || cam || tlam))
	return NULL;
    
    if(gam && (cam || tlam))
	ajFatal("ensAssemblyMapperNew ");
    
    if(cam && (gam || tlam))
	ajFatal("ensAssemblyMapperNew ");
    
    if(tlam && (gam || cam))
	ajFatal("ensAssemblyMapperNew ");
    
    AJNEW0(am);
    
    am->Adaptor = ama;
    
    am->Generic  = ensGenericAssemblyMapperNewRef(gam);
    am->Chained  = ensChainedAssemblyMapperNewRef(cam);
    am->TopLevel = ensTopLevelAssemblyMapperNewRef(tlam);
    
    am->Use = 1;
    
    return am;
}




/* @func ensAssemblyMapperNewRef **********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
**
** @return [EnsPAssemblyMapper] Ensembl Assembly Mapper or NULL
** @@
******************************************************************************/

EnsPAssemblyMapper ensAssemblyMapperNewRef(EnsPAssemblyMapper am)
{
    if(!am)
	return NULL;
    
    am->Use++;
    
    return am;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Assembly Mapper.
**
** @fdata [EnsPAssemblyMapper]
** @fnote None
**
** @nam3rule Del Destroy (free) an EnsPAssemblyMapper object
**
** @argrule * Pam [EnsPAssemblyMapper*] Assembly Mapper object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensAssemblyMapperDel *************************************************
**
** Default destructor for an Ensembl Assembly Mapper.
**
** @param [d] Pam [EnsPAssemblyMapper] Ensembl Assembly Mapper address
**
** @return [void]
** @@
******************************************************************************/

void ensAssemblyMapperDel(EnsPAssemblyMapper* Pam)
{
    EnsPAssemblyMapper pthis = NULL;
    
    if(!Pam)
	return;
    
    if(!*Pam)
	return;

    pthis = *Pam;
    
    /*
     ajDebug("ensAssemblyMapperDel\n"
	     "  *Pam %p\n",
	     *Pam);
     */
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pam = NULL;
	
	return;
    }
    
    ensGenericAssemblyMapperDel(&pthis->Generic);
    
    ensChainedAssemblyMapperDel(&pthis->Chained);
    
    ensTopLevelAssemblyMapperDel(&pthis->TopLevel);
    
    AJFREE(*Pam);

    *Pam = NULL;
    
    return;
}




/* @func ensAssemblyMapperMap *************************************************
**
** Transform coordinates from one Ensembl Coordinate System to another by means
** of an Ensembl Assembly Mapper.
**
** The caller is responsible for deleting the
** Ensembl Mapper Results before deleting the AJAX List.
**
** @param [r] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] sr [const EnsPSeqRegion] Ensembl Sequence Region
** @param [r] srstart [ajint] Sequence Region start coordinate
** @param [r] srend [ajint] Sequence Region end coordinate
** @param [r] srstrand [ajint] Sequence Region strand information
** @param [u] mrs [AjList] AJAX List of Ensembl Mapper Results
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyMapperMap(EnsPAssemblyMapper am,
                            const EnsPSeqRegion sr,
                            ajint srstart,
                            ajint srend,
                            ajint srstrand,
                            AjPList mrs)
{
    if(!am)
	return ajFalse;
    
    if(am->Generic)
	ensGenericAssemblyMapperMap(am->Generic,
				    sr,
				    srstart,
				    srend,
				    srstrand,
				    mrs);
    
    if(am->Chained)
	ensChainedAssemblyMapperMap(am->Chained,
				    sr,
				    srstart,
				    srend,
				    srstrand,
				    (const EnsPSeqRegion) NULL,
				    ajFalse,
				    mrs);
    
    if(am->TopLevel)
	ensTopLevelAssemblyMapperMap(am->TopLevel,
				     sr,
				     srstart,
				     srend,
				     srstrand,
				     ajFalse,
				     mrs);
    
    return ajTrue;
}




/* @func ensAssemblyMapperFastMap *********************************************
**
** Transform coordinates from one Ensembl Coordinate System to another by means
** of an Ensembl Assembly Mapper.
**
** The caller is responsible for deleting the
** Ensembl Mapper Results before deleting the AJAX List.
**
** @param [r] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] sr [const EnsPSeqRegion] Ensembl Sequence Region
** @param [r] srstart [ajint] Sequence Region Start coordinate
** @param [r] srend [ajint] Sequence Region End coordinate
** @param [r] srstrand [ajint] Sequence Region Strand information
** @param [u] mrs [AjList] AJAX List of Ensembl Mapper Results
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyMapperFastMap(EnsPAssemblyMapper am,
                                const EnsPSeqRegion sr,
                                ajint srstart,
                                ajint srend,
                                ajint srstrand,
                                AjPList mrs)
{
    if(!am)
	return ajFalse;
    
    if(am->Generic)
	ensGenericAssemblyMapperFastMap(am->Generic,
					sr,
					srstart,
					srend,
					srstrand,
					mrs);
    
    if(am->Chained)
	ensChainedAssemblyMapperMap(am->Chained,
				    sr,
				    srstart,
				    srend,
				    srstrand,
				    (const EnsPSeqRegion) NULL,
				    ajTrue,
				    mrs);
    
    if(am->TopLevel)
	ensTopLevelAssemblyMapperMap(am->TopLevel,
				     sr,
				     srstart,
				     srend,
				     srstrand,
				     ajTrue,
				     mrs);
    
    return ajTrue;
}




/* @func ensAssemblyMapperMapToSeqRegion **************************************
**
** Transform coordinates from one Ensembl Coordinate System to another by means
** of an Ensembl Assembly Mapper.
**
** The caller is responsible for deleting the
** Ensembl Mapper Results before deleting the AJAX List.
**
** @param [r] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] sr [const EnsPSeqRegion] Ensembl Sequence Region
** @param [r] srstart [ajint] Sequence Region start coordinate
** @param [r] srend [ajint] Sequence Region end coordinate
** @param [r] srstrand [ajint] Sequence Region strand information
** @param [r] optsr [const EnsPSeqRegion] Optional Ensembl Sequence Region
** @param [u] mrs [AjList] AJAX List of Ensembl Mapper Results
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyMapperMapToSeqRegion(EnsPAssemblyMapper am,
                                       const EnsPSeqRegion sr,
                                       ajint srstart,
                                       ajint srend,
                                       ajint srstrand,
                                       const EnsPSeqRegion optsr,
                                       AjPList mrs)
{
    if(!am)
	return ajFalse;
    
    if(am->Generic)
	ensGenericAssemblyMapperMap(am->Generic,
				    sr,
				    srstart,
				    srend,
				    srstrand,
				    mrs);
    
    if(am->Chained)
	ensChainedAssemblyMapperMap(am->Chained,
				    sr,
				    srstart,
				    srend,
				    srstrand,
				    optsr,
				    ajFalse,
				    mrs);
    
    if(am->TopLevel)
	ensTopLevelAssemblyMapperMap(am->TopLevel,
				     sr,
				     srstart,
				     srend,
				     srstrand,
				     ajFalse,
				     mrs);
    
    return ajTrue;
}




/* @datasection [EnsPAssemblyMapperAdaptor] Assembly Mapper Adaptor ***********
**
** Functions for manipulating Ensembl Assembly Mapper Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor CVS Revision: 1.57
**
** @nam2rule AssemblyMapperAdaptor
**
******************************************************************************/




/* @funcstatic assemblyMapperAdaptorHasMultipleMappings ***********************
**
** Check whether an Ensembl Sequence Region maps to more than one location.
**
** @param [r] ama [const EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper
**                                                  Adaptor
** @param [r] srid [ajuint] Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyMapperAdaptorHasMultipleMappings(
    const EnsPAssemblyMapperAdaptor ama,
    ajuint srid)
{
    if(!ama)
	return ajFalse;
    
    if(!srid)
	return ajFalse;
    
    if(!ama->MultipleMappings)
	ajFatal("assemblyMapperAdaptorHasMultipleMappings AJAX Table for "
		"multiple Sequence Region mappings has not been "
		"initialised!\n");
    
    if(ajTableFetch(ama->MultipleMappings, (const void *) &srid))
	return ajTrue;
    
    return ajFalse;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Assembly Mapper Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Assembly Mapper Adaptor. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPAssemblyMapperAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPAssemblyMapperAdaptor] Ensembl Assembly
**                                                 Mapper Adaptor
** @argrule Ref object [EnsPAssemblyMapperAdaptor] Ensembl Assembly
**                                                 Mapper Adaptor
**
** @valrule * [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
**
** @fcategory new
******************************************************************************/




/* @funcstatic assemblyMapperAdaptorMultipleMappingsInit **********************
**
** Initialise Ensembl Assembly Mapper Adaptor-internal cache of
** Ensembl Sequence Regions that map to more than one location.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::
**     cache_seq_ids_with_mult_assemblys
** @param [u] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyMapperAdaptorMultipleMappingsInit(
    EnsPAssemblyMapperAdaptor ama)
{
    ajuint identifier = 0;
    
    ajuint *Pidentifier = NULL;
    
    AjBool *Pbool = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    /*
     ajDebug("assemblyMapperAdaptorMultipleMappingsInit\n"
	     "  ama %p\n",
	     ama);
     */
    
    if(!ama)
	return ajFalse;
    
    if(ama->MultipleMappings)
	return ajTrue;
    
    ama->MultipleMappings = MENSTABLEUINTNEW(0);
    
    dba = ensAssemblyMapperAdaptorGetDatabaseAdaptor(ama);
    
    statement = ajFmtStr("SELECT "
			 "seq_region_attrib.seq_region_id "
			 "FROM "
			 "seq_region_attrib, "
			 "attrib_type, "
			 "seq_region, "
			 "coord_system "
			 "WHERE "
			 "seq_region_attrib.attrib_type_id = "
			 "attrib_type.attrib_type_id "
			 "AND "
			 "attrib_type.code = 'MultAssem' "
			 "AND "
			 "seq_region_attrib.seq_region_id = "
			 "seq_region.seq_region_id "
			 "AND "
			 "seq_region.coord_system_id = "
			 "coord_system.coord_system_id "
			 "AND "
			 "coord_system.species_id = %u",
			 ensDatabaseAdaptorGetIdentifier(dba));
    
    sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier = 0;
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, &identifier);
	
	AJNEW0(Pidentifier);
	
	*Pidentifier = identifier;
	
	AJNEW0(Pbool);
	
	*Pbool = ajTrue;
	
	if(ajTableFetch(ama->MultipleMappings, (const void *) Pidentifier))
	{
	    ajWarn("assemblyMapperAdaptorMultipleMappingsInit already "
		   "cached Ensembl Sequence region with identifier %u.\n",
		   *Pidentifier);
	    
	    AJFREE(Pidentifier);
	    AJFREE(Pbool);
	}
	else
	    ajTablePut(ama->MultipleMappings,
		       (void *) Pidentifier,
		       (void *) Pbool);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorNew ******************************************
**
** Default constructor for an Ensembl Assembly Mapper Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::new
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor or NULL
** @@
******************************************************************************/

EnsPAssemblyMapperAdaptor ensAssemblyMapperAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPAssemblyMapperAdaptor ama = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorNew\n"
	     "  dba %p\n",
	     dba);
     */
    
    if(!dba)
	return NULL;
    
    AJNEW0(ama);
    
    ama->Adaptor = dba;
    
    ama->AsmMapperCache = ajTablestrNewLen(0);
    
    assemblyMapperAdaptorMultipleMappingsInit(ama);
    
    return ama;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Assembly Mapper Adaptor.
**
** @fdata [EnsPAssemblyMapperAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an EnsPAssemblyMapperAdaptor object
**
** @argrule * Pama [EnsPAssemblyMapperAdaptor*] Assembly Mapper Adaptor
**                                              object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @funcstatic assemblyMapperAdaptorMultipleMappingsExit **********************
**
** Clears the Ensembl Assembly Mapper Adaptor-internal cache of
** Ensembl Sequence Regions that map to more than one location.
**
** @param [u] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyMapperAdaptorMultipleMappingsExit(
    EnsPAssemblyMapperAdaptor ama)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajint i = 0;
    
    if(!ama)
	return ajFalse;
    
    /*
     ajDebug("ensAsemblyMapperAdaptorMultipleMappingsExit\n"
	     "  ama %p\n",
	     ama);
     */
    
    /* Clear the Multiple Mappings AJAX Table. */
    
    if(ama->MultipleMappings)
    {
        ajTableToarray(ama->MultipleMappings, &keyarray, &valarray);
	
        /* Free the ajuint key and the AjBool value data. */
	
        for(i = 0; keyarray[i]; i++)
        {
            AJFREE(keyarray[i]);
            AJFREE(valarray[i]);
        }
	
        AJFREE(keyarray);
	AJFREE(valarray);
    }
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorCacheClear ***********************************
**
** Clears the Ensembl Assembly Mapper Adaptor-internal cache of
** Ensembl Assembly Mappers.
**
** @param [u] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyMapperAdaptorCacheClear(EnsPAssemblyMapperAdaptor ama)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajint i = 0;
    
    if(!ama)
	return ajFalse;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorCacheClear\n"
	     "  ama %p\n",
	     ama);
     */
    
    /* Delete the Assembly Mapper Cache. */
    
    if(ama->AsmMapperCache)
    {
        ajTableToarray(ama->AsmMapperCache, &keyarray, &valarray);
	
        /* Free the AJAX String key and Ensembl Assembly Mapper value data. */
	
        for(i = 0; keyarray[i]; i++)
        {
            ajTableRemove(ama->MultipleMappings, (const void *) keyarray[i]);
	    
	    ajStrDel((AjPStr *) &keyarray[i]);
	    
            ensAssemblyMapperDel((EnsPAssemblyMapper *) &valarray[i]);
        }
	
        AJFREE(keyarray);
	AJFREE(valarray);
    }
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorDel ******************************************
**
** Default destructor for an Ensembl Assembly Mapper Adaptor.
**
** @param [d] Pama [EnsPAssemblyMapperAdaptor*] Ensembl Assembly Mapper
**                                              Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensAssemblyMapperAdaptorDel(EnsPAssemblyMapperAdaptor* Pama)
{
    EnsPAssemblyMapperAdaptor pthis = NULL;
    
    if(!Pama)
	return;
    
    if(!*Pama)
	return;

    pthis = *Pama;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorDel\n"
	     "  *Pama %p\n",
	     *Pama);
     */
    
    /* Clear the Assembly Mapper AJAX Table. */
    
    ensAssemblyMapperAdaptorCacheClear(pthis);
    
    ajTableFree(&pthis->AsmMapperCache);
    
    /* Clear the Multiple Mappings AJAX Table. */
    
    assemblyMapperAdaptorMultipleMappingsExit(pthis);
    
    ajTableFree(&pthis->MultipleMappings);
    
    AJFREE(pthis);

    *Pama = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Assembly Mapper Adaptor object.
**
** @fdata [EnsPAssemblyMapperAdaptor]
** @fnote None
**
** @nam3rule Get Return Assembly Mapper Adaptor attribute(s)
** @nam4rule GetDatabaseAdaptor Return the Ensembl Database Adaptor
**
** @argrule * ama [const EnsPAssemblyMapperAdaptor] Ensembl Assembly
**                                                  Mapper Adaptor
**
** @valrule DatabaseAdaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensAssemblyMapperAdaptorGetDatabaseAdaptor ***************************
**
** Get the Ensembl Database Adaptor element of an
** Ensembl Assembly Mapper Adaptor.
**
** @param [r] ama [const EnsPAssemblyMapperAdaptor] Ensembl Assembly
**                                                  Mapper Adaptor
**
** @return [EnsPDatabaseAdaptor] Ensembl Database Adaptor or NULL
** @@
******************************************************************************/

EnsPDatabaseAdaptor ensAssemblyMapperAdaptorGetDatabaseAdaptor(
    const EnsPAssemblyMapperAdaptor ama)
{
    if(!ama)
	return NULL;
    
    return ama->Adaptor;
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Assembly Mapper objects from an
** Ensembl Core database.
**
** @fdata [EnsPAssemblyMapperAdaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Assembly Mapper object(s)
** @nam4rule FetchAll Retrieve all Ensembl Assembly Mapper objects
** @nam5rule FetchAllBy Retrieve all Ensembl Assembly Mapper objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Assembly Mapper object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPAssemblyMapperAdaptor] Ensembl Assembly
**                                                      Mapper Adaptor
** @argrule FetchAll amlist [AjPList] AJAX List of Ensembl Assembly Mappers
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ensAssemblyMapperAdaptorFetchByCoordSystems **************************
**
** Fetch an Ensembl Assembly Mapper via an Ensembl Coordinate System pair.
**
** The caller is responsible for deleting the Ensembl Assembly Mapper.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::fetch_by_CoordSystems
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [r] cs1 [EnsPCoordSystem] Ensembl Coordinate System
** @param [r] cs2 [EnsPCoordSystem] Ensembl Coordinate System
**
** @return [EnsPAssemblyMapper] Ensembl Assembly Mapper or NULL
** @@
******************************************************************************/

EnsPAssemblyMapper ensAssemblyMapperAdaptorFetchByCoordSystems(
    EnsPAssemblyMapperAdaptor ama,
    EnsPCoordSystem cs1,
    EnsPCoordSystem cs2)
{
    const AjPList mappath = NULL;
    AjIList iter          = NULL;
    
    AjPStr keystr = NULL;
    
    EnsPAssemblyMapper am = NULL;
    
    EnsPChainedAssemblyMapper cam = NULL;
    
    EnsPGenericAssemblyMapper gam = NULL;
    
    EnsPTopLevelAssemblyMapper tlam = NULL;
    
    EnsPCoordSystem srccs = NULL;
    EnsPCoordSystem midcs = NULL;
    EnsPCoordSystem trgcs = NULL;

    EnsPCoordSystemAdaptor csa = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorFetchByCoordSystems\n"
	     "  ama %p\n"
	     "  cs1 %p\n"
	     "  cs2 %p\n",
	     ama,
	     cs1,
	     cs2);
     
     ensCoordSystemTrace(cs1, 1);
     
     ensCoordSystemTrace(cs2, 1);
     */
    
    if(!ama)
    {
        ajDebug("ensAssemblyMapperAdaptorFetchByCoordSystems requires an "
                "Ensembl Assembly Mapper Adaptor.\n");
	
        return NULL;
    }
    
    if(!cs1)
    {
        ajDebug("ensAssemblyMapperAdaptorFetchByCoordSystems requires "
                "Ensembl Coordinate System one.\n");
	
        return NULL;
    }
    
    if(!cs2)
    {
        ajDebug("ensAssemblyMapperAdaptorFetchByCoordSystems requires "
                "Ensembl Coordinate System two.\n");
	
        return NULL;
    }
    
    /*
    ** FIXME: This is no longer the case. Mappings for Sequence Regions with
    ** multiple locations within the same Coordinate System are now possible.
    ** 
    ** if (ensCoordSystemMatch(cs1, cs2))
    ** ajFatal("ensAssemblyMapperAdaptorFetchByCoordSystems cannot create "
    **	     "an Ensembl Assembly Mapper between identical "
    **	     "Ensembl Coordinate Systems.");
    */
    
    if(ensCoordSystemIsTopLevel(cs1))
    {
	tlam = ensTopLevelAssemblyMapperNew(ama, cs1, cs2);
	
	am = ensAssemblyMapperNew(ama,
				  (EnsPGenericAssemblyMapper) NULL,
				  (EnsPChainedAssemblyMapper) NULL,
				  tlam);
	
	ensTopLevelAssemblyMapperDel(&tlam);
	
	return am;
    }
    
    if(ensCoordSystemIsTopLevel(cs2))
    {
	tlam = ensTopLevelAssemblyMapperNew(ama, cs2, cs1);
	
	am = ensAssemblyMapperNew(ama,
				  (EnsPGenericAssemblyMapper) NULL,
				  (EnsPChainedAssemblyMapper) NULL,
				  tlam);
	
	ensTopLevelAssemblyMapperDel(&tlam);
	
	return am;
    }
    
    csa = ensRegistryGetCoordSystemAdaptor(ama->Adaptor);
    
    mappath = ensCoordSystemAdaptorGetMappingPath(csa, cs1, cs2);
    
    if(!ajListGetLength(mappath))
    {
	ajWarn("ensAssemblyMapperAdaptorFetchByCoordSystems "
	       "got no mapping path between Coordinate Systems "
	       "'%S:%S' and '%S:%S'.\n",
	       ensCoordSystemGetName(cs1),
	       ensCoordSystemGetVersion(cs1),
	       ensCoordSystemGetName(cs2),
	       ensCoordSystemGetVersion(cs2));
	
	return NULL;
    }
    
    keystr = ajStrNew();
    
    iter = ajListIterNewread(mappath);
    
    while(!ajListIterDone(iter))
    {
	srccs = (EnsPCoordSystem) ajListIterGet(iter);
	
	ajFmtPrintAppS(&keystr, "%u:", ensCoordSystemGetIdentifier(srccs));
    }
    
    ajListIterDel(&iter);
    
    am = (EnsPAssemblyMapper)
	ajTableFetch(ama->AsmMapperCache, (const void *) keystr);
    
    if(!am)
    {
	if(ajListGetLength(mappath) == 1)
	    ajFatal("ensAssemblyMapperAdaptorFetchByCoordSystems "
		    "got an incorrect mapping path from Ensembl Core 'meta' "
		    "table. Zero step mapping path defined between "
		    "Coordinate Systems '%S:%S' and '%S:%S'.",
		    ensCoordSystemGetName(cs1), ensCoordSystemGetVersion(cs1),
		    ensCoordSystemGetName(cs2), ensCoordSystemGetVersion(cs2));
	else if(ajListGetLength(mappath) == 2)
	{
	    ajListPeekNumber(mappath, 0, (void **) &srccs);
	    
	    ajListPeekNumber(mappath, 1, (void **) &trgcs);
	    
	    gam = ensGenericAssemblyMapperNew(ama, srccs, trgcs);
	    
	    am = ensAssemblyMapperNew(ama,
				      gam,
				      (EnsPChainedAssemblyMapper) NULL,
				      (EnsPTopLevelAssemblyMapper) NULL);
	    
	    /*
	     ** The Assembly Mapper reference counter is incremented with
	     ** the return statement.
	     */
	    
	    ajTablePut(ama->AsmMapperCache,
		       (void *) ajStrNewS(keystr),
		       (void *) am);
	    
	    /*
	     ** If you want multiple pieces on two Ensembl Sequence Regions
	     ** to map to each other you need to make an 'assembly.mapping'
	     ** entry in the 'meta' table of the Ensembl Core database, that
	     ** is seperated with a '#' character instead of a '|' character.
	     */
	    
	    ensGenericAssemblyMapperDel(&gam);
	}
	else if(ajListGetLength(mappath) == 3)
	{
	    ajListPeekNumber(mappath, 0, (void **) &srccs);
	    
	    ajListPeekNumber(mappath, 1, (void **) &midcs);
	    
	    ajListPeekNumber(mappath, 2, (void **) &trgcs);
	    
	    cam = ensChainedAssemblyMapperNew(ama, srccs, midcs, trgcs);
	    
	    am = ensAssemblyMapperNew(ama,
				      (EnsPGenericAssemblyMapper) NULL,
				      cam,
				      (EnsPTopLevelAssemblyMapper) NULL);
	    
	    /*
	    ** The Assembly Mapper reference counter is incremented with
	    ** the return statement.
	    */
	    
	    ajTablePut(ama->AsmMapperCache,
		       (void *) ajStrNewS(keystr),
		       (void *) am);
	    
	    ajStrAssignClear(&keystr);
	    
            /*
	    ** Register this Chained Assembly Mapper also for
	    ** Coordinate System identifiers in reverse order.
	    */
	    
	    iter = ajListIterNewreadBack(mappath);
	    
	    while(!ajListIterDoneBack(iter))
	    {
		srccs = (EnsPCoordSystem) ajListIterGetBack(iter);
		
		ajFmtPrintAppS(&keystr,
			       "%u:",
			       ensCoordSystemGetIdentifier(srccs));
	    }
	    
	    ajListIterDel(&iter);
	    
	    /*
	    ** Registering this Chained Assembly Mapper twice requires
	    ** increasing its reference counter.
	    */
	    
	    ajTablePut(ama->AsmMapperCache,
		       (void *) ajStrNewS(keystr),
		       (void *) ensAssemblyMapperNewRef(am));
	    
	    /*
	    ** In multi-step mapping it is possible to get requests with the
	    ** coordinate system ordering reversed since both mapping
	    ** directions cache on both orderings just in case
	    ** e.g.
	    ** chromosome <-> contig <-> clone
	    ** and
	    ** clone <-> contig <-> chromosome
	    */
	    
	    ensChainedAssemblyMapperDel(&cam);
	}
	else
	    ajFatal("ensAssemblyMapperAdaptorFetchByCoordSystems "
		    "got incorrect mapping path of length %u defined "
		    "between Coordinate Systems '%S:%S' and '%S:%S'.",
		    ajListGetLength(mappath),
		    ensCoordSystemGetName(cs1),
		    ensCoordSystemGetVersion(cs1),
		    ensCoordSystemGetName(cs2),
		    ensCoordSystemGetVersion(cs2));
    }
    
    ajStrDel(&keystr);
    
    return ensAssemblyMapperNewRef(am);
}




/* @func ensAssemblyMapperAdaptorSeqRegionIdsToNames **************************
**
** Converts database-internal Ensembl Sequence Region identifiers to their
** names.
**
** The caller is responsible for deleting the
** AJAX Strings before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::seq_ids_to_regions
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::_seq_region_id_to_name
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [r] identifiers [const AjPList] AJAX List of AJAX unsigned integers
**                                        of Ensembl Sequence Region
**                                        identifiers
** @param [u] names [AjPList] AJAX List of AJAX Strings of
**                            Ensembl Sequence Region names
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: This function is obsolete since the Ensembl Sequence Region records
** its identifier internally.
** TODO: Suggest this to the Ensembl Core team.
*/

AjBool ensAssemblyMapperAdaptorSeqRegionIdsToNames(
    EnsPAssemblyMapperAdaptor ama,
    const AjPList identifiers,
    AjPList names)
{
    ajuint *Pid = NULL;
    
    AjIList iter = NULL;
    
    EnsPSeqRegion sr         = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    if(!ama)
	return ajFalse;
    
    if(!identifiers)
	return ajFalse;
    
    if(!names)
	return ajFalse;
    
    sra = ensRegistryGetSeqRegionAdaptor(ama->Adaptor);
    
    iter = ajListIterNewread(identifiers);
    
    while(!ajListIterDone(iter))
    {
	Pid = (ajuint *) ajListIterGet(iter);
	
	ensSeqRegionAdaptorFetchByIdentifier(sra, *Pid, &sr);
	
	if(sr)
	    ajListPushAppend(names,
			     (void *) ajStrNewS(ensSeqRegionGetName(sr)));
	else
	    ajWarn("ensAssemblyMapperAdaptorSeqRegionIdsToNames could not "
		   "resolve Sequence Region identifier %u to a "
		   "Sequence Region.\n",
		   *Pid);
	
	ensSeqRegionDel(&sr);
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorSeqRegionNamesToIds **************************
**
** Converts Ensembl Sequence Region names to their internal database
** identifiers.
**
** The caller is responsible for deleting the
** AJAX unsigned integers before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::seq_regions_to_ids
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::_seq_region_name_to_id
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [r] cs [EnsPCoordSystem] Ensembl Coordinate System
** @param [r] names [const AjPList] AJAX List of AJAX Strings of
**                                  Ensembl Sequence Region names
** @param [u] identifiers [AjPList] AJAX List of AJAX unsigned integers of
**                                  Ensembl Sequence Region identifiers
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: This function is obsolete since the Ensembl Sequence Region records
** its identifier internally.
** TODO: Suggest this to the Ensembl Core team.
*/

AjBool ensAssemblyMapperAdaptorSeqRegionNamesToIds(
    EnsPAssemblyMapperAdaptor ama,
    EnsPCoordSystem cs,
    const AjPList names,
    AjPList identifiers)
{
    ajuint *Pid  = NULL;
    AjIList iter = NULL;
    
    AjPStr srname = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    if(!ama)
	return ajFalse;
    
    if(!cs)
	return ajFalse;
    
    if(!names)
	return ajFalse;
    
    if(!identifiers)
	return ajFalse;
    
    sra = ensRegistryGetSeqRegionAdaptor(ama->Adaptor);
    
    iter = ajListIterNewread(names);
    
    while(!ajListIterDone(iter))
    {
	srname = (AjPStr) ajListIterGet(iter);
	
	ensSeqRegionAdaptorFetchByName(sra, cs, srname, &sr);
	
	if(sr)
	{
	    AJNEW0(Pid);
	    
	    *Pid = ensSeqRegionGetIdentifier(sr);
	    
	    ajListPushAppend(identifiers, (void *) Pid);
	}
	else
	    ajWarn("ensAssemblyMapperAdaptorSeqRegionNamesToIds could not "
		   "resolve Sequence Region name '%S' and "
		   "Coordinate System identifier %u to a valid "
		   "Sequence Region.\n",
		   srname,
		   ensCoordSystemGetIdentifier(cs));
	
	ensSeqRegionDel(&sr);
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorRegisterAssembled ****************************
**
** Registers an assembled Ensembl Sequence Region in an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::register_assembled
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] asmsrid [ajuint] Assembled Ensembl Sequence Region identifier
** @param [r] regstart [ajint] Start coordinate of the assembled
**                             Ensembl Sequence Region
** @param [r] regend [ajint] End ccordinate of the assembled
**                           Ensembl Sequence Region
**
** @return [AjBool] ajTrue on success, ajFalse otherwise.
** @@
** Declares an assembled Ensembl Sequence Region to the
** Ensembl Assembly Mapper.
** This extracts the relevant data from the 'assembly' table and stores it in
** the Ensembl Mapper internal to the Ensembl Assembly Mapper.
** It therefore must be called before any mapping is attempted on that region.
** Otherwise only gaps will be returned. Note that the Ensembl Assembly Mapper
** automatically calls this function when the need arises.
******************************************************************************/

/*
** FIXME: The Perl API
** Bio::EnsEMBL::DBSQL::AssenblyMapperAdaptor::register_assembled method
** expects a sequence region identifier not a sequence region name as in the
** documentation. This is only apparent from the SQL statement.
** The documentation should be updated and the variable name should probably
** change back to seq_region_id as it was in CVS revision 1.39.
** The Perl API lacks a return statement.
*/

AjBool ensAssemblyMapperAdaptorRegisterAssembled(
    EnsPAssemblyMapperAdaptor ama,
    EnsPGenericAssemblyMapper gam,
    ajuint asmsrid,
    ajint regstart,
    ajint regend)
{
    register ajint i = 0;
    
    ajint ori  = 0;
    ajint temp = 0;
    
    ajint chunkstart = 0;
    ajint chunkend   = 0;
    
    ajint chunkregionstart = 0;
    ajint chunkregionend   = 0;
    
    ajint asmsrstart = 0;
    ajint asmsrend   = 0;
    
    ajuint cmpsrid    = 0;
    ajint cmpsrstart  = 0;
    ajint cmpsrend    = 0;
    ajint cmpsrlength = 0;
    
    AjPList chunkregions = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli = NULL;
    AjPSqlRow sqlr = NULL;
    
    AjPStr statement = NULL;
    AjPStr cmpsrname = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    EnsPMapperRange chunkregion = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorRegisterAssembled\n"
	     "  ama %p\n"
	     "  gam %p\n"
	     "  asmsrid %u\n"
	     "  regstart %d\n"
	     "  regend %d\n",
	     ama,
	     gam,
	     asmsrid,
	     regstart,
	     regend);
     */
    
    if(!ama)
	return ajFalse;
    
    if(!gam)
	return ajFalse;
    
    if(!asmsrid)
	return ajFalse;
    
    /*
    ** Split up the region to be registered into fixed chunks.
    ** This allows us to keep track of regions that have already been
    ** registered and also works under the assumption that if a small region
    ** is requested it is likely that other requests will be made in the
    ** vicinity (the minimum size registered the chunksize (2^chunkfactor)).
    */
    
    chunkregions = ajListNew();
    
    /*
    ** Determine the span of chunks.
    ** A bitwise shift right is a fast and easy integer division.
    */
    
    /*
    ** FIXME: regstart and regend are signed. Does bit shifting or a division
    ** make sense in this case?
    ** It may be sufficient to use unsigned integers???
    */
    
    chunkstart = regstart >> assemblyMapperChunkFactor;
    
    chunkend = regend >> assemblyMapperChunkFactor;
    
    /*
     ** Inserts have start == end + 1, on boundary condition start chunk
     ** could be less than end chunk.
     */
    
    if (regstart == (regend + 1))
    {
	temp = regstart;
	
	regstart = regend;
	
	regend = temp;
    }
    
    /* Find regions of continuous unregistered chunks. */
    
    for(i = chunkstart; i <= chunkend; i++)
    {
	if(ensGenericAssemblyMapperHaveRegisteredAssembled(gam, asmsrid, i))
	{
	    if(chunkregionstart >= 0)
	    {
		/* This is the end of an unregistered region. */
		
		chunkregion =
		ensMapperRangeNew(chunkregionstart <<
				  assemblyMapperChunkFactor,
				  ((chunkregionend + 1) <<
				   assemblyMapperChunkFactor) - 1);
		
		ajListPushAppend(chunkregions, (void *) chunkregion);
		
		chunkregionstart = -1;
		
		chunkregionend = -1;
	    }
	}
	else
	{
	    if(chunkregionstart < 0)
		chunkregionstart = i;
	    
	    chunkregionend = i + 1;
	    
	    ensGenericAssemblyMapperRegisterAssembled(gam, asmsrid, i);
	}
    }
    
    /* The last part may have been an unregistered region too. */
    
    if(chunkregionstart >= 0)
    {
	chunkregion =
	ensMapperRangeNew(chunkregionstart << assemblyMapperChunkFactor,
			  ((chunkregionend + 1) << assemblyMapperChunkFactor) -
			  1);
	
	ajListPushAppend(chunkregions, (void *) chunkregion);
    }
    
    if(!ajListGetLength(chunkregions))
	return ajTrue;
    
    /* Keep the Mapper to a reasonable size. */
    
    if(ensGenericAssemblyMapperGetPairCount(gam) > gam->MaxPairCount)
    {
	ensGenericAssemblyMapperClear(gam);
	
	while(ajListPop(chunkregions, (void **) &chunkregion))
	    ensMapperRangeDel(&chunkregion);
	
	/* After clearing the cache, everything needs to be re-registered. */
	
	chunkregion =
	    ensMapperRangeNew(chunkstart << assemblyMapperChunkFactor,
			      ((chunkend + 1) << assemblyMapperChunkFactor) -
			      1);
	
	ajListPushAppend(chunkregions, (void *) chunkregion);
	
	for(i = chunkstart; i <= chunkend; i++)
	    ensGenericAssemblyMapperRegisterAssembled(gam, asmsrid, i);
    }
    
    /*
     ** Retrieve the description of how the assembled region is made from
     ** component regions for each of the continuous blocks of unregistered,
     ** chunked regions.
     */
    
    sra = ensRegistryGetSeqRegionAdaptor(ama->Adaptor);
    
    while(ajListPop(chunkregions, (void **) &chunkregion))
    {
	statement =
	ajFmtStr("SELECT "
		 "assembly.cmp_start, "
		 "assembly.cmp_end, "
		 "assembly.cmp_seq_region_id, "
		 "seq_region.name, "
		 "seq_region.length, "
		 "assembly.ori, "
		 "assembly.asm_start, "
		 "assembly.asm_end "
		 "FROM "
		 "assembly, "
		 "seq_region "
		 "WHERE "
		 "assembly.asm_seq_region_id = %u "
		 "AND "
		 "assembly.asm_end >= %d "
		 "AND "
		 "assembly.asm_start <= %d "
		 "AND "
		 "assembly.cmp_seq_region_id = seq_region.seq_region_id "
		 "AND "
		 "seq_region.coord_system_id = %u",
		 asmsrid,
		 ensMapperRangeGetStart(chunkregion),
		 ensMapperRangeGetEnd(chunkregion),
		 ensCoordSystemGetIdentifier(gam->CmpCoordSystem));
	
	sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
	
	sqli = ajSqlRowIterNew(sqls);
	
	while(!ajSqlRowIterDone(sqli))
	{
	    cmpsrstart = 0;
	    cmpsrend = 0;
	    cmpsrid = 0;
	    cmpsrname = ajStrNew();
	    cmpsrlength = 0;
	    ori = 0;
	    asmsrstart = 0;
	    asmsrend = 0;
	    
	    sqlr = ajSqlRowIterGet(sqli);
	    
	    ajSqlColumnToInt(sqlr, &cmpsrstart);
	    ajSqlColumnToInt(sqlr, &cmpsrend);
	    ajSqlColumnToUint(sqlr, &cmpsrid);
	    ajSqlColumnToStr(sqlr, &cmpsrname);
	    ajSqlColumnToInt(sqlr, &cmpsrlength);
	    ajSqlColumnToInt(sqlr, &ori);
	    ajSqlColumnToInt(sqlr, &asmsrstart);
	    ajSqlColumnToInt(sqlr, &asmsrend);
	    
	    /*
	    ** Only load unregistered Ensembl Sequence Regions of the
	    ** Ensembl Mapper and Sequence Regions that map to
	    ** multiple locations.
	    */
	    
	    if(!(ensGenericAssemblyMapperHaveRegisteredComponent(gam, cmpsrid)
                 &&
                 (!assemblyMapperAdaptorHasMultipleMappings(ama, cmpsrid))))
	    {
		ensGenericAssemblyMapperRegisterComponent(gam, cmpsrid);
		
		ensMapperAddCoordinates(gam->Mapper,
					asmsrid,
					asmsrstart,
					asmsrend,
					ori,
					cmpsrid,
					cmpsrstart,
					cmpsrend);
		
		/*
		 ** Add the component Ensembl Sequence Region into the
		 ** Ensembl Sequence Region Adaptor cache.
		 */
		
		sr = ensSeqRegionNew(sra,
				     cmpsrid,
				     gam->CmpCoordSystem,
				     cmpsrname,
				     cmpsrlength);
		
		ensSeqRegionAdaptorCacheInsert(sra, &sr);
		
		ensSeqRegionDel(&sr);
	    }
	    
	    ajStrDel(&cmpsrname);
	}
	
	ajSqlRowIterDel(&sqli);
	
	ajSqlStatementDel(&sqls);
	
	ajStrDel(&statement);
	
	ensMapperRangeDel(&chunkregion);
    }
    
    ajListFree(&chunkregions);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorRegisterComponent ****************************
**
** Registers a component Ensembl Sequence Region in an
** Ensembl Generic Assembly Mapper.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::register_component
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
** @param [r] cmpsrid [ajuint] Component Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue on success, ajFalse otherwise.
** @@
** Declares a component Ensembl Sequence Region to the Ensembl AssemblyMapper.
** This extracts the relevant data from the 'assembly' table and stores it in
** the Ensembl Mapper internal to the Ensembl Assembly Mapper.
** It therefore must be called before any mapping is attempted on that region.
** Otherwise only gaps will be returned. Note that the Ensembl Assembly Mapper
** automatically calls this function when the need arises.
******************************************************************************/

/*
** FIXME: The Perl API
** Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::register_component method
** expects a sequence region identifier not a sequence region name as in the
** documentation. This is only apparent from the SQL statement.
** The documentation should be updated and the variable name should probably
** change back to seq_region_id as it was in CVS revision 1.39.
*/

AjBool ensAssemblyMapperAdaptorRegisterComponent(
    EnsPAssemblyMapperAdaptor ama,
    EnsPGenericAssemblyMapper gam,
    ajuint cmpsrid)
{
    ajuint asmsrid    = 0;
    ajint asmsrstart  = 0;
    ajint asmsrend    = 0;
    ajint asmsrlength = 0;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli = NULL;
    AjPSqlRow sqlr = NULL;
    
    AjPStr statement = NULL;
    AjPStr asmsrname = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorRegisterComponent\n"
	     "  ama %p\n"
	     "  gam %p\n"
	     "  cmpsrid %u\n",
	     ama,
	     gam,
	     cmpsrid);
     */
    
    if(!ama)
	return ajFalse;
    
    if(!gam)
	return ajFalse;
    
    if(!cmpsrid)
	return ajFalse;
    
    if(ensGenericAssemblyMapperHaveRegisteredComponent(gam, cmpsrid) &&
       (! assemblyMapperAdaptorHasMultipleMappings(ama, cmpsrid)))
	return ajTrue;
    
    /*
    ** Determine what part of the assembled Sequence Region this component
    ** region makes up.
    */
    
    statement =
	ajFmtStr("SELECT "
		 "assembly.asm_start, "
		 "assembly.asm_end, "
		 "assembly.asm_seq_region_id, "
		 "seq_region.name, "
		 "seq_region.length "
		 "FROM "
		 "assembly, seq_region "
		 "WHERE "
		 "assembly.cmp_seq_region_id = %u "
		 "AND "
		 "assembly.asm_seq_region_id = seq_region.seq_region_id "
		 "AND "
		 "seq_region.coord_system_id = %u",
		 cmpsrid,
		 ensCoordSystemGetIdentifier(gam->AsmCoordSystem));
    
    sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
    
    if(!ajSqlStatementGetSelectedRows(sqls))
    {
	ensGenericAssemblyMapperRegisterComponent(gam, cmpsrid);
	
	ajSqlStatementDel(&sqls);
	
	ajStrDel(&statement);
	
	return ajTrue;
    }
    
    if(ajSqlStatementGetSelectedRows(sqls) > 1)
	ajFatal("ensAssemblyMapperAdaptorRegisterComponent "
		"multiple assembled Sequence Regions for single "
		"component Sequence Region with identifier %u.\n"
		"Remember that multiple mappings require the '#' operator "
		"in the Ensembl Core meta.meta_value entries with "
		"meta.meta_key = 'assembly.mapping'.\n",
		cmpsrid);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	asmsrstart = 0;
	asmsrend = 0;
	asmsrid = 0;
	asmsrname = ajStrNew();
	asmsrlength = 0;
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToInt(sqlr, &asmsrstart);
	ajSqlColumnToInt(sqlr, &asmsrend);
	ajSqlColumnToUint(sqlr, &asmsrid);
	ajSqlColumnToStr(sqlr, &asmsrname);
	ajSqlColumnToInt(sqlr, &asmsrlength);
	
	/*
	** Register the corresponding assembled region. This allows us to
	** register things in assembled chunks which allows us to:
	** (1) Keep track of what assembled regions are registered
	** (2) Use locality of reference
	** if they want something in same general region it will already
	** be registered.
	*/
	
	ensAssemblyMapperAdaptorRegisterAssembled(ama,
						  gam,
						  asmsrid,
						  asmsrstart,
						  asmsrend);
	
	/*
	** Add the assembled Ensembl Sequence Region into the
	** Ensembl Sequence Region Adaptor cache.
	*/
	
	sr = ensSeqRegionNew(sra,
			     asmsrid,
			     gam->AsmCoordSystem,
			     asmsrname,
			     asmsrlength);
	
	ensSeqRegionAdaptorCacheInsert(sra, &sr);
	
	ensSeqRegionDel(&sr);
	
	ajStrDel(&asmsrname);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorRegisterAll **********************************
**
** Registers all component Ensembl Sequence Regions in an
** Ensembl Generic Assembly Mapper.
**
** This function registers the entire set of mappings between two
** Ensembl Coordinate Systems in an Ensembl Assembly Mapper.
** This will use a lot of memory but will be much more efficient when doing a
** lot of mapping which is spread over the entire genome.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::register_all
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] gam [EnsPGenericAssemblyMapper] Ensembl Generic Assembly Mapper
**
** @return [AjBool] ajTrue on success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensAssemblyMapperAdaptorRegisterAll(EnsPAssemblyMapperAdaptor ama,
                                           EnsPGenericAssemblyMapper gam)
{
    register ajint i = 0;
    
    ajint ori = 0;
    ajint endchunk = 0;
    
    ajuint asmsrid = 0;
    ajint asmsrstart = 0;
    ajint asmsrend = 0;
    ajint asmsrlength = 0;
    
    ajuint cmpsrid = 0;
    ajint cmpsrstart = 0;
    ajint cmpsrend = 0;
    ajint cmpsrlength = 0;
    
    AjISqlRow sqli = NULL;
    AjPSqlStatement sqls = NULL;
    AjPSqlRow sqlr = NULL;
    
    AjPStr asmsrname = NULL;
    AjPStr cmpsrname = NULL;
    
    AjPStr sqlfmt = NULL;
    AjPStr statement = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorRegisterAll\n"
	     "  ama %p\n"
	     "  gam %p\n",
	     ama,
	     gam);
     */
    
    dba = ensAssemblyMapperAdaptorGetDatabaseAdaptor(gam->Adaptor);
    
    sra = ensRegistryGetSeqRegionAdaptor(dba);
    
    sqlfmt = ajStrNewC("SELECT "
		       "assembly.cmp_start, "
		       "assembly.cmp_end, "
		       "assembly.cmp_seq_region_id, "
		       "cmp_sr.name, "
		       "cmp_sr.length, "
		       "assembly.ori, "
		       "assembly.asm_start, "
		       "assembly.asm_end, "
		       "assembly.asm_seq_region_id, "
		       "asm_sr.name, "
		       "asm_sr.length "
		       "FROM "
		       "assembly, "
		       "seq_region asm_sr, "
		       "seq_region cmp_sr "
		       "WHERE "
		       "assembly.cmp_seq_region_id = cmp_sr.seq_region_id "
		       "AND "
		       "assembly.asm_seq_region_id = asm_sr.seq_region_id "
		       "AND "
		       "cmp_sr.coord_system_id = %u "
		       "AND "
		       "asm_sr.coord_system_id = %u");
    
    statement = ajFmtStr(ajStrGetPtr(sqlfmt),
			 ensCoordSystemGetIdentifier(gam->CmpCoordSystem),
			 ensCoordSystemGetIdentifier(gam->AsmCoordSystem));
    
    sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	cmpsrstart = 0;
	cmpsrend = 0;
	cmpsrid = 0;
	cmpsrname = ajStrNew();
	cmpsrlength = 0;
	ori = 0;
	asmsrstart = 0;
	asmsrend = 0;
	asmsrid = 0;
	asmsrname = ajStrNew();
	asmsrlength = 0;
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToInt(sqlr, &cmpsrstart);
	ajSqlColumnToInt(sqlr, &cmpsrend);
	ajSqlColumnToUint(sqlr, &cmpsrid);
	ajSqlColumnToStr(sqlr, &cmpsrname);
	ajSqlColumnToInt(sqlr, &cmpsrlength);
	ajSqlColumnToInt(sqlr, &ori);
	ajSqlColumnToInt(sqlr, &asmsrstart);
	ajSqlColumnToInt(sqlr, &asmsrend);
	ajSqlColumnToUint(sqlr, &asmsrid);
	ajSqlColumnToStr(sqlr, &asmsrname);
	ajSqlColumnToInt(sqlr, &asmsrlength);
	
	/* Register the component Sequence Region. */
	
	ensGenericAssemblyMapperRegisterComponent(gam, cmpsrid);
	
	ensMapperAddCoordinates(gam->Mapper,
				asmsrid,
				asmsrstart,
				asmsrend,
				ori,
				cmpsrid,
				cmpsrstart,
				cmpsrend);
	
	/*
	** FIXME: The following code should not be neccessary as
	** ensMapperRegisterAssembled should be fast. Nevertheless,
	** the registering process is re-run for each SQL Row.
	**
	** Only register each Assembled Sequence Region once since it
	** requires some work.
	** 
	** if(! $asm_registered{$asm_seq_region_id})
	** {
	**     $asm_registered{$asm_seq_region_id} = 1;
	**     ...
	** }
	*/
	
	/*
	** Register all chunks from the start to the end of the
	** assembled Sequence Region.
	*/
	
	endchunk = asmsrlength >> assemblyMapperChunkFactor;
	
	for(i = 0; i <= endchunk; i++)
	    ensGenericAssemblyMapperRegisterAssembled(gam, asmsrid, i);
	
	/*
	** Add the component Ensembl Sequence Region into the
	** Ensembl Sequence Region Adaptor cache.
	*/
	
	sr = ensSeqRegionNew(sra,
			     cmpsrid,
			     gam->CmpCoordSystem,
			     cmpsrname,
			     cmpsrlength);
	
	ensSeqRegionAdaptorCacheInsert(sra, &sr);
	
	ensSeqRegionDel(&sr);
	
	/*
	** Add the assembled Ensembl Sequence Region into the
	** Ensembl Sequence Region Adaptor cache.
	*/
	
	sr = ensSeqRegionNew(sra,
			     asmsrid,
			     gam->AsmCoordSystem,
			     asmsrname,
			     asmsrlength);
	
	ensSeqRegionAdaptorCacheInsert(sra, &sr);
	
	ensSeqRegionDel(&sr);
	
	/* Clean up. */
	
	ajStrDel(&asmsrname);
	
	ajStrDel(&cmpsrname);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    ajStrDel(&sqlfmt);
    
    return ajTrue;
}




/* @funcstatic assemblyMapperAdaptorBuildCombinedMapper ***********************
**
** Build a combined Mapper after both halves of an
** Ensembl Chained Assembly Mapper have been loaded.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::_build_combined_mapper
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [r] ranges [AjPList] AJAX List of Ensembl Mapper Ranges
** @param [r] srcmidmapper [EnsPMapper] Ensembl Mapper for source to middle
**                                      Coordinate System mapping
** @param [r] trgmidmapper [EnsPMapper] Ensembl Mapper for target to middle
**                                      Coordinate System mapping
** @param [u] srctrgmapper [EnsPMapper] Ensembl Mapper for source to target
**                                      Coordinate System mapping,
**                                      the combined Mapper
** @param [r] srctype [AjPStr] Source mapping type
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
** After both halves of an Ensembl Chained Mapper are loaded this function
** maps all Ensembl Mapper Ranges in the AJAX List and loads the
** Ensembl Mapper Results into the combined (srctrgmapper) Ensembl Mapper.
******************************************************************************/

/*
** FIXME: The Perl API assigns an $istrand variable, but does not use it.
*/

static AjBool assemblyMapperAdaptorBuildCombinedMapper(
    EnsPAssemblyMapperAdaptor ama,
    AjPList ranges,
    EnsPMapper srcmidmapper,
    EnsPMapper trgmidmapper,
    EnsPMapper srctrgmapper,
    const AjPStr srctype)
{
    ajint sum = 0;
    
    ajint totalstart = 0;
    ajint totalend = 0;
    
    AjIList iter = NULL;
    AjPList mrs1 = NULL;
    AjPList mrs2 = NULL;
    
    AjPStr midtype = NULL;
    
    EnsPMapperResult mr1 = NULL;
    EnsPMapperResult mr2 = NULL;
    
    EnsPMapperUnit mu = NULL;
    
    /*
     ajDebug("assemblyMapperAdaptorBuildCombinedMapper\n"
	     "  ama %p\n"
	     "  ranges %p\n"
	     "  srcmidmapper %p\n"
	     "  trgmidmapper %p\n"
	     "  srctrgmapper %p\n"
	     "  srctype '%S'\n",
	     ama,
	     ranges,
	     srcmidmapper,
	     trgmidmapper,
	     srctrgmapper,
	     srctype);
     
     ensMapperTrace(srcmidmapper, 1);
     
     ensMapperTrace(trgmidmapper, 1);
     
     ensMapperTrace(srctrgmapper, 1);
     */
    
    if(!ama)
        return ajFalse;
    
    if(!ranges)
        return ajFalse;
    
    if(!srcmidmapper)
        return ajFalse;
    
    if(!trgmidmapper)
        return ajFalse;
    
    if(!srctrgmapper)
        return ajFalse;
    
    if(!srctype)
        return ajFalse;
    
    midtype = ajStrNewC("middle");
    
    iter = ajListIterNew(ranges);
    
    while(!ajListIterDone(iter))
    {
	mu = (EnsPMapperUnit) ajListIterGet(iter);
	
	sum = 0;
	
	mrs1 = ajListNew();
	
	ensMapperMapCoordinates(srcmidmapper,
				ensMapperUnitGetObjectIdentifier(mu),
				ensMapperUnitGetStart(mu),
				ensMapperUnitGetEnd(mu),
				1,
				srctype,
				mrs1);
	
	while(ajListPop(mrs1, (void **) &mr1))
	{
	    if(ensMapperResultGetType(mr1) == ensEMapperResultGap)
	    {
		sum += ensMapperResultGetGapLength(mr1);
		
		ensMapperResultDel(&mr1);
		
		continue;
	    }
	    
	    /* Feed the results of the first Ensembl Mapper into the second. */
	    
	    mrs2 = ajListNew();
	    
	    ensMapperMapCoordinates(trgmidmapper,
				    ensMapperResultGetObjectIdentifier(mr1),
				    ensMapperResultGetStart(mr1),
				    ensMapperResultGetEnd(mr1),
				    ensMapperResultGetStrand(mr1),
				    midtype,
				    mrs2);
	    
            while(ajListPop(mrs2, (void **) &mr2))
	    {
		if(ensMapperResultGetType(mr2) == ensEMapperResultCoordinate)
		{
		    totalstart = ensMapperUnitGetStart(mu) + sum;
		    
		    totalend = totalstart +
			ensMapperResultGetCoordinateLength(mr2) - 1;
		    
		    if(ajStrMatchC(srctype, "source"))
			ensMapperAddCoordinates(
                            srctrgmapper,
                            ensMapperUnitGetObjectIdentifier(mu),
                            totalstart,
                            totalend,
                            ensMapperResultGetStrand(mr2),
                            ensMapperResultGetObjectIdentifier(mr2),
                            ensMapperResultGetStart(mr2),
                            ensMapperResultGetEnd(mr2));
		    else
			ensMapperAddCoordinates(
                            srctrgmapper,
                            ensMapperResultGetObjectIdentifier(mr2),
                            ensMapperResultGetStart(mr2),
                            ensMapperResultGetEnd(mr2),
                            ensMapperResultGetStrand(mr2),
                            ensMapperUnitGetObjectIdentifier(mu),
                            totalstart,
                            totalend);
		}
		
		sum += ensMapperResultGetLength(mr2);
		
		ensMapperResultDel(&mr2);
	    }
	    
	    ajListFree(&mrs2);
	    
	    ensMapperResultDel(&mr1);
	}
	
	ajListFree(&mrs1);
    }
    
    ajListIterDel(&iter);
    
    ajStrDel(&midtype);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorRegisterChained ******************************
**
** Registers an AJAX List of previously unregistered Ensembl Mapper Ranges in
** an Ensembl Chained Assembly Mapper.
**
** Optionally, only those Ensembl Mapper Ranges can be registered, which map
** onto a particular Ensembl Sequence Region, if its identifier was specified.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::register_chained
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
** @param [r] source [AjPStr] Mapping type for the source
** @param [r] srcsrid [ajuint] Source Ensembl Sequence Region identifier
** @param [r] optsrid [ajuint] Optional Ensembl Sequence Region identifier
** @param [r] ranges [AjPList] AJAX List of Ensembl Mapper Ranges
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyMapperAdaptorRegisterChained(EnsPAssemblyMapperAdaptor ama,
                                               EnsPChainedAssemblyMapper cam,
                                               const AjPStr source,
                                               ajuint srcsrid,
                                               ajuint optsrid,
                                               AjPList ranges)
{
    ajint ori = 0;
    
    ajint srcsrstart = 0;
    ajint srcsrend   = 0;
    
    ajuint midsrid    = 0;
    ajint midsrstart  = 0;
    ajint midsrend    = 0;
    ajint midsrlength = 0;
    
    ajuint trgsrid    = 0;
    ajint trgsrstart  = 0;
    ajint trgsrend    = 0;
    ajint trgsrlength = 0;
    
    AjPList srcranges = NULL;
    AjPList midranges = NULL;
    
    AjIList liter = NULL;
    const AjPList mappath = NULL;
    
    AjISqlRow sqli       = NULL;
    AjPSqlStatement sqls = NULL;
    AjPSqlRow sqlr       = NULL;
     
    AjPStr srctype = NULL;
    AjPStr midtype = NULL;
    AjPStr trgtype = NULL;
    
    AjPStr midsrname = NULL;
    AjPStr trgsrname = NULL;
    
    AjPStr asm2cmp   = NULL;
    AjPStr cmp2asm   = NULL;
    AjPStr sqlfmt    = NULL;
    AjPStr statement = NULL;
    
    EnsPCoordSystem srccs = NULL;
    EnsPCoordSystem midcs = NULL;
    EnsPCoordSystem trgcs = NULL;
    
    EnsPCoordSystem asmcs = NULL;
    EnsPCoordSystem cmpcs = NULL;
    EnsPCoordSystem tstcs = NULL;
    
    EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPMapper srcmidmapper = NULL;
    EnsPMapper srctrgmapper = NULL;
    EnsPMapper trgmidmapper = NULL;
    
    EnsPMapperRange mr = NULL;
    EnsPMapperRangeRegistry srcregistry = NULL;
    EnsPMapperRangeRegistry trgregistry = NULL;
    
    EnsPMapperUnit mu = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegion optsr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorRegisterChained\n"
	     "  ama %p\n"
	     "  cam %p\n"
	     "  source '%S'\n"
	     "  srcsrid %u\n"
	     "  optsrid %u\n"
	     "  ranges %p\n",
	     ama,
	     cam,
	     source,
	     srcsrid,
	     optsrid,
	     ranges);
     */
    
    if(!ama)
	return ajFalse;
    
    if(!cam)
        return ajFalse;
    
    if(!(source && ajStrGetLen(source)))
        return ajFalse;
    
    if(!srcsrid)
        return ajFalse;
    
    if(!ranges)
        return ajFalse;
    
    if(optsrid)
    {
	if(ensCoordSystemMatch(
               ensChainedAssemblyMapperGetSourceCoordSystem(cam),
               ensChainedAssemblyMapperGetTargetCoordSystem(cam)))
	    return ensAssemblyMapperAdaptorRegisterChainedSpecial(ama,
								  cam,
								  source,
								  srcsrid,
								  optsrid,
								  ranges);
    }
	   
    csa = ensRegistryGetCoordSystemAdaptor(ama->Adaptor);
    
    sra = ensRegistryGetSeqRegionAdaptor(ama->Adaptor);
    
    if(ajStrMatchC(source, "source"))
    {
        srctype = ajStrNewC("source");
	
        srcmidmapper = cam->SourceMiddleMapper;
        
	srccs = cam->SourceCoordSystem;
        
	srcregistry = cam->SourceRegistry;
        
	trgtype = ajStrNewC("target");
        
	trgmidmapper = cam->TargetMiddleMapper;
        
	trgcs = cam->TargetCoordSystem;
        
	trgregistry = cam->TargetRegistry;
    }
    else if(ajStrMatchC(source, "target"))
    {
        srctype = ajStrNewC("target");
	
        srcmidmapper = cam->TargetMiddleMapper;
        
	srccs = cam->TargetCoordSystem;
        
	srcregistry = cam->TargetRegistry;
        
	trgtype = ajStrNewC("source");
        
	trgmidmapper = cam->SourceMiddleMapper;
        
	trgcs = cam->SourceCoordSystem;
        
	trgregistry = cam->SourceRegistry;
    }
    else
        ajFatal("ensAssemblyMapperAdaptorRegisterChained invalid 'source' "
                "argument '%S' must be 'source' or 'target'.",
		source);
    
    midtype = ajStrNewC("middle");
    
    srctrgmapper = cam->SourceTargetMapper;
    
    midcs = cam->MiddleCoordSystem;
    
    /*
    ** Obtain the first half of the mappings and load them into the
    ** source <-> middle Mapper. Check for the simple case, where the
    ** Chained Assembly Mapper is short and requires only a one-step
    ** source <-> target Mapper.
    */
    
    if(midcs)
        mappath = ensCoordSystemAdaptorGetMappingPath(csa, srccs, midcs);
    else
    {
	mappath = ensCoordSystemAdaptorGetMappingPath(csa, srccs, trgcs);
	
        srcmidmapper = srctrgmapper;
    }
    
    if (ajListGetLength(mappath) > 2)
    {
	/*
	** FIXME: Below the Perl version tests specifically for length == 2
	** or length == 3 and empty middle node. Here it tests only for
	** length != 2 and an empty middle node. What happens if the length
	** is actually zero or one???
	*/
	
	/*
	** Test the middle Coordinate System of multi-component mapping paths.
	** If the middle Coordinate System is NULL, we have a case where
	** multiple parts of a 'component' map to more than one 'assembled'
	** part. See ensCoordSystemAdaptorGetMappingPath for details.
	** If the middle Coordinate System is defined we have a
	** multi-component mapping path, which is not acceptable.
	*/
	
	ajListPeekNumber(mappath, 1, (void **) &tstcs);
	
	if(tstcs)
	    ajFatal("ensAssemblyMapperAdaptorRegisterChained "
		    "unexpected mapping between start and intermediate "
		    "Coordinate Systems '%S:%S' and '%S:%S'. "
		    "Expected path length 1, but got length %d.",
		    ensCoordSystemGetName(srccs),
		    ensCoordSystemGetVersion(srccs),
		    ensCoordSystemGetName(midcs),
		    ensCoordSystemGetVersion(midcs),
		    ajListGetLength(mappath) - 1);
	
	/* FIXME: The path report is missing from the above error report. */
    }
    
    ajListPeekFirst(mappath, (void **) &asmcs);
    
    ajListPeekLast(mappath, (void **) &cmpcs);
    
    asm2cmp = ajStrNewC("SELECT "
			"assembly.cmp_start, "
			"assembly.cmp_end, "
			"assembly.cmp_seq_region_id, "
			"seq_region.name, "
			"seq_region.length, "
			"assembly.ori, "
			"assembly.asm_start, "
			"assembly.asm_end "
			"FROM "
			"assembly, "
			"seq_region "
			"WHERE "
			"assembly.asm_seq_region_id = %u "
			"AND "
			"%d <= assembly.asm_end "
			"AND "
			"%d >= assembly.asm_start "
			"AND "
			"assembly.cmp_seq_region_id = "
			"seq_region.seq_region_id "
			"AND "
			"seq_region.coord_system_id = %u");
    
    cmp2asm = ajStrNewC("SELECT "
			"assembly.asm_start, "
			"assembly.asm_end, "
			"assembly.asm_seq_region_id, "
			"seq_region.name, "
			"seq_region.length, "
			"assembly.ori, "
			"assembly.cmp_start, "
			"assembly.cmp_end "
			"FROM "
			"assembly, "
			"seq_region "
			"WHERE "
			"assembly.cmp_seq_region_id = %u "
			"AND "
			"%d <= assembly.cmp_end "
			"AND "
			"%d >= assembly.cmp_start "
			"AND "
			"assembly.asm_seq_region_id = "
			"seq_region.seq_region_id "
			"AND "
			"seq_region.coord_system_id = %u");
    
    if(optsrid)
    {
	ensSeqRegionAdaptorFetchByIdentifier(sra, optsrid, &optsr);
	
	if(ensCoordSystemMatch(asmcs, ensSeqRegionGetCoordSystem(optsr)))
	{
	    ajFmtPrintAppS(&cmp2asm,
			   " AND asm.asm_seq_region_id = %u",
			   ensSeqRegionGetIdentifier(optsr));
	}
	else if (ensCoordSystemMatch(cmpcs, ensSeqRegionGetCoordSystem(optsr)))
	{
	    ajFmtPrintAppS(&asm2cmp,
			   " AND asm.cmp_seq_region_id = %u",
			   ensSeqRegionGetIdentifier(optsr));
	}
	else
	    ajWarn("ensAssemblyMapperAdaptorRegisterChained got an optional "
		   "Sequence Region, which is neither linked to the "
		   "source nor target Coordinate System.");
    }
    
    sqlfmt = (ensCoordSystemMatch(srccs, asmcs)) ? asm2cmp : cmp2asm;
    
    /* Run an SQL statement for each previously unregistered Mapper Range. */
    
    srcranges = ajListNew();
    
    midranges = ajListNew();
    
    liter = ajListIterNew(ranges);
    
    while(!ajListIterDone(liter))
    {
        mr = (EnsPMapperRange) ajListIterGet(liter);
	
	statement = ajFmtStr(ajStrGetPtr(sqlfmt),
			     srcsrid,
			     ensMapperRangeGetStart(mr),
			     ensMapperRangeGetEnd(mr),
			     (midcs) ?
			     ensCoordSystemGetIdentifier(midcs) :
			     ensCoordSystemGetIdentifier(trgcs));
	
	sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
	
	sqli = ajSqlRowIterNew(sqls);
	
	while(!ajSqlRowIterDone(sqli))
	{
	    midsrstart = 0;
	    midsrend = 0;
	    midsrid = 0;
	    midsrname = ajStrNew();
	    midsrlength = 0;
	    ori = 0;
	    srcsrstart = 0;
	    srcsrend = 0;
	    
	    sqlr = ajSqlRowIterGet(sqli);
	    
	    ajSqlColumnToInt(sqlr, &midsrstart);
	    ajSqlColumnToInt(sqlr, &midsrend);
	    ajSqlColumnToUint(sqlr, &midsrid);
	    ajSqlColumnToStr(sqlr, &midsrname);
	    ajSqlColumnToInt(sqlr, &midsrlength);
	    ajSqlColumnToInt(sqlr, &ori);
	    ajSqlColumnToInt(sqlr, &srcsrstart);
	    ajSqlColumnToInt(sqlr, &srcsrend);
	    
	    /*
	    ** Load the results into the source <-> middle Mapper or the
	    ** source <-> target Mapper in case the Chained Assembly Mapper
	    ** is short.
	    */
	    
	    if(midcs)
		ensMapperAddCoordinates(srcmidmapper,
					srcsrid,
					srcsrstart,
					srcsrend,
					ori,
					midsrid,
					midsrstart,
					midsrend);
	    else
	    {
		if(ajStrMatchC(source, "source"))
		    ensMapperAddCoordinates(srctrgmapper,
					    srcsrid,
					    srcsrstart,
					    srcsrend,
					    ori,
					    midsrid,
					    midsrstart,
					    midsrend);
		else
		    ensMapperAddCoordinates(srctrgmapper,
					    midsrid,
					    midsrstart,
					    midsrend,
					    ori,
					    srcsrid,
					    srcsrstart,
					    srcsrend);
	    }
	    
	    /* Record Mapper Units for the source part. */
	    
	    mu = ensMapperUnitNew(srcsrid, srcsrstart, srcsrend);
	    
	    ajListPushAppend(srcranges, (void *) mu);
	    
	    /* Record Mapper Units for the middle part. */
	    
	    mu = ensMapperUnitNew(midsrid, midsrstart, midsrend);
	    
	    ajListPushAppend(midranges, (void *) mu);
	    
	    /*
	    ** Add the (new) middle Ensembl Sequence Region into the
	    ** Ensembl Sequence Region Adaptor cache.
	    */
	    
	    sr = ensSeqRegionNew(sra,
				 midsrid,
				 (midcs) ? midcs : trgcs,
				 midsrname,
				 midsrlength);
	    
	    ensSeqRegionAdaptorCacheInsert(sra, &sr);
	    
	    ensSeqRegionDel(&sr);
	    
	    /*
	    ** Register the source Mapper Units in the source Mapper Registry.
	    ** The region we actually register may be smaller or larger than
	    ** the region we originally intended to register.
	    ** Register the intersection of the region so we do not end up
	    ** doing extra work later.
	    */
	    
	    if((srcsrstart < ensMapperRangeGetStart(mr)) ||
	       (srcsrend > ensMapperRangeGetEnd(mr)))
                ensMapperRangeRegistryCheckAndRegister(srcregistry,
						       srcsrid,
						       srcsrstart,
						       srcsrend,
						       srcsrstart,
						       srcsrend,
						       (AjPList) NULL);
	    
	    ajStrDel(&midsrname);
	}
	
	ajSqlRowIterDel(&sqli);
	
	ajSqlStatementDel(&sqls);
	
	ajStrDel(&statement);
    }
    
    ajListIterDel(&liter);
    
    /*
     ** In the one-step case, the middle Mapper Units are simply registered
     ** in the target Mapper Range Registry.
     */
    
    if(!midcs)
    {
	/* The source Mapper Units are no longer needed in this case. */
	
	while(ajListPop(srcranges, (void **) &mu))
	    ensMapperUnitDel(&mu);
	
	ajListFree(&srcranges);
	
	/*
	 ** Register the middle Mapper Units in the
	 ** target Mapper Range Registry
	 ** and that's it for the simple case ...
	 */
	
	while(ajListPop(midranges, (void **) &mu))
	{
	    ensMapperRangeRegistryCheckAndRegister(
                trgregistry,
                ensMapperUnitGetObjectIdentifier(mu),
                ensMapperUnitGetStart(mu),
                ensMapperUnitGetEnd(mu),
                ensMapperUnitGetStart(mu),
                ensMapperUnitGetEnd(mu),
                (AjPList) NULL);
	    
	    ensMapperUnitDel(&mu);
	}
	
	ajListFree(&midranges);
	
	ajStrDel(&asm2cmp);
	
	ajStrDel(&cmp2asm);
	
	ajStrDel(&srctype);
	
	ajStrDel(&midtype);
	
	ajStrDel(&trgtype);
	
	return ajTrue;
    }
    
    /*
    ** For the second half of the mapping an SQL statement for each middle
    ** Mapper Unit is performed. The results are loaded into the
    ** middle <-> target Mapper.
    ** But first, ascertain which is the component and which is actually the
    ** assembled Coordinate System.
    */
    
    mappath = ensCoordSystemAdaptorGetMappingPath(csa, midcs, trgcs);
    
    /*
    ** Check, if the mapping path is something like
    ** supercontig#contig#chromosome.
    ** Only simple, two component mapping paths or three component mapping
    ** paths with an empty middle Coordinate System, which indicate mappings
    ** from multiple 'component' regions to the same 'assembled' region,
    ** are acceptable.
    */
    
    /*
    ** FIXME: In contrast to the code above this would exclude mapping paths
    ** with only one element. Sould the same apply to the code above?
    */
    
    if(ajListGetLength(mappath) != 2)
    {
	/*
	** Test the middle node of multi-component mapping paths.
	** If the middle node is empty, we have a case where multiple parts
	** of a 'component' map to more than one 'assembled' part.
	** See ensCoordSystemAdaptorGetMappingPath for details.
	** If the middle node is defined we have a multi-component mapping
	** path, which is not acceptable.
	*/
	
	if(ajListGetLength(mappath) > 2)
	    ajListPeekNumber(mappath, 1, (void **) &tstcs);
	
	if((ajListGetLength(mappath) < 2) || tstcs)
	    ajFatal("ensAssemblyMapperAdaptorRegisterChained "
		    "unexpected mapping between intermediate and target "
		    "Coordinate Systems '%S:%S' and '%S:%S'. "
		    "Expected path length 1, but got length %d.",
		    ensCoordSystemGetName(midcs),
		    ensCoordSystemGetVersion(midcs),
		    ensCoordSystemGetName(trgcs),
		    ensCoordSystemGetVersion(trgcs),
		    ajListGetLength(mappath) - 1);
	
	/* FIXME: The path report is missing from the above error report. */
    }
    
    ajListPeekFirst(mappath, (void **) &asmcs);
    
    ajListPeekLast(mappath, (void **) &cmpcs);
    
    sqlfmt = (ensCoordSystemMatch(midcs, asmcs)) ? asm2cmp : cmp2asm;
    
    while(ajListPop(midranges, (void **) &mu))
    {
	statement = ajFmtStr(ajStrGetPtr(sqlfmt),
			     ensMapperUnitGetObjectIdentifier(mu),
			     ensMapperUnitGetStart(mu),
			     ensMapperUnitGetEnd(mu),
			     ensCoordSystemGetIdentifier(trgcs));
	
	sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
	
	sqli = ajSqlRowIterNew(sqls);
	
	while(!ajSqlRowIterDone(sqli))
	{
	    trgsrstart = 0;
	    trgsrend = 0;
	    trgsrid = 0;
	    trgsrname = ajStrNew();
	    trgsrlength = 0;
	    ori = 0;
	    midsrstart = 0;
	    midsrend = 0;
	    
	    sqlr = ajSqlRowIterGet(sqli);
	    
	    ajSqlColumnToInt(sqlr, &trgsrstart);
	    ajSqlColumnToInt(sqlr, &trgsrend);
	    ajSqlColumnToUint(sqlr, &trgsrid);
	    ajSqlColumnToStr(sqlr, &trgsrname);
	    ajSqlColumnToInt(sqlr, &trgsrlength);
	    ajSqlColumnToInt(sqlr, &ori);
	    ajSqlColumnToInt(sqlr, &midsrstart);
	    ajSqlColumnToInt(sqlr, &midsrend);
	    
	    /* Load the results into the target <-> middle Mapper. */
	    
	    ensMapperAddCoordinates(trgmidmapper,
				    trgsrid,
				    trgsrstart,
				    trgsrend,
				    ori,
				    ensMapperUnitGetObjectIdentifier(mu),
				    midsrstart,
				    midsrend);
	    
	    /*
	     ** Add the (new) target Ensembl Sequence Region into the
	     ** Ensembl Sequence Region Adaptor cache.
	     */
	    
	    sr = ensSeqRegionNew(sra,
				 trgsrid,
				 trgcs,
				 trgsrname,
				 trgsrlength);
	    
	    ensSeqRegionAdaptorCacheInsert(sra, &sr);
	    
	    ensSeqRegionDel(&sr);
	    
	    /*
	     ** Register the target Mapper Unit in the
	     ** target Mapper Range Registry.
	     */
	    
	    ensMapperRangeRegistryCheckAndRegister(trgregistry,
						   trgsrid,
						   trgsrstart,
						   trgsrend,
						   trgsrstart,
						   trgsrend,
						   (AjPList) NULL);
	    
	    ajStrDel(&trgsrname);
	}
	
	ajSqlRowIterDel(&sqli);
	
	ajSqlStatementDel(&sqls);
	
	ajStrDel(&statement);
	
	ensMapperUnitDel(&mu);
    }
    
    ajListFree(&midranges);
    
    /*
    ** Now that both halves are loaded do stepwise mapping using both of
    ** the loaded Mappers to load the final source <-> target Mapper.
    */
    
    assemblyMapperAdaptorBuildCombinedMapper(ama,
					     srcranges,
					     srcmidmapper,
					     trgmidmapper,
					     srctrgmapper,
					     srctype);
    
    while (ajListPop(srcranges, (void **) &mu))
	ensMapperUnitDel(&mu);
    
    ajListFree(&srcranges);
    
    ajStrDel(&srctype);
    
    ajStrDel(&midtype);
    
    ajStrDel(&trgtype);
    
    ajStrDel(&asm2cmp);
    
    ajStrDel(&cmp2asm);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorRegisterChainedSpecial ***********************
**
** Registers an AJAX List of previously unregistered Ensembl Mapper Ranges in
** an Ensembl Chained Assembly Mapper.
**
** Optionally, only those Ensembl Mapper Ranges can be registered, which map
** onto a particular Ensembl Sequence Region, if its identifier was specified.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::register_chained_special
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
** @param [r] source [AjPStr] Mapping type for the source
** @param [r] srcsrid [ajuint] Ensembl Sequence Region identifier
** @param [rE] optsrid [ajuint] Optional Ensembl Sequence Region identifier
** @param [r] ranges [AjPList] AJAX List of Ensembl Mapper Ranges
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyMapperAdaptorRegisterChainedSpecial(
    EnsPAssemblyMapperAdaptor ama,
    EnsPChainedAssemblyMapper cam,
    const AjPStr source,
    ajuint srcsrid,
    ajuint optsrid,
    AjPList ranges)
{
    ajint i   = 0;
    ajint ori = 0;
    
    ajint srcsrstart = 0;
    ajint srcsrend   = 0;
    
    ajuint midsrid    = 0;
    ajint midsrstart  = 0;
    ajint midsrend    = 0;
    ajint midsrlength = 0;
    
    /*
     ** FIXME: Remove this!
     ajuint trgsrid = 0;
     ajint trgsrstart = 0;
     ajint trgsrend = 0;
     ajint trgsrlength = 0;
     */
    
    AjBool found = AJFALSE;
    
    AjPList srcranges = NULL;
    AjPList midranges = NULL;
    
    AjIList liter = NULL;
    const AjPList mappath = NULL;
    
    AjISqlRow sqli       = NULL;
    AjPSqlStatement sqls = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr srctype = NULL;
    AjPStr midtype = NULL;
    AjPStr trgtype = NULL;
    
    AjPStr midsrname = NULL;
    
    /*
    ** FIXME: Remove this!
    ** AjPStr trgsrname = NULL;
    */
    
    AjPStr asm2cmp = NULL;
    AjPStr sqlfmt = NULL;
    AjPStr statement = NULL;
    
    EnsPCoordSystem srccs = NULL;
    EnsPCoordSystem midcs = NULL;
    EnsPCoordSystem trgcs = NULL;
    
    EnsPCoordSystem asmcs = NULL;
    EnsPCoordSystem cmpcs = NULL;
    EnsPCoordSystem tstcs = NULL;
    
    EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPMapper srcmidmapper = NULL;
    EnsPMapper srctrgmapper = NULL;
    EnsPMapper trgmidmapper = NULL;
    
    EnsPMapperRange mr = NULL;
    EnsPMapperRangeRegistry srcregistry = NULL;
    EnsPMapperRangeRegistry trgregistry = NULL;
    
    EnsPMapperUnit mu = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegion optsr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorRegisterChainedSpecial\n"
	     "  ama %p\n"
	     "  cam %p\n"
	     "  source '%S'\n"
	     "  srcsrid %u\n"
	     "  optsrid %u\n"
	     "  ranges %p\n",
	     ama,
	     cam,
	     source,
	     srcsrid,
	     optsrid,
	     ranges);
     */
    
    if(!ama)
	return ajFalse;
    
    if(!cam)
        return ajFalse;
    
    if((!source) && (!ajStrGetLen(source)))
        return ajFalse;
    
    if(!srcsrid)
        return ajFalse;
    
    if(!ranges)
        return ajFalse;
    
    csa = ensRegistryGetCoordSystemAdaptor(ama->Adaptor);
    
    sra = ensRegistryGetSeqRegionAdaptor(ama->Adaptor);
    
    if(ajStrMatchC(source, "source"))
    {
        srctype = ajStrNewC("source");
	
        srcmidmapper = cam->SourceMiddleMapper;
        
	srccs = cam->SourceCoordSystem;
        
	srcregistry = cam->SourceRegistry;
        
	trgtype = ajStrNewC("target");
        
	trgmidmapper = cam->TargetMiddleMapper;
        
	trgcs = cam->TargetCoordSystem;
        
	trgregistry = cam->TargetRegistry;
    }
    
    else if (ajStrMatchC(source, "target"))
    {
        srctype = ajStrNewC("target");
	
        srcmidmapper = cam->TargetMiddleMapper;
        
	srccs = cam->TargetCoordSystem;
        
	srcregistry = cam->TargetRegistry;
        
	trgtype = ajStrNewC("source");
        
	trgmidmapper = cam->SourceMiddleMapper;
        
	trgcs = cam->SourceCoordSystem;
        
	trgregistry = cam->SourceRegistry;
    }
    
    else
        ajFatal("ensAssemblyMapperAdaptorRegisterChained invalid 'source' "
                "argument '%S' must be 'source' or 'target'.",
		source);
    
    midtype = ajStrNewC("middle");
    
    srctrgmapper = cam->SourceTargetMapper;
    
    midcs = cam->MiddleCoordSystem;
    
    /*
    ** Obtain the first half of the mappings and load them into the
    ** source <-> middle Mapper. Check for the simple case, where the
    ** Chained Assembly Mapper is short and requires only a one-step
    ** source <-> target Mapper.
    */
    
    /*
    ** FIXME: This block appears twice in the Perl API.
    ** my $combined_mapper = $casm_mapper->first_last_mapper();
    ** my $mid_cs          = $casm_mapper->middle_CoordSystem();
    ** my $mid_name        = 'middle';
    ** my $csa             = $self->db->get_CoordSystemAdaptor();
    */
    
    if(midcs)
        mappath = ensCoordSystemAdaptorGetMappingPath(csa, srccs, midcs);
    else
    {
	mappath = ensCoordSystemAdaptorGetMappingPath(csa, srccs, trgcs);
	
        srcmidmapper = srctrgmapper;
    }
    
    if(ajListGetLength(mappath) > 2)
    {
	/*
	** FIXME: Below the Perl version tests specifically for length == 2
	** or length == 3 and empty middle node. Here it tests only for
	** length != 2 and an empty middle node. What happens if the length
	** is actually zero or one???
	*/
	
	/*
	** Test the middle Coordinate System of multi-component mapping paths.
	** If the middle Coordinate System is NULL, we have a case where
	** multiple parts of a 'component' map to more than one 'assembled'
	** part. See ensCoordSystemAdaptorGetMappingPath for details.
	** If the middle Coordinate System is defined we have a
	** multi-component mapping path, which is not acceptable.
	*/
	
	ajListPeekNumber(mappath, 1, (void **) &tstcs);
	
	if(tstcs)
	    ajFatal("ensAssemblyMapperAdaptorRegisterChained "
		    "unexpected mapping between start and intermediate "
		    "Coordinate Systems '%S:%S' and '%S:%S'. "
		    "Expected path length 1, but got length %d.",
		    ensCoordSystemGetName(srccs),
		    ensCoordSystemGetVersion(srccs),
		    ensCoordSystemGetName(midcs),
		    ensCoordSystemGetVersion(midcs),
		    ajListGetLength(mappath) - 1);
	
	/* FIXME: The path report is missing from the above error report. */
    }
    
    ajListPeekFirst(mappath, (void **) &asmcs);
    
    ajListPeekLast(mappath, (void **) &cmpcs);
    
    asm2cmp = ajStrNewC("SELECT "
			"assembly.cmp_start, "
			"assembly.cmp_end, "
			"assembly.cmp_seq_region_id, "
			"seq_region.name, "
			"seq_region.length, "
			"assembly.ori, "
			"assembly.asm_start, "
			"assembly.asm_end "
			"FROM "
			"assembly, "
			"seq_region "
			"WHERE "
			"assembly.asm_seq_region_id = %u "
			"AND "
			"%d <= assembly.asm_end "
			"AND "
			"%d >= assembly.asm_start "
			"AND "
			"assembly.cmp_seq_region_id = "
			"seq_region.seq_region_id "
			"AND "
			"seq_region.coord_system_id = %u "
			"AND "
			"assembly.cmp_seq_region_id = %u");
    
    if(optsrid)
	ensSeqRegionAdaptorFetchByIdentifier(sra, optsrid, &optsr);
    
    /*
     ** FIXME: Remove this!
     if (slice)
     {
	 if (ensCoordSystemMatch(asmcs, ensSliceGetCoordSystem(slice)))
	 {
	     ajFmtPrintAppS(&cmp2asm,
			    " AND asm.asm_seq_region_id = %u",
			    ensSliceGetSeqRegionIdentifier(slice));
	 }
	 
	 else if (ensCoordSystemMatch(cmpcs, ensSliceGetCoordSystem(slice)))
	 {
	     ajFmtPrintAppS(&asm2cmp,
			    " AND asm.cmp_seq_region_id = %u",
			    ensSliceGetSeqRegionIdentifier(slice));
	 }
	 
	 else
	     ajWarn("ensAssemblyMapperAdaptorRegisterChained got a Slice, "
		    "which is neither in the source nor target "
		    "Coordinate System.");
     }
     
     sqlfmt = (ensCoordSystemMatch(srccs, asmcs)) ? asm2cmp : cmp2asm;
     */
    
    sqlfmt = asm2cmp;
    
    /* Run an SQL statement for each previously unregistered Mapper Range. */
    
    srcranges = ajListNew();
    
    midranges = ajListNew();
    
    for(i = 0; i <= 1; i++)
    {
	
	liter = ajListIterNew(ranges);
	
	while(!ajListIterDone(liter))
	{
	    mr = (EnsPMapperRange) ajListIterGet(liter);
	    
	    statement = ajFmtStr(ajStrGetPtr(sqlfmt),
				 (i) ? optsrid : srcsrid,
				 ensMapperRangeGetStart(mr),
				 ensMapperRangeGetEnd(mr),
				 ensCoordSystemGetIdentifier(
                                     ensSeqRegionGetCoordSystem(optsr)),
				 (i) ? srcsrid : optsrid);
	    
	    sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
	    
	    sqli = ajSqlRowIterNew(sqls);
	    
	    while(!ajSqlRowIterDone(sqli))
	    {
		found = ajTrue;
		
		midsrstart = 0;
		midsrend = 0;
		midsrid = 0;
		midsrname = ajStrNew();
		midsrlength = 0;
		ori = 0;
		srcsrstart = 0;
		srcsrend = 0;
		
		sqlr = ajSqlRowIterGet(sqli);
		
		ajSqlColumnToInt(sqlr, &midsrstart);
		ajSqlColumnToInt(sqlr, &midsrend);
		ajSqlColumnToUint(sqlr, &midsrid);
		ajSqlColumnToStr(sqlr, &midsrname);
		ajSqlColumnToInt(sqlr, &midsrlength);
		ajSqlColumnToInt(sqlr, &ori);
		ajSqlColumnToInt(sqlr, &srcsrstart);
		ajSqlColumnToInt(sqlr, &srcsrend);
		
		/*
		** Load the results into the source <-> middle Mapper or the
		** source <-> target Mapper in case the
		** Chained Assembly Mapper is short.
		*/
		
		if(midcs)
		    ensMapperAddCoordinates(srcmidmapper,
					    (i) ? optsrid : srcsrid,
					    srcsrstart,
					    srcsrend,
					    ori,
					    midsrid,
					    midsrstart,
					    midsrend);
		else
		{
		    if(ajStrMatchC(source, "source"))
		    {
			if(i)
			    ensMapperAddCoordinates(srctrgmapper,
						    (i) ? optsrid : srcsrid,
						    srcsrstart,
						    srcsrend,
						    ori,
						    midsrid,
						    midsrstart,
						    midsrend);
			else
			    ensMapperAddCoordinates(srctrgmapper,
						    midsrid,
						    midsrstart,
						    midsrend,
						    ori,
						    (i) ? optsrid : srcsrid,
						    srcsrstart,
						    srcsrend);
		    }
		    else
		    {
			if(i)
			    ensMapperAddCoordinates(srctrgmapper,
						    midsrid,
						    midsrstart,
						    midsrend,
						    ori,
						    (i) ? optsrid : srcsrid,
						    srcsrstart,
						    srcsrend);
			else
			    ensMapperAddCoordinates(srctrgmapper,
						    (i) ? optsrid : srcsrid,
						    srcsrstart,
						    srcsrend,
						    ori,
						    midsrid,
						    midsrstart,
						    midsrend);
		    }
		}
		
		/* Record Mapper Units for the source part. */
		
		mu = ensMapperUnitNew((i) ? optsrid : srcsrid,
				      srcsrstart,
				      srcsrend);
		
		ajListPushAppend(srcranges, (void *) mu);
		
		/* Record Mapper Units for the middle part. */
		
		mu = ensMapperUnitNew(midsrid, midsrstart, midsrend);
		
		ajListPushAppend(midranges, (void *) mu);
		
		/*
		** Add the (new) middle Ensembl Sequence Region into the
		** Ensembl Sequence Region Adaptor cache.
		*/
		
		/*
                ** FIXME: The Perl API uses a $mid_sr_id here, but never
                ** assigns it a value!!!
                */
		
		sr = ensSeqRegionNew(sra,
				     midsrid,
				     (midcs) ? midcs : trgcs,
				     midsrname,
				     midsrlength);
		
		ensSeqRegionAdaptorCacheInsert(sra, &sr);
		
		ensSeqRegionDel(&sr);
		
		/*
		** Register the source Mapper Units in the source
		** Mapper Registry. The region we actually register may be
		** smaller or larger than the region we originally intended
		** to register. Register the intersection of the region so we
		** do not end up doing extra work later.
		*/
		
		if((srcsrstart < ensMapperRangeGetStart(mr)) ||
		    (srcsrend > ensMapperRangeGetEnd(mr)))
		    ensMapperRangeRegistryCheckAndRegister(
                        srcregistry,
                        (i) ? optsrid : srcsrid,
                        srcsrstart,
                        srcsrend,
                        srcsrstart,
                        srcsrend,
                        (AjPList) NULL);
		
		ajStrDel(&midsrname);
	    }
	    
	    ajSqlRowIterDel(&sqli);
	    
	    ajSqlStatementDel(&sqls);
	    
	    ajStrDel(&statement);
	}
	
	ajListIterDel(&liter);
    }
    
    /*
    ** In the one-step case, the middle Mapper Units are simply registered
    ** in the target Mapper Range Registry.
    */
    
    if(found && (! midcs))
    {
	/* The source Mapper Units are no longer needed in this case. */
	
	while(ajListPop(srcranges, (void **) &mu))
	    ensMapperUnitDel(&mu);
	
	ajListFree(&srcranges);
	
	/*
	 ** Register the middle Mapper Units in the
	 ** target Mapper Range Registry
	 ** and that's it for the simple case ...
	 */
	
	while(ajListPop(midranges, (void **) &mu))
	{
	    ensMapperRangeRegistryCheckAndRegister(
                trgregistry,
                ensMapperUnitGetObjectIdentifier(mu),
                ensMapperUnitGetStart(mu),
                ensMapperUnitGetEnd(mu),
                ensMapperUnitGetStart(mu),
                ensMapperUnitGetEnd(mu),
                (AjPList) NULL);
	    
	    ensMapperUnitDel(&mu);
	}
	
	ajListFree(&midranges);
	
	ajStrDel(&asm2cmp);
	
	ajStrDel(&srctype);
	
	ajStrDel(&midtype);
	
	ajStrDel(&trgtype);
	
	return ajTrue;
    }
    
    while(ajListPop(srcranges, (void **) &mu))
	ensMapperUnitDel(&mu);
    
    ajListFree(&srcranges);
    
    while(ajListPop(midranges, (void **) &mu))
	ensMapperUnitDel(&mu);
    
    ajListFree(&midranges);
    
    ajStrDel(&srctype);
    
    ajStrDel(&midtype);
    
    ajStrDel(&trgtype);
    
    ajStrDel(&asm2cmp);
    
    return ajTrue;
}




/* @func ensAssemblyMapperAdaptorRegisterAllChained ***************************
**
** Registers all Ensembl Mapper Ranges in an
** Ensembl Chained Assembly Mapper.
**
** @cc Bio::EnsEMBL::DBSQL::AssemblyMapperAdaptor::register_all_chained
** @param [r] ama [EnsPAssemblyMapperAdaptor] Ensembl Assembly Mapper Adaptor
** @param [u] cam [EnsPChainedAssemblyMapper] Ensembl Chained Assembly Mapper
**
** @return [AjBool] ajTrue on success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: The Perl API uses the splice function once to remove empty middle
** nodes and a complicated construct once to achieve the same.
*/

AjBool ensAssemblyMapperAdaptorRegisterAllChained(
    EnsPAssemblyMapperAdaptor ama,
    EnsPChainedAssemblyMapper cam)
{
    ajint ori = 0;
    
    ajuint srcsrid    = 0;
    ajint srcsrstart  = 0;
    ajint srcsrend    = 0;
    ajint srcsrlength = 0;
    
    ajuint midsrid    = 0;
    ajint midsrstart  = 0;
    ajint midsrend    = 0;
    ajint midsrlength = 0;
    
    ajuint trgsrid    = 0;
    ajint trgsrstart  = 0;
    ajint trgsrend    = 0;
    ajint trgsrlength = 0;
    
    const AjPList mappath = NULL;
    AjPList ranges = NULL;
    
    AjISqlRow sqli = NULL;
    AjPSqlStatement sqls = NULL;
    AjPSqlRow sqlr = NULL;
    
    AjPStr srcsrname = NULL;
    AjPStr midsrname = NULL;
    AjPStr trgsrname = NULL;
    
    AjPStr sqlfmt    = NULL;
    AjPStr statement = NULL;
    AjPStr srctype   = NULL;
    
    EnsPCoordSystem srccs = NULL;
    EnsPCoordSystem midcs = NULL;
    EnsPCoordSystem trgcs = NULL;
    
    EnsPCoordSystem asmcs = NULL;
    EnsPCoordSystem cmpcs = NULL;
    EnsPCoordSystem tstcs = NULL;
    
    EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPMapper srcmidmapper = NULL;
    EnsPMapper trgmidmapper = NULL;
    EnsPMapper srctrgmapper = NULL;
    EnsPMapper mapper = NULL;
    EnsPMapperUnit mu = NULL;
    
    EnsPSeqRegion sr = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    /*
     ajDebug("ensAssemblyMapperAdaptorRegisterAllChained\n"
	     "  ama %p\n"
	     "  cam %p\n",
	     ama,
	     cam);
     */
    
    if(!ama)
	return ajFalse;
    
    if(!cam)
	return ajFalse;
    
    srccs = cam->SourceCoordSystem;
    
    midcs = cam->MiddleCoordSystem;
    
    trgcs = cam->TargetCoordSystem;
    
    srcmidmapper = cam->SourceMiddleMapper;
    
    trgmidmapper = cam->TargetMiddleMapper;
    
    srctrgmapper = cam->SourceTargetMapper;
    
    csa = ensRegistryGetCoordSystemAdaptor(ama->Adaptor);
    
    sra = ensRegistryGetSeqRegionAdaptor(ama->Adaptor);
    
    sqlfmt = ajStrNewC("SELECT "
		       "assembly.cmp_start, "
		       "assembly.cmp_end, "
		       "assembly.cmp_seq_region_id, "
		       "sr_cmp.name, "
		       "sr_cmp.length, "
		       "assembly.ori, "
		       "assembly.asm_start, "
		       "assembly.asm_end, "
		       "assembly.asm_seq_region_id, "
		       "sr_asm.name, "
		       "sr_asm.length "
		       "FROM "
		       "assembly, seq_region sr_asm, seq_region sr_cmp "
		       "WHERE "
		       "sr_asm.seq_region_id = assembly.asm_seq_region_id "
		       "AND "
		       "sr_cmp.seq_region_id = assembly.cmp_seq_region_id "
		       "AND "
		       "sr_asm.coord_system_id = %u "
		       "AND "
		       "sr_cmp.coord_system_id = %u");
    
    if(midcs)
    {
	mappath = ensCoordSystemAdaptorGetMappingPath(csa, srccs, midcs);
	
	mapper = srcmidmapper;
    }
    else
    {
	mappath = ensCoordSystemAdaptorGetMappingPath(csa, srccs, trgcs);
	
	mapper = srctrgmapper;
    }
    
    /*
    ** Check if we have something like supercontig#contig#chromosome.
    ** Only simple, two-component mapping paths or three-component mapping
    ** paths with empty middle nodes, which indicate mappings from multiple
    ** 'component' regions to the same 'assembled' region, are acceptable.
    */
    
    if(ajListGetLength(mappath) != 2)
    {
	/*
	** Test the middle node of multi-component mapping paths.
	** If the middle node is empty, we have a case where multiple parts of
	** a 'component' map to more than one 'assembled' part.
	** See ensCoordSystemAdaptorGetMappingPath for details.
	** If the middle node is defined we have a multi-component mapping
	** path, which is not acceptable.
	*/
	
	if(ajListGetLength(mappath) > 2)
            ajListPeekNumber(mappath, 1, (void **) &tstcs);
	
	if((ajListGetLength(mappath) < 2) || tstcs)
	    ajFatal("ensAssemblyMapperAdaptorRegisterAllChained "
		    "unexpected mapping between source and intermediate "
		    "Coordinate Systems '%S:%S' and '%S:%S'. "
		    "Expected path length 1, but got length %d.",
		    ensCoordSystemGetName(srccs),
		    ensCoordSystemGetVersion(srccs),
		    ensCoordSystemGetName(midcs),
		    ensCoordSystemGetVersion(midcs),
		    ajListGetLength(mappath) - 1);
	
	/* FIXME: The path report is missing from the above error report. */
    }
    
    ranges = ajListNew();
    
    ajListPeekFirst(mappath, (void **) &asmcs);
    
    ajListPeekLast(mappath, (void **) &cmpcs);
    
    statement = ajFmtStr(ajStrGetPtr(sqlfmt),
			 ensCoordSystemGetIdentifier(asmcs),
			 ensCoordSystemGetIdentifier(cmpcs));
    
    sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	srcsrstart = 0;
	srcsrend = 0;
	srcsrid = 0;
	srcsrname = ajStrNew();
	srcsrlength = 0;
	ori = 0;
	midsrstart = 0;
	midsrend = 0;
	midsrid = 0;
	midsrname = ajStrNew();
	midsrlength = 0;
	
	sqlr = ajSqlRowIterGet(sqli);
	
	if(ensCoordSystemMatch(srccs, asmcs))
	{
	    ajSqlColumnToInt(sqlr, &midsrstart);
	    ajSqlColumnToInt(sqlr, &midsrend);
	    ajSqlColumnToUint(sqlr, &midsrid);
	    ajSqlColumnToStr(sqlr, &midsrname);
	    ajSqlColumnToInt(sqlr, &midsrlength);
	    ajSqlColumnToInt(sqlr, &ori);
	    ajSqlColumnToInt(sqlr, &srcsrstart);
	    ajSqlColumnToInt(sqlr, &srcsrend);
	    ajSqlColumnToUint(sqlr, &srcsrid);
	    ajSqlColumnToStr(sqlr, &srcsrname);
	    ajSqlColumnToInt(sqlr, &srcsrlength);
	}
	else
	{
	    ajSqlColumnToInt(sqlr, &srcsrstart);
	    ajSqlColumnToInt(sqlr, &srcsrend);
	    ajSqlColumnToUint(sqlr, &srcsrid);
	    ajSqlColumnToStr(sqlr, &srcsrname);
	    ajSqlColumnToInt(sqlr, &srcsrlength);
	    ajSqlColumnToInt(sqlr, &ori);
	    ajSqlColumnToInt(sqlr, &midsrstart);
	    ajSqlColumnToInt(sqlr, &midsrend);
	    ajSqlColumnToUint(sqlr, &midsrid);
	    ajSqlColumnToStr(sqlr, &midsrname);
	    ajSqlColumnToInt(sqlr, &midsrlength);
	}
	
	/*
	** Load the results into the source <-> middle Mapper or the
	** source <-> target Mapper in case the Chained Assembly Mapper
	** is short.
	*/
	
	ensMapperAddCoordinates(mapper,
				srcsrid,
				srcsrstart,
				srcsrend,
				ori,
				midsrid,
				midsrstart,
				midsrend);
	
	/* Record Mapper Units for the source part. */
	
	mu = ensMapperUnitNew(srcsrid, srcsrstart, srcsrend);
	
	ajListPushAppend(ranges, (void *) mu);
	
	/* Register the source Mapper Units in the source Mapper Registry. */
	
	ensMapperRangeRegistryCheckAndRegister(cam->SourceRegistry,
					       srcsrid,
					       1,
					       srcsrlength,
					       1,
					       srcsrlength,
					       (AjPList) NULL);
	
	/*
	** If the Chained Assembly Mapper is short, immediately register the
	** middle Mapper Units in the target Mapper Registry.
	*/
	
	if(!midcs)
	    ensMapperRangeRegistryCheckAndRegister(cam->TargetRegistry,
						   midsrid,
						   midsrstart,
						   midsrend,
						   midsrstart,
						   midsrend,
						   (AjPList) NULL);
	
	/*
	** Add the (new) source Ensembl Sequence Region into the
	** Ensembl Sequence Region Adaptor cache.
	*/
	
	sr = ensSeqRegionNew(sra, srcsrid, srccs, srcsrname, srcsrlength);
	
	ensSeqRegionAdaptorCacheInsert(sra, &sr);
	
	ensSeqRegionDel(&sr);
	
	/*
	** Add the (new) middle Ensembl Sequence Region into the
	** Ensembl Sequence Region Adaptor cache.
	*/
	
	sr = ensSeqRegionNew(sra,
			     midsrid,
			     (midcs) ? midcs : trgcs,
			     midsrname,
			     midsrlength);
	
	ensSeqRegionAdaptorCacheInsert(sra, &sr);
	
	ensSeqRegionDel(&sr);
	
	ajStrDel(&srcsrname);
	
	ajStrDel(&trgsrname);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    if(!midcs)
    {
	/* That is all for the simple case. */
	
	while(ajListPop(ranges, (void **) &mu))
	    ensMapperUnitDel(&mu);
	
	ajListFree(&ranges);
	
	ajStrDel(&sqlfmt);
	
	return ajTrue;
    }
    
    mappath = ensCoordSystemAdaptorGetMappingPath(csa, trgcs, midcs);
    
    /*
    ** Check if we have something like supercontig#contig#chromosome.
    ** Only simple, two component mapping paths or three component mapping
    ** paths with empty middle nodes, which indicate mappings from multiple
    ** 'component' regions to the same 'assembled' region, are acceptable.
    */
    
    if(midcs && (ajListGetLength(mappath) != 2))
    {
	/*
	** Test the middle node of multi-component mapping paths.
	** If the middle node is empty, we have a case where multiple parts of
	** a 'component' map to more than one 'assembled' part.
	** See ensCoordSystemAdaptorGetMappingPath for details.
	** If the middle node is defined we have a multi-component mapping
	** path, which is not acceptable.
	*/
	
	if(ajListGetLength(mappath) > 2)
	    ajListPeekNumber(mappath, 1, (void **) &tstcs);
	
	if((ajListGetLength(mappath) < 2) || tstcs)
	    ajFatal("ensAssemblyMapperAdaptorRegisterAllChained "
		    "unexpected mapping between target and intermediate "
		    "Coordinate Systems '%S:%S' and '%S:%S'. "
		    "Expected path length 1, but got length %d.",
		    ensCoordSystemGetName(trgcs),
		    ensCoordSystemGetVersion(trgcs),
		    ensCoordSystemGetName(midcs),
		    ensCoordSystemGetVersion(midcs),
		    ajListGetLength(mappath) - 1);
	
	/*
	** FIXME: The Perl API error message states start and intermediate
	** here, but it should be last to intermediate.
	*/
    }
    
    ajListPeekFirst(mappath, (void **) &asmcs);
    
    ajListPeekLast(mappath, (void **) &cmpcs);
    
    statement = ajFmtStr(ajStrGetPtr(sqlfmt),
			 ensCoordSystemGetIdentifier(asmcs),
			 ensCoordSystemGetIdentifier(cmpcs));
    
    sqls = ensDatabaseAdaptorSqlStatementNew(ama->Adaptor, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	midsrstart = 0;
	midsrend = 0;
	midsrid = 0;
	midsrname = ajStrNew();
	midsrlength = 0;
	ori = 0;
	trgsrstart = 0;
	trgsrend = 0;
	trgsrid = 0;
	trgsrname = ajStrNew();
	trgsrlength = 0;
	
	sqlr = ajSqlRowIterGet(sqli);
	
	if(ensCoordSystemMatch(midcs, asmcs))
	{
	    ajSqlColumnToInt(sqlr, &trgsrstart);
	    ajSqlColumnToInt(sqlr, &trgsrend);
	    ajSqlColumnToUint(sqlr, &trgsrid);
	    ajSqlColumnToStr(sqlr, &trgsrname);
	    ajSqlColumnToInt(sqlr, &trgsrlength);
	    ajSqlColumnToInt(sqlr, &ori);
	    ajSqlColumnToInt(sqlr, &midsrstart);
	    ajSqlColumnToInt(sqlr, &midsrend);
	    ajSqlColumnToUint(sqlr, &midsrid);
	    ajSqlColumnToStr(sqlr, &midsrname);
	    ajSqlColumnToInt(sqlr, &midsrlength);
	}
	else
	{
	    ajSqlColumnToInt(sqlr, &midsrstart);
	    ajSqlColumnToInt(sqlr, &midsrend);
	    ajSqlColumnToUint(sqlr, &midsrid);
	    ajSqlColumnToStr(sqlr, &midsrname);
	    ajSqlColumnToInt(sqlr, &midsrlength);
	    ajSqlColumnToInt(sqlr, &ori);
	    ajSqlColumnToInt(sqlr, &trgsrstart);
	    ajSqlColumnToInt(sqlr, &trgsrend);
	    ajSqlColumnToUint(sqlr, &trgsrid);
	    ajSqlColumnToStr(sqlr, &trgsrname);
	    ajSqlColumnToInt(sqlr, &trgsrlength);
	}
	
	/*
	** Load the results into the target <-> middle Mapper of the
	** Chained Assembly Mapper.
	*/
	
	ensMapperAddCoordinates(trgmidmapper,
				trgsrid,
				trgsrstart,
				trgsrend,
				ori,
				midsrid,
				midsrstart,
				midsrend);
	
	/*
	** Register the target Mapper Unit in the
	** target Mapper Range Registry.
	*/
	
	ensMapperRangeRegistryCheckAndRegister(cam->TargetRegistry,
					       trgsrid,
					       1,
					       trgsrlength,
					       1,
					       trgsrlength,
					       (AjPList) NULL);
	
	/*
	** Add the target Ensembl Sequence Region into the
	** Ensembl Sequence Region Adaptor cache.
	*/
	
	sr = ensSeqRegionNew(sra, trgsrid, trgcs, trgsrname, trgsrlength);
	
	ensSeqRegionAdaptorCacheInsert(sra, &sr);
	
	ensSeqRegionDel(&sr);
	
	ajStrDel(&srcsrname);
	
	ajStrDel(&trgsrname);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    srctype = ajStrNewC("source");
    
    assemblyMapperAdaptorBuildCombinedMapper(ama,
					     ranges,
					     srcmidmapper,
					     trgmidmapper,
					     srctrgmapper,
					     srctype);
    
    ajStrDel(&srctype);
    
    while (ajListPop(ranges, (void **) &mu))
	ensMapperUnitDel(&mu);
    
    ajListFree(&ranges);
    
    ajStrDel(&sqlfmt);
    
    return ajTrue;
}
