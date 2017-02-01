## Makefile.am for pursuitbase  top directory

## process subdirecty 
SUBDIRS=${GENERIC_LIBRARY_NAME}

## add configuration file of doxygen to distribution
EXTRA_DIST=doc/doxygen.cfg

## make doc will create doxygen output (if doxygen is installed)
doc: 
	doxygen doc/doxygen.cfg

## Clean doc/html directory where doxygen places its output
CLEANFILES=doc/html/*

## to make sure make doc will always call doxygen rule
.PHONY: doc
