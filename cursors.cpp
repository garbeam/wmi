// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include <X11/cursorfont.h>
}

#include "cursors.h"

#include "kernel.h"
#include "wmi.h"
#include "xcore.h"

Cursor Cursors::NORMAL_CURSOR = 0;
Cursor Cursors::RESIZE_LEFT_CURSOR = 0;
Cursor Cursors::RESIZE_RIGHT_CURSOR = 0;
Cursor Cursors::RESIZE_UP_CURSOR = 0;
Cursor Cursors::RESIZE_DOWN_CURSOR = 0;

Cursor Cursors::RESIZE_NORTH_WEST_CURSOR = 0;
Cursor Cursors::RESIZE_NORTH_EAST_CURSOR = 0;
Cursor Cursors::RESIZE_SOUTH_EAST_CURSOR = 0;
Cursor Cursors::RESIZE_SOUTH_WEST_CURSOR = 0;

bool Cursors::initialized_ = false;

void Cursors::cleanup() {

    XCORE->freeCursor(NORMAL_CURSOR);
    XCORE->freeCursor(RESIZE_LEFT_CURSOR);
    XCORE->freeCursor(RESIZE_RIGHT_CURSOR);
    XCORE->freeCursor(RESIZE_UP_CURSOR);
    XCORE->freeCursor(RESIZE_DOWN_CURSOR);

    XCORE->freeCursor(RESIZE_NORTH_WEST_CURSOR);
    XCORE->freeCursor(RESIZE_NORTH_EAST_CURSOR);
    XCORE->freeCursor(RESIZE_SOUTH_WEST_CURSOR);
    XCORE->freeCursor(RESIZE_SOUTH_EAST_CURSOR);
}

void Cursors::initCursors()
{
    if (initialized_) {
        return;
    }

    NORMAL_CURSOR = XCORE->createFontCursor(XC_left_ptr);
    RESIZE_LEFT_CURSOR = XCORE->createFontCursor(XC_left_side);
    RESIZE_RIGHT_CURSOR = XCORE->createFontCursor(XC_right_side);
    RESIZE_UP_CURSOR = XCORE->createFontCursor(XC_top_side);
    RESIZE_DOWN_CURSOR = XCORE->createFontCursor(XC_bottom_side);

    RESIZE_NORTH_WEST_CURSOR = XCORE->createFontCursor(XC_top_left_corner);
    RESIZE_NORTH_EAST_CURSOR = XCORE->createFontCursor(XC_top_right_corner);
    RESIZE_SOUTH_WEST_CURSOR = XCORE->createFontCursor(XC_bottom_left_corner);
    RESIZE_SOUTH_EAST_CURSOR = XCORE->createFontCursor(XC_bottom_right_corner);

    initialized_ = true;
}
