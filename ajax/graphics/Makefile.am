## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libajaxg.la


if ISCYGWIN
CYGWIN_LDFLAGS = -no-undefined -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--enable-auto-import
CYGWIN_LDAJAXG = -L../../plplot -L../pcre -L../core -lajax -leplplot -lepcre
endif

INCLUDES = -I$(top_srcdir)/plplot -I$(top_srcdir)/ajax/pcre \
-I$(top_srcdir)/ajax/core

AJAXGSRC = ajgraph.c ajgraphxml.c ajhist.c

include_HEADERS = ajgraph.h ajgraphstruct.h ajgraphxml.h ajhist.h

libajaxg_la_SOURCES = $(AJAXGSRC)

VERS_INF = -version-info 6:1:0

if ISCYGWIN
LINKFLAGS = $(VERS_INF) $(CYGWIN_LDFLAGS) $(CYGWIN_LDAJAXG)
else
LINKFLAGS = $(VERS_INF)
endif

libajaxg_la_LDFLAGS = $(LINKFLAGS)
