// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "xcore.h"

extern "C" {
#include <X11/Xatom.h>
#include <X11/Xproto.h>
}

#include <sstream>

#include "atoms.h"
#include "font.h"
#include "kernel.h"
#include "logger.h"
#include "rectangle.h"

#include "xfont.h"
#include "xftfont.h"

XCore::XCore() {}

XCore::~XCore() {}

void XCore::setDisplay(Display *display) {
    display_ = display;
}

Window XCore::createWindow(Window root, XSetWindowAttributes *attr,
                           int x, int y, unsigned int w, unsigned int h,
                           unsigned long mask)
{
    return XCreateWindow(display_, root, x, y, w, h, 0,
                         CopyFromParent, InputOutput, CopyFromParent,
                         mask, attr);
}

void XCore::sendEvent(Window window, unsigned long mask, XEvent *event) {
    XSendEvent(display_, window, True, mask, event);
}

void XCore::maskEvent(unsigned long mask, XEvent *event) {
    XMaskEvent(display_, mask, event);
}

void XCore::selectEvents(Window window, long mask) {
    XSelectInput(display_, window, mask);
}

void XCore::show(Window window) {
    XMapWindow(display_, window);
}

void XCore::showRaised(Window window) {
    XMapRaised(display_, window);
}

void XCore::hide(Window window) {
    XUnmapWindow(display_, window);
}

GC XCore::createGC(Window window, unsigned long mask, XGCValues *gcv) {
    return XCreateGC(display_, window, mask, gcv); 
}

void XCore::ungrabKey(Window window, unsigned int modMask, KeyCode keycode)
{
    XUngrabKey(display_, keycode, modMask, window);
}

void XCore::grabKeyboard(Window window) {
    XGrabKeyboard(display_, window, True, GrabModeAsync,
                  GrabModeAsync, CurrentTime);
}

void XCore::ungrabKeyboard() {
    XUngrabKeyboard(display_, CurrentTime);
}

void XCore::free(void *data) {
    XFree(data);
}

void XCore::freeCursor(Cursor cursor) {
    XFreeCursor(display_, cursor);
}

void XCore::sync(bool discard) {
    XSync(display_, discard);
}

void XCore::setForeground(GC gc, unsigned long color) {
    XSetForeground(display_, gc, color);
}

void XCore::drawLines(Window window, GC gc, XPoint *points, unsigned int num)
{
    XDrawLines(display_, window, gc, points, num,
               CoordModeOrigin);
}

void XCore::drawRectangle(Window window, GC gc, int x, int y,
                          unsigned int w, unsigned int h)
{
    XDrawRectangle(display_, window, gc, x, y, w, h);
}

void XCore::drawRectangle(Window window, GC gc, Rectangle *rect) {
    XDrawRectangle(display_, window, gc, rect->x(), rect->y(),
                    rect->width(), rect->height());
}

void XCore::drawLine(Window window, GC gc, unsigned int x1, unsigned int y1,
                     unsigned int x2, unsigned int y2)
{
    XDrawLine(display_, window, gc, x1, y1, x2, y2);
}

void XCore::clearArea(Window window, Rectangle *rect) {
    XClearArea(display_, window, rect->x(), rect->y(),
               rect->width(), rect->height(), False);
}

void XCore::clearArea(Window window, int x, int y, unsigned int w, unsigned int h) {
    XClearArea(display_, window, x, y, w, h, False);
}

void XCore::clearWindow(Window window) {
    XClearWindow(display_, window);
}

void XCore::fillRectangle(Window window, GC gc, Rectangle *rect) {
    XFillRectangle(display_, window, gc, rect->x(), rect->y(),
                   rect->width(), rect->height());
}

void XCore::fillRectangle(Window window, GC gc, int x, int y,
                          unsigned int w, unsigned int h)
{
    XFillRectangle(display_, window, gc, x, y, w, h);
}

XFontStruct *XCore::loadQueryFont(string name) {
    return XLoadQueryFont(display_, name.c_str());
}

void XCore::drawText(Window window, GC gc, int x, int y,
                       string text)
{
    XDrawString(display_, window, gc, x, y,
                text.c_str(), text.length());
}

int XCore::textWidth(XFontStruct *font, string text) {
    return XTextWidth(font, text.c_str(), text.length());
}

#ifdef XFT_SUPPORT
XftFont *XCore::xftOpenFontName(unsigned int screen, string name) {
    return XftFontOpenName(display_, screen, name.c_str());
}

void XCore::xftCloseFont(XftFont *font) {
    if (font != 0) {
        XftFontClose(display_, font);
    }
}

