dnl @synopsis CHECK_PNGDRIVER()
dnl
dnl This macro searches for an installed png/gd/zlib library. If nothing
dnl was specified when calling configure, it searches first in /usr/local
dnl and then in /usr. If the --with-pngdriver=DIR is specified, it will try
dnl to find it in DIR/include/zlib.h and DIR/lib/libz.a. If --without-pngdriver
dnl is specified, the library is not searched at all.
dnl
dnl It defines the symbol PLD_png if the librarys are found. You should
dnl use autoheader to include a definition for this symbol in a config.h
dnl file.
dnl
dnl Sources files should then use something like
dnl
dnl   #ifdef PLD_png
dnl   #include <zlib.h>
dnl   #endif /* PLD_png */
dnl
dnl @author Ian Longden <il@sanger.ac.uk>
dnl Modified: Alan Bleasby. Corrected library order
dnl Modified: David Mathog. Looks in ALT_HOME, and then /usr no matter what
dnl                         for all 3 libs.
dnl Modified: Alan Bleasby. Correct png def. Correct cacheing. Correct
dnl                         for case of all libs in /usr
dnl

AC_DEFUN(CHECK_PNGDRIVER,
#
# Handle user hints
#
[AC_MSG_CHECKING(if png driver is wanted)
AC_ARG_WITH(pngdriver,
[  --with-pngdriver=DIR root directory path of png/gd/zlib installation [defaults to
                    /usr/local or /usr if not found in /usr/local]
  --without-pngdriver to disable pngdriver usage completely],
[if test "$withval" != no ; then
  AC_MSG_RESULT(yes)
  ALT_HOME="$withval"
  ALT_ALLINUSR=1
else
  AC_MSG_RESULT(no)
  ALT_ALLINUSR=1
fi], [
AC_MSG_RESULT(yes)
ALT_HOME=/usr/local
ALT_USED=0
ALT_ALLINUSR=1
])



#
# Locate png/gd/zlib, if wanted
#
if test -d "${ALT_HOME}"
then

# See if they're all in /usr anyway
	AC_CHECK_LIB(z, inflateEnd, CHECK=1, CHECK=0, -L/usr/lib -lz)
	if test $CHECK = "0" ; then
		ALT_ALLINUSR=0
	fi
	AC_CHECK_LIB(png, png_destroy_read_struct, CHECK=1, CHECK=0 , -L/usr/lib -lz)
	if test $CHECK = "0" ; then
		ALT_ALLINUSR=0
	fi
	AC_CHECK_LIB(gd, gdImageCreateFromPng, CHECK=1, CHECK=0 , -L/usr/lib -lgd -lpng -lz -lm)
	if test $CHECK = "0" ; then
		ALT_ALLINUSR=0
	fi

	unset ac_cv_lib_z_inflateEnd
	unset ac_cv_lib_png_png_destroy_read_struct
	unset ac_cv_lib_gd_gdImageCreateFromPng


if test $ALT_ALLINUSR = "0" ; then

#
# Keep a copy if it fails
#
	ALT_LDFLAGS="$LDFLAGS"
	ALT_CPPFLAGS="$CPPFLAGS"

#
# Set 
#
        LDFLAGS="${LDFLAGS} -L${ALT_HOME}/lib"
        CPPFLAGS="$CPPFLAGS -I$ALT_HOME/include"
#
# Check for zlib in ALT_HOME
#
        AC_CHECK_LIB(z, inflateEnd, CHECK=1, CHECK=0, -L${ALT_HOME}/lib -lz)
#
# Failed?  Look in /usr/lib too.  If it shows up there it won't change LDFLAGS which
# should always include /usr/lib.
#       
	if test $CHECK = "0" ; then
	unset ac_cv_lib_z_inflateEnd
	  AC_CHECK_LIB(z, inflateEnd, CHECK=1, CHECK=0 , -L/usr/lib -lz)
        else
          ALT_USED=1
	fi

#
# Check for png
#
	if test $CHECK = "1" ; then
	  AC_CHECK_LIB(png, png_destroy_read_struct, CHECK=1, CHECK=0 , -L${ALT_HOME}/lib -lz)
	  if test $CHECK = "0" ; then
	    unset ac_cv_lib_png_png_destroy_read_struct
	    AC_CHECK_LIB(png, png_destroy_read_struct, CHECK=1, CHECK=0 , -L/usr/lib -lz)
          else
            ALT_USED=1
	  fi
	fi
	
#
# Check for gd
#
	if test $CHECK = "1"; then
	  AC_CHECK_LIB(gd, gdImageCreateFromPng, CHECK=1, CHECK=0 , -L${ALT_HOME}/lib -lgd -lpng -lz -lm)
	  if test $CHECK = "0" ; then
	    unset ac_cv_lib_gd_gdImageCreateFromPng
	    AC_CHECK_LIB(gd, gdImageCreateFromPng, CHECK=1, CHECK=0 , -L/usr/lib -lgd -lpng -lz -lm)
          else
            ALT_USED=1
	  fi
          if test $CHECK = "0"; then
		echo need to upgrade gd for png driver for plplot
	  fi
	fi
#
# If everything found okay then proceed to include png driver in config.
#
	if test $CHECK = "1" ; then
	  LIBS="$LIBS -lgd -lpng -lz -lm"
	  AC_DEFINE(PLD_png)
	  AM_CONDITIONAL(AMPNG, true)
          if test $ALT_USED = "1" ; then
	    echo Using libz, libgd, and/or libpng from ${ALT_HOME}
          else
            LDFLAGS="$ALT_LDFLAGS"
	    CPPFLAGS="$ALT_CPPFLAGS"
          fi
	else
#
# If not okay then reset FLAGS.
#
	  LDFLAGS="$ALT_LDFLAGS"
	  CPPFLAGS="$ALT_CPPFLAGS"
	  echo No png driver will be made due to librarys missing/old.
	fi
#       echo PNG STUFF FOLLOWS!!!
#       echo CHECK = $CHECK
#       echo LIBS = $LIBS
#       echo LDFLAGS = $LDFLAGS
#       echo CPPFLAGS = $CPPFLAGS


else
	echo "PNG libraries found in standard system locations"
	LIBS="$LIBS -lgd -lpng -lz -lm"
	LDFLAGS=$LDFLAGS
	CPPFLAGS=$CPPFLAGS
fi



else
        if test $withval != "no"; then
		echo "Directory $ALT_HOME does not exist"
		exit 0
        fi
fi
])
