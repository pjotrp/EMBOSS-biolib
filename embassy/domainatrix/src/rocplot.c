/*  @source rocplot application
**
**  Provides interpretation and graphical display of the 
**  performance of discriminating elements (e.g. profiles for
**  protein families).  rocplot reads file(s) of hits from 
**  discriminator-database search(es), performs ROC analysis 
**  on the hits, and writes graphs illustrating the diagnostic 
**  performance of the discriminating elements.
**
**
**  @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
**  @@
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
**
*******************************************************************************
** 
**  ROCPLOT documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  A generic software tool for ROC analysis and the validation of predictive
**  methods.
**  Applied Bioinformatics (submitted).  
**  Jon C. Ison & Matthew J. Blades. 
** 
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  NOTES
**  Replace "AJFREE(today);" with a call to the destructor function when one 
**  becomes available.
**  Could possibly set more sensible values for barchart - wait for user 
**  feedback.
**  
******************************************************************************/



#include "emboss.h"
#include <math.h>




/* Constants and globals */
#define  NUMCLASSES    5
#define  TRUEHIT       0
#define  CROSSHIT      1
#define  UNCERTAINHIT  2
#define  UNKNOWNHIT    3	       
#define  FALSEHIT      4 	     

/* Do NOT change these values - see rocplot_write_barchart() */
#define  MAXBINS    20    /* For bar chart */
#define  BINSTART   0.0   /* For bar chart */
#define  BINSIZE    0.05  /* For bar chart */
    
char    *CLASSNAMES[]={"True", "Cross", "Uncertain", "Unknown", "False"};

AjPFile    errf      = NULL;



      


/******************************************************************************
**
** STRUCTURE DEFINITIONS
**
******************************************************************************/

/* @data AjPHitdata *********************************************************
**
** Hitdata object.
**
** Holds data for a hit.
**
** AjPHit is implemented as a pointer to a C data structure.
**
** @attr Class [AjPStr]    Classification of hit            
** @attr Acc  [AjPStr]    Accession number of hit sequence (if available)
** @attr Start [ajint]   Start of hit in sequence (if available)        
** @attr End [ajint]     End of hit in sequence (if available)          
**
** @alias AjOHitdata
** @alias AjSHitdata
**
****************************************************************************/
typedef struct AjSHitdata
{
    AjPStr    Class;      
    AjPStr    Acc;        
    ajint     Start;      
    ajint     End;        
} AjOHitdata, *AjPHitdata;


/* @data AjPXYdata **********************************************************
**
** XYdata object.
**
** Holds X/Y coordinate.
**
** AjPXYdata is implemented as a pointer to a C data structure.
**
** @attr X [float] X coordinate.
** @attr Y [float] Y coordinate.
**
** @alias AjOXYdata
** @alias AjSXYdata
**
****************************************************************************/
typedef struct AjSXYdata
{
    float     X;      
    float     Y;        
} AjOXYdata, *AjPXYdata;



/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjBool rocplot_write_summary(AjPDir outdir, 
				    AjPFile outf,
				    ajint mode, 
				    ajint multimode, 
				    ajint datamode, 
				    ajint numfiles, 
				    AjPStr *hitsnames,
				    ajint roc, 
				    AjPFloat rocn, 
				    AjPInt nrelatives);

static AjBool   rocplot_write_barchart(AjPDir outdir,
				       AjPStr fname, 
				       AjPStr title, 
				       AjPStr xlabel,
				       AjPStr ylabel, 
				       ajint nbins,
				       float binstart, 
				       float binsize,
				       AjPFloat rocn, 
				       ajint numfiles);

static AjBool   rocplot_write_classplot(AjPDir outdir,
					AjPStr fname,
					AjPStr title, 
					AjPStr xlabel,
					AjPStr ylabel, 
					ajint nseries,
					ajint filen, 
					AjPStr *legend,
					AjPFloat2d classx, 
					AjPFloat3d classy,
					ajint npoints);

static AjBool   rocplot_write_rocplot(AjPDir outdir,
				      AjPStr fname, 
				      AjPStr title, 
				      AjPStr xlabel,
				      AjPStr ylabel, 
				      ajint nseries,
				      AjPStr *legend, 
				      AjPFloat2d rocx,
				      AjPFloat2d rocy, 
				      AjPFloat rocn,
				      AjPInt hitcnt);

static AjBool rocplot_count_class(AjPHitdata tmphit,
				  ajint hitcnt, 
				  ajint plotn,
				  AjBool reset, 
				  AjPFloat2d *classx,
				  AjPFloat3d *classy, 
				  ajint *ntrue,
				  ajint *nottrue);

static AjBool rocplot_calcdata(int mode, 
			       int multimode, 
			       int datamode, 
			       AjPList *hitslists, 
			       ajint numfiles, 
			       ajint thresh,
			       ajint roc, 
			       AjPInt nrelatives, 
			       AjPFloat *rocn, 
			       AjPFloat2d *rocx,
			       AjPFloat2d *rocy, 
			       AjPFloat2d *classx, 
			       AjPFloat3d *classy, 
			       AjPInt *hitcnt,
			       AjBool norange);

static AjBool rocplot_overlap(AjPHitdata h1,
			      AjPHitdata h2,
			      ajint thresh);

static AjBool rocplot_hit_is_unique(AjPHitdata  hit,
				    AjPList mrglist, 
				    ajint thresh,
				    AjBool norange);

static void rocplot_HitdataDel(AjPHitdata *thys);

static AjPHitdata rocplot_HitdataNew(void);

static AjBool rocplot_read_hits_files(int mode,
			       int multimode,
			       int datamode, 
			       AjPList hitsfiles,
			       AjPStr **hitsnames, 
			       AjPList *hitslists,
			       AjPInt *nrelatives,
			       ajint *roc);

static void rocplot_XYdataDel(AjPXYdata *thys);

static AjPXYdata rocplot_XYdataNew(void);

static ajint rocplot_compX(const void *ptr1,
		    const void *ptr2);







