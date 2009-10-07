/******************************************************************************
** @source Ensembl Database Connection functions.
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified $Date: 2009/10/07 19:21:41 $ by $Author: rice $
** @version $Revision: 1.2 $
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

#include "ensdatabaseconnection.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */




/* @filesection ensdatabaseconnection *****************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/



/* @datasection [EnsPDatabaseConnection] Database Connection ******************
**
** Functions for manipulating Ensembl Database Connection objects
**
** @cc Bio::EnsEMBL::DBSQL::DbConnection CVS Revision: 1.51
**
** @nam2rule DatabaseConnection
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Database Connection by pointer.
** It is the responsibility of the user to first destroy any previous
** Database Connection. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDatabaseConnection]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPDatabaseConnection] Ensembl Database Connection
** @argrule Ref object [EnsPDatabaseConnection] Ensembl Database Connection
**
** @valrule * [EnsPDatabaseConnection] Ensembl Database Connection
**
** @fcategory new
******************************************************************************/




/* @func ensDatabaseConnectionNew *********************************************
**
** Default Ensembl Database Connection constructor.
**
** @param [r] client [AjEnum] AJAX SQL client type
** @param [u] user [AjPStr] SQL user name
** @param [u] password [AjPStr] SQL password
** @param [u] host [AjPStr] SQL host name
** @param [u] port [AjPStr] SQL host port
** @param [u] socket [AjPStr] UNIX socket file
** @param [u] database [AjPStr] SQL database name
**
** @return [EnsPDatabaseConnection] Ensembl Database Connection or NULL
** @@
******************************************************************************/

EnsPDatabaseConnection ensDatabaseConnectionNew(AjEnum client,
                                                AjPStr user,
                                                AjPStr password,
                                                AjPStr host,
                                                AjPStr port,
                                                AjPStr socket,
                                                AjPStr database)
{
    EnsPDatabaseConnection dbc = NULL;
    
    if(!client)
	return NULL;
    
    /*
     ajDebug("ensDatabaseConnectionNew\n"
	     "  client %d\n"
	     "  user '%S'\n"
	     "  password '***'\n"
	     "  host '%S'\n"
	     "  port '%S'\n"
	     "  socket '%S'\n"
	     "  database '%S'\n",
	     client,
	     user,
	     host,
	     port,
	     socket,
	     database);
     */
    
    AJNEW0(dbc);
    
    dbc->SqlClientType = client;
    
    if(user)
	dbc->UserName = ajStrNewRef(user);
    
    if(password)
	dbc->Password = ajStrNewRef(password);
    
    if(host)
	dbc->HostName = ajStrNewRef(host);
    
    if(port)
	dbc->HostPort = ajStrNewRef(port);
    
    if(socket)
	dbc->Socket = ajStrNewRef(socket);
    
    if(database)
	dbc->DatabaseName = ajStrNewRef(database);
    
    dbc->Use = 1;
    
    return dbc;
}




/* @func ensDatabaseConnectionNewC ********************************************
**
** Construct an Ensembl Database Connection on an already existing connection.
** Optionally, a database name may be provided to connect to a different
** database on the same SQL server using the same SQL account information.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
** @param [u] database [AjPStr] SQL database name (optional)
**
** @return [EnsPDatabaseConnection] Ensembl Database Connection or NULL
** @@
******************************************************************************/

EnsPDatabaseConnection ensDatabaseConnectionNewC(
    EnsPDatabaseConnection dbc, AjPStr database)
{
    EnsPDatabaseConnection newdbc = NULL;
    
    if(!dbc)
        return NULL;
    
    /*
     ajDebug("ensDatabaseConnectionNewC\n"
	     "  dbc %p\n"
	     "  database '%S'\n",
	     dbc,
	     database);
     
     ensDatabaseConnectionDebug(dbc, 1);
     */
    
    AJNEW0(newdbc);
    
    newdbc->SqlClientType = dbc->SqlClientType;
    
    if(dbc->UserName)
	newdbc->UserName = ajStrNewRef(dbc->UserName);
    
    if(dbc->Password)
	newdbc->Password = ajStrNewRef(dbc->Password);
    
    if(dbc->HostName)
	newdbc->HostName = ajStrNewRef(dbc->HostName);
    
    if(dbc->HostPort)
	newdbc->HostPort = ajStrNewRef(dbc->HostPort);
    
    if(dbc->Socket)
	newdbc->Socket = ajStrNewRef(dbc->Socket);
    
    if(database && ajStrGetLen(database))
        newdbc->DatabaseName = ajStrNewRef(database);
    else
    {
	if(dbc->DatabaseName)
	    newdbc->DatabaseName = ajStrNewRef(dbc->DatabaseName);
    }
    
    newdbc->Use = 1;
    
    return newdbc;
}




