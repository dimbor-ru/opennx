/* $Id: osdep.c 729 2013-11-25 17:58:26Z felfert $
 *
 * Copyright (C) 2006 The OpenNX Team
 * Author: Fritz Elfert
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
/*
 * Defines canonicalized platform names (e.g. __LINUX__)
 */
#include <wx/platform.h>

#ifdef APP_OPENNX
# ifdef HAVE_LIBCURL
#  include <curl/curl.h>
static void __attribute__ ((constructor))
initcurl()
{
    curl_global_init(CURL_GLOBAL_ALL);
}
# endif
#endif

int inKdeSession = 0;

#ifdef __WXMSW__
#include <windows.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <stdio.h>

static char _kbd[_MAX_PATH+1];

char *x11_keyboard_type = _kbd;
/* dummy for now */
char *x11_socket_path = "";

/* Mappings from windows to X11 keyboard names */
typedef struct {
    unsigned int winlayout;
    int winkbtype;
    char *xkbmodel;
    char *xkblayout;
} WinKBLayoutRec, *WinKBLayoutPtr;

static WinKBLayoutRec winKBLayouts[] = 
{
    {  0x404, -1, "pc105", "zh_TW"},
    {  0x405, -1, "pc105", "cz"},
    {0x10405, -1, "pc105", "cz_qwerty"},
    {  0x406, -1, "pc105", "dk"},
    {  0x407, -1, "pc105", "de"},
    {0x10407, -1, "pc105", "de"},
    {  0x807, -1, "pc105", "de_CH"},
    {  0x409, -1, "pc105", "us"},
    {0x10409, -1, "pc105", "dvorak"}, 
    {0x20409, -1, "pc105", "us_intl"}, 
    {  0x809, -1, "pc105", "gb"},
    { 0x1809, -1, "pc105", "ie"},
    {  0x40a, -1, "pc105", "es"},
    {  0x40b, -1, "pc105", "fi"},
    {  0x40c, -1, "pc105", "fr"},
    {  0x80c, -1, "pc105", "be"},
    {  0xc0c, -1, "pc105", "ca_enhanced"},
    { 0x100c, -1, "pc105", "fr_CH"},
    {  0x40e, -1, "pc105", "hu"},
    {  0x410, -1, "pc105", "it"},
    {  0x411,  7, "jp106", "jp"},
    {  0x813, -1, "pc105", "be"},  
    {  0x414, -1, "pc105", "no"},
    {  0x415, -1, "pc105", "pl"},
    {  0x416, -1, "pc105", "br"},
    {0x10416, -1, "abnt2", "br"},
    {  0x816, -1, "pc105", "pt"},
    {  0x41a, -1, "pc105", "hr"},
    {  0x41d, -1, "pc105", "se"},
    {  0x424, -1, "pc105", "si"},
    {  0x425, -1, "pc105", "ee"},
    {     -1, -1, NULL,    NULL}
};

