/******************************************************************************
** @source Ensembl Slice functions
**
** An Ensembl Slice object represents a region of a genome. It can be used to
** retrieve sequence or features from an area of interest.
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

#include "ensslice.h"
#include "ensattribute.h"
#include "enssequence.h"
#include "ensrepeat.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAssemblyExceptionAdaptor
ensRegistryGetAssemblyExceptionAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPAssemblyMapperAdaptor
ensRegistryGetAssemblyMapperAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPCoordSystemAdaptor
ensRegistryGetCoordSystemAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPRepeatFeatureAdaptor
ensRegistryGetRepeatFeatureAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPSequenceAdaptor
ensRegistryGetSequenceAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPSeqRegionAdaptor
ensRegistryGetSeqRegionAdaptor(EnsPDatabaseAdaptor dba);

static EnsPProjectionSegment sliceConstrainToSeqRegion(EnsPSlice slice);

static void* sliceAdaptorCacheReference(void *value);

static void sliceAdaptorCacheDelete(void **value);

static ajuint sliceAdaptorCacheSize(const void* value);




/* @filesection ensslice ******************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPSlice] Slice *********************************************
**
** Functions for manipulating Ensembl Slice objects
**
** @cc Bio::EnsEMBL::Slice CVS Revision: 1.239
**
** @nam2rule Slice
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Slice by pointer.
** It is the responsibility of the user to first destroy any previous
** Slice. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPSlice]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPSlice] Ensembl Slice
** @argrule Ref object [EnsPSlice] Ensembl Slice
**
** @valrule * [EnsPSlice] Ensembl Slice
**
** @fcategory new
******************************************************************************/




/* @func ensSliceNewS *********************************************************
**
** Construct a new Ensembl Slice with sequence information.
**
** @cc Bio::EnsEMBL::Slice::new
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [u] sr [EnsPSeqRegion] Ensembl Sequence Region
** @param [r] start [ajint] Ensembl Sequence Region start
** @param [r] end [ajint] Ensembl Sequence Region end
** @param [r] strand [ajint] Ensembl Sequence Region strand
** @param [u] sequence [AjPStr] Sequence (optional)
**
** @return [EnsPSlice] Ensembl Slice or NULL
** @@
******************************************************************************/

EnsPSlice ensSliceNewS(EnsPSliceAdaptor adaptor,
                       EnsPSeqRegion sr,
                       ajint start,
                       ajint end,
                       ajint strand,
                       AjPStr sequence)
{
    EnsPSlice slice = NULL;
    
    if(!sr)
    {
        ajDebug("ensSliceNewS requires an Ensembl Sequence Region.\n");
	
	return NULL;
    }
    
    if(ensCoordSystemIsTopLevel(ensSeqRegionGetCoordSystem(sr)))
    {
	ajDebug("ensSliceNewS cannot create a Slice on a Sequence Region "
		"with a top-level Coordinate System.\n");
	
	return NULL;
    }
    
    if(!start)
	start = 1;
    
    if(!end)
	end = ensSeqRegionGetLength(sr);
    
    if(start > (end + 1))
    {
        ajDebug("ensSliceNewS requires that the start coordinate %d is "
		"less than or equal to the end coordinate %d + 1.\n",
		start, end);
	
        return NULL;
    }
    
    if(!strand)
	strand = 1;
    
    if((strand != 1) && (strand != -1))
    {
	ajDebug("ensSliceNewS requires a strand of either 1 or -1 "
		"not %d.\n", strand);
	
	return NULL;
    }
    
    if(sequence &&
	(ajStrGetLen(sequence) != (ajuint) ensSeqRegionGetLength(sr)))
    {
	ajDebug("ensSliceNewS requires that the Sequence Region length %d "
		"matches the length of the Sequence string %u.\n",
		ensSeqRegionGetLength(sr), ajStrGetLen(sequence));
	
	return NULL;
    }
    
    AJNEW0(slice);
    
    slice->Adaptor = adaptor;
    
    slice->SeqRegion = ensSeqRegionNewRef(sr);
    
    if(sequence)
	slice->Sequence = ajStrNewRef(sequence);
    
    slice->Start = start;
    
    slice->End = end;
    
    slice->Strand = strand;
    
    slice->Use = 1;
    
    return slice;
}




/* @func ensSliceNew **********************************************************
**
** Default constructor for an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::new
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [u] sr [EnsPSeqRegion] Ensembl Sequence Region
** @param [r] start [ajint] Ensembl Sequence Region start
** @param [r] end [ajint] Ensembl Sequence Region end
** @param [r] strand [ajint] Ensembl Sequence Region strand
**
** @return [EnsPSlice] Ensembl Slice or NULL
** @@
******************************************************************************/

EnsPSlice ensSliceNew(EnsPSliceAdaptor adaptor,
                      EnsPSeqRegion sr,
                      ajint start,
                      ajint end,
                      ajint strand)
{
    EnsPSlice slice = NULL;
    
    if(!sr)
    {
        ajDebug("ensSliceNew requires an Ensembl Sequence Region.\n");
	
	return NULL;
    }
    
    if(ensCoordSystemIsTopLevel(ensSeqRegionGetCoordSystem(sr)))
    {
	ajDebug("ensSliceNew cannot create a Slice on a Sequence Region "
		"with a top-level Coordinate System.\n");
	
	return NULL;
    }
    
    if(!start)
	start = 1;
    
    if(!end)
	end = ensSeqRegionGetLength(sr);
    
    if(start > (end + 1))
    {
        ajDebug("ensSliceNew requires that the start coordinate %d is "
		"less than or equal to the end coordinate %d + 1.\n",
		start, end);
	
        return NULL;
    }
    
    if(!strand)
	strand = 1;
    
    if((strand != 1) && (strand != -1))
    {
	ajDebug("ensSliceNew requires a strand of either 1 or -1 "
		"not %d.\n", strand);
	
	return NULL;
    }
    
    AJNEW0(slice);
    
    slice->Adaptor = adaptor;
    
    slice->SeqRegion = ensSeqRegionNewRef(sr);
    
    slice->Sequence = (AjPStr) NULL;
    
    slice->Start = start;
    
    slice->End = end;
    
    slice->Strand = strand;
    
    slice->Use = 1;
    
    return slice;
}




/* @func ensSliceNewObj *******************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [EnsPSlice] Ensembl Slice
**
** @return [EnsPSlice] Ensembl Slice or NULL
** @@
******************************************************************************/

EnsPSlice ensSliceNewObj(EnsPSlice object)
{
    EnsPSlice slice = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(slice);
    
    slice->Adaptor = object->Adaptor;
    
    slice->SeqRegion = ensSeqRegionNewRef(object->SeqRegion);
    
    if(object->Sequence)
	slice->Sequence = ajStrNewRef(object->Sequence);
    
    slice->Start = object->Start;
    
    slice->End = object->End;
    
    slice->Strand = object->Strand;
    
    slice->Use = 1;
    
    return slice;
}




/* @func ensSliceNewRef *******************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] slice [EnsPSlice] Ensembl Slice
**
** @return [EnsPSlice] Ensembl Slice or NULL
** @@
******************************************************************************/

