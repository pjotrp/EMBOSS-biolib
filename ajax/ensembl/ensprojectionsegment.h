#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensprojectionsegment_h
#define ensprojectionsegment_h

#include "ensslice.h"




/* @data EnsPProjectionSegment ************************************************
**
** Ensembl Projection Segment.
**
** Holds information about a genome sequence slice.
**
** @alias EnsSProjectionSegment
** @alias EnsOProjectionSegment
**
** @attr SrcStart [ajuint] Source start coordinate.
** @attr SrcEnd [ajuint] Source end coordinate.
** @attr TrgSlice [EnsPSlice] Target Ensembl Slice.
** @attr Use [ajuint] Use counter.
** @attr Padding [ajuint] Padding to alignment boundary.
** @@
******************************************************************************/

typedef struct EnsSProjectionSegment
{
    ajuint SrcStart;
    ajuint SrcEnd;
    EnsPSlice TrgSlice;
    ajuint Use;
    ajuint Padding;
} EnsOProjectionSegment;

#define EnsPProjectionSegment EnsOProjectionSegment*




/*
** Prototype definitions
*/

EnsPProjectionSegment ensProjectionSegmentNew(ajuint srcstart, ajuint srcend,
                                              EnsPSlice trgslice);

EnsPProjectionSegment ensProjectionSegmentNewRef(EnsPProjectionSegment ps);

void ensProjectionSegmentDel(EnsPProjectionSegment* Pps);

ajuint ensProjectionSegmentGetSrcStart(const EnsPProjectionSegment ps);

ajuint ensProjectionSegmentGetSrcEnd(const EnsPProjectionSegment ps);

EnsPSlice ensProjectionSegmentGetTrgSlice(const EnsPProjectionSegment ps);

AjBool ensProjectionSegmentTrace(const EnsPProjectionSegment ps, ajuint level);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
