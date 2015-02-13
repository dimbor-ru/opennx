// $Id: ProxyPropertyDialog.cpp 647 2011-06-23 17:52:29Z felfert $
//
// Copyright (C) 2010 The OpenNX team
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ProxyPropertyDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/cshelp.h>
#include <wx/config.h>

////@begin includes
////@end includes

#include "ProxyPropertyDialog.h"
#include "MyValidator.h"
#include "opennxApp.h"

////@begin XPM images
////@end XPM images


/*!
 * ProxyPropertyDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ProxyPropertyDialog, wxDialog )


/*!
 * ProxyPropertyDialog event table definition
 */

BEGIN_EVENT_TABLE( ProxyPropertyDialog, wxDialog )

////@begin ProxyPropertyDialog event table entries
    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_HTTPPROXY"), ProxyPropertyDialog::OnRadiobuttonHttpproxySelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_EXTERNALPROXY"), ProxyPropertyDialog::OnRadiobuttonExternalproxySelected )

////@end ProxyPropertyDialog event table entries
//
    EVT_MENU(wxID_CONTEXT_HELP, ProxyPropertyDialog::OnContextHelp)

END_EVENT_TABLE()


/*!
 * ProxyPropertyDialog constructors
 */

ProxyPropertyDialog::ProxyPropertyDialog()
{
    Init();
}

ProxyPropertyDialog::ProxyPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * ProxyPropertyDialog creator
 */

bool ProxyPropertyDialog::Create( wxWindow* parent, wxWindowID, const wxString&, const wxPoint&, const wxSize&, long )
{
////@begin ProxyPropertyDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ProxyPropertyDialog creation
    ::wxGetApp().EnableContextHelp(this);
    wxString d;
    wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &d);
    wxFileName hfile(d, wxT("pconnect.html"));
    hfile.AppendDir(wxT("share"));
    hfile.MakeAbsolute();
    m_pProxyCmdHelp->SetURL(hfile.GetFullPath().Prepend(wxT("file://")));
    return true;
}

/*!
 * ProxyPropertyDialog destructor
 */

ProxyPropertyDialog::~ProxyPropertyDialog()
{
////@begin ProxyPropertyDialog destruction
////@end ProxyPropertyDialog destruction
}


/*!
 * Member initialisation
 */

void ProxyPropertyDialog::Init()
{
////@begin ProxyPropertyDialog member initialisation
    m_bUseProxy = true;
    m_iProxyPort = 3128;
    m_bProxyPassRemember = false;
    m_bExternalProxy = false;
    m_pCtrlProxyHost = NULL;
    m_pCtrlProxyPort = NULL;
    m_pCtrlProxyUser = NULL;
    m_pCtrlProxyPass = NULL;
    m_pCtrlProxyPassRemember = NULL;
    m_pCtrlProxyCommand = NULL;
    m_pProxyCmdHelp = NULL;
////@end ProxyPropertyDialog member initialisation
}

/*!
 * Control creation for ProxyPropertyDialog
 */

