// $Id: XdmPropertyDialog.cpp 663 2012-01-04 04:53:13Z felfert $
//
// Copyright (C) 2011 The OpenNX team
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "XdmPropertyDialog.h"
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

#include "XdmPropertyDialog.h"
#include "Icon.h"
#include "MyXmlConfig.h"
#include "MyValidator.h"
#include "opennxApp.h"

#include <wx/config.h>
#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images


/*!
 * XdmPropertyDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( XdmPropertyDialog, wxDialog )


    /*!
     * XdmPropertyDialog event table definition
     */

BEGIN_EVENT_TABLE( XdmPropertyDialog, wxDialog )

    ////@begin XdmPropertyDialog event table entries
    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_XDM_AUTO"), XdmPropertyDialog::OnRadiobuttonXdmAutoSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_XDM_DIRECT"), XdmPropertyDialog::OnRadiobuttonXdmDirectSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_XDM_BROADCAST"), XdmPropertyDialog::OnRadiobuttonXdmBroadcastSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_XDM_INDIRECT"), XdmPropertyDialog::OnRadiobuttonXdmIndirectSelected )

    EVT_BUTTON( wxID_OK, XdmPropertyDialog::OnOkClick )

    ////@end XdmPropertyDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, XdmPropertyDialog::OnContextHelp)

END_EVENT_TABLE()


    /*!
     * XdmPropertyDialog constructors
     */

    XdmPropertyDialog::XdmPropertyDialog()
: m_pCfg(NULL)
{
    Init();
}

    XdmPropertyDialog::XdmPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
: m_pCfg(NULL)
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


void XdmPropertyDialog::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

/*!
 * XdmPropertyDialog creator
 */

bool XdmPropertyDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxUnusedVar(id);
    wxUnusedVar(caption);
    wxUnusedVar(pos);
    wxUnusedVar(size);
    wxUnusedVar(style);
    ////@begin XdmPropertyDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end XdmPropertyDialog creation
    return true;
}

/*!
 * XdmPropertyDialog destructor
 */

XdmPropertyDialog::~XdmPropertyDialog()
{
    ////@begin XdmPropertyDialog destruction
    ////@end XdmPropertyDialog destruction
}

/*!
 * Member initialisation
 */

void XdmPropertyDialog::Init()
{
    ////@begin XdmPropertyDialog member initialisation
    m_sXdmQueryHost = wxT("localhost");
    m_iXdmQueryPort = 177;
    m_sXdmListHost = wxT("localhost");
    m_iXdmListPort = 177;
    m_iXdmBroadcastPort = 177;
    m_pCtrlRadioXdmServer = NULL;
    m_pCtrlRadioXdmQuery = NULL;
    m_pCtrlQueryHost = NULL;
    m_pCtrlQueryPort = NULL;
    m_pCtrlRadioXdmBroadcast = NULL;
    m_pCtrlBroadcastPort = NULL;
    m_pCtrlRadioXdmList = NULL;
    m_pCtrlListHost = NULL;
    m_pCtrlListPort = NULL;
    ////@end XdmPropertyDialog member initialisation
}


/*!
 * Control creation for XdmPropertyDialog
 */

