/* @source eyinoyang application
**
** Wrapper for yinOyang
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


/* @prog eyinoyang ************************************************************
**
** Wrapper for yinOyang
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPSeqset seqset  = NULL;
    AjPStr    cl      = NULL;
    AjPSeqout seqout  = NULL;
    float     thresh  = 0.;
    AjBool    netphos = ajFalse;
    AjPStr    format  = NULL;
    AjBool    plot    = ajFalse;

    const AjPStr ofn = NULL;
    AjPStr    fn     = NULL;
    AjPStr    stmp   = NULL;
    
    AjPFile outf = NULL;
    
    
    embInitP("eyinoyang", argc, argv, "CBSTOOLS");


    seqset  = ajAcdGetSeqset("sequence");
    outf    = ajAcdGetOutfile("outfile");
    plot    = ajAcdGetBoolean("plot");
    netphos = ajAcdGetBoolean("netphos");
    thresh  = ajAcdGetFloat("threshold");
    format  = ajAcdGetListSingle("format");
    
    
    cl   = ajStrNewC("yinOyang ");
    fn   = ajStrNew();
    stmp = ajStrNew();
    


    ajFilenameSetTempname(&fn);
    seqout = ajSeqoutNew();
    if(!ajSeqoutOpenFilename(seqout, fn))
	ajFatal("Cannot open temporary file %S",fn);
    ajSeqoutSetFormatC(seqout, "fasta");
    ajSeqoutWriteSet(seqout,seqset);
    ajSeqoutClose(seqout);


    if(ajStrMatchC(format,"short"))
        ajStrAppendC(&cl," -f s");
    else if(ajStrMatchC(format,"long"))
        ajStrAppendC(&cl," -f l");
    
    if(plot)
        ajStrAppendC(&cl," -g");

    if(netphos)
        ajStrAppendC(&cl," -y");
    
    if(netphos)
    {
        ajFmtPrintS(&stmp," -t %f",thresh);
        ajStrAppendS(&cl,stmp);
    }

    ajFmtPrintS(&stmp," %S",fn);
    ajStrAppendS(&cl,stmp);

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
    ajStrDel(&fn);
    ajStrDel(&stmp);
    ajStrDel(&format);
    ajSeqoutDel(&seqout);
    ajSeqsetDel(&seqset);

    embExit();

    return 0;
}
