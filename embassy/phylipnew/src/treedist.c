
#include "phylip.h"
#include "cons.h"

/* version 3.6. (c) Copyright 1993-2002 by the University of Washington.
   Written by Dan Fineman, Joseph Felsenstein, Hisashi Horino,
   Akiko Fuseki, Sean Lamont, and Andrew Keeffe.
   Permission is granted to copy and use this program provided no fee
   is charged for it and provided that this copyright notice is not removed. */

long output_scheme ;

extern long tree_pairing ;

/* The following extern's refer to things declared in cons.c */

extern Char intreename[FNMLNGTH], intree2name[FNMLNGTH];
const char* outfilename;
const char* outtreename;
extern node *root;

extern long numopts, outgrno, col, setsz;
extern long maxgrp;               /* max. no. of groups in all trees found  */

extern boolean trout, firsttree, noroot, outgropt, didreroot, prntsets,
               progress, treeprint, goteof;
extern pointarray treenode, nodep;
extern group_type **grouping, **grping2, **group2;/* to store groups found  */
extern long **order, **order2, lasti;
extern group_type *fullset;
extern node *grbg;
extern long tipy;

extern double **timesseen, **tmseen2, **times2 ;
extern double trweight, ntrees ;

AjPPhyloTree* phylotrees = NULL;
AjPPhyloTree* phylomoretrees = NULL;

#ifndef OLDC
/* function prototpes */
void   assign_tree(group_type **, pattern_elm ***, long, long *);
boolean group_is_null(group_type **, long);
long    tree_diff(group_type **, group_type **, long, long);
void   compute_distances(pattern_elm ***, long, long, long *);
void   free_patterns(pattern_elm ***, long); 
void   produce_square_matrix(long, long *);
void   produce_full_matrix(long, long, long *);
void   output_distances(long, long, long *); 
void   output_submenu(void);
void   pairing_submenu(void);

void   read_second_file(pattern_elm ***, double *, long *, long *);
/*void   getoptions(void);*/
void emboss_getoptions(char *pgm, int argc, char *argv[]);
/* function prototpes */
#endif


void assign_tree(group_type **treeN, pattern_elm ***pattern_array,
                long tree_index, long *pattern_size) 
{ /* set treeN to be the tree_index-th tree in pattern_elm */
  long i ;

  for (i = 0 ; i < setsz ; i++)
    {
      treeN[i] = pattern_array[i][tree_index]->apattern ;
    }
  *pattern_size = *pattern_array[0][tree_index]->patternsize;
}  /* assign_tree */


boolean group_is_null(group_type **treeN, long index)
{
  /* Check to see if a given index to a tree array points to an empty
     group */
  long i ;

  for (i = 0 ; i < setsz ; i++)
    if (treeN[i][index] != (group_type) 0)
      return false ;

  /* If we've gotten this far, then the index is to an empty group in
     the tree. */
  return true ;
}  /* group_is_null */


long tree_diff(group_type **tree1, group_type **tree2,
                long patternsz1, long patternsz2)
{
  /* Compute the symmetric difference between 2 given trees. Return
     that value as a long. */

  long index1, index2, return_value = 0 ;
  boolean match_found ;
  long i;

  if (group_is_null (tree1, 0) || group_is_null (tree2, 0))
    {
      printf ("Error computing tree difference.\n") ;
      return 0;
    }

  for (index1 = 0 ; index1 < patternsz1 ; index1++)
    {
      /* For every element in the first tree, see if there's
         a match to it in the second tree. */
      match_found = false ;
      if (group_is_null (tree1, index1))
        {
          /* When we've gone over all the elements in tree1, greater
             number of elements in tree2 will constitute that much more
             of a difference... */
          while (! group_is_null (tree2, index1)) 
            {
              return_value++ ;
              index1++ ;
/*              printf ("Found null group %ld, return value at %ld. . .\n", 
                      index1-1, return_value) ; */
            }
          break ;
        }

      for (index2 = 0 ; index2 < patternsz2 ; index2++)
         {
          /* For every element in the second tree, see if any match
             the current element in the first tree. */
          if (group_is_null (tree2, index2)) {
              /* When we've gone over all the elements in tree2 */
              match_found = false ;
              break ;
            }
          else 
            {
              /* Tentatively set match_found; will be changed later if
                 neccessary. . . */
              match_found = true ;  

              for (i = 0 ; i < setsz ; i++) {
                  /* See if we've got a match, */ 
                  if (tree1[i][index1] != tree2[i][index2])
                    match_found = false ;
                }

              if (match_found == true)
                /* If the previous loop ran from 0 to setsz without setting
                   match_found to false, */
                break ;
            }
        }

      if (match_found == false) {
          return_value++ ;
        }
    }
  return return_value ;
}  /* tree_diff */


