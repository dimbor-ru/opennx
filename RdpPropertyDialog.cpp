// $Id: RdpPropertyDialog.cpp 590 2010-10-23 22:42:11Z felfert $
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
#pragma implementation "RdpPropertyDialog.h"
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

#include "RdpPropertyDialog.h"
#include "Icon.h"
#include "MyXmlConfig.h"
#include "MyValidator.h"
#include "opennxApp.h"

#include <wx/config.h>
#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images

/*!
 * RdpPropertyDialog type definition
 */

IMPLEMENT_CLASS( RdpPropertyDialog, wxDialog )

/*!
 * RdpPropertyDialog event table definition
 */

BEGIN_EVENT_TABLE( RdpPropertyDialog, wxDialog )

////@begin RdpPropertyDialog event table entries
    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_WINLOGON"), RdpPropertyDialog::OnRadiobuttonRdpWinlogonSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_NXAUTH"), RdpPropertyDialog::OnRadiobuttonRdpNxauthSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_AUTOLOGIN"), RdpPropertyDialog::OnRadiobuttonRdpAutologinSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_DESKTOP"), RdpPropertyDialog::OnRadiobuttonRdpDesktopSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_RUNAPP"), RdpPropertyDialog::OnRadiobuttonRdpRunappSelected )

    EVT_BUTTON( wxID_OK, RdpPropertyDialog::OnOkClick )

////@end RdpPropertyDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, RdpPropertyDialog::OnContextHelp)

END_EVENT_TABLE()

/*!
 * RdpPropertyDialog constructors
 */

RdpPropertyDialog::RdpPropertyDialog( )
    : m_pCfg(NULL)
{
}

RdpPropertyDialog::RdpPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_pCfg(NULL)
{
    Create(parent, id, caption, pos, size, style);
}

void RdpPropertyDialog::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

/*!
 * RdpPropertyDialog creator
 */

bool RdpPropertyDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin RdpPropertyDialog member initialisation
    m_bAutoLogin = false;
    m_bShowWinLogon = false;
    m_bUseNxAuth = false;
    m_pCtrlAutologin = NULL;
    m_pCtrlUsername = NULL;
    m_pCtrlPassword = NULL;
    m_pCtrlRememberPassword = NULL;
    m_pCtrlRootless = NULL;
    m_pCtrlRunApplication = NULL;
    m_pCtrlApplicationString = NULL;
////@end RdpPropertyDialog member initialisation
    wxASSERT_MSG(m_pCfg, _T("RdpPropertyDialog::Create: No configuration"));
    if (m_pCfg) {
        m_bRememberPassword = m_pCfg->bGetRdpRememberPassword();
        m_bRootless = m_pCfg->bGetRdpRootless();
        m_bRunApplication = m_pCfg->bGetRdpRunApplication();
        m_bRunDesktop = !m_bRunApplication;
        m_sHostname = m_pCfg->sGetRdpHostName();
        m_sUsername = m_pCfg->sGetRdpUsername();
        m_sPassword = m_pCfg->sGetRdpPassword();
        m_sRunCommand = m_pCfg->sGetRdpApplication();
        m_sRdpDomain = m_pCfg->sGetRdpDomain();
        int atype = m_pCfg->iGetRdpAuthType();
        switch (atype) {
            case 0:
                m_bAutoLogin = true;
                break;
            case 1:
                m_bShowWinLogon = true;
                break;
            case 2:
                m_bUseNxAuth = true;
                break;
        }

    }
    wxConfigBase::Get()->Read(wxT("Config/StorePasswords"), &m_bStorePasswords, true);
    if (!m_bStorePasswords)
        m_bRememberPassword = false;

////@begin RdpPropertyDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end RdpPropertyDialog creation
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

/*!
 * Control creation for RdpPropertyDialog
 */

void RdpPropertyDialog::CreateControls()
{    
////@begin RdpPropertyDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_SETTINGS_RDP")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlAutologin = XRCCTRL(*this, "ID_RADIOBUTTON_RDP_AUTOLOGIN", wxRadioButton);
    m_pCtrlUsername = XRCCTRL(*this, "ID_TEXTCTRL_RDP_USERNAME", wxTextCtrl);
    m_pCtrlPassword = XRCCTRL(*this, "ID_TEXTCTRL_RDP_PASSWORD", wxTextCtrl);
    m_pCtrlRememberPassword = XRCCTRL(*this, "ID_CHECKBOX_RDP_REMEMBER_PWD", wxCheckBox);
    m_pCtrlRootless = XRCCTRL(*this, "ID_CHECKBOX_RDP_ROOTLESS", wxCheckBox);
    m_pCtrlRunApplication = XRCCTRL(*this, "ID_RADIOBUTTON_RDP_RUNAPP", wxRadioButton);
    m_pCtrlApplicationString = XRCCTRL(*this, "ID_TEXTCTRL_RDP_APPLICATION", wxTextCtrl);
    // Set validators
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_HOST")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_HOST"))->SetValidator( wxGenericValidator(& m_sHostname) );
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_DOMAIN")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_DOMAIN"))->SetValidator( wxTextValidator(wxFILTER_NONE, & m_sRdpDomain) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_WINLOGON")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_WINLOGON"))->SetValidator( wxGenericValidator(& m_bShowWinLogon) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_NXAUTH")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_NXAUTH"))->SetValidator( wxGenericValidator(& m_bUseNxAuth) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_AUTOLOGIN")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_AUTOLOGIN"))->SetValidator( wxGenericValidator(& m_bAutoLogin) );
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_USERNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_USERNAME"))->SetValidator( wxGenericValidator(& m_sUsername) );
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_PASSWORD")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_PASSWORD"))->SetValidator( wxGenericValidator(& m_sPassword) );
    if (FindWindow(XRCID("ID_CHECKBOX_RDP_REMEMBER_PWD")))
        FindWindow(XRCID("ID_CHECKBOX_RDP_REMEMBER_PWD"))->SetValidator( wxGenericValidator(& m_bRememberPassword) );
    if (FindWindow(XRCID("ID_CHECKBOX_RDP_ROOTLESS")))
        FindWindow(XRCID("ID_CHECKBOX_RDP_ROOTLESS"))->SetValidator( wxGenericValidator(& m_bRootless) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_DESKTOP")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_DESKTOP"))->SetValidator( wxGenericValidator(& m_bRunDesktop) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_RUNAPP")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_RUNAPP"))->SetValidator( wxGenericValidator(& m_bRunApplication) );
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_APPLICATION")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_APPLICATION"))->SetValidator( wxGenericValidator(& m_sRunCommand) );
////@end RdpPropertyDialog content construction

    if (!m_bStorePasswords)
        m_pCtrlRememberPassword->Enable(false);

