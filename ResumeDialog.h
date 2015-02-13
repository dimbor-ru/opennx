// $Id: ResumeDialog.h 674 2012-02-05 05:55:40Z felfert $
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

#ifndef _RESUMEDIALOG_H_
#define _RESUMEDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ResumeDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "ResumeDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/listctrl.h"
#include "wx/statline.h"
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
#define ID_RESUMEDIALOG 10050
#define SYMBOL_RESUMEDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_RESUMEDIALOG_TITLE _("Select session - OpenNX")
#define SYMBOL_RESUMEDIALOG_IDNAME ID_RESUMEDIALOG
#define SYMBOL_RESUMEDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_RESUMEDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * ResumeDialog class declaration
 */

class ResumeDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ResumeDialog )
    DECLARE_EVENT_TABLE()

public:
    typedef enum {
        Refresh,
        New,
        Resume,
        Takeover,
        Terminate,
    } Mode;
    
    /// Constructors
    ResumeDialog( );
    ResumeDialog( wxWindow* parent, wxWindowID id = SYMBOL_RESUMEDIALOG_IDNAME, const wxString& caption = SYMBOL_RESUMEDIALOG_TITLE, const wxPoint& pos = SYMBOL_RESUMEDIALOG_POSITION, const wxSize& size = SYMBOL_RESUMEDIALOG_SIZE, long style = SYMBOL_RESUMEDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_RESUMEDIALOG_IDNAME, const wxString& caption = SYMBOL_RESUMEDIALOG_TITLE, const wxPoint& pos = SYMBOL_RESUMEDIALOG_POSITION, const wxSize& size = SYMBOL_RESUMEDIALOG_SIZE, long style = SYMBOL_RESUMEDIALOG_STYLE );

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    void EnableNew(bool);
    void SetPreferredSession(const wxString &name) { m_sPreferredSession = name; };
    void AddSession(const wxString&, const wxString&, const wxString&, const wxString&,
            const wxString&, const wxString&, const wxString&, const wxString&, const wxString& user = wxT(""));
    void SetAttachMode(bool);

private:

////@begin ResumeDialog event handler declarations

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL_SESSIONS
    void OnListctrlSessionsSelected( wxListEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_REFRESH
    void OnRefreshClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TERMINATE
    void OnButtonTerminateClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TAKEOVER
    void OnButtonTakeoverClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RESUME
    void OnButtonResumeClick( wxCommandEvent& event );

////@end ResumeDialog event handler declarations

public:
////@begin ResumeDialog member function declarations

    Mode GetMode() const { return m_eMode ; }
    void SetMode(Mode value) { m_eMode = value ; }

    wxString GetSelectedId() const { return m_sSelectedId ; }
    void SetSelectedId(wxString value) { m_sSelectedId = value ; }

    wxString GetSelectedName() const { return m_sSelectedName ; }
    void SetSelectedName(wxString value) { m_sSelectedName = value ; }

    wxString GetSelectedType() const { return m_sSelectedType ; }
    void SetSelectedType(wxString value) { m_sSelectedType = value ; }

    wxString GetSelectedPort() const { return m_sSelectedPort ; }
    void SetSelectedPort(wxString value) { m_sSelectedPort = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ResumeDialog member function declarations

private:
    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ResumeDialog member variables
    wxListCtrl* m_pCtrlSessions;
    wxButton* m_pCtrlTerminate;
    wxButton* m_pCtrlTakeover;
    wxButton* m_pCtrlResume;
    wxButton* m_pCtrlNew;
private:
    long m_lActiveSession;
    Mode m_eMode;
    wxString m_sSelectedId;
    wxString m_sSelectedName;
    wxString m_sSelectedType;
    wxString m_sSelectedPort;
    bool m_bShadow;
////@end ResumeDialog member variables

    wxString m_sPreferredSession;
};

#endif
    // _RESUMEDIALOG_H_
