## Process this file with automake to produce Makefile.in


bin_PROGRAMS = topo

FLAGS=-lX11

INCLUDES = -I../h -I../../../ajax -I../../../plplot


if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax -lajaxg
else
AM_CFLAGS =
endif
endif


topo_SOURCES = topo.c 

noinst_HEADERS = common.h

LDADD = ../../../ajax/libajaxg.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)


