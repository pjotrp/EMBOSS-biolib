## Process this file with automake to produce Makefile.in

bin_PROGRAMS = mse

if LOCALLINK
INCLUDES = -I../h \
	-I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/expat -I../../../ajax/core \
	-I../../../ajax/graphics -I../../../ajax/ensembl \
	-I../../../ajax/ajaxdb -I../../../ajax/acd \
	-I../../../plplot
else
INCLUDES = -I../h -I${embprefix}/include \
	-I${embprefix}/include/eplplot
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

mse_SOURCES = mse.c 

if LOCALLINK
LDADD = ../ckit/libckit.la \
	../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la ../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la ../../../plplot/libeplplot.la \
	$(XLIB)
else
LDADD = ../ckit/libckit.la -L${embprefix}/lib -lnucleus -lacd -lajaxdb \
	-lensembl -lajaxg -lajax -leexpat -lepcre -leplplot $(XLIB)
endif
