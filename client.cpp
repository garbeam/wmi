// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: client.cpp 734 2004-09-27 18:15:45Z garbeam $

#include <sstream>

extern "C" {
#include <assert.h>
#include <X11/Xatom.h>
}

#include "client.h"

#include "atoms.h"
#include "binder.h"
#include "cursors.h"
#include "draw.h"
#include "clientbar.h"
#include "frame.h"
#include "kernel.h"
#include "label.h"
#include "logger.h"
#include "rectangle.h"
#include "statusbar.h"
#include "monitor.h"
#include "util.h"
#include "slot.h"
#include "theme.h"
#include "workspace.h"
#include "xcore.h"

Client::Client(Monitor *monitor, Window window, XWindowAttributes *attr)
    : Thing(monitor, new Rectangle(), Thing::CLIENT)
{
    clientWindow_ = window;
    frame_ = 0;
    state_ = WithdrawnState;
    iconName_ = "";
    isCentered_ = false;
    protocols_ = 0;
    hasFrame_ = false;
    requestsFocus_ = false;
    isDestroyed_ = false;
    hooked_ = "";
    workspace_ = 0;

    ostringstream oss;
    oss << "<" << monitor->nextClientId() << ">";
    id_ = oss.str();

    eventMask_ = PropertyChangeMask;
    XCORE->selectEvents(clientWindow_, eventMask_);

    setX(attr->x);
    setY(attr->y);
    setWidth(attr->width);
    setHeight(attr->height);
    borderWidth_ = attr->border_width;

    oss.str("");
    oss << "constructing new client with dimensions: " <<
        x() << "," << y() << "," << width() << "," << height();
    LOGDEBUG(oss.str());

    className_ = XCORE->className(clientWindow_);
    instanceName_ = XCORE->instanceName(clientWindow_);

    string value;

    updateTransient();
    if (transient_) {
         value = Util::get(KERNEL->sessionSettings(),
                     "client." + className_ + "::" + instanceName_ + ".transient-mode");

        if ("" == value) {
            value = Util::get(KERNEL->commonSettings(),
                              "default.transient-mode");

            if ("" == value) {
                value = "float";
            }
        }
    }
    else {
        value = Util::get(KERNEL->sessionSettings(),
                         "client." + className_ + "::" + instanceName_ + ".mode");
    }

    if (value == "float") {
        mode_ = FLOAT;
    }
    else if (value == "sticky") {
        mode_ = STICKY;
    }
    else if (value == "max") {
        mode_ = MAX;
    }
#ifdef SLOT_SUPPORT
    else if (value == "slot") {
        mode_ = SLOT;
    }
#endif // SLOT_SUPPORT
    // fallback, unknown yet
    else if (Util::get(KERNEL->commonSettings(), "default.client-mode")
            == "float")
    {
        mode_ = FLOAT;
    }
    else {
        mode_ = MAX;
    }

    hooked_ = Util::get(KERNEL->sessionSettings(),
            "client." + className_ + "::" + instanceName_ + ".hooked");
}

Client::~Client() {
}

string Client::id() {
    return id_;
}

void Client::initICCCM() {

    state_ = XCORE->state(clientWindow_);
    iconName_ = XCORE->atomValue(clientWindow_, XA_WM_ICON_NAME);
    setName(XCORE->atomValue(clientWindow_, XA_WM_NAME) + id_);
    if (iconName_ == "") {
        iconName_ = name();
    }
    else {
        iconName_ += id_;
    }
    updateSize();
    protocols_ = XCORE->protocols(clientWindow_);
    if (XCORE->hasDecoration(clientWindow_) && !frameWindow()) {
        hasDecoration_ = true;
    }

    updateTransient();
}

void Client::updateSize() {

    unsigned int dummy;
    // TODO: increment handling
    XCORE->updateSize(clientWindow_, &dummy, &dummy,
                      &dummy, &dummy, &isCentered_);
}

void Client::updateTransient() {

    Window trans = XCORE->transient(clientWindow_);
    transient_ = KERNEL->clientForWindow(trans);
}

