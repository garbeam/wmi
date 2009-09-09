// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include <iostream>

#include "draw.h"

#include "config.h"
#include "kernel.h"
#include "rectangle.h"
#include "xcore.h"

#define XCORE XCore::instance()
#define FIGURE_DISTANCE 3

void Draw::drawDetachButton(Window window, GC gc,
        Rectangle *rect, unsigned long background,
        unsigned long shineBorder, unsigned long shadowBorder,
        unsigned long shineFigure, unsigned long shadowFigure)
{

    XCORE->setForeground(gc, background);
    XCORE->fillRectangle(window, gc, rect);

    XPoint shineButtonPoints[3];
    shineButtonPoints[0].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[0].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shineButtonPoints[1].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[1].y = rect->y() + rect->height() - 2 * FIGURE_DISTANCE - 1;
    shineButtonPoints[2].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shineButtonPoints[2].y = rect->y() + rect->height() - 2 * FIGURE_DISTANCE - 1;
    XCORE->setForeground(gc, shineFigure);
    XCORE->drawLines(window, gc, shineButtonPoints, 3);

    XPoint shadowButtonPoints[3];
    shadowButtonPoints[0].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[0].y = rect->y() + rect->height() - 2 * FIGURE_DISTANCE - 1;
    shadowButtonPoints[1].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[1].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[2].x = rect->x() + FIGURE_DISTANCE;
    shadowButtonPoints[2].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    XCORE->setForeground(gc, shadowFigure);
    XCORE->drawLines(window, gc, shadowButtonPoints, 3);

    drawRectBorder(window, gc, rect, shineBorder, shadowBorder);
}

void Draw::drawMaxButton(Window window, GC gc,
        Rectangle *rect, unsigned long background,
        unsigned long shineBorder, unsigned long shadowBorder,
        unsigned long shineFigure, unsigned long shadowFigure,
        bool fill)

{
    XCORE->setForeground(gc, background);
    XCORE->fillRectangle(window, gc, rect);

    XPoint shadowButtonPoints[3];
    shadowButtonPoints[0].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[0].y = rect->y() + FIGURE_DISTANCE;
    shadowButtonPoints[1].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[1].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[2].x = rect->x() + FIGURE_DISTANCE;
    shadowButtonPoints[2].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    XCORE->setForeground(gc, shadowFigure);
    XCORE->drawLines(window, gc, shadowButtonPoints, 3);

    XPoint shineButtonPoints[3];
    shineButtonPoints[0].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[0].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shineButtonPoints[1].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[1].y = rect->y() + FIGURE_DISTANCE;
    shineButtonPoints[2].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shineButtonPoints[2].y = rect->y() + FIGURE_DISTANCE;
    XCORE->setForeground(gc, shineFigure);
    XCORE->drawLines(window, gc, shineButtonPoints, 3);

    if (fill) {
        XCORE->setForeground(gc, shineFigure);
        XCORE->fillRectangle(window, gc,
                             rect->x() + FIGURE_DISTANCE + 1,
                             rect->y() + FIGURE_DISTANCE + 1,
                             rect->width() - 2 * FIGURE_DISTANCE - 2,
                             rect->height() - 2 * FIGURE_DISTANCE - 2);
    }

    drawRectBorder(window, gc, rect, shineBorder, shadowBorder);
}

