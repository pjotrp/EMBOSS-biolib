/* @source dbxstat application
**
** Statistics for a dbx database
**
** @author Copyright (C) 2007 Alan Bleasby (ajb@ebi.ac.uk)
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




/* @datastatic DbxstatNames *************************************************
**
** Index file names
**
** @alias DbxstatSNames
** @alias DbxstatONames
**
** @attr name [const char*] Field name
** @attr iname [const char*] Field index name
** @attr isid [AjBool] Field is an identifier (false for keywords)
** @@
******************************************************************************/

typedef struct DbxflatSNames
{
    const char* name;
    const char* iname;
    AjBool isid;
} DbxflatONames;
#define DbxflatPNames DbxflatONames*




static DbxflatONames inxnames[] =
{
    {"id", "xid", AJTRUE},
    {"ac", "xac", AJTRUE},
    {"sv", "xsv", AJTRUE},
    {"de", "xde", AJFALSE},
    {"kw", "xkw", AJFALSE},
    {"tx", "xtx", AJFALSE},
    {NULL, NULL, 0}
};




/* @prog dbxstat **************************************************************
**
** Statistics for a dbx index
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr   dbname = NULL;
    AjPStr   idir   = NULL;
    AjPStr   itype  = NULL;
    ajint    imin;
    ajint    imax;
    AjPFile  outf = NULL;

    const char *basenam;
    const char *baseidir;
    
    ajint i;

    ajint order;
    ajint nperbucket;
    ajint pagesize;
    ajint level;
    ajint cachesize;
    ajint sorder;
    ajint snperbucket;
    ajint kwlimit;
    ajlong count;
    ajlong countall;
    ajint idorder;
    ajint idnperbucket;
    ajint idpagesize;
    ajint idlevel;
    ajint idcachesize;
    ajint idsorder;
    ajint idsnperbucket;
    ajint idkwlimit;
    ajlong idcount;
    ajlong idcountall;

    AjPBtcache cache = NULL;
    AjPBtcache idcache = NULL;
    
    embInit("dbxstat", argc, argv);
    
    dbname = ajAcdGetString("dbname");
    idir   = ajAcdGetDirectoryName("indexdir");
    itype  = ajAcdGetListSingle("idtype");
    imin   = ajAcdGetInt("minimum");
    imax   = ajAcdGetInt("maximum");
    outf   = ajAcdGetOutfile("outfile");
    
    basenam  = ajStrGetPtr(dbname);
    baseidir = ajStrGetPtr(idir);
    
    i = 0;
    while(inxnames[i].name)
    {
	if(ajStrMatchC(itype,inxnames[i].name))
	    break;
	++i;
    }
    if(!inxnames[i].name)
	ajFatal("Unrecognised index type");
    

    ajBtreeReadParams(basenam,inxnames[i].iname,baseidir,
		      &order,&nperbucket,&pagesize,&level,
		      &cachesize,&sorder,&snperbucket,
		      &count,&countall,&kwlimit);
    
    ajBtreeReadParams(basenam,"xid",baseidir,
		      &idorder,&idnperbucket,&idpagesize,&idlevel,
		      &idcachesize,&idsorder,&idsnperbucket,
		      &idcount,&idcountall,&idkwlimit);
    cache = ajBtreeSecCacheNewC(basenam,inxnames[i].iname,baseidir,"r+",
				pagesize, order, nperbucket, level,
				cachesize, sorder, 0, snperbucket,
				count, countall, kwlimit);
    idcache = ajBtreeSecCacheNewC(basenam,"xid",baseidir,"r+",
				idpagesize, idorder, idnperbucket, idlevel,
				idcachesize, idsorder, 0, idsnperbucket,
                                idcount, idcountall, idkwlimit);
    if(!cache)
	ajFatal("Cannot open index file '%s' for reading",
                inxnames[i].name);
    

    if(inxnames[i].isid)
        ajBtreeDumpHybKeys(cache,imin,imax,outf);
    else
        ajBtreeDumpKeywords(cache,idcache,imin,imax,outf);

    ajStrDel(&dbname);
    ajStrDel(&idir);

    ajBtreeCacheDel(&cache);
    ajFileClose(&outf);
    
    embExit();
    
    return 0;
}
