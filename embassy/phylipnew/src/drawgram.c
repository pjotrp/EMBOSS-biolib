
#ifdef OSX_CARBON
#include <Carbon/Carbon.h>
#endif

#include "phylip.h"
#include "draw.h"

/* Version 3.6.  Copyright (c) 1986-2002 by Joseph Felsenstein and
  Christopher A. Meacham.  Additional code written by Hisashi Horino,
  Sean Lamont, Andrew Keefe, Daniel Fineman, and Akiko Fuseki.
  Permission is granted to copy, distribute, and modify this
  program provided that (1) this copyright message is not removed
  and (2) no fee is charged for this program. */


#define gap 0.5    /* distance in character heights between the end
                      of a branch and the start of the name */
FILE *plotfile;
AjPFile embossplotfile;
const char *pltfilename;
char trefilename[FNMLNGTH];
char *progname;

AjPPhyloTree* phylotrees = NULL;

long nextnode,  strpwide, strpdeep, strpdiv,
        strptop, strpbottom, payge, numlines, hpresolution, iteration;
boolean preview, previewing, dotmatrix,
         haslengths, uselengths, empty, rescaled, firstscreens,
         pictbold, pictitalic, pictshadow, pictoutline, multiplot, finished;
double xmargin, ymargin, topoflabels, bottomoflabels, rightoflabels,
       leftoflabels, tipspacing,maxheight, scale, xscale, yscale,
       xoffset, yoffset, nodespace, stemlength, treedepth, xnow, ynow,
       xunitspercm, yunitspercm,
       xsize, ysize, xcorner, ycorner, labelheight,labelrotation,expand, rootx,
       rooty, bscale, xx0, yy0, fontheight, maxx, minx, maxy, miny;
double pagex, pagey, paperx, papery, hpmargin, vpmargin;

double *textlength, *firstlet;
striptype stripe;
plottertype plotter, oldplotter, previewer;
growth grows;
treestyle style;
node *root;
pointarray nodep;
pointarray treenode;
fonttype font;
long filesize;
Char ch, resopts;
double trweight;   /* starting here, needed to make sccs version happy */
boolean goteof;
node *grbg;
long *zeros;     /* ... down to here */

       enum {yes, no} penchange, oldpenchange;
static enum {weighted, intermediate, centered, inner, vshaped} nodeposition;
winactiontype winaction;

#ifdef X
String res[]= {
        "*.input: True",
        "*.menubar.orientation: horizontal",
        "*.menubar.borderWidth: 0",
        "*.drawing_area.background: #CCFFFF",
        "*.drawing_area.foreground: #000000",
        "*.menubar.right: ChainLeft",
        "*.menubar.bottom: ChainTop",
        "*.menubar.top: ChainTop",
        "*.menubar.left: ChainLeft",
        "*.drawing_area.fromVert: menubar",
        "*.drawing_area.top: ChainTop",
        "*.drawing_area.bottom: ChainBottom",
        "*.drawing_area.left: ChainLeft",
        "*.drawing_area.right: ChainRight",
        "*.dialog.label: Drawgram -- Rooted tree plotting program\\n\\n\\nPHYLIP version 3.6. (c) Copyright 1993-2002 by The University of Washington.\\nWritten by Joseph Felsenstein, Andrew Keeffe, Akiko Fuseki, Sean Lamont\\nand Dan Fineman\\nPermission is granted to copy and use this program provided no fee is\\ncharged for it and provided that this copyright notice is not removed.",
        NULL
};
#endif


#ifndef OLDC
/* function prototypes */
void emboss_getoptions(char *pgm, int argc, char *argv[]);

void   initdrawgramnode(node **, node **, node *, long, long, long *,
                long *, initops, pointarray, pointarray, Char *, Char *, char**);
void   initialparms(void);
char   showparms(void);
void   getparms(char);
void   calctraverse(node *, double, double *);
void   calculate(void);
void   rescale(void);
void   setup_environment(Char *argv[], boolean *);
void   user_loop(boolean *);
/* function prototypes */
#endif

void initdrawgramnode(node **p, node **grbg, node *q, long len,
                long nodei, long *ntips, long *parens, initops whichinit,
                pointarray treenode, pointarray nodep, Char *str, Char *ch,
                char** treestr)
{
  /* initializes a node */
  long i;
  boolean minusread;
  double valyew, divisor;

  switch (whichinit) {

  case bottom:
    gnu(grbg, p);
    (*p)->index = nodei;
    (*p)->tip = false;
    for (i=0;i<MAXNCH;i++)
      (*p)->nayme[i] = '\0';
    nodep[(*p)->index - 1] = (*p);
    break;

  case nonbottom:
    gnu(grbg, p);
    (*p)->index = nodei;
    break;

  case tip:
    (*ntips)++;
    gnu(grbg, p);
    nodep[(*ntips) - 1] = *p;
    setupnode(*p, *ntips);
    (*p)->tip = true;
    (*p)->naymlength = len ;
    strncpy ((*p)->nayme, str, MAXNCH);
    break;

  case length:
    processlength(&valyew, &divisor, ch, &minusread, treestr, parens);
    if (!minusread)
      (*p)->oldlen = valyew / divisor;
    else
      (*p)->oldlen = 0.0;
    break;

  case hsnolength:
    haslengths = false;
    break;

  default:        /* cases hslength,treewt,unittrwt,iter        */
    break;        /* should never occur                        */
  }
} /* initdrawgramnode */


