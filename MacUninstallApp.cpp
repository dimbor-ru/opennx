/////////////////////////////////////////////////////////////////////////////
// Name:        macuninstallapp.cpp
// Purpose:     
// Author:      Fritz Elfert
// Modified by: 
// Created:     Fri 11 Sep 2009 01:17:42 PM CEST
// RCS-ID:      
// Copyright:   (C) 2009 by Fritz Elfert
// Licence:     
/////////////////////////////////////////////////////////////////////////////

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

#include <wx/stdpaths.h>
#include <wx/apptrait.h>
#include <wx/filename.h>
#include <wx/cmdline.h>
#include <wx/xml/xml.h>
#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fstream>

#include "macuninstallapp.h"

static unsigned long failed_files;
static unsigned long failed_dirs;

class RmRfTraverser : public wxDirTraverser
{
    public:
        RmRfTraverser(bool testMode)
            : m_bTestMode(testMode) { }

        ~RmRfTraverser()
        {
            int n = m_aFiles.GetCount() - 1;
            wxString fn;
            while (n >= 0) {
                fn = m_aFiles[n--];
                if (m_bTestMode) {
                    ::wxLogMessage(_("TEST: Would delete file: %s"), fn.c_str());
                } else {
                    if (::wxRemoveFile(fn))
                        ::wxLogMessage(_("Deleted file: %s"), fn.c_str());
                    else {
                        failed_files++;
                        ::wxLogWarning(_("Could not delete file %s"), fn.c_str());
                    }
                }
            }
            n = m_aDirs.GetCount() - 1;
            while (n >= 0) {
                fn = m_aDirs[n--];
                if (m_bTestMode) {
                    ::wxLogMessage(_("TEST: Would delete diretory %s"), fn.c_str());
                } else {
                    if (::wxRmdir(fn))
                        ::wxLogMessage(_("Deleted diretory %s"), fn.c_str());
                    else {
                        failed_dirs++;
                        ::wxLogWarning(_("Could not delete directory %s"), fn.c_str());
                    }
                }
            }
        }

        virtual wxDirTraverseResult OnFile(const wxString& filename)
        {
            m_aFiles.Add(filename);
            return wxDIR_CONTINUE;
        }

        virtual wxDirTraverseResult OnDir(const wxString& dirpath)
        {
            m_aDirs.Add(dirpath);
            return wxDIR_CONTINUE;
        }

    private:
        bool m_bTestMode;
        wxArrayString m_aDirs;
        wxArrayString m_aFiles;
};

IMPLEMENT_APP(MacUninstallApp)
IMPLEMENT_CLASS(MacUninstallApp, wxApp)

BEGIN_EVENT_TABLE(MacUninstallApp, wxApp)
END_EVENT_TABLE()

MacUninstallApp::MacUninstallApp()
{
    failed_files = failed_dirs = 0;
    m_bBatchMode = false;
    m_bTestMode = false;
    m_bCancelled = false;
    m_sSelfPath = wxFileName(
            GetTraits()->GetStandardPaths().GetExecutablePath()).GetFullPath();
    m_nodelete.insert(wxT("."));
    m_nodelete.insert(wxT("./Applications"));
    m_nodelete.insert(wxT("./Library"));
    m_nodelete.insert(wxT("./usr/share/man/man1"));
    m_nodelete.insert(wxT("./usr/share/man"));
    m_nodelete.insert(wxT("./usr/share"));
    m_nodelete.insert(wxT("./usr"));
}

void MacUninstallApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    // Init standard options (--help, --verbose);
    wxApp::OnInitCmdLine(parser);
    parser.AddSwitch(wxEmptyString, wxT("test"),
            _("Test mode. Just create log file reporting what would happen."));
    parser.AddSwitch(wxEmptyString, wxT("batch"),
            _("Uninstall without asking the user (needs admin rights)."));
}

bool MacUninstallApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    m_bBatchMode = parser.Found(wxT("batch"));
    m_bTestMode = parser.Found(wxT("test"));
    if (m_bTestMode)
        m_bBatchMode = true;
    return true;
}

/*
 * Initialisation for MacUninstallApp
 */

