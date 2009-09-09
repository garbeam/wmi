// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: monitor.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __MONITOR_H
#define __MONITOR_H

extern "C" {
#include <X11/Xlib.h>
}

#include <list>
#include <map>
#include <string>
#include "wmi.h"

#include "container.h"
#include "rectangle.h"

class Action;
class Box;
class Client;
class ClientBar;
class Frame;
class InputBar;
class Manager;
class MultiFrame;
#ifdef SLOT_SUPPORT
class Slot;
#endif // SLOT_SUPPORT
class StatusBar;
class Theme;
class Thing;
class WFont;
class Workspace;
class XFont;

typedef list<Client *> LClient;
typedef list<Thing *> LThing;
typedef list<Workspace *> LWorkspace;
typedef map<long, Client *, less<long> > MClient;
typedef map<string, string, less<string> > MSettings;
typedef Container<Monitor, LClient, LClient::iterator, Client> CClient;

/**
 * Represents a physical monitor in multi-head or single-head
 * configurations not using Xinerama or a Xinerama monitor in
 * configurations using Xinerama of WMI.
 */
class Monitor :
    public Container<Display, LWorkspace, LWorkspace::iterator, Workspace>,
    public Rectangle
{

public:

    ////////////////////////////////////////////////////////////
    // Monitor stuff
    Monitor(Display *display, unsigned int id);
    ~Monitor();

    unsigned int id() const;

    /** Returns the root window of this monitor. */
    Window rootWindow() const;

    string displayString() const;

    Theme *theme() const;

    unsigned int resizeFactor() const;

    string prefix() const;

    void serialize();

    ////////////////////////////////////////////////////////////
    // Bar stuff
    StatusBar *statusBar() const;

    ClientBar *clientBar() const;

    InputBar *inputBar() const;

    void toggleStatusBar();

    void toggleClientBar();

    void scanWindows();
    void updateBars();

    ////////////////////////////////////////////////////////////
    // Client stuff

    Client *focusedClient();

    /**
     * Returns the client for the given window if it exists,
     * otherwise <code>0</code>.
     * If the window is the root window or <code>0</code>
     * then <code>0</code> will be returned.
     */
    Client *clientForWindow(Window window);

    /** Adds client. */ 
    void addClient(Client *client);

    /** Removes the specified client. */
    void removeClient(Client *client);

    void detachClient(); // hide
    void detachClient(Client *client);
    void attachLastClient(); // show
    void attachClientByName(string name); // show
    // with transient handling
    void attachClient(Client *client);

    /** Attaches all detached clients. */
    void attachAllClients();
    void detachAllClients();
    void toggleAllClientStates();

    CClient *detachedClients();

    /** Maximization handling. */
    void toggleThingMaximization();

    bool isThingMaximized();

    /** Attaches detached client to workspace. */
    void attachDetachedClient(Workspace *workspace, Client *client);

    unsigned int titleBarHeight() const;
    unsigned int buttonWidth() const;

    ////////////////////////////////////////////////////////////
    // Workspace stuff
    void createNewWorkspace(string name);

    void focus(Workspace *workspace);
    void focusWorkspaceNum(unsigned int workspaceNum);
    void focusWorkspaceByName(string name);
    void focusClientByName(string name);
    void focusClientById(string id);

    void destroyWorkspace(Workspace *workspace);

    /** Rename methods. */
    void renameWorkspace(Workspace *workspace, string newName);

    void matchWorkspace(Workspace *workspace, Direction dir);
    void illuminateTransRect(Rectangle *rect, unsigned int barHeight = 0);
    void illuminateTransText(Rectangle *rect, string text);

    Thing *thingWindow(Window window);

    Bar *barWindow(Window window);

    Menu *menuWindow(Window window);

#ifdef SLOT_SUPPORT
    Slot *slotWindow(Window window);
    Slot *slot() const;
    void unslotClient();
    void slotClient();
    void toggleSlot();
    void cycleSlotTabPrev();
    void cycleSlotTabNext();
#endif // SLOT_SUPPORT

    /**
     * Used for differentiation of clients, each client has an
     * unique id.
     */
    unsigned int nextClientId();

    void resetClientId() {
        clientIds_ = 0;
    };

    GC borderGC();

    CClient *stickyClients();

    WFont *font() const;

    /**
     * Reparents all client windows to the root window, destroys all
     * frames and WMI related items of clients.
     */
    void cleanup();

    MClient *clients();
    void banish();

    Workspace *workspaceForName(string name);

    Box *box() const;

    void hookClient();
    void unhookClient();

private:

    /** Inits all fonts. */
    void initFonts();

    /** Inits display string. */
    void initDisplayString();

    /** Inits workspaces. */
    void initWorkspaces();

    /** Inits the bar. */
    void initBars();

    /** Inits gc. */
    void initGC();

    /** Initializes rectangle with workspace size. */
    void rectForWorkspace(Workspace *workspace);

    void focusClient(Client *client);

    GC pseudoBorderGC_;
    GC borderGC_;
    Window rootWindow_;
    WFont *font_;
    XFont *fixed_; // default font for pseudo border, if XFT
                   // font is used


    Box *box_;
    StatusBar *statusBar_;
    ClientBar *clientBar_;
    InputBar *inputBar_;

#ifdef SLOT_SUPPORT
    /** Inits slot. */
    void initSlot();

    Slot *slot_;
#endif // SLOT_SUPPORT


    unsigned int resizeFactor_;

    unsigned int id_;

    string displayString_;

    Theme *theme_;

    MClient clients_;

    CClient detachedClients_;
    CClient stickyClients_;

    MSettings *themeSettings_;
    MSettings *sessionSettings_;
    MSettings *commonSettings_;
    string prefix_;

    Thing *maxThing_;

    unsigned int clientIds_;
};

#endif // __MONITOR_H
