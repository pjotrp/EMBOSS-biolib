#include "emboss.h"

int main (int argc, char **argv) {
  AjPFeattable feattable;
  AjPStr name=NULL,desc=NULL,source=NULL,type=NULL;
  char strand='+';
  ajint frame=0;
  AjBool sortbytype,dictionary,sortbystart,tracedict;
  AjPFile file;
  AjPFeature feature;
  AjPFeattabOut output = NULL;
  ajint i;
  float score = 0.0;

  embInit ("demofeatures", argc, argv);

  /*  file =        ajAcdGetOutfile("outfile");*/
  output     =  ajAcdGetFeatout("featout");
  dictionary =  ajAcdGetBool("dictionary");
  sortbytype =  ajAcdGetBool("typesort");
  sortbystart = ajAcdGetBool("startsort");
  tracedict =   ajAcdGetBool("tracedict");

  /* first read the dictionary if one is to be used */

  ajStrAssC(&name,"seq1");

  feattable = ajFeattableNew(name);

  ajStrAssC(&source,"demofeature");
  score = 1.0;
  
  for(i=1;i<11;i++){
    if(i & 1)
      ajStrAssC(&type,"CDS");
    else
      ajStrAssC(&type,"misc_feature");

    feature = ajFeatureNew(feattable, source, type,
			  i, i+10, score, strand, frame,
			  desc , 0, 0) ;    

  }
  
  
  if(sortbytype)
    ajFeatSortByType(feattable);
  if(sortbystart)
    ajFeatSortByStart(feattable);

  ajFeaturesWrite (output, feattable);
  
  ajStrDel(&name);
  ajStrDel(&type);

  return 0;
}
