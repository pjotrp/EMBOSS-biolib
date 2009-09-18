#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensattribute_h
#define ensattribute_h

#include "ensdata.h"




/*
** Prototype definitions
*/

/* Ensembl Attribute */

EnsPAttribute ensAttributeNew(AjPStr code, AjPStr name, AjPStr description,
                              AjPStr value);

EnsPAttribute ensAttributeNewObj(const EnsPAttribute object);

EnsPAttribute ensAttributeNewRef(EnsPAttribute attribute);

void ensAttributeDel(EnsPAttribute* Pattribute);

AjPStr ensAttributeGetCode(const EnsPAttribute attribute);

AjPStr ensAttributeGetName(const EnsPAttribute attribute);

AjPStr ensAttributeGetDescription(const EnsPAttribute attribute);

AjPStr ensAttributeGetValue(const EnsPAttribute attribute);

ajuint ensAttributeGetMemSize(const EnsPAttribute attribute);

AjBool ensAttributeTrace(const EnsPAttribute attribute, ajuint level);

/* Ensembl Attribute Adaptor */

AjBool ensAttributeAdaptorFetchAllByGene(EnsPDatabaseAdaptor adaptor,
                                         const EnsPGene gene,
                                         const AjPStr code,
                                         AjPList attributes);

AjBool ensAttributeAdaptorFetchAllBySeqRegion(EnsPDatabaseAdaptor adaptor,
                                              const EnsPSeqRegion sr,
                                              const AjPStr code,
                                              AjPList attributes);

AjBool ensAttributeAdaptorFetchAllBySlice(EnsPDatabaseAdaptor adaptor,
                                          const EnsPSlice slice,
                                          const AjPStr code,
                                          AjPList attributes);

AjBool ensAttributeAdaptorFetchAllByTranscript(EnsPDatabaseAdaptor adaptor,
                                               const EnsPTranscript transcript,
                                               const AjPStr code,
                                               AjPList attributes);

AjBool ensAttributeAdaptorFetchAllByTranslation(
    EnsPDatabaseAdaptor adaptor,
    const EnsPTranslation translation,
    const AjPStr code,
    AjPList attributes);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