void initialparms()
{
  /* initialize parameters */

  /* these are set by emboss_getoptions */
/*
//     plotter = DEFPLOTTER;
//     previewer = DEFPREV;
*/

  paperx=20.6375;
  pagex=20.6375;
  papery=26.9875;
  pagey=26.9875;
  strcpy(fontname,"Times-Roman");
  plotrparms(spp);   /* initial, possibly bogus, parameters */
  style = phenogram;
  grows = horizontal;
  labelrotation = 90.0;
  nodespace = 3.0;
  stemlength = 0.05;
  treedepth = 0.5 / 0.95;
  rescaled = true;
  bscale = 1.0;
  uselengths = haslengths;
  if (uselengths)
    nodeposition = weighted;
  else
    nodeposition = centered;
  xmargin = 0.08 * xsize;
  ymargin = 0.08 * ysize;
  preview = true;
  hpmargin = 0.02*pagex;
  vpmargin = 0.02*pagey;
}  /* initialparms */





void emboss_getoptions(char *pgm, int argc, char *argv[])
{
  /* get from user the relevant parameters for the plotter and diagram */

  boolean getgrows;
  int m, n;
  AjPStr getstyle = NULL;
  AjPStr plottercode = NULL;
  AjPStr getpreviewer = NULL;
  AjPStr getnodeposition = NULL;
  
  embInitP (pgm, argc, argv, "PHYLIPNEW");

  n = (int)((pagex-hpmargin-0.01)/(paperx-hpmargin)+1.0);
  m = (int)((pagey-vpmargin-0.01)/(papery-vpmargin)+1.0);

    phylotrees = ajAcdGetTree("intreefile");
 
    plottercode = ajAcdGetListI("plotter", 1);
    
    getplotter(ajStrGetCharFirst(plottercode));

    getpreviewer = ajAcdGetListI("previewer", 1);

    if(ajStrMatchC(getpreviewer, "n")) {
      preview = false;
      previewer = other;   /* Added by Dan F. */
    }
    else if(ajStrMatchC(getpreviewer, "i")) previewer = ibm;
    else if(ajStrMatchC(getpreviewer, "m")) previewer = mac;
    else if(ajStrMatchC(getpreviewer, "x")) previewer = xpreview;
    else if(ajStrMatchC(getpreviewer, "w")) previewer = winpreview;
    else if(ajStrMatchC(getpreviewer, "i")) previewer = tek; 
    else if(ajStrMatchC(getpreviewer, "i")) previewer = decregis;
    else if(ajStrMatchC(getpreviewer, "o")) previewer = other;

    getgrows = ajAcdGetBool("grows");
    if(getgrows) grows = horizontal;
    else grows = vertical;


    getstyle = ajAcdGetListI("style", 1);
    if(ajStrMatchC(getstyle, "c")) style = cladogram;
    else if(ajStrMatchC(getstyle, "p")) style = phenogram;
    else if(ajStrMatchC(getstyle, "e")) style = eurogram;
    else if(ajStrMatchC(getstyle, "s")) style = swoopogram;
    else if(ajStrMatchC(getstyle, "v")) style = curvogram;
    else if(ajStrMatchC(getstyle, "o")) {
      style = circular;
      treedepth = 1.0;
    }



    uselengths = ajAcdGetBool("lengths");

    labelrotation = ajAcdGetFloat("labelrotation");

    if(plotter==ray) {
      xmargin = ajAcdGetFloat("xmarginray");
      ymargin = ajAcdGetFloat("ymarginray");
    }
    else {
      xmargin = ajAcdGetFloat("xmargin");
      ymargin = ajAcdGetFloat("ymargin");
    }


    rescaled = ajAcdGetToggle("rescaled");
    if(rescaled) bscale = ajAcdGetFloat("bscale");

    treedepth = ajAcdGetFloat("treedepth");
    stemlength = ajAcdGetFloat("stemlength");
    nodespace = ajAcdGetFloat("nodespace");
    nodespace = 1.0 / nodespace;



    m = ajAcdGetFloat("pagesheight");
    n = ajAcdGetFloat("pageswidth"); 

    paperx = ajAcdGetFloat("paperx");
    papery = ajAcdGetFloat("papery");

    hpmargin = ajAcdGetFloat("hpmargin");
    vpmargin = ajAcdGetFloat("vpmargin");

    pagex = ((double)n * (paperx-hpmargin)+hpmargin);
    pagey = ((double)m * (papery-vpmargin)+vpmargin);
 

    getnodeposition = ajAcdGetListI("nodeposition", 1);

    if(ajStrMatchC(getnodeposition, "i"))  nodeposition = intermediate;
    else if(ajStrMatchC(getnodeposition, "w")) nodeposition = weighted;
    else if(ajStrMatchC(getnodeposition, "c")) nodeposition = centered;
    else if(ajStrMatchC(getnodeposition, "i")) nodeposition = inner;
    else if(ajStrMatchC(getnodeposition, "v")) nodeposition = vshaped;

  embossplotfile = ajAcdGetOutfile("plotfile");
  emboss_openfile(embossplotfile, &plotfile, &pltfilename);

}  /* getparms */


