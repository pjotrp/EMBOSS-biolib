/* @source libscan application
 **
 ** Scans each signature, profile or HMM in a directory against 
 ** swissprot and writes a signature hits file for each one. Or 
 ** scans sequences against such a library of discriminating 
 ** elements and writes a library scan file for each one.
 **
 ** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
 ** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
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
 **
 **/

#include "emboss.h"
#include <math.h>


/* @data LibscanPCoord ********************************************************
**
** Undocumented
**
** @alias LibscanOCoord
** @alias LibscanSCoord
**
******************************************************************************/

typedef struct LibscanSCoord
{
    AjPStr   Class;        /* SCOP class                                     */
    AjPStr   Fold;         /* SCOP fold                                      */
    AjPStr   Superfamily;  /* SCOP superfamily                               */
    AjPStr   Family;       /* SCOP family                                    */
    ajint    Sunid_Family; /* the sun_id for a given SCOP family             */
    AjPStr   Model_Type;   /* The type of model used to generate the scores  */
    AjPStr   Acc;          /* Accession number of sequence entry             */
    AjPStr   Spr;          /* Swissprot code of sequence entry               */
    ajint    x;            /* The score interval                             */
    ajint    y;            /* Frequency of scores                            */
} LibscanOCoord;
#define LibscanPCoord LibscanOCoord*


/****************************************************************************
 **
 **
 **                              HMM RELATED FUNCTIONS
 **
 **
 ****************************************************************************/
static AjBool libscan_HmmSearch(AjPSeqset db, AjPStr hmmfile,
				AjPStr family,AjPStr superfamily, 
				AjPStr fold,AjPStr class,ajint sun_id, 
				AjPStr model, AjPFile outf);

static AjBool libscan_HmmLibScan(AjPSeq seq, AjPStr hmmpath, 
				 AjPStr hmmextn, AjPList scoplist, 
				 AjPStr model,AjPStr hmmoutpath, 
				 AjPStr hmmoutextn);

static AjBool libscan_RunHmmsearch(AjPStr filename, AjPStr db,
				   AjPStr outfname);

static AjBool libscan_RunHmmerInModeOne(AjPSeqset db, AjPStr hmmpath,
					AjPStr hmmextn, AjPStr *mode,
					AjPStr model,AjPList scoplist, 
					AjPStr hmmoutpath,AjPStr hmmoutextn);

static AjBool libscan_RunHmmerInModeTwo(AjPSeqset db, AjPStr hmmpath,
					AjPStr hmmextn, AjPStr *mode,
					AjPList scoplist, AjPStr model, 
					AjPStr hmmoutpath, AjPStr hmmoutextn);


/****************************************************************************
 **
 **
 **                                PROFILE RELATED FUNCTIONS
 **
 **
 ****************************************************************************/
static AjBool libscan_ProfileSearch(AjPSeqset db, AjPStr profile,
				    float gapopen, float gapextn,
				    AjPStr family,
				    AjPStr superfamily, AjPStr fold,
				    AjPStr class, ajint sun_id, 
				    AjPStr model, AjPFile outf);

static AjBool libscan_ProfileLibScan(AjPSeq seq, AjPStr path, AjPStr extn,
				     float gapopen, float gapextn,
				     AjPList scoplist, AjPStr model,
				     AjPStr profoutpath, AjPStr profoutextn);

static AjBool libscan_RunProphetInModeOne(AjPSeqset db, AjPStr profpath,
					  AjPStr profextn, float gapo,
					  float gape, AjPStr *mode,
					  AjPStr model,
					  AjPList scoplist,AjPStr profoutpath,
					  AjPStr profoutextn);

static AjBool libscan_RunProphetInModeTwo(AjPSeqset db, AjPStr profpath,
					  AjPStr profextn, float gapo,
					  float gape, AjPStr *mode,
					  AjPList scoplist,AjPStr model,
					  AjPStr profoutpath,AjPStr profoutextn);

static AjBool libscan_RunProphet(AjPStr profile, AjPStr db,
				 float gapopen, float gapextend,
				 AjPStr outfname);



/****************************************************************************
 **
 **
 **                        SIGNATURE RELATED FUNCTIONS
 **
 **
 ****************************************************************************/
static AjBool libscan_SignatureSearch(AjPSeqset db, AjPStr sigfile,
				      AjPMatrixf sub, float gapo, float gape,
				      ajint ntop,
				      AjPStr family, AjPStr superfamily,
				      AjPStr fold, AjPStr class, 
				      ajint sun_id,AjPStr model,
				      AjPFile outf);

static AjBool libscan_SignatureLibScan(AjPSeq seq, AjPStr path, AjPStr extn,
				       float gapopen,float gapextn,
				       AjPMatrixf sub,ajint ntopt, 
				       AjPList scoplist, AjPStr model,
				       AjPStr sigoutpath, AjPStr sigoutextn);

static AjBool libscan_RunSignatureInModeOne(AjPSeqset db, AjPStr sigpath,
					    AjPStr sigextn, float gapo,
					    float gape, AjPMatrixf sub,
					    ajint ntopt, AjPStr *mode,
					    AjPStr model,AjPList scoplist,
					    AjPStr sigoutpath, AjPStr sigoutextn);

static AjBool libscan_RunSignatureInModeTwo(AjPSeqset db, AjPStr sigpathpath,
					    AjPStr sigextn, float gapo,
					    float gape, AjPMatrixf sub,
					    ajint ntopt, AjPStr *mode,
					    AjPList scoplist, AjPStr model,
					    AjPStr sigoutpath,AjPStr sigoutextn);


/****************************************************************************
 **
 **
 **                   SAM RELATED FUNCTION
 **
 **
 ****************************************************************************/
static AjBool libscan_RunSamInModeOne(AjPStr samdb, AjPStr sampath,
				      AjPStr samextn, AjPStr *mode,
				      AjPStr model,AjPList scoplist,
				      AjPStr samoutpath, AjPStr samoutextn);

static AjBool libscan_RunSamInModeTwo(AjPSeqset db, AjPStr sampath,
				      AjPStr samextn, AjPStr *mode, 
				      AjPList scoplist, AjPStr model, 
				      AjPStr samoutpath, AjPStr samoutextn);

static AjBool libscan_SamSearch(AjPStr db, AjPStr samfile, 
				AjPStr family, AjPStr superfamily,
				AjPStr fold, AjPStr class, 
				ajint sun_id, AjPStr model, 
				AjPFile outf);



/****************************************************************************
 **
 **
 **                   PSSM RELATED FUNCTION
 **
 **
 ****************************************************************************/
static AjBool libscan_RunBlastpgp(AjPStr db, AjPStr pssmpath,
				      AjPStr pssmextn, AjPStr *mode,
				      AjPStr model,AjPList scoplist,
				      AjPStr pssmoutpath, AjPStr pssmoutextn);

static AjBool libscan_RunImpala(AjPSeqset db, AjPStr pssmpath,
				      AjPStr pssmextn, AjPStr *mode, 
				      AjPList scoplist, AjPStr model, 
				      AjPStr pssmoutpath, AjPStr pssmoutextn);

static AjBool libscan_PssmSearch(AjPStr db, AjPStr pssmfile, 
				AjPStr family, AjPStr superfamily,
				AjPStr fold, AjPStr class, 
				ajint sun_id, AjPStr model, 
				AjPFile outf);



/****************************************************************************
 **
 **
 **                    COMMON FUNCTIONS FOR DATA ANALYSIS
 **
 **
 ****************************************************************************/
static ajint libscan_CoordBinSearchScore(float score, LibscanPCoord *arr,
					 ajint siz);

static float libscan_ScoreToPvalue (float score, AjPList list);

static AjBool libscan_SunidToScopInfo (ajint sunid, AjPStr *family,
				       AjPStr *superfamily, AjPStr *fold,
				       AjPStr *klass, AjPList list);

static void libscan_CoordDel(LibscanPCoord *pthis);

static LibscanPCoord libscan_CoordNew(void);

static AjPList libscan_GetLibrary(AjPStr path, AjPStr extn);

static AjPList libscan_CalcDistribution(AjPList listhits, AjPStr class,
					AjPStr fold, AjPStr superfamily,
					AjPStr family, ajint sun_id);

static AjPList libscan_CalcLibscanDistribution(AjPList scophits);

static float libscan_CalcSdv(AjPList scophits, float mean);

static float libscan_ScoreToZscore(float score,float mean,float sdv);

static float libscan_CalcMean(AjPList scophits);





/****************************************************************************
 **
 **
 **                        MAIN FUNCTION
 **
 **
 ****************************************************************************/
