/******************************************************************************
** @source AJAX SQL functions
**
** @author Copyright (C) 2006 Michael K. Schuster
** @version 1.0
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

#include "ajax.h"

#ifdef HAVE_MYSQL
#ifdef WIN32
#include <windows.h>
#endif
#include "mysql.h"
#endif

#ifdef HAVE_POSTGRESQL
#include "libpq-fe.h"
#endif




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static ajlong sqlConnectionTotalCount = 0;
static ajlong sqlConnectionFreeCount  = 0;
static ajlong sqlStatementTotalCount  = 0;
static ajlong sqlStatementFreeCount   = 0;


static AjBool arrVoidResize(AjPVoid *thys, ajuint size);




/* @filesection ajsql *********************************************************
**
** @nam1rule aj Function belongs to the AJAX library
**
******************************************************************************/




/* @func ajSqlInit ************************************************************
**
** Initialises implementation-specific SQL client libraries.
**
** @return [AjBool] ajTrue if the initialisation was successful.
** @@
******************************************************************************/

AjBool ajSqlInit(void)
{
#ifdef HAVE_MYSQL
    
    int argc = 0;
    
    char **argv   = NULL;
    char **groups = NULL;
    
    if(mysql_library_init(argc, argv, groups))
    {
        ajDebug("ajSqlInit MySQL initialisation failed.\n");
	
	return ajFalse;
    }
    else
        ajDebug("ajSqlInit MySQL client library %s\n",
                mysql_get_client_info());
#else
    ajDebug("ajSqlInit EMBOSS AJAX library built without "
            "MySQL client support.\n");
    
#endif
    
#ifdef HAVE_POSTGRESQL
    /* PostgreSQL does not require library initialisation. */
#else
    ajDebug("ajSqlInit EMBOSS AJAX library built without "
            "PostgreSQL client support.\n");
#endif
    
    return ajTrue;
}




/* @func ajSqlExit ************************************************************
**
** Finalises implementation-specific SQL client libraries.
**
** @return [void]
** @@
******************************************************************************/

void ajSqlExit(void)
{
#ifdef HAVE_MYSQL
    mysql_library_end();
#endif
    
#ifdef HAVE_POSTGRESQL
    /* PostgreSQL does not require library finalisation. */
#endif
    
    ajDebug("SQL Connection usage: %Ld opened, %Ld closed, %Ld in use\n",
            sqlConnectionTotalCount, sqlConnectionFreeCount,
            sqlConnectionTotalCount - sqlConnectionFreeCount);
    
    ajDebug("SQL Statement usage: %Ld opened, %Ld closed, %Ld in use\n",
            sqlStatementTotalCount, sqlStatementFreeCount,
            sqlStatementTotalCount - sqlStatementFreeCount);
    
    return;
}




/* @datasection [AjPSqlConnection] SQL Connection *****************************
**
** Functions for manipulating AJAX SQL Connections.
**
** @nam2rule SqlConnection Functions for manipulating AJAX SQL Connections.
**
******************************************************************************/




/* @section constructors ******************************************************
**
** Functions for constructing AJAX SQL Connection objects.
**
** @fdata [AjPSqlConnection]
**
** @nam3rule New Construct a new AJAX SQL Connection
** @nam3rule NewRef Constructor by incrementing the reference counter
**
** @valrule * [AjPSqlConnection] AJAX SQL Connection
**
** @fcategory new
******************************************************************************/




/* @func ajSqlConnectionNew ***************************************************
**
** Default AJAX SQL Connection constructor, which also allocates a client
** library-specific connection object.
** A connection to an SQL server is established.
**
** @param [r] client [const AjEnum] SQL client enumerator
** @param [r] user [const AjPStr] SQL account user name
** @param [r] password [const AjPStr] SQL account password
** @param [r] host [const AjPStr] SQL server hostname or IP address
** @param [r] port [const AjPStr] SQL server port number
** @param [r] socket [const AjPStr] SQL server UNIX socket file
**                   MySQL: Absolute path to the socket file.
**                   PostgreSQL: Absolute path to the socket directory only.
**                     Socket file names are then generated from this directory
**                     information and the port number above.
**                     See "%s/.s.PGSQL.%d" in macro UNIXSOCK_PATH in source
**                     file pgsql/src/include/libpq/pqcomm.h
** @param [r] database [const AjPStr] SQL database name
**
** @return [AjPSqlConnection] AJAX SQL Connection or NULL
** @@
******************************************************************************/

AjPSqlConnection ajSqlConnectionNew(const AjEnum client,
                                    const AjPStr user,
                                    const AjPStr password,
                                    const AjPStr host,
                                    const AjPStr port,
                                    const AjPStr socket,
                                    const AjPStr database)
{
    
#ifdef HAVE_MYSQL
    MYSQL *Pmysql = NULL;
    
    unsigned long flag = 0;
    ajint number = 0;
#endif
    
#ifdef HAVE_POSTGRESQL
    PGconn *Ppgconn = NULL;
    
    AjPStr conninfo = NULL;
    AjPStr tmp = NULL;
#endif
    
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    
    AjPSqlConnection sqlc = NULL;
    
#endif
    
    /*
    ** TODO: Data checking?
    ** The host and socket parameters are mutually exclusive.
    ** The host parameter must be NULL or localhost to connect to the local
    ** machine. The connection could be TCP or via a socket if specified.
    ** If the driver is ajESqlClientPostgreSQL then a port number is mandatory
    ** as it is part of the socket filename ("%s/.s.PGSQL.%d").
    */
    
    if(client == ajESqlClientMySQL)
    {

#ifndef HAVE_MYSQL
#ifndef HAVE_POSTGRESQL
        (void) user;
        (void) password;
        (void) host;
        (void) port;
        (void) socket;
        (void) database;
#endif
#endif

#ifdef HAVE_MYSQL
	
	Pmysql = mysql_init((MYSQL *) NULL);
	
        if(Pmysql == NULL)
        {
            ajDebug("ajSqlConnectionNew MySQL connection object "
                    "initialisation via mysql_init failed.\n");
	    
            return NULL;
        }
	
	/*
	** Read options from the [client] and [EMBOSS] groups of the
	** default my.cnf options file.
	*/
	
	mysql_options(Pmysql, MYSQL_READ_DEFAULT_GROUP, "EMBOSS");
	
        ajStrToInt(port, &number);
	
        if(mysql_real_connect(Pmysql,
                              ajStrGetPtr(host),
                              ajStrGetPtr(user),
                              ajStrGetPtr(password),
                              ajStrGetPtr(database),
                              (unsigned int) number,
                              ajStrGetPtr(socket),
                              flag)
	    )
        {
	    /* The connection was successful. */
	    
	    AJNEW0(sqlc);
	    
	    sqlc->Pconnection = (void *) Pmysql;
	    
	    sqlc->Client = client;
	    
	    sqlc->Use = 1;
	    
            ajDebug("ajSqlConnectionNew established a MySQL connection "
                    "to server '%S' on port '%S' (%d) as user '%S' "
                    "for database '%S'.\n",
		    host, port, number, user, database);
	    
            sqlConnectionTotalCount++;
	    
            return sqlc;
        }
        else
        {
            /* The connection was not successful. */
	    
	    ajDebug("ajSqlConnectionNew could not establish a "
		    "MySQL connection to server '%S' on port '%S' (%d) "
                    "as user '%S' for database '%S'.\n",
		    host, port, number, user, database);
	    
            ajDebug("ajSqlConnectionNew MySQL error: %s\n",
		    mysql_error(Pmysql));
	    
	    mysql_close(Pmysql);
	    
            return NULL;
        }
	
#else
        ajDebug("ajSqlConnectionNew EMBOSS AJAX library built without "
                "MySQL client support.\n");
	
        return NULL;
#endif
	
    }
    
    if(client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
        conninfo = ajStrNew();
	
        tmp = ajStrNew();
	
        /* PostgreSQL needs escaping of ' and \ to \' and \\. */
	
        if(ajStrGetLen(user))
        {
            ajStrAssignS(&tmp, user);
            ajStrExchangeCC(&tmp, "'", "\'");
            ajStrExchangeCC(&tmp, "\\", "\\\\");
	    
	    ajFmtPrintAppS(&conninfo, "user = '%S' ", tmp);
        }
	
        if(ajStrGetLen(password))
        {
            ajStrAssignS(&tmp, password);
            ajStrExchangeCC(&tmp, "'", "\'");
            ajStrExchangeCC(&tmp, "\\", "\\\\");
	    
	    ajFmtPrintAppS(&conninfo, "password = '%S' ", tmp);
        }
	
        if(ajStrGetLen(host))
        {
            ajStrAssignS(&tmp, host);
            ajStrExchangeCC(&tmp, "'", "\'");
            ajStrExchangeCC(&tmp, "\\", "\\\\");
	    
	    ajFmtPrintAppS(&conninfo, "host = '%S' ", tmp);
        }
	
        if(ajStrGetLen(socket))
        {
            ajStrAssignS(&tmp, socket);
            ajStrExchangeCC(&tmp, "'", "\'");
            ajStrExchangeCC(&tmp, "\\", "\\\\");
	    
	    ajFmtPrintAppS(&conninfo, "host = '%S' ", tmp);
        }
	
        if(ajStrGetLen(port))
        {
            ajStrAssignS(&tmp, port);
            ajStrExchangeCC(&tmp, "'", "\'");
            ajStrExchangeCC(&tmp, "\\", "\\\\");
	    
	    ajFmtPrintAppS(&conninfo, "port = '%S' ", tmp);
        }
	
        if(ajStrGetLen(database))
        {
            ajStrAssignS(&tmp, database);
            ajStrExchangeCC(&tmp, "'", "\'");
            ajStrExchangeCC(&tmp, "\\", "\\\\");
	    
	    ajFmtPrintAppS(&conninfo, "dbname = '%S' ", tmp);
        }
	
        /* hostaddr:        Numeric IP address of host to connect to. */
        /* connect_timeout: Maximum wait for connection, in seconds. */
        /* options:         Command line options to be sent to the server. */
        /* sslmode:         disable, allow, prefer, require */
        /* requiressl:      Deprecated use sslmode option instead. */
        /* krbsrvname:      Kerberos5 service name. */
        /* service:         Service name to use for additional parameters. */
	
        Ppgconn = PQconnectdb(ajStrGetPtr(conninfo));
	
        ajStrDel(&conninfo);
	ajStrDel(&tmp);
	
        if(Ppgconn == NULL)
        {
            ajDebug("ajSqlConnectionNew PostgreSQL connection object "
                    "initialisation via PQconnectdb failed.\n");
	    
            return NULL;
        }
	
        if(PQstatus(Ppgconn) == CONNECTION_OK)
        {
	    /* CONNECTION_OK */
	    
	    AJNEW0(sqlc);
	    
	    sqlc->Pconnection = (void *) Ppgconn;
	    
	    sqlc->Client = client;
	    
	    sqlc->Use = 1;
	    
            ajDebug("ajSqlConnectionNew established a PostgreSQL connection "
                    "to server '%S' on port '%S' as user '%S' "
		    "for database '%S'\n",
		    host, port, user, database);
	    
            sqlConnectionTotalCount++;
	    
            return sqlc;
        }
        else
        {
	    /* CONNECTION_BAD */
	    
            ajDebug("ajSqlConnectionNew could not establish a "
                    "PostgreSQL connection to server '%S' on port '%S' "
                    "as user '%S' for database '%S'.\n",
		    host, port, user, database);
	    
            ajDebug("ajSqlConnectionNew PostgreSQL error: %s\n",
                    PQerrorMessage(Ppgconn));
	    
            PQfinish(Ppgconn);
	    
            return NULL;
        }
	
#else
	
        ajDebug("ajSqlConnectionNew EMBOSS AJAX library built without "
                "PostgreSQL client support.\n");
	
        return NULL;
	
#endif
	
    }
    
    ajDebug("ajSqlConnectionNew SQL client identifier %d not supported.\n",
	    client);
    
    return NULL;
}




