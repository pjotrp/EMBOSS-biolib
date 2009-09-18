/******************************************************************************
** @source Ensembl Mapper functions
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

#include "ensmapper.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static int mapperCompareMapperPairSources(const void* P1, const void* P2);

static int mapperCompareMapperPairTargets(const void* P1, const void* P2);

static AjBool mapperMergePairs(EnsPMapper mapper);

static AjBool mapperSort(EnsPMapper mapper);

static AjBool mapperMapInsert(EnsPMapper mapper,
                              ajuint oid,
                              ajint start,
                              ajint end,
                              ajint strand,
                              const AjPStr type,
                              AjBool fastmap,
                              AjPList mrs);




/* @filesection ensmapper *****************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/


/* @datasection [EnsPMapperUnit] Mapper Unit **********************************
**
** Functions for manipulating Ensembl Mapper Unit objects
**
** Bio::EnsEMBL::Mapper::Unit CVS Revision: 1.8
**
** @nam2rule MapperUnit
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Mapper Unit by pointer.
** It is the responsibility of the user to first destroy any previous
** Mapper Unit. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMapperUnit]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMapperUnit] Ensembl Mapper Unit
** @argrule Ref object [EnsPMapperUnit] Ensembl Mapper Unit
**
** @valrule * [EnsPMapperUnit] Ensembl Mapper Unit
**
** @fcategory new
******************************************************************************/




/* @func ensMapperUnitNew *****************************************************
**
** Default constructor for an Ensembl Mapper Unit.
**
** @cc Bio::EnsEMBL::Mapper::Unit::new
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
**
** @return [EnsPMapperUnit] Ensembl Mapper Unit or NULL
** @@
******************************************************************************/

EnsPMapperUnit ensMapperUnitNew(ajuint oid, ajint start, ajint end)
{
    EnsPMapperUnit mu = NULL;
    
    if(!oid)
        return NULL;
    
    AJNEW0(mu);
    
    mu->ObjectIdentifier = oid;
    
    mu->Start = start;
    mu->End   = end;
    mu->Use   = 1;
    
    return mu;
}




/* @func ensMapperUnitNewObj **************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMapperUnit] Ensembl Mapper Unit
**
** @return [EnsPMapperUnit] Ensembl Mapper Unit or NULL
** @@
******************************************************************************/

EnsPMapperUnit ensMapperUnitNewObj(const EnsPMapperUnit object)
{
    EnsPMapperUnit mu = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(mu);
    
    mu->ObjectIdentifier = object->ObjectIdentifier;
    
    mu->Start = object->Start;
    mu->End   = object->End;
    mu->Use   = 1;
    
    return mu;
}




/* @func ensMapperUnitNewRef **************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mu [EnsPMapperUnit] Ensembl Mapper Unit
**
** @return [EnsPMapperUnit] Ensembl Mapper Unit or NULL
** @@
******************************************************************************/

