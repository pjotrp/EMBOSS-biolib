#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include "hsp_regex.h"

#include "hsp_utils.h"

/* === regerror.ih === */
static char *regatoi(const hsp_regex_t *preg, char *localbuf);

/* ========= end regerror.ih ========= */

/*
   = #define	HSREG_OKAY	 0
   = #define	HSREG_NOMATCH	 1
   = #define	HSREG_BADPAT	 2
   = #define	HSREG_ECOLLATE	 3
   = #define	HSREG_ECTYPE	 4
   = #define	HSREG_EESCAPE	 5
   = #define	HSREG_ESUBREG	 6
   = #define	HSREG_EBRACK	 7
   = #define	HSREG_EPAREN	 8
   = #define	HSREG_EBRACE	 9
   = #define	HSREG_BADBR	10
   = #define	HSREG_ERANGE	11
   = #define	HSREG_ESPACE	12
   = #define	HSREG_BADRPT	13
   = #define	HSREG_EMPTY	14
   = #define	HSREG_ASSERT	15
   = #define	HSREG_INVARG	16
   = #define	HSREG_ATOI	255	// convert name to number (!)
   = #define	HSREG_ITOA	0400	// convert number to name (!)
   */
static struct rerr {
    ajint code;
    char *name;
    char *explain;
} rerrs[] = {
    {HSREG_OKAY,	"HSREG_OKAY",	"no errors detected"},
    {HSREG_NOMATCH,	"HSREG_NOMATCH", "hsp_regexec() failed to match"},
    {HSREG_BADPAT,	"HSREG_BADPAT",	"invalid regular expression"},
    {HSREG_ECOLLATE,	"HSREG_ECOLLATE", "invalid collating element"},
    {HSREG_ECTYPE,	"HSREG_ECTYPE",	"invalid character class"},
    {HSREG_EESCAPE,	"HSREG_EESCAPE", "trailing backslash (\\)"},
    {HSREG_ESUBREG,	"HSREG_ESUBREG", "invalid backreference number"},
    {HSREG_EBRACK,	"HSREG_EBRACK",	"brackets ([ ]) not balanced"},
    {HSREG_EPAREN,	"HSREG_EPAREN",	"parentheses not balanced"},
    {HSREG_EBRACE,	"HSREG_EBRACE",	"braces not balanced"},
    {HSREG_BADBR,	"HSREG_BADBR",	"invalid repetition count(s)"},
    {HSREG_ERANGE,	"HSREG_ERANGE",	"invalid character range"},
    {HSREG_ESPACE,	"HSREG_ESPACE",	"out of memory"},
    {HSREG_BADRPT,	"HSREG_BADRPT",	"repetition-operator operand invalid"},
    {HSREG_EMPTY,	"HSREG_EMPTY",	"empty (sub)expression"},
    {HSREG_ASSERT,	"HSREG_ASSERT",	"\"can't happen\" -- you found a bug"},
    {HSREG_INVARG,	"HSREG_INVARG",	"invalid argument to regex routine"},
    {-1,		"",		"*** unknown regexp error code ***"},
};

/* @func hsp_regerror *********************************************************
**
** the interface to error numbers
**
** @param [r] errcode [ajint] Undocumented
** @param [r] preg [const hsp_regex_t *] Undocumented
** @param [r] errbuf [char *] Undocumented
** @param [r] errbuf_size [size_t] Undocumented
** @return [size_t] Undocumented
******************************************************************************/

size_t hsp_regerror(ajint errcode,const hsp_regex_t *preg,char *errbuf,
		    size_t errbuf_size)
{
    register struct rerr *r;
    register size_t len;
    register ajint target = errcode &~ HSREG_ITOA;
    register char *s;
    char convbuf[50];

    if (errcode == HSREG_ATOI)
	s = regatoi(preg, convbuf);
    else
    {
	for (r = rerrs; r->code >= 0; r++)
	    if (r->code == target)
		break;

	if (errcode&HSREG_ITOA)
	{
	    if (r->code >= 0)
		(void) strcpy(convbuf, r->name);
	    else
		(void) sprintf(convbuf, "HSREG_0x%x", target);
	    assert(strlen(convbuf) < sizeof(convbuf));
	    s = convbuf;
	}
	else
	    s = r->explain;
    }

    len = strlen(s) + 1;
    if (errbuf_size > 0)
    {
	if (errbuf_size > len)
	    (void) strcpy(errbuf, s);
	else
	{
	    (void) strncpy(errbuf, s, errbuf_size-1);
	    errbuf[errbuf_size-1] = '\0';
	}
    }

    return(len);
}

/* @funcstatic regatoi ********************************************************
**
** internal routine to implement HSREG_ATOI
**
** @param [r] preg [const hsp_regex_t *] Undocumented
** @param [r] localbuf [char *] Undocumented
** @return [char*] Undocumented
******************************************************************************/

static char* regatoi(const hsp_regex_t *preg,char *localbuf)
{
    register struct rerr *r;

    for (r = rerrs; r->code >= 0; r++)
	if (strcmp(r->name, preg->re_endp) == 0)
	    break;
    if (r->code < 0)
	return("0");

    (void) sprintf(localbuf, "%d", r->code);
    return(localbuf);
}
