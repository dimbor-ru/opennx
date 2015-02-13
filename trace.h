// $Id: trace.h 631 2011-06-12 19:20:21Z felfert $
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

#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vector>
#include <string>
#include <wx/filename.h>
#include "mylog.h"

extern std::vector<std::string> *_allTraceTags;

#define ENABLE_TRACE \
    static wxString MYTRACETAG(wxFileName::FileName(wxT(__FILE__)).GetName()); \
    static void __attribute__ ((constructor)) _trinit() { \
        if (!_allTraceTags) _allTraceTags = new std::vector<std::string>(); \
        _allTraceTags->push_back(std::string(__FILE__)); \
    }

#define DECLARE_TRACETAGS \
    std::vector<std::string> *_allTraceTags = NULL; \
    static wxArrayString allTraceTags; \
    static void initWxTraceTags() { \
       std::vector<std::string>::iterator i; \
       for (i = _allTraceTags->begin(); i != _allTraceTags->end(); i++) { \
           wxString tag(i->c_str(), wxConvUTF8); \
           allTraceTags.Add(wxFileName::FileName(tag).GetName()); \
       } \
    } \
    static void CheckAllTrace(wxString &params) { \
        if (params.IsSameAs(wxT("all"))) { \
            params.Clear(); \
            size_t i; \
            for (i = 0; i < allTraceTags.GetCount(); ++i) { \
                if (!params.IsEmpty()) \
                    params.Append(wxT(",")); \
                params.Append(allTraceTags[i]); \
            } \
        } \
    }

#endif
