## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libepcre.la


if ISCYGWIN
CYGWIN_LDFLAGS = -no-undefined -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--enable-auto-import
endif

## PCRE stuff

POSIX_MALLOC_THRESHOLD = @POSIX_MALLOC_THRESHOLD@
PCRELIBVERSION = @PCRE_LIB_VERSION@
PCREPOSIXLIBVERSION = @PCRE_POSIXLIB_VERSION@

INCLUDES = $(POSIX_MALLOC_THRESHOLD) 

PCRESRC = pcre.c pcre_study.c pcreposix.c

include_HEADERS = pcre.h pcre_chartables.c pcre_config.h pcre_get.c \
pcre_printint.c pcre_internal.h pcreposix.h

libepcre_la_SOURCES = $(PCRESRC)

VERS_INF = -version-info 4:3:0

if ISCYGWIN
LINKFLAGS = $(VERS_INF) $(CYGWIN_LDFLAGS)
else
LINKFLAGS = $(VERS_INF)
endif

libepcre_la_LDFLAGS = $(LINKFLAGS)
