## Process this file with automake to produce Makefile.in


bin_PROGRAMS = crystalball

if LOCALLINK
INCLUDES = -I../../../nucleus -I../../../ajax -I../../../plplot
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif


crystalball_SOURCES = crystalball.c 

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif


