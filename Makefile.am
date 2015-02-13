AM_LIBTOOLFLAGS = --silent
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po docs $(PLATFORM_SUBDIRS)
DIST_SUBDIRS = po docs packaging/win32 packaging/MacOSX

noinst_PROGRAMS = bin2hdr pulsetest $(TESTXAUTH) $(MACTESTKBD)

bin2hdr_SOURCES = bin2hdr.cpp
bin2hdr_LDFLAGS = -all-static

bin_PROGRAMS = opennx pconnect $(TRACELOG) $(WATCHREADER) $(WATCHUSBIP) \
	       $(MACFINDREADER) $(MACUNINSTALL)

EXTRA_PROGRAMS = tracelog$(EXEEXT) watchreader$(EXEEXT) \
	watchusbip$(EXEEXT) macfindreader$(EXEEXT) macuninstall$(EXEEXT) \
	mactestkbd$(EXEEXT) testxauth$(EXEEXT)

opennx_SOURCES = AboutDialog.cpp AsyncProcess.cpp LibOpenSC.cpp \
	CardWaiterDialog.cpp ConnectDialog.cpp ExtHtmlWindow.cpp \
	ForeignFrame.cpp Icon.cpp LogDialog.cpp KeyDialog.cpp LibUSB.cpp \
	LoginDialog.cpp mylog.cpp md5.c MyIPC.cpp MyDynlib.cpp MySession.cpp \
	MyValidator.cpp MyWizard.cpp MyXmlConfig.cpp opennxApp.cpp \
	osdep.c PanicDialog.cpp pwcrypt.cpp ProxyPasswordDialog.cpp \
	PulseAudio.cpp QuitDialog.cpp RdpImageSettingsDialog.cpp \
	RdpPropertyDialog.cpp ResumeDialog.cpp ProxyPropertyDialog.cpp \
	SessionAdmin.cpp SessionList.cpp SessionProperties.cpp \
	SimpleXauth.cpp ShareProperties.cpp TextViewer.cpp \
	UnixImageSettingsDialog.cpp UsbFilterDetailsDialog.cpp \
	UsbIp.cpp VncImageSettingsDialog.cpp VncPropertyDialog.cpp \
	wxprec.cpp WinShare.cpp X11PropertyDialog.cpp xh_richtext.cpp \
	MacOSX.c WrappedStatic.cpp SupressibleMessageDialog.cpp \
	XdmPropertyDialog.cpp

noinst_HEADERS = AboutDialog.h AboutDialog_symbols.h AsyncProcess.h \
	LibOpenSC.h CardWaiterDialog.h CardWaiterDialog_symbols.h \
	ConnectDialog.h ConnectDialog_symbols.h debugleak.h \
	DebugStringGrabber.h ExtHtmlWindow.h ForeignFrame.h \
	ForeignFrame_symbols.h Icon.h KeyDialog.h KeyDialog_symbols.h \
	LibUSB.h LogDialog.h LogDialog_symbols.h LoginDialog.h mylog.h \
	LoginDialog_symbols.h md5.h memres.h MyIPC.h MyDynlib.h MySession.h \
	MyValidator.h MyWizard.h MyWizard_symbols.h MyXmlConfig.h opennxApp.h \
	osdep.h  PanicDialog.h PanicDialog_symbols.h PulseAudio.h pwcrypt.h \
	QuitDialog.h QuitDialog_symbols.h ProxyPasswordDialog.h \
	ProxyPasswordDialog_symbols.h ProxyPropertyDialog.h pulseTest.h \
	ProxyPropertyDialog_symbols.h RdpImageSettingsDialog.h \
	RdpImageSettingsDialog_symbols.h RdpPropertyDialog.h \
	RdpPropertyDialog_symbols.h ResumeDialog.h ResumeDialog_symbols.h \
	SessionAdmin.h SessionAdmin_symbols.h SessionList.h SimpleXauth.h \
	SessionProperties.h SessionProperties_symbols.h ShareProperties.h \
	ShareProperties_symbols.h TextViewer.h TextViewer_symbols.h \
	TraceLogFrame.h TraceLogFrame_symbols.h UnixImageSettingsDialog.h \
	UnixImageSettingsDialog_symbols.h UsbFilterDetailsDialog.h \
	UsbFilterDetailsDialog_symbols.h UsbIp.h \
	VncImageSettingsDialog.h VncImageSettingsDialog_symbols.h \
	VncPropertyDialog.h VncPropertyDialog_symbols.h WinShare.h \
	X11PropertyDialog.h X11PropertyDialog_symbols.h trace.h tracelogApp.h \
	watchReaderApp.h watchUsbIpApp.h xh_richtext.h MacUninstallApp.h \
	WrappedStatic.h SupressibleMessageDialog.h SupressibleMessageDialog_symbols.h \
	TraceLogRemote_symbols.h TraceLogRemote.h \
	opensc/errors.h opensc/opensc.h opensc/scconf.h opensc/types.h opensc/pkcs15.h \
	libsmbclient.h \
	XdmPropertyDialog.h XdmPropertyDialog_symbols.h