void Draw::drawMenuButton(Window window, GC gc,
        Rectangle *rect, unsigned long background,
        unsigned long shineBorder, unsigned long shadowBorder,
        unsigned long shineFigure, unsigned long shadowFigure)
{
    XCORE->setForeground(gc, background);
    XCORE->fillRectangle(window, gc, rect);

    // shine lines
    XCORE->setForeground(gc, shineFigure);
    XCORE->drawLine(window, gc, rect->x() + FIGURE_DISTANCE,
                                rect->y() + FIGURE_DISTANCE,
                                rect->x() + rect->width() - FIGURE_DISTANCE,
                                rect->y() + FIGURE_DISTANCE);
    XCORE->drawLine(window, gc, rect->x() + FIGURE_DISTANCE,
                                rect->y() + rect->height() / 2,
                                rect->x() + rect->width() - FIGURE_DISTANCE,
                                rect->y() + rect->height() / 2);
    XCORE->drawLine(window, gc, rect->x() + FIGURE_DISTANCE,
                                rect->y() + rect->height() - FIGURE_DISTANCE - 1,
                                rect->x() + rect->width() - FIGURE_DISTANCE,
                                rect->y() + rect->height() - FIGURE_DISTANCE - 1);

    // shadow lines
    XCORE->setForeground(gc, shadowFigure);
    XCORE->drawLine(window, gc, rect->x() + FIGURE_DISTANCE,
                                rect->y() + FIGURE_DISTANCE + 1,
                                rect->x() + rect->width() - FIGURE_DISTANCE,
                                rect->y() + FIGURE_DISTANCE + 1);
    XCORE->drawLine(window, gc, rect->x() + FIGURE_DISTANCE,
                                rect->y() + rect->height() / 2 + 1,
                                rect->x() + rect->width() - FIGURE_DISTANCE,
                                rect->y() + rect->height() / 2 + 1);
    XCORE->drawLine(window, gc, rect->x() + FIGURE_DISTANCE,
                                rect->y() + rect->height() - FIGURE_DISTANCE,
                                rect->x() + rect->width() - FIGURE_DISTANCE,
                                rect->y() + rect->height() - FIGURE_DISTANCE);

    drawRectBorder(window, gc, rect, shineBorder, shadowBorder);
}

void Draw::drawFloatButton(Window window, GC gc,
        Rectangle *rect, unsigned long background,
        unsigned long shineBorder, unsigned long shadowBorder,
        unsigned long shineFigure, unsigned long shadowFigure,
        bool fill)
{
    XCORE->setForeground(gc, background);
    XCORE->fillRectangle(window, gc, rect);

    XPoint shadowButtonPoints[3];
    shadowButtonPoints[0].x = rect->x() + rect->width() - 2 * FIGURE_DISTANCE - 1;
    shadowButtonPoints[0].y = rect->y() + FIGURE_DISTANCE;
    shadowButtonPoints[1].x = rect->x() + rect->width() - 2 * FIGURE_DISTANCE - 1;
    shadowButtonPoints[1].y = rect->y() + rect->height() - 2 * FIGURE_DISTANCE - 1;
    shadowButtonPoints[2].x = rect->x() + FIGURE_DISTANCE;
    shadowButtonPoints[2].y = rect->y() + rect->height() - 2 * FIGURE_DISTANCE - 1;
    XCORE->setForeground(gc, shadowFigure);
    XCORE->drawLines(window, gc, shadowButtonPoints, 3);

    shadowButtonPoints[0].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[0].y = rect->y() + 2 * FIGURE_DISTANCE;
    shadowButtonPoints[1].x = rect->x() + rect->width() -  FIGURE_DISTANCE - 1;
    shadowButtonPoints[1].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[2].x = rect->x() + 2 * FIGURE_DISTANCE;
    shadowButtonPoints[2].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    XCORE->drawLines(window, gc, shadowButtonPoints, 3);

    XPoint shineButtonPoints[3];
    shineButtonPoints[0].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[0].y = rect->y() + rect->height() - 2 * FIGURE_DISTANCE - 1;
    shineButtonPoints[1].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[1].y = rect->y() + FIGURE_DISTANCE;
    shineButtonPoints[2].x = rect->x() + rect->width() - 2 * FIGURE_DISTANCE - 1;
    shineButtonPoints[2].y = rect->y() + FIGURE_DISTANCE;
    XCORE->setForeground(gc, shineFigure);
    XCORE->drawLines(window, gc, shineButtonPoints, 3);

    shineButtonPoints[0].x = rect->x() + 2 * FIGURE_DISTANCE;
    shineButtonPoints[0].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shineButtonPoints[1].x = rect->x() + 2 * FIGURE_DISTANCE;
    shineButtonPoints[1].y = rect->y() + 2 * FIGURE_DISTANCE;
    shineButtonPoints[2].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shineButtonPoints[2].y = rect->y() + 2 * FIGURE_DISTANCE;
    XCORE->drawLines(window, gc, shineButtonPoints, 3);

    if (fill) {
        XCORE->setForeground(gc, shineFigure);
        XCORE->fillRectangle(window, gc,
                             rect->x() + FIGURE_DISTANCE + 1,
                             rect->y() + FIGURE_DISTANCE + 1,
                             rect->width() - 3 * FIGURE_DISTANCE - 2,
                             rect->height() - 3 * FIGURE_DISTANCE - 2);

        XCORE->fillRectangle(window, gc,
                             rect->x() + 2 * FIGURE_DISTANCE + 1,
                             rect->y() + 2 * FIGURE_DISTANCE + 1,
                             rect->width() - 3 * FIGURE_DISTANCE - 2,
                             rect->height() - 3 * FIGURE_DISTANCE - 2);
    }

    drawRectBorder(window, gc, rect, shineBorder, shadowBorder);
}

