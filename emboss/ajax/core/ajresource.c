/******************************************************************************
** @source AJAX public data resource functions
**
** @author Copyright (C) 2010 Peter Rice
** @version 1.0
** @modified Jun 10 pmr First version
** @modified July 27 Jon Second version (Added ajResourceNew, ajResourceDel,
** ajResourceWrite.  Changed calls of ajListNew to ajListstrNew.)
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

#include <math.h>
#include "ajax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <limits.h>

static ajuint     resourceErrorCount = 0;


static AjPTable resourceIdTable   = NULL;
static AjPTable resourceAccTable  = NULL;
static AjPTable resourceNameTable = NULL;

const char* resourceStatus[] = {
    "Supported",
    "Referenced",
    "Unavailable",
    "None",
    NULL
};

const char* resourceDataFormat[] = {
    "Unknown",
    "HTML",
    "Text",
    "Text (EMBL)",
    "Text (fasta)",
    "Text (gff)",
    NULL
};
    
const char* resourceTags[] = {
    "ID",
    "IDalt",
    "Acc",
    "Name",
    "Desc",
    "URL",
    "Cat",
    "EDAMres",
    "EDAMdat",
    "EDAMid",
    "EDAMfmt",
    "Xref",
    "Query",
    "Example",
    "Contact",
    "Email",
    "CCxref",
    "CCmisc",
    "CClink",
    "CCrest",
    "CCsoap", 
    "Status",
    NULL
};

static void resourceWarn(const AjPFile resfile, ajuint linecnt,
                         const char* fmt, ...);




/* @func ajResourceParse ******************************************************
**
** Parse the standard list of public data resources
**
** @param [u] resfile [AjPFile] Open resource file
** @param [r] validations [const char*] Validations to be turned on or off
**
******************************************************************************/

