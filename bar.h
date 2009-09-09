// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __BAR_H
#define __BAR_H

extern "C" {
#include <X11/Xlib.h>
}

#include "widget.h"

class Label;
class Monitor;
class Theme;

/** Base class for status and client bars. */
class Bar : public Widget {

public:

    Bar(Monitor *monitor, Rectangle *rect);
    virtual ~Bar();

    virtual void illuminate() = 0;

    virtual void handleButtonPress(XButtonEvent *event) = 0;
    virtual void handleButtonRelease(XButtonEvent *event) = 0;
    virtual void handleMotionNotify(XMotionEvent *event) = 0;

protected:

    Label *label_;
    Theme *theme_;

    /** Draws border. */
    void drawBorder();
    bool isButtonVisible_;

private:

    Rectangle borderRect_;
};

#endif // __BAR_H
