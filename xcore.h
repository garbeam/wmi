// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __XCORE_H
#define __XCORE_H

#include "wmi.h"

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef XFT_SUPPORT
#include <X11/Xft/Xft.h>
#endif // XFT_SUPPORT
}

#include <string>

#include "singleton.h"

#define PDELETE    1
#define PTAKEFOCUS 2

/**
 * Motif bits - see Motif reference manual for further details.
 */
struct MWMHints {
    // Think: do we need CARD32 and INT32 here?
    long flags;
    long functions;
    long decorations;
    int inputMode;
    long status;
};

class XCore;
class Rectangle;
class WFont;

/**
 * This class provides an abstraction layer for graphical user
 * interfaces such as X11 or maybe Y in future.
 */
class XCore : public Singleton<XCore>
{

public:

    /** Constructs new kernel instance. */
    XCore();

    ~XCore();

    void setDisplay(Display *display);

    /** Creates new window. */
    Window createWindow(Window root, XSetWindowAttributes *attr,
                        int x, int y, unsigned int w, unsigned int h,
                        unsigned long mask);

    /** Selects events specified by mask for window. */
    void selectEvents(Window window, long mask);

    /** Input focus setting. */
    void setInputFocus(Window window);

    /** Shows the specified window. (map) */
    void show(Window window);

    /** Shows the specified window raised. (map raised) */
    void showRaised(Window window);

    /** Hides the window. (unmap) */
    void hide(Window window);

    /** 
     * Creates new GC for the specified window, with gcv masked values.
     */
    GC createGC(Window window, unsigned long mask, XGCValues *gcv);

    /** Grabs the keyboard for the specific window.  */
    void grabKeyboard(Window window);

    /** Ungrabs the keyboard. */
    void ungrabKeyboard();

    /** Frees the data by X request. */
    void free(void *data);

    /** Syncs with X. */
    void sync(bool discard = False);

    /** Sets foreground color. */
    void setForeground(GC gc, unsigned long color);

    /** Draws lines performantly. */
    void drawLines(Window window, GC gc, XPoint *points, unsigned int num);

    /** Draws a line. */
    void drawLine(Window window, GC gc, unsigned int x1, unsigned int y1,
                  unsigned int x2, unsigned int y2);

    /** Draws rectangle. */
    void drawRectangle(Window window, GC gc, int x, int y, unsigned int w,
                       unsigned int h);

    /** Draws rectangle. */
    void drawRectangle(Window window, GC gc, Rectangle *rect);

    /** Clears the specific area within specified window. */
    void clearArea(Window window, Rectangle *rect);

    /** Clears the specific area within specified window. */
    void clearArea(Window window, int x, int y, unsigned int w, unsigned int h);

    /** Clears the specified window. */
    void clearWindow(Window window);

    /** Fills the specific window with rectangle. */
    void fillRectangle(Window window, GC gc, Rectangle *rect);

    /** Fills the specific window with rectangle. */
    void fillRectangle(Window window, GC gc, int x, int y,
                       unsigned int w, unsigned int h);

    /** Loads the font specified by name. */
    XFontStruct *loadQueryFont(string name);

    /** Draws a string at (x, y) to the specific window. */
    void drawText(Window window, GC gc, int x, int y,
                  string text);

    /** Returns text width for the given values. */
    int textWidth(XFontStruct *font, string text);

#ifdef XFT_SUPPORT
    /** Opens a XftFont. */
    XftFont *xftOpenFontName(unsigned int screen, string name);

    /** Closes the specific XftFont. */
    void xftCloseFont(XftFont *font);

    /** Draws a string at (x, y) to the specific window. */
    void drawText(XftFont *font, Window window, GC gc,
                  unsigned int screen, int x, int y,
                  string text);

    /** Returns text width for the given values. */
    int textWidth(XftFont *font, string text);
#endif

    /** Returns class name for the specific window. */
    string className(Window window);

    /** Returns instance name for the specific window. */
    string instanceName(Window window);

    /** Returns state for the specific window. */
    int state(Window window);

    /** Sets the state for the specific window. */
    void setState(Window window, int state);

    /** Returns value of the specific atom as string. */
    string atomValue(Window window, Atom atom);

    /** Updates size infos for the specific window. */
    void updateSize(Window window,
                    unsigned int *minW, unsigned int *minH,
                    unsigned int *maxW, unsigned int *maxH,
                    bool *isCentered);

    /** Returns true if the specific window has MWM decoration. */
    bool hasDecoration(Window window);

