/* @source embindex.c
**
** B+ Tree Indexing plus Disc Cache.
** Copyright (c) 2003 Alan Bleasby
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

#define BTENTRYFILE     ".ent"
#define KWLIMIT 12




static AjPFile btreeCreateFile(const AjPStr idirectory, const AjPStr dbname,
			       const char *add);




/* @func embBtreeEmblKW **************************************************
**
** Extract keywords from an EMBL KW line 
**
** @param [r] kwline[const AjPStr] keyword line
** @param [w] kwlist [AjPList] list of keywords
** @param [w] maxlen [ajint] max keyword length
**
** @return [void]
** @@
******************************************************************************/

void embBtreeEmblKW(const AjPStr kwline, AjPList kwlist, ajint maxlen)
{
    AjPStr line      = NULL;
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    
    line  = ajStrNew();
    token = ajStrNew();
    
    ajStrAssC(&line, &MAJSTRSTR(kwline)[5]);

    handle = ajStrTokenInit(line,"\n;");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrTrimEndC(&token,".");
	ajStrChomp(&token);
	if(ajStrLen(token))
	{
	    str = ajStrNew();
	    if(maxlen)
	    {
		if(ajStrLen(token) > maxlen)
		    ajStrAssSub(&str,token,0,maxlen-1);
		else
		    ajStrAssS(&str,token);
		
	    }
	    else
		ajStrAssS(&str,token);
	    ajStrToLower(&str);
	    ajListPush(kwlist,(void *)str);
	}
    }

    ajStrDel(&token);
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    
    return;
}




/* @func embBtreeEmblAC **************************************************
**
** Extract accession numbers from an EMBL AC line 
**
** @param [r] acline[const AjPStr] AC line
** @param [w] aclist [AjPList] list of accession numbers
**
** @return [void]
** @@
******************************************************************************/

void embBtreeEmblAC(const AjPStr acline, AjPList aclist)
{
    AjPStr line      = NULL;
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    AjPStr tstr      = NULL;
    AjPStr prefix    = NULL;
    AjPStr format    = NULL;
    char *p          = NULL;
    char *q          = NULL;
    ajint lo = 0;
    ajint hi = 0;
    ajint field = 0;
    ajint i;
    
    line   = ajStrNew();
    token  = ajStrNew();
    tstr   = ajStrNew();
    prefix = ajStrNew();
    format = ajStrNew();
    
    ajStrAssC(&line, &MAJSTRSTR(acline)[5]);

    handle = ajStrTokenInit(line,"\n;");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrChomp(&token);
	if((p=strchr(MAJSTRSTR(token),(int)'-')))
	{
	    q = p;
	    while(isdigit((int)*(--q)));
	    ++q;
	    ajStrAssSubC(&tstr,q,0,p-q-1);
	    ajStrToInt(tstr,&lo);
	    field = p-q;
	    ajFmtPrintS(&format,"%%S%%0%dd",field);
	    
	    ++p;
	    q = p;
	    while(!isdigit((int)*q))
		++q;
	    sscanf(q,"%d",&hi);
	    ajStrAssSubC(&prefix,p,0,q-p-1);
	    
	    for(i=lo;i<=hi;++i)
	    {
		str = ajStrNew();
		ajFmtPrintS(&str,MAJSTRSTR(format),prefix,i);
		ajListPush(aclist,(void *)str);
	    }
	}
	else
	{
	    str = ajStrNew();
	    ajStrAssS(&str,token);
	    ajListPush(aclist,(void *)str);
	}
    }

    ajStrDel(&tstr);
    ajStrDel(&prefix);
    ajStrDel(&format);
    ajStrDel(&token);
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    
    return;
}




/* @func embBtreeEmblDE **************************************************
**
** Extract keywords from an EMBL DE line 
**
** @param [r] deline[const AjPStr] description line
** @param [w] delist [AjPList] list of descriptions
** @param [w] maxlen [ajint] max keyword length
**
** @return [void]
** @@
******************************************************************************/

