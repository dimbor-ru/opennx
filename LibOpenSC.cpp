// $Id: LibOpenSC.cpp 451 2010-01-27 12:24:56Z felfert $
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
#pragma implementation "LibOpenSC.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/event.h>
#include <wx/thread.h>
#include <wx/process.h>
#include <wx/config.h>

#include "MyDynlib.h"
#include "LibOpenSC.h"
#ifdef APP_OPENNX
# include "opennxApp.h"
# include "CardWaiterDialog.h"
#endif
#ifdef APP_WATCHREADER
# include "watchReaderApp.h"
#endif

#include "trace.h"
ENABLE_TRACE;

#ifdef __WXMAC__
// Current SCA implementation on MacOSX does not
// allow multiple applications. So we use the same
// context like nxssh.
# define OPENSC_CTXNAME "openssh"
#else
# define OPENSC_CTXNAME "opennx"
#endif

#include <opensc/opensc.h>
typedef int (*Tsc_establish_context)(sc_context_t **ctx, const char *app_name);
typedef int (*Tsc_release_context)(sc_context_t *ctx);
typedef int (*Tsc_detect_card_presence)(sc_reader_t *reader, int slot_id);

#ifdef APP_OPENNX
DEFINE_LOCAL_EVENT_TYPE(wxEVT_CARDINSERTED);

class CardWaitThread : public wxThreadHelper
{
    public:
        CardWaitThread(wxEvtHandler *handler = NULL);
        virtual ~CardWaitThread();

        virtual wxThread::ExitCode Entry();

        bool IsOk() { return m_bOk; }
        int GetReader() { return m_iFoundID; }
        void SetHandler(wxEvtHandler *handler) { m_pEvtHandler = handler; }

    private:
        wxEvtHandler *m_pEvtHandler;
        bool m_bOk;
        bool m_bTerminate;
        bool m_bFirstLoopDone;
        int m_iFoundID;
};

CardWaitThread::CardWaitThread(wxEvtHandler *handler)
    : wxThreadHelper()
    , m_pEvtHandler(handler)
    , m_bOk(false)
    , m_bTerminate(false)
    , m_bFirstLoopDone(false)
    , m_iFoundID(-1)
{
    if (CreateThread(
#ifdef __OPENBSD__
            32768
#endif
          ) == wxTHREAD_NO_ERROR) {
        GetThread()->Run();
        while ((!m_bFirstLoopDone) && GetThread()->IsRunning())
            wxThread::Sleep(100);
        myLogTrace(MYTRACETAG, wxT("opensc API is %savailable"), m_bOk ? wxT("") : wxT("un"));
        if (m_bOk)
            myLogTrace(MYTRACETAG, wxT("reader-ID: %d"), (int)m_iFoundID);
    } else
        myLogTrace(MYTRACETAG, wxT("could not create waiter thread"));
}

CardWaitThread::~CardWaitThread()
{
    m_pEvtHandler = NULL;
    if (m_bOk) {
        m_bTerminate = true;
        GetThread()->Delete();
        while (m_bOk)
            wxThread::Sleep(100);
    }
}

    wxThread::ExitCode
CardWaitThread::Entry()
{
#ifdef __WXMAC__
    m_bOk = true;
    wxString cmd;
    wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &cmd);
    cmd << wxFileName::GetPathSeparator() << wxT("bin")
        << wxFileName::GetPathSeparator() << wxT("macfindreader");
    while (!m_thread->TestDestroy()) {
        int res = system(cmd.mb_str());
        if ((res >= 0) && (res < 255)) {
            if (res != m_iFoundID) {
                m_iFoundID = res;
                if (m_pEvtHandler) {
                    wxCommandEvent ev(wxEVT_CARDINSERTED, wxID_ANY);
                    ev.SetInt(m_iFoundID);
                    m_pEvtHandler->AddPendingEvent(ev);
                }
            }
        }
        if (m_bFirstLoopDone)
            wxThread::Sleep(500);
        m_bFirstLoopDone = true;
    }
