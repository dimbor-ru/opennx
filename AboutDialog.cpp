// $Id: AboutDialog.cpp 281 2009-07-01 14:43:00Z felfert $
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
#pragma implementation "AboutDialog.h"
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
#include "AboutDialog.h"
#include "ExtHtmlWindow.h"
#include "Icon.h"
#include "opennxApp.h"

////@begin XPM images
////@end XPM images

/*!
 * AboutDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( AboutDialog, wxDialog )

    /*!
     * AboutDialog event table definition
     */

BEGIN_EVENT_TABLE( AboutDialog, wxDialog )

////@begin AboutDialog event table entries
    EVT_BUTTON( wxID_OK, AboutDialog::OnOKClick )

////@end AboutDialog event table entries

END_EVENT_TABLE()

    /*!
     * AboutDialog constructors
     */

AboutDialog::AboutDialog( )
{
}

AboutDialog::AboutDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * AboutDialog creator
 */

bool AboutDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin AboutDialog member initialisation
    m_pHtmlWindow = NULL;
////@end AboutDialog member initialisation

////@begin AboutDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end AboutDialog creation

    int fs[7];
    wxFont fv = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxFont ff = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    for (int i = 0; i < 7; i++)
        fs[i] = fv.GetPointSize();
    m_pHtmlWindow->SetFonts(fv.GetFaceName(), ff.GetFaceName(), fs);
    m_pHtmlWindow->SetBorders(0);

    wxString version = _("Version") + wxString::Format(wxT(" <B>%s</B>"),
        ::wxGetApp().GetVersion().c_str());

    wxString content = ::wxGetApp().LoadFileFromResource(wxT("res/about.html"));
    content.Replace(wxT("<VERSION>"), version);
    content.Replace(wxT("<WXVERSION>"), wxVERSION_STRING);
    content.Replace(wxT("\"res:"), wxT("\"") + ::wxGetApp().GetResourcePrefix());

    m_pHtmlWindow->SetPage(content);
    m_pHtmlWindow->SetBackgroundColour(GetBackgroundColour());
    if (!content.IsEmpty()) {
        int width, height;
        m_pHtmlWindow->GetSize(&width, &height);
        m_pHtmlWindow->GetInternalRepresentation()->Layout(width);
        height = m_pHtmlWindow->GetInternalRepresentation()->GetHeight();
        width = m_pHtmlWindow->GetInternalRepresentation()->GetWidth();
        m_pHtmlWindow->SetSize(width, height);
        m_pHtmlWindow->SetSizeHints(width, height);
        Fit();
    }

    return TRUE;
}

/*!
 * Control creation for AboutDialog
 */

void AboutDialog::CreateControls()
{    
    ////@begin AboutDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_ABOUT")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pHtmlWindow = XRCCTRL(*this, "ID_HTMLWINDOW_ABOUT", extHtmlWindow);
    ////@end AboutDialog content construction

    // Create custom windows not generated automatically here.

    ////@begin AboutDialog content initialisation
    ////@end AboutDialog content initialisation

}

/*!
 * Should we show tooltips?
 */

bool AboutDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap AboutDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon AboutDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void AboutDialog::OnOKClick( wxCommandEvent& event )
{
    wxUnusedVar(event);
    EndDialog(0);
}

