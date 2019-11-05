// $Id: TextViewer.cpp 451 2010-01-27 12:24:56Z felfert $
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TextViewer.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/mstream.h>
#include <wx/richtext/richtextxml.h>

#include "debugleak.h"

////@begin includes
////@end includes

#include "TextViewer.h"
#include "opennxApp.h"
#include "Icon.h"

////@begin XPM images
////@end XPM images

class myRichTextCtrl : public wxRichTextCtrl {
DECLARE_DYNAMIC_CLASS(myRichTextCtrl);

    public:

    bool DoLoadFile(const wxString& filename, wxRichTextFileType fileType = wxRICHTEXT_TYPE_XML)
    {
        bool success = false;
        wxFileSystem fs;
        wxFSFile *f = fs.OpenFile(filename);
        if (f) {
            wxInputStream *is = f->GetStream();
            if (is->IsOk()) {
                success = GetBuffer().LoadFile(*is, (wxRichTextFileType)fileType);
                if (success)
                    m_filename = filename;
            }
        }

        DiscardEdits();
        SetInsertionPoint(0);
        LayoutContent();
        PositionCaret();
        SetupScrollbars(true);
        Refresh(false);
        SendTextUpdatedEvent();

        if (success)
            return true;
        wxLogError(_("File couldn't be loaded."));
        return false;
    }
};
IMPLEMENT_DYNAMIC_CLASS(myRichTextCtrl, wxRichTextCtrl);

/*!
 * TextViewer type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TextViewer, wxDialog )

    /*!
     * TextViewer event table definition
     */

BEGIN_EVENT_TABLE( TextViewer, wxDialog )

    ////@begin TextViewer event table entries
    EVT_BUTTON( wxID_CLOSE, TextViewer::OnCloseClick )

    ////@end TextViewer event table entries

END_EVENT_TABLE()

    /*!
     * TextViewer constructors
     */

TextViewer::TextViewer( )
{
}

TextViewer::TextViewer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * TextViewer creator
 */

bool TextViewer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    ////@begin TextViewer member initialisation
    m_sFileName = wxT("");
    m_pRichTextCtrl = NULL;
    ////@end TextViewer member initialisation

    ////@begin TextViewer creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end TextViewer creation
    wxUnusedVar(style);
    wxUnusedVar(size);
    wxUnusedVar(pos);
    wxUnusedVar(caption);
    wxUnusedVar(id);
    return true;
}

/*!
 * Control creation for TextViewer
 */

void TextViewer::CreateControls()
{   
    ////@begin TextViewer content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_TEXTVIEWER")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pRichTextCtrl = XRCCTRL(*this, "ID_RICHTEXTCTRL", myRichTextCtrl);
    ////@end TextViewer content construction

    m_pRichTextCtrl->GetBuffer().AddHandler(new wxRichTextXMLHandler());
    m_pRichTextCtrl->SetHandlerFlags(wxRICHTEXT_HANDLER_CONVERT_FACENAMES);
 
    ////@begin TextViewer content initialisation
    ////@end TextViewer content initialisation
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CLOSE
 */

void TextViewer::OnCloseClick( wxCommandEvent& event )
{
    Destroy();
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool TextViewer::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TextViewer::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon TextViewer::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

    bool
TextViewer::LoadFile(const wxString &sFileName)
{
    bool ret = false;
    m_sFileName = sFileName;
    wxFileName fn(sFileName);
    {
        wxLogNull l;
        ret = m_pRichTextCtrl->LoadFile(m_sFileName, wxRICHTEXT_TYPE_XML);
    }
    if (ret)
        SetTitle(fn.GetName());
    return ret;
}
