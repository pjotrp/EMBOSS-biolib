/******************************************************************************
** @source Ensembl Quality Check Database functions
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
*****************************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensqcdatabase.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *qcDatabaseClass[] =
{
    (const char *) NULL,
    "unknown",
    "reference",
    "test",
    "both",
    "genome",
    (const char *) NULL
};




static const char *qcDatabaseType[] =
{
    (const char *) NULL,
    "unknown",
    "dna",
    "protein",
    (const char *) NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisAdaptor
ensRegistryGetAnalysisAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPQCDatabaseAdaptor
ensRegistryGetQCDatabaseAdaptor(EnsPDatabaseAdaptor dba);

static AjBool qcDatabaseAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                             const AjPStr statement,
                                             EnsPAssemblyMapper am,
                                             EnsPSlice slice,
                                             AjPList qcdbs);

static AjBool qcDatabaseAdaptorCacheInsert(EnsPQCDatabaseAdaptor qcdba,
                                           EnsPQCDatabase *Pqcdb);

static AjBool qcDatabaseAdaptorCacheInit(EnsPQCDatabaseAdaptor qcdba);

static void qcDatabaseAdaptorCacheClearIdentifier(void **key, void **value,
                                                  void *cl);

static void qcDatabaseAdaptorCacheClearName(void **key, void **value, void *cl);

static AjBool qcDatabaseAdaptorCacheExit(EnsPQCDatabaseAdaptor adaptor);




/* @filesection ensqcdatabase *************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPQCDatabase] QC Database **********************************
**
** Functions for manipulating Ensembl QC Database objects
**
** Bio::EnsEMBL::QC::SequenceDB CVS Revision:
**
** @nam2rule QCDatabase
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Database by pointer.
** It is the responsibility of the user to first destroy any previous
** QC Database. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCDatabase]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPQCDatabase] Ensembl QC Database
** @argrule Ref object [EnsPQCDatabase] Ensembl QC Database
**
** @valrule * [EnsPQCDatabase] Ensembl QC Database
**
** @fcategory new
******************************************************************************/




/* @func ensQCDatabaseNew *****************************************************
**
** Default constructor for an Ensembl QC Database.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::QC::SequenceDB::new
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
** @param [u] name [AjPStr] Name
** @param [u] release [AjPStr] Release
** @param [u] date [AjPStr] Date
** @param [u] format [AjPStr] Format
** @param [r] class [AjEnum] Class
** @param [r] type [AjEnum] Type
** @param [u] species [AjPStr] Ensembl Database Adaptor species element
** @param [r] group [AjEnum] Ensembl Database Adaptor group element
** @param [u] host [AjPStr] Host
** @param [u] directory [AjPStr] Directory
** @param [u] file [AjPStr] File
** @param [u] externalurl [AjPStr] External URL
** @param [u] internalurl [AjPStr] Internal URL
**
** @return [EnsPQCDatabase] Ensembl QC Database or NULL
** @@
******************************************************************************/

EnsPQCDatabase ensQCDatabaseNew(EnsPQCDatabaseAdaptor adaptor,
                                ajuint identifier,
                                EnsPAnalysis analysis,
                                AjPStr name,
                                AjPStr release,
                                AjPStr date,
                                AjPStr format,
                                AjEnum class,
                                AjEnum type,
                                AjPStr species,
                                AjEnum group,
                                AjPStr host,
                                AjPStr directory,
                                AjPStr file,
                                AjPStr externalurl)
{
    EnsPQCDatabase qcdb = NULL;
    
    if(!analysis)
	return NULL;
    
    if(!name)
	return NULL;
    
    if(!release)
	return NULL;
    
    AJNEW0(qcdb);
    
    qcdb->Use = 1;
    
    qcdb->Identifier = identifier;
    
    qcdb->Adaptor = adaptor;
    
    qcdb->Analysis = ensAnalysisNewRef(analysis);
    
    if(name)
	qcdb->Name = ajStrNewRef(name);
    
    if(release)
	qcdb->Release = ajStrNewRef(release);
    
    if(date)
	qcdb->Date = ajStrNewRef(date);
    
    if(format)
	qcdb->Format = ajStrNewRef(format);
    
    qcdb->Class = class;
    
    qcdb->Type = type;
    
    if(species)
	qcdb->Species = ajStrNewRef(species);
    
    qcdb->Group = group;
    
    if(host)
	qcdb->Host = ajStrNewRef(host);
    
    if(directory)
	qcdb->Directory = ajStrNewRef(directory);
    
    if(file)
	qcdb->File = ajStrNewRef(file);
    
    if(externalurl)
	qcdb->ExternalURL = ajStrNewRef(externalurl);
    
    qcdb->InternalURL = NULL;
    
    return qcdb;
}




/* @func ensQCDatabaseNewObj **************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPQCDatabase] Ensembl QC Database
**
** @return [EnsPQCDatabase] Ensembl QC Database or NULL
** @@
******************************************************************************/

EnsPQCDatabase ensQCDatabaseNewObj(const EnsPQCDatabase object)
{
    EnsPQCDatabase qcdb = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(qcdb);
    
    qcdb->Use = 1;
    
    qcdb->Identifier = object->Identifier;
    
    qcdb->Adaptor = object->Adaptor;
    
    if(object->Analysis)
	qcdb->Analysis = ensAnalysisNewRef(object->Analysis);
    
    if(object->Name)
	qcdb->Name = ajStrNewRef(object->Name);
    
    if(object->Release)
	qcdb->Release = ajStrNewRef(object->Release);
    
    if(object->Date)
	qcdb->Date = ajStrNewRef(object->Date);
    
    if(object->Format)
	qcdb->Format = ajStrNewRef(object->Format);
    
    qcdb->Class = object->Class;
    
    qcdb->Type = object->Type;
    
    if(object->Species)
	qcdb->Species = ajStrNewRef(object->Species);
    
    qcdb->Group = object->Group;
    
    if(object->Host)
	qcdb->Host = ajStrNewRef(object->Host);
    
    if(object->Directory)
	qcdb->Directory = ajStrNewRef(object->Directory);
    
    if(object->File)
	qcdb->File = ajStrNewRef(object->File);
    
    if(object->ExternalURL)
	qcdb->ExternalURL = ajStrNewRef(object->ExternalURL);
    
    if(object->InternalURL)
	qcdb->InternalURL = ajStrNewRef(object->InternalURL);
    
    return qcdb;
}




/* @func ensQCDatabaseNewRef **************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
**
** @return [EnsPQCDatabase] Ensembl QC Database or NULL
** @@
******************************************************************************/

