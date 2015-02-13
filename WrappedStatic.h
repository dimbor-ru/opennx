// $Id: WrappedStatic.h 508 2010-03-07 13:30:36Z felfert $
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

#ifndef _WRAPPED_STATIC_H_
#define _WRAPPED_STATIC_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "WrappedStatic.cpp"
#endif

#include <wx/stattext.h>

class WrappedStaticText : public wxStaticText
{
    DECLARE_DYNAMIC_CLASS(WrappedStaticText);

    public:
        WrappedStaticText()
            : wxStaticText()
        {
#if defined(__WXMSW__) || defined(__WXMAC__)
            minwrap = -1;
            inWrap = false;
#endif
        }

        WrappedStaticText(wxWindow *parent, wxWindowID id, const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0, const wxString &name = wxStaticTextNameStr )
            : wxStaticText(parent, id, label, pos, size, style, name)
        {
#if defined(__WXMSW__) || defined(__WXMAC__)
            minwrap = -1;
            inWrap = false;
#endif
        }

#if defined(__WXMSW__) || defined(__WXMAC__)
        virtual void DoSetSize(int x, int y, int w, int h, int sizeFlags)
        {
            if ((!inWrap) && (w > minwrap)) {
                minwrap = w;
                Wrap(w);
            }
            wxStaticText::DoSetSize(x, y, w, h, sizeFlags);
        }

        void Wrap(int width);

    private:
        int minwrap;
        bool inWrap;
#endif
};

#endif