EnsPSlice ensSliceNewRef(EnsPSlice slice)
{
    if(!slice)
	return NULL;
    
    slice->Use++;
    
    return slice;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Slice.
**
** @fdata [EnsPSlice]
** @fnote None
**
** @nam3rule Del Destroy (free) a Slice object
**
** @argrule * Pslice [EnsPSlice*] Slice object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensSliceDel **********************************************************
**
** Default Ensembl Slice destructor.
**
** @param [d] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [void]
** @@
******************************************************************************/

void ensSliceDel(EnsPSlice *Pslice)
{
    EnsPSlice pthis = NULL;
    
    if(!Pslice)
        return;
    
    if(!*Pslice)
        return;

    pthis = *Pslice;
    
    /*
     ajDebug("ensSliceDel\n"
	     "  *Pslice %p\n",
	     *Pslice);
     */
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pslice = NULL;
	
	return;
    }
    
    ensSeqRegionDel(&pthis->SeqRegion);
    
    ajStrDel(&pthis->Sequence);
    
    AJFREE(pthis);

    *Pslice = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Slice object.
**
** @fdata [EnsPSlice]
** @fnote None
**
** @nam3rule Get Return Slice attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Slice Adaptor
** @nam4rule GetSeqRegion Return the Ensembl Sequence Region
** @nam4rule GetSequence Return the sequence
** @nam4rule GetStart Return the start
** @nam4rule GetEnd Return the end
** @nam4rule GetStrand Return the strand
**
** @argrule * slice [const EnsPSlice] Slice
**
** @valrule Adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @valrule SeqRegion [EnsPSeqRegion] Ensembl Sequence Region
** @valrule Sequence [AjPStr] Sequence
** @valrule Start [ajint] Start
** @valrule End [ajint] End
** @valrule Strand [ajint] Strand
**
** @fcategory use
******************************************************************************/




/* @func ensSliceGetAdaptor ***************************************************
**
** Get the Ensembl Slice Adaptor element an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::adaptor
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @@
******************************************************************************/

EnsPSliceAdaptor ensSliceGetAdaptor(const EnsPSlice slice)
{
    if(!slice)
        return NULL;
    
    return slice->Adaptor;
}




/* @func ensSliceGetSeqRegion *************************************************
**
** Get the Ensembl Sequence Region element of an Ensembl Slice.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [EnsPSeqRegion] Ensembl Sequence Region
** @@
******************************************************************************/

EnsPSeqRegion ensSliceGetSeqRegion(const EnsPSlice slice)
{
    if(!slice)
        return NULL;
    
    return slice->SeqRegion;
}




/* @func ensSliceGetSequence **************************************************
**
** Get the sequence element of an Ensembl Slice.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [const AjPStr] Sequence
** @@
******************************************************************************/

const AjPStr ensSliceGetSequence(const EnsPSlice slice)
{
    if(!slice)
	return NULL;
    
    return slice->Sequence;
}




/* @func ensSliceGetStart *****************************************************
**
** Get the start coordinate element of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::start
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajint] Start coordinate
** @@
******************************************************************************/

ajint ensSliceGetStart(const EnsPSlice slice)
{
    if(!slice)
	return 0;
    
    return slice->Start;
}




/* @func ensSliceGetEnd *******************************************************
**
** Get the end coordinate element of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::end
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajint] End coordinate
** @@
******************************************************************************/

ajint ensSliceGetEnd(const EnsPSlice slice)
{
    if(!slice)
	return 0;
    
    return slice->End;
}




/* @func ensSliceGetStrand ****************************************************
**
** Get the strand element of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::strand
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajint] Strand information
** @@
******************************************************************************/

ajint ensSliceGetStrand(const EnsPSlice slice)
{
    if(!slice)
	return 0;
    
    return slice->Strand;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Slice object.
**
** @fdata [EnsPSlice]
** @fnote None
**
** @nam3rule Set Set one element of a Slice
** @nam4rule SetAdaptor Set the Ensembl Slice Adaptor
** @nam4rule SetSequence Set the sequence
**
** @argrule * slice [EnsPSlice] Ensembl Slice object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensSliceSetAdaptor ***************************************************
**
** Set the Ensembl Slice Adaptor element of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::adaptor
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceSetAdaptor(EnsPSlice slice, EnsPSliceAdaptor adaptor)
{
    if(!slice)
        return ajFalse;
    
    slice->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensSliceSetSequence **************************************************
**
** Set the sequence element of an Ensembl Slice.
**
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [u] sequence [AjPStr] Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceSetSequence(EnsPSlice slice, AjPStr sequence)
{
    if(!slice)
        return ajFalse;
    
    ajStrDel(&slice->Sequence);
    
    if(sequence)
    {
	if(ajStrGetLen(sequence) == ensSliceGetLength(slice))
	    slice->Sequence = ajStrNewRef(sequence);
	else
	    ajFatal("ensSliceSetSequence got sequence of length %u, "
		    "which does not match the length of the Slice %u.\n",
		    ajStrGetLen(sequence),
		    ensSliceGetLength(slice));
    }
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Slice object.
**
** @fdata [EnsPSlice]
** @nam3rule Trace Report Ensembl Slice elements to debug file
**
** @argrule Trace slice [const EnsPSlice] Ensembl Slice
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensSliceTrace ********************************************************
**
** Trace an Ensembl Slice.
**
** @param [r] cs [const EnsPSlice] Ensembl Slice
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceTrace(const EnsPSlice slice, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!slice)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensSliceTrace %p\n"
	    "%S  Adaptor %p\n"
	    "%S  SeqRegion %p\n"
	    "%S  Sequence %p\n"
	    "%S  Start %d\n"
	    "%S  End %d\n"
	    "%S  Strand %d\n"
	    "%S  Use %u\n",
	    indent, slice,
	    indent, slice->Adaptor,
	    indent, slice->SeqRegion,
	    indent, slice->Sequence,
	    indent, slice->Start,
	    indent, slice->End,
	    indent, slice->Strand,
	    indent, slice->Use);
    
    ensSeqRegionTrace(slice->SeqRegion, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensSliceGetSeqRegionIdentifier ***************************************
**
** Get the identifier element of the Ensembl Sequence Region element of an
** Ensembl Slice.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajuint] Ensembl Sequence Region identifier
** @@
******************************************************************************/

ajuint ensSliceGetSeqRegionIdentifier(const EnsPSlice slice)
{
    if(!slice)
	return 0;
    
    return ensSeqRegionGetIdentifier(slice->SeqRegion);
}




/* @func ensSliceGetSeqRegionName *********************************************
**
** Get the name element of the Ensembl Sequence Region element of an
** Ensembl Slice.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [const AjPStr] Ensembl Sequence Region name
** @@
******************************************************************************/

const AjPStr ensSliceGetSeqRegionName(const EnsPSlice slice)
{
    if(!slice)
        return NULL;
    
    return ensSeqRegionGetName(slice->SeqRegion);
}




/* @func ensSliceGetSeqRegionLength *******************************************
**
** Get the length element of the Ensembl Sequence Region element of an
** Ensembl Slice.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajint] Ensembl Sequence Region length
** @@
******************************************************************************/

ajint ensSliceGetSeqRegionLength(const EnsPSlice slice)
{
    if(!slice)
        return 0;
    
    return ensSeqRegionGetLength(slice->SeqRegion);
}




/* @func ensSliceGetCoordSystem ***********************************************
**
** Get the Ensembl Coordinate System element of the
** Ensembl Sequence Region element of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::coord_system
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [EnsPCoordSystem] Ensembl Coordinate System
** @@
******************************************************************************/

EnsPCoordSystem ensSliceGetCoordSystem(const EnsPSlice slice)
{
    if(!slice)
	return NULL;
    
    if(!slice->SeqRegion)
    {
	ajDebug("ensSliceGetCoordSystem cannot get the Coordinate System of "
		"a Slice without a Sequence Region.\n");
	
	return NULL;
    }
    
    return ensSeqRegionGetCoordSystem(slice->SeqRegion);
}




/* @func ensSliceGetCoordSystemName *******************************************
**
** Get the name element of the Ensembl Coordinate System element of the
** Ensembl Sequence Region element of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::coord_system_name
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [const AjPStr] Ensembl Coordinate System name
** @@
******************************************************************************/

const AjPStr ensSliceGetCoordSystemName(const EnsPSlice slice)
{
    if(!slice)
        return NULL;
    
    if(!slice->SeqRegion)
    {
	ajDebug("ensSliceGetCoordSystemName cannot get the Coordinate System "
		"name of a Slice without a Sequence Region.\n");
	
	return NULL;
    }
    
    return ensCoordSystemGetName(ensSeqRegionGetCoordSystem(slice->SeqRegion));
}




/* @func ensSliceGetCoordSystemVersion ****************************************
**
** Get the version element of the Ensembl Coordinate System element of the
** Ensembl Sequence Region element of an Ensembl Slice.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [const AjPStr] Ensembl Coordinate System version
** @@
******************************************************************************/

const AjPStr ensSliceGetCoordSystemVersion(const EnsPSlice slice)
{
    if(!slice)
        return NULL;
    
    if(!slice->SeqRegion)
    {
	ajDebug("ensSliceGetCoordSystemVersion cannot get the "
		"Coordinate System version for a Slice without a "
		"Sequence Region.\n");
	
	return NULL;
    }
    
    return ensCoordSystemGetVersion(ensSeqRegionGetCoordSystem(
                                        slice->SeqRegion));
}




/* @func ensSliceGetCentrePoint ***********************************************
**
** Get the centre point coordinate of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::centrepoint
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajint] Centre point coordinate or 0
** @@
******************************************************************************/

ajint ensSliceGetCentrePoint(const EnsPSlice slice)
{
    if(!slice)
        return 0;
    
    return ((slice->Start + slice->End) / 2);
}




/* @func ensSliceGetLength ****************************************************
**
** Get the length of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::length
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajuint] Ensembl Slice length
** @@
******************************************************************************/

ajuint ensSliceGetLength(const EnsPSlice slice)
{
    if(!slice)
        return 0;
    
    return (ajuint) (slice->End - slice->Start + 1);
}




/* @func ensSliceGetMemSize ***************************************************
**
** Get the memory size in bytes of an Ensembl Slice.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensSliceGetMemSize(const EnsPSlice slice)
{
    ajuint size = 0;
    
    if(!slice)
	return 0;
    
    size += (ajuint) sizeof (EnsOSlice);
    
    size += ensSeqRegionGetMemSize(slice->SeqRegion);
    
    if(slice->Sequence)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(slice->Sequence);
    }
    
    return size;
}




/* @func ensSliceFetchName ****************************************************
**
** Fetch the name of an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::name
** @param [r] slice [const EnsPSlice] Ensembl Slice
** @param [wP] Pname [AjPStr*] Name String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchName(const EnsPSlice slice, AjPStr* Pname)
{
    EnsPCoordSystem cs = NULL;
    
    if(!slice)
        return ajFalse;
    
    if(!Pname)
        return ajFalse;
    
    cs = ensSeqRegionGetCoordSystem(slice->SeqRegion);
    
    *Pname = ajFmtStr("%S:%S:%S:%d:%d:%d",
		      ensCoordSystemGetName(cs),
		      ensCoordSystemGetVersion(cs),
		      ensSeqRegionGetName(slice->SeqRegion),
		      slice->Start,
		      slice->End,
		      slice->Strand);
    
    return ajTrue;
}




/* @func ensSliceMatch ********************************************************
**
** Test for matching two Ensembl Slices.
**
** @param [r] slice1 [const EnsPSlice] First Ensembl Slice
** @param [r] slice2 [const EnsPSlice] Second Ensembl Slice
**
** @return [AjBool] ajTrue if the Slices are equal
** @@
** The comparison is based on an initial pointer equality test and if that
** fails, the Coordinate System, the Sequence Region, as well as the Slice
** start, end and strand elements are compared. In case the Slice has a
** sequence String attached, it is compared as well.
******************************************************************************/

AjBool ensSliceMatch(const EnsPSlice slice1, const EnsPSlice slice2)
{
    /*
     ajDebug("ensSliceMatch\n"
	     "  slice1 %p\n"
	     "  slice2 %p\n",
	     slice1,
	     slice2);
     
     ensSliceTrace(slice1, 1);
     
     ensSliceTrace(slice2, 1);
     */
    
    if(!slice1)
	return ajFalse;
    
    if(!slice2)
	return ajFalse;
    
    /* Try a direct pointer comparison first. */
    
    if(slice1 == slice2)
	return ajTrue;
    
    if(!ensSeqRegionMatch(slice1->SeqRegion, slice2->SeqRegion))
	return ajFalse;
    
    if(slice1->Start != slice2->Start)
	return ajFalse;
    
    if(slice1->End != slice2->End)
	return ajFalse;
    
    if(slice1->Strand != slice2->Strand)
	return ajFalse;
    
    if((slice1->Sequence || slice2->Sequence) &&
	(!ajStrMatchS(slice1->Sequence, slice2->Sequence)))
	return ajFalse;
    
    return ajTrue;
}




/* @func ensSliceIsTopLevel ***************************************************
**
** Test whether an Ensembl Slice is based on an Ensembl Sequence Region, which
** has an Ensembl Attribute of value 'top-level' set.
**
** @cc Bio::EnsEMBL::Slice::is_toplevel
** @param [r] slice [EnsPSlice] Ensembl Slice
**
** @return [AjBool] ajTrue if the Ensembl Sequence Region has a 'top-level'
**                  Ensembl Attribute set
** @@
******************************************************************************/

AjBool ensSliceIsTopLevel(EnsPSlice slice)
{
    if(!slice)
	return ajFalse;
    
    if(!slice->SeqRegion)
    {
	ajDebug("ensSliceIsTopLevel requires the Ensembl Slice to have an "
		"Ensembl Sequence region attached.\n");
	
	return ajFalse;
    }
    
    return ensSeqRegionIsTopLevel(slice->SeqRegion);
}




/* @func ensSliceFetchSequenceSeq *********************************************
**
** Fetch the sequence of an Ensembl Slice as AJAX Sequence.
**
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [wP] Psequence [AjPSeq*] AJAX Sequence address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchSequenceSeq(EnsPSlice slice, AjPSeq *Psequence)
{
    AjPStr name     = NULL;
    AjPStr sequence = NULL;
    
    if(!slice)
	return ajFalse;
    
    if(!Psequence)
	return ajFalse;
    
    /* Fetch the Slice name. */
    
    name = ajStrNew();
    
    ensSliceFetchName(slice, &name);
    
    /* Fetch the Slice sequence. */
    
    sequence = ajStrNew();
    
    ensSliceFetchSequenceStr(slice, &sequence);
    
    *Psequence = ajSeqNewNameS(sequence, name);
    
    ajStrDel(&sequence);
    ajStrDel(&name);
    
    return ajTrue;
}




/* @func ensSliceFetchSequenceStr *********************************************
**
** Fetch the sequence of an Ensembl Slice as AJAX String.
**
** @cc Bio::EnsEMBL::Slice::seq
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [wP] Psequence [AjPStr*] Sequence String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchSequenceStr(EnsPSlice slice, AjPStr *Psequence)
{
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPSequenceAdaptor sa = NULL;
    
    if(!slice)
	return ajFalse;
    
    if(!Psequence)
	return ajFalse;
    
    if(*Psequence)
	ajStrAssignClear(Psequence);
    else
	*Psequence = ajStrNewRes(ensSliceGetLength(slice) + 1);
    
    /* Special case for "in-between" (insertion) coordinates. */
    
    if(slice->Start == (slice->End + 1))
	return ajTrue;
    
    if(slice->Sequence)
    {
	/*
	** Since the Slice has sequence attached, check whether its
	** Slice length matches its sequence length.
	*/
	
	if(ajStrGetLen(slice->Sequence) != ensSliceGetLength(slice))
	    ajFatal("ensSliceFetchSequenceStr got a Slice, "
		    "which sequence length %u does not match its length %u.\n",
		    ajStrGetLen(slice->Sequence),
		    ensSliceGetLength(slice));
	
	ajStrAssignS(Psequence, slice->Sequence);
    }
    else if(slice->Adaptor)
    {
	/*
	** Since the Slice has a Slice Adaptor attached, it is possible to
	** retrieve the sequence from the database.
	*/
	
	dba = ensSliceAdaptorGetDatabaseAdaptor(slice->Adaptor);
	
	sa = ensRegistryGetSequenceAdaptor(dba);
	
	ensSequenceAdaptorFetchStrBySlice(sa, slice, Psequence);
    }
    else
    {
	/*
	** The Slice has no sequence and no Slice Adaptor attached,
	** so just return Ns.
	*/
	
	ajStrAppendCountK(Psequence, 'N', ensSliceGetLength(slice));
    }
    
    if(ajStrGetLen(*Psequence) != ensSliceGetLength(slice))
	ajWarn("ensSliceFetchSequenceStr got sequence of length %u "
	       "for Ensembl Slice of length %u.\n",
	       ajStrGetLen(*Psequence),
	       ensSliceGetLength(slice));
    
    return ajTrue;
}




/* @func ensSliceFetchSubSequenceSeq ******************************************
**
** Fetch a sub-sequence of an Ensembl Slice as AJAX Sequence.
**
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [wP] Psequence [AjPSeq*] Sequence address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchSubSequenceSeq(EnsPSlice slice,
                                   ajint start,
                                   ajint end,
                                   ajint strand,
                                   AjPSeq* Psequence)
{
    ajint srstart  = 0;
    ajint srend    = 0;
    ajint srstrand = 0;
    
    AjPStr name     = NULL;
    AjPStr sequence = NULL;
    
    if(!slice)
	return ajFalse;
    
    if(!strand)
	strand = 1;
    
    if(!Psequence)
	return ajFalse;
    
    /*
    ** Construct the Slice name, but convert relative Slice coordinates into
    ** absolute Sequence Region coordinates.
    */
    
    if(slice->Strand >= 0)
    {
	srstart = slice->Start + start - 1;
	
	srend = slice->Start + end - 1;
    }
    else
    {
	srstart = slice->End - end + 1;
	
	srend = slice->End - start + 1;
    }
    
    srstrand = slice->Strand * strand;
    
    name = ajFmtStr("%S:%S:%S:%d:%d:%d",
		    ensSliceGetCoordSystemName(slice),
		    ensSliceGetCoordSystemVersion(slice),
		    ensSliceGetSeqRegionName(slice),
		    srstart,
		    srend,
		    srstrand);
    
    /* Fetch the Slice sequence. */
    
    sequence = ajStrNew();
    
    ensSliceFetchSubSequenceStr(slice, start, end, strand, &sequence);
    
    *Psequence = ajSeqNewNameS(sequence, name);
    
    ajStrDel(&sequence);
    ajStrDel(&name);
    
    return ajTrue;
}




/* @func ensSliceFetchSubSequenceStr ******************************************
**
** Fetch a sub-sequence of an Ensembl Slice as AJAX String
** in releative coordinates.
**
** @cc Bio::EnsEMBL::Slice::subseq
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [wP] Psequence [AjPStr*] Sequence address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchSubSequenceStr(EnsPSlice slice,
                                   ajint start,
                                   ajint end,
                                   ajint strand,
                                   AjPStr *Psequence)
{
    ajint length = 0;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPSequenceAdaptor sa = NULL;
    
    /*
     ajDebug("ensSliceFetchSubSequenceStr\n"
	     "  slice %p\n"
	     "  start %d\n"
	     "  end %d\n"
	     "  strand %d\n"
	     "  Psequence %p\n",
	     slice,
	     start,
	     end,
	     strand,
	     Psequence);
     */
    
    if(!slice)
	return ajFalse;
    
    if(start > (end + 1))
    {
	ajDebug("ensSliceFetchSubSequenceStr requires the start coordinate %d "
		"to be less than the end coordinate %d + 1.\n",
		start, end);
	
	return ajFalse;
    }
    
    if(!strand)
	strand = 1;
    
    if(!Psequence)
	return ajFalse;
    
    if(*Psequence)
	ajStrAssignClear(Psequence);
    else
	*Psequence = ajStrNewRes((ajuint) (end - start + 1));
    
    /* For "in-between" (insertion) coordinates return an empty string. */
    
    if(start == (end + 1))
	return ajTrue;
    
    if(slice->Sequence)
    {
	/*
	** Since the Slice has sequence attached, check whether
	** Slice length and sequence length match.
	*/
	
	if(ajStrGetLen(slice->Sequence) != ensSliceGetLength(slice))
	    ajFatal("ensSliceFetchSubSequenceStr got a Slice, "
		    "which sequence length (%u) does not match its "
		    "length (%u).\n",
		    ajStrGetLen(slice->Sequence),
		    ensSliceGetLength(slice));
	
	/* Relative Slice coordinates range from 1 to length. */
	
	/* Check for a gap at the beginning and pad it with Ns. */
	
	if(start < 1)
	{
	    ajStrAppendCountK(Psequence, 'N', 1 - start);
	    
	    start = 1;
	}
	
	ajStrAppendSubS(Psequence,
			slice->Sequence,
			start - 1,
			end - start + 1);
	
	/* Check that the Slice is within signed integer range. */
	
	if(ensSliceGetLength(slice) <= INT_MAX)
	    length = (ajint) ensSliceGetLength(slice);
	else
	    ajFatal("ensSliceFetchSubSequenceStr got an "
		    "Ensembl Slice, which length (%u) exceeds the "
		    "maximum integer limit (%d).\n",
		    ensSliceGetLength(slice), INT_MAX);
	
	/* Check for a gap at the end and pad it again with Ns. */
	
	if(end > length)
	    ajStrAppendCountK(Psequence, 'N', (ajuint) (end - length));
	
	if(strand < 0)
	    ajSeqstrReverse(Psequence);
    }
    else if(slice->Adaptor)
    {
	/*
	** Since the Slice has a Slice Adaptor attached, it is possible to
	** retrieve the sequence from the database.
	*/
	
	dba = ensSliceAdaptorGetDatabaseAdaptor(slice->Adaptor);
	
	sa = ensRegistryGetSequenceAdaptor(dba);
	
	ensSequenceAdaptorFetchSubStrBySlice(sa,
					     slice,
					     start,
					     end,
					     strand,
					     Psequence);
    }
    
    else
    {
	/*
	** The Slice has no sequence and no Slice Adaptor attached,
	** so just return Ns.
	*/
	
	ajStrAppendCountK(Psequence, 'N', (ajuint) (end - start + 1));
    }
    
    if(ajStrGetLen(*Psequence) != (ajuint) (end - start + 1))
	ajWarn("ensSliceFetchSubSequenceStr got sequence of length %u "
	       "for region of length %u.\n",
	       ajStrGetLen(*Psequence),
	       (ajuint) (end - start + 1));
    
    return ajTrue;
}