EnsPQCDatabase ensQCDatabaseNewRef(EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    qcdb->Use++;
    
    return qcdb;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Database.
**
** @fdata [EnsPQCDatabase]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Database object
**
** @argrule * Pqcdb [EnsPQCDatabase*] Ensembl QC Database object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCDatabaseDel *****************************************************
**
** Default destructor for an Ensembl QC Database.
**
** @param [d] Pqcdb [EnsPQCDatabase*] Ensembl QC Database address
**
** @return [void]
** @@
******************************************************************************/

void ensQCDatabaseDel(EnsPQCDatabase *Pqcdb)
{
    EnsPQCDatabase pthis = NULL;
    
    /*
     ajDebug("ensQCDatabaseDel\n"
	     "  *Pqcdb %p\n",
	     *Pqcdb);
     
     ensQCDatabaseTrace(*Pqcdb, 1);
     */
    
    if(!Pqcdb)
	return;
    
    if(!*Pqcdb)
	return;

    pthis = *Pqcdb;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pqcdb = NULL;
	
	return;
    }
    
    ensAnalysisDel(&pthis->Analysis);
    
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Release);
    ajStrDel(&pthis->Date);
    ajStrDel(&pthis->Format);
    ajStrDel(&pthis->Species);
    ajStrDel(&pthis->Host);
    ajStrDel(&pthis->Directory);
    ajStrDel(&pthis->File);
    ajStrDel(&pthis->ExternalURL);
    ajStrDel(&pthis->InternalURL);
    
    AJFREE(pthis);

    *Pqcdb = NULL;
    
    return;
}



