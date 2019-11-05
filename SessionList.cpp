// $Id: SessionList.cpp 687 2012-02-18 00:00:24Z felfert $
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
#pragma implementation "SessionList.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SessionList.h"
#include "MySession.h"

#include <wx/dir.h>
#include <wx/filename.h>

#include "trace.h"
ENABLE_TRACE;

DEFINE_LOCAL_EVENT_TYPE(wxEVT_SESSIONLIST_ACTION);

class SessionTraverser : public wxDirTraverser
{
    public:
        SessionTraverser(wxArrayString& dirs) : m_dirs(dirs) { }

        virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename))
        {
            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString& dirpath)
        {
            wxString name = wxFileName::FileName(dirpath).GetFullName();
            m_dirs.Add(name);
            return wxDIR_IGNORE;
        }

    private:
        wxArrayString& m_dirs;
};

class RmRfTraverser : public wxDirTraverser
{
    public:
        RmRfTraverser() { }

        ~RmRfTraverser()
        {
            int n = m_aFiles.GetCount() - 1;
            while (n >= 0) {
                myLogTrace(MYTRACETAG, wxT("Removing file %s"), VMB(m_aFiles[n]));
                ::wxRemoveFile(m_aFiles[n--]);
            }
            n = m_aDirs.GetCount() - 1;
            while (n >= 0) {
                myLogTrace(MYTRACETAG, wxT("Removing dir %s"), VMB(m_aDirs[n]));
                ::wxRmdir(m_aDirs[n--]);
            }
        }

        virtual wxDirTraverseResult OnFile(const wxString& filename)
        {
            myLogTrace(MYTRACETAG, wxT("going to delete file %s"), VMB(filename));
            m_aFiles.Add(filename);
            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString& dirpath)
        {
            myLogTrace(MYTRACETAG, wxT("going to delete dir %s"), VMB(dirpath));
            m_aDirs.Add(dirpath);
            return wxDIR_CONTINUE;
        }

    private:
        wxArrayString m_aDirs;
        wxArrayString m_aFiles;
};

WX_DECLARE_STRING_HASH_MAP(MySession, SessionHash);

SessionList::SessionList(wxString dir, wxEvtHandler* h)
    : wxThreadHelper()
    , m_dirName(dir)
    , m_pAdminHandler(h)
    , m_dir(NULL)
    , m_reValid(false)
{
    m_sessions = new SessionHash();
    m_re = new wxRegEx();
    if (m_re->Compile(wxT("(([TF])-)?([SC])-(.*)-([[:digit:]]+)-([[:xdigit:]]{32})$"), wxRE_ADVANCED))
        m_reValid = true;
    if (m_reValid && (!m_dirName.IsEmpty())) {
        CreateThread();
        m_thread->Run();
    }
}

SessionList::~SessionList()
{
    m_thread->Delete();
    while (m_thread->IsRunning())
        wxThread::Sleep(100);
    if (m_dir != NULL)
        delete m_dir;
    delete m_re;
    m_sessions->clear();
    delete m_sessions;
}

void SessionList::SetDir(wxString dir)
{
    m_dirName = dir;
    if (m_reValid && (!m_dirName.IsEmpty())) {
        if (m_thread == NULL) {
            CreateThread();
            m_thread->Run();
        } else {
            m_csDir.Enter();
            if (m_dir)
                delete m_dir;
            m_dir = NULL;
            m_csDir.Leave();
        }
    }
}

