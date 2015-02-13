IURAR = innounp029.rar
IUDIR = innounp%200.29

tmpinstall: '{app}'
	mkdir -p dist/bin dist/lib dist/share
	cp '{app}'/bin/*.dll dist/lib
	cp '{app}'/bin/*.dll dist/bin
	for f in $(CYGBINS) ; do \
		cp '{app}'/bin/$$f.exe dist/bin ; \
	done
	for f in fonts keyboards rgb ; do \
		test $(BUILDXMING) = yes || cp -r '{app}'/share/$$f dist/share ; \
	done

'{app}': innounp.exe nxsetup.exe
	$(WINE) innounp.exe -x -q -y nxsetup.exe

$(IURAR):
	$(WGET) -O $@ -q 'http://sourceforge.net/projects/innounp/files/innounp/$(IUDIR)/$(IURAR)/download'

innounp.exe: unrar.available $(IURAR)
	$(UNRAR) e -o+ -idcd $(IURAR) $@

unrar.available:
	@if test -z "$(UNRAR)" ; then \
		echo "No unrar available! Please unpack innounp rar archive manually." ; \
		exit 1 ; \
	fi
	touch $@

nxsetup.exe:
	$(PERL) getnxsetup

CLEANFILES = -r dist '{app}' install_script.iss unrar.available
DISTCLEANFILES = $(IURAR) innounp.exe nxsetup.exe
EXTRA_DIST = getnxsetup
MAINTAINERCLEANFILES = Makefile.in