/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl QC Database object.
**
** @fdata [EnsPQCDatabase]
** @fnote None
**
** @nam3rule Get Return QC Database attribute(s)
** @nam4rule GetAdaptor Return the Ensembl QC Database Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
**
** @argrule * qcdb [const EnsPQCDatabase] QC Database
**
** @valrule Adaptor [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Analysis [EnsPAnalysis] Ensembl Analysis
** @valrule Name [AjPStr] Name
** @valrule Release [AjPStr] Release
** @valrule Date [AjPStr] Date
** @valrule Format [AjPStr] Format
** @valrule Class [AjEnum] Class
** @valrule Type [AjEnum] Type
** @valrule Species [AjPStr] Ensembl Database Adaptor species element
** @valrule Group [AjEnum] Ensembl Database Adaptor group element
** @valrule Host [AjPStr] Host
** @valrule Directory [AjPStr] Directory
** @valrule File [AjPStr] File
** @valrule ExternalURL [AjPStr] External URL
** @valrule InternalURL [AjPStr] Internal URL
**
** @fcategory use
******************************************************************************/




/* @func ensQCDatabaseGetAdaptor **********************************************
**
** Get the Ensembl QC Database Adaptor element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @@
******************************************************************************/

EnsPQCDatabaseAdaptor ensQCDatabaseGetAdaptor(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Adaptor;
}




/* @func ensQCDatabaseGetIdentifier *******************************************
**
** Get the SQL database-internal identifier element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensQCDatabaseGetIdentifier(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return 0;
    
    return qcdb->Identifier;
}




/* @func ensQCDatabaseGetAnalysis *********************************************
**
** Get the Ensembl Analysis element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [const EnsPAnalysis] Ensembl Analysis
** @@
******************************************************************************/

const EnsPAnalysis ensQCDatabaseGetAnalysis(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Analysis;
}




/* @func ensQCDatabaseGetName *************************************************
**
** Get the name element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetName(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Name;
}




/* @func ensQCDatabaseGetRelease **********************************************
**
** Get the release element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Release
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetRelease(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Release;
}




/* @func ensQCDatabaseGetDate *************************************************
**
** Get the date element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Date
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetDate(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Date;
}




/* @func ensQCDatabaseGetFormat ***********************************************
**
** Get the format element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Format
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetFormat(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Format;
}




/* @func ensQCDatabaseGetClass ************************************************
**
** Get the class element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjEnum] Class
** @@
******************************************************************************/

AjEnum ensQCDatabaseGetClass(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return ensEQCDatabaseClassNULL;
    
    return qcdb->Class;
}




/* @func ensQCDatabaseGetType *************************************************
**
** Get the type element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjEnum] Type
** @@
******************************************************************************/

AjEnum ensQCDatabaseGetType(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return ensEQCDatabaseTypeNULL;
    
    return qcdb->Type;
}




/* @func ensQCDatabaseGetSpecies **********************************************
**
** Get the Ensembl Database Adaptor species element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Ensembl Database Adaptor species
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetSpecies(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Species;
}




/* @func ensQCDatabaseGetGroup ************************************************
**
** Get the Ensembl Database Adaptor group element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjEnum] Ensembl Database Adaptor group
** @@
******************************************************************************/

AjEnum ensQCDatabaseGetGroup(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return ensEDatabaseAdaptorGroupNULL;
    
    return qcdb->Group;
}




/* @func ensQCDatabaseGetHost *************************************************
**
** Get the host element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Host
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetHost(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Host;
}




/* @func ensQCDatabaseGetDirectory ********************************************
**
** Get the directory element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Directory
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetDirectory(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->Directory;
}




/* @func ensQCDatabaseGetFile *************************************************
**
** Get the file element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] File
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetFile(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->File;
}




/* @func ensQCDatabaseGetExternalURL ******************************************
**
** Get the external URL element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] External URL
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetExternalURL(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->ExternalURL;
}




/* @func ensQCDatabaseGetInternalURL ******************************************
**
** Get the internal URL element of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjPStr] Internal URL
** @@
******************************************************************************/

AjPStr ensQCDatabaseGetInternalURL(const EnsPQCDatabase qcdb)
{
    if(!qcdb)
	return NULL;
    
    return qcdb->InternalURL;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl QC Database object.
**
** @fdata [EnsPQCDatabase]
** @fnote None
**
** @nam3rule Set Set one element of a QC Database
** @nam4rule SetAdaptor Set the Ensembl QC Database Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetRelease Set the release
** @nam4rule SetDate Set the date
** @nam4rule SetFormat Set the format
** @nam4rule SetClass Set the class
** @nam4rule SetType Set the type
** @nam4rule SetSpecies Set the Ensembl Database Adaptor species
** @nam4rule SetGroup Set the Ensembl Database Adaptor group
** @nam4rule SetHost Set the host
** @nam4rule SetDirectory Set the directory
** @nam4rule SetFile Set the file
** @nam4rule SetExternalURL Set the external URL
** @nam4rule SetInternalURL Set the internal URL
**
** @argrule * qcdb [EnsPQCDatabase] Ensembl QC Database
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensQCDatabaseSetAdaptor **********************************************
**
** Set the Ensembl QC Database Adaptor element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [r] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetAdaptor(EnsPQCDatabase qcdb, EnsPQCDatabaseAdaptor qcdba)
{
    if(!qcdb)
	return ajFalse;
    
    qcdb->Adaptor = qcdba;
    
    return ajTrue;
}




/* @func ensQCDatabaseSetIdentifier *******************************************
**
** Set the SQL database-internal identifier element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetIdentifier(EnsPQCDatabase qcdb, ajuint identifier)
{
    if(!qcdb)
	return ajFalse;
    
    qcdb->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensQCDatabaseSetAnalysis *********************************************
**
** Set the Ensembl Analysis element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] analysis [EnsPAnalysis] Ensembl Analysis
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetAnalysis(EnsPQCDatabase qcdb, EnsPAnalysis analysis)
{
    if(!qcdb)
	return ajFalse;
    
    ensAnalysisDel(&qcdb->Analysis);
    
    qcdb->Analysis = ensAnalysisNewRef(analysis);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetName *************************************************
**
** Set the name element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetName(EnsPQCDatabase qcdb, AjPStr name)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->Name);
    
    qcdb->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetRelease **********************************************
**
** Set the release element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] release [AjPStr] Release
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetRelease(EnsPQCDatabase qcdb, AjPStr release)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->Release);
    
    qcdb->Release = ajStrNewRef(release);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetDate *************************************************
**
** Set the date element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] date [AjPStr] Date
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetDate(EnsPQCDatabase qcdb, AjPStr date)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->Date);
    
    qcdb->Date = ajStrNewRef(date);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetFormat ***********************************************
**
** Set the format element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] format [AjPStr] Format
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetFormat(EnsPQCDatabase qcdb, AjPStr format)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->Format);
    
    qcdb->Format = ajStrNewRef(format);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetClass ************************************************
**
** Set the class element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [r] class [AjEnum] Class
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetClass(EnsPQCDatabase qcdb, AjEnum class)
{
    if(!qcdb)
	return ajFalse;
    
    qcdb->Class = class;
    
    return ajTrue;
}




/* @func ensQCDatabaseSetType *************************************************
**
** Set the type element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [r] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetType(EnsPQCDatabase qcdb, AjEnum type)
{
    if(!qcdb)
	return ajFalse;
    
    qcdb->Type = type;
    
    return ajTrue;
}




/* @func ensQCDatabaseSetSpecies **********************************************
**
** Set the Ensembl Database Adaptor species element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] species [AjPStr] Ensembl Database Adaptor species element
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetSpecies(EnsPQCDatabase qcdb, AjPStr species)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->Species);
    
    qcdb->Species = ajStrNewRef(species);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetGroup ************************************************
**
** Set the Ensembl Database Adaptor group element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] group [AjEnum] Ensembl Database Adaptor group element
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetGroup(EnsPQCDatabase qcdb, AjEnum group)
{
    if(!qcdb)
	return ajFalse;
    
    qcdb->Group = group;
    
    return ajTrue;
}




/* @func ensQCDatabaseSetHost *************************************************
**
** Set the host element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] host [AjPStr] Host
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetHost(EnsPQCDatabase qcdb, AjPStr host)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->Host);
    
    qcdb->Host = ajStrNewRef(host);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetDirectory ********************************************
**
** Set the directory element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] directory [AjPStr] Directory
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetDirectory(EnsPQCDatabase qcdb, AjPStr directory)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->Directory);
    
    qcdb->Directory = ajStrNewRef(directory);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetFile *************************************************
**
** Set the file element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] file [AjPStr] File
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetFile(EnsPQCDatabase qcdb, AjPStr file)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->File);
    
    qcdb->File = ajStrNewRef(file);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetExternalURL ******************************************
**
** Set the external URL element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] url [AjPStr] External URL
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetExternalURL(EnsPQCDatabase qcdb, AjPStr url)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->ExternalURL);
    
    qcdb->ExternalURL = ajStrNewRef(url);
    
    return ajTrue;
}




/* @func ensQCDatabaseSetInternalURL ******************************************
**
** Set the internal URL element of an Ensembl QC Database.
**
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
** @param [u] url [AjPStr] Internal URL
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseSetInternalURL(EnsPQCDatabase qcdb, AjPStr url)
{
    if(!qcdb)
	return ajFalse;
    
    ajStrDel(&qcdb->InternalURL);
    
    qcdb->InternalURL = ajStrNewRef(url);
    
    return ajTrue;
}




/* @func ensQCDatabaseGetMemSize **********************************************
**
** Get the memory size in bytes of an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensQCDatabaseGetMemSize(const EnsPQCDatabase qcdb)
{
    ajuint size = 0;
    
    if(!qcdb)
	return 0;
    
    size += (ajuint) sizeof (EnsOQCDatabase);
    
    size += ensAnalysisGetMemSize(qcdb->Analysis);
    
    if(qcdb->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->Name);
    }
    
    if(qcdb->Release)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->Release);
    }
    
    if(qcdb->Date)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->Date);
    }
    
    if(qcdb->Format)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->Format);
    }
    
    if(qcdb->Species)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->Species);
    }
    
    if(qcdb->Host)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->Host);
    }
    
    if(qcdb->Directory)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->Directory);
    }
    
    if(qcdb->File)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->File);
    }
    
    if(qcdb->ExternalURL)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->ExternalURL);
    }
    
    if(qcdb->InternalURL)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(qcdb->InternalURL);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl QC Database object.
**
** @fdata [EnsPQCDatabase]
** @nam3rule Trace Report Ensembl QC Database elements to debug file
**
** @argrule Trace qcdb [const EnsPQCDatabase] Ensembl QC Database
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensQCDatabaseTrace ***************************************************
**
** Trace an Ensembl QC Database.
**
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseTrace(const EnsPQCDatabase qcdb, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!qcdb)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensQCDatabaseTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Analysis %p\n"
	    "%S  Name '%S'\n"
	    "%S  Release '%S'\n"
	    "%S  Date '%S'\n"
	    "%S  Format '%S'\n"
	    "%S  Class %d\n"
	    "%S  Type %d\n"
	    "%S  Species '%S'\n"
	    "%S  Group '%s'\n"
	    "%S  Host '%S'\n"
	    "%S  Directory '%S'\n"
	    "%S  File '%S'\n"
	    "%S  ExternalURL %p\n"
	    "%S  InternalURL '%S'\n",
	    indent, qcdb,
	    indent, qcdb->Use,
	    indent, qcdb->Identifier,
	    indent, qcdb->Adaptor,
	    indent, qcdb->Analysis,
	    indent, qcdb->Name,
	    indent, qcdb->Release,
	    indent, qcdb->Date,
	    indent, qcdb->Format,
	    indent, qcdb->Class,
	    indent, qcdb->Type,
	    indent, qcdb->Species,
	    indent, ensDatabaseAdaptorGroupToChar(qcdb->Group),
	    indent, qcdb->Host,
	    indent, qcdb->Directory,
	    indent, qcdb->File,
	    indent, qcdb->ExternalURL,
	    indent, qcdb->InternalURL);
    
    ensAnalysisTrace(qcdb->Analysis, 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensQCDatabaseClassFromStr ********************************************
**
** Convert an AJAX String into an Ensembl QC Database class element.
**
** @param [r] class [const AjPStr] Class string
**
** @return [AjEnum] Ensembl QC Database class element or
**                  ensEQCDatabaseClassNULL
** @@
******************************************************************************/

AjEnum ensQCDatabaseClassFromStr(const AjPStr class)
{
    register ajuint i = 0;
    
    AjEnum eclass = ensEQCDatabaseClassNULL;
    
    for(i = 1; qcDatabaseClass[i]; i++)
	if(ajStrMatchCaseC(class, qcDatabaseClass[i]))
	    eclass = i;
    
    if(!eclass)
	ajDebug("ensQCDatabaseClassFromStr encountered "
		"unexpected string '%S'.\n", class);
    
    return eclass;
}




/* @func ensQCDatabaseTypeFromStr *********************************************
**
** Convert an AJAX String into an Ensembl QC Database type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [AjEnum] Ensembl QC Database type element or
**                  ensEQCDatabaseTypeNULL
** @@
******************************************************************************/

AjEnum ensQCDatabaseTypeFromStr(const AjPStr type)
{
    register ajuint i = 0;
    
    AjEnum etype = ensEQCDatabaseTypeNULL;
    
    for(i = 1; qcDatabaseType[i]; i++)
	if(ajStrMatchCaseC(type, qcDatabaseType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensQCDatabaseTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensQCDatabaseClassToChar *********************************************
**
** Convert an Ensembl QC Database class element into a C-type (char*) string.
**
** @param [r] class [const AjEnum] Ensembl QC Database class enumerator
**
** @return [const char*] Ensembl QC Database class C-type (char*) string
** @@
******************************************************************************/

const char* ensQCDatabaseClassToChar(const AjEnum class)
{
    register ajint i = 0;
    
    if(!class)
	return NULL;
    
    for(i = 1; qcDatabaseClass[i] && (i < class); i++);
    
    if(!qcDatabaseClass[i])
	ajDebug("ensQCDatabaseClassToChar encountered an "
		"out of boundary error on group %d.\n", class);
    
    return qcDatabaseClass[i];
}




/* @func ensQCDatabaseTypeToChar **********************************************
**
** Convert an Ensembl QC Database type element into a C-type (char*) string.
**
** @param [r] type [const AjEnum] Ensembl QC Database type enumerator
**
** @return [const char*] Ensembl QC Database type C-type (char*) string
** @@
******************************************************************************/

const char* ensQCDatabaseTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; qcDatabaseType[i] && (i < type); i++);
    
    if(!qcDatabaseType[i])
	ajDebug("ensQCDatabaseTypeToChar encountered an "
		"out of boundary error on group %d.\n", type);
    
    return qcDatabaseType[i];
}




/* @func ensQCDatabaseMatch ***************************************************
**
** Tests for matching two Ensembl QC Databases.
**
** @param [r] qcdb1 [const EnsPQCDatabase] First Ensembl QC Database
** @param [r] qcdb2 [const EnsPQCDatabase] Second Ensembl QC Database
**
** @return [AjBool] ajTrue if the Ensembl QC Databases are equal
** @@
** The comparison is based on initial pointer equality and if that fails
** each element is compared.
******************************************************************************/

AjBool ensQCDatabaseMatch(const EnsPQCDatabase qcdb1,
                          const EnsPQCDatabase qcdb2)
{
    if(!qcdb1)
        return ajFalse;
    
    if(!qcdb2)
        return ajFalse;
    
    if(qcdb1 == qcdb2)
	return ajTrue;
    
    if(qcdb1->Identifier != qcdb2->Identifier)
	return ajFalse;
    
    if((qcdb1->Adaptor && qcdb2->Adaptor) &&
	(qcdb1->Adaptor != qcdb2->Adaptor))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->Name, qcdb2->Name))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->Release, qcdb2->Release))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->Date, qcdb2->Date))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->Format, qcdb2->Format))
	return ajFalse;
    
    if(qcdb1->Class != qcdb2->Class)
	return ajFalse;
    
    if(qcdb1->Type != qcdb2->Type)
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->Species, qcdb2->Species))
	return ajFalse;
    
    if(qcdb1->Group != qcdb2->Group)
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->Host, qcdb2->Host))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->Directory, qcdb2->Directory))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->File, qcdb2->File))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->ExternalURL, qcdb2->ExternalURL))
	return ajFalse;
    
    if(!ajStrMatchCaseS(qcdb1->InternalURL, qcdb2->InternalURL))
	return ajFalse;
    
    return ajTrue;
}




/* @datasection [EnsPQCDatabaseAdaptor] QC Database Adaptor *******************
**
** Functions for manipulating Ensembl QC Database Adaptor objects
**
** Bio::EnsEMBL::QC::DBSQL::SequenceDBAdaptor CVS Revision:
**
** @nam2rule QCDatabaseAdaptor
**
******************************************************************************/

static const char *qcDatabaseAdaptorTables[] =
{
    "sequence_db",
    (const char *) NULL
};




static const char *qcDatabaseAdaptorColumns[] =
{
    "sequence_db.sequence_db_id",
    "sequence_db.analysis_id",
    "sequence_db.name",
    "sequence_db.dbrelease",
    "sequence_db.date",
    "sequence_db.format",
    "sequence_db.type",
    "sequence_db.class",
    "sequence_db.species",
    "sequence_db.dbgroup",
    "sequence_db.host",
    "sequence_db.directory",
    "sequence_db.file",
    "sequence_db.external_url",
    (const char *) NULL
};




static EnsOBaseAdaptorLeftJoin qcDatabaseAdaptorLeftJoin[] =
{
    {(const char*) NULL, (const char*) NULL}
};




static const char *qcDatabaseAdaptorDefaultCondition =
(const char*) NULL;




static const char *qcDatabaseAdaptorFinalCondition =
(const char *) NULL;




