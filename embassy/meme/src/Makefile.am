## Process this file with automake to produce Makefile.in

SUBDIRS = INCLUDE

lib_LTLIBRARIES = libmeme.la

if ISCYGWIN
CYGWIN_LDFLAGS = -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--enable-auto-import
if LOCALLINK
CYGWIN_LDMEME = -L../../../plplot -L../../../ajax/pcre -L../../../ajax/expat \
-L../../../ajax/core -L../../../ajax/graphics -L../../../ajax/ensembl \
-L../../../ajax/ajaxdb -L../../../ajax/acd -L../../../nucleus \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leplplot -leexpat -lepcre
else
CYGWIN_LDMEME = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl \
                -lajaxg -lajax -leplplot -leexpat -lepcre
endif
endif

bin_PROGRAMS = omeme

if LOCALLINK
INCLUDES = -IINCLUDE \
	-I../../../nucleus -I../../../ajax/pcre \
	I../../../ajax/expat -I../../../ajax/core \
	-I../../../ajax/graphics -I../../../ajax/ensembl \
	-I../../../ajax/ajaxdb -I../../../ajax/acd \
	-I../../../plplot
else
INCLUDES = -IINCLUDE -I${embprefix}/include \
	-I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs \
-L../../../ajax/pcre/.libs -L../../../ajax/expat/.libs \
-L../../../ajax/core/.libs -L../../../ajax/graphics/.libs \
-L../../../ajax/ensembl/.libs -L../../../ajax/ajaxdb/.libs \
-L../../../ajax/acd/.libs -L../../../nucleus/.libs \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -leplplot
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -leplplot
endif
else
AM_CFLAGS =
endif
endif


MEMESRC = clock.c display.c em.c init.c meme_util.c seq2theta.c \
	shorten.c starts.c prior.c split.c align.c justlike.c \
	oops.c tcm.c zoops.c banner.c logs.c palindrome.c gamma.c \
	likelihood.c subseq7.c normalize.c chi.c mp.c message.c \
	diagram.c logodds.c regress.c \
	hash.c hash_alph.c read_seq_file.c

libmeme_la_SOURCES = $(MEMESRC)

VERS_INF = -version-info 2:1:1

if ISCYGWIN
LINKFLAGS = $(VERS_INF) $(CYGWIN_LDFLAGS) $(CYGWIN_LDMEME)
else
LINKFLAGS = $(VERS_INF)
endif

libmeme_la_LDFLAGS = $(LINKFLAGS)

omeme_SOURCES = meme.c

if LOCALLINK
LDADD = libmeme.la \
	../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la ../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la ../../../plplot/libeplplot.la \
	$(XLIB)
else
LDADD = libmeme.la -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl \
	-lajaxg -lajax -leexpat -lepcre -leplplot $(XLIB)
endif