int main(int argc, char **argv)
{
    AjPStr hmmpath     = NULL;	/* Directory of hmm models. */
    AjPStr hmmextn     = NULL;	/* File extension of models. */

    AjPStr sampath     = NULL;	/* Directory of sam models. */
    AjPStr samextn     = NULL;	/* File extension of models. */

    AjPStr pssmpath    = NULL;	/* Directory of sam models. */
    AjPStr pssmextn    = NULL;	/* File extension of models. */

    AjPStr gbvpath     = NULL;	/* Directory of gribskov profile models.  */
    AjPStr gbvextn     = NULL;	/* File extension of models. */

    AjPStr hnfpath     = NULL;	/* Directory of henikoff profile models.*/
    AjPStr hnfextn     = NULL;	/* File extension of models. */

    AjPStr sigpath     = NULL;	/* Directory of signatures.*/
    AjPStr sigextn     = NULL;	/* File extension of signatures */

    AjPStr hmmoutpath  = NULL;
    AjPStr hmmoutextn  = NULL;

    AjPStr gbvoutpath  = NULL;
    AjPStr gbvoutextn  = NULL;

    AjPStr hnfoutpath  = NULL;
    AjPStr hnfoutextn  = NULL;

    AjPStr sigoutpath  = NULL;
    AjPStr sigoutextn  = NULL;

    AjPStr samoutpath  = NULL;
    AjPStr samoutextn  = NULL;

    AjPStr pssmoutpath = NULL;
    AjPStr pssmoutextn = NULL;

    AjPStr mrgoutpath  = NULL;
    AjPStr mrgoutextn  = NULL;

    AjPStr outpath     = NULL;
    AjPStr outextn     = NULL;
    
    AjPStr model       = NULL;	/* the model type for the Model Record */
    AjPStr dbpath      = NULL;	/* location of database */
  
    AjPStr *nterm      = NULL;	/* Holds N-terminal matching options from acd */

    AjPFile scopf      = NULL;

    AjPSeqset db       = NULL;	/* The database to search */
  
    ajint ntopt    = 0;
    ajint cnt      = 0;		/* keep a track of the model types used */
    ajint maxhit   = 0;         /* pssm search option */
    ajint niter    = 0;         /* pssm search option */
    
    float thresh   = 0.0;       /* pssm search option */

    float gbvgapo  = 0.0;
    float gbvgape  = 0.0;
    float hnfgapo  = 0.0;
    float hnfgape  = 0.0;
    float siggapo  = 0.0;
    float siggape  = 0.0;
   
    AjPMatrixf sub = NULL;		/* Residue substitution matrix*/

    AjPList scoplist = NULL;		/* List of scop objects. */

  
    AjPScop entry = NULL;		/* scop object to put on to list */

    AjIList iter = NULL;

    AjPStr *mode = NULL;		/* Mode of operation from acd */


    AjBool  do_grib    = ajFalse;
    AjBool  do_henik   = ajFalse;
    AjBool  do_hmm     = ajFalse;
    AjBool  do_sam     = ajFalse;
    AjBool  do_pssm    = ajFalse;
    AjBool  do_sig     = ajFalse;

    ajNamInit("emboss");

    ajAcdInitP("libscan",argc,argv,"DOMAINATRIX");
  
    mode        = ajAcdGetList("mode");
    do_grib     = ajAcdGetBool("grib");
    do_henik    = ajAcdGetBool("henik");
    do_hmm      = ajAcdGetBool("hmm");
    do_sig      = ajAcdGetBool("sig");
    do_sam      = ajAcdGetBool("sam");
    do_pssm     = ajAcdGetBool("pssm");
    

    model  = ajStrNew();
    scoplist = ajListNew();

    /* initialise hmmer acd parameters */
    if(do_hmm)
    {
	cnt++;

	hmmpath    = ajAcdGetString("hmmpath");
	hmmextn    = ajAcdGetString("hmmextn");
	hmmoutpath = ajAcdGetString("hmmoutpath"); 
	hmmoutextn = ajAcdGetString("hmmoutextn");

	if(ajStrChar(*mode,0)=='1')
	{
	    if((!ajFileDir(&hmmoutpath)))
		ajFatal("Could not open directory");    
	}
    }  

    /* initialise sam-99 acd parameters */
    if(do_sam)
    {
	cnt++;
      
	sampath    = ajAcdGetString("sampath");
	samextn    = ajAcdGetString("samextn");
	samoutpath = ajAcdGetString("samoutpath"); 
	samoutextn = ajAcdGetString("samoutextn");

	if(ajStrChar(*mode,0)=='1')
	{
	    if((!ajFileDir(&samoutpath)))
		ajFatal("Could not open directory");    
	}
    }	

    /* initialise blastpgp acd parameters */
    if(do_pssm)
    {
	cnt++;
      
	pssmpath    = ajAcdGetString("pssmpath");
	pssmextn    = ajAcdGetString("pssmextn");
	pssmoutpath = ajAcdGetString("pssmoutpath"); 
	pssmoutextn = ajAcdGetString("pssmoutextn");
	maxhit      = ajAcdGetInt("maxhits");
	niter       = ajAcdGetInt("niter");
	thresh      = ajAcdGetFloat("thresh");

	if(ajStrChar(*mode,0)=='1')
	{
	    if((!ajFileDir(&pssmoutpath)))
		ajFatal("Could not open directory");    
	}
    }	

    /* initialise gribskov acd parameters */
    if(do_grib)
    {
	cnt++;
      
	gbvpath    = ajAcdGetString("gbvpath");

	if((!ajFileDir(&gbvpath)))
	    ajFatal("Could not open directory");    

	gbvextn    = ajAcdGetString("gbvextn");
	gbvgapo    = ajAcdGetFloat("gbvgapo");
	gbvgape    = ajAcdGetFloat("gbvgape");
	gbvoutpath = ajAcdGetString("gbvoutpath");
	gbvoutextn = ajAcdGetString("gbvoutextn");
 
	if(ajStrChar(*mode,0)=='1')
	{
	    if((!ajFileDir(&gbvoutpath)))
		ajFatal("Could not open directory");    
	}
    }

    /* initialise hennikoff acd parameters */
    if(do_henik)
    {
	cnt++;
      
	hnfpath    = ajAcdGetString("hnfpath");
	if((!ajFileDir(&hnfpath)))
	    ajFatal("Could not open directory");    
	hnfextn    = ajAcdGetString("hnfextn");
	hnfgapo    = ajAcdGetFloat("hnfgapo");
	hnfgape    = ajAcdGetFloat("hnfgape");
	hnfoutpath = ajAcdGetString("hnfoutpath"); 
	hnfoutextn = ajAcdGetString("hnfoutextn");

	if(ajStrChar(*mode,0)=='1')
	{
	    if((!ajFileDir(&hnfoutpath)))
		ajFatal("Could not open directory");    
	}
    }

    /* initialise signature acd parameters */
    if(do_sig)
    {
        cnt++;

	sigpath    = ajAcdGetString("sigpath");
	if((!ajFileDir(&sigpath)))
	    ajFatal("Could not open directory");    
	sigextn    = ajAcdGetString("sigextn");
	siggapo    = ajAcdGetFloat("siggapo");
	siggape    = ajAcdGetFloat("siggape");
	sub        = ajAcdGetMatrixf("sub");
	nterm      = ajAcdGetList("nterm");
	sigoutpath = ajAcdGetString("sigoutpath"); 
	sigoutextn = ajAcdGetString("sigoutextn");

	/*Assign N-terminal matching option */
	ajFmtScanS(nterm[0], "%d", &ntopt);

	if(ajStrChar(*mode,0)=='1')
	{
	    if((!ajFileDir(&sigoutpath)))
		ajFatal("Could not open directory");    
	}
    }

    /* initialise other parameters */
    db    = ajAcdGetSeqset("db");
    ajStrAssS(&dbpath,db->Filename);
    scopf = ajAcdGetInfile("scopf");


  
    /* create scop list */
    while((entry = (ajScopReadCNew(scopf, "*"))))
	ajListPushApp(scoplist,(AjPScop)entry); 
    ajFileClose(&scopf);
  
    /*sort scoplist by Sunid_Family prior to binary search*/
    ajListSort(scoplist,ajScopMatchSunid);


    /* run hmmer related searches */
    if(do_hmm)
    {
	ajStrAssC(&model,"HMM");
      
	if(!scoplist)
	    ajFatal("scoplist not good !\n");
      
	if(ajStrChar(*mode,0)=='1')
	    libscan_RunHmmerInModeOne(db,hmmpath,hmmextn,mode,
				      model,scoplist,hmmoutpath,hmmoutextn);
	else
	    libscan_RunHmmerInModeTwo(db,hmmpath,hmmextn,mode,scoplist,model,
				      hmmoutpath,hmmoutextn);
    }


    /* run sam-99 related searches */
    if(do_sam)
    {
	ajStrAssC(&model,"SAM");
      
	if(!scoplist)
	    ajFatal("scoplist not good !\n");
      
	if(ajStrChar(*mode,0)=='1')
	    libscan_RunSamInModeOne(dbpath,sampath,samextn,mode,
				    model,scoplist,samoutpath,samoutextn);
	else
	    libscan_RunSamInModeTwo(db,sampath,samextn,mode,scoplist,model,
				    samoutpath,samoutextn);
    }

    /* run psi-blast pssm related searches */
    if(do_pssm)
    {
	ajStrAssC(&model,"PSSM");
      
	if(!scoplist)
	    ajFatal("scoplist not good !\n");

	ajWarn("IMPORTANT - REMEMBER TO UNCOMMENT CODE\n");
	
	/*  IMPORTANT - REMEMBER TO UNCOMMENT THIS 
	if(ajStrChar(*mode,0)=='1')
	    libscan_RunBlastpgp(dbpath,maxhit,niter,thresh,pssmpath,pssmextn,
				mode,model,scoplist,pssmoutpath,pssmoutextn);
	else
	    libscan_RunImpala(db,pssmpath,pssmextn,mode,scoplist,model,
			      pssmoutpath,pssmoutextn);
         */
    }

    /* run gribskov related searches */
    if(do_grib)
    {
	ajStrAssC(&model,"GRIBSKOV");

	if(ajStrChar(*mode,0)=='1')
	    libscan_RunProphetInModeOne(db,gbvpath,gbvextn,gbvgapo,gbvgape,mode,
					model,scoplist,gbvoutpath,gbvoutextn);
	else
	    libscan_RunProphetInModeTwo(db,gbvpath,gbvextn,gbvgapo,gbvgape,mode,
					scoplist,model,gbvoutpath,gbvoutextn);
    }

    /* run hennikoff related searches */
    if(do_henik)
    {
	ajStrAssC(&model,"HENNIKOFF");

	if(ajStrChar(*mode,0)=='1')
	    libscan_RunProphetInModeOne(db,hnfpath,hnfextn,hnfgapo,hnfgape,mode,
					model,scoplist,hnfoutpath,hnfoutextn);
	else
	    libscan_RunProphetInModeTwo(db,hnfpath,hnfextn,hnfgapo,hnfgape,mode,scoplist,
					model,hnfoutpath,hnfoutextn);
    }

    /* run signature related searches */
    if(do_sig)
    {
	ajStrAssC(&model,"SIGNATURE");

	if(ajStrChar(*mode,0)=='1')
	    libscan_RunSignatureInModeOne(db,sigpath,sigextn,siggapo,siggape,sub,ntopt,mode,
					  model, scoplist,sigoutpath,sigoutextn);
	else
	    libscan_RunSignatureInModeTwo(db,sigpath,sigextn,siggapo,siggape,sub,ntopt,mode,
					  scoplist,model,sigoutpath,
					  sigoutextn);
    }


    /* clean up scoplist */
    iter = ajListIterRead(scoplist);
    while((entry = (AjPScop)ajListIterNext(iter)))
	ajScopDel(&entry);
    ajListDel(&scoplist);
    ajListIterFree(&iter);

    /* clean up hmmer parameters */
    if(do_hmm)
    {
	ajStrDel(&hmmpath);
	ajStrDel(&hmmextn);
	if(ajStrChar(*mode,0)=='1')
	{
	    ajStrDel(&hmmoutpath);
	    ajStrDel(&hmmoutextn);
	}
    }

    /* clean up sam parameters */
    if(do_sam)
    {
	ajStrDel(&sampath);
	ajStrDel(&samextn);
	if(ajStrChar(*mode,0)=='1')
	{
	    ajStrDel(&samoutpath);
	    ajStrDel(&samoutextn);
	}
    }

    /* clean up pssm parameters */
    if(do_pssm)
    {
	ajStrDel(&pssmpath);
	ajStrDel(&pssmextn);
	if(ajStrChar(*mode,0)=='1')
	{
	    ajStrDel(&pssmoutpath);
	    ajStrDel(&pssmoutextn);
	}
    }

    /* clean up gribskov parameters */
    if(do_grib)
    {
	ajStrDel(&gbvpath);
	ajStrDel(&gbvextn);
	if(ajStrChar(*mode,0)=='1')
	{
	    ajStrDel(&gbvoutpath);
	    ajStrDel(&gbvoutextn);        
	}
    }

    /* clean up hennikoff parameters */
    if(do_henik)
    {
	ajStrDel(&hnfpath);
	ajStrDel(&hnfextn);    
	if(ajStrChar(*mode,0)=='1')
	{
	    ajStrDel(&hnfoutpath);
	    ajStrDel(&hnfoutextn);
	}  
    }

    /* clean up signature parameters */
    if(do_sig)
    {
	ajStrDel(&sigpath);      
	ajStrDel(&sigextn);      
	if(ajStrChar(*mode,0)=='1')
	{
	    ajStrDel(&sigoutpath);
	    ajStrDel(&sigoutextn);
	}
    }

    /* clean up other parameters */
    if(ajStrChar(*mode,0)=='1')
    {
	ajStrDel(&mrgoutpath);
	ajStrDel(&mrgoutextn);
    }

    if(ajStrChar(*mode,0)=='2')
    {
	ajStrDel(&outpath);
	ajStrDel(&outextn);
    }
  
    ajSeqsetDel(&db);
    ajStrDel(&dbpath);

    if(do_sig)
      ajMatrixfDel(&sub);

    ajStrDel(&model);
    
  
    return 0;
    ajExit();
}




/* @funcstatic libscan_HmmSearch ********************************************
**
** Runs hmmsearch at the command line and writes the results to a specified
** file.
**
**
** @param [r] filename  [AjPStr]  Name of file containing the hmm.
** @paran [r] db        [AjPStr]  The database to be searched.
** @param [r] outfname  [AjPStr]  Name of output file for search results.
**
** @return [AjBool] Returns true on completion.

** @@
**
******************************************************************************/
static AjBool libscan_HmmSearch(AjPSeqset db, AjPStr hmmfile, AjPStr family,
				AjPStr superfamily,AjPStr fold, 
				AjPStr class, ajint sun_id,
				AjPStr model, AjPFile outf)
{
  ajint cnt           = 0;
  ajint nhits         = 0;

  AjPStr tmp            = NULL;
  AjPStr line           = NULL;       /* the next line in a file */
  AjPStr hmminfname     = NULL;       /* name of the hmmer input file */
  AjPStr hmmoutfname    = NULL;       /* name of the hmmer output file */

  AjPFile inf         = NULL;
  AjPFile hmminf      = NULL;         /* file pointer for the hmmer input file */

  AjPRegexp rexp1     = NULL;
  AjPRegexp rexp2     = NULL;
  AjPRegexp rexp3     = NULL;
  AjPRegexp rexp4     = NULL;
  AjPRegexp rexp5     = NULL;

  AjPStr  id          = NULL;         /* Hit identifier */
  AjPStr  startfrag   = NULL;         /* from regualr expression */
  ajint   start       = 0;            /* start of hit */

  AjPStr  endfrag     = NULL;         /* from the regular expression */
  ajint   end         = 0;            /* end of hit */

  AjPStr  scorefrag   = NULL;         /* from the regular expression */
  float   score       = 0.0;          /* score for alignment */

  AjPStr seqfrag      = NULL;
  AjPStr sequence     = NULL;


  AjPHit hit          = NULL;         /* hit that gets constructed in the first parse */
  AjPHit* hits        = NULL;

  AjPScophit scop     = NULL;

  AjPHitlist hitlist  = NULL;

  AjPList listhits    = NULL;         /* the list of hits */
  AjPList scophits    = NULL;
  AjPList tmplist     = NULL;

  AjIList iter        = NULL;         /* list iterator */


  if(!db || !hmmfile || !outf)
    {
      ajWarn("Bad arguments passed to libscan_HmmSearch\n");
      return ajFalse;
    }

  rexp1  = ajRegCompC("(^[0-9a-zA-Z_]+)");
  rexp2  = ajRegCompC("(from [0-9]+)");
  rexp3  = ajRegCompC("(to [0-9]+)");
  rexp4  = ajRegCompC("(score [0-9.-]+)");
  rexp5  = ajRegCompC("(E = )([0-9.e-]+)");
  
  line          = ajStrNew();
  tmp           = ajStrNew();
  hmminfname    = ajStrNew();
  hmmoutfname   = ajStrNew();
  startfrag     = ajStrNew();
  endfrag       = ajStrNew();
  scorefrag     = ajStrNew();
  id            = ajStrNew();
  seqfrag       = ajStrNew();
  sequence      = ajStrNew();
  
  listhits    = ajListNew();
  scophits    = ajListNew();
  tmplist     = ajListNew();
  
  /* CREAT TEMPERARY FILE NAMES */
  /* create the filename for the hmmersearch output file */
  ajRandomSeed();
  ajStrAssC(&tmp, ajFileTempName(NULL));
  
  ajStrAssS(&hmminfname,tmp);
  ajStrAppC(&hmminfname, ".hmminf");
  
  ajStrAssS(&hmmoutfname,tmp);
  ajStrAppC(&hmmoutfname, ".hmmoutf");
  
  /* GET SCORES FOR EACH SEQUENCE IN THE DATABASE */
  for(cnt = 0; cnt<db->Size;cnt++){
    hmminf    = ajFileNewOut(hmminfname);
    
    ajFmtPrintF(hmminf,">%S\n%S\n",db->Seq[cnt]->Name,db->Seq[cnt]->Seq);
    ajFileClose(&hmminf);
    
    /* RUN HMMSEARCH */
    libscan_RunHmmsearch(hmmfile, hmminfname, hmmoutfname);
        
    inf = ajFileNewIn(hmmoutfname);
    
    /* GET ALL INFOMATION ASSOCIATED WITH A MATCH */
    while(ajFileReadLine(inf,&line)){
      if(ajStrFindC(line,"Alignments of top-scoring domains:")>=0){
	while(ajFileReadLine(inf,&line)){
	  if(ajRegExec(rexp1,line) && ajRegExec(rexp2,line) && ajRegExec(rexp3,line)&&
	     ajRegExec(rexp4,line) && ajRegExec(rexp5,line)){
	    
	    /* Get the header for the current entry */
	    ajFmtScanS(line,"%S ",&id);
	    ajRegSubI(rexp1,1,&id);
	    
	    ajRegSubI(rexp2,1,&startfrag);
	    ajFmtScanS(startfrag,"%*s %d",&start);
	    
	    ajRegSubI(rexp3,1,&endfrag);
	    ajFmtScanS(endfrag,"%*s %d",&end);
	    
	    ajRegSubI(rexp4,1,&scorefrag);
	    ajFmtScanS(scorefrag,"%*s %f",&score);
	  }
	  
	  /* Get the sequence for the current entry */
	  else if(((ajStrFind(line,id))>=0)){
	    ajFmtScanS(line,"%*s %*d %S %*d",&seqfrag);
	    ajStrSubstituteCC(&seqfrag,"-","");
	    ajStrApp(&sequence,seqfrag);
	  }
	  
	  /* Print the sequence for the last entry */
	  else if((ajStrFindC(line,"Histogram of all scores:")>=0)){
	    /* construct the last  hit */
	    hit = embHitNew();

	    /* construct the hit */
	    ajStrAssS(&hit->Acc,id);
	    ajStrAssS(&hit->Model,model);
	    hit->Start = start;
	    hit->End   = end;
	    hit->Score = score;
	    ajStrAssS(&hit->Seq,sequence);
	      
	    ajListPushApp(listhits,hit);
	    ajStrAssC(&sequence,"");
	      
	    break;
	  }
	  
	  else
	    continue;
	}
      }
      
      else
	continue;
    }
    
    ajFileClose(&inf);
    ajSysUnlink(hmmoutfname);
    ajSysUnlink(hmminfname);
    
  }
  
  hitlist = embHitlistNew(0);
  
  /* write hit structure */
  ajStrAssS(&hitlist->Class,class);
  ajStrAssS(&hitlist->Fold,fold);
  ajStrAssS(&hitlist->Superfamily,superfamily);
  ajStrAssS(&hitlist->Family,family);
  ajStrAssS(&hitlist->Model,model);
  hitlist->Sunid_Family = sun_id;
  
  /* Convert list to array within Hitlist object */
  nhits=ajListToArray(listhits,(void ***)&hitlist->hits);
  hitlist->N = nhits;
  
  if(!(hitlist->N))
    ajFatal("hitlist->N==0 in libscan_HmmSearch\n");
  
  /* convert the hitlist to scophits so the related hits can be targetted for removal
     in order to calculate an unbiased distribution */
  ajListPushApp(tmplist,hitlist);
  embDmxHitlistToScophits(tmplist,&scophits);
  
  /*******************************************************************
   **
   ** THE HITLIST MUST BE WRITTEN OUT TO A FILE AND NOT THE HIT LIST
   ** THE MERGING SHOULD BE DONE AS A SEPARATE APPLICATION
   **
   *******************************************************************/
  ajDmxScophitsWriteFasta(outf,scophits);
    
  /* clean up tmplist / hitlist */
  /* Set N to zero so that the nodes in the list are not freed,
     the structure itself will be freed though */
  hitlist->N=0;
  embHitlistDel(&hitlist);
  ajListDel(&tmplist);
  
  /* clean up scophits */
  iter = ajListIter(scophits);
  while((scop = (AjPScophit)ajListIterNext(iter)))
    ajDmxScophitDel(&scop);
  ajListDel(&scophits);
  ajListIterFree(&iter);
  
  /* Clean up listhits */
  iter = ajListIter(listhits);
  while((hit = (AjPHit)ajListIterNext(iter)))
    embHitDel(&hit);
  ajListDel(&listhits);
  ajListIterFree(&iter);

  /* Clean up hits array */
  AJFREE(hits);

  /* clean up variables */
  ajRegFree(& rexp1);
  ajRegFree(& rexp2);
  ajRegFree(& rexp3);
  ajRegFree(& rexp4);
  ajRegFree(& rexp5);

  ajStrDel(&tmp );
  ajStrDel(&hmminfname);
  ajStrDel(&hmmoutfname);
  ajStrDel(&id);
  ajStrDel(&startfrag);
  ajStrDel(& endfrag);
  ajStrDel(&scorefrag);
  ajStrDel(&seqfrag);
  ajStrDel(&sequence);
  ajStrDel(&line);

  return ajTrue;
}



