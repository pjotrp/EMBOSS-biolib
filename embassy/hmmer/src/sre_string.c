/*****************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 *****************************************************************/

/* sre_string.c
 * 
 * my library of extra string functions. Some for portability
 * across UNIXes
 *
 * RCS $Id: sre_string.c,v 1.1 2001/07/29 14:13:49 ajb Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "squid.h"

#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif

/* global sqd_parse[] are managed by Strparse().
 */
char *sqd_parse[10];


/* Function: Strdup()
 * 
 * Purpose:  Implementation of the common (but non-ANSI) function
 *           strdup(). Robust against being passed a NULL pointer.
 *           
 */
char *
Strdup(char *s)
{
  char *new;
  if (s == NULL) return NULL;
  if ((new = (char *) malloc (strlen(s) +1)) == NULL) return NULL;
  strcpy(new, s);
  return new;
}

/* Function: StringChop()
 * Date:     SRE, Wed Oct 29 12:10:02 1997 [TWA 721]
 * 
 * Purpose:  Chop trailing whitespace off of a string.
 */
void
StringChop(char *s)
{
  int   i;

  i = strlen(s) - 1;		         /* set i at last char in string     */
  while (i >= 0 && isspace((int)s[i])) i--;   /* i now at last non-whitespace char, or -1 */
  s[i+1] = '\0';
}

int
Strinsert(char  *s1,            /* string to insert a char into  */
	  char   c,		/* char to insert                */
	  int    pos)		/* position in s1 to insert c at */
{
  char    oldc;
  char   *s;

  for (s = s1 + pos; c; s++)
    {
				/* swap current char for inserted one */
      oldc = *s;		/* pick up current */
      *s   = c;   		/* put down inserted one    */
      c    = oldc;		/* old becomes next to insert */
    }
  *s = '\0';

  return 1;
}


int
Strdelete(char *s1,             /* string to delete a char from       */
	  int   pos)		/* position of char to delete 0..n-1  */
{
  char *s;                      

  for (s = s1 + pos; *s; s++)
    *s = *(s + 1);

  return 1;
}

void
s2lower(char *s)
{
  for (; *s != '\0'; s++)
    *s = sre_tolower((int) *s);
}

void
s2upper(char *s)
{
  for (; *s != '\0'; s++)
    *s = sre_toupper((int) *s);
}


void *
sre_malloc(char *file, int line, size_t size)
{
  void *ptr;

  if ((ptr = malloc (size)) == NULL)
    Die("malloc of %d bytes failed: file %s line %d", size, file, line);
  return ptr;
}

void *
sre_realloc(char *file, int line, void *p, size_t size)
{
  void *ptr;

  if ((ptr = realloc(p, size)) == NULL)
    Die("realloc of %d bytes failed: file %s line %d", size, file, line);
  return ptr;
}


/* Function: Strparse()
 * 
 * Purpose:  Match a regexp to a string. Returns 1 if pattern matches,
 *           else 0.
 *
 *           Much like Perl, Strparse() makes copies of the matching
 *           substrings available via globals, sqd_parse[].
 *           sqd_parse[0] contains a copy of the complete matched
 *           text. sqd_parse[1-9] contain copies of up to nine
 *           different substrings matched within parentheses.
 *           The memory for these strings is internally managed and
 *           volatile; the next call to Strparse() may destroy them.
 *           If the caller needs the matched substrings to persist
 *           beyond a new Strparse() call, it must make its own 
 *           copies.
 *           
 *           A minor drawback of the memory management is that
 *           there will be a small amount of unfree'd memory being
 *           managed by Strparse() when a program exits; this may
 *           confuse memory debugging (Purify, dbmalloc). The
 *           general cleanup function SqdClean() is provided;
 *           you can call this before exiting.
 *           
 *           Uses an extended POSIX regular expression interface.
 *           A copylefted GNU implementation is included in the squid
 *           implementation (gnuregex.c) for use on non-POSIX compliant
 *           systems. POSIX 1003.2-compliant systems (all UNIX,
 *           some WinNT, I believe) can omit the GNU code if necessary.
 *           
 *           I built this for ease of use, not speed nor efficiency.
 *
 * Example:  Strparse("foo-...-baz", "foo-bar-baz")  returns 0
 *           Strparse("foo-(...)-baz", "foo-bar-baz")
 *              returns 0; sqd_parse[0] is "foo-bar-baz";
 *              sqd_parse[1] is "bar".
 *              
 * Args:     rexp  - regular expression, extended POSIX form
 *           s     - string to match against
 *           ntok  - number of () substrings we will save (maximum NSUBEXP-1)
 *                   
 * Return:   1 on match, 0 if no match
 */
