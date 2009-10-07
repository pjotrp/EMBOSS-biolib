## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libajaxseq.la


if ISCYGWIN
CYGWIN_LDFLAGS = -no-undefined -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--enable-auto-import
CYGWIN_LDAJAXSEQ = -L../../plplot -L../pcre -L../core -L../ensembl \
-lensembl lajax -leplplot -lepcre
endif

INCLUDES = -I$(top_srcdir)/plplot -I$(top_srcdir)/ajax/pcre \
-I$(top_srcdir)/ajax/core -I$(top_srcdir)/ajax/ensembl

AJAXSEQSRC = ajseqdb.c

# Uncomment and amend when there are some real headers
include_HEADERS = ajseqdb.h

libajaxseq_la_SOURCES = $(AJAXSEQSRC)

VERS_INF = -version-info 6:1:0

if ISCYGWIN
LINKFLAGS = $(VERS_INF) $(CYGWIN_LDFLAGS) $(CYGWIN_LDAJAXSEQ)
else
LINKFLAGS = $(VERS_INF)
endif

libajaxseq_la_LDFLAGS = $(LINKFLAGS)
