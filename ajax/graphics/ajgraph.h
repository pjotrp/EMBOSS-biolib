#ifdef __cplusplus
extern "C"
{
#endif

/* @source ajgraph.h
**
** General Plot/Printing routines.
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


#ifndef ajgraph_h
#define ajgraph_h

#include "plplot.h"
#include "ajgraphstruct.h"
#include "ajdefine.h"
#include "ajstr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define GRAPH_XGAP      60
#define GRAPH_YGAP      60
#define GRAPH_TITLE_GAP 60

#define AJGRAPH_X_BOTTOM   0x0001  /* print xaxis bottom line */
#define AJGRAPH_Y_LEFT     0x0002  /* print yaxis left line */
#define AJGRAPH_X_TOP      0x0004  /* print xaxis top line */
#define AJGRAPH_Y_RIGHT    0x0008  /* printf y axis on the right*/
#define AJGRAPH_X_TICK     0x0010  /* add tick marks for x axis (bottom) */
#define AJGRAPH_Y_TICK     0x0020  /* add tick marks dor y axis (left) */
#define AJGRAPH_X_LABEL    0x0040  /* add x axis label */
#define AJGRAPH_Y_LABEL    0x0080  /* add y axis label */
#define AJGRAPH_TITLE      0x0100  /* add title */
#define AJGRAPH_JOINPOINTS 0x0200  /* join the point data by a line a line */
#define AJGRAPH_OVERLAP    0x0400  /* write plots on top of each other */
#define AJGRAPH_Y_NUMLABEL_LEFT   0x0800
#define AJGRAPH_Y_INVERT_TICK     0x1000
#define AJGRAPH_Y_GRID            0x2000
#define AJGRAPH_X_NUMLABEL_ABOVE  0x4000
#define AJGRAPH_X_INVERT_TICK     0x8000
#define AJGRAPH_X_GRID            0x10000
#define AJGRAPH_CIRCLEPOINTS      0x20000
#define AJGRAPH_SUBTITLE          0x40000
#define AJGRAPH_GAPS              0x80000

#define GRAPH_XY (AJGRAPH_X_BOTTOM + AJGRAPH_Y_LEFT + AJGRAPH_X_TOP + \
		  AJGRAPH_Y_RIGHT + AJGRAPH_X_TICK + AJGRAPH_Y_TICK + \
		  AJGRAPH_Y_LABEL + AJGRAPH_JOINPOINTS + AJGRAPH_X_LABEL + \
		  AJGRAPH_TITLE + AJGRAPH_SUBTITLE + AJGRAPH_OVERLAP)

#define GRAPH_XY_MAIN (AJGRAPH_X_BOTTOM + AJGRAPH_Y_LEFT + AJGRAPH_Y_RIGHT + \
		       AJGRAPH_X_TOP + AJGRAPH_Y_TICK + AJGRAPH_X_LABEL + \
		       AJGRAPH_Y_LABEL + AJGRAPH_JOINPOINTS + AJGRAPH_TITLE + \
		       AJGRAPH_SUBTITLE + AJGRAPH_OVERLAP)



/*
** Prototype definitions
*/

void          ajGraphListDevices (AjPList list);
ajint*        ajGraphicsBasecolourNewNuc(const AjPStr codes);
ajint*        ajGraphicsBasecolourNewProt(const AjPStr codes);
ajint         ajGraphicsCheckColourC(const char* text);
ajint         ajGraphicsCheckColourS(const AjPStr colour);

void          ajGraphicsGetCharsize(float *defheight, float *currentscale);
ajint         ajGraphicsGetColourFore(void);
ajint         ajGraphicsGetOutfiles(AjPList* files);
void          ajGraphicsGetParamsPage(float *xp,float *yp, ajint *xleng,
                                      ajint *yleng, ajint *xoff, ajint *yoff);

void          ajGraphicsSetBgcolourBlack(void);
void          ajGraphicsSetBgcolourReset(void);
void          ajGraphicsSetBgcolourWhite(void);
float         ajGraphicsSetCharscale(float scale);
float         ajGraphicsSetCharsize(float size);
ajint         ajGraphicsSetColourFore(ajint colour);
void          ajGraphicsSetColourReset(void);
float         ajGraphicsSetDefcharsize(float size);
void          ajGraphicsSetLabelsC(const char *x, const char *y,
                                   const char *title, const char *subtitle);
