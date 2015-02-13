PALIBTOOL             = libtool
PALIBTOOL_VERSION     = 2.4
LIBSNDFILE            = libsndfile
LIBSNDFILE_VERSION    = 1.0.24
LIBSAMPLERATE         = libsamplerate
LIBSAMPLERATE_VERSION = 0.1.7
JSONC				  = json-c
JSONC_VERSION		  = 0.9
SPEEX                 = speex
SPEEX_VERSION         = 1.2rc1
PULSEAUDIO            = pulseaudio
PULSEAUDIO_VERSION    = 1.0-fe-20111209
PULSEAUDIO_SVER       = 1.0

TMPDIST = $(abs_builddir)/dist
TMPHDR = $(abs_builddir)/include

# For caching pre-built binaries on the build slave
CACHE_FLAG = $(HOME)/.docache_pulse
CACHE_DIR = $(HOME)/savebuild
CACHE_TGZ = $(CACHE_DIR)/pulse.tgz

EXTRA_DIST = urls.txt cfg patches $(LIBSNDFILE).shasum $(SPEEX).shasum $(JSONC).shasum \
	$(LIBSAMPLERATE).shasum $(PULSEAUDIO).shasum

CLEANFILES = -r $(TMPDIST) $(TMPHDR) pkg-config tmpinstall .*-build .*-srcdir .*-checkarchive .*-lipo \
	$(LIBSAMPLERATE) $(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION) \
	$(LIBSNDFILE) $(LIBSNDFILE)-$(LIBSNDFILE_VERSION) \
	$(JSONC) $(JSONC)-$(JSONC_VERSION) \
	$(PALIBTOOL) $(PALIBTOOL)-$(PALIBTOOL_VERSION) \
	$(SPEEX) $(SPEEX)-$(SPEEX_VERSION) \
	$(PULSEAUDIO) $(PULSEAUDIO)-$(PULSEAUDIO_VERSION)

DISTCLEANFILES = *.tar.gz

MAINTAINERCLEANFILES = Makefile.in

CFG_GENERIC = --prefix=$(prefix) --enable-shared --disable-static --disable-dependency-tracking

CFG_libtool =
CFG_speexLE = --enable-sse --without-ogg
CFG_speexBE = --libdir=$(libdir)/ppc --disable-oggtest --without-ogg --host ppc-apple-darwin10
CFG_jsonc =
CFG_libsndfileLE = --disable-external-libs
CFG_libsndfileBE = --disable-external-libs --libdir=$(libdir)/ppc --host ppc-apple-darwin10
CFG_libsamplerateLE =
CFG_libsamplerateBE = --libdir=$(libdir)/ppc --host ppc-apple-darwin10
CFG_pulseaudioLE = --disable-glib2 --disable-openssl --disable-nls --with-udev-rules-dir=$(prefix)/lib \
	--with-mac-version-min=$(OSX_MINVER) --with-mac-sysroot=$(OSX_SYSROOT) --program-suffix=.i386
CFG_pulseaudioBE = --disable-glib2 --disable-openssl --disable-nls --with-udev-rules-dir=$(prefix)/lib \
	--with-mac-version-min=$(OSX_MINVER) --with-mac-sysroot=$(OSX_SYSROOT) --libdir=$(libdir)/ppc --host ppc-apple-darwin10 --program-suffix=.ppc

PRECFG_libtool = true
PRECFG_speex = true
PRECFG_jsonc = autoreconf -f -i
PRECFG_libsndfile = true
PRECFG_libsamplerate = true
PRECFG_pulseaudio = true

POSTCFG_libtool = true
POSTCFG_speex = true
POSTCFG_jsonc = true
POSTCFG_libsndfile = true
POSTCFG_libsamplerate = true
POSTCFG_pulseaudio = true