void embBtreeEmblDE(const AjPStr deline, AjPList delist, ajint maxlen)
{
    AjPStr line      = NULL;
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    
    line  = ajStrNew();
    token = ajStrNew();
    
    ajStrAssC(&line, &MAJSTRSTR(deline)[5]);

    handle = ajStrTokenInit(line,"\n,");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrTrimEndC(&token,".");
	ajStrChomp(&token);
	if(ajStrLen(token))
	{
	    str = ajStrNew();
	    if(maxlen)
	    {
		if(ajStrLen(token) > maxlen)
		    ajStrAssSub(&str,token,0,maxlen-1);
		else
		    ajStrAssS(&str,token);
		
	    }
	    else
		ajStrAssS(&str,token);
	    ajStrToLower(&str);
	    ajListPush(delist,(void *)str);
	}
    }

    ajStrDel(&token);
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    
    return;
}




/* @func embBtreeGenBankAC **************************************************
**
** Extract accession numbers from a GenBank ACCESSION line 
**
** @param [r] acline[const AjPStr] AC line
** @param [w] aclist [AjPList] list of accession numbers
**
** @return [void]
** @@
******************************************************************************/

void embBtreeGenBankAC(const AjPStr acline, AjPList aclist)
{
    AjPStr line      = NULL;
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    AjPStr tstr      = NULL;
    AjPStr prefix    = NULL;
    AjPStr format    = NULL;
    char *p          = NULL;
    char *q          = NULL;
    ajint lo = 0;
    ajint hi = 0;
    ajint field = 0;
    ajint i;
    
    line   = ajStrNew();
    token  = ajStrNew();
    tstr   = ajStrNew();
    prefix = ajStrNew();
    format = ajStrNew();
    
    ajStrAssC(&line, &MAJSTRSTR(acline)[12]);

    handle = ajStrTokenInit(line,"\n ");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrChomp(&token);
	if((p=strchr(MAJSTRSTR(token),(int)'-')))
	{
	    q = p;
	    while(isdigit((int)*(--q)));
	    ++q;
	    ajStrAssSubC(&tstr,q,0,p-q-1);
	    ajStrToInt(tstr,&lo);
	    field = p-q;
	    ajFmtPrintS(&format,"%%S%%0%dd",field);
	    
	    ++p;
	    q = p;
	    while(!isdigit((int)*q))
		++q;
	    sscanf(q,"%d",&hi);
	    ajStrAssSubC(&prefix,p,0,q-p-1);
	    
	    for(i=lo;i<=hi;++i)
	    {
		str = ajStrNew();
		ajFmtPrintS(&str,MAJSTRSTR(format),prefix,i);
		ajListPush(aclist,(void *)str);
	    }
	}
	else
	{
	    str = ajStrNew();
	    ajStrAssS(&str,token);
	    ajListPush(aclist,(void *)str);
	}
    }

    ajStrDel(&tstr);
    ajStrDel(&prefix);
    ajStrDel(&format);
    ajStrDel(&token);
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    
    return;
}




/* @func embBtreeGenBankKW **************************************************
**
** Extract keywords from a GenBank KEYWORDS line 
**
** @param [r] kwline[const AjPStr] keyword line
** @param [w] kwlist [AjPList] list of keywords
** @param [w] maxlen [ajint] max keyword length
**
** @return [void]
** @@
******************************************************************************/

void embBtreeGenBankKW(const AjPStr kwline, AjPList kwlist, ajint maxlen)
{
    AjPStr line      = NULL;
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    
    line  = ajStrNew();
    token = ajStrNew();
    
    ajStrAssC(&line, &MAJSTRSTR(kwline)[8]);

    handle = ajStrTokenInit(line,"\n;");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrTrimEndC(&token,".");
	ajStrChomp(&token);
	if(ajStrLen(token))
	{
	    str = ajStrNew();
	    if(maxlen)
	    {
		if(ajStrLen(token) > maxlen)
		    ajStrAssSub(&str,token,0,maxlen-1);
		else
		    ajStrAssS(&str,token);
		
	    }
	    else
		ajStrAssS(&str,token);
	    ajStrToLower(&str);
	    ajListPush(kwlist,(void *)str);
	}
    }

    ajStrDel(&token);
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    
    return;
}




