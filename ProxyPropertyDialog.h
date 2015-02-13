// $Id: ProxyPropertyDialog.h 653 2011-12-07 15:39:54Z felfert $
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

#ifndef _PROXYPROPERTYDIALOG_H_
#define _PROXYPROPERTYDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ProxyPropertyDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "ProxyPropertyDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
#include "wx/spinctrl.h"
#include "wx/valtext.h"
#include "wx/hyperlink.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSpinCtrl;
class wxHyperlinkCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_PROXYPROPERTYDIALOG 10083
#define SYMBOL_PROXYPROPERTYDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_PROXYPROPERTYDIALOG_TITLE _("Proxy settings - OpenNX")
#define SYMBOL_PROXYPROPERTYDIALOG_IDNAME ID_PROXYPROPERTYDIALOG
#define SYMBOL_PROXYPROPERTYDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_PROXYPROPERTYDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * ProxyPropertyDialog class declaration
 */

class ProxyPropertyDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ProxyPropertyDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    ProxyPropertyDialog();
    ProxyPropertyDialog( wxWindow* parent, wxWindowID id = SYMBOL_PROXYPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_PROXYPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_PROXYPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_PROXYPROPERTYDIALOG_SIZE, long style = SYMBOL_PROXYPROPERTYDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PROXYPROPERTYDIALOG_IDNAME, const wxString& caption = SYMBOL_PROXYPROPERTYDIALOG_TITLE, const wxPoint& pos = SYMBOL_PROXYPROPERTYDIALOG_POSITION, const wxSize& size = SYMBOL_PROXYPROPERTYDIALOG_SIZE, long style = SYMBOL_PROXYPROPERTYDIALOG_STYLE );

    /// Destructor
    ~ProxyPropertyDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    void UpdateDialogConstraints();

    void OnContextHelp(wxCommandEvent &);

////@begin ProxyPropertyDialog event handler declarations

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_HTTPPROXY
    void OnRadiobuttonHttpproxySelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_EXTERNALPROXY
    void OnRadiobuttonExternalproxySelected( wxCommandEvent& event );

////@end ProxyPropertyDialog event handler declarations

////@begin ProxyPropertyDialog member function declarations

    bool GetBUseProxy() const { return m_bUseProxy ; }
    void SetBUseProxy(bool value) { m_bUseProxy = value ; }

    wxString GetSProxyHost() const { return m_sProxyHost ; }
    void SetSProxyHost(wxString value) { m_sProxyHost = value ; }

    int GetIProxyPort() const { return m_iProxyPort ; }
    void SetIProxyPort(int value) { m_iProxyPort = value ; }

    wxString GetSProxyUser() const { return m_sProxyUser ; }
    void SetSProxyUser(wxString value) { m_sProxyUser = value ; }

    wxString GetSProxyPass() const { return m_sProxyPass ; }
    void SetSProxyPass(wxString value) { m_sProxyPass = value ; }

    bool GetBProxyPassRemember() const { return m_bProxyPassRemember ; }
    void SetBProxyPassRemember(bool value) { m_bProxyPassRemember = value ; }

    bool GetBExternalProxy() const { return m_bExternalProxy ; }
    void SetBExternalProxy(bool value) { m_bExternalProxy = value ; }

    wxString GetSProxyCommand() const { return m_sProxyCommand ; }
    void SetSProxyCommand(wxString value) { m_sProxyCommand = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ProxyPropertyDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ProxyPropertyDialog member variables
    wxTextCtrl* m_pCtrlProxyHost;
    wxSpinCtrl* m_pCtrlProxyPort;
    wxTextCtrl* m_pCtrlProxyUser;
    wxTextCtrl* m_pCtrlProxyPass;
    wxCheckBox* m_pCtrlProxyPassRemember;
    wxTextCtrl* m_pCtrlProxyCommand;
    wxHyperlinkCtrl* m_pProxyCmdHelp;
private:
    bool m_bUseProxy;
    wxString m_sProxyHost;
    int m_iProxyPort;
    wxString m_sProxyUser;
    wxString m_sProxyPass;
    bool m_bProxyPassRemember;
    bool m_bExternalProxy;
    wxString m_sProxyCommand;
////@end ProxyPropertyDialog member variables
};

#endif
    // _PROXYPROPERTYDIALOG_H_
