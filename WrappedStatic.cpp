// $Id: WrappedStatic.cpp 677 2012-02-05 19:52:01Z felfert $
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
#pragma implementation "WrappedStatic.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "WrappedStatic.h"

IMPLEMENT_DYNAMIC_CLASS(WrappedStaticText, wxStaticText)

#if defined(__WXMSW__) || defined(__WXMAC__)

// this class is used to wrap the text on word boundary: wrapping is done by
// calling OnStartLine() and OnOutputLine() functions
//
// Modified copy of wxWidgets source, since this is unfortunately not public
// in wxWidgets

class MinTextWrapper
{
public:
    MinTextWrapper() { m_eol = false; }

    // win is used for getting the font, text is the text to wrap, width is the
    // max line width or -1 to disable wrapping
    void Wrap(wxWindow *win, const wxString& text, int widthMax)
    {
        const wxChar *lastSpace = NULL;
        wxString line;

        const wxChar *lineStart = text.mb_str();
        for ( const wxChar *p = lineStart; ; p++ )
        {
            if ( IsStartOfNewLine() )
            {
                OnNewLine();

                lastSpace = NULL;
                line.clear();
                lineStart = p;
            }

            if ( *p == _T('\n') || *p == _T('\0') )
            {
                DoOutputLine(line);

                if ( *p == _T('\0') )
                    break;
            }
            else // not EOL
            {
                if ( *p == _T(' ') )
                    lastSpace = p;

                line += *p;

                if ( widthMax >= 0 && lastSpace )
                {
                    int width;
                    win->GetTextExtent(line, &width, NULL);

                    if ( width > widthMax )
                    {
                        // remove the last word from this line
                        line.erase(lastSpace - lineStart, p + 1 - lineStart);
                        DoOutputLine(line);

                        // go back to the last word of this line which we didn't
                        // output yet
                        p = lastSpace;
                    }
                }
                //else: no wrapping at all or impossible to wrap
            }
        }
    }

    // we don't need it, but just to avoid compiler warnings
    virtual ~MinTextWrapper() { }

protected:
    // line may be empty
    virtual void OnOutputLine(const wxString& line) = 0;

    // called at the start of every new line (except the very first one)
    virtual void OnNewLine() { }

private:
    // call OnOutputLine() and set m_eol to true
    void DoOutputLine(const wxString& line)
    {
        OnOutputLine(line);

        m_eol = true;
    }

    // this function is a destructive inspector: when it returns true it also
    // resets the flag to false so calling it again woulnd't return true any
    // more
    bool IsStartOfNewLine()
    {
        if ( !m_eol )
            return false;

        m_eol = false;

        return true;
    }

    bool m_eol;
};

class MinLabelWrapper : public MinTextWrapper
{
public:
    void WrapLabel(wxWindow *text, int widthMax)
    {
        m_text.clear();
        Wrap(text, text->GetLabel(), widthMax);
        text->SetLabel(m_text);
    }

protected:
    virtual void OnOutputLine(const wxString& line)
    {
        m_text += line;
    }

    virtual void OnNewLine()
    {
        m_text += _T('\n');
    }

private:
    wxString m_text;
};

void WrappedStaticText::Wrap(int width)
{
    wxUnusedVar(width);
    inWrap = true;
    MinLabelWrapper wrapper;
    wrapper.WrapLabel(this, 300);
    inWrap = false;
}

#endif
