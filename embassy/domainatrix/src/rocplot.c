/* @source rocplot application
**
** ROCPLOT is a generic tool for interpretation and graphical display of the
** performance of predictive methods.  ROCPLOT reads one or more files of hits,
** performs Receiver Operator Characteristic (ROC) analysis on the hits, and 
** writes graphs, including ROC plots, illustrating diagnostic performance.  
** For example, a profile or other type of discriminating element can be 
** generated for a protein family and scanned against a sequence database to 
** identify putative new family members.  The file(s) of hits would contain the
** results of the profile-database search(es). 
**
**
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
** 02111-1307, USA.
**
*****************************************************************************
** 
** ROCPLOT documentation
** 
** 1.0   SUMMARY
** 2.0   INPUTS & OUTPUTS
** 2.1   ROCPLOT modes 
** 2.1.1 Single hits file mode
** 2.1.2 Multiple hits files mode
** 2.1.3 Do not combine data mode
** 2.1.4 Combine data mode
** 2.1.5 Single gold standard mode
** 2.1.6 Multiple gold standard mode
** 3.0   INPUT FILE FORMAT
** 3.1   Hits files
** 4.0   OUTPUT FILE FORMAT
** 4.1   Summary file
** 4.2   GNUPLOT files
** 5.0   DATA FILES
** 6.0   SISTER APPLICATIONS
** 7.0   NOTES
** 8.0   DESCRIPTION
** 8.1   Sensitivity and specificity
** 8.2   ROC plot
** 8.3   ROC value 
** 8.4   Classification plot
** 8.5   Processing multiple lists of hits (no combination of lists)
** 8.6   Processing multiple lists of hits (combination of lists)
** 9.0   ALGORITHM
** 9.1   Classification plot
** 9.2   ROC plot
** 9.2.1 "Single hits file" mode & 
**       "Multiple hits files" - "Do not combine data" mode
** 9.2.2 "Multiple hits files" / "Combine data" mode 
** 9.3   ROC value
** 9.3.1 "Single hits file" mode &
**       "Multiple hits files" - "Do not combine data" mode
** 9.3.2 "Multiple hits files" / "Combine data" mode
** 9.4   Identifying unique hits
** 10.0  USAGE
** 11.0  KNOWN BUGS & WARNINGS
** 12.0  DIAGNOSTIC ERROR MESSAGES
** 13.0  AUTHORS
** 14.0  REFERENCES
** 
** 
** 
** 
** 
** 1.0  SUMMARY
** ROCPLOT is a generic tool for interpretation and graphical display of the 
** performance of predictive methods.  ROCPLOT reads one or more files of hits,
** performs Receiver Operator Characteristic (ROC) analysis on the hits, and 
** writes graphs, including ROC plots, illustrating diagnostic performance.  
** For example, a profile or other type of discriminating element can be 
** generated for a protein family and scanned against a sequence database to 
** identify putative new family members.  The file(s) of hits would contain 
** the results of the profile-database search(es). 
** 
** 
** 
** 
** 
** 2.0  INPUTS & OUTPUTS
** ROCPLOT reads a directory of one or more hits files and writes a text, 
** summary file containing ROC value(s), which are a convenient numerical 
** measure of the sensitivity and specificity of a predictive method.  GNUPLOT
** files for the following graphs are also written.  
** 
** (i)   ROC plots displaying graphically the method sensitivity and specificity.
** (ii)  Classification plots, which are a useful aid in interpreting ROC plots 
**       and ROC values.  
** (iii) In some modes (see below) a bar chart of the distribution of ROC values 
**       is generated.  
** 
** 
** 2.1  ROCPLOT modes 
** ROCPLOT runs in one of two basic modes:
** 
** (i) "Single hits file" 
** (ii) "Multiple hits file".
** 
** 
** 2.1.1  Single hits file mode
** ROC analysis is performed on the single hits file.  A ROC plot containing 
** one ROC curve and a single ROC value and classification plot are generated.
** 
** 
** 2.1.2  Multiple hits files mode
** The same ROC number must be given in the hits files and each file must 
** contain at least this number of non-TRUE hits (see Section 3.1): an error 
** is generated and the program terminates otherwise.  
** In "multiple hits file mode" there are two sub-modes: 
** (i) "Do not combine data" 
** (ii) "Combine data".
** 
** 
** 2.1.3  Do not combine data mode
** ROC analysis is performed separately for each hits file.  Multiple ROC curves
** are given on the same ROC plot.  A ROC value and classification plot are 
** generated for each hits file.  A bar chart giving the distribution of ROCn 
** values is also generated.  The mean and standard deviation of ROCn values are
** written to the summary file.  
** 
** 
** 2.1.4  Combine data mode
** The hits are combined and ROC analysis is performed on the whole (see Section 
** 8.6).  A ROC plot containing one ROC curve and a single ROC value and 
** classification plot are generated.  
** In "combine data" mode there are a further two sub-modes: 
** 
** (i)  "Single gold standard" 
** (ii) "Multiple gold standard".  
** 
** These determine how the ROC number and value are calculated. 
** 
** 
** 2.1.5  Single gold standard mode
** There is a single gold standard (list of known true hits) for the different 
** searches.  The same number of known true hits must be specified in the hits 
** files: an error is generated and the program terminates otherwise.  The 
** accession number (or other code) and start and end point of each hit must 
** also be given (see Section 3.1).
** 
** 
** 2.1.6  Multiple gold standard mode
** There is a gold standard for each different search.   
** 
** The output in the different modes is summarised (Figure 1).
** 
** Figure 1  Summary of ROCPLOT output 
**                       ____________________________________________________
**                       | SINGLE HITS FILE  |      MULTIPLE HITS FILES     |
**                       |                   |                |             |
**                       |                   | Do not combine |   Combine   |      
**                       |                   | data           |   data      |
**  _____________________|___________________|________________|_____________|
**                       |                   |                |             |
**  ROC curves / value   | Single            | Multiple (1)   | Single      |
**  Bar chart            | -                 | Yes            | -           |
**  Classification plot  | Single            | Multiple       | Single      |
**  Summary file         | Yes               | Yes            | Yes         |
**  _____________________|___________________|________________|_____________|
**  (1) Multiple ROC curves are given on a single ROC plot.
** 
**  
** 
** 3.0 INPUT FILE FORMAT
** 3.1  Hits files
** A hits file (Figure 2) contains a list of classified hits that are 
** rank-ordered on the basis of score.  The first line must have '>' in the 
** first character position and a space (' ') in the second, then two token 
** - integer pairs delimited by ';'.  The integer following 'RELATED' is the 
** total number of known true hits ('relatives') and is the maximum number of 
** TRUE tokens (see below) that could ever appear in the hits file.  The 
** integer following 'ROC' is the ROC value that will be calculated.  This 
** integer also determines the limit of the x-axes of the ROC and classification
** plots (see Sections 8.2 & 8.4).  
** The file then contains a number of lines corresponding to a list of 
** classified hits.  The hits *must* be rank-ordered on the basis of score, 
** p-value, E-value etc, with the highest scoring / most significant hit given 
** in the highest rank (1); i.e. on the second line of the file.  Other hits 
** should then be given in order of decreasing score / significance.  
** 
** The first string in a hit line is the classification and must be one of the 
** following: 'TRUE', 'CROSS', 'UNCERTAIN', 'UNKNOWN' or 'FALSE'.  If ROCPLOT 
** is run in "Multiple hits files" - "Combine data" - "Single gold standard" 
** modes, each hit line must contain a second string followed by 2 integers. 
** These are required so that ROCPLOT can identify unique hits in the lists of 
** hits (see Section 9.4).  For hits to sequences, the string is the accession 
** number (or other database code) and the integers are the start and end point
** of the hit relative to the full length sequence.  For some applications the
** start and end point data are not required to define unique hits: in these
** cases the start and end values for all hits should be set to 0 and 1 
** respectively.  
** 
** Figure 2   Excerpt from a hits file
** > RELATED 140 ; ROC 50
** TRUE         DBCODE1   1   50
** TRUE         DBCODE2   12  65
** TRUE         DBCODE3   13  62
** TRUE         DBCODE4   13  63
** CROSS        DBCODE5   13  63
** TRUE         DBCODE6   14  61
** TRUE         DBCODE7   1   50
** CROSS        DBCODE8   32  84
** TRUE         DBCODE9   31  85
** FALSE        DBCODE10  13  63
** TRUE         DBCODE11  1   50 
** UNCERTAIN    DBCODE12  1   51
** UNCERTAIN    DBCODE13  1   52
** UNKNOWN      DBCODE14  37  84
** UNKNOWN      DBCODE15  35  82
** FALSE        DBCODE16  13  61
** FALSE        DBCODE17  1   51
** 
** 
** 
** 
** 
** 4.0  OUTPUT FILE FORMAT
** 4.1  Summary file
** The summary file is shown in Figure 3. The first section is comments including
** the modes ROCPLOT was run in.  The file may then contain a section where the 
** file name, number of known true hits and ROCn value are given for each hits 
** file.  In cases where data from multiple hits files were combined a single
** ROCn value will be given instead of this section.  The mean and SD of the ROCn
** values are given if calculated. 
** 
** Figure 3 Summary file 
** < This section is always given >
** ROCPLOT summary file
** mode         == 2 (Multiple hits file)
** multimode    == 2 (Combine data)  
** datamode     == 1 (Single gold standard)
** < The section is only given in "Single hits file" mode and "Multiple hits 
** files" - "Do not combine data" mode. >
** File         Known 	ROC50 (*1)
** user_data1   124       0.874
** user_data2   124       0.687
** < This line is only given in "Multiple hits files" - "Combine data" mode. >
** ROC50        == 0.666 (combined) (*1)
** < This section is only given in "Multiple hits files" - "Do not combine data" 
**   mode >
** mean ROC50   == 0.781 (*1)
** sd   ROC50   == 0.093 (*1)
**   
** 
** 4.2  GNUPLOT files
** ROCPLOT generates various gnuplot driver and data files depending upon mode.  
** For example, the user specifies the base name of the rocplot, classification, 
** bar chart and summary files to be "_rocplot", "_classplot", "_barchart" and 
** "_summary" respectively.  If ROCPLOT is run in "Multiple hits files" - 
** "Combine data" - "Single gold standard" mode the following files are 
** generated.
** 
**  _classplot_dat0   Data file for classification plot
**  _classplot_dat1   Data file for classification plot
**  _classplot_dat2   Data file for classification plot
**  _classplot_dat3   Data file for classification plot
**  _classplot_dat4   Data file for classification plot
**  _classplot        Driver file for classification plot
**  _rocplot_dat0     Data file for roc plot.
**  _rocplot          Driver file for roc plot.
**  _summary          Summary file.
** 
** If ROCPLOT is run in "Multiple hits files" - "Combine data" - "Single gold
** standard" mode the following files are generated.
**  _classplot0_dat0  Data file for first classification plot
**  _classplot0_dat1  ""  
**  _classplot0_dat2  ""   
**  _classplot0_dat3  ""  
** _classplot0_dat4  ""  
** _classplot0       Driver file for first classification plot
** _classplot1_dat0  Data file for second classification plot
** _classplot1_dat1  ""  
** _classplot1_dat3  ""  
** _classplot1_dat4  ""  
** _classplot1       Driver file for second classification plot 
**  _rocplot_dat0     Data file for roc plot.
**  _rocplot_dat1     "" 
**  _rocplot          Driver file for roc plot.
**  _summary          Summary file.
** 
** Note that there is no _classplot1_dat2 indicating that the second hits file 
** did not contain any hits for one of the data series (see Section 8.4).  
** 
** If ROCPLOT is run in "Multiple hits files" - "Do not combine data" the 
** following files are generated.
** _barchart_dat      Data file for bar chart.
** _barchart          Driver file for bar chart.
** 
** The plots are visualised by using GNUPLOT, for example by typing load 
** '_classplot1' from the GNUPLOT command line.
**  
** 
** 
** 
**  
** 5.0 DATA FILES
** ROCPLOT does not use a data file.
** 
** 
** 
** 
** 
** 6.0  SISTER APPLICATIONS
** None
** 
** 
** 
** 
** 
** 7.0  NOTES
** A future implementation will 
** Accept a feature file as input.
** Split ROCPLOT into separate programs, one for each of the major modes. 
** 
** 
** 
** 
** 
** 8.0  DESCRIPTION
** Predictive methods are a mainstay of bioinformatics.  Discrciminating 
** elements such as hidden Markov models (HMM), sparse protein signatures 
** and profiles can be generated for a set of proteins with related sequence,
** structural or functional properties.  These discriminators are 
** characteristic of the property considered and can be used diagnostically, 
** for instance, by screening a database of uncharacterised sequences.  When
** assessing predictive performance a "gold standard" of truth is required.  
** This is a set of examples that are known to be related to the discriminating
** element, and, ideally, a further set that is known to be definitely not 
** related.  For example, to assess a protein family HMM to detect true members
** of that family requires, at least, a list of the known family members.  If a
** method works well for the "gold standard" we can infer it will work well 
** generally.  Traditionally, swissprot annotation was used but this is somewhat
** unreliable because the annotation is derived from sequence comparison as well
** as experimental data.  Increasingly, use is made of databases such as SCOP, 
** in which sequence, structural and functional relationships are classified. 
** As an aside, such databases are biased for domains, which are the unit of 
** classification, so it's important to check that a method tested on e.g. SCOP
** will also work on full-length sequences. 
** 
** 
** 8.1  Sensitivity and specificity
** Most predictive methods can be placed into two broad groupings: (i) Methods
** that produce a definite yes/no answer.  There is a single list of "hits" and
** things not in this list are "misses".  (ii) Methods that produce a list of 
** hits that is rank-ordered on the basis of the score or p-value of the 
** discrimintor-sequence match.  The hit with the highest / most significant
** score will be in highest rank, i.e. rank 1.  Usually, a cutoff value of rank,
** score or p-value is applied; "hits" occur at and above the cuttoff and 
** "misses" occur below it.  
** 
** Armed with the notion of a "gold standard" and "hits" and "misses", all hits 
** retrieved by a search can be organised as in Figure 4.
** 
** Figure 4  Classification of hits
** 
**                  From the gold standard
**                 |          |          |
**                 | Related  | Unrelated| 
**          _______|__________|__________|_______
**                 |          |          |
** S r      (+ve)  |    TP    |    FP    |   P   (=TP+FP) 
** e e      hits   |          |          |
** a s      _______|_ ________|__________|_______
** r u             |          |          | 
** c l      (-ve)  |    FN    |    TN    |   N   (=FN+TN)
** h t      misses |          |          |
**          _______|__________|__________|_______
**                 |          |          |
**                 |    R     |    U     |
**                 | (=TP+FN) | (=FP+TN) | 
** 
** Where TP are true positives, FN are false negatives, R (TP+FN) is the total 
** number of known true hits (relatives).  FP are false positives, TN are true 
** negatives and U (FP+TN) is the total number of known non-relatives.  The 
** number of positives is given by P (TP+FP) and the number of misses by N 
** (FN+TN).  
** 
** The two basic types of error are where (i) a relationship is missed ("false 
** negative" or "ommission error") and (ii) a relationship is inferred which
** does not truly exist ("false positive" or "commission error").  The cost of
** these two errors are not usually equal: it depends on the specific 
** application but usually false positives are worse than false negatives.  A 
** crude way to measure the performance is to quote ommission and commission 
** error rates at a fixed cutoff value to the list of hits.  These rates are 
** usually given as sensitivity (SENS or "coverage") and specificity (SPEC or 
** "accuracy") of the method and are defined as follows.
** 
** SENS = TP / R
** 
**  SPEC = TP / P
**  
**  Another measure of specificity (JMB 282, 903-918) defines SENS = TN / U.  The
**  measure used depends on the specific application, but TP/P is often most 
**  suitable as it reflects the hits that are actually retrieved by the search.  
**  TP / P is used in ROCPLOT (see Section 9.2).
**  
**  The most basic graphical representation of sensitivity and specificity is 
**  the "coverage versus error plot" or "sensitivity curve" (Figure 5).  This 
**  plots the number of true positives detected (y-axis) versus the number of 
**  false positives detected (x-axis), at different cutoff values in the list of
**  hits.  The word 'detected' here refers to a hit that is above the cutoff, 
**  i.e. is of a higher or more significant score.  
**  
**  Figure 5  A "coverage versus error" plot
**   
**              |
**              |                           * 
**   No. true   |                    * 
**   positives  |              *
**   detected   |          *
**              |       *
**              |     *
**              |   *
**              |  *
**              | *
**              |*
**              |______________________________
**                                   No. false
**                                   positives 
**                                   detected
**  
**  8.2  ROC plot
**  A superior measure of diagnostic performance is to use Receiver Operator 
**  Characteristic (ROC) curves to display graphically the sensitivity and 
**  specificity of a method.  ROC analysis is a powerful aid to interpretation 
**  and has been widely used, for instance to evaluate clinical diagnostic tests
**  and in the bioinformatics literature.  A ROC curve (Figure 6) is a 
**  generalised version of the "coverage versus error" plot.  It plots SENS 
**  (TP/R) on the y-axis, i.e. the fraction of known true hits detected or the 
**  "rate of true positives", versus 1-SPEC (1 - TP/P) on the x-axis, i.e. 1 
**  minus the fraction of detected hits that are true positives or the "rate of 
**  false positives".  ROC curves are generated by plotting SENS versus (1-SPEC) 
**  for all possible cutoff values in a rank-ordered list of hits.
**  
**  Figure 6  A ROC curve
**  
**              |
**              |                           * 
**      SENS    |                    * 
**              |              *
**    "rate of  |          *
**      true    |       *
**   positives" |     *
**              |   *
**              |  *
**              | *
**              |*
**              |______________________________
**   	                     1 - SPEC
**                       "rate of false positives"
**  
**  A ROC curve shows the trade-off between sensitivitiy and specificity: as 
**  sensitivitiy increases, specificity decreases.  The ideal ROC curve lies on 
**  the y-axis, i.e. there is perfect discrimination between related and 
**  unrelated proteins.  A ROC curve for a good prediction should always be to 
**  the left of the diagonal.  ROC curves are very useful for comparing two 
**  diffent methods (e.g. homology search methods) because if one method produces
**  a curve to the left of another then that method is superior, regardless of 
**  the cost of ommission and commission errors.  
**  
**  
**  8.3  ROC value
**  The area under the ROC curve (AUROC) gives the probability of a correct 
**  classification and is a very convenient numerical measure of the sensitivity 
**  and specificity of a method.  Areas are relative to a ROC space which is a 
**  unit square in which both SENS and SPEC are plotted from 0 to 1.  An area of 
**  0.9 for example means that a sequence from the group of known relatives has 
**  a probability of 0.9 of scoring higher than a sequence from the group of 
**  known non-relatives.  The best possible prediction has an AUROC of 1.  
**  
**  In most cases however there are vastly more true negatives than true 
**  positives.  This is the case when a search is made with a sequence against a
**  large sequence database.  As most sequence are quite discriminating for 
**  their family, the AUROC for a ROC curve plotted for the results of the entire
**  database search will be very close to 1.  The AUROC value is still useful but 
**  it has to be calculated to 5 or 6 decimal places.  Furthermore all the curves
**  would look identical which makes comparing two methods by eye impossible, all 
**  the database scores would have to be written to disk, and the value does not 
**  really represent the way in which the average biologist, who is unprepared to 
**  inspect many thousands of false positives, would use the method.  For these 
**  reasons, ROC curves are usually truncated to the first 50 or 100 false hits, 
**  and the so-called ROC50 or ROC100 value calculated.  ROCn values are quicker 
**  and more convenient to calculate, can be expressed by fewer decimal places 
**  and reflect the way in which the average biologist will use the method.
**  
**  
**  8.4  Classification plot
**  In many cases not every hit returned by a search can be clearly classified as
**  true or false or it might otherwise be desirable to manage hits with an 
**  intermediate classification.  This might be the case where the gold standard 
**  is based on a hierarchic structure (e.g. SCOP).  Consider conceptual "cross",
**  "uncertain" and "unknown" hits.  "Cross hits" have a definite relation to the
**  query but not at such a fine level as a "true" hit.  An example is a query 
**  matching a sequence belonging to a different family but the same superfamily
**  as the query.  An "uncertain hit" might show some but not clear evidence of a
**  relation.  An example would be a query matching a sequence belonging to a 
**  different family and superfamily, but the same fold as the query.  For other 
**  hits, nothing may be known either way and these would be classified as 
**  "unknown".  ROCPLOT supports "cross", "uncertain" and "unknown" hits and
**  provides a graphical representation of the classifications of hits by 
**  generating a "classification plot".  
**   
**  A classification plot (Figure 7) shows the proportion of hits detected that 
**  are 'true', 'cross', 'uncertain', 'unknown' and 'false'.  The y-axis is the 
**  proportion of the hits detected that are of a certain type, the x-axis is 
**  the proportion of the total number of hits detected.  A separate curve is 
**  given for hits of each type.  In ROCPLOT a classification plot is generated
**  by plotting these proportions at each rank in the list of hits up to the
**  point where a user-defined number of 'false' hits are detected.  As ROC plots
**  and values (see below) do not consider 'cross', 'uncertain' and 'unknown'
**  hits, the classification plot is a useful aid in interpreting the ROC plot 
**  and value for some applications.  
**  
**  Figure 7  A classification plot
**  
**   Proportion of 1.0|
**   hits detected    |                             
**   that are of a    |                      
**   certain type     |                              
**                    |                       *     *  TRUE
**                    |              *        .     .  CROSS
**                    |        *      .         
**                    |    *   .
**                    |  *  .                    x  x  FALSE
**                    | *.              x
**                    |*.          x
**                    |______________________________
**                   0                              1.0
**                                   Proportion of total
**                                   number of hits detected.
**  
**  Hits of classification 'uncertain' and 'unknown' are not shown for clarity.
**  
**  
**  8.5  Processing multiple lists of hits (no combination of lists)
**  ROC analysis is a powerful way to compare predictive methods side by side.  
**  A ROC value can be generated for each method and a curve plotted on the same 
**  ROC plot.  For some applications a summary of a set of ROC values is required. 
**  Depending upon mode (see Section 2.1), ROCPLOT will generate the mean, 
**  standard deviation (SD) and a bar chart (Figure 8) of the distribution of 
**  ROCn values.  In constructing the bar chart, the range of possible ROC values 
**  from 0 to 1 is divided into 20 bins of size 0.05 and the frequency of 
**  occurence of ROC values in each bin range is calculated. 
**  
**  Figure 8  Bar chart for distribution of ROCn values
**  
**  Frequency   |
**              |                        ___  
**              |                       |   |
**              |                    ___|   |  
**              |            ___    |   |   |
**              |           |   |   |   |   |
**              |    ___    |   |   |   |   |
**              |   |   |___|   |   |   |   |
**              |   |   |   |   |___|   |   |
**              |___|   |   |   |   |   |   |
**              |   |   |   |   |   |   |   |
**              |___|___|___|___|___|___|___|__
**                             
**                           Bins for different
**                           ranges of value of 
**                           ROCn value
**  
**  
**  
**  8.6  Processing multiple lists of hits (combination of lists)
**  In some cases it is desirable to combine data from multiple lists of hits and
**  derive a single ROC curve and value.  Such cases fall into one of two broad 
**  groups: (i) There is a single set of known true relatives for the different 
**  searches, for example, when assessing the performance of multiple
**  discriminating elements for a single family.  In these cases the typical
**  ROC50 or ROC100 value is generated.  (ii) There is a different set of known 
**  true relatives for each different search, for example, when assessing the
**  performance of a single discriminating element over mutliple families.  A
**  much higher ROC number is used.  For exmaple, ROC500 is reasonable if 10 
**  lists of hits are combined.  
**  
**  Lists of hits arising from different searches can be combined and reordered
**  if they are scored on the same scoring scale or have been assigned a p-value.
**  In principle one way to use ROCPLOT is to do the combination and reordering 
**  yourself and provide ROCPLOT with a single list of hits as input.  This, 
**  however, is not possible if the lists of hits use different scoring schemes 
**  and a p-value is not available.  Furthermore, in many cases the relative 
**  positioning of hits in the list is more important than the absolute score. 
**  If two lists of hits (A and B) whose hits lie on different regions of the 
**  same scoring scale are merged and reordered, true hits, which rank very 
**  highly in their own list (A), might be relegated way down the merged list, 
**  appearing after false hits from list B.  Therefore the high-ranking and 
**  potentially interesting hits in list A might, depending on the ROCn value 
**  calculated, not be considered in the combination ROC value.  To overcome
**  this, the lists of hits can be processed in parallel: to consider all the 
**  hits at rank 1 in the different lists first, then all the hits at rank 1 
**  and 2, and so on. This is the approach taken in ROCPLOT (see Section 9).
**  
**  
**   
**  
**  
**  9.0 ALGORITHM
**  9.1  Classification plot
**  The proportion of the total hits detected that are of a certain type (TRUE, 
**  CROSS, UNCERTAIN, UNKNOWN and FALSE) is calculated at each rank position in
**  the list of hits, from the first rank (hit) up to and including the hit 
**  corresponding to the nth false positive.  n is the ROC number given in the 
**  hits file.  For example, if i is the current rank number,
**  Proportion(TRUE) = (Number of TRUE tokens from ranks 1 to i / i).
**  
**  
**  9.2  ROC plot
**  9.2.1 "Single hits file" mode and "Multiple hits files" - "Do not 
**        combine data" mode
**  SENS and SPEC are calculated at each rank in the list of hits from the first
**  rank up to and including the hit that is the nth false positive.  n is the 
**  ROC number given in the hits file.  SENS and SPEC are calculated as follows.
**  
**  SENS(i) = TP / R
**  SPEC(i) = TP / i
**  
**  Where i is the current rank number, TP is the number of TRUE tokens occuring
**  from rank 1 to i.  R is the total number of known true hits (relatives)
**  specified after the 'RELATED' token in the hits file(s) (see Section 3.1).
**  
**  Hits classified as CROSS, UNCERTAIN and UNKNOWN are all treated as FALSE.  
**  This means that the ROC curve is really giving "rate of noise" on the x-axis
**  rather than the "rate of false positives".  The "noise" might actually 
**  include genuinely interesting hits and for this reason, the ROC plot must be
**  interpreted in the light of the classification plot if CROSS, UNCERTAIN and
**  UNKNOWN classifications are used.  If the hits file contains fewer than n 
**  hits that are non-TRUE, an error is generated and ROCPLOT terminates. 
**  
**  9.2.2  "Multiple hits files" / "Combine data" mode
**  SENS and SPEC are calculated at different ranks as before but this time the 
**  lists are processed in parallel.  SENS and SPEC are calculated from each list
**  in turn at each rank from the first rank up to and including the rank at 
**  which n false positive (from the different lists) are detected.  If there are 
**  5 hits files for example, a maximum of 5 hits are considered to yield up to 5 
**  SENS and 5 SPEC values at each rank.  In "Single gold standard" mode, n is 
**  the ROC number specified after the 'ROC' token in the hits files.  In 
**  "Multiple gold standard" mode, n = (ROC number from hits files * number of 
**  input files).  SENS and SPEC are calculated as follows.
**  
**  SENS(i, j) = TP / R
**  SPEC(i, j) = TP / nhits 
**  
**  Where i is the current rank number and j is the number of the list of the hit
**  being considered.  TP is the number of true positives.  TP = (Number of TRUE 
**  tokens in ranks 1 to i-1 in all lists + number of TRUE tokens in rank i in 
**  lists 1 to j).  Note that in "Single gold standard" mode only those TRUE 
**  tokens corresponding to unique hits (see below) are counted.  R is the number 
**  of known 'true' hits (relatives).  In "Single gold standard" mode, R equals
**  the value after the 'RELATED' token in the hits files.  In "Multiple gold 
**  standard" mode, R equals the sum of the values given after the 'RELATED' 
**  tokens.  nhits is the number of hits considered so far.  If the hits files 
**  contain equal numbers of hits, nhits = (i-1)*N + j, where N is the total
**  number of hits files.
**  
**  9.3  ROC value
**  9.3.1  "Single hits file" mode and "Multiple hits files" - "Do not combine 
**  data" mode
**  The ROCn value is defined as:
**  
**  ROCn = 1/nR * T  (T is Ti summed for 1<=i<=n)
**  
**  n is the ROC number from the hits file.  R is the total number of known true 
**  hits given in the hits file after the 'RELATED' token.  Ti is the number of 
**  TRUE tokens occuring from rank 1 up to the rank for the ith non-TRUE hit. 
**  In other words, Ti is the number of 'true' hits detected above the ith 'false' 
**  hit.
**  
**  9.3.2  "Multiple hits files" / "Combine data" mode
**  Again, the ROCn value is defined as :
**  
**  ROCn = 1/nR * T  (T is Ti summed for 1<=i<=n)
**  
**  n is the ROC number used.  In "Single gold standard" mode, n is the ROC 
**  number given in the hits files.  In "Multiple gold standard" mode, n = (ROC
**  number given in hits files * number of input files).  R is the number of 
**  known true hits (relatives).  In "Single gold standard" mode, R equals the
**  value given after the 'RELATED' token in the hits files.   In "Multiple gold
**  standard" mode, R equals the sum of the values given after the 'RELATED' 
**  tokens.  
**  Ti is the number of TRUE tokens found up to the ith token that is not 'TRUE'.  
**  If k and j are the rank and number of list respectively at which the nth 
**  non-TRUE hit is detected, Ti = (number of TRUE tokens in ranks 1 to k-1 in 
**  all lists + number of TRUEn tokens in rank k in lists 1 to j).  Again, Ti 
**  is the number of 'true' hits detected above the ith 'false' hit.
**  
**  9.4  Identifying unique hits
**  In "Multiple hits files" - "Combine data" - "Single gold standard" mode, 
**  ROCPLOT only counts unique hits when calculating SENS and SPEC.  Two hits 
**  are 'unique' if they have (i) different accesssion numbers or (ii) the same 
**  accession numbers but which do not overlap by any more than a user-defined 
**  number of residues.  The overlap is determined from the start and end points
**  of the hit.  For example two hits, with the same accession numbers and start 
**  and end points of 1-100 and 91 - 190 respectively, are not unique if the 
**  overlap threshold is 10 or less.  Duplicate hits (the second and subsequent
**  occurences of non-unique ones) in the hits files are discarded - they are 
**  NOT considered when calculating the ROC curve and value.
**   
**  The different hits files might contain different numbers of hits and 
**  therefore at higher ranks, SENS and SPEC might only consider hits from a 
**  subset of all the hits files, up to the last rank for which it is likely 
**  just a single hit will be considered.  This is illustrated in Figure 9, 
**  which shows the lists of hits for 3 hits files, a ROC number of 3 is given
**  for each one.  At ranks 1 up to 6, SENS and SPEC would consider hits from 
**  all 3 input files.  At rank 7 however, only hits from files 2 and 3 would 
**  be considered as 3 false hits have been detected in file 1 and no more hits 
**  are listed. Similarly at ranks 10 and 11 only hits from file 3 will be 
**  considered. 
**  
**  Figure 9   Calculation of ROC value for multiple hits files
**  Rank  File1  File2  File3
**        ROC3   ROC3   ROC3
**  1     TRUE   TRUE   TRUE  
**  2     TRUE   TRUE   TRUE  
**  3     TRUE   TRUE   TRUE
**  4     FALSE  TRUE   TRUE 
**  5     FALSE  TRUE   TRUE 
**  6     FALSE  FALSE  TRUE
**  7            FALSE  FALSE
**  8            TRUE   FALSE
**  9            FALSE  TRUE
**  10                  TRUE 
**  11                  FALSE
**   
**  
**  
**  10.0  USAGE
**  An example of interactive use of ROCPLOT is shown below.
**  >: rocplot
**  Provides interpretation and graphical display of the performance of
**  discriminating elements (e.g. profiles for protein families). ROCPLOT 
**  reads file(s) of hits from discriminator-database search(es), performs
**  ROC analysis on the hits, and writes graphs illustrating the diagnostic
**  performance of the discriminating elements.
**  Directory of hits files []: rocplot
**  Available modes
**           1 : single hits file mode
**           2 : multiple hits file mode
**  Select mode of operation [1]: 2
**  Available modes
**           1 : Do not combine data (multiple ROC curves in single ROC plot
**    multiple classification plots) :
**           2 : Combine data (single ROC curve
**    single classification plot) :
**  Select mode of operation [1]: 2
**  Available modes
**           1 : single gold standard
**           2 : Multiple lists of known true relatives
**  Select mode of operation [1]: 1
**  Overlap threshold for hits [10]:
**  Name of ROC plot file (output) []: _rocplot
**  Name of summary file (output) []: _summary
**  Base name of classification plot file(s) (output) []: _classplot
**  Processing data1.rocplot
**  Processing data2.rocplot
**  Please wait ... done!
**  >:
**   
**  The following command line would achieve the same result.
**  rocplot rocplot _rocplot -mode 2 -multimode 2 -datamode 1 
**  -thresh 10 -outfdata _summary -classbasename _classplot
**   
**  
**  
**  11.0 KNOWN BUGS & WARNINGS
**  GNUPLOT must be started in the same directory as the gnuplot data files.
**  
**  If you run ROCPLOT on many input files without specifying combination of 
**  data the ROC plot generated can get very cluttered.  This is not a flaw of
**  ROCPLOT, but an inevitable consequence of trying to draw too many things 
**  on the same plot.  The recomended maximum is 5 to 10 input files.
**  
**  The hits in the hits files *must* be rank-ordered on the basis of score, 
**  p-value, E-value etc, with the highest scoring / most significant hit given
**  in the highest rank (1); i.e. on the second line of the file.  Other hits 
**  should then be given in order of decreasing score / significance.
**  
**  
**  
**  12.0 DIAGNOSTIC ERROR MESSAGES
**  For purposes of generating the ROC plot and ROC curve, hits classified as 
**  CROSS, UNCERTAIN and UNKNOWN are all treated as FALSE.  An error is 
**  generated and ROCPLOT terminates in the following cases.
**  
**  If the hits file contains more TRUE hits than the number after the 
**  'RELATED' token.
**  
**  In "Multiple hits files" mode, if different values are given after the 
**  'ROC' token in the files.
**  
**  The number of non-TRUE hits is less than the value after the 'ROC' token.
**  
**  In "Single gold standard" mode, if different values are given after the 
**  'RELATED' token in the files.
**  
**   
**   
**  13.0 AUTHORS
**  Jon Ison    (jison@hgmp.mrc.ac.uk)
**  
**  
**   
**  14.0 REFERENCES
**  Please cite the authors and EMBOSS.
**  
**  
****************************************************************************/



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


      



