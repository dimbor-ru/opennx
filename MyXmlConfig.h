// $Id: MyXmlConfig.h 679 2012-02-09 23:29:53Z felfert $
//
// Copyright (C) 2006 The OpenNX Team
// Author: Fritz Elfert
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef _MYXMLCONFIG_H_
#define _MYXMLCONFIG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "MyXmlConfig.cpp"
#endif

#include <wx/dynarray.h>
#include <wx/string.h>
#include <wx/object.h>

#include "WinShare.h"

class wxXmlNode;

class ShareGroup : public wxObject
{
    DECLARE_DYNAMIC_CLASS(ShareGroup)

    public:
        ShareGroup() : wxObject() {m_eType = SharedResource::SHARE_UNKNOWN; }
        virtual ~ShareGroup() {}

        bool operator ==(const ShareGroup &);
        bool operator !=(const ShareGroup &);
        wxString toString();

        bool m_bDefault; // CUPS only
        bool m_bPublic; // CUPS only
        SharedResource::ShareType m_eType;
        wxString m_sGroupName;
        wxString m_sAlias; // SMB Only
        wxString m_sDriver; // CUPS Only
        wxString m_sPassword;
        wxString m_sShareName;
        wxString m_sUsername;
};

WX_DECLARE_OBJARRAY(ShareGroup, ArrayOfShareGroups);

class USBDevice;

class SharedUsbDevice : public wxObject
{
    DECLARE_DYNAMIC_CLASS(SharedUsbDevice)

    public:
        typedef enum {
            MODE_UNKNOWN = 0,
            MODE_LOCAL,
            MODE_REMOTE,
        } Mode;

        SharedUsbDevice() : wxObject() { m_eMode = MODE_UNKNOWN; }
        virtual ~SharedUsbDevice() {}
        wxString toShortString();
        bool cmpNoMode(const SharedUsbDevice &);
        bool operator ==(const SharedUsbDevice &);
        bool operator !=(const SharedUsbDevice &);
        bool MatchHotplug(const USBDevice &);

        Mode m_eMode;
        wxString m_sVendor;
        wxString m_sProduct;
        wxString m_sSerial;
        int m_iClass;
        int m_iVendorID;
        int m_iProductID;
};

WX_DECLARE_OBJARRAY(SharedUsbDevice, ArrayOfUsbForwards);

class MyXmlConfig
{
    public:
        typedef enum {
            SPEED_MODEM = 0,
            SPEED_ISDN,
            SPEED_ADSL,
            SPEED_WAN,
            SPEED_LAN,
        } ConnectionSpeed;

        typedef enum {
            STYPE_UNIX = 0,
            STYPE_WINDOWS,
            STYPE_VNC,
            STYPE_SHADOW,
        } SessionType;

        typedef enum {
            DTYPE_ANY = -3,
            DTYPE_RDP = -2,
            DTYPE_RFB = -1,
            DTYPE_KDE = 0,
            DTYPE_GNOME,
            DTYPE_CDE,
            DTYPE_XFCE,
            DTYPE_XDM,
            DTYPE_CUSTOM,
        } DesktopType;

        typedef enum {
            DPTYPE_640x480 = 0,
            DPTYPE_800x600,
            DPTYPE_1024x768,
            DPTYPE_AVAILABLE,
            DPTYPE_FULLSCREEN,
            DPTYPE_CUSTOM,
            DPTYPE_NODECORATION,
            DPTYPE_REMOTE,
        } DisplayType;

        typedef enum {
            CACHEMEM_0MB = 0,
            CACHEMEM_1MB,
            CACHEMEM_2MB,
            CACHEMEM_4MB,
            CACHEMEM_8MB,
            CACHEMEM_16MB,
            CACHEMEM_32MB,
            CACHEMEM_64MB,
            CACHEMEM_128MB,
        } CacheMemory;

        typedef enum CacheDisk {
            CACHEDISK_0MB = 0,
            CACHEDISK_4MB,
            CACHEDISK_8MB,
            CACHEDISK_16MB,
            CACHEDISK_32MB,
            CACHEDISK_64MB,
            CACHEDISK_128MB,
            CACHEDISK_256MB,
            CACHEDISK_512MB,
        } CacheDisk;

