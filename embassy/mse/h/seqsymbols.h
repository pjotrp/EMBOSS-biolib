/**  SeqSymbols.h ******************************************************
**
**  Note the values for the first indice of the SeqCharSet is dependent
**  on the sequence format defintion found in the file "SeqDef.h".
**
**    William A. Gilbert, Whitehead Institute, MIT
**
**  Used by:
**           ReadSeq
**
*************************************************************************/

#define NOTSEQ    -1     /* Not a sequence character i.e puctuation     */
#define NEXTENTRY -2     /* Begining of the next entry i.e. '>' for NBRF*/
#define ENDSEQ    -3     /* End of Sequence character i.e. '*' for NBRF */
#define IGNORE    -4     /* Ignore the remainder of this line           */
#define COMMENT   -5     /* Start of a comment with the sequence        */

int seqCharSet[7][128]={

/* NBRF Sequence Symbols */
/* Return upper and lower case, "-" as the gap character, "!" as the 
** ignore character, ">" as the "Start of new entry" character, "*"
** as the end of sequence character.
*/
{  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -3, -1, -1,'-', -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -1,
  -1,'A','B','C','D','E','F','G','H','I', -1,'K','L','M','N', -1,
 'P','Q','R','S','T','U','V','W','X','Y','Z', -1, -1, -1, -1, -1,
  -1,'a','b','c','d','e','f','g','h','i', -1,'k','l','m','n', -1,
 'p','q','r','s','t','u','v','w','x','y','z', -1, -1, -1, -1, -1  },

/* RAW Sequence Symbols  */
/* Return upper and lower case, "-" as the gap character, "!" as the 
** ignore character.
*/
{  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,'-', -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1,'A','B','C','D','E','F','G','H','I', -1,'K','L','M','N', -1,
 'P','Q','R','S','T','U','V','W','X','Y','Z', -1, -1, -1, -1, -1,
  -1,'a','b','c','d','e','f','g','h','i', -1,'k','l','m','n', -1,
 'p','q','r','s','t','u','v','w','x','y','z', -1, -1, -1, -1, -1  },

/* Staden Sequence Symbols */
/* Return upper and lower case.
**  5,6,7,8 map to IUPAC symbols M,K,W,S
**  1,2,3,4 map to the ambiguous symbols c,t,a,g
**  '-' is the gap character.
*/
{  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,'-', -1, -1,
  -1,'c','t','a','g','M','K','W','S', -1, -1, -1, -1, -1, -1, -1,
  -1,'A','B','C','D','E','F','G','H','I', -1,'K','L','M','N', -1,
 'P','Q','R','S','T','U','V','W','X','Y','Z', -1, -1, -1, -1, -1,
  -1,'a','b','c','d','e','f','g','h','i', -1,'k','l','m','n', -1,
 'p','q','r','s','t','u','v','w','x','y','z', -1, -1, -1, -1, -1  },

/* GCG Sequence Symbols  */
/* Return upper and lower case, "." as the gap character, ">", "<" 
** and "$" comment characters.  In UWGCG format these characters bound
** the comment and may cross lines.
*/
{  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -5, -1, -1, -1, -1, -1, -3, -1, -1, -1,'-', -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -5, -1, -5, -1,
  -1,'A','B','C','D','E','F','G','H','I', -1,'K','L','M','N', -1,
 'P','Q','R','S','T','U','V','W','X','Y','Z', -1, -1, -1, -1, -1,
  -1,'a','b','c','d','e','f','g','h','i', -1,'k','l','m','n', -1,
 'p','q','r','s','t','u','v','w','x','y','z', -1, -1, -1, -1, -1  },

/* IG Sequence Symbols */
/* Return upper and lower case, "-" as the gap character, ";" as the 
** ignore character, "1" and "2" as end of sequence characters
*/
{  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -3, -1, -1,'-', -1, -1,
  -1, -3, -3, -3, -3, -3, -3, -3, -3, -3, -1, -1, -1, -1, -2, -1,
  -1,'A','B','C','D','E','F','G','H','I', -1,'K','L','M','N', -1,
 'P','Q','R','S','T','U','V','W','X','Y','Z', -1, -1, -1, -1, -1,
  -1,'a','b','c','d','e','f','g','h','i', -1,'k','l','m','n', -1,
 'p','q','r','s','t','u','v','w','x','y','z', -1, -1, -1, -1, -1  },
                                                                
/* IBI Sequence Symbols */
/* Return upper and lower case, "-" as the gap character, "/" as the 
**  end of entry character
*/
{  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,'-', -1, -3,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1,'A','B','C','D','E','F','G','H','I', -1,'K','L','M','N', -1,
 'P','Q','R','S','T','U','V','W','X','Y','Z', -1, -1, -1, -1, -1,
  -1,'a','b','c','d','e','f','g','h','i', -1,'k','l','m','n', -1,
 'p','q','r','s','t','u','v','w','x','y','z', -1, -1, -1, -1, -1  },

/* DNA Strider Sequence Symbols  */
/* Return upper and lower case, "-" as the gap character, ";" as the 
** ignore character. // denotes the end of the sequence.
*/

{  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,'-', -1, -3,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -4, -1, -1, -1, -1,
  -1,'A','B','C','D','E','F','G','H','I', -1,'K','L','M','N', -1,
 'P','Q','R','S','T','U','V','W','X','Y','Z', -1, -1, -1, -1, -1,
  -1,'a','b','c','d','e','f','g','h','i', -1,'k','l','m','n', -1,
 'p','q','r','s','t','u','v','w','x','y','z', -1, -1, -1, -1, -1  }

};

