// $Id: AsyncProcess.cpp 603 2011-02-21 19:18:11Z felfert $
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
#include "config.h"
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "AsyncProcess.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/filename.h>
#include <wx/txtstrm.h>

#include "AsyncProcess.h"

#include "trace.h"
ENABLE_TRACE;

#if 0
// trying to get rid of file descriptors which are left
// open by redirected wxExecute ...
#define private public
#define protected public
#include <wx/file.h>
#include <wx/wfstream.h>
#undef private
#undef protected
#endif

DEFINE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_STDOUT);
DEFINE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_STDERR);
DEFINE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_EXIT);

IMPLEMENT_DYNAMIC_CLASS(AsyncProcess, wxProcess)

AsyncProcess::AsyncProcess()
    : wxProcess(),
    wxThreadHelper(),
    m_pEvtHandler(NULL)
{
    Redirect();
}

AsyncProcess::AsyncProcess(const wxString& cmd, const wxString &wdir, const wxString &special, wxEvtHandler *h /* = NULL */)
    : wxProcess(),
    wxThreadHelper(),
    m_pEvtHandler(h),
    m_sCmd(cmd),
    m_sDir(wdir),
    m_sSpecial(special)
{
    Redirect();
}

AsyncProcess::AsyncProcess(const wxString& cmd, const wxString &wdir, wxEvtHandler *h /* = NULL */)
    : wxProcess(),
    wxThreadHelper(),
    m_pEvtHandler(h),
    m_sCmd(cmd),
    m_sDir(wdir)
{
    Redirect();
}

AsyncProcess::AsyncProcess(const wxString& cmd, wxEvtHandler *h /* = NULL */)
    : wxProcess(),
    wxThreadHelper(),
    m_pEvtHandler(h),
    m_sCmd(cmd),
    m_sDir(::wxGetCwd())
{
    Redirect();
}

AsyncProcess::~AsyncProcess()
{
    myLogTrace(MYTRACETAG, wxT("~AsyncProcess"));
    if (m_thread && m_thread->IsRunning()) {
        m_thread->Delete();
        while (m_thread->IsRunning())
            wxThread::Sleep(100);
        m_thread = NULL;
    }
    myLogTrace(MYTRACETAG, wxT("~AsyncProcess exit"));
    m_pEvtHandler = NULL;
}

    wxThread::ExitCode
AsyncProcess::Entry()
{
    myLogTrace(MYTRACETAG, wxT("IoThread starting"));
    while (!m_thread->TestDestroy()) {
        while (IsInputAvailable()) {
            char c = GetInputStream()->GetC();
            if (m_pEvtHandler) {
                m_cOutWatch.Start();
                switch (c) {
                    case '\r':
                        break;
                    case '\n':
                        {
                            wxCommandEvent event(wxEVT_PROCESS_STDOUT, wxID_ANY);
                            event.SetString(m_sOutBuf);
                            event.SetClientData(this);
                            m_pEvtHandler->AddPendingEvent(event);
                            m_sOutBuf.Empty();
                        }
                        break;
                    default:
                        if (c >= ' ')
                            m_sOutBuf += c;
                        break;
                }
            }
        }
        if (m_pEvtHandler) {
            // If no LF received within a second, send buffer anyway
            if ((m_sSpecial.Len() && m_sOutBuf.StartsWith(m_sSpecial)) ||
                    ((m_cOutWatch.Time() > 100) && (!m_sOutBuf.IsEmpty()))) {
                myLogTrace(MYTRACETAG, wxT("IoThread outwatch timed out"));
                wxCommandEvent event(wxEVT_PROCESS_STDOUT, wxID_ANY);
                event.SetString(m_sOutBuf);
                event.SetClientData(this);
                m_pEvtHandler->AddPendingEvent(event);
                m_sOutBuf.Empty();
            }
        }
        while (IsErrorAvailable()) {
            char c = GetErrorStream()->GetC();
            if (m_pEvtHandler) {
                m_cErrWatch.Start();
                switch (c) {
                    case '\r':
                        break;
                    case '\n':
                        {
                            wxCommandEvent event(wxEVT_PROCESS_STDERR, wxID_ANY);
                            event.SetString(m_sErrBuf);
                            event.SetClientData(this);
                            m_pEvtHandler->AddPendingEvent(event);
                            m_sErrBuf.Empty();
                        }
                        break;
                    default:
                        if (c >= ' ')
                            m_sErrBuf += c;
                        break;
                }
            }
        }
        if (m_pEvtHandler) {
            // If no LF received within a second, send buffer anyway
            if ((m_sSpecial.Len() && m_sErrBuf.StartsWith(m_sSpecial)) ||
                    ((m_cErrWatch.Time() > 100) && (!m_sErrBuf.IsEmpty()))) {
                myLogTrace(MYTRACETAG, wxT("IoThread errwatch timed out"));
                wxCommandEvent event(wxEVT_PROCESS_STDERR, wxID_ANY);
                event.SetString(m_sErrBuf);
                event.SetClientData(this);
                m_pEvtHandler->AddPendingEvent(event);
                m_sErrBuf.Empty();
            }
        }
        wxThread::Sleep(10);
    }
    myLogTrace(MYTRACETAG, wxT("IoThread exiting"));
    return 0;
}

    bool
