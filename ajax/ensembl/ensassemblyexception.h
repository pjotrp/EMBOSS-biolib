#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensassemblyexception_h
#define ensassemblyexception_h

#include "ensdatabaseadaptor.h"
#include "enstable.h"




/* @data EnsPAssemblyExceptionadaptor *****************************************
**
** Ensembl Assembly Exception Adaptor.
**
** @alias EnsSAssemblyExceptionadaptor
** @alias EnsOAssemblyExceptionadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @attr CacheBySeqRegionIdentifier [AjPTable] Ensembl Assembly Exception cache
**                                             indexed on Ensembl Sequence
**                                             Region identifiers
** @@
******************************************************************************/

typedef struct EnsSAssemblyExceptionadaptor
{
    EnsPDatabaseadaptor Adaptor;
    AjPTable CacheBySeqRegionIdentifier;
} EnsOAssemblyExceptionadaptor;

#define EnsPAssemblyExceptionadaptor EnsOAssemblyExceptionadaptor*




/******************************************************************************
**
** Ensembl Assembly Exception Type enumeration.
**
******************************************************************************/

enum EnsEAssemblyExceptionType
{
    ensEAssemblyExceptionTypeNULL,
    ensEAssemblyExceptionTypeHAP,
    ensEAssemblyExceptionTypePAR,
    ensEAssemblyExceptionTypeHAPREF
};




/* @data EnsPAssemblyException ************************************************
**
** Ensembl Assembly Exception.
**
** An Ensembl Assembly Exception object encapsulates information about
** exceptions in the reference assembly path. Currently, the Ensembl system
** models two types of assembly exceptions.
**
** Haplotype (HAPs) assemblies represent alternative assembly paths of regions
** of increased genetic variablility.
** Pseudo-autosomal regions (PARs) are paired stretches at either tip of
** (mammalian) sex chromosomes where frequent chrossing over events occur.
** Due to the frequent exchange of genetic material those sequences represent
** exact copies of each other.
**
** @alias EnsSAssemblyException
** @alias EnsOAssemblyException
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPAssemblyExceptionadaptor] Ensembl Assembly
**                                              Exception Adaptor
** @cc Bio::EnsEMBL::AssemblyException
** @attr SeqRegionIdentifier [ajuint] Ensembl Sequence Region identifier
** @attr SeqRegionStart [ajuint] Ensembl Sequence Region start
** @attr SeqRegionEnd [ajuint] Ensembl Sequence Region end
** @attr ExcRegionIdentifier [ajuint] Ensembl Sequence Region identifier
** @attr ExcRegionStart [ajuint] Ensembl Sequence Region start
** @attr ExcRegionEnd [ajuint] Ensembl Sequence Region end
** @attr Orientation [ajint] Orientation
** @attr Type [AjEnum] Type
** @@
******************************************************************************/

typedef struct EnsSAssemblyException
{
    ajuint Use;
    ajuint Identifier;
    EnsPAssemblyExceptionadaptor Adaptor;
    ajuint SeqRegionIdentifier;
    ajuint SeqRegionStart;
    ajuint SeqRegionEnd;
    ajuint ExcRegionIdentifier;
    ajuint ExcRegionStart;
    ajuint ExcRegionEnd;
    ajint Orientation;
    AjEnum Type;
} EnsOAssemblyException;

#define EnsPAssemblyException EnsOAssemblyException*




/*
** Prototype definitions
*/

/* Ensembl Assembly Exception */

EnsPAssemblyException ensAssemblyExceptionNew(
                        EnsPAssemblyExceptionadaptor adaptor,
			ajuint identifier,
			ajuint srid,
			ajuint srstart,
			ajuint srend,
			ajuint exid,
			ajuint exstart,
			ajuint exend,
			ajint ori,
			AjEnum type);

EnsPAssemblyException ensAssemblyExceptionNewObj(
    const EnsPAssemblyException object);

EnsPAssemblyException ensAssemblyExceptionNewRef(EnsPAssemblyException ae);

