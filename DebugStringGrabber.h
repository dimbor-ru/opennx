// $Id: DebugStringGrabber.h 148 2009-02-10 20:04:38Z felfert $
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

#ifndef _DEBUGSTRINGGRABBER_H_
#define _DEBUGSTRINGGRABBER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "DebugStringGrabber.cpp"
#endif

#include <wx/event.h>
#include <wx/thread.h>

#ifdef __WXMSW__
DECLARE_LOCAL_EVENT_TYPE(wxEVT_DEBUGSTRING, -4);

class DebugStringGrabberData;

/**
 * DebugStringGrabber uses the Windows debugger API to
 * fetch messages, sent by OutputDebugString() API.
 * For each message, it delivers an wxWidgets event to
 * the wxEventHandler, specified in it's constructor.
 */
class DebugStringGrabber : public wxThreadHelper
{
    public:
        DebugStringGrabber(wxEvtHandler *handler = NULL, pid_t filterPID = 0);
        virtual ~DebugStringGrabber();

        virtual wxThread::ExitCode Entry();

        bool IsOk() { return m_bOk; }
        void SetHandler(wxEvtHandler *handler) { m_pEvtHandler = handler; }

    private:
        wxEvtHandler *m_pEvtHandler;
        bool m_bOk;
        DebugStringGrabberData *m_pData;
};

#endif // __WXMSW__

#endif