/* @funcstatic qcDatabaseAdaptorFetchAllBySQL *********************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl QC Database objects.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] qcdbs [AjPList] AJAX List of Ensembl QC Databases
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcDatabaseAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                             const AjPStr statement,
                                             EnsPAssemblyMapper am,
                                             EnsPSlice slice,
                                             AjPList qcdbs)
{
    ajuint identifier = 0;
    ajuint analysisid = 0;
    
    AjEnum egroup = ensEDatabaseAdaptorGroupNULL;
    AjEnum eclass = ensEQCDatabaseClassNULL;
    AjEnum etype  = ensEQCDatabaseTypeNULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr name        = NULL;
    AjPStr release     = NULL;
    AjPStr date        = NULL;
    AjPStr format      = NULL;
    AjPStr type        = NULL;
    AjPStr class       = NULL;
    AjPStr species     = NULL;
    AjPStr group       = NULL;
    AjPStr host        = NULL;
    AjPStr directory   = NULL;
    AjPStr file        = NULL;
    AjPStr externalurl = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisAdaptor aa = NULL;
    
    EnsPQCDatabase qcdb         = NULL;
    EnsPQCDatabaseAdaptor qcdba = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!qcdbs)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisAdaptor(dba);
    
    qcdba = ensRegistryGetQCDatabaseAdaptor(dba);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier  = 0;
	analysisid  = 0;
	name        = ajStrNew();
	release     = ajStrNew();
	date        = ajStrNew();
	format      = ajStrNew();
	type        = ajStrNew();
	class       = ajStrNew();
	species     = ajStrNew();
	group       = ajStrNew();
	host        = ajStrNew();
	directory   = ajStrNew();
	file        = ajStrNew();
	externalurl = ajStrNew();
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, &identifier);
        ajSqlColumnToUint(sqlr, &analysisid);
        ajSqlColumnToStr(sqlr, &name);
        ajSqlColumnToStr(sqlr, &release);
        ajSqlColumnToStr(sqlr, &date);
        ajSqlColumnToStr(sqlr, &format);
        ajSqlColumnToStr(sqlr, &type);
        ajSqlColumnToStr(sqlr, &class);
        ajSqlColumnToStr(sqlr, &species);
        ajSqlColumnToStr(sqlr, &group);
        ajSqlColumnToStr(sqlr, &host);
        ajSqlColumnToStr(sqlr, &directory);
        ajSqlColumnToStr(sqlr, &file);
        ajSqlColumnToStr(sqlr, &externalurl);
	
	ensAnalysisAdaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	etype = ensQCDatabaseTypeFromStr(type);
	
	eclass = ensQCDatabaseClassFromStr(class);
	
	egroup = ensDatabaseAdaptorGroupFromStr(group);
	
	qcdb = ensQCDatabaseNew(qcdba,
				identifier,
				analysis,
				name,
				release,
				date,
				format,
				etype,
				eclass,
				species,
				egroup,
				host,
				directory,
				file,
				externalurl);
	
	ajListPushAppend(qcdbs, (void *) qcdb);
	
	ensAnalysisDel(&analysis);
	
	ajStrDel(&name);
	ajStrDel(&release);
	ajStrDel(&date);
	ajStrDel(&format);
	ajStrDel(&type);
	ajStrDel(&class);
	ajStrDel(&species);
	ajStrDel(&group);
	ajStrDel(&host);
	ajStrDel(&directory);
	ajStrDel(&file);
	ajStrDel(&externalurl);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @funcstatic qcDatabaseAdaptorCacheInsert ***********************************
**
** Insert an Ensembl QC Database into the QC Database Adaptor-internal cache.
** If a QC Database with the same name element is already present in the
** adaptor cache, the QC Database is deleted and a pointer to the cached
** QC Database is returned.
**
** @param [u] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [u] Pqcdb [EnsPQCDatabase*] Ensembl QC Database address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcDatabaseAdaptorCacheInsert(EnsPQCDatabaseAdaptor qcdba,
                                           EnsPQCDatabase *Pqcdb)
{
    ajuint *Pidentifier = NULL;
    
    AjPStr key = NULL;
    
    EnsPQCDatabase qcdb1 = NULL;
    EnsPQCDatabase qcdb2 = NULL;
    
    if(!qcdba)
        return ajFalse;
    
    if(!qcdba->CacheByIdentifier)
        return ajFalse;
    
    if(!qcdba->CacheByName)
        return ajFalse;
    
    if(!Pqcdb)
        return ajFalse;
    
    if(!*Pqcdb)
        return ajFalse;
    
    /* Search the identifer cache. */
    
    qcdb1 = (EnsPQCDatabase)
	ajTableFetch(qcdba->CacheByIdentifier,
		     (const void *) &((*Pqcdb)->Identifier));
    
    /* Search the name cache. */
    
    key = ajFmtStr("%S:%S", (*Pqcdb)->Name, (*Pqcdb)->Release);
    
    qcdb2 = (EnsPQCDatabase)
	ajTableFetch(qcdba->CacheByName, (const void *) key);
    
    if((!qcdb1) && (! qcdb2))
    {
	/* Insert into the identifier cache. */
	
	AJNEW0(Pidentifier);
	
	*Pidentifier = (*Pqcdb)->Identifier;
	
	ajTablePut(qcdba->CacheByIdentifier,
		   (void *) Pidentifier,
		   (void *) ensQCDatabaseNewRef(*Pqcdb));
	
	/* Insert into the name cache. */
	
        ajTablePut(qcdba->CacheByName,
		   (void *) ajStrNewS(key),
		   (void *) ensQCDatabaseNewRef(*Pqcdb));
    }
    
    if(qcdb1 && qcdb2 && (qcdb1 == qcdb2))
    {
        ajDebug("qcDatabaseAdaptorCacheInsert replaced "
		"Ensembl QC Database %p with "
		"one already cached %p.\n",
		*Pqcdb, qcdb1);
	
	ensQCDatabaseDel(Pqcdb);
	
	Pqcdb = &qcdb1;
    }
    
    if(qcdb1 && qcdb2 && (qcdb1 != qcdb2))
	ajDebug("qcDatabaseAdaptorCacheInsert detected "
		"Ensembl QC Databases in the "
		"identifier and name cache with identical names "
		"('%S' and '%S') but different addresses (%p and %p).\n",
		qcdb1->Name, qcdb2->Name, qcdb1, qcdb2);
    
    if(qcdb1 && (!qcdb2))
        ajDebug("qcDatabaseAdaptorCacheInsert detected an "
		"Ensembl QC Database "
		"in the identifier, but not in the name cache.\n");
    
    if((!qcdb1) && qcdb2)
        ajDebug("qcDatabaseAdaptorCacheInsert detected an "
		"Ensembl QC Database "
		"in the name, but not in the identifier cache.\n");
    
    ajStrDel(&key);
    
    return ajTrue;
}




