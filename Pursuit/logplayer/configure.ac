AC_PREREQ(2.57)
AC_INIT(pursuit_logplayer,0.8,0, jellekok@science.uva.nl)
AC_CONFIG_SRCDIR(src/main.C)
AM_INIT_AUTOMAKE(pursuit_logplayer,0.8)
AM_CONFIG_HEADER(config.h)

#check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_LIBTOOL

AC_PATH_PROG(GTKCONFIG, gtk-config,,$XPATH:$PATH)

if test "$GTKCONFIG"; then
	CPPFLAGS=$($GTKCONFIG --cflags)
	LDFLAGS=$($GTKCONFIG --libs)
	AC_MSG_RESULT($GTKCONFIG)
else
	AC_MSG_RESULT(missing)
	AC_MSG_ERROR([can't find working gtk-config in \$PATH])
fi

# add options in order to manually specify the glut include and lib directory
AC_ARG_WITH(gtk-incdir,
        AC_HELP_STRING([--with-gtk-incdir=DIR],
                       [add DIR as gtk include directory]),
                       CPPFLAGS="$CPPFLAGS -I$withval", )

AC_ARG_WITH(gtk-libdir,
        AC_HELP_STRING([--with-gtk-libdir=DIR],
                       [add DIR as gtk library directory]),
                       LDFLAGS="$LDFLAGS -L$withval", )

AC_ARG_WITH(glib-incdir,
        AC_HELP_STRING([--with-glib-incdir=DIR],
                       [add DIR as glib include directory]),
                       CPPFLAGS="$CPPFLAGS -I$withval", )

AC_ARG_WITH(glibconfig-incdir,
        AC_HELP_STRING([--with-glibconfig-incdir=DIR],
                       [add DIR as glibconfig include directory]),
                       CPPFLAGS="$CPPFLAGS -I$withval", )
AC_ARG_WITH(gdk-libdir,
        AC_HELP_STRING([--with-gdk-libdir=DIR],
                       [add DIR as gdk library directory]),
                       LDFLAGS="$LDFLAGS -L$withval", )


AC_CHECK_HEADERS([glibconfig.h],[], AC_MSG_ERROR([Could not find the glibconfig headers.]))
AC_CHECK_HEADERS([glib.h],[], AC_MSG_ERROR([Could not find the glib headers.]))
AC_CHECK_HEADERS([gtk/gtk.h],[], AC_MSG_ERROR([Could not find the gtk headers.]))

AC_CHECK_LIB([gdk], [gdk_exit], [], AC_MSG_ERROR([Could not find the gdk library.]) )
AC_CHECK_LIB([gtk], [gtk_init], [], AC_MSG_ERROR([Could not find the gtk library.]), -lgdk)
#check libs
# FIXME: Replace `main' with a function in `-lGL':
#AC_CHECK_LIB([GL], [glMatrixMode], [], AC_MSG_ERROR([Could not find the GL library.]))
# FIXME: Replace `main' with a function in `-lGLU':
#AC_CHECK_LIB([GLU], [gluCylinder], [], AC_MSG_ERROR([Could not find the GLU library.]))
# FIXME: Replace `main' with a function in `-lX11':
#AC_CHECK_LIB([X11], [XtVaAppInitialize])
# FIXME: Replace `main' with a function in `-lXext':
#AC_CHECK_LIB([Xext], [XmbufChangeBufferAttributes])
# FIXME: Replace `main' with a function in `-lXi':
#AC_CHECK_LIB([Xi], [main])
# FIXME: Replace `main' with a function in `-lXmu':
#AC_CHECK_LIB([Xmu], [XmuAddCloseDisplayHook])
# FIXME: Replace `main' with a function in `-ldl':
#AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lglut':
#AC_CHECK_LIB([glut], [glutInit],[], AC_MSG_ERROR([Could not find the glut library.]), -lGL -lGLU)
# FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [inet_ntoa])
AC_CHECK_LIB([pthread], [pthread_create])
# FIXME: Replace `main' with a function in `-lsocket':
AC_CHECK_LIB([socket], [recvfrom])


#check header files
AC_HEADER_STDC
#AC_CHECK_HEADERS([GL/glut.h],[], AC_MSG_ERROR([Could not find the glut headers.]))
#AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([atexit on_exit select strrchr])


AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
