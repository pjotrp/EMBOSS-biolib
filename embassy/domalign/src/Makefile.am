## Process this file with automake to produce Makefile.in

if LOCALLINK
INCLUDES = -I../include -I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/expat -I../../../ajax/zlib \
	-I../../../ajax/core -I../../../ajax/graphics \
	-I../../../ajax/ensembl -I../../../ajax/ajaxdb \
	-I../../../ajax/acd -I../../../plplot
else
INCLUDES = -I../include -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs \
-L../../../ajax/pcre/.libs -L../../../ajax/expat/.libs \
-L../../../ajax/zlib/.libs -L../../../ajax/core/.libs \
-L../../../ajax/graphics/.libs -L../../../ajax/ensembl/.libs \
-L../../../ajax/ajaxdb/.libs -L../../../ajax/acd/.libs \
-L../../../nucleus/.libs \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre \
-lezlib -leplplot
else
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot
endif
endif
endif

AM_CFLAGS =  $(AIX_CFLAGS) $(WARN_CFLAGS) $(DEVWARN_CFLAGS)

bin_PROGRAMS = allversusall domainalign domainrep \
               seqalign 

allversusall_SOURCES = allversusall.c 
domainalign_SOURCES = domainalign.c  
domainrep_SOURCES = domainrep.c  
seqalign_SOURCES = seqalign.c  

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la \
	../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la \
	../../../ajax/zlib/libezlib.la \
	../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl -lajaxg \
        -lajax -leexpat -lepcre -lezlib -leplplot $(XLIB)
endif
