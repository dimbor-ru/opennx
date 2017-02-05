// $Id: SessionProperties.cpp 705 2012-03-16 13:01:13Z felfert $
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
#pragma implementation "SessionProperties.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes
#include <wx/config.h>
#include <wx/imaglist.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/dir.h>
#include <wx/scopeguard.h>
#include <wx/cshelp.h>

#include "SessionProperties.h"
#include "ProxyPropertyDialog.h"
#include "UnixImageSettingsDialog.h"
#include "UsbFilterDetailsDialog.h"
#include "RdpImageSettingsDialog.h"
#include "VncImageSettingsDialog.h"
#include "X11PropertyDialog.h"
#include "XdmPropertyDialog.h"
#include "RdpPropertyDialog.h"
#include "VncPropertyDialog.h"
#include "AboutDialog.h"
#include "ShareProperties.h"
#include "MyXmlConfig.h"
#include "MyValidator.h"
#include "Icon.h"
#include "KeyDialog.h"
#include "ExtHtmlWindow.h"
#include "LibUSB.h"
#include "opennxApp.h"
#include "osdep.h"
#include "PulseAudio.h"

////@begin XPM images
////@end XPM images

#include "trace.h"
ENABLE_TRACE;

#ifdef __UNIX__
# ifdef __WXMAC__
#  define LD_LIBRARY_PATH wxT("DYLD_LIBRARY_PATH")
# else
#  define LD_LIBRARY_PATH wxT("LD_LIBRARY_PATH")
# endif
#endif

wxString wxGetLibCheckRes(const wxString& libname)
{
    wxString ret = wxT("");
#ifndef __WXMSW__
        wxString ldpath;
        bool libfound = false;
        if (::wxGetEnv(LD_LIBRARY_PATH, &ldpath)) {
            wxStringTokenizer t(ldpath, wxT(":"));
            while (t.HasMoreTokens()) {
                wxString abslib = t.GetNextToken() + wxFileName::GetPathSeparator() + libname;
                libfound = wxFileName::IsFileReadable(abslib);
                if (libfound)
                    break;
            }
            if (!libfound)
                ret.Append(wxString::Format(_T(" %s not found in [DY]LD_LIBRARY_PATH=%s."),
                    libname.c_str(),ldpath.c_str()));
        } else
            ret.Append(wxT(" Environment variable [DY]LD_LIBRARY_PATH is empty."));
#endif
        return ret;
}

class KbdLayout {
    public:
        wxString sLayoutName;
        wxString sIsoCode;
        unsigned long winCode; // win32 language code;
};

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(KbdLayoutTable);

class CacheCleaner : public wxDirTraverser
{
    public:
        CacheCleaner(const wxString &toplevel)
        {
            m_sTopLevel = toplevel;
        }

        ~CacheCleaner()
        {
            int n = m_aFiles.GetCount() - 1;
            while (n >= 0)
                ::wxRemoveFile(m_aFiles[n--]);
            n = m_aDirs.GetCount() - 1;
            while (n >= 0)
                ::wxRmdir(m_aDirs[n--]);
        }

        virtual wxDirTraverseResult OnFile(const wxString &name)
        {
            if (name.StartsWith(m_sTopLevel + wxT("/cache")))
                m_aFiles.Add(name);
            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString &name)
        {
            if (name.StartsWith(m_sTopLevel + wxT("/cache"))) {
                m_aDirs.Add(name);
                return wxDIR_CONTINUE;
            } else
                return wxDIR_IGNORE;
        }

    private:
        wxArrayString m_aDirs;
        wxArrayString m_aFiles;
        wxString m_sTopLevel;
};

/*!
 * SessionProperties type definition
 */

IMPLEMENT_CLASS( SessionProperties, wxDialog )

/*!
 * SessionProperties event table definition
 */

BEGIN_EVENT_TABLE( SessionProperties, wxDialog )
////@begin SessionProperties event table entries
    EVT_TEXT( XRCID("ID_TEXTCTRL_HOST"), SessionProperties::OnTextctrlHostUpdated )

    EVT_SPINCTRL( XRCID("ID_SPINCTRL_PORT"), SessionProperties::OnSpinctrlPortUpdated )
    EVT_TEXT( XRCID("ID_SPINCTRL_PORT"), SessionProperties::OnTextctrlPortUpdated )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_PWSAVE"), SessionProperties::OnCheckboxPwsaveClick )

    EVT_BUTTON( XRCID("ID_BUTTON_KEYMANAGE"), SessionProperties::OnButtonKeymanageClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_SMARTCARD"), SessionProperties::OnCheckboxSmartcardClick )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_DPROTO"), SessionProperties::OnComboboxDprotoSelected )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_DTYPE"), SessionProperties::OnComboboxDtypeSelected )

    EVT_BUTTON( XRCID("ID_BUTTON_DSETTINGS"), SessionProperties::OnButtonDsettingsClick )

    EVT_SLIDER( XRCID("ID_SLIDER_SPEED"), SessionProperties::OnSliderSpeedUpdated )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_DISPTYPE"), SessionProperties::OnComboboxDisptypeSelected )

    EVT_SPINCTRL( XRCID("ID_SPINCTRL_WIDTH"), SessionProperties::OnSpinctrlWidthUpdated )
    EVT_TEXT( XRCID("ID_SPINCTRL_WIDTH"), SessionProperties::OnSpinctrlWidthTextUpdated )

    EVT_SPINCTRL( XRCID("ID_SPINCTRL_HEIGHT"), SessionProperties::OnSpinctrlHeightUpdated )
    EVT_TEXT( XRCID("ID_SPINCTRL_HEIGHT"), SessionProperties::OnSpinctrlHeightTextUpdated )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_IMG_CUSTOM"), SessionProperties::OnCheckboxImgCustomClick )

    EVT_BUTTON( XRCID("ID_BUTTON_IMG_CUSTOM"), SessionProperties::OnButtonImgCustomClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_KBDOTHER"), SessionProperties::OnCheckboxKbdotherClick )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_KBDLAYOUT"), SessionProperties::OnComboboxKbdlayoutSelected )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_DISABLETCPNODEL"), SessionProperties::OnCheckboxDisabletcpnodelClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_DISABLEZCOMP"), SessionProperties::OnCheckboxDisablezcompClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_ENABLESSL"), SessionProperties::OnCheckboxEnablesslClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_PROXY"), SessionProperties::OnCheckboxProxyClick )

    EVT_BUTTON( XRCID("ID_BUTTON_PROXYSETTINGS"), SessionProperties::OnButtonProxysettingsClick )

#if defined(__WXMSW__)
    EVT_CHECKBOX( XRCID("ID_CHECKBOX_DISABLEDX"), SessionProperties::OnCheckboxDisabledxClick )
#endif

#if defined(__WXMSW__)
    EVT_CHECKBOX( XRCID("ID_CHECKBOX_GRABKB"), SessionProperties::OnCheckboxGrabkbClick )
#endif

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_NODEFERRED"), SessionProperties::OnCheckboxNodeferredClick )

#if defined(__WXMSW__)
    EVT_COMBOBOX( XRCID("ID_COMBOBOX_CLIPFILTER"), SessionProperties::OnComboboxClipfilterSelected )
#endif

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_CACHEMEM"), SessionProperties::OnComboboxCachememSelected )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_CACHEDISK"), SessionProperties::OnComboboxCachediskSelected )

    EVT_BUTTON( XRCID("ID_BUTTON_CACHECLEAN"), SessionProperties::OnButtonCachecleanClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_SMB"), SessionProperties::OnCheckboxSmbClick )

    EVT_SPINCTRL( XRCID("ID_SPINCTRL_SMBPORT"), SessionProperties::OnSpinctrlSmbportUpdated )
    EVT_TEXT( XRCID("ID_SPINCTRL_SMBPORT"), SessionProperties::OnSpinctrlSmbportTextUpdated )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_CUPSENABLE"), SessionProperties::OnCheckboxCupsenableClick )

    EVT_SPINCTRL( XRCID("ID_SPINCTRL_CUPSPORT"), SessionProperties::OnSpinctrlCupsportUpdated )
    EVT_TEXT( XRCID("ID_SPINCTRL_CUPSPORT"), SessionProperties::OnSpinctrlCupsportTextUpdated )

    EVT_LIST_ITEM_SELECTED( XRCID("ID_LISTCTRL_SMB_SHARES"), SessionProperties::OnListctrlSmbSharesSelected )
    EVT_LIST_ITEM_ACTIVATED( XRCID("ID_LISTCTRL_SMB_SHARES"), SessionProperties::OnListctrlSmbSharesItemActivated )

    EVT_BUTTON( XRCID("ID_BUTTON_SMB_ADD"), SessionProperties::OnButtonSmbAddClick )

    EVT_BUTTON( XRCID("ID_BUTTON_SMB_MODIFY"), SessionProperties::OnButtonSmbModifyClick )

    EVT_BUTTON( XRCID("ID_BUTTON_SMB_DELETE"), SessionProperties::OnButtonSmbDeleteClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_MMEDIA"), SessionProperties::OnCheckboxMmediaClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_NATIVEPA"), SessionProperties::OnCheckboxNativePAClick )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_RATEPA"), SessionProperties::OnComboboxRatePASelected )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_MONOPA"), SessionProperties::OnCheckboxMonoPAClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_USBENABLE"), SessionProperties::OnCHECKBOXUSBENABLEClick )

    EVT_LIST_ITEM_SELECTED( XRCID("ID_LISTCTRL_USBFILTER"), SessionProperties::OnListctrlUsbfilterSelected )
    EVT_LIST_ITEM_ACTIVATED( XRCID("ID_LISTCTRL_USBFILTER"), SessionProperties::OnListctrlUsbfilterItemActivated )

    EVT_BUTTON( XRCID("ID_BUTTON_USBADD"), SessionProperties::OnButtonUsbaddClick )

    EVT_BUTTON( XRCID("ID_BUTTON_USBMODIFY"), SessionProperties::OnButtonUsbmodifyClick )

    EVT_BUTTON( XRCID("ID_BUTTON_USBDELETE"), SessionProperties::OnButtonUsbdeleteClick )

    EVT_TEXT( XRCID("ID_TEXTCTRL_USERDIR"), SessionProperties::OnTextctrlUserdirUpdated )

    EVT_BUTTON( XRCID("ID_BUTTON_BROWSE_USERDIR"), SessionProperties::OnButtonBrowseUserdirClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_REMOVEOLDSF"), SessionProperties::OnCheckboxRemoveoldsfClick )

    EVT_TEXT( XRCID("ID_TEXTCTRL_SYSDIR"), SessionProperties::OnTextctrlSysdirUpdated )

    EVT_BUTTON( XRCID("ID_BUTTON_BROWSE_SYSDIR"), SessionProperties::OnButtonBrowseSysdirClick )

    EVT_TEXT( XRCID("ID_TEXTCTRL_CUPSPATH"), SessionProperties::OnTextctrlCupspathUpdated )

    EVT_BUTTON( XRCID("ID_BUTTON_BROWSE_CUPSPATH"), SessionProperties::OnButtonBrowseCupspathClick )

    EVT_TEXT( XRCID("ID_TEXTCTRL_USBIPD_SOCKET"), SessionProperties::OnTextctrlUsbipdSocketTextUpdated )

    EVT_BUTTON( XRCID("ID_BUTTON_BROWSE_USBIPD_SOCKET"), SessionProperties::OnButtonBrowseUsbipdSocketClick )

    EVT_SPINCTRL( XRCID("ID_SPINCTRL_USB_LOCALPORT"), SessionProperties::OnSpinctrlUsbLocalportUpdated )
    EVT_TEXT( XRCID("ID_SPINCTRL_USB_LOCALPORT"), SessionProperties::OnSpinctrlUsbLocalportTextUpdated )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_CREATEICON"), SessionProperties::OnCheckboxCreateiconClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_RESETMSGBOXES"), SessionProperties::OnCheckboxResetmsgboxesClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_LOWERCASE_LOGIN"), SessionProperties::OnCheckboxLowercaseLoginClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_CLEAR_PASSONABORT"), SessionProperties::OnCheckboxClearPassonabortClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_NOMAGICPIXEL"), SessionProperties::OnCheckboxNomagicpixelClick )

    EVT_BUTTON( wxID_DELETE, SessionProperties::OnDeleteClick )

    EVT_BUTTON( wxID_APPLY, SessionProperties::OnApplyClick )