void          ajGraphicsSetLabelsS(const AjPStr x, const AjPStr y,
                                   const AjPStr title, const AjPStr subtitle);
void          ajGraphicsSetPortrait(AjBool set);

void          ajGraphSetPenWidth(float width);
void          ajGraphPlenv (float xmin, float xmax, float ymin, float ymax,
			    ajint flags);
void          ajGraphPrintType(AjPFile outf, AjBool full);
ajint         ajGraphSetFillPat (ajint style);
ajint         ajGraphSetLineStyle (ajint style);
void          ajGraphSetPage(ajuint width, ajuint height);

void          ajGraphClose (void);
void          ajGraphCloseWin (void);

void          ajGraphDumpDevices (void);

void          ajGraphArrayMaxMin(const float *array, ajint npoints, float *min,
				  float *max);

PLFLT         *ajComputeCoord(PLFLT xcentre, PLFLT ycentre, PLFLT Radius,
			      PLFLT Angle);
PLFLT         ajGraphTextLength(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
				const char *text);
PLFLT         ajGraphTextHeight(PLFLT xx1, PLFLT xx2, PLFLT yy1, PLFLT yy2);
PLFLT         ajGraphDistPts(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2);
PLFLT         ajGraphFitTextAtline(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
                                   const char *text, PLFLT TextHeight);


void          ajGraphicsDrawArc(PLFLT xcentre, PLFLT ycentre, PLFLT Radius,
                                PLFLT StartAngle, PLFLT EndAngle);
void          ajGraphicsDrawBarsHoriz(ajint numofpoints, PLFLT *y,
                                      PLFLT *xmin, PLFLT *xmax);
void          ajGraphicsDrawBarsVert(ajint numofpoints, PLFLT *x,
                                     PLFLT *ymin, PLFLT *ymax);
void          ajGraphicsDrawBox(PLFLT xx0, PLFLT yy0,PLFLT size);
void          ajGraphicsDrawBoxFill(PLFLT xx0, PLFLT yy0, PLFLT size);
void          ajGraphicsDrawCircle(PLFLT xcentre, PLFLT ycentre, float radius);
void          ajGraphicsDrawDia(PLFLT xx0, PLFLT yy0, PLFLT size);
void          ajGraphicsDrawDiaFill(PLFLT xx0, PLFLT yy0, PLFLT size);
void          ajGraphicsDrawDots(PLFLT *xx1,PLFLT *yy1, ajint numofdots);
void          ajGraphicsDrawLine(PLFLT xx1,PLFLT yy1,PLFLT xx2,PLFLT yy2);
void          ajGraphicsDrawLines(PLFLT *xx1,PLFLT *yy1,PLFLT *xx2,PLFLT *yy2,
                                  ajint numoflines);
void          ajGraphicsDrawPoly(ajint n, PLFLT *x, PLFLT *y);
void          ajGraphicsDrawPolyFill(ajint n, PLFLT *x, PLFLT *y);
void          ajGraphicsDrawRect(PLFLT xx0, PLFLT yy0,PLFLT xx1, PLFLT yy1) ;
void          ajGraphicsDrawRectFill(PLFLT xx0, PLFLT yy0,
                                     PLFLT xx1, PLFLT yy1) ;
void          ajGraphicsDrawRectOncurve(PLFLT xcentre, PLFLT ycentre,
                                        PLFLT Radius,  PLFLT BoxHeight,
                                        PLFLT StartAngle, PLFLT EndAngle);
void          ajGraphicsDrawRectOncurveFill(PLFLT xcentre, PLFLT ycentre,
                                            PLFLT Radius, PLFLT BoxHeight,
                                            PLFLT StartAngle, PLFLT EndAngle);
void          ajGraphicsDrawRlabelC(const char *text);
void          ajGraphicsDrawRlabelS(const AjPStr str);
void          ajGraphicsDrawSymbols(ajint numofdots, PLFLT *xx1,PLFLT *yy1,
                                    ajint symbol);
void          ajGraphicsDrawText(PLFLT xx1, PLFLT yy1, const char *text,
                                 PLFLT just);
void          ajGraphicsDrawTextOncurve(PLFLT xcentre, PLFLT ycentre,
                                        PLFLT Radius, PLFLT StartAngle,
                                        PLFLT EndAngle,
                                        const char *Text, PLFLT just);