void calctraverse(node *p, double lengthsum, double *tipx)
{
  /* traverse to establish initial node coordinates */
  double x1, y1, x2, y2, x3, x4, x5, w1, w2, sumwx, sumw, nodeheight;
  node *pp, *plast, *panc;

  if (p == root)
    nodeheight = 0.0;
  else if (uselengths)
      nodeheight = lengthsum + fabs(p->oldlen);
    else
      nodeheight = 1.0;
  if (nodeheight > maxheight)
    maxheight = nodeheight;
  if (p->tip) {
    p->xcoord = *tipx;
    p->tipsabove = 1;
    if (uselengths)
      p->ycoord = nodeheight;
    else
      p->ycoord = 1.0;
    *tipx += tipspacing;
    return;
  }
  sumwx = 0.0;
  sumw = 0.0;
  p->tipsabove = 0;
  pp = p->next;
  x3 = 0.0;
  do {
    calctraverse(pp->back, nodeheight, tipx);
    p->tipsabove += pp->back->tipsabove;
    sumw += pp->back->tipsabove;
    sumwx += pp->back->tipsabove * pp->back->xcoord;
    if (fabs(pp->back->xcoord - 0.5) < fabs(x3 - 0.5))
      x3 = pp->back->xcoord;
    plast = pp;
    pp = pp->next;
  } while (pp != p);
  x1 = p->next->back->xcoord;
  x2 = plast->back->xcoord;
  y1 = p->next->back->ycoord;
  y2 = plast->back->ycoord;

  switch (nodeposition) {

  case weighted:
    w1 = y1 - p->ycoord;
    w2 = y2 - p->ycoord;
    if (w1 + w2 <= 0.0)
      p->xcoord = (x1 + x2) / 2.0;
    else
      p->xcoord = (w2 * x1 + w1 * x2) / (w1 + w2);
    break;

  case intermediate:
    p->xcoord = (x1 + x2) / 2.0;
    break;

  case centered:
    p->xcoord = sumwx / sumw;
    break;

  case inner:
    p->xcoord = x3;
    break;

  case vshaped:
    if (iteration > 1) {
      if (!(p == root)) {
        panc = nodep[p->back->index-1];
        w1 = p->ycoord - panc->ycoord;
        w2 = y1 - p->ycoord;
        if (w1+w2 < 0.000001)
          x4 = (x1+panc->xcoord)/2.0;
        else
          x4 = (w1*x1+w2*panc->xcoord)/(w1+w2);
        w2 = y2 - p->ycoord;
        if (w1+w2 < 0.000001)
          x5 = (x2+panc->xcoord)/2.0;
        else
          x5 = (w1*x2+w2*panc->xcoord)/(w1+w2);
        if (panc->xcoord < p->xcoord)
          p->xcoord = x5;
        else
          p->xcoord = x4;
      }
      else {
        if ((y1-2*p->ycoord+y2) < 0.000001)
          p->xcoord = (x1+x2)/2;
        else
          p->xcoord = ((y2-p->ycoord)*x1+(y1-p->ycoord))/(y1-2*p->ycoord+y2);
        }
    }
    break;
  }
  if (uselengths) {
    p->ycoord = nodeheight;
    return;
  }
  if (nodeposition != inner) {
    p->ycoord = (y1 + y2 - sqrt((y1 + y2) * (y1 + y2) - 4 * (y1 * y2 -
                           (x2 - p->xcoord) * (p->xcoord - x1)))) / 2.0;
   /* this formula comes from the requirement that the vector from
   (x,y) to (x1,y1) be at right angles to that from (x,y) to (x2,y2) */
    return;
  }
  if (fabs(x1 - 0.5) > fabs(x2 - 0.5)) {
    p->ycoord = y1 + x1 - x2;
    w1 = y2 - p->ycoord;
  } else {
    p->ycoord = y2 + x1 - x2;
    w1 = y1 - p->ycoord;
  }
  if (w1 < epsilon)
    p->ycoord -= fabs(x1 - x2);
}  /* calctraverse */


