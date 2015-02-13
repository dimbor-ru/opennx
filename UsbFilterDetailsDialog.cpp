// $Id: UsbFilterDetailsDialog.cpp 605 2011-02-22 04:00:58Z felfert $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "UsbFilterDetailsDialog.h"
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

#include "UsbFilterDetailsDialog.h"
#include "MyValidator.h"
#include "Icon.h"
#ifdef APP_OPENNX
# include "opennxApp.h"
#endif
#ifdef APP_WATCHUSBIP
# include "watchUsbIpApp.h"
#endif
#include "LibUSB.h"

#include <wx/config.h>
#include <wx/cshelp.h>

#include "trace.h"

////@begin XPM images
////@end XPM images


/*!
 * UsbFilterDetailsDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS(UsbFilterDetailsDialog, wxDialog)


    /*!
     * UsbFilterDetailsDialog event table definition
     */

BEGIN_EVENT_TABLE( UsbFilterDetailsDialog, wxDialog )

    ////@begin UsbFilterDetailsDialog event table entries
    EVT_COMBOBOX( XRCID("ID_COMBOBOX_USBDEVS"), UsbFilterDetailsDialog::OnComboboxUsbdevsSelected )

    ////@end UsbFilterDetailsDialog event table entries

    EVT_MENU(wxID_CONTEXT_HELP, UsbFilterDetailsDialog::OnContextHelp)

END_EVENT_TABLE()


    /*!
     * UsbFilterDetailsDialog constructors
     */

UsbFilterDetailsDialog::UsbFilterDetailsDialog()
{
    Init();
}

UsbFilterDetailsDialog::UsbFilterDetailsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * UsbFilterDetailsDialog creator
 */

bool UsbFilterDetailsDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxUnusedVar(id);
    wxUnusedVar(caption);
    wxUnusedVar(pos);
    wxUnusedVar(size);
    wxUnusedVar(style);
    ////@begin UsbFilterDetailsDialog creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY|wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end UsbFilterDetailsDialog creation
    ::wxGetApp().EnableContextHelp(this);
    return true;
}

void UsbFilterDetailsDialog::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * UsbFilterDetailsDialog destructor
 */

UsbFilterDetailsDialog::~UsbFilterDetailsDialog()
{
    ////@begin UsbFilterDetailsDialog destruction
    ////@end UsbFilterDetailsDialog destruction
}


/*!
 * Member initialisation
 */

void UsbFilterDetailsDialog::Init()
{
    m_eMode = MODE_EDIT;
    ////@begin UsbFilterDetailsDialog member initialisation
    m_bStoreFilter = false;
    m_pCtrlHotplug = NULL;
    m_pCtrlRemember = NULL;
    m_pCtrlDevSelect = NULL;
    m_pCtrlDevlist = NULL;
    m_pCtrlVendorID = NULL;
    m_pCtrlProductID = NULL;
    m_pCtrlDevClass = NULL;
    m_pCtrlVendor = NULL;
    m_pCtrlProduct = NULL;
    m_pCtrlSerial = NULL;
    m_pCtrlMode = NULL;
    ////@end UsbFilterDetailsDialog member initialisation
}

void UsbFilterDetailsDialog::SetDeviceList(const ArrayOfUSBDevices &a)
{
    m_aUsbForwards.Clear();
    for (size_t i = 0; i < a.GetCount(); i++) {
        SharedUsbDevice dev;
        dev.m_iVendorID = a[i].GetVendorID();
        dev.m_iProductID = a[i].GetProductID();
        dev.m_iClass = a[i].GetDeviceClass();
        dev.m_sVendor = a[i].GetVendor();
        dev.m_sProduct = a[i].GetProduct();
        dev.m_sSerial = a[i].GetSerial();
        m_aUsbForwards.Add(dev);
    }
}

void UsbFilterDetailsDialog::SetDialogMode(eDialogMode mode)
{
    m_eMode = mode;
    switch (m_eMode) {
        case MODE_EDIT:
            if (m_pCtrlHotplug)
                m_pCtrlHotplug->Hide();
            if (m_pCtrlDevSelect)
                m_pCtrlDevSelect->Hide();
            break;
        case MODE_ADD:
            if (m_pCtrlHotplug)
                m_pCtrlHotplug->Hide();
            if (m_pCtrlDevSelect)
                m_pCtrlDevSelect->Show();
            if (m_pCtrlDevlist) {
                m_pCtrlDevlist->Clear();
                for (size_t i = 0; i < m_aUsbForwards.GetCount(); i++)
                    m_pCtrlDevlist->Append(m_aUsbForwards[i].toShortString());
            }
            break;
        case MODE_HOTPLUG:
            if (m_pCtrlHotplug)
                m_pCtrlHotplug->Show();
            if (m_pCtrlDevSelect)
                m_pCtrlDevSelect->Hide();
            SetTitle(_("New USB device - OpenNX"));
            break;
    }
    Fit();
}