void          ajGraphicsDrawTextAtend(PLFLT xx1, PLFLT yy1, const char *text);
void          ajGraphicsDrawTextAtmid(PLFLT xx1, PLFLT yy1, const char *text);
void          ajGraphicsDrawTextAtstart(PLFLT xx1, PLFLT yy1,
                                        const char *text);
void          ajGraphicsDrawTextAtline(PLFLT xx1, PLFLT yy1,
                                       PLFLT xx2, PLFLT yy2,
                                       const char *text, PLFLT just);
void          ajGraphicsDrawTri(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
                                PLFLT x3, PLFLT y3);
void          ajGraphicsDrawTriFill(PLFLT xx1, PLFLT yy1, PLFLT xx2, PLFLT yy2,
                                    PLFLT x3, PLFLT y3);

AjPGraph      ajGraphNew (void);
AjPGraph      ajGraphxyNewI (ajint numofpoints);

void          ajGraphClear(AjPGraph thys);

const AjPStr  ajGraphGetSubTitle(const AjPGraph thys);
const char*   ajGraphGetSubTitleC(const AjPGraph thys);
const AjPStr  ajGraphGetTitle(const AjPGraph thys);
const char*   ajGraphGetTitleC(const AjPGraph thys);
const AjPStr  ajGraphGetXTitle(const AjPGraph thys);
const char*   ajGraphGetXTitleC(const AjPGraph thys);
const AjPStr  ajGraphGetYTitle(const AjPGraph thys);
const char*   ajGraphGetYTitleC(const AjPGraph thys);

void          ajGraphInitSeq (AjPGraph thys, const AjPSeq seq);
AjBool        ajGraphIsData(const AjPGraph thys);


void          ajGraphNewPage (AjPGraph thys, AjBool resetdefaults);
void          ajGraphAddLine (AjPGraph thys, float xx1, float yy1,
			      float xx2, float yy2, ajint colour);
void          ajGraphAddRect (AjPGraph thys, float xx1, float yy1,
			      float xx2, float yy2, ajint colour,
			      ajint fill);
void          ajGraphAddText (AjPGraph thys, float xx1, float yy1,
				 ajint colour, const char *text);
void          ajGraphAddTextScale (AjPGraph thys, float xx1, float yy1,
				   ajint colour, float scale,
				   const char *text);
void          ajGraphOpen (AjPGraph thys, PLFLT xmin, PLFLT xmax,
			   PLFLT ymin, PLFLT ymax, ajint flags);
void          ajGraphOpenPlot (AjPGraph thys, ajint numofsets);
void          ajGraphOpenWin  (AjPGraph thys, float xmin, float xmax,
			       float ymin, float ymax);
AjBool        ajGraphSet (AjPGraph thys, const AjPStr type);
void          ajGraphSetDevice(const AjPGraph thys);
void          ajGraphSetMulti (AjPGraph thys, ajint numsets);
void          ajGraphSetName (const AjPGraph thys);
void          ajGraphTrace (const AjPGraph thys);
ajint         ajGraphDataAdd (AjPGraph thys, AjPGraphPlpData graphdata);
ajint         ajGraphDataReplace (AjPGraph thys, AjPGraphPlpData graphdata);
ajint         ajGraphDataReplaceI (AjPGraph thys, AjPGraphPlpData graphdata,
				   ajint num);
void          ajGraphSetTitlePlus(AjPGraph thys, const AjPStr title);
void          ajGraphxyDel (AjPGraph* pmult);
void          ajGraphxyDisplay (AjPGraph thys, AjBool closeit );
void          ajGraphxyCheckMaxMin (AjPGraph thys);
AjBool        ajGraphxySet (AjPGraph thys, const AjPStr type);
void          ajGraphxySetCirclePoints (AjPGraph thys, AjBool set);
void          ajGraphSetFlag (AjPGraph thys, ajint flag, AjBool istrue);
void          ajGraphxySetJoinPoints (AjPGraph thys, AjBool set);
void          ajGraphxySetMaxMin(AjPGraph thys,float xmin,float xmax,
				 float ymin,float ymax);
void          ajGraphSetOut (AjPGraph thys, const AjPStr txt);
void          ajGraphSetOutC (AjPGraph thys, const char *txt);
void          ajGraphxySetOverLap (AjPGraph thys, AjBool overlap);

void          ajGraphSetShowSubtitle (AjPGraph thys, AjBool set);
void          ajGraphSetShowTitle (AjPGraph thys, AjBool set);

void          ajGraphSetDescS(AjPGraph thys, const AjPStr title);

