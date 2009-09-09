// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "thing.h"

#include "client.h"
#include "cursors.h"
#include "draw.h"
#include "font.h"
#include "frame.h"
#include "kernel.h"
#include "label.h"
#include "logger.h"
#include "monitor.h"
#include "theme.h"
#include "workspace.h"
#include "xcore.h"
#include "xfont.h"

Thing::Thing(Monitor *monitor, Rectangle *rect, Type type)
    : Rectangle(*rect)
{
    monitor_ = monitor;
    type_ = type;
    isVisible_ = false;
    hasDecoration_ = false;
    clientAreaRect_ = Rectangle(*rect);
    name_ = "";

    // view
    theme_ = monitor_->theme();

    areButtonsVisible_ =
        (Util::get(KERNEL->commonSettings(), "frame.buttons") == "yes");

    cursor_ = Cursors::NORMAL_CURSOR;
    prevRect_ = new Rectangle(*rect);
    buttonState_ = NONE;

    // creates frame window
    if (type == FRAME) {
        initFrameWindow();
        fitClientArea();
    }
    else {
        frameWindow_ = 0;
        gc_ = 0;
        label_ = 0;
        titleBarHeight_ = 0;
        borderWidth_ = 0;
    }
}

Thing::~Thing() {
    delete prevRect_;

    if (frameWindow_) {
        delete label_;
        XCORE->free(gc_);
        XCORE->destroy(frameWindow_);
        LOGDEBUG("frame destroyed");
    }
}

