/******************************************************************************
**
** A genome feature (in AJAX program context) is a description of a
** genomic entity which was determined by some 'source' analysis
** (which may be of 'wet lab' experimental or 'in silico'
** computational nature), has a 'primary' descriptor ('Primary_Tag'),
** may have some 'score' asserting the level of analysis confidence in
** its identity (e.g. log likelihood relative to a null hypothesis or
** other similar entity), has a 'Position' in the genome, and may have
** any arbitrary number of descriptor tags associated with it.
**
** @author Copyright (C) 1999 Richard Bruskiewich
** modified by Ian Longden.
** modified by Peter Rice.
** @version 3.0
** @@
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
** Implementation Notes:
**
** see also the header file, ajfeat.h, for details on class structure.
**
******************************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ajax.h"

typedef struct FeatSTagval {
  AjPStr Tag;
  AjPStr Value;
} FeatOTagval, *FeatPTagval;

#define FEATFLAG_START_BEFORE_SEQ 0x0001 /* <start */
#define FEATFLAG_END_AFTER_SEQ    0x0002 /* >end */
#define FEATFLAG_CHILD            0x0004 /* join() */
#define FEATFLAG_BETWEEN_SEQ      0x0008  /* x^y */
#define FEATFLAG_START_TWO        0x0010  /* x.y.. */
#define FEATFLAG_END_TWO          0x0020  /* ..x.y */
#define FEATFLAG_POINT            0x0040  /* x */
#define FEATFLAG_COMPLEMENT_MAIN  0x0080  /* complement around the join */
#define FEATFLAG_MULTIPLE         0x0100  /* part of a multiple i.e. join*/
#define FEATFLAG_GROUP            0x0200  /* group */
#define FEATFLAG_ORDER            0x0400  /* order */
#define FEATFLAG_ONEOF            0x0800  /* one_of */
#define FEATFLAG_REMOTEID         0x1000  /* AB012345.6: */
#define FEATFLAG_LABEL            0x2000  /* label as location */

static AjBool   FeatInitDone = AJFALSE;
static AjPTable FeatTypeTable = NULL;
static AjPTable FeatTagsTable = NULL;

static AjBool   FeatInitGff = AJFALSE;
static AjPTable FeatTypeTableGff = NULL;
static AjPTable FeatTagsTableGff = NULL;

static AjBool   FeatInitEmbl = AJFALSE;
static AjPTable FeatTypeTableEmbl = NULL;
static AjPTable FeatTagsTableEmbl = NULL;

static AjBool   FeatInitSwiss = AJFALSE;
static AjPTable FeatTypeTableSwiss = NULL;
static AjPTable FeatTagsTableSwiss = NULL;

static AjBool   FeatInitAcedb = AJFALSE;
static AjPTable FeatTypeTableAcedb = NULL;
static AjPTable FeatTagsTableAcedb = NULL;

static AjPStr featTagFmt (AjPStr name, AjPTable table);
static AjPStr featTagLimit (AjPStr name, AjPTable table);
static void   featDumpGff (AjPFeature thys, AjPFeattable owner,
			   AjPFile file);
static void   featDumpEmbl (AjPFeature thys, AjPStr location,
			    AjPFile file, AjBool IsEmbl);
static void   featDumpSwiss (AjPFeature thys,
			     AjPFile file);
static void featInit (void);
static AjPStr featType (AjPStr type);
static AjPStr featTag (AjPStr type);
static AjPStr featTableType (AjPStr type, AjPTable table);
static AjPStr featTableTag (AjPStr tag, AjPTable table);
static AjPStr featTableTagC (char *tag, AjPTable table);
static AjBool featVocabRead (char *name, AjPTable TypeTable,
			     AjPTable TagsTable);

static void FeattabInit ( AjPFeattable thys, 
                          AjPStr name);
 
static FeatPTagval featTagvalNew ( AjPStr tag, AjPStr value);
static FeatPTagval featTagvalNewC ( char* tag, AjPStr value);
static FeatPTagval featTagval ( AjPFeature thys, AjPStr tag);
static void featTagSetDefault (AjPStr tag, AjPStr value,
			AjPStr* pdeftag, AjPStr* pdefval);
static char featFrame (ajint frame);
static char featStrand (ajint strand);

static void ajFeatSetFlag(ajint *flags, ajint val);

static AjPFeature featSwissFromLine ( AjPFeattable thys, AjPStr line);

static AjPStr featLocEmblWrapC(AjPStr* pval, ajint width,
			       char* prefix, char* preftyp);
static AjBool featTagSpecialAllAnticodon(AjPStr* pval);
static AjBool featTagSpecialAllCitation(AjPStr* pval);
static AjBool featTagSpecialAllCodon(AjPStr* pval);
static AjBool featTagSpecialAllConssplice(AjPStr* pval);
static AjBool featTagSpecialAllRptunit(AjPStr* pval);
static AjBool featTagSpecialAllTranslexcept(AjPStr* pval);
static AjBool featTagSpecialAllDbxref(AjPStr* pval);
static AjBool featTagSpecialAllProteinid(AjPStr* pval);
static AjBool featTagSpecialAllReplace(AjPStr* pval);
static AjBool featTagSpecialAllTranslation(AjPStr* pval);
static AjBool featTagAllLimit(AjPStr* pval, AjPStr values);
static void   featTagEmblQuote(AjPStr* pval);
static AjPStr featTagEmblWrapC(AjPStr* pval, ajint width, char* prefix);
static void   featTagEmblDefault(AjPStr* pout, AjPStr tag, AjPStr* pval);
static void   featTagGffDefault(AjPStr* pout, AjPStr tag, AjPStr* pval);
static AjBool featTagSpecial(AjPStr* pval, AjPStr tag);
static AjBool featTagGffSpecial(AjPStr* pval, AjPStr tag);


static AjBool featReadUnknown  ( AjPFeattable thys, AjPFileBuff file) ;
static AjBool featReadAcedb    ( AjPFeattable thys, AjPFileBuff file) ;
static AjBool featReadEmbl     ( AjPFeattable thys, AjPFileBuff file) ;
static AjBool featReadGff      ( AjPFeattable thys, AjPFileBuff file) ;
static AjBool featReadSwiss    ( AjPFeattable thys, AjPFileBuff file) ;

static AjBool featRegInitAcedb();
static AjBool featRegInitEmbl();
static AjBool featRegInitGff();
static AjBool featRegInitSwiss();

static void featClear ( AjPFeature thys );
static AjBool featDelRegAcedb();
static AjBool featDelRegEmbl();
static AjBool featDelRegGff();
static AjBool featDelRegSwiss();

static AjBool featGetUsaSection(AjPStr* tmp, AjPStr token, ajint* begin,
				ajint* end, AjPStr usa);

static AjPFeature featFeatureNew(AjPFeattable thys,
				 AjPStr       source, 
				 AjPStr       type,
				 ajint        Start, ajint End,
				 float        score,
				 char         strand,
				 ajint        frame, 
				 AjPStr       desc,
				 ajint        exon,
				 ajint        Start2, ajint End2,
				 AjPStr       entryid, AjPStr label,
				 ajint        flags );

static ajint featGetPos(AjPStr *pos,ajint *ipos);
static ajint featCompByStart(const void *a, const void *b);
static ajint featCompByEnd(const void *a, const void *b);
static ajint featCompByGroup(const void *a, const void *b);
static ajint featCompByType(const void *a, const void *b);
static void featGroupSet (AjPFeature gf, AjPFeattable table,
			  AjPStr grouptag);
static void featFlagSet (AjPFeature gf, AjPStr flags);

static AjBool feattableWriteEmbl (AjPFeattable Feattab, AjPFile file,
				  AjBool IsEmbl);

typedef struct FeatSInFormat {
  char *Name;
  AjBool Dna;
  AjBool Prot;
  AjBool Used;
  AjBool (*Read)  (AjPFeattable thys, AjPFileBuff file);
  AjBool (*InitReg)();
  AjBool (*DelReg)();
} FeatOInFormat, *FeatPInFormat;

/* name             Dna   Protein
   input-function   init-regex-function del-regex-function */

static FeatOInFormat featInFormatDef[] = {
  {"unknown",       AJTRUE,             AJTRUE,        AJFALSE,
   featReadUnknown, NULL,               NULL},
  {"embl",          AJTRUE,             AJFALSE,       AJFALSE,
   featReadEmbl,    featRegInitEmbl,    featDelRegEmbl},
  {"genbank",       AJTRUE,             AJFALSE,       AJFALSE,
   featReadEmbl,    featRegInitEmbl,    featDelRegEmbl},
  {"gb",            AJTRUE,             AJFALSE,       AJFALSE,
   featReadEmbl,    featRegInitEmbl,    featDelRegEmbl},
  {"ddbj",          AJTRUE,             AJFALSE,       AJFALSE,
   featReadEmbl,    featRegInitEmbl,    featDelRegEmbl},
  {"gff",           AJTRUE,             AJTRUE,        AJFALSE,
   featReadGff,     featRegInitGff,     featDelRegGff},
  {"acedb",         AJTRUE,             AJFALSE,       AJFALSE,
   featReadAcedb,   featRegInitAcedb,   featDelRegAcedb},
  {"swissprot",     AJFALSE,            AJTRUE,        AJFALSE,
   featReadSwiss,   featRegInitSwiss,   featDelRegSwiss},
  {"sw",            AJFALSE,             AJTRUE,       AJFALSE,
   featReadSwiss,   featRegInitSwiss,   featDelRegSwiss},
  {NULL, AJFALSE, AJFALSE, AJFALSE, NULL, NULL, NULL}
};

static FeatPInFormat featInFormat = featInFormatDef;

static AjBool ajFeattableWriteUnknown (AjPFeattable features, AjPFile file);

static AjPFeature featEmblFromLine ( AjPFeattable thys, AjPStr line,
				     AjPStr* saveloc, AjPStr* saveline);
static AjPFeature featEmblProcess  ( AjPFeattable thys, AjPStr feature,
				     AjPStr source,
				     AjPStr* loc, AjPStr* tags);
static AjPFeature featGffFromLine ( AjPFeattable thys, AjPStr line,
				    float version);

static void GFFProcessTagValues (AjPFeature gf, AjPFeattable table,
				 AjPStr groupfield, float version);

typedef struct FeatSOutFormat {
  char *Name;
  AjBool (*Write) (AjPFeattable thys, AjPFile file);
} FeatOOutFormat, *FeatPOutFormat;

static FeatOOutFormat featOutFormatDef[] = {
  {"unknown",   ajFeattableWriteUnknown},
  {"embl",      ajFeattableWriteEmbl},
  {"genbank",   ajFeattableWriteGenbank},
  {"gb",        ajFeattableWriteGenbank},
  {"ddbj",      ajFeattableWriteDdbj},
  {"gff",       ajFeattableWriteGff},
  {"swissprot", ajFeattableWriteSwiss},
  {"sw",        ajFeattableWriteSwiss},
  {NULL, NULL}
};

static FeatPOutFormat featOutFormat = featOutFormatDef;


/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const Except_T Null_Feature_Object     = {
  "NULL AjPFeature object encountered!" };
static const Except_T Not_a_Subclass          = {
  "Invalid AjPFeature object/class encountered!" };
static const Except_T Null_Feature_Tag        = {
  "NULL AjPFeature tag encountered!" };
static const Except_T Null_IO_Handle          = {
  "NULL feature I/O handle encountered!" };

/* Set each of the regular expressions below, depending on feature format */

static AjPRegexp 
  GffRegexNumeric   = NULL,
  GffRegexblankline = NULL,
  GffRegexversion   = NULL,
  GffRegexdate      = NULL,
  GffRegexregion    = NULL,
  GffRegexcomment   = NULL,

  GffRegexTvTagval   = NULL,

  EmblRegexNew         = NULL,
  EmblRegexNext        = NULL,
  EmblRegexTv          = NULL,
  EmblRegexTvTag       = NULL,
  EmblRegexTvTagQuote  = NULL,
  EmblRegexTvTagQuote2 = NULL,
  EmblRegexOperIn      = NULL,
  EmblRegexOperOut     = NULL,
  EmblRegexOperNone    = NULL,
  EmblRegexLocMulti    = NULL,
  EmblRegexLoc         = NULL,
  EmblRegexLocRange    = NULL,
  EmblRegexLocNum      = NULL,

  SwRegexFeature   = NULL,
  
  DummyRegExec     = NULL
;

/* ==================================================================== */
/* ======================== private methods ========================= */
/* ==================================================================== */

static AjBool featFindInFormat (AjPStr format, ajint* iformat);
static AjBool featFindOutFormat (AjPStr format, ajint* iformat);

static AjBool featoutUfoProcess (AjPFeattabOut thys, AjPStr ufo);
static void FeattabClear  ( AjPFeattable thys ) ;

/* ==================================================================== */
/* ========================= constructors ============================= */
/* ==================================================================== */

/* @section Feature Object Constructors ***************************************
**
** All constructors return a new feature or feature table set by pointer. 
** It is the responsibility of the user to first destroy any previous feature. 
** The target pointer does not need to be initialised to NULL, but it is good
** programming practice to do so anyway.
**
** Generally, however, the  'table' class object for AjFeatures must first
** be created, prior to creating any 'AjFeature' objects.
**
** To replace or reuse an existing feature object see instead
** the {Feature Assignments} and {Feature Modifiers} methods.
**
******************************************************************************/

/* @func ajFeattabOutOpen *****************************************************
**
** Processes the specified UFO, and opens the resulting output file.
**
** @param [r] thys [AjPFeattabOut] Features table output object
** @param [r] ufo [AjPStr] UFO feature output specifier
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajFeattabOutOpen (AjPFeattabOut thys, AjPStr ufo) {

  ajDebug("ajFeattabOutOpen UFO '%S'\n", ufo);
  if (thys->Handle)
    return ajTrue;

  if (!featoutUfoProcess (thys, ufo))
    return ajFalse;

  ajDebug("trying to open '%S'\n", thys->Filename);
  thys->Handle = ajFileNewOut(thys->Filename);
  if (!thys->Handle) return ajFalse;
  ajDebug("after opening '%S'\n", thys->Filename);
  
  return ajTrue;
}

/* @func ajFeattabInNew *******************************************************
**
** Constructor for an empty feature table input object
**
** @return [AjPFeattabIn] Feature table input object
** @@
******************************************************************************/

AjPFeattabIn ajFeattabInNew (void) {
  AjPFeattabIn pthis;
  AJNEW0(pthis);

  return pthis;
}

/* @func ajFeattabInNewSSF ****************************************************
**
** Constructor for an empty feature table input object
**
** @param [r] fmt [AjPStr] feature format
** @param [r] name [AjPStr] sequence name
** @param [r] type [char*] feature type
** @param [r] buff [AjPFileBuff] Buffer containing feature data
** @return [AjPFeattabIn] Feature table input object
** @@
******************************************************************************/

AjPFeattabIn ajFeattabInNewSSF (AjPStr fmt, AjPStr name, char* type,
			       AjPFileBuff buff) {
  AjPFeattabIn pthis;
  ajint iformat = 0;

  if (!featFindInFormat(fmt, &iformat)) return NULL;

  pthis = ajFeattabInNew ();
  ajStrAssC (&pthis->Formatstr, featInFormat[pthis->Format].Name);
  pthis->Format = iformat;
  ajStrAssC (&pthis->Type, type);
  ajStrAssS (&pthis->Seqname, name);
  pthis->Handle = buff;

  return pthis;
}

/* @func ajFeattabOutNew ******************************************************
**
** Constructor for an empty feature table output object
**
** @return [AjPFeattabOut] Feature table input object
** @@
******************************************************************************/

AjPFeattabOut ajFeattabOutNew (void) {
  AjPFeattabOut pthis;
  AJNEW0(pthis);
  return pthis;
}

/* @func ajFeattabOutNewSSF ***************************************************
**
** Constructor for an empty feature table output object
**
** @param [r] fmt [AjPStr] feature format
** @param [r] name [AjPStr] sequence name
** @param [r] type [char*] feature type
** @param [r] file [AjPFile] Output file
** @return [AjPFeattabOut] Feature table output object
** @@
******************************************************************************/

AjPFeattabOut ajFeattabOutNewSSF (AjPStr fmt, AjPStr name, char* type,
			       AjPFile file) {
  AjPFeattabOut pthis;
  ajint iformat = 0;

  if (!featFindOutFormat(fmt, &iformat)) return NULL;

  pthis = ajFeattabOutNew ();
  ajStrAssC (&pthis->Formatstr, featOutFormat[pthis->Format].Name);
  pthis->Format = iformat;
  ajStrAssC (&pthis->Type, type);
  ajStrAssS (&pthis->Seqname, name);
  pthis->Handle = file;

  return pthis;
}

/* @func ajFeaturesRead *******************************************************
**
** Generic interface function for reading in features from a file
** given the file handle, class of map, data format of input
** and possibly other associated data.
**
** @param  [rC] ftin   [AjPFeattabIn]  Specifies the external source (file)
**                                     of the features to be read in
** @return [AjPFeattable] Pointer to a new feature table containing
** the features read in
** @cre 'file' argument must be a valid AjPFile
** @exception  'Null_IO_Handle' if ftin or its embedded file handle is invalid
** @@
**
** Version 1.0, 7/6/99 ACD to ajfeat access function (reading features)
**
******************************************************************************/

AjPFeattable ajFeaturesRead  ( AjPFeattabIn  ftin ) 
{
   AjPFileBuff   file ;
   ajint           format ;
   AjPFeattable features = NULL ;
   AjBool       result   = ajFalse ;

   if(ftin  == NULL) AJRAISE(Null_IO_Handle) ;
   file     = ftin->Handle ;
   if(file  == NULL) AJRAISE(Null_IO_Handle) ;
   format   = ftin->Format ;

   if (!format)
     return NULL;

   ajDebug ("ajFeaturesRead format %d '%s'\n",
	    format, featInFormat[format].Name);

   if(!featInFormat[format].Used) {
     if(!featInFormat[format].InitReg()) {
       ajDebug("No InitReg yet for %s\n",featInFormat[format].Name);
       ajErr ("Initialisation failed for feature format %s",
	      featInFormat[format].Name);
     }
     featInFormat[format].Used = ajTrue;
   }

   features = ajFeattableNew (ftin->Seqname);
   result = featInFormat[format].Read(features, file);

   if(result) {
     ajFeattableTrace (features);
      return features ;
   } else {
      ajFeattabDel(&(features)) ;
   }
   return NULL;
}

/* @func ajFeatureNew *********************************************************
**
** Constructor - must specify associated 'ajFeattable'
**               to which the new feature is automatically added!
**
** @param  [rC]   thys    [AjPFeattable] Pointer to the ajFeattable which
**                         owns the feature
** @param  [rENU] source   [AjPStr]      Analysis basis for feature
** @param  [rENU] type     [AjPStr]      Type of feature (e.g. exon)
** @param  [rNU]  Start    [ajint]  Start position of the feature
** @param  [rNU]  End      [ajint]  End position of the feature
** @param  [rENU] score    [float]      Analysis score for the feature
** @param  [rNU]  strand   [char]  Strand of the feature
** @param  [rNU]  frame    [ajint]   Frame of the feature
** @param  [rENU] desc     [AjPStr]      desc of feature feature
** @param  [rNU]  Start2   [ajint]  2nd Start position of the feature
** @param  [rNU]  End2     [ajint]  2nd End position of the feature
** @return [AjPFeature] newly allocated feature object
** @cre 'table' argument must be a valid ajFeattable
** @exception 'Mem_Failed' from memory allocation
** @@
** 
******************************************************************************/

AjPFeature ajFeatureNew (AjPFeattable thys,
			 AjPStr       source, 
			 AjPStr       type,
			 ajint Start, ajint End,
			 float        score,
			 char         strand,
			 ajint        frame, 
			 AjPStr       desc,
			 ajint Start2, ajint End2) 
{
  ajint flags = 0;
  AjPFeature ret = NULL ; 
 
  ret = featFeatureNew(thys,source,type,Start,End,score,strand,frame,desc,
		    0,Start2,End2,NULL, NULL,flags);

  return ret;
}

/* @funcstatic featCompByStart ************************************************
**
** Compare two features by their start.
**
** @param [r] a [const void *] feature
** @param [r] b [const void *] another feature
** 
** @return [ajint] -1 if a is less than b, 0 if a is equal to b else +1.
** @@
******************************************************************************/

static ajint featCompByStart(const void *a, const void *b) {

  AjPFeature gfa = *(AjPFeature *) a;  
  AjPFeature gfb = *(AjPFeature *) b;  

  ajint val=0;

  val = gfa->Start - gfb->Start;
  if(val){
    return val;
  }
  else{
    val = gfb->End - gfa->End;
    if(val)
      return val;
    else
      return 0;
  }
}
/* @funcstatic featCompByEnd **************************************************
**
** Compare two features by their end.
**
** @param [r] a [const void *] feature
** @param [r] b [const void *] another feature
** 
** @return [ajint] -1 if a is less than b, 0 if a is equal to b else +1.
** @@
******************************************************************************/

static ajint featCompByEnd(const void *a, const void *b) {

  AjPFeature gfa = *(AjPFeature *) a;  
  AjPFeature gfb = *(AjPFeature *) b;  

  ajint val=0;

  val = gfa->End - gfb->End;
  if(val){
    return val;
  }
  else{
    val = gfa->Start - gfb->Start;
    if(val)
      return val;
    else
      return 0;
  }
}
/* @funcstatic featCompByGroup ************************************************
**
** Compare two features by their group and exon numbers
**
** @param [r] a [const void *] feature
** @param [r] b [const void *] another feature
** 
** @return [ajint] -1 if a is less than b, 0 if a is equal to b else +1.
** @@
******************************************************************************/

static ajint featCompByGroup(const void *a, const void *b) {

  AjPFeature gfa = *(AjPFeature *) a;  
  AjPFeature gfb = *(AjPFeature *) b;  

  ajint val=0;

  val = gfa->Group - gfb->Group;
  if(val){
    return val;
  }
  else{
    val = gfa->Exon - gfb->Exon;
      return val;
  }
}

/* @funcstatic featCompByType *************************************************
**
** Compare two features by their type (key).
**
** @param [r] a [const void *] feature
** @param [r] b [const void *] another feature
** 
** @return [ajint] -1 if a is less than b, 0 if a is equal to b else +1.
** @@
******************************************************************************/

