// $Id: MyIPC.h 700 2012-02-22 11:56:52Z felfert $
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

#ifndef _MYIPC_H_
#define _MYIPC_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "MyIPC.cpp"
#endif

#include <wx/event.h>

DECLARE_LOCAL_EVENT_TYPE(wxEVT_NXSSH, -1);
DECLARE_LOCAL_EVENT_TYPE(wxEVT_GENERIC, -2);

class AsyncProcess;
class wxRegEx;

class MyIPC : public wxEvtHandler
{
    DECLARE_CLASS(MyIPC);
    DECLARE_EVENT_TABLE();

public:
    typedef enum {
        ActionNone,
        ActionStatus,
        ActionHello,
        ActionLog,
        ActionWarning,
        ActionError,
        ActionPromptYesNo,
        ActionKeyChangedYesNo,
        ActionOffendingKey,
        ActionSendUsername,
        ActionSendPassword,
        ActionNextCommand,
        ActionWelcome,
        ActionPassphraseDialog,
        ActionPinDialog,
        ActionSetSessionID,
        ActionSetProxyCookie,
        ActionSetProxyIP,
        ActionSetSessionType,
        ActionSetSessionCache,
        ActionSetSessionDisplay,
        ActionSetAgentCookie,
        ActionSetSslTunneling,
        ActionSetSubscription,
        ActionSetSmbPort,
        ActionSetShadowGeometry,
        ActionExit,
        ActionStartProxy,
        ActionSessionRunning,
        ActionSessionListStart,
        ActionSessionListEnd,
        ActionSessionPushLength,
        ActionSessionPushStart,
        ActionResList,
        ActionTerminated,
        ActionStdout,
        ActionStderr,
    } tSessionEvents;

    MyIPC();
    virtual ~MyIPC();
    bool GenericProcess(const wxString&, const wxString&, wxEvtHandler *);
    bool SshProcess(const wxString&, const wxString&, wxEvtHandler *);
    bool IsRunning();
    bool Kill();
    void Print(const wxString &s, bool doLog = true);
    int GetResult();
    long GetPID();
    void Detach();

private:
    typedef enum {
        TypeNone,
        TypeSsh,
    } ProcessType;

    int parseCode(const wxString &);
    virtual void OnOutReceived(wxCommandEvent &);
    virtual void OnErrReceived(wxCommandEvent &);
    virtual void OnTerminate(wxCommandEvent &);

    void sendUpEvent(wxCommandEvent &event);

#ifdef __WXMSW__
    unsigned int m_MsgSession;
    unsigned int m_MsgKill;
#endif
    AsyncProcess *m_pProcess;
    wxEvtHandler *m_pEvtHandler;
    wxMutex m_cEvhMutex;
    wxRegEx *m_re;
    ProcessType m_eType;
    int m_iSshPid;
    int m_iOutCollect;
    int m_iErrCollect;
    int m_nLines618;;
    wxString m_sOutMessage;
    wxString m_sErrMessage;
    wxString m_s595msg;
    wxString m_s618msg;
};

#endif
// _MYIPC_
