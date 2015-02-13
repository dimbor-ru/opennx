// $Id: SessionAdmin.cpp 688 2012-02-18 02:36:07Z felfert $
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
#pragma implementation "SessionAdmin.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include <wx/filename.h>
#include <wx/config.h>
#include <wx/process.h>

#include "LogDialog.h"
#include "opennxApp.h"
#include "pwcrypt.h"
#include "MySession.h"
#include "SessionAdmin.h"
#include "SessionList.h"
#include "LoginDialog.h"
#include "AboutDialog.h"
#include "Icon.h"

#ifdef __UNIX__
#include <sys/types.h>
#include <signal.h>
#endif

#include "osdep.h"

#include "trace.h"
ENABLE_TRACE;

////@begin XPM images
////@end XPM images

/*!
 * SessionAdmin type definition
 */

IMPLEMENT_CLASS( SessionAdmin, wxFrame )

/*!
 * SessionAdmin event table definition
 */

BEGIN_EVENT_TABLE( SessionAdmin, wxFrame )

////@begin SessionAdmin event table entries
    EVT_MENU( XRCID("ID_MENU_SESSION_NEW"), SessionAdmin::OnMenuSessionNewClick )

    EVT_MENU( wxID_PREFERENCES, SessionAdmin::OnPREFERENCESClick )

    EVT_MENU( wxID_EXIT, SessionAdmin::OnEXITClick )

    EVT_MENU( XRCID("ID_MENU_SESSION_TERMINATE"), SessionAdmin::OnMenuSessionTerminateClick )

    EVT_MENU( XRCID("ID_MENU_SESSION_PSTATS"), SessionAdmin::OnMenuSessionPstatsClick )

    EVT_MENU( XRCID("ID_MENU_SESSION_FSTATS"), SessionAdmin::OnMenuSessionFstatsClick )

    EVT_MENU( XRCID("ID_MENU_SESSION_LOG"), SessionAdmin::OnMenuSessionLogClick )

    EVT_MENU( XRCID("ID_MENU_SESSION_REMOVE"), SessionAdmin::OnMenuSessionRemoveClick )

    EVT_MENU( XRCID("ID_MENU_SESSION_KILL"), SessionAdmin::OnMenuSessionKillClick )

    EVT_MENU( XRCID("ID_MENU_REFRESH"), SessionAdmin::OnMenuRefreshClick )

    EVT_MENU( wxID_ABOUT, SessionAdmin::OnABOUTClick )

    EVT_MENU( XRCID("ID_TOOL_SESSION_NEW"), SessionAdmin::OnToolSessionNewClick )

    EVT_MENU( XRCID("ID_TOOL_SESSION_TERMINATE"), SessionAdmin::OnToolSessionTerminateClick )

    EVT_MENU( XRCID("ID_TOOL_SESSION_PSTATS"), SessionAdmin::OnToolSessionPstatsClick )

    EVT_MENU( XRCID("ID_TOOL_SESSION_FSTATS"), SessionAdmin::OnToolSessionFstatsClick )

    EVT_MENU( XRCID("ID_TOOL_SESSION_LOG"), SessionAdmin::OnToolSessionLogClick )

    EVT_MENU( XRCID("ID_TOOL_REFRESH"), SessionAdmin::OnToolRefreshClick )

    EVT_MENU( XRCID("ID_TOOL_SESSION_REMOVE"), SessionAdmin::OnToolSessionRemoveClick )

    EVT_MENU( XRCID("ID_TOOL_SESSION_KILL"), SessionAdmin::OnToolSessionKillClick )

    EVT_LIST_ITEM_SELECTED( XRCID("ID_LISTCTRL"), SessionAdmin::OnListctrlSelected )
    EVT_LIST_ITEM_DESELECTED( XRCID("ID_LISTCTRL"), SessionAdmin::OnListctrlDeselected )

////@end SessionAdmin event table entries

    EVT_COMMAND(wxID_ANY, wxEVT_SESSIONLIST_ACTION, SessionAdmin::OnSessionList )

END_EVENT_TABLE()

/*!
 * SessionAdmin constructors
 */

SessionAdmin::SessionAdmin()
{
}

