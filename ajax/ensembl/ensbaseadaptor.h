#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensbaseadaptor_h
#define ensbaseadaptor_h

#include "ensassemblymapper.h"
#include "ensdata.h"




/*
** Prototype definitions
*/

/* Ensembl Base Adaptor */

EnsPBaseAdaptor ensBaseAdaptorNew(
    EnsPDatabaseAdaptor dba,
    const char **Ptables,
    const char **Pcolumns,
    EnsOBaseAdaptorLeftJoin *left_join,
    const char *condition,
    const char *final,
    AjBool Fquery(EnsPDatabaseAdaptor dba,
                  const AjPStr statement,
                  EnsPAssemblyMapper am,
                  EnsPSlice slice,
                  AjPList objects));

void ensBaseAdaptorDel(EnsPBaseAdaptor *Pba);

EnsPDatabaseAdaptor ensBaseAdaptorGetDatabaseAdaptor(const EnsPBaseAdaptor ba);

const char **ensBaseAdaptorGetTables(const EnsPBaseAdaptor ba);

const char **ensBaseAdaptorGetColumns(const EnsPBaseAdaptor ba);

AjBool ensBaseAdaptorSetTables(EnsPBaseAdaptor ba, const char ** Ptables);

AjBool ensBaseAdaptorSetColumns(EnsPBaseAdaptor ba, const char ** Pcolumns);

AjBool ensBaseAdaptorSetDefaultCondition(EnsPBaseAdaptor ba,
                                         const char *condition);

AjBool ensBaseAdaptorSetFinalCondition(EnsPBaseAdaptor ba, const char *final);

const char *ensBaseAdaptorGetPrimaryTable(const EnsPBaseAdaptor ba);

AjBool ensBaseAdaptorGetMultiSpecies(const EnsPBaseAdaptor ba);

ajuint ensBaseAdaptorGetSpeciesIdentifier(const EnsPBaseAdaptor ba);

AjBool ensBaseAdaptorEscapeCS(EnsPBaseAdaptor ba, char **Ptxt,
                              const AjPStr str);

AjBool ensBaseAdaptorEscapeSS(EnsPBaseAdaptor ba, AjPStr *Pstr,
                              const AjPStr str);

AjBool ensBaseAdaptorGenericFetch(const EnsPBaseAdaptor ba,
                                  const AjPStr constraint,
                                  EnsPAssemblyMapper am,
                                  EnsPSlice slice,
                                  AjPList objects);

void *ensBaseAdaptorFetchByIdentifier(EnsPBaseAdaptor ba, ajuint identifier);

AjBool ensBaseAdaptorFetchAllByIdentifiers(EnsPBaseAdaptor ba,
                                           AjPList idlist,
                                           AjPList objects);

AjBool ensBaseAdaptorFetchAll(EnsPBaseAdaptor ba, AjPList objects);

AjBool ensBaseAdaptorFetchAllIdentifiers(EnsPBaseAdaptor ba,
				  AjPStr table,
				  AjPStr prikey,
				  AjPList idlist);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
