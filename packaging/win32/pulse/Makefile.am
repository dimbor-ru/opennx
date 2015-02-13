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

TMPDIST = $(abs_builddir)/dist
TMPHDR = $(abs_builddir)/include

EXTRA_DIST = urls.txt cfg patches $(LIBSNDFILE).md5sum $(SPEEX).md5sum $(JSONC).md5sum \
	$(LIBSAMPLERATE).md5sum $(PULSEAUDIO).md5sum

CLEANFILES = -r $(TMPDIST) $(TMPHDR) pkg-config \
	$(LIBSAMPLERATE) $(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION) $(LIBSAMPLERATE)-build-$(PULSE_BTYPE) \
	$(LIBSAMPLERATE)-srcdir $(LIBSAMPLERATE)-checkarchive \
	$(LIBSNDFILE) $(LIBSNDFILE)-$(LIBSNDFILE_VERSION) $(LIBSNDFILE)-build-$(PULSE_BTYPE) \
	$(LIBSNDFILE)-srcdir $(LIBSNDFILE)-checkarchive \
	$(JSONC) $(JSONC)-$(JSONC_VERSION) $(JSONC)-build-$(PULSE_BTYPE) \
	$(JSONC)-srcdir $(JSONC)-checkarchive \
	$(SPEEX) $(SPEEX)-$(SPEEX_VERSION) $(SPEEX)-build-$(PULSE_BTYPE) \
	$(SPEEX)-srcdir $(SPEEX)-checkarchive \
	$(PULSEAUDIO) $(PULSEAUDIO)-$(PULSEAUDIO_VERSION) $(PULSEAUDIO)-build-$(PULSE_BTYPE) \
	$(PULSEAUDIO)-srcdir $(PULSEAUDIO)-checkarchive .tmpinstall

MAINTAINERCLEANFILES = Makefile.in \
	$(LIBSNDFILE)-$(LIBSNDFILE_VERSION).tar.gz $(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION).tar.gz \
	$(SPEEX)-$(SPEEX_VERSION).tar.gz $(JSONC)-$(JSONC_VERSION).tar.gz \
	$(PULSEAUDIO)-$(PULSEAUDIO_VERSION).tar.gz

CFG_GENERIC = --prefix=$(TMPDIST) --libdir=$(TMPDIST)/lib --bindir=$(TMPDIST)/bin \
			 --mandir=$(TMPDIST)/share/man --includedir=$(TMPDIST)/include \
			 --datadir=$(TMPDIST)/share --exec-prefix=$(TMPDIST)/libexec \
			 --libexecdir=$(TMPDIST)/libexec --sysconfdir=$(TMPDIST)/etc

CFG_speex = --enable-sse --enable-shared --disable-static
CFG_jsonc = --enable-shared --disable-static --cache-file=mingw32-config.cache
CFG_libsndfile = --enable-shared --disable-static --disable-external-libs
CFG_libsamplerate = --enable-shared --disable-static 
CFG_pulseaudio = --enable-shared --disable-static --disable-glib2 --disable-openssl --disable-nls

PRECFG_speex = true
PRECFG_jsonc = autoreconf -f -i
PRECFG_libsndfile = true
PRECFG_libsamplerate = true
PRECFG_pulseaudio = true

POSTCFG_speex = true
POSTCFG_jsonc = true
POSTCFG_libsndfile = true
POSTCFG_libsamplerate = true
POSTCFG_pulseaudio = true

ENV_speex =
ENV_jsonc =
ENV_libsndfile =
ENV_libsamplerate = env SNDFILE_CFLAGS="-I$(TMPDIST)/include" SNDFILE_LIBS="-L$(TMPDIST)/lib -lsndfile"
ENV_pulseaudio = env LIBSNDFILE_CFLAGS="-I$(TMPDIST)/include" LIBSNDFILE_LIBS="-L$(TMPDIST)/lib -lsndfile" \
	LIBSAMPLERATE_CFLAGS="-I$(TMPDIST)/include" LIBSAMPLERATE_LIBS="-L$(TMPDIST)/lib -lsamplerate" \
	LIBJSON_CFLAGS="-I$(TMPDIST)/include/json" LIBJSON_LIBS="-L$(TMPDIST)/lib -ljson" \
	LIBSPEEX_CFLAGS="-I$(TMPDIST)/include" LIBSPEEX_LIBS="-L$(TMPDIST)/lib -lspeex -lspeexdsp" \
	CPPFLAGS="-I$(TMPDIST)/include" PATH="$(abs_builddir):$(PATH)" LDFLAGS="$(LDFLAGS) -mwindows"

