/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */

#ifndef hsregexp_h
#define hsregexp_h

#include "ajarch.h"

#define NSUBEXP  10

/* @data AjPHsRegexp **********************************************************
**
** Regular expression data structure, based on the Henry Spencer
** regexp library.
**
** Regular expressions must be compiled before they can be used.
**
** @new ajHsRegComp Compiles a regular expression from an AjPStr string.
** @new ajHsRegCompC Compiles a regular expression from a char* string.
** @delete ajHsRegFree Clears and frees a compiled regular expression.
** @set ajHsRegExec Compares a regular expression to an AjPStr string.
** @set ajHsRegExecC Compares a regular expression to a char* string.
** @use ajHsRegSub Parses a string and substitues matches and submatches.
** @use ajHsRegSubC Parses a string and substitues matches and submatches.
** @use ajHsRegTrace Traces a compiled and used regular expression.
** @cast ajHsRegOffset Returns the offset of a match to an AjPStr.
** @cast ajHsRegOffsetI Returns the offset of the nth substring match.
** @cast ajHsRegOffsetC Returns the offset of a match to char*.
** @cast ajHsRegOffsetIC Returns the offset of the nth substring match.
** @cast ajHsRegLenI Returns the length of the nth substring match.
** @cast ajHsRegPost Returns the remainder of the string.
** @cast ajHsRegPostC Returns the remainder of the string as char*.
** @cast ajHsRegSubI Returns the nth substring match.
** @@
******************************************************************************/

/* Note that char program[1] must be at the end of the structure
 * as it is a place marker for the start of a node!
 */

typedef struct regexp {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	const char *orig;	/* AJAX extension: start of original */
	ajlong regsize;	/* AJAX extension: size */
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
        char padding[2];        /* Some chumminess with compiler */
	char *regmust;		/* Internal use only. */
	ajint regmlen;		/* Internal use only. */
	char program[1];	/* more chumminess with compiler. */
} regexp, AjOHsRegexp, *AjPHsRegexp;

extern regexp *hsregcomp(const char *re);
extern ajint hsregexec(regexp *rp, const char *s);
extern void hsregsub(const regexp *rp, const char *src, char *dst);
extern void hsregerror(const char *message);
extern void hsregerror2(const char* fmt, const char *str);
extern void hsregfree (regexp **r);

#ifndef HAVE_MEMMOVE
#ifndef EXIT_FAILURE
#  define EXIT_FAILURE   (1)             /* exit function failure       */
#endif
#endif

#endif
