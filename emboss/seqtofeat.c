/* @source seqtofeat application
**
** Sequence to features?
**
** @author: Unknown
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

int main(int argc, char **argv)
{

    AjPSeq seq;
    AjPFeattabOut outft;
    AjPFeattable ftab = NULL;
    AjPStr pattern;
    AjPRegexp exp = NULL;
    AjPStr cpyseq = NULL;
    AjPStr match = NULL;
    AjPStr src = NULL;
    AjPStr ftname = NULL;

    ajint ioffset = 1;
    ajint icnt = 0;
    ajint istart, iend;

    embInit ("seqtofeat", argc, argv);

    seq = ajAcdGetSeq ("sequence");
    pattern = ajAcdGetString ("pattern");
    outft = ajAcdGetFeatout ("outfeat");

    exp = ajRegComp(pattern);

    cpyseq = ajSeqStrCopy (seq);
    ajUser ("using pattern '%S' cpyseq len %d", pattern, ajStrLen(cpyseq));

    ftab = ajFeattableNewSeq (seq);

    src = ajStrNewC ("seqtofeat");
    ftname = ajStrNewC ("pattern");

    while (ajStrLen(cpyseq) && ajRegExec (exp, cpyseq))
    {
	istart = ioffset + ajRegOffset(exp);
	iend = istart + ajRegLenI(exp, 0) - 1;
	ajRegSubI (exp, 0, &match);
	ajUser ("offset %d match %d to %d '%S'",
		ioffset, istart, iend, match);
	ajRegPost (exp, &cpyseq);
	ajFeatureNew (ftab, src, ftname, istart, iend, NULL, '+', 0,
		      NULL, istart, iend);
	ioffset = iend + 1;
	icnt++;
    }

    ajFeaturesWrite(outft, ftab);

    ajExit ();
    return 0;
}