MACENV_UNIVERSAL = CC="$(CC)" CXX="$(CXX)" LD="$(LD)" \
	 CFLAGS="$(UNIFLAGS) $(OSX_SDKFLAGS) -I$(TMPDIST)/include" \
	 CXXFLAGS="$(UNIFLAGS) $(OSX_SDKFLAGS)" CPPFLAGS="-I$(TMPDIST)$(includedir)"
MACENV_INTEL = CC="$(CC)" CXX="$(CXX)" LD="$(LD)" \
	 CFLAGS="-arch i386 $(OSX_SDKFLAGS)" \
	 CXXFLAGS="-arch i386 $(OSX_SDKFLAGS)" CPPFLAGS="-I$(TMPDIST)$(includedir)"
MACENV_PPC = CC="$(CC)" CXX="$(CXX)" LD="$(LD)" \
	 CFLAGS="-arch ppc $(OSX_SDKFLAGS)" \
	 CXXFLAGS="-arch ppc $(OSX_SDKFLAGS)" CPPFLAGS="-I$(TMPDIST)$(includedir)"

ENV_libtool = env $(MACENV_UNIVERSAL)
ENV_speexLE = env $(MACENV_INTEL)
ENV_speexBE = env $(MACENV_PPC) OGG_LIBS= OGG_CFLAGS=
ENV_jsonc = env $(MACENV_UNIVERSAL)
ENV_libsndfileLE = env $(MACENV_INTEL)
ENV_libsndfileBE = env $(MACENV_PPC)
ENV_libsamplerateLE = env $(MACENV_INTEL) \
	SNDFILE_CFLAGS="-I$(TMPDIST)$(includedir)" SNDFILE_LIBS="-L$(TMPDIST)$(libdir) -lsndfile"
ENV_libsamplerateBE = env $(MACENV_PPC) \
	SNDFILE_CFLAGS="-I$(TMPDIST)$(includedir)" SNDFILE_LIBS="-L$(TMPDIST)$(libdir) -lsndfile"
ENV_pulseaudioLE = env $(MACENV_INTEL) CC="gcc-4.2" CXX="g++-4.2" \
	LDFLAGS="-L$(TMPDIST)$(libdir)" \
	LIBSNDFILE_CFLAGS="-I." LIBSNDFILE_LIBS="-lsndfile" \
	LIBSAMPLERATE_CFLAGS="-I." LIBSAMPLERATE_LIBS="-lsamplerate" \
	LIBJSON_CFLAGS="-I$(TMPDIST)$(includedir)/json" LIBJSON_LIBS="-ljson" \
	LIBSPEEX_CFLAGS="-I." LIBSPEEX_LIBS="-lspeex -lspeexdsp" \
	PATH="$(abs_builddir):$(PATH)"
ENV_pulseaudioBE = env $(MACENV_PPC) CC="gcc-4.2" CXX="g++-4.2" \
	LDFLAGS="-L$(TMPDIST)$(libdir)" \
	LIBSNDFILE_CFLAGS="-I." LIBSNDFILE_LIBS="-lsndfile" \
	LIBSAMPLERATE_CFLAGS="-I." LIBSAMPLERATE_LIBS="-lsamplerate" \
	LIBJSON_CFLAGS="-I$(TMPDIST)$(includedir)/json" LIBJSON_LIBS="-ljson" \
	LIBSPEEX_CFLAGS="-I." LIBSPEEX_LIBS="-lspeex -lspeexdsp" \
	PATH="$(abs_builddir):$(PATH)"

upinstall: cachedinstall
	cp -av $(TMPDIST)/ $(PKGROOT)/

cachedinstall:
	test -f $(CACHE_TGZ) && tar xzf $(CACHE_TGZ) || true
	test -f $(CACHE_TGZ) || $(MAKE) tmpinstall