void calculate()
{
  /* compute coordinates for tree */
  double tipx;
  double sum, temp, maxtextlength, maxfirst=0, leftfirst, angle;
  double lef = 0.0, rig = 0.0, top = 0.0, bot = 0.0;
  double *firstlet, *textlength;
  long i;

  firstlet = (double *)Malloc(nextnode*sizeof(double));
  textlength = (double *)Malloc(nextnode*sizeof(double));
  for (i = 0; i < nextnode; i++) {
    nodep[i]->xcoord = 0.0;
    nodep[i]->ycoord = 0.0;
    if (nodep[i]->naymlength > 0)
      firstlet[i] = lengthtext(nodep[i]->nayme, 1L,fontname,font);
    else
      firstlet[i] = 0.0;
  }
  i = 0;
  do
    i++;
  while (!nodep[i]->tip);
  leftfirst = firstlet[i];
  maxheight = 0.0;
  maxtextlength = 0.0;
  for (i = 0; i < nextnode; i++) {
   if (nodep[i]->tip) {
      textlength[i] = lengthtext(nodep[i]->nayme, nodep[i]->naymlength,
                                fontname, font);
      if (textlength[i]-0.5*firstlet[i] > maxtextlength) {
        maxtextlength = textlength[i]-0.5*firstlet[i];
        maxfirst = firstlet[i];
      }
    }
  }
  maxtextlength = maxtextlength + 0.5*maxfirst;
  fontheight = heighttext(font,fontname);
  if (style == circular) {
    if (grows == vertical)
      angle = pi / 2.0;
    else
      angle = 2.0*pi;
  } else
     angle = pi * labelrotation / 180.0;
  maxtextlength /= fontheight;
  maxfirst /= fontheight;
  leftfirst /= fontheight;
  for (i = 0; i < nextnode; i++) {
    if (nodep[i]->tip) {
      textlength[i] /= fontheight;
      firstlet[i] /= fontheight;
    }
  }
  if (spp > 1)
    labelheight = 1.0 / (nodespace * (spp - 1));
  else
    labelheight = 1.0 / nodespace;
  if (angle < pi / 6.0)
    tipspacing = (nodespace
        + cos(angle) * (maxtextlength - 0.5*maxfirst)) * labelheight;
  else if (spp > 1) {
      if (style == circular) {
        tipspacing = 1.0 / spp;
      } else
        tipspacing = 1.0 / (spp - 1.0);
    } else
      tipspacing = 1.0;
  finished = false;
  iteration = 1;
  do {
    if (style == circular)
      tipx = 1.0/(2.0*(double)spp);
    else
      tipx = 0.0;
    sum = 0.0;
    calctraverse(root, sum, &tipx);
    iteration++;
  }
  while ((nodeposition == vshaped) && (iteration < 4*spp));
  rooty = root->ycoord;
  labelheight *= 1.0 - stemlength;
  for (i = 0; i < nextnode; i++) {
    if (rescaled) {
      if (style != circular)
        nodep[i]->xcoord *= 1.0 - stemlength;
      nodep[i]->ycoord = stemlength * treedepth + (1.0 - stemlength) *
            treedepth * (nodep[i]->ycoord - rooty) / (maxheight - rooty);
      nodep[i]->oldtheta = angle;
    } else {
      nodep[i]->xcoord = nodep[i]->xcoord * (maxheight - rooty) / treedepth;
      nodep[i]->ycoord = stemlength / (1 - stemlength) * (maxheight - rooty) +
                         nodep[i]->ycoord;
      nodep[i]->oldtheta = angle;
    }
  }
  topoflabels = 0.0;
  bottomoflabels = 0.0;
  leftoflabels = 0.0;
  rightoflabels = 0.0;
  if  (style == circular) {
    for (i = 0; i < nextnode; i++) {
      temp = nodep[i]->xcoord;
      if (grows == vertical) {
        nodep[i]->xcoord = (1.0+nodep[i]->ycoord
                                * cos((1.5-2.0*temp)*pi)/treedepth)/2.0;
        nodep[i]->ycoord = (1.0+nodep[i]->ycoord
                                * sin((1.5-2.0*temp)*pi)/treedepth)/2.0;
        nodep[i]->oldtheta = (1.5-2.0*temp)*pi;
      } else {
        nodep[i]->xcoord = (1.0+nodep[i]->ycoord
                                * cos((1.0-2.0*temp)*pi)/treedepth)/2.0;
        nodep[i]->ycoord = (1.0+nodep[i]->ycoord
                                * sin((1.0-2.0*temp)*pi)/treedepth)/2.0;
        nodep[i]->oldtheta = (1.0-2.0*temp)*pi;
      }
    }
    tipspacing *= 2.0*pi;
  }
  maxx = nodep[0]->xcoord;
  maxy = nodep[0]->ycoord;
  minx = nodep[0]->xcoord;
  if (style == circular)
    miny = nodep[0]->ycoord;
  else
    miny = 0.0;
  for (i = 1; i < nextnode; i++) {
    if (nodep[i]->xcoord > maxx)
      maxx = nodep[i]->xcoord;
    if (nodep[i]->ycoord > maxy)
      maxy = nodep[i]->ycoord;
    if (nodep[i]->xcoord < minx)
      minx = nodep[i]->xcoord;
    if (nodep[i]->ycoord < miny)
      miny = nodep[i]->ycoord;
  }
  if  (style == circular) {
    for (i = 0; i < nextnode; i++) {
      if (nodep[i]->tip) {
        angle = nodep[i]->oldtheta;
        if (cos(angle) < 0.0)
          angle -= pi;
        top = (nodep[i]->ycoord - maxy) / labelheight + sin(nodep[i]->oldtheta);
        rig = (nodep[i]->xcoord - maxx) / labelheight + cos(nodep[i]->oldtheta);
        bot = (miny - nodep[i]->ycoord) / labelheight - sin(nodep[i]->oldtheta);
        lef = (minx - nodep[i]->xcoord) / labelheight - cos(nodep[i]->oldtheta);
        if (cos(nodep[i]->oldtheta) > 0) {
          if (sin(angle) > 0.0)
            top += sin(angle) * textlength[i];
          top += sin(angle - 1.25 * pi) * gap * firstlet[i];
          if (sin(angle) < 0.0)
            bot -= sin(angle) * textlength[i];
          bot -= sin(angle - 0.75 * pi) * gap * firstlet[i];
          if (sin(angle) > 0.0)
            rig += cos(angle - 0.75 * pi) * gap * firstlet[i];
          else
            rig += cos(angle - 1.25 * pi) * gap * firstlet[i];
          rig += cos(angle) * textlength[i];
          if (sin(angle) > 0.0)
            lef -= cos(angle - 1.25 * pi) * gap * firstlet[i];
          else
            lef -= cos(angle - 0.75 * pi) * gap * firstlet[i];
        } else {
          if (sin(angle) < 0.0)
            top -= sin(angle) * textlength[i];
          top += sin(angle + 0.25 * pi) * gap * firstlet[i];
          if (sin(angle) > 0.0)
            bot += sin(angle) * textlength[i];
          bot -= sin(angle - 0.25 * pi) * gap * firstlet[i];
          if (sin(angle) > 0.0)
            rig += cos(angle - 0.25 * pi) * gap * firstlet[i];
          else
            rig += cos(angle + 0.25 * pi) * gap * firstlet[i];
          if (sin(angle) < 0.0)
            rig += cos(angle) * textlength[i];
          if (sin(angle) > 0.0)
            lef -= cos(angle + 0.25 * pi) * gap * firstlet[i];
          else
            lef -= cos(angle - 0.25 * pi) * gap * firstlet[i];
          lef += cos(angle) * textlength[i];
        }
      }
      if (top > topoflabels)
        topoflabels = top;
      if (bot > bottomoflabels)
        bottomoflabels = bot;
      if (rig > rightoflabels)
        rightoflabels = rig;
      if (lef > leftoflabels)
        leftoflabels = lef;
    }
    topoflabels *= labelheight;
    bottomoflabels *= labelheight;
    leftoflabels *= labelheight;
    rightoflabels *= labelheight;
  }
  if (style != circular) {
    topoflabels = labelheight *
              (1.0 + sin(angle) * (maxtextlength - 0.5 * maxfirst)
                   + cos(angle) * 0.5 * maxfirst);
    rightoflabels = labelheight *
                    (cos(angle) * (textlength[nextnode-1] - 0.5 * maxfirst)
                   + sin(angle) * 0.5);
    leftoflabels = labelheight * (cos(angle) * leftfirst * 0.5
                                    + sin(angle) * 0.5);
  }
  rooty = miny;
  free(firstlet);
  free(textlength);
}  /* calculate */