/* Object definitions */

/* @data AjPHitdata *********************************************************
**
** Class    Classification of hit            
** Acc      Accession number of hit sequence (if available)
** Start    Start of hit in sequence (if available)        
** End      End of hit in sequence (if available)          
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
** Class    Classification of hit            
** Acc      Accession number of hit sequence (if available)
** Start    Start of hit in sequence (if available)        
** End      End of hit in sequence (if available)          
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




/* Function prototypes */
static AjBool rocplot_write_summary(AjPFile outf, ajint mode, 
				    ajint multimode, ajint datamode, 
				    ajint numfiles, AjPStr *hitsnames,
				    ajint roc, 
				    AjPFloat rocn, AjPInt nrelatives);
static AjBool   rocplot_write_barchart(AjPStr fname, AjPStr title, 
				       AjPStr xlabel, AjPStr ylabel, 
				       ajint nbins, float binstart, 
				       float binsize, AjPFloat rocn, 
				       ajint numfiles);
static AjBool   rocplot_write_classplot(AjPStr fname, AjPStr title, 
					AjPStr xlabel, AjPStr ylabel, 
					ajint nseries, ajint filen, 
					AjPStr *legend, AjPFloat2d classx, 
					AjPFloat3d classy, ajint npoints);
static AjBool   rocplot_write_rocplot(AjPStr fname, AjPStr title, 
				      AjPStr xlabel, AjPStr ylabel, 
				      ajint nseries, AjPStr *legend, 
				      AjPFloat2d rocx, AjPFloat2d rocy, 
				      AjPFloat rocn, AjPInt hitcnt);
