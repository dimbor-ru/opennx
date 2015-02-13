// $Id: LoginDialog.cpp 692 2012-02-20 10:35:37Z felfert $
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
#include "config.h"
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "LoginDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/config.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/xml/xml.h>
#include <wx/cshelp.h>

////@begin includes
////@end includes

#include "LoginDialog.h"
#include "MyXmlConfig.h"
#include "SessionProperties.h"
#include "MySession.h"
#include "Icon.h"
#include "opennxApp.h"
#include "SupressibleMessageDialog.h"

////@begin XPM images
////@end XPM images

#include "trace.h"
ENABLE_TRACE;

#ifdef SINGLE_SESSION
# define NXSSH_TIMER 5432
#endif
#define AUTOLOGIN_TIMER 5433

/*!
 * LoginDialog type definition
 */

IMPLEMENT_CLASS( LoginDialog, wxDialog )

    /*!
     * LoginDialog event table definition
     */

BEGIN_EVENT_TABLE( LoginDialog, wxDialog )

    ////@begin LoginDialog event table entries
    EVT_INIT_DIALOG( LoginDialog::OnInitDialog )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_SESSION"), LoginDialog::OnComboboxSessionSelected )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_SMARTCARD"), LoginDialog::OnCheckboxSmartcardClick )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_GUESTLOGIN"), LoginDialog::OnCheckboxGuestloginClick )

    EVT_BUTTON( XRCID("ID_BUTTON_CONFIGURE"), LoginDialog::OnButtonConfigureClick )

    EVT_BUTTON( wxID_OK, LoginDialog::OnOkClick )

    ////@end LoginDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, LoginDialog::OnContextHelp)
#ifdef __WXMAC__
    EVT_MENU(wxID_EXIT, LoginDialog::OnMenuExit)
#endif

#ifdef SINGLE_SESSION
    EVT_TIMER(NXSSH_TIMER, LoginDialog::OnTimer)
#endif
    EVT_TIMER(AUTOLOGIN_TIMER, LoginDialog::OnLoginTimer)

END_EVENT_TABLE()

    /*!
     * LoginDialog constructors
     */

    LoginDialog::LoginDialog( )
: m_pCurrentCfg(NULL)
#ifdef SINGLE_SESSION
, m_cNxSshWatchTimer(this, NXSSH_TIMER)
#endif
, m_cAutoLoginTimer(this, AUTOLOGIN_TIMER)
{
    m_bGuestLogin = false;
    m_bUseSmartCard = false;
}

    LoginDialog::LoginDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
: m_pCurrentCfg(NULL)
#ifdef SINGLE_SESSION
, m_cNxSshWatchTimer(this, NXSSH_TIMER)
#endif
, m_cAutoLoginTimer(this, AUTOLOGIN_TIMER)
{
    m_bGuestLogin = false;
    m_bUseSmartCard = false;
    Create(parent, id, caption, pos, size, style);
}

LoginDialog::~LoginDialog()
{
    if (m_pCurrentCfg)
        delete m_pCurrentCfg;
    m_pCurrentCfg = NULL;
}

void LoginDialog::SetInitialFocus()
{
    if (m_pCtrlUsername->GetValue().IsEmpty()) {
        m_pCtrlUsername->SetFocus();
        ::myLogTrace(MYTRACETAG, wxT("set initial focus on user name"));
    } else {
        wxString p = m_pCtrlPassword->GetValue();
        if (p.IsEmpty() || p.IsSameAs(wxT("\010\010\010\010\010\010\010\010\010\010\010\010\010\010\010\010"))) {
            m_pCtrlPassword->SetFocus();
            ::myLogTrace(MYTRACETAG, wxT("set initial focus on password"));
        } else {
            m_pCtrlLoginButton->SetFocus();
            ::myLogTrace(MYTRACETAG, wxT("set initial focus on login button"));
        }
    }
}