void rescale()
{
  /* compute coordinates of tree for plot or preview device */
  long i;
  double treeheight, treewidth, extrax, extray, temp;
  treeheight = maxy - miny;
  treewidth = maxx - minx;
  if (style == circular) {
    treewidth = 1.0;
    treeheight = 1.0;
    if (!rescaled) {
      if (uselengths) {
        labelheight *= (maxheight - rooty) / treedepth;
        topoflabels *= (maxheight - rooty) / treedepth;
        bottomoflabels *= (maxheight - rooty) / treedepth;
        leftoflabels *= (maxheight - rooty) / treedepth;
        rightoflabels *= (maxheight - rooty) / treedepth;
        treewidth *= (maxheight - rooty) / treedepth;
      }
    }
  }
  treewidth += rightoflabels + leftoflabels;
  treeheight += topoflabels + bottomoflabels;
  if (grows == vertical) {
    if (!rescaled)
      expand = bscale;
    else {
      expand = (xsize - 2 * xmargin) / treewidth;
      if ((ysize - 2 * ymargin) / treeheight < expand)
        expand = (ysize - 2 * ymargin) / treeheight;
    }
    extrax = (xsize - 2 * xmargin - treewidth * expand) / 2.0;
    extray = (ysize - 2 * ymargin - treeheight * expand) / 2.0;
  } else {
    if (!rescaled)
      expand = bscale;
    else {
      expand = (ysize - 2 * ymargin) / treewidth;
      if ((xsize - 2 * xmargin) / treeheight < expand)
        expand = (xsize - 2 * xmargin) / treeheight;
    }
    extrax = (xsize - 2 * xmargin - treeheight * expand) / 2.0;
    extray = (ysize - 2 * ymargin - treewidth * expand) / 2.0;
  }
  for (i = 0; i < nextnode; i++) {
    nodep[i]->xcoord = expand * (nodep[i]->xcoord + leftoflabels);
    nodep[i]->ycoord = expand * (nodep[i]->ycoord + bottomoflabels);
    if ((style != circular) && (grows == horizontal)) {
      temp = nodep[i]->ycoord;
      nodep[i]->ycoord = expand * treewidth - nodep[i]->xcoord;
      nodep[i]->xcoord = temp;
    }
    nodep[i]->xcoord += xmargin + extrax;
    nodep[i]->ycoord += ymargin + extray;
  }
  if (style == circular) {
    xx0 = xmargin+extrax+expand*(leftoflabels + 0.5);
    yy0 = ymargin+extray+expand*(bottomoflabels + 0.5);
  }
  else if (grows == vertical)
      rooty = ymargin + extray;
    else
      rootx = xmargin + extrax;
}  /* rescale */