/* @func ensAnalysisNewRef ****************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [EnsPDatabaseConnection] Ensembl Database Connection
** @@
******************************************************************************/

EnsPDatabaseConnection ensDatabaseConnectionNewRef(EnsPDatabaseConnection dbc)
{
    if(!dbc)
	return NULL;
    
    dbc->Use++;
    
    return dbc;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Database Connection.
**
** @fdata [EnsPDatabaseConnection]
** @fnote None
**
** @nam3rule Del Destroy (free) an Database Connection object
**
** @argrule * Pdbc [EnsPDatabaseConnection*] Ensembl Database Connection
**                                           object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDatabaseConnectionDel *********************************************
**
** Default Ensembl Database Connection destructor. Before freeing memory the
** connection to the SQL server is dropped.
**
** @param [d] Pdbc [EnsPDatabaseConnection*] Ensembl Database Connection
**                                           address
**
** @return [void]
** @@
******************************************************************************/

void ensDatabaseConnectionDel(EnsPDatabaseConnection* Pdbc)
{
    EnsPDatabaseConnection pthis = NULL;
    
    if(!Pdbc)
        return;
    
    if(!*Pdbc)
        return;

    pthis = *Pdbc;
    
    /*
     ajDebug("ensDatabaseConnectionDel\n"
	     "  *Pdbc %p\n",
	     *Pdbc);
     */
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pdbc = NULL;
	
	return;
    }
    
    ajSqlconnectionDel(&pthis->Sqlconnection);
    
    ajStrDel(&pthis->UserName);
    
    ajStrDel(&pthis->Password);
    
    ajStrDel(&pthis->HostName);
    
    ajStrDel(&pthis->HostPort);
    
    ajStrDel(&pthis->Socket);
    
    ajStrDel(&pthis->DatabaseName);
    
    AJFREE(pthis);

    *Pdbc = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Database Connection object.
**
** @fdata [EnsPDatabaseConnection]
** @fnote None
**
** @nam3rule Get Return Database Connection attribute(s)
** @nam4rule GetSqlconnection Return the AJAX SQL Connection
** @nam4rule GetUserName Return the user name
** @nam4rule GetPassword Return the password
** @nam4rule GetHostName Return the host name
** @nam4rule GetHostPort Return host port
** @nam4rule GetSocket Return the UNIX socket
** @nam4rule GetDatabaseName Return the database name
** @nam4rule GetSqlClientType Return the SQL client type
**
** @argrule * dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @valrule Sqlconnection [AjPSqlconnection] AJAX SQL Connection
** @valrule UserName [AjPStr] User name
** @valrule Password [AjPStr] Password
** @valrule HostName [AjPStr] Host name
** @valrule HostPort [AjPStr] Host port
** @valrule Socket [AjPStr] UNIX socket
** @valrule DatabaseName [AjPStr] Database name
** @valrule SqlClientType [AjEnum] SQL client type
**
** @fcategory use
******************************************************************************/




/* @func ensDatabaseConnectionGetSqlconnection ********************************
**
** Get the AJAX SQL Connection element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjPSqlconnection] AJAX SQL Connection
** @@
******************************************************************************/

AjPSqlconnection ensDatabaseConnectionGetSqlconnection(
    const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return NULL;
    
    return dbc->Sqlconnection;
}




/* @func ensDatabaseConnectionGetUserName *************************************
**
** Get the user name element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjPStr] User name
** @@
******************************************************************************/

AjPStr ensDatabaseConnectionGetUserName(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return NULL;
    
    return dbc->UserName;
}




/* @func ensDatabaseConnectionGetPassword *************************************
**
** Get the password element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjPStr] Password
** @@
******************************************************************************/

AjPStr ensDatabaseConnectionGetPassword(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return NULL;
    
    return dbc->Password;
}




/* @func ensDatabaseConnectionGetHostName *************************************
**
** Get the host name element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjPStr] Host name
** @@
******************************************************************************/

AjPStr ensDatabaseConnectionGetHostName(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return NULL;
    
    return dbc->HostName;
}




/* @func ensDatabaseConnectionGetHostPort *************************************
**
** Get the host port element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjPStr] Host port
** @@
******************************************************************************/

AjPStr ensDatabaseConnectionGetHostPort(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return NULL;
    
    return dbc->HostPort;
}




/* @func ensDatabaseConnectionGetSocket ***************************************
**
** Get the UNIX socket element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjPStr] UNIX socket
** @@
******************************************************************************/

AjPStr ensDatabaseConnectionGetSocket(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return NULL;
    
    return dbc->Socket;
}




/* @func ensDatabaseConnectionGetDatabaseName *********************************
**
** Get the database name element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjPStr] Database name
** @@
******************************************************************************/

AjPStr ensDatabaseConnectionGetDatabaseName(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return NULL;
    
    return dbc->DatabaseName;
}




/* @func ensDatabaseConnectionGetSqlClientType ********************************
**
** Get the SQL client type element of an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjEnum] SQL client type
** @@
******************************************************************************/

AjEnum ensDatabaseConnectionGetSqlClientType(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return ajESqlClientNULL;
    
    return dbc->SqlClientType;
}




/* @func ensDatabaseConnectionMatch *******************************************
**
** Tests for matching two Ensembl Database Connections.
**
** @param [r] dbc1 [const EnsPDatabaseConnection] Ensembl Database Connection
** @param [r] dbc2 [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjBool] ajTrue if the Ensembl Database Connections are equal
** @@
** The comparison is based on initial pointer equality and if that fails a
** direct comparison of Ensembl Database Connection elements.
******************************************************************************/

AjBool ensDatabaseConnectionMatch(const EnsPDatabaseConnection dbc1,
                                  const EnsPDatabaseConnection dbc2)
{
    if(!dbc1)
	return ajFalse;
    
    if(!dbc2)
	return ajFalse;
    
    if(dbc1 == dbc2)
	return ajTrue;
    
    /*
    ** The AJAX SQL Connection is not tested as it can be disconnected at any
    ** time and the database name is most likely to be different and
    ** therefore tested first. String matches are rather expensive...
    */
    
    if(!ajStrMatchS(dbc1->DatabaseName, dbc2->DatabaseName))
	return ajFalse;
    
    if(!ajStrMatchS(dbc1->UserName, dbc2->UserName))
	return ajFalse;
    
    if(!ajStrMatchS(dbc1->Password, dbc2->Password))
	return ajFalse;
    
    if(!ajStrMatchS(dbc1->HostName, dbc2->HostName))
	return ajFalse;
    
    if(!ajStrMatchS(dbc1->HostPort, dbc2->HostPort))
	return ajFalse;
    
    if(!ajStrMatchS(dbc1->Socket, dbc2->Socket))
	return ajFalse;
    
    if(dbc1->SqlClientType != dbc2->SqlClientType)
	return ajFalse;
    
    return ajTrue;
}




/* @func ensDatabaseConnectionConnect *****************************************
**
** Connect an Ensembl Database Connection to the specified SQL database.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseConnectionConnect(EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return ajFalse;
    
    if(dbc->Sqlconnection)
        return ajTrue;
    
    /*
     ajDebug("ensDatabaseConnectionConnect\n"
	     "  dbc %p\n",
	     dbc);
     */
    
    dbc->Sqlconnection = ajSqlconnectionNewData(dbc->SqlClientType,
                                                dbc->UserName,
                                                dbc->Password,
                                                dbc->HostName,
                                                dbc->HostPort,
                                                dbc->Socket,
                                                dbc->DatabaseName);
    
    if(!dbc->Sqlconnection)
    {
        ajWarn("Could not establish an SQL connection for user '%S' "
               "to host '%S' at port '%S' for database '%S'.\n",
	       dbc->UserName,
	       dbc->HostName,
               dbc->HostPort,
	       dbc->DatabaseName);
	
        return ajFalse;
    }
    
    return ajTrue;
}




