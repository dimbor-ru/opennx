// $Id: RdpPropertyDialog.h 590 2010-10-23 22:42:11Z felfert $
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

#ifndef _RDPPROPERTYDIALOG_H_
#define _RDPPROPERTYDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "RdpPropertyDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "RdpPropertyDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
#include "wx/valtext.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

class MyXmlConfig;

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_SETTINGS_RDP 10065
#define SYMBOL_RDPPROPERTYDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_RDPPROPERTYDIALOG_TITLE _("RDP Session Settings - OpenNX")
#define SYMBOL_RDPPROPERTYDIALOG_IDNAME ID_DIALOG_SETTINGS_RDP
#define SYMBOL_RDPPROPERTYDIALOG_SIZE wxSize(200, 150)
#define SYMBOL_RDPPROPERTYDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * RdpPropertyDialog class declaration
 */

class RdpPropertyDialog: public wxDialog
{    
    DECLARE_CLASS( RdpPropertyDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RdpPropertyDialog( );
    RdpPropertyDialog( wxWindow* parent, wxWindowID id = SYMBOL_RDPPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_RDPPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_RDPPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_RDPPROPERTYDIALOG_SIZE, long style = SYMBOL_RDPPROPERTYDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_RDPPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_RDPPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_RDPPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_RDPPROPERTYDIALOG_SIZE, long style = SYMBOL_RDPPROPERTYDIALOG_STYLE );

    void SetConfig(MyXmlConfig *);
    void UpdateDialogConstraints();

private:
    /// Creates the controls and sizers
    void CreateControls();

    void OnContextHelp(wxCommandEvent &);

////@begin RdpPropertyDialog event handler declarations

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_WINLOGON
    void OnRadiobuttonRdpWinlogonSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_NXAUTH
    void OnRadiobuttonRdpNxauthSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_AUTOLOGIN
    void OnRadiobuttonRdpAutologinSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_DESKTOP
    void OnRadiobuttonRdpDesktopSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_RUNAPP
    void OnRadiobuttonRdpRunappSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end RdpPropertyDialog event handler declarations

public:
////@begin RdpPropertyDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end RdpPropertyDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
////@begin RdpPropertyDialog member variables
    wxRadioButton* m_pCtrlAutologin;
    wxTextCtrl* m_pCtrlUsername;
    wxTextCtrl* m_pCtrlPassword;
    wxCheckBox* m_pCtrlRememberPassword;
    wxRadioButton* m_pCtrlRunApplication;
    wxTextCtrl* m_pCtrlApplicationString;
private:
    wxString m_sHostname;
    wxString m_sUsername;
    wxString m_sPassword;
    wxString m_sRunCommand;
    bool m_bAutoLogin;
    bool m_bRememberPassword;
    bool m_bShowWinLogon;
    bool m_bUseNxAuth;
    bool m_bRunDesktop;
    bool m_bRunApplication;
    wxString m_sRdpDomain;
////@end RdpPropertyDialog member variables

    MyXmlConfig *m_pCfg;
    bool m_bStorePasswords;
};

#endif
    // _RDPPROPERTYDIALOG_H_