void          ajGraphSetOutputDirS(AjPGraph thys, const AjPStr txt);
void          ajGraphSetTitleC(AjPGraph thys, const char *title);
void          ajGraphSetTitleS(AjPGraph thys, const AjPStr title);
void          ajGraphSetSubtitleC(AjPGraph thys, const char *title);
void          ajGraphSetSubtitleS(AjPGraph thys, const AjPStr title);
void          ajGraphSetXlabelC(AjPGraph thys, const char *title);
void          ajGraphSetXlabelS(AjPGraph thys, const AjPStr title);
void          ajGraphSetYlabelC(AjPGraph thys, const char *title);
void          ajGraphSetYlabelS(AjPGraph thys, const AjPStr title);

void          ajGraphxySetGaps(AjPGraph thys, AjBool overlap);

void          ajGraphxySetShowRaxis(AjPGraph thys, AjBool set);
void          ajGraphxySetShowUaxis(AjPGraph thys, AjBool set);
void          ajGraphxySetShowUnum(AjPGraph thys, AjBool set);
void          ajGraphxySetShowXaxis(AjPGraph thys, AjBool set);
void          ajGraphxySetShowXlabel(AjPGraph thys, AjBool set);
void          ajGraphxySetShowXtick(AjPGraph thys, AjBool set);
void          ajGraphxySetShowYaxis(AjPGraph thys, AjBool set);
void          ajGraphxySetShowYlabel(AjPGraph thys, AjBool set);
void          ajGraphxySetShowYnum(AjPGraph thys, AjBool set);
void          ajGraphxySetShowYtick(AjPGraph thys, AjBool set);

void          ajGraphxySetXendF(AjPGraph thys, float val);
void          ajGraphxySetXgrid(AjPGraph thys, AjBool set);
void          ajGraphxySetXinvert(AjPGraph thys, AjBool set);
void          ajGraphxySetXrangeII(AjPGraph thys, ajint start, ajint end);
void          ajGraphxySetXstartF(AjPGraph thys, float val);
void          ajGraphxySetYendF(AjPGraph thys, float val);
void          ajGraphxySetYgrid(AjPGraph thys, AjBool set);
void          ajGraphxySetYinvert(AjPGraph thys, AjBool set);
void          ajGraphxySetYrangeII(AjPGraph thys, ajint start, ajint end);
void          ajGraphxySetYstartF(AjPGraph thys, float val);

AjPGraphPlpData  ajGraphPlpDataNew (void);
AjPGraphPlpData  ajGraphPlpDataNewI (ajint numsets);

void          ajGraphPlpDataDel(AjPGraphPlpData *thys);
void          ajGraphPlpDataAddLine (AjPGraphPlpData graphs,
				     float xx1, float yy1,
				     float xx2, float yy2, ajint colour);
void          ajGraphPlpDataAddRect (AjPGraphPlpData graphs,
				     float xx1, float yy1,
				     float xx2, float yy2,
				     ajint colour, ajint fill);
void          ajGraphPlpDataAddText (AjPGraphPlpData graphs,
				     float xx1, float yy1,
				     ajint colour, const char *text);
void          ajGraphPlpDataAddTextScale (AjPGraphPlpData graphs,
					  float xx1, float yy1,
					  ajint colour, float scale,
					  const char *text);
void          ajGraphPlpDataSetMaxMin (AjPGraphPlpData graphdata, float xmin,
				      float xmax, float ymin, float ymax);

void          ajGraphPlpDataSetMaxima(AjPGraphPlpData graphdata, float xmin,
				     float xmax, float ymin, float ymax);
void          ajGraphPlpDataSetTypeC(AjPGraphPlpData graphdata,
				     const char *type);

void          ajGraphPlpDataCalcXY (AjPGraphPlpData graphdata,
				    ajint numofpoints,
				    float start, float incr,
				    const float* y);
void          ajGraphPlpDataSetXY (AjPGraphPlpData graphdata,
				   const float *x, const float *y);
void          ajGraphPlpDataSetSubTitle  (AjPGraphPlpData graphdata,
					  const AjPStr title);
void          ajGraphPlpDataSetSubTitleC (AjPGraphPlpData graphdata,
					  const char *title);
void          ajGraphPlpDataSetTitle  (AjPGraphPlpData graphdata,
				       const AjPStr title);
void          ajGraphPlpDataSetTitleC (AjPGraphPlpData graphdata,
				       const char *title);