Client *Client::transient() const {
    return transient_;
}

void Client::sendConfiguration() {
    XConfigureEvent event;

    ostringstream oss;

    oss << "send configuration: " << clientAreaRect_.x() << ","
        << clientAreaRect_.y() << "," << clientAreaRect_.width()
        << "," << clientAreaRect_.height();
    LOGDEBUG(oss.str());
    event.type = ConfigureNotify;
    event.event = clientWindow_;
    event.window = clientWindow_;
    if (hasFrame_) {
        event.x = x() + clientAreaRect_.x();
        event.y = y() + clientAreaRect_.y();
    }
    else if (frame()) {
        event.x = frame()->x() + clientAreaRect_.x();
        event.y = frame()->y() + clientAreaRect_.y();
    }
    else {
        event.x = clientAreaRect_.x();
        event.y = clientAreaRect_.y();
    }
    event.width = clientAreaRect_.width();
    event.height = clientAreaRect_.height();
    event.border_width = clientBorderWidth_;
    event.above = None;
    event.override_redirect = False;

    XCORE->configure(clientWindow_, &event);
}

void Client::resize() {

    clientAreaRect_.copy(this);

    if (hasFrame_) {
        ostringstream oss;
        oss << "frame resize: x=" << x() << ", y=" << y() << ", w=" <<
            width() << ", h=" << height();
        LOGDEBUG(oss.str());
        XCORE->moveResize(frameWindow(), this);

        if (frame()) {
            // maximize client window
            clientAreaRect_.setX(0);
            clientAreaRect_.setY(0);
            clientAreaRect_.setWidth(width());
            clientAreaRect_.setHeight(height());
        }
        else {
            label_->setY(borderWidth_); // label fix for toggleBorder
            fitClientArea();
        }
    }
    XCORE->moveResize(clientWindow_, &clientAreaRect_);
    sendConfiguration();
    illuminate();
}

void Client::gravitate(bool invert) {

    int dx = 0;
    int dy = 0;
    int gravity = NorthWestGravity;
    XSizeHints sizeHints;

    XCORE->sizeHints(clientWindow_, &sizeHints);

    if (sizeHints.flags & PWinGravity) {
        gravity = sizeHints.win_gravity;
    }

    switch (gravity) {
    case NorthEastGravity:
    case EastGravity:
    case SouthEastGravity:
        dx = -borderWidth_;
        break;
    default:
        break;
    }

    switch (gravity) {
    case SouthWestGravity:
    case SouthGravity:
    case SouthEastGravity:
        dy = -titleBarHeight_ - borderWidth_;
        break;
    default:
        break;
    }

    if (invert) {
        dx = -dx;
        dy = -dy;
    }

    setX(x() + dx);
    setY(y() + dy);
}