/* @prog rocplot ************************************************************
**
** Provides interpretation and graphical display of the performance of 
** discriminating elements (e.g. profiles for protein families).
**
** rocplot reads file(s) of hits from discriminator-database search(es), 
** performs ROC analysis on the hits, and writes graphs illustrating the 
** diagnostic performance of the discriminating elements.
**
****************************************************************************/
int main(int argc, char **argv)
{
    /*************************/
    /* VARIABLE DECLARATIONS */
    /*************************/

    /* For ACD data */
    AjPList   hitsfiles  = NULL;    /* Directory of hits files from ACD. */
    AjPDir    outdir     = NULL;    /* Directory for output files.       */
    AjPStr   *mode       = NULL;    /* 
				     ** Mode of operation from ACD: 
				     ** 1: Single input file mode.
				     ** 2: Multiple input file mode. 
				     */
    AjPStr   *multimode  = NULL;    /* 
				     ** Mode of operation from ACD: 
				     ** 1: Do not combine data (multiple ROC 
				     **    curves).
				     ** 2: Combine data (single ROC curve).
				     */
    AjPStr   *datamode   = NULL;    /* 
				     ** Mode of operation from ACD:
				     ** 1: Single list of known true 
				     **    relatives.
				     ** 2: Multiple lists of known true 
				     **    relatives.
				     */
    ajint     thresh     = 0;       /* Overlap threshold for hits from ACD */
    AjPStr    rocbasename= NULL;    /* ROC plot file from ACD              */
    AjPFile   outfdata   = NULL;    /* Summary file from ACD.              */
    AjPStr    barbasename= NULL;    /* Bar chart for ROC value distribution  
                                       from ACD.                           */
    AjPStr    classbasename = NULL; /* Base name of file(s) containing 
				       classification plot(s) from ACD.    */
    AjBool    norange = ajFalse;    /* 	Disregard range data */
    

    /* For housekeeping */
    AjPStr     *hitsnames  = NULL;  /* Array of names only of hits files.  */
    ajint       modei      = 0;     /* ACD mode as int                     */
    ajint       multimodei = 0;     /* ACD multimode as int                */
    ajint       datamodei  = 0;	    /* ACD datamode as int                 */ 
    ajint       numfiles   = 0;     /* Number of input files               */
    ajint       x          = 0;     /* Loop counter                        */
    AjPStr      fullname   = NULL;  /* Full name of file(s) containing 
				       classification plot(s)              */ 
    AjPList    *hitslists  = NULL;  /* Lists of hits from hits files       */
    AjPHitdata  tmphit     = NULL;  /* Object for temp. data               */ 
    AjPInt      nrelatives = NULL;  /* No. known relatives from each hits 
				       file - array of token3 values.      */
    ajint       roc        = 0;     /* ROC number from input files         */
    AjPStr      tmpstr     = NULL;  /* Temp. string for freeing list       */
    
    
    /* For drawing the ROC plot */
    AjPStr     roctitle   = NULL;   /* Title of ROC plot                   */
    AjPStr     rocxlabel  = NULL;   /* x-axis label for ROC plot           */
    AjPStr     rocylabel  = NULL;   /* y-axis label for ROC plot           */
    ajint      nrocseries = 0;      /* No. of data series                  */
    AjPStr    *roclegend  = NULL;   /* Label for each data series          */
    AjPFloat2d rocx       = NULL;   /* Data points (x) for each data series*/
    AjPFloat2d rocy       = NULL;   /* Data points (y) for each data series*/
    
    /* For drawing the classification plot */
    AjPStr     classtitle   = NULL; /* Title of classification plot        */
    AjPStr     classxlabel  = NULL; /* x-axis label for classification plot*/
    AjPStr     classylabel  = NULL; /* y-axis label for classification plot*/
    AjPStr    *classlegend  = NULL; /* Label for each data series          */
    AjPFloat2d classx       = NULL; /* Data points (x) for each data series*/
    AjPFloat3d classy       = NULL; /* Data points (y) for each data series*/
    AjPFloat   rocn         = NULL; /* ROCn values                         */
    AjPInt     hitcnt       = 0;    /* Count of hits read from each input 
				       file.  Or, when there are multiple 
				       input files and the data are combined
				       hitcnt[0] only is used and gives the 
				       the sum of the counts. */
    
    
    /* For drawing the bar chart */
    AjPStr    bartitle   = NULL;    /* Title of bar chart                  */
    AjPStr    barxlabel  = NULL;    /* x-axis label for bar chart          */
    AjPStr    barylabel  = NULL;    /* y-axis label for bar chart          */
    ajint     nbins      = 0;       /* No. of data bins                    */
    float     binstart   = 0.0;     /* Start value of first bin            */
    float     binsize    = 0.0;     /* Bin size                            */

    /* DIAGNOSTICS 
    AjIList    iter      = NULL; */
    


    /********************/
    /* PROCESS ACD DATA */
    /********************/    
    ajNamInit("emboss");
    ajAcdInitP("rocplot", argc, argv, "DOMAINATRIX");

    hitsfiles     = ajAcdGetDirlist("hitsfilespath");
    outdir        = ajAcdGetOutdir("outdir");
    mode          = ajAcdGetList("mode");
    if(ajStrChar(*mode,0)=='2')
	multimode = ajAcdGetList("multimode");
    if((ajStrChar(*mode,0)=='2') && (ajStrChar(*multimode,0)=='2'))
	datamode  = ajAcdGetList("datamode");
    if((ajStrChar(*mode,0)=='2') && (ajStrChar(*multimode,0)=='2') && 
       (ajStrChar(*datamode,0)=='1'))
	thresh    = ajAcdGetInt("thresh");
    rocbasename       = ajAcdGetString("rocbasename");
    outfdata      = ajAcdGetOutfile("outfile");
    if((ajStrChar(*mode,0)=='2') && (ajStrChar(*multimode,0)=='1'))
	barbasename   = ajAcdGetString("barbasename");
    classbasename      = ajAcdGetString("classbasename");
    norange           = ajAcdGetBool("norange");
    

    /* DIAGNOSTICS */
    errf          = ajAcdGetOutfile("errfile");    




    /****************************************/
    /* MEMORY ALLOCATION AND INITIALISATION */
    /****************************************/
    /* Note that some memory is allocated even though all of it isn't used in
       all modes. This is to improve readability of the code. */

    /* Housekeeping */
    modei      = (ajint) ajStrChar(*mode,0)-48;
    if(modei==2)
	multimodei = (ajint) ajStrChar(*multimode,0)-48;
    if((modei==2) && (multimodei==2))
	datamodei  = (ajint) ajStrChar(*datamode,0)-48;    


    /* DIAGNOSTICS */
    ajFmtPrintF(errf, "MODE INFO\n");
    ajFmtPrintF(errf, "modei: %d\nmultimodei: %d\ndatamodei: %d\n", 
		modei, multimodei, datamodei);
    ajFmtPrintF(errf, "\n\n\n");    
    

    numfiles  = ajListLength(hitsfiles);
/*    printf("numfiles: %d\n", numfiles); */
    

    /* DIAGNOSTICS */
    ajFmtPrintF(errf, "NUMBER OF INPUT FILES\n");
    ajFmtPrintF(errf, "numfiles: %d\n", numfiles);
    ajFmtPrintF(errf, "\n\n\n");    


    AJCNEW0(hitslists, numfiles);
    for(x=0; x<numfiles; x++)
	hitslists[x] = ajListNew();
    nrelatives = ajIntNew();
    hitcnt     = ajIntNew();
    fullname   = ajStrNew();



    /* For ROC plot */
    roctitle     = ajStrNew();
    rocxlabel    = ajStrNew();
    ajStrAssC(&rocxlabel, "1 - SPEC");
    rocylabel    = ajStrNew();
    ajStrAssC(&rocylabel, "SENS");
    AJCNEW0(roclegend, numfiles);
    for(x=0;x<numfiles;x++)
	roclegend[x]=ajStrNew();
    rocx = ajFloat2dNew();
    rocy = ajFloat2dNew();    
    rocn = ajFloatNew();

    /* For Classification plot */
    classtitle   = ajStrNew();
    classxlabel  = ajStrNew();
    ajStrAssC(&classxlabel, "Number of hits detected");
    classylabel  = ajStrNew();
    ajStrAssC(&classylabel, "Proportion of hits detected that are of a "
	      "certain type");    
    AJCNEW0(classlegend, NUMCLASSES);
    for(x=0;x<NUMCLASSES;x++)
	classlegend[x]=ajStrNew();
    ajStrAssC(&classlegend[TRUEHIT], "TRUE");
    ajStrAssC(&classlegend[CROSSHIT], "CROSS");    
    ajStrAssC(&classlegend[UNCERTAINHIT], "UNCERTAIN");
    ajStrAssC(&classlegend[UNKNOWNHIT], "UNKNOWN");
    ajStrAssC(&classlegend[FALSEHIT], "FALSE");
    classx = ajFloat2dNew();
    classy = ajFloat3dNew();    

    /* For bar chart */
    bartitle    = ajStrNew();
    barxlabel   = ajStrNew();
    barylabel   = ajStrNew();





    /************************************/
    /* PARSE INPUT FILES & CHECK FORMAT */
    /************************************/
    /* hitsnames, hitslists, nrelatives & roc are written */
    if(!rocplot_read_hits_files(modei, multimodei, datamodei, 
				hitsfiles, &hitsnames, hitslists, 
				&nrelatives, &roc))
	ajFatal("rocplot_read_hits_files failed");
    

    /* DIAGNOSTICS */
    ajFmtPrintF(errf, "NAMES ONLY OF INPUT FILES\n");
    for(x=0; x<numfiles; x++)
	ajFmtPrintF(errf, "hitsnames[%d]: %S\n", x, hitsnames[x]);
    ajFmtPrintF(errf, "\n\n\n");    

    /* DIAGNOSTICS 
    ajFmtPrintF(errf, "LISTS OF HITS IN EACH FILE\n");
    for(x=0; x<numfiles; x++)    
    {
	ajFmtPrintF(errf, ">>>>>  FILE %d >>>>>\n", x);
	iter = ajListIterRead(hitslists[x]);
	while((tmphit = (AjPHitdata)ajListIterNext(iter)))
	    ajFmtPrintF(errf, "%S\n", tmphit->Class);
	ajListIterFree(&iter);
	ajFmtPrintF(errf, "\n\n\n");    	
    }	
    ajFmtPrintF(errf, "NUMBER OF RELATIVES\n");
    for(x=0; x<numfiles; x++)    
	ajFmtPrintF(errf, "nrelatives[%d]: %d\n", x, 
		    ajIntGet(nrelatives, x));
    ajFmtPrintF(errf, "\n\n\n");    	    
    */
    
    /*********************************************************************/
    /* CALCULATE x/y DATA FOR ROC AND CLASSIFICATION PLOTS AND ROC VALUE */
    /*********************************************************************/
    /* roc, rocx, rocy, classx and classy are written */
    if(!rocplot_calcdata(modei, multimodei, datamodei, hitslists, 
			 numfiles, thresh, roc, 
			 nrelatives, &rocn, &rocx, &rocy, &classx, &classy, 
			 &hitcnt, norange))
	ajFatal("rocplot_calcdata failed");


    
    /* DIAGNOSTICS */
    ajFmtPrintF(errf, "ROC NUMBER\n");
    ajFmtPrintF(errf, "roc: %d\n", roc);
    ajFmtPrintF(errf, "\n\n\n");    
    ajFmtPrintF(errf, "ROC VALUES\n");
    if((modei==2)&&(multimodei==1))
	for(x=0; x<numfiles; x++)    
	    ajFmtPrintF(errf, "rocn[%d]: %f\n", x, ajFloatGet(rocn, x));    
    else
	ajFmtPrintF(errf, "rocn[0]: %f\n", x, ajFloatGet(rocn, 0));    
    ajFmtPrintF(errf, "\n\n\n");    
    ajFmtPrintF(errf, "COUNT OF HITS\n");    
    if((modei==2) && (multimodei==2))
	ajFmtPrintF(errf, "hitcnt[0]: %d\n", ajIntGet(hitcnt, 0));	
    else
	for(x=0; x<numfiles; x++)    
	    ajFmtPrintF(errf, "hitcnt[%d]: %d\n", x, ajIntGet(hitcnt, x));
    ajFmtPrintF(errf, "\n\n\n");        
    /*
    ajFmtPrintF(errf, "ROC x,y DATA\n");
    for(x=0; x<numfiles; x++)    
    {
	ajFmtPrintF(errf, ">>>>>  FILE %d >>>>>\n", x);
	for(y=0; y<ajIntGet(hitcnt, x); x++) 
	    ajFmtPrintF(errf, "rocn[%d][%d]: %f\n", x, y, 
			ajFloatGet(rocn, x));
    }
    */

    for(x=0; x<numfiles; x++)    
	ajFmtPrint("Processing %S\n", hitsnames[x]);
    ajFmtPrint("Please wait ... ");
    

    /**********************************************************************/
    /* SET DATA FOR OUTPUT FILES. WRITE CLASSIFICATION PLOT AND BAR CHART */
    /**********************************************************************/

    /************************/
    /* 1. Single input file */
    /************************/
    if(modei==1)
    {
	/* Set plot data */
	nrocseries = 1;
	ajFmtPrintS(&roctitle, "ROC plot for %S", hitsnames[0]);
	ajFmtPrintS(&roclegend[0], "%S", hitsnames[0]);

	/* Write classification plot file */
	ajFmtPrintS(&classtitle, "Classification plot for %S", hitsnames[0]);	
	if(!rocplot_write_classplot(outdir, classbasename, classtitle, classxlabel, 
				    classylabel, NUMCLASSES, 0, classlegend, 
				    classx, classy, ajIntGet(hitcnt, 0)))
	    ajFatal("rocplot_write_classplot failed");
    }
    /****************************/
    /* 2. Multiple input files  */
    /****************************/
    else
    {
	/* Set plot data */
	ajFmtPrintS(&roctitle, "ROC plots for ");
	for(x=0; x<numfiles; x++)
	{
	    if( (x==(numfiles-1)) && (numfiles>1) )
		ajStrAppC(&roctitle, " & ");
	    else if(x>=1)
		ajStrAppC(&roctitle, ", ");
	    ajStrApp(&roctitle, hitsnames[x]);
	}


	
	/*******************************************************************/
	/* 2.1.  Do not combine data (multiple ROC curves in single ROC    */
	/*       plot, multiple classification plots, bar chart).          */
	/*******************************************************************/
	if(multimodei==1)
	{
	    /* Write classification plot files */
	    for(x=0; x<numfiles; x++)
	    {
		ajFmtPrintS(&classtitle, "Classification plot for %S", 
			    hitsnames[x]);	 

		ajFmtPrintS(&fullname, "%S%d", classbasename, x);

		if(!rocplot_write_classplot(outdir, fullname, classtitle, 
					    classxlabel, 
					    classylabel, NUMCLASSES, x, 
					    classlegend, classx, classy, 
					    ajIntGet(hitcnt, x)))
		    ajFatal("rocplot_write_classplot failed");
	    }
	    
	    /* Set plot data */
	    nrocseries = numfiles;
	    ajStrAppC(&roctitle, " (no combination)");
	    for(x=0; x<numfiles; x++)
		ajFmtPrintS(&roclegend[x], "%S", hitsnames[x]);
	    

	    /* Calculate data for bar chart and write file */
	    ajFmtPrintS(&bartitle, "Bar chart of distribution of ROC%d "
			"values", roc);
	    ajFmtPrintS(&barxlabel, "ROC%d value", roc);
	    ajStrAssC(&barylabel, "Frequency");
	    nbins    = MAXBINS;
	    binstart = BINSTART;
	    binsize  = BINSIZE;
	    /* Could possibly set more sensible values here - wait for 
	     user feedback. */
	    if(!rocplot_write_barchart(outdir, barbasename, bartitle, barxlabel, 
				       barylabel, nbins, binstart, binsize,
				       rocn, numfiles))
		ajFatal("rocplot_write_barchart failed");

	}
	/***************************************************************/
	/* 2.2.  Combine data (single ROC curve, single classification */
	/*       plot).                                                */
	/***************************************************************/
	else
	{
	    /* Write class plot data */
	    ajFmtPrintS(&classtitle, "Classification plot for ");
	    for(x=0; x<numfiles; x++)
	    {
		if( (x==(numfiles-1)) && (numfiles>1) )
		    ajStrAppC(&classtitle, " & ");
		else if(x>=1)
		    ajStrAppC(&classtitle, ", ");
		ajStrApp(&classtitle, hitsnames[x]);
	    }
	    
	    /* Write roc plot data */
	    nrocseries = 1;
	    ajFmtPrintS(&roclegend[0], "%s", "Combined dataset");

	    /**********************************************/
	    /* 2.2.1. Single list of known true relatives */
	    /**********************************************/
	    if(datamodei==1)
	    {
		ajStrAppC(&roctitle, " (combined - single list of targets)");
		ajStrAppC(&classtitle, 
			  " (combined - single list of targets)");
	    }
	    
	    /*************************************************/
	    /* 2.2.2. Multiple lists of known true relatives */
	    /*************************************************/	    
	    else
	    {	
		ajStrAppC(&roctitle, " (combined - multiple lists of "
			 "targets)");
		ajStrAppC(&classtitle, " (combined - multiple lists of "
			 "targets)");
	    }

	    /* Write classification plot files */
	    if(!rocplot_write_classplot(outdir, classbasename, classtitle, 
					classxlabel, 
					classylabel, NUMCLASSES, 0, 
					classlegend,
					classx, classy, ajIntGet(hitcnt, 0)))
		ajFatal("rocplot_write_classplot failed");

	}
    } 	
    /***********************/
    /* WRITE ROC PLOT FILE */
    /***********************/

    if(!rocplot_write_rocplot(outdir, rocbasename, roctitle, rocxlabel, 
			      rocylabel, 
			      nrocseries, roclegend, rocx, rocy, rocn, 
			      hitcnt))
	ajFatal("rocplot_write_rocplot failed");
			    
    
    /**********************/
    /* WRITE SUMMARY FILE */
    /* ********************/
    if(!rocplot_write_summary(outdir, outfdata, modei, 
			      multimodei, 
			      datamodei, 
			      numfiles, hitsnames, roc, rocn, 
			      nrelatives))	
	ajFatal("rocplot_write_summary failed");


    /********************************/
    /* FREE MEMORY AND EXIT CLEANLY */
    /********************************/

    /*  For ACD variables */
    while(ajListPop(hitsfiles, (void **)&tmpstr))
	ajStrDel(&tmpstr);
    ajListDel(&hitsfiles);
    ajStrDel(&mode[0]);
    AJFREE(mode);
    if(modei==2)
    {
	ajStrDel(&multimode[0]);
	AJFREE(multimode);
    }
    if((modei==2) && (multimodei==2))
    {
	ajStrDel(&datamode[0]);
	AJFREE(datamode);
    }
    ajStrDel(&rocbasename);
    ajFileClose(&outfdata);
    if((modei==2) && (multimodei==1))
	ajStrDel(&barbasename);
    ajStrDel(&classbasename);

    /* Housekeeping */
    for(x=0;x<numfiles;x++)
	ajStrDel(&hitsnames[x]);
    AJFREE(hitsnames);
    ajStrDel(&fullname);
    for(x=0;x<numfiles;x++)
    {
	while((ajListPop(hitslists[x], (void **)&tmphit)))
	    rocplot_HitdataDel(&tmphit);
	ajListDel(&hitslists[x]);
    }
    AJFREE(hitslists);
    ajIntDel(&nrelatives);
    ajIntDel(&hitcnt);

    /* For ROC plot */
    ajStrDel(&roctitle);
    ajStrDel(&rocxlabel);    
    ajStrDel(&rocylabel);    
    for(x=0;x<numfiles;x++)
	ajStrDel(&roclegend[x]);
    AJFREE(roclegend);
    ajFloat2dDel(&rocx);
    ajFloat2dDel(&rocy);    
    ajFloatDel(&rocn);

    /* For classification plot */
    ajStrDel(&classtitle);
    ajStrDel(&classxlabel);    
    ajStrDel(&classylabel);    
    for(x=0;x<NUMCLASSES;x++)
	ajStrDel(&classlegend[x]);
    AJFREE(classlegend);    
    ajFloat2dDel(&classx);
    ajFloat3dDel(&classy);    

    /* For bar chart */
    ajStrDel(&bartitle);
    ajStrDel(&barxlabel);
    ajStrDel(&barylabel);

    /* For output directory */
    ajDirDel(&outdir);
    
    
    /* DIAGNOSTICS */
    ajFileClose(&errf); 

    /* Exit cleanly */
    ajFmtPrint("done!\n");
    ajExit();
    return 0;
}