void ensAssemblyExceptionDel(EnsPAssemblyException* Pae);

EnsPAssemblyExceptionadaptor ensAssemblyExceptionGetadaptor(
    const EnsPAssemblyException ae);

ajuint ensAssemblyExceptionGetIdentifier(const EnsPAssemblyException ae);

ajuint ensAssemblyExceptionGetSeqRegionIdentifier(
    const EnsPAssemblyException ae);

ajuint ensAssemblyExceptionGetSeqRegionStart(const EnsPAssemblyException ae);

ajuint ensAssemblyExceptionGetSeqRegionEnd(const EnsPAssemblyException ae);

ajuint ensAssemblyExceptionGetExcRegionIdentifier(
    const EnsPAssemblyException ae);

ajuint ensAssemblyExceptionGetExcRegionStart(const EnsPAssemblyException ae);

ajuint ensAssemblyExceptionGetExcRegionEnd(const EnsPAssemblyException ae);

ajint ensAssemblyExceptionGetOrientation(const EnsPAssemblyException ae);

AjEnum ensAssemblyExceptionGetType(const EnsPAssemblyException ae);

AjBool ensAssemblyExceptionSetadaptor(EnsPAssemblyException ae,
                                      EnsPAssemblyExceptionadaptor adaptor);

AjBool ensAssemblyExceptionSetIdentifier(EnsPAssemblyException ae,
                                         ajuint identifier);

AjBool ensAssemblyExceptionSetSeqRegionIdentifier(EnsPAssemblyException ae,
                                                  ajuint srid);

AjBool ensAssemblyExceptionSetSeqRegionStart(EnsPAssemblyException ae,
                                             ajuint srstart);

AjBool ensAssemblyExceptionSetSeqRegionEnd(EnsPAssemblyException ae,
                                           ajuint srend);

AjBool ensAssemblyExceptionSetExcRegionIdentifier(EnsPAssemblyException ae,
                                                  ajuint exid);

AjBool ensAssemblyExceptionSetEcxRegionStart(EnsPAssemblyException ae,
                                             ajuint exstart);

AjBool ensAssemblyExceptionSetExcRegionStart(EnsPAssemblyException ae,
                                             ajuint exend);

AjBool ensAssemblyExceptionSetOrientation(EnsPAssemblyException ae,
                                          ajint orientation);

AjBool ensAssemblyExceptionSetType(EnsPAssemblyException ae, AjEnum type);

AjBool ensAssemblyExceptionTrace(const EnsPAssemblyException ae, ajuint level);

ajuint ensAssemblyExceptionGetMemSize(const EnsPAssemblyException ae);

AjEnum ensAssemblyExceptionTypeFromStr(const AjPStr type);

const char* ensAssemblyExceptionTypeToChar(const AjEnum type);

/* Ensembl Assembly Exception Adaptor */

EnsPAssemblyExceptionadaptor ensAssemblyExceptionadaptorNew(
    EnsPDatabaseadaptor dba);

void ensAssemblyExceptionadaptorDel(EnsPAssemblyExceptionadaptor* Padaptor);

AjBool ensAssemblyExceptionadaptorFetchAll(
    const EnsPAssemblyExceptionadaptor adaptor,
    AjPList aes);

AjBool ensAssemblyExceptionadaptorFetchAllBySeqRegionIdentifier(
    EnsPAssemblyExceptionadaptor adaptor,
    ajuint srid,
    AjPList aes);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif

/*
** FIXME: The Ensembl Core SQL schema has some inconsistencies between the
** 'seq_region' and 'assembly_exception' tables.
** The 'assembly_exception.seq_region_start' and
** the 'assembly_exception.seq_region_end' fields are unsigned, while
** the 'seq_region.length' field is signed.
** The 'seq_region.length' field should really be unsigned.
**
** The 'assembly_exception.ori' field could be SMALLINT, but the table is
** small, so the gain of storage space would be minimal.
**
** FIXME: Ensembl core should move from TINYINT to SMALLINT as TINYINT is
** not part of the SQL:2003 standard.
*/