AsyncProcess::IsRunning() {
    bool ret = ((m_pid > 0) && Exists(m_pid));
    return ret;
}

    bool
AsyncProcess::Print(const wxString &s, bool doLog)
{
    wxOutputStream *os = GetOutputStream();
    if (os) {
        if (doLog)
            myLogTrace(MYTRACETAG, wxT("Sending: '%s'"), s.c_str());
        else
            myLogTrace(MYTRACETAG, wxT("Sending (hidden): '************'"));
        wxString sbuf = s +  (s.IsEmpty() ? wxT("NullCommand\n") : wxT("\n")) ;
        const wxWX2MBbuf buf = wxConvCurrent->cWX2MB(sbuf);
        os->Write(buf, strlen(buf));
        return true;
    }
    return false;
}

    void
AsyncProcess::OnTerminate(int pid, int status)
{
    myLogTrace(MYTRACETAG, wxT("Process %u terminated with exit code %d."), pid, status);
    wxLog::FlushActive();
    m_iStatus = status;
    if (m_thread && m_thread->IsRunning()) {
        while ((m_cOutWatch.Time() < 2000) || (m_cErrWatch.Time() < 2000))
            wxThread::Sleep(100);
        m_thread->Delete();
        while (m_thread->IsRunning())
            wxThread::Sleep(100);
        myLogTrace(MYTRACETAG, wxT("OnTerminate(): IoThread has ended."));
        m_thread = NULL;
    }
    if (m_pEvtHandler) {
        wxCommandEvent event(wxEVT_PROCESS_EXIT, wxID_ANY);
        event.SetInt(m_iStatus);
        event.SetClientData(this);
        m_pEvtHandler->AddPendingEvent(event);
    }
}

    bool
AsyncProcess::Start()
{
    bool ret = false;

    if (!m_sCmd.IsEmpty()) {
        wxString cwd = ::wxGetCwd();
        myLogTrace(MYTRACETAG, wxT("Starting '%s'"), m_sCmd.c_str());
        if (!m_sDir.IsEmpty())
            wxFileName::SetCwd(m_sDir);
        m_sOutBuf.Empty();
        m_sErrBuf.Empty();
        m_pid = ::wxExecute(m_sCmd, wxEXEC_ASYNC, this);
        myLogTrace(MYTRACETAG, wxT("wxExecute returned %d"), (int)m_pid);
        ret = (m_pid > 0);
        if (!m_sDir.IsEmpty())
            wxFileName::SetCwd(cwd);
        if (ret) {
            myLogTrace(MYTRACETAG, wxT(">Create()"));
            if (IsRunning())
            #if wxCHECK_VERSION(3, 0, 0)
                CreateThread();
            #else
                Create();
            #endif
            myLogTrace(MYTRACETAG, wxT("<Create()"));
            if (IsRunning())
                m_thread->Run();
            myLogTrace(MYTRACETAG, wxT("Run()"));
        }
    }
    return ret;
}

    void
AsyncProcess::Detach()
{
    myLogTrace(MYTRACETAG, wxT("Detach() called"));
    m_pEvtHandler = NULL;
#if 0
    int out_fd = reinterpret_cast<wxFileOutputStream*>(m_outputStream)->m_file->m_fd;
    int err_fd = reinterpret_cast<wxFileInputStream*>(m_errorStream)->m_file->m_fd;
    int inp_fd = reinterpret_cast<wxFileInputStream*>(m_inputStream)->m_file->m_fd;
    myLogTrace(MYTRACETAG, wxT("inp=%d out=%d err=%d"), inp_fd, out_fd, err_fd);
#endif
    wxProcess::Detach();
    if (m_thread && m_thread->IsRunning()) {
        m_thread->Delete();
        myLogTrace(MYTRACETAG, wxT("Detatch(): waiting for IoThread"));
        while (m_thread->IsRunning())
            wxThread::Sleep(100);
        myLogTrace(MYTRACETAG, wxT("Detatch(): IoThread has ended"));
        m_thread = NULL;
    }
#if 0
    close(inp_fd);
    close(out_fd);
    close(err_fd);
#endif
}

    bool
AsyncProcess::Kill()
{
    myLogTrace(MYTRACETAG, wxT("Kill() called"));
    bool ret = false;
    if (IsRunning()) {
        myLogTrace(MYTRACETAG, wxT("Kill(): actually send sig"));
        switch (wxProcess::Kill(GetPid(), wxSIGKILL)) {
            case wxKILL_OK:
                Detach();
            case wxKILL_NO_PROCESS:
                ret = true;
                break;
            case wxKILL_BAD_SIGNAL:
            case wxKILL_ACCESS_DENIED:
            case wxKILL_ERROR:
                break;
        }
    }
    return ret;
}