/* Listing of language codes from MSDN

Support ID       XKB        Language
====================================================================
   ?    0x0000              Language Neutral
   ?    0x0400              Process or User Default Language
   ?    0x0800              System Default Language
        0x0401              Arabic (Saudi Arabia)
        0x0801              Arabic (Iraq)
        0x0c01              Arabic (Egypt)
        0x1001              Arabic (Libya)
        0x1401              Arabic (Algeria)
        0x1801              Arabic (Morocco)
        0x1c01              Arabic (Tunisia)
        0x2001              Arabic (Oman)
        0x2401              Arabic (Yemen)
        0x2801              Arabic (Syria)
        0x2c01              Arabic (Jordan)
        0x3001              Arabic (Lebanon)
        0x3401              Arabic (Kuwait)
        0x3801              Arabic (U.A.E.)
        0x3c01              Arabic (Bahrain)
        0x4001              Arabic (Qatar)
                            Arabic (102) AZERTY        				
        0x0402              Bulgarian
        0x0403              Catalan
        0x0404              Chinese (Taiwan)
        0x0804              Chinese (PRC)
        0x0c04              Chinese (Hong Kong SAR, PRC)
        0x1004              Chinese (Singapore)
        0x1404              Chinese (Macao SAR) (98/ME,2K/XP)
   X    0x0405  cz          Czech
   X            cz_qwerty   Czech (QWERTY)
                            Czech (Programmers)
   X    0x0406  dk          Danish
   X    0x0407  de          German (Standard)
   X    0x0807  de_CH       German (Switzerland)
        0x0c07              German (Austria)
        0x1007              German (Luxembourg)
        0x1407              German (Liechtenstein)
        0x0408              Greek
   X    0x0409  us          English (United States)
   X    0x0809  gb          English (United Kingdom)
        0x0c09              English (Australian)
        0x1009              English (Canadian)
        0x1409              English (New Zealand)
   X    0x1809  ie          English (Ireland)
        0x1c09              English (South Africa)
        0x2009              English (Jamaica)
        0x2409              English (Caribbean)
        0x2809              English (Belize)
        0x2c09              English (Trinidad)
        0x3009              English (Zimbabwe) (98/ME,2K/XP)
        0x3409              English (Philippines) (98/ME,2K/XP)
   X    0x040a  es          Spanish (Spain, Traditional Sort)
        0x080a              Spanish (Mexican)
        0x0c0a              Spanish (Spain, Modern Sort)
        0x100a              Spanish (Guatemala)
        0x140a              Spanish (Costa Rica)
        0x180a              Spanish (Panama)
        0x1c0a              Spanish (Dominican Republic)
        0x200a              Spanish (Venezuela)
        0x240a              Spanish (Colombia)
        0x280a              Spanish (Peru)
        0x2c0a              Spanish (Argentina)
        0x300a              Spanish (Ecuador)
        0x340a              Spanish (Chile)
        0x380a              Spanish (Uruguay)
        0x3c0a              Spanish (Paraguay)
        0x400a              Spanish (Bolivia)
        0x440a              Spanish (El Salvador)
        0x480a              Spanish (Honduras)
        0x4c0a              Spanish (Nicaragua)
        0x500a              Spanish (Puerto Rico)
   X    0x040b  fi          Finnish
                            Finnish (with Sami)
   X    0x040c  fr          French (Standard)
   X    0x080c  be          French (Belgian)
   .    0x0c0c              French (Canadian)
                            French (Canadian, Legacy)
                            Canadian (Multilingual)
   X    0x100c  fr_CH       French (Switzerland)
        0x140c              French (Luxembourg)
        0x180c              French (Monaco) (98/ME,2K/XP)
        0x040d              Hebrew
   X    0x040e  hu          Hungarian
   .    0x040f              Icelandic
   X    0x0410  it          Italian (Standard)
        0x0810              Italian (Switzerland)
   X    0x0411  jp          Japanese
        0x0412              Korean
        0x0812              Korean (Johab) (95,NT)
   .    0x0413              Dutch (Netherlands)
   X    0x0813  be          Dutch (Belgium)
   X    0x0414  no          Norwegian (Bokmal)
        0x0814              Norwegian (Nynorsk)
   .    0x0415              Polish
   X    0x0416  br          Portuguese (Brazil)
   X    0x0816  pt          Portuguese (Portugal)
   .    0x0418              Romanian
        0x0419              Russian
   .    0x041a              Croatian
   .    0x081a              Serbian (Latin)
   .    0x0c1a              Serbian (Cyrillic)
        0x101a              Croatian (Bosnia and Herzegovina)
        0x141a              Bosnian (Bosnia and Herzegovina)
        0x181a              Serbian (Latin, Bosnia, and Herzegovina)
        0x1c1a              Serbian (Cyrillic, Bosnia, and Herzegovina)
   .    0x041b              Slovak
   .    0x041c              Albanian
   X    0x041d  se          Swedish
        0x081d              Swedish (Finland)
        0x041e              Thai
        0x041f              Turkish
        0x0420              Urdu (Pakistan) (98/ME,2K/XP) 
        0x0820              Urdu (India)
        0x0421              Indonesian
        0x0422              Ukrainian
        0x0423              Belarusian
   .    0x0424              Slovenian
        0x0425              Estonian
        0x0426              Latvian
        0x0427              Lithuanian
        0x0827              Lithuanian (Classic) (98)
        0x0429              Farsi
        0x042a              Vietnamese (98/ME,NT,2K/XP)
        0x042b              Armenian. This is Unicode only. (2K/XP)
                            Armenian Eastern
                            Armenian Western
        0x042c              Azeri (Latin)
        0x082c              Azeri (Cyrillic)
        0x042d              Basque
        0x042f              Macedonian (FYROM)
        0x0430              Sutu
        0x0432              Setswana/Tswana (South Africa)
        0x0434              isiXhosa/Xhosa (South Africa)
        0x0435              isiZulu/Zulu (South Africa)
        0x0436              Afrikaans
        0x0437              Georgian. This is Unicode only. (2K/XP)
   .    0x0438              Faeroese
        0x0439              Hindi. This is Unicode only. (2K/XP)
        0x043a              Maltese (Malta)
        0x043b              Sami, Northern (Norway)
        0x083b              Sami, Northern (Sweden)
        0x0c3b              Sami, Northern (Finland)
        0x103b              Sami, Lule (Norway)
        0x143b              Sami, Lule (Sweden)
        0x183b              Sami, Southern (Norway)
        0x1c3b              Sami, Southern (Sweden)
        0x203b              Sami, Skolt (Finland)
        0x243b              Sami, Inari (Finland)
        0x043e              Malay (Malaysian)
        0x083e              Malay (Brunei Darussalam)
        0x0440              Kyrgyz. (XP)
        0x0441              Swahili (Kenya)
        0x0443              Uzbek (Latin)
        0x0843              Uzbek (Cyrillic)
        0x0444              Tatar (Tatarstan)
        0x0445              Bengali (India)
                            Bengali (Inscript)
        0x0446              Punjabi. This is Unicode only. (XP)
        0x0447              Gujarati. This is Unicode only. (XP)
        0x0449              Tamil. This is Unicode only. (2K/XP)
        0x044a              Telugu. This is Unicode only. (XP)
        0x044b              Kannada. This is Unicode only. (XP)
        0x044c              Malayalam (India)
        0x044e              Marathi. This is Unicode only. (2K/XP)
        0x044f              Sanskrit. This is Unicode only. (2K/XP)
        0x0450              Mongolian (XP)
        0x0452              Welsh (United Kingdom)
        0x0455              Burmese
        0x0456              Galician (XP)
        0x0457              Konkani. This is Unicode only. (2K/XP)
        0x045a              Syriac. This is Unicode only. (XP)
        0x0465              Divehi. This is Unicode only. (XP)
                            Divehi (Phonetic)
                            Divehi (Typewriter)
        0x046b              Quechua (Bolivia)
        0x086b              Quechua (Ecuador)
        0x0c6b              Quechua (Peru)
        0x046c              Sesotho sa Leboa/Northern Sotho (South Africa)
        0x007f              LOCALE_INVARIANT. See MAKELCID.
        0x0481              Maori (New Zealand)
*/

