// $Id: MyWizard.cpp 686 2012-02-17 21:25:41Z felfert $
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
#pragma implementation "MyWizard.h"
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
////@end includes
#include <wx/config.h>
#ifdef __WXMAC__
# include <wx/utils.h>
#endif

#include "MyWizard.h"
#include "MyXmlConfig.h"
#include "MyValidator.h"
#include "X11PropertyDialog.h"
#include "XdmPropertyDialog.h"
#include "RdpPropertyDialog.h"
#include "VncPropertyDialog.h"
#include "SessionProperties.h"
#include "opennxApp.h"
#include "Icon.h"
#include "WrappedStatic.h"

////@begin XPM images
////@end XPM images

#include "trace.h"
ENABLE_TRACE;

/*!
 * MyWizard type definition
 */

IMPLEMENT_CLASS( MyWizard, wxWizard )

    /*!
     * MyWizard event table definition
     */

BEGIN_EVENT_TABLE( MyWizard, wxWizard )

    ////@begin MyWizard event table entries
    ////@end MyWizard event table entries

END_EVENT_TABLE()

    /*!
     * MyWizard constructors
     */

    MyWizard::MyWizard( )
    : minW(290)
      , minH(200)
{
    m_pCfg = new MyXmlConfig();
}

    MyWizard::MyWizard( wxWindow* parent, wxWindowID id, const wxPoint& pos )
    : minW(290)
      , minH(200)
{
    m_pCfg = new MyXmlConfig();
    Create(parent, id, pos);
}

MyWizard::~MyWizard()
{
    if (m_pCfg)
        delete m_pCfg;
}

/*!
 * MyWizard creator
 */

bool MyWizard::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxPoint& WXUNUSED(pos) )
{
    ////@begin MyWizard member initialisation
    m_pPageWelcome = NULL;
    m_pPageSession = NULL;
    m_pPageDesktop = NULL;
    m_pPageSecurity = NULL;
    m_pPageFinish = NULL;
    ////@end MyWizard member initialisation

    ////@begin MyWizard creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/opennx-wizard.png")));
    ////@end MyWizard creation
    return TRUE;
}

/*!
 * Control creation for MyWizard
 */

void MyWizard::CreateControls()
{
    ////@begin MyWizard content construction
    if (!wxXmlResource::Get()->LoadObject(this, GetParent(), _T("ID_WIZARD"), wxT("wxWizard")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));

    for (wxWindowList::Node* node = GetChildren().GetFirst(); node; node = node->GetNext())
    {
        wxWizardPage* page = wxDynamicCast(node->GetData(), wxWizardPage);
        if (page)
            GetPageAreaSizer()->Add(page);
    }

    m_pPageWelcome = XRCCTRL(*this, "ID_WIZARDPAGE_WELCOME", WizardPageWelcome);
    m_pPageSession = XRCCTRL(*this, "ID_WIZARDPAGE_SESSION", WizardPageSession);
    m_pPageDesktop = XRCCTRL(*this, "ID_WIZARDPAGE_DESKTOP", WizardPageDesktop);
    m_pPageSecurity = XRCCTRL(*this, "ID_WIZARDPAGE_SECURITY", WizardPageSecurity);
    m_pPageFinish = XRCCTRL(*this, "ID_WIZARDPAGE_FINISH", WizardPageFinish);
    ////@end MyWizard content construction

    // Create custom windows not generated automatically here.

    ////@begin MyWizard content initialisation
    ////@end MyWizard content initialisation
    nextButton = wxDynamicCast(FindWindowById(wxID_FORWARD, this), wxButton);
    m_pPageWelcome->Create(NULL);
    m_pPageSession->Create(NULL);
    m_pPageDesktop->Create(NULL);
    m_pPageSecurity->Create(NULL);
    m_pPageFinish->Create(NULL);

    for (wxWindowList::Node* n = GetChildren().GetFirst(); n; n = n->GetNext()) {
        wxWizardPage* p = wxDynamicCast(n->GetData(), wxWizardPage);
        if (p) {
            int w, h;
            p->GetSize(&w, &h);
            if (w > minW)
                minW = w;
            if (h > minH)
                minH = h;
        }
    }
}

bool MyWizard::RunWizard(wxWizardPage *firstPage)
{
    wxCHECK_MSG( firstPage, false, wxT("can't run empty wizard") );

    // can't return false here because there is no old page
    (void)ShowPage(firstPage, true /* forward */);
    int result = ShowModal();
    return (result == wxID_OK);
}

void MyWizard::EnableNext(bool enable)
{
    nextButton->Enable(enable);
}

void MyWizard::KeyTyped()
{
    wxWizardPage *p = GetCurrentPage();
    if (p->IsKindOf(CLASSINFO(WizardPageSession)))
        wxDynamicCast(p, WizardPageSession)->KeyTyped();
}

wxSize MyWizard::GetPageSize() const
{
    // Original Height of 290 is waaayyy to big
    return wxSize(minW, minH);
}

/*!
 * Runs the wizard.
 */