int
Strparse(char *rexp, char *s, int ntok)
{
  sqd_regexp *pat;
  int         code;
  int         len;
  int         i;
				/* sanity check */
  if (ntok >= NSUBEXP )  Die("Strparse(): ntok must be <= %d", NSUBEXP-1); 

  /* Free previous global substring buffers
   */
  for (i = 0; i <= ntok; i++)
    if (sqd_parse[i] != NULL) 
      { 
	free(sqd_parse[i]);
	sqd_parse[i] = NULL;
      }

  /* Compile and match the pattern, using our modified 
   * copy of Henry Spencer's regexp library
   */
  if ((pat = sqd_regcomp(rexp)) == NULL) 
    Die("regexp compilation failed.");
  code = sqd_regexec(pat, s);

  /* Fill the global substring buffers
   */
  if (code == 1) 
    for (i = 0; i <= ntok; i++)
      if (pat->startp[i] != NULL && pat->endp[i] != NULL)
	{
	  len = pat->endp[i] - pat->startp[i];
	  sqd_parse[i] = (char *) MallocOrDie(sizeof(char) * (len+1));
	  strncpy(sqd_parse[i], pat->startp[i], len);
	  sqd_parse[i][len] = '\0';
	}

  free(pat);
  return code;
}


/* Function: SqdClean()
 * Date:     SRE, Wed Oct 29 12:52:08 1997 [TWA 721]
 * 
 * Purpose:  Clean up any squid library allocations before exiting
 *           a program, so we don't leave unfree'd memory around
 *           and confuse a malloc debugger like Purify or dbmalloc.
 */
void
SqdClean(void)
{
  int i;

  /* Free global substring buffers that Strparse() uses
   */
  for (i = 0; i <= 9; i++)
    if (sqd_parse[i] != NULL) {
      free(sqd_parse[i]);
      sqd_parse[i] = NULL;
    }
}



/* Function: StrShuffle()
 * 
 * Purpose:  Returns a shuffled version of s2, in s1.
 *           (s1 and s2 can be identical, to shuffle in place.)
 *  
 * Args:     s1 - allocated space for shuffled string.
 *           s2 - string to shuffle.
 *           
 * Return:   void
 */
void
StrShuffle(char *s1, char *s2)
{
  int  len;
  int  pos;
  char c;
  
  if (s1 != s2) strcpy(s1, s2);
  for (len = strlen(s1); len > 1; len--)
    {				
      pos       = CHOOSE(len);
      c         = s1[pos];
      s1[pos]   = s1[len-1];
      s1[len-1] = c;
    }
}
  
/* Function: StrReverse()
 * Date:     SRE, Thu Nov 20 10:54:52 1997 [St. Louis]
 * 
 * Purpose:  Returns a reversed version of s2, in s1.
 *           (s1 and s2 can be identical, to reverse in place)
 * 
 * Args:     s1 - allocated space for reversed string.
 *           s2 - string to reverse.
 *           
 * Return:   (void)
 */                
void
StrReverse(char *s1, char *s2)
{
  int  len;
  int  pos;
  char c;
  
  if (s1 != s2) strcpy(s1, s2);
  len = strlen(s1);
  for (pos = 0; pos < len/2; pos++)
    {				/* swap ends */
      c             = s1[len-pos-1];
      s1[len-pos-1] = s1[pos];
      s1[pos]       = c;
    }
}

/* Function: StrRegionalShuffle()
 * Date:     SRE, Thu Nov 20 11:02:34 1997 [St. Louis]
 * 
 * Purpose:  Returns a regionally shuffled version of s2, in s1.
 *           (s1 and s2 can be identical to regionally 
 *           shuffle in place.) See [Pearson88].
 *           
 * Args:     s1 - allocated space for regionally shuffled string.
 *           s2 - string to regionally shuffle
 *           w  - window size (typically 10 or 20)      
 *           
 * Return:   (void)
 */
void
StrRegionalShuffle(char *s1, char *s2, int w)
{
  int  len;
  char c;
  int  pos;
  int  i, j;

  if (s1 != s2) strcpy(s1, s2);
  len = strlen(s1);

  for (i = 0; i < len; i += w)
    for (j = MIN(len-1, i+w-1); j > i; j--)
      {
	pos     = i + CHOOSE(j-i);
	c       = s1[pos];
	s1[pos] = s1[j];
	s1[j]   = c;
      }
}





/* Function: RandomSequence()
 * 
 * Purpose:  Generate an iid symbol sequence according
 *           to some alphabet, alphabet_size, probability
 *           distribution, and length. Return the
 *           sequence.
 *           
 * Args:     alphabet  - e.g. "ACGT"
 *           p         - probability distribution [0..n-1]
 *           n         - number of symbols in alphabet
 *           len       - length of generated sequence 
 *           
 * Return:   ptr to random sequence, or NULL on failure.
 */
char *
RandomSequence(char *alphabet, float *p, int n, int len)
{
  char *s;
  int   x;

  s = (char *) MallocOrDie (sizeof(char) * (len+1));
  for (x = 0; x < len; x++)
    s[x] = alphabet[FChoose(p,n)];
  s[x] = '\0';
  return s;
}

