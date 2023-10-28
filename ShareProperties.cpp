// $Id: ShareProperties.cpp 707 2012-03-22 13:53:19Z felfert $
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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "ShareProperties.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "MyValidator.h"
#include "MyXmlConfig.h"
#include "WinShare.h"
#include "ShareProperties.h"
#include "Icon.h"
#include "opennxApp.h"

#include <wx/bmpcbox.h>
#include <wx/cshelp.h>

////@begin XPM images
////@end XPM images

#include "trace.h"
ENABLE_TRACE;

/*!
 * ShareProperties type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ShareProperties, wxDialog )

    /*!
     * ShareProperties event table definition
     */

BEGIN_EVENT_TABLE( ShareProperties, wxDialog )

    ////@begin ShareProperties event table entries
    EVT_COMBOBOX( XRCID("ID_COMBOBOX_SHARE_LOCALNAME"), ShareProperties::OnComboboxShareLocalnameSelected )

    EVT_BUTTON( wxID_OK, ShareProperties::OnOkClick )

    ////@end ShareProperties event table entries

    EVT_MENU(wxID_CONTEXT_HELP, ShareProperties::OnContextHelp)

END_EVENT_TABLE()

    /*!
     * ShareProperties constructors
     */

    ShareProperties::ShareProperties( )
    : m_iCurrentShare(-1)
    , m_bUseSmb(false)
      , m_bUseCups(false)
{
}

    ShareProperties::ShareProperties( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
    : m_iCurrentShare(-1)
    , m_bUseSmb(false)
      , m_bUseCups(false)
{
    Create(parent, id, caption, pos, size, style);
}

void ShareProperties::SetConfig(MyXmlConfig *cfg)
{
    m_pCfg = cfg;
}

void ShareProperties::SetCurrentShare(int cs)
{
    m_iCurrentShare = cs;
}

void ShareProperties::SetUse(bool useSmb, bool useCups)
{
    m_bUseSmb = useSmb;
    m_bUseCups = useCups;
}

/*!
 * ShareProperties creator
 */

bool ShareProperties::Create( wxWindow* parent, wxWindowID WXUNUSED(id), const wxString& WXUNUSED(caption), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style) )
{
    ////@begin ShareProperties member initialisation
    m_bCupsPublic = false;
    m_bSmbPublic = false;
    m_sCupsDriver = wxT("cups driver");
    m_sSmbDiskUsername = ::wxGetUserId();
    m_sSmbPrintUsername = ::wxGetUserId();
    m_pCtrlLocalShares = NULL;
    m_pCtrlSmbPrintOptions = NULL;
    m_pCtrlSmbDriver = NULL;
    m_pCtrlSmbPrivate = NULL;
    m_pCtrlSmbPublic = NULL;
    m_pCtrlSmbPrintUsername = NULL;
    m_pCtrlSmbPrintPassword = NULL;
    m_pCtrlCupsOptions = NULL;
    m_pCtrlCupsPrivate = NULL;
    m_pCtrlCupsPublic = NULL;
    m_pCtrlCupsPrintUsername = NULL;
    m_pCtrlCupsPrintPassword = NULL;
    m_pCtrlUsbOptions = NULL;
    m_pCtrlSmbDiskOptions = NULL;
    m_pCtrlMountPoint = NULL;
    m_pCtrlUsername = NULL;
    m_pCtrlPassword = NULL;
    ////@end ShareProperties member initialisation

    ////@begin ShareProperties creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY|wxWS_EX_BLOCK_EVENTS|wxDIALOG_EX_CONTEXTHELP);
    SetParent(parent);
    CreateControls();
    SetIcon(GetIconResource(wxT("res/nx.png")));
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end ShareProperties creation
    ::wxGetApp().EnableContextHelp(this);
    return TRUE;
}

#if wxCHECK_VERSION(3,1,0)
static int cmpshares(SharedResource **a, SharedResource **b)
#else
static int cmpshares(_wxObjArrayArrayOfShares **a, _wxObjArrayArrayOfShares **b)
#endif
{
    return (*a)->name.Cmp((*b)->name);
}