        typedef enum XdmMode {
            XDM_MODE_SERVER,
            XDM_MODE_QUERY,
            XDM_MODE_BROADCAST,
            XDM_MODE_LIST,
        } XdmMode;

        typedef enum RatePA {
            RATEPA_NORESAMPLE = 0,
            RATEPA_48000,
            RATEPA_44100,
            RATEPA_32000,
            RATEPA_16000,
            RATEPA_8000,
        } RatePA;

        MyXmlConfig();
        MyXmlConfig(const wxString &);
        virtual ~MyXmlConfig();

        MyXmlConfig &operator =(const MyXmlConfig &);
        bool operator ==(const MyXmlConfig &);

        void saveState();
        bool checkChanged();
        bool SaveToFile();
        bool LoadFromFile(const wxString &);
        bool LoadFromURL(const wxString &);
        bool LoadFromString(const wxString &, bool);

        wxString sGetSessionParams(const long, bool, const wxString &);
        wxString sGetListParams(const long);
        wxString sGetProxyParams(const long);
#ifdef __WXMSW__
        wxString sGetXserverParams(bool forNXWin);
#endif

        bool IsValid() { return m_bValid; }
        bool IsWritable() { return m_bWritable; }
        bool WasOldConfig() { return m_bOldConfig; }

        bool bGetDisableBackingstore() { return m_bDisableBackingstore; }
        bool bGetDisableComposite() { return m_bDisableComposite; }
        bool bGetDisableRender() { return m_bDisableRender; }
        bool bGetDisableShmem() { return m_bDisableShmem; }
        bool bGetDisableShpix() { return m_bDisableShpix; }
        bool bGetDisableTaint() { return m_bDisableTaint; }
        bool bGetDisableTcpNoDelay() { return m_bDisableTcpNoDelay; }
        bool bGetDisableXagent() { return m_bDisableXagent; }
        bool bGetDisableZlibCompression() { return m_bDisableZlibCompression; }
        bool bGetEnableMultimedia() { return m_bEnableMultimedia; }
        bool bGetEnableNativePA() { return m_bEnableNativePA; }
        bool bGetEnableMonoPA() { return m_bEnableMonoPA; }
        bool bGetEnableSmbSharing() { return m_bEnableSmbSharing; }
        bool bGetEnableSSL() { return m_bEnableSSL; }
        bool bGetExternalProxy() { return m_bExternalProxy; }
        bool bGetGuestMode() { return m_bGuestMode; }
        bool bGetKbdLayoutOther() { return m_bKbdLayoutOther; }
        bool bGetProxyPassRemember() { return m_bProxyPassRemember; }
        bool bGetRdpCache() { return m_bRdpCache; }
        bool bGetRdpRememberPassword() { return m_bRdpRememberPassword; }
        bool bGetRdpRunApplication() { return m_bRdpRunApplication; }
        bool bGetRememberPassword() { return m_bRememberPassword || m_bGuestMode; }
        bool bGetRemoveOldSessionFiles() { return m_bRemoveOldSessionFiles; }
        bool bGetRunConsole() { return m_bRunConsole; }
        bool bGetRunXclients() { return m_bRunXclients; }
        bool bGetUseCups() { return m_bUseCups; }
        bool bGetUseCustomImageEncoding() { return m_bUseCustomImageEncoding; }
        bool bGetUseProxy() { return m_bUseProxy; }
        bool bGetUseSmartCard() { return m_bUseSmartCard; }
        bool bGetVirtualDesktop() { return m_bVirtualDesktop; }
        bool bGetVncRememberPassword() { return m_bVncRememberPassword; }
        bool bGetVncRootless() { return m_bVncRootless; }
        bool bGetVncUseNxAuth() { return m_bVncUseNxAuth; }
        bool bGetEnableUSBIP() { return m_bEnableUSBIP; }
        bool bGetDisableDirectDraw() { return m_bDisableDirectDraw; }
        bool bGetDisableDeferredUpdates() { return m_bDisableDeferredUpdates; }
        bool bGetGrabKeyboard() { return m_bGrabKeyboard; }

