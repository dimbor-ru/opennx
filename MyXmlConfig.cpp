// $Id: MyXmlConfig.cpp 704 2012-03-14 14:55:15Z felfert $
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "MyXmlConfig.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <wx/arrimpl.cpp>
#include <wx/file.h>
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <wx/regex.h>
#include <wx/url.h>
#include <wx/config.h>
#include <wx/display.h>

class wxConfigBase;

#include "LibUSB.h"
#include "MyXmlConfig.h"
#include "WinShare.h"

#ifdef APP_OPENNX
# include "opennxApp.h"
#endif
#ifdef APP_WATCHUSBIP
# include "watchUsbIpApp.h"
#endif

#include "pwcrypt.h"
#include "osdep.h"

#ifdef APP_OPENNX
# ifdef HAVE_LIBCURL
#  include <curl/curl.h>
# endif
#endif

#include "trace.h"
ENABLE_TRACE;

#define STR_TRUE wxT("true")
#define STR_FALSE wxT("false");
#define STR_ONE wxT("1")
#define STR_ZERO wxT("0")

// We use something the user cannot type here!
#define DUMMY_MD5_PASSWORD wxT("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b")
#define DUMMY_CLR_PASSWORD wxT("\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r")
#define DEFAULT_GUEST_USER wxT("NX guest user")

IMPLEMENT_DYNAMIC_CLASS(ShareGroup, wxObject);
IMPLEMENT_DYNAMIC_CLASS(SharedUsbDevice, wxObject);

WX_DEFINE_OBJARRAY(ArrayOfShareGroups);
WX_DEFINE_OBJARRAY(ArrayOfUsbForwards);

wxString ShareGroup::toString()
{
    wxString ret = wxT("Name: '");
    ret.Append(m_sGroupName);
    ret.Append(wxT("', Type: "));
    switch (m_eType) {
        case SharedResource::SHARE_UNKNOWN:
            ret.Append(wxT("unknown"));
            break;
        case SharedResource::SHARE_SMB_DISK:
            ret.Append(wxT("SMBdisk, Share: '"));
            ret.Append(m_sShareName);
            ret.Append(wxT("', Alias: '"));
            ret.Append(m_sAlias);
            ret.Append(wxT("'"));
            break;
        case SharedResource::SHARE_SMB_PRINTER:
            ret.Append(wxT("SMBprinter, Share: '"));
            ret.Append(m_sShareName);
            ret.Append(wxT("', Alias: '"));
            ret.Append(m_sAlias);
            ret.Append(wxT("'"));
            break;
        case SharedResource::SHARE_CUPS_PRINTER:
            ret.Append(wxT("CUPSprinter, Share: '"));
            ret.Append(m_sShareName);
            ret.Append(wxT("', Driver: '"));
            ret.Append(m_sDriver);
            ret.Append(wxT("', Public: "));
            ret.Append(m_bPublic ? wxT("true") : wxT("false"));
            ret.Append(wxT(", Default: "));
            ret.Append(m_bDefault ? wxT("true") : wxT("false"));
            break;
    }
    return ret;
}

bool ShareGroup::operator ==(const ShareGroup &other)
{
    if (m_eType != other.m_eType) return false;
    if (m_sGroupName != other.m_sGroupName) return false;
    if (m_sUsername != other.m_sUsername) return false;
    if (m_sPassword != other.m_sPassword) return false;
    if (m_sShareName != other.m_sShareName) return false;
    switch (m_eType) {
        case SharedResource::SHARE_UNKNOWN:
            break;
        case SharedResource::SHARE_SMB_DISK:
            if (m_sAlias != other.m_sAlias) return false;
            break;
        case SharedResource::SHARE_SMB_PRINTER:
        case SharedResource::SHARE_CUPS_PRINTER:
            if (m_sDriver != other.m_sDriver) return false;
            if (m_bPublic != other.m_bPublic) return false;
            if (m_bDefault != other.m_bDefault) return false;
            break;
    }
    return true;
}

bool ShareGroup::operator !=(const ShareGroup &other)
{
    return (!(*this == other));
}

bool SharedUsbDevice::operator ==(const SharedUsbDevice &other)
{
    if (m_eMode != other.m_eMode) return false;
    if (m_sVendor != other.m_sVendor) return false;
    if (m_sProduct != other.m_sProduct) return false;
    if (m_sSerial != other.m_sSerial) return false;
    if (m_iVendorID != other.m_iVendorID) return false;
    if (m_iProductID != other.m_iProductID) return false;
    if (m_iClass != other.m_iClass) return false;
    return true;
}

bool SharedUsbDevice::cmpNoMode(const SharedUsbDevice &other)
{
    if (m_sVendor != other.m_sVendor) return false;
    if (m_sProduct != other.m_sProduct) return false;
    if (m_sSerial != other.m_sSerial) return false;
    if (m_iVendorID != other.m_iVendorID) return false;
    if (m_iProductID != other.m_iProductID) return false;
    if (m_iClass != other.m_iClass) return false;
    return true;
}

bool SharedUsbDevice::operator !=(const SharedUsbDevice &other)
{
    return (!(*this == other));
}

bool SharedUsbDevice::MatchHotplug(const USBDevice &udev)
{
    if (m_iVendorID >= 0) {
        if (m_iVendorID != udev.GetVendorID())
            return false;
    }
    if (m_iProductID >= 0) {
        if (m_iProductID != udev.GetProductID())
            return false;
    }
    if (m_iClass >= 0) {
        if (m_iClass != udev.GetDeviceClass())
            return false;
    }
    if (!m_sVendor.IsEmpty()) {
        if (!udev.GetVendor().StartsWith(m_sVendor))
            return false;
    }
    if (!m_sProduct.IsEmpty()) {
        if (!udev.GetProduct().StartsWith(m_sProduct))
            return false;
    }
    if (!m_sSerial.IsEmpty()) {
        if (!udev.GetSerial().StartsWith(m_sSerial))
            return false;
    }
    return true;
}

wxString SharedUsbDevice::toShortString()
{
    wxString ret = m_sVendor.Strip(wxString::both);
    if ((!ret.IsEmpty()) && (!m_sProduct.IsEmpty()))
        ret.Append(wxT(" "));
    ret.Append(m_sProduct.Strip(wxString::both));
    return ret;
}

void
MyXmlConfig::init()
{
    saved = NULL;
    m_bDisableBackingstore = false;
    m_bDisableComposite = false;
    m_bDisableRender = false;
    m_bDisableShmem = false;
    m_bDisableShpix = false;
    m_bDisableTaint = false;
    m_bDisableTcpNoDelay = false;
    m_bDisableXagent = false;
    m_bDisableZlibCompression = false;
    m_bEnableMultimedia = false;
    m_bEnableNativePA = true;
    m_bEnableMonoPA = false;
    m_bEnableSmbSharing = false;
    m_bEnableSSL = true;
    m_bEnableUSBIP = false;
    m_bExternalProxy = false;
    m_bGuestMode = false;
    m_bKbdLayoutOther = false;
    m_bOldConfig = false;
    m_bProxyPassRemember = false;
    m_bRdpCache = true;
    m_bRdpRememberPassword = false;
    m_bRdpRootless = true;
    m_bRdpRunApplication = false;
    m_bRememberPassword = false;
    m_bRemoveOldSessionFiles = true;
    m_bRunConsole = false;
    m_bRunXclients = false;
    m_bUseCups = false;
    m_bUseCustomImageEncoding = false;
    m_bUseProxy = false;
    m_bUseSmartCard = false;
    m_bValid = false;
    m_bWritable = true;
    m_bVirtualDesktop = false;
    m_bVncRememberPassword = false;
    m_bVncRootless = true;
    m_bVncUseNxAuth = false;
    m_bDisableDirectDraw = false;
    m_bDisableDeferredUpdates = false;
    m_bGrabKeyboard = false;

    m_iClipFilter = 2;
    m_iCupsPort = 631;
    m_iDisplayHeight = 480;
    m_iDisplayWidth = 640;
    m_iImageEncoding = 3;
    m_iJpegQuality = 6;
    m_iProxyPort = 3128;
    m_iRdpAuthType = 1;
    m_iRdpColors = 8;
    m_iRdpImageEncoding = 3;
    m_iRdpJpegQuality = 6;
    m_iServerPort = 22;
    m_iSmbPort = 445;
    m_iUsedShareGroups = 0;
    m_iVncDisplayNumber = 0;
    m_iVncImageEncoding = 3;
    m_iVncJpegQuality = 6;
    m_iXdmBroadcastPort = 177;
    m_iXdmListPort = 177;
    m_iXdmQueryPort = 177;

    m_eCacheDisk = CACHEDISK_32MB;
    m_eCacheMemory = CACHEMEM_8MB;
    m_eConnectionSpeed = SPEED_ADSL;
    m_eDesktopType = DTYPE_KDE;
    m_eDisplayType = DPTYPE_AVAILABLE;
    m_eSessionType = STYPE_UNIX;
    m_eXdmMode = XDM_MODE_SERVER;
    m_eRatePA = RATEPA_NORESAMPLE;

    m_sCommandLine = wxEmptyString;
    wxConfigBase::Get()->Read(wxT("Config/CupsPath"), &m_sCupsPath);
    if (m_sCupsPath.IsEmpty())
        m_sCupsPath = wxT(CUPS_DEFAULT_PATH);
    m_sFileName = wxEmptyString;
    m_sGuestUser = wxEmptyString;
    m_sGuestPassword = wxEmptyString;
    m_sKbdLayoutLanguage = wxString(wxConvLocal.cMB2WX(x11_keyboard_type)).AfterFirst(wxT('/')).BeforeFirst(wxT(','));
    m_sName = wxEmptyString;
    m_sPassword = wxEmptyString;
    m_sProxyCommand = wxEmptyString;
    m_sProxyHost = wxEmptyString;
    m_sProxyPass = wxEmptyString;
    m_sProxyUser = wxEmptyString;
    m_sRdpApplication = wxEmptyString;
    m_sRdpDomain = wxEmptyString;
    m_sRdpHostName = wxEmptyString;
    m_sRdpPassword = wxEmptyString;
    m_sRdpUsername = wxEmptyString;
    m_sServerHost = wxEmptyString;
    m_sSshKey = wxEmptyString;
    m_sUsername = wxEmptyString;
    m_sVncHostName = wxEmptyString;
    m_sVncPassword = wxEmptyString;
    m_sXdmListHost = wxT("localhost");
    m_sXdmQueryHost = wxT("localhost");

    m_pMd5Password = NULL;
    m_pClrPassword = NULL;
}

MyXmlConfig::MyXmlConfig()
{
    init();
}

MyXmlConfig::MyXmlConfig(const wxString &filename)
{
    init();
    if (filename.StartsWith(wxT("http://")) ||
            filename.StartsWith(wxT("https://")) ||
            filename.StartsWith(wxT("ftp://")) ||
            filename.StartsWith(wxT("file://")))
        LoadFromURL(filename);
    else
        LoadFromFile(filename);
}

MyXmlConfig::~MyXmlConfig()
{
    if (saved)
        delete saved;
    if (m_pMd5Password)
        delete m_pMd5Password;
    if (m_pClrPassword)
        delete m_pClrPassword;
}

    MyXmlConfig &
