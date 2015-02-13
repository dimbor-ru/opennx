/////////////////////////////////////////////////////////////////////////////
// Name:        macuninstallapp.h
// Purpose:     
// Author:      Fritz Elfert
// Modified by: 
// Created:     Fri 11 Sep 2009 01:17:42 PM CEST
// RCS-ID:      
// Copyright:   (C) 2009 by Fritz Elfert
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _MACUNINSTALLAPP_H_
#define _MACUNINSTALLAPP_H_

#include "wx/image.h"
#include <wx/intl.h>
#include <wx/hashset.h>

class wxArrayString;

WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, StringSet);

class MacUninstallApp: public wxApp
{    
    DECLARE_CLASS( MacUninstallApp )
        DECLARE_EVENT_TABLE()

    public:
        MacUninstallApp();

        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        virtual bool OnInit();
        virtual int OnExit();

    private:
        wxVariant GetPlistValue(const wxXmlDocument &doc, const wxString &docname, const wxString &keyname);
        bool ElevatedUninstall(const wxString &, const wxString &);
        bool DoUninstall(const wxString &, const wxString &);
        bool TestReceipt(const wxString &, const wxString &);
        bool FetchBOM(const wxString &, wxArrayString &, wxArrayString &);
        wxString GetInstalledPath(const wxString &);
        wxString MacAuthError(long);

        bool m_bBatchMode;
        bool m_bTestMode;
        bool m_bCancelled;
        wxLocale m_cLocale;
        wxString m_sSelfPath;
        wxString m_sLogName;
        StringSet m_nodelete;
};

DECLARE_APP(MacUninstallApp)

#endif
    // _MACUNINSTALLAPP_H_
