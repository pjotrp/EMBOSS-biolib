## Process this file with automake to produce Makefile.in

bin_PROGRAMS = libgen matgen3d \
               rocplot rocon \
               siggen siggenlig sigscan sigscanlig 

if LOCALLINK
INCLUDES = -I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/expat -I../../../ajax/core \
	-I../../../ajax/graphics -I../../../ajax/ensembl \
	-I../../../ajax/ajaxdb -I../../../ajax/acd \
	-I../../../plplot
else
INCLUDES = -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs \
-L../../../ajax/pcre/.libs -L../../../ajax/expat/.libs \
-L../../../ajax/core/.libs -L../../../ajax/graphics/.libs \
-L../../../ajax/ensembl/.libs -L../../../ajax/ajaxdb/.libs \
-L../../../ajax/acd/.libs -L../../../nucleus/.libs \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -leplplot
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -leplplot
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
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la ../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la ../../../plplot/libeplplot.la \
	$(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl -lajaxg \
        -lajax -leexpat -lepcre -leplplot $(XLIB)
endif