void Client::handleConfigureRequest(XConfigureRequestEvent *event) {

    XWindowChanges wc;

    if (event->value_mask & CWStackMode) {
        if (attached()) {
            if (!isFocused()) {
                requestsFocus_ = true;
            }
            if (monitor()->focused() != attached()) {
                attached()->setRequestsFocus(true);
            }
        }
        event->value_mask &= ~CWStackMode;
    }

    event->value_mask &= ~CWSibling;
    if (!frame()
#ifdef SLOT_SUPPORT
        || (mode_ == SLOT)
#endif
       ) {

        // floating client

        gravitate(true);

        if (event->value_mask & CWX) {
            setX(event->x);
        }
        if (event->value_mask & CWY) {
            setY(event->y);
        }
        if (event->value_mask & CWWidth) {
            setWidth(event->width);
        }
        if (event->value_mask & CWHeight) {
            setHeight(event->height);
        }
        if (event->value_mask & CWBorderWidth) {
            clientBorderWidth_ = event->border_width;
        }
        ostringstream oss;
        oss << "configure request: x=" << x() << ", y=" << y()
            << ", w=" << width() << ", h=" << height();
        LOGDEBUG(oss.str());

        gravitate(false);

        // applied patch by Dr. J. Pfefferl
        if (hasFrame_) {

            if((event->value_mask & CWWidth)) {
                setWidth(width() + 2 * borderWidth_);
            }
            if((event->value_mask & CWHeight)) {
                setHeight(height() + titleBarHeight_ + 2 * borderWidth_
                        + (titleBarHeight_ ? 1 : 0));
            }
            if((event->value_mask & CWX)) {
                setX(x() - borderWidth_);
            }
            if((event->value_mask & CWY)) {
                setY(y() - titleBarHeight_ - borderWidth_);
            }

            wc.x = x();
            wc.y = y();
            wc.width = width();
            wc.height = height();
            wc.border_width = 1; // event->border_width
            wc.sibling = None;
            wc.stack_mode = event->detail;
            XCORE->configureWindow(frameWindow(), event->value_mask, &wc);
            fitClientArea();
            sendConfiguration();
        }
        else {
            // save current dimensions to client area
            clientAreaRect_.copy(this);
        }
    }

#ifdef SLOT_SUPPORT
    if (mode_ == SLOT) {
        monitor()->slot()->manage();
    }
    else
#endif // SLOT_SUPPORT
    {
        // If client is attached to a frame, the clientAreaRect_ has
        // the size of the frames client area.
        wc.x = clientAreaRect_.x();
        wc.y = clientAreaRect_.y();
        wc.width = clientAreaRect_.width();
        wc.height = clientAreaRect_.height();
        wc.border_width = 0;
        wc.sibling = None;
        event->value_mask |= CWBorderWidth;

        XCORE->configureWindow(clientWindow_, event->value_mask, &wc);

        illuminate();
    }
}

void Client::handlePropertyNotify(XPropertyEvent *event) {

    if (event->state == PropertyDelete) {
        return; // ignore delete properties
    }

    // WM atoms
    if (event->atom == Atoms::WM_PROTOCOLS) {
        protocols_ = XCORE->protocols(clientWindow_);
        return;
    }
    else if (event->atom == Atoms::MWM_HINTS) {
        if (XCORE->hasDecoration(clientWindow_) && !frameWindow()) {
            hasDecoration_ = true;
        }
        return;
    }

    // default atoms
    static string tmp;
    ostringstream oss;
    switch (event->atom) {
    case XA_WM_ICON_NAME:
        tmp = XCORE->atomValue(clientWindow_, XA_WM_ICON_NAME);
        if (tmp.length() > 0) {
            // update only if the icon name is != 0
            iconName_ = tmp + id_;
        }
        monitor()->clientBar()->illuminate();
        monitor()->statusBar()->illuminate();
        break;
    case XA_WM_NAME:
        tmp = XCORE->atomValue(clientWindow_, XA_WM_NAME);
        if (tmp.length() > 0) {
            // update only if the title is != 0
            setName(tmp + id_);
        }
        illuminate();
        monitor()->statusBar()->illuminate();
        break;
    case XA_WM_TRANSIENT_FOR:
        updateTransient();
        break;
    case XA_WM_NORMAL_HINTS:
        updateSize();
        oss << "git size: x=" << x() << ", y=" << y() << ", w=" <<
            width() << ", h=" << height();
        LOGDEBUG(oss.str());
        break;
    }
}

void Client::handleUnmapNotify(XUnmapEvent *event) {

    if (!isVisible_ || !event->send_event) {
       LOGDEBUG("client is already unvisible");
       return;
    }

    LOGDEBUG("handle unmap client: " + name());
    if (frameWindow()) {
        XCORE->hide(frameWindow());
    }
    isVisible_ = false;
    XCORE->setState(clientWindow_, WithdrawnState);
    Workspace *ws = this->attached();
    if (ws) {
        LOGDEBUG("going detaching client from workspace");
        ws->detachClient(this);
    }
}

void Client::show() {
    if (isDestroyed_) {
        return;
    }
    XCORE->showRaised(clientWindow_);
    if (hasFrame_) {
        XCORE->showRaised(frameWindow());
    }
    state_ = NormalState;
    KERNEL->installCursor(Cursors::NORMAL_CURSOR, clientWindow_);
    XCORE->setState(clientWindow_, state_);
    isVisible_ = true;
}