void          ajGraphPlpDataSetXTitle  (AjPGraphPlpData graphdata,
				       const AjPStr title);
void          ajGraphPlpDataSetXTitleC (AjPGraphPlpData graphdata,
				       const char *title);
void          ajGraphPlpDataSetYTitle  (AjPGraphPlpData graphdata,
				       const AjPStr title);
void          ajGraphPlpDataSetYTitleC (AjPGraphPlpData graphdata,
				       const char *title);
void          ajGraphPlpDataSetColour (AjPGraphPlpData graphdata,
				       ajint colour);
void          ajGraphPlpDataSetLineType (AjPGraphPlpData graphdata,
					 ajint type);
void          ajGraphPlpDataTrace (const AjPGraphPlpData thys);

void          ajGraphUnused(void);

/*
** End of prototype definitions
*/


/*
**  These prototypes do not have any corresponding .c code so cannot
**  go into the block above, otherwise the VC compilation will fail 
*/
void          ajGraphTraceInt (FILE* outf);


__deprecated ajint         ajGraphInfo(AjPList* files);
__deprecated void          ajGraphGetOut (float *xp,float *yp,
                                          ajint *xleng, ajint *yleng,
                                          ajint *xoff, ajint *yoff);
__deprecated ajint         ajGraphGetColour(void);
__deprecated ajint*        ajGraphGetBaseColour (void);
__deprecated ajint*        ajGraphGetBaseColourNuc(const AjPStr codes);
__deprecated ajint*        ajGraphGetBaseColourProt(const AjPStr codes);
__deprecated float         ajGraphSetCharScale (float scale);
__deprecated float         ajGraphSetCharSize(float size);
__deprecated void          ajGraphGetCharSize(float *defheight,
                                              float *currentscale);
