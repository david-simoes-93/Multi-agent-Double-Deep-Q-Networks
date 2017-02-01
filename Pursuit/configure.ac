dnl Process this file with autoconf to produce a configure script.
AC_INIT(pursuit_package, 0.8.0, jellekok@science.uva.nl)
AC_CONFIG_SRCDIR([pursuit/configure.ac])
AM_INIT_AUTOMAKE(pursuit_package, 0.9)

dnl process all subdirs, pursuitbase first because it is used by the others
AC_CONFIG_SUBDIRS([pursuitbase pursuit monitor logplayer skeletons])
AC_PROG_MAKE_SET

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
