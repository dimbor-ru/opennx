// $Id: watchUsbIpApp.cpp 605 2011-02-22 04:00:58Z felfert $
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/cmdline.h>
#include <wx/xrc/xmlres.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/fs_zip.h>
#include <wx/fs_mem.h>
#include <wx/wfstream.h>
#include <wx/mimetype.h>
#include <wx/sysopt.h>
#include <wx/socket.h>
#include <wx/tokenzr.h>
#include <wx/process.h>

#include "watchUsbIpApp.h"
#include "Icon.h"
#include "xh_richtext.h"
#include "MyXmlConfig.h"
#include "UsbFilterDetailsDialog.h"
#include "UsbIp.h"
#include "LibUSB.h"
#include "osdep.h"

#include "memres.h"

#include "trace.h"
ENABLE_TRACE;
DECLARE_TRACETAGS;

DECLARE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_DIED, -1);
DEFINE_LOCAL_EVENT_TYPE(wxEVT_PROCESS_DIED);

IMPLEMENT_APP( watchUsbIpApp )
IMPLEMENT_CLASS( watchUsbIpApp, wxApp )

BEGIN_EVENT_TABLE(watchUsbIpApp, wxApp)
    EVT_HOTPLUG(watchUsbIpApp::OnHotplug)
    EVT_COMMAND(wxID_ANY, wxEVT_PROCESS_DIED, watchUsbIpApp::OnSshDied)
END_EVENT_TABLE()

#ifdef __UNIX__
# include <signal.h>
static void terminate(int sig __attribute((unused)))
{
    ::wxGetApp().Terminate();
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);
}
#endif

class ProcessWatcher : public wxThreadHelper
{
    public:
        ProcessWatcher(wxEvtHandler *handler, long pid)
            :wxThreadHelper()
            ,m_pEvtHandler(handler)
            ,m_bOk(false)
            ,m_bTerminate(false)
            ,m_lPid(pid)
            {
                if (Create(
#ifdef __OPENBSD__
                            32768
#endif
                          ) == wxTHREAD_NO_ERROR) {
                    GetThread()->Run();
                    while ((!m_bOk) && GetThread()->IsRunning())
                        wxThread::Sleep(100);
                    if (!m_bOk)
                        myLogTrace(MYTRACETAG, wxT("ssh watch thread terminated unexpectedly"));
                } else
                    myLogTrace(MYTRACETAG, wxT("could not create ssh watch thread"));
            }

        virtual ~ProcessWatcher()
        {
            m_pEvtHandler = NULL;
            if (m_bOk) {
                m_bTerminate = true;
                GetThread()->Delete();
                while (m_bOk)
                    wxThread::Sleep(100);
            }
        }

        virtual wxThread::ExitCode Entry()
        {
            m_bOk = true;
            while (!m_thread->TestDestroy()) {
                if (m_bTerminate)
                    break;
                if (!wxProcess::Exists(m_lPid)) {
                    wxCommandEvent ev(wxEVT_PROCESS_DIED, wxID_ANY);
                    ev.SetInt(m_lPid);
                    if (m_pEvtHandler) {
                        m_bTerminate = true;
                        m_pEvtHandler->AddPendingEvent(ev);
                    }
                } else
                    wxThread::Sleep(1000);
            }
            m_bOk = false;
            return 0;
        }

        bool IsOk() { return m_bOk; }
        void SetHandler(wxEvtHandler *handler) { m_pEvtHandler = handler; }

    private:
        wxEvtHandler *m_pEvtHandler;
        bool m_bOk;
        bool m_bTerminate;
        long m_lPid;
};

    watchUsbIpApp::watchUsbIpApp()
    : m_pSessionCfg(NULL)
    , m_pUsbIp(NULL)
      , m_pDialog(NULL)
{
    SetAppName(wxT("OpenNX"));
    wxConfig *cfg;
#ifdef __WXMSW__
    cfg = new wxConfig(wxT("OpenNX"), wxT("InnoviData"));
#else
# ifdef __WXMAC__
    cfg = new wxConfig(wxT("OpenNX"), wxT("InnoviData"), wxT("OpenNX Preferences"), wxT("OpenNX Preferences"));
# else
    cfg = new wxConfig(wxT("OpenNX"), wxT("InnoviData"), wxT(".opennx"), wxT("opennx.conf"));
# endif 
#endif
    wxConfigBase::Set(cfg);

    // Language overrides from KDE - only applied if running inside a KDE session. 
    if (inKdeSession != 0) {
        wxLogNull dummy;

        // If KDE_LANG is set, then it has precedence over kdeglobals.
        wxString lang;
        if (::wxGetEnv(wxT("KDE_LANG"), &lang)) {
            myLogDebug(wxT("Overriding LANG from KDE_LANG environment to: '%s'"), VMB(lang));
            ::wxSetEnv(wxT("LANG"), lang);
        } else {
            // Try to get KDE language settings and override locale accordingly
            wxFileInputStream fis(::wxGetHomeDir() +
                    wxFileName::GetPathSeparator() + wxT(".kde") + 
                    wxFileName::GetPathSeparator() + wxT("share") + 
                    wxFileName::GetPathSeparator() + wxT("config") + 
                    wxFileName::GetPathSeparator() + wxT("kdeglobals"));
            if (fis.IsOk()) {
                wxFileConfig cfg(fis);
                wxString country = cfg.Read(wxT("Locale/Country"), wxEmptyString);
                wxString lang = cfg.Read(wxT("Locale/Language"), wxEmptyString);
                if ((!lang.IsEmpty()) && (!country.IsEmpty())) {
                    if (lang.Contains(wxT(":")))
                        lang = lang.BeforeFirst(wxT(':'));
                    if (lang.Length() < 3)
                        lang << wxT("_") << country.Upper();
                    lang << wxT(".UTF-8");
                    myLogDebug(wxT("Overriding LANG from kdeglobals to: '%s'"), VMB(lang));
                    ::wxSetEnv(wxT("LANG"), lang);
                }
            }
        }
    }
}

void watchUsbIpApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    // Init standard options (--help, --verbose);
    wxApp::OnInitCmdLine(parser);

    // tags will be appended to the last switch/option
    wxString tags;
    allTraceTags.Sort();
    for (size_t i = 0; i < allTraceTags.GetCount(); i++) {
        if (!tags.IsEmpty())
            tags += wxT(" ");
        tags += allTraceTags.Item(i);
    }
    tags.Prepend(_("\n\nSupported trace tags: "));

    parser.AddOption(wxT("s"), wxT("sessionid"), _("NX SessionID to watch for."),
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);
    parser.AddOption(wxT("c"), wxT("config"), _("Path of session config file."),
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);
    parser.AddOption(wxT("p"), wxT("pid"), _("Process ID of the nxssh process."),
            wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_OPTION_MANDATORY);
    parser.AddOption(wxEmptyString, wxT("trace"),
            _("Specify wxWidgets trace mask.") + tags);
}

bool watchUsbIpApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (!wxApp::OnCmdLineParsed(parser))
        return false;
    parser.Found(wxT("s"), &m_sSessionID);
    if (parser.Found(wxT("c"), &m_sSessionConfig)) {
        if (!m_sSessionConfig.IsEmpty())
            m_pSessionCfg = new MyXmlConfig(m_sSessionConfig);
    }
    parser.Found(wxT("p"), &m_lSshPid);

    wxString traceTags;
    if (parser.Found(wxT("trace"), &traceTags)) {
        wxStringTokenizer t(traceTags, wxT(","));
        while (t.HasMoreTokens()) {
            wxString tag = t.GetNextToken();
            if (allTraceTags.Index(tag) == wxNOT_FOUND) {
                OnCmdLineError(parser);
                return false;
            }
            myLogDebug(wxT("Trace for '%s' enabled"), VMB(tag));
            wxLog::AddTraceMask(tag);
        }
    }
    return true;
}