/* @func embBtreeGenBankDE **************************************************
**
** Extract keywords from a GenBank DESCRIPTION line 
**
** @param [r] kwline[const AjPStr] keyword line
** @param [w] kwlist [AjPList] list of keywords
** @param [w] maxlen [ajint] max keyword length
**
** @return [void]
** @@
******************************************************************************/

void embBtreeGenBankDE(const AjPStr kwline, AjPList kwlist, ajint maxlen)
{
    AjPStr line      = NULL;
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    
    line  = ajStrNew();
    token = ajStrNew();
    
    ajStrAssC(&line, &MAJSTRSTR(kwline)[10]);

    handle = ajStrTokenInit(line,"\n ");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrTrimEndC(&token,".");
	ajStrChomp(&token);
	if(ajStrLen(token))
	{
	    str = ajStrNew();
	    if(maxlen)
	    {
		if(ajStrLen(token) > maxlen)
		    ajStrAssSub(&str,token,0,maxlen-1);
		else
		    ajStrAssS(&str,token);
		
	    }
	    else
		ajStrAssS(&str,token);
	    ajStrToLower(&str);
	    ajListPush(kwlist,(void *)str);
	}
    }

    ajStrDel(&token);
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    
    return;
}




/* @func embBtreeGenBankTX **************************************************
**
** Extract keywords from a GenBank ORGANISM line 
**
** @param [r] kwline[const AjPStr] keyword line
** @param [w] kwlist [AjPList] list of keywords
** @param [w] maxlen [ajint] max keyword length
**
** @return [void]
** @@
******************************************************************************/

void embBtreeGenBankTX(const AjPStr kwline, AjPList kwlist, ajint maxlen)
{
    AjPStr line      = NULL;
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    
    line  = ajStrNew();
    token = ajStrNew();
    
    ajStrAssC(&line, &MAJSTRSTR(kwline)[9]);

    handle = ajStrTokenInit(line,"\n;");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrTrimEndC(&token,".");
	ajStrChomp(&token);
	if(ajStrLen(token))
	{
	    str = ajStrNew();
	    if(maxlen)
	    {
		if(ajStrLen(token) > maxlen)
		    ajStrAssSub(&str,token,0,maxlen-1);
		else
		    ajStrAssS(&str,token);
		
	    }
	    else
		ajStrAssS(&str,token);
	    ajStrToLower(&str);
	    ajListPush(kwlist,(void *)str);
	}
    }

    ajStrDel(&token);
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    
    return;
}




/* @func embBtreeFastaDE **************************************************
**
** Extract keywords from a Fasta description
**
** @param [r] kwline[const AjPStr] keyword line
** @param [w] kwlist [AjPList] list of keywords
** @param [w] maxlen [ajint] max keyword length
**
** @return [void]
** @@
******************************************************************************/

void embBtreeFastaDE(const AjPStr kwline, AjPList kwlist, ajint maxlen)
{
    AjPStrTok handle = NULL;
    AjPStr token     = NULL;
    AjPStr str       = NULL;
    
    token = ajStrNew();
    
    handle = ajStrTokenInit(kwline,"\n ");

    while(ajStrToken(&token,&handle,NULL))
    {
	ajStrTrimEndC(&token,".");
	ajStrChomp(&token);
	if(ajStrLen(token))
	{
	    str = ajStrNew();
	    if(maxlen)
	    {
		if(ajStrLen(token) > maxlen)
		    ajStrAssSub(&str,token,0,maxlen-1);
		else
		    ajStrAssS(&str,token);
		
	    }
	    else
		ajStrAssS(&str,token);
	    ajStrToLower(&str);
	    ajListPush(kwlist,(void *)str);
	}
    }

    ajStrDel(&token);
    ajStrTokenClear(&handle);
    
    return;
}




