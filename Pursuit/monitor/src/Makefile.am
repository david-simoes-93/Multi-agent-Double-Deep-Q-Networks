## specify program and related sources 
bin_PROGRAMS = pursuit_monitor
pursuit_monitor_SOURCES = Monitor.C \
        MonitorParameters.C \
        Monitor.h \
	MonitorParameters.h \
	RGBcolor.C \
	RGBcolor.h 

## also put configuration file in the distribution
EXTRA_DIST=monitor.conf

## make reference for include files and library file (libtool) to pursuitbase
pursuitdir=${top_builddir}/../pursuitbase/pursuitbase
INCLUDES = -I$(top_srcdir)/../pursuitbase/pursuitbase
pursuit_monitor_LDADD = ${pursuitdir}/libConnection.la ${pursuitdir}/libGenericValues.la