void compute_distances(pattern_elm ***pattern_array, long trees_in_1,
                long trees_in_2, long *diff_array)
{
  /* Compute symmetric distances between arrays of trees */
  long i, tree_index, diff1, diff2, end_tree, index1, index2,
    diff_index ;
  group_type **treeA, **treeB ;
  long patternsz1, patternsz2;

  diff_index = 0 ;

  /* Put together space for treeA and treeB */
  treeA = (group_type **) Malloc (setsz * sizeof (group_type *)) ;
  treeB = (group_type **) Malloc (setsz * sizeof (group_type *)) ;

  for (i=0 ; i<setsz; i++) {
    treeA[i] = (group_type *) Malloc (maxgrp * sizeof (group_type)) ;
    treeB[i] = (group_type *) Malloc (maxgrp * sizeof (group_type)) ;
  }
        
  switch (tree_pairing) {
  case (ADJACENT_PAIRS) : 
    end_tree = trees_in_1 - 1 ;
    for (tree_index = 0 ; tree_index < end_tree ; tree_index += 2)
      {
        /* For every tree, compute the distance between it and the tree
           at the next location; do this in both directions */

        assign_tree (treeA, pattern_array, tree_index, &patternsz1) ;
        assign_tree (treeB, pattern_array, tree_index + 1, &patternsz2) ;

        diff1 = tree_diff (treeB, treeA, patternsz2, patternsz1) ;        
        diff2 = tree_diff (treeA, treeB, patternsz1, patternsz2) ;
        diff_array[diff_index++] = diff1 + diff2 ;
        if (tree_index + 2 == end_tree)
          printf("\nWARNING: extra tree at the end of input tree file.\n");
      }
    break ;

  case (ALL_IN_FIRST) : 
    end_tree   = trees_in_1 ;

    for (index1 = 0 ; index1 < end_tree ; index1++)
      {
        /* For every tree, compute the distance between it and every
           other tree in that file. */
        assign_tree (treeA, pattern_array, index1, &patternsz1) ;

        for (index2 = 0 ; index2 < end_tree ; index2++)
          {
            if (index1 == index2)
              {
                /* No need to compute the distance between a tree and
                   itself, */
                diff_array[diff_index++] = 0 ;
                continue ;
              }
            else if (index1 > index2) 
              {
                /* No need to re-compute something we've already done, */
                diff_array[diff_index++] = 
                  diff_array[(index2 * trees_in_1) + index1] ;
              }
            else 
              {
                assign_tree (treeB, pattern_array, index2, &patternsz2) ;
                diff1 = tree_diff (treeB, treeA, patternsz2, patternsz1) ;        
                diff2 = tree_diff (treeA, treeB, patternsz1, patternsz2) ;
                diff_array[diff_index++] = diff1 + diff2 ;
              }
          
          }
      }
    break ;

  case (CORR_IN_1_AND_2) :
    if (trees_in_1 != trees_in_2)
      {
        /* Print something out to the outfile and to the terminal. */
        fprintf (outfile, "\n\n") ;
        fprintf (outfile, "*** Warning: differing number of trees in first and second\n") ;
        fprintf (outfile, "*** tree files.  Only computing %ld pairs.\n\n",
                 trees_in_1 > trees_in_2 ? trees_in_2 : trees_in_1) ;

        printf ("\n *** Warning: differing number of trees in first and second\n") ;
        printf (" *** tree files.  Only computing %ld pairs.\n\n",
                 trees_in_1 > trees_in_2 ? trees_in_2 : trees_in_1) ;

        /* Set end tree to the smaller of the two totals. */
        end_tree = trees_in_1 > trees_in_2 ? trees_in_2 : trees_in_1 ;
      }
    else
      end_tree = trees_in_1 ;

    for (tree_index = 0 ; tree_index < end_tree ; tree_index++)
      {
        /* For every tree, compute the distance between it and the
           tree at the parallel location in the other file; do this in
           both directions */

        assign_tree (treeA, pattern_array, tree_index, &patternsz1) ;

        /* (tree_index + trees_in_1) will be the corresponding tree in
           the second file. */
        assign_tree (treeB, pattern_array, tree_index + trees_in_1, &patternsz2) ;
        diff1 = tree_diff (treeB, treeA, patternsz2, patternsz1) ;
        diff2 = tree_diff (treeA, treeB, patternsz1, patternsz2) ;
        diff_array[tree_index] = diff1 + diff2 ;
      }
    break ; 

  case (ALL_IN_1_AND_2) :
    end_tree = trees_in_1 + trees_in_2 ;

    for (tree_index = 0 ; tree_index < trees_in_1 ; tree_index++)
      {
        /* For every tree in the first file, compute the distance
           between it and every tree in the second file. */

        assign_tree (treeA, pattern_array, tree_index, &patternsz1) ;

        for (index2 = trees_in_1 ; index2 < end_tree ; index2++)
          {
            assign_tree (treeB, pattern_array, index2, &patternsz2) ;

            diff1 = tree_diff (treeB, treeA, patternsz2, patternsz1) ;        
            diff2 = tree_diff (treeA, treeB, patternsz1, patternsz2) ;
            
            diff_array[diff_index++] = diff1 + diff2 ;
          }
      }

    for ( ; tree_index < end_tree ; tree_index++)
      {
        /* For every tree in the second file, compute the distance
           between it and every tree in the first file. */

        assign_tree (treeA, pattern_array, tree_index, &patternsz1) ;

        for (index2 = 0 ; index2 < trees_in_1 ; index2++)
          {
            assign_tree (treeB, pattern_array, index2, &patternsz2) ;

            diff1 = tree_diff (treeB, treeA, patternsz2, patternsz1) ;        
            diff2 = tree_diff (treeA, treeB, patternsz1, patternsz2) ;
            
            diff_array[diff_index++] = diff1 + diff2 ;
          }
      }
    break ; 
  }

  /* Free up treeA and treeB */
  for (i=0 ; i<setsz; i++)
    {
      free (treeA[i]) ;
      free (treeB[i]) ;
    }
  free (treeA) ;
  free (treeB) ;        
}  /* compute_distances */


