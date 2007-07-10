## Process this file with automake to produce Makefile.in


bin_PROGRAMS = topo

if LOCALLINK
INCLUDES = -I../h -I../../../ajax -I../../../plplot -I../../../nucleus
else
INCLUDES = -I../h -I${embprefix}/include -I${embprefix}/include/eplplot
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


topo_SOURCES = topo.c 

noinst_HEADERS = common.h

if LOCALLINK

if ISCYGWIN
LDADD = ../../../nucleus/libnucleus.la  ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajaxg.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
endif

else
if ISCYGWIN
LDADD = -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajaxg -lajax -leplplot $(XLIB)
endif

endif
