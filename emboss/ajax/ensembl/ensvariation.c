/******************************************************************************
** @source Ensembl Genetic Variation functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.15 $
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

#include "ensvariation.h"
#include "ensgene.h"




/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== global variables ========================== */
/* ==================================================================== */




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static AjBool gvsourceadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                           const AjPStr statement,
                                           EnsPAssemblymapper am,
                                           EnsPSlice slice,
                                           AjPList gvss);

static AjBool gvsourceadaptorCacheInsert(EnsPGvsourceadaptor gvsa,
                                         EnsPGvsource *Pgvs);

static AjBool gvsourceadaptorCacheInit(EnsPGvsourceadaptor gvsa);

static void gvsourceadaptorCacheClearIdentifier(void **key,
                                                void **value,
                                                void *cl);

static void gvsourceadaptorCacheClearName(void **key,
                                          void **value,
                                          void *cl);

static AjBool gvsourceadaptorCacheExit(EnsPGvsourceadaptor gvsa);

static void gvsourceadaptorFetchAll(const void *key,
                                    void **value,
                                    void *cl);

static void gvvariationClearGenes(void **key,
                                  void **value,
                                  void *cl);

static void tableClearGvvariations(void **key,
                                   void **value,
                                   void *cl);

static void gvvariationadaptorClearAlleles(void **key,
                                           void **value,
                                           void *cl);

static void gvvariationadaptorClearPopulations(void **key,
                                               void **value,
                                               void *cl);

static void gvvariationadaptorClearSynonyms(void **key,
                                            void **value,
                                            void *cl);

static AjBool gvvariationadaptorFetchAllBySQL(EnsPGvvariationadaptor gvva,
                                              const AjPStr statement,
                                              AjPList gvvs);

static int gvvariationlistCompareIdentifier(const void *P1, const void *P2);

static void gvvariationlistDelete(void **PP1, void *cl);

static AjBool gvvariationadaptorFetchFlankFromCore(EnsPGvvariationadaptor gvva,
                                                   ajuint srid,
                                                   ajint srstart,
                                                   ajint srend,
                                                   ajint srstrand,
                                                   AjPStr *Psequence);

static int gvvariationfeatureCompareStartAscending(const void* P1,
                                                   const void* P2);

static int gvvariationfeatureCompareStartDescending(const void* P1,
                                                    const void* P2);

static AjBool gvvariationfeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                     const AjPStr statement,
                                                     EnsPAssemblymapper am,
                                                     EnsPSlice slice,
                                                     AjPList gvvfs);

static void* gvvariationfeatureadaptorCacheReference(void *value);

static void gvvariationfeatureadaptorCacheDelete(void **value);

static ajulong gvvariationfeatureadaptorCacheSize(const void *value);

static EnsPFeature gvvariationfeatureadaptorGetFeature(const void *value);

static int gvvariationsetlistCompareIdentifier(const void *P1, const void *P2);

static void gvvariationsetlistDelete(void **PP1, void *cl);

static void gypopulationgenotypeadaptorClearGvpstogvpgs(void **key,
                                                        void **value,
                                                        void *cl);

static void gypopulationgenotypeadaptorClearGvvstogvpgs(void **key,
                                                        void **value,
                                                        void *cl);




/* ==================================================================== */
/* ===================== All functions by section ===================== */
/* ==================================================================== */

/* @filesection ensvariation **************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPGvallele] Genetic Variation Allele ***********************
**
** Functions for manipulating Ensembl Genetic Variation Allele objects
**
** @cc Bio::EnsEMBL::Variation::Allele CVS Revision: 1.5
**
** @nam2rule Gvallele
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Allele by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Allele. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvallele] Ensembl Genetic Variation Allele
** @argrule Ref object [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @valrule * [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @fcategory new
******************************************************************************/




/* @func ensGvalleleNew *******************************************************
**
** Default constructor for an Ensembl Genetic Variation Allele.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvaa [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                                       Allele Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Allele::new
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] allele [AjPStr] Allele
** @param [r] frequency [float] Frequency
** @param [r] subsnpid [ajuint] Sub-SNP identifier
**
** @return [EnsPGvallele] Ensembl Genetic Variation Allele
** @@
******************************************************************************/

EnsPGvallele ensGvalleleNew(EnsPGvalleleadaptor gvaa,
                            ajuint identifier,
                            EnsPGvpopulation gvp,
                            AjPStr allele,
                            float frequency,
                            ajuint subsnpid)
{
    EnsPGvallele gva = NULL;

    if(!gvp)
        return NULL;

    if(!allele)
        return NULL;

    AJNEW0(gva);

    gva->Use = 1;

    gva->Identifier = identifier;

    gva->Adaptor = gvaa;

    gva->Gvpopulation = ensGvpopulationNewRef(gvp);

    if(allele)
        gva->Allele = ajStrNewS(allele);

    gva->Frequency = frequency;

    gva->SubSNPIdentifier = subsnpid;

    return gva;
}




/* @func ensGvalleleNewObj ****************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvallele] Ensembl Genetic Variation Allele or NULL
** @@
******************************************************************************/

EnsPGvallele ensGvalleleNewObj(const EnsPGvallele object)
{
    EnsPGvallele gva = NULL;

    if(!object)
        return NULL;

    AJNEW0(gva);

    gva->Use = 1;

    gva->Identifier = object->Identifier;

    gva->Adaptor = object->Adaptor;

    gva->Gvpopulation = ensGvpopulationNewRef(object->Gvpopulation);

    if(object->Allele)
        gva->Allele = ajStrNewRef(object->Allele);

    gva->Frequency = object->Frequency;

    gva->SubSNPIdentifier = object->SubSNPIdentifier;

    if(gva->SubSNPHandle)
        gva->SubSNPHandle = ajStrNewRef(object->SubSNPHandle);

    return gva;
}




/* @func ensGvalleleNewRef ****************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvallele] Ensembl Genetic Variation Allele
** @@
******************************************************************************/

EnsPGvallele ensGvalleleNewRef(EnsPGvallele gva)
{
    if(!gva)
        return NULL;

    gva->Use++;

    return gva;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Allele.
**
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Allele object
**
** @argrule * Pgva [EnsPGvallele*] Genetic Variation Allele object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvalleleDel *******************************************************
**
** Default destructor for an Ensembl Genetic Variation Allele.
**
** @param [d] Pgva [EnsPGvallele*] Ensembl Genetic Variation Allele address
**
** @return [void]
** @@
******************************************************************************/

void ensGvalleleDel(EnsPGvallele *Pgva)
{
    EnsPGvallele pthis = NULL;

    if(!Pgva)
        return;

    if(!*Pgva)
        return;

    if(ajDebugTest("ensGvalleleDel"))
    {
        ajDebug("ensGvalleleDel\n"
                "  *Pgva %p\n",
                *Pgva);

        ensGvalleleTrace(*Pgva, 1);
    }

    pthis = *Pgva;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgva = NULL;

        return;
    }

    ensGvpopulationDel(&pthis->Gvpopulation);

    ajStrDel(&pthis->Allele);

    ajStrDel(&pthis->SubSNPHandle);

    AJFREE(pthis);

    *Pgva = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Allele object.
**
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Allele attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Allele Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGvpopulation Return the Ensembl Genetic Variation Population
** @nam4rule GetAllele Return the allele
** @nam4rule GetFrequency Return the frequency
** @nam4rule GetSubSNPIdentifier Return the sub-SNP identifier
** @nam4rule GetSubSNPHandle Return the sub-SNP handle
**
** @argrule * gva [const EnsPGvallele] Genetic Variation Allele
**
** @valrule Adaptor [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                                        Allele Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Gvpopulation [EnsPGvpopulation] Ensembl Genetic Variation
**                                          Population
** @valrule Allele [AjPStr] Allele
** @valrule Frequency [float] Frequency
** @valrule SubSNPIdentifier [ajuint] Sub-SNP identifier
** @valrule SubSNPHandle [AjPStr] Sub-SNP handle
**
** @fcategory use
******************************************************************************/




/* @func ensGvalleleGetAdaptor ************************************************
**
** Get the Ensembl Genetic Variation Allele Adaptor element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvalleleadaptor] Ensembl Genetic Variation Allele Adaptor
** @@
******************************************************************************/

EnsPGvalleleadaptor ensGvalleleGetAdaptor(const EnsPGvallele gva)
{
    if(!gva)
        return NULL;

    return gva->Adaptor;
}




/* @func ensGvalleleGetIdentifier *********************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvalleleGetIdentifier(const EnsPGvallele gva)
{
    if(!gva)
        return 0;

    return gva->Identifier;
}




/* @func ensGvalleleGetPopulation *********************************************
**
** Get the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvpopulation] Ensembl Genetic Variation Population
** @@
******************************************************************************/

EnsPGvpopulation ensGvalleleGetPopulation(const EnsPGvallele gva)
{
    if(!gva)
        return NULL;

    return gva->Gvpopulation;
}




/* @func ensGvalleleGetAllele *************************************************
**
** Get the allele element of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [AjPStr] Allele
** @@
******************************************************************************/

AjPStr ensGvalleleGetAllele(const EnsPGvallele gva)
{
    if(!gva)
        return NULL;

    return gva->Allele;
}




/* @func ensGvalleleGetFrequency **********************************************
**
** Get the frequency element of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [float] Frequency
** @@
******************************************************************************/

float ensGvalleleGetFrequency(const EnsPGvallele gva)
{
    if(!gva)
        return 0;

    return gva->Frequency;
}




/* @func ensGvalleleGetSubSNPHandle *******************************************
**
** Get the sub-SNP handle element of an Ensembl Genetic Variation Allele.
**
** This is not a simple accessor function, it will fetch the Ensembl Genetic
** Variation Allele Sub-SNP handle from the Ensembl Genetic Variation database
** in case the AJAX String is not defined.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [AjPStr] Sub-SNP handle
** @@
******************************************************************************/

AjPStr ensGvalleleGetSubSNPHandle(EnsPGvallele gva)
{
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr statement = NULL;

    if(!gva)
        return NULL;

    if(gva->SubSNPHandle)
        return gva->SubSNPHandle;

    if(!gva->Adaptor)
        ajDebug("ensGvalleleGetSubSNPHandle cannot fetch a Sub-SNP handle "
                "from the database for an Ensembl Genetic Variation Allele "
                "whithout an Ensembl Genetic Variation Allele Adaptor.\n");

    statement = ajFmtStr(
        "SELECT "
        "subsnp_handle.handle "
        "FROM "
        "subsnp_handle "
        "WHERE "
        "subsnp_handle.subsnp_id = %u",
        gva->SubSNPIdentifier);

    sqls = ensDatabaseadaptorSqlstatementNew(gva->Adaptor, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        gva->SubSNPHandle = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToStr(sqlr, &gva->SubSNPHandle);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(gva->Adaptor, &sqls);

    return gva->SubSNPHandle;
}




/* @func ensGvalleleGetSubSNPIdentifier ***************************************
**
** Get the sub-SNP identifier element of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [ajuint] Sub-SNP identifier
** @@
******************************************************************************/

ajuint ensGvalleleGetSubSNPIdentifier(const EnsPGvallele gva)
{
    if(!gva)
        return 0;

    return gva->SubSNPIdentifier;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Allele object.
**
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Allele
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Allele Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetGvpopulation Set the Ensembl Genetic Variation Population
** @nam4rule SetAllele Set the allele
** @nam4rule SetFrequency Set the frequency
** @nam4rule SetSubSNPIdentifier Set the sub-SNP identifier
** @nam4rule SetSubSNPHandle Set the sub-SNP handle
**
** @argrule * gva [EnsPGvallele] Ensembl Genetic Variation Allele object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvalleleSetAdaptor ************************************************
**
** Set the Ensembl Genetic Variation Allele Adaptor element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] gvaa [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                                       Allele Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetAdaptor(EnsPGvallele gva, EnsPGvalleleadaptor gvaa)
{
    if(!gva)
        return ajFalse;

    gva->Adaptor = gvaa;

    return ajTrue;
}




/* @func ensGvalleleSetIdentifier *********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetIdentifier(EnsPGvallele gva, ajuint identifier)
{
    if(!gva)
        return ajFalse;

    gva->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvalleleSetPopulation *********************************************
**
** Set the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetPopulation(EnsPGvallele gva, EnsPGvpopulation gvp)
{
    if(!gva)
        return ajFalse;

    ensGvpopulationDel(&gva->Gvpopulation);

    gva->Gvpopulation = ensGvpopulationNewRef(gvp);

    return ajTrue;
}




/* @func ensGvalleleSetAllele *************************************************
**
** Set the allele element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [u] allele [AjPStr] Allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetAllele(EnsPGvallele gva, AjPStr allele)
{
    if(!gva)
        return ajFalse;

    ajStrDel(&gva->Allele);

    if(allele)
        gva->Allele = ajStrNewRef(allele);

    return ajTrue;
}




/* @func ensGvalleleSetFrequency **********************************************
**
** Set the frequency element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] frequency [float] Frequency
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetFrequency(EnsPGvallele gva, float frequency)
{
    if(!gva)
        return ajFalse;

    gva->Frequency = frequency;

    return ajTrue;
}




/* @func ensGvalleleSetSubSNPHandle *******************************************
**
** Set the sub-SNP handle element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [u] subsnphandle [AjPStr] Sub-SNP handle
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetSubSNPHandle(EnsPGvallele gva, AjPStr subsnphandle)
{
    if(!gva)
        return ajFalse;

    ajStrDel(&gva->SubSNPHandle);

    if(subsnphandle)
        gva->SubSNPHandle = ajStrNewRef(subsnphandle);

    return ajTrue;
}




/* @func ensGvalleleSetSubSNPIdentifier ***************************************
**
** Set the sub-SNP identifier element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] subsnpid [ajuint] Sub-SNP identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetSubSNPIdentifier(EnsPGvallele gva, ajuint subsnpid)
{
    if(!gva)
        return ajFalse;

    gva->SubSNPIdentifier = subsnpid;

    return ajTrue;
}




/* @func ensGvalleleGetMemsize ************************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvalleleGetMemsize(const EnsPGvallele gva)
{
    ajulong size = 0;

    if(!gva)
        return 0;

    size += sizeof (EnsOGvallele);

    size += ensGvpopulationGetMemsize(gva->Gvpopulation);

    if(gva->Allele)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gva->Allele);
    }

    if(gva->SubSNPHandle)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gva->SubSNPHandle);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Allele object.
**
** @fdata [EnsPGvallele]
** @nam3rule Trace Report Ensembl Genetic Variation Allele elements to
**                 debug file
**
** @argrule Trace gva [const EnsPGvallele] Ensembl Genetic Variation Allele
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvalleleTrace *****************************************************
**
** Trace an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleTrace(const EnsPGvallele gva, ajuint level)
{
    AjPStr indent = NULL;

    if(!gva)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvalleleTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Population %p\n"
            "%S  Allele '%S'\n"
            "%S  Frequency %f\n"
            "%S  SubSNPIdentifier %u\n"
            "%S  SubSNPHandle '%S'\n",
            indent, gva,
            indent, gva->Use,
            indent, gva->Identifier,
            indent, gva->Adaptor,
            indent, gva->Gvpopulation,
            indent, gva->Allele,
            indent, gva->Frequency,
            indent, gva->SubSNPIdentifier,
            indent, gva->SubSNPHandle);

    ensGvpopulationTrace(gva->Gvpopulation, level + 1);

    ajStrDel(&indent);

    return ajTrue;
}




/* @datasection [EnsPGvgenotype] Genetic Variation Genotype *******************
**
** Functions for manipulating Ensembl Genetic Variation Genotype objects
**
** @cc Bio::EnsEMBL::Variation::Genotype CVS Revision: 1.3
**
** @nam2rule Gvgenotype
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Genotype by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Genotype. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @argrule Ref object [EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @valrule * [EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @fcategory new
******************************************************************************/




/* @func ensGvgenotypeNew *****************************************************
**
** Default constructor for an Ensembl Genetic Variation Genotype.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvga [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                         Genotype Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Genotype::new
** @param [u] allele1 [AjPStr] Allele 1
** @param [u] allele2 [AjPStr] Allele 2
**
** @return [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @@
******************************************************************************/

EnsPGvgenotype ensGvgenotypeNew(EnsPGvgenotypeadaptor gvga,
                                ajuint identifier,
                                AjPStr allele1,
                                AjPStr allele2)
{
    EnsPGvgenotype gvg = NULL;

    AJNEW0(gvg);

    gvg->Use = 1;

    gvg->Identifier = identifier;

    gvg->Adaptor = gvga;

    if(allele1)
        gvg->Allele1 = ajStrNewRef(allele1);

    if(allele2)
        gvg->Allele2 = ajStrNewRef(allele2);

    return gvg;
}




/* @func ensGvgenotypeNewObj **************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGvgenotype] Ensembl Genetic Variation Genotype or NULL
** @@
******************************************************************************/

EnsPGvgenotype ensGvgenotypeNewObj(const EnsPGvgenotype object)
{
    EnsPGvgenotype gvg = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvg);

    gvg->Use = 1;

    gvg->Identifier = object->Identifier;

    gvg->Adaptor = object->Adaptor;

    if(object->Allele1)
        gvg->Allele1 = ajStrNewRef(object->Allele1);

    if(object->Allele2)
        gvg->Allele2 = ajStrNewRef(object->Allele2);

    return gvg;
}




/* @func ensGvgenotypeNewRef **************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @@
******************************************************************************/

EnsPGvgenotype ensGvgenotypeNewRef(EnsPGvgenotype gvg)
{
    if(!gvg)
        return NULL;

    gvg->Use++;

    return gvg;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Genotype.
**
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Genotype object
**
** @argrule * Pgvg [EnsPGvgenotype*] Genetic Variation Genotype
**                                   object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvgenotypeDel *****************************************************
**
** Default destructor for an Ensembl Genetic Variation Genotype.
**
** @param [d] Pgvg [EnsPGvgenotype*] Ensembl Genetic Variation Genotype
**                                   address
**
** @return [void]
** @@
******************************************************************************/

void ensGvgenotypeDel(EnsPGvgenotype *Pgvg)
{
    EnsPGvgenotype pthis = NULL;

    if(!Pgvg)
        return;

    if(!*Pgvg)
        return;

    if(ajDebugTest("ensGvgenotypeDel"))
    {
        ajDebug("ensGvgenotypeDel\n"
                "  *Pgvg %p\n",
                *Pgvg);

        ensGvgenotypeTrace(*Pgvg, 1);
    }

    pthis = *Pgvg;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvg = NULL;

        return;
    }

    ajStrDel(&pthis->Allele1);
    ajStrDel(&pthis->Allele2);

    AJFREE(pthis);

    *Pgvg = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Genotype object.
**
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Genotype attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Genotype Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetAllele1 Return the allele 1
** @nam4rule GetAllele2 Return the allele 2
**
** @argrule * gvg [const EnsPGvgenotype] Genetic Variation Genotype
**
** @valrule Adaptor [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                          Genotype Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Allele1 [AjPStr] Allele 1
** @valrule Allele2 [AjPStr] Allele 2
**
** @fcategory use
******************************************************************************/




/* @func ensGvgenotypeGetAdaptor **********************************************
**
** Get the Ensembl Genetic Variation Genotype Adaptor element of an
** Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGvgenotypeadaptor] Ensembl Genetic Variation Genotype Adaptor
** @@
******************************************************************************/

EnsPGvgenotypeadaptor ensGvgenotypeGetAdaptor(const EnsPGvgenotype gvg)
{
    if(!gvg)
        return NULL;

    return gvg->Adaptor;
}




/* @func ensGvgenotypeGetIdentifier *******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvgenotypeGetIdentifier(const EnsPGvgenotype gvg)
{
    if(!gvg)
        return 0;

    return gvg->Identifier;
}




/* @func ensGvgenotypeGetAllele1 **********************************************
**
** Get the allele 1 element of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [AjPStr] Allele 1
** @@
******************************************************************************/

AjPStr ensGvgenotypeGetAllele1(const EnsPGvgenotype gvg)
{
    if(!gvg)
        return NULL;

    return gvg->Allele1;
}




/* @func ensGvgenotypeGetAllele2 **********************************************
**
** Get the allele 2 element of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [AjPStr] Allele 2
** @@
******************************************************************************/

AjPStr ensGvgenotypeGetAllele2(const EnsPGvgenotype gvg)
{
    if(!gvg)
        return NULL;

    return gvg->Allele2;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Genotype object.
**
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Genotype
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Genotype Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetAllele1 Set the allele 1
** @nam4rule SetAllele2 Set the allele 2
**
** @argrule * gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvgenotypeSetAdaptor **********************************************
**
** Set the Ensembl Genetic Variation Genotype Adaptor element of an
** Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [u] gvga [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                         Genotype Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetAdaptor(EnsPGvgenotype gvg, EnsPGvgenotypeadaptor gvga)
{
    if(!gvg)
        return ajFalse;

    gvg->Adaptor = gvga;

    return ajTrue;
}




/* @func ensGvgenotypeSetIdentifier *******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetIdentifier(EnsPGvgenotype gvg, ajuint identifier)
{
    if(!gvg)
        return ajFalse;

    gvg->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvgenotypeSetAllele1 **********************************************
**
** Set the allele 1 element of an Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [u] allele1 [AjPStr] Allele 1
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetAllele1(EnsPGvgenotype gvg, AjPStr allele1)
{
    if(!gvg)
        return ajFalse;

    ajStrDel(&gvg->Allele1);

    if(allele1)
        gvg->Allele1 = ajStrNewRef(allele1);

    return ajTrue;
}




/* @func ensGvgenotypeSetAllele2 **********************************************
**
** Set the allele 2 element of an Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [u] allele2 [AjPStr] Allele 2
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetAllele2(EnsPGvgenotype gvg, AjPStr allele2)
{
    if(!gvg)
        return ajFalse;

    ajStrDel(&gvg->Allele2);

    if(allele2)
        gvg->Allele2 = ajStrNewRef(allele2);

    return ajTrue;
}




/* @func ensGvgenotypeGetMemsize **********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvgenotypeGetMemsize(const EnsPGvgenotype gvg)
{
    ajulong size = 0;

    if(!gvg)
        return 0;

    size += sizeof (EnsOGvgenotype);

    if(gvg->Allele1)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvg->Allele1);
    }

    if(gvg->Allele2)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvg->Allele2);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Genotype object.
**
** @fdata [EnsPGvgenotype]
** @nam3rule Trace Report Ensembl Genetic Variation Genotype elements to
**                 debug file
**
** @argrule Trace gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvgenotypeTrace ***************************************************
**
** Trace an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeTrace(const EnsPGvgenotype gvg, ajuint level)
{
    AjPStr indent = NULL;

    if(!gvg)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvgenotypeTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Allele1 '%S'\n"
            "%S  Allele2 '%S'\n",
            indent, gvg,
            indent, gvg->Use,
            indent, gvg->Identifier,
            indent, gvg->Adaptor,
            indent, gvg->Allele1,
            indent, gvg->Allele2);

    ajStrDel(&indent);

    return ajTrue;
}




/* @datasection [EnsPGvsource] Genetic Variation Source ***********************
**
** Functions for manipulating Ensembl Genetic Variation Source objects
**
** NOTE: The Ensembl Genetic Variation Source object has no counterpart in the
** Ensembl Variation Perl API. It has been split out of the
** Bio::EnsEMBL::Variation::Variation object, because a small number of entries
** in the 'variation_source' table is linked from a large number of entries in
** other tables. By caching all Ensembl Genetic Variation Source objects in an
** Ensembl Genetic Variation Source Adaptor, all other Ensembl Genetic
** Variation objects can reference the same data objects.
**
** @cc Bio::EnsEMBL::Variation::Variation CVS Revision: 1.37
**
** @nam2rule Gvsource
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Source by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Source. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvsource]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvsource] Ensembl Genetic Variation Source
** @argrule Ref object [EnsPGvsource] Ensembl Genetic Variation Source
**
** @valrule * [EnsPGvsource] Ensembl Genetic Variation Source
**
** @fcategory new
******************************************************************************/




/* @func ensGvsourceNew *******************************************************
**
** Default constructor for an Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::*::new
** @param [u] name [AjPStr] Name
** @param [u] version [AjPStr] Version
** @param [u] description [AjPStr] Description
** @param [u] url [AjPStr] Uniform Resource Locator
** @param [r] somatic [AjBool] Somatic or germline variation
**
** @return [EnsPGvsource] Ensembl Genetic Variation Source
** @@
******************************************************************************/

EnsPGvsource ensGvsourceNew(EnsPGvsourceadaptor gvsa,
                            ajuint identifier,
                            AjPStr name,
                            AjPStr version,
                            AjPStr description,
                            AjPStr url,
                            AjBool somatic)
{
    EnsPGvsource gvs = NULL;

    AJNEW0(gvs);

    gvs->Use = 1;

    gvs->Identifier = identifier;

    gvs->Adaptor = gvsa;

    if(name)
        gvs->Name = ajStrNewRef(name);

    if(version)
        gvs->Version = ajStrNewRef(version);

    if(description)
        gvs->Description = ajStrNewRef(description);

    if(url)
        gvs->URL = ajStrNewRef(url);

    gvs->Somatic = somatic;

    return gvs;
}




/* @func ensGvsourceNewObj ****************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [EnsPGvsource] Ensembl Genetic Variation Source or NULL
** @@
******************************************************************************/

EnsPGvsource ensGvsourceNewObj(const EnsPGvsource object)
{
    EnsPGvsource gvs = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvs);

    gvs->Use = 1;

    gvs->Identifier = object->Identifier;

    gvs->Adaptor = object->Adaptor;

    if(object->Name)
        gvs->Name = ajStrNewRef(object->Name);

    if(object->Version)
        gvs->Version = ajStrNewRef(object->Version);

    if(object->Description)
        gvs->Description = ajStrNewRef(object->Description);

    if(object->URL)
        gvs->URL = ajStrNewRef(object->URL);

    gvs->Somatic = object->Somatic;

    return gvs;
}




/* @func ensGvsourceNewRef ****************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [EnsPGvsource] Ensembl Genetic Variation Source
** @@
******************************************************************************/

EnsPGvsource ensGvsourceNewRef(EnsPGvsource gvs)
{
    if(!gvs)
        return NULL;

    gvs->Use++;

    return gvs;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Source.
**
** @fdata [EnsPGvsource]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Source object
**
** @argrule * Pgvs [EnsPGvsource*] Genetic Variation Source object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvsourceDel *******************************************************
**
** Default destructor for an Ensembl Genetic Variation Source.
**
** @param [d] Pgvs [EnsPGvsource*] Ensembl Genetic Variation Source address
**
** @return [void]
** @@
******************************************************************************/

void ensGvsourceDel(EnsPGvsource *Pgvs)
{
    EnsPGvsource pthis = NULL;

    if(!Pgvs)
        return;

    if(!*Pgvs)
        return;

    if(ajDebugTest("ensGvsourceDel"))
    {
        ajDebug("ensGvsourceDel\n"
                "  *Pgvs %p\n",
                *Pgvs);

        ensGvsourceTrace(*Pgvs, 1);
    }

    pthis = *Pgvs;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvs = NULL;

        return;
    }

    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Version);
    ajStrDel(&pthis->Description);
    ajStrDel(&pthis->URL);

    AJFREE(pthis);

    *Pgvs = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Source object.
**
** @fdata [EnsPGvsource]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Source attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Source Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetVersion Return the version
** @nam4rule GetDescription Return the description
** @nam4rule GetURL Return the Uniform Resource Locator
** @nam4rule GetSomatic Return the somatic or germline flag
**
** @argrule * gvs [const EnsPGvsource] Genetic Variation Source
**
** @valrule Adaptor [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                        Source Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Version [AjPStr] Version
** @valrule Description [AjPStr] Description
** @valrule URL [AjPStr] Uniform Resource Locator
** @valrule Somatic [AjBool] Somatic or germline flag
**
** @fcategory use
******************************************************************************/




/* @func ensGvsourceGetAdaptor ************************************************
**
** Get the Ensembl Genetic Variation Source Adaptor element of an
** Ensembl Genetic Variation Source.
**
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [EnsPGvsourceadaptor] Ensembl Genetic Variation Source Adaptor
** @@
******************************************************************************/

EnsPGvsourceadaptor ensGvsourceGetAdaptor(const EnsPGvsource gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Adaptor;
}




/* @func ensGvsourceGetIdentifier *********************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Source.
**
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvsourceGetIdentifier(const EnsPGvsource gvs)
{
    if(!gvs)
        return 0;

    return gvs->Identifier;
}





/* @func ensGvsourceGetName ***************************************************
**
** Get the name element of an Ensembl Genetic Variation Source.
**
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvsourceGetName(const EnsPGvsource gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Name;
}





/* @func ensGvsourceGetVersion ************************************************
**
** Get the version element of an Ensembl Genetic Variation Source.
**
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjPStr] Version
** @@
******************************************************************************/

AjPStr ensGvsourceGetVersion(const EnsPGvsource gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Version;
}





/* @func ensGvsourceGetDescription ********************************************
**
** Get the description element of an Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Variation::Variation::source_description
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjPStr] Description
** @@
******************************************************************************/

AjPStr ensGvsourceGetDescription(const EnsPGvsource gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Description;
}





/* @func ensGvsourceGetURL ****************************************************
**
** Get the Uniform Resource Locator element of an
** Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Variation::Variation::source_url
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjPStr] Uniform Resource Locator
** @@
******************************************************************************/

AjPStr ensGvsourceGetURL(const EnsPGvsource gvs)
{
    if(!gvs)
        return NULL;

    return gvs->URL;
}





/* @func ensGvsourceGetSomatic ************************************************
**
** Get the somatic or germline flag element of an
** Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Variation::Variation::is_somatic
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjBool] Somatic or germline flag
** @@
******************************************************************************/

AjBool ensGvsourceGetSomatic(const EnsPGvsource gvs)
{
    if(!gvs)
        return ajFalse;

    return gvs->Somatic;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Source object.
**
** @fdata [EnsPGvsource]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Source
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Source Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetVersion Set the version
** @nam4rule SetDescription Set the description
** @nam4rule SetURL Set the Uniform Resource Locator
** @nam4rule SetSomatic Set the somatic or germline flag
**
** @argrule * gvs [EnsPGvsource] Ensembl Genetic Variation Source object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvsourceSetAdaptor ************************************************
**
** Set the Ensembl Genetic Variation Source Adaptor element of an
** Ensembl Genetic Variation Source.
**
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceSetAdaptor(EnsPGvsource gvs, EnsPGvsourceadaptor gvsa)
{
    if(!gvs)
        return ajFalse;

    gvs->Adaptor = gvsa;

    return ajTrue;
}




/* @func ensGvsourceSetIdentifier *********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Source.
**
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceSetIdentifier(EnsPGvsource gvs, ajuint identifier)
{
    if(!gvs)
        return ajFalse;

    gvs->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvsourceSetName ***************************************************
**
** Set the name element of an Ensembl Genetic Variation Source.
**
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceSetName(EnsPGvsource gvs, AjPStr name)
{
    if(!gvs)
        return ajFalse;

    ajStrDel(&gvs->Name);

    if(name)
        gvs->Name = ajStrNewRef(name);

    return ajTrue;
}




/* @func ensGvsourceSetVersion ************************************************
**
** Set the version element of an Ensembl Genetic Variation Source.
**
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] version [AjPStr] Version
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceSetVersion(EnsPGvsource gvs, AjPStr version)
{
    if(!gvs)
        return ajFalse;

    ajStrDel(&gvs->Version);

    if(version)
        gvs->Version = ajStrNewRef(version);

    return ajTrue;
}




/* @func ensGvsourceSetDescription ********************************************
**
** Set the description element of an Ensembl Genetic Variation Source.
**
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] description [AjPStr] Description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceSetDescription(EnsPGvsource gvs, AjPStr description)
{
    if(!gvs)
        return ajFalse;

    ajStrDel(&gvs->Description);

    if(description)
        gvs->Description = ajStrNewRef(description);

    return ajTrue;
}




/* @func ensGvsourceSetURL ****************************************************
**
** Set the Uniform Resopurce Locator element of an
** Ensembl Genetic Variation Source.
**
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] url [AjPStr] Uniform Resopurce Locator
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceSetURL(EnsPGvsource gvs, AjPStr url)
{
    if(!gvs)
        return ajFalse;

    ajStrDel(&gvs->URL);

    if(url)
        gvs->URL = ajStrNewRef(url);

    return ajTrue;
}




/* @func ensGvsourceSetSomatic ************************************************
**
** Set the somatic or germline flag element of an
** Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Variation::Variation::is_somatic
** @param [u] gvs [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] somatic [AjBool] Somatic or germline flag
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceSetSomatic(EnsPGvsource gvs, AjBool somatic)
{
    if(!gvs)
        return ajFalse;

    gvs->Somatic = somatic;

    return ajTrue;
}




/* @func ensGvsourceGetMemsize ************************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Source.
**
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvsourceGetMemsize(const EnsPGvsource gvs)
{
    ajulong size = 0;

    if(!gvs)
        return 0;

    size += sizeof (EnsOGvsource);

    if(gvs->Name)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvs->Name);
    }

    if(gvs->Version)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvs->Version);
    }

    if(gvs->Description)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvs->Description);
    }

    if(gvs->URL)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvs->URL);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Source object.
**
** @fdata [EnsPGvsource]
** @nam3rule Trace Report Ensembl Genetic Variation Source elements to
**                 debug file
**
** @argrule Trace gvs [const EnsPGvsource] Ensembl Genetic Variation Source
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvsourceTrace *****************************************************
**
** Trace an Ensembl Genetic Variation Source.
**
** @param [r] gvs [const EnsPGvsource] Ensembl Genetic Variation Source
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceTrace(const EnsPGvsource gvs, ajuint level)
{
    AjPStr indent = NULL;

    if(!gvs)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvsourceTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Name '%S'\n"
            "%S  Version '%S'\n"
            "%S  Description '%S'\n"
            "%S  URL '%S'\n"
            "%S  Somatic '%B'\n",
            indent, gvs,
            indent, gvs->Use,
            indent, gvs->Identifier,
            indent, gvs->Adaptor,
            indent, gvs->Name,
            indent, gvs->Version,
            indent, gvs->Description,
            indent, gvs->URL,
            indent, gvs->Somatic);

    ajStrDel(&indent);

    return ajTrue;
}




/* @section comparison ********************************************************
**
** Functions for comparing Ensembl Coordinate Systems.
**
** @fdata [EnsPCoordsystem]
**
** @nam3rule  Match Compare two Coordinate Systems
**
** @argrule * cs1 [const EnsPCoordsystem] Coordinate System
** @argrule * cs2 [const EnsPCoordsystem] Coordinate System
**
** @valrule * [AjBool] True on success
**
** @fcategory use
******************************************************************************/




/* @func ensGvsourceMatch *****************************************************
**
** Test for matching two Ensembl Genetic Variation Sources.
**
** @param [r] gvs1 [const EnsPGvsource] First Ensembl Genetic Variation Source
** @param [r] gvs2 [const EnsPGvsource] Second Ensembl Genetic Variation Source
**
** @return [AjBool] ajTrue if the Ensembl Genetic Variation Source are equal
** @@
** The comparison is based on an initial pointer equality test and if that
** fails, a case-insensitive string comparison of the name and version elements
** is performed.
******************************************************************************/

AjBool ensGvsourceMatch(const EnsPGvsource gvs1, const EnsPGvsource gvs2)
{
    if(ajDebugTest("ensGvsourceMatch"))
    {
        ajDebug("ensGvsourceMatch\n"
                "  gvs1 %p\n"
                "  gvs2 %p\n",
                gvs1,
                gvs2);

        ensGvsourceTrace(gvs1, 1);
        ensGvsourceTrace(gvs2, 1);
    }

    if(!gvs1)
        return ajFalse;

    if(!gvs2)
        return ajFalse;

    if(gvs1 == gvs2)
        return ajTrue;

    if(gvs1->Identifier != gvs2->Identifier)
        return ajFalse;

    if(gvs1->Adaptor != gvs2->Adaptor)
        return ajFalse;

    if(!ajStrMatchCaseS(gvs1->Name, gvs2->Name))
        return ajFalse;

    if(!ajStrMatchCaseS(gvs1->Version, gvs2->Version))
        return ajFalse;

    if(!ajStrMatchCaseS(gvs1->Description, gvs2->Description))
        return ajFalse;

    if(!ajStrMatchCaseS(gvs1->URL, gvs2->URL))
        return ajFalse;

    if(gvs1->Somatic != gvs2->Somatic)
        return ajFalse;

    return ajTrue;
}




/* @datasection [EnsPGvsourceadaptor] Ensembl Genetic Variation Source Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Source Adaptor objects
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor CVS Revision:
**
** @nam2rule Gvsourceadaptor
**
******************************************************************************/

static const char *gvsourceadaptorTables[] =
{
    "source",
    NULL
};

static const char *gvsourceadaptorColumns[] =
{
    "source.source_id",
    "source.name",
    "source.version",
    "source.description",
    "source.url"
    "source.somatic",
    NULL
};

static EnsOBaseadaptorLeftJoin gvsourceadaptorLeftJoin[] =
{
    {NULL, NULL}
};

static const char *gvsourceadaptorDefaultCondition = NULL;

static const char *gvsourceadaptorFinalCondition = NULL;




/* @funcstatic gvsourceadaptorFetchAllBySQL ***********************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Genetic Variation Source objects.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::_objFromHashref
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] gvss [AjPList] AJAX List of Ensembl Genetic Variation Sources
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvsourceadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                           const AjPStr statement,
                                           EnsPAssemblymapper am,
                                           EnsPSlice slice,
                                           AjPList gvss)
{
    ajuint identifier = 0;

    AjBool somatic = AJFALSE;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr name        = NULL;
    AjPStr version     = NULL;
    AjPStr description = NULL;
    AjPStr url         = NULL;

    EnsPGvsource gvs         = NULL;
    EnsPGvsourceadaptor gvsa = NULL;

    if(ajDebugTest("gvsourceadaptorFetchAllBySQL"))
        ajDebug("gvsourceadaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  gvss %p\n",
                dba,
                statement,
                am,
                slice,
                gvss);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!gvss)
        return ajFalse;

    gvsa = ensRegistryGetGvsourceadaptor(dba);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier  = 0;
        name        = ajStrNew();
        version     = ajStrNew();
        description = ajStrNew();
        url         = ajStrNew();
        somatic     = ajFalse;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToStr(sqlr, &name);
        ajSqlcolumnToStr(sqlr, &version);
        ajSqlcolumnToStr(sqlr, &description);
        ajSqlcolumnToStr(sqlr, &url);
        ajSqlcolumnToBool(sqlr, &somatic);

        gvs = ensGvsourceNew(gvsa,
                             identifier,
                             name,
                             version,
                             description,
                             url,
                             somatic);

        ajListPushAppend(gvss, (void *) gvs);

        ajStrDel(&name);
        ajStrDel(&version);
        ajStrDel(&description);
        ajStrDel(&url);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    return ajTrue;
}




/* @funcstatic gvsourceadaptorCacheInsert *************************************
**
** Insert an Ensembl Genetic Variation Source into the Ensembl Genetic
** Variation Source Adaptor-internal cache. If a Source with the same name
** element is already present in the adaptor cache, the Source is deleted and
** a pointer to the cached Source is returned.
**
** @param [u] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
** @param [u] Pgvs [EnsPGvsource*] Ensembl Genetic Variation Source address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvsourceadaptorCacheInsert(EnsPGvsourceadaptor gvsa,
                                         EnsPGvsource *Pgvs)
{
    ajuint *Pidentifier = NULL;

    EnsPGvsource gvs1 = NULL;
    EnsPGvsource gvs2 = NULL;

    if(!gvsa)
        return ajFalse;

    if(!gvsa->CacheByIdentifier)
        return ajFalse;

    if(!gvsa->CacheByName)
        return ajFalse;

    if(!Pgvs)
        return ajFalse;

    if(!*Pgvs)
        return ajFalse;

    /* Search the identifier cache. */

    gvs1 = (EnsPGvsource) ajTableFetch(
        gvsa->CacheByIdentifier,
        (const void *) &((*Pgvs)->Identifier));

    /* Search the name cache. */

    gvs2 = (EnsPGvsource) ajTableFetch(
        gvsa->CacheByName,
        (const void *) (*Pgvs)->Name);

    if((!gvs1) && (!gvs2))
    {
        /* Insert into the identifier cache. */

        AJNEW0(Pidentifier);

        *Pidentifier = (*Pgvs)->Identifier;

        ajTablePut(gvsa->CacheByIdentifier,
                   (void *) Pidentifier,
                   (void *) ensGvsourceNewRef(*Pgvs));

        /* Insert into the name cache. */

        ajTablePut(gvsa->CacheByName,
                   (void *) ajStrNewS((*Pgvs)->Name),
                   (void *) ensGvsourceNewRef(*Pgvs));
    }

    if(gvs1 && gvs2 && (gvs1 == gvs2))
    {
        ajDebug("gvsourceadaptorCacheInsert replaced Ensembl Genetic "
                "Variation Source %p with one already cached %p.\n",
                *Pgvs, gvs1);

        ensGvsourceDel(Pgvs);

        ensGvsourceNewRef(gvs1);

        Pgvs = &gvs1;
    }

    if(gvs1 && gvs2 && (gvs1 != gvs2))
        ajDebug("gvsourceadaptorCacheInsert detected an Ensembl Genetic "
                "Variation Source in the identifier and name cache with "
                "identical names ('%S' and '%S') but different addresses "
                "(%p and %p).\n",
                gvs1->Name, gvs2->Name, gvs1, gvs2);

    if(gvs1 && (!gvs2))
        ajDebug("gvsourceadaptorCacheInsert detected an Ensembl Analysis "
                "in the identifier, but not in the name cache.\n");

    if((!gvs1) && gvs2)
        ajDebug("gvsourceadaptorCacheInsert detected an Ensembl Analysis "
                "in the name, but not in the identifier cache.\n");

    return ajTrue;
}




/* @funcstatic gvsourceadaptorCacheRemove *************************************
**
** Remove an Ensembl Genetic Variation Source from the Ensembl Genetic
** Variation Source Adaptor-internal cache.
**
** @param [u] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
** @param [u] gvs [EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

#if AJFALSE

static AjBool gvsourceadaptorCacheRemove(EnsPGvsourceadaptor gvsa,
                                         EnsPGvsource gvs)
{
    ajuint *Pidentifier = NULL;

    AjPStr key = NULL;

    EnsPGvsource gvs1 = NULL;
    EnsPGvsource gvs2 = NULL;

    if(!gvsa)
        return ajFalse;

    if(!gvs)
        return ajFalse;

    /* Remove the table nodes. */

    gvs1 = (EnsPGvsource)
        ajTableRemoveKey(gvsa->CacheByIdentifier,
                         (const void *) &gvs->Identifier,
                         (void **) &Pidentifier);

    gvs2 = (EnsPGvsource)
        ajTableRemoveKey(gvsa->CacheByName,
                         (const void *) gvs->Name,
                         (void **) &key);

    if(gvs1 && (!gvs2))
        ajWarn("gvsourceadaptorCacheRemove could remove Ensembl Genetic "
               "Variation Source with identifier %u and name '%S' only from "
               "the identifier cache.\n",
               gvs->Identifier,
               gvs->Name);

    if((!gvs1) && gvs2)
        ajWarn("gvsourceadaptorCacheRemove could remove Ensembl Genetic "
               "Variation Source with identifier %u and name '%S' only from "
               "the name cache.\n",
               gvs->Identifier,
               gvs->Name);

    /* Delete the keys. */

    AJFREE(Pidentifier);

    ajStrDel(&key);

    /* Delete (or at least de-reference) the Ensembl Analyses. */

    ensGvsourceDel(&gvs1);
    ensGvsourceDel(&gvs2);

    return ajTrue;
}

#endif




/* @funcstatic gvsourceadaptorCacheInit ***************************************
**
** Initialise the internal Ensembl Genetic Variation Source cache of an
** Ensembl Genetic Variation Source Adaptor.
**
** @param [u] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvsourceadaptorCacheInit(EnsPGvsourceadaptor gvsa)
{
    AjPList gvss = NULL;
    AjPList mis  = NULL;

    AjPStr key = NULL;

    EnsPDatabaseadaptor dba = NULL;

    EnsPGvsource gvs = NULL;

    EnsPMetainformation        mi  = NULL;
    EnsPMetainformationadaptor mia = NULL;

    if(ajDebugTest("gvsourceadaptorCacheInit"))
        ajDebug("gvsourceadaptorCacheInit\n"
                "  gvsa %p\n",
                gvsa);

    if(!gvsa)
        return ajFalse;

    if(gvsa->CacheByIdentifier)
        return ajFalse;
    else
        gvsa->CacheByIdentifier =
            ajTableNewFunctionLen(0, ensTableCmpUint, ensTableHashUint);

    if(gvsa->CacheByName)
        return ajFalse;
    else
        gvsa->CacheByName = ajTablestrNewCaseLen(0);

    /* Get the default Ensembl Genetic Variation Source. */

    dba = ensBaseadaptorGetDatabaseadaptor(gvsa->Adaptor);

    mia = ensRegistryGetMetainformationadaptor(dba);

    key = ajStrNewC("source.default_source");

    mis = ajListNew();

    ensMetainformationadaptorFetchAllByKey(mia, key, mis);

    ajStrDel(&key);

    ajListPeekFirst(mis, (void **) &mi);

    /* Fetch all Ensembl Genetic Variation Sources. */

    gvss = ajListNew();

    ensBaseadaptorGenericFetch(gvsa->Adaptor,
                               (const AjPStr) NULL,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               gvss);

    while(ajListPop(gvss, (void **) &gvs))
    {
        if(ajStrMatchS(gvs->Name, ensMetainformationGetValue(mi)))
            gvsa->DefaultGvsource = (void *) ensGvsourceNewRef(gvs);

        gvsourceadaptorCacheInsert(gvsa, &gvs);

        /*
        ** Both caches hold internal references to the
        ** Ensembl Genetic Variation Source objects.
        */

        ensGvsourceDel(&gvs);
    }

    ajListFree(&gvss);

    while(ajListPop(mis, (void **) &mi))
        ensMetainformationDel(&mi);

    ajListFree(&mis);

    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Source Adaptor by
** pointer. It is the responsibility of the user to first destroy any previous
** Ensembl Genetic Variation Source Adaptor. The target pointer does not need
** to be initialised to NULL, but it is good programming practice to do so
** anyway.
**
** @fdata [EnsPGvsourceadaptor]
** @fnote None
**
** @nam3rule New Constructor
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule * [EnsPGvsourceadaptor] Ensembl Genetic Variation Source Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensGvsourceadaptorNew ************************************************
**
** Default constructor for an Ensembl Genetic Variation Source Adaptor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetGvsourceadaptor
**
** @cc Bio::EnsEMBL::Variation::DBSQL::*::new
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvsourceadaptor] Ensembl Genetic Variation Source Adaptor
**                               or NULL
** @@
******************************************************************************/

EnsPGvsourceadaptor ensGvsourceadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPGvsourceadaptor gvsa = NULL;

    if(!dba)
        return NULL;

    if(ajDebugTest("ensGvsourceadaptorNew"))
        ajDebug("ensGvsourceadaptorNew\n"
                "  dba %p\n",
                dba);

    AJNEW0(gvsa);

    gvsa->Adaptor = ensBaseadaptorNew(
        dba,
        gvsourceadaptorTables,
        gvsourceadaptorColumns,
        gvsourceadaptorLeftJoin,
        gvsourceadaptorDefaultCondition,
        gvsourceadaptorFinalCondition,
        gvsourceadaptorFetchAllBySQL);

    /*
    ** NOTE: The cache cannot be initialised here because the
    ** gvsourceadaptorCacheInit function calls ensBaseadaptorGenericFetch,
    ** which calls gvsourceadaptorFetchAllBySQL, which calls
    ** ensRegistryGetGvsourceadaptor. At that point, however, the
    ** Ensembl Genetic Variation Source Adaptor has not been stored in the
    ** Ensembl Registry. Therefore, each ensGvsourceadaptorFetch function
    ** has to test the presence of the adaptor-internal cache and eventually
    ** initialise before accessing it.
    **
    **  gvsourceadaptorCacheInit(aa);
    */

    return gvsa;
}




/* @funcstatic gvsourceadaptorCacheClearIdentifier ****************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Source Adaptor-internal Ensembl Genetic Variation Source cache.
** This function deletes the unsigned integer identifier key and the
** Ensembl Genetic Variation value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Genetic Variation Source
**                           value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvsourceadaptorCacheClearIdentifier(void **key,
                                                void **value,
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

    ensGvsourceDel((EnsPGvsource *) value);

    return;
}




/* @funcstatic gvsourceadaptorCacheClearName **********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Source Adaptor-internal Ensembl Genetic Variation Source cache.
** This function deletes the AJAX String key data and the
** Ensembl Genetic Variation Source value data.
**
** @param [u] key [void**] AJAX String key data address
** @param [u] value [void**] Ensembl Genetic Variation Source
**                           value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvsourceadaptorCacheClearName(void **key,
                                          void **value,
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

    ajStrDel((AjPStr *) key);

    ensGvsourceDel((EnsPGvsource *) value);

    return;
}




/* @funcstatic gvsourceadaptorCacheExit ***************************************
**
** Clears the internal Ensembl Genetic Variation Source cache of an
** Ensembl Genetic Variation Source Adaptor.
**
** @param [u] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvsourceadaptorCacheExit(EnsPGvsourceadaptor gvsa)
{
    if(!gvsa)
        return ajFalse;

    /* Clear and delete the identifier cache. */

    ajTableMapDel(gvsa->CacheByIdentifier,
                  gvsourceadaptorCacheClearIdentifier,
                  NULL);

    ajTableFree(&gvsa->CacheByIdentifier);

    /* Clear and delete the name cache. */

    ajTableMapDel(gvsa->CacheByName,
                  gvsourceadaptorCacheClearName,
                  NULL);

    ajTableFree(&gvsa->CacheByName);

    ensGvsourceDel((EnsPGvsource *) &gvsa->DefaultGvsource);

    return ajTrue;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Source Adaptor.
**
** @fdata [EnsPGvsourceadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Genetic Variation Source Adaptor
**               object.
**
** @argrule * Pgvsa [EnsPGvsourceadaptor*] Ensembl Genetic Variation
**                                         Source Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvsourceadaptorDel *********************************************
**
** Default destructor for an Ensembl Genetic Variation Source Adaptor.
** This function also clears the internal caches.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Pgvsa [EnsPGvsourceadaptor*] Ensembl Genetic Variation Source
**                                         Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvsourceadaptorDel(EnsPGvsourceadaptor* Pgvsa)
{
    EnsPGvsourceadaptor pthis = NULL;

    if(!Pgvsa)
        return;

    if(!*Pgvsa)
        return;

    pthis = *Pgvsa;

    gvsourceadaptorCacheExit(pthis);

    ensBaseadaptorDel(&pthis->Adaptor);

    AJFREE(pthis);

    *Pgvsa = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Source Adaptor object.
**
** @fdata [EnsPGvsourceadaptor]
** @fnote None
**
** @nam3rule Get Return Ensembl Genetic Variation Source Adaptor attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Base Adaptor
**
** @argrule * gvsa [const EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                             Source Adaptor
**
** @valrule Adaptor [EnsPBaseadaptor] Ensembl Base Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensGvsourceadaptorGetBaseadaptor *************************************
**
** Get the Ensembl Base Adaptor element of an
** Ensembl Genetic Variation Source Adaptor.
**
** @param [r] gvsa [const EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                             Source Adaptor
**
** @return [EnsPBaseadaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

EnsPBaseadaptor ensGvsourceadaptorGetBaseadaptor(
    const EnsPGvsourceadaptor gvsa)
{
    if(!gvsa)
        return NULL;

    return gvsa->Adaptor;
}




/* @func ensGvsourceadaptorGetDatabaseadaptor *********************************
**
** Get the Ensembl Database Adaptor element of an
** Ensembl Genetic Variation Source Adaptor.
**
** @param [r] gvsa [const EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                             Source Adaptor
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

EnsPDatabaseadaptor ensGvsourceadaptorGetDatabaseadaptor(
    const EnsPGvsourceadaptor gvsa)
{
    if(!gvsa)
        return NULL;

    return ensBaseadaptorGetDatabaseadaptor(gvsa->Adaptor);
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Genetic Variation Source objects from an
** Ensembl Variation database.
**
** @fdata [EnsPGvsourceadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Genetic Variation Source object(s)
** @nam4rule FetchAll Retrieve all Ensembl Genetic Variation Source objects
** @nam5rule FetchAllBy Retrieve all Ensembl Genetic Variation Source objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Genetic Variation Source object
**                   matching a criterion
**
** @argrule * aa [EnsPAnalysisadaptor] Ensembl Genetic Variation Source Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Genetic Variation Source
**                             objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @funcstatic gvsourceadaptorFetchAll ****************************************
**
** An ajTableMap 'apply' function to return all
** Ensembl Genetic Variation Source objects from the
** Ensembl Genetic Variation Source Adaptor-internal cache.
**
** @param [u] key [const void *] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Genetic Variation Source
**                           value data address
** @param [u] cl [void*] AJAX List of Ensembl Genetic Variation Source objects,
**                       passed in via ajTableMap
** @see ajTableMap
**
** @return [void]
** @@
******************************************************************************/

static void gvsourceadaptorFetchAll(const void *key,
                                    void **value,
                                    void *cl)
{
    if(!key)
        return;

    if(!value)
        return;

    if(!*value)
        return;

    if(!cl)
        return;

    ajListPushAppend((AjPList) cl,
                     (void *) ensGvsourceNewRef(*((EnsPGvsource *) value)));

    return;
}




/* @func ensGvsourceadaptorFetchAll *******************************************
**
** Fetch all Ensembl Genetic Variation Sources.
**
** The caller is responsible for deleting the Ensembl Genetic Variation Source
** before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::get_all_sources
** @param [r] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
** @param [u] gvss [AjPList] AJAX List of Ensembl Genetic Variation Sources
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceadaptorFetchAll(EnsPGvsourceadaptor gvsa,
                                  AjPList gvss)
{
    if(!gvsa)
        return ajFalse;

    if(!gvss)
        return ajFalse;

    if(!gvsa->CacheByIdentifier)
        gvsourceadaptorCacheInit(gvsa);

    ajTableMap(gvsa->CacheByIdentifier,
               gvsourceadaptorFetchAll,
               (void *) gvss);

    return ajTrue;
}




/* @func ensGvsourceadaptorFetchByIdentifier **********************************
**
** Fetch an Ensembl Genetic Variation Source by its SQL database-internal
** identifier.
** The caller is responsible for deleting the Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::*Adaptor::fetch_by_dbID
** @param [r] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pgvs [EnsPGvsource*] Ensembl Genetic Variation Source address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceadaptorFetchByIdentifier(EnsPGvsourceadaptor gvsa,
                                           ajuint identifier,
                                           EnsPGvsource *Pgvs)
{
    AjPList gvss = NULL;

    AjPStr constraint = NULL;

    EnsPGvsource gvs = NULL;

    if(!gvsa)
        return ajFalse;

    if(!identifier)
        return ajFalse;

    if(!Pgvs)
        return ajFalse;

    /*
    ** Initally, search the identifier cache.
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */

    if(!gvsa->CacheByIdentifier)
        gvsourceadaptorCacheInit(gvsa);

    *Pgvs = (EnsPGvsource)
        ajTableFetch(gvsa->CacheByIdentifier, (const void *) &identifier);

    if(*Pgvs)
    {
        ensGvsourceNewRef(*Pgvs);

        return ajTrue;
    }

    /* For a cache miss re-query the database. */

    constraint = ajFmtStr("source.source_id = %u", identifier);

    gvss = ajListNew();

    ensBaseadaptorGenericFetch(gvsa->Adaptor,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               gvss);

    if(ajListGetLength(gvss) > 1)
        ajWarn("ensGvsourceadaptorFetchByIdentifier got more than one "
               "Ensembl Genetic Variation Sources for (PRIMARY KEY) "
               "identifier %u.\n",
               identifier);

    ajListPop(gvss, (void **) Pgvs);

    gvsourceadaptorCacheInsert(gvsa, Pgvs);

    while(ajListPop(gvss, (void **) &gvs))
    {
        gvsourceadaptorCacheInsert(gvsa, &gvs);

        ensGvsourceDel(&gvs);
    }

    ajListFree(&gvss);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensGvsourceadaptorFetchByName ****************************************
**
** Fetch an Ensembl Genetic Variation Source by its name.
** The caller is responsible for deleting the Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::get_source_version
** @param [r] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
** @param [r] name [const AjPStr] Ensembl Genetic Variation Source name
** @param [wP] Pgvs [EnsPGvsource*] Ensembl Genetic Variation Source address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceadaptorFetchByName(EnsPGvsourceadaptor gvsa,
                                     const AjPStr name,
                                     EnsPGvsource *Pgvs)
{
    char *txtname = NULL;

    AjPList gvss = NULL;

    AjPStr constraint = NULL;

    EnsPGvsource gvs = NULL;

    if(!gvsa)
        return ajFalse;

    if((!name) && (!ajStrGetLen(name)))
        return ajFalse;

    if(!Pgvs)
        return ajFalse;

    /*
    ** Initally, search the name cache.
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */

    if(!gvsa->CacheByName)
        gvsourceadaptorCacheInit(gvsa);

    *Pgvs = (EnsPGvsource)
        ajTableFetch(gvsa->CacheByName, (const void *) name);

    if(*Pgvs)
    {
        ensGvsourceNewRef(*Pgvs);

        return ajTrue;
    }

    /* In case of a cache miss, re-query the database. */

    ensBaseadaptorEscapeC(gvsa->Adaptor, &txtname, name);

    constraint = ajFmtStr("source.name = '%s'", txtname);

    ajCharDel(&txtname);

    gvss = ajListNew();

    ensBaseadaptorGenericFetch(gvsa->Adaptor,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               gvss);

    if(ajListGetLength(gvss) > 1)
        ajWarn("ensGvsourceadaptorFetchByName got more than one "
               "Ensembl Genetic Variation Source for (UNIQUE) name '%S'.\n",
               name);

    ajListPop(gvss, (void **) Pgvs);

    gvsourceadaptorCacheInsert(gvsa, Pgvs);

    while(ajListPop(gvss, (void **) &gvs))
    {
        gvsourceadaptorCacheInsert(gvsa, &gvs);

        ensGvsourceDel(&gvs);
    }

    ajListFree(&gvss);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensGvsourceadaptorFetchDefault ***************************************
**
** Fetch the default Ensembl Genetic Variation Source.
** The caller is responsible for deleting the Ensembl Genetic Variation Source.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::get_default_source
** @param [u] gvsa [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                       Source Adaptor
** @param [u] Pgvs [EnsPGvsource*] Ensembl Genetic Variation Source address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsourceadaptorFetchDefault(EnsPGvsourceadaptor gvsa,
                                      EnsPGvsource *Pgvs)
{
    if(!gvsa)
        return ajFalse;

    if(!Pgvs)
        return ajFalse;

    *Pgvs = ensGvsourceNewRef((EnsPGvsource) gvsa->DefaultGvsource);

    return ajTrue;
}




/* @datasection [EnsPGvsynonym] Genetic Variation Synonym *********************
**
** Functions for manipulating Ensembl Genetic Variation Synonym objects
**
** NOTE: The Ensembl Genetic Variation Synonym object has no counterpart in the
** Ensembl Variation Perl API. It has been split out of the
** Bio::EnsEMBL::Variation::Variation object to formalise entries from the
** 'variation_synonym' table.
**
** @cc Bio::EnsEMBL::Variation::Variation CVS Revision: 1.37
**
** @nam2rule Gvsynonym
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Synonym by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Synonym. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvsynonym]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @argrule Ref object [EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @valrule * [EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @fcategory new
******************************************************************************/




/* @func ensGvsynonymNew ******************************************************
**
** Default constructor for an Ensembl Genetic Variation Synonym.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvsa [EnsPGvsynonymadaptor] Ensembl Genetic Variation
**                                        Synonym Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::*::new
** @param [u] gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] name [AjPStr] Name
** @param [u] moleculetype [AjPStr] Molecule type
** @param [r] gvvidentifier [ajuint] Ensembl Genetic Variation identifier
** @param [r] subidentifier [AjBool] Sub identifier
**
** @return [EnsPGvsource] Ensembl Genetic Variation Source
** @@
******************************************************************************/

EnsPGvsynonym ensGvsynonymNew(EnsPGvsynonymadaptor gvsa,
                              ajuint identifier,
                              EnsPGvsource gvsource,
                              AjPStr name,
                              AjPStr moleculetype,
                              ajuint gvvidentifier,
                              ajuint subidentifier)
{
    EnsPGvsynonym gvs = NULL;

    AJNEW0(gvs);

    gvs->Use = 1;

    gvs->Identifier = identifier;

    gvs->Adaptor = gvsa;

    gvs->Gvsource = ensGvsourceNewRef(gvsource);

    if(name)
        gvs->Name = ajStrNewRef(name);

    if(moleculetype)
        gvs->Moleculetype = ajStrNewRef(moleculetype);

    gvs->Gvvariationidentifier = gvvidentifier;

    gvs->Subidentifier = subidentifier;

    return gvs;
}




/* @func ensGvsynonymNewObj ***************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [EnsPGvsynonym] Ensembl Genetic Variation Synonym or NULL
** @@
******************************************************************************/

EnsPGvsynonym ensGvsynonymNewObj(const EnsPGvsynonym object)
{
    EnsPGvsynonym gvs = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvs);

    gvs->Use = 1;

    gvs->Identifier = object->Identifier;

    gvs->Adaptor = object->Adaptor;

    gvs->Gvsource = ensGvsourceNewRef(object->Gvsource);

    if(object->Name)
        gvs->Name = ajStrNewRef(object->Name);

    if(object->Moleculetype)
        gvs->Moleculetype = ajStrNewRef(object->Moleculetype);

    gvs->Gvvariationidentifier = object->Gvvariationidentifier;

    gvs->Subidentifier = object->Subidentifier;

    return gvs;
}




/* @func ensGvsynonymNewRef ***************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @@
******************************************************************************/

EnsPGvsynonym ensGvsynonymNewRef(EnsPGvsynonym gvs)
{
    if(!gvs)
        return NULL;

    gvs->Use++;

    return gvs;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Synonym.
**
** @fdata [EnsPGvsynonym]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Synonym object
**
** @argrule * Pgvs [EnsPGvsynonym*] Genetic Variation Synonym object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvsynonymDel ******************************************************
**
** Default destructor for an Ensembl Genetic Variation Synonym.
**
** @param [d] Pgvs [EnsPGvsynonym*] Ensembl Genetic Variation Synonym address
**
** @return [void]
** @@
******************************************************************************/

void ensGvsynonymDel(EnsPGvsynonym *Pgvs)
{
    EnsPGvsynonym pthis = NULL;

    if(!Pgvs)
        return;

    if(!*Pgvs)
        return;

    if(ajDebugTest("ensGvsynonymDel"))
    {
        ajDebug("ensGvsynonymDel\n"
                "  *Pgvs %p\n",
                *Pgvs);

        ensGvsynonymTrace(*Pgvs, 1);
    }

    pthis = *Pgvs;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvs = NULL;

        return;
    }

    ensGvsourceDel(&pthis->Gvsource);

    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Moleculetype);

    AJFREE(pthis);

    *Pgvs = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Synonym object.
**
** @fdata [EnsPGvsynonym]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Synonym attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Synonym Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGvsource Return the Ensembl Genetic Variation Source
** @nam4rule GetName Return the name
** @nam4rule GetMoleculetype Return the molecule type
** @nam4rule GetGvvariationidentifier Return the Ensembl Genetic Variation
**                                    identifier
** @nam4rule GetSubidentifier Return the sub-identifier
**
** @argrule * gvs [const EnsPGvsynonym] Genetic Variation Synonym
**
** @valrule Adaptor [EnsPGvsynonymadaptor] Ensembl Genetic Variation
**                                         Synonym Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @valrule Name [AjPStr] Name
** @valrule Moleculetype [AjPStr] Molecule type
** @valrule GetGvvariationidentifier [ajuint] Ensembl Genetic Variation
**                                            identifier
** @valrule Subidentifier [ajuint] Sub-identifier
**
** @fcategory use
******************************************************************************/




/* @func ensGvsynonymGetAdaptor ***********************************************
**
** Get the Ensembl Genetic Variation Synonym Adaptor element of an
** Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [EnsPGvsynonymadaptor] Ensembl Genetic Variation Synonym Adaptor
** @@
******************************************************************************/

EnsPGvsynonymadaptor ensGvsynonymGetAdaptor(const EnsPGvsynonym gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Adaptor;
}




/* @func ensGvsynonymGetIdentifier ********************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvsynonymGetIdentifier(const EnsPGvsynonym gvs)
{
    if(!gvs)
        return 0;

    return gvs->Identifier;
}





/* @func ensGvsynonymGetGvsource **********************************************
**
** Get the Ensembl Genetic Variation Source element of an
** Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [EnsPGvsource] Ensembl Genetic Variation Source
** @@
******************************************************************************/

EnsPGvsource ensGvsynonymGetGvsource(const EnsPGvsynonym gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Gvsource;
}





/* @func ensGvsynonymGetName **************************************************
**
** Get the name element of an Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvsynonymGetName(const EnsPGvsynonym gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Name;
}





/* @func ensGvsynonymGetMoleculetype ******************************************
**
** Get the molecule type element of an Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [AjPStr] Molecule type
** @@
******************************************************************************/

AjPStr ensGvsynonymGetMoleculetype(const EnsPGvsynonym gvs)
{
    if(!gvs)
        return NULL;

    return gvs->Moleculetype;
}





/* @func ensGvsynonymGetGvvariationidentifier *********************************
**
** Get the Ensembl Genetic Variation identifier element of an
** Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [ajuint] Ensembl Genetic Variation identifier
** @@
******************************************************************************/

ajuint ensGvsynonymGetGvvariationidentifier(const EnsPGvsynonym gvs)
{
    if(!gvs)
        return 0;

    return gvs->Gvvariationidentifier;
}





/* @func ensGvsynonymGetSubidentifier *****************************************
**
** Get the Sub-identifier element of an Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [ajuint] Sub-identifier
** @@
******************************************************************************/

ajuint ensGvsynonymGetSubidentifier(const EnsPGvsynonym gvs)
{
    if(!gvs)
        return 0;

    return gvs->Subidentifier;
}





/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Synonym object.
**
** @fdata [EnsPGvsynonym]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Synonym
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Synonym Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetGvsource Set the Ensembl Genetic Variation Source
** @nam4rule SetName Set the name
** @nam4rule SetMoleculetype Set the molecule type
** @nam4rule SetGvvariationidentifier Set the Ensembl Genetic Variation
**                                    identifier
** @nam4rule SetSubidentifier Set the sub-identifier
**
** @argrule * gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvsynonymSetAdaptor ***********************************************
**
** Set the Ensembl Genetic Variation Synonym Adaptor element of an
** Ensembl Genetic Variation Synonym.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [u] gvsa [EnsPGvsynonymadaptor] Ensembl Genetic Variation
**                                        Synonym Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymSetAdaptor(EnsPGvsynonym gvs,
                              EnsPGvsynonymadaptor gvsa)
{
    if(!gvs)
        return ajFalse;

    gvs->Adaptor = gvsa;

    return ajTrue;
}




/* @func ensGvsynonymSetIdentifier ********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Synonym.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymSetIdentifier(EnsPGvsynonym gvs,
                                 ajuint identifier)
{
    if(!gvs)
        return ajFalse;

    gvs->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvsynonymSetGvsource **********************************************
**
** Set the Ensembl Genetic Variation Source element of an
** Ensembl Genetic Variation Synonym.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [u] gvsource [EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymSetGvsource(EnsPGvsynonym gvs,
                               EnsPGvsource gvsource)
{
    if(!gvs)
        return ajFalse;

    ensGvsourceDel(&gvs->Gvsource);

    gvs->Gvsource = ensGvsourceNewRef(gvsource);

    return ajTrue;
}




/* @func ensGvsynonymSetName **************************************************
**
** Set the name element of an Ensembl Genetic Variation Synonym.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymSetName(EnsPGvsynonym gvs,
                           AjPStr name)
{
    if(!gvs)
        return ajFalse;

    ajStrDel(&gvs->Name);

    if(name)
        gvs->Name = ajStrNewRef(name);

    return ajTrue;
}




/* @func ensGvsynonymSetMoleculetype ******************************************
**
** Set the molecule type element of an Ensembl Genetic Variation Synonym.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [u] moleculetype [AjPStr] Molecule type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymSetMoleculetype(EnsPGvsynonym gvs,
                                   AjPStr moleculetype)
{
    if(!gvs)
        return ajFalse;

    ajStrDel(&gvs->Moleculetype);

    if(moleculetype)
        gvs->Moleculetype = ajStrNewRef(moleculetype);

    return ajTrue;
}




/* @func ensGvsynonymSetGvvariationidentifier *********************************
**
** Set the Ensembl Genetic Variation identifier element of an
** Ensembl Genetic Variation Synonym.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [r] gvvidentifier [ajuint] Ensembl Genetic Variation identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymSetGvvariationidentifier(EnsPGvsynonym gvs,
                                            ajuint gvvidentifier)
{
    if(!gvs)
        return ajFalse;

    gvs->Gvvariationidentifier = gvvidentifier;

    return ajTrue;
}




/* @func ensGvsynonymSetSubidentifier *****************************************
**
** Set the sub-identifier element of an Ensembl Genetic Variation Synonym.
**
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [r] subidentifier [ajuint] Sub-identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymSetSubidentifier(EnsPGvsynonym gvs,
                                    ajuint subidentifier)
{
    if(!gvs)
        return ajFalse;

    gvs->Subidentifier = subidentifier;

    return ajTrue;
}




/* @func ensGvsynonymGetMemsize ***********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvsynonymGetMemsize(const EnsPGvsynonym gvs)
{
    ajulong size = 0;

    if(!gvs)
        return 0;

    size += sizeof (EnsOGvsynonym);

    size += ensGvsourceGetMemsize(gvs->Gvsource);

    if(gvs->Name)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvs->Name);
    }

    if(gvs->Moleculetype)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvs->Moleculetype);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Synonym object.
**
** @fdata [EnsPGvsynonym]
** @nam3rule Trace Report Ensembl Genetic Variation Synonym elements to
**                 debug file
**
** @argrule Trace gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvsynonymTrace ****************************************************
**
** Trace an Ensembl Genetic Variation Synonym.
**
** @param [r] gvs [const EnsPGvsynonym] Ensembl Genetic Variation Synonym
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvsynonymTrace(const EnsPGvsynonym gvs, ajuint level)
{
    AjPStr indent = NULL;

    if(!gvs)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvsynonymTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Gvsource %p\n"
            "%S  Name '%S'\n"
            "%S  Moleculetype '%S'\n"
            "%S  Gvvariationidentifier %u\n"
            "%S  Subidentifier %u\n",
            indent, gvs,
            indent, gvs->Use,
            indent, gvs->Identifier,
            indent, gvs->Adaptor,
            indent, gvs->Gvsource,
            indent, gvs->Name,
            indent, gvs->Moleculetype,
            indent, gvs->Gvvariationidentifier,
            indent, gvs->Subidentifier);

    ensGvsourceTrace(gvs->Gvsource, level  + 1);

    ajStrDel(&indent);

    return ajTrue;
}




/* @datasection [EnsPGvvariation] Genetic Variation Variation *****************
**
** Functions for manipulating Ensembl Genetic Variation Variation objects
**
** @cc Bio::EnsEMBL::Variation::Variation CVS Revision: 1.37
**
** @nam2rule Gvvariation
**
******************************************************************************/

/* #conststatic gvvariationValidationState ************************************
**
** The Ensembl Genetic Variation Variation validation status element is
** enumerated in both, the SQL table definition and the data structure.
** The following strings are used for conversion in database operations and
** correspond to EnsEGvvariationValidationState and the
** 'variation.validation_status' field.
**
** #attr [static const char**] gvvariationValidationState
** ##
******************************************************************************/

static const char *gvvariationValidationState[] =
{
    "cluster",
    "freq",
    "submitter",
    "doublehit",
    "hapmap",
    "failed",
    "non-polymorphic",
    "observed",
    NULL
};




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Variation by
** pointer. It is the responsibility of the user to first destroy any previous
** Genetic Variation Variation. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvvariation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvvariation] Ensembl Genetic Variation Variation
** @argrule Ref object [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @valrule * [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @fcategory new
******************************************************************************/




/* @func ensGvvariationNew ****************************************************
**
** Default constructor for an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Variation::new
** @param [u] gvs [AjPStr] Ensembl Genetic Variation Source
** @param [u] name [AjPStr] Name
** @param [uN] ancestralallele [AjPStr] Ancestral allele
** @param [uN] gvalleles [AjPList] AJAX List of Ensembl Genetic
**                                Variation Alleles
** @param [uN] gvsynonyms [AjPList] AJAX List of Ensembl Genetic
**                                 Variation Alleles
** @param [uN] validationstates [AjPStr] Comma-separated list of
**                                      validation states
** @param [uN] moltype [AjPStr] Molecule type
** @param [uN] fiveflank [AjPStr] Five prime flanking sequence
** @param [uN] threeflank [AjPStr] Three prime flanking sequence
** @param [uN] faileddescription [AjPStr] Failed description
** @param [r] flank [AjBool] Flank
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationNew(EnsPGvvariationadaptor gvva,
                                  ajuint identifier,
                                  EnsPGvsource gvsource,
                                  AjPStr name,
                                  AjPStr ancestralallele,
                                  AjPList gvalleles,
                                  AjPList gvsynonyms,
                                  AjPStr validationstates,
                                  AjPStr moltype,
                                  AjPStr fiveflank,
                                  AjPStr threeflank,
                                  AjPStr faileddescription,
                                  AjBool flank)
{
    AjIList iter = NULL;

    EnsPGvallele gva = NULL;

    EnsPGvsynonym gvsynonym = NULL;

    EnsPGvvariation gvv = NULL;

    if(!gvsource)
        return NULL;

    if(!name)
        return NULL;

    AJNEW0(gvv);

    gvv->Use = 1;

    gvv->Identifier = identifier;

    gvv->Adaptor = gvva;

    gvv->Gvsource = ensGvsourceNewRef(gvsource);

    if(name)
        gvv->Name = ajStrNewRef(name);

    if(ancestralallele)
        gvv->AncestralAllele = ajStrNewRef(ancestralallele);

    /* Copy the AJAX List of Ensembl Genetic Variation Alleles. */

    if(gvalleles)
    {
        gvv->Gvalleles = ajListNew();

        iter = ajListIterNew(gvalleles);

        while(!ajListIterDone(iter))
        {
            gva = (EnsPGvallele) ajListIterGet(iter);

            ajListPushAppend(gvv->Gvalleles,
                             (void *) ensGvalleleNewRef(gva));
        }

        ajListIterDel(&iter);
    }

    /* Copy the AJAX List of Ensembl Genetic Variation Synonyms. */

    if(gvsynonyms)
    {
        gvv->Gvsynonyms = ajListNew();

        iter = ajListIterNew(gvsynonyms);

        while(!ajListIterDone(iter))
        {
            gvsynonym = (EnsPGvsynonym) ajListIterGet(iter);

            ajListPushAppend(gvv->Gvsynonyms,
                             (void *) ensGvsynonymNewRef(gvsynonym));
        }

        ajListIterDel(&iter);
    }

    if(moltype)
        gvv->MoleculeType = ajStrNewRef(moltype);

    if(fiveflank)
        gvv->FivePrimeFlank = ajStrNewRef(fiveflank);

    if(threeflank)
        gvv->ThreePrimeFlank = ajStrNewRef(threeflank);

    if(faileddescription)
        gvv->FailedDescription = ajStrNewRef(faileddescription);

    gvv->Flank = flank;

    gvv->ValidationStates =
        ensGvvariationValidationStatesFromSet(validationstates);

    return gvv;
}




/* @func ensGvvariationNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvvariation] Ensembl Genetic Variation
**                                           Variation
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation or NULL
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationNewObj(const EnsPGvvariation object)
{
    AjIList iter = NULL;

    EnsPGvallele gva = NULL;

    EnsPGvsynonym gvsynonym = NULL;

    EnsPGvvariation gvv = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvv);

    gvv->Use = 1;

    gvv->Identifier = object->Identifier;

    gvv->Adaptor = object->Adaptor;

    gvv->Gvsource = ensGvsourceNewRef(object->Gvsource);

    if(object->Name)
        gvv->Name = ajStrNewRef(object->Name);

    if(object->AncestralAllele)
        gvv->AncestralAllele = ajStrNewRef(object->AncestralAllele);

    /* Copy the AJAX List of Ensembl Genetic Variation Alleles. */

    if(object->Gvalleles)
    {
        gvv->Gvalleles = ajListNew();

        iter = ajListIterNew(object->Gvalleles);

        while(!ajListIterDone(iter))
        {
            gva = (EnsPGvallele) ajListIterGet(iter);

            ajListPushAppend(gvv->Gvalleles,
                             (void *) ensGvalleleNewRef(gva));
        }

        ajListIterDel(&iter);
    }

    /* Copy the AJAX List of Ensembl Genetic Variation Synonyms. */

    if(object->Gvsynonyms)
    {
        gvv->Gvsynonyms = ajListNew();

        iter = ajListIterNew(object->Gvsynonyms);

        while(!ajListIterDone(iter))
        {
            gvsynonym = (EnsPGvsynonym) ajListIterGet(iter);

            ajListPushAppend(gvv->Gvsynonyms,
                             (void *) ensGvsynonymNewRef(gvsynonym));
        }

        ajListIterDel(&iter);
    }

    if(object->MoleculeType)
        gvv->MoleculeType = ajStrNewRef(object->MoleculeType);

    if(object->FivePrimeFlank)
        gvv->FivePrimeFlank = ajStrNewRef(object->FivePrimeFlank);

    if(object->ThreePrimeFlank)
        gvv->ThreePrimeFlank = ajStrNewRef(object->ThreePrimeFlank);

    if(object->FailedDescription)
        gvv->FailedDescription = ajStrNewRef(object->FailedDescription);

    gvv->Flank = object->Flank;

    gvv->ValidationStates = object->ValidationStates;

    return gvv;
}




/* @func ensGvvariationNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationNewRef(EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    gvv->Use++;

    return gvv;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Variation.
**
** @fdata [EnsPGvvariation]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Variation object
**
** @argrule * Pgvv [EnsPGvvariation*] Genetic Variation Variation
**                                    object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvariationDel ****************************************************
**
** Default destructor for an Ensembl Genetic Variation Variation.
**
** @param [d] Pgvv [EnsPGvvariation*] Ensembl Genetic Variation
**                                    Variation address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationDel(EnsPGvvariation *Pgvv)
{
    EnsPGvallele gva = NULL;

    EnsPGvsynonym gvsynonym = NULL;

    EnsPGvvariation pthis = NULL;

    if(!Pgvv)
        return;

    if(!*Pgvv)
        return;

    if(ajDebugTest("ensGvvariationDel"))
    {
        ajDebug("ensGvvariationDel\n"
                "  *Pgvv %p\n",
                *Pgvv);

        ensGvvariationTrace(*Pgvv, 1);
    }

    pthis = *Pgvv;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvv = NULL;

        return;
    }

    ensGvsourceDel(&pthis->Gvsource);

    ajStrDel(&pthis->Name);

    ajStrDel(&pthis->AncestralAllele);

    /* Clear the AJAX List of Ensembl Genetic Variation Alleles. */

    while(ajListPop(pthis->Gvalleles, (void **) &gva))
        ensGvalleleDel(&gva);

    ajListFree(&pthis->Gvalleles);

    /* Clear the AJAX List of Ensembl Genetic Variation Synonyms. */

    while(ajListPop(pthis->Gvsynonyms, (void **) &gvsynonym))
        ensGvsynonymDel(&gvsynonym);

    ajListFree(&pthis->Gvsynonyms);

    ajStrDel(&pthis->MoleculeType);
    ajStrDel(&pthis->FivePrimeFlank);
    ajStrDel(&pthis->ThreePrimeFlank);
    ajStrDel(&pthis->FailedDescription);

    AJFREE(pthis);

    *Pgvv = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Variation object.
**
** @fdata [EnsPGvvariation]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Variation attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Variation Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGvsource Return the Ensembl Genetic Variation Source
** @nam4rule GetName Return the name
** @nam4rule GetAncestralAllele Return the ancestral allele
** @nam4rule GetGvalleles Return Ensembl Genetic Variation Alleles
** @nam4rule GetGvsynoyms Return Ensembl Genetic Variation Synonms
** @nam4rule GetValidationStates Return the validation states
** @nam4rule GetMoleculeType Return the molecule type
** @nam4rule GetFivePrimeFlank Return the five prime flank
** @nam4rule GetThreePrimeFlank Return the three prime flank
** @nam4rule GetFailedDescription Return the failed description
** @nam4rule GetFlank Return the flank
**
** @argrule * gvv [const EnsPGvvariation] Genetic Variation Variation
**
** @valrule Adaptor [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                           Variation Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @valrule Name [AjPStr] Name
** @valrule AncestralAllele [AjPStr] Ancestral allele
** @valrule Gvalleles [const AjPList] Ensembl Genetic Variation Alleles
** @valrule Gvsynonyms [const AjPList] Ensembl Genetic Variation Synoyms
** @valrule ValidationStates [ajuint] Validation states
** @valrule MoleculeType [AjPStr] Molecule type
** @valrule FivePrimeFlank [AjPStr] Five prime flank
** @valrule ThreePrimeFlank [AjPStr] Three prime flank
** @valrule FailedDescription [AjPStr] Failed description
** @valrule Flank [AjBool] Flank
**
** @fcategory use
******************************************************************************/




/* @func ensGvvariationGetAdaptor *********************************************
**
** Get the Ensembl Genetic Variation Variation Adaptor element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                  Variation Adaptor
** @@
******************************************************************************/

EnsPGvvariationadaptor ensGvvariationGetAdaptor(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->Adaptor;
}




/* @func ensGvvariationGetIdentifier ******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvvariationGetIdentifier(const EnsPGvvariation gvv)
{
    if(!gvv)
        return 0;

    return gvv->Identifier;
}




/* @func ensGvvariationGetGvsource ********************************************
**
** Get the Ensembl Genetic Variation Source element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::source
** @cc Bio::EnsEMBL::Variation::Variation::source_description
** @cc Bio::EnsEMBL::Variation::Variation::source_url
** @cc Bio::EnsEMBL::Variation::Variation::is_somatic
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [EnsPGvsource] Ensembl Genetic Variation Source
** @@
******************************************************************************/

EnsPGvsource ensGvvariationGetGvsource(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->Gvsource;
}




/* @func ensGvvariationGetName ************************************************
**
** Get the name element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::name
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvvariationGetName(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->Name;
}




/* @func ensGvvariationGetAncestralAllele *************************************
**
** Get the ancestral allele element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::ancestral_allele
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Ancestral allele
** @@
******************************************************************************/

AjPStr ensGvvariationGetAncestralAllele(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->AncestralAllele;
}




/* @func ensGvvariationGetGvalleles *******************************************
**
** Get all Ensembl Genetic Variation Alleles of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_Alleles
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [const AjPList] AJAX List of Ensembl Genetic Variation Alleles
** @@
******************************************************************************/

const AjPList ensGvvariationGetGvalleles(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->Gvalleles;
}




/* @func ensGvvariationGetGvsynonyms ******************************************
**
** Get the AJAx List of Ensembl Genetic Variation Synonyms element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_synonyms
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @see ensGvvariationFetchAllGvsynonyms
** @see ensGvvariationFetchAllGvsynonymGvsources
**
** @return [const AjPList] Synonyms
** @@
******************************************************************************/

const AjPList ensGvvariationGetGvsynonyms(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->Gvsynonyms;
}




/* @func ensGvvariationGetValidationStates ************************************
**
** Get all validation states of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_validation_states
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [ajuint] Validation states
** @@
******************************************************************************/

ajuint ensGvvariationGetValidationStates(const EnsPGvvariation gvv)
{
    if(!gvv)
        return 0;

    return gvv->ValidationStates;
}




/* @func ensGvvariationGetMoleculeType ****************************************
**
** Get the molecule type element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::moltype
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Molecule type
** @@
******************************************************************************/

AjPStr ensGvvariationGetMoleculeType(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->MoleculeType;
}




/* @func ensGvvariationGetFivePrimeFlank **************************************
**
** Get the five prime flank element of an Ensembl Genetic Variation Variation.
**
** This is not a simple accessor function, it will fetch the five prime
** flanking sequence from the Ensembl Variation database in case it is not
** defined.
**
** @cc Bio::EnsEMBL::Variation::Variation::five_prime_flanking_seq
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Five prime flank
** @@
******************************************************************************/

AjPStr ensGvvariationGetFivePrimeFlank(EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    if(gvv->FivePrimeFlank)
        return gvv->FivePrimeFlank;

    if(gvv->Adaptor)
        ensGvvariationadaptorFetchFlankingSequence(gvv->Adaptor,
                                                   gvv->Identifier,
                                                   &gvv->FivePrimeFlank,
                                                   &gvv->ThreePrimeFlank);

    return gvv->FivePrimeFlank;
}




/* @func ensGvvariationGetThreePrimeFlank *************************************
**
** Get the three prime flank element of an Ensembl Genetic Variation Variation.
**
** This is not a simple accessor function, it will fetch the three prime
** flanking sequence from the Ensembl Variation database in case it is not
** defined.
**
** @cc Bio::EnsEMBL::Variation::Variation::three_prime_flanking_seq
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Three prime flank
** @@
******************************************************************************/

AjPStr ensGvvariationGetThreePrimeFlank(EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    if(gvv->ThreePrimeFlank)
        return gvv->ThreePrimeFlank;

    if(gvv->Adaptor)
        ensGvvariationadaptorFetchFlankingSequence(gvv->Adaptor,
                                                   gvv->Identifier,
                                                   &gvv->FivePrimeFlank,
                                                   &gvv->ThreePrimeFlank);

    return gvv->ThreePrimeFlank;
}




/* @func ensGvvariationGetFailedDescription ***********************************
**
** Get the failed description element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::failed_description
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Failed description
** @@
******************************************************************************/

AjPStr ensGvvariationGetFailedDescription(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;

    return gvv->FailedDescription;
}




/* @func ensGvvariationGetFlank ***********************************************
**
** Get the flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::flank_flag
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjBool] Flank flag
** @@
******************************************************************************/

AjBool ensGvvariationGetFlank(const EnsPGvvariation gvv)
{
    if(!gvv)
        return ajFalse;

    return gvv->Flank;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Variation object.
**
** @fdata [EnsPGvvariation]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Variation
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Variation Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetGvsource Set the Ensembl Genetic Variation Source
** @nam4rule SetName Set the name
** @nam4rule SetSourceDescription Set the source description
** @nam4rule SetSourceURL Set the source URL
** @nam4rule SetAncestralAllele Set the ancestral allele
** @nam4rule SetMoleculeType Set the molecule type
** @nam4rule SetFivePrimeFlank Set the five prime flank
** @nam4rule SetThreePrimeFlank Set the three prime flank
** @nam4rule SetFailedDescription Set the failed description
** @nam4rule SetFlank Set the flank
**
** @argrule * gvv [EnsPGvvariation] Ensembl Genetic Variation Variation object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvvariationSetAdaptor *********************************************
**
** Set the Ensembl Genetic Variation Variation Adaptor element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetAdaptor(EnsPGvvariation gvv,
                                EnsPGvvariationadaptor gvva)
{
    if(!gvv)
        return ajFalse;

    gvv->Adaptor = gvva;

    return ajTrue;
}




/* @func ensGvvariationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetIdentifier(EnsPGvvariation gvv,
                                   ajuint identifier)
{
    if(!gvv)
        return ajFalse;

    gvv->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvvariationSetGvsource ********************************************
**
** Set the Ensembl Genetic Variation Source element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::source
** @cc Bio::EnsEMBL::Variation::Variation::source_description
** @cc Bio::EnsEMBL::Variation::Variation::source_url
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gvs [AjPStr] Ensembl Genetic Variation Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetGvsource(EnsPGvvariation gvv,
                                 EnsPGvsource gvs)
{
    if(!gvv)
        return ajFalse;

    ensGvsourceDel(&gvv->Gvsource);

    gvv->Gvsource = ensGvsourceNewRef(gvs);

    return ajTrue;
}




/* @func ensGvvariationSetName ************************************************
**
** Set the name element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::name
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetName(EnsPGvvariation gvv,
                             AjPStr name)
{
    if(!gvv)
        return ajFalse;

    ajStrDel(&gvv->Name);

    if(name)
        gvv->Name = ajStrNewRef(name);

    return ajTrue;
}




/* @func ensGvvariationSetAncestralAllele *************************************
**
** Set the ancestral allele element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::ancestral_allele
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] ancestralallele [AjPStr] Ancestral allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetAncestralAllele(EnsPGvvariation gvv,
                                        AjPStr ancestralallele)
{
    if(!gvv)
        return ajFalse;

    ajStrDel(&gvv->AncestralAllele);

    if(ancestralallele)
        gvv->AncestralAllele = ajStrNewRef(ancestralallele);

    return ajTrue;
}




/* @func ensGvvariationSetMoleculeType ****************************************
**
** Set the molecule type element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::moltype
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] moltype [AjPStr] Molecule type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetMoleculeType(EnsPGvvariation gvv,
                                     AjPStr moltype)
{
    if(!gvv)
        return ajFalse;

    ajStrDel(&gvv->AncestralAllele);

    if(moltype)
        gvv->AncestralAllele = ajStrNewRef(moltype);

    return ajTrue;
}




/* @func ensGvvariationSetFivePrimeFlank **************************************
**
** Set the five prime flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::five_prime_flanking_seq
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] fiveflank [AjPStr] Five prime flank
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetFivePrimeFlank(EnsPGvvariation gvv,
                                       AjPStr fiveflank)
{
    if(!gvv)
        return ajFalse;

    ajStrDel(&gvv->FivePrimeFlank);

    if(fiveflank)
        gvv->FivePrimeFlank = ajStrNewRef(fiveflank);

    return ajTrue;
}




/* @func ensGvvariationSetThreePrimeFlank *************************************
**
** Set the three prime flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::three_prime_flanking_seq
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] threeflank [AjPStr] Three prime flank
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetThreePrimeFlank(EnsPGvvariation gvv,
                                        AjPStr threeflank)
{
    if(!gvv)
        return ajFalse;

    ajStrDel(&gvv->ThreePrimeFlank);

    if(threeflank)
        gvv->ThreePrimeFlank = ajStrNewRef(threeflank);

    return ajTrue;
}




/* @func ensGvvariationSetFailedDescription ***********************************
**
** Set the failed description element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::failed_description
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] faileddescription [AjPStr] Failed description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetFailedDescription(EnsPGvvariation gvv,
                                          AjPStr faileddescription)
{
    if(!gvv)
        return ajFalse;

    ajStrDel(&gvv->FailedDescription);

    if(faileddescription)
        gvv->FailedDescription = ajStrNewRef(faileddescription);

    return ajTrue;
}




/* @func ensGvvariationSetFlank ***********************************************
**
** Set the flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::failed_description
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] flank [AjBool] Flank
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetFlank(EnsPGvvariation gvv,
                              AjBool flank)
{
    if(!gvv)
        return ajFalse;

    gvv->Flank = flank;

    return ajTrue;
}




/* @func ensGvvariationGetMemsize *********************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvvariationGetMemsize(const EnsPGvvariation gvv)
{
    ajulong size = 0;

    AjIList iter = NULL;

    EnsPGvallele gva = NULL;

    EnsPGvsynonym gvsynonym = NULL;

    if(!gvv)
        return 0;

    size += sizeof (EnsOGvvariation);

    size += ensGvsourceGetMemsize(gvv->Gvsource);

    if(gvv->Name)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvv->Name);
    }

    if(gvv->AncestralAllele)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvv->AncestralAllele);
    }

    /* Summarise the AJAX List of Ensembl Genetic Variation Alleles. */

    iter = ajListIterNewread(gvv->Gvalleles);

    while(!ajListIterDone(iter))
    {
        gva = (EnsPGvallele) ajListIterGet(iter);

        size += ensGvalleleGetMemsize(gva);
    }

    ajListIterDel(&iter);

    /* Summarise the AJAX List of Ensembl Genetic Variation Synonyms. */

    iter = ajListIterNewread(gvv->Gvsynonyms);

    while(!ajListIterDone(iter))
    {
        gvsynonym = (EnsPGvsynonym) ajListIterGet(iter);

        size += ensGvsynonymGetMemsize(gvsynonym);
    }

    ajListIterDel(&iter);

    if(gvv->MoleculeType)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvv->MoleculeType);
    }

    if(gvv->FivePrimeFlank)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvv->FivePrimeFlank);
    }

    if(gvv->ThreePrimeFlank)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvv->ThreePrimeFlank);
    }

    if(gvv->FailedDescription)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvv->FailedDescription);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Variation object.
**
** @fdata [EnsPGvvariation]
** @nam3rule Trace Report Ensembl Genetic Variation Variation elements
**                 to debug file
**
** @argrule Trace gvv [const EnsPGvvariation] Ensembl Genetic Variation
**                                            Variation
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvvariationTrace **************************************************
**
** Trace an Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationTrace(const EnsPGvvariation gvv, ajuint level)
{
    AjIList iter = NULL;

    AjPStr indent = NULL;
    AjPStr states = NULL;

    EnsPGvallele gva = NULL;

    EnsPGvsynonym gvs = NULL;

    if(!gvv)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvvariationTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Gvsource %p\n"
            "%S  Name '%S'\n"
            "%S  AncestralAllele '%S'\n"
            "%S  Gvalleles %p\n"
            "%S  Gvsynonyms %p\n"
            "%S  MoleculeType '%S'\n"
            "%S  FivePrimeFlank '%S'\n"
            "%S  ThreePrimeFlank '%S'\n"
            "%S  FailedDescription '%S'\n"
            "%S  ValidationStates %u\n"
            "%S  Flank '%B'\n",
            indent, gvv,
            indent, gvv->Use,
            indent, gvv->Identifier,
            indent, gvv->Adaptor,
            indent, gvv->Gvsource,
            indent, gvv->Name,
            indent, gvv->AncestralAllele,
            indent, gvv->Gvalleles,
            indent, gvv->Gvsynonyms,
            indent, gvv->MoleculeType,
            indent, gvv->FivePrimeFlank,
            indent, gvv->ThreePrimeFlank,
            indent, gvv->FailedDescription,
            indent, gvv->ValidationStates,
            indent, gvv->Flank);

    states = ajStrNew();

    ensGvvariationValidationStatesToSet(gvv->ValidationStates, &states);

    ajDebug("%S  Validation States SQL set: '%S'\n", indent, states);

    ajStrDel(&states);

    ensGvsourceTrace(gvv->Gvsource, level + 1);

    /* Trace the AJAX List of Ensembl Genetic Variation Alleles. */

    ajDebug("%S  Ensembl Genetic Variation Alleles:\n", indent);

    iter = ajListIterNew(gvv->Gvalleles);

    while(!ajListIterDone(iter))
    {
        gva = (EnsPGvallele) ajListIterGet(iter);

        ensGvalleleTrace(gva, level + 1);
    }

    ajListIterDel(&iter);

    /* Trace the AJAX List of Ensembl Genetic Variation Synonyms. */

    ajDebug("%S  Ensembl Genetic Variation Synonyms:\n", indent);

    iter = ajListIterNew(gvv->Gvsynonyms);

    while(!ajListIterDone(iter))
    {
        gvs = (EnsPGvsynonym) ajListIterGet(iter);

        ensGvsynonymTrace(gvs, level + 1);
    }

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensGvvariationAddGvallele ********************************************
**
** Add an Ensembl Genetic Variation Allele to an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::add_Allele
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationAddGvallele(EnsPGvvariation gvv, EnsPGvallele gva)
{
    if(!gvv)
        return ajFalse;

    if(!gva)
        return ajFalse;

    if(!gvv->Gvalleles)
        gvv->Gvalleles = ajListNew();

    ajListPushAppend(gvv->Gvalleles, (void *) ensGvalleleNewRef(gva));

    return ajTrue;
}




/* @func ensGvvariationAddGvsynonym *******************************************
**
** Add an Ensembl Genetic Variation Synonym to an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::add_synonym
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gvs [EnsPGvsynonym] Ensembl Genetic Variation Synonym
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationAddGvsynonym(EnsPGvvariation gvv, EnsPGvsynonym gvs)
{
    if(!gvv)
        return ajFalse;

    if(!gvs)
        return ajFalse;

    if(!gvv->Gvsynonyms)
        gvv->Gvsynonyms = ajListNew();

    ajListPushAppend(gvv->Gvsynonyms, (void *) ensGvsynonymNewRef(gvs));

    return ajTrue;
}




/* @func ensGvvariationAddValidationState *************************************
**
** Add a validation state to an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::add_validation_state
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] state [EnsEGvvariationValidationState] Ensembl Genetic
**                                                   Variation Variation state
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationAddValidationState(EnsPGvvariation gvv,
                                        EnsEGvvariationValidationState state)
{
    if(!gvv)
        return ajFalse;

    gvv->ValidationStates |= (1 << state);

    return ajTrue;
}




/* @func ensGvvariationValidationStateFromStr *********************************
**
** Convert an AJAX String into an Ensembl Genetic Variation Variation
** validation state element.
**
** @param [r] state [const AjPStr] Validation state string
**
** @return [EnsEGvvariationValidationState] Ensembl Genetic Variation
**                                          Variation validation state or
**                                          ensEGvvariationValidationStateNULL
** @@
******************************************************************************/

EnsEGvvariationValidationState ensGvvariationValidationStateFromStr(
    const AjPStr state)
{
    register EnsEGvvariationValidationState i =
        ensEGvvariationValidationStateNULL;

    EnsEGvvariationValidationState estate =
        ensEGvvariationValidationStateNULL;

    for(i = ensEGvvariationValidationStateCluster;
        gvvariationValidationState[i];
        i++)
        if(ajStrMatchC(state, gvvariationValidationState[i]))
            estate = i;

    if(!estate)
        ajDebug("ensGvvariationValidationStateFromStr encountered "
                "unexpected string '%S'.\n", state);

    return estate;
}




/* @func ensGvvariationValidationStateToChar **********************************
**
** Convert an Ensembl Genetic Variation Variation validation state element
** into a C-type (char*) string.
**
** @param [r] state [EnsEGvvariationValidationState] Ensembl Genetic Variation
**                                                   Variation validation state
**
** @return [const char*] Ensembl Genetic Variation Variation validation state
**                       C-type (char*) string
** @@
******************************************************************************/

const char* ensGvvariationValidationStateToChar(
    EnsEGvvariationValidationState state)
{
    register EnsEGvvariationValidationState i =
        ensEGvvariationValidationStateNULL;

    if(!state)
        return NULL;

    for(i = ensEGvvariationValidationStateCluster;
        gvvariationValidationState[i] && (i < state);
        i++);

    if(!gvvariationValidationState[i])
        ajDebug("ensGvvariationValidationStateToChar encountered an "
                "out of boundary error on gender %d.\n", state);

    return gvvariationValidationState[i];
}




/* @func ensGvvariationValidationStatesFromSet ********************************
**
** Convert an AJAX String representing a comma-separared SQL set of validation
** states into an Ensembl Genetic Variation Variation validation state element.
** The validation states are represented as bits of an AJAX unsigned integer.
**
** @param [r] set [const AjPStr] Comma-separated SQL set
**
** @return [ajuint] Ensembl Genetic Variation Variation validation state or 0
** @@
******************************************************************************/

ajuint ensGvvariationValidationStatesFromSet(const AjPStr set)
{
    ajuint states = 0;

    AjPStr state = NULL;

    AjPStrTok token = NULL;

    if(!set)
        return 0;

    /* Split the comma-separated list of validation states. */

    token = ajStrTokenNewC(set, ",");

    state = ajStrNew();

    while(ajStrTokenNextParse(&token, &state))
        states |= (1 << ensGvvariationValidationStateFromStr(state));

    ajStrDel(&state);

    ajStrTokenDel(&token);

    return states;
}




/* @func ensGvvariationValidationStatesToSet **********************************
**
** Convert an AJAX Integer into an AJAX String representing a comma-separared
** SQL set of Ensembl Genetic Variation Variation validation states.
** The validation states are represented as bits of an AJAX unsigned integer.
**
** The caller is responsible for deleting the AJAX String.
**
** @param [r] set [const AjPStr] Comma-separated SQL set
**
** @return [ajuint] Ensembl Genetic Variation Variation validation state or 0
** @@
******************************************************************************/

AjBool ensGvvariationValidationStatesToSet(ajuint states, AjPStr *Pstr)
{
    register ajuint i = 0;

    if(*Pstr)
        ajStrAssignClear(Pstr);
    else
        *Pstr = ajStrNew();

    for(i = 1; gvvariationValidationState[i]; i++)
        if(states & (1 << i))
        {
            ajStrAppendC(Pstr, gvvariationValidationState[i]);
            ajStrAppendC(Pstr, ",");
        }

    /* Remove the last comma if one exists. */

    if(ajStrGetLen(*Pstr) > 0)
        ajStrCutEnd(Pstr, 1);

    return ajTrue;
}




/* @func ensGvvariationFetchAllGvsynonyms *************************************
**
** Fetch all Ensembl Genetic Variation Synonyms of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_synonyms
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] gvsourcename [const AjPStr] Ensembl Genetic Variation Source name
** @param [u] gvss [AjPList] AJAX List of Ensembl Genetic Variation Synonyms
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationFetchAllGvsynonyms(const EnsPGvvariation gvv,
                                        const AjPStr gvsourcename,
                                        AjPList gvss)
{
    AjIList iter = NULL;

    EnsPGvsource gvsource = NULL;

    EnsPGvsynonym gvsynonym = NULL;

    if(!gvv)
        return ajFalse;

    if(!gvss)
        return ajFalse;

    iter = ajListIterNew(gvv->Gvsynonyms);

    while(!ajListIterDone(iter))
    {
        gvsynonym = (EnsPGvsynonym) ajListIterGet(iter);

        if(gvsourcename && ajStrGetLen(gvsourcename))
        {
            gvsource = ensGvsynonymGetGvsource(gvsynonym);

            if(ajStrMatchS(ensGvsourceGetName(gvsource), gvsourcename))
                ajListPushAppend(gvss, (void *) ensGvsynonymNewRef(gvsynonym));
        }
        else
            ajListPushAppend(gvss, (void *) ensGvsynonymNewRef(gvsynonym));
    }

    ajListIterDel(&iter);

    return ajTrue;
}




/* @func ensGvvariationFetchAllGvsynonymGvsources *****************************
**
** Fetch all Ensembl Genetic Variation Sources of
** Ensembl Genetic Variation Synonyms of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_synonym_sources
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gvss [AjPList] AJAX List of Ensembl Genetic Variation Sources
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationFetchAllGvsynonymGvsources(const EnsPGvvariation gvv,
                                                AjPList gvss)
{
    AjBool found = AJFALSE;

    AjIList iter1 = NULL;
    AjIList iter2 = NULL;

    EnsPGvsource gvs1 = NULL;
    EnsPGvsource gvs2 = NULL;

    if(!gvv)
        return ajFalse;

    if(!gvss)
        return ajFalse;

    iter1 = ajListIterNew(gvv->Gvsynonyms);
    iter2 = ajListIterNew(gvss);

    while(!ajListIterDone(iter1))
    {
        gvs1 = (EnsPGvsource) ajListIterGet(iter1);

        ajListIterRewind(iter2);

        found = ajFalse;

        while(!ajListIterDone(iter2))
        {
            gvs2 = (EnsPGvsource) ajListIterGet(iter2);

            if(ensGvsourceMatch(gvs1, gvs2))
            {
                found = ajTrue;

                break;
            }
        }

        if(!found)
            ajListPushAppend(gvss, (void *) ensGvsourceNewRef(gvs1));
    }

    ajListIterDel(&iter1);
    ajListIterDel(&iter2);

    return ajTrue;
}




/* @funcstatic gvvariationClearGenes ******************************************
**
** An ajTableMapDel 'apply' function to clear an AJAX Table of Ensembl Genes
** used in ensGvvariationFetchAllGenes. This function deletes the AJAX unsigned
** identifier key data and moves the Ensembl Gene value data onto the
** AJAX List.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Gene value data address
** @param [u] cl [void*] AJAX List, passed in from ajTableMapDel
** @see ajTableMapDel
** @see ensGvvariationFetchAllGenes
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationClearGenes(void **key,
                                  void **value,
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

    if(!cl)
        return;

    AJFREE(*key);

    ajListPushAppend(cl, value);

    return;
}




/* @func ensGvvariationFetchAllGenes ******************************************
**
** Fetch all Ensembl Genes where an Ensembl Genetic Variation Variation has
** a consequence.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_Genes
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] flank [ajint] Overlap between Genes and Varaiton Features,
**                          defaults to 5000 base pairs
** @param [u] genes [AjPList] AJAX List of Ensembl Genes
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationFetchAllGenes(EnsPGvvariation gvv,
                                   ajint flank,
                                   AjPList genes)
{
    ajuint *Pidentifier = NULL;

    ajuint identifier = 0;

    AjPList genelist = NULL;
    AjPList gvvfs    = NULL;

    AjPTable genetable = NULL;

    EnsPDatabaseadaptor coredba = NULL;

    EnsPFeature gfeature = NULL;
    EnsPFeature vfeature = NULL;

    EnsPGene        gene  = NULL;
    EnsPGeneadaptor genea = NULL;

    EnsPGvvariationfeature        gvvf  = NULL;
    EnsPGvvariationfeatureadaptor gvvfa = NULL;

    EnsPSlice        slice  = NULL;
    EnsPSlice     newslice  = NULL;
    EnsPSliceadaptor slicea = NULL;

    if(!gvv)
        return ajFalse;

    if(!flank)
        flank = 5000;

    if(!genes)
        return ajFalse;

    if(!gvv->Adaptor)
        return ajTrue;

    genetable = MENSTABLEUINTNEW(0);

    coredba = ensRegistryGetDatabaseadaptor(
        ensEDatabaseadaptorGroupCore,
        ensDatabaseadaptorGetSpecies(gvv->Adaptor));

    slicea = ensRegistryGetSliceadaptor(coredba);

    genea = ensRegistryGetGeneadaptor(coredba);

    genelist = ajListNew();

    gvvfa = ensRegistryGetGvvariationfeatureadaptor(gvv->Adaptor);

    gvvfs = ajListNew();

    ensGvvariationfeatureadaptorFetchAllByGvvariation(gvvfa, gvv, gvvfs);

    /*
    ** Foreach Ensembl Genetic Variation Variation Feature, get the Slice
    ** is on, use the USTREAM and DOWNSTREAM limits to get all the Genes,
    ** and see if the Ensembl Genetic Variation Variation Feature is within
    ** the gene.
    */

    while(ajListPop(gvvfs, (void **) &gvvf))
    {
        /* Fetch an expanded Feature Slice. */

        vfeature = ensGvvariationfeatureGetFeature(gvvf);

        ensSliceadaptorFetchByFeature(slicea, vfeature, flank, &slice);

        /* Fetch all Genes on the new Slice. */

        ensGeneadaptorFetchAllBySlice(genea,
                                      newslice,
                                      (AjPStr) NULL,
                                      (AjPStr) NULL,
                                      (AjPStr) NULL,
                                      ajFalse,
                                      genelist);

        while(ajListPop(genelist, (void **) &gene))
        {
            if((ensFeatureGetSeqregionStart(vfeature)
                >= (ensFeatureGetSeqregionStart(gfeature) - flank))
               &&
               (ensFeatureGetSeqregionStart(vfeature)
                <= (ensFeatureGetSeqregionEnd(gfeature) + flank))
               &&
               (ensFeatureGetSeqregionEnd(vfeature)
                <= (ensFeatureGetSeqregionEnd(gfeature) + flank)))
            {
                /*
                ** The Ensembl Genetic Variation Variation Feature is
                ** affecting the Gene, add to the AJAX Table if not present
                ** already
                */

                identifier = ensGeneGetIdentifier(gene);

                if(ajTableFetch(genetable, &identifier) == NULL)
                {
                    AJNEW0(Pidentifier);

                    *Pidentifier = ensGeneGetIdentifier(gene);

                    ajTablePut(genetable,
                               (void *) Pidentifier,
                               (void *) ensGeneNewRef(gene));
                }

                ensGeneDel(&gene);
            }
        }

        ensSliceDel(&slice);

        ensGvvariationfeatureDel(&gvvf);
    }

    ajListFree(&genelist);
    ajListFree(&gvvfs);

    /* Move all Ensembl Genes from the AJAX Table onto the AJAX List. */

    ajTableMapDel(genetable, gvvariationClearGenes, genes);

    ajTableFree(&genetable);

    return ajTrue;
}




/* @func ensGvvariationFetchAllGvvariationfeatures ****************************
**
** Fetch all Ensembl Genetic Variation Variation Features of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_VariationFeatures
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationFetchAllGvvariationfeatures(EnsPGvvariation gvv,
                                                 AjPList gvvfs)
{
    EnsPGvvariationfeatureadaptor gvvfa = NULL;

    if(!gvv)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    if(!gvv->Adaptor)
    {
        ajDebug("ensGvvariationFetchAllGvvariationfeatures got an "
                "Ensembl Genetic Variation Variation without an "
                "Ensembl Genetic Variation Variation Adaptor attached.\n");

        return ajTrue;
    }

    gvvfa = ensRegistryGetGvvariationfeatureadaptor(gvv->Adaptor);

    return ensGvvariationfeatureadaptorFetchAllByGvvariation(gvvfa,
                                                             gvv,
                                                             gvvfs);
}




/* @funcstatic tableClearGvvariations *****************************************
**
** An ajTableMapDel 'apply' function to clear the AJAX Table of Ensembl Genetic
** Variation Variation identifier key data and Ensembl Genetic Variation
** Variation object value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Genetic Variation Variation objects
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
** @see ensTableDeleteGvvariations
**
** @return [void]
** @@
******************************************************************************/

static void tableClearGvvariations(void **key,
                                   void **value,
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

    ensGvvariationDel((EnsPGvvariation *) value);

    *key   = NULL;
    *value = NULL;

    return;
}




/* @func ensTableClearGvvariations ********************************************
**
** Utility function to clear and delete AJAX Tables of AJAX unsigned integer
** key data and Ensembl Genetic Variation Variation object value data.
**
** @param [d] Pgvvs [AjPTable*] AJAX Table
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensTableDeleteGvvariations(AjPTable *Pgvvs)
{
    AjPTable pthis = NULL;

    if(!Pgvvs)
        return ajFalse;

    if(!*Pgvvs)
        return ajFalse;

    pthis = *Pgvvs;

    ajTableMapDel(pthis, tableClearGvvariations, NULL);

    ajTableFree(&pthis);

    *Pgvvs = NULL;

    return ajTrue;
}




/*
** TODO: The following methods are not implemented:
**   get_VariationFeature_by_dbID
**   get_all_IndividualGenotypes
**   get_all_PopulationGenotypes
**   add_PopulationGenotype
**   ambig_code
**   var_class
**   derived_allele_frequency
**   derived_allele
*/




/* @datasection [EnsPGvvariationadaptor] Genetic Variation Variation Adaptor **
**
** Functions for manipulating Ensembl Genetic Variation Variation Adaptor
** objects
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor CVS Revision: 1.46
**
** @nam2rule Gvvariationadaptor
**
******************************************************************************/




/* @funcstatic gvvariationadaptorClearAlleles *********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal Genetic Variation Alleles cache. This function
** deletes the unsigned identifier key data and the Ensembl Genetic Variation
** Alleles value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Genetic Variation Allele
**                            value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationadaptorClearAlleles(void **key,
                                           void **value,
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

    ensGvalleleDel((EnsPGvallele *) value);

    return;
}




/* @funcstatic gvvariationadaptorClearPopulations *****************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal Genetic Variation Population cache. This function
** deletes the unsigned identifier key data and the Ensembl Genetic Variation
** Population value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Genetic Variation Population
**                            value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationadaptorClearPopulations(void **key,
                                               void **value,
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

    ensGvpopulationDel((EnsPGvpopulation *) value);

    return;
}




/* @funcstatic gvvariationadaptorClearSynonyms ********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal synonym cache. This function deletes the
** AJAX String key data and the unsigned integer value data.
**
** @param [u] key [void**] AJAX String key data address
** @param [u] value [void**] AJAX unsigned integer value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationadaptorClearSynonyms(void **key,
                                            void **value,
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

    AJFREE(*value);

    ensGvsynonymDel((EnsPGvsynonym *) value);

    return;
}




/* @funcstatic gvvariationadaptorFetchAllBySQL ********************************
**
** Fetch all Ensembl Genetic Variation Variation objects via an SQL statement.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::_objs_from_sth
** @param [r] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation
**                           Variation objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvvariationadaptorFetchAllBySQL(EnsPGvvariationadaptor gvva,
                                              const AjPStr statement,
                                              AjPList gvvs)
{
    float gvafrequency = 0;

    ajuint gvvidentifier     = 0;
    ajuint gvvidentifierold  = 0;
    ajuint gvasubidentifier  = 0;
    ajuint gvvsourceid       = 0;
    ajuint gvaidentifier     = 0;
    ajuint gvaidentifierold  = 0;
    ajuint gvagvsampleid     = 0;
    ajuint gvvsidentifier    = 0;
    ajuint gvvssubidentifier = 0;
    ajuint gvvssourceid      = 0;

    ajuint *Pidentifier = NULL;

    AjBool flank = AJFALSE;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr gvvname          = NULL;
    AjPStr gvvvalidation    = NULL;
    AjPStr gvvancestral     = NULL;
    AjPStr gvaallele        = NULL;
    AjPStr gvvsmoleculetype = NULL;
    AjPStr gvvsname         = NULL;
    AjPStr failed           = NULL;

    AjPTable gvalleles   = NULL;
    AjPTable populations = NULL;
    AjPTable synonyms    = NULL;

    EnsPGvallele        gva  = NULL;
    EnsPGvalleleadaptor gvaa = NULL;

    EnsPGvpopulation        gvp  = NULL;
    EnsPGvpopulationadaptor gvpa = NULL;

    EnsPGvvariation gvv = NULL;

    EnsPGvsource        gvsource  = NULL;
    EnsPGvsourceadaptor gvsourcea = NULL;

    EnsPGvsynonym        gvsynonym  = NULL;
    EnsPGvsynonymadaptor gvsynonyma = NULL;

    if(!gvva)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    gvalleles = MENSTABLEUINTNEW(0);

    populations = MENSTABLEUINTNEW(0);

    synonyms = MENSTABLEUINTNEW(0);

    gvpa = ensRegistryGetGvpopulationadaptor(gvva);

    gvsourcea = ensRegistryGetGvsourceadaptor(gvva);

    sqls = ensDatabaseadaptorSqlstatementNew(gvva, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        gvvidentifier     = 0;
        gvvsourceid       = 0;
        gvvname           = ajStrNew();
        gvvvalidation     = ajStrNew();
        gvvancestral      = ajStrNew();
        gvaidentifier     = 0;
        gvasubidentifier  = 0;
        gvaallele         = ajStrNew();
        gvafrequency      = 0;
        gvagvsampleid     = 0;
        gvvsidentifier    = 0;
        gvvssubidentifier = 0;
        gvvsourceid       = 0;
        gvvsname          = ajStrNew();
        gvvsmoleculetype  = ajStrNew();
        failed            = ajStrNew();
        flank             = ajFalse;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &gvvidentifier);
        ajSqlcolumnToUint(sqlr, &gvvsourceid);
        ajSqlcolumnToStr(sqlr, &gvvname);
        ajSqlcolumnToStr(sqlr, &gvvvalidation);
        ajSqlcolumnToStr(sqlr, &gvvancestral);
        ajSqlcolumnToUint(sqlr, &gvaidentifier);
        ajSqlcolumnToUint(sqlr, &gvasubidentifier);
        ajSqlcolumnToStr(sqlr, &gvaallele);
        ajSqlcolumnToFloat(sqlr, &gvafrequency);
        ajSqlcolumnToUint(sqlr, &gvagvsampleid);
        ajSqlcolumnToUint(sqlr, &gvvsidentifier);
        ajSqlcolumnToUint(sqlr, &gvvssubidentifier);
        ajSqlcolumnToUint(sqlr, &gvvssourceid);
        ajSqlcolumnToStr(sqlr, &gvvsname);
        ajSqlcolumnToStr(sqlr, &gvvsmoleculetype);
        ajSqlcolumnToStr(sqlr, &failed);
        ajSqlcolumnToBool(sqlr, &flank);

        if(gvvidentifier != gvvidentifierold)
        {
            ensGvsourceadaptorFetchByIdentifier(gvsourcea,
                                                gvvsourceid,
                                                &gvsource);

            gvv = ensGvvariationNew(gvva,
                                    gvvidentifier,
                                    gvsource,
                                    gvvname,
                                    gvvancestral,
                                    (AjPList) NULL, /* synonyms */
                                    (AjPList) NULL, /* alleles */
                                    gvvvalidation,
                                    gvvsmoleculetype,
                                    (AjPStr) NULL, /* fiveflank */
                                    (AjPStr) NULL, /* threeflank */
                                    failed,
                                    flank);

            ajListPushAppend(gvvs, (void *) gvv);

            ensGvsourceDel(&gvsource);

            gvvidentifierold = gvvidentifier;
        }

        if(gvaidentifier != gvaidentifierold)
        {
            /* Fetch the Ensembl Genetic Variation Population. */

            if(gvagvsampleid)
            {
                gvp = (EnsPGvpopulation)
                    ajTableFetch(populations, (void *) &gvagvsampleid);

                if(!gvp)
                {
                    ensGvpopulationadaptorFetchByIdentifier(gvpa,
                                                            gvagvsampleid,
                                                            &gvp);

                    if(gvp)
                    {
                        AJNEW0(Pidentifier);

                        *Pidentifier = ensGvpopulationGetIdentifier(gvp);

                        ajTablePut(populations,
                                   (void *) Pidentifier,
                                   (void *) gvp);
                    }
                }
            }
            else
                gvp = (EnsPGvpopulation) NULL;

            if(gvaidentifier)
            {
                /* FIXME: The gvalleles table is not used. */

                gva = ensGvalleleNew(gvaa,
                                     gvaidentifier,
                                     gvp,
                                     gvaallele,
                                     gvafrequency,
                                     gvasubidentifier);

                ensGvvariationAddGvallele(gvv, gva);

                ensGvalleleDel(&gva);

                gvaidentifierold = gvaidentifier;
            }
            else
                gva = (EnsPGvallele) NULL;
        }

        if(gvvsidentifier)
        {
            gvsynonym = ajTableFetch(synonyms, (const void *) &gvvsidentifier);

            if(!gvsynonym)
            {
                ensGvsourceadaptorFetchByIdentifier(gvsourcea,
                                                    gvvssourceid,
                                                    &gvsource);

                gvsynonym = ensGvsynonymNew(gvsynonyma,
                                            gvvsidentifier,
                                            gvsource,
                                            gvvsname,
                                            gvvsmoleculetype,
                                            gvvidentifier,
                                            gvvssubidentifier);

                if(gvsynonym)
                {
                    AJNEW0(Pidentifier);

                    *Pidentifier = gvvsidentifier;

                    ajTablePut(synonyms,
                               (void *) Pidentifier,
                               (void *) gvsynonym);
                }

                ensGvsourceDel(&gvsource);
            }

            ensGvvariationAddGvsynonym(gvv, gvsynonym);
        }

        ajStrDel(&gvvname);
        ajStrDel(&gvvvalidation);
        ajStrDel(&gvvancestral);
        ajStrDel(&gvaallele);
        ajStrDel(&gvvsmoleculetype);
        ajStrDel(&gvvsname);
        ajStrDel(&failed);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(gvva, &sqls);

    /* Clear the AJAX Table of Ensembl Genetic Variation Populations. */

    ajTableMapDel(populations, gvvariationadaptorClearPopulations, NULL);

    ajTableFree(&populations);

    /* Clear the AJAX Table of Ensembl Genetic Variation Alleles. */

    ajTableMapDel(gvalleles, gvvariationadaptorClearAlleles, NULL);

    ajTableFree(&gvalleles);

    /* Clear the AJAX Table of Synonyms. */

    ajTableMapDel(synonyms, gvvariationadaptorClearSynonyms, NULL);

    ajTableFree(&synonyms);

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchByIdentifier *******************************
**
** Fetch an Ensembl Genetic Variation Variation via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_by_dbID
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pgvv [EnsPGvvariation*] Ensembl Genetic Variation
**                                     Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchByIdentifier(EnsPGvvariationadaptor gvva,
                                              ajuint identifier,
                                              EnsPGvvariation *Pgvv)
{
    AjPList variations = NULL;

    AjPStr statement = NULL;

    EnsPGvvariation gvv = NULL;

    if(!gvva)
        return ajFalse;

    if(!identifier)
        return ajFalse;

    if(!Pgvv)
        return ajFalse;

    /*
    ** The flanking sequence table is joined here and in
    ** ensGvvariationadaptorFetchByName so that the flank_flag can be
    ** set where the flanking sequence differs from the reference. It is just
    ** set to 0 when fetching variations by other methods since otherwise the
    ** join takes too long.
    */

    statement = ajFmtStr(
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.variation_synonym_id, "
        "variation_synonym.subsnp_id, "
        "variation_synonym.source_id, "
        "variation_synonym.name, "
        "variation_synonym.moltype, "
        "failed_description.description, "
        "("
        "flanking_sequence.up_seq IS NOT NULL "
        "OR "
        "flanking_sequence.down_seq IS NOT NULL"
        ") "
        "FROM "
        "(variation, flanking_sequence) "
        "LEFT JOIN "
        "allele "
        "ON "
        "variation.variation_id = allele.variation_id "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = "
        "variation_synonym.variation_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = "
        "failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.variation_id = flanking_sequence.variation_id "
        "AND "
        "variation.variation_id = %u",
        identifier);

    variations = ajListNew();

    gvvariationadaptorFetchAllBySQL(gvva, statement, variations);

    if(ajListGetLength(variations) > 1)
        ajWarn("ensGvvariationadaptorFetchByIdentifier go more than one "
               "Ensembl Genetic Variation Variation for identifier %u.",
               identifier);

    ajListPop(variations, (void **) Pgvv);

    while(ajListPop(variations, (void *) &gvv))
        ensGvvariationDel(&gvv);

    ajListFree(&variations);

    ajStrDel(&statement);

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchByName *************************************
**
** Fetch an Ensembl Genetic Variation Variation via its name.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_by_name
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] name [const AjPStr] Name
** @param [rN] source [const AjPStr] Source
** @param [wP] Pgvv [EnsPGvvariation*] Ensembl Genetic Variation
**                                     Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchByName(EnsPGvvariationadaptor gvva,
                                        const AjPStr name,
                                        const AjPStr source,
                                        EnsPGvvariation *Pgvv)
{
    char *txtname   = NULL;
    char *txtsource = NULL;

    AjPList gvvs = NULL;

    AjPStr extra     = NULL;
    AjPStr statement = NULL;

    EnsPGvvariation gvv = NULL;

    if(!gvva)
        return ajFalse;

    if(!name)
        return ajFalse;

    if(!Pgvv)
        return ajFalse;

    /*
    ** The flanking sequence table is joined here and in
    ** ensGvvariationadaptorFetchByIdentifier so that the flank_flag can be
    ** set where the flanking sequence differs from the reference. It is just
    ** set to 0 when fetching variations by other methods since otherwise the
    ** join takes too long.
    */

    ensDatabaseadaptorEscapeC(gvva, &txtname, name);

    if(source && ajStrGetLen(source))
    {
        ensDatabaseadaptorEscapeC(gvva, &txtsource, source);

        extra = ajFmtStr("AND source1.name = '%s' ", txtsource);

        ajCharDel(&txtsource);
    }
    else
        extra = ajStrNew();

    statement = ajFmtStr(
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.variation_synonym_id, "
        "variation_synonym.subsnp_id, "
        "variation_synonym.source_id, "
        "variation_synonym.name, "
        "variation_synonym.moltype, "
        "failed_description.description, "
        "("
        "flanking_sequence.up_seq IS NOT NULL "
        "OR "
        "flanking_sequence.down_seq IS NOT NULL"
        ") "
        "FROM "
        "(variation, flanking_sequence) "
        "LEFT JOIN "
        "allele "
        "ON "
        "variation.variation_id = allele.variation_id "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = variation_synonym.variation_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.variation_id = flanking_sequence.variation_id "
        "AND "
        "variation.name = '%s' "
        "%S "
        "ORDER BY "
        "allele.allele_id",
        txtname,
        extra);

    gvvs = ajListNew();

    gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

    ajStrDel(&statement);

    if(ajListGetLength(gvvs) > 1)
        ajWarn("ensGvvariationadaptorFetchByIdentifier go more than one "
               "Ensembl Genetic Variation Variation for name '%S' and "
               "source '%S'.", name, source);

    if(ajListGetLength(gvvs) == 0)
    {
        statement = ajFmtStr(
            "SELECT "
            "variation.variation_id, "
            "variation.source_id, "
            "variation.name, "
            "variation.validation_status, "
            "variation.ancestral_allele, "
            "allele.allele_id, "
            "allele.subsnp_id, "
            "allele.allele, "
            "allele.frequency, "
            "allele.sample_id, "
            "variation_synonym2.variation_synonym_id, "
            "variation_synonym2.subsnp_id, "
            "variation_synonym2.source_id, "
            "variation_synonym2.name, "
            "variation_synonym2.moltype, "
            "NULL, "
            "failed_description.description "
            "("
            "flanking_sequence.up_seq IS NOT NULL "
            "OR "
            "flanking_sequence.down_seq IS NOT NULL"
            ") "
            "FROM "
            "("
            "variation, "
            "allele, "
            "variation_synonym variation_synonym1, "
            "variation_synonym variation_synonym2, "
            "flanking_sequence"
            ") "
            "WHERE "
            "variation.variation_id = allele.variation_id "
            "AND "
            "variation.variation_id = "
            "variation_synonym1.variation_id "
            "AND "
            "variation.variation_id = "
            "variation_synonym2.variation_id "
            "AND "
            "variation.variation_id = flanking_sequence.variation_id "
            "AND "
            "variation_synonym1.name = '%s' "
            "%S "
            "ORDER BY "
            "allele.allele_id",
            txtname,
            extra);

        gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

        ajStrDel(&statement);
    }

    if(ajListGetLength(gvvs) > 1)
        ajWarn("ensGvvariationadaptorFetchByIdentifier go more than one "
               "Ensembl Genetic Variation Variation for name '%S' and "
               "source '%S'.", name, source);

    ajListPop(gvvs, (void **) Pgvv);

    while(ajListPop(gvvs, (void *) &gvv))
        ensGvvariationDel(&gvv);

    ajListFree(&gvvs);

    ajCharDel(&txtname);

    ajStrDel(&extra);

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchAllBySource ********************************
**
** Fetch all Ensembl Genetic Variation Variations via a source.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_all_by_source
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] source [const AjPStr] Source
** @param [r] primary [AjBool] Primary
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchAllBySource(EnsPGvvariationadaptor gvva,
                                             const AjPStr source,
                                             AjBool primary,
                                             AjPList gvvs)
{
    char *txtsource = NULL;

    AjPStr statement = NULL;

    if(!gvva)
        return ajFalse;

    if(!source)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    ensDatabaseadaptorEscapeC(gvva, &txtsource, source);

    statement = ajFmtStr(
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.variation_synonym_id, "
        "variation_synonym.subsnp_id, "
        "variation_synonym.source_id, "
        "variation_synonym.name, "
        "variation_synonym.moltype, "
        "failed_description.description, "
        "0 "
        "FROM "
        "(variation, source) "
        "LEFT JOIN "
        "allele "
        "ON "
        "variation.variation_id = "
        "allele.variation_id "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = "
        "variation_synonym.variation_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = "
        "failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.source_id = "
        "source.source_id "
        "AND "
        "source.name = '%s'",
        txtsource);

    gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

    ajStrDel(&statement);

    if(!primary)
    {
        /*
        ** We need to include variation_synonym as well,
        ** where the variation was merged with NCBI dbSNP.
        */

        statement = ajFmtStr(
            "SELECT "
            "variation.variation_id, "
            "variation.source_id, "
            "variation.name, "
            "variation.validation_status, "
            "variation.ancestral_allele, "
            "allele.allele_id, "
            "allele.subsnp_id, "
            "allele.allele, "
            "allele.frequency, "
            "allele.sample_id, "
            "variation_synonym1.variation_synonym_id, "
            "variation_synonym1.subsnp_id, "
            "variation_synonym1.source_id, "
            "variation_synonym1.name, "
            "variation_synonym1.moltype, "
            "source2.name, "
            "NULL, "
            "0 "
            "FROM "
            "("
            "variation, "
            "source, "
            "variation_synonym variation_synonym1"
            ") "
            "LEFT JOIN "
            "allele "
            "ON "
            "variation.variation_id = "
            "allele.variation_id "
            "WHERE "
            "variation.variation_id = "
            "variation_synonym1.variation_id "
            "AND "
            "variation_synonym1.source_id = "
            "source.source_id "
            "AND "
            "source.name = '%s' "
            "ORDER BY "
            "variation.variation_id",
            txtsource);

        /* need to merge both lists, trying to avoid duplicates. */

        gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

        ajStrDel(&statement);
    }

    ajCharDel(&txtsource);

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchAllByGvpopulation **************************
**
** Fetch all Ensembl Genetic Variation Variations via an
** Ensembl Genetic Variation Population.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::
**     fetch_all_by_Population
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchAllByGvpopulation(EnsPGvvariationadaptor gvva,
                                                   const EnsPGvpopulation gvp,
                                                   AjPList gvvs)
{
    AjPStr statement = NULL;

    if(!gvva)
        return ajFalse;

    if(!gvp)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    statement = ajFmtStr(
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.variation_synonym_id, "
        "variation_synonym.subsnp_id, "
        "variation_synonym.source_id, "
        "variation_synonym.name, "
        "variation_synonym.moltype, "
        "failed_description.description, "
        "0 "
        "FROM "
        "(variation, source, allele) "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = "
        "variation_synonym.variation_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = "
        "failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.variation_id = allele.variation_id "
        "AND "
        "allele.smaple_id = %u",
        ensGvpopulationGetIdentifier(gvp));

    gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

    ajStrDel(&statement);

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchAllByGvpopulationFrequency *****************
**
** Fetch all Ensembl Genetic Variation Variations via an
** Ensembl Genetic Variation Population and frequency information.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::
**     fetch_all_by_Population
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [r] frequency [float] Lower minor allele frequency (MAF) threshold
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchAllByGvpopulationFrequency(
    EnsPGvvariationadaptor gvva,
    const EnsPGvpopulation gvp,
    float frequency,
    AjPList gvvs)
{
    AjPStr statement = NULL;

    if(!gvva)
        return ajFalse;

    if(!gvp)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    /* Adjust the frequency if given a percentage. */

    if(frequency > 1)
        frequency /= 100.0;

    statement = ajFmtStr(
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.variation_synonym_id, "
        "variation_synonym.subsnp_id, "
        "variation_synonym.source_id, "
        "variation_synonym.name, "
        "variation_synonym.moltype, "
        "failed_description.description, "
        "0 "
        "FROM "
        "(variation, allele) "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = "
        "variation_synonym.variation_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = "
        "failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.variation_id = allele.variation_id "
        "AND "
        "allele.sample_id = %u "
        "AND "
        "("
        "IF("
        "allele.frequency > 0.5, "
        "1 - allele.frequency, "
        "allele.frequency"
        ") > %f"
        ")",
        ensGvpopulationGetIdentifier(gvp),
        frequency);

    gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

    ajStrDel(&statement);

    return ajTrue;
}




/* @funcstatic gvvariationlistCompareIdentifier *******************************
**
** AJAX List comparison function to sort Ensembl Genetic Variation Variation
** objects by identifier in ascending order.
**
** @param [r] P1 [const void*] Ensembl Genetic Variation Variation 1
** @param [r] P2 [const void*] Ensembl Genetic Variation Variation 2
** @see ajListSort
** @see ajListSortUnique
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int gvvariationlistCompareIdentifier(const void *P1, const void *P2)
{
    int value = 0;

    const EnsPGvvariation gvv1 = NULL;
    const EnsPGvvariation gvv2 = NULL;

    gvv1 = *(EnsPGvvariation const *) P1;
    gvv2 = *(EnsPGvvariation const *) P2;

    if(ajDebugTest("gvvariationlistCompare"))
        ajDebug("gvvariationlistCompare\n"
                "  gvv1 %p\n"
                "  gvv2 %p\n",
                gvv1,
                gvv2);

    if(gvv1->Identifier < gvv2->Identifier)
        value = -1;

    if(gvv1->Identifier > gvv2->Identifier)
        value = +1;

    return value;
}




/* @funcstatic gvvariationlistDelete ******************************************
**
** ajListSortUnique nodedelete function to delete Ensembl Genetic Variation
** Variation objects that are redundant.
**
** @param [r] PP1 [void**] Ensembl Genetic Variation Variation address 1
** @param [r] cl [void*] Standard, passed in from ajListSortUnique
** @see ajListSortUnique
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationlistDelete(void **PP1, void *cl)
{
    if(!PP1)
        return;

    (void) cl;

    ensGvvariationDel((EnsPGvvariation *) PP1);

    return;
}




/* @func ensGvvariationadaptorFetchAllByGvvariationset ************************
**
** Fetch all Ensembl Genetic Variation Variations via an
** Ensembl Genetic Variation Variation Set.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::
**     fetch_all_by_VariationSet
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] gvvset [const EnsPGvvariationset] Ensembl Genetic Variation
**                                              Variation Set
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchAllByGvvariationset(
    EnsPGvvariationadaptor gvva,
    EnsPGvvariationset gvvset,
    AjPList gvvs)
{
    AjPList variationsets = NULL;

    AjPStr statement = NULL;

    EnsPGvvariationset variationset = NULL;

    if(!gvva)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    if(!gvvset->Adaptor)
        return ajFalse;

    if(!gvvset->Identifier)
        return ajFalse;

    variationsets = ajListNew();

    /*
    ** First, get all immediate subsets of the specified Ensembl Genetic
    ** Variation Variation Set and get their Ensembl Genetic Variation
    ** Variations.
    */

    ensGvvariationsetFetchAllSubSets(gvvset, ajTrue, variationsets);

    while(ajListPop(variationsets, (void **) &variationset))
    {
        ensGvvariationadaptorFetchAllByGvvariationset(gvva,
                                                      variationset,
                                                      gvvs);

        ensGvvariationsetDel(&variationset);
    }

    ajListFree(&variationsets);

    /*
    ** Then get all Ensembl Genetic Variation Variations belonging to this
    ** Ensembl Genetic Variation Variation Set.
    */

    statement = ajFmtStr(
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.variation_synonym_id, "
        "variation_synonym.subsnp_id, "
        "variation_synonym.source_id, "
        "variation_synonym.name, "
        "variation_synonym.moltype, "
        "variation_synonym.moltype, "
        "failed_description.description, "
        "0 "
        "FROM "
        "(variation, allele, variation_set_variation) "
        "LEFT JOIN "
        "allele "
        "ON "
        "variation.variation_id = allele.variation_id "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = variation_synonym.variation_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = "
        "failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.variation_id = variation_set_variation.variation_id ",
        "AND "
        "variation_set_variation.variation_set_id = %u",
        ensGvvariationsetGetIdentifier(gvvset));

    gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

    ajStrDel(&statement);

    ajListSortUnique(gvvs,
                     gvvariationlistCompareIdentifier,
                     gvvariationlistDelete);

    return ajTrue;
}




/* @funcstatic gvvariationadaptorFetchFlankFromCore ***************************
**
** Fetch the flanking sequence from the corresponding Ensembl Core Database.
**
** The caller is responsible for deleting the AJAX String.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::_get_flank_from_core
** @param [r] gvva [const EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] srid [ajuint] Ensembl Sequence Region identifier
** @param [r] srstart [ajint] Ensembl Sequence Region start
** @param [r] srend [ajint] Ensembl Sequence Region end
** @param [r] srstrand [ajint] Ensembl Sequence Region strand
** @param [wP] Psequence [AjPStr*] Flanking sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvvariationadaptorFetchFlankFromCore(EnsPGvvariationadaptor gvva,
                                                   ajuint srid,
                                                   ajint srstart,
                                                   ajint srend,
                                                   ajint srstrand,
                                                   AjPStr *Psequence)
{
    EnsPDatabaseadaptor rsa = NULL;

    EnsPSlice slice      = NULL;
    EnsPSliceadaptor sla = NULL;

    if(!gvva)
        return ajFalse;

    if(!srid)
        return ajFalse;

    if(!Psequence)
        return ajFalse;

    rsa = ensRegistryGetReferenceadaptor(gvva);

    sla = ensRegistryGetSliceadaptor(rsa);

    if(sla)
    {
        ensSliceadaptorFetchBySeqregionIdentifier(sla, srid, 0, 0, 0, &slice);

        if(!slice)
            ajWarn("gvvariationadaptorFetchFlankFromCore could not get an "
                   "Ensembl Slice for Ensembl Sequence region identifier %u.",
                   srid);

        ensSliceFetchSubSequenceStr(slice,
                                    srstart,
                                    srend,
                                    srstrand,
                                    Psequence);
    }

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchFlankingSequence ***************************
**
** Fetch the flanking sequence of an Ensembl Variation.
**
** If the flanking sequence is not in the flankinq_sequence table of the
** Ensembl Genetic Variation database the sequence is retrieved from the
** corresponding Ensembl Core database.
**
** The caller is responsible for deleting the AJAX Strings.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::get_flanking_sequence
** @param [r] gvva [const EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] gvvidentifier [ajuint] Ensembl Genetic Variation
**                                 Variation identifier
** @param [wP] Pfiveseq [AjPStr*] 5' flanking sequence
** @param [wP] Pthreeseq [AjPStr*] 3' flanking sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchFlankingSequence(EnsPGvvariationadaptor gvva,
                                                  ajuint gvvidentifier,
                                                  AjPStr *Pfiveseq,
                                                  AjPStr *Pthreeseq)
{
    ajint srstrand = 0;

    ajuint srid     = 0;
    ajuint usrstart = 0;
    ajuint usrend   = 0;
    ajuint dsrstart = 0;
    ajuint dsrend   = 0;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr statement = NULL;
    AjPStr useq      = NULL;
    AjPStr dseq      = NULL;

    if(!gvva)
        return ajFalse;

    if(!gvvidentifier)
        return ajFalse;

    if(!Pfiveseq)
        return ajFalse;

    if(!Pthreeseq)
        return ajFalse;

    statement = ajFmtStr(
        "SELECT "
        "flanking_sequence.seq_region_id, "
        "flanking_sequence.seq_region_strand, "
        "flanking_sequence.up_seq, "
        "flanking_sequence.down_seq, "
        "flanking_sequence.up_seq_region_start, "
        "flanking_sequence.up_seq_region_end, "
        "flanking_sequence.down_seq_region_start, "
        "flanking_sequence.down_seq_region_end "
        "FROM "
        "flanking_sequence "
        "WHERE "
        "flanking_sequence.variation_id = %u",
        gvvidentifier);

    sqls = ensDatabaseadaptorSqlstatementNew(gvva, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        srid     = 0;
        srstrand = 0;
        useq     = ajStrNew();
        dseq     = ajStrNew();
        usrstart = 0;
        usrend   = 0;
        dsrstart = 0;
        dsrend   = 0;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &srid);
        ajSqlcolumnToInt(sqlr, &srstrand);
        ajSqlcolumnToStr(sqlr, &useq);
        ajSqlcolumnToStr(sqlr, &dseq);
        ajSqlcolumnToUint(sqlr, &usrstart);
        ajSqlcolumnToUint(sqlr, &usrend);
        ajSqlcolumnToUint(sqlr, &dsrstart);
        ajSqlcolumnToUint(sqlr, &dsrend);

        if(!ajStrGetLen(useq))
        {
            if(srid)
                gvvariationadaptorFetchFlankFromCore(gvva,
                                                     srid,
                                                     usrstart,
                                                     usrend,
                                                     srstrand,
                                                     &useq);
            else
                ajWarn("ensGvvariationadaptorFetchFlankingSequence "
                       "could not get "
                       "Ensembl Sequence Region identifier for "
                       "Ensembl Genetic Variation Variation %u.",
                       gvvidentifier);
        }

        ajStrAssignS(Pfiveseq, useq);

        if(!ajStrGetLen(dseq))
        {
            if(srid)
                gvvariationadaptorFetchFlankFromCore(gvva,
                                                     srid,
                                                     dsrstart,
                                                     dsrend,
                                                     srstrand,
                                                     &dseq);
            else
                ajWarn("ensGvvariationadaptorFetchFlankingSequence "
                       "could not get "
                       "Ensembl Sequence Region identifier for "
                       "Ensembl Genetic Variation Variation %u.",
                       gvvidentifier);
        }

        ajStrAssignS(Pthreeseq, dseq);

        ajStrDel(&useq);
        ajStrDel(&dseq);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(gvva, &sqls);

    ajStrDel(&statement);

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchAllByIdentifiers ***************************
**
** Fetch Ensembl Variations by an AJAX Table of AJAX unsigned integer
** key data and assign them as value data.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation value data before deleting the AJAX Table.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_all_by_dbID_list
** @param [r] gvva [const EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [u] gvvs [AjPTable] AJAX Table of AJAX unsigned integer identifier
**                            key data and Ensembl Genetic Variation
**                            Variation value data
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchAllByIdentifiers(EnsPGvvariationadaptor gvva,
                                                  AjPTable gvvs)
{
    void **keyarray = NULL;

    const char *template =
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.moltype, "
        "variation_synonym.name, "
        "source2.name, "
        "failed_description.description, "
        "0 "
        "FROM (variation, source source1) "
        "LEFT JOIN "
        "allele "
        "ON "
        "variation.variation_id = allele.variation_id "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = variation_synonym.variation_id "
        "LEFT JOIN "
        "source source2 "
        "ON "
        "variation_synonym.source_id = source2.source_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.source_id = source1.source_id "
        "AND "
        "variation.variation_id IN (%S)";

    register ajuint i = 0;

    ajuint *Pidentifier = NULL;

    AjPList lgvvs = NULL;

    AjPStr statement = NULL;
    AjPStr values    = NULL;

    EnsPGvvariation gvv = NULL;

    if(!gvva)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    lgvvs = ajListNew();

    values = ajStrNew();

    /*
    ** MySQL is faster and we ensure that we do not exceed the maximum
    ** query size by splitting large queries into smaller queries of
    ** up to 200 identifiers.
    */

    ajTableToarrayKeys(gvvs, &keyarray);

    for(i = 0; keyarray[i]; i++)
    {
        ajFmtPrintAppS(&values, "%u, ", *((ajuint *) keyarray[i]));

        /* Run the statement if the maximum chunk size is exceed. */

        if(!(i % EnsMBaseadaptorMaximumIdentifiers))
        {
            /* Remove the last comma and space. */

            ajStrCutEnd(&values, 2);

            statement = ajFmtStr(template, values);

            gvvariationadaptorFetchAllBySQL(gvva, statement, lgvvs);

            ajStrDel(&statement);

            ajStrAssignClear(&values);
        }
    }

    AJFREE(keyarray);

    /* Run the final statement, but remove the last comma and space first. */

    ajStrCutEnd(&values, 2);

    statement = ajFmtStr(template, values);

    gvvariationadaptorFetchAllBySQL(gvva, statement, lgvvs);

    ajStrDel(&statement);
    ajStrDel(&values);

    /*
    ** Move Ensembl Genetic Variation Variation objects from the AJAX List
    ** to the AJAX Table.
    */

    while(ajListPop(lgvvs, (void **) &gvv))
    {
        AJNEW0(Pidentifier);

        *Pidentifier = ensGvvariationGetIdentifier(gvv);

        if(ajTableFetch(gvvs, (const void *) Pidentifier))
            AJFREE(Pidentifier);
        else
            ajTablePut(gvvs,
                       (void *) Pidentifier,
                       (void *) ensGvvariationNewRef(gvv));

        ensGvvariationDel(&gvv);
    }

    return ajTrue;
}




/* @func ensGvvariationadaptorFetchAllByNames *********************************
**
** Fetch Ensembl Variations by an AJAX List of AJAX Sring names.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_all_by_name_list
** @param [r] gvva [const EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] names [const AjPList] AJAX List of AJAX String names
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchAllByNames(EnsPGvvariationadaptor gvva,
                                            const AjPList names,
                                            AjPList gvvs)
{
    const char *template =
        "SELECT "
        "variation.variation_id, "
        "variation.source_id, "
        "variation.name, "
        "variation.validation_status, "
        "variation.ancestral_allele, "
        "allele.allele_id, "
        "allele.subsnp_id, "
        "allele.allele, "
        "allele.frequency, "
        "allele.sample_id, "
        "variation_synonym.moltype, "
        "variation_synonym.name, "
        "source2.name, "
        "failed_description.description, "
        "0 "
        "FROM (variation, source source1) "
        "LEFT JOIN "
        "allele "
        "ON "
        "variation.variation_id = allele.variation_id "
        "LEFT JOIN "
        "variation_synonym "
        "ON "
        "variation.variation_id = variation_synonym.variation_id "
        "LEFT JOIN "
        "source source2 "
        "ON "
        "variation_synonym.source_id = source2.source_id "
        "LEFT JOIN "
        "failed_variation "
        "ON "
        "variation.variation_id = failed_variation.variation_id "
        "LEFT JOIN "
        "failed_description "
        "ON "
        "failed_variation.failed_description_id = "
        "failed_description.failed_description_id "
        "WHERE "
        "variation.source_id = source1.source_id "
        "AND "
        "variation.name IN (%S)";

    register ajuint i = 0;

    AjIList iter = NULL;

    AjPStr name      = NULL;
    AjPStr statement = NULL;
    AjPStr values    = NULL;

    if(!gvva)
        return ajFalse;

    if(!names)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    /*
    ** MySQL is faster and we ensure that we do not exceed the maximum
    ** query size by splitting large queries into smaller queries of
    ** up to 200 names.
    */

    iter = ajListIterNewread(names);

    while(!ajListIterDone(iter))
    {
        name = (AjPStr) ajListIterGet(iter);

        ajFmtPrintAppS(&values, "%S, ", name);

        i++;

        /*
        ** Run the statement if we exceed the maximum chunk size.
        ** Assume that average name strings are 4 times longer than
        ** integer strings.
        */

        if(!(i % (EnsMBaseadaptorMaximumIdentifiers / 4)))
        {
            /* Remove the last comma and space. */

            ajStrCutEnd(&values, 2);

            statement = ajFmtStr(template, values);

            gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

            ajStrDel(&statement);

            ajStrAssignClear(&values);
        }
    }

    ajListIterDel(&iter);

    /* Run the final statement. */

    /* Remove the last comma and space. */

    ajStrCutEnd(&values, 2);

    statement = ajFmtStr(template, values);

    gvvariationadaptorFetchAllBySQL(gvva, statement, gvvs);

    ajStrDel(&statement);
    ajStrDel(&values);

    return ajTrue;
}