bool watchUsbIpApp::OnInit()
{    
    wxString tmp;

    initWxTraceTags();
    if (::wxGetEnv(wxT("WXTRACE"), &tmp)) {
        wxStringTokenizer t(tmp, wxT(",:"));
        while (t.HasMoreTokens()) {
            wxString tag = t.GetNextToken();
            if (allTraceTags.Index(tag) != wxNOT_FOUND) {
                myLogDebug(wxT("Trace for '%s' enabled"), VMB(tag));
                wxLog::AddTraceMask(tag);
            }
        }
    }

    wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &tmp);
    m_cLocale.AddCatalogLookupPathPrefix(tmp + wxFileName::GetPathSeparator()
            + wxT("share") + wxFileName::GetPathSeparator() + wxT("locale"));
    m_cLocale.AddCatalogLookupPathPrefix(wxT("locale"));
    m_cLocale.Init();
    m_cLocale.AddCatalog(wxT("opennx"));

    // Win: Don't remap bitmaps to system colors
    wxSystemOptions::SetOption(wxT("msw.remap"), 0);
    // WinXP: Don't draw themed gradients on notebook pages
    wxSystemOptions::SetOption(wxT("msw.notebook.themed-background"), 0);

    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
    wxInitAllImageHandlers();
    wxBitmap::InitStandardHandlers();
    wxXmlResource::Get()->InitAllHandlers();
    wxXmlResource::Get()->AddHandler(new wxRichTextCtrlXmlHandler());

    // This enable socket-I/O from other threads.
    wxSocketBase::Initialize();

    bool resok = false;
    wxString optionalRsc = tmp + wxFileName::GetPathSeparator() + wxT("share")
        + wxFileName::GetPathSeparator() + wxT("opennx.rsc");
    if (wxFileName::FileExists(optionalRsc)) {
        wxFile rf(optionalRsc);
        if (rf.IsOpened()) {
            unsigned char *resptr = (unsigned char *)malloc(rf.Length());
            if (resptr) {
                if (rf.Read(resptr, rf.Length()) == rf.Length()) {
                    wxMemoryFSHandler::AddFileWithMimeType(wxT("memrsc"), resptr, rf.Length(), wxT("application/zip"));
                    {
                        // The following code eliminates a stupid error dialog which shows up
                        // if some .desktop entires (in KDE or GNOME applink dirs) are dangling symlinks.
                        wxLogNull lognull;
                        wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("zip"));
                    }
                    resok = true;
                }
                free(resptr);
            }
        }
    }
    if (!resok) {
        const unsigned char *resptr = get_mem_res();
        if (resptr) {
            wxMemoryFSHandler::AddFileWithMimeType(wxT("memrsc"), resptr, cnt_mem_res, wxT("application/zip"));
            {
                // The following code eliminates a stupid error dialog which shows up
                // if some .desktop entires (in KDE or GNOME applink dirs) are dangling symlinks.
                wxLogNull lognull;
                wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("zip"));
            }
            free_mem_res(resptr);
        }
        resok = true;
    }
    if (!resok) {
        wxLogFatalError(wxT("Could not load application resource."));
        return false;
    }

    m_sResourcePrefix = wxT("memory:memrsc#zip:");
    if (!wxXmlResource::Get()->Load(m_sResourcePrefix + wxT("res/opennx.xrc")))
        return false;

    if (!wxApp::OnInit())
        return false;

    if (m_sSessionID.IsEmpty()) {
        wxLogError(_("An empty session ID is not allowed"));
        return false;
    }

    if ((!m_pSessionCfg) || (!m_pSessionCfg->IsValid())) {
        wxLogError(_("Could not load session config file"));
        return false;
    }
    wxFileName tmpfn(m_pSessionCfg->sGetFileName());
    if (tmpfn.GetName().IsSameAs(m_sSessionID)) {
        // If the basename of the session config is the sessionID, then
        // this file is temporary and can be deleted now.
        ::wxRemoveFile(tmpfn.GetFullPath());   
    }

    if (!m_pSessionCfg->bGetEnableUSBIP()) {
        // No need to watch for hotplug events, silently exit
        return false;
    }

    UsbIp *usbip = new UsbIp();
    wxString usock = wxConfigBase::Get()->Read(wxT("Config/UsbipdSocket"),
            wxT("/var/run/usbipd2.socket"));
    if (usbip->Connect(usock)) {
        usbip->SetSession(m_sSessionID);
        if (usbip->IsConnected()) {
            m_pDialog = new UsbFilterDetailsDialog(NULL);
            m_pDialog->SetDialogMode(UsbFilterDetailsDialog::MODE_HOTPLUG);
            // SetTopWindow(m_pDialog);
            usbip->SetEventHandler(m_pDialog);
            if (!usbip->RegisterHotplug()) {
                wxLogError(_("Could not register at usbipd2! No hotplugging functionality."));
                m_pDialog->Destroy();
                return false;
            }
            m_pUsbIp = usbip;
        }
    } else
        wxLogError(_("Could not connect to usbipd2! No hotplugging functionality."));
#ifdef __UNIX__
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);
#endif
    m_pProcessWatcher = new ProcessWatcher(this, m_lSshPid);
    return true;
}

void watchUsbIpApp::OnSshDied(wxCommandEvent &event)
{
    wxUnusedVar(event);
    myLogTrace(MYTRACETAG, wxT("nxssh has terminated"));
    m_pDialog->Destroy();
}

