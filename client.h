// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: client.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __CLIENT_H
#define __CLIENT_H

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

#include <list>
#include <string>
#include "thing.h"
#include "wmi.h"

// forward declaration
class Client;
class Monitor;
class Workspace;

typedef list<Client *> LClient;

/**
 * Representates an X client window.
 */
class Client : public Thing
{

public:

    enum Mode {FLOAT, MAX, STICKY
#ifdef SLOT_SUPPORT
        , SLOT
#endif // SLOT_SUPPORT
        };

    Client(Monitor *monitor, Window window, XWindowAttributes *attr);
    ~Client();

    /** Returns if this client is transient. */
    void setTransient(Client *client);
    Client *transient() const;

    long eventMask() const;

    string className() const;

    string instanceName() const;

    string iconName() const;

    void sendConfiguration();

    virtual void resize();

    /** event handler. */
    void handleConfigureRequest(XConfigureRequestEvent *event);
    void handleMapNotify(XMapEvent *event);
    void handlePropertyNotify(XPropertyEvent *event);
    void handleUnmapNotify(XUnmapEvent *event);

    virtual void show();
    virtual void hide();

    int protocols() const;

    bool isCentered() const;

    void initICCCM();

    void setMode(Mode mode);

    Mode mode() const;

    int state() const;

    /** Illuminates this frame. */
    virtual void illuminate();

    virtual bool isFocused();

    virtual void handleButtonPress(XButtonEvent *event);
    virtual void handleButtonRelease(XButtonEvent *event);

    virtual Cursor cursorForXY(int pointerX, int pointerY);

    virtual Window window();
    Window frameWindow() const;
    Window clientWindow() const;

    void setDestroyed(bool isDestroyed);
    bool isDestroyed() const;

    void createFrame();

    void reparent(Window parentWindow, int x, int y);

    string id();

    Frame *frame() const;
    void setFrame(Frame *frame);

    Workspace *attached() const;
    void setAttached(Workspace *workspace);

    bool requestsFocus() const;
    void setRequestsFocus(bool requestsFocus);

    /**
     * Defines where this client prefers to be attached to.
     * Must be a valid workspace or slot tab name.
     */
    string hooked() const;
    void setHooked(string hooked);

private:

    void gravitate(bool invert);

    /** Updates the size of this client. */
    void updateSize();

    /** Updates transient for this client. */
    void updateTransient();

    Window clientWindow_;

    string className_;
    string instanceName_;
    string iconName_;
    string hooked_;

    Workspace *workspace_;
    Client *transient_;
    Frame *frame_;

    unsigned int clientBorderWidth_;

    int protocols_;
    int state_;
    long eventMask_;

    bool isCentered_;
    bool hasFrame_;
    bool requestsFocus_;
    bool isDestroyed_;

    Mode mode_;
    string id_;
};

#endif // __CLIENT_H