void ProxyPropertyDialog::CreateControls()
{    
////@begin ProxyPropertyDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_PROXYPROPERTYDIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlProxyHost = XRCCTRL(*this, "ID_TEXTCTRL_PROXYHOST", wxTextCtrl);
    m_pCtrlProxyPort = XRCCTRL(*this, "ID_SPINCTRL_PROXYPORT", wxSpinCtrl);
    m_pCtrlProxyUser = XRCCTRL(*this, "ID_TEXTCTRL_PROXYUSER", wxTextCtrl);
    m_pCtrlProxyPass = XRCCTRL(*this, "ID_TEXTCTRL_PROXYPASS", wxTextCtrl);
    m_pCtrlProxyPassRemember = XRCCTRL(*this, "ID_CHECKBOX_PROXYPASS_REMEMBER", wxCheckBox);
    m_pCtrlProxyCommand = XRCCTRL(*this, "ID_TEXTCTRL_PROXYCOMMAND", wxTextCtrl);
    m_pProxyCmdHelp = XRCCTRL(*this, "ID_HYPERLINKCTRL", wxHyperlinkCtrl);
    // Set validators
    if (FindWindow(XRCID("ID_RADIOBUTTON_HTTPPROXY")))
        FindWindow(XRCID("ID_RADIOBUTTON_HTTPPROXY"))->SetValidator( wxGenericValidator(& m_bUseProxy) );
    if (FindWindow(XRCID("ID_TEXTCTRL_PROXYHOST")))
        FindWindow(XRCID("ID_TEXTCTRL_PROXYHOST"))->SetValidator( MyValidator(MyValidator::MYVAL_HOST, & m_sProxyHost) );
    if (FindWindow(XRCID("ID_SPINCTRL_PROXYPORT")))
        FindWindow(XRCID("ID_SPINCTRL_PROXYPORT"))->SetValidator( wxGenericValidator(& m_iProxyPort) );
    if (FindWindow(XRCID("ID_TEXTCTRL_PROXYUSER")))
        FindWindow(XRCID("ID_TEXTCTRL_PROXYUSER"))->SetValidator( wxTextValidator(wxFILTER_NONE, & m_sProxyUser) );
    if (FindWindow(XRCID("ID_TEXTCTRL_PROXYPASS")))
        FindWindow(XRCID("ID_TEXTCTRL_PROXYPASS"))->SetValidator( wxTextValidator(wxFILTER_NONE, & m_sProxyPass) );
    if (FindWindow(XRCID("ID_CHECKBOX_PROXYPASS_REMEMBER")))
        FindWindow(XRCID("ID_CHECKBOX_PROXYPASS_REMEMBER"))->SetValidator( wxGenericValidator(& m_bProxyPassRemember) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_EXTERNALPROXY")))
        FindWindow(XRCID("ID_RADIOBUTTON_EXTERNALPROXY"))->SetValidator( wxGenericValidator(& m_bExternalProxy) );
    if (FindWindow(XRCID("ID_TEXTCTRL_PROXYCOMMAND")))
        FindWindow(XRCID("ID_TEXTCTRL_PROXYCOMMAND"))->SetValidator( wxTextValidator(wxFILTER_NONE, & m_sProxyCommand) );
////@end ProxyPropertyDialog content construction

    // Create custom windows not generated automatically here.
////@begin ProxyPropertyDialog content initialisation
////@end ProxyPropertyDialog content initialisation
}

void ProxyPropertyDialog::UpdateDialogConstraints()
{
    if (m_pCtrlProxyHost)
        m_pCtrlProxyHost->Enable(m_bUseProxy);
    if (m_pCtrlProxyPort)
        m_pCtrlProxyPort->Enable(m_bUseProxy);
    if (m_pCtrlProxyUser)
        m_pCtrlProxyUser->Enable(m_bUseProxy);
    if (m_pCtrlProxyPass)
        m_pCtrlProxyPass->Enable(m_bUseProxy);
    if (m_pCtrlProxyPassRemember)
        m_pCtrlProxyPassRemember->Enable(m_bUseProxy);
    if (m_pCtrlProxyCommand)
        m_pCtrlProxyCommand->Enable(m_bExternalProxy);
}

/*!
 * Should we show tooltips?
 */

bool ProxyPropertyDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ProxyPropertyDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ProxyPropertyDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ProxyPropertyDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ProxyPropertyDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ProxyPropertyDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ProxyPropertyDialog icon retrieval
}

void ProxyPropertyDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_HTTPPROXY
 */

void ProxyPropertyDialog::OnRadiobuttonHttpproxySelected( wxCommandEvent& event )
{
    m_pCtrlProxyHost->Enable(true);
    m_pCtrlProxyPort->Enable(true);
    m_pCtrlProxyUser->Enable(true);
    m_pCtrlProxyPass->Enable(true);
    m_pCtrlProxyPassRemember->Enable(true);
    m_pCtrlProxyCommand->Enable(false);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_EXTERNALPROXY
 */

void ProxyPropertyDialog::OnRadiobuttonExternalproxySelected( wxCommandEvent& event )
{
    m_pCtrlProxyHost->Enable(false);
    m_pCtrlProxyPort->Enable(false);
    m_pCtrlProxyUser->Enable(false);
    m_pCtrlProxyPass->Enable(false);
    m_pCtrlProxyPassRemember->Enable(false);
    m_pCtrlProxyCommand->Enable(true);
    event.Skip();
}



