// $Id: LogDialog.cpp 688 2012-02-18 02:36:07Z felfert $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "LogDialog.h"
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
#include <wx/textfile.h>
#include <wx/config.h>

#include "LogDialog.h"
#include "Icon.h"

////@begin XPM images
////@end XPM images

#include "trace.h"
ENABLE_TRACE;

/*!
 * LogDialog type definition
 */

IMPLEMENT_CLASS( LogDialog, wxDialog )

/*!
 * LogDialog event table definition
 */

BEGIN_EVENT_TABLE( LogDialog, wxDialog )

////@begin LogDialog event table entries
////@end LogDialog event table entries

END_EVENT_TABLE()

/*!
 * LogDialog constructors
 */

LogDialog::LogDialog( )
{
}

LogDialog::LogDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * LogDialog creator
 */

bool LogDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin LogDialog member initialisation
    m_TextCtrl = NULL;
////@end LogDialog member initialisation

////@begin LogDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LogDialog creation
    if (!m_sFileName.IsEmpty())
        ReadLogFile(m_sFileName);
    return TRUE;
}

/*!
 * Control creation for LogDialog
 */

void LogDialog::CreateControls()
{    
////@begin LogDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_LOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_TextCtrl = XRCCTRL(*this, "ID_TEXTCTRL", wxTextCtrl);
////@end LogDialog content construction

    // Create custom windows not generated automatically here.

////@begin LogDialog content initialisation
////@end LogDialog content initialisation

    wxFont ff(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    myLogTrace(MYTRACETAG, wxT("Font=%s"), ff.GetNativeFontInfoDesc().c_str());
    m_TextCtrl->SetFont(ff);
}

void LogDialog::SetFileName(wxString fn)
{
    m_sFileName = fn;
}

void LogDialog::ReadLogFile(wxString name)
{
    if (!name.IsEmpty())
        m_sFileName = name;

    if (!m_sFileName.IsEmpty()) {
        m_TextCtrl->LoadFile(m_sFileName);
        long n = m_TextCtrl->GetLastPosition();
        if (m_TextCtrl->GetRange(n - 1, n) == wxT("\004"))
            m_TextCtrl->Remove(n - 1, n);
    }
}

/*!
 * Should we show tooltips?
 */

bool LogDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap LogDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon LogDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}


