## Process this file with automake to produce Makefile.in

if ISCYGWIN
CYGWIN_LDFLAGS = -no-undefined -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--enable-auto-import
if LOCALLINK
CYGWIN_LDVIENNA = -L../../../plplot -L../../../ajax/pcre \
-L../../../ajax/expat -L../../../ajax/zlib \
-L../../../ajax/core -L../../../ajax/graphics -L../../../ajax/ensembl \
-L../../../ajax/ajaxdb -L../../../ajax/acd -L../../../nucleus \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leplplot -leexpat \
-lezlib -lepcre
else
CYGWIN_LDVIENNA = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl \
                -lajaxg -lajax -leplplot -leexpat -lezlib -lepcre
endif
endif

if LOCALLINK
INCLUDES = -I../H -I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/expat -I../../../ajax/zlib \
	-I../../../ajax/core -I../../../ajax/graphics \
	-I../../../ajax/ensembl -I../../../ajax/ajaxdb \
	-I../../../ajax/acd -I../../../plplot
else
INCLUDES = -I../H -I${embprefix}/include -I${embprefix}/include/eplplot
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

lib_LTLIBRARIES = libviennarna.la

bin_PROGRAMS = vrnaalifold vrnacofoldpf vrnaevalpair vrnainverse \
	vrnaalifoldpf vrnadistance vrnafold vrnalfold \
	vrnacofold vrnaduplex vrnafoldpf vrnaplot \
	vrnacofoldconc vrnaeval vrnaheat vrnasubopt

VIENNALIBSRC = dist_vars.c fold_vars.c part_func.c eread_epars.c \
        treedist.c energy_par.c inverse.c ProfileDist.c RNAstruct.c utils.c \
        fold.c naview.c ePS_dot.c stringdist.c params.c subopt.c list.c \
        cofold.c eLfold.c epart_func_co.c part_func_up.c ProfileAln.c \
	duplex.c alifold.c alipfold.c aln_util.c getconstraints.c


noinst_HEADERS = intloops.h list.h

EXTRA_DIST = alicircfold.inc circfold.inc config.h

libviennarna_la_SOURCES = $(VIENNALIBSRC)

VERS_INF = -version-info 1:7:0

if ISCYGWIN
LINKFLAGS = $(VERS_INF) $(CYGWIN_LDFLAGS) $(CYGWIN_LDVIENNA)
else
LINKFLAGS = $(VERS_INF)
endif

libviennarna_la_LDFLAGS = $(LINKFLAGS)

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
LDADD = libviennarna.la \
	../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la \
	../../../ajax/zlib/libezlib.la \
	../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la \
	../../../plplot/libeplplot.la \
	$(XLIB)
else
LDADD = libviennarna.la -L${embprefix}/lib -lnucleus -lacd -lajaxdb \
	-lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot $(XLIB)
endif
