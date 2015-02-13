// $Id: SupressibleMessageDialog.cpp 587 2010-10-21 22:35:37Z felfert $
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
#pragma implementation "SupessibleMessageDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/config.h>
////@begin includes
////@end includes

#include "SupressibleMessageDialog.h"
#include "Icon.h"

////@begin XPM images
////@end XPM images


/*!
 * SupressibleMessageDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( SupressibleMessageDialog, wxDialog )


/*!
 * SupressibleMessageDialog event table definition
 */

BEGIN_EVENT_TABLE( SupressibleMessageDialog, wxDialog )

////@begin SupressibleMessageDialog event table entries
////@end SupressibleMessageDialog event table entries

END_EVENT_TABLE()

int SupressibleMessageDialog::ShowConditional(const wxString &msgid, int defaultResult)
{
    wxString cfgname(wxT("SupressedDialogs/"));
    cfgname.Append(msgid);
    bool hide = false;
    wxConfigBase::Get()->Read(cfgname, &hide, false);
    if (hide)
        return defaultResult;
    int ret = wxDialog::ShowModal();
    if ((wxID_CANCEL == ret) && (wxYES_NO == (m_lButtonStyle & (wxCANCEL|wxYES_NO)))) {
        // No cancel button: Closing dialog => defaultResult
        ret = defaultResult;
    }
    if (m_bHide)
        wxConfigBase::Get()->Write(cfgname, true);
    return ret;
}

/*!
 * SupressibleMessageDialog constructors
 */

SupressibleMessageDialog::SupressibleMessageDialog()
{
    Init();
}

SupressibleMessageDialog::SupressibleMessageDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


SupressibleMessageDialog::SupressibleMessageDialog(wxWindow *parent, const wxString &message, const wxString &caption, long style /* = wxICON_EXCLAMATION|wxOK */)
{
    Init();
    Create(parent, SYMBOL_SUPRESSIBLEMESSAGEDIALOG_IDNAME, caption,
            SYMBOL_SUPRESSIBLEMESSAGEDIALOG_POSITION, SYMBOL_SUPRESSIBLEMESSAGEDIALOG_SIZE,
            SYMBOL_SUPRESSIBLEMESSAGEDIALOG_STYLE);
    m_lIconStyle = style & wxICON_MASK;
    m_lButtonStyle = style & ~wxICON_MASK;
    switch (m_lIconStyle) {
        case wxICON_EXCLAMATION:
            m_pDialogIcon->SetBitmap(wxArtProvider::GetBitmap(wxART_WARNING, wxART_MESSAGE_BOX));
            break;
        case wxICON_ERROR:
            m_pDialogIcon->SetBitmap(wxArtProvider::GetBitmap(wxART_ERROR, wxART_MESSAGE_BOX));
            break;
        case wxICON_QUESTION:
            m_pDialogIcon->SetBitmap(wxArtProvider::GetBitmap(wxART_QUESTION, wxART_MESSAGE_BOX));
            break;
        case wxICON_INFORMATION:
            m_pDialogIcon->SetBitmap(wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX));
            break;
    }
    wxSizer *sz = GetSizer();
    if (sz) {
        wxSizer *bs = CreateSeparatedButtonSizer(m_lButtonStyle);
        if (bs) {
            sz->Add(bs, wxSizerFlags().Expand().Border(wxALL, 5));
        }
        if (m_lButtonStyle & wxYES_NO) {
            // Add missing event handler for "No" button.
            Connect(XRCID("wxID_NO"), wxEVT_COMMAND_BUTTON_CLICKED,
                    wxCommandEventHandler(SupressibleMessageDialog::OnNoClick));
        }
    }
    SetTitle(caption);
    m_pMessage->SetLabel(message);
    Fit();
}

/*!
 * SupessibleMessageDialog creator
 */

bool SupressibleMessageDialog::Create( wxWindow* parent, wxWindowID, const wxString&, const wxPoint&, const wxSize&, long)
{
    ////@begin SupressibleMessageDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end SupressibleMessageDialog creation
    return true;
}


/*!
 * SupressibleMessageDialog destructor
 */

SupressibleMessageDialog::~SupressibleMessageDialog()
{
    ////@begin SupressibleMessageDialog destruction
    ////@end SupressibleMessageDialog destruction
}


/*!
 * Member initialisation
 */

void SupressibleMessageDialog::Init()
{
    ////@begin SupressibleMessageDialog member initialisation
    m_bHide = false;
    m_lButtonStyle = 0;
    m_lIconStyle = 0;
    m_pDialogIcon = NULL;
    m_pMessage = NULL;
    ////@end SupressibleMessageDialog member initialisation
}


/*!
 * Control creation for SupessibleMessageDialog
 */

void SupressibleMessageDialog::CreateControls()
{    
    ////@begin SupressibleMessageDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_SUPRESSIBLEMESSAGEDIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pDialogIcon = XRCCTRL(*this, "wxID_STATIC", wxStaticBitmap);
    m_pMessage = XRCCTRL(*this, "wxID_DLGMESSAGE", wxStaticText);
    // Set validators
    if (FindWindow(XRCID("ID_CHECKBOX")))
        FindWindow(XRCID("ID_CHECKBOX"))->SetValidator( wxGenericValidator(& m_bHide) );
    ////@end SupressibleMessageDialog content construction

    // Create custom windows not generated automatically here.
    ////@begin SupressibleMessageDialog content initialisation
    ////@end SupressibleMessageDialog content initialisation
}


/*!
 * Should we show tooltips?
 */

bool SupressibleMessageDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap SupressibleMessageDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon SupressibleMessageDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}


void SupressibleMessageDialog::OnNoClick( wxCommandEvent& )
{
    EndModal(wxID_NO);
}

