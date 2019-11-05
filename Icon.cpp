// $Id: Icon.cpp 206 2009-03-24 15:22:10Z felfert $
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
#include "config.h"
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/filesys.h>

#ifdef APP_OPENNX
# include "opennxApp.h"
#endif
#ifdef APP_TRACELOG
# include "tracelogApp.h"
#endif
#ifdef APP_WATCHUSBIP
# include "watchUsbIpApp.h"
#endif

static const char * broken_xpm[] = {
"14 16 17 1",
" 	c None",
".	c #000000",
"+	c #800000",
"@	c #008000",
"#	c #808000",
"$	c #000080",
"%	c #800080",
"&	c #008080",
"*	c #C0C0C0",
"=	c #808080",
"-	c #FF0000",
";	c #00FF00",
">	c #FFFF00",
",	c #0000FF",
"'	c #FF00FF",
")	c #00FFFF",
"!	c #FFFFFF",
"..........=   ",
".!!!!!!!!!==  ",
".!********=!= ",
".!***@@***=!!=",
".!**@;@.**....",
".!**@@@.****!.",
".!***..*****!.",
".!******,,,.! ",
".!*-****,),.! ",
".!*'-***,,$   ",
".!*''-**..  !.",
".!*'''-*   *!.",
".!*...     *!.",
".!**     ***!.",
".!!     !!!!!.",
"..      ......"
};

wxBitmap CreateBitmapFromFile(const wxString &filename, int width, int height)
{
    wxFileName fn(filename);
    wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(fn.GetExt());

    wxBitmap bm(broken_xpm);
    if (ft) {
        wxString mt;
        if (ft->GetMimeType(&mt)) {
            wxBitmapType t = wxBITMAP_TYPE_INVALID;

            if (mt == _T("image/png"))
                t = wxBITMAP_TYPE_PNG;
            if (mt == _T("image/bmp"))
                t = wxBITMAP_TYPE_BMP;
            if (mt == _T("image/gif"))
                t = wxBITMAP_TYPE_GIF;
            if (mt == _T("image/x-bitmap"))
                t = wxBITMAP_TYPE_XBM;
            if (mt == _T("image/jpeg"))
                t = wxBITMAP_TYPE_JPEG;
            if (mt == _T("image/tiff"))
                t = wxBITMAP_TYPE_TIF;
            if (mt == _T("image/pcx"))
                t = wxBITMAP_TYPE_PCX;
            if (mt == _T("image/pnm"))
                t = wxBITMAP_TYPE_PNM;

            if (t != wxBITMAP_TYPE_INVALID) {
                wxBitmap rbm;
                wxFileSystem fs;
                wxFSFile *f = fs.OpenFile(::wxGetApp().GetResourcePrefix() + filename);
                if (f) {
                    wxInputStream *s = f->GetStream();
                    if (s && s->IsOk()) {
                        wxImage wi(*s, t);
                        rbm = wxBitmap(wi);
                    }
                    delete f;
                }
                if (rbm.Ok())
                    bm = rbm;
            }
        }
        delete ft;
    }
    if (width == -1)
        width = bm.GetWidth();
    if (height == -1)
        height = bm.GetHeight();
    if ((width != bm.GetWidth()) || (height != bm.GetHeight()))
        bm = wxBitmap(bm.ConvertToImage().Scale(width, height));
    return bm;
}

wxIcon CreateIconFromFile(const wxString &filename, int width, int height)
{
    wxIcon icon = wxNullIcon;
    wxBitmap bm = CreateBitmapFromFile(filename, width, height);
    if (bm.Ok())
      icon.CopyFromBitmap(bm);
    return icon;
}
