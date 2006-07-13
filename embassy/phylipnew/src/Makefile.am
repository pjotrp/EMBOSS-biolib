## Process this file with automake to produce Makefile.in

## To add programs
## Add the program binary name to bin_PROGRAMS
## (using \ as a continuation character for multiple lines)
##
## And add a programname_SOURCES line to define the source files
## to be compiled and linked
##
## make will compile and link the program
## make install will copy the program to the install directory

bin_PROGRAMS = fclique fconsense fcontml fcontrast \
fdnacomp fdnadist fdnainvar fdnaml fdnamlk fdnamove fdnapars fdnapenny \
fdolmove fdollop fdolpenny fdrawgram fdrawtree \
ffactor ffitch fgendist fkitsch fmix fmove fneighbor fpars \
fpenny fproml fpromlk fprotdist fprotpars \
frestdist frestml fretree \
fdiscboot ffreqboot frestboot fseqboot fseqbootall \
ftreedist ftreedistpair


if LOCALLINK
INCLUDES = -I../include -I../../../ajax -I../../../nucleus -I../../../plplot
else
INCLUDES = -I../include -I${embprefix}/include
endif


if ISSHARED
if ISAIXIA64
if LOCALLINK
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs -L../../../ajax/.libs -L../../../nucleus/.libs -lnucleus -lajax -lplplot
else
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lajax -lplplot
endif
else
AM_CFLAGS =
endif
endif


fclique_SOURCES = clique.c disc.c phylip.c
fconsense_SOURCES = consense.c cons.c phylip.c
fcontml_SOURCES = contml.c cont.c phylip.c
fcontrast_SOURCES = contrast.c cont.c phylip.c

fdnacomp_SOURCES = dnacomp.c seq.c phylip.c
fdnadist_SOURCES = dnadist.c seq.c phylip.c
fdnainvar_SOURCES = dnainvar.c seq.c phylip.c
fdnaml_SOURCES = dnaml.c seq.c phylip.c
fdnamlk_SOURCES = dnamlk.c  seq.c phylip.c
fdnamove_SOURCES = dnamove.c moves.c seq.c phylip.c
fdnapenny_SOURCES = dnapenny.c seq.c phylip.c
fdnapars_SOURCES = dnapars.c seq.c phylip.c

fdolmove_SOURCES = dolmove.c disc.c moves.c dollo.c phylip.c
fdollop_SOURCES = dollop.c disc.c dollo.c phylip.c
fdolpenny_SOURCES = dolpenny.c disc.c dollo.c phylip.c

fdrawgram_SOURCES = drawgram.c draw.c draw2.c phylip.c
fdrawtree_SOURCES = drawtree.c draw.c draw2.c phylip.c

ffactor_SOURCES = factor.c phylip.c
ffitch_SOURCES = fitch.c dist.c phylip.c
fgendist_SOURCES = gendist.c phylip.c
fkitsch_SOURCES = kitsch.c dist.c phylip.c
fmix_SOURCES = mix.c disc.c wagner.c phylip.c
fmove_SOURCES = move.c disc.c moves.c wagner.c phylip.c
fneighbor_SOURCES = neighbor.c dist.c phylip.c
fpars_SOURCES = pars.c discrete.c phylip.c
fpenny_SOURCES = penny.c disc.c wagner.c phylip.c

fproml_SOURCES = proml.c seq.c phylip.c
fpromlk_SOURCES = promlk.c seq.c phylip.c
fprotdist_SOURCES = protdist.c seq.c phylip.c
fprotpars_SOURCES = protpars.c seq.c phylip.c

frestdist_SOURCES = restdist.c seq.c phylip.c
frestml_SOURCES = restml.c seq.c phylip.c
fretree_SOURCES = retree.c moves.c phylip.c
ftreedist_SOURCES = treedist.c cons.c phylip.c
ftreedistpair_SOURCES = treedistpair.c cons.c phylip.c

fdiscboot_SOURCES = discboot.c seq.c phylip.c
ffreqboot_SOURCES = freqboot.c seq.c phylip.c
frestboot_SOURCES = restboot.c seq.c phylip.c
fseqboot_SOURCES = seqboot.c seq.c phylip.c
fseqbootall_SOURCES = seqbootall.c seq.c phylip.c


if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/libajax.la ../../../plplot/libplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lajax -lplplot $(XLIB)
endif
