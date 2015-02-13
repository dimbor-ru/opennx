// $Id: LogDialog.h 84 2006-12-13 06:14:36Z felfert $
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

#ifndef _LOGDIALOG_H_
#define _LOGDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "LogDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "LogDialog_symbols.h"
#include "wx/xrc/xmlres.h"
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
#define ID_DIALOG_LOG 10104
#define SYMBOL_LOGDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX
#define SYMBOL_LOGDIALOG_TITLE _("Session Log - OpenNX")
#define SYMBOL_LOGDIALOG_IDNAME ID_DIALOG_LOG
#define SYMBOL_LOGDIALOG_SIZE wxSize(200, 150)
#define SYMBOL_LOGDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * LogDialog class declaration
 */

class LogDialog: public wxDialog
{    
    DECLARE_CLASS( LogDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    LogDialog( );
    LogDialog( wxWindow* parent, wxWindowID id = SYMBOL_LOGDIALOG_IDNAME, const wxString& caption = SYMBOL_LOGDIALOG_TITLE, const wxPoint& pos = SYMBOL_LOGDIALOG_POSITION, const wxSize& size = SYMBOL_LOGDIALOG_SIZE, long style = SYMBOL_LOGDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LOGDIALOG_IDNAME, const wxString& caption = SYMBOL_LOGDIALOG_TITLE, const wxPoint& pos = SYMBOL_LOGDIALOG_POSITION, const wxSize& size = SYMBOL_LOGDIALOG_SIZE, long style = SYMBOL_LOGDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

    void SetFileName(wxString);
    void ReadLogFile(wxString);

////@begin LogDialog event handler declarations

////@end LogDialog event handler declarations

private:

////@begin LogDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LogDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LogDialog member variables
    wxTextCtrl* m_TextCtrl;
////@end LogDialog member variables

    wxString m_sFileName;
};

#endif
    // _LOGDIALOG_H_
