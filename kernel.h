// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: kernel.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __KERNEL_H
#define __KERNEL_H

#include "singleton.h"

extern "C" {
#include <X11/Xlib.h>
}

// STL list
#include <list>
#include <string>
#include <map>

#include "wmi.h"

// forward declaration
class Action;
class Bar;
class Box;
class Client;
template <class P, class LT, class LTI, class T> class Container;
class Frame;
class Kernel;
class Menu;
class Monitor;
class Prompt;
class Rectangle;
class Shortcut;
#ifdef SLOT_SUPPORT
class Slot;
#endif // SLOT_SUPPORT
class Thing;
class Workspace;

typedef list<Action *> LAction;
typedef list<Monitor *> LMonitor;
typedef map<string, string, less<string> > MSettings;
typedef map<string, Action *, less<string> > MBindings;
typedef Container<Kernel, LMonitor, LMonitor::iterator, Monitor> CMonitor;

/**
 * Main controller of WMI. Contains all base functionality
 * of a window manager.
 */
class Kernel : public Singleton<Kernel>
{

public:

    /** Constructs a new window manager instance.  */
    Kernel();

    ~Kernel();

    /** Returns settings. */
    MSettings *themeSettings();

    MSettings *commonSettings();

    MSettings *actionSettings();

    MSettings *sessionSettings();

    /** Returns the display. */
    Display *display() const;

    /** Returns the currently focused monitor. */
    Monitor *focusedMonitor() const;

    /** Returns wether input mode is active. */
    bool isInputMode();

    /** Returns focused client. */
    Client *focusedClient();

    void killClient(Client *client);

    /** Returns the root window. */
    Window rootWindow();

    /** Returns the default root window (of deafault monitor). */
    Window defaultRootWindow();

    /** Runlevel enum. */
    enum Runlevel {START, RUN, STOP, RESTART};

    Runlevel runlevel() const;

    /** Start method. */
    int start(char *argv[],
              Display *display,
              MSettings *themeSettings,
              MSettings *commonSettings,
              MSettings *actionSettings,
              MSettings *sessionSettings,
              bool isFirstRun = false);

    void stop();
    void restart();

    /** Updates all bars.*/
    void updateBars();

    /** Saves session settings. */
    void saveSettings();

    MBindings *actionBindings() const;

    /** Returns <code>true</code> if window is a root window. */
    bool isRootWindow(Window window);

    /** Returns bar if window is a barwindow. */
    Bar *barWindow(Window window);

    /** Returns box if window is a boxwindow. */
    Box *boxWindow(Window window);

    /** Returns menu if window is a menu window. */
    Menu *menuWindow(Window window);

#ifdef SLOT_SUPPORT
    /** Returns slot if window is a slot window. */
    Slot *slotWindow(Window window);
#endif // SLOT_SUPPORT

    /** Returns frame if window is a frame window. */
    Thing *thingWindow(Window window);

    /** Returns <code>true</code> if window is a WMI window. */
    bool isWMIWindow(Window window);

    /** Returns client for this window. */
    Client *clientForWindow(Window window);

    /** Binds the shortcut for the given <action id>=<key> pair. */
    void bindShortcut(string argument);

    void installCursor(Cursor cursor, Window window); 

    /** Adds client. */ 
    void addClient(Client *client);

    /** Main event loop. */
    int run();

    /** Resize event loop. */
    void runResizeMode(Thing *thing, XButtonEvent *buttonEvent,
                       Direction dir, bool resize = false);

    bool isRecording() const;

    LAction *recordedActions();

    void beginRecording();
    void endChainRecording(string name);
    void endScriptRecording(string name);
    void cancelRecording();


    unsigned int borderWidth() const;

    void cleanup();

    Prompt *defaultPrompt() const;

    void setMenuMode(bool isMenuMode);

    void toggleSloppyMode();

    bool isStackedTabbing() const;

    void toggleShortcuts();

    CMonitor *monitors() const;
    void selectMonitor(string displayString);

    /** Enters runResizeMode for moving focussed clients with a
        faked XButtonEvent. */
    void grabMove();

private:

    /** event handler. */
    void handleButtonPress(XButtonEvent *event);
    void handleButtonRelease(XButtonEvent *event);
    void handleConfigureRequest(XConfigureRequestEvent *event);
    void handleClientMessage(XClientMessageEvent *event);
    void handleCreateNotify(XCreateWindowEvent *event);
    void handleDestroyNotify(XDestroyWindowEvent *event);
    void handleExpose(XExposeEvent *event);
    void handleKeyPress(XKeyEvent *event);
    void handleMapRequest(XMapRequestEvent *event);
    void handleMotionNotify(XMotionEvent *event);
    void handlePropertyNotify(XPropertyEvent *event);
    void handleUnmapNotify(XUnmapEvent *event);

    /** Atom initialization. */
    void initAtoms();

    /** Inits all monitors. */
    void initMonitors();

    /** Inits all existing windows. */
    void initWindows();

    /** Inits the bind map for all actions. */
    void initActionBindings();

    /** Inits keys on all monitors. */
    void initKeys();

    /** Grabs shortcut on all monitors. */
    void grabShortcutOnAllMonitors(Shortcut *shortcut);

    /** Ungrabs shortcut on all monitors. */
    void ungrabShortcutOnAllMonitors(Shortcut *shortcut);

    /**
     * Serializes and updates all settings.
     */
    void serialize();

    bool isSloppyMode_;
    bool isRecording_;
    LAction recordedActions_;
    Display *display_;
    CMonitor *monitors_;

    unsigned int borderWidth_;
    bool isMenuMode_;
    bool isStackedTabbing_;

    // default rc settings
    MSettings *themeSettings_;
    MSettings *commonSettings_;
    MSettings *actionSettings_;
    MSettings *sessionSettings_;

    // bind map
    MBindings *actionBindings_;

    Runlevel runlevel_;
    Prompt *defaultPrompt_;

};

#endif // __KERNEL_H