void Client::hide() {
    if (isDestroyed_) {
        return;
    }
    if (hasFrame_) {
        XCORE->hide(frameWindow());
    }
    XCORE->hide(clientWindow_);
    state_ = WithdrawnState;
    XCORE->setState(clientWindow_, state_);
    isVisible_ = false;
    LOGDEBUG("client state changed");
    XCORE->sync();
}

void Client::setMode(Client::Mode mode) {

    mode_ = mode;
    string modeStr;

    switch (mode_) {
    case MAX:
        modeStr = "max";
        break;
    case STICKY:
        modeStr = "sticky";
        break;
    case FLOAT:
        modeStr = "float";
        break;
#ifdef SLOT_SUPPORT
    case SLOT:
        modeStr = "slot";
        break;
#endif // SLOT_SUPPORT
    }
    if (transient_) {
        (*KERNEL->sessionSettings())["client." + className_ + "::" +
                instanceName_ + ".transient-mode"] = modeStr;
    }
    else {
        (*KERNEL->sessionSettings())["client." + className_ + "::" +
                instanceName_ + ".mode"] = modeStr;
    }
}

long Client::eventMask() const {
    return eventMask_;
}

string Client::className() const {
    return className_;
}

string Client::instanceName() const {
    return instanceName_;
}

string Client::iconName() const {
    return iconName_;
}

int Client::protocols() const {
    return protocols_;
}

// TODO: It would be a good idea to implement an align method
//       for gravitation with value VH_CENTER
bool Client::isCentered() const {
    return isCentered_;
}

Client::Mode Client::mode() const {
    return mode_;
}

int Client::state() const {
    return state_;
}

Window Client::window() {
    if (hasFrame_) {
        return frameWindow_;
    }
    else {
        return clientWindow_;
    }
}

void Client::reparent(Window parentWindow, int x, int y) {
    if (isDestroyed_) {
        return;
    }
    XCORE->reparent(clientWindow_, parentWindow, x, y);
    assert(parentWindow);
    hasFrame_ = (parentWindow == frameWindow());
}

void Client::createFrame() {
    initFrameWindow();
    fitClientArea();
    XCORE->sync();
}

bool Client::isFocused() {

    if (frame()) {
        return frame()->isFocused()
               && frame()->focused() == this;
    }
    else if (attached()) {
        return attached()->topClient() == this;
    }
    return 0;
}

void Client::illuminate() {

    if (!isVisible_) {
        return;
    }

    if (frame()) {
        frame()->illuminate();
        return;
    }

    if (!hasFrame_) {
        return;
    }
    LOGDEBUG("within client illuminate");
    XCORE->sync();

    illuminateBorder();

    bool foc = isFocused();
    unsigned int buttonWidth = 0;

    if (foc) {
        buttonWidth = monitor()->buttonWidth();
    }

    if (titleBarHeight_) {
        label_->setText(name());
        label_->setX(borderWidth_);
        unsigned int textWidth = label_->adjustWidth();
        unsigned long tabShine = 0;
        unsigned long tabShadow = 0;
        label_->setWidth(width() - label_->x() - borderWidth_);
        if (foc) {
            if (areButtonsVisible_) {
                label_->setWidth(label_->width() - 3 * buttonWidth - 2);
            }
            tabShine = theme_->TAB_SHINE_ACTIVE_FOCUSSED;
            tabShadow = theme_->TAB_SHADOW_ACTIVE_FOCUSSED;
            label_->update(theme_->TAB_BACKGROUND_ACTIVE_FOCUSSED,
                           theme_->TAB_TEXT_ACTIVE_FOCUSSED,
                           tabShine, tabShadow, true, true);
        }
        else if (requestsFocus_) {
            tabShine = theme_->FOCUSREQ_SHINE;
            tabShadow = theme_->FOCUSREQ_SHADOW;
            label_->update(theme_->FOCUSREQ_BACKGROUND,
                           theme_->FOCUSREQ_TEXT,
                           tabShine, tabShadow, true, true);
        }
        else if (frame() && frame()->focused() == this) {
            tabShine = theme_->TAB_SHINE_ACTIVE_NORMAL;
            tabShadow = theme_->TAB_SHADOW_ACTIVE_NORMAL;
            label_->update(theme_->TAB_BACKGROUND_ACTIVE_NORMAL,
                           theme_->TAB_TEXT_ACTIVE_NORMAL,
                           tabShine, tabShadow, true, true);
        }
        else {
            tabShine = theme_->TAB_SHINE_INACTIVE_NORMAL;
            tabShadow = theme_->TAB_SHADOW_INACTIVE_NORMAL;
            label_->update(theme_->TAB_BACKGROUND_INACTIVE_NORMAL,
                           theme_->TAB_TEXT_INACTIVE_NORMAL,
                           tabShine, tabShadow, true, true);
        }

        if (mode_ == Client::STICKY) {

            Draw::drawStickyNotifier(frameWindow(), gc_, label_,
                    tabShine, tabShadow, textWidth);
        }
    }

    if (foc) {
        Draw::drawFloatBorderAnchors(frameWindow(), gc_, this,
                theme_->FRAME_SHINE_FOCUSSED,
                theme_->FRAME_SHADOW_FOCUSSED,
                titleBarHeight_ + borderWidth_ + 1,
                borderWidth_);
    }
}