void Draw::drawCloseButton(Window window, GC gc,
        Rectangle *rect, unsigned long background,
        unsigned long shineBorder, unsigned long shadowBorder,
        unsigned long shineFigure, unsigned long shadowFigure)
{
    XCORE->setForeground(gc, background);
    XCORE->fillRectangle(window, gc, rect);

    int mx = rect->x() + rect->width() / 2;
    int my = rect->y() + rect->height() / 2;
    int md = FIGURE_DISTANCE / 2 + 1;
    XPoint shineButtonPoints[3];
    shineButtonPoints[0].x = mx - md;
    shineButtonPoints[0].y = my + md;
    shineButtonPoints[1].x = shineButtonPoints[0].x;
    shineButtonPoints[1].y = my - md;
    shineButtonPoints[2].x = mx + md;
    shineButtonPoints[2].y = shineButtonPoints[1].y;
    XCORE->setForeground(gc, shineFigure);
    XCORE->drawLines(window, gc, shineButtonPoints, 3);

    XPoint shadowButtonPoints[3];
    shadowButtonPoints[0].x = mx + md;
    shadowButtonPoints[0].y = my - md;
    shadowButtonPoints[1].x = shadowButtonPoints[0].x;
    shadowButtonPoints[1].y = my + md;
    shadowButtonPoints[2].x = mx - md;
    shadowButtonPoints[2].y = my + md;
    XCORE->setForeground(gc, shadowFigure);
    XCORE->drawLines(window, gc, shadowButtonPoints, 3);

    drawRectBorder(window, gc, rect, shineBorder, shadowBorder);
}

void Draw::drawInputModeButton(Window window, GC gc, Rectangle *rect,
        unsigned long background,
        unsigned long shineBorder, unsigned long shadowBorder,
        unsigned long shineFigure, unsigned long shadowFigure)
{
    XCORE->setForeground(gc, background);
    XCORE->fillRectangle(window, gc, rect);

    XPoint shadowButtonPoints[2];
    shadowButtonPoints[0].x = rect->x() + 2 * FIGURE_DISTANCE;
    shadowButtonPoints[0].y = rect->y() + FIGURE_DISTANCE;
    shadowButtonPoints[1].x = rect->x() + 2 * FIGURE_DISTANCE;
    shadowButtonPoints[1].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[2].x = rect->x() + FIGURE_DISTANCE;
    shadowButtonPoints[2].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    XCORE->setForeground(gc, shadowFigure);
    XCORE->drawLines(window, gc, shadowButtonPoints, 3);

    shadowButtonPoints[0].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[0].y = rect->y() + FIGURE_DISTANCE;
    shadowButtonPoints[1].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[1].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shadowButtonPoints[2].x = rect->x() + rect->width() - 2 * FIGURE_DISTANCE - 1;
    shadowButtonPoints[2].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    XCORE->drawLines(window, gc, shadowButtonPoints, 3);

    XPoint shineButtonPoints[3];
    shineButtonPoints[0].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[0].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shineButtonPoints[1].x = rect->x() + FIGURE_DISTANCE;
    shineButtonPoints[1].y = rect->y() + FIGURE_DISTANCE;
    shineButtonPoints[2].x = rect->x() + 2 * FIGURE_DISTANCE;
    shineButtonPoints[2].y = rect->y() + FIGURE_DISTANCE;
    XCORE->setForeground(gc, shineFigure);
    XCORE->drawLines(window, gc, shineButtonPoints, 3);

    shineButtonPoints[0].x = rect->x() + rect->width() - 2 * FIGURE_DISTANCE - 1;
    shineButtonPoints[0].y = rect->y() + rect->height() - FIGURE_DISTANCE - 1;
    shineButtonPoints[1].x = rect->x() + rect->width() - 2 * FIGURE_DISTANCE - 1;
    shineButtonPoints[1].y = rect->y() + FIGURE_DISTANCE;
    shineButtonPoints[2].x = rect->x() + rect->width() - FIGURE_DISTANCE - 1;
    shineButtonPoints[2].y = rect->y() + FIGURE_DISTANCE;
    XCORE->drawLines(window, gc, shineButtonPoints, 3);

    drawRectBorder(window, gc, rect, shineBorder, shadowBorder);
}

