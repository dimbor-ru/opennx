// $Id: tracelogApp.h 674 2012-02-05 05:55:40Z felfert $
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

#ifndef _TRACELOGAPP_H_
#define _TRACELOGAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/image.h"
#include "TraceLogFrame.h"
////@end includes
#include <wx/socket.h>
#include <wx/regex.h>
#include <map>

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

class wxTaskBarIcon;
class TraceLogFrame;
class DebugStringGrabber;
class wxSocketOutputStream;

/*!
 * tracelogApp class declaration
 */

class tracelogApp: public wxApp
{    
    DECLARE_CLASS( tracelogApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    tracelogApp();

    void Init();
    const wxString &GetResourcePrefix() { return m_sResourcePrefix; }

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

    void Terminate();
    void OnDebugString(wxCommandEvent &event);
    void OnSocketEvent(wxSocketEvent& event);
    void OnServerEvent(wxSocketEvent& event);

    void StartSocketServer();
    void StopSocketServer();
    bool ServerEnabled() {
        return (NULL != m_pSocketServer);
    }

////@begin tracelogApp event handler declarations

////@end tracelogApp event handler declarations

////@begin tracelogApp member function declarations

////@end tracelogApp member function declarations

////@begin tracelogApp member variables
////@end tracelogApp member variables

private:
    wxString m_sResourcePrefix;
    wxString m_sAllowedPeers;
    wxTaskBarIcon *m_pTaskBarIcon;
    TraceLogFrame *m_pLogFrame;
    DebugStringGrabber *m_pGrabber;
    wxSocketServer *m_pSocketServer;
    std::map<wxSocketBase *, wxSocketOutputStream *> m_mClients;
    wxRegEx m_cAllowedPeers;
    int m_nPort;
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(tracelogApp)
////@end declare app

#endif
    // _TRACELOGAPP_H_
