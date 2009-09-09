// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __DRAW_H
#define __DRAW_H

extern "C" {
#include <X11/Xlib.h>
}

class Light;
class Rectangle;

/**
 * Main class of all low-level drawing methods used by WMI.
 */
class Draw {

public:

    static void drawMenuButton(Window window, GC gc, Rectangle *rect,
                         unsigned long background, unsigned long shineBorder,
                         unsigned long shadowBorder, unsigned long shineFigure,
                         unsigned long shadowFigure);

    static void drawDetachButton(Window window, GC gc, Rectangle *rect,
                         unsigned long background,
                         unsigned long shineBorder, unsigned long shadowBorder,
                         unsigned long shineFigure, unsigned long shadowFigure);

    static void drawMaxButton(Window window, GC gc, Rectangle *rect,
                         unsigned long background,
                         unsigned long shineBorder, unsigned long shadowBorder,
                         unsigned long shineFigure, unsigned long shadowFigure,
                         bool fill = false);

    static void drawFloatButton(Window window, GC gc, Rectangle *rect,
                         unsigned long background,
                         unsigned long shineBorder, unsigned long shadowBorder,
                         unsigned long shineFigure, unsigned long shadowFigure,
                         bool fill = false);

    static void drawCloseButton(Window window, GC gc, Rectangle *rect,
                         unsigned long background,
                         unsigned long shineBorder, unsigned long shadowBorder,
                         unsigned long shineFigure, unsigned long shadowFigure);

    static void drawInputModeButton(Window window, GC gc, Rectangle *rect,
                         unsigned long background,
                         unsigned long shineBorder, unsigned long shadowBorder,
                         unsigned long shineFigure, unsigned long shadowFigure);

    static void drawMeter(Window window, GC gc, Rectangle *rect,
                          unsigned int percentage, unsigned long background,
                          unsigned long highFigure, unsigned long normalFigure,
                          unsigned long lowFigure, unsigned long shineBorder,
                          unsigned long shadowBorder);

    static void drawBorder(Window window, GC commonGC, GC borderGC, Rectangle *rect,
                         unsigned long background, unsigned long shine,
                         unsigned long shadow, unsigned int titleBarHeight,
                         unsigned int borderWidth);

    static void drawFloatBorderAnchors(Window window, GC gc, Rectangle *rect,
                         unsigned long shine, unsigned long shadow,
                         unsigned int titleBarHeight, unsigned int borderWidth);

    static void drawShineBorder(Window window, GC gc, Rectangle *rect,
                         unsigned long shine);

    static void drawShadowBorder(Window window, GC gc, Rectangle *rect,
                         unsigned long shadow);

    static void drawRectBorder(Window window, GC gc, Rectangle *rect,
                         unsigned long shine, unsigned long shadow);

    static void drawStickyNotifier(Window window, GC gc, Rectangle *rect,
                         unsigned long shine, unsigned long shadow,
                         unsigned int textWidth);

    static void drawTransRectangle(Window window, GC gc,
                          Rectangle *rect, unsigned int barHeight,
                          unsigned int borderWidth);
};

#endif // __DRAW_H