/* @datasection [EnsPGvvariationfeature] Genetic Variation Variation Feature **
**
** Functions for manipulating Ensembl Genetic Variation Variation Feature
** objects
**
** @cc Bio::EnsEMBL::Variation::VariationFeature CVS Revision:
**
** @nam2rule Gvvariationfeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Variation Feature by
** pointer. It is the responsibility of the user to first destroy any previous
** Genetic Variation Variation Feature. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                              Variation Feature
** @argrule Ref object [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                              Variation Feature
**
** @valrule * [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                     Variation Feature
**
** @fcategory new
******************************************************************************/




/* @func ensGvvariationfeatureNew *********************************************
**
** Default constructor for an Ensembl Genetic Variation Variation Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvvfa [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::VariationFeature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @param [u] gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] allele [AjPStr] Allele
** @param [u] name [AjPStr] Name
** @param [u] validation [AjPStr] Validation code
** @param [u] consequence [AjPStr] Consequence type
** @param [r] mapweight [ajuint] Map weight
**
** @return [EnsPGvvariationfeature] Ensembl Genetic Variation Variation Feature
** @@
******************************************************************************/

EnsPGvvariationfeature ensGvvariationfeatureNew(
    EnsPGvvariationfeatureadaptor gvvfa,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGvsource gvsource,
    EnsPGvvariation gvv,
    AjPStr allele,
    AjPStr name,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight)
{
    EnsPGvvariationfeature gvvf = NULL;

    if(!feature)
        return NULL;

    if(!gvv)
        return NULL;

    AJNEW0(gvvf);

    gvvf->Use = 1;

    gvvf->Identifier = identifier;

    gvvf->Adaptor = gvvfa;

    gvvf->Feature = ensFeatureNewRef(feature);

    gvvf->Gvsource = ensGvsourceNewRef(gvsource);

    gvvf->Gvvariation = ensGvvariationNewRef(gvv);

    if(allele)
        gvvf->Allele = ajStrNewRef(allele);

    if(name)
        gvvf->Name = ajStrNewRef(name);

    if(validation)
        gvvf->ValidationCode = ajStrNewRef(validation);

    if(consequence)
        gvvf->ConsequenceType = ajStrNewRef(consequence);

    gvvf->MapWeight = mapweight;

    return gvvf;
}