void ajResourceParse(AjPFile resfile, const char* validations)
{
    AjPStr line = NULL;
    ajuint linecnt = 0;
    AjPResource res = NULL;
    AjPStr token = NULL;
    AjPStr rest = NULL;
    AjPStr tmpstr = NULL;
    AjPStr idkey = NULL;
    AjPStr acckey = NULL;
    AjPStr namekey = NULL;
    ajuint j;

    (void) validations;

    while (ajReadlineTrim(resfile, &line))
    {
        linecnt++;

        if(!ajStrGetLen(line))
            continue;

        if(ajStrGetCharFirst(line) == '#')
            continue;

        ajStrExtractWord(line, &rest, &token);

        j=0;
        while(resourceTags[j] &&
                  !ajStrMatchC(token, resourceTags[j]))
                j++;
        if(!resourceTags[j])
        {
            resourceWarn(resfile, linecnt,
                         "unknown tag name '%S'",
                         token);
            continue;
        }

        if(ajStrMatchC(token, "ID"))
        {
            AJNEW0(res);
            res->Id = ajStrNewS(rest);
            idkey = ajStrNewS(rest);
            ajTablePut(resourceIdTable, idkey, res);
        }
        else  if(ajStrMatchC(token, "IDalt"))
        {
            if(ajStrMatchC(rest, "None"))
                continue;
            if(!res->Idalt)
                res->Idalt = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Idalt, tmpstr);
            tmpstr = NULL;
            idkey = ajStrNewS(rest);
            ajTablePut(resourceIdTable, idkey, res);
        }
        else if(ajStrMatchC(token, "Acc"))
        {
            if(ajStrMatchC(rest, "None"))
                continue;
            res->Acc = ajStrNewS(rest);
            acckey = ajStrNewS(rest);
            ajTablePut(resourceAccTable, acckey, res);
        }
        else  if(ajStrMatchC(token, "Name"))
        {
            if(ajStrGetLen(res->Name))
                resourceWarn(resfile, linecnt,
                             "duplicate tag Name");
            else
                res->Name = ajStrNewS(rest);
            namekey = ajStrNewS(rest);
            ajTablePut(resourceNameTable, namekey, res);                
        }   
        else  if(ajStrMatchC(token, "Desc"))
        {
            if(ajStrGetLen(res->Desc))
                resourceWarn(resfile, linecnt,
                             "duplicate tag Desc");
            else
                res->Desc = ajStrNewS(rest);
        }   
        else  if(ajStrMatchC(token, "URL"))
        {
            if(ajStrGetLen(res->Url))
                resourceWarn(resfile, linecnt,
                             "duplicate tag URL");
            else
                res->Url = ajStrNewS(rest);
        }   
        else  if(ajStrMatchC(token, "Cat"))
        {
            if(ajStrGetLen(res->Cat))
                resourceWarn(resfile, linecnt,
                             "duplicate tag Cat");
            else
                res->Cat = ajStrNewS(rest);
        }   
        else  if(ajStrMatchC(token, "EDAMres"))
        {
            if(!res->Edamres)
                res->Edamres = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Edamres, tmpstr);
            tmpstr = NULL;
        }
        else  if(ajStrMatchC(token, "EDAMdat"))
        {
            if(!res->Edamdat)
                res->Edamdat = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Edamdat, tmpstr);
            tmpstr = NULL;
        }
        else  if(ajStrMatchC(token, "EDAMid"))
        {
            if(!res->Edamid)
                res->Edamid = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Edamid, tmpstr);
            tmpstr = NULL;
        }
        else  if(ajStrMatchC(token, "EDAMfmt"))
        {
            if(!res->Edamfmt)
                res->Edamfmt = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Edamfmt, tmpstr);
            tmpstr = NULL;
        }   
        else  if(ajStrMatchC(token, "Xref"))
        {
            if(!res->Xref)
                res->Xref = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Xref, tmpstr);
            tmpstr = NULL;
        }   
        else  if(ajStrMatchC(token, "Query"))
        {
            if(!res->Query)
                res->Query = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Query, tmpstr);
            tmpstr = NULL;
        }   
        else  if(ajStrMatchC(token, "Example"))
        {
            if(!res->Example)
                res->Example = ajListstrNew();
            tmpstr = ajStrNewS(rest);
            ajListPushAppend(res->Example, tmpstr);
            tmpstr = NULL;
        }   
        else  if(ajStrMatchC(token, "Contact"))
        {
            if(ajStrGetLen(res->Contact))
                resourceWarn(resfile, linecnt,
                             "duplicate tag Contact");
            else
                res->Contact = ajStrNewS(rest);
        }   
        else  if(ajStrMatchC(token, "Email"))
        {
            if(ajStrGetLen(res->Email))
                resourceWarn(resfile, linecnt,
                             "duplicate tag Email");
            else
                res->Email = ajStrNewS(rest);
        }   
        else  if(ajStrMatchC(token, "CCxref"))
        {
            if(ajStrGetLen(res->CCxref))
                resourceWarn(resfile, linecnt,
                             "duplicate tag CCxref");
            else
                res->CCxref = ajStrNewS(rest);
        }
        else  if(ajStrMatchC(token, "CCmisc"))
        {
            if(ajStrGetLen(res->CCmisc))
                resourceWarn(resfile, linecnt,
                             "duplicate tag CCmisc");
            else
                res->CCmisc = ajStrNewS(rest);
        }
                else  if(ajStrMatchC(token, "CClink"))
        {
            if(ajStrGetLen(res->CClink))
                resourceWarn(resfile, linecnt,
                             "duplicate tag CClink");
            else
                res->CClink = ajStrNewS(rest);
        }
                else  if(ajStrMatchC(token, "CCrest"))
        {
            if(ajStrGetLen(res->CCrest))
                resourceWarn(resfile, linecnt,
                             "duplicate tag CCrest");
            else
                res->CCrest = ajStrNewS(rest);
        }
                else  if(ajStrMatchC(token, "CCsoap"))
        {
            if(ajStrGetLen(res->CCsoap))
                resourceWarn(resfile, linecnt,
                             "duplicate tag CCsoap");
            else
                res->CCsoap = ajStrNewS(rest);
        }   
        else  if(ajStrMatchC(token, "Status"))
        {
            j=0;
            while(resourceStatus[j] &&
                  !ajStrMatchC(rest, resourceStatus[j]))
                j++;
            if(!resourceStatus[j])
                resourceWarn(resfile, linecnt,
                             "unknown Status '%S'", rest);

            if(ajStrGetLen(res->Status))
                resourceWarn(resfile, linecnt,
                             "duplicate tag Desc");
            else
                res->Status = ajStrNewS(rest);
        }   
    }

    return;
}




/* @funcstatic resourceWarn **************************************************
**
** Formatted write as an error message.
**
** @param [r] resfile [const AjPFile] Input resource file
** @param [r] linecnt [ajuint] Current line number
** @param [r] fmt [const char*] Format string
** @param [v] [...] Format arguments.
** @return [void]
** @@
******************************************************************************/

static void resourceWarn(const AjPFile resfile, ajuint linecnt,
                         const char* fmt, ...)
{
    va_list args;
    AjPStr errstr = NULL;

    resourceErrorCount++;

    va_start(args, fmt) ;
    ajFmtVPrintS(&errstr, fmt, args);
    va_end(args) ;

    ajWarn("File %F line %u: %S", resfile, linecnt, errstr);
    ajStrDel(&errstr);

    return;
}




/* @func ajResourceNew ******************************************************
**
** Default constructor for Resource object (AjPResource)
**
** @return [AjPResource]  New Resource object.
**
******************************************************************************/

AjPResource ajResourceNew(void)
{
    AjPResource ret = NULL;

    AJNEW0(ret);

    ret->Id       = ajStrNew();
    ret->Idalt    = ajListstrNew();
    ret->Acc      = ajStrNew();
    ret->Name     = ajStrNew();
    ret->Desc     = ajStrNew();
    ret->Url      = ajStrNew();
    ret->Cat      = ajStrNew();
    ret->Edamres  = ajListstrNew();
    ret->Edamdat  = ajListstrNew();
    ret->Edamid   = ajListstrNew();
    ret->Edamfmt  = ajListstrNew();
    ret->Xref    = ajListstrNew();
    ret->Query  = ajListstrNew();
    ret->Example = ajListstrNew();
    ret->Contact  = ajStrNew();
    ret->Email    = ajStrNew();
    ret->CCxref    = ajStrNew();
    ret->CCmisc    = ajStrNew();
    ret->CClink    = ajStrNew();
    ret->CCrest    = ajStrNew();
    ret->CCsoap    = ajStrNew();
    ret->Status   = ajStrNew();
    
    return ret;
}