int XCore::textWidth(XftFont *font, string text) {

    if (font == 0) {
        return 0;
    }
    XGlyphInfo ginfo;
    XftTextExtents8(display_, font, (XftChar8 *)text.c_str(),
                    text.length(), &ginfo);
    return ginfo.xOff;
}

void XCore::drawText(XftFont *font, Window window, GC gc,
                     unsigned int screen, int x, int y, string text)
{
    if (font == 0) {
        return;
    }

    Visual *visual = DefaultVisual(display_, screen);
    Colormap cmap = DefaultColormap(display_, screen);

    XftDraw *draw = XftDrawCreate(display_, window, visual, cmap);

    XGCValues gcv;

    // foreground pixel for XRenderColor value
    XGetGCValues(display_, gc, GCForeground, &gcv);

    // RGB values
    XColor color;
    color.pixel = gcv.foreground;
    XQueryColor(display_, cmap, &color);

    XRenderColor renderColor;
    renderColor.red = color.red;
    renderColor.green = color.green;
    renderColor.blue = color.blue;
    renderColor.alpha = 0xFFFF;
    XftColor xftColor;  
    XftColorAllocValue(display_, visual, cmap, &renderColor, &xftColor);

    // draw text
    XftDrawString8(draw, &xftColor, font, x, y,
                   (XftChar8 *)(text.c_str()), text.length());

    // clean up
    XftColorFree(display_, visual, cmap, &xftColor);
    XftDrawDestroy(draw);
}

#endif // XFT_SUPPORT

string XCore::className(Window window) {
    XClassHint classHints;

    if (XGetClassHint(display_, window, &classHints) != 0) {
        return (string)classHints.res_class;
    }
    return "";
}

string XCore::instanceName(Window window) {
    XClassHint classHints;

    if (XGetClassHint(display_, window, &classHints) != 0) {
        return (string)classHints.res_name;
    }
    return "";
}

int XCore::state(Window window) {

    // state hints
    XWMHints *hints = XGetWMHints(display_, window);
    int result;

    long *prop = 0;
    if (property(window, Atoms::WM_STATE, Atoms::WM_STATE,
                2L, (unsigned char**)&prop) > 0)
    {
        result = (int)*prop;
        free((long *)prop);
    }
    else {
        result = hints ? hints->initial_state : NormalState;
    }

    if (hints) {
        free(hints);
    }

    return result;
}

void XCore::setState(Window window, int state) {

    long data[2];
    data[0] = (long)state;
    data[1] = (long)None;

    XChangeProperty(display_, window,
            Atoms::WM_STATE, Atoms::WM_STATE,
            32, PropModeReplace, (unsigned char *)data, 2);
}

string XCore::atomValue(Window window, Atom atom) {

    string result;
    unsigned char *prop;

    if (property(window, atom, XA_STRING, 100L, &prop) > 0) {
        result = (char *)prop;
        free((char *)prop);
    }
    else {
        result = "";
    }

    return result;
}

void XCore::sizeHints(Window window, XSizeHints *sizeHints) {
    long msize;
    // size hints
    XGetWMNormalHints(display_, window, sizeHints, &msize);
}

void XCore::updateSize(Window window,
                       unsigned int *minW, unsigned int *minH,
                       unsigned int *maxW, unsigned int *maxH,
                       bool *isCentered)
{
    XSizeHints sizeHints;
    long msize;

    // size hints
    if (!XGetWMNormalHints(display_, window, &sizeHints, &msize)
            || !sizeHints.flags)
    {
        sizeHints.flags = PSize;
    }

    *isCentered = !((sizeHints.flags & USPosition) ||
                    (sizeHints.flags & PPosition));

    // minimal dimensions 
    if (sizeHints.flags & PBaseSize) {
        *minW = sizeHints.base_width;
        *minH = sizeHints.base_height;
    }
    else if (sizeHints.flags & PMinSize) {
        *minW = sizeHints.min_width;
        *minH = sizeHints.min_height;
    }
    else {
        *minW = *minH = 23;
    }

    // maximal dimensions 
    if (sizeHints.flags & PMaxSize) {
        *maxW = sizeHints.max_width;
        *maxH = sizeHints.max_height;
    }
}

bool XCore::hasDecoration(Window window) {

    MWMHints *mwmHints = 0;
    long result = property(window, Atoms::MWM_HINTS,
                           Atoms::MWM_HINTS, 5, (unsigned char **)
                           &mwmHints);

    if ((result >= 0) && mwmHints) {
        bool decor = !(mwmHints->decorations == 0 &&
                       (mwmHints->flags & MWM_HINTS_DECORATIONS));
        free((void *)mwmHints);
        return decor;
    }
    // fallback, assumes decoration
    return true;
}

