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
** Ajax Collection base class for AjPFeatures. The table may have a default
** data Format, a format 'Version' and a 'Data' timestamp associated with it.
** The table maintains a 'dictionary' of tags associated with the 'Source', 
** 'Type' and 'Tag' fields of its component AjPFeatures. The features
** themselves may be accessed via iteration methods.
**
** @alias AjSFeattable
** @alias AjOFeattable
**
** @new    ajFeattabNew          Constructor
** @delete ajFeattabDel          Default destructor
** @assign ajFeattabCopy         Copy constructor
** @mod    ajFeattabAdd          Adds an AjPFeature to a set
** @mod    ajFeattabSetVersion   (Re)sets the current  AjPFeattable data
**                               format version
** @mod    ajFeattabSetDate      (Re)sets the current date of the 
**                               AjPFeattable (file) dataset
** @mod    ajFeattabDefFormat    Sets the original input ('source') data
**                               format for the table.
** @mod    ajFeattabDefSource    Sets default 'Source' label for features
**                               created without source label specified
** @mod    ajFeattabDefType      Sets default 'Type' label for features
**                               created without type label specified
** @cast   ajFeattabFormat       Returns the current AjPFeattable data
**                               format type
** @cast   ajFeattabVersion      Returns the current AjPFeattable data
**                               format version
** @cast   ajFeattabDate         Returns the date of the AjPFeattable
**                               (file) dataset
** @cast   ajFeattabDictionary   Returns the AjFeatLexicon 'dictionary'
**                               of feature tags
** @cast   ajFeattabFirst        Iterator: returns the first AjPFeature
**                               of a set
** @cast   ajFeattabNext         Iterator: returns the next AjPeature of
**                               a set (ajFeattabFirst method must be called
**                               first before using this method)
** @cast   ajFeattabCount        Returns the number of features in the
**                               feature set
** @set    ajFeattabMerge        Merges another AjPFeattable to the current one
** @mod    ajFeattabRead         Reads in a feature set in a specified format
** @set    ajFeattabHeader       Dumps out the (Format specific) header
**                               describing a feature set
** @set    ajFeattabDump         Dumps the feature set to a file, in a
**                               specified format
** @use    ajFeattabDumpMatches  Dumps the feature set to a file, with
**                               information about features cross matched
**                               between different feature tables.
**
** Functions being contemplated but not yet implemented:
**
** #cast   ajFeattabMember       Predicate to test AjFeature membership in
**                               a table
** #set    ajFeattabUnion        Returns 'union set' of two feature tables
** #set    ajFeattabIntersection Returns 'intersection set' of two feature
**                               tables
** #set    ajFeattabDifference   Returns 'difference set' of one 
**                               feature table 'subtracted' again another
** #mod    ajFeattabSort         Sorts a feature table based upon user
**                               defined criteria
** #set    ajFeattabFilter       Returns a new feature table based 
**                               upon a filtered version of an existing one
** #mod    ajFeattabRewrite      Method to systematically rewrite source,
**                               type and tag fields
**                               of all the AjFeature's in the table
** #set    ajFeattabCluster      Returns an array of feature tables each
**                               containing a group of features sharing some
**                               attribute (by pairwise comparison) 
** #set    ajFeattabGroup        Returns an table of feature tables each
**                               containing a group of features sharing
**                               some fixed, named attribute 
** #set    ajFeattabRange        Returns the features of a feature table 
**                               which lie within a given AjRange
** #set    ajFeattabOverlapSelf  Returns a feature table filtered for
**                               'self overlaps'
** #cast   ajFeattabOverlapStats Returns summary statistics about feature
**                               table matches
** #set    ajFeattabStudy        Performs a general analysis of a feature
**                               table, for *Range, *Score cast's
** #cast   ajFeattabMinScore     Returns the minimum score of a feature table
** #cast   ajFeattabMaxScore     Returns the maximum score of a feature table
** #cast   ajFeattabAveScore     Returns the average score of a feature table
** @@
******************************************************************************/

typedef struct AjSFeattable {
  AjPStr            Name ;
  AjPStr            Type ;
  float             Version ;	/* GFF version */
  AjPTime           Date ;
  ajint             DefFormat ; /* Original input or 'source' format
				   of the feature table */
  AjPStr            DefSource ;
  AjPList           Features ;	/* List of AjPFeatures... */
  ajint             Start;	/* a.k.a. GFF region... */
  ajint             End;
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