static void __attribute__ ((constructor))
getkeyboardtype()
{
    char *xkbmodel = NULL;
    char *xkblayout = NULL;
    char layoutName[KL_NAMELENGTH];
    char regLayoutName[256];
    int keyboardType = GetKeyboardType(0);
    if (keyboardType > 0 && GetKeyboardLayoutNameA(layoutName)) {
        WinKBLayoutPtr pLayout;

        unsigned int layoutNum = strtoul(layoutName, (char **)NULL, 16);
        if ((layoutNum & 0xffff) == 0x411) {
            /* The japanese layouts know a lot of different IMEs which all have
               different layout numbers set. Map them to a single entry. 
               Same might apply for chinese, korean and other symbol languages
               too */
            layoutNum = (layoutNum & 0xffff);
            if (keyboardType == 7) {
                /* Japanese layouts have problems with key event messages
                   such as the lack of WM_KEYUP for Caps Lock key.
                   Loading US layout fixes this problem. */
                LoadKeyboardLayoutA("00000409", KLF_ACTIVATE);
            }
        }

        for (pLayout = winKBLayouts; pLayout->winlayout != -1; pLayout++) {
            if (pLayout->winlayout != layoutNum)
                continue;
            if (pLayout->winkbtype > 0 && pLayout->winkbtype != keyboardType)
                continue;
            xkbmodel = pLayout->xkbmodel;
            xkblayout = pLayout->xkblayout;
            break;
        }

        /* Fallback to registry */
        if (NULL == xkbmodel) {
            HKEY regkey = NULL;
            const char *keyname = "SYSTEM\\CurrentControlSet\\Control\\DosKeybCodes\\";
            DWORD namesize = sizeof(regLayoutName);

            if (ERROR_SUCCESS == RegOpenKeyA(HKEY_LOCAL_MACHINE, keyname, &regkey)) {
                if (ERROR_SUCCESS == RegQueryValueExA(regkey, layoutName,
                            0, NULL, (unsigned char *)regLayoutName, &namesize)) {
                    xkbmodel = "pc105";
                    xkblayout = regLayoutName;
                }
                RegCloseKey(regkey);
            }
        }
    }
    /* Last resort to pc101-us */
    if (NULL == xkbmodel) {
        xkbmodel = "pc101";
        xkblayout = "us";
    }
    snprintf(_kbd, sizeof(_kbd), "%s/%s", xkbmodel, xkblayout);
}

