// $Id: TraceLogRemote.h 674 2012-02-05 05:55:40Z felfert $
//
// Copyright (C) 2009 The OpenNX team
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


#ifndef _TRACELOGREMOTE_H_
#define _TRACELOGREMOTE_H_


/*!
 * Includes
 */

////@begin includes
#include "TraceLogRemote_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
#include "wx/spinctrl.h"
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
#define ID_TRACELOGREMOTE 10001
#define SYMBOL_TRACELOGREMOTE_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_TRACELOGREMOTE_TITLE _("Activate remote logging")
#define SYMBOL_TRACELOGREMOTE_IDNAME ID_TRACELOGREMOTE
#define SYMBOL_TRACELOGREMOTE_SIZE wxSize(400, 300)
#define SYMBOL_TRACELOGREMOTE_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * TraceLogRemote class declaration
 */

class TraceLogRemote: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( TraceLogRemote )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    TraceLogRemote();
    TraceLogRemote( wxWindow* parent, wxWindowID id = SYMBOL_TRACELOGREMOTE_IDNAME, const wxString& caption = SYMBOL_TRACELOGREMOTE_TITLE, const wxPoint& pos = SYMBOL_TRACELOGREMOTE_POSITION, const wxSize& size = SYMBOL_TRACELOGREMOTE_SIZE, long style = SYMBOL_TRACELOGREMOTE_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TRACELOGREMOTE_IDNAME, const wxString& caption = SYMBOL_TRACELOGREMOTE_TITLE, const wxPoint& pos = SYMBOL_TRACELOGREMOTE_POSITION, const wxSize& size = SYMBOL_TRACELOGREMOTE_SIZE, long style = SYMBOL_TRACELOGREMOTE_STYLE );

    /// Destructor
    ~TraceLogRemote();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin TraceLogRemote event handler declarations

////@end TraceLogRemote event handler declarations

////@begin TraceLogRemote member function declarations

    int GetPort() const { return port ; }
    void SetPort(int value) { port = value ; }

    wxString GetCregex() const { return cregex ; }
    void SetCregex(wxString value) { cregex = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TraceLogRemote member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TraceLogRemote member variables
private:
    int port;
    wxString cregex;
////@end TraceLogRemote member variables
};

#endif
    // _TRACELOGREMOTE_H_