/* @func ensGvvariationfeatureNewIdentifier ***********************************
**
** Constructor for an Ensembl Genetic Variation Variation Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvvfa [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::VariationFeature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @param [u] gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @param [u] gvvidentifier [ajuint] Ensembl Genetic Variation
**                                   Variation identifier
** @param [u] allele [AjPStr] Allele
** @param [u] name [AjPStr] Name
** @param [u] validation [AjPStr] Validation code
** @param [u] consequence [AjPStr] Consequence type
** @param [r] mapweight [ajuint] Map weight
**
** @return [EnsPGvvariationfeature] Ensembl Genetic Variation Variation Feature
** @@
******************************************************************************/

EnsPGvvariationfeature ensGvvariationfeatureNewIdentifier(
    EnsPGvvariationfeatureadaptor gvvfa,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGvsource gvsource,
    ajuint gvvidentifier,
    AjPStr allele,
    AjPStr name,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight)
{
    EnsPGvvariationfeature gvvf = NULL;

    if(!feature)
        return NULL;

    if(!gvvidentifier)
        return NULL;

    AJNEW0(gvvf);

    gvvf->Use = 1;

    gvvf->Identifier = identifier;

    gvvf->Adaptor = gvvfa;

    gvvf->Feature = ensFeatureNewRef(feature);

    gvvf->Gvsource = ensGvsourceNewRef(gvsource);

    gvvf->Gvvariationidentifier = gvvidentifier;

    if(allele)
        gvvf->Allele = ajStrNewRef(allele);

    if(name)
        gvvf->Name = ajStrNewRef(name);

    if(validation)
        gvvf->ValidationCode = ajStrNewRef(validation);

    if(consequence)
        gvvf->ConsequenceType = ajStrNewRef(consequence);

    gvvf->MapWeight = mapweight;

    return gvvf;
}