bool MacUninstallApp::OnInit()
{
    wxFileName fn(m_sSelfPath);
    fn.RemoveLastDir();
    fn.AppendDir(wxT("share"));
    fn.AppendDir(wxT("locale"));
    m_cLocale.AddCatalogLookupPathPrefix(fn.GetPath());
    m_cLocale.Init();
    m_cLocale.AddCatalog(wxT("opennx"));
    wxString targetPkg = wxT("OpenNX");
    wxString targetPkgId = wxT("org.opennx.OpenNX");

    // Call to base class needed for initializing command line processing
    if (!wxApp::OnInit())
        return false;

    wxInitAllImageHandlers();
    wxBitmap::InitStandardHandlers();
    m_sLogName << wxT("/tmp/uninstall-") << targetPkgId << wxT(".log");
    if (m_bBatchMode) {
        if ((!m_bTestMode) && (0 != geteuid())) {
            ::wxMessageBox(_("Batch uninstall needs to be started as root."),
                    wxString::Format(_("Uninstall %s"), targetPkg.c_str()),
                    wxOK|wxICON_ERROR);
            while (Pending())
                Dispatch();
            return false;
        }
        bool ok = DoUninstall(targetPkg, targetPkgId);
        ::wxLogMessage(_("Uninstall finished at %s"), wxDateTime::Now().Format().c_str());
        ::wxLogMessage(_("Status: %s, failed files: %lu, failed dirs: %lu"),
            (ok ? _("OK") : _("FAILED")), failed_files, failed_dirs);
        // Print result to stdout for parent (elevation wrapper)
        wxString ptmp;
        if (::wxGetEnv(wxT("MACUNINST_ELEVATION_PID"), &ptmp)) {
            long epid;
            if (ptmp.ToLong(&epid)) {
                if (getppid() == epid) {
                    ::wxLogMessage(_("Reporting result to elevation wrapper %s"), ptmp.c_str());
                    printf("%d %lu %lu\n", ok ? 0 : 1, failed_files, failed_dirs);
                }
            }
        }
    } else {
        int r = ::wxMessageBox(
                wxString::Format(
                    _("This operation can not be undone!\nDo you really want to uninstall %s?"),
                    targetPkg.c_str()),
                wxString::Format(_("Uninstall %s"), targetPkg.c_str()),
                wxYES_NO|wxNO_DEFAULT|wxICON_EXCLAMATION);
        if (wxYES == r) {
            if (!TestReceipt(targetPkg, targetPkgId)) {
                while (Pending())
                    Dispatch();
                return false;
            }
            if (ElevatedUninstall(targetPkg, targetPkgId)) {
                if (!m_bCancelled) {
                    if (0 == (failed_files + failed_dirs)) {
                        ::wxMessageBox(
                                wxString::Format(_("%s has been removed successfully."),
                                    targetPkg.c_str()),
                                _("Uninstallation complete"), wxOK|wxICON_INFORMATION);
                    } else {
                        ::wxMessageBox(
                                wxString::Format(
                                    _("%s could not be removed completely.\nSome files or directories could not be deleted.\nPlease investigate the log file\n%s\n for more information."),
                                    targetPkg.c_str(), m_sLogName.c_str()),
                                _("Uninstallation incomplete"), wxOK|wxICON_EXCLAMATION);
                    }
                }
            } else
                ::wxMessageBox(
                        wxString::Format(
                            _("Uninstallation has failed.\nThe reason should have been logged in the file\n%s"),
                            m_sLogName.c_str()),
                        _("Uninstallation failed"), wxOK|wxICON_ERROR);
        }
    }
    while (Pending())
        Dispatch();
    return false;
}

int MacUninstallApp::OnExit()
{
    return wxApp::OnExit();
}