bool MyWizard::Run()
{
    wxWindowListNode* node = GetChildren().GetFirst();
    while (node)
    {
        wxWizardPage* startPage = wxDynamicCast(node->GetData(), wxWizardPage);
        if (startPage) {
            m_bCancelForced = false;
            bool ret = RunWizard(startPage);
            if (m_bCancelForced)
                ret = false;
            return ret;
        }
        node = node->GetNext();
    }
    return FALSE;
}

/*!
 * Should we show tooltips?
 */

bool MyWizard::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap MyWizard::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon MyWizard::GetIconResource( const wxString& name)
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

wxString MyWizard::sGetConfigName()
{
    return m_pCfg ? m_pCfg->sGetFileName() : _T("");
}

/*!
 * WizardPageWelcome type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardPageWelcome, wxWizardPageSimple )

    /*!
     * WizardPageWelcome event table definition
     */

BEGIN_EVENT_TABLE( WizardPageWelcome, wxWizardPageSimple )

    ////@begin WizardPageWelcome event table entries
    EVT_WIZARD_PAGE_CHANGED( -1, WizardPageWelcome::OnWizardpageWelcomePageChanged )

    ////@end WizardPageWelcome event table entries

END_EVENT_TABLE()

    /*!
     * WizardPageWelcome constructors
     */

WizardPageWelcome::WizardPageWelcome( )
{
}

