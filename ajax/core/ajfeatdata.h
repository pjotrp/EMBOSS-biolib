#ifdef __cplusplus
extern "C"
{
#endif

/*
**
** ajfeatdata.h - AJAX Sequence Feature include file
**            Version 1.0 - March 2001
**
*/

#ifndef ajfeatdata_h
#define ajfeatdata_h

/****************************************************************************
** Table Classes  ***********************************************************
******************************************************************************/

/* @data AjPFeattabIn *******************************************************
**
** Ajax class for feature table input
**
** @alias AjSFeattabIn
** @alias AjOFeattabIn
**
** @new    ajFeatUfoProcess Constructor based upon a 'UFO' and file mode (rwa)
** @delete ajFeattabInClose Destructor, closes file and releases resources
** @@
******************************************************************************/

typedef struct AjSFeattabIn {
   AjPStr        Ufo;
   AjPStr        Formatstr;
   ajint         Format;
   AjPStr        Filename;
   AjPStr        Entryname;
   AjPStr        Type;
   AjPFileBuff   Handle ;
   AjPStr        Seqname ;  /* name of AjPSeq assoc. with feature table */
}  AjOFeattabIn, *AjPFeattabIn ;  

/* @data AjPFeattabOut *******************************************************
**
** Ajax class for feature table output
**
** @alias AjSFeattabOut
** @alias AjOFeattabOut
**
** @new    ajFeatUfoProcess Constructor based upon a 'UFO' and file mode (rwa)
** @delete ajFeattabOutClose Destructor, closes file and releases resources
** @@
******************************************************************************/

typedef struct AjSFeattabOut {
   AjPStr        Ufo;
   AjPStr        Formatstr;
   ajint         Format;
   AjPStr        Filename;
   AjPStr        Entryname;
   AjPStr        Type;
   AjPFile       Handle ;
   AjPStr        Seqname ;  /* name of AjPSeq assoc. with feature table */
}  AjOFeattabOut, *AjPFeattabOut ;  


/* @data AjPFeattable *******************************************************
**
** Ajax data type for collecting AjPFeatures in a 'feature table'.
** The features themselves may be accessed via iteration methods.
**
** @alias AjSFeattable
** @alias AjOFeattable
**
** @new    ajFeattableNew        Constructor
** @delete ajFeattabDel          Default destructor
** @mod    ajFeattabAdd          Adds an AjPFeature to a set
** @mod    ajFeaturesRead        Reads in a feature set in a specified format
** @@
******************************************************************************/

typedef struct AjSFeattable {
  AjPStr            Name ;	/* Sequence name */
  AjPStr            Type ;	/* Sequence type: P or N */ 
  /*float             Version ;*/	/* GFF version */
  /*AjPTime           Date ;*/
  ajint             DefFormat ; /* Original input or 'source' format
				   of the feature table */
  /* AjPStr            DefSource ;*/
  AjPList           Features ;	/* List of AjPFeatures... */
  ajint             Start;      /* First and last position used */
  ajint             End;        /* Rather like begin and end for sequences */
  ajint             Groups;
}  AjOFeattable, *AjPFeattable ;  


/******************************************************************************
** Feature Classes  ***********************************************************
******************************************************************************/


/* @data AjPFeature *******************************************************
**
** Ajax Biological Feature object superclass.
**
** Holds generic data describing a single genome feature.
**
** A feature is a description of a
** sequence location which was determined by some 'source' analysis
** (which may be of 'wet lab' experimental or 'in silico'
** computational nature), has a 'primary' descriptor ('Type'),
** may have some 'score' asserting the level of analysis confidence in
** its identity (e.g. log likelihood relative to a null hypothesis or
** other similar entity), has a 'Location' in the genome, and may have
** any arbitrary number of descriptor Tags and TagValues associated with it.
**
**
** @alias AjSFeature
** @alias AjOFeature
**
** @new    ajFeatNew             Constructor - must specify the associated
**                               (non-null) AjPFeattable?
** @new    ajFeatNewFromTPS      Constructor: given type, position & score,
**                               assumes default Source
** @new    ajFeatNewFromPS       Constructor: given position & score,
**                               assumes default Source and Type 
** @delete ajFeatDel             Default destructor
** @assign ajFeatCopy            Copy constructor
** @set    ajFeatSetSource       Sets the name of the analysis which
**                               ascertained the feature
** @set    ajFeatSetType         Sets the type of feature (e.g. exon, etc.)
** @set    ajFeatSetScore        Sets the score associated with the feature,
**                               if any
** @set    ajFeatSetTagValue     Sets a specified tag for a feature and any
**                               (optional) value associated with it
**                               If a value is unspecified (NULL), then the
**                               current value associated with the tag
**                               if returned. If a new value is specified,
**                               the old value is returned.
** @set    ajFeatClearTags       Clears all tags (except Source and Type tag)
** @set    ajFeatAddSubFeature   Adds a subsidiary AjFeature to the feature
** @cast   ajFeatSource          Returns the name of the analysis which
**                               ascertained the feature
** @cast   ajFeatType            Returns the type of feature (e.g. exon, etc.) 
** @cast   ajFeatScore           Returns the score associated with the
**                               feature, if any
** @cast   ajFeatTag             Returns the value associated with a
**                               specified tag associated with the feature
** @cast   ajFeatTagSet          Returns a list of the tags associated
**                               with the feature, including the source and
**                               type tags
** @cast   ajFeatSubFeatures     Returns the AjPFeattable of subfeatures of
**                               the feature
** @mod    ajFeatFromLine        Parses in a string, in a specified feature
**                               format
** @use    ajFeatDumpString      Dumps the feature to a string, in a
**                               specified format
** @use    ajFeatDump            Dumps the feature to a file, in a specified
**                               format
** @@
******************************************************************************/

typedef struct AjSFeature {
  AjPStr            Source ;
  AjPStr            Type ;
  ajint               Start ;
  ajint               End;
  ajint               Start2;
  ajint               End2;
  float             Score ;
  AjPList           Tags ; /* a.k.a. the [group] field tag-values of GFF2 */
  AjPStr            Comment ;
  char              Strand ;
  ajint               Frame ;
  AjPStr            desc ;
  ajint               Flags;
  ajint               Group;
  ajint               Exon;
  AjPStr            Remote ;
  AjPStr            Label ;
} AjOFeature, *AjPFeature ;


#endif /* ajfeatdata_h */

#ifdef __cplusplus
}
#endif
