// $Id: UnixImageSettingsDialog.cpp 708 2012-03-23 18:18:38Z felfert $
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
#pragma implementation "UnixImageSettingsDialog.h"
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

#include "UnixImageSettingsDialog.h"
#include "MyXmlConfig.h"
#include "Icon.h"
#include "opennxApp.h"

#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images

/*!
 * UnixImageSettingsDialog type definition
 */

IMPLEMENT_CLASS( UnixImageSettingsDialog, wxDialog )

/*!
 * UnixImageSettingsDialog event table definition
 */

BEGIN_EVENT_TABLE( UnixImageSettingsDialog, wxDialog )

////@begin UnixImageSettingsDialog event table entries
    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_JPEG_AND_RGB"), UnixImageSettingsDialog::OnRadiobuttonX11JpegAndRgbSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_JPEG"), UnixImageSettingsDialog::OnRadiobuttonX11JpegSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_PNG"), UnixImageSettingsDialog::OnRadiobuttonX11PngSelected )

    EVT_RADIOBUTTON( XRCID("ID_RADIOBUTTON_X11_PLAIN"), UnixImageSettingsDialog::OnRadiobuttonX11PlainSelected )

    EVT_CHECKBOX( XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY"), UnixImageSettingsDialog::OnCheckboxX11JpegCustomqualityClick )

    EVT_BUTTON( wxID_OK, UnixImageSettingsDialog::OnOKClick )

////@end UnixImageSettingsDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, UnixImageSettingsDialog::OnContextHelp)

END_EVENT_TABLE()

/*!
 * UnixImageSettingsDialog constructors
 */

UnixImageSettingsDialog::UnixImageSettingsDialog( )
    : m_pCfg(NULL)
{
}

UnixImageSettingsDialog::UnixImageSettingsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_pCfg(NULL)
{
    Create(parent, id, caption, pos, size, style);
}

void UnixImageSettingsDialog::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

/*!
 * UnixImageSettingsDialog creator
 */

bool UnixImageSettingsDialog::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin UnixImageSettingsDialog member initialisation
    m_bImageEncodingBoth = true;
    m_pCtrlUseJpegQuality = NULL;
    m_pCtrlJpegQuality = NULL;
    m_pCtrlDisableSharedPixmaps = NULL;
////@end UnixImageSettingsDialog member initialisation

    wxASSERT_MSG(m_pCfg, _T("UnixImageSettingsDialog::Create: No configuration"));
    if (m_pCfg) {
        switch (m_pCfg->iGetImageEncoding()) {
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
        m_iJpegQuality = m_pCfg->iGetJpegQuality();

        m_bDisableRender = m_pCfg->bGetDisableRender();
        m_bDisableBackingstore = m_pCfg->bGetDisableBackingstore();
        m_bDisableComposite = m_pCfg->bGetDisableComposite();
        m_bDisableShmem = m_pCfg->bGetDisableShmem();
        m_bDisableShpix = m_pCfg->bGetDisableShpix();
    }

    ////@begin UnixImageSettingsDialog creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY|wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    Centre();
    ////@end UnixImageSettingsDialog creation
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

/*!
 * Control creation for UnixImageSettingsDialog
 */

void UnixImageSettingsDialog::CreateControls()
{    
    ////@begin UnixImageSettingsDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_IMAGE_X11")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlUseJpegQuality = XRCCTRL(*this, "ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY", wxCheckBox);
    m_pCtrlJpegQuality = XRCCTRL(*this, "ID_SLIDER_X11_JPEG_QALITY", wxSlider);
    m_pCtrlDisableSharedPixmaps = XRCCTRL(*this, "ID_CHECKBOX_PERF_DISABLESHPIX", wxCheckBox);
    // Set validators
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_JPEG_AND_RGB")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_JPEG_AND_RGB"))->SetValidator( wxGenericValidator(& m_bImageEncodingBoth) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_JPEG")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_JPEG"))->SetValidator( wxGenericValidator(& m_bImageEncodingJpeg) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_PNG")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_PNG"))->SetValidator( wxGenericValidator(& m_bImageEncodingPNG) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_X11_PLAIN")))
        FindWindow(XRCID("ID_RADIOBUTTON_X11_PLAIN"))->SetValidator( wxGenericValidator(& m_bImageEncodingPlainX) );
    if (FindWindow(XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY")))
        FindWindow(XRCID("ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY"))->SetValidator( wxGenericValidator(& m_bUseJpegQuality) );
    if (FindWindow(XRCID("ID_SLIDER_X11_JPEG_QALITY")))
        FindWindow(XRCID("ID_SLIDER_X11_JPEG_QALITY"))->SetValidator( wxGenericValidator(& m_iJpegQuality) );
    if (FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLERENDER")))
        FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLERENDER"))->SetValidator( wxGenericValidator(& m_bDisableRender) );
    if (FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLEBACKINGSTORE")))
        FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLEBACKINGSTORE"))->SetValidator( wxGenericValidator(& m_bDisableBackingstore) );
    if (FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLECOMPOSITE")))
        FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLECOMPOSITE"))->SetValidator( wxGenericValidator(& m_bDisableComposite) );
    if (FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLESHMEM")))
        FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLESHMEM"))->SetValidator( wxGenericValidator(& m_bDisableShmem) );
    if (FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLESHPIX")))
        FindWindow(XRCID("ID_CHECKBOX_PERF_DISABLESHPIX"))->SetValidator( wxGenericValidator(& m_bDisableShpix) );
    ////@end UnixImageSettingsDialog content construction

    // Create custom windows not generated automatically here.

    ////@begin UnixImageSettingsDialog content initialisation
    ////@end UnixImageSettingsDialog content initialisation
    UpdateDialogConstraints(false);
}

/*!
 * Should we show tooltips?
 */

bool UnixImageSettingsDialog::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap UnixImageSettingsDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon UnixImageSettingsDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void UnixImageSettingsDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

void UnixImageSettingsDialog::UpdateDialogConstraints(bool fromWindow /* = true */)
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
    if (m_bDisableShmem) {
        m_pCtrlDisableSharedPixmaps->SetValue(true);
        m_pCtrlDisableSharedPixmaps->Disable();
    } else {
        m_pCtrlDisableSharedPixmaps->Enable();
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void UnixImageSettingsDialog::OnOKClick( wxCommandEvent& event )
{
    wxASSERT_MSG(m_pCfg, _T("UnixImageSettingsDialog::OnOkClick: No configuration"));
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
        m_pCfg->iSetImageEncoding(ienc);
        m_pCfg->iSetJpegQuality(m_iJpegQuality);
        m_pCfg->bSetDisableRender(m_bDisableRender);
        m_pCfg->bSetDisableBackingstore(m_bDisableBackingstore);
        m_pCfg->bSetDisableComposite(m_bDisableComposite);
        m_pCfg->bSetDisableShmem(m_bDisableShmem);
        m_pCfg->bSetDisableShpix(m_bDisableShpix);
    }
    event.Skip();
}



/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_JPEG_AND_RGB
 */

void UnixImageSettingsDialog::OnRadiobuttonX11JpegAndRgbSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_JPEG
 */

void UnixImageSettingsDialog::OnRadiobuttonX11JpegSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_PNG
 */

void UnixImageSettingsDialog::OnRadiobuttonX11PngSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_PLAIN
 */

void UnixImageSettingsDialog::OnRadiobuttonX11PlainSelected( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY
 */

void UnixImageSettingsDialog::OnCheckboxX11JpegCustomqualityClick( wxCommandEvent& event )
{
    UpdateDialogConstraints();
    event.Skip();
}

