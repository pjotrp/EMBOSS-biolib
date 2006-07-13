## Process this file with automake to produce Makefile.in

bin_PROGRAMS = emnu

if LOCALLINK
INCLUDES = -I../../../plplot -I../../../nucleus -I../../../ajax
else
INCLUDES = -I${embprefix}/include
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lnucleus -lajax -lplplot
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lajax -lplplot
endif
else
AM_CFLAGS =
endif
endif

emnu_SOURCES = emnu.c 

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la -lmenu -lform $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -lplplot -lmenu -lform $(XLIB)
endif
