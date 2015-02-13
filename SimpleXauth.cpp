// $Id: SimpleXauth.cpp 501 2010-03-01 08:10:39Z felfert $
//
// Copyright (C) 2010 The OpenNX Team
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
#include "config.h"
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "SimpleXauth.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

# if defined(__WXMSW__) || defined(APP_TESTXAUTH)

#include "SimpleXauth.h"
#include "pwcrypt.h"
#include <wx/socket.h>
#include <wx/tokenzr.h>
#include <wx/hashset.h>
#include <wx/wfstream.h>

# define XauthFamilyInternet              0     /* IPv4 */
# define XauthFamilyDECnet                1
# define XauthFamilyChaos                 2
# define XauthFamilyInternet6             6     /* IPv6 */
# define XauthFamilyServerInterpreted     5
# define XauthFamilyLocal               256     /* not part of X standard (i.e. X.h) */
# define XauthFamilyWild              65535
# define XauthFamilyNetname             254     /* not part of X standard */
# define XauthFamilyKrb5Principal       253     /* Kerberos 5 principal name */
# define XauthFamilyLocalHost           252     /* for local non-net authentication */

typedef struct xauth {
    unsigned short   family;
    unsigned short   address_length;
    char            *address;
    unsigned short   number_length;
    char            *number;
    unsigned short   name_length;
    char            *name;
    unsigned short   data_length;
    char            *data;
} Xauth;

class SimpleXauthEntry
{
    public:
        SimpleXauthEntry();
        SimpleXauthEntry(wxString cookie);
        SimpleXauthEntry(const SimpleXauthEntry &);
        ~SimpleXauthEntry();
        bool Read(wxInputStream &);
        bool Write(wxOutputStream &);
        void SetUnix(wxString name, int display);
        void SetInternet(wxIPV4address addr, int display);
        wxString GetKey() const;
        wxString Dump() const;
    private:
        Xauth au;
        wxIPV4address iaddr;
        wxString naddr;
        wxString dpystr;
        void init();
        bool readShort(wxInputStream &, unsigned short &);
        bool readString(wxInputStream &, unsigned short &, char *&);
        bool writeShort(wxOutputStream &, unsigned short);
        bool writeString(wxOutputStream &, unsigned short, char *);

        friend class SimpleXauthEntryHash;
        friend class SimpleXauthEntryEqual;
};

SimpleXauthEntry::SimpleXauthEntry()
{
    init();
}

SimpleXauthEntry::SimpleXauthEntry(wxString cookie)
{
    init();
    unsigned long l;
    unsigned int idx = 0;
    for (idx = 0; idx < cookie.Length(); idx += 2) {
        if (!cookie.Mid(idx, 2).ToULong(&l, 16))
            break;
        au.data_length++;
    }
    au.data = new char[au.data_length];
    for (idx = 0; idx < cookie.Length(); idx += 2) {
        if (!cookie.Mid(idx, 2).ToULong(&l, 16))
            break;
        au.data[idx / 2] = l;
    }
}

SimpleXauthEntry::SimpleXauthEntry(const SimpleXauthEntry &other)
{
    memcpy(&au, &other.au, sizeof(au));
    if (au.address_length && au.address) {
        au.address = new char [au.address_length];
        if (au.address)
            memcpy(au.address, other.au.address, au.address_length);
    }
    if (au.number_length && au.number) {
        au.number = new char [au.number_length];
        if (au.number)
            memcpy(au.number, other.au.number, au.number_length);
    }
    if (au.name_length && au.name) {
        au.name = new char [au.name_length];
        if (au.name)
            memcpy(au.name, other.au.name, au.name_length);
    }
    if (au.data_length && au.data) {
        au.data = new char [au.data_length];
        if (au.data)
            memcpy(au.data, other.au.data, au.data_length);
    }
    dpystr = other.dpystr;
    naddr = other.naddr;
    iaddr = other.iaddr;
}

SimpleXauthEntry::~SimpleXauthEntry()
{
    if (au.address)
        delete au.address;
    if (au.number)
        delete au.number;
    if (au.name)
        delete au.name;
    if (au.data)
        delete au.data;
}

