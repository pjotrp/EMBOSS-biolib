/***  Sequence.c  *********************************************************
**
**
**  William A. Gilbert, Whitehead Institute
**
**************************************************************************/
#include <ctype.h>
#include <string.h>
#include "ckittypes.h"
#include "seqdef.h"

/*
** External functions used by this file.
*/

extern char *StrToUpper(char *);                           /* Strings.c */
extern char *StrCollapse(char *);                          /* Strings.c */
extern char *StrChange(char *, char, char);                /* Strings.c */


/*
** Function declaration and prototypes for this file
*/

 int  CheckSum(char *strand);
long  DeGap(char *strand);
char  CompBase(char base);
char *RevComp(char *strand, Choice type);
char *DNAtoRNA(char *strand);
char *RNAtoDNA(char *strand);
long NucToProtein(Choice geneticCode, char *strand);
double Weight(char *strand, Choice type);

/********************************  Methods  ********************************/
/**  CheckSum ***************************************************************
**
**  This function returns a checksum calculated from a sequence.  The method
**  used in the Devereux GCG package is implemented.
**
****************************************************************************/

int CheckSum(char *strand)

{
int checkSum, count;

	for (count=0, checkSum=0; *strand; strand++) {
	  checkSum += ++count * toupper((int)*strand);
	  if ( count == 57 ) count = 0;
	}

	return ( checkSum%10000 );

} /* End of CheckSum */

/***  RevComp  **********************************************************
**
**  This creates the reverse strand for a region of a DNA sequence.
**  For protein sequence the strand is reversed.
**
*************************************************************************/

char *RevComp(char *strand, Choice type)

{
char *ptr=strand;
int begin, end;
int i,temp=0;

	begin = 1;
	end = strlen(strand);
	for( i=1; i <= (begin+end)/2; i++) {
	  if ( type >= DNA ) {
	    temp = CompBase(strand[i-1]);
	    strand[i-1] = CompBase(strand[begin+end-i-1]);
	  } else if ( type <= PROTEIN ) {
	    temp = strand[i-1];
	    strand[i-1] = strand[begin+end-i-1];
	  }

	  strand[begin+end-i-1] = temp;
	}

	if ( type >= RNA ) DNAtoRNA(strand);

	return(ptr);

} /* End  of RevComp */

/**  CompBase  *************************************************************
**
**	This performs the  mapping:
**
**	    G -> C   X -> X   L -> K  R -> Y
**	    T -> A   Y -> R   Z -> Z  M -> J
**	    A -> T   S -> W   W -> S  K -> L
**	    C -> G   Q -> Q   J -> M  - -> -
**	    U -> A   N -> N    
**	    All Else -> UnChanged
**
**	Character case of the base is preserved.
**
***************************************************************************/

char CompBase(char base)

{
static int comp[26] = {
	  'T', 'V', 'G', 'H', '-', '-', 'C', 'D',   /* A B C D - - G H */
	  '-', '-', 'M', '-', 'K', 'N', '-', '-',   /* - - K - M N - - */
	  '-', 'Y', 'S', 'A', 'A', 'B', 'W', 'X',   /* - R S T U V W X */
 	  'R', '-'};                                /* Y -             */

	if ( isalpha((int)base) )
	   base = islower((int)base) ? comp[base-'a'] + ' ' : comp[base-'A'] ;

	return(base);

}  /* End of CompBase */

/***  DNAtoRNA  *********************************************************
**
**  Converts all T's to U's and t's to u's
**
************************************************************************/

char *DNAtoRNA(char *strand)

{
char *ptr = strand;

	while( *strand ) {
	  if ( *strand == 'T' ) *strand = 'U';
	  if ( *strand == 't' ) *strand = 'u';
	  *strand++;
	}

	return(ptr);

} /* End of DNAtoRNA */

/***  RNAtoDNA  **********************************************************
**
**  Converts all U's to T's and u's to t's
**
**************************************************************************/

char *RNAtoDNA(char *strand)

