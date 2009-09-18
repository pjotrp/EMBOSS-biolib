#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajsql_h
#define ajsql_h




/* @data AjPVoid **************************************************************
**
** Ajax address object.
**
** Holds a void array with additional data.
** The length is known and held internally.
**
** Saves on length calculation, and allows growth in reserved memory without
** changing the pointer in the calling routine.
**
** AjPVoid is implemented as a pointer to a C data structure.
**
** @alias AjSVoid
** @alias AjOVoid
**
** @new    ajVoidNew Default constructor
** @new    ajVoidNewL Constructor with reserved size
** @delete ajVoidDel Default destructor
** @cast   ajVoidGet Retrieve a pointer from an array
** @modify ajVoidPut Load a pointer array element
**
** @attr Res [ajuint] Reserved space in case of extension
** @attr Len [ajuint] Actual length used
** @attr Ptr [void **] Array of void pointers
** @@
******************************************************************************/

typedef struct AjSVoid
{
  ajuint Res;
  ajuint Len;
  void **Ptr;
} AjOVoid;
#define AjPVoid AjOVoid*




/* @datatype AjESqlClient *****************************************************
**
** AJAX SQL Client enumeration.
**
** @attr enum [AjESqlClient] Value
** @@
******************************************************************************/

enum AjESqlClient
{
    ajESqlClientNULL,
    ajESqlClientMySQL,
    ajESqlClientPostgreSQL
};




/* @data AjPSqlConnection *****************************************************
**
** AJAX SQL Connection.
**
** Holds the client type and a pointer to a SQL client library-specific
** connection object.
**
** @alias AjSSqlConnection
** @alias AjOSqlConnection
**
** @attr Pconnection [void *] SQL client library-specific connection object
**                            (MYSQL*) for the MySQL client library
**                            (PGconn*) for the PostgreSQL client library
** @attr Client [AjEnum] SQL client type (ajESqlMySQL, ajESqlPostgreSQL, ...)
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct AjSSqlConnection
{
    void *Pconnection;
    AjEnum Client;
    ajuint Use;
} AjOSqlConnection;

#define AjPSqlConnection AjOSqlConnection*




/* @data AjPSqlStatement ******************************************************
**
** AJAX SQL Statement.
**
** Holds pointers to an AJAX SQL Connection and to a client library-specific
** result object.
**
** @alias AjSSqlStatement
** @alias AjOSqlStatement
**
** @attr SqlConnection [AjPSqlConnection] AJAX SQL Connection.
** @attr Presult [void *] SQL client library-specific result object
**                        (MYSQL_RES*) for the MySQL client library
**                        (PGresult*) for the PostgreSQL client library
** @attr AffectedRows [ajulong] Number of rows affected by non-SELECT SQL
**                              statements
** @attr SelectedRows [ajulong] Number of rows selected by SELECT-like SQL
**                              statements
** @attr Columns [ajuint] Number of columns returned by SELECT-like statements
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct AjSSqlStatement
{
    AjPSqlConnection SqlConnection;
    void *Presult;
    ajulong AffectedRows;
    ajulong SelectedRows;
    ajuint Columns;
    ajuint Use;
} AjOSqlStatement;

#define AjPSqlStatement AjOSqlStatement*




/* @data AjPSqlRow ************************************************************
**
** AJAX SQL Result Row.
**
** Holds an AjPChar array of C-type character strings and an AjLong array of
** data lengths for each column data values in SQL client library-specific
** result objects.
**
** @alias AjSSqlRow
** @alias AjOSqlRow
**
** @attr Values [AjPVoid] AJAX Character Array of SQL column values
** @attr Lengths [AjPLong] AJAX Long Integer Array of SQL column value lengths
** @attr Columns [ajuint] Number of columns per row
** @attr Current [ajuint] Current column in column interations
** @@
******************************************************************************/

typedef struct AjSSqlRow
{
    AjPVoid Values;
    AjPLong Lengths;
    ajuint Columns;
    ajuint Current;
} AjOSqlRow;

#define AjPSqlRow AjOSqlRow*




/* @data AjISqlRow ************************************************************
**
** AJAX SQL Row Iterator.
**
** Allows iteration over AJAX SQL Rows of an AJAX SQL Statement.
**
** @alias AjSSqlRowIter
** @alias AjOSqlRowIter
** @alias AjPSqlRowIter
**
** @attr SqlStatement [AjPSqlStatement] AJAX SQL Statement
** @attr SqlRow [AjPSqlRow] AJAX SQL Result Row
** @attr Current [ajulong] Current row number
** @@
******************************************************************************/

typedef struct AjSSqlRowIter
{
    AjPSqlStatement SqlStatement;
    AjPSqlRow SqlRow;
    ajulong Current;
} AjOSqlRowIter;

#define AjPSqlRowIter AjOSqlRowIter*

#define AjISqlRow AjOSqlRowIter*




/*
** Prototype definitions
*/

AjBool ajSqlInit(void);

void ajSqlExit(void);

/* AJAX SQL Connection */