void LoginDialog::ReadConfigDirectory()
{
    wxString cfgdir;
    wxConfigBase::Get()->Read(wxT("Config/UserNxDir"), &cfgdir);
    cfgdir = cfgdir + wxFileName::GetPathSeparator() + wxT("config");
    m_aConfigFiles.Empty();

    wxDir::GetAllFiles(cfgdir, &m_aConfigFiles, wxT("*.nxs"), wxDIR_FILES);
    size_t i;
    m_sSessionName.Empty();
    if (m_pCurrentCfg)
        delete m_pCurrentCfg;
    m_pCurrentCfg = NULL;
    m_pCtrlSessionName->Clear();
    ::myLogTrace(MYTRACETAG, wxT("ReadConfigDirectory: LastSession='%s'"), m_sLastSessionFilename.c_str());
    if (m_sLastSessionFilename.StartsWith(wxT("http://")) ||
            m_sLastSessionFilename.StartsWith(wxT("https://")) ||
            m_sLastSessionFilename.StartsWith(wxT("ftp://")) ||
            ((m_aConfigFiles.Index(m_sLastSessionFilename) == wxNOT_FOUND) &&
             (wxFile::Exists(m_sLastSessionFilename)))) {
        m_aConfigFiles.Add(m_sLastSessionFilename);
        ::myLogTrace(MYTRACETAG, wxT("ReadConfigDirectory: Adding '%s'"), m_sLastSessionFilename.c_str());
    }
    for (i = 0; i < m_aConfigFiles.GetCount(); i++) {
        MyXmlConfig cfg(m_aConfigFiles[i]);
        if (cfg.IsValid()) {
            m_pCtrlSessionName->Append(cfg.sGetName(), (void *)m_aConfigFiles[i].c_str());
            if ((cfg.sGetFileName() == m_sLastSessionFilename) ||
                    (cfg.sGetName() == m_sLastSessionFilename)) {
                m_pCurrentCfg = new MyXmlConfig(m_aConfigFiles[i]);
                m_sSessionName = cfg.sGetName();
                m_bGuestLogin = cfg.bGetGuestMode();
                if (m_bGuestLogin) {
                    m_sTmpUsername = cfg.sGetUsername();
                    m_sTmpPassword = cfg.sGetPassword();
                    m_sUsername = wxEmptyString;
                    m_sPassword = wxEmptyString;
                } else {
                    m_sUsername = cfg.sGetUsername();
                    m_sPassword = cfg.sGetPassword();
                }
                m_bUseSmartCard = ::wxGetApp().NxSmartCardSupport() && cfg.bGetUseSmartCard();
            }
        }
    }
    if (m_pCurrentCfg) {
        ::myLogTrace(MYTRACETAG, wxT("SS1='%s'"), m_sSessionName.c_str());
        m_pCtrlSessionName->SetStringSelection(m_sSessionName);
        wxCommandEvent event;
        OnComboboxSessionSelected(event);
    } else {
        ::myLogTrace(MYTRACETAG, wxT("SS2='%s'"), m_sLastSessionFilename.c_str());
        // Last session name might be a plain session name (backward compatibility)
        m_pCtrlSessionName->SetStringSelection(m_sLastSessionFilename);
        wxCommandEvent event;
        OnComboboxSessionSelected(event);
    }
}

#ifdef __WXMAC__
void LoginDialog::SelectSession(wxString name)
{
    MyXmlConfig cfg(name);
    if (cfg.IsValid()) {
        m_pCtrlSessionName->Append(cfg.sGetName(), (void *)name.c_str());
        m_pCtrlSessionName->SetStringSelection(cfg.sGetName());
        wxCommandEvent event;
        OnComboboxSessionSelected(event);
    }
}
#endif

/*!
 * LoginDialog creator
 */

bool LoginDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
    ////@begin LoginDialog member initialisation
    m_sTmpUsername = wxT("");
    m_sTmpPassword = wxT("");
    m_pCtrlUsername = NULL;
    m_pCtrlPassword = NULL;
    m_pCtrlSessionName = NULL;
    m_pCtrlUseSmartCard = NULL;
    m_pCtrlGuestLogin = NULL;
    m_pCtrlConfigure = NULL;
    m_pCtrlLoginButton = NULL;
    ////@end LoginDialog member initialisation

    ////@begin LoginDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end LoginDialog creation
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

void LoginDialog::Run()
{
    wxCommandEvent event;
    OnOkClick(event);
}

/*!
 * Control creation for LoginDialog
 */