/* @func ajSqlConnectionNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] sqlc [AjPSqlConnection] AJAX SQL Connection
**
** @return [AjPSqlConnection] AJAX SQL Connection or NULL
** @@
******************************************************************************/

AjPSqlConnection ajSqlConnectionNewRef(AjPSqlConnection sqlc)
{
    if(!sqlc)
	return NULL;
    
    sqlc->Use++;
    
    return sqlc;
}




/* @section destructors *******************************************************
**
** Functions for destruction of AJAX SQL Connection objects.
**
** @fdata [AjPSqlConnection]
**
** @nam3rule Del Destroy (free) an existing AJAX SQL Connection
**
** @argrule Del Psqlc [AjPSqlConnection *] AJAX SQL Connection address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ajSqlConnectionDel ***************************************************
**
** Default AJAX SQL Connection destructor, which disconnects from the
** SQL server first, frees the client library-specific connection object and
** finally the AJAX SQL Connection object.
**
** @param [d] Psqlc [AjPSqlConnection *] AJAX SQL Connection address
**
** @return [void]
** @@
******************************************************************************/

void ajSqlConnectionDel(AjPSqlConnection *Psqlc)
{
    AjPSqlConnection pthis = NULL;
    
    if(!Psqlc)
        return;
    
    if(!*Psqlc)
        return;

    pthis = *Psqlc;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Psqlc = NULL;
	
	return;
    }
    
    if(pthis->Client == ajESqlClientMySQL)
    {
	
#ifdef HAVE_MYSQL
	
        mysql_close((MYSQL *) pthis->Pconnection);
	
        ajDebug("ajSqlConnectionDel MySQL connection deleted.\n");
	
#else
	
        ajDebug("ajSqlConnectionDel MySQL client support not built, "
                "but object claims MySQL connection.\n");
	
#endif
	
    }
    
    if(pthis->Client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
        PQfinish((PGconn *) pthis->Pconnection);
	
        ajDebug("ajSqlConnectionDel PostgreSQL connection deleted.\n");
	
#else
	
        ajDebug("ajSqlConnectionDel PostgreSQL client support not built, "
                "but object claims PostgreSQL connection.\n");
	
#endif
	
    }
    
    AJFREE(pthis);

    *Psqlc = NULL;
    
    sqlConnectionFreeCount++;
    
    return;
}




/* @func ajSqlConnectionGetClient *********************************************
**
** Get the client element of an AJAX SQL Connection.
**
** @param [r] sqlc [const AjPSqlConnection] AJAX SQL Connection
**
** @return [AjEnum] SQL client enumerator
** @@
******************************************************************************/

AjEnum ajSqlConnectionGetClient(const AjPSqlConnection sqlc)
{
    if(!sqlc)
        return ajESqlClientNULL;
    
    return sqlc->Client;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an AJAX SQL Connection object.
**
** @fdata [AjPSqlConnection]
** @nam3rule Trace Report AJAX SQL Connection elements to debug file 
**
** @argrule Trace sqlc [const AjPSqlConnection] AJAX SQL Connection
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ajSqlConnectionTrace *************************************************
**
** Trace an AJAX SQL Connection.
**
** @param [r] sqlc [const AjPSqlConnection] AJAX SQL Connection
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlConnectionTrace(const AjPSqlConnection sqlc, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!sqlc)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SajSqlConnectionTrace %p\n"
	    "%S  Pconnection %p\n"
	    "%S  Client %d\n"
	    "%S  Use %u\n",
	    indent, sqlc,
	    indent, sqlc->Pconnection,
	    indent, sqlc->Client,
	    indent, sqlc->Use);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ajSqlConnectionEscapeCS **********************************************
