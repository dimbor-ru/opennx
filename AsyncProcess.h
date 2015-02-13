// $Id: AsyncProcess.h 603 2011-02-21 19:18:11Z felfert $
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

#ifndef _ASYNCPROCESS_H_
#define _ASYNCPROCESS_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "AsyncProcess.cpp"
#endif

#include <wx/event.h>
#include <wx/process.h>
#include <wx/thread.h>

DECLARE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_STDOUT, -3);
DECLARE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_STDERR, -4);
DECLARE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_EXIT,   -5);

class AsyncProcess : public wxProcess, wxThreadHelper
{

    DECLARE_DYNAMIC_CLASS(AsyncProcess)

    public:
        AsyncProcess();
        AsyncProcess(const wxString& cmd, const wxString &wdir, const wxString &special, wxEvtHandler *h = NULL);
        AsyncProcess(const wxString& cmd, const wxString &wdir, wxEvtHandler *h = NULL);
        AsyncProcess(const wxString& cmd, wxEvtHandler *h = NULL);

        virtual ~AsyncProcess();

        bool Start();
        bool Kill();
        bool Print(const wxString &, bool doLog);
        bool IsRunning();
        int GetStatus() { return m_iStatus; }
        void Detach();
        virtual void OnTerminate(int pid, int status);

    private:
        virtual wxThread::ExitCode Entry();

        int m_iStatus;
        wxEvtHandler *m_pEvtHandler;

        wxString m_sOutBuf;
        wxString m_sErrBuf;
        wxString m_sCmd;
        wxString m_sDir;
        wxString m_sSpecial;
        wxStopWatch m_cOutWatch;
        wxStopWatch m_cErrWatch;
};

#endif
// _ASCYNCPROCESS_H_
