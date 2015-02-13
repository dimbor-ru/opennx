/* $Id: MacOSX.c 680 2012-02-09 23:31:11Z felfert $
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

/*
 * Defines canonicalized platform names (e.g. __LINUX__)
 */
#include <wx/platform.h>

#ifdef __WXMAC__
/*
 * Allow app to be started from the commandline
 */
#include <ApplicationServices/ApplicationServices.h>
static void __attribute__ ((constructor))
    makeForegroundApp()
{
    ProcessSerialNumber PSN;
    if (noErr == GetCurrentProcess(&PSN)) {
# ifdef APP_WATCHREADER
        TransformProcessType(&PSN, 0);
# else
        TransformProcessType(&PSN, kProcessTransformToForegroundApplication);
# endif
    }
}

/*
 * 10.4 compatibility. Hence the use of deprecated keyboard services.
 */
#include <Carbon/Carbon.h>
const char *getMacKeyboard() {
    static char ret[256];
	KeyboardLayoutRef klr;
    memset(ret, 0, sizeof(ret));
	if (noErr == KLGetCurrentKeyboardLayout(&klr)) {
		unsigned int pt = kKLLanguageCode;
		const void *oValue;
		if (noErr == KLGetKeyboardLayoutProperty(klr, pt, &oValue)) {
			char buf[128];
			if (CFStringGetCString((CFStringRef)oValue, buf, sizeof(buf), kCFStringEncodingISOLatin1)) {
                const char *ktype = "pc105";
                snprintf(ret, sizeof(ret), "%s/%s", ktype, buf);
			}
		}
	}
    return ret;
}

#include <IOKit/IOKitLib.h>
#ifndef kIOPlatformUUIDKey
# define kIOPlatformUUIDKey	"IOPlatformUUID"
#endif
const char *getMacMachineID()
{
    static char ret[256];
    io_registry_entry_t ioRegistryRoot =
        IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/");
    CFStringRef uuidCf =
        (CFStringRef)IORegistryEntryCreateCFProperty(ioRegistryRoot, CFSTR(kIOPlatformUUIDKey), kCFAllocatorDefault, 0);
    IOObjectRelease(ioRegistryRoot);
    CFStringGetCString(uuidCf, ret, sizeof(ret), kCFStringEncodingMacRoman);
    CFRelease(uuidCf);
    return ret;
}

#endif