/* @func embBtreeReadDir ******************************************************
**
** Read files to index
**
** @param [w] filelist [AjPStr**] list of files to read
** @param [r] fdirectory [const AjPStr] Directory to scan
** @param [r] files [const AjPStr] Filename to search for (or NULL)
** @param [r] exclude [const AjPStr] list of files to exclude
**
** @return [ajint] number of matching files
** @@
******************************************************************************/

ajint embBtreeReadDir(AjPStr **filelist, const AjPStr fdirectory,
		      const AjPStr files, const AjPStr exclude)
{
    AjPList lfiles = NULL;
    ajint nfiles;
    ajint nremove;
    ajint i;
    ajint j;
    AjPStr file    = NULL;
    AjPStr *remove = NULL;

    /* ajDebug("In ajBtreeReadDir\n"); */

    lfiles = ajListNew();
    nfiles = ajFileScan(fdirectory,files,&lfiles,ajFalse,ajFalse,NULL,NULL,
			ajFalse,NULL);

    nremove = ajArrCommaList(exclude,&remove);
    
    for(i=0;i<nfiles;++i)
    {
	ajListPop(lfiles,(void **)&file);
	ajSysBasename(&file);
	for(j=0;j<nremove && ! ajStrMatchWild(file,remove[j]);++j);
	if(j == nremove)
	    ajListPushApp(lfiles,(void *)file);
    }

    nfiles =  ajListToArray(lfiles,(void ***)&(*filelist));
    ajListDel(&lfiles);

    for(i=0; i<nremove;++i)
	ajStrDel(&remove[i]);
    AJFREE(remove);

    return nfiles;
}




/* @func embBtreeWriteFileList ***********************************************
**
** Read files to index
**
** @param [r] filelist [const AjPStr*] list of files
** @param [r] nfiles [ajint] number of files
** @param [r] fdirectory [const AjPStr] flatfile directory
** @param [r] idirectory [const AjPStr] index directory
** @param [r] dbname [const AjPStr] name of database
**
** @return [AjBool] true if success
** @@
******************************************************************************/

AjBool embBtreeWriteFileList(const AjPStr *filelist, ajint nfiles,
			     const AjPStr fdirectory, const AjPStr idirectory,
			     const AjPStr dbname)
{
    AjPFile entfile = NULL;
    ajint i;
    
    /* ajDebug("In ajBtreeWriteFileList\n"); */

    entfile = btreeCreateFile(idirectory,dbname,BTENTRYFILE);
    if(!entfile)
	return ajFalse;
    
    ajFmtPrintF(entfile,"#Number of files\t%d\n",nfiles);
    for(i=0;i<nfiles;++i)
	ajFmtPrintF(entfile,"%S/%S\n",fdirectory,filelist[i]);

    ajFileClose(&entfile);
    
    return ajTrue;
}




/* @funcstatic btreeCreateFile ************************************************
**
** Open B+tree file for writing
**
** @param [r] idirectory [const AjPStr] Directory for index files
** @param [r] dbname [const AjPStr] name of database
** @param [r] add [const char *] type of file
**
** @return [AjPFile] opened file
** @@
******************************************************************************/

static AjPFile btreeCreateFile(const AjPStr idirectory, const AjPStr dbname,
			       const char *add)
{
    AjPStr filename = NULL;
    AjPFile fp      = NULL;
    
    /* ajDebug("In btreeCreateFile\n"); */

    filename = ajStrNew();

    ajFmtPrintS(&filename,"%S/%S%s",idirectory,dbname,add);
    
    fp =  ajFileNewOut(filename);

    ajStrDel(&filename);
    return fp;
}




/* @func embBtreeEntryNew ***********************************************
**
** Construct a database entry object
**
** @return [EmbPBtreeEntry] db entry object pointer
** @@
******************************************************************************/

