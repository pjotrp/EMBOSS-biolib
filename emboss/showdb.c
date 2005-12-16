/* @source showdb application
**
** Displays information on the currently available databases
**
** @author Copyright (C) Gary Williams (gwilliam@hgmp.mrc.ac.uk)
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "emboss.h"



static void showdbDBWidth (const AjPStr dbname, const AjPStr type,
			    const AjPStr methods, 
			    const AjPStr defined, const AjPStr release,
			    ajint *maxname, ajint *maxtype, ajint *maxmethod,
			    ajint *maxfield, ajint* maxdefined,
			    ajint *maxrelease);
static void showdbDBHead (AjPFile outfile, AjBool html, AjBool dotype,
			   AjBool doid, AjBool doqry, AjBool doall,
			   AjBool domethod, AjBool dofields, AjBool dodefined,
			   AjBool docomment, AjBool dorelease,
			   ajint maxname, ajint maxtype, ajint maxmethod, 
			   ajint maxfield, ajint maxdefined, ajint maxrelease);

static void   showdbDBOut(AjPFile outfile,
			   const AjPStr dbname, const AjPStr type,
			   AjBool id, AjBool qry, AjBool all,
			   const AjPStr methods, const AjPStr defined,
			   const AjPStr comment, const AjPStr release,
			   AjBool html, AjBool dotype,
			   AjBool doid, AjBool doqry, AjBool doall,
			   AjBool domethod, AjBool dofields, AjBool dodefined,
			   AjBool docomment, AjBool dorelease,
			   ajint maxname, ajint maxtype, ajint maxmethod,
			   ajint maxfield, ajint maxdefined,
			   ajint maxrelease);

static AjPStr showdbGetFields(const AjPStr dbname);
static int    showdbDBSortDefined(const void* str1, const void* str2);




/* @prog showdb ***************************************************************
**
** Displays information on the currently available databases
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjBool html;
    AjBool protein;
    AjBool nucleic;
    AjBool doheader;
    AjBool dotype;
    AjBool doid;
    AjBool doqry;
    AjBool doall;
    AjBool domethod;
    AjBool dofields;
    AjBool dodefined;
    AjBool dorelease;
    AjBool docomment;
    AjBool only;

    AjPFile outfile = NULL;
    AjPStr dbname   = NULL;	/* the next database name to look at */
    AjPStr type     = NULL;
    AjBool id;
    AjBool qry;
    AjBool all;
    AjPStr methods = NULL;
    AjPStr defined = NULL;
    AjPStr release = NULL;
    AjPStr comment = NULL;

    AjPList dbnames;
    AjIList iter = NULL;

    ajint maxname = 14;
    ajint maxmethod = 6;
    ajint maxfield = 6;
    ajint maxtype = 4;
    ajint maxdefined = 7;
    ajint maxrelease = 7;

    ajNamSetControl("namvalid");	/* validate database/resource defs */

    embInit("showdb", argc, argv);

    dbname  = ajAcdGetString("database");
    outfile = ajAcdGetOutfile("outfile");

    html    = ajAcdGetBool("html");
    protein = ajAcdGetBool("protein");
    nucleic = ajAcdGetBool("nucleic");

    doheader  = ajAcdGetBool("heading");
    dotype    = ajAcdGetBool("type");
    doid      = ajAcdGetBool("id");
    doqry     = ajAcdGetBool("query");
    doall     = ajAcdGetBool("all");
    dofields  = ajAcdGetBool("fields");
    dodefined = ajAcdGetBool("defined");
    domethod  = ajAcdGetBool("methods");
    docomment = ajAcdGetBool("comment");
    dorelease = ajAcdGetBool("release");
    only      = ajAcdGetBool("only");
    
    dbnames = ajListstrNew();
    
    
    /* start the HTML table */
    if(html)
	ajFmtPrintF(outfile, "<table border cellpadding=4 bgcolor="
		    "\"#FFFFF0\">\n");
    
    
    /* Just one specified name to get details on? */
    if(ajStrLen(dbname))
    {
	if(ajNamDbDetails(dbname, &type, &id, &qry, &all, &comment,
			  &release, &methods, &defined))
	    showdbDBWidth(dbname, type, methods, defined, release,
			   &maxname, &maxtype, &maxmethod,
			   &maxfield, &maxdefined, &maxrelease);

	/* print the header information */
	if(doheader)
	{
	    showdbDBHead(outfile, html, dotype, doid, doqry,
			  doall, domethod, dofields, dodefined,
			  docomment, dorelease,
			  maxname, maxtype, maxmethod, maxfield,
			  maxdefined, maxrelease);
    
    	    showdbDBOut(outfile, dbname, type, id, qry, all,
			 methods, defined,
			 comment, release, html,
			 dotype, doid, doqry, doall, 
			 domethod, dofields, dodefined,
			 docomment, dorelease,
			 maxname, maxtype,
			 maxmethod, maxfield, maxdefined, maxrelease);
	}
	else
	    ajFatal("The database '%S' does not exist", dbname);
    }
    else
    {
	/* get the list of database names */
	ajNamListListDatabases(dbnames);

	/* sort it */
	/*ajListSort(dbnames, ajStrCmp);*/
	ajListSort(dbnames, showdbDBSortDefined);

	/* iterate through the dbnames list */
	iter = ajListIterRead(dbnames);

	maxname = 14;
	maxmethod = 6;
	maxfield = 6;
	maxtype = 4;
	maxrelease = 7;

	/* find the field widths */
	while((dbname = ajListIterNext(iter)) != NULL)
	{
	    if(ajNamDbDetails(dbname, &type, &id, &qry, &all, &comment,
			      &release, &methods, &defined))
	    showdbDBWidth(dbname, type, methods, defined, release,
			   &maxname, &maxtype, &maxmethod,
			   &maxfield, &maxdefined, &maxrelease);
	}

	/* print the header information */
	if(doheader)
	    showdbDBHead(outfile, html, dotype, doid, doqry,
			  doall, domethod, dofields, dodefined,
			  docomment, dorelease,
			  maxname, maxtype, maxmethod, maxfield,
			  maxdefined, maxrelease);
    
	/* iterate through the dbnames list */
	iter = ajListIterRead(dbnames);

	/* write out protein databases */
	while((dbname = ajListIterNext(iter)) != NULL)
	    if(ajNamDbDetails(dbname, &type, &id, &qry, &all, &comment,
			      &release, &methods, &defined))
	    {
		if(protein &&
		   (ajStrMatchC(type, "P") || ajStrMatchC(type, "Protein")))
		   showdbDBOut(outfile, dbname, type, id, qry, all,
				 methods, defined,
				 comment, release, html, dotype, doid,
				 doqry, doall, domethod, dofields, dodefined,
				 docomment, 
				 dorelease, maxname,maxtype,  maxmethod,
				 maxfield, maxdefined, maxrelease);
	    }
	    else
		ajFatal("The database '%S' does not exist", dbname);


	/* reset the iterator */
	ajListIterFree(&iter);
	iter = ajListIterRead(dbnames);

	/* now write out nucleic databases */
	while((dbname = ajListIterNext(iter)) != NULL)
	{
	    if(ajNamDbDetails(dbname, &type, &id, &qry, &all, &comment,
			      &release, &methods, &defined))
	    {
		if( nucleic &&
		   (ajStrMatchC(type, "N") || ajStrMatchC(type, "Nucleotide")))
		    showdbDBOut(outfile, dbname, type, id, qry, all,
				 methods, defined,
				 comment, release, html,
				 dotype, doid,
				 doqry, doall, domethod, dofields, dodefined,
				 docomment, dorelease,
				 maxname, maxtype, maxmethod,
				 maxfield, maxdefined, maxrelease);
	    }
	    else
		ajFatal("The database '%S' does not exist", dbname);
	}

	ajListIterFree(&iter);
	ajListDel(&dbnames);
    }
    
    /* end the HTML table */
    if(html)
	ajFmtPrintF(outfile, "</table>\n");
    
    ajFileClose(&outfile);
    
    ajExit();

    return 0;
}


