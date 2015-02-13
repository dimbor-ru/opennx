// $Id: SupressibleMessageDialog.h 587 2010-10-21 22:35:37Z felfert $
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

#ifndef _SUPESSIBLEMESSAGEDIALOG_H_
#define _SUPESSIBLEMESSAGEDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SupessibleMessageDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "SupressibleMessageDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
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
#define ID_SUPRESSIBLEMESSAGEDIALOG 10142
#define SYMBOL_SUPRESSIBLEMESSAGEDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_SUPRESSIBLEMESSAGEDIALOG_TITLE _("SupressibleMessageDialog")
#define SYMBOL_SUPRESSIBLEMESSAGEDIALOG_IDNAME ID_SUPRESSIBLEMESSAGEDIALOG
#define SYMBOL_SUPRESSIBLEMESSAGEDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_SUPRESSIBLEMESSAGEDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * SupressibleMessageDialog class declaration
 */

class SupressibleMessageDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( SupressibleMessageDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    SupressibleMessageDialog();
    SupressibleMessageDialog( wxWindow* parent, wxWindowID id = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_IDNAME, const wxString& caption = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_TITLE, const wxPoint& pos = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_POSITION, const wxSize& size = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_SIZE, long style = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_STYLE );

    SupressibleMessageDialog(wxWindow *parent, const wxString &message, const wxString &caption, long style = wxICON_EXCLAMATION|wxOK);

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_IDNAME, const wxString& caption = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_TITLE, const wxPoint& pos = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_POSITION, const wxSize& size = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_SIZE, long style = SYMBOL_SUPRESSIBLEMESSAGEDIALOG_STYLE );

    /// Destructor
    ~SupressibleMessageDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    int ShowConditional(const wxString &msgid, int defaultResult);

    void OnNoClick( wxCommandEvent& event );

////@begin SupressibleMessageDialog event handler declarations

////@end SupressibleMessageDialog event handler declarations

////@begin SupressibleMessageDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SupressibleMessageDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SupressibleMessageDialog member variables
    wxStaticBitmap* m_pDialogIcon;
    wxStaticText* m_pMessage;
private:
    bool m_bHide;
    long m_lButtonStyle;
    long m_lIconStyle;
////@end SupressibleMessageDialog member variables
};

#endif
    // _SUPESSIBLEMESSAGEDIALOG_H_
