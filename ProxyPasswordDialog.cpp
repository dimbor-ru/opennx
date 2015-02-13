// $Id: ProxyPasswordDialog.cpp 455 2010-01-30 03:26:11Z felfert $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ProxyPasswordDialog.h"
#endif

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

#include "ProxyPasswordDialog.h"

////@begin XPM images
////@end XPM images


/*!
 * ProxyPasswordDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ProxyPasswordDialog, wxDialog )


/*!
 * ProxyPasswordDialog event table definition
 */

BEGIN_EVENT_TABLE( ProxyPasswordDialog, wxDialog )

////@begin ProxyPasswordDialog event table entries
////@end ProxyPasswordDialog event table entries

END_EVENT_TABLE()


/*!
 * ProxyPasswordDialog constructors
 */

ProxyPasswordDialog::ProxyPasswordDialog()
{
    Init();
}

ProxyPasswordDialog::ProxyPasswordDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * ProxyPasswordDialog creator
 */

bool ProxyPasswordDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin ProxyPasswordDialog creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY|wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ProxyPasswordDialog creation
    return true;
}


/*!
 * ProxyPasswordDialog destructor
 */

ProxyPasswordDialog::~ProxyPasswordDialog()
{
////@begin ProxyPasswordDialog destruction
////@end ProxyPasswordDialog destruction
}


/*!
 * Member initialisation
 */

void ProxyPasswordDialog::Init()
{
////@begin ProxyPasswordDialog member initialisation
////@end ProxyPasswordDialog member initialisation
}


/*!
 * Control creation for ProxyPasswordDialog
 */

void ProxyPasswordDialog::CreateControls()
{    
////@begin ProxyPasswordDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_PROXYPASSWORDDIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    // Set validators
    if (FindWindow(XRCID("ID_TEXTCTRL_PROXYPASS2")))
        FindWindow(XRCID("ID_TEXTCTRL_PROXYPASS2"))->SetValidator( wxTextValidator(wxFILTER_NONE, & m_sPassword) );
////@end ProxyPasswordDialog content construction

    // Create custom windows not generated automatically here.
    if (FindWindow(XRCID("ID_TEXTCTRL_PROXYPASS2")))
        FindWindow(XRCID("ID_TEXTCTRL_PROXYPASS2"))->SetFocus();
}


/*!
 * Should we show tooltips?
 */

bool ProxyPasswordDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ProxyPasswordDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ProxyPasswordDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ProxyPasswordDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ProxyPasswordDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ProxyPasswordDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ProxyPasswordDialog icon retrieval
}