void XdmPropertyDialog::CreateControls()
{    
    ////@begin XdmPropertyDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_XDMPROPERTYDIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlRadioXdmServer = XRCCTRL(*this, "ID_RADIOBUTTON_XDM_AUTO", wxRadioButton);
    m_pCtrlRadioXdmQuery = XRCCTRL(*this, "ID_RADIOBUTTON_XDM_DIRECT", wxRadioButton);
    m_pCtrlQueryHost = XRCCTRL(*this, "ID_TEXTCTRL_QUERYHOST", wxTextCtrl);
    m_pCtrlQueryPort = XRCCTRL(*this, "ID_SPINCTRL_QUERYPORT", wxSpinCtrl);
    m_pCtrlRadioXdmBroadcast = XRCCTRL(*this, "ID_RADIOBUTTON_XDM_BROADCAST", wxRadioButton);
    m_pCtrlBroadcastPort = XRCCTRL(*this, "ID_SPINCTRL_BROADCASTPORT", wxSpinCtrl);
    m_pCtrlRadioXdmList = XRCCTRL(*this, "ID_RADIOBUTTON_XDM_INDIRECT", wxRadioButton);
    m_pCtrlListHost = XRCCTRL(*this, "ID_TEXTCTRL_LISTHOST", wxTextCtrl);
    m_pCtrlListPort = XRCCTRL(*this, "ID_SPINCTRL_LISTPORT", wxSpinCtrl);
    // Set validators
    if (FindWindow(XRCID("ID_TEXTCTRL_QUERYHOST")))
        FindWindow(XRCID("ID_TEXTCTRL_QUERYHOST"))->SetValidator( MyValidator(MyValidator::MYVAL_HOST, & m_sXdmQueryHost) );
    if (FindWindow(XRCID("ID_SPINCTRL_QUERYPORT")))
        FindWindow(XRCID("ID_SPINCTRL_QUERYPORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iXdmQueryPort) );
    if (FindWindow(XRCID("ID_SPINCTRL_BROADCASTPORT")))
        FindWindow(XRCID("ID_SPINCTRL_BROADCASTPORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iXdmBroadcastPort) );
    if (FindWindow(XRCID("ID_TEXTCTRL_LISTHOST")))
        FindWindow(XRCID("ID_TEXTCTRL_LISTHOST"))->SetValidator( MyValidator(MyValidator::MYVAL_HOST, & m_sXdmListHost) );
    if (FindWindow(XRCID("ID_SPINCTRL_LISTPORT")))
        FindWindow(XRCID("ID_SPINCTRL_LISTPORT"))->SetValidator( MyValidator(MyValidator::MYVAL_NUMERIC, & m_iXdmListPort) );
    ////@end XdmPropertyDialog content construction

    wxASSERT_MSG(m_pCfg, _T("XdmPropertyDialog::Create: No configuration"));
    if (m_pCfg) {
        m_sXdmListHost = m_pCfg->sGetXdmListHost();
        m_sXdmQueryHost = m_pCfg->sGetXdmQueryHost();
        m_iXdmBroadcastPort = m_pCfg->iGetXdmBroadcastPort();
        m_iXdmListPort = m_pCfg->iGetXdmListPort();
        m_iXdmQueryPort = m_pCfg->iGetXdmQueryPort();
        switch (m_pCfg->eGetXdmMode()) {
            case MyXmlConfig::XDM_MODE_SERVER:
                m_iXdmMode = 0;
                m_pCtrlQueryHost->Enable(false);
                m_pCtrlQueryPort->Enable(false);
                m_pCtrlBroadcastPort->Enable(false);
                m_pCtrlListHost->Enable(false);
                m_pCtrlListPort->Enable(false);
                m_pCtrlRadioXdmServer->SetValue(true);
                break;
            case MyXmlConfig::XDM_MODE_QUERY:
                m_iXdmMode = 1;
                m_pCtrlQueryHost->Enable(true);
                m_pCtrlQueryPort->Enable(true);
                m_pCtrlBroadcastPort->Enable(false);
                m_pCtrlListHost->Enable(false);
                m_pCtrlListPort->Enable(false);
                m_pCtrlRadioXdmQuery->SetValue(true);
                break;
            case MyXmlConfig::XDM_MODE_BROADCAST:
                m_iXdmMode = 2;
                m_pCtrlQueryHost->Enable(false);
                m_pCtrlQueryPort->Enable(false);
                m_pCtrlBroadcastPort->Enable(true);
                m_pCtrlListHost->Enable(false);
                m_pCtrlListPort->Enable(false);
                m_pCtrlRadioXdmBroadcast->SetValue(true);
                break;
            case MyXmlConfig::XDM_MODE_LIST:
                m_iXdmMode = 3;
                m_pCtrlQueryHost->Enable(false);
                m_pCtrlQueryPort->Enable(false);
                m_pCtrlBroadcastPort->Enable(false);
                m_pCtrlListHost->Enable(true);
                m_pCtrlListPort->Enable(true);
                m_pCtrlRadioXdmList->SetValue(true);
                break;
        }
    }

    ////@begin XdmPropertyDialog content initialisation
    ////@end XdmPropertyDialog content initialisation
}


/*!
 * Should we show tooltips?
 */

bool XdmPropertyDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap XdmPropertyDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon XdmPropertyDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void XdmPropertyDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void XdmPropertyDialog::OnOkClick( wxCommandEvent& event )
{
    TransferDataFromWindow();
    if (m_pCfg) {
        m_pCfg->sSetXdmListHost(m_sXdmListHost);
        m_pCfg->sSetXdmQueryHost(m_sXdmQueryHost);
        m_pCfg->iSetXdmBroadcastPort(m_iXdmBroadcastPort);
        m_pCfg->iSetXdmListPort(m_iXdmListPort);
        m_pCfg->iSetXdmQueryPort(m_iXdmQueryPort);
        switch (m_iXdmMode) {
            case 0:
                m_pCfg->eSetXdmMode(MyXmlConfig::XDM_MODE_SERVER);
                break;
            case 1:
                m_pCfg->eSetXdmMode(MyXmlConfig::XDM_MODE_QUERY);
                break;
            case 2:
                m_pCfg->eSetXdmMode(MyXmlConfig::XDM_MODE_BROADCAST);
                break;
            case 3:
                m_pCfg->eSetXdmMode(MyXmlConfig::XDM_MODE_LIST);
                break;
        }
    }
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_AUTO
 */

void XdmPropertyDialog::OnRadiobuttonXdmAutoSelected( wxCommandEvent& event )
{
    m_iXdmMode = 0;
    m_pCtrlQueryHost->Enable(false);
    m_pCtrlQueryPort->Enable(false);
    m_pCtrlBroadcastPort->Enable(false);
    m_pCtrlListHost->Enable(false);
    m_pCtrlListPort->Enable(false);
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_DIRECT
 */

void XdmPropertyDialog::OnRadiobuttonXdmDirectSelected( wxCommandEvent& event )
{
    m_iXdmMode = 1;
    m_pCtrlQueryHost->Enable(true);
    m_pCtrlQueryPort->Enable(true);
    m_pCtrlBroadcastPort->Enable(false);
    m_pCtrlListHost->Enable(false);
    m_pCtrlListPort->Enable(false);
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_BROADCAST
 */

void XdmPropertyDialog::OnRadiobuttonXdmBroadcastSelected( wxCommandEvent& event )
{
    m_iXdmMode = 2;
    m_pCtrlQueryHost->Enable(false);
    m_pCtrlQueryPort->Enable(false);
    m_pCtrlBroadcastPort->Enable(true);
    m_pCtrlListHost->Enable(false);
    m_pCtrlListPort->Enable(false);
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_INDIRECT
 */

void XdmPropertyDialog::OnRadiobuttonXdmIndirectSelected( wxCommandEvent& event )
{
    m_iXdmMode = 3;
    m_pCtrlQueryHost->Enable(false);
    m_pCtrlQueryPort->Enable(false);
    m_pCtrlBroadcastPort->Enable(false);
    m_pCtrlListHost->Enable(true);
    m_pCtrlListPort->Enable(true);
    event.Skip();
}
