// $Id: LibUSB.cpp 605 2011-02-22 04:00:58Z felfert $
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "LibUSB.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "MyDynlib.h"
#include "LibUSB.h"

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/log.h>
#include <wx/arrimpl.cpp>

#include "trace.h"
ENABLE_TRACE;

WX_DEFINE_OBJARRAY(ArrayOfUSBDevices);

#ifdef SUPPORT_USBIP
#include <usb.h>

typedef void (*Tusb_init)(void);
typedef int (*Tusb_find_busses)(void);
typedef int (*Tusb_find_devices)(void);
typedef struct usb_bus *(*Tusb_get_busses)(void);
typedef int (*Tusb_get_string_simple)(usb_dev_handle *dev, int index, char *buf,
	size_t buflen);
typedef usb_dev_handle *(*Tusb_open)(struct usb_device *dev);
typedef int (*Tusb_close)(usb_dev_handle *dev);
#endif

USBDevice::USBDevice(int v, int p, unsigned char c) {
    m_iVendor = v;
    m_iProduct = p;
    m_iClass = c;
}

wxString USBDevice::toString() {
    wxString ret = wxString::Format(wxT("%d-%d %04X/%04X/%02X"),
            (int)m_iBusNum, (int)m_iDevNum, (int)m_iVendor,
            (int)m_iProduct, (int)m_iClass);
    if (!m_sVendor.IsEmpty())
        ret.Append(wxT(" ")).Append(m_sVendor);
    if (!m_sProduct.IsEmpty())
        ret.Append(wxT(" ")).Append(m_sProduct);
    if (!m_sSerial.IsEmpty())
        ret.Append(wxT(" #")).Append(m_sSerial);
    return ret;
}

wxString USBDevice::toShortString() {
    wxString ret = m_sVendor;
    if ((!ret.IsEmpty()) && (!m_sProduct.IsEmpty()))
        ret.Append(wxT(" "));
    ret.Append(m_sProduct);
    return ret;
}

#ifdef SUPPORT_USBIP
static wxString readSysfs(int bus, int dev, const wxString &attr)
{
    wxLogNull nolog;
    int i;
    // Find the device with the specified devnum
    for (i = 0; i < 16 ; ++i) {
        wxString path(wxString::Format(wxT("/sys/bus/usb/devices/%d-%d/devnum"), bus, i));
        wxFileInputStream fis(path);
        wxTextInputStream tis(fis);
        if (dev == tis.Read16S())
            break;
    }
    // Read the attribute
    wxString path(wxString::Format(wxT("/sys/bus/usb/devices/%d-%d/%s"), bus, i, attr.c_str()));
    wxFileInputStream fis(path);
    wxTextInputStream tis(fis);
    return tis.ReadLine();
}
#endif

void USB::adddev(MyDynamicLibrary *dll, struct usb_device *dev, unsigned char dclass)
{
#ifdef SUPPORT_USBIP
    usb_dev_handle *udev;
    char string[256];

    switch (dclass) {
        case USB_CLASS_HUB:
        case USB_CLASS_HID:
            return;
    }

    wxDYNLIB_FUNCTION(Tusb_open, usb_open, *dll);
    if (!pfnusb_open)
        return;
    wxDYNLIB_FUNCTION(Tusb_close, usb_close, *dll);
    if (!pfnusb_close)
        return;
    wxDYNLIB_FUNCTION(Tusb_get_string_simple, usb_get_string_simple, *dll);
    if (!pfnusb_get_string_simple)
        return;
    udev = pfnusb_open(dev);
    if (!udev)
        return;
    USBDevice d(dev->descriptor.idVendor, dev->descriptor.idProduct, dclass);
    if (dev->descriptor.iManufacturer) {
        if (0 < pfnusb_get_string_simple(udev,
                    dev->descriptor.iManufacturer, string, sizeof(string)))
            d.m_sVendor = wxConvUTF8.cMB2WX(string);
    }

    if (dev->descriptor.iProduct) {
        if (0 < pfnusb_get_string_simple(udev,
                    dev->descriptor.iProduct, string, sizeof(string)))
            d.m_sProduct = wxConvUTF8.cMB2WX(string);
    }
    if (dev->descriptor.iSerialNumber) {
        if (0 < pfnusb_get_string_simple(udev,
                    dev->descriptor.iSerialNumber, string, sizeof(string)))
            d.m_sSerial = wxConvUTF8.cMB2WX(string);
    }
    wxString tmp(dev->bus->dirname, wxConvUTF8);
    long lval;
    tmp.ToLong(&lval);
    d.m_iBusNum = lval;
    tmp = wxConvUTF8.cMB2WX(dev->filename);
    tmp.ToLong(&lval);
    d.m_iDevNum = lval;

    // Try fallback to sysfs, if libusb could not read info due to
    // permission problems.
    if (d.m_sVendor.IsEmpty()) {
        d.m_sVendor = readSysfs(d.m_iBusNum, d.m_iDevNum, wxT("manufacturer"));
    }
    if (d.m_sProduct.IsEmpty()) {
        d.m_sProduct = readSysfs(d.m_iBusNum, d.m_iDevNum, wxT("product"));
    }
    if (d.m_sSerial.IsEmpty()) {
        d.m_sSerial = readSysfs(d.m_iBusNum, d.m_iDevNum, wxT("serial"));
    }

    myLogTrace(MYTRACETAG, wxT("Device found: %s"), d.toString().c_str());
    m_aDevices.Add(d);
    pfnusb_close(udev);
#else
    wxUnusedVar(dll);
    wxUnusedVar(dev);
    wxUnusedVar(dclass);
#endif
}

