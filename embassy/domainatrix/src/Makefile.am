## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = allversusall cathparse contacts \
               domainer domainalign domainnr domainrep domainreso domainseqs domainsse \
               hetparse interface libgen \
               pdbtosp pdbparse pdbplus \
               rocplot rocon \
               scopparse seqalign seqnr seqsearch seqfraggle seqsort seqwords \
               sigscan siggen sites ssematch
               
if LOCALLINK
INCLUDES = -I../../../plplot -I../../../ajax -I../../../nucleus
else
INCLUDES = -I${embprefix}/include
endif

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif

allversusall_SOURCES = allversusall.c
cathparse_SOURCES = cathparse.c
contacts_SOURCES = contacts.c
domainer_SOURCES = domainer.c
domainalign_SOURCES = domainalign.c
domainnr_SOURCES = domainnr.c
domainrep_SOURCES = domainrep.c
domainreso_SOURCES = domainreso.c
domainseqs_SOURCES = domainseqs.c
domainsse_SOURCES = domainsse.c
hetparse_SOURCES = hetparse.c
interface_SOURCES = interface.c
libgen_SOURCES = libgen.c
pdbtosp_SOURCES = pdbtosp.c
pdbparse_SOURCES = pdbparse.c
pdbplus_SOURCES = pdbplus.c
rocplot_SOURCES = rocplot.c
rocon_SOURCES = rocon.c
scopparse_SOURCES = scopparse.c
seqalign_SOURCES = seqalign.c
seqnr_SOURCES = seqnr.c
seqsearch_SOURCES = seqsearch.c
seqfraggle_SOURCES = seqfraggle.c
seqsort_SOURCES = seqsort.c
seqwords_SOURCES = seqwords.c
siggen_SOURCES = siggen.c
sigscan_SOURCES = sigscan.c
sites_SOURCES = sites.c
ssematch_SOURCES = ssematch.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -lplplot $(XLIB)
endif