void plottree(node *p, node *q)
{
  /* plot part or all of tree on the plotting device */
  long i;
  double x00=0, y00=0, x1, y1, x2, y2, x3, y3, x4, y4,
           cc, ss, f, g, fract=0, minny, miny;
  node *pp;

  x2 = xscale * (xoffset + p->xcoord);
  y2 = yscale * (yoffset + p->ycoord);
  if (style == circular) {
    x00 = xscale * (xx0 + xoffset);
    y00 = yscale * (yy0 + yoffset);
  }
  if (p != root) {
    x1 = xscale * (xoffset + q->xcoord);
    y1 = yscale * (yoffset + q->ycoord);

    switch (style) {

    case cladogram:
      plot(penup, x1, y1);
      plot(pendown, x2, y2);
      break;

    case phenogram:
      plot(penup, x1, y1);
      if (grows == vertical)
        plot(pendown, x2, y1);
      else
        plot(pendown, x1, y2);
      plot(pendown, x2, y2);
      break;

    case curvogram:
      plot(penup, x1, y1) ;
      curvespline(x1,y1,x2,y2,(boolean)(grows == vertical),20);
      break;

    case eurogram:
      plot(penup, x1, y1);
      if (grows == vertical)
        plot(pendown, x2, (2 * y1 + y2) / 3);
      else
        plot(pendown, (2 * x1 + x2) / 3, y2);
      plot(pendown, x2, y2);
      break;

    case swoopogram:
      plot(penup, x1, y1);
      if ((grows == vertical && fabs(y1 - y2) >= epsilon) ||
          (grows == horizontal && fabs(x1 - x2) >= epsilon)) {
        if (grows == vertical)
          miny = p->ycoord;
        else
          miny = p->xcoord;
        pp = q->next;
        while (pp != q) {
          if (grows == vertical)
            minny = pp->back->ycoord;
          else
            minny = pp->back->xcoord;
          if (minny < miny)
            miny = minny;
          pp = pp->next;
        }
        if (grows == vertical)
          miny = yscale * (yoffset + miny);
        else
          miny = xscale * (xoffset + miny);
        if (grows == vertical)
          fract = 0.3333 * (miny - y1) / (y2 - y1);
        else
          fract = 0.3333 * (miny - x1) / (x2 - x1);
      } if ((grows == vertical && fabs(y1 - y2) >= epsilon) ||
          (grows == horizontal && fabs(x1 - x2) >= epsilon)) {
        if (grows == vertical)
          miny = p->ycoord;
        else
          miny = p->xcoord;
        pp = q->next;
        while (pp != q) {
          if (grows == vertical)
            minny = pp->back->ycoord;
          else
            minny = pp->back->xcoord;
          if (minny < miny)
            miny = minny;
          pp = pp->next;
        }
        if (grows == vertical)
          miny = yscale * (yoffset + miny);
        else
          miny = xscale * (xoffset + miny);
        if (grows == vertical)
          fract = 0.3333 * (miny - y1) / (y2 - y1);
        else
          fract = 0.3333 * (miny - x1) / (x2 - x1);
      }
      swoopspline(x1,y1,x1+fract*(x2-x1),y1+fract*(y2-y1),
                  x2,y2,(boolean)(grows != vertical),segments);        
      break;

  case circular:
      plot(penup, x1, y1);
      if (fabs(x1-x00)+fabs(y1-y00) > 0.00001) {
        g = ((x1-x00)*(x2-x00)+(y1-y00)*(y2-y00))
                       /sqrt(((x1-x00)*(x1-x00)+(y1-y00)*(y1-y00))
                             *((x2-x00)*(x2-x00)+(y2-y00)*(y2-y00)));
        if (g > 1.0) 
          g = 1.0;
        if (g < -1.0)
          g = -1.0;
        f = acos(g);
        if ((x2-x00)*(y1-y00)>(x1-x00)*(y2-y00))
          f = -f;
        if (fabs(g-1.0) > 0.0001) {
          cc = cos(f/segments);
          ss = sin(f/segments);
          x3 = x1;
          y3 = y1;
          for (i = 1; i <= segments; i++) {
            x4 = x00 + cc*(x3-x00) - ss*(y3-y00);
            y4 = y00 + ss*(x3-x00) + cc*(y3-y00);
            x3 = x4;
            y3 = y4;
            plot(pendown, x3, y3);
            }
          }
        }
      plot(pendown, x2, y2);
      break;

    }
  } else {
    if (style == circular) {
      x1 = x00;
      y1 = y00;
    } else {
      if (grows == vertical) {
        x1 =  xscale *  (xoffset + p->xcoord);
        y1 =  yscale *  (yoffset + rooty);
      } else {
        x1 =  xscale *  (xoffset + rootx);
        y1 =  yscale *  (yoffset + p->ycoord);
      }
    }
    plot(penup, x1, y1);
    plot(pendown, x2, y2);
  }
  if (p->tip)
    return;
  pp = p->next;
  while (pp != p) {
    plottree(pp->back, p);
    pp = pp->next;
  }
}  /* plottree */


