PULSEAUDIO            = pa-win32
PULSEAUDIO_VERSION    = 6.0-11.1
OURPA                 = $(PULSEAUDIO)-$(PULSEAUDIO_VERSION)

TMPDIST = $(abs_builddir)/dist

EXTRA_DIST = urls.txt
CLEANFILES = -r $(TMPDIST) $(OURPA)
DISTCLEANFILES=$(OURPA).tar.gz
MAINTAINERCLEANFILES = Makefile.in


tmpdist: tmpinstall
	cp -av $(OURPA) $(TMPDIST)

tmpinstall: $(OURPA)

$(OURPA): $(OURPA).tar.gz
	$(vunpack__$(AM_DEFAULT_VERBOSITY)$(V))tar xzf $(OURPA).tar.gz

$(OURPA).tar.gz:
	$(WGET) -O $@ -q 'http://unixforum.org/up/nxman/opennx/$(OURPA).tar.gz'