Window XCore::transient(Window window) {
    Window trans;

    if (!XGetTransientForHint(display_, window, &trans)) {
        return 0;
    }
    return trans;
}

int XCore::property(Window window, Atom atom, Atom type,
                    long length, unsigned char **prop)
{
    Atom realType;
    int format;
    unsigned long result, extra;
    int status;

    status = XGetWindowProperty(display_, window,
            atom, 0L, length,
            False, type, &realType, &format,
            &result,
            &extra,
            prop);

    if (status != Success || *prop == 0) {
        return 0;
    }
    if (result == 0) {
        free((void*) *prop);
    }

    return result;
}

void XCore::configureWindow(Window window, unsigned int mask, XWindowChanges *wc) {

    XConfigureWindow(display_, window, mask, wc);
}

void XCore::configure(Window window, XConfigureEvent *event) {
    XSendEvent(display_, window, False, StructureNotifyMask,
               (XEvent *)event);
    XFlush(display_);
}

int XCore::protocols(Window window) {
    Atom *protocols;
    long result;
    int protos = 0;

    result = property(window, Atoms::WM_PROTOCOLS, XA_ATOM,
                      20L, (unsigned char **) &protocols);
    if (result <= 0) {
        return protos;
    }

    for (int i = 0; i < result; i++) {
        if (protocols[i] == Atoms::WM_DELETE) {
            protos |= PDELETE;
        }
        else if (protocols[i] == Atoms::WM_TAKE_FOCUS) {
            protos |= PTAKEFOCUS;
        }
    }
    free((char *) protocols);
    return protos;
}

void XCore::sendMessage(Window window, Atom atom, long value) {

    XEvent event;
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = atom;
    event.xclient.format = 32;
    event.xclient.data.l[0] = value;
    event.xclient.data.l[1] = CurrentTime;

    XSendEvent(display_, window, False, NoEventMask, &event);
    XFlush(display_);
}

void XCore::kill(Window window, int protos) {

    if (protos & PDELETE) {
        sendMessage(window, Atoms::WM_PROTOCOLS, Atoms::WM_DELETE);
    }
    else {
        XKillClient(display_, window);
    }
}

void XCore::moveResize(Window window, Rectangle *rect) {
    XMoveResizeWindow(display_, window, rect->x(), rect->y(),
                      rect->width(), rect->height());
}

void XCore::destroy(Window window) {
    XDestroyWindow(display_, window);
}

void XCore::raise(Window window) {
    XRaiseWindow(display_, window);
}

void XCore::lower(Window window) {
    XLowerWindow(display_, window);
}

void XCore::reparent(Window child, Window parent, int x, int y) {
    XReparentWindow(display_, child, parent, x, y);
}

void XCore::setInputFocus(Window window) {
    XSetInputFocus(display_, window, RevertToPointerRoot, CurrentTime);
}

void XCore::grabKey(Window window, unsigned int modMask, KeyCode keycode) {
    XGrabKey(display_, keycode, modMask, window, True, GrabModeAsync,
             GrabModeAsync);
}

void XCore::grabButton(Window window, unsigned int modMask, unsigned int button) {
    XGrabButton(display_, button, modMask, window, False,
                ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
                GrabModeAsync, None, None);
}

void XCore::ungrabButton(Window window, unsigned int modMask, unsigned int button) {
    XUngrabButton(display_, button, modMask, window);
}

void XCore::setWindowAttributes(Window window, unsigned int mask,
                                XSetWindowAttributes *attr)
{
    XChangeWindowAttributes(display_, window, mask, attr);
}

void XCore::windowAttributes(Window window, XWindowAttributes *attr)
{

    XGetWindowAttributes(display_, window, attr);
}

Atom XCore::internAtom(string atom) {
    return XInternAtom(display_, atom.c_str(), False);
}


Cursor XCore::createFontCursor(unsigned int cursor) {
    return XCreateFontCursor(display_, cursor);
}


string XCore::atomName(Atom atom) {
    return (string)XGetAtomName(display_, atom);
}


void XCore::nextEvent(XEvent *event) {
    XNextEvent(display_, event);
}

bool XCore::checkMaskEvent(long mask, XEvent *event) {
    return XCheckMaskEvent(display_, mask, event);
}

KeyCode XCore::stringToKeyCode(string key) {
    KeySym keySym = XStringToKeysym(key.c_str());
    if (keySym) {
        return XKeysymToKeycode(display_, keySym);
    }
    return 0;
}

