// $Id: ProxyPasswordDialog.h 449 2010-01-24 21:43:59Z felfert $
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

#ifndef _PROXYPASSWORDDIALOG_H_
#define _PROXYPASSWORDDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ProxyPasswordDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "ProxyPasswordDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valtext.h"
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
#define ID_PROXYPASSWORDDIALOG 10184
#define SYMBOL_PROXYPASSWORDDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_PROXYPASSWORDDIALOG_TITLE _("HTTP proxy password - OpenNX")
#define SYMBOL_PROXYPASSWORDDIALOG_IDNAME ID_PROXYPASSWORDDIALOG
#define SYMBOL_PROXYPASSWORDDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_PROXYPASSWORDDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * ProxyPasswordDialog class declaration
 */

class ProxyPasswordDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ProxyPasswordDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    ProxyPasswordDialog();
    ProxyPasswordDialog( wxWindow* parent, wxWindowID id = SYMBOL_PROXYPASSWORDDIALOG_IDNAME, const wxString& caption = SYMBOL_PROXYPASSWORDDIALOG_TITLE, const wxPoint& pos = SYMBOL_PROXYPASSWORDDIALOG_POSITION, const wxSize& size = SYMBOL_PROXYPASSWORDDIALOG_SIZE, long style = SYMBOL_PROXYPASSWORDDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PROXYPASSWORDDIALOG_IDNAME, const wxString& caption = SYMBOL_PROXYPASSWORDDIALOG_TITLE, const wxPoint& pos = SYMBOL_PROXYPASSWORDDIALOG_POSITION, const wxSize& size = SYMBOL_PROXYPASSWORDDIALOG_SIZE, long style = SYMBOL_PROXYPASSWORDDIALOG_STYLE );

    /// Destructor
    ~ProxyPasswordDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin ProxyPasswordDialog event handler declarations

////@end ProxyPasswordDialog event handler declarations

////@begin ProxyPasswordDialog member function declarations

    wxString GetPassword() const { return m_sPassword ; }
    void SetPassword(wxString value) { m_sPassword = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ProxyPasswordDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ProxyPasswordDialog member variables
private:
    wxString m_sPassword;
////@end ProxyPasswordDialog member variables
};

#endif
    // _PROXYPASSWORDDIALOG_H_
