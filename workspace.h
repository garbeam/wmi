// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: workspace.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __WORKSPACE_H
#define __WORKSPACE_H

#include <string>
#include <list>

#include "rectangle.h"
#include "container.h"
#include "client.h"

// forward declaration
class Frame;
class Monitor;
class Settings;
class Thing;
class Tree;
class Workspace;
class XCore;

typedef list<Client *> LClient;
typedef list<Frame *> LFrame;
typedef Container<Monitor, LClient, LClient::iterator, Client> CClient;
typedef Container<Monitor, LFrame, LFrame::iterator, Frame> CFrame;

/**
 * Represents a workspace of WMI.
 */
class Workspace :
      public Rectangle
{

public:

    Workspace(Monitor *monitor, unsigned int id, Rectangle *rect);

    ~Workspace();

    unsigned int id() const;

    /** Hides this workspace. */
    void hide();

    /** Shows this workspace. */
    void show();

    void illuminate();

    void setName(string name);

    string name() const;

    string prefix() const;

    Tree* root() const;

    bool isFrameMode() const;

    void toggleMode();
    void raiseFrames();

    /**
     * Attaches client to this workspace, if client->mode() is
     * <code>Client::MAX</code> and isChangeMode is <code>true</code>
     * this method will try to attach the client to the frame where the
     * center point of the client is located.
     */
    void attachClient(Client *client, bool isChangeMode = false);
    void detachClient(Client *client);

    /**
     * Splits the focused frame, if it has more than two clients
     * and attaches the focused client to it.
     */
    void splitFrame(Direction dir);
    void joinFrame(Direction dir);
    void sendClient(Direction dir);

    /**
     * Resize behavior for the focused frame.
     */
    void resizeFrame(Direction dir, bool grow);

    void resizeClient(Direction dir, bool grow);

    /** Focused client resize methods. */
    void moveClient(Direction dir);

    void selectFrame(Direction dir);
    void swapFrame(Direction dir);
    void swapClient(Direction dir);

    Thing *focusedThing();
    Frame *frameForPoint(unsigned int x, unsigned int y);

    void focus(Thing *thing, bool raise = true);

    Thing *thingWindow(Window window);

    void matchBarNeighbors(Direction dir);

    void raise();
    void lower();

    void toggleClientMode();
    void toggleClientSticky();
    void changeClientMode(Client *client, Client::Mode mode);

    void serialize();

    Tree *neighborTree(Direction dir);

    void toggleBars(bool visible);
    void toggleBorders(bool visible);

    void cycleClientNext();
    void cycleClientPrev();

    void removeClient(Client *client);

    /**
     * Used for differentiation of multiframes, each client has an
     * unique id.
     */
    unsigned int nextFrameId();

    void setClientBarVisible(bool visible);
    void setStatusBarVisible(bool visible);
#ifdef SLOT_SUPPORT
    void setSlotVisible(bool visible);
    bool isSlotVisible() const;
    string slotTabName() const;
#endif // SLOT_SUPPORT

    void resize(Thing *thing, Direction dir, bool grow);

    Client *topClient();
    void removePushClient(Client *client);

    void fitClient();

    bool isClientBarVisible() const;
    bool isStatusBarVisible() const;

    Frame *focusedFrame() const;
    Monitor *attached() const;

    LClient *clients();
    CClient *floatingClients();
    CFrame *frames();

    bool requestsFocus() const;
    void setRequestsFocus(bool requestsFocus);
    void destroyFrame(Frame *frame);

private:

    Monitor *monitor_;
    void pushClient(Client *client);
    bool stackContainsClient(Client *client);

    Frame *nextFrame(Direction dir, bool isResize = true);

    /** Returns the recently visited frame in the given direction.  */
    Frame *recentVisitedFrame(Direction dir);

    /** Helper for resizing clients. */

    void attachFrame(Frame *frame, Direction dir = DOWN);

    /** Creates empty attached frame and returns it. */
    Frame *newFrame();

    LClient globalClientStack_;
    CClient floatingClients_;
    CFrame frames_;
    bool isFrameMode_;
    bool requestsFocus_;
    unsigned int id_;
    string name_;
    string prefix_;

    Tree *root_;
    unsigned int frameIds_;

    bool isClientBarVisible_;
    bool isStatusBarVisible_;
#ifdef SLOT_SUPPORT
    string slotTabName_; // "" means not slot visible
#endif // SLOT_SUPPORT
};

#endif // __WORKSPACE_H
