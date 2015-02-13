// $Id: XdmPropertyDialog.h 653 2011-12-07 15:39:54Z felfert $
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

#ifndef _XDMPROPERTYDIALOG_H_
#define _XDMPROPERTYDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "XdmPropertyDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "XdmPropertyDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/spinctrl.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSpinCtrl;
////@end forward declarations
class MyXmlConfig;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_XDMPROPERTYDIALOG 10187
#define SYMBOL_XDMPROPERTYDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_XDMPROPERTYDIALOG_TITLE _("XDM Session Settings - OpenNX")
#define SYMBOL_XDMPROPERTYDIALOG_IDNAME ID_XDMPROPERTYDIALOG
#define SYMBOL_XDMPROPERTYDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_XDMPROPERTYDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * XdmPropertyDialog class declaration
 */

class XdmPropertyDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( XdmPropertyDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    XdmPropertyDialog();
    XdmPropertyDialog( wxWindow* parent, wxWindowID id = SYMBOL_XDMPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_XDMPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_XDMPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_XDMPROPERTYDIALOG_SIZE, long style = SYMBOL_XDMPROPERTYDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_XDMPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_XDMPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_XDMPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_XDMPROPERTYDIALOG_SIZE, long style = SYMBOL_XDMPROPERTYDIALOG_STYLE );

    /// Destructor
    ~XdmPropertyDialog();

    /// Initialises member variables
    void Init();

    void SetConfig(MyXmlConfig *);

private:
    /// Creates the controls and sizers
    void CreateControls();

    void OnContextHelp(wxCommandEvent &);

////@begin XdmPropertyDialog event handler declarations

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_AUTO
    void OnRadiobuttonXdmAutoSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_DIRECT
    void OnRadiobuttonXdmDirectSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_BROADCAST
    void OnRadiobuttonXdmBroadcastSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_XDM_INDIRECT
    void OnRadiobuttonXdmIndirectSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end XdmPropertyDialog event handler declarations

////@begin XdmPropertyDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end XdmPropertyDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin XdmPropertyDialog member variables
    wxRadioButton* m_pCtrlRadioXdmServer;
    wxRadioButton* m_pCtrlRadioXdmQuery;
    wxTextCtrl* m_pCtrlQueryHost;
    wxSpinCtrl* m_pCtrlQueryPort;
    wxRadioButton* m_pCtrlRadioXdmBroadcast;
    wxSpinCtrl* m_pCtrlBroadcastPort;
    wxRadioButton* m_pCtrlRadioXdmList;
    wxTextCtrl* m_pCtrlListHost;
    wxSpinCtrl* m_pCtrlListPort;
private:
    wxString m_sXdmQueryHost;
    int m_iXdmQueryPort;
    wxString m_sXdmListHost;
    int m_iXdmListPort;
    int m_iXdmBroadcastPort;
////@end XdmPropertyDialog member variables

    MyXmlConfig *m_pCfg;
    int m_iXdmMode;
};

#endif
    // _XDMPROPERTYDIALOG_H_