static ajint featCompByType(const void *a, const void *b) {

  AjPFeature gfa = *(AjPFeature *) a;  
  AjPFeature gfb = *(AjPFeature *) b;  

  ajint val=0;

  val = ajStrCmp(&gfa->Type,&gfb->Type);
  if(val){
    return val;
  }
  else{
    val = gfa->Start - gfb->Start;
    if(val){
      return val;
    }
    else{
      val = gfa->End - gfb->End;
      if(val){
	return val;
      }
      else
	return 0;
    }
    
  }
}

/* @funcstatic featGetPos *****************************************************
**
** Convert a string to a position.
**
** @param [r] pos [AjPStr *] String to be converted.
** @param [w] ipos [ajint *]   integer value to be returned.
**
** @return [ajint] 0 if okay. 1 if first char removed (usually angle bracket)
** @@
******************************************************************************/

static ajint featGetPos(AjPStr *pos,ajint *ipos) {

    if(!ajStrToInt(*pos,ipos))
    {
	
	ajStrCut(pos,0,0);
	if(ajStrToInt(*pos, ipos))
	    return 1;
	else
	{
	    *ipos=0;
	    ajDebug("Error getting start (set to 0 now) for line\n");
	    return -1;
	}
    }
    return 0;
}


/******************************************************************************
**
** Utility classes...
** 
******************************************************************************/


/* @funcstatic featFeatureNew *************************************************
**
** Constructor - must specify associated 'ajFeattable'
**               to which the new feature is automatically added!
**
** @param  [rC]   thys     [AjPFeattable] Pointer to the ajFeattable which
**                         owns the feature
** @param  [rENU] source   [AjPStr]      Analysis basis for feature
** @param  [rENU] type     [AjPStr]      Type of feature (e.g. exon)
** @param  [rNU]  Start    [ajint]  Start position of the feature
** @param  [rNU]  End      [ajint]  End position of the feature
** @param  [rENU] score    [float]      Analysis score for the feature
** @param  [rNU]  strand   [char]  Strand of the feature
** @param  [rNU]  frame    [ajint]   Frame of the feature
** @param  [rENU] desc     [AjPStr]      desc of feature feature
** @param  [rNU]  exon     [ajint]  exon number (0 for default value)
** @param  [rNU]  Start2   [ajint]  2nd Start position of the feature
** @param  [rNU]  End2     [ajint]  2nd End position of the feature
** @param  [rENU] entryid  [AjPStr] Entry ID for location in another entry
** @param  [rENU] label    [AjPStr] Label for location (non-numeric)
** @param  [rNU]  flags    [ajint]  flags.
** @return [AjPFeature] newly allocated feature object
** @exception 'Mem_Failed' from memory allocation
** @cre 'table' argument must be a valid ajFeattable
** @@
** 
******************************************************************************/

static AjPFeature featFeatureNew (AjPFeattable thys,
				  AjPStr       source, 
				  AjPStr       type,
				  ajint        Start,
				  ajint        End,
				  float        score,
				  char         strand,
				  ajint        frame, 
				  AjPStr       desc,
				  ajint        exon,
				  ajint        Start2,
				  ajint        End2,
				  AjPStr       entryid,
				  AjPStr       label,
				  ajint        flags ) {
  AjPFeature ret = NULL ;
  static ajint maxexon = 0;

  ajDebug ("\nfeatFeatureNew '%S' %d .. %d %x\n", type, Start, End, flags);

  if(!ajStrLen(type)){
    return NULL;
  }

  featInit();

  /* Allocate the object... */
  AJNEW0(ret) ;

  if(flags & FEATFLAG_CHILD){
    ret->Group = thys->Groups;
    if (exon)
      ret->Exon  = exon;
    else
      ret->Exon = ++maxexon;
  }
  else{
    thys->Groups++;
    ret->Group = thys->Groups;
    ret->Exon = 0;
  }

  ajStrAssS (&ret->Source, source);

  ajStrAssS (&ret->Type, featType(type));

  ret->Score = score;

  ret->Flags = flags;
  
  ajStrAssS (&ret->desc, desc);

  ret->Tags = ajListNew() ;        /* Assume empty until otherwise needed */

  ret->Comment = NULL ;
  ret->Strand = strand ;
    
  ret->Frame  = frame ;
  ret->Start = Start;
  ret->End = End;
  ret->Start2 = Start2;
  ret->End2 = End2;

  if (ajStrLen(entryid))
    ajStrAssS (&ret->Remote, entryid);

  if (ajStrLen(label))
    ajStrAssS (&ret->Label, label);

  ajFeattabAdd(thys,ret) ;

  return ret ;
}

/* ==================================================================== */
/* =========================== destructor ============================= */
/* ==================================================================== */

/* @section Feature Object Destructors ****************************************
**
** (Simple minded) object destruction by release of memory.
**
** No reference counting (for now). 
**
******************************************************************************/

/* @func ajFeattabInDel *******************************************************
**
** Destructor for a feature table input object
**
** @param [d] pthis [AjPFeattabIn*] Feature table input object
** @return [void]
** @@
******************************************************************************/

void ajFeattabInDel (AjPFeattabIn* pthis) {
  
  AjPFeattabIn thys = *pthis;

  if (!thys) return;

  ajFileBuffDel(&thys->Handle);
  ajStrDel(&thys->Ufo);
  ajStrDel(&thys->Formatstr);
  ajStrDel(&thys->Filename);
  ajStrDel(&thys->Entryname);
  ajStrDel(&thys->Seqname);
  ajStrDel(&thys->Type);
  AJFREE(*pthis);
  return;
}

/* @func ajFeattabDel *********************************************************
**
** Destructor for ajFeattable objects.
** If the given object (pointer) is NULL, or a NULL pointer, simply returns.
**
** @param  [wP] pthis [AjPFeattable*] Pointer to the object to be deleted.
**         The pointer is always deleted.
** @return [void]
** @@
******************************************************************************/

void ajFeattabDel(AjPFeattable *pthis)
{
  AjPFeattable thys;
  if (!pthis) return ;
  if (!*pthis) return ;

  thys = *pthis;

  FeattabClear(thys) ;

  ajStrDel (&thys->Type);

  AJFREE (*pthis) ; /* free the object */
  *pthis = NULL ;

  return;
}

/* @func ajFeatDel ************************************************************
**
** Destructor for AjFeature objects.
** If the given object (pointer) is NULL, or a NULL pointer, simply returns.
**
** @param  [wPC] pthis [AjPFeature*] Pointer to the object to be deleted.
**         The pointer is always deleted.
** @return [void]
** @@
******************************************************************************/

void ajFeatDel(AjPFeature *pthis) {
  if (!pthis) return ;
  if (!*pthis) return ;

  featClear(*pthis) ;

  AJFREE (*pthis) ; /* free the object */
  *pthis = NULL ;

  return;
}

/* @funcstatic featClear ******************************************************
**
** Deletes all feature tag-value pairs from a feature line
**
** @param [r] thys [AjPFeature] Feature
** @return [void]
** @@
******************************************************************************/

static void featClear ( AjPFeature thys ) { 
  AjIList        iter = NULL ;
  FeatPTagval    item = NULL ;
  
  if (!thys) return ;
    
  /* We need to delete the associated Tag data structures too!!!*/

  if (thys->Tags) {
    iter = ajListIter(thys->Tags) ;
    while(ajListIterMore(iter))
    {
      item = (FeatPTagval)ajListIterNext (iter) ;
      /* assuming a simple block memory free for now...*/
      ajStrDel(&item->Value) ;
      ajStrDel(&item->Tag) ;
      AJFREE(item);
      ajListRemove(iter) ;
    }
    ajListIterFree(iter) ;
      
  }
  ajListFree(&(thys->Tags));
  ajListDel(&(thys->Tags)) ;
  
}

/* ==================================================================== */
/* ========================== Assignments ============================= */
/* ==================================================================== */

/* @section Feature Assignments ***********************************************
**
** This category of class methods have three forms: 
**
** 1. Copy constructors ("aj*Copy()" methods): assignments overwrite
**    the target 'Feature' object provided as the first argument by
**    calling the appropriate {Feature destructors} first.  A NULL
**    value is always acceptable so these methods are often used to
**    create new objects by assignment. (As in all object
**    construction), the caller of the function is responsible for
**    calling the respective object constructor.
**  
** 2. Initializers ("aj*Init()" methods): assume an empty object of
**    the specified type and dereferenced by the object pointer
**    provided as the first argument, is to be is to be initialized in
**    two ways: first, by initialization of any 'base class' data (by
**    recursive calls to 'super' class initializers) and second, for
**    the current object class, using arguments provided (if
**    any). These methods are *automatically* called by {Feature
**    constructors} for dynamically created objects, but can (and
**    should) be called explicitly for statically defined objects.
**
******************************************************************************/

/* @func ajFeattabAdd *********************************************************
**
** Method to add a new AjPFeature to a AjPFeattable
**
** @param  [rC] thys    [AjPFeattable] The feature table
** @cre 'thys' must be a valid AjPFeattable
** @param  [rC] feature [AjPFeature]        Feature to be added to the set
** @cre 'feature' must be a valid AjPFeature (non-null)
** @return [void]
** @exception  'Null_Feature_Object' or 'Not_a_Subclass' if 'thys' is invalid
** @exception  'Null_Feature_Object' if null feature argument seen
** @@
**
******************************************************************************/

void ajFeattabAdd ( AjPFeattable thys, AjPFeature feature )
{
  ajListPushApp ( thys->Features, feature);  ;

  if(feature->Type)
    ajDebug ("ajFeattabAdd list size %d '%S' %d %d\n",
	     ajListLength(thys->Features), feature->Type,
	     feature->Start, feature->End);
  else
    ajDebug ("ajFeattabAdd list size %d '%S' %d %d\n",
	     ajListLength(thys->Features), NULL,
	     feature->Start, feature->End);

  return;
}


/* @func ajFeatRead ***********************************************************
**
** Parses a UFO, opens an input file, and reads a feature table
**
** @param [w] pthis [AjPFeattable*] Feature table created, (or NULL if failed)
** @param [r] featin [AjPFeattabIn] Feature input object
** @param [r] ufo [AjPStr] UFO feature spec
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajFeatRead (AjPFeattable* pthis, AjPFeattabIn featin, AjPStr ufo) {
  static AjPRegexp fmtexp = NULL;
  static AjPRegexp filexp = NULL;
  static AjPStr ufotest = NULL;

  AjBool fmtstat = ajFalse;	/* status returns from regex tests */
  AjBool filstat = ajFalse;	/* status returns from regex tests */
  AjBool ret = ajFalse;
  ajint i;

  if (!fmtexp)
    fmtexp = ajRegCompC ("^([A-Za-z0-9]+):+(.*)$");
				/* \1 format */
				/* \2 remainder */

  if (!filexp)
    filexp = ajRegCompC ("^([^:]+)$");

  ajDebug("ajFeatRead UFO '%S'\n", ufo);

  (void) ajStrAssS (&ufotest, ufo);

  if (ajStrLen(ufo)) {
    fmtstat = ajRegExec (fmtexp, ufotest);
    ajDebug("feat format regexp: %B\n", fmtstat);
  }

  if (fmtstat) {
    ajRegSubI (fmtexp, 1, &featin->Formatstr);
    (void) ajStrSetC (&featin->Formatstr, featInFormat[0].Name); /* unknown */
    ajRegSubI (fmtexp, 2, &ufotest); /* trim off the format */
    ajDebug ("found feat format %S\n", featin->Formatstr);

    if (!featFindInFormat (featin->Formatstr, &featin->Format))
      ajErr ("unknown input feature table format '%S'\n"
	     " NO Features will be read in", featin->Formatstr);
  }

  filstat = ajRegExec (filexp, ufotest);
  ajDebug("filexp: %B\n", filstat);
  if (filstat) {
    ajRegSubI (filexp, 1, &featin->Filename);
  }
  else {
    (void) ajFmtPrintS(&ufotest, "%S.%S", featin->Seqname, featin->Formatstr);
    (void) ajStrSet (&featin->Filename, ufotest);
    ajDebug ("generate filename  '%S'\n", featin->Filename);
  }

  /* Open the file so that we can try to read it */

  ajDebug("trying to open '%S'\n", featin->Filename);
  featin->Handle = ajFileBuffNewIn (featin->Filename);
  if (!featin->Handle) return ajFalse;
  ajDebug("after opening '%S'\n", featin->Filename);


 /* OKAY if we have a format specified try this and this ONLY */
  if (fmtstat) {
    if(featin->Format){
      *pthis = ajFeaturesRead(featin);
    }
  }
  /* else loop through all types and try to read gff's */
  else {
    for(i=1;featInFormat[i].Name;i++){
      featin->Format = i;

      *pthis = ajFeaturesRead(featin);
 
      /* Reset buffer to start */
      ajFileBuffReset(featin->Handle);

      if(*pthis){
	ajFileBuffDel(&featin->Handle);
	return ajTrue;
      }
    }
  }
  if (!*pthis) 
    ret = ajFalse;
  else
    ret = ajTrue;

  ajFileBuffDel(&featin->Handle);

  return ret;
}

/* @func ajFeatWrite **********************************************************
**
** Parses a UFO, opens an output file, and writes a feature table to it.
**
** @param [w] thys [AjPFeattable] Feature table created
** @param [r] featout [AjPFeattabOut] Feature output object
** @param [r] ufo [AjPStr] UFO feature spec (ignored)
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajFeatWrite (AjPFeattable thys, AjPFeattabOut featout, AjPStr ufo) {

  ajFeattabOutOpen (featout, ufo);

  return ajFeaturesWrite(featout, thys);
}

/* @func ajFeattableWrite *****************************************************
**
** Parses a UFO, opens an output file, and writes a feature table to it.
**
** @param [w] thys [AjPFeattable] Feature table created
** @param [r] ufo [AjPStr] UFO feature spec (ignored)
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

AjBool ajFeattableWrite (AjPFeattable thys, AjPStr ufo) {
  AjPFeattabOut tabout=NULL;
    
  tabout= ajFeattabOutNew();
  featoutUfoProcess (tabout, ufo);

  return ajFeaturesWrite(tabout, thys);
}

/* @func ajFeatSortByType *****************************************************
**
** Sort Feature table by Type.
**
** @param [rw] Feattab [AjPFeattable] Feature table to be sorted.
**
** @return [void]
** @@
******************************************************************************/
void ajFeatSortByType(AjPFeattable Feattab){
  ajListSort(Feattab->Features,*featCompByType);
}

/* @func ajFeatSortByStart ****************************************************
**
** Sort Feature table by Start position.
**
** @param [rw] Feattab [AjPFeattable] Feature table to be sorted.
**
** @return [void]
** @@
******************************************************************************/
void ajFeatSortByStart(AjPFeattable Feattab){
  ajListSort(Feattab->Features,*featCompByStart);
}

/* @func ajFeatSortByEnd ******************************************************
**
** Sort Feature table by End position. 
**
** @param [rw] Feattab [AjPFeattable] Feature table to be sorted.
**
** @return [void]
** @@
******************************************************************************/
void ajFeatSortByEnd(AjPFeattable Feattab){
  ajListSort(Feattab->Features,*featCompByEnd);
}
  
/* ==================================================================== */
/* ======================== Operators ==================================*/
/* ==================================================================== */

/* @section Feature Object Operators ******************************************
**
** These functions use the contents of a feature object,
** but do not make any changes.
**
******************************************************************************/

/* @funcstatic FeattabInit ****************************************************
**
** Initialize the components of a previously allocated AjPFeattable object.
**
** @param [uC]   thys       [AjPFeattable]   Target feature table object
** @param [rNE]  name       [AjPStr]         Name of the table (e.g.
**                                           sequence name)
** @return [void]
** @cre 'thys' must be non-NULL and pointing to an AjPFeature (or subclass
**      thereof) object
** 
** @@
** 
******************************************************************************/

static void FeattabInit ( AjPFeattable thys, 
                          AjPStr name) { 
  ajDebug ("FeattabInit Entering...\n");

  ajDebug ("FeattabInit initializing name: '%S'\n", name);
  (void) ajStrAssS(&thys->Name,name) ;
  thys->DefFormat = 0;

  thys->Features = ajListNew() ;
}

/* @funcstatic FeattabClear ***************************************************
**
** Clears a feature table of all features
**
** @param [r] thys [AjPFeattable] Feature table
** @return [void]
** @@
******************************************************************************/

static void FeattabClear ( AjPFeattable thys )
{
  AjIList iter       = NULL ;
  AjPFeature feature = NULL ;

  if (!thys) return ;

  /* Format and Version are simple variables, non-allocated...*/
  /* Don't worry about the Date... probably static...*/

  ajStrDel(&thys->Name);
  
  /* I traverse the list of features and delete them first... */
  if (thys->Features) {
    iter = ajListIter(thys->Features) ;
    while(ajListIterMore(iter)) {
      feature = (AjPFeature)ajListIterNext (iter) ;
      ajFeatDel(&feature) ; 
      ajListRemove(iter) ;
    }
    ajListIterFree(iter) ;
  }

  ajListDel(&(thys->Features)) ;
}

/* @funcstatic featoutUfoProcess **********************************************
**
** Converts a UFO Uniform Feature Object into an open output file.
**
** First tests for "format::" and sets thys if it is found
**
** @param [u] thys [AjPFeattabOut] Feature table to be written.
** @param [u] ufo [AjPStr] UFO.
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

static AjBool featoutUfoProcess (AjPFeattabOut thys, AjPStr ufo) {

  static AjPRegexp fmtexp = NULL;
  static AjPRegexp filexp = NULL;

  static AjPStr ufotest = NULL;

  AjBool fmtstat = ajFalse;	/* status returns from regex tests */
  AjBool filstat = ajFalse;	/* status returns from regex tests */
  AjPFeattabOut featout = thys;
 
  if (!fmtexp)
    fmtexp = ajRegCompC ("^([A-Za-z0-9]*):+(.*)$");
				/* \1 format */
				/* \2 remainder */
  if (!filexp)
    filexp = ajRegCompC ("^([^:]+)$");

  ajDebug("featoutUfoProcess UFO '%S'\n", ufo);

  (void) ajStrAssS (&ufotest, ufo);

  if (ajStrLen(ufo)) {
    fmtstat = ajRegExec (fmtexp, ufotest);
    ajDebug("feat format regexp: %B\n", fmtstat);
  }

  if (fmtstat) {
    ajRegSubI (fmtexp, 1, &featout->Formatstr);
    (void) ajStrSetC (&featout->Formatstr,
		      featOutFormat[0].Name); /* unknown */
    ajRegSubI (fmtexp, 2, &ufotest); /* trim off the format */
    ajDebug ("found feat format %S\n", featout->Formatstr);
   }
  else {
    ajDebug("no feat format specified in UFO '%S' try '%S' or 'gff'\n",
	    ufo, featout->Formatstr);
    (void) ajStrSetC(&featout->Formatstr, "gff");
  }

  if (!featFindOutFormat (featout->Formatstr, &featout->Format)){
      ajErr ("unknown output feature format '%S' will write as gff instead\n",
	     featout->Formatstr );
  }
  /* now go for the filename */

  filstat = ajRegExec (filexp, ufotest);
  ajDebug("filexp: %B\n", filstat);
  if (filstat) {
    ajRegSubI (filexp, 1, &featout->Filename);
  }
  else {
    if (ajStrLen(featout->Seqname))
      (void) ajFmtPrintS(&ufotest, "%S.%S", featout->Seqname,
			 featout->Formatstr);
    else
      (void) ajFmtPrintS(&ufotest, "unknown.%S", featout->Formatstr);
	
    (void) ajStrSet (&featout->Filename, ufotest);
    ajDebug ("generate filename  '%S'\n", featout->Filename);
  }

  ajDebug ("\n");

  return ajTrue;
}

/* @funcstatic featFindInFormat ***********************************************
**
** Looks for the specified format(s) in the internal definitions and
** returns the index.
**
** Given a single format, sets iformat.
**
** @param [P] format [AjPStr] Format required.
** @param [w] iformat [ajint*] Index
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

static AjBool featFindInFormat (AjPStr format, ajint* iformat) {

  static AjPStr tmpformat = NULL;
  ajint i = 0;

  ajDebug ("featFindInFormat '%S'\n", format);
  if (!ajStrLen(format))
    return ajFalse;

  (void) ajStrAssS (&tmpformat, format);
  (void) ajStrToLower(&tmpformat);
  for (i=0; featInFormat[i].Name; i++) {
    ajDebug ("test %d '%s' \n", i, featInFormat[i].Name);
    if (!ajStrNCmpC(tmpformat, featInFormat[i].Name, ajStrLen(tmpformat) )) {
      *iformat = i;
      (void) ajStrDelReuse(&tmpformat);
      ajDebug ("found '%s' at %d\n", featInFormat[i].Name, i);
      return ajTrue;
    }
  }

  ajErr ("Unknown input feat format '%S'", format);

  (void) ajStrDelReuse(&tmpformat);
  return ajFalse;
}


/* @funcstatic featFindOutFormat **********************************************
**
** Looks for the specified format(s) in the internal definitions and
** returns the index.
**
** Given a single format, sets iformat.
**
** @param [P] format [AjPStr] Format required.
** @param [w] iformat [ajint*] Index
** @return [AjBool] ajTrue on success.
** @@
******************************************************************************/

static AjBool featFindOutFormat (AjPStr format, ajint* iformat) {

  static AjPStr tmpformat = NULL;
  ajint i = 0;

  ajDebug ("featFindOutFormat '%S'\n", format);
  if (!ajStrLen(format))
    return ajFalse;

  (void) ajStrAssS (&tmpformat, format);
  (void) ajStrToLower(&tmpformat);
  for (i=0; featOutFormat[i].Name; i++) {
    ajDebug ("test %d '%s' len=%d\n",
	     i, featOutFormat[i].Name,ajStrLen(tmpformat));
    if (!ajStrNCmpC(tmpformat, featOutFormat[i].Name,ajStrLen(tmpformat))) {
      *iformat = i;
      (void) ajStrDelReuse(&tmpformat);
      ajDebug ("found '%s' at %d\n", featOutFormat[i].Name, i);
      return ajTrue;
    }
  }

  /*  ajErr ("Unknown output feat format '%S'", format);*/

  (void) ajStrDelReuse(&tmpformat);
  *iformat = 1;
  return ajFalse;
}