void LoginDialog::CreateControls()
{

    ////@begin LoginDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_LOGIN")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlUsername = XRCCTRL(*this, "ID_TEXTCTRL_USERNAME", wxTextCtrl);
    m_pCtrlPassword = XRCCTRL(*this, "ID_TEXTCTRL_PASSWORD", wxTextCtrl);
    m_pCtrlSessionName = XRCCTRL(*this, "ID_COMBOBOX_SESSION", wxComboBox);
    m_pCtrlUseSmartCard = XRCCTRL(*this, "ID_CHECKBOX_SMARTCARD", wxCheckBox);
    m_pCtrlGuestLogin = XRCCTRL(*this, "ID_CHECKBOX_GUESTLOGIN", wxCheckBox);
    m_pCtrlConfigure = XRCCTRL(*this, "ID_BUTTON_CONFIGURE", wxButton);
    m_pCtrlLoginButton = XRCCTRL(*this, "wxID_OK", wxButton);
    // Set validators
    if (FindWindow(XRCID("ID_TEXTCTRL_USERNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_USERNAME"))->SetValidator( wxGenericValidator(& m_sUsername) );
    if (FindWindow(XRCID("ID_TEXTCTRL_PASSWORD")))
        FindWindow(XRCID("ID_TEXTCTRL_PASSWORD"))->SetValidator( wxGenericValidator(& m_sPassword) );
    if (FindWindow(XRCID("ID_COMBOBOX_SESSION")))
        FindWindow(XRCID("ID_COMBOBOX_SESSION"))->SetValidator( wxGenericValidator(& m_sSessionName) );
    if (FindWindow(XRCID("ID_CHECKBOX_SMARTCARD")))
        FindWindow(XRCID("ID_CHECKBOX_SMARTCARD"))->SetValidator( wxGenericValidator(& m_bUseSmartCard) );
    if (FindWindow(XRCID("ID_CHECKBOX_GUESTLOGIN")))
        FindWindow(XRCID("ID_CHECKBOX_GUESTLOGIN"))->SetValidator( wxGenericValidator(& m_bGuestLogin) );
    ////@end LoginDialog content construction

    // Create custom windows not generated automatically here.

    ////@begin LoginDialog content initialisation
    ////@end LoginDialog content initialisation

    ReadConfigDirectory();
    m_bUseSmartCard = ::wxGetApp().NxSmartCardSupport() && m_pCurrentCfg && m_pCurrentCfg->bGetUseSmartCard();
    m_pCtrlUseSmartCard->SetValue(m_bUseSmartCard);
    m_pCtrlUseSmartCard->Enable(::wxGetApp().NxSmartCardSupport() && m_pCurrentCfg && m_pCurrentCfg->IsWritable());
    if (m_bGuestLogin) {
        m_pCtrlUsername->Enable(false);
        m_pCtrlPassword->Enable(false);
    }
    m_pCtrlGuestLogin->Enable(m_pCurrentCfg && m_pCurrentCfg->IsWritable());
    m_pCtrlConfigure->Enable(m_pCurrentCfg && m_pCurrentCfg->IsWritable());
#ifdef SINGLE_SESSION
    m_pCtrlLoginButton->Enable(false);
    m_cNxSshWatchTimer.Start(1000);
    ::myLogTrace(MYTRACETAG, wxT("Starting nxssh watch timer"));
#endif
}

void LoginDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SMARTCARD
 */

void LoginDialog::OnCheckboxSmartcardClick( wxCommandEvent& event )
{
    // Nothing to do here (validator sets var already)
    event.Skip();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_GUESTLOGIN
 */

void LoginDialog::OnCheckboxGuestloginClick( wxCommandEvent& event )
{
    if (m_pCtrlGuestLogin->IsChecked()) {
        m_sTmpUsername = m_pCtrlUsername->GetValue();
        m_sTmpPassword = m_pCtrlPassword->GetValue();
        m_pCtrlUsername->SetValue(wxEmptyString);
        m_pCtrlPassword->SetValue(wxEmptyString);
        m_pCtrlUsername->Enable(false);
        m_pCtrlPassword->Enable(false);
    } else {
        m_pCtrlUsername->SetValue(m_sTmpUsername);
        m_pCtrlPassword->SetValue(m_sTmpPassword);
        m_pCtrlUsername->Enable(true);
        m_pCtrlPassword->Enable(true);
    }
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool LoginDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap LoginDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon LoginDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CONFIGURE
 */

void LoginDialog::OnButtonConfigureClick( wxCommandEvent& event )
{
    if (m_pCurrentCfg) {
        SessionProperties d;
        d.SetConfig(m_pCurrentCfg);
        wxString fn = m_pCurrentCfg->sGetFileName();
        d.Create(this);
        switch (d.ShowModal()) {
            case wxID_CANCEL:
                delete m_pCurrentCfg;
                m_pCurrentCfg = new MyXmlConfig(fn);
                break;
            case wxID_CLEAR:
                ::myLogTrace(MYTRACETAG, wxT("deleting '%s'"), fn.c_str());
                ::wxRemoveFile(fn);
                ReadConfigDirectory();
                break;
            case wxID_OK:
                m_bUseSmartCard = ::wxGetApp().NxSmartCardSupport() &&
                    m_pCurrentCfg->bGetUseSmartCard();
                m_pCtrlUseSmartCard->SetValue(m_bUseSmartCard);
                if (!m_pCurrentCfg->SaveToFile())
                    wxMessageBox(wxString::Format(_("Could not save session to\n%s"),
                                m_pCurrentCfg->sGetFileName().c_str()), _("Error saving - OpenNX"),
                            wxICON_ERROR | wxOK);
                wxConfigBase::Get()->Write(wxT("Config/UserNxDir"), d.GetsUserNxDir());
                wxConfigBase::Get()->Write(wxT("Config/SystemNxDir"), d.GetsSystemNxDir());
#ifdef SUPPORT_USBIP
                wxConfigBase::Get()->Write(wxT("Config/UsbipdSocket"), d.GetUsbipdSocket());
                wxConfigBase::Get()->Write(wxT("Config/UsbipPort"), d.GetUsbLocalPort());
#endif
                bool bDTI = ::wxGetApp().CheckDesktopEntry(m_pCurrentCfg);
                if (d.GetbCreateDesktopIcon() != bDTI) {
                    if (d.GetbCreateDesktopIcon())
                        ::wxGetApp().CreateDesktopEntry(m_pCurrentCfg);
                    else
                        ::wxGetApp().RemoveDesktopEntry(m_pCurrentCfg);
                }
                break;
        }
    }
    event.Skip();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_SESSION
 */

void LoginDialog::OnComboboxSessionSelected( wxCommandEvent& event )
{
    if (m_pCurrentCfg)
        delete m_pCurrentCfg;
    m_pCurrentCfg = NULL;
    int i = m_pCtrlSessionName->GetSelection();
    if (i != wxNOT_FOUND) {
        wxString fn = (wxChar *)m_pCtrlSessionName->GetClientData(i);
        MyXmlConfig cfg(fn);
        if (cfg.IsValid()) {
            m_pCurrentCfg = new MyXmlConfig(cfg.sGetFileName());
            m_bGuestLogin = cfg.bGetGuestMode();
            m_pCtrlGuestLogin->SetValue(m_bGuestLogin);
            if (m_bGuestLogin) {
                m_sTmpUsername = cfg.sGetUsername();
                m_pCtrlUsername->SetValue(wxEmptyString);
                m_sTmpPassword = cfg.sGetPassword();
                m_pCtrlPassword->SetValue(wxEmptyString);
                m_pCtrlPassword->Enable(false);
                m_pCtrlUsername->Enable(false);
            } else {
                m_pCtrlPassword->SetValue(cfg.sGetPassword());
                m_pCtrlUsername->SetValue(cfg.sGetUsername());
                m_pCtrlPassword->Enable(true);
                m_pCtrlUsername->Enable(true);
            }
            m_pCtrlUseSmartCard->SetValue(::wxGetApp().NxSmartCardSupport() && cfg.bGetUseSmartCard());
        }
    }
    m_pCtrlUseSmartCard->Enable(m_pCurrentCfg && m_pCurrentCfg->IsWritable());
    m_pCtrlGuestLogin->Enable(m_pCurrentCfg && m_pCurrentCfg->IsWritable());
    m_pCtrlConfigure->Enable(m_pCurrentCfg && m_pCurrentCfg->IsWritable());
    if (m_pCurrentCfg && m_pCurrentCfg->WasOldConfig()) {
        wxString msg;
        if (m_pCurrentCfg->IsWritable()) {
            // m_pCurrentCfg->SaveToFile();
            msg = _("An old session configuration has been detected.\nThe session '%s' has been converted to the new format.\nPlease verify the custom image compression settings.");
        } else {
            msg = _("An old session configuration has been detected.\nThe session '%s' has been converted but could not be saved.\nPlease verify the custom image compression settings.");
        }
        wxString cfgid(wxT("oldcfg."));
        SupressibleMessageDialog d(this, wxString::Format(msg, m_pCurrentCfg->sGetName().c_str()),
                _("Warning - OpenNX"), wxOK|wxICON_EXCLAMATION);
        d.ShowConditional(cfgid.Append(msg.Left(15)), wxID_OK);
    }
    if (!m_bGuestLogin)
        SetInitialFocus();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void LoginDialog::OnOkClick(wxCommandEvent& event)
{
    if (m_pCurrentCfg) {
        TransferDataFromWindow();
        m_pCurrentCfg->bSetGuestMode(m_bGuestLogin);
        if (!m_bGuestLogin) {
            m_pCurrentCfg->sSetUsername(m_sUsername);
            if (m_pCurrentCfg->bGetRememberPassword())
                m_pCurrentCfg->sSetPassword(m_sPassword);
        }
        m_pCurrentCfg->bSetUseSmartCard(m_bUseSmartCard);
        if (m_bUseSmartCard || (!::wxGetApp().NxProxyAvailable()))
            m_pCurrentCfg->bSetEnableSSL(true);

        // Workaround for a bug-compatibility to original nxclient:
        // At least in guest mode, original stores RDP password,
        // even if remember is false.
        if (m_bGuestLogin)
            m_pCurrentCfg->bSetRdpRememberPassword(true);

        MySession s;
#ifdef SINGLE_SESSION
        m_cNxSshWatchTimer.Stop();
        ::myLogTrace(MYTRACETAG, wxT("Stopping nxssh watch timer"));
#endif
        Disable();
        bool b = s.Create(*m_pCurrentCfg, m_sPassword, this);
        Enable();
        if (!b) {
            // Clear password after failure or abort.
            if (!m_bGuestLogin) {
                bool clpw = true;
                wxConfigBase::Get()->Read(wxT("Config/ClearPassOnAbort"), &clpw, true);
                if (clpw) {
                    m_pCtrlPassword->SetValue(wxEmptyString);
                    m_pCtrlPassword->SetFocus();
                }
            }
#ifdef SINGLE_SESSION
            m_cNxSshWatchTimer.Start(1000);
            ::myLogTrace(MYTRACETAG, wxT("Starting nxssh watch timer"));
#endif
            return;
        }
        if (m_pCurrentCfg->IsWritable()) {
            if (!m_pCurrentCfg->SaveToFile())
                wxMessageBox(wxString::Format(_("Could not save session to\n%s"),
                            m_pCurrentCfg->sGetFileName().c_str()), _("Error saving - OpenNX"),
                        wxICON_ERROR | wxOK);

        }
        m_sLastSessionFilename = m_pCurrentCfg->sGetFileName();
    }
    event.Skip();
}

#ifdef __WXMAC__
/*!
 * Handle quit event on Mac OSX
 */
void LoginDialog::OnMenuExit(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
    event.Skip();
}
#endif

/*!
 * Handle events from AutoLoginTimer
 */
void LoginDialog::OnLoginTimer(wxTimerEvent&)
{
    wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    AddPendingEvent(ev);
}

#ifdef SINGLE_SESSION
/*!
 * Handle events from NxSshWatchTimer
 */
void LoginDialog::OnTimer(wxTimerEvent&)
{
    wxArrayString cmdout;
    wxExecute(wxT("ps h -C nxssh"), cmdout, wxEXEC_SYNC|wxEXEC_NODISABLE);
    bool enable = (cmdout.GetCount() == 0);
    if (NULL != m_pCtrlLoginButton)
        m_pCtrlLoginButton->Enable(enable);
    if (enable && ::wxGetApp().AutoLogin()) {
        wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
        AddPendingEvent(ev);
    }
}
#endif


/*!
 * wxEVT_INIT_DIALOG event handler for ID_DIALOG_LOGIN
 */

void LoginDialog::OnInitDialog( wxInitDialogEvent& event )
{
    wxDialog::OnInitDialog(event);
#ifndef SINGLE_SESSION
    if (::wxGetApp().AutoLogin())
        m_cAutoLoginTimer.Start(1000, wxTIMER_ONE_SHOT);
#endif
    event.Skip();
}