#else
    sc_context *ctx;
    MyDynamicLibrary dll;
    {
        wxLogNull ignoreErrors;
        if (!dll.Load(wxT("libopensc")))
            return 0;
    }

    wxDYNLIB_FUNCTION(Tsc_establish_context, sc_establish_context, dll);
    if (!pfnsc_establish_context)
        return 0;
    if (SC_SUCCESS != pfnsc_establish_context(&ctx, OPENSC_CTXNAME))
        return 0;
    wxDYNLIB_FUNCTION(Tsc_release_context, sc_release_context, dll);
    if (!pfnsc_release_context)
        return 0;

    wxDYNLIB_FUNCTION(Tsc_detect_card_presence, sc_detect_card_presence, dll);
    if (!pfnsc_detect_card_presence)
        return 0;

    m_bOk = true;
    while (!m_thread->TestDestroy()) {
        if (m_bTerminate)
            break;
        int found_id = -1;
        int r, j;
		unsigned int i;

        unsigned int rc = ctx->reader_count;
        if (rc > 0) {
            unsigned int errc = 0;

            for (i = 0; i < rc; i++) {
                sc_reader_t *reader = ctx->reader[i];
                if (!reader)
                    continue;
                myLogTrace(MYTRACETAG, wxT("Trying reader %d"), i);
                for (j = 0; j < reader->slot_count; j++) {
                    r = pfnsc_detect_card_presence(reader, j);
                    if (r > 0) {
                        found_id = i;
                        break;
                    }
                    if (r < 0) {
                        errc++;
                        myLogTrace(MYTRACETAG, wxT("error %d during sc_detect_card_presence"), r);
                    }
                }
                if (found_id != -1)
                    break;
            }
            if (errc >= rc) {
                myLogTrace(MYTRACETAG, wxT("All readers returned an error"));
                pfnsc_release_context(ctx);
                m_bOk = false;
                return 0;
            }
        } else
            myLogTrace(MYTRACETAG, wxT("no readers found"));
        if (found_id == -1)
            myLogTrace(MYTRACETAG, wxT("no cards found"));
        else {
            myLogTrace(MYTRACETAG, wxT("found card in reader %d"), found_id);
            if (found_id != m_iFoundID) {
                m_iFoundID = found_id;
                if (m_pEvtHandler) {
                    wxCommandEvent ev(wxEVT_CARDINSERTED, wxID_ANY);
                    ev.SetInt(m_iFoundID);
                    m_pEvtHandler->AddPendingEvent(ev);
                }
            }
        }
        if (m_bFirstLoopDone)
            wxThread::Sleep(500);
        m_bFirstLoopDone = true;
    }
    pfnsc_release_context(ctx);
#endif
    m_bOk = false;
    myLogTrace(MYTRACETAG, wxT("terminating waiter thread"));
    return 0;
}
#endif // APP_OPENNX

LibOpenSC::LibOpenSC()
{
    myLogTrace(MYTRACETAG, wxT("LibOpenSC()"));
}

LibOpenSC::~LibOpenSC()
{
    myLogTrace(MYTRACETAG, wxT("~LibOpenSC()"));
}

bool LibOpenSC::HasOpenSC() {
    wxLogNull ignoreErrors;
    MyDynamicLibrary dll;
    if (!dll.Load(wxT("libopensc")))
        return false;
    wxDYNLIB_FUNCTION(Tsc_establish_context, sc_establish_context, dll);
    if (!pfnsc_establish_context)
        return false;
    wxDYNLIB_FUNCTION(Tsc_release_context, sc_release_context, dll);
    if (!pfnsc_release_context)
        return false;
    wxDYNLIB_FUNCTION(Tsc_detect_card_presence, sc_detect_card_presence, dll);
    if (!pfnsc_detect_card_presence)
        return false;
    return true;
}

#ifdef APP_OPENNX
int LibOpenSC::WaitForCard(CardWaiterDialog *d) {
    CardWaitThread t(d);
    if (t.IsOk() && (t.GetReader() != -1))
        return t.GetReader();
    d->ShowModal();
    return d->GetReader();
}
#endif