/* @funcstatic rocplot_read_hits_files **************************************
**
** Checks format of input files. 
** 
** Read each file in the directory of hits files provided by the user and
** check whether:
** 1. The file is in the correct format. 
** 2. The input file contains at least as many 'FALSE' hits as are 
**    specified after the 'ROC' token.
** 3. Where multiple input files are given, that the same value is given
**    after the 'ROC' token in each file.
** 4. Where multiple input files are given and the user specifies a single
**    list of known relatives, that the same value is given after the 
**    'RELATED' token in each file.
**
** Also:
** 1. Write lists of hits 
**
** @param [r] mode       [ajint]        Mode of operation from ACD: 
**				        1: Single input file mode.
**				        2: Multiple input file mode. 
** @param [r] multimode  [ajint]        Mode of operation from ACD: 
**			                1: Do not combine data (multiple
**                                         ROC curves).
**				        2: Combine data (single ROC 
**                                         curve).
** @param [r] datamode   [ajint]        Mode of operation from ACD: 
**			                1: Single list of known true 
**                                         relatives.
**				        2: Multiple lists of known true 
**                                         relatives.
** @param [r] hitsfiles  [AjPList]      Directory of hits files from ACD
** @param [w] hitsnames  [AjPStr **]    Input file names
** @param [w] hitslists  [AjPList *]    Array of lists of hits from hits 
**                                      files.
** @param [w] nrelatives [AjPInt *]     Number of known relatives from hits 
** @param [w] roc        [ajint *]      ROC number from input files.
** 
** @return [AjBool] True if all file formats were ok.
** @@
****************************************************************************/
static AjBool rocplot_read_hits_files(int mode, int multimode, int datamode, 
			       AjPList hitsfiles, AjPStr **hitsnames, 
			       AjPList *hitslists, AjPInt *nrelatives,
			       ajint *roc)
{
    /* Variables for parsing & checking format of a hits file.             */
    AjPStr    hitsline   = NULL;    /* Line from a hits file               */
    char      token1;               /* First token, must be '>'.           */
    AjPStr    token2     = NULL;    /* Second token, must be "RELATED".    */
    ajint     nrels      = 0;       /* Third token - no. known relatives 
				       from hits file.                     */
    char      token4;               /* Fourth token, must be ';'.          */
    AjPStr    token5     = NULL;    /* Fifth token, must be "ROC".         */
    ajint     roctmp     = 0;       /* Sixth token - ROC value from hits   
				       file                                */
    ajint     nrels_tmp  = 0;       /* Temp. variable for no. relatives    */
    AjPStr    class      = NULL;    /* Classification of hit from hits file*/
    AjPStr    accession  = NULL;    /* Accession number of hit             */
    ajint     hitstart   = 0;       /* Start of hit                        */
    ajint     hitend     = 0;       /* End of hit                          */


    /* Housekeeping variables */
    AjPStr    hitsfile   = NULL;    /* Name of hits file                   */
    AjPStr    tmpname    = NULL;    /* Temp. variable for holding file name*/
    AjPList   tmpnames   = NULL;    /* List for names only of hits files.  */    
    AjPFile   inf        = NULL;    /* For hits file (input)               */
    ajint     lastroc    = 0;       /* Last ROC value read in              */
    ajint     filecnt    = 0;       /* Counter for no. hits files parsed   */
    ajint     nfalse     = 0;       /* Count of FALSE hits parsed          */
    ajint     ntrue      = 0;       /* Count of TRUE hits parsed           */

    AjPHitdata   tmphit  = NULL;    /* Object for temp. data               */ 
    AjIList   iter       = NULL;    /* Generic list iterator               */
    ajint     x          = 0;       /* Loop counter                        */
    AjBool    ret        = ajFalse; /* Checks whether a line was read ok   */
    


    

    /* Check args */
    if(!hitsfiles || !hitsnames || !hitslists || !nrelatives)
	return ajFalse;
    if(!(*hitslists))
	return ajFalse;	
    if(!(hitslists[0]))
	return ajFalse;	
      




    /* Allocate temporary variables for parsing file */
    hitsline  = ajStrNew();
    class     = ajStrNew();
    token2    = ajStrNew();
    token5    = ajStrNew();
    accession = ajStrNew();
    tmpnames  = ajListstrNew();

    

    
    /* Memory is NOT freed before the ajFatal below */
    iter = ajListIterRead(hitsfiles);
    while((hitsfile = (AjPStr)ajListIterNext(iter)))
    {
	/* Create the list of file names only from full paths */
	tmpname   = ajStrNew();
	for(x=MAJSTRLEN(hitsfile)-1; x>=0; x--)
	    if(ajStrChar(hitsfile, x)=='/')
	    {
		ajStrAssSub(&tmpname, hitsfile, x+1, -1);
		break;
	    }
	if(!MAJSTRLEN(tmpname))
	    ajFatal("Zero length file name in rocplot_read_hits_files");
	else
	    ajListstrPushApp(tmpnames, tmpname);

	/* DIAGNOSTIC */
	ajFmtPrint("%S\n", hitsfile);
	fflush(stdout);
	
	
	/* Open input file and check format of first line */
	inf      = ajFileNewIn(hitsfile);
	if(!inf)
	    ajFatal("Cannot open file %S",hitsfile);

	/* Disregard comments lines beginning with '#' */
	while((ret=ajFileReadLine(inf,&hitsline)))
	{
	    if(ajStrChar(hitsline, 0)!='#')
		break;
	}
	if(!ret)
	    ajFatal("Empty file %S",hitsfile);

	if(ajFmtScanS(hitsline, "%c %S %d%c %S %d", &token1, &token2, 
		      &nrels, &token4, &token5, &roctmp)!=6)
	    ajFatal("Incorrect format %S",hitsfile);
	if((token1!='>') ||
	   (!ajStrMatchC(token2, "RELATED")) ||
	   (token4!=';') ||
	   (!ajStrMatchC(token5, "ROC")))
	    ajFatal("Incorrect format %S",hitsfile);
	
	
	/* Check that the same ROC number is specified in each of the
	   multiple input files. Assign number of relatives. */
	if(filecnt)
	    if(roctmp!=lastroc)
		ajFatal("Multiple input files but different values are "
			"given after the ROC tokens");
	lastroc = roctmp;
	ajIntPut(nrelatives, filecnt, nrels);
			

	/* Read hits from file and create list */
	nfalse   = 0;
	ntrue    = 0;
	while(ajFileReadLine(inf,&hitsline))
	{
	    if(ajStrChar(hitsline, 0)=='#')
		continue;

	    /* Check that all required data are present in hits line */
	    if((mode==2) && (multimode==2) && 
	       (datamode==1))
	    {
		if(ajFmtScanS(hitsline, "%S %S %d %d", &class, &accession, 
			      &hitstart, &hitend) != 4)
		    ajFatal("Incorrect format of hit line in %S",hitsfile);
	    }
	    else 
	    {
		ajFmtScanS(hitsline, "%S", &class);
	    } 
	    
	    /* Count number of false hits.
	       Any hit that isn't a 'TRUE' is classed as a 'FALSE' when 
	       calculating the ROC curve and value. */
	    if(ajStrMatchC(class, "TRUE"))
		ntrue++;
	    else
		nfalse++;
	    
	    /* Check for valid classification string */
	    if(!ajStrMatchC(class, "TRUE") &&
	       !ajStrMatchC(class, "CROSS") &&
	       !ajStrMatchC(class, "UNCERTAIN") &&
	       !ajStrMatchC(class, "UNKNOWN") &&
	       !ajStrMatchC(class, "FALSE"))
		ajFatal("Invalid classification of hit in %S",hitsfile);

	    /* Write hit to appropriate list */
	    tmphit = rocplot_HitdataNew();
	    if((mode==2) && (multimode==2) && 
	       (datamode==1))
	    {
		ajStrAssS(&tmphit->Class, class);
		ajStrAssS(&tmphit->Acc, accession);	
		tmphit->Start = hitstart;
		tmphit->End = hitend;
	    }
	    else
		ajStrAssS(&tmphit->Class, class);
	    ajListPushApp(hitslists[filecnt], (void *)tmphit);
	}
	/* Check there are at least as many FALSE hits as the ROC number */
	if(nfalse<roctmp)
	    ajFatal("Number of FALSE hits (%d) lower than ROC number (%d) "
		    "in %S", nfalse, roctmp, hitsfile);

	/* Check there are at least as many TRUE hits as the number after 
	   the RELATED token. */
	if(ntrue > nrels)
	    ajFatal("Number of TRUE hits (%d) greater than number (%d) "
		    "after RELATED token in %S", ntrue, nrels, hitsfile);

	ajFileClose(&inf);

	filecnt++;
    }   
    ajListIterFree(&iter);

    /* Create array of file names only */
    ajListToArray(tmpnames, (void***)hitsnames);
    
    /* Assign ROC number */
    *roc = roctmp;
        
    /* Check data in hits files is ok */
    /* Multiple input files, single list of known true relatives */
    if((mode==2) && (multimode==2) && 
       (datamode==1))    
    {
	nrels_tmp = ajIntGet(*nrelatives, 0);
	for(x=1; x<filecnt; x++)
	{
	    if(nrels_tmp != ajIntGet(*nrelatives, x))
		ajFatal("User specified multiple lists of hits with single "
			"list of known true relatives but different values "
			"were given after the RELATED tokens in the input"
			" files.");
	}
	
    }
    if(!filecnt)
	ajFatal("No hits files found in input directory");
    if((filecnt>1) && (mode==1))
	ajFatal("User specified single list of hits but multiple files found"
		" in input directory");
    if((filecnt==1) && (mode==2))
	ajFatal("User specified multiple lists of hits but single file only "
		"found in input directory");

    

    ajStrDel(&hitsline);
    ajStrDel(&token2);
    ajStrDel(&token5);
    ajStrDel(&class);
    ajStrDel(&accession);
    ajListstrDel(&tmpnames);


    return ajTrue;
}





