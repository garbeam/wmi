// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __FLOAT_H
#define __FLOAT_H

#include "wmi.h"

class Rectangle;

/**
 * Provides base algorithms for moving and resizing rectangles
 * in a floating manner.
 */
class Float {

public:

    /** Used for resizing. */
    static void resize(Rectangle *rect, Direction dir,
                       int stepW, int stepH);

    static void move(Rectangle *rect, int stepW, int stepH);

    static void center(Rectangle *rect, Rectangle *parentRect);

};
#endif // __FLOAT_H