bool LibOpenSC::WatchHotRemove(unsigned int ridx, long sshpid) {

    MyDynamicLibrary dll;
    myLogTrace(MYTRACETAG, wxT("WatchHotRemove loading OpenSC"));
    {
        wxLogNull ignoreErrors;
        if (!dll.Load(wxT("libopensc")))
            return false;
    }

    myLogTrace(MYTRACETAG, wxT("WatchHotRemove checking OpenSC functions"));
    wxDYNLIB_FUNCTION(Tsc_establish_context, sc_establish_context, dll);
    if (!pfnsc_establish_context)
        return false;
    wxDYNLIB_FUNCTION(Tsc_release_context, sc_release_context, dll);
    if (!pfnsc_release_context)
        return false;
    wxDYNLIB_FUNCTION(Tsc_detect_card_presence, sc_detect_card_presence, dll);
    if (!pfnsc_detect_card_presence)
        return false;

    sc_context *ctx = NULL;
    myLogTrace(MYTRACETAG, wxT("WatchHotRemove waiting for card removal"));
    myLogTrace(MYTRACETAG, wxT("WatchHotRemove trying to establish context"));
    if (SC_SUCCESS == pfnsc_establish_context(&ctx, OPENSC_CTXNAME)) {
        while (true) {
            if (sshpid != 0) {
                if (!wxProcess::Exists(sshpid)) {
                    myLogTrace(MYTRACETAG, wxT("nxssh pid %d has terminated"), (int)sshpid);
                    return false;
                }
            }
            unsigned int rc = ctx->reader_count;
            if (rc <= ridx) {
                // reader is gone
                myLogTrace(MYTRACETAG, wxT("reader is gone"));
                break;
            }
            sc_reader_t *reader = ctx->reader[ridx];
            if (reader) {
                int r = 1;
                int j;
                for (j = 0; j < reader->slot_count; j++) {
                    myLogTrace(MYTRACETAG, wxT("WatchHotRemove checking for card in reader %d, slot %d"), ridx, j);
                    r = pfnsc_detect_card_presence(reader, j);
                    if (r == 0) {
                        // card is gone
                        myLogTrace(MYTRACETAG, wxT("card is gone"));
                        break;
                    }
                    if (r < 0) {
                        myLogTrace(MYTRACETAG, wxT("error %d during sc_detect_card_presence"), r);
                        break;
                    }
                }
                if (r <= 0)
                    break;
            } else {
                myLogTrace(MYTRACETAG, wxT("no readers found"));
                break;
            }
            while (wxGetApp().Pending())
                wxGetApp().Dispatch();
            wxThread::Sleep(1000);
        }
    } else
        myLogTrace(MYTRACETAG, wxT("could not establish context"));
    if (ctx)
        pfnsc_release_context(ctx);
    if (sshpid != 0) {
        myLogTrace(MYTRACETAG, wxT("Sending HUP to nxssh pid %d"), (int)sshpid);
        int trycount = 10;
        while (wxProcess::Exists(sshpid) && (0 < trycount)) {
            wxProcess::Kill(sshpid, wxSIGHUP);
            while (::wxGetApp().Pending())
                ::wxGetApp().Dispatch();
            wxThread::Sleep(500);
            trycount--;
        }
        if (!wxProcess::Exists(sshpid))
            return true;
        // Again, this time SIGTERM
        myLogTrace(MYTRACETAG, wxT("Sending TERM to nxssh pid %d"), (int)sshpid);
        trycount = 10;
        while (wxProcess::Exists(sshpid) && (0 < trycount)) {
            wxProcess::Kill(sshpid, wxSIGTERM);
            while (::wxGetApp().Pending())
                ::wxGetApp().Dispatch();
            wxThread::Sleep(500);
            trycount--;
        }
        if (!wxProcess::Exists(sshpid))
            return true;
        // Finally, use brute force
        myLogTrace(MYTRACETAG, wxT("Sending KILL to nxssh pid %d"), (int)sshpid);
        trycount = 10;
        while (wxProcess::Exists(sshpid) && (0 < trycount)) {
            wxProcess::Kill(sshpid, wxSIGKILL);
            while (::wxGetApp().Pending())
                ::wxGetApp().Dispatch();
            wxThread::Sleep(500);
            trycount--;
        }
        if (!wxProcess::Exists(sshpid))
            return true;
        wxLogError(_("Could not terminate nxssh"));
    } else
        return true;
    return false;
}
