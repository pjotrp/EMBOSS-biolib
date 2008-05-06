## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = eiprscan

if LOCALLINK
INCLUDES = -I../../../ajax -I../../../nucleus -I../../../plplot
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
endif
else
AM_CFLAGS =
endif
endif

eiprscan_SOURCES = eiprscan.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
