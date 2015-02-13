// $Id: PanicDialog.cpp 148 2009-02-10 20:04:38Z felfert $
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
# include "config.h"
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "PanicDialog.h"
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

#include "PanicDialog.h"

////@begin XPM images
////@end XPM images

#include <wx/artprov.h>
#include "Icon.h"

/*!
 * PanicDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanicDialog, wxDialog )

/*!
 * PanicDialog event table definition
 */

BEGIN_EVENT_TABLE( PanicDialog, wxDialog )

////@begin PanicDialog event table entries
////@end PanicDialog event table entries

END_EVENT_TABLE()

/*!
 * PanicDialog constructors
 */

PanicDialog::PanicDialog( )
{
    Init();
}

PanicDialog::PanicDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * PanicDialog creator
 */

bool PanicDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanicDialog creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanicDialog creation
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

void PanicDialog::Init()
{
////@begin PanicDialog member initialisation
    m_iDialogClass = wxICON_WARNING;
    m_sMessage = wxEmptyString;
    m_pCtrlBitmap = NULL;
    m_pCtrlMessage = NULL;
////@end PanicDialog member initialisation
}
/*!
 * Control creation for PanicDialog
 */

void PanicDialog::CreateControls()
{    
////@begin PanicDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_PANICDIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlBitmap = XRCCTRL(*this, "wxID_STATIC_BITMAP", wxStaticBitmap);
    m_pCtrlMessage = XRCCTRL(*this, "wxID_STATIC", wxStaticText);
////@end PanicDialog content construction

    // Create custom windows not generated automatically here.
////@begin PanicDialog content initialisation
////@end PanicDialog content initialisation

    wxArtID artid = wxART_WARNING;
    switch (m_iDialogClass) {
        case wxICON_INFORMATION:
            artid = wxART_INFORMATION;
            break;
        case wxICON_ERROR:
            artid = wxART_ERROR;
            break;
    }
    m_pCtrlBitmap->SetBitmap(wxArtProvider::GetBitmap(artid, wxART_MESSAGE_BOX));
    m_pCtrlMessage->SetLabel(m_sMessage);
}

/*!
 * Should we show tooltips?
 */

bool PanicDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap PanicDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon PanicDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}