        MyXmlConfig::ConnectionSpeed eGetConnectionSpeed() { return m_eConnectionSpeed; }
        MyXmlConfig::DesktopType eGetDesktopType() { return m_eDesktopType; }
        MyXmlConfig::DisplayType eGetDisplayType() { return m_eDisplayType; }
        MyXmlConfig::SessionType eGetSessionType() { return m_eSessionType; }
        MyXmlConfig::CacheMemory eGetCacheMemory() { return m_eCacheMemory; }
        MyXmlConfig::CacheDisk eGetCacheDisk() { return m_eCacheDisk; }
        MyXmlConfig::XdmMode eGetXdmMode() { return m_eXdmMode; }
        MyXmlConfig::RatePA eGetRatePA() { return m_eRatePA; }

        int iGetClipFilter() { return m_iClipFilter; }
        int iGetCupsPort() { return m_iCupsPort; }
        int iGetDisplayHeight() { return m_iDisplayHeight; }
        int iGetDisplayWidth() { return m_iDisplayWidth; }
        int iGetImageEncoding() { return m_iImageEncoding; }
        int iGetJpegQuality() { return m_iJpegQuality; }
        int iGetProxyPort() { return m_iProxyPort; }
        int iGetRdpAuthType() { return m_iRdpAuthType; }
        int iGetRdpColors() { return m_iRdpColors; }
        int iGetRdpImageEncoding() { return m_iRdpImageEncoding; }
        int iGetRdpJpegQuality() { return m_iRdpJpegQuality; }
        int iGetServerPort() { return m_iServerPort; }
        int iGetSmbPort() { return m_iSmbPort; }
        int iGetUsedShareGroups() { return m_iUsedShareGroups; }
        int iGetVncDisplayNumber() { return m_iVncDisplayNumber; }
        int iGetVncImageEncoding() { return m_iVncImageEncoding; }
        int iGetVncJpegQuality() { return m_iVncJpegQuality; }
        int iGetXdmBroadcastPort() { return m_iXdmBroadcastPort; }
        int iGetXdmListPort() { return m_iXdmListPort; }
        int iGetXdmQueryPort() { return m_iXdmQueryPort; }

        wxString sGetCommandLine() { return m_sCommandLine; }
        wxString sGetCupsPath() { return m_sCupsPath; }
        wxString sGetFileName() { return m_sFileName; }
        wxString sGetGuestUser() { return m_sGuestUser; }
        wxString sGetGuestPassword() { return m_sGuestPassword; }
        wxString sGetKbdLayoutLanguage() { return m_sKbdLayoutLanguage; }
        wxString sGetName() { return m_sName; }
        wxString sGetPassword() { return m_sPassword; }
        wxString sGetProxyCommand() { return m_sProxyCommand; }
        wxString sGetProxyHost() { return m_sProxyHost; }
        wxString sGetProxyPass() { return m_sProxyPass; }
        wxString sGetProxyUser() { return m_sProxyUser; }
        wxString sGetRdpApplication() { return m_sRdpApplication; }
        wxString sGetRdpDomain() { return m_sRdpDomain; }
        wxString sGetRdpHostName() { return m_sRdpHostName; }
        wxString sGetRdpPassword() { return m_sRdpPassword; }
        wxString sGetRdpUsername() { return m_sRdpUsername; }
        wxString sGetServerHost() { return m_sServerHost; }
        wxString sGetSshKey() { return m_sSshKey; }
        wxString sGetUsername() { return m_sUsername; }
        wxString sGetVncHostName() { return m_sVncHostName; }
        wxString sGetVncPassword() { return m_sVncPassword; }
        wxString sGetXdmListHost() { return m_sXdmListHost; }
        wxString sGetXdmQueryHost() { return m_sXdmQueryHost; }

        // For use by MySession
        wxString sGetSessionUser();
        wxString sGetSessionPassword();

        wxArrayString aGetUsedShareGroups() { return m_aUsedShareGroups; }
        ArrayOfShareGroups aGetShareGroups() { return m_aShareGroups; }
        ArrayOfUsbForwards aGetUsbForwards() { return m_aUsbForwards; }