**
** Escape an AJAX String based on an AJAX SQL Connection.
** The caller is responsible for deleting the C-type char string at the
** returned address. 
**
** @param [r] sqlc [const AjPSqlConnection] AJAX SQL Connection
** @param [w] Ptxt [char **] Address of the (new) SQL-escaped C-type string
** @param [r] str [const AjPStr] AJAX String to be escaped
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlConnectionEscapeCS(const AjPSqlConnection sqlc,
                               char **Ptxt,
                               const AjPStr str)
{
    
#ifdef HAVE_POSTGRESQL
    
    int error = 0;
    
#endif
    
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    
    ajuint length = 0;
    
#endif
    
    if(!sqlc)
	return ajFalse;
    
    if(Ptxt)
	ajCharDel(Ptxt);
    
    if(!str)
	return ajFalse;
    
    if(sqlc->Client == ajESqlClientMySQL)
    {
	
#ifdef HAVE_MYSQL
	
	/* At maximum the escaped string could be 2 * n + 1 characters long. */
	
	/*
	** FIXME: MySQL uses unsigned long. It is necessary to test, whether
	** the maximum unsigned integer value has been exceeded.
	*/
	
	length = ajStrGetLen(str);
	
	*Ptxt = ajCharNewRes(2 * length + 1);
	
	length = (ajuint)
	    mysql_real_escape_string((MYSQL *) sqlc->Pconnection,
				     *Ptxt,
				     ajStrGetPtr(str),
				     (unsigned long) length);
	
#else
	
        ajDebug("ajSqlConnectionEscapeCS MySQL client support not built, "
                "but object claims MySQL connection.\n");
	
#endif
	
    }
    
    if(sqlc->Client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
	/* At maximum the escaped string could be 2 * n + 1 characters long. */
	
	/*
	** FIXME: PostgeSQL uses size_t. It is necessary to test, whether
	** the maximum unsigned integer value has been exceeded.
	*/
	
	length = ajStrGetLen(str);
	
	*Ptxt = ajCharNewRes(2 * length + 1);
	
	length = (ajuint)
	    PQescapeStringConn((PGconn *) sqlc->Pconnection,
			       *Ptxt,
			       ajStrGetPtr(str),
			       (size_t) length,
			       &error);
	
	if(error)
	    ajDebug("ajSqlConnectionEscapeCS PostgreSQL client encountered an "
		    "error during string escaping.\n");
	
#else
	
        ajDebug("ajSqlConnectionEscapeCS PostgreSQL client support not built, "
                "but object claims PostgreSQL connection.\n");
	
#endif
	
    }
    
    return ajTrue;
}




/* @func ajSqlConnectionEscapeSS **********************************************
**
** Escape an AJAX String based on an AJAX SQL Connection.
** The caller is responsible for deleting the AJAX String at the returned
** address. 
**
** @param [r] sqlc [const AjPSqlConnection] AJAX SQL Connection
** @param [w] Pstr [AjPStr *] Address of the (new) SQL-escaped AJAX String
** @param [r] str [const AjPStr] AJAX String to be escaped
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlConnectionEscapeSS(const AjPSqlConnection sqlc,
                               AjPStr *Pstr,
                               const AjPStr str)
{
    char **Ptxt = NULL;
    
    if(!sqlc)
	return ajFalse;
    
    if(!str)
	return ajFalse;
    
    ajSqlConnectionEscapeCS(sqlc, Ptxt, str);
    
    ajStrAssignC(Pstr, *Ptxt);
    
    ajCharDel(Ptxt);
    
    return ajTrue;
}




/* @datasection [AjPSqlStatement] AJAX SQL Statement **************************
**
** Functions for manipulating AJAX SQL Statements.
**
** @nam2rule SqlStatement Functions for manipulating AJAX SQL Statements.
**
******************************************************************************/




/* @section constructors ******************************************************
**
** Functions for constructing AJAX SQL Statement objects.
**
** @fdata [AjPSqlStatement]
**
** @nam3rule New Construct a new AJAX SQL Statement
** @nam3rule NewRef Constructor by incrementing the reference counter
**
** @valrule * [AjPSqlStatement] AJAX SQL Statement
**
** @fcategory new
******************************************************************************/




/* @func ajSqlStatementNew ****************************************************
**
** Default AJAX SQL Statement constructor. Upon construction the SQL Statement
** is run against the SQL server specified in the AJAX SQL Connection.
** Eventual results of the SQL Statement are then stored inside this object.
**
** @param [r] sqlc [AjPSqlConnection] AJAX SQL Connection
** @param [r] statement [const AjPStr] SQL statement
**
** @return [AjPSqlStatement] AJAX SQL Statement or NULL
** @@
******************************************************************************/

AjPSqlStatement ajSqlStatementNew(AjPSqlConnection sqlc, const AjPStr statement)
{
    
#ifdef HAVE_MYSQL
    
    MYSQL *Pmysql        = NULL;
    MYSQL_RES *Pmysqlres = NULL;
    
#endif
    
#ifdef HAVE_POSTGRESQL
    
    PGconn *Ppgconn     = NULL;
    PGresult *Ppgresult = NULL;
    
    AjPStr affected = NULL;
    
#endif
    
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    
    AjPSqlStatement sqls = NULL;
    
#endif
    
    if(!sqlc)
        return NULL;
    
    if(!statement)
        return NULL;
    
    ajDebug("ajSqlStatementNew %S\n", statement);
    
    if(sqlc->Client == ajESqlClientMySQL)
    {
	
#ifdef HAVE_MYSQL
	
	Pmysql = (MYSQL *) sqlc->Pconnection;
	
	if(!Pmysql)
	    ajFatal("ajSqlStatementNew got AJAX SQL Connection without "
		    "MYSQL client library-specific connection object.");
	
        if(mysql_real_query(Pmysql,
			     ajStrGetPtr(statement),
                             (unsigned long) ajStrGetLen(statement))
	    )
        {
            /* An error occured. */
	    
	    ajDebug("ajSqlStatementNew MySQL error: %s\n",
		    mysql_error(Pmysql));
	    
            return NULL;
        }
        else
        {
            /*
            * The SQL statement was successful, now process any data returned.
            * The mysql_store_result function retrieves all rows from the
            * server and stores them in the client immediately. This is
            * memory intensive, but reduces the server load.
            */
	    
            Pmysqlres = mysql_store_result(Pmysql);
	    
            if(Pmysqlres)
            {
                /*
		** A result was returned for this SQL statement hence
		** create an AJAX SQL Statement object.
		*/
		
		AJNEW0(sqls);
		
		sqls->SqlConnection = ajSqlConnectionNewRef(sqlc);
		
		sqls->Presult = (void *) Pmysqlres;
		
		sqls->AffectedRows = 0;
		
		sqls->SelectedRows = (ajulong) mysql_num_rows(Pmysqlres);
		
                sqls->Columns = (ajuint) mysql_num_fields(Pmysqlres);
                
		sqls->Use = 1;
		
                ajDebug("ajSqlStatementNew MySQL selected rows %Lu "
			"columns %u\n",
                        sqls->SelectedRows,
			sqls->Columns);
		
                sqlStatementTotalCount++;
		
                return sqls;
            }
            else
            {
                /*
		** No result has been returned, so check whether the
		** SQL statement should have returned rows.
		*/
		
		if(mysql_field_count(Pmysql))
                {
                    /* The statement should have returned rows. */
		    
                    ajDebug("ajSqlStatementNew MySQL error: %s\n",
                            mysql_error(Pmysql));
		    
                    return NULL;
                }
                else
                {
                    /* The statement should not have returned rows. */
		    
		    AJNEW0(sqls);
		    
		    sqls->SqlConnection = ajSqlConnectionNewRef(sqlc);
		    
		    sqls->Presult = NULL;
		    
                    sqls->AffectedRows = (ajulong) mysql_affected_rows(Pmysql);
		    
                    sqls->SelectedRows = 0;
		    
                    sqls->Columns = 0;
		    
		    sqls->Use = 1;
		    
                    ajDebug("ajSqlStatementNew MySQL affected rows %Lu\n",
                            sqls->AffectedRows);
		    
                    sqlStatementTotalCount++;
		    
                    return sqls;
                }
            }
        }
	
#else
	
        ajDebug("ajSqlStatementNew EMBOSS AJAX library built without "
                "MySQL client library support.\n");
	
        return NULL;
	
#endif
	
    }
    
    if(sqlc->Client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
	Ppgconn = (PGconn *) sqlc->Pconnection;
	
	if(!Ppgconn)
	    ajFatal("ajSqlStatementNew got AJAX SQL Connection without "
		    "PostgreSQL client library-specific connection object.");
	
	Ppgresult = PQexec(Ppgconn, ajStrGetPtr(statement));
	
        if(Ppgresult)
        {
            switch (PQresultStatus(Ppgresult))
            {
		case PGRES_EMPTY_QUERY:
		    
		    ajDebug("ajSqlStatementNew PostgreSQL reported empty "
			    "statement string.\n");
		    
		    PQclear(Ppgresult);
		    
		    return NULL;
		    
		    break;
		    
		case PGRES_COMMAND_OK:
		    
		    /*
		    ** PGRES_COMMAND_OK for statements that return no result
		    ** rows but do affect rows.
		    */
		    
		    affected = ajStrNewC(PQcmdTuples(Ppgresult));
		    
		    AJNEW0(sqls);
		    
		    sqls->SqlConnection = ajSqlConnectionNewRef(sqlc);
		    
		    sqls->Presult = NULL;
		    
		    ajStrToLong(affected, (ajlong *) &(sqls->AffectedRows));
		    
		    sqls->SelectedRows = 0;
		    
		    sqls->Columns = 0;
		    
		    sqls->Use = 1;
		    
		    ajDebug("ajSqlStatementNew PostgreSQL affected rows %Lu\n",
			    sqls->AffectedRows);
		    
		    ajStrDel(&affected);
		    
		    sqlStatementTotalCount++;
		    
		    return sqls;
		    
		    break;
		    
		case PGRES_TUPLES_OK:
		    
		    /* PGRES_TUPLES_OK for commands that return result rows. */
		    
		    AJNEW0(sqls);
		    
		    sqls->SqlConnection = ajSqlConnectionNewRef(sqlc);
		    
		    sqls->Presult = (void *) Ppgresult;
		    
		    sqls->AffectedRows = 0;
		    
		    sqls->SelectedRows = (ajulong) PQntuples(Ppgresult);
		    
		    sqls->Columns = (ajuint) PQnfields(Ppgresult);
		    
		    sqls->Use = 1;
		    
		    ajDebug("ajSqlStatementNew PostgreSQL selected rows %Lu "
			    "columns %u\n",
			    sqls->SelectedRows,
			    sqls->Columns);
		    
		    sqlStatementTotalCount++;
		    
		    return sqls;
		    
		    break;
		    
		case PGRES_FATAL_ERROR:
		    
		    ajDebug("ajSqlStatementNew PostgreSQL fatal error: %s\n",
			    PQresultErrorMessage(Ppgresult));
		    
		    PQclear(Ppgresult);
		    
		    return NULL;
		    
		    break;
		    
		default:
		    
		    ajDebug("ajSqlStatementNew PostgreSQL returned an "
			    "unexpected status: %s\n",
			    PQresStatus(PQresultStatus(Ppgresult)));
		    
		    PQclear(Ppgresult);
		    
		    return NULL;
	    }
        }
	else
	{
	    ajDebug("ajSqlStatementNew PostgreSQL error: %s\n",
		    PQerrorMessage(Ppgconn));
	    
	    return NULL;
        }
	
#else
	
	ajDebug("ajSqlStatementNew EMBOSS AJAX library built without "
		"PostgreSQL client library support.\n");
	
	return NULL;
	
#endif
	
    }
    
    ajDebug("ajSqlStatementNew AJAX SQL Connection client identifier %d "
	    "not supported.\n", sqlc->Client);
    
    return NULL;
}




