## Process this file with automake to produce Makefile.in


bin_PROGRAMS = mse

FLAGS=-lX11

INCLUDES = -I../h -I../../../ajax

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS =
endif
endif

mse_SOURCES = mse.c 

LDADD = ../ckit/libckit.la ../../../ajax/libajax.la 