/*!
 * Control creation for UsbFilterDetailsDialog
 */

void UsbFilterDetailsDialog::CreateControls()
{    
    ////@begin UsbFilterDetailsDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_USBFILTERDETAILSDIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlHotplug = XRCCTRL(*this, "ID_PANEL_HOTPLUG", wxPanel);
    m_pCtrlRemember = XRCCTRL(*this, "ID_CHECKBOX_FILTER_STORE", wxCheckBox);
    m_pCtrlDevSelect = XRCCTRL(*this, "ID_PANEL_DEVSELECT", wxPanel);
    m_pCtrlDevlist = XRCCTRL(*this, "ID_COMBOBOX_USBDEVS", wxComboBox);
    m_pCtrlVendorID = XRCCTRL(*this, "ID_TEXTCTRL_USB_VENDORID", wxTextCtrl);
    m_pCtrlProductID = XRCCTRL(*this, "ID_TEXTCTRL_USB_PRODUCTID", wxTextCtrl);
    m_pCtrlDevClass = XRCCTRL(*this, "ID_TEXTCTRL_USB_CLASS", wxTextCtrl);
    m_pCtrlVendor = XRCCTRL(*this, "ID_TEXTCTRL_USB_VENDOR", wxTextCtrl);
    m_pCtrlProduct = XRCCTRL(*this, "ID_TEXTCTRL_USB_PRODUCT", wxTextCtrl);
    m_pCtrlSerial = XRCCTRL(*this, "ID_TEXTCTRL_USB_SERIAL", wxTextCtrl);
    m_pCtrlMode = XRCCTRL(*this, "ID_COMBOBOX_MODE", wxComboBox);
    // Set validators
    if (FindWindow(XRCID("ID_CHECKBOX_FILTER_STORE")))
        FindWindow(XRCID("ID_CHECKBOX_FILTER_STORE"))->SetValidator( wxGenericValidator(& m_bStoreFilter) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USB_VENDORID")))
        FindWindow(XRCID("ID_TEXTCTRL_USB_VENDORID"))->SetValidator( MyValidator(MyValidator::MYVAL_HEX, & m_sVendorID) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USB_PRODUCTID")))
        FindWindow(XRCID("ID_TEXTCTRL_USB_PRODUCTID"))->SetValidator( MyValidator(MyValidator::MYVAL_HEX, & m_sProductID) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USB_CLASS")))
        FindWindow(XRCID("ID_TEXTCTRL_USB_CLASS"))->SetValidator( MyValidator(MyValidator::MYVAL_HEX, & m_sDeviceClass) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USB_VENDOR")))
        FindWindow(XRCID("ID_TEXTCTRL_USB_VENDOR"))->SetValidator( wxGenericValidator(& m_sVendor) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USB_PRODUCT")))
        FindWindow(XRCID("ID_TEXTCTRL_USB_PRODUCT"))->SetValidator( wxGenericValidator(& m_sProduct) );
    if (FindWindow(XRCID("ID_TEXTCTRL_USB_SERIAL")))
        FindWindow(XRCID("ID_TEXTCTRL_USB_SERIAL"))->SetValidator( wxGenericValidator(& m_sSerial) );
    ////@end UsbFilterDetailsDialog content construction

    SetDialogMode(m_eMode);
    // Create custom windows not generated automatically here.
    ////@begin UsbFilterDetailsDialog content initialisation
    ////@end UsbFilterDetailsDialog content initialisation
}


/*!
 * Should we show tooltips?
 */

bool UsbFilterDetailsDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap UsbFilterDetailsDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon UsbFilterDetailsDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_USBDEVS
 */

void UsbFilterDetailsDialog::OnComboboxUsbdevsSelected( wxCommandEvent& event )
{
    wxUnusedVar(event);
    int i = m_pCtrlDevlist->GetSelection();
    if (i != wxNOT_FOUND) {
        SharedUsbDevice dev = m_aUsbForwards[i];
        m_sVendorID = wxString::Format(wxT("%04X"), dev.m_iVendorID);
        m_sProductID = wxString::Format(wxT("%04X"), dev.m_iProductID);
        m_sDeviceClass = wxString::Format(wxT("%02X"), dev.m_iClass);
        m_sVendor = dev.m_sVendor;
        m_sProduct = dev.m_sProduct;
        m_sSerial = dev.m_sSerial;
        TransferDataToWindow();
    }
}

