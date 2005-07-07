## Process this file with automake to produce Makefile.in

EXTRA_DIST = Makefile.am

bin_PROGRAMS = myseq mytest

if LOCALLINK
INCLUDES = -I../include -I../../../nucleus -I../../../ajax -I../../../plplot
else
INCLUDES = -I../include -I${embprefix}/include
endif


if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lajax
endif
else
AM_CFLAGS =
endif
endif


myseq_SOURCES = myseq.c
mytest_SOURCES = mytest.c



if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -lplplot $(XLIB)
endif
