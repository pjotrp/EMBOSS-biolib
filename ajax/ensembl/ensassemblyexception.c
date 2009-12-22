/******************************************************************************
** @source Ensembl Assembly Exception functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.5 $
** @@
**
** The Ensembl Assembly Exception object has been split out of the
** Bio::EnsEMBL::DBSQL::SliceAdaptor Perl object.
**
** Bio::EnsEMBL::Slice CVS Revision: 1.219
** Bio::EnsEMBL::DBSQL::SliceAdaptor CVS Revision: 1.104
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
    EnsPAssemblyexceptionadaptor adaptor,
    const AjPStr statement,
    AjPList aes);

static AjBool assemblyExceptionadaptorCacheInit(
    EnsPAssemblyexceptionadaptor adaptor);

static void assemblyExceptionadaptorClearIdentifierCache(void **key,
                                                         void **value,
                                                         void *cl);

static AjBool assemblyExceptionadaptorCacheExit(
    EnsPAssemblyexceptionadaptor adaptor);

static void assemblyExceptionadaptorFetchAll(const void *key, void **value,
                                             void *cl);




/* @filesection ensassemblyexception ******************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPAssemblyexception] Assembly Exception ********************
**
** Functions for manipulating Ensembl Assembly Exception objects
**
** @nam2rule Assemblyexception
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Assembly Exception by pointer.
** It is the responsibility of the user to first destroy any previous
** Assembly Exception. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPAssemblyexception]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPAssemblyexception] Ensembl Assembly Exception
** @argrule Ref object [EnsPAssemblyexception] Ensembl Assembly Exception
**
** @valrule * [EnsPAssemblyexception] Ensembl Assembly Exception
**
** @fcategory new
******************************************************************************/




/* @func ensAssemblyexceptionNew **********************************************
**
** Default constructor for an Ensembl Assembly Exception.
**
** @param [r] adaptor [EnsPAssemblyexceptionadaptor] Ensembl Assembly
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
** @return [EnsPAssemblyexception] Ensembl Assembl Exception or NULL
** @@
******************************************************************************/

EnsPAssemblyexception ensAssemblyexceptionNew(
    EnsPAssemblyexceptionadaptor adaptor,
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
    EnsPAssemblyexception ae = NULL;

    if(!srid)
        return NULL;

    if(!erid)
        return NULL;

    AJNEW0(ae);

    ae->Use = 1;

    ae->Identifier          = identifier;
    ae->Adaptor             = adaptor;
    ae->SeqregionIdentifier = srid;
    ae->SeqregionStart      = srstart;
    ae->SeqregionEnd        = srend;
    ae->ExcRegionIdentifier = erid;
    ae->ExcRegionStart      = erstart;
    ae->ExcRegionEnd        = erend;
    ae->Orientation         = ori;
    ae->Type                = type;

    return ae;
}




/* @func ensAssemblyexceptionNewObj *******************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [EnsPAssembly] Ensembl Assembly Exception or NULL
** @@
******************************************************************************/

EnsPAssemblyexception ensAssemblyexceptionNewObj(
    const EnsPAssemblyexception object)
{
    EnsPAssemblyexception ae = NULL;

    if(!object)
        return NULL;

    AJNEW0(ae);

    ae->Use = 1;

    ae->Identifier          = object->Identifier;
    ae->Adaptor             = object->Adaptor;
    ae->SeqregionIdentifier = object->SeqregionIdentifier;
    ae->SeqregionStart      = object->SeqregionStart;
    ae->SeqregionEnd        = object->SeqregionEnd;
    ae->ExcRegionIdentifier = object->ExcRegionIdentifier;
    ae->ExcRegionStart      = object->ExcRegionStart;
    ae->ExcRegionEnd        = object->ExcRegionEnd;
    ae->Orientation         = object->Orientation;
    ae->Type                = object->Type;

    return ae;
}




/* @func ensAssemblyexceptionNewRef *******************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [EnsPAssemblyexception] Ensembl Assembly Exception or NULL
** @@
******************************************************************************/