////@end SessionProperties event table entries

    EVT_MENU(wxID_CONTEXT_HELP, SessionProperties::OnContextHelp)

END_EVENT_TABLE()

/*!
 * SessionProperties constructors
 */

SessionProperties::SessionProperties( )
    : m_bKeyTyped(false)
    , m_pCfg(NULL)
{
    readKbdLayouts();
}

SessionProperties::SessionProperties( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_bKeyTyped(false)
    , m_pCfg(NULL)
{
    readKbdLayouts();
    Create(parent, id, caption, pos, size, style);
}

    void
SessionProperties::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
    SaveState();
}

    void
SessionProperties::removePage(const wxString &title)
{
    for (size_t i = 0; i < m_pNoteBook->GetPageCount(); i++) {
        if (m_pNoteBook->GetPageText(i).IsSameAs(title)) {
            m_pNoteBook->DeletePage(i);
            break;
        }
    }
}

    void
SessionProperties::CheckCfgChanges(bool changed)
{
    if (m_pCfg)
        changed |= m_pCfg->checkChanged();
    m_pCtrlApplyButton->Enable(changed);
}

    void
SessionProperties::CheckChanged()
{
    wxASSERT_MSG(m_pCfg, _T("SessionProperties::CheckChanged: No configuration"));
    if (m_pCfg) {
        m_bKeyTyped = false;
        TransferDataFromWindow();

        // variables on 'General' Tab
        m_pCfg->bSetRememberPassword(m_bRememberPassword);
        m_pCfg->bSetUseSmartCard(m_bUseSmartCard);
        m_pCfg->bSetUseCustomImageEncoding(m_bUseCustomImageEncoding);
        m_pCfg->iSetServerPort(m_iPort);
        m_pCfg->eSetSessionType((MyXmlConfig::SessionType)m_iSessionType);
        switch (m_iSessionType) {
            case MyXmlConfig::STYPE_UNIX:
                m_iDesktopType = m_iDesktopTypeDialog;
                break;
            case MyXmlConfig::STYPE_WINDOWS:
                m_iDesktopType = MyXmlConfig::DTYPE_RDP;
                break;
            case MyXmlConfig::STYPE_VNC:
                m_iDesktopType = MyXmlConfig::DTYPE_RFB;
                break;
        }
        m_pCfg->eSetDesktopType((MyXmlConfig::DesktopType)m_iDesktopType);
        m_pCfg->eSetConnectionSpeed((MyXmlConfig::ConnectionSpeed)m_iConnectionSpeed);
        m_pCfg->eSetDisplayType((MyXmlConfig::DisplayType)m_iDisplayType);
        m_pCfg->iSetDisplayWidth(m_iDisplayWidth);
        m_pCfg->iSetDisplayHeight(m_iDisplayHeight);
        m_pCfg->sSetServerHost(m_sHostName);
        m_pCfg->bSetKbdLayoutOther(m_bKbdLayoutOther);
        m_pCfg->sSetKbdLayoutLanguage(m_sKbdLayoutLanguage);

        // variables on 'Advanced' tab
        m_pCfg->bSetDisableTcpNoDelay(m_bDisableTcpNoDelay);
        m_pCfg->bSetDisableZlibCompression(m_bDisableZlibCompression);
        m_pCfg->bSetEnableSSL(m_bEnableSSL);
        m_pCfg->eSetCacheMemory((MyXmlConfig::CacheMemory)m_iCacheMem);
        m_pCfg->eSetCacheDisk((MyXmlConfig::CacheDisk)m_iCacheDisk);

        m_pCfg->bSetUseProxy(m_bUseProxy);
        m_pCfg->bSetExternalProxy(m_bExternalProxy);
        m_pCfg->sSetProxyHost(m_sProxyHost);
        m_pCfg->sSetProxyPass(m_sProxyPass);
        m_pCfg->sSetProxyUser(m_sProxyUser);
        m_pCfg->bSetProxyPassRemember(m_bProxyPassRemember);
        m_pCfg->iSetProxyPort(m_iProxyPort);
        m_pCfg->sSetProxyCommand(m_sProxyCommand);
        m_pCfg->bSetGrabKeyboard(m_bGrabKeyboard);
        m_pCfg->bSetDisableDirectDraw(m_bDisableDirectDraw);
        m_pCfg->bSetDisableDeferredUpdates(m_bDisableDeferredUpdates);
        m_pCfg->iSetClipFilter(m_iClipFilter);

        // variables on 'Services' tab
        m_pCfg->bSetEnableSmbSharing(m_bEnableSmbSharing);
        m_pCfg->bSetEnableMultimedia(m_bEnableMultimedia);
        m_pCfg->bSetEnableNativePA(m_bEnableNativePA);
        m_pCfg->eSetRatePA((MyXmlConfig::RatePA)m_iRatePA);
//        switch (m_iRatePA) {
//            case MyXmlConfig::RATEPA_:
//                m_iRate = ;
//                break;
//        }
        m_pCfg->bSetEnableMonoPA(m_bEnableMonoPA);
        m_pCfg->bSetUseCups(m_bUseCups);
        m_pCfg->iSetCupsPort(m_iCupsPort);
        m_pCfg->iSetSmbPort(m_iSmbPort);

#ifdef SUPPORT_USBIP
        // variables on the 'USB' tab
        m_pCfg->bSetEnableUSBIP(m_bEnableUSBIP);
        m_pCfg->aSetUsbForwards(m_aUsbForwards);
#endif

        // variabless on 'Environment' tab
        m_pCfg->bSetRemoveOldSessionFiles(m_bRemoveOldSessionFiles);
        m_pCfg->sSetCupsPath(m_sCupsPath);

        bool changed = false;
        changed |= (m_sSavedUserNxDir != m_sUserNxDir);
        changed |= (m_sSavedSystemNxDir != m_sSystemNxDir);
        changed |= (m_sSavedUsbipdSocket != m_sUsbipdSocket);
        changed |= (m_iSavedUsbLocalPort != m_iUsbLocalPort);
        changed |= (m_bSavedCreateDesktopIcon != m_bCreateDesktopIcon);
        changed |= (m_bSavedResetMessageBoxes != m_bResetMessageBoxes);
        changed |= (m_bSavedLowercaseLogin != m_bLowercaseLogin);
        changed |= (m_bSavedClearPassOnAbort != m_bClearPassOnAbort);
        changed |= (m_bSavedDisableMagicPixel != m_bDisableMagicPixel);
        CheckCfgChanges(changed);
    }
}

void
SessionProperties::KeyTyped() {
    m_bKeyTyped = true;
}

/*!
 * SessionProperties creator
 */

bool SessionProperties::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
    ////@begin SessionProperties member initialisation
    m_bClearPassOnAbort = false;
    m_bCreateDesktopIcon = false;
    m_bDisableDeferredUpdates = false;
    m_bDisableDirectDraw = false;
    m_bDisableMagicPixel = false;
    m_bGrabKeyboard = false;
    m_bLowercaseLogin = false;
    m_bProxyPassRemember = false;
    m_bResetMessageBoxes = false;
    m_bSavedClearPassOnAbort = true;
    m_bSavedCreateDesktopIcon = false;
    m_bSavedDisableMagicPixel = false;
    m_bSavedLowercaseLogin = false;
    m_bSavedResetMessageBoxes = false;
    m_iClipFilter = 2;
    m_iPseudoDesktopTypeIndex = -1;
    m_iPseudoDisplayTypeIndex = -1;
    m_iSmbPort = 445;
    m_iUsbLocalPort = 3240;
    m_pNoteBook = NULL;
    m_pCtrlHostname = NULL;
    m_pCtrlPort = NULL;
    m_pCtrlUseSmartCard = NULL;
    m_pCtrlSessionType = NULL;
    m_pCtrlDesktopType = NULL;
    m_pCtrlDesktopSettings = NULL;
    m_pCtrlDisplayType = NULL;
    m_pCtrlDisplayWidth = NULL;
    m_pCtrlDisplayHeight = NULL;
    m_pCtrlImageEncCustom = NULL;
    m_pCtrlImageSettings = NULL;
    m_pCtrlKeyboardOther = NULL;
    m_pCtrlKeyboardLayout = NULL;
    m_pCtrlEnableSSL = NULL;
    m_pCtrlProxySettings = NULL;
    m_pCtrlSmbEnable = NULL;
    m_pCtrlSmbPort = NULL;
    m_pCtrlCupsEnable = NULL;
    m_pCtrlCupsPort = NULL;
    m_pCtrlSmbShares = NULL;
    m_pCtrlShareAdd = NULL;
    m_pCtrlShareModify = NULL;
    m_pCtrlShareDelete = NULL;
    m_pCtrlEnableMultimedia = NULL;
    m_pCtrlEnableNativePA = NULL;
    m_pCtrlRatePA = NULL;
    m_pCtrlEnableMonoPA = NULL;
    m_pCtrlUsbEnable = NULL;
    m_pCtrlUsbFilter = NULL;
    m_pCtrlUsbAdd = NULL;
    m_pCtrlUsbModify = NULL;
    m_pCtrlUsbDelete = NULL;
    m_pCtrlUserNxDir = NULL;
    m_pCtrlSystemNxDir = NULL;
    m_pCtrlCupsPath = NULL;
    m_pCtrlCupsBrowse = NULL;
    m_pCtrlUsbipdDaemon = NULL;
    m_pCtrlUsbIpdSocket = NULL;
    m_pCtrlUsbipdSocketBrowse = NULL;
    m_pCtrlUsbLocalPort = NULL;
    m_pCtrlResetMessageBoxes = NULL;
    m_pCtrlPanelAbout = NULL;
    m_pHtmlWindow = NULL;
    m_pCtrlApplyButton = NULL;
    ////@end SessionProperties member initialisation

    wxASSERT_MSG(m_pCfg, _T("SessionProperties::Create: No configuration"));
#ifdef HAVE_PULSE_PULSEAUDIO_H
#ifndef __WXMSW__
    PulseAudio pa;
