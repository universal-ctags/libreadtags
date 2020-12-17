# -*- Autoconf -*-
AC_INIT(libreadtags, 0.0.0)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([readtags.c])

AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL

# libtool -version-info
AC_SUBST(LT_VERSION, [0:0:0])

AC_ARG_ENABLE([gcov],
	[AS_HELP_STRING([--enable-gcov],
		[enable 'gcov' coverage testing tool [no]])])
if test "${enable_gcov}" = "yes"; then
	GCOV_CFLAGS="--coverage"
fi
AC_SUBST([GCOV_CFLAGS])

AC_PROG_CC_C99

AC_CONFIG_FILES([Makefile
		libreadtags.pc
		tests/Makefile])
AC_OUTPUT
