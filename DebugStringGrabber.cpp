// $Id: DebugStringGrabber.cpp 484 2010-02-20 20:33:35Z felfert $
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
#pragma implementation "DebugStringGrabber.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef __WXMSW__

#include "DebugStringGrabber.h"
#include <windows.h>

DEFINE_LOCAL_EVENT_TYPE(wxEVT_DEBUGSTRING);

typedef struct {
    DWORD pid;
    char msg[1];
} tDebugBuffer;

class DebugStringGrabberData {
    friend class DebugStringGrabber;
    private:
        DWORD FilterPID;
        HANDLE hDataReady;
        bool bTerminate;
};

DebugStringGrabber::DebugStringGrabber(wxEvtHandler *handler, pid_t filterPID)
    : wxThreadHelper()
    , m_pEvtHandler(handler)
    , m_bOk(false)
{
    m_pData = new DebugStringGrabberData();
    m_pData->FilterPID = filterPID;
    m_pData->hDataReady = NULL;
    m_pData->bTerminate = false;
    if (Create(
#ifdef __OPENBSD__
            32768
#endif
          ) == wxTHREAD_NO_ERROR) {
        GetThread()->Run();
        while ((!m_bOk) && GetThread()->IsRunning())
            wxThread::Sleep(100);
    }
}
    
DebugStringGrabber::~DebugStringGrabber()
{
    m_pEvtHandler = NULL;
    if (m_bOk) {
        m_pData->bTerminate = true;
        if (m_pData->hDataReady)
            ::SetEvent(m_pData->hDataReady);
        GetThread()->Delete();
        while (m_bOk)
            wxThread::Sleep(100);
    }
    delete m_pData;
}

    wxThread::ExitCode
DebugStringGrabber::Entry()
{
    HANDLE bufferready = ::CreateEvent(NULL, FALSE, FALSE, wxT("DBWIN_BUFFER_READY"));
    if (NULL == bufferready)
        return 0;
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
        return 0;
    m_pData->hDataReady = ::CreateEvent(NULL, FALSE, FALSE, wxT("DBWIN_DATA_READY"));
    if (NULL == m_pData->hDataReady) {
        ::CloseHandle(bufferready);
        return 0;
    }
    HANDLE hBuf = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, wxT("DBWIN_BUFFER"));
    if (NULL == hBuf) {
        ::CloseHandle(bufferready);
        ::CloseHandle(m_pData->hDataReady);
        m_pData->hDataReady = NULL;
        return 0;
    }
    tDebugBuffer *buf = (tDebugBuffer *)::MapViewOfFile(hBuf, FILE_MAP_READ, 0, 0, 4096);
    if (buf == NULL) {
        ::CloseHandle(bufferready);
        ::CloseHandle(m_pData->hDataReady);
        m_pData->hDataReady = NULL;
        ::CloseHandle(hBuf);
        return 0;
    }
    DWORD lastpid = 0xffffffff;

    m_bOk = true;
    while (!m_thread->TestDestroy()) {
        if (::SetEvent(bufferready) == FALSE) {
            break;
        }
        if (::WaitForSingleObject(m_pData->hDataReady, INFINITE) != WAIT_OBJECT_0)
            break;
        if (m_pData->bTerminate)
            break;
        if (lastpid != buf->pid)
            lastpid = buf->pid;
        if (m_pData->FilterPID && (lastpid != m_pData->FilterPID))
            continue;
        if (m_pEvtHandler) {
            wxString msg(buf->msg, wxConvUTF8);
            wxCommandEvent ev(wxEVT_DEBUGSTRING, wxID_ANY);
            ev.SetInt(lastpid);
            ev.SetString(msg.Trim());
            m_pEvtHandler->AddPendingEvent(ev);
        } else
            fprintf(stderr, "%s\n", buf->msg); fflush(stderr);
    }
    m_bOk = false;
    ::CloseHandle(bufferready);
    ::CloseHandle(m_pData->hDataReady);
    m_pData->hDataReady = NULL;
    ::UnmapViewOfFile(buf);
    ::CloseHandle(hBuf);

    return 0;
}

#endif // __WXMSW__