void free_patterns(pattern_elm ***pattern_array, long total_trees) 
{
  long i, j ;
  long end_pattern = total_trees - 1 ;

  /* Free each pattern array, */
  for (i=0 ; i < setsz ; i++)
    {
      for (j = 0 ; j < end_pattern ; j++) {
        free (pattern_array[i][j]->apattern) ;
        free (pattern_array[i][j]->patternsize) ;
      }
      free (pattern_array[i]) ;
    }
  free (pattern_array) ;
}  /* free_patterns */


void produce_square_matrix(long trees_in_1, long *diff_array)
{
  long i, j ;
  long block_start, block_end, block_index, end_block ;

  end_block         = (int) ceil ((double) trees_in_1 /
                                  (double) COLUMNS_PER_BLOCK) ;
  block_start       = 0 ;
  block_end         = 0 ;

  for (block_index = 0 ; block_index < end_block; block_index++)
    {
      block_start  = block_index * COLUMNS_PER_BLOCK ;
      block_end    = block_start + COLUMNS_PER_BLOCK ;

      if (block_end > trees_in_1) 
        block_end = trees_in_1 ;

      /* Leading spaces in the top line */
      fprintf (outfile, "         ") ;

      /* Here's the top line index, */
      for (i = block_start ; i < block_end ; i++) 
        fprintf (outfile,"%2ld    ", i+1) ;
        
      /* And then a delimiting line. */
      fprintf (outfile,"\n      \\") ;
      for (i=block_start ; i < block_end ; i++)
        fprintf (outfile,"------") ;
      
      fprintf (outfile,"\n") ;

      for (i = 0 ; i < trees_in_1 ; i++)
          {
            fprintf (outfile," %4ld |", i+1) ;   /* The row indicator, */
            for (j = 0 ; j < block_end - block_start ; j++)
            {
               fprintf (outfile, "%4ld  ", 
                       diff_array [(i * trees_in_1)  +
                       (block_index * COLUMNS_PER_BLOCK) +
                       j]) ;
            }
            fprintf (outfile,"\n") ;
          }
      fprintf (outfile,"\n\n") ;
    }
}  /* produce_square_matrix */


