// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __XFTFONT_H
#define __XFTFONT_H

#include "wmi.h"

#ifdef XFT_SUPPORT

extern "C" {
#include <X11/Xft/Xft.h>
}

#include <string>

#include "font.h"

class Monitor;

/** Base class for XftFonts. */
class WXftFont : public WFont {

public:

    WXftFont(Monitor *monitor, XftFont *font);
    virtual ~WXftFont();

    virtual void drawText(Window window, GC gc, int x, int y, string text);

    virtual int textWidth(string text);

    virtual unsigned int height();

    virtual void setHeight(unsigned int height);

    virtual int ascent();

    virtual int descent();

    static WXftFont *load(Monitor *monitor, string name);

private:

    XftFont *font_;
    unsigned int height_;

};

#endif // XFT_SUPPORT

#endif // __XFTFONT_H