/* @funcstatic rocplot_HitdataNew *******************************************
**
** Hitdata object constructor.
** This is normally called by the rocplot_HitdataNew function.
**
** @return [AjPHitdata] Pointer to an Hitdata object
** @@
****************************************************************************/
static AjPHitdata rocplot_HitdataNew(void)
{
    AjPHitdata ret = NULL;

    AJNEW0(ret);
    
    ret->Class = ajStrNew();
    ret->Acc   = ajStrNew();

    return ret;
}





/* @funcstatic rocplot_HitdataDel *******************************************
**
** Destructor for Hitdata object.
**
** @param [w] thys [AjPHitdata*] Hitdata object pointer
**
** @return [void]
** @@
****************************************************************************/
static void rocplot_HitdataDel(AjPHitdata *thys)
{
    AjPHitdata pthis = *thys;

    if(!thys || !pthis)
        return;


    ajStrDel(&pthis->Class);
    ajStrDel(&pthis->Acc);

    AJFREE(pthis);
    pthis = NULL;

    return;
}





/* @funcstatic rocplot_hit_is_unique ****************************************
**
**  Checks whether a hit is unique when compared to a list of hits.  
**  Two hits are 'unique' if they have (i) different
**  accesssion numbers or (ii) the same accession numbers but which do not 
**  overlap by any more than a user-defined number of residues.  The overlap
**  is determined from the start and end points of the hit.  For example two
**  hits both with the same accession numbers and with the start and end 
**  points of 1-100 and 91 - 190 respectively are considered to be the same
**  hit if the overlap threshold is 10 or less.
**  If 'norange' is true however (i.e. in cases where the 'norange' 
**  additional ACD option was set on)  then range data are disregarded when 
**  identifying unique hits: two hits are classed as unique if they have 
**  different accession numbers.
**
** @param [r] hit     [AjPHitdata] Hit to check for uniqueness
** @param [r] mrglist [AjPList]    List of hits
** @param [r] thresh  [ajint]      Overlap threshold
** @param [r] norange [AjBool]     Disregard range data
** 
** @return [AjBool] True if Hit did not occur in list.
** @@
**
****************************************************************************/
static AjBool rocplot_hit_is_unique(AjPHitdata  hit, AjPList mrglist, 
				    ajint thresh, AjBool norange)
{
    AjIList     iter      = NULL;    
    AjPHitdata  tmphit    = NULL; 

    iter = ajListIterRead(mrglist);
    while((tmphit = (AjPHitdata)ajListIterNext(iter)))
    {
	if(norange) 
	{
	    if(ajStrMatch(tmphit->Acc, hit->Acc))
	    {
		ajListIterFree(&iter);
		return ajFalse;
	    }
	}
	else
	    if(rocplot_overlap(tmphit, hit, thresh))
	    {
		ajListIterFree(&iter);
		return ajFalse;
	    }
    }
    ajListIterFree(&iter);
    return ajTrue;
}