void produce_full_matrix(long trees_in_1, long trees_in_2,
                long *diff_array)
{
  long i, j, block_start, block_end, block_index, end_block ;

  end_block         = ((double) trees_in_2  /
                       (double) COLUMNS_PER_BLOCK) + 1 ;

  block_start = trees_in_1 ;
  block_end   = 0 ;

  /* Print out the top row, */
  fprintf (outfile,"\n\n") ;
  fprintf (outfile, "First\\  Second tree file:\n") ;
  fprintf (outfile, "tree  \\\n") ;
  fprintf (outfile, "file:  \\   ") ;

  for (block_index = 0 ; block_index < end_block; block_index++)
    {
      block_start += block_index * COLUMNS_PER_BLOCK ;
      block_end    = block_start + COLUMNS_PER_BLOCK ;

      if (block_end > (trees_in_1 + trees_in_2))
        block_end = (trees_in_1 + trees_in_2) ;

      /* This is the top row for the block, listing trees in the
         second file. */
      if (block_start != trees_in_1) 
        /* For blocks beyond the first, */
        fprintf (outfile,"           ") ;

      /* Here's the top line index, */
      for (i=block_start ; i < block_end ; i++)
        fprintf (outfile,"%2ld    ", (i+1) - trees_in_1) ;

      /* And then a delimiting line. */
      fprintf (outfile,"\n        \\") ;
      for (i=block_start ; i < block_end ; i++)
        fprintf (outfile,"------") ;

      fprintf (outfile,"\n") ;
      
      for (i = 0 ; i < trees_in_1 ; i++)
        {
          fprintf (outfile,"  %2ld    |", i+1) ;   /* The row indicator, */
          for (j = 0 ; j < block_end - block_start ; j++)
            {
              fprintf (outfile, "%4ld  ", 
                       diff_array [(i * trees_in_2)  +
                       (block_index * COLUMNS_PER_BLOCK) +
                       j]) ;
            }
          fprintf (outfile,"\n") ;
        }
      fprintf (outfile,"\n\n") ;
    }
} /* produce_full_matrix */


void output_distances(long trees_in_1, long trees_in_2, long *diff_array) 
{
  long i, j, end_tree, diff_index ;

  diff_index = 0 ;

  switch (tree_pairing) {
  case (ADJACENT_PAIRS) : 
    end_tree = trees_in_1 - 1 ;

    if (output_scheme == VERBOSE) 
      {
        fprintf(outfile, "\nTree distance program, version %s\n\n", VERSION);
        fprintf (outfile, 
             "Symmetric differences between adjacent pairs of trees:\n\n");
        for (i = 0 ; i < end_tree ; i += 2)
          fprintf (outfile, "Trees %ld and %ld:    %ld\n", 
                   i+1, i+2, diff_array[diff_index++]) ;
        fprintf(outfile, "\n");
      }
    else if (output_scheme == SPARSE)
      {
        for (i = 0 ; i < end_tree ; i += 2)
          fprintf (outfile, "%ld %ld %ld\n", 
                   i+1, i+2, diff_array[diff_index++]) ;
      }
    else
      printf ("Error -- cannot output adjacent pairs into a full matrix.\n") ;

    break ;

  case (ALL_IN_FIRST) : 
    end_tree   = trees_in_1 ;

    if (output_scheme == VERBOSE) 
      {
        fprintf(outfile, "\nTree distance program, version %s\n\n", VERSION);
        fprintf (outfile, 
         "Symmetric differences between all pairs of trees in tree file:\n\n");
        for (i=0; i<end_tree; i++)
          for (j=0 ; j<end_tree; j++)
            fprintf (outfile, "Trees %ld and %ld:    %ld\n", 
                     i+1, j+1, diff_array[diff_index++]) ;
        fprintf(outfile, "\n");
      }
    else if (output_scheme == SPARSE)
      {
        for (i=0; i<end_tree; i++)
          for (j=0 ; j<end_tree; j++)
            fprintf (outfile, "%ld %ld %ld\n", 
                     i+1, j+1, diff_array[diff_index++]) ;
            
      }
    else if (output_scheme == FULL_MATRIX)
      {
        fprintf(outfile, "\nTree distance program, version %s\n\n", VERSION);
        fprintf (outfile, 
         "Symmetric differences between all pairs of trees in tree file:\n\n");
        produce_square_matrix (trees_in_1, diff_array) ;
      }
    break ;

  case (CORR_IN_1_AND_2) :

    if (trees_in_1 != trees_in_2)
      end_tree = trees_in_1 > trees_in_2 ? trees_in_2 : trees_in_1 ;
    else
      end_tree = trees_in_1 ;

    if (output_scheme == VERBOSE)
      {
        fprintf(outfile, "\nTree distance program, version %s\n\n", VERSION);
        fprintf (outfile, 
             "Symmetric differences between corresponding pairs of trees\n");
        fprintf (outfile, 
             "   from first and second tree files:\n\n");
        for (i = 0 ; i < end_tree ; i++)
          fprintf (outfile, "Tree pair %ld:    %ld\n", 
                   i+1, diff_array[i]) ;
        fprintf(outfile, "\n");
      }
    else if (output_scheme == SPARSE)
      {
        for (i = 0 ; i < end_tree ; i++)
          fprintf (outfile, "%ld %ld\n", 
                   i+1, diff_array[i]) ;
      }
    else
      printf ("Error -- cannot output corresponding pairs into a full matrix.\n") ;

    break ; 

  case (ALL_IN_1_AND_2) :
    end_tree = trees_in_1 + trees_in_2 ;

    switch (output_scheme) {

    case (VERBOSE) :
      fprintf(outfile, "\nTree distance program, version %s\n\n", VERSION);
      fprintf (outfile, 
             "Symmetric differences between all pairs of trees\n");
      fprintf (outfile, 
             "   from first and second tree files:\n\n");
      for (i = 0 ; i < trees_in_1 ; i++)
        for (j = trees_in_1 ; j < end_tree ; j++)
          {
            fprintf (outfile, "Trees %ld and %ld:    %ld\n", 
                     i+1, j+1, diff_array[diff_index++]) ;
          }
      for (i = trees_in_1; i < end_tree ; i++)
        for (j = 0 ; j < trees_in_1 ; j++)
          {
            fprintf (outfile, "Trees %ld and %ld:    %ld\n", 
                     i+1, j+1, diff_array[diff_index++]) ;
          }
      fprintf(outfile, "\n");
      break ;

    case (SPARSE) :
      for (i = 0 ; i < trees_in_1 ; i++)
        for (j = trees_in_1 ; j < end_tree ; j++)
          {
            fprintf (outfile, "%ld %ld %ld\n", 
                     i+1, j+1, diff_array[diff_index++]) ;
          }
      for (i = trees_in_1; i < end_tree ; i++)
        for (j = 0 ; j < trees_in_1 ; j++)
          {
            fprintf (outfile, "%ld %ld %ld\n", 
                     i+1, j+1, diff_array[diff_index++]) ;
          }
      break ;

    case (FULL_MATRIX) :
      fprintf(outfile, "\nTree distance program, version %s\n\n", VERSION);
      produce_full_matrix (trees_in_1, trees_in_2, diff_array) ;
      break ;
    }
    break ; 
  }
} /* output_distances */


