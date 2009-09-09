// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: widget.h 734 2004-09-27 18:15:45Z garbeam $

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
