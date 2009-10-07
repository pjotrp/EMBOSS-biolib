#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensexternaldatabase_h
#define ensexternaldatabase_h

#include "ensdata.h"




/*
** Prototype definitions
*/

/* Ensembl External Database */

EnsPExternalDatabase ensExternalDatabaseNew(EnsPExternalDatabaseadaptor adaptor,
                                            ajuint identifier,
                                            AjPStr name,
                                            AjPStr release,
                                            AjPStr displayname,
                                            AjPStr secondaryname,
                                            AjPStr secondarytable,
                                            AjPStr description,
                                            AjBool primarylinkable,
                                            AjBool displaylinkable,
                                            AjEnum status,
                                            AjEnum type,
                                            ajint priority);

EnsPExternalDatabase ensExternalDatabaseNewObj(
    const EnsPExternalDatabase object);

EnsPExternalDatabase ensExternalDatabaseNewRef(EnsPExternalDatabase edb);

void ensExternalDatabaseDel(EnsPExternalDatabase *Pedb);

const EnsPExternalDatabaseadaptor ensExternalDatabaseGetadaptor(
    const EnsPExternalDatabase edb);

ajuint ensExternalDatabaseGetIdentifier(const EnsPExternalDatabase edb);

AjPStr ensExternalDatabaseGetName(const EnsPExternalDatabase edb);

AjPStr ensExternalDatabaseGetRelease(const EnsPExternalDatabase edb);

AjPStr ensExternalDatabaseGetSecondaryName(const EnsPExternalDatabase edb);

AjPStr ensExternalDatabaseGetSecondaryTable(const EnsPExternalDatabase edb);

AjPStr ensExternalDatabaseGetDescription(const EnsPExternalDatabase edb);

AjBool ensExternalDatabaseGetPrimaryIdIsLinkable(
    const EnsPExternalDatabase edb);

AjBool ensExternalDatabaseGetDisplayIdIsLinkable(
    const EnsPExternalDatabase edb);

AjEnum ensExternalDatabaseGetStatus(const EnsPExternalDatabase edb);

AjEnum ensExternalDatabaseGetType(const EnsPExternalDatabase edb);

ajint ensExternalDatabaseGetPriority(const EnsPExternalDatabase edb);

AjBool ensExternalDatabaseSetadaptor(EnsPExternalDatabase edb,
                                     EnsPExternalDatabaseadaptor adaptor);

AjBool ensExternalDatabaseSetIdentifier(EnsPExternalDatabase edb,
                                        ajuint identifier);

AjBool ensExternalDatabaseSetName(EnsPExternalDatabase edb, AjPStr name);

AjBool ensExternalDatabaseSetRelease(EnsPExternalDatabase edb, AjPStr release);

AjBool ensExternalDatabaseSetSecondaryName(EnsPExternalDatabase edb,
                                           AjPStr secondaryname);

AjBool ensExternalDatabaseSetSecondaryTable(EnsPExternalDatabase edb,
                                            AjPStr secondarytable);

AjBool ensExternalDatabaseSetDescription(EnsPExternalDatabase edb,
                                         AjPStr description);

AjBool ensExternalDatabaseSetPrimaryIdIsLinkable(EnsPExternalDatabase edb,
                                                  AjBool primarylinkable);

AjBool ensExternalDatabaseSetDisplayIdIsLinkable(EnsPExternalDatabase edb,
                                                 AjBool displaylinkable);

AjBool ensExternalDatabaseSetStatus(EnsPExternalDatabase edb, AjEnum status);

AjBool ensExternalDatabaseSetType(EnsPExternalDatabase edb, AjEnum type);

AjBool ensExternalDatabaseSetPriority(EnsPExternalDatabase edb, ajint priority);

AjBool ensExternalDatabaseTrace(const EnsPExternalDatabase edb, ajuint level);

AjEnum ensExternalDatabaseStatusFromStr(const AjPStr status);

AjEnum ensExternalDatabaseTypeFromStr(const AjPStr type);

const char *ensExternalDatabaseStatusToChar(const AjEnum status);

const char *ensExternalDatabaseTypeToChar(const AjEnum type);

ajuint ensExternalDatabaseGetMemSize(const EnsPExternalDatabase edb);

/* Ensembl External Database Adaptor */

EnsPExternalDatabaseadaptor ensExternalDatabaseadaptorNew(
    EnsPDatabaseadaptor dba);

void ensExternalDatabaseadaptorDel(EnsPExternalDatabaseadaptor* Pedba);

AjBool ensExternalDatabaseadaptorFetchByIdentifier(
    EnsPExternalDatabaseadaptor edba,
    ajuint identifier,
    EnsPExternalDatabase *Pedb);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