void Client::handleButtonPress(XButtonEvent *event) {

    LOGDEBUG("entered ClientFrame::handleButtonPress");
#ifdef SLOT_SUPPORT
    if (mode_ == SLOT) {
        monitor()->slot()->focusClient(this);
        return;
    }
#endif
    if (attached() && !isFocused()) {
        attached()->focus(this);
        return;
    }
    else {
        XCORE->raise(window());
    }

    if (frame()) {
        return;
    }

    buttonState_ = NONE;

    if (event->button == Button1) {

        if (titleBarHeight_ && areButtonsVisible_ &&
                (cursor_ == Cursors::NORMAL_CURSOR))
        {
            int xPosition = event->x;
            unsigned int buttonWidth = titleBarHeight_ + 1;
            unsigned int offsetButtonGroup = width() - borderWidth_ - 3 * buttonWidth;
            if ((xPosition > (int)offsetButtonGroup) &&
                    (xPosition < (int)(width() - borderWidth_ - 1)))
            {
                xPosition -= offsetButtonGroup;
                unsigned int buttonNum = xPosition / buttonWidth;
                switch (buttonNum) {
                case 0:
                    buttonState_ = MINCLIENT;
                    break;
                case 1:
                    buttonState_ = DEMAX;
                    break;
                case 2:
                    buttonState_ = CLOSE;
                    break;
                }
                illuminate();
                return;
            }
        }

        if (monitor()->isThingMaximized()) {
            return;
        }

        Direction dir = SOUTH_EAST;
        if (cursor_ == Cursors::RESIZE_LEFT_CURSOR) {
            dir = LEFT;
        }
        else if (cursor_ == Cursors::RESIZE_RIGHT_CURSOR) {
            dir = RIGHT;
        }
        else if (cursor_ == Cursors::RESIZE_UP_CURSOR) {
            dir = UP;
        }
        else if (cursor_ == Cursors::RESIZE_DOWN_CURSOR) {
            dir = DOWN;
        }
        else if (cursor_ == Cursors::RESIZE_NORTH_WEST_CURSOR) {
            dir = NORTH_WEST;
        }
        else if (cursor_ == Cursors::RESIZE_NORTH_EAST_CURSOR) {
            dir = NORTH_EAST;
        }
        else if (cursor_ == Cursors::RESIZE_SOUTH_WEST_CURSOR) {
            dir = SOUTH_WEST;
        }
        else if (cursor_ == Cursors::RESIZE_SOUTH_EAST_CURSOR) {
            dir = SOUTH_EAST;
        }

        KERNEL->runResizeMode(this, event, dir,
                              cursor_ != Cursors::NORMAL_CURSOR);
    }
}