/* @func ajSqlStatementNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] sqls [AjPSqlStatement] AJAX SQL Statement
**
** @return [AjPSqlStatement] AJAX SQL Statement or NULL
** @@
******************************************************************************/

AjPSqlStatement ajSqlStatementNewRef(AjPSqlStatement sqls)
{
    if(!sqls)
	return NULL;
    
    sqls->Use++;
    
    return sqls;
}




/* @section destructors *******************************************************
**
** Functions for destruction of AJAX SQL Statement objects.
**
** @fdata [AjPSqlStatement]
**
** @nam3rule Del Destroy (free) an existing AJAX SQL Statement
**
** @argrule Del Psqls [AjPSqlStatement *] AJAX SQL Statement address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ajSqlStatementDel ****************************************************
**
** Default AJAX SQL Statement destructor, which also destroys the SQL client
** library-specific result object.
**
** @param [d] Psqls [AjPSqlStatement *] AJAX SQL Statement address
**
** @return [void]
** @@
******************************************************************************/

void ajSqlStatementDel(AjPSqlStatement *Psqls)
{
    AjPSqlStatement pthis = NULL;
    
    if(!Psqls)
        return;
    
    if(!*Psqls)
        return;

    pthis = *Psqls;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Psqls = NULL;
	
	return;
    }
    
    if(pthis->SqlConnection->Client == ajESqlClientMySQL)
    {
	
#ifdef HAVE_MYSQL
	
        mysql_free_result((MYSQL_RES *) pthis->Presult);
	
	/*
	 ajDebug("ajSqlStatementDel MySQL Statement deleted.\n");
	 */
	
#else
	
        ajDebug("ajSqlStatementDel MySQL client library support not built, "
                "but object claims MySQL connection?\n");
	
#endif
	
    }
    
    if(pthis->SqlConnection->Client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
        PQclear((PGresult *) pthis->Presult);
	
	/*
	 ajDebug("ajSqlStatementDel PostgreSQL Statement deleted.\n");
	 */
	
#else
	
        ajDebug("ajSqlStatementDel PostgreSQL client library support not "
                "built, but object claims PostgreSQL connection?\n");
	
#endif
	
    }
    
    ajSqlConnectionDel(&pthis->SqlConnection);
    
    AJFREE(pthis);

    *Psqls = NULL;
    
    sqlStatementFreeCount++;
    
    return;
}




/* @func ajSqlStatementGetAffectedRows ****************************************
**
** Get the number of rows affected by a non-SELECT SQL statement.
**
** @param [r] sqls [AjPSqlStatement] AJAX SQL Statement
**
** @return [ajulong] Number of affected rows
** @@
******************************************************************************/

ajulong ajSqlStatementGetAffectedRows(const AjPSqlStatement sqls)
{
    if(!sqls)
        return 0;
    
    return sqls->AffectedRows;
}




/* @func ajSqlStatementGetSelectedRows ****************************************
**
** Get the number of rows selected by a SELECT-like SQL statement.
**
** @param [r] sqls [const AjPSqlStatement] AJAX SQL Statement
**
** @return [ajulong] Number of selected rows
** @@
******************************************************************************/

ajulong ajSqlStatementGetSelectedRows(const AjPSqlStatement sqls)
{
    if(!sqls)
        return 0;
    
    return sqls->SelectedRows;
}




/* @func ajSqlStatementGetColumns *********************************************
**
** Get the number of columns returned by a SELECT-like SQL statement.
**
** @param [r] sqls [const AjPSqlStatement] AJAX SQL Statement
**
** @return [ajuint] Number of selected columns
** @@
******************************************************************************/

ajuint ajSqlStatementGetColumns(const AjPSqlStatement sqls)
{
    if(!sqls)
        return 0;
    
    return sqls->Columns;
}




/* @func ajSqlStatementGetIdentifier ******************************************
**
** Get the identifier of a row inserted by the last INSERT-like SQL statement.
**
** @param [r] sqls [const AjPSqlStatement] AJAX SQL Statement
**
** @return [ajuint] Identifier of last inserted row
** @@
******************************************************************************/

ajuint ajSqlStatementGetIdentifier(const AjPSqlStatement sqls)
{
    ajuint identifier = 0;
    
#ifdef HAVE_MYSQL
    
    MYSQL *Pmysql = NULL;
    
#endif
    
#ifdef HAVE_POSTGRESQL
    
    PGresult *Ppgresult = NULL;
    
#endif
    
    if(!sqls)
	return 0;
    
    if(sqls->SqlConnection->Client == ajESqlClientMySQL)
    {
	
#ifdef HAVE_MYSQL
	
	Pmysql = (MYSQL *) sqls->SqlConnection->Pconnection;
	
	identifier = (ajuint) mysql_insert_id(Pmysql);
	
#else
	
        ajDebug("ajSqlStatementDel MySQL client library support not built, "
                "but object claims MySQL connection?\n");
	
#endif
	
    }
    
    if(sqls->SqlConnection->Client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
	Ppgresult = (PGresult *) sqls->Presult;
	
	identifier = (ajuint) PQoidValue(Ppgresult);
	
#else
	
        ajDebug("ajSqlStatementDel PostgreSQL client library support not "
                "built, but object claims PostgreSQL connection?\n");
	
#endif
	
    }
    
    return identifier;
}




/* @datasection [AjISqlRow] AJAX SQL Row Iterator *****************************
**
** Functions for manipulating AJAX SQL Row Iterators.
**
** @nam2rule SqlRowIter Functions for manipulating AJAX SQL Row Iterators.
**
******************************************************************************/




/* @section constructors ******************************************************
**
** Functions for constructing AJAX SQL Row Iterator objects.
**
** @fdata [AjISqlRow]
**
** @nam3rule New Construct a new AJAX SQL Row Iterator
** @nam3rule NewRef Constructor by incrementing the reference counter
**
** @valrule * [AjISqlRow] AJAX SQL Row Iterator
**
** @fcategory new
******************************************************************************/




/* @func ajSqlRowIterNew ******************************************************
**
** Default AJAX SQL Row Iterator constructor.
**
** @param [r] sqls [AjPSqlStatement] AJAX SQL Statement
**
** @return [AjISqlRow] AJAX SQL Row Iterator or NULL if the
**                     AJAX SQL Statement did not return rows and columns
** @@
******************************************************************************/

AjISqlRow ajSqlRowIterNew(AjPSqlStatement sqls)
{
    AjISqlRow sqli = NULL;
    
    if(!sqls)
        return NULL;
    
    if(!sqls->SelectedRows)
        return NULL;
    
    if(!sqls->Columns)
        return NULL;
    
    AJNEW0(sqli);
    
    sqli->SqlStatement = ajSqlStatementNewRef(sqls);
    
    sqli->SqlRow = ajSqlRowNew(sqls->Columns);
    
    sqli->Current = 0;
    
    return sqli;
}




/* @section destructors *******************************************************
**
** Functions for destruction of AJAX SQL Row Iterator objects.
**
** @fdata [AjISqlRow]
**
** @nam3rule Del Destroy (free) an existing AJAX SQL Row Iterator
**
** @argrule Del Psqli [AjISqlRow *] AJAX SQL Row Iterator address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ajSqlRowIterDel ******************************************************
**
** Default AJAX SQL Row Iterator destructor.
**
** @param [d] Psqli [AjISqlRow *] AJAX SQL Row Iterator address
**
** @return [void]
** @@
******************************************************************************/

void ajSqlRowIterDel(AjISqlRow *Psqli)
{
    AjISqlRow pthis = NULL;
    
    if(!Psqli)
        return;
    
    if(!*Psqli)
        return;

    pthis = *Psqli;
    
    ajSqlStatementDel(&pthis->SqlStatement);
    
    ajSqlRowDel(&pthis->SqlRow);
    
    AJFREE(pthis);

    *Psqli = NULL;
    
    return;
}




/* @section tests *************************************************************
**
** @fdata [AjISqlRow]
**
** @nam4rule Done Check whether iteration has ended (no more AJAX SQL Rows).
**
** @argrule Done sqli [const AjISqlRow] AJAX SQL Row Iterator
**
** @valrule * [AjBool] Result of test
**
** @fcategory use
******************************************************************************/




/* @func ajSqlRowIterDone *****************************************************
**
** Tests whether an AJAX SQL Row Iterator has completed yet.
**
** @param [r] sqli [AjISqlRow] AJAX SQL Row Iterator
**
** @return [AjBool] ajTrue if the iterator is exhausted
** @@
******************************************************************************/

AjBool ajSqlRowIterDone(const AjISqlRow sqli)
{
    if(!sqli)
        return ajTrue;
    
    if(sqli->Current < sqli->SqlStatement->SelectedRows)
	return ajFalse;
    

    return ajTrue;
}




/* @section stepping **********************************************************
**
** @fdata [AjISqlRow]
**
** @nam4rule Get Next AJAX SQL Row.
**
** @argrule * sqli [AjISqlRow] AJAX SQL Row Iterator
**
** @valrule * [AjPSqlRow] AJAX SQL Row
**
** @fcategory modify
******************************************************************************/




/* @func ajSqlRowIterGet ******************************************************
**
** Returns the next AJAX SQL Row using the iterator, or steps off the end.
**
** @param [u] sqli [AjISqlRow] AJAX SQL Row Iterator
**
** @return [AjPSqlRow] AJAX SQL Row or NULL
** @@
******************************************************************************/

AjPSqlRow ajSqlRowIterGet(AjISqlRow sqli)
{
    
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    
    register ajuint i = 0;
    
#endif
    
#ifdef HAVE_MYSQL
    
    unsigned long *lengths;
    
    MYSQL *Pmysql = NULL;
    MYSQL_ROW mysqlrow;
    
#endif
    
    if(!sqli)
        return NULL;
    
    /*
     ajDebug("ajSqlRowIterGet current row %Lu\n", iter->Current);
     */
    
    /* Check that the Iterator is within a valid range. */
    
    if(sqli->Current >= sqli->SqlStatement->SelectedRows)
    {
	ajDebug("ajSqlRowIterGet No more AJAX SQL Rows to fetch.\n");
	
	return NULL;
    }
    
    /*
    ** If a SQl Row already exits, reset the current column value,
    ** which can be used for iterating over columns of a row, otherwise
    ** construct a new SQL Row with the correct number of columns.
    */
    
    if(sqli->SqlRow)
	sqli->SqlRow->Current = 0;
    else
	sqli->SqlRow = ajSqlRowNew(sqli->SqlStatement->Columns);
    
    if(sqli->SqlStatement->SqlConnection->Client == ajESqlClientMySQL)
    {
	
#ifdef HAVE_MYSQL
	
        mysqlrow = mysql_fetch_row((MYSQL_RES *) sqli->SqlStatement->Presult);
	
        if(mysqlrow)
        {
            lengths =
	    mysql_fetch_lengths((MYSQL_RES *) sqli->SqlStatement->Presult);
	    
            for(i = 0; i < sqli->SqlStatement->Columns; i++)
            {
                ajVoidPut(&(sqli->SqlRow->Values), i, (void *) mysqlrow[i]);
		
                ajLongPut(&(sqli->SqlRow->Lengths), i, (ajlong) lengths[i]);
            }
	    
            sqli->Current++;
	    
            return sqli->SqlRow;
        }
        else
        {
	    Pmysql = (MYSQL *) sqli->SqlStatement->SqlConnection->Pconnection;
	    
	    if(mysql_errno(Pmysql))
		ajDebug("ajSqlRowIterGet MySQL error %s", mysql_error(Pmysql));
	    else
		ajDebug("ajSqlRowIterGet No more MySQL rows to fetch?\n");
	    
            return NULL;
        }
	
#else
	
        ajDebug("ajSqlRowIterGet EMBOSS AJAX library built without "
                "MySQL client library support.\n");
	
        return NULL;
	
#endif
	
    }
    
    if(sqli->SqlStatement->SqlConnection->Client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
	for(i = 0; i < sqli->SqlStatement->Columns; i++)
	{
	    ajVoidPut(&(sqli->SqlRow->Values), i,
		      (void *) PQgetvalue((PGresult *)
					  sqli->SqlStatement->Presult,
					  (int) sqli->Current,
					  (int) i));
	    
	    ajLongPut(&(sqli->SqlRow->Lengths), i,
		      (ajlong) PQgetlength((PGresult *)
					   sqli->SqlStatement->Presult,
					   (int) sqli->Current,
					   (int) i));
	}
	
	sqli->Current++;
	
	return sqli->SqlRow;
	
#else
	
        ajDebug("ajSqlRowIterGet EMBOSS AJAX library built without "
                "PostgreSQL client library support.\n");
	
        return NULL;
	
#endif
	
    }
    
    return NULL;
}




/* @section modifiers *********************************************************
**
** @fdata [AjISqlRow]
**
** @nam4rule Rewind Rewind to start
**
** @argrule * sqli [AjISqlRow] SQL Row Iterator
**
** @valrule * [void]
**
** @fcategory modify
******************************************************************************/




/* @func ajSqlRowIterRewind ***************************************************
**
** Rewind an AJAX SQL Row Iterator to the start position.
**
** @param [u] sqli [AjISqlRow] AJAX SQL Row Iterator
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlRowIterRewind(AjISqlRow sqli)
{
    if(!sqli)
	return ajFalse;
    
    
    if(sqli->SqlStatement->SqlConnection->Client == ajESqlClientMySQL)
    {
	
#ifdef HAVE_MYSQL
	
	/* For MySQL the MySQL cursor and the current row needs restting. */
	
	sqli->Current = 0;
	
	mysql_data_seek((MYSQL_RES *) sqli->SqlStatement->Presult, 0);
	
	return ajTrue;
	
#else
	
        ajDebug("ajSqlRowIterGet EMBOSS AJAX library built without "
                "MySQL client library support.\n");
	
        return ajFalse;
	
#endif
	
    }
    
    if(sqli->SqlStatement->SqlConnection->Client == ajESqlClientPostgreSQL)
    {
	
#ifdef HAVE_POSTGRESQL
	
	/* For PostgreSQL just the current row needs resetting. */
	
	sqli->Current = 0;
	
	return ajTrue;
	
#else
	
        ajDebug("ajSqlRowIterGet EMBOSS AJAX library built without "
                "PostgreSQL client library support.\n");
	
        return ajFalse;
	
#endif
	
    }
    
    return ajFalse;
}




