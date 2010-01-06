SUBDIRS = axis

all-local:
if EHAVEANT
	ant -f ../build.xml package -Demboss.data=../emboss/data -Dname=jemboss -Djar.dir=lib -Dnotformemboss=yes;
endif

# The install-exec-hook is for the case where a jemboss.jar has
# been created by the 'ant' packaging for Jemboss.
# In that case it needs to be copied to the install dir.

install-exec-hook:
	if test -f jemboss.jar ; then \
	    (if test -d $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/lib ; then \
	        echo "jemboss/lib exists"; \
	     else \
	        mkdir $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/lib; \
	     fi); \
	cp jemboss.jar $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/lib; \
	fi;


pkgdata_DATA = README activation.jar jakarta-regexp-1.2.jar \
mail.jar jalviewApplet.jar

pkgdatadir=$(prefix)/share/$(PACKAGE)/jemboss/lib
