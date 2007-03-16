## Process this file with automake to produce Makefile.in


bin_PROGRAMS = mse

if LOCALLINK
INCLUDES = -I../h -I../../../ajax -I../../../nucleus -I../../../plplot
else
INCLUDES = -I../h -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -L../../../nucleus/.libs -L../../../plplot/.libs -leplplot -lajax -lnucleus
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -leplplot -lajax -lnucleus
endif
else
AM_CFLAGS =
endif
endif

mse_SOURCES = mse.c 

if LOCALLINK
LDADD = ../ckit/libckit.la ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = ../ckit/libckit.la -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
