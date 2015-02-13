// $Id: MyIPC.cpp 700 2012-02-22 11:56:52Z felfert $
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
#pragma implementation "MyIPC.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/process.h>
#include <wx/txtstrm.h>
#include <wx/filename.h>
#include <wx/regex.h>

#include "MyIPC.h"
#include "AsyncProcess.h"

#include "trace.h"
ENABLE_TRACE;

DEFINE_LOCAL_EVENT_TYPE(wxEVT_NXSSH);
DEFINE_LOCAL_EVENT_TYPE(wxEVT_NXSERVICE);
DEFINE_LOCAL_EVENT_TYPE(wxEVT_GENERIC);

IMPLEMENT_CLASS(MyIPC, wxEvtHandler);

BEGIN_EVENT_TABLE(MyIPC, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, wxEVT_PROCESS_STDOUT, MyIPC::OnOutReceived )
    EVT_COMMAND(wxID_ANY, wxEVT_PROCESS_STDERR, MyIPC::OnErrReceived )
    EVT_COMMAND(wxID_ANY, wxEVT_PROCESS_EXIT,   MyIPC::OnTerminate )
END_EVENT_TABLE();

MyIPC::MyIPC()
    : m_pProcess(NULL)
    , m_pEvtHandler(NULL)
    , m_cEvhMutex()
    , m_eType(TypeNone)
    , m_iOutCollect(0)
    , m_iErrCollect(0)
    , m_nLines618(0)
    , m_sOutMessage(wxEmptyString)
    , m_sErrMessage(wxEmptyString)
{
#ifdef __WXMSW__
    m_MsgSession = ::RegisterWindowMessage(wxT("NX_SESSION_MESSAGE"));
    m_MsgKill = ::RegisterWindowMessage(wxT("NxKill"));
#endif
    m_re = new wxRegEx();
    m_re->Compile(wxT("\\(yes/no\\)\\?"), wxRE_ADVANCED|wxRE_ICASE);
}

MyIPC::~MyIPC()
{
    ::myLogTrace(MYTRACETAG, wxT("~MyIPC() called"));
    Detach();
    delete m_re;
}

    void
MyIPC::Detach()
{
    ::myLogTrace(MYTRACETAG, wxT("Detach() called"));
    m_cEvhMutex.Lock();
    m_pEvtHandler = NULL;
    m_cEvhMutex.Unlock();
    if (m_pProcess) {
        if (m_pProcess->IsRunning())
            m_pProcess->Detach();
        else
            delete m_pProcess;
        m_pProcess = NULL;
    }
}

    void
MyIPC::sendUpEvent(wxCommandEvent &event)
{
    wxMutexLocker lock(m_cEvhMutex);
    if (lock.IsOk() && (NULL != m_pEvtHandler)) {
        m_pEvtHandler->AddPendingEvent(event);
    }
}

    bool
MyIPC::IsRunning()
{
    return (m_pProcess && m_pProcess->IsRunning());
}

int MyIPC::GetResult()
{
    return m_pProcess ? m_pProcess->GetStatus() : -1;
}

    bool
MyIPC::Kill()
{
    bool ret (m_pProcess && m_pProcess->Kill());
    m_pProcess = NULL;
    return ret;
}

    void
MyIPC::Print(const wxString &s, bool doLog /* = true */ )
{
    if (m_pProcess)
        m_pProcess->Print(s, doLog);
}

    bool
MyIPC::GenericProcess(const wxString &cmd, const wxString &dir, wxEvtHandler *h)
{
    m_eType = TypeNone;
    bool ret = false;
    m_iOutCollect = 0;
    m_iErrCollect = 0;
    m_sOutMessage.Empty();
    m_sErrMessage.Empty();
    m_pEvtHandler = h;
    m_pProcess = new AsyncProcess(cmd, dir, this);
    ret = m_pProcess->Start();
    if (!ret) {
        delete m_pProcess;
        m_pProcess = NULL;
    }
    return ret;
}

    bool
MyIPC::SshProcess(const wxString &cmd, const wxString &dir, wxEvtHandler *h)
{
    m_eType = TypeSsh;
    bool ret = false;
    m_iOutCollect = 0;
    m_iErrCollect = 0;
    m_sOutMessage.Empty();
    m_sErrMessage.Empty();
    m_pEvtHandler = h;
    m_pProcess = new AsyncProcess(cmd, dir, wxT("NX> 105 "), this);
    ret = m_pProcess->Start();
    if (!ret) {
        delete m_pProcess;
        m_pProcess = NULL;
    }
    return ret;
}

    int