void SessionList::ScanDir()
{
    wxCriticalSectionLocker dlocker(m_csDir);

    if (m_dir == NULL) {
        if (m_dirName.IsEmpty())
            return;
        if (!wxDir::Exists(m_dirName)) {
            m_dirName = wxEmptyString;
            return;
        }
        m_dir = new wxDir(m_dirName);
    }

    // get the names of all session directories
    wxArrayString sdirs;
    SessionTraverser traverser(sdirs);
    m_dir->Traverse(traverser);
    size_t cnt = sdirs.GetCount();
    size_t i;

    // Format of session dir name:
    //
    // ([TF]-)?[SC]-(.*)-[:digit:]+-[:xdigit:]{32}
    //
    // 1. element is session status
    // "T-" resp. "F-" stands for terminated or failed respectively.
    // If it is missing, the session is undefined (probably running).
    //
    // 2. element is session type
    // S- resp. C- stands for server resp. client session type
    //
    // 3. element is host name of the NX server
    // 4. element is port name
    // 5. element is an md5sum. (//FE: For what data?)
    //

    bool changed = false;
    SessionHash::iterator it;
    size_t oldcount = m_sessions->size();
    for (it = m_sessions->begin(); it != m_sessions->end(); ++it)
        it->second.bSetTouched(false);

    for (i = 0; i < cnt; i++) {
        wxString tmp = sdirs[i];
        if (m_re->Matches(tmp)) {
            wxString md5 = m_re->GetMatch(tmp, 6);

            it = m_sessions->find(md5);
            if (it == m_sessions->end()) {
                // New session found
                long port;

                m_re->GetMatch(tmp,5).ToLong(&port);
                myLogTrace(MYTRACETAG,
                        wxT("State='%s', Type='%s', Host='%s', Port=%d, MD5='%s'"),
                        VMB(m_re->GetMatch(tmp,2)),
                        VMB(m_re->GetMatch(tmp,3)),
                        VMB(m_re->GetMatch(tmp,4)),
                        (int)port, VMB(md5));
                // Create new hash entry
                MySession s(m_dirName + wxFileName::GetPathSeparator() + tmp,
                        m_re->GetMatch(tmp,2), m_re->GetMatch(tmp,3),
                        m_re->GetMatch(tmp,4), port, md5);
                (*m_sessions)[md5] = s;
                if (m_pAdminHandler) {
                    wxCommandEvent ev(wxEVT_SESSIONLIST_ACTION, wxID_ANY);
                    ev.SetInt(SessionAdded);
                    ev.SetClientData(&(m_sessions->find(md5)->second));
                    m_pAdminHandler->AddPendingEvent(ev);
                    changed = true;
                }
            } else {
                // Existing session found, mark it
                it->second.bSetTouched(true);
            }
        }
    }
    bool finished = false;

    while (!finished) {
        finished = true;
        for (it = m_sessions->begin(); it != m_sessions->end(); ++it) {
            if (it->second.bGetTouched()) {
                MySession::tSessionStatus st = it->second.eGetSessionStatus();
                it->second.CheckState();
                if (it->second.eGetSessionStatus() != st) {
                    if (m_pAdminHandler) {
                        wxCommandEvent ev(wxEVT_SESSIONLIST_ACTION, wxID_ANY);
                        ev.SetInt(SessionChanged);
                        ev.SetClientData(&it->second);
                        m_pAdminHandler->AddPendingEvent(ev);
                        changed = true;
                    }
                }
            } else {
                wxString md5 = it->second.sGetMd5();
                myLogTrace(MYTRACETAG, wxT("Session '%s' disappeared"), VMB(md5));
                finished = false;
                if (m_pAdminHandler) {
                    wxCommandEvent ev(wxEVT_SESSIONLIST_ACTION, wxID_ANY);
                    ev.SetInt(SessionRemoved);
                    ev.SetClientData(&it->second);
                    m_pAdminHandler->AddPendingEvent(ev);
                    changed = true;
                }
                RemoveFromList(md5);
                break;
            }
        }
    }
    if (changed && (m_pAdminHandler != NULL)) {
        wxCommandEvent ev(wxEVT_SESSIONLIST_ACTION, wxID_ANY);
        ev.SetInt(UpdateList);
        m_pAdminHandler->AddPendingEvent(ev);
    }
    if (m_sessions->size() != oldcount)
        myLogTrace(MYTRACETAG, wxT("SessionList: Now %d sessions"), (int)m_sessions->size());
}

    void
SessionList::RemoveFromList(wxString md5)
{
    m_sessions->erase(md5);
}

    void
SessionList::CleanupDir(wxString &dir)
{
    if (!dir.IsEmpty()) {
        {
            myLogTrace(MYTRACETAG, wxT("CleanupDir '%s'"), VMB(dir));
            wxDir d(dir);
            RmRfTraverser t;
            d.Traverse(t);
        }
        ::wxRmdir(dir);
    }
}

    wxThread::ExitCode
SessionList::Entry()
{
    int cnt = 0;
    while (!m_thread->TestDestroy()) {
        if (cnt-- == 0) {
            ScanDir();
            cnt = 20;
        }
        wxThread::Sleep(100);
    }
    return 0;
}
