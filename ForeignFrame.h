// $Id: ForeignFrame.h 591 2010-10-24 23:58:47Z felfert $
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

#ifndef _FOREIGNFRAME_H_
#define _FOREIGNFRAME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ForeignFrame.h"
#endif

/*
 * Defines canonicalized platform names (e.g. __LINUX__)
 */
#include <wx/platform.h>

#ifndef __WXMSW__
/*!
 * Includes
 */

////@begin includes
#include "ForeignFrame_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/frame.h"
#include "wx/toolbar.h"
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
#define ID_FOREIGNFRAME 10152
#define SYMBOL_FOREIGNFRAME_STYLE wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP|wxNO_BORDER
#define SYMBOL_FOREIGNFRAME_TITLE _("OPENNX-PULLDOWN")
#define SYMBOL_FOREIGNFRAME_IDNAME ID_FOREIGNFRAME
#define SYMBOL_FOREIGNFRAME_SIZE wxDefaultSize
#define SYMBOL_FOREIGNFRAME_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * ForeignFrame class declaration
 */

class ForeignFrame: public wxFrame
{    
    DECLARE_CLASS( ForeignFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    ForeignFrame( );
    ForeignFrame( wxWindow* parent, wxWindowID id = SYMBOL_FOREIGNFRAME_IDNAME, const wxString& caption = SYMBOL_FOREIGNFRAME_TITLE, const wxPoint& pos = SYMBOL_FOREIGNFRAME_POSITION, const wxSize& size = SYMBOL_FOREIGNFRAME_SIZE, long style = SYMBOL_FOREIGNFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FOREIGNFRAME_IDNAME, const wxString& caption = SYMBOL_FOREIGNFRAME_TITLE, const wxPoint& pos = SYMBOL_FOREIGNFRAME_POSITION, const wxSize& size = SYMBOL_FOREIGNFRAME_SIZE, long style = SYMBOL_FOREIGNFRAME_STYLE );

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    void OnTimer( wxTimerEvent &);

////@begin ForeignFrame event handler declarations

    /// wxEVT_CREATE event handler for ID_FOREIGNFRAME
    void OnCreate( wxWindowCreateEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_FOREIGN_SUSPEND
    void OnForeignSuspendClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_FOREIGN_TERMINATE
    void OnForeignTerminateClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_FOREIGN_CLOSE
    void OnForeignCloseClick( wxCommandEvent& event );

////@end ForeignFrame event handler declarations

////@begin ForeignFrame member function declarations

    long GetForeignWindowID() const { return m_nForeignWindowID ; }
    void SetForeignWindowID(long value) { m_nForeignWindowID = value ; }

    long GetOtherPID() const { return m_nOtherPID ; }
    void SetOtherPID(long value) { m_nOtherPID = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ForeignFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ForeignFrame member variables
private:
    long m_nForeignWindowID;
    long m_nOtherPID;
    wxTimer m_cTimer;
////@end ForeignFrame member variables
};

#endif
    // !__WXMSW__
#endif
    // _FOREIGNFRAME_H_