void output_submenu()
{
  /* this allows the user to select a different output of distances scheme. */
/*
//  long loopcount;
//  boolean done = false;
//  Char    ch   ;
*/

    AjPStr stylestr;

    stylestr = ajAcdGetListI("style", 1);

  if (tree_pairing == NO_PAIRING)
    return ;

    if (ajStrMatchC(stylestr, "Full"))
    {
	if ((tree_pairing == ALL_IN_1_AND_2) ||
	    (tree_pairing == ALL_IN_FIRST))
	    output_scheme = FULL_MATRIX;
	else
	{
	    output_scheme = VERBOSE;
	    ajWarn("Full output style only available for Pairs or ListAll");
	}
    }
    else if (ajStrMatchC(stylestr, "Verbose"))
	output_scheme = VERBOSE;
    else if (ajStrMatchC(stylestr, "Sparse"))
	output_scheme = SPARSE;
    else
	ajDie("Unknown 'style' option '%S'", stylestr);

 /*
//  loopcount = 0;
//  while (!done) {
//    printf ("\nDistances output options:\n") ;
//
//    if ((tree_pairing == ALL_IN_1_AND_2) ||
//          (tree_pairing == ALL_IN_FIRST))
//      printf (" F     Full matrix.\n") ;
//      printf (" V     One pair per line, verbose.\n") ;
//      printf (" S     One pair per line, sparse.\n") ;
//      
//      if ((tree_pairing == ALL_IN_1_AND_2) ||
//          (tree_pairing == ALL_IN_FIRST))
//        printf ("\n Choose one: (F,V,S)\n") ;
//      else
//        printf ("\n Choose one: (V,S)\n") ;
//
//      scanf("%c%*[^\n]", &ch);
//      getchar();
//      uppercase(&ch);
//      
//      if (strchr("FVS",ch) != NULL) {
//        switch (ch) 
//          {
//          case 'F':
//            if ((tree_pairing == ALL_IN_1_AND_2) ||
//                (tree_pairing == ALL_IN_FIRST))
//              output_scheme = FULL_MATRIX ;
//            else
//              /# If this can't be a full matrix... #/
//              continue ;
//            break ;
//            
//          case 'V':
//            output_scheme = VERBOSE ;
//            break ;
//            
//          case 'S':
//            output_scheme = SPARSE ;
//            break ;
//          }
//        done = true ;
//        }
//      countup(&loopcount, 10);
//    }
*/

}  /* output_submenu */