static AjBool rocplot_count_class(AjPHitdata tmphit, ajint hitcnt, 
				  ajint plotn, AjBool reset, 
				  AjPFloat2d *classx, AjPFloat3d *classy, 
				  ajint *ntrue, ajint *nottrue);
static AjBool rocplot_calcdata(int mode, int multimode, int datamode, 
			       AjPList *hitslists, ajint numfiles, 
			       ajint thresh, ajint roc, 
			       AjPInt nrelatives, AjPFloat *rocn, 
			       AjPFloat2d *rocx, AjPFloat2d *rocy, 
			       AjPFloat2d *classx, AjPFloat3d *classy, 
			       AjPInt *hitcnt);
static AjBool rocplot_overlap(AjPHitdata h1, AjPHitdata h2, ajint thresh);
static AjBool rocplot_hit_is_unique(AjPHitdata  hit, AjPList mrglist, 
				    ajint thresh);
void rocplot_HitdataDel(AjPHitdata *thys);
AjPHitdata rocplot_HitdataNew(void);
AjBool rocplot_read_hits_files(int mode, int multimode, int datamode, 
			       AjPList hitsfiles, AjPStr **hitsnames, 
			       AjPList *hitslists, AjPInt *nrelatives,
			       ajint *roc);
void rocplot_XYdataDel(AjPXYdata *thys);
AjPXYdata rocplot_XYdataNew(void);
ajint rocplot_compX(const void *ptr1, const void *ptr2);




    AjPFile    errf      = NULL;




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
/*  AjPFile   rocbasename    = NULL; */ /* ROC plot file from ACD              */
    AjPStr    rocbasename    = NULL;    /* ROC plot file from ACD              */
    AjPFile   outfdata   = NULL;    /* Summary file from ACD.              */