WizardPageWelcome::WizardPageWelcome( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardPageWelcome creator
 */

bool WizardPageWelcome::Create( wxWizard* WXUNUSED(parent) )
{
    ////@begin WizardPageWelcome member initialisation
    ////@end WizardPageWelcome member initialisation

    ////@begin WizardPageWelcome creation
    CreateControls();
    ////@end WizardPageWelcome creation
    CreateControls();
    return TRUE;
}

/*!
 * Control creation for WizardPageWelcome
 */

void WizardPageWelcome::CreateControls()
{
    ////@begin WizardPageWelcome content construction
    ////@end WizardPageWelcome content construction

    // Create custom windows not generated automatically here.

    ////@begin WizardPageWelcome content initialisation
    ////@end WizardPageWelcome content initialisation
}

/*!
 * Should we show tooltips?
 */

bool WizardPageWelcome::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardPageWelcome::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    ////@begin WizardPageWelcome bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end WizardPageWelcome bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardPageWelcome::GetIconResource( const wxString& name )
{
    // Icon retrieval
    ////@begin WizardPageWelcome icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end WizardPageWelcome icon retrieval
}

/*!
 * WizardPageSession type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardPageSession, wxWizardPageSimple )

    /*!
     * WizardPageSession event table definition
     */

BEGIN_EVENT_TABLE( WizardPageSession, wxWizardPageSimple )

    ////@begin WizardPageSession event table entries
    EVT_WIZARD_PAGE_CHANGED( -1, WizardPageSession::OnWizardpageSessionPageChanged )
    EVT_WIZARD_PAGE_CHANGING( -1, WizardPageSession::OnWizardpageSessionPageChanging )

    EVT_TEXT( XRCID("ID_TEXTCTRL_SESSION_NAME"), WizardPageSession::OnTextctrlSessionNameUpdated )

    EVT_TEXT( XRCID("ID_TEXTCTRL_SVRNAME"), WizardPageSession::OnTextctrlSvrnameUpdated )

    EVT_TEXT( XRCID("ID_TEXTCTRL_SVRPORT"), WizardPageSession::OnTextctrlSvrportUpdated )

    ////@end WizardPageSession event table entries

END_EVENT_TABLE()

    /*!
     * WizardPageSession constructors
     */

    WizardPageSession::WizardPageSession( )
: m_bKeyTyped(false)
{
}

    WizardPageSession::WizardPageSession( wxWizard* parent )
: m_bKeyTyped(false)
{
    Create( parent );
}

bool WizardPageSession::ConfigExists(wxString &sessionName)
{
    wxString cfgfn;
    wxConfigBase::Get()->Read(_T("Config/UserNxDir"), &cfgfn);
    cfgfn = cfgfn + wxFileName::GetPathSeparator() + _T("config");
    cfgfn = cfgfn + wxFileName::GetPathSeparator() + sessionName + _T(".nxs");
    return wxFileName::FileExists(cfgfn);
}

/*!
 * WizardPageSession creator
 */

bool WizardPageSession::Create( wxWizard* parent )
{
    wxUnusedVar(parent);
    ////@begin WizardPageSession member initialisation
    m_iPort = 22;
    m_iConnectionSpeed = MyXmlConfig::SPEED_ADSL;
    m_pCtrlSessionName = NULL;
    m_pCtrlHostName = NULL;
    m_pCtrlPort = NULL;
    ////@end WizardPageSession member initialisation
    m_sSessionName = _("New Session");
    if (ConfigExists(m_sSessionName)) {
        for (int i = 2;;i++) {
            m_sSessionName = wxString::Format(_("New Session %d"), i);
            if (!ConfigExists(m_sSessionName))
                break;
        }
    }

    ////@begin WizardPageSession creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    CreateControls();
    ////@end WizardPageSession creation
    CreateControls();
    wxDynamicCast(m_pCtrlSessionName->GetValidator(), MyValidator)->SetKeyTyped(wxDynamicCast(GetParent(), MyWizard));
    wxDynamicCast(m_pCtrlHostName->GetValidator(), MyValidator)->SetKeyTyped(wxDynamicCast(GetParent(), MyWizard));
    wxDynamicCast(m_pCtrlPort->GetValidator(), MyValidator)->SetKeyTyped(wxDynamicCast(GetParent(), MyWizard));
    return TRUE;
}

/*!
 * Control creation for WizardPageSession
 */

void WizardPageSession::CreateControls()
{
    ////@begin WizardPageSession content construction
    m_pCtrlSessionName = XRCCTRL(*this, "ID_TEXTCTRL_SESSION_NAME", wxTextCtrl);
    m_pCtrlHostName = XRCCTRL(*this, "ID_TEXTCTRL_SVRNAME", wxTextCtrl);
    m_pCtrlPort = XRCCTRL(*this, "ID_TEXTCTRL_SVRPORT", wxTextCtrl);
    // Set validators
    if (FindWindow(XRCID("ID_TEXTCTRL_SESSION_NAME")))
        FindWindow(XRCID("ID_TEXTCTRL_SESSION_NAME"))->SetValidator( MyValidator(MyValidator::MYVAL_FILENAME, & m_sSessionName) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SVRNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_SVRNAME"))->SetValidator( MyValidator(MyValidator::MYVAL_HOST, & m_sHostName) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SVRPORT")))
        FindWindow(XRCID("ID_TEXTCTRL_SVRPORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iPort) );
    if (FindWindow(XRCID("ID_SLIDER")))
        FindWindow(XRCID("ID_SLIDER"))->SetValidator( wxGenericValidator(& m_iConnectionSpeed) );
    ////@end WizardPageSession content construction

    // Create custom windows not generated automatically here.

    ////@begin WizardPageSession content initialisation
    ////@end WizardPageSession content initialisation
}

/*!
 * Should we show tooltips?
 */

bool WizardPageSession::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardPageSession::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    ////@begin WizardPageSession bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end WizardPageSession bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardPageSession::GetIconResource( const wxString& name )
{
    // Icon retrieval
    ////@begin WizardPageSession icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end WizardPageSession icon retrieval
}

void WizardPageSession::KeyTyped()
{
    m_bKeyTyped = true;
}

void WizardPageSession::CheckNextEnable()
{
    bool enable = true;

    m_bKeyTyped = false;
    if (m_pCtrlSessionName->GetValue().IsEmpty())
        enable = false;
    if (m_pCtrlHostName->GetValue().IsEmpty())
        enable = false;
    if (m_pCtrlPort->GetValue().IsEmpty())
        enable = false;

    if (enable) {
        wxString cfgfn;
        wxConfigBase::Get()->Read(_T("Config/UserNxDir"), &cfgfn);
        cfgfn = cfgfn + wxFileName::GetPathSeparator() + _T("config");
        cfgfn = cfgfn + wxFileName::GetPathSeparator() + m_pCtrlSessionName->GetValue() + _T(".nxs");
        if (wxFileName::FileExists(cfgfn))
            enable = false;
    }
    wxDynamicCast(GetParent(), MyWizard)->EnableNext(enable);
}

#define TIMERID_RDP 1
#define TIMERID_VNC 2

/*!
 * WizardPageDesktop type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardPageDesktop, wxWizardPageSimple )

    /*!
     * WizardPageDesktop event table definition
     */

BEGIN_EVENT_TABLE( WizardPageDesktop, wxWizardPageSimple )

    ////@begin WizardPageDesktop event table entries
    EVT_WIZARD_PAGE_CHANGED( -1, WizardPageDesktop::OnWizardpageDesktopPageChanged )
    EVT_WIZARD_PAGE_CHANGING( -1, WizardPageDesktop::OnWizardpageDesktopPageChanging )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_DPROTO"), WizardPageDesktop::OnComboboxDprotoSelected )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_DTYPE"), WizardPageDesktop::OnComboboxDtypeSelected )

    EVT_BUTTON( XRCID("ID_BUTTON_DSETTINGS"), WizardPageDesktop::OnButtonDsettingsClick )

    EVT_COMBOBOX( XRCID("ID_COMBOBOX_DISPTYPE"), WizardPageDesktop::OnComboboxDisptypeSelected )

    ////@end WizardPageDesktop event table entries

    EVT_TIMER(TIMERID_RDP, WizardPageDesktop::OnRdpDialogTimer)
EVT_TIMER(TIMERID_VNC, WizardPageDesktop::OnVncDialogTimer)

END_EVENT_TABLE()

    /*!
     * WizardPageDesktop constructors
     */

WizardPageDesktop::WizardPageDesktop( )
{
    m_pRdpDialogTimer = new wxTimer(this, TIMERID_RDP);
    m_pVncDialogTimer = new wxTimer(this, TIMERID_VNC);
}

WizardPageDesktop::~WizardPageDesktop( )
{
    delete m_pRdpDialogTimer;
    delete m_pVncDialogTimer;
}

