## Process this file with automake to produce Makefile.in


bin_PROGRAMS = topo

FLAGS=-lX11

INCLUDES = -I../h -I../../../ajax -I../../../plplot

topo_SOURCES = topo.c 

noinst_HEADERS = common.h

LDADD = ../../../ajax/libajaxg.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)


