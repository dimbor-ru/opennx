NXESD             = nxesd
NXCOMP            = nxcomp
NXPROXY           = nxproxy
AUDIOFILE         = audiofile
AUDIOFILE_VERSION = 0.2.6
LIBJPEG           = libjpeg
LIBJPEG_VERSION   = turbo-1.1.1
TMPDIST           = $(abs_builddir)/dist

CFG_GENERIC = \
	--prefix=/ --libdir=/lib --bindir=/bin --mandir=/share/man \
	--includedir=/include --datadir=/share --exec-prefix=/libexec \
	--libexecdir=/libexec --sysconfdir=/etc
CFG_audiofile = $(CFG_GENERIC) --disable-shared --enable-static
CFG_nxesd = $(CFG_GENERIC) --disable-audiofiletest --with-audiofile=yes --program-transform-name='s/i686-pc-mingw32-//'
CFG_nxcomp = $(CFG_GENERIC) \
			 --x-includes=$(abs_top_builddir)/packaging/win32/xming/xc/exports/include \
			 --x-libraries=$(abs_top_builddir)/packaging/win32/xming/dist/usr/X11R6/lib
CFG_nxproxy = $(CFG_GENERIC) --program-transform-name='s/i686-pc-mingw32-//'
CFG_nxssh = $(CFG_GENERIC)
CFG_libjpeg = $(CFG_GENERIC) --disable-static --enable-shared
CFG_libpng = $(CFG_GENERIC) --disable-static --enable-shared --with-libpng-compat
CFG_jpeg =  --prefix=$(TMPDIST) --libdir=$(TMPDIST)/lib --bindir=$(TMPDIST)/bin \
	--mandir=$(TMPDIST)/share/man --includedir=$(TMPDIST)/include \
	--datadir=$(TMPDIST)/share --exec-prefix=$(TMPDIST) \
	--libexecdir=$(TMPDIST)/libexec --sysconfdir=$(TMPDIST)/etc --enable-shared --disable-static

ENV_GENERIC = CPPFLAGS="-I$(TMPDIST)/include" LDFLAGS="-L$(TMPDIST)/lib"
ENV_audiofile =
ENV_libpng =
ENV_jpeg =
ENV_nxproxy = env $(ENV_GENERIC) 
ENV_nxcomp = env $(ENV_GENERIC)
ENV_nxesd = env $(ENV_GENERIC) LIBS=-laudiofile

EXTRA_DIST = urls.txt patches $(AUDIOFILE).md5sum $(NXESD).md5sum \
	$(NXCOMP).md5sum $(LIBJPEG).md5sum

CLEANFILES = -r $(TMPDIST) \
	$(AUDIOFILE)-$(AUDIOFILE_VERSION) $(AUDIOFILE)-build-$(NX_BTYPE) \
	$(AUDIOFILE)-srcdir $(AUDIOFILE)-checkarchive \
	$(NXESD) $(NXESD)-build-$(NX_BTYPE) $(NXESD)-srcdir $(NXESD)-checkarchive \
	$(NXCOMP) $(NXCOMP)-build-$(NX_BTYPE) $(NXCOMP)-srcdir $(NXCOMP)-checkarchive \
	$(NXPROXY) $(NXPROXY)-build-$(NX_BTYPE) $(NXPROXY)-srcdir $(NXPROXY)-checkarchive \
	$(LIBJPEG)-$(LIBJPEG_VERSION) $(LIBJPEG)-build-$(NX_BTYPE) $(LIBJPEG)-srcdir $(LIBJPEG)-checkarchive

DISTCLEANFILES = -r $(CLEANFILES) \
	$(LIBJPEG)-$(LIBJPEG_VERSION).tar.gz \
	$(AUDIOFILE)-$(AUDIOFILE_VERSION).tar.gz \
	$(NXESD)*.tar.gz \
	$(NXCOMP)*.tar.gz \
	$(NXPROXY)*.tar.gz

MAINTAINERCLEANFILES = Makefile.in

tmpinstall:
	mkdir -p $(TMPDIST)/man/man1
	$(MAKE) subtarget=$(LIBJPEG) subsrc=$(LIBJPEG)-$(LIBJPEG_VERSION) build-$(NX_BTYPE)
	$(MAKE) subtarget=$(AUDIOFILE) subsrc=$(AUDIOFILE)-$(AUDIOFILE_VERSION) build-$(NX_BTYPE)
	$(MAKE) subtarget=$(NXESD) subsrc=$(NXESD) build-$(NX_BTYPE)
	$(MAKE) subtarget=$(NXCOMP) subsrc=$(NXCOMP) build-$(NX_BTYPE)
	$(MAKE) subtarget=$(NXPROXY) subsrc=$(NXPROXY) build-$(NX_BTYPE)
	rm -f \
		$(TMPDIST)/bin/esd{cat,ctl,filt,loop,mon,play,rec,sample}.exe \
		$(TMPDIST)/bin/[cd]jpeg.exe $(TMPDIST)/bin/jpegtran.exe \
		$(TMPDIST)/bin/jpgtest.exe $(TMPDIST)/bin/libturbojpeg.dll \
		$(TMPDIST)/bin/rdjpgcom.exe $(TMPDIST)/bin/sfconvert.exe \
		$(TMPDIST)/bin/sfinfo.exe $(TMPDIST)/bin/wrjpgcom.exe

build-cross: $(subtarget)-build-cross

build-native: $(subtarget)-build-native

$(subtarget)-build-cross: $(subtarget)-srcdir
	@echo Building $(subtarget)
	@cd $(subsrc) && $(ENV_$(subtarget)) mingw32-configure $(CFG_$(subtarget))
	@$(MAKE) -C $(subsrc) DESTDIR=$(TMPDIST) install
	@touch $@

$(subtarget)-build-native: $(subtarget)-srcdir
	@echo Building $(subtarget)
	@cd $(subsrc) && $(ENV_$(subtarget)) ./configure $(CFG_$(subtarget))
	@$(MAKE) -C $(subsrc) DESTDIR=$(TMPDIST) install
	@touch $@

$(subtarget)-srcdir: $(subtarget)-checkarchive
	@$(RM) -rf $(subsrc)
	@echo Unpacking $(subsrc).tar.gz
	@tar xzf $(subsrc).tar.gz
	@shopt -s nullglob; for p in $(srcdir)/patches/$(subtarget)-*.patch ; do \
		echo Applying `basename $$p`; \
		(cd $(subsrc) && patch -p1 < ../$$p) ; \
	done
	@touch $@

$(subtarget)-checkarchive: $(subsrc).tar.gz
	@echo Checking tarball $(subsrc).tar.gz
	@md5sum --status -c $(srcdir)/$(subtarget).md5sum
	@touch $@

$(subsrc).tar.gz: $(srcdir)/urls.txt
	@echo Downloading $@
	@rm -f $@
	@if grep -q $(subtarget) $(srcdir)/urls.txt ; then \
		$(WGET) -O $@ $(shell grep $(subtarget) $(srcdir)/urls.txt | grep -v '#') ; \
	else \
		$(top_srcdir)/getnxsrcpkg $(subsrc) && $(LN_S) $(subsrc)-*.tar.gz $(subsrc).tar.gz ; \
	fi
	@touch $@
