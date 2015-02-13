// $Id: testxauth.cpp 494 2010-02-28 05:36:15Z felfert $
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SimpleXauth.h"

class TestXauth: public wxApp
{    
    DECLARE_CLASS(TestXauth);
    DECLARE_EVENT_TABLE()

    public:
        TestXauth();
        virtual bool OnInit();
        virtual int OnExit();
};

DECLARE_APP(TestXauth)

IMPLEMENT_APP(TestXauth)
IMPLEMENT_CLASS(TestXauth, wxApp)

BEGIN_EVENT_TABLE(TestXauth, wxApp)
END_EVENT_TABLE()

TestXauth::TestXauth()
{
}

bool TestXauth::OnInit()
{    
    if (!wxApp::OnInit())
        return false;
    SetExitOnFrameDelete(true);
    SimpleXauth xa(wxT("XfooAuth"));
    xa.AddDisplay(0);
    printf("cookie=%s\n", (const char *)xa.GetCookie().mb_str());
    return false;
}


int TestXauth::OnExit()
{
    return wxApp::OnExit();
}

