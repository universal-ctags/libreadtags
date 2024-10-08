# -*- Autoconf -*-
#
# When updating the above version, you also update VERSION in CMakeList.txt.
AC_INIT(libreadtags, 0.4.0)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([readtags.c])

AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL

# LT_VERSION => CURRENT[:REVISION[:AGE]]
#
# (the following instructions are taken from configure.ac in libskk
#  See also "7 Library interface versions" in the libtool info document.)
#
# If library source has changed since last release, increment revision
# If public symbols have been added, removed or changed since last release,
#  increment current and set revision to 0
# If public symbols have been added since last release, increment age
# If public symbols have been removed since last release, set age to 0
#
# Revision history
#
# 0:0:0
#    initial version; API is the same as readtags.h in Exuberant-ctags.
#    (libreadtags.so.0.0.0)
#
# 1:0:0
#    introduced tagsGetErrno() and tagErrno.
#    rename sortType to tagSortType.
#    (libreadtags.so.1.0.0)
#
# 2:0:1
#    introduced TagErrnoFileMayTooBig.
#    introduced tagsFindPseudoTag.
#    (libreadtags.so.1.1.0)
#
# 2:1:1
#    no change in public interface.
#    (libreadtags.so.1.1.1)
#
# 2:2:1
#    no change in public interface.
#    (libreadtags.so.1.1.2)
#
# When updating LT_VERSION, update BUILD_VERSION of CMakeLists.txt.
#
AC_SUBST(LT_VERSION, [2:2:1])

AC_ARG_ENABLE([gcov],
	[AS_HELP_STRING([--enable-gcov],
		[enable 'gcov' coverage testing tool [no]])])
if test "${enable_gcov}" = "yes"; then
	GCOV_CFLAGS="--coverage"
fi
AC_SUBST([GCOV_CFLAGS])

AC_PROG_CC_C99
AC_C_INLINE

AC_CONFIG_FILES([Makefile
		libreadtags.pc
		libreadtags-uninstalled.pc
		tests/Makefile])
AC_OUTPUT
