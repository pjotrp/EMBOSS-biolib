## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libhmmer.la

bin_PROGRAMS = ehmmcalibrate ehmmconvert ehmmemit ehmmfetch ehmmindex \
	ehmmbuild ehmmalign ehmmpfam ehmmsearch ealistat

INCLUDES = -IINCLUDE -I../../../ajax

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS =
endif
endif


HMMERSRC = alphabet.c core_algorithms.c debug.c emit.c emulation.c \
	   histogram.c hmmio.c mathsupport.c masks.c misc.c \
	   modelmakers.c plan7.c plan9.c prior.c threads.c tophits.c \
	   trace.c aligneval.c alignio.c cluster.c dayhoff.c file.c \
	   getopt.c gsi.c hsregex.c interleaved.c iupac.c msf.c \
	   revcomp.c selex.c sqerror.c sqio.c sre_ctype.c sre_math.c \
	   sre_string.c stack.c translate.c types.c weight.c

noinst_HEADERS = config.h funcs.h globals.h gnuregex.h sqfuncs.h \
	squid.h structs.h version.h

libhmmer_la_SOURCES = $(HMMERSRC)

libhmmer_la_LDFLAGS = -version-info 2:1:1

ehmmcalibrate_SOURCES = ehmmcalibrate.c
ehmmconvert_SOURCES   = ehmmconvert.c
ehmmemit_SOURCES      = ehmmemit.c
ehmmfetch_SOURCES     = ehmmfetch.c
ehmmindex_SOURCES     = ehmmindex.c
ehmmbuild_SOURCES     = ehmmbuild.c
ehmmalign_SOURCES     = ehmmalign.c
ehmmpfam_SOURCES      = ehmmpfam.c
ehmmsearch_SOURCES    = ehmmsearch.c
ealistat_SOURCES      = ealistat.c

LDADD = libhmmer.la ../../../ajax/libajax.la