/* @funcstatic showdbDBWidth **************************************************
**
** Update maximum width for variable length text
**
** @param [r] dbname [const AjPStr] database name
** @param [r] type [const AjPStr] database type
** @param [r] methods [const AjPStr] database access method(s)
** @param [r] defined [const AjPStr] database definition file short name
** @param [r] release [const AjPStr] database release number
** @param [u] maxname [ajint*] Maximum width for name
** @param [u] maxtype [ajint*] Maximum width for type
** @param [u] maxmethod [ajint*] Maximum width for list of access methods
** @param [u] maxfield [ajint*] Maximum width for list of fields
** @param [u] maxdefined [ajint*] Maximum width for definition file
** @param [u] maxrelease [ajint*] Maximum width for release number
******************************************************************************/

static void showdbDBWidth (const AjPStr dbname,
			   const AjPStr type, const AjPStr methods,
			   const AjPStr defined, const AjPStr release,
			   ajint *maxname, ajint* maxtype, ajint *maxmethod,
			   ajint *maxfield, ajint* maxdefined,
			   ajint *maxrelease)
{
    ajint i;

    if (ajStrLen(dbname) > *maxname)
	*maxname = ajStrLen(dbname);

    if (ajStrLen(type) > *maxtype)
	*maxtype = ajStrLen(type);

    if (ajStrLen(methods) > *maxmethod)
	*maxmethod = ajStrLen(methods);

    if (ajStrLen(defined) > *maxdefined)
	*maxdefined = ajStrLen(defined);

    if (ajStrLen(release) > *maxrelease)
	*maxrelease = ajStrLen(release);

    i = ajStrLen(showdbGetFields(dbname));
    if (i > *maxfield)
	*maxfield = i;
    return;

    return;
}