EnsPMapperUnit ensMapperUnitNewRef(EnsPMapperUnit mu)
{
    if(!mu)
	return NULL;
    
    mu->Use++;
    
    return mu;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Mapper Unit.
**
** @fdata [EnsPMapperUnit]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Mapper Unit object
**
** @argrule * Pmu [EnsPMapperUnit*] Ensembl Mapper Unit object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMapperUnitDel *****************************************************
**
** Default Ensembl Mapper Unit destructor.
**
** @param [d] Pmu [EnsPMapperUnit*] Ensembl Mapper Unit address
**
** @return [void]
** @@
******************************************************************************/

void ensMapperUnitDel(EnsPMapperUnit *Pmu)
{
    EnsPMapperUnit pthis = NULL;
    
    if(!Pmu)
        return;
    
    if(!*Pmu)
        return;

    pthis = *Pmu;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmu = NULL;
	
	return;
    }
    
    AJFREE(pthis);

    *Pmu = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Mapper Unit object.
**
** @fdata [EnsPMapperUnit]
** @fnote None
**
** @nam3rule Get Return Ensembl Mapper Unit attribute(s)
** @nam4rule GetObjectIdentifier Return the Ensembl Object identifier
** @nam4rule GetStart Return the start coordinate
** @nam4rule GetEnd Return the end coordinate
**
** @argrule * mu [const EnsPMapperUnit] Ensembl Mapper Unit
**
** @valrule ObjectIdentifier [ajuint] Ensembl Object identifier
** @valrule Start [ajint] Start coordinate
** @valrule End [ajint] End coordinate
**
** @fcategory use
******************************************************************************/




/* @func ensMapperUnitGetObjectIdentifier *************************************
**
** Get the Ensembl Object identifier element of an Ensembl Mapper Unit.
**
** @cc Bio::EnsEMBL::Mapper::Unit::id
** @param [r] mu [const EnsPMapperUnit] Ensembl Mapper Unit
**
** @return [ajuint] Ensembl Object identifier
** @@
******************************************************************************/

ajuint ensMapperUnitGetObjectIdentifier(const EnsPMapperUnit mu)
{
    if(!mu)
        return 0;
    
    return mu->ObjectIdentifier;
}




/* @func ensMapperUnitGetStart ************************************************
**
** Get the start coordinate element of an Ensembl Mapper Unit.
**
** @cc Bio::EnsEMBL::Mapper::Unit::start
** @param [r] mu [const EnsPMapperUnit] Ensembl Mapper Unit
**
** @return [ajint] Start coordinate
** @@
******************************************************************************/

ajint ensMapperUnitGetStart(const EnsPMapperUnit mu)
{
    if(!mu)
        return 0;
    
    return mu->Start;
}




/* @func ensMapperUnitGetEnd **************************************************
**
** Get the end coordinate element of an Ensembl Mapper Unit.
**
** @cc Bio::EnsEMBL::Mapper::Unit::end
** @param [r] mu [const EnsPMapperUnit] Ensembl Mapper Unit
**
** @return [ajint] End coordinate
** @@
******************************************************************************/

ajint ensMapperUnitGetEnd(const EnsPMapperUnit mu)
{
    if(!mu)
        return 0;
    
    return mu->End;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl Mapper Unit object.
**
** @fdata [EnsPMapperUnit]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl Mapper Unit
** @nam4rule SetObjectIdentifier Set the Ensembl Object identifier
** @nam4rule SetStart Set the start coordinate
** @nam4rule SetEnd Set the end coordinate
**
** @argrule * mu [EnsPMapperUnit] Ensembl Mapper Unit object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMapperUnitSetObjectIdentifier *************************************
**
** Set the Ensembl Object identifier element of an Ensembl Mapper Unit.
**
** @cc Bio::EnsEMBL::Mapper::Unit::id
** @param [u] mu [EnsPMapperUnit] Ensembl Mapper Unit
** @param [r] oid [ajuint] Ensembl Object identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperUnitSetObjectIdentifier(EnsPMapperUnit mu, ajuint oid)
{
    if(!mu)
        return ajFalse;
    
    if(!oid)
        return ajFalse;
    
    mu->ObjectIdentifier = oid;
    
    return ajTrue;
}




/* @func ensMapperUnitSetStart ************************************************
**
** Set the start coordinate element of an Ensembl Mapper Unit.
**
** @cc Bio::EnsEMBL::Mapper::Unit::start
** @param [u] mu [EnsPMapperUnit] Ensembl Mapper Unit
** @param [r] start [ajint] Start coordinate
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperUnitSetStart(EnsPMapperUnit mu, ajint start)
{
    if(!mu)
        return ajFalse;
    
    mu->Start = start;
    
    return ajTrue;
}




/* @func ensMapperUnitSetEnd **************************************************
**
** Set the end coordinate element of an Ensembl Mapper Unit.
**
** @cc Bio::EnsEMBL::Mapper::Unit::end
** @param [u] mu [EnsPMapperUnit] Ensembl Mapper Unit
** @param [r] end [ajint] End coordinate
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperUnitSetEnd(EnsPMapperUnit mu, ajint end)
{
    if(!mu)
        return ajFalse;
    
    mu->End = end;
    
    return ajTrue;
}




/* @func ensMapperUnitGetMemSize **********************************************
**
** Get the memory size in bytes of an Ensembl Mapper Unit.
**
** @param [r] mu [const EnsPMapperUnit] Ensembl Mapper Unit
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMapperUnitGetMemSize(const EnsPMapperUnit mu)
{
    ajuint size = 0;
    
    if(!mu)
	return 0;
    
    size += (ajuint) sizeof (EnsOMapperUnit);
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Mapper Unit object.
**
** @fdata [EnsPMapperUnit]
** @nam3rule Trace Report Ensembl Mapper Unit elements to debug file
**
** @argrule Trace mu [const EnsPMapperUnit] Ensembl Mapper Unit
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMapperUnitTrace ***************************************************
**
** Trace an Ensembl Mapper Unit.
**
** @param [r] mr [const EnsPMapperUnit] Ensembl Mapper Unit
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperUnitTrace(const EnsPMapperUnit mu, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!mu)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMapperUnitTrace %p\n"
	    "%S  ObjectIdentifier %u\n"
	    "%S  Start %d\n"
	    "%S  End %d\n"
	    "%S  Use %u\n",
	    indent, mu,
	    indent, mu->ObjectIdentifier,
	    indent, mu->Start,
	    indent, mu->End,
	    indent, mu->Use);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPMapperPair] Mapper Pair **********************************
**
** Functions for manipulating Ensembl Mapper Pair objects
**
** Bio::EnsEMBL::Mapper::IndelPair CVS Revision: 1.4
** Bio::EnsEMBL::Mapper::Pair CVS Revision: 1.8
**
** @nam2rule MapperPair
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Mapper Pair by pointer.
** It is the responsibility of the user to first destroy any previous
** Mapper Pair. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMapperPair]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMapperPair] Ensembl Mapper Pair
** @argrule Ref object [EnsPMapperPair] Ensembl Mapper Pair
**
** @valrule * [EnsPMapperPair] Ensembl Mapper Pair
**
** @fcategory new
******************************************************************************/




/* @func ensMapperPairNew *****************************************************
**
** Default constructor for an Ensembl Mapper Pair.
**
** @cc Bio::EnsEMBL::Mapper::Pair::new
** @param [u] source [EnsPMapperUnit] Source Ensembl Mapper Unit
** @param [u] target [EnsPMapperUnit] Target Ensembl Mapper Unit
** @param [r] ori [ajint] Relative orientation of the Ensembl Mapper Units
** @cc Bio::EnsEMBL::Mapper::IndelPair::new
** @param [r] indel [AjBool] Insertion-deletion attribute
**
** @return [EnsPMapperPair] Ensembl Mapper Pair or NULL
** @@
******************************************************************************/

EnsPMapperPair ensMapperPairNew(EnsPMapperUnit source,
                                EnsPMapperUnit target,
                                ajint ori,
                                AjBool indel)
{
    EnsPMapperPair mp = NULL;
    
    if(!source)
        return NULL;
    
    if(!target)
        return NULL;
    
    AJNEW0(mp);
    
    mp->Source = ensMapperUnitNewRef(source);
    
    mp->Target = ensMapperUnitNewRef(target);
    
    mp->Orientation = ori;
    
    mp->InsertionDeletion = indel;
    
    mp->Use = 1;
    
    return mp;
}




/* @func ensMapperPairNewC ****************************************************
**
** Construct an Ensembl Mapper Pair from components of Ensembl Mapper Units.
**
** @param [r] srcoid [ajuint] Source Ensembl Object identifier
** @param [r] srcstart [ajint] Source Start coordinate
** @param [r] srcend [ajint] Source End coordinate
** @param [r] trgoid [ajuint] Target Ensembl Object identifier
** @param [r] trgstart [ajint] Target Start coordinate
** @param [r] trgend [ajint] Target End coordinate
** @param [r] ori [ajint] Relative orientation of the Ensembl Mapper Units
** @param [r] indel [AjBool] Insertion-deletion attribute
**
** @return [EnsPMapperPair] Ensembl Mapper Pair or NULL
** @@
******************************************************************************/

EnsPMapperPair ensMapperPairNewC(ajuint srcoid,
                                 ajint srcstart,
                                 ajint srcend,
                                 ajuint trgoid,
                                 ajint trgstart,
                                 ajint trgend,
                                 ajint ori,
                                 AjBool indel)
{
    EnsPMapperUnit source = NULL;
    EnsPMapperUnit target = NULL;
    
    EnsPMapperPair mp = NULL;
    
    if(!srcoid)
	return NULL;
    
    if(!trgoid)
	return NULL;
    
    source = ensMapperUnitNew(srcoid, srcstart, srcend);
    
    target = ensMapperUnitNew(trgoid, trgstart, trgend);
    
    mp = ensMapperPairNew(source, target, ori, indel);
    
    ensMapperUnitDel(&source);
    
    ensMapperUnitDel(&target);
    
    return mp;
}




/* @func ensMapperPairNewObj **************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMapperPair] Ensembl Mapper Pair
**
** @return [EnsPMapperPair] Ensembl Mapper Pair or NULL
** @@
******************************************************************************/

EnsPMapperPair ensMapperPairNewObj(const EnsPMapperPair object)
{
    EnsPMapperPair mp = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(mp);
    
    mp->Source = ensMapperUnitNewObj(object->Source);
    mp->Target = ensMapperUnitNewObj(object->Target);
    
    mp->Orientation = object->Orientation;
    
    mp->InsertionDeletion = object->InsertionDeletion;
    
    mp->Use = 1;
    
    return mp;
}




/* @func ensMapperPairNewRef **************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count
**
** @param [u] mp [EnsPMapperPair] Ensembl Mapper Pair
**
** @return [EnsPMapperPair] Ensembl Mapper Pair or NULL
** @@
******************************************************************************/

EnsPMapperPair ensMapperPairNewRef(EnsPMapperPair mp)
{
    if(!mp)
        return NULL;
    
    mp->Use++;
    
    return mp;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Mapper Pair.
**
** @fdata [EnsPMapperPair]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Mapper Pair object
**
** @argrule * Pmp [EnsPMapperPair*] Mapper Pair object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMapperPairDel *****************************************************
**
** Default Ensembl Mapper Pair destructor.
**
** @param [d] Pmp [EnsPMapperPair*] Ensembl Mapper Pair address
**
** @return [void]
** @@
******************************************************************************/

void ensMapperPairDel(EnsPMapperPair *Pmp)
{
    EnsPMapperPair pthis = NULL;
    
    if(!Pmp)
        return;
    
    if(!*Pmp)
        return;

    pthis = *Pmp;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmp = NULL;
	
	return;
    }
    
    ensMapperUnitDel(&pthis->Source);
    
    ensMapperUnitDel(&pthis->Target);
    
    AJFREE(pthis);

    *Pmp = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Mapper Pair object.
**
** @fdata [EnsPMapperPair]
** @fnote None
**
** @nam3rule Get Return Ensembl Mapper Pair attribute(s)
** @nam4rule GetSource Return the source Ensembl Mapper Unit
** @nam4rule GetTarget Return the target Ensembl Mapper Unit
** @nam4rule GetOrientation Return the orientation
** @nam4rule GetInsertionDeletion Return the 'Insertion Deletion' element
**
** @argrule * mu [const EnsPMapperUnit] Ensembl Mapper Unit
**
** @valrule Source [EnsPMapperUnit] Ensembl Mapper Unit
** @valrule Target [EnsPMapperUnit] Ensembl Mapper Unit
** @valrule Orientation [ajint] Orientation
** @valrule InsertionDeletion [AjBool] 'Insertion Deletion' element
**
** @fcategory use
******************************************************************************/




/* @func ensMapperPairGetSource ***********************************************
**
** Get the source Ensembl Mapper Unit element of an Ensembl Mapper Pair.
**
** @cc Bio::EnsEMBL::Mapper::Pair::from
** @param [r] mu [const EnsPMapperPair] Ensembl Mapper Pair
**
** @return [EnsPMapperUnit] Source Ensembl Mapper Unit
** @@
******************************************************************************/

EnsPMapperUnit ensMapperPairGetSource(const EnsPMapperPair mp)
{
    if(!mp)
        return NULL;
    
    return mp->Source;
}




/* @func ensMapperPairGetTarget ***********************************************
**
** Get the Target Ensembl Mapper Unit element of an Ensembl Mapper Pair.
**
** @cc Bio::EnsEMBL::Mapper::Pair::to
** @param [r] mu [const EnsPMapperPair] Ensembl Mapper Pair
**
** @return [EnsPMapperUnit] Target Ensembl Mapper Unit
** @@
******************************************************************************/

EnsPMapperUnit ensMapperPairGetTarget(const EnsPMapperPair mp)
{
    if(!mp)
        return NULL;
    
    return mp->Target;
}




/* @func ensMapperPairGetOrientation ******************************************
**
** Get the relative orientation element of an Ensembl Mapper Pair.
**
** @cc Bio::EnsEMBL::Mapper::Pair::ori
** @param [r] mu [const EnsPMapperPair] Ensembl Mapper Pair
**
** @return [ajint] Relative orientation
** @@
******************************************************************************/

ajint ensMapperPairGetOrientation(const EnsPMapperPair mp)
{
    if(!mp)
        return 0;
    
    return mp->Orientation;
}




/* @func ensMapperPairGetInsertionDeletion ************************************
**
** Test whether this Ensembl Mapper Pair represents an insertion-deletion.
**
** @cc Bio::EnsEMBL::Mapper::IndelPair::???
** @param [r] mu [const EnsPMapperPair] Ensembl Mapper Pair
**
** @return [AjBool] ajTrue if this Mapper Pair represents an
**                  insertion-deletion
** @@
******************************************************************************/

AjBool ensMapperPairGetInsertionDeletion(const EnsPMapperPair mp)
{
    if(!mp)
        return ajFalse;
    
    return mp->InsertionDeletion;
}




/* @func ensMapperPairGetUnit *************************************************
**
** Get the Ensembl Mapper Unit element of an Ensembl Mapper Pair via an
** Ensembl Mapper Unit Type enumeration.
**
** @cc Bio::EnsEMBL::Mapper::Pair::from
** @cc Bio::EnsEMBL::Mapper::Pair::to
** @param [r] mu [const EnsPMapperPair] Ensembl Mapper Pair
** @param [r] type [AjEnum] Ensembl Mapper Unit Type
**
** @return [EnsPMapperUnit] Source or Target Ensembl Mapper Unit or NULL
** @@
******************************************************************************/

EnsPMapperUnit ensMapperPairGetUnit(const EnsPMapperPair mp, AjEnum type)
{
    if(!mp)
        return NULL;
    
    if(!type)
	return NULL;
    
    if(type == ensEMapperUnitTypeSource)
	return mp->Source;
    else if(type == ensEMapperUnitTypeTarget)
	return mp->Target;
    
    return NULL;
}




/* @func ensMapperPairGetMemSize **********************************************
**
** Get the memory size in bytes of an Ensembl Mapper Pair.
**
** @param [r] mp [const EnsPMapperPair] Ensembl Mapper Pair
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMapperPairGetMemSize(const EnsPMapperPair mp)
{
    ajuint size = 0;
    
    if(!mp)
	return 0;
    
    size += (ajuint) sizeof (EnsOMapperPair);
    
    size += ensMapperUnitGetMemSize(mp->Source);
    size += ensMapperUnitGetMemSize(mp->Target);
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Mapper Pair object.
**
** @fdata [EnsPMapperPair]
** @nam3rule Trace Report Ensembl Mapper Pair elements to debug file
**
** @argrule Trace mp [const EnsPMapperPair] Ensembl Mapper Pair
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMapperPairTrace ***************************************************
**
** Trace an Ensembl Mapper Pair.
**
** @param [r] mp [const EnsPMapperPair] Ensembl Mapper Pair
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperPairTrace(const EnsPMapperPair mp, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!mp)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMapperPairTrace %p\n"
	    "%S  Source %p\n"
	    "%S  Target %p\n"
	    "%S  Orientation %d\n"
	    "%S  InsertionDeletion '%B'\n"
	    "%S  Use %u\n",
	    indent, mp,
	    indent, mp->Source,
	    indent, mp->Target,
	    indent, mp->Orientation,
	    indent, mp->InsertionDeletion,
	    indent, mp->Use);
    
    ensMapperUnitTrace(mp->Source, level + 1);
    ensMapperUnitTrace(mp->Target, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPMapperResult] Mapper Result ******************************
**
** Functions for manipulating Ensembl Mapper Result objects
**
** Bio::EnsEMBL::Mapper::Coordinate CVS Revision: 1.11
** Bio::EnsEMBL::Mapper::Gap CVS Revision: 1.9
** Bio::EnsEMBL::Mapper::IndelCoordinate CVS Revision: 1.5
**
** @nam2rule MapperResult
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Mapper Result by pointer.
** It is the responsibility of the user to first destroy any previous
** Mapper Result. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMapperResult]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMapperResult] Ensembl Mapper Result
** @argrule Ref object [EnsPMapperResult] Ensembl Mapper Result
**
** @valrule * [EnsPMapperResult] Ensembl Mapper Result
**
** @fcategory new
******************************************************************************/




/* @func ensMapperResultNew ***************************************************
**
** Default constructor for an Ensembl Mapper Result.
**
** @param [r] type [AjEnum] Ensembl Mapper Result Type
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [u] cs [EnsPCoordSystem] Ensembl Coordinate System
** @param [r] gapstart [ajint] Gap start coordinate
** @param [r] gapend [ajint] Gap end coordinate
**
** @return [EnsPMapperResult] Ensembl Mapper Result or NULL
** @@
******************************************************************************/

EnsPMapperResult ensMapperResultNew(AjEnum type,
                                    ajuint oid,
                                    ajint start,
                                    ajint end,
                                    ajint strand,
                                    EnsPCoordSystem cs,
                                    ajint gapstart,
                                    ajint gapend)
{
    EnsPMapperResult mr = NULL;
    
    if(!type)
        return NULL;
    
    if((type == ensEMapperResultCoordinate) ||
	(type == ensEMapperResultInDel))
    {
        if(!oid)
            return NULL;
	
        if(!cs)
            return NULL;
    }
    
    AJNEW0(mr);
    
    mr->Type = type;
    
    mr->ObjectIdentifier = oid;
    
    mr->Start = start;
    
    mr->End = end;
    
    mr->Strand = strand;
    
    mr->CoordSystem = ensCoordSystemNewRef(cs);
    
    mr->GapStart = gapstart;
    
    mr->GapEnd = gapend;
    
    mr->Use = 1;
    
    return mr;
}




/* @func ensMapperResultNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [EnsPMapperResult] Ensembl Mapper Result or NULL
** @@
******************************************************************************/

EnsPMapperResult ensMapperResultNewObj(const EnsPMapperResult object)
{
    EnsPMapperResult mr = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(mr);
    
    mr->Type = object->Type;
    
    mr->ObjectIdentifier = object->ObjectIdentifier;
    
    mr->Start = object->Start;
    
    mr->End = object->End;
    
    mr->Strand = object->Strand;
    
    mr->CoordSystem = ensCoordSystemNewRef(object->CoordSystem);
    
    mr->GapStart = object->GapStart;
    
    mr->GapEnd = object->GapEnd;
    
    mr->Use = 1;
    
    return mr;
}




/* @func ensMapperResultNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mr [EnsPMapperResult] Ensembl Mapper Result
**
** @return [EnsPMapperResult] Ensembl Mapper Result or NULL
** @@
******************************************************************************/

EnsPMapperResult ensMapperResultNewRef(EnsPMapperResult mr)
{
    if(!mr)
	return NULL;
    
    mr->Use++;
    
    return mr;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Mapper Result.
**
** @fdata [EnsPMapperResult]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Mapper Result object
**
** @argrule * Pmr [EnsPMapperResult*] Mapper Result object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMapperResultDel ***************************************************
**
** Default Ensembl Mapper Result destructor.
**
** @param [d] Pmr [EnsPMapperResult*] Ensembl Mapper Result address
**
** @return [void]
** @@
******************************************************************************/

void ensMapperResultDel(EnsPMapperResult *Pmr)
{
    EnsPMapperResult pthis = NULL;
    
    if(!Pmr)
        return;
    
    if(!*Pmr)
        return;

    pthis = *Pmr;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmr = NULL;
	
	return;
    }
    
    ensCoordSystemDel(&pthis->CoordSystem);
    
    AJFREE(pthis);

    *Pmr = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Mapper Result object.
**
** @fdata [EnsPMapperResult]
** @fnote None
**
** @nam3rule Get Return Ensembl Mapper Result attribute(s)
** @nam4rule GetType Return the Mapper Result Type
** @nam4rule GetObjectIdentifier Return the Ensembl Object identifier
** @nam4rule GetStart Return the start coordinate
** @nam4rule GetEnd Return the end coordinate
** @nam4rule GetStrand Return the strand information
** @nam4rule GetCoordSystem Return the Ensembl Coordinate System
** @nam4rule GetGapStart Return the gap start coordinate
** @nam4rule GetGapEnd Return the gap end coordinate
**
** @argrule * mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @valrule Type [AjEnum] Ensembl Mapper Result type
** @valrule ObjectIdentifier [ajuint] Ensembl Object identifier
** @valrule Start [ajint] Start coordinate
** @valrule End [ajint] End coordinate
** @valrule Strand [ajint] Strand information
** @valrule CoordSystem [EnsPCoordSystem] Ensembl Coordinate System
** @valrule GapStart [ajint] Gap start coordinate
** @valrule GapEnd [ajint] Gap end coordinate
**
** @fcategory use
******************************************************************************/




/* @func ensMapperResultGetType ***********************************************
**
** Get the type element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [AjEnum] Ensembl Mapper Result type
** @@
******************************************************************************/

AjEnum ensMapperResultGetType(const EnsPMapperResult mr)
{
    if(!mr)
        return ensEMapperResultNULL;
    
    return mr->Type;
}




/* @func ensMapperResultGetObjectIdentifier ***********************************
**
** Get the Object identifier element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajuint] Ensembl Object identifier
** @@
******************************************************************************/

ajuint ensMapperResultGetObjectIdentifier(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return mr->ObjectIdentifier;
}




/* @func ensMapperResultGetStart **********************************************
**
** Get the start coordinate element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajint] Ensembl Mapper Result start coordinate
** @@
******************************************************************************/

ajint ensMapperResultGetStart(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return mr->Start;
}




/* @func ensMapperResultGetEnd ************************************************
**
** Get the end coordinate element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajint] Ensembl Mapper Result end coordinate
** @@
******************************************************************************/

ajint ensMapperResultGetEnd(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return mr->End;
}




/* @func ensMapperResultGetStrand *********************************************
**
** Get the strand element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajint] Ensembl Mapper Result strand information
** @@
******************************************************************************/

ajint ensMapperResultGetStrand(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return mr->Strand;
}




/* @func ensMapperResultGetCoordSystem ****************************************
**
** Get the Ensembl Coordinate System element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [EnsPCoordSystem] Ensembl Coordinate System
** @@
******************************************************************************/

EnsPCoordSystem ensMapperResultGetCoordSystem(const EnsPMapperResult mr)
{
    if(!mr)
        return NULL;
    
    return mr->CoordSystem;
}




/* @func ensMapperResultGetGapStart *******************************************
**
** Get the Gap Start element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajint] Ensembl Mapper Result Gap Start Coordinate
** @@
******************************************************************************/

ajint ensMapperResultGetGapStart(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return mr->GapStart;
}




/* @func ensMapperResultGetGapEnd *********************************************
**
** Get the Gap End element of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajint] Ensembl Mapper Result Gap End Coordinate
** @@
******************************************************************************/

ajint ensMapperResultGetGapEnd(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return mr->GapEnd;
}




/* @func ensMapperResultGetCoordinateLength ***********************************
**
** Get the coordinate length of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajuint] Ensembl Mapper Result coordinate length
** @@
******************************************************************************/

ajuint ensMapperResultGetCoordinateLength(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return (mr->End - mr->Start + 1);
}




/* @func ensMapperResultGetGapLength ******************************************
**
** Get the gap length of an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajuint] Ensembl Mapper Result gap length
** @@
******************************************************************************/

ajuint ensMapperResultGetGapLength(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    return (mr->GapEnd - mr->GapStart + 1);
}




/* @func ensMapperResultGetLength *********************************************
**
** Get the length of an Ensembl Mapper Result.
** This is the coordinate length for Mapper Results of type Coordinate and
** Insertion-Dletion and the gap length for Mapper Resuls of type Gap.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
**
** @return [ajuint] Ensembl Mapper Result length
** @@
******************************************************************************/

ajuint ensMapperResultGetLength(const EnsPMapperResult mr)
{
    if(!mr)
        return 0;
    
    if(mr->Type == ensEMapperResultCoordinate)
	return ensMapperResultGetCoordinateLength(mr);
    else if(mr->Type == ensEMapperResultGap)
	return ensMapperResultGetGapLength(mr);
    else if(mr->Type == ensEMapperResultInDel)
	return ensMapperResultGetCoordinateLength(mr);
    else
	ajWarn("ensMapperResultGetLength got unexpected "
	       "Mapper Result type %d.\n", mr->Type);
    
    return 0;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Mapper Result object.
**
** @fdata [EnsPMapperResult]
** @nam3rule Trace Report Ensembl Mapper Result elements to debug file
**
** @argrule Trace mr [const EnsPMapperResult] Ensembl Mapper Result
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMapperResultTrace *************************************************
**
** Trace an Ensembl Mapper Result.
**
** @param [r] mr [const EnsPMapperResult] Ensembl Mapper Result
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperResultTrace(const EnsPMapperResult mr, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!mr)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMapperResultTrace %p\n"
	    "%S  Type %d\n"
	    "%S  ObjectIdentifier %u\n"
	    "%S  Start %d\n"
	    "%S  End %d\n"
	    "%S  Strand %d\n"
	    "%S  CoordSystem %p\n"
	    "%S  GapStart %d\n"
	    "%S  GapEnd %d\n"
	    "%S  Use %u\n",
	    indent, mr,
	    indent, mr->Type,
	    indent, mr->ObjectIdentifier,
	    indent, mr->Start,
	    indent, mr->End,
	    indent, mr->Strand,
	    indent, mr->CoordSystem,
	    indent, mr->GapStart,
	    indent, mr->GapEnd,
	    indent, mr->Use);
    
    ensCoordSystemTrace(mr->CoordSystem, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPMapperRange] Mapper Range ********************************
**
** Functions for manipulating Ensembl Mapper Range objects
**
** @nam2rule MapperRange
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Mapper Range by pointer.
** It is the responsibility of the user to first destroy any previous
** Mapper Range. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMapperRange]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMapperRange] Ensembl Mapper Range
** @argrule Ref object [EnsPMapperRange] Ensembl Mapper Range
**
** @valrule * [EnsPMapperRange] Ensembl Mapper Range
**
** @fcategory new
******************************************************************************/




/* @func ensMapperRangeNew ****************************************************
**
** Default constructor for an Ensembl Mapper Range.
**
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
**
** @return [EnsPMapperRange] Ensembl Mapper Range
** @@
******************************************************************************/

EnsPMapperRange ensMapperRangeNew(ajint start, ajint end)
{
    EnsPMapperRange mr = NULL;
    
    AJNEW0(mr);
    
    mr->Start = start;
    mr->End   = end;
    mr->Use   = 1;
    
    return mr;
}




/* @func ensMapperRangeNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMapperRange] Ensembl Mapper Range
**
** @return [EnsPMapperRange] Ensembl Mapper Range or NULL
** @@
******************************************************************************/

EnsPMapperRange ensMapperRangeNewObj(const EnsPMapperRange object)
{
    EnsPMapperRange mr = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(mr);
    
    mr->Start = object->Start;
    mr->End   = object->End;
    mr->Use   = 1;
    
    return mr;
}




/* @func ensMapperRangeNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mr [EnsPMapperRange] Ensembl Mapper Range
**
** @return [EnsPMapperRange] Ensembl Mapper Range or NULL
** @@
******************************************************************************/

EnsPMapperRange ensMapperRangeNewRef(EnsPMapperRange mr)
{
    if(!mr)
	return NULL;
    
    mr->Use++;
    
    return mr;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Mapper Range.
**
** @fdata [EnsPMapperRange]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Mapper Range object
**
** @argrule * Pmr [EnsPMapperRange*] Ensembl Mapper Range object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMapperRangeDel ****************************************************
**
** Default Ensembl Mapper Range destructor.
**
** @param [d] Pmr [EnsPMapperRange*] Ensembl Mapper Range address
**
** @return [void]
** @@
******************************************************************************/

void ensMapperRangeDel(EnsPMapperRange *Pmr)
{
    EnsPMapperRange pthis = NULL;
    
    if(!Pmr)
	return;
    
    if(!*Pmr)
	return;

    pthis = *Pmr;
    
    /*
     ajDebug("ensMapperRangeDel\n"
	     "  *Pmr %p\n",
	     *Pmr);
     */
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmr = NULL;
	
	return;
    }
    
    AJFREE(pthis);

    *Pmr = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Mapper Range object.
**
** @fdata [EnsPMapperRange]
** @fnote None
**
** @nam3rule Get Return Ensembl Mapper Range attribute(s)
** @nam4rule GetStart Return the start coordinate
** @nam4rule GetEnd Return the end coordinate
**
** @argrule * mr [const EnsPMapperRange] Ensembl Mapper Range
**
** @valrule Start [ajint] Start coordinate
** @valrule End [ajint] End coordinate
**
** @fcategory use
******************************************************************************/




/* @func ensMapperRangeGetStart ***********************************************
**
** Get the Start coordinate element of an Ensembl Mapper Range.
**
** @param [r] mr [const EnsPMapperRange] Ensembl Mapper Range
**
** @return [ajint] Ensembl Mapper Range start coordinate
** @@
******************************************************************************/

ajint ensMapperRangeGetStart(const EnsPMapperRange mr)
{
    if(!mr)
	return 0;
    
    return mr->Start;
}




/* @func ensMapperRangeGetEnd *************************************************
**
** Get the End coordinate element of an Ensembl Mapper Range.
**
** @param [r] mr [const EnsPMapperRange] Ensembl Mapper Range
**
** @return [ajint] Ensembl Mapper Range end coordinate
** @@
******************************************************************************/

ajint ensMapperRangeGetEnd(const EnsPMapperRange mr)
{
    if(!mr)
	return 0;
    
    return mr->End;
}




/* @datasection [EnsPMapperRangeRegistry] Mapper Range Registry ***************
**
** Functions for manipulating Ensembl Mapper Range Registry objects
**
** Bio::EnsEMBL::Mapper::RangeRegistry CVS Revision: 1.11
**
** @nam2rule MapperRangeRegistry
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Mapper Range Registry by pointer.
** It is the responsibility of the user to first destroy any previous
** Mapper Range Registry. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMapperRangeRegistry]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry
** @argrule Ref object [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry
**
** @valrule * [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry
**
** @fcategory new
******************************************************************************/




/* @func ensMapperRangeRegistryNew ********************************************
**
** Default constructor for an Ensembl Mapper Range Registry.
**
** @cc Bio::EnsEMBL::Mapper::RangeRegistry::new
** @return [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry
** @@
** The Ensembl Mapper Range Registry maintains an internal list of registered
** regions and is used to quickly ascertain if and what regions of a provided
** range need registration. It is implemented as a first-level AJAX table with
** Ensembl Object identifers as keys and second-level AJAX Lists of
** Ensembl Mapper Ranges as values.
******************************************************************************/

EnsPMapperRangeRegistry ensMapperRangeRegistryNew(void)
{
    EnsPMapperRangeRegistry mrr = NULL;
    
    /*
     ajDebug("ensMapperRangeRegistryNew\n");
     */
    
    AJNEW0(mrr);
    
    mrr->Registry =
	ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    mrr->Use = 1;
    
    return mrr;
}




/* @func ensMapperRangeRegistryNewRef *****************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mrr [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry
**
** @return [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry or NULL
** @@
******************************************************************************/

EnsPMapperRangeRegistry ensMapperRangeRegistryNewRef(
    EnsPMapperRangeRegistry mrr)
{
    if(!mrr)
	return NULL;
    
    mrr->Use++;
    
    return mrr;
}




/* @func ensMapperRangeRegistryClear ******************************************
**
** Clear an Ensembl Mapper Range Registry.
** This function clears the unsigned integer key data and the AJAX List value
** data from the first-level AJAX Table, as well as the Ensembl Mapper Ranges
** from the second-level AJAX List.
**
** @param [u] mrr [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperRangeRegistryClear(EnsPMapperRangeRegistry mrr)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    EnsPMapperRange mr = NULL;
    
    /*
     ajDebug("ensMapperRangeRegistryClear\n"
	     "  mrr %p\n",
	     mrr);
     */
    
    if(!mrr)
	return ajFalse;
    
    /* Clear the first-level AJAX Table. */
    
    ajTableToarray(mrr->Registry, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	ajTableRemove(mrr->Registry, (const void *) keyarray[i]);
	
	/* Delete unsigned integer key data. */
	
	AJFREE(keyarray[i]);
	
	/* Clear and delete the second-level AJAX List value data. */
	
	while(ajListPop((AjPList) valarray[i], (void **) &mr))
	    ensMapperRangeDel(&mr);
	
	ajListFree((AjPList *) &valarray[i]);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return ajTrue;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Mapper Range Registry.
**
** @fdata [EnsPMapperRangeRegistry]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Mapper Range Registry object
**
** @argrule * Pmrr [EnsPMapperRangeRegistry*] Ensembl Mapper Range Registry
**                                            object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMapperRangeRegistryDel ********************************************
**
** Default Ensembl Mapper Range Registry destructor.
**
** @param [d] Pmrr [EnsPMapperRangeRegistry*] Ensembl Mapper
**                                            Range Registry address
**
** @return [void]
** @@
******************************************************************************/

void ensMapperRangeRegistryDel(EnsPMapperRangeRegistry *Pmrr)
{
    EnsPMapperRangeRegistry pthis = NULL;
    
    if(!Pmrr)
	return;
    
    if(!*Pmrr)
	return;

    pthis = *Pmrr;
    
    /*
     ajDebug("ensMapperRangeRegistryDel\n"
	     "  *Pmrr %p\n",
	     *Pmrr);
     */
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmrr = NULL;
	
	return;
    }
    
    ensMapperRangeRegistryClear(pthis);
    
    ajTableFree(&pthis->Registry);
    
    AJFREE(pthis);

    *Pmrr = NULL;
    
    return;
}




/* @func ensMapperRangeRegistryCheckAndRegister *******************************
**
** Check and register Ensembl Mapper Ranges in the
** Ensembl Mapper Range Registry.
**
** @cc Bio::EnsEMBL::Mapper::RangeRegistry::check_and_register
** @param [u] mrr [EnsPMapperRangeRegistry] Ensembl Mapper Range Registry
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] chkstart [ajint] Start coordinate of the region to be checked
** @param [r] chkend [ajint] End coordinate of the region to be checked
** @param [r] regstart [ajint] Start coordinate of the region to register
** @param [r] regend [ajint] End coordinate of the region to register
** @param [uN] ranges [AjPList] AJAX List of Ensembl Mapper Ranges
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
**
** Checks the Ensembl Mapper Range Registry to see if the entire range denoted
** by (oid:chkstart-chkend) is already registered. If it already is, an empty
** AJAX List is returned. If it is not, then the range specified by regstart
** and regend is registered and an AJAX List of Ensembl Mapper Regions that
** were required to completely register this region is returned.
**
** If regstart and regend are not defined, they default to chkstart and chkend,
** respectively.
**
** The reason, there is just a single function to do both the checking and
** registering is to reduce the overhead. Much of the work to check if a range
** is registered is the same as registering a region around that range.
******************************************************************************/

AjBool ensMapperRangeRegistryCheckAndRegister(EnsPMapperRangeRegistry mrr,
                                              ajuint oid,
                                              ajint chkstart,
                                              ajint chkend,
                                              ajint regstart,
                                              ajint regend,
                                              AjPList ranges)
{
    register ajuint i = 0;
    
    ajuint length = 0;
    
    /* Binary search indices range from 0 to list length - 1 */
    
    ajuint idxstart = 0;
    ajuint idxmid   = 0;
    ajuint idxend   = 0;
    
    /* Registry indices */
    
    ajuint idxregstart = 0;
    ajuint idxregend   = 0;
    ajuint idxregpos   = 0;
    
    ajint gapstart = 0;
    ajint gapend   = 0;
    ajint newstart = 0;
    ajint newend   = 0;
    
    ajuint *Poid = NULL;
    
    AjBool idxregstartset = AJFALSE;
    AjBool idxregposset   = AJFALSE;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    EnsPMapperRange range = NULL;
    EnsPMapperRange gap   = NULL;
    
    /*
     ajDebug("ensMapperRangeRegistryCheckAndRegister\n"
	     "  mrr %p\n"
	     "  oid %u\n"
	     "  chkstart %d\n"
	     "  chkend %d\n"
	     "  regstart %d (%#x)\n"
	     "  regend %d (%#x)\n"
	     "  ranges %p\n",
	     mrr,
	     oid,
	     chkstart,
	     chkend,
	     regstart, regstart,
	     regend, regend,
	     ranges);
     */
    
    if(!mrr)
    {	
        ajDebug("ensMapperRangeRegistryCheckAndRegister requires an "
                "Ensembl Range Registry.\n");
	
	return ajFalse;
    }
    
    if(!oid)
	ajWarn("ensMapperRangeRegistryCheckAndRegister did not get a valid "
	       "Ensembl Object identifier.\n");
    
    if(chkstart > chkend)
    {
	/*
	** FIXME: The Perl API states:
	** "start argument must be less than end argument",
	** but the actual test is for less than or equal.
	*/
	
	ajWarn("ensMapperRangeRegistryCheckAndRegister requires the start "
	       "coordinate to be less than the end coordinate for the "
	       "region to be checked.\n");
	
	return ajFalse;
    }
    
    if((!regstart) && (!regend))
    {
	regstart = chkstart;
	
	regend = chkend;
    }
    
    if(regstart > regend)
    {
	/*
	** FIXME: The Perl API states:
	** "rend [$rstart] argument must be less than rend [$rend] argument",
	** but the actual test is for less than or equal.
	*/
	
	ajWarn("ensMapperRangeRegistryCheckAndRegister requires the start "
	       "coordinate %d to be less than the end coordinate %d "
	       "for the region to be registered.\n",
	       regstart, regend);
	
	return ajFalse;
    }
    
    if(regstart > chkstart)
    {
	ajWarn("ensMapperRangeRegistryCheckAndRegister requires the start "
	       "coordinate %d of the region to be registered to be less "
	       "than or equal to the start coordinate %d of the "
	       "region to be checked.\n",
	       regstart, chkstart);
	
	return ajFalse;
    }
    
    if(regend < chkend)
    {
	ajWarn("ensMapperRangeRegistryCheckAndRegister requires the end "
	       "coordinate %d of the region to be registered to be less "
	       "than or equal to the end coordinate %d of the "
	       "region to be checked.\n",
	       regend, chkend);
	
	return ajFalse;
    }
    
    if(!mrr->Registry)
	ajFatal("ensMapperRangeRegistryCheckAndRegister got "
		"Mapper Range Registry without a valid Registry "
		"AJAX Table.\n");
    
    list = (AjPList) ajTableFetch(mrr->Registry, (const void *) &oid);
    
    if(!list)
    {
	AJNEW0(Poid);
	
	*Poid = oid;
	
	list = ajListNew();
	
	ajTablePut(mrr->Registry, (void *) Poid, (void *) list);
    }
    
    length = ajListGetLength(list);
    
    if(!length)
    {
        /*
	** This is the first request for this Ensembl Object identifier,
	** return a gap Mapper Range for the entire range and register it as
	** seen.
	*/
	
	range = ensMapperRangeNew(regstart, regend);
	
	ajListPushAppend(list, (void *) range);
	
	if(ranges)
	    ajListPushAppend(ranges, (void *) ensMapperRangeNewRef(range));
	
	return ajTrue;
    }
    
    /*
    ** Loop through the List of existing Ensembl Mapper Ranges recording any
    ** "gaps" where the existing Mapper Ranges do not cover part of the
    ** requested range.
    */
    
    idxstart = 0;
    
    idxend = length - 1;
    
    /*
    ** Binary search the relevant Ensembl Mapper Ranges,
    ** which helps if the list is big.
    */
    
    while((idxend - idxstart) > 1)
    {
	idxmid = (idxstart + idxend) >> 1;
	
	ajListPeekNumber(list, idxmid, (void **) &range);
	
	if(range->End < regstart)
	    idxstart = idxmid;
	else
	    idxend = idxmid;
    }
    
    gapstart = regstart;
    
    for(i = idxstart; i < length; i++)
    {
	ajListPeekNumber(list, i, (void **) &range);
	
	/*
	** No work needs to be done at all if we find a Mapper Range that
	** entirely overlaps the requested region.
	*/
	
	if((range->Start <= chkstart) && (range->End >= chkend))
	    return ajTrue;
	
	/* Find adjacent or overlapping regions already registered. */
	
	if((range->Start <= (regend + 1)) && (range->End >= (regstart - 1)))
	{
	    if(!idxregstartset)
	    {
		idxregstartset = ajTrue;
		
		idxregstart = i;
	    }
	    
	    idxregend = i;
	}
	
	/* Find previously un-registered regions (gaps). */
	
	if(range->Start > regstart)
	{
	    gapend = (regend < range->Start) ? regend : (range->Start - 1);
	    
	    if(ranges)
	    {
		gap = ensMapperRangeNew(gapstart, gapend);
		
		ajListPushAppend(ranges, (void *) gap);
	    }
	}
	
	gapstart = (regstart > range->End) ? regstart : (range->End + 1);
	
	if((range->End >= regend) && (!idxregposset))
	{
	    idxregposset = ajTrue;
	    
	    idxregpos = i;
	    
	    break;
	}
    }
    
    /* Do we have to define another gap? */
    
    if((gapstart <= regend) && ranges)
    {
	gap = ensMapperRangeNew(gapstart, regend);
	
	ajListPushAppend(ranges, (void *) gap);
    }
    
    /* Merge the new Ensembl Mapper Range into the registered List. */
    
    if(idxregstartset)
    {
	/* Adjacent or overlapping regions have been found. */
	
	ajListPeekNumber(list, idxregstart, (void **) &range);
	
	newstart = (regstart < range->Start) ? regstart : range->Start;
	
	ajListPeekNumber(list, idxregend, (void **) &range);
	
	newend = (regend > range->End) ? regend : range->End;
	
	iter = ajListIterNew(list);
	
	/* Position the List Iterator at idxregstart. */
	
	for(i = 0; i < idxregstart; i++)
	    range = (EnsPMapperRange) ajListIterGet(iter);
	
	/*
	** Now, remove idxregend - idxregstart + 1 elements
	** from the List.
	*/
	
	for(i = 0; i < (idxregend - idxregstart + 1); i++)
	{
	    range = (EnsPMapperRange) ajListIterGet(iter);
	    
	    ajListIterRemove(iter);
	    
	    ensMapperRangeDel(&range);
	}
	
	/* Finally, insert a new Mapper Range at the current position. */
	
	range = ensMapperRangeNew(newstart, newend);
	
	ajListIterInsert(iter, (void *) range);
	
	ajListIterDel(&iter);
    }
    else if(idxregposset)
    {
	iter = ajListIterNew(list);
	
	/* Position the List Iterator at idxregpos. */
	
	for(i = 0; i < idxregpos; i++)
	    range = (EnsPMapperRange) ajListIterGet(iter);
	
	/* Insert a new Mapper Range at this position. */
	
	range = ensMapperRangeNew(regstart, regend);
	
	ajListIterInsert(iter, (void *) range);
	
	ajListIterDel(&iter);
    }
    else
    {
	range = ensMapperRangeNew(regstart, regend);
	
	ajListPushAppend(list, (void *) range);
    }
    
    return ajTrue;
}




/* @func ensMapperRangeRegistryGetRanges **************************************
**
** Get Ensembl Mapper Ranges from the Ensembl Mapper Range Registry.
**
** @cc Bio::EnsEMBL::Mapper::RangeRegistry::get_ranges
** @param [r] mrr [const EnsPMapperRangeRegistry] Ensembl Mapper
**                                                Range Registry
** @param [r] oid [ajuint] Ensembl Object identifier
**
** @return [const AjPList] AJAX List of Ensembl Mapper Ranges or NULL
** @@
******************************************************************************/

const AjPList ensMapperRangeRegistryGetRanges(const EnsPMapperRangeRegistry mrr,
                                              ajuint oid)
{
    if(!mrr)
	return NULL;
    
    if(!oid)
	return NULL;
    
    return (AjPList) ajTableFetch(mrr->Registry, (const void *) &oid);
}




/* @func ensMapperRangeRegistryOverlapSize ************************************
**
** Get the overlap size of Ensembl Mapper Ranges in the
** Ensembl Mapper Range Registry. Finds out how many bases in the given range
** are already registered in an Ensembl Mapper Range Registry.
**
** @cc Bio::EnsEMBL::Mapper::RangeRegistry::overlap_size
** @param [u] mrr [const EnsPMapperRangeRegistry] Ensembl Mapper
**                                                Range Registry
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
**
** @return [ajuint] Overlap size or 0
** @@
******************************************************************************/

/*
** FIXME: The Perl API Pdoc problems:
** Example uses "chr1" as method parameter. The 'overlap_size'
** method, however, requires sequence region identifiers or rather
** generic object ids (see TranscriptMapper) than sequence region names!
**
** Instead of string $id for Arg [1] there should be int $id.
**
** There are two Arg [1] lines.
*/

ajuint ensMapperRangeRegistryOverlapSize(const EnsPMapperRangeRegistry mrr,
                                         ajuint oid,
                                         ajint start,
                                         ajint end)
{
    register ajuint i = 0;
    
    ajuint length = 0;
    
    /* Binary search indices range from 0 to list length - 1 */
    
    ajuint idxstart = 0;
    ajuint idxmid   = 0;
    ajuint idxend   = 0;
    
    ajint mrstart = 0;
    ajint mrend   = 0;
    
    ajuint overlap = 0;
    
    AjPList list = NULL;
    
    EnsPMapperRange range = NULL;
    
    if(!mrr)
	return 0;
    
    if(start > end)
	return 0;
    
    list = (AjPList) ajTableFetch(mrr->Registry, (const void *) &oid);
    
    if(!list)
	return 0;
    
    length = ajListGetLength(list);
    
    if(!length)
	return 0;
    
    idxstart = 0;
    
    idxend = length - 1;
    
    /*
    ** Binary search the relevant Ensembl Mapper Ranges,
    ** which helps if the list is big.
    */
    
    while((idxend - idxstart) > 1)
    {
	idxmid = (idxstart + idxend) >> 1;
	
	ajListPeekNumber(list, idxmid, (void **) &range);
	
	if(range->End < start)
	    idxstart = idxmid;
	else
	    idxend = idxmid;
    }
    
    for(i = idxstart; i < length; i++)
    {
	ajListPeekNumber(list, i, (void **) &range);
	
	/*
	** Check, wheher the loop has already overrun.
	** If that was the case, there are no more interesting Mapper Ranges.
	*/
	
	if(range->Start > start)
	    break;
	
	/*
	** No work needs to be done at all if we find a Mapper Range that
	** entirely overlaps the requested region.
	*/
	
	if((range->Start <= start) && (range->End >= end))
	{
	    overlap = (ajuint) (end - start + 1);
	    
	    break;
	}
	
	mrstart = (start < range->Start) ? range->Start : start;
	
	mrend = (end < range->End) ? end : range->End;
	
	if((mrend - mrstart) >= 0)
	    overlap += (ajuint) (mrend - mrstart + 1);
    }
    
    return overlap;
}




/* @datasection [EnsPMapper] Mapper *******************************************
**
** Functions for manipulating Ensembl Mapper objects
**
** @cc Bio::EnsEMBL::Mapper CVS Revision: 1.46
**
** @nam2rule Mapper
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Mapper by pointer.
** It is the responsibility of the user to first destroy any previous
** Mapper. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMapper]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMapper] Ensembl Mapper
** @argrule Ref object [EnsPMapper] Ensembl Mapper
**
** @valrule * [EnsPMapper] Ensembl Mapper
**
** @fcategory new
******************************************************************************/




/* @func ensMapperNew *********************************************************
**
** Default constructor for an Ensembl Mapper.
**
** @cc Bio::EnsEMBL::Mapper::new
** @param [r] srctype [AjPStr] Source mapping type
** @param [r] trgtype [AjPStr] Target mapping type
** @param [u] srccs [EnsPCoordSystem] Source Ensembl Coordinate System
** @param [u] trgcs [EnsPCoordSystem] Target Ensembl Coordinate System
**
** @return [EnsPMapper] Ensembl Mapper or NULL
** @@
******************************************************************************/

EnsPMapper ensMapperNew(AjPStr srctype,
                        AjPStr trgtype,
                        EnsPCoordSystem srccs,
                        EnsPCoordSystem trgcs)
{
    AjPTable table = NULL;
    
    EnsPMapper mapper = NULL;
    
    /*
     ajDebug("ensMapperNew\n"
	     "  srctype '%S'\n"
	     "  trgtype '%S'\n"
	     "  srccs %p\n"
	     "  trgcs %p\n",
	     srctype,
	     trgtype,
	     srccs,
	     trgcs);
     
     ensCoordSystemTrace(srccs, 1);
     
     ensCoordSystemTrace(trgcs, 1);
     */
    
    if(!srctype)
        return NULL;
    
    if(!trgtype)
        return NULL;
    
    if(!srccs)
	return NULL;
    
    if(!trgcs)
	return NULL;
    
    AJNEW0(mapper);
    
    mapper->SourceType = ajStrNewRef(srctype);
    
    mapper->TargetType = ajStrNewRef(trgtype);
    
    mapper->SourceCoordSystem = ensCoordSystemNewRef(srccs);
    
    mapper->TargetCoordSystem = ensCoordSystemNewRef(trgcs);
    
    mapper->Pairs = ajTablestrNewCaseLen(0);
    
    mapper->PairCount = 0;
    
    mapper->IsSorted = ajFalse;
    
    mapper->Use = 1;
    
    /*
    ** Initialise second-level AJAX Tables with Ensembl Object identifier
    ** keys and put them into the first-level AJAX Table indexed on
    ** source and target types.
    */
    
    table = ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    ajTablePut(mapper->Pairs, (void *) ajStrNewS(srctype), (void *) table);
    
    table = ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    ajTablePut(mapper->Pairs, (void *) ajStrNewS(trgtype), (void *) table);
    
    return mapper;
}




/* @func ensMapperNewRef ******************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mapper [EnsPMapper] Ensembl Mapper
**
** @return [EnsPMapper] Ensembl Mapper or NULL
** @@
******************************************************************************/

EnsPMapper ensMapperNewRef(EnsPMapper mapper)
{
    if(!mapper)
	return NULL;
    
    mapper->Use++;
    
    return mapper;
}




/* @func ensMapperClear *******************************************************
**
** Clear an Ensembl Mapper.
**
** This function does not delete or modify first-level AJAX Tables with mapping
** type AJAX Strings as keys and second-level AJAX Tables as values.
** The second-level AJAX Tables with Object identifiers as keys and
** third-level AJAX Lists of Ensembl Mapping Pairs are cleared.
** The third-level Lists of Ensembl Mapping Pairs are cleared and deleted, as
** are the Ensembl Mapper Pair list elements.
**
** @cc Bio::EnsEMBL::Mapper::flush
** @param [u] mapper [EnsPMapper] Ensembl Mapper
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperClear(EnsPMapper mapper)
{
    void **keyarray1 = NULL;
    void **valarray1 = NULL;
    void **keyarray2 = NULL;
    void **valarray2 = NULL;
    
    register ajuint i = 0;
    register ajuint j = 0;
    
    EnsPMapperPair mp = NULL;
    
    if(!mapper)
	return ajFalse;
    
    /*
     ajDebug("ensMapperClear\n"
	     "  mapper %p\n",
	     mapper);
     
     ensMapperTrace(mapper);
     */
    
    /*
    ** Clear the first-level AJAX Table with mapping type AJAX Strings of
    ** source and target as keys and AJAX Tables as values into an array.
    ** This first-level table is not modified here, but deleted later by the
    ** ensMapperDel function.
    */
    
    ajTableToarray(mapper->Pairs, &keyarray1, &valarray1);
    
    for(i = 0; keyarray1[i]; i++)
    {
	/*
	** Clear the second-level AJAX Tables with Ensembl Object identifiers
	** as keys and AJAX Lists as values.
	*/
	
	ajTableToarray((AjPTable) valarray1[i], &keyarray2, &valarray2);
	
	for(j = 0; keyarray2[j]; j++)
	{
	    ajTableRemove((AjPTable) valarray1[i],
			  (const void *) keyarray2[j]);
	    
	    AJFREE(keyarray2[j]);
	    
	    /*
	     ** Clear the third-level AJAX Lists of Ensembl Mapper Pairs and
	     ** delete the Ensembl Mapper Pair list elements.
	     */
	    
	    while(ajListPop((AjPList) valarray2[j], (void **) &mp))
		ensMapperPairDel(&mp);
	    
	    /* Delete the third-level AJAX Lists */
	    
	    ajListFree((AjPList *) &valarray2[j]);
	}
	
	AJFREE(keyarray2);
	AJFREE(valarray2);
    }
    
    AJFREE(keyarray1);
    AJFREE(valarray1);
    
    mapper->PairCount = 0;
    mapper->IsSorted  = ajFalse;
    
    return ajTrue;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Mapper.
**
** @fdata [EnsPMapper]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Mapper object
**
** @argrule * Pmapper [EnsPMapper*] Ensembl Mapper object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMapperDel *********************************************************
**
** Default destructor for an Ensembl Mapper.
**
** This functionclears and deletes first-level AJAX Tables with mapping
** type AJAX Strings as keys and second-level AJAX Tables as values.
** The second-level AJAX Tables with Object identifiers as keys and
** third-level AJAX Lists of Ensembl Mapping Pairs are also cleared and deleted.
** The third-level Lists of Ensembl Mapping Pairs are cleared and deleted, as
** are the Ensembl Mapper Pair list elements.
**
** @param [d] Pmapper [EnsPMapper*] Ensembl Mapper address
**
** @return [void]
** @@
******************************************************************************/

void ensMapperDel(EnsPMapper *Pmapper)
{
    void **keyarray = NULL;
    void **valarray = NULL;

    EnsPMapper pthis = NULL;
    
    register ajuint i = 0;
    
    if(!Pmapper)
	return;
    
    if(!*Pmapper)
	return;

    pthis = *Pmapper;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmapper = NULL;
	
	return;
    }
    
    ensMapperClear(pthis);
    
    ajStrDel(&pthis->SourceType);
    
    ajStrDel(&pthis->TargetType);
    
    ensCoordSystemDel(&pthis->SourceCoordSystem);
    
    ensCoordSystemDel(&pthis->TargetCoordSystem);
    
    ajTableToarray(pthis->Pairs, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	/*
	** Clear the first-level AJAX Tables with source and target mapping
	** types as keys and second-level AJAX tables as values.
	*/
	
        ajStrDel((AjPStr *) &keyarray[i]);
	
	ajTableFree((AjPTable *) &valarray[i]);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    /* Delete the first-level AJAX Table. */
    
    ajTableFree(&pthis->Pairs);
    
    AJFREE(pthis);

    *Pmapper = NULL;
    
    return;
}




/* @func ensMapperGetSourceType ***********************************************
**
** Get the source type element of an Ensembl Mapper.
**
** @cc Bio::EnsEMBL::Mapper::from
** @param [r] mapper [const EnsPMapper] Ensembl Mapper
**
** @return [AjPStr] Source type
** @@
******************************************************************************/

AjPStr ensMapperGetSourceType(const EnsPMapper mapper)
{
    if(!mapper)
	return NULL;
    
    return mapper->SourceType;
}




/* @func ensMapperGetTargetType ***********************************************
**
** Get the target type element of an Ensembl Mapper.
**
** @cc Bio::EnsEMBL::Mapper::to
** @param [r] mapper [const EnsPMapper] Ensembl Mapper
**
** @return [AjPStr] Target type
** @@
******************************************************************************/

AjPStr ensMapperGetTargetType(const EnsPMapper mapper)
{
    if(!mapper)
	return NULL;
    
    return mapper->TargetType;
}




/* @func ensMapperGetPairCount ************************************************
**
** Get the number of Ensembl Mapper Pairs in an Ensembl Mapper.
**
** @param [r] mapper [const EnsPMapper] Ensembl Mapper
**
** @return [ajuint] Number of Ensembl Mapper Pairs in an Ensembl Mapper
** @@
******************************************************************************/

ajuint ensMapperGetPairCount(const EnsPMapper mapper)
{
    if(!mapper)
	return 0;
    
    return mapper->PairCount;
}




/* @func ensMapperIsSorted ****************************************************
**
** Test whether the Ensembl Mapper Pairs are sorted in the Ensembl Mapper.
**
** @cc Bio::EnsEMBL::Mapper::_is_sorted
** @param [r] mapper [const EnsPMapper] Ensembl Mapper
**
** @return [AjBool] ajTrue if the Mapper Pairs are sorted
** @@
******************************************************************************/

AjBool ensMapperIsSorted(const EnsPMapper mapper)
{
    if(!mapper)
        return ajFalse;
    
    return mapper->IsSorted;
}




/* @funcstatic mapperCompareMapperPairSources *********************************
**
** Comparison function to sort source Ensembl Mapper Units of
** Ensembl Mapper Pairs by their Start Coordinates in ascending order.
**
** @param [r] P1 [const void*] Ensembl Mapper Pair address 1
** @param [r] P2 [const void*] Ensembl Mapper Pair address 2
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int mapperCompareMapperPairSources(const void* P1, const void* P2)
{
    int value = 0;
    
    const EnsPMapperPair mp1 = NULL;
    const EnsPMapperPair mp2 = NULL;
    
    mp1 = *(EnsPMapperPair const *) P1;
    mp2 = *(EnsPMapperPair const *) P2;
    
    /*
     ajDebug("mapperCompareMapperPairSources\n"
	     "  mp1 %p\n"
	     "  mp2 %p\n",
	     mp1,
	     mp2);
     
     ensMapperPairTrace(mp1, 1);
     
     ensMapperPairTrace(mp2, 1);
     */
    
    if(!mp1)
    {
	ajDebug("mapperCompareMapperPairSources got empty mp1.\n");
	
	return 0;
    }
    
    if(!mp2)
    {
	ajDebug("mapperCompareMapperPairSources got empty mp2.\n");
	
	return 0;
    }
    
    if(!mp1->Source)
    {
	ajDebug("mapperCompareMapperPairSources got Mapper Pair 1 without a "
		"source Mapper Unit.\n");
	
	return 0;
    }
    
    if(!mp2->Source)
    {
	ajDebug("mapperCompareMapperPairSources got Mapper Pair 2 without a "
		"source Mapper Unit.\n");
	
	return 0;
    }
    
    if(mp1->Source->Start < mp2->Source->Start)
        value = -1;
    
    if(mp1->Source->Start == mp2->Source->Start)
        value = 0;
    
    if(mp1->Source->Start > mp2->Source->Start)
        value = +1;
    
    return value;
}




/* @funcstatic mapperCompareMapperPairTargets *********************************
**
** Comparison function to sort target Ensembl Mapper Units of
** Ensembl Mapper Pairs by their Start Coordinates in ascending order.
**
** @param [r] P1 [const void*] Ensembl Mapper Pair address 1
** @param [r] P2 [const void*] Ensembl Mapper Pair address 2
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int mapperCompareMapperPairTargets(const void* P1, const void* P2)
{
    int value = 0;
    
    const EnsPMapperPair mp1 = NULL;
    const EnsPMapperPair mp2 = NULL;
    
    mp1 = *(EnsPMapperPair const *) P1;
    mp2 = *(EnsPMapperPair const *) P2;
    
    /*
     ajDebug("mapperCompareMapperPairTargets\n"
	     "  mp1 %p\n"
	     "  mp2 %p\n",
	     mp1,
	     mp2);
     
     ensMapperPairTrace(mp1, 1);
     
     ensMapperPairTrace(mp2, 1);
     */
    
    if(!mp1)
    {
	ajDebug("mapperCompareMapperPairTargets got empty mp1.\n");
	
	return 0;
    }
    
    if(!mp2)
    {
	ajDebug("mapperCompareMapperPairTargets got empty mp2.\n");
	
	return 0;
    }
    
    if(!mp1->Target)
    {
	ajDebug("mapperCompareMapperPairTargets got Mapper Pair 1 without a "
		"target Mapper Unit.\n");
	
	return 0;
    }
    
    if(!mp2->Target)
    {
	ajDebug("mapperCompareMapperPairTargets got Mapper Pair 2 without a "
		"target Mapper Unit.\n");
	
	return 0;
    }
    
    if(mp1->Target->Start < mp2->Target->Start)
        value = -1;
    
    if(mp1->Target->Start == mp2->Target->Start)
        value = 0;
    
    if(mp1->Target->Start > mp2->Target->Start)
        value = +1;
    
    return value;
}




/* @funcstatic mapperMergePairs ***********************************************
**
** Merge adjacent Ensembl Mapper Pairs in an Ensembl Mapper into one.
**
** @cc Bio::EnsEMBL::Mapper::_merge_pairs
** @param [r] mapper [EnsPMapper] Ensembl Mapper
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/* FIXME: The Perl API lacks a return statement! */

static AjBool mapperMergePairs(EnsPMapper mapper)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjIList srciter = NULL;
    AjIList trgiter = NULL;
    
    AjPList srclist = NULL;
    AjPList trglist = NULL;
    
    AjPTable srctable = NULL;
    AjPTable trgtable = NULL;
    
    EnsPMapperPair delpair  = NULL;
    EnsPMapperPair srcpair  = NULL;
    EnsPMapperPair trgpair1 = NULL;
    EnsPMapperPair trgpair2 = NULL;
    
    /*
     ajDebug("mapperMergePairs\n"
	     "  mapper %p\n",
	     mapper);
     
     ensMapperTrace(mapper, 1);
     */
    
    if(!mapper)
        return ajFalse;
    
    trgtable = (AjPTable)
	ajTableFetch(mapper->Pairs, (const void *) mapper->TargetType);
    
    ajTableToarray(trgtable, &keyarray, &valarray);
    
    for(i = 0; valarray[i]; i++)
    {
        trglist = (AjPList) valarray[i];
	
        trgiter = ajListIterNew(trglist);
	
        while(!ajListIterDone(trgiter))
        {
            trgpair1 = (EnsPMapperPair) ajListIterGet(trgiter);
	    
            trgpair2 = (EnsPMapperPair) ajListIterGet(trgiter);
	    
	    /*
	    ** If target Mapper Pair 2 is not defined, there are no more
	    ** Mapper Pairs to compare.
	    */
	    
            if((!trgpair1) || (! trgpair2))
                break;
	    
	    /* Do not merge insertion or deletion Mapper Pairs. */
	    
	    if(trgpair1->InsertionDeletion || trgpair2->InsertionDeletion)
		continue;
	    
	    assert(trgpair1->Source);
	    assert(trgpair1->Target);

	    assert(trgpair2->Source);
	    assert(trgpair2->Target);
	    
	    /* Merge overlapping Mapper Pairs. */
	    
            if((trgpair1->Source->ObjectIdentifier ==
                trgpair2->Source->ObjectIdentifier) &&
               (trgpair1->Target->Start == trgpair2->Target->Start))
                delpair = trgpair2;
	    /* Merge adjacent Mapper Pairs. */
            else if((trgpair1->Source->ObjectIdentifier ==
		      trgpair2->Source->ObjectIdentifier) &&
		     (trgpair1->Orientation == trgpair2->Orientation) &&
		     (trgpair1->Target->End == (trgpair2->Target->Start - 1)))
	    {
		if(trgpair1->Orientation >= 0)
		{
		    /* Check for a potential parallel merge. */
		    
		    if(trgpair1->Source->End == (trgpair2->Source->Start - 1))
		    {
			/* FIXME: For debugging only! */
			ajDebug("mapperMergePairs merged %p with %p "
				"in parallel orientation.\n",
				trgpair1, trgpair2);
			
			ensMapperPairTrace(trgpair1, 1);
			ensMapperPairTrace(trgpair2, 1);
			/* FIXME: End of debugging! */
			
			/* Merge in parallel orientation. */
			
			trgpair1->Source->End = trgpair2->Source->End;
			trgpair1->Target->End = trgpair2->Target->End;
			
			delpair = trgpair2;
		    }
		}
		else
		{
		    /* Check for a potential anti-parallel merge. */
		    
		    if(trgpair1->Source->Start == (trgpair2->Source->End + 1))
		    {
			/* FIXME: For debugging only! */
			ajDebug("mapperMergePairs merged %p with %p "
				"in anti-parallel orientation.\n",
				trgpair1, trgpair2);
			
			ensMapperPairTrace(trgpair1, 1);
			ensMapperPairTrace(trgpair2, 1);
			/* FIXME: End of debugging! */
			
			/* Merge in anti-parallel orientation. */
			
			trgpair1->Source->Start = trgpair2->Source->Start;
			trgpair1->Target->End = trgpair2->Target->End;
			
			delpair = trgpair2;
		    }
		}
            }
	    
	    /* Remove the redundant Mapper Pair also from the source Table. */
	    
            if(delpair)
            {
                ajListIterRemove(trgiter);
		
                srctable = (AjPTable)
		    ajTableFetch(mapper->Pairs,
				 (const void *) mapper->SourceType);
		
                srclist = (AjPList)
		    ajTableFetch(srctable,
				 (const void *)
				 &(delpair->Source->ObjectIdentifier));
		
                srciter = ajListIterNew(srclist);
		
                while(!ajListIterDone(srciter))
                {
                    srcpair = (EnsPMapperPair) ajListIterGet(srciter);
		    
                    if(srcpair == delpair)
		    {
                        ajListIterRemove(srciter);
			
			ensMapperPairDel(&srcpair);
		    }
                }
		
                ajListIterDel(&srciter);
		
		ensMapperPairDel(&delpair);
            }
        }
	
        ajListIterDel(&trgiter);
	
	mapper->PairCount = ajListGetLength(trglist);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return ajTrue;
}




/* @funcstatic mapperSort *****************************************************
**
** @cc Bio::EnsEMBL::Mapper::_sort
** Sort Ensembl Mapper Pairs in an Ensembl Mapper.
**
** @param [r] mapper [EnsPMapper] Ensembl Mapper
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/* FIXME: The Perl API lacks a return statement! */

static AjBool mapperSort(EnsPMapper mapper)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjPTable table = NULL;
    
    /*
     ajDebug("mapperSort\n"
	     "  mapper %p\n",
	     mapper);
     
     ensMapperTrace(mapper, 1);
     */
    
    if(!mapper)
        return ajFalse;
    
    table = (AjPTable)
	ajTableFetch(mapper->Pairs, (const void *) mapper->SourceType);
    
    ajTableToarray(table, &keyarray, &valarray);
    
    for(i = 0; valarray[i]; i++)
	ajListSort((AjPList) valarray[i], mapperCompareMapperPairSources);
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    table = (AjPTable)
	ajTableFetch(mapper->Pairs, (const void *) mapper->TargetType);
    
    ajTableToarray(table, &keyarray, &valarray);
    
    for(i = 0; valarray[i]; i++)
	ajListSort((AjPList) valarray[i], mapperCompareMapperPairTargets);
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    mapperMergePairs(mapper);
    
    mapper->IsSorted = ajTrue;
    
    return ajTrue;
}