void pairing_submenu()
{
  /* this allows the user to select a different tree pairing scheme. */
/*
  long loopcount;
  boolean done = false;
  Char    ch   ;
*/

    AjPStr pairing;

    pairing = ajAcdGetListI("pairing", 1);

    if (ajStrMatchC(pairing, "Adjacent"))
	tree_pairing = ADJACENT_PAIRS;
    else if (ajStrMatchC(pairing, "Pairs"))
	tree_pairing = ALL_IN_FIRST;
    else if (ajStrMatchC(pairing, "Correspond"))
	tree_pairing = CORR_IN_1_AND_2;
    else if (ajStrMatchC(pairing, "ListAll"))
	tree_pairing = ALL_IN_1_AND_2;
    else
	ajDie("Unknown 'pairing' option '%S'", pairing);

      output_submenu() ;

/*
//  loopcount = 0;
//  while (!done) {
//    cleerhome();
//    printf ("Tree Pairing Submenu:\n") ;
//    printf (" A     Distances between adjacent pairs in tree file.\n") ;
//    printf (" P     Distances between all possible pairs in tree file.\n") ;
//    printf (" C     Distances between corresponding pairs in one tree file and another.\n") ;
//    printf (" L     Distances between all pairs in one tree file and another.\n") ;
//
//    printf ("\n Choose one: (A,P,C,L)\n") ;
//      
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    uppercase(&ch);
//
//    if (strchr("APCL",ch) != NULL) {
//      switch (ch) {
//        case 'A':
//          tree_pairing = ADJACENT_PAIRS ;
//          break ;
//              
//        case 'P':
//          tree_pairing = ALL_IN_FIRST ;
//          break ;
//
//        case 'C':
//          tree_pairing = CORR_IN_1_AND_2 ;
//          break ;
//              
//        case 'L':
//          tree_pairing = ALL_IN_1_AND_2 ;
//          break ;
//      }
//      output_submenu() ;
//      done = true ;
//    }
//    countup(&loopcount, 10);
//  }
*/
}  /* pairing_submenu */


void read_second_file(pattern_elm ***pattern_array,
		      double *timesseen_changes,
		      long *trees_in_1, long *trees_in_2)
{
  boolean firsttree2, haslengths, initial;
  long nextnode;
  long j;
  char* treestr;

  firsttree2 = true;
  grbg = NULL;
  initial = true;
  *trees_in_2 = 0;
  while (phylomoretrees[*trees_in_2]) {
    treestr = ajStrStr(phylomoretrees[*trees_in_2]->Tree);
    goteof = false;
    nextnode = 0;
    haslengths = false;
    allocate_nodep(&nodep, treestr, &spp);
    if (firsttree2)
      nayme = (naym *)Malloc(spp*sizeof(naym));
    treeread(&treestr, &root, treenode, &goteof, &firsttree2,
               nodep, &nextnode, &haslengths,
               &grbg, initconsnode);
    if (!initial) { 
      reordertips();
    } else {
      initial = false;
      dupname(root);
      initreenode(root);
    }
    if (goteof)
      continue;
    ntrees += trweight;
    if (noroot) {
      reroot(nodep[outgrno - 1], &nextnode);
      didreroot = outgropt;
    }
    accumulate(root);
    gdispose(root);
    for (j = 0; j < 2*(1 + spp); j++)
      nodep[j] = NULL;
    free(nodep);

    store_pattern (pattern_array, 
                   timesseen_changes,
                   (*trees_in_1) + (*trees_in_2)) ;
    (*trees_in_2)++ ;
  }
  free(nayme);
}  /* read_second_file */



/************ EMBOSS GET OPTIONS ROUTINES ******************************/

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjStatus retval;
 
    /* initialize global variables */

    tree_pairing   = ADJACENT_PAIRS ;
    output_scheme  = VERBOSE ;
    ibmpc          = IBMCRT;
    ansi           = ANSICRT;
    didreroot      = false;
    spp            = 0 ;
    grbg           = NULL;
    col            = 0 ;

    noroot = true;
    numopts = 0;
    outgrno = 1;
    outgropt = false;
    progress = true;

    ajNamInit("emboss");
    retval =  ajAcdInitP (pgm, argc, argv, "PHYLIP");

    /* ajAcdGet */

    progress = ajAcdGetBool("progress");


    /* init functions for standard ajAcdGet */

    pairing_submenu ();

    /* cleanup for clashing options */

}