WizardPageDesktop::WizardPageDesktop( wxWizard* parent )
{
    m_pRdpDialogTimer = new wxTimer(this, TIMERID_RDP);
    m_pVncDialogTimer = new wxTimer(this, TIMERID_VNC);
    Create( parent );
}

/*!
 * WizardPageDesktop creator
 */

bool WizardPageDesktop::Create( wxWizard* parent )
{
    wxUnusedVar(parent);
    ////@begin WizardPageDesktop member initialisation
    m_iSessionType = MyXmlConfig::STYPE_UNIX;
    m_iDesktopTypeDialog = MyXmlConfig::DTYPE_KDE;
    m_iDisplayType = MyXmlConfig::DPTYPE_AVAILABLE;
    m_iDisplayWidth = 800;
    m_iDisplayHeight = 600;
    m_iPseudoDesktopTypeIndex = -1;
    m_iPseudoDisplayTypeIndex = -1;
    m_pCtrlDesktopType = NULL;
    m_pCtrlDesktopSettings = NULL;
    m_pCtrlDisplayType = NULL;
    m_pCtrlDisplayWidth = NULL;
    m_pCtrlDisplayHeight = NULL;
    ////@end WizardPageDesktop member initialisation
    m_iUnixDesktopType = m_iDesktopTypeDialog;

    ////@begin WizardPageDesktop creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    CreateControls();
    ////@end WizardPageDesktop creation
    CreateControls();
    UpdateDialogConstraints(false);
    m_pCtrlDisplayWidth->SetFont(wxSystemSettings::GetFont(wxSYS_ANSI_VAR_FONT));
    m_pCtrlDisplayHeight->SetFont(wxSystemSettings::GetFont(wxSYS_ANSI_VAR_FONT));

    return TRUE;
}

/*!
 * Control creation for WizardPageDesktop
 */

