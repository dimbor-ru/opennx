IURAR = innounp041.rar
INXCSN = NXClientCE-3.5.0-9-r15.6.exe

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
	$(WGET) -O $@ 'https://unixforum.org/up/nxman/opennx/$(IURAR)'

innounp.exe: unrar.available $(IURAR)
	$(UNRAR) e -o+ -idcd $(IURAR) $@

unrar.available:
	@if test -z "$(UNRAR)" ; then \
		echo "No unrar available! Please unpack innounp rar archive manually." ; \
		exit 1 ; \
	fi
	touch $@

nxsetup.exe:
	$(WGET) -O $@ -q 'http://unixforum.org/up/nxman/$(INXCSN)'

CLEANFILES = -r dist '{app}' install_script.iss unrar.available
DISTCLEANFILES = $(IURAR) innounp.exe nxsetup.exe
MAINTAINERCLEANFILES = Makefile.in
