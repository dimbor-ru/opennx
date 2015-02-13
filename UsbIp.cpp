// $Id: UsbIp.cpp 451 2010-01-27 12:24:56Z felfert $
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
#pragma implementation "UsbIp.h"
#endif

#ifdef SUPPORT_USBIP

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "UsbIp.h"

#include <wx/log.h>
#include <wx/socket.h>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>
#include <wx/regex.h>
#include <wx/arrimpl.cpp>

#ifdef APP_OPENNX
# include "opennxApp.h"
#endif
#ifdef APP_WATCHUSBIP
# include "watchUsbIpApp.h"
#endif

#include "trace.h"
ENABLE_TRACE;

IMPLEMENT_DYNAMIC_CLASS(HotplugEvent, wxEvent)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_HOTPLUG)

WX_DEFINE_OBJARRAY(ArrayOfUsbIpDevices);

wxString UsbIpDevice::toString() const {
    wxString ret = wxString::Format(wxT("%s %d-%d %04X/%04X %s"),
            m_sUsbIpBusId.c_str(), m_iUsbBusnum, m_iUsbDevnum,
            m_iVendorID, m_iProductID, m_sDriver.c_str());
    return ret;
}

IMPLEMENT_CLASS(UsbIp, wxEvtHandler);

#define SOCKET_ID 55555

BEGIN_EVENT_TABLE(UsbIp, wxEvtHandler)
EVT_SOCKET(SOCKET_ID, UsbIp::OnSocketEvent)
END_EVENT_TABLE()

UsbIp::UsbIp()
    : m_pEvtHandler(NULL)
{
    m_pSocketClient = new wxSocketClient();
    m_pSocketClient->SetEventHandler(*this, SOCKET_ID);
    m_pSocketClient->SetNotify(
            wxSOCKET_INPUT_FLAG|wxSOCKET_CONNECTION_FLAG|wxSOCKET_LOST_FLAG);
    m_pSocketClient->Notify(true);
    m_pSocketClient->SetTimeout(10); // 10 sec.
    m_bConnected = false;
    m_bError = false;
    m_sSid = wxEmptyString;
    m_sLineBuffer = wxEmptyString;
    m_eState = None;
}

UsbIp::~UsbIp()
{
    if (m_pSocketClient) {
        if (m_bConnected) {
            m_eState = Terminating;
            if (send(wxT("quit\n")))
                waitforstate(None);
        }
        m_pSocketClient->Destroy();
    }
    m_pSocketClient = NULL;
}

bool UsbIp::HasError()
{
    return m_pSocketClient->Error();
}

bool UsbIp::Wait(long sec = -1, long msec = 0)
{
    return m_pSocketClient->Wait(sec, msec);
}

bool UsbIp::Connect(const wxString &socketPath)
{
#ifdef __WXMSW__
    m_bConnected = false;
# warning win32 implementation missing
#else
    wxUNIXaddress addr;
    addr.Filename(socketPath);
    ::myLogTrace(MYTRACETAG, wxT("Connecting to %s"), socketPath.c_str());
    m_pSocketClient->Connect(addr, false);
    // It's a local unix socket and the server must be running already,
    // so 5 secs should be more than enough.
    m_pSocketClient->WaitOnConnect(5);
    m_bConnected = m_pSocketClient->IsConnected();
    ::myLogTrace(MYTRACETAG, wxT("m_bConnected = %d"), m_bConnected);
    if (m_bConnected)
        m_eState = Initializing;
    else
        m_pSocketClient->Close();
#endif
    return m_bConnected;
}

void UsbIp::SetSession(const wxString &sid)
{
    m_sSid = sid;
}

bool UsbIp::WaitForSession(int secs /* = 10 */)
{
    long timeout = secs * 1000;
    if (m_sSid.IsEmpty())
        return false;
    if (Initializing > m_eState)
        return false;
    if (!waitforstate(Idle))
        return false;
    wxStopWatch sw;
    ::myLogTrace(MYTRACETAG, wxT("waiting for session ..."));
    while (!findsession(m_sSid)) {
        ::wxGetApp().Yield(true);
        wxLog::FlushActive();
        m_pSocketClient->Wait(0, 1000);
        if (0 < timeout) {
            if (sw.Time() > timeout) {
                ::myLogTrace(MYTRACETAG, wxT("waitforsession timed out"));
                return false;
            }
        }
    }
    return true;
}

bool UsbIp::ExportDevice(const wxString &busid)
{
    if (m_sSid.IsEmpty())
        return false;
    if (Initializing > m_eState)
        return false;
    if (!waitforstate(Idle))
        return false;
    ::myLogTrace(MYTRACETAG, wxT("Exporting ..."));
    if (!findsession(m_sSid)) {
        ::myLogTrace(MYTRACETAG, wxT("Session not found"));
        return false;
    }
    m_eState = Exporting;
    if (!send(wxT("export %s %s\n"), busid.c_str(), m_sSid.c_str())) {
        m_eState = Idle;
        return false;
    }
    if (!waitforstate(Exported)) {
        m_eState = Idle;
        if (404 == m_iLastError) {
            ::wxLogWarning(_("USB device is already exported in another session"));
            return true; // Prevent calling app from showing an error.
        }
        return false;
    }
    m_eState = Idle;
    return true;
}