void SimpleXauthEntry::init()
{
    memset(&au, 0, sizeof(au));
    // set our defaults
    au.name = strdup("MIT-MAGIC-COOKIE-1");
    au.name_length = 18;
}

wxString SimpleXauthEntry::Dump() const
{
    wxString tmp;
    tmp << wxT("family: ") << au.family << wxT(", addr: ");
    if (au.family == XauthFamilyInternet)
        tmp << iaddr.IPAddress();
    else
        tmp << naddr;
    tmp << wxT(", dpy: ") << dpystr;
    return tmp;
}

wxString SimpleXauthEntry::GetKey() const
{
    wxString key;
    key << au.family << wxT(":");
    switch (au.family) {
        case XauthFamilyInternet:
            key << iaddr.IPAddress();
            break;
        default:
            key << naddr.Upper();
            break;
    }
    key << wxT(":") << dpystr;
    return key;
}

bool SimpleXauthEntry::readShort(wxInputStream &s, unsigned short &val)
{
    unsigned char buf[2];
    if (s.Read(&buf, sizeof(buf)).LastRead() == sizeof(buf)) {
        val = buf[0] * 256 + buf[1];
        return true;
    }
    return false;
}

bool SimpleXauthEntry::readString(wxInputStream &s, unsigned short &len, char * &buf)
{
    if (readShort(s, len)) {
        if (buf) {
            delete buf;
            buf = NULL;
        }
        if (len > 0) {
            buf = new char[len];
            if (!buf)
                return false;
            return (s.Read(buf, len).LastRead() == len);
        } else
            return true;
    }
    return false;
}

bool SimpleXauthEntry::Read(wxInputStream &s)
{
    char *tmp;
    if (!readShort(s, au.family))
        return false;
    if (!readString(s, au.address_length, au.address))
        return false;
    if (!readString(s, au.number_length, au.number))
        return false;
    if (!readString(s, au.name_length, au.name))
        return false;
    if (!readString(s, au.data_length, au.data))
        return false;
    switch (au.family) {
        case XauthFamilyLocal:
            if (au.address_length > 0) {
                tmp = new char[au.address_length + 1];
                if (tmp) {
                    memcpy(tmp, au.address, au.address_length);
                    tmp[au.address_length] = '\0';
                    naddr = wxString(wxConvUTF8.cMB2WC(tmp));
                    delete tmp;
                }
            }
            break;
        case XauthFamilyInternet:
            if (au.address_length == 4) {
                iaddr.Hostname(wxString::Format(wxT("%d.%d.%d.%d"),
                            au.address[0] & 255, au.address[1] & 255,
                            au.address[2] & 255, au.address[3] & 255));
            }
            break;
        default:
            naddr = wxEmptyString;
            for (int i = 0; i < au.address_length; i++) {
                int ia = au.address[i];
                naddr.Append(wxString::Format(wxT("%02x"), ia));
            }
            break;
    }
    if (au.number_length > 0) {
        tmp = new char[au.number_length + 1];
        if (tmp) {
            memcpy(tmp, au.number, au.number_length);
            tmp[au.number_length] = '\0';
            dpystr = wxString(wxConvUTF8.cMB2WC(tmp));
            delete tmp;
        }
    }
    return true;
}

bool SimpleXauthEntry::writeShort(wxOutputStream &s, unsigned short val)
{
    unsigned char buf[2];

    buf[0] = (val & (unsigned)0xff00) >> 8;
    buf[1] = val & 0xff;
    return (s.Write(&buf, sizeof(buf)).LastWrite() == sizeof(buf));
}

bool SimpleXauthEntry::writeString(wxOutputStream &s, unsigned short len, char *buf)
{
    if (!writeShort(s, len))
        return false;
    if (len > 0)
        return (s.Write(buf, len).LastWrite() == len);
    return true;
}

bool SimpleXauthEntry::Write(wxOutputStream &s)
{
    if (!writeShort(s, au.family))
        return false;
    if (!writeString(s, au.address_length, au.address))
        return false;
    if (!writeString(s, au.number_length, au.number))
        return false;
    if (!writeString(s, au.name_length, au.name))
        return false;
    return writeString(s, au.data_length, au.data);
}