/*  AjPFile   barbasename    = NULL; */ /* Bar chart for ROC value distribution  
                                       from ACD.                           */
    AjPStr    barbasename    = NULL;    /* Bar chart for ROC value distribution  
                                       from ACD.                           */
    AjPStr    classbasename   = NULL;    /* Base name of file(s) containing 
				       classification plot(s) from ACD.    */

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
/*  AjPFile     outfclass  = NULL;*//* Classification plot                 */
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

    /* DIAGNOSTICS */
    AjIList    iter      = NULL;
    


    /********************/
    /* PROCESS ACD DATA */
    /********************/    
    ajNamInit("emboss");
    ajAcdInitP("rocplot", argc, argv, "DOMAINATRIX");

    hitsfiles     = ajAcdGetDirlist("hitsfiles");
    mode          = ajAcdGetList("mode");
    if(ajStrChar(*mode,0)=='2')
	multimode = ajAcdGetList("multimode");
    if((ajStrChar(*mode,0)=='2') && (ajStrChar(*multimode,0)=='2'))
	datamode  = ajAcdGetList("datamode");
    if((ajStrChar(*mode,0)=='2') && (ajStrChar(*multimode,0)=='2') && 
       (ajStrChar(*datamode,0)=='1'))
	thresh    = ajAcdGetInt("thresh");
