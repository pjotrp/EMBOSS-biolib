## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libviennarna.la

bin_PROGRAMS = vrnaalifold vrnacofoldpf vrnaevalpair vrnainverse \
	vrnaalifoldpf vrnadistance vrnafold vrnalfold \
	vrnacofold vrnaduplex vrnafoldpf vrnaplot \
	vrnacofoldconc vrnaeval vrnaheat vrnasubopt


if LOCALLINK
INCLUDES = -I../H -I../../../ajax -I../../../nucleus -I../../../plplot
else
INCLUDES = -I../H -I${embprefix}/include -I${embprefix}/include/eplplot
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


VIENNALIBSRC = dist_vars.c fold_vars.c part_func.c eread_epars.c \
        treedist.c energy_par.c inverse.c ProfileDist.c RNAstruct.c utils.c \
        fold.c naview.c ePS_dot.c stringdist.c params.c subopt.c list.c \
        cofold.c eLfold.c epart_func_co.c part_func_up.c ProfileAln.c \
	duplex.c alifold.c alipfold.c aln_util.c getconstraints.c


noinst_HEADERS = intloops.h list.h

EXTRA_DIST = alicircfold.inc circfold.inc config.h

libviennarna_la_SOURCES = $(VIENNALIBSRC)

libviennarna_la_LDFLAGS = -version-info 1:6:1

vrnaalifold_SOURCES = vrnaalifold.c
vrnaalifoldpf_SOURCES = vrnaalifoldpf.c
vrnacofold_SOURCES = vrnacofold.c
vrnacofoldconc_SOURCES = vrnacofoldconc.c
vrnacofoldpf_SOURCES = vrnacofoldpf.c
vrnadistance_SOURCES = vrnadistance.c
vrnaduplex_SOURCES = vrnaduplex.c
vrnaeval_SOURCES = vrnaeval.c
vrnaevalpair_SOURCES = vrnaevalpair.c
vrnafold_SOURCES = vrnafold.c
vrnafoldpf_SOURCES = vrnafoldpf.c
vrnaheat_SOURCES = vrnaheat.c
vrnainverse_SOURCES = vrnainverse.c
vrnalfold_SOURCES = vrnalfold.c
vrnaplot_SOURCES = vrnaplot.c
vrnasubopt_SOURCES = vrnasubopt.c

if LOCALLINK
LDADD = libviennarna.la ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libeplplot.la $(XLIB)
else
LDADD = libviennarna.la -L${embprefix}/lib -lnucleus -lajax -leplplot $(XLIB)
endif
