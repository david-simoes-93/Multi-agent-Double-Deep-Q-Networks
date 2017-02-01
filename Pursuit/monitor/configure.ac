AC_PREREQ(2.57)

AC_INIT(pursuit_monitor, 0.8.0. jellekok@science.uva.nl)
AC_CONFIG_SRCDIR(src/Monitor.C)
AM_INIT_AUTOMAKE(pursuit_monitor,0.8)
AM_CONFIG_HEADER(config.h)

#check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_LIBTOOL

# add options in order to manually specify the glut include and lib directory
AC_ARG_WITH(glut-incdir,
        AC_HELP_STRING([--with-glut-incdir=DIR],
                       [add DIR as glut include directory]),
                       CPPFLAGS="$CPPFLAGS -I$withval", )

AC_ARG_WITH(glut-libdir,
        AC_HELP_STRING([--with-glut-libdir=DIR],
                       [add DIR as glut library directory]),
                       LDFLAGS="$LDFLAGS -L$withval", )
#check libs
# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [glMatrixMode], [], AC_MSG_ERROR([Could not find the GL library.]))
# FIXME: Replace `main' with a function in `-lGLU':
AC_CHECK_LIB([GLU], [gluCylinder], [], AC_MSG_ERROR([Could not find the GLU library.]))
# FIXME: Replace `main' with a function in `-lX11':
#AC_CHECK_LIB([X11], [XtVaAppInitialize])
# FIXME: Replace `main' with a function in `-lXext':
#AC_CHECK_LIB([Xext], [XmbufChangeBufferAttributes])
# FIXME: Replace `main' with a function in `-lXi':
AC_CHECK_LIB([Xi], [main])
# FIXME: Replace `main' with a function in `-lXmu':
AC_CHECK_LIB([Xmu], [XmuAddCloseDisplayHook])
# FIXME: Replace `main' with a function in `-ldl':
#AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lglut':
AC_CHECK_LIB([glut], [glutInit],[], AC_MSG_ERROR([Could not find the glut library.]), -lGL -lGLU )
# FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [inet_ntoa])
AC_CHECK_LIB([pthread], [pthread_create])
# FIXME: Replace `main' with a function in `-lsocket':
AC_CHECK_LIB([socket], [recvfrom])


#check header files
AC_HEADER_STDC
AC_CHECK_HEADERS([GL/glut.h],[], AC_MSG_ERROR([Could not find the glut headers.]))
AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname gettimeofday inet_ntoa memset select socket strstr strtol])
AC_SEARCH_LIBS(gethostbyname, nsl )
AC_SEARCH_LIBS(socket, socket )


AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