MyIPC::parseCode(const wxString &buf)
{
    int ret = -1;
    long n;
    if (buf.Find(wxT("NX> ")) == 0) {
        int idx = 0;
        while (buf.Mid(4+idx, 1).IsNumber()) {
            if (ret == -1)
                ret = 0;
            buf.Mid(4+idx, 1).ToLong(&n);
            ret *= 10;
            ret += n;
            idx++;
        }
    }
    return ret;
}

    void
MyIPC::OnTerminate(wxCommandEvent &event)
{
    switch (m_eType) {
        case TypeNone:
            ::myLogTrace(MYTRACETAG, wxT("process terminated"));
            if (m_pEvtHandler) {
                wxCommandEvent upevent(wxEVT_GENERIC, wxID_ANY);
                upevent.SetInt(ActionTerminated);
                sendUpEvent(upevent);
            }
            break;
        case TypeSsh:
            ::myLogTrace(MYTRACETAG, wxT("nxssh terminated"));
            if (m_pEvtHandler) {
                wxCommandEvent upevent(wxEVT_NXSSH, wxID_ANY);
                upevent.SetInt(ActionTerminated);
                sendUpEvent(upevent);
            }
            break;
    }
    event.Skip();
}

    void
MyIPC::OnOutReceived(wxCommandEvent &event)
{
    wxString msg;
    int code;

    switch (m_eType) {
        case TypeNone:
            ::myLogTrace(MYTRACETAG, wxT("process O: '%s'"), msg.c_str());
            if (m_pEvtHandler) {
                wxCommandEvent upevent(wxEVT_GENERIC, wxID_ANY);
                upevent.SetInt(ActionStdout);
                upevent.SetString(event.GetString());
                sendUpEvent(upevent);
            }
            break;
        case TypeSsh:
            msg = event.GetString();
            // Normalize "FREENX>" -> "NX>"
            if (msg.StartsWith(wxT("FREENX>")))
                    msg = msg.Mid(4);
            code = parseCode(msg);
            ::myLogTrace(MYTRACETAG, wxT("nxssh O[%04d]: '%s'"), code, msg.c_str());
            if (m_pEvtHandler) {
                wxCommandEvent upevent(wxEVT_NXSSH, wxID_ANY);
                upevent.SetInt(ActionLog);
                upevent.SetString(msg);
                sendUpEvent(upevent);
                switch (code) {
                    case -1:
                        // No code found
                        if (m_iOutCollect) {
                            m_iOutCollect--;
                            m_sOutMessage << wxT("\n") << msg;
                            if (m_iOutCollect == 0) {
                                m_re->Replace(&m_sOutMessage, wxEmptyString);
                                upevent.SetString(m_sOutMessage + wxT("?"));
                                upevent.SetInt(ActionPromptYesNo);
                                sendUpEvent(upevent);
                            }
                            break;
                        }
                        if (msg.StartsWith(wxT("HELLO NXSERVER - Version "))) {
                            upevent.SetString(msg.Mid(25).BeforeFirst(wxT(' ')).Strip(wxString::both).BeforeFirst(wxT('-')));
                            upevent.SetInt(ActionHello);
                            sendUpEvent(upevent);
                            break;
                        }
                        if (msg.StartsWith(wxT("Warning: the RSA host key"))) {
                            m_sOutMessage = msg;
                            m_iOutCollect = 4;
                            break;
                        }
                        if (msg.StartsWith(wxT("Enter PIN for"))) {
                            // Generated by scard-opensc
                            upevent.SetInt(ActionPinDialog);
                            sendUpEvent(upevent);
                        }
                        break;
                    case 100:
                        // Server version & licence
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionStatus);
                        sendUpEvent(upevent);
                        break;
                    case 101:
                        // request username
                        upevent.SetInt(ActionSendUsername);
                        sendUpEvent(upevent);
                        break;
                    case 102:
                        // request password
                        upevent.SetInt(ActionSendPassword);
                        sendUpEvent(upevent);
                        break;
                    case 103:
                        // Welcome message
                        upevent.SetInt(ActionWelcome);
                        sendUpEvent(upevent);
                        break;
                    case 104:
                        // NX4 resource list
                        upevent.SetInt(ActionResList);
                        sendUpEvent(upevent);
                        break;
                    case 105:
                        // request for next command
                        upevent.SetInt(ActionNextCommand);
                        sendUpEvent(upevent);
                        break;
                    case 106:
                        // Result: server parameters
                    case 110:
                        // Result: server status (running|stopped)
                    case 113:
                        // Announce: changing password
                    case 114:
                        // Announce: password changed
                    case 122:
                        // Result: service started
                    case 123:
                        // Result: service stopped
                        break;
                    case 127:
                        // Result: session list
                        upevent.SetInt(ActionSessionListStart);
                        sendUpEvent(upevent);
                        break;
                    case 134:
                        // NX> 134 Accepted protocol: <version>
                        break;
                    case 140:
                        // Set length for push session config
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionSessionPushLength);
                        sendUpEvent(upevent);
                        break;
                    case 141:
                        // Push session config
                        upevent.SetInt(ActionSessionPushStart);
                        sendUpEvent(upevent);
                        break;
                    case 146:
                        // Result: user list
                        break;
                    case 147:
                        // Result: server capacity reached
                    case 148:
                        // Result: server capacity not reached
                        upevent.SetInt(ActionSessionListEnd);
                        upevent.SetExtraLong(code);
                        sendUpEvent(upevent);
                        break;
                        // Codes 200 - 299 are from local nxssh
                    case 200:
                        // Connected to adress ...
                        break;
#ifdef OBSOLETE_CODE
                    // Apparently gone
                    case 201:
                        ret = wxT("NX Server not installed or the server access has been disabled");
                        return ActionError;
#endif
                    case 202:
                        // Authenticating user ...
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionStatus);
                        sendUpEvent(upevent);
                        break;
                    case 203:
                        // NXSSH started with pid: ...
                        msg = msg.AfterFirst(wxT(':')).Strip(wxString::both);
                        if (msg.IsNumber()) {
                            long n;
                            msg.ToLong(&n);
                            m_iSshPid = n;
                        }
                        break;
                    case 204:
                        // Authentication failed
                        upevent.SetString(_("Authentication failed"));
                        upevent.SetInt(ActionError);
                        sendUpEvent(upevent);
                        break;
                    case 205:
                        // ?? generic
                        m_iOutCollect = 2;
                        m_sOutMessage = msg.Mid(8);
                        break;
                    case 208:
                        // Using auth method: ...
                        break;
                    case 209:
                        // Remote host authentication has changed
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionWarning);
                        sendUpEvent(upevent);
                        break;
                    case 210:
                        // Enter passphrase for key ...
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionPassphraseDialog);
                        sendUpEvent(upevent);
                        break;
                    case 211:
                        // The authenticity of host ... can't be verified ... (multiline message)
                        m_iOutCollect = 2;
                        m_sOutMessage = msg.Mid(8);
                        break;
                    case 250:
                        // NX4 request username or password
                        if (msg.Find(wxT("login required")) != wxNOT_FOUND) {
                            upevent.SetInt(ActionSendUsername);
                            sendUpEvent(upevent);
                        }
                        if (msg.Find(wxT("password required")) != wxNOT_FOUND) {
                            upevent.SetInt(ActionSendPassword);
                            sendUpEvent(upevent);
                        }
                        break;
                    case 280:
                    case 282:
                    case 285:
                    case 286:
                        break;
                    case 287:
                        // Successfully redirected ports
                        upevent.SetInt(ActionSessionRunning);
                        sendUpEvent(upevent);
                        break;
                    case 288:
                    case 289:
                        // Debug messages ?
                    case 290:
                    case 291:
                    case 292:
                    case 294:
                    case 295:
                    case 296:
                    case 297:
                    case 298:
                    case 299:
                        // Subchannel system ?
                        break;