/* @datasection [AjPSqlRow] AJAX SQL Row **************************************
**
** Functions for manipulating AJAX SQL Rows.
**
** @nam2rule SqlRow Functions for manipulating AJAX SQL Rows.
**
******************************************************************************/




/* @section constructors ******************************************************
**
** Functions for constructing AJAX SQL Row objects.
**
** @fdata [AjPSqlRow]
**
** @nam3rule New Construct a new AJAX SQL Row
** @nam3rule NewRef Constructor by incrementing the reference counter
**
** @valrule * [AjPSqlRow] AJAX SQL Row
**
** @fcategory new
******************************************************************************/




/* @func ajSqlRowNew **********************************************************
**
** Default AJAX SQL Row constructor.
**
** @param [u] columns [ajuint] Number of columns per AJAX SQL Row
**
** @return [AjPSqlRow] AJAX SQL Row
** @@
******************************************************************************/

AjPSqlRow ajSqlRowNew(ajuint columns)
{
    AjPSqlRow sqlr = NULL;
    
    AJNEW0(sqlr);
    
    sqlr->Values = ajVoidNewL(columns);
    
    sqlr->Lengths = ajLongNewL(columns);
    
    sqlr->Columns = columns;
    
    sqlr->Current = 0;
    
    return sqlr;
}




/* @section destructors *******************************************************
**
** Functions for destruction of AJAX SQL Row objects.
**
** @fdata [AjPSqlRow]
**
** @nam3rule Del Destroy (free) an existing AJAX SQL Row
**
** @argrule Del Psqlr [AjPSqlRow *] AJAX SQL Row address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ajSqlRowDel **********************************************************
**
** Default AJAX SQL Row destructor.
**
** @param [d] Psqlr [AjPSqlRow *] AJAX SQL Row address
**
** @return [void]
** @@
******************************************************************************/

