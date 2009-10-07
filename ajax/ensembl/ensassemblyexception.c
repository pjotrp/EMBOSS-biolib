/******************************************************************************
** @source Ensembl Assembly Exception functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.2 $
** @@
**
** The Ensembl Assembly Exception object has been split out of the
** Bio::EnsEMBL::DBSQL::Slice
adaptor Perl object.
**
** Bio::EnsEMBL::Slice CVS Revision: 1.219
** Bio::EnsEMBL::DBSQL::Sliceadaptor CVS Revision: 1.102
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

#include "ensassemblyexception.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *assemblyExceptionType[] =
{
    (const char *) NULL,
    /* Haplotype Assembly Exception */
    "HAP",
    /* Pseudo-autosomal Region Assembly Exception */
    "PAR",
    (const char *) NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static AjBool assemblyExceptionadaptorFetchAllBySQL(
    EnsPAssemblyExceptionadaptor adaptor,
    const AjPStr statement,
    AjPList aes);

static AjBool assemblyExceptionadaptorCacheInit(
    EnsPAssemblyExceptionadaptor adaptor);

static void assemblyExceptionadaptorClearIdentifierCache(void **key,
                                                         void **value,
                                                         void *cl);

static AjBool assemblyExceptionadaptorCacheExit(
    EnsPAssemblyExceptionadaptor adaptor);

static void assemblyExceptionadaptorFetchAll(const void *key, void **value,
                                             void *cl);




/* @filesection ensassemblyexception ******************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPAssemblyException] Assembly Exception ********************
**
** Functions for manipulating Ensembl Assembly Exception objects
**
** @nam2rule AssemblyException
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Assembly Exception by pointer.
** It is the responsibility of the user to first destroy any previous
** Assembly Exception. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPAssemblyException]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPAssemblyException] Ensembl Assembly Exception
** @argrule Ref object [EnsPAssemblyException] Ensembl Assembly Exception
**
** @valrule * [EnsPAssemblyException] Ensembl Assembly Exception
**
** @fcategory new
******************************************************************************/




/* @func ensAssemblyExceptionNew **********************************************
**
** Default constructor for an Ensembl Assembly Exception.
**
** @param [r] adaptor [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [r] srid [ajuint] Reference Ensembl Sequence Region identifier
** @param [r] srstart [ajuint] Reference Ensembl Sequence Region start
** @param [r] srend [ajuint] Reference Ensembl Sequence Region end
** @param [r] erid [ajuint] Exception Ensembl Sequence Region identifier
** @param [r] erstart [ajuint] Exception Ensembl Sequence Region start
** @param [r] erend [ajuint] Exception Ensembl Sequence Region end
** @param [r] ori [ajint] Orientation
** @param [r] type [AjEnum] Type
**
** @return [EnsPAssemblyException] Ensembl Assembl Exception or NULL
** @@
******************************************************************************/

EnsPAssemblyException ensAssemblyExceptionNew(
    EnsPAssemblyExceptionadaptor adaptor,
    ajuint identifier,
    ajuint srid,
    ajuint srstart,
    ajuint srend,
    ajuint erid,
    ajuint erstart,
    ajuint erend,
    ajint ori,
    AjEnum type)
{
    EnsPAssemblyException ae = NULL;
    
    if(!srid)
	return NULL;
    
    if(!erid)
	return NULL;
    
    AJNEW0(ae);
    
    ae->Use = 1;
    
    ae->Identifier          = identifier;
    ae->Adaptor             = adaptor;
    ae->SeqRegionIdentifier = srid;
    ae->SeqRegionStart      = srstart;
    ae->SeqRegionEnd        = srend;
    ae->ExcRegionIdentifier = erid;
    ae->ExcRegionStart      = erstart;
    ae->ExcRegionEnd        = erend;
    ae->Orientation         = ori;
    ae->Type                = type;
    
    return ae;
}




/* @func ensAssemblyExceptionNewObj *******************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [EnsPAssembly] Ensembl Assembly Exception or NULL
** @@
******************************************************************************/