EmbPBtreeEntry embBtreeEntryNew()
{
    EmbPBtreeEntry thys;

    AJNEW0(thys);

    thys->do_id          = ajFalse;
    thys->do_accession   = ajFalse;
    thys->do_sv          = ajFalse;
    thys->do_keyword     = ajFalse;
    thys->do_taxonomy    = ajFalse;
    thys->do_description = ajFalse;

    thys->dbname  = ajStrNew();
    thys->dbrs    = ajStrNew();
    thys->date    = ajStrNew();
    thys->release = ajStrNew();
    thys->dbtype  = ajStrNew();

    thys->directory  = ajStrNew();
    thys->idirectory = ajStrNew();
    
    thys->files = ajListNew();

    thys->id = ajStrNew();
    thys->ac = ajListNew();
    thys->de = ajListNew();
    thys->sv = ajListNew();
    thys->kw = ajListNew();
    thys->tx = ajListNew();

    return thys;
}




/* @func embBtreeEntryDel ***********************************************
**
** Delete a database entry object
**
** @return [void]
** @@
******************************************************************************/

void embBtreeEntryDel(EmbPBtreeEntry *thys)
{
    EmbPBtreeEntry pthis;
    AjPStr tmpstr = NULL;
    
    pthis = *thys;

    ajStrDel(&pthis->dbname);
    ajStrDel(&pthis->dbrs);
    ajStrDel(&pthis->date);
    ajStrDel(&pthis->release);
    ajStrDel(&pthis->dbtype);

    ajStrDel(&pthis->directory);
    ajStrDel(&pthis->idirectory);


    while(ajListPop(pthis->files,(void **)&tmpstr))
	ajStrDel(&tmpstr);
    ajListDel(&pthis->files);

    ajStrDel(&pthis->id);
    ajListDel(&pthis->ac);
    ajListDel(&pthis->de);
    ajListDel(&pthis->sv);
    ajListDel(&pthis->kw);
    ajListDel(&pthis->tx);

    *thys = NULL;
    AJFREE(pthis);

    return;
}




/* @func embBtreeSetFields ***********************************************
**
** Set database fields to index
**
** @param [w] entry [EmbPDbEntry] Database entry information
** @param [r] fields [const AjPStr *] user specified fields
**
** @return [ajint] number of fields set
** @@
******************************************************************************/

ajint embBtreeSetFields(EmbPBtreeEntry entry, AjPStr *fields)
{
    ajint nfields;


    nfields = 0;
    while(fields[nfields])
    {
	if(ajStrMatchCaseC(fields[nfields], "id"))
	    entry->do_id = ajTrue;

	else if(ajStrMatchCaseC(fields[nfields], "ac"))
	    entry->do_accession = ajTrue;

	else if(ajStrMatchCaseC(fields[nfields], "sv"))
	    entry->do_sv = ajTrue;

	else if(ajStrMatchCaseC(fields[nfields], "de"))
	    entry->do_description = ajTrue;

	else if(ajStrMatchCaseC(fields[nfields], "kw"))
	    entry->do_keyword = ajTrue;

	else if(ajStrMatchCaseC(fields[nfields], "tx"))
	    entry->do_taxonomy = ajTrue;

	else
	    ajWarn("Parsing unknown field '%S': ignored",
		   fields[nfields]);
	++nfields;
    }
    
    return nfields;
}




/* @func embBtreeSetDbInfo ***********************************************
**
** Set general database information
**
** @param [w] entry [EmbPDbEntry] Database entry information
** @param [r] fields [const AjPStr] user specified fields
**
** @return [void]
** @@
******************************************************************************/

void embBtreeSetDbInfo(EmbPBtreeEntry entry, const AjPStr name,
		       const AjPStr dbrs,
		       const AjPStr date, const AjPStr release,
		       const AjPStr type, const AjPStr directory,
		       const AjPStr idirectory)
{
    ajStrAssS(&entry->dbname, name);
    ajStrAssS(&entry->date, date);
    ajStrAssS(&entry->release, release);
    ajStrAssS(&entry->dbtype, type);
    ajStrAssS(&entry->dbrs, dbrs);
    
    ajStrAssS(&entry->directory,directory);
    ajStrAssS(&entry->idirectory,idirectory);

    return;
}