/* @funcstatic qcDatabaseAdaptorCacheInit *************************************
**
** Initialise the internal QC Database cache of an Ensembl QC Database Adaptor.
**
** @param [u] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcDatabaseAdaptorCacheInit(EnsPQCDatabaseAdaptor qcdba)
{
    AjPList qcdbs = NULL;
    
    EnsPQCDatabase qcdb = NULL;
    
    if(!qcdba)
	return ajFalse;
    
    if(qcdba->CacheByIdentifier)
	return ajFalse;
    else
	qcdba->CacheByIdentifier =
	    ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    if(qcdba->CacheByName)
	return ajFalse;
    else
	qcdba->CacheByName = ajTablestrNewLen(0);
    
    qcdbs = ajListNew();
    
    ensBaseAdaptorGenericFetch(qcdba->Adaptor,
			       (const AjPStr) NULL,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcdbs);
    
    while(ajListPop(qcdbs, (void **) &qcdb))
    {
	qcDatabaseAdaptorCacheInsert(qcdba, &qcdb);
	
	ensQCDatabaseDel(&qcdb);
    }
    
    ajListFree(&qcdbs);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl QC Database Adaptor by
** pointer. It is the responsibility of the user to first destroy any previous
** QC Database Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPQCDatabaseAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @argrule Ref object [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
**
** @valrule * [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensQCDatabaseAdaptorNew **********************************************
**
** Default constructor for an Ensembl QC Database Adaptor.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor or NULL
** @@
******************************************************************************/

EnsPQCDatabaseAdaptor ensQCDatabaseAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPQCDatabaseAdaptor qcdba = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(qcdba);
    
    qcdba->Adaptor =
	ensBaseAdaptorNew(dba,
			  qcDatabaseAdaptorTables,
			  qcDatabaseAdaptorColumns,
			  qcDatabaseAdaptorLeftJoin,
			  qcDatabaseAdaptorDefaultCondition,
			  qcDatabaseAdaptorFinalCondition,
			  qcDatabaseAdaptorFetchAllBySQL);
    
    qcDatabaseAdaptorCacheInit(qcdba);
    
    return qcdba;
}




/* @funcstatic qcDatabaseAdaptorCacheClearIdentifier **************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl QC Database
** Adaptor-internal QC Database cache. This function deletes the
** unsigned integer identifier key and the Ensembl QC Database value
** data.
**
** @param [u] key [void **] AJAX unsigned integer key data address
** @param [u] value [void **] Ensembl QC Database value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void qcDatabaseAdaptorCacheClearIdentifier(void **key, void **value,
                                                  void *cl)
{
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
    
    ensQCDatabaseDel((EnsPQCDatabase *) value);
    
    return;
}




/* @funcstatic qcDatabaseAdaptorCacheClearName ********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl QC Database
** Adaptor-internal QC Database cache. This function deletes the name
** AJAX String key data and the Ensembl QC Database value data.
**
** @param [u] key [void **] AJAX String key data address
** @param [u] value [void **] Ensembl QC Database value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void qcDatabaseAdaptorCacheClearName(void **key, void **value, void *cl)
{
    if(!key)
	return;
    
    if(!*key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    (void) cl;
    
    ajStrDel((AjPStr *) key);
    
    ensQCDatabaseDel((EnsPQCDatabase *) value);
    
    return;
}




/* @funcstatic qcDatabaseAdaptorCacheExit *************************************
**
** Clears the internal QC Database cache of an Ensembl QC Database Adaptor.
**
** @param [u] adaptor [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool qcDatabaseAdaptorCacheExit(EnsPQCDatabaseAdaptor adaptor)
{
    if(!adaptor)
        return ajFalse;
    
    /* Clear and delete the identifier cache. */
    
    ajTableMapDel(adaptor->CacheByIdentifier,
		  qcDatabaseAdaptorCacheClearIdentifier,
		  NULL);
    
    ajTableFree(&(adaptor->CacheByIdentifier));
    
    /* Clear and delete the name cache. */
    
    ajTableMapDel(adaptor->CacheByName,
		  qcDatabaseAdaptorCacheClearName,
		  NULL);
    
    ajTableFree(&(adaptor->CacheByName));
    
    return ajTrue;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl QC Database Adaptor.
**
** @fdata [EnsPQCDatabaseAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a QC Database Adaptor object
**
** @argrule * Pqcdba [EnsPQCDatabaseAdaptor*] QC Database Adaptor
**                                             object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensQCDatabaseAdaptorDel **********************************************
**
** Default destructor for an Ensembl QC Database Adaptor.
**
** @param [d] Pqcdba [EnsPQCDatabaseAdaptor*] Ensembl QC Database Adaptor
**                                             address
**
** @return [void]
** @@
******************************************************************************/

void ensQCDatabaseAdaptorDel(EnsPQCDatabaseAdaptor *Pqcdba)
{
    EnsPQCDatabaseAdaptor pthis = NULL;
    
    /*
     ajDebug("ensQCDatabaseAdaptorDel\n"
	     "  *Pqcdba %p\n",
	     *Pqcdba);
     */
    
    if(!Pqcdba)
	return;
    
    if(!*Pqcdba)
	return;

    pthis = *Pqcdba;
    
    qcDatabaseAdaptorCacheExit(pthis);
    
    ensBaseAdaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Pqcdba = NULL;
    
    return;
}




/* @func ensQCDatabaseAdaptorFetchByIdentifier ********************************
**
** Fetch an Ensembl QC Database by its SQL database-internal identifier.
**
** @param [r] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pqcdb [EnsPQCDatabase*] Ensembl QC Database address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseAdaptorFetchByIdentifier(EnsPQCDatabaseAdaptor qcdba,
                                             ajuint identifier,
                                             EnsPQCDatabase *Pqcdb)
{
    if(!qcdba)
	return ajFalse;
    
    if(!Pqcdb)
	return ajFalse;
    
    /*
    ** Initally, search the identifier cache.
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */
    
    *Pqcdb = (EnsPQCDatabase)
	ajTableFetch(qcdba->CacheByIdentifier, (const void *) &identifier);
    
    if(*Pqcdb)
    {
	ensQCDatabaseNewRef(*Pqcdb);
	
	return ajTrue;
    }
    
    /*
    ** TODO: Try to retrieve directly from the database?
    ** 
    ** *Pqcdb = (EnsPQCDatabase)
    ** ensBaseAdaptorFetchByIdentifier(adaptor->Adaptor, identifier);
    ** 
    ** qcDatabaseAdaptorCacheInsert(adaptor, Pqcdb);
    */
    
    return ajTrue;
}