/* @funcstatic rocplot_overlap **********************************************
**
** Checks for overlap and identical accession numbers between two hits.
**
** @param [r] h1     [AjPHitdata]  Pointer to hit 1
** @param [r] h2     [AjPHitdata]  Pointer to hit 2
** @param [r] thresh [ajint]       Threshold number of residues for overlap
**
** @return [AjBool] True if the hits have the same accession number and the 
** overlap between them is at least as long as the threshold. False 
** otherwise.
** @@
****************************************************************************/

static AjBool rocplot_overlap(AjPHitdata h1, AjPHitdata h2, ajint thresh)
{
    if( ((((h1->End - h2->Start + 1)>=thresh) && (h2->Start >= h1->Start)) ||
	 (((h2->End - h1->Start + 1)>=thresh) && (h1->Start >= h2->Start)))&&
       (ajStrMatch(h1->Acc, h2->Acc)))
	return ajTrue;

    return ajFalse;
}





/* @funcstatic rocplot_calcdata *********************************************
**
** Calculates x & y data points for ROC and classification plots.  Calculates
** ROCn value.
**
** @param [r] mode       [ajint]          Mode of operation from ACD: 
**				          1: Single input file mode.
**				          2: Multiple input file mode. 
** @param [r] multimode  [ajint]          Mode of operation from ACD: 
**			                  1: Do not combine data (multiple
**                                           ROC curves).
**				          2: Combine data (single ROC 
**                                           curve).
** @param [r] datamode   [ajint]          Mode of operation from ACD: 
**			                  1: Single list of known true 
**                                           relatives.
**				          2: Multiple lists of known true 
**                                           relatives.
** @param [r] hitslists  [AjPList *]      Array of lists of hits from hits 
**                                        files.
** @param [r] numfiles   [ajint]          Size of hitslists array, i.e. 
**                                        number of input files. 
** @param [r] thresh     [ajint]          Threshold overlap for hits  
** @param [r] roc        [ajint]          ROC number from input files.
** @param [r] nrelatives [AjPInt]         Number of known relatives from
**                                        hits files.
** @param [w] rocn       [AjPFloat   *]   Calculated ROCn values.
** @param [w] rocx       [AjPFloat2d *]   ROC x data to calculate.
** @param [w] rocy       [AjPFloat2d *]   ROC y data to calculate.
** @param [w] classx     [AjPFloat2d *]   Classification x data to calculate.
** @param [w] classy     [AjPFloat3d *]   Classification y data to calculate.
** @param [w] hitcnt     [AjPInt *]       Count of hits processed. 
** @param [r] norange    [AjBool]         Disregard range data
** 
** @return [AjBool] True on success.
** @@
**
****************************************************************************/
static AjBool rocplot_calcdata(int mode, int multimode, int datamode, 
			       AjPList *hitslists, ajint numfiles, 
			       ajint thresh, ajint roc, 
			       AjPInt nrelatives, AjPFloat *rocn, 
			       AjPFloat2d *rocx, AjPFloat2d *rocy, 
			       AjPFloat2d *classx, AjPFloat3d *classy, 
			       AjPInt *hitcnt, AjBool norange)
{ 
    ajint       maxfalse   = 0;       /* Number of FALSE hits to consider  */
    AjPList     mrglist    = NULL;    /* Single list for hits from multiple. 
					 input files                       */
    AjBool     *done       = NULL;    /* True if we have processed all hits  
					 in a list.                        */
    AjBool      doneall    = ajFalse; /* True if we have proceessed all 
					 hits in all lists.                */  
    AjIList    *iters      = NULL;    /* Iterators for hitslists.          */
    AjPHitdata  tmphit     = NULL;    /* Temp. variable for hit.           */
    ajint       x          = 0;       /* Loop counter.                     */
    ajint       y          = 0;       /* Loop counter.                     */
    ajint       ntrue      = 0;       /* Count of TRUE hits parsed         */
    ajint       nottrue    = 0;       /* Count of non-TRUE hits parsed     */
    AjBool      reset      = ajFalse; /* Set to True to reset hit counts   */
    float       sens       = 0.0;     /* Sensitivity                       */
    float       spec       = 0.0;     /* Specificity                       */
    float       ntrue_sum  = 0.0;     /* Temp. float for calculating ROCn  */
    float       rocn_tmp   = 0.0;     /* Temp. float for calculating ROCn  */
    ajint       sumrels    = 0;       /* Sum of number of relatives from 
					 each input file                   */
    AjPXYdata   xyptr      = NULL;    /* Temp. pointer for x,y  data.      */
    AjPList     xylist     = NULL;
    

    /* Check args */
    if(!hitslists || !nrelatives || !rocn || !rocx || !rocy || 
       !classx || !classy || !hitcnt)
	return ajFalse;
    if((!(*rocn)) || (!(*classx)) || (!(*classx)) || (!(*rocn)) || 
       (!(*hitcnt)))
	return ajFalse;
    if(!numfiles)
	return ajFalse;
    for(x=0; x<numfiles; x++)
	if(!hitslists[x])
	    return ajFalse;
    
    

    /* Multiple input files, combine data, multiple lists of known true 
       relatives */
    if((mode==2)&&(multimode==2)&&(datamode==2))    
	maxfalse = roc*numfiles;
    else
	maxfalse = roc;

    for(x=0; x<numfiles; x++)
	sumrels += ajIntGet(nrelatives, x);
    
    mrglist = ajListNew();

    AJCNEW0(done, numfiles);
    for(x=0; x<numfiles; x++)  
	done[x]=ajFalse;

    AJCNEW0(iters, numfiles);
    for(x=0; x<numfiles; x++)    
	iters[x]=ajListIterRead(hitslists[x]);
    
    /* Make hitcnt array safe for calls to ajIntGet */
    for(x=0; x<numfiles; x++)	    
	ajIntPut(hitcnt, x, 0);


    
    /* Multiple input files, combine data */
    if((mode==2)&&(multimode==2))
    {
	xylist = ajListNew();
	
	while(1)
	{
	    for(doneall=ajTrue, x=0; x<numfiles; x++)	    
		if(!done[x])
		{
		    doneall=ajFalse;
		    break;
		}	
	    if(doneall)
		break;
	    
	    for(x=0; x<numfiles; x++)	    
	    {
		if((tmphit = (AjPHitdata)ajListIterNext(iters[x])))
		{
		    if(((datamode==1) && 
			(rocplot_hit_is_unique(tmphit, mrglist, thresh, norange))) ||
		       (datamode==2))
		    {
			ajIntInc(hitcnt, 0);

			/* Calculate data for classification plot */
			if(!rocplot_count_class(tmphit, ajIntGet(*hitcnt, 0),
						0, ajFalse, 
						classx, classy,  &ntrue, 
						&nottrue))
			    ajFatal("rocplot_count_class failed");
			
			/* Calculate data for roc plot.  Any hit that isn't a 
			   'TRUE' is classed as a 'FALSE' when calculating the 
			   ROC curve and value. */
			if(!ajStrMatchC(tmphit->Class, "TRUE"))
			    ntrue_sum += (float) ntrue;
			/* DIAGNOSTICS 
			ajFmtPrintF(errf, "For hit %5d ntrue=%5d ntrue_sum=%5.3f\n", 
				    ajIntGet(*hitcnt, x),ntrue, ntrue_sum); */

			/* Single list of known true relatives */	
			if(datamode==1) 
			    /* All values of nrelatives are the same */
			    sens = (float) ((float)ntrue / 
					    (float)ajIntGet(nrelatives, x));
			/* Multiple lists of known true relatives */	
			else
			    sens = (float) ((float)ntrue / (float)sumrels);
			spec = (float) ((float)ntrue / 
					(float)ajIntGet(*hitcnt, 0));
			
			/* Create data point and populate list */
			xyptr = rocplot_XYdataNew();
			xyptr->X = (1 - spec);
			xyptr->Y = sens;
			ajListPushApp(xylist, (void *)xyptr);
			
			if(nottrue==maxfalse)
			{
			    /* Sort list of data points by X value and 
			       populate arrays */
			    y=0;
			    while(ajListPop(xylist, (void **)&xyptr))
			    {
				ajFloat2dPut(rocx, 0, y, xyptr->X);
				ajFloat2dPut(rocy, 0, y, xyptr->Y);		 

				rocplot_XYdataDel(&xyptr);
				y++;
			    }
			    if(y!=ajIntGet(*hitcnt, 0))
				ajFatal("Discrepancy in hits count in "
					"rocplot_calcdata");
			    ajListDel(&xylist);
			    break;
			}
		    }
		    ajListPushApp(mrglist, (void *) tmphit);
		}
		else
		    done[x]=ajTrue;
	    }
	    if(nottrue==maxfalse)
		break;
	}
	/* Calculate ROCn value */
	/* Single list of known true relatives */	
	if(datamode==1) 
	    rocn_tmp = ( 1 / (float)((float)roc * 
				     (float)ajIntGet(nrelatives, 0)))
		* ((float)ntrue_sum);
	
	/* Multiple lists of known true relatives */	
	else
	{
	    ajFmtPrint("roc: %d\nnumfiles: %d\nsumrels: %d\n"
		       "ntrue_sum: %f\n",
		       roc, numfiles, sumrels, ntrue_sum);
	    
	    rocn_tmp = ( 1 / (float)((float)roc * (float)numfiles *
				     (float)sumrels))
		* ((float)ntrue_sum);
	}
	
	
	ajFloatPut(rocn, 0, rocn_tmp);
    }
    /* Single input file OR
       Multiple input files, do not combine data */
    else
    {
	for(x=0; x<numfiles; x++)	    
	{
	    reset=ajTrue;
	    ntrue_sum=0.0;
	    
	    xylist = ajListNew();

	    while((tmphit = (AjPHitdata)ajListIterNext(iters[x])))
	    {
		ajIntInc(hitcnt, x);
		
		/* Calculate data for classification plot */
		if(!rocplot_count_class(tmphit, ajIntGet(*hitcnt, x), x, 
					reset, classx, classy, 
					&ntrue, &nottrue))
		    ajFatal("rocplot_count_class failed");

		/* Calculate data for roc plot.  Any hit that isn't a 'TRUE' 
		 is classed as a 'FALSE' when calculating the ROC curve and
		 value. */
		if(!ajStrMatchC(tmphit->Class, "TRUE"))
		    ntrue_sum += (float) ntrue;

		/* DIAGNOSTICS 
		ajFmtPrintF(errf, "For hit %5d ntrue=%5d ntrue_sum=%5.3f\n", 
			    ajIntGet(*hitcnt, x),ntrue, ntrue_sum); */
		

		/* DIAGNOSTICS 
		ajFmtPrintF(errf, "[%d]   sens = %d / %d      spec = %d / %d", 
			    ajIntGet(*hitcnt, x)-1, 
			    ntrue, 
			    ajIntGet(nrelatives, x), 
			    ntrue, 
			    ajIntGet(*hitcnt, x)); */
		

		sens = (float) ((float)ntrue / 
				(float)ajIntGet(nrelatives, x));
		spec = (float) ((float)ntrue / 
				(float)ajIntGet(*hitcnt, x));
				

		/* ajFmtPrintF(errf, "   (sens = %f  spec = %f\n",sens, spec);  */

		/* Create data point and populate list */
		xyptr = rocplot_XYdataNew();
		xyptr->X = (1 - spec);
		xyptr->Y = sens;

		ajListPushApp(xylist, (void *)xyptr);
			
		/* Housekeeping */
		reset = ajFalse;


		if(nottrue==maxfalse)
		{
		    /* Sort list of data points by X value and populate 
		       arrays */
		    y=0;
		    while(ajListPop(xylist, (void **)&xyptr))
		    {
			ajFloat2dPut(rocx, x, y, xyptr->X);
			ajFloat2dPut(rocy, x, y, xyptr->Y);		 
			

			/* DIAGNOSTICS 
			ajFmtPrintF(errf, "sens[%d]: %f   1-spec[%d]: %f\n", 
				    y, xyptr->Y, y, xyptr->X);	 */
			

			rocplot_XYdataDel(&xyptr);
			y++;
		    }
		    if(y!=ajIntGet(*hitcnt, x))
			ajFatal("Discrepancy in hits count in "
				"rocplot_calcdata");
		    ajListDel(&xylist);
		    break;
		}
	    }

	    /* Calculate ROCn value */
	    /* DIAGNOSTICS 
	    ajFmtPrintF(errf, "roc: %d\n"
		       "nrelativess: %d\n"
		       "ntrue_sum: %f\n",
		       roc, ajIntGet(nrelatives, x), ntrue_sum); */


	    rocn_tmp = (1 / (float)((float)roc * 
				    (float)ajIntGet(nrelatives, x)))
		* ((float)ntrue_sum);
	    
	    ajFloatPut(rocn, x, rocn_tmp);
	}
    }
    
    /* Free memory and return */
    ajListDel(&mrglist);
    AJFREE(done);
    
    for(x=0; x<numfiles; x++)
	ajListIterFree(&iters[x]);
    AJFREE(iters);

    return ajTrue;
}