tmpinstall:
	$(MAKE) ecfg= subtarget=$(PALIBTOOL) subvar=$(PALIBTOOL) subsrc=$(PALIBTOOL)-$(PALIBTOOL_VERSION) tbuild
	$(MAKE) ecfg= subtarget=$(JSONC) subvar=jsonc subsrc=$(JSONC)-$(JSONC_VERSION) tbuild
	mkdir -p $(TMPDIST)$(libdir)/ppc
	$(MAKE) ecfg=LE subtarget=$(SPEEX) subvar=$(SPEEX) subsrc=$(SPEEX)-$(SPEEX_VERSION) tbuild
	$(MAKE) -C $(SPEEX)-$(SPEEX_VERSION) distclean
	rm -f .$(SPEEX)-build
	$(MAKE) ecfg=BE subtarget=$(SPEEX) subvar=$(SPEEX) subsrc=$(SPEEX)-$(SPEEX_VERSION) tbuild
	$(MAKE) ecfg=BE subtarget=$(SPEEX) subvar=$(SPEEX) subsrc=$(SPEEX)-$(SPEEX_VERSION) tlipo
	$(MAKE) ecfg=LE subtarget=$(LIBSNDFILE) subvar=$(LIBSNDFILE) subsrc=$(LIBSNDFILE)-$(LIBSNDFILE_VERSION) tbuild
	$(MAKE) -C $(LIBSNDFILE)-$(LIBSNDFILE_VERSION) distclean
	rm -f .$(LIBSNDFILE)-build
	$(MAKE) ecfg=BE subtarget=$(LIBSNDFILE) subvar=$(LIBSNDFILE) subsrc=$(LIBSNDFILE)-$(LIBSNDFILE_VERSION) tbuild
	$(MAKE) ecfg=BE subtarget=$(LIBSNDFILE) subvar=$(LIBSNDFILE) subsrc=$(LIBSNDFILE)-$(LIBSNDFILE_VERSION) tlipo
	$(MAKE) ecfg=LE subtarget=$(LIBSAMPLERATE) subvar=$(LIBSAMPLERATE) subsrc=$(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION) tbuild
	$(MAKE) -C $(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION) distclean
	rm -f .$(LIBSAMPLERATE)-build
	$(MAKE) ecfg=BE subtarget=$(LIBSAMPLERATE) subvar=$(LIBSAMPLERATE) subsrc=$(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION) tbuild
	$(MAKE) ecfg=BE subtarget=$(LIBSAMPLERATE) subvar=$(LIBSAMPLERATE) subsrc=$(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION) tlipo
	@echo "#!/bin/sh" > pkg-config; \
		echo "case \$$* in *--exists*) exit 1 ;; *) exit 0 ;; esac" >> pkg-config; \
		chmod a+x pkg-config || true
	$(MAKE) ecfg=LE subtarget=$(PULSEAUDIO) subvar=$(PULSEAUDIO) subsrc=$(PULSEAUDIO)-$(PULSEAUDIO_VERSION) tbuild
	$(MAKE) -C $(PULSEAUDIO)-$(PULSEAUDIO_VERSION) distclean
	rm -f .$(PULSEAUDIO)-build
	$(MAKE) ecfg=BE subtarget=$(PULSEAUDIO) subvar=$(PULSEAUDIO) subsrc=$(PULSEAUDIO)-$(PULSEAUDIO_VERSION) tbuild
	$(MAKE) ecfg=BE subtarget=$(PULSEAUDIO) subvar=$(PULSEAUDIO) subsrc=$(PULSEAUDIO)-$(PULSEAUDIO_VERSION) modsub=/pulse-$(PULSEAUDIO_SVER)/modules tlipo
	rmdir $(TMPDIST)$(libdir)/ppc
	mkdir -p $(TMPHDR)
	cp -a $(TMPDIST)$(prefix)/include/* $(TMPHDR)
	rm -rf $(TMPDIST)$(prefix)/{share,libexec,include} $(TMPDIST)${libdir}/pkgconfig $(TMPDIST)${libdir}/cmake
	find $(TMPDIST) -name "*.la" -o -name "*.a"|xargs rm -f
	rm -f $(TMPDIST)$(bindir)/sndfile-* $(TMPDIST)$(bindir)/libtool*
	cp $(srcdir)/cfg/* $(TMPDIST)$(prefix)/etc/pulse/
	test -f $(CACHE_FLAG) && tar czf $(CACHE_TGZ) dist include || true
	touch $@

tbuild: .$(subtarget)-build

tlipo: .$(subtarget)-lipo

.$(subtarget)-lipo: .$(subtarget)-build
	@echo Merging $(subtarget) to universal
	@for f in $(TMPDIST)$(libdir)/ppc/*.dylib ; do \
		bn=`basename $$f` ; \
		test -L $$f || mv $(TMPDIST)$(libdir)/$$bn $(TMPDIST)$(libdir)/i386-$$bn ; \
		test -L $$f || lipo -create -output $(TMPDIST)$(libdir)/$$bn $$f $(TMPDIST)$(libdir)/i386-$$bn ; \
		rm -f $(TMPDIST)$(libdir)/i386-$$bn ; \
	done
	@if test -n "$(modsub)" ; then \
		echo Merging $(subtarget) modules to universal ; \
		for f in $(TMPDIST)$(libdir)/ppc$(modsub)/*.{dylib,so} ; do \
			bn=`basename $$f` ; \
			mv $(TMPDIST)$(libdir)$(modsub)/$$bn $(TMPDIST)$(libdir)$(modsub)/i386-$$bn ; \
			lipo -create -output $(TMPDIST)$(libdir)$(modsub)/$$bn \
				$$f $(TMPDIST)$(libdir)$(modsub)/i386-$$bn ; \
			rm $(TMPDIST)$(libdir)$(modsub)/i386-$$bn ; \
		done ; \
		echo Merging $(subtarget) programs to universal ; \
		for f in $(TMPDIST)$(bindir)/*.i386 ; do \
			bn=`basename $$f .i386` ; \
			file $$f|grep -q shell || lipo -create -output $(TMPDIST)$(bindir)/$$bn $$f $(TMPDIST)$(bindir)/$$bn.ppc ; \
			file $$f|grep -q shell && mv $$f $(TMPDIST)$(bindir)/$$bn || true ; \
			rm -f $$f $(TMPDIST)$(bindir)/$$bn.ppc ; \
		done ; \
	fi
	@rm -rf $(TMPDIST)$(libdir)/ppc/*
	@touch $@

.$(subtarget)-build: .$(subtarget)-srcdir
	@echo Configuring $(subtarget)
	cd $(subsrc) && $(PRECFG_$(subvar)) && $(ENV_$(subvar)$(ecfg)) ./configure $(CFG_GENERIC) $(CFG_$(subvar)$(ecfg))
	@$(POSTCFG_$(subvar))
	@echo building $(subtarget)
	@$(MAKE) V=$(V) DESTDIR=$(TMPDIST) -C $(subsrc) install
	@touch $@

.$(subtarget)-patch:
	@echo Applying patches for $(subtarget)
	@shopt -s nullglob; for p in $(srcdir)/patches/$(subvar)-*.patch ; do \
		test $(V) = 0 || echo Applying `basename $$p`; \
		( cd $(subsrc) && patch -p1 < ../$$p ) ; \
	done

.$(subtarget)-srcdir: .$(subtarget)-checkarchive
	@$(RM) -rf $(subsrc)
	@echo Unpacking $(subtarget)
	@tar xzf $(subsrc).tar.gz
	@test -n "$(NOPATCH_$(subtarget))" || $(MAKE) .$(subtarget)-patch
	@touch $@

.$(subtarget)-checkarchive: $(subsrc).tar.gz
	@echo Verifying $(subtarget)
	@test -n "$(NOCHECK_$(subtarget))" || shasum --status -c $(srcdir)/$(subtarget).shasum
	@touch $@

$(subsrc).tar.gz: $(srcdir)/urls.txt
	@echo Downloading $(subtarget)
	@test -f $@ || $(CURL) -f -L -o $(subsrc).tar.gz $(shell grep $(subsrc).tar.gz $(srcdir)/urls.txt)
	@touch $@