wxVariant MacUninstallApp::GetPlistValue(
        const wxXmlDocument &doc, const wxString &docname, const wxString &keyname)
{
    wxVariant ret;
    ret.Clear();
    if (doc.GetRoot()->GetName() != wxT("plist")) {
        ::wxLogError(_("Not an XML plist: %s"), docname.c_str());
        return ret;
    }
    wxXmlNode *child = doc.GetRoot()->GetChildren();
    if (child->GetName() != wxT("dict")) {
        ::wxLogError(
                _("Invalid plist (missing toplevel <dict> in %s"),
                docname.c_str());
        return ret;
    }
    child = child->GetChildren();
    bool needkey = true;
    bool found = false;
    while (child) {
        if (needkey) {
            if (child->GetName() != wxT("key")) {
                ::wxLogError(
                        _("Invalid plist (expected a key) in %s"),
                        docname.c_str());
                return ret;
            }
            if (child->GetNodeContent().IsSameAs(keyname))
                found = true;
        } else {
            if (found) {
                if (child->GetName().IsSameAs(wxT("array"))) {
                    ::wxLogError(
                            _("Unsupported type array in %s"), docname.c_str());
                    return ret;
                }
                if (child->GetName().IsSameAs(wxT("real"))) {
                    double val;
                    if (!child->GetNodeContent().ToDouble(&val)) {
                        ::wxLogError(
                                _("Invalid plist (invalid real value %s) in %s"),
                                child->GetNodeContent().c_str(), docname.c_str());
                    } else
                        ret = val;
                    return ret;
                }
                if (child->GetName().IsSameAs(wxT("integer"))) {
                    long val;
                    if (!child->GetNodeContent().ToLong(&val)) {
                        ::wxLogError(
                                _("Invalid plist (invalid integer value %s) in %s"),
                                child->GetNodeContent().c_str(), docname.c_str());
                    } else
                        ret = val;
                    return ret;
                }
                if (child->GetName().IsSameAs(wxT("string"))) {
                    ret = child->GetNodeContent();
                    return ret;
                }
                if (child->GetName().IsSameAs(wxT("true")) ||
                        child->GetName().IsSameAs(wxT("false"))) {
                    ret = child->GetName().IsSameAs(wxT("true"));
                    return ret;
                }
                if (child->GetName().IsSameAs(wxT("date"))) {
                    wxDateTime dt;
                    if (NULL == dt.ParseDateTime(child->GetNodeContent())) {
                        ::wxLogError(
                                _("Invalid plist (invalid date value %s) in %s"),
                                child->GetNodeContent().c_str(), docname.c_str());
                    } else
                        ret = dt;
                    return ret;
                }
                ::wxLogError(
                        _("Invalid plist (unexpected type %s) in %s"),
                        child->GetName().c_str(), docname.c_str());
                return ret;
            }
        }
        needkey = (!needkey);
        child = child->GetNext();
    }
    return ret;
}

wxString MacUninstallApp::GetInstalledPath(const wxString &rcpt)
{
    wxXmlDocument doc;

    // Test for binary plist and convert it to xml if necessary.
    wxFileInputStream fis(rcpt);
    if (fis.IsOk()) {
        char buf[8];
        memset(buf, 0, 8);
        fis.Read(&buf, 7);
        if (0 == memcmp(buf, "bplist0", 7)) {
            wxString cmd(wxT("plutil -convert xml1 -o - "));
            cmd << rcpt;
            wxArrayString lines;
            if (0 == ::wxExecute(cmd, lines)) {
                size_t i;
                wxString fbuf;
                for (i = 0; i < lines.GetCount(); ++i) {
                    // ::wxLogMessage(_("RLINE: %s"), lines[i].c_str());
                    fbuf.Append(lines[i]).Append(wxT("\n"));
                }
                wxMemoryInputStream mis(fbuf.mb_str(), fbuf.Length());
                doc.Load(mis);
            }
        } else {
            doc.Load(rcpt);
        }
    }

    if (doc.IsOk()) {
        wxVariant v = GetPlistValue(doc, rcpt, wxT("InstallPrefixPath"));
        if (v.IsNull()) {
            // Old variant
            v = GetPlistValue(doc, rcpt, wxT("IFPkgFlagRelocatable"));
            if (v.IsNull()) {
                ::wxLogError(_("Could not find requested key 'IFPkgFlagRelocatable' in receipt %s"),
                        rcpt.c_str());
                return wxEmptyString;
            }
            wxString t = v.GetType();
            if (t != wxT("bool")) {
                ::wxLogError(_("Unexpected type '%s' of 'IFPkgFlagRelocatable' in receipt %s"),
                        t.c_str(), rcpt.c_str());
                return wxEmptyString;
            }
            wxString pkey = (v.GetBool() ?
                    wxT("IFPkgRelocatedPath") : wxT("IFPkgFlagDefaultLocation"));
            v = GetPlistValue(doc, rcpt, pkey);
            if (v.IsNull()) {
                ::wxLogError(_("Could not find requested key '%s' in receipt %s"),
                        pkey.c_str(), rcpt.c_str());
                return wxEmptyString;
            }
            t = v.GetType();
            if (t != wxT("string")) {
                ::wxLogError(_("Unexpected type '%s' of '%s' in receipt %s"),
                        t.c_str(), pkey.c_str(), rcpt.c_str());
                return wxEmptyString;
            }
            return v.GetString();
        } else {
            // New variant
            return v.GetString().Append(wxT("/"));
        }
    } else
        ::wxLogError(_("Could not read package receipt %s"), rcpt.c_str());
    return wxEmptyString;
}