/* @func ajFeaturesWrite ******************************************************
**
** Generic interface function for reading in features from a file
** given the file handle, class of map, data format of input
** and possibly other associated data.
**
** @param  [rC] ftout   [AjPFeattabOut]  Specifies the external source
**                                       (file) of the features to be read in
** @cre 'ftout' argument must be a valid AjPFeattabOut object
** @param  [rC] features [AjPFeattable]  Feature set to be written out
** @return [AjBool]  Returns ajTrue if successful; ajFalse otherwise
** @exception  'Null_IO_Handle' if 'ftout' or embedded file file handle invalid
** @exception  'Null_Feature_Object' or 'Not_a_Subclass' if 'features'
**             is invalid
** @exception  'Format_Not_Supported' or 'Unknown_Format' if 'format'
**             is invalid
** @@
**
** Version 1.0, 21/6/99 ACD to ajfeat access function (for writing features)
**
** Old function protocol:
**
** AjBool ajFeaturesWrite( AjPFile file, 
**                         AjPFeattable features, 
**                         ajint format ) 
**
******************************************************************************/

AjBool ajFeaturesWrite ( AjPFeattabOut ftout, AjPFeattable features )  
{
  AjPFile       file ;
  ajint format ;
  AjBool result          = ajFalse ;

  if(features){
    ajDebug( "ajFeaturesWrite Validating arguments\n");
    if(ftout == NULL) AJRAISE(Null_IO_Handle) ;
    file    = ftout->Handle ;
    if(file  == NULL) AJRAISE(Null_IO_Handle) ;
    format  = ftout->Format ;
    
    ajDebug( "ajFeaturesWrite format is %d OK\n",ftout->Format);
    
    result = featOutFormat[format].Write(features, file);

    return result ;
  }
  else{
    ajDebug( " NO Features to output\n");
    return AJTRUE;
  }
}

/* @funcstatic featReadAcedb **************************************************
**
** Reads feature data in ACEDB format
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFileBuff] Buffered input file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featReadAcedb    ( AjPFeattable thys, AjPFileBuff file){
  ajDebug("featReadAcedb NOT IMPLEMENTED YET\n");
  return ajFalse;
}

/* @funcstatic featReadEmbl ***************************************************
**
** Reads feature data in EMBL format
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFileBuff] Buffered input file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featReadEmbl     ( AjPFeattable thys, AjPFileBuff file){
  static AjPStr line  = NULL ;
  AjBool found = ajFalse ;
  static AjPStr saveline = NULL;
  static AjPStr saveloc  = NULL;


  if(!line)
    line = ajStrNewL(100);
  
  while( ajFileBuffGet (file, &line) ) {

    (void) ajStrChomp(&line) ;
    
    if(!ajStrNCmpC(line, "FT", 2)){  /* if it's a feature do stuff */
      if(featEmblFromLine(thys, line, &saveloc, &saveline)) 
	found = ajTrue ;
    }
  }
  if (featEmblFromLine(thys, NULL, &saveloc, &saveline))
      found = ajTrue;

  ajStrDel (&saveloc);
  ajStrDel (&saveline);

  return found;
}

/* @funcstatic featReadUnknown ************************************************
**
** Reads feature data in Unknown format
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFileBuff] Buffered input file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featReadUnknown    ( AjPFeattable thys, AjPFileBuff file) {
  return ajFalse;
}

/* @funcstatic featReadSwiss **************************************************
**
** Reads feature data in SwissProt format
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFileBuff] Buffered input file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featReadSwiss    ( AjPFeattable thys, AjPFileBuff file) {
  static AjPStr line  = NULL ;
  AjBool found = ajFalse;
  ajDebug("featReadSwiss..........\n");
  
  while( ajFileBuffGet (file, &line) ) {
    
    (void) ajStrChomp(&line) ;
    
    if(ajRegExec(SwRegexFeature, line)) {
      if(featSwissFromLine(thys, line))  /* does start with FT - what else?? */
	found = ajTrue ;
    }
  }
  return found ;
}


/* ==================================================================== */
/* ======================== GFF Processing functions ================== */
/* =================================================================== */

/* ajfeat defaults to version 2 GFF only...*/
#define DEFAULT_GFF_VERSION 2


/* @funcstatic featFlagSet ****************************************************
**
** Sets the flags for a feature.
**
** @param [u] gf       [AjPFeature]  Feature
** @param [r] flagstr  [AjPStr]      Flags as a hexadecimal value
** @return [void]
** @@
******************************************************************************/

static void featFlagSet (AjPFeature gf, AjPStr flagstr) {

  static AjPRegexp flagexp = NULL;
  static AjPRegexp moreexp = NULL;
  static AjPStr savstr = NULL;
  static AjPStr tmpstr = NULL;
  static AjPStr typstr = NULL;
  static AjPStr valstr = NULL;
  ajint flags = 0;
  ajint num = 0;

  if (!flagexp)
      flagexp = ajRegCompC ("[ \"]*(0x[0-9a-f]+)");
  if (!moreexp)
      moreexp = ajRegCompC ("[ \"]*([^:]+):([^: \"]+)");

  ajDebug ("featFlagSet '%S'\n", flagstr);
  ajStrAssS (&savstr, flagstr);

  if (ajRegExec(flagexp, savstr)) {
    ajRegSubI (flagexp, 1, &tmpstr);
    if (ajStrToHex(tmpstr, &flags))
	gf->Flags = flags;
    ajDebug("flags: %x", gf->Flags);
    ajRegPost (flagexp, &tmpstr);
    ajStrAssS (&savstr, tmpstr);
  }

  while (ajRegExec(moreexp, savstr)) {
    ajRegSubI (moreexp, 1, &typstr);
    ajRegSubI (moreexp, 2, &valstr);
    ajDebug ("flag type '%S' val '%S'\n", typstr, valstr);
    if (ajStrMatchCaseC(typstr, "start2")) {
      if (ajStrToInt(valstr, &num))
	gf->Start2 = num;
    }
    else if(ajStrMatchCaseC(typstr, "end2")) {
      if (ajStrToInt(valstr, &num))
	gf->End2 = num;
    }
    else if(ajStrMatchCaseC(typstr, "remoteid")) {
      ajStrAssS (&gf->Remote, valstr);
    }
    else if(ajStrMatchCaseC(typstr, "label")) {
      ajStrAssS (&gf->Label, valstr);
    }
    else {
	ajWarn("Unknown GFF FeatFlags type '%S:%S'", typstr, valstr);
    }

    ajRegPost (moreexp, &tmpstr);
    ajStrAssS (&savstr, tmpstr);
  }

  return;
}

/* @funcstatic featGroupSet ***************************************************
**
** Sets the group tag for a feature.
**
** @param [u] gf       [AjPFeature]  Feature
** @param [u] table    [AjPFeattable] Feature table
** @param [r] grouptag [AjPStr]      Group field identifier
** @return [void]
** @@
******************************************************************************/

static void featGroupSet (AjPFeature gf, AjPFeattable table,
			  AjPStr grouptag) {

  static AjPRegexp groupexp = NULL;
  static AjPStr namstr = NULL;
  static AjPStr grpstr = NULL;
  static AjPStr savgrpstr = NULL;
  static ajint saveexon = 0;
  static ajint savegroup = 0;
  ajint grpnum;

  if (!groupexp)
    groupexp = ajRegCompC("^\"(([^.]*)[.])?([0-9]+)");

  if (ajStrLen(grouptag) && ajStrMatchCase(grouptag, savgrpstr)) {
    gf->Group = savegroup;
    gf->Exon = ++saveexon;
    return;
  }


  if (ajStrLen(grouptag) && ajRegExec(groupexp, grouptag)) {
    ajStrAssS (&savgrpstr, grouptag);
    ajRegSubI (groupexp, 2, &namstr);
    ajRegSubI (groupexp, 3, &grpstr);
    ajDebug ("featGroupSet '%S' name: '%S' group: '%S'\n",
	     grouptag, namstr, grpstr);
    if (ajStrToInt (grpstr, &grpnum)) {/* true, if the regex worked */
      gf->Group = grpnum;
      savegroup = grpnum;
    }
    else
      gf->Group = ++(table->Groups);
    if (ajStrLen(namstr)) {
      if (!ajStrMatchCase (namstr, table->Name)) {
	ajDebug ("GFF group field '%S' table '%S'\n",
		 grouptag, table->Name);
	ajWarn ("GFF group field '%S' for table '%S'",
		 grouptag, table->Name);
      }
    }
  }
  else {			/* regex failed, make something up */
    ajStrAssS(&grpstr, grouptag);
    gf->Group = ++(table->Groups);
    savegroup = gf->Group;
    gf->Exon = 0;
    saveexon = 0;
  }

}

/* @funcstatic GFFProcessTagValues ********************************************
**
** Parses and adds a tag-value set to the specified AjPFeature;
** looked at 'parse_group' method in GFF::GeneFeature.pm Perl module
** for inspiration
**
** @param [u] gf [AjPFeature] Feature
** @param [u] table [AjPFeattable] Feature
** @param [r] groupfield [AjPStr] Group field identifier
** @return [void]
** @@
******************************************************************************/

static void GFFProcessTagValues (AjPFeature gf, AjPFeattable table,
				 AjPStr groupfield, float version) 
{
  static AjPStr  TvString  = NULL ;
  AjPStr  tmptag      = NULL ;
  AjPStr  tmpval      = NULL ;
  AjBool  grpset = ajFalse;

  ajDebug("GFFProcessTagValues  version %3.1f '%S'\n", version, groupfield);

  /* Validate arguments */
  if(!ajStrLen(groupfield))	/* no tags, must be new */
    return;

  if( version == 1.0 ) {
    (void) featGroupSet (gf, table, groupfield) ;
    ajDebug("V1.0 group: '%S'\n", groupfield);
    grpset = ajTrue;

    return;
  }

/*
 *     Version 2 or greater: parse groupfield for semicolon ';'
 *     delimited tag-value structures, taking special care about
 *     double quoted string context. rbskfirst versin of code was
 *     adapted from GFF.pm (th/rbsk), itself inherited from AceParse.pm,
 *     courtesy of James Gilbert 
*/

  (void) ajStrAssS( &TvString, groupfield) ;
  while (ajStrLen(TvString)) {
    if (ajRegExec(GffRegexTvTagval, TvString)) {
      ajRegSubI(GffRegexTvTagval, 1, &tmptag);
      ajRegSubI(GffRegexTvTagval, 2, &tmpval);
      (void) ajStrChomp(&tmpval) ;
      ajDebug("GffTv '%S' '%S'\n", tmptag, tmpval);
      ajRegPost (GffRegexTvTagval, &TvString);
      if (ajStrMatchC (tmptag, "Sequence")) {
	(void) featGroupSet (gf, table, tmpval) ;
	grpset = ajTrue;
      }
      else if (ajStrMatchC (tmptag, "FeatFlags"))
	(void) featFlagSet (gf, tmpval) ;
      else
	(void) ajFeatTagSet(gf,tmptag,tmpval) ;
    }
    else {
      ajDebug("Choked on '%S'\n", TvString);
      ajWarn("GFF tag parsing failed on '%S' in\n'%S'",
	     TvString, groupfield);
      ajStrDel(&TvString);
    }
  }

  if (!grpset) {
    (void) featGroupSet (gf, table, NULL) ;
    grpset = ajTrue;
  }

  return;
}

/* @funcstatic featSwissFromLine **********************************************
**
** Read input file in Swiss format
**
** Format is :-
** 0-1    FT
** 5-12   Keyname
** 14-19  From
** 21-26  To
** 34-74  Descrition
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] line [AjPStr] Input buffered file
** @return [AjPFeature] NULL if error.
** @@
******************************************************************************/

static AjPFeature featSwissFromLine ( AjPFeattable thys, AjPStr line)
{
  static AjPStr source   = NULL;
  static AjPStr feature  = NULL;
  static AjPStr start    = NULL;
  static AjPStr end      = NULL;
  static AjPStr desc     = NULL;
  ajint frame = 0 ;
  char strand  = '\0';
  ajint flags = 0;
  static AjPFeature gf    = NULL ;    /* made static so that it's easy
					 to add second line of description */
  ajint Start, End, val;
  float fscore = 0.0;
  static AjPStr entryid=NULL;
  static AjPStr label=NULL;

  if(!source){
    source    = ajStrNewC("SWISSPROT");
    feature   = ajStrNewL(10);
    start     = ajStrNewL(8);
    end       = ajStrNewL(8);
    desc      = ajStrNewL(81);
  }
  
  /* reg exp has already checked that line starts with FT */
  
  /*STR MUST BE AT least 22 chars long (must has a to)*/
  if(ajStrLen(line) < 22)
    return gf;
  
  /* Get Keyname ---> feature in gff */
  ajStrAssSub(&feature,line,5,12);
  ajStrChomp(&feature); 
  if(!ajStrLen(feature)){
    /*
      Actually test for description. If it exist then add to
      static gf's already existing description.
    */
    if(!gf)
      return NULL;
    ajStrAssC(&desc,"");
    ajStrAssSub(&desc,line,34,74);
    ajStrChomp(&desc); 
    if(ajStrLen(desc)){
      ajStrApp(&(gf->desc),desc);
      ajStrDel(&desc);
      return NULL; /* entry added already */
    }
    else {
      ajStrDel(&desc);
      return NULL;
    }
  }
  
  /* Get the start point */
  ajStrAssSub(&start,line,14,19);
  ajStrChomp(&start); 
  if(!ajStrLen(start)){
    return NULL;
  }

  /* Get the end point */
  ajStrAssSub(&end,line,21,26);
  ajStrChomp(&end); 
  if(!ajStrLen(end)){
    return NULL;
  }

  /* Get the description THIS IS OPTIONAL */
  ajStrAssC(&desc,"");
  ajStrAssSub(&desc,line,34,74);
  ajStrChomp(&desc); 

  val = featGetPos(&start,&Start);
  if(val==-1)
    return NULL;
  else if(val)
    ajFeatSetFlag(&flags,FEATFLAG_START_BEFORE_SEQ);

  val = featGetPos(&end,   &End);
  if(val==-1)
    return NULL;
  else if (val)
    ajFeatSetFlag(&flags,FEATFLAG_END_AFTER_SEQ);

  ajDebug("flags = %d\n",flags);

  gf = featFeatureNew( thys,
		     source, 
		     feature,
		     Start, End,
		     fscore,
		     strand,
		     frame,
		     desc,0,0,0, entryid, label, flags ) ;
  

  return gf;
}

/* @funcstatic featEmblFromLine ***********************************************
**
** Converts an input EMBL format line into a feature
**
** @param [r] thys     [AjPFeattable] Feature table
** @param [r] origline [AjPStr] Input line
** @param [r] saveloc  [AjPStr*] Continued location
** @param [r] saveline [AjPStr*] Continued tag-value pairs
** @return [AjPFeature] New feature
** @@
******************************************************************************/

static AjPFeature featEmblFromLine ( AjPFeattable thys,
				     AjPStr origline,
				     AjPStr* saveloc,
				     AjPStr* saveline)
{
  static AjPFeature gf    = NULL ;      /* so tag-values can be added LATER */
  static AjPStr source    = NULL;
  static AjPStr feature   = NULL;
  static AjPStr line      = NULL;
  static AjPStr temp=NULL;
  AjBool newft = ajFalse;

  if(!source)
    source = ajStrNewC("EMBL");

  ajStrAssS(&line,origline);              /* As BufferFile cannot be edited */

  if (origline)
    newft = ajRegExec(EmblRegexNew, line);

  /*
    ajDebug ("+ newft: %B\n+ line '%S'\n",
	   newft, line);
  */

  if ((newft || !origline)) { /* process the last feature */
    ajDebug ("++ saveloc '%S'\n+ saveline '%S'\n",
	     *saveloc, *saveline);
    if (ajStrLen(*saveloc)) {
      gf = featEmblProcess (thys, feature, source, saveloc, saveline);
    }
    else {
      gf = NULL;
    }
    ajStrDelReuse(saveloc);
    ajStrDelReuse(saveline);
  }

  if (newft) {	/* if new feature initialise for it */
    ajRegSubI (EmblRegexNew, 1, &feature);
    ajRegSubI (EmblRegexNew, 2, saveloc);
    ajRegSubI (EmblRegexNew, 3, saveline);
    ajStrChomp(&feature);
    return gf;
  }
  else if(!ajStrLen(*saveline)) { /* more location? */
    ajRegExec(EmblRegexNext, line); 
    ajRegSubI(EmblRegexNext, 1, &temp);
    if (temp)
      ajStrApp (saveloc, temp);
    ajRegSubI(EmblRegexNext, 2, &temp);
    if (temp)
      ajStrApp (saveline, temp);
  }
  else {			/* tag-values */
    ajRegExec(EmblRegexTv, line); 
    ajRegSubI(EmblRegexTv, 1, &temp);
    ajStrApp (saveline, temp);
  }

  return gf;
}

/* @funcstatic featEmblProcess ************************************************
**
** Processes one feature location and qualifier tags for EMBL
**
** @param [r] thys [AjPFeattable] Feature table
** @param [u] feature [AjPStr] Feature type key
** @param [u] source [AjPStr] Feature table source
** @param [r] loc [AjPStr*] Feature location
** @param [r] tags [AjPStr*] Feature qualifier tags string
** @return [AjPFeature] Feature as inserted into the feature table
** @@
******************************************************************************/

