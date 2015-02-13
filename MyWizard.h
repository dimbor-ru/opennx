// $Id: MyWizard.h 625 2011-06-09 14:50:12Z felfert $
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

#ifndef _MYWIZARD_H_
#define _MYWIZARD_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "MyWizard.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "MyWizard_symbols.h"
#include "wx/xrc/xmlres.h"
#include "wx/wizard.h"
#include "wx/valgen.h"
#include "wx/spinctrl.h"
////@end includes

#include "MyValidator.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class WizardPageWelcome;
class WizardPageSession;
class WizardPageDesktop;
class wxSpinCtrl;
class WizardPageSecurity;
class WizardPageFinish;
class WrappedStaticText;
////@end forward declarations
class MyXmlConfig;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_WIZARD 10076
#define ID_WIZARDPAGE_WELCOME 10077
#define ID_WIZARDPAGE_SESSION 10084
#define ID_WIZARDPAGE_DESKTOP 10133
#define ID_WIZARDPAGE_SECURITY 10141
#define ID_WIZARDPAGE_FINISH 10179
#define SYMBOL_MYWIZARD_IDNAME ID_WIZARD
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * MyWizard class declaration
 */

class MyWizard: public wxWizard, public KeyTypeCallback
{    
    DECLARE_CLASS( MyWizard )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    MyWizard( );
    MyWizard( wxWindow* parent, wxWindowID id = SYMBOL_MYWIZARD_IDNAME, const wxPoint& pos = wxDefaultPosition );

    ~MyWizard();

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MYWIZARD_IDNAME, const wxPoint& pos = wxDefaultPosition );

    /// Creates the controls and sizers
    void CreateControls();

    // Local implementation handles modality
    // on OSX differently.
    virtual bool RunWizard(wxWizardPage *firstPage);

    virtual wxSize GetPageSize() const;

    void EnableNext(bool enable = true);
    virtual void KeyTyped();
    void SetCancelled() { m_bCancelForced = true; }

    wxString sGetConfigName();

    MyXmlConfig *pGetConfig() { return m_pCfg; }

////@begin MyWizard event handler declarations

////@end MyWizard event handler declarations

////@begin MyWizard member function declarations

    /// Runs the wizard
    bool Run();

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end MyWizard member function declarations

private:
    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin MyWizard member variables
    WizardPageWelcome* m_pPageWelcome;
    WizardPageSession* m_pPageSession;
    WizardPageDesktop* m_pPageDesktop;
    WizardPageSecurity* m_pPageSecurity;
    WizardPageFinish* m_pPageFinish;
////@end MyWizard member variables

    wxButton *nextButton;
    MyXmlConfig *m_pCfg;
    bool m_bCancelForced;
    int minW, minH;
};

/*!
 * WizardPageWelcome class declaration
 */

class WizardPageWelcome: public wxWizardPageSimple
{    
    DECLARE_DYNAMIC_CLASS( WizardPageWelcome )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WizardPageWelcome( );

    WizardPageWelcome( wxWizard* parent );

    /// Creation
    bool Create( wxWizard* parent );

private:

    /// Creates the controls and sizers
    void CreateControls();

////@begin WizardPageWelcome event handler declarations

    /// wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_WELCOME
    void OnWizardpageWelcomePageChanged( wxWizardEvent& event );

////@end WizardPageWelcome event handler declarations

////@begin WizardPageWelcome member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end WizardPageWelcome member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin WizardPageWelcome member variables
////@end WizardPageWelcome member variables
};

/*!
 * WizardPageSession class declaration
 */

class WizardPageSession: public wxWizardPageSimple
{    
    DECLARE_DYNAMIC_CLASS( WizardPageSession )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WizardPageSession( );

    WizardPageSession( wxWizard* parent );

    /// Creation
    bool Create( wxWizard* parent );

    void KeyTyped();

private:

    /// Creates the controls and sizers
    void CreateControls();

    bool ConfigExists(wxString &);

    void CheckNextEnable();

////@begin WizardPageSession event handler declarations

    /// wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_SESSION
    void OnWizardpageSessionPageChanged( wxWizardEvent& event );

    /// wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_SESSION
    void OnWizardpageSessionPageChanging( wxWizardEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SESSION_NAME
    void OnTextctrlSessionNameUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SVRNAME
    void OnTextctrlSvrnameUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SVRPORT
    void OnTextctrlSvrportUpdated( wxCommandEvent& event );

////@end WizardPageSession event handler declarations

////@begin WizardPageSession member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end WizardPageSession member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin WizardPageSession member variables
    wxTextCtrl* m_pCtrlSessionName;
    wxTextCtrl* m_pCtrlHostName;
    wxTextCtrl* m_pCtrlPort;
    wxString m_sHostName;
    int m_iPort;
    int m_iConnectionSpeed;
    wxString m_sSessionName;
////@end WizardPageSession member variables

    bool m_bKeyTyped;
};

/*!
 * WizardPageDesktop class declaration
 */

class WizardPageDesktop: public wxWizardPageSimple
{    
    DECLARE_DYNAMIC_CLASS( WizardPageDesktop )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WizardPageDesktop( );
    WizardPageDesktop( wxWizard* parent );
    ~WizardPageDesktop( );

