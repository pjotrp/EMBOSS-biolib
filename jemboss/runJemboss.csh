#!/bin/csh -f
setenv CLASSPATH :lib/soap.jar:lib/xerces.jar:lib/mail.jar:lib/activation.jar:lib/jakarta-regexp-1.2.jar:.:uk/embnet/org/jemboss/:resources:
#setenv LD_LIBRARY_PATH /packages/emboss_dev/tcarver/embossjava/emboss/ajax/.libs
/packages/java/jdk1.3/bin/java org/emboss/jemboss/Jemboss &