pconnect_SOURCES = pconnect.c
pconnect_LDADD = $(WS2LIB) $(RESLIB)

pulsetest_SOURCES = pulseTest.cpp PulseAudio.cpp MyDynlib.cpp mylog.cpp osdep.c MacOSX.c
pulsetest_LDADD = $(WX_LIBS) $(X_LIBS) $(X_PRE_LIBS) $(X_EXTRA_LIBS) $(APP_LDADD)
pulsetest_CPPFLAGS = $(WX_CPPFLAGS) $(PLATFORM_INCLUDES) -DAPP_PULSETEST
pulsetest_DEPENDENCIES = $(OPENNX_RES) $(PLATFORM_DEPENDENCIES)
pulsetest_CXXFLAGS = $(WX_CXXFLAGS)
pulsetest_CFLAGS = $(WX_CFLAGS)


testxauth_SOURCES = testxauth.cpp SimpleXauth.cpp pwcrypt.cpp md5.c
testxauth_LDADD = $(WX_LIBS)
testxauth_CPPFLAGS = $(WX_CPPFLAGS) -DAPP_TESTXAUTH
testxauth_CXXFLAGS = $(WX_CXXFLAGS)
testxauth_CFLAGS = $(WX_CFLAGS)

watchreader_SOURCES = watchReaderApp.cpp LibOpenSC.cpp MyDynlib.cpp MacOSX.c osdep.c mylog.cpp
watchreader_DEPENDENCIES = $(WATCHREADER_RES)
watchreader_LDADD = $(WATCHREADER_RES) $(WX_LIBS) $(X_LIBS) \
	$(X_PRE_LIBS) $(X_EXTRA_LIBS) $(APP_LDADD)
watchreader_CPPFLAGS = $(WX_CPPFLAGS) $(X_CFLAGS) -DAPP_WATCHREADER
watchreader_CXXFLAGS = $(WX_CXXFLAGS)
watchreader_CFLAGS = $(WX_CFLAGS)

watchusbip_SOURCES = watchUsbIpApp.cpp UsbIp.cpp Icon.cpp LibUSB.cpp \
	md5.c MyValidator.cpp MyXmlConfig.cpp osdep.c pwcrypt.cpp mylog.cpp \
	UsbFilterDetailsDialog.cpp wxprec.cpp xh_richtext.cpp MyDynlib.cpp
watchusbip_LDADD = $(WX_LIBS) $(X_LIBS) $(X_PRE_LIBS) $(X_EXTRA_LIBS) \
	$(APP_LDADD)
watchusbip_CPPFLAGS = $(WX_CPPFLAGS) $(X_CFLAGS) -DAPP_WATCHUSBIP
watchusbip_CXXFLAGS = $(WX_CXXFLAGS)
watchusbip_CFLAGS = $(WX_CFLAGS)

tracelog_SOURCES = tracelogApp.cpp TraceLogFrame.cpp \
	DebugStringGrabber.cpp wxprec.cpp Icon.cpp TraceLogRemote.cpp