MyXmlConfig::operator =(const MyXmlConfig &other)
{
    saved = NULL;

    m_bDisableBackingstore = other.m_bDisableBackingstore;
    m_bDisableComposite = other.m_bDisableComposite;
    m_bDisableRender = other.m_bDisableRender;
    m_bDisableShmem = other.m_bDisableShmem;
    m_bDisableShpix = other.m_bDisableShpix;
    m_bDisableTaint = other.m_bDisableTaint;
    m_bDisableTcpNoDelay = other.m_bDisableTcpNoDelay;
    m_bDisableXagent = other.m_bDisableXagent;
    m_bDisableZlibCompression = other.m_bDisableZlibCompression;
    m_bEnableMultimedia = other.m_bEnableMultimedia;
    m_bEnableNativePA = other.m_bEnableNativePA;
    m_bEnableMonoPA = other.m_bEnableMonoPA;
    m_bEnableSmbSharing = other.m_bEnableSmbSharing;
    m_bEnableSSL = other.m_bEnableSSL;
    m_bEnableUSBIP = other.m_bEnableUSBIP;
    m_bExternalProxy = other.m_bExternalProxy;
    m_bGuestMode = other.m_bGuestMode;
    m_bKbdLayoutOther = other.m_bKbdLayoutOther;
    m_bProxyPassRemember = other.m_bProxyPassRemember;
    m_bRdpCache = other.m_bRdpCache;
    m_bRdpRememberPassword = other.m_bRdpRememberPassword;
    m_bRdpRootless = other.m_bRdpRootless;
    m_bRdpRunApplication = other.m_bRdpRunApplication;
    m_bRememberPassword = other.m_bRememberPassword;
    m_bRemoveOldSessionFiles = other.m_bRemoveOldSessionFiles;
    m_bRunConsole = other.m_bRunConsole;
    m_bRunXclients = other.m_bRunXclients;
    m_bUseCups = other.m_bUseCups;
    m_bUseCustomImageEncoding = other.m_bUseCustomImageEncoding;
    m_bUseProxy = other.m_bUseProxy;
    m_bUseSmartCard = other.m_bUseSmartCard;
    m_bValid = other.m_bValid;
    // Don't copy readonly flag
    m_bVirtualDesktop = other.m_bVirtualDesktop;
    m_bVncRememberPassword = other.m_bVncRememberPassword;
    m_bVncRootless = other.m_bVncRootless;
    m_bVncUseNxAuth = other.m_bVncUseNxAuth;
    m_bDisableDirectDraw = other.m_bDisableDirectDraw;
    m_bDisableDeferredUpdates = other.m_bDisableDeferredUpdates;
    m_bGrabKeyboard = other.m_bGrabKeyboard;

    m_iClipFilter = other.m_iClipFilter;
    m_iCupsPort = other.m_iCupsPort;
    m_iDisplayHeight = other.m_iDisplayHeight;
    m_iDisplayWidth = other.m_iDisplayWidth;
    m_iImageEncoding = other.m_iImageEncoding;
    m_iJpegQuality = other.m_iJpegQuality;
    m_sKbdLayoutLanguage = other.m_sKbdLayoutLanguage;
    m_iProxyPort = other.m_iProxyPort;
    m_iRdpAuthType = other.m_iRdpAuthType;
    m_iRdpColors = other.m_iRdpColors;
    m_iRdpImageEncoding = other.m_iRdpImageEncoding;
    m_iRdpJpegQuality = other.m_iRdpJpegQuality;
    m_iServerPort = other.m_iServerPort;
    m_iSmbPort = other.m_iSmbPort;
    m_iUsedShareGroups = other.m_iUsedShareGroups;
    m_iVncDisplayNumber = other.m_iVncDisplayNumber;
    m_iVncImageEncoding = other.m_iVncImageEncoding;
    m_iVncJpegQuality = other.m_iVncJpegQuality;
    m_iXdmBroadcastPort = other.m_iXdmBroadcastPort;
    m_iXdmListPort = other.m_iXdmListPort;
    m_iXdmQueryPort = other.m_iXdmQueryPort;

    m_eCacheDisk = other.m_eCacheDisk;
    m_eCacheMemory = other.m_eCacheMemory;
    m_eConnectionSpeed = other.m_eConnectionSpeed;
    m_eDesktopType = other.m_eDesktopType;
    m_eDisplayType = other.m_eDisplayType;
    m_eSessionType = other.m_eSessionType;
    m_eXdmMode = other.m_eXdmMode;
    m_eRatePA = other.m_eRatePA;

    m_sCommandLine = other.m_sCommandLine;
    m_sCupsPath = other.m_sCupsPath;
    m_sGuestPassword = other.m_sGuestPassword;
    m_sGuestUser = other.m_sGuestUser;
    m_sName = other.m_sName;
    m_sPassword = other.m_sPassword;
    m_sProxyCommand = other.m_sProxyCommand;
    m_sProxyHost = other.m_sProxyHost;
    m_sProxyPass = other.m_sProxyPass;
    m_sProxyUser = other.m_sProxyUser;
    m_sRdpApplication = other.m_sRdpApplication;
    m_sRdpDomain = other.m_sRdpDomain;
    m_sRdpHostName = other.m_sRdpHostName;
    m_sRdpPassword = other.m_sRdpPassword;
    m_sRdpUsername = other.m_sRdpUsername;
    m_sServerHost = other.m_sServerHost;
    m_sSshKey = other.m_sSshKey;
    m_sUsername = other.m_sUsername;
    m_sVncHostName = other.m_sVncHostName;
    m_sVncPassword = other.m_sVncPassword;
    m_sXdmListHost = other.m_sXdmListHost;
    m_sXdmQueryHost = other.m_sXdmQueryHost;

    m_aShareGroups = other.m_aShareGroups;
    m_aUsedShareGroups = other.m_aUsedShareGroups;
    m_aUsbForwards = other.m_aUsbForwards;

    if (other.m_pMd5Password) {
        if (m_pMd5Password)
            delete m_pMd5Password;
        m_pMd5Password = new wxString(*other.m_pMd5Password);
    }
    if (other.m_pClrPassword) {
        if (m_pClrPassword)
            delete m_pClrPassword;
        m_pClrPassword = new wxString(*other.m_pClrPassword);
    }
    return *this;
}

// Retrieve parameters for proxy otion file
    wxString
MyXmlConfig::sGetProxyParams(const long protocolVersion)
{
    wxUnusedVar(protocolVersion);
    wxString ret = wxEmptyString;
    ret << wxT(",shmem=") << (m_bDisableShmem ? 0 : 1)
        << wxT(",shpix=") << (m_bDisableShpix ? 0 : 1)
        ;
    // FIXME: use real settings
    ret << wxT(",font=1");
    ret << wxT(",aux=1");
    return ret;
}

// Retrieve parameters for listsession command
    wxString
MyXmlConfig::sGetListParams(const long protocolVersion)
{
    wxUnusedVar(protocolVersion);
    wxString ret = wxT(" --user=\"");
    if (m_eSessionType == STYPE_SHADOW) {
        ret = wxT(" --type=\"shadow\"");
        return ret;
    }
    if (m_bGuestMode) {
        if (m_sGuestUser.IsEmpty())
            ret << DEFAULT_GUEST_USER;
        else
            ret << m_sGuestUser;
    } else
        ret << m_sUsername;
    if (protocolVersion >= 0x00040000)
        ret << wxT("\" --status=\"disconnected,connected\"");
    else
        ret << wxT("\" --status=\"suspended,running\"");
    ret << wxT(" --type=\"");
    switch (m_eSessionType) {
        case STYPE_SHADOW:
            break;
        case STYPE_UNIX:
            ret << wxT("unix-");
            switch (m_eDesktopType) {
                case DTYPE_ANY:
                case DTYPE_RDP:
                case DTYPE_RFB:
                    break;
                case DTYPE_KDE:
                    ret << wxT("kde\"");
                    break;
                case DTYPE_GNOME:
                    ret << wxT("gnome\"");
                    break;
                case DTYPE_CDE:
                    ret << wxT("cde\"");
                    break;
                case DTYPE_XFCE:
                    ret << wxT("xfce\"");
                    break;
                case DTYPE_XDM:
                    ret << wxT("xdm\"");
                    break;
                case DTYPE_CUSTOM:
                    if (m_bRunConsole)
                        ret << wxT("console\"");
                    else if (m_bRunXclients)
                        ret << wxT("default\"");
                    else
                        ret << wxT("application\"");
                    break;
            }
            break;
        case STYPE_WINDOWS:
            ret << wxT("windows\"");
            break;
        case STYPE_VNC:
            ret << wxT("vnc\"");
            break;
    }
    int w, h;
    ::wxDisplaySize(&w, &h);
    ret << wxT(" --geometry=\"") << w << wxT("x") << h << wxT("x")
        << ::wxDisplayDepth() << (m_bDisableRender ? wxEmptyString : wxT("+render"))
        << ((m_eDisplayType == DPTYPE_FULLSCREEN) ? wxT("+fullscreen") : wxEmptyString)
        << wxT("\"");
    return ret;
}

    wxString
MyXmlConfig::UrlEsc(const wxString &s)
{
    size_t len = s.Length();
    wxString ret;
    for (size_t i = 0; i < len; i++) {
        switch ((wchar_t)s[i]) {
            case wxT(' '):
            case wxT(':'):
            case wxT('"'):
            case wxT('&'):
            case wxT('$'):
            case wxT('`'):
            case wxT('\''):
            case wxT('\\'):
                ret << wxString::Format(wxT("%%%02X"), s[i]);
                break;
            default:
                ret << s[i];
        }
    }
    return ret;
}

    void
MyXmlConfig::getDesktopSize(int &dw, int &dh, int &ww, int &wh)
{
    // Fetch the size of the display and the workarea
    // (workarea == display size reduced by the size of the taskbar and window
    // decorations) where our toplevel dialog are shown.
    wxWindow *tlw = ::wxGetApp().GetTopWindow();
    if (NULL == tlw) {
        wxLogError(_("Could not find application window"));
        return;
    }
    int dspidx = wxDisplay::GetFromWindow(tlw);
    wxDisplay dsp(dspidx);
    wxRect r = dsp.GetGeometry();
    dw = r.GetWidth();
    dh = r.GetHeight();
    r = dsp.GetClientArea();
    wxSize sz = tlw->GetSize();
    wxSize clsz = tlw->GetClientSize();
#ifdef __WXMSW__
    ww = r.GetWidth();
    wh = r.GetHeight();
#else
    ww = r.GetWidth() - (sz.GetWidth() - clsz.GetWidth());
    wh = r.GetHeight() - (sz.GetHeight() - clsz.GetHeight());
#endif
    myLogTrace(MYTRACETAG, wxT("Display: %dx%d, Workarea: %dx%d (netto: %d,%d)"),
            dw, dh, (int)r.GetWidth(), (int)r.GetHeight(), ww, wh);
}

// Retrieve parameters for startsession command
    wxString
