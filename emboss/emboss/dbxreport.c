/* @source dbxreport application
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




/* @datastatic DbxreportSData *************************************************
**
** Index file names
**
** @alias DbxreportSData
** @alias DbxreportOData
**
** @attr cache [AjPBtcache] Index cache
** @attr num [ajuint] Index to list of standard index names
** @attr nroot [ajuint] Number of root pages
** @attr nnroot [ajuint] Number of numeric root pages
** @attr ninternal [ajuint] Number of internal node pages
** @attr nleaf [ajuint] Number of leaf node pages
** @attr nbucket [ajuint] Number of bucket pages
** @attr noverflow [ajuint] Number of overflow pages
** @attr npribucket [ajuint] Number of primary key bucket pages
** @attr nsecbucket [ajuint] Number of secondary key bucket pages
** @attr nnumbucket [ajuint] Number of numbucket pages
** @attr nunknown [ajuint] Number of unknown type pages
** @attr nkeys [ajuint] Number of keys
** @attr nover [ajuint] Number of node overflow pages
** @attr nbkeys [ajuint] Number of bucket key pages
** @attr nbdups [ajuint]  Number of bucket duplicate pages
** @attr nbxtra [ajuint]   Number of bucket extra pages
** @attr nbover [ajuint] Number of bucket overflow pages
** @attr npkeys [ajuint]  Number of primary keys
** @attr npover [ajuint]  Number of primary key overflow pages
** @attr nskeys [ajuint]  Number of secondary keys
** @attr nsover [ajuint]  Number of secondary key overflow pages
** @attr nnkeys [ajuint]  Number of numeric keys
** @attr nndups [ajuint]  Number of numeric key duplicates
** @attr nnover [ajuint]  Number of numeric key overflow pages
** @attr nlkeys [ajuint] Number of leaf keys
** @attr nlover [ajuint] Number of leaf overflow pages
** @attr freespace [ajlong] Free space total for all pages
** @attr pagecount [ajlong] Page count
** @attr totsize [ajlong] Total index size
** @@
******************************************************************************/

typedef struct DbxreportSData
{
    AjPBtcache cache;
    ajuint num;
    ajuint nroot;
    ajuint nnroot;
    ajuint ninternal;
    ajuint nleaf;
    ajuint nbucket;
    ajuint noverflow;
    ajuint npribucket;
    ajuint nsecbucket;
    ajuint nnumbucket;
    ajuint nunknown;
    ajuint nkeys;
    ajuint nover;
    ajuint nbkeys;
    ajuint nbdups;
    ajuint nbxtra;
    ajuint nbover;
    ajuint npkeys;
    ajuint npover;
    ajuint nskeys;
    ajuint nsover;
    ajuint nnkeys;
    ajuint nndups;
    ajuint nnover;
    ajuint nlkeys;
    ajuint nlover;
    ajlong freespace;
    ajlong pagecount;
    ajlong totsize;
} DbxreportOData;
#define DbxreportPData DbxreportOData*




/* @datastatic DbxreportNames *************************************************
**
** Index file names
**
** @alias DbxreportSNames
** @alias DbxreportONames
**
** @attr name [const char*] Field name
** @attr iname [const char*] Field index name
** @attr isid [AjBool] Field is an identifier (false for keywords)
** @@
******************************************************************************/

typedef struct DbxreportSNames
{
    const char* name;
    const char* iname;
    AjBool isid;
} DbxreportONames;
#define DbxreportPNames DbxreportONames*



static DbxreportONames inxnames[] =
{
/*   name  iname  isid*/
    {"id", "xid", AJTRUE},
    {"ac", "xac", AJTRUE},
    {"sv", "xsv", AJTRUE},
    {"de", "xde", AJFALSE},
    {"kw", "xkw", AJFALSE},
    {"tx", "xtx", AJFALSE},
    {NULL, NULL, 0}
};




