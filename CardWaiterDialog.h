// $Id: CardWaiterDialog.h 209 2009-03-25 14:48:16Z felfert $
//
// Copyright (C) 2008 The OpenNX team
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

#ifndef _CARDWAITERDIALOG_H_
#define _CARDWAITERDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "CardWaiterDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "CardWaiterDialog_symbols.h"
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
#define ID_CARDWAITERDIALOG 10106
#define SYMBOL_CARDWAITERDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_CARDWAITERDIALOG_TITLE _("Insert card - OpenNX")
#define SYMBOL_CARDWAITERDIALOG_IDNAME ID_CARDWAITERDIALOG
#define SYMBOL_CARDWAITERDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_CARDWAITERDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * CardWaiterDialog class declaration
 */

class CardWaiterDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( CardWaiterDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CardWaiterDialog();
    CardWaiterDialog( wxWindow* parent, wxWindowID id = SYMBOL_CARDWAITERDIALOG_IDNAME, const wxString& caption = SYMBOL_CARDWAITERDIALOG_TITLE, const wxPoint& pos = SYMBOL_CARDWAITERDIALOG_POSITION, const wxSize& size = SYMBOL_CARDWAITERDIALOG_SIZE, long style = SYMBOL_CARDWAITERDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CARDWAITERDIALOG_IDNAME, const wxString& caption = SYMBOL_CARDWAITERDIALOG_TITLE, const wxPoint& pos = SYMBOL_CARDWAITERDIALOG_POSITION, const wxSize& size = SYMBOL_CARDWAITERDIALOG_SIZE, long style = SYMBOL_CARDWAITERDIALOG_STYLE );

    /// Destructor
    ~CardWaiterDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin CardWaiterDialog event handler declarations

////@end CardWaiterDialog event handler declarations

////@begin CardWaiterDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end CardWaiterDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();
    void OnCardInserted(wxCommandEvent &);
    int GetReader() { return m_iReader; }
    int WaitForCard(wxWindow *parent);

private:
    int m_iReader;

////@begin CardWaiterDialog member variables
////@end CardWaiterDialog member variables
};

#endif
    // _CARDWAITERDIALOG_H_
