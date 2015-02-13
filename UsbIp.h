// $Id: UsbIp.h 709 2012-05-12 22:06:14Z felfert $
//
// Copyright (C) 2009 The OpenNX Team
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

#ifndef _USBIP_H_
#define _USBIP_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "UsbIp.h"
#endif

#ifdef SUPPORT_USBIP

# include <wx/event.h>
# include <wx/dynarray.h>

class wxSocketClient;
class wxSocketEvent;

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE(wxEVT_HOTPLUG, -1)
END_DECLARE_EVENT_TYPES()

class HotplugEvent : public wxEvent {
    public:
        HotplugEvent(wxEventType t = wxEVT_HOTPLUG)
        { m_eventType = t; }

        HotplugEvent(const HotplugEvent &event)
            : wxEvent(event)
            , m_sBusID(event.m_sBusID)
            , m_sCookie(event.m_sCookie)
            , m_iBusNum(event.m_iBusNum)
            , m_iDevNum(event.m_iDevNum)
            , m_iVendor(event.m_iVendor)
            , m_iProduct(event.m_iProduct) { }

        void SetBusID(wxString id) { m_sBusID = id; }
        const wxString & GetBusID() { return m_sBusID; }

        void SetCookie(wxString id) { m_sCookie = id; }
        const wxString & GetCookie() { return m_sCookie; }

        void SetBusNum(int i) { m_iBusNum = i; }
        int GetBusNum() { return m_iBusNum; }

        void SetDevNum(int i) { m_iDevNum = i; }
        int GetDevNum() { return m_iDevNum; }

        void SetProduct(int i) { m_iProduct = i; }
        int GetProduct() { return m_iProduct; }

        void SetVendor(int i) { m_iVendor = i; }
        int GetVendor() { return m_iVendor; }

        virtual wxEvent *Clone() const { return new HotplugEvent(*this); }

    private:
        wxString m_sBusID;
        wxString m_sCookie;
        int m_iBusNum;
        int m_iDevNum;
        int m_iVendor;
        int m_iProduct;

        DECLARE_DYNAMIC_CLASS(HotplugEvent)
};

typedef void (wxEvtHandler::*HotplugEventFunction)(HotplugEvent &);
#define HotplugEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(HotplugEventFunction, &func)
#define EVT_HOTPLUG(func) \
    wx__DECLARE_EVT0(wxEVT_HOTPLUG, HotplugEventHandler(func))


class UsbIpDevice : public wxObject {
    public:
        UsbIpDevice() : wxObject() { }
        virtual ~UsbIpDevice() {}

        wxString toString() const;
        wxString GetUsbBusID() const {
            return wxString::Format(wxT("%d-%d"), m_iUsbBusnum, m_iUsbDevnum);
        }
        const wxString& GetUsbIpBusID() const { return m_sUsbIpBusId; }

    private:
        friend class UsbIp;

        wxString m_sUsbIpBusId;
        wxString m_sDriver;
        wxString m_sConfig;
        int m_iUsbBusnum;
        int m_iUsbDevnum;
        int m_iVendorID;
        int m_iProductID;
};

WX_DECLARE_OBJARRAY(UsbIpDevice, ArrayOfUsbIpDevices);

class UsbIp : public wxEvtHandler {
    DECLARE_CLASS(UsbIp);
    DECLARE_EVENT_TABLE();

    public:
        UsbIp();
        virtual ~UsbIp();

        bool Connect(const wxString &);
        void SetSession(const wxString &);
        bool WaitForSession(int secs = 10);
        bool ExportDevice(const wxString &);
        bool UnexportDevice(const wxString &);
        bool RegisterHotplug();
        bool SendHotplugResponse(const wxString &);
        ArrayOfUsbIpDevices GetDevices();

        bool IsConnected() { return m_bConnected; }
        bool HasError();
        bool Wait(long, long);

        void SetEventHandler(wxEvtHandler *h) { m_pEvtHandler = h; }

    private:
        typedef enum {
            None,
            Terminating,
            Initializing,
            Idle,
            Exporting,
            UnExporting,
            ListSessions,
            ListDevices,
            Registering,
            Responding,
            Exported,
            UnExported,
            GotSessions,
            GotDevices,
            Registered,
            Responded,
        } tStates;

        virtual void OnSocketEvent(wxSocketEvent &);
        void parse(const wxString &);
        void parsesession(const wxString &);
        void parsedevice(const wxString &);
        bool findsession(const wxString &);
        void parsehev(const wxString &);
        bool waitforstate(tStates, long state = 5000);

// Compatibility with patched wxWidgets 2.8.12 on Ubuntu
#ifndef WX_ATTRIBUTE_PRINTF_1
# ifdef ATTRIBUTE_PRINTF_1
#  define WX_ATTRIBUTE_PRINTF_1 ATTRIBUTE_PRINTF_1
# endif
#endif
        bool send(const wxChar *fmt, ...) WX_ATTRIBUTE_PRINTF_1;

        wxEvtHandler *m_pEvtHandler;
        wxSocketClient *m_pSocketClient;
        wxString m_sSid;
        wxString m_sLineBuffer;
        bool m_bConnected;
        bool m_bError;
        int m_iLastError;
        tStates m_eState;
        wxArrayString m_aSessions;
        ArrayOfUsbIpDevices m_aDevices;
};

#endif
// SUPPORT_USBIP

#endif
// _USBIP_H_
