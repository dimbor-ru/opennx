TARBALL = Xming_69031_source.tar.bz2
DLURL = http://sourceforge.net/projects/xming/files/Xming-source/6.9.0.31/$(TARBALL)/download
TMPDIST = $(abs_builddir)/dist

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = -rf tmpinstall build-$(XMING_BTYPE) patchedsrc srcdir checktarball ccdir \
	xc ccbin include
DISTCLEANFILES = -rf dist $(TARBALL)
EXTRA_DIST = version.def Xming_69031_source.tar.bz2.md5sum patches
XMING_GENERIC_PATCHES = xming-build.patch xming-build-subsys.patch \
	xming-build-xauth.patch xming-build-xset.patch xming-build-nls.patch \
	xming-envbasedir.patch xming-optionhide.patch xming-mutex.patch \
	xming-terminate.patch xming-build-fontutils.patch xming-logdebug.patch \
	xming-clipfilter.patch xming-kbhook.patch xming-kioskhack.patch

buildfromscratch: distclean tmpinstall

install-cross: build-cross
	@$(MAKE) -C xc install DESTDIR=$(TMPDIST) \
		CROSSCOMPILEDIR=$(abs_builddir)/ccbin 
install-native: build-native
	@$(MAKE) -C xc install DESTDIR=$(TMPDIST)

xming-distdir:
	@echo installing Xming to $(TMPDIST)
	@rm -rf $(TMPDIST)
	@mkdir $(TMPDIST)

tmpinstall: xming-distdir install-$(XMING_BTYPE)
	@echo applying post install fixes
	@for f in $(TMPDIST)/usr/X11R6/bin/* ; do \
		FT="`file $$f`" ; \
		case "$$FT" in \
			*symbolic?link*) rm -f $$f ;; \
			*script*) rm -f $$f ;; \
		esac ; \
	done
	@touch $@

build-cross: ccdir patchedsrc
	@echo building Xming
	@$(MAKE) -C xc World CROSSCOMPILEDIR=$(abs_builddir)/ccbin
	@touch $@

build-native: patchedsrc
	@echo building Xming
	@$(MAKE) -C xc World CC=$(CC) PREPROCESS_CMD="$(CC) -E" \
		BOOTSTRAPCFLAGS="-DCROSSCOMPILE_CPP"
	@touch $@

patchedsrc: srcdir version.def
	@echo applying build fixes
	@for p in $(XMING_GENERIC_PATCHES) $(XMING_PLATFORM_PATCHES) ; do \
		if test -f $(srcdir)/patches/$$p ; then \
    		echo applying $$p ; \
			patch -p0 < $(srcdir)/patches/$$p ; \
		fi \
	done
	@cp $(top_srcdir)/extres/nx.ico xc/programs/Xserver/hw/xwin/X.ico
	@cp $(top_srcdir)/extres/nx.ico xc/programs/xkbcomp/X.ico
	@touch $@

srcdir: checktarball
	@rm -rf xc
	@echo unpacking tarball
	@tar xjf $(TARBALL)
	@touch $@

checktarball: $(TARBALL)
	@echo checking tarball $(TARBALL)
	@md5sum --status -c $(srcdir)/$(TARBALL).md5sum 2>/dev/null
	@touch $@

$(TARBALL):
	@echo downloading tarball $(TARBALL)
	@$(WGET) -O $@ "$(DLURL)"

ccdir:
	@echo creating CROSSCOMPILEDIR
	@rm -rf ccbin include
	@mkdir ccbin
	@for f in /usr/bin/$(target_alias)-* ; do \
		bn=`basename $$f|sed -e 's/$(target_alias)-//'` ; \
		$(LN_S) $$f ccbin/$$bn ; \
	done
	$(LN_S) gcc ccbin/cc
	$(LN_S) $(includedir) include
	@touch $@

