// $Id: watchReaderApp.h 172 2009-02-20 05:44:20Z felfert $
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

#ifndef _WATCHREADERAPP_H_
#define _WATCHREADERAPP_H_

#include <wx/intl.h>

class watchReaderApp: public wxApp
{    
    DECLARE_CLASS(watchReaderApp);
    DECLARE_EVENT_TABLE()

    public:
        watchReaderApp();
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        virtual int OnExit();

    private:
        wxLocale m_cLocale;
        int m_iReader;
        long m_lSshPid;
};

DECLARE_APP(watchReaderApp)

#endif
    // _WATCHREADERAPP_H_