EnsPAssemblyException ensAssemblyExceptionNewObj(
    const EnsPAssemblyException object)
{
    EnsPAssemblyException ae = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(ae);
    
    ae->Use = 1;
    
    ae->Identifier          = object->Identifier;
    ae->Adaptor             = object->Adaptor;
    ae->SeqRegionIdentifier = object->SeqRegionIdentifier;
    ae->SeqRegionStart      = object->SeqRegionStart;
    ae->SeqRegionEnd        = object->SeqRegionEnd;
    ae->ExcRegionIdentifier = object->ExcRegionIdentifier;
    ae->ExcRegionStart      = object->ExcRegionStart;
    ae->ExcRegionEnd        = object->ExcRegionEnd;
    ae->Orientation         = object->Orientation;
    ae->Type                = object->Type;
    
    return ae;
}




/* @func ensAssemblyExceptionNewRef *******************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [EnsPAssemblyException] Ensembl Assembly Exception or NULL
** @@
******************************************************************************/

EnsPAssemblyException ensAssemblyExceptionNewRef(EnsPAssemblyException ae)
{
    if(!ae)
	return NULL;
    
    ae->Use++;
    
    return ae;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Assembly Exception.
**
** @fdata [EnsPAssemblyException]
** @fnote None
**
** @nam3rule Del Destroy (free) an Assembly Exception object
**
** @argrule * Pae [EnsPAssemblyException*] Ensembl Assembly Exception object
**                                         address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensAssemblyExceptionDel **********************************************
**
** Default Ensembl Assembly Exception destructor.
**
** @param [d] Pae [EnsPAssemblyException*] Ensembl Assembly Exception address
**
** @return [void]
** @@
******************************************************************************/

void ensAssemblyExceptionDel(EnsPAssemblyException* Pae)
{
    EnsPAssemblyException pthis = NULL;
    
    if(!Pae)
	return;
    
    if(!*Pae)
	return;

    pthis = *Pae;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pae = NULL;
	
	return;
    }
    
    AJFREE(*Pae);
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Assembly Exception object.
**
** @fdata [EnsPAssemblyException]
** @fnote None
**
** @nam3rule Get Return Assembly Exception attribute(s)
** @nam4rule Getadaptor Return the Ensembl Assembly Excepton Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetSeqRegion Return reference Ensembl Sequence Region attribute(s)
** @nam5rule GetSeqRegionIdentfier Return the identifier
** @nam5rule GetSeqRegionStart Return the start
** @nam5rule GetSeqRegionEnd Return the end
** @nam4rule GetExcRegion Return exception Ensembl Sequence Region attribute(s)
** @nam5rule GetExcRegionIdentifier Return the identifier
** @nam5rule GetExcRegionStart Return the start
** @nam5rule GetExcRegionEnd Return the end
** @nam4rule GetOrientation Return the orientation
** @nam4rule GetType Return the type
**
** @argrule * ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @valrule Adaptor [EnsPAssemblyExceptionadaptor] Ensembl Assembly Exception
**                                                 Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule SeqRegionIdentfier [ajuint] Reference Sequence Region identifier
** @valrule SeqRegionStart [ajuint] Reference Sequence Region start
** @valrule SeqRegionEnd [ajuint] Reference Sequence Region end
** @valrule ExcRegionIdentifier [ajuint] Exception Sequence Region identifier
** @valrule ExcRegionStart [ajuint] Exception Sequence Region start
** @valrule ExcRegionEnd [ajuint] Exception Sequence Region end
** @valrule Orientation [ajuint] Orientation
** @valrule Type [AjEnum] Type
**
** @fcategory use
******************************************************************************/




/* @func ensAssemblyExceptionGetadaptor ***************************************
**
** Get the Ensembl Assembly Exception Adaptor element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [EnsPAssemblyExceptionadaptor] Ensembl Assembly Exception Adaptor
** @@
******************************************************************************/

EnsPAssemblyExceptionadaptor ensAssemblyExceptionGetadaptor(
    const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->Adaptor;
}




/* @func ensAssemblyExceptionGetIdentifier ************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetIdentifier(const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->Identifier;
}




/* @func ensAssemblyExceptionGetSeqRegionIdentfier ****************************
**
** Get the reference Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Reference Ensembl Sequence Region identifier
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetSeqRegionIdentifier(
    const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->SeqRegionIdentifier;
}




/* @func ensAssemblyExceptionGetSeqRegionStart ********************************
**
** Get the reference Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Reference Ensembl Sequence Region start
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetSeqRegionStart(const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->SeqRegionStart;
}




/* @func ensAssemblyExceptionGetSeqRegionEnd **********************************
**
** Get the reference Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Reference Ensembl Sequence Region end
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetSeqRegionEnd(const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->SeqRegionEnd;
}




/* @func ensAssemblyExceptionGetExcRegionIdentifier ***************************
**
** Get the exception Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Exception Ensembl Sequence Region identifier
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetExcRegionIdentifier(
    const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->ExcRegionIdentifier;
}




/* @func ensAssemblyExceptionGetExcRegionStart ********************************
**
** Get the exception Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Exception Ensembl Sequence Region start
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetExcRegionStart(const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->SeqRegionStart;
}




/* @func ensAssemblyExceptionGetExcRegionEnd **********************************
**
** Get the exception Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Exception Ensembl Sequence Region end
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetExcRegionEnd(const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->SeqRegionEnd;
}




/* @func ensAssemblyExceptionGetOrientation ***********************************
**
** Get the orientation element of an Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajint] Orientation
** @@
******************************************************************************/

ajint ensAssemblyExceptionGetOrientation(const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->Orientation;
}




/* @func ensAssemblyExceptionGetType ******************************************
**
** Get the type element of an Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [AjEnum] Type
** @@
******************************************************************************/

AjEnum ensAssemblyExceptionGetType(const EnsPAssemblyException ae)
{
    if(!ae)
	return 0;
    
    return ae->Type;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Assembly Exception object.
**
** @fdata [EnsPAssemblyException]
** @fnote None
**
** @nam3rule Set Set one element of an Assembly Exception
** @nam4rule Setadaptor Set the Ensembl Assembly Excepton Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetSeqRegion Set reference Ensembl Sequence Region attribute(s)
** @nam5rule SetSeqRegionIdentfier Set the identifier
** @nam5rule SetSeqRegionStart Set the start
** @nam5rule SetSeqRegionEnd Set the end
** @nam4rule SetExcRegion Set exception Ensembl Sequence Region attribute(s)
** @nam5rule SetExcRegionIdentifier Set the identifier
** @nam5rule SetExcRegionStart Set the start
** @nam5rule SetExcRegionEnd Set the end
** @nam4rule SetOrientation Set the orientation
** @nam4rule SetType Set the type
**
** @argrule * ae [EnsPAssemblyException] Assembly Exception
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensAssemblyExceptionSetadaptor ***************************************
**
** Set the Ensembl Assembly Exception Adaptor element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] adaptor [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetadaptor(EnsPAssemblyException ae,
                                      EnsPAssemblyExceptionadaptor adaptor)
{
    if(!ae)
        return ajFalse;
    
    ae->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetIdentfier *************************************
**
** Set the SQL database identifier element of an Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetIdentifier(EnsPAssemblyException ae,
                                         ajuint identifier)
{
    if(!ae)
        return ajFalse;
    
    ae->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetSeqRegionIdentifier ***************************
**
** Set the reference Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] srid [ajuint] Reference Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetSeqRegionIdentifier(EnsPAssemblyException ae,
                                                  ajuint srid)
{
    if(!ae)
        return ajFalse;
    
    ae->SeqRegionIdentifier = srid;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetSeqRegionStart ********************************
**
** Set the reference Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] srstart [ajuint] Reference Ensembl Sequence Region start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetSeqRegionStart(EnsPAssemblyException ae,
                                             ajuint srstart)
{
    if(!ae)
        return ajFalse;
    
    ae->SeqRegionStart = srstart;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetSeqRegionEnd **********************************
**
** Set the reference Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] srend [ajuint] Reference Ensembl Sequence Region end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetSeqRegionEnd(EnsPAssemblyException ae,
                                           ajuint srend)
{
    if(!ae)
        return ajFalse;
    
    ae->SeqRegionEnd = srend;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetExcRegionIdentifier ***************************
**
** Set the exception Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] exid [ajuint] Exception Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetExcRegionIdentifier(EnsPAssemblyException ae,
                                                  ajuint exid)
{
    if(!ae)
        return ajFalse;
    
    ae->ExcRegionIdentifier = exid;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetExcRegionStart ********************************
**
** Set the exception Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] exstart [ajuint] Exception Ensembl Sequence Region start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetEcxRegionStart(EnsPAssemblyException ae,
                                             ajuint exstart)
{
    if(!ae)
        return ajFalse;
    
    ae->ExcRegionStart = exstart;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetExcRegionEnd **********************************
**
** Set the exception Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] exend [ajuint] Exception Ensembl Sequence Region end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetExcRegionStart(EnsPAssemblyException ae,
                                             ajuint exend)
{
    if(!ae)
        return ajFalse;
    
    ae->ExcRegionEnd = exend;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetOrientation ***********************************
**
** Set the orientation element of an Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] orientation [ajint] Orientation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetOrientation(EnsPAssemblyException ae,
                                          ajint orientation)
{
    if(!ae)
        return ajFalse;
    
    ae->Orientation = orientation;
    
    return ajTrue;
}




/* @func ensAssemblyExceptionSetType ******************************************
**
** Set the type element of an Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionSetType(EnsPAssemblyException ae, AjEnum type)
{
    if(!ae)
        return ajFalse;
    
    ae->Type = type;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Assembly Exception object.
**
** @fdata [EnsPAssemblyException]
** @nam3rule Trace Report Ensembl Assembly Exception elements to debug file
**
** @argrule Trace ae [const EnsPAssemblyException] Ensembl Assembly Exception
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensAssemblyExceptionTrace ********************************************
**
** Trace an Ensembl Assembly Eception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionTrace(const EnsPAssemblyException ae, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!ae)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensAssemblyExceptionTrace %p\n"
	    "%S  Identfier %u\n"
	    "%S  SeqRegionIdentfier %u\n"
	    "%S  SeqRegionStart %u\n"
	    "%S  SeqRegionEnd %u\n"
	    "%S  ExcSeqRegionIdentifier %u\n"
	    "%S  ExcRegionStart %d\n"
	    "%S  ExcRegionEnd %u\n"
	    "%S  Orientation %d\n"
	    "%S  Type '%s'\n"
	    "%S  Use %u\n",
	    indent, ae,
	    indent, ae->Identifier,
	    indent, ae->SeqRegionIdentifier,
	    indent, ae->SeqRegionStart,
	    indent, ae->SeqRegionEnd,
	    indent, ae->ExcRegionIdentifier,
	    indent, ae->ExcRegionStart,
	    indent, ae->ExcRegionEnd,
	    indent, ensAssemblyExceptionTypeToChar(ae->Type),
	    indent, ae->Orientation,
	    indent, ae->Use);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensAssemblyExceptionGetMemSize ***************************************
**
** Get the memory size in bytes of an Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyException] Ensembl Assembly Exception
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensAssemblyExceptionGetMemSize(const EnsPAssemblyException ae)
{
    ajuint size = 0;
    
    if(!ae)
	return 0;
    
    size += sizeof (EnsOAssemblyException);
    
    return size;
}




/* @func ensAssemblyExceptionTypeFromStr **************************************
**
** Convert an AJAX String into an Ensembl Assembly Exception type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [AjEnum] Ensembl Assembly Exception type element or
**                  ensEAssemblyExceptionTypeNULL
** @@
******************************************************************************/

AjEnum ensAssemblyExceptionTypeFromStr(const AjPStr type)
{
    register ajint i = 0;
    
    AjEnum etype = ensEAssemblyExceptionTypeNULL;
    
    for (i = 1; assemblyExceptionType[i]; i++)
	if (ajStrMatchC(type, assemblyExceptionType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensAssemblyExceptionTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensAssemblyExceptionTypeToChar ***************************************
**
** Convert an Ensembl Assembly Exception type element into a
** C-type (char*) string.
**
** @param [r] type [const AjEnum] Assembly Exception type enumerator
**
** @return [const char*] Assembly Exception status C-type (char*) string
** @@
******************************************************************************/

const char* ensAssemblyExceptionTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; assemblyExceptionType[i] && (i < type); i++);
    
    if(!assemblyExceptionType[i])
	ajDebug("ensAssemblyExceptionTypeToChar encountered an "
		"out of boundary error on status %d.\n", type);
    
    return assemblyExceptionType[i];
}




/* @datasection [EnsPAssemblyExceptionadaptor] Assembly Exception Adaptor *****
**
** Functions for manipulating Ensembl Assembly Exception Adaptor objects
**
** @cc Bio::EsnSEMBL::DBSQL::Sliceadaptor CVS Revision: 1.104
**
** @nam2rule AssemblyExceptionadaptor
**
******************************************************************************/



/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Assembly Exception Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Assembly Exception Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPAssemblyExceptionadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                    Exception Adaptor
** @argrule Ref object [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                    Exception Adaptor
**
** @valrule * [EnsPAssemblyExceptionadaptor] Ensembl Assembly Exception Adaptor
**
** @fcategory new
******************************************************************************/




/* @funcstatic assemblyExceptionadaptorFetchAllBySQL **************************
**
** Run a SQL statement against an Ensembl Assembly Exception Adaptor and
** consolidate the results into an AJAX List of Ensembl Assembly Exception
** objects.
**
** @param [r] adaptor [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] aes [AjPList] AJAX List of Ensembl Assembly Exceptions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyExceptionadaptorFetchAllBySQL(
    EnsPAssemblyExceptionadaptor adaptor,
    const AjPStr statement,
    AjPList aes)
{
    ajint ori = 0;
    
    ajuint identifier = 0;
    ajuint erid       = 0;
    ajuint srid       = 0;
    ajuint erstart    = 0;
    ajuint srstart    = 0;
    ajuint erend      = 0;
    ajuint srend      = 0;
    
    AjEnum type = ensEAssemblyExceptionTypeNULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr typestr = NULL;
    
    EnsPAssemblyException ae = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!statement)
        return ajFalse;
    
    if(!aes)
        return ajFalse;
    
    sqls = ensDatabaseadaptorSqlstatementNew(adaptor->Adaptor, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	srid       = 0;
	srstart    = 0;
	srend      = 0;
	typestr    = ajStrNew();
	erid       = 0;
	erstart    = 0;
	erend      = 0;
	ori        = 0;
	
	type = ensEAssemblyExceptionTypeNULL;
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &srid);
        ajSqlcolumnToUint(sqlr, &srstart);
        ajSqlcolumnToUint(sqlr, &srend);
        ajSqlcolumnToStr(sqlr, &typestr);
        ajSqlcolumnToUint(sqlr, &erid);
        ajSqlcolumnToUint(sqlr, &erstart);
        ajSqlcolumnToUint(sqlr, &erend);
        ajSqlcolumnToInt(sqlr, &ori);
	
	/* Set the Assembly Exception type. */
	
	type = ensAssemblyExceptionTypeFromStr(typestr);
	
	if(!type)
	    ajFatal("assemblyExceptionadaptorFetchAllBySQL "
		    "got unexpected Assembly Exception type '%S' "
		    "from database.\n",
		    typestr);
	
        ae = ensAssemblyExceptionNew(adaptor,
				     identifier,
				     srid,
				     srstart,
				     srend,
				     erid,
				     erstart,
				     erend,
				     ori,
				     type);
	
        ajListPushAppend(aes, (void *) ae);
	
	ajStrDel(&typestr);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @funcstatic assemblyExceptionadaptorCacheInit ******************************
**
** Initialise the Ensembl Assembly Exception cache in the
** Assembly Exception Adaptor.
**
** @param [u] adaptor [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyExceptionadaptorCacheInit(
    EnsPAssemblyExceptionadaptor adaptor)
{
    ajuint *Pidentifier = NULL;
    
    AjPList list = NULL;
    AjPList aes  = NULL;
    
    AjPStr statement = NULL;
    
    EnsPAssemblyException ae = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(adaptor->CacheBySeqRegionIdentifier)
        return ajTrue;
    
    else
        adaptor->CacheBySeqRegionIdentifier =
	    ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    statement = ajFmtStr("SELECT "
			 "assembly_exception.assembly_exception_id, "
			 "assembly_exception.seq_region_id, "
			 "assembly_exception.seq_region_start, "
			 "assembly_exception.seq_region_end, "
			 "assembly_exception.exc_type, "
			 "assembly_exception.exc_seq_region_id, "
			 "assembly_exception.exc_seq_region_start, "
			 "assembly_exception.exc_seq_region_end, "
			 "assembly_exception.ori "
			 "FROM "
			 "assembly_exception, "
			 "seq_region, "
			 "coord_system "
			 "WHERE "
			 "seq_region.seq_region_id = "
			 "assembly_exception.seq_region_id "
			 "AND "
			 "seq_region.coord_system_id = "
			 "coord_system.coord_system_id "
			 "AND "
			 "coord_system.species_id = %u",
			 ensDatabaseadaptorGetIdentifier(adaptor->Adaptor));
    
    aes = ajListNew();
    
    assemblyExceptionadaptorFetchAllBySQL(adaptor, statement, aes);
    
    ajStrDel(&statement);
    
    while(ajListPop(aes, (void **) &ae))
    {
        list = (AjPList)
	ajTableFetch(adaptor->CacheBySeqRegionIdentifier,
		     (const void *) &(ae->SeqRegionIdentifier));
	
	if(!list)
	{
	    AJNEW0(Pidentifier);
	    
	    *Pidentifier = ae->SeqRegionIdentifier;
	    
	    list = ajListNew();
	    
	    ajTablePut(adaptor->CacheBySeqRegionIdentifier,
		       (void *) Pidentifier,
		       (void *) list);
	}
	
	ajListPushAppend(list, (void *) ae);
    }
    
    ajListFree(&aes);
    
    return ajTrue;
}




/* @func ensAssemblyExceptionadaptorNew ***************************************
**
** Default constructor for an Ensembl Assembly Exception Adaptor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyExceptionadaptor] Ensembl Assembly Exception Adaptor
**                                        or NULL
** @@
******************************************************************************/

EnsPAssemblyExceptionadaptor ensAssemblyExceptionadaptorNew(
    EnsPDatabaseadaptor dba)
{
    EnsPAssemblyExceptionadaptor adaptor = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(adaptor);
    
    adaptor->Adaptor = dba;
    
    assemblyExceptionadaptorCacheInit(adaptor);
    
    return adaptor;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Assembly Exception Adaptor.
**
** @fdata [EnsPAssemblyExceptionadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Assembly Exception Adaptor object
**
** @argrule * Padaptor [EnsPAnalysisadaptor*] Assembly Exception Adaptor
**                                            object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @funcstatic assemblyExceptionadaptorClearIdentifierCache *******************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Assembly Exception
** Adaptor-internal Ensembl Assembly Exception cache. This function clears the
** unsigned integer key data and the AJAX List value data, as well as the
** Ensembl Assembly Exceptions from the AJAX List.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] AJAX List value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void assemblyExceptionadaptorClearIdentifierCache(void **key,
                                                         void **value,
                                                         void *cl)
{
    EnsPAssemblyException ae = NULL;
    
    if(!key)
	return;
    
    if(!*key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    (void) cl;
    
    AJFREE(*key);
    
    while(ajListPop(*((AjPList *) value), (void **) &ae))
	ensAssemblyExceptionDel(&ae);
    
    ajListFree((AjPList *) value);
    
    return;
}




/* @funcstatic assemblyExceptionadaptorCacheExit ******************************
**
** Clears the Ensembl Assembly Exception cache in the
** Ensembl Assembly Exception Adaptor.
**
** @param [u] adaptor [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyExceptionadaptorCacheExit(
    EnsPAssemblyExceptionadaptor adaptor)
{
    if(!adaptor)
        return ajFalse;
    
    if(!adaptor->CacheBySeqRegionIdentifier)
	return ajTrue;
    
    ajTableMapDel(adaptor->CacheBySeqRegionIdentifier,
		  assemblyExceptionadaptorClearIdentifierCache,
		  NULL);
    
    ajTableFree(&(adaptor->CacheBySeqRegionIdentifier));
    
    return ajTrue;
}




/* @func ensAssemblyExceptionadaptorDel ***************************************
**
** Default destructor for an Ensembl Assembly Exception Adaptor.
** This function also clears the internal Assembly Exception cache.
**
** @param [r] Padaptor [EnsPAssemblyExceptionadaptor*] Ensembl Assembly
**                                                     Exception Adaptor
**                                                     address
**
** @return [void]
** @@
******************************************************************************/

void ensAssemblyExceptionadaptorDel(EnsPAssemblyExceptionadaptor* Padaptor)
{
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;
    
    assemblyExceptionadaptorCacheExit(*Padaptor);
    
    AJFREE(*Padaptor);
    
    return;
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Assembly Exception objects from an
** Ensembl Core database.
**
** @fdata [EnsPAssemblyExceptionadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Assembly Exception object(s)
** @nam4rule FetchAll Retrieve all Ensembl Assembly Exception objects
** @nam5rule FetchAllBy Retrieve all Ensembl Assembly Exception objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Assembly Exception object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                         Exception Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Assembly Exception objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @funcstatic assemblyExceptionadaptorFetchAll *******************************
**
** An ajTableMap 'apply' function to return all Ensembl Assembly Exceptions
** from the Ensembl Assembly Exception Adaptor-internal cache.
**
** @param [u] key [const void *] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Assembly Exception value data address
** @param [u] cl [void *] AJAX List of Ensembl Assembly Exception objects,
**                        passed in via ajTableMap
** @see ajTableMap
**
** @return [void]
** @@
******************************************************************************/

static void assemblyExceptionadaptorFetchAll(const void *key, void **value,
                                             void *cl)
{
    AjIList iter = NULL;
    
    EnsPAssemblyException ae = NULL;
    
    if(!key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    if(!cl)
	return;
    
    iter = ajListIterNew(*((AjPList *) value));
    
    while(!ajListIterDone(iter))
    {
	ae = (EnsPAssemblyException) ajListIterGet(iter);
	
	ajListPushAppend((AjPList) cl,
			 (void *) ensAssemblyExceptionNewRef(ae));
    }
    
    ajListIterDel(&iter);
    
    return;
}




/* @func ensAssemblyExceptionadaptorFetchAll **********************************
**
** Fetch all Ensembl Assembly Exceptions.
**
** The caller is responsible for deleting the Ensembl Assembly Exception
** objects before deleting the AJAX List.
**
** @param [r] adaptor [const EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                         Exception Adaptor
** @param [u] aes [AjPList] AJAX List of Ensembl Assembly Exceptions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionadaptorFetchAll(
    const EnsPAssemblyExceptionadaptor adaptor,
    AjPList aes)
{
    if(!adaptor)
        return ajFalse;
    
    if(!aes)
        return ajFalse;
    
    ajTableMap(adaptor->CacheBySeqRegionIdentifier,
	       assemblyExceptionadaptorFetchAll,
	       (void *) aes);
    
    return ajTrue;
}




/* @func ensAssemblyExceptionadaptorFetchAllBySeqRegionIdentifier *************
**
** Fetch all Ensembl Assembly Exceptions by a Sequence Region identifier.
**
** The caller is responsible for deleting the Ensembl Assembly Exception
** objects before deleting the AJAX List.
**
** @param [r] adaptor [const EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                                         Exception Adaptor
** @param [r] srid [ajuint] Ensembl Sequence Region identifier
** @param [u] aes [AjPList] AJAX List of Ensembl Assembly Exceptions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyExceptionadaptorFetchAllBySeqRegionIdentifier(
    EnsPAssemblyExceptionadaptor adaptor,
    ajuint srid,
    AjPList aes)
{
    AjIList iter = NULL;
    AjPList list = NULL;
    
    EnsPAssemblyException ae = NULL;
    
    /*
     ajDebug("ensAssemblyExceptionadaptorFetchAllBySeqRegionIdentifier\n"
	     "  adaptor %p\n"
	     "  srid %u\n"
	     "  aes %p\n",
	     adaptor,
	     srid,
	     aes);
     */
    
    if(!adaptor)
	return ajFalse;
    
    if(!aes)
	return ajFalse;
    
    list = (AjPList)
	ajTableFetch(adaptor->CacheBySeqRegionIdentifier,
		     (const void *) &srid);
    
    iter = ajListIterNew(list);
    
    while(!ajListIterDone(iter))
    {
	ae = (EnsPAssemblyException) ajListIterGet(iter);
	
	ajListPushAppend(aes, (void *) ensAssemblyExceptionNewRef(ae));
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}