        void bSetDisableBackingstore(const bool b) { m_bDisableBackingstore = b; }
        void bSetDisableComposite(const bool b) { m_bDisableComposite = b; }
        void bSetDisableRender(const bool b) { m_bDisableRender = b; }
        void bSetDisableShmem(const bool b) { m_bDisableShmem = b; }
        void bSetDisableShpix(const bool b) { m_bDisableShpix = b; }
        void bSetDisableTaint(const bool b) { m_bDisableTaint = b; }
        void bSetDisableTcpNoDelay(const bool b) { m_bDisableTcpNoDelay = b; }
        void bSetDisableXagent(const bool b) { m_bDisableXagent = b; }
        void bSetDisableZlibCompression(const bool b) { m_bDisableZlibCompression = b; }
        void bSetEnableMultimedia(const bool b) { m_bEnableMultimedia = b; }
        void bSetEnableNativePA(const bool b) { m_bEnableNativePA = b; }
        void bSetEnableMonoPA(const bool b) { m_bEnableMonoPA = b; }
        void bSetEnableSmbSharing(const bool b) { m_bEnableSmbSharing = b; }
        void bSetEnableSSL(const bool b) { m_bEnableSSL = b; }
        void bSetExternalProxy(const bool b) { m_bExternalProxy = b; }
        void bSetGuestMode(const bool b) { m_bGuestMode = b; }
        void bSetKbdLayoutOther(const bool b) { m_bKbdLayoutOther = b; }
        void bSetProxyPassRemember(const bool b) { m_bProxyPassRemember = b; }
        void bSetRdpCache(const bool b) { m_bRdpCache = b; }
        void bSetRdpRememberPassword(const bool b) { m_bRdpRememberPassword = b; }
        void bSetRdpRunApplication(const bool b) { m_bRdpRunApplication = b; }
        void bSetRememberPassword(const bool b) { m_bRememberPassword = b; }
        void bSetRemoveOldSessionFiles(const bool b) { m_bRemoveOldSessionFiles = b; }
        void bSetRunConsole(const bool b) { m_bRunConsole = b; }
        void bSetRunXclients(const bool b) { m_bRunXclients = b; }
        void bSetUseCups(const bool b) { m_bUseCups = b; }
        void bSetUseCustomImageEncoding(const bool b) { m_bUseCustomImageEncoding = b; }
        void bSetUseProxy(const bool b) { m_bUseProxy = b; }
        void bSetUseSmartCard(const bool b) { m_bUseSmartCard = b; }
        void bSetVirtualDesktop(const bool b) { m_bVirtualDesktop = b; }
        void bSetVncRememberPassword(const bool b) { m_bVncRememberPassword = b; }
        void bSetVncRootless(const bool b) { m_bVncRootless = b; }
        void bSetVncUseNxAuth(const bool b) { m_bVncUseNxAuth = b; }
        void bSetEnableUSBIP(const bool b) { m_bEnableUSBIP = b; }
        void bSetDisableDirectDraw(const bool b) { m_bDisableDirectDraw = b; }
        void bSetDisableDeferredUpdates(const bool b) { m_bDisableDeferredUpdates = b; }
        void bSetGrabKeyboard(const bool b) { m_bGrabKeyboard = b; }

        void eSetCacheDisk(MyXmlConfig::CacheDisk e) { m_eCacheDisk = e; }
        void eSetCacheMemory(MyXmlConfig::CacheMemory e) { m_eCacheMemory = e; }
        void eSetConnectionSpeed(MyXmlConfig::ConnectionSpeed e) { m_eConnectionSpeed = e; }
        void eSetDesktopType(MyXmlConfig::DesktopType e) { m_eDesktopType = e; }
        void eSetDisplayType(MyXmlConfig::DisplayType e) { m_eDisplayType = e; }
        void eSetSessionType(MyXmlConfig::SessionType e) { m_eSessionType = e; }
        void eSetXdmMode(MyXmlConfig::XdmMode e) { m_eXdmMode = e; }
        void eSetRatePA(MyXmlConfig::RatePA e) { m_eRatePA = e; }

