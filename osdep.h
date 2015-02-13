/* $Id: osdep.h 591 2010-10-24 23:58:47Z felfert $
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

#ifndef _OSDEP_H_
#define _OSDEP_H_

extern "C" {
    extern const char *x11_socket_path;
    extern const char *x11_keyboard_type;
#ifndef __WXMSW__
    extern void reparent_pulldown(long);
    extern void close_foreign(long);
    extern void close_sid(const char *);
#endif
    extern int inKdeSession;
#ifdef __WXMSW__
    extern long getppid();
    extern int checkMultiMonitors();
    extern int CreateDetachedProcess(const char *cmdline);
    extern int GetDetachedPID();
    extern int getXmingPort(int firstfree);
#endif
#ifdef __WXMAC__
    extern unsigned short macFirstFreePort(unsigned short startPort);
#endif
};
#endif
