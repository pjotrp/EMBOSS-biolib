#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensdatabaseconnection_h
#define ensdatabaseconnection_h

#include "ajax.h"




/* @data EnsPDatabaseConnection ***********************************************
**
** Ensembl Database Connection.
**
** Holds information to connect to an SQL database.
**
** @alias EnsSDatabaseConnection
** @alias EnsODatabaseConnection
**
** @attr SqlConnection [AjPSqlConnection] AJAX SQL Connection
** @attr UserName [AjPStr] SQL user name
** @attr Password [AjPStr] SQL password
** @attr HostName [AjPStr] SQL host name
** @attr HostPort [AjPStr] SQL host port
** @attr Socket [AjPStr] SQL host UNIX socket
** @attr DatabaseName [AjPStr] SQL database name
** @attr SqlClientType [AjEnum] AJAX SQL client type
** @attr Use [ajuint] Use counter
**
** @@
******************************************************************************/

typedef struct EnsSDatabaseConnection
{
    AjPSqlConnection SqlConnection;
    AjPStr UserName;
    AjPStr Password;
    AjPStr HostName;
    AjPStr HostPort;
    AjPStr Socket;
    AjPStr DatabaseName;
    AjEnum SqlClientType;
    ajuint Use;
} EnsODatabaseConnection;

#define EnsPDatabaseConnection EnsODatabaseConnection*




/*
** Prototype definitions
*/

EnsPDatabaseConnection ensDatabaseConnectionNew(AjEnum client,
                                                AjPStr user,
                                                AjPStr password,
                                                AjPStr host,
                                                AjPStr port,
                                                AjPStr socket,
                                                AjPStr database);

EnsPDatabaseConnection ensDatabaseConnectionNewC(EnsPDatabaseConnection dbc,
                                                 AjPStr database);

EnsPDatabaseConnection ensDatabaseConnectionNewRef(EnsPDatabaseConnection dbc);

void ensDatabaseConnectionDel(EnsPDatabaseConnection* Pdbc);

AjPSqlConnection ensDatabaseConnectionGetSqlConnection(
    const EnsPDatabaseConnection dbc);

AjPStr ensDatabaseConnectionGetUserName(const EnsPDatabaseConnection dbc);

AjPStr ensDatabaseConnectionGetPassword(const EnsPDatabaseConnection dbc);

AjPStr ensDatabaseConnectionGetHostName(const EnsPDatabaseConnection dbc);

AjPStr ensDatabaseConnectionGetHostPort(const EnsPDatabaseConnection dbc);

AjPStr ensDatabaseConnectionGetSocket(const EnsPDatabaseConnection dbc);

AjPStr ensDatabaseConnectionGetDatabaseName(const EnsPDatabaseConnection dbc);

AjEnum ensDatabaseConnectionGetSqlClientType(const EnsPDatabaseConnection dbc);

AjBool ensDatabaseConnectionMatch(const EnsPDatabaseConnection dbc1,
                                  const EnsPDatabaseConnection dbc2);

AjBool ensDatabaseConnectionConnect(EnsPDatabaseConnection dbc);

void ensDatabaseConnectionDisconnect(EnsPDatabaseConnection dbc);

AjBool ensDatabaseConnectionIsConnected(const EnsPDatabaseConnection dbc);

AjPSqlStatement ensDatabaseConnectionSqlStatementNew(EnsPDatabaseConnection dbc,
                                                     const AjPStr statement);

AjBool ensDatabaseConnectionEscapeCS(EnsPDatabaseConnection dbc,
                                     char **Ptxt,
                                     const AjPStr str);

AjBool ensDatabaseConnectionEscapeSS(EnsPDatabaseConnection dbc,
                                     AjPStr *Pstr,
                                     const AjPStr str);

AjBool ensDatabaseConnectionTrace(const EnsPDatabaseConnection dbc,
                                  ajuint level);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
