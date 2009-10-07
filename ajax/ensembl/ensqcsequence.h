#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensqcsequence_h
#define ensqcsequence_h

#include "ensqcdatabase.h"




/* @data EnsPQCSequenceadaptor ************************************************
**
** Ensembl QC Sequence Adaptor
**
** @alias EnsSQCSequenceadaptor
** @alias EnsOQCSequenceadaptor
**
** @attr Adaptor [EnsPBaseadaptor] Ensembl Base Adaptor
** @attr CacheByIdentifier [AjPTable] Identifier cache
** @attr CacheByName [AjPTable] Name cache
** @@
******************************************************************************/

typedef struct EnsSQCSequenceadaptor
{
    EnsPBaseadaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByName;
    /*
    ** FIXME: Should this also have a CacheByAccessionVersion???
    */
} EnsOQCSequenceadaptor;

#define EnsPQCSequenceadaptor EnsOQCSequenceadaptor*




/* @data EnsPQCSequence *******************************************************
**
** Ensembl QC Sequence
**
** @alias EnsSQCSequence
** @alias EnsOQCSequence
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQCSequenceadaptor] Ensembl Sequence Adaptor
** @cc Bio::EnsEMBL::QC::Sequence
** @cc 'sequence' SQL table
** @attr QCDatabase [EnsPQCDatabase] Ensembl QC Database
** @attr Name [AjPStr] Name
** @attr Accession [AjPStr] Accession
** @attr Description [AjPStr] Description
** @attr Type [AjPStr] Type
** @attr Version [ajuint] Version
** @attr Length [ajuint] Length
** @attr CDSStart [ajuint] Coding sequence start
** @attr CDSEnd [ajuint] Coding sequence end
** @attr CDSStrand [ajint] Coding sequence strand
** @attr PolyA [ajuint] Poly A+ tail length
** @@
******************************************************************************/

typedef struct EnsSQCSequence
{
    ajuint Use;
    ajuint Identifier;
    EnsPQCSequenceadaptor Adaptor;
    EnsPQCDatabase QCDatabase;
    AjPStr Name;
    AjPStr Accession;
    AjPStr Description;
    AjPStr Type;
    ajuint Version;
    ajuint Length;
    ajuint CDSStart;
    ajuint CDSEnd;
    ajint CDSStrand;
    ajuint PolyA;
} EnsOQCSequence;

#define EnsPQCSequence EnsOQCSequence*




/*
** Prototype definitions
*/

/* Ensembl Quality Check Sequence */

EnsPQCSequence ensQCSequenceNew(EnsPQCSequenceadaptor adaptor,
                                ajuint identifier,
                                EnsPQCDatabase qcdb,
                                AjPStr name,
                                AjPStr accession,
                                ajuint version,
                                AjPStr type,
                                ajuint length,
                                ajuint cdsstart,
                                ajuint cdsend,
                                ajint cdsstrand,
                                ajuint polya,
                                AjPStr description);

EnsPQCSequence ensQCSequenceNewObj(const EnsPQCSequence object);

EnsPQCSequence ensQCSequenceNewRef(EnsPQCSequence qcs);

void ensQCSequenceDel(EnsPQCSequence* Pqcs);

EnsPQCSequenceadaptor ensQCSequenceGetadaptor(const EnsPQCSequence qcs);

ajuint ensQCSequenceGetIdentifier(const EnsPQCSequence qcs);

EnsPQCDatabase ensQCSequenceGetQCDatabase(const EnsPQCSequence qcs);

AjPStr ensQCSequenceGetName(const EnsPQCSequence qcs);

AjPStr ensQCSequenceGetAccession(const EnsPQCSequence qcs);

ajuint ensQCSequenceGetVersion(const EnsPQCSequence qcs);

AjPStr ensQCSequenceGetType(const EnsPQCSequence qcs);

ajuint ensQCSequenceGetLength(const EnsPQCSequence qcs);

ajuint ensQCSequenceGetCDSStart(const EnsPQCSequence qcs);

ajuint ensQCSequenceGetCDSEnd(const EnsPQCSequence qcs);

ajint ensQCSequenceGetCDSStrand(const EnsPQCSequence qcs);

ajuint ensQCSequenceGetPolyA(const EnsPQCSequence qcs);

