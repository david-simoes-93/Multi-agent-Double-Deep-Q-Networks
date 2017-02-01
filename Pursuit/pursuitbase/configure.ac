AC_PREREQ(2.57)

#create libray name, version numbering
GENERIC_LIBRARY_NAME=pursuitbase

srcfile=${GENERIC_LIBRARY_NAME}./Connection.C
AC_INIT(srcfile)
AC_CONFIG_SRCDIR($srcfile)
AM_CONFIG_HEADER(config.h)

GENERIC_LIBRARY_NAME=pursuitbase
AC_SUBST(GENERIC_LIBRARY_NAME)

#release versioning
GENERIC_MAJOR_VERSION=0
GENERIC_MINOR_VERSION=8
GENERIC_MICRO_VERSION=0

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
GENERIC_API_VERSION=1.0
AC_SUBST(GENERIC_API_VERSION)

#shared library versioning
GENERIC_LIBRARY_VERSION=0:8:0
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed

# subst makes sure related Makefile.am's also contain this variable
AC_SUBST(GENERIC_LIBRARY_VERSION)

PACKAGE=$GENERIC_LIBRARY_NAME


GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

VERSION=$GENERIC_VERSION

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)

#check for programs
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_PROG_INSTALL

#check libs

#check for header fils
AC_HEADER_STDC
AC_CHECK_HEADERS(arpa/inet.h netdb.h stdlib.h string.h sys/socket.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS(gethostbyname inet_ntoa memset select socket strcasecmp strchr strdup strstr)
AC_SEARCH_LIBS(gethostbyname, nsl )#, [LIBS="$LIBS -lnsl"])
AC_SEARCH_LIBS(socket, socket )#, [LIBS="$LIBS -lsocket"])


AC_CONFIG_FILES([Makefile pursuitbase/Makefile])
AC_OUTPUT