void watchUsbIpApp::OnHotplug(HotplugEvent &event)
{
    ArrayOfUsbForwards af = m_pSessionCfg->aGetUsbForwards();
    SharedUsbDevice *sdev = NULL;
    int retry = 0;
    size_t i, j;


    bool found = false;
    bool doexport = false;
    while ((NULL == sdev) && (retry++ < 10)) {
        USB u;
        ArrayOfUSBDevices au = u.GetDevices();
        for (i = 0; i < au.GetCount(); i++) {
            if ((au[i].GetBusNum() == event.GetBusNum()) && (au[i].GetDevNum() == event.GetDevNum())) {
                for (j = 0; j < af.GetCount(); j++) {
                    if (af[j].MatchHotplug(au[i])) {
                        found = true;
                        doexport = (af[j].m_eMode == SharedUsbDevice::MODE_REMOTE);
                        break;
                    }
                }
                sdev = new SharedUsbDevice;
                sdev->m_iVendorID = au[i].GetVendorID();
                sdev->m_iProductID = au[i].GetProductID();
                sdev->m_iClass = au[i].GetDeviceClass();
                sdev->m_sVendor = au[i].GetVendor();
                sdev->m_sProduct = au[i].GetProduct();
                sdev->m_sSerial = au[i].GetSerial();
                break;
            }
        }
        if (NULL == sdev)
            wxThread::Sleep(500);
    }
    if (NULL == sdev) {
        m_pUsbIp->SendHotplugResponse(event.GetCookie());
        wxLogError(_("Got hotplug event, but device is not available in libusb"));
        return;
    }
    if (found) {
        // Found device in session config. Silently act on configuration
        myLogTrace(MYTRACETAG, wxT("Found device in session config action=%s"),
                doexport ? wxT("export") : wxT("local"));
        if (!m_pUsbIp->SendHotplugResponse(event.GetCookie()))
            wxLogError(_("Could not send hotplug response"));
    } else {
        // Device not in session config. Ask user
        m_pDialog->SetVendorID(wxString::Format(wxT("%04X"), (int)sdev->m_iVendorID));
        m_pDialog->SetProductID(wxString::Format(wxT("%04X"), (int)sdev->m_iProductID));
        m_pDialog->SetDeviceClass(wxString::Format(wxT("%02X"), (int)sdev->m_iClass));
        m_pDialog->SetVendor(sdev->m_sVendor);
        m_pDialog->SetProduct(sdev->m_sProduct);
        m_pDialog->SetSerial(sdev->m_sSerial);
        int result = m_pDialog->ShowModal();

        m_pUsbIp->SendHotplugResponse(event.GetCookie());
        // Do NOT report an error here, because user might inot have responded in time.

        if (wxID_OK == result) {
            doexport = m_pDialog->GetForwarding();
            myLogTrace(MYTRACETAG, wxT("Dialog OK, store=%d action=%s"),
                    (int)m_pDialog->GetStoreFilter(), doexport ? wxT("export") : wxT("local"));
            if (m_pDialog->GetStoreFilter()) {
                ArrayOfUsbForwards a = m_pSessionCfg->aGetUsbForwards();
                SharedUsbDevice dev;
                long tmp;
                if (m_pDialog->GetVendorID().IsEmpty())
                    dev.m_iVendorID = -1;
                else {
                    m_pDialog->GetVendorID().ToLong(&tmp, 16);
                    dev.m_iVendorID = tmp;
                }
                if (m_pDialog->GetProductID().IsEmpty())
                    dev.m_iProductID = -1;
                else {
                    m_pDialog->GetProductID().ToLong(&tmp, 16);
                    dev.m_iProductID = tmp;
                }
                if (m_pDialog->GetDeviceClass().IsEmpty())
                    dev.m_iClass = -1;
                else {
                    m_pDialog->GetDeviceClass().ToLong(&tmp, 16);
                    dev.m_iClass = tmp;
                }
                dev.m_sVendor = m_pDialog->GetVendor();
                dev.m_sProduct = m_pDialog->GetProduct();
                dev.m_sSerial = m_pDialog->GetSerial();
                dev.m_eMode = doexport ? SharedUsbDevice::MODE_REMOTE : SharedUsbDevice::MODE_LOCAL;
                found = false;
                for (size_t i = 0; i < a.GetCount(); i++) {
                    if (dev.cmpNoMode(a[i])) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    a.Add(dev);
                    m_pSessionCfg->aSetUsbForwards(a);
                    myLogTrace(MYTRACETAG, wxT("saving to %s"), VMB(m_pSessionCfg->sGetFileName()));
                    if (!m_pSessionCfg->SaveToFile())
                        wxLogError(_("Could not save session config"));
                }
            }
            myLogTrace(MYTRACETAG, wxT("action=%s"), doexport ? wxT("export") : wxT("local"));
        }
    }
    delete sdev;
    if (doexport) {
        if (!m_pUsbIp->ExportDevice(event.GetBusID()))
            wxLogError(_("Could not export USB device"));
    }
}

void watchUsbIpApp::Terminate()
{
    ::wxMutexGuiEnter();
    myLogTrace(MYTRACETAG, wxT("Terminate()"));
    wxCommandEvent ev(wxEVT_PROCESS_DIED, wxID_ANY);
    ev.SetInt(0);
    AddPendingEvent(ev);
    ::wxMutexGuiLeave();
}

int watchUsbIpApp::OnExit()
{
    if (m_pUsbIp)
        delete m_pUsbIp;
    m_pUsbIp = NULL;
    return wxApp::OnExit();
}

void
watchUsbIpApp::EnableContextHelp(wxWindow *w)
{
    if (NULL == w)
        return;
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_SHIFT, WXK_F1, wxID_CONTEXT_HELP);
    wxAcceleratorTable accel(1, entries);
    w->SetAcceleratorTable(accel);
    w->SetFocus();
}

