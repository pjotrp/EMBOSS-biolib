## Makefile.am for Jemboss
## Process this file with automake to produce Makefile.in in all directories.
AUTOMAKE_OPTIONS = gnits

SUBDIRS = api embreo images lib org resources utils

# files with nonstandard names in this directory
EXTRA_DIST = LICENSE

all-local:
	@if test "${JAVA_OK}" != "yes" ; then \
	  echo "Not compiling Jemboss"; \
	else \
	  declare -x CLASSPATH=":lib/soap.jar:lib/xerces.jar:lib/mail.jar:lib/activation.jar:lib/jakarta-regexp-1.2.jar:embreo/embreo-full.jar:.:"; \
	  export CLASSPATH; \
	  javac org/emboss/jemboss/Jemboss.java; \
	fi

pkgdata_DATA = LICENSE interface.html readme.html runJemboss.csh setup.html

pkgdatadir=$(prefix)/share/$(PACKAGE)/jemboss



# tar to pick up the other directories
# then remove any CVS subdirectories

dist-hook:
	tar cBf - api | ( cd $(distdir); tar xBf - ; rm -rf api/CVS ) 