#endif
#endif
    if (m_pCfg) {
        // variables on 'General' Tab
        m_bRememberPassword = m_pCfg->bGetRememberPassword();
        m_bUseSmartCard = ::wxGetApp().NxSmartCardSupport() && m_pCfg->bGetUseSmartCard();
        m_bUseCustomImageEncoding = m_pCfg->bGetUseCustomImageEncoding();
        m_iPort = m_pCfg->iGetServerPort();
        m_iSessionType = m_pCfg->eGetSessionType();
        m_iDesktopType = m_pCfg->eGetDesktopType();
        m_iDesktopTypeDialog = (m_iDesktopType > 0) ? m_iDesktopType : 0;
        m_iUnixDesktopType = m_iDesktopType;
        m_iConnectionSpeed = m_pCfg->eGetConnectionSpeed();
        m_iDisplayType = m_pCfg->eGetDisplayType();
        m_iDisplayWidth = m_pCfg->iGetDisplayWidth();
        m_iDisplayHeight = m_pCfg->iGetDisplayHeight();
        m_sHostName = m_pCfg->sGetServerHost();

        // variables on 'Advanced' tab
        m_bDisableTcpNoDelay = m_pCfg->bGetDisableTcpNoDelay();
        m_bDisableZlibCompression = m_pCfg->bGetDisableZlibCompression();
        m_bEnableSSL = m_pCfg->bGetEnableSSL();
        m_bUseProxy = m_pCfg->bGetUseProxy();
        m_bExternalProxy = m_pCfg->bGetExternalProxy();
        m_bKbdLayoutOther = m_pCfg->bGetKbdLayoutOther();
        m_iCacheMem = m_pCfg->eGetCacheMemory();
        m_iCacheDisk = m_pCfg->eGetCacheDisk();
        m_sKbdLayoutLanguage = m_pCfg->sGetKbdLayoutLanguage();
        m_iProxyPort = m_pCfg->iGetProxyPort();
        m_sProxyHost = m_pCfg->sGetProxyHost();
        m_sProxyUser = m_pCfg->sGetProxyUser();
        m_sProxyPass = m_pCfg->sGetProxyPass();
        m_bProxyPassRemember = m_pCfg->bGetProxyPassRemember();
        m_sProxyCommand = m_pCfg->sGetProxyCommand();
        m_iClipFilter = m_pCfg->iGetClipFilter();

        m_bGrabKeyboard = m_pCfg->bGetGrabKeyboard();
        m_bDisableDirectDraw = m_pCfg->bGetDisableDirectDraw();

        // variables on 'Services' tab
        m_bEnableSmbSharing = m_pCfg->bGetEnableSmbSharing();
#ifdef HAVE_PULSE_PULSEAUDIO_H
#ifndef __WXMSW__
        if (pa.IsAvailable()) {
#endif
#endif
            m_bEnableMultimedia = m_pCfg->bGetEnableMultimedia();
#ifdef HAVE_PULSE_PULSEAUDIO_H
#ifndef __WXMSW__
        } else {
            // disable MM, disable checkbox
            m_bEnableMultimedia = false;
            m_pCfg->bSetEnableMultimedia(false);
        }
#endif
#endif
        m_bEnableNativePA = m_pCfg->bGetEnableNativePA();
        m_iRatePA = m_pCfg->eGetRatePA();
        m_bEnableMonoPA = m_pCfg->bGetEnableMonoPA();
#ifdef __UNIX__
        m_bUseCups = m_pCfg->bGetUseCups();
#else
        m_bUseCups = false;
#endif
        m_iCupsPort = m_pCfg->iGetCupsPort();
        m_iSmbPort = m_pCfg->iGetSmbPort();

        // variables on 'USB' tab
#ifdef SUPPORT_USBIP
        m_bEnableUSBIP = m_pCfg->bGetEnableUSBIP();
        m_aUsbForwards = m_pCfg->aGetUsbForwards();
#else
        m_bEnableUSBIP = false;
        m_pCfg->bSetEnableUSBIP(false);
#endif

        // variables on 'Environment' tab
        m_bRemoveOldSessionFiles = m_pCfg->bGetRemoveOldSessionFiles();
        m_sCupsPath = m_pCfg->sGetCupsPath();
        m_bCreateDesktopIcon = m_bSavedCreateDesktopIcon =
            ::wxGetApp().CheckDesktopEntry(m_pCfg);
    }
    // Global config
    wxConfigBase::Get()->Read(wxT("Config/UserNxDir"), &m_sUserNxDir);
    wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &m_sSystemNxDir);
    wxConfigBase::Get()->Read(wxT("Config/LowercaseLogin"), &m_bLowercaseLogin, false);
    wxConfigBase::Get()->Read(wxT("Config/ClearPassOnAbort"), &m_bClearPassOnAbort, true);
    wxConfigBase::Get()->Read(wxT("Config/DisableMagicPixel"), &m_bDisableMagicPixel, false);
#ifdef SUPPORT_USBIP
    wxConfigBase::Get()->Read(wxT("Config/UsbipdSocket"), &m_sUsbipdSocket);
    wxConfigBase::Get()->Read(wxT("Config/UsbipPort"), &m_iUsbLocalPort);
#else
    m_sUsbipdSocket = wxEmptyString;
#endif
    // This setting can be used by an admin to disable storing passwords
    // It has to be set manually in the global config file.
    wxConfigBase::Get()->Read(wxT("Config/StorePasswords"), &m_bStorePasswords, true);
    if (!m_bStorePasswords)
        m_bRememberPassword = false;

    ////@begin SessionProperties creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY|wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end SessionProperties creation

#ifdef SUPPORT_USBIP
    m_pCtrlUsbipdDaemon->Show();
    wxSize sz = GetBestSize();
    sz.IncBy(0, 35);
    SetInitialSize(sz);
#else
    removePage(_("USB"));
    m_pCtrlUsbipdDaemon->Hide();
#endif

    // Populate keyboard layout combobox
    size_t idx1 = (size_t)-1;
    size_t idx2 = (size_t)-1;
    // Fix invalid keyboard lang
    wxString mykbdlang = wxString(wxConvLocal.cMB2WX(x11_keyboard_type)).AfterFirst(wxT('/')).BeforeFirst(wxT(','));
    size_t i;
    for (i = 0; i < m_aKbdLayoutTable.GetCount(); i++) {
        KbdLayout &l = m_aKbdLayoutTable.Item(i);
        m_pCtrlKeyboardLayout->Append(l.sLayoutName, (void *)i);
        if (l.sIsoCode == m_sKbdLayoutLanguage)
            idx1 = i;
        if (l.sIsoCode == mykbdlang)
            idx2 = i;
    }
    if (idx1 != (size_t)-1)
        m_pCtrlKeyboardLayout->SetValue(m_aKbdLayoutTable.Item(idx1).sLayoutName);
    else if (idx2 != (size_t)-1)
        m_pCtrlKeyboardLayout->SetValue(m_aKbdLayoutTable.Item(idx2).sLayoutName);

#define SHI_SIZE 16,16
    wxImageList *il = new wxImageList(SHI_SIZE);
    il->Add(CreateBitmapFromFile(wxT("res/shbroken.png"), SHI_SIZE));
    il->Add(CreateBitmapFromFile(wxT("res/smbfolder.png"), SHI_SIZE));
    il->Add(CreateBitmapFromFile(wxT("res/smbprinter.png"), SHI_SIZE));
    il->Add(CreateBitmapFromFile(wxT("res/cupsprinter.png"), SHI_SIZE));
#ifdef SUPPORT_USBIP
    wxImageList *il2 = new wxImageList(SHI_SIZE);
    il2->Add(CreateBitmapFromFile(wxT("res/usblocal.png"), SHI_SIZE));
    il2->Add(CreateBitmapFromFile(wxT("res/usbremote.png"), SHI_SIZE));
#endif
#undef SHI_SIZE

    m_pCtrlSmbShares->AssignImageList(il, wxIMAGE_LIST_SMALL);
    m_pCtrlSmbShares->InsertColumn(0, _("Share"));
    m_pCtrlSmbShares->InsertColumn(1, _("Mount on"));
    m_pCtrlSmbShares->InsertColumn(2, _("Comment"));
    if (m_pCfg && (m_pCfg->iGetUsedShareGroups() > 0)) {
        size_t i;
        SmbClient sc;
        CupsClient cc;
        ArrayOfShareGroups sg = m_pCfg->aGetShareGroups();
        ArrayOfShares sa  = sc.GetShares();
        WX_APPEND_ARRAY(sa, cc.GetShares());
        wxArrayString usg = m_pCfg->aGetUsedShareGroups();
        for (i = 0; i < sg.GetCount(); i++) {
            if (usg.Index(sg[i].m_sGroupName) != wxNOT_FOUND) {
                size_t si;
                wxString comment = _("Currently not available");
                int lidx = -1;
                for (si = 0; si < sa.GetCount(); si++) {
                    if ((sa[si].name == sg[i].m_sShareName) &&
                            (sa[si].sharetype == sg[i].m_eType)) {
                        comment = sa[si].description;
                        switch (sa[si].sharetype) {
                            case SharedResource::SHARE_UNKNOWN:
                                break;
                            case SharedResource::SHARE_SMB_DISK:
                                myLogTrace(MYTRACETAG, wxT("%s"), sg[i].toString().c_str());
                                lidx = m_pCtrlSmbShares->InsertItem(0, sg[i].m_sShareName, 1);
                                m_pCtrlSmbShares->SetItem(lidx, 1, sg[i].m_sAlias);
                                m_pCtrlSmbShares->SetItem(lidx, 2, comment);
                                break;
                            case SharedResource::SHARE_SMB_PRINTER:
                                myLogTrace(MYTRACETAG, wxT("%s"), sg[i].toString().c_str());
                                lidx = m_pCtrlSmbShares->InsertItem(0, sg[i].m_sShareName, 2);
                                m_pCtrlSmbShares->SetItem(lidx, 1, sg[i].m_sDriver);
                                m_pCtrlSmbShares->SetItem(lidx, 2, comment);
                                break;
                            case SharedResource::SHARE_CUPS_PRINTER:
                                myLogTrace(MYTRACETAG, wxT("%s"), sg[i].toString().c_str());
                                lidx = m_pCtrlSmbShares->InsertItem(0, sg[i].m_sShareName, 3);
                                m_pCtrlSmbShares->SetItem(lidx, 1, sg[i].m_sDriver);
                                m_pCtrlSmbShares->SetItem(lidx, 2, comment);
                                break;
                        }
                        break;
                    }
                }
                if (0 > lidx) {
                    myLogTrace(MYTRACETAG, wxT("Broken '%s'"), sg[i].toString().c_str());
                    lidx = m_pCtrlSmbShares->InsertItem(0, sg[i].m_sShareName, 0);
                    m_pCtrlSmbShares->SetItem(lidx, 1, sg[i].m_sAlias);
                    m_pCtrlSmbShares->SetItem(lidx, 2, comment);
                }
                m_pCtrlSmbShares->SetItemData(lidx, i);
            }
        }
    }
    updateListCtrlColumnWidth(m_pCtrlSmbShares);

#ifdef SUPPORT_USBIP
    m_pCtrlUsbFilter->AssignImageList(il2, wxIMAGE_LIST_SMALL);
    m_pCtrlUsbFilter->InsertColumn(0, _("VendorID"));
    m_pCtrlUsbFilter->InsertColumn(1, _("ProductID"));
    m_pCtrlUsbFilter->InsertColumn(2, _("Class"));
    m_pCtrlUsbFilter->InsertColumn(3, _("Name"));
    m_pCtrlUsbFilter->InsertColumn(4, _("Serial"));
    for (i = 0; i < m_aUsbForwards.GetCount(); i++)
        appendUsbDevice(m_aUsbForwards[i], i);
    updateListCtrlColumnWidth(m_pCtrlUsbFilter);
#endif

#ifdef HAVE_PULSE_PULSEAUDIO_H
#ifndef __WXMSW__
    m_pCtrlEnableMultimedia->Enable(pa.IsAvailable());
#endif
#endif

    UpdateDialogConstraints(false);

    // Fix broken fonts in Spin-Controls
    m_pCtrlDisplayWidth->SetFont(wxSystemSettings::GetFont(wxSYS_ANSI_VAR_FONT));
    m_pCtrlDisplayHeight->SetFont(wxSystemSettings::GetFont(wxSYS_ANSI_VAR_FONT));

    SaveState();
    m_pCtrlApplyButton->Enable(false);

    // Hook into OnChar events of wxTextCtrl's and wxSpinCtrl's
    InstallOnCharHandlers();

#ifndef __UNIX__
    // disable CUPS-related stuff on non-Unix platforms
    m_pCtrlCupsEnable->Enable(false);
    m_pCtrlCupsPort->Enable(false);
    m_pCtrlCupsPath->Enable(false);
    m_pCtrlCupsBrowse->Enable(false);
#endif

    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

void SessionProperties::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

void SessionProperties::updateListCtrlColumnWidth(wxListCtrl *ctrl)
{
    int cc = ctrl->GetColumnCount();
    int i;
    if (0 == cc) {
        for (i = 0; i < cc; i++)
            ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
        return;
    }
    int *w = new int[cc];
    // int w = (ctrl->GetItemCount() > 0) ? wxLIST_AUTOSIZE : wxLIST_AUTOSIZE_USEHEADER;
    for (i = 0; i < cc; i++)
        ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE);
    for (i = 0; i < cc; i++)
        w[i] = ctrl->GetColumnWidth(i);
    for (i = 0; i < cc; i++)
        ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
    for (i = 0; i < cc; i++) {
        if (w[i] > ctrl->GetColumnWidth(i))
            ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE);
    }
    delete []w;
}