bool MacUninstallApp::FetchBOM(const wxString &bom,
        wxArrayString &dirs, wxArrayString &files)
{
    if (!wxFileName::FileExists(bom)) {
        ::wxLogError(
                _("Missing BOM (Bill Of Materials) '%s'. Already unistalled?"), bom.c_str());
        return false;
    }
    wxString cmd(wxT("lsbom -fbcl -p f "));
    cmd << bom;
    wxArrayString err;
    if (0 != ::wxExecute(cmd, files, err)) {
        ::wxLogError(
                _("Could not list BOM (Bill Of Materials) '%s'. Already unistalled?"), bom.c_str());
        return false;
    }
    if (0 != err.GetCount() != 0) {
        ::wxLogError(
                _("Invalid BOM (Bill Of Materials) '%s'. Already unistalled?"), bom.c_str());
        return false;
    }
    cmd = wxT("lsbom -d -p f ");
    cmd << bom;
    err.Empty();
    if (0 != ::wxExecute(cmd, dirs, err)) {
        ::wxLogError(
                _("Could not list BOM (Bill Of Materials) '%s'. Already unistalled?"), bom.c_str());
        return false;
    }
    if (0 != err.GetCount() != 0) {
        ::wxLogError(
                _("Invalid BOM (Bill Of Materials) '%s'. Already unistalled?"), bom.c_str());
        return false;
    }
    return true;
}

bool MacUninstallApp::TestReceipt(const wxString &pkg, const wxString &pkgid)
{
    wxString rpath = wxT("/Library/Receipts/");
    wxString proot;
    wxString bompath;
    rpath.Append(pkg).Append(wxT(".pkg"));
    if (wxFileName::DirExists(rpath)) {
        proot = GetInstalledPath(rpath + wxT("/Contents/Info.plist"));
        bompath = rpath + wxT("/Contents/Archive.bom");
    } else {
        rpath = wxT("/var/db/receipts/");
        bompath = rpath;
        rpath.Append(pkgid).Append(wxT(".plist"));
        if (wxFileName::FileExists(rpath)) {
            proot = GetInstalledPath(rpath);
            bompath.Append(pkgid).Append(wxT(".bom"));
        } else {
            ::wxLogWarning(
                    _("The package receipt does not exist. Already unistalled?"));
            return false;
        }
    }
    if (proot.IsEmpty())
        return false;
    if (!wxFileName::DirExists(proot)) {
        ::wxLogWarning(
                _("The package install path does not exist. Already unistalled?"));
        return false;
    }
    wxArrayString d;
    wxArrayString f;
    if (!FetchBOM(bompath, d, f))
        return false;
    return true;
}