/* @funcstatic rocplot_count_class ********************************************
**
** Calculates x,y data for classification plot. Makes counts of different 
** types of hit.
**  
** @param [r] tmphit  [AjPHitdata]     Hit to be classified.
** @param [r] hitcnt  [ajint]          Count of the hit.
** @param [r] plotn   [ajint]          Number of the classification plot.
** @param [r] reset   [AjBool]         True if counts are to be reset.
** @param [w] classx  [AjPFloat2d *]   Classification x data to calculate.
** @param [w] classy  [AjPFloat3d *]   Classification y data to calculate.
** @param [w] ntrue   [ajint *]        Count of TRUE hits.
** @param [w] nottrue [ajint *]        Count of non-TRUE hits.
** 
** @return [AjBool] True on success.
** @@
**
******************************************************************************/
static AjBool rocplot_count_class(AjPHitdata tmphit, ajint hitcnt, 
				  ajint plotn, AjBool reset, 
				  AjPFloat2d *classx, AjPFloat3d *classy, 
				  ajint *ntrue, ajint *nottrue)
{
    static ajint ncross     = 0;   /* Count of CROSS hits parsed so far    */
    static ajint nuncertain = 0;   /* Count of UNCERTAIN hits parsed so far*/ 
    static ajint nunknown   = 0;   /* Count of UNKNOWN hits parsed so far  */
    static ajint nfalse     = 0;   /* Count of FALSE hits parsed so far    */
 

    /* Check args */
    if(!tmphit || !classx || !classy)
	return ajFalse;
    
    if(!(*classx) || !(*classy))
	return ajFalse;
    
    
    if(reset)
    {	
	*ntrue     = 0;
	*nottrue   = 0;	
	nfalse     = 0;  
	ncross     = 0;
	nuncertain = 0;
	nunknown   = 0;
    }
    
    if(ajStrMatchC(tmphit->Class, "TRUE"))
	(*ntrue)++;
    else
 	(*nottrue)++;

    if(ajStrMatchC(tmphit->Class, "CROSS"))
	ncross++;
    else if(ajStrMatchC(tmphit->Class, "UNCERTAIN"))
	nuncertain++;
    else if(ajStrMatchC(tmphit->Class, "UNKNOWN"))
	nunknown++;
    else if(ajStrMatchC(tmphit->Class, "FALSE"))
	(nfalse)++;

    ajFloat3dPut(classy, plotn, TRUEHIT, hitcnt-1, 
		 (float)  ((float)*ntrue/(float)hitcnt));
    ajFloat3dPut(classy, plotn, CROSSHIT, hitcnt-1, 
		 (float)((float)ncross/(float)hitcnt));
    ajFloat3dPut(classy, plotn, UNCERTAINHIT, hitcnt-1, 
		 (float)((float)nuncertain/(float)hitcnt));
    ajFloat3dPut(classy, plotn, UNKNOWNHIT, hitcnt-1, 
		 (float)((float)nunknown/(float)hitcnt));
    ajFloat3dPut(classy, plotn, FALSEHIT, hitcnt-1, 
		 (float)((float)nfalse/(float)hitcnt));
    ajFloat2dPut(classx, plotn, hitcnt-1, (float)hitcnt);
    
    return ajTrue;
}





/* @funcstatic rocplot_write_rocplot ******************************************
**
** Writes a file of meta-data for drawing the ROC plot.
**
** @param [w] fname   [AjPStr]      Output file
** @param [r] title   [AjPStr]      Title of ROC plot  
** @param [r] xlabel  [AjPStr]      x-axis label for ROC plot
** @param [r] ylabel  [AjPStr]      y-axis label for ROC plot
** @param [r] nseries [ajint]       No. of data series
** @param [r] legend  [AjPStr *]    Label for each data series
** @param [r] rocx    [AjPFloat2d]  Data points (x) for each data series
** @param [r] rocy    [AjPFloat2d]  Data points (y) for each data series
** @param [r] rocn    [AjPFloat]    ROCn value for each dataset
** @param [r] hitcnt  [AjPInt]      No. of points in data series
** @param [r] outdir  [AjPDir]      Directory for output files
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
******************************************************************************/
static AjBool   rocplot_write_rocplot(AjPDir outdir,AjPStr fname, AjPStr title, 
				      AjPStr xlabel, AjPStr ylabel, 
				      ajint nseries, AjPStr *legend, 
				      AjPFloat2d rocx, AjPFloat2d rocy, 
				      AjPFloat rocn, AjPInt hitcnt)
{
    /*  Convert fname to an AjPFile if the graphics library functions are 
	ever adapated to take AjPFile rather than AjPStr. */

    /* For XML output 
    AjPGraph graph   = NULL;
    AjPStr   tmpstr  = NULL; */
    
    
    ajint    x       = 0;
    ajint    y       = 0;
    AjPFile  outf       = NULL;
    AjPStr   outfname   = NULL;
    AjBool   done       = ajFalse;
    AjPStr   shorttitle = NULL;
    AjPStr   shortlegend = NULL;    


    /* Check args */
    if(!fname || !title || !xlabel || !ylabel || !nseries || !legend ||
       !rocx || !rocy)
	return ajFalse;

    
    outfname   = ajStrNew();
    shorttitle = ajStrNew();
    shortlegend = ajStrNew();


    /* Create gnuplot data files */
    for(x=0; x<nseries; x++)
    {
	ajFmtPrintS(&outfname, "%S%S_dat%d", ajDirName(outdir), fname, x);
	if(!(outf = ajFileNewOut(outfname)))
	    ajFatal("Could not open gnuplot data file (%S) for writing in "
		    "rocplot_write_rocplot", outfname);
	ajFmtPrintF(outf, "# GNUPLOT data file for rocplot, series %d\n", x);
	for(y=0; y<ajIntGet(hitcnt, x); y++)
	    ajFmtPrintF(outf, "%-.3f    %-.3f\n", 
			ajFloat2dGet(rocx, x, y), 
			ajFloat2dGet(rocy, x, y));
	ajFileClose(&outf);
    }

    /* Create gnuplot driver file */
    ajStrAssS(&outfname, ajDirName(outdir));
    ajStrApp(&outfname, fname);

/*    if(!(outf = ajFileNewOut(fname))) */
    if(!(outf = ajFileNewOut(outfname)))
	ajFatal("Could not open gnuplot driver file (%S) for writing in "
		"rocplot_write_rocplot", fname);
    ajFmtPrintF(outf, "# GNUPLOT driver file for roc plot\n");
    /* The title is truncated to 50 characters */
    ajStrAssSub(&shorttitle, title, 0, 49);
    
    ajFmtPrintF(outf, "set title \"%50S\"\n", shorttitle);
    ajFmtPrintF(outf, "set xlabel \"%S\"\n", xlabel);
    ajFmtPrintF(outf, "set ylabel \"%S\"\n", ylabel);
    ajFmtPrintF(outf, "set nokey\n");	    
    ajFmtPrintF(outf, "set noautoscale\n");	    
    ajFmtPrintF(outf, "set xrange [0:1]\n");
    ajFmtPrintF(outf, "set yrange [0:1]\n");
    ajFmtPrintF(outf, "set key top outside title \"Data Series\" box 3\n");
    ajFmtPrintF(outf, "set data style points\n");
    ajFmtPrintF(outf, "set pointsize 0.45\n");
    ajFmtPrintF(outf, "plot ");

    /* The legend is truncated to 50 characters.  Only necessary to
       do this for legend[0] which in principle could get huge with 
       lots of input files. */
    ajStrAssSub(&shortlegend, legend[0], 0, 49);

    for(x=0; x<nseries; x++)
    {	
	if(done)
	    ajFmtPrintF(outf, ", ");
	ajFmtPrintS(&outfname, "%S_dat%d", fname, x);
	if(x==0)
	    ajFmtPrintF(outf, "\"%S\" smooth bezier title "
			"\"%S (%.3f)\"", outfname, shortlegend, 
			ajFloatGet(rocn, x));
	else
	    ajFmtPrintF(outf, "\"%S\" smooth bezier title "
			"\"%S (%.3f)\"", outfname, legend[x], 
			ajFloatGet(rocn, x));
	done = ajTrue;
    }
    ajFmtPrintF(outf, "\n");
    ajFileClose(&outf);
    

    /* For XML output 
    graph  = ajXmlCreateNewOutputFile();
    tmpstr = ajStrNew();
    ajStrAssC(&tmpstr, "Graph.shortTitle");
    
    
    for(x=0; x<nseries; x++)
    {
	ajXmlAddGraphicC(graph, "Graph");
	ajXmlAddMainTitle(graph, title);
	ajXmlAddXTitle(graph, xlabel);
	ajXmlAddYTitle(graph, ylabel);
	ajXmlAddGraphicOptionC(graph, tmpstr, legend[x]);
	ajXmlSetColourFromCode(graph, x);
	ajXmlAddJoinedLineSetF(graph, rocx->Ptr[x]->Ptr, 
			       rocy->Ptr[x]->Ptr, ajIntGet(hitcnt, x));
    }
    ajXmlWriteFile(graph, fname);

    ajStrDel(&tmpstr);
    ajXmlClearFile(graph);
    */

    ajStrDel(&outfname);
    ajStrDel(&shorttitle);
    ajStrDel(&shortlegend);
    return ajTrue;
}