#ifdef OBSOLETE_CODE
                        // Apparently gone
                    case 300:
                    case 301:
                        ret = wxT("Connection error");
                        return -99;
                    case 302:
                    case 303:
                        ret = wxT("Connection refused");
                        return -99;
                    case 304:
                    case 305:
                        ret = wxT("Host not found");
                        return -99;
                    case 306:
                        ret = wxT("Network unreachable");
                        return -99;
                    case 307:
                    case 310:
                        ret = wxT("Host key verification failed");
                        return -99;
#endif
                    case 404:
                        // Error: wrong password or login
                    case 500:
                        // Generic error
                        upevent.SetString(m_s595msg + msg.Mid(8));
                        m_s595msg.Empty();
                        upevent.SetInt(ActionError);
                        sendUpEvent(upevent);
                        break;
                    case 595:
                        // Fatal server error (usually multiline and
                        // followed by a 500
                        m_s595msg << msg.Mid(15) << wxT("\n");
                        break;
                    case 503:
                        // Unknown command
                    case 504:
                        // Session startup failed
                    case 536:
                        // User limit exceeded
                    case 537:
                        // Passwords did not match
                    case 542:
                        // Max # of guest sessions reached
                    case 596:
                        // No running session, resp. Session startup failed
                    case 598:
                        // Unrecoverable exception
                    case 599:
                        // Reached the maximum number of concurrent sessions on this server.
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionError);
                        sendUpEvent(upevent);
                        break;
                    case 618:
                        // Evaluation license expired (commercial NoMachine server) - 3 lines
                        if (3 > ++m_nLines618) {
                            m_s618msg.Append(msg.Mid(8)).Append(wxT("\n"));
                        } else {
                            m_nLines618 = 0;
                            m_s618msg.Append(msg.Mid(8));
                            upevent.SetString(m_s618msg);
                            upevent.SetInt(ActionError);
                            sendUpEvent(upevent);
                        }
                        break;
                    case 700:
                        // Session ID
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetSessionID);
                        sendUpEvent(upevent);
                        break;
                    case 701:
                        // Session proxy cookie
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetProxyCookie);
                        sendUpEvent(upevent);
                        break;
                    case 702:
                        // Session proxy IP
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetProxyIP);
                        sendUpEvent(upevent);
                        break;
                    case 703:
                        // Session type
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetSessionType);
                        sendUpEvent(upevent);
                        break;
                    case 704:
                        // Session cache
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetSessionCache);
                        sendUpEvent(upevent);
                        break;
                    case 705:
                        // Session display
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetSessionDisplay);
                        sendUpEvent(upevent);
                        break;
                    case 706:
                        // Session agent cookie
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetAgentCookie);
                        sendUpEvent(upevent);
                        break;
                    case 707:
                        // Session ssl tunneling
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetSslTunneling);
                        sendUpEvent(upevent);
                        break;
                    case 708:
                        // Subscription
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetSubscription);
                        sendUpEvent(upevent);
                        break;
                    case 709:
                        // SMB Port
                        upevent.SetString(msg.AfterFirst(wxT(':')).Strip(wxString::both));
                        upevent.SetInt(ActionSetSmbPort);
                        sendUpEvent(upevent);
                        break;
                    case 710:
                        // session running
                        upevent.SetInt(ActionStartProxy);
                        sendUpEvent(upevent);
                        break;
                    case 716:
                        // terminationg session on user request
                    case 718:
                        // session restore successful
                        break;
                    case 719:
                        // SMB filesystem: running
                        /* Workaround for newer nxnodes (since 3.2.0?):
                         * On those, the message "NX> 709 Filesharing Port: nnn" is missing.
                         * Therefore, we now use message 719 and hardcode the port
                         * to SMBFS (139) if running on Windows and CIFS (445) otherwise.
                         */
