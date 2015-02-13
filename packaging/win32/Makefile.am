SUBDIRS = xming nx cygnx pulse

FNT1DIR = dejavu-fonts-ttf-2.32
FNT1BALL = $(FNT1DIR).tar.bz2
FNT1URL = http://sourceforge.net/projects/dejavu/files/$(FNT1BALL)
FNT2DIR = font-misc-misc-1.1.0
FNT2BALL = $(FNT2DIR).tar.bz2
FNT2URL = ftp://ftp.gwdg.de/pub/x11/x.org/pub/X11R7.5/src/font/$(FNT2BALL)

.setupdir: $(FNT1DIR) .miscfonts
	$(RM) -rf setupdir
	$(MAKE) -C $(top_builddir) DESTDIR=$(abs_builddir)/setupdir \
		transform= prefix=/ bindir=/bin datadir=/share install
	test $(BUILDXMING) = no || $(MAKE) -C xming tmpinstall
	test $(BUILDXMING) = no || $(INSTALL_PROGRAM) xming/dist/usr/X11R6/bin/*.exe setupdir/bin
	test $(BUILDXMING) = no || $(INSTALL_PROGRAM) xming/dist/usr/X11R6/bin/*.dll setupdir/bin
	test $(BUILDXMING) = no || $(INSTALL) -d setupdir/share/Xming
	test $(BUILDXMING) = no || $(INSTALL_DATA) xming/dist/usr/X11R6/lib/X11/XErrorDB setupdir/share/Xming
	test $(BUILDXMING) = no || $(INSTALL_DATA) xming/dist/usr/X11R6/lib/X11/XKeysymDB setupdir/share/Xming
	test $(BUILDXMING) = no || $(INSTALL_DATA) xming/dist/usr/X11R6/lib/X11/xserver/SecurityPolicy setupdir/share/Xming
	test $(BUILDXMING) = no || cp -r xming/dist/usr/X11R6/lib/X11/locale setupdir/share/Xming/locale
	test $(BUILDXMING) = no || $(INSTALL_DATA) xming/xc/programs/rgb/rgb.txt setupdir/share/Xming
	test $(BUILDXMING) = no || $(INSTALL) -d setupdir/share/Xming/fonts/TTF
	test $(BUILDXMING) = no || $(INSTALL_DATA) $(FNT1DIR)/ttf/* setupdir/share/Xming/fonts/TTF
	test $(BUILDXMING) = no || $(MAKE) -C $(FNT2DIR) DESTDIR=$(abs_builddir)/setupdir install
	$(MAKE) -C $(NXBUILD) tmpinstall
	$(INSTALL_PROGRAM) $(NXBUILD)/dist/bin/*.exe setupdir/bin
	$(INSTALL_PROGRAM) $(NXBUILD)/dist/bin/*.dll setupdir/bin
	$(INSTALL_PROGRAM) $(NXBUILD)/dist/lib/*.dll setupdir/bin
	test $(BUILDXMING) = yes || cp -r $(NXBUILD)/dist/share/* setupdir/share
	$(MAKE) -C pulse tmpdist
	$(INSTALL_PROGRAM) pulse/dist/bin/*.exe setupdir/bin
	$(INSTALL_PROGRAM) pulse/dist/bin/*.dll setupdir/bin
	cp -av pulse/dist/etc setupdir
	cp -av pulse/dist/lib setupdir
	for lang in $(shell cat $(top_srcdir)/po/LINGUAS) ; do \
		$(INSTALL_DATA) $(localedir)/$$lang/LC_MESSAGES/wxstd.mo \
			setupdir/share/locale/$$lang/LC_MESSAGES/ ; \
	done
	$(INSTALL_DATA) $(top_srcdir)/extres/nx.ico setupdir/bin/
	touch $@

.dlldeps: .setupdir findrequires.pl
	env DEPDEBUG=1 perl findrequires.pl "$(DLLPATH)" setupdir/bin/*.{exe,dll} 2>$@|xargs cp -t setupdir/bin
	touch $@

.stripsetup: .setupdir
	$(RM) -rf setupdir/share/icons setupdir/share/applnk
	$(STRIP) --strip-unneeded setupdir/bin/*.exe `ls setupdir/bin/*.dll|grep -v cyg`
	touch $@

nativesetup: .setupdir .dlldeps .stripsetup
	"$(ISCC)" "$(srcdir)/opennx.iss" -dBUILDXMING=$(BUILDXMING)

crosssetup: .setupdir .dlldeps .stripsetup isccwrap.sh
	"$(ISCC)" "$(srcdir)/opennx.iss" -dBUILDXMING=$(BUILDXMING)

setup: $(SETUP)

$(FNT1BALL):
	@echo downloading tarball $(FNT1BALL)
	@$(WGET) "$(FNT1URL)"

$(FNT1DIR): $(FNT1BALL)
	@tar xjf $<

$(FNT2BALL):
	@echo downloading tarball $(FNT2BALL)
	@$(WGET) "$(FNT2URL)"

$(FNT2DIR): $(FNT2BALL)
	@tar xjf $<

.miscfonts: $(FNT2DIR)
	@(cd $(FNT2DIR) && ./configure \
		--with-fontrootdir=/share/Xming/fonts \
		--with-compression=gzip )
	touch $@

EXTRA_DIST = opennx.iss
MAINTAINERCLEANFILES = Makefile.in $(FNT1BALL) $(FNT2BALL)
CLEANFILES = -r .setupdir .dlldeps .stripsetup setupdir $(FNT1DIR) $(FNT2DIR)
