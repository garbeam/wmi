// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: slot.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __SLOT_H
#define __SLOT_H

#include "wmi.h"

#ifdef SLOT_SUPPORT

extern "C" {
#include <X11/Xlib.h>
}

#include <list>
#include <string>
#include "container.h"
#include "widget.h"

class Client;
class Label;
class Monitor;
class Theme;

typedef list<Client *> LClient;

/**
 * Class for slot tabs.
 */
class Tab {

public:

    Tab(const string name);
    ~Tab();

    string name() const;
    LClient *clients();
    void addClient(Client *client);
    void removeClient(Client *client);

    void show();
    void hide();
    bool isVisible() const;

    Client *focused() const;
    void focus(Client *client);

private:

    Client *focused_;
    bool isVisible_;
    string name_;
    LClient clients_;
};

typedef list<Tab *> LTab;

/**
 * Base class of the slot which is capable to handle all kinds
 * of X clients in a dockapp manner.
 */
class Slot : public Widget,
             public Container<Monitor, LTab, LTab::iterator, Tab>
{

public:

    Slot(Monitor *monitor, Rectangle *rect, Direction align);

    virtual ~Slot();

    Direction alignment() const;

    void show(string tabName);
    virtual void hide();
    bool hasClients();
    bool isGrabFocus() const;
    void setGrabFocus(bool grabFocus);
    void focusClient(Client *client);

    void attachClient(Client *client);
    void detachClient(Client *client);
    void manage();

    /** called on Expose by Kernel. */
    void illuminate();

    void handleButtonPress(XButtonEvent *event);
    void handleMotionNotify(XMotionEvent *event);

    string tabName();

private:

    void initTabs();
    Tab *tabForName(string name);

    Direction align_;
    Label *label_;
    bool isSolid_;
    bool isGrabFocus_;
    Theme *theme_;
};

#endif // SLOT_SUPPORT
#endif // __SLOT_H