MyXmlConfig::sGetSessionParams(const long protocolVersion, bool bNew, const wxString &clrpass)
{
    wxUnusedVar(protocolVersion);
    wxString ret = wxEmptyString;
    bool bNeedGeometry = true;
    int dspw, dsph, clientw, clienth, gw, gh;

    getDesktopSize(dspw, dsph, clientw, clienth);
    ret << wxString::Format(wxT(" --session=\"%s\""), VMB(m_sName));
    ret << wxT(" --type=\"");
    switch (m_eSessionType) {
        case STYPE_SHADOW:
            ret << wxT("shadow\"");
            if (m_bUseCustomImageEncoding) {
                ret << wxT(" --imagecompressionmethod=\"") << m_iImageEncoding << wxT("\"")
                    << wxT(" --imagecompressionlevel=\"")
                    << (((-1 == m_iImageEncoding) || (4 == m_iImageEncoding)) ? m_iJpegQuality : -1) << wxT("\"");
            }
            break;
        case STYPE_UNIX:
            ret << wxT("unix-");
            switch (m_eDesktopType) {
                case DTYPE_ANY:
                case DTYPE_RDP:
                case DTYPE_RFB:
                    break;
                case DTYPE_KDE:
                    ret << wxT("kde\"");
                    break;
                case DTYPE_GNOME:
                    ret << wxT("gnome\"");
                    break;
                case DTYPE_CDE:
                    ret << wxT("cde\"");
                    break;
                case DTYPE_XFCE:
                    ret << wxT("xfce\"");
                    break;
                case DTYPE_XDM:
                    ret << wxT("xdm\"");
                    switch (m_eXdmMode) {
                        case XDM_MODE_SERVER:
                            ret << wxT(" --xdm_port=\"-1\"");
                            break;
                        case XDM_MODE_QUERY:
                            ret << wxT(" --xdm_type=\"query\"")
                                << wxT(" --xdm_host=\"") << m_sXdmQueryHost << wxT("\"")
                                << wxT(" --xdm_port=\"") << m_iXdmQueryPort << wxT("\"");
                            break;
                        case XDM_MODE_BROADCAST:
                            ret << wxT(" --xdm_type=\"broadcast\"")
                                << wxT(" --xdm_port=\"") << m_iXdmBroadcastPort << wxT("\"");
                            break;
                        case XDM_MODE_LIST:
                            ret << wxT(" --xdm_type=\"list\"")
                                << wxT(" --xdm_host=\"") << m_sXdmListHost << wxT("\"")
                                << wxT(" --xdm_port=\"") << m_iXdmListPort << wxT("\"");
                            break;
                    }
                    break;
                case DTYPE_CUSTOM:
                    if (m_bRunConsole)
                        ret << wxT("console\"");
                    else if (m_bRunXclients)
                        ret << wxT("default\"");
                    else {
//                        bNeedGeometry = false;
                        ret << wxT("application\"");
                        ret    << wxT(" --application=\"")
                            << UrlEsc(m_sCommandLine) << wxT("\"");
                    }
                    ret << wxT(" --rootless=\"") << (m_bVirtualDesktop ? 0 : 1)
                        << wxT("\" --virtualdesktop=\"")
                        << (m_bVirtualDesktop ? 1 : 0) << wxT("\"");
                    break;
            }
            if (DTYPE_CUSTOM != m_eDesktopType)
                ret << wxT(" --rootless=\"0\" --virtualdesktop=\"1\"");
            if (m_bUseCustomImageEncoding) {
                ret << wxT(" --imagecompressionmethod=\"") << m_iImageEncoding << wxT("\"")
                    << wxT(" --imagecompressionlevel=\"")
                    << (((-1 == m_iImageEncoding) || (4 == m_iImageEncoding)) ? m_iJpegQuality : -1) << wxT("\"");
            }
            break;
        case STYPE_WINDOWS:
            ret << wxT("windows\"")
                << wxT(" --agent_server=\"") << UrlEsc(m_sRdpHostName)
                << wxT("\" --agent_domain=\"") << UrlEsc(m_sRdpDomain)
                << wxT("\"");
            switch (m_iRdpAuthType) {
                case 0:
                    // use specified user/passwd
                    ret << wxT(" --agent_user=\"") << UrlEsc(m_sRdpUsername)
                        << wxT("\" --agent_password=\"") << UrlEsc(m_sRdpPassword)
                        << wxT("\"");
                    break;
                case 1:
                    // show winlogon (empty username and password)
                    break;
                case 2:
                    // use NX credentials
                    ret << wxT(" --agent_user=\"") << UrlEsc(sGetSessionUser())
                        << wxT("\" --agent_password=\"")
                        << UrlEsc(m_bRememberPassword ? sGetSessionPassword() : clrpass)
                        << wxT("\"");
                    break;
                    break;
            }
            if (m_bRdpRunApplication)
                ret << wxT(" --application=\"") << UrlEsc(m_sRdpApplication) << wxT("\"");
            if (m_bUseCustomImageEncoding) {
                ret << wxT(" --rdpcolors=\"");
                switch (m_iRdpColors) {
                    case 0:
                        ret << wxT("256\"");
                        break;
                    case 1:
                        ret << wxT("32K\"");
                        break;
                    case 2:
                        ret << wxT("64K\"");
                        break;
                    case 3:
                        ret << wxT("16M\"");
                        break;
                }
                ret << wxT(" --rdpcache=\"") << (m_bRdpCache ? 1 : 0) << wxT("\"")
                    << wxT(" --imagecompressionmethod=\"") << m_iRdpImageEncoding << wxT("\"")
                    << wxT(" --imagecompressionlevel=\"")
                    << (((-1 == m_iRdpImageEncoding) || (4 == m_iRdpImageEncoding)) ? m_iRdpJpegQuality : -1)
                    << wxT("\"");
            }
            if (m_bRdpRootless)
                ret << wxT(" --rootless=\"1\" --virtualdesktop=\"0\"");
            break;
            break;
        case STYPE_VNC:
            ret << wxT("vnc\"")
                << wxT(" --agent_server=\"")
                << UrlEsc(m_sVncHostName) << wxT("%3A") << m_iVncDisplayNumber
                << wxT("\" --agent_password=\"") << UrlEsc(m_sVncPassword) << wxT("\"");
            if (m_bUseCustomImageEncoding) {
                ret << wxT(" --imagecompressionmethod=\"") << m_iVncImageEncoding << wxT("\"")
                    << wxT(" --imagecompressionlevel=\"")
                    << (((-1 == m_iVncImageEncoding) || (4 == m_iVncImageEncoding)) ? m_iVncJpegQuality : -1)
                    << wxT("\"");
            }
            if (m_bVncRootless)
                ret << wxT(" --rootless=\"1\" --virtualdesktop=\"0\"");
            break;
    }
    ret << wxT(" --cache=\"");
    switch (m_eCacheMemory) {
        case CACHEMEM_0MB:
            ret << wxT("0M\"");
            break;
        case CACHEMEM_1MB:
            ret << wxT("1M\"");
            break;
        case CACHEMEM_2MB:
            ret << wxT("2M\"");
            break;
        case CACHEMEM_4MB:
            ret << wxT("4M\"");
            break;
        case CACHEMEM_8MB:
            ret << wxT("8M\"");
            break;
        case CACHEMEM_16MB:
            ret << wxT("16M\"");
            break;
        case CACHEMEM_32MB:
            ret << wxT("32M\"");
            break;
        case CACHEMEM_64MB:
            ret << wxT("64M\"");
            break;
        case CACHEMEM_128MB:
            ret << wxT("128M\"");
            break;
    }
    ret << wxT(" --images=\"");
    switch (m_eCacheDisk) {
        case CACHEDISK_0MB:
            ret << wxT("0M\"");
            break;
        case CACHEDISK_4MB:
            ret << wxT("4M\"");
            break;
        case CACHEDISK_8MB:
            ret << wxT("8M\"");
            break;
        case CACHEDISK_16MB:
            ret << wxT("16M\"");
            break;
        case CACHEDISK_32MB:
            ret << wxT("32M\"");
            break;
        case CACHEDISK_64MB:
            ret << wxT("64M\"");
            break;
        case CACHEDISK_128MB:
            ret << wxT("128M\"");
            break;
        case CACHEDISK_256MB:
            ret << wxT("256M\"");
            break;
        case CACHEDISK_512MB:
            ret << wxT("512M\"");
            break;
    }
    ret << wxT(" --link=\"");
    switch (m_eConnectionSpeed) {
        case SPEED_MODEM:
            ret << wxT("modem\"");
            break;
        case SPEED_ISDN:
            ret << wxT("isdn\"");
            break;
        case SPEED_ADSL:
            ret << wxT("adsl\"");
            break;
        case SPEED_WAN:
            ret << wxT("wan\"");
            break;
        case SPEED_LAN:
            ret << wxT("lan\"");
            break;
    }
    gw = clientw; gh = clienth;
    if (bNeedGeometry) {
        ret << wxT(" --geometry=\"");
        switch (m_eDisplayType) {
            case DPTYPE_640x480:
                gw = 640; gh = 480;
                break;
            case DPTYPE_800x600:
                gw = 800; gh = 600;
                break;
            case DPTYPE_1024x768:
                gw = 1024; gh = 768;
                break;
            case DPTYPE_1280x1024:
                gw = 1280; gh = 1024;
                break;
            case DPTYPE_1400x1050:
                gw = 1400; gh = 1050;
                break;
            case DPTYPE_1440x900:
                gw = 1440; gh = 900;
                break;
            case DPTYPE_1680x1050:
                gw = 1680; gh = 1050;
                break;
            case DPTYPE_1920x1080:
                gw = 1920; gh = 1080;
                break;
            case DPTYPE_AVAILABLE:
            case DPTYPE_NODECORATION:
                gw = clientw; gh = clienth;
                break;
            case DPTYPE_CUSTOM:
                gw = m_iDisplayWidth; gh = m_iDisplayHeight;
                break;
            case DPTYPE_REMOTE:
            case DPTYPE_FULLSCREEN:
                gw = dspw; gh = dsph;
                break;
        }
        ret << wxString::Format(wxT("%dx%d\""), gw, gh);
        if (m_eDisplayType == DPTYPE_FULLSCREEN)
            ret << wxT(" --fullscreen=\"1\"");
    }
    if (m_eSessionType != STYPE_SHADOW) {
        int dspw2 = dspw, dsph2 = dsph;
        if ((m_eDesktopType != DTYPE_CUSTOM) || (m_bVirtualDesktop)) {
            dspw2 = gw; dsph2 =gh;
        }
        if ((m_eSessionType == STYPE_VNC) || (m_eSessionType == STYPE_WINDOWS))
        {
            dspw2 = clientw; dsph2 =clienth;
        }
        ret << wxT(" --screeninfo=\"") << dspw2 << wxT("x") << dsph2 << wxT("x")
            << ::wxDisplayDepth() << (m_bDisableRender ? wxEmptyString : wxT("+render"))
            << ((m_eDisplayType == DPTYPE_FULLSCREEN) ? wxT("+fullscreen") : wxEmptyString)
            << wxT("\"");
    }

    wxString kbdLocal = wxString(wxConvLocal.cMB2WX(x11_keyboard_type)).BeforeFirst(wxT(','));
    ret << wxT(" --keyboard=\"");
    if (m_bKbdLayoutOther) {
        if (wxNOT_FOUND != kbdLocal.Find(wxT('/')))
            ret << kbdLocal.BeforeFirst(wxT('/')) << wxT("/");
        ret << m_sKbdLayoutLanguage;
    } else {
        if (kbdLocal.IsEmpty())
            kbdLocal = wxT("query");
        ret << kbdLocal;
    }
    ret << wxT("\"")
        << wxT(" --backingstore=\"") << (m_bDisableBackingstore ? 0 : 1) << wxT("\"")
        << wxT(" --encryption=\"") << (m_bEnableSSL ? 1 : 0) << wxT("\"");
    if (bNew) {
        ret << wxT(" --render=\"") << (m_bDisableRender ? 0 : 1) << wxT("\"");
    }
    ret << wxT(" --composite=\"") << (m_bDisableComposite ? 0 : 1) << wxT("\"")
        << wxT(" --shmem=\"") << (m_bDisableShmem ? 0 : 1) << wxT("\"")
        << wxT(" --shpix=\"") << (m_bDisableShpix ? 0 : 1) << wxT("\"");
    // deprecated << wxT(" --streaming=\"") << (m_bDisableStreaming ? 0 : 1) << wxT("\"")
    if (bNew) {
        ret << wxT(" --samba=\"") << (m_bEnableSmbSharing ? 1 : 0) << wxT("\"")
            << wxT(" --cups=\"") << (m_bUseCups ? 1 : 0) << wxT("\"")
            << wxT(" --nodelay=\"") << (m_bDisableTcpNoDelay ? 0 : 1) << wxT("\"")
            << wxT(" --defer=\"") << (m_bDisableDeferredUpdates ? 1 : 0) << wxT("\"");
    }
#ifdef __WXMAC__
    ret << wxT(" --client=\"macosx\"");
#else
# ifdef __UNIX__
    ret << wxT(" --client=\"linux\"");
# else
# ifdef __WXMSW__
    ret << wxT(" --client=\"winnt\"");
# else
    ret << wxT(" --client=\"linux\"");
# endif
# endif
#endif
    ret << wxT(" --media=\"") << (m_bEnableMultimedia ? 1 : 0) << wxT("\"");
    if (m_bEnableMultimedia) {
        if (m_bEnableNativePA) {
            ret << wxT(" --mediahelper=\"pa");
            if (m_eRatePA != RATEPA_NORESAMPLE) {
                switch (m_eRatePA) {
                    case RATEPA_48000:
                        ret << wxT("-48000"); break;
                    case RATEPA_44100:
                        ret << wxT("-44100"); break;
                    case RATEPA_32000:
                        ret << wxT("-32000"); break;
                    case RATEPA_16000:
                        ret << wxT("-16000"); break;
                    case RATEPA_8000:
                        ret << wxT("-8000"); break;
                }
                if (m_bEnableMonoPA)
                        ret << wxT("-1");
            }
            ret << wxT("\"");
        } else
            ret << wxT(" --mediahelper=\"esd\"");
    }
    // Original always uses those?!
    ret << wxT(" --strict=\"0\"");
    if (bNew) {
        // With this parameter set, attaching to shadow sessions fails, if the server's display base is >= 10000
        // so we simply leave it out for shadow sessions.
        ret << wxT(" --aux=\"1\"");
    }
    return ret;
}

#ifdef __WXMSW__
    wxString
MyXmlConfig::sGetXserverParams(bool forNXWin)
{
    wxString ret;
    int dspw, dsph, clientw, clienth;
    getDesktopSize(dspw, dsph, clientw, clienth);

    if (forNXWin) {
      if ((m_eDesktopType == MyXmlConfig::DTYPE_CUSTOM) && !m_bVirtualDesktop)
        ret << wxT(" -screen 0 640x480");
      else
        switch (m_eDisplayType) {
            case MyXmlConfig::DPTYPE_640x480:
                ret << wxT(" -screen 0 640x480");
                break;
            case MyXmlConfig::DPTYPE_1024x768:
                ret << wxT(" -screen 0 1024x768");
                break;
            case MyXmlConfig::DPTYPE_1280x1024:
                ret << wxT(" -screen 0 1280x1024");
                break;
            case MyXmlConfig::DPTYPE_1400x1050:
                ret << wxT(" -screen 0 1400x1050");
                break;
            case MyXmlConfig::DPTYPE_1440x900:
                ret << wxT(" -screen 0 1440x900");
                break;
            case MyXmlConfig::DPTYPE_1680x1050:
                ret << wxT(" -screen 0 1680x1050");
                break;
            case MyXmlConfig::DPTYPE_1920x1080:
                ret << wxT(" -screen 0 1920x1080");
                break;
            case MyXmlConfig::DPTYPE_AVAILABLE:
                ret << wxT(" -screen 0 ") << clientw << wxT("x") << clienth;
                break;
            case MyXmlConfig::DPTYPE_FULLSCREEN:
                ret << wxT(" -fullscreen ");
                break;
            case MyXmlConfig::DPTYPE_CUSTOM:
                // Fall thru
            case MyXmlConfig::DPTYPE_REMOTE:
                ret << wxT(" -screen 0 ") << m_iDisplayWidth << wxT("x") << m_iDisplayHeight;
                break;
            case MyXmlConfig::DPTYPE_800x600:
                // fall thru
            default:
                ret << wxT(" -screen 0 800x600");
                break;
        }
    } else {
        if (MyXmlConfig::DPTYPE_FULLSCREEN == m_eDisplayType) {
            ret << wxT(" -terminate -fullscreen -hide");
        } else {
            ret << wxT(" -noreset -lesspointer -multiwindow");
        }
    }
    if (m_bDisableDirectDraw)
        ret << wxT(" -engine 1");
    ret << wxT(" -") << (m_bGrabKeyboard ? wxT("") : wxT("no")) << wxT("keyhook");
    return ret;
}
#endif

    ShareGroup &
MyXmlConfig::findShare(const wxString &name)
{
    size_t cnt = m_aShareGroups.GetCount();
    static ShareGroup ret;
    for (size_t i = 0; i < cnt; i++) {
        if (m_aShareGroups[i].m_sGroupName == name)
            return m_aShareGroups[i];
    }
    return ret;
}

// Compare two arrays of ShareGroup where sequence of elements is irrelevant
// return true, if arrays are different.
    bool
MyXmlConfig::cmpShareGroups(ArrayOfShareGroups a1, ArrayOfShareGroups a2)
{
    size_t cnt = a1.GetCount();
    if (cnt != a2.GetCount())
        return true;
    ArrayOfShareGroups ca = a2;
    for (size_t i = 0; i < cnt; i++) {
        size_t cnt2 = ca.GetCount();
        int idx = wxNOT_FOUND;
        for (size_t j = 0; j < cnt2; j++) {
            if (a1[i] == ca[j]) {
                idx = j;
                break;
            }
        }
        if (idx == wxNOT_FOUND)
            return true;
        ca.RemoveAt(idx);
    }
    return (ca.GetCount() != 0);
}

// Compare two arrays of UsbForwards where sequence of elements is irrelevant
// return true, if arrays are different.
    bool
