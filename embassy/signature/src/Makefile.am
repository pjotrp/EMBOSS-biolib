## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = libgen matgen3d \
               rocplot rocon \
               siggen siggenlig sigscan sigscanlig 
               
if LOCALLINK
INCLUDES = -I../../../plplot -I../../../ajax -I../../../nucleus
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
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

libgen_SOURCES = libgen.c
matgen3d_SOURCES = matgen3d.c
rocplot_SOURCES = rocplot.c
rocon_SOURCES = rocon.c
siggen_SOURCES = siggen.c
siggenlig_SOURCES = siggenlig.c
sigscan_SOURCES = sigscan.c
sigscanlig_SOURCES = sigscanlig.c


if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