/* @func ensDatabaseConnectionDisconnect **************************************
**
** Disconnect an Ensembl Database Connection from its SQL database.
**
** @param [u] Pdbc [EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [void]
** @@
******************************************************************************/

void ensDatabaseConnectionDisconnect(EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return;
    
    /*
     ajDebug("ensDatabaseConnectionDisconnect\n"
	     "  dbc %p\n",
	     dbc);
     
     ensDatabaseConnectionDebug(dbc, 1);
     */
    
    ajSqlconnectionDel(&(dbc->Sqlconnection));
    
    return;
}




/* @func ensDatabaseConnectionIsConnected *************************************
**
** Test whether an Ensembl Database Connection has an active
** AJAX SQL Connection assigned.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjBool] ajTrue if the Ensembl Database Connection has an active
**                  AJAX SQL Connection assigned
** @@
******************************************************************************/

AjBool ensDatabaseConnectionIsConnected(const EnsPDatabaseConnection dbc)
{
    if(!dbc)
        return ajFalse;
    
    if(dbc->Sqlconnection)
        return ajTrue;
    
    return ajFalse;
}




/* @func ensDatabaseConnectionSqlstatementNew *********************************
**
** Run an SQL statement against an Ensembl Database Connection.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
** @param [r] statement [const AjPStr] SQL statement
**
** @return [AjPSqlstatement] AJAX SQL Statement
** @@
******************************************************************************/