AjPStr ensQCSequenceGetDescription(const EnsPQCSequence qcs);

AjBool ensQCSequenceSetadaptor(EnsPQCSequence qcs, EnsPQCSequenceadaptor qcsa);

AjBool ensQCSequenceSetIdentifier(EnsPQCSequence qcs, ajuint identifier);

AjBool ensQCSequenceSetQCDatabase(EnsPQCSequence qcs, EnsPQCDatabase qcdb);

AjBool ensQCSequenceSetName(EnsPQCSequence qcs, AjPStr name);

AjBool ensQCSequenceSetAccession(EnsPQCSequence qcs, AjPStr accession);

AjBool ensQCSequenceSetVersion(EnsPQCSequence qcs, ajuint version);

AjBool ensQCSequenceSetType(EnsPQCSequence qcs, AjPStr type);

AjBool ensQCSequenceSetLength(EnsPQCSequence qcs, ajuint length);

AjBool ensQCSequenceSetCDSStart(EnsPQCSequence qcs, ajuint cdsstart);

AjBool ensQCSequenceSetCDSEnd(EnsPQCSequence qcs, ajuint cdsend);

AjBool ensQCSequenceSetCDSStrand(EnsPQCSequence qcs, ajint cdsstrand);

AjBool ensQCSequenceSetPolyA(EnsPQCSequence qcs, ajuint polya);

AjBool ensQCSequenceSetDescription(EnsPQCSequence qcs, AjPStr description);

ajuint ensQCSequenceGetMemSize(const EnsPQCSequence qcs);

AjBool ensQCSequenceTrace(const EnsPQCSequence qcs, ajuint level);

AjBool ensQCSequenceMatch(const EnsPQCSequence qcs1, const EnsPQCSequence qcs2);

ajuint ensQCSequenceGetQCDatabaseIdentifier(const EnsPQCSequence qcs);

AjBool ensHTMLEncodeSGMLID(AjPStr *Pstr);
/* FIXME: This function should move! */

AjBool ensHTMLEncodeEntities(AjPStr *Pstr);
/* FIXME: This function should move! */

AjBool ensQCSequenceFetchExternalURL(const EnsPQCSequence qcs, AjPStr *Pstr);

AjBool ensQCSequenceFetchExternalAnchor(EnsPQCSequence qcs,
                                        AjPStr *Pstr,
                                        AjBool htmlid);

AjBool ensQCSequenceFetchInternalAnchor(EnsPQCSequence qcs, AjPStr *Pstr);

/* Ensembl Quality Check Sequence Adaptor */

EnsPQCSequenceadaptor ensQCSequenceadaptorNew(EnsPDatabaseadaptor dba);

void ensQCSequenceadaptorDel(EnsPQCSequenceadaptor* Pqcsa);

AjBool ensQCSequenceadaptorFetchByIdentifier(EnsPQCSequenceadaptor adaptor,
                                             ajuint identifier,
                                             EnsPQCSequence *Pqcs);

AjBool ensQCSequenceadaptorFetchByAccession(EnsPQCSequenceadaptor qcsa,
                                            ajuint qcdbid,
                                            const AjPStr accession,
                                            EnsPQCSequence *Pqcs);

AjBool ensQCSequenceadaptorFetchByAccessionVersion(EnsPQCSequenceadaptor qcsa,
                                                   ajuint qcdbid,
                                                   const AjPStr accession,
                                                   ajuint version,
                                                   EnsPQCSequence *Pqcs);

AjBool ensQCSequenceadaptorFetchByName(EnsPQCSequenceadaptor qcsa,
                                       ajuint qcdbid,
                                       const AjPStr name,
                                       EnsPQCSequence *Pqcs);

AjBool ensQCSequenceadaptorFetchAllByQCDatabase(EnsPQCSequenceadaptor qcsa,
                                                EnsPQCDatabase qcdb,
                                                AjPList qcss);

AjBool ensQCSequenceadaptorStore(EnsPQCSequenceadaptor qcsa,
                                 EnsPQCSequence qcs);

AjBool ensQCSequenceadaptorUpdate(EnsPQCSequenceadaptor qcsa,
                                  const EnsPQCSequence qcs);

AjBool ensQCSequenceadaptorDelete(EnsPQCSequenceadaptor qcsa,
                                  const EnsPQCSequence qcs);


/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