/* @func libscan_HmmLibScan ****************************************************
**
** Scan a protein sequence against a library of hidden markov models. The
** models represents SCOP portein domain families.
**
** @param [r] db       [AjPSeqset]  Name of sequence file.
** @param [r] path     [AjPStr]     Directory of hmm models.
** @param [r] extn     [AjPStr]     File extension of models.
** @param [r] scoplist [AjPList]    List of scop objects.
**
** @return [AjPList] returns familyhits.
** @@
**
******************************************************************************/
AjBool libscan_HmmLibScan(AjPSeq seq, AjPStr hmmpath, 
			  AjPStr hmmextn, AjPList scoplist, 
			  AjPStr model,AjPStr hmmoutpath, 
			  AjPStr hmmoutextn)
{
  AjPStr line        = NULL;		/* line from a file */

  AjPStr id          = NULL;		/* protein identification tag */

  AjPStr  startfrag  = NULL;		/* from regualr expression */
  ajint   start      = 0;		/* start of hit */

  AjPStr  endfrag    = NULL;		/* from the regular expression */
  ajint   end        = 0;			/* end of hit */

  AjPStr  scorefrag  = NULL;		/* from the regular expression */
  float   score      = 0.0;		/* score for alignment */

  AjPStr  seqfrag    = NULL;		/* the hit sequence fragment*/
  AjPStr  sequence   = NULL;		/* complete hit sequence */

  AjPStr hmmfile     = NULL;		/* Name of model file */

  AjPStr tmpname     = NULL;		/* Holds a randomly generated string */
  AjPStr sunidfrag   = NULL;		/* the Sunid_Family for bin search */

  AjPStr family      = NULL;		/* Name of scop family for a given entry*/
  AjPStr superfamily = NULL;		/* Name of scop superfamily for a given entry */
  AjPStr fold        = NULL;		/* Name of scop fold for a given entry */
  AjPStr class       = NULL;

  AjPStr outfile     = NULL;		/* Name of output file */

  AjPList modelnames = NULL;		/* A list of model names */
  AjPList familyhits = NULL;		/* A list of scophits structures corresponding to each SCOP family */
  AjPList tmplist    = NULL;


  AjPFile inf        = NULL;		/* input file pointer for hmmserach output file */
  AjPFile hmminf     = NULL;		/* file pointer for the hmmer input file */
  AjPFile hmmoutf    = NULL;

  AjPRegexp rexp1    = NULL;
  AjPRegexp rexp2    = NULL;
  AjPRegexp rexp3    = NULL;
  AjPRegexp rexp4    = NULL;
  AjPRegexp rexp5    = NULL;

  ajint posdash      = 0;
  ajint posdot       = 0;

  ajint sunid        = 0;

  ajint i            = 0;
  ajint nhits        = 0;

  float zvalue       = 0.0;
  float mean         = 0.0;
  float sdv          = 0.0;
  

  AjPScophit* hits   = NULL;
  AjPScophit hit     = NULL;		/* Scophit structure to hold the scored family */
  AjPScophit scophit = NULL;
  AjPScophit mhit    = NULL;

  AjIList iter       = NULL;		/* list iterator */

  AjPStr hmminfname   = NULL;		/* name of the hmmer input file */
  AjPStr hmmoutfname  = NULL;		/* name of the hmmer output file */


  line         = ajStrNew();
  id           = ajStrNew();
  hmmfile      = ajStrNew();
  tmpname      = ajStrNew();
  startfrag    = ajStrNew();
  endfrag      = ajStrNew();
  scorefrag    = ajStrNew();
  seqfrag      = ajStrNew();
  sequence     = ajStrNew();
  sunidfrag    = ajStrNew();
  hmminfname   = ajStrNew();
  hmmoutfname  = ajStrNew();
  outfile      = ajStrNew();

  family      = ajStrNew();
  superfamily = ajStrNew();
  fold        = ajStrNew();
  class       = ajStrNew();


  rexp1  = ajRegCompC("(^[0-9a-zA-Z_]+)");
  rexp2  = ajRegCompC("(from [0-9]+)");
  rexp3  = ajRegCompC("(to [0-9]+)");
  rexp4  = ajRegCompC("(score [0-9.-]+)");
  rexp5  = ajRegCompC("(E = )([0-9.e-]+)");

  if(!hmmpath || !seq || !hmmextn || !scoplist){
    ajWarn("Bad arguments passed to libscan_HmmLibScan\n");
    return ajFalse;
  }

  /* get the models */
  modelnames = libscan_GetLibrary(hmmpath,hmmextn);

  tmplist  = ajListNew();

  /* Initialise random number generator for naming of temp. files
     and create hmmer output file */
  ajRandomSeed();
  ajStrAssC(&tmpname, ajFileTempName(NULL));

  ajStrAssS(&hmminfname, tmpname);
  ajStrAppC(&hmminfname, ".hmminf");

  hmminf = ajFileNewOut(hmminfname);
  ajFmtPrintF(hmminf,">%S\n%S\n",seq->Name,seq->Seq);
  ajFileClose(&hmminf);

  ajStrAssS(&outfile,seq->Name);
  ajStrApp(&outfile,hmmoutextn);

  iter = ajListIter(modelnames);

  /*Start of main application loop*/
  while((hmmfile = (AjPStr)ajListIterNext(iter))){
    /* get the Sunid_Family from the hmmfile. THIS WILL NEED TO BE CHANGED */
    posdash = ajStrRFindC(hmmfile, "/");
    posdot  = ajStrRFindC(hmmfile, ".");

    if(posdash >= posdot)
      ajWarn("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");
    else{
      ajStrAssSub(&sunidfrag, hmmfile, posdash+1, posdot-1);
      ajStrToInt(sunidfrag,&sunid);
    }

    /* do a binary search on scoplist to find the relavent entry */
    if(!(libscan_SunidToScopInfo(sunid,&family,&superfamily,&fold,&class, scoplist)))
      ajFatal("libscan_SunidToScopInfo failed in libscan_HmmLibScan. email rranasin@hgmp.mrc.ac.uk\n");

    ajRandomSeed();
    ajStrAssC(&tmpname, ajFileTempName(NULL));

    ajStrAssS(&hmmoutfname, tmpname);
    ajStrAppC(&hmmoutfname, ".hmmoutf");

    libscan_RunHmmsearch(hmmfile, hmminfname, hmmoutfname);

    /* open hmmsearch output file */
    if(!(inf = ajFileNewIn(hmmoutfname)))
      ajWarn("File %S could not be opened\n",hmmoutfname);


    /* get scoring information and alignment */
    while(ajFileReadLine(inf,&line)){
      if(ajStrFindC(line,"Alignments of top-scoring domains:")>=0){
	while(ajFileReadLine(inf,&line)){
	  if(ajRegExec(rexp1,line) && ajRegExec(rexp2,line) && ajRegExec(rexp3,line) &&
	     ajRegExec(rexp4,line) && ajRegExec(rexp5,line)){
	    /* Get the header for the current entry */
	    ajFmtScanS(line,"%S ",&id);
	    ajRegSubI(rexp1,1,&id);

	    ajRegSubI(rexp2,1,&startfrag);
	    ajFmtScanS(startfrag,"%*s %d",&start);

	    ajRegSubI(rexp3,1,&endfrag);
	    ajFmtScanS(endfrag,"%*s %d",&end);

	    ajRegSubI(rexp4,1,&scorefrag);
	    ajFmtScanS(scorefrag,"%*s %f",&score);
	  }

	  /* Get the sequence*/
	  else if(((ajStrFind(line,id))>=0)){
	    ajFmtScanS(line,"%*s %*d %S %*d",&seqfrag);
	    ajStrApp(&sequence,seqfrag);
	  }

	  /* Print the sequence for the last entry */
	  else if((ajStrFindC(line,"Histogram of all scores:")>=0)){
	    /* construct the scophit structure */
	    hit = ajDmxScophitNew();
	    ajStrAssS(&hit->Class,class);
	    ajStrAssS(&hit->Fold,fold);
	    ajStrAssS(&hit->Superfamily,superfamily);
	    ajStrAssS(&hit->Family,family);
	    ajStrAssS(&hit->Acc,id);
	    ajStrAssS(&hit->Model,model);
	    ajStrAssS(&hit->Seq,sequence);
	    hit->Sunid_Family = sunid;
	    hit->Score = score;
	    hit->Start = start;
	    hit->End = end;

	    /* push the scop hit onto list */
	    ajListPushApp(tmplist,(AjPScophit) hit);

	    break;
	  }
	  else
	    continue;
	}
      }
      else
	continue;
    }

    ajFileClose(&inf);
    ajSysUnlink(hmmoutfname);
  }

  ajListIterFree(&iter);


  hmmoutf     = ajFileNewOutD(hmmoutpath,outfile);
  familyhits  = ajListNew();


  /* calculate the mean and standard deviation from tmplist*/
  mean = libscan_CalcMean(tmplist);
  sdv  = libscan_CalcSdv(tmplist,mean);


  /* convert the original list of hits to an array for convenience */
  nhits = ajListToArray(tmplist,(void ***)&hits);

  /* DO A SECOND PARSE THROUGH THE HITS FILE AND CALCULATE THE Z-VALUES */
  for(i=0;i<nhits;i++){
    scophit = ajDmxScophitNew();
    zvalue = libscan_ScoreToZscore(hits[i]->Score,mean,sdv);
    hits[i]->Pval = zvalue;
    ajDmxScophitCopy(&scophit,hits[i]);
    ajListPushApp(familyhits,scophit);
  }

  /* write out the familyhits list */
  ajDmxScophitsWriteFasta(hmmoutf,familyhits);
  ajFileClose(&hmmoutf);

  AJFREE(hits);

  /* clean up familyhits list */
  iter = ajListIter(familyhits);
  while((mhit = (AjPScophit)ajListIterNext(iter)))
    ajDmxScophitDel(&mhit);
  ajListIterFree(&iter);
  ajListDel(&familyhits);


  /* remove temparary files from directory */
  ajSysUnlink(hmminfname);
  ajSysUnlink(hmmoutfname);

  /* delete and clean up tmplist */
  iter=ajListIter(tmplist);
  while((hit=(AjPScophit)ajListIterNext(iter)))
    ajDmxScophitDel(&hit);
  ajListIterFree(&iter);
  ajListDel(&tmplist);


  /* delete and clean up modelnames */
  iter=ajListIter(modelnames);
  ajStrDel(&tmpname);
  while((tmpname=(AjPStr)ajListIterNext(iter)))
    ajStrDel(&tmpname);
  ajListIterFree(&iter);
  ajListDel(&modelnames);


  /* clean up */
  ajRegFree(&rexp1);
  ajRegFree(&rexp2);
  ajRegFree(&rexp3);
  ajRegFree(&rexp4);
  ajRegFree(&rexp5);

  ajStrDel(&hmmoutfname);
  ajStrDel(&hmminfname);
  ajStrDel(&line);
  ajStrDel(&id);

  ajStrDel(&startfrag);
  ajStrDel(&endfrag);
  ajStrDel(&scorefrag);
  ajStrDel(&seqfrag);
  ajStrDel(&sequence);
  ajStrDel(&sunidfrag);
  ajStrDel(&family);
  ajStrDel(&superfamily);
  ajStrDel(&fold);
  ajStrDel(&outfile);

  return ajTrue;
}


/* @funcstatic libscan_ProfileSearch ******************************************
**
** Runs prohet at the command line with a profile and writes the results to 
** a specified file.
**
** @param [r] filename  [AjPStr]  Name of file containing the gribskov profile.
** 
** @paran [r] db        [AjPStr]  The database to be searched.
** @param [r] outfname  [AjPStr]  Name of output file for search results.
**
** @return [AjBool] Returns true on completion.
** @@
** 
******************************************************************************/

