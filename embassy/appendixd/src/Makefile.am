## Process this file with automake to produce Makefile.in


bin_PROGRAMS = crystalball

INCLUDES = -I../../../nucleus -I../../../ajax -I../../../plplot


if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif


crystalball_SOURCES = crystalball.c 

LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)