#ifdef SUPPORT_USBIP
void SessionProperties::appendUsbDevice(SharedUsbDevice &dev, int aidx)
{
    long idx;
    wxString lbl = (-1 == dev.m_iVendorID) ? wxT("*") : wxString::Format(wxT("%04X"), (int)dev.m_iVendorID);
    idx = m_pCtrlUsbFilter->InsertItem(m_pCtrlUsbFilter->GetItemCount(),
            lbl, (dev.m_eMode == SharedUsbDevice::MODE_REMOTE) ? 1 : 0);
    lbl = (-1 == dev.m_iProductID) ? wxT("*") : wxString::Format(wxT("%04X"), (int)dev.m_iProductID);
    m_pCtrlUsbFilter->SetItem(idx, 1, lbl);
    lbl = (-1 == dev.m_iClass) ? wxT("*") : wxString::Format(wxT("%02X"), (int)dev.m_iClass);
    m_pCtrlUsbFilter->SetItem(idx, 2, lbl);
    lbl = dev.toShortString().IsEmpty() ? wxT("*") : dev.toShortString();
    m_pCtrlUsbFilter->SetItem(idx, 3, lbl);
    lbl = dev.m_sSerial.IsEmpty() ? wxT("*") : dev.m_sSerial;
    m_pCtrlUsbFilter->SetItem(idx, 4, lbl);
    m_pCtrlUsbFilter->SetItemData(idx, aidx);
}
#endif

/**
 * Installs event handler for OnChar event in all wxTextCtrl and wxSpinCtrl
 * childs.
 */
    void
SessionProperties::InstallOnCharHandlers(wxWindow *w /* = NULL*/)
{
    if (!w)
        w = this;
    wxWindowList& children = w->GetChildren();
    wxWindowList::Node *node;
    for (node = children.GetFirst(); node; node = node->GetNext()) {
        w = node->GetData();
        if (w->IsKindOf(CLASSINFO(wxTextCtrl)) || w->IsKindOf(CLASSINFO(wxSpinCtrl))) {
            wxValidator *v = w->GetValidator();
            if (v) {
                if (v->IsKindOf(CLASSINFO(MyValidator)))
                    wxDynamicCast(v, MyValidator)->SetKeyTyped(this);
                else
                    wxLogError(wxT("Detected %s window with validator other than MyValidator!"),
                            w->IsKindOf(CLASSINFO(wxTextCtrl)) ? wxT("wxTextCtrl") : wxT("wxSpinCtrl"));
            } else {
#ifdef __WXMAC__
                // wxWidgets on MacOSX generates additional windows
                if (w->GetName().IsEmpty() || w->GetName().IsSameAs(wxT("text")))
                    continue;
#endif
                wxLogError(wxT("Detected %s (name=%s) window without validator!"),
                        (w->IsKindOf(CLASSINFO(wxTextCtrl)) ? wxT("wxTextCtrl") : wxT("wxSpinCtrl")),
                        (w->GetName().IsEmpty() ? wxEmptyString : w->GetName().c_str()));
            }
        } else {
            if (!w->GetChildren().IsEmpty())
                InstallOnCharHandlers(w);
        }
    }
} 

/**
 * Handle dialog constraints (i.e.: Enable/Disable various controls depending
 * on other controls.
 */ 
void SessionProperties::UpdateDialogConstraints(bool getValues)
{
    if (getValues)
        TransferDataFromWindow();

    // 'General' tab
    switch (m_iSessionType) {
        case MyXmlConfig::STYPE_UNIX:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
                m_iDesktopTypeDialog = 0;
            }
            m_pCtrlDesktopType->SetSelection(m_iDesktopTypeDialog);
            m_pCtrlDesktopType->Enable(true);
            m_pCtrlDesktopSettings->Enable(
                    (m_iDesktopTypeDialog == MyXmlConfig::DTYPE_XDM) ||
                    (m_iDesktopTypeDialog == MyXmlConfig::DTYPE_CUSTOM));
            if (m_iPseudoDisplayTypeIndex != -1) {
                m_pCtrlDisplayType->Delete(m_iPseudoDisplayTypeIndex);
                if (m_iDisplayType >= m_iPseudoDisplayTypeIndex)
                    m_pCtrlDisplayType->SetSelection(3);
                m_iPseudoDisplayTypeIndex = -1;
            }
            m_pCtrlCupsEnable->Enable(true);
            m_pCtrlSmbEnable->Enable(true);
#ifdef SUPPORT_USBIP
            m_pCtrlUsbEnable->Enable(true);
#endif
            break;
        case MyXmlConfig::STYPE_WINDOWS:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
            }
            m_iPseudoDesktopTypeIndex = m_pCtrlDesktopType->Append(_("RDP"), (void *)MyXmlConfig::DTYPE_RDP);
            m_pCtrlDesktopType->SetSelection(m_iPseudoDesktopTypeIndex);
            m_iDesktopType = MyXmlConfig::DTYPE_RDP;
            m_iDesktopTypeDialog = getValues ? 0 : m_iPseudoDesktopTypeIndex;
            m_pCtrlDesktopType->Enable(false);
            m_pCtrlDesktopSettings->Enable(true);
            if (m_iPseudoDisplayTypeIndex != -1) {
                m_pCtrlDisplayType->Delete(m_iPseudoDisplayTypeIndex);
                if (m_iDisplayType >= m_iPseudoDisplayTypeIndex)
                    m_pCtrlDisplayType->SetSelection(3);
                m_iPseudoDisplayTypeIndex = -1;
            }
            m_pCtrlCupsEnable->Enable(false);
            m_pCtrlSmbEnable->Enable(false);
            m_bUseCups = false;
            m_bEnableSmbSharing = false;
            m_bEnableUSBIP = false;
#ifdef SUPPORT_USBIP
            m_pCtrlUsbEnable->Enable(false);
#endif
            break;
        case MyXmlConfig::STYPE_VNC:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
            }
            m_iPseudoDesktopTypeIndex = m_pCtrlDesktopType->Append(_("RFB"), (void *)MyXmlConfig::DTYPE_RFB);
            m_pCtrlDesktopType->SetSelection(m_iPseudoDesktopTypeIndex);
            m_iDesktopType = MyXmlConfig::DTYPE_RFB;
            m_pCtrlDesktopType->Enable(true);
            m_iDesktopTypeDialog = getValues ? 0 : m_iPseudoDesktopTypeIndex;
            m_pCtrlDesktopType->Enable(false);
            m_pCtrlDesktopSettings->Enable(true);
            if (m_iPseudoDisplayTypeIndex != -1) {
                m_pCtrlDisplayType->Delete(m_iPseudoDisplayTypeIndex);
                if (m_iDisplayType >= m_iPseudoDisplayTypeIndex)
                    m_pCtrlDisplayType->SetSelection(3);
                m_iPseudoDisplayTypeIndex = -1;
            }
            m_pCtrlCupsEnable->Enable(false);
            m_pCtrlSmbEnable->Enable(false);
            m_bUseCups = false;
            m_bEnableSmbSharing = false;
            m_bEnableUSBIP = false;
#ifdef SUPPORT_USBIP
            m_pCtrlUsbEnable->Enable(false);
#endif
            break;
        case MyXmlConfig::STYPE_SHADOW:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
            }
            m_iPseudoDesktopTypeIndex = m_pCtrlDesktopType->Append(_("Any"), (void *)MyXmlConfig::DTYPE_ANY);
            m_pCtrlDesktopType->SetSelection(m_iPseudoDesktopTypeIndex);
            m_iDesktopType = MyXmlConfig::DTYPE_ANY;
            m_iDesktopTypeDialog = getValues ? 0 : m_iPseudoDesktopTypeIndex;
            m_pCtrlDesktopType->Enable(false);
            m_pCtrlDesktopSettings->Enable(false);
            if (m_iPseudoDisplayTypeIndex == -1) {
                m_iPseudoDisplayTypeIndex = m_pCtrlDisplayType->Append(_("As on server"), (void *)MyXmlConfig::DPTYPE_REMOTE);
                m_pCtrlDisplayType->SetSelection(m_iPseudoDisplayTypeIndex);
            }
            m_pCtrlCupsEnable->Enable(false);
            m_pCtrlSmbEnable->Enable(false);
            m_bUseCups = false;
            m_bEnableSmbSharing = false;
            m_bEnableUSBIP = false;
#ifdef SUPPORT_USBIP
            m_pCtrlUsbEnable->Enable(false);
#endif
            break;
    }
    switch (m_iDisplayType) {
        case MyXmlConfig::DPTYPE_CUSTOM:
            m_pCtrlDisplayWidth->Enable(true);
            m_pCtrlDisplayHeight->Enable(true);
            break;
        default:
            m_pCtrlDisplayWidth->Enable(false);
            m_pCtrlDisplayHeight->Enable(false);
            break;
    }
    if (m_bUseSmartCard) {
        m_pCtrlEnableSSL->SetValue(true);
        m_pCtrlEnableSSL->Enable(false);
        m_bEnableSSL = true;
    } else
        m_pCtrlEnableSSL->Enable(true);

    m_pCtrlImageSettings->Enable(m_bUseCustomImageEncoding);

    // 'Services' tab
    bool bTmp = m_bEnableSmbSharing || m_bUseCups;
    m_pCtrlSmbShares->Enable(bTmp);
    m_pCtrlShareAdd->Enable(bTmp);
    m_pCtrlShareDelete->Enable(bTmp && (m_pCtrlSmbShares->GetSelectedItemCount() > 0));
    m_pCtrlShareModify->Enable(bTmp && (m_pCtrlSmbShares->GetSelectedItemCount() > 0));
    m_pCtrlSmbPort->Enable(m_bEnableSmbSharing);
    m_pCtrlCupsPort->Enable(m_bUseCups);

    m_pCtrlEnableNativePA->Enable(m_bEnableMultimedia);
    m_pCtrlRatePA->Enable(m_bEnableMultimedia && m_bEnableNativePA);
    m_pCtrlEnableMonoPA->Enable(m_bEnableMultimedia && m_bEnableNativePA
                                && (MyXmlConfig::RATEPA_NORESAMPLE != m_iRatePA));

#ifdef SUPPORT_USBIP
    // 'USB' tab
    m_pCtrlUsbFilter->Enable(m_bEnableUSBIP);
    m_pCtrlUsbAdd->Enable(m_bEnableUSBIP);
    m_pCtrlUsbDelete->Enable(m_bEnableUSBIP && (m_pCtrlUsbFilter->GetSelectedItemCount() > 0));
    m_pCtrlUsbModify->Enable(m_bEnableUSBIP && (m_pCtrlUsbFilter->GetSelectedItemCount() > 0));
#endif

    // 'Advanced' tab
    m_pCtrlKeyboardLayout->Enable(m_bKbdLayoutOther);
    m_pCtrlProxySettings->Enable(m_bUseProxy);

    // 'Environment' tab
}

/*!
 * Control creation for SessionProperties
 */