# custom silent rules
vcfg = $(vcfg_$(V))
vcfg_ = $(vcfg_$(AM_DEFAULT_VERBOSITY))
vcfg_0 = @echo CONFIGURE $(subtarget);
vbld = $(vbld_$(V))
vbld_ = $(vbld_$(AM_DEFAULT_VERBOSITY))
vbld_0 = @echo BUILD $(subtarget);
vpatch = $(vpatch_$(V))
vpatch_ = $(vpatch_$(AM_DEFAULT_VERBOSITY))
vpatch_0 = @echo PATCH $(subtarget);
vunpack = $(vunpack_$(V))
vunpack_ = $(vunpack_$(AM_DEFAULT_VERBOSITY))
vunpack_0 = @echo UNPACK $(subsrc).tar.gz;
vcheck = $(vcheck_$(V))
vcheck_ = $(vcheck_$(AM_DEFAULT_VERBOSITY))
vcheck_0 = @echo VERIFYING $(subsrc).tar.gz;
vfetch = $(vfetch_$(V))
vfetch_ = $(vfetch_$(AM_DEFAULT_VERBOSITY))
vfetch_0 = @echo DOWNLOAD $(subtarget);

tmpinstall:
	$(MAKE) subtarget=$(SPEEX) subvar=$(SPEEX) subsrc=$(SPEEX)-$(SPEEX_VERSION) build-$(PULSE_BTYPE)
	$(MAKE) subtarget=$(JSONC) subvar=jsonc subsrc=$(JSONC)-$(JSONC_VERSION) build-$(PULSE_BTYPE)
	$(MAKE) subtarget=$(LIBSNDFILE) subvar=$(LIBSNDFILE) subsrc=$(LIBSNDFILE)-$(LIBSNDFILE_VERSION) build-$(PULSE_BTYPE)
	$(MAKE) subtarget=$(LIBSAMPLERATE) subvar=$(LIBSAMPLERATE) subsrc=$(LIBSAMPLERATE)-$(LIBSAMPLERATE_VERSION) build-$(PULSE_BTYPE)
	@echo "#!/bin/sh" > pkg-config; \
		echo "case \$$* in *--exists*) exit 1 ;; *) exit 0 ;; esac" >> pkg-config; \
		chmod a+x pkg-config || true
	$(MAKE) subtarget=$(PULSEAUDIO) subvar=$(PULSEAUDIO) subsrc=$(PULSEAUDIO)-$(PULSEAUDIO_VERSION) build-$(PULSE_BTYPE)
	mkdir -p $(TMPHDR)
	cp -a $(TMPDIST)/include/* $(TMPHDR)

.tmpinstall:
	$(MAKE) tmpinstall
	touch $@

tmpdist: .tmpinstall
	rm -rf $(TMPDIST)/share  $(TMPDIST)/libexec $(TMPDIST)/include $(TMPDIST)/lib/pkgconfig
	-mv $(TMPDIST)/lib/pulse-1.0/bin/*.dll $(TMPDIST)/bin
	rm -rf $(TMPDIST)/lib/pulse-1.0/bin $(TMPDIST)/lib/pulse-1.0/lib/cmake $(TMPDIST)/bin/esdcompat
	find $(TMPDIST) -name "*.la" -o -name "*.a"|xargs rm -f
	rm -f $(TMPDIST)/bin/*-sndfile-*.exe $(TMPDIST)/bin/speex*.exe
	cp -av cfg/* $(TMPDIST)/etc/pulse/

build-cross: $(subtarget)-build-cross

build-native: $(subtarget)-build-native

$(subtarget)-build-cross: $(subtarget)-srcdir
	$(vcfg)cd $(subsrc) && $(PRECFG_$(subvar)) && $(ENV_$(subvar)) mingw32-configure $(CFG_GENERIC) $(CFG_$(subvar))
	@$(POSTCFG_$(subvar))
	$(vbld)$(MAKE) V=$(V) -C $(subsrc) install
	@touch $@

$(subtarget)-build-native: $(subtarget)-srcdir
	$(vcfg)cd $(subsrc) && $(ENV_$(subvar)) ./configure $(CFG_GENERIC) $(CFG_$(subvar))
	$(POSTCFG_$(subvar))
	$(vbld)$(MAKE) V=$(V) -C $(subsrc) install
	@touch $@

$(subtarget)-patch:
	$(vpatch)shopt -s nullglob; for p in $(srcdir)/patches/$(subvar)-*.patch ; do \
		test "$(V)" = "0" || echo Applying `basename $$p`; \
		patch -p0 < $$p ; \
	done

$(subtarget)-srcdir: $(subtarget)-checkarchive
	@$(RM) -rf $(subsrc)
	$(vunpack)tar xzf $(subsrc).tar.gz
	@test -n "$(NOPATCH_$(subtarget))" || $(MAKE) $(subtarget)-patch
	@touch $@

$(subtarget)-checkarchive: $(subsrc).tar.gz
	$(vcheck)test -n "$(NOCHECK_$(subtarget))" || md5sum --status -c $(srcdir)/$(subtarget).md5sum
	@touch $@

$(subsrc).tar.gz: $(srcdir)/urls.txt
	$(vfetch)test -f $@ || $(WGET) -nc $(shell grep $(subsrc).tar.gz $(srcdir)/urls.txt)
	@touch $@
