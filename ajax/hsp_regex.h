#ifndef hsp_regex_h
#define	hsp_regex_h	/* never again */

#define NPSUBEXP  10
#include "ajarch.h"
#include <sys/types.h>


/* === hsp_regex2.h === */

typedef struct re_guts REGUTSSTRUCT;
typedef off_t hsp_regoff_t;

/* @data hsp_regex_t **********************************************************
**
** Internal data structure for the Henry Spencer regular expression
** (regex) library
**
******************************************************************************/

typedef struct hsp_regex_t {
    const char* orig;		/* AJAX extension: start of original */
    ajint re_magic;
    size_t re_nsub;		/* number of parenthesized subexpressions */
    const char *re_endp;	/* end pointer for HSREG_PEND */
    REGUTSSTRUCT *re_g;		/* none of your business :-) */
} hsp_regex_t;

/* @data AjPPosRegmatch *******************************************************
**
** Internal data structure for the Henry Spencer regular expression
** (regex) library.
**
** @alias hsp_regmatch_t
** @alias AjOPosRegmatch
**
******************************************************************************/

typedef struct {
    hsp_regoff_t rm_so;		/* start of match */
    hsp_regoff_t rm_eo;		/* end of match */
} hsp_regmatch_t, AjOPosRegmatch, *AjPPosRegmatch;

/* @data AjPPosRegexp *********************************************************
 **
 ** Regular expression data structure, based on the Henry Spencer
 ** regexp library.
 **
 ** Regular expressions must be compiled before they can be used.
 **
 ** @new ajPosRegComp Compiles a regular expression from an AjPStr string.
 ** @new ajPosRegCompC Compiles a regular expression from a char* string.
 ** @new ajPosRegCompCase Compiles a regular expression from an
 **                          AjPStr string that is case insensitive.
 ** @new ajPosRegCompCaseC Compiles a regular expression from a
 **                          char* string that is case insensitive.
 ** @new ajPosRegCompNosub Compiles a regular expression from an
 **                          AjPStr string that ignores saved substrings.
 ** @new ajPosRegCompNosubC Compiles a regular expression from a
 **                          char* string that ignores saved substrings.
 ** @new ajPosRegCompNewline Compiles a regular expression from an
 **                          AjPStr string that uses internal newlines.
 ** @new ajPosRegCompNewlineC Compiles a regular expression from a
 **                          char* string that uses internal newlines.
 ** @delete ajPosRegFree Clears and frees a compiled regular expression.
 ** @set ajPosRegExec Compares a regular expression to an AjPStr string.
 ** @set ajPosRegExecC Compares a regular expression to a char* string.
 ** @cast ajPosRegOffset Returns the offset of a match to an AjPStr.
 ** @cast ajPosRegOffsetI Returns the offset of the nth substring match.
 ** @cast ajPosRegOffsetC Returns the offset of a match to char*.
 ** @cast ajPosRegOffsetIC Returns the offset of the nth substring match.
 ** @cast ajPosRegLenI Returns the length of the nth substring match.
 ** @cast ajPosRegPost Returns the remainder of the string.
 ** @cast ajPosRegPostC Returns the remainder of the string as char*.
 ** @cast ajPosRegSubI Returns the nth substring match.
 ** @use ajPosRegSub Parses a string and substitues matches and submatches.
 ** @use ajPosRegSubC Parses a string and substitues matches and submatches.
 ** @use ajPosRegTrace Traces a compiled and used regular expression.
 ** @@
 ****************************************************************************/

typedef struct AjSPosRegexp{
    ajint Subs;			/* size of match array */
    hsp_regex_t *Regex;
    AjPPosRegmatch Match;
} AjOPosRegexp, *AjPPosRegexp;

/* === hsp_regcomp.c === */
extern ajint hsp_regcomp(hsp_regex_t *, const char *, ajint);
#define	HSREG_BASIC	0000
#define	HSREG_EXTENDED	0001
#define	HSREG_ICASE	0002
#define	HSREG_NOSUB	0004
#define	HSREG_NEWLINE	0010
#define	HSREG_NOSPEC	0020
#define	HSREG_PEND	0040
#define	HSREG_DUMP	0200


/* === regerror.c === */
#define	HSREG_OKAY	 0
#define	HSREG_NOMATCH	 1
#define	HSREG_BADPAT	 2
#define	HSREG_ECOLLATE	 3
#define	HSREG_ECTYPE	 4
#define	HSREG_EESCAPE	 5
#define	HSREG_ESUBREG	 6
#define	HSREG_EBRACK	 7
#define	HSREG_EPAREN	 8
#define	HSREG_EBRACE	 9
#define	HSREG_BADBR	10
#define	HSREG_ERANGE	11
#define	HSREG_ESPACE	12
#define	HSREG_BADRPT	13
#define	HSREG_EMPTY	14
#define	HSREG_ASSERT	15
#define	HSREG_INVARG	16
#define	HSREG_ATOI	255	/* convert name to number (!) */
#define	HSREG_ITOA	0400	/* convert number to name (!) */
extern size_t hsp_regerror(ajint, const hsp_regex_t *, char *, size_t);


/* === regexec.c === */
extern ajint hsp_regexec(const hsp_regex_t *, const char *, size_t,
		       hsp_regmatch_t [], ajint);

#define	HSREG_NOTBOL	00001
#define	HSREG_NOTEOL	00002
#define	HSREG_STARTEND	00004
#define	HSREG_TRACE	00400	/* tracing of execution */
#define	HSREG_LARGE	01000	/* force large representation */
#define	HSREG_BACKR	02000	/* force use of backref code */


/* === regfree.c === */
extern void hsp_regfree(hsp_regex_t *);

/* ========= end header generated by ./mkh ========= */
#endif
