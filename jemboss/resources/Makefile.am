install-exec-hook:
	  @if test "${JAVA_OK}" = "yes" ; then \
	          (if test -d $(DESTDIR)/$(prefix)/share/$(PACKAGE)/jemboss/resources; then \
			echo "resources exists"; \
		   else \
			mkdir $(DESTDIR)/$(prefix)/share/$(PACKAGE)/jemboss/resources; \
		   fi); \
	          rm -f $(DESTDIR)/$(prefix)/share/$(PACKAGE)/jemboss/resources/acdstore.jar; \
		  cd $(EMBOSS_TOP)/emboss/acd; \
		  jar cvf $(DESTDIR)/$(prefix)/share/$(PACKAGE)/jemboss/resources/acdstore.jar *.acd; \
	  fi


pkgdata_DATA = jemboss.properties readme.html readmeAlign.html readmeDNADraw.html results.html version filemgr.html seqList.html

pkgdatadir=$(prefix)/share/$(PACKAGE)/jemboss/resources
