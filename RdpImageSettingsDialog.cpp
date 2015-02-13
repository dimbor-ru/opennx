// $Id: RdpImageSettingsDialog.cpp 676 2012-02-05 16:18:21Z felfert $
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
# include "config.h"
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "RdpImageSettingsDialog.h"
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

#include "RdpImageSettingsDialog.h"
#include "MyXmlConfig.h"
#include "Icon.h"
#include "opennxApp.h"

#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images

/*!
 * RdpImageSettingsDialog type definition
 */

IMPLEMENT_CLASS( RdpImageSettingsDialog, wxDialog )

/*!
 * RdpImageSettingsDialog event table definition
 */

BEGIN_EVENT_TABLE( RdpImageSettingsDialog, wxDialog )

////@begin RdpImageSettingsDialog event table entries
    EVT_COMMAND_SCROLL_THUMBRELEASE( XRCID("ID_SLIDER_RDP_COLORS"), RdpImageSettingsDialog::OnSliderRdpColorsScrollThumbRelease )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_JPEG_AND_RGB"), RdpImageSettingsDialog::OnRadiobuttonRdpJpegAndRgbSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_JPEG"), RdpImageSettingsDialog::OnRadiobuttonRdpJpegSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_PNG"), RdpImageSettingsDialog::OnRadiobuttonRdpPngSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_RDP_PLAIN"), RdpImageSettingsDialog::OnRadiobuttonRdpPlainSelected )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY"), RdpImageSettingsDialog::OnCheckboxX11JpegCustomqualityClick )

    EVT_BUTTON( wxID_OK, RdpImageSettingsDialog::OnOkClick )

////@end RdpImageSettingsDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, RdpImageSettingsDialog::OnContextHelp)

END_EVENT_TABLE()

/*!
 * RdpImageSettingsDialog constructors
 */

RdpImageSettingsDialog::RdpImageSettingsDialog( )
    : m_pCfg(NULL)
{
}

RdpImageSettingsDialog::RdpImageSettingsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_pCfg(NULL)
{
    Create(parent, id, caption, pos, size, style);
}

void RdpImageSettingsDialog::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

/*!
 * RdpImageSettingsDialog creator
 */

bool RdpImageSettingsDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin RdpImageSettingsDialog member initialisation
    m_iRdpColors = 0;
    m_pCtrlUseJpegQuality = NULL;
    m_pCtrlJpegQuality = NULL;
////@end RdpImageSettingsDialog member initialisation

    wxASSERT_MSG(m_pCfg, _T("RdpImageSettingsDialog::Create: No configuration"));
    if (m_pCfg) {
        switch (m_pCfg->iGetRdpImageEncoding()) {
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
        m_iJpegQuality = m_pCfg->iGetRdpJpegQuality();
        m_bRdpCache = m_pCfg->bGetRdpCache();
        m_iRdpColors = m_pCfg->iGetRdpColors();
    }

////@begin RdpImageSettingsDialog creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY|wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end RdpImageSettingsDialog creation
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

/*!
 * Control creation for RdpImageSettingsDialog
 */

void RdpImageSettingsDialog::CreateControls()
{    
////@begin RdpImageSettingsDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_IMAGE_RDP")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlUseJpegQuality = XRCCTRL(*this, "ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY", wxCheckBox);
    m_pCtrlJpegQuality = XRCCTRL(*this, "ID_SLIDER_X11_JPEG_QALITY", wxSlider);
    // Set validators
    if (FindWindow(XRCID("ID_SLIDER_RDP_COLORS")))
        FindWindow(XRCID("ID_SLIDER_RDP_COLORS"))->SetValidator( wxGenericValidator(& m_iRdpColors) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_JPEG_AND_RGB")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_JPEG_AND_RGB"))->SetValidator( wxGenericValidator(& m_bImageEncodingBoth) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_JPEG")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_JPEG"))->SetValidator( wxGenericValidator(& m_bImageEncodingJpeg) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_PNG")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_PNG"))->SetValidator( wxGenericValidator(& m_bImageEncodingPNG) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_RDP_PLAIN")))
        FindWindow(XRCID("ID_RADIOBUTTON_RDP_PLAIN"))->SetValidator( wxGenericValidator(& m_bImageEncodingPlainX) );
    if (FindWindow(XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY")))
        FindWindow(XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY"))->SetValidator( wxGenericValidator(& m_bUseJpegQuality) );
    if (FindWindow(XRCID("ID_SLIDER_X11_JPEG_QALITY")))
        FindWindow(XRCID("ID_SLIDER_X11_JPEG_QALITY"))->SetValidator( wxGenericValidator(& m_iJpegQuality) );
    if (FindWindow(XRCID("ID_CHECKBOX_RDP_IMGCACHE")))
        FindWindow(XRCID("ID_CHECKBOX_RDP_IMGCACHE"))->SetValidator( wxGenericValidator(& m_bRdpCache) );
////@end RdpImageSettingsDialog content construction

    // Create custom windows not generated automatically here.

////@begin RdpImageSettingsDialog content initialisation
////@end RdpImageSettingsDialog content initialisation
    UpdateDialogConstraints(false);
}

/*!
 * Should we show tooltips?
 */

bool RdpImageSettingsDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap RdpImageSettingsDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon RdpImageSettingsDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void RdpImageSettingsDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

void RdpImageSettingsDialog::UpdateDialogConstraints(bool fromWindow /* = true */)
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

void RdpImageSettingsDialog::OnOkClick( wxCommandEvent& event )
{
    wxASSERT_MSG(m_pCfg, _T("RdpImageSettingsDialog::OnOkClick: No configuration"));
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
        m_pCfg->iSetRdpImageEncoding(ienc);
        m_pCfg->iSetRdpJpegQuality(m_iJpegQuality);
        m_pCfg->iSetRdpColors(m_iRdpColors);
        m_pCfg->bSetRdpCache(m_bRdpCache);
    }
    event.Skip();
}


/*!
 * wxEVT_SCROLL_THUMBRELEASE event handler for ID_SLIDER_RDP_COLORS
 */

void RdpImageSettingsDialog::OnSliderRdpColorsScrollThumbRelease( wxScrollEvent& event )
{
    wxSlider *sl = wxDynamicCast(event.GetEventObject(), wxSlider);
    // Stick slider to nearest integer position
    int p = event.GetPosition();
    sl->SetValue((p > 0) ?  p - 1 : p + 1);
    sl->SetValue(p);
    event.Skip();
}



/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_JPEG_AND_RGB
 */

void RdpImageSettingsDialog::OnRadiobuttonRdpJpegAndRgbSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_JPEG
 */

void RdpImageSettingsDialog::OnRadiobuttonRdpJpegSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_PNG
 */

void RdpImageSettingsDialog::OnRadiobuttonRdpPngSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_PLAIN
 */

void RdpImageSettingsDialog::OnRadiobuttonRdpPlainSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY
 */

void RdpImageSettingsDialog::OnCheckboxX11JpegCustomqualityClick( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}