void SessionProperties::CreateControls()
{
    ////@begin SessionProperties content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_PROPERTIES")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pNoteBook = XRCCTRL(*this, "ID_NOTEBOOK", wxNotebook);
    m_pCtrlHostname = XRCCTRL(*this, "ID_TEXTCTRL_HOST", wxTextCtrl);
    m_pCtrlPort = XRCCTRL(*this, "ID_SPINCTRL_PORT", wxSpinCtrl);
    m_pCtrlUseSmartCard = XRCCTRL(*this, "ID_CHECKBOX_SMARTCARD", wxCheckBox);
    m_pCtrlSessionType = XRCCTRL(*this, "ID_COMBOBOX_DPROTO", wxComboBox);
    m_pCtrlDesktopType = XRCCTRL(*this, "ID_COMBOBOX_DTYPE", wxComboBox);
    m_pCtrlDesktopSettings = XRCCTRL(*this, "ID_BUTTON_DSETTINGS", wxButton);
    m_pCtrlDisplayType = XRCCTRL(*this, "ID_COMBOBOX_DISPTYPE", wxComboBox);
    m_pCtrlDisplayWidth = XRCCTRL(*this, "ID_SPINCTRL_WIDTH", wxSpinCtrl);
    m_pCtrlDisplayHeight = XRCCTRL(*this, "ID_SPINCTRL_HEIGHT", wxSpinCtrl);
    m_pCtrlImageEncCustom = XRCCTRL(*this, "ID_CHECKBOX_IMG_CUSTOM", wxCheckBox);
    m_pCtrlImageSettings = XRCCTRL(*this, "ID_BUTTON_IMG_CUSTOM", wxButton);
    m_pCtrlKeyboardOther = XRCCTRL(*this, "ID_CHECKBOX_KBDOTHER", wxCheckBox);
    m_pCtrlKeyboardLayout = XRCCTRL(*this, "ID_COMBOBOX_KBDLAYOUT", wxComboBox);
    m_pCtrlEnableSSL = XRCCTRL(*this, "ID_CHECKBOX_ENABLESSL", wxCheckBox);
    m_pCtrlProxySettings = XRCCTRL(*this, "ID_BUTTON_PROXYSETTINGS", wxButton);
    m_pCtrlSmbEnable = XRCCTRL(*this, "ID_CHECKBOX_SMB", wxCheckBox);
    m_pCtrlSmbPort = XRCCTRL(*this, "ID_SPINCTRL_SMBPORT", wxSpinCtrl);
    m_pCtrlCupsEnable = XRCCTRL(*this, "ID_CHECKBOX_CUPSENABLE", wxCheckBox);
    m_pCtrlCupsPort = XRCCTRL(*this, "ID_SPINCTRL_CUPSPORT", wxSpinCtrl);
    m_pCtrlSmbShares = XRCCTRL(*this, "ID_LISTCTRL_SMB_SHARES", wxListCtrl);
    m_pCtrlShareAdd = XRCCTRL(*this, "ID_BUTTON_SMB_ADD", wxButton);
    m_pCtrlShareModify = XRCCTRL(*this, "ID_BUTTON_SMB_MODIFY", wxButton);
    m_pCtrlShareDelete = XRCCTRL(*this, "ID_BUTTON_SMB_DELETE", wxButton);
    m_pCtrlEnableMultimedia = XRCCTRL(*this, "ID_CHECKBOX_MMEDIA", wxCheckBox);
    m_pCtrlEnableNativePA = XRCCTRL(*this, "ID_CHECKBOX_NATIVEPA", wxCheckBox);
    m_pCtrlRatePA = XRCCTRL(*this, "ID_COMBOBOX_RATEPA", wxComboBox);
    m_pCtrlEnableMonoPA = XRCCTRL(*this, "ID_CHECKBOX_MONOPA", wxCheckBox);
    m_pCtrlUsbEnable = XRCCTRL(*this, "ID_CHECKBOX_USBENABLE", wxCheckBox);
    m_pCtrlUsbFilter = XRCCTRL(*this, "ID_LISTCTRL_USBFILTER", wxListCtrl);
    m_pCtrlUsbAdd = XRCCTRL(*this, "ID_BUTTON_USBADD", wxButton);
    m_pCtrlUsbModify = XRCCTRL(*this, "ID_BUTTON_USBMODIFY", wxButton);
    m_pCtrlUsbDelete = XRCCTRL(*this, "ID_BUTTON_USBDELETE", wxButton);
    m_pCtrlUserNxDir = XRCCTRL(*this, "ID_TEXTCTRL_USERDIR", wxTextCtrl);
    m_pCtrlSystemNxDir = XRCCTRL(*this, "ID_TEXTCTRL_SYSDIR", wxTextCtrl);
    m_pCtrlCupsPath = XRCCTRL(*this, "ID_TEXTCTRL_CUPSPATH", wxTextCtrl);
    m_pCtrlCupsBrowse = XRCCTRL(*this, "ID_BUTTON_BROWSE_CUPSPATH", wxButton);
    m_pCtrlUsbipdDaemon = XRCCTRL(*this, "ID_PANEL_USBIP_DAEMON", wxPanel);
    m_pCtrlUsbIpdSocket = XRCCTRL(*this, "ID_TEXTCTRL_USBIPD_SOCKET", wxTextCtrl);
    m_pCtrlUsbipdSocketBrowse = XRCCTRL(*this, "ID_BUTTON_BROWSE_USBIPD_SOCKET", wxButton);
    m_pCtrlUsbLocalPort = XRCCTRL(*this, "ID_SPINCTRL_USB_LOCALPORT", wxSpinCtrl);
    m_pCtrlResetMessageBoxes = XRCCTRL(*this, "ID_CHECKBOX_RESETMSGBOXES", wxCheckBox);
    m_pCtrlPanelAbout = XRCCTRL(*this, "ID_PANEL_ABOUT", wxPanel);
    m_pHtmlWindow = XRCCTRL(*this, "ID_HTMLWINDOW_ABOUT", extHtmlWindow);
    m_pCtrlApplyButton = XRCCTRL(*this, "wxID_APPLY", wxButton);
    // Set validators
    if (FindWindow(XRCID("ID_TEXTCTRL_HOST")))
        FindWindow(XRCID("ID_TEXTCTRL_HOST"))->SetValidator( MyValidator(MyValidator::MYVAL_HOST, & m_sHostName) );
    if (FindWindow(XRCID("ID_SPINCTRL_PORT")))
        FindWindow(XRCID("ID_SPINCTRL_PORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iPort) );
    if (FindWindow(XRCID("ID_CHECKBOX_PWSAVE")))
        FindWindow(XRCID("ID_CHECKBOX_PWSAVE"))->SetValidator( wxGenericValidator(& m_bRememberPassword) );
    if (FindWindow(XRCID("ID_CHECKBOX_SMARTCARD")))
        FindWindow(XRCID("ID_CHECKBOX_SMARTCARD"))->SetValidator( wxGenericValidator(& m_bUseSmartCard) );
    if (FindWindow(XRCID("ID_COMBOBOX_DPROTO")))
        FindWindow(XRCID("ID_COMBOBOX_DPROTO"))->SetValidator( wxGenericValidator(& m_iSessionType) );
    if (FindWindow(XRCID("ID_COMBOBOX_DTYPE")))
        FindWindow(XRCID("ID_COMBOBOX_DTYPE"))->SetValidator( wxGenericValidator(& m_iDesktopTypeDialog) );
    if (FindWindow(XRCID("ID_SLIDER_SPEED")))
        FindWindow(XRCID("ID_SLIDER_SPEED"))->SetValidator( wxGenericValidator(& m_iConnectionSpeed) );
    if (FindWindow(XRCID("ID_COMBOBOX_DISPTYPE")))
        FindWindow(XRCID("ID_COMBOBOX_DISPTYPE"))->SetValidator( wxGenericValidator(& m_iDisplayType) );
    if (FindWindow(XRCID("ID_SPINCTRL_WIDTH")))
        FindWindow(XRCID("ID_SPINCTRL_WIDTH"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iDisplayWidth) );
    if (FindWindow(XRCID("ID_SPINCTRL_HEIGHT")))
        FindWindow(XRCID("ID_SPINCTRL_HEIGHT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iDisplayHeight) );
    if (FindWindow(XRCID("ID_CHECKBOX_IMG_CUSTOM")))
        FindWindow(XRCID("ID_CHECKBOX_IMG_CUSTOM"))->SetValidator( wxGenericValidator(& m_bUseCustomImageEncoding) );
    if (FindWindow(XRCID("ID_CHECKBOX_KBDOTHER")))
        FindWindow(XRCID("ID_CHECKBOX_KBDOTHER"))->SetValidator( wxGenericValidator(& m_bKbdLayoutOther) );
    if (FindWindow(XRCID("ID_CHECKBOX_DISABLETCPNODEL")))
        FindWindow(XRCID("ID_CHECKBOX_DISABLETCPNODEL"))->SetValidator( wxGenericValidator(& m_bDisableTcpNoDelay) );
    if (FindWindow(XRCID("ID_CHECKBOX_DISABLEZCOMP")))
        FindWindow(XRCID("ID_CHECKBOX_DISABLEZCOMP"))->SetValidator( wxGenericValidator(& m_bDisableZlibCompression) );
    if (FindWindow(XRCID("ID_CHECKBOX_ENABLESSL")))
        FindWindow(XRCID("ID_CHECKBOX_ENABLESSL"))->SetValidator( wxGenericValidator(& m_bEnableSSL) );
    if (FindWindow(XRCID("ID_CHECKBOX_PROXY")))
        FindWindow(XRCID("ID_CHECKBOX_PROXY"))->SetValidator( wxGenericValidator(& m_bUseProxy) );
#if defined(__WXMSW__)
    if (FindWindow(XRCID("ID_CHECKBOX_DISABLEDX")))
        FindWindow(XRCID("ID_CHECKBOX_DISABLEDX"))->SetValidator( wxGenericValidator(& m_bDisableDirectDraw) );
#endif
#if defined(__WXMSW__)
    if (FindWindow(XRCID("ID_CHECKBOX_GRABKB")))
        FindWindow(XRCID("ID_CHECKBOX_GRABKB"))->SetValidator( wxGenericValidator(& m_bGrabKeyboard) );
#endif
    if (FindWindow(XRCID("ID_CHECKBOX_NODEFERRED")))
        FindWindow(XRCID("ID_CHECKBOX_NODEFERRED"))->SetValidator( wxGenericValidator(& m_bDisableDeferredUpdates) );
#if defined(__WXMSW__)
    if (FindWindow(XRCID("ID_COMBOBOX_CLIPFILTER")))
        FindWindow(XRCID("ID_COMBOBOX_CLIPFILTER"))->SetValidator( wxGenericValidator(& m_iClipFilter) );
#endif
    if (FindWindow(XRCID("ID_COMBOBOX_CACHEMEM")))
        FindWindow(XRCID("ID_COMBOBOX_CACHEMEM"))->SetValidator( wxGenericValidator(& m_iCacheMem) );
    if (FindWindow(XRCID("ID_COMBOBOX_CACHEDISK")))
        FindWindow(XRCID("ID_COMBOBOX_CACHEDISK"))->SetValidator( wxGenericValidator(& m_iCacheDisk) );
    if (FindWindow(XRCID("ID_CHECKBOX_SMB")))
        FindWindow(XRCID("ID_CHECKBOX_SMB"))->SetValidator( wxGenericValidator(& m_bEnableSmbSharing) );
    if (FindWindow(XRCID("ID_SPINCTRL_SMBPORT")))
        FindWindow(XRCID("ID_SPINCTRL_SMBPORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iSmbPort) );
    if (FindWindow(XRCID("ID_CHECKBOX_CUPSENABLE")))
        FindWindow(XRCID("ID_CHECKBOX_CUPSENABLE"))->SetValidator( wxGenericValidator(& m_bUseCups) );
    if (FindWindow(XRCID("ID_SPINCTRL_CUPSPORT")))
        FindWindow(XRCID("ID_SPINCTRL_CUPSPORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iCupsPort) );
    if (FindWindow(XRCID("ID_CHECKBOX_MMEDIA")))
        FindWindow(XRCID("ID_CHECKBOX_MMEDIA"))->SetValidator( wxGenericValidator(& m_bEnableMultimedia) );
    if (FindWindow(XRCID("ID_CHECKBOX_NATIVEPA")))
        FindWindow(XRCID("ID_CHECKBOX_NATIVEPA"))->SetValidator( wxGenericValidator(& m_bEnableNativePA) );
    if (FindWindow(XRCID("ID_COMBOBOX_RATEPA")))
        FindWindow(XRCID("ID_COMBOBOX_RATEPA"))->SetValidator( wxGenericValidator(& m_iRatePA) );
    if (FindWindow(XRCID("ID_CHECKBOX_MONOPA")))
        FindWindow(XRCID("ID_CHECKBOX_MONOPA"))->SetValidator( wxGenericValidator(& m_bEnableMonoPA) );
    if (FindWindow(XRCID("ID_CHECKBOX_USBENABLE")))
        FindWindow(XRCID("ID_CHECKBOX_USBENABLE"))->SetValidator( wxGenericValidator(& m_bEnableUSBIP) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USERDIR")))
        FindWindow(XRCID("ID_TEXTCTRL_USERDIR"))->SetValidator( MyValidator(& m_sUserNxDir) );
    if (FindWindow(XRCID("ID_CHECKBOX_REMOVEOLDSF")))
        FindWindow(XRCID("ID_CHECKBOX_REMOVEOLDSF"))->SetValidator( wxGenericValidator(& m_bRemoveOldSessionFiles) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SYSDIR")))
        FindWindow(XRCID("ID_TEXTCTRL_SYSDIR"))->SetValidator( MyValidator(& m_sSystemNxDir) );
    if (FindWindow(XRCID("ID_TEXTCTRL_CUPSPATH")))
        FindWindow(XRCID("ID_TEXTCTRL_CUPSPATH"))->SetValidator( MyValidator(& m_sCupsPath) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USBIPD_SOCKET")))
        FindWindow(XRCID("ID_TEXTCTRL_USBIPD_SOCKET"))->SetValidator( MyValidator(& m_sUsbipdSocket) );
    if (FindWindow(XRCID("ID_SPINCTRL_USB_LOCALPORT")))
        FindWindow(XRCID("ID_SPINCTRL_USB_LOCALPORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iUsbLocalPort) );
    if (FindWindow(XRCID("ID_CHECKBOX_CREATEICON")))
        FindWindow(XRCID("ID_CHECKBOX_CREATEICON"))->SetValidator( wxGenericValidator(& m_bCreateDesktopIcon) );
    if (FindWindow(XRCID("ID_CHECKBOX_RESETMSGBOXES")))
        FindWindow(XRCID("ID_CHECKBOX_RESETMSGBOXES"))->SetValidator( wxGenericValidator(& m_bResetMessageBoxes) );
    if (FindWindow(XRCID("ID_CHECKBOX_LOWERCASE_LOGIN")))
        FindWindow(XRCID("ID_CHECKBOX_LOWERCASE_LOGIN"))->SetValidator( wxGenericValidator(& m_bLowercaseLogin) );
    if (FindWindow(XRCID("ID_CHECKBOX_CLEAR_PASSONABORT")))
        FindWindow(XRCID("ID_CHECKBOX_CLEAR_PASSONABORT"))->SetValidator( wxGenericValidator(& m_bClearPassOnAbort) );
    if (FindWindow(XRCID("ID_CHECKBOX_NOMAGICPIXEL")))
        FindWindow(XRCID("ID_CHECKBOX_NOMAGICPIXEL"))->SetValidator( wxGenericValidator(& m_bDisableMagicPixel) );
    ////@end SessionProperties content construction

    if ((!m_bStorePasswords) && FindWindow(XRCID("ID_CHECKBOX_PWSAVE")))
        FindWindow(XRCID("ID_CHECKBOX_PWSAVE"))->Enable(false);

    ////@begin SessionProperties content initialisation
    ////@end SessionProperties content initialisation

#ifdef __WXMSW__
    // wxSpinCtrl is too small on windows
    wxSize spin_size = m_pCtrlDisplayWidth->GetMinSize();
    spin_size.IncBy(8, 0);
    m_pCtrlDisplayWidth->SetSize(spin_size);
    m_pCtrlDisplayWidth->SetMinSize(spin_size);
    m_pCtrlDisplayWidth->SetSizeHints(spin_size);
    m_pCtrlDisplayHeight->SetSize(spin_size);
    m_pCtrlDisplayHeight->SetMinSize(spin_size);
    m_pCtrlDisplayHeight->SetSizeHints(spin_size);
    Layout();
#endif
    m_pCtrlUseSmartCard->Enable(::wxGetApp().NxSmartCardSupport());

    int fs[7];
    wxFont fv = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxFont ff = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);

    for (int i = 0; i < 7; i++)
        fs[i] = fv.GetPointSize();
    m_pHtmlWindow->SetFonts(fv.GetFaceName(), ff.GetFaceName(), fs);
    m_pHtmlWindow->SetBorders(0);

    wxString version = _("Version") + wxString::Format(wxT(" <B>%s</B>"),
            ::wxGetApp().GetVersion().c_str());
#ifdef __WXDEBUG__
    version += wxT(" (DEBUG)");
#else
    version += wxT(" (RELEASE)");
#endif

    wxString content = ::wxGetApp().LoadFileFromResource(wxT("res/about.html"));
    content.Replace(wxT("<VERSION>"), version);
    content.Replace(wxT("<WXVERSION>"), wxVERSION_STRING);
    content.Replace(wxT("\"res:"), wxT("\"") + ::wxGetApp().GetResourcePrefix());

    m_pHtmlWindow->SetPage(content);
    m_pHtmlWindow->SetBackgroundColour(GetBackgroundColour());
    m_pHtmlWindow->SetBackgroundStyle(GetBackgroundStyle());
    if (!content.IsEmpty()) {
        int width, height;
#ifdef __WXMAC__
        // On MacOS wxHtmlContainerCell behaves unexpected?!
        // -> Set size of about panel statically
        width = 150;
        height = 250;
#else
        m_pCtrlPanelAbout->GetSize(&width, &height);
#endif
        m_pHtmlWindow->GetInternalRepresentation()->Layout(width);
        height = m_pHtmlWindow->GetInternalRepresentation()->GetHeight();
        width = m_pHtmlWindow->GetInternalRepresentation()->GetWidth();
        m_pHtmlWindow->SetSize(width, height);
        m_pHtmlWindow->SetSizeHints(width, height);
        m_pCtrlPanelAbout->Layout();
    }
}

/*!
 * Should we show tooltips?
 */

    bool
SessionProperties::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap SessionProperties::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon SessionProperties::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

    int
SessionProperties::findSelectedShare()
{
    for (int i = 0; i < m_pCtrlSmbShares->GetItemCount(); i++) {
        int state = m_pCtrlSmbShares->GetItemState(i, wxLIST_STATE_SELECTED);
        if (state)
            return i;
    }
    return -1;
}

#ifdef SUPPORT_USBIP
    int
SessionProperties::findSelectedUsbDevice()
{
    for (int i = 0; i < m_pCtrlUsbFilter->GetItemCount(); i++) {
        int state = m_pCtrlUsbFilter->GetItemState(i, wxLIST_STATE_SELECTED);
        if (state)
            return i;
    }
    return -1;
}
#endif

    bool
SessionProperties::readKbdLayouts()
{
    wxString kbdCfg;
    wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &kbdCfg);
    kbdCfg << wxFileName::GetPathSeparator() << wxT("share")
        << wxFileName::GetPathSeparator() << wxT("keyboards");
    wxFileInputStream fis(kbdCfg);
    if (!fis.IsOk()) {
        wxLogError(_("Error while reading keyboard table"));
        return false;
    }
    wxTextInputStream tis(fis);
    m_aKbdLayoutTable.Empty();
    while (!fis.Eof()) {
        wxString line = tis.ReadLine();
        wxStringTokenizer t(line, wxT(","));
        int i = 0;
        long n;
        KbdLayout kl;

        while (t.HasMoreTokens()) {
            switch (i++) {
                case 0:
                    // ISO country code
                    kl.sIsoCode = t.GetNextToken();
                    break;
                case 1:
                    // win32 language code
                    if (t.GetNextToken().ToLong(&n))
                        kl.winCode = n;
                    break;
                case 2:
                    // Human readable name
                    kl.sLayoutName = t.GetNextToken();
                    m_aKbdLayoutTable.Add(kl);
                    break;
                default:
                    wxLogError(_("Invalid line='%s'"), line.c_str());
                    wxLogError(_("Invalid entry in %s"), kbdCfg.c_str());
                    break;
            }
        }
    }
    return true;
}

void SessionProperties::SaveState()
{
    if (m_pCfg)
        m_pCfg->saveState();
    m_sSavedUserNxDir = m_sUserNxDir;
    m_sSavedSystemNxDir = m_sSystemNxDir;
    m_sSavedUsbipdSocket = m_sUsbipdSocket;
    m_iSavedUsbLocalPort = m_iUsbLocalPort;
    m_bSavedCreateDesktopIcon = m_bCreateDesktopIcon;
    m_bSavedLowercaseLogin = m_bLowercaseLogin;
    m_bSavedClearPassOnAbort = m_bClearPassOnAbort;
    m_bSavedResetMessageBoxes = m_bResetMessageBoxes = false;
    m_bSavedDisableMagicPixel = m_bDisableMagicPixel;
}

// ====================== Event handlers ===============================

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_DSETTINGS
 */

void SessionProperties::OnButtonDsettingsClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    switch (m_iSessionType) {
        case MyXmlConfig::STYPE_UNIX:
            if (MyXmlConfig::DTYPE_XDM == m_iUnixDesktopType) {
                XdmPropertyDialog d;
                d.SetConfig(m_pCfg);
                d.Create(this);
                d.ShowModal();
                CheckCfgChanges(false);
            } else {
                X11PropertyDialog d;
                d.SetConfig(m_pCfg);
                d.Create(this);
                d.ShowModal();
                CheckCfgChanges(false);
            }
            break;
        case MyXmlConfig::STYPE_WINDOWS:
            {
                RdpPropertyDialog d;
                d.SetConfig(m_pCfg);
                d.Create(this);
                d.ShowModal();
                CheckCfgChanges(false);
            }
            break;
        case MyXmlConfig::STYPE_VNC:
            {
                VncPropertyDialog d;
                d.SetConfig(m_pCfg);
                d.Create(this);
                d.ShowModal();
                CheckCfgChanges(false);
            }
            break;
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_IMG_CUSTOM
 */

void SessionProperties::OnButtonImgCustomClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    switch (m_iSessionType) {
        case MyXmlConfig::STYPE_UNIX:
        case MyXmlConfig::STYPE_SHADOW:
            {
                UnixImageSettingsDialog d;
                d.SetConfig(m_pCfg);
                d.Create(this);
                d.ShowModal();
                CheckChanged();
            }
            break;
        case MyXmlConfig::STYPE_WINDOWS:
            {
                RdpImageSettingsDialog d;
                d.SetConfig(m_pCfg);
                d.Create(this);
                d.ShowModal();
                CheckChanged();
            }
            break;
        case MyXmlConfig::STYPE_VNC:
            {
                VncImageSettingsDialog d;
                d.SetConfig(m_pCfg);
                d.Create(this);
                d.ShowModal();
                CheckChanged();
            }
            break;
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CACHECLEAN
 */

void SessionProperties::OnButtonCachecleanClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    wxMessageDialog d(this, _("Do you really want to delete all cache directories?"),
            _("Clean cache - OpenNX"), wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION);
    if (d.ShowModal() == wxID_YES) {
        wxString userDir;
        if (wxConfigBase::Get()->Read(wxT("Config/UserNxDir"), &userDir)) {
            myLogTrace(MYTRACETAG, wxT("cleaning cache"));
            wxDir ud;
            if (ud.Open(userDir)) {
                CacheCleaner cc(userDir);
                ud.Traverse(cc);
            }
        }
    }
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SMB
 */

void SessionProperties::OnCheckboxSmbClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    SmbClient s;
    if (s.IsAvailable()) {
#ifdef __LINUX__
        wxString wstr = wxT("");
        wxString sharedir = wxT("/var/lib/samba/usershares");
        if ( IsFileStickyBitSet(sharedir.fn_str()) == 0)
            wstr.Append(wxString::Format(wxT(" %s - sticky bit should be set;"),sharedir.c_str()));
        if (!wstr.IsEmpty()) {
            wstr.Prepend(wxT("Some system settings may be incorrect:"));
            wxLogWarning(wstr.c_str());
        }
#endif
        UpdateDialogConstraints(true);
        CheckChanged();
    } else {
        wxString errstr = wxT("No local samba server is running;");
        wxString checkres = wxGetLibCheckRes(wxT("libsmbclient.so"));
        if (!checkres.IsEmpty())
            errstr.Append(checkres);
        wxLogWarning(errstr.c_str());
        wxDynamicCast(event.GetEventObject(), wxCheckBox)->SetValue(false);
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SMB_ADD
 */

void SessionProperties::OnButtonSmbAddClick( wxCommandEvent& event )
{
    SmbClient sc;
    CupsClient cc;

    wxUnusedVar(event);
    if ((sc.GetShares().GetCount() + cc.GetShares().GetCount()) > 0) {
        ShareProperties d;
        d.SetConfig(m_pCfg);
        d.SetUse(m_bEnableSmbSharing, m_bUseCups);
        d.Create(this);
        if (d.ShowModal() == wxID_OK) {
            size_t sgidx = m_pCfg->aGetShareGroups().GetCount() - 1;
            ArrayOfShares sa = sc.GetShares();
            WX_APPEND_ARRAY(sa, cc.GetShares());
            ShareGroup sg = m_pCfg->aGetShareGroups().Item(sgidx);
            size_t si;
            wxString comment = _("Currently not available");
            for (si = 0; si < sa.GetCount(); si++) {
                if ((sa[si].name == sg.m_sShareName) &&
                        (sa[si].sharetype == sg.m_eType)) {
                    comment = sa[si].description;
                    break;
                }
            }
            int lidx = -1;
            switch (sg.m_eType) {
                case SharedResource::SHARE_UNKNOWN:
                    break;
                case SharedResource::SHARE_SMB_DISK:
                    lidx = m_pCtrlSmbShares->InsertItem(0, sg.m_sShareName, 1);
                    m_pCtrlSmbShares->SetItem(lidx, 1, sg.m_sAlias);
                    break;
                case SharedResource::SHARE_SMB_PRINTER:
                    lidx = m_pCtrlSmbShares->InsertItem(0, sg.m_sShareName, 2);
                    m_pCtrlSmbShares->SetItem(lidx, 1, sg.m_sDriver);
                    break;
                case SharedResource::SHARE_CUPS_PRINTER:
                    lidx = m_pCtrlSmbShares->InsertItem(0, sg.m_sShareName, 3);
                    m_pCtrlSmbShares->SetItem(lidx, 1, sg.m_sDriver);
                    break;
            }
            updateListCtrlColumnWidth(m_pCtrlSmbShares);
        }
        CheckChanged();
    } else {
        wxLogWarning(_("No sharable resources found"));
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SMB_MODIFY
 */

void SessionProperties::OnButtonSmbModifyClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    int idx = findSelectedShare();
    if (idx != -1) {
        ShareProperties d;
        d.SetConfig(m_pCfg);
        d.SetUse(m_bEnableSmbSharing, m_bUseCups);
        d.SetCurrentShare(m_pCtrlSmbShares->GetItemData(idx));
        d.Create(this);
        if (d.ShowModal() == wxID_OK) {
            ShareGroup sg = m_pCfg->aGetShareGroups().Item(m_pCtrlSmbShares->GetItemData(idx));
            switch (sg.m_eType) {
                case SharedResource::SHARE_UNKNOWN:
                    break;
                case SharedResource::SHARE_SMB_DISK:
                    m_pCtrlSmbShares->SetItem(idx, 1, sg.m_sAlias);
                    break;
                case SharedResource::SHARE_SMB_PRINTER:
                    m_pCtrlSmbShares->SetItem(idx, 1, sg.m_sDriver);
                    break;
                case SharedResource::SHARE_CUPS_PRINTER:
                    m_pCtrlSmbShares->SetItem(idx, 1, sg.m_sDriver);
                    break;
            }
            updateListCtrlColumnWidth(m_pCtrlSmbShares);
        }
        CheckChanged();
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SMB_DELETE
 */

void SessionProperties::OnButtonSmbDeleteClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    int idx = findSelectedShare();
    if (idx != -1) {
        int shidx = m_pCtrlSmbShares->GetItemData(idx);
        wxArrayString as = m_pCfg->aGetUsedShareGroups();
        ArrayOfShareGroups asg = m_pCfg->aGetShareGroups();
        as.Remove(asg[shidx].m_sGroupName);
        asg.RemoveAt(shidx);
        m_pCfg->aSetShareGroups(asg);
        m_pCfg->aSetUsedShareGroups(as);
        m_pCfg->iSetUsedShareGroups(m_pCfg->iGetUsedShareGroups() - 1);
        m_pCtrlSmbShares->DeleteItem(idx);
        for (int i = 0; i < m_pCtrlSmbShares->GetItemCount(); i++) {
            int id = m_pCtrlSmbShares->GetItemData(i);
            if (id > shidx)
                m_pCtrlSmbShares->SetItemData(i, id - 1);
        }
        updateListCtrlColumnWidth(m_pCtrlSmbShares);
        CheckChanged();
        UpdateDialogConstraints(false);
    }
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSE_USERDIR
 */

void SessionProperties::OnButtonBrowseUserdirClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    const wxString& dir = wxDirSelector(_("Select User NX directory"),
            m_sUserNxDir, 0, wxDefaultPosition, this);
    if (!dir.IsEmpty()) {
        m_pCtrlUserNxDir->SetValue(dir);
        CheckChanged();
    }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSE_SYSDIR
 */

void SessionProperties::OnButtonBrowseSysdirClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    const wxString& dir = wxDirSelector(_("Select System NX directory"),
            m_sSystemNxDir, 0, wxDefaultPosition, this);
    if (!dir.IsEmpty()) {
        m_pCtrlSystemNxDir->SetValue(dir);
        CheckChanged();
    }
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_DELETE
 */

void SessionProperties::OnDeleteClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (wxMessageBox(wxString::Format(_("Really delete Session '%s' ?"),
                    m_pCfg->sGetName().c_str()), _("Delete Session"),
                wxICON_QUESTION|wxYES_NO|wxNO_DEFAULT) == wxYES) {
        ::wxGetApp().RemoveDesktopEntry(m_pCfg);
        myLogTrace(MYTRACETAG, wxT("Removing '%s'"), m_pCfg->sGetFileName().c_str());
        ::wxRemoveFile(m_pCfg->sGetFileName());
        EndModal(wxID_CLEAR);
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SAVE
 */

void SessionProperties::OnApplyClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    wxConfigBase::Get()->Write(wxT("Config/UserNxDir"), m_sUserNxDir);
    wxConfigBase::Get()->Write(wxT("Config/SystemNxDir"), m_sSystemNxDir);
    wxConfigBase::Get()->Write(wxT("Config/LowercaseLogin"), m_bLowercaseLogin);
    wxConfigBase::Get()->Write(wxT("Config/ClearPassOnAbort"), m_bClearPassOnAbort);
    wxConfigBase::Get()->Write(wxT("Config/DisableMagicPixel"), m_bDisableMagicPixel);
#ifdef SUPPORT_USBIP
    wxConfigBase::Get()->Write(wxT("Config/UsbipdSocket"), m_sUsbipdSocket);
    wxConfigBase::Get()->Write(wxT("Config/UsbipPort"), m_iUsbLocalPort);
#endif

    if (m_bSavedCreateDesktopIcon != m_bCreateDesktopIcon) {
        if (NULL != m_pCfg) {
            if (m_bCreateDesktopIcon)
                ::wxGetApp().CreateDesktopEntry(m_pCfg);
            else
                ::wxGetApp().RemoveDesktopEntry(m_pCfg);
        }
    }
    if (m_bSavedResetMessageBoxes != m_bResetMessageBoxes) {
        if (m_bResetMessageBoxes) {
            wxConfigBase::Get()->DeleteGroup(wxT("SupressedDialogs"));
        }
    }
    SaveState();
    m_pCtrlResetMessageBoxes->SetValue(m_bResetMessageBoxes);
    m_pCtrlApplyButton->Enable(false);
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DPROTO
 */

void SessionProperties::OnComboboxDprotoSelected( wxCommandEvent& event )
{
    if (event.GetInt() == 0) {
        m_iDesktopTypeDialog = m_iUnixDesktopType;
        m_pCtrlDesktopType->SetSelection(m_iDesktopTypeDialog);
    }
    UpdateDialogConstraints(true);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DTYPE
 */

void SessionProperties::OnComboboxDtypeSelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    m_iUnixDesktopType = m_iDesktopTypeDialog;
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DISPTYPE
 */

void SessionProperties::OnComboboxDisptypeSelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_PWSAVE
 */

void SessionProperties::OnCheckboxPwsaveClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SMARTCARD
 */

void SessionProperties::OnCheckboxSmartcardClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_SPEED
 */

void SessionProperties::OnSliderSpeedUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL_WIDTH
 */

void SessionProperties::OnSpinctrlWidthUpdated( wxSpinEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL_HEIGHT
 */

void SessionProperties::OnSpinctrlHeightUpdated( wxSpinEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_DISABLETCPNODEL
 */

void SessionProperties::OnCheckboxDisabletcpnodelClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_DISABLEZCOMP
 */

void SessionProperties::OnCheckboxDisablezcompClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_ENABLESSL
 */

void SessionProperties::OnCheckboxEnablesslClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_CACHEMEM
 */

void SessionProperties::OnComboboxCachememSelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_CACHEDISK
 */

void SessionProperties::OnComboboxCachediskSelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_KBDLAYOUT
 */

void SessionProperties::OnComboboxKbdlayoutSelected( wxCommandEvent& event )
{
    size_t idx = (size_t)m_pCtrlKeyboardLayout->GetClientData(event.GetInt());
    m_sKbdLayoutLanguage = m_aKbdLayoutTable.Item(idx).sIsoCode;
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_REMOVEOLDSF
 */

void SessionProperties::OnCheckboxRemoveoldsfClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_HOST
 */

void SessionProperties::OnTextctrlHostUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlHostname))
        CheckChanged();
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_USERDIR
 */

void SessionProperties::OnTextctrlUserdirUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlUserNxDir))
        CheckChanged();
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SYSDIR
 */

void SessionProperties::OnTextctrlSysdirUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlSystemNxDir))
        CheckChanged();
}



/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL_SMB_SHARES
 */

void SessionProperties::OnListctrlSmbSharesSelected( wxListEvent& event )
{
    wxUnusedVar(event);
    m_pCtrlShareDelete->Enable(true);
    m_pCtrlShareModify->Enable(true);
}

/*!
 * wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_LISTCTRL_SMB_SHARES
 */

void SessionProperties::OnListctrlSmbSharesItemActivated( wxListEvent& event )
{
    wxUnusedVar(event);
    wxCommandEvent e;
    OnButtonSmbModifyClick(e);
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_CUPS
 */

void SessionProperties::OnCheckboxCupsenableClick( wxCommandEvent& event )
{
    wxString errstr = wxT("");
    CupsClient cl;
    if (!cl.IsAvailable()) {
        errstr.Append(wxT("No cups server available;"));
        wxString checkres = wxGetLibCheckRes(wxT("libcups.so"));
        if (!checkres.IsEmpty())
            errstr.Append(checkres);
    }
    if (!wxFileName::IsFileExecutable(m_sCupsPath))
        errstr.Append(wxString::Format(_T(" %s must be executable (755);"),m_sCupsPath.c_str()));
    wxString libdirs = wxT("/usr/lib:/usr/lib64:/usr/libexec");
    wxString ippexec = wxT("");
    wxStringTokenizer t(libdirs, wxT(":"));
    while (t.HasMoreTokens()) {
        wxString tmpstr = t.GetNextToken() + wxT("/cups/backend/ipp");
        if (wxFileName::FileExists(tmpstr)) {
            ippexec = tmpstr;
            break;
        }
    }
    if (ippexec.IsEmpty()) {
        errstr.Append(wxT(" ipp backend not found;"));
    } else if (!wxFileName::IsFileExecutable(ippexec)) {
        errstr.Append(wxString::Format(_T(" CUPS printing support cannot be enabled. Please ensure that permissions for %s are set to 755."),ippexec.c_str()));
    }
    if (errstr.IsEmpty()) {
        UpdateDialogConstraints(true);
        CheckChanged();
    } else {
        wxLogWarning(errstr.c_str());
        wxDynamicCast(event.GetEventObject(), wxCheckBox)->SetValue(false);
        m_bUseCups = false;
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSE_CUPSD
 */

void SessionProperties::OnButtonBrowseCupspathClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    wxFileName fn(m_sCupsPath);
    const wxString& file = ::wxFileSelector(_("Select System CUPS daemon"),
            fn.GetPath(), fn.GetName(), wxEmptyString, wxT("*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, this);
    if (!file.IsEmpty()) {
        m_pCtrlCupsPath->SetValue(file);
        CheckChanged();
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_KEYMANAGE
 */

void SessionProperties::OnButtonKeymanageClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    KeyDialog d;
    d.SetSshKey(m_pCfg->sGetSshKey());
    d.Create(this);
    if (d.ShowModal() != wxCANCEL) {
        m_pCfg->sSetSshKey(d.GetSshKey());
        CheckChanged();
    }
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_MMEDIA
 */

void SessionProperties::OnCheckboxMmediaClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_NATIVEPA
 */

void SessionProperties::OnCheckboxNativePAClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_RATEPA
 */

void SessionProperties::OnComboboxRatePASelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_MONOPA
 */

void SessionProperties::OnCheckboxMonoPAClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL_PORT
 */

void SessionProperties::OnSpinctrlPortUpdated( wxSpinEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL_PORT
 */

void SessionProperties::OnTextctrlPortUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlPort))
        m_pCtrlApplyButton->Enable(true);
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL_WIDTH
 */

void SessionProperties::OnSpinctrlWidthTextUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlDisplayWidth))
        m_pCtrlApplyButton->Enable(true);
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL_HEIGHT
 */

void SessionProperties::OnSpinctrlHeightTextUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlDisplayHeight))
        m_pCtrlApplyButton->Enable(true);
}

/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL_SMBPORT
 */

void SessionProperties::OnSpinctrlSmbportUpdated( wxSpinEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL_SMBPORT
 */

void SessionProperties::OnSpinctrlSmbportTextUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlSmbPort))
        CheckChanged();
}

/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL_CUPSPORT
 */

void SessionProperties::OnSpinctrlCupsportUpdated( wxSpinEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL_CUPSPORT
 */

void SessionProperties::OnSpinctrlCupsportTextUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlCupsPort))
        CheckChanged();
        //m_pCtrlApplyButton->Enable(true);
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_CUPSPATH
 */

void SessionProperties::OnTextctrlCupspathUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlCupsPath))
        CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_USBENABLE
 */

void SessionProperties::OnCHECKBOXUSBENABLEClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_USBADD
 */

void SessionProperties::OnButtonUsbaddClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
#ifdef SUPPORT_USBIP
    USB u;
    if (!u.IsAvailable()) {
        wxLogWarning(_("libusb is not available. No USB devices will be exported"));
        return;
    }
    ArrayOfUSBDevices a = u.GetDevices();
    if (a.IsEmpty()) {
        wxLogWarning(_("No USB devices available."));
        return;
    }
    UsbFilterDetailsDialog d(this);
    d.SetDeviceList(a);
    d.SetDialogMode(UsbFilterDetailsDialog::MODE_ADD);
    if (d.ShowModal() == wxID_OK) {
        SharedUsbDevice dev;
        long tmp;
        if (d.GetVendorID().IsEmpty())
            dev.m_iVendorID = -1;
        else {
            d.GetVendorID().ToLong(&tmp, 16);
            dev.m_iVendorID = tmp;
        }
        if (d.GetProductID().IsEmpty())
            dev.m_iProductID = -1;
        else {
            d.GetProductID().ToLong(&tmp, 16);
            dev.m_iProductID = tmp;
        }
        if (d.GetDeviceClass().IsEmpty())
            dev.m_iClass = -1;
        else {
            d.GetDeviceClass().ToLong(&tmp, 16);
            dev.m_iClass = tmp;
        }
        dev.m_sVendor = d.GetVendor();
        dev.m_sProduct = d.GetProduct();
        dev.m_sSerial = d.GetSerial();
        dev.m_eMode = d.GetForwarding() ? SharedUsbDevice::MODE_REMOTE : SharedUsbDevice::MODE_LOCAL;
        bool found = false;
        for (size_t i = 0; i < m_aUsbForwards.GetCount(); i++) {
            if (dev.cmpNoMode(m_aUsbForwards[i])) {
                found = true;
                break;
            }
        }
        if (!found) {
            appendUsbDevice(dev, m_aUsbForwards.GetCount());
            updateListCtrlColumnWidth(m_pCtrlUsbFilter);
            m_aUsbForwards.Add(dev);
            UpdateDialogConstraints(false);
            CheckChanged();
        }
    }
#endif
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_USBMODIFY
 */

void SessionProperties::OnButtonUsbmodifyClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
#ifdef SUPPORT_USBIP
    int idx = findSelectedUsbDevice();
    if (idx != -1) {
        int aidx = m_pCtrlUsbFilter->GetItemData(idx);
        SharedUsbDevice dev = m_aUsbForwards[aidx];
        UsbFilterDetailsDialog d(this);
        d.SetDialogMode(UsbFilterDetailsDialog::MODE_EDIT);
        d.SetVendorID((-1 == dev.m_iVendorID) ? wxT("") : wxString::Format(wxT("%04X"), (int)dev.m_iVendorID));
        d.SetProductID((-1 == dev.m_iProductID) ? wxT("") : wxString::Format(wxT("%04X"), (int)dev.m_iProductID));
        d.SetDeviceClass((-1 == dev.m_iClass) ? wxT("") : wxString::Format(wxT("%02X"), (int)dev.m_iClass));
        d.SetVendor(dev.m_sVendor);
        d.SetProduct(dev.m_sProduct);
        d.SetSerial(dev.m_sSerial);
        d.SetForwarding(dev.m_eMode == SharedUsbDevice::MODE_REMOTE);
        if (d.ShowModal() == wxID_OK) {
            long tmp;
            if (d.GetVendorID().IsEmpty())
                dev.m_iVendorID = -1;
            else {
                d.GetVendorID().ToLong(&tmp, 16);
                dev.m_iVendorID = tmp;
            }
            if (d.GetProductID().IsEmpty())
                dev.m_iProductID = -1;
            else {
                d.GetProductID().ToLong(&tmp, 16);
                dev.m_iProductID = tmp;
            }
            if (d.GetDeviceClass().IsEmpty())
                dev.m_iClass = -1;
            else {
                d.GetDeviceClass().ToLong(&tmp, 16);
                dev.m_iClass = tmp;
            }
            dev.m_sVendor = d.GetVendor();
            dev.m_sProduct = d.GetProduct();
            dev.m_sSerial = d.GetSerial();
            dev.m_eMode = d.GetForwarding() ? SharedUsbDevice::MODE_REMOTE : SharedUsbDevice::MODE_LOCAL;
            m_aUsbForwards[aidx] = dev;
            wxString lbl = (-1 == dev.m_iVendorID) ? wxT("*") : wxString::Format(wxT("%04X"), (int)dev.m_iVendorID);
            m_pCtrlUsbFilter->SetItem(idx, 0, lbl, (dev.m_eMode == SharedUsbDevice::MODE_REMOTE) ? 1 : 0);
            lbl = (-1 == dev.m_iProductID) ? wxT("*") : wxString::Format(wxT("%04X"), (int)dev.m_iProductID);
            m_pCtrlUsbFilter->SetItem(idx, 1, lbl);
            lbl = (-1 == dev.m_iClass) ? wxT("*") : wxString::Format(wxT("%02X"), (int)dev.m_iClass);
            m_pCtrlUsbFilter->SetItem(idx, 2, lbl);
            lbl = dev.toShortString().IsEmpty() ? wxT("*") : dev.toShortString();
            m_pCtrlUsbFilter->SetItem(idx, 3, lbl);
            lbl = dev.m_sSerial.IsEmpty() ? wxT("*") : dev.m_sSerial;
            m_pCtrlUsbFilter->SetItem(idx, 4, lbl);
            updateListCtrlColumnWidth(m_pCtrlUsbFilter);
            UpdateDialogConstraints(false);
            CheckChanged();
        }
    }
#endif
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_USBDELETE
 */

void SessionProperties::OnButtonUsbdeleteClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
#ifdef SUPPORT_USBIP
    int idx = findSelectedUsbDevice();
    if (idx != -1) {
        int aidx = m_pCtrlUsbFilter->GetItemData(idx);
        m_aUsbForwards.RemoveAt(aidx);
        m_pCtrlUsbFilter->DeleteItem(idx);
        for (int i = 0; i < m_pCtrlUsbFilter->GetItemCount(); i++) {
            int id = m_pCtrlUsbFilter->GetItemData(i);
            if (id > aidx)
                m_pCtrlUsbFilter->SetItemData(i, id - 1);
        }
        updateListCtrlColumnWidth(m_pCtrlUsbFilter);
        CheckChanged();
        UpdateDialogConstraints(false);
    }
#endif
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL_USBFILTER
 */

void SessionProperties::OnListctrlUsbfilterSelected( wxListEvent& event )
{
    wxUnusedVar(event);
#ifdef SUPPORT_USBIP
    m_pCtrlUsbModify->Enable(true);
    m_pCtrlUsbDelete->Enable(true);
#endif
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_LISTCTRL_USBFILTER
 */

void SessionProperties::OnListctrlUsbfilterItemActivated( wxListEvent& event )
{
#ifdef SUPPORT_USBIP
    OnButtonUsbmodifyClick(event);
#else
    wxUnusedVar(event);
#endif
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL_USB_LOCALPORT
 */

void SessionProperties::OnSpinctrlUsbLocalportUpdated( wxSpinEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL_USB_LOCALPORT
 */

void SessionProperties::OnSpinctrlUsbLocalportTextUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlUsbLocalPort))
        m_pCtrlApplyButton->Enable(true);
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_USBIPD_SOCKET
 */

void SessionProperties::OnTextctrlUsbipdSocketTextUpdated( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlUsbIpdSocket))
        CheckChanged();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BROWSE_USBIPD_SOCKET
 */

void SessionProperties::OnButtonBrowseUsbipdSocketClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    wxFileName fn(m_sUsbipdSocket);
    const wxString& file = ::wxFileSelector(_("Select path of USBIPD socket"),
            fn.GetPath(), fn.GetName(), wxEmptyString, wxT("*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, this);
    if (!file.IsEmpty()) {
        m_pCtrlUsbIpdSocket->SetValue(file);
        CheckChanged();
    }
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_CREATEICON
 */

void SessionProperties::OnCheckboxCreateiconClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_PROXY
 */

void SessionProperties::OnCheckboxProxyClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PROXYSETTINGS
 */
void SessionProperties::OnButtonProxysettingsClick( wxCommandEvent& )
{
    ProxyPropertyDialog d(this);
    d.SetSProxyHost(m_sProxyHost);
    d.SetIProxyPort(m_iProxyPort);
    d.SetSProxyUser(m_sProxyUser);
    d.SetSProxyPass(m_sProxyPass);
    d.SetBProxyPassRemember(m_bProxyPassRemember);
    d.SetSProxyCommand(m_sProxyCommand);
    d.SetBExternalProxy(m_bExternalProxy);
    d.SetBUseProxy(!m_bExternalProxy);
    d.UpdateDialogConstraints();
    if (wxID_OK == d.ShowModal()) {
        m_sProxyHost = d.GetSProxyHost();
        m_iProxyPort = d.GetIProxyPort();
        m_sProxyUser = d.GetSProxyUser();
        m_sProxyPass = d.GetSProxyPass();
        m_bProxyPassRemember = d.GetBProxyPassRemember();
        m_bExternalProxy = d.GetBExternalProxy();
        m_sProxyCommand = d.GetSProxyCommand();
        CheckChanged();
    }
}

#if defined(__WXMSW__)

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_DISABLEDX
 */

void SessionProperties::OnCheckboxDisabledxClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}
#endif

#if defined(__WXMSW__)

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_GRABKB
 */

void SessionProperties::OnCheckboxGrabkbClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}
#endif


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_NODEFERRED
 */

void SessionProperties::OnCheckboxNodeferredClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_RESETMSGBOXES
 */

void SessionProperties::OnCheckboxResetmsgboxesClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_KBDOTHER
 */

void SessionProperties::OnCheckboxKbdotherClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_IMG_CUSTOM
 */

void SessionProperties::OnCheckboxImgCustomClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    UpdateDialogConstraints(true);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_LOWERCASE_LOGIN
 */

void SessionProperties::OnCheckboxLowercaseLoginClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

#if defined(__WXMSW__)

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_CLIPFILTER
 */

void SessionProperties::OnComboboxClipfilterSelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}
#endif


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_CLEAR_PASSONABORT
 */

void SessionProperties::OnCheckboxClearPassonabortClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_NOMAGICPIXEL
 */

void SessionProperties::OnCheckboxNomagicpixelClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    CheckChanged();
}

