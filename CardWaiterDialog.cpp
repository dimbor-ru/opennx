// $Id: CardWaiterDialog.cpp 281 2009-07-01 14:43:00Z felfert $
//
// Copyright (C) 2008 The OpenNX team
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
# include "config.h"
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "CardWaiterDialog.h"
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

#include "CardWaiterDialog.h"
#include "LibOpenSC.h"
#include "Icon.h"

////@begin XPM images
////@end XPM images


/*!
 * CardWaiterDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CardWaiterDialog, wxDialog )


/*!
 * CardWaiterDialog event table definition
 */

BEGIN_EVENT_TABLE( CardWaiterDialog, wxDialog )

////@begin CardWaiterDialog event table entries
////@end CardWaiterDialog event table entries
    EVT_COMMAND(wxID_ANY, wxEVT_CARDINSERTED, CardWaiterDialog::OnCardInserted)
END_EVENT_TABLE()


/*!
 * CardWaiterDialog constructors
 */

CardWaiterDialog::CardWaiterDialog()
{
    Init();
}

CardWaiterDialog::CardWaiterDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * CardWaiterDialog creator
 */

bool CardWaiterDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxUnusedVar(id);
    wxUnusedVar(caption);
    wxUnusedVar(pos);
    wxUnusedVar(size);
    wxUnusedVar(style);
////@begin CardWaiterDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end CardWaiterDialog creation
    return true;
}


/*!
 * CardWaiterDialog destructor
 */

CardWaiterDialog::~CardWaiterDialog()
{
////@begin CardWaiterDialog destruction
////@end CardWaiterDialog destruction
}


/*!
 * Member initialisation
 */

void CardWaiterDialog::Init()
{
////@begin CardWaiterDialog member initialisation
////@end CardWaiterDialog member initialisation
    m_iReader = -1;
}


/*!
 * Control creation for CardWaiterDialog
 */

void CardWaiterDialog::CreateControls()
{    
////@begin CardWaiterDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_CARDWAITERDIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
////@end CardWaiterDialog content construction

    // Create custom windows not generated automatically here.
////@begin CardWaiterDialog content initialisation
////@end CardWaiterDialog content initialisation
}


/*!
 * Should we show tooltips?
 */

bool CardWaiterDialog::ShowToolTips()
{
    return true;
}

void CardWaiterDialog::OnCardInserted(wxCommandEvent &ev)
{
    m_iReader = ev.GetInt();
    ev.Skip();
    EndModal(wxID_OK);
}

/*!
 * Get bitmap resources
 */

wxBitmap CardWaiterDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon CardWaiterDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin CardWaiterDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end CardWaiterDialog icon retrieval
}

int CardWaiterDialog::WaitForCard(wxWindow *parent) {
    Init();
    if (Create(parent)) {
        LibOpenSC osc;
        return osc.WaitForCard(this);
    }
    return -1;
}
