## Process this file with automake to produce Makefile.in


bin_PROGRAMS = eclique econsense econtml econtrast ednacomp ednadist ednainvar \
ednaml ednamlk ednapars ednapenny edollop edolpenny efactor efitch egendist \
ekitsch emix epenny eprotdist eprotpars erestml eseqboot eneighbor

INCLUDES = -I../include -I../../../ajax


if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS =
endif
endif


eclique_SOURCES = clique.c
econsense_SOURCES = consense.c
econtml_SOURCES = contml.c
econtrast_SOURCES = contrast.c
ednacomp_SOURCES = dnacomp.c
ednadist_SOURCES = dnadist.c 
ednainvar_SOURCES = dnainvar.c
ednaml_SOURCES = dnaml.c dnaml2.c
ednamlk_SOURCES = dnamlk.c dnamlk2.c
ednapars_SOURCES = dnapars.c
ednapenny_SOURCES = dnapenny.c
edollop_SOURCES = dollop.c
edolpenny_SOURCES = dolpenny.c
efactor_SOURCES = factor.c
efitch_SOURCES = fitch.c
egendist_SOURCES = gendist.c
ekitsch_SOURCES = kitsch.c
emix_SOURCES = mix.c mix2.c
epenny_SOURCES = penny.c
eprotdist_SOURCES = protdist.c
eprotpars_SOURCES = protpars.c
erestml_SOURCES = restml.c restml2.c
eseqboot_SOURCES = seqboot.c
eneighbor_SOURCES = neighbor.c


LDADD = ../../../ajax/libajax.la 


