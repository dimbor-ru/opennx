// $Id: VncImageSettingsDialog.cpp 708 2012-03-23 18:18:38Z felfert $
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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "VncImageSettingsDialog.h"
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

#include "VncImageSettingsDialog.h"
#include "MyXmlConfig.h"
#include "Icon.h"
#include "opennxApp.h"

#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images

/*!
 * VncImageSettingsDialog type definition
 */

IMPLEMENT_CLASS( VncImageSettingsDialog, wxDialog )

/*!
 * VncImageSettingsDialog event table definition
 */

BEGIN_EVENT_TABLE( VncImageSettingsDialog, wxDialog )

////@begin VncImageSettingsDialog event table entries
    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_VNC_JPEG_AND_RGB"), VncImageSettingsDialog::OnRadiobuttonVncJpegAndRgbSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_VNC_JPEG"), VncImageSettingsDialog::OnRadiobuttonVncJpegSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_VNC_PNG"), VncImageSettingsDialog::OnRadiobuttonVncPngSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_VNC_PLAIN"), VncImageSettingsDialog::OnRadiobuttonVncPlainSelected )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY"), VncImageSettingsDialog::OnCheckboxX11JpegCustomqualityClick )

    EVT_BUTTON( wxID_OK, VncImageSettingsDialog::OnOkClick )

////@end VncImageSettingsDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, VncImageSettingsDialog::OnContextHelp)

END_EVENT_TABLE()

/*!
 * VncImageSettingsDialog constructors
 */

VncImageSettingsDialog::VncImageSettingsDialog( )
    : m_pCfg(NULL)
{
}

VncImageSettingsDialog::VncImageSettingsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_pCfg(NULL)
{
    Create(parent, id, caption, pos, size, style);
}

void VncImageSettingsDialog::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

/*!
 * VncImageSettingsDialog creator
 */

bool VncImageSettingsDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin VncImageSettingsDialog member initialisation
    m_pCtrlUseJpegQuality = NULL;
    m_pCtrlJpegQuality = NULL;
////@end VncImageSettingsDialog member initialisation

    wxASSERT_MSG(m_pCfg, _T("VncImageSettingsDialog::Create: No configuration"));
    if (m_pCfg) {
        switch (m_pCfg->iGetVncImageEncoding()) {
            case -1:
                m_bImageEncodingBoth = false;
                m_bImageEncodingJpeg = true;
                m_bImageEncodingPNG = false;
                m_bImageEncodingPlainX = false;
                m_bUseJpegQuality = true;
                break;
            case 0:
                m_bImageEncodingBoth = false;
                m_bImageEncodingJpeg = false;
                m_bImageEncodingPNG = false;
                m_bImageEncodingPlainX = true;
                m_bUseJpegQuality = false;
                break;
            case 1:
                m_bImageEncodingBoth = false;
                m_bImageEncodingJpeg = true;
                m_bImageEncodingPNG = false;
                m_bImageEncodingPlainX = false;
                m_bUseJpegQuality = false;
                break;
            case 2:
                m_bImageEncodingBoth = false;
                m_bImageEncodingJpeg = false;
                m_bImageEncodingPNG = true;
                m_bImageEncodingPlainX = false;
                m_bUseJpegQuality = false;
                break;
            case 3:
                m_bImageEncodingBoth = true;
                m_bImageEncodingJpeg = false;
                m_bImageEncodingPNG = false;
                m_bImageEncodingPlainX = false;
                m_bUseJpegQuality = false;
                break;
            case 4:
                m_bImageEncodingBoth = true;
                m_bImageEncodingJpeg = false;
                m_bImageEncodingPNG = false;
                m_bImageEncodingPlainX = false;
                m_bUseJpegQuality = true;
                break;
        }
        m_iJpegQuality = m_pCfg->iGetVncJpegQuality();
    }

////@begin VncImageSettingsDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    Centre();
////@end VncImageSettingsDialog creation
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

/*!
 * Control creation for VncImageSettingsDialog
 */

