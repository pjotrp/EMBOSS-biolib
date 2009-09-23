## Process this file with automake to produce Makefile.in

bin_PROGRAMS = emast ememe ememetext

if LOCALLINK
INCLUDES = -I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/core -I../../../ajax/graphics \
	-I../../../ajax/ensembl \
	-I../../../ajax/ajaxseq -I../../../ajax/acd \
	-I../../../plplot
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs \
-L../../../ajax/pcre/.libs -L../../../ajax/core/.libs \
-L../../../ajax/graphics/.libs -L../../../ajax/ensembl/.libs \
-L../../../ajax/ajaxseq/.libs -L../../../ajax/acd/.libs \
-L../../../nucleus/.libs \
-lnucleus -lacd -lajaxseq -lensembl -lajaxg -lajax -lepcre -leplplot
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxseq -lensembl -lajaxg -lajax -lepcre -leplplot
endif
else
AM_CFLAGS =
endif
endif

emast_SOURCES = emast.c
ememe_SOURCES = ememe.c
ememetext_SOURCES = ememetext.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxseq/libajaxseq.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la ../../../ajax/pcre/libepcre.la \
	../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lacd -lajaxseq -lensembl -lajaxg \
        -lajax -lepcre -leplplot $(XLIB)
endif
