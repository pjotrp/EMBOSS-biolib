## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libhmmer.la

bin_PROGRAMS = ohmmcalibrate ohmmconvert ohmmemit ohmmfetch ohmmindex \
	ohmmbuild ohmmalign ohmmpfam ohmmsearch oalistat

if LOCALLINK
INCLUDES = -IINCLUDE -I../../../ajax
else
INCLUDES = -IINCLUDE -I${embprefix}/include -I${embprefix}/include/eplplot
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lajax
endif
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

ohmmcalibrate_SOURCES = ohmmcalibrate.c
ohmmconvert_SOURCES   = ohmmconvert.c
ohmmemit_SOURCES      = ohmmemit.c
ohmmfetch_SOURCES     = ohmmfetch.c
ohmmindex_SOURCES     = ohmmindex.c
ohmmbuild_SOURCES     = ohmmbuild.c
ohmmalign_SOURCES     = ohmmalign.c
ohmmpfam_SOURCES      = ohmmpfam.c
ohmmsearch_SOURCES    = ohmmsearch.c
oalistat_SOURCES      = oalistat.c

if LOCALLINK
LDADD = libhmmer.la ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = libhmmer.la -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