int getXmingPort(int firstfree)
{
    while (firstfree >= 6000) {
        int dpy = firstfree - 6000;
        char name[MAX_PATH];
        snprintf(name, sizeof(name), "Global\\XMING_DISPLAY:%d", dpy);
        HANDLE h = OpenMutexA(SYNCHRONIZE, FALSE, name);
        if (NULL != h) {
            CloseHandle(h);
            return firstfree;
        }
        firstfree--;
    }
    return 0;
}

/**
 * Helper function to get the parent process id on
 * windows.
 */
long getppid()
{
    OSVERSIONINFO osver;
    HINSTANCE hKernel32;
    HANDLE hSnapShot;
    PROCESSENTRY32 procentry;
    long mypid;
    long ppid = 0;

    /* ToolHelp Function Pointers.*/
    HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD);
    BOOL (WINAPI *lpfProcess32First)(HANDLE,LPPROCESSENTRY32);
    BOOL (WINAPI *lpfProcess32Next)(HANDLE,LPPROCESSENTRY32);

    osver.dwOSVersionInfoSize = sizeof(osver);
    if (!GetVersionEx(&osver))
        return 0;
    if (osver.dwPlatformId != VER_PLATFORM_WIN32_NT)
        return 0;
    if ((hKernel32 = LoadLibraryA("kernel32.dll")) == NULL)
        return 0;
    lpfCreateToolhelp32Snapshot= (HANDLE(WINAPI *)(DWORD,DWORD))
        GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
    lpfProcess32First= (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
        GetProcAddress(hKernel32, "Process32First");
    lpfProcess32Next= (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
        GetProcAddress(hKernel32, "Process32Next");
    if (lpfProcess32Next == NULL || lpfProcess32First == NULL || lpfCreateToolhelp32Snapshot == NULL) {
        FreeLibrary(hKernel32);
        return 0;
    }
    hSnapShot = lpfCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE) {
        FreeLibrary(hKernel32);
        return 0;
    }
    memset((LPVOID)&procentry,0,sizeof(PROCESSENTRY32));
    procentry.dwSize = sizeof(PROCESSENTRY32);
    mypid = GetCurrentProcessId();
    if (lpfProcess32First(hSnapShot, &procentry)) {
        do {
            if (mypid == (long)procentry.th32ProcessID) {
                ppid =  procentry.th32ParentProcessID;
                break;
            }
            procentry.dwSize = sizeof(PROCESSENTRY32);
        } while (lpfProcess32Next(hSnapShot, &procentry));
    }	
    FreeLibrary(hKernel32);
    return ppid;
}

static DWORD detachedPID = 0;

/**
 * Create a detached process, without showing it's
 * console window.
 *
 * wxWidgets hides a console window only if the process
 * is started with redirection. This creates additional I/O-threads
 * as well as a hidden window for IPC. We use our own function
 * in order to eliminate this overhead.
 *
 * Returns: 0 on success, win32 errorcode on failure.
 */
