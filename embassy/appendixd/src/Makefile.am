## Process this file with automake to produce Makefile.in


bin_PROGRAMS = crystalball

INCLUDES = -I../../../plplot -I../../../nucleus -I../../../ajax


if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif


crystalball_SOURCES = crystalball.c 

LDADD = ../../../plplot/libplplot.la ../../../ajax/libajax.la ../../../nucleus/libnucleus.la