#ifdef __WXMSW__
                        upevent.SetString(wxT("139"));
#else
                        upevent.SetString(wxT("445"));
#endif
                        upevent.SetInt(ActionSetSmbPort);
                        sendUpEvent(upevent);
                        break;
                    case 720:
                        // CUPS printer: running
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionStatus);
                        sendUpEvent(upevent);
                        break;
                    case 725:
                        // Shadow: Geometry 1024x768x24
                        upevent.SetString(msg.Mid(25).Strip(wxString::both));
                        upevent.SetInt(ActionSetShadowGeometry);
                        sendUpEvent(upevent);
                        break;
                    case 726:
                        // Asking user for authorization to attach to session
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionStatus);
                        sendUpEvent(upevent);
                        break;
                    case 728:
                        // Session caption: NX - felfert@nx3-test.id-testlab.str.topalis:1008 - nx3-test
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionStatus);
                        sendUpEvent(upevent);
                        break;
                    case 900:
                        // Terminating session
                    case 1000:
                        // nxnode version
                    case 1001:
                        // Bye
                    case 1002:
                        // Commit
                        break;
                    case 1004:
                        // Session status: failed
                        upevent.SetString(msg.Mid(8));
                        upevent.SetInt(ActionError);
                        sendUpEvent(upevent);
                        break;
                    case 1005:
                        // Session status: suspended
                    case 1006:
                        // Session status: running|closed
                    case 1009:
                        // Session status: terminating
                        break;
                    case 999:
                        // Terminating
                        upevent.SetInt(ActionExit);
                        sendUpEvent(upevent);
                        break;
                }
            }
            break;
    }
    event.Skip();
}

    void
