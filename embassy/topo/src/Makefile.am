## Process this file with automake to produce Makefile.in


bin_PROGRAMS = topo

if LOCALLINK
INCLUDES = -I../h -I../../../ajax -I../../../plplot
else
INCLUDES = -I../h -I${embprefix}/include
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


topo_SOURCES = topo.c 

noinst_HEADERS = common.h

if LOCALLINK

if ISCYGWIN
LDADD = ../../../nucleus/libnucleus.la  ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
else
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajaxg.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
endif

else
if ISCYGWIN
LDADD = -L${embprefix}/lib -lnucleus -lajax -lplplot $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajaxg -lajax -lplplot $(XLIB)
endif

endif
