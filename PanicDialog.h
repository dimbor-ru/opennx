// $Id: PanicDialog.h 683 2012-02-09 23:37:19Z felfert $
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

#ifndef _PANICDIALOG_H_
#define _PANICDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "PanicDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "PanicDialog_symbols.h"
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
#define ID_PANICDIALOG 10149
#define SYMBOL_PANICDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxCLOSE_BOX
#define SYMBOL_PANICDIALOG_TITLE _T("")
#define SYMBOL_PANICDIALOG_IDNAME ID_PANICDIALOG
#define SYMBOL_PANICDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_PANICDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * PanicDialog class declaration
 */

class PanicDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( PanicDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanicDialog( );
    PanicDialog( wxWindow* parent, wxWindowID id = SYMBOL_PANICDIALOG_IDNAME, const wxString& caption = SYMBOL_PANICDIALOG_TITLE, const wxPoint& pos = SYMBOL_PANICDIALOG_POSITION, const wxSize& size = SYMBOL_PANICDIALOG_SIZE, long style = SYMBOL_PANICDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANICDIALOG_IDNAME, const wxString& caption = SYMBOL_PANICDIALOG_TITLE, const wxPoint& pos = SYMBOL_PANICDIALOG_POSITION, const wxSize& size = SYMBOL_PANICDIALOG_SIZE, long style = SYMBOL_PANICDIALOG_STYLE );

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanicDialog event handler declarations

////@end PanicDialog event handler declarations

////@begin PanicDialog member function declarations

    int GetDialogClass() const { return m_iDialogClass ; }
    void SetDialogClass(int value) { m_iDialogClass = value ; }

    wxString GetMessage() const { return m_sMessage ; }
    void SetMessage(wxString value) { m_sMessage = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanicDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanicDialog member variables
    wxStaticBitmap* m_pCtrlBitmap;
    wxStaticText* m_pCtrlMessage;
private:
    int m_iDialogClass;
    wxString m_sMessage;
////@end PanicDialog member variables
};

#endif
    // _PANICDIALOG_H_
