// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __THING_H
#define __THING_H

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

#include <string>

#include "rectangle.h"
#include "wmi.h"

class Label;
class Monitor;
class Frame;
class Group;
class Theme;
class Workspace;

/**
 * Base class of frames and clients which provides the base
 * functionality for both.
 */
class Thing : public Rectangle {

public:

    ////////////////////////////////////////////////////////////
    // model

    enum Type {CLIENT, FRAME};

    Thing(Monitor *monitor, Rectangle *rect, Type type_);

    virtual ~Thing();

    void initFrameWindow();

    Type type() const;

    Monitor *monitor() const;

    bool isVisible() const;

    ////////////////////////////////////////////////////////////
    // view

    enum InvertButton {NONE, MINCLIENT, DEMAX, CLOSE};

    void setTitleBarHeight(unsigned int titleBarHeight);

    unsigned int titleBarHeight() const;

    void setBorderWidth(unsigned int borderWidth);

    unsigned int borderWidth() const;

    Rectangle *prevRectangle() const;

    InvertButton buttonState() const;

    void setButtonState(InvertButton state);

    void handleMotionNotify(XMotionEvent *event);

    virtual Window window() = 0;
    virtual void illuminate() = 0;
    virtual void resize() = 0;
    virtual bool isFocused() = 0;
    virtual void handleButtonPress(XButtonEvent *event) = 0;
    virtual void handleButtonRelease(XButtonEvent *event) = 0;
    virtual Cursor cursorForXY(int pointerX, int pointerY) = 0;

    virtual void show();
    virtual void hide();

    void setName(string name);
    string name() const;
    bool hasDecoration() const;

protected:

    void fitClientArea();

    // model
    bool isVisible_;
    string name_;

    // view
    void illuminateBorder();

    Rectangle clientAreaRect_;
    Theme *theme_;
    Label *label_;
    GC gc_;
    unsigned int titleBarHeight_;
    unsigned int borderWidth_;
    bool areButtonsVisible_;

    Cursor cursor_;

    InvertButton buttonState_;

    // model
    Window frameWindow_;
    bool hasDecoration_;

private:

    // view
    void initGC();

    Monitor *monitor_;
    Type type_;

    Rectangle *prevRect_;
};

#endif // __THING_H