static AjPFeature featEmblProcess  ( AjPFeattable thys, AjPStr feature,
				     AjPStr source,
				     AjPStr* loc, AjPStr* tags) {

  AjPFeature ret = NULL;
  AjPFeature gf = NULL;
  AjPStr tag=NULL;
  AjPStr val=NULL;
  AjPStr opnam=NULL;
  AjPStr opval=NULL;
  AjPStr tmpstr=NULL;
  AjBool Fwd = ajTrue;
  AjBool LocFwd = ajTrue;
  AjPStr begstr=NULL;
  AjPStr delstr=NULL;
  AjPStr endstr=NULL;
  AjPStr locstr=NULL;
  AjBool Simple=ajFalse;
  AjBool BegBound=ajFalse;
  AjBool EndBound=ajFalse;
  ajint BegNum=0;
  ajint EndNum=0;
  ajint Beg2=0;
  ajint End2=0;
  static AjPStr entryid=NULL;
  static AjPStr label=NULL;
  AjBool Between = ajFalse;
  AjBool Join = ajFalse;
  AjBool Group = ajFalse;
  AjBool Order = ajFalse;
  AjBool OneOf = ajFalse;
  ajint Flags;
  char Strand = '+';
  AjBool Mother = ajTrue;
  ajint Frame = 0;
  float Score = 0.0;
  AjBool HasOper = ajFalse;
  AjBool RemoteId = ajFalse;
  AjBool IsLabel = ajFalse;
  ajint Exon = 0;

  ajStrCleanWhite(loc);	/* no white space needed */
  ajStrClean(tags);		/* single spaces only */
  ajDebug("cleaned feat loc: '%S'\n            tags: '%S'\n",
	  *loc, *tags);


  ajDebug("Location '%S'\n", *loc);
  ajStrAssS (&opval, *loc);
  if (ajRegExec(EmblRegexLocMulti, opval)) {
    ajDebug("Multiple locations, test operator(s)\n");
     while (ajStrLen(opval) && ajRegExec (EmblRegexOperOut, opval)) {
      ajRegSubI(EmblRegexOperOut, 1, &opnam);
      ajRegSubI(EmblRegexOperOut, 2, &tmpstr);
      ajDebug("OperOut %S( '%S' )\n", opnam, tmpstr);
      if (ajStrMatchCaseC(opnam, "complement")) {
	Fwd = !Fwd;
      }
      if (ajStrMatchCaseC(opnam, "one_of")) {
	OneOf = ajTrue;
      }
      if (ajStrMatchCaseC(opnam, "join")) {
	Join = ajTrue;
      }
      if (ajStrMatchCaseC(opnam, "order")) {
	Order = ajTrue;
      }
      if (ajStrMatchCaseC(opnam, "group")) {
	Group = ajTrue;
      }
      ajStrAssS (&opval, tmpstr);
    }
  }

  while (ajStrLen(opval)) {
    LocFwd = Fwd;
    BegBound = ajFalse;
    EndBound = ajFalse;
    Simple = ajFalse;
    Between = ajFalse;
    BegNum = EndNum = Beg2 = End2 = 0;
    HasOper = ajFalse;
    RemoteId = ajFalse;
    IsLabel = ajFalse;
    ajStrDelReuse(&entryid);
    ajStrDelReuse(&label);
    if (ajRegExec (EmblRegexOperIn, opval)) {
      ajRegSubI(EmblRegexOperIn, 1, &opnam);
      ajRegSubI(EmblRegexOperIn, 2, &locstr);
      ajDebug("OperIn %S( '%S' )\n", opnam, locstr);
      if (ajStrMatchCaseC(opnam, "complement"))
	LocFwd = !LocFwd;
      ajRegPost (EmblRegexOperIn, &tmpstr);
      ajStrAssS (&opval, tmpstr);
      ajDebug("rest: '%S'\n", opval);
      HasOper = ajTrue;
    }
    else {
      ajStrAssS(&locstr, opval);
    }
    if  (ajRegExec (EmblRegexOperNone, locstr)) { /* one exon */
      ajRegSubI(EmblRegexOperNone, 2, &entryid); /* if any */
      ajRegSubI(EmblRegexOperNone, 3, &tmpstr);	/* position (or label) */
      ajDebug("OperNone '%S' \n", tmpstr);
      if (ajStrLen(entryid)) {
	ajDebug("External entryid '%S'\n", entryid);
	RemoteId = ajTrue;
      }
      if (ajRegExec(EmblRegexLoc, tmpstr)) { /* xxx..xxx or xxx^xxx */
	ajRegSubI(EmblRegexLoc, 1, &begstr);
	ajRegSubI(EmblRegexLoc, 2, &delstr);
	if (ajStrMatchC(delstr, "^"))
	  Between = ajTrue;
	ajRegSubI(EmblRegexLoc, 3, &endstr);
	ajDebug("Location: '%S' '%S' '%S'\n", begstr, delstr, endstr);
      }
      else {
	ajStrAssS(&begstr, tmpstr);
	ajStrAssS(&endstr, begstr);
	Simple = ajTrue;
	ajDebug("Location: label='%S'\n", begstr);
      }

      ajRegPost (EmblRegexOperNone, &tmpstr);
      if (!HasOper) ajStrAssS (&opval, tmpstr);

      if (ajRegExec(EmblRegexLocNum, begstr)) {
	ajRegSubI(EmblRegexLocNum, 1, &tmpstr);
	if (ajStrLen(tmpstr))
	  BegBound=ajTrue;
	ajRegSubI(EmblRegexLocNum, 2, &tmpstr);
	ajStrToInt(tmpstr, &BegNum);
	ajDebug ("Begin '%S' %d  Bound: %B\n", begstr, BegNum, BegBound);
      }
      else if (ajRegExec(EmblRegexLocRange, begstr)) {
	ajRegSubI(EmblRegexLocRange, 1, &tmpstr);
	ajStrToInt(tmpstr, &BegNum);
	ajRegSubI(EmblRegexLocRange, 2, &tmpstr);
	ajStrToInt(tmpstr, &Beg2);
	ajDebug ("Begin range (%d . %d)\n", BegNum, Beg2);
      }
      else {
	ajDebug("Begin is a label '%S'\n", begstr);
	IsLabel = ajTrue;
	Simple = ajTrue;
	ajStrAssS (&label, begstr);
      }

      if (ajRegExec(EmblRegexLocNum, endstr)) {
	ajRegSubI(EmblRegexLocNum, 1, &tmpstr);
	if (ajStrLen(tmpstr))
	  EndBound=ajTrue;
	ajRegSubI(EmblRegexLocNum, 2, &tmpstr);
	ajStrToInt(tmpstr, &EndNum);
	ajDebug ("  End '%S' %d  Bound: %B\n", endstr, EndNum, EndBound);
      }
      else if (ajRegExec(EmblRegexLocRange, endstr)) {
	ajRegSubI(EmblRegexLocRange, 1, &tmpstr);
	ajStrToInt(tmpstr, &End2);
	ajRegSubI(EmblRegexLocRange, 2, &tmpstr);
	ajStrToInt(tmpstr, &EndNum);
	ajDebug ("  End range (%d . %d)\n", End2, EndNum);
      }
      else {
	IsLabel = ajTrue;
	Simple = ajTrue;
	ajStrAssS (&label, endstr);
	ajDebug("  End is a label '%S'\n", endstr);
      }
    }
    else {
      ajErr ("Unable to parse location:\n'%S'", opval);
    }

    /* location has been read in, now store it */

    Flags = 0;
    if (LocFwd) Strand = '+';
    else Strand = '-';

    if (Mother) {
      if (!Fwd) Flags |= FEATFLAG_COMPLEMENT_MAIN;
    }
    else {
      Flags |= FEATFLAG_CHILD;
    }
    if (Join || Order || Group || OneOf) Flags |= FEATFLAG_MULTIPLE;
    if (Group) Flags |= FEATFLAG_GROUP;
    if (Order) Flags |= FEATFLAG_ORDER;
    if (OneOf) Flags |= FEATFLAG_ONEOF;

    if (Simple) Flags |= FEATFLAG_POINT;
    if (Between) Flags |= FEATFLAG_BETWEEN_SEQ;
    if (End2) Flags |= FEATFLAG_END_TWO;
    if (Beg2) Flags |= FEATFLAG_START_TWO;
    if (BegBound) Flags |= FEATFLAG_START_BEFORE_SEQ;
    if (EndBound) Flags |= FEATFLAG_END_AFTER_SEQ;
    if (RemoteId) Flags |= FEATFLAG_REMOTEID;
    if (IsLabel) Flags |= FEATFLAG_LABEL;

    ajDebug("Calling featFeatureNew, Flags: %x\n", Flags);

    gf = featFeatureNew ( thys,
			source,	/* source sequence */
			feature,
			BegNum, EndNum,
			Score,
			Strand,
			Frame,
			NULL,	/* description, see tags */
			Exon, Beg2, End2, entryid, label, Flags ) ;
    if (Mother)
      ret = gf;
    Mother = ajFalse;
    /*if (OneOf) break;*/
  }

  while (ajStrLen(*tags) && ajRegExec (EmblRegexTvTag, *tags)) {

    /* first process quoted values, which can look like multiple values */
    /* the expressions cheat - they include the first quote because we
       always need it but exclude the last quote so we can add '\' for
       internal quotes, and append '"' for the last quote */

    if (ajRegExec (EmblRegexTvTagQuote, *tags)) { /* /tag="val" */
      ajRegSubI (EmblRegexTvTagQuote, 1, &tag);
      ajRegSubI (EmblRegexTvTagQuote, 2, &val);
      ajRegPost (EmblRegexTvTagQuote, &tmpstr);
      ajStrAssS (tags, tmpstr);

      /* internal quotes are "" and turn into two strings */
      while (ajRegExec (EmblRegexTvTagQuote2, *tags)) { /* "quoted ""val""" */
       ajRegSubI (EmblRegexTvTagQuote2, 1, &tmpstr);
       ajStrAppC (&val, "\\");	/* escape the internal leading quote */
       ajStrApp (&val, tmpstr);
       ajRegPost (EmblRegexTvTagQuote2, &tmpstr);
       ajStrAssS (tags, tmpstr);
      }
      ajStrAppC (&val, "\"");	/* add the closing quote */
      ajDebug ("(a) /%S=%S\n", tag, val);
    }
    else {
      ajRegSubI (EmblRegexTvTag, 1, &tag);
      ajRegSubI (EmblRegexTvTag, 3, &val);
      if (ajStrLen(val))
	ajDebug ("(b) /%S='%S'\n", tag, val);
      else
	ajDebug ("(c) /%S\n", tag);
      ajRegPost (EmblRegexTvTag, &tmpstr);
      ajStrAssS (tags, tmpstr);
    }
    ajFeatTagSet (ret, tag, val);
  }

  ajStrDel (&tmpstr);
  ajStrDel (&val);
  ajStrDel (&tag);
  ajStrDel (&begstr);
  ajStrDel (&delstr);
  ajStrDel (&opnam);
  ajStrDel (&opval);
  ajStrDel (&locstr);
  ajStrDel (&endstr);

  return ret;
}

/* @funcstatic featGffFromLine ************************************************
**
** Converts an input GFF format line into a feature
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] line [AjPStr] Input line
** @return [AjPFeature] New feature
** @@
******************************************************************************/

static AjPFeature featGffFromLine ( AjPFeattable thys, AjPStr line,
				    float version)
{
    AjPFeature gf    = NULL ;
    static AjPStrTok split  = NULL  ;
    static AjPStr
      seqname   = NULL,
      source    = NULL,
      feature   = NULL,
      start     = NULL,
      end       = NULL,
      score     = NULL,
      strandstr = NULL,
      framestr  = NULL,
      tagvalue  = NULL ;
    ajint Start=0, End=0;
    float fscore = 0.0;
    static AjPStr entryid=NULL;
    static AjPStr label=NULL;

    if(!ajStrLen(line)) return NULL ;

    split = ajStrTokenInit (line, "\t") ;

    if( !ajStrToken (&seqname, &split, NULL)) {           /* seqname */
        goto Error; 
    } else if( !ajStrToken (&source, &split, NULL)) {     /* source  */
        goto Error; 
    } else if( !ajStrToken (&feature, &split, NULL)) {    /* feature */
        goto Error; 
    } else if( !ajStrToken (&start, &split, NULL)) {      /* start   */ 
        goto Error; 
    } else if( !ajStrToken (&end, &split, NULL)) {        /* end     */
        goto Error; 
    } else if( !ajStrToken (&score, &split, NULL)) {      /* score   */
        goto Error; 
    } else if( !ajStrToken (&strandstr, &split, NULL)) {  /* strand  */
        goto Error; 
    } else if( !ajStrToken (&framestr, &split, NULL)) {   /* frame   */
        goto Error; 
    } else {

      /* feature object construction
	 and group tag */

        char   strand ;
        ajint    frame ;
        AjPStr          groupfield = NULL ;

        if(!ajStrToInt (start, &Start))
           Start = 0 ;
        if(!ajStrToInt (end,   &End))
           End   = 0 ;
        if(!ajStrToFloat (score,   &fscore))
           fscore   = 0.0 ;

        if(!ajStrCmpC(strandstr,"+")) {
           strand = '+' ;
        } else if( !ajStrCmpC(strandstr,"-")) {
           strand = '-' ;
        } else {
           strand = '\0' ;	/* change to \0 later */
        }

        if(!ajStrCmpC(framestr,"0")) {
           frame = 1 ;
        } else if( !ajStrCmpC(framestr,"1")) {
           frame = 2 ;
        } else if( !ajStrCmpC(framestr,"2")) {
           frame = 3 ;
        } else {
           frame = 0 ;
        }

        gf = featFeatureNew( thys,
                           source, 
                           feature,
                           Start, End,
                           fscore,
                           strand,
                           frame,
			   NULL,0,0,0, entryid, label, 0 ) ;

        if( ajStrTokenRest(&groupfield, &split))
           GFFProcessTagValues( gf, thys, groupfield, version) ;

	ajStrDel(&groupfield) ; 
	ajStrTokenClear(&split) ;
	
	(void) ajStrDelReuse(&seqname) ;
	(void) ajStrDelReuse(&source) ;
	(void) ajStrDelReuse(&feature) ;
	(void) ajStrDelReuse(&start) ;
	(void) ajStrDelReuse(&end) ;
	(void) ajStrDelReuse(&score) ;
	(void) ajStrDelReuse(&strandstr) ;
	(void) ajStrDelReuse(&framestr) ;
	(void) ajStrDelReuse(&tagvalue) ;
	
	return gf ;
    }
    
Error:

    ajStrTokenClear(&split) ;

    (void) ajStrDelReuse(&seqname) ;
    (void) ajStrDelReuse(&source) ;
    (void) ajStrDelReuse(&feature) ;
    (void) ajStrDelReuse(&start) ;
    (void) ajStrDelReuse(&end) ;
    (void) ajStrDelReuse(&score) ;
    (void) ajStrDelReuse(&strandstr) ;
    (void) ajStrDelReuse(&framestr) ;
    (void) ajStrDelReuse(&tagvalue) ;

    return gf ;
}

/* @funcstatic featReadGff ****************************************************
**
** Read input file in GFF format
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFileBuff] Input buffered file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featReadGff ( AjPFeattable thys, AjPFileBuff file)
{
  static AjPStr line  = NULL ;
  AjBool found = ajFalse ;
  float version = 2.0;

  if (!FeatTypeTableGff) {
    FeatTypeTableGff = ajStrTableNewCase(200); /* for the types + tags*/
    FeatTagsTableGff = ajStrTableNewCase(200); /* for the tags + valuetype */

    featVocabRead ("gff", FeatTypeTableGff, FeatTagsTableGff);
    ajDebug ("Tables gff Type: %x Tags: %x\n",
	     FeatTypeTableGff, FeatTagsTableGff);
  }

  ajDebug("featReadGff..........\n");

  while( ajFileBuffGet (file, &line) ) {

    (void) ajStrChomp(&line) ;

    /* Header information */

    if(ajRegExec(GffRegexblankline, line)) {
      ; /* ignore */
    }
    else if(ajRegExec(GffRegexversion,line)) {
      AjPStr verstr = NULL ;
      ajRegSubI (GffRegexversion, 1, &verstr);
      (void) ajStrToFloat (verstr, &version);
      ajStrDel(&verstr);
    }
/*
//    else if(ajRegExec(GffRegexdate,line)) {
//      AjPStr year  = NULL ;
//      AjPStr month = NULL ;
//      AjPStr day   = NULL ;
//      ajint nYear, nMonth, nDay ;
//      ajRegSubI (GffRegexdate, 1, &year); 
//      ajRegSubI (GffRegexdate, 2, &month); 
//      ajRegSubI (GffRegexdate, 3, &day);
//      (void) ajStrToInt (year,  &nYear);
//      (void) ajStrToInt (month, &nMonth);
//      (void) ajStrToInt (day,   &nDay);
//      ajStrDel(&year);
//      ajStrDel(&month);
//      ajStrDel(&day);
//    }
*/
    else if(ajRegExec(GffRegexregion,line)) {
      AjPStr start = NULL ;
      AjPStr end   = NULL ;
      ajRegSubI (GffRegexregion, 2, &start); 
      ajRegSubI (GffRegexregion, 3, &end);
      (void) ajStrToInt (start, &(thys->Start));
      (void) ajStrToInt (end,   &(thys->End));
      ajStrDel(&start);
      ajStrDel(&end);
    }
    else if(ajRegExec(GffRegexcomment,line)) {
      ; /* ignore for now... could store them in 
	   ajFeattable for future reference though?...*/
    }

    /* the real feature stuff */

    else {			/* must be a real feature at last !! */
      if(featGffFromLine(thys, line, version))  /* does the ajFeattabAdd */
	found = ajTrue ;
    }

    ajStrDelReuse(&line);
  }
  return found ;
}

/* @func ajFeattableWriteGff **************************************************
**
** Write feature table in GFF format
**
** @param [r] Feattab [AjPFeattable] feature table
** @param [r] file [AjPFile] Output file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajFeattableWriteGff (AjPFeattable Feattab, AjPFile file)
{
  AjIList    iter = NULL ;
  AjPFeature gf   = NULL ;

  if (!FeatTypeTableGff) {
    FeatTypeTableGff = ajStrTableNewCase(200); /* for the types + tags*/
    FeatTagsTableGff = ajStrTableNewCase(200); /* for the tags + valuetype */

    featVocabRead ("gff", FeatTypeTableGff, FeatTagsTableGff);
    ajDebug ("Tables gff Type: %x Tags: %x\n",
	     FeatTypeTableGff, FeatTagsTableGff);
  }

  /* Check arguments */
  ajDebug ("ajFeattableWriteGff Checking arguments\n");
  if(file == NULL) AJRAISE(Null_IO_Handle) ;
  
  /* Print header first */
  (void) ajFmtPrintF (file, "##gff-version 2.0\n") ;

  (void) ajFmtPrintF (file, "##date %D\n", ajTimeTodayF("GFF")) ;

  /* type defaults to DNA */

  (void) ajFmtPrintF (file, "##Type %s %S\n", "DNA", Feattab->Name) ;

  /*
  (void) ajFmtPrintF (file, "##sequence-region %s %d %d\n",
		      ajStrStr(Feattab->Name),
		      Feattab->Start,
		      Feattab->End) ;
  */

  /* For all features... relatively simple because internal structures
     are deliberately styled on GFF */

  if (Feattab->Features) {
    iter = ajListIter(Feattab->Features) ;
    while(ajListIterMore(iter)) {
      gf = ajListIterNext (iter) ;
      featDumpGff(gf, Feattab, file) ;
    }
    ajListIterFree(iter) ;
  }
   
  return ajTrue ;
}

/* @funcstatic featRegInitAcedb ***********************************************
**
** Initialize regular expressions and data structures for ajFeat  format
**
** @return [AjBool] ajTrue if successful
** @@
******************************************************************************/

static AjBool featRegInitAcedb (void) 
{
  featInit();

  ajDebug("Hello Acedb\n");
  return ajFalse;
}
/* @funcstatic featRegInitEmbl ************************************************
**
** Initialize regular expressions and data structures for
** EMBL/GenBank/DDBJ format
**
** @return [AjBool] ajTrue if successful
** @@
******************************************************************************/

static AjBool featRegInitEmbl (void) {

  if (FeatInitEmbl)
    return ajTrue;

  featInit();

  FeatTypeTableEmbl = ajStrTableNewCase(200); /* for the types + tags*/
  FeatTagsTableEmbl = ajStrTableNewCase(200); /* for the tags + valuetype */

  featVocabRead ("embl", FeatTypeTableEmbl, FeatTagsTableEmbl);
  ajDebug ("Tables embl Type: %x Tags: %x\n",
	   FeatTypeTableEmbl, FeatTagsTableEmbl);

  EmblRegexLoc  = ajRegCompC("^(.*)([.][.]|\\^)(.*)$") ; /* start+end */
  EmblRegexLocMulti  = ajRegCompC(",") ; /* multiple location */
  EmblRegexLocNum  = ajRegCompC("^([<>]?)([0-9]+)$") ; /* <n >n */
  EmblRegexLocRange  = ajRegCompC("^[(]([0-9]+)[.]([0-9]+)[)]$") ; /* (n.n) */
  EmblRegexNew = ajRegCompC("^..   ([^ ]+) +([^/]*)(/.*)?") ; /* start of new feature */
  EmblRegexNext = ajRegCompC("^..    +([^/]*)(/.*)?") ; /* start of new feature */
  EmblRegexOperIn  = ajRegCompC("^([a-zA-Z_]+)[(]([^)]+)[)],?") ; /* oper() internal*/
  EmblRegexOperNone  = ajRegCompC("^(([^:,]+):)?([^,]+),?") ; /* simple location */
  EmblRegexOperOut = ajRegCompC("^([a-zA-Z_]+)[(](([^[()]+([(][^()]+[)])?)+)[)]$") ; /* oper() outside*/
  EmblRegexTv = ajRegCompC("^..    +( .*)") ; /* start of new feature */
  EmblRegexTvTag = ajRegCompC("^ */([^/= ]+)(=([^/ ]+))?") ; /* tag=val */
  /* quoted strings include the first quote, but not the last */
  EmblRegexTvTagQuote = ajRegCompC("^ */([^/\"= ]+)=(\"[^\"]*)\"") ; /* quoted string */
  EmblRegexTvTagQuote2 = ajRegCompC("^(\"[^\"]*)\"") ; /* more string */

  FeatInitEmbl = ajTrue;

  return ajTrue;
}

/* @funcstatic featRegInitSwiss ***********************************************
**
** Initialize regular expressions and data structures for ajFeat#
** SwissProt format
**
** @return [AjBool] ajTrue if successful
** @@
******************************************************************************/

static AjBool featRegInitSwiss (void) 
{
  if (FeatInitSwiss)
    return ajTrue;

  featInit();

  FeatTypeTableSwiss = ajStrTableNewCase(200); /* for the types + tags*/
  FeatTagsTableSwiss = ajStrTableNewCase(200); /* for the tags + valuetype */

  featVocabRead ("embl", FeatTypeTableSwiss, FeatTagsTableSwiss);
  ajDebug ("Tables embl Type: %x Tags: %x\n",
	   FeatTypeTableSwiss, FeatTagsTableSwiss);

  ajDebug ("featRegInitSwiss Compiling featDumpSwiss() regexps\n");
  if (!SwRegexFeature)
    SwRegexFeature = ajRegCompC("^FT(.*)$") ;

  FeatInitSwiss = ajTrue;

  return ajTrue;
}

/* @funcstatic featRegInitGff *************************************************
**
** Initialize regular expressions and data structures for ajFeat GFF format
**
** @return [AjBool] ajTrue if successful
** @@
******************************************************************************/

static AjBool featRegInitGff (void) {
    /* Setup any global static runtime resources here
       for example, regular expression compilation calls */

  if (FeatInitGff)
    return ajTrue;

  featInit();


  ajDebug ("featRegInitGff Compiling featDumpGff() regexps\n");
  GffRegexNumeric = ajRegCompC("^[\\+-]?[0-9]+\\.?[0-9]*$") ;

  ajDebug ("featRegInitGff  Compiling featReadGff() regexps\n");
  GffRegexblankline = ajRegCompC("^[ ]*$") ;
  GffRegexversion   = ajRegCompC("^##gff-version[ ]+([0-9]+)") ;
  GffRegexdate      = ajRegCompC("^##date[ ]+([0-9][0-9][0-9][0-9])-([0-9][0-9]?)-([0-9][0-9]?)") ;
  GffRegexregion    = ajRegCompC("^##sequence-region[ ]+([0-9a-zA-Z]+)[ ]+([\\+-]?[0-9]+)[ ]+([\\+-]?[0-9]+)") ;
  GffRegexcomment   = ajRegCompC ("^#[ ]*(.*)") ;

  ajDebug ("featRegInitGff  Compiling GFFProcessTagValues() regexps\n");

  /*
    GffTvRegex1 = ajRegCompC ("^\"+([ ]+|;|$)"); 
    GffTvRegex2 = ajRegCompC ("^\\\\(\")"); 
    GffTvRegex3 = ajRegCompC ("^([^\"]*)"); 
    GffTvRegex4 = ajRegCompC ("^\""); 
    GffTvRegex5 = ajRegCompC ("^;"); 
    GffTvRegex6 = ajRegCompC ("^([^; ]+)[ ]*"); 
    GffTvRegex7 = ajRegCompC ("^[ ]+"); 
    GffTvRegex8 = ajRegCompC ("^(.+) ([0-9]+) ([0-9]+)$"); 
  */

  GffRegexTvTagval = ajRegCompC (" *([^ ]+) *((\"(\\.|[^\\\"])*\"|[^;]+)*)(;|$)"); /* "tag name */

  FeatInitGff = ajTrue;

  return ajTrue;
}

/* @funcstatic featDelRegAcedb ************************************************
**
** Cleanup and exit routines. Free and destroy regular expressions
**
** @return [AjBool] ajFalse if unsuccesful
** @@
******************************************************************************/
static AjBool featDelRegAcedb(void) {

  if (!FeatInitAcedb)
    return ajTrue;

  ajStrTableFree(&FeatTypeTableAcedb);
  ajStrTableFree(&FeatTagsTableAcedb);

  FeatInitAcedb = ajFalse;

  return ajFalse;
}

/* @funcstatic featDelRegEmbl *************************************************
**
** Cleanup and exit routines. Free and destroy regular expressions
**
** @return [AjBool] ajFalse if unsuccesful
** @@
******************************************************************************/
static AjBool featDelRegEmbl(void) {

  if (!FeatInitEmbl)
    return ajTrue;

  ajRegFree(&EmblRegexLoc);
  ajRegFree(&EmblRegexLocMulti);
  ajRegFree(&EmblRegexLocNum);
  ajRegFree(&EmblRegexLocRange);
  ajRegFree(&EmblRegexNew);
  ajRegFree(&EmblRegexNext);
  ajRegFree(&EmblRegexOperIn);
  ajRegFree(&EmblRegexOperNone);
  ajRegFree(&EmblRegexOperOut);
  ajRegFree(&EmblRegexTv);
  ajRegFree(&EmblRegexTvTag);
  ajRegFree(&EmblRegexTvTagQuote);
  ajRegFree(&EmblRegexTvTagQuote2);

  ajStrTableFree(&FeatTypeTableEmbl);
  ajStrTableFree(&FeatTagsTableEmbl);

  FeatInitEmbl = ajFalse;
  return ajTrue;
}