void plotlabels(char *fontname)
{
  long i;
  double compr, dx = 0, dy = 0, labangle, cosl, sinl, cosv, sinv, vec;
  boolean left, right;
  node *lp;
  double *firstlet;

  firstlet = (double *)Malloc(nextnode*sizeof(double));
  textlength = (double *)Malloc(nextnode*sizeof(double));
  compr = xunitspercm / yunitspercm;
  if (penchange == yes)
    changepen(labelpen);
  for (i = 0; i < nextnode; i++) {
    if (nodep[i]->tip) {
      lp = nodep[i];
      firstlet[i] = lengthtext(nodep[i]->nayme,1L,fontname,font)
                              /fontheight;
      textlength[i] = lengthtext(nodep[i]->nayme, nodep[i]->naymlength,
                                fontname, font)/fontheight;
      labangle = nodep[i]->oldtheta;
      if (cos(labangle) < 0.0)
        labangle += pi;
      cosl = cos(labangle);
      sinl = sin(labangle);
      vec = sqrt(1.0+firstlet[i]*firstlet[i]);
      cosv = firstlet[i]/vec;
      sinv = 1.0/vec;
      if (style == circular) {
        right = cos(nodep[i]->oldtheta) > 0.0;
        left = !right;
        if (right) {
          dx = labelheight * expand * cos(nodep[i]->oldtheta);
          dy = labelheight * expand * sin(nodep[i]->oldtheta);
          dx -= labelheight * expand * 0.5 * vec * (cosl*sinv-sinl*cosv);
          dy -= labelheight * expand * 0.5 * vec * (sinl*sinv+cosl*cosv);
        }
        if (left) {
          dx = labelheight * expand * cos(nodep[i]->oldtheta);
          dy = labelheight * expand * sin(nodep[i]->oldtheta);
          dx -= labelheight * expand * textlength[i] * cosl;
          dy -= labelheight * expand * textlength[i] * sinl;
          dx += labelheight * expand * 0.5 * vec * (cosl*cosv+sinl*sinv);
          dy -= labelheight * expand * 0.5 * vec * (-sinl*cosv+cosl*sinv);
        }
      } else {
          dx = labelheight * expand * cos(nodep[i]->oldtheta);
          dy = labelheight * expand * sin(nodep[i]->oldtheta);
          dx += labelheight * expand * 0.5 * vec * (cosl*cosv+sinl*sinv);
          dy += labelheight * expand * 0.5 * vec * (-sinl*cosv+cosl*sinv);
        }
      if (style == circular) {
        plottext(lp->nayme, lp->naymlength,
             labelheight * expand * xscale / compr, compr,
             xscale * (lp->xcoord + dx + xoffset),
             yscale * (lp->ycoord + dy + yoffset), 180 * (-labangle) / pi,
             font,fontname);
      } else {
        if (grows == vertical)
          plottext(lp->nayme, lp->naymlength,
                   labelheight * expand * xscale / compr, compr,
                   xscale * (lp->xcoord + dx + xoffset),
                   yscale * (lp->ycoord + dy + yoffset),
                   -labelrotation, font,fontname);
        else
          plottext(lp->nayme, lp->naymlength, labelheight * expand * yscale,
                   compr, xscale * (lp->xcoord + dy + xoffset),
                   yscale * (lp->ycoord - dx + yoffset), 90.0 - labelrotation,
                   font,fontname);
      }
    }
  }
  if (penchange == yes)
    changepen(treepen);
  free(firstlet);
  free(textlength);
}  /* plotlabels */


