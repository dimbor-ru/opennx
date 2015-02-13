// $Id: watchUsbIpApp.h 605 2011-02-22 04:00:58Z felfert $
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

#ifndef _WATCHUSBIPAPP_H_
#define _WATCHUSBIPAPP_H_

#include <wx/intl.h>

class MyXmlConfig;
class UsbIp;
class UsbFilterDetailsDialog;
class HotplugEvent;
class ProcessWatcher;

class watchUsbIpApp: public wxApp
{    
    DECLARE_CLASS(watchUsbIpApp);
    DECLARE_EVENT_TABLE()

    public:
        watchUsbIpApp();

        const wxString &GetResourcePrefix() { return m_sResourcePrefix; }

        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        virtual int OnExit();

        void OnHotplug(HotplugEvent &);
        void OnSshDied(wxCommandEvent &);
        void SendHotplugResponse(int, wxString);
        void Terminate();
        void EnableContextHelp(wxWindow *w);

    private:
        MyXmlConfig *m_pSessionCfg;
        wxLocale m_cLocale;
        wxString m_sResourcePrefix;
        wxString m_sSessionID;
        wxString m_sSessionConfig;
        long m_lSshPid;
        UsbIp *m_pUsbIp;
        UsbFilterDetailsDialog *m_pDialog;
        ProcessWatcher *m_pProcessWatcher;
};

DECLARE_APP(watchUsbIpApp)

#endif
    // _WATCHUSBIPAPP_H_
