;
; $Id: opennx.iss 674 2012-02-05 05:55:40Z felfert $
;
#undef DEBUG

#define APPNAME "OpenNXCE"
; Automatically get version from executable resp. dll
#define APPEXE "setupdir\bin\opennx.exe"
#include "version.iss"

#define MSWU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall"
#define APPIDSTR "{56D797D7-543C-408F-BBEB-B56787873D2F}"
#define APPIDVAL "{" + APPIDSTR

[Setup]
AppName={#=APPNAME}
AppVersion={#=APPFULLVER}
AppVerName={#=APPFULLVERNAME}
AppPublisher=OpenNX Community Edition
AppPublisherURL=Community from http://unixforum.org
AppCopyright=(C) 2011 The OpenNX Team
;VersionInfoVersion={#=APPFULLVER}
DefaultDirName={pf}\{#=APPNAME}
DefaultGroupName={#=APPNAME}
#ifdef DEBUG
PrivilegesRequired=none
#endif
DisableStartupPrompt=true
ChangesAssociations=yes
OutputDir=.
OutputBaseFileName={#=SETUPFVNAME}
ShowLanguageDialog=no
MinVersion=0,5.0.2195sp3
AppID={#=APPIDVAL}
UninstallFilesDir={app}\uninstall
Compression=lzma/ultra64
SolidCompression=yes
SetupLogging=yes
WizardImageFile=compiler:wizmodernimage-IS.bmp
WizardSmallImageFile=compiler:wizmodernsmallimage-IS.bmp
; The following breaks in older wine versions, so we
; check the wine version in the invoking script and
; define BADWINE, if we are crossbuilding and have a
; broken wine version.
#ifndef BADWINE
SetupIconFile=setupdir\bin\nx.ico
#endif
UninstallDisplayIcon={app}\bin\opennx.exe
LicenseFile=lgpl.rtf

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[CustomMessages]
dticon=Create a &desktop icon
dticon_group=Additional icons:
cwizard=OpenNX Connection Wizard
sadmin=OpenNX Session Administrator
uninst_opennx=Uninstall OpenNX
fwadd=Adding firewall rules
doc_pconnect=Pconnect Manual

de.dticon=Desktop-Verknüpfung &anlegen
de.dticon_group=Zusätzliche Verknüpfungen:
de.cwizard=OpenNX Verbindungs-Assistent
de.sadmin=OpenNX Sitzungsverwaltung
de.uninst_opennx=Deinstalliere OpenNX
de.fwadd=Erstelle Firewall-Regeln
de.doc_pconnect=Pconnect Handbuch

[Tasks]
Name: "desktopicon"; Description: "{cm:dticon}"; GroupDescription: "{cm:dticon_group}";

[Files]
Source: setupdir\*; DestDir: {app}; Flags: recursesubdirs restartreplace replacesameversion uninsrestartdelete
#if BUILDXMING == "no"
Source: setupdir\bin\opennx.exe; DestDir: {app}; DestName: nxclient.exe; Flags: restartreplace replacesameversion uninsrestartdelete
#endif

[Icons]
Name: "{group}\OpenNX"; Filename: "{app}\bin\opennx.exe";
Name: "{group}\{cm:cwizard}"; Filename: "{app}\bin\opennx.exe"; Parameters: "--wizard"; IconFilename: "{app}\bin\opennx.exe"; IconIndex: 2;
Name: "{group}\{cm:sadmin}"; Filename: "{app}\bin\opennx.exe"; Parameters: "--admin"; IconFilename: "{app}\bin\opennx.exe"; IconIndex: 3
Name: "{group}\{cm:doc_pconnect}"; Filename: "{app}\share\pconnect.html";
Name: "{group}\{cm:uninst_opennx}"; Filename: "{uninstallexe}";
Name: "{commondesktop}\OpenNX"; Filename: "{app}\bin\opennx.exe"; Tasks: "desktopicon";

[Registry]
; Delete original NXClient file global associations
Root: HKLM; Subkey: "Software\Classes\.nxs"; ValueType: none; Flags: deletekey
Root: HKLM; Subkey: "Software\Classes\NXClient.session"; ValueType: none; Flags: deletekey;
;
Root: HKLM; Subkey: "Software\Classes\.nxs"; ValueType: String; ValueData: "OpenNX.session"; Flags: deletekey uninsdeletekey;
Root: HKLM; Subkey: "Software\Classes\.nxs"; ValueName: "Content Type"; ValueType: String; ValueData: "application/nx-session";
Root: HKLM; Subkey: "Software\Classes\OpenNX.session"; ValueType: String; ValueData: "OpenNX session file"; Flags: deletekey uninsdeletekey;
Root: HKLM; Subkey: "Software\Classes\OpenNX.session\DefaultIcon"; ValueType: String; ValueData: "{app}\bin\opennx.exe,1";
Root: HKLM; Subkey: "Software\Classes\OpenNX.session\shell\open\command"; ValueType: String; ValueData: """{app}\bin\opennx.exe"" --session ""%1""";
; Delete original NXClient class regs in HKCU
Root: HKCU; Subkey: "Software\Classes\.nxs"; ValueName: ""; ValueType: none; Flags: deletekey deletevalue
Root: HKCU; Subkey: "Software\Classes\.nxs"; ValueName: "Content Type"; ValueType: none; Flags: deletekey deletevalue
Root: HKCU; Subkey: "Software\Classes\NXClient.session"; ValueName: ""; ValueType: none; Flags: deletekey deletevalue
Root: HKCU; Subkey: "Software\Classes\OpenNX.session\DefaultIcon"; ValueType: none; Flags: deletekey deletevalue
Root: HKCU; Subkey: "Software\Classes\OpenNX.session\shell\open\command"; ValueType: none; Flags: deletekey deletevalue

[Run]
; Allow nxssh, nxesd, pulseaudio, Xming and NXWin in Windows firewall
#if FileExists("setupdir\bin\nxssh.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\bin\nxssh.exe"" ""OpenNX nxssh"" ENABLE"; StatusMsg: {cm:fwadd}; Flags: runhidden skipifdoesntexist
#endif
#if FileExists("setupdir\bin\nxesd.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\bin\nxesd.exe"" ""OpenNX nxesd"" ENABLE"; StatusMsg: {cm:fwadd}; Flags: runhidden skipifdoesntexist
#endif
#if FileExists("setupdir\bin\tracelog.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\bin\tracelog.exe"" ""OpenNX tracelog"" ENABLE"; StatusMsg: {cm:fwadd}; Flags: runhidden skipifdoesntexist
#endif
#if FileExists("setupdir\bin\pulseaudio.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\bin\pulseaudio.exe"" ""OpenNX pulseaudio"" ENABLE"; StatusMsg: {cm:fwadd}; Flags: runhidden skipifdoesntexist
#endif
#if FileExists("setupdir\bin\Xming.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\bin\Xming.exe"" ""OpenNX Xming"" ENABLE"; StatusMsg: {cm:fwadd}; Flags: runhidden skipifdoesntexist
#endif
#if FileExists("setupdir\bin\NXWin.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall add allowedprogram ""{app}\bin\NXWin.exe"" ""OpenNX NXWin"" ENABLE"; StatusMsg: {cm:fwadd}; Flags: runhidden skipifdoesntexist
#endif

[UninstallRun]
; Remove firewall exceptions
#if FileExists("setupdir\bin\nxssh.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\bin\nxssh.exe"" ALL"; Flags: runhidden skipifdoesntexist; RunOnceId: fwdelnxssh
#endif
#if FileExists("setupdir\bin\nxesd.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\bin\nxesd.exe"" ALL"; Flags: runhidden skipifdoesntexist; RunOnceId: fwdelnxesd
#endif
#if FileExists("setupdir\bin\tracelog.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\bin\tracelog.exe"" ALL"; Flags: runhidden skipifdoesntexist; RunOnceId: fwdelnxesd
#endif
#if FileExists("setupdir\bin\pulseaudio.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\bin\pulseaudio.exe"" ALL"; Flags: runhidden skipifdoesntexist; RunOnceId: fwdelnxesd
#endif
#if FileExists("setupdir\bin\Xming.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\bin\Xming.exe"" ALL"; Flags: runhidden skipifdoesntexist; RunOnceId: fwdelxming
#endif
#if FileExists("setupdir\bin\NXWin.exe")
Filename: "{sys}\netsh.exe"; Parameters: "firewall delete allowedprogram ""{app}\bin\NXWin.exe"" ALL"; Flags: runhidden skipifdoesntexist; RunOnceId: fwdelnxwin
#endif

#if BUILDXMING == "yes"
[UninstallDelete]
Type: files; Name: "{app}\share\Xming\font-dirs"

[Code]

procedure CurStepChanged(step: TSetupStep);
var
    fontDir, mkfsexe, s: String;
    i, r: Integer;
    FindRec: TFindRec;
    fontDirs: TStringList;

begin
    if step = ssPostInstall then begin
        (* Create {app}\share\Xming\font-dirs *)
        fontDir := ExpandConstant('{app}\share\Xming\fonts');
        fontDirs := TStringList.Create;
        (* First, create a list of all subdirs in fonts *)
        if DirExists(fontDir) then begin
            if FindFirst(fontDir + '\*', FindRec) then begin
                try
                    repeat
                        if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY <> 0 then
                            fontDirs.Append(fontDir + '\' + FindRec.Name);
                    until not FindNext(FindRec);
                finally
                    FindClose(FindRec);
                end;
            end;
        end;
        (* Add the windows font dir to the list *)
        fontDirs.Append(ExpandConstant('{fonts}'));
        (* Create the file, containing the comma-separated directory list. *)
        s := ExpandConstant('{app}\share\Xming\font-dirs');
        Log('Creating ' + s);
        SaveStringToFile(s, fontDirs.CommaText, false);
        (* Finally, run mkfontscale for all font directories *)
        mkfsexe := ExpandConstant('{app}\bin\mkfontscale.exe');
        for i := 0 to fontDirs.Count - 1 do begin
            s := AddQuotes(fontDirs[i]);
            Log('Creating fonts.scale and fonts.dir in ' + s);
            Exec(mkfsexe, s, '', SW_HIDE, ewWaitUntilTerminated, r);
            Exec(mkfsexe, '-b -s -l ' + s, '', SW_HIDE, ewWaitUntilTerminated, r);
        end;
    end;
end;

procedure CurUninstallStepChanged(ustep: TUninstallStep);
var
    s, txt, wfonts: String;
    i: Integer;
    fontDirs: TStringList;

begin
    if ustep = usUninstall then begin
        s := ExpandConstant('{app}\share\Xming\font-dirs');
        wfonts := ExpandConstant('{fonts}');
        if LoadStringFromFile(s, txt) then begin
            fontDirs := TStringList.Create;
            fontDirs.CommaText := txt;
            for i := 0 to fontDirs.Count - 1 do begin
                s := fontDirs[i];
                if s <> wfonts then begin
                    s := AddBackSlash(s);
                    DeleteFile(s + 'fonts.scale');
                    DeleteFile(s + 'fonts.dir');
                end;
            end;
        end;
    end;
end;
#endif