int CreateDetachedProcess(const char *cmdline) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE|DETACHED_PROCESS;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    if (CreateProcessA(NULL, (char *)cmdline, NULL, NULL,
                FALSE, dwFlags, NULL, NULL, &si, &pi)) {
        detachedPID = pi.dwProcessId;
        //{ Djelf
        if (WaitForSingleObject(pi.hProcess, 500) == 0) {
            detachedPID = 0;
            return GetLastError();
        }
        //} Djelf
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    }
    detachedPID = 0;
    return GetLastError();
}

int GetDetachedPID() {
    return detachedPID;
}

/**
 * Xming can handle multiple monitors only, if
 * *all* monitors have the same geometry and color depth.
 * This function checks for that constraint.
 *
 * Returns number of monitors or 0, if constraits not met.
 *
 * TODO: Check, if the same applies to NXWin (very likely)
 */
int checkMultiMonitors() {
    DWORD didx = 0;
    int mnum = 0;
    DISPLAY_DEVICE dd;
    int w = 0, h = 0, bpp = 0;

    memset(&dd, 0, sizeof(dd));
    dd.cb = sizeof(dd);
    while (EnumDisplayDevices(0, didx, &dd, 0)) {
        if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {
            if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {
                /* get information about the monitor attached to this display adapter.
                 * Dualhead cards and laptop video cards can have multiple monitors attached.
                 */
                DISPLAY_DEVICE ddMon;
                memset(&ddMon, 0, sizeof(ddMon));
                ddMon.cb = sizeof(ddMon);
                DWORD midx = 0;

                /* This enumeration may not return the correct monitor if multiple monitors are attached.
                 * This is because not all display drivers return the ACTIVE flag for the monitor
                 * that is actually active.
                 */
                while (EnumDisplayDevices(dd.DeviceName, midx, &ddMon, 0)) {
                    DEVMODE dm;
                    memset(&dm, 0, sizeof(dm));
                    dm.dmSize = sizeof(dm);
                    if (EnumDisplaySettingsEx(ddMon.DeviceName, ENUM_CURRENT_SETTINGS, &dm, 0)) {
                        if (0 == mnum) {
                            w = dm.dmPelsWidth;
                            h = dm.dmPelsHeight;
                            bpp = dm.dmBitsPerPel;
                        } else {
                            if (w != dm.dmPelsWidth)
                                return 0;
                            if (h != dm.dmPelsHeight)
                                return 0;
                            if (bpp != dm.dmBitsPerPel)
                                return 0;
                        }
                        mnum++;
                    }
                    midx++;
                }
            }
        }
        didx++;
    }
    return mnum;
}

#else /* ! __WXMSW__ */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/WinUtil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <string.h>

static char _spath[PATH_MAX+1];
static char _kbd[PATH_MAX+1];

char *x11_socket_path = _spath;
char *x11_keyboard_type = _kbd;

#ifdef __WXMAC__
#include <time.h>

static void fatal(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    /* On MacOS, we use osascript (AppleScript) for displaying a simple
     * error message. (wxWidgets is not initialized yet).
     */
    FILE *p = popen("osascript", "w");
    fprintf(p, "display dialog \"");
    vfprintf(p, fmt, ap);
    fprintf(p, "\" buttons {\"OK\"} default button \"OK\" with icon stop with title \"OpenNX Error\"\n");
    pclose(p);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}
#endif

# ifdef __WXMAC__

/* get first free TCP port */
unsigned short
macFirstFreePort(unsigned short startPort) {
    int on = 1;
    unsigned short port = startPort;
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_aton("127.0.0.1", &sa.sin_addr);
    while (port < 65535) {
        int sock = socket(AF_INET, SOCK_STREAM, 6);
        if (sock >= 0) {
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == 0) {
                sa.sin_port = htons(port);
                if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) == 0) {
                    close(sock);
                    return port;
                }
            }
            close(sock);
        }
        port++;
    }
    return 0;
}

