#

SUBDIRS = source emboss_acd

EXTRA_DIST = depcomp ltmain.sh

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS)