/* @func ensQCDatabaseAdaptorFetchByName **************************************
**
** Fetch an Ensembl QC Database by its name and release.
**
** @param [r] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [r] name [const AjPStr] Ensembl QC Database name
** @param [r] release [const AjPStr] Ensembl QC Database release
** @param [wP] Pqcdb [EnsPQCDatabase*] Ensembl QC Database address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseAdaptorFetchByName(EnsPQCDatabaseAdaptor qcdba,
                                       const AjPStr name,
                                       const AjPStr release,
                                       EnsPQCDatabase *Pqcdb)
{
    char *txtname    = NULL;
    char *txtrelease = NULL;
    
    AjPList qcdbs = NULL;
    
    AjPStr key        = NULL;
    AjPStr constraint = NULL;
    
    EnsPQCDatabase qcdb = NULL;
    
    if(!qcdba)
        return ajFalse;
    
    if((!name) && (!ajStrGetLen(name)))
        return ajFalse;
    
    if((!release) && (!ajStrGetLen(release)))
        return ajFalse;
    
    if(!Pqcdb)
	return ajFalse;
    
    /*
    ** Initally, search the name cache.
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */
    
    key = ajFmtStr("%S:%S", name, release);
    
    *Pqcdb = (EnsPQCDatabase)
	ajTableFetch(qcdba->CacheByName, (const void *) key);
    
    ajStrDel(&key);
    
    if(*Pqcdb)
    {
	ensQCDatabaseNewRef(*Pqcdb);
	
	return ajTrue;
    }
    
    /* In case of a cache miss, query the database. */
    
    ensBaseAdaptorEscapeCS(qcdba->Adaptor, &txtname, name);
    
    ensBaseAdaptorEscapeCS(qcdba->Adaptor, &txtrelease, release);
    
    constraint = ajFmtStr("sequence_db.name = '%s' "
			  "AND "
			  "sequence_db.release = '%s'",
			  txtname,
			  txtrelease);
    
    ajCharDel(&txtname);
    
    ajCharDel(&txtrelease);
    
    qcdbs = ajListNew();
    
    ensBaseAdaptorGenericFetch(qcdba->Adaptor,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       qcdbs);
    
    if(ajListGetLength(qcdbs) > 1)
	ajWarn("ensQCDatabaseAdaptorFetchByName got more than "
	       "one Ensembl QC Database for(UNIQUE) name '%S' "
	       "and release '%S'.\n", name, release);
    
    ajListPop(qcdbs, (void **) Pqcdb);
    
    qcDatabaseAdaptorCacheInsert(qcdba, Pqcdb);
    
    while(ajListPop(qcdbs, (void **) &qcdb))
    {
	qcDatabaseAdaptorCacheInsert(qcdba, &qcdb);
	
	ensQCDatabaseDel(&qcdb);
    }
    
    ajListFree(&qcdbs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensQCDatabaseAdaptorFetchAllByClassType ******************************
**
** Fetch an Ensembl QC Database by its class and type.
**
** @param [r] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [r] class [AjEnum] Ensembl QC Database class
** @param [r] type [AjEnum] Ensembl QC Database type
** @param [u] qcdbs [AjPList] AJAX List of Ensembl QC Databases
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseAdaptorFetchAllByClassType(EnsPQCDatabaseAdaptor qcdba,
                                               AjEnum class,
                                               AjEnum type,
                                               AjPList qcdbs)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjBool cmatch = AJFALSE;
    AjBool tmatch = AJFALSE;
    
    if(!qcdba)
	return ajFalse;
    
    if(!qcdbs)
	return ajFalse;
    
    ajTableToarray(qcdba->CacheByIdentifier, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	/* Match the class element if one has been provided. */
	
	if(class)
	{
	    if(((EnsPQCDatabase) valarray[i])->Class == class)
		cmatch = ajTrue;
	    else
		cmatch = ajFalse;
	}
	else
	    cmatch = ajTrue;
	
	/* Match the type element if one has been provided. */
	
	if(type)
	{
	    if(((EnsPQCDatabase) valarray[i])->Type == type)
		tmatch = ajTrue;
	    else
		tmatch = ajFalse;
	}
	else
	    tmatch = ajTrue;
	
	if(cmatch && tmatch)
	    ajListPushAppend(qcdbs, (void *)
			     ensQCDatabaseNewRef(valarray[i]));
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return ajTrue;
}




/* @func ensQCDatabaseAdaptorStore ********************************************
**
** Store an Ensembl QC Database.
**
** @param [r] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [u] qcdb [EnsPQCDatabase] Ensembl QC Database
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseAdaptorStore(EnsPQCDatabaseAdaptor qcdba,
                                 EnsPQCDatabase qcdb)
{
    char *txtname        = NULL;
    char *txtrelease     = NULL;
    char *txtdate        = NULL;
    char *txtformat      = NULL;
    char *txtspecies     = NULL;
    char *txthost        = NULL;
    char *txtdirectory   = NULL;
    char *txtfile        = NULL;
    char *txtexternalurl = NULL;
    
    AjBool value = ajFalse;
    
    AjPSqlStatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!qcdba)
	return ajFalse;
    
    if(!qcdb)
	return ajFalse;
    
    if(ensQCDatabaseGetAdaptor(qcdb) &&
	ensQCDatabaseGetIdentifier(qcdb))
	return ajFalse;
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(qcdba->Adaptor);
    
    ensDatabaseAdaptorEscapeCS(dba, &txtname, qcdb->Name);
    ensDatabaseAdaptorEscapeCS(dba, &txtrelease, qcdb->Release);
    ensDatabaseAdaptorEscapeCS(dba, &txtdate, qcdb->Date);
    ensDatabaseAdaptorEscapeCS(dba, &txtformat, qcdb->Format);
    ensDatabaseAdaptorEscapeCS(dba, &txtspecies, qcdb->Species);
    ensDatabaseAdaptorEscapeCS(dba, &txthost, qcdb->Host);
    ensDatabaseAdaptorEscapeCS(dba, &txtdirectory, qcdb->Directory);
    ensDatabaseAdaptorEscapeCS(dba, &txtfile, qcdb->File);
    ensDatabaseAdaptorEscapeCS(dba, &txtexternalurl, qcdb->ExternalURL);
    
    statement = ajFmtStr("INSERT IGNORE INTO "
			 "sequence_db "
			 "SET "
			 "sequence_db.analysis_id = %u, "
			 "sequence_db.name = '%s', "
			 "sequence_db.dbrelease = '%s', "
			 "sequence_db.date = '%s', "
			 "sequence_db.format = '%s' "
			 "sequence_db.type = '%s', "
			 "sequence_db.class = '%s', "
			 "sequence_db.species = '%s', "
			 "sequence_db.dbgroup = '%s', "
			 "sequence_db.host = '%s', "
			 "sequence_db.directory = '%s', "
			 "sequence_db.file = '%s', "
			 "sequence_db.external_url = '%s'",
			 ensAnalysisGetIdentifier(qcdb->Analysis),
			 txtname,
			 txtrelease,
			 txtdate,
			 txtformat,
			 ensQCDatabaseTypeToChar(qcdb->Type),
			 ensQCDatabaseClassToChar(qcdb->Class),
			 txtspecies,
			 ensDatabaseAdaptorGroupToChar(qcdb->Group),
			 txthost,
			 txtdirectory,
			 txtfile,
			 txtexternalurl);
    
    ajCharDel(&txtname);
    ajCharDel(&txtrelease);
    ajCharDel(&txtdate);
    ajCharDel(&txtformat);
    ajCharDel(&txtspecies);
    ajCharDel(&txthost);
    ajCharDel(&txtdirectory);
    ajCharDel(&txtfile);
    ajCharDel(&txtexternalurl);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    if(ajSqlStatementGetAffectedRows(sqls))
    {
	ensQCDatabaseSetIdentifier(qcdb, ajSqlStatementGetIdentifier(sqls));
	
	ensQCDatabaseSetAdaptor(qcdb, qcdba);
	
	value = ajTrue;
    }
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCDatabaseAdaptorUpdate *******************************************
**
** Update an Ensembl QC Database.
**
** @param [r] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseAdaptorUpdate(EnsPQCDatabaseAdaptor qcdba,
                                  const EnsPQCDatabase qcdb)
{
    char *txtname        = NULL;
    char *txtrelease     = NULL;
    char *txtdate        = NULL;
    char *txtformat      = NULL;
    char *txtspecies     = NULL;
    char *txthost        = NULL;
    char *txtdirectory   = NULL;
    char *txtfile        = NULL;
    char *txtexternalurl = NULL;
    
    AjBool value = ajFalse;
    
    AjPSqlStatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!qcdba)
	return ajFalse;
    
    if(!qcdb)
	return ajFalse;
    
    if(!ensQCDatabaseGetIdentifier(qcdb))
	return ajFalse;
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(qcdba->Adaptor);
    
    ensDatabaseAdaptorEscapeCS(dba, &txtname, qcdb->Name);
    ensDatabaseAdaptorEscapeCS(dba, &txtrelease, qcdb->Release);
    ensDatabaseAdaptorEscapeCS(dba, &txtdate, qcdb->Date);
    ensDatabaseAdaptorEscapeCS(dba, &txtformat, qcdb->Format);
    ensDatabaseAdaptorEscapeCS(dba, &txtspecies, qcdb->Species);
    ensDatabaseAdaptorEscapeCS(dba, &txthost, qcdb->Host);
    ensDatabaseAdaptorEscapeCS(dba, &txtdirectory, qcdb->Directory);
    ensDatabaseAdaptorEscapeCS(dba, &txtfile, qcdb->File);
    ensDatabaseAdaptorEscapeCS(dba, &txtexternalurl, qcdb->ExternalURL);
    
    statement = ajFmtStr("UPDATE IGNORE "
			 "sequence_db "
			 "SET "
			 "sequence_db.analysis_id = %u, "
			 "sequence_db.name = '%s', "
			 "sequence_db.dbrelease = '%s', "
			 "sequence_db.date = '%s', "
			 "sequence_db.format = '%s' "
			 "sequence_db.type = '%s', "
			 "sequence_db.class = '%s', "
			 "sequence_db.species = '%s', "
			 "sequence_db.dbgroup = '%s', "
			 "sequence_db.host = '%s', "
			 "sequence_db.directory = '%s', "
			 "sequence_db.file = '%s', "
			 "sequence_db.external_url = '%s' "
			 "WHERE "
			 "sequence_db.sequence_db_id = %u",
			 ensAnalysisGetIdentifier(qcdb->Analysis),
			 txtname,
			 txtrelease,
			 txtdate,
			 txtformat,
			 ensQCDatabaseTypeToChar(qcdb->Type),
			 ensQCDatabaseClassToChar(qcdb->Class),
			 txtspecies,
			 ensDatabaseAdaptorGroupToChar(qcdb->Group),
			 txthost,
			 txtdirectory,
			 txtfile,
			 txtexternalurl,
			 qcdb->Identifier);
    
    ajCharDel(&txtname);
    ajCharDel(&txtrelease);
    ajCharDel(&txtdate);
    ajCharDel(&txtformat);
    ajCharDel(&txtspecies);
    ajCharDel(&txthost);
    ajCharDel(&txtdirectory);
    ajCharDel(&txtfile);
    ajCharDel(&txtexternalurl);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    if(ajSqlStatementGetAffectedRows(sqls))
	value = ajTrue;
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensQCDatabaseAdaptorDelete *******************************************
**
** Delete an Ensembl QC Database.
**
** @param [r] qcdba [EnsPQCDatabaseAdaptor] Ensembl QC Database Adaptor
** @param [r] qcdb [const EnsPQCDatabase] Ensembl QC Database
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensQCDatabaseAdaptorDelete(EnsPQCDatabaseAdaptor qcdba,
                                  const EnsPQCDatabase qcdb)
{
    AjBool value = ajFalse;
    
    AjPSqlStatement sqls = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!qcdba)
	return ajFalse;
    
    if(!qcdb)
	return ajFalse;
    
    if(!ensQCDatabaseGetIdentifier(qcdb))
	return ajFalse;
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(qcdba->Adaptor);
    
    statement = ajFmtStr("DELETE FROM "
			 "sequence_db "
			 "WHERE "
			 "sequence_db.sequence_db_id = %u",
			 qcdb->Identifier);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    if(ajSqlStatementGetAffectedRows(sqls))
	value = ajTrue;
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return value;
}