/* @funcstatic rocplot_write_classplot ****************************************
**
** Writes a file of meta-data for drawing the ROC plot.
**
** @param [w] fname   [AjPStr]      Output file
** @param [r] title   [AjPStr]      Title of classification plot  
** @param [r] xlabel  [AjPStr]      x-axis label for classification plot
** @param [r] ylabel  [AjPStr]      y-axis label for classification plot
** @param [r] nseries [ajint]       No. of data series
** @param [r] filen   [ajint]       No. of input file
** @param [r] legend  [AjPStr *]    Label for each data series
** @param [r] x       [AjPFloat2d]  Data points (x) for each data series
** @param [r] y       [AjPFloat3d]  Data points (y) for each data series
** @param [r] npoints [ajint]       No. of points in data series
** @param [r] outdir  [AjPDir]      Directory for output files
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
*****************************************************************************/
static AjBool   rocplot_write_classplot(AjPDir outdir,AjPStr fname, 
					AjPStr title, 
					AjPStr xlabel, AjPStr ylabel, 
					ajint nseries, ajint filen, 
					AjPStr *legend, AjPFloat2d classx, 
					AjPFloat3d classy, ajint npoints)
{   	
    /*  Convert fname to an AjPFile if the graphics library functions are 
	ever adapated to take AjPFile rather than AjPStr. */

    /* For XML output 
    AjPGraph graph      = NULL;
    AjPStr   tmpstr     = NULL;*/ 

    ajint    x          = 0;
    ajint    y          = 0;
    AjPFile  outf       = NULL;
    AjPStr   outfname   = NULL;
    AjBool   done       = ajFalse;
    AjPStr   shorttitle = NULL;    

    /* Check args */
    if(!fname || !title || !xlabel || !ylabel || !nseries || !legend ||
       !classx || !classy || !npoints)
	return ajFalse;

    
    outfname   = ajStrNew();
    shorttitle = ajStrNew();


    /* Create gnuplot data files */
    for(x=0; x<nseries; x++)
    {
	if(ajFloat3dGet(classy, filen, x, npoints-1)!=0.0)
	{
	    ajFmtPrintS(&outfname, "%S%S_dat%d", ajDirName(outdir), fname, x);
	    if(!(outf = ajFileNewOut(outfname)))
		ajFatal("Could not open gnuplot data file (%S) for writing "
			"in rocplot_write_classplot", outfname);	   
	    ajFmtPrintF(outf, "# GNUPLOT data file for %s hits, series %d\n",
			CLASSNAMES[x], x);
	    for(y=0; y<npoints; y++)
		ajFmtPrintF(outf, "%-.3f    %-.3f\n", 
			    ajFloat2dGet(classx, filen, y), 
			    ajFloat3dGet(classy, filen, x, y));
	    ajFileClose(&outf);
	}
    }

    /* Create gnuplot driver file */
    ajStrAssS(&outfname, ajDirName(outdir));
    ajStrApp(&outfname, fname);

    if(!(outf = ajFileNewOut(outfname)))
	ajFatal("Could not open gnuplot driver file (%S) for writing in "
		"rocplot_write_classplot", fname);		
    ajFmtPrintF(outf, "# GNUPLOT driver file for classification plot\n");

    /* The title is truncated to 50 characters */
    ajStrAssSub(&shorttitle, title, 0, 49);
    ajFmtPrintF(outf, "set title \"%S\"\n", shorttitle);

    ajFmtPrintF(outf, "set xlabel \"%S\"\n", xlabel);
    ajFmtPrintF(outf, "set ylabel \"%S\"\n", ylabel);
    ajFmtPrintF(outf, "set nokey\n");	    
    ajFmtPrintF(outf, "set key top outside title \"Data Series\" box 3\n");
    ajFmtPrintF(outf, "set data style points\n");
    ajFmtPrintF(outf, "set pointsize 0.45\n");
    ajFmtPrintF(outf, "plot ");
    for(x=0; x<nseries; x++)
	if(ajFloat3dGet(classy, filen, x, npoints-1)!=0.0)
	{	
	    if(done)
		ajFmtPrintF(outf, ", ");
	    ajFmtPrintS(&outfname, "%S_dat%d", fname, x); 
	    ajFmtPrintF(outf, "\"%S\" smooth bezier title "
			"\"%s hits\"", outfname, CLASSNAMES[x]);
	    done = ajTrue;
	    
	}
    ajFmtPrintF(outf, "\n");
    ajFileClose(&outf);

    
    /* For XML output 
    graph  = ajXmlCreateNewOutputFile();
    tmpstr = ajStrNew();
    ajStrAssC(&tmpstr, "Graph.shortTitle");

    for(x=0; x<nseries; x++)
    {
	if(ajFloat3dGet(classy, filen, x, npoints-1)!=0.0)
	{
	    ajXmlAddGraphicC(graph, "Graph");
	    ajXmlAddMainTitle(graph, title);
	    ajXmlAddXTitle(graph, xlabel);
	    ajXmlAddYTitle(graph, ylabel);
	    ajXmlAddGraphicOptionC(graph, tmpstr, legend[x]);
	    ajXmlSetColourFromCode(graph, x);
	    ajXmlAddJoinedLineSetF(graph, classx->Ptr[filen]->Ptr, 
				   classy->Ptr[filen]->Ptr[x]->Ptr, npoints);
	}
	   
    }
    ajXmlWriteFile(graph, fname);

    ajStrDel(&tmpstr);
    ajXmlClearFile(graph);
    */

    ajStrDel(&shorttitle);
    ajStrDel(&outfname);

    return ajTrue;
}





/* @funcstatic rocplot_write_barchart *****************************************
**
** Writes a file of meta-data for drawing the ROC plot.
**
** @param [w] fname     [AjPStr]   Output file
** @param [r] title     [AjPStr]   Title of bar chart
** @param [r] xlabel    [AjPStr]   x-axis label for bar chart
** @param [r] ylabel    [AjPStr]   y-axis label for bar chart 
** @param [r] nbins     [ajint]    No. of data bins
** @param [r] binstart  [float]    Start value of first bin
** @param [r] binsize   [float]    Bin size
** @param [r] rocn      [AjPFloat] ROCn values
** @param [r] numfiles  [ajint]    No. of input files
** @param [r] outdir    [AjPDir]   Directory for output files
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
******************************************************************************/
static AjBool   rocplot_write_barchart(AjPDir outdir,AjPStr fname, 
				       AjPStr title, 
				       AjPStr xlabel, AjPStr ylabel, 
				       ajint nbins, float binstart, 
				       float binsize, AjPFloat rocn, 
				       ajint numfiles)

