// $Id: PulseAudio.h 654 2011-12-09 18:07:38Z felfert $
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

#ifndef _PULSEAUDIO_H_
#define _PULSEAUDIO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "PulseAudio.h"
#endif

#include "MyXmlConfig.h"

class pawrapper;
class MyDynamicLibrary;

class PulseAudio {
    public:
        PulseAudio();
        virtual ~PulseAudio();

        /**
         * Return true, if a pulseaudio server is running.
         */
        bool IsAvailable();

        bool ActivateEsound(int port);
        bool ActivateNative(int port, int rrate, bool mono);

        const int GetEsoundPort() { return m_iPortEsound; }
        const int GetNativePort() { return m_iPortNative; }

        int FoundModuleIDs(wxString modname, wxString s_argstpl,
                                wxArrayString &a_indexes, wxArrayString &a_args,
                                bool HardAccordance);
        bool UnloadExistingModules(wxString modname, wxString s_argstpl,
                                bool HardAccordance);

    private:
        bool AutoSpawn();

        int m_iPortEsound;
        int m_iPortNative;

        pawrapper *pa;
        MyDynamicLibrary *dll;
        bool m_bPulseAvailable;
};

#endif
// _PULSEAUDIO_H_