KeySym XCore::keyCodeToKeySym(KeyCode keyCode) {
    return XKeycodeToKeysym(display_, keyCode, 0);
}

string XCore::keyCodeToString(KeyCode keyCode) {
    KeySym keySym = XKeycodeToKeysym(display_, keyCode, 0);
    if (keySym) {
        return XKeysymToString(XKeycodeToKeysym(display_, keyCode, 0));
    }
    return "";
}

void XCore::grabServer() {
    XGrabServer(display_);
}

void XCore::ungrabServer() {
    XUngrabServer(display_);
}

void XCore::grabPointer(Window window, unsigned int mask,
                        Time time, Cursor cursor)
{

    XGrabPointer(display_, window, False, mask,
                        GrabModeAsync, GrabModeAsync,
                        None, cursor, time);
}

void XCore::ungrabPointer(Time time) {
    XUngrabPointer(display_, time);
}

void XCore::translateCoordinates(Window child, Window parent,
                                 int srcX, int srcY, int *destX, int *destY)
{

    Window dummy;
    XTranslateCoordinates(display_, child, parent, srcX, srcY,
                          destX, destY, &dummy);
}

bool XCore::allocNamedColor(Colormap colormap, string name, XColor *color) {
    return XAllocNamedColor(display_, colormap, name.c_str(),
                            color, color);
}

void XCore::installColormap(Colormap colormap) {
    XInstallColormap(display_, colormap);
}

KeySym XCore::lookupNextKeySym(XKeyEvent *event, int *count, char *buffer) {
    KeySym result;
    *count = XLookupString(event, buffer, 32, &result, 0);
    return result;
}

bool XCore::textProperty(Window window, Atom atom, string *value) {
    XTextProperty property;

    if (XGetTextProperty(display_, window, &property, atom)) {

        if (property.nitems > 0) {
            *value = (const char *)property.value;
        }
        else {
            *value = "";
        }
        free(property.value);
        return true;
    }
    return false;
}

void XCore::queryTree(Window root, Window **windows, unsigned int *num) {

    Window rootReturn, parentReturn;

    XQueryTree(display_, root, &rootReturn,
               &parentReturn, windows, num);
}

// X error manager
int XCore::handleErrors(Display *display, XErrorEvent *e)
{
    Logger *log = Logger::instance();
    Kernel *kernel = Kernel::instance();

    if ((kernel->runlevel() == Kernel::START) &&
        (e->request_code == X_ChangeWindowAttributes) &&
        (e->error_code == BadAccess))
    {
        log->error("seems, that there's running another window manager", true);
        // exits wmi
    }

    char errtxt[128];

    XGetErrorText(kernel->display(), e->error_code, errtxt, 128);
    ostringstream oss;
    oss << errtxt << "(" << (int)e->error_code << ") opcodes "
        << (int)e->request_code << "/" << (int)e->minor_code 
        << " resource 0x"<< hex << (int)e->resourceid;
    log->warning(oss.str());

#ifdef DEBUG
    XTextProperty property;

    char *text = (char *)oss.str().c_str();
    XStringListToTextProperty(&text, 1, &property);
    XSetTextProperty(kernel->display(), kernel->defaultRootWindow(), &property,
                     Atoms::WMI_STATUSTEXT);
#endif

    return 0;
}

void XCore::setTextProperty(Display *display, Window window,
                            XTextProperty *textProperty, Atom atom)
{
    XSetTextProperty(display, window, textProperty, atom);
}

void XCore::stringListToTextProperty(string text, XTextProperty *textProperty)
{
    char *txt = (char *)text.c_str();
    XStringListToTextProperty(&txt, 1, textProperty);
}

Colormap XCore::defaultColormap(unsigned int id) {
    return DefaultColormap(display_, id);
}

XModifierKeymap *XCore::modifierMapping() {
    return XGetModifierMapping(display_);
}

void XCore::freeModifierMapping(XModifierKeymap *modmap) {
    if (modmap) {
        XFreeModifiermap(modmap);
    }
}

void XCore::addToSaveSet(Window window) {
    XAddToSaveSet(display_, window);
}

void XCore::removeFromSaveSet(Window window) {
    XRemoveFromSaveSet(display_, window);
}

void XCore::warpPointer(Window window, int x, int y) {
    XWarpPointer(display_, None, window, 0, 0, 0, 0, x, y);
}

XWMHints *XCore::getWMHints(Window window) {
    return XGetWMHints(display_, window);
}
