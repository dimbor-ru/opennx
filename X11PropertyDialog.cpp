// $Id: X11PropertyDialog.cpp 590 2010-10-23 22:42:11Z felfert $
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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "X11PropertyDialog.h"
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

#include "X11PropertyDialog.h"
#include "MyXmlConfig.h"
#include "Icon.h"
#include "opennxApp.h"

#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images

/*!
 * X11PropertyDialog type definition
 */

IMPLEMENT_CLASS( X11PropertyDialog, wxDialog )

/*!
 * X11PropertyDialog event table definition
 */

BEGIN_EVENT_TABLE( X11PropertyDialog, wxDialog )

////@begin X11PropertyDialog event table entries
    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_CONSOLE"), X11PropertyDialog::OnRadiobuttonX11ConsoleSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_XCLIENTS"), X11PropertyDialog::OnRadiobuttonX11XclientsSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_CUSTOMCMD"), X11PropertyDialog::OnRadiobuttonX11CustomcmdSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_VDESKTOP"), X11PropertyDialog::OnRadiobuttonX11VdesktopSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_WIN_FLOATING"), X11PropertyDialog::OnRadiobuttonX11WinFloatingSelected )

    EVT_BUTTON( wxID_OK, X11PropertyDialog::OnOkClick )

////@end X11PropertyDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, X11PropertyDialog::OnContextHelp)

END_EVENT_TABLE()

/*!
 * X11PropertyDialog constructors
 */

X11PropertyDialog::X11PropertyDialog( )
    : m_pCfg(NULL)
{
}

X11PropertyDialog::X11PropertyDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_pCfg(NULL)
{
    Create(parent, id, caption, pos, size, style);
}

void X11PropertyDialog::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

/*!
 * X11PropertyDialog creator
 */

bool X11PropertyDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin X11PropertyDialog member initialisation
    m_pCtrlRunCustomCommand = NULL;
    m_pCtrlCustomCommand = NULL;
    m_pCtrlWinFloating = NULL;
    m_pCtrlDisableXagent = NULL;
    m_pCtrlDisableTaint = NULL;
////@end X11PropertyDialog member initialisation
    wxASSERT_MSG(m_pCfg, _T("X11PropertyDialog::Create: No configuration"));
    if (m_pCfg) {
        m_bRunConsole = m_pCfg->bGetRunConsole();
        m_bRunXclients = m_pCfg->bGetRunXclients();
        m_bRunCustom = ((!m_bRunConsole) && (!m_bRunXclients));
        m_sCustomCommand = m_pCfg->sGetCommandLine();
        m_bVirtualDesktop = m_pCfg->bGetVirtualDesktop();
        m_bFloatingWindow = !m_bVirtualDesktop;
        m_bDisableTaint = m_pCfg->bGetDisableTaint();
        m_bDisableXagent = m_pCfg->bGetDisableXagent();
    }

////@begin X11PropertyDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    Centre();
////@end X11PropertyDialog creation
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

/*!
 * Control creation for X11PropertyDialog
 */