/************ END EMBOSS GET OPTIONS ROUTINES **************************/

/*
//void getoptions()
//{
//  /# interactively set options #/
//  long loopcount, loopcount2;
//  Char ch;
//  boolean done, done1;
//
//  /# Initial settings #/
//  tree_pairing   = ADJACENT_PAIRS ;
//  output_scheme  = VERBOSE ;
//  ibmpc          = IBMCRT;
//  ansi           = ANSICRT;
//  didreroot      = false;
//  spp            = 0 ;
//  grbg           = NULL;
//  col            = 0 ;
//
//  putchar('\n');
//  noroot = true;
//  numopts = 0;
//  outgrno = 1;
//  outgropt = false;
//  progress = true;
//
//  /# The following are not used by treedist, but may be used
//     in functions in cons.c, so we set them here. #/
//  treeprint = false;
//  trout = false;
//  prntsets = false;
//
//  loopcount = 0;
//  do {
//    cleerhome();
//    printf("\nTree distance program, version %s\n\n", VERSION);
//    printf("Settings for this run:\n");
//    if (noroot) {
//      printf(" O                         Outgroup root:");
//      if (outgropt)
//        printf("  Yes, at species number%3ld\n", outgrno);
//      else
//        printf("  No, use as outgroup species%3ld\n", outgrno);
//      }
//    printf(" R         Trees to be treated as Rooted:");
//    if (noroot)
//      printf("  No\n");
//    else
//      printf("  Yes\n");
//    printf(" T    Terminal type (IBM PC, ANSI, none):");
//    if (ibmpc)
//      printf("  IBM PC\n");
//    if (ansi)
//      printf("  ANSI\n");
//    if (!(ibmpc || ansi))
//      printf("  (none)\n");
//    printf(" 1  Print indications of progress of run:  %s\n",
//           (progress ? "Yes" : "No"));
//
//    /# Added by Dan F. #/
//    printf(" 2                 Tree distance submenu:") ;
//    switch (tree_pairing)
//      {
//      case NO_PAIRING: 
//        printf("\n\nERROR: Unallowable option!\n\n") ;
//        exxit(-1);
//        break ;
//
//      case ADJACENT_PAIRS:
//        printf("  Distance between adjacent pairs\n") ;
//        break ;
//
//      case CORR_IN_1_AND_2: 
//        printf("  Distances between corresponding \n") ;
//        printf("                                             pairs in first and second tree files\n") ;
//        break ;
//
//      case ALL_IN_FIRST: 
//        printf("  Distances between all possible\n") ;
//        printf("                                             pairs in tree file.\n") ;
//        break ;
//
//      case ALL_IN_1_AND_2: 
//        printf("  Distances between all pairs in\n") ;
//        printf("                                              first and second tree files\n") ;
//        break ;
//      }
//
//    printf("\nAre these settings correct? (type Y or the letter for one to change)\n");
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    uppercase(&ch);
//    done = (ch == 'Y');
//    if (!done) {
//      if ((noroot && (ch == 'O')) || strchr("RT12",ch) != NULL) {
//        switch (ch) {
//
//        case 'O':
//          outgropt = !outgropt;
//          if (outgropt) {
//            numopts++;
//            done1 = true;
//            loopcount2 = 0;
//            do {
//              printf("Type number of the outgroup:\n");
//              scanf("%ld%*[^\n]", &outgrno);
//              getchar();
//              done1 = (outgrno >= 1);
//              if (!done1) {
//                printf("\n\nERROR: Bad outgroup number: %ld", outgrno);
//                printf("  (must be greater than zero)\n\n");
//              }
//              countup(&loopcount2, 10);
//            } while (done1 != true);
//          }
//          break;
//
//        case 'R':
//          noroot = !noroot;
//          break;
//
//        case 'T':
//          initterminal(&ibmpc, &ansi);
//          break;
//
//        case '1':
//          progress = !progress;
//          break;
//        
//        case '2':
//          pairing_submenu() ;
//          break ;
//        }
//      } else
//        printf("Not a possible option!\n");
//    }
//    countup(&loopcount, 100);
//  } while (!done);
//}  /# getoptions #/
*/