/* @funcstatic showdbDBHead ***************************************************
**
** Output header for db information
**
** @param [w] outfile [AjPFile] outfile
** @param [r] html [AjBool] do html
** @param [r] dotype [AjBool] show type
** @param [r] doid [AjBool] show id
** @param [r] doqry [AjBool] show query status
** @param [r] doall [AjBool] show everything
** @param [r] domethod [AjBool] show access method(s)
** @param [r] dofields [AjBool] show query fields
** @param [r] dodefined [AjBool] show definition file
** @param [r] docomment [AjBool] show comment
** @param [r] dorelease [AjBool] show release
** @param [r] maxname [ajint] Maximum width for name
** @param [r] maxtype [ajint] Maximum width for type
** @param [r] maxmethod [ajint] Maximum width for access method
** @param [r] maxfield [ajint] Maximum width for list of fields
** @param [r] maxdefined [ajint] Maximum width for definition file
** @param [r] maxrelease [ajint] Maximum width for release number
******************************************************************************/

static void showdbDBHead (AjPFile outfile, AjBool html, AjBool dotype,
			  AjBool doid, AjBool doqry, AjBool doall,
			  AjBool domethod, AjBool dofields, AjBool dodefined,
			  AjBool docomment, AjBool dorelease,
			  ajint maxname, ajint maxtype, ajint maxmethod,
			  ajint maxfield, ajint maxdefined, ajint maxrelease)
{

    if(html)
	/* start the HTML table title line and output the Name header */
	ajFmtPrintF(outfile, "<tr><th>Name</th>");
    else
	ajFmtPrintF(outfile, "# Name%*s ", maxname-6, " ");

    if(dotype)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>Type</th>");
	else
	    ajFmtPrintF(outfile, "Type%*s ", maxtype-4, " ");
    }

    if(doid)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>ID</th>");
	else
	    ajFmtPrintF(outfile, "ID  ");
    }

    if(doqry)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>Qry</th>");
	else
	    ajFmtPrintF(outfile, "Qry ");
    }

    if(doall)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>All</th>");
	else
	    ajFmtPrintF(outfile, "All ");
    }

    if(domethod)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>Method</th>");
	else
	    ajFmtPrintF(outfile, "Method%*s ", maxmethod-6, " ");
    }

    if(dofields)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>Fields</th>");
	else
	    ajFmtPrintF(outfile, "Fields%*s ", maxfield-6, " ");
    }

    if(dodefined)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>Defined</th>");
	else
	    ajFmtPrintF(outfile, "Defined%*s ", maxdefined-7, " ");
    }

    if(dorelease)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>Release</th>");
	else
	    ajFmtPrintF(outfile, "Release%*s ", maxrelease-7, " ");
    }

    if(docomment)
    {
	if(html)
	    ajFmtPrintF(outfile, "<th>Comment</th>");
	else
	    ajFmtPrintF(outfile, "Comment");
    }

    if(html)
	/* end the HTML table title line */
	ajFmtPrintF(outfile, "</tr>\n");
    else
    {
	ajFmtPrintF(outfile, "\n");
	
	ajFmtPrintF(outfile, "# %.*s ", maxname-2,
		    "=====================================================");
	if(dotype)
	    ajFmtPrintF(outfile, "%.*s ", maxtype,
			"==================================================");

	if(doid)
	    ajFmtPrintF(outfile, "==  ");

	if(doqry)
	    ajFmtPrintF(outfile, "=== ");

	if(doall)
	    ajFmtPrintF(outfile, "=== ");

	if(domethod)
	    ajFmtPrintF(outfile, "%.*s ", maxmethod,
			"==================================================");

	if(dofields)
	    ajFmtPrintF(outfile, "%.*s ", maxfield,
			"==================================================");

	if(dodefined)
	    ajFmtPrintF(outfile, "%.*s ", maxdefined,
			"==================================================");

	if(dorelease)
	    ajFmtPrintF(outfile, "%.*s ", maxrelease,
			"==================================================");

	if(docomment)
	    ajFmtPrintF(outfile, "=======");

	ajFmtPrintF(outfile, "\n");
    }
    return;
}

