// $Id: MyDynlib.cpp 682 2012-02-09 23:34:48Z felfert $
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "MyDynlib.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "MyDynlib.h"

#include <wx/dynlib.h>
#include <wx/log.h>
#include <wx/utils.h>
#include <wx/tokenzr.h>

#include "trace.h"
ENABLE_TRACE;

#ifdef __WXMAC__
# define LD_LIBRARY_PATH wxT("DYLD_LIBRARY_PATH")
#else
# define LD_LIBRARY_PATH wxT("LD_LIBRARY_PATH")
#endif

bool MyDynamicLibrary::Load(const wxString& name, int flags /* = wxDL_DEFAULT */)
{
    ::myLogTrace(MYTRACETAG, wxT("Load(%s, 0x%0x)"), name.c_str(), flags);
#ifdef __WXMSW__
    return wxDynamicLibrary::Load(name, flags);
#else
    wxString ldpath;
    if (::wxGetEnv(LD_LIBRARY_PATH, &ldpath)) {
        wxStringTokenizer t(ldpath, wxT(":"));
        while (t.HasMoreTokens()) {
            wxString abslib = t.GetNextToken() + wxFileName::GetPathSeparator() + name;
            ::myLogTrace(MYTRACETAG, wxT("Trying to load(%s 0x%0x)"), abslib.c_str(), flags);
            if (wxDynamicLibrary::Load(abslib, flags))
                return true;
#ifdef __WXMAC__
            if (!abslib.EndsWith(wxT(".dylib"))) {
                abslib += wxT(".dylib");
                ::myLogTrace(MYTRACETAG, wxT("Trying to load(%s, 0x%0x)"), abslib.c_str(), flags|wxDL_VERBATIM);
                if (wxDynamicLibrary::Load(abslib, flags|wxDL_VERBATIM))
                    return true;
            }
#endif
        }
    }
    ::myLogTrace(MYTRACETAG, wxT("Falling back to default load"));
    if (wxDynamicLibrary::Load(name, flags))
        return true;
#ifdef __WXMAC__
    if (!name.EndsWith(wxT(".dylib"))) {
        wxString dlib(name);
        dlib.Append(wxT(".dylib"));
        ::myLogTrace(MYTRACETAG, wxT("Trying to load(%s, 0x%0x)"), dlib.c_str(), flags|wxDL_VERBATIM);
        if (wxDynamicLibrary::Load(dlib, flags|wxDL_VERBATIM))
            return true;
    }
#endif
    ::myLogTrace(MYTRACETAG, wxT("Failed to load %s"), name.c_str());
    return false;
#endif
}
