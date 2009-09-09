// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: frame.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __FRAME_H
#define __FRAME_H

#include "container.h"
#include "thing.h"
#include "wmi.h"

// forward declaration
class Client;
class Tree;
class Workspace;

typedef list<Client *> LClient;

/**
 * Represents a frame of WMI. Frames are container for clients in
 * non-overlapping configurations.
 */
class Frame
    : public Thing,
      public Container<Workspace, LClient, LClient::iterator, Client>
{

public:

    Frame(Workspace *workspace, Rectangle *rect);

    ~Frame();

    void focus(Client *client);

    void attach(Client *client);
    Client *detach(Client *client);

    virtual Window window();

    /** Resizes all clients of this frame. */
    virtual void resize();

    /** Illuminates this frame. */
    virtual void illuminate();

    virtual bool isFocused();

    virtual void handleButtonPress(XButtonEvent *event);
    virtual void handleButtonRelease(XButtonEvent *event);

    virtual Cursor cursorForXY(int pointerX, int pointerY);

    /** Matches client size. */
    void matchClientSize(Client *client);

    void setTree(Tree *t);

    Tree *tree() const;

    void toggleTiled();
    void zoomClient();

private:

    bool isTiled_;
    Tree *tree_;
};

#endif // __FRAME_H
