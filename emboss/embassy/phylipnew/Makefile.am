#
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src emboss_acd data emboss_doc

EXTRA_DIST = depcomp ltmain.sh install-sh config.sub config.guess

# tar to pick up the other directories
# then remove any CVS subdirectories

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS ) 
	tar cBf - emboss_doc | ( cd $(distdir); tar xBf - ; cd emboss_doc; rm -rf CVS; rm -rf master) 
	tar cBf - doc | ( cd $(distdir); tar xBf - ; cd doc; rm -rf CVS ) 
	tar cBf - include | ( cd $(distdir); tar xBf - ; cd include; rm -rf CVS ) 
	tar cBf - data | ( cd $(distdir); tar xBf - ; cd data; rm -rf CVS ) 
