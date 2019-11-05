// $Id: mylog.h 709 2012-05-12 22:06:14Z felfert $
//
// Copyright (C) 2006 The OpenNX team
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

#ifndef _MYLOG_H_
#define _MYLOG_H_

#include <wx/log.h>

// Compatibility with patched wxWidgets 2.8.12 on Ubuntu
#ifndef WX_ATTRIBUTE_PRINTF_2
# ifdef ATTRIBUTE_PRINTF_2
#  define WX_ATTRIBUTE_PRINTF_2 ATTRIBUTE_PRINTF_2
# endif
#endif

// Macro for get Variadic MultiByte param from wsString object
#if wxCHECK_VERSION(3,0,0)
#ifdef __WXMSW__
# define VMB(xxx) (static_cast<const wchar_t*>(xxx.wc_str()))
#else
# define VMB(xxx) ((const char*) (xxx.ToUTF8()))
#endif
#else
# define VMB(xxx) (xxx.c_str())
#endif
extern void myLogDebug(const wxChar *szFormat, ...);
extern void myLogTrace(const wxChar *mask, const wxChar *szFormat, ...) WX_ATTRIBUTE_PRINTF_2;
extern void myLogTrace(wxTraceMask mask, const wxChar *szFormat, ...) WX_ATTRIBUTE_PRINTF_2;

#endif // _MYLOG_H_
