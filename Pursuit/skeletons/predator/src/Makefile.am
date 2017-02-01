bin_PROGRAMS = predator 
predator_SOURCES = Predator.C \
  Predator.h 

## make reference for include files and library file (libtool) to pursuitbase
pursuitdir=${top_builddir}/../../pursuitbase/pursuitbase
INCLUDES = -I$(top_srcdir)/../../pursuitbase/pursuitbase
predator_LDADD = ${pursuitdir}/libConnection.la ${pursuitdir}/libGenericValues.la