EnsPAssemblyexception ensAssemblyexceptionNewRef(EnsPAssemblyexception ae)
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
** @fdata [EnsPAssemblyexception]
** @fnote None
**
** @nam3rule Del Destroy (free) an Assembly Exception object
**
** @argrule * Pae [EnsPAssemblyexception*] Ensembl Assembly Exception object
**                                         address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensAssemblyexceptionDel **********************************************
**
** Default Ensembl Assembly Exception destructor.
**
** @param [d] Pae [EnsPAssemblyexception*] Ensembl Assembly Exception address
**
** @return [void]
** @@
******************************************************************************/

void ensAssemblyexceptionDel(EnsPAssemblyexception* Pae)
{
    EnsPAssemblyexception pthis = NULL;

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
** @fdata [EnsPAssemblyexception]
** @fnote None
**
** @nam3rule Get Return Assembly Exception attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Assembly Excepton Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetSeqregion Return reference Ensembl Sequence Region attribute(s)
** @nam5rule GetSeqregionIdentfier Return the identifier
** @nam5rule GetSeqregionStart Return the start
** @nam5rule GetSeqregionEnd Return the end
** @nam4rule GetExcRegion Return exception Ensembl Sequence Region attribute(s)
** @nam5rule GetExcRegionIdentifier Return the identifier
** @nam5rule GetExcRegionStart Return the start
** @nam5rule GetExcRegionEnd Return the end
** @nam4rule GetOrientation Return the orientation
** @nam4rule GetType Return the type
**
** @argrule * ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @valrule Adaptor [EnsPAssemblyexceptionadaptor] Ensembl Assembly Exception
**                                                 Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule SeqregionIdentfier [ajuint] Reference Sequence Region identifier
** @valrule SeqregionStart [ajuint] Reference Sequence Region start
** @valrule SeqregionEnd [ajuint] Reference Sequence Region end
** @valrule ExcRegionIdentifier [ajuint] Exception Sequence Region identifier
** @valrule ExcRegionStart [ajuint] Exception Sequence Region start
** @valrule ExcRegionEnd [ajuint] Exception Sequence Region end
** @valrule Orientation [ajuint] Orientation
** @valrule Type [AjEnum] Type
**
** @fcategory use
******************************************************************************/




/* @func ensAssemblyexceptionGetAdaptor ***************************************
**
** Get the Ensembl Assembly Exception Adaptor element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [EnsPAssemblyexceptionadaptor] Ensembl Assembly Exception Adaptor
** @@
******************************************************************************/

EnsPAssemblyexceptionadaptor ensAssemblyexceptionGetAdaptor(
    const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->Adaptor;
}




/* @func ensAssemblyexceptionGetIdentifier ************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetIdentifier(const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->Identifier;
}




/* @func ensAssemblyexceptionGetSeqregionIdentfier ****************************
**
** Get the reference Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Reference Ensembl Sequence Region identifier
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetSeqregionIdentifier(
    const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->SeqregionIdentifier;
}




/* @func ensAssemblyexceptionGetSeqregionStart ********************************
**
** Get the reference Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Reference Ensembl Sequence Region start
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetSeqregionStart(const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->SeqregionStart;
}




/* @func ensAssemblyexceptionGetSeqregionEnd **********************************
**
** Get the reference Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Reference Ensembl Sequence Region end
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetSeqregionEnd(const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->SeqregionEnd;
}




/* @func ensAssemblyexceptionGetExcRegionIdentifier ***************************
**
** Get the exception Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Exception Ensembl Sequence Region identifier
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetExcRegionIdentifier(
    const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->ExcRegionIdentifier;
}




/* @func ensAssemblyexceptionGetExcRegionStart ********************************
**
** Get the exception Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Exception Ensembl Sequence Region start
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetExcRegionStart(const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->SeqregionStart;
}




/* @func ensAssemblyexceptionGetExcRegionEnd **********************************
**
** Get the exception Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Exception Ensembl Sequence Region end
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetExcRegionEnd(const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->SeqregionEnd;
}




/* @func ensAssemblyexceptionGetOrientation ***********************************
**
** Get the orientation element of an Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajint] Orientation
** @@
******************************************************************************/

ajint ensAssemblyexceptionGetOrientation(const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->Orientation;
}




/* @func ensAssemblyexceptionGetType ******************************************
**
** Get the type element of an Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [AjEnum] Type
** @@
******************************************************************************/

AjEnum ensAssemblyexceptionGetType(const EnsPAssemblyexception ae)
{
    if(!ae)
        return 0;

    return ae->Type;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Assembly Exception object.
**
** @fdata [EnsPAssemblyexception]
** @fnote None
**
** @nam3rule Set Set one element of an Assembly Exception
** @nam4rule SetAdaptor Set the Ensembl Assembly Excepton Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetSeqregion Set reference Ensembl Sequence Region attribute(s)
** @nam5rule SetSeqregionIdentfier Set the identifier
** @nam5rule SetSeqregionStart Set the start
** @nam5rule SetSeqregionEnd Set the end
** @nam4rule SetExcRegion Set exception Ensembl Sequence Region attribute(s)
** @nam5rule SetExcRegionIdentifier Set the identifier
** @nam5rule SetExcRegionStart Set the start
** @nam5rule SetExcRegionEnd Set the end
** @nam4rule SetOrientation Set the orientation
** @nam4rule SetType Set the type
**
** @argrule * ae [EnsPAssemblyexception] Assembly Exception
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensAssemblyexceptionSetAdaptor ***************************************
**
** Set the Ensembl Assembly Exception Adaptor element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] adaptor [EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetAdaptor(EnsPAssemblyexception ae,
                                      EnsPAssemblyexceptionadaptor adaptor)
{
    if(!ae)
        return ajFalse;

    ae->Adaptor = adaptor;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetIdentfier *************************************
**
** Set the SQL database identifier element of an Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetIdentifier(EnsPAssemblyexception ae,
                                         ajuint identifier)
{
    if(!ae)
        return ajFalse;

    ae->Identifier = identifier;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetSeqregionIdentifier ***************************
**
** Set the reference Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] srid [ajuint] Reference Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetSeqregionIdentifier(EnsPAssemblyexception ae,
                                                  ajuint srid)
{
    if(!ae)
        return ajFalse;

    ae->SeqregionIdentifier = srid;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetSeqregionStart ********************************
**
** Set the reference Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] srstart [ajuint] Reference Ensembl Sequence Region start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetSeqregionStart(EnsPAssemblyexception ae,
                                             ajuint srstart)
{
    if(!ae)
        return ajFalse;

    ae->SeqregionStart = srstart;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetSeqregionEnd **********************************
**
** Set the reference Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] srend [ajuint] Reference Ensembl Sequence Region end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetSeqregionEnd(EnsPAssemblyexception ae,
                                           ajuint srend)
{
    if(!ae)
        return ajFalse;

    ae->SeqregionEnd = srend;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetExcRegionIdentifier ***************************
**
** Set the exception Ensembl Sequence Region identifier element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] exid [ajuint] Exception Ensembl Sequence Region identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetExcRegionIdentifier(EnsPAssemblyexception ae,
                                                  ajuint exid)
{
    if(!ae)
        return ajFalse;

    ae->ExcRegionIdentifier = exid;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetExcRegionStart ********************************
**
** Set the exception Ensembl Sequence Region start element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] exstart [ajuint] Exception Ensembl Sequence Region start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetExcRegionStart(EnsPAssemblyexception ae,
                                             ajuint exstart)
{
    if(!ae)
        return ajFalse;

    ae->ExcRegionStart = exstart;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetExcRegionEnd **********************************
**
** Set the exception Ensembl Sequence Region end element of an
** Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] exend [ajuint] Exception Ensembl Sequence Region end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetExcRegionEnd(EnsPAssemblyexception ae,
                                           ajuint exend)
{
    if(!ae)
        return ajFalse;

    ae->ExcRegionEnd = exend;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetOrientation ***********************************
**
** Set the orientation element of an Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] orientation [ajint] Orientation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetOrientation(EnsPAssemblyexception ae,
                                          ajint orientation)
{
    if(!ae)
        return ajFalse;

    ae->Orientation = orientation;

    return ajTrue;
}




/* @func ensAssemblyexceptionSetType ******************************************
**
** Set the type element of an Ensembl Assembly Exception.
**
** @param [u] ae [EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionSetType(EnsPAssemblyexception ae, AjEnum type)
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
** @fdata [EnsPAssemblyexception]
** @nam3rule Trace Report Ensembl Assembly Exception elements to debug file
**
** @argrule Trace ae [const EnsPAssemblyexception] Ensembl Assembly Exception
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensAssemblyexceptionTrace ********************************************
**
** Trace an Ensembl Assembly Eception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionTrace(const EnsPAssemblyexception ae, ajuint level)
{
    AjPStr indent = NULL;

    if(!ae)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensAssemblyexceptionTrace %p\n"
            "%S  Identfier %u\n"
            "%S  SeqregionIdentfier %u\n"
            "%S  SeqregionStart %u\n"
            "%S  SeqregionEnd %u\n"
            "%S  ExcSeqregionIdentifier %u\n"
            "%S  ExcRegionStart %d\n"
            "%S  ExcRegionEnd %u\n"
            "%S  Orientation %d\n"
            "%S  Type '%s'\n"
            "%S  Use %u\n",
            indent, ae,
            indent, ae->Identifier,
            indent, ae->SeqregionIdentifier,
            indent, ae->SeqregionStart,
            indent, ae->SeqregionEnd,
            indent, ae->ExcRegionIdentifier,
            indent, ae->ExcRegionStart,
            indent, ae->ExcRegionEnd,
            indent, ensAssemblyexceptionTypeToChar(ae->Type),
            indent, ae->Orientation,
            indent, ae->Use);

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensAssemblyexceptionGetMemSize ***************************************
**
** Get the memory size in bytes of an Ensembl Assembly Exception.
**
** @param [r] ae [const EnsPAssemblyexception] Ensembl Assembly Exception
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensAssemblyexceptionGetMemSize(const EnsPAssemblyexception ae)
{
    ajuint size = 0;

    if(!ae)
        return 0;

    size += sizeof (EnsOAssemblyexception);

    return size;
}




/* @func ensAssemblyexceptionTypeFromStr **************************************
**
** Convert an AJAX String into an Ensembl Assembly Exception type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [AjEnum] Ensembl Assembly Exception type element or
**                  ensEAssemblyexceptionTypeNULL
** @@
******************************************************************************/

AjEnum ensAssemblyexceptionTypeFromStr(const AjPStr type)
{
    register ajint i = 0;

    AjEnum etype = ensEAssemblyexceptionTypeNULL;

    for(i = 1; assemblyExceptionType[i]; i++)
        if(ajStrMatchC(type, assemblyExceptionType[i]))
            etype = i;

    if(!etype)
        ajDebug("ensAssemblyexceptionTypeFromStr encountered "
                "unexpected string '%S'.\n", type);

    return etype;
}




/* @func ensAssemblyexceptionTypeToChar ***************************************
**
** Convert an Ensembl Assembly Exception type element into a
** C-type (char*) string.
**
** @param [r] type [const AjEnum] Assembly Exception type enumerator
**
** @return [const char*] Assembly Exception status C-type (char*) string
** @@
******************************************************************************/

const char* ensAssemblyexceptionTypeToChar(const AjEnum type)
{
    register ajint i = 0;

    if(!type)
        return NULL;

    for(i = 1; assemblyExceptionType[i] && (i < type); i++);

    if(!assemblyExceptionType[i])
        ajDebug("ensAssemblyexceptionTypeToChar encountered an "
                "out of boundary error on status %d.\n", type);

    return assemblyExceptionType[i];
}




/* @datasection [EnsPAssemblyexceptionadaptor] Assembly Exception Adaptor *****
**
** Functions for manipulating Ensembl Assembly Exception Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::SliceAdaptor CVS Revision: 1.104
**
** @nam2rule Assemblyexceptionadaptor
**
******************************************************************************/



/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Assembly Exception Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Assembly Exception Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPAssemblyexceptionadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                    Exception Adaptor
** @argrule Ref object [EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                    Exception Adaptor
**
** @valrule * [EnsPAssemblyexceptionadaptor] Ensembl Assembly Exception Adaptor
**
** @fcategory new
******************************************************************************/




/* @funcstatic assemblyExceptionadaptorFetchAllBySQL **************************
**
** Run a SQL statement against an Ensembl Assembly Exception Adaptor and
** consolidate the results into an AJAX List of Ensembl Assembly Exception
** objects.
**
** @param [r] adaptor [EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] aes [AjPList] AJAX List of Ensembl Assembly Exceptions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyExceptionadaptorFetchAllBySQL(
    EnsPAssemblyexceptionadaptor adaptor,
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

    AjEnum type = ensEAssemblyexceptionTypeNULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr typestr = NULL;

    EnsPAssemblyexception ae = NULL;

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

        type = ensEAssemblyexceptionTypeNULL;

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

        type = ensAssemblyexceptionTypeFromStr(typestr);

        if(!type)
            ajFatal("assemblyExceptionadaptorFetchAllBySQL "
                    "got unexpected Assembly Exception type '%S' "
                    "from database.\n",
                    typestr);

        ae = ensAssemblyexceptionNew(adaptor,
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
** @param [u] adaptor [EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyExceptionadaptorCacheInit(
    EnsPAssemblyexceptionadaptor adaptor)
{
    ajuint *Pidentifier = NULL;

    AjPList list = NULL;
    AjPList aes  = NULL;

    AjPStr statement = NULL;

    EnsPAssemblyexception ae = NULL;

    if(!adaptor)
        return ajFalse;

    if(adaptor->CacheBySeqregionIdentifier)
        return ajTrue;

    else
        adaptor->CacheBySeqregionIdentifier =
            ajTableNewFunctionLen(0, ensTableCmpUint, ensTableHashUint);

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
            ajTableFetch(adaptor->CacheBySeqregionIdentifier,
                         (const void *) &(ae->SeqregionIdentifier));

        if(!list)
        {
            AJNEW0(Pidentifier);

            *Pidentifier = ae->SeqregionIdentifier;

            list = ajListNew();

            ajTablePut(adaptor->CacheBySeqregionIdentifier,
                       (void *) Pidentifier,
                       (void *) list);
        }

        ajListPushAppend(list, (void *) ae);
    }

    ajListFree(&aes);

    return ajTrue;
}




/* @func ensAssemblyexceptionadaptorNew ***************************************
**
** Default constructor for an Ensembl Assembly Exception Adaptor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPAssemblyexceptionadaptor] Ensembl Assembly Exception Adaptor
**                                        or NULL
** @@
******************************************************************************/

EnsPAssemblyexceptionadaptor ensAssemblyexceptionadaptorNew(
    EnsPDatabaseadaptor dba)
{
    EnsPAssemblyexceptionadaptor adaptor = NULL;

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
** @fdata [EnsPAssemblyexceptionadaptor]
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
    EnsPAssemblyexception ae = NULL;

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
        ensAssemblyexceptionDel(&ae);

    ajListFree((AjPList *) value);

    return;
}




/* @funcstatic assemblyExceptionadaptorCacheExit ******************************
**
** Clears the Ensembl Assembly Exception cache in the
** Ensembl Assembly Exception Adaptor.
**
** @param [u] adaptor [EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                   Exception Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool assemblyExceptionadaptorCacheExit(
    EnsPAssemblyexceptionadaptor adaptor)
{
    if(!adaptor)
        return ajFalse;

    if(!adaptor->CacheBySeqregionIdentifier)
        return ajTrue;

    ajTableMapDel(adaptor->CacheBySeqregionIdentifier,
                  assemblyExceptionadaptorClearIdentifierCache,
                  NULL);

    ajTableFree(&(adaptor->CacheBySeqregionIdentifier));

    return ajTrue;
}




/* @func ensAssemblyexceptionadaptorDel ***************************************
**
** Default destructor for an Ensembl Assembly Exception Adaptor.
** This function also clears the internal Assembly Exception cache.
**
** @param [r] Padaptor [EnsPAssemblyexceptionadaptor*] Ensembl Assembly
**                                                     Exception Adaptor
**                                                     address
**
** @return [void]
** @@
******************************************************************************/

void ensAssemblyexceptionadaptorDel(EnsPAssemblyexceptionadaptor* Padaptor)
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
** @fdata [EnsPAssemblyexceptionadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Assembly Exception object(s)
** @nam4rule FetchAll Retrieve all Ensembl Assembly Exception objects
** @nam5rule FetchAllBy Retrieve all Ensembl Assembly Exception objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Assembly Exception object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPAssemblyexceptionadaptor] Ensembl Assembly
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

    EnsPAssemblyexception ae = NULL;

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
        ae = (EnsPAssemblyexception) ajListIterGet(iter);

        ajListPushAppend((AjPList) cl,
                         (void *) ensAssemblyexceptionNewRef(ae));
    }

    ajListIterDel(&iter);

    return;
}




/* @func ensAssemblyexceptionadaptorFetchAll **********************************
**
** Fetch all Ensembl Assembly Exceptions.
**
** The caller is responsible for deleting the Ensembl Assembly Exception
** objects before deleting the AJAX List.
**
** @param [r] adaptor [const EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                         Exception Adaptor
** @param [u] aes [AjPList] AJAX List of Ensembl Assembly Exceptions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionadaptorFetchAll(
    const EnsPAssemblyexceptionadaptor adaptor,
    AjPList aes)
{
    if(!adaptor)
        return ajFalse;

    if(!aes)
        return ajFalse;

    ajTableMap(adaptor->CacheBySeqregionIdentifier,
               assemblyExceptionadaptorFetchAll,
               (void *) aes);

    return ajTrue;
}




/* @func ensAssemblyexceptionadaptorFetchAllBySeqregionIdentifier *************
**
** Fetch all Ensembl Assembly Exceptions by a Sequence Region identifier.
**
** The caller is responsible for deleting the Ensembl Assembly Exception
** objects before deleting the AJAX List.
**
** @param [r] adaptor [const EnsPAssemblyexceptionadaptor] Ensembl Assembly
**                                                         Exception Adaptor
** @param [r] srid [ajuint] Ensembl Sequence Region identifier
** @param [u] aes [AjPList] AJAX List of Ensembl Assembly Exceptions
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensAssemblyexceptionadaptorFetchAllBySeqregionIdentifier(
    const EnsPAssemblyexceptionadaptor adaptor,
    ajuint srid,
    AjPList aes)
{
    AjIList iter = NULL;
    AjPList list = NULL;

    EnsPAssemblyexception ae = NULL;

#if AJFALSE
    ajDebug("ensAssemblyexceptionadaptorFetchAllBySeqregionIdentifier\n"
            "  adaptor %p\n"
            "  srid %u\n"
            "  aes %p\n",
            adaptor,
            srid,
            aes);
#endif

    if(!adaptor)
        return ajFalse;

    if(!aes)
        return ajFalse;

    list = (AjPList)
        ajTableFetch(adaptor->CacheBySeqregionIdentifier,
                     (const void *) &srid);

    iter = ajListIterNew(list);

    while(!ajListIterDone(iter))
    {
        ae = (EnsPAssemblyexception) ajListIterGet(iter);

        ajListPushAppend(aes, (void *) ensAssemblyexceptionNewRef(ae));
    }

    ajListIterDel(&iter);

    return ajTrue;
}
