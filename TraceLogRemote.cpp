// $Id: TraceLogRemote.cpp 674 2012-02-05 05:55:40Z felfert $
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


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "TraceLogRemote.h"

////@begin XPM images
////@end XPM images


/*!
 * TraceLogRemote type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TraceLogRemote, wxDialog )


/*!
 * TraceLogRemote event table definition
 */

BEGIN_EVENT_TABLE( TraceLogRemote, wxDialog )

////@begin TraceLogRemote event table entries
////@end TraceLogRemote event table entries

END_EVENT_TABLE()


/*!
 * TraceLogRemote constructors
 */

TraceLogRemote::TraceLogRemote()
{
    Init();
}

TraceLogRemote::TraceLogRemote( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * TraceLogRemote creator
 */

bool TraceLogRemote::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TraceLogRemote creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end TraceLogRemote creation
    return true;
}


/*!
 * TraceLogRemote destructor
 */

TraceLogRemote::~TraceLogRemote()
{
////@begin TraceLogRemote destruction
////@end TraceLogRemote destruction
}


/*!
 * Member initialisation
 */

void TraceLogRemote::Init()
{
////@begin TraceLogRemote member initialisation
    port = 2020;
    cregex = wxT("^\\d+\\.\\d+\\.\\d+\\.\\d+$");
////@end TraceLogRemote member initialisation
}


/*!
 * Control creation for TraceLogRemote
 */

void TraceLogRemote::CreateControls()
{    
////@begin TraceLogRemote content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_TRACELOGREMOTE")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    // Set validators
    if (FindWindow(XRCID("ID_SPINCTRL_PORT")))
        FindWindow(XRCID("ID_SPINCTRL_PORT"))->SetValidator( wxGenericValidator(& port) );
    if (FindWindow(XRCID("ID_TEXTCTRL")))
        FindWindow(XRCID("ID_TEXTCTRL"))->SetValidator( wxGenericValidator(& cregex) );
////@end TraceLogRemote content construction

    // Create custom windows not generated automatically here.
////@begin TraceLogRemote content initialisation
////@end TraceLogRemote content initialisation
}


/*!
 * Should we show tooltips?
 */

bool TraceLogRemote::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TraceLogRemote::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin TraceLogRemote bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end TraceLogRemote bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TraceLogRemote::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin TraceLogRemote icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end TraceLogRemote icon retrieval
}