/* @func ensMapperAddCoordinates **********************************************
**
** Store details of mapping between a source and a target region.
**
** @cc Bio::EnsEMBL::Mapper::add_map_coordinates
** @param [u] mapper [EnsPMapper] Ensembl Mapper
** @param [r] srcoid [ajuint] Source Object identifier
** @param [r] srcstart [ajint] Source start coordinate
** @param [r] srcend [ajint] Source end coordinate
** @param [r] ori [ajint] Orientation
** @param [r] trgoid [ajuint] Target Object identifier
** @param [r] trgstart [ajint] Target start coordinate
** @param [r] trgend [ajint] Target end coordinate
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: The Perl API documentation is not in sync with the code.
** "Returns 1 if the pair was added, 0 if it was already in."
** This is however not reflected in the function as a return statement
** is completely missing!
**
** The Perl API does some expensive pattern matching exercises to subsitute
** digits with noting. The test however is not evaluated later.
*/

AjBool ensMapperAddCoordinates(EnsPMapper mapper,
                               ajuint srcoid,
                               ajint srcstart,
                               ajint srcend,
                               ajint ori,
                               ajuint trgoid,
                               ajint trgstart,
                               ajint trgend)
{
    AjBool value = AJFALSE;
    
    EnsPMapperPair mp = NULL;
    
    /*
     ajDebug("ensMapperAddCoordinates\n"
	     "  mapper %p\n"
	     "  srcoid %u\n"
	     "  srcstart %d\n"
	     "  srcend %d\n"
	     "  ori %d\n"
	     "  trgoid %u\n"
	     "  trgstart %d\n"
	     "  trgend %d\n",
	     mapper,
	     srcoid,
	     srcstart,
	     srcend,
	     ori,
	     trgoid,
	     trgstart,
	     trgend);
     */
    
    if(!mapper)
        return ajFalse;
    
    if(!srcoid)
        return ajFalse;
    
    if(!trgoid)
        return ajFalse;
    
    if((srcend - srcstart) != (trgend - trgstart))
        ajFatal("ensMapperAddCoordinates cannot deal with mis-lengthed "
                "mappings so far.\n");
    
    mp = ensMapperPairNewC(srcoid, srcstart, srcend,
			   trgoid, trgstart, trgend,
			   ori,
			   ajFalse);
    
    value = ensMapperAddMapperPair(mapper, mp);
    
    ensMapperPairDel(&mp);
    
    return value;
}




