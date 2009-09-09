// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __CURSORS_H
#define __CURSORS_H

extern "C" {
#include <X11/Xlib.h>
}

/**
 * Container class of all X cursors used by WMI.
 */
class Cursors {

public:

    static void initCursors();

    static Cursor NORMAL_CURSOR;
    static Cursor RESIZE_LEFT_CURSOR;
    static Cursor RESIZE_RIGHT_CURSOR;
    static Cursor RESIZE_UP_CURSOR;
    static Cursor RESIZE_DOWN_CURSOR;

    static Cursor RESIZE_NORTH_WEST_CURSOR;
    static Cursor RESIZE_NORTH_EAST_CURSOR;
    static Cursor RESIZE_SOUTH_EAST_CURSOR;
    static Cursor RESIZE_SOUTH_WEST_CURSOR;

    static void cleanup();

private:

    static bool initialized_;
};

#endif // __CURSORS_H
