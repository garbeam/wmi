// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: theme.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "theme.h"

#include "logger.h"
#include "monitor.h"
#include "xcore.h"

Theme::Theme() {
}

Theme::~Theme() {
}

unsigned long Theme::allocateColor(string name, Colormap cmap) {

    XColor color;

    if (!XCORE->allocNamedColor(cmap, name, &color))
    {
        Logger::instance()->error("cannot allocate color '"
                                   + name + "'", true);
        // exits WMI
    }

    return color.pixel;
}

void Theme::initTheme(MSettings *ts, Colormap cmap) {

    BAR_BACKGROUND = allocateColor(Util::get(ts, "color.bar.background"), cmap);
    BAR_TEXT = allocateColor(Util::get(ts, "color.bar.text"), cmap);
    BAR_SHINE = allocateColor(Util::get(ts, "color.bar.shine"), cmap);
    BAR_SHADOW = allocateColor(Util::get(ts, "color.bar.shadow"), cmap);

    METER_BACKGROUND = allocateColor(Util::get(ts, "color.meter.background"), cmap);
    METER_FIGURE_HIGH = allocateColor(Util::get(ts, "color.meter.figure.high"), cmap);
    METER_FIGURE_NORMAL = allocateColor(Util::get(ts, "color.meter.figure.normal"), cmap);
    METER_FIGURE_LOW = allocateColor(Util::get(ts, "color.meter.figure.low"), cmap);
    METER_BORDER_SHINE= allocateColor(Util::get(ts, "color.meter.border.shine"), cmap);
    METER_BORDER_SHADOW= allocateColor(Util::get(ts, "color.meter.border.shadow"), cmap);

    FOCUSREQ_BACKGROUND = allocateColor(Util::get(ts, "color.focusreq.background"), cmap);
    FOCUSREQ_TEXT = allocateColor(Util::get(ts, "color.focusreq.text"), cmap);
    FOCUSREQ_SHINE = allocateColor(Util::get(ts, "color.focusreq.shine"), cmap);
    FOCUSREQ_SHADOW = allocateColor(Util::get(ts, "color.focusreq.shadow"), cmap);

    TILED_SHINE_FOCUSSED = allocateColor(Util::get(ts, "color.tiled.shine.focussed"), cmap);
    TILED_SHADOW_FOCUSSED = allocateColor(Util::get(ts, "color.tiled.shadow.focussed"), cmap);
    TILED_SHINE_NORMAL = allocateColor(Util::get(ts, "color.tiled.shine.normal"), cmap);
    TILED_SHADOW_NORMAL = allocateColor(Util::get(ts, "color.tiled.shadow.normal"), cmap);

    BUTTON_BACKGROUND_NORMAL =
        allocateColor(Util::get(ts, "color.button.background.normal"), cmap);
    BUTTON_BACKGROUND_PRESSED =
        allocateColor(Util::get(ts, "color.button.background.pressed"), cmap);
    BUTTON_SHINE_BORDER_NORMAL =
        allocateColor(Util::get(ts, "color.button.border.shine.normal"), cmap);
    BUTTON_SHINE_BORDER_PRESSED =
        allocateColor(Util::get(ts, "color.button.border.shine.pressed"), cmap);
    BUTTON_SHADOW_BORDER_NORMAL =
        allocateColor(Util::get(ts, "color.button.border.shadow.normal"), cmap);
    BUTTON_SHADOW_BORDER_PRESSED =
        allocateColor(Util::get(ts, "color.button.border.shadow.pressed"), cmap);
    BUTTON_SHINE_FIGURE_NORMAL =
        allocateColor(Util::get(ts, "color.button.figure.shine.normal"), cmap);
    BUTTON_SHINE_FIGURE_PRESSED =
        allocateColor(Util::get(ts, "color.button.figure.shine.pressed"), cmap);
    BUTTON_SHADOW_FIGURE_NORMAL =
        allocateColor(Util::get(ts, "color.button.figure.shadow.normal"), cmap);
    BUTTON_SHADOW_FIGURE_PRESSED =
        allocateColor(Util::get(ts, "color.button.figure.shadow.pressed"), cmap);

    FRAME_BACKGROUND_NORMAL =
        allocateColor(Util::get(ts, "color.frame.background.normal"), cmap);
    FRAME_BACKGROUND_FOCUSSED =
        allocateColor(Util::get(ts, "color.frame.background.focussed"), cmap);
    FRAME_SHINE_NORMAL =
        allocateColor(Util::get(ts, "color.frame.shine.normal"), cmap);
    FRAME_SHINE_FOCUSSED =
        allocateColor(Util::get(ts, "color.frame.shine.focussed"), cmap);
    FRAME_SHADOW_NORMAL =
        allocateColor(Util::get(ts, "color.frame.shadow.normal"), cmap);
    FRAME_SHADOW_FOCUSSED =
        allocateColor(Util::get(ts, "color.frame.shadow.focussed"), cmap);
    FRAME_PSEUDO =
        allocateColor(Util::get(ts, "color.frame.pseudo"), cmap);

    LABEL_BACKGROUND_NORMAL =
        allocateColor(Util::get(ts, "color.label.background.normal"), cmap);
    LABEL_BACKGROUND_FOCUSSED =
        allocateColor(Util::get(ts, "color.label.background.focussed"), cmap);
    LABEL_TEXT_NORMAL =
        allocateColor(Util::get(ts, "color.label.text.normal"), cmap);
    LABEL_TEXT_FOCUSSED =
        allocateColor(Util::get(ts, "color.label.text.focussed"), cmap);
    LABEL_SHINE_NORMAL =
        allocateColor(Util::get(ts, "color.label.shine.normal"), cmap);
    LABEL_SHINE_FOCUSSED =
        allocateColor(Util::get(ts, "color.label.shine.focussed"), cmap);
    LABEL_SHADOW_NORMAL =
        allocateColor(Util::get(ts, "color.label.shadow.normal"), cmap);
    LABEL_SHADOW_FOCUSSED =
        allocateColor(Util::get(ts, "color.label.shadow.focussed"), cmap);

    TAB_BACKGROUND_ACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.background.active.normal"), cmap);
    TAB_BACKGROUND_INACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.background.inactive.normal"), cmap);
    TAB_BACKGROUND_ACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.background.active.focussed"), cmap);
    TAB_BACKGROUND_INACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.background.inactive.focussed"), cmap);
    TAB_TEXT_ACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.text.active.normal"), cmap);
    TAB_TEXT_INACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.text.inactive.normal"), cmap);
    TAB_TEXT_ACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.text.active.focussed"), cmap);
    TAB_TEXT_INACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.text.inactive.focussed"), cmap);
    TAB_SHINE_ACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.shine.active.normal"), cmap);
    TAB_SHINE_INACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.shine.inactive.normal"), cmap);
    TAB_SHINE_ACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.shine.active.focussed"), cmap);
    TAB_SHINE_INACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.shine.inactive.focussed"), cmap);
    TAB_SHADOW_ACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.shadow.active.normal"), cmap);
    TAB_SHADOW_INACTIVE_NORMAL =
        allocateColor(Util::get(ts, "color.tab.shadow.inactive.normal"), cmap);
    TAB_SHADOW_ACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.shadow.active.focussed"), cmap);
    TAB_SHADOW_INACTIVE_FOCUSSED =
        allocateColor(Util::get(ts, "color.tab.shadow.inactive.focussed"), cmap);
}