bool MacUninstallApp::DoUninstall(const wxString &pkg, const wxString &pkgid)
{
    std::ofstream *log = new std::ofstream();
    log->open(m_sLogName.mb_str());
    delete wxLog::SetActiveTarget(new wxLogStream(log));
    ::wxLogMessage(_("Uninstall started at %s"), wxDateTime::Now().Format().c_str());
    wxString rpath = wxT("/Library/Receipts/");
    wxString proot;
    wxString bompath;
    bool oldreceipt = true;
    rpath.Append(pkg).Append(wxT(".pkg"));
    if (wxFileName::DirExists(rpath)) {
        proot = GetInstalledPath(rpath + wxT("/Contents/Info.plist"));
        bompath = rpath + wxT("/Contents/Archive.bom");
    } else {
        rpath = wxT("/var/db/receipts/");
        bompath = rpath;
        rpath.Append(pkgid).Append(wxT(".plist"));
        proot = GetInstalledPath(rpath);
        bompath.Append(pkgid).Append(wxT(".bom"));
        oldreceipt = false;
    }
    if (proot.IsEmpty())
        return false;
    wxArrayString d;
    wxArrayString f;
    if (!FetchBOM(bompath, d, f))
        return false;
    size_t i;
    ::wxLogMessage(_("Deleting package content"));
    for (i = 0; i < f.GetCount(); i++) {
        if (m_nodelete.find(f[i]) != m_nodelete.end()) {
            f.RemoveAt(i--);
            continue;
        }
        wxFileName fn(proot + f[i]);
        if (fn.Normalize(wxPATH_NORM_DOTS|wxPATH_NORM_ABSOLUTE)) {
            wxString name = fn.GetFullPath();
            if (m_bTestMode) {
                f.RemoveAt(i--);
                ::wxLogMessage(_("TEST: Would delete file: %s"), name.c_str());
            } else {
                if (::wxRemoveFile(name) || (!fn.FileExists())) {
                    f.RemoveAt(i--);
                    ::wxLogMessage(_("Deleted file: %s"), name.c_str());
                } else {
                    failed_files++;
                    ::wxLogWarning(_("Could not delete file %s"), name.c_str());
                }
            }
        }
    }
    size_t lcd;
    do {
        lcd = d.GetCount();
        for (i = 0; i < d.GetCount(); i++) {
            if (m_nodelete.find(d[i]) != m_nodelete.end()) {
                d.RemoveAt(i--);
                continue;
            }
            wxFileName fn(proot + d[i]);
            if (fn.Normalize(wxPATH_NORM_DOTS|wxPATH_NORM_ABSOLUTE)) {
                wxString name = fn.GetFullPath();
                if (m_bTestMode) {
                    d.RemoveAt(i--);
                    ::wxLogMessage(_("TEST: Would delete directory: %s"), name.c_str());
                } else {
                    if (::wxRmdir(name) || (!fn.DirExists())) {
                        d.RemoveAt(i--);
                        ::wxLogMessage(_("Deleted directory: %s"), name.c_str());
                    }
                }
            }
        }
    } while (lcd != d.GetCount());
    if (0 < d.GetCount()) {
        for (i = 0; i < d.GetCount(); i++) {
            failed_dirs++;
            ::wxLogWarning(_("Could not delete directory %s"), d[i].c_str());
        }
    }
    if (0 == (d.GetCount() + f.GetCount())) {
        // Finally delete the receipe itself
        ::wxLogMessage(_("Deleting receipt"));
        if (oldreceipt) {
            {
                wxDir d(rpath);
                RmRfTraverser t(m_bTestMode);
                d.Traverse(t);
            }
            if (m_bTestMode) {
                ::wxLogMessage(_("TEST: Would delete directory: %s"), rpath.c_str());
            } else {
                if (::wxRmdir(rpath))
                    ::wxLogMessage(_("Deleted directory: %s"), rpath.c_str());
                else {
                    failed_dirs++;
                    ::wxLogWarning(_("Could not delete directory %s"), rpath.c_str());
                }
            }
        } else {
            if (m_bTestMode) {
                ::wxLogMessage(_("TEST: Would delete receipt: %s"), rpath.c_str());
                ::wxLogMessage(_("TEST: Would delete BOM: %s"), bompath.c_str());
            } else {
                if (::wxRemoveFile(rpath)) {
                    ::wxLogMessage(_("Deleted receipt: %s"), rpath.c_str());
                } else {
                    ::wxLogWarning(_("Could not delete receipt %s"), rpath.c_str());
                }
                if (::wxRemoveFile(bompath)) {
                    ::wxLogMessage(_("Deleted BOM: %s"), bompath.c_str());
                } else {
                    ::wxLogWarning(_("Could not delete BOM %s"), bompath.c_str());
                }
            }
        }
    } else
        ::wxLogMessage(_("Receipt NOT deleted, because package files have been left."));
    return true;
}

/**
 * Execute with administrative rights.
 */