////@begin RdpPropertyDialog content initialisation
////@end RdpPropertyDialog content initialisation
    UpdateDialogConstraints();
}

/*!
 * Should we show tooltips?
 */

bool RdpPropertyDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap RdpPropertyDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon RdpPropertyDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void RdpPropertyDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_AUTOLOGIN
 */

void RdpPropertyDialog::OnRadiobuttonRdpAutologinSelected( wxCommandEvent& event )
{
    m_bAutoLogin = true;
    UpdateDialogConstraints();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_NXAUTH
 */

void RdpPropertyDialog::OnRadiobuttonRdpNxauthSelected( wxCommandEvent& event )
{
    m_bAutoLogin = false;
    UpdateDialogConstraints();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_WINLOGON
 */

void RdpPropertyDialog::OnRadiobuttonRdpWinlogonSelected( wxCommandEvent& event )
{
    m_bAutoLogin = false;
    UpdateDialogConstraints();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_DESKTOP
 */

void RdpPropertyDialog::OnRadiobuttonRdpDesktopSelected( wxCommandEvent& event )
{
    m_bRunApplication = false;
    UpdateDialogConstraints();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_RUNAPP
 */

void RdpPropertyDialog::OnRadiobuttonRdpRunappSelected( wxCommandEvent& event )
{
    m_bRunApplication = true;
    UpdateDialogConstraints();
    event.Skip();
}

void RdpPropertyDialog::UpdateDialogConstraints()
{
    m_pCtrlUsername->Enable(m_bAutoLogin);
    m_pCtrlPassword->Enable(m_bAutoLogin);
    m_pCtrlRememberPassword->Enable(m_bStorePasswords && m_bAutoLogin);
    m_pCtrlApplicationString->Enable(m_bRunApplication);
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void RdpPropertyDialog::OnOkClick( wxCommandEvent& event )
{
    wxASSERT_MSG(m_pCfg, _T("RdpPropertyDialog::OnOkClick: No configuration"));
    if (m_pCfg) {
        TransferDataFromWindow();
        m_pCfg->bSetRdpRememberPassword(m_bRememberPassword);
        m_pCfg->bSetRdpRootless(m_bRootless);
        m_pCfg->bSetRdpRunApplication(m_bRunApplication);
        m_pCfg->sSetRdpHostName(m_sHostname);
        m_pCfg->sSetRdpUsername(m_sUsername);
        m_pCfg->sSetRdpPassword(m_sPassword);
        m_pCfg->sSetRdpApplication(m_sRunCommand);
        m_pCfg->sSetRdpDomain(m_sRdpDomain);
        int atype = 2;
        if (m_bShowWinLogon)
            atype = 1;
        if (m_bAutoLogin)
            atype = 0;
        m_pCfg->iSetRdpAuthType(atype);
    }
    event.Skip();
}

#ifdef __WXMSW__
/* dimbor: on wxWindows 3.0.2 we need to make a sunset by hand
   for some reason */
bool RdpPropertyDialog::TransferDataToWindow()
{
    wxDialog::TransferDataToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_HOST")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_HOST"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_DOMAIN")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_DOMAIN"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_WINLOGON")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_WINLOGON"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_NXAUTH")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_NXAUTH"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_AUTOLOGIN")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_AUTOLOGIN"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_USERNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_USERNAME"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_PASSWORD")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_PASSWORD"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_RDP_REMEMBER_PWD")))
        FindWindow(XRCID("ID_CHECKBOX_RDP_REMEMBER_PWD"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_RDP_ROOTLESS")))
        FindWindow(XRCID("ID_CHECKBOX_RDP_ROOTLESS"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_DESKTOP")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_DESKTOP"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_RUNAPP")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_RUNAPP"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_APPLICATION")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_APPLICATION"))->GetValidator()->TransferToWindow();
    return true;
}

bool RdpPropertyDialog::TransferDataFromWindow()
{
    wxDialog::TransferDataFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_HOST")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_HOST"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_DOMAIN")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_DOMAIN"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_WINLOGON")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_WINLOGON"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_NXAUTH")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_NXAUTH"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_AUTOLOGIN")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_AUTOLOGIN"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_USERNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_USERNAME"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_PASSWORD")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_PASSWORD"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_RDP_REMEMBER_PWD")))
        FindWindow(XRCID("ID_CHECKBOX_RDP_REMEMBER_PWD"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_RDP_ROOTLESS")))
        FindWindow(XRCID("ID_CHECKBOX_RDP_ROOTLESS"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_DESKTOP")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_DESKTOP"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_RUNAPP")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_RUNAPP"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_RDP_APPLICATION")))
        FindWindow(XRCID("ID_TEXTCTRL_RDP_APPLICATION"))->GetValidator()->TransferFromWindow();
    return true;
}
#endif