/* @func embBtreeGetFiles *****************************************************
**
** Read files to index
**
** @param [u] entry [EmbPBtreeEntry] list of files to read
** @param [r] fdirectory [const AjPStr] Directory to scan
** @param [r] files [const AjPStr] Filename to search for (or NULL)
** @param [r] exclude [const AjPStr] list of files to exclude
**
** @return [ajint] number of matching files
** @@
******************************************************************************/

ajint embBtreeGetFiles(EmbPBtreeEntry entry, const AjPStr fdirectory,
		       const AjPStr files, const AjPStr exclude)
{
    ajint nfiles;
    ajint nremove;
    ajint i;
    ajint j;
    AjPStr file    = NULL;
    AjPStr *remove = NULL;
    ajint count = 0;
    
    /* ajDebug("In embBtreeGetFiles\n"); */

    nfiles = ajFileScan(fdirectory,files,&entry->files,ajFalse,ajFalse,NULL,
			NULL,ajFalse,NULL);

    nremove = ajArrCommaList(exclude,&remove);

    count = 0;
    for(i=0;i<nfiles;++i)
    {
	ajListPop(entry->files,(void **)&file);
	ajSysBasename(&file);
	for(j=0;j<nremove && !ajStrMatchWild(file,remove[j]);++j);
	if(j == nremove)
	{
	    ajListPushApp(entry->files,(void *)file);
	    ++count;
	}
    }

    ajListSort(entry->files,ajStrCmp);

    entry->nfiles = count;

    for(i=0; i<nremove;++i)
	ajStrDel(&remove[i]);
    AJFREE(remove);

    return count;
}




/* @func embBtreeWriteEntryFile ***********************************************
**
** Put files to entry file
**
** @param [r] entry [const EmbPBtreeEntry] database data
**
** @return [AjBool] true on success
** @@
******************************************************************************/

AjBool embBtreeWriteEntryFile(const EmbPBtreeEntry entry)
{
    AjPFile entfile = NULL;
    ajint i;
    AjPStr tmpstr = NULL;
    
    /* ajDebug("In embBtreeWriteEntryFile\n"); */

    entfile = btreeCreateFile(entry->idirectory,entry->dbname,BTENTRYFILE);
    if(!entfile)
	return ajFalse;
    
    ajFmtPrintF(entfile,"#Number of files\t%d\n",entry->nfiles);
    for(i=0;i<entry->nfiles;++i)
    {
	ajListPop(entry->files,(void **)&tmpstr);
	ajFmtPrintF(entfile,"%S%S\n",entry->directory,tmpstr);
	ajListPushApp(entry->files,(void *)tmpstr);
    }

    ajFileClose(&entfile);
    
    return ajTrue;
}




/* @func embBtreeGetRsInfo ***********************************************
**
** Get resource information for selected database
**
** @param [u] entry [const EmbPBtreeEntry] database data
** @param [r] database [const char *] database name
**
** @return [void]
** @@
******************************************************************************/