AjPSqlConnection ajSqlConnectionNew(const AjEnum client,
                                    const AjPStr user,
                                    const AjPStr password,
                                    const AjPStr host,
                                    const AjPStr port,
                                    const AjPStr socket,
                                    const AjPStr database);

AjPSqlConnection ajSqlConnectionNewRef(AjPSqlConnection sqlc);

void ajSqlConnectionDel(AjPSqlConnection* Psqlc);

AjEnum ajSqlConnectionGetClient(const AjPSqlConnection sqlc);

AjBool ajSqlConnectionTrace(const AjPSqlConnection sqlc, ajuint level);

AjBool ajSqlConnectionEscapeCS(const AjPSqlConnection sqlc,
                               char **Ptxt,
                               const AjPStr str);

AjBool ajSqlConnectionEscapeSS(const AjPSqlConnection sqlc,
                               AjPStr *Pstr,
                               const AjPStr str);

/* AJAX SQL Statement */

AjPSqlStatement ajSqlStatementNew(AjPSqlConnection sqlc,
                                  const AjPStr statement);

AjPSqlStatement ajSqlStatementNewRef(AjPSqlStatement sqls);

void ajSqlStatementDel(AjPSqlStatement* Psqls);

ajulong ajSqlStatementGetAffectedRows(const AjPSqlStatement sqls);

ajulong ajSqlStatementGetSelectedRows(const AjPSqlStatement sqls);

ajuint ajSqlStatementGetColumns(const AjPSqlStatement sqls);

ajuint ajSqlStatementGetIdentifier(const AjPSqlStatement sqls);

/* AJAX SQL Row Iterator */

AjISqlRow ajSqlRowIterNew(AjPSqlStatement sqls);

void ajSqlRowIterDel(AjISqlRow *Psqli);

AjBool ajSqlRowIterDone(const AjISqlRow sqli);

AjPSqlRow ajSqlRowIterGet(AjISqlRow sqli);

AjBool ajSqlRowIterRewind(AjISqlRow sqli);

/* AJAX SQL Row */

AjPSqlRow ajSqlRowNew(ajuint columns);

void ajSqlRowDel(AjPSqlRow *Psqlr);

AjPVoid ajSqlRowGetValues(const AjPSqlRow sqlr);

AjPLong ajSqlRowGetLengths(const AjPSqlRow sqlr);

ajuint ajSqlRowGetColumns(const AjPSqlRow sqlr);

ajuint ajSqlRowGetCurrent(const AjPSqlRow sqlr);

AjBool ajSqlColumnGetValue(AjPSqlRow sqlr, void **Pvalue, ajulong *Plength);

AjBool ajSqlColumnToStr(AjPSqlRow sqlr, AjPStr *Pvalue);

AjBool ajSqlColumnToInt(AjPSqlRow sqlr, ajint *Pvalue);

AjBool ajSqlColumnToUint(AjPSqlRow sqlr, ajuint *Pvalue);

AjBool ajSqlColumnToLong(AjPSqlRow sqlr, ajlong *Pvalue);

AjBool ajSqlColumnToFloat(AjPSqlRow sqlr, float *Pvalue);

AjBool ajSqlColumnToDouble(AjPSqlRow sqlr, double *Pvalue);

AjBool ajSqlColumnToBool(AjPSqlRow sqlr, AjBool *Pvalue);

AjBool ajSqlColumnToTime(AjPSqlRow sqlr, AjPTime *Pvalue);

AjBool ajSqlColumnRewind(AjPSqlRow sqlr);

AjBool ajSqlColumnNumberGetValue(const AjPSqlRow sqlr,
                                 ajuint column,
                                 void **Pvalue,
                                 ajulong *Plength);

AjBool ajSqlColumnNumberToStr(const AjPSqlRow sqlr, ajuint column,
                              AjPStr *Pvalue);

AjBool ajSqlColumnNumberToInt(const AjPSqlRow sqlr, ajuint column,
                              ajint *Pvalue);

AjBool ajSqlColumnNumberToUint(const AjPSqlRow sqlr, ajuint column,
                               ajuint *Pvalue);

AjBool ajSqlColumnNumberToLong(const AjPSqlRow sqlr, ajuint column,
                               ajlong *Pvalue);

AjBool ajSqlColumnNumberToFloat(const AjPSqlRow sqlr, ajuint column,
                                float *Pvalue);

AjBool ajSqlColumnNumberToDouble(const AjPSqlRow sqlr, ajuint column,
                                 double *Pvalue);

AjBool ajSqlColumnNumberToBool(const AjPSqlRow sqlr, ajuint column,
                               AjBool *Pvalue);

AjBool ajSqlColumnNumberToTime(const AjPSqlRow sqlr, ajuint column,
                               AjPTime *Pvalue);

AjBool ajSqlColumnNumberIsDefined(const AjPSqlRow sqlr, ajuint column);

/* AJAX Void Array */

AjPVoid ajVoidNew(void);

AjPVoid ajVoidNewL(ajuint size);

void ajVoidDel(AjPVoid *thys);

void *ajVoidGet(const AjPVoid thys, ajuint elem);

AjBool ajVoidPut(AjPVoid *thys, ajuint elem, void *v);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