    /// Creation
    bool Create( wxWizard* parent );

private:
    void UpdateDialogConstraints(bool);
    void ShowRdpPropertyDialog();
    void ShowVncPropertyDialog();

    /// Creates the controls and sizers
    void CreateControls();

    void CheckNextEnable();

////@begin WizardPageDesktop event handler declarations

    /// wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_DESKTOP
    void OnWizardpageDesktopPageChanged( wxWizardEvent& event );

    /// wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_DESKTOP
    void OnWizardpageDesktopPageChanging( wxWizardEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DPROTO
    void OnComboboxDprotoSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DTYPE
    void OnComboboxDtypeSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_DSETTINGS
    void OnButtonDsettingsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_DISPTYPE
    void OnComboboxDisptypeSelected( wxCommandEvent& event );

////@end WizardPageDesktop event handler declarations

    void OnRdpDialogTimer(wxTimerEvent &);
    void OnVncDialogTimer(wxTimerEvent &);

////@begin WizardPageDesktop member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end WizardPageDesktop member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin WizardPageDesktop member variables
    wxComboBox* m_pCtrlDesktopType;
    wxButton* m_pCtrlDesktopSettings;
    wxComboBox* m_pCtrlDisplayType;
    wxSpinCtrl* m_pCtrlDisplayWidth;
    wxSpinCtrl* m_pCtrlDisplayHeight;
    int m_iSessionType;
    int m_iDesktopTypeDialog;
    int m_iDisplayType;
    int m_iDisplayWidth;
    int m_iDisplayHeight;
private:
    int m_iPseudoDesktopTypeIndex;
    int m_iPseudoDisplayTypeIndex;
////@end WizardPageDesktop member variables
    int m_iDesktopType;
    int m_iUnixDesktopType;
    wxTimer *m_pRdpDialogTimer;
    wxTimer *m_pVncDialogTimer;
};

/*!
 * WizardPageSecurity class declaration
 */

class WizardPageSecurity: public wxWizardPageSimple
{    
    DECLARE_DYNAMIC_CLASS( WizardPageSecurity )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WizardPageSecurity( );

    WizardPageSecurity( wxWizard* parent );

    /// Creation
    bool Create( wxWizard* parent );

private:

    /// Creates the controls and sizers
    void CreateControls();

////@begin WizardPageSecurity event handler declarations

    /// wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_SECURITY
    void OnWizardpageSecurityPageChanged( wxWizardEvent& event );

    /// wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_SECURITY
    void OnWizardpageSecurityPageChanging( wxWizardEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SCARD
    void OnCheckboxScardClick( wxCommandEvent& event );

////@end WizardPageSecurity event handler declarations

////@begin WizardPageSecurity member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end WizardPageSecurity member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin WizardPageSecurity member variables
    wxCheckBox* m_pCtrlUseSmartCard;
    wxCheckBox* m_pCtrlEnableSSL;
    bool m_bUseSmartCard;
    bool m_bEnableSSL;
////@end WizardPageSecurity member variables
};

/*!
 * WizardPageFinish class declaration
 */

class WizardPageFinish: public wxWizardPageSimple
{    
    DECLARE_DYNAMIC_CLASS( WizardPageFinish )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WizardPageFinish( );

    WizardPageFinish( wxWizard* parent );

    /// Creation
    bool Create( wxWizard* parent );

private:

    /// Creates the controls and sizers
    void CreateControls();

////@begin WizardPageFinish event handler declarations

    /// wxEVT_WIZARD_PAGE_CHANGED event handler for ID_WIZARDPAGE_FINISH
    void OnWizardpageFinishPageChanged( wxWizardEvent& event );

    /// wxEVT_WIZARD_PAGE_CHANGING event handler for ID_WIZARDPAGE_FINISH
    void OnWizardpageFinishPageChanging( wxWizardEvent& event );

////@end WizardPageFinish event handler declarations

////@begin WizardPageFinish member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end WizardPageFinish member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin WizardPageFinish member variables
    WrappedStaticText* m_pCtrlHeader;
    bool m_bCreateShortcut;
    bool m_bShowAdvancedConfig;
////@end WizardPageFinish member variables
};

#endif
    // _MYWIZARD_H_
