#

SUBDIRS = src emboss_acd

EXTRA_DIST = copyright depcomp

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS)