/* @func ensGvvariationfeatureNewObj ******************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                  Variation Feature
**
** @return [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                  Variation Feature or NULL
** @@
******************************************************************************/

EnsPGvvariationfeature ensGvvariationfeatureNewObj(
    const EnsPGvvariationfeature object)
{
    EnsPGvvariationfeature gvvf = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvvf);

    gvvf->Use = 1;

    gvvf->Identifier = object->Identifier;

    gvvf->Adaptor = object->Adaptor;

    gvvf->Feature = ensFeatureNewRef(object->Feature);

    gvvf->Gvsource = ensGvsourceNewRef(object->Gvsource);

    gvvf->Gvvariation = ensGvvariationNewRef(object->Gvvariation);

    if(object->Allele)
        gvvf->Allele = ajStrNewRef(object->Allele);

    if(object->Name)
        gvvf->Name = ajStrNewRef(object->Name);

    if(object->ValidationCode)
        gvvf->ValidationCode = ajStrNewRef(object->ValidationCode);

    if(object->ConsequenceType)
        gvvf->ConsequenceType = ajStrNewRef(object->ConsequenceType);

    gvvf->MapWeight = object->MapWeight;

    gvvf->Gvvariationidentifier = object->Gvvariationidentifier;

    return gvvf;
}




/* @func ensGvvariationfeatureNewRef ******************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
**
** @return [EnsPGvvariationfeature] Ensembl Genetic Variation Variation Feature
** @@
******************************************************************************/

EnsPGvvariationfeature ensGvvariationfeatureNewRef(EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    gvvf->Use++;

    return gvvf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Variation Feature.
**
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Variation Feature object
**
** @argrule * Pgvvf [EnsPGvvariationfeature*] Genetic Variation
**                                            Variation Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvariationfeatureDel *********************************************
**
** Default destructor for an Ensembl Genetic Variation Variation Feature.
**
** @param [d] Pgvvf [EnsPGvvariationfeature*] Ensembl Genetic Variation
**                                            Variation Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationfeatureDel(EnsPGvvariationfeature *Pgvvf)
{
    EnsPGvvariationfeature pthis = NULL;

    if(!Pgvvf)
        return;

    if(!*Pgvvf)
        return;

    if(ajDebugTest("ensGvvariationfeatureDel"))
    {
        ajDebug("ensGvvariationfeatureDel\n"
                "  *Pgvvf %p\n",
                *Pgvvf);

        ensGvvariationfeatureTrace(*Pgvvf, 1);
    }

    pthis = *Pgvvf;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvvf = NULL;

        return;
    }

    ensFeatureDel(&pthis->Feature);

    ensGvsourceDel(&pthis->Gvsource);

    ensGvvariationDel(&pthis->Gvvariation);

    ajStrDel(&pthis->Allele);
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->ValidationCode);
    ajStrDel(&pthis->ConsequenceType);

    AJFREE(pthis);

    *Pgvvf = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Variation Feature object.
**
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Variation Feature attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation
**                      Variation Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetGvsource Return the Ensemb Genetic Variation Source
** @nam4rule GetGvvariation Return the Ensembl Genetic Variation Variation
** @nam4rule GetAllele Return the allele
** @nam4rule GetName Return the name
** @nam4rule GetValidationCode Return the validation code
** @nam4rule GetConsequenceType Return the consequence type
** @nam4rule GetMapWeight Return the map weight
**
** @argrule * gvvf [const EnsPGvvariationfeature] Genetic Variation
**                                                Variation Feature
**
** @valrule Adaptor [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule Gvsource [EnsPGvsource] Ensembl  Genetic Variation Source
** @valrule Gvvariation [EnsPGvvariation] Ensembl Genetic Variation Variation
** @valrule Name [AjPStr] Name
** @valrule Allele [AjPStr] Allele
** @valrule ValidationCode [AjPStr] Validation code
** @valrule ConsequenceType [AjPStr] Consequence type
** @valrule MapWeight [ajuint] Map weight
**
** @fcategory use
******************************************************************************/




/* @func ensGvvariationfeatureGetAdaptor **************************************
**
** Get the Ensembl Genetic Variation Variation Feature Adaptor element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                         Variation Feature Adaptor
** @@
******************************************************************************/

EnsPGvvariationfeatureadaptor ensGvvariationfeatureGetAdaptor(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    return gvvf->Adaptor;
}




/* @func ensGvvariationfeatureGetIdentifier ***********************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvvariationfeatureGetIdentifier(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return 0;

    return gvvf->Identifier;
}




/* @func ensGvvariationfeatureGetFeature **************************************
**
** Get the Ensembl Feature element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensGvvariationfeatureGetFeature(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    return gvvf->Feature;
}




/* @func ensGvvariationfeatureGetGvsource *************************************
**
** Get the Ensembl Genetic Variation Source element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPGvsource] Ensembl Genetic Variation Source
** @@
******************************************************************************/

EnsPGvsource ensGvvariationfeatureGetGvsource(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    return gvvf->Gvsource;
}




/* @func ensGvvariationfeatureGetGvvariation **********************************
**
** Get the Ensembl Genetic Variation Variation element of an
** Ensembl Genetic Variation Variation Feature.
**
** This is not a simple accessor function, it will fetch the Ensembl Genetic
** Variation Variation from the Ensembl Variation database in case it is not
** defined.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationfeatureGetGvvariation(
    EnsPGvvariationfeature gvvf)
{
    EnsPDatabaseadaptor dba = NULL;

    EnsPGvvariationadaptor gvva = NULL;

    if(!gvvf)
        return NULL;

    if(gvvf->Gvvariation)
        return gvvf->Gvvariation;

    if(gvvf->Adaptor)
    {
        dba = ensFeatureadaptorGetDatabaseadaptor(gvvf->Adaptor);

        gvva = ensRegistryGetGvvariationadaptor(dba);

        ensGvvariationadaptorFetchByIdentifier(gvva,
                                               gvvf->Gvvariationidentifier,
                                               &gvvf->Gvvariation);
    }

    return gvvf->Gvvariation;
}




/* @func ensGvvariationfeatureGetAllele ***************************************
**
** Get the allele element of an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Allele
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetAllele(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    return gvvf->Allele;
}




/* @func ensGvvariationfeatureGetName *****************************************
**
** Get the name element of an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetName(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    return gvvf->Name;
}




/* @func ensGvvariationfeatureGetValidationCode *******************************
**
** Get the validation code element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Validation code
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetValidationCode(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    return gvvf->ValidationCode;
}




/* @func ensGvvariationfeatureGetConsequenceType ******************************
**
** Get the consequence type element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Consequence type
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetConsequenceType(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;

    return gvvf->ConsequenceType;
}




/* @func ensGvvariationfeatureGetMapWeight ************************************
**
** Get the map weight element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Map weight
** @@
******************************************************************************/

ajuint ensGvvariationfeatureGetMapWeight(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return 0;

    return gvvf->MapWeight;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Variation Feature object.
**
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Variation Feature
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation
**                      Variation Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetGvvariation Set the Ensembl Genetic Variation Variation
** @nam4rule SetName Set the name
** @nam4rule SetSource Set the source
** @nam4rule SetValidationCode Set the validation code
** @nam4rule SetConsequenceType Set the consequence type
** @nam4rule SetMapWeight Set the map weight
**
** @argrule * gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvvariationfeatureSetAdaptor **************************************
**
** Set the Ensembl Genetic Variation Variation Feature Adaptor element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] gvvfa [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetAdaptor(EnsPGvvariationfeature gvvf,
                                       EnsPGvvariationfeatureadaptor gvvfa)
{
    if(!gvvf)
        return ajFalse;

    gvvf->Adaptor = gvvfa;

    return ajTrue;
}




/* @func ensGvvariationfeatureSetIdentifier ***********************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetIdentifier(EnsPGvvariationfeature gvvf,
                                          ajuint identifier)
{
    if(!gvvf)
        return ajFalse;

    gvvf->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvvariationfeatureSetFeature **************************************
**
** Set the Ensembl Feature element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetFeature(EnsPGvvariationfeature gvvf,
                                       EnsPFeature feature)
{
    if(!gvvf)
        return ajFalse;

    ensFeatureDel(&gvvf->Feature);

    gvvf->Feature = ensFeatureNewRef(feature);

    return ajTrue;
}




/* @func ensGvvariationfeatureSetGvsource *************************************
**
** Set the Ensembl Genetic Variation Source element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] gvsource [EnsPGvsource] Ensembl Genetic Variation Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetGvsource(EnsPGvvariationfeature gvvf,
                                        EnsPGvsource gvsource)
{
    if(!gvvf)
        return ajFalse;

    ensGvsourceDel(&gvvf->Gvsource);

    gvvf->Gvsource = ensGvsourceNewRef(gvsource);

    return ajTrue;
}




/* @func ensGvvariationfeatureSetGvvariation **********************************
**
** Set the Ensembl Genetic Variation Variation element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetGvvariation(EnsPGvvariationfeature gvvf,
                                           EnsPGvvariation gvv)
{
    if(!gvvf)
        return ajFalse;

    ensGvvariationDel(&gvvf->Gvvariation);

    gvvf->Gvvariation = ensGvvariationNewRef(gvv);

    return ajTrue;
}




/* @func ensGvvariationfeatureSetAllele ***************************************
**
** Set the allele element of an Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] allele [AjPStr] Allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetAllele(EnsPGvvariationfeature gvvf,
                                      AjPStr allele)
{
    if(!gvvf)
        return ajFalse;

    ajStrDel(&gvvf->Allele);

    if(allele)
        gvvf->Allele = ajStrNewRef(allele);

    return ajTrue;
}




/* @func ensGvvariationfeatureSetName *****************************************
**
** Set the name element of an Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetName(EnsPGvvariationfeature gvvf,
                                    AjPStr name)
{
    if(!gvvf)
        return ajFalse;

    ajStrDel(&gvvf->Name);

    if(name)
        gvvf->Name = ajStrNewRef(name);

    return ajTrue;
}




/* @func ensGvvariationfeatureSetValidationCode *******************************
**
** Set the validation code element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] validationcode [AjPStr] Validation code
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetValidationCode(EnsPGvvariationfeature gvvf,
                                              AjPStr validationcode)
{
    if(!gvvf)
        return ajFalse;

    ajStrDel(&gvvf->ValidationCode);

    if(validationcode)
        gvvf->ValidationCode = ajStrNewRef(validationcode);

    return ajTrue;
}




/* @func ensGvvariationfeatureSetConsequenceType ******************************
**
** Set the consequence type element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] consequencetype [AjPStr] Consequence type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetConsequenceType(EnsPGvvariationfeature gvvf,
                                               AjPStr consequencetype)
{
    if(!gvvf)
        return ajFalse;

    ajStrDel(&gvvf->ConsequenceType);

    if(consequencetype)
        gvvf->ConsequenceType = ajStrNewRef(consequencetype);

    return ajTrue;
}




/* @func ensGvvariationfeatureSetMapWeight ************************************
**
** Set the map weight element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] mapweight [ajuint] Map weight
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetMapWeight(EnsPGvvariationfeature gvvf,
                                         ajuint mapweight)
{
    if(!gvvf)
        return ajFalse;

    gvvf->MapWeight = mapweight;

    return ajTrue;
}




/* @func ensGvvariationfeatureGetMemsize **************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvvariationfeatureGetMemsize(const EnsPGvvariationfeature gvvf)
{
    ajulong size = 0;

    if(!gvvf)
        return 0;

    size += sizeof (EnsOGvvariationfeature);

    size += ensFeatureGetMemsize(gvvf->Feature);

    size += ensGvsourceGetMemsize(gvvf->Gvsource);

    size += ensGvvariationGetMemsize(gvvf->Gvvariation);

    if(gvvf->Allele)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvvf->Allele);
    }

    if(gvvf->Name)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvvf->Name);
    }

    if(gvvf->ValidationCode)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvvf->ValidationCode);
    }

    if(gvvf->ConsequenceType)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvvf->ConsequenceType);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an
** Ensembl Genetic Variation Variation Feature object.
**
** @fdata [EnsPGvvariationfeature]
** @nam3rule Trace Report Ensembl Genetic Variation Variation Feature elements
**                 to debug file
**
** @argrule Trace gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                    Variation Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvvariationfeatureTrace *******************************************
**
** Trace an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureTrace(const EnsPGvvariationfeature gvvf,
                                  ajuint level)
{
    AjPStr indent = NULL;

    if(!gvvf)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvvariationfeatureTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Feature %p\n"
            "%S  Gvsource %p\n"
            "%S  Gvvariation %p\n"
            "%S  Allele '%S'\n"
            "%S  Name '%S'\n"
            "%S  ValidationCode '%S'\n"
            "%S  ConsequenceType '%S'\n"
            "%S  MapWeight %u\n",
            indent, gvvf,
            indent, gvvf->Use,
            indent, gvvf->Identifier,
            indent, gvvf->Adaptor,
            indent, gvvf->Feature,
            indent, gvvf->Gvsource,
            indent, gvvf->Gvvariation,
            indent, gvvf->Allele,
            indent, gvvf->Name,
            indent, gvvf->ValidationCode,
            indent, gvvf->ConsequenceType,
            indent, gvvf->MapWeight);

    ajStrDel(&indent);

    ensFeatureTrace(gvvf->Feature, level + 1);

    ensGvsourceTrace(gvvf->Gvsource, level + 1);

    ensGvvariationTrace(gvvf->Gvvariation, level + 1);

    return ajTrue;
}




/* @funcstatic gvvariationfeatureCompareStartAscending ************************
**
** Comparison function to sort Ensembl Genetic Variation Variation Feature by
** their Ensembl Feature start coordinate in ascending order.
**
** @param [r] P1 [const void*] Ensembl Genetic Variation
**                             Variation Feature address 1
** @param [r] P2 [const void*] Ensembl Genetic Variation
**                             Variation Feature address 2
** @see ajListSort
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int gvvariationfeatureCompareStartAscending(const void* P1,
                                                   const void* P2)
{
    const EnsPGvvariationfeature gvvf1 = NULL;
    const EnsPGvvariationfeature gvvf2 = NULL;

    gvvf1 = *(EnsPGvvariationfeature const *) P1;
    gvvf2 = *(EnsPGvvariationfeature const *) P2;

    if(ajDebugTest("gvvariationfeatureCompareStartAscending"))
        ajDebug("gvvariationfeatureCompareStartAscending\n"
                "  gvvf1 %p\n"
                "  gvvf2 %p\n",
                gvvf1,
                gvvf2);

    /* Sort empty values towards the end of the AJAX List. */

    if(gvvf1 && (!gvvf2))
        return -1;

    if((!gvvf1) && (!gvvf2))
        return 0;

    if((!gvvf1) && gvvf2)
        return +1;

    return ensFeatureCompareStartAscending(gvvf1->Feature, gvvf2->Feature);
}




/* @func ensGvvariationfeatureSortByStartAscending ****************************
**
** Sort Ensembl Genetic Variation Variation Feature by their
** Ensembl Feature start coordinate in ascending order.
**
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSortByStartAscending(AjPList gvvfs)
{
    if(!gvvfs)
        return ajFalse;

    ajListSort(gvvfs, gvvariationfeatureCompareStartAscending);

    return ajTrue;
}




/* @funcstatic gvvariationfeatureCompareStartDescending ***********************
**
** Comparison function to sort Ensembl Genetic Variation Variation Feature by
** their Ensembl Feature start coordinate in descending order.
**
** @param [r] P1 [const void*] Ensembl Genetic Variation
**                             Variation Feature address 1
** @param [r] P2 [const void*] Ensembl Genetic Variation
**                             Variation Feature address 2
** @see ajListSort
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int gvvariationfeatureCompareStartDescending(const void* P1,
                                                    const void* P2)
{
    const EnsPGvvariationfeature gvvf1 = NULL;
    const EnsPGvvariationfeature gvvf2 = NULL;

    gvvf1 = *(EnsPGvvariationfeature const *) P1;
    gvvf2 = *(EnsPGvvariationfeature const *) P2;

    if(ajDebugTest("gvvariationfeatureCompareStartDescending"))
        ajDebug("gvvariationfeatureCompareStartDescending\n"
                "  gvvf1 %p\n"
                "  gvvf2 %p\n",
                gvvf1,
                gvvf2);

    /* Sort empty values towards the end of the AJAX List. */

    if(gvvf1 && (!gvvf2))
        return -1;

    if((!gvvf1) && (!gvvf2))
        return 0;

    if((!gvvf1) && gvvf2)
        return +1;

    return ensFeatureCompareStartDescending(gvvf1->Feature, gvvf2->Feature);
}




/* @func ensGvvariationfeatureSortByStartDescending ***************************
**
** Sort Ensembl Genetic Variation Variation Feature by their
** Ensembl Feature start coordinate in descending order.
**
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSortByStartDescending(AjPList gvvfs)
{
    if(!gvvfs)
        return ajFalse;

    ajListSort(gvvfs, gvvariationfeatureCompareStartDescending);

    return ajTrue;
}




/*
** TODO: The following methods are not implented:
**   * get_all_TranscriptVariations
**   * add_TranscriptVariation
**   * display_consequence
**   * add_consequence_type
*/




/* @datasection [EnsPGvvariationfeatureadaptor] Genetic Variation Variation
**                                              Feature Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Variation
** Feature Adaptor objects
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor
**     CVS Revision: 1.35
**
** @nam2rule Gvvariationfeatureadaptor
**
** NOTE: Because of the small number of entries, source information has been
** consolidated into EnsPGvsource and EnsPGvsourceadaptor objects.
** The EnsPGvsourceadaptor caches all EnsPGvsource entries, which should help
** reduce memory requirements.
******************************************************************************/

static const char *gvvariationfeatureadaptorTables[] =
{
    "variation_feature",
    "source",
    NULL
};