/* @funcstatic featDelRegSwiss ************************************************
**
** Cleanup and exit routines. Free and destroy regular expressions
**
** @return [AjBool] ajFalse if unsuccesful
** @@
******************************************************************************/
static AjBool featDelRegSwiss(void) {

  if (!FeatInitSwiss)
    return ajTrue;

  ajRegFree(&SwRegexFeature) ;

  ajStrTableFree(&FeatTypeTableSwiss);
  ajStrTableFree(&FeatTagsTableSwiss);

  FeatInitSwiss = ajFalse;
  return ajTrue;
}

/* @funcstatic featDelRegGff **************************************************
**
** Cleanup and exit routines. Free and destroy regular expressions
**
** @return [AjBool] ajFalse if unsuccesful
** @@
******************************************************************************/
static AjBool featDelRegGff(void)
{

  if (!FeatInitGff)
    return ajTrue;

    /* Clean-up any global static runtime resources here
       for example, regular expression pattern variables */

  ajRegFree(&GffRegexNumeric) ;
  ajRegFree(&GffRegexblankline) ;
  ajRegFree(&GffRegexversion) ;
  ajRegFree(&GffRegexdate) ;
  ajRegFree(&GffRegexregion) ;
  ajRegFree(&GffRegexcomment) ;
  ajRegFree(&GffRegexTvTagval) ;

  ajStrTableFree(&FeatTypeTableGff);
  ajStrTableFree(&FeatTagsTableGff);

  FeatInitGff = ajFalse;

  return ajTrue;
}

/* @func ajFeattableWriteDdbj *************************************************
**
** Write a feature table in DDBJ format.
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFile] Output file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajFeattableWriteDdbj (AjPFeattable thys, AjPFile file) {
  return feattableWriteEmbl(thys,file,ajFalse);
}

/* @func ajFeattableWriteEmbl *************************************************
**
** Write a feature table in EMBL format.
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFile] Output file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajFeattableWriteEmbl (AjPFeattable thys, AjPFile file) {
  return feattableWriteEmbl(thys,file,ajTrue);
}

/* @func ajFeattableWriteGenbank **********************************************
**
** Write a feature table in GenBank format.
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFile] Output file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajFeattableWriteGenbank (AjPFeattable thys, AjPFile file) {
  return feattableWriteEmbl(thys,file,ajFalse);
}

/* @funcstatic feattableWriteEmbl *********************************************
**
** Write a feature table in EMBL format.
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFile] Output file
** @param [r] IsEmbl [AjBool] ajTrue for EMBL (different line prefix)
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool feattableWriteEmbl (AjPFeattable thys, AjPFile file,
				  AjBool IsEmbl) {
  AjIList    iter = NULL ;
  AjPFeature gf   = NULL ;
  AjPFeature gfprev   = NULL ;
  AjBool join=ajFalse;
  AjBool whole = ajFalse;           /* has "complement(" been added */
  AjPStr location = NULL;        /* location list as a string */
  AjPStr temp=NULL;
  AjPStr pos=NULL;
  ajint oldgroup = -1;

  /* Check arguments */

  ajDebug ("feattableWriteEmbl Checking arguments\n");
  if(file == NULL) AJRAISE(Null_IO_Handle) ;

  /* feature table heading */

  if (IsEmbl) {
    ajFmtPrintF (file, "FH   Key             Location/Qualifiers\n");
    ajFmtPrintF (file, "FH\n");
  }
  else {
    ajFmtPrintF (file, "FEATURES             Location/Qualifiers\n");
  }

  location = ajStrNewL(80);
  temp = ajStrNewL(80);
  pos = ajStrNewL(80);

  /* For all features... */
  
  ajListSort(thys->Features,*featCompByGroup);

  /* embl can only dump in the order read in*/
  /* other option is to dump separately ie. CDS join(1..4,7..8)*/
  /* would become CDS 1..4 and       */
  /*  CDS 7..8      on separate lines */ 

  if (thys->Features) {
    iter = ajListIter(thys->Features) ;
    while(ajListIterMore(iter)) {
      gf = ajListIterNext (iter) ;

      if((oldgroup != gf->Group) && gfprev){ /* previous location ready */
	if(join) {
	  ajStrAppC(&location,")");      /* close bracket for join */
	  ajDebug("join: closing ')' appended\n");
	}
	if(whole){
	  ajStrInsertC(&location,0,"complement(");
	  ajStrAppC(&location,")");
	  ajDebug("wrap with complement(), reset whole %b to N\n", whole);
	  whole = ajFalse;
	}
	  
	ajDebug("calling featDumpEmbl for gfprev\n");
	ajDebug("location: '%S'\n", location);
	featDumpEmbl(gfprev,location, file, IsEmbl) ; /* gfprev has tag data */
	
	/* reset the values from previous */
	ajDebug("reset location\n");
	ajStrClear(&location);
        ajDebug("reset join  %b to N\n", join);
	join = ajFalse;  
      }

      oldgroup = gf->Group;

      /* process the new gf */

      ajDebug("\n'%S' group: %d exon: %d flags:%x tags: %d\n",
	      gf->Type, gf->Group,gf->Exon, gf->Flags, ajListLength(gf->Tags));

      if(gf->Flags & FEATFLAG_COMPLEMENT_MAIN) {
	ajDebug("set2 whole %b to Y\n", whole);
	whole =ajTrue;
      }

      if(ajStrLen(location)) { /* one location already there */
	if(!join){
          ajDebug("insert 'join(', set join Y\n");
	  if (gf->Flags & FEATFLAG_GROUP)
	    ajStrInsertC(&location,0,"group(");
	  else if (gf->Flags & FEATFLAG_ORDER)
	    ajStrInsertC(&location,0,"order(");
	  else if (gf->Flags & FEATFLAG_ONEOF)
	    ajStrInsertC(&location,0,"one_of(");
	  else
	    ajStrInsertC(&location,0,"join(");
	  join = ajTrue;
	}
	ajStrAppC(&location,",");
        ajDebug("append ','\n");
      }
      
      ajStrClear(&temp);
      ajStrClear(&pos);

      if(gf->Flags & FEATFLAG_REMOTEID){
	ajFmtPrintAppS(&pos,"%S:",gf->Remote);
        ajDebug("remote: %S\n", gf->Remote);
      }

      if(gf->Flags & FEATFLAG_LABEL){
	ajFmtPrintAppS(&pos,"%S",gf->Label);
        ajDebug("label: %S\n", gf->Label);
       }
      else if(gf->Flags & FEATFLAG_START_BEFORE_SEQ){
	ajFmtPrintAppS(&pos,"<%d",gf->Start);
        ajDebug("<start\n");
      }
      else if(gf->Flags & FEATFLAG_START_TWO){
	ajFmtPrintAppS(&pos,"(%d.%d)",gf->Start,gf->Start2);
        ajDebug("start2 (%d.%d)\n", gf->Start, gf->Start2);
     }
      else{
	ajFmtPrintAppS(&pos,"%d",gf->Start);
        ajDebug("start\n");
      }
      
      if(!(gf->Flags & FEATFLAG_POINT)){
	if(gf->Flags & FEATFLAG_BETWEEN_SEQ){
	  ajFmtPrintAppS(&pos,"^%d",gf->End);
          ajDebug("between ^end\n");
	}
	else if(gf->Flags & FEATFLAG_END_AFTER_SEQ){
	  ajFmtPrintAppS(&pos,"..>%d",gf->End);
	  ajDebug(">end\n");
	}
	else if(gf->Flags & FEATFLAG_END_TWO){
	  ajFmtPrintAppS(&pos,"..(%d.%d)",gf->End2,gf->End);
          ajDebug("end2 (%d.%d)\n", gf->End2, gf->End);
	}
	else{
	  ajFmtPrintAppS(&pos,"..%d",gf->End);
	  ajDebug(".. end\n");
	}
      }
      
      if(gf->Strand == '-' && !whole){
	ajStrAssC(&temp,"complement(");
	ajStrApp(&temp,pos);
	ajStrAppC(&temp,")");
	ajDebug("strand '-', wrap exon with complement()\n");
      }
      else {
	ajStrAss(&temp,pos);
	ajDebug("simple exon\n");
      }
      ajStrClear(&pos);
      ajStrApp(&location,temp);
      if (!(gf->Flags & FEATFLAG_CHILD)) /* this is the parent/only feature */
	gfprev=gf;
    }

    ajListIterFree(iter);

    /* Don't forget the last one !!! */
    if(join) {
      ajStrAppC(&location,")");      /* close bracket for join */
      ajDebug("last: join: closing ')' appended\n");
    }
    if(whole){
      ajStrInsertC(&location,0,"complement(");
      ajStrAppC(&location,")");
      ajDebug("lest: wrap with complement(), reset whole %b to N\n", whole);
      whole = ajFalse;
    }
    
    ajDebug("last: calling featDumpEmbl for gfprev\n");
    ajDebug("location: '%S'\n", location);

    featDumpEmbl(gfprev,location, file, IsEmbl) ; /* gfprev has tag data */
    ajStrDel(&location);
    ajStrDel(&pos);
    ajStrDel(&temp);
  }

  ajDebug ("ajFeattableWriteEmbl Done\n");

  return ajTrue;
}

/* @funcstatic ajFeattableWriteUnknown ****************************************
**
** Write a feature table in 'unknown' format.
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFile] Output file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool ajFeattableWriteUnknown (AjPFeattable thys, AjPFile file) {
  if(file == NULL) AJRAISE(Null_IO_Handle) ;
  (void) ajFmtPrintF (file, "Unknown feature format hence no output."
		      "Except this line!!\n");

  return ajFalse;
}

/* @func ajFeattableWriteSwiss ************************************************
**
** Write a feature table in SwissProt format.
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] file [AjPFile] Output file
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajFeattableWriteSwiss (AjPFeattable thys, AjPFile file) {
  AjIList    iter = NULL ;
  AjPFeature gf   = NULL ;

  /* Check arguments */
  ajDebug ("ajFeattableWriteSwiss Checking arguments\n");
  if(file == NULL) AJRAISE(Null_IO_Handle) ;

  /* For all features... */

  if (thys->Features) {
    iter = ajListIter(thys->Features) ;
    while(ajListIterMore(iter)) {
      gf = ajListIterNext (iter) ;
      featDumpSwiss(gf, file) ;
    }
    ajListIterFree(iter) ;
  }
   
  return ajTrue ;
}

/* @func ajFeatGetName ********************************************************
**
** Returns the name of a feature table object. This is a copy of the
** pointer to the name, and is still owned by the feature table** and
** is not to be destroyed.
**
** @param [r] thys [AjPFeattable] Feature table
** @return [AjPStr] Feature table name.
** @@
******************************************************************************/

AjPStr ajFeatGetName (AjPFeattable thys) {
  return thys->Name;
}


/* @funcstatic featFrame ******************************************************
**
** Converts a frame number in the range 0 to 3 to a GFF frame character
**
** @param [r] frame [ajint] Feature frame number
** @return [char] character for this frame in GFF
******************************************************************************/

static char featFrame (ajint frame) {

  static char framestr[] = ".012";

  if (frame < 0) return '.';
  if (frame > 3) return '.';
		  
  return framestr[frame];
}

/* @funcstatic featStrand *****************************************************
**
** Converts a strand number to a GFF strand character. NULL characters
** are converted to NULL All other values are simply cast to character.
**
** @param [r] strand [ajint] Strand
** @return [char] GFF character for this strand.
** @@
******************************************************************************/

static char featStrand (ajint strand) {

  if (!strand) return '\0';

  return ajSysItoC(strand);
}

/* @func ajFeatIsProt *********************************************************
**
** Returns ajTrue if a feature table is protein
**
** @param [r] thys [AjPFeattable] Feature table
** @return [AjBool] ajTrue for a protein feature table
** @@
******************************************************************************/

AjBool ajFeatIsProt (AjPFeattable thys) {
  ajDebug("ajFeatIsProt NOT IMPLEMENTED YET\n");
  return ajFalse;
}

/* @func ajFeatLen ************************************************************
**
** Returns the sequence length of a feature table
**
** @param [r] thys [AjPFeattable] Feature table
** @return [ajint] Length in bases or residues
** @@
******************************************************************************/

ajint ajFeatLen (AjPFeattable thys) {
  if (!thys) return 0;
  return (thys->End);
}

/* @func ajFeatSize ***********************************************************
**
** Returns the number of features in a feature table
**
** @param [r] thys [AjPFeattable] Feature table
** @return [ajint] Number of features
** @@
******************************************************************************/

ajint ajFeatSize (AjPFeattable thys) {
  if (!thys) return 0;
  return ajListLength (thys->Features);
}

/* @funcstatic ajFeatSetFlag **************************************************
**
** Set flag.
**
** @param [rw] flags [ajint*] ajint to add flag to.
** @param [r]  val   [ajint] flag to be set.
**
** @return [void]
** @@
******************************************************************************/
static void ajFeatSetFlag(ajint *flags, ajint val){
  if(*flags & val)		/* already set */
    return;
  else
    *flags |= val;
  return;
}

/* @func ajFeattabInClear *****************************************************
**
** Clears a Tabin input object back to "as new" condition, except
** for the USA list which must be preserved.
**
** @param [P] thys [AjPFeattabIn] Sequence input
** @return [void]
** @@
******************************************************************************/

void ajFeattabInClear (AjPFeattabIn thys)
{

  ajDebug ("ajFeattabInClear called\n");

  (void) ajStrClear(&thys->Ufo);
  (void) ajStrClear(&thys->Seqname);
  (void) ajStrClear(&thys->Formatstr);
  (void) ajStrClear(&thys->Filename);
  (void) ajStrClear(&thys->Entryname);
  (void) ajStrClear(&thys->Type);
  if (thys->Handle)
    ajFileBuffDel(&thys->Handle);
  if (thys->Handle)
    ajFatal("ajFeattabInClear did not delete Handle");

  return;
}


/* @func ajFeatLocToSeq *******************************************************
**
** Returns a sequence entry from a feature location
**
** @param [r] seq [AjPStr] sequence
** @param [r] line [AjPStr] location
** @param [w] res [AjPStr*] sequence construct
** @param [r] usa [AjPStr] usa of query
** @return [AjBool] true on success
** @@
******************************************************************************/

AjBool ajFeatLocToSeq(AjPStr seq, AjPStr line, AjPStr *res, AjPStr usa)
{
    static AjPStr str=NULL;
    char *p;
    char *q;
    
    ajint len;
    ajint i;
    ajint off;
    static AjPStr token=NULL;
    static AjPStr tmp=NULL;
    static AjPStr ent=NULL;
    
    AjPRegexp exp_ndotn = NULL;
    AjPRegexp exp_brnbr = NULL;
    AjPRegexp exp_compbrndashnbr = NULL;
    AjPRegexp exp_joinbr = NULL;
    AjPStrTok handle = NULL;
    
    AjBool isglobcomp=ajFalse;
    AjBool isjoin=ajFalse;
    AjBool docomp=ajFalse;
    AjBool dbentry=ajFalse;
    
    ajint begin=0;
    ajint end=0;
    
    if(!str)
    {
	token = ajStrNew();
	str   = ajStrNew();
	tmp   = ajStrNew();
	ent   = ajStrNew();
    }

    ajStrAssS(&str,line);


    /* Remove chevrons */
    p=ajStrStr(str);
    len = ajStrLen(str);
    for(i=0;i<len;++i)
    {
	if(*p=='<' || *p=='>')
	    *p=' ';
	++p;
    }

    ajStrCleanWhite(&str);

    /* Replace sites by a single location */
    p=ajStrStr(str);
    len = ajStrLen(str);
    while(*p)
    {
	if(*p=='^')
	{
	    *(p++)=' ';
	    while(*p>='0' && *p<='9')
		*(p++) = ' ';
	}
	else
	    ++p;
    }
    ajStrCleanWhite(&str);


    /* Replace any x.y with x */
    exp_ndotn = ajRegCompC("([0-9]+)[.]([0-9]+)");
    p = ajStrStr(str);
    while(ajRegExec(exp_ndotn,str))
    {
	off = ajRegOffset(exp_ndotn);
	while(p[off]!='.')
	    ++off;
	p[off++]=' ';
	while(p[off]>='0' && p[off]<='9')
	    p[off++]=' ';
    }
    ajRegFree(&exp_ndotn);
    ajStrCleanWhite(&str);

    /* Replace any (n) with n */
    exp_brnbr = ajRegCompC("[(]([0-9]+)[)]");
    p = ajStrStr(str);
    while(ajRegExec(exp_brnbr,str))
    {
	off = ajRegOffset(exp_brnbr);
	p[off++]=' ';
	while(p[off]!=')')
	    ++off;
	p[off++]=' ';
    }
    ajRegFree(&exp_brnbr);
    ajStrCleanWhite(&str);

    /* See if its a global complement and remove complement enclosure */
    if(ajStrPrefixC(str,"complement("))
    {
	p=ajStrStr(str);
	len=ajStrLen(str);
	ajStrAssSub(&str,str,11,len-2);
	isglobcomp=ajTrue;
    }
    
    /* Replace .. with - */
    p = ajStrStr(str);
    while(*p)
    {
	if(*p=='.' && *(p+1)=='.')
	{
	    *p='-';
	    *(p+1)=' ';
	}
	++p;
    }

    ajStrCleanWhite(&str);    


    /* Replace complement(n-n) with ^n-n */
    exp_compbrndashnbr = ajRegCompC("complement[(]([A-Za-z0-9:.]+)[-]([0-9]+)[)]");
/*    exp_compbrndashnbr = ajRegCompC("complement[(]([0-9]+)[-]([0-9]+)[)]");*/
    p = ajStrStr(str);
    while(ajRegExec(exp_compbrndashnbr,str))
    {
	off = ajRegOffset(exp_compbrndashnbr);
	for(i=0;i<10;++i)
	    p[off++]=' ';
	p[off]='^';
	while(p[off]!=')')
	    ++off;
	p[off++]=' ';
    }
    ajStrCleanWhite(&str);
    ajRegFree(&exp_compbrndashnbr);
    

    /* Check for only one "join" */
    p = ajStrStr(str);
    exp_joinbr = ajRegCompC("join[(]");    
    i=0;
    while(ajRegExec(exp_joinbr,str))
    {
	off = ajRegOffset(exp_joinbr);
	++i;
	if(off)
	{
	    ajWarn("Too many joins");
	    return ajFalse;
	}
	p=ajStrStr(str);
	len=ajStrLen(str);
	ajStrAssSub(&str,str,5,len-2);
	isjoin=ajTrue;
    }
    ajRegFree(&exp_joinbr);
    

    /* Construct the sequence */
    ajStrAssC(res,"");
    handle = ajStrTokenInit(str,",");
    while(ajStrToken(&token,&handle,NULL))
    {
	p = ajStrStr(token);
	if(*p=='^')
	{
	    ++p;
	    docomp=ajTrue;
	}
	else
	    docomp=ajFalse;

	q=p;
	dbentry = ajFalse;
	while(*q)
	    if(*(q++)==':')
	    {
		dbentry = ajTrue;
		break;
	    }

	if(dbentry)
	{
	    if(*ajStrStr(token)=='^')
		ajStrAssC(&token,ajStrStr(token)+1);
	    if(!featGetUsaSection(&ent,token,&begin,&end,usa))
	    {
		ajWarn("Couldn't find embedded entry %S\n",token);
		return ajFalse;
	    }
	    ajStrAssSubC(&tmp,ajStrStr(ent),--begin,--end);
	}
	else
	{
	    if(sscanf(p,"%d-%d",&begin,&end)!=2)
	    {
		if(*p>='0' && *p<='9')
		{
		    if(sscanf(p,"%d",&begin)==1)
			end=begin;
		    else
		    {
			ajWarn("LocToSeq: Unpaired range");
			return ajFalse;
		    }
		}
	    }
	    ajStrAssSubC(&tmp,ajStrStr(seq),--begin,--end);
       }
	

	if(docomp)
	    ajSeqReverseStr(&tmp);
	ajStrAppC(res,ajStrStr(tmp));
    }
    
    if(isglobcomp)
	ajSeqReverseStr(res);

    return ajTrue;
}

/* @func ajFeatGetLocs ********************************************************
**
** Returns location information from catenated sequence entry
**
** @param [r] str [AjPStr] catenated (seq->TextPtr) entry
** @param [w] cds [AjPStr**] array of locations
** @param [r] type [char*] type (e.g. CDS/mrna)

** @return [ajint] number of location lines
** @@
******************************************************************************/

ajint ajFeatGetLocs(AjPStr str, AjPStr **cds, char *type)
{
    AjPStr *entry=NULL;
    ajint nlines=0;
    ajint i=0;
    ajint ncds=0;
    ajint nc=0;
    char *p=NULL;
    AjPStr test=NULL;

    test = ajStrNew();
    ajFmtPrintS(&test,"     %s",type);
    
    nlines = ajStrListToArray(str, &entry);

    for(i=0;i<nlines;++i)
    {
	if(ajStrPrefixC(entry[i],"FT "))
	{
	    p = ajStrStr(entry[i]);
	    *p = *(p+1) = ' ';
	}
	
	if(ajStrPrefix(entry[i],test))
	    ++ncds;
    }
    

    if(ncds)
    {
	AJCNEW0(*cds,ncds);
	for(i=0;i<ncds;++i)
	    (*cds)[i] = ajStrNew();
    }


    for(nc=i=0;nc<ncds;++nc)
    {
	if(ajStrPrefixC(entry[i],"FT "))
	{
	    p = ajStrStr(entry[i]);
	    *p = *(p+1) = ' ';
	}
	
	while(!ajStrPrefix(entry[i],test))
	    ++i;
	
	ajStrAssC(&(*cds)[nc],ajStrStr(entry[i++])+21);
	while( *(p=ajStrStr(entry[i]))==' ')
	{
	    if(*(p+21)=='/' || *(p+5)!=' ')
		break;
	    ajStrAppC(&(*cds)[nc],p+21);
	    ++i;
	}
	ajStrCleanWhite(&(*cds)[nc]);
    }
	

    for(i=0;i<nlines;++i)
	ajStrDel(&entry[i]);
    AJFREE(entry);

    ajStrDel(&test);

    return ncds;
}

