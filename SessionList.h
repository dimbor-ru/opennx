// $Id: SessionList.h 399 2009-09-09 19:35:12Z felfert $
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

#ifndef _SESSIONLIST_H_
#define _SESSIONLIST_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "SessionList.cpp"
#endif

#include <wx/thread.h>

class wxDir;
class wxRegEx;
class SessionHash;

DECLARE_LOCAL_EVENT_TYPE(wxEVT_SESSIONLIST_ACTION, -1)

class SessionList : public wxThreadHelper
{
public:
    enum {
        SessionAdded,
        SessionChanged,
        SessionRemoved,
        UpdateList
    };

    SessionList(wxString dir = _T(""), wxEvtHandler *h = NULL);
    virtual ~SessionList();

    virtual wxThread::ExitCode Entry();
    
    void SetDir(wxString dir);
    void SetAdminHandler(wxEvtHandler *h) { m_pAdminHandler = h; }

    void ScanDir();
    void CleanupDir(wxString &);
    void RemoveFromList(wxString md5);

private:
    wxCriticalSection m_csDir;
    wxString m_dirName;
    wxEvtHandler *m_pAdminHandler;
    SessionHash *m_sessions;
    wxDir *m_dir;
    wxRegEx *m_re;
    bool m_reValid;
};

#endif // _SESSIONLIST_H_