    /** Returns transient for the specific window. */
    Window transient(Window window);

    /** Returns property for specific values. */
    int property(Window window, Atom atom, Atom type,
                 long length, unsigned char **prop);

    /** Sends configuration to the specific window. */
    void configure(Window window, XConfigureEvent *event);

    /** Returns protocols for the specific window. */
    int protocols(Window window);

    /** Sends a message to the specific window. */
    void sendMessage(Window window, Atom atom, long value);

    /** Kills the client with the specific window. */
    void kill(Window window, int protos);

    /** Resizes the specific window. */
    void moveResize(Window window, Rectangle *rect);

    /** Destroys the window. */
    void destroy(Window window);

    /** Raises the specific window. */
    void raise(Window window);

    /** Lowers the specific window. */
    void lower(Window window);

    /** Reparents the specific window. */
    void reparent(Window child, Window parent, int x, int y);

    /** Grabs the key for the specified window. */
    void grabKey(Window window, unsigned int modMask, KeyCode keycode);

    /** Ungrabs key. */
    void ungrabKey(Window window, unsigned int modMask, KeyCode keycode);

    /** Grabs button. */
    void grabButton(Window window, unsigned int modMask, unsigned int button);

    /** Ungrabs button. */
    void ungrabButton(Window window, unsigned int modMask, unsigned int button);

    /** Grabs the server. */
    void grabServer();

    /** Ungrabs the server. */
    void ungrabServer();

    /** Grabs the pointer. */
    void grabPointer(Window window, unsigned int mask, Time time,
                     Cursor cursor = None);

    /** Ungrabs pointer. */
    void ungrabPointer(Time time);

    /** Returns specified atom. */
    Atom internAtom(string atom);

    /** Returns new font cursor specified by cursor. */
    Cursor createFontCursor(unsigned int cursor);

    /** Set window attributes. */
    void setWindowAttributes(Window window, unsigned int mask,
                             XSetWindowAttributes *attr);

    /** Writes window attributes to attr. */
    void windowAttributes(Window window, XWindowAttributes *attr);

    /** Returns KeyCode for string. */
    KeyCode stringToKeyCode(string key);

    /** Returns string for KeyCode. */
    string keyCodeToString(KeyCode keyCode);

    /** Returns KeySym for KeyCode. */
    KeySym keyCodeToKeySym(KeyCode keyCode);

    /** Returns name of atom. */
    string atomName(Atom atom);

    /** Writes next event back to event. */
    void nextEvent(XEvent *event);

    /** Checks for events specified by mask, returns true if matched. */
    bool checkMaskEvent(long mask, XEvent *event);

    /**
     * Translates Coordinates given by srcX and srcY absoluted to child
     * window into destX and destY coordinates absoluted to parent
     * to window.
     */
    void translateCoordinates(Window child, Window parent,
                              int srcX, int srcY, int *destX, int *destY);

    /** Returns true if color could be allocated, otherwise false. */
    bool allocNamedColor(Colormap colormap, string name, XColor *color);

    /** Installs colormap. */
    void installColormap(Colormap colormap);

    Colormap defaultColormap(unsigned int id);

    /** Queries a window tree. */
    void queryTree(Window root, Window **windows, unsigned int *num);

    /** Returns KeySym for next char by this XKeyEvent. */
    KeySym lookupNextKeySym(XKeyEvent *event, int *count, char *buffer);

    /** Writes back text property value for window and specified atom. */
    bool textProperty(Window window, Atom atom, string *value);

    /** Converts string to XTextProperty. */
    void stringListToTextProperty(string text, XTextProperty *textProperty);
    void setTextProperty(Display *display, Window window,
                         XTextProperty *textProperty, Atom atom);

    /** Default error handler. */
    static int handleErrors(Display *display, XErrorEvent *e);

    void configureWindow(Window window, unsigned int mask, XWindowChanges *wc);

    /** NumLock/ScrollLock modifier detection. */
    XModifierKeymap *modifierMapping();
    void freeModifierMapping(XModifierKeymap *modmap);

    void addToSaveSet(Window window);
    void removeFromSaveSet(Window window);

    /** Fetches size hints. */
    void sizeHints(Window window, XSizeHints *sizeHints);

    void warpPointer(Window window, int x, int y);

    void freeCursor(Cursor cursor);

    void maskEvent(unsigned long mask, XEvent *event);

    void sendEvent(Window window, unsigned long mask, XEvent *event);

    XWMHints *getWMHints(Window window);

private:

    Display *display_;

};

#endif // __XCORE_H
