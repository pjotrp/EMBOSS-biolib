#

SUBDIRS = src emboss_acd

EXTRA_DIST = depcomp

# tar to pick up the other directories
# then remove any CVS subdirectories

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS ) 
	tar cBf - doc | ( cd $(distdir); tar xBf - ; cd doc; rm -rf CVS ) 
	tar cBf - include | ( cd $(distdir); tar xBf - ; cd include; rm -rf CVS ) 
