#

SUBDIRS = source emboss_acd

EXTRA_DIST = depcomp ltmain.sh

# tar to pick up the other directories
# then remove any CVS subdirectories

dist-hook:
	tar cBf - emboss_acd | ( cd $(distdir); tar xBf - ; cd emboss_acd; rm -rf CVS) 

