#

SUBDIRS = ckit source h emboss_acd

EXTRA_DIST = LICENSE depcomp

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS) 