static const char *quartz_locations[] = {
    "/opt/X11/bin/quartz-wm",
    "/usr/X11R6/bin/quartz-wm",
    "/usr/bin/quartz-wm",
    NULL
};

/* Startup XDarwin and try connecting to :0 */
static Display *launchX11() {
    Display *ret = NULL;
    time_t ts = time(NULL) + 15; /* Wait 15sec for X startup */
    const char **qlptr;
    char qcmd[64];
    system("/usr/X11R6/bin/X :0 -nolisten tcp &");
    while (!ret) {
        ret = XOpenDisplay(":0");
        if (!ret) {
            sleep(1);
            if (time(NULL) > ts)
                fatal("Timeout while waiting for X server.");
        }
    }
    putenv("DISPLAY=:0");
    /* If the following fails, there is usually a WM already running
     * so we don't care ... */
    for (qlptr = quartz_locations; *qlptr; ++qlptr) {
        if (0 == access(qlptr, X_OK)) {
            snprintf(qcmd, sizeof(qcmd), "%s &", qlptr);
            system(qcmd);
            return ret;
        }
    }
    return ret;
}
# endif /* __WXMAC__ */

#ifdef __WXMAC__
extern const char *getMacKeyboard();
#endif

    static void __attribute__ ((constructor))
getx11socket()
{
    memset(&_spath, 0, sizeof(_spath));
    memset(&_kbd, 0, sizeof(_kbd));
    Display *dpy = XOpenDisplay(NULL);
#ifdef __WXMAC__
    /* Start X server, if necessary */
    if (!dpy)
        dpy = launchX11();
#endif
    if (dpy) {
        /* Find out X11 socket path */
        struct sockaddr_un una;
        socklen_t sl = sizeof(una);
        if (getpeername(ConnectionNumber(dpy), (struct sockaddr *)&una, &sl)) {
            perror("getpeername");
            exit(-1);
        } else {
            if (una.sun_family == AF_UNIX) {
                sl -= ((char *)&una.sun_path - (char *)&una);
                sl = (sl > sizeof(_spath)) ? sizeof(_spath) : sl;
                strncpy(_spath, una.sun_path, sl);
            }
        }

        Atom a = XInternAtom(dpy, "_KDE_RUNNING", True);
        /* Check, if we are running inside a KDE session */
        inKdeSession = (a != None);

        a = XInternAtom(dpy, "_XKB_RULES_NAMES", True);
        /* Get XKB rules (current keyboard layout language) */
        if (a != None) {
            Atom type;
            int fmt;
            unsigned long n;
            unsigned long ba;
            char *prop;
            int res = XGetWindowProperty(dpy,
                    RootWindowOfScreen(DefaultScreenOfDisplay(dpy)),
                    a, 0, PATH_MAX, False, AnyPropertyType,
                    &type, &fmt, &n, &ba, (unsigned char **)&prop);
            if ((res == Success) && (fmt == 8) && prop) {
                unsigned long i = 0;
                int idx = 0;
                while (i < n) {
                    switch (idx++) {
                        case 1:
                            strncat(_kbd, prop, PATH_MAX);
                            strncat(_kbd, "/", PATH_MAX);
                            break;
                        case 2:
                            strncat(_kbd, prop, PATH_MAX);
                            break;
                    }
                    i += strlen(prop) + 1;
                    prop += strlen(prop) + 1;
                }
            }
        }
        XCloseDisplay(dpy);
    }
#if 0 // ifdef __WXMAC__
    if ((_kbd[0] == '\0') || strstr(_kbd, "empty")) {
        strncpy(_kbd, getMacKeyboard(), sizeof(_kbd));
    }
#endif
}
#endif /* !__WXMSW__ */

#ifndef __WXMSW__
/*
 * Close a foreign X11 window (just like a window-manager would do.
 */
