// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: font.cpp 734 2004-09-27 18:15:45Z garbeam $

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
