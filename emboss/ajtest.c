#include "emboss.h"




/* @prog ajtest ***************************************************************
**
** testing, and subject to frequent change
**
******************************************************************************/

static void kim (AjPStr seqout_name, AjPSeq subseq);

int main(int argc, char **argv)
{
    AjPSeqset seqset;
    AjPSeqall seqall;
    AjPSeq seq;
    ajint i = 0;
    AjPStr kimout = NULL;
    AjPStr dir = NULL;

    embInit("ajtest", argc, argv);

    seqall = ajAcdGetSeqall ("sequence");
    seqset = ajAcdGetSeqset ("bsequence");
    dir = ajAcdGetDirectory("directory");

    ajUser("Directory '%S'", dir);
    ajUser("Set of %d", ajSeqsetSize(seqset));
    while(ajSeqallNext (seqall, &seq))
    {
	ajUser ("%3d <%S>", i++, ajSeqGetUsa(seq));
	ajFmtPrintS(&kimout, "kim%d.out", i);
	kim (kimout, seq);
    }

    ajExit();

    return 0;
}

static void kim (AjPStr seqout_name, AjPSeq subseq)
{
    AjPFile seqout_file = ajFileNewOut(seqout_name);
    AjPSeqout named_seqout = ajSeqoutNewF(seqout_file);

    AjPStr format_str = ajStrNew();
    ajStrAssC(&format_str, "embl");

    ajSeqOutSetFormat(named_seqout, format_str);

    ajSeqWrite(named_seqout, subseq);

}