/* @func ajResourceDel ********************************************************
**
** Default constructor for resource object (AjPResource)
** 
** @param [r] resource [AjPResource *] Handle on resource object
**
** @return [void]
**
******************************************************************************/

void ajResourceDel(AjPResource *resource)
{
    AjPResource  thys;

    if(!resource)
        return;

    if(!*resource)
        return;

    thys = *resource;

    ajStrDel(&thys->Id);
    ajListstrFreeData(&thys->Idalt);
    ajStrDel(&thys->Acc);
    ajStrDel(&thys->Name);
    ajStrDel(&thys->Desc);
    ajStrDel(&thys->Url);
    ajStrDel(&thys->Cat);
    ajListstrFreeData(&thys->Edamres);
    ajListstrFreeData(&thys->Edamdat);
    ajListstrFreeData(&thys->Edamid);
    ajListstrFreeData(&thys->Edamfmt);
    ajListstrFreeData(&thys->Xref);
    ajListstrFreeData(&thys->Query);
    ajListstrFreeData(&thys->Example);
    ajStrDel(&thys->Contact);
    ajStrDel(&thys->Email);
    ajStrDel(&thys->CCxref);
    ajStrDel(&thys->CCmisc);
    ajStrDel(&thys->CClink);
    ajStrDel(&thys->CCrest);
    ajStrDel(&thys->CCsoap);
    ajStrDel(&thys->Status);

    return;
}

    
/* @funcstatic resourceWrite *************************************************
**
** Write resource object to file in EMBOSS db.dat format.
**
** All elements / fields are written.
**
** @param [r] resource [AjPResource] Resource object
** @param [w] resfile [AjPFile] Output resource file
** @return [void]
** @@
******************************************************************************/

void ajResourceWrite(AjPResource resource, const AjPFile resfile)
{
    AjPStr  tmpstr  = NULL;
    AjIList iter    = NULL;
        
    if(!resfile)
    {
        ajWarn("NULL file passed to resourceWrite in ajresource.c");
        return;
    }

    ajFmtPrintF(resfile, "%-8s%S", "ID", resource->Id);

    iter = ajListIterNew(resource->Idalt);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "IDalt", tmpstr);
    ajListIterDel(&iter);

    ajFmtPrintF(resfile, "%-8s%S", "Acc", resource->Acc);
    ajFmtPrintF(resfile, "%-8s%S", "Name", resource->Name);
    ajFmtPrintF(resfile, "%-8s%S", "Desc", resource->Desc);
    ajFmtPrintF(resfile, "%-8s%S", "URL", resource->Url);
    ajFmtPrintF(resfile, "%-8s%S", "Cat", resource->Cat);

    iter = ajListIterNew(resource->Edamres);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "EDAMres", tmpstr);
    ajListIterDel(&iter);
    
    iter = ajListIterNew(resource->Edamdat);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "EDAMdat", tmpstr);
    ajListIterDel(&iter);

    iter = ajListIterNew(resource->Edamid);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "EDAMid", tmpstr);
    ajListIterDel(&iter);
    
    iter = ajListIterNew(resource->Edamfmt);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "EDAMfmt", tmpstr);
    ajListIterDel(&iter);

    iter = ajListIterNew(resource->Xref);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "Xref", tmpstr);
    ajListIterDel(&iter);
    
    iter = ajListIterNew(resource->Query);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "Query", tmpstr);
    ajListIterDel(&iter);

    iter = ajListIterNew(resource->Example);
    while((tmpstr = ajListstrIterGet(iter)))
        ajFmtPrintF(resfile, "%-8s%S", "Example", tmpstr);
    ajListIterDel(&iter);
    
    ajFmtPrintF(resfile, "%-8s%S", "Contact", resource->Contact);
    ajFmtPrintF(resfile, "%-8s%S", "Email", resource->Email);
    ajFmtPrintF(resfile, "%-8s%S", "CCxref", resource->CCxref);
    ajFmtPrintF(resfile, "%-8s%S", "CCmisc", resource->CCmisc);
    ajFmtPrintF(resfile, "%-8s%S", "CClink", resource->CClink);
    ajFmtPrintF(resfile, "%-8s%S", "CCrest", resource->CCrest);
    ajFmtPrintF(resfile, "%-8s%S", "CCsoap", resource->CCsoap);
    ajFmtPrintF(resfile, "%-8s%S", "Status", resource->Status);

    return;
}




/* @funcstatic resourceWriteSubset ********************************************
**
** Write resource object to file in EMBOSS db.dat format.
**
** Only a subset (the most important) of the elements / fields are written.
**
** @param [r] resource [AjPResource] Resource object
** @param [w] resfile [AjPFile] Output resource file
** @return [void]
** @@
******************************************************************************/
