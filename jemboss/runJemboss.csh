#!/bin/csh -f
#
# 
#
setenv LIB ./lib
setenv CLASSPATH :$LIB/soap.jar:$LIB/xerces.jar:$LIB/mail.jar:$LIB/activation.jar:$LIB/jakarta-regexp-1.2.jar:$LIB/jalview.jar:$LIB/jnet.jar:$LIB/jcert.jar:$LIB/jsse.jar:.:
#
#Commented vars are required for standalone use
#
#setenv EMBOSS_INSTALL /packages/emboss/STABLE/lib
#setenv LD_LIBRARY_PATH $EMBOSS_INSTALL
#
#For MacOSX:
#setenv DYLD_LIBRARY_PATH $EMBOSS_INSTALL/lib
#
#For HPUX:
#setenv SHLIB_PATH $EMBOSS_INSTALL/lib
#
#Add local to run as 'standalone':
#java org.emboss.jemboss.Jemboss local &
#
java org.emboss.jemboss.Jemboss &