void USB::usbscan(MyDynamicLibrary *dll)
{
#ifdef SUPPORT_USBIP
    struct usb_bus *bus;

    wxDYNLIB_FUNCTION(Tusb_find_busses, usb_find_busses, *dll);
    if (!pfnusb_find_busses)
        return;
    wxDYNLIB_FUNCTION(Tusb_find_devices, usb_find_devices, *dll);
    if (!pfnusb_find_devices)
        return;
    wxDYNLIB_FUNCTION(Tusb_get_busses, usb_get_busses, *dll);
    if (!pfnusb_get_busses)
        return;
    pfnusb_find_busses();
    pfnusb_find_devices();
    for (bus = pfnusb_get_busses(); bus; bus = bus->next) {
        struct usb_device *dev;
        for (dev = bus->devices; dev; dev = dev->next) {
            int devclass = dev->descriptor.bDeviceClass;

            if ((devclass == USB_CLASS_PER_INTERFACE) && (dev->config)) {
                for (int c = 0; c < dev->descriptor.bNumConfigurations; c++) {
                    struct usb_config_descriptor *cfg = &dev->config[c];
                    for (int i = 0; i < cfg->bNumInterfaces; i++) {
                        struct usb_interface *uif = &cfg->interface[i];
                        for (int ac = 0; ac < uif->num_altsetting; ac++)
                            adddev(dll, dev, uif->altsetting[ac].bInterfaceClass);
                    }
                }
            } else
                adddev(dll, dev, devclass);
        }
    }
#else
    wxUnusedVar(dll);
#endif
}

USB::USB() {
    m_bAvailable = false;
#ifdef SUPPORT_USBIP
    wxLogNull noerrors;
    MyDynamicLibrary dll;
    if (dll.Load(wxT("libusb"))) {
        wxDYNLIB_FUNCTION(Tusb_init, usb_init, dll);
        if (!pfnusb_init)
            return;
        wxDYNLIB_FUNCTION(Tusb_find_busses, usb_find_busses, dll);
        if (!pfnusb_find_busses)
            return;
        wxDYNLIB_FUNCTION(Tusb_find_devices, usb_find_devices, dll);
        if (!pfnusb_find_devices)
            return;
        wxDYNLIB_FUNCTION(Tusb_get_busses, usb_get_busses, dll);
        if (!pfnusb_get_busses)
            return;
        wxDYNLIB_FUNCTION(Tusb_open, usb_open, dll);
        if (!pfnusb_open)
            return;
        wxDYNLIB_FUNCTION(Tusb_close, usb_close, dll);
        if (!pfnusb_close)
            return;
        wxDYNLIB_FUNCTION(Tusb_get_string_simple, usb_get_string_simple, dll);
        if (!pfnusb_get_string_simple)
            return;
        pfnusb_init();
        m_bAvailable = true;
        usbscan(&dll);
    }
#endif
}

ArrayOfUSBDevices USB::GetDevices() {
    return m_aDevices;
}

bool USB::IsAvailable() {
    myLogTrace(MYTRACETAG, wxT("USB::IsAvailable() = %s"), m_bAvailable ? wxT("true") : wxT("false"));
    return m_bAvailable;
}

