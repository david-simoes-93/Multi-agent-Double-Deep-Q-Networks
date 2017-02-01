## specify the program and the related sources
bin_PROGRAMS = pursuit_logplayer
pursuit_logplayer_SOURCES = main.C \
  LogWorld.h \
  LogWorld.C \
  LogPlayer.h \
  LogPlayer.C \
  Controller.h \
  Controller.C \
  types.h

## add the logplayer pics to the distribution
EXTRA_DIST = go.xpm  minus.xpm  play.xpm  plus.xpm  rev.xpm  stop.xpm

## make reference for include files and library file (libtool) to pursuitbase
pursuitdir=${top_builddir}/../pursuitbase/pursuitbase
INCLUDES = -I$(top_srcdir)/../pursuitbase/pursuitbase
pursuit_logplayer_LDADD = ${pursuitdir}/libConnection.la ${pursuitdir}/libGenericValues.la