/* @prog dbxreport ************************************************************
**
** Statistics for a dbx index
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr   dbname = NULL;
    AjPStr   idir   = NULL;
    AjPStr  *itype  = NULL;
    AjPFile  outf = NULL;

    const char *basenam;
    const char *baseidir;
    AjBool full;
    
    ajint i;
    ajint j;

    ajint order;
    ajint fill;
    ajint pagesize;
    ajint level;
    ajint cachesize;
    ajint sorder;
    ajint sfill;
    ajint kwlimit;
    ajlong count;
    ajlong countall;
    ajint idorder;
    ajint idfill;
    ajint idpagesize;
    ajint idlevel;
    ajint idcachesize;
    ajint idsorder;
    ajint idsfill;
    ajint idkwlimit;
    ajlong idcount;
    ajlong idcountall;
    ajuint ntypes;
    ajuint nindex;
    ajuint num;
    AjBool ok = ajFalse;
    AjPBtcache idcache = NULL;
    AjPBtpage page = NULL;
    DbxreportPData dbxdata = NULL;
    ajlong pagecount;
    ajlong totsize;
    ajlong ipage;
    ajlong pageoffset;
    const char* nodetype;
    ajint nkeys;
    ajint ndups;
    ajint nextra;
    ajint nover;
    ajint freespace;
    ajint totlen;
    ajlong nfound;
    ajlong nfoundall;

    embInit("dbxreport", argc, argv);
    
    dbname = ajAcdGetString("dbname");
    idir   = ajAcdGetDirectoryName("indexdir");
    itype  = ajAcdGetList("idtype");
    outf   = ajAcdGetOutfile("outfile");
    full   = ajAcdGetBoolean("fullreport");
    
    basenam  = ajStrGetPtr(dbname);
    baseidir = ajStrGetPtr(idir);
    
    ntypes = 0;
    while(inxnames[ntypes].name)
	++ntypes;
    AJCNEW0(dbxdata,ntypes);

    i=0;
    nindex=0;
    while(itype[i])
    {
	if(ajStrMatchC(itype[i],"all"))
        {
            for(j=0;j<ntypes;j++)
                dbxdata[j].num = j;
            nindex=ntypes;
            break;
        }
        ok = ajFalse;
        for(j=0;j<ntypes;j++)
        {
            if(ajStrMatchC(itype[i],inxnames[j].name))
            {
                dbxdata[nindex++].num = j;
                ok = ajTrue;
            }
            
        }
        if(!ok)
            ajDie("Unrecognised index type '%S'", itype[i]);
        ++i;
    }
    
    ajBtreeReadParams(basenam,"xid",baseidir,
                      &idorder,&idfill,&idpagesize,&idlevel,
                      &idcachesize,&idsorder,&idsfill,
                      &idcount,&idcountall, &idkwlimit);
    idcache = ajBtreeSecCacheNewC(basenam,"xid",baseidir,"r",
                                  idpagesize, idorder, idfill, idlevel,
                                  idcachesize, idsorder, 0, idsfill,
                                  idcount, idcountall, idkwlimit);
    for(i=0; i<nindex;i++)
    {
        num = dbxdata[i].num;
        if(ajBtreeReadParams(basenam,inxnames[num].iname,baseidir,
                              &order,&fill,&pagesize,&level,
                              &cachesize,&sorder,&sfill,
                             &count,&countall, &kwlimit))
        {
            ajUser("Index '%s'", inxnames[num].name);
            ajUser("  cachesize: %4d (Size of cache)", cachesize);
            ajUser("   pagesize: %4d (Page size)", pagesize);
            ajUser("      order: %4d (Tree order)", order);
            ajUser("     sorder: %4d (Order of secondary tree)", sorder);
            ajUser("      level: %4d (Level of tree)", level);
            ajUser("     slevel: %4d (Level of secondary tree)", 0);
            ajUser("       fill: %4d (Number of entries per bucket)", fill);
            ajUser("      sfill: %4d (Number of entries per secondary bucket)",
                   sfill);
            ajUser("    kwlimit: %4d (Max key size)", kwlimit);
            ajUser("      count: %4Ld (Number of unique entries in the index)",
                   count);
            ajUser("  fullcount: %4Ld (Number of total entries in the index)",
                   countall);
            dbxdata[i].cache = ajBtreeSecCacheNewC(basenam,inxnames[num].iname,
                                                   baseidir,"r",
                                                   pagesize, order, fill,
                                                   level,
                                                   cachesize, sorder, 0,
                                                   sfill,
                                                   count, countall, kwlimit);
        }
        else
        {
            ajFmtPrintF(outf, "No '%s' index found\n",
                        inxnames[num].name);
            continue;
        }
    }

    for(i=0; i<nindex;i++)
    {
        if(!dbxdata[i].cache) continue;
        num = dbxdata[i].num;
        if(inxnames[num].isid)
        {
            pagecount = ajBtreeGetPagecount(dbxdata[i].cache);
            totsize = ajBtreeGetTotsize(dbxdata[i].cache);
            dbxdata[i].pagecount = pagecount;
            dbxdata[i].totsize = totsize;
            ajFmtPrintF(outf, "Primary index '%s' index found with %Ld pages\n",
                        inxnames[num].name, pagecount);
            pageoffset = 0L;
            for(ipage=0L; ipage < pagecount; ipage++)
            {
                page = ajBtreeCacheRead(dbxdata[i].cache, pageoffset);
                nodetype = ajBtreePageGetTypename(page);
                pageoffset += pagesize;
                GBT_TOTLEN(page->buf,&totlen);
                switch((int) *nodetype)
                {
                    case 'r':
                        if(totlen)
                            dbxdata[i].nroot++;
                        else
                            dbxdata[i].nnroot++;
                        ajBtreeStatNode(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nover += nover;
                        break;
                    case 'i':
                        dbxdata[i].ninternal++;
                        ajBtreeStatNode(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nover += nover;
                        break;
                    case 'l':
                        dbxdata[i].nleaf++;
                        ajBtreeStatNode(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nlkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nlover += nover;
                        break;
                    case 'b':
                        dbxdata[i].nbucket++;
                        ajBtreeStatBucket(dbxdata[i].cache, page, full,
                                          &nkeys, &ndups, &nextra,
                                          &nover, &freespace);
                        dbxdata[i].nbkeys += nkeys;
                        dbxdata[i].nbdups += ndups;
                        dbxdata[i].nbxtra += nextra;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nbover += nover;
                        break;
                    case 'o':
                        dbxdata[i].noverflow++;
                        break;
                    case 'p':
                        dbxdata[i].npribucket++;
                        ajBtreeStatPribucket(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].npkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].npover += nover;
                        break;
                    case 's':
                        dbxdata[i].nsecbucket++;
                        ajBtreeStatSecbucket(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nskeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nsover += nover;
                        break;
                    case 'n':
                        dbxdata[i].nnumbucket++;
                        ajBtreeStatNumbucket(dbxdata[i].cache, page, full,
                                             &nkeys, &nover, &freespace);
                        dbxdata[i].nnkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nnover += nover;
                        break;
                    case 'u':
                    default:
                        dbxdata[i].nunknown++;
                        break;
                }
            }
        }
        else
        {
            pagecount = ajBtreeGetPagecount(dbxdata[i].cache);
            totsize = ajBtreeGetTotsize(dbxdata[i].cache);
            dbxdata[i].pagecount = pagecount;
            dbxdata[i].totsize = totsize;
            ajFmtPrintF(outf, "Secondary index '%s' index found with %Ld pages\n",
                        inxnames[num].name, pagecount);
            pageoffset = 0L;
            for(ipage=0L; ipage < pagecount; ipage++)
            {
                page = ajBtreeCacheRead(dbxdata[i].cache, pageoffset);
                nodetype = ajBtreePageGetTypename(page);
                pageoffset += pagesize;
                switch((int) *nodetype)
                {
                    case 'r':
                        if(totlen)
                            dbxdata[i].nroot++;
                        else
                            dbxdata[i].nnroot++;
                        ajBtreeStatNode(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nover += nover;
                        break;
                    case 'i':
                        dbxdata[i].ninternal++;
                        ajBtreeStatNode(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nover += nover;
                        break;
                    case 'l':
                        dbxdata[i].nleaf++;
                        ajBtreeStatNode(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nlkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nlover += nover;
                        break;
                    case 'b':
                        dbxdata[i].nbucket++;
                        ajBtreeStatBucket(dbxdata[i].cache, page, full,
                                          &nkeys, &ndups, &nextra,
                                          &nover, &freespace);
                        dbxdata[i].nbkeys += nkeys;
                        dbxdata[i].nbdups += ndups;
                        dbxdata[i].nbxtra += nextra;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nbover += nover;
                        break;
                    case 'o':
                        dbxdata[i].noverflow++;
                        break;
                    case 'p':
                        dbxdata[i].npribucket++;
                        ajBtreeStatPribucket(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].npkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].npover += nover;
                        break;
                    case 's':
                        dbxdata[i].nsecbucket++;
                        ajBtreeStatSecbucket(dbxdata[i].cache, page, full,
                                        &nkeys, &nover, &freespace);
                        dbxdata[i].nskeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nsover += nover;
                        break;
                    case 'n':
                        dbxdata[i].nnumbucket++;
                        ajBtreeStatNumbucket(dbxdata[i].cache, page, full,
                                             &nkeys, &nover, &freespace);
                        dbxdata[i].nnkeys += nkeys;
                        dbxdata[i].freespace += (ajlong) freespace;
                        dbxdata[i].nnover += nover;
                        break;
                    case 'u':
                    default:
                        dbxdata[i].nunknown++;
                        break;
                }
            }
        }
        if(dbxdata[i].nroot)
            ajFmtPrintF(outf, "      COUNT: %u\n", dbxdata[i].cache->countunique);
        if(dbxdata[i].nroot)
            ajFmtPrintF(outf, "   COUNTALL: %u\n", dbxdata[i].cache->countall);
        if(dbxdata[i].nroot)
            ajFmtPrintF(outf, "       root: %u\n", dbxdata[i].nroot);
        if(dbxdata[i].nnroot)
            ajFmtPrintF(outf, "    numroot: %u\n", dbxdata[i].nnroot);
        if(dbxdata[i].ninternal)
            ajFmtPrintF(outf, "   internal: %u\n", dbxdata[i].ninternal);
        if(dbxdata[i].nleaf)
            ajFmtPrintF(outf, "       leaf: %u\n", dbxdata[i].nleaf);
        if(dbxdata[i].nbucket)
            ajFmtPrintF(outf, "     bucket: %u\n", dbxdata[i].nbucket);
        if(dbxdata[i].noverflow)
            ajFmtPrintF(outf, "   overflow: %u\n", dbxdata[i].noverflow);
        if(dbxdata[i].nnumbucket)
            ajFmtPrintF(outf, "  numbucket: %u\n", dbxdata[i].nnumbucket);
        if(dbxdata[i].npribucket)
            ajFmtPrintF(outf, "  pribucket: %u\n", dbxdata[i].npribucket);
        if(dbxdata[i].nsecbucket)
            ajFmtPrintF(outf, "  secbucket: %u\n", dbxdata[i].nsecbucket);
        if(dbxdata[i].nunknown)
            ajFmtPrintF(outf, "    unknown: %u\n", dbxdata[i].nunknown);
        if(dbxdata[i].nkeys)
            ajFmtPrintF(outf, "...       keys: %u\n", dbxdata[i].nkeys);
        if(dbxdata[i].nlkeys)
            ajFmtPrintF(outf, "...   leafkeys: %u\n", dbxdata[i].nlkeys);
        if(dbxdata[i].nbkeys)
            ajFmtPrintF(outf, "...      bkeys: %u\n", dbxdata[i].nbkeys);
        if(dbxdata[i].nbdups)
            ajFmtPrintF(outf, "...      bdups: %u\n", dbxdata[i].nbdups);
        if(dbxdata[i].nbxtra)
            ajFmtPrintF(outf, "...      bxtra: %u\n", dbxdata[i].nbxtra);
        if(dbxdata[i].npkeys)
            ajFmtPrintF(outf, "...      pkeys: %u\n", dbxdata[i].npkeys);
        if(dbxdata[i].nskeys)
            ajFmtPrintF(outf, "...      skeys: %u\n", dbxdata[i].nskeys);
        if(dbxdata[i].nnkeys)
            ajFmtPrintF(outf, "...      nkeys: %u (%u)\n",
                        dbxdata[i].nnkeys, dbxdata[i].nndups);
        if(dbxdata[i].nover)
            ajFmtPrintF(outf, "...    overflows_used: %u\n", dbxdata[i].nover);
        if(dbxdata[i].nlover)
            ajFmtPrintF(outf, "...leafoverflows_used: %u\n", dbxdata[i].nlover);
        if(dbxdata[i].nbover)
            ajFmtPrintF(outf, "...   boverflows_used: %u\n", dbxdata[i].nbover);
        if(dbxdata[i].npover)
            ajFmtPrintF(outf, "...   poverflows_used: %u\n", dbxdata[i].npover);
        if(dbxdata[i].nsover)
            ajFmtPrintF(outf, "...   soverflows_used: %u\n", dbxdata[i].nsover);
        if(dbxdata[i].nnover)
            ajFmtPrintF(outf, "...   noverflows_used: %u\n", dbxdata[i].nnover);
        if(dbxdata[i].freespace)
            ajFmtPrintF(outf, "...    freespace: %Ld, %.1f\n", dbxdata[i].freespace,
                        100.0 * (float)dbxdata[i].freespace/(float)dbxdata[i].totsize);
        if(inxnames[num].isid)
        {
            nfound = dbxdata[i].nbkeys;
            nfoundall = nfound + dbxdata[i].nbxtra;
        }
        else
        {
            nfound = dbxdata[i].npkeys;
            nfoundall = nfound + dbxdata[i].nskeys;
        }
        ajFmtPrintF(outf, "      FOUND: %Ld\n", nfound);
        ajFmtPrintF(outf, "   FOUNDALL: %Ld\n", nfoundall);
        ajUser("      found: %Ld (Number of unique entries in the index)",
               nfound);
        ajUser("   foundall: %Ld (Number of total entries in the index)",
               nfoundall);
    }

/*
    for(i=2;i<100;i+=3)
    {
        ajint lb, lm, lk;
        ajint rb, rm, rk;
        ajint k=13;
        ajint lrest, rrest;
        AjBool ret;
        for(j=i;j>1;j/=2)
        {
            ret = ajBtreeBucketCalc(i, j, k, &lb, &lm, &lk, &rb, &rm, &rk);
            lrest = lk - (lb*lm);
            rrest = rk - (rb*rm);
            ajUser("%3d %3d %d*%d +%d = %d %d*%d +%d = %d %B",
                   i, j, lb, lm, lrest, lk, rb, rm, rrest, rk, ret);
        }

        
    }
*/    
    ajStrDel(&dbname);
    ajStrDel(&idir);

    ajFileClose(&outf);
    for(i=0;i<nindex;i++)
    {
        ajBtreeCacheDel(&dbxdata[i].cache);
    }
    
    AJFREE(dbxdata);

    embExit();
    
    return 0;
}
