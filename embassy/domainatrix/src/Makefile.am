## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = contacts domainer fraggle hetparse hmmgen interface libscan \
               pdbparse pdbtosp profgen scopalign scopnr scopparse \
               scoprep scopreso scopseqs seqalign seqnr seqsearch seqsort \
               seqwords siggen sigplot sigscan

INCLUDES = -I../../../plplot -I../../../ajax -I../../../nucleus

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif

contacts_SOURCES = contacts.c
domainer_SOURCES = domainer.c
fraggle_SOURCES = fraggle.c
hetparse_SOURCES = hetparse.c
hmmgen_SOURCES = hmmgen.c
interface_SOURCES = interface.c
libscan_SOURCES = libscan.c
pdbparse_SOURCES = pdbparse.c
pdbtosp_SOURCES = pdbtosp.c
profgen_SOURCES = profgen.c
scopalign_SOURCES = scopalign.c
scopnr_SOURCES = scopnr.c
scopparse_SOURCES = scopparse.c
scoprep_SOURCES = scoprep.c
scopreso_SOURCES = scopreso.c
scopseqs_SOURCES = scopseqs.c
seqalign_SOURCES = seqalign.c
seqnr_SOURCES = seqnr.c
seqsearch_SOURCES = seqsearch.c
seqsort_SOURCES = seqsort.c
seqwords_SOURCES = seqwords.c
siggen_SOURCES = siggen.c
sigplot_SOURCES = sigplot.c
sigscan_SOURCES = sigscan.c

LDADD = ../../../plplot/libplplot.la ../../../ajax/libajaxg.la ../../../ajax/libajax.la ../../../nucleus/libnucleus.la $(XLIB)
