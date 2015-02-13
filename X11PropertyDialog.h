// $Id: X11PropertyDialog.h 590 2010-10-23 22:42:11Z felfert $
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

#ifndef _X11PROPERTYDIALOG_H_
#define _X11PROPERTYDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "X11PropertyDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "X11PropertyDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations
class MyXmlConfig;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_SETTINGS_X11 10090
#define SYMBOL_X11PROPERTYDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_X11PROPERTYDIALOG_TITLE _("X11 Session Settings - OpenNX")
#define SYMBOL_X11PROPERTYDIALOG_IDNAME ID_DIALOG_SETTINGS_X11
#define SYMBOL_X11PROPERTYDIALOG_SIZE wxSize(114, 75)
#define SYMBOL_X11PROPERTYDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * X11PropertyDialog class declaration
 */

class X11PropertyDialog: public wxDialog
{    
    DECLARE_CLASS( X11PropertyDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    X11PropertyDialog( );
    X11PropertyDialog( wxWindow* parent, wxWindowID id = SYMBOL_X11PROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_X11PROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_X11PROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_X11PROPERTYDIALOG_SIZE, long style = SYMBOL_X11PROPERTYDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_X11PROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_X11PROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_X11PROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_X11PROPERTYDIALOG_SIZE, long style = SYMBOL_X11PROPERTYDIALOG_STYLE );

    void SetConfig(MyXmlConfig *);

private:
    /// Creates the controls and sizers
    void CreateControls();
    void UpdateDialogConstraints(bool);

    void OnContextHelp(wxCommandEvent &);

////@begin X11PropertyDialog event handler declarations

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_CONSOLE
    void OnRadiobuttonX11ConsoleSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_XCLIENTS
    void OnRadiobuttonX11XclientsSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_CUSTOMCMD
    void OnRadiobuttonX11CustomcmdSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_VDESKTOP
    void OnRadiobuttonX11VdesktopSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_WIN_FLOATING
    void OnRadiobuttonX11WinFloatingSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end X11PropertyDialog event handler declarations

public:
////@begin X11PropertyDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end X11PropertyDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin X11PropertyDialog member variables
    wxRadioButton* m_pCtrlRunCustomCommand;
    wxTextCtrl* m_pCtrlCustomCommand;
    wxRadioButton* m_pCtrlWinFloating;
    wxCheckBox* m_pCtrlDisableXagent;
    wxCheckBox* m_pCtrlDisableTaint;
private:
    bool m_bRunConsole;
    bool m_bRunXclients;
    bool m_bRunCustom;
    bool m_bVirtualDesktop;
    bool m_bFloatingWindow;
    bool m_bDisableTaint;
    wxString m_sCustomCommand;
    bool m_bDisableXagent;
////@end X11PropertyDialog member variables

    MyXmlConfig *m_pCfg;
};

#endif
    // _X11PROPERTYDIALOG_H_