/* @funcstatic showdbDBOut ****************************************************
**
** Output db information
**
** @param [w] outfile [AjPFile] outfile
** @param [r] dbname [const AjPStr] database name
** @param [r] type [const AjPStr] type
** @param [r] id [AjBool] id
** @param [r] qry [AjBool] queryable
** @param [r] all [AjBool] all info
** @param [r] methods [const AjPStr] db access method(s)
** @param [r] defined [const AjPStr] db definition file short name
** @param [r] comment [const AjPStr] db comment
** @param [r] release [const AjPStr] db release
** @param [r] html [AjBool] do html
** @param [r] dotype [AjBool] show type
** @param [r] doid [AjBool] show id
** @param [r] doqry [AjBool] show query status
** @param [r] doall [AjBool] show everything
** @param [r] domethod [AjBool] show access method(s)
** @param [r] dofields [AjBool] show query fields
** @param [r] dodefined [AjBool] show access method(s)
** @param [r] docomment [AjBool] show comment
** @param [r] dorelease [AjBool] show release
** @param [r] maxname [ajint] Maximum width for name
** @param [r] maxtype [ajint] Maximum width for type
** @param [r] maxmethod [ajint] Maximum width for list of access methods
** @param [r] maxfield [ajint] Maximum width for list of fields
** @param [r] maxdefined [ajint] Maximum width for definition file
** @param [r] maxrelease [ajint] Maximum width for release number
** @@
******************************************************************************/