tracelog_LDADD = $(TRACELOG_RES) $(WX_LIBS)
tracelog_DEPENDENCIES = $(TRACELOG_RES)
tracelog_CPPFLAGS = $(WX_CPPFLAGS) -DAPP_TRACELOG
tracelog_CXXFLAGS = $(WX_CXXFLAGS)
tracelog_CFLAGS = $(WX_CFLAGS)

macfindreader_SOURCES = macfindreader.c
macfindreader_CFLAGS = $(UNIFLAGS) $(OSX_SDKFLAGS)
macfindreader_LDADD = -ldl

macuninstall_SOURCES = MacUninstallApp.cpp MacOSX.c
macuninstall_LDADD = $(WX_LIBS)
macuninstall_LDFLAGS = -framework Security
macuninstall_CPPFLAGS = $(WX_CPPFLAGS) -DAPP_MACUNINSTALL
macuninstall_CXXFLAGS = $(WX_CXXFLAGS)
macuninstall_CFLAGS = $(WX_CFLAGS)

mactestkbd_SOURCES = mactestkbd.c MacOSX.c
mactestkbd_LDADD = $(WX_LIBS)
mactextkbd_CPPFLAGS = $(WX_CPPFLAGS) -DAPP_MACTESTKBD
mactestkbd_CFLAGS = $(WX_CFLAGS) -DAPP_MACTESTKBD

EXTRA_HEADERS = res/opennx.rc

EXTRA_opennx_SOURCES = opennx.rc
EXTRA_tracelog_SOURCES = tracelog.rc
EXTRA_watchreader_SOURCES = watchreader.rc

opennx_LDADD = $(OPENNX_RES) $(WX_LIBS) $(X_LIBS) $(X_PRE_LIBS) \
	$(X_EXTRA_LIBS) $(APP_LDADD) $(LIBCURL)
opennx_DEPENDENCIES = $(OPENNX_RES) $(PLATFORM_DEPENDENCIES)
opennx_CPPFLAGS = $(WX_CPPFLAGS) $(PLATFORM_INCLUDES) $(X_CFLAGS) -DAPP_OPENNX
opennx_CXXFLAGS = $(WX_CXXFLAGS)
opennx_CFLAGS = $(WX_CFLAGS)


GETTEXTIZE = $(shell which gettextize)

bootstrap:
#   Update ChangeLog from SVN
	svn2cl -i --break-before-msg --authors=conf/authors.xml
# 	Convince gettextize not to modify EXTRA_DIST
	test -f conf/config.rpath || touch conf/config.rpath
# 	Convince gettextize not to modify AC_CONFIG_FILES
	test -f po/Makefile.in.in || touch po/Makefile.in.in
#	Tweak gettextize
#	Stupid gettextize uses /dev/tty for interactively getting a
#	confirm of it's "oh so incredibly important notes".
#	YES, i've read them at least a dozen times and now, i REALLY
#	don't want to hit RETURN anymore!
	test -n "$(GETTEXTIZE)" && \
		sed -e 's@/dev/tty@/dev/null@' "$(GETTEXTIZE)" | bash -s -- -f --no-changelog
	test -e conf/mkinstalldirs || touch conf/mkinstalldirs
	autoreconf -if

bundle:
	$(srcdir)/packaging/MacOSX/mkbundle OpenNX.app OpenNXapp \
	    $(srcdir)/extres/nx.icns $(abs_builddir)/opennx \
	    $(builddir)/packaging/MacOSX/opennx-main.plist "OPNX"
	$(INSTALL) -m 0644 $(srcdir)/extres/nx-desktop.icns OpenNX.app/Contents/Resources
	$(srcdir)/packaging/MacOSX/mkbundle OpenNXAdmin.app OpenNXAdmin \
	    $(srcdir)/extres/opennx-admin.icns $(abs_builddir)/opennx \
	    $(builddir)/packaging/MacOSX/opennx.plist "" --admin
	$(srcdir)/packaging/MacOSX/mkbundle OpenNXWizard.app OpenNXWizard \
	    $(srcdir)/extres/opennx-wizard.icns $(abs_builddir)/opennx \
	    $(builddir)/packaging/MacOSX/opennx.plist "" --wizard
	$(srcdir)/packaging/MacOSX/mkbundle "Uninstall OpenNX.app" "" \
	    $(srcdir)/extres/opennx-uninstall.icns $(abs_builddir)/macuninstall \
	    $(builddir)/packaging/MacOSX/opennx.plist ""
	$(srcdir)/packaging/MacOSX/mkbundle Message.app OpenNXMessage \
	    $(srcdir)/extres/nx.icns $(abs_builddir)/opennx \
	    $(builddir)/packaging/MacOSX/opennx.plist "" \
	    --dialog ok --style info --caption CARDREMOVED --message CARDREMOVED

