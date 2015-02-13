// $Id: MyDynlib.h 222 2009-04-17 12:06:07Z felfert $
//
// Copyright (C) 2009 The OpenNX Team
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

#ifndef _MYDYNLIB_H_
#define _MYDYNLIB_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MyDynlib.h"
#endif

#include <wx/dynlib.h>

class MyDynamicLibrary : public wxDynamicLibrary
{
    public:
        MyDynamicLibrary() : wxDynamicLibrary() {}
        MyDynamicLibrary(const wxString& name, int flags = wxDL_DEFAULT) : wxDynamicLibrary() { Load(name, flags); }
        bool Load(const wxString& name, int flags = wxDL_DEFAULT);
};

#endif
// _MYDYNLIB_H_
