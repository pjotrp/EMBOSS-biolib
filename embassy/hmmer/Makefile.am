#

SUBDIRS = src emboss_acd

EXTRA_DIST =

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS)

