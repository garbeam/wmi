// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "xftfont.h"

#ifdef XFT_SUPPORT

extern "C" {
#include <X11/Xlib.h>
}

#include "monitor.h"
#include "xcore.h"

WXftFont::WXftFont(Monitor *monitor, XftFont *font) :
    WFont(monitor)
{
    font_ = font;
    type_ = WFont::XFT;
}

WXftFont *WXftFont::load(Monitor *monitor, string name) {

    XftFont *font = XCORE->xftOpenFontName(monitor->id(), name);

    WXftFont *result = 0;
    if (font) {
        result = new WXftFont(monitor, font);
        result->setHeight(font->height);
    }
    return result;
}

WXftFont::~WXftFont() {
    XCORE->xftCloseFont(font_);
}

int WXftFont::textWidth(string text) {
    return XCORE->textWidth(font_, text);
}

unsigned int WXftFont::height() {
    return height_;
}

void WXftFont::setHeight(unsigned int height) {
    height_ = height;
}

void WXftFont::drawText(Window window, GC gc, int x, int y, string text) {
    XCORE->drawText(font_, window, gc, monitor_->id(),  x, y, text);
}

int WXftFont::ascent() {
    return font_->ascent;
}

int WXftFont::descent() {
    return font_->descent;
}

#endif
