## Process this file with automake to produce Makefile.in


bin_PROGRAMS = myseq

FLAGS=

INCLUDES = -I../../../nucleus -I../../../ajax -I../../../plplot


if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax -lnucleus
else
AM_CFLAGS =
endif
endif


myseq_SOURCES = myseq.c 

LDADD = ../../../ajax/libajax.la ../../../nucleus/libnucleus.la  $(XLIB)



