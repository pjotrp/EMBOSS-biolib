## Process this file with automake to produce Makefile.in

EXTRA_DIST = Makefile.am

## uncomment these lines and add the names of your binaries here
## uncomment the _SOURCES lines below to define the source file(s)
##     for each binary
## this is all you need to do to compile and install your own applications
## see the emboss_doc/master directory for adding application documentation

##bin_PROGRAMS = myprog \
##          myotherprog

if LOCALLINK
INCLUDES = -I../include -I../../../nucleus -I../../../ajax -I../../../plplot
else
INCLUDES = -I../include -I${embprefix}/include -I${embprefix}/include/eplplot
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


#myprog_SOURCES = myprog.c
#myotherprog_SOURCES = myotherprog.c



if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajaxg.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajaxg -lajax -leplplot $(XLIB)
endif
