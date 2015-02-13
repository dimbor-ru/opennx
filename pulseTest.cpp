// $Id: pulseTest.cpp 654 2011-12-09 18:07:38Z felfert $
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

#include <wx/tokenzr.h>
#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/wfstream.h>
#include <wx/sysopt.h>
#include <wx/utils.h>

#include "pulseTest.h"
#include "PulseAudio.h"

#include "trace.h"
ENABLE_TRACE;
DECLARE_TRACETAGS;

IMPLEMENT_APP( pulseTest )
IMPLEMENT_CLASS( pulseTest, wxApp )

BEGIN_EVENT_TABLE(pulseTest, wxApp)
END_EVENT_TABLE()

pulseTest::pulseTest()
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
}

void pulseTest::OnInitCmdLine(wxCmdLineParser& parser)
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

    parser.AddOption(wxEmptyString, wxT("trace"),
            _("Specify wxWidgets trace mask.") + tags);
}

bool pulseTest::OnCmdLineParsed(wxCmdLineParser& parser)
{
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
            ::myLogDebug(wxT("Trace for '%s' enabled"), tag.c_str());
            wxLog::AddTraceMask(tag);
        }
    }
    return true;
}

bool pulseTest::OnInit()
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
        ::wxLogSysError(wxT("Can not set PATH"));
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
        ::wxLogSysError(wxT("Can not set LD_LIBRARY_PATH"));
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
                ::myLogDebug(wxT("Trace for '%s' enabled"), tag.c_str());
                wxLog::AddTraceMask(tag);
            }
        }
    }

    PulseAudio pa;
    if (pa.IsAvailable()) {
        pa.ActivateEsound(12345);
    }
    SetExitOnFrameDelete(true);
    return false;
}


int pulseTest::OnExit()
{
    return wxApp::OnExit();
}