{
char *ptr = strand;

	while( *strand ) {
	  if ( *strand == 'U' ) *strand = 'T';
	  if ( *strand == 'u' ) *strand = 't';
	  *strand++;
	}

	return(ptr);

} /* End of RNAtoDNA */

/**  NucToProtein  ***********************************************************
**
**  This function translates a nucleotide strand, "Strand", of length, 
**  "SeqLength" into a protein sequence using Genetic Code, SGCode, for 
**  translation.
**  
**  The translation continues until a stop codon or the end of the nucleotide
**  strand occurs.  The translated sequence is returned in the string "Strand".
**  The function returns a pointer to the new strand.
**
**  Special Genetic Codes:
**  ---------------------
**    0 - Universal Genetic code         (UGC)
**    1 - Mammalian Mitochondrial code   (SGC1)
**    2 - Yeast Mitochondrial code       (SGC2)
**    3 - Aspergillus Mitochondrial code (SGC3)
**    4 - Drosophilia Mitochondrial code (SGC4)
**    5 - Ciliate Macronuclear code      (SGC5)
**    9 - Stop codons translated as "x"  
**
**  Translation Algorithm:
**  ---------------------
**  In order to generate an index into the GCodes arrays each base in
**  the codon is looked up in "NonAmbigBases", if it's not found the
**  Index in increased such that it will be beyond the valid range.  If the
**  base is either T,C,A,G or U then Index is incremented according to the
**  amount in "Indx" depending on the base position in the codon.
**
**  For example, consider the codon, ATG
**
**                   T  C  A  G  U
**                   -------------
**      First base   0 16 32 48  0         A give  32  
**      Second base  0  4  8 12  0         T gives  0
**      Third base   1  2  3  4  1         G gives  4
**      
**  By looking at the 36th character in "GCodes" we obtain an "M".
**  
**  This algorithm is used by the NBRF-PIR program NAQ from which it was
**  translated to "C" from FORTRAN.
**
**  RESTRICTION: Will not translate "RAY" to the ambiguous amino acid "B"
**               or the codon "SAR" to the ambiguous amino acid "Z"
**
**    William A. Gilbert, Whitehead Institute
**
*****************************************************************************/

long NucToProtein(Choice sgCode, char *strand)

{

/*
** Universal and Special Genetic Code translation tables
*/

#define CODES 9    /* Number of special genetic codes */

static char gCodes[CODES+1][65] =  {  
   " FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
   " FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSS**VVVVAAAADDEEGGGG",
   " FFLLSSSSYY**CCWWTTTTPPPPHHQQRRRRIIMMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
   " FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
   " FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSSSSVVVVAAAADDEEGGGG",
   " FFLLSSSSYYQQCC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
   " FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
   " FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
   " FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
   " FFLLSSSSYYxxCCxWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG"
   };

/*
** Ambiguous nucleotide weights i.e. binary equivalents
*/

static char numNuc[26] = 
   /*  A B C  D E F G  H I J K L  M  N O P Q  R S T U  V W  X Y Z*/
      {8,7,4,11,0,0,2,13,0,0,3,0,12,15,0,0,0,10,6,1,1,14,9,15,5,0};

/*
**  Nucleotide look-up table weights
*/

static char nonAmbigBases[] = "TCAGU";
static int indx[3][5] = {             {0, 16, 32, 48,  0},
                               {0,  4,  8, 12,  0},
                               {1,  2,  3,  4,  1} };

char *pStrand, *pPos;
long seqLength;
long lPro;
int index, base;
int i, j, k;

static int nBase[4] = {1,4,8,2};
int nNuc[3], nAlt;

	if ( sgCode > CODES ) sgCode = 0;
	StrToUpper(strand);

	lPro = 0;
	base = 0;
	pStrand=strand;
	seqLength = strlen(strand);
	while( base <= seqLength-3 ) {
	  for ( index=0, i=0; i<3; i++)
	    if ( (pPos = strchr(nonAmbigBases,strand[base+i])) )
	      index += indx[i][pPos-nonAmbigBases];
	    else
	      index += 65;   /* Amibiguous base pushes total out of range */

	  if ( index <= 64 ) {
	    *pStrand = gCodes[sgCode][index];
	  } else {
	    nNuc[0] = numNuc[strand[base]-65];
	    nNuc[1] = numNuc[strand[base+1]-65];
	    nNuc[2] = numNuc[strand[base+2]-65];
	    nAlt = 0;
	    *pStrand = 'X';      /* any ambiguous codons return unknown */
	    for ( i=0; i<=3; i++ ) {
	      if ( (nBase[i] & nNuc[0]) == 0 ) continue;
	      for ( j=0; j<=3; j++) {
	        if ( (nBase[j] & nNuc[1]) == 0 ) continue;
	        for ( k=0; k<=3; k++ ) {
	          if ( (nBase[k] & nNuc[2]) == 0 ) continue;
	          nAlt++;
	          index = indx[0][i] + indx[1][j] + indx[2][k];
	          if ( nAlt == 1 )
	            *pStrand = gCodes[sgCode][index];
	          else if ( *pStrand != gCodes[sgCode][index] ) {
	            *pStrand = 'X';
	            goto Next;	  
	          }
	        }
	      }
	    }  
	  }

Next:
	  if ( *pStrand == '*' ) break;
	  lPro++;
	  base+=3;
	  pStrand++;
	}

	*pStrand = '\0';
	return(lPro);

} /* End of NucToProtein */

