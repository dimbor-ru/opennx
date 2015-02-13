// $Id: UnixImageSettingsDialog.h 708 2012-03-23 18:18:38Z felfert $
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

#ifndef _UNIXIMAGESETTINGSDIALOG_H_
#define _UNIXIMAGESETTINGSDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "UnixImageSettingsDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "UnixImageSettingsDialog_symbols.h"
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
#define ID_DIALOG_IMAGE_X11 10086
#define SYMBOL_UNIXIMAGESETTINGSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_UNIXIMAGESETTINGSDIALOG_TITLE _("Unix Image Settings - OpenNX")
#define SYMBOL_UNIXIMAGESETTINGSDIALOG_IDNAME ID_DIALOG_IMAGE_X11
#define SYMBOL_UNIXIMAGESETTINGSDIALOG_SIZE wxSize(114, 75)
#define SYMBOL_UNIXIMAGESETTINGSDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * UnixImageSettingsDialog class declaration
 */

class UnixImageSettingsDialog: public wxDialog
{    
    DECLARE_CLASS( UnixImageSettingsDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    UnixImageSettingsDialog( );
    UnixImageSettingsDialog( wxWindow* parent, wxWindowID id = SYMBOL_UNIXIMAGESETTINGSDIALOG_IDNAME, const wxString& caption = SYMBOL_UNIXIMAGESETTINGSDIALOG_TITLE, const wxPoint& pos = SYMBOL_UNIXIMAGESETTINGSDIALOG_POSITION, const wxSize& size = SYMBOL_UNIXIMAGESETTINGSDIALOG_SIZE, long style = SYMBOL_UNIXIMAGESETTINGSDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_UNIXIMAGESETTINGSDIALOG_IDNAME, const wxString& caption = SYMBOL_UNIXIMAGESETTINGSDIALOG_TITLE, const wxPoint& pos = SYMBOL_UNIXIMAGESETTINGSDIALOG_POSITION, const wxSize& size = SYMBOL_UNIXIMAGESETTINGSDIALOG_SIZE, long style = SYMBOL_UNIXIMAGESETTINGSDIALOG_STYLE );
    void SetConfig(MyXmlConfig *);

private:
    /// Creates the controls and sizers
    void CreateControls();

    void OnContextHelp(wxCommandEvent &);

////@begin UnixImageSettingsDialog event handler declarations

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_JPEG_AND_RGB
    void OnRadiobuttonX11JpegAndRgbSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_JPEG
    void OnRadiobuttonX11JpegSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_PNG
    void OnRadiobuttonX11PngSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for ID_RADIOBUTTON_X11_PLAIN
    void OnRadiobuttonX11PlainSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_X11_JPEG_CUSTOMQUALITY
    void OnCheckboxX11JpegCustomqualityClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOKClick( wxCommandEvent& event );

////@end UnixImageSettingsDialog event handler declarations

////@begin UnixImageSettingsDialog member function declarations

    bool GetBImageEncodingBoth() const { return m_bImageEncodingBoth ; }
    void SetBImageEncodingBoth(bool value) { m_bImageEncodingBoth = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end UnixImageSettingsDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    void UpdateDialogConstraints(bool fromWindow = true);

////@begin UnixImageSettingsDialog member variables
    wxCheckBox* m_pCtrlUseJpegQuality;
    wxSlider* m_pCtrlJpegQuality;
    wxCheckBox* m_pCtrlDisableSharedPixmaps;
private:
    bool m_bDisableBackingstore;
    bool m_bDisableComposite;
    bool m_bDisableRender;
    bool m_bDisableShmem;
    bool m_bDisableShpix;
    bool m_bImageEncodingBoth;
    bool m_bImageEncodingJpeg;
    bool m_bImageEncodingPNG;
    bool m_bImageEncodingPlainX;
    bool m_bUseJpegQuality;
    int m_iJpegQuality;
////@end UnixImageSettingsDialog member variables

    MyXmlConfig *m_pCfg;
};

#endif
    // _UNIXIMAGESETTINGSDIALOG_H_
