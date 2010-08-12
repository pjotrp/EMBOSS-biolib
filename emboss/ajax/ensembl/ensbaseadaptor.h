#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensbaseadaptor_h
#define ensbaseadaptor_h

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensassemblymapper.h"
#include "ensdata.h"




/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */

/* EnsMBaseadaptorMaximumIdentifiers ******************************************
**
** Limit the number of identifiers in SQL queries to chunks of maximum size.
** Ensure that the MySQL max_allowed_packet is not exceeded, which defaults
** to 1 MB, by splitting large queries into smaller queries of at most 256 KiB
** (32768 8-bit characters). Assuming a (generous) average identifier string
** length of 16, this means a maximum of 2048 identifiers in each statement.
**
******************************************************************************/

#define EnsMBaseadaptorMaximumIdentifiers 2048




/* ==================================================================== */
/* ========================== public data ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ======================= public functions =========================== */
/* ==================================================================== */

/*
** Prototype definitions
*/

/* Ensembl Base Adaptor */

EnsPBaseadaptor ensBaseadaptorNew(
    EnsPDatabaseadaptor dba,
    const char **Ptables,
    const char **Pcolumns,
    EnsOBaseadaptorLeftJoin *leftjoin,
    const char *condition,
    const char *final,
    AjBool Fquery(EnsPDatabaseadaptor dba,
                  const AjPStr statement,
                  EnsPAssemblymapper am,
                  EnsPSlice slice,
                  AjPList objects));

void ensBaseadaptorDel(EnsPBaseadaptor *Pba);

EnsPDatabaseadaptor ensBaseadaptorGetDatabaseadaptor(const EnsPBaseadaptor ba);

const char **ensBaseadaptorGetTables(const EnsPBaseadaptor ba);

const char **ensBaseadaptorGetColumns(const EnsPBaseadaptor ba);

AjBool ensBaseadaptorSetTables(EnsPBaseadaptor ba, const char ** Ptables);

AjBool ensBaseadaptorSetColumns(EnsPBaseadaptor ba, const char ** Pcolumns);

AjBool ensBaseadaptorSetDefaultCondition(EnsPBaseadaptor ba,
                                         const char *condition);

AjBool ensBaseadaptorSetFinalCondition(EnsPBaseadaptor ba, const char *final);

const char *ensBaseadaptorGetPrimaryTable(const EnsPBaseadaptor ba);

AjBool ensBaseadaptorGetMultiSpecies(const EnsPBaseadaptor ba);

ajuint ensBaseadaptorGetSpeciesIdentifier(const EnsPBaseadaptor ba);

AjBool ensBaseadaptorEscapeC(EnsPBaseadaptor ba, char **Ptxt,
                             const AjPStr str);

AjBool ensBaseadaptorEscapeS(EnsPBaseadaptor ba, AjPStr *Pstr,
                             const AjPStr str);

AjBool ensBaseadaptorGenericFetch(const EnsPBaseadaptor ba,
                                  const AjPStr constraint,
                                  EnsPAssemblymapper am,
                                  EnsPSlice slice,
                                  AjPList objects);

void *ensBaseadaptorFetchByIdentifier(const EnsPBaseadaptor ba,
                                      ajuint identifier);

AjBool ensBaseadaptorFetchAllByIdentifiers(const EnsPBaseadaptor ba,
                                           const AjPList identifiers,
                                           AjPList objects);

AjBool ensBaseadaptorFetchAll(const EnsPBaseadaptor ba, AjPList objects);

AjBool ensBaseadaptorFetchAllIdentifiers(const EnsPBaseadaptor ba,
                                         const AjPStr table,
                                         const AjPStr primary,
                                         AjPList identifiers);

AjBool ensBaseadaptorFetchAllStrings(const EnsPBaseadaptor ba,
                                     const AjPStr table,
                                     const AjPStr primary,
                                     AjPList strings);

/*
** End of prototype definitions
*/




#endif /* ensbaseadaptor_h */

#ifdef __cplusplus
}
#endif