static AjBool libscan_ProfileSearch(AjPSeqset db, AjPStr profile,
				    float gapopen, float gapextn, AjPStr family,
				    AjPStr superfamily, AjPStr fold, 
				    AjPStr class, ajint sun_id,
				    AjPStr model, AjPFile outf)
{
  ajint cnt           = 0;
  ajint nhits         = 0;
    
  AjPStr tmp           = NULL;
  AjPStr line          = NULL;         /* the next line in a file */
  AjPStr profoutname   = NULL;
  AjPStr profinfname   = NULL;         /* name of the gribskov input file */
  AjPStr profoutfname  = NULL;         /* name of the gribskov output file */

  
  AjPFile inf         = NULL;
  AjPFile profinf     = NULL;         /* file pointer for the gribskov input file */
    
  AjPStr  id          = NULL;         /* Hit identifier */
  
  ajint   tmpstart    = 0;
  ajint   start       = 0;            /* start of hit */
  ajint   end         = 0;            /* end of hit */
  
  AjPStr seqfrag      = NULL;
  AjPStr sequence     = NULL;
  
  float score        = 0.0;
  
  AjPHit hit          = NULL;
  AjPHit* hits        = NULL;
  
  AjPScophit scop     = NULL;
  
  AjPHitlist hitlist  = NULL;
  
  AjPList listhits    = NULL;         /* the list of hits */
  AjPList scophits    = NULL;
  AjPList tmplist     = NULL;
  
  AjIList iter        = NULL;         /* list iterator */

  
  if(! db || !profile || !outf){
    ajWarn("Bad arguments passed to libscan_HmmSearch\n");
    return ajFalse;
  }
  
  line          = ajStrNew();
  tmp           = ajStrNew();
  profinfname   = ajStrNew();
  profoutfname  = ajStrNew();
  profoutname   = ajStrNew();
  id            = ajStrNew();
  seqfrag       = ajStrNew();
  sequence      = ajStrNew();
  
  listhits      = ajListNew();
  scophits      = ajListNew();
  tmplist       = ajListNew();
  
  /* CREAT TEMPERARY FILE NAMES */
  ajRandomSeed();
  ajStrAssC(&tmp, ajFileTempName(NULL));
  
  ajStrAssS(&profoutname,tmp);
  ajStrAppC(&profoutname, ".profoutname");
  
  ajStrAssS(&profinfname,tmp);
  ajStrAppC(&profinfname, ".profinf");
  
  ajStrAssS(&profoutfname,tmp);
  ajStrAppC(&profoutfname, ".profoutf");

  /* GET SCORES FOR EACH SEQUENCE IN THE DATABASE */
  for(cnt = 0; cnt<db->Size;cnt++){
    profinf = ajFileNewOut(profinfname);
    
    ajFmtPrintF(profinf,">%S\n%S\n",db->Seq[cnt]->Name,db->Seq[cnt]->Seq);
    ajFileClose(&profinf);
    
    /* RUN PROPHET */
    libscan_RunProphet(profile, profinfname, gapopen, gapextn, profoutfname);
    
    inf = ajFileNewIn(profoutfname);
    
    /* GET ALL INFOMATION ASSOCIATED WITH A MATCH */
    while(ajFileReadLine(inf,&line)){
      if((ajStrPrefixC(line,"Local:"))){
	/* GET THE ID */
	ajFmtScanS(line,"%*s %*s %*s %S",&id);
	ajStrAssC(&sequence,"");
      }
      
      /* GET THE SCORE */
      else if(ajStrPrefixC(line,"Score:"))
	ajFmtScanS(line,"%*s %f",&score);
      
      /* GET THE SEQUENCE */
      else if(ajStrPrefix(line,id)){
	ajFmtScanS(line,"%*s %d %S %d",&tmpstart,&seqfrag,&end);
	
	/* get start position from the first seqfrag only */
	if(!ajStrCmpC(sequence,""))
	  start = tmpstart;
	
	ajStrSubstituteCC(&seqfrag,".","");
	ajStrApp(&sequence,seqfrag);
      }
      
      else
	continue;
    }
    
    ajFileClose(&inf);
    ajSysUnlink(profoutfname);
    ajSysUnlink(profinfname);
    
    
    /* create a new hit structure */
    hit = embHitNew();
    
    ajStrAssS(&hit->Acc,id);
    ajStrAssS(&hit->Model,model);
    hit->Start = start;
    hit->End   = end;
    hit->Score = score;
    ajStrAssS(&hit->Seq,sequence);
    
    nhits++;
  }
  
  /* CONSTRUCT THE HITLIST TO REMOVE RELATED HITS TO CREATE
     THE RANDOM SCORES NECCESSARY TO CARRY OUT THE MONTE
     CARLO SIMULATION */
  hitlist = embHitlistNew(0);
  
  /* write hit structure */
  ajStrAssS(&hitlist->Class,class);
  ajStrAssS(&hitlist->Fold,fold);
  ajStrAssS(&hitlist->Superfamily,superfamily);
  ajStrAssS(&hitlist->Family,family);
  ajStrAssS(&hitlist->Model,model);
  hitlist->Sunid_Family = sun_id;
  
  
  /* Convert list to array within Hitlist object */
  nhits=ajListToArray(listhits,(void ***)&hitlist->hits);
  hitlist->N = nhits;
  
    /* convert the hitlist to scophits so the related hits can be targetted for removal
     in order to calculate an unbiased distribution */
  ajListPushApp(tmplist,hitlist);
  embDmxHitlistToScophits(tmplist,&scophits);
  
  
  /*******************************************************************
   **
   ** THE HITLIST MUST BE WRITTEN OUT TO A FILE AND NOT THE HIT LIST
   ** THE MERGING SHOULD BE DONE AS A SEPARATE APPLICATION
   **
   *******************************************************************/
  ajDmxScophitsWriteFasta(outf,scophits);
  

  /* Set N to zero so that the nodes in the list are not freed,
     the structure itself will be freed though */
  hitlist->N=0;
  embHitlistDel(&hitlist);
  ajListDel(&tmplist);
  
  /* clean up scophits */
  iter = ajListIter(scophits);
  while((scop = (AjPScophit)ajListIterNext(iter)))
    ajDmxScophitDel(&scop);
  ajListDel(&scophits);
  ajListIterFree(&iter);
  
  
  /* Free listhits nodes */
  iter = ajListIter(listhits);
  while((hit = (AjPHit)ajListIterNext(iter)))
    embHitDel(&hit);
  ajListDel(&listhits);
  ajListIterFree(&iter);
    
  /* Clean up hits array */
  AJFREE(hits);
  
  
  /* clean up variables */
  ajStrDel(&tmp );
  ajStrDel(&profoutname);
  ajStrDel(&profinfname);
  ajStrDel(&profoutfname);
  ajStrDel(&id);
  ajStrDel(&seqfrag);
  ajStrDel(&sequence);
  ajStrDel(&line);
  
  return ajTrue;  
}

    
/* @funcstatic libscan_ProfileLibScan *****************************************
**
** Scan a protein sequence against a library of profile models. The 
** models represents SCOP portein domain families. 
**
** @param [r] seqfname [AjPStr]  Name of sequence file. 
** @param [r] path     [AjPStr]  Directory of profile models.
** @param [r] extn     [AjPStr]  File extension of models.
** @param [r] scoplist [AjPList] List of scop objects.
**
** @return [AjPList] retuns familyhits.
** @@
** 
******************************************************************************/
static AjBool libscan_ProfileLibScan(AjPSeq seq, AjPStr path, AjPStr extn,
				     float gapopen, float gapextn,
				     AjPList scoplist, AjPStr model,
				     AjPStr profoutpath, AjPStr profoutextn)
{
  AjPStr  line       = NULL;
  AjPStr  id         = NULL;		/* Hit identifier */
  
  ajint   tmpstart   = 0;
  ajint   start      = 0;		/* start of hit */
  
  ajint   end        = 0;		/* end of hit */
  
  float   score      = 0.0;		/* score for alignment */
  
  AjPStr  seqfrag    = NULL;		/* the hit sequence fragment*/
  AjPStr  sequence   = NULL;		/* complete hit sequence */
  
  AjPStr sunidfrag   = NULL;		/* the sunid as a string */
  
  AjPStr family      = NULL;		/* scop family */
  AjPStr superfamily = NULL;		/* scop superfamily */
  AjPStr fold        = NULL;		/* scop fold */
  AjPStr class       = NULL;
  
  
  AjPStr  profile    = NULL;		/* name of seqret output file */
  AjPStr  tmpname    = NULL;
  AjPStr  outfname   = NULL;
  
  AjPStr outfile     = NULL;
  
  AjPFile inf        = NULL;
  AjPFile profileinf = NULL;		/* file pointer for the hmmer input file */
  AjPFile profoutf    = NULL;
  
  AjPScophit hit     = NULL;		/* Scophit structure */
  AjPScophit* hits   = NULL;
  
  AjPScophit scophit = NULL;
  AjPScophit mhit    = NULL;
  
  ajint posdash      = 0;
  ajint posdot       = 0;
  ajint sunid        = 0;
  ajint i            = 0;		/* counter for the number of hits */
  ajint nhits        = 0;
  
  float zvalue       = 0.0;
  
  float mean          = 0.0;
  float sdv           = 0.0;
  
  AjIList iter       = NULL;		/* list iterator */
  
  AjPList familyhits = NULL;		/* a list of hits */
  AjPList modelnames = NULL;		/* A list of model names */
  AjPList tmplist    = NULL;
  
  AjPStr profileinfname  = NULL;	/* name of the hmmer input file */
  AjPStr profileoutfname = NULL;	/* name of the hmmer output file */
  
  
  line            = ajStrNew();
  seqfrag         = ajStrNew();
  sunidfrag       = ajStrNew();
  profile         = ajStrNew();
  tmpname         = ajStrNew();
  outfname        = ajStrNew();
  sequence        = ajStrNew();
  profileinfname  = ajStrNew();
  profileoutfname = ajStrNew();
  family          = ajStrNew();
  superfamily     = ajStrNew();
  fold            = ajStrNew();
  class           = ajStrNew();
  outfile         = ajStrNew();
  
  if(!path || !seq || !extn || !scoplist){
    ajWarn("Bad arguments passed to libscan_ProfileLibScan\n");
    return ajFalse;
  }
  
  /* get the models */
  modelnames = libscan_GetLibrary(path,extn);
  
  tmplist     = ajListNew();
  
  /* Initialise random number generator for naming of temp. files
     and create hmmer output file */
  ajRandomSeed();
  ajStrAssC(&tmpname, ajFileTempName(NULL));
  
  ajStrAssS(&profileinfname, tmpname);
  ajStrAppC(&profileinfname, ".profileinf");
  
  profileinf = ajFileNewOut(profileinfname);
  ajFmtPrintF(profileinf,">%S\n%S\n",seq->Name,seq->Seq);
  ajFileClose(&profileinf);
  
  ajStrAssS(&outfile,seq->Name);
  ajStrApp(&outfile,profoutextn);
  
  iter = ajListIter(modelnames);
  /*Start of main application loop*/
  while((profile = (AjPStr)ajListIterNext(iter))){
    /* get the Sunid_Family from the profile. THIS WILL NEED TO BE CHANGED */
    posdash = ajStrRFindC(profile, "/");
    posdot  = ajStrRFindC(profile, ".");
    
    if(posdash >= posdot)
      ajWarn("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");
    else{
      ajStrAssSub(&sunidfrag, profile, posdash+1, posdot-1);
      ajStrToInt(sunidfrag,&sunid);
    }
    
    /* do a binary search on scoplist to find the relavent entry */
    if(!(libscan_SunidToScopInfo(sunid,&family,&superfamily,&fold, &class, scoplist)))
      ajFatal("libscan_SunidToScopInfo failed in libscan_ProfileLibScan. email rranasin@hgmp.mrc.ac.uk\n");
    
    ajRandomSeed();
    ajStrAssC(&tmpname, ajFileTempName(NULL));
    
    ajStrAssS(&profileoutfname, tmpname);
    ajStrAppC(&profileoutfname, ".profileoutf");
    
    libscan_RunProphet(profile, profileinfname, gapopen, gapextn, profileoutfname);
    
    /* open hmmsearch output file */
    if(!(inf = ajFileNewIn(profileoutfname)))
      ajWarn("File %S could not be opened\n",profileoutfname);
    
    while(ajFileReadLine(inf,&line)){
      if((ajStrPrefixC(line,"Local:"))){
	/* GET THE ID */
	ajFmtScanS(line,"%*s %*s %*s %S",&id);
	ajStrAssC(&sequence,"");
      }
      
      /* GET THE SCORE */
      else if(ajStrPrefixC(line,"Score:"))
	ajFmtScanS(line,"%*s %f",&score);
      /* GET THE SEQUENCE */
      
      else if(ajStrPrefix(line,id)){
	ajFmtScanS(line,"%*s %d %S %d",&tmpstart,&seqfrag,&end);
	
	/* get start position from the first seqfrag only */
	if(!ajStrCmpC(sequence,""))
	  start = tmpstart;
	
	ajStrSubstituteCC(&seqfrag,".","");
	ajStrApp(&sequence,seqfrag);
      }
      
      else
	continue;
    }
    
    /* construct the scophit structure */
    hit = ajDmxScophitNew();
    ajStrAssS(&hit->Class,class);
    ajStrAssS(&hit->Fold,fold);
    ajStrAssS(&hit->Superfamily,superfamily);
    ajStrAssS(&hit->Family,family);
    ajStrAssS(&hit->Acc,id);
    ajStrAssS(&hit->Model,model);
    ajStrAssS(&hit->Seq,sequence);
    hit->Sunid_Family = sunid;
    hit->Score = score;
    hit->Start = start;
    hit->End = end;
    
    
    /*push the scop hit onto list */
    ajListPushApp(tmplist,(AjPScophit) hit);
    
    ajFileClose(&inf);
    ajSysUnlink(profileoutfname);
  }
  ajListIterFree(&iter);
  
  
  profoutf = ajFileNewOutD(profoutpath,outfile);
  
  familyhits  = ajListNew();
  
  /* convert the original list of hits to an array for convenience */
  nhits = ajListToArray(tmplist,(void ***)&hits);
  
  /* calculate the mean and standard deviation from tmplist*/
  mean = libscan_CalcMean(tmplist);
  sdv  = libscan_CalcSdv(tmplist,mean);
  
  /* DO A SECOND PARSE THROUGH THE HITS FILE AND CALCULATE THE Z-VALUES */
  for(i=0;i<nhits;i++){
    scophit = ajDmxScophitNew();

    zvalue = libscan_ScoreToZscore(hits[i]->Score,mean,sdv);
    hits[i]->Pval = zvalue;

    ajDmxScophitCopy(&scophit,hits[i]);
    ajListPushApp(familyhits,scophit);
  }
  
  /* write out the familyhits list */
  ajDmxScophitsWriteFasta(profoutf,familyhits);
  ajFileClose(&profoutf);
  
  AJFREE(hits);
  
  /* cleanup familyhits */
  iter = ajListIter(familyhits);
  while((mhit = (AjPScophit)ajListIterNext(iter)))
    ajDmxScophitDel(&mhit);
  ajListIterFree(&iter);
  ajListDel(&familyhits);
  ajSysUnlink(profileinfname);
  
  
  /* delete and clean up tmplist */
  iter=ajListIter(tmplist);
  while((hit=(AjPScophit)ajListIterNext(iter)))
    ajDmxScophitDel(&hit);
  ajListIterFree(&iter);
  ajListDel(&tmplist);
  
  
  /* delete and clean up modelnames */
  iter=ajListIter(modelnames);
  ajStrDel(&tmpname);    
  while((tmpname=(AjPStr)ajListIterNext(iter)))
    ajStrDel(&tmpname);
  ajListIterFree(&iter);
  ajListDel(&modelnames);
  
  ajStrDel(&tmpname);
  ajStrDel(&id);
  ajStrDel(&line);
  ajStrDel(&seqfrag);
  ajStrDel(&sequence);
  ajStrDel(&profile);
  ajStrDel(&tmpname);
  ajStrDel(&outfname);
  ajStrDel(&sunidfrag);
  ajStrDel(&fold);
  ajStrDel(&superfamily);
  ajStrDel(&family);
  ajStrDel(&profileinfname);
  ajStrDel(&profileoutfname);
  ajStrDel(&outfile);
  
  return ajTrue;
}