void Client::handleButtonRelease(XButtonEvent *event) {

    // TODO: make buttons customizable
    if (event->button == Button1) {

        if (titleBarHeight_ && (cursor_ == Cursors::NORMAL_CURSOR) &&
            (buttonState_ != NONE)) 
        {
            if (monitor()->isThingMaximized()) {
                monitor()->toggleThingMaximization();
            }
            switch (buttonState_) {
            case MINCLIENT:
                monitor()->detachClient();
                break;
            case DEMAX:
                attached()->toggleClientMode();
                break;
            case CLOSE:
                XCORE->kill(clientWindow_, protocols_);
                break;
            default:
                break;
            }
            buttonState_ = NONE;
            return;
        }
    }
}

Cursor Client::cursorForXY(int pointerX, int pointerY) {

    if (frame() || (borderWidth_ == 0))
    {
        return Cursors::NORMAL_CURSOR;
    }

    // rectangle attributes of client are used
    bool left = pointerX < clientAreaRect_.x();
    bool right = pointerX >= (int)(clientAreaRect_.x()
                                   + clientAreaRect_.width());
    bool up = pointerY < (int)(clientAreaRect_.y()
                               - titleBarHeight_ - 1);
    bool down = pointerY >= (int)(clientAreaRect_.y()
                                  + clientAreaRect_.height());

    bool tolLeft = pointerX < (int)(clientAreaRect_.x() + titleBarHeight_);
    bool tolRight = pointerX > (int)(clientAreaRect_.x()
                                     + clientAreaRect_.width()
                                     - titleBarHeight_ - 1);
    bool tolUp = pointerY < (int)(clientAreaRect_.y() + titleBarHeight_);
    bool tolDown = pointerY > (int)(clientAreaRect_.y()
                                    + clientAreaRect_.height()
                                    - titleBarHeight_ - 1);

    if ((left && up) || (left && tolUp) || (up && tolLeft)) {
        return Cursors::RESIZE_NORTH_WEST_CURSOR;
    }
    else if ((right && up) || (right && tolUp) || (up && tolRight)) {
        return Cursors::RESIZE_NORTH_EAST_CURSOR;
    }
    else if ((left && down) || (left && tolDown) ||
             (down && tolLeft))
    {
        return Cursors::RESIZE_SOUTH_WEST_CURSOR;
    }
    else if ((right && down) || (right && tolDown) || (down &&
                tolRight))
    {
        return Cursors::RESIZE_SOUTH_EAST_CURSOR;
    }

    if (left) {
        return Cursors::RESIZE_LEFT_CURSOR;
    }
    else if (right) {
        return Cursors::RESIZE_RIGHT_CURSOR;
    }
    else if (up) {
        return Cursors::RESIZE_UP_CURSOR;
    }
    else if (down) {
        return Cursors::RESIZE_DOWN_CURSOR;
    }

    return Cursors::NORMAL_CURSOR;
}

Frame *Client::frame() const {
    return frame_;
}

void Client::setFrame(Frame *frame) {
     frame_ = frame;
}

Workspace *Client::attached() const {
    return workspace_;
}

void Client::setAttached(Workspace *workspace) {
     workspace_ = workspace;
}

Window Client::frameWindow() const {
    return frameWindow_;
}

Window Client::clientWindow() const {
    return clientWindow_;
}

bool Client::requestsFocus() const {
    return requestsFocus_;
}

void Client::setRequestsFocus(bool requestsFocus) {
    requestsFocus_ = requestsFocus;
}

void Client::setHooked(string hooked) {
    hooked_ = hooked;
    if (hooked_ != "") {
        (*KERNEL->sessionSettings())["client." + className_ + "::" +
                                    instanceName_ + ".hooked"] = hooked_;
    }
    else {
        Util::remove(KERNEL->sessionSettings(),
                     "client." + className_ + "::" + instanceName_ + ".hooked");
    }
}

string Client::hooked() const {
    return hooked_;
}

void Client::setDestroyed(bool isDestroyed) {
    isDestroyed_ = isDestroyed;
}

bool Client::isDestroyed() const {
    return isDestroyed_;
}
