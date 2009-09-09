// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __WIDGET_H
#define __WIDGET_H

extern "C" {
#include <X11/Xlib.h>
}

#include "rectangle.h"

class Monitor;

/** Base class for widgets, such as menu, bar, slot, thing. */
class Widget : public Rectangle {

public:

    Widget(Monitor *monitor, Rectangle *rect,
           bool initWindowAndGC = true);
    virtual ~Widget();

    Monitor *monitor() const;
    GC gc() const;

    Window window() const;

    virtual void resize();
    virtual void show();
    virtual void hide();

    bool isVisible() const;

protected:
    bool isVisible_;

private:

    void initWindow();
    void initGC();

    GC gc_;
    Monitor *monitor_;
    Window window_;
};

#endif // __WIDGET_H
