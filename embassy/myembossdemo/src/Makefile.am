## Process this file with automake to produce Makefile.in

EXTRA_DIST = Makefile.am

## to install the applications, change the next line
## from check_PROGRAMS to bin_PROGRAMS and run "make install"

check_PROGRAMS = \
demoalign demofeatures demolist demoreport \
demosequence demostring demostringnew demotable \
myseq mytest

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


demoalign_SOURCES = demoalign.c
demofeatures_SOURCES = demofeatures.c
demolist_SOURCES = demolist.c
demoreport_SOURCES = demoreport.c
demosequence_SOURCES = demosequence.c
demostring_SOURCES = demostring.c
demostringnew_SOURCES = demostringnew.c
demotable_SOURCES = demotable.c
myseq_SOURCES = myseq.c
mytest_SOURCES = mytest.c



if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajaxg.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajaxg -lajax -leplplot $(XLIB)
endif