all-local: $(BUNDLE)

clean-local:
	rm -rf OpenNX.app OpenNXAdmin.app OpenNXWizard.app Message.app \
	    "Uninstall OpenNX.app"

distclean-local:
	rm -f po/stamp-po

install-wxstdmo-local:
	for lang in $(shell cat $(top_srcdir)/po/LINGUAS) ; do \
		$(INSTALL) -d $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES ; \
		$(INSTALL_DATA) $(WX_PREFIX)/share/locale/$$lang/LC_MESSAGES/wxstd.mo \
			$(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/ ; \
	done

install-data-local: $(STATIC_LOCAL_INSTALLDATA_TARGET)

RESDEPS = $(wildcard $(srcdir)/res/*.xrc $(srcdir)/res/*.png \
		  $(srcdir)/res/*.html $(srcdir)/res/*.xml)
RESSRC = $(subst $(srcdir)/,,$(RESDEPS))

tmpres.zip: $(RESDEPS)
	$(AM_V_GEN)cd $(srcdir) && zip -q9r $(abs_builddir)/$(@F) $(RESSRC)

memres.h: tmpres.zip bin2hdr$(EXEEXT)
	$(AM_V_GEN)./bin2hdr$(EXEEXT) tmpres.zip memres.h mem_res

$(top_srcdir)/opennxApp.cpp: memres.h

opennxApp.cpp: memres.h

tracelogApp.cpp: memres.h

watchUsbIpApp.cpp: memres.h

PulseAudio.cpp: $(PAHEADER)

SUFFIXES = .rc

.rc.o: $(OSRESDEPS)
	$(AM_V_GEN)$(WX_RESCOMP) -I$(top_builddir) -o $@ $<

CLEANFILES = memres.h tmpres.zip *~ po/*~ *.bak *~ \#.* .\#* OpenNX.app

MAINTAINERCLEANFILES = ABOUT-NLS Makefile.in configure aclocal.m4 \
	conf/config.guess conf/config.sub conf/depcomp conf/install-sh \
	conf/missing conf/mkinstalldirs conf/ltmain.sh conf/config.rpath \
	m4/*~ m4/[cgilnpuwx]*.m4 m4/s[it]*.m4 \
	po/*.sin po/*.sed po/*.header po/Rules-quot po/*.template po/Makefile*.in

DISTCLEANFILES = mingw32-config.cache res/*.prv.*

man_MANS = docs/pconnect.1

data_DATA = share/keyboards

keydir = $(datadir)/keys
key_DATA = $(top_srcdir)/share/keys/*

cupsdir = $(datadir)/cups
cups_DATA = $(top_srcdir)/share/cups/*

install-data-local:
	$(MAKE) -C po stamp-po install-data-yes
	$(INSTALL) -d $(DESTDIR)$(datadir)/icons
	cd $(top_srcdir)/extres && \
		for d in 16x16 32x32 48x48 128x128 256x256 512x512 scalable ; do \
			$(INSTALL) -d $(DESTDIR)$(datadir)/icons/$$d ; \
			( \
				cd $$d && \
				for subd in * ; do \
					$(INSTALL) -d $(DESTDIR)$(datadir)/icons/$$d/$$subd ; \
					for f in $$subd/* ; do \
						$(INSTALL_DATA) $$f $(DESTDIR)$(datadir)/icons/$$d/$$subd ; \
					done \
				done \
			) \
		done
	$(INSTALL) -d $(DESTDIR)$(datadir)/applnk/xdg
	cd $(top_srcdir)/extres/xdg && \
	for d in * ; do \
		$(INSTALL_DATA) $$d $(DESTDIR)$(datadir)/applnk/xdg ; \
	done

uninstall-local:
	cd $(top_srcdir)/extres/xdg && \
	for d in * ; do \
		$(RM) -r $(DESTDIR)$(datadir)/applnk/xdg/$$d ; \
	done
	-rmdir $(DESTDIR)$(datadir)/applnk/xdg
	-rmdir $(DESTDIR)$(datadir)/applnk
	cd $(top_srcdir)/extres && \
		for d in 16x16 32x32 48x48 128x128 256x256 512x512 scalable ; do \
			( \
				cd $$d && \
				for subd in * ; do \
					for f in $$subd/* ; do \
						$(RM) $(DESTDIR)$(datadir)/icons/$$d/$$f ; \
					done ; \
					rmdir $(DESTDIR)$(datadir)/icons/$$d/$$subd || true ; \
				done \
			) ; \
			rmdir $(DESTDIR)$(datadir)/icons/$$d || true ; \
		done
	-rmdir $(DESTDIR)$(datadir)/icons

dist-hook:
	find $(distdir) -type d -name .svn | xargs $(RM) -r

# Platform specific special targets
debprep: distcheck
	tar xfz $(PACKAGE)-$(VERSION).tar.gz
	cd $(PACKAGE)-$(VERSION) && ./getnxsrcpkg nxcomp nxproxy nxssh
	cd $(PACKAGE)-$(VERSION) && tar xzf nxcomp-*.tar.gz
	cd $(PACKAGE)-$(VERSION) && tar xzf nxproxy-*.tar.gz
	cd $(PACKAGE)-$(VERSION) && tar xzf nxssh-*.tar.gz
	$(RM) $(PACKAGE)-$(VERSION)/*.tar.gz
	$(RM) $(PACKAGE)-$(VERSION)/nxssh/scard/Ssh.bin
	cd $(PACKAGE)-$(VERSION) && perl debian/mkchangelog.pl $(VERSION) \
		< ChangeLog > debian/changelog

# Create debian source package
debsrc: debprep
	ln -snf $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)_$(VERSION).orig.tar.gz
	dpkg-source -b -sP $(PACKAGE)-$(VERSION) $(PACKAGE)_$(VERSION).orig.tar.gz
	echo "Debtransform-Tar: $(PACKAGE)_$(VERSION).orig.tar.gz" >> $(PACKAGE)_$(VERSION)-$(SVNREV).dsc

deb: debprep
	cd $(PACKAGE)-$(VERSION) && dpkg-buildpackage -us -uc
	lintian -i $(PACKAGE)_$(VERSION).$(SVNREV)_*.changes
	$(RM) -r $(PACKAGE)-$(VERSION)

dmg:
	$(MAKE) -C packaging/MacOSX dmg

rpm: distcheck
	$(RPMBUILD) $(RPMBUILD_OPTS) -ta --with fetchnx $(PACKAGE)-$(VERSION).tar.gz

setup: all
	$(MAKE) -C packaging/win32 setup

unknown:
	@echo "Don't know how to build a distribution for $(target)"
	@exit 1

dist-platform: $(PLATFORMDIST)

packaging/MacOSX/pulse/include/pulse/pulseaudio.h:
	$(MAKE) -C packaging/MacOSX/pulse cachedinstall

packaging/win32/pulse/include/pulse/pulseaudio.h:
	$(MAKE) -C packaging/win32/pulse tmpinstall

EXTRA_DIST = conf/mkinstalldirs  conf/mkinstalldirs  conf/mkinstalldirs  conf/mkinstalldirs  conf/mkinstalldirs  conf/mkinstalldirs conf/config.rpath conf/authors.xml \
	INSTALL INSTALL.generic COPYING share etc/60-opennx.rules \
	opennx.pjd opennx.rc tracelog.rc watchreader.rc \
	opennx.spec patches getnxsrcpkg $(RESDEPS) ChangeLog extres debian \
	$(wildcard m4/*.m4) $(man_MANS)
