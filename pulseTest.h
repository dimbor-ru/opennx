// $Id: pulseTest.h 582 2010-10-21 06:52:48Z felfert $
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

#ifndef _PULSETEST_H_
#define _PULSETEST_H_

#include <wx/intl.h>

class pulseTest: public wxApp
{    
    DECLARE_CLASS(pulseTest);
    DECLARE_EVENT_TABLE()

    public:
        pulseTest();
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        virtual int OnExit();

    private:
        wxLocale m_cLocale;
};

DECLARE_APP(pulseTest)

#endif
    // _PULSETEST_H_