static const char *gvvariationfeatureadaptorColumns[] =
{
    "variation_feature.variation_feature_id",
    "variation_feature.seq_region_id",
    "variation_feature.seq_region_start",
    "variation_feature.seq_region_end",
    "variation_feature.seq_region_strand",
    "variation_feature.variation_id",
    "variation_feature.allele_string",
    "variation_feature.variation_name",
    "variation_feature.map_weight",
    "variation_feature.source_id",
    "variation_feature.validation_status",
    "variation_feature.consequence_type",
    NULL
};

static EnsOBaseadaptorLeftJoin gvvariationfeatureadaptorLeftJoin[] =
{
    {NULL, NULL}
};

static const char *gvvariationfeatureadaptorDefaultCondition =
    "variation_feature.source_id = source.source_id";

static const char *gvvariationfeatureadaptorFinalCondition = NULL;




/* @funcstatic gvvariationfeatureadaptorFetchAllBySQL *************************
**
** Fetch all Ensembl Genetic Variation Feature objects via an SQL statement.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::_objs_from_sth
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Feature objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvvariationfeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                     const AjPStr statement,
                                                     EnsPAssemblymapper am,
                                                     EnsPSlice slice,
                                                     AjPList gvvfs)
{
    ajint mapweight = 0;

    ajuint identifier    = 0;
    ajuint gvvidentifier = 0;
    ajuint sourceid  = 0;

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

    AjPStr allele         = NULL;
    AjPStr name       = NULL;
    AjPStr validation = NULL;
    AjPStr consequence    = NULL;

    EnsPAssemblymapperadaptor ama = NULL;

    EnsPCoordsystemadaptor csa = NULL;

    EnsPGvsource        gvsource  = NULL;
    EnsPGvsourceadaptor gvsourcea = NULL;

    EnsPGvvariationfeature        gvvf  = NULL;
    EnsPGvvariationfeatureadaptor gvvfa = NULL;

    EnsPFeature feature = NULL;

    EnsPMapperresult mr = NULL;

    EnsPSlice srslice   = NULL;
    EnsPSliceadaptor sa = NULL;

    if(ajDebugTest("gvvariationfeatureadaptorFetchAllBySQL"))
        ajDebug("gvvariationfeatureadaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  gvvfs %p\n",
                dba,
                statement,
                am,
                slice,
                gvvfs);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    csa = ensRegistryGetCoordsystemadaptor(dba);

    gvvfa = ensRegistryGetGvvariationfeatureadaptor(dba);

    gvsourcea = ensRegistryGetGvsourceadaptor(dba);

    sa = ensRegistryGetSliceadaptor(dba);

    if(slice)
        ama = ensRegistryGetAssemblymapperadaptor(dba);

    mrs = ajListNew();

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier     = 0;
        srid           = 0;
        srstart        = 0;
        srend          = 0;
        srstrand       = 0;
        gvvidentifier  = 0;
        allele         = ajStrNew();
        name       = ajStrNew();
        mapweight      = 0;
        sourceid   = 0;
        validation = ajStrNew();
        consequence    = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &srid);
        ajSqlcolumnToUint(sqlr, &srstart);
        ajSqlcolumnToUint(sqlr, &srend);
        ajSqlcolumnToInt(sqlr, &srstrand);
        ajSqlcolumnToUint(sqlr, &gvvidentifier);
        ajSqlcolumnToStr(sqlr, &allele);
        ajSqlcolumnToStr(sqlr, &name);
        ajSqlcolumnToInt(sqlr, &mapweight);
        ajSqlcolumnToUint(sqlr, &sourceid);
        ajSqlcolumnToStr(sqlr, &validation);
        ajSqlcolumnToStr(sqlr, &consequence);

        /* Need to get the internal Ensembl Sequence Region identifier. */

        srid = ensCoordsystemadaptorGetInternalSeqregionIdentifier(csa, srid);

        /*
        ** Since the Ensembl SQL schema defines Sequence Region start and end
        ** coordinates as unsigned integers for all Features, the range needs
        ** checking.
        */

        if(srstart <= INT_MAX)
            slstart = (ajint) srstart;
        else
            ajFatal("exonadaptorFetchAllBySQL got a "
                    "Sequence Region start coordinate (%u) outside the "
                    "maximum integer limit (%d).",
                    srstart, INT_MAX);

        if(srend <= INT_MAX)
            slend = (ajint) srend;
        else
            ajFatal("exonadaptorFetchAllBySQL got a "
                    "Sequence Region end coordinate (%u) outside the "
                    "maximum integer limit (%d).",
                    srend, INT_MAX);

        slstrand = srstrand;

        /* Fetch a Slice spanning the entire Sequence Region. */

        ensSliceadaptorFetchBySeqregionIdentifier(sa, srid, 0, 0, 0, &srslice);

        /*
        ** Increase the reference counter of the Ensembl Assembly Mapper if
        ** one has been specified, otherwise fetch it from the database if a
        ** destination Slice has been specified.
        */

        if(am)
            am = ensAssemblymapperNewRef(am);
        else if(slice && (!ensCoordsystemMatch(
                              ensSliceGetCoordsystem(slice),
                              ensSliceGetCoordsystem(srslice))))
            am = ensAssemblymapperadaptorFetchBySlices(ama, slice, srslice);

        /*
        ** Remap the Feature coordinates to another Ensembl Coordinate System
        ** if an Ensembl Assembly Mapper is defined at this point.
        */

        if(am)
        {
            ensAssemblymapperFastMap(am,
                                     ensSliceGetSeqregion(srslice),
                                     slstart,
                                     slend,
                                     slstrand,
                                     mrs);

            /*
            ** The ensAssemblymapperFastMap function returns at best one
            ** Ensembl Mapper Result.
            */

            ajListPop(mrs, (void **) &mr);

            /*
            ** Skip Features that map to gaps or
            ** Coordinate System boundaries.
            */

            if(ensMapperresultGetType(mr) != ensEMapperresultTypeCoordinate)
            {
                /* Load the next Feature but destroy first! */

                ajStrDel(&allele);
                ajStrDel(&name);
                ajStrDel(&validation);
                ajStrDel(&consequence);

                ensSliceDel(&srslice);

                ensAssemblymapperDel(&am);

                ensMapperresultDel(&mr);

                continue;
            }

            srid = ensMapperresultGetObjectIdentifier(mr);

            slstart = ensMapperresultGetStart(mr);

            slend = ensMapperresultGetEnd(mr);

            slstrand = ensMapperresultGetStrand(mr);

            /*
            ** Delete the Sequence Region Slice and fetch a Slice in the
            ** Coordinate System we just mapped to.
            */

            ensSliceDel(&srslice);

            ensSliceadaptorFetchBySeqregionIdentifier(sa,
                                                      srid,
                                                      0,
                                                      0,
                                                      0,
                                                      &srslice);

            ensMapperresultDel(&mr);
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
                ajFatal("exonadaptorFetchAllBySQL got a Slice, "
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
               (srid != ensSliceGetSeqregionIdentifier(slice)))
            {
                /* Load the next Feature but destroy first! */

                ajStrDel(&allele);
                ajStrDel(&name);
                ajStrDel(&validation);
                ajStrDel(&consequence);

                ensSliceDel(&srslice);

                ensAssemblymapperDel(&am);

                continue;
            }

            /* Delete the Sequence Region Slice and set the requested Slice. */

            ensSliceDel(&srslice);

            srslice = ensSliceNewRef(slice);
        }

        /*
        ** Finally, create a new
        ** Ensembl Genetic Variation Variation Feature.
        */

        feature = ensFeatureNewS((EnsPAnalysis) NULL,
                                 srslice,
                                 slstart,
                                 slend,
                                 slstrand);

        ensGvsourceadaptorFetchByIdentifier(gvsourcea, sourceid, &gvsource);

        gvvf = ensGvvariationfeatureNewIdentifier(gvvfa,
                                                  identifier,
                                                  feature,
                                                  gvsource,
                                                  gvvidentifier,
                                                  allele,
                                                  name,
                                                  validation,
                                                  consequence,
                                                  mapweight);

        ajListPushAppend(gvvfs, (void *) gvvf);

        ensGvsourceDel(&gvsource);

        ensFeatureDel(&feature);

        ensAssemblymapperDel(&am);

        ensSliceDel(&srslice);

        ajStrDel(&allele);
        ajStrDel(&name);
        ajStrDel(&validation);
        ajStrDel(&consequence);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajListFree(&mrs);

    return ajTrue;
}




/* @funcstatic gvvariationfeatureadaptorCacheReference ************************
**
** Wrapper function to reference an
** Ensembl Genetic Variation Variation Feature from an Ensembl Cache.
**
** @param [r] value [void*] Ensembl Genetic Variation Variation Feature
**
** @return [void*] Ensembl Genetic Variation Variation Feature or NULL
** @@
******************************************************************************/

static void* gvvariationfeatureadaptorCacheReference(void *value)
{
    if(!value)
        return NULL;

    return (void *) ensGvvariationfeatureNewRef(
        (EnsPGvvariationfeature) value);
}




/* @funcstatic gvvariationfeatureadaptorCacheDelete ***************************
**
** Wrapper function to delete an
** Ensembl Genetic Variation Variation Feature from an Ensembl Cache.
**
** @param [r] value [void**] Ensembl Genetic Variation Variation Feature
**                           address
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationfeatureadaptorCacheDelete(void **value)
{
    if(!value)
        return;

    ensGvvariationfeatureDel((EnsPGvvariationfeature *) value);

    return;
}




/* @funcstatic gvvariationfeatureadaptorCacheSize *****************************
**
** Wrapper function to determine the memory size of an
** Ensembl Genetic Variation Variation Feature from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Genetic Variation Variation Feature
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

static ajulong gvvariationfeatureadaptorCacheSize(const void *value)
{
    if(!value)
        return 0;

    return ensGvvariationfeatureGetMemsize(
        (const EnsPGvvariationfeature) value);
}




/* @funcstatic gvvariationfeatureadaptorGetFeature ****************************
**
** Wrapper function to get the Ensembl Feature of an
** Ensembl Genetic Variation Variation Feature
** from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl Genetic Variation Variation Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature gvvariationfeatureadaptorGetFeature(const void *value)
{
    if(!value)
        return NULL;

    return ensGvvariationfeatureGetFeature(
        (const EnsPGvvariationfeature) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Variation Feature
** Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Variation Feature Adaptor.
** The target pointer does not need to be initialised to NULL,
** but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvvariationfeatureadaptor]
** @fnote None
**
** @nam3rule New Constructor
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @valrule * [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                            Variation Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensGvvariationfeatureadaptorNew **************************************
**
** Default Ensembl Genetic Variation Variation Feature Adaptor constructor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetGvvariationfeatureadaptor
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                         Variation Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPGvvariationfeatureadaptor ensGvvariationfeatureadaptorNew(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    return ensFeatureadaptorNew(
        dba,
        gvvariationfeatureadaptorTables,
        gvvariationfeatureadaptorColumns,
        gvvariationfeatureadaptorLeftJoin,
        gvvariationfeatureadaptorDefaultCondition,
        gvvariationfeatureadaptorFinalCondition,
        gvvariationfeatureadaptorFetchAllBySQL,
        (void * (*)(const void *key)) NULL,
        gvvariationfeatureadaptorCacheReference,
        (AjBool (*)(const void* value)) NULL,
        gvvariationfeatureadaptorCacheDelete,
        gvvariationfeatureadaptorCacheSize,
        gvvariationfeatureadaptorGetFeature,
        "Ensembl Genetic Variation Variation Feature");
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Variation Feature Adaptor.
**
** @fdata [EnsPGvvariationfeatureadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Genetic Variation
**               Variation Feature Adaptor object
**
** @argrule * Pgvvfa [EnsPGvvariationfeatureadaptor*] Ensembl Genetic
**                          Variation Variation Feature Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvariationfeatureadaptorDel **************************************
**
** Default destructor for an
** Ensembl Genetic Variation Variation Feature Adaptor.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Pgvvfa [EnsPGvvariationfeatureadaptor*] Ensembl Genetic
**                                 Variation Variation Feature Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationfeatureadaptorDel(EnsPGvvariationfeatureadaptor *Pgvvfa)
{
    EnsPGvvariationfeatureadaptor pthis = NULL;

    if(!Pgvvfa)
        return;

    if(!*Pgvvfa)
        return;

    pthis = *Pgvvfa;

    ensFeatureadaptorDel(&pthis);

    AJFREE(pthis);

    *Pgvvfa = NULL;

    return;
}




/* @func ensGvvariationfeatureAdaptorFetchAllBySliceConstraint ****************
**
** Fetch all Ensembl Genetic Variation Variation Features matching a
** SQL constraint on an Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation Features before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_by_Slice_constraint
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_somatic_by_Slice_constraint
** @param [r] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [rN] constraint [const AjPStr] SQL constraint
** @param [r] somatic [AjBool] Somatic (ajTrue) or germline (ajFalse)
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureAdaptorFetchAllBySliceConstraint(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPSlice slice,
    const AjPStr constraint,
    AjBool somatic,
    AjPList gvvfs)
{
    AjBool value = AJFALSE;

    AjPStr newconstraint = NULL;

    if(!gvvfa)
        return ajFalse;

    if(!slice)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    /* By default, filter out somatic mutations. */

    if(constraint && ajStrGetLen(constraint))
        newconstraint = ajFmtStr("%S AND source.somatic = %d",
                                 constraint,
                                 somatic);
    else
        newconstraint = ajFmtStr("source.somatic = %d",
                                 somatic);

    value = ensFeatureadaptorFetchAllBySliceConstraint(
        gvvfa,
        slice,
        newconstraint,
        (AjPStr) NULL,
        gvvfs);

    ajStrDel(&newconstraint);

    return value;
}




/* @func ensGvvariationfeatureAdaptorFetchAllBySlice **************************
**
** Fetch all Ensembl Genetic Variation Variation Features on an Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation Features before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_by_Slice
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_somatic_by_Slice
** @param [r] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [r] somatic [AjBool] Somatic (ajTrue) or germline (ajFalse)
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureAdaptorFetchAllBySlice(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPSlice slice,
    AjBool somatic,
    AjPList gvvfs)
{
    if(!gvvfa)
        return ajFalse;

    if(!slice)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    return ensGvvariationfeatureAdaptorFetchAllBySliceConstraint(
        gvvfa,
        slice,
        (AjPStr) NULL,
        somatic,
        gvvfs);
}




/* @func ensGvvariationfeatureadaptorFetchAllBySliceGenotyped *****************
**
** Fetch all genotyped Ensembl Genetic Variation Variation Features on an
** Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation Features before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_genotyped_by_Slice
** @param [r] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureadaptorFetchAllBySliceGenotyped(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPSlice slice,
    AjPList gvvfs)
{
    AjBool value = AJFALSE;

    AjPStr constraint = NULL;

    EnsPBaseadaptor ba = NULL;

    if(!gvvfa)
        return ajFalse;

    if(!slice)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    ba = ensFeatureadaptorGetBaseadaptor(gvvfa);

    constraint = ajStrNewC("variation_feature.flags & 1");

    value = ensGvvariationfeatureAdaptorFetchAllBySliceConstraint(
        gvvfa,
        slice,
        constraint,
        ajTrue,
        gvvfs);

    ajStrDel(&constraint);

    return value;
}




/* @func ensGvvariationfeatureadaptorFetchAllByGvvariation ********************
**
** Fetch all Ensembl Genetic Variation Variation Features by an
** Ensembl Genetic Variation Variation.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation Features before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_by_Variation
** @param [r] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureadaptorFetchAllByGvvariation(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPGvvariation gvv,
    AjPList gvvfs)
{
    AjBool value = AJFALSE;

    AjPStr constraint = NULL;

    EnsPBaseadaptor ba = NULL;

    if(!gvvfa)
        return ajFalse;

    if(!gvv)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    ba = ensFeatureadaptorGetBaseadaptor(gvvfa);

    constraint = ajFmtStr("variation_feature.variation_id = %u",
                          ensGvvariationGetIdentifier(gvv));

    value = ensBaseadaptorGenericFetch(ba,
                                       constraint,
                                       (EnsPAssemblymapper) NULL,
                                       (EnsPSlice) NULL,
                                       gvvfs);

    ajStrDel(&constraint);

    return value;
}




/* @func ensGvvariationfeatureadaptorFetchAllBySliceAnnotated *****************
**
** Fetch all Ensembl Genetic Variation Variation Features associated with
** annotations on an Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation Features before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     _internal_fetch_all_with_annotation_by_Slice
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_with_annotation_by_Slice
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_somatic_with_annotation_by_Slice
** @param [r] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [r] vsource [const AjPStr] variation_feature source
** @param [r] psource [const AjPStr] annotation_source
** @param [r] annotation [const AjPStr] annotation_name
** @param [r] somatic [AjBool] Somatic (ajTrue) or germline (ajFalse)
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureadaptorFetchAllBySliceAnnotated(
    EnsPGvvariationfeatureadaptor gvvfa,
    const AjPStr vsource,
    const AjPStr psource,
    const AjPStr annotation,
    AjBool somatic,
    EnsPSlice slice,
    AjPList gvvfs)
{
    const char ** txtcolumns = NULL;

    char *txtstring = NULL;

    register ajuint i = 0;

    AjPStr columns    = NULL;
    AjPStr constraint = NULL;
    AjPStr statement  = NULL;

    EnsPBaseadaptor ba = NULL;

    EnsPDatabaseadaptor dba = NULL;

    constraint = ajFmtStr("vsource.somatic = %d", somatic);

    if(vsource && ajStrGetLen(vsource))
    {
        ensFeatureadaptorEscapeC(gvvfa, &txtstring, vsource);

        ajFmtPrintAppS(&constraint, " AND vsource.name = '%s'", txtstring);

        ajCharDel(&txtstring);
    }

    if(psource && ajStrGetLen(psource))
    {
        ensFeatureadaptorEscapeC(gvvfa, &txtstring, psource);

        ajFmtPrintAppS(&constraint, " AND psource.name = '%s'", txtstring);

        ajCharDel(&txtstring);
    }

    if(annotation && ajStrGetLen(annotation))
    {
        ensFeatureadaptorEscapeC(gvvfa, &txtstring, annotation);

        if(ajStrIsNum(annotation))
            ajFmtPrintAppS(&constraint,
                           " AND phenotype.phenotype_id = %s",
                           txtstring);
        else
            ajFmtPrintAppS(&constraint,
                           " AND "
                           "("
                           "phenotype.name = '%s' "
                           "OR "
                           "phenotype.description LIKE '%%%s%%'"
                           ")",
                           txtstring,
                           txtstring);

        ajCharDel(&txtstring);
    }

    /* Build the column expression. */

    ba = ensFeatureadaptorGetBaseadaptor(gvvfa);

    txtcolumns = ensBaseadaptorGetColumns(ba);

    columns = ajStrNew();

    for(i = 0; txtcolumns[i]; i++)
        ajFmtPrintAppS(&columns, "%s, ", txtcolumns[i]);

    /* Remove last comma and space from the column expression. */

    ajStrCutEnd(&columns, 2);

    /* Build the final SQL statement. */

    dba = ensFeatureadaptorGetDatabaseadaptor(gvvfa);

    statement = ajFmtStr(
        "SELECT "
        "%S "
        "FROM "
        "variation_feature, "
        "variation_annotation, "
        "phenotype, "
        "source vsource, "
        "source psource "
        "WHERE "
        "variation_annotation.source_id = psource.source_id "
        "AND "
        "variation_feature.source_id = vsource.source_id "
        "AND "
        "variation_feature.variation_id = variation_annotation.variation_id "
        "AND "
        "variation_annotation.phenotype_id = phenotype.phenotype_id "
        "%S "
        "AND "
        "variation_feature.seq_region_id = %u "
        "AND "
        "variation_feature.seq_region_end > %d "
        "AND "
        "variation_feature.seq_region_start < %d "
        "GROUP BY "
        "variation_feature.variation_feature_id",
        columns,
        constraint,
        ensSliceGetSeqregionIdentifier(slice),
        ensSliceGetStart(slice),
        ensSliceGetEnd(slice));

    gvvariationfeatureadaptorFetchAllBySQL(dba,
                                           statement,
                                           (EnsPAssemblymapper) NULL,
                                           (EnsPSlice) NULL,
                                           gvvfs);

    ajStrDel(&columns);
    ajStrDel(&constraint);
    ajStrDel(&statement);

    return ajTrue;
}




/* @func ensGvvariationfeatureadaptorFetchAllByGvpopulation *******************
**
** Fetch all Ensembl Genetic Variation Variation Features associated with an
** Ensembl Genetic Variation Population on an Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation Features before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_by_Slice_Population
** @param [r] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureadaptorFetchAllByGvpopulation(
    EnsPGvvariationfeatureadaptor gvvfa,
    const EnsPGvpopulation gvp,
    EnsPSlice slice,
    AjPList gvvfs)
{
    const char ** txtcolumns = NULL;

    register ajuint i = 0;

    AjPStr columns   = NULL;
    AjPStr statement = NULL;

    EnsPBaseadaptor ba = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!gvvfa)
        return ajFalse;

    if(!gvp)
        return ajFalse;

    if(!slice)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    /* Build the column expression. */

    ba = ensFeatureadaptorGetBaseadaptor(gvvfa);

    txtcolumns = ensBaseadaptorGetColumns(ba);

    columns = ajStrNew();

    for(i = 0; txtcolumns[i]; i++)
        ajFmtPrintAppS(&columns, "%s, ", txtcolumns[i]);

    /* Remove last comma and space from the column expression. */

    ajStrCutEnd(&columns, 2);

    /* Build the final SQL statement. */

    dba = ensFeatureadaptorGetDatabaseadaptor(gvvfa);

    statement = ajFmtStr(
        "SELECT "
        "%S "
        "FROM "
        "variation_feature vf, "
        "source s, "
        "allele a "
        "WHERE "
        "variation_feature.source_id = s.source_id "
        "AND "
        "variation_feature.variation_id = a.variation_id "
        "AND "
        "a.sample_id = %u "
        "AND "
        "variation_feature.seq_region_id = %u "
        "AND "
        "variation_feature.seq_region_end > %d "
        "AND "
        "variation_feature.seq_region_start < %d "
        "GROUP BY "
        "allele.variation_id",
        columns,
        ensGvpopulationGetIdentifier(gvp),
        ensSliceGetSeqregionIdentifier(slice),
        ensSliceGetStart(slice),
        ensSliceGetEnd(slice));

    gvvariationfeatureadaptorFetchAllBySQL(dba,
                                           statement,
                                           (EnsPAssemblymapper) NULL,
                                           (EnsPSlice) NULL,
                                           gvvfs);

    ajStrDel(&columns);
    ajStrDel(&statement);

    return ajTrue;
}




/* @func ensGvvariationfeatureadaptorFetchAllByGvpopulationFrequency **********
**
** Fetch all Ensembl Genetic Variation Variation Features associated with an
** Ensembl Genetic Variation Population and minor allele frequency on an
** Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl Genetic Variation
** Variation Features before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::
**     fetch_all_by_Slice_Population
** @param [r] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [r] frequency [float] Minor allele frequency
** @param [u] slice [EnsPSlice] Ensembl Slice
** @param [u] gvvfs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureadaptorFetchAllByGvpopulationFrequency(
    EnsPGvvariationfeatureadaptor gvvfa,
    const EnsPGvpopulation gvp,
    float frequency,
    EnsPSlice slice,
    AjPList gvvfs)
{
    const char ** txtcolumns = NULL;

    register ajuint i = 0;

    AjPStr columns   = NULL;
    AjPStr statement = NULL;

    EnsPBaseadaptor ba = NULL;

    EnsPDatabaseadaptor dba = NULL;

    if(!gvvfa)
        return ajFalse;

    if(!gvp)
        return ajFalse;

    if(!slice)
        return ajFalse;

    if(!gvvfs)
        return ajFalse;

    /* Adjust frequency if given a percentage. */

    if(frequency > 1.0)
        frequency /= 100.0;

    /* Build the column expression. */

    ba = ensFeatureadaptorGetBaseadaptor(gvvfa);

    txtcolumns = ensBaseadaptorGetColumns(ba);

    columns = ajStrNew();

    for(i = 0; txtcolumns[i]; i++)
        ajFmtPrintAppS(&columns, "%s, ", txtcolumns[i]);

    /* Remove last comma and space from the column expression. */

    ajStrCutEnd(&columns, 2);

    /* Build the final SQL statement. */

    dba = ensFeatureadaptorGetDatabaseadaptor(gvvfa);

    statement = ajFmtStr(
	"SELECT "
        "%S "
	"FROM "
        "variation_feature vf, "
        "source s, "
        "allele a "
	"WHERE "
        "variation_feature.source_id = s.source_id "
	"AND "
        "variation_feature.variation_id = a.variation_id "
	"AND "
        "a.sample_id = %u "
	"AND "
        "("
        "IF(allele.frequency > 0.5, 1 - allele.frequency, allele.frequency) > %f"
        ") "
	"AND "
        "variation_feature.seq_region_id = %u "
	"AND "
        "variation_feature.seq_region_end > %d "
	"AND "
        "variation_feature.seq_region_start < %d "
        "GROUP BY "
        "allele.variation_id",
        columns,
        ensGvpopulationGetIdentifier(gvp),
        frequency,
        ensSliceGetSeqregionIdentifier(slice),
        ensSliceGetStart(slice),
        ensSliceGetEnd(slice));

    gvvariationfeatureadaptorFetchAllBySQL(dba,
                                           statement,
                                           (EnsPAssemblymapper) NULL,
                                           (EnsPSlice) NULL,
                                           gvvfs);

    ajStrDel(&columns);
    ajStrDel(&statement);

    return ajTrue;
}




/* @func ensGvvariationfeatureadaptorFetchAllIdentifiers **********************
**
** Fetch all SQL database-internal identifiers of Ensembl Genetic Variation
** Variation Features.
**
** The caller is responsible for deleting the AJAX unsigned integers before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationFeatureAdaptor::list_dbIDs
** @param [u] gvvfa [const EnsPGvvariationfeatureadaptor] Ensembl Genetic
**                                         Variation Variation Feature Adaptor
** @param [u] identifiers [AjPList] AJAX List of AJAX unsigned integers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureadaptorFetchAllIdentifiers(
    const EnsPGvvariationfeatureadaptor gvvfa,
    AjPList identifiers)
{
    AjBool value = AJFALSE;

    AjPStr table = NULL;

    EnsPBaseadaptor ba = NULL;

    if(!gvvfa)
        return ajFalse;

    if(!identifiers)
        return ajFalse;

    ba = ensFeatureadaptorGetBaseadaptor(gvvfa);

    table = ajStrNewC("variation_feature");

    value = ensBaseadaptorFetchAllIdentifiers(ba,
                                              table,
                                              (AjPStr) NULL,
                                              identifiers);

    ajStrDel(&table);

    return value;
}




/* @datasection [EnsPGvvariationset] Genetic Variation Variation Set **********
**
** Functions for manipulating Ensembl Genetic Variation Variation Set objects
**
** @cc Bio::EnsEMBL::Variation::VariationSet CVS Revision: 1.3
**
** @nam2rule Gvvariationset
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Variation Set
** by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Variation Set. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvvariationset]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvvariationset] Ensembl Genetic Variation
**                                          Variation Set
** @argrule Ref object [EnsPGvvariationset] Ensembl Genetic Variation
**                                          Variation Set
**
** @valrule * [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
**
** @fcategory new
******************************************************************************/




/* @func ensGvvariationsetNew *************************************************
**
** Default constructor for an Ensembl Genetic Variation Variation Set.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [rN] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                               Variation Set Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::VariationSet::new
** @param [u] name [AjPStr] Name
** @param [u] description [AjPStr] Description
**
** @return [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
** @@
******************************************************************************/

EnsPGvvariationset ensGvvariationsetNew(EnsPGvvariationsetadaptor gvvsa,
                                        ajuint identifier,
                                        AjPStr name,
                                        AjPStr description)
{
    EnsPGvvariationset gvvs = NULL;

    if(!name)
        return NULL;

    if(!description)
        return NULL;

    AJNEW0(gvvs);

    gvvs->Use = 1;

    gvvs->Identifier = identifier;

    gvvs->Adaptor = gvvsa;

    if(name)
        gvvs->Name = ajStrNewS(name);

    if(description)
        gvvs->Description = ajStrNewS(description);

    return gvvs;
}




/* @func ensGvvariationsetNewObj **********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvvariationset] Ensembl Genetic Variation
**                                              Variation Set
**
** @return [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
**                              or NULL
** @@
******************************************************************************/

EnsPGvvariationset ensGvvariationsetNewObj(const EnsPGvvariationset object)
{
    EnsPGvvariationset gvvs = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvvs);

    gvvs->Use = 1;

    gvvs->Identifier = object->Identifier;

    gvvs->Adaptor = object->Adaptor;

    if(object->Name)
        gvvs->Name = ajStrNewRef(object->Name);

    if(object->Description)
        gvvs->Description = ajStrNewRef(object->Description);

    return gvvs;
}




/* @func ensGvvariationsetNewRef **********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
**
** @return [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
** @@
******************************************************************************/