void Draw::drawMeter(Window window, GC gc, Rectangle *rect,
                     unsigned int percentage, unsigned long background,
                     unsigned long highFigure, unsigned long normalFigure,
                     unsigned long lowFigure, unsigned long shineBorder,
                     unsigned long shadowBorder)
{
    XCORE->setForeground(gc, background);
    XCORE->fillRectangle(window, gc, rect);

    unsigned int pHeight  = (rect->height() * percentage) / 100;
    unsigned long figure = lowFigure;
    if (percentage > 66) {
        figure = highFigure;
    }
    else if (percentage > 33) {
        figure = normalFigure;
    }
    XCORE->setForeground(gc, figure);
    XCORE->fillRectangle(window, gc,
                         rect->x() + 1, rect->y() + (rect->height() - 1 - pHeight),
                         rect->width() - 2, pHeight);

    drawRectBorder(window, gc, rect, shineBorder, shadowBorder);
}

void Draw::drawBorder(Window window, GC commonGC,
        GC borderGC, Rectangle *rect,
        unsigned long background, unsigned long shine,
        unsigned long shadow, unsigned int titleBarHeight,
        unsigned int borderWidth)
{
    // titlebar 1px separator
    XCORE->setForeground(commonGC, background);
    if (titleBarHeight > 0 && borderWidth > 0) {
        XPoint titleBorderPoints[2];
        titleBorderPoints[0].x = borderWidth;
        titleBorderPoints[0].y = titleBarHeight;
        titleBorderPoints[1].x = rect->width() - borderWidth;
        titleBorderPoints[1].y = titleBarHeight;
        XCORE->drawLines(window, commonGC, titleBorderPoints, 2);
    }

    if (borderWidth == 0) {
        return;
    }

    // background border
    XCORE->setForeground(borderGC, background);
    XCORE->drawRectangle(window, borderGC,
            borderWidth / 2, borderWidth / 2,
            rect->width() - borderWidth,
            rect->height() - borderWidth);

    // shadow border
    XPoint shadowBorderPoints[3];
    shadowBorderPoints[0].x = rect->width() - 1;
    shadowBorderPoints[0].y = 0;
    shadowBorderPoints[1].x = rect->width() - 1;
    shadowBorderPoints[1].y = rect->height() - 1;
    shadowBorderPoints[2].x = 0;
    shadowBorderPoints[2].y = rect->height() - 1;

    XCORE->setForeground(commonGC, shadow);
    XCORE->drawLines(window, commonGC, shadowBorderPoints, 3);

    // shine border
    XPoint shineBorderPoints[3];
    shineBorderPoints[0].x = 0;
    shineBorderPoints[0].y = rect->height() - 1;
    shineBorderPoints[1].x = 0;
    shineBorderPoints[1].y = 0;
    shineBorderPoints[2].x = rect->width() - 1;
    shineBorderPoints[2].y = 0;

    XCORE->setForeground(commonGC, shine);
    XCORE->drawLines(window, commonGC, shineBorderPoints, 3);
}

