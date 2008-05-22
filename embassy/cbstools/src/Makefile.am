## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = esignalp etmhmm enetnglyc enetoglyc enetphos eprop \
	eyinoyang echlorop elipop

if LOCALLINK
INCLUDES = -I../../../ajax -I../../../nucleus -I../../../plplot
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
endif
else
AM_CFLAGS =
endif
endif

esignalp_SOURCES = esignalp.c
etmhmm_SOURCES = etmhmm.c
enetnglyc_SOURCES = enetnglyc.c
enetoglyc_SOURCES = enetoglyc.c
enetphos_SOURCES = enetphos.c
eprop_SOURCES = eprop.c
eyinoyang_SOURCES = eyinoyang.c
echlorop_SOURCES = echlorop.c
elipop_SOURCES = elipop.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
