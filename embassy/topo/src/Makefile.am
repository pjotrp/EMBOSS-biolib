## Process this file with automake to produce Makefile.in


bin_PROGRAMS = topo

INCLUDES = -I../h -I../../../ajax -I../../../plplot


if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS =
endif
endif


topo_SOURCES = topo.c 

noinst_HEADERS = common.h

LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)


