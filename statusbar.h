// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __STATUSBAR_H
#define __STATUSBAR_H

#include "bar.h"

#include <set>
#include <string>
#include "wmi.h"

class Kernel;
class Monitor;

/**
 * Represents the statusbar of WMI.
 */
class StatusBar : public Bar  {

public:

    StatusBar(Monitor *monitor, Rectangle *rect);

    ~StatusBar();

    virtual void illuminate();

    virtual void handleMotionNotify(XMotionEvent *event);
    virtual void handleButtonPress(XButtonEvent *event);
    virtual void handleButtonRelease(XButtonEvent *event);

    void setText(const string text);
    void setMeterText(const string meterText);

private:

    unsigned int calculateMetersWidth();
    unsigned int calculateWorkspaceWidth();
    void invokeClickedThing(int xPosition);

    void updateInputModeButton(unsigned int *offsetX);
    void updateWorkspacePager(unsigned int *offsetX);
    void updateDetachedClients(unsigned int *offsetX);
    void updateStatus(unsigned int *offsetX);
    void updateMeters(unsigned int offsetX);

    string meterText_;
    string text_;
    bool buttonPressed_;
};

#endif // __STATUSBAR_H