__deprecated float         ajGraphSetDefCharSize (float size);
__deprecated void          ajGraphxySetXBottom (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXLabelTop (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXTop (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXEnd (AjPGraph thys, float val);
__deprecated void          ajGraphxySetXGrid (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXInvTicks (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXLabel (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXRangeII (AjPGraph thys,
                                                 ajint start, ajint end);
__deprecated void          ajGraphxySetXStart (AjPGraph thys, float val);
__deprecated void          ajGraphxySetYEnd (AjPGraph thys, float val);
__deprecated void          ajGraphxySetYGrid (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYInvTicks (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYStart (AjPGraph thys, float val);
__deprecated void          ajGraphxySetYLeft (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYLabel (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetXTick (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYLabelLeft (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYRangeII (AjPGraph thys,
                                                 ajint start, ajint end);
__deprecated void          ajGraphxySetYTick (AjPGraph thys, AjBool set);
__deprecated void          ajGraphxySetYRight(AjPGraph thys, AjBool set);
__deprecated void          ajGraphSetDir(AjPGraph thys, const AjPStr txt);
__deprecated void          ajGraphSetDesc(AjPGraph thys, const AjPStr title);
__deprecated void          ajGraphSetSubTitle (AjPGraph thys,
                                               const AjPStr title);
__deprecated void          ajGraphSetSubTitleC (AjPGraph thys,
                                                const char *title);
__deprecated void          ajGraphSetXTitle  (AjPGraph thys,
                                              const AjPStr title);
__deprecated void          ajGraphSetXTitleC (AjPGraph thys,
                                              const char *title);
__deprecated void          ajGraphSetYTitle  (AjPGraph thys,
                                              const AjPStr title);
__deprecated void          ajGraphSetYTitleC (AjPGraph thys,
                                              const char *title);
__deprecated void          ajGraphSetSubTitleDo (AjPGraph thys, AjBool set);
__deprecated void          ajGraphSetTitleDo (AjPGraph thys, AjBool set);
__deprecated void          ajGraphSetTitle (AjPGraph thys, const AjPStr title);
__deprecated void          ajGraphLabel (const char *x, const char *y,
                                         const char *title,
                                         const char *subtitle);
__deprecated void          ajGraphDrawTextOnCurve(PLFLT xcentre, PLFLT ycentre,
                                                  PLFLT Radius,
                                                  PLFLT StartAngle,
                                                  PLFLT EndAngle,
                                                  const char *Text, PLFLT just);
__deprecated void          ajGraphFillRectangleOnCurve(PLFLT xcentre,
                                                       PLFLT ycentre,
                                                       PLFLT Radius,
                                                       PLFLT BoxHeight,
                                                       PLFLT StartAngle,
                                                       PLFLT EndAngle);

__deprecated void          ajGraphRectangleOnCurve(PLFLT xcentre, PLFLT ycentre,
                                                   PLFLT Radius,
                                                   PLFLT BoxHeight,
                                                   PLFLT StartAngle,
                                                   PLFLT EndAngle);
__deprecated void          ajGraphPartCircle(PLFLT xcentre, PLFLT ycentre,
                                             PLFLT Radius,
                                             PLFLT StartAngle, PLFLT EndAngle);
__deprecated void          ajGraphBox (PLFLT xx0, PLFLT yy0,PLFLT size);
__deprecated void          ajGraphBoxFill (PLFLT xx0, PLFLT yy0, PLFLT size);
__deprecated void          ajGraphRectFill (PLFLT xx0, PLFLT yy0,
                                            PLFLT xx1, PLFLT yy1) ;
__deprecated void          ajGraphRect (PLFLT xx0, PLFLT yy0,
                                        PLFLT xx1, PLFLT yy1) ;
__deprecated void          ajGraphLine (PLFLT xx1,PLFLT yy1,
                                        PLFLT xx2,PLFLT yy2);
__deprecated void          ajGraphCircle (PLFLT xcentre, PLFLT ycentre,
                                          float radius);
__deprecated void          ajGraphDia (PLFLT xx0, PLFLT yy0, PLFLT size);
__deprecated void          ajGraphDiaFill (PLFLT xx0, PLFLT yy0, PLFLT size);
__deprecated void          ajGraphPoly (ajint n, PLFLT *x, PLFLT *y);
__deprecated void          ajGraphPolyFill (ajint n, PLFLT *x, PLFLT *y);
__deprecated void          ajGraphLines (PLFLT *xx1,PLFLT *yy1,
                                         PLFLT *xx2,PLFLT *yy2,
                                         ajint numoflines);
__deprecated void          ajGraphTextLine (PLFLT xx1, PLFLT yy1,
                                            PLFLT xx2, PLFLT yy2,
                                            const char *text, PLFLT just);
__deprecated void          ajGraphTri (PLFLT xx1, PLFLT yy1,
                                       PLFLT xx2, PLFLT yy2,
                                       PLFLT x3, PLFLT y3);
__deprecated void          ajGraphTriFill (PLFLT xx1, PLFLT yy1,
                                           PLFLT xx2, PLFLT yy2,
                                           PLFLT x3, PLFLT y3);
__deprecated void          ajGraphDots (PLFLT *xx1,PLFLT *yy1, ajint numofdots);
__deprecated void          ajGraphSymbols( ajint numofdots,
                                           PLFLT *xx1,PLFLT *yy1,
                                           ajint symbol);
__deprecated void          ajGraphText (PLFLT xx1, PLFLT yy1,
                                        const char *text, PLFLT just);
__deprecated void          ajGraphTextEnd (PLFLT xx1, PLFLT yy1,
                                           const char *text);
__deprecated void          ajGraphTextMid (PLFLT xx1, PLFLT yy1,
                                           const char *text);
__deprecated void          ajGraphTextStart (PLFLT xx1, PLFLT yy1,
                                             const char *text);

__deprecated void          ajGraphHoriBars (ajint numofpoints, PLFLT *y,
                                            PLFLT *xmin, PLFLT *xmax);
__deprecated void          ajGraphVertBars (ajint numofpoints, PLFLT *x,
                                            PLFLT *ymin, PLFLT *ymax);

__deprecated void          ajGraphSetBackBlack (void);
__deprecated void          ajGraphSetBackWhite (void);
__deprecated void          ajGraphColourBack (void);
__deprecated void          ajGraphColourFore(void);
__deprecated ajint         ajGraphSetFore (ajint colour);

__deprecated void          ajGraphInit (const char *pgm,
                                        ajint argc, char * const argv[]);
__deprecated void          ajGraphInitPV(const char *pgm,
                                         ajint argc, char * const argv[],
                                         const char *package,
                                         const char *packversion);
__deprecated void          ajGraphLabelYRight (const char *text);

__deprecated void          ajGraphSetOri(ajint ori);

__deprecated ajint         ajGraphCheckColour (const AjPStr colour);

/* End of prototypes without C code */



#endif /* ajgraph_h */



#ifdef __cplusplus
}
#endif
