dnl @synopsis CHECK_JAVA()
dnl
dnl Need to specify --with-java and --with-javajni
dnl @author Alan Bleasby (ableasby@hgmp.mrc.ac.uk)
dnl

AC_DEFUN(CHECK_JAVA,
#
# Handle user hints
#
[AC_MSG_CHECKING(if java include directory given)
AC_ARG_WITH(java,
[  --with-java=DIR root directory path of java  installation [defaults /usr/java/jdk1.3.1/include]
  --without-java to disable java],
[if test "$withval" != no ; then
  AC_MSG_RESULT(yes)
  ALT_HOME="$withval"
else
  AC_MSG_RESULT(no)
fi], [
AC_MSG_RESULT(yes)
ALT_HOME=/usr/java/jdk1.3.1/include
])

if test -n "${ALT_HOME}"
then
	CPPFLAGS="$CPPFLAGS -I${ALT_HOME} -DHAVE_JAVA"
fi

])



AC_DEFUN(CHECK_JAVAOS,
#
# Handle user hints
#
[AC_MSG_CHECKING(if java OS include directory given)
AC_ARG_WITH(javaos,
[  --with-javaos=DIR root directory path of java installation include OS [defaults /usr/java/jdk1.3.1/include/linux] ],
[if test "$withval" != no ; then
  AC_MSG_RESULT(yes)
  ALT_HOMEOS="$withval"
else
  AC_MSG_RESULT(no)
fi], [
AC_MSG_RESULT(yes)
ALT_HOMEOS=/usr/java/jdk1.3.1/include/linux
])

if test -n "${ALT_HOMEOS}"
then
	CPPFLAGS="$CPPFLAGS -I${ALT_HOMEOS}"
fi


])