void SimpleXauthEntry::SetUnix(wxString addr, int display)
{
    au.family = XauthFamilyLocal;
    dpystr = wxString::Format(wxT("%d"), display);
    au.number_length = dpystr.Length();
    au.number = strdup((const char *)dpystr.mb_str());
    naddr = addr;
    if (au.address)
        delete au.address;
    au.address_length = naddr.Length();
    au.address = strdup((const char *)naddr.mb_str());
}

void SimpleXauthEntry::SetInternet(wxIPV4address addr, int display)
{
    au.family = XauthFamilyInternet;
    dpystr = wxString::Format(wxT("%d"), display);
    au.number_length = dpystr.Length();
    au.number = strdup((const char *)dpystr.mb_str());
    iaddr = addr;
    au.address_length = 4;
    if (au.address)
        delete au.address;
    au.address = new char[4];
    if (au.address) {
        int i = 0;
        wxStringTokenizer t(iaddr.IPAddress(), wxT("."));
        while (t.HasMoreTokens() && (i < 4)) {
            unsigned long l;
            if (t.GetNextToken().ToULong(&l))
                au.address[i++] = l;
        }
    }
}

class SimpleXauthEntryHash
{
    public:
        SimpleXauthEntryHash() { }

        unsigned long operator()(const SimpleXauthEntry & k) const
        {
            return wxStringHash::wxCharStringHash(k.GetKey());
        }

        SimpleXauthEntryHash& operator=(const SimpleXauthEntryHash&) { return *this; }
};

class SimpleXauthEntryEqual
{
    public:
        SimpleXauthEntryEqual() { }

        bool operator()(const SimpleXauthEntry& a, const SimpleXauthEntry& b) const
        {
            return a.GetKey().IsSameAs(b.GetKey());
        }

        SimpleXauthEntryEqual& operator=(const SimpleXauthEntryEqual&) { return *this; }
};

WX_DECLARE_HASH_SET(
        SimpleXauthEntry, SimpleXauthEntryHash, SimpleXauthEntryEqual, SimpleXauthEntrySet
);

class SimpleXauthPrivate
{
    public:
        SimpleXauthPrivate() {
            cookie = md5sum(::wxGetUserId() + wxDateTime::Now().Format());
        };
        wxString filename;
        wxString cookie;
        SimpleXauthEntrySet entries;
};

SimpleXauth::SimpleXauth(wxString filename)
{
    m_pPrivate = new SimpleXauthPrivate();
    m_pPrivate->filename = filename;
    readFile();
}

SimpleXauth::~SimpleXauth()
{
    writeFile();
    delete m_pPrivate;
}

bool SimpleXauth::readFile()
{
    wxLogNull nolog;
    wxFileInputStream s(m_pPrivate->filename);
    while (s.IsOk()) {
        SimpleXauthEntry e;
        if (!e.Read(s))
            return (m_pPrivate->entries.size() > 0);
        m_pPrivate->entries.insert(e);
    }
    return false;
}

bool SimpleXauth::writeFile()
{
    wxFileOutputStream s(m_pPrivate->filename);
    if (s.IsOk()) {
        SimpleXauthEntrySet::iterator it = m_pPrivate->entries.begin();
        for (; it != m_pPrivate->entries.end(); ++it) {
            if (!it->Write(s))
                return false;
        }
        return true;
    }
    return false;
}

wxString SimpleXauth::GetCookie()
{
    return m_pPrivate->cookie;
}

void SimpleXauth::AddDisplay(int displayNum)
{
    // We always add the following 3 variants (suitable
    // for both Xming and NXWin):
    //   localhost:X
    //   ipaddress:X
    //   localname/unix:X

    wxString host = ::wxGetFullHostName();
    wxIPV4address a;
    a.Hostname(host);
    SimpleXauthEntry e(m_pPrivate->cookie);
    e.SetInternet(a, displayNum);
    m_pPrivate->entries.insert(e);
    a.LocalHost();
    e.SetInternet(a, displayNum);
    m_pPrivate->entries.insert(e);
    e.SetUnix(host.BeforeFirst(wxT('.')), displayNum);
    m_pPrivate->entries.insert(e);
}
#endif
