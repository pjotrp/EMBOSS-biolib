## Process this file with automake to produce Makefile.in

if !ESYSTEMLIBS
LLINCLUDES = -I../../../ajax/expat -I../../../ajax/zlib
NLINCLUDES = -I${embprefix}/include/ezlib -I${embprefix}/include/eexpat
LLAIXLIBS  = -L../../../ajax/expat/.libs -L../../../ajax/zlib/.libs
NLAIXLIBS  = -leexpat -lezlib
LLADD = ../../../ajax/expat/libeexpat.la ../../../ajax/zlib/libezlib.la
NLADD = -leexpat -lezlib
endif


if LOCALLINK
AM_CPPFLAGS = -I../../../nucleus -I../../../ajax/pcre \
	$(LLINCLUDES) \
	-I../../../ajax/core -I../../../ajax/graphics \
	-I../../../ajax/ensembl -I../../../ajax/ajaxdb \
	-I../../../ajax/acd -I../../../plplot
else
AM_CPPFLAGS = -I${embprefix}/include -I${embprefix}/include/eplplot \
	$(NLINCLUDES) \
	-I${embprefix}/include/epcre
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs \
-L../../../ajax/pcre/.libs $(LLAIXLIBS) \
-L../../../ajax/core/.libs \
-L../../../ajax/graphics/.libs -L../../../ajax/ensembl/.libs \
-L../../../ajax/ajaxdb/.libs -L../../../ajax/acd/.libs \
-L../../../nucleus/.libs \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -lepcre \
 $(NLAIXLIBS) -leplplot
else
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -lepcre $(NLAIXLIBS) -leplplot
endif
endif
endif

AM_CFLAGS =  $(AIX_CFLAGS) $(WARN_CFLAGS) $(DEVWARN_CFLAGS)

bin_PROGRAMS = libgen matgen3d \
               rocplot rocon \
               siggen siggenlig sigscan sigscanlig 

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
	../../../ajax/core/libajax.la \
	$(LLADD) \
	../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la \
	../../../plplot/libeplplot.la \
	$(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl -lajaxg \
        -lajax -lepcre $(NLADD) -leplplot $(XLIB)
endif