/***  Weight  ******************************************************************
**
**   WEIGHT returns the molecular weights of a sequence of amino acids or
**   nucleotides.
**
**  Input:
**      "Seq" is the sequence, "Type" is the type of molecule.
**
**  Output:
**      WEIGHT returns the molecular weight as a double.
**
** Credits: Borrowed from NBRF's PSQ program
**          W. Gilbert, Whitehead Institute
**
***************************************************************************/

double Weight(char *strand, Choice type)

{

/* Amino acid molecular weight table */

static double pW[24]={
               89.09,    121.15,    133.10,    147.13,   /* ACDE */
              165.19,     75.07,    155.16,    131.17,   /* FGHI */
              146.19,    131.17,    149.21,    132.12,   /* KLMN */
              115.13,    146.15,    174.20,    105.09,   /* PQRS */
              119.12,    117.15,    204.23,    181.19,   /* TVWY */
              132.61,    146.64,      0.00,              /* BZ-  */
              128.16   };                                /* All others */

/* Nucleic acid moleclar weight table */

static double nW[18]={
              347.22, 323.20, 363.22, 322.21, 308.18,   0.00, /* ACGTU- */
	      335.21, 355.22, 334.72, 343.21, 322.70, 342.72, /* MRWSYK */
	      344.55, 330.88, 344.23, 336.21, 338.96,         /* VHDBN  */
	      338.96 };                                       /* All others */
              
Boolean protein;
static char pNames[24] = "ACDEFGHIKLMNPQRSTVWYBZ-";
static char nNames[18] = "ACGTU-MRWSYKVHDBN";
char *names;

char *pSeq, test;

double *w;                                              

double total;        
int i, lenght;

	total = 0.0;
	lenght = 0;

	protein = (type <= PROTEIN ) ? 1 : 0;

	names = ( protein ) ? pNames : nNames;  /* Point to correct string */
	w = ( protein ) ? pW : nW ;             /* point to correct table  */

	for(pSeq=strand; *pSeq; pSeq++) {
          test = (char) toupper((int)*pSeq);
	  for(i=0; names[i]; i++)
	    if ( test == names[i] ) break;

	  if ( test ^= '-' ) lenght++;          /* Don't count gaps       */
	  total += w[i];
	}

	/* Remove "water" for each peptide nucleotide bond */

	total -= (lenght-1) * 18.015;

	return((lenght > 0) ? total : 0.0);

} /* End of Weight */

/***  DeGap  *******************************************************************
**
**
*******************************************************************************/

long DeGap(char * strand)
{
	StrChange(strand,'-',' ');
	StrCollapse(strand);

	return(strlen(strand));
}