/*  rocbasename       = ajAcdGetOutfile("rocbasename");  */
    rocbasename       = ajAcdGetString("rocbasename");
    outfdata      = ajAcdGetOutfile("outfdata");
    /*
    if((ajStrChar(*mode,0)=='2') && (ajStrChar(*multimode,0)=='1'))
	barbasename   = ajAcdGetOutfile("barbasename"); */
    if((ajStrChar(*mode,0)=='2') && (ajStrChar(*multimode,0)=='1'))
	barbasename   = ajAcdGetString("barbasename");
    classbasename      = ajAcdGetString("classbasename");

    /* DIAGNOSTICS */
    errf          = ajAcdGetOutfile("errf");    




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
/*    if(!(errf=ajFileNewOutC("rocplot.errors")))
	ajFatal("Could not open error file"); */
    ajFmtPrintF(errf, "MODE INFO\n");
    ajFmtPrintF(errf, "modei: %d\nmultimodei: %d\ndatamodei: %d\n", 
		modei, multimodei, datamodei);
    ajFmtPrintF(errf, "\n\n\n");    
    

    numfiles  = ajListLength(hitsfiles);
    printf("numfiles: %d\n", numfiles);
    

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
    ajFmtPrintF(errf, "LISTS OF HITS IN EACH FILE\n");
    for(x=0; x<numfiles; x++)    
    {
	ajFmtPrintF(errf, ">>>>>  FILE %d >>>>>\n", x);
	iter = ajListIter(hitslists[x]);
	while((tmphit = (AjPHitdata)ajListIterNext(iter)))
	    ajFmtPrintF(errf, "%S\n", tmphit->Class);
	ajListIterFree(iter);
	ajFmtPrintF(errf, "\n\n\n");    	
    }	
    ajFmtPrintF(errf, "NUMBER OF RELATIVES\n");
    for(x=0; x<numfiles; x++)    
	ajFmtPrintF(errf, "nrelatives[%d]: %d\n", x, 
		    ajIntGet(nrelatives, x));
    ajFmtPrintF(errf, "\n\n\n");    	    
    
    /*********************************************************************/
    /* CALCULATE x/y DATA FOR ROC AND CLASSIFICATION PLOTS AND ROC VALUE */
    /*********************************************************************/
    /* roc, rocx, rocy, classx and classy are written */
    if(!rocplot_calcdata(modei, multimodei, datamodei, hitslists, 
			 numfiles, thresh, roc, 
			 nrelatives, &rocn, &rocx, &rocy, &classx, &classy, 
			 &hitcnt))
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
/*	outfclass = ajFileNewOut(classbasename); */
	ajFmtPrintS(&classtitle, "Classification plot for %S", hitsnames[0]);	
