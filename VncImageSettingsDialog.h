// $Id: VncImageSettingsDialog.h 676 2012-02-05 16:18:21Z felfert $
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

#ifndef _VNCIMAGESETTINGSDIALOG_H_
#define _VNCIMAGESETTINGSDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "VncImageSettingsDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "VncImageSettingsDialog_symbols.h"
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
#define ID_DIALOG_IMAGE_VNC 10078
#define SYMBOL_VNCIMAGESETTINGSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_VNCIMAGESETTINGSDIALOG_TITLE _("VNC Image Settings - OpenNX")
#define SYMBOL_VNCIMAGESETTINGSDIALOG_IDNAME ID_DIALOG_IMAGE_VNC
#define SYMBOL_VNCIMAGESETTINGSDIALOG_SIZE wxDefaultSize
#define SYMBOL_VNCIMAGESETTINGSDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * VncImageSettingsDialog class declaration
 */

class VncImageSettingsDialog: public wxDialog
{    
    DECLARE_CLASS( VncImageSettingsDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    VncImageSettingsDialog( );
    VncImageSettingsDialog( wxWindow* parent, wxWindowID id = SYMBOL_VNCIMAGESETTINGSDIALOG_IDNAME, const wxString& caption = SYMBOL_VNCIMAGESETTINGSDIALOG_TITLE, const wxPoint& pos = SYMBOL_VNCIMAGESETTINGSDIALOG_POSITION, const wxSize& size = SYMBOL_VNCIMAGESETTINGSDIALOG_SIZE, long style = SYMBOL_VNCIMAGESETTINGSDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_VNCIMAGESETTINGSDIALOG_IDNAME, const wxString& caption = SYMBOL_VNCIMAGESETTINGSDIALOG_TITLE, const wxPoint& pos = SYMBOL_VNCIMAGESETTINGSDIALOG_POSITION, const wxSize& size = SYMBOL_VNCIMAGESETTINGSDIALOG_SIZE, long style = SYMBOL_VNCIMAGESETTINGSDIALOG_STYLE );
    void SetConfig(MyXmlConfig *);

private:
    /// Creates the controls and sizers
    void CreateControls();

    void OnContextHelp(wxCommandEvent &);

////@begin VncImageSettingsDialog event handler declarations

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_JPEG_AND_RGB
    void OnRadiobuttonVncJpegAndRgbSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_JPEG
    void OnRadiobuttonVncJpegSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_PNG
    void OnRadiobuttonVncPngSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_VNC_PLAIN
    void OnRadiobuttonVncPlainSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY
    void OnCheckboxX11JpegCustomqualityClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end VncImageSettingsDialog event handler declarations

////@begin VncImageSettingsDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end VncImageSettingsDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    void UpdateDialogConstraints(bool fromWindow = true);

////@begin VncImageSettingsDialog member variables
    wxCheckBox* m_pCtrlUseJpegQuality;
    wxSlider* m_pCtrlJpegQuality;
private:
    bool m_bImageEncodingBoth;
    bool m_bImageEncodingJpeg;
    bool m_bImageEncodingPNG;
    bool m_bImageEncodingPlainX;
    bool m_bUseJpegQuality;
    int m_iJpegQuality;
////@end VncImageSettingsDialog member variables

    MyXmlConfig *m_pCfg;
};

#endif
    // _VNCIMAGESETTINGSDIALOG_H_
