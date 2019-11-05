// $Id: watchReaderApp.cpp 663 2012-01-04 04:53:13Z felfert $
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/wfstream.h>
#include <wx/sysopt.h>
#include <wx/tokenzr.h>
#include <wx/regex.h>

#include "watchReaderApp.h"
#include "LibOpenSC.h"
#include "osdep.h"

#include "trace.h"
ENABLE_TRACE;
DECLARE_TRACETAGS;

IMPLEMENT_APP( watchReaderApp )
IMPLEMENT_CLASS( watchReaderApp, wxApp )

BEGIN_EVENT_TABLE(watchReaderApp, wxApp)
END_EVENT_TABLE()

watchReaderApp::watchReaderApp()
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

#ifndef __WXMSW__
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
#endif
}


void watchReaderApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    // Init standard options (--help, --verbose);
    wxApp::OnInitCmdLine(parser);

    // tags will be appended to the last switch/option
    wxString tags;
    allTraceTags.Sort();
    size_t i;
    for (i = 0; i < allTraceTags.GetCount(); i++) {
        if (!tags.IsEmpty())
            tags += wxT(" ");
        tags += allTraceTags.Item(i);
    }
    tags.Prepend(_("\n\nSupported trace tags: "));

    parser.AddOption(wxT("r"), wxT("reader"), _("Number of smart card reader to watch for."),
            wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_OPTION_MANDATORY);
    parser.AddOption(wxT("p"), wxT("pid"), _("Process ID of the nxssh process.") + tags,
            wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_OPTION_MANDATORY);
    parser.AddOption(wxEmptyString, wxT("trace"),
            _("Specify wxWidgets trace mask."));
    // Workaround for commandline compatibility:
    // Despite of the doc (specifying space, colon and '='),
    // wxCmdLineParser insists on having a '=' as separator
    // between option and option-value. The original however
    // *requires* the separator to be a space instead.
#ifdef __WXMSW__
    wxRegEx re(wxT("^--((reader)|(pid)|(trace))$"));
#else
    // On Unix, --display is a toolkit option
    wxRegEx re(wxT("^--((reader)|(pid)|(trace))$"));
#endif
    wxArrayString as = argv.GetArguments();
    for (i = 1; i < as.GetCount(); i++) {
        if (re.Matches(as[i])) {
            if ((i + 1) < as.GetCount()) {
                as[i].Append(wxT("=")).Append(as[i+1]);
                as.RemoveAt(i+1);
            }
        }
    }
    wxChar **xargv = new wxChar* [as.GetCount()];
    for (i = 0; i < as.GetCount(); i++)
        xargv[i] = wxStrdup(as[i].wx_str());
    parser.SetCmdLine(as.GetCount(), xargv);
}

bool watchReaderApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (!wxApp::OnCmdLineParsed(parser))
        return false;
    long tmp;
    if (parser.Found(wxT("r"), &tmp))
        m_iReader = tmp;
    if (parser.Found(wxT("p"), &tmp))
        m_lSshPid = tmp;
    wxString traceTags;
    if (parser.Found(wxT("trace"), &traceTags)) {
        CheckAllTrace(traceTags);
        wxStringTokenizer t(traceTags, wxT(","));
        while (t.HasMoreTokens()) {
            wxString tag = t.GetNextToken();
            if (allTraceTags.Index(tag) == wxNOT_FOUND) {
                OnCmdLineError(parser);
                return false;
            }
            ::myLogDebug(wxT("Trace for '%s' enabled"), VMB(tag));
            wxLog::AddTraceMask(tag);
        }
    }
    return true;
}

bool watchReaderApp::OnInit()
{    
    initWxTraceTags();
    wxString tmp;
    wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &tmp);
    m_cLocale.AddCatalogLookupPathPrefix(tmp + wxFileName::GetPathSeparator()
            + wxT("share") + wxFileName::GetPathSeparator() + wxT("locale"));
    m_cLocale.AddCatalogLookupPathPrefix(wxT("locale"));
    m_cLocale.Init();
    m_cLocale.AddCatalog(wxT("opennx"));

    // Win: Don't remap bitmaps to system colors
    wxSystemOptions::SetOption(wxT("msw.remap"), 0);

#ifdef __WXMSW__
    wxString ldpath;
    if (::wxGetEnv(wxT("PATH"), &ldpath))
        ldpath += wxT(";");
    ldpath = tmp + wxT("\\bin");
    if (!::wxSetEnv(wxT("PATH"), ldpath)) {
        wxLogSysError(wxT("Can not set PATH"));
        return false;
    }
#endif

#ifdef __UNIX__
# ifdef __WXMAC__
#  define LD_LIBRARY_PATH wxT("DYLD_LIBRARY_PATH")
# else
#  define LD_LIBRARY_PATH wxT("LD_LIBRARY_PATH")
# endif

    wxString ldpath;
    if (::wxGetEnv(LD_LIBRARY_PATH, &ldpath))
        ldpath += wxT(":");
# if defined(__x86_64) || defined(__IA64__)
    ldpath += tmp + wxT("/lib64");
# else
    ldpath += tmp + wxT("/lib");
# endif
# ifdef __WXMAC__
    ldpath += wxT(":/Library/OpenSC/lib");
# endif
    if (!::wxSetEnv(LD_LIBRARY_PATH, ldpath)) {
        wxLogSysError(wxT("Can not set LD_LIBRARY_PATH"));
        return false;
    }
#endif

    if (!wxApp::OnInit())
        return false;

    if (::wxGetEnv(wxT("WXTRACE"), &tmp)) {
        CheckAllTrace(tmp);
        wxStringTokenizer t(tmp, wxT(",:"));
        while (t.HasMoreTokens()) {
            wxString tag = t.GetNextToken();
            if (allTraceTags.Index(tag) != wxNOT_FOUND) {
                ::myLogDebug(wxT("Trace for '%s' enabled"), VMB(tag));
                wxLog::AddTraceMask(tag);
            }
        }
    }

    LibOpenSC opensc;
    if (opensc.WatchHotRemove(m_iReader, m_lSshPid)) {
#ifdef __WXMAC__
        // on MacOS, we use the --dialog functionality of opennx
        wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &tmp);
        tmp << wxFileName::GetPathSeparator() << wxT("Message.app");
        myLogTrace(MYTRACETAG, wxT("Executing %s"), VMB(tmp));
        ::wxExecute(tmp);
#else
        myLogTrace(MYTRACETAG, wxT("Showing info dialog"));
        wxMessageBox(
                _("OpenNX session has been suspended, because\nthe authenticating smart card has been removed."),
                _("Smart card removed"), wxOK|wxICON_INFORMATION);
#endif
    }
    SetExitOnFrameDelete(true);
    return false;
}


int watchReaderApp::OnExit()
{
    return wxApp::OnExit();
}