bool UsbIp::UnexportDevice(const wxString &busid)
{
    if (m_sSid.IsEmpty())
        return false;
    if (Initializing > m_eState)
        return false;
    if (!waitforstate(Idle))
        return false;
    ::myLogTrace(MYTRACETAG, wxT("Unexporting ..."));
    if (!findsession(m_sSid)) {
        ::myLogTrace(MYTRACETAG, wxT("Session not found"));
        return false;
    }
    if (!send(wxT("unexport %s %s\n"), busid.c_str(), m_sSid.c_str()))
        return false;
    return true;
}

bool UsbIp::RegisterHotplug()
{
    if (m_sSid.IsEmpty())
        return false;
    if (Initializing > m_eState)
        return false;
    if (!waitforstate(Idle))
        return false;
    m_eState = Registering;
    ::myLogTrace(MYTRACETAG, wxT("Registering for hotplug ..."));
    if (!send(wxT("setsid %s\n"), m_sSid.c_str())) {
        m_eState = Idle;
        return false;
    }
    if (!waitforstate(Registered)) {
        m_eState = Idle;
        return false;
    }
    m_eState = Idle;
    return true;
}

bool UsbIp::SendHotplugResponse(const wxString &cookie)
{
    if (m_sSid.IsEmpty())
        return false;
    if (Initializing > m_eState)
        return false;
    if (!waitforstate(Idle))
        return false;
    m_eState = Responding;
    ::myLogTrace(MYTRACETAG, wxT("Sending hotplug response ..."));
    if (!send(wxT("handled %s\n"), cookie.c_str())) {
        m_eState = Idle;
        return false;
    }
    if (!waitforstate(Responded)) {
        m_eState = Idle;
        return false;
    }
    m_eState = Idle;
    return true;
}

ArrayOfUsbIpDevices UsbIp::GetDevices() {
    m_aDevices.Empty();
    if (Initializing > m_eState)
        return m_aDevices;
    if (!waitforstate(Idle))
        return m_aDevices;
    ::myLogTrace(MYTRACETAG, wxT("Fetching device list ..."));
    m_eState = ListDevices;
    if (!send(wxT("list\n"))) {
        m_eState = Idle;
        return m_aDevices;
    }
    if (!waitforstate(GotDevices)) {
        m_eState = Idle;
        return m_aDevices;
    }
    m_eState = Idle;
    return m_aDevices;
}

bool UsbIp::findsession(const wxString &sid)
{
    if (Initializing > m_eState)
        return false;
    if (!waitforstate(Idle))
        return false;
    m_aSessions.Empty();
    m_eState = ListSessions;
    if (!send(wxT("sessions\n"))) {
        m_eState = Idle;
        return false;
    }
    if (!waitforstate(GotSessions)) {
        m_eState = Idle;
        return false;
    }
    bool found = (m_aSessions.Index(sid, true) != wxNOT_FOUND);
    m_eState = Idle;
    return found;
}

bool UsbIp::send(const wxChar *fmt, ...)
{
    wxString buf;
    va_list args;
    va_start(args, fmt);
    if (0 > buf.PrintfV(fmt, args))
        return false;
    m_bError = false;
    m_pSocketClient->Write(buf.mb_str(wxConvUTF8), buf.Len());
    return ((m_pSocketClient->LastCount() == buf.Len()) && (!HasError()));
}

bool UsbIp::waitforstate(tStates state, long timeout /* = 5000 */)
{
    wxStopWatch watch;
    while (m_eState != state) {
        ::wxGetApp().Yield(true);
        wxLog::FlushActive();
        m_pSocketClient->Wait(0, 100);
        if (0 < timeout) {
            if (watch.Time() > timeout) {
                return false;
            }
        }
    }
    return !m_bError;
}

void UsbIp::parsehev(const wxString &line)
{
    wxRegEx re(wxT("([a-f\\d]+)\\s+(\\d+-[\\d\\.]+)\\s+(\\d+)\\s+(\\d+)\\s+([a-f\\d]+)\\s+([a-f\\d]+)"),
            wxRE_ADVANCED);
    if (!re.IsValid()) {
        ::wxLogFatalError(_("Invalid regular expression in %s %d"), __FILE__, __LINE__);
        return;
    }
    if (re.Matches(line)) {
        HotplugEvent ev;
        long lval;
        ev.SetCookie(re.GetMatch(line, 1));
        ev.SetBusID(re.GetMatch(line, 2));
        if (!re.GetMatch(line, 3).ToLong(&lval, 10))
            return;
        ev.SetBusNum(lval);
        if (!re.GetMatch(line, 4).ToLong(&lval, 10))
            return;
        ev.SetDevNum(lval);
        if (!re.GetMatch(line, 5).ToLong(&lval, 16))
            return;
        ev.SetProduct(lval);
        if (!re.GetMatch(line, 6).ToLong(&lval, 16))
            return;
        ev.SetVendor(lval);
        ev.SetEventObject(this);
        if (m_pEvtHandler)
            m_pEvtHandler->AddPendingEvent(ev);
    } else
        ::myLogTrace(MYTRACETAG, wxT("hev not matched"));
}

