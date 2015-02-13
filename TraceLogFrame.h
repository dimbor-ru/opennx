// $Id: TraceLogFrame.h 387 2009-09-06 12:02:50Z felfert $
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

#ifndef _TRACELOGFRAME_H_
#define _TRACELOGFRAME_H_


/*!
 * Includes
 */

////@begin includes
#include "TraceLogFrame_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/frame.h"
#include "wx/listctrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxListCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_TRACELOGFRAME 10000
#define SYMBOL_TRACELOGFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_TRACELOGFRAME_TITLE _("Trace Log")
#define SYMBOL_TRACELOGFRAME_IDNAME ID_TRACELOGFRAME
#define SYMBOL_TRACELOGFRAME_SIZE wxSize(400, 300)
#define SYMBOL_TRACELOGFRAME_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * TraceLogFrame class declaration
 */

class TraceLogFrame: public wxFrame
{    
    DECLARE_CLASS( TraceLogFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    TraceLogFrame();
    TraceLogFrame( wxWindow* parent, wxWindowID id = SYMBOL_TRACELOGFRAME_IDNAME, const wxString& caption = SYMBOL_TRACELOGFRAME_TITLE, const wxPoint& pos = SYMBOL_TRACELOGFRAME_POSITION, const wxSize& size = SYMBOL_TRACELOGFRAME_SIZE, long style = SYMBOL_TRACELOGFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TRACELOGFRAME_IDNAME, const wxString& caption = SYMBOL_TRACELOGFRAME_TITLE, const wxPoint& pos = SYMBOL_TRACELOGFRAME_POSITION, const wxSize& size = SYMBOL_TRACELOGFRAME_SIZE, long style = SYMBOL_TRACELOGFRAME_STYLE );

    /// Destructor
    ~TraceLogFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    void AddEntry(const wxDateTime &, int, const wxString &);

////@begin TraceLogFrame event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_TRACELOGFRAME
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CLOSE
    void OnCLOSEClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_SAVEAS
    void OnSAVEASClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CLEAR
    void OnCLEARClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
    void OnEXITClick( wxCommandEvent& event );

////@end TraceLogFrame event handler declarations

////@begin TraceLogFrame member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TraceLogFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TraceLogFrame member variables
    wxListCtrl* m_pCtrlTraceLog;
////@end TraceLogFrame member variables
};

#endif
    // _TRACELOGFRAME_H_