/* @func ajFeatGetTrans *******************************************************
**
** Returns ytanslation information from catenated sequence entry
**
** @param [r] str [AjPStr] catenated (seq->TextPtr) entry
** @param [w] cds [AjPStr**] array of translations
**
** @return [ajint] number of location lines
** @@
******************************************************************************/

ajint ajFeatGetTrans(AjPStr str, AjPStr **cds)
{
    AjPStr *entry=NULL;
    ajint nlines=0;
    ajint i=0;
    ajint ncds=0;
    ajint nc=0;
    char *p=NULL;
    static AjPRegexp exp_tr=NULL;
    
    
    nlines = ajStrListToArray(str, &entry);

    exp_tr = ajRegCompC("/translation=");

    for(i=0;i<nlines;++i)
    {
	if(ajStrPrefixC(entry[i],"FT "))
	{
	    p = ajStrStr(entry[i]);
	    *p = *(p+1) = ' ';
	}
	
	if(ajRegExec(exp_tr,entry[i]))
	    ++ncds;
    }
    

    if(ncds)
    {
	AJCNEW0(*cds,ncds);
	for(i=0;i<ncds;++i)
	    (*cds)[i] = ajStrNew();
    }


    for(nc=i=0;nc<ncds;++nc)
    {
	if(ajStrPrefixC(entry[i],"FT "))
	{
	    p = ajStrStr(entry[i]);
	    *p = *(p+1) = ' ';
	}
	
	while(!ajRegExec(exp_tr,entry[i]))
	    ++i;
	
	ajStrAssC(&(*cds)[nc],ajStrStr(entry[i++])+35);
	while( *(p=ajStrStr(entry[i]))==' ')
	{
	    if(*(p+21)=='/' || *(p+5)!=' ')
		break;
	    ajStrAppC(&(*cds)[nc],p+21);
	    ++i;
	}
	p = ajStrStr((*cds)[nc]);
	p[ajStrLen((*cds)[nc])-2] = ' ';
	ajStrCleanWhite(&(*cds)[nc]);
    }
	

    for(i=0;i<nlines;++i)
	ajStrDel(&entry[i]);
    AJFREE(entry);

    return ncds;
}

/* @funcstatic featGetUsaSection **********************************************
**
** Returns the sequence for a USA
**
** What is this doing in ajfeat ???
**
** @param [R] thys [AjPStr*] Result
** @param [R] token [AjPStr] Token
** @param [R] begin [ajint*] Start position
** @param [R] end [ajint*] End position
** @param [R] usa [AjPStr] Usa to be processed
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featGetUsaSection(AjPStr* thys, AjPStr token, ajint* begin,
				ajint* end, AjPStr usa)
{
    AjPStrTok handle=NULL;
    AjPStrTok hand2=NULL;
    
    AjPStr db=NULL;
    AjPStr entry=NULL;
    AjPStr entry2=NULL;
    AjPStr numbers=NULL;
    AjBool ok=ajTrue;
    char   *p=NULL;
    AjPSeq seq=NULL;
    
    db      = ajStrNew();
    entry   = ajStrNew();
    entry2  = ajStrNew();
    numbers = ajStrNew();
    seq     = ajSeqNew();

    handle = ajStrTokenInit(usa,":");
    ajStrToken(&db,&handle,NULL);
    ajStrTokenClear(&handle);
    ajStrAppC(&db,":");
    
    handle = ajStrTokenInit(token,":");
    ajStrToken(&entry,&handle,NULL);

    hand2 = ajStrTokenInit(entry,".");
    ajStrToken(&entry2,&hand2,NULL);
    ajStrTokenClear(&hand2);

    ajStrToken(&numbers,&handle,NULL);
    ajStrTokenClear(&handle);

    p = ajStrStr(numbers);
    if(sscanf(p,"%d-%d",begin,end)!=2)
    {
	if(sscanf(p,"%d",begin)==1)
	    *end=*begin;
	else
	    ok = ajFalse;
    }

    ajStrApp(&db,entry2);

    if(!ajSeqGetFromUsa(db,0,&seq))
	ok = ajFalse;

    ajStrAssC(thys,ajSeqChar(seq));

    ajStrDel(&db);
    ajStrDel(&entry);
    ajStrDel(&entry2);
    ajStrDel(&numbers);
    ajSeqDel(&seq);

    return ok;
}

/*========================================================================
======================= NEW FUNCTIONS ====================================
========================================================================*/

/* @func ajFeatTest ***********************************************************
**
** Temporary testing function for feature handling
** to be deleted when all is working happily
**
** @return [void]
** @@
******************************************************************************/

void ajFeatTest (void) {
  AjPFeattable table=NULL;
  AjPStr desc=NULL;
  AjPFeature ft=NULL;

  featInit();
  table = ajFeattableNew(NULL);

  ajStrAssC(&desc, "Testing feature addition");

  ft = ajFeatAddC(table, "testft",  5, 7, 1.23, '+', 0, desc);
  ajStrAssC(&desc, "And again...");

  ft = ajFeatAddC(table, "testft",  9, 19, 4.56, '-', 3, desc);
  ajStrAssC(&desc, "...but subject to change");
  ajFeatSetDesc(ft, desc);

  ajFeattableTrace(table);

  ajFeattabDel(&table);
  ajStrDel (&desc);

  return;
}

/* @funcstatic featInit *******************************************************
**
** Initialises everything needed for feature handling
**
** @return [void]
** @@
******************************************************************************/

static void featInit (void) {

  if (FeatInitDone)
    return;

  FeatInitDone = ajTrue;

  FeatTypeTable = ajStrTableNewCase(200); /* for the types + tags*/
  FeatTagsTable = ajStrTableNewCase(200); /* for the tags + valuetype */

  featVocabRead ("emboss", FeatTypeTable, FeatTagsTable);
  ajDebug ("Tables internal Type: %x Tags: %x\n",
	   FeatTypeTable, FeatTagsTable);
  

  return;
}

/* @funcstatic featVocabRead **************************************************
**
** Reads the possible feature types (keys) and tags (qualifiers)
** from files.
**
** @param [R] name [char*] Feature format
** @param [R] TypeTable [AjPTable] Feature type table
** @param [R] TagsTable [AjPTable] Feature tags table
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featVocabRead (char* name, AjPTable TypeTable,
			     AjPTable TagsTable) {

  AjPFile TagsFile=NULL;
  AjPFile TypeFile=NULL;
  AjPStr line = NULL;
  AjPStr tagname = NULL;
  AjPStr tagtype = NULL;
  AjPStr tag = NULL;
  AjPStr req = NULL;
  AjPStr type=NULL;
  AjPStr tmpstr=NULL;
  AjPStr token=NULL;
  AjPStr savetype = NULL;
  AjPStr tagstr = NULL;
  AjPStr typtagstr = NULL;
  AjPStr defname = NULL;

  AjPStr TagsFName=NULL;
  AjPStr TypeFName=NULL;
  static AjPRegexp ValExp =  NULL;
  static AjPRegexp TagExp = NULL;
  static AjPRegexp VocabExp = NULL;

  ajint numtype = -1;
  ajint typecount = 0;
  ajint tagscount = 0;
  ajint linecount = 0;
  ajint i;

  char* TagType[] = {
    "QTEXT",			/* quoted text */
    "TEXT",			/* unquoted, no white space */
    "VOID",			/* no value */
    "LIMITED",			/* limited vocabulary */
    "QLIMITED",			/* limited vocabulary, with quotes */
    "SPECIAL",			/* special formatting */
    "QSPECIAL",			/* special formatting, with quotes */
    NULL
  };

  if (!ValExp)
    ValExp = ajRegCompC("([^ \t]+) +([^ \t]+)") ;
  if (!TagExp)
    TagExp = ajRegCompC("(([mM])?/([^ \t]+))|([^/ \t]+)") ;
  if (!VocabExp)
    VocabExp = ajRegCompC("([^\", \t]+)") ;

  /* First read in the list of all possible tags */

  ajFmtPrintS (&TagsFName, "Etags.%s", name);
  ajDebug("featVocabRead '%S'\n", TagsFName);

  ajFileDataNew (TagsFName, &TagsFile);
  if(!TagsFile){
    ajErr("Unable to read data file '%S'\n", TagsFName);
    return ajFalse;
  }
    
  tagscount =0;
  linecount=0;
  while(ajFileReadLine(TagsFile,&line)){
    linecount++;

    if(ajStrLen(line) && ajStrNCmpC(line,"#",1)) { /* skip comments */
      if (ajRegExec(ValExp,line)){
	numtype=-1;
	tagname = NULL;		/* create a new tag */
	ajRegSubI (ValExp, 1, &tagname) ;
	ajRegSubI (ValExp, 2, &tagtype) ;

	for (i=0; TagType[i]; i++) {

	  if(!ajStrCmpC(tagtype,TagType[i])) {
	    numtype = i;
	    break;
	  }
	}
	if (numtype < 0) {
	  ajDebug ("Bad feature tag type '%S' in %F line %d\n",
		 tagtype, TagsFile, linecount);
	  ajErr ("Bad feature tag type '%S' in %F line %d",
		 tagtype, TagsFile, linecount);
	  break;
	}
	ajStrDel(&tagtype);

	tagscount++;
	if (tagscount == 1) {	/* save first tag as the default */
	  tagstr = NULL;
	  ajStrAssC(&defname, "");
	  ajStrAssS(&tagstr, tagname);
	  if (ajTablePut (TagsTable, defname, tagstr))
	    ajErr("Etags.%s duplicate tag for '%S'", name, defname);
	  tagstr = NULL;
	  defname = NULL;
	}
	tagstr = NULL;
	ajFmtPrintS (&tagstr, "%s;", TagType[numtype]);

	/*
	** Controlled vocabulary :
	** read the list of valid values
	*/

	if(ajStrMatchCaseCC(TagType[numtype], "LIMITED") ||
	   ajStrMatchCaseCC(TagType[numtype], "QLIMITED")){
	  (void) ajRegPost(ValExp,&tmpstr);

	  while(ajRegExec(VocabExp, tmpstr)){
	    ajRegSubI (VocabExp, 1, &token) ;
	    ajFmtPrintAppS(&tagstr, "%S;", token);
	    (void) ajRegPost(VocabExp,&tmpstr);
	  }
	  ajStrDelReuse(&tmpstr);
	}

	if (ajTablePut (TagsTable, tagname, tagstr))
	  ajErr("Etags.%s duplicate tag for '%S'", name, tagname);
	tagstr = NULL;
	tagname = NULL;
      }
      else {
	ajDebug ("** line format bad **\n%S", line);
      }
    }
  }
  ajFileClose(&TagsFile);


  /* Efeatures file
  ** format: featuretype
  **            M/mandatorytag
  **             /tag
  **             /tag
  **
  ** All tags must be defined in the Etags file (read earlier into TagsTable)
  */

  ajFmtPrintS (&TypeFName, "Efeatures.%s", name);
  ajDebug("Trying to open %S...",TypeFName);
  ajFileDataNew(TypeFName,&TypeFile);
  if(!TypeFile){
    ajErr("Unable to read data file '%S'\n", TagsFName);
    ajRegFree(&ValExp);
    ajRegFree(&TagExp);
    ajRegFree(&VocabExp);
    return ajFalse;
  }

  typecount =0;
  while(ajFileReadLine(TypeFile,&line)){
    if(ajStrNCmpC(line,"#",1)){ /* if a comment skip it */
      if(ajRegExec(TagExp,line)){
	ajRegSubI (TagExp, 2, &req) ;    /* get the mandatory code */
	ajRegSubI (TagExp, 3, &tag) ;    /* and get the tag ... */
	ajRegSubI (TagExp, 4, &type) ;    /* ... or, get the type */

	if (ajStrLen(type)) {	/* new feature type */
	  typecount++;
	  if (typecount == 1) {	/* first type saved as "" default */
	    defname = NULL;
	    typtagstr = NULL;
	    ajStrAssC(&defname, "");
	    ajStrAssS(&typtagstr, type);
	    if (ajTablePut (TypeTable, defname, typtagstr))
	      ajErr("Efeatures.%s duplicate tag for '%S'", name, defname);
	    typtagstr = NULL;
	  }
	  else {		/* save the previous feature type */
	    if (ajTablePut (TypeTable, savetype, typtagstr))
	      ajErr("Efeatures.%s duplicate tag for '%S'", name, savetype);
	    typtagstr = NULL;
	  }

	  /*
	  ** set up new feature type and type-tag strings
	  ** ready to save the details
	  */

	  typtagstr = ajStrNewCL(";", 256);
	  savetype = type;
	  type = NULL;
	}
	else {			/* tag name */
	  if (!ajTableGet(TagsTable, tag)) {
	    ajWarn ("%S: tag %S (feature %S) not in Etags file",
		    TypeFName, tag, savetype);
	  }
	  if (ajStrLen(req))
	    ajFmtPrintAppS (&typtagstr, "*");
	  ajFmtPrintAppS (&typtagstr, "%S;", tag);
	}

      }
    }
  }

  if (typecount > 0) {		/* save the last feature type */
    if (ajTablePut (TypeTable, savetype, typtagstr))
      ajErr("Efeatures.%s duplicate tag for '%S'", name, savetype);
    typtagstr = NULL;
    savetype = NULL;
  }

  ajFileClose(&TypeFile);

  ajStrDel(&line);
  ajStrDel(&token);

  line = (AjPStr) ajTableGet (TypeTable, ajStrNew());
  ajDebug ("Default type: '%S'\n", line);

  line = (AjPStr) ajTableGet (TagsTable, ajStrNew());
  ajDebug ("Default tag:  '%S'\n", line);

  /*
  ajTableTrace (TypeTable);
  ajTableTrace (TagsTable);
  ajStrTablePrint (TypeTable);
  ajStrTablePrint (TagsTable);
  */

  ajStrDel (&tmpstr);
  ajStrDel (&TypeFName);
  ajStrDel (&TagsFName);
  ajStrDel (&req);
  ajStrDel (&tag);

  return ajTrue;

}

/* @func ajFeatAdd ************************************************************
**
** Adds a feature to a feature table
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] type [AjPStr] Feature type
** @param [r] start [ajint] Start position
** @param [r] end [ajint] End position
** @param [r] score [float] Score (0.0 if missing) 
** @param [r] strand [char] Strand '+' or '-'
** @param [r] frame [ajint] Frame or 0 if not relevent
** @param [r] desc [AjPStr] Description (simple text)
** @return [AjPFeature] Feature record
** @@
******************************************************************************/

AjPFeature ajFeatAdd (AjPFeattable thys, AjPStr type,
		      ajint start, ajint end, float score,
		      char strand, ajint frame, AjPStr desc) {

  ajDebug("ajFeatAdd '%S' %d..%d (%.3f) '%c' %d '%S'\n",
	  type, start, end, score, strand, frame, desc);

  return ajFeatNew (thys, NULL, type, start, end, score,
		    strand, frame, desc);
}

/* @func ajFeatAddC ***********************************************************
**
** Adds a feature to a feature table
**
** @param [r] thys [AjPFeattable] Feature table
** @param [r] type [char*] Feature type
** @param [r] start [ajint] Start position
** @param [r] end [ajint] End position
** @param [r] score [float] Score (0.0 if missing) 
** @param [r] strand [char] Strand '+' or '-'
** @param [r] frame [ajint] Frame or 0 if not relevent
** @param [r] desc [AjPStr] Description (simple text)
** @return [AjPFeature] ajTrue on success
** @@
******************************************************************************/

AjPFeature ajFeatAddC (AjPFeattable thys, char* type,
		       ajint start, ajint end, float score,
		       char strand, ajint frame, AjPStr desc) {
  
  static AjPStr stype = NULL;

  ajStrAssC (&stype, type);

  ajDebug("ajFeatAdd '%S' %d..%d (%.3f) '%c' %d '%S'\n",
	  stype, start, end, score, strand, frame, desc);

  return ajFeatNew (thys, NULL, stype, start, end,
		    score, strand, frame, desc);
}

/* @func ajFeatNew ************************************************************
**
** Constructor for a new feature.
**
** @param  [rC]   thys     [AjPFeattable] Pointer to the ajFeattable which
**                         owns the feature
** @param  [rENU] source     [AjPStr]  Source of feature
** @param  [rENU] type     [AjPStr]  Feature type
** @param  [rNU]  start    [ajint]  Start position of the feature
** @param  [rNU]  end      [ajint]  End position of the feature
** @param  [rENU] score    [float]      Analysis score for the feature
** @param  [rNU]  strand   [char]  Strand (+/-/.) of the feature
** @param  [rNU]  frame    [ajint]   Frame (1,2,3 or 0) of the feature
** @param  [rENU] desc     [AjPStr] Description of feature (simple text)
** @return [AjPFeature] Newly allocated feature object
** @exception 'Mem_Failed' from memory allocation
** @@
** 
******************************************************************************/

AjPFeature ajFeatNew (AjPFeattable thys,
		      AjPStr       source, 
		      AjPStr       type,
		      ajint        start,
		      ajint        end,
		      float        score,
		      char         strand,
		      ajint        frame, 
		      AjPStr       desc) {

  AjPFeature ret = NULL ;

  if(!ajStrLen(type))
    return NULL;

  ajDebug ("ajFeatNew '%S' %d .. %d\n", type, start, end);

  /* Allocate the object... */
  AJNEW0(ret) ;

  ret->Tags = ajListNew();
  thys->Groups++;
  ret->Group = thys->Groups;
  ret->Exon = 0;

  ajStrCopy (&ret->Source, source);
  ret->Score = score;

  ret->Strand = strand ;
    
  ret->Frame  = frame ;
  ret->Start = start;
  ret->End = end;

  ajFeatSetDesc (ret, desc);

  ajListPushApp (thys->Features, ret);

  return ret;
}

/* @func ajFeatSetDesc ********************************************************
**
** Sets the description for a feature
**
** @param [r] thys [AjPFeature] Feature
** @param [r] desc [AjPStr] Feature description (simple text)
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

AjBool ajFeatSetDesc (AjPFeature thys, AjPStr desc) {

  ajFeatTagSetC (thys, "note", desc);

  return ajTrue;
}
 
/* @func ajFeatTagSetC ********************************************************
**
** Sets a feature tag value
**
** @param [r] thys [AjPFeature] Feature
** @param [r] tag [char*] Feature tag
** @param [r] value [AjPStr] Feature tag value
** @return [AjPStr] Previous tag value
** @@
******************************************************************************/

AjPStr ajFeatTagSetC (AjPFeature thys, char* tag, AjPStr value) {

  static AjPStr tmptag = NULL;
  ajStrAssC (&tmptag, tag);
  return ajFeatTagSet (thys, tmptag, value);
}

/* @func ajFeatTagSet *********************************************************
**
** Sets a feature tag value
**
** @param [r] thys [AjPFeature] Feature
** @param [r] tag [AjPStr] Feature tag
** @param [r] value [AjPStr] Feature tag value
** @return [AjPStr] Previous tag value
** @@
******************************************************************************/

AjPStr ajFeatTagSet (AjPFeature thys, AjPStr tag, AjPStr value) {

  FeatPTagval tv = NULL;
  static AjPStr oldvalue = NULL;
  AjPStr tmptag = NULL;		/* these come from AjPTable */
  AjPStr tmpfmt = NULL;		/* so please, please don't delete */
  static AjPStr tmpval = NULL;
  static AjPStr outtag = NULL;
  char* cp;

  ajDebug ("ajFeatTagSet '%S' '%S'\n", tag, value);

  featInit();

  tmptag = featTableTag (tag, FeatTagsTable);
  tmpfmt = featTagFmt (tmptag,  FeatTagsTable);
  ajStrAssS (&tmpval, value);
  ajStrAssS (&outtag, tmptag);

  cp = ajStrStr(tmpfmt);
  switch (CASE2(cp[0], cp[1])) {
  case CASE2('L','I') :
      ajDebug ("case limited\n"); /* limited */
      break;
  case CASE2('Q', 'L') :	/* limited, escape quotes */
      ajDebug ("case qlimited\n");
      break;
  case CASE2('Q', 'S') :	/* special regexp, quoted */
      ajDebug ("case qspecial\n");
      if (!featTagSpecial(&tmpval, tmptag)) {
	featTagSetDefault (tmptag, value, &outtag, &tmpval);
      }
      break;
  case CASE2('S','P') :	/* special regexp */
      ajDebug ("case special\n");
      if (!featTagSpecial(&tmpval, tmptag)) {
	featTagSetDefault (tmptag, value, &outtag, &tmpval);
      }
      break;
  case CASE2('T','E') :	/* no space, no quotes, wrap at margin */
      ajDebug ("case text\n");
      break;
  case CASE2('V','O') :	/* no value, so an error here */
      ajDebug ("case void\n");
      break;
  case CASE2('Q','T') :	/* escape quotes, wrap at space */
      ajDebug ("case qtext\n");
      break;
  default:
      ajWarn("Unknown internal feature tag type '%S' for '%S'",
	     tmpfmt, tmptag);
  }

  tv = featTagval (thys, outtag);
  if (tv) {			/* replace current value */
    ajStrAssS (&oldvalue, tv->Value);
    ajStrAssS (&tv->Value, tmpval);
    ajDebug ("...replaced old value '%S'\n", oldvalue);
    return oldvalue;
  }
  else {			/* new tag-value */
    tv = featTagvalNew (outtag, tmpval);
    ajListPushApp (thys->Tags, tv);
    ajDebug("...new tag-value\n");
    return NULL;
  }

  return oldvalue;
}

/* @funcstatic featTagSetDefault **********************************************
**
** Sets a feature tag value, using the default feature tag
**
** @param [r] tag [AjPStr] Feature tag
** @param [r] value [AjPStr] feature tag value
** @param [r] pdeftag [AjPStr*] Default tag
** @param [r] pdefval [AjPStr*] Default tag value as "*tag: value"
** @return [void]
** @@
******************************************************************************/

static void featTagSetDefault (AjPStr tag, AjPStr value,
			AjPStr* pdeftag, AjPStr* pdefval) {

  featInit();

  ajStrAssS (pdeftag, (AjPStr) ajTableGet (FeatTagsTable, ajStrNew()));
  ajFmtPrintS (pdefval, "*%S: %S", tag, value);

  return;
}