void ajSqlRowDel(AjPSqlRow *Psqlr)
{
    AjPSqlRow pthis = NULL;
    
    if(!Psqlr)
        return;
    
    if(!*Psqlr)
        return;

    pthis = *Psqlr;
    
    ajVoidDel(&pthis->Values);
    
    ajLongDel(&pthis->Lengths);
    
    AJFREE(pthis);

    *Psqlr = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an AJAX SQL Row object.
**
** @fdata [AjPSqlRow]
** @fnote None
**
** @nam3rule Get Return SQL Row attribute(s)
** @nam4rule GetValues Return the AJAX Void Pointer Array of column values
** @nam4rule GetLength Return the AJAX Long Array of column value lengths
** @nam4rule GetColumns Return the number of columns
** @nam4rule GetCurrent Return the number of the current column in iteration
**
** @argrule * sqlr [const AjPSqlRow] SQL Row
**
** @valrule Columns [ajuint] Number of columns
**
** @fcategory use
******************************************************************************/




/* @func ajSqlRowGetValues ****************************************************
**
** Get the AJAX Void Pointer Array of column values in an AJAX SQL Row.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
**
** @return [AjPVoid] AJAX Void Pointer Array
** @@
******************************************************************************/

AjPVoid ajSqlRowGetValues(const AjPSqlRow sqlr)
{
    if(!sqlr)
        return NULL;
    
    return sqlr->Values;
}




/* @func ajSqlRowGetLengths ***************************************************
**
** Get the AJAX Long Integer Array of column value lengths in an AJAX SQL Row.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
**
** @return [AjPLong] AJAX Long Integer Array
** @@
******************************************************************************/

AjPLong ajSqlRowGetLengths(const AjPSqlRow sqlr)
{
    if(!sqlr)
        return NULL;
    
    return sqlr->Lengths;
}




/* @func ajSqlRowGetColumns ***************************************************
**
** Get the number of columns in an AJAX SQL Row.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
**
** @return [ajuint] Number of columns in an AJAX SQL Row
** @@
******************************************************************************/

ajuint ajSqlRowGetColumns(const AjPSqlRow sqlr)
{
    if(!sqlr)
        return 0;
    
    return sqlr->Columns;
}




/* @func ajSqlRowGetCurrent ***************************************************
**
** Get the number of the current column in column iterations of an
** AJAX SQL Row.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
**
** @return [ajuint] Number of current column of an AJAX SQL Row
** @@
******************************************************************************/

ajuint ajSqlRowGetCurrent(const AjPSqlRow sqlr)
{
    if(!sqlr)
        return 0;
    
    return sqlr->Columns;
}




/* @section column retrieval **************************************************
**
** @fdata [AjPSqlRow]
**
** @nam3rule Column Select the next column of an AJAX SQL Row
** @nam4rule ColumnGetValue Get the value of the next column
**
** @argrule * sqlr [const AjPSqlRow] AJAX SQL Row
** @argrule * Pvalue [void **] Value address
** @argrule * Plength [ajulong*] Value length address
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ajSqlColumnGetValue **************************************************
**
** Get the value of the next column of an AJAX SQL Row.
** Calling this function also advances the column iterator.
**
** @param [u] sqlr [AjPSqlRow] AJAX SQL Row
** @param [u] Pvalue [void**] Value address
** @param [u] Plength [ajulong*] Value length address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnGetValue(AjPSqlRow sqlr, void **Pvalue, ajulong *Plength)
{
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(!Plength)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    *Pvalue = ajVoidGet(sqlr->Values, sqlr->Current);
    
    *Plength = ajLongGet(sqlr->Lengths, sqlr->Current);
    
    sqlr->Current++;
    
    return ajTrue;
}




/* @section column to datatype conversion functions ***************************
**
** Functions for converting SQL Column values to other datatypes.
**
** @fdata [AjPSqlRow]
** @fnote None
**
** @nam3rule Column Convert the next column of an AJAX SQL Row to a
**                  differnt datatype
** @nam4rule ColumnToStr Convert to AJAX String
** @nam4rule ColumnToInt Convert to AJAX Signed Integer
** @nam4rule ColumnToUint Convert ot AJAX Unsigned Integer
** @nam4rule ColumnToLong Convert to AJAX Signed Long Integer
** @nam4rule ColumnToUlong Convert to AJAX Unsigned Long Integer
** @nam4rule ColumnToFloat Convert to C-type float
** @nam4rule ColumnToDouble Convert to C-type double
** @nam4rule ColumnToBool Convert to AJAX Boolean
** @nam4rule ColumnToTime Conver to AJAX Time
**
** @argrule * sqlr [const AjPSqlRow] SQL Row
** @argrule * column [ajuint] Column number
** @argrule ColumnToStr [AjPStr*] AJAX String address
** @argrule ColumnToInt [ajint*] AJAX Signed Integer address
** @argrule ColumnToUint [ajuint*] AJAX Unsigned Integer address
** @argrule ColumnToLong [ajlong*] AJAX Signed Long Integer address
** @argrule ColumnToUlong [ajulong*] AJAX Unsigned Long Integer address
** @argrule ColumnToFloat [float*] C-type float address
** @argrule ColumnToDouble [double*] C-type double address
** @argrule ColumnToBool [AjPBool*] AJAX Bool address
** @argrule ColumnToTime [AjPTime*] AJAX Time address
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ajSqlColumnToStr *****************************************************
**
** Converts the value of the next column of an AJAX SQL Row into an
** AJAX String value.
**
** @param [u] sqlr [AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [AjPStr *] AJAX String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToStr(AjPSqlRow sqlr, AjPStr *Pvalue)
{
    void *value = NULL;
    
    ajulong length = 0;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    ajStrAssignClear(Pvalue);
    
    if(ajSqlColumnGetValue(sqlr, &value, &length))
    {
	if(value == NULL)
	    return ajFalse;
	
	if(length > UINT_MAX)
	    return ajFalse;
	
	ajStrAssignLenC(Pvalue, (char *) value, (ajuint) length);
	
	return ajTrue;
    }
    
    return ajFalse;
}




/* @func ajSqlColumnToInt *****************************************************
**
** Converts the value in the next column of an AJAX SQL Row into an
** AJAX Integer value.
**
** @param [u] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [ajint *] AJAX Integer address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToInt(AjPSqlRow sqlr, ajint *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnToStr(sqlr, &str);
    
    if(bool)
	ajStrToInt(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnToUint ****************************************************
**
** Converts the value in the next column of an AJAX SQL Row into an
** AJAX Unsigned Integer value.
**
** @param [u] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [ajuint *] AJAX Unsigned Integer address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToUint(AjPSqlRow sqlr, ajuint *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnToStr(sqlr, &str);
    
    if(bool)
	ajStrToUint(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnToLong ****************************************************
**
** Converts the value in the next column of an AJAX SQL Row into an
** AJAX Long Integer value.
**
** @param [u] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [ajlong *] AJAX Long Integer address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToLong(AjPSqlRow sqlr, ajlong *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnToStr(sqlr, &str);
    
    if(bool)
	ajStrToLong(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/*
** TODO: ajSqlColumnToUlong is missing but there is also no ajStrToUlong
** function in the AJAX library.
*/




/* @func ajSqlColumnToFloat ***************************************************
**
** Converts the value in the next column of an AJAX SQL Row into a
** C-type float value.
**
** @param [u] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [float *] C-type float address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToFloat(AjPSqlRow sqlr, float *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnToStr(sqlr, &str);
    
    if(bool)
	ajStrToFloat(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnToDouble **************************************************
**
** Converts the value in the next column of an AJAX SQL Row into a
** C-type double value.
**
** @param [u] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [double *] C-type double address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToDouble(AjPSqlRow sqlr, double *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnToStr(sqlr, &str);
    
    if(bool)
	ajStrToDouble(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnToBool ****************************************************
**
** Converts the value in the next column of an AJAX SQL Row into an
** AJAX Boolean value.
** This function uses ajStrToBool to convert the AJAX String representing the
** column value into an AJAX boolean value. The function converts 'yes' and
** 'true', as well as 'no' and 'false' into its corresponding AJAX Bool values.
** It also assumes any numeric value as true and 0 as false.
**
** @param [u] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [AjBool *] AJAX Boolean address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToBool(AjPSqlRow sqlr, AjBool *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnToStr(sqlr, &str);
    
    if(bool)
	ajStrToBool(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnToTime ****************************************************
**
** Converts the value in the next column of an AJAX SQL Row into an
** AJAX Time value.
**
** @param [u] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [w] Pvalue [AjPTime *] AJAX Time address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnToTime(AjPSqlRow sqlr, AjPTime *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(!*Pvalue)
	return ajFalse;
    
    if(sqlr->Current >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnToStr(sqlr, &str);
    
    if(bool)
	ajTimeSetS(*Pvalue, str);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnRewind ****************************************************
**
** Rewind an AJAX SQL Column Iterator to the start position.
**
** @param [u] sqlr [AjPSqlRow] AJAX SQL Row
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnRewind(AjPSqlRow sqlr)
{
    if(!sqlr)
        return ajFalse;
    
    sqlr->Current = 0;
    
    return ajTrue;
}




/* @section column retrieval **************************************************
**
** @fdata [AjPSqlRow]
**
** @nam3rule ColumnNumber Select a particular column of an AJAX SQL Row.
** @nam4rule ColumnNumberGetValue Get the value of a particular column
**
** @argrule * sqlr [const AjPSqlRow] AJAX SQL Row
** @argrule * column [ajuint] Column number
**
** @argrule ColumnNumberGetValue Pvalue [void**] Value address
** @argrule ColumnNumberGetValue Plength [ajulong*] Value length address
**
** @fcategory use
******************************************************************************/




/* @func ajSqlColumnNumberGetValue ********************************************
**
** Get the value of a particular column of an AJAX SQL Row.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [u] Pvalue [void **] Value address
** @param [u] Plength [ajulong*] Value length address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberGetValue(const AjPSqlRow sqlr,
                                 ajuint column,
                                 void **Pvalue,
                                 ajulong *Plength)
{
    if(!sqlr)
        return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(!Plength)
	return ajFalse;
    
    *Pvalue = ajVoidGet(sqlr->Values, column);
    
    *Plength = ajLongGet(sqlr->Lengths, column);
    
    return ajTrue;
}




/* @section column to datatype conversion functions ***************************
**
** Functions for converting SQL Column values to other datatypes.
**
** @fdata [AjPSqlRow]
** @fnote None
**
** @nam3rule ColumnNumber Convert a particular column to a differnt datatype
** @nam4rule ColumnNumberToStr Convert to AJAX String
** @nam4rule ColumnNumberToInt Convert to AJAX Signed Integer
** @nam4rule ColumnNumberToUint Convert ot AJAX Unsigned Integer
** @nam4rule ColumnNumberToLong Convert to AJAX Signed Long Integer
** @nam4rule ColumnNumberToUlong Convert to AJAX Unsigned Long Integer
** @nam4rule ColumnNumberToFloat Convert to C-type float
** @nam4rule ColumnNumberToDouble Convert to C-type double
** @nam4rule ColumnNumberToBool Convert to AJAX Boolean
** @nam4rule ColumnNumberToTime Conver to AJAX Time
**
** @argrule * sqlr [const AjPSqlRow] SQL Row
** @argrule * column [ajuint] Column number
** @argrule ColumnNumberToStr [AjPStr*] AJAX String address
** @argrule ColumnNumberToInt [ajint*] AJAX Signed Integer address
** @argrule ColumnNumberToUint [ajuint*] AJAX Unsigned Integer address
** @argrule ColumnNumberToLong [ajlong*] AJAX Signed Long Integer address
** @argrule ColumnNumberToUlong [ajulong*] AJAX Unsigned Long Integer address
** @argrule ColumnNumberToFloat [float*] C-type float address
** @argrule ColumnNumberToDouble [double*] C-type double address
** @argrule ColumnNumberToBool [AjPBool*] AJAX Bool address
** @argrule ColumnNumberToTime [AjPTime*] AJAX Time address
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ajSqlColumnNumberToStr ***********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into an
** AJAX String value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [w] Pvalue [AjPStr *] AJAX String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberToStr(const AjPSqlRow sqlr, ajuint column,
                              AjPStr *Pvalue)
{
    void *value = NULL;
    
    ajulong length = 0;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    ajStrAssignClear(Pvalue);
    
    if(ajSqlColumnNumberGetValue(sqlr, column, &value, &length))
    {
	
	if(value == NULL)
	    return ajFalse;
	
	if(length > UINT_MAX)
	    return ajFalse;
	
	ajStrAssignLenC(Pvalue, (char *) value, (ajuint) length);
	
	return ajTrue;
    }
    
    return ajFalse;
}




/* @func ajSqlColumnNumberToInt ***********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into an
** AJAX Integer value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [w] Pvalue [ajint *] AJAX Integer address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberToInt(const AjPSqlRow sqlr, ajuint column,
                              ajint *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnNumberToStr(sqlr, column, &str);
    
    if(bool)
	ajStrToInt(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnNumberToUint **********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into an
** AJAX Unsigned Integer value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [const ajuint] Column number
** @param [w] Pvalue [ajuint *] AJAX Unsigned Integer address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberToUint(const AjPSqlRow sqlr, ajuint column,
                               ajuint *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnNumberToStr(sqlr, column, &str);
    
    if(bool)
	ajStrToUint(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnNumberToLong **********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into an
** AJAX Long Integer value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [w] Pvalue [ajlong *] AJAX Long Integer address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberToLong(const AjPSqlRow sqlr, ajuint column,
                               ajlong *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnNumberToStr(sqlr, column, &str);
    
    if(bool)
	ajStrToLong(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/*
** TODO: ajSqlColumnNumberToUlong is missing but there is also no ajStrToUlong
** function in the AJAX library.
*/




/* @func ajSqlColumnNumberToFloat *********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into a
** C-type float value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [w] Pvalue [float *] C-type float address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberToFloat(const AjPSqlRow sqlr, ajuint column,
                                float *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnNumberToStr(sqlr, column, &str);
    
    if(bool)
	ajStrToFloat(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnNumberToDouble ********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into a
** C-type double value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [w] Pvalue [double *] C-type double address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberToDouble(const AjPSqlRow sqlr, ajuint column,
                                 double *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnNumberToStr(sqlr, column, &str);
    
    if(bool)
	ajStrToDouble(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnNumberToBool **********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into an
** AJAX Boolean value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [w] Pvalue [AjBool *] AJAX Boolean address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

/*
** FIXME: This function uses ajStrToBool to set the value. It is necessary
** to test what the AJAX function expects for the string representation. If
** it expects yes and no or y and n, then it should not be used for Ensembl
** library functions, as the database stores Boolean values as TINYINT values.
*/

AjBool ajSqlColumnNumberToBool(const AjPSqlRow sqlr, ajuint column,
                               AjBool *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnNumberToStr(sqlr, column, &str);
    
    if(bool)
	ajStrToBool(str, Pvalue);
    
    ajStrDel(&str);
    
    return bool;
}




/* @func ajSqlColumnNumberToTime **********************************************
**
** Converts the value in a particular column of an AJAX SQL Row into an
** AJAX Time value.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
** @param [w] Pvalue [AjPTime *] AJAX Time address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ajSqlColumnNumberToTime(const AjPSqlRow sqlr, ajuint column,
                               AjPTime *Pvalue)
{
    AjBool bool = ajFalse;
    
    AjPStr str = NULL;
    
    if(!sqlr)
        return ajFalse;
    
    if(!Pvalue)
	return ajFalse;
    
    if(!*Pvalue)
	return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    str = ajStrNew();
    
    bool = ajSqlColumnNumberToStr(sqlr, column, &str);
    
    if(bool)
	ajTimeSetS(*Pvalue, str);
    
    ajStrDel(&str);
    
    return bool;
}




/* @section tests *************************************************************
**
** @fdata [AjPSqlRow]
**
** @nam4rule ColumnNumberIsDefined Test if the value in a particular column of
**                                 an AJAX SQL Row is defined.
**
** @argrule ColumnNumberIsDefined sqlr [const AjPSqlRow] AJAX SQL Row
** @argrule ColumnNumberIsDefined column [ajuint] Column number
**
** @valrule ColumnNumberIsDefined [AjBool] Result of test
**
** @fcategory use
******************************************************************************/




/* @func ajSqlColumnNumberIsDefined *******************************************
**
** Test if the value in a particular column of an AJAX SQL Row is defined.
**
** @param [r] sqlr [const AjPSqlRow] AJAX SQL Row
** @param [r] column [ajuint] Column number
**
** @return [AjBool] ajTrue if the value is defined,
**                  ajFalse if the value is undefined (\N)
** @@
******************************************************************************/

AjBool ajSqlColumnNumberIsDefined(const AjPSqlRow sqlr, ajuint column)
{
    if(!sqlr)
        return ajFalse;
    
    if(column >= sqlr->Columns)
        return ajFalse;
    
    if(ajVoidGet(sqlr->Values, column) == NULL)
	return ajFalse;
    
    else
	return ajTrue;
}




/*
** FIXME: Move the AJAX Void Pointer Array into ajarr.c.
**
** FIXME: Should we rename this to AjPArrayVoid? All the other arrays should be
** renamed as well? AjPLong should be an alias for ajlong *, while
** AjPArrayLong would be the AJAX Array holding AJAX Long Integer values?? 
*/

#define RESERVED_SIZE 32

/* @func ajVoidNew ************************************************************
**
** Default constructor for empty AJAX Void Pointer Arrays.
**
** @return [AjPVoid] Pointer to an empty Pointer Array structure
** @category new [AjPVoid] Default constructor
** @@
******************************************************************************/

AjPVoid ajVoidNew(void)
{
    AjPVoid thys = NULL;
    
    AJNEW0(thys);
    
    thys->Ptr = AJALLOC0(RESERVED_SIZE * sizeof(void *));
    thys->Len = 0;
    thys->Res = RESERVED_SIZE;
    
    /*
    ** FIXME: This works only in ajarr.c
    arrTotal++;
    arrAlloc += RESERVED_SIZE * sizeof(void *);
    */
    
    return thys;
}




/* @func ajVoidNewL ***********************************************************
**
** Constructor given an initial reserved size.
**
** @param [r] size [ajuint] Reserved size
** @return [AjPVoid] Pointer to an empty Pointer Array struct
**                   of specified size
** @category new [AjPVoid] Constructor with reserved size
** @@
******************************************************************************/

AjPVoid ajVoidNewL(ajuint size)
{
    AjPVoid thys = NULL;
    
    size = ajRound(size, RESERVED_SIZE);
    
    AJNEW0(thys);
    
    thys->Ptr = AJALLOC0(size * sizeof(void *));
    thys->Len = 0;
    thys->Res = size;
    
    /*
    ** FIXME: This works only in ajarr.c
    arrTotal++;
    arrAlloc += size * sizeof(void *);
    */
    
    return thys;
}




/* @func ajVoidDel *********************************************************
**
** Default destructor for AJAX Pointer arrays.
**
** If the given array is a NULL pointer, simply returns.
**
** @param  [d] thys [AjPVoid*] Pointer to the Pointer Array to be deleted.
**         The pointer is always deleted.
** @return [void]
** @category delete [AjPVoid] Default destructor
** @@
******************************************************************************/

void ajVoidDel(AjPVoid *thys)
{
    if(!thys || !*thys)
	return;
    
    /*ajDebug("ajVoidDel Len %u Res %u\n",
    (*thys)->Len, (*thys)->Res);*/
    
    AJFREE((*thys)->Ptr);
    AJFREE(*thys);
    
    *thys = NULL;
    
    /*
    ** FIXME: This works only in ajarr.c
    arrFreeCount++;
    */
    
    return;
}




/* @func ajVoidGet ************************************************************
**
** Retrieve an element from an AJAX Pointer Array.
**
** If the given array is a NULL pointer, simply returns.
**
** @param  [r] thys [const AjPVoid] Pointer to the Pointer array
** @param  [r] elem [ajuint] array element
**
** @return [void *] contents of array element
** @category cast [AjPVoid] Retrieve an address from an array
** @@
******************************************************************************/

void *ajVoidGet(const AjPVoid thys, ajuint elem)
{
    if(!thys || elem >= thys->Len)
	ajErr("Attempt to access bad Pointer array index %d\n", elem);
    
    return thys->Ptr[elem];
}




/* @funcstatic arrVoidResize **************************************************
**
** Resize a void pointer array.
**
** If the given array is a NULL pointer an error is generated.
** Negative indices generate an error.
**
** @param  [w] thys [AjPVoid*] Pointer to the void pointer array
** @param  [r] size [ajuint] new size
**
** @return [AjBool] true if the array was extended
** @@
******************************************************************************/

static AjBool arrVoidResize(AjPVoid *thys, ajuint size)
{
    AjPVoid p = NULL;
    ajuint    s;
    ajuint    clen;
    ajuint    limit;
    
    
    if(!thys || !*thys)
	ajErr("Illegal attempt to resize void pointer array");
    
    clen = ajRound((*thys)->Len - 1, RESERVED_SIZE);
    s = ajRound(size + 1, RESERVED_SIZE);
    if(s <= clen)
	return ajFalse;
    
    /*ajDebug("ajVoidResize %d (%d) -> %d (%d)\n",
	(*thys)->Len, clen, size, s);*/
    
    p = *thys;
    
    *thys = ajVoidNewL(s);
    
    if(size < p->Len)
	limit = size + 1;
    else
	limit = p->Len;
    
    memmove((*thys)->Ptr, p->Ptr, limit * sizeof(void *));
    
    (*thys)->Len = size + 1;
    
    ajVoidDel(&p);
    
    /*
    ** FIXME: This works only in ajarr.c
    arrResize++;
    */
    
    return ajTrue;
}




/* @func ajVoidPut ************************************************************
**
** Load a void pointer array element.
**
** If the given array is a NULL pointer an error is generated.
** If the array is of insufficient size then the array is extended.
** Negative indices generate an error.
**
** @param  [w] thys [AjPVoid*] Pointer to the void pointer array
** @param  [r] elem [ajuint] array element
** @param  [r] v [void *] value to load
**
** @return [AjBool] true if the array was extended
** @category modify [AjPChar] Load a character array element
** @@
******************************************************************************/

AjBool ajVoidPut(AjPVoid *thys, ajuint elem, void *v)
{
    if(!thys || !*thys)
	ajErr("Attempt to write to illegal array value %d\n", elem);
    
    if(elem < (*thys)->Res)
    {
	if(elem >= (*thys)->Len)
	    (*thys)->Len = elem + 1;
	
	(*thys)->Ptr[elem] = v;

	return ajFalse;
    }
    
    arrVoidResize(thys, elem);
    
    (*thys)->Ptr[elem] = v;
    
    return ajTrue;
}
