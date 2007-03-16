## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = contacts domainer hetparse interface pdbparse \
               pdbplus pdbtosp sites 
               
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

contacts_SOURCES = contacts.c
domainer_SOURCES = domainer.c
hetparse_SOURCES = hetparse.c
interface_SOURCES = interface.c
pdbparse_SOURCES = pdbparse.c
pdbplus_SOURCES = pdbplus.c
pdbtosp_SOURCES = pdbtosp.c
sites_SOURCES = sites.c


if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
