// $Id: mylog.cpp 451 2010-01-27 12:24:56Z felfert $
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

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "mylog.h"

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

// The following functions are replacing the regular
// debug- and trace logging functions of wxWidgets.
// Since those are available in debug builds only, we use
// our own framework here in order enable tracing even in
// releas builds.

static void logit(const wxChar *szString, time_t WXUNUSED(t))
{
    wxString str;
#ifdef __WXMSW__
    wxLog::TimeStamp(&str);
#endif
    str << szString;

#ifdef __WXMSW__
    OutputDebugStringA((const char *)str.mb_str());
#else
# ifdef HAVE_SYSLOG_H
    static bool initial = true;
    if (initial) {
        initial = false;
        openlog("opennx", LOG_CONS|LOG_PID, LOG_USER);
    }
    syslog(LOG_DEBUG, "%s", (const char *)str.mb_str());
# else
    fprintf(stderr, "%s\n", (const char *)str.mb_str());
    fflush(stderr);
# endif
#endif
}

static void myVLogDebug(const wxChar *szFormat, va_list argptr)
{
    logit(wxString::FormatV(szFormat, argptr), time(NULL));
}

void myLogDebug(const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    myVLogDebug(szFormat, argptr);
    va_end(argptr);
}

static void myVLogTrace(const wxChar *mask, const wxChar *szFormat, va_list argptr)
{
    if (wxLog::IsAllowedTraceMask(mask) ) {
        wxString msg;
        msg << _T("(") << mask << _T(") ") << wxString::FormatV(szFormat, argptr);
        logit(msg, time(NULL));
    }
}

void myLogTrace(const wxChar *mask, const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    myVLogTrace(mask, szFormat, argptr);
    va_end(argptr);
}

static void myVLogTrace(wxTraceMask mask, const wxChar *szFormat, va_list argptr)
{
    if ((wxLog::GetTraceMask() & mask) == mask)
        logit(wxString::FormatV(szFormat, argptr), time(NULL));
}

void myLogTrace(wxTraceMask mask, const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    myVLogTrace(mask, szFormat, argptr);
    va_end(argptr);
}