/* @funcstatic libscan_SignatureSearch ****************************************
**
** Runs prohet at the command line with a profile and writes the results to 
** a specified file.
**
** @param [r] filename  [AjPStr]  Name of file containing the gribskov profile.
** @paran [r] db        [AjPStr]  The database to be searched.
** @param [r] outfname  [AjPStr]  Name of output file for search results.
**
** @return [AjBool] Returns true on completion.
** @@
** 
******************************************************************************/
static AjBool libscan_SignatureSearch(AjPSeqset db, AjPStr sigfile,
				      AjPMatrixf sub, float gapo, 
				      float gape,ajint ntopt, AjPStr family, 
				      AjPStr superfamily,AjPStr fold, 
				      AjPStr class, ajint sun_id,
				      AjPStr model, AjPFile outf)
{
   AjPSignature sig        = NULL;       /* Signature data structure*/
   
   AjIList iter            = NULL;
   AjPList listhits        = NULL;
   AjPList scophits        = NULL;
   AjPList tmplist         = NULL;
   
   AjPFile sigin           = NULL;

   ajint cnt               = 0;
   ajint nhits             = 0;
      
   AjPSeq seq              = NULL;
   
   AjPHit hit              = NULL;
   AjPHit* hits            = NULL;
   
   AjPScophit scop         = NULL;
   AjPHitlist hitlist      = NULL;
   
   sigin    = ajFileNewIn(sigfile);
   
   listhits = ajListNew();
   tmplist  = ajListNew();
   scophits = ajListNew();
   
   
   /* READ SIGNATURE FILE */
   if(!(sig=embSignatureReadNew(sigin))){   
     ajMatrixfDel(&sub);
     ajFileClose(&sigin);
     return ajFalse;
     ajFatal("Error reading signature file");
   }   
   else
     ajFmtPrint("Signature file read ok\n");
   
   
   /* COMPILE SIGNATURE */
   if(!embSignatureCompile(&sig, gapo, gape, sub)){  
     embSignatureDel(&sig);
     ajMatrixfDel(&sub);
     ajFileClose(&sigin);
      return ajFalse;
      ajFatal("Error compiling signature");
   }  
   else
     ajFmtPrint("Signature compiled ok\n");    
   
   
   /* SEARCH DATABASE AND GET SCORES FOR EACH SEQUENCE IN THE DATABASE */
   for(cnt = 0; cnt<db->Size;cnt++){
     seq = ajSeqNew();
     
     ajStrAssS(&seq->Name,db->Seq[cnt]->Name);
     ajStrAssS(&seq->Acc,db->Seq[cnt]->Name);
     ajStrAssS(&seq->Seq,db->Seq[cnt]->Seq);
     
     /* Allocate memory for hit */
     hit = embHitNew();
     
     if(!embSignatureAlignSeq(sig, seq, &hit, ntopt)){       
       embHitDel(&hit);
       ajSeqDel(&seq);
       continue;
     }
     
     /* CORRECTION */ 
     ajSeqDel(&seq);      
     ajStrAssS(&hit->Model,model);
     ajListPush(listhits,(AjPHit) hit);
   }
   
   /* CONSTRUCT THE HITLIST */
   hitlist = embHitlistNew(0);
   
   /* write hit structure */
   ajStrAssS(&hitlist->Class,class);
   ajStrAssS(&hitlist->Fold,fold);
   ajStrAssS(&hitlist->Superfamily,superfamily);
   ajStrAssS(&hitlist->Family,family);
   ajStrAssS(&hitlist->Model,model);
   hitlist->Sunid_Family = sun_id;
   
   /* Convert list to array within Hitlist object */
   nhits=ajListToArray(listhits,(void ***)&hitlist->hits);
   hitlist->N = nhits;
   
   /* convert the hitlist to scophits so the related hits can be targetted for removal
      in order to calculate an unbiased distribution */
   ajListPushApp(tmplist,hitlist);
   embDmxHitlistToScophits(tmplist,&scophits);
   
   
   /* write the scophits to file */
   ajDmxScophitsWriteFasta(outf,scophits);   
   
   /* Set N to zero so that the nodes in the list are not freed,
      the structure itself will be freed though */
   hitlist->N=0;
   embHitlistDel(&hitlist);
   ajListDel(&tmplist);
   
   
   /* clean up scophits */
   iter = ajListIterRead(scophits);
   while((scop = (AjPScophit)ajListIterNext(iter)))
     ajDmxScophitDel(&scop);
   ajListDel(&scophits);
   ajListIterFree(&iter);
   
   
   /* Set N to zero so that the nodes in the list are not freed, 
      the structure itself will be freed though */
   hitlist->N=0;
   embHitlistDel(&hitlist);
   ajListDel(&tmplist);	
   
   /* Clean up listhits nodes */
   iter = ajListIterRead(listhits);
   while((hit = (AjPHit)ajListIterNext(iter)))
     embHitDel(&hit);	
   ajListDel(&listhits);
   ajListIterFree(&iter); 
   
    /* Clean up hits array */
   AJFREE(hits);
   
   embSignatureDel(&sig);
   ajFileClose(&sigin);
   
   return ajTrue;   
}