/* @func ajFeattableNew *******************************************************
**
** Constructor for a new (generic) feature table
**
** @param [R] name [AjPStr] Name for new feature table (or NULL for unnamed)
** @return [AjPFeattable] Pointer to a new (empty) feature table
** @exception  'Mem_Failed' from memory allocation
** @@
** 
******************************************************************************/

AjPFeattable ajFeattableNew( AjPStr name )
{
    return ajFeattableNewDna (name) ;
}

/* @func ajFeattableNewDna ****************************************************
**
** Constructor for a new DNA feature table
**
** @param [R] name [AjPStr] Name for new feature table (or NULL for unnamed)
** @return [AjPFeattable] Pointer to a new (empty) feature table
** @exception  'Mem_Failed' from memory allocation
** @@
** 
******************************************************************************/

AjPFeattable ajFeattableNewDna( AjPStr name )
{
  AjPFeattable thys = NULL ;

  /* Allocate the object... */
  AJNEW0(thys) ;

  FeattabInit(thys, name) ;
  ajStrAssC (&thys->Type, "N");

  return thys ;
}

/* @func ajFeattableNewSeq ****************************************************
**
** Constructor for a new feature table for an existing sequence.
** The feature table type is determined by the sequence type.
**
** @param [R] seq [AjPSeq] Sequence object to provide the name and type
** @return [AjPFeattable] Pointer to a new (empty) feature table
** @exception  'Mem_Failed' from memory allocation
** @@
** 
******************************************************************************/

AjPFeattable ajFeattableNewSeq( AjPSeq seq )
{
  AjPFeattable thys = NULL ;

  /* Allocate the object... */
  AJNEW0(thys) ;

  if (ajSeqIsProt(seq))
    thys = ajFeattableNewProt(ajSeqGetName(seq)) ;
  else
    thys = ajFeattableNewDna(ajSeqGetName(seq)) ;

  return thys ;
}

/* @func ajFeattableNewProt ***************************************************
**
** Constructor for a new protein feature table
**
** @param [R] name [AjPStr] Name for new feature table (or NULL for unnamed)
** @return [AjPFeattable] Pointer to a new (empty) feature table
** @exception  'Mem_Failed' from memory allocation
** @@
** 
******************************************************************************/

AjPFeattable ajFeattableNewProt ( AjPStr name )
{
  AjPFeattable thys = NULL ;

  /* Allocate the object... */
  AJNEW0(thys) ;

  FeattabInit(thys, name) ;
  ajStrAssC (&thys->Type, "P");

  return thys ;
}

/* @funcstatic featTagvalNew **************************************************
**
** Constructor for a feature tag-value pair
**
** @param [r]   tag    [AjPStr]   Tag name
** @param [r]   value  [AjPStr]   Tag value
** @return [FeatPTagval] New tag-value pair object
** @@
******************************************************************************/

static FeatPTagval featTagvalNew ( AjPStr tag, AjPStr value) { 

  FeatPTagval ret;
  AjPStr tmptag = NULL;		/* from AjPTable, don't delete */

  featInit();

  AJNEW0(ret);

  tmptag = featTableTag (tag, FeatTagsTable);

  ajStrAssS (&ret->Tag, tmptag);
  ajStrAssS (&ret->Value, value);

  return ret;
}

/* @funcstatic featTagval *****************************************************
**
** Checks for the existence of a defined tag for a feature.
**
** @param [r]   thys [AjPFeature]  Feature object
** @param [r]   tag  [AjPStr]      Tag name
** @return [FeatPTagval] Returns the tag-value pair if found,
**                       NULL if not found.
** @@
******************************************************************************/

static FeatPTagval featTagval ( AjPFeature thys, AjPStr tag) { 

  AjIList iter = NULL;
  FeatPTagval ret = NULL;
  FeatPTagval tv = NULL;

  iter = ajListIter( thys->Tags );
  while (ajListIterMore(iter)) {
    tv = ajListIterNext(iter);
    if (ajStrMatchCase (tv->Tag, tag)) {
      ajDebug ("featTagval '%S' found value '%S'\n", tag, tv->Value);
      ret = tv;
      break;
    }
  }

  ajListIterFree( iter);

  /*
    if (!ret)
      ajDebug ("featTagval '%S' not found\n", tag);
  */


  return ret;
}

/* @funcstatic featTagvalNewC *************************************************
**
** Constructor for a feature tag-value pair
**
** @param [r]   tag    [char*]   Tag name
** @param [r]   value  [AjPStr]   Tag value
** @return [FeatPTagval] New tag-value pair object
** @@
******************************************************************************/

static FeatPTagval featTagvalNewC ( char* tag, AjPStr value) { 

  static AjPStr tmptag = NULL;

  ajStrAssC (&tmptag, tag);
  return featTagvalNew (tmptag, value);
}

/* @func ajFeatTrace **********************************************************
**
** Traces (to the debug file) a feature object
**
** @param [r]   thys  [AjPFeature]  Feature
** @return [void]
** @@
******************************************************************************/

void ajFeatTrace (AjPFeature thys) {

  ajDebug ("  Source: '%S'\n", thys->Source);
  ajDebug ("  Type: '%S'\n", thys->Type);
  ajDebug ("  Location: %d..%d\n", thys->Start, thys->End);
  ajDebug ("  Strand: '%c'\n", thys->Strand);
  ajDebug ("  Frame: '%d'\n", thys->Frame);
  ajDebug ("  Flags: '%x'\n", thys->Flags);
  ajDebug ("  Start2: '%d'\n", thys->Start2);
  ajDebug ("  End2: '%d'\n", thys->Start2);
  ajDebug ("  RemoteId: '%S'\n", thys->Remote);
  ajDebug ("  Label: '%S'\n", thys->Label);

  ajFeatTagTrace (thys);
  return;
}

/* @func ajFeatTagTrace *******************************************************
**
** Traces (to the debug file) the tag-value pairs of a feature object
**
** @param [r]   thys  [AjPFeature]  Feature
** @return [void]
** @@
******************************************************************************/

void ajFeatTagTrace (AjPFeature thys) {

  AjIList iter;
  ajint i=0;
  FeatPTagval tv = NULL;

  iter = ajListIter (thys->Tags);
  while (ajListIterMore(iter)) {
    tv = ajListIterNext(iter);
    ajDebug (" %3d  %S : '%S'\n", ++i, tv->Tag, tv->Value);
  }
  ajListIterFree (iter);

  return;
}

/* @func ajFeatTagIter **************************************************
**
** Returns an iterator over all feature tag-value pairs
**
** @param [r]   thys  [AjPFeature]  Feature
** @return [AjIList] List iterator
** @@
******************************************************************************/

AjIList ajFeatTagIter (AjPFeature thys) {
  return ajListIter( thys->Tags);
}

/* @func ajFeatTagval *********************************************************
**
** Traces (to the debug file) the tag-value pairs of a feature object
**
** @param [r]  iter  [AjIList] List iterator from ajFeatTagIter
** @param [W] tagnam [AjPStr*] Tag name
** @param [W] tagval [AjPStr*] Tag val
** @return [AjBool] ajTrue if another tag-value pair was returned
** @@
******************************************************************************/

AjBool ajFeatTagval (AjIList iter, AjPStr* tagnam, AjPStr* tagval) {

  FeatPTagval tv = NULL;

  tv = ajListIterNext(iter);
  if (!tv)
    return ajFalse;
  ajStrAssS(tagnam, tv->Tag);
  ajStrAssS (tagval, tv->Value);

  return ajTrue;
}

/* @func ajFeattableTrace *****************************************************
**
** Traces (to the debug file) a complete feature table
**
** @param [r]   thys  [AjPFeattable]  Feature table
** @return [void]
** @@
******************************************************************************/

void ajFeattableTrace (AjPFeattable thys) {

  AjIList iter = NULL;
  AjPFeature ft = NULL;
  ajint i=0;

  ajDebug ("== ajFeattableTrace Start ==\n");

  if (!thys) {
    ajDebug("NULL table\n");
    return;
  }

  ajDebug("  Name: '%S'\n", thys->Name);

  iter = ajListIter (thys->Features);
  while (ajListIterMore(iter)) {
    ft = ajListIterNext(iter);
    ajDebug("Features[%d]\n", ++i);
    ajFeatTrace (ft);
  }
  ajListIterFree (iter);

  ajDebug ("== ajFeattableTrace Done ==\n");
  return;
}

/* @funcstatic featType *******************************************************
**
** Given a feature type name,
** returns the valid feature type for the internal feature table
**
** @param [r]   type  [AjPStr] Type name
** @return [AjPStr] Valid feature type
** @@
******************************************************************************/

static AjPStr featType (AjPStr type) {

  featInit();

  return featTableType (type, FeatTypeTable);
}  

/* @funcstatic featTag ********************************************************
**
** Given a feature tag name,
** returns the valid feature tag for the internal feature table
**
** @param [r]   thys  [AjPStr] Tag name
** @return [AjPStr] Valid feature tag name
** @@
******************************************************************************/

static AjPStr featTag (AjPStr thys) {

  featInit();

  return featTableTag (thys, FeatTagsTable);
}  

/* @funcstatic featTableType **************************************************
**
** Given a feature type name,
** returns the valid feature type for a feature table
**
** @param [r]   type  [AjPStr] Type name
** @param [r]   table [AjPTable]  Feature table
** @return [AjPStr] Valid feature type
** @@
******************************************************************************/

static AjPStr featTableType (AjPStr type, AjPTable table) {

  static AjPStr ret = NULL;

  ret = (AjPStr) ajTableKey (table, type);
  if (ret) {
    /*
      ajDebug ("featType '%S' found in internal table as '%S'\n",
	     type, ret);
    */
    return ret;
  }
  else {
    ret = (AjPStr) ajTableGet (table, ajStrNew());
    ajDebug ("featType '%S' not in internal table %x, default to '%S'\n",
	     type, table, ret);
    /* ajStrTableTrace (table); */
  }
  return ret;
}

/* @funcstatic featTableTag ***************************************************
**
** Given a feature tag name,
** returns the valid feature tag name for a feature table
**
** @param [r]   tag  [AjPStr] Type name
** @param [r]   table [AjPTable]  Feature table
** @return [AjPStr] Valid feature tag name
** @@
******************************************************************************/

static AjPStr featTableTag (AjPStr tag, AjPTable table) {

  static AjPStr ret = NULL;

  ret = (AjPStr) ajTableKey (table, tag);
  if (ret) {
    /*
      ajDebug ("featTag '%S' found in internal table as '%S'\n",
	     tag, ret);
    */
    return ret;
  }
  else  {
    ret = (AjPStr) ajTableGet (table, ajStrNew());
    ajDebug ("featTag '%S' not in internal table %x, default to '%S'\n",
	     tag, table, ret);
    /* ajStrTableTrace (table); */
  }
  return ret;
}

/* @funcstatic featTableTagC **************************************************
**
** Given a feature tag name,
** returns the valid feature tag name for a feature table
**
** @param [r]   tag  [char*] Type name
** @param [r]   table [AjPTable]  Feature table
** @return [AjPStr] Valid feature tag name
** @@
******************************************************************************/

static AjPStr featTableTagC (char* tag, AjPTable table) {

  static AjPStr ret = NULL;
  static AjPStr tmptag = NULL;

  ajStrAssC (&tmptag, tag);

  ret = (AjPStr) ajTableKey (table, tmptag);
  if (ret) {
    /*
      ajDebug ("featTag '%S' found in internal table as '%S'\n",
	     tag, ret);
    */
    return ret;
  }
  else  {
    ret = (AjPStr) ajTableGet (table, ajStrNew());
    ajDebug ("featTag '%S' not in internal table %x, default to '%S'\n",
	     tag, table, ret);
    /* ajStrTableTrace (table); */
  }
  return ret;
}

/* @funcstatic featTagSpecialAllAnticodon *************************************
**
** Tests a string as a valid internal (EMBL) feature /anticodon tag
**
** The format is  (pos:<base_range>,aa:<amino_acid>)
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllAnticodon (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr begstr = NULL;
  static AjPStr endstr = NULL;
  static AjPStr aastr  = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^[(]pos:([0-9]+)[.][.]([0-9]+),aa:([^)]+)[)]$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &begstr);
    ajRegSubI (exp, 2, &endstr);
    ajRegSubI (exp, 3, &aastr);
  }

  if (!ret) {
    ajDebug("bad EMBL /anticodon value '%S'\n", *pval);
    ajWarn ("bad EMBL /anticodon value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllCitation **************************************
**
** Tests a string as a valid internal (EMBL) feature /citation tag
**
** The format is [1] where the number is a citation in an EMBL entry.
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllCitation (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr numstr = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^\\[([0-9]+)\\]$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &numstr);
  }

  if (!ret) {
    ajDebug("bad EMBL /citation value '%S'\n", *pval);
    ajWarn ("bad EMBL /citation value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllCodon *****************************************
**
** Tests a string as a valid internal (EMBL) feature /codon tag
**
** The format is (seq:"ttt",aa:Leu)
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllCodon (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr seqstr = NULL;
  static AjPStr aastr  = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^[(]seq:\"([acgt][acgt][acgt])\",aa:([^)]+)[)]$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &seqstr);
    ajRegSubI (exp, 2, &aastr);
  }

  if (!ret) {
    ajDebug("bad EMBL /codon value '%S'\n", *pval);
    ajWarn ("bad EMBL /codon value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllConssplice ************************************
**
** Tests a string as a valid internal (EMBL) feature /cons_splice tag
**
** The format is (5'site:YES,3'site:NO) where the booleans can
** be YES, NO, or ABSENT (intended for use where one site is NO and
** the other is missing)
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllConssplice (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr begstr = NULL;
  static AjPStr endstr = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^[(]5'site:([A-Z]+),3'site:([A-Z]+)[)]$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &begstr);
    ajRegSubI (exp, 2, &endstr);
  }

  if (!ret) {
    ajDebug("bad EMBL /cons_splice value '%S'\n", *pval);
    ajWarn ("bad EMBL /cons_splice value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllRptunit ***************************************
**
** Tests a string as a valid internal (EMBL) feature /rpt_unit tag
**
** The format is 123..789
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllRptunit (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr begstr = NULL;
  static AjPStr endstr = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^([0-9]+)[.][.]([0-9]+)$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &begstr);
    ajRegSubI (exp, 2, &endstr);
 }

  if (!ret) {
    ajDebug("bad EMBL /rpt_unit value '%S'\n", *pval);
    ajWarn ("bad EMBL /rpt_unit value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllTranslexcept **********************************
**
** Tests a string as a valid internal (EMBL) feature /transl_except tag
**
** The format is (pos:213..215,aa:Trp)
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllTranslexcept (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr begstr = NULL;
  static AjPStr endstr = NULL;
  static AjPStr aastr  = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^[(]pos:([0-9]+)[.][.]([0-9]+),aa:([^)]+)[)]$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &begstr);
    ajRegSubI (exp, 2, &endstr);
    ajRegSubI (exp, 3, &aastr);
  }

  if (!ret) {
    ajDebug("bad EMBL /transl_except value '%S'\n", *pval);
    ajWarn ("bad EMBL /transl_except value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllDbxref ****************************************
**
** Tests a string as a valid internal (EMBL) feature /db_xref tag
**
** The format is <database>:<identifier>
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllDbxref (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr dbstr = NULL;
  static AjPStr idstr = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^\"([^:]+):(.+)\"$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &dbstr);
    ajRegSubI (exp, 2, &idstr);
  }

  if (!ret) {
    ajDebug("bad EMBL /db_xref value '%S'\n", *pval);
    ajWarn ("bad EMBL /db_xref value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllProteinid *************************************
**
** Tests a string as a valid internal (EMBL) feature /protein_id tag
**
** The format is AAA12345.1
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllProteinid (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr idstr = NULL;
  static AjPStr preidstr = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^\"(([A-Z0-9]+)[.][0-9]+)\"$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &idstr);
    ajRegSubI (exp, 2, &preidstr);
  }

  if (!ret) {
    ajDebug("bad EMBL /protein_id value '%S'\n", *pval);
    ajWarn ("bad EMBL /protein_id value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagSpecialAllReplace ***************************************
**
** Tests a string as a valid internal (EMBL) feature /replace tag
**
** The format is "<sequence>"
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllReplace (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr seqstr = NULL;
  AjBool ret = ajFalse;

  if (!exp)
    exp = ajRegCompC("^\"([acgt]*)\"$");

  ajDebug("Before quote '%S' %c %c\n", *pval,
	  ajStrChar(*pval, 0), ajStrChar(*pval, -1));
  ajStrQuote (pval);
  ajDebug(" After quote '%S' %c %c\n", *pval,
	  ajStrChar(*pval, 0), ajStrChar(*pval, -1));

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &seqstr);
  }

  if (!ret) {
    ajDebug("bad EMBL /replace value '%S'\n", *pval);
    ajWarn ("bad EMBL /replace value '%S'",   *pval);
  }

  return ret;
}


/* @funcstatic featTagSpecialAllTranslation ***********************************
**
** Tests a string as a valid internal (EMBL) feature /translation tag
**
** The format is valid amino acid codes, no white space
**
** @param  [r] pval [AjPStr*] parameter value
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagSpecialAllTranslation (AjPStr* pval) {

  static AjPRegexp exp = NULL;

  static AjPStr seqstr = NULL;
  AjBool ret = ajFalse;

  ajStrCleanWhite(pval);
  if (!exp)
    exp = ajRegCompC("^\"([ACDEFGHIKLMNPQRSTVWYX]+)\"$");

  if (ajRegExec(exp, *pval)) {
    ret = ajTrue;
    ajRegSubI (exp, 1, &seqstr);
  }

  if (!ret) {
    ajDebug("bad EMBL /translation value '%S'\n", *pval);
    ajWarn ("bad EMBL /translation value '%S'",   *pval);
  }

  return ret;
}

/* @funcstatic featTagEmblQuote ***********************************************
**
** Escaped quotes converted to two double quotes
** for EMBL feature tag values
**
** @param  [r] pval [AjPStr*] parameter value
** @return [void]
** @@
******************************************************************************/

static void featTagEmblQuote (AjPStr* pval) {

  static AjPRegexp quoteexp = NULL;
  static AjPStr substr = NULL;
  static AjPStr valcopy = NULL;
  static AjPStr tmpstr = NULL;

  if (!quoteexp)
    quoteexp = ajRegCompC("([^\\\\]*)\\\\\"");

  /* ajDebug("featTagEmblQuote '%S'\n", *pval); */
 
  ajStrAssS (&valcopy, *pval);
  ajStrDelReuse (pval);
  while (ajRegExec(quoteexp, valcopy)) {
    ajRegSubI(quoteexp, 1, &substr);
    /* ajDebug("part '%S'\n", substr); */
    ajStrApp(pval, substr);
    ajStrAppC(pval, "\"\"");
    ajRegPost (quoteexp, &tmpstr);
    ajStrAssS (&valcopy, tmpstr);
  }
  /* ajDebug("rest '%S'\n", valcopy); */
  ajStrApp(pval, valcopy);

  return;
}

/* @funcstatic featLocEmblWrapC ***********************************************
**
** Splits EMBL feature location at the last possible comma
** and adds the appropriate prefix (e.g. the EMBL FT line type)
**
** @param  [r] ploc [AjPStr*] location as a string
** @param  [r] margin [ajint] Right margin
** @param  [r] prefix [char*] Left margin prefix string
** @param  [r] preftyp [char*] Left margin prefix string for first line
**                            (includes the feature key)
** @return [AjPStr] String with prefix and newlines
** @@
******************************************************************************/

static AjPStr featLocEmblWrapC (AjPStr *ploc, ajint margin,
				char* prefix, char* preftyp) {

  static AjPStr ret = NULL;
  ajint left = 0;
  ajint width = 0;
  ajint len = 0;
  ajint i, j, k;
  static AjPStr tmpstr = NULL;
  ajint last;

  left = strlen(prefix);
  width = margin - left;	/* available width for printing */

  ajStrCleanWhite(ploc);	/* no white space in locations */
  len = ajStrLen(*ploc);

  ajDebug("featLocEmblWrapC %d <%d> '%S'\n", len, width, *ploc);
  for (i=0; i < len; i+= k) {
    last = i + width - 1;

    ajDebug("try %d to %d (len %d)\n", i, last, len);

    if ((last+1) >= len) { /* no need to split */
      ajStrAssSub(&tmpstr, *ploc, i, len);
      ajDebug ("last %d >= len %d\n", last, len);
      j = 0;
    }
    else {
      ajStrAssSub(&tmpstr, *ploc, i, last); /* save max string */
      j = ajStrRFindC(tmpstr, ","); /* last comma in tmpstr */
      ajDebug ("comma at %d\n", j);
    }
    if (j < 1) {		/* no comma found */
      j = ajStrLen(tmpstr);
      k = j;
    }
    else {			/* print up to last comma */
      j++;
      k = j;			/* start after the comma */
    }
    ajDebug ("%d +%d '%.*S'\n", i, j, j, tmpstr);
    if (!i)
      ajFmtPrintAppS(&ret, "%s%.*S\n", preftyp,j, tmpstr);
    else
      ajFmtPrintAppS(&ret, "%s%.*S\n", prefix,j, tmpstr);
  }

  return ret;
}

/* @funcstatic featTagEmblWrapC ***********************************************
**
** Splits feature table output at the last possible space (or
** the last column if there are no spaces) and adds the appropriate
** prefix (e.g. the EMBL FT line type)
**
** @param  [r] pval [AjPStr*] parameter value
** @param  [r] margin [ajint] Right margin
** @param  [r] prefix [char*] Left margin prefix string
** @return [AjPStr] String with prefix and newlines
** @@
******************************************************************************/