void WizardPageDesktop::CreateControls()
{
    ////@begin WizardPageDesktop content construction
    m_pCtrlDesktopType = XRCCTRL(*this, "ID_COMBOBOX_DTYPE", wxComboBox);
    m_pCtrlDesktopSettings = XRCCTRL(*this, "ID_BUTTON_DSETTINGS", wxButton);
    m_pCtrlDisplayType = XRCCTRL(*this, "ID_COMBOBOX_DISPTYPE", wxComboBox);
    m_pCtrlDisplayWidth = XRCCTRL(*this, "ID_SPINCTRL_WIDTH", wxSpinCtrl);
    m_pCtrlDisplayHeight = XRCCTRL(*this, "ID_SPINCTRL_HEIGHT", wxSpinCtrl);
    // Set validators
    if (FindWindow(XRCID("ID_COMBOBOX_DPROTO")))
        FindWindow(XRCID("ID_COMBOBOX_DPROTO"))->SetValidator( wxGenericValidator(& m_iSessionType) );
    if (FindWindow(XRCID("ID_COMBOBOX_DTYPE")))
        FindWindow(XRCID("ID_COMBOBOX_DTYPE"))->SetValidator( wxGenericValidator(& m_iDesktopTypeDialog) );
    if (FindWindow(XRCID("ID_COMBOBOX_DISPTYPE")))
        FindWindow(XRCID("ID_COMBOBOX_DISPTYPE"))->SetValidator( wxGenericValidator(& m_iDisplayType) );
    if (FindWindow(XRCID("ID_SPINCTRL_WIDTH")))
        FindWindow(XRCID("ID_SPINCTRL_WIDTH"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iDisplayWidth) );
    if (FindWindow(XRCID("ID_SPINCTRL_HEIGHT")))
        FindWindow(XRCID("ID_SPINCTRL_HEIGHT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iDisplayHeight) );
    ////@end WizardPageDesktop content construction

    // Create custom windows not generated automatically here.
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

    ////@begin WizardPageDesktop content initialisation
    ////@end WizardPageDesktop content initialisation
}

void WizardPageDesktop::UpdateDialogConstraints(bool getValues)
{
    if (getValues)
        TransferDataFromWindow();

    // 'General' tab
    switch (m_iSessionType) {
        case MyXmlConfig::STYPE_UNIX:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
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
            break;
        case MyXmlConfig::STYPE_WINDOWS:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
            }
            m_iPseudoDesktopTypeIndex = m_pCtrlDesktopType->Append(_("RDP"), (void *)MyXmlConfig::DTYPE_RDP);
            m_pCtrlDesktopType->SetSelection(m_iPseudoDesktopTypeIndex);
            m_iDesktopType = MyXmlConfig::DTYPE_RDP;
            m_iDesktopTypeDialog = 0;
            m_pCtrlDesktopType->Enable(false);
            m_pCtrlDesktopSettings->Enable(true);
            if (m_iPseudoDisplayTypeIndex != -1) {
                m_pCtrlDisplayType->Delete(m_iPseudoDisplayTypeIndex);
                if (m_iDisplayType >= m_iPseudoDisplayTypeIndex)
                    m_pCtrlDisplayType->SetSelection(3);
                m_iPseudoDisplayTypeIndex = -1;
            }
            break;
        case MyXmlConfig::STYPE_VNC:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
            }
            m_iPseudoDesktopTypeIndex = m_pCtrlDesktopType->Append(_("RFB"), (void *)MyXmlConfig::DTYPE_RFB);
            m_pCtrlDesktopType->SetSelection(m_iPseudoDesktopTypeIndex);
            m_iDesktopType = MyXmlConfig::DTYPE_RFB;
            m_iDesktopTypeDialog = 0;
            m_pCtrlDesktopType->Enable(false);
            m_pCtrlDesktopSettings->Enable(true);
            if (m_iPseudoDisplayTypeIndex != -1) {
                m_pCtrlDisplayType->Delete(m_iPseudoDisplayTypeIndex);
                if (m_iDisplayType >= m_iPseudoDisplayTypeIndex)
                    m_pCtrlDisplayType->SetSelection(3);
                m_iPseudoDisplayTypeIndex = -1;
            }
            break;
        case MyXmlConfig::STYPE_SHADOW:
            if (m_iPseudoDesktopTypeIndex != -1) {
                m_pCtrlDesktopType->Delete(m_iPseudoDesktopTypeIndex);
                m_iPseudoDesktopTypeIndex = -1;
            }
            m_iPseudoDesktopTypeIndex = m_pCtrlDesktopType->Append(_("Any"), (void *)MyXmlConfig::DTYPE_ANY);
            m_pCtrlDesktopType->SetSelection(m_iPseudoDesktopTypeIndex);
            m_iDesktopType = MyXmlConfig::DTYPE_ANY;
            m_iDesktopTypeDialog = 0;
            m_pCtrlDesktopType->Enable(false);
            m_pCtrlDesktopSettings->Enable(false);
            if (m_iPseudoDisplayTypeIndex == -1) {
                m_iPseudoDisplayTypeIndex = m_pCtrlDisplayType->Append(_("As on server"), (void *)MyXmlConfig::DPTYPE_REMOTE);
                m_pCtrlDisplayType->SetSelection(m_iPseudoDisplayTypeIndex);
            }
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
}

/*!
 * Should we show tooltips?
 */

bool WizardPageDesktop::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardPageDesktop::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    ////@begin WizardPageDesktop bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end WizardPageDesktop bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardPageDesktop::GetIconResource( const wxString& name )
{
    // Icon retrieval
    ////@begin WizardPageDesktop icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end WizardPageDesktop icon retrieval
}

void WizardPageDesktop::ShowRdpPropertyDialog()
{
    RdpPropertyDialog d;
    d.SetConfig(wxDynamicCast(GetParent(), MyWizard)->pGetConfig());
    d.Create(this);
    d.ShowModal();
    CheckNextEnable();
}

void WizardPageDesktop::ShowVncPropertyDialog()
{
    VncPropertyDialog d;
    d.SetConfig(wxDynamicCast(GetParent(), MyWizard)->pGetConfig());
    d.Create(this);
    d.ShowModal();
    CheckNextEnable();
}

void WizardPageDesktop::OnRdpDialogTimer(wxTimerEvent &)
{
    ShowRdpPropertyDialog();
}

void WizardPageDesktop::OnVncDialogTimer(wxTimerEvent &)
{
    ShowVncPropertyDialog();
}

void WizardPageDesktop::CheckNextEnable()
{
    bool enable = true;
    MyXmlConfig *cfg = wxDynamicCast(GetParent(), MyWizard)->pGetConfig();
    switch (m_iSessionType) {
        case MyXmlConfig::STYPE_UNIX:
            if (m_iDesktopTypeDialog == MyXmlConfig::DTYPE_CUSTOM) {
                if (cfg->sGetCommandLine().IsEmpty() && (!cfg->bGetRunConsole()) && (!cfg->bGetRunXclients()))
                    enable = false;
            }
            break;
        case MyXmlConfig::STYPE_WINDOWS:
            if (cfg->sGetRdpHostName().IsEmpty())
                enable = false;
            break;
        case MyXmlConfig::STYPE_VNC:
            if (cfg->sGetVncHostName().IsEmpty())
                enable = false;
            break;
    }
    wxDynamicCast(GetParent(), MyWizard)->EnableNext(enable);
}

/*!
 * WizardPageSecurity type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardPageSecurity, wxWizardPageSimple )

    /*!
     * WizardPageSecurity event table definition
     */

BEGIN_EVENT_TABLE( WizardPageSecurity, wxWizardPageSimple )

    ////@begin WizardPageSecurity event table entries
    EVT_WIZARD_PAGE_CHANGED( -1, WizardPageSecurity::OnWizardpageSecurityPageChanged )
    EVT_WIZARD_PAGE_CHANGING( -1, WizardPageSecurity::OnWizardpageSecurityPageChanging )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_SCARD"), WizardPageSecurity::OnCheckboxScardClick )

    ////@end WizardPageSecurity event table entries

END_EVENT_TABLE()

    /*!
     * WizardPageSecurity constructors
     */

WizardPageSecurity::WizardPageSecurity( )
{
}

WizardPageSecurity::WizardPageSecurity( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardPage creator
 */

bool WizardPageSecurity::Create( wxWizard* parent )
{
    wxUnusedVar(parent);
    ////@begin WizardPageSecurity member initialisation
    m_bUseSmartCard = false;
    m_bEnableSSL = true;
    m_pCtrlUseSmartCard = NULL;
    m_pCtrlEnableSSL = NULL;
    ////@end WizardPageSecurity member initialisation
    m_bUseSmartCard = ::wxGetApp().NxSmartCardSupport();

    ////@begin WizardPageSecurity creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    CreateControls();
    ////@end WizardPageSecurity creation
    CreateControls();
    return TRUE;
}

/*!
 * Control creation for WizardPage
 */

void WizardPageSecurity::CreateControls()
{
    ////@begin WizardPageSecurity content construction
    m_pCtrlUseSmartCard = XRCCTRL(*this, "ID_CHECKBOX_SCARD", wxCheckBox);
    m_pCtrlEnableSSL = XRCCTRL(*this, "ID_CHECKBOX_SSLENABLE", wxCheckBox);
    // Set validators
    if (FindWindow(XRCID("ID_CHECKBOX_SCARD")))
        FindWindow(XRCID("ID_CHECKBOX_SCARD"))->SetValidator( wxGenericValidator(& m_bUseSmartCard) );
    if (FindWindow(XRCID("ID_CHECKBOX_SSLENABLE")))
        FindWindow(XRCID("ID_CHECKBOX_SSLENABLE"))->SetValidator( wxGenericValidator(& m_bEnableSSL) );
    ////@end WizardPageSecurity content construction

    // Create custom windows not generated automatically here.

    ////@begin WizardPageSecurity content initialisation
    ////@end WizardPageSecurity content initialisation
    m_pCtrlUseSmartCard->Enable(::wxGetApp().NxSmartCardSupport());
    if (m_bUseSmartCard || (!::wxGetApp().NxProxyAvailable())) {
        m_pCtrlEnableSSL->SetValue(true);
        m_pCtrlEnableSSL->Enable(false);
    }
}

/*!
 * Should we show tooltips?
 */

bool WizardPageSecurity::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardPageSecurity::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    ////@begin WizardPageSecurity bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end WizardPageSecurity bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardPageSecurity::GetIconResource( const wxString& name )
{
    // Icon retrieval
    ////@begin WizardPageSecurity icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end WizardPageSecurity icon retrieval
}

/*!
 * WizardPageFinish type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardPageFinish, wxWizardPageSimple )

    /*!
     * WizardPageFinish event table definition
     */

BEGIN_EVENT_TABLE( WizardPageFinish, wxWizardPageSimple )

    ////@begin WizardPageFinish event table entries
    EVT_WIZARD_PAGE_CHANGED( -1, WizardPageFinish::OnWizardpageFinishPageChanged )
    EVT_WIZARD_PAGE_CHANGING( -1, WizardPageFinish::OnWizardpageFinishPageChanging )

    ////@end WizardPageFinish event table entries

END_EVENT_TABLE()

    /*!
     * WizardPageFinish constructors
     */

WizardPageFinish::WizardPageFinish( )
{
}

WizardPageFinish::WizardPageFinish( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardPageFinish creator
 */

bool WizardPageFinish::Create( wxWizard* parent )
{
    wxUnusedVar(parent);
    ////@begin WizardPageFinish member initialisation
    m_bCreateShortcut = true;
    m_bShowAdvancedConfig = false;
    m_pCtrlHeader = NULL;
    ////@end WizardPageFinish member initialisation

    ////@begin WizardPageFinish creation
    CreateControls();
    ////@end WizardPageFinish creation
    CreateControls();
    return TRUE;
}

/*!
 * Control creation for WizardPageFinish
 */

void WizardPageFinish::CreateControls()
{
    ////@begin WizardPageFinish content construction
    m_pCtrlHeader = XRCCTRL(*this, "wxID_STATIC", WrappedStaticText);
    // Set validators
    if (FindWindow(XRCID("ID_CHECKBOX_SHORTCUT")))
        FindWindow(XRCID("ID_CHECKBOX_SHORTCUT"))->SetValidator( wxGenericValidator(& m_bCreateShortcut) );
    if (FindWindow(XRCID("ID_CHECKBOX_ADVANCED")))
        FindWindow(XRCID("ID_CHECKBOX_ADVANCED"))->SetValidator( wxGenericValidator(& m_bShowAdvancedConfig) );
    ////@end WizardPageFinish content construction

    // Create custom windows not generated automatically here.

    ////@begin WizardPageFinish content initialisation
    ////@end WizardPageFinish content initialisation
}

/*!
 * Should we show tooltips?
 */

bool WizardPageFinish::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap WizardPageFinish::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    ////@begin WizardPageFinish bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end WizardPageFinish bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WizardPageFinish::GetIconResource( const wxString& name )
{
    // Icon retrieval
    ////@begin WizardPageFinish icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end WizardPageFinish icon retrieval
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DPROTO
 */

void WizardPageDesktop::OnComboboxDprotoSelected( wxCommandEvent& event )
{
    myLogTrace(MYTRACETAG, _T("WizardPageDesktop::OnComboboxDprotoSelected"));
    if (event.GetInt() == 0) {
        m_iDesktopTypeDialog = m_iUnixDesktopType;
        m_pCtrlDesktopType->SetSelection(m_iDesktopTypeDialog);
    }
    UpdateDialogConstraints(true);
    MyXmlConfig *cfg = wxDynamicCast(GetParent(), MyWizard)->pGetConfig();
    CheckNextEnable();
    switch (m_iSessionType) {
        case MyXmlConfig::STYPE_WINDOWS:
            if (cfg->sGetRdpHostName().IsEmpty())
                m_pRdpDialogTimer->Start(200, true);
            break;
        case MyXmlConfig::STYPE_VNC:
            if (cfg->sGetVncHostName().IsEmpty())
                m_pVncDialogTimer->Start(200, true);
            break;
    }
    event.Skip();
}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DTYPE
 */

void WizardPageDesktop::OnComboboxDtypeSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints(true);
    m_iUnixDesktopType = m_iDesktopTypeDialog;
    ::myLogTrace(MYTRACETAG, wxT("dtype=%d"), m_iDesktopTypeDialog);
    CheckNextEnable();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DISPTYPE
 */

void WizardPageDesktop::OnComboboxDisptypeSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints(true);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SCARD
 */

void WizardPageSecurity::OnCheckboxScardClick( wxCommandEvent& event )
{
    if (event.IsChecked() || (!::wxGetApp().NxProxyAvailable())) {
        m_pCtrlEnableSSL->SetValue(true);
        m_pCtrlEnableSSL->Enable(false);
    } else
        m_pCtrlEnableSSL->Enable(true);
    event.Skip();
}

/*!
 * wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_WELCOME
 */

void WizardPageWelcome::OnWizardpageWelcomePageChanged( wxWizardEvent& event )
{
    wxDynamicCast(GetParent(), MyWizard)->EnableNext(true);
    event.Skip();
}

/*!
 * wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_SESSION
 */

void WizardPageSession::OnWizardpageSessionPageChanged( wxWizardEvent& event )
{
    if (event.GetDirection())
        wxDynamicCast(GetParent(), MyWizard)->EnableNext(false);
    event.Skip();
}

/*!
 * wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_DESKTOP
 */

void WizardPageDesktop::OnWizardpageDesktopPageChanged( wxWizardEvent& event )
{
    wxDynamicCast(GetParent(), MyWizard)->EnableNext(true);
    event.Skip();
}


/*!
 * wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_SECURITY
 */

void WizardPageSecurity::OnWizardpageSecurityPageChanged( wxWizardEvent& event )
{
    wxDynamicCast(GetParent(), MyWizard)->EnableNext(true);
    event.Skip();
}


/*!
 * wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_FINISH
 */

void WizardPageFinish::OnWizardpageFinishPageChanged( wxWizardEvent& event )
{
    wxDynamicCast(GetParent(), MyWizard)->EnableNext(true);
    MyXmlConfig *cfg = wxDynamicCast(GetParent(), MyWizard)->pGetConfig();
    wxString cfgName = cfg->sGetName();
    //cfgName.Replace(_T(" "), _T("&nbsp;"));
    m_pCtrlHeader->SetLabel(wxString::Format(
                _("Congratulations. The connection to '%s' will be saved as '%s'. You may further configure your session by running the Advanced Configuration dialog."),
                cfg->sGetServerHost().c_str(), cfgName.c_str()));
    m_pCtrlHeader->Wrap(9999);
    Layout();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SESSION_NAME
 */

void WizardPageSession::OnTextctrlSessionNameUpdated( wxCommandEvent& event )
{
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlSessionName))
        CheckNextEnable();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SVRNAME
 */

void WizardPageSession::OnTextctrlSvrnameUpdated( wxCommandEvent& event )
{
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlHostName))
        CheckNextEnable();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SVRPORT
 */

void WizardPageSession::OnTextctrlSvrportUpdated( wxCommandEvent& event )
{
    if (m_bKeyTyped && (wxWindow::FindFocus() == (wxWindow *)m_pCtrlPort))
        CheckNextEnable();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_DSETTINGS
 */

void WizardPageDesktop::OnButtonDsettingsClick( wxCommandEvent& event )
{
    switch (m_iSessionType) {
        case MyXmlConfig::STYPE_UNIX:
            if (MyXmlConfig::DTYPE_XDM == m_iUnixDesktopType) {
                XdmPropertyDialog d;
                d.SetConfig(wxDynamicCast(GetParent(), MyWizard)->pGetConfig());
                d.Create(this);
                d.ShowModal();
                CheckNextEnable();
            } else {
                X11PropertyDialog d;
                d.SetConfig(wxDynamicCast(GetParent(), MyWizard)->pGetConfig());
                d.Create(this);
                d.ShowModal();
                CheckNextEnable();
            }
            break;
        case MyXmlConfig::STYPE_WINDOWS:
            ShowRdpPropertyDialog();
            break;
        case MyXmlConfig::STYPE_VNC:
            ShowVncPropertyDialog();
            break;
    }
    event.Skip();
}

/*!
 * wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_SESSION
 */

void WizardPageSession::OnWizardpageSessionPageChanging( wxWizardEvent& event )
{
    if (event.GetDirection()) {
        MyXmlConfig *cfg = wxDynamicCast(GetParent(), MyWizard)->pGetConfig();
        TransferDataFromWindow();
        cfg->sSetName(m_sSessionName);
        cfg->sSetServerHost(m_sHostName);
        cfg->iSetServerPort(m_iPort);
        cfg->eSetConnectionSpeed(wx_static_cast(MyXmlConfig::ConnectionSpeed, m_iConnectionSpeed));
        wxString cfgfn;
        wxConfigBase::Get()->Read(_T("Config/UserNxDir"), &cfgfn);
        cfgfn = cfgfn + wxFileName::GetPathSeparator() + _T("config");
        cfgfn = cfgfn + wxFileName::GetPathSeparator() + m_pCtrlSessionName->GetValue() + _T(".nxs");
        cfg->sSetFileName(cfgfn);
    }
    event.Skip();
}


/*!
 * wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_DESKTOP
 */

void WizardPageDesktop::OnWizardpageDesktopPageChanging( wxWizardEvent& event )
{
    if (event.GetDirection()) {
        MyXmlConfig *cfg = wxDynamicCast(GetParent(), MyWizard)->pGetConfig();
        TransferDataFromWindow();
        cfg->eSetSessionType(wx_static_cast(MyXmlConfig::SessionType, m_iSessionType));
        switch (cfg->eGetSessionType()) {
            case MyXmlConfig::STYPE_WINDOWS:
                cfg->eSetDesktopType(MyXmlConfig::DTYPE_RDP);
                break;
            case MyXmlConfig::STYPE_VNC:
                cfg->eSetDesktopType(MyXmlConfig::DTYPE_RFB);
                break;
            case MyXmlConfig::STYPE_SHADOW:
                cfg->eSetDesktopType(MyXmlConfig::DTYPE_ANY);
                break;
            default:
                cfg->eSetDesktopType(wx_static_cast(MyXmlConfig::DesktopType, m_iDesktopTypeDialog));
                break;
        }
        cfg->eSetDisplayType(wx_static_cast(MyXmlConfig::DisplayType, m_iDisplayType));
        cfg->iSetDisplayWidth(m_iDisplayWidth);
        cfg->iSetDisplayHeight(m_iDisplayHeight);
    }
    event.Skip();
}


/*!
 * wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_SECURITY
 */

void WizardPageSecurity::OnWizardpageSecurityPageChanging( wxWizardEvent& event )
{
    if (event.GetDirection()) {
        MyXmlConfig *cfg = wxDynamicCast(GetParent(), MyWizard)->pGetConfig();
        TransferDataFromWindow();
        cfg->bSetEnableSSL(m_bEnableSSL);
        cfg->bSetUseSmartCard(m_bUseSmartCard);
    }
    event.Skip();
}


/*!
 * wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_FINISH
 */

void WizardPageFinish::OnWizardpageFinishPageChanging( wxWizardEvent& event )
{
    if (event.GetDirection()) {
        MyXmlConfig *cfg = wxDynamicCast(GetParent(), MyWizard)->pGetConfig();
        ::myLogTrace(MYTRACETAG, _T("MyWizard: creating new config %s"), cfg->sGetFileName().c_str());
        cfg->SaveToFile();
        TransferDataFromWindow();
        if (m_bShowAdvancedConfig) {
            SessionProperties d;
            d.SetConfig(cfg);
            wxString fn = cfg->sGetFileName();
            d.Create(this);
            switch (d.ShowModal()) {
                case wxID_CANCEL:
                    break;
                case wxID_CLEAR:
                    ::myLogTrace(MYTRACETAG, _T("deleting '%s'"), fn.c_str());
                    ::wxRemoveFile(fn);
                    // actually we were not cancelled but we want to behave
                    // as if we were, because the just created session has
                    // been deleted.
                    wxDynamicCast(GetParent(), MyWizard)->SetCancelled();
                    m_bCreateShortcut = false;
                    break;
                case wxID_OK:
                    if (!cfg->SaveToFile())
                        wxMessageBox(wxString::Format(_("Could not save session to\n%s"),
                                    fn.c_str()), _("Error saving - OpenNX"), wxICON_ERROR | wxOK);
                    wxConfigBase::Get()->Write(wxT("Config/UserNxDir"), d.GetsUserNxDir());
                    wxConfigBase::Get()->Write(wxT("Config/SystemNxDir"), d.GetsSystemNxDir());
#ifdef SUPPORT_USBIP
                    wxConfigBase::Get()->Write(wxT("Config/UsbipdSocket"), d.GetUsbipdSocket());
                    wxConfigBase::Get()->Write(wxT("Config/UsbipPort"), d.GetUsbLocalPort());
#endif
                    break;
            }
        }
        if (m_bCreateShortcut)
            ::wxGetApp().CreateDesktopEntry(cfg);
    }
    event.Skip();
}