void VncImageSettingsDialog::CreateControls()
{    
////@begin VncImageSettingsDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_IMAGE_VNC")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlUseJpegQuality = XRCCTRL(*this, "ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY", wxCheckBox);
    m_pCtrlJpegQuality = XRCCTRL(*this, "ID_SLIDER_X11_JPEG_QALITY", wxSlider);
    // Set validators
    if (FindWindow(XRCID("ID_RADIOBUTTON_VNC_JPEG_AND_RGB")))
        FindWindow(XRCID("ID_RADIOBUTTON_VNC_JPEG_AND_RGB"))->SetValidator( wxGenericValidator(& m_bImageEncodingBoth) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_VNC_JPEG")))
        FindWindow(XRCID("ID_RADIOBUTTON_VNC_JPEG"))->SetValidator( wxGenericValidator(& m_bImageEncodingJpeg) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_VNC_PNG")))
        FindWindow(XRCID("ID_RADIOBUTTON_VNC_PNG"))->SetValidator( wxGenericValidator(& m_bImageEncodingPNG) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_VNC_PLAIN")))
        FindWindow(XRCID("ID_RADIOBUTTON_VNC_PLAIN"))->SetValidator( wxGenericValidator(& m_bImageEncodingPlainX) );
    if (FindWindow(XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY")))
        FindWindow(XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY"))->SetValidator( wxGenericValidator(& m_bUseJpegQuality) );
    if (FindWindow(XRCID("ID_SLIDER_X11_JPEG_QALITY")))
        FindWindow(XRCID("ID_SLIDER_X11_JPEG_QALITY"))->SetValidator( wxGenericValidator(& m_iJpegQuality) );
////@end VncImageSettingsDialog content construction

    // Create custom windows not generated automatically here.

////@begin VncImageSettingsDialog content initialisation
////@end VncImageSettingsDialog content initialisation
    UpdateDialogConstraints(false);
}

/*!
 * Should we show tooltips?
 */

bool VncImageSettingsDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap VncImageSettingsDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon VncImageSettingsDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void VncImageSettingsDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

void VncImageSettingsDialog::UpdateDialogConstraints(bool fromWindow /* = true */)
{
    if (fromWindow)
        TransferDataFromWindow();
    if (m_bImageEncodingBoth || m_bImageEncodingJpeg) {
        m_pCtrlUseJpegQuality->Enable();
        m_pCtrlJpegQuality->Enable(m_bUseJpegQuality);
    } else {
        m_pCtrlUseJpegQuality->Disable();
        m_pCtrlJpegQuality->Disable();
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void VncImageSettingsDialog::OnOkClick( wxCommandEvent& event )
{
    wxASSERT_MSG(m_pCfg, _T("VncImageSettingsDialog::OnOkClick: No configuration"));
    if (m_pCfg) {
        TransferDataFromWindow();
        int ienc = 3;
        if (m_bImageEncodingBoth) {
            ienc = (m_bUseJpegQuality) ? 4 : 3;
        } else if (m_bImageEncodingJpeg) {
            ienc = (m_bUseJpegQuality) ? -1 : 1;
        } else if (m_bImageEncodingPNG) {
            ienc = 2;
        } else if (m_bImageEncodingPlainX) {
            ienc = 0;
        }
        m_pCfg->iSetVncImageEncoding(ienc);
        m_pCfg->iSetVncJpegQuality(m_iJpegQuality);
    }
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_JPEG_AND_RGB
 */

void VncImageSettingsDialog::OnRadiobuttonVncJpegAndRgbSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_JPEG
 */

void VncImageSettingsDialog::OnRadiobuttonVncJpegSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_PNG
 */

void VncImageSettingsDialog::OnRadiobuttonVncPngSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_PLAIN
 */

void VncImageSettingsDialog::OnRadiobuttonVncPlainSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY
 */

void VncImageSettingsDialog::OnCheckboxX11JpegCustomqualityClick( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}

