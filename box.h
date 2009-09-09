// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __BOX_H
#define __BOX_H

extern "C" {
#include <X11/Xlib.h>
}

#include <string>

#include "widget.h"
#include "wmi.h"

class Label;
class Monitor;
class Theme;

/** Basic info box. */
class Box : public Widget {

public:

    Box(Monitor *monitor, Rectangle *rect, string text);
    ~Box();

    void setText(string text);
    void illuminate();

private:

    Label *label_;
    Theme *theme_;
};

#endif // __BOX_H