void Draw::drawFloatBorderAnchors(Window window, GC gc, Rectangle *rect,
                                unsigned long shine, unsigned long shadow,
                                unsigned int titleBarHeight, unsigned int borderWidth)
{
    XCORE->setForeground(gc, shadow);
    XCORE->drawLine(window, gc, titleBarHeight, 0, titleBarHeight, borderWidth);
    XCORE->drawLine(window, gc, 0, titleBarHeight, borderWidth, titleBarHeight);
    XCORE->drawLine(window, gc, 0, rect->height() - titleBarHeight - 1,
                     borderWidth, rect->height() - titleBarHeight);
    XCORE->drawLine(window, gc, titleBarHeight, rect->height() - borderWidth,
                     titleBarHeight, rect->height());
    XCORE->drawLine(window, gc, rect->width() - titleBarHeight - 1, 0,
                     rect->width() - titleBarHeight - 1, borderWidth);
    XCORE->drawLine(window, gc, rect->width() - borderWidth, titleBarHeight,
                     rect->width(), titleBarHeight);
    XCORE->drawLine(window, gc, rect->width() - borderWidth, rect->height()
                     - titleBarHeight - 1, rect->width(), rect->height()
                     - titleBarHeight - 1);
    XCORE->drawLine(window, gc, rect->width() - titleBarHeight - 1,
                     rect->height() - borderWidth,
                     rect->width() - titleBarHeight - 1,
                     rect->height());

    XCORE->setForeground(gc, shine);
    XCORE->drawLine(window, gc, titleBarHeight + 1, 0,
                    titleBarHeight + 1, borderWidth);
    XCORE->drawLine(window, gc, 0, titleBarHeight + 1, borderWidth,
                    titleBarHeight + 1);
    XCORE->drawLine(window, gc, 0, rect->height() - titleBarHeight,
                    borderWidth, rect->height() - titleBarHeight);
    XCORE->drawLine(window, gc, titleBarHeight + 1,
                    rect->height() - borderWidth,
                    titleBarHeight + 1, rect->height());
    XCORE->drawLine(window, gc, rect->width() - titleBarHeight, 0,
                     rect->width() - titleBarHeight, borderWidth);
    XCORE->drawLine(window, gc, rect->width() - borderWidth,
                    titleBarHeight + 1,
                    rect->width(), titleBarHeight + 1);
    XCORE->drawLine(window, gc, rect->width() - borderWidth,
                    rect->height() - titleBarHeight,
                    rect->width(), rect->height() - titleBarHeight);
    XCORE->drawLine(window, gc, rect->width() - titleBarHeight,
                     rect->height() - borderWidth,
                     rect->width() - titleBarHeight,
                     rect->height());
}

void Draw::drawShineBorder(Window window, GC gc, Rectangle *rect,
                           unsigned long shine)
{
    XPoint shineBorderPoints[3];
    shineBorderPoints[0].x = rect->x();
    shineBorderPoints[0].y = rect->y() + rect->height() - 1;
    shineBorderPoints[1].x = rect->x();
    shineBorderPoints[1].y = rect->y();
    shineBorderPoints[2].x = rect->x() + rect->width() - 1;
    shineBorderPoints[2].y = rect->y();

    XCORE->setForeground(gc, shine);
    XCORE->drawLines(window, gc, shineBorderPoints, 3);
}

void Draw::drawShadowBorder(Window window, GC gc, Rectangle *rect,
                                 unsigned long shadow)
{
    XPoint shadowBorderPoints[3];
    shadowBorderPoints[0].x = rect->x() + rect->width() - 1;
    shadowBorderPoints[0].y = rect->y();
    shadowBorderPoints[1].x = rect->x() + rect->width() - 1;
    shadowBorderPoints[1].y = rect->y() + rect->height() - 1;
    shadowBorderPoints[2].x = rect->x();
    shadowBorderPoints[2].y = rect->y() + rect->height() - 1;

    XCORE->setForeground(gc, shadow);
    XCORE->drawLines(window, gc, shadowBorderPoints, 3);
}

void Draw::drawRectBorder(Window window, GC gc, Rectangle *rect,
                         unsigned long shine, unsigned long shadow)
{
    drawShadowBorder(window, gc, rect, shadow);
    drawShineBorder(window, gc, rect, shine);
}

