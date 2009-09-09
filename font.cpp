// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "font.h"

#include "xcore.h"
#include "xfont.h"
#include "xftfont.h"

WFont::WFont(Monitor *monitor) {
    monitor_ = monitor;
}

WFont::~WFont() {
}

WFont *WFont::load(Monitor *monitor, string name) {

    WFont *font = XFont::load(monitor, name);
#ifdef XFT_SUPPORT
    if (!font) {
        // fallback if XFT_SUPPORT enabled
        font = WXftFont::load(monitor, name);
    }
#endif
    return font;
}


WFont::Type WFont::type() const{
    return type_;
}
