data_DATA = pconnect.html

pconnect.html: pconnect.1
	man -l -Thtml $< > $@

EXTRA_DIST = $(data_DATA)
MAINTAINERCLEANFILES = Makefile.in

.PRECIOUS: pconnect.html
