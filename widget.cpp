// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: widget.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "widget.h"

#include "cursors.h"
#include "kernel.h"
#include "monitor.h"
#include "xcore.h"
#include "xfont.h"

Widget::Widget(Monitor *monitor, Rectangle *rect, bool initWindowAndGC)
    : Rectangle(*rect)
{
    monitor_ = monitor;
    isVisible_ = false;
    if (initWindowAndGC) {
        initWindow();
        initGC();
    }
    else {
        window_ = 0;
        gc_ = 0;
    }
}

Widget::~Widget() {
    XCORE->free(gc_);
    XCORE->destroy(window_);
}

Monitor *Widget::monitor() const  {
    return monitor_;
}

Window Widget::window() const {
    return window_;
}

GC Widget::gc() const {
    return gc_;
}

void Widget::show() {
    isVisible_ = true;
    XCORE->showRaised(window_);
}

void Widget::hide() {
    isVisible_ = false;
    XCORE->hide(window_);
}

bool Widget::isVisible() const {
    return isVisible_;
}

void Widget::initWindow() {
    Kernel *kernel = KERNEL;

    XSetWindowAttributes attr;
    attr.override_redirect = 1;
    attr.background_pixmap = ParentRelative;

    window_ = XCORE->createWindow(monitor_->rootWindow(), &attr,
                                  x(), y(), width(), height(),
                                  CWOverrideRedirect | CWBackPixmap);
    XCORE->selectEvents(window_,
                 ExposureMask | ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | SubstructureRedirectMask |
                 SubstructureNotifyMask);

    kernel->installCursor(Cursors::NORMAL_CURSOR, window_);
}

void Widget::initGC() {
    unsigned long mask = 0;
    XGCValues gcv;

    mask = GCForeground | GCBackground |
           GCFunction | GCLineWidth | GCLineStyle;

    gcv.function = GXcopy;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    if (monitor_->font()->type() == WFont::NORMAL) {
        gcv.font = ((XFont *)monitor_->font())->font()->fid;
        mask |= GCFont;
    }

    gc_ = XCORE->createGC(window_, mask, &gcv);
}

void Widget::resize() {
    XCORE->moveResize(window_, this);
}
