// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: clientbar.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __CLIENTBAR_H
#define __CLIENTBAR_H

#include "bar.h"

class Menu;
class Monitor;

/**
 * Represents the floating app bar of WMI.
 */
class ClientBar : public Bar {

public:

    enum Mode {PAGER, CLIENTINFO};

    ClientBar(Monitor *monitor, Rectangle *rect);

    ~ClientBar();

    virtual void illuminate();

    virtual void handleMotionNotify(XMotionEvent *event);
    virtual void handleButtonPress(XButtonEvent *event);
    virtual void handleButtonRelease(XButtonEvent *event);

    void toggleMenuMode();
    void setMode(Mode mode);
    Mode mode() const;

    Menu *menu() const;

private:

    void updateClientInfo();
    void updateClientPager();
    void updateModeButton();
    void updateMenuButton();

    void invokeClickedThing(int xPosition);
    bool isModeButtonPressed_;
    bool isMenuButtonPressed_;
    Mode mode_;
    Menu *menu_;
};

#endif // __CLIENTBAR_H
