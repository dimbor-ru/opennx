// $Id: pwcrypt.h 102 2007-08-23 11:41:13Z felfert $
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

#ifndef _PWCRYPT_H_
#define _PWCRYPT_H_

#include <wx/string.h>

extern wxString encodeString(const wxString &);
extern wxString decodeString(const wxString &);
extern wxString cryptString(const wxString &);
extern wxString decryptString(const wxString &);
extern wxString md5sum(const wxString &);
extern wxString Md5OfFile(const wxString &);

#endif
// _PWCRYPT_H_
