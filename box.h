// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: box.h 734 2004-09-27 18:15:45Z garbeam $

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