void Thing::initFrameWindow() {

    titleBarHeight_ =
        (Util::get(KERNEL->commonSettings(), "default.bar-mode")
         == "show") ? monitor_->titleBarHeight() : 0;
    borderWidth_ = 
        (Util::get(KERNEL->commonSettings(), "default.border-mode")
         == "show") ? KERNEL->borderWidth() : 0;

    if (type_ == CLIENT) {
        setX(x() - borderWidth_);
        setY(y() - titleBarHeight_ - borderWidth_ - 1);
        setWidth(width() + 2 * borderWidth_);
        setHeight(height() + titleBarHeight_ + 2 * borderWidth_ + 1);
    }

    XSetWindowAttributes attr;
    attr.background_pixmap = ParentRelative;
    attr.override_redirect = 1;
    frameWindow_ = XCORE->createWindow(this->monitor()->rootWindow(), &attr,
                                       x(), y(), width(), height(),
                                       CWOverrideRedirect | CWBackPixmap);

    LOGDEBUG("init GC for frame");
    initGC();
    label_ = new Label(this->monitor(), frameWindow_, Label::CENTER, gc_);
    label_->setY(borderWidth_);
    label_->setHeight(titleBarHeight_);

    XCORE->selectEvents(frameWindow_,
            ExposureMask | SubstructureRedirectMask | SubstructureNotifyMask |
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

    KERNEL->installCursor(Cursors::NORMAL_CURSOR, frameWindow_);
    hasDecoration_ = true;
}

Thing::Type Thing::type() const {
    return type_;
}

Monitor *Thing::monitor() const {
    return monitor_;
}

bool Thing::isVisible() const {
    return isVisible_;
}

void Thing::initGC() {

    if (frameWindow_) {
        unsigned long mask =
            GCForeground | GCBackground |
            GCFunction | GCLineWidth | GCLineStyle;

        XGCValues gcv;
        gcv.function = GXcopy;
        gcv.line_width = 1;
        gcv.line_style = LineSolid;
        if (monitor()->font()->type() == WFont::NORMAL) {
            gcv.font = ((XFont *)monitor()->font())->font()->fid;
            mask |= GCFont;
        }
        gc_ = XCORE->createGC(frameWindow_, mask, &gcv);
    }
}

void Thing::illuminateBorder() {

    if (!isVisible_ || !frameWindow_) {
        return;
    }

    bool focused = isFocused();

    if (titleBarHeight_) {
        label_->setText("");
        label_->setX(borderWidth_);
        label_->setWidth(width() - 2 * borderWidth_);

        if (focused) {
            label_->update(theme_->FRAME_BACKGROUND_FOCUSSED, 0,
                           theme_->FRAME_SHINE_FOCUSSED,
                           theme_->FRAME_SHADOW_FOCUSSED);
        }
        else {
            label_->update(theme_->FRAME_BACKGROUND_NORMAL, 0,
                           theme_->FRAME_SHINE_NORMAL,
                           theme_->FRAME_SHADOW_NORMAL);
        }
    }

    LOGDEBUG("drawing border");
    if (focused) {
        Draw::drawBorder(frameWindow_, gc_, monitor()->borderGC(), this,
                        theme_->FRAME_BACKGROUND_FOCUSSED,
                        theme_->FRAME_SHINE_FOCUSSED,
                        theme_->FRAME_SHADOW_FOCUSSED,
                        titleBarHeight_ + borderWidth_,
                        borderWidth_);
    }
    else {
        Draw::drawBorder(frameWindow_, gc_, monitor()->borderGC(), this,
                        theme_->FRAME_BACKGROUND_NORMAL,
                        theme_->FRAME_SHINE_NORMAL,
                        theme_->FRAME_SHADOW_NORMAL,
                        titleBarHeight_ + borderWidth_,
                        borderWidth_);
    }

    if (titleBarHeight_ && areButtonsVisible_ && focused) {

        if (type_ == FRAME && !((Frame *)this)->size()) {
            // no need to draw buttons
            return;
        }

        // detach button
        label_->setText("");
        label_->setX(width() - borderWidth_ - 3 * monitor_->buttonWidth());
        label_->setWidth(monitor_->buttonWidth());
        if (buttonState_ != MINCLIENT) {

            Draw::drawDetachButton(frameWindow_, gc_, label_,
                           theme_->BUTTON_BACKGROUND_NORMAL,
                           theme_->BUTTON_SHINE_BORDER_NORMAL,
                           theme_->BUTTON_SHADOW_BORDER_NORMAL,
                           theme_->BUTTON_SHINE_FIGURE_NORMAL,
                           theme_->BUTTON_SHADOW_FIGURE_NORMAL);
        }
        else {

            Draw::drawDetachButton(frameWindow_, gc_, label_,
                           theme_->BUTTON_BACKGROUND_PRESSED,
                           theme_->BUTTON_SHINE_BORDER_PRESSED,
                           theme_->BUTTON_SHADOW_BORDER_PRESSED,
                           theme_->BUTTON_SHINE_FIGURE_PRESSED,
                           theme_->BUTTON_SHADOW_FIGURE_PRESSED);
        }

        // (de)max client button
        label_->setX(label_->x() + monitor_->buttonWidth());

        if (buttonState_ != DEMAX) {

            if (type_ == CLIENT) {

                Draw::drawMaxButton(frameWindow_, gc_, label_,
                           theme_->BUTTON_BACKGROUND_NORMAL,
                           theme_->BUTTON_SHINE_BORDER_NORMAL,
                           theme_->BUTTON_SHADOW_BORDER_NORMAL,
                           theme_->BUTTON_SHINE_FIGURE_NORMAL,
                           theme_->BUTTON_SHADOW_FIGURE_NORMAL);
            }
            else {

                Draw::drawFloatButton(frameWindow_, gc_, label_,
                           theme_->BUTTON_BACKGROUND_NORMAL,
                           theme_->BUTTON_SHINE_BORDER_NORMAL,
                           theme_->BUTTON_SHADOW_BORDER_NORMAL,
                           theme_->BUTTON_SHINE_FIGURE_NORMAL,
                           theme_->BUTTON_SHADOW_FIGURE_NORMAL);
            }
        }
        else {

            if (type_ == CLIENT) {

                Draw::drawMaxButton(frameWindow_, gc_, label_,
                           theme_->BUTTON_BACKGROUND_PRESSED,
                           theme_->BUTTON_SHINE_BORDER_PRESSED,
                           theme_->BUTTON_SHADOW_BORDER_PRESSED,
                           theme_->BUTTON_SHINE_FIGURE_PRESSED,
                           theme_->BUTTON_SHADOW_FIGURE_PRESSED);
            }
            else {

                Draw::drawFloatButton(frameWindow_, gc_, label_,
                           theme_->BUTTON_BACKGROUND_PRESSED,
                           theme_->BUTTON_SHINE_BORDER_PRESSED,
                           theme_->BUTTON_SHADOW_BORDER_PRESSED,
                           theme_->BUTTON_SHINE_FIGURE_PRESSED,
                           theme_->BUTTON_SHADOW_FIGURE_PRESSED);
            }

        }

        // close button
        label_->setX(label_->x() + monitor_->buttonWidth());
        if (buttonState_ != CLOSE) {

            Draw::drawCloseButton(frameWindow_, gc_, label_,
                    theme_->BUTTON_BACKGROUND_NORMAL,
                    theme_->BUTTON_SHINE_BORDER_NORMAL,
                    theme_->BUTTON_SHADOW_BORDER_NORMAL,
                    theme_->BUTTON_SHINE_FIGURE_NORMAL,
                    theme_->BUTTON_SHADOW_FIGURE_NORMAL);
        }
        else {

            Draw::drawCloseButton(frameWindow_, gc_, label_,
                    theme_->BUTTON_BACKGROUND_PRESSED,
                    theme_->BUTTON_SHINE_BORDER_PRESSED,
                    theme_->BUTTON_SHADOW_BORDER_PRESSED,
                    theme_->BUTTON_SHINE_FIGURE_PRESSED,
                    theme_->BUTTON_SHADOW_FIGURE_PRESSED);

        }
    }
}

void Thing::handleMotionNotify(XMotionEvent *event) {

    if (monitor()->isThingMaximized()) {
        return;
    }

    Cursor cursor = cursorForXY(event->x, event->y);

    if (cursor != cursor_) {
        cursor_ = cursor;
        KERNEL->installCursor(cursor, frameWindow_);
    }

    if (buttonState_ != NONE) {
        buttonState_ = NONE;
        illuminate();
    }
}

void Thing::setTitleBarHeight(unsigned int titleBarHeight) {
    titleBarHeight_ = titleBarHeight;
}

void Thing::setBorderWidth(unsigned int borderWidth) {
    borderWidth_ = borderWidth;
}

unsigned int Thing::borderWidth() const {
    return borderWidth_;
}

unsigned int Thing::titleBarHeight() const {
    return titleBarHeight_;
}

Rectangle *Thing::prevRectangle() const {
    return prevRect_;
}

Thing::InvertButton Thing::buttonState() const {
    return buttonState_;
}

void Thing::setButtonState(InvertButton state) {
    buttonState_ = state;
}

void Thing::show() {
    if (frameWindow_) {
        XCORE->show(frameWindow_);
        isVisible_ = true;
    }
}

void Thing::hide() {
    if (frameWindow_) {
        XCORE->hide(frameWindow_);
        isVisible_ = false;
    }
}

void Thing::fitClientArea() {

    clientAreaRect_.setX(borderWidth_);
    clientAreaRect_.setY(titleBarHeight_ + borderWidth_ +
                         (titleBarHeight_ ? 1 : 0));
    clientAreaRect_.setWidth(width() - 2 * borderWidth_);
    clientAreaRect_.setHeight(height() - titleBarHeight_
                              - 2 * borderWidth_
                              - (titleBarHeight_ ? 1 : 0));
}

void Thing::setName(string name) {
    name_ = name;
}

string Thing::name() const {
    return name_;
}

bool Thing::hasDecoration() const {
    return hasDecoration_;
}
