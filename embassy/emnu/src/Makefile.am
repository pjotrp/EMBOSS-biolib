## Process this file with automake to produce Makefile.in

bin_PROGRAMS = emnu

FLAGS=

INCLUDES = -I../../../plplot -I../../../nucleus -I../../../ajax

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax -lajaxg
else
AM_CFLAGS =
endif
endif

emnu_SOURCES = emnu.c 

LDADD = ../../../plplot/libplplot.la ../../../ajax/libajax.la ../../../nucleus/libnucleus.la -lmenu -lform $(XLIB)




