## Process this file with automake to produce Makefile.in

SUBDIRS =

bin_PROGRAMS = domainer domainalign domainnr domainrep domainreso domainseqs \
               libgen pdbparse scopparse \
               seqalign seqnr seqsearch seqfraggle seqsort \
               rocplot rocon 

INCLUDES = -I../../../plplot -I../../../ajax -I../../../nucleus

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif

domainer_SOURCES = domainer.c
domainalign_SOURCES = domainalign.c
domainnr_SOURCES = domainnr.c
domainrep_SOURCES = domainrep.c
domainreso_SOURCES = domainreso.c
domainseqs_SOURCES = domainseqs.c
libgen_SOURCES = libgen.c
pdbparse_SOURCES = pdbparse.c
scopparse_SOURCES = scopparse.c
seqalign_SOURCES = seqalign.c
seqnr_SOURCES = seqnr.c
seqsearch_SOURCES = seqsearch.c
seqfraggle_SOURCES = seqfraggle.c
seqsort_SOURCES = seqsort.c
rocplot_SOURCES = rocplot.c
rocon_SOURCES = rocon.c

LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
