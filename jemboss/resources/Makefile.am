# The intention here is to check whether JAVA_OK exists and, if
# so, do a server installation. Otherwise check to see whether
# there is already a resources.jar in the directory (from
# an 'ant' build of a standalone setup) and, if so, copy it.
#
install-exec-hook:
	  @if test "${JAVA_OK}" = "yes" ; then \
	          (if test -d $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources; then \
			echo "resources exists"; \
		   else \
			mkdir $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources; \
		   fi); \
	          rm -f   $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources/acdstore.jar; \
		  cd $(EMBOSS_TOP)/emboss/acd; \
		  jar cvf $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources/acdstore.jar *.acd; \
                  rm -f   $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources/resources.jar; \
                  cd $(DESTDIR)$(prefix)/share/$(PACKAGE)/data; \
                  jar cvf $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources/resources.jar EPAM* EBLOSUM* ENUC*; \
                  rm -f   $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources/Ajax.jar; \
                  cd $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss; \
                  jar cvf $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources/Ajax.jar org/emboss/jemboss/parser/Ajax.class; \
	  else \
		(if test -f resources.jar ; then \
	            (if test -d $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources ; then \
			  echo "resources exists"; \
		     else \
			  mkdir $(DESTDIR)$(prefix)/share/$(PACKAGE)/jemboss/resources; \
		     fi); \
		  cp resources.jar $(prefix)/share/$(PACKAGE)/jemboss/resources; \
		fi); \
	fi

pkgdata_DATA = jemboss.properties readme.html readmeAlign.html readmeDNADraw.html results.html version filemgr.html seqList.html

pkgdatadir=$(prefix)/share/$(PACKAGE)/jemboss/resources