void close_foreign(long parentID)
{
#if defined(__LINUX__) || defined(__OPENBSD__) || defined(__WXMAC__)
    Display *dpy = XOpenDisplay(NULL);
    if (dpy) {
        XClientMessageEvent ev;
        ev.type = ClientMessage;
        ev.window = parentID;
        ev.message_type = XInternAtom(dpy, "WM_PROTOCOLS", True);
        ev.format = 32;
        ev.data.l[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
        ev.data.l[1] = CurrentTime;
        XSendEvent(dpy, parentID, False, 0, (XEvent *)&ev);
        XCloseDisplay(dpy);
    }
#else
    (void)parentID;
#pragma message(__FILE__ " : warning: Implement close_foreign")
#endif
}

/*
 * Reparent our custom toolbar to the top center of a given foreign
 * X11 window. Since there is no way to retrieve our toolbar's X11-WindowID,
 * we use the _NET_WM_PID property (which automatically get's set by gdk), to find
 * our window. (At the time of the call it obviously should be the only one).
 * After that, proceed with a standard X11 reparenting ...
 */
void reparent_pulldown(long parentID)
{
#if defined(__LINUX__) || defined(__OPENBSD__) || defined(__WXMAC__)
    Display *dpy = XOpenDisplay(NULL);
    if (dpy) {
        Atom a = XInternAtom(dpy, "_NET_WM_PID", True);
        if (a != None) {
            int s;
            for (s = 0; s < ScreenCount(dpy); s++) {
                Window root = RootWindow(dpy, s);
                Window dummy;
                Window *childs = NULL;
                unsigned int nchilds = 0;
                if (XQueryTree(dpy, root, &dummy, &dummy, &childs, &nchilds)) {
                    unsigned int c;
                    for (c = 0; c < nchilds; c++) {
                        Window w = childs[c];
                        if (w != None) {
                            Atom type;
                            int fmt;
                            unsigned long n;
                            unsigned long ba;
                            unsigned long *prop;
                            int res = XGetWindowProperty(dpy, w, a, 0, 32, False,
                                    XA_CARDINAL, &type, &fmt, &n, &ba, (unsigned char **)&prop);
                            if ((res == Success) && (fmt = 32) && (n == 1) && prop) {
                                if (*prop == (unsigned long)getpid()) {
                                    int dummy;
                                    unsigned int pw, cw, udummy;
                                    Window wdummy;

                                    XGetGeometry(dpy, w, &wdummy, &dummy, &dummy,
                                            &cw, &udummy, &udummy, &udummy);
                                    XGetGeometry(dpy, parentID, &wdummy, &dummy, &dummy,
                                            &pw, &udummy, &udummy, &udummy);
                                    XReparentWindow(dpy, w, parentID, pw / 2 - cw / 2, 0);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        XCloseDisplay(dpy);
    }
#else
    (void)parentID;
#pragma message(__FILE__ " : warning: Implement reparent_pulldown")
#endif
}

void close_sid(const char *sid)
{
#if defined(__LINUX__) || defined(__OPENBSD__) || defined(__WXMAC__)
    Display *dpy = XOpenDisplay(NULL);
    int closed = 0;
    if (dpy) {
        int s;
        for (s = 0; s < ScreenCount(dpy); s++) {
            Window root = RootWindow(dpy, s);
            Window dummy;
            Window *childs = NULL;
            unsigned int nchilds = 0;
            if (XQueryTree(dpy, root, &dummy, &dummy, &childs, &nchilds)) {
                unsigned int c;
                for (c = 0; c < nchilds; c++) {
                    Window w = XmuClientWindow(dpy, childs[c]);
                    if (w != None) {
                        char **cargv = NULL;
                        int cargc = 0;
                        if (XGetCommand(dpy, w, &cargv, &cargc)) { 
                            if ((cargc > 3) &&
                                    (strcmp(cargv[0], "/usr/NX/bin/nxagent") == 0) &&
                                    (strcmp(cargv[1], "-D") == 0) &&
                                    (strcmp(cargv[2], "-options") == 0) &&
                                    (strstr(cargv[3], sid) != NULL)) {
                                close_foreign(w);
                                closed = 1;
                                XFreeStringList(cargv);
                                break;
                            }
                        }
                        XFreeStringList(cargv);
                    }
                }
            }
            if (closed)
                break;
        }
        XCloseDisplay(dpy);
    }
#else
    (void)sid;
#pragma message(__FILE__ " : warning: Implement close_sid")
#endif
}

#endif /* !__WXMSW__ */