void setup_environment(Char *argv[], boolean *canbeplotted)
{
  boolean firsttree;
  char* treestr;

  /* Set up all kinds of fun stuff */
#ifdef MAC
  OSErr retcode;
  FInfo  fndrinfo;
  macsetup("Drawgram","Preview");
#endif
#ifdef TURBOC
  if ((registerbgidriver(EGAVGA_driver) <0) ||
      (registerbgidriver(Herc_driver) <0)   ||
      (registerbgidriver(CGA_driver) <0)){
    printf("Graphics error: %s ",grapherrormsg(graphresult()));
    exit(-1);}
#endif

  

 
  printf("DRAWGRAM from PHYLIP version %s\n",VERSION);
  printf("Reading tree ... \n");
  firsttree = true;
  treestr = ajStrGetuniquePtr(&phylotrees[0]->Tree); 
  allocate_nodep(&nodep, treestr, &spp);
  treeread (&treestr, &root, treenode, &goteof, &firsttree,
            nodep, &nextnode, &haslengths,
            &grbg, initdrawgramnode);
  root->oldlen = 0.0;
  printf("Tree has been read.\n");
  printf("Loading the font .... \n");
  loadfont(font,argv[0]);
  printf("Font loaded.\n");
  previewing = false;
  ansi = ANSICRT;
  ibmpc = IBMCRT;
  firstscreens = true;
  initialparms();
  (*canbeplotted) = false;
}  /* setup_environment */

     
void user_loop(boolean *canbeplotted)
{
  
  long stripedepth;
  
  while (!(*canbeplotted)) {
/*
    // do {
    //  input_char=showparms();
    // firstscreens = false;
      //if (input_char != 'Y')
        //getparms(input_char);
    // } while (input_char != 'Y');
*/
    if (dotmatrix) {
      stripedepth = allocstripe(stripe,(strpwide/8),
                                ((long)(yunitspercm * ysize)));
      strpdeep = stripedepth;
      strpdiv  = stripedepth;
    }
    plotrparms(spp); 
    numlines = dotmatrix ? 
      ((long)floor(yunitspercm * ysize + 0.5) / strpdeep) :1;
    xscale = xunitspercm;
    yscale = yunitspercm;
    calculate();
    rescale();
    (*canbeplotted) = true;
    if (preview) {
      previewing = true;
      (*canbeplotted) = plotpreview(fontname,&xoffset,&yoffset,
                                  &scale,spp,root);
    } else {
      /*(*canbeplotted) = plot_without_preview(fontname,&xoffset,&yoffset,
                                           &scale,spp,root);*/
      (*canbeplotted)=true;
    }
    if ((previewer == winpreview || previewer == xpreview || previewer == mac) && (winaction == quitnow)) {
      break;
    }
  }
} /* user_loop */

int main(int argc, Char *argv[])
{
  boolean canbeplotted;
  boolean wasplotted = false;
#ifdef MAC
  OSErr retcode;
  FInfo  fndrinfo;
#ifdef OSX_CARBON
  FSRef fileRef;
  FSSpec fileSpec;
#endif
#ifdef __MWERKS__
  SIOUXSetTitle("\pPHYLIP:  Drawtree");
#endif
  argv[0] = "Drawgram";
#endif

  grbg = NULL;
  progname = argv[0];

#ifdef X
  nargc=argc;
  nargv=argv;
#endif
  
  init(argc, argv);
  emboss_getoptions("fdrawgram",argc,argv);

  setup_environment(argv, &canbeplotted);

  user_loop(&canbeplotted);
  if (!((previewer == winpreview || previewer == xpreview || previewer == mac) && (winaction == quitnow))) {
    
    previewing = false;
    initplotter(spp,fontname);
    numlines = dotmatrix ? ((long)floor(yunitspercm * ysize + 0.5)/strpdeep) : 1;
    if (plotter != ibm)
      printf("\nWriting plot file ...\n");
    drawit(fontname,&xoffset,&yoffset,numlines,root);
    finishplotter();
    FClose(plotfile);
    wasplotted = true;
    printf("\nPlot written to file \"%s\"\n\n", pltfilename);
  }
  FClose(intree);
#ifdef MAC
  if (plotter == pict && wasplotted){
#ifdef OSX_CARBON
    FSPathMakeRef((unsigned char *)pltfilename, &fileRef, NULL);
    FSGetCatalogInfo(&fileRef, kFSCatInfoNone, NULL, NULL, &fileSpec, NULL);
    FSpGetFInfo(&fileSpec, &fndrinfo);
    fndrinfo.fdType='PICT';
    fndrinfo.fdCreator='MDRW';
    FSpSetFInfo(&fileSpec, &fndrinfo);
#else
    retcode=GetFInfo(CtoPstr(PLOTFILE),0,&fndrinfo);
    fndrinfo.fdType='PICT';
    fndrinfo.fdCreator='MDRW';
    retcode=SetFInfo(CtoPstr(PLOTFILE),0,&fndrinfo);
#endif
  }
  if (plotter == lw && wasplotted){
#ifdef OSX_CARBON
    FSPathMakeRef((unsigned char *)pltfilename, &fileRef, NULL);
    FSGetCatalogInfo(&fileRef, kFSCatInfoNone, NULL, NULL, &fileSpec, NULL);
    FSpGetFInfo(&fileSpec, &fndrinfo);
    fndrinfo.fdType='TEXT';
    FSpSetFInfo(&fileSpec, &fndrinfo);
#else
    retcode=GetFInfo(CtoPstr(PLOTFILE),0,&fndrinfo);
    fndrinfo.fdType='TEXT';
    retcode=SetFInfo(CtoPstr(PLOTFILE),0,&fndrinfo);
#endif
  }
#endif
  printf("Done.\n\n");

#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif

  return 0;
}