/* @funcstatic libscan_SignatureLibScan ***************************************
**
** Scan a protein sequence against a library of Signature models. The 
** models represents SCOP portein domain families. 
**
** @param [r] db       [AjPSeqset]  Name of sequence file. 
** @param [r] path     [AjPStr]     Directory of profile models.
** @param [r] extn     [AjPStr]     File extension of models.
** @param [r] scoplist [AjPList]    List of scop objects.
**
** @return [AjPList] retuns familyhits.
** @@
** 
******************************************************************************/
static AjBool libscan_SignatureLibScan(AjPSeq seq, AjPStr path, AjPStr extn,
				       float gapopen, float gapextn, 
				       AjPMatrixf sub,  ajint ntopt,
				       AjPList scoplist, AjPStr model, 
				       AjPStr sigoutpath, AjPStr sigoutextn)
{
    AjPSignature sig   = NULL;		/* Signature data structure*/

    AjPStr sigfile     = NULL;		/* name of seqret output file */
    AjPStr tmpname     = NULL;
    AjPStr outfile     = NULL;
    
    AjPFile sigin      = NULL;
    AjPFile sigoutf    = NULL;
    
    AjPScophit hit     = NULL;
    AjPScophit* hits   = NULL;
    AjPScophit scophit = NULL;
    
    AjPHit rawhit      = NULL;		/* Scophit structure */
    
    ajint nhits        = 0;

    ajint i            = 0;

    float zvalue       = 0.0;
    float mean         = 0.0;
    float sdv          = 0.0;

    AjPList familyhits = NULL;		/* a list of hits */
    AjPList modelnames = NULL;		/* A list of model names */
    AjPList pointlist  = NULL;
    AjPList tmplist    = NULL;

    AjIList iter       = NULL;

    tmpname     = ajStrNew();
    outfile     = ajStrNew();
    

    if(!path || !seq || !extn || !scoplist){
      ajWarn("Bad arguments passed to libscan_SignatureLibScan\n");
      return ajFalse;
    }
    
    /* get the models */
    modelnames = libscan_GetLibrary(path,extn);
    
    tmplist     = ajListNew();

    ajStrAssS(&outfile,seq->Name);
    ajStrApp(&outfile,sigoutextn);
    
    iter = ajListIterRead(modelnames);
    while((sigfile=(AjPStr)ajListIterNext(iter))){
      sigin  = ajFileNewIn(sigfile);
      
      /* READ SIGNATURE FILE */
      if(!(sig=embSignatureReadNew(sigin))){       
	ajMatrixfDel(&sub);
	ajFileClose(&sigin);
	return ajFalse;
	ajFatal("Error reading signature file");
      }        
      else
	ajFmtPrint("Signature file read ok\n");
      
      
      /* COMPILE SIGNATURE */
      if(!embSignatureCompile(&sig, gapopen, gapextn, sub)){     
	embSignatureDel(&sig);
	ajMatrixfDel(&sub);
	ajFileClose(&sigin);
	return ajFalse;
	ajFatal("Error compiling signature");
      }       
      else{
	if(!sig)
	  continue;
	ajFmtPrint("Signature compiled ok\n");    
      }
      
      
      /* Allocate memory for hit */
      rawhit = embHitNew();
      
      if(!embSignatureAlignSeq(sig, seq, &rawhit, ntopt)){       
	embHitDel(&rawhit);
	/*CORRECTION*/            
	ajFileClose(&sigin);
	/*CORRECTION*/            
	embSignatureDel(&sig);
	continue;
      }
      
      /* create a hit */
      hit = ajDmxScophitNew();
      
      ajStrAssS(&hit->Class,sig->Class);
      ajStrAssS(&hit->Fold,sig->Fold);
      ajStrAssS(&hit->Superfamily,sig->Superfamily);
      ajStrAssS(&hit->Family,sig->Family);
      ajStrAssS(&hit->Acc,rawhit->Acc);
      ajStrAssS(&hit->Seq,rawhit->Seq);
      hit->Sunid_Family = sig->Sunid_Family;
      ajStrAssS(&hit->Model,model);
      hit->Score = rawhit->Score;
      hit->Start = rawhit->Start;
      hit->End   = rawhit->End;
      
      ajListPushApp(tmplist,(AjPScophit) hit);

      embHitDel(&rawhit);
      embSignatureDel(&sig);
      ajFileClose(&sigin); 
      
    }
    ajListIterFree(&iter);
    
    
    if(ajListLength(pointlist)!=0){
      familyhits  = ajListNew();
      
      /* convert the original list of hits to an array for convenience */ 
      nhits = ajListToArray(tmplist,(void ***)&hits);
      
      /* calculate the mean and standard deviation from tmplist*/
      mean = libscan_CalcMean(tmplist);
      sdv  = libscan_CalcSdv(tmplist,mean);

      sigoutf = ajFileNewOutD(sigoutpath,outfile);
      
      /* DO A SECOND PARSE THROUGH THE HITS FILE AND CALCULATE THE
	 Z-VALUES */
      for(i=0;i<nhits;i++){
	scophit = ajDmxScophitNew();

	zvalue = libscan_ScoreToZscore(hits[i]->Score,mean,sdv);
	hits[i]->Pval = zvalue;

	ajDmxScophitCopy(&scophit,hits[i]);
	ajListPushApp(familyhits,scophit);
      }
      
      /*sort the list before printing */
      ajListSort(familyhits,ajDmxScophitCompPval);
      
      /* write out the familyhits list */
      ajDmxScophitsWriteFasta(sigoutf,familyhits);
      ajFileClose(&sigoutf);
      
      AJFREE(hits);
    }
    
    /* delete and clean up tmplist */
    iter=ajListIterRead(tmplist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
      ajDmxScophitDel(&hit);
    ajListIterFree(&iter);
    ajListDel(&tmplist);

    /* delete and clean up modelnames */
    iter=ajListIterRead(modelnames);
    while((tmpname=(AjPStr)ajListIterNext(iter)))
      ajStrDel(&tmpname);
    ajListIterFree(&iter);
    ajListDel(&modelnames); 
    
    ajStrDel(&outfile);
    
    return ajTrue;    
}



/* @funcstatic libscan_RunHmmsearch ********************************************
**
 ** Runs hmmsearch at the command line and writes the results to a specified
 ** file.
 **
 
 ** @param [r] filename  [AjPStr]  Name of file containing the hmm.
 ** @paran [r] db        [AjPStr]  The database to be searched.
 ** @param [r] outfname  [AjPStr]  Name of output file for search results.
 **
 ** @return [AjBool] Returns true on completion.
 ** @@
 ** 
 ******************************************************************************/
AjBool libscan_RunHmmsearch(AjPStr filename, AjPStr db, AjPStr outfname)
{
    AjPStr cmd  = NULL;                 /* the command line to be executed */
    
    if(!filename || !outfname)
    {
        ajWarn("Bad args passed to libscan_RunHmmsearch\n");
        return ajFalse;
    }

    cmd = ajStrNew();

    /* construct command line for hmmsearch */
    ajFmtPrintS(&cmd,"/usr/local/hmm211/bin/hmmsearch %S %S > %S",filename,db,outfname);

    /* execute the command line */
    system(ajStrStr(cmd));
  
    /* clean up */
    ajStrDel(&cmd);
  
    return ajTrue;
  
}


/* @funcstatic libscan_RunProphet **********************************************
 **
 ** Runs prophet at the command line and writes the results to a specified
 ** file.
 **
 ** @param [r] profile   [AjPStr]  Name of file containing the profile.
 ** @paran [r] db        [AjPStr]  The database to be searched.
 ** @param [r] gapopen   [ajint]   Gap open panelty 
 ** @param [r] gapextend [ajint]   Gap extension penalty
 ** @param [r] outfname  [AjPStr]  Name of output file for search results.
 **
 ** @return [AjBool] Returns true on completion.
 ** @@
 ** 
 ******************************************************************************/
AjBool libscan_RunProphet(AjPStr profile, AjPStr db, float gapopen, float gapextend, 
                       AjPStr outfname)
{
    AjPStr cmd  = NULL;                 /* the command line to be executed */

    if(!profile || !outfname)
    {
        ajWarn("Bad args passed to libscan_RunHmmsearch\n");
        return ajFalse;
    }

    cmd = ajStrNew();

    /* construct command line for hmmsearch */
    ajFmtPrintS(&cmd,"/m1/rranasin/emboss/emboss/emboss/prophet -sequence %S -infile %S -gapopen %1f -gapextend %1f -outfile %S",
                db,profile,gapopen,gapextend,outfname);

    /* execute the command line */
    system(ajStrStr(cmd));

    /* clean up */
    ajStrDel(&cmd);

    return ajTrue;
  
}


/* @funcstatic libscan_GetLibrary *********************************************
**
** Given the directory where the models exist and a file extension of the 
** models, the routine returns a list of file names matching the user defined 
** file extension.
**
** @param [r]  path [AjPStr] Directory of models. 
** @param [r]  extn [AjPStr] File extension.
**
** @return [AjPList] a list of file names.
** @@
** 
******************************************************************************/
static AjPList libscan_GetLibrary(AjPStr path, AjPStr extn)
{
    AjPList list = NULL;                /* a list of filenames */
    
    AjPStr tmp   = NULL;
    
    
    list = ajListNew();
    
    /* Create list of files in the path */
    ajStrAssC(&tmp, "*");               /* assign a wildcard to tmp */

    if((ajStrChar(extn, 0)=='.')) /* checks if the file extension
				     starts with "." */
        ajStrApp(&tmp, extn);           /* assign the acd input file
  					   extension to tmp */
  
    /* this picks up situations where the user has specified an
       extension without a "." */
    else
    {
        ajStrAppC(&tmp, ".");           /* assign "." to tmp */  
        ajStrApp(&tmp, extn);           /* append tmp with a user
					   specified extension */
    }   
  

    /* all files containing hidden markov models will be in a list */
    ajFileScan(path, tmp, &list, ajFalse, ajFalse, NULL, NULL, ajFalse, NULL);
    
    ajStrDel(&tmp);

    return list;
}


/* @funcstatic libscan_CalcDistribution ***************************************
**
** Calculate the distribution from a list of hits.
**
** @param [r] listhits   [AjPList] List is hits 
** @param [r] fold        [AjPStr ] SCOP fold
** @param [r] superfamily [AjPStr ] SCOP superfamily
** @param [r] famly       [AjPStr ] SCOP family
** @param [r] sun_id      [ajint  ] SCOP family sunid
**
** @return [AjPList] A list of coordinates in a distribution.
** @@
******************************************************************************/
static AjPList libscan_CalcDistribution(AjPList scophits, AjPStr class,
					AjPStr fold, AjPStr superfamily,
					AjPStr family, ajint sun_id)
{
    AjPList pointlist = NULL;
    
    ajint i           = 0;
    ajint nhits       = 0;
    ajint high        = 0;
    ajint low         = 0;
    ajint freq        = 0;
    ajint interval    = 0;              /* the score interva, x-axis */

    LibscanPCoord  point   = NULL; /* a Datapoint object to hold
				      individual coordinates */

    AjPScophit* hits  = NULL;
    
    if(!scophits)
        ajFatal("No list of hits passed to libscan_CalcDistribution\n");
    

    pointlist = ajListNew();

    /* but only remove them inside the libscan_CalcDistribution sub-routine */
/*CORRECTION*/    ajListGarbageCollect(scophits, ajDmxScophitDelWrap,
                         (int(*)(const void*)) ajDmxScophitCheckTarget);

    /* sort the list according to score */
    ajListSort(scophits,ajDmxScophitCompScore);

    /* convert list to array for convenience */
    nhits = ajListToArray(scophits,(void ***)&hits);

    if(!nhits)
	ajFatal("No hits in libscan_CalcDistribution!");
    
    low  = (ajint)hits[0]->Score;

    high = (ajint)hits[nhits-1]->Score;
    freq = 0;
    
    /* only for testing purposes
       printf("1 = %f 2 = %f 3 = %f 4 = %f 5= %f 6= %f 7= %f\n",hits[0]->Score,
       hits[1]->Score,hits[2]->Score,hits[3]->Score,
       hits[4]->Score,hits[5]->Score,hits[6]->Score); */
  
    /* set the starting point */
    if(low > 0)
        interval = (low+1);
    else if(low < 0)
        interval = (low-1);
    
    for(i=0; i<nhits; i++)
    { 
        if(hits[i]->Score < (float)interval)
        {
            freq++;
       
            if(interval == (high+1))
            {
                point = libscan_CoordNew();
         
		ajStrAssS(&point->Class,class);
                ajStrAssS(&point->Fold,fold);
                ajStrAssS(&point->Superfamily,superfamily);
                ajStrAssS(&point->Family,family);
                point->Sunid_Family = sun_id;
                ajStrAssS(&point->Acc,hits[i]->Acc);
                point->x = interval;
                point->y = freq;
		
                ajListPushApp(pointlist,point);
            }
        }
     
        else if(hits[i]->Score > (float)interval)
        { 
            while((float)interval <= hits[i]->Score)
            {           
                point = libscan_CoordNew();
         
		ajStrAssS(&point->Class,class);
                ajStrAssS(&point->Fold,fold);
                ajStrAssS(&point->Superfamily,superfamily);
                ajStrAssS(&point->Family,family);
                point->Sunid_Family = sun_id;
                ajStrAssS(&point->Acc,hits[i]->Acc);
                point->x = interval;
                point->y = freq;
                
		ajListPushApp(pointlist,point);
                interval++;
        
            }

            freq++;
        }
    }


    AJFREE(hits);

    return pointlist;
}


/* @funcstatic libscan_RunHmmerInModeOne **************************************
**
** Scan database with hidden markov model. 
**
** @param [r] hit1
** @param [r] hit2
**
** @return [AjBool]
** @@
******************************************************************************/
static AjBool libscan_RunHmmerInModeOne(AjPSeqset db, AjPStr hmmpath,
					AjPStr hmmextn, AjPStr *mode,
					AjPStr model,AjPList scoplist,
					AjPStr hmmoutpath, AjPStr hmmoutextn)
{
    AjPList hmmlist    = NULL;		/* a list of hmm file names */
    
    AjPStr hmmfile     = NULL;
    AjPStr family      = NULL;
    AjPStr superfamily = NULL;
    AjPStr fold        = NULL;
    AjPStr class       = NULL;
    AjPStr sunid       = NULL;
    AjPStr hmmsearch   = NULL;
    
    AjPFile hmmoutf    = NULL;

    ajint posdash;
    ajint posdot;       
    ajint sun_id;    

    sunid       = ajStrNew();
    hmmsearch   = ajStrNew();
    family      = ajStrNew();
    superfamily = ajStrNew();
    fold        = ajStrNew();
    class       = ajStrNew();

    if(ajStrChar(*mode,0)=='1')
    {   
        hmmlist = libscan_GetLibrary(hmmpath,hmmextn);
        
        /*Start of main application loop*/   
        while(ajListPop(hmmlist,(void **)&hmmfile))
        {
            /* get the "seed" bit of filenames from the hmmfile */
            posdash = ajStrRFindC(hmmfile, "/");
            posdot  = ajStrRFindC(hmmfile, ".");
        
	    
            if(posdash >= posdot)
                ajFatal("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");
	    
            else
            {
                /* get the Family_Sunid */
                ajStrAssSub(&sunid, hmmfile, posdash+1, posdot-1);
		
                /* CREAT OUTPUT FILE NAMES */
                ajStrAssS(&hmmsearch,sunid);
                ajStrApp(&hmmsearch, hmmoutextn);
		
		
                /* CREAT OUTPUT STREAMS */
                hmmoutf = ajFileNewOutD(hmmoutpath,hmmsearch);
            
                ajStrToInt(sunid,&sun_id);
            
                /* do a binary search on scoplist to find the relavent entry */
                if(!(libscan_SunidToScopInfo(sun_id,&family,&superfamily,
					     &fold, &class, scoplist)))
                    ajFatal("libscan_SunidToScopInfo failed in "
			    "libscan_RunHmmerInModeOne. "
			    "email rranasin@hgmp.mrc.ac.uk\n");
            

                /* READ SCOP FAMILIES FILE & FILL THE LIST OF TARGETS*/
		
                /* search with HMM */
                libscan_HmmSearch(db,hmmfile, family,superfamily,fold,class,
				  sun_id,model,hmmoutf);
                ajFileClose(&hmmoutf);
		ajStrDel(&hmmfile);
	    }
        }
	
	ajListDel(&hmmlist);
	
	
	ajStrDel(&sunid);
	ajStrDel(&hmmsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold);   
     /* CORRECTION */ 	ajStrDel(&class);        
	return ajTrue;
    } 
    
    else
    {
	ajStrDel(&hmmfile);
	ajStrDel(&sunid);
	ajStrDel(&hmmsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold);
	ajStrDel(&class);
       
        return ajFalse;
    }
}       


/* @funcstatic libscan_RunHmmerInModeTwo **************************************
**
** Scan sequences with hidden markov model. 
**
** @param [r] 
** @param [r] 
**
** @return [AjBool] 
** @@
******************************************************************************/

static AjBool libscan_RunHmmerInModeTwo(AjPSeqset db, AjPStr hmmpath,
					AjPStr hmmextn, AjPStr *mode, AjPList
					scoplist, AjPStr model, AjPStr
					hmmoutpath, AjPStr hmmoutextn)
{
    AjPSeq seq = NULL;
    ajint i;
						   
    if(ajStrChar(*mode,0)=='2') 
    {
        for(i=0;i<db->Size;i++)
        {
            seq = ajSeqNew();
            ajStrAssS(&seq->Name,db->Seq[i]->Name);
            ajStrAssS(&seq->Seq,db->Seq[i]->Seq);
            
            /* search a library of hidden markov models */
            libscan_HmmLibScan(seq,hmmpath,hmmextn,scoplist,model,
			       hmmoutpath,hmmoutextn);
            
            ajSeqDel(&seq);
            
        }
       
        return ajTrue;
    }

    else
        return ajFalse;
}


/* @funcstatic libscan_RunProphetInModeOne ************************************
**
** Scan database with profile model. 
**
** @param [r] hit1
** @param [r] hit2
**
** @return [AjBool]
** @@
******************************************************************************/
static AjBool libscan_RunProphetInModeOne(AjPSeqset db, AjPStr profpath,
					  AjPStr profextn, float gapo,
					  float gape, AjPStr *mode,
					  AjPStr model,AjPList scoplist,
					  AjPStr profoutpath,AjPStr profoutextn)
{
    AjPList proflist    = NULL;		/* a list of hmm file names */
    
    AjPStr profile      = NULL;
    AjPStr family       = NULL;
    AjPStr superfamily  = NULL;
    AjPStr fold         = NULL;
    AjPStr class        = NULL;
 
    AjPStr sunid        = NULL;
    AjPStr profsearch   = NULL;
    
    AjPFile profoutf    = NULL;
    
    ajint posdash;
    ajint posdot;       
    
    ajint sun_id;
    
    
    sunid       = ajStrNew();
    profsearch  = ajStrNew();
    family      = ajStrNew();
    superfamily = ajStrNew();    
    fold        = ajStrNew();
    class       = ajStrNew();

    if(ajStrChar(*mode,0)=='1')
    {
        proflist = libscan_GetLibrary(profpath,profextn);
      
        /*Start of main application loop*/   
        while(ajListPop(proflist,(void **)&profile))
        {
            /* get the "seed" bit of filenames from the gbvfile */
            posdash = ajStrRFindC(profile, "/");
            posdot  = ajStrRFindC(profile, ".");
            
            if(posdash >= posdot)
                ajFatal("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");

            else
            {
                /* Add a '.' to outextn if one does not already exist
                   if((ajStrChar(outextn, 0)!='.'))       
                   ajStrInsertC(&outextn, 0, ".");*/

                /* get the Family_Sunid */
                ajStrAssSub(&sunid, profile, posdash+1, posdot-1);
         
                /* CREAT OUTPUT FILE NAMES */
                ajStrAssS(&profsearch,sunid);
                ajStrApp(&profsearch, profoutextn);
                                      
                /* CREAT OUTPUT STREAMS */
                profoutf = ajFileNewOutD(profoutpath,profsearch);
                 
                ajStrToInt(sunid,&sun_id);
                 
                /* do a binary search on scoplist to find the relavent entry */
                if(!(libscan_SunidToScopInfo(sun_id,&family,&superfamily,
					     &fold, &class, scoplist)))
                    ajFatal("libscan_SunidToScopInfo failed in "
			    "libscan_RunProphetInModeOne. "
			    "email rranasin@hgmp.mrc.ac.uk\n");
                                  
                /* search with Gribskov Profile */
                libscan_ProfileSearch(db,profile,gapo,gape,family,superfamily,fold,class,
				      sun_id,model,profoutf);
                ajFileClose(&profoutf);
	    }   
	    ajStrDel(&profile);
	}

	ajListDel(&proflist);
	ajStrDel(&sunid);
	ajStrDel(&profsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold); 
	ajStrDel(&class); 
        return ajTrue;
    }
    
    else
    {
	ajStrDel(&profile);
	ajStrDel(&sunid);
	ajStrDel(&profsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold); 
	ajStrDel(&class);
	
        return ajFalse;
    }   
}



/* @funcstatic libscan_RunProphetInModeTwo ************************************
**
** Scan sequences with hidden markov model. 
**
** @param [r] 
** @param [r] 
**
** @return [AjBool] 
** @@
******************************************************************************/
static AjBool libscan_RunProphetInModeTwo(AjPSeqset db, AjPStr profpath,
					  AjPStr profextn, float gapo,
					  float gape, AjPStr *mode,
					  AjPList scoplist, AjPStr model,
					  AjPStr profoutpath, AjPStr profoutextn)
{
    AjPSeq seq   = NULL;

    ajint i;

    if(ajStrChar(*mode,0)=='2') 
    {
        for(i=0;i<db->Size;i++)
        {
            seq = ajSeqNew();
            ajStrAssS(&seq->Name,db->Seq[i]->Name);
            ajStrAssS(&seq->Seq,db->Seq[i]->Seq);
            
            
            /* search a library of gribskov profiles */
            libscan_ProfileLibScan(seq,profpath,profextn,gapo,gape,
				   scoplist,model,profoutpath,profoutextn);
            
            ajSeqDel(&seq);
            
        }
        return ajTrue;
    }

    else
        return ajFalse;
    
}


/* @funcstatic libscan_RunSignatureInModeOne **********************************
**
** Scan database with signatures. 
**
** @param [r] hit1
** @param [r] hit2
**
** @return [AjBool]
** @@
******************************************************************************/
static AjBool libscan_RunSignatureInModeOne(AjPSeqset db, AjPStr sigpath,
					    AjPStr sigextn, float gapo, 
					    float gape, AjPMatrixf sub,
					    ajint ntopt, AjPStr *mode, 
					    AjPStr model,AjPList scoplist, 
					    AjPStr sigoutpath,AjPStr sigoutextn)
{
    AjPList siglist     = NULL;		/* a list of hmm file names */
    
    AjPStr sigfile      = NULL;
    AjPStr family       = NULL;
    AjPStr superfamily  = NULL;
    AjPStr fold         = NULL;
    AjPStr class        = NULL;
    
    AjPStr sunid        = NULL;
    AjPStr sigsearch    = NULL;
    
    AjPFile sigoutf     = NULL;
    
    ajint posdash;
    ajint posdot;       
    ajint sun_id;
    
    sunid       = ajStrNew();
    sigsearch   = ajStrNew();
    family      = ajStrNew();
    superfamily = ajStrNew();
    fold        = ajStrNew();
    class       = ajStrNew();
    
    if(ajStrChar(*mode,0)=='1')
    {
        siglist = libscan_GetLibrary(sigpath,sigextn);

      
        /*Start of main application loop*/   
        while(ajListPop(siglist,(void **)&sigfile))
        {
            /* get the "seed" bit of filenames from the gbvfile */
            posdash = ajStrRFindC(sigfile, "/");
            posdot  = ajStrRFindC(sigfile, ".");
            
            if(posdash >= posdot)
                ajFatal("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");

            else
            {
                /* Add a '.' to outextn if one does not already exist
                   if((ajStrChar(outextn, 0)!='.'))       
                   ajStrInsertC(&outextn, 0, ".");*/

                /* get the Family_Sunid */
                ajStrAssSub(&sunid, sigfile, posdash+1, posdot-1);
         
                /* CREAT OUTPUT FILE NAMES */
                ajStrAssS(&sigsearch,sunid);

                ajStrApp(&sigsearch, sigoutextn);
                                      
                /* CREAT OUTPUT STREAMS */
                sigoutf = ajFileNewOutD(sigoutpath,sigsearch);

                 
                ajStrToInt(sunid,&sun_id);
                 
                /* do a binary search on scoplist to find the relavent entry */
                if(!(libscan_SunidToScopInfo(sun_id,&family,&superfamily,
					     &fold,&class,scoplist)))
                    ajFatal("libscan_SunidToScopInfo failed. "
			    "email rranasin@hgmp.mrc.ac.uk\n");
                 
		/* search with Signature */
                libscan_SignatureSearch(db,sigfile,sub,gapo,gape,ntopt,
					family,superfamily,fold,class,
					sun_id,model,sigoutf);
                ajFileClose(&sigoutf);
            }   
            ajStrDel(&sigfile);
        }
	ajListDel(&siglist);
	ajStrDel(&sunid);
	ajStrDel(&sigsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold);
	ajStrDel(&class);
        return ajTrue;
    }
    
    else
    {
	ajStrDel(&sigfile);
	ajStrDel(&sunid);
	ajStrDel(&sigsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold); 
	ajStrDel(&class);
	
	return ajFalse;
    }
}


/* @funcstatic libscan_RunSignatureInModeTwo **********************************
**
** Scan sequences library of sigantures. 
**
** @param [r] 
** @param [r] 
**
** @return [AjBool] 
** @@
******************************************************************************/
static AjBool libscan_RunSignatureInModeTwo(AjPSeqset db, AjPStr sigpath,
					    AjPStr sigextn, 
					    float gapo,float gape,
					    AjPMatrixf sub, ajint ntopt, 
					    AjPStr *mode, AjPList scoplist,
					    AjPStr model, AjPStr sigoutpath, 
					    AjPStr sigoutextn)
{
        
    AjPSeq seq   = NULL;
    ajint i;

    if(ajStrChar(*mode,0)=='2') 
    {
	 for(i=0;i<db->Size;i++)
	 {
	     seq = ajSeqNew();
	     ajStrAssS(&seq->Name,db->Seq[i]->Name);
	     ajStrAssS(&seq->Seq,db->Seq[i]->Seq);            
            
	     ajSeqDel(&seq);
	 }

        for(i=0;i<db->Size;i++)
        {
            seq = ajSeqNew();
            ajStrAssS(&seq->Name,db->Seq[i]->Name);
            ajStrAssS(&seq->Seq,db->Seq[i]->Seq);            

            /* search a library of signature */
            libscan_SignatureLibScan(seq,sigpath,sigextn,gapo,gape,
				     sub,ntopt,scoplist,model,sigoutpath,
				     sigoutextn);
            
            ajSeqDel(&seq);
        }
        return ajTrue;
    }

    else
        return ajFalse;
    
}







/* @funcstatic libscan_CalcLibscanDistribution ********************************
**
** Calculate the distribution from a list of hits.
**
** @param [r] listhits   [AjPList] List is hits 
** @param [r] fold        [AjPStr ] SCOP fold
** @param [r] superfamily [AjPStr ] SCOP superfamily
** @param [r] famly       [AjPStr ] SCOP family
** @param [r] sun_id      [ajint  ] SCOP family sunid
**
** @return [AjPList] A list of coordinates in a distribution.
** @@
******************************************************************************/
static AjPList libscan_CalcLibscanDistribution(AjPList scophits)
{
    AjPList pointlist = NULL;
    
    ajint i           = 0;
    ajint nhits       = 0;
    ajint high        = 0;
    ajint low         = 0;
    ajint freq        = 0;
    ajint interval    = 0;              /* the score interva, x-axis */

    LibscanPCoord  point   = NULL;	/* a Datapoint object to hold
					   individual coordinates */

    AjPScophit* hits  = NULL;
    
    if(!scophits)
        ajFatal("No list of hits passed to libscan_CalcDistribution\n");
    
    if(ajListLength(scophits)==0)
	return NULL;
    
    pointlist = ajListNew();

    /* but only remove them inside the libscan_CalcDistribution sub-routine */
    ajListGarbageCollect(scophits, ajDmxScophitDelWrap,
                         (int(*)(const void*)) ajDmxScophitCheckTarget);

    /* sort the list according to score */
    ajListSort(scophits,ajDmxScophitCompScore);

    /* convert list to array for convenience */
    nhits = ajListToArray(scophits,(void ***)&hits);

    if(!nhits)
	ajFatal("No hits in libscan_CalcDistribution!");
    
    low  = (ajint)hits[0]->Score;
    
    high = (ajint)hits[nhits-1]->Score;
    freq = 0;
    
    /* set the starting point */
    if(low > 0)
        interval = (low+1);
    
    else if(low < 0)
        interval = (low-1);
    
    for(i=0; i<nhits; i++)
    { 
	/* check if the array only contains only one hit */
	if((nhits)==1)
	{
	    point = libscan_CoordNew();
	    ajStrAssS(&point->Acc,hits[i]->Acc);
	    point->x = interval;
	    point->y = 1;

	    ajListPushApp(pointlist,point);
	    break;

	}		

        if(hits[i]->Score < (float)interval)
        {
            freq++;
	    
	    /* check to see if end of the array has been reached */
            if(interval == (ajint)hits[nhits-1]->Score)
            {
                point = libscan_CoordNew();
         
                ajStrAssS(&point->Acc,hits[i]->Acc);
                point->x = interval;
                point->y = freq;

                ajListPushApp(pointlist,point);
            }
        }
     
        else if(hits[i]->Score > (float)interval)
        { 
            while((float)interval <= hits[i]->Score)
            {           
                point = libscan_CoordNew();
         
                ajStrAssS(&point->Acc,hits[i]->Acc);
                point->x = interval;
                point->y = freq;
                
		ajListPushApp(pointlist,point);
                interval++;
        
            }

            freq++;
        }
    }

    AJFREE(hits);

    return pointlist;
}




/* @funcstatic libscan_CoordNew ***********************************************
**
** Coord object constructor. This is normally called by the
** ajDmxDiscordToCoords function.
**
** 
** @return [LibscanPCoord] Pointer to a Coord object
** @@
******************************************************************************/

static LibscanPCoord libscan_CoordNew(void)
{
    LibscanPCoord ret = NULL;

    AJNEW0(ret);
    ret->Class       = ajStrNew();
    ret->Fold        = ajStrNew();
    ret->Superfamily = ajStrNew();
    ret->Family      = ajStrNew();
    ret->Model_Type  = ajStrNew();
    ret->Acc         = ajStrNew();
    ret->Spr         = ajStrNew();
    ret->x           = 0.0;
    ret->y           = 0.0;

    return ret;
}




/* @funcstatic libscan_CoordDel ***********************************************
**
** Destructor for Coord object.
**
** @param [w] pthis [LibscanPCoord*] Coord object pointer
**
** @return [void]
** @@
******************************************************************************/

static void libscan_CoordDel(LibscanPCoord *pthis)
{
    ajStrDel(&(*pthis)->Class);
    ajStrDel(&(*pthis)->Fold);
    ajStrDel(&(*pthis)->Superfamily);
    ajStrDel(&(*pthis)->Family);
    ajStrDel(&(*pthis)->Acc);
    ajStrDel(&(*pthis)->Spr);
 
    AJFREE(*pthis);
    *pthis = NULL;
    
    return;
}






/* @funcstatic libscan_ScoreToPvalue ******************************************
**
** Read a score  and calculates the p-value. Relies on a list 
** of Coord objects sorted by score.
** 
** @param [r] score  [float]    The sunid_family 
** @param [r] list   [AjPList]  Sorted list of Coord objects
**
** @return [float] a p-value calculated from a list of scores.
** @@
*****************************************************************************/

static float libscan_ScoreToPvalue(float score, AjPList list)
{
    LibscanPCoord *arr = NULL;       /* array derived from list */
    ajint dim = 0;              /* size of the array */
    ajint idx = 0;              /* index into the array for the score */
    float p_value = 0.0;
    
    if(!score || !list)
        ajWarn("Bad args passed to libscan_ScoreToPvalue");
    
    dim = ajListToArray(list,(void***)&(arr));
    
    if(!dim)
        ajWarn("Empty list passed to libscan_ScoreToPvalue");
    
    if((idx = libscan_CoordBinSearchScore(score, arr, dim))==-1)
    {
      AJFREE(arr);
      ajFatal("libscan_ScoreToPvalue fatal error.");

      return p_value;
    }
    
    p_value = ((1-(float)arr[idx]->y/(float)arr[dim-1]->y));  
    AJFREE(arr);
    
    return p_value;
}





/* @funcstatic libscan_SunidToScopInfo ****************************************
**
** Read a sunid  and writes the relavent scop infomation for it. Relies on 
** a list of scop objects sorted by Sunid_Family code.
** 
** @param [r] sunid  [ajint]   The sunid_family 
** @param [w] family [AjPStr*]  Family 
** @param [w] superfamily [AjPStr*]  Superfamily
** @param [w] fold [AjPStr*]  Fold
** @param [w] klass [AjPStr*]  Class
** @param [r] list   [AjPList]  Sorted list of scop objects
**
** @return [AjBool]  True if a swissprot identifier code was found for the
**                    Scop code.
** @@
******************************************************************************/
static AjBool libscan_SunidToScopInfo (ajint sunid, AjPStr *family,
				       AjPStr *superfamily, AjPStr *fold,
				       AjPStr *klass, AjPList list)
{
    const AjPScop *arr = NULL;             /* array derived from list */
    ajint dim    = 0;                /* size of the array */
    ajint idx    = 0; /* index into the array for the Sunid_family */

    AjPStr *class = NULL;

    class = klass;
    
    if(!sunid || !list)
    {
        ajWarn("Bad args passed to libscan_SunidToScopInfo");
        return ajFalse;
    }
    
    dim = ajListToArray(list,(void***)&(arr));
    
    if(!dim)
    {
        ajWarn("Empty list passed to libscan_SunidToScopInfo");
        return ajFalse;
    }

    if( (idx = ajScopArrFindSunid(arr, dim, sunid))==-1)
    {
        AJFREE(arr);
        return ajFalse;
    }   
    
    ajStrAssS(family, arr[idx]->Family);
    ajStrAssS(superfamily, arr[idx]->Superfamily);
    ajStrAssS(fold, arr[idx]->Fold);
    ajStrAssS(class, arr[idx]->Class);
    
    AJFREE(arr);

    return ajTrue;
}





/* @funcstatic libscan_CoordBinSearchScore ************************************
**
** Performs a binary search for a given score over an array of score generated
** from a model on a set of randomly generated protein sequences. 
**
** @param [r] score [float]           Search score
** @param [r] arr [LibscanPCoord*]    Array of LibscanPCoord objects
** @param [r] siz [ajint]            Size of array
**
** @return [ajint] Index of first AjPScop object found with an PDB code
** matching id, or -1 if id is not found.
** @@
******************************************************************************/

static ajint libscan_CoordBinSearchScore(float score, LibscanPCoord *arr,
					 ajint siz)
{
    int l;
    int m;
    int h;
  
    l = 0;
    h = siz-1;
  
    while(l<=h)
    {
        m = (l+h)>>1;
    
        if( (m==0) && (score < arr[m]->x))
          return m;

        else if( (m<siz-1) &&  (score >= arr[m]->x) && (score < arr[m+1]->x))
          return m;

        else if( (m==siz-1) &&  (score >= arr[m]->x))
          return m;

        else if(score < arr[m]->x)
          h=m-1;

        else if(score > arr[m]->x)
          l=m+1;
    }

    return -1;
}


/* @func libscan_CalcMean ***********************************************
 **
 ** Calculate the mean score from a list of hits.
 **
 ** @param [r] scophits [AjPList] List of scophits
 **
 ** @return [float] the mean score.

 ** @@
 *********************************************************************/
static float libscan_CalcMean(AjPList scophits)
{
  ajint i           = 0;
  ajint nhits       = 0;
  
  float sum         = 0.0;
  float mean        = 0.0;
  
  AjPScophit* hits  = NULL;
  
  if(!scophits)
    ajFatal("No list of hits passed to libscan_CalcMean\n");
  
  if(ajListLength(scophits)==0)
    return ajFalse;
  
  /* convert list to array for convenience */
  nhits = ajListToArray(scophits,(void ***)&hits);
  
  if(!nhits)
    ajFatal("No hits in libscan_CalcMean!");
  
  /* get the sum of all scores */
  for(i=0; i<nhits; i++){
    sum += hits[i]->Score;
  }
  
  /* calculate the mean */
  mean = sum/nhits;
  
  AJFREE(hits);
  
  return mean;
}



/* @func libscan_CalcSdv ***********************************************
 **
 ** Calculate the standard deviation from a list of hits.
 **
 ** @param [r] scophits [AjPList] List of scophits
 ** @param [r] mean     [float] mean score
 **
 ** @return [float] the standard deviation.

 ** @@
 *********************************************************************/
static float libscan_CalcSdv(AjPList scophits, float mean)
{
  ajint i           = 0;
  ajint nhits       = 0;
  
  float sum_dev     = 0.0;
  float dev         = 0.0;
  float sdv         = 0.0;
  
  AjPScophit* hits  = NULL;
  
  if(!scophits)
    ajFatal("No scophits passed to libscan_CalSdv\n");
  
  if(!mean)
    ajFatal("No mean passed to libscan_CalSdv\n");
  
  if(ajListLength(scophits)==0)
    return ajFalse;
  
  /* convert list to array for convenience */
  nhits = ajListToArray(scophits,(void ***)&hits);
  
  if(!nhits)
    ajFatal("No hits in libscan_CalcMean!");
  
  
  for(i=0; i<nhits; i++){
    sum_dev += ((hits[i]->Score - mean)*(hits[i]->Score - mean));
  }
  
  
  dev = (sum_dev/(nhits - 1));
  
  sdv = sqrt(dev);
  
  return sdv;
}


/* @func libscan_ScoreZscore ********************************************
 **
 ** Given the mean and standard deviation, calculate a z-score for
 ** a hit.
 **
 ** @param [r] score [float] raw hit score
 ** @param [r] mean  [float] mean score
 ** @param [r] sdv   [float] the standard deviation
 **
 ** @return [float] the z-score.

 ** @@
 *********************************************************************/
static float libscan_ScoreToZscore(float score,float mean,float sdv)
{
  float z_score = 0.0;
  
  if(!score)
    ajFatal("No score passed to libscan_ScoreZscore\n");
  
  if(!mean)
    ajFatal("No mean passed to libscan_ScoreZscore\n");
  
  if(!sdv)
    ajFatal("No sdv passed to libscan_ScoreZscore\n");
  
  z_score = ((score - mean)/sdv);
  
  return z_score;
}










/* @funcstatic libscan_RunSamInModeOne **************************************
**
** Scan database with sam hidden markov model. 
**
** @param [r] hit1
** @param [r] hit2
**
** @return [AjBool]
** @@
******************************************************************************/
static AjBool libscan_RunSamInModeOne(AjPStr samdb, AjPStr sampath,
				      AjPStr samextn, AjPStr *mode,
				      AjPStr model,AjPList scoplist,
				      AjPStr samoutpath, AjPStr samoutextn)
{
    AjPList samlist    = NULL;		/* a list of sam file names */
    
    AjPStr samfile     = NULL;
    AjPStr family      = NULL;
    AjPStr superfamily = NULL;
    AjPStr fold        = NULL;
    AjPStr class       = NULL;
    AjPStr sunid       = NULL;
    AjPStr samsearch   = NULL;
    
    AjPFile samoutf    = NULL;
    
    ajint posdash;
    ajint posdot;       
    ajint sun_id;    
    
    sunid       = ajStrNew();
    samsearch   = ajStrNew();
    family      = ajStrNew();
    superfamily = ajStrNew();
    fold        = ajStrNew();
    class       = ajStrNew();
    
    if(ajStrChar(*mode,0)=='1')
    {   
        samlist = libscan_GetLibrary(sampath,samextn);
        
        /*Start of main application loop*/   
        while(ajListPop(samlist,(void **)&samfile))
        {
            /* get the "seed" bit of filenames from the samfile */
            posdash = ajStrRFindC(samfile, "/");
            posdot  = ajStrRFindC(samfile, ".");
	    
	    
            if(posdash >= posdot)
                ajFatal("Could not create filename. Email rranasin@hgmp.mrc.ac.uk");
	    
            else
            {
                /* get the Family_Sunid */
                ajStrAssSub(&sunid, samfile, posdash+1, posdot-1);
		
                /* CREAT OUTPUT FILE NAMES */
                ajStrAssS(&samsearch,sunid);
                ajStrApp(&samsearch, samoutextn);
		
		
                /* CREAT OUTPUT STREAMS */
                samoutf = ajFileNewOutD(samoutpath,samsearch);
		
                ajStrToInt(sunid,&sun_id);
            
                /* do a binary search on scoplist to find the relavent entry */
                if(!(libscan_SunidToScopInfo(sun_id,&family,&superfamily,
					     &fold, &class, scoplist)))
                    ajFatal("libscan_SunidToScopInfo failed in "
			    "libscan_RunSamerInModeOne. "
			    "email rranasin@hgmp.mrc.ac.uk\n");
            
                /* search with SAM HMM */
                libscan_SamSearch(samdb,samfile, family,superfamily,fold,class,
				  sun_id,model,samoutf);

                ajFileClose(&samoutf);
		ajStrDel(&samfile);
	    }
        }
	
	ajListDel(&samlist);
	
	
	ajStrDel(&sunid);
	ajStrDel(&samsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold);   
	ajStrDel(&class);        
	return ajTrue;
    } 
    
    else
    {
	ajStrDel(&samfile);
	ajStrDel(&sunid);
	ajStrDel(&samsearch);
	ajStrDel(&family);
	ajStrDel(&superfamily);
	ajStrDel(&fold);
	ajStrDel(&class);
       
        return ajFalse;
    }
}       


/* @funcstatic libscan_RunSamInModeTwo **************************************
**
** Scan sequences with hidden markov model. 
**
** @param [r] 
** @param [r] 
**
** @return [AjBool] 
** @@
******************************************************************************/

static AjBool libscan_RunSamInModeTwo(AjPSeqset db, AjPStr sampath,
				      AjPStr samextn, AjPStr *mode, 
				      AjPList scoplist, AjPStr model, 
				      AjPStr samoutpath, AjPStr samoutextn)
{
    AjPSeq seq = NULL;
    ajint i;
						   
    if(ajStrChar(*mode,0)=='2') 
    {
        for(i=0;i<db->Size;i++)
        {
            seq = ajSeqNew();
            ajStrAssS(&seq->Name,db->Seq[i]->Name);
            ajStrAssS(&seq->Seq,db->Seq[i]->Seq);
            
            /* search a library of hidden markov models */
            /*libscan_SamLibScan(seq,sampath,samextn,scoplist,model,
			       samoutpath,samoutextn);*/
            
            ajSeqDel(&seq);
        }
       
        return ajTrue;
    }

    else
        return ajFalse;
}


/* @funcstatic libscan_SamSearch ********************************************
 **
 ** Runs samsearch at the command line and writes the results to a specified
 ** file.
 **
 **
 ** @param [r] filename  [AjPStr]  Name of file containing the hmm.
 ** @paran [r] db        [AjPStr]  The database to be searched.
 ** @param [r] outfname  [AjPStr]  Name of output file for search results.
 **
 ** @return [AjBool] Returns true on completion.

 ** @@
 **
 ******************************************************************************/
static AjBool libscan_SamSearch(AjPStr samdb, AjPStr samfile, AjPStr family,
				AjPStr superfamily,AjPStr fold, 
				AjPStr class, ajint sun_id,
				AjPStr model, AjPFile outf)
{	
    ajint start       = 0;
    ajint end         = 0;
    
    ajint nhits         = 0;

    AjPStr tmp            = NULL;
    AjPStr srt_end_str    = NULL;       /* string fragment containing start and end info */
    AjPStr start_str      = NULL;
    AjPStr end_str        = NULL;
  
    AjPStr line           = NULL;       /* the next line in a file */
    AjPStr sambasename    = NULL;       /* name of the sam input file */
    AjPStr samoutfname    = NULL;       /* name of the sam output file */

    AjPFile inf         = NULL;

    AjPRegexp rexp1     = NULL;
    AjPRegexp rexp2     = NULL;
    AjPRegexp rexp3     = NULL;
    

    AjPStr cmd          = NULL;         /* command line to run SAM */
    AjPStr  id          = NULL;         /* Hit identifier */

    float   score       = 0.0;          /* score for alignment */

    AjPHit hit          = NULL;         /* hit that gets constructed in the first parse */
    AjPHit* hits        = NULL;

    AjPScophit scop     = NULL;

    AjPHitlist hitlist  = NULL;

    AjPList listhits    = NULL;         /* the list of hits */
    AjPList scophits    = NULL;
    AjPList tmplist     = NULL;

    AjIList iter        = NULL;         /* list iterator */


    if(!samdb || !samfile || !outf)
    {
      ajWarn("Bad arguments passed to libscan_SamSearch\n");
      return ajFalse;
    }

    rexp1  = ajRegCompC("^([/]+)");
    rexp2  = ajRegCompC("^/([0-9]+).dhf#([0-9]+)_([0-9]+):([0-9]+)");
    rexp3  = ajRegCompC(":([0-9a-zA-Z]+)");
  
    cmd           = ajStrNew();
    line          = ajStrNew();
    tmp           = ajStrNew();
    srt_end_str   = ajStrNew();
    start_str     = ajStrNew();
    end_str       = ajStrNew();
    sambasename   = ajStrNew();
    samoutfname   = ajStrNew();
    id            = ajStrNew();
  
    listhits    = ajListNew();
    scophits    = ajListNew();
    tmplist     = ajListNew();
  
    /* CREAT TEMPERARY FILE NAMES */
    /* create the filename for the samsearch output file */
    ajRandomSeed();
    ajStrAssC(&tmp, ajFileTempName(NULL));
  
    /* temp base name for sam output file*/
    ajStrAssS(&sambasename,tmp);
  
    /* name of the output file created */
    ajStrAssS(&samoutfname,sambasename);
    ajStrAppC(&samoutfname, ".mstat");
    
    /* RUN SAMSEARCH */
    ajFmtPrintS(&cmd,"/usr/local/sam/bin/hmmscore %S -i %S -db %S -select_mdalign 8 -sw 3",sambasename,samfile,samdb);

    /* execute the command line */
    system(ajStrStr(cmd));
        
    inf = ajFileNewIn(samoutfname);
    
    /* GET ALL INFOMATION ASSOCIATED WITH A MATCH */
    while(ajFileReadLine(inf,&line)){

      if(ajRegExec(rexp1,line)){
	
	/* Get the statistics for the current entry */
	ajFmtScanS(line,"%S %*d %f %*f %*g %S",&srt_end_str,&score,&tmp);

	/* get accession number information */
	ajRegExec(rexp3,tmp);
	ajRegSubI(rexp3,1,&id);
	
	/* get the start and end information */
	ajRegExec(rexp2,srt_end_str);

	ajRegSubI(rexp2,3,&start_str);
	ajStrToInt(start_str,&start);

	ajRegSubI(rexp2,4,&end_str);
	ajStrToInt(end_str,&end);


	/* construct the last  hit */
	hit = embHitNew();
	
	/* construct the hit */
	ajStrAssS(&hit->Acc,id);
	ajStrAssS(&hit->Model,model);
	hit->Score = score;
	hit->Start = start;
	hit->End   = end;
	
	ajListPushApp(listhits,hit);	
      }
      else
	continue;
    }
    
    ajFileClose(&inf);
  
    hitlist = embHitlistNew(0);
  
    /* write hit structure */
    ajStrAssS(&hitlist->Class,class);
    ajStrAssS(&hitlist->Fold,fold);
    ajStrAssS(&hitlist->Superfamily,superfamily);
    ajStrAssS(&hitlist->Family,family);
    ajStrAssS(&hitlist->Model,model);
    hitlist->Sunid_Family = sun_id;
   
    /* Convert list to array within Hitlist object */
    nhits=ajListToArray(listhits,(void ***)&hitlist->hits);
    hitlist->N = nhits;
  
    if(!(hitlist->N))
	ajFatal("hitlist->N==0 in libscan_SamSearch\n");
  
    /* convert the hitlist to scophits so the related hits can be targetted for removal
       in order to calculate an unbiased distribution */
    ajListPushApp(tmplist,hitlist);
    embDmxHitlistToScophits(tmplist,&scophits);
  
    /*******************************************************************
     **
     ** THE HITLIST MUST BE WRITTEN OUT TO A FILE AND NOT THE HIT LIST
     ** THE MERGING SHOULD BE DONE AS A SEPARATE APPLICATION
     **
     *******************************************************************/
    ajDmxScophitsWriteFasta(outf,scophits);
    
    /* clean up tmplist / hitlist */
    /* Set N to zero so that the nodes in the list are not freed,
       the structure itself will be freed though */
    hitlist->N=0;
    embHitlistDel(&hitlist);
    ajListDel(&tmplist);
  
    /* clean up scophits */
    iter = ajListIter(scophits);
    while((scop = (AjPScophit)ajListIterNext(iter)))
	ajDmxScophitDel(&scop);
    ajListDel(&scophits);
    ajListIterFree(&iter);
  
    /* Clean up listhits */
    iter = ajListIter(listhits);
    while((hit = (AjPHit)ajListIterNext(iter)))
	embHitDel(&hit);
    ajListDel(&listhits);
    ajListIterFree(&iter);

    /* Clean up hits array */
    AJFREE(hits);

    /* free up regular expressions */
    ajRegFree(& rexp1);
    ajRegFree(& rexp2);
    ajRegFree(& rexp3);
    
    /* clean up directory */
    ajFmtPrintS(&cmd,"rm %S.dist",sambasename);
    system(ajStrStr(cmd));
    ajFmtPrintS(&cmd,"rm %S.mstat",sambasename);
    system(ajStrStr(cmd));
    ajFmtPrintS(&cmd,"rm %S.mult",sambasename);
    system(ajStrStr(cmd));

    ajStrDel(&tmp );
    ajStrDel(&srt_end_str);
    ajStrDel(&sambasename);
    ajStrDel(&samoutfname);
    ajStrDel(&id);
    ajStrDel(&line);
    ajStrDel(&start_str);
    ajStrDel(&end_str);

    return ajTrue;
}