/* @func ensSliceFetchInvertedSlice *******************************************
**
** Fetch an inverted Slice from an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::invert
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [wP] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchInvertedSlice(EnsPSlice slice, EnsPSlice *Pslice)
{
    AjPStr sequence = NULL;
    
    if(!slice)
	return ajFalse;
    
    if(!Pslice)
	return ajFalse;
    
    /*
    ** Retrieve the inverted Slice via the Slice Adaptor if one is attached.
    ** This will automatically register the new Slice in the
    ** Slice Adaptor-internal cache and will also take care of
    ** an eventual Slice-internal sequence.
    ** Use relative Slice coordinates, which range from 1 to length.
    */
    
    if(slice->Adaptor)
    {
	ensSliceAdaptorFetchBySlice(slice->Adaptor,
				    slice,
				    1,
				    (ajint) ensSliceGetLength(slice),
				    -1,
				    Pslice);
	return ajTrue;
    }
    
    if(slice->Sequence)
    {
	/* Reverse (and complement) the sequence if one has been defined. */
	
        sequence = ajStrNewS(slice->Sequence);
	
	ajSeqstrReverse(&sequence);
	
	*Pslice = ensSliceNewS(slice->Adaptor,
			       slice->SeqRegion,
			       slice->Start,
			       slice->End,
			       slice->Strand * -1,
			       sequence);
	
	ajStrDel(&sequence);
    }
    else
	*Pslice = ensSliceNew(slice->Adaptor,
			      slice->SeqRegion,
			      slice->Start,
			      slice->End,
			      slice->Strand * -1);
    
    return ajTrue;
}




/* @func ensSliceFetchSubSlice ************************************************
**
** Fetch a Sub-Slice from an Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::sub_Slice
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [wP] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchSubSlice(EnsPSlice slice,
                             ajint start,
                             ajint end,
                             ajint strand,
                             EnsPSlice *Pslice)
{
    ajint srstart  = 0;
    ajint srend    = 0;
    ajint srstrand = 0;
    
    AjPStr sequence = NULL;
    
    if(!slice)
    {
	ajDebug("ensSliceGetSubSlice requires an Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    if((start < 1) || (start > slice->End))
    {
	ajDebug("ensSliceGetSubSlice requires the start coordinate %d "
		"to be greater than or equal to 1 and less than or equal to "
		"the end coordinate %d of the Ensembl Slice.\n",
		start, slice->End);
	
	return ajFalse;
    }
    
    if((end < start) || (end > slice->End))
    {
	ajDebug("ensSliceGetSubSlice requires the end coordinate %d "
		"to be less than or equal to the start coordinate %d "
		"and to be less than or equal to the "
		"end coordinate %d of the Ensembl Slice.\n",
		end, start, slice->End);
	
	return ajFalse;
    }
    
    if(!strand)
	strand = 1;
    
    /*
    ** Retrieve the Sub-Slice via the Slice Adaptor if one is attached.
    ** This will automatically register the new Slice in the
    ** Slice Adaptor-internal cache and will also take care of
    ** an eventual Slice-internal sequence.
    */
    
    if(slice->Adaptor)
    {
	ensSliceAdaptorFetchBySlice(slice->Adaptor,
				    slice,
				    start,
				    end,
				    strand,
				    Pslice);
	
	return ajTrue;
    }
    
    /* Transform relative into absolute Slice coordinates. */
    
    if(slice->Strand >= 0)
    {
	srstart = slice->Start + start - 1;
	
	srend = slice->Start + end - 1;
    }
    else
    {
	srstart = slice->End - end + 1;
	
	srend = slice->End - start + 1;
    }
    
    srstrand = slice->Strand * strand;
    
    if(slice->Sequence)
    {
	sequence = ajStrNewRes(end - start + 1);
	
	ensSliceFetchSubSequenceStr(slice,
				    start,
				    end,
				    strand,
				    &sequence);
				    
	*Pslice = ensSliceNewS(slice->Adaptor,
			       slice->SeqRegion,
			       srstart,
			       srend,
			       srstrand,
			       sequence);
	
	ajStrDel(&sequence);
    }
    else
	*Pslice = ensSliceNew(slice->Adaptor,
			      slice->SeqRegion,
			      srstart,
			      srend,
			      srstrand);
    
    return ajTrue;
}




/* @func ensSliceFetchExpandedSlice *******************************************
**
** Fetch an expanded Ensembl Slice.
**
** @cc Bio::EnsEMBL::Slice::expand
** @param [r] slice [const EnsPSlice] Ensembl Slice
** @param [r] five [ajint] Number of bases to expand the 5' region
**            Positive values expand the Slice, negative values contract it
** @param [r] three [ajint] Number of bases to expand the 3' region
**            Positive values expand the Slice, negative values contract it
** @param [r] force [AjBool] Force Slice contraction
** @param [w] Pfive [ajint*] Maximum possible 5' shift when 'force' is set
** @param [w] Pthree [ajint*] Maximum possible 3' shift when 'force' is set
** @param [wP] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** Returns a slice which is a resized copy of this slice. The start and end
** are moved outwards from the center of the slice if positive values are
** provided and moved inwards if negative values are provided. This slice
** remains unchanged. A slice may not be contracted below 1 basepair but may
** grow to be arbitrarily large.
**
** Note on 'force' slice contraction:
** The slice will be contracted even in the case when shifts 'five' and 'three'
** overlap. In that case 'five' and 'three' will be set to a maximum possible
** number and that will result in the slice which would have only 2 base pairs.
******************************************************************************/

/*
** FIXME: This should probably go though the Slice Adaptor so that the Slice
** would be inserted into the Adaptor cache. Expanded Slices would then exist
** in memory only once.
**
** FIXME: The ensSliceFetchSubSlice function does in principle the same and
** could expand a Slice by providing a negative start and an end that lies
** beyond the length. The reason why two functions are required are Slices
** that have a sequence attached. It is possible to contract them (sub-slice)
** but it is not possible to expand them without loosing information.
*/

AjBool ensSliceFetchExpandedSlice(const EnsPSlice slice,
                                  ajint five,
                                  ajint three,
                                  AjBool force,
                                  ajint *Pfive,
                                  ajint *Pthree,
                                  EnsPSlice *Pslice)
{
    ajint sshift = 0;
    ajint eshift = 0;
    
    ajuint srid   = 0;
    ajint srstart = 0;
    ajint srend   = 0;
    
    /*
     ajDebug("ensSliceFetchExpandedSlice\n"
	     "  slice %p\n"
	     "  five %d\n"
	     "  three %d\n"
	     "  force '%B'\n"
	     "  Pslice %p\n",
	     slice,
	     five,
	     three,
	     force,
	     Pslice);
     */
    
    if(!slice)
        return ajFalse;
    
    if(!Pslice)
	return ajFalse;
    
    *Pslice = (EnsPSlice) NULL;
    
    if(slice->Sequence)
    {
        ajDebug("ensSliceFetchExpandedSlice cannot expand an Ensembl Slice "
                "with an attached sequence.\n");
	
        return ajFalse;
    }
    
    if(slice->Strand > 0)
    {
        sshift = five;
	
        eshift = three;
    }
    else
    {
        sshift = three;
	
        eshift = five;
    }
    
    srid = ensSeqRegionGetIdentifier(slice->SeqRegion);
    
    srstart = slice->Start - sshift;
    
    srend = slice->End + eshift;
    
    if(srstart > srend)
    {
        if(force)
        {
            /* Apply the maximal possible shift, if force is set. */
	    
            if(sshift < 0)
	    {
		/*
		** If we are contracting the slice from the start,
		** move the start just before the end.
		*/
		
                srstart = srend - 1;
		
                sshift = slice->Start - srstart;
            }
	    
            if(srstart > srend)
            {
                /*
		** If the Slice still has a negative length,
		** try to move the end.
		*/
		
                if(eshift < 0)
                {
                    srend = srstart + 1;
		    
                    eshift = srend - slice->End;
                }
		
                *Pfive = (slice->Strand >= 0) ? eshift : sshift;
		
                *Pthree = (slice->Strand >= 0) ? sshift : eshift;
            }
        }
	
        if(srstart > srend)
        {
            /* If the Slice still has a negative length, return NULL. */
	    
	    ajDebug("ensSliceFetchExpandedSlice requires the Slice start %d "
		    "to be less than the Slice end %d coordinate.\n",
		    srstart, srend);
	    
            return ajFalse;
        }
    }
    
    if(slice->Adaptor)
	ensSliceAdaptorFetchBySeqRegionIdentifier(slice->Adaptor,
						  srid,
						  srstart,
						  srend,
						  slice->Strand,
						  Pslice);
    else
	*Pslice = ensSliceNew(slice->Adaptor,
			      slice->SeqRegion,
			      srstart,
			      srend,
			      slice->Strand);
    
    return ajTrue;
}




/* @funcstatic sliceConstrainToSeqRegion **************************************
**
** Constrain an Ensembl Slice to its Ensembl Sequence Region element.
**
** @cc Bio::EnsEMBL::Slice::_constrain_to_region
** @param [r] slice [EnsPSlice] Ensembl Slice
**
** @return [EnsPProjectionSegment] Ensembl Projection Segment or NULL
** @@
******************************************************************************/

/*
** FIXME: This should probably go though the Slice Adaptor so that the Slice
** would be inserted into the Adaptor cache. Expanded Slices would then exist
** in memory only once.
*/

static EnsPProjectionSegment sliceConstrainToSeqRegion(EnsPSlice slice)
{
    ajint sshift = 0;
    ajint eshift = 0;
    
    ajint *Pfive  = NULL;
    ajint *Pthree = NULL;
    
    EnsPProjectionSegment ps = NULL;
    
    EnsPSlice nslice = NULL;
    
    if(!slice)
    {
        ajDebug("sliceConstrainToSeqRegion requires an Ensembl Slice.\n");
	
	return NULL;
    }
    
    /*
    ** If the Slice has negative coordinates or coordinates exceeding the
    ** length of the Sequence Region the Slice needs shrinking to the
    ** defined Sequence Region.
    */
    
    /* Return NULL, if this Slice does not overlap with its Sequence Region. */
    
    if((slice->Start > ensSeqRegionGetLength(slice->SeqRegion)) ||
	(slice->End < 1))
	return NULL;
    
    if(slice->Start < 1)
	sshift = slice->Start - 1;
    
    if(slice->End > ensSeqRegionGetLength(slice->SeqRegion))
	eshift = ensSeqRegionGetLength(slice->SeqRegion) - slice->End;
    
    if(sshift || eshift)
	ensSliceFetchExpandedSlice(slice,
				   sshift,
				   eshift,
				   ajFalse,
				   Pfive,
				   Pthree,
				   &nslice);
    else
	nslice = ensSliceNewRef(slice);
    
    ps = ensProjectionSegmentNew(1 - sshift,
				 ensSliceGetLength(slice) + eshift,
				 nslice);
    
    ensSliceDel(&nslice);
    
    return ps;
}




/* @func ensSliceProject ******************************************************
**
** Project an Ensembl Slice onto another Ensembl Coordinate System.
** Projecting this Slice onto a Coordinate System that the Slice is assembled
** from is analogous to retrieving a tiling path. This method may also be used
** to project up-wards to a higher-level Ensembl Coordinate System.
**
** The caller is responsible for deleting the Ensembl Projection Segments
** before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Slice::project
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [r] csname [const AjPStr] Ensembl Coordinate System name
** @param [r] csversion [const AjPStr] Ensembl Coordinate System version
** @param [u] pslist [AjList] AJAX List of Ensembl Projection Segments
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceProject(EnsPSlice slice,
                       const AjPStr csname,
                       const AjPStr csversion,
                       AjPList pslist)
{
    ajint start  = 0;
    ajint end    = 0;
    ajint length = 0;
    
    AjPList nrmpslist = NULL;
    AjPList mrs       = NULL;
    
    EnsPAssemblyMapper am         = NULL;
    EnsPAssemblyMapperAdaptor ama = NULL;
    
    EnsPCoordSystem srccs     = NULL;
    EnsPCoordSystem trgcs      = NULL;
    EnsPCoordSystem nrmcs      = NULL;
    EnsPCoordSystem mrcs       = NULL;
    EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPProjectionSegment nrmps = NULL;
    EnsPProjectionSegment ps    = NULL;
    
    EnsPMapperResult mr = NULL;
    
    const EnsPSlice nrmslc = NULL;
    EnsPSlice newslc = NULL;
    
    /*
     ajDebug("ensSliceProject\n"
	     "  slice %p\n"
	     "  csname '%S'\n"
	     "  cvsversion '%S'\n"
	     "  pslist %p\n",
	     slice,
	     csname,
	     csversion,
	     pslist);
     
     ensSliceTrace(slice, 1);
     */
    
    if(!slice)
    {
        ajDebug("ensSliceProject requires an Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    if(!csname)
    {
        ajDebug("ensSliceProject requires an "
                "Ensembl Coordinate System name.\n");
	
        return ajFalse;
    }
    
    if(!csversion)
    {
        ajDebug("ensSliceProject requires an "
		"Ensembl Coordinate System version.\n");
	
        return ajFalse;
    }
    
    if(!slice->Adaptor)
    {
	ajDebug("ensSliceProject requires that an Ensembl Slice Adaptor has "
		"been set in the Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    if(!slice->SeqRegion)
    {
	ajDebug("ensSliceProject requires that an Ensembl Sequence Region "
		"has been set in the Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    dba = ensSliceAdaptorGetDatabaseAdaptor(slice->Adaptor);
    
    ama = ensRegistryGetAssemblyMapperAdaptor(dba);
    
    csa = ensRegistryGetCoordSystemAdaptor(dba);
    
    /*
    ** Get the source Coordinate System, which is the
    ** Ensembl Coordinate System element of the
    ** Ensembl Sequence Region element of this Ensembl Slice.
    */
    
    srccs = ensSeqRegionGetCoordSystem(slice->SeqRegion);
    
    if(!srccs)
    {
	ajDebug("ensSliceProject requires that an Ensembl Coordinate System "
		"element has been set in the Ensembl Sequence Region element "
		"of the Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    /* Fetch the target Coordinate System. */
    
    ensCoordSystemAdaptorFetchByName(csa, csname, csversion, &trgcs);
    
    if(!trgcs)
    {
	ajDebug("ensSliceProject cannot project to an unknown "
                "Ensembl Coordinate System '%S:%S'.\n", csname, csversion);
	
	return ajFalse;
    }
    
    /*
    ** No mapping is needed if the requested Ensembl Coordinate System is the
    ** one this Ensembl Slice is based upon, but we do need to check if some
    ** of the Slice is outside of defined regions.
    */
    
    if(ensCoordSystemMatch(srccs, trgcs))
    {
        ajListPushAppend(pslist, (void *) sliceConstrainToSeqRegion(slice));
	
	ensCoordSystemDel(&trgcs);
	
	return ajTrue;
    }
    
    start = 1;
    
    /*
    ** Decompose this Slice into its symlinked components, which allows
    ** handling of haplotypes (HAPs) and pseudo-autosomal region (PARs).
    */
    
    nrmpslist = ajListNew();
    
    ensSliceAdaptorFetchNormalisedSliceProjection(slice->Adaptor,
						  slice,
						  nrmpslist);
    
    while(ajListPop(nrmpslist, (void **) &nrmps))
    {
	nrmslc = ensProjectionSegmentGetTrgSlice(nrmps);
	
	nrmcs = ensSeqRegionGetCoordSystem(nrmslc->SeqRegion);
	
	am = ensAssemblyMapperAdaptorFetchByCoordSystems(ama, nrmcs, trgcs);
	
	mrs = ajListNew();
	
	if(am)
	    ensAssemblyMapperMap(am,
				 nrmslc->SeqRegion,
				 nrmslc->Start,
				 nrmslc->End,
				 nrmslc->Strand,
				 mrs);
	else
	{
	    ajDebug("ensSliceProject got no AssemblyMapper -> gap\n");
	    
	    mr = MENSMAPPERGAPNEW(nrmslc->Start, nrmslc->End);
	    
	    ajListPushAppend(mrs, (void *) mr);
	}
	
	ensAssemblyMapperDel(&am);
	
	/* Construct a projection from the mapping results and return it. */
	
	while(ajListPop(mrs, (void **) &mr))
	{
	    length = ensMapperResultGetLength(mr);
	    
	    /* Skip gaps. */
	    
	    if(ensMapperResultGetType(mr) == ensEMapperResultCoordinate)
	    {
		mrcs = ensMapperResultGetCoordSystem(mr);
		
		/*
		** If the normalised projection just ended up mapping to the
		** same coordinate system we were already in then we should
		** just return the original region. This can happen for
		** example, if we were on a PAR region on Y which referred
		** to X and a projection to 'toplevel' was requested.
		*/
		
		if(ensCoordSystemMatch(mrcs, nrmcs))
		{
		    /* Trim off regions, which are not defined. */
		    
		    ajListPushAppend(pslist, (void *)
				     sliceConstrainToSeqRegion(slice));
		    
		    /*
		    ** Delete this Ensembl Mapper Result and the rest of the
		    ** Ensembl Mapper Results including the AJAX List.
		    */
		    
		    ensMapperResultDel(&mr);
		    
		    while(ajListPop(mrs, (void **) &mr))
			ensMapperResultDel(&mr);
		    
		    ajListFree(&mrs);
		    
		    /*
		    ** Delete this normalised Projection Segment and the rest
		    ** of the normalised Projection Segments including the
		    ** AJAX List.
		    */
		    
		    ensProjectionSegmentDel(&nrmps);
		    
		    while(ajListPop(nrmpslist, (void **) &nrmps))
		    	ensProjectionSegmentDel(&nrmps);
		    
		    ajListFree(&nrmpslist);
		    
		    /* Finally, delete the target Coordinate System. */
		    
		    ensCoordSystemDel(&trgcs);
		    
		    return ajTrue;
		}
		else
		{
		    /* Create a Slice for the target Coordinate System. */
		    
		    ensSliceAdaptorFetchBySeqRegionIdentifier(
                        slice->Adaptor,
                        ensMapperResultGetObjectIdentifier(mr),
                        ensMapperResultGetStart(mr),
                        ensMapperResultGetEnd(mr),
                        ensMapperResultGetStrand(mr),
                        &newslc);
		    
		    /*
		    ** TODO: We could have a ensSliceAdaptorFetchByMapperResult
		    ** function to simplify this.
		    */
		    
		    end = start + length - 1;
		    
		    ps = ensProjectionSegmentNew(start, end, newslc);
		    
		    ajListPushAppend(pslist, (void *) ps);
		    
		    ensSliceDel(&newslc);
		}
	    }
	    
	    start += length;
	    
	    ensMapperResultDel(&mr);
	}
	
	ajListFree(&mrs);
	
	ensProjectionSegmentDel(&nrmps);
    }
    
    ajListFree(&nrmpslist);
    
    ensCoordSystemDel(&trgcs);
    
    return ajTrue;
}




/* @func ensSliceProjectToSlice ***********************************************
**
** Project an Ensembl Slice onto another Ensembl Slice.
** Needed for cases where multiple assembly mappings exist and a specific
** mapping is specified.
**
** The caller is responsible for deleting the Ensembl Projection Segments
** before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Slice::project_to_slice
** @param [u] srcslice [EnsPSlice] Ensembl Slice
** @param [r] trgslice [EnsPSlice] Target Ensembl Slice
** @param [u] pslist [AjList] AJAX List of Ensembl Projection Segments
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensPSliceProjectToSlice(EnsPSlice srcslice,
                               EnsPSlice trgslice,
                               AjPList pslist)
{
    ajint start  = 0;
    ajint end    = 0;
    ajint length = 0;
    
    AjPList nrmpslist = NULL;
    AjPList mrs       = NULL;
    
    EnsPAssemblyMapper am         = NULL;
    EnsPAssemblyMapperAdaptor ama = NULL;
    
    EnsPCoordSystem srccs = NULL;
    EnsPCoordSystem trgcs = NULL;
    EnsPCoordSystem nrmcs = NULL;
    EnsPCoordSystem mrcs  = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPProjectionSegment nrmps = NULL;
    EnsPProjectionSegment ps    = NULL;
    
    EnsPMapperResult mr = NULL;
    
    const EnsPSlice nrmslc = NULL;
    EnsPSlice newslc       = NULL;
    
    if(!srcslice)
	return ajFalse;
    
    if(!trgslice)
	return ajFalse;
    
    dba = ensSliceAdaptorGetDatabaseAdaptor(srcslice->Adaptor);
    
    ama = ensRegistryGetAssemblyMapperAdaptor(dba);
    
    /*
    ** Get the source Coordinate System, which is the
    ** Ensembl Coordinate System element of the
    ** Ensembl Sequence Region element of this Ensembl Slice.
    */
    
    srccs = ensSeqRegionGetCoordSystem(srcslice->SeqRegion);
    
    if(!srccs)
    {
	ajDebug("ensSliceProjectToSlice requires that an "
		"Ensembl Coordinate System element has been set in the "
		"Ensembl Sequence Region element of the source "
		"Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    /*
    ** Get the target Coordinate System, which is the
    ** Ensembl Coordinate System element of the
    ** Ensembl Sequence Region element of this Ensembl Slice.
    */
    
    trgcs = ensSeqRegionGetCoordSystem(trgslice->SeqRegion);
    
    if(!trgcs)
    {
	ajDebug("ensSliceProjectToSlice requires that an "
		"Ensembl Coordinate System element has been set in the "
		"Ensembl Sequence Region element of the target "
		"Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    /*
    ** Decompose this Slice into its symlinked components, which allows
    ** handling of haplotypes (HAPs) and pseudo-autosomal region (PARs).
    */
    
    nrmpslist = ajListNew();
    
    ensSliceAdaptorFetchNormalisedSliceProjection(srcslice->Adaptor,
						  srcslice,
						  nrmpslist);
    
    while(ajListPop(nrmpslist, (void **) &nrmps))
    {
	nrmslc = ensProjectionSegmentGetTrgSlice(nrmps);
	
	nrmcs = ensSeqRegionGetCoordSystem(nrmslc->SeqRegion);
	
	am = ensAssemblyMapperAdaptorFetchByCoordSystems(ama, nrmcs, trgcs);
	
	mrs = ajListNew();
	
	if(am)
	    ensAssemblyMapperMapToSeqRegion(am,
					    nrmslc->SeqRegion,
					    nrmslc->Start,
					    nrmslc->End,
					    nrmslc->Strand,
					    trgslice->SeqRegion,
					    mrs);
	else
	{
	    ajDebug("ensSliceProject got no AssemblyMapper -> gap\n");
	    
	    mr = MENSMAPPERGAPNEW(nrmslc->Start, nrmslc->End);
	    
	    ajListPushAppend(mrs, (void *) mr);
	}
	
	ensAssemblyMapperDel(&am);
	
	/* Construct a projection from the mapping results and return it. */
	
	while(ajListPop(mrs, (void **) &mr))
	{
	    length = ensMapperResultGetLength(mr);
	    
	    /* Skip gaps. */
	    
	    if(ensMapperResultGetType(mr) == ensEMapperResultCoordinate)
	    {
		mrcs = ensMapperResultGetCoordSystem(mr);
		
		/* Create a Slice for the target Coordinate System. */
		
		ensSliceAdaptorFetchBySeqRegionIdentifier(
                    srcslice->Adaptor,
                    ensMapperResultGetObjectIdentifier(mr),
                    ensMapperResultGetStart(mr),
                    ensMapperResultGetEnd(mr),
                    ensMapperResultGetStrand(mr),
                    &newslc);
		
		/*
		** TODO: We could have a ensSliceAdaptorFetchByMapperResult
		** function to simplify this.
	        */
		
		end = start + length - 1;
		
		ps = ensProjectionSegmentNew(start, end, newslc);
		
		ajListPushAppend(pslist, (void *) ps);
		
		ensSliceDel(&newslc);
	    }
	    
	    start += length;
	    
	    ensMapperResultDel(&mr);
	}
	
	ajListFree(&mrs);
	
	ensProjectionSegmentDel(&nrmps);
    }
    
    ajListFree(&nrmpslist);
    
    ensCoordSystemDel(&trgcs);
    
    /*
    ** Delete the Ensembl Assembly Mapper Adaptor cache,
    ** as the next mapping may include a different set.
    */
    
    ensAssemblyMapperAdaptorCacheClear(ama);
    
    return ajTrue;
}




/* @func ensSliceFetchAllAttributes *******************************************
**
** Fetch all Ensembl Attributes for an Ensembl Slice.
** The caller is responsible for deleting the Ensembl Attributes before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Slice::get_all_Attributes
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [rN] code [const AjPStr] Ensembl Attribute code
** @param [u] attributes [AjPList] AJAX List of Ensembl Attributes
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchAllAttributes(EnsPSlice slice,
                                  const AjPStr code,
                                  AjPList attributes)
{
    if(!slice)
	return ajFalse;
    
    if(!attributes)
	return ajFalse;
    
    if(!slice->SeqRegion)
    {
	ajDebug("ensSliceFetchAllAttributes cannot fetch Ensembl Attributes "
		"for a Slice without a Sequence Region.\n");
	
	return ajFalse;
    }
    
    ensSeqRegionFetchAllAttributes(slice->SeqRegion, code, attributes);
    
    return ajTrue;
}




/* @func ensSliceFetchAllRepeatFeatures ***************************************
**
** Fetch all Ensembl Repeat Features on an Ensembl Slice.
** The caller is responsible for deleting the Ensembl Repeat Fetures before
** deleting the AJAX List.
**
** @param [r] slice [const EnsPSlice] Ensembl Slice
** @param [r] anname [AjPStr] Ensembl Analysis name
** @param [r] rctype [AjPStr] Ensembl Repeat Consensus type
** @param [r] rcclass [AjPStr] Ensembl Repeat Consensus class
** @param [r] rcname [AjPStr] Ensembl Repeat Consensus name
** @param [u] rfs [AjPList] AJAX List of Ensembl Repeat Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceFetchAllRepeatFeatures(EnsPSlice slice,
                                      AjPStr anname,
                                      AjPStr rctype,
                                      AjPStr rcclass,
                                      AjPStr rcname,
                                      AjPList rfs)
{
    EnsPRepeatFeatureAdaptor rfa = NULL;
    
    /*
     ajDebug("ensSliceFetchAllRepeatFeatures\n"
	     "  slice %p\n"
	     "  anname '%S'\n"
	     "  rctype '%S'\n"
	     "  rcclass '%S'\n"
	     "  rcname '%S'\n"
	     "  rfs %p\n",
	     slice,
	     anname,
	     rctype,
	     rcclass,
	     rcname,
	     rfs);
     */
    
    if(!slice)
	return ajFalse;
    
    if(!slice->Adaptor)
    {
	ajDebug("ensSliceFetchAllRepeatFeatures cannot get Repeat Features "
		"without a Slice Adaptor attached to the Slice.\n");
	
	return ajFalse;
    }
    
    rfa = ensRegistryGetRepeatFeatureAdaptor(slice->Adaptor->Adaptor);
    
    return ensRepeatFeatureAdaptorFetchAllBySlice(
        rfa,
        slice,
        anname,
        rctype,
        rcclass,
        rcname,
        rfs);
}




/* @datasection [EnsPSliceAdaptor] Slice Adaptor ******************************
**
** Functions for manipulating Ensembl Slice Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::SliceAdaptor CVS Revision: 1.104
**
** @nam2rule SliceAdaptor
**
******************************************************************************/

/* 1 << 26 = 64 MiB */

static ajuint sliceAdaptorCacheMaxBytes = 1 << 26;

/* 1 << 16 = 64 Ki  */

static ajuint sliceAdaptorCacheMaxCount = 1 << 16;

static ajuint sliceAdaptorCacheMaxSize = 0;




/* @funcstatic sliceAdaptorCacheReference *************************************
**
** Wrapper function to reference an Ensembl Slice
** from an Ensembl Cache.
**
** @param [u] value [void*] Ensembl Slice
**
** @return [void*] Ensembl Slice or NULL
** @@
******************************************************************************/

static void* sliceAdaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *) ensSliceNewRef((EnsPSlice) value);
}




/* @funcstatic sliceAdaptorCacheDelete ****************************************
**
** Wrapper function to delete (or de-reference) an Ensembl Slice
** from an Ensembl Cache.
**
** @param [u] value [void**] Ensembl Slice address
**
** @return [void]
** @@
** When deleting from the Cache, this function also removes and deletes the
** Ensembl Slice from the name cache, which is based on a
** conventional AJAX Table.
******************************************************************************/

static void sliceAdaptorCacheDelete(void **value)
{
    ajint start  = 0;
    ajint end    = 0;
    ajint strand = 0;
    
    AjPStr newkey = NULL;
    AjPStr oldkey = NULL;
    
    EnsPCoordSystem cs = NULL;
    
    EnsPSlice oldslice = NULL;
    EnsPSlice *Pslice  = NULL;
    
    if(!value)
	return;
    
    /*
    ** Synchronise the deletion of this Sequence Region from the
    ** identifier cache, which is based on an Ensembl (LRU) Cache,
    ** with the name cache, based on a conventional AJAX Table,
    ** both in the Sequence Adaptor.
    */
    
    Pslice = (EnsPSlice *) value;
    
    if(!*Pslice)
	return;
    
    if((*Pslice)->Adaptor && (*Pslice)->Adaptor->CacheByName)
    {
	cs = ensSeqRegionGetCoordSystem((*Pslice)->SeqRegion);
	
	start = (*Pslice)->Start;
	
	end = (*Pslice)->End;
	
	strand = (*Pslice)->Strand;
	
	if((start == 1) &&
	    (end == ensSeqRegionGetLength((*Pslice)->SeqRegion)) &&
	    (strand == 1))
	{
	    start = 0;
	    
	    end = 0;
	    
	    strand = 0;
	}
	
	/* Remove from the name cache. */
	
	newkey = ajFmtStr("%S:%S:%S:%d:%d:%d",
			  ensCoordSystemGetName(cs),
			  ensCoordSystemGetVersion(cs),
			  ensSeqRegionGetName((*Pslice)->SeqRegion),
			  start, end, strand);
	
	oldslice = (EnsPSlice)
	    ajTableRemoveKey((*Pslice)->Adaptor->CacheByName,
			     (const void *) newkey,
			     (void **) &oldkey);
	
	/* Delete the old and new key data. */
	
	ajStrDel(&oldkey);
	
	ajStrDel(&newkey);
	
	/* Delete the value data. */
	
	ensSliceDel(&oldslice);
    }
    
    ensSliceDel(Pslice);
    
    return;
}




/* @funcstatic sliceAdaptorCacheSize ******************************************
**
** Wrapper function to determine the memory size of an Ensembl Slice
** from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Slice
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint sliceAdaptorCacheSize(const void* value)
{
    if(!value)
	return 0;
    
    return ensSliceGetMemSize((const EnsPSlice) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Slice Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Slice Adaptor. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPSliceAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @argrule Ref object [EnsPSliceAdaptor] Ensembl Slice Adaptor
**
** @valrule * [EnsPSliceAdaptor] Ensembl Slice Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensSliceAdaptorNew ***************************************************
**
** Default constructor for an Ensembl Slice Adaptor.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPSliceAdaptor] Ensembl Slice Adaptor or NULL
** @@
******************************************************************************/

EnsPSliceAdaptor ensSliceAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPSliceAdaptor adaptor = NULL;
    
    if(!dba)
        return NULL;
    
    AJNEW0(adaptor);
    
    adaptor->Adaptor = dba;
    
    adaptor->CacheByIdentifier =
	ensCacheNew(ensECacheTypeAlphaNumeric,
		    sliceAdaptorCacheMaxBytes,
		    sliceAdaptorCacheMaxCount,
		    sliceAdaptorCacheMaxSize,
		    sliceAdaptorCacheReference,
		    sliceAdaptorCacheDelete,
		    sliceAdaptorCacheSize,
		    (void* (*)(const void* key)) NULL, /* Fread */
		    (AjBool (*)(const void* value)) NULL, /* Fwrite */
		    ajFalse,
		    "Slice");
    
    adaptor->CacheByName = ajTablestrNewLen(0);
    
    return adaptor;
}




/* @func ensSliceAdaptorDel ***************************************************
**
** Default destructor for an Ensembl Slice Adaptor.
** This function also clears the internal Sequence Region caches.
**
** @param [r] Padaptor [EnsPSliceAdaptor*] Ensembl Slice Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensSliceAdaptorDel(EnsPSliceAdaptor *Padaptor)
{
    EnsPSliceAdaptor pthis = NULL;
    
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;

    pthis = *Padaptor;
    
    /*
     ajDebug("ensSliceAdaptorDel\n"
	     "  *Padaptor %p\n",
	     *Padaptor);
     */
    
    /*
     ** Clear the identifier cache, which is based on an Ensembl LRU Cache.
     ** Clearing the Ensembl LRU Cache automatically clears the name cache
     ** via sliceAdaptorCacheDelete.
     */
    
    ensCacheDel(&pthis->CacheByIdentifier);
    
    ajTableFree(&pthis->CacheByName);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @func ensSliceAdaptorGetDatabaseAdaptor ************************************
**
** Get the Ensembl Database Adaptor element of an Ensembl Slice Adaptor.
**
** @param [r] adaptor [const EnsPSliceAdaptor] Ensembl Slice Adaptor
**
** @return [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

EnsPDatabaseAdaptor ensSliceAdaptorGetDatabaseAdaptor(
    const EnsPSliceAdaptor adaptor)
{
    if(!adaptor)
        return NULL;
    
    return adaptor->Adaptor;
}




/* @func ensSliceAdaptorCacheInsert *******************************************
**
** Insert an Ensembl Slice into the Slice Adaptor-internal cache.
**
** @param [u] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [r] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceAdaptorCacheInsert(EnsPSliceAdaptor adaptor, EnsPSlice* Pslice)
{
    ajint start  = 0;
    ajint end    = 0;
    ajint strand = 0;
    
    AjPStr ikey = NULL;
    AjPStr nkey = NULL;
    
    EnsPCoordSystem cs = NULL;
    
    EnsPSlice slice1 = NULL;
    EnsPSlice slice2 = NULL;
    
    /*
     ajDebug("ensSliceAdaptorCacheInsert\n"
	     "  adaptor %p\n"
	     "  *Pslice %p\n",
	     adaptor,
	     *Pslice);
     
     ensSliceTrace(*Pslice, 1);
     */
    
    if(!adaptor)
	return ajFalse;
    
    if(!Pslice)
	return ajFalse;
    
    if(!*Pslice)
	return ajFalse;
    
    cs = ensSeqRegionGetCoordSystem((*Pslice)->SeqRegion);
    
    start = (*Pslice)->Start;
    
    end = (*Pslice)->End;
    
    strand = (*Pslice)->Strand;
    
    /* For Slices that cover entire Sequence Regions zero all coordinates. */
    
    if((start == 1) &&
	(end == ensSeqRegionGetLength((*Pslice)->SeqRegion)) &&
	(strand == 1))
    {
	start = 0;
	
	end = 0;
	
	strand = 0;
    }
    
    /* Search the identifier cache. */
    
    ikey = ajFmtStr("%u:%d:%d:%d",
		    ensSeqRegionGetIdentifier((*Pslice)->SeqRegion),
		    start, end, strand);
    
    slice1 = (EnsPSlice)
	ensCacheFetch(adaptor->CacheByIdentifier, (void *) ikey);
    
    /* Search the name cache. */
    
    nkey = ajFmtStr("%S:%S:%S:%d:%d:%d",
		    ensCoordSystemGetName(cs),
		    ensCoordSystemGetVersion(cs),
		    ensSeqRegionGetName((*Pslice)->SeqRegion),
		    start, end, strand);
    
    slice2 = (EnsPSlice)
	ajTableFetch(adaptor->CacheByName, (const void *) nkey);
    
    if((!slice1) && (! slice2))
    {
	/*
	** None of the caches returned an identical Ensembl Slice so add this
	** one to both caches. The Ensembl LRU Cache automatically references
	** the Ensembl Slice via the sliceAdaptorCacheReference function,
	** while the AJAX Table-based cache needs manual referencing.
	*/
	
	ensCacheStore(adaptor->CacheByIdentifier,
		      (void *) ikey,
		      (void **) Pslice);
	
	ajTablePut(adaptor->CacheByName,
		   (void *) ajStrNewS(nkey),
		   (void *) ensSliceNewRef(*Pslice));
    }
    
    if(slice1 && slice2 && (slice1 == slice2))
    {
	/*
	** Both caches returned the same Ensembl Slice so delete it and
	** return a pointer to the one already in the cache.
	*/
	
	ensSliceDel(Pslice);
	
	*Pslice = ensSliceNewRef(slice2);
    }
    
    if(slice1 && slice2 && (slice1 != slice2))
	ajDebug("ensSliceAdaptorCacheInsert detected Slices in the "
		"identifier '%S' and name '%S' cache with "
		"different addresses (%p and %p).\n",
		ikey, nkey, slice1, slice2);
    
    if(slice1 && (!slice2))
	ajDebug("ensSliceAdaptorCacheInsert detected a Slice in "
		"the identifier, but not in the name cache.\n");
    
    if((!slice1) && slice2)
	ajDebug("ensSliceAdaptorCacheInsert detected a Slice in "
		"the name, but not in the identifier cache.\n");
    
    ensSliceDel(&slice1);
    
    ajStrDel(&ikey);
    ajStrDel(&nkey);
    
    return ajTrue;
}




/* @func ensSliceAdaptorFetchBySeqRegionIdentifier ****************************
**
** Fetch an Ensembl Slice by an Ensembl Sequence Region identifier.
** The caller is responsible for deleting the Ensembl Slice.
**
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [r] srid [ajuint] Ensembl Sequence Region identifier
** @param [r] srstart [ajint] Start coordinate
** @param [r] srend [ajint] End coordinate
** @param [r] srstrand [ajint] Strand information
** @param [wP] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceAdaptorFetchBySeqRegionIdentifier(EnsPSliceAdaptor adaptor,
                                                 ajuint srid,
                                                 ajint srstart,
                                                 ajint srend,
                                                 ajint srstrand,
                                                 EnsPSlice *Pslice)
{
    AjPStr key = NULL;
    
    EnsPSeqRegion sr         = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    /*
     ajDebug("ensSliceAdaptorFetchBySeqRegionIdentifier\n"
	     "  adaptor %p\n"
	     "  srid %u\n"
	     "  srstart %d\n"
	     "  srend %d\n"
	     "  srstrand %d\n"
	     "  Pslice %p\n",
	     adaptor,
	     srid,
	     srstart,
	     srend,
	     srstrand,
	     Pslice);
     */
    
    if(!adaptor)
    {
	ajDebug("ensSliceAdaptorFetchBySeqRegionIdentifier requires an "
		"Ensembl Slice Adaptor.\n");
	
	return ajFalse;
    }
    
    if(!srid)
    {
	ajDebug("ensSliceAdaptorFetchBySeqRegionIdentifier requires an "
		"Ensembl Sequence Region identifier.\n");
	
	return ajFalse;
    }
    
    if(!Pslice)
	return ajFalse;
    
    /* Check the cache first. */
    
    key = ajFmtStr("%u:%d:%d:%d", srid, srstart, srend, srstrand);
    
    *Pslice = (EnsPSlice)
	ensCacheFetch(adaptor->CacheByIdentifier, (void *) key);
    
    ajStrDel(&key);
    
    if(*Pslice)
	return ajTrue;
    
    sra = ensRegistryGetSeqRegionAdaptor(adaptor->Adaptor);
    
    ensSeqRegionAdaptorFetchByIdentifier(sra, srid, &sr);
    
    if(!sr)
    {
	ajDebug("ensSliceAdaptorFetchBySeqRegionIdentifier could not load an "
		"Ensembl Sequence Region for identifier %d.\n", srid);
	
	return ajTrue;
    }
    
    if(!srstart)
	srstart = 1;
    
    if(!srend)
	srend = ensSeqRegionGetLength(sr);
    
    if(srstart > (srend + 1))
	ajFatal("ensSliceAdaptorFetchBySeqRegionIdentifier requires the Slice "
		"start %d to be less than or equal to the end %d + 1.",
		srstart, srend);
    
    if(!srstrand)
	srstrand = 1;
    
    *Pslice = ensSliceNew(adaptor, sr, srstart, srend, srstrand);
    
    ensSliceAdaptorCacheInsert(adaptor, Pslice);
    
    ensSeqRegionDel(&sr);
    
    return ajTrue;
}




/* @func ensSliceAdaptorFetchByRegion *****************************************
**
** Fetch an Ensembl Slice by region information. At a minimum the name
** of the Ensembl Sequence Region must be provided.
** If no Coordinate System name is provided then a Slice on the highest ranked
** Coordinate System with a matching Sequence Region name will be returned.
** The caller is responsible for deleting the Ensembl Slice.
**
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [rN] csname [const AjPStr] Ensembl CoordinateSystem name
** @param [rN] csversion [const AjPStr] Ensembl CoordinateSystem version
** @param [r] srname [const AjPStr] Ensembl Sequence Region name
** @param [r] srstart [ajint] Start coordinate
** @param [r] srend [ajint] End coordinate
** @param [r] srstrand [ajint] Strand information
** @param [wP] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
**
** Some fuzzy matching is performed if no exact match for the provided
** Sequence Region name is found. This allows clones to be fetched even when
** their sequence version is not known.
** For example ensSliceAdaptorFetchByRegion(..., 'clone', 'AC008066', ...)
** will retrieve the Sequence Region with name 'AC008066.4'.
**
** The fuzzy matching can be turned off by setting the $no_fuzz argument to a
** true value. FIXME: Not implemented!
******************************************************************************/

AjBool ensSliceAdaptorFetchByRegion(EnsPSliceAdaptor adaptor,
                                    const AjPStr csname,
                                    const AjPStr csversion,
                                    const AjPStr srname,
                                    ajint srstart,
                                    ajint srend,
                                    ajint srstrand,
                                    EnsPSlice *Pslice)
{
    AjPStr key = NULL;
    
    EnsPCoordSystem cs         = NULL;
    EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPSeqRegion sr         = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    /*
     ajDebug("ensSliceAdaptorFetchByRegion\n"
	     "  adaptor %p\n"
	     "  csname '%S'\n"
	     "  csversion '%S'\n"
	     "  srname '%S'\n"
	     "  srstart %d\n"
	     "  srend %d\n"
	     "  srstrand %d\n"
	     "  Pslice %p\n",
	     adaptor,
	     csname,
	     csversion,
	     srname,
	     srstart,
	     srend,
	     srstrand,
	     Pslice);
     */
    
    if(!adaptor)
    {
	ajDebug("ensSliceAdaptorFetchByRegion requires an "
		"Ensembl Slice Adaptor.\n");
	
	return ajFalse;
    }
    
    if((!srname) || (! ajStrGetLen(srname)))
    {
	ajDebug("ensSliceAdaptorFetchByRegion requires an "
		"Ensembl Sequence Region name.\n");
	
	return ajFalse;
    }
    
    if(!Pslice)
	return ajFalse;
    
    /*
    ** Initially, search the cache, which can only return an Ensembl Slice,
    ** which is associated with a name and version of a regular Ensembl
    ** Coordinate System. For requests specifying the top-level Coordinate
    ** System or no Coordinate System at all the database needs to be queried
    ** for the Sequence Region associated with the Coordinate System of the
    ** highest rank. However, all Slices will be inserted into the cache with
    ** their true Coordinate System, keeping at least the memory requirements
    ** minimal.
    **
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */
    
    key = ajFmtStr("%S:%S:%S:%d:%d:%d",
		   csname,
		   csversion,
		   srname,
		   srstart,
		   srend,
		   srstrand);
    
    *Pslice = (EnsPSlice)
	ajTableFetch(adaptor->CacheByName, (const void *) key);
    
    ajStrDel(&key);
    
    if(*Pslice)
    {
	ensSliceNewRef(*Pslice);
	
	return ajTrue;
    }
    
    /* Load the Ensembl Coordinate System if a name has been provided. */
    
    if(csname && ajStrGetLen(csname))
    {
	csa = ensRegistryGetCoordSystemAdaptor(adaptor->Adaptor);
	
	ensCoordSystemAdaptorFetchByName(csa, csname, csversion, &cs);
	
	if(!cs)
	{
	    ajDebug("ensSliceAdaptorFetchByRegion could not load an Ensembl "
		    "Coordinate System for name '%S' and version '%S'.\n",
		    csname, csversion);
	    
	    return ajTrue;
	}
    }
    
    /* Load the Ensembl Sequence Region. */
    
    sra = ensRegistryGetSeqRegionAdaptor(adaptor->Adaptor);
    
    ensSeqRegionAdaptorFetchByName(sra, cs, srname, &sr);
    
    if(!sr)
    {
	ajDebug("ensSliceAdaptorFetchByRegion could not load an Ensembl "
		"Sequence Region for name '%S' and Coordinate System "
		"identifier %d.\n", srname, ensCoordSystemGetIdentifier(cs));
	
	ensCoordSystemTrace(cs, 1);
	
	ensCoordSystemDel(&cs);
	
	return ajTrue;
    }
    
    if(!srstart)
	srstart = 1;
    
    if(!srend)
	srend = ensSeqRegionGetLength(sr);
    
    if(srstart > (srend + 1))
	ajFatal("ensSliceAdaptorFetchByRegion requires the Slice start %d "
		"to be less than or equal to the end coordinate %d + 1.",
		srstart, srend);
    
    if(!srstrand)
	srstrand = 1;
    
    *Pslice = ensSliceNew(adaptor, sr, srstart, srend, srstrand);
    
    ensSliceAdaptorCacheInsert(adaptor, Pslice);
    
    ensCoordSystemDel(&cs);
    
    ensSeqRegionDel(&sr);
    
    return ajTrue;
}




/* @func ensSliceAdaptorFetchByName *******************************************
**
** Fetch an Ensembl Slice by name.
** The caller is responsible for deleting the Ensembl Slice.
**
** An Ensembl Slice name consists of the following fields:
** csname:csversion:srname:srstart:srend:srstrand
**
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [r] name [const AjPStr] Ensembl Slice name
** @param [wP] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceAdaptorFetchByName(EnsPSliceAdaptor adaptor,
                                  const AjPStr name,
                                  EnsPSlice *Pslice)
{
    ajint srstart  = 0;
    ajint srend    = 0;
    ajint srstrand = 0;
    
    AjPStr csname    = NULL;
    AjPStr csversion = NULL;
    AjPStr srname    = NULL;
    AjPStr tmpstr    = NULL;
    
    AjPStrTok token = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!ajStrGetLen(name))
	return ajFalse;
    
    if(!Pslice)
	return ajFalse;
    
    token = ajStrTokenNewC(name, ":");
    
    /*
    ** FIXME: This does not work for slice names like the following:
    ** "clone::AC120349.5:0:0:0".
    ** Change from ajStrTokenNextParse to ajStrTokenNextFind, since the search
    ** string is a set of characters.
    ** Seems like there is a problem in the AJAX function. There should be a
    ** test for an empty string between two delimiters?
    ** For ajStrTokenNextParse, which uses the C runtime library strcspn, the
    ** secon string seems to be the set of characters. This does not match the
    ** documentation.
    ** For ajStrTokenNextFind the C runtime library function strstr is used,
    ** which seems to treat the second string as a character string that needs
    ** to match. In this case there should be a check, if the length difference
    ** between two matches is 1, an empty string should be returned.
    **
    ** We should treat ':' as set of characters, hence use ajStrTokenNextParse!
    **
    ** FIXME: Maybe this could directly use the Slice name that was provided
    ** for the cache look up? This could save us the parsing and re-assigning
    ** steps.
    */
    
    ajStrTokenNextParse(&token, &csname);
    
    ajDebug("ensSliceAdaptorFetchByName got csname '%S'\n", csname);
    
    ajStrTokenNextParse(&token, &csversion);
    
    ajDebug("ensSliceAdaptorFetchByName got csversion '%S'\n", csversion);
    
    ajStrTokenNextParse(&token, &srname);
    
    ajDebug("ensSliceAdaptorFetchByName got srname '%S'\n", srname);
    
    ajStrTokenNextParse(&token, &tmpstr);
    
    ajDebug("ensSliceAdaptorFetchByName got srstart '%S'\n", tmpstr);
    
    ajStrToInt(tmpstr, &srstart);
    
    ajStrTokenNextParse(&token, &tmpstr);
    
    ajDebug("ensSliceAdaptorFetchByName got srend '%S'\n", tmpstr);
    
    ajStrToInt(tmpstr, &srend);
    
    if(ajStrTokenNextParse(&token, &tmpstr))
    {
	ajDebug("ensSliceAdaptorFetchByName got srstrand '%S'\n", tmpstr);
	
	ajStrToInt(tmpstr, &srstrand);
	
	ensSliceAdaptorFetchByRegion(adaptor,
				     csname,
				     csversion,
				     srname,
				     srstart,
				     srend,
				     srstrand,
				     Pslice);
    }
    else
	ajDebug("ensSliceAdaptorFetchByName got malformed Ensembl Slice name "
		"'%S', should be something like "
		"chromosome:NCBI36:X:1000000:2000000:1\n", name);
    
    ajStrDel(&csname);
    ajStrDel(&csversion);
    ajStrDel(&srname);
    ajStrDel(&tmpstr);
    
    ajStrTokenDel(&token);
    
    return ajTrue;
}




/* @func ensSliceAdaptorFetchBySlice ******************************************
**
** Fetch an Ensembl Slice by an Ensembl Slice in relative coordinates.
**
** This function is experimental, but the advantage of going through the
** Slice Adaptor would be that a (Sub-)Slice would be registered in the
** Slice Adaptor-internal cache, which in turn reduces memory consumption.
** The caller is responsible for deleting the Ensembl Slice.
**
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [wP] Pslice [EnsPSlice*] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceAdaptorFetchBySlice(EnsPSliceAdaptor adaptor,
                                   EnsPSlice slice,
                                   ajint start,
                                   ajint end,
                                   ajint strand,
                                   EnsPSlice *Pslice)
{
    ajuint srid    = 0;
    ajint srstart  = 0;
    ajint srend    = 0;
    ajint srstrand = 0;
    
    if(!adaptor)
	return ajFalse;
    
    if(!slice)
	return ajFalse;
    
    if(start > (end + 1))
    {
	ajDebug("ensSliceAdaptorFetchBySlice requires the start coordinate %d "
		"to be less than the end coordinate %d + 1.\n",
		start, end);
	
	return ajFalse;
    }
    
    if(!strand)
	strand = 1;
    
    if(!Pslice)
	return ajFalse;
    
    /* Transform relative into absolute Sequence Region coordinates. */
    
    /*
     **
     **           s           e
     **     SS     \    +1   /     SE
     **  1    \     |-------|     /    length
     **   \    |------Slice------|    /
     **    |--------SeqRegion--------|
     **        |------Slice------|
     **       /     |-------|     \
     **     SS     /    -1   \     SE
     **           e           s
     **
     */
    
    srid = ensSeqRegionGetIdentifier(slice->SeqRegion);
    
    if(slice->Strand >= 0)
    {
	srstart = slice->Start + start - 1;
	
	srend = slice->Start + end - 1;
    }
    else
    {
	srstart = slice->End - end + 1;
	
	srend = slice->End - start + 1;
    }
    
    srstrand = slice->Strand * strand;
    
    ensSliceAdaptorFetchBySeqRegionIdentifier(adaptor,
					      srid,
					      srstart,
					      srend,
					      srstrand,
					      Pslice);
    
    if(!*Pslice)
	return ajFalse;
    
    /* Adjust the sequence in case the Slice has one associated. */
    
    if(slice->Sequence)
	ensSliceFetchSubSequenceStr(slice,
				    start,
				    end,
				    strand,
				    &((*Pslice)->Sequence));
    
    return ajTrue;
}




/* @func ensSliceAdaptorFetchByFeature ****************************************
**
** Retrieve an Ensembl Slice around an Ensembl Feature.
**
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [r] feature [const EnsPFeature] Ensembl Feature
** @param [r] flank [ajint] Flanking region in base pair coordinates
** @param [wP] Pslice [EnsPSlice] Ensembl Slice address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** All this really does is return a resized version of the Slice that the
** Feature is already on. Note that Slices returned from this function are
** always on the forward strand of the Sequence Region regardless of the
** strandedness of the Feature passed in.
******************************************************************************/

AjBool ensSliceAdaptorFetchByFeature(EnsPSliceAdaptor adaptor,
                                     const EnsPFeature feature,
                                     ajint flank,
                                     EnsPSlice *Pslice)
{
    ajuint srid   = 0;
    ajint srstart = 0;
    ajint srend   = 0;
    
    EnsPSlice slice = NULL;
    
    /*
     ajDebug("ensSliceAdaptorFetchByFeature\n"
	     "  adaptor %p\n"
	     "  feature %p\n"
	     "  flank %d\n"
	     "  Pslice %p\n",
	     adaptor,
	     feature,
	     flank,
	     Pslice);
     
     ensFeatureTrace(feature, 1);
     */
    
    if(!adaptor)
    {
	ajDebug("ensSliceAdaptorFetchByFeature requires an "
		"Ensembl Slice Adaptor.\n");
	
	return ajFalse;
    }
    
    if(!feature)
    {
	ajDebug("ensSliceAdaptorFetchByFeature requires an "
		"Ensembl Feature.\n");
	
	return ajFalse;
    }
    
    if(!Pslice)
	return ajFalse;
    
    slice = ensFeatureGetSlice(feature);
    
    if(!slice)
    {
	ajDebug("ensSliceAdaptorFetchByFeature requires an "
		"Ensembl Slice attached to the Ensembl Feature.\n");
	
	return ajFalse;
    }
    
    srid = ensSliceGetSeqRegionIdentifier(slice);
    
    /* Convert the Feature Slice coordinates to Sequence Region coordinates. */
    
    if(slice->Strand >= 0)
    {
	srstart = slice->Start + ensFeatureGetStart(feature) - 1;
	
	srend = slice->Start + ensFeatureGetEnd(feature) - 1;
    }
    else
    {
	srstart = slice->End - ensFeatureGetEnd(feature) + 1;
	
	srend = slice->End - ensFeatureGetStart(feature) + 1;
    }
    
    ensSliceAdaptorFetchBySeqRegionIdentifier(adaptor,
					      srid,
					      srstart - flank,
					      srend + flank,
					      1,
					      Pslice);
    
    return ajTrue;
}




/* @func ensSliceAdaptorFetchNormalisedSliceProjection ************************
**
** Fetch a normalised projection for an Ensembl Slice.
** This function returns an AJAX List of Ensembl Projection Segments where
** symbolic links exist for the given Ensembl Slice.
** The caller is resposible for deleting the Ensembl Projection Segments before
** deleting the AJAX List.
**
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] pslist [AjPList] AJAX List of Ensembl Projection Segments
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensSliceAdaptorFetchNormalisedSliceProjection(EnsPSliceAdaptor adaptor,
                                                     EnsPSlice slice,
                                                     AjPList pslist)
{
    ajuint srid   = 0;
    ajuint srlen  = 0;
    ajuint exlen  = 0;
    ajuint maxlen = 0;
    ajuint diff   = 0;
    ajuint start  = 0;
    
    AjEnum aetype = ensEAssemblyExceptionTypeNULL;
    
    AjPList haps    = NULL;
    AjPList pars    = NULL;
    AjPList syms    = NULL;
    AjPList regions = NULL;
    AjPList linked  = NULL;
    
    AjPStr source = NULL;
    AjPStr target = NULL;
    
    EnsPAssemblyException ae         = NULL;
    EnsPAssemblyException tmpae      = NULL;
    EnsPAssemblyExceptionAdaptor aea = NULL;
    
    EnsPMapper mapper = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPProjectionSegment ps = NULL;
    
    EnsPSlice srslice = NULL;
    EnsPSlice exslice = NULL;
    
    /*
     ajDebug("ensSliceAdaptorFetchNormalisedSliceProjection\n"
	     "  adaptor %p\n"
	     "  slice %p\n"
	     "  pslist %p\n",
	     adaptor,
	     slice,
	     pslist);
     */
    
    if(!adaptor)
    {
	ajDebug("ensSliceAdaptorFetchNormalisedSliceProjection requires an "
		"Ensembl Slice Adaptor.\n");
	
	return ajFalse;
    }
    
    if(!slice)
    {
	ajDebug("ensSliceAdaptorFetchNormalisedSliceProjection requires an "
		"Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    if(!pslist)
    {
	ajDebug("ensSliceAdaptorFetchNormalisedSliceProjection requires an "
		"AJAX List.\n");
	
	return ajFalse;
    }
    
    aea = ensRegistryGetAssemblyExceptionAdaptor(adaptor->Adaptor);
    
    srid = ensSliceGetSeqRegionIdentifier(slice);
    
    haps = ajListNew();
    
    pars = ajListNew();
    
    regions = ajListNew();
    
    ensAssemblyExceptionAdaptorFetchAllBySeqRegionIdentifier(aea, srid,
                                                             regions);
    
    while(ajListPop(regions, (void **) &ae))
    {
	/*
	** We need all overlapping pseudo-autosomal regions (PARs) and all
	** haplotype regions (HAPs) if any.
	*/
	
	if(ensAssemblyExceptionGetType(ae) == ensEAssemblyExceptionTypePAR)
	{
	    if((ensAssemblyExceptionGetSeqRegionStart(ae) <= slice->End) &&
		(ensAssemblyExceptionGetSeqRegionEnd(ae) >= slice->Start))
		ajListPushAppend(pars, (void *) ae);
	    else
		ensAssemblyExceptionDel(&ae);
	}
	else if(ensAssemblyExceptionGetType(ae) ==
		 ensEAssemblyExceptionTypeHAP)
	    ajListPushAppend(haps, (void *) ae);
	else
	    ajWarn("ensSliceAdaptorFetchNormalisedSliceProjection got "
		   "unexpected Assembly Exception type %d.\n",
		   ensAssemblyExceptionGetType(ae));
    }
    
    ajListFree(&regions);
    
    if((!ajListGetLength(haps)) && (! ajListGetLength(pars)))
    {
	/*
	** There are no haplotypes and no pseudo-autosomal regions,
	** so return just this slice.
	*/
	
	ajListFree(&haps);
	ajListFree(&pars);
	
	ps = ensProjectionSegmentNew(1, ensSliceGetLength(slice), slice);
	
	ajListPushAppend(pslist, (void *) ps);
	
	return ajTrue;
    }
    
    if(ajListGetLength(haps) > 1)
	ajFatal("ensSliceAdaptorFetchNormalisedSliceProjection does not "
		"support more than one haplotypes yet.");
    
    syms = ajListNew();
    
    if(ajListGetLength(haps) == 1)
    {
	ajListPop(haps, (void **) &ae);
	
	ensSliceAdaptorFetchBySeqRegionIdentifier(
            adaptor,
            srid,
            0,
            0,
            0,
            &srslice);
	
	ensSliceAdaptorFetchBySeqRegionIdentifier(
            adaptor,
            ensAssemblyExceptionGetExcRegionIdentifier(ae),
            0,
            0,
            0,
            &exslice);
	
	/*
	** The lengths of haplotype and reference sequences in the database
	** may be different. We want to use the maximum possible length for
	** the mapping between the two Coordinate Systems.
	*/
	
	srlen = ensSliceGetLength(srslice);
	
	exlen = ensSliceGetLength(exslice);
	
	maxlen = (srlen > exlen) ? srlen : exlen;
	
	/*
	** The inserted Sequence Region can differ in length, but mapped
	** sections need to be of same lengths.
	*/
	
	diff = ensAssemblyExceptionGetExcRegionEnd(ae) -
	    ensAssemblyExceptionGetSeqRegionEnd(ae);
	
	aetype = ensEAssemblyExceptionTypeNULL;
	
	/*
	** We want the region of the haplotype inverted, which means that we
	** want the two regions of the Slice that are not covered by the
	** haplotype as Projection Segments.
	**
	** Haplotype:                   *******
	** Slice:                 -------------------
	** Projection Segments:   ++++++       ++++++
	*/
	
	tmpae = ensAssemblyExceptionNew(
            (EnsPAssemblyExceptionAdaptor) NULL,
            0,
            srid,
            1,
            ensAssemblyExceptionGetSeqRegionStart(ae) - 1,
            ensAssemblyExceptionGetExcRegionIdentifier(ae),
            1,
            ensAssemblyExceptionGetExcRegionStart(ae) - 1,
            1,
            aetype);
	
	ajListPushAppend(syms, (void *) tmpae);
	
	tmpae = ensAssemblyExceptionNew(
            (EnsPAssemblyExceptionAdaptor) NULL,
            0,
            srid,
            ensAssemblyExceptionGetSeqRegionEnd(ae) + 1,
            maxlen - diff,
            ensAssemblyExceptionGetExcRegionIdentifier(ae),
            ensAssemblyExceptionGetExcRegionEnd(ae) + 1,
            maxlen,
            1,
            aetype);
	
	ajListPushAppend(syms, (void *) tmpae);
	
	ensSliceDel(&srslice);
	ensSliceDel(&exslice);
	
	ensAssemblyExceptionDel(&ae);
    }
    
    ajListFree(&haps);
    
    /*
    ** The ajListPushlist function adds all nodes from the second list to
    ** the first and deletes the second list.
    */
    
    ajListPushlist(syms, &pars);
    
    source = ajStrNewC("sym");
    
    target = ajStrNewC("org");
    
    /*
    ** FIXME: The Perl API does not set Coordinate Systems for the Mapper,
    ** which leads to problems constructing a Mapper Result from the
    ** ensMapperMapCoordinates function.
    ** Experimentally, we set the Coordinate System of the Sequence Region
    ** element of the current Slice two times for source and target.
    ** The line was previously:
    ** mapper = ensMapperNew(source, target, (EnsPCoordSystem) NULL,
    **                       (EnsPCoordSystem) NULL);
    */
    
    mapper = ensMapperNew(source,
			  target,
			  ensSeqRegionGetCoordSystem(slice->SeqRegion),
			  ensSeqRegionGetCoordSystem(slice->SeqRegion));
    
    while(ajListPop(syms, (void **) &ae))
    {
	ensMapperAddCoordinates(mapper,
				srid,
				ensAssemblyExceptionGetSeqRegionStart(ae),
				ensAssemblyExceptionGetSeqRegionEnd(ae),
				1,
				ensAssemblyExceptionGetExcRegionIdentifier(ae),
				ensAssemblyExceptionGetExcRegionStart(ae),
				ensAssemblyExceptionGetExcRegionEnd(ae));
	
	ensAssemblyExceptionDel(&ae);
    }
    
    ajListFree(&syms);
    
    linked = ajListNew();
    
    ensMapperMapCoordinates(mapper,
			    srid,
			    slice->Start,
			    slice->End,
			    slice->Strand,
			    source,
			    linked);
    
    ajStrDel(&source);
    ajStrDel(&target);
    
    if(ajListGetLength(linked) == 1)
    {
	/*
	** If there was only one Mapper Result and it is a gap, we know it is
	** just the same Slice with no overlapping sym-links.
	*/
	
	ajListPop(linked, (void **) &mr);
	
	if(ensMapperResultGetType(mr) == ensEMapperResultGap)
	{
	    ps = ensProjectionSegmentNew(1, ensSliceGetLength(slice), slice);
	    
	    ajListPushAppend(pslist, (void *) ps);
	}
	
	ensMapperResultDel(&mr);
	
	ajListFree(&linked);
	
	ensMapperDel(&mapper);
	
	return ajTrue;
    }
    
    start = 1;
    
    while(ajListPop(linked, (void **) &mr))
    {
	if(ensMapperResultGetType(mr) == ensEMapperResultGap)
	{
	    exslice = ensSliceNew(adaptor,
				  slice->SeqRegion,
				  ensMapperResultGetGapStart(mr),
				  ensMapperResultGetGapEnd(mr),
				  slice->Strand);
	    
	    ps = ensProjectionSegmentNew(start,
					 ensMapperResultGetGapLength(mr)
					 + start - 1,
					 exslice);
	    
	    ajListPushAppend(pslist, (void *) ps);
	    
	    ensSliceDel(&exslice);
	}
	
	else
	{
	    ensSliceAdaptorFetchBySeqRegionIdentifier(
                adaptor,
                ensMapperResultGetObjectIdentifier(mr),
                ensMapperResultGetStart(mr),
                ensMapperResultGetEnd(mr),
                ensMapperResultGetStrand(mr),
                &exslice);
	    
	    ps = ensProjectionSegmentNew(start,
					 ensMapperResultGetLength(mr)
					 + start - 1,
					 exslice);
	    
	    ajListPushAppend(pslist, (void *) ps);
	    
	    ensSliceDel(&exslice);
	}
	
	start += ensMapperResultGetLength(mr);
	
	/*
	** FIXME: The else case includes Mapper Result Coordinates and Indels.
	** Is this desired? As MapperResultGetLength returns either the
	** Coordinate or Gap Length, we could add the length to the start after
	** the else case. We can also replace ensMapperResultGetGapLength with
	** ensMapperResultGetLength for ensEMapperResultGap.
	**
	** FIXME: Should the Mapper Result also provide distinct
	** GetCoordinateStart
	** GetGapStart,
	** GetCoordinateEnd
	** GetGapEnd functions, as well as functions
	** GetStart and
	** GetEnd that return the coordinate for Coordinates and Indels and
	** Gaps for Gap?
        */
	
	ensMapperResultDel(&mr);
    }
    
    ajListFree(&linked);
    
    ensMapperDel(&mapper);
    
    return ajTrue;
}




/* @func ensSliceAdaptorFetchAll **********************************************
**
** Fetch all Ensembl Slices representing Sequence Regions of a given
** Coordinate System.
**
** @param [r] adaptor [EnsPSliceAdaptor] Ensembl Slice Adaptor
** @param [r] csname [const AjPStr] Ensembl Coordinate System name
** @param [r] csversion [const AjPStr] Ensembl Coordinate System version
** @param [r] nonref [AjBool] Include non-reference Sequence Regions
** @param [r] duplicates [AjBool] Include duplicate Sequence Regions
** @param [u] slices [AjPList] An AJAX List of Ensembl Slices
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** The Coordinate System name may be the name of an actual Coordinate System
** or an alias like 'seqlevel' or 'toplevel'.
** If the Coordinate System name provided is 'toplevel', all non-redundant,
** top-level Slices are returned. The Coordinate System version will be
** ignored in that case.
** Non-reference Slices reflect alternative haplotype assemblies.
** Examples for human NCBI36 would be c5_H2, c6_COX, c6_QBL and c22_H2.
** If the duplicate option is not set and a pseudo-autosomal region (PAR) is
** at the beginning of the Sequence Region, then the resulting Slice will not
** start at position 1, so that coordinates retrieved from this Slice might
** not be what was expected.
******************************************************************************/

AjBool ensSliceAdaptorFetchAll(EnsPSliceAdaptor adaptor,
                               const AjPStr csname,
                               const AjPStr csversion,
                               AjBool nonref,
                               AjBool duplicates,
                               AjPList slices)
{
    register ajuint i = 0;
    
    ajuint srid = 0;
    
    void **keyarray = NULL;
    void **valarray = NULL;
    
    ajuint *Psrid  = NULL;
    
    AjBool *Pvalue = NULL;
    
    AjPList aelist = NULL;
    AjPList pslist = NULL;
    AjPList srlist = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr statement = NULL;
    
    AjPTable nonrefsr = NULL;
    
    EnsPAssemblyException ae         = NULL;
    EnsPAssemblyExceptionAdaptor aea = NULL;
    
    EnsPCoordSystem cs         = NULL;
    EnsPCoordSystemAdaptor csa = NULL;
    
    EnsPProjectionSegment ps = NULL;
    
    EnsPSeqRegion sr         = NULL;
    EnsPSeqRegionAdaptor sra = NULL;
    
    EnsPSlice slice  = NULL;
    EnsPSlice nslice = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!csname)
	return ajFalse;
    
    if(!csversion)
	return ajFalse;
    
    if(!slices)
	return ajFalse;
    
    if(!duplicates)
	aea = ensRegistryGetAssemblyExceptionAdaptor(adaptor->Adaptor);
    
    csa = ensRegistryGetCoordSystemAdaptor(adaptor->Adaptor);
    
    sra = ensRegistryGetSeqRegionAdaptor(adaptor->Adaptor);
    
    ensCoordSystemAdaptorFetchByName(csa, csname, csversion, &cs);
    
    if(!cs)
    {
	ajWarn("ensSliceAdaptorFetchAll could not retrieve Coordinate System "
	       "for name '%S' and version '%S'.\n", csname, csversion);
	
	return ajTrue;
    }
    
    /* Store non-reference Sequence Region identifiers in an AJAX Table. */
    
    nonrefsr = MENSTABLEUINTNEW(0);
    
    if(!nonref)
    {
	/*
	** FIXME: Should this be handled via the Sequence Region Adaptor?
	** A function like ensSeqRegionAdaptorFetchAllByAttribute?
	**
	** There could be a Cache in the SeqRegion Adaptor that holds all
	** non-ref Sequence Regions so that we can have a function
	** ensSeqRegionAdaptorFetchAllNonRef
	*/
	
	statement = ajFmtStr("SELECT "
			     "seq_region.seq_region_id "
			     "FROM "
			     "attrib_type, "
			     "seq_region_attrib, "
			     "seq_region, "
			     "coord_system "
			     "WHERE "
			     "attribute_type.code = 'non_ref' "
			     "AND "
			     "attrib_type.attrib_type_id = "
			     "seq_region_attrib.attrib_type_id"
			     "AND "
			     "seq_region_attrib.seq_region_id = "
			     "seq_region.seq_region_id "
			     "AND "
			     "seq_region.coord_system_id = "
			     "coord_system.coord_system_id "
			     "AND "
			     "coord_system.species_id = %u",
			     ensDatabaseAdaptorGetIdentifier(adaptor->Adaptor));
	
	sqls = ensDatabaseAdaptorSqlStatementNew(adaptor->Adaptor, statement);
	
	sqli = ajSqlRowIterNew(sqls);
	
	while(!ajSqlRowIterDone(sqli))
	{
	    AJNEW0(Psrid);
	    
	    AJNEW0(Pvalue);
	    
	    sqlr = ajSqlRowIterGet(sqli);
	    
	    ajSqlColumnToUint(sqlr, Psrid);
	    
	    *Pvalue = ajTrue;
	    
	    ajTablePut(nonrefsr, (void *) Psrid, (void *) Pvalue);
	}
	
	ajSqlRowIterDel(&sqli);
	
	ajSqlStatementDel(&sqls);
	
	ajStrDel(&statement);
    }
    
    /* Retrieve the Sequence Regions from the database. */
    
    srlist = ajListNew();
    
    ensSeqRegionAdaptorFetchAllByCoordSystem(sra, cs, srlist);
    
    while(ajListPop(srlist, (void **) &sr))
    {
	srid = ensSeqRegionGetIdentifier(sr);
	
	Pvalue = (AjBool *) ajTableFetch(nonrefsr, (const void *) &srid);
	
	if(!Pvalue)
	{
	    /* Create a new Slice spanning the entire Sequence Region. */
	    
	    slice = ensSliceNew(adaptor, sr, 1, ensSeqRegionGetLength(sr), 1);
	    
	    if(duplicates)
	    {
		/*
		** Duplicate regions are allowed, so no further checks are
		** required.
		*/
		
		ajListPushAppend(slices, (void *) slice);
	    }
	    else
	    {
		/*
		** Test if this Slice *could* have a duplicate region by
		** checking for an Assembly Exception for the
		** Ensembl Sequence Region element of this Ensembl Slice.
		*/
		
		aelist = ajListNew();
		
		ensAssemblyExceptionAdaptorFetchAllBySeqRegionIdentifier(
                    aea,
                    srid,
                    aelist);
		
		if(ajListGetLength(aelist))
		{
		    /*
		    ** This Slice may have duplicate regions, so de-reference
		    ** symlinked assembly regions and remove any Slices,
		    ** which have a symlink, because these are duplicates.
		    ** Replace them with any symlinked Slices based on the
		    ** same Sequence Region and Coordinate System as the
		    ** original Slice.
		    */
		    
		    pslist = ajListNew();
		    
		    ensSliceAdaptorFetchNormalisedSliceProjection(adaptor,
								  slice,
								  pslist);
		    
		    while(ajListPop(pslist, (void **) &ps))
		    {
			nslice = ensProjectionSegmentGetTrgSlice(ps);
			
			if(ensSeqRegionMatch(nslice->SeqRegion,
					      slice->SeqRegion))
			    ajListPushAppend(slices,
					     (void *) ensSliceNewRef(nslice));
			
			ensProjectionSegmentDel(&ps);
		    }
		    
		    ajListFree(&pslist);
		    
		    ensSliceDel(&slice);
		}
		else
		{
		    /* No duplicates for this Slice. */
		    
		    ajListPushAppend(slices, (void *) slice);		
		}
		
		while(ajListPop(aelist, (void **) &ae))
		    ensAssemblyExceptionDel(&ae);
		
		ajListFree(&aelist);
	    }
	}
	
	ensSeqRegionDel(&sr);
    }
    
    ajListFree(&srlist);
    
    ensCoordSystemDel(&cs);
    
    /* Clear the AJAX Table of non-reference Sequence Region identifiers. */
    
    ajTableToarray(nonrefsr, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	AJFREE(keyarray[i]);
	AJFREE(valarray[i]);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    ajTableFree(&nonrefsr);
    
    return ajTrue;
}




/* @datasection [EnsPRepeatMaskedSlice] Repeat-Masked Slice *******************
**
** Functions for manipulating Ensembl Repeat-Masked Slice objects
**
** @cc Bio::EnsEMBL::RepeatMaskedSlice CVS Revision: 1.12
**
** @nam2rule RepeatMaskedSlice
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Repeat-Masked Slice by pointer.
** It is the responsibility of the user to first destroy any previous
** Repeat-Masked Slice. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPRepeatMaskedSlice]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice
** @argrule Ref object [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice
**
** @valrule * [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice
**
** @fcategory new
******************************************************************************/




/* @func ensRepeatMaskedSliceNew **********************************************
**
** Default constructor for an Ensembl Repeat-Masked Slice.
**
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [r] annames [AjPList] AJAX List of Ensembl Analysis name
**                              AJAX Strings
** @param [r] masking [AjPTable] AJAX Table to override masking types
**
** @return [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice or NULL
** @@
******************************************************************************/

EnsPRepeatMaskedSlice ensRepeatMaskedSliceNew(EnsPSlice slice,
                                              AjPList annames,
                                              AjPTable masking)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjEnum *Pmsktyp = NULL;
    
    AjIList iter = NULL;
    
    AjPStr key = NULL;
    
    EnsPRepeatMaskedSlice rmslice = NULL;
    
    /*
     ajDebug("ensRepeatMaskedSliceNew\n"
	     "  slice %p\n"
	     "  annames %p\n"
	     "  masking %p\n",
	     slice,
	     annames,
	     masking);
     
     ensSliceTrace(slice, 1);
     */
    
    if(!slice)
	return NULL;
    
    if(!annames)
	return NULL;
    
    if(!masking)
	return NULL;
    
    AJNEW0(rmslice);
    
    rmslice->Slice = ensSliceNewRef(slice);
    
    /* Copy the AJAX List of Ensembl Analysis name AJAX Strings. */
    
    rmslice->AnalysisNames = ajListstrNew();
    
    iter = ajListIterNew(annames);
    
    while(!ajListIterDone(iter))
    {
	key = (AjPStr) ajListIterGet(iter);
	
	ajListPushAppend(rmslice->AnalysisNames, (void *) ajStrNewRef(key));
    }
    
    ajListIterDel(&iter);
    
    /* Copy the AJAX Table of AJAX String key and AJAX Enum masking types. */
    
    rmslice->Masking = ajTablestrNewLen(0);
    
    ajTableToarray(masking, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	key = ajStrNewS((AjPStr) keyarray[i]);
	
	AJNEW0(Pmsktyp);
	
	*Pmsktyp = *((AjEnum *) valarray[i]);
	
	ajTablePut(rmslice->Masking, (void *) key, (void *) Pmsktyp);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    rmslice->Use = 1;
    
    return rmslice;
}




/* @func ensRepeatMaskedSliceNewObj *******************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice
**
** @return [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice or NULL
** @@
******************************************************************************/

EnsPRepeatMaskedSlice ensRepeatMaskedSliceNewObj(EnsPRepeatMaskedSlice object)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjEnum *Pmsktyp = NULL;
    
    AjIList iter = NULL;
    
    AjPStr key = NULL;
    
    EnsPRepeatMaskedSlice rmslice = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(rmslice);
    
    rmslice->Slice = ensSliceNewRef(object->Slice);
    
    /* Copy the AJAX List of Ensembl Analysis name AJAX Strings. */
    
    if(object->AnalysisNames)
    {
	rmslice->AnalysisNames = ajListstrNew();
	
	iter = ajListIterNew(object->AnalysisNames);
	
	while(!ajListIterDone(iter))
	{
	    key = (AjPStr) ajListIterGet(iter);
	    
	    ajListPushAppend(rmslice->AnalysisNames, ajStrNewRef(key));
	}
	
	ajListIterDel(&iter);
    }
    
    /* Copy the AJAX Table of AJAX String key and AJAX Enum masking types. */
    
    rmslice->Masking = ajTablestrNewLen(0);
    
    ajTableToarray(object->Masking, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	key = ajStrNewS((AjPStr) keyarray[i]);
	
	AJNEW0(Pmsktyp);
	
	*Pmsktyp = *((AjEnum *) valarray[i]);
	
	ajTablePut(rmslice->Masking, (void *) key, (void *) Pmsktyp);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    rmslice->Use = 1;
    
    return rmslice;
}




/* @func ensRepeatMaskedSliceNewRef *******************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] rmslice [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice
**
** @return [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice or NULL
** @@
******************************************************************************/

EnsPRepeatMaskedSlice ensRepeatMaskedSliceNewRef(EnsPRepeatMaskedSlice rmslice)
{
    if(!rmslice)
	return NULL;
    
    rmslice->Use++;
    
    return rmslice;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Repeat-Masked Slice.
**
** @fdata [EnsPRepeatMaskedSlice]
** @fnote None
**
** @nam3rule Del Destroy (free) a Repeat-Masked Slice object
**
** @argrule * Prmslice [EnsPRepeatMaskedSlice*] Repeat-Masked Slice object
**                                              address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensRepeatMaskedSliceDel **********************************************
**
** Default Ensembl Repeat-Masked Slice destructor.
**
** @param [d] Prmslice [EnsPRepeatMaskedSlice*] Ensembl Repeat-Masked Slice
**                                              address
**
** @return [void]
** @@
******************************************************************************/

void ensRepeatMaskedSliceDel(EnsPRepeatMaskedSlice *Prmslice)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    EnsPRepeatMaskedSlice pthis = NULL;
    
    register ajuint i = 0;
    
    if(!Prmslice)
	return;
    
    if(!*Prmslice)
	return;

    pthis = *Prmslice;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Prmslice = NULL;
	
	return;
    }
    
    ensSliceDel(&pthis->Slice);
    
    /* Clear and delete the AJAX List of Ensembl Analysis name AJAX Strings. */
    
    ajListstrFreeData(&pthis->AnalysisNames);
    
    /* Clear and delete the AJAX Table. */
    
    ajTableToarray(pthis->Masking, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	ajTableRemove(pthis->Masking, (const void *) keyarray[i]);
	
	ajStrDel((AjPStr *) &keyarray[i]);
	
	AJFREE(valarray[i]);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    ajTableFree(&pthis->Masking);
    
    AJFREE(pthis);

    *Prmslice = NULL;
    
    return;
}




/* @func ensRepeatMaskedSliceFetchSequenceSeq *********************************
**
** Fetch the (masked) sequence of an Ensembl Repeat-Masked Slice as
** AJAX Sequence.
**
** @param [r] rmslice [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice
** @param [r] mtype [AjEnum] Repeat masking type, i.e. none, soft, hard
** @param [wP] Psequence [AjPSeq*] Sequence address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatMaskedSliceFetchSequenceSeq(EnsPRepeatMaskedSlice rmslice,
                                            AjEnum mtype,
                                            AjPSeq *Psequence)
{
    AjPStr name    = NULL;
    AjPStr sequence = NULL;
    
    if(!rmslice)
	return ajFalse;
    
    if(!Psequence)
	return ajFalse;
    
    name = ajStrNew();
    
    ensSliceFetchName(rmslice->Slice, &name);
    
    sequence = ajStrNew();
    
    ensRepeatMaskedSliceFetchSequenceStr(rmslice, mtype, &sequence);
    
    *Psequence = ajSeqNewNameS(sequence, name);
    
    ajStrDel(&sequence);
    ajStrDel(&name);
    
    return ajTrue;
}




/* @func ensRepeatMaskedSliceFetchSequenceStr *********************************
**
** Fetch the (masked) sequence for an Ensembl Repeat-Masked Slice as
** AJAX String.
**
** @cc Bio::EnsEMBL::RepeatMaskedSlice::seq
** @cc Bio::EnsEMBL::Slice::_mask_features
** @param [r] rmslice [EnsPRepeatMaskedSlice] Ensembl Repeat-Masked Slice
** @param [r] mtype [AjEnum] Repeat masking type, i.e. none, soft, hard
** @param [wP] Psequence [AjPStr*] Sequence String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: This function also incorporates the private
** Bio::EnsEMBL::Slice::_mask_features. Passing information between
** Slice.pm and RepeatMaskedSlice.pm seems rather inefficient and many
** parameter checks can be avoided if sequence and Repeat Features are
** fetched from the same Slice.
** FIXME: The private function however is not only used by
** Bio::EnsEMBL::RepeatMaskedSlice::seq, but also used by
** Bio::EnsEMBL::RepeatMaskedSlice::subseq.
** The old function is still available from the OldCode.c file.
*/

AjBool ensRepeatMaskedSliceFetchSequenceStr(EnsPRepeatMaskedSlice rmslice,
                                            AjEnum mtype,
                                            AjPStr *Psequence)
{
    ajint start = 0;
    ajint end   = 0;
    
    AjEnum msktyp   = 0;
    AjEnum *Pmsktyp = NULL;
    
    AjIList iter = NULL;
    AjPList rfs  = NULL;
    
    AjPStr key = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPRepeatConsensus rc = NULL;
    
    EnsPRepeatFeature rf         = NULL;
    EnsPRepeatFeatureAdaptor rfa = NULL;
    
    EnsPSliceAdaptor sa = NULL;
    
    /*
     ajDebug("ensRepeatMaskedSliceFetchSequenceStr\n"
	     "  rmslice %p\n"
	     "  *Psequence %p\n"
	     "  mtype %d\n",
	     rmslice,
	     *Psequence,
	     mtype);
     */
    
    if(!rmslice)
	return ajFalse;
    
    if(!Psequence)
	return ajFalse;
    
    if((mtype <= 0) || (mtype > ensERepeatMaskTypeHard))
    {
	ajDebug("ensRepeatMaskedSliceFetchSequenceStr got unsupported "
		"masking type %d\n", mtype);
	
	return ajFalse;
    }
    
    if(!rmslice->Slice)
    {
	ajDebug("ensRepeatMaskedSliceFetchSequenceStr got an "
		"Ensembl Repeat-Masked Slice without an "
		"Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    ensSliceFetchSequenceStr(rmslice->Slice, Psequence);
    
    /* Sanity check */
    
    if(ensSliceGetLength(rmslice->Slice) != ajStrGetLen(*Psequence))
	ajWarn("ensRepeatMaskedSliceFetchSequenceStr got Slice of length %u, "
	       "but DNA sequence of length %u.\n",
	       ensSliceGetLength(rmslice->Slice),
	       ajStrGetLen(*Psequence));
    
    sa = ensSliceGetAdaptor(rmslice->Slice);
    
    dba = ensSliceAdaptorGetDatabaseAdaptor(sa);
    
    rfa = ensRegistryGetRepeatFeatureAdaptor(dba);
    
    rfs = ajListNew();
    
    iter = ajListIterNew(rmslice->AnalysisNames);
    
    while(!ajListIterDone(iter))
    {
	key = (AjPStr) ajListIterGet(iter);
	
	ensRepeatFeatureAdaptorFetchAllBySlice(rfa,
					       rmslice->Slice,
					       key,
					       (AjPStr) NULL,
					       (AjPStr) NULL,
					       (AjPStr) NULL,
					       rfs);
    }
    
    ajListIterDel(&iter);
    
    /*
     sliceMaskRepeatFeatures(rmslice->Slice,
			     Psequence,
			     rfs,
			     mtype,
			     rmslice->Masking);
     */
    
    while(ajListPop(rfs, (void **) &rf))
    {
	feature = ensRepeatFeatureGetFeature(rf);
	
	start = ensFeatureGetStart(feature);
	
	end = ensFeatureGetEnd(feature);
	
	/*
	** Discard any Repeat Features that map completely outside the
	** expected range.
	*/
	
	/*
	** FIXME: This should no longer be required as Features are
        ** already on this Slice!
	 
	 if((start > dnalen) || (end < 1))
	 {
	     ajDebug("sliceMaskRepeatFeatures got "
		     "start %d > DNA sequence length %u or "
		     "end %d < 1.\n", start, dnalen, end);
	     
	     ensFeatureDel(&newfeature);
	     
	     continue;
	 }
	*/
	
	/*
	** For Repeat Features partly outside the Slice range correct the
	** Repeat Feature start or end and length to the Slice size if
	** neccessary.
	*/
	
	if(start < 1)
	    start = 1;
	
	if(end > ensSliceGetLength(rmslice->Slice))
	    end = ensSliceGetLength(rmslice->Slice);
	
	rc = ensRepeatFeatureGetRepeatConsensus(rf);
	
	/* Get the masking type for the Repeat Consensus Type if defined. */
	
	key = ajFmtStr("repeat_type_%S", ensRepeatConsensusGetType(rc));
	
	Pmsktyp = (AjEnum *)
	    ajTableFetch(rmslice->Masking, (const void *) key);
	
	if(Pmsktyp)
	    msktyp = *Pmsktyp;
	
	ajStrDel(&key);
	
	/* Get the masking type for the Repeat Consensus Class if defined. */
	
	key = ajFmtStr("repeat_class_%S", ensRepeatConsensusGetClass(rc));
	
	Pmsktyp = (AjEnum *)
	    ajTableFetch(rmslice->Masking, (const void *) key);
	
	if(Pmsktyp)
	    msktyp = *Pmsktyp;
	
	ajStrDel(&key);
	
	/* Get the masking type for the Repeat Consensus Name if defined. */
	
	key = ajFmtStr("repeat_name_%S", ensRepeatConsensusGetName(rc));
	
	Pmsktyp = (AjEnum *)
	    ajTableFetch(rmslice->Masking, (const void *) key);
	
	if(Pmsktyp)
	    msktyp = *Pmsktyp;
	
	ajStrDel(&key);
	
	/* Set the default masking type, if no other type has been defined. */
	
	if(!msktyp)
	    msktyp = mtype;
	
	/* Mask the DNA sequence. */
	
	if(msktyp == ensERepeatMaskTypeSoft)
	    ajStrFmtLowerSub(Psequence, start, end);
	else if(msktyp == ensERepeatMaskTypeHard)
	    ajStrMaskRange(Psequence, start, end, 'N');
	else
	    ajDebug("ensRepeatMaskedSliceFetchSequenceStr got unsupported "
		    "masking type %d\n", msktyp);
	
	ensRepeatFeatureDel(&rf);
    }
    
    ajListFree(&rfs);
    
    return ajTrue;
}

/*
** FIXME: Bio::EnsEMBL::RepeatMaskedSlice::subseq is missing.
** Currently, Features are not globally cached by the BaseFeatureAdaptor so
** that this function does not offer much benefit. Each sub-Slice would still
** require RepeatFeatures to be called.
*/

#if AJFALSE

AjBool ensRepeatMaskedSliceFetchSubSequenceStr(EnsPRepeatMaskedSlice rmslice,
                                               AjEnum mtype,
                                               AjPStr *Psequence)
{
    if(!rmslice)
	return ajFalse;
    
    if(!Psequence)
	return ajFalse;
    
    if((mtype <= 0) || (mtype > ensERepeatMaskTypeHard))
    {
	ajDebug("ensRepeatMaskedSliceFetchSubSequenceStr got unsupported "
		"masking type %d\n", mtype);
	
	return ajFalse;
    }
    
    if(!rmslice->Slice)
    {
	ajDebug("ensRepeatMaskedSliceFetchSubSequenceStr got an "
		"Ensembl Repeat-Masked Slice without an "
		"Ensembl Slice.\n");
	
	return ajFalse;
    }
    
    
}

#endif