SessionAdmin::SessionAdmin( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create( parent, id, caption, pos, size, style );
}

SessionAdmin::~SessionAdmin()
{
    delete m_sessions;
}

/*!
 * SessionAdmin creator
 */

bool SessionAdmin::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
////@begin SessionAdmin member initialisation
    m_SessionListCtrl = NULL;
////@end SessionAdmin member initialisation

    wxConfigBase::Get()->Read(_T("Config/UserNxDir"), &m_NxDirectory);

////@begin SessionAdmin creation
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/opennx-admin.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end SessionAdmin creation

    m_sessions = new SessionList(m_NxDirectory, this);
    return TRUE;
}

/*!
 * Control creation for SessionAdmin
 */

void SessionAdmin::CreateControls()
{    
////@begin SessionAdmin content construction
    if (!wxXmlResource::Get()->LoadFrame(this, GetParent(), wxT("ID_FRAME_ADMIN")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_SessionListCtrl = XRCCTRL(*this, "ID_LISTCTRL", wxListCtrl);
////@end SessionAdmin content construction

    // Create custom windows not generated automatically here.

////@begin SessionAdmin content initialisation
////@end SessionAdmin content initialisation

    SessionToolsEnable(false);
    m_SessionListCtrl->InsertColumn(0, _("Server"));
    m_SessionListCtrl->InsertColumn(1, _("Port"));
    m_SessionListCtrl->InsertColumn(2, _("Session ID"));
    m_SessionListCtrl->InsertColumn(3, _("Creation Date"));
    m_SessionListCtrl->InsertColumn(4, _("PID"));
    m_SessionListCtrl->InsertColumn(5, _("Status"));
    m_SessionListCtrl->InsertColumn(6, _("Type"));
    for (int i = 0; i < m_SessionListCtrl->GetColumnCount(); i++) 
        m_SessionListCtrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
}


void SessionAdmin::SessionToolsEnable(bool enable, bool running /* = false */)
{
    GetToolBar()->EnableTool(XRCID("ID_TOOL_SESSION_TERMINATE"), enable && running);
    GetToolBar()->EnableTool(XRCID("ID_TOOL_SESSION_PSTATS"), enable && running);
    GetToolBar()->EnableTool(XRCID("ID_TOOL_SESSION_FSTATS"), enable && running);
    GetToolBar()->EnableTool(XRCID("ID_TOOL_SESSION_LOG"), enable);
    GetToolBar()->EnableTool(XRCID("ID_TOOL_SESSION_REMOVE"), enable && !running);
    GetToolBar()->EnableTool(XRCID("ID_TOOL_SESSION_KILL"), enable && running);
    GetMenuBar()->FindItem(XRCID("ID_MENU_SESSION_TERMINATE"))->Enable(enable && running);
    GetMenuBar()->FindItem(XRCID("ID_MENU_SESSION_PSTATS"))->Enable(enable && running);
    GetMenuBar()->FindItem(XRCID("ID_MENU_SESSION_FSTATS"))->Enable(enable && running);
    GetMenuBar()->FindItem(XRCID("ID_MENU_SESSION_LOG"))->Enable(enable);
    GetMenuBar()->FindItem(XRCID("ID_MENU_SESSION_REMOVE"))->Enable(enable && !running);
    GetMenuBar()->FindItem(XRCID("ID_MENU_SESSION_KILL"))->Enable(enable && running);
}

/*!
 * Should we show tooltips?
 */

bool SessionAdmin::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap SessionAdmin::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon SessionAdmin::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}

void SessionAdmin::ShowSessionStats(long item, bool full)
{
    MySession *s =
        wxDynamicCast((void *)m_SessionListCtrl->GetItemData(item), MySession);
    if (s) {
        wxFileName fn(s->sGetDir(), wxT("stats"));
        wxString md5stats1 = Md5OfFile(fn.GetFullPath());
        wxString md5stats2 = md5stats1;
        bool ok = false;
#ifdef __UNIX__
        ok = (kill((pid_t)s->lGetPID(), full ? SIGUSR1 : SIGUSR2) == 0);
#else
#endif
        if (ok) {
            // Wait until file starts changing
            ::myLogTrace(MYTRACETAG, wxT("wait for change of stats"));
            while (ok) {
                md5stats2 = Md5OfFile(fn.GetFullPath());
                if (md5stats1 != md5stats2)
                    break;
                ::wxGetApp().Yield(true);
                wxThread::Sleep(100);
            }
            // Wait until file stopped changing
            wxThread::Sleep(100);
            ::myLogTrace(MYTRACETAG, wxT("wait for settling stats"));
            while (ok) {
                md5stats1 = Md5OfFile(fn.GetFullPath());
                if (md5stats1 == md5stats2)
                    break;
                md5stats2 = md5stats1;
                ::wxGetApp().Yield(true);
                wxThread::Sleep(100);
            }
            if (ok) {
                LogDialog d(NULL);
                d.SetTitle(full ?
                        _("Full session statistics - OpenNX") :
                        _("Partial session statistics - OpenNX"));
                d.ReadLogFile(s->sGetDir() + wxFileName::GetPathSeparator() + wxT("stats"));
                d.ShowModal();
            }
        }
    }
}

/*!
 * Handle events from SessionList
 */
void SessionAdmin::OnSessionList(wxCommandEvent& event)
{
    long idx;
    MySession *s;
    switch (event.GetInt()) {
        case SessionList::SessionAdded:
            s = wxDynamicCast((void *)event.GetClientData(), MySession);
            // Hostname
            idx = m_SessionListCtrl->InsertItem(0, s->sGetHost(), 0);
            m_SessionListCtrl->SetItemData(idx, (long)s);
            // Port
            m_SessionListCtrl->SetItem(idx, 1, s->sGetPort());
            // Session ID
            m_SessionListCtrl->SetItem(idx, 2, s->sGetMd5());
            // Creation Time
            m_SessionListCtrl->SetItem(idx, 3, s->sGetCreationTime());
            // PID
            m_SessionListCtrl->SetItem(idx, 4, s->sGetPID());
            // Status
            m_SessionListCtrl->SetItem(idx, 5, s->sGetSessionStatus());
            // Type
            m_SessionListCtrl->SetItem(idx, 6, s->sGetSessionType());
            break;
        case SessionList::SessionChanged:
            s = wxDynamicCast((void *)event.GetClientData(), MySession);
            idx = m_SessionListCtrl->FindItem(-1, (long)s);
            ::myLogTrace(MYTRACETAG, wxT("state changed: %d"), idx);
            if (idx != -1) {
                m_SessionListCtrl->SetItem(idx, 3, s->sGetCreationTime());
                m_SessionListCtrl->SetItem(idx, 4, s->sGetPID());
                m_SessionListCtrl->SetItem(idx, 5, s->sGetSessionStatus());
                if (m_SessionListCtrl->GetItemState(idx, wxLIST_STATE_SELECTED)) {
                    bool running = (s->eGetSessionStatus() == MySession::Running);
                    SessionToolsEnable(true, running);
                }
            }
            break;
        case SessionList::SessionRemoved:
            idx = m_SessionListCtrl->FindItem(-1, (long)event.GetClientData());
            if (idx != -1) {
                m_SessionListCtrl->DeleteItem(idx);
            }
            break;
        case SessionList::UpdateList:
#ifndef __WXMAC__ // On OSX, this messes up column headers
            {
                int width = m_SessionListCtrl->GetItemCount() ?
                    wxLIST_AUTOSIZE : wxLIST_AUTOSIZE_USEHEADER;
                for (int i = 0; i < m_SessionListCtrl->GetColumnCount(); i++) 
                    m_SessionListCtrl->SetColumnWidth(i, width);
            }
#endif
            m_SessionListCtrl->Update();
            break;
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL_SESSION_NEW
 */

void SessionAdmin::OnToolSessionNewClick( wxCommandEvent& event )
{
    OnMenuSessionNewClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CHDIR
 */

void SessionAdmin::OnPREFERENCESClick( wxCommandEvent& )
{
    const wxString& dir = wxDirSelector(_("Choose new NX session directory."),
            m_NxDirectory, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if (!dir.IsEmpty()) {
        m_NxDirectory = dir;
        m_sessions->SetDir(dir);
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_EXIT
 */

void SessionAdmin::OnEXITClick( wxCommandEvent& )
{
    Close();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_TERMINATE
 */

void SessionAdmin::OnToolSessionTerminateClick( wxCommandEvent& event )
{
    OnMenuSessionTerminateClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_PSTATS
 */

void SessionAdmin::OnToolSessionPstatsClick( wxCommandEvent& event )
{
    OnMenuSessionPstatsClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_FSTATS
 */

void SessionAdmin::OnToolSessionFstatsClick( wxCommandEvent& event )
{
    OnMenuSessionFstatsClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_LOG
 */

void SessionAdmin::OnToolSessionLogClick( wxCommandEvent& event )
{
    OnMenuSessionLogClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_REMOVE
 */

void SessionAdmin::OnToolSessionRemoveClick( wxCommandEvent& event )
{
    OnMenuSessionRemoveClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_KILL
 */

void SessionAdmin::OnToolSessionKillClick( wxCommandEvent& event )
{
    OnMenuSessionKillClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_OPTIONS_REFRESH
 */

void SessionAdmin::OnToolRefreshClick( wxCommandEvent& event )
{
    OnMenuRefreshClick(event);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_HELP_ABOUT
 */

void SessionAdmin::OnABOUTClick( wxCommandEvent& )
{
    AboutDialog d(this);
    d.ShowModal();
}

/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL
 */

void SessionAdmin::OnListctrlSelected( wxListEvent& event )
{
    MySession *s =
        wxDynamicCast((void *)m_SessionListCtrl->GetItemData(event.GetIndex()), MySession);
    bool running = ((NULL != s) && (s->eGetSessionStatus() == MySession::Running));
    SessionToolsEnable(true, running);
}

/*!
 * wxEVT_COMMAND_LIST_ITEM_DESELECTED event handler for ID_LISTCTRL
 */

void SessionAdmin::OnListctrlDeselected( wxListEvent& )
{
    SessionToolsEnable(false);
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_NEW
 */

void SessionAdmin::OnMenuSessionNewClick( wxCommandEvent& )
{
    LoginDialog d(this);
    d.ShowModal();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_TERMINATE
 */

void SessionAdmin::OnMenuSessionTerminateClick( wxCommandEvent& )
{
#ifndef __WXMSW__
    long item = m_SessionListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1) {
        MySession *s =
            wxDynamicCast((void *)m_SessionListCtrl->GetItemData(item), MySession);
        if (s)
            close_sid(s->sGetMd5().mb_str());
    }
#endif
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_PSTATS
 */

void SessionAdmin::OnMenuSessionPstatsClick( wxCommandEvent& )
{
    long item = m_SessionListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1)
        ShowSessionStats(item, false);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_FSTATS
 */

void SessionAdmin::OnMenuSessionFstatsClick( wxCommandEvent& )
{
    long item = m_SessionListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1)
        ShowSessionStats(item, true);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_LOG
 */

void SessionAdmin::OnMenuSessionLogClick( wxCommandEvent& )
{
    long item = m_SessionListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1) {
        MySession *s =
            wxDynamicCast((void *)m_SessionListCtrl->GetItemData(item), MySession);
        if (s) {
            LogDialog d(NULL);
            d.ReadLogFile(s->sGetDir() + wxFileName::GetPathSeparator() + wxT("session"));
            d.ShowModal();
        }
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_REMOVE
 */

void SessionAdmin::OnMenuSessionRemoveClick( wxCommandEvent& )
{
    long item = m_SessionListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1) {
        MySession *s =
            wxDynamicCast((void *)m_SessionListCtrl->GetItemData(item), MySession);
        if (s) {
            m_SessionListCtrl->DeleteItem(item);
            wxString dir = s->sGetDir();
            m_sessions->CleanupDir(dir);
        }
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_SESSION_KILL
 */

void SessionAdmin::OnMenuSessionKillClick( wxCommandEvent& )
{
    long item = m_SessionListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1) {
        MySession *s =
            wxDynamicCast((void *)m_SessionListCtrl->GetItemData(item), MySession);
        wxProcess::Kill(s->lGetPID());
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_REFRESH
 */

void SessionAdmin::OnMenuRefreshClick( wxCommandEvent& )
{
    m_sessions->ScanDir();
}


