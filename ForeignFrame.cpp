// $Id: ForeignFrame.cpp 591 2010-10-24 23:58:47Z felfert $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ForeignFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

/*
 * Defines canonicalized platform names (e.g. __LINUX__)
 */
#include <wx/platform.h>

#ifndef __WXMSW__

////@begin includes
////@end includes

#include "ForeignFrame.h"
#include "osdep.h"

////@begin XPM images
////@end XPM images

/*!
 * ForeignFrame type definition
 */

#define ID_TIMER 5999

IMPLEMENT_CLASS( ForeignFrame, wxFrame )

/*!
 * ForeignFrame event table definition
 */

BEGIN_EVENT_TABLE( ForeignFrame, wxFrame )

////@begin ForeignFrame event table entries
    EVT_WINDOW_CREATE( ForeignFrame::OnCreate )

    EVT_MENU( XRCID("ID_FOREIGN_SUSPEND"), ForeignFrame::OnForeignSuspendClick )

    EVT_MENU( XRCID("ID_FOREIGN_TERMINATE"), ForeignFrame::OnForeignTerminateClick )

    EVT_MENU( XRCID("ID_FOREIGN_CLOSE"), ForeignFrame::OnForeignCloseClick )

////@end ForeignFrame event table entries

    EVT_TIMER(ID_TIMER, ForeignFrame::OnTimer)

END_EVENT_TABLE()

/*!
 * ForeignFrame constructors
 */

ForeignFrame::ForeignFrame( )
    : m_cTimer(this, ID_TIMER)
{
    Init();
}

ForeignFrame::ForeignFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_cTimer(this, ID_TIMER)
{
    Init();
    Create( parent, id, caption, pos, size, style );
}

/*!
 * ForeignFrame creator
 */

bool ForeignFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ForeignFrame creation
    SetParent(parent);
    CreateControls();
////@end ForeignFrame creation
	wxUnusedVar(style);
	wxUnusedVar(size);
	wxUnusedVar(pos);
	wxUnusedVar(caption);
	wxUnusedVar(id);
    return true;
}

/*!
 * Member initialisation 
 */

void ForeignFrame::Init()
{
////@begin ForeignFrame member initialisation
////@end ForeignFrame member initialisation
}
/*!
 * Control creation for ForeignFrame
 */

void ForeignFrame::CreateControls()
{
////@begin ForeignFrame content construction
    if (!wxXmlResource::Get()->LoadFrame(this, GetParent(), wxT("ID_FOREIGNFRAME")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
////@end ForeignFrame content construction

    // Create custom windows not generated automatically here.
////@begin ForeignFrame content initialisation
////@end ForeignFrame content initialisation

    wxToolBar *tb = GetToolBar();
    tb->Fit();
    wxSize sz = tb->GetBestSize();
    SetMinSize(sz);
    SetMaxSize(sz);
    SetSize(sz);
}

/*!
 * Should we show tooltips?
 */

bool ForeignFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ForeignFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ForeignFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ForeignFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ForeignFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ForeignFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ForeignFrame icon retrieval
}

void ForeignFrame::OnTimer( wxTimerEvent& event )
{
    Hide();
    Close();
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_FOREIGN_SUSPEND
 */

void ForeignFrame::OnForeignSuspendClick( wxCommandEvent& event )
{
    Close();
    event.Skip();
    ::wxKill(getppid(), wxSIGHUP);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_FOREIGN_TERMINATE
 */

void ForeignFrame::OnForeignTerminateClick( wxCommandEvent& event )
{
    Close();
    event.Skip();
    ::wxKill(m_nOtherPID, wxSIGTERM);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_FOREIGN_CLOSE
 */

void ForeignFrame::OnForeignCloseClick( wxCommandEvent& event )
{
    Close();
    event.Skip();
    close_foreign(m_nForeignWindowID);
}

/*!
 * wxEVT_CREATE event handler for ID_FOREIGNFRAME
 */

void ForeignFrame::OnCreate( wxWindowCreateEvent& event )
{
    reparent_pulldown(m_nForeignWindowID);
    m_cTimer.Start(6000, wxTIMER_ONE_SHOT);
    event.Skip();
}

#endif
    // !__WXMSW__
