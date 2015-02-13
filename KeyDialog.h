// $Id: KeyDialog.h 590 2010-10-23 22:42:11Z felfert $
//
// Copyright (C) 2006 The OpenNX team
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

#ifndef _KEYDIALOG_H_
#define _KEYDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "KeyDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "KeyDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_KEYDIALOG 10122
#define SYMBOL_KEYDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_KEYDIALOG_TITLE _("Key Management - OpenNX")
#define SYMBOL_KEYDIALOG_IDNAME ID_KEYDIALOG
#define SYMBOL_KEYDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_KEYDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * KeyDialog class declaration
 */

class KeyDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( KeyDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    KeyDialog( );
    KeyDialog( wxWindow* parent, wxWindowID id = SYMBOL_KEYDIALOG_IDNAME, const wxString& caption = SYMBOL_KEYDIALOG_TITLE, const wxPoint& pos = SYMBOL_KEYDIALOG_POSITION, const wxSize& size = SYMBOL_KEYDIALOG_SIZE, long style = SYMBOL_KEYDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_KEYDIALOG_IDNAME, const wxString& caption = SYMBOL_KEYDIALOG_TITLE, const wxPoint& pos = SYMBOL_KEYDIALOG_POSITION, const wxSize& size = SYMBOL_KEYDIALOG_SIZE, long style = SYMBOL_KEYDIALOG_STYLE );

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

private:

    void CheckChanged();

    void OnContextHelp(wxCommandEvent &);

////@begin KeyDialog event handler declarations

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SSHKEY
    void OnTextctrlSshkeyUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_IMPORT
    void OnButtonImportClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_DEFAULT
    void OnDEFAULTClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_SAVE
    void OnSAVEClick( wxCommandEvent& event );

////@end KeyDialog event handler declarations

public:

////@begin KeyDialog member function declarations

    wxString GetSshKey() const { return m_sSshKey ; }
    void SetSshKey(wxString value) { m_sSshKey = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end KeyDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
////@begin KeyDialog member variables
    wxTextCtrl* m_pCtrlSshKey;
    wxButton* m_pCtrlSave;
private:
    wxString m_sSshKey;
////@end KeyDialog member variables
};

#endif
    // _KEYDIALOG_H_