/*!
 * Control creation for ShareProperties
 */

void ShareProperties::CreateControls()
{    
    ////@begin ShareProperties content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("ID_DIALOG_SHARE_ADD")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_pCtrlLocalShares = XRCCTRL(*this, "ID_COMBOBOX_SHARE_LOCALNAME", wxBitmapComboBox);
    m_pCtrlSmbPrintOptions = XRCCTRL(*this, "ID_PANEL_SMB_PRINTER", wxPanel);
    m_pCtrlSmbDriver = XRCCTRL(*this, "ID_COMBOBOX_SMBDRIVER", wxComboBox);
    m_pCtrlSmbPrivate = XRCCTRL(*this, "ID_RADIOBUTTON_SMB_PRIVATE", wxRadioButton);
    m_pCtrlSmbPublic = XRCCTRL(*this, "ID_RADIOBUTTON_SMB_PUBLIC", wxRadioButton);
    m_pCtrlSmbPrintUsername = XRCCTRL(*this, "ID_TEXTCTRL_SMBPRINT_USERNAME", wxTextCtrl);
    m_pCtrlSmbPrintPassword = XRCCTRL(*this, "ID_TEXTCTRL_SMBPRINT_PASSWORD", wxTextCtrl);
    m_pCtrlCupsOptions = XRCCTRL(*this, "ID_PANEL_CUPSOPTIONS", wxPanel);
    m_pCtrlCupsPrivate = XRCCTRL(*this, "ID_RADIOBUTTON_CUPS_PRIVATE", wxRadioButton);
    m_pCtrlCupsPublic = XRCCTRL(*this, "ID_RADIOBUTTON_CUPS_PUBLIC", wxRadioButton);
    m_pCtrlCupsPrintUsername = XRCCTRL(*this, "ID_TEXTCTRL_CUPSPRINT_USERNAME", wxTextCtrl);
    m_pCtrlCupsPrintPassword = XRCCTRL(*this, "ID_TEXTCTRL_CUPSPRINT_PASSWORD", wxTextCtrl);
    m_pCtrlUsbOptions = XRCCTRL(*this, "ID_PANEL_USBIP", wxPanel);
    m_pCtrlSmbDiskOptions = XRCCTRL(*this, "ID_PANEL_SMBOPTIONS", wxPanel);
    m_pCtrlMountPoint = XRCCTRL(*this, "ID_TEXTCTRL_SHARE_MOUNTPOINT", wxTextCtrl);
    m_pCtrlUsername = XRCCTRL(*this, "ID_TEXTCTRL_SHARE_USERNAME", wxTextCtrl);
    m_pCtrlPassword = XRCCTRL(*this, "ID_TEXTCTRL_SHARE_PASSWORD", wxTextCtrl);
    // Set validators
    if (FindWindow(XRCID("ID_COMBOBOX_SMBDRIVER")))
        FindWindow(XRCID("ID_COMBOBOX_SMBDRIVER"))->SetValidator( wxGenericValidator(& m_sSmbDriver) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_SMB_PUBLIC")))
        FindWindow(XRCID("ID_RADIOBUTTON_SMB_PUBLIC"))->SetValidator( wxGenericValidator(& m_bSmbPublic) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SMBPRINT_USERNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_SMBPRINT_USERNAME"))->SetValidator( MyValidator(& m_sSmbPrintUsername) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SMBPRINT_PASSWORD")))
        FindWindow(XRCID("ID_TEXTCTRL_SMBPRINT_PASSWORD"))->SetValidator( MyValidator(& m_sSmbPrintPassword) );
    if (FindWindow(XRCID("ID_COMBOBOX_CUPSDRIVER")))
        FindWindow(XRCID("ID_COMBOBOX_CUPSDRIVER"))->SetValidator( wxGenericValidator(& m_sCupsDriver) );
    if (FindWindow(XRCID("ID_RADIOBUTTON_CUPS_PUBLIC")))
        FindWindow(XRCID("ID_RADIOBUTTON_CUPS_PUBLIC"))->SetValidator( wxGenericValidator(& m_bCupsPublic) );
    if (FindWindow(XRCID("ID_TEXTCTRL_CUPSPRINT_USERNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_CUPSPRINT_USERNAME"))->SetValidator( MyValidator(& m_sCupsPrintUsername) );
    if (FindWindow(XRCID("ID_TEXTCTRL_CUPSPRINT_PASSWORD")))
        FindWindow(XRCID("ID_TEXTCTRL_CUPSPRINT_PASSWORD"))->SetValidator( MyValidator(& m_sCupsPrintPassword) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SHARE_MOUNTPOINT")))
        FindWindow(XRCID("ID_TEXTCTRL_SHARE_MOUNTPOINT"))->SetValidator( MyValidator(& m_sMountPoint) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SHARE_USERNAME")))
        FindWindow(XRCID("ID_TEXTCTRL_SHARE_USERNAME"))->SetValidator( MyValidator(& m_sSmbDiskUsername) );
    if (FindWindow(XRCID("ID_TEXTCTRL_SHARE_PASSWORD")))
        FindWindow(XRCID("ID_TEXTCTRL_SHARE_PASSWORD"))->SetValidator( MyValidator(& m_sSmbDiskPassword) );
    ////@end ShareProperties content construction

    // Create custom windows not generated automatically here.

    ////@begin ShareProperties content initialisation
    ////@end ShareProperties content initialisation

    int minw = 0;
    int minh = 0;
    int w, h;
    m_pCtrlSmbDiskOptions->Show(false);
    m_pCtrlSmbPrintOptions->Show(true);
    m_pCtrlCupsOptions->Show(false);
    InvalidateBestSize();
    Layout();
    m_pCtrlSmbPrintOptions->SetMinSize(m_pCtrlSmbPrintOptions->GetBestSize());
    GetBestSize(&minw, &minh);

    m_pCtrlSmbDiskOptions->Show(false);
    m_pCtrlSmbPrintOptions->Show(false);
    m_pCtrlCupsOptions->Show(true);
    InvalidateBestSize();
    Layout();
    m_pCtrlCupsOptions->SetMinSize(m_pCtrlCupsOptions->GetBestSize());
    GetBestSize(&w, &h);
    if (w > minw)
        minw = w;
    if (h > minh)
        minh = h;

    m_pCtrlCupsOptions->Show(false);
    m_pCtrlSmbDiskOptions->Show(true);
    m_pCtrlSmbPrintOptions->Show(false);
    InvalidateBestSize();
    Layout();
    m_pCtrlSmbDiskOptions->SetMinSize(m_pCtrlSmbDiskOptions->GetBestSize());
    GetBestSize(&w, &h);
    if (w > minw)
        minw = w;
    if (h > minh)
        minh = h;
    SetMinSize(wxSize(minw, minh));

    if (m_iCurrentShare != -1) {
        ShareGroup sg = m_pCfg->aGetShareGroups().Item(m_iCurrentShare);
        wxBitmap bm = wxNullBitmap;
        m_pCtrlLocalShares->Append(sg.m_sShareName, bm, &sg);
        m_pCtrlLocalShares->SetSelection(0);
        m_pCtrlLocalShares->Enable(false);
        SetTitle(_("Modify shared resource - OpenNX"));
        switch (sg.m_eType) {
            case SharedResource::SHARE_UNKNOWN:
                break;
            case SharedResource::SHARE_SMB_DISK:
                m_pCtrlCupsOptions->Show(false);
                m_pCtrlSmbPrintOptions->Show(false);
                m_pCtrlSmbDiskOptions->Show(true);
                m_sMountPoint = sg.m_sAlias;
                m_sSmbDiskUsername = sg.m_sUsername;
                if (m_sSmbDiskUsername.IsEmpty())
                    m_sSmbDiskUsername = ::wxGetUserId();
                m_sSmbDiskPassword = sg.m_sPassword;
                Layout();
                break;
            case SharedResource::SHARE_SMB_PRINTER:
                m_pCtrlCupsOptions->Show(false);
                m_pCtrlSmbDiskOptions->Show(false);
                m_pCtrlSmbPrintOptions->Show(true);
                m_sSmbDriver = sg.m_sDriver;
                m_sSmbPrintUsername = sg.m_sUsername;
                if (m_sSmbPrintUsername.IsEmpty())
                    m_sSmbPrintUsername = ::wxGetUserId();
                m_sSmbPrintPassword = sg.m_sPassword;
                if (sg.m_bPublic)
                    m_pCtrlSmbPublic->SetValue(true);
                else
                    m_pCtrlSmbPrivate->SetValue(true);
                Layout();
                break;
            case SharedResource::SHARE_CUPS_PRINTER:
                m_pCtrlSmbPrintOptions->Show(false);
                m_pCtrlSmbDiskOptions->Show(false);
                m_pCtrlCupsOptions->GetSizer()->Layout();
                m_pCtrlCupsOptions->Show(true);
                m_sCupsPrintUsername = sg.m_sUsername;
                if (m_sCupsPrintUsername.IsEmpty())
                    m_sCupsPrintUsername = ::wxGetUserId();
                m_sCupsPrintPassword = sg.m_sPassword;
                if (sg.m_bPublic)
                    m_pCtrlCupsPublic->SetValue(true);
                else
                    m_pCtrlCupsPrivate->SetValue(true);
                Layout();
                break;
        }
        m_pCtrlMountPoint->SetValue(m_sMountPoint);
    } else {
        // Fetch list of shares
        if (m_bUseSmb) {
            SmbClient sc;
            m_aShares = sc.GetShares();
        }
        if (m_bUseCups) {
            CupsClient cc;
            ArrayOfShares cupsShares = cc.GetShares();
            WX_APPEND_ARRAY(m_aShares, cupsShares);
        }
        // Apparently, wxGTK (perhaps GTK itself) has a bug which
        // results in data pointers not being associated properly to the
        // ComboBox, if that ComboBox is sorted (wxCB_SORT attribute).
        // As a woraround, we use an *unsorted* ComboBox and sort the
        // shares before adding them to the ComboBox.
        m_aShares.Sort(cmpshares);

        // Build ComboBox content
        for (size_t i = 0; i < m_aShares.GetCount(); i++) {
            wxBitmap bm;
            switch (m_aShares[i].sharetype) {
                case SharedResource::SHARE_SMB_DISK:
                    bm = GetBitmapResource(wxT("res/smbfolder.png"));
                    break;
                case SharedResource::SHARE_SMB_PRINTER:
                    bm = GetBitmapResource(wxT("res/smbprinter.png"));
                    break;
                case SharedResource::SHARE_CUPS_PRINTER:
                    bm = GetBitmapResource(wxT("res/cupsprinter.png"));
                    break;
                default:
                    bm = wxNullBitmap;
                    break;
            }
            m_pCtrlLocalShares->Append(m_aShares[i].name, bm, m_aShares[i].GetThisVoid());
        }

        if (m_aShares.GetCount() > 0) {
            // Select first element of ComboBox
            m_pCtrlLocalShares->SetSelection(0);
            SharedResource *res = wxDynamicCast(m_pCtrlLocalShares->GetClientData(0), SharedResource);
            wxASSERT(res);
            switch (res->sharetype) {
                case SharedResource::SHARE_UNKNOWN:
                    break;
                case SharedResource::SHARE_SMB_DISK:
                    m_pCtrlSmbPrintOptions->Show(false);
                    m_pCtrlCupsOptions->Show(false);
                    m_pCtrlSmbDiskOptions->Show(true);
                    m_sMountPoint = res->name;
                    Layout();
                    break;
                case SharedResource::SHARE_SMB_PRINTER:
                    m_pCtrlSmbDiskOptions->Show(false);
                    m_pCtrlCupsOptions->Show(false);
                    m_pCtrlSmbPrintOptions->Show(true);
                    m_sSmbDriver = wxT("HP LaserJet ?");
                    Layout();
                    break;
                case SharedResource::SHARE_CUPS_PRINTER:
                    m_pCtrlSmbDiskOptions->Show(false);
                    m_pCtrlSmbPrintOptions->Show(false);
                    m_pCtrlCupsOptions->Show(true);
                    Layout();
                    break;
            }
        } else {
            wxLogMessage(_("No shares found"));
            m_pCtrlLocalShares->Enable(false);
            m_pCtrlMountPoint->Enable(false);
            m_pCtrlUsername->Enable(false);
            m_pCtrlPassword->Enable(false);
        }
    }
}

void ShareProperties::OnContextHelp(wxCommandEvent &)
{
    wxContextHelp contextHelp(this);
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_SHARE_LOCALNAME
 */

void ShareProperties::OnComboboxShareLocalnameSelected( wxCommandEvent& event )
{
    SharedResource *res = wxDynamicCast(m_pCtrlLocalShares->GetClientData(event.GetInt()), SharedResource);
    wxASSERT(res);
    switch (res->sharetype) {
        case SharedResource::SHARE_UNKNOWN:
            break;
        case SharedResource::SHARE_SMB_DISK:
            m_pCtrlCupsOptions->Show(false);
            m_pCtrlSmbPrintOptions->Show(false);
            m_pCtrlSmbDiskOptions->Show(true);
            m_sMountPoint = res->name;
            m_pCtrlMountPoint->SetValue(m_sMountPoint);
            Layout();
            break;
        case SharedResource::SHARE_SMB_PRINTER:
            m_sSmbDriver = wxT("HP LaserJet ?");
            m_pCtrlSmbDriver->SetValue(m_sSmbDriver);
            m_pCtrlCupsOptions->Show(false);
            m_pCtrlSmbDiskOptions->Show(false);
            m_pCtrlCupsOptions->Show(false);
            m_pCtrlCupsOptions->GetSizer()->Layout();
            m_pCtrlSmbPrintOptions->Show(true);
            Layout();
            break;
        case SharedResource::SHARE_CUPS_PRINTER:
            m_sCupsDriver = wxT("HP LaserJet ?");
            m_pCtrlSmbPrintOptions->Show(false);
            m_pCtrlSmbDiskOptions->Show(false);
            m_pCtrlCupsOptions->GetSizer()->Layout();
            m_pCtrlCupsOptions->Show(true);
            Layout();
            break;
    }
    event.Skip();
}




void ShareProperties::askForDefault(ArrayOfShareGroups &sg, ShareGroup &g)
{
    if (g.m_bDefault)
        return;
    wxString msg =
        wxString::Format(_("Do you want to make\n%s\nthe default printer?"), VMB(g.m_sShareName));
    wxMessageDialog d(this, msg, _("Default printer"), wxYES_NO|wxICON_QUESTION);
    if (d.ShowModal() == wxID_YES) {
        for (size_t i = 0; i < sg.GetCount(); i++)
            sg[i].m_bDefault = false;
        g.m_bDefault = true;
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ShareProperties::OnOkClick( wxCommandEvent& event )
{
    ArrayOfShareGroups sg = m_pCfg->aGetShareGroups();
    int nPrinters = 0;
    TransferDataFromWindow();
    for (size_t i = 0; i < sg.GetCount(); i++) {
        switch (sg[i].m_eType) {
            case SharedResource::SHARE_UNKNOWN:
                break;
            case SharedResource::SHARE_CUPS_PRINTER:
                nPrinters++;
                break;
            case SharedResource::SHARE_SMB_PRINTER:
                nPrinters++;
                break;
            case SharedResource::SHARE_SMB_DISK:
                if ((sg[i].m_sAlias == m_sMountPoint) && ((int)i != m_iCurrentShare)) {
                    wxMessageBox(_("Mountpoint is already used. Please chose another mountpoint"),
                            _("Invalid mountpoint"), wxICON_ERROR|wxOK);
                    if (m_iCurrentShare != -1)
                        m_pCtrlMountPoint->SetValue(sg[m_iCurrentShare].m_sAlias);
                    return;
                }
                break;
        }
    }
    if (m_iCurrentShare != -1) {
        switch (sg[m_iCurrentShare].m_eType) {
            case SharedResource::SHARE_UNKNOWN:
                break;
            case SharedResource::SHARE_SMB_DISK:
                sg[m_iCurrentShare].m_sAlias = m_sMountPoint;
                sg[m_iCurrentShare].m_sUsername = m_sSmbDiskUsername;
                sg[m_iCurrentShare].m_sPassword = m_sSmbDiskPassword;
                break;
            case SharedResource::SHARE_SMB_PRINTER:
                sg[m_iCurrentShare].m_sDriver = m_sSmbDriver;
                sg[m_iCurrentShare].m_bPublic = m_bSmbPublic;
                sg[m_iCurrentShare].m_sUsername = m_sSmbPrintUsername;
                sg[m_iCurrentShare].m_sPassword = m_sSmbPrintPassword;
                if (nPrinters > 0)
                    askForDefault(sg, sg[m_iCurrentShare]);
                break;
            case SharedResource::SHARE_CUPS_PRINTER:
                sg[m_iCurrentShare].m_sUsername = ::wxGetUserId();
                sg[m_iCurrentShare].m_sDriver = m_sCupsDriver;
                sg[m_iCurrentShare].m_bPublic = m_bCupsPublic;
                sg[m_iCurrentShare].m_sUsername = m_sCupsPrintUsername;
                sg[m_iCurrentShare].m_sPassword = m_sCupsPrintPassword;
                if (nPrinters > 0)
                    askForDefault(sg, sg[m_iCurrentShare]);
                break;
        }
        m_pCfg->aSetShareGroups(sg);
    } else {
        ShareGroup g;
        SharedResource *res = wxDynamicCast(m_pCtrlLocalShares->GetClientData(m_pCtrlLocalShares->GetSelection()), SharedResource);
        wxASSERT(res);
        myLogTrace(MYTRACETAG, wxT("selected: %d %p"), (int)event.GetInt(), res);

        g.m_eType = res->sharetype;
        g.m_sShareName = res->name;
        g.m_sGroupName = wxString::Format(wxT("Share%d"), (int)sg.GetCount());
        g.m_bDefault = false;
        switch (res->sharetype) {
            case SharedResource::SHARE_UNKNOWN:
                break;
            case SharedResource::SHARE_SMB_DISK:
                g.m_sAlias = m_sMountPoint;
                g.m_sUsername = m_sSmbDiskUsername;
                g.m_sPassword = m_sSmbDiskPassword;
                break;
            case SharedResource::SHARE_SMB_PRINTER:
                g.m_sDriver = m_sSmbDriver;
                g.m_bPublic = m_bSmbPublic;
                g.m_sUsername = m_sSmbPrintUsername;
                g.m_sPassword = m_sSmbPrintPassword;
                if (nPrinters > 0)
                    askForDefault(sg, g);
                break;
            case SharedResource::SHARE_CUPS_PRINTER:
                g.m_sDriver = wxT("cups driver");
                g.m_bPublic = m_bCupsPublic;
                g.m_sUsername = m_sCupsPrintUsername;
                g.m_sPassword = m_sCupsPrintPassword;
                if (nPrinters > 0)
                    askForDefault(sg, g);
                break;
        }
        sg.Add(g);
        m_pCfg->aSetShareGroups(sg);
        wxArrayString as = m_pCfg->aGetUsedShareGroups();
        as.Add(g.m_sGroupName);
        m_pCfg->aSetUsedShareGroups(as);
        m_pCfg->iSetUsedShareGroups(m_pCfg->iGetUsedShareGroups() + 1);
    }
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool ShareProperties::ShowToolTips()
{
    return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap ShareProperties::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    return CreateBitmapFromFile(name);
}

/*!
 * Get icon resources
 */

wxIcon ShareProperties::GetIconResource( const wxString& name )
{
    // Icon retrieval
    return CreateIconFromFile(name);
}
