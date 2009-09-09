// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: xfont.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __XFONT_H
#define __XFONT_H

#include "wmi.h"

extern "C" {
#include <X11/Xlib.h>
}

#include <string>

#include "font.h"

class Monitor;

/** Base class for default XFonts. */
class XFont : public WFont {

public:

    XFont(Monitor *monitor, XFontStruct *font);
    virtual ~XFont();

    virtual void drawText(Window window, GC gc, int x, int y, string text);

    virtual int textWidth(string text);

    virtual unsigned int height();

    virtual void setHeight(unsigned int height);

    XFontStruct *font() const;

    virtual int ascent();

    virtual int descent();

    static XFont *load(Monitor *monitor, string name);

private:

    XFontStruct *font_;
    unsigned int height_;

};

#endif // __XFONT_H