void UsbIp::parsesession(const wxString &line)
{
    wxRegEx re(wxT("\\[\\d+\\.\\d+\\.\\d+\\.\\d+\\]:\\d+\\s+\\(SID:\\s+([0-9,A-F]+)\\)"),
            wxRE_ADVANCED);
    if (!re.IsValid()) {
        ::wxLogFatalError(_("Invalid regular expression %s %d"), __FILE__, __LINE__);
        return;
    }
    if (re.Matches(line))
        m_aSessions.Add(re.GetMatch(line, 1));
}

void UsbIp::parsedevice(const wxString &line)
{
    wxStringTokenizer tkz(line, wxT(" "));
    if (6 == tkz.CountTokens()) {
        UsbIpDevice dev;
        long lval;
        dev.m_sUsbIpBusId = tkz.GetNextToken();
        dev.m_sConfig = tkz.GetNextToken();
        if (!tkz.GetNextToken().ToLong(&lval))
            return;
        dev.m_iUsbBusnum = lval;
        if (!tkz.GetNextToken().ToLong(&lval))
            return;
        dev.m_iUsbDevnum = lval;
        wxStringTokenizer tkz2(tkz.GetNextToken(), wxT(":"));
        dev.m_sDriver = tkz.GetNextToken();
        if (2 != tkz2.CountTokens())
            return;
        if (!tkz2.GetNextToken().ToLong(&lval, 16))
            return;
        dev.m_iVendorID = lval;
        if (!tkz2.GetNextToken().ToLong(&lval, 16))
            return;
        dev.m_iProductID = lval;
        m_aDevices.Add(dev);
    }
}

void UsbIp::parse(const wxString &line)
{
    wxString cs = line.Left(4).Trim();
    if (cs.Len() == 3) {
        long code;
        if (cs.ToLong(&code)) {
            if (200 != code)
                ::myLogTrace(MYTRACETAG, wxT("Got Line: '%s'"), line.c_str());
            switch (code) {
                case 100:
                    if (m_eState == Initializing)
                        m_bError = !send(wxT("\n"));
                    break;
                case 101:
                    if (m_eState == Initializing)
                        m_eState = Idle;
                    break;
                case 200:
                    m_bError = false;
                    switch (m_eState) {
                        case Exporting:
                            m_eState = Exported;
                            break;
                        case UnExporting:
                            m_eState = UnExported;
                            break;
                        case ListSessions:
                            m_eState = GotSessions;
                            break;
                        case ListDevices:
                            m_eState = GotDevices;
                            break;
                        case Registering:
                            m_eState = Registered;
                            break;
                        case Responding:
                            m_eState = Responded;
                            break;
                        default:
                            break;
                    }
                    break;
                case 201:
                    if (m_eState == Terminating)
                        m_eState = None;
                    break;
                case 202:
                    switch (m_eState) {
                        case ListSessions:
                            parsesession(line.Mid(4));
                            break;
                        case ListDevices:
                            parsedevice(line.Mid(4));
                            break;
                        default:
                            break;
                    }
                    break;
                case 203:
                    parsehev(line.Mid(4));
                    break;
                case 404:
                case 400:
                case 401:
                case 402:
                case 500:
                case 501:
                    m_iLastError = code;
                    m_bError = true;
                    switch (m_eState) {
                        case Initializing:
                            m_eState = Idle;
                            break;
                        case Exporting:
                            m_eState = Exported;
                            break;
                        case UnExporting:
                            m_eState = UnExported;
                            break;
                        case ListSessions:
                            m_eState = GotSessions;
                            break;
                        case ListDevices:
                            m_eState = GotDevices;
                            break;
                        case Registering:
                            m_eState = Registered;
                            break;
                        case Responding:
                            m_eState = Responded;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }
    }
}

void UsbIp::OnSocketEvent(wxSocketEvent &event)
{
    char *p;
    char *q;
    char buf[128];
    ::myLogTrace(MYTRACETAG, wxT("SocketEvent"));
    switch (event.GetSocketEvent()) {
        case wxSOCKET_OUTPUT:
            break;
        case wxSOCKET_INPUT:
            m_pSocketClient->Read(buf, sizeof(buf) - 1);
            buf[m_pSocketClient->LastCount()] = '\0';
            q = buf;
            while ((p = strchr(q, '\n'))) {
                *p++ = '\0';
                parse(m_sLineBuffer.Append(wxString(q, wxConvUTF8)));
                m_sLineBuffer = wxEmptyString;
                q = p;
            }
            m_sLineBuffer.Append(wxString(q, wxConvUTF8));
            break;
        case wxSOCKET_CONNECTION:
            ::myLogTrace(MYTRACETAG, wxT("OnConnection"));
            m_bConnected = true;
            break;
        case wxSOCKET_LOST:
            ::myLogTrace(MYTRACETAG, wxT("OnLost"));
            m_bConnected = false;
            break;
    }
}

#endif
// SUPPORT_USBIP