        void iSetClipFilter(int i) { m_iClipFilter = i; }
        void iSetCupsPort(int i) { m_iCupsPort = i; }
        void iSetDisplayHeight(int i) { m_iDisplayHeight = i; }
        void iSetDisplayWidth(int i) { m_iDisplayWidth = i; }
        void iSetImageEncoding(int i) { m_iImageEncoding = i; }
        void iSetJpegQuality(int i) { m_iJpegQuality = i; }
        void iSetProxyPort(int i) { m_iProxyPort = i; }
        void iSetRdpAuthType(int i) { m_iRdpAuthType = i; }
        void iSetRdpColors(int i) { m_iRdpColors = i; }
        void iSetRdpImageEncoding(int i) { m_iRdpImageEncoding = i; }
        void iSetRdpJpegQuality(int i) { m_iRdpJpegQuality = i; }
        void iSetServerPort(int i) { m_iServerPort = i; }
        void iSetSmbPort(int i) { m_iSmbPort = i; }
        void iSetUsedShareGroups(int i) { m_iUsedShareGroups = i; }
        void iSetVncDisplayNumber(int i) { m_iVncDisplayNumber = i; }
        void iSetVncImageEncoding(int i) { m_iVncImageEncoding = i; }
        void iSetVncJpegQuality(int i) { m_iVncJpegQuality = i; }
        void iSetXdmBroadcastPort(int i) { m_iXdmBroadcastPort = i; }
        void iSetXdmListPort(int i) { m_iXdmListPort = i; }
        void iSetXdmQueryPort(int i) { m_iXdmQueryPort = i; }

        void sSetCommandLine(const wxString &s) { m_sCommandLine = s; }
        void sSetCupsPath(const wxString &s) { m_sCupsPath = s; }
        void sSetFileName(const wxString &s) { m_sFileName = s; }
        void sSetGuestUser(const wxString &s) { m_sGuestUser = s; }
        void sSetGuestPassword(const wxString &s) { m_sGuestPassword = s; }
        void sSetKbdLayoutLanguage(const wxString &s) { m_sKbdLayoutLanguage = s; }
        void sSetName(const wxString &s) { m_sName = s; }
        void sSetPassword(const wxString &s) { m_sPassword = s; }
        void sSetProxyCommand(const wxString &s) { m_sProxyCommand = s; }
        void sSetProxyHost(const wxString &s) { m_sProxyHost = s; }
        void sSetProxyPass(const wxString &s) { m_sProxyPass = s; }
        void sSetProxyUser(const wxString &s) { m_sProxyUser = s; }
        void sSetRdpApplication(const wxString &s) { m_sRdpApplication = s; }
        void sSetRdpDomain(const wxString &s) { m_sRdpDomain = s; }
        void sSetRdpHostName(const wxString &s) { m_sRdpHostName = s; }
        void sSetRdpPassword(const wxString &s) { m_sRdpPassword = s; }
        void sSetRdpUsername(const wxString &s) { m_sRdpUsername = s; }
        void sSetServerHost(const wxString &s) { m_sServerHost = s; }
        void sSetSshKey(const wxString &s) { m_sSshKey = s; }
        void sSetVncHostName(const wxString &s) { m_sVncHostName = s; }
        void sSetVncPassword(const wxString &s) { m_sVncPassword = s; }
        void sSetXdmListHost(const wxString &s) { m_sXdmListHost = s; }
        void sSetXdmQueryHost(const wxString &s) { m_sXdmQueryHost = s; }

        void aSetShareGroups(const ArrayOfShareGroups &a) { m_aShareGroups = a; }
        void aSetUsedShareGroups(const wxArrayString &a) { m_aUsedShareGroups = a; }
        void aSetUsbForwards(const ArrayOfUsbForwards &a) { m_aUsbForwards = a; }

        void sSetUsername(const wxString &s);

        static wxString UrlEsc(const wxString &s);

    private:
        static size_t CurlWriteCallback(void *, size_t, size_t, void *);

    private:
        // Disable copy constructor
        MyXmlConfig(const MyXmlConfig &);

        void init();
        bool getBool(wxXmlNode *, const wxString &, bool defval = false);
        long getLong(wxXmlNode *, const wxString &, long defval = 0);
        bool getLongBool(wxXmlNode *, const wxString &, bool defval = false);
        wxString getString(wxXmlNode *, const wxString &, const wxString &defval = _T(""));
        wxString getPassword(wxXmlNode *, const wxString &, const wxString &defval = _T(""));
        wxString *getStringNew(wxXmlNode *, const wxString &, wxString *defval = NULL);
        bool cmpShareGroups(const ArrayOfShareGroups, const ArrayOfShareGroups);
        bool cmpUsbForwards(const ArrayOfUsbForwards, const ArrayOfUsbForwards);
        bool cmpUsedShareGroups(const wxArrayString, const wxArrayString);
        ShareGroup &findShare(const wxString &);
        void bAddOption(wxXmlNode *, const wxString &, const bool);
        void iAddOption(wxXmlNode *, const wxString &, const long);
        void iAddOptionBool(wxXmlNode *, const wxString &, const bool);
        void sAddOption(wxXmlNode *, const wxString &, const wxString &);
        wxXmlNode *AddGroup(wxXmlNode *, const wxString &);
        bool loadFromStream(wxInputStream &is, bool);
        void getDesktopSize(int &dw, int &dh, int &ww, int &wh);