/*	rocplot_write_classplot(outfclass, classtitle, classxlabel, 
				classylabel, NUMCLASSES, 0, classlegend, 
				classx, classy, hitcnt); */
	if(!rocplot_write_classplot(classbasename, classtitle, classxlabel, 
				    classylabel, NUMCLASSES, 0, classlegend, 
				    classx, classy, ajIntGet(hitcnt, 0)))
	    ajFatal("rocplot_write_classplot failed");
/*	ajFileClose(&outfclass); */
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
/*		outfclass = ajFileNewOut(fullname); */
/*		rocplot_write_classplot(outfclass, classtitle, classxlabel, 
					classylabel, NUMCLASSES, x, 
					classlegend, classx, classy, 
					hitcnt); */
		if(!rocplot_write_classplot(fullname, classtitle, 
					    classxlabel, 
					    classylabel, NUMCLASSES, x, 
					    classlegend, classx, classy, 
					    ajIntGet(hitcnt, x)))
		    ajFatal("rocplot_write_classplot failed");
/*		ajFileClose(&outfclass); */
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
	    /* XXX Could possibly set more sensible values here - wait for 
	     user feedback. */
	    if(!rocplot_write_barchart(barbasename, bartitle, barxlabel, 
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
	    
/*	    outfclass = ajFileNewOut(classbasename); */
/*	    rocplot_write_classplot(outfclass, classtitle, classxlabel, 
				    classylabel, NUMCLASSES, 0, classlegend,
				    classx, classy, hitcnt); */

/*	    ajFileClose(&outfclass); */

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
	    if(!rocplot_write_classplot(classbasename, classtitle, classxlabel, 
					classylabel, NUMCLASSES, 0, 
					classlegend,
					classx, classy, ajIntGet(hitcnt, 0)))
		ajFatal("rocplot_write_classplot failed");

	}
    } 	
    /***********************/
    /* WRITE ROC PLOT FILE */
    /***********************/

    if(!rocplot_write_rocplot(rocbasename, roctitle, rocxlabel, rocylabel, 
			      nrocseries, roclegend, rocx, rocy, rocn, 
			      hitcnt))
	ajFatal("rocplot_write_rocplot failed");
			    
    
    /**********************/
    /* WRITE SUMMARY FILE */
    /* ********************/
    if(!rocplot_write_summary(outfdata, modei, 
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
/*  ajFileClose(&rocbasename); */
    ajStrDel(&rocbasename);
    ajFileClose(&outfdata);
    if((modei==2) && (multimodei==1))
	/* ajFileClose(&barbasename); */
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
AjBool rocplot_read_hits_files(int mode, int multimode, int datamode, 
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
    iter = ajListIter(hitsfiles);
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
		if(ajFmtScanS(hitsline, "%S", &class) != 1)
		    ajFatal("Incorrect format of hit line in %S",hitsfile);
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
    ajListIterFree(iter);

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
AjPHitdata rocplot_HitdataNew(void)
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
void rocplot_HitdataDel(AjPHitdata *thys)
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
**
** @param [r] hit     [AjPHitdata] Hit to check for uniqueness
** @param [r] mrglist [AjPList]    List of hits
** @param [r] thresh  [ajint]      Overlap threshold
** 
** @return [AjBool] True if Hit did not occur in list.
** @@
**
****************************************************************************/
static AjBool rocplot_hit_is_unique(AjPHitdata  hit, AjPList mrglist, 
				    ajint thresh)
{
    AjIList     iter      = NULL;    
    AjPHitdata  tmphit    = NULL; 

    iter = ajListIter(mrglist);
    while((tmphit = (AjPHitdata)ajListIterNext(iter)))
    {
	if(rocplot_overlap(tmphit, hit, thresh))
	{
	    ajListIterFree(iter);
	    return ajFalse;
	}
    }
    ajListIterFree(iter);
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
			       AjPInt *hitcnt)
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
	iters[x]=ajListIter(hitslists[x]);
    
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
			(rocplot_hit_is_unique(tmphit, mrglist, thresh))) ||
		       (datamode==2))
		    {
			ajIntInc(hitcnt, 0);

			/* Calculate data for classification plot */
			if(!rocplot_count_class(tmphit, ajIntGet(*hitcnt, 0),
						0, ajFalse, 
						classx, classy,  &ntrue, 
						&nottrue))
			    ajFatal("rocplot_count_class failed");
			
			/* Calculate data for roc plot.  Any hit that isn't a 'TRUE' 
			   is classed as a 'FALSE' when calculating the ROC curve and
			   value. */
			if(!ajStrMatchC(tmphit->Class, "TRUE"))
			    ntrue_sum += (float) ntrue;
			ajFmtPrintF(errf, "For hit %5d ntrue=%5d ntrue_sum=%5.3f\n", ajIntGet(*hitcnt, x),ntrue, ntrue_sum);

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
			
/*			ajFloat2dPut(rocx, 0, *hitcnt-1, (1 - spec));
			ajFloat2dPut(rocy, 0, *hitcnt-1, sens);		 */

			if(nottrue==maxfalse)
			{
			    /* Sort list of data points by X value and 
			       populate arrays */
/*			    ajListSort(xylist, rocplot_compX);   */
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
				     (float)ajIntGet(nrelatives, x)))
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
		ajFmtPrintF(errf, "For hit %5d ntrue=%5d ntrue_sum=%5.3f\n", ajIntGet(*hitcnt, x),ntrue, ntrue_sum);
		

		/* DIAGNOSTICS */
		ajFmtPrintF(errf, "[%d]   sens = %d / %d      spec = %d / %d", 
			    ajIntGet(*hitcnt, x)-1, 
			    ntrue, 
			    ajIntGet(nrelatives, x), 
			    ntrue, 
			    ajIntGet(*hitcnt, x));
		

		sens = (float) ((float)ntrue / 
				(float)ajIntGet(nrelatives, x));
		spec = (float) ((float)ntrue / 
				(float)ajIntGet(*hitcnt, x));
				

		ajFmtPrintF(errf, "   (sens = %f  spec = %f\n",sens, spec); 

		/* Create data point and populate list */
		xyptr = rocplot_XYdataNew();
		xyptr->X = (1 - spec);
		xyptr->Y = sens;

		ajListPushApp(xylist, (void *)xyptr);
			
/*		ajFloat2dPut(rocx, x, *hitcnt-1, (1 - spec));
		ajFloat2dPut(rocy, x, *hitcnt-1, sens);		 */
		
		/* Housekeeping */
		reset = ajFalse;

		if(nottrue==maxfalse)
		{
		    /* Sort list of data points by X value and populate 
		       arrays */
/*		    ajListSort(xylist, rocplot_compX);    */
		    y=0;
		    while(ajListPop(xylist, (void **)&xyptr))
		    {
			ajFloat2dPut(rocx, x, y, xyptr->X);
			ajFloat2dPut(rocy, x, y, xyptr->Y);		 
			
			/* DIAGNOSTICS */
			ajFmtPrintF(errf, "sens[%d]: %f   1-spec[%d]: %f\n", 
				    y, xyptr->Y, y, xyptr->X);	
			

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
	    ajFmtPrintF(errf, "roc: %d\n"
		       "nrelativess: %d\n"
		       "ntrue_sum: %f\n",
		       roc, ajIntGet(nrelatives, x), ntrue_sum);


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
	ajListIterFree(iters[x]);
    AJFREE(iters);

    return ajTrue;
}





/* @funcstatic rocplot_count_class ******************************************
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
****************************************************************************/
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





/* @funcstatic rocplot_write_rocplot ****************************************
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
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
****************************************************************************/
static AjBool   rocplot_write_rocplot(AjPStr fname, AjPStr title, 
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
	ajFmtPrintS(&outfname, "%S_dat%d", fname, x);
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
    if(!(outf = ajFileNewOut(fname)))
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





/* @funcstatic rocplot_write_classplot **************************************
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
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
****************************************************************************/
static AjBool   rocplot_write_classplot(AjPStr fname, AjPStr title, 
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
	    ajFmtPrintS(&outfname, "%S_dat%d", fname, x);
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
    if(!(outf = ajFileNewOut(fname)))
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





/* @funcstatic rocplot_write_barchart ***************************************
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
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
****************************************************************************/
static AjBool   rocplot_write_barchart(AjPStr fname, AjPStr title, 
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
	    ajFmtPrint("x: %d   numfiles: %d  nbins: %d   binsize: %f   min: %f   max: %f   roc: %f\n", 
		       x, numfiles, nbins, binsize, min, max, roc);
	    
	    ajFatal("Could not put data in bin");
	}
    }

    
    /* Create gnuplot data files */
    ajFmtPrintS(&outfname, "%S_dat", fname);
    if(!(outf = ajFileNewOut(outfname)))
	ajFatal("Could not open gnuplot data file (%S) for writing in "
		"rocplot_write_barchart", outfname);	
    ajFmtPrintF(outf, "# GNUPLOT data file for barchart\n");

    /* WARNING - this code will probably only work if MAXBINS==20, 
       BINSTART==0.0 and BINSIZE==0.05 */
    for(y=0; y<MAXBINS; y++)
	ajFmtPrintF(outf, "%-.3f    %d\n", 
		    (0.05*(y+1))-0.025, 
		    ajIntGet(freqs, y));
    ajFileClose(&outf);

    
    /* Create gnuplot driver file */
    if(!(outf = ajFileNewOut(fname)))
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





/* @funcstatic rocplot_write_summary ****************************************
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
** 
** @return [AjBool] True if file was succesfully written.
** @@
**
****************************************************************************/
static AjBool rocplot_write_summary(AjPFile outf, ajint mode, 
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
    	

    /* XXX Replace this with a call to the destructor function when one 
       becomes available */
    AJFREE(today);
    
    return ajTrue;
}




/* @funcstatic rocplot_XYdataNew ********************************************
**
** XYdata object constructor.
** This is normally called by the rocplot_XYdataNew function.
**
** @return [AjPXYdata] Pointer to an XYdata object
** @@
****************************************************************************/
AjPXYdata rocplot_XYdataNew(void)
{
    AjPXYdata ret = NULL;

    AJNEW0(ret);
    
    return ret;
}





/* @funcstatic rocplot_XYdataDel ********************************************
**
** Destructor for XYdata object.
**
** @param [w] thys [AjPXYdata*] XYdata object pointer
**
** @return [void]
** @@
****************************************************************************/
void rocplot_XYdataDel(AjPXYdata *thys)
{
    AjPXYdata pthis = *thys;

    if(!thys || !pthis)
        return;

    AJFREE(pthis);
    pthis = NULL;

    return;
}


/* @func rocplot_compX *****************************************************
**
** Function to sort AjOXYdata objects by X element. Usually called by 
** ajListSort.  
**
** @param [r] ptr1  [const void*] Pointer to AjOXYdata object 1
** @param [r] ptr2  [const void*] Pointer to AjOXYdata object 2
**
** @return [ajint] 1 if score1>score2, 0 if score1==score2, else -1.
** @@
****************************************************************************/

ajint rocplot_compX(const void *ptr1, const void *ptr2)
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



























