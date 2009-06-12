## Makefile.am for Jemboss
## Process this file with automake to produce Makefile.in in all directories.
AUTOMAKE_OPTIONS = gnits

SUBDIRS = api images lib org resources utils

# files with nonstandard names in this directory
EXTRA_DIST = LICENSE

all-local:
if ISCYGWIN
	@if test "${JAVA_OK}" != "yes" ; then \
	  echo "Not compiling Jemboss"; \
	else \
	  (if test "${SHELL}" = "/bin/bash" ; then \
	  declare -x CLASSPATH=`cygpath -wp lib/JembossPrintAlignment.jar:lib/mail.jar:lib/activation.jar:lib/jakarta-regexp-1.2.jar:lib/jalview.jar:lib/jcert.jar:lib/jnet.jar:lib/jsse.jar:.:lib/axis/axis-ant.jar:lib/axis/axis.jar:lib/axis/commons-discovery.jar:lib/axis/commons-logging.jar:lib/axis/jaxrpc.jar:lib/axis/log4j-1.2.4.jar:lib/axis/saaj.jar:lib/axis/wsdl4j.jar:lib/grout.jar:$(srcdir)`; \
	  export CLASSPATH; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java org/emboss/jemboss/draw/PrintDNAImage.java_o; \
          javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/draw/DNADraw.java; \
	  javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/Jemboss.java; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o ./org/emboss/jemboss/editor/PrintAlignmentImage.java; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java_o org/emboss/jemboss/draw/PrintDNAImage.java; \
	  else \
	  CLASSPATH="lib/JembossPrintAlignment.jar:lib/mail.jar:lib/activation.jar:lib/jakarta-regexp-1.2.jar:lib/jalview.jar:lib/jcert.jar:lib/jnet.jar:lib/jsse.jar:.:lib/axis/axis-ant.jar:lib/axis/axis.jar:lib/axis/commons-discovery.jar:lib/axis/commons-logging.jar:lib/axis/jaxrpc.jar:lib/axis/log4j-1.2.4.jar:lib/axis/saaj.jar:lib/axis/wsdl4j.jar:lib/grout.jar:$(srcdir)"; \
	  export CLASSPATH; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java org/emboss/jemboss/draw/PrintDNAImage.java_o; \
          javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/draw/DNADraw.java; \
	  javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/Jemboss.java; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o ./org/emboss/jemboss/editor/PrintAlignmentImage.java; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java_o org/emboss/jemboss/draw/PrintDNAImage.java; \
	  fi); \
	fi
else
	@if test "${JAVA_OK}" != "yes" ; then \
	  echo "Not compiling Jemboss"; \
	else \
	  (if test "${SHELL}" = "/bin/bash" ; then \
	  declare -x CLASSPATH=lib/JembossPrintAlignment.jar:lib/mail.jar:lib/activation.jar:lib/jakarta-regexp-1.2.jar:lib/jalview.jar:lib/jcert.jar:lib/jnet.jar:lib/jsse.jar:.:lib/axis/axis-ant.jar:lib/axis/axis.jar:lib/axis/commons-discovery.jar:lib/axis/commons-logging.jar:lib/axis/jaxrpc.jar:lib/axis/log4j-1.2.4.jar:lib/axis/saaj.jar:lib/axis/wsdl4j.jar:lib/grout.jar:$(srcdir); \
	  export CLASSPATH; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java org/emboss/jemboss/draw/PrintDNAImage.java_o; \
          javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/draw/DNADraw.java; \
	  javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/Jemboss.java; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o ./org/emboss/jemboss/editor/PrintAlignmentImage.java; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java_o org/emboss/jemboss/draw/PrintDNAImage.java; \
	  else \
	  CLASSPATH="lib/JembossPrintAlignment.jar:lib/mail.jar:lib/activation.jar:lib/jakarta-regexp-1.2.jar:lib/jalview.jar:lib/jcert.jar:lib/jnet.jar:lib/jsse.jar:.:lib/axis/axis-ant.jar:lib/axis/axis.jar:lib/axis/commons-discovery.jar:lib/axis/commons-logging.jar:lib/axis/jaxrpc.jar:lib/axis/log4j-1.2.4.jar:lib/axis/saaj.jar:lib/axis/wsdl4j.jar:lib/grout.jar:$(srcdir)"; \
	  export CLASSPATH; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java org/emboss/jemboss/draw/PrintDNAImage.java_o; \
          javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/draw/DNADraw.java; \
	  javac -target 1.3 -source 1.3 -d . org/emboss/jemboss/Jemboss.java; \
          mv -f ./org/emboss/jemboss/editor/PrintAlignmentImage.java_o ./org/emboss/jemboss/editor/PrintAlignmentImage.java; \
          mv -f org/emboss/jemboss/draw/PrintDNAImage.java_o org/emboss/jemboss/draw/PrintDNAImage.java; \
	  fi); \
	fi
endif

install-exec-hook:
	(cd $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources; sed "s|user.auth=true|user.auth=false|" jemboss.properties | sed "s|jemboss.server=true|jemboss.server=false|" |  sed "s|/packages/emboss/STABLE|$(prefix)|" > jemboss.properties.new; mv -f jemboss.properties.new jemboss.properties; cd -); \
    sed "s|JEMBOSS_HOME=.|JEMBOSS_HOME=$(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/|" runJemboss.sh > $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/runJemboss.sh; \
    chmod 755 $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/runJemboss.sh;
    
pkgdata_DATA = LICENSE readme.html README

pkgdatadir=$(prefix)/share/$(PACKAGE)/jemboss



# tar to pick up the other directories
# then remove any CVS subdirectories

dist-hook:
	tar cBf - api | ( cd $(distdir); tar xBf - ; rm -rf api/CVS );
if EHAVEANT
	ant package -Demboss.data=../emboss/data -Dname=client -Djar.dir=lib -Dnotformemboss=yes
	ant clean
endif
