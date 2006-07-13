## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = emast ememe
               
if LOCALLINK
INCLUDES = -I../../../plplot -I../../../ajax -I../../../nucleus
else
INCLUDES = -I${embprefix}/include
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lnucleus -lajax -lplplot
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lajax -lplplot
endif
else
AM_CFLAGS =
endif
endif

emast_SOURCES = emast.c
ememe_SOURCES = ememe.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -lplplot $(XLIB)
endif
