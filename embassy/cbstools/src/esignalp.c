/* @source esignalp application
**
** Wrapper for SIGNALP
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


/* @prog esignalp ************************************************************
**
** Wrapper for SIGNALP
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPStr    cl     = NULL;
    AjPSeqset seqset = NULL;
    AjPSeqout seqout = NULL;
    AjPStr    type   = NULL;
    AjBool    plot   = ajFalse;
    const AjPStr ofn = NULL;
    AjPStr    fn     = NULL;
    AjPStr    stmp   = NULL;
    AjPStr    method = NULL;
    AjPStr    format = NULL;
    AjPStr    gmode  = NULL;
    AjPStr    synver = NULL;
    AjBool    keep   = ajFalse;
    ajint     trunc  = 0;

    

    AjPFile outf = NULL;
    
    
    embInitP("esignalp", argc, argv, "CBSTOOLS");


    seqset  = ajAcdGetSeqset("sequence");
    type    = ajAcdGetListSingle("type");
    method  = ajAcdGetListSingle("method");
    format  = ajAcdGetListSingle("format");
    gmode   = ajAcdGetListSingle("gmode");
    synver  = ajAcdGetListSingle("synver");
    keep    = ajAcdGetBoolean("keepall");
    trunc   = ajAcdGetInt("truncate");
    outf    = ajAcdGetOutfile("outfile");    

    cl   = ajStrNewC("signalp -t ");
    fn   = ajStrNew();
    stmp = ajStrNew();
    

    ajFilenameSetTempname(&fn);
    seqout = ajSeqoutNew();
    if(!ajSeqoutOpenFilename(seqout, fn))
	ajFatal("Cannot open temporary file %S",fn);
    ajSeqoutSetFormatC(seqout, "fasta");
    ajSeqoutWriteSet(seqout,seqset);
    ajSeqoutClose(seqout);

    if(plot)
        ajStrAppendC(&cl," -g");

    if(ajStrMatchC(type,"euk"))
        ajStrAppendC(&cl,"euk");
    else if(ajStrMatchC(type,"gramp"))
        ajStrAppendC(&cl,"""gram+""");
    else
        ajStrAppendC(&cl,"""gram-""");

    ajFmtPrintS(&stmp," -m %S",method);
    ajStrAppendS(&cl,stmp);

    ajFmtPrintS(&stmp," -f %S",format);
    ajStrAppendS(&cl,stmp);

    if(!ajStrMatchC(gmode,"none"))
    {
        ajFmtPrintS(&stmp," -graphics %S",gmode);
        ajStrAppendS(&cl,stmp);
    }
    
    ajFmtPrintS(&stmp," -synver %S",synver);
    ajStrAppendS(&cl,stmp);


    ajFmtPrintS(&stmp," %S",fn);
    ajStrAppendS(&cl,stmp);


    if(keep)
        ajStrAppendC(&cl," -keepall");

    if(trunc)
    {
        ajFmtPrintS(&stmp," -trunc %d",trunc);
        ajStrAppendS(&cl,stmp);
    }
    
    ofn = ajFileGetNameS(outf);
    ajFmtPrintS(&stmp," > %S",ofn);
    ajStrAppendS(&cl,stmp);
    ajFileClose(&outf);

#if 0
   ajFmtPrint("%S\n",cl);
#endif

#if 1
   if(system(ajStrGetPtr(cl)) == -1)
       ajFatal("Command %S failed",cl);
#endif

    ajSysFileUnlink(fn);

    ajStrDel(&cl);
    ajStrDel(&type);
    ajStrDel(&method);
    ajStrDel(&format);
    ajStrDel(&gmode);
    ajStrDel(&synver);
    ajStrDel(&stmp);
    ajStrDel(&fn);
    ajSeqoutDel(&seqout);
    ajSeqsetDel(&seqset);

    embExit();

    return 0;
}
