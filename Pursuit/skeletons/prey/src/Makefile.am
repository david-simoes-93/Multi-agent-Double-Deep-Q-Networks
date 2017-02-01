bin_PROGRAMS = prey 
prey_SOURCES = Prey.C \
  Prey.h 


## make reference for include files and library file (libtool) to pursuitbase
pursuitdir=${top_builddir}/../../pursuitbase/pursuitbase
INCLUDES = -I$(top_srcdir)/../../pursuitbase/pursuitbase
prey_LDADD = ${pursuitdir}/libConnection.la ${pursuitdir}/libGenericValues.la

