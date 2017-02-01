AC_INIT(pursuit, 0.80, jellekok@science.uva.nl) 

AC_PREREQ(2.57)

AC_CONFIG_SRCDIR(src/main.C)
AM_INIT_AUTOMAKE(pursuit,0.8)
AC_CONFIG_HEADERS(config.h)

#check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_LIBTOOL 

## Do not ask for the pursuitbase directory since it is part of source tree

# Ask user for path to pursuitbase
#AC_ARG_WITH(pursuitbase,
#    [ --with-pursuitbase=<path> prefix of pursuitbase installation],
#    [PURSUIT_PREFIX=$with_pursuitbase],
#    AC_MSG_ERROR([You must call configure with the --with-pursuitbase option.
#    This tells configure where to find the pursuitbase library and headers.
#    e.g. --with-pursuitbase=/usr/local or --with-pursuitbase=/usr])
#)

#CPPFLAGS="$CPPFLAGS -I../pursuitbase/pursuitbase"
#LDFLAGS="$LDFLAGS -L../pursuitbase/pursuitbase"

#AC_ARG_WITH(connection-incdir,
#        AC_HELP_STRING([--with-connection-incdir=DIR],
#                       [add DIR as Connection include directory]),
#                       CPPFLAGS="$CPPFLAGS -I$withval", )

#AC_ARG_WITH(connection-libdir,
#        AC_HELP_STRING([--with-connection-libdir=DIR],
#                       [add DIR as connection library directory]),
#                       LDFLAGS="$LDFLAGS -L$withval", )

#AC_ARG_WITH(genericvalues-incdir,
#        AC_HELP_STRING([--with-genericvalues-incdir=DIR],
#                       [add DIR as Genericvalues include directory]),
#                       CPPFLAGS="$CPPFLAGS -I$withval", )

#AC_ARG_WITH(genericvalues-libdir,
#        AC_HELP_STRING([--with-genericvalues-libdir=DIR],
#                       [add DIR as Genericvalues library directory]),
#                       LDFLAGS="$LDFLAGS -L$withval", )


#AC_SUBST(PURSUIT_PREFIX)
#PURSUIT_LIBS="-L${PURSUIT_PREFIX} -lConnection -lGenericValues"
#PURSUIT_CFLAGS="-I${PURSUIT_PREFIX}/include"
#AC_SUBST(PURSUIT_LIBS)
#AC_SUBST(PURSUIT_CFLAGS)

#AC_LANG(C++)
#AC_CHECK_HEADERS([Connection.h],[], AC_MSG_ERROR([Could not find the Connection headers.]))

#AC_CHECK_HEADERS([GenericValues.h],[], AC_MSG_ERROR([Could not find the GenericValues headers.]))

#AC_CHECK_LIB([Connection], [Connection], [], AC_MSG_ERROR([Could not find the Connection library.]) )
#AC_CHECK_LIB([GenericValues], [getClassName], [], AC_MSG_ERROR([Could not find the GenericValues library.]) )

#check libs
AC_CHECK_LIB(pthread, pthread_create)

#check for headers
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h string.h sys/time.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(atexit on_exit gettimeofday select socket strcasecmp strchr strdup strstr strtol)
AC_SEARCH_LIBS(socket, socket )#, [LIBS="$LIBS -lsocket"])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
