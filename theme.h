// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: theme.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __THEME_H
#define __THEME_H

extern "C" {
#include <X11/Xlib.h>
}

#include <map>
#include <string>

#include "wmi.h"

typedef map<string, string, less<string> > MSettings;

/**
 * Represents a theme of WMI.
 */
class Theme
{

public:

    Theme();
    ~Theme();

    void initTheme(MSettings *settings, Colormap cmap);

    unsigned long BAR_BACKGROUND;
    unsigned long BAR_TEXT;
    unsigned long BAR_SHINE;
    unsigned long BAR_SHADOW;

    unsigned long METER_BACKGROUND;
    unsigned long METER_FIGURE_HIGH;
    unsigned long METER_FIGURE_NORMAL;
    unsigned long METER_FIGURE_LOW;
    unsigned long METER_BORDER_SHINE;
    unsigned long METER_BORDER_SHADOW;

    unsigned long FOCUSREQ_BACKGROUND;
    unsigned long FOCUSREQ_TEXT;
    unsigned long FOCUSREQ_SHINE;
    unsigned long FOCUSREQ_SHADOW;

    unsigned long TILED_SHINE_FOCUSSED;
    unsigned long TILED_SHADOW_FOCUSSED;
    unsigned long TILED_SHINE_NORMAL;
    unsigned long TILED_SHADOW_NORMAL;

    unsigned long BUTTON_BACKGROUND_NORMAL;
    unsigned long BUTTON_BACKGROUND_PRESSED;
    unsigned long BUTTON_SHINE_BORDER_NORMAL;
    unsigned long BUTTON_SHINE_BORDER_PRESSED;
    unsigned long BUTTON_SHADOW_BORDER_NORMAL;
    unsigned long BUTTON_SHADOW_BORDER_PRESSED;
    unsigned long BUTTON_SHINE_FIGURE_NORMAL;
    unsigned long BUTTON_SHINE_FIGURE_PRESSED;
    unsigned long BUTTON_SHADOW_FIGURE_NORMAL;
    unsigned long BUTTON_SHADOW_FIGURE_PRESSED;

    unsigned long FRAME_BACKGROUND_NORMAL;
    unsigned long FRAME_BACKGROUND_FOCUSSED;
    unsigned long FRAME_SHINE_NORMAL;
    unsigned long FRAME_SHINE_FOCUSSED;
    unsigned long FRAME_SHADOW_NORMAL;
    unsigned long FRAME_SHADOW_FOCUSSED;
    unsigned long FRAME_PSEUDO;

    unsigned long LABEL_BACKGROUND_NORMAL;
    unsigned long LABEL_BACKGROUND_FOCUSSED;
    unsigned long LABEL_TEXT_NORMAL;
    unsigned long LABEL_TEXT_FOCUSSED;
    unsigned long LABEL_SHINE_NORMAL;
    unsigned long LABEL_SHINE_FOCUSSED;
    unsigned long LABEL_SHADOW_NORMAL;
    unsigned long LABEL_SHADOW_FOCUSSED;

    unsigned long TAB_BACKGROUND_ACTIVE_NORMAL;
    unsigned long TAB_BACKGROUND_INACTIVE_NORMAL;
    unsigned long TAB_BACKGROUND_ACTIVE_FOCUSSED;
    unsigned long TAB_BACKGROUND_INACTIVE_FOCUSSED;
    unsigned long TAB_TEXT_ACTIVE_NORMAL;
    unsigned long TAB_TEXT_INACTIVE_NORMAL;
    unsigned long TAB_TEXT_ACTIVE_FOCUSSED;
    unsigned long TAB_TEXT_INACTIVE_FOCUSSED;
    unsigned long TAB_SHINE_ACTIVE_NORMAL;
    unsigned long TAB_SHINE_INACTIVE_NORMAL;
    unsigned long TAB_SHINE_ACTIVE_FOCUSSED;
    unsigned long TAB_SHINE_INACTIVE_FOCUSSED;
    unsigned long TAB_SHADOW_ACTIVE_NORMAL;
    unsigned long TAB_SHADOW_INACTIVE_NORMAL;
    unsigned long TAB_SHADOW_ACTIVE_FOCUSSED;
    unsigned long TAB_SHADOW_INACTIVE_FOCUSSED;

private:
    unsigned long allocateColor(string name, Colormap cmap);

};

#endif // __THEME_H
