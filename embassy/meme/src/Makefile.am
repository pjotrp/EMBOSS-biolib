## Process this file with automake to produce Makefile.in

SUBDIRS = INCLUDE

lib_LTLIBRARIES = libmeme.la

bin_PROGRAMS = meme

INCLUDES = -IINCLUDE -I../../../ajax

if ISSHARED
if ISAIXIA64
AM_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../ajax/.libs -lajax
else
AM_CFLAGS =
endif
endif

MEMESRC = clock.c display.c em.c init.c meme_util.c seq2theta.c \
	shorten.c starts.c prior.c split.c align.c justlike.c \
	oops.c tcm.c zoops.c banner.c logs.c palindrome.c gamma.c \
	likelihood.c subseq7.c normalize.c chi.c mp.c message.c \
	diagram.c logodds.c regress.c \
	hash.c hash_alph.c read_seq_file.c

libmeme_la_SOURCES = $(MEMESRC)

libmeme_la_LDFLAGS = -version-info 2:3:1

meme_SOURCES = meme.c

LDADD = libmeme.la ../../../ajax/libajax.la
