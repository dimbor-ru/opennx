// $Id: ExtHtmlWindow.cpp 202 2009-03-24 07:26:30Z felfert $
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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "ExtHtmlWindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ExtHtmlWindow.h"
#include "TextViewer.h"

#include <wx/utils.h>

IMPLEMENT_DYNAMIC_CLASS( extHtmlWindow, wxHtmlWindow )

void extHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
    wxString href = link.GetHref();
    wxString target = link.GetTarget();
    if (target.StartsWith(wxT("TV:"))) {
        TextViewer *tw = new TextViewer(this);
        tw->LoadFile(href);
        target = target.AfterFirst(wxT(':'));
        if (!target.IsEmpty())
            tw->SetTitle(target);
        tw->ShowModal();
        return;
    }
#ifdef __WXMSW__
    ShellExecute((HWND)GetHandle(), wxT("open"), href.mb_str(), NULL, NULL, SW_SHOWNORMAL);
#else
    ::wxLaunchDefaultBrowser(href, wxBROWSER_NEW_WINDOW);
#endif
}