{
    /*  Convert fname to an AjPFile if the graphics library functions are 
	ever adapated to take AjPFile rather than AjPStr. */

    AjPInt   freqs   = NULL;
    ajint    maxfreq = 0;
    ajint    freq    = 0;
    float    roc     = 0.0;
    float    min     = 0.0;
    float    max     = 0.0;
    ajint    x       = 0;
    ajint    y       = 0;    
    AjBool   done    = ajFalse;
    AjPFile  outf       = NULL;
    AjPStr   outfname   = NULL;
    AjPStr   fulllegend = NULL;


    /* For XML output 
       AjPGraph graph   = NULL; */

    /* Check args */
    if(!fname || !title || !xlabel || !ylabel || !nbins || !binsize ||
       !rocn || !numfiles)
	return ajFalse;


    outfname   = ajStrNew();
    fulllegend = ajStrNew();
    
    /* Place data into bins (frequencies of different ranges of value of 
       rocn value) */
    freqs = ajIntNew();
    ajIntPut(&freqs, nbins-1, 0);
    
    for(x=0; x<numfiles; x++)
    {
	done = ajFalse;
	roc = ajFloatGet(rocn, x);
	for(y=0; y<nbins; y++)   
	{
	    min = y*binsize + binstart;
	    max = (y+1)*binsize + binstart;
	    if((roc>=min) && (roc<=max))
	    {
		ajIntPut(&freqs, y, (freq=(ajIntGet(freqs, y)+1)));
		if(freq>maxfreq)
		    maxfreq = freq;
		done = ajTrue;
		break;
	    }
	}
	if(!done)
	{
	    ajIntDel(&freqs);
	    ajFmtPrint("x: %d   numfiles: %d  nbins: %d   binsize: %f   "
		       "min: %f   max: %f   roc: %f\n", 
		       x, numfiles, nbins, binsize, min, max, roc);
	    
	    ajFatal("Could not put data in bin");
	}
    }

    
    /* Create gnuplot data files */
    ajFmtPrintS(&outfname, "%S%S_dat", ajDirName(outdir), fname);
    if(!(outf = ajFileNewOut(outfname)))
	ajFatal("Could not open gnuplot data file (%S) for writing in "
		"rocplot_write_barchart", outfname);	
    ajFmtPrintF(outf, "# GNUPLOT data file for barchart\n");

    for(y=0; y<MAXBINS; y++)
	ajFmtPrintF(outf, "%-.3f    %d\n", 
		    (0.05*(y+1))-0.025, 
		    ajIntGet(freqs, y));
    ajFileClose(&outf);

    
    /* Create gnuplot driver file */
    ajStrAssS(&outfname, ajDirName(outdir));
    ajStrApp(&outfname, fname);
        
    if(!(outf = ajFileNewOut(outfname)))
	ajFatal("Could not open gnuplot driver file (%S) for writing in "
		"rocplot_write_barchart", fname);
    ajFmtPrintF(outf, "set title \"%S\"\n", title);
    ajFmtPrintF(outf, "set xlabel \"%S\"\n", xlabel);
    ajFmtPrintF(outf, "set ylabel \"%S\"\n", ylabel);
    ajFmtPrintF(outf, "set nokey\n");	    
    ajFmtPrintF(outf, "set xtics 0.1\n");
    ajFmtPrintF(outf, "set xrange [0:1]\n");
    ajFmtPrintF(outf, "set yrange [0:%d]\n", maxfreq);
    ajFmtPrintF(outf, "plot \"%S_dat\" with boxes\n", fname); 
    ajFileClose(&outf);

    /* Create bar chart */
    /* For XML output 
    graph = ajXmlCreateNewOutputFile();
    ajXmlAddGraphicC(graph, "Graph");
    ajXmlAddMainTitle(graph, title);
    ajXmlAddXTitle(graph, xlabel);
    ajXmlAddYTitle(graph, ylabel);
    ajXmlAddHistogramEqualGapsF(graph, freqs->Ptr, nbins, 
				binstart, binsize);
    ajXmlWriteFile(graph, fname);
    ajXmlClearFile(graph);
    */

    ajStrDel(&outfname);
    ajStrDel(&fulllegend);
    ajIntDel(&freqs);
    return ajTrue;
}





/* @funcstatic rocplot_write_summary ******************************************
**
** Writes the summary file.
**
** @param [w] outf       [AjPFile]   Output file
** @param [r] mode       [ajint]     From ACD file
** @param [r] multimode  [ajint]     From ACD file
** @param [r] datamode   [ajint]     From ACD file
** @param [r] numfiles   [ajint]     Number of datasets (input files)
** @param [r] hitsnames  [AjPStr *]  Input file names
** @param [r] roc        [ajint]     ROC number
** @param [r] rocn       [AjPFloat]  ROCn value for each dataset
** @param [r] nrelatives [AjPInt]    Number of known relatives for dataset
** @param [r] outdir     [AjPDir]    Directory for output files
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
******************************************************************************/
static AjBool rocplot_write_summary(AjPDir outdir, AjPFile outf, ajint mode, 
				    ajint multimode, ajint datamode, 
				    ajint numfiles, AjPStr *hitsnames, 
				    ajint roc, AjPFloat rocn, 
				    AjPInt nrelatives)
{
    AjPTime today   = NULL;
    ajint   x       = 0;
    ajint   wid     = 0;
    float   mean    = 0.0;      /* Mean rocn value */
    double  sd      = 0.0;      /* Standard deviation of rocn values */
    float   sd_tmp  = 0.0;      /* Standard deviation of rocn values */    
    
    
    /* Check args */
    if((!outf) || (!hitsnames) || (!rocn) || (!nrelatives))
	return ajFalse;
    
    /* Calculate mean and SD */
    if((mode == 2) && (multimode == 1))
    {
	for(x=0; x<numfiles; x++)
	    mean += ajFloatGet(rocn, x);
	mean /= numfiles;

	for(x=0; x<numfiles; x++)
	    sd_tmp += (ajFloatGet(rocn, x) - mean) * 
		(ajFloatGet(rocn, x) - mean);
	sd_tmp /= numfiles;
	sd = sqrt((double)sd_tmp);
    }
    

    today = ajTimeTodayF("dd Mon yyyy");
    ajFmtPrintF(outf, "rocplot summary file (%D)\n\n", today);
    
    
    if(mode == 1)
	ajFmtPrintF(outf, "mode      == 1 (Single input file mode)\n");
    else
	ajFmtPrintF(outf, "mode      == 2 (Multiple input file mode)\n");
    
    if((mode == 2) && (multimode == 1))
	ajFmtPrintF(outf, "multimode == 1 (Do not combine data: multiple "
		    "ROC curves in single ROC plot, multiple classification "
		    "plots)\n");
    else if((mode == 2) && (multimode == 2))
	ajFmtPrintF(outf, "multimode == 2 (Combine data: single "
		    "ROC plot, single classification plot.)\n");

    if((mode == 2) && (multimode == 2) && (datamode == 1))
	ajFmtPrintF(outf, "datamode  == 1 (Single list of known true "
		    "relatives.)\n");
    else if((mode == 2) && (multimode == 2) && (datamode == 2))
	ajFmtPrintF(outf, "datamode  == 2 (Multiple lists of known true "
		    "relatives\n");
    ajFmtPrintF(outf, "\n");
    
    
    for(x=0; x<numfiles; x++)
	if((MAJSTRLEN(hitsnames[x]) > wid))
	    wid = MAJSTRLEN(hitsnames[x]);
    if(wid<10)
	wid = 10;
    

    if((mode == 2) && (multimode == 2))
    {
	ajFmtPrintF(outf, "%-15s%-15s\n", 
		    "File", 
		    "Known");
	for(x=0; x<numfiles; x++)
	    ajFmtPrintF(outf, "%-15S%-15d\n", 
			hitsnames[x], 
			ajIntGet(nrelatives, x));
    }
    else
    {
	ajFmtPrintF(outf, "%-15s%-15s%s%d\n", 
		    "File", 
		    "Known", 
		    "ROC", roc);
	for(x=0; x<numfiles; x++)
	    ajFmtPrintF(outf, "%-15S%-15d%-15.3f\n", 
			hitsnames[x], 
			ajIntGet(nrelatives, x), 
			ajFloatGet(rocn, x));
    }
    ajFmtPrintF(outf, "\n");

    if((mode == 2) && (multimode == 2) && (datamode==1))
    	ajFmtPrintF(outf, "ROC%-4d   == %.3f (combined)\n", roc, 
		    ajFloatGet(rocn, 0));
    else if((mode == 2) && (multimode == 2) && (datamode==2))
    	ajFmtPrintF(outf, "ROC%-4d   == %.3f (combined)\n", roc*numfiles, 
		    ajFloatGet(rocn, 0));

    if((mode == 2) && (multimode == 1))
	ajFmtPrintF(outf, "%-5sROC%-4d   == %.3f\n%-5sROC%-4d   == %.3f\n", 
		    "mean", roc, mean, 
		    "sd", roc, sd);
    	

    /* Replace this with a call to the destructor function when one 
       becomes available */
    AJFREE(today);
    
    return ajTrue;
}




/* @funcstatic rocplot_XYdataNew **********************************************
**
** XYdata object constructor.
** This is normally called by the rocplot_XYdataNew function.
**
** @return [AjPXYdata] Pointer to an XYdata object
** @@
******************************************************************************/
static AjPXYdata rocplot_XYdataNew(void)
{
    AjPXYdata ret = NULL;

    AJNEW0(ret);
    
    return ret;
}





/* @funcstatic rocplot_XYdataDel **********************************************
**
** Destructor for XYdata object.
**
** @param [w] thys [AjPXYdata*] XYdata object pointer
**
** @return [void]
** @@
******************************************************************************/
static void rocplot_XYdataDel(AjPXYdata *thys)
{
    AjPXYdata pthis = *thys;

    if(!thys || !pthis)
        return;

    AJFREE(pthis);
    pthis = NULL;

    return;
}


/* @funcstatic rocplot_compX **************************************************
**
** Function to sort AjOXYdata objects by X element. Usually called by 
** ajListSort.  
**
** @param [r] ptr1  [const void*] Pointer to AjOXYdata object 1
** @param [r] ptr2  [const void*] Pointer to AjOXYdata object 2
**
** @return [ajint] 1 if score1>score2, 0 if score1==score2, else -1.
** @@
******************************************************************************/

static ajint rocplot_compX(const void *ptr1, const void *ptr2)
{
    AjPXYdata a = NULL;
    AjPXYdata b = NULL;

    a = (*(AjPXYdata*)ptr1);
    b = (*(AjPXYdata*)ptr2);
    
    if(a->X > b->X)
	return 1;
    else if (a->X == b->X)
	return 0;
    return -1;
}




/* ****************************************************************************
**
** Dummy function to prevent compiler grumbling.
** 
******************************************************************************/

void seqsort_unused(void)
{
    rocplot_compX(NULL, NULL);
}


