static AjPStr featTagEmblWrapC (AjPStr *pval, ajint margin, char* prefix) {

  static AjPStr ret = NULL;
  ajint left = 0;
  ajint width = 0;
  ajint len = 0;
  ajint i, j, k;
  static AjPStr tmpstr = NULL;
  ajint last;

  left = strlen(prefix);
  width = margin - left;	/* available width for printing */

  ajStrClean(pval);		/* single spaces only */
  len = ajStrLen(*pval);

  ajDebug("featTagEmblWrapC %d <%d> '%S'\n", len, width, *pval);
  for (i=0; i < len; i+= k) {
    last = i + width - 1;

    ajDebug("try %d to %d (len %d)\n", i, last, len);

    if ((last+1) >= len) { /* no need to split */
      ajStrAssSub(&tmpstr, *pval, i, len);
      ajDebug ("last %d >= len %d\n", last, len);
      j = 0;
    }
    else if (ajStrChar(*pval, (last+1)) == ' ') { /* split at max width */
      ajStrAssSub(&tmpstr, *pval, i, last);
      j = last + 1;
    }
    else {
      ajStrAssSub(&tmpstr, *pval, i, last); /* save max string */
      j = ajStrRFindC(tmpstr, " "); /* last space in tmpstr */
      ajDebug ("space at %d\n", j);
    }
    if (j < 1) {		/* no space found */
      j = ajStrLen(tmpstr);
      k = j;
    }
    else {			/* print up to last space */
      k = j + 1;		/* start after the space */
    }
    ajDebug ("%d +%d '%.*S'\n", i, j, j, tmpstr);
    ajFmtPrintAppS(&ret, "%s%.*S\n", prefix,j, tmpstr);
  }

  return ret;
}

/* @funcstatic featTagAllLimit ***********************************************
**
** Tests a string as a valid feature value, given a
** list of possible values.
**
** @param  [r] pval [AjPStr*] parameter value
** @param  [r] values [AjPStr] comma delimited list of values
** @return [AjBool] ajTrue for a valid value, possibly corrected
**                  ajFalse if invalid, to be converted to default (note) type
** @@
******************************************************************************/

static AjBool featTagAllLimit (AjPStr* pval, AjPStr values) {

  static AjPRegexp limitexp = NULL;
  static AjPStr limstr = NULL;
  static AjPStr valcopy = NULL;
  static AjPStr tmpstr = NULL;
  AjBool ret = ajFalse;

  if (!limitexp)
    limitexp = ajRegCompC("([^,]+),?");

  ajDebug("featTagAllLimit '%S' '%S'\n", *pval, values);
 
  ajStrAssS (&valcopy, values);

  while (ajRegExec(limitexp, valcopy)) {
    ajRegSubI(limitexp, 1, &limstr);
    ajDebug("test '%S'\n", limstr);
    if (ajStrMatchCase(*pval, limstr)) {
      if (!ajStrMatch(*pval, limstr)) {
	ajStrAssS(pval, limstr);
      }
      ret = ajTrue;
      break;
    }
    ajRegPost (limitexp, &tmpstr);
    ajStrAssS (&valcopy, tmpstr);
  }

  return ret;
}

/* @funcstatic featTagEmblDefault *********************************************
**
** Give up, and generate a default feature tag
**
** @param  [r] pout [AjPStr*] Output string
** @param  [r] tag [AjPStr] original tag name
** @param  [r] pval [AjPStr*] parameter value
** @return [void]
** @@
******************************************************************************/

static void featTagEmblDefault (AjPStr* pout, AjPStr tag, AjPStr* pval) {

  ajDebug("featTagEmblDefault '%S' '%S'\n", tag, *pval);
 
  featTagEmblQuote(pval);
  ajFmtPrintS(pout, "/note=\"%S: %S\"", tag, *pval);
  return;
}

/* @funcstatic featTagGffDefault **********************************************
**
** Give up, and generate a default feature tag
**
** @param  [r] pout [AjPStr*] Output string
** @param  [r] tag [AjPStr] original tag name
** @param  [r] pval [AjPStr*] parameter value
** @return [void]
** @@
******************************************************************************/

static void featTagGffDefault (AjPStr* pout, AjPStr tag, AjPStr* pval) {

  ajDebug("featTagGffDefault '%S' '%S'\n", tag, *pval);
 
  featTagEmblQuote(pval);
  ajFmtPrintS(pout, "note \"%S: %S\"", tag, *pval);
  return;
}

/* @funcstatic featTagSpecial *************************************************
**
** Special processing for known internal (EMBL) tags
**
** @param  [u] pval [AjPStr*] parameter value
** @param  [r] tag [AjPStr] original tag name
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featTagSpecial (AjPStr* pval, AjPStr tag) {

  ajDebug("featTagSpecial '%S'\n", tag);
 
  if (ajStrMatchC(tag, "anticodon"))
    return featTagSpecialAllAnticodon(pval);

  if (ajStrMatchC(tag, "citation"))
    return featTagSpecialAllCitation(pval);

  if (ajStrMatchC(tag, "codon"))
    return featTagSpecialAllCodon(pval);

  if (ajStrMatchC(tag, "cons_splice"))
    return featTagSpecialAllConssplice(pval);

  if (ajStrMatchC(tag, "rpt_unit"))
    return featTagSpecialAllRptunit(pval);

  if (ajStrMatchC(tag, "transl_except"))
    return featTagSpecialAllTranslexcept(pval);

  if (ajStrMatchC(tag, "db_xref"))
    return featTagSpecialAllDbxref(pval);

  if (ajStrMatchC(tag, "protein_id"))
    return featTagSpecialAllProteinid(pval);

  if (ajStrMatchC(tag, "replace"))
    return featTagSpecialAllReplace(pval);

  if (ajStrMatchC(tag, "translation"))
    return featTagSpecialAllTranslation(pval);

  ajDebug ("Unrecognised special EMBL feature tag '%S'\n", tag);
  ajWarn  ("Unrecognised special EMBL feature tag '%S'",   tag);

  return ajFalse;
}

/* @funcstatic featTagGffSpecial **********************************************
**
** Special processing for known GFF tags
**
** This function will be very similar to featTagSpecial, with scope
** for future GFF-specific extensions
**
** @param  [u] pval [AjPStr*] tag value
** @param  [r] tag [AjPStr] original tag name
** @return [AjBool] ajTrue on success
** @@
******************************************************************************/

static AjBool featTagGffSpecial (AjPStr* pval, AjPStr tag) {

  ajDebug("featTagGffSpecial '%S' '%S'\n", tag, *pval);
 
  if (ajStrMatchC(tag, "anticodon"))
    return featTagSpecialAllAnticodon(pval);

  if (ajStrMatchC(tag, "citation"))
    return featTagSpecialAllCitation(pval);

  if (ajStrMatchC(tag, "codon"))
    return featTagSpecialAllCodon(pval);

  if (ajStrMatchC(tag, "cons_splice"))
    return featTagSpecialAllConssplice(pval);

  if (ajStrMatchC(tag, "rpt_unit"))
    return featTagSpecialAllRptunit(pval);

  if (ajStrMatchC(tag, "transl_except"))
    return featTagSpecialAllTranslexcept(pval);

  if (ajStrMatchC(tag, "db_xref"))
    return featTagSpecialAllDbxref(pval);

  if (ajStrMatchC(tag, "protein_id"))
    return featTagSpecialAllProteinid(pval);

  if (ajStrMatchC(tag, "replace"))
    return featTagSpecialAllReplace(pval);

  if (ajStrMatchC(tag, "translation"))
    return featTagSpecialAllTranslation(pval);

  ajDebug ("Unrecognised special GFF feature tag '%S'\n", tag);
  ajWarn  ("Unrecognised special GFF feature tag '%S'",   tag);

  return ajFalse;
}

/* @funcstatic featDumpEmbl ***************************************************
**
** Write details of single feature to file in EMBL/GenBank/DDBJ format
**
** @param [r] feat     [AjPFeature] Feature
** @param [r] location [AjPStr] location list
** @param [r] file     [AjPFile] Output file
** @param [r] IsEmbl   [AjBool] ajTrue if writing EMBL format (FT prefix)
** @return [void]
** @@
******************************************************************************/

static void featDumpEmbl (AjPFeature feat, AjPStr location, AjPFile file,
			  AjBool IsEmbl) {

  AjIList iter = NULL;
  ajint i=0;
  FeatPTagval tv = NULL;
  AjPStr tmptyp=NULL;		/* these come from AjPTable */
  AjPStr tmptag=NULL;		/* so please, please */
  AjPStr tmpfmt=NULL;		/* don't delete them */
  static AjPStr outstr = NULL;
  static AjPStr tmpval=NULL;
  static AjPStr tmplim = NULL;
  static AjPStr deftag = NULL;
  char* cp;
  AjPStr wrapstr = NULL;
  static AjPStr preftyploc = NULL;
  static AjPStr preftyptag = NULL;
  static AjPStr tmploc = NULL;

  ajStrAssC (&deftag, "note");

  ajDebug ("featDumpEmbl Start\n");

 /* print the location */

  ajStrAssS (&tmploc, location);
  tmptyp = featTableType (feat->Type, FeatTypeTableEmbl);

  if (IsEmbl) {
    ajFmtPrintS (&preftyploc, "%s   %-15.15S ", "FT", tmptyp);
    ajFmtPrintS (&preftyptag, "%s                   ", "FT");
  }
  else {
    ajFmtPrintS (&preftyploc, "%s   %-15.15S ", "  ", tmptyp);
    ajFmtPrintS (&preftyptag, "%s                   ", "  ");
  }

  wrapstr = featLocEmblWrapC(&tmploc, 72,
			     ajStrStr(preftyptag),
			     ajStrStr(preftyploc));
  ajFmtPrintF (file, "%S", wrapstr);
  ajStrDel (&wrapstr);

  /* print the qualifiers */

  iter = ajListIter (feat->Tags);
  while (ajListIterMore(iter)) {
    tv = ajListIterNext(iter);
    ++i;
    tmptag = featTableTag (tv->Tag, FeatTagsTableEmbl);
    tmpfmt = featTagFmt (tmptag, FeatTagsTableEmbl);
    /* ajDebug (" %3d  %S value: '%S'\n", i, tv->Tag, tv->Value); */
    ajDebug (" %3d  %S format: '%S'\n", i, tmptag, tmpfmt);
    ajFmtPrintS (&outstr, "/%S", tmptag);
    if (tv->Value) {
      ajStrAssS(&tmpval, tv->Value);
      cp = ajStrStr(tmpfmt);
      switch (CASE2(cp[0], cp[1])) {
      case CASE2('L','I') :
	ajDebug ("case limited\n"); /* limited */
	tmplim = featTagLimit (tmptag, FeatTagsTableEmbl);
	featTagAllLimit (&tmpval, tmplim);
	ajFmtPrintAppS (&outstr, "=%S\n", tmpval);
	ajStrDel(&tmplim);
	break;
      case CASE2('Q', 'L') :	/* limited, escape quotes */
	ajDebug ("case qlimited\n");
	tmplim = featTagLimit (tmptag, FeatTagsTableEmbl);
	featTagAllLimit (&tmpval, tmplim);
	featTagEmblQuote (&tmpval);
	ajFmtPrintAppS (&outstr, "=%S\n", tmpval);
	ajStrDel(&tmplim);
	break;
      case CASE2('Q', 'S') :	/* special regexp, quoted */
	ajDebug ("case qspecial\n");
	if (!featTagSpecial(&tmpval, tmptag)) {
	  featTagEmblDefault(&outstr, tmptag, &tmpval);
	}
	else {
	  featTagEmblQuote(&tmpval);
	  ajFmtPrintAppS (&outstr, "=%S\n", tmpval);
	}
	break;
      case CASE2('S','P') :	/* special regexp */
	ajDebug ("case special\n");
	if (!featTagSpecial(&tmpval, tmptag)) {
	  featTagEmblDefault(&outstr, tmptag, &tmpval);
	}
	else {
	  ajFmtPrintAppS (&outstr, "=%S\n", tmpval);
	}
	break;
      case CASE2('T','E') :	/* no space, no quotes, wrap at margin */
	ajDebug ("case text\n");
	ajStrCleanWhite(&tmpval);
	ajFmtPrintAppS (&outstr, "=%S\n", tmpval);
	break;
      case CASE2('V','O') :	/* no value, so an error here */
	ajDebug ("case void\n");
	break;
      case CASE2('Q','T') :	/* escape quotes, wrap at space */
	ajDebug ("case qtext\n");
	featTagEmblQuote(&tmpval);
	ajFmtPrintAppS (&outstr, "=%S\n", tmpval);
	break;
      default:
	ajWarn("Unknown EMBL feature tag type '%S' for '%S'",
	       tmpfmt, tmptag);
      }
    }
    else {
      ajDebug ("no value, hope it is void: '%S'\n", tmpfmt);
    }
    wrapstr = featTagEmblWrapC(&outstr, 80, ajStrStr(preftyptag));
    ajFmtPrintF (file, "%S", wrapstr);
    ajStrDel (&wrapstr);
  }

  ajDebug ("featDumpEmbl Done %d tags\n", i);

  ajListIterFree (iter);
  return;
}

/* @funcstatic featDumpSwiss **************************************************
**
** Write details of single feature to output file
**
** @param [r] feat [AjPFeature] Feature
** @param [r] file [AjPFile] Output file
** @return [void]
** @@
******************************************************************************/

static void featDumpSwiss (AjPFeature feat, AjPFile file) {

  /* simple line-by-line */

  return;
}

/* @funcstatic featDumpGff ****************************************************
**
** Write details of single feature to GFF output file
**
** @param [r] thys [AjPFeature] Feature
** @param [r] owner [AjPFeattable] Feature table (used for the sequence name)
** @param [r] file [AjPFile] Output file
** @return [void]
** @@
******************************************************************************/

static void featDumpGff (AjPFeature thys, AjPFeattable owner, AjPFile file) {

  AjIList iter = NULL;
  AjPStr outtyp = NULL;		/* these come from AjPTable */
  AjPStr outtag = NULL;		/* so please, please */
  AjPStr outfmt = NULL;		/* don't delete them */
  static AjPStr outstr = NULL;
  static AjPStr tmpval=NULL;
  static AjPStr tmplim = NULL;
  AjPStr outval = NULL;
  FeatPTagval tv = NULL;
  ajint end;
  ajint i=0;
  char* cp;
  AjPStr flagdata = NULL;

  /* header done by calling routine */

  ajDebug ("featDumpGff...\n");

  /* simple line-by line with Gff tags */

  outtyp = featTableType (thys->Type, FeatTypeTableGff);

  ajDebug("Type '%S' => '%S'\n", thys->Type, outtyp);

  end = thys->End;
  if(thys->Flags & FEATFLAG_END_TWO)
    end = thys->End2;

  (void) ajFmtPrintF (file,
	       "%S\t%S\t%S\t%d\t%d\t%.3f\t%c\t%c\t",
	       owner->Name,
	       thys->Source,
	       thys->Type,
	       thys->Start,
	       thys->End,
	       thys->Score, 
	       featStrand(thys->Strand),
	       featFrame(thys->Frame) ) ;

  if(thys->Flags) {
    ajFmtPrintS(&flagdata, "0x%x", thys->Flags);
  }
  if (thys->Start2) {
    if (ajStrLen(flagdata)) ajStrAppC (&flagdata, " ");
    ajFmtPrintAppS (&flagdata, "start2:%d", thys->Start2);
  }
  if (thys->End2) {
    if (ajStrLen(flagdata)) ajStrAppC (&flagdata, " ");
    ajFmtPrintAppS (&flagdata, "end2:%d", thys->End2);
  }
  if (ajStrLen(thys->Remote)) {
    if (ajStrLen(flagdata)) ajStrAppC (&flagdata, " ");
    ajFmtPrintAppS (&flagdata, "remoteid:%S", thys->Remote);
  }
  if (ajStrLen(thys->Label)) {
    if (ajStrLen(flagdata)) ajStrAppC (&flagdata, " ");
    ajFmtPrintAppS (&flagdata, "label:%S", thys->Label);
  }

  /* group and flags */

  (void) ajFmtPrintF (file, "Sequence \"%S.%d\"",
		      owner->Name, thys->Group) ;
  i++;

  if(ajStrLen(flagdata)) {
/*
** Move this code up to run for all features - to preserve the order
** when rewriting in EMBL format
//    if ( FEATFLAG_MULTIPLE){
//      (void) ajFmtPrintF (file, "Sequence \"%S.%d\" ; ",
//			  owner->Name, thys->Group) ;
//      i++;
//    }
*/
    if (i++)
      (void) ajFmtPrintF (file, " ; ") ;
    (void) ajFmtPrintF (file, "FeatFlags \"%S\"", flagdata) ;
  }

  /* For all tag-values... */

  iter = ajFeatTagIter (thys);

  while (ajListIterMore(iter)) {
    tv = ajListIterNext(iter);
    outtag = featTableTag (tv->Tag, FeatTagsTableGff);
    outfmt = featTagFmt (outtag, FeatTagsTableGff);
    ajDebug("Tag '%S' => '%S' %S '%S'\n",
	    tv->Tag, outtag, outfmt, tv->Value);
    if (i++)
      (void) ajFmtPrintF (file, " ; ") ;
    ajFmtPrintAppS (&outstr, "%S", outtag);

    if (tv->Value) {
      ajStrAssS(&tmpval, tv->Value);
      cp = ajStrStr(outfmt);
      switch (CASE2(cp[0], cp[1])) {
      case CASE2('L','I') :	/* limited */
      case CASE2('Q', 'L') :	/* limited, escape quotes */
	ajDebug ("case limited\n");
	tmplim = featTagLimit (outtag, FeatTagsTableGff);
	featTagAllLimit (&tmpval, tmplim);
	ajFmtPrintAppS (&outstr, " %S", tmpval);
	ajStrDel(&tmplim);
	break;
      case CASE2('T','E') :	/* no space, no quotes, wrap at margin */
	ajDebug ("case text\n");
	ajStrCleanWhite(&tmpval);
	ajFmtPrintAppS (&outstr, " %S", tmpval);
	break;
      case CASE2('Q','T') :	/* escape quotes, wrap at space */
	ajDebug ("case qtext\n");
	/*featTagGffQuote(&tmpval);*/ /* should be \" already? */
	ajFmtPrintAppS (&outstr, " %S", tmpval);
	break;
      case CASE2('Q', 'S') :	/* special regexp, quoted */
	ajDebug ("case qspecial\n");
	if (!featTagGffSpecial(&tmpval, outtag)) {
	  featTagGffDefault(&outstr, outtag, &tmpval);
	}
	else {
	  /*featTagGffQuote(&tmpval);*/ /* should be \" already? */
	  ajFmtPrintAppS (&outstr, " %S", tmpval);
	}
	break;
      case CASE2('S','P') :	/* special regexp */
	ajDebug ("case special\n");
	if (!featTagGffSpecial(&tmpval, outtag)) {
	  featTagGffDefault(&outstr, outtag, &tmpval);
	}
	else {
	  ajFmtPrintAppS (&outstr, " %S", tmpval);
	}
	break;
      case CASE2('V','O') :	/* no value, so an error here */
	ajDebug ("case void\n");
	break;
      default:
	ajWarn("Unknown GFF feature tag type '%S' for '%S'",
	       outfmt, outtag);
      }
    }
    else {
      ajDebug ("no value, hope it is void: '%S'\n", outfmt);
    }

    ajFmtPrintF (file, "%S", outstr);
    ajStrDelReuse(&outstr);
    ajStrDel(&outval);
  }

  ajListIterFree(iter);

  (void) ajFmtPrintF (file, "\n") ;

  ajStrDel (&flagdata);

  return;
}

/* @funcstatic featTagFmt **************************************************
**
** Converts a feature tag value into the correct format, after
** checking it is an acceptable value
**
** @param [R] name  [AjPStr] Tag name
** @param [R] table [AjPTable] Tag table
** @return [AjPStr] string with formatted value.
** @@
******************************************************************************/

static AjPStr featTagFmt (AjPStr name, AjPTable table) {

  static AjPStr ret = NULL;

  static AjPStr valtype = NULL;

  static AjPStr tagstr = NULL;
  static AjPRegexp ExpTag = NULL;

  if (!ExpTag)
    ExpTag = ajRegCompC ("([^;]*);");

  tagstr = (AjPStr) ajTableGet (table, name);

  ajRegExec (ExpTag, tagstr);
  ajRegSubI (ExpTag, 1, &valtype);

  ajDebug ("featTagFmt '%S' type '%S' (%S)\n",
	   name, valtype, tagstr);

  ajStrAssS (&ret, valtype);

  return ret;
}

/* @funcstatic featTagLimit ************************************************
**
** Returns the controlled vocabulary list for a limited value.
**
** @param [R] name  [AjPStr] Tag name
** @param [R] table [AjPTable] Tag table
** @return [AjPStr] string with formatted value.
** @@
******************************************************************************/

static AjPStr featTagLimit (AjPStr name, AjPTable table) {

  static AjPStr ret = NULL;

  static AjPStr vallist = NULL;

  static AjPStr tagstr = NULL;
  static AjPRegexp ExpTag = NULL;

  if (!ExpTag)
    ExpTag = ajRegCompC ("[^;]*;(.*)");

  tagstr = (AjPStr) ajTableGet (table, name);

  ajRegExec (ExpTag, tagstr);
  ajRegSubI (ExpTag, 1, &vallist);

  ajDebug ("featTagLimit '%S' list '%S' (%S)\n",
	   name, vallist, tagstr);

  ajStrAssS (&ret, vallist);

  return ret;
}

/* @func ajFeatExit ***********************************************************
**
** Prints a summary of file usage with debug calls
**
** Cleans up feature table internal memory
**
** @return [void]
** @@
******************************************************************************/

void ajFeatExit (void) {
  ajint i;

  for(i=1;featInFormat[i].Name;i++){
    if (featInFormat[i].Used) {
      if (!featInFormat[i].DelReg()) {
	ajDebug("No DelReg yet for %s\n",featInFormat[i].Name);
	ajErr ("No DelReg yet for %s\n",featInFormat[i].Name);
      }
    }
  }

  ajStrTableFree(&FeatTypeTableAcedb);
  ajStrTableFree(&FeatTagsTableAcedb);

  ajStrTableFree(&FeatTypeTableEmbl);
  ajStrTableFree(&FeatTagsTableEmbl);

  ajStrTableFree(&FeatTypeTableGff);
  ajStrTableFree(&FeatTagsTableGff);

  ajStrTableFree(&FeatTypeTableSwiss);
  ajStrTableFree(&FeatTagsTableSwiss);

  ajStrTableFree(&FeatTypeTable);
  ajStrTableFree(&FeatTagsTable);

  return;
}

/* @func ajFeatUnused **************************************************
**
** Dummy function to prevent compiler warnings
**
******************************************************************************/

void ajFeatUnused (void) {
  if (!DummyRegExec)
    DummyRegExec = ajRegCompC(".*");

  (void) featTag (NULL);
  (void) featTableTagC (NULL, NULL);
  (void) featTagvalNewC (NULL, NULL);
}
