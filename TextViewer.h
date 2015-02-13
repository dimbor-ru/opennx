// $Id: TextViewer.h 186 2009-02-24 13:01:39Z felfert $
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

#ifndef _TEXTVIEWER_H_
#define _TEXTVIEWER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TextViewer.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "TextViewer_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class myRichTextCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_TEXTVIEWER 10020
#define SYMBOL_TEXTVIEWER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxDIALOG_NO_PARENT|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX
#define SYMBOL_TEXTVIEWER_TITLE _("Text Viewer - OpenNX")
#define SYMBOL_TEXTVIEWER_IDNAME ID_TEXTVIEWER
#define SYMBOL_TEXTVIEWER_SIZE wxDefaultSize
#define SYMBOL_TEXTVIEWER_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * TextViewer class declaration
 */

class TextViewer: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( TextViewer )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    TextViewer( );
    TextViewer( wxWindow* parent, wxWindowID id = SYMBOL_TEXTVIEWER_IDNAME, const wxString& caption = SYMBOL_TEXTVIEWER_TITLE, const wxPoint& pos = SYMBOL_TEXTVIEWER_POSITION, const wxSize& size = SYMBOL_TEXTVIEWER_SIZE, long style = SYMBOL_TEXTVIEWER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TEXTVIEWER_IDNAME, const wxString& caption = SYMBOL_TEXTVIEWER_TITLE, const wxPoint& pos = SYMBOL_TEXTVIEWER_POSITION, const wxSize& size = SYMBOL_TEXTVIEWER_SIZE, long style = SYMBOL_TEXTVIEWER_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

    bool LoadFile(const wxString &file);

private:
////@begin TextViewer event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CLOSE
    void OnCloseClick( wxCommandEvent& event );

////@end TextViewer event handler declarations

////@begin TextViewer member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TextViewer member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TextViewer member variables
    myRichTextCtrl* m_pRichTextCtrl;
    wxString m_sFileName;
////@end TextViewer member variables
};

#endif
    // _TEXTVIEWER_H_