EnsPGvvariationset ensGvvariationsetNewRef(EnsPGvvariationset gvvs)
{
    if(!gvvs)
        return NULL;

    gvvs->Use++;

    return gvvs;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Variation Set.
**
** @fdata [EnsPGvvariationset]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Variation Set object
**
** @argrule * Pgvvs [EnsPGvvariationset*] Genetic Variation Variation Set
**                                        object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvariationsetDel *************************************************
**
** Default destructor for an Ensembl Genetic Variation Variation Set.
**
** @param [d] Pgvvs [EnsPGvvariationset*] Ensembl Genetic Variation
**                                        Variation Set address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationsetDel(EnsPGvvariationset *Pgvvs)
{
    EnsPGvvariationset pthis = NULL;

    if(!Pgvvs)
        return;

    if(!*Pgvvs)
        return;

    if(ajDebugTest("ensGvvariationsetDel"))
    {
        ajDebug("ensGvvariationsetDel\n"
                "  *Pgvvs %p\n",
                *Pgvvs);

        ensGvvariationsetTrace(*Pgvvs, 1);
    }

    pthis = *Pgvvs;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvvs = NULL;

        return;
    }

    ajStrDel(&pthis->Name);

    ajStrDel(&pthis->Description);

    AJFREE(pthis);

    *Pgvvs = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Set object.
**
** @fdata [EnsPGvvariationset]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Variation Set attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation
**                      Variation Set Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetDescription Return the description
**
** @argrule * gvvs [const EnsPGvvariationset] Genetic Variation Variation Set
**
** @valrule Adaptor [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Description [AjPStr] Description
**
** @fcategory use
******************************************************************************/




/* @func ensGvvariationsetGetAdaptor ******************************************
**
** Get the Ensembl Genetic Variation Variation Set Adaptor element of an
** Ensembl Genetic Variation Variation Set.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] gvvs [const EnsPGvvariationset] Ensembl Genetic Variation
**                                            Variation Set
**
** @return [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                     Variation Set Adaptor
** @@
******************************************************************************/

EnsPGvvariationsetadaptor ensGvvariationsetGetAdaptor(
    const EnsPGvvariationset gvvs)
{
    if(!gvvs)
        return NULL;

    return gvvs->Adaptor;
}




/* @func ensGvvariationsetGetIdentifier ***************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Set.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] gvvs [const EnsPGvvariationset] Ensembl Genetic Variation
**                                            Variation Set
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvvariationsetGetIdentifier(
    const EnsPGvvariationset gvvs)
{
    if(!gvvs)
        return 0;

    return gvvs->Identifier;
}




/* @func ensGvvariationsetGetName *********************************************
**
** Get the name element of an Ensembl Genetic Variation Variation Set.
**
** @param [r] gvvs [const EnsPGvvariationset] Ensembl Genetic Variation
**                                            Variation Set
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvvariationsetGetName(
    const EnsPGvvariationset gvvs)
{
    if(!gvvs)
        return NULL;

    return gvvs->Name;
}




/* @func ensGvvariationsetGetDescription **************************************
**
** Get the description element of an Ensembl Genetic Variation Variation Set.
**
** @param [r] gvvs [const EnsPGvvariationset] Ensembl Genetic Variation
**                                            Variation Set
**
** @return [AjPStr] Description
** @@
******************************************************************************/

AjPStr ensGvvariationsetGetDescription(
    const EnsPGvvariationset gvvs)
{
    if(!gvvs)
        return NULL;

    return gvvs->Description;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Variation Set object.
**
** @fdata [EnsPGvvariationset]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Variation Set
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation
**                      Variation Set Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetDescription Set the description
**
** @argrule * gvvs [EnsPGvvariationset] Ensembl Genetic Variation
**                                      Variation Set object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvvariationsetSetAdaptor ******************************************
**
** Set the Ensembl Genetic Variation Variation Set Adaptor element of an
** Ensembl Genetic Variation Variation Set.
**
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation
**                                      Variation Set
** @param [r] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetSetAdaptor(EnsPGvvariationset gvvs,
                                   EnsPGvvariationsetadaptor gvvsa)
{
    if(!gvvs)
        return ajFalse;

    gvvs->Adaptor = gvvsa;

    return ajTrue;
}




/* @func ensGvvariationsetSetIdentifier ***************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Set.
**
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation
**                                      Variation Set
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetSetIdentifier(EnsPGvvariationset gvvs,
                                      ajuint identifier)
{
    if(!gvvs)
        return ajFalse;

    gvvs->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvvariationsetSetName *********************************************
**
** Set the name element of an
** Ensembl Genetic Variation Variation Set.
**
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation
**                                      Variation Set
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetSetName(EnsPGvvariationset gvvs,
                                AjPStr name)
{
    if(!gvvs)
        return ajFalse;

    ajStrDel(&gvvs->Name);

    if(name)
        gvvs->Name = ajStrNewRef(name);

    return ajTrue;
}




/* @func ensGvvariationsetSetDescription **************************************
**
** Set the description element of an
** Ensembl Genetic Variation Variation Set.
**
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation
**                                      Variation Set
** @param [u] description [AjPStr] Description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetSetDescription(EnsPGvvariationset gvvs,
                                       AjPStr description)
{
    if(!gvvs)
        return ajFalse;

    ajStrDel(&gvvs->Description);

    if(description)
        gvvs->Description = ajStrNewRef(description);

    return ajTrue;
}




/* @func ensGvvariationsetGetMemsize ******************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Variation Set.
**
** @param [r] gvvs [const EnsPGvvariationset] Ensembl Genetic Variation
**                                            Variation Set
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvvariationsetGetMemsize(const EnsPGvvariationset gvvs)
{
    ajulong size = 0;

    if(!gvvs)
        return 0;

    size += sizeof (EnsOGvvariationset);

    if(gvvs->Name)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvvs->Name);
    }

    if(gvvs->Description)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvvs->Description);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an
** Ensembl Genetic Variation Variation Set object.
**
** @fdata [EnsPGvvariationset]
** @nam3rule Trace Report Ensembl Genetic Variation Variation Set elements
**                 to debug file
**
** @argrule Trace gvvs [const EnsPGvvariationset] Ensembl Genetic Variation
**                                                Variation Set
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvvariationsetTrace ***********************************************
**
** Trace an Ensembl Genetic Variation Variation Set.
**
** @param [r] gvvs [const EnsPGvvariationset] Ensembl Genetic Variation
**                                            Variation Set
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetTrace(const EnsPGvvariationset gvvs,
                              ajuint level)
{
    AjPStr indent = NULL;

    if(!gvvs)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvvariationsetTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Name '%S'\n"
            "%S  Description '%S'\n",
            indent, gvvs,
            indent, gvvs->Use,
            indent, gvvs->Identifier,
            indent, gvvs->Adaptor,
            indent, gvvs->Name,
            indent, gvvs->Description);

    ajStrDel(&indent);

    return ajTrue;
}




/* @func ensGvvariationsetFetchAllSubSets *************************************
**
** Recursively fetch all Ensembl Genetic Variation Variation Sets, which are
** subsets of this Ensembl Genetic Variation Variation Set.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Sets before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::VariationSet::get_all_sub_VariationSets
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
** @param [r] immediate [AjBool] ajTrue:  only the direct subsets of this
**                                        variation set will be fetched
**                               ajFalse: recursively fetch all subsets
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Set objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetFetchAllSubSets(EnsPGvvariationset gvvs,
                                        AjBool immediate,
                                        AjPList gvvss)
{
    if(!gvvs)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    if(gvvs->Adaptor)
        return ajTrue;

    return ensGvvariationsetadaptorFetchAllBySuperSet(gvvs->Adaptor,
                                                      gvvs,
                                                      immediate,
                                                      gvvss);
}




/* @func ensGvvariationsetFetchAllSuperSets ***********************************
**
** Recursively fetch all Ensembl Genetic Variation Variation Sets, which are
** supersets of this Ensembl Genetic Variation Variation Set.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Sets before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::VariationSet::get_all_sub_VariationSets
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
** @param [r] immediate [AjBool] ajTrue:  only the direct supersets of this
**                                        variation set will be fetched
**                               ajFalse: recursively fetch all supersets
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Set objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetFetchAllSuperSets(EnsPGvvariationset gvvs,
                                          AjBool immediate,
                                          AjPList gvvss)
{
    if(!gvvs)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    if(gvvs->Adaptor)
        return ajTrue;

    return ensGvvariationsetadaptorFetchAllBySubSet(gvvs->Adaptor,
                                                    gvvs,
                                                    immediate,
                                                    gvvss);
}




/* @func ensGvvariationsetFetchAllGvvariations ********************************
**
** Recursively fetch all Ensembl Genetic Variation Variation Sets, which belong
** to this Ensembl Genetic Variation Variation Set and all subsets.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::VariationSet::get_all_Variations
** @param [u] gvvs [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetFetchAllGvvariations(
    EnsPGvvariationset gvvs,
    AjPList gvvss)
{
    EnsPDatabaseadaptor dba = NULL;

    EnsPGvvariationadaptor gvva = NULL;

    if(!gvvs)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    if(!gvvs->Adaptor)
        return ajTrue;

    dba = ensBaseadaptorGetDatabaseadaptor(gvvs->Adaptor);

    gvva = ensRegistryGetGvvariationadaptor(dba);

    return ensGvvariationadaptorFetchAllByGvvariationset(gvva, gvvs, gvvss);
}




/* @datasection [EnsPGvvariationsetadaptor] Genetic Variation
**                                          Variation Set Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Variation
** Set Adaptor objects
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor
**     CVS Revision: 1.3
**
** @nam2rule Gvvariationsetadaptor
******************************************************************************/

static const char *gvvariationsetadaptorTables[] =
{
    "variation_set",
    NULL
};

static const char *gvvariationsetadaptorColumns[] =
{
    "variation_set.variation_set_id",
    "variation_set.name",
    "variation_set.description",
    NULL
};

static EnsOBaseadaptorLeftJoin gvvariationsetadaptorLeftJoin[] =
{
    {NULL, NULL}
};

static const char *gvvariationsetadaptorDefaultCondition = NULL;

static const char *gvvariationsetadaptorFinalCondition = NULL;




/* @funcstatic gvvariationsetadaptorFetchAllBySQL *****************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Genetic Variation Variation Set
** objects.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::_objs_from_sth
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Sets
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvvariationsetadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                 const AjPStr statement,
                                                 EnsPAssemblymapper am,
                                                 EnsPSlice slice,
                                                 AjPList gvvss)
{
    ajuint identifier = 0;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr name        = NULL;
    AjPStr description = NULL;

    EnsPGvvariationset        gvvs  = NULL;
    EnsPGvvariationsetadaptor gvvsa = NULL;

    if(ajDebugTest("gvvariationsetadaptorFetchAllBySQL"))
        ajDebug("gvvariationsetadaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  gvvss %p\n",
                dba,
                statement,
                am,
                slice,
                gvvss);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    gvvsa = ensRegistryGetGvvariationsetadaptor(dba);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier  = 0;
        name        = ajStrNew();
        description = ajStrNew();

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToStr(sqlr, &name);
        ajSqlcolumnToStr(sqlr, &description);

        gvvs = ensGvvariationsetNew(gvvsa,
                                    identifier,
                                    name,
                                    description);

        ajListPushAppend(gvvss, (void *) gvvs);

        ajStrDel(&name);
        ajStrDel(&description);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    return ajTrue;
}




/* @func ensGvvariationsetadaptorNew ******************************************
**
** Default constructor for an Ensembl Genetic Variation Variation Set Adaptor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetGvvariationsetadaptor
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::new
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                     Variation Set Adaptor or NULL
** @@
******************************************************************************/

EnsPGvvariationsetadaptor ensGvvariationsetadaptorNew(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    if(ajDebugTest("ensGvvariationsetadaptorNew"))
        ajDebug("ensGvvariationsetadaptorNew\n"
                "  dba %p\n",
                dba);

    return ensBaseadaptorNew(
        dba,
        gvvariationsetadaptorTables,
        gvvariationsetadaptorColumns,
        gvvariationsetadaptorLeftJoin,
        gvvariationsetadaptorDefaultCondition,
        gvvariationsetadaptorFinalCondition,
        gvvariationsetadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Variation Set Adaptor.
**
** @fdata [EnsPGvvariationsetadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Genetic Variation
**                                 Variation Set Adaptor object.
**
** @argrule * Pgvvsa [EnsPGvvariationsetadaptor*] Ensembl Genetic Variation
**                                        Variation Set Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvariationsetadaptorDel ******************************************
**
** Default destructor for an Ensembl Genetic Variation Variation Set Adaptor.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Pgvvsa [EnsPGvvariationsetadaptor*] Ensembl Genetic Variation
**                                               Variation Set Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationsetadaptorDel(EnsPGvvariationsetadaptor* Pgvvsa)
{
    EnsPGvvariationsetadaptor pthis = NULL;

    if(!Pgvvsa)
        return;

    if(!*Pgvvsa)
        return;

    pthis = *Pgvvsa;

    ensBaseadaptorDel(&pthis);

    *Pgvvsa = NULL;

    return;
}




/* @func ensGvvariationsetadaptorFetchByIdentifier ****************************
**
** Fetch an Ensembl Genetic Variation Variation Set via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Set.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::fetch_by_dbID
** @param [u] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pgvvs [EnsPGvvariationset*] Ensembl Genetic Variation
**                                         Variation Set address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetadaptorFetchByIdentifier(
    EnsPGvvariationsetadaptor gvvsa,
    ajuint identifier,
    EnsPGvvariationset *Pgvvs)
{
    if(!gvvsa)
        return ajFalse;

    if(!identifier)
        return ajFalse;

    if(!Pgvvs)
        return ajFalse;

    *Pgvvs = ensBaseadaptorFetchByIdentifier(gvvsa, identifier);

    return ajTrue;
}




/* @func ensGvvariationsetadaptorFetchByName **********************************
**
** Fetch an Ensembl Genetic Variation Variation Set via its name.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Set.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::fetch_by_name
** @param [u] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
** @param [r] name [const AjPStr] name
** @param [wP] Pgvvs [EnsPGvvariationset*] Ensembl Genetic Variation
**                                         Variation Set address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetadaptorFetchByName(
    EnsPGvvariationsetadaptor gvvsa,
    const AjPStr name,
    EnsPGvvariationset *Pgvvs)
{
    char *txtname = NULL;

    AjPList gvvss = NULL;

    AjPStr constraint = NULL;

    EnsPGvvariationset gvvs = NULL;

    if(!gvvsa)
        return ajFalse;

    if(!(name && ajStrGetLen(name)))
        return ajFalse;

    if(!Pgvvs)
        return ajFalse;

    ensBaseadaptorEscapeC(gvvsa, &txtname, name);

    constraint = ajFmtStr("variation_set.name = %s", txtname);

    ajCharDel(&txtname);

    gvvss = ajListNew();

    ensBaseadaptorGenericFetch(gvvsa,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               gvvss);

    if(ajListGetLength(gvvss) > 1)
        ajWarn("ensGvvariationsetadaptorFetchByName got more "
               "than one Ensembl Genetic Variation Variation Set for "
               "name '%S'.\n", name);

    ajListPop(gvvss, (void **) Pgvvs);

    while(ajListPop(gvvss, (void **) &gvvs))
        ensGvvariationsetDel(&gvvs);

    ajListFree(&gvvss);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @func ensGvvariationsetadaptorFetchAllToplevel *****************************
**
** Fetch all top-level Ensembl Genetic Variation Variation Sets, which
** are not sub-sets of any other Ensembl Genetic Variation Variation Set.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Sets before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::
**     fetch_all_top_VariationSets
** @param [u] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Set objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetadaptorFetchAllToplevel(
    EnsPGvvariationsetadaptor gvvsa,
    AjPList gvvss)
{
    AjPStr constraint = NULL;

    if(!gvvss)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    constraint = ajStrNewC(
        "NOT EXISTS "
        "("
        "SELECT "
        "* "
        "FROM "
        "variation_set_structure "
        "WHERE "
        "variation_set_structure.variation_set_sub = "
        "variation_set.variation_set_id "
        ")");

    ensBaseadaptorGenericFetch(gvvsa,
                               constraint,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               gvvss);

    ajStrDel(&constraint);

    return ajTrue;
}




/* @funcstatic gvvariationsetlistCompareIdentifier ****************************
**
** AJAX List comparison function to sort Ensembl Genetic Variation Variation
** Set objects by identifier in ascending order.
**
** @param [r] P1 [const void*] Ensembl Genetic Variation Variation Set 1
** @param [r] P2 [const void*] Ensembl Genetic Variation Variation Set 2
** @see ajListSort
** @see ajListSortUnique
**
** @return [int] The comparison function returns an integer less than,
**               equal to, or greater than zero if the first argument is
**               considered to be respectively less than, equal to, or
**               greater than the second.
** @@
******************************************************************************/

static int gvvariationsetlistCompareIdentifier(const void *P1, const void *P2)
{
    int value = 0;

    const EnsPGvvariationset gvvs1 = NULL;
    const EnsPGvvariationset gvvs2 = NULL;

    gvvs1 = *(EnsPGvvariationset const *) P1;
    gvvs2 = *(EnsPGvvariationset const *) P2;

    if(ajDebugTest("gvvariationsetlistCompare"))
        ajDebug("gvvariationsetlistCompare\n"
                "  gvvs1 %p\n"
                "  gvvs2 %p\n",
                gvvs1,
                gvvs2);

    if(gvvs1->Identifier < gvvs2->Identifier)
        value = -1;

    if(gvvs1->Identifier > gvvs2->Identifier)
        value = +1;

    return value;
}




/* @funcstatic gvvariationsetlistDelete ***************************************
**
** ajListSortUnique nodedelete function to delete Ensembl Genetic Variation
** Variation Set objects that are redundant.
**
** @param [r] PP1 [void**] Ensembl Genetic Variation Variation Set address 1
** @param [r] cl [void*] Standard, passed in from ajListSortUnique
** @see ajListSortUnique
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationsetlistDelete(void **PP1, void *cl)
{
    if(!PP1)
        return;

    (void) cl;

    ensGvvariationsetDel((EnsPGvvariationset *) PP1);

    return;
}




/* @funcstatic gvvariationsetadaptorClearGvvariationsets **********************
**
** An ajTableMapDel 'apply' function to clear an AJAX Table of Ensembl Genetic
** Variation Variation Sets used in ensGvvariationsetadaptorFetchAllBySubSet.
** This function deletes the AJAX unsigned identifier key data and moves the
** Ensembl Genetic Variation Variation Set value data onto the
** AJAX List.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Gene value data address
** @param [u] cl [void*] AJAX List, passed in from ajTableMapDel
** @see ajTableMapDel
** @see ensGvvariationsetadaptorFetchAllBySubSet
** @see ensGvvariationsetadaptorFetchAllBySuperSet
**
** @return [void]
** @@
******************************************************************************/

static void gvvariationsetadaptorClearGvvariationsets(void **key,
                                                      void **value,
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

    if(!cl)
        return;

    AJFREE(*key);

    ajListPushAppend(cl, value);

    return;
}




/* @func ensGvvariationsetadaptorFetchAllByGvvariation ************************
**
** Fetch an Ensembl Genetic Variation Variation Set via an
** Ensembl Genetic Variation Variation.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Sets before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::fetch_by_Variation
** @param [u] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Sets
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetadaptorFetchAllByGvvariation(
    EnsPGvvariationsetadaptor gvvsa,
    const EnsPGvvariation gvv,
    AjPList gvvss)
{
    const char **txtcolumns = NULL;

    register ajuint i = 0;

    AjPList variationsets = NULL;

    AjPStr columns = NULL;
    AjPStr statement = NULL;

    EnsPGvvariationset gvvs = NULL;

    if(!gvvsa)
        return ajFalse;

    if(!gvv)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    txtcolumns = ensBaseadaptorGetColumns(gvvsa);

    columns = ajStrNew();

    for(i = 0; txtcolumns[i]; i++)
        ajFmtPrintAppS(&columns, "%s, ", txtcolumns[i]);

    /* Remove last comma and space from the column expression. */

    ajStrCutEnd(&columns, 2);

    statement = ajFmtStr(
        "SELECT "
        "%S "
        "FROM "
        "variation_set, "
        "variation_set_variation "
        "WHERE "
        "variation_set.variation_set_id = "
        "variation_set_variation.variation_set_id "
        "AND "
        "variation_set_variation.variation_id = %u",
        columns,
        ensGvvariationGetIdentifier(gvv));

    ajStrDel(&columns);

    variationsets = ajListNew();

    ensBaseadaptorGenericFetch(gvvsa,
                               statement,
                               (EnsPAssemblymapper) NULL,
                               (EnsPSlice) NULL,
                               variationsets);

    ajStrDel(&statement);

    /*
    ** Fetch all supersets of the returned Ensembl Genetic Variation
    ** Variation Sets as well. Since an Ensembl Genetic Variation Variation may
    ** occur at several places in a hierarchy, which will cause duplicated
    ** data sort the AJAX List uniquely.
    */

    while(ajListPop(variationsets, (void **) &gvvs))
    {
        ensGvvariationsetadaptorFetchAllBySubSet(gvvsa,
                                                 gvvs,
                                                 ajFalse,
                                                 gvvss);

        /*
        ** Move the current Ensembl Genetic Variation Variation Set onto the
        ** AJAX List of subsets, too.
        */

        ajListPushAppend(gvvss, (void *) gvvs);
    }

    ajListFree(&variationsets);

    ajListSortUnique(gvvss,
                     gvvariationsetlistCompareIdentifier,
                     gvvariationsetlistDelete);

    return ajTrue;
}




/* @func ensGvvariationsetadaptorFetchAllBySubSet *****************************
**
** Fetch all Ensembl Genetic Variation Variation Sets, by a subset of an
** Ensembl Genetic Variation Variation Set.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Sets before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::
**     fetch_all_by_sub_VariationSet
** @param [u] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
** @param [r] gvvs [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
** @param [r] immediate [AjBool] ajTrue:  only the direct supersets of this
**                                        variation set will be fetched
**                               ajFalse: recursively fetch all supersets
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Set objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetadaptorFetchAllBySubSet(
    EnsPGvvariationsetadaptor gvvsa,
    const EnsPGvvariationset gvvs,
    AjBool immediate,
    AjPList gvvss)
{
    ajuint *Pidentifier = NULL;

    ajuint vssid = 0;

    AjPList variationsets = NULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr statement = NULL;

    AjPTable gvvstable = NULL;

    EnsPDatabaseadaptor dba = NULL;

    EnsPGvvariationset variationset = NULL;

    if(!gvvsa)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    variationsets = ajListNew();

    gvvstable = MENSTABLEUINTNEW(0);

    dba = ensBaseadaptorGetDatabaseadaptor(gvvsa);

    /*
    ** First, get all Ensembl Genetic Variation Variation Sets that are
    ** direct supersets of this one.
    */

    statement = ajFmtStr(
        "SELECT "
        "variation_set_structure.variation_set_super "
        "FROM "
        "variation_set_structure "
        "WHERE "
        "variation_set_structure.variation_set_sub = %u",
        gvvs->Identifier);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        vssid = 0;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &vssid);

        if(!ajTableFetch(gvvstable, (const void *) &vssid))
        {
            ensGvvariationsetadaptorFetchByIdentifier(gvvsa,
                                                      vssid,
                                                      &variationset);

            if(!variationset)
                continue;

            AJNEW0(Pidentifier);

            *Pidentifier = vssid;

            ajTablePut(gvvstable, (void *) Pidentifier, (void *) variationset);

            if(immediate)
                continue;

            ensGvvariationsetadaptorFetchAllBySubSet(gvvsa,
                                                     variationset,
                                                     immediate,
                                                     variationsets);

            while(ajListPop(variationsets, (void **) &variationset))
            {
                if(!variationset)
                    continue;

                vssid = ensGvvariationsetGetIdentifier(variationset);

                if(!ajTableFetch(gvvstable, (const void *) &vssid))
                {
                    AJNEW0(Pidentifier);

                    *Pidentifier = ensGvvariationsetGetIdentifier(variationset);

                    ajTablePut(gvvstable,
                               (void *) Pidentifier,
                               (void *) ensGvvariationsetNewRef(variationset));
                }

                ensGvvariationsetDel(&variationset);
            }
        }
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    ajListFree(&variationsets);

    /*
    ** Move all Ensembl Genetic Variation Variation Sets from the
    ** AJAX Table onto the AJAX List.
    */

    ajTableMapDel(gvvstable, gvvariationsetadaptorClearGvvariationsets, gvvss);

    ajTableFree(&gvvstable);

    return ajTrue;
}




/* @func ensGvvariationsetadaptorFetchAllBySuperSet ***************************
**
** Fetch all Ensembl Genetic Variation Variation Sets, by a superset of an
** Ensembl Genetic Variation Variation Set.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation Sets before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationSetAdaptor::
**     fetch_all_by_super_VariationSet
** @param [u] gvvsa [EnsPGvvariationsetadaptor] Ensembl Genetic Variation
**                                              Variation Set Adaptor
** @param [r] gvvs [EnsPGvvariationset] Ensembl Genetic Variation Variation Set
** @param [r] immediate [AjBool] ajTrue:  only the direct subsets of this
**                                        variation set will be fetched
**                               ajFalse: recursively fetch all supersets
** @param [u] gvvss [AjPList] AJAX List of Ensembl Genetic Variation
**                            Variation Set objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationsetadaptorFetchAllBySuperSet(
    EnsPGvvariationsetadaptor gvvsa,
    const EnsPGvvariationset gvvs,
    AjBool immediate,
    AjPList gvvss)
{
    ajuint *Pidentifier = NULL;

    ajuint vssid = 0;

    AjPList variationsets = NULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr statement = NULL;

    AjPTable gvvstable = NULL;

    EnsPDatabaseadaptor dba = NULL;

    EnsPGvvariationset variationset = NULL;

    if(!gvvsa)
        return ajFalse;

    if(!gvvs)
        return ajFalse;

    if(!gvvss)
        return ajFalse;

    variationsets = ajListNew();

    gvvstable = MENSTABLEUINTNEW(0);

    dba = ensBaseadaptorGetDatabaseadaptor(gvvsa);

    /*
    ** First, get all Ensembl Genetic Variation Variation Sets that are
    ** direct supersets of this one.
    */

    statement = ajFmtStr(
        "SELECT "
        "variation_set_structure.variation_set_sub "
        "FROM "
        "variation_set_structure "
        "WHERE "
        "variation_set_structure.variation_set_super = %u",
        gvvs->Identifier);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        vssid = 0;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &vssid);

        if(!ajTableFetch(gvvstable, (const void *) &vssid))
        {
            ensGvvariationsetadaptorFetchByIdentifier(gvvsa,
                                                      vssid,
                                                      &variationset);

            if(!variationset)
                continue;

            AJNEW0(Pidentifier);

            *Pidentifier = vssid;

            ajTablePut(gvvstable, (void *) Pidentifier, (void *) variationset);

            if(immediate)
                continue;

            ensGvvariationsetadaptorFetchAllBySubSet(gvvsa,
                                                     variationset,
                                                     immediate,
                                                     variationsets);

            while(ajListPop(variationsets, (void **) &variationset))
            {
                if(!variationset)
                    continue;

                vssid = ensGvvariationsetGetIdentifier(variationset);

                if(!ajTableFetch(gvvstable, (const void *) &vssid))
                {
                    AJNEW0(Pidentifier);

                    *Pidentifier = ensGvvariationsetGetIdentifier(variationset);

                    ajTablePut(gvvstable,
                               (void *) Pidentifier,
                               (void *) ensGvvariationsetNewRef(variationset));
                }

                ensGvvariationsetDel(&variationset);
            }
        }
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    ajStrDel(&statement);

    ajListFree(&variationsets);

    /*
    ** Move all Ensembl Genetic Variation Variation Sets from the
    ** AJAX Table onto the AJAX List.
    */

    ajTableMapDel(gvvstable, gvvariationsetadaptorClearGvvariationsets, gvvss);

    ajTableFree(&gvvstable);

    return ajTrue;
}




/* @datasection [EnsPGvconsequence] Genetic Variation Consequence *************
**
** Functions for manipulating Ensembl Genetic Variation Consequence objects
**
** @cc Bio::EnsEMBL::Variation::ConsequenceType CVS Revision: 1.22
**
** @nam2rule Gvconsequence
**
******************************************************************************/




/* consequenceType ************************************************************
**
** The special consequence type, SARA, that only applies to the effect of the
** Alleles, not Variations and is equivalent to Same As Reference Allele,
** meaning that the Allele is the same as in reference sequence, so has no
** effect but it is not stored anywhere in the database and need no conversion
** at all when creating the VariationFeature object, thus the absence in the
** array.
**
******************************************************************************/

const char *consequenceType[] =
{
    "ESSENTIAL_SPLICE_SITE",  /* 1 <<  0 =       1 */
    "STOP_GAINED",            /* 1 <<  1 =       2 */
    "STOP_LOST",              /* 1 <<  2 =       4 */
    "COMPLEX_INDEL",          /* 1 <<  3 =       8 */
    "FRAMESHIFT_CODING",      /* 1 <<  4 =      16 */
    "NON_SYNONYMOUS_CODING",  /* 1 <<  5 =      32 */
    "SPLICE_SITE",            /* 1 <<  6 =      64 */
    "PARTIAL_CODON",          /* 1 <<  7 =     128 */
    "SYNONYMOUS_CODING",      /* 1 <<  8 =     256 */
    "REGULATORY_REGION",      /* 1 <<  9 =     512 */
    "WITHIN_MATURE_miRNA",    /* 1 << 10 =    1024 */
    "5PRIME_UTR",             /* 1 << 11 =    2048 */
    "3PRIME_UTR",             /* 1 << 12 =    2094 */
    "UTR",                    /* 1 << 13 =    4096 */
    "INTRONIC",               /* 1 << 14 =    8192 */
    "NMD_TRANSCRIPT",         /* 1 << 15 =   16384 */
    "WITHIN_NON_CODING_GENE", /* 1 << 16 =   32768 */
    "UPSTREAM",               /* 1 << 17 =   65536 */
    "DOWNSTREAM",             /* 1 << 18 =  131072 */
    "HGMD_MUTATION",          /* 1 << 19 =  262144 */
    "NO_CONSEQUENCE",         /* 1 << 20 =  524288 */
    "INTERGENIC",             /* 1 << 21 = 1048576 */
    "_",                      /* 1 << 22 = 2097152 */
    NULL
};




/* consequenceTranslation *****************************************************
**
** Array of consequence types that have an influence on the Transaltion of a
** Transcript.
**
******************************************************************************/

const char *consequenceTranslation[] =
{
    "STOP_GAINED",
    "STOP_LOST",
    "COMPLEX_INDEL",
    "FRAMESHIFT_CODING",
    "NON_SYNONYMOUS_CODING",
    "PARTIAL_CODON",
    NULL
};




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Consequence by
** pointer. It is the responsibility of the user to first destroy any previous
** Genetic Variation Consequence. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvconsequence]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvconsequence] Ensembl Genetic Variation
**                                         Consequence
** @argrule Ref object [EnsPGvconsequence] Ensembl Genetic Variation
**                                         Consequence
**
** @valrule * [EnsPGvconsequence] Ensembl Genetic Variation Consequence
**
** @fcategory new
******************************************************************************/




/* @func ensGvconsequenceNew **************************************************
**
** Default constructor for an Ensembl Genetic Variation Consequence.
**
** @cc Bio::EnsEMBL::Variation::ConsequenceType::new
** @param [r] transcriptid [ajuint] Ensembl Transcript identifier
** @param [r] gvvfid [ajuint] Ensembl Genetic Variation Variation Feature
**                            identifier
** @param [r] start [ajint] Start
** @param [r] end [ajint] End
** @param [r] strand [ajint] Strand
**
** @return [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @@
******************************************************************************/

EnsPGvconsequence ensGvconsequenceNew(ajuint transcriptid,
                                      ajuint gvvfid,
                                      ajint start,
                                      ajint end,
                                      ajint strand)
{
    EnsPGvconsequence gvc = NULL;

    if(!transcriptid)
        return NULL;

    if(!gvvfid)
        return NULL;

    AJNEW0(gvc);

    gvc->Use = 1;

    gvc->TranscriptIdentifier = transcriptid;

    gvc->GvvariationfeatureIdentifier = gvvfid;

    gvc->Start = start;

    gvc->End = end;

    gvc->Strand = strand;

    return gvc;
}




/* @func ensGvconsequenceNewObj ***********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvconsequence] Ensembl Genetic Variation
**                                             Consequence
**
** @return [EnsPGvconsequence] Ensembl Genetic Variation Consequence or NULL
** @@
******************************************************************************/

EnsPGvconsequence ensGvconsequenceNewObj(const EnsPGvconsequence object)
{
    EnsPGvconsequence gvvc = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvvc);

    gvvc->Use = 1;

    gvvc->TranscriptIdentifier = object->TranscriptIdentifier;

    gvvc->GvvariationfeatureIdentifier = object->GvvariationfeatureIdentifier;

    gvvc->Start = object->Start;

    gvvc->End = object->End;

    gvvc->Strand = object->Strand;

    gvvc->Alleles = ajListstrNew();

    gvvc->Types = ajListstrNew();

    return gvvc;
}




/* @func ensGvconsequenceNewRef ***********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvvc [EnsPGvconsequence] Ensembl Genetic Variation Consequence
**
** @return [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @@
******************************************************************************/

