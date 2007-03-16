## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = cathparse domainnr domainreso domainseqs domainsse \
               scopparse ssematch
               
if LOCALLINK
INCLUDES = -I../../../plplot -I../../../ajax -I../../../nucleus
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lnucleus -lajax -leplplot
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lajax -leplplot
endif
else
AM_CFLAGS =
endif
endif

cathparse_SOURCES = cathparse.c
domainnr_SOURCES = domainnr.c
domainreso_SOURCES = domainreso.c
domainseqs_SOURCES = domainseqs.c
domainsse_SOURCES = domainsse.c
scopparse_SOURCES = scopparse.c
ssematch_SOURCES = ssematch.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
