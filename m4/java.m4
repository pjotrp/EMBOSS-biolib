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
  if test -d $withval ; then
	echo "Java directory $withval exists"
  else
	echo "Java directory $withval does not exist"
	exit 0
  fi

  ALT_HOME="${ALT_HOME} -DHAVE_JAVA"
  AC_CHECK_PROG(havejavac,javac,"yes","no")
  if test "${havejavac}" != yes ; then
  echo "Error: Either JAVA not installed or 'javac' not in your PATH"
  exit 1
  fi
  JAVA_OK=yes
  AC_SUBST(JAVA_OK)
else
  JAVA_OK=no
  AC_SUBST(JAVA_OK)
  AC_MSG_RESULT(no)

fi], [

AC_MSG_RESULT(no)

])

if test -n "${ALT_HOME}"
then
	CPPFLAGS="$CPPFLAGS -I${ALT_HOME}"
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
	  if test -d $withval ; then
		echo "Javaos directory $withval exists"
	  else
		echo "Javaos directory $withval does not exist"
		exit 0
	  fi
	  ALT_HOMEOS="$withval"
else
  AC_MSG_RESULT(no)
fi], [
AC_MSG_RESULT(no)
	if test "$JAVA_OK" = "yes" ; then
		echo "Error: --with-java=dir unspecified"
		exit 0
	fi
])

if test -n "${ALT_HOMEOS}"
then
	CPPFLAGS="$CPPFLAGS -I${ALT_HOMEOS}"
fi


])