MyIPC::OnErrReceived(wxCommandEvent &event)
{
    wxString msg = event.GetString();
    int code;

    switch (m_eType) {
        case TypeNone:
            ::myLogTrace(MYTRACETAG, wxT("process E: '%s'"), msg.c_str());
            if (m_pEvtHandler) {
                wxCommandEvent upevent(wxEVT_GENERIC, wxID_ANY);
                upevent.SetInt(ActionStderr);
                upevent.SetString(event.GetString());
                sendUpEvent(upevent);
            }
            break;
        case TypeSsh:
            code = parseCode(msg);
            ::myLogTrace(MYTRACETAG, wxT("nxssh E[%04d]: '%s'"), code, msg.c_str());
            if (m_pEvtHandler) {
                wxCommandEvent upevent(wxEVT_NXSSH, wxID_ANY);
                upevent.SetInt(ActionLog);
                upevent.SetString(event.GetString());
                sendUpEvent(upevent);
                switch (code) {
                    case -1:
                        // No code found
                        if (m_iErrCollect) {
                            m_iErrCollect--;
                            m_sErrMessage << wxT("\n") << msg;
                            if (m_iErrCollect == 0) {
                                upevent.SetString(m_sErrMessage);
                                upevent.SetInt(ActionError);
                                sendUpEvent(upevent);
                            }
                            break;
                        }
                        if (msg.StartsWith(wxT("Offending key in "))) {
                            upevent.SetString(msg.Mid(17));
                            upevent.SetInt(ActionOffendingKey);
                            sendUpEvent(upevent);
                            break;
                        }
                        if (msg.Contains(wxT("WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!"))) {
                            m_sErrMessage = msg.AfterFirst(wxT('@')).BeforeLast(wxT('@')).Strip(wxString::both);
                            m_iErrCollect = 99;
                            break;
                        }
                        if (msg.Contains(wxT("usage:"))) {
                            m_sErrMessage = msg;
                            m_iErrCollect = 6;
                            break;
                        }
                        if (msg.Contains(wxT("Connection refused")) ||
                                msg.Contains(wxT("onnection closed")) ||
                                msg.Contains(wxT("no address associated")) ||
                                msg.Contains(wxT("failed with error")) ||
                                msg.Contains(wxT("Smartcard init failed")) ||
                                msg.Contains(wxT("PIN code verification failed")) ||
                                msg.Contains(wxT("No certificates found on smartcard")) ||
                                msg.Contains(wxT("proxy error:"))
                                ) {
                            upevent.SetString(msg);
                            upevent.SetInt(ActionError);
                            sendUpEvent(upevent);
                            break;
                        }
                        if (msg.Contains(wxT("Warning:"))) {
                            upevent.SetString(msg.Mid(8));
                            upevent.SetInt(ActionWarning);
                            sendUpEvent(upevent);
                            break;
                        }
                        break;
                    case 209:
                        // Remote host identification changed
                        m_iErrCollect = 0;
                        m_sErrMessage.Replace(wxT("@"), wxEmptyString);
                        m_sErrMessage.Remove(m_sErrMessage.Find(wxT("Please contact")));
                        upevent.SetString(m_sErrMessage);
                        upevent.SetInt(ActionKeyChangedYesNo);
                        sendUpEvent(upevent);
                        break;
                    case 287:
                        // Successfully redirected ports
                        upevent.SetInt(ActionSessionRunning);
                        sendUpEvent(upevent);
                        break;
                    case 999:
                        upevent.SetInt(ActionExit);
                        sendUpEvent(upevent);
                        break;
                }
            }
    }
    event.Skip();
}

long MyIPC::GetPID()
{
    if (m_pProcess && (m_pProcess->IsRunning()))
        return m_pProcess->GetPid();
    return 0;
}

// vim:cindent:expandtab:shiftwidth=4
