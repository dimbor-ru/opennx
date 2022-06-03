// $Id: VncPropertyDialog.cpp 590 2010-10-23 22:42:11Z felfert $
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
#pragma implementation "VncPropertyDialog.h"
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "VncPropertyDialog.h"
#include "Icon.h"
#include "MyXmlConfig.h"
#include "MyValidator.h"
#include "opennxApp.h"

#include <wx/config.h>
#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images

/*!
 * VncPropertyDialog type definition
 */

IMPLEMENT_CLASS( VncPropertyDialog, wxDialog )

/*!
 * VncPropertyDialog event table definition
 */

BEGIN_EVENT_TABLE( VncPropertyDialog, wxDialog )

////@begin VncPropertyDialog event table entries
    EVT_BUTTON( wxID_OK, VncPropertyDialog::OnOkClick )

////@end VncPropertyDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, VncPropertyDialog::OnContextHelp)

END_EVENT_TABLE()

/*!
 * VncPropertyDialog constructors
 */

VncPropertyDialog::VncPropertyDialog( )
    : m_pCfg(NULL)
{
}

VncPropertyDialog::VncPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_pCfg(NULL)
{
    Create(parent, id, caption, pos, size, style);
}

void VncPropertyDialog::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

/*!
 * VncPropertyDialog creator
 */

bool VncPropertyDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin VncPropertyDialog member initialisation
    m_pCtrlPassword = NULL;
    m_pCtrlRememberPassword = NULL;
////@end VncPropertyDialog member initialisation

    wxASSERT_MSG(m_pCfg, _T("VncPropertyDialog::Create: No configuration"));
    if (m_pCfg) {
        m_bRememberPassword = m_pCfg->bGetVncRememberPassword();
        m_bRootless = m_pCfg->bGetVncRootless();
        m_iDisplayNumber = m_pCfg->iGetVncDisplayNumber();
        m_sHostname = m_pCfg->sGetVncHostName();
        m_sPassword = m_pCfg->sGetVncPassword();
    }
    wxConfigBase::Get()->Read(wxT("Config/StorePasswords"), &m_bStorePasswords, true);
    if (!m_bStorePasswords)
        m_bRememberPassword = false;

////@begin VncPropertyDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end VncPropertyDialog creation
    
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

/*!
 * Control creation for VncPropertyDialog
 */

void VncPropertyDialog::CreateControls()
{    
////@begin VncPropertyDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_SETTINGS_VNC")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlPassword = XRCCTRL(*this, "ID_TEXTCTRL_VNC_PASSWD", wxTextCtrl);
    m_pCtrlRememberPassword = XRCCTRL(*this, "ID_CHECKBOX_VNC_REMEMBER_PWD", wxCheckBox);
    m_pCtrlRootless = XRCCTRL(*this, "ID_CHECKBOX_VNC_ROOTLESS", wxCheckBox);
    // Set validators
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_HOST")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_HOST"))->SetValidator( wxTextValidator(wxFILTER_NONE, & m_sHostname) );
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_DPY")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_DPY"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iDisplayNumber) );
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_PASSWD")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_PASSWD"))->SetValidator( wxGenericValidator(& m_sPassword) );
    if (FindWindow(XRCID("ID_CHECKBOX_VNC_REMEMBER_PWD")))
        FindWindow(XRCID("ID_CHECKBOX_VNC_REMEMBER_PWD"))->SetValidator( wxGenericValidator(& m_bRememberPassword) );
    if (FindWindow(XRCID("ID_CHECKBOX_VNC_ROOTLESS")))
        FindWindow(XRCID("ID_CHECKBOX_VNC_ROOTLESS"))->SetValidator( wxGenericValidator(& m_bRootless) );
////@end VncPropertyDialog content construction

    if (!m_bStorePasswords)
        m_pCtrlRememberPassword->Enable(false);

////@begin VncPropertyDialog content initialisation
////@end VncPropertyDialog content initialisation
}

/*!
 * Should we show tooltips?
 */

bool VncPropertyDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap VncPropertyDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon VncPropertyDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void VncPropertyDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void VncPropertyDialog::OnOkClick( wxCommandEvent& event )
{
    wxASSERT_MSG(m_pCfg, _T("VncPropertyDialog::OnOkClick: No configuration"));
    if (m_pCfg) {
        TransferDataFromWindow();
        m_pCfg->bSetVncRememberPassword(m_bRememberPassword);
        m_pCfg->bSetVncRootless(m_bRootless);
        m_pCfg->iSetVncDisplayNumber(m_iDisplayNumber);
        m_pCfg->sSetVncHostName(m_sHostname);
        m_pCfg->sSetVncPassword(m_sPassword);
    }
    event.Skip();
}

#ifdef __WXMSW__
/* dimbor: on wxWindows 3.0.2 we need to make a sunset by hand
   for some reason */
bool VncPropertyDialog::TransferDataToWindow()
{
    wxDialog::TransferDataToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_HOST")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_HOST"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_DPY")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_DPY"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_PASSWD")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_PASSWD"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_VNC_REMEMBER_PWD")))
        FindWindow(XRCID("ID_CHECKBOX_VNC_REMEMBER_PWD"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_VNC_ROOTLESS")))
        FindWindow(XRCID("ID_CHECKBOX_VNC_ROOTLESS"))->GetValidator()->TransferToWindow();
    return true;
}

bool VncPropertyDialog::TransferDataFromWindow()
{
    wxDialog::TransferDataFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_HOST")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_HOST"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_DPY")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_DPY"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_VNC_PASSWD")))
        FindWindow(XRCID("ID_TEXTCTRL_VNC_PASSWD"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_VNC_REMEMBER_PWD")))
        FindWindow(XRCID("ID_CHECKBOX_VNC_REMEMBER_PWD"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_VNC_ROOTLESS")))
        FindWindow(XRCID("ID_CHECKBOX_VNC_ROOTLESS"))->GetValidator()->TransferFromWindow();
    return true;
}
#endif