MyXmlConfig::cmpUsbForwards(ArrayOfUsbForwards a1, ArrayOfUsbForwards a2)
{
    size_t cnt = a1.GetCount();
    if (cnt != a2.GetCount())
        return true;
    ArrayOfUsbForwards ca = a2;
    for (size_t i = 0; i < cnt; i++) {
        size_t cnt2 = ca.GetCount();
        int idx = wxNOT_FOUND;
        for (size_t j = 0; j < cnt2; j++) {
            if (a1[i] == ca[j]) {
                idx = j;
                break;
            }
        }
        if (idx == wxNOT_FOUND)
            return true;
        ca.RemoveAt(idx);
    }
    return (ca.GetCount() != 0);
}

// Compare two wxArrayString where sequence of elements is irrelevant
// return true, if arrays are different.
    bool
MyXmlConfig::cmpUsedShareGroups(wxArrayString a1, wxArrayString a2)
{
    size_t cnt = a1.GetCount();
    if (cnt != a2.GetCount())
        return true;
    wxArrayString ca = a2;
    for (size_t i = 0; i < cnt; i++) {
        int idx = ca.Index(a1.Item(i));
        if (idx == wxNOT_FOUND)
            return true;
        ca.RemoveAt(idx);
    }
    return (ca.GetCount() != 0);
}

    bool
MyXmlConfig::operator ==(const MyXmlConfig &other)
{
    if (m_bDisableBackingstore != other.m_bDisableBackingstore) return false;
    if (m_bDisableComposite != other.m_bDisableComposite) return false;
    if (m_bDisableRender != other.m_bDisableRender) return false;
    if (m_bDisableShmem != other.m_bDisableShmem) return false;
    if (m_bDisableShpix != other.m_bDisableShpix) return false;
    if (m_bDisableTaint != other.m_bDisableTaint) return false;
    if (m_bDisableTcpNoDelay != other.m_bDisableTcpNoDelay) return false;
    if (m_bDisableXagent != other.m_bDisableXagent) return false;
    if (m_bDisableZlibCompression != other.m_bDisableZlibCompression) return false;
    if (m_bEnableMultimedia != other.m_bEnableMultimedia) return false;
    if (m_bEnableNativePA != other.m_bEnableNativePA) return false;
    if (m_bEnableMonoPA != other.m_bEnableMonoPA) return false;
    if (m_bEnableSmbSharing != other.m_bEnableSmbSharing) return false;
    if (m_bEnableSSL != other.m_bEnableSSL) return false;
    if (m_bEnableUSBIP != other.m_bEnableUSBIP) return false;
    if (m_bExternalProxy != other.m_bExternalProxy) return false;
    if (m_bGuestMode != other.m_bGuestMode) return false;
    if (m_bKbdLayoutOther != other.m_bKbdLayoutOther) return false;
    if (m_bProxyPassRemember != other.m_bProxyPassRemember) return false;
    if (m_bRdpCache != other.m_bRdpCache) return false;
    if (m_bRdpRememberPassword != other.m_bRdpRememberPassword) return false;
    if (m_bRdpRootless != other.m_bRdpRootless) return false;
    if (m_bRdpRunApplication != other.m_bRdpRunApplication) return false;
    if (m_bRememberPassword != other.m_bRememberPassword) return false;
    if (m_bRemoveOldSessionFiles != other.m_bRemoveOldSessionFiles) return false;
    if (m_bRunConsole != other.m_bRunConsole) return false;
    if (m_bRunXclients != other.m_bRunXclients) return false;
    if (m_bUseCups != other.m_bUseCups) return false;
    if (m_bUseCustomImageEncoding != other.m_bUseCustomImageEncoding) return false;
    if (m_bUseProxy != other.m_bUseProxy) return false;
    if (m_bUseSmartCard != other.m_bUseSmartCard) return false;
    if (m_bValid != other.m_bValid) return false;
    // Don't compare readonly flag
    if (m_bVirtualDesktop != other.m_bVirtualDesktop) return false;
    if (m_bVncRememberPassword != other.m_bVncRememberPassword) return false;
    if (m_bVncRootless != other.m_bVncRootless) return false;
    if (m_bVncUseNxAuth != other.m_bVncUseNxAuth) return false;
    if (m_bDisableDirectDraw != other.m_bDisableDirectDraw) return false;
    if (m_bDisableDeferredUpdates != other.m_bDisableDeferredUpdates) return false;
    if (m_bGrabKeyboard != other.m_bGrabKeyboard) return false;

    if (m_iClipFilter != other.m_iClipFilter) return false;
    if (m_iCupsPort != other.m_iCupsPort) return false;
    if (m_iDisplayHeight != other.m_iDisplayHeight) return false;
    if (m_iDisplayWidth != other.m_iDisplayWidth) return false;
    if (m_iImageEncoding != other.m_iImageEncoding) return false;
    if (m_iJpegQuality != other.m_iJpegQuality) return false;
    if (m_iProxyPort != other.m_iProxyPort) return false;
    if (m_iRdpAuthType != other.m_iRdpAuthType) return false;
    if (m_iRdpColors != other.m_iRdpColors) return false;
    if (m_iRdpImageEncoding != other.m_iRdpImageEncoding) return false;
    if (m_iRdpJpegQuality != other.m_iRdpJpegQuality) return false;
    if (m_iServerPort != other.m_iServerPort) return false;
    if (m_iSmbPort != other.m_iSmbPort) return false;
    if (m_iUsedShareGroups != other.m_iUsedShareGroups) return false;
    if (m_iVncDisplayNumber != other.m_iVncDisplayNumber) return false;
    if (m_iVncImageEncoding != other.m_iVncImageEncoding) return false;
    if (m_iVncJpegQuality != other.m_iVncJpegQuality) return false;
    if (m_iXdmBroadcastPort != other.m_iXdmBroadcastPort) return false;
    if (m_iXdmListPort != other.m_iXdmListPort) return false;
    if (m_iXdmQueryPort != other.m_iXdmQueryPort) return false;

    if (m_eCacheDisk != other.m_eCacheDisk) return false;
    if (m_eCacheMemory != other.m_eCacheMemory) return false;
    if (m_eConnectionSpeed != other.m_eConnectionSpeed) return false;
    if (m_eDesktopType != other.m_eDesktopType) return false;
    if (m_eDisplayType != other.m_eDisplayType) return false;
    if (m_eSessionType != other.m_eSessionType) return false;
    if (m_eXdmMode != other.m_eXdmMode) return false;
    if (m_eRatePA != other.m_eRatePA) return false;

    if (m_sCommandLine != other.m_sCommandLine) return false;
    if (m_sCupsPath != other.m_sCupsPath) return false;
    // if (m_sFileName != other.m_sFileName) return false;
    if (m_sKbdLayoutLanguage != other.m_sKbdLayoutLanguage) return false;
    if (m_sName != other.m_sName) return false;
    if (m_sPassword != other.m_sPassword) return false;
    if (m_sProxyCommand != other.m_sProxyCommand) return false;
    if (m_sProxyHost != other.m_sProxyHost) return false;
    if (m_sProxyPass != other.m_sProxyPass) return false;
    if (m_sProxyUser != other.m_sProxyUser) return false;
    if (m_sRdpApplication != other.m_sRdpApplication) return false;
    if (m_sRdpDomain != other.m_sRdpDomain) return false;
    if (m_sRdpHostName != other.m_sRdpHostName) return false;
    if (m_sRdpPassword != other.m_sRdpPassword) return false;
    if (m_sRdpUsername != other.m_sRdpUsername) return false;
    if (m_sServerHost != other.m_sServerHost) return false;
    if (m_sSshKey != other.m_sSshKey) return false;
    if (m_sUsername != other.m_sUsername) return false;
    if (m_sVncHostName != other.m_sVncHostName) return false;
    if (m_sVncPassword != other.m_sVncPassword) return false;
    if (m_sXdmListHost != other.m_sXdmListHost) return false;
    if (m_sXdmQueryHost != other.m_sXdmQueryHost) return false;

    if (cmpUsedShareGroups(m_aUsedShareGroups, other.m_aUsedShareGroups)) return false;
    if (cmpShareGroups(m_aShareGroups, other.m_aShareGroups)) return false;
    if (cmpUsbForwards(m_aUsbForwards, other.m_aUsbForwards)) return false;
    return true;
}

    bool
MyXmlConfig::LoadFromString(const wxString &content, bool isPush)
{
    char *cnt = strdup(content.mb_str());
    wxMemoryInputStream mis(cnt, strlen(cnt));
    bool ret = loadFromStream(mis, isPush);
    free(cnt);
    return ret;
}

    bool
MyXmlConfig::LoadFromFile(const wxString &filename)
{
    {
        wxLogNull dummy; 
        wxFile *f = new wxFile(filename);
        if ((!f->IsOpened()) || f->Eof()) {
            delete f;
            return false;
        }
        delete f;
    }
    myLogTrace(MYTRACETAG, wxT("Reading %s"), VMB(filename));
    wxFileInputStream fis(filename);
    if (loadFromStream(fis, false)) {
        m_sName = wxFileName(filename).GetName();
        m_sFileName = wxFileName(filename).GetFullPath();
        m_bWritable = wxFileName::IsFileWritable(m_sFileName);
        return true;
    }
    return false;
}

size_t MyXmlConfig::CurlWriteCallback(void *buffer, size_t size, size_t nmemb, void *userp)
{
#ifdef APP_OPENNX
# ifdef HAVE_LIBCURL
    if (userp) {
        wxMemoryOutputStream *mos = (wxMemoryOutputStream *)userp;
        return mos->GetOutputStreamBuffer()->Write(buffer, size * nmemb);
        // return size * nmemb;
    }
# endif
#else
    wxUnusedVar(buffer);
    wxUnusedVar(size);
    wxUnusedVar(nmemb);
    wxUnusedVar(userp);
#endif
    return -1;
}

    bool
MyXmlConfig::LoadFromURL(const wxString &filename)
{
    bool ret = false;
#ifdef APP_OPENNX
# ifdef HAVE_LIBCURL
    wxMemoryOutputStream mos;
    char ebuf[CURL_ERROR_SIZE];
    CURL *c = curl_easy_init();
    // curl_easy_setopt(c, CURLOPT_FAILONERROR, 1);
    // curl_easy_setopt(c, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(c, CURLOPT_NOPROGRESS, 1);
    curl_easy_setopt(c, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(c, CURLOPT_URL, (const char *)filename.mb_str());
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(c, CURLOPT_MAXREDIRS, 10);
    // curl_easy_setopt(c, CURLOPT_COOKIE, "foo=bar");
    curl_easy_setopt(c, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 2);
    if (!::wxGetApp().GetCaCert().IsEmpty())
        curl_easy_setopt(c, CURLOPT_CAINFO, (const char *)::wxGetApp().GetCaCert().mb_str());
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &mos);
    curl_easy_setopt(c, CURLOPT_ERRORBUFFER, ebuf);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    myLogTrace(MYTRACETAG, wxT("Fetching %s"), VMB(filename));
    CURLcode r = curl_easy_perform(c);
    if (0 == r) {
        off_t len = mos.TellO();
        if (len > 0) {
            int rcode = 200;
            if (filename.StartsWith(wxT("http"))) {
                curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &rcode);
            }
            if (200 == rcode) {
                myLogTrace(MYTRACETAG, wxT("Fetching %s"), VMB(filename));
                char * const data = new char[len];
                mos.CopyTo(data, len);
                wxMemoryInputStream mis(data, len);
                if (loadFromStream(mis, false)) {
                    wxURI uri(filename);
                    m_sName = wxFileName(uri.GetPath()).GetName();
                    if (0 == uri.GetScheme().CmpNoCase(wxT("file"))) {
                        m_sFileName = wxFileName(uri.GetPath()).GetFullPath();
                        m_bWritable = wxFileName::IsFileWritable(m_sFileName);
                    } else {
                        wxURL url(filename);
                        m_sFileName = url.BuildUnescapedURI();
                        m_bWritable = false;
                    }
                    ret = true;
                }
                delete data;
            } else {
                wxLogError(_("Error %d while fetching session configuration"), rcode);
            }
        }
    } else {
        wxString msg(ebuf, *wxConvCurrent);
        wxLogError(_("Error while fetching session configuration:\n%s"), VMB(msg));
    }
    curl_easy_cleanup(c);
# else
    wxURL url(filename);
    {
        wxLogNull dummy;
        if (!url.IsOk())
            return false;
    }
    myLogTrace(MYTRACETAG, wxT("Fetching %s"), VMB(filename));
    wxInputStream *is = url.GetInputStream();
    if (is && loadFromStream(*is, false)) {
        wxURI uri(filename);
        m_sName = wxFileName(uri.GetPath()).GetName();
        if (0 == uri.GetScheme().CmpNoCase(wxT("file"))) {
            m_sFileName = wxFileName(uri.GetPath()).GetFullPath();
            m_bWritable = wxFileName::IsFileWritable(m_sFileName);
        } else {
            m_sFileName = url.BuildUnescapedURI();
            m_bWritable = false;
        }
        ret = true;
    }
# endif
#else
    wxUnusedVar(filename);
#endif
    return ret;
}

    bool