        // Special: This gets set, if a pre 0.16.173 "Images" section was detected
        // during reading. If this is set, we inform the user, suggesting a re-check
        // of his image settings.
        bool m_bOldConfig;

        bool m_bDisableBackingstore;
        bool m_bDisableComposite;
        bool m_bDisableRender;
        bool m_bDisableShmem;
        bool m_bDisableShpix;
        bool m_bDisableTaint;
        bool m_bDisableTcpNoDelay;
        bool m_bDisableXagent;
        bool m_bDisableZlibCompression;
        bool m_bEnableMultimedia;
        bool m_bEnableNativePA;
        bool m_bEnableMonoPA;
        bool m_bEnableSmbSharing;
        bool m_bEnableSSL;
        bool m_bEnableUSBIP;
        bool m_bExternalProxy;
        bool m_bGuestMode;
        bool m_bKbdLayoutOther;
        bool m_bProxyPassRemember;
        bool m_bRdpCache;
        bool m_bRdpRememberPassword;
        bool m_bRdpRunApplication;
        bool m_bWritable;
        bool m_bRememberPassword;
        bool m_bRemoveOldSessionFiles;
        bool m_bRunConsole;
        bool m_bRunXclients;
        bool m_bUseCups;
        bool m_bUseCustomImageEncoding;
        bool m_bUseProxy;
        bool m_bUseSmartCard;
        bool m_bValid;
        bool m_bVirtualDesktop;
        bool m_bVncRememberPassword;
        bool m_bVncRootless;
        bool m_bVncUseNxAuth;
        bool m_bDisableDirectDraw;
        bool m_bGrabKeyboard;
        bool m_bDisableDeferredUpdates;

        int m_iCupsPort;
        int m_iDisplayHeight;
        int m_iDisplayWidth;
        int m_iImageEncoding;
        int m_iJpegQuality;
        int m_iProxyPort;
        int m_iRdpAuthType;
        int m_iRdpColors;
        int m_iRdpImageEncoding;
        int m_iRdpJpegQuality;
        int m_iServerPort;
        int m_iSmbPort;
        int m_iUsedShareGroups;
        int m_iVncDisplayNumber;
        int m_iVncImageEncoding;
        int m_iVncJpegQuality;
        int m_iXdmBroadcastPort;
        int m_iXdmListPort;
        int m_iXdmQueryPort;
        int m_iClipFilter;

        wxString m_sCommandLine;
        wxString m_sCupsPath;
        wxString m_sFileName;
        wxString m_sGuestPassword;
        wxString m_sGuestUser;
        wxString m_sKbdLayoutLanguage;
        wxString m_sName;
        wxString m_sPassword;
        wxString m_sProxyCommand;
        wxString m_sProxyHost;
        wxString m_sProxyPass;
        wxString m_sProxyUser;
        wxString m_sRdpApplication;
        wxString m_sRdpDomain;
        wxString m_sRdpHostName;
        wxString m_sRdpPassword;
        wxString m_sRdpUsername;
        wxString m_sServerHost;
        wxString m_sSshKey;
        wxString m_sUsername;
        wxString m_sVncHostName;
        wxString m_sVncPassword;
        wxString m_sXdmListHost;
        wxString m_sXdmQueryHost;

        ConnectionSpeed m_eConnectionSpeed;
        DesktopType m_eDesktopType;
        DisplayType m_eDisplayType;
        SessionType m_eSessionType;
        CacheMemory m_eCacheMemory;
        CacheDisk m_eCacheDisk;
        XdmMode m_eXdmMode;
        RatePA m_eRatePA;

        ArrayOfShareGroups m_aShareGroups;
        wxArrayString m_aUsedShareGroups;
        ArrayOfUsbForwards m_aUsbForwards;

        MyXmlConfig *saved;
        wxString *m_pMd5Password;
        wxString *m_pClrPassword;
};

#endif
// _MYXMLCONFIG_H_