EnsPGvconsequence ensGvconsequenceNewRef(EnsPGvconsequence gvvc)
{
    if(!gvvc)
        return NULL;

    gvvc->Use++;

    return gvvc;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Consequence.
**
** @fdata [EnsPGvconsequence]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Consequence object
**
** @argrule * Pgvvc [EnsPGvconsequence*] Genetic Variation
**                                       Consequence object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvconsequenceDel **************************************************
**
** Default destructor for an Ensembl Genetic Variation Consequence.
**
** @param [d] Pgvvc [EnsPGvconsequence*] Ensembl Genetic Variation
**                                       Consequence address
**
** @return [void]
** @@
******************************************************************************/

void ensGvconsequenceDel(EnsPGvconsequence *Pgvvc)
{
    EnsPGvconsequence pthis = NULL;

    if(!Pgvvc)
        return;

    if(!*Pgvvc)
        return;

    if(ajDebugTest("ensGvconsequenceDel"))
    {
        ajDebug("ensGvconsequenceDel\n"
                "  *Pgvvc %p\n",
                *Pgvvc);

        ensGvconsequenceTrace(*Pgvvc, 1);
    }

    pthis = *Pgvvc;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvvc = NULL;

        return;
    }

    ajListstrFree(&pthis->Alleles);
    ajListstrFree(&pthis->Types);

    AJFREE(pthis);

    *Pgvvc = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Consequence object.
**
** @fdata [EnsPGvconsequence]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Consequence attribute(s)
** @nam4rule GetTranscriptIdentifier Return the Ensembl Transcript identifier
** @nam4rule GetGvvariationfeatureIdentifier Return the Ensembl Genetic
**                                           Variation Variation Feature
**                                           identifier
** @nam4rule GetStart Return the start
** @nam4rule GetEnd Return the end
** @nam4rule GetStrand Return the strand
** @nam4rule GetAlleles Return the AJAX List of AJAX String alleles
** @nam4rule GetTypes Return the AJAX List of AJAX String types
**
** @argrule * gvvc [const EnsPGvconsequence] Genetic Variation Consequence
**
** @valrule TranscriptIdentifier [ajuint] Ensembl Transcript identifier
** @valrule GvvariationfeatureIdentifier [ajuint] Ensembl Genetic
**                                                Variation Variation Feature
**                                                identifier
** @valrule Start [ajint] Start
** @valrule End [ajint] End
** @valrule Strand [ajint] Strand
** @valrule Alleles [AjPList] AJAX List of AJAX String alleles
** @valrule Types [AjPList] AJAX List of AJAX String types
**
** @fcategory use
******************************************************************************/




/* @func ensGvconsequenceGetTranscriptIdentifier ******************************
**
** Get the Ensembl Transcript identifier element of an
** Ensembl Genetic Variation Consequence.
**
** @cc Bio::EnsEMBL::Variation::ConsequenceType::transcript_id
** @param [r] gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                           Consequence
**
** @return [ajuint] Ensembl Transcript identifier
** @@
******************************************************************************/

ajuint ensGvconsequenceGetTranscriptIdentifier(
    const EnsPGvconsequence gvvc)
{
    if(!gvvc)
        return 0;

    return gvvc->TranscriptIdentifier;
}




/* @func ensGvconsequenceGetGvvariationfeatureIdentifier **********************
**
** Get the Ensembl Genetic Variation Variation Feature identifier element of an
** Ensembl Genetic Variation Consequence.
**
** @cc Bio::EnsEMBL::Variation::ConsequenceType::variation_feature_id
** @param [r] gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                           Consequence
**
** @return [ajuint] Ensembl Genetic Variation Variation Feature identifier
** @@
******************************************************************************/

ajuint ensGvconsequenceGetGvvariationfeatureIdentifier(
    const EnsPGvconsequence gvvc)
{
    if(!gvvc)
        return 0;

    return gvvc->GvvariationfeatureIdentifier;
}




/* @func ensGvconsequenceGetStart *********************************************
**
** Get the start element of an Ensembl Genetic Variation Consequence.
**
** @cc Bio::EnsEMBL::Variation::ConsequenceType::start
** @param [r] gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                           Consequence
**
** @return [ajint] Start
** @@
******************************************************************************/

ajint ensGvconsequenceGetStart(
    const EnsPGvconsequence gvvc)
{
    if(!gvvc)
        return 0;

    return gvvc->Start;
}




/* @func ensGvconsequenceGetEnd ***********************************************
**
** Get the end element of an Ensembl Genetic Variation Consequence.
**
** @cc Bio::EnsEMBL::Variation::ConsequenceType::end
** @param [r] gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                           Consequence
**
** @return [ajint] End
** @@
******************************************************************************/

ajint ensGvconsequenceGetEnd(
    const EnsPGvconsequence gvvc)
{
    if(!gvvc)
        return 0;

    return gvvc->End;
}




/* @func ensGvconsequenceGetStrand ********************************************
**
** Get the strand element of an Ensembl Genetic Variation Consequence.
**
** @cc Bio::EnsEMBL::Variation::ConsequenceType::strand
** @param [r] gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                           Consequence
**
** @return [ajint] Strand
** @@
******************************************************************************/

ajint ensGvconsequenceGetStrand(
    const EnsPGvconsequence gvvc)
{
    if(!gvvc)
        return 0;

    return gvvc->Strand;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Consequence object.
**
** @fdata [EnsPGvconsequence]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Consequence
** @nam4rule SetTranscriptIdentifier Set the Ensembl Transcript identifier
** @nam4rule SetGvvariationfeatureIdentifier Set the Ensembl Genetic Variation
**                                           Variation Feature identifier
** @nam4rule SetStart Set the start
** @nam4rule SetEnd Set the end
** @nam4rule SetStrand Set the strand
**
** @argrule * gvvc [EnsPGvconsequence] Ensembl Genetic Variation Consequence
**                                     object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvconsequenceSetTranscriptIdentifier ******************************
**
** Set the Ensembl Transcript identifier element of an
** Ensembl Genetic Variation Consequence.
**
** @param [u] gvvc [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @param [r] identifier [ajuint] Ensembl Transcript identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvconsequenceSetTranscriptIdentifier(EnsPGvconsequence gvvc,
                                               ajuint transcriptid)
{
    if(!gvvc)
        return ajFalse;

    gvvc->TranscriptIdentifier = transcriptid;

    return ajTrue;
}




/* @func ensGvconsequenceSetGvvariationfeatureIdentifier **********************
**
** Set the Ensembl Genetic Variation Variation Feature identifier element of an
** Ensembl Genetic Variation Consequence.
**
** @param [u] gvvc [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @param [r] gvvfid [ajuint] Ensembl Genetic Variation Variation Feature
**                                identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvconsequenceSetGvvariationfeatureIdentifier(EnsPGvconsequence gvvc,
                                                       ajuint gvvfid)
{
    if(!gvvc)
        return ajFalse;

    gvvc->GvvariationfeatureIdentifier = gvvfid;

    return ajTrue;
}




/* @func ensGvconsequenceSetStart *********************************************
**
** Set the start element of an
** Ensembl Genetic Variation Consequence.
**
** @param [u] gvvc [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @param [r] start [ajint] Start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvconsequenceSetStart(EnsPGvconsequence gvvc,
                                ajint start)
{
    if(!gvvc)
        return ajFalse;

    gvvc->Start = start;

    return ajTrue;
}




/* @func ensGvconsequenceSetEnd ***********************************************
**
** Set the end element of an
** Ensembl Genetic Variation Consequence.
**
** @param [u] gvvc [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @param [r] end [ajint] End
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvconsequenceSetEnd(EnsPGvconsequence gvvc,
                              ajint end)
{
    if(!gvvc)
        return ajFalse;

    gvvc->End = end;

    return ajTrue;
}




/* @func ensGvconsequenceSetStrand ********************************************
**
** Set the strand element of an
** Ensembl Genetic Variation Consequence.
**
** @param [u] gvvc [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @param [r] strand [ajint] Strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvconsequenceSetStrand(EnsPGvconsequence gvvc,
                                 ajint strand)
{
    if(!gvvc)
        return ajFalse;

    gvvc->Strand = strand;

    return ajTrue;
}




/* @func ensGvconsequenceGetMemsize *******************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Consequence.
**
** @param [r] gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                           Consequence
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvconsequenceGetMemsize(const EnsPGvconsequence gvvc)
{
    ajulong size = 0;

    if(!gvvc)
        return 0;

    size += sizeof (EnsOGvconsequence);

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an
** Ensembl Genetic Variation Consequence object.
**
** @fdata [EnsPGvconsequence]
** @nam3rule Trace Report Ensembl Genetic Variation Consequence elements
**                 to debug file
**
** @argrule Trace gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                               Consequence
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvconsequenceTrace ************************************************
**
** Trace an Ensembl Genetic Variation Consequence.
**
** @param [r] gvvc [const EnsPGvconsequence] Ensembl Genetic Variation
**                                           Consequence
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvconsequenceTrace(const EnsPGvconsequence gvvc,
                             ajuint level)
{
    AjPStr indent = NULL;

    if(!gvvc)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvconsequenceTrace %p\n"
            "%S  TranscriptIdentifier %u\n"
            "%S  GvvariationfeatureIdentifier %u\n"
            "%S  Start %d\n"
            "%S  End %d\n"
            "%S  Strand %d\n",
            indent, gvvc,
            indent, gvvc->TranscriptIdentifier,
            indent, gvvc->GvvariationfeatureIdentifier,
            indent, gvvc->Start,
            indent, gvvc->End,
            indent, gvvc->Strand);

    ajStrDel(&indent);

    return ajTrue;
}




/* @datasection [EnsPGvpopulationgenotype] Genetic Variation
**                                        Population Genotype
**
** Functions for manipulating Ensembl Genetic Variation Population Genotype
** objects
**
** @cc Bio::EnsEMBL::Variation::PopulationGenotype CVS Revision: 1.5
**
** @nam2rule Gvpopulationgenotype
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Population Genotype
** by pointer. It is the responsibility of the user to first destroy any
** previous Genetic Variation Population Genotype. The target pointer does not
** need to be initialised to NULL, but it is good programming practice to do
**  so anyway.
**
** @fdata [EnsPGvpopulationgenotype]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                Population Genotype
** @argrule Ref object [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                Population Genotype
**
** @valrule * [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                       Population Genotype
**
** @fcategory new
******************************************************************************/




/* @func ensGvpopulationgenotypeNew *******************************************
**
** Default constructor for an Ensembl Genetic Variation Population Genotype.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPGvpopulationgenotypeadaptor] Ensembl Genetic
**                                               Variation Population Genotype
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::PopulationGenotype::new
** @param [uN] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [uN] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [uN] allele1 [AjPStr] Allele 1
** @param [uN] allele2 [AjPStr] Allele 2
** @param [r] frequency [float] Frequency
**
** @return [EnsPGvconsequence] Ensembl Genetic Variation Consequence
** @@
******************************************************************************/

EnsPGvpopulationgenotype ensGvpopulationgenotypeNew(
    EnsPGvpopulationgenotypeadaptor gvpga,
    ajuint identifier,
    EnsPGvpopulation gvp,
    EnsPGvvariation gvv,
    AjPStr allele1,
    AjPStr allele2,
    float frequency)
{
    EnsPGvpopulationgenotype gvpg = NULL;

    AJNEW0(gvpg);

    gvpg->Use = 1;

    gvpg->Adaptor = gvpga;

    gvpg->Identifier = identifier;

    gvpg->Gvpopulation = ensGvpopulationNewRef(gvp);

    gvpg->Gvvariation = ensGvvariationNewRef(gvv);

    if(allele1)
        gvpg->Allele1 = ajStrNewRef(allele1);

    if(allele2)
        gvpg->Allele2 = ajStrNewRef(allele2);

    gvpg->Frequency = frequency;

    return gvpg;
}




/* @func ensGvpoulationgenotypeNewObj *****************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvpopulationgenotype] Ensembl Genetic
**                                               Variation Population Genotype
**
** @return [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                 Population Genotype or NULL
** @@
******************************************************************************/

EnsPGvpopulationgenotype ensGvpopulationgenotypeNewObj(
    const EnsPGvpopulationgenotype object)
{
    EnsPGvpopulationgenotype gvpg = NULL;

    if(!object)
        return NULL;

    AJNEW0(gvpg);

    gvpg->Use = 1;

    gvpg->Identifier = object->Identifier;

    gvpg->Adaptor = object->Adaptor;

    gvpg->Gvpopulation = ensGvpopulationNewRef(object->Gvpopulation);

    gvpg->Gvvariation = ensGvvariationNewRef(object->Gvvariation);

    if(object->Allele1)
        gvpg->Allele1 = ajStrNewRef(object->Allele1);

    if(object->Allele2)
        gvpg->Allele2 = ajStrNewRef(object->Allele2);

    gvpg->Frequency = object->Frequency;

    return gvpg;
}




/* @func ensGvpoulationgenotypeNewRef *****************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
**
** @return [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                    Population Genotype
** @@
******************************************************************************/

EnsPGvpopulationgenotype ensGvpopulationgenotypeNewRef(
    EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return NULL;

    gvpg->Use++;

    return gvpg;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Population Genotype.
**
** @fdata [EnsPGvpopulationgenotype]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Population Genotype object
**
** @argrule * Pgvpg [EnsPGvpopulationgenotype*] Genetic Variation
**                                              Population Genotype
**                                              object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvpopulationgenotypeDel *******************************************
**
** Default destructor for an Ensembl Genetic Variation Population Genotype.
**
** @param [d] Pgvpg [EnsPGvpopulationgenotype*] Ensembl Genetic Variation
**                                              Population Genotype address
**
** @return [void]
** @@
******************************************************************************/

void ensGvpopulationgenotypeDel(EnsPGvpopulationgenotype *Pgvpg)
{
    EnsPGvpopulationgenotype pthis = NULL;

    if(!Pgvpg)
        return;

    if(!*Pgvpg)
        return;

    if(ajDebugTest("ensGvpopulationgenotypeDel"))
    {
        ajDebug("ensGvpopulationgenotypeDel\n"
                "  *Pgvpg %p\n",
                *Pgvpg);

        ensGvpopulationgenotypeTrace(*Pgvpg, 1);
    }

    pthis = *Pgvpg;

    pthis->Use--;

    if(pthis->Use)
    {
        *Pgvpg = NULL;

        return;
    }

    ensGvpopulationDel(&pthis->Gvpopulation);

    ensGvvariationDel(&pthis->Gvvariation);

    ajStrDel(&pthis->Allele1);
    ajStrDel(&pthis->Allele2);

    AJFREE(pthis);

    *Pgvpg = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Population Genotype object.
**
** @fdata [EnsPGvpopulationgenotype]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Population Genotype attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation
**                      Population Genotype Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGvpopulation Return the Ensembl Genetic Variation Population
** @nam4rule GetGvvariation Return the Ensembl Genetic Variation Variation
** @nam4rule GetAllele1 Return the allele 1
** @nam4rule GetAllele2 Return the allele 2
** @nam4rule GetSubidentifier Return the sub-identifier
** @nam4rule GetFrequency Return the frequency
**
** @argrule * gvpg [const EnsPGvpopulationgenotype] Genetic Variation
**                                                  Population Genotype
**
** @valrule Adaptor [EnsPGvpopulationgenotypeadaptor] Ensembl Genetic Variation
**                                                  Population Genotype Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Gvpopulation [EnsPGvpopulation] Ensembl Genetic Variation
**                                          Population
** @valrule Gvvariation [EnsPGvvariation] Ensembl Genetic Variation Variation
** @valrule Allele1 [AjPStr] Allele 1
** @valrule Allele2 [AjPStr] Allele 2
** @valrule Subidentifier [ajuint] Sub-Identifier
** @valrule Frequency [float] Frequency
**
** @fcategory use
******************************************************************************/




/* @func ensGvpopulationgenotypeGetAdaptor ************************************
**
** Get the Ensembl Genetic Variation Population Genotype Adaptor element of an
** Ensembl Genetic Variation Population Genotype.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [EnsPGvpopulationgenotypeadaptor] Ensembl Genetic Variation
**                                           Population Genotype Adaptor
** @@
******************************************************************************/

EnsPGvpopulationgenotypeadaptor ensGvpopulationgenotypeGetAdaptor(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return NULL;

    return gvpg->Adaptor;
}




/* @func ensGvpoulationgenotypeGetIdentifier **********************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Population Genotype.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvpopulationgenotypeGetIdentifier(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return 0;

    return gvpg->Identifier;
}




/* @func ensGvpopulationgenotypeGetGvpopulation *******************************
**
** Get the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [EnsPGvpopulation] Ensembl Genetic Variation Population or NULL
** @@
******************************************************************************/

EnsPGvpopulation ensGvpopulationgenotypeGetGvpopulation(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return NULL;

    return gvpg->Gvpopulation;
}




/* @func ensGvpopulationgenotypeGetGvvariation ********************************
**
** Get the Ensembl Genetic Variation Variation element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation or NULL
** @@
******************************************************************************/

EnsPGvvariation ensGvpopulationgenotypeGetGvvariation(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return NULL;

    return gvpg->Gvvariation;
}




/* @func ensGvpopulationgenotypeGetAllele1 ************************************
**
** Get the allele1 element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [AjPStr] Allele1 or NULL
** @@
******************************************************************************/

AjPStr ensGvpopulationgenotypeGetAllele1(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return NULL;

    return gvpg->Allele1;
}




/* @func ensGvpopulationgenotypeGetAllele2 ************************************
**
** Get the allele2 element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [AjPStr] Allele2 or NULL
** @@
******************************************************************************/

AjPStr ensGvpopulationgenotypeGetAllele2(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return NULL;

    return gvpg->Allele2;
}




/* @func ensGvpopulationgenotypeGetSubidentifier ******************************
**
** Get the sub-identifier element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [ajuint] Sub-identifier or 0
** @@
******************************************************************************/

ajuint ensGvpopulationgenotypeGetSubidentifier(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return 0;

    return gvpg->Subidentifier;
}




/* @func ensGvpopulationgenotypeGetFrequency **********************************
**
** Get the frequency element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [float] Frequency or 0.0
** @@
******************************************************************************/

float ensGvpopulationgenotypeGetFrequency(
    const EnsPGvpopulationgenotype gvpg)
{
    if(!gvpg)
        return 0.0;

    return gvpg->Frequency;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Population Genotype object.
**
** @fdata [EnsPGvpopulationgenotype]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Population Genotype
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation
**                      Population Genotype Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetGvpopulation Set the Ensembl Gentic Variation Population
** @nam4rule SetGvvariation Set the Ensembl Genetic Variation Variation
** @nam4rule SetAllele1 Set the allele 1
** @nam4rule SetAllele2 Set the allele 2
**
** @argrule * gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvpopulationgenotypeSetAdaptor ************************************
**
** Set the Ensembl Genetic Variation Population Genotype Adaptor element of an
** Ensembl Genetic Variation Population Genotype.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population
** @param [r] gvpga [EnsPGvpopulationgenotypeadaptor] Ensembl Genetic Variation
**                                                  Population Genotype Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetAdaptor(EnsPGvpopulationgenotype gvpg,
                                         EnsPGvpopulationgenotypeadaptor gvpga)
{
    if(!gvpg)
        return ajFalse;

    gvpg->Adaptor = gvpga;

    return ajTrue;
}




/* @func ensGvpopulationgenotypeSetIdentifier *********************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Population Genotype.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetIdentifier(EnsPGvpopulationgenotype gvpg,
                                            ajuint identifier)
{
    if(!gvpg)
        return ajFalse;

    gvpg->Identifier = identifier;

    return ajTrue;
}




/* @func ensGvpopulationgenotypeSetGvpopulation *******************************
**
** Set the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetGvpopulation(EnsPGvpopulationgenotype gvpg,
                                              EnsPGvpopulation gvp)
{
    if(!gvpg)
        return ajFalse;

    ensGvpopulationDel(&gvpg->Gvpopulation);

    gvpg->Gvpopulation = ensGvpopulationNewRef(gvp);

    return ajTrue;
}




/* @func ensGvpopulationgenotypeSetGvvariation ********************************
**
** Set the Ensembl Genetic Variation Variation element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetGvvariation(EnsPGvpopulationgenotype gvpg,
                                             EnsPGvvariation gvv)
{
    if(!gvpg)
        return ajFalse;

    ensGvvariationDel(&gvpg->Gvvariation);

    gvpg->Gvvariation = ensGvvariationNewRef(gvv);

    return ajTrue;
}




/* @func ensGvpopulationgenotypeSetAllele1 ************************************
**
** Set the allele 1 element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
** @param [u] allele1 [AjPStr] Allele 1
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetAllele1(EnsPGvpopulationgenotype gvpg,
                                         AjPStr allele1)
{
    if(!gvpg)
        return ajFalse;

    ajStrDel(&gvpg->Allele1);

    gvpg->Allele1 = ajStrNewRef(allele1);

    return ajTrue;
}




/* @func ensGvpopulationgenotypeSetAllele2 ************************************
**
** Set the allele 2 element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
** @param [u] allele2 [AjPStr] Allele 2
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetAllele2(EnsPGvpopulationgenotype gvpg,
                                         AjPStr allele2)
{
    if(!gvpg)
        return ajFalse;

    ajStrDel(&gvpg->Allele2);

    gvpg->Allele2 = ajStrNewRef(allele2);

    return ajTrue;
}




/* @func ensGvpopulationgenotypeSetSubidentifier ******************************
**
** Set the sub-identifier element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
** @param [r] subidentifier [ajuint] Sub-identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetSubidentifier(EnsPGvpopulationgenotype gvpg,
                                               ajuint subidentifier)
{
    if(!gvpg)
        return ajFalse;

    gvpg->Subidentifier = subidentifier;

    return ajTrue;
}




/* @func ensGvpopulationgenotypeSetFrequency **********************************
**
** Set the frequency element of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [u] gvpg [EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                            Population Genotype
** @param [r] frequency [float] Frequency
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeSetFrequency(EnsPGvpopulationgenotype gvpg,
                                           float frequency)
{
    if(!gvpg)
        return ajFalse;

    gvpg->Frequency = frequency;

    return ajTrue;
}




/* @func ensGvpopulationgenotypeGetMemsize ************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
**
** @return [ajulong] Memory size
** @@
******************************************************************************/

ajulong ensGvpopulationgenotypeGetMemsize(const EnsPGvpopulationgenotype gvpg)
{
    ajulong size = 0;

    if(!gvpg)
        return 0;

    size += sizeof (EnsOGvpopulationgenotype);

    size += ensGvpopulationGetMemsize(gvpg->Gvpopulation);

    size += ensGvvariationGetMemsize(gvpg->Gvvariation);

    if(gvpg->Allele1)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvpg->Allele1);
    }

    if(gvpg->Allele2)
    {
        size += sizeof (AjOStr);

        size += ajStrGetRes(gvpg->Allele2);
    }

    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an
** Ensembl Genetic Variation Population Genotype object.
**
** @fdata [EnsPGvpopulationgenotype]
** @nam3rule Trace Report Ensembl Genetic Variation Population Genotype
**                 elements to debug file
**
** @argrule Trace gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic
**                                               Variation Population Genotype
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvpopulationgenotypeTrace *****************************************
**
** Trace an Ensembl Genetic Variation Population Genotype.
**
** @param [r] gvpg [const EnsPGvpopulationgenotype] Ensembl Genetic Variation
**                                                  Population Genotype
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationgenotypeTrace(const EnsPGvpopulationgenotype gvpg,
                                    ajuint level)
{
    AjPStr indent = NULL;

    if(!gvpg)
        return ajFalse;

    indent = ajStrNew();

    ajStrAppendCountK(&indent, ' ', level * 2);

    ajDebug("%SensGvpopulationgenotypeTrace %p\n"
            "%S  Use %u\n"
            "%S  Identifier %u\n"
            "%S  Adaptor %p\n"
            "%S  Gvpopulation %p\n"
            "%S  Gvvariation %p\n"
            "%S  Allele1 '%S'\n"
            "%S  Allele2 '%S'\n"
            "%S  Subidentifier %u\n"
            "%S  Frequency %f\n",
            indent, gvpg,
            indent, gvpg->Use,
            indent, gvpg->Identifier,
            indent, gvpg->Adaptor,
            indent, gvpg->Gvpopulation,
            indent, gvpg->Gvvariation,
            indent, gvpg->Allele1,
            indent, gvpg->Allele2,
            indent, gvpg->Subidentifier,
            indent, gvpg->Frequency);

    ensGvpopulationTrace(gvpg->Gvpopulation, level + 1);

    ensGvvariationTrace(gvpg->Gvvariation, level + 1);

    ajStrDel(&indent);

    return ajTrue;
}




/* @datasection [EnsPGvpopulationgenotypeadaptor] Genetic Variation Population
**                                                Genotype Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Population Genotype
** Adaptor objects
**
** @cc Bio::EnsEMBL::Variation::DBSQL::PopulationGenotypeAdaptor
**     CVS Revision: 1.10
**
** @nam2rule Gvpopulationgenotypeadaptor
**
******************************************************************************/

static const char *gvpopulationgenotypeadaptorTables[] =
{
    "population_genotype",
    NULL
};

static const char *gvpopulationgenotypeadaptorColumns[] =
{
    "population_genotype.population_genotype_id",
    "population_genotype.sample_id",
    "population_genotype.variation_id",
    "population_genotype.allele_1",
    "population_genotype.allele_2",
    "population_genotype.subsnp_id",
    "population_genotype.frequency",
    NULL
};

static EnsOBaseadaptorLeftJoin gvpopulationgenotypeadaptorLeftJoin[] =
{
    {NULL, NULL}
};

static const char *gvpopulationgenotypeadaptorDefaultCondition = NULL;

static const char *gvpopulationgenotypeadaptorFinalCondition = NULL;




/* @funcstatic gypopulationgenotypeadaptorClearGvpstogvpgs ********************
**
** An ajTableMapDel 'apply' function to clear the AJAX Table of Ensembl Genetic
** Variation Population identifier to Ensembl Genetic Variation Population
** Genotype objects.
** This function deletes the AJAX unsigned integer identifier key and the
** AJAX List objects of Ensembl Genetic Variation Population Genotype objects.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] AJAX Lists of Ensembl Genetic Variation
**                           Population Genotype objects
** @param [u] cl [void*] AJAX Table of Ensembl Genetic Variation Population
**                       objects, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gypopulationgenotypeadaptorClearGvpstogvpgs(void **key,
                                                        void **value,
                                                        void *cl)
{
    EnsPGvpopulation gvp = NULL;

    EnsPGvpopulationgenotype gvpg = NULL;

    if(!key)
        return;

    if(!*key)
        return;

    if(!value)
        return;

    if(!*value)
        return;

    if(!cl)
        return;

    gvp = (EnsPGvpopulation) ajTableFetch(cl, key);

    /*
    ** The Ensembl Genetic Variation Population Genotype objects can be
    ** deleted after associating them with Ensembl Genetic Variation
    ** Population objects, because this AJAX Table holds independent
    ** references for these objects.
    */

    while(ajListPop(*((AjPList *) value), (void **) &gvpg))
    {
        ensGvpopulationgenotypeSetGvpopulation(gvpg, gvp);

        ensGvpopulationgenotypeDel(&gvpg);
    }

    AJFREE(*key);

    ajListFree((AjPList *) value);

    *key   = NULL;
    *value = NULL;

    return;
}




/* @funcstatic gypopulationgenotypeadaptorClearGvvstogvpgs ********************
**
** An ajTableMapDel 'apply' function to clear the AJAX Table of Ensembl Genetic
** Variation Variation identifier to Ensembl Genetic Variation Population
** Genotype objects.
** This function deletes the AJAX unsigned integer identifier key and the
** AJAX List objects of Ensembl Genetic Variation Population Genotype objects.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] AJAX Lists of Ensembl Genetic Variation
**                           Population Genotype objects
** @param [u] cl [void*] AJAX Table of Ensembl Genetic Variation Variation
**                       objects, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
** @@
******************************************************************************/

static void gypopulationgenotypeadaptorClearGvvstogvpgs(void **key,
                                                        void **value,
                                                        void *cl)
{
    EnsPGvvariation gvv = NULL;

    EnsPGvpopulationgenotype gvpg = NULL;

    if(!key)
        return;

    if(!*key)
        return;

    if(!value)
        return;

    if(!*value)
        return;

    if(!cl)
        return;

    gvv = (EnsPGvvariation) ajTableFetch(cl, key);

    /*
    ** The Ensembl Genetic Variation Population Genotype objects can be
    ** deleted after associating them with Ensembl Genetic Variation
    ** Variation objects, because this AJAX Table holds independent
    ** references for these objects.
    */

    while(ajListPop(*((AjPList *) value), (void **) &gvpg))
    {
        ensGvpopulationgenotypeSetGvvariation(gvpg, gvv);

        ensGvpopulationgenotypeDel(&gvpg);
    }

    AJFREE(*key);

    ajListFree((AjPList *) value);

    *key   = NULL;
    *value = NULL;

    return;
}




/* @funcstatic gvpopulationgenotypeadaptorFetchAllBySQL ***********************
**
** Fetch all Ensembl Genetic Variation Population Genotype objects via an
** SQL statement.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::PopulationGenotypeAdaptor::
**     _objs_from_sth
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [uN] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [uN] slice [EnsPSlice] Ensembl Slice
** @param [u] gvpgs [AjPList] AJAX List of Ensembl Genetic Variation
**                            Population Genotype objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvpopulationgenotypeadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                       const AjPStr statement,
                                                       EnsPAssemblymapper am,
                                                       EnsPSlice slice,
                                                       AjPList gvpgs)
{
    float frequency = 0.0;

    ajuint *Pidentifier = NULL;

    ajuint identifier    = 0;
    ajuint gvvidentifier = 0;
    ajuint gvsidentifier = 0;
    ajuint subidentifier = 0;

    AjPList list = NULL;

    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;

    AjPStr allele1 = NULL;
    AjPStr allele2 = NULL;

    AjPTable gvps        = NULL;
    AjPTable gvpstogvpgs = NULL;
    AjPTable gvvs        = NULL;
    AjPTable gvvstogvpgs = NULL;

    EnsPGvpopulationadaptor gvpa = NULL;

    EnsPGvpopulationgenotype        gvpg  = NULL;
    EnsPGvpopulationgenotypeadaptor gvpga = NULL;

    EnsPGvvariationadaptor gvva = NULL;

    if(ajDebugTest("gvpopulationgenotypeadaptorFetchAllBySQL"))
        ajDebug("gvpopulationgenotypeadaptorFetchAllBySQL\n"
                "  dba %p\n"
                "  statement %p\n"
                "  am %p\n"
                "  slice %p\n"
                "  gvpgs %p\n",
                dba,
                statement,
                am,
                slice,
                gvpgs);

    if(!dba)
        return ajFalse;

    if(!statement)
        return ajFalse;

    if(!gvpgs)
        return ajFalse;

    gvps        = MENSTABLEUINTNEW(0);
    gvpstogvpgs = MENSTABLEUINTNEW(0);
    gvvs        = MENSTABLEUINTNEW(0);
    gvvstogvpgs = MENSTABLEUINTNEW(0);

    gvpga = ensRegistryGetGvpopulationgenotypeadaptor(dba);

    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);

    sqli = ajSqlrowiterNew(sqls);

    while(!ajSqlrowiterDone(sqli))
    {
        identifier    = 0;
        gvsidentifier = 0;
        gvvidentifier = 0;
        allele1       = ajStrNew();
        allele2       = ajStrNew();
        subidentifier = 0;
        frequency     = 0.0;

        sqlr = ajSqlrowiterGet(sqli);

        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToUint(sqlr, &gvsidentifier);
        ajSqlcolumnToUint(sqlr, &gvvidentifier);
        ajSqlcolumnToStr(sqlr, &allele1);
        ajSqlcolumnToStr(sqlr, &allele2);
        ajSqlcolumnToUint(sqlr, &subidentifier);
        ajSqlcolumnToFloat(sqlr, &frequency);

        gvpg = ensGvpopulationgenotypeNew(gvpga,
                                          identifier,
                                          (EnsPGvpopulation) NULL,
                                          (EnsPGvvariation) NULL,
                                          allele1,
                                          allele2,
                                          frequency);

        ajListPushAppend(gvpgs, (void *) gvpg);

        /*
        ** Populate two AJAX Tables to fetch Ensembl Genetic Variation
        ** Population objects from the database and associate them with
        ** Ensembl Genetic Variation Population Genotyope objects.
        **
        ** gvps
        **   key data:   Ensembl Genetic Variation Population (or Sample)
        **               identifiers
        **   value data: Ensembl Genetic Variation Population objects
        **               fetched by ensGvpopulationadaptorFetchAllByIdentifiers
        **
        ** gvpstogvpgs
        **   key data:   Ensembl Genetic Variation Population (or Sample)
        **               identifiers
        **   value data: AJAX List objects of Ensembl Genetic Variation
        **               Population Genotype objects that need to be associated
        **               with Ensembl Genetic Variation Population objects once
        **               they have been fetched from the database
        */

        if(!ajTableFetch(gvps, (const void *) &gvsidentifier))
        {
            AJNEW0(Pidentifier);

            *Pidentifier = gvsidentifier;

            ajTablePut(gvps, (void *) Pidentifier, (void *) NULL);
        }

        if(!ajTableFetch(gvpstogvpgs, (const void *) &gvsidentifier))
        {
            AJNEW0(Pidentifier);

            *Pidentifier = gvsidentifier;

            list = ajListNew();

            ajTablePut(gvpstogvpgs,
                       (void *) Pidentifier,
                       (void *) list);
        }

        ajListPushAppend(list, (void *) ensGvpopulationgenotypeNewRef(gvpg));

        /*
        ** Populate two AJAX Tables to fetch Ensembl Genetic Variation
        ** Variation objects from the database and associate them with
        ** Ensembl Genetic Variation Population Genotyope objects.
        **
        ** gvvs
        **   key data:   Ensembl Genetic Variation Variation identifiers
        **   value data: Ensembl Genetic Variation Variation objects
        **               fetched by ensGvvariationadaptorFetchAllByIdentifiers
        **
        ** gvvstogvpgs
        **   key data:   Ensembl Genetic Variation Variation identifiers
        **   value data: AJAX List objects of Ensembl Genetic Variation
        **               Population Genotype objects that need to be associated
        **               with Ensembl Genetic Variation Variation objects once
        **               they have been fetched from the database
        */

        if(!ajTableFetch(gvvs, (const void *) &gvvidentifier))
        {
            AJNEW0(Pidentifier);

            *Pidentifier = gvvidentifier;

            ajTablePut(gvvs, (void *) Pidentifier, (void *) NULL);
        }

        if(!ajTableFetch(gvvstogvpgs, (const void *) &gvvidentifier))
        {
            AJNEW0(Pidentifier);

            *Pidentifier = gvvidentifier;

            list = ajListNew();

            ajTablePut(gvvstogvpgs, (void *) Pidentifier, (void *) list);
        }

        ajListPushAppend(list, (void *) ensGvpopulationgenotypeNewRef(gvpg));

        ajStrDel(&allele1);
        ajStrDel(&allele2);
    }

    ajSqlrowiterDel(&sqli);

    ensDatabaseadaptorSqlstatementDel(dba, &sqls);

    /* Fetch the Ensembl Genetic Variation Population objects. */

    gvpa = ensRegistryGetGvpopulationadaptor(dba);

    ensGvpopulationadaptorFetchAllByIdentifiers(gvpa, gvps);

    /* Fetch the Ensembl Genetic Variation Variation objects. */

    gvva = ensRegistryGetGvvariationadaptor(dba);

    ensGvvariationadaptorFetchAllByIdentifiers(gvva, gvvs);

    /*
    ** Associate
    ** Ensembl Genetic Variation Population objects with
    ** Ensembl Genetic Variation Population Genotype objects.
    */

    ajTableMapDel(gvpstogvpgs,
                  gypopulationgenotypeadaptorClearGvpstogvpgs,
                  (void *) gvps);

    ajTableFree(&gvpstogvpgs);

    /*
    ** Associate
    ** Ensembl Genetic Variation Variation objects with
    ** Ensembl Genetic Variation Population Genotype objects.
    */

    ajTableMapDel(gvvstogvpgs,
                  gypopulationgenotypeadaptorClearGvvstogvpgs,
                  (void *) gvvs);

    ajTableFree(&gvvstogvpgs);

    /* Delete the utility AJAX Table objects. */

    ensTableDeleteGvpopulations(&gvps);

    ensTableDeleteGvvariations(&gvvs);

    return ajTrue;
}



/* @func ensGvpopulationgenotypeadaptorNew ************************************
**
** Default constructor for an
** Ensembl Genetic Variation Population Genotype Adaptor.
**
** Ensembl Object Adaptors are singleton objects in the sense that a single
** instance of an Ensembl Object Adaptor connected to a particular database is
** sufficient to instantiate any number of Ensembl Objects from the database.
** Each Ensembl Object will have a weak reference to the Object Adaptor that
** instantiated it. Therefore, Ensembl Object Adaptors should not be
** instantiated directly, but rather obtained from the Ensembl Registry,
** which will in turn call this function if neccessary.
**
** @see ensRegistryGetDatabaseadaptor
** @see ensRegistryGetGvpopulationgenotypeadaptor
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationGenotypeAdaptor::new
** @param [u] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvpopulationgenotypeadaptor] Ensembl Genetic Variation
**                                           Population Genotype Adaptor
**                                           or NULL
** @@
******************************************************************************/

EnsPGvpopulationgenotypeadaptor ensGvpopulationgenotypeadaptorNew(
    EnsPDatabaseadaptor dba)
{
    if(!dba)
        return NULL;

    if(ajDebugTest("ensGvpopulationgenotypeadaptorNew"))
        ajDebug("ensGvpopulationgenotypeadaptorNew\n"
                "  dba %p\n",
                dba);

    return ensBaseadaptorNew(
        dba,
        gvpopulationgenotypeadaptorTables,
        gvpopulationgenotypeadaptorColumns,
        gvpopulationgenotypeadaptorLeftJoin,
        gvpopulationgenotypeadaptorDefaultCondition,
        gvpopulationgenotypeadaptorFinalCondition,
        gvpopulationgenotypeadaptorFetchAllBySQL);
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Population Genotype
** Adaptor.
**
** @fdata [EnsPGvpopulationgenotypeadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Genetic Variation
**                                 Population Genotype Adaptor object.
**
** @argrule * Pgvpga [EnsPGvpopulationgenotypeadaptor*] Ensembl Genetic
**                        Variation Population Genotype Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvpopulationgenotypeadaptorDel ************************************
**
** Default destructor for an
** Ensembl Genetic Variation Population Genotype Adaptor.
**
** Ensembl Object Adaptors are singleton objects that are registered in the
** Ensembl Registry and weakly referenced by Ensembl Objects that have been
** instantiated by it. Therefore, Ensembl Object Adaptors should never be
** destroyed directly. Upon exit, the Ensembl Registry will call this function
** if required.
**
** @param [d] Pgvpga [EnsPGvpopulationgenotypeadaptor*] Ensembl Genetic
**                               Variation Population Genotype Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvpopulationgenotypeadaptorDel(EnsPGvpopulationgenotypeadaptor* Pgvpga)
{
    EnsPGvpopulationgenotypeadaptor pthis = NULL;

    if(!Pgvpga)
        return;

    if(!*Pgvpga)
        return;

    pthis = *Pgvpga;

    ensBaseadaptorDel(&pthis);

    *Pgvpga = NULL;

    return;
}