void embBtreeGetRsInfo(EmbPBtreeEntry entry)
{
    const char *resource;
    AjPStr value = NULL;
    ajint  n = 0;
    
    value = ajStrNew();
    

    resource = ajStrStr(entry->dbrs);
    if(!ajNamRsAttrValueC(resource,"type",&value))
	ajFatal("Missing resource entry (%S) for indexing",entry->dbrs);
    if(!ajStrMatchC(value,"Index"))
	ajFatal("Incorrect 'type' field for resource (%S)",entry->dbrs);

    if(!ajNamRsAttrValueC(resource,"order",&value))
	entry->order = BTREE_DEF_ORDER;
    else
    {
	if(ajStrToInt(value,&n))
	    entry->order = n;
	else
	{
	    ajErr("Bad value for index resource 'order'");
	    entry->order = BTREE_DEF_ORDER;
	}
    }
    
    if(!ajNamRsAttrValueC(resource,"fill",&value))
	entry->fill = BTREE_DEF_FILL;
    else
    {
	if(ajStrToInt(value,&n))
	    entry->fill = n;
	else
	{
	    ajErr("Bad value for index resource 'fill'");
	    entry->fill = BTREE_DEF_FILL;
	}
    }

    if(!ajNamRsAttrValueC(resource,"secorder",&value))
	entry->secorder = BTREE_DEF_SECORDER;
    else
    {
	if(ajStrToInt(value,&n))
	    entry->secorder = n;
	else
	{
	    ajErr("Bad value for index resource 'secorder'");
	    entry->secorder = BTREE_DEF_SECORDER;
	}
    }
    
    if(!ajNamRsAttrValueC(resource,"secfill",&value))
	entry->secfill = BTREE_DEF_SECFILL;
    else
    {
	if(ajStrToInt(value,&n))
	    entry->secfill = n;
	else
	{
	    ajErr("Bad value for index resource 'secfill'");
	    entry->secfill = BTREE_DEF_SECFILL;
	}
    }

    if(!ajNamRsAttrValueC(resource,"kwlimit",&value))
	entry->kwlimit = BTREE_DEF_KWLIMIT;
    else
    {
	if(ajStrToInt(value,&n))
	    entry->kwlimit = n;
	else
	{
	    ajErr("Bad value for index resource 'kwlimit'");
	    entry->kwlimit = BTREE_DEF_KWLIMIT;
	}
    }


    if(!ajNamGetValueC("CACHESIZE",&value))
	entry->cachesize = BTREE_DEF_CACHESIZE;
    else
    {
	if(ajStrToInt(value,&n))
	    entry->cachesize = n;
	else
	{
	    ajErr("Bad value for environment variable 'CACHESIZE'");
	    entry->cachesize = BTREE_DEF_CACHESIZE;
	}
    }

    if(!ajNamGetValueC("PAGESIZE",&value))
	entry->pagesize = BTREE_DEF_PAGESIZE;
    else
    {
	if(ajStrToInt(value,&n))
	    entry->pagesize = n;
	else
	{
	    ajErr("Bad value for environment variable 'PAGESIZE'");
	    entry->pagesize = BTREE_DEF_PAGESIZE;
	}
    }

    
    ajStrDel(&value);

    return;
}




/* @func embBtreeOpenCaches ***********************************************
**
** Open index files
**
** @param [u] entry [EmbPBtreeEntry] database data
**
** @return [AjBool] true on success
** @@
******************************************************************************/

AjBool embBtreeOpenCaches(EmbPBtreeEntry entry)
{
    char *basenam = NULL;
    char *idir    = NULL;
    ajint level   = 0;
    ajint slevel  = 0;
    ajint count   = 0;
    
    basenam = entry->dbname->Ptr;
    idir    = entry->idirectory->Ptr;


    if(entry->do_id)
    {
	entry->idcache = ajBtreeCacheNewC(basenam,ID_EXTENSION,idir,"w+",
					  entry->pagesize, entry->order,
					  entry->fill, level,
					  entry->cachesize);
	ajBtreeCreateRootNode(entry->idcache,0L);
    }

    if(entry->do_accession)
    {
	entry->accache = ajBtreeCacheNewC(basenam,AC_EXTENSION,idir,"w+",
					  entry->pagesize, entry->order,
					  entry->fill, level,
					  entry->cachesize);
	ajBtreeCreateRootNode(entry->accache,0L);
    }

    if(entry->do_sv)
    {
	entry->svcache = ajBtreeCacheNewC(basenam,SV_EXTENSION,idir,"w+",
					  entry->pagesize, entry->order,
					  entry->fill, level,
					  entry->cachesize);
	ajBtreeCreateRootNode(entry->svcache,0L);
    }


    if(entry->do_keyword)
    {
	entry->kwcache = ajBtreeSecCacheNewC(basenam,KW_EXTENSION,idir,"w+",
					     entry->pagesize,
					     entry->order, entry->fill, level,
					     entry->cachesize,
					     entry->secorder, slevel,
					     entry->secfill, count,
					     entry->kwlimit);
	ajBtreeCreateRootNode(entry->kwcache,0L);
    }
    
    if(entry->do_description)
    {
	entry->decache = ajBtreeSecCacheNewC(basenam,DE_EXTENSION,idir,"w+",
					     entry->pagesize,
					     entry->order, entry->fill, level,
					     entry->cachesize,
					     entry->secorder, slevel,
					     entry->secfill, count,
					     entry->kwlimit);
	ajBtreeCreateRootNode(entry->decache,0L);
    }
    
    if(entry->do_taxonomy)
    {
	entry->txcache = ajBtreeSecCacheNewC(basenam,TX_EXTENSION,idir,"w+",
					     entry->pagesize,
					     entry->order, entry->fill, level,
					     entry->cachesize,
					     entry->secorder, slevel,
					     entry->secfill, count,
					     entry->kwlimit);
	ajBtreeCreateRootNode(entry->txcache,0L);
    }
    

    return ajTrue;
}




/* @func embBtreeCloseCaches ***********************************************
**
** Close index files
**
** @param [u] entry [EmbPBtreeEntry] database data
**
** @return [AjBool] true on success
** @@
******************************************************************************/

AjBool embBtreeCloseCaches(EmbPBtreeEntry entry)
{

    if(entry->do_id)
    {
	ajBtreeCacheSync(entry->idcache,0L);
	ajBtreeCacheDel(&entry->idcache);
    }

    if(entry->do_accession)
    {
	ajBtreeCacheSync(entry->accache,0L);
	ajBtreeCacheDel(&entry->accache);
    }

    if(entry->do_sv)
    {
	ajBtreeCacheSync(entry->svcache,0L);
	ajBtreeCacheDel(&entry->svcache);
    }


    if(entry->do_keyword)
    {
	ajBtreeCacheSync(entry->kwcache,0L);
	ajBtreeCacheDel(&entry->kwcache);
    }
    
    if(entry->do_description)
    {
	ajBtreeCacheSync(entry->decache,0L);
	ajBtreeCacheDel(&entry->decache);
    }
    
    if(entry->do_taxonomy)
    {
	ajBtreeCacheSync(entry->txcache,0L);
	ajBtreeCacheDel(&entry->txcache);
    }
    

    return ajTrue;
}




/* @func embBtreeDumpParameters ***********************************************
**
** Write index parameter files
**
** @param [u] entry [EmbPBtreeEntry] database data
**
** @return [AjBool] true on success
** @@
******************************************************************************/

AjBool embBtreeDumpParameters(EmbPBtreeEntry entry)
{
    char *basenam = NULL;
    char *idir    = NULL;

    basenam = entry->dbname->Ptr;
    idir    = entry->idirectory->Ptr;
    

    if(entry->do_id)
	ajBtreeWriteParams(entry->idcache, basenam, ID_EXTENSION, idir);

    if(entry->do_accession)
	ajBtreeWriteParams(entry->accache, basenam, AC_EXTENSION, idir);

    if(entry->do_sv)
	ajBtreeWriteParams(entry->svcache, basenam, SV_EXTENSION, idir);

    if(entry->do_keyword)
	ajBtreeWriteParams(entry->kwcache, basenam, KW_EXTENSION, idir);
    
    if(entry->do_description)
	ajBtreeWriteParams(entry->decache, basenam, DE_EXTENSION, idir);
    
    if(entry->do_taxonomy)
	ajBtreeWriteParams(entry->txcache, basenam, TX_EXTENSION, idir);    

    return ajTrue;
}