int main(int argc, Char *argv[])
{  
  /* Local variables added by Dan F. */
  pattern_elm  ***pattern_array ;
  double *timesseen_changes ;
  long trees_in_1 = 0, trees_in_2 = 0 ;
  long *diff_array ;

#ifdef MAC
  argc = 1;                /* macsetup("Treedist", "");        */
  argv[0] = "Treedist";
#endif
  init(argc, argv);
  emboss_getoptions("ftreedist",argc,argv);
  /*openfile(&intree, INTREE, "input tree file", "r", argv[0], intreename);*/
  openfile(&outfile, OUTFILE, "output file", "w", argv[0], &outfilename);

  /* Initialize option-based variables, then ask for changes regarding
     their values. */
/*  getoptions();*/

  ntrees = 0.0;
  maxgrp = 10000;
  lasti  = -1;

  if ((tree_pairing == ALL_IN_1_AND_2) ||
      (tree_pairing == CORR_IN_1_AND_2))
    /* If another intree file should exist, */
    /*openfile(&intree2, INTREE2, "input tree file 2", "r", 
             argv[0], intree2name);*/

  if (tree_pairing != NO_PAIRING){
    timesseen_changes = (double *)Malloc(maxgrp * sizeof(double)) ;
  }

  /* Read the (first) tree file and put together grouping, order, and
     timesseen */
  read_groups (&pattern_array, timesseen_changes, &trees_in_1,
                 phylotrees);

  if ((tree_pairing == ADJACENT_PAIRS) ||
      (tree_pairing == ALL_IN_FIRST)) {

    trees_in_2 = 0 ;   /* Just to avoid problems. . .*/
    
    /* Here deal with the adjacent or all-in-first pairing
       difference computation */
    if (tree_pairing == ADJACENT_PAIRS)
      diff_array = (long *) Malloc (trees_in_1 * sizeof (long *)) ;
    else if (tree_pairing == ALL_IN_FIRST) 
      diff_array = (long *) Malloc ((trees_in_1 * trees_in_1)
                                   * sizeof (long *)) ;

    compute_distances (pattern_array, trees_in_1, trees_in_2, diff_array) ;
    output_distances  (trees_in_1, trees_in_2, diff_array) ;

    /* Free all the buffers needed to compute the differences. */
    free (diff_array) ;
    free (timesseen_changes) ;
    /* Patterns need to be freed in a more complex fashion. */
    /* This removed 'cause it was causing problems */
    /* free_patterns (pattern_array, trees_in_1 + trees_in_2) ;*/

  } else if ((tree_pairing == CORR_IN_1_AND_2) ||
             (tree_pairing == ALL_IN_1_AND_2)) {
    /* Here, open the other tree file, parse it, and then put
         together the difference array */
    read_second_file (pattern_array, timesseen_changes, 
                      &trees_in_1, &trees_in_2) ;

    /* Allocate a proper amount of space for the diff_array, */
    if (tree_pairing == CORR_IN_1_AND_2)
      diff_array = (long *) Malloc ((trees_in_1 + trees_in_2)
                                   * sizeof (long *)) ;
    else if (tree_pairing == ALL_IN_1_AND_2)
      diff_array = (long *) Malloc ((trees_in_1 * trees_in_2)
                                   * 2 * sizeof (long *)) ;

    compute_distances (pattern_array, trees_in_1, trees_in_2, diff_array) ;
    output_distances (trees_in_1, trees_in_2, diff_array) ;

    /* Free all the buffers needed to compute the differences. */
    free (diff_array) ;
    free (timesseen_changes) ;
    /* Patterns need to be freed in a more complex fashion. */
    /* This removed 'cause it was causing problems */
    /* free_patterns (pattern_array, trees_in_1 + trees_in_2) ; */

  } else if (tree_pairing == NO_PAIRING) {
    /* Compute the consensus tree. */
    putc('\n', outfile);
    /* consensus();         Reserved for future development */
  }

  if (progress)
    printf("\nOutput written to file \"%s\"\n\n", outfilename);
/*
  FClose(outtree);
  FClose(intree);
  FClose(outfile);
  if ((tree_pairing == ALL_IN_1_AND_2) || 
      (tree_pairing == CORR_IN_1_AND_2))
    FClose(intree2) ;
*/

  /*printf("Done.\n\n");*/

#ifdef MAC
  fixmacfile(outfilename);
  fixmacfile(outtreename);
#endif
  ajExit();
  return 0;
}  /* main */

