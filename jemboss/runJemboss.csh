#!/bin/csh -f
setenv CLASSPATH :lib/soap.jar:lib/xerces.jar:lib/mail.jar:lib/activation.jar:lib/jakarta-regexp-1.2.jar:lib/jalview.jar:lib/jnet.jar:lib/jcert.jar:lib/jsse.jar:.:
#
#Commented vars are required for standalone use
#
#setenv EMBOSS_INSTALL /usr/local/lib
#setenv LD_LIBRARY_PATH $EMBOSS_INSTALL
#
#For MacOSX:
#setenv DYLD_LIBRARY_PATH $EMBOSS_INSTALL/lib
#
#For HPUX:
#setenv SHLIB_PATH $EMBOSS_INSTALL/lib

java org/emboss/jemboss/Jemboss &
