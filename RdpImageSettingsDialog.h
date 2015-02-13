// $Id: RdpImageSettingsDialog.h 676 2012-02-05 16:18:21Z felfert $
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

#ifndef _RDPIMAGESETTINGSDIALOG_H_
#define _RDPIMAGESETTINGSDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "RdpImageSettingsDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "RdpImageSettingsDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations
class MyXmlConfig;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_IMAGE_RDP 10085
#define SYMBOL_RDPIMAGESETTINGSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_RDPIMAGESETTINGSDIALOG_TITLE _("RDP Image Settings - OpenNX")
#define SYMBOL_RDPIMAGESETTINGSDIALOG_IDNAME ID_DIALOG_IMAGE_RDP
#define SYMBOL_RDPIMAGESETTINGSDIALOG_SIZE wxDefaultSize
#define SYMBOL_RDPIMAGESETTINGSDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * RdpImageSettingsDialog class declaration
 */

class RdpImageSettingsDialog: public wxDialog
{    
    DECLARE_CLASS( RdpImageSettingsDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RdpImageSettingsDialog( );
    RdpImageSettingsDialog( wxWindow* parent, wxWindowID id = SYMBOL_RDPIMAGESETTINGSDIALOG_IDNAME, const wxString& caption = SYMBOL_RDPIMAGESETTINGSDIALOG_TITLE, const wxPoint& pos = SYMBOL_RDPIMAGESETTINGSDIALOG_POSITION, const wxSize& size = SYMBOL_RDPIMAGESETTINGSDIALOG_SIZE, long style = SYMBOL_RDPIMAGESETTINGSDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_RDPIMAGESETTINGSDIALOG_IDNAME, const wxString& caption = SYMBOL_RDPIMAGESETTINGSDIALOG_TITLE, const wxPoint& pos = SYMBOL_RDPIMAGESETTINGSDIALOG_POSITION, const wxSize& size = SYMBOL_RDPIMAGESETTINGSDIALOG_SIZE, long style = SYMBOL_RDPIMAGESETTINGSDIALOG_STYLE );
    void SetConfig(MyXmlConfig *);

private:
    /// Creates the controls and sizers
    void CreateControls();

    void OnContextHelp(wxCommandEvent &);

////@begin RdpImageSettingsDialog event handler declarations

    /// wxEVT_SCROLL_THUMBRELEASE event handler for ID_SLIDER_RDP_COLORS
    void OnSliderRdpColorsScrollThumbRelease( wxScrollEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_JPEG_AND_RGB
    void OnRadiobuttonRdpJpegAndRgbSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_JPEG
    void OnRadiobuttonRdpJpegSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_PNG
    void OnRadiobuttonRdpPngSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_RDP_PLAIN
    void OnRadiobuttonRdpPlainSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY
    void OnCheckboxX11JpegCustomqualityClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end RdpImageSettingsDialog event handler declarations

////@begin RdpImageSettingsDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end RdpImageSettingsDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    void UpdateDialogConstraints(bool fromWindow = true);

////@begin RdpImageSettingsDialog member variables
    wxCheckBox* m_pCtrlUseJpegQuality;
    wxSlider* m_pCtrlJpegQuality;
private:
    bool m_bRdpCache;
    int m_iRdpColors;
    bool m_bImageEncodingBoth;
    bool m_bImageEncodingJpeg;
    bool m_bImageEncodingPNG;
    bool m_bImageEncodingPlainX;
    bool m_bUseJpegQuality;
    int m_iJpegQuality;
////@end RdpImageSettingsDialog member variables

    MyXmlConfig *m_pCfg;
};

#endif
    // _RDPIMAGESETTINGSDIALOG_H_