void Draw::drawStickyNotifier(Window window, GC gc, Rectangle *rect,
                              unsigned long shine, unsigned long shadow,
                              unsigned int textWidth)
{
    // We assume that the text is centered!
    XCORE->setForeground(gc, shadow);
    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + 1,
                    rect->y() + FIGURE_DISTANCE + 2,
                    rect->x() + rect->width() / 2 - textWidth / 2 - FIGURE_DISTANCE - 1,
                    rect->y() + FIGURE_DISTANCE + 2);

    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + rect->width() / 2 + textWidth / 2 + FIGURE_DISTANCE,
                    rect->y() + FIGURE_DISTANCE + 2,
                    rect->x() + rect->width() - FIGURE_DISTANCE - 1,
                    rect->y() + FIGURE_DISTANCE + 2);

    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + 1,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 1,
                    rect->x() + rect->width() / 2 - textWidth / 2 - FIGURE_DISTANCE - 1,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 1);

    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + rect->width() / 2 + textWidth / 2 + FIGURE_DISTANCE,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 1,
                    rect->x() + rect->width() - FIGURE_DISTANCE - 1,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 1);

    XCORE->setForeground(gc, shine);
    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + 1,
                    rect->y() + FIGURE_DISTANCE + 1,
                    rect->x() + rect->width() / 2 - textWidth / 2 - FIGURE_DISTANCE - 1,
                    rect->y() + FIGURE_DISTANCE + 1);

    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + rect->width() / 2 + textWidth / 2 + FIGURE_DISTANCE,
                    rect->y() + FIGURE_DISTANCE + 1,
                    rect->x() + rect->width() - FIGURE_DISTANCE - 1,
                    rect->y() + FIGURE_DISTANCE + 1);

    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + 1,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 2,
                    rect->x() + rect->width() / 2 - textWidth / 2 - FIGURE_DISTANCE - 1,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 2);

    XCORE->drawLine(window, gc,
                    rect->x() + FIGURE_DISTANCE + rect->width() / 2 + textWidth / 2 + FIGURE_DISTANCE,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 2,
                    rect->x() + rect->width() - FIGURE_DISTANCE - 1,
                    rect->y() + rect->height() - FIGURE_DISTANCE - 2);
}

void Draw::drawTransRectangle(Window window, GC gc,
                          Rectangle *rect, unsigned int barHeight,
                          unsigned int borderWidth)
{
    XCORE->drawRectangle(window, gc,
                   rect->x(), rect->y(),
                   rect->width(), rect->height());

    unsigned int offsetY = borderWidth;
    if (barHeight > 0) {
        XCORE->drawLine(window, gc,
                  rect->x() + borderWidth / 2,
                  rect->y() + barHeight +
                      borderWidth,
                  rect->x() + rect->width() -
                      borderWidth / 2,
                  rect->y() + barHeight +
                      borderWidth);
        offsetY += barHeight + borderWidth;
    }
#if DIAMOND_BOX
    XPoint orthRectPoints[3];

    orthRectPoints[0].x = rect->x() + rect->width() / 3;
    orthRectPoints[0].y = rect->y() + rect->height() - borderWidth;
    orthRectPoints[1].x = rect->x() + borderWidth;
    orthRectPoints[1].y = rect->y() + rect->height() / 2 + offsetY / 2;
    orthRectPoints[2].x = orthRectPoints[0].x;
    orthRectPoints[2].y = rect->y() + offsetY;
    XCORE->drawLines(window, gc, orthRectPoints, 3);

    orthRectPoints[0].x = rect->x() + (2 * rect->width()) / 3;
    orthRectPoints[0].y = rect->y() + offsetY;
    orthRectPoints[1].x = rect->x() + rect->width() - borderWidth;
    // same as above
    //orthRectPoints[1].y = rect->y() + rect->height() / 2 + offsetY / 2;
    orthRectPoints[2].x = orthRectPoints[0].x;
    orthRectPoints[2].y = rect->y() + rect->height() - borderWidth;
    XCORE->drawLines(window, gc, orthRectPoints, 3);
#endif
}
