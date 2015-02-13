// $Id: opennxApp.h 725 2013-11-25 15:59:59Z felfert $
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

#ifndef opennxApp_H
#define opennxApp_H

#include <wx/intl.h>

class wxConfigBase;
class wxTaskBarIcon;
class MyXmlConfig;
class LoginDialog;

// Define a new application type, each program should derive a class from wxApp
class opennxApp : public wxApp
{
    public:
        opennxApp();
        virtual ~opennxApp();

        wxLocale *GetLocale() { return &m_cLocale; }
        const wxString &GetResourcePrefix() { return m_sResourcePrefix; }
        const wxString &GetVersion() { return m_sVersion; }
        const wxString &GetSelfPath() { return m_sSelfPath; }
        const wxString &GetCaCert() { return m_sCaCert; }

        wxString LoadFileFromResource(const wxString &loc, bool bUseLocale = true);
#ifdef __WXMAC__
        // Respond to Apple Event for opening a document
        virtual void MacOpenFile(const wxString& filename);
#endif
        bool CreateDesktopEntry(MyXmlConfig *);
        bool RemoveDesktopEntry(MyXmlConfig *);
        bool CheckDesktopEntry(MyXmlConfig *);
        /**
         * Return status of SmartCard support
         * in nxssh. If nxssh has SmartCard support
         * compiled in, return true, otherwise return false.
         */
        bool NxSmartCardSupport() { return m_bNxSmartCardSupport; }
        bool LibUSBAvailable() { return m_bLibUSBAvailable; }
        bool NxProxyAvailable() { return m_bNxProxyAvailable; }
        bool AutoLogin() { return m_bAutoLogin; }
        bool AutoResume() { return m_bAutoResume; }

        void SetRequireWatchReader(bool b) { m_bRequireWatchReader = b; }
        void SetRequireStartUsbIp(bool b) { m_bRequireStartUsbIp = b; }
        void SetNxSshPID(long l) { m_nNxSshPID = l; }
        void SetReader(int i) { m_iReader = i; }
        void SetSessionCfg(MyXmlConfig &);
        void SetSessionID(const wxString &s) { m_sSessionID = s; };
        void EnableContextHelp(wxWindow *w);

        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        virtual int OnExit();
        int FilterEvent(wxEvent& event);

    private:
        enum mode {
            MODE_INVALID = -1,
            MODE_CLIENT = 0,
            MODE_EXPORTRES,
            MODE_ADMIN,
            MODE_WIZARD,
            MODE_MAC_WAITOPEN,
            MODE_DIALOG_OK,
            MODE_DIALOG_YESNO,
            MODE_DIALOG_ERROR,
            MODE_DIALOG_PANIC,
            MODE_DIALOG_QUIT,
            MODE_FOREIGN_TOOLBAR,
        };

        wxConfigBase *m_pCfg;
        MyXmlConfig *m_pSessionCfg;
        wxLocale m_cLocale;
        wxString m_sSessionName;
        wxString m_sSessionID;
        wxString m_sResourcePrefix;
        wxString m_sVersion;
        wxString m_sSelfPath;
        wxString m_sDialogCaption;
        wxString m_sDialogMessage;
        wxString m_sCaCert;
        wxString m_sExportFile;
        long m_nNxSshPID;
        long m_nOtherPID;
        long m_nWindowID;
        int m_iDialogStyle;
        int m_iReader;
        enum mode m_eMode;
        bool m_bNxSmartCardSupport;
        bool m_bRunproc;
        bool m_bLibUSBAvailable;
        bool m_bRequireWatchReader;
        bool m_bRequireStartUsbIp;
        bool m_bTestCardWaiter;
        bool m_bNxProxyAvailable;
        bool m_bAutoLogin;
        bool m_bAutoResume;
        bool m_bKillErrors;
        bool m_bNoGui;
        LoginDialog *m_pLoginDialog;

        bool preInit();
        bool realInit();
        bool setSelfPath();
        void setUserDir();
        void checkNxSmartCardSupport();
        void checkLibUSB();
        void checkNxProxy();
        wxString findExecutable(wxString name);
};

DECLARE_APP(opennxApp)

#endif // opennxApp_H