bool MacUninstallApp::ElevatedUninstall(const wxString &pkg, const wxString &pkgid)
{
    // If we are already root, do the uninstall directly
    if (geteuid() == 0)
        return DoUninstall(pkg, pkgid);

    wxString msg = wxString::Format(
            _("In order to uninstall %s, administrative rights are required.\n\n"),
            pkg.c_str());
    char *prompt = strdup(msg.utf8_str());

    OSStatus st;
    AuthorizationFlags aFlags = kAuthorizationFlagDefaults;
    AuthorizationRef aRef;
    AuthorizationItem promptItem = {
        kAuthorizationEnvironmentPrompt, strlen(prompt), prompt, 0
    };
    AuthorizationEnvironment aEnv = { 1, &promptItem };

    st = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment,
            kAuthorizationFlagDefaults, &aRef);
    if (errAuthorizationSuccess != st) {
        ::wxLogError(_("Authorization could not be created: %s"), MacAuthError(st).c_str());
        return true;
    }
    AuthorizationItem aItems = { kAuthorizationRightExecute, 0, NULL, 0 };
    AuthorizationRights aRights = { 1, &aItems };
    aFlags = kAuthorizationFlagDefaults |
        kAuthorizationFlagInteractionAllowed |
        kAuthorizationFlagPreAuthorize |
        kAuthorizationFlagExtendRights;
    st = AuthorizationCopyRights(aRef, &aRights, &aEnv, aFlags, NULL );
    bool ret = true;
    if (errAuthorizationSuccess == st) {
        char *executable = strdup(m_sSelfPath.utf8_str());
        char *args[] = { "--batch", NULL };
        FILE *pout = NULL;

        if (!::wxGetEnv(wxT("TMPDIR"), NULL))
            ::wxSetEnv(wxT("TMPDIR"), wxFileName::GetTempDir());
        ::wxSetEnv(wxT("MACUNINST_ELEVATION_PID"), wxString::Format(wxT("%d"), ::wxGetProcessId()));
        st = AuthorizationExecuteWithPrivileges(aRef,
                executable, kAuthorizationFlagDefaults, args, &pout);
        if (errAuthorizationSuccess == st) {
            int status;
            fscanf(pout, "%d %lu %lu", &status, &failed_files, &failed_dirs);
            ret = (0 == status);
        } else
            ::wxLogError(_("Could not execute with administrative rights:\n%s"), MacAuthError(st).c_str());
    } else {
        if (st) {
            m_bCancelled = (errAuthorizationCanceled == st);
            if (!m_bCancelled)
                ::wxLogError(_("Authorization failed: %s"), MacAuthError(st).c_str());
        }
    }
    AuthorizationFree(aRef, kAuthorizationFlagDefaults);
    return ret;
}

wxString MacUninstallApp::MacAuthError(long code)
{
    wxString ret;
    switch (code) {
        case errAuthorizationSuccess:
            return wxT("The operation completed successfully.");
        case errAuthorizationInvalidSet:
            return wxT("The set parameter is invalid.");
        case errAuthorizationInvalidRef:
            return wxT("The authorization parameter is invalid.");
        case errAuthorizationInvalidTag:
            return wxT("The tag parameter is invalid.");
        case errAuthorizationInvalidPointer:
            return wxT("The authorizedRights parameter is invalid.");
        case errAuthorizationDenied:
            return wxT("The Security Server denied authorization for one or more requested rights.");
        case errAuthorizationCanceled:
            return wxT("The user canceled the operation.");
        case errAuthorizationInteractionNotAllowed:
            return wxT("The Security Server denied authorization because no user interaction is allowed.");
        case errAuthorizationInternal:
            return wxT("An unrecognized internal error occurred.");
        case errAuthorizationExternalizeNotAllowed:
            return wxT("The Security Server denied externalization of the authorization reference.");
        case errAuthorizationInternalizeNotAllowed:
            return wxT("The Security Server denied internalization of the authorization reference.");
        case errAuthorizationInvalidFlags:
            return wxT("The flags parameter is invalid.");
        case errAuthorizationToolExecuteFailure:
            return wxT("The tool failed to execute.");
        case errAuthorizationToolEnvironmentError:
            return wxT("The attempt to execute the tool failed to return a success or an error code.");
    }
    return ret;
}
