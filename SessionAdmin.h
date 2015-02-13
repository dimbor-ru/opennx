// $Id: SessionAdmin.h 399 2009-09-09 19:35:12Z felfert $
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

#ifndef _SESSIONADMIN_H_
#define _SESSIONADMIN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "SessionAdmin.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "SessionAdmin_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/frame.h"
#include "wx/toolbar.h"
#include "wx/listctrl.h"
////@end includes

/*!
 * Forward declarations
 */

class SessionList;
////@begin forward declarations
class wxListCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAME_ADMIN 10103
#define SYMBOL_SESSIONADMIN_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX
#define SYMBOL_SESSIONADMIN_TITLE _("Session Administrator - OpenNX")
#define SYMBOL_SESSIONADMIN_IDNAME ID_FRAME_ADMIN
#define SYMBOL_SESSIONADMIN_SIZE wxDefaultSize
#define SYMBOL_SESSIONADMIN_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * SessionAdmin class declaration
 */

class SessionAdmin: public wxFrame
{    
    DECLARE_CLASS( SessionAdmin )
    DECLARE_EVENT_TABLE()

public:

    void SessionToolsEnable(bool enable, bool running = false);
    /// Constructors
    SessionAdmin( );
    SessionAdmin( wxWindow* parent, wxWindowID id = SYMBOL_SESSIONADMIN_IDNAME, const wxString& caption = SYMBOL_SESSIONADMIN_TITLE, const wxPoint& pos = SYMBOL_SESSIONADMIN_POSITION, const wxSize& size = SYMBOL_SESSIONADMIN_SIZE, long style = SYMBOL_SESSIONADMIN_STYLE );
    virtual ~SessionAdmin();

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SESSIONADMIN_IDNAME, const wxString& caption = SYMBOL_SESSIONADMIN_TITLE, const wxPoint& pos = SYMBOL_SESSIONADMIN_POSITION, const wxSize& size = SYMBOL_SESSIONADMIN_SIZE, long style = SYMBOL_SESSIONADMIN_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

    // Event handler for events from SessionList
    void OnSessionList(wxCommandEvent& event);

////@begin SessionAdmin event handler declarations

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_NEW
    void OnMenuSessionNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_PREFERENCES
    void OnPREFERENCESClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
    void OnEXITClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_TERMINATE
    void OnMenuSessionTerminateClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_PSTATS
    void OnMenuSessionPstatsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_FSTATS
    void OnMenuSessionFstatsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_LOG
    void OnMenuSessionLogClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_REMOVE
    void OnMenuSessionRemoveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_KILL
    void OnMenuSessionKillClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_REFRESH
    void OnMenuRefreshClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_ABOUT
    void OnABOUTClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_NEW
    void OnToolSessionNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_TERMINATE
    void OnToolSessionTerminateClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_PSTATS
    void OnToolSessionPstatsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_FSTATS
    void OnToolSessionFstatsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_LOG
    void OnToolSessionLogClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_REFRESH
    void OnToolRefreshClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_REMOVE
    void OnToolSessionRemoveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_KILL
    void OnToolSessionKillClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL
    void OnListctrlSelected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_DESELECTED event handler for ID_LISTCTRL
    void OnListctrlDeselected( wxListEvent& event );

////@end SessionAdmin event handler declarations

////@begin SessionAdmin member function declarations

    wxString GetNxDirectory() const { return m_NxDirectory ; }
    void SetNxDirectory(wxString value) { m_NxDirectory = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SessionAdmin member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
    void ShowSessionStats(long, bool);


////@begin SessionAdmin member variables
    wxListCtrl* m_SessionListCtrl;
    wxString m_NxDirectory;
////@end SessionAdmin member variables

    SessionList *m_sessions;
};

#endif
    // _SESSIONADMIN_H_