void X11PropertyDialog::CreateControls()
{    
////@begin X11PropertyDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_SETTINGS_X11")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlRunCustomCommand = XRCCTRL(*this, "ID_RADIOBUTTON_X11_CUSTOMCMD", wxRadioButton);
    m_pCtrlCustomCommand = XRCCTRL(*this, "ID_TEXTCTRL_X11_CUSTOMCMD", wxTextCtrl);
    m_pCtrlWinFloating = XRCCTRL(*this, "ID_RADIOBUTTON_X11_WIN_FLOATING", wxRadioButton);
    m_pCtrlDisableXagent = XRCCTRL(*this, "ID_CHECKBOX_X11_DISABLE_XAGENT", wxCheckBox);
    m_pCtrlDisableTaint = XRCCTRL(*this, "ID_CHECKBOX_X11_DISABLE_TAINT", wxCheckBox);
    // Set validators
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_CONSOLE")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_CONSOLE"))->SetValidator( wxGenericValidator(& m_bRunConsole) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_XCLIENTS")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_XCLIENTS"))->SetValidator( wxGenericValidator(& m_bRunXclients) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_CUSTOMCMD")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_CUSTOMCMD"))->SetValidator( wxGenericValidator(& m_bRunCustom) );
    if (FindWindow(XRCID("ID_TEXTCTRL_X11_CUSTOMCMD")))
        FindWindow(XRCID("ID_TEXTCTRL_X11_CUSTOMCMD"))->SetValidator( wxGenericValidator(& m_sCustomCommand) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_VDESKTOP")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_VDESKTOP"))->SetValidator( wxGenericValidator(& m_bVirtualDesktop) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_WIN_FLOATING")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_WIN_FLOATING"))->SetValidator( wxGenericValidator(& m_bFloatingWindow) );
    if (FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_XAGENT")))
        FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_XAGENT"))->SetValidator( wxGenericValidator(& m_bDisableXagent) );
    if (FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_TAINT")))
        FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_TAINT"))->SetValidator( wxGenericValidator(& m_bDisableTaint) );
////@end X11PropertyDialog content construction

    // Create custom windows not generated automatically here.

////@begin X11PropertyDialog content initialisation
////@end X11PropertyDialog content initialisation
    UpdateDialogConstraints(false);
}

/*!
 * Should we show tooltips?
 */

bool X11PropertyDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap X11PropertyDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon X11PropertyDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void X11PropertyDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_CONSOLE
 */

void X11PropertyDialog::OnRadiobuttonX11ConsoleSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints(true);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_XCLIENTS
 */

void X11PropertyDialog::OnRadiobuttonX11XclientsSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints(true);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_CUSTOMCMD
 */

void X11PropertyDialog::OnRadiobuttonX11CustomcmdSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints(true);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_VDESKTOP
 */

void X11PropertyDialog::OnRadiobuttonX11VdesktopSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints(true);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_WIN_FLOATING
 */

void X11PropertyDialog::OnRadiobuttonX11WinFloatingSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints(true);
    event.Skip();
}

void X11PropertyDialog::UpdateDialogConstraints(bool getValues)
{
    if (getValues)
        TransferDataFromWindow();

    m_pCtrlCustomCommand->Enable(m_bRunCustom);
    m_pCtrlDisableTaint->Enable(m_bFloatingWindow);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void X11PropertyDialog::OnOkClick( wxCommandEvent& event )
{
    wxASSERT_MSG(m_pCfg, _T("X11PropertyDialog::OnOkClick: No configuration"));
    if (m_pCfg) {
        TransferDataFromWindow();
        m_pCfg->bSetRunConsole(m_bRunConsole);
        m_pCfg->bSetRunXclients(m_bRunXclients);
        m_pCfg->sSetCommandLine(m_sCustomCommand);
        m_pCfg->bSetVirtualDesktop(m_bVirtualDesktop);
        m_pCfg->bSetDisableTaint(m_bDisableTaint);
        m_pCfg->bSetDisableXagent(m_bDisableXagent);
    }
    event.Skip();
}

#ifdef __WXMSW__
/* dimbor: on wxWindows 3.0.2 we need to make a sunset by hand
   for some reason */
bool X11PropertyDialog::TransferDataToWindow()
{
    wxDialog::TransferDataToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_CONSOLE")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_CONSOLE"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_XCLIENTS")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_XCLIENTS"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_CUSTOMCMD")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_CUSTOMCMD"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_X11_CUSTOMCMD")))
        FindWindow(XRCID("ID_TEXTCTRL_X11_CUSTOMCMD"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_VDESKTOP")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_VDESKTOP"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_WIN_FLOATING")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_WIN_FLOATING"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_XAGENT")))
        FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_XAGENT"))->GetValidator()->TransferToWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_TAINT")))
        FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_TAINT"))->GetValidator()->TransferToWindow();
    return true;
}

bool X11PropertyDialog::TransferDataFromWindow()
{
    wxDialog::TransferDataFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_CONSOLE")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_CONSOLE"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_XCLIENTS")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_XCLIENTS"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_CUSTOMCMD")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_CUSTOMCMD"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_TEXTCTRL_X11_CUSTOMCMD")))
        FindWindow(XRCID("ID_TEXTCTRL_X11_CUSTOMCMD"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_VDESKTOP")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_VDESKTOP"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_WIN_FLOATING")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_WIN_FLOATING"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_XAGENT")))
        FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_XAGENT"))->GetValidator()->TransferFromWindow();
    if (FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_TAINT")))
        FindWindow(XRCID("ID_CHECKBOX_X11_DISABLE_TAINT"))->GetValidator()->TransferFromWindow();
    return true;
}
#endif