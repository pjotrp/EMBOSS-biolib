## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libajax.la libajaxg.la

if ISSHARED
if ISAIXIA64
if NEEDAJAX
AM_CFLAGS = -Wl,-G -L../plplot/.libs -L.libs -lajax -lplplot -lm
else
AM_CFLAGS = -Wl,-G -L../plplot/.libs -L.libs -lplplot -lm
endif
else
AM_CFLAGS =
endif
endif

## PCRE stuff (evrsion 4.3)

POSIX_MALLOC_THRESHOLD = @POSIX_MALLOC_THRESHOLD@
PCRELIBVERSION = @PCRE_LIB_VERSION@
PCREPOSIXLIBVERSION = @PCRE_POSIXLIB_VERSION@

INCLUDES = -DAJAX_FIXED_ROOT=$(AJAX_FIXED_ROOT) -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/plplot $(POSIX_MALLOC_THRESHOLD) 
#LDFLAGS = -no-undefined
LIBS =

AJAXSRC = ajacd.c ajalign.c ajarr.c ajassert.c \
ajbase.c \
ajcall.c ajcod.c \
ajdan.c ajexcept.c \
ajfeat.c ajfile.c ajfmt.c ajgraphxml.c ajhsreg.c ajjava.c ajlist.c \
ajmath.c ajmatrices.c ajmem.c ajmess.c ajnam.c ajphylo.c \
ajposreg.c ajrange.c ajreg.c ajreport.c \
ajseq.c ajseqabi.c ajseqdb.c ajseqread.c ajseqtype.c ajseqwrite.c \
ajsort.c ajstr.c ajsys.c \
ajtable.c ajtime.c ajtranslate.c ajutil.c ajxyz.c \
hsp_debug.c hsp_regcomp.c hsp_regerror.c \
hsp_regexec.c hsp_regfree.c hsp_split.c \
hsregerror.c hsregexp.c hsregsub.c \
pcre.c pcre_get.c pcre_study.c pcreposix.c

# included in pcre.c : pcre_chartables.c pcre_printint.c

AJAXGSRC = ajgraph.c ajhist.c

noinst_HEADERS = ajax.h ajarch.h ajdefine.h \
ajacd.h ajalign.h ajarr.h ajassert.h \
ajbase.h ajcall.h ajcod.h \
ajdan.h ajexcept.h ajfeat.h ajfeatdata.h ajfile.h ajfmt.h \
ajgraph.h ajgraphxml.h ajgraphstruct.h ajhist.h ajhsreg.h ajjava.h ajlist.h \
ajmath.h ajmatrices.h ajmem.h ajmess.h ajnam.h ajphylo.h \
ajposreg.h ajrange.h ajreg.h ajreport.h \
ajseq.h ajseqabi.h ajseqdata.h ajseqdb.h ajseqread.h ajseqtype.h ajseqwrite.h \
ajsort.h ajstr.h ajsys.h \
ajtable.h ajtime.h ajtranslate.h ajutil.h ajxyz.h \
hsregmagic.h hsregexp.h \
hsp_cclass.h hsp_cname.h hsp_engine.c hsp_engine.h \
hsp_regcomp.h hsp_regerror.h \
hsp_regex.h hsp_regex2.h hsp_utils.h \
pcre.h pcre_config.h pcre_internal.h pcreposix.h


libajax_la_SOURCES = $(AJAXSRC)
libajax_la_LDFLAGS = -version-info 1:0:1
libajaxg_la_SOURCES = $(AJAXGSRC)
libajaxg_la_LDFLAGS = -version-info 1:0:1
