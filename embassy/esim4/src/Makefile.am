## Process this file with automake to produce Makefile.in
## MATHOG, it needs plplot includes but not link as no graphics output

bin_PROGRAMS = esim4

FLAGS=

INCLUDES = -I../../../nucleus -I../../../ajax -I../../../plplot

noinst_HEADERS = align.h args.h charvec.h discrim.h dna.h encoding.h \
	libc.h misc.h poly.h prnt.h psublast.h seq.h sim4b1.h \
	sim4.h splice.h types.h Xtend1.h

esim4_SOURCES = align.c charvec.c dna.c misc.c prnt.c seq_read.c sim4b1.c Xtend1.c args.c discrim.c encoding.c poly.c seq.c sim4.init.c splice.c

LDADD = ../../../ajax/libajax.la ../../../nucleus/libnucleus.la  $(XLIB)




