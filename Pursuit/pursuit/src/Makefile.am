## Makefile.am for pursuit server

## define the program that has to be built and the related sources
bin_PROGRAMS = pursuit
pursuit_SOURCES = World.C \
  World.h \
  Parameters.h \
  Parameters.C \
  types.h \
  types.C \
  Agent.h \
  Agent.C \
  main.C 

## make reference for include files and library file (libtool) to pursuitbase
pursuitdir=${top_builddir}/../pursuitbase/pursuitbase
INCLUDES = -I$(top_srcdir)/../pursuitbase/pursuitbase
pursuit_LDADD = ${pursuitdir}/libConnection.la ${pursuitdir}/libGenericValues.la

## also distribute configuration file with distribution 
EXTRA_DIST = pursuit.conf

