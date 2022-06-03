// $Id: VncPropertyDialog.h 590 2010-10-23 22:42:11Z felfert $
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

#ifndef _VNCPROPERTYDIALOG_H_
#define _VNCPROPERTYDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "VncPropertyDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "VncPropertyDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
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
#define ID_DIALOG_SETTINGS_VNC 10057
#define SYMBOL_VNCPROPERTYDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_VNCPROPERTYDIALOG_TITLE _("VNC Session Settings - OpenNX")
#define SYMBOL_VNCPROPERTYDIALOG_IDNAME ID_DIALOG_SETTINGS_VNC
#define SYMBOL_VNCPROPERTYDIALOG_SIZE wxSize(200, 150)
#define SYMBOL_VNCPROPERTYDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * VncPropertyDialog class declaration
 */

class VncPropertyDialog: public wxDialog
{    
    DECLARE_CLASS( VncPropertyDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    VncPropertyDialog( );
    VncPropertyDialog( wxWindow* parent, wxWindowID id = SYMBOL_VNCPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_VNCPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_VNCPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_VNCPROPERTYDIALOG_SIZE, long style = SYMBOL_VNCPROPERTYDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_VNCPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_VNCPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_VNCPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_VNCPROPERTYDIALOG_SIZE, long style = SYMBOL_VNCPROPERTYDIALOG_STYLE );

    void SetConfig(MyXmlConfig *);

private:
    /// Creates the controls and sizers
    void CreateControls();

    void OnContextHelp(wxCommandEvent &);

////@begin VncPropertyDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end VncPropertyDialog event handler declarations

public:
////@begin VncPropertyDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end VncPropertyDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
////@begin VncPropertyDialog member variables
    wxCheckBox* m_pCtrlRootless;
    wxTextCtrl* m_pCtrlPassword;
    wxCheckBox* m_pCtrlRememberPassword;
    wxString m_sPassword;
    wxString m_sHostname;
    int m_iDisplayNumber;
    bool m_bRootless;
    bool m_bRememberPassword;
////@end VncPropertyDialog member variables

    MyXmlConfig *m_pCfg;
    bool m_bStorePasswords;

};

#endif
    // _VNCPROPERTYDIALOG_H_