/* @func ensMapperAddMapperUnits **********************************************
**
** Insert Ensembl Mapper Units into an Ensembl Mapper.
**
** @param [u] mapper [EnsPMapper] Ensembl Mapper
** @param [u] srcmu [EnsPMapperUnit] Source Ensembl Mapper Unit
** @param [u] trgmu [EnsPMapperUnit] Target Ensembl Mapper Unit
** @param [r] ori [ajint] Relative orientation of the Ensembl Mapper Units
** @param [r] indel [AjBool] Insertion-deletion attribute
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperAddMapperUnits(EnsPMapper mapper,
                               EnsPMapperUnit srcmu,
                               EnsPMapperUnit trgmu,
                               ajint ori,
                               AjBool indel)
{
    AjBool value = AJFALSE;
    
    EnsPMapperPair mp = NULL;
    
    if(!mapper)
	return ajFalse;
    
    if(!srcmu)
	return ajFalse;
    
    if(!trgmu)
	return ajFalse;
    
    mp = ensMapperPairNew(srcmu, trgmu, ori, indel);
    
    value = ensMapperAddMapperPair(mapper, mp);
    
    ensMapperPairDel(&mp);
    
    return value;
}




/* @func ensMapperAddMapperPair ***********************************************
**
** Insert an Ensembl Mapper Pair into an Ensembl Mapper.
**
** @param [u] mapper [EnsPMapper] Ensembl Mapper
** @param [u] mp [EnsPMapperPair] Ensembl Mapper Pair
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperAddMapperPair(EnsPMapper mapper, EnsPMapperPair mp)
{
    ajuint *Poid = NULL;
    
    AjPList list = NULL;
    
    AjPTable table = NULL;
    
    /*
     ajDebug("ensMapperAddMapperPair\n"
	     "  mapper %p\n"
	     "  mp %p\n",
	     mapper,
	     mp);
     
     ensMapperPairTrace(mp, 1);
     */
    
    if(!mapper)
        return ajFalse;
    
    if(!mp)
        return ajFalse;
    
    if(!mp->Source)
	ajFatal("ensMapperAddMapperPair requires a Mapper Pair with a "
		"Source Mapper Unit.\n");
    
    if(!mp->Target)
	ajFatal("ensMapperAddMapperPair requires a Mapper Pair with a "
		"Target Mapper Unit.\n");
    
    /* FIXME: The test for mis-matching lengths is not performed on InDels. */
    
    if((!mp->InsertionDeletion) &&
	((mp->Source->End - mp->Source->Start) !=
	 (mp->Target->End - mp->Target->Start)))
        ajFatal("ensMapperAddMapperPair cannot deal with mis-lengthed "
                "mappings so far.\n");
    
    /*
    ** Insert the Mapper Pair into the Source branch of the Mapper.
    **
    ** Search the first-level AJAX Table of Ensembl Mapper type Strings for
    ** the second-level AJAX Table of Ensembl Object Identifers.
    */
    
    table = (AjPTable)
	ajTableFetch(mapper->Pairs, (const void *) mapper->SourceType);
    
    if(table)
    {
	/*
	** Search the second-level AJAX Table of Ensembl Object identifiers
	** for the third-level AJAX List of Ensembl Mapper Pairs.
	*/
	
        list = (AjPList)
	ajTableFetch(table, (const void *) &(mp->Source->ObjectIdentifier));
	
        if(!list)
        {
            AJNEW0(Poid);
	    
            *Poid = mp->Source->ObjectIdentifier;
	    
            list = ajListNew();
	    
            ajTablePut(table, (void *) Poid, (void *) list);
	}
	
	ajListPushAppend(list, (void *) ensMapperPairNewRef(mp));
    }
    else
        ajFatal("ensMapperAddMapperPair first-level AJAX Table for "
		"Ensembl Mapper Source type '%S' has not been initialised.",
		mapper->SourceType);
    
    /*
    ** Insert the Mapper Pair into the Target branch of the Mapper.
    **
    ** Search the first-level AJAX Table of Ensembl Mapper type Strings for
    ** the second-level AJAX Table of Ensembl Object Identifers.
    */
    
    table = (AjPTable)
	ajTableFetch(mapper->Pairs, (const void *) mapper->TargetType);
    
    if(table)
    {
	/*
	** Search the second-level AJAX Table of Ensembl Object identifiers
	** for the third-level AJAX List of Ensembl Mapper Pairs.
	*/
	
        list = (AjPList)
	ajTableFetch(table, (const void *) &(mp->Target->ObjectIdentifier));
	
        if(!list)
        {
            AJNEW0(Poid);
	    
            *Poid = mp->Target->ObjectIdentifier;
	    
            list = ajListNew();
	    
            ajTablePut(table, (void *) Poid, (void *) list);
        }
	
	ajListPushAppend(list, (void *) ensMapperPairNewRef(mp));
    }
    else
        ajFatal("ensMapperAddMapperPair first-level AJAX Table for "
		"Ensembl Mapper Target type '%S' has not been initialised.",
		mapper->TargetType);
    
    mapper->PairCount++;
    
    mapper->IsSorted = ajFalse;
    
    return ajTrue;
}




/* @func ensMapperListPairs ***************************************************
**
** List Ensembl Mapper Pairs in an Ensembl Mapper.
** The caller is responsible for deleting the Ensembl Mapper Pairs before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Mapper::list_pairs
** @param [r] mapper [EnsPMapper] Ensembl Mapper
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] type [const AjPStr] Ensembl Mapper type
** @param [u] mps [AjPList] AJAX List of Ensembl Mapper Pairs
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: The Perl API Pdoc states that type is an integer. It is however
** a string defining either the Source or Target Ensembl Mapper Type.
**
** Error message suggests coordinates start.end.id.type but the order is
** id, start, end, type!
*/

AjBool ensMapperListPairs(EnsPMapper mapper,
                          ajuint oid,
                          ajint start,
                          ajint end,
                          const AjPStr type,
                          AjPList mps)
{
    AjEnum mutype = ensEMapperUnitTypeNULL;
    
    AjPList list = NULL;
    AjIList iter = NULL;
    
    AjPTable table = NULL;
    
    EnsPMapperPair mp = NULL;
    
    EnsPMapperUnit mu = NULL;
    
    if(!mapper)
        return ajFalse;
    
    if(!oid)
        return ajFalse;
    
    if(!type)
        return ajFalse;
    
    if(!mapper->IsSorted)
        mapperSort(mapper);
    
    if(start > end)
        ajFatal("ensMapperListPairs start %d is greater than end %d "
                "for Ensembl Object identifier %u.\n",
		start, end, oid);
    
    if(ajStrMatchCaseS(mapper->SourceType, type))
        mutype = ensEMapperUnitTypeSource;
    else if(ajStrMatchCaseS(mapper->TargetType, type))
	mutype = ensEMapperUnitTypeTarget;
    else
	ajFatal("ensMapperListPairs type '%S' is neither source '%S' nor "
		"target '%S' type of the Ensembl Mapper.\n",
		type, mapper->SourceType, mapper->TargetType);
    
    table = (AjPTable) ajTableFetch(mapper->Pairs, (const void *) type);
    
    if(!table)
        ajFatal("ensMapperListPairs first-level AJAX Table for "
		"Ensembl Mapper type '%S' has not been initialised.",
		type);
    
    list = (AjPList) ajTableFetch(table, (const void *) &oid);
    
    if(!list)
        return ajTrue;
    
    iter = ajListIterNew(list);
    
    while(!ajListIterDone(iter))
    {
        mp = (EnsPMapperPair) ajListIterGet(iter);
	
        if((!start) && (! end))
        {
            ajListPushAppend(mps, (void *) ensMapperPairNewRef(mp));
	    
            continue;
        }
	
	mu = ensMapperPairGetUnit(mp, mutype);
	
	if(mu->End < start)
	    continue;
	
	if(mu->Start > end)
	    break;
	
        ajListPushAppend(mps, (void *) ensMapperPairNewRef(mp));
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @funcstatic mapperMapInsert ************************************************
**
** Internal function to handle the special mapping case for inserts, where by
** Ensembl convention (start == end + 1). This function will be called
** automatically by the map function so there is no reason to call it directly.
** The caller is responsible for deleting the Ensembl Mapper Results before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Mapper::map_insert
** @param [u] mapper [EnsPMapper] Ensembl Mapper
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [r] type [const AjPStr] Ensembl Mapper source type
** @param [r] fastmap [AjBool] Fast-mapping attribute
** @param [u] mrs [AjPList] AJAX list of Ensembl Mapper Results
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool mapperMapInsert(EnsPMapper mapper,
                              ajuint oid,
                              ajint start,
                              ajint end,
                              ajint strand,
                              const AjPStr type,
                              AjBool fastmap,
                              AjPList mrs)
{
    ajint srcstart = 0;
    ajint srcend   = 0;
    
    AjPList coords = NULL;
    
    EnsPMapperResult mr = NULL;
    
    if(!mapper)
	return ajFalse;
    
    if(!oid)
	return ajFalse;
    
    ajDebug("mapperMapInsert\n"
	    "  mapper %p\n"
	    "  oid %u\n"
	    "  start %d\n"
	    "  end %d\n"
	    "  strand %d\n"
	    "  type '%S'\n"
	    "  fastmap '%B'\n"
	    "  mrs %p\n",
	    mapper,
	    oid,
	    start,
	    end,
	    strand,
	    type,
	    fastmap,
	    mrs);
    
    srcstart = end;
    srcend   = start;
    
    coords = ajListNew();
    
    ensMapperMapCoordinates(mapper, oid, srcstart, srcend, strand,
			    type, coords);
    
    if(ajListGetLength(coords) == 1)
    {
	ajListPop(coords, (void **) &mr);
	
	/*
	** Swap start and end to convert back into an insert where
	** (start == end + 1)
	*/
	
	srcstart = mr->End;
	srcend   = mr->Start;
	
	mr->Start = srcstart;
	mr->End   = srcend;
	
	ajListPushAppend(mrs, (void *) ensMapperResultNewRef(mr));
	
	ensMapperResultDel(&mr);
    }
    else
    {
	if(ajListGetLength(coords) != 2)
	    ajFatal("mapperMapInsert got %d Ensembl Mapper Pairs "
		    "but expected only two.\n",
		    ajListGetLength(coords));
	
	/* Adjust coordinates and remove gaps. */
	
	if(strand < 0)
	    ajListReverse(coords);
	
	ajListPop(coords, (void **) &mr);
	
	if(mr->Type == ensEMapperResultCoordinate)
	{
	    /* The insert is after the first coordinate. */
	    
	    if((mr->Strand * strand) < 0)
		mr->End--;
	    else
		mr->Start++;
	    
	    ajListPushAppend(mrs, (void *) ensMapperResultNewRef(mr));
	}
	
	ensMapperResultDel(&mr);
	
	ajListPop(coords, (void **) &mr);
	
	if(mr->Type == ensEMapperResultCoordinate)
	{
	    /* The insert is before the second coordinate. */
	    
	    if((mr->Strand * strand) < 0)
		mr->Start++;
	    else
		mr->End++;
	    
	    if(strand < 0)
		ajListPush(mrs, (void *) ensMapperResultNewRef(mr));
	    else
		ajListPushAppend(mrs, (void *) ensMapperResultNewRef(mr));
	}
	
	ensMapperResultDel(&mr);
    }
    
    if(fastmap && (ajListGetLength(mrs) != 1))
	while(ajListPop(mrs, (void **) &mr))
	    ensMapperResultDel(&mr);
    
    ajListFree(&coords);
    
    return ajTrue;
}




/* @func ensMapperMapCoordinates **********************************************
**
** Map Coordinates.
** The caller is responsible for deleting the Ensembl Mapper Results before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Mapper::map_coordinates
** @param [u] mapper [EnsPMapper] Ensembl Mapper.
** @param [r] oid [ajuint] Ensembl Object identifier.
** @param [r] start [ajint] Start coordinate.
** @param [r] end [ajint] End coordinate.
** @param [r] strand [ajint] Strand information.
** @param [r] type [const AjPStr] Ensembl Mapper type to map from.
** @param [u] mrs [AjPList] AJAX list of Ensembl Mapper Results.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

/*
** FIXME: The Perl API documents type as int, which should be a string.
** The initial function parameter testing is dodgy. The error message suggests
** a different parameter order than is actually required.
*/

AjBool ensMapperMapCoordinates(EnsPMapper mapper,
                               ajuint oid,
                               ajint start,
                               ajint end,
                               ajint strand,
                               const AjPStr type,
                               AjPList mrs)
{
    register ajuint i = 0;
    
    ajuint length = 0;
    
    /* Binary search indices range from 0 to list length - 1 */
    
    ajuint idxstart = 0;
    ajuint idxmid   = 0;
    ajuint idxend   = 0;
    
    ajint srcstart = 0;
    ajint srcend = 0;
    
    ajuint trgoid  = 0;
    ajint trgstart = 0;
    ajint trgend   = 0;
    
    AjEnum srctype = ensEMapperUnitTypeNULL;
    AjEnum trgtype = ensEMapperUnitTypeNULL;
    
    AjPList list = NULL;
    
    AjPTable table = NULL;
    
    EnsPCoordSystem cs = NULL;
    
    EnsPMapperPair mp     = NULL;
    EnsPMapperPair lastmp = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPMapperUnit srcmu = NULL;
    EnsPMapperUnit trgmu = NULL;
    
    /*
     ajDebug("ensMapperMapCoordinates\n"
	     "  mapper %p\n"
	     "  oid %u\n"
	     "  start %d\n"
	     "  end %d\n"
	     "  strand %d\n"
	     "  type '%S'\n"
	     "  mrs %p\n",
	     mapper,
	     oid,
	     start,
	     end,
	     strand,
	     type,
	     mrs);
     */
    
    if(!mapper)
        return ajFalse;
    
    if(!oid)
        return ajFalse;
    
    if(!mrs)
        return ajFalse;
    
    if(start == (end + 1))
        return mapperMapInsert(mapper, oid, start, end, strand, type,
                               ajFalse, mrs);
    
    if(!mapper->IsSorted)
        mapperSort(mapper);
    
    if(ajStrMatchCaseS(mapper->SourceType, type))
    {
        srctype = ensEMapperUnitTypeSource;
	trgtype = ensEMapperUnitTypeTarget;
	
	cs = mapper->TargetCoordSystem;
    }
    else if(ajStrMatchCaseS(mapper->TargetType, type))
    {
	srctype = ensEMapperUnitTypeTarget;
	trgtype = ensEMapperUnitTypeSource;
	
	cs = mapper->SourceCoordSystem;
    }
    else
	ajFatal("ensMapperMapCoordinates type '%S' is neither "
		"source '%S' nor "
		"target '%S' type of the Ensembl Mapper.\n",
		type,
		mapper->SourceType,
		mapper->TargetType);
    
    table = (AjPTable) ajTableFetch(mapper->Pairs, (const void *) type);
    
    if(!table)
        ajFatal("ensMapperMapCoordinates first-level AJAX Table for "
		"Ensembl Mapper type '%S' has not been initialised.\n",
		type);
    
    list = (AjPList) ajTableFetch(table, (const void *) &oid);
    
    if(!list)
    {
	ajDebug("ensMapperMapCoordinates could not find an AJAX List for "
		"Ensembl Object identifier %u --> one big gap!\n",
		oid);
	
        /* Just one big gap! */
	
        mr = MENSMAPPERGAPNEW(start, end);
	
        ajListPushAppend(mrs, (void *) mr);
	
	return ajTrue;
    }
    
    length = ajListGetLength(list);
    
    idxstart = 0;
    
    idxend = length - 1;
    
    /*
    ** Binary search the relevant Ensembl Mapper Pairs,
    ** which helps if the List is big.
    */
    
    while((idxend - idxstart) > 1)
    {
        idxmid = (idxstart + idxend) >> 1;
	
        ajListPeekNumber(list, idxmid, (void **) &mp);
	
	srcmu = ensMapperPairGetUnit(mp, srctype);
	
	if(srcmu->End < start)
            idxstart = idxmid;
	
        else
            idxend = idxmid;
    }
    
    srcstart = start;
    
    srcend = end;
    
    for(i = idxstart; i < length; i++)
    {
        ajListPeekNumber(list, i, (void **) &mp);
	
        srcmu = ensMapperPairGetUnit(mp, srctype);
	
	trgmu = ensMapperPairGetUnit(mp, trgtype);
	
	/* FIXME: For debugging only.
	    ajDebug("ensMapperMapCoordinates coordinates "
		    "%u:%d:%d:%d %d:%d srcMU %u:%d:%d\n",
		    oid,
		    start,
		    end,
		    strand,
		    srcstart,
		    srcend,
		    srcmu->ObjectIdentifier,
		    srcmu->Start,
		    srcmu->End);
	*/
	
	/*
	** Check for cases where the source Mapper Unit maps to more than one
	** location.
	** If the target object identifier changes, and the source start
	** coordinate is less than the current start coordinate, this source
	** Mapper Unit maps to more than one location on a target Mapper Unit.
	** Reset the current source start position to the original start.
	*/
	
	if(trgoid && (trgoid != trgmu->ObjectIdentifier))
	{
	    if(srcmu->Start < start)
		srcstart = start;
	}
	else
	    trgoid = trgmu->ObjectIdentifier;
	
	/* In case the loop hasn't even reached the start, move on. */
	
	if(srcmu->End < start)
            continue;
	
        /* In case the loop has over-run, break. */
	
        if(srcmu->Start > srcend)
            break;
	
        if(srcmu->Start > srcstart)
        {
	    /* A gap has been detected. */
	    
            mr = MENSMAPPERGAPNEW(srcstart, srcmu->Start - 1);
	    
            ajListPushAppend(mrs, (void *) mr);
	    
            srcstart = srcmu->Start;
        }
	
        if(mp->InsertionDeletion)
        {
            /*
	    ** If the Mapper Pair represents an Insertion or Deletion,
	    ** create a Mapper Result of type Insertion or Deletion.
	    */
	    
	    mr = MENSMAPPERINDELNEW(trgmu->ObjectIdentifier,
				    trgmu->Start,
				    trgmu->End,
				    mp->Orientation * strand,
				    cs,
				    srcstart,
				    (srcmu->End < srcend) ?
				    srcmu->End : srcend);
        }
	else
	{
	    /* The start is somewhere inside the region. */
	    
	    if(mp->Orientation >= 0)
		trgstart = trgmu->Start + (srcstart - srcmu->Start);
	    else
		trgend = trgmu->End - (srcstart - srcmu->Start);
	    
	    /*
	    ** Either we are enveloping this map or not. If yes, then the end
	    ** point (self perspective) is determined solely by target.
	    ** If not we need to adjust.
	    */
	    
	    if(srcend > srcmu->End)
	    {
		/* enveloped */
		
		if(mp->Orientation >= 0)
		    trgend = trgmu->End;
		else
		    trgstart = trgmu->Start;
	    }
	    else
	    {
		/* The end needs to be adjusted. */
		
		if(mp->Orientation >= 0)
		    trgend = trgmu->Start + (srcend - srcmu->Start);
		else
		    trgstart = trgmu->End - (srcend - srcmu->Start);
	    }
	    
	    mr = MENSMAPPERCOORDINATENEW(trgmu->ObjectIdentifier,
					 trgstart,
					 trgend,
					 mp->Orientation * strand,
					 cs);
	}
	
	ajListPushAppend(mrs, (void *) mr);
	
	lastmp = mp;
	
	srcstart = srcmu->End + 1;
    }
    
    if(lastmp)
    {
	/*
	 ** Previously, a Mapper Pair has been found,
	 ** check for a gap inbetween.
	 */
	
	srcmu = ensMapperPairGetUnit(lastmp, srctype);
	
	if(srcmu->End < srcend)
	{
	    /* A gap at the end has been detected. */
	    
	    mr = MENSMAPPERGAPNEW(srcmu->End + 1, srcend);
	    
	    ajListPushAppend(mrs, (void *) mr);
	}
    }
    else
    {
	/* Since no Mapper Pair has been found the entire region is a gap. */
	
	mr = MENSMAPPERGAPNEW(srcstart, srcend);
	
	ajListPushAppend(mrs, (void *) mr);
    }
    
    if(strand < 0)
	ajListReverse(mrs);
    
    return ajTrue;
}




/* @func ensMapperFastMap *****************************************************
**
** Inferior mapping function, which will only perform ungapped,
** unsplit mapping.
** The caller is responsible for deleting the Ensembl Mapper Results before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Mapper::fastmap
** @param [u] mapper [EnsPMapper] Ensembl Mapper
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [r] type [const AjPStr] Ensembl Mapper type to map from
** @param [u] mrs [AjPList] AJAX list of Ensembl Mapper Results
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperFastMap(EnsPMapper mapper,
                        ajuint oid,
                        ajint start,
                        ajint end,
                        ajint strand,
                        const AjPStr type,
                        AjPList mrs)
{
    AjEnum srctype = ensEMapperUnitTypeNULL;
    AjEnum trgtype = ensEMapperUnitTypeNULL;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    AjPTable table = NULL;
    
    EnsPCoordSystem cs = NULL;
    
    EnsPMapperPair mp = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPMapperUnit srcmu = NULL;
    EnsPMapperUnit trgmu = NULL;
    
    if(!mapper)
	return ajFalse;
    
    if(!oid)
	return ajFalse;
    
    if(start == (end + 1))
	return mapperMapInsert(mapper, oid, start, end, strand, type,
			       ajTrue, mrs);
    
    if(!mapper->IsSorted)
	mapperSort(mapper);
    
    if(ajStrMatchCaseS(mapper->SourceType, type))
    {
        srctype = ensEMapperUnitTypeSource;
	trgtype = ensEMapperUnitTypeTarget;
        
	cs = mapper->TargetCoordSystem;
    }
    
    else if(ajStrMatchCaseS(mapper->TargetType, type))
    {
        srctype = ensEMapperUnitTypeTarget;
	trgtype = ensEMapperUnitTypeSource;
	
	cs = mapper->SourceCoordSystem;
    }
    else
	ajFatal("ensMapperFastMap type '%S' is neither "
		"source '%S' nor "
		"target '%S' type of the Ensembl Mapper.\n",
		type,
		mapper->SourceType,
		mapper->TargetType);
    
    table = (AjPTable) ajTableFetch(mapper->Pairs, (const void *) type);
    
    if(!table)
        ajFatal("ensMapperFastMap first-level AJAX Table for "
		"Ensembl Mapper type '%S' has not been initialised.\n",
		type);
    
    list = (AjPList) ajTableFetch(table, (const void *) &oid);
    
    iter = ajListIterNew(list);
    
    while(!ajListIterDone(iter))
    {
	mp = (EnsPMapperPair) ajListIterGet(iter);
	
        srcmu = ensMapperPairGetUnit(mp, srctype);
	
	trgmu = ensMapperPairGetUnit(mp, trgtype);
	
	/* Only super easy mapping is done! */
	
	if((start < srcmu->Start) || (end > srcmu->End))
	    continue;
	
	if(mp->Orientation >= 0)
	    mr = MENSMAPPERCOORDINATENEW(trgmu->ObjectIdentifier,
					 trgmu->Start + (start - srcmu->Start),
					 trgmu->Start + (end - srcmu->Start),
					 strand,
					 cs);
	else
	    mr = MENSMAPPERCOORDINATENEW(trgmu->ObjectIdentifier,
					 trgmu->End - (end - srcmu->Start),
					 trgmu->End - (start - srcmu->Start),
					 - strand,
					 cs);
	
	ajListPushAppend(mrs, (void *) mr);
	
	break;
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensMapperAddInDelCoordinates *****************************************
**
** Store details of mapping between a source and a target region.
**
** @cc Bio::EnsEMBL::Mapper::add_indel_coordinates
** @param [u] mapper [EnsPMapper] Ensembl Mapper
** @param [r] srcoid [ajuint] Source Object identifier
** @param [r] srcstart [ajint] Source start coordinate
** @param [r] srcend [ajint] Source end coordinate
** @param [r] ori [ajint] Orientation
** @param [r] trgoid [ajuint] Target Object identifier
** @param [r] trgstart [ajint] Target start coordinate
** @param [r] trgend [ajint] Target end coordinate
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperAddInDelCoordinates(EnsPMapper mapper,
                                    ajuint srcoid,
                                    ajint srcstart,
                                    ajint srcend,
                                    ajint ori,
                                    ajuint trgoid,
                                    ajint trgstart,
                                    ajint trgend)
{
    AjBool value = AJFALSE;
    
    EnsPMapperPair mp = NULL;
    
    if(!mapper)
	return ajFalse;
    
    if(!srcoid)
	return ajFalse;
    
    if(!trgoid)
	return ajFalse;
    
    mp = ensMapperPairNewC(srcoid, srcstart, srcend,
			   trgoid, trgstart, trgend,
			   ori, ajTrue);
    
    value = ensMapperAddMapperPair(mapper, mp);
    
    ensMapperPairDel(&mp);
    
    return value;
}




/* @func ensMapperMapInDel ****************************************************
**
** Map InDel Coordinates.
** The caller is responsible for deleting the Ensembl Mapper Results before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Mapper::map_indel
** @param [u] mapper [EnsPMapper] Ensembl Mapper
** @param [r] oid [ajuint] Ensembl Object identifier
** @param [r] start [ajint] Start coordinate
** @param [r] end [ajint] End coordinate
** @param [r] strand [ajint] Strand information
** @param [r] type [const AjPStr] Ensembl Mapper type to map from
** @param [u] mrs [AjPList] AJAX list of Ensembl Mapper Results
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: The Perl API defines a last_used_pair, which isn't really used!
** This function seems to return a list of Bio::EnsEMBL::Mapper::Unit
** objects rather than Bio::EnsEMBL::Mapper::IndelCoordinate objects?
** The Perl API uses a $self_coord variable in the last loop, which is
** not used.
*/

AjBool ensMapperMapInDel(EnsPMapper mapper,
                         ajuint oid,
                         ajint start,
                         ajint end,
                         ajint strand,
                         const AjPStr type,
                         AjPList mrs)
{
    register ajuint i = 0;
    
    ajuint length = 0;
    
    /* Binary search indices range from 0 to list length - 1 */
    
    ajuint idxstart = 0;
    ajuint idxend   = 0;
    ajuint idxmid   = 0;
    
    ajint srcstart = 0;
    ajint srcend   = 0;
    
    AjEnum srctype = ensEMapperUnitTypeNULL;
    AjEnum trgtype = ensEMapperUnitTypeNULL;
    
    AjPList list = NULL;
    
    AjPTable table = NULL;
    
    EnsPCoordSystem cs = NULL;
    
    EnsPMapperPair mp = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPMapperUnit srcmu = NULL;
    EnsPMapperUnit trgmu = NULL;
    
    ajDebug("ensMapperMapInDel\n"
	    "  mapper %p\n"
	    "  oid %u\n"
	    "  start %u\n"
	    "  end %u\n"
	    "  strand %d\n"
	    "  type '%S'"
	    "  mrs %p\n",
	    mapper,
	    oid,
	    start,
	    end,
	    strand,
	    type,
	    mrs);
    
    if(!mapper)
        return ajFalse;
    
    if(!oid)
        return ajFalse;
    
    if(!mrs)
        return ajFalse;
    
    /* Swap start and end and map the resulting 2 base pair coordinate. */
    
    srcstart = end;
    
    srcend = start;
    
    if(!mapper->IsSorted)
        mapperSort(mapper);
    
    if(ajStrMatchCaseS(mapper->SourceType, type))
    {
        srctype = ensEMapperUnitTypeSource;
	trgtype = ensEMapperUnitTypeTarget;
	
	cs = mapper->TargetCoordSystem;
    }
    else if(ajStrMatchCaseS(mapper->TargetType, type))
    {
	srctype = ensEMapperUnitTypeTarget;
	trgtype = ensEMapperUnitTypeSource;
	
	cs = mapper->SourceCoordSystem;
    }
    else
	ajFatal("ensMapperMapInDel type '%S' is neither "
		"source '%S' nor "
		"target '%S' type of the Ensembl Mapper.\n",
		type,
		mapper->SourceType,
		mapper->TargetType);
    
    table = (AjPTable) ajTableFetch(mapper->Pairs, (const void *) type);
    
    if(!table)
        ajFatal("ensMapperMapInDel first-level AJAX Table for "
		"Ensembl Mapper type '%S' has not been initialised.",
		type);
    
    list = (AjPList) ajTableFetch(table, (const void *) &oid);
    
    if(!list)
    {
	ajFatal("ensMapperMapInDel could not find an AJAX List for "
		"Ensembl Object identifier %u!\n",
		oid);
	
	/*
	** FIXME: What happens if the AJAX List does not exist?
	** The ensMapperMapCoordinates function inserts just one big gap.
	 mr = MENSMAPPERGAPNEW(srcstart, srcend);
	 
	 ajListPushAppend(mrs, (void *) mr);
	 
	 return ajTrue;
	*/
    }
    
    length = ajListGetLength(list);
    
    idxstart = 0;
    
    idxend = length - 1;
    
    /*
    ** Binary search the relevant Ensembl Mapper Pairs,
    ** which helps if the List is big.
    */
    
    while((idxend - idxstart) > 1)
    {
	idxmid = (idxstart + idxend) >> 1;
	
	ajListPeekNumber(list, idxmid, (void **) &mp);
	
	srcmu = ensMapperPairGetUnit(mp, srctype);
	
        /* FIXME: Difference to ensMapperMapCoordinates. */
	if(srcmu->End <= srcstart)
            idxstart = idxmid;
        else
            idxend = idxmid;
    }
    
    for(i = idxstart; i < length; i++)
    {
	ajListPeekNumber(list, i, (void **) &mp);
	
        /*
	** FIXME: This is defined by the Perl API but not needed.
	** srcmu = ensMapperPairGetUnit(mp, srctype);
	*/
	
	trgmu = ensMapperPairGetUnit(mp, trgtype);
	
	if(mp->InsertionDeletion)
	{
	    /*
	    ** FIXME: The Perl API pushes MapperUnits onto the list of
	    ** MapperResults as of CVS revision 1.39, which still can't
	    ** be right.
	    **
	    ** This seems only used in Bio::EnsEMBL::AlignStrainSlice???
	    ** 
	    **  mr = MENSMAPPERINDELNEW(trgmu->ObjectIdentifier,
	    **			     trgmu->Start,
	    **			     trgmu->End,
	    **			     mp->Orientation * strand,
	    **			     cs,
	    **			     gstart???,
	    **			     gend???);
	    */
	    
	    mr = MENSMAPPERCOORDINATENEW(trgmu->ObjectIdentifier,
					 trgmu->Start,
					 trgmu->End,
					 mp->Orientation * strand,
					 cs);
	    
	    ajListPushAppend(mrs, (void *) mr);
	    
	    break;
	}
    }
    
    return ajTrue;
}




/* @func ensMapperAddMapper ***************************************************
**
** Transfer all Ensembl Mapper Pairs from the second into the first
** Ensembl Mapper.
**
** @cc Bio::EnsEMBL::Mapper::add_Mapper
** @param [u] mapper1 [EnsPMapper] First Ensembl Mapper
** @param [u] mapper2 [EnsPMapper] Second Ensembl Mapper
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperAddMapper(EnsPMapper mapper1, EnsPMapper mapper2)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    ajuint srccounter = 0;
    ajuint trgcounter = 0;
    
    AjPList list1 = NULL;
    AjPList list2 = NULL;
    
    AjPTable table1 = NULL;
    AjPTable table2 = NULL;
    
    EnsPMapperPair mp = NULL;
    
    if(!mapper1)
	return ajFalse;
    
    if(!mapper2)
	return ajFalse;
    
    if(!ajStrMatchCaseS(mapper1->SourceType, mapper2->SourceType) ||
       ! ajStrMatchCaseS(mapper1->TargetType, mapper2->TargetType))
	ajFatal("ensMapperAddMapper trying to add Ensembl Mapper of "
		"incompatible type. '%S:%S' vs '%S:%S'",
		mapper1->SourceType,
		mapper1->TargetType,
		mapper2->SourceType,
		mapper2->TargetType);
    
    /*
    ** Get the first-level AJAX Tables for the source type of both
    ** Ensembl Mappers.
    */
    
    table1 = (AjPTable)
	ajTableFetch(mapper1->Pairs, (const void *) mapper1->SourceType);
    
    if(!table1)
        ajFatal("ensMapperAddMapper first-level AJAX table for first "
		"Ensembl Mapper source type '%S' not initialised.",
		mapper1->SourceType);
    
    table2 = (AjPTable)
	ajTableFetch(mapper2->Pairs, (const void *) mapper2->SourceType);
    
    if(!table2)
        ajFatal("ensMapperAddMapper first-level AJAX table for second "
		"Ensembl Mapper source type '%S' not initialised.",
		mapper2->SourceType);
    
    /*
    ** Convert the second-level AJAX Table with Ensembl Object
    ** identifiers as key data and AJAX List value data for the second
    ** Ensembl Mapper.
    */
    
    ajTableToarray(table2, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	/*
	** Get the third-level AJAX Lists for corresponding
	** Ensembl Object identifiers.
	*/
	
	list1 = (AjPList) ajTableFetch(table1, (const void *) &keyarray[i]);
	
	list2 = (AjPList) valarray[i];
	
	while(ajListPop(list2, (void **) &mp))
	{
	    ajListPushAppend(list1, (void *) mp);
	    
	    srccounter++;
	}
	
	/*
	** Remove the entry from the AJAX Table and free the
	** Object identifier and the list.
	*/
	
	ajTableRemove(table2, (const void *) keyarray[i]);
	
	AJFREE(keyarray[i]);
	
	ajListFree(&list2);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    /*
    ** Get the first-level AJAX Tables for the target type of both
    ** Ensembl Mappers.
    */
    
    table1 = (AjPTable)
	ajTableFetch(mapper1->Pairs, (const void *) mapper1->TargetType);
    
    if(!table1)
        ajFatal("ensMapperAddMapper first-level AJAX table for first "
		"Ensembl Mapper target type '%S' not initialised.",
		mapper1->TargetType);
    
    table2 = (AjPTable)
	ajTableFetch(mapper2->Pairs, (const void *) mapper2->TargetType);
    
    if(!table2)
        ajFatal("ensMapperAddMapper first-level AJAX table for second "
		"Ensembl Mapper target type '%S' not initialised.",
		mapper2->TargetType);
    
    /*
    ** Convert the second-level AJAX Table with Ensembl Object identifiers
    ** as key data and AJAX List value data for the second Ensembl Mapper.
    */
    
    ajTableToarray(table2, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	/*
	** Get the third-level AJAX Lists for corresponding
	** Ensembl Object identifiers.
	*/
	
	list1 = (AjPList) ajTableFetch(table1, (const void *) &keyarray[i]);
	
	list2 = (AjPList) valarray[i];
	
	while(ajListPop(list2, (void **) &mp))
	{
	    ajListPushAppend(list1, (void *) mp);
	    
	    trgcounter++;
	}
	
	/*
	** Remove the entry from the AJAX Table and free the
	** Ensembl Object identifier and the list.
	*/
	
	ajTableRemove(table2, (const void *) keyarray[i]);
	
	AJFREE(keyarray[i]);
	
	ajListFree(&list2);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    if(srccounter == trgcounter)
	mapper1->PairCount += srccounter;
    else
	ajFatal("ensMapperAddMapper Ensembl Mapper Pairs for source %u and "
		"target %u types do not match in the second Ensembl Mapper.",
		srccounter, trgcounter);
    
    mapper1->IsSorted = ajFalse;
    
    return ajTrue;
}




/* @func ensMapperGetMemSize **************************************************
**
** Get the memory size in bytes of an Ensembl Mapper.
**
** @param [r] mapper [const EnsPMapper] Ensembl Mapper
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMapperGetMemSize(const EnsPMapper mapper)
{
    void **keyarray1 = NULL;
    void **valarray1 = NULL;
    
    void **keyarray2 = NULL;
    void **valarray2 = NULL;
    
    register ajuint i = 0;
    register ajuint j = 0;
    
    ajuint size = 0;
    
    AjIList iter = NULL;
    
    EnsPMapperPair mp = NULL;
    
    if(!mapper)
	return 0;
    
    size += (ajuint) sizeof (EnsOMapper);
    
    if(mapper->SourceType)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(mapper->SourceType);
    }
    
    if(mapper->TargetType)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(mapper->TargetType);
    }
    
    size += ensCoordSystemGetMemSize(mapper->SourceCoordSystem);
    size += ensCoordSystemGetMemSize(mapper->TargetCoordSystem);
    
    /* Level 0 data (AjOTable). */
    
    size += (ajuint) sizeof (AjOTable);
    
    ajTableToarray(mapper->Pairs, &keyarray1, &valarray1);
    
    for(i = 0; valarray1[i]; i++)
    {
	/* Level 1 key data (AjOStr). */
	
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes((AjPStr) keyarray1[i]);
	
	/* Level 1 value data (AjOTable). */
	
	size += (ajuint) sizeof (AjOTable);
	
	ajTableToarray(valarray1[i], &keyarray2, &valarray2);
	
	for(j = 0; valarray2[j]; j++)
	{
	    /* Level 2 key data (ajuint). */
	    
	    size += (ajuint) sizeof (ajuint);
	    
	    /* Level 2 value data (AjOList). */
	    
	    size += (ajuint) sizeof (AjOList);
	    
	    iter = ajListIterNew((AjPList) valarray2[j]);
	    
	    while(!ajListIterDone(iter))
	    {
		/* Level 3 data (EnsOMapperPair). */
		
		mp = (EnsPMapperPair) ajListIterGet(iter);
		
		size += ensMapperPairGetMemSize(mp);
	    }
	    
	    ajListIterDel(&iter);
	}
	
	AJFREE(keyarray2);
	AJFREE(valarray2);
    }
    
    AJFREE(keyarray1);
    AJFREE(valarray1);
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Mapper object.
**
** @fdata [EnsPMapper]
** @nam3rule Trace Report Ensembl Mapper elements to debug file
**
** @argrule Trace mapper [const EnsPMapper] Ensembl Mapper
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMapperTrace *******************************************************
**
** Trace an Ensembl Mapper.
**
** @param [r] mp [const EnsPMapper] Ensembl Mapper
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMapperTrace(const EnsPMapper mapper, ajuint level)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    ajuint *Poid = NULL;
    
    AjIList iter = NULL;
    
    AjPStr indent = NULL;
    
    AjPTable table = NULL;
    
    EnsPMapperPair mp = NULL;
    
    if(!mapper)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMapperTrace %p\n"
	    "%S  SourceType '%S'\n"
	    "%S  TargetType '%S'\n"
	    "%S  SourceCoordSystem %p\n"
	    "%S  TargetCoordSystem %p\n"
	    "%S  Pairs %p\n"
	    "%S  PairCount %u\n"
	    "%S  IsSorted '%B'\n"
	    "%S  Use %u\n",
	    indent, mapper,
	    indent, mapper->SourceType,
	    indent, mapper->TargetType,
	    indent, mapper->SourceCoordSystem,
	    indent, mapper->TargetCoordSystem,
	    indent, mapper->Pairs,
	    indent, mapper->PairCount,
	    indent, mapper->IsSorted,
	    indent, mapper->Use);
    
    ensCoordSystemTrace(mapper->SourceCoordSystem, level + 1);
    
    ensCoordSystemTrace(mapper->TargetCoordSystem, level + 1);
    
    /* Trace the AJAX Table for the SourceType. */
    
    table = (AjPTable)
	ajTableFetch(mapper->Pairs, (const void *) mapper->SourceType);
    
    ajDebug("%S  AJAX Table %p for SourceType '%S'\n",
	    indent, table, mapper->SourceType);
    
    ajTableToarray(table, &keyarray, &valarray);
    
    for(i = 0; valarray[i]; i++)
    {
	Poid = (ajuint *) keyarray[i];
	
	ajDebug("%S    AJAX List %p for Object identifier %u\n",
		indent, valarray[i], *Poid);
	
	iter = ajListIterNew((AjPList) valarray[i]);
	
	while(!ajListIterDone(iter))
	{
	    mp = (EnsPMapperPair) ajListIterGet(iter);
	    
	    ensMapperPairTrace(mp, level + 3);
	}
	
	ajListIterDel(&iter);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    /* Trace the AJAX Table for the TargetType. */
    
    table = (AjPTable)
	ajTableFetch(mapper->Pairs, (const void *) mapper->TargetType);
    
    ajDebug("%S  AJAX Table %p for TargetType '%S'\n",
	    indent, table, mapper->TargetType);
    
    ajTableToarray(table, &keyarray, &valarray);
    
    for(i = 0; valarray[i]; i++)
    {
	Poid = (ajuint *) keyarray[i];
	
	ajDebug("%S    AJAX List %p for Object identifier %u\n",
		indent, valarray[i], *Poid);
	
	iter = ajListIterNew((AjPList) valarray[i]);
	
	while(!ajListIterDone(iter))
	{
	    mp = (EnsPMapperPair) ajListIterGet(iter);
	    
	    ensMapperPairTrace(mp, level + 3);
	}
	
	ajListIterDel(&iter);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    ajStrDel(&indent);
    
    return ajTrue;
}
