// $Id: ExtHtmlWindow.h 193 2009-03-19 09:52:42Z felfert $
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

#ifndef _EXTHTMLWINDOW_H_
#define _EXTHTMLWINDOW_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ExtHtmlWindow.cpp"
#endif

#include "wx/html/htmlwin.h"

/**
 * Custom HTML window.
 * This derivate of wxHtmlWindow interpretes links in a special
 * way:<br>
 * <ul>
 * <li>If "TV" is specified as target, a builting TextViewer is used.
 * <li>Other links are opened in an external browser.
 * </ul>
 */
class extHtmlWindow : public wxHtmlWindow
{
    DECLARE_DYNAMIC_CLASS( extHtmlWindow )

public:
    extHtmlWindow() : wxHtmlWindow() { }

    /**
     * Performs the actual action.
     * If target is TV, open url in internal TextViewer,
     * otherwise in external browser.
     */
    virtual void OnLinkClicked(const wxHtmlLinkInfo&);
};

#endif
    // _EXTHTMLWINDOW_H_
