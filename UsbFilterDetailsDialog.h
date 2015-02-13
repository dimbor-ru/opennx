// $Id: UsbFilterDetailsDialog.h 605 2011-02-22 04:00:58Z felfert $
//
// Copyright (C) 2009 The OpenNX team
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

#ifndef _USBFILTERDETAILSDIALOG_H_
#define _USBFILTERDETAILSDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "UsbFilterDetailsDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "UsbFilterDetailsDialog_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/valgen.h"
#include "wx/statline.h"
////@end includes

#include "MyXmlConfig.h"

class ArrayOfUSBDevices;

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_USBFILTERDETAILSDIALOG 10168
#define SYMBOL_USBFILTERDETAILSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_USBFILTERDETAILSDIALOG_TITLE _("USB Filter Details - OpenNX")
#define SYMBOL_USBFILTERDETAILSDIALOG_IDNAME ID_USBFILTERDETAILSDIALOG
#define SYMBOL_USBFILTERDETAILSDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_USBFILTERDETAILSDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * UsbFilterDetailsDialog class declaration
 */

class UsbFilterDetailsDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( UsbFilterDetailsDialog )
    DECLARE_EVENT_TABLE()

public:
    typedef enum {
        MODE_EDIT = 0,
        MODE_ADD,
        MODE_HOTPLUG
    } eDialogMode;

    /// Constructors
    UsbFilterDetailsDialog();
    UsbFilterDetailsDialog( wxWindow* parent, wxWindowID id = SYMBOL_USBFILTERDETAILSDIALOG_IDNAME, const wxString& caption = SYMBOL_USBFILTERDETAILSDIALOG_TITLE, const wxPoint& pos = SYMBOL_USBFILTERDETAILSDIALOG_POSITION, const wxSize& size = SYMBOL_USBFILTERDETAILSDIALOG_SIZE, long style = SYMBOL_USBFILTERDETAILSDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_USBFILTERDETAILSDIALOG_IDNAME, const wxString& caption = SYMBOL_USBFILTERDETAILSDIALOG_TITLE, const wxPoint& pos = SYMBOL_USBFILTERDETAILSDIALOG_POSITION, const wxSize& size = SYMBOL_USBFILTERDETAILSDIALOG_SIZE, long style = SYMBOL_USBFILTERDETAILSDIALOG_STYLE );

    /// Destructor
    ~UsbFilterDetailsDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    void OnContextHelp(wxCommandEvent &);

////@begin UsbFilterDetailsDialog event handler declarations

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_USBDEVS
    void OnComboboxUsbdevsSelected( wxCommandEvent& event );

////@end UsbFilterDetailsDialog event handler declarations

////@begin UsbFilterDetailsDialog member function declarations

    wxString GetVendorID() const { return m_sVendorID ; }
    void SetVendorID(wxString value) { m_sVendorID = value ; }

    wxString GetProductID() const { return m_sProductID ; }
    void SetProductID(wxString value) { m_sProductID = value ; }

    wxString GetDeviceClass() const { return m_sDeviceClass ; }
    void SetDeviceClass(wxString value) { m_sDeviceClass = value ; }

    wxString GetVendor() const { return m_sVendor ; }
    void SetVendor(wxString value) { m_sVendor = value ; }

    wxString GetProduct() const { return m_sProduct ; }
    void SetProduct(wxString value) { m_sProduct = value ; }

    wxString GetSerial() const { return m_sSerial ; }
    void SetSerial(wxString value) { m_sSerial = value ; }

    bool GetStoreFilter() const { return m_bStoreFilter ; }
    void SetStoreFilter(bool value) { m_bStoreFilter = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end UsbFilterDetailsDialog member function declarations

    void SetDeviceList(const ArrayOfUSBDevices &);

    bool GetForwarding() {
        return ((NULL != m_pCtrlMode) &&
                (m_pCtrlMode->GetSelection() == 1));
    }
    void SetForwarding(bool b) {
        if (NULL != m_pCtrlMode)
            m_pCtrlMode->Select(b ? 1 : 0);
    }

    /// Should we show tooltips?
    static bool ShowToolTips();
    void SetDialogMode(eDialogMode);

////@begin UsbFilterDetailsDialog member variables
    wxPanel* m_pCtrlHotplug;
    wxCheckBox* m_pCtrlRemember;
    wxPanel* m_pCtrlDevSelect;
    wxComboBox* m_pCtrlDevlist;
    wxTextCtrl* m_pCtrlVendorID;
    wxTextCtrl* m_pCtrlProductID;
    wxTextCtrl* m_pCtrlDevClass;
    wxTextCtrl* m_pCtrlVendor;
    wxTextCtrl* m_pCtrlProduct;
    wxTextCtrl* m_pCtrlSerial;
    wxComboBox* m_pCtrlMode;
private:
    wxString m_sVendorID;
    wxString m_sProductID;
    wxString m_sDeviceClass;
    wxString m_sVendor;
    wxString m_sProduct;
    wxString m_sSerial;
    bool m_bStoreFilter;
////@end UsbFilterDetailsDialog member variables

    eDialogMode m_eMode;
    ArrayOfUsbForwards m_aUsbForwards;
};

#endif
    // _USBFILTERDETAILSDIALOG_H_
