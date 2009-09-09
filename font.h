// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: font.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __FONT_H
#define __FONT_H

extern "C" {
#include <X11/Xlib.h>
}

#include <string>

#include "wmi.h"

class Monitor;

/** Base class for fonts (currently default XFonts and XftFonts). */
class WFont {

public:

    enum Type {NORMAL, XFT};

    WFont(Monitor *monitor);
    virtual ~WFont();

    Type type() const;

    static WFont *load(Monitor *monitor, string name);

    virtual void drawText(Window window, GC gc, int x, int y, string text) = 0;

    virtual int textWidth(string text) = 0;

    virtual unsigned int height() = 0;

    virtual void setHeight(unsigned int height) = 0;

    virtual int ascent() = 0;

    virtual int descent() = 0;

protected:

    Monitor *monitor_;
    Type type_;

};

#endif // __FONT_H