MyXmlConfig::loadFromStream(wxInputStream &is, bool isPush)
{
    wxString tmp;
    int itmp;
    wxXmlDocument cfg(is);

    wxXmlNode *cfgnode = cfg.GetRoot();
    if (cfgnode && (cfgnode->GetName() == wxT("NXClientSettings"))) {
        if (!(cfgnode->GetAttribute(wxT("application"), wxEmptyString) == wxT("nxclient")))
            return false;
        if (!(cfgnode->GetAttribute(wxT("version"), wxEmptyString) == wxT("1.3")))
            return false;
        cfgnode = cfgnode->GetChildren();
        while (cfgnode) {
            if (cfgnode->GetName() == wxT("group")) {

                // Tab "Advanced"
                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("Advanced")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {

                        itmp = getLong(opt, wxT("Cache size"), -1);
                        switch (itmp) {
                            case 0:
                                m_eCacheMemory = CACHEMEM_0MB;
                                break;
                            case 1:
                                m_eCacheMemory = CACHEMEM_1MB;
                                break;
                            case 2:
                                m_eCacheMemory = CACHEMEM_2MB;
                                break;
                            case 4:
                                m_eCacheMemory = CACHEMEM_4MB;
                                break;
                            case 8:
                                m_eCacheMemory = CACHEMEM_8MB;
                                break;
                            case 16:
                                m_eCacheMemory = CACHEMEM_16MB;
                                break;
                            case 32:
                                m_eCacheMemory = CACHEMEM_32MB;
                                break;
                            case 64:
                                m_eCacheMemory = CACHEMEM_64MB;
                                break;
                            case 128:
                                m_eCacheMemory = CACHEMEM_128MB;
                                break;
                        }
                        itmp = getLong(opt, wxT("Cache size on disk"), -1);
                        switch (itmp) {
                            case 0:
                                m_eCacheDisk = CACHEDISK_0MB;
                                break;
                            case 4:
                                m_eCacheDisk = CACHEDISK_4MB;
                                break;
                            case 8:
                                m_eCacheDisk = CACHEDISK_8MB;
                                break;
                            case 16:
                                m_eCacheDisk = CACHEDISK_16MB;
                                break;
                            case 32:
                                m_eCacheDisk = CACHEDISK_32MB;
                                break;
                            case 64:
                                m_eCacheDisk = CACHEDISK_64MB;
                                break;
                            case 128:
                                m_eCacheDisk = CACHEDISK_128MB;
                                break;
                            case 256:
                                m_eCacheDisk = CACHEDISK_256MB;
                                break;
                            case 512:
                                m_eCacheDisk = CACHEDISK_512MB;
                                break;
                        }
#if 0
                        if (wxGetEnv(wxT("THINCLIENT"), NULL))
                            m_eCacheDisk = CACHEDISK_0MB;
#endif
                        m_bKbdLayoutOther = !getBool(opt, wxT("Current keyboard"),
                                !m_bKbdLayoutOther);
                        m_sKbdLayoutLanguage = getString(opt,
                                wxT("Custom keyboard layout"), m_sKbdLayoutLanguage);
                        m_bDisableTcpNoDelay = getBool(opt, wxT("Disable TCP no-delay"),
                                m_bDisableTcpNoDelay);
                        m_bDisableZlibCompression = getBool(opt,
                                wxT("Disable ZLIB stream compression"),
                                m_bDisableZlibCompression);
                        m_bGrabKeyboard = getBool(opt, wxT("Grab keyboard"),
                                m_bGrabKeyboard);
                        m_bDisableDirectDraw = getBool(opt, wxT("Disable DirectDraw"),
                                m_bDisableDirectDraw);
                        m_bDisableDeferredUpdates = getBool(opt, wxT("Disable deferred updates"),
                                m_bDisableDeferredUpdates);

                        tmp = getString(opt, wxT("Clipboard filter"), wxEmptyString);
                        if (!tmp.IsEmpty()) {
                            myLogTrace(MYTRACETAG, wxT("read: Clipboard filter '%s'"), VMB(tmp));
                            if (tmp.CmpNoCase(wxT("primary")) == 0) {
                                m_iClipFilter = 0;
                            }
                            if (tmp.CmpNoCase(wxT("clipboard")) == 0) {
                                m_iClipFilter = 1;
                            }
                            if (tmp.CmpNoCase(wxT("both")) == 0) {
                                m_iClipFilter = 2;
                            }
                            myLogTrace(MYTRACETAG, wxT("read: m_iClipFilter=%d"), (int)m_iClipFilter);
                        }
                        m_bUseProxy = getBool(opt, wxT("Enable HTTP proxy"), m_bUseProxy);
                        m_bExternalProxy = getBool(opt, wxT("Enable external proxy"), m_bExternalProxy);
                        m_bEnableSSL = getBool(opt, wxT("Enable SSL encryption"), m_bEnableSSL);
                        m_bEnableUSBIP = getBool(opt, wxT("Enable USBIP"), m_bEnableUSBIP);
                        // Enable response time optimizations false
                        m_sProxyCommand = getString(opt, wxT("Proxy command"), m_sProxyCommand);
                        m_sProxyHost = getString(opt, wxT("HTTP proxy host"), m_sProxyHost);
                        m_iProxyPort = getLong(opt, wxT("HTTP proxy port"), m_iProxyPort);
                        m_sProxyUser = getString(opt, wxT("HTTP proxy username"), m_sProxyUser);
                        m_bProxyPassRemember = getBool(opt, wxT("Remember HTTP proxy password"), m_bProxyPassRemember);
                        if (m_bProxyPassRemember)
                            m_sProxyPass = getPassword(opt, wxT("HTTP proxy password"), m_sProxyPass);
                        else
                            m_sProxyPass = wxEmptyString;
                        // Restore cache true
                        // StreamCompression ""

                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                // Tab "Environment"
                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("Environment")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {
                        m_sCupsPath = getString(opt, wxT("CUPSD path"), m_sCupsPath);
                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                // Tab "General"
                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("General")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {

                        // Automatic reconnect true
                        m_sCommandLine = getString(opt, wxT("Command line"), m_sCommandLine);
                        tmp = getString(opt,
                                wxT("Custom Unix Desktop"), wxEmptyString);
                        if (!tmp.IsEmpty()) {
                            if (tmp.CmpNoCase(wxT("application")) == 0) {
                                m_bRunConsole = false;
                                m_bRunXclients = false;
                            }
                            if (tmp.CmpNoCase(wxT("console")) == 0) {
                                m_bRunConsole = true;
                                m_bRunXclients = false;
                            }
                            if (tmp.CmpNoCase(wxT("default")) == 0) {
                                m_bRunConsole = false;
                                m_bRunXclients = true;
                            }
                        }
                        tmp = getString(opt, wxT("Desktop"));
                        if (tmp.CmpNoCase(wxT("RDP")) == 0)
                            m_eDesktopType = DTYPE_RDP;
                        if (tmp.CmpNoCase(wxT("RFB")) == 0)
                            m_eDesktopType = DTYPE_RFB;
                        if (tmp.CmpNoCase(wxT("KDE")) == 0)
                            m_eDesktopType = DTYPE_KDE;
                        if (tmp.CmpNoCase(wxT("Gnome")) == 0)
                            m_eDesktopType = DTYPE_GNOME;
                        if (tmp.CmpNoCase(wxT("CDE")) == 0)
                            m_eDesktopType = DTYPE_CDE;
                        if (tmp.CmpNoCase(wxT("XFCE")) == 0)
                            m_eDesktopType = DTYPE_XFCE;
                        if (tmp.CmpNoCase(wxT("XDM")) == 0)
                            m_eDesktopType = DTYPE_XDM;
                        if (tmp.CmpNoCase(wxT("Console")) == 0)
                            m_eDesktopType = DTYPE_CUSTOM;
                        m_bDisableShmem = getBool(opt, wxT("Disable SHM"), m_bDisableShmem);
                        m_bDisableShpix = getBool(opt, wxT("Disable emulate shared pixmaps"), m_bDisableShpix);
                        tmp = getString(opt, wxT("Link speed"));
                        if (tmp == wxT("modem"))
                            m_eConnectionSpeed = SPEED_MODEM;
                        if (tmp == wxT("isdn"))
                            m_eConnectionSpeed = SPEED_ISDN;
                        if (tmp == wxT("adsl"))
                            m_eConnectionSpeed = SPEED_ADSL;
                        if (tmp == wxT("wan"))
                            m_eConnectionSpeed = SPEED_WAN;
                        if (tmp == wxT("lan"))
                            m_eConnectionSpeed = SPEED_LAN;

                        m_bRememberPassword = getBool(opt, wxT("Remember password"),
                                m_bRememberPassword);

                        tmp = getString(opt, wxT("Resolution"));
                        if (!tmp.IsEmpty())
                            m_eDisplayType = DPTYPE_CUSTOM;
                        if (tmp == wxT("640x480"))
                            m_eDisplayType = DPTYPE_640x480;
                        if (tmp == wxT("800x600"))
                            m_eDisplayType = DPTYPE_800x600;
                        if (tmp == wxT("1024x768"))
                            m_eDisplayType = DPTYPE_1024x768;
                        if (tmp == wxT("1280x1024"))
                            m_eDisplayType = DPTYPE_1280x1024;
                        if (tmp == wxT("1400x1050"))
                            m_eDisplayType = DPTYPE_1400x1050;
                        if (tmp == wxT("1440x900"))
                            m_eDisplayType = DPTYPE_1440x900;
                        if (tmp == wxT("1680x1050"))
                            m_eDisplayType = DPTYPE_1680x1050;
                        if (tmp == wxT("1920x1080"))
                            m_eDisplayType = DPTYPE_1920x1080;
                        if (tmp == wxT("available"))
                            m_eDisplayType = DPTYPE_AVAILABLE;
                        if (tmp == wxT("fullscreen"))
                            m_eDisplayType = DPTYPE_FULLSCREEN;
                        if (tmp == wxT("remote"))
                            m_eDisplayType = DPTYPE_REMOTE;
                        if (tmp == wxT("nodecoration"))
                            m_eDisplayType = DPTYPE_NODECORATION;

                        m_iDisplayHeight = getLong(opt, wxT("Resolution height"),
                                m_iDisplayHeight);
                        m_iDisplayWidth = getLong(opt, wxT("Resolution width"),
                                m_iDisplayWidth);
                        m_sServerHost = getString(opt, wxT("Server host"), m_sServerHost);
                        m_iServerPort = getLong(opt, wxT("Server port"), m_iServerPort);

                        tmp = getString(opt, wxT("Session"));
                        if (tmp.CmpNoCase(wxT("application")) == 0) {
                            m_eSessionType = STYPE_UNIX;
                            m_bValid = true;
                        }
                        if (tmp.CmpNoCase(wxT("unix")) == 0) {
                            m_eSessionType = STYPE_UNIX;
                            m_bValid = true;
                        }
                        if (tmp.CmpNoCase(wxT("windows")) == 0) {
                            m_eSessionType = STYPE_WINDOWS;
                            m_bValid = true;
                        }
                        if (tmp.CmpNoCase(wxT("vnc")) == 0) {
                            m_eSessionType = STYPE_VNC;
                            m_bValid = true;
                        }
                        if (tmp.CmpNoCase(wxT("shadow")) == 0) {
                            m_eSessionType = STYPE_SHADOW;
                            m_bValid = true;
                        }
                        // this is 0/1 in NX but getBool can handle that
                        m_bUseCustomImageEncoding = getBool(opt,
                                wxT("Use default image encoding"), m_bUseCustomImageEncoding);
                        m_bDisableRender = !getBool(opt, wxT("Use render"), !m_bDisableRender);
                        m_bUseSmartCard = getBool(opt, wxT("Use smartcard"), m_bUseSmartCard);
                        m_bDisableTaint = !getBool(opt, wxT("Use taint"), !m_bDisableTaint);
                        m_bVirtualDesktop = getBool(opt, wxT("Virtual desktop"),
                                m_bVirtualDesktop);
                        m_bDisableXagent = !getBool(opt, wxT("XAgent encoding"),
                                !m_bDisableXagent);
                        // displaySaveOnExit  = true

                        tmp = getString(opt, wxT("xdm mode"));
                        if (tmp.CmpNoCase(wxT("server decide")) == 0) {
                            m_eXdmMode = XDM_MODE_SERVER;
                        }
                        if (tmp.CmpNoCase(wxT("server_decide")) == 0) {
                            m_eXdmMode = XDM_MODE_SERVER;
                        }
                        if (tmp.CmpNoCase(wxT("query")) == 0) {
                            m_eXdmMode = XDM_MODE_QUERY;
                        }
                        if (tmp.CmpNoCase(wxT("broadcast")) == 0) {
                            m_eXdmMode = XDM_MODE_BROADCAST;
                        }
                        if (tmp.CmpNoCase(wxT("list")) == 0) {
                            m_eXdmMode = XDM_MODE_LIST;
                        }
                        m_sXdmListHost = getString(opt, wxT("xdm list host"), m_sXdmListHost);
                        m_sXdmQueryHost = getString(opt, wxT("xdm query host"), m_sXdmQueryHost);
                        m_iXdmBroadcastPort = getLong(opt, wxT("xdm broadcast port"), m_iXdmBroadcastPort);
                        m_iXdmListPort = getLong(opt, wxT("xdm list port"), m_iXdmListPort);
                        m_iXdmQueryPort = getLong(opt, wxT("xdm query port"), m_iXdmQueryPort);

                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                // Sub-Dialog Custom image compresion
                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("Images")) {
                    // First, determine, if we have an old (pre 0.16.173) config file
                    wxXmlNode *opt = cfgnode->GetChildren();
                    m_bOldConfig = false;
                    while (opt) {
                        wxString key = opt->GetAttribute(wxT("key"), wxEmptyString);
                        if (key == wxT("Windows Image Compression")) {
                            m_bOldConfig = true;
                            break;
                        }
                        opt = opt->GetNext();
                    }
                    opt = cfgnode->GetChildren();
                    while (opt) {
                        m_bDisableBackingstore = getBool(opt, wxT("Disable backingstore"),
                                m_bDisableBackingstore);
                        m_bDisableComposite = getBool(opt, wxT("Disable coposite"),
                                m_bDisableComposite);

                        m_iImageEncoding = getLong(opt, wxT("Image Compression Type"), m_iImageEncoding);
                        m_iJpegQuality = getLong(opt, wxT("JPEG Quality"), m_iJpegQuality);
                        // RDP optimization for low-bandwidth link = false
                        // Reduce colors to = ""
                        m_iVncImageEncoding = getLong(opt, wxT("VNC images compression"),
                                m_iVncImageEncoding);
                        m_iVncJpegQuality = getLong(opt, wxT("VNC JPEG Quality"), m_iVncJpegQuality);
                        m_iRdpImageEncoding = getLong(opt, wxT("RDP Image Encoding"),
                                m_iRdpImageEncoding);
                        m_iRdpJpegQuality = getLong(opt, wxT("RDP JPEG Quality"), m_iRdpJpegQuality);
                        opt = opt->GetNext();
                    }
                    if (m_bOldConfig) {
                        if (m_bUseCustomImageEncoding) {
                            // Convert from old settings format
                            switch (m_iImageEncoding) {
                                case 0:
                                    m_iImageEncoding = 3;
                                    break;
                                case 1:
                                    m_iImageEncoding = 4;
                                    break;
                                case 2:
                                    m_iImageEncoding = 2;
                                    break;
                            }
                            m_iRdpJpegQuality = m_iVncJpegQuality = m_iJpegQuality;

                            opt = cfgnode->GetChildren();
                            int oldvncenc = 0;
                            int oldrdpenc = 0;
                            while (opt) {
                                oldvncenc = getLong(opt, wxT("Image Encoding Type"), oldvncenc);
                                oldrdpenc = getLong(opt, wxT("Windows Image Compression"), oldrdpenc);
                                opt = opt->GetNext();
                            }
                            m_iVncImageEncoding = (oldvncenc + 1) % 3;
                            switch (oldrdpenc) {
                                case 0:
                                    m_iRdpImageEncoding = 3;
                                    break;
                                case 1:
                                    m_iRdpImageEncoding = 1;
                                    break;
                                case 2:
                                    m_iRdpImageEncoding = 2;
                                    break;
                                case 3:
                                    m_iRdpImageEncoding = 0;
                                    break;
                            }
                        } else {
                            // Custom settings are disabled, so simply set defaults ...
                            m_iImageEncoding =  m_iRdpImageEncoding =  m_iVncImageEncoding = 3;
                            m_iJpegQuality = m_iRdpJpegQuality = m_iVncJpegQuality = 6;
                            // ... and shut up (no need to bother the user)
                            m_bOldConfig = false;
                        }
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                // Main login params
                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("Login")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {
                        m_bGuestMode = getBool(opt, wxT("Guest Mode"), m_bGuestMode);
                        if (m_bGuestMode && isPush) {
                            m_sGuestUser = getString(opt, wxT("User"), m_sGuestUser);
                            tmp = getString(opt, wxT("Auth"), wxEmptyString);
                            if (!tmp.IsEmpty())
                                m_sGuestPassword = cryptString(tmp);
                        } else {
                            m_pMd5Password = getStringNew(opt, wxT("Auth"), m_pMd5Password);
                            m_sUsername = getString(opt, wxT("User"), m_sUsername);
                            m_sGuestPassword = getString(opt, wxT("Guest password"),
                                    m_sGuestPassword);
                            m_sGuestUser = getString(opt, wxT("Guest username"), m_sGuestUser);
                        }
                        // Login method = "nx"
                        tmp = m_sSshKey;
                        m_sSshKey = getString(opt, wxT("Public Key"), m_sSshKey);
                        if (tmp != m_sSshKey) {
                            if (!m_sSshKey.IsEmpty()) {
                                wxRegEx r(wxT("^(-----BEGIN .SA PRIVATE KEY-----)\\s+(.*)(-----END .SA PRIVATE KEY-----)\\s*$"), wxRE_ADVANCED);
                                if (r.Matches(m_sSshKey)) {
                                    tmp = r.GetMatch(m_sSshKey, 2);
                                    tmp.Replace(wxT(" "), wxT("\n"));
                                    m_sSshKey = r.GetMatch(m_sSshKey, 1) + wxT("\n")
                                        + tmp + r.GetMatch(m_sSshKey, 3) + wxT("\n");
                                }
                            }
                        }
                        m_pClrPassword = getStringNew(opt, wxT("Password"), m_pClrPassword);

                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                // Tab "Services"
                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("Services")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {
                        m_bEnableMultimedia = getBool(opt, wxT("Audio"), m_bEnableMultimedia);
                        m_iCupsPort = getLong(opt, wxT("IPPPort"), m_iCupsPort);
                        m_iSmbPort = getLong(opt, wxT("SmbDefaultPort"), m_iSmbPort);
                        m_bUseCups = getBool(opt, wxT("IPPPrinting"), m_bUseCups);
                        m_bEnableSmbSharing = getBool(opt, wxT("Shares"), m_bEnableSmbSharing);
                        m_bEnableNativePA = getBool(opt, wxT("NativePA"), m_bEnableNativePA);
                        tmp = getString(opt, wxT("RatePA"));
                        if (tmp == wxT("NoResample"))
                            m_eRatePA = RATEPA_NORESAMPLE;
                        if (tmp == wxT("48000"))
                            m_eRatePA = RATEPA_48000;
                        if (tmp == wxT("44100"))
                            m_eRatePA = RATEPA_44100;
                        if (tmp == wxT("32000"))
                            m_eRatePA = RATEPA_32000;
                        if (tmp == wxT("16000"))
                            m_eRatePA = RATEPA_16000;
                        if (tmp == wxT("8000"))
                            m_eRatePA = RATEPA_8000;
                        m_bEnableMonoPA = getBool(opt, wxT("MonoPA"), m_bEnableMonoPA);
                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("VNC Session")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {
                        tmp = getString(opt, wxT("Display"));
                        if (tmp.Left(1) == wxT(":")) {
                            tmp = tmp.Mid(1);
                            if (tmp.IsNumber()) {
                                long l;
                                tmp.ToLong(&l, 0);
                                m_iVncDisplayNumber = l;
                            }
                        }
                        m_sVncPassword = getPassword(opt, wxT("Password"), m_sVncPassword);
                        m_bVncRememberPassword = getBool(opt, wxT("Remember"),
                                m_bVncRememberPassword);
                        m_bVncRootless = getBool(opt, wxT("Rootless"),
                                m_bVncRootless);
                        m_sVncHostName = getString(opt, wxT("Server"), m_sVncHostName);
                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("Windows Session")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {
                        m_sRdpApplication = getString(opt, wxT("Application"),
                                m_sRdpApplication);
                        m_iRdpAuthType = getLong(opt, wxT("Authentication"), m_iRdpAuthType);
                        itmp = -1;
                        switch (getLong(opt, wxT("Color Depth"), itmp)) {
                            case 8:
                                m_iRdpColors = 0;
                                break;
                            case 15:
                                m_iRdpColors = 1;
                                break;
                            case 16:
                                m_iRdpColors = 2;
                                break;
                            case 24:
                                m_iRdpColors = 3;
                                break;
                        }
                        m_sRdpDomain = getString(opt, wxT("Domain"), m_sRdpDomain);
                        m_bRdpCache = getBool(opt, wxT("Image Cache"), m_bRdpCache);
                        m_sRdpPassword = getPassword(opt, wxT("Password"), m_sRdpPassword);
                        m_bRdpRememberPassword = getBool(opt, wxT("Remember"),
                                m_bRdpRememberPassword);
                        m_bRdpRootless = getBool(opt, wxT("Rootless"),
                                m_bRdpRootless);
                        m_bRdpRunApplication = getBool(opt, wxT("Run application"),
                                m_bRdpRunApplication);
                        m_sRdpHostName = getString(opt, wxT("Server"), m_sRdpHostName);
                        m_sRdpUsername = getString(opt, wxT("User"), m_sRdpUsername);
                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString) == wxT("share chosen")) {
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {
                        wxString key = opt->GetAttribute(wxT("key"), wxEmptyString);
                        if (key == wxT("Share number")) {
                            m_iUsedShareGroups = getLong(opt, wxT("Share number"),
                                    m_iUsedShareGroups);
                        } else if (key == wxT("default printer")) {
                            // Ignoring this key, because it is set in the share as well.
                        } else
                            m_aUsedShareGroups.Add(opt->GetAttribute(wxT("value"), wxEmptyString));
                        opt = opt->GetNext();
                    }
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                // When we reach here, we got an "unknown" group name. This is usually
                // either a mounted share description or an UsbForward entry.
                if (cfgnode->GetAttribute(wxT("name"), wxEmptyString).StartsWith(wxT("UsbForward"))) {
                    SharedUsbDevice dev;
                    wxXmlNode *opt = cfgnode->GetChildren();
                    int optcount = 0;
                    while (opt) {
                        wxString key = opt->GetAttribute(wxT("key"), wxEmptyString);
                        if (key == wxT("Vendor")) {
                            optcount++;
                            dev.m_sVendor = getString(opt, wxT("Vendor"), wxEmptyString);
                            opt = opt->GetNext();
                            continue;
                        }
                        if (key == wxT("Product")) {
                            optcount++;
                            dev.m_sProduct = getString(opt, wxT("Product"), wxEmptyString);
                            opt = opt->GetNext();
                            continue;
                        }
                        if (key == wxT("Serial")) {
                            optcount++;
                            dev.m_sSerial = getString(opt, wxT("Serial"), wxEmptyString);
                            opt = opt->GetNext();
                            continue;
                        }
                        if (key == wxT("VendorID")) {
                            optcount++;
                            dev.m_iVendorID = getLong(opt, wxT("VendorID"), 0);
                            opt = opt->GetNext();
                            continue;
                        }
                        if (key == wxT("ProductID")) {
                            optcount++;
                            dev.m_iProductID = getLong(opt, wxT("ProductID"), 0);
                            opt = opt->GetNext();
                            continue;
                        }
                        if (key == wxT("Class")) {
                            optcount++;
                            dev.m_iClass = getLong(opt, wxT("Class"), 0);
                            opt = opt->GetNext();
                            continue;
                        }
                        if (key == wxT("Mode")) {
                            optcount++;
                            wxString tmp = getString(opt, wxT("Mode"), wxEmptyString);
                            if (tmp.IsSameAs(wxT("local")))
                                dev.m_eMode = SharedUsbDevice::MODE_LOCAL;
                            if (tmp.IsSameAs(wxT("remote")))
                                dev.m_eMode = SharedUsbDevice::MODE_REMOTE;
                            opt = opt->GetNext();
                            continue;
                        }
                        opt = opt->GetNext();
                    }
                    if ((7 == optcount) && (dev.m_eMode != SharedUsbDevice::MODE_UNKNOWN))
                        m_aUsbForwards.Add(dev);
                    cfgnode = cfgnode->GetNext();
                    continue;
                }

                // When we reach here, we got an "unknown" group name. This is usually
                // a mounted share description.
                {
                    int shareOptions = 0;
                    ShareGroup s;
                    s.m_sGroupName = cfgnode->GetAttribute(wxT("name"), wxEmptyString);
                    s.m_eType = SharedResource::SHARE_UNKNOWN;
                    wxXmlNode *opt = cfgnode->GetChildren();
                    while (opt) {
                        wxString key = opt->GetAttribute(wxT("key"), wxEmptyString);
                        if (key == wxT("Alias")) {
                            shareOptions++;
                            s.m_sAlias = getString(opt, wxT("Alias"), wxEmptyString);
                        }
                        if (key == wxT("Default")) {
                            shareOptions++;
                            s.m_bDefault = getBool(opt, wxT("Default"), false);
                        }
                        if (key == wxT("Driver")) {
                            shareOptions++;
                            s.m_sDriver = getString(opt, wxT("Driver"), wxEmptyString);
                        }
                        if (key == wxT("Password")) {
                            shareOptions++;
                            s.m_sPassword = getPassword(opt, wxT("Password"), wxEmptyString);
                        }
                        if (key == wxT("Public")) {
                            shareOptions++;
                            s.m_bPublic = getBool(opt, wxT("Public"), false);
                        }
                        if (key == wxT("Sharename")) {
                            shareOptions++;
                            s.m_sShareName = getString(opt, wxT("Sharename"), wxEmptyString);
                        }
                        if (key == wxT("Type")) {
                            shareOptions++;
                            wxString tmp = getString(opt, wxT("Type"), wxEmptyString);
                            if (tmp.CmpNoCase(wxT("cupsprinter")) == 0)
                                s.m_eType = SharedResource::SHARE_CUPS_PRINTER;
                            if (tmp.CmpNoCase(wxT("disk")) == 0)
                                s.m_eType = SharedResource::SHARE_SMB_DISK;
                            if (tmp.CmpNoCase(wxT("smbprinter")) == 0)
                                s.m_eType = SharedResource::SHARE_SMB_PRINTER;
                        }
                        if (key == wxT("Username")) {
                            shareOptions++;
                            s.m_sUsername = getString(opt, wxT("Username"), wxEmptyString);
                        }
                        opt = opt->GetNext();
                    }
                    if (shareOptions >= 5)
                        m_aShareGroups.Add(s);
                }

            }
            cfgnode = cfgnode->GetNext();
        }
    }
    if (m_bValid) {
        if (m_pMd5Password) {
            m_sPassword = DUMMY_MD5_PASSWORD;
        } else if (m_pClrPassword) {
            m_sPassword = DUMMY_CLR_PASSWORD;
        }
        if (STYPE_SHADOW == m_eSessionType)
            m_eDesktopType = DTYPE_ANY;
    } else {
        m_sUsername = wxEmptyString;
        m_sPassword = wxEmptyString;
    }
    return m_bValid;
}

    wxString
MyXmlConfig::sGetSessionUser()
{
    if (m_bGuestMode) {
        if (m_sGuestUser.IsEmpty())
            return wxT("NX guest user");
        else
            return m_sGuestUser;
    } else
        return m_sUsername;
}

    wxString
MyXmlConfig::sGetSessionPassword()
{
    wxString ret;
    if (m_bGuestMode) {
        ret = decryptString(m_sGuestPassword);
    } else {
        if ((m_sPassword == DUMMY_MD5_PASSWORD) && m_pMd5Password)
            ret = decryptString(*m_pMd5Password);
        else
            ret = m_sPassword;
    }
    return ret;
}

    void
MyXmlConfig::sSetUsername(const wxString &s)
{
    bool b;
    wxConfigBase::Get()->Read(wxT("Config/LowercaseLogin"), &b, false);
    if (b) {
        m_sUsername = s.Lower();
    } else {
        m_sUsername = s;
    }
}

    void 
MyXmlConfig::bAddOption(wxXmlNode *group, const wxString &name, const bool val)
{
    wxXmlNode *n = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("option"));
    n->AddAttribute(new wxXmlAttribute(wxT("key"), name, NULL));
    n->AddAttribute(new wxXmlAttribute(wxT("value"), val ? wxT("true") : wxT("false"), NULL));
    group->AddChild(n);
}

    void
MyXmlConfig::iAddOptionBool(wxXmlNode *group, const wxString &name, const bool val)
{
    wxXmlNode *n = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("option"));
    n->AddAttribute(new wxXmlAttribute(wxT("key"), name, NULL));
    n->AddAttribute(new wxXmlAttribute(wxT("value"), wxString::Format(wxT("%d"), val ? 1 : 0), NULL));
    group->AddChild(n);
}

    void
MyXmlConfig::iAddOption(wxXmlNode *group, const wxString &name, const long val)
{
    wxXmlNode *n = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("option"));
    n->AddAttribute(new wxXmlAttribute(wxT("key"), name, NULL));
    n->AddAttribute(new wxXmlAttribute(wxT("value"), wxString::Format(wxT("%d"), (int)val), NULL));
    group->AddChild(n);
}

    void
MyXmlConfig::sAddOption(wxXmlNode *group, const wxString &name, const wxString &val)
{
    wxXmlNode *n = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("option"));
    n->AddAttribute(new wxXmlAttribute(wxT("key"), name, NULL));
    n->AddAttribute(new wxXmlAttribute(wxT("value"), val, NULL));
    group->AddChild(n);
}

    wxXmlNode *
MyXmlConfig::AddGroup(wxXmlNode *parent, const wxString &name)
{
    wxXmlNode *n = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("group"));
    n->AddAttribute(new wxXmlAttribute(wxT("name"), name, NULL));
    parent->AddChild(n);
    return n;
}

    bool
MyXmlConfig::SaveToFile()
{
    wxXmlDocument cfg;
    wxXmlNode *r;
    wxXmlNode *g;
    wxString optval;
    size_t i;

    if (m_sFileName.StartsWith(wxT("http://"))
            || m_sFileName.StartsWith(wxT("https://"))
            || m_sFileName.StartsWith(wxT("ftp://"))
       )
        return false;
    r = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("NXClientSettings"));
    r->AddAttribute(new wxXmlAttribute(wxT("application"), wxT("nxclient"), NULL));
    r->AddAttribute(new wxXmlAttribute(wxT("version"), wxT("1.3"), NULL));
    cfg.SetRoot(r);

    // SessionProperties, Tab "General"
    g = AddGroup(r, wxT("General"));
    bAddOption(g, wxT("Automatic reconnect"), true); // Not (yet?) configurable
    sAddOption(g, wxT("Command line"), m_sCommandLine);
    optval = wxT("application");
    if (m_bRunConsole)
        optval = wxT("console");
    if (m_bRunXclients)
        optval = wxT("default");
    sAddOption(g, wxT("Custom Unix Desktop"), optval);
    switch (m_eDesktopType) {
        case DTYPE_ANY:
            optval = wxT("any");
            break;
        case DTYPE_RDP:
            optval = wxT("rdp");
            break;
        case DTYPE_RFB:
            optval = wxT("rfb");
            break;
        case DTYPE_KDE:
            optval = wxT("kde");
            break;
        case DTYPE_GNOME:
            optval = wxT("gnome");
            break;
        case DTYPE_CDE:
            optval = wxT("cde");
            break;
        case DTYPE_XFCE:
            optval = wxT("xfce");
            break;
        case DTYPE_XDM:
            optval = wxT("xdm");
            break;
        case DTYPE_CUSTOM:
            optval = wxT("console");
            break;
    }
    sAddOption(g, wxT("Desktop"), optval);
    bAddOption(g, wxT("Disable SHM"), m_bDisableShmem); // Not in original GUI but in config ?!
    bAddOption(g, wxT("Disable emulate shared pixmaps"), m_bDisableShpix); // Not in original GUI but in config ?!
    switch (m_eConnectionSpeed) {
        case SPEED_MODEM:
            optval = wxT("modem");
            break;
        case SPEED_ISDN:
            optval = wxT("isdn");
            break;
        case SPEED_ADSL:
            optval = wxT("adsl");
            break;
        case SPEED_WAN:
            optval = wxT("wan");
            break;
        case SPEED_LAN:
            optval = wxT("lan");
            break;
    }
    sAddOption(g, wxT("Link speed"), optval);
    bAddOption(g, wxT("Remember password"), m_bRememberPassword);
    switch (m_eDisplayType) {
        case DPTYPE_CUSTOM:
            optval = wxString::Format(wxT("%dx%d"),
                    (int)m_iDisplayWidth, (int)m_iDisplayHeight);
            break;
        case DPTYPE_640x480:
            optval = wxT("640x480");
            break;
        case DPTYPE_800x600:
            optval = wxT("800x600");
            break;
        case DPTYPE_1024x768:
            optval = wxT("1024x768");
            break;
        case DPTYPE_1280x1024:
            optval = wxT("1280x1024");
            break;
        case DPTYPE_1400x1050:
            optval = wxT("1400x1050");
            break;
        case DPTYPE_1440x900:
            optval = wxT("1440x900");
            break;
        case DPTYPE_1680x1050:
            optval = wxT("1680x1050");
            break;
        case DPTYPE_1920x1080:
            optval = wxT("1920x1080");
            break;
        case DPTYPE_AVAILABLE:
            optval = wxT("available");
            break;
        case DPTYPE_FULLSCREEN:
            optval = wxT("fullscreen");
            break;
        case DPTYPE_REMOTE:
            optval = wxT("remote");
            break;
        case DPTYPE_NODECORATION:
            optval = wxT("nodecoration");
            break;
    }
    sAddOption(g, wxT("Resolution"), optval);
    iAddOption(g, wxT("Resolution height"), m_iDisplayHeight);
    iAddOption(g, wxT("Resolution width"), m_iDisplayWidth);
    sAddOption(g, wxT("Server host"), m_sServerHost);
    iAddOption(g, wxT("Server port"), m_iServerPort);
    switch (m_eSessionType) {
        case STYPE_UNIX:
            optval = wxT("unix");
            break;
        case STYPE_WINDOWS:
            optval = wxT("windows");
            break;
        case STYPE_VNC:
            optval = wxT("vnc");
            break;
        case STYPE_SHADOW:
            optval = wxT("shadow");
            break;
    }
    sAddOption(g, wxT("Session"), optval);
    iAddOption(g, wxT("Use default image encoding"), m_bUseCustomImageEncoding);
    bAddOption(g, wxT("Use render"), !m_bDisableRender);
    bAddOption(g, wxT("Use taint"), !m_bDisableTaint);
    bAddOption(g, wxT("Virtual desktop"), m_bVirtualDesktop);
    bAddOption(g, wxT("XAgent encoding"), !m_bDisableXagent);
    bAddOption(g, wxT("displaySaveOnExit"), true); // Not in original GUI but in config ?!
    iAddOption(g, wxT("xdm broadcast port"), m_iXdmBroadcastPort);
    sAddOption(g, wxT("xdm list host"), m_sXdmListHost);
    iAddOption(g, wxT("xdm list port"), m_iXdmListPort);
    switch (m_eXdmMode) {
        case XDM_MODE_SERVER:
            optval = wxT("server decide");
            break;
        case XDM_MODE_QUERY:
            optval = wxT("query");
            break;
        case XDM_MODE_BROADCAST:
            optval = wxT("broadcast");
            break;
        case XDM_MODE_LIST:
            optval = wxT("list");
            break;
    }
    sAddOption(g, wxT("xdm mode"), optval);
    sAddOption(g, wxT("xdm query host"), m_sXdmQueryHost);
    iAddOption(g, wxT("xdm query port"), m_iXdmQueryPort);
    bAddOption(g, wxT("Use smartcard"), m_bUseSmartCard); // Not in original

    // == UnixImageSettingsDialog, RdpImageSettingsDialog, VncImageSettingsDialog
    g = AddGroup(r, wxT("Images"));

    // deprecated but written by NX client, m_bDisableJpeg
    iAddOption(g, wxT("Disable JPEG Compression"), 0); 

    // deprecated but written by NX client, m_bDisableImageCompression
    bAddOption(g, wxT("Disable all image optimisations"), false);

    bAddOption(g, wxT("Disable backingstore"), m_bDisableBackingstore);
    bAddOption(g, wxT("Disable coposite"), m_bDisableComposite);

    // deprecated bAddOption(g, wxT("Disable image streaming"), m_bDisableStreaming);

    iAddOption(g, wxT("Image Compression Type"), m_iImageEncoding);

    // deprecated but written by NX client
    iAddOption(g, wxT("Image Encoding Type"), 0);

    // deprecated but written by NX client, m_bUseTightJpeg
    bAddOption(g, wxT("Image JPEG Encoding"), false);

    iAddOption(g, wxT("JPEG Quality"), m_iJpegQuality);
    iAddOption(g, wxT("RDP Image Encoding"), m_iRdpImageEncoding);
    iAddOption(g, wxT("RDP JPEG Quality"), m_iRdpJpegQuality);

    // Written by NX client, unknown purpose
    bAddOption(g, wxT("RDP optimization for low-bandwidth link"), false);

    // Written by NX client, unknown purpose
    sAddOption(g, wxT("Reduce colors to"), wxEmptyString);

    // deprecated but written by NX client, m_bImageEncodingPNG
    bAddOption(g, wxT("Use PNG Compression"), true);

    iAddOption(g, wxT("VNC images compression"), m_iVncImageEncoding);
    iAddOption(g, wxT("VNC JPEG Quality"), m_iVncJpegQuality);

    // deprecated - If this exists, we consider it an "old" config file during read
    // iAddOption(g, wxT("Windows Image Compression"), m_iRdpImageCompression);

    g = AddGroup(r, wxT("Login"));
    if (m_bRememberPassword) {
        if ((m_sPassword != DUMMY_MD5_PASSWORD) &&
                (m_sPassword != DUMMY_CLR_PASSWORD)   ) {
            optval = cryptString(m_sPassword);
            sAddOption(g, wxT("Auth"), optval);
        } else {
            if (m_pMd5Password)
                sAddOption(g, wxT("Auth"), *m_pMd5Password);
            if (m_pClrPassword)
                sAddOption(g, wxT("Password"), *m_pClrPassword);
        }
    } else {
        optval = wxEmptyString;
        sAddOption(g, wxT("Auth"), optval);
        sAddOption(g, wxT("Password"), optval);
    }
    bAddOption(g, wxT("Guest Mode"), m_bGuestMode);
    sAddOption(g, wxT("Guest password"), m_sGuestPassword);
    sAddOption(g, wxT("Guest username"), m_sGuestUser);
    sAddOption(g, wxT("Login Method"), wxT("nx"));
    sAddOption(g, wxT("Public Key"), m_sSshKey);
    sAddOption(g, wxT("User"), m_sUsername);

    g = AddGroup(r, wxT("Environment"));
    sAddOption(g, wxT("CUPSD path"), m_sCupsPath);

    g = AddGroup(r, wxT("Services"));
    bAddOption(g, wxT("Audio"), m_bEnableMultimedia);
    bAddOption(g, wxT("NativePA"), m_bEnableNativePA);
    switch (m_eRatePA) {
        case RATEPA_NORESAMPLE:
            optval = wxT("NoResample");
            break;
        case RATEPA_48000:
            optval = wxT("48000");
            break;
        case RATEPA_44100:
            optval = wxT("44100");
            break;
        case RATEPA_32000:
            optval = wxT("32000");
            break;
        case RATEPA_16000:
            optval = wxT("16000");
            break;
        case RATEPA_8000:
            optval = wxT("8000");
            break;
    }
    sAddOption(g, wxT("RatePA"), optval);
    bAddOption(g, wxT("MonoPA"), m_bEnableMonoPA);
    bAddOption(g, wxT("Shares"), m_bEnableSmbSharing);
    bAddOption(g, wxT("IPPPrinting"), m_bUseCups);
    iAddOption(g, wxT("IPPPort"), m_iCupsPort);
    iAddOption(g, wxT("SmbDefaultPort"), m_iSmbPort);

    if (m_aUsedShareGroups.GetCount()) {
        g = AddGroup(r, wxT("share chosen"));
        iAddOption(g, wxT("Share number"), m_iUsedShareGroups);
        wxString sDefaultPrinter = wxEmptyString; 
        for (i = 0; i < m_aUsedShareGroups.GetCount(); i++) {
            optval = wxString::Format(wxT("Share%d"), (int)i);
            sAddOption(g, optval, m_aUsedShareGroups[i]);
            switch (findShare(optval).m_eType) {
                case SharedResource::SHARE_UNKNOWN:
                case SharedResource::SHARE_SMB_DISK:
                    break;
                case SharedResource::SHARE_CUPS_PRINTER:
                case SharedResource::SHARE_SMB_PRINTER:
                    if (findShare(optval).m_bDefault)
                        sDefaultPrinter = optval;
            }
        }
        if (!sDefaultPrinter.IsEmpty())
            sAddOption(g, wxT("default printer"), sDefaultPrinter);
    }

    g = AddGroup(r, wxT("Advanced"));
    switch (m_eCacheMemory) {
        case CACHEMEM_0MB:
            optval = wxT("0");
            break;
        case CACHEMEM_1MB:
            optval = wxT("1");
            break;
        case CACHEMEM_2MB:
            optval = wxT("2");
            break;
        case CACHEMEM_4MB:
            optval = wxT("4");
            break;
        case CACHEMEM_8MB:
            optval = wxT("8");
            break;
        case CACHEMEM_16MB:
            optval = wxT("16");
            break;
        case CACHEMEM_32MB:
            optval = wxT("32");
            break;
        case CACHEMEM_64MB:
            optval = wxT("64");
            break;
        case CACHEMEM_128MB:
            optval = wxT("128");
            break;
    }
    sAddOption(g, wxT("Cache size"), optval);
    switch (m_eCacheDisk) {
        case CACHEDISK_0MB:
            optval = wxT("0");
            break;
        case CACHEDISK_4MB:
            optval = wxT("4");
            break;
        case CACHEDISK_8MB:
            optval = wxT("8");
            break;
        case CACHEDISK_16MB:
            optval = wxT("16");
            break;
        case CACHEDISK_32MB:
            optval = wxT("32");
            break;
        case CACHEDISK_64MB:
            optval = wxT("64");
            break;
        case CACHEDISK_128MB:
            optval = wxT("128");
            break;
        case CACHEDISK_256MB:
            optval = wxT("256");
            break;
        case CACHEDISK_512MB:
            optval = wxT("512");
            break;
    }
    sAddOption(g, wxT("Cache size on disk"), optval);
    bAddOption(g, wxT("Current keyboard"), !m_bKbdLayoutOther);
    sAddOption(g, wxT("Custom keyboard layout"), m_sKbdLayoutLanguage);
    bAddOption(g, wxT("Disable TCP no-delay"), m_bDisableTcpNoDelay);
    bAddOption(g, wxT("Disable ZLIB stream compression"), m_bDisableZlibCompression);
    bAddOption(g, wxT("Enable SSL encryption"), m_bEnableSSL);
    switch (m_iClipFilter) {
        case 0:
            optval = wxT("primary");
            break;
        case 1:
            optval = wxT("clipboard");
            break;
        default:
            optval = wxT("both");
            break;
    }
    sAddOption(g, wxT("Clipboard filter"), optval);
    bAddOption(g, wxT("Enable HTTP proxy"), m_bUseProxy);
    bAddOption(g, wxT("Enable external proxy"), m_bExternalProxy);
    bAddOption(g, wxT("Enable USBIP"), m_bEnableUSBIP);
    bAddOption(g, wxT("Enable response time optimisations"), false); // ???
    bAddOption(g, wxT("Grab keyboard"), m_bGrabKeyboard);
    bAddOption(g, wxT("Disable DirectDraw"), m_bDisableDirectDraw);
    bAddOption(g, wxT("Disable deferred updates"), m_bDisableDeferredUpdates);
    sAddOption(g, wxT("Proxy command"), m_sProxyCommand);
    sAddOption(g, wxT("HTTP proxy host"), m_sProxyHost);
    optval = m_bProxyPassRemember ? encodeString(m_sProxyPass) : wxT("");
    sAddOption(g, wxT("HTTP proxy password"), optval);
    iAddOption(g, wxT("HTTP proxy port"), m_iProxyPort);
    sAddOption(g, wxT("HTTP proxy username"), m_sProxyUser);
    bAddOption(g, wxT("Remember HTTP proxy password"), m_bProxyPassRemember);
    bAddOption(g, wxT("Restore cache"), true); // ???
    sAddOption(g, wxT("StreamCompression"), wxEmptyString); // ???

    g = AddGroup(r, wxT("Windows Session"));
    bAddOption(g, wxT("Remember"), m_bRdpRememberPassword);
    bAddOption(g, wxT("Rootless"), m_bRdpRootless);
    bAddOption(g, wxT("Run application"), m_bRdpRunApplication);
    iAddOption(g, wxT("Authentication"), m_iRdpAuthType);
    switch (m_iRdpColors) {
        default:
            optval = wxT("8");
            break;
        case 1:
            optval = wxT("15");
            break;
        case 2:
            optval = wxT("16");
            break;
        case 3:
            optval = wxT("24");
            break;
    }
    sAddOption(g, wxT("Color Depth"), optval);
    sAddOption(g, wxT("Domain"), m_sRdpDomain);
    bAddOption(g, wxT("Image Cache"), m_bRdpCache);
    sAddOption(g, wxT("Server"), m_sRdpHostName);
    sAddOption(g, wxT("User"), m_sRdpUsername);
    optval = m_bRdpRememberPassword ? encodeString(m_sRdpPassword) : wxT("");
    sAddOption(g, wxT("Password"), optval);
    sAddOption(g, wxT("Application"), m_sRdpApplication);

    g = AddGroup(r, wxT("VNC Session"));
    bAddOption(g, wxT("Rootless"), m_bVncRootless);
    bAddOption(g, wxT("Remember"), m_bVncRememberPassword);
    sAddOption(g, wxT("Server"), m_sVncHostName);
    optval = m_bVncRememberPassword ? encodeString(m_sVncPassword) : wxT("");
    sAddOption(g, wxT("Password"), optval);
    optval = wxString::Format(wxT(":%d"), (int)m_iVncDisplayNumber);
    sAddOption(g, wxT("Display"), optval);

    for (i = 0; i < m_aShareGroups.GetCount(); i++) {
        g = AddGroup(r, m_aShareGroups[i].m_sGroupName);
        sAddOption(g, wxT("Sharename"), m_aShareGroups[i].m_sShareName);
        sAddOption(g, wxT("Username"), m_aShareGroups[i].m_sUsername);
        optval = encodeString(m_aShareGroups[i].m_sPassword);
        sAddOption(g, wxT("Password"), optval);
        switch (m_aShareGroups[i].m_eType) {
            case SharedResource::SHARE_UNKNOWN:
                break;
            case SharedResource::SHARE_SMB_DISK:
                sAddOption(g, wxT("Alias"), m_aShareGroups[i].m_sAlias);
                sAddOption(g, wxT("Type"), wxT("disk"));
                break;
            case SharedResource::SHARE_SMB_PRINTER:
                bAddOption(g, wxT("Default"), m_aShareGroups[i].m_bDefault);
                sAddOption(g, wxT("Driver"), m_aShareGroups[i].m_sDriver);
                bAddOption(g, wxT("Public"), m_aShareGroups[i].m_bPublic);
                sAddOption(g, wxT("Type"), wxT("smbprinter"));
                break;
            case SharedResource::SHARE_CUPS_PRINTER:
                bAddOption(g, wxT("Default"), m_aShareGroups[i].m_bDefault);
                sAddOption(g, wxT("Driver"), m_aShareGroups[i].m_sDriver);
                bAddOption(g, wxT("Public"), m_aShareGroups[i].m_bPublic);
                sAddOption(g, wxT("Type"), wxT("cupsprinter"));
                break;
        }
    }
    for (i = 0; i < m_aUsbForwards.GetCount(); i++) {
        g = AddGroup(r, wxString::Format(wxT("UsbForward%d"), i));
        sAddOption(g, wxT("Vendor"), m_aUsbForwards[i].m_sVendor);
        sAddOption(g, wxT("Product"), m_aUsbForwards[i].m_sProduct);
        sAddOption(g, wxT("Serial"), m_aUsbForwards[i].m_sSerial);
        iAddOption(g, wxT("VendorID"), m_aUsbForwards[i].m_iVendorID);
        iAddOption(g, wxT("ProductID"), m_aUsbForwards[i].m_iProductID);
        iAddOption(g, wxT("Class"), m_aUsbForwards[i].m_iClass);
        switch (m_aUsbForwards[i].m_eMode) {
            case SharedUsbDevice::MODE_UNKNOWN:
                sAddOption(g, wxT("Mode"), wxT("unknown"));
                break;
            case SharedUsbDevice::MODE_LOCAL:
                sAddOption(g, wxT("Mode"), wxT("local"));
                break;
            case SharedUsbDevice::MODE_REMOTE:
                sAddOption(g, wxT("Mode"), wxT("remote"));
                break;
        }
    }

    // Must write to memory first, to get rid of the standard XML
    // doctype header
    wxMemoryOutputStream mos;
    cfg.Save(mos);
    off_t len = mos.TellO();
    if (len > 0) {
        char *data = new char[len];
        mos.CopyTo(data, len);
        char *secondline = (char *)memchr(data, '\n', len);
        if (secondline++) {
            // Replace 1st line with non-standard NXclient doctype
            len -= (secondline - data);
            myLogTrace(MYTRACETAG, wxT("Writing '%s'"), VMB(m_sFileName));
            wxFile f;
            if (!f.Create(m_sFileName, true, wxS_IRUSR|wxS_IWUSR)) {
                delete data;
                return false;
            }
            wxFileOutputStream fos(f);
            if (!fos.Ok()) {
                delete data;
                return false;
            }
            // NO wxT() here!!
            fos.Write("<!DOCTYPE NXClientSettings>\n", 28);
            if (fos.LastWrite() != 28) {
                delete data;
                return false;
            }
            // Write out rest of file
            fos.Write(secondline, len);
            if (fos.LastWrite() != (size_t)len) {
                delete data;
                return false;
            }
        } else {
            delete data;
            return false;
        }
        delete data;
    }
#ifdef __WXMAC__
    wxFileName fn(m_sFileName);
    fn.MacSetTypeAndCreator('TEXT', 'OPNX');
#endif
    return true;
}

void MyXmlConfig::saveState()
{
    if (saved)
        delete saved;
    saved = new MyXmlConfig();
    *saved = *this;
}

bool MyXmlConfig::checkChanged()
{
    if (!saved)
        return true;
    return (!(*this == *saved));
}

bool MyXmlConfig::getBool(wxXmlNode *opt, const wxString &key, bool defval)
{
    bool val = defval;

    if (opt->GetName() == wxT("option")) {
        if (opt->GetAttribute(wxT("key"), wxEmptyString) == key) {
            wxString tmp = opt->GetAttribute(wxT("value"), wxEmptyString);
            if (!tmp.IsEmpty())
                val = ((tmp == STR_TRUE) || (tmp == STR_ONE));
        }
    }
    return val;
}

long MyXmlConfig::getLong(wxXmlNode *opt, const wxString &key, long defval)
{
    long val = defval;

    if (opt->GetName() == wxT("option")) {
        if (opt->GetAttribute(wxT("key"), wxEmptyString) == key) {
            wxString tmp = opt->GetAttribute(wxT("value"), wxEmptyString);
            if (tmp.IsNumber())
                tmp.ToLong(&val, 0);
        }
    }
    return val;
}

bool MyXmlConfig::getLongBool(wxXmlNode *opt, const wxString &key, bool defval)
{
    long val = defval ? 1 : 0;

    if (opt->GetName() == wxT("option")) {
        if (opt->GetAttribute(wxT("key"), wxEmptyString) == key) {
            wxString tmp = opt->GetAttribute(wxT("value"), wxEmptyString);
            if (tmp.IsNumber())
                tmp.ToLong(&val, 0);
        }
    }
    return (val != 0);
}

wxString MyXmlConfig::getString(wxXmlNode *opt, const wxString &key, const wxString &defval)
{
    wxString tmp = defval;

    if (opt->GetName() == wxT("option")) {
        if (opt->GetAttribute(wxT("key"), wxEmptyString) == key)
            tmp = opt->GetAttribute(wxT("value"), defval);
    }
    return tmp;
}

wxString *MyXmlConfig::getStringNew(wxXmlNode *opt, const wxString &key, wxString *defval)
{
    wxString *val = defval;

    if (opt->GetName() == wxT("option")) {
        if (opt->GetAttribute(wxT("key"), wxEmptyString) == key) {
            if (opt->HasAttribute(wxT("value"))) {
                val = new wxString(opt->GetAttribute(wxT("value"), defval ? *defval : wxT("")));
                if (val && val->IsEmpty() && (!defval)) {
                    delete val;
                    val = NULL;
                }
                if (defval)
                    delete defval;
            }
        }
    }
    return val;
}

wxString MyXmlConfig::getPassword(wxXmlNode *opt, const wxString &key, const wxString &defval)
{
    wxString val = defval;
    wxString *enc = getStringNew(opt, key, NULL);
    if (enc && enc->Left(1) == wxT(":") && enc->Right(1) == wxT(":") && enc->Length() > 1) {
        int idx = 1;
        wxString newpw = wxEmptyString;
        enc->Remove(0, 1);
        while (enc->Length()) {
            int p = enc->Find(wxT(":"));
            if (p != -1) {
                long l;
                enc->Left(p).ToLong(&l);
                newpw += wxChar(l - idx++);
            } else
                break;
            enc->Remove(0, p+1);
        }
        val = newpw;
    }
    if (enc)
        delete enc;
    return val;
}