AjPSqlstatement ensDatabaseConnectionSqlstatementNew(
    EnsPDatabaseConnection dbc,
    const AjPStr statement)
{
    if(!dbc)
        return NULL;
    
    /*
     ajDebug("ensDatabaseConnectionSqlstatementNew\n"
	     "  dbc %p\n"
	     "  statement '%S'\n",
	     dbc,
	     statement);
     
     ensDatabaseConnectionTrace(dbc, 1);
     */
    
    if(!ensDatabaseConnectionIsConnected(dbc))
        if(!ensDatabaseConnectionConnect(dbc))
            return NULL;
    
    return ajSqlstatementNewRun(dbc->Sqlconnection, statement);
}




/* @func ensDatabaseConnectionEscapeC ****************************************
**
** Escape an AJAX String based on an AJAX SQL Connection.
** The caller is responsible for deleting the char string at the returned
** address.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
** @param [wP] Ptxt [char**] Address of the (new) SQL-escaped C string
** @param [r] str [const AjPStr] AJAX String to be escaped
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseConnectionEscapeC(EnsPDatabaseConnection dbc,
                                     char **Ptxt,
                                     const AjPStr str)
{
    if(!dbc)
        return ajFalse;
    
    if(!str)
	return ajFalse;
    
    /*
     ajDebug("ensDatabaseConnectionEscapeC\n"
	     "  dbc %p\n"
	     "  Ptxt %p\n"
	     "  str '%S'\n",
	     dbc,
	     Ptxt,
	     str);
     
     ensDatabaseConnectionTrace(dbc, 1);
     */
    
    if(!ensDatabaseConnectionIsConnected(dbc))
        if(!ensDatabaseConnectionConnect(dbc))
            return ajFalse;
    
    return ajSqlconnectionEscapeC(dbc->Sqlconnection, Ptxt, str);
}




/* @func ensDatabaseConnectionEscapeS ****************************************
**
** Escape an AJAX String based on an AJAX SQL Connection.
** The caller is responsible for deleting the AJAX String at the returned
** address.
**
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
** @param [wP] Pstr [AjPStr*] Address of the (new) SQL-escaped AJAX String
** @param [r] str [const AjPStr] AJAX String to be escaped
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseConnectionEscapeS(EnsPDatabaseConnection dbc,
                                    AjPStr *Pstr,
                                    const AjPStr str)
{
    if(!dbc)
        return ajFalse;
    
    if(!str)
	return ajFalse;
    
    /*
     ajDebug("ensDatabaseConnectionEscapeS\n"
	     "  dbc %p\n"
	     "  Pstr %p\n"
	     "  str '%S'\n",
	     dbc,
	     Pstr,
	     str);
     
     ensDatabaseConnectionTrace(dbc, 1);
     */
    
    if(!ensDatabaseConnectionIsConnected(dbc))
        if(!ensDatabaseConnectionConnect(dbc))
            return ajFalse;
    
    return ajSqlconnectionEscapeS(dbc->Sqlconnection, Pstr, str);
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Database Connection object.
**
** @fdata [EnsPDatabaseConnection]
** @nam3rule Trace Report Ensembl Database Connection elements to debug file
**
** @argrule Trace dbc [const EnsPDatabaseConnection] Ensembl Database
**                                                   Connection
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensDatabaseConnectionTrace *******************************************
**
** Trace an Ensembl Database Connection.
**
** @param [r] dbc [const EnsPDatabaseConnection] Ensembl Database Connection
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseConnectionTrace(const EnsPDatabaseConnection dbc,
                                  ajuint level)
{
    AjPStr indent = NULL;
    
    if(!dbc)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensDatabaseConnectionTrace %p\n"
	    "%S  Sqlconnection %p\n"
	    "%S  SqlClientType %d\n"
	    "%S  UserName '%S'\n"
	    "%S  Password '***'\n"
	    "%S  HostName '%S'\n"
	    "%S  HostPort '%S'\n"
	    "%S  Socket '%S'\n"
	    "%S  DatabaseName '%S'\n"
	    "%S  Use %u\n",
	    indent, dbc,
	    indent, dbc->Sqlconnection,
	    indent, dbc->SqlClientType,
	    indent, dbc->UserName,
	    indent,
	    indent, dbc->HostName,
	    indent, dbc->HostPort,
	    indent, dbc->Socket,
	    indent, dbc->DatabaseName,
	    indent, dbc->Use);
    
    ajSqlconnectionTrace(dbc->Sqlconnection, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}
