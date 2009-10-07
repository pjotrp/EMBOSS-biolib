## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libacd.la


if ISCYGWIN
CYGWIN_LDFLAGS = -no-undefined -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--enable-auto-import
CYGWIN_LDACD = -L../../plplot -L../pcre -L../core -L../graphics -L../ensembl \
-L../ajaxseq -lajaxseq -lensembl -lajaxg -lajax -leplplot -lepcre
endif

INCLUDES = -I$(top_srcdir)/plplot -I$(top_srcdir)/ajax/pcre \
-I$(top_srcdir)/ajax/core \
-I$(top_srcdir)/ajax/graphics \
-I$(top_srcdir)/ajax/ensembl \
-I$(top_srcdir)/ajax/ajaxseq

ACDSRC = ajacd.c

# Uncomment and amend when there are some real headers
include_HEADERS = ajacd.h

libacd_la_SOURCES = $(ACDSRC)

VERS_INF = -version-info 6:1:0

if ISCYGWIN
LINKFLAGS = $(VERS_INF) $(CYGWIN_LDFLAGS) $(CYGWIN_LDACD)
else
LINKFLAGS = $(VERS_INF)
endif

libacd_la_LDFLAGS = $(LINKFLAGS)
