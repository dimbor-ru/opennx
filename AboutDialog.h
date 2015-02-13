// $Id: AboutDialog.h 151 2009-02-10 22:30:59Z felfert $
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

#ifndef _ABOUTDIALOG_H_
#define _ABOUTDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "AboutDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "AboutDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/html/htmlwin.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class extHtmlWindow;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_ABOUT 10036
#define SYMBOL_ABOUTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_ABOUTDIALOG_TITLE _("About OpenNX")
#define SYMBOL_ABOUTDIALOG_IDNAME ID_DIALOG_ABOUT
#define SYMBOL_ABOUTDIALOG_SIZE wxDefaultSize
#define SYMBOL_ABOUTDIALOG_POSITION wxDefaultPosition
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
 * AboutDialog class declaration
 */

class AboutDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( AboutDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    AboutDialog( );
    AboutDialog( wxWindow* parent, wxWindowID id = SYMBOL_ABOUTDIALOG_IDNAME, const wxString& caption = SYMBOL_ABOUTDIALOG_TITLE, const wxPoint& pos = SYMBOL_ABOUTDIALOG_POSITION, const wxSize& size = SYMBOL_ABOUTDIALOG_SIZE, long style = SYMBOL_ABOUTDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ABOUTDIALOG_IDNAME, const wxString& caption = SYMBOL_ABOUTDIALOG_TITLE, const wxPoint& pos = SYMBOL_ABOUTDIALOG_POSITION, const wxSize& size = SYMBOL_ABOUTDIALOG_SIZE, long style = SYMBOL_ABOUTDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin AboutDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOKClick( wxCommandEvent& event );

////@end AboutDialog event handler declarations

////@begin AboutDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end AboutDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin AboutDialog member variables
    extHtmlWindow* m_pHtmlWindow;
////@end AboutDialog member variables
};

#endif
    // _ABOUTDIALOG_H_
