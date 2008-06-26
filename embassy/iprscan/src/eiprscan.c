/* @source eiprscan application
**
** Wrapper for IPRSCAN
**
** @author Copyright (C) Alan Bleasby
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


/* @prog eiprscan ************************************************************
**
** Wrapper for IPRSCAN
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr    cl     = NULL;
    AjPSeqset seqset = NULL;
    AjPSeqout seqout = NULL;
    
    AjPStr    stmp   = NULL;
    AjPStr    email  = NULL;
    AjPStr    fmt    = NULL;
    AjPStr    trtab  = NULL;
    const AjPStr ofn = NULL;
    
    AjPStr    *applist = NULL;

    
    AjBool crc = ajFalse;
    AjBool alt = ajFalse;

    AjBool iprlook = ajFalse;
    AjBool goterms = ajFalse;
    
    ajint trlen = 0;
    
    
    ajuint i    = 0;
    ajuint lcnt = 0;
    
    
    AjPStr fn = NULL;
    AjPFile outf = NULL;
    
    
    embInitP("eiprscan", argc, argv, "IPRSCAN");


    seqset  = ajAcdGetSeqset("sequence");
    email   = ajAcdGetString("email");
    crc     = ajAcdGetBoolean("crc");
    applist = ajAcdGetList("appl");
    fmt     = ajAcdGetListSingle("format");
    trtab   = ajAcdGetListSingle("trtable");
    trlen   = ajAcdGetInt("trlen");
    alt     = ajAcdGetBoolean("altjobs");
    iprlook = ajAcdGetBoolean("iprlookup");
    goterms = ajAcdGetBoolean("goterms");
    outf    = ajAcdGetOutfile("outfile");
    
    stmp = ajStrNew();
    cl   = ajStrNewC("iprscan -cli");
    fn   = ajStrNew();
    


    ajFilenameSetTempname(&fn);
    seqout = ajSeqoutNew();
    if(!ajSeqoutOpenFilename(seqout, fn))
	ajFatal("Cannot open temporary file %S",fn);
    ajSeqoutSetFormatC(seqout, "fasta");
    ajSeqoutWriteSet(seqout,seqset);
    ajSeqoutClose(seqout);

    ajFmtPrintS(&stmp," -i %S",fn);
    ajStrAppendS(&cl,stmp);

    if(!ajSeqsetIsProt(seqset))
        ajStrAppendC(&cl," -seqtype n");

    if(!crc)
        ajStrAppendC(&cl," -nocrc");

    if(ajStrGetLen(email))
    {
        ajFmtPrintS(&stmp," -email %S",email);
        ajStrAppendS(&cl,stmp);
    }
    

    i = 0;
    lcnt = 0;
    
    while(applist[i])
    {
        ++lcnt;
        ++i;
    }
    

    i = 0;
    while(applist[i])
    {
        if(ajStrMatchC(applist[i],"all"))
        {
            if(lcnt != 1)
                ajFatal("Cannot specify 'all' with multiple "
                        "applications");
            ++i;
            continue;
        }
        
        ajFmtPrintS(&stmp," -appl %S",applist[i]);
        ajStrAppendS(&cl,stmp);
        ++i;
    }
    
    ajFmtPrintS(&stmp," -format %S",fmt);
    ajStrAppendS(&cl,stmp);



    ajFmtPrintS(&stmp," -trtable %S",trtab);
    ajStrAppendS(&cl,stmp);

    ajFmtPrintS(&stmp," -trlen %d",trlen);
    ajStrAppendS(&cl,stmp);

    if(alt)
        ajStrAppendC(&cl," -altjobs");

    if(iprlook)
        ajStrAppendC(&cl," -iprlookup");

    if(goterms)
        ajStrAppendC(&cl," -goterms");


    ofn = ajFileGetNameS(outf);
    ajFmtPrintS(&stmp," -o %S",ofn);
    ajFileClose(&outf);
    ajStrAppendS(&cl,stmp);

    
#if 0
   ajFmtPrint("%S\n",cl);
#endif

#if 1
    system(ajStrGetPtr(cl));
#endif


    ajSysFileUnlink(fn);
    
    
    ajStrDel(&cl);
    ajStrDel(&fn);
    ajStrDel(&stmp);
    ajStrDel(&email);
    ajStrDel(&fmt);
    ajStrDel(&trtab);
    ajStrDelarray(&applist);
    ajSeqoutDel(&seqout);
    ajSeqsetDel(&seqset);
    
    embExit();

    return 0;
}