static void showdbDBOut(AjPFile outfile,
			const AjPStr dbname, const AjPStr type,
			AjBool id, AjBool qry, AjBool all,
			const AjPStr methods, const AjPStr defined,
			const AjPStr comment, const AjPStr release,
			AjBool html, AjBool dotype,
			AjBool doid, AjBool doqry, AjBool doall,
			AjBool domethod, AjBool dofields, AjBool dodefined,
			AjBool docomment, AjBool dorelease,
			ajint maxname, ajint maxtype, ajint maxmethod,
			ajint maxfield, ajint maxdefined, ajint maxrelease)
{

    if(html)
	/* start table line and output name */
	ajFmtPrintF(outfile, "<tr><td>%S</td>", dbname);
    else
    {
	ajFmtPrintF(outfile, "%-*S ", maxname, dbname);
    }

    if(dotype)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>%S</td>", type);
	else
	    ajFmtPrintF(outfile, "%-*S ", maxtype, type);
    }

    if(doid)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

	if(id)
	    ajFmtPrintF(outfile, "%s", "OK  ");
	else
	    ajFmtPrintF(outfile, "%s", "-   ");

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }

    if(doqry)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

	if(qry)
	    ajFmtPrintF(outfile, "%s", "OK  ");
	else
	    ajFmtPrintF(outfile, "%s", "-   ");

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }

    if(doall)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

	if(all)
	    ajFmtPrintF(outfile, "%s", "OK  ");
	else
	    ajFmtPrintF(outfile, "%s", "-   ");

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }

    if(domethod)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

        ajFmtPrintF(outfile, "%-*S ", maxmethod, methods);

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }

    if(dofields)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

        ajFmtPrintF(outfile, "%-*S ", maxfield, showdbGetFields(dbname));

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }

    if(dodefined)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

        ajFmtPrintF(outfile, "%-*S ", maxdefined, defined);

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }

    if(dorelease)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

	if(release != NULL)
	    ajFmtPrintF(outfile, "%-*S ", maxrelease, release);
	else
	    ajFmtPrintF(outfile, "%-*s ", maxrelease, "");

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }


    if(docomment)
    {
	if(html)
	    ajFmtPrintF(outfile, "<td>");

	if(comment != NULL)
	    ajFmtPrintF(outfile, "%S", comment);
	else
	    ajFmtPrintF(outfile, "-");

	if(html)
	    ajFmtPrintF(outfile, "</td>");
    }

    if(html)
	ajFmtPrintF(outfile, "</tr>\n");	/* end table line */
    else
	ajFmtPrintF(outfile, "\n");

    return;
}




/* @funcstatic showdbGetFields ***********************************************
**
** Get a database's valid query fields (apart from the default 'id' and 'acc')
**
** @param [r] dbname [const AjPStr] database name
** @return [AjPStr] the available search fields
** @@
******************************************************************************/

static AjPStr showdbGetFields(const AjPStr dbname)
{
    static AjPStr str = NULL;
    AjPSeqQuery query;


    query = ajSeqQueryNew();

    ajStrAssS(&query->DbName, dbname);
    ajNamDbData(query);
    ajStrAssS(&str, query->DbFields);

    /* if there are no query fields, then change to a '_' */
    if(!ajStrLen(str))
  	ajStrAssC(&str, "-     ");
    else
	/* change spaces to commas to make the result one word */
	ajStrConvertCC(&str, " ", ",");

    return str;
}


/* @funcstatic showdbDBSortDefined ********************************************
**
** Compares the value of two strings for use in sorting (e.g. ajListSort)
**
** @param [r] str1 [const void*] First string
** @param [r] str2 [const void*] Second string
** @return [int] -1 if first string should sort before second, +1 if the
**         second string should sort first. 0 if they are identical
**         in length and content.
** @category use [AjPStr] String compare
** @@
******************************************************************************/

static int showdbDBSortDefined(const void* str1, const void* str2)
{
    AjPStr db1 = *(AjPStr*) str1;
    AjPStr db2 = *(AjPStr*) str2;

    AjPStr methods = NULL;
    AjPStr release = NULL;
    AjPStr comment = NULL;
    AjPStr type    = NULL;
    AjBool id;
    AjBool qry;
    AjBool all;
    AjPStr defined1 = NULL;
    AjPStr defined2 = NULL;

    int ret;

    if(ajNamDbDetails(db1, &type, &id, &qry, &all, &comment,
		      &release, &methods, &defined1) &&
       ajNamDbDetails(db2, &type, &id, &qry, &all, &comment,
		      &release, &methods, &defined2))
    {
	ret = ajStrCmpO(defined1, defined2);
	ajDebug("Sorting1 %S:%S %S:%S %d\n", db1, db2, defined1, defined2, ret);
	if (ret) return ret;
    }

    ret = ajStrCmpO(db1, db2);
    ajDebug("Sorting2 %S:%S %d\n", db1, db2, ret);
    return ret;
}


