// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: shortcut.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __SHORTCUT_H
#define __SHORTCUT_H

extern "C" {
#include <X11/Xlib.h>
}

#include <string>
#include "wmi.h"

/**
 * Represents a shortcut. If this shortcut has a postdecessor the next
 * pointer may point to it.
 */
class Shortcut {

public:

    Shortcut(unsigned long modMask, KeyCode keyCode, Shortcut *next,
             unsigned int button = 0);
    ~Shortcut();

    KeyCode keyCode();
    void setKeyCode(KeyCode keyCode);

    unsigned long modMask() const;
    void setModMask(unsigned long modMask);

    unsigned int button() const;

    Shortcut *next() const;

    void setNext(Shortcut *next);

    static Shortcut *shortcut(string keys);

private:

    Shortcut *next_;
    KeyCode keyCode_;
    unsigned long modMask_;
    unsigned int button_;
};

#endif // __SHORTCUT_H
