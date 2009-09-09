// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: frame.cpp 734 2004-09-27 18:15:45Z garbeam $

extern "C" {
#include <assert.h>
}

#include <sstream>

#include "frame.h"

#include "client.h"
#include "cursors.h"
#include "draw.h"
#include "frame.h"
#include "kernel.h"
#include "label.h"
#include "logger.h"
#include "monitor.h"
#include "split.h"
#include "theme.h"
#include "tree.h"
#include "util.h"
#include "workspace.h"
#include "xcore.h"

Frame::Frame(Workspace *workspace, Rectangle *rect)
    : Thing(workspace->attached(), rect, Thing::FRAME),
      Container<Workspace, LClient, LClient::iterator, Client>(workspace)
{
    ostringstream oss;
    oss << "<" << workspace->nextFrameId() << ">";
    setName(oss.str());
    isTiled_ = Util::get(KERNEL->commonSettings(), "frame.mode") == "tiled";
}

Frame::~Frame() {
}

bool Frame::isFocused() {
    bool result = attached() && attached()->focusedFrame() == this;
    if (focused()) {
        result = result && (focused() == attached()->topClient());
    }
    return result;
}

void Frame::focus(Client *client) {

    Container<Workspace, LClient, LClient::iterator, Client>
          ::focus(client);
    XCORE->raise(client->clientWindow());
}

void Frame::illuminate() {

    if (!isVisible()) {
        return;
    }

    illuminateBorder();

    bool foc = this->isFocused();

    if (titleBarHeight_) {

        unsigned int buttonWidth = 0;
        unsigned int labelsWidth = width() - 2 * borderWidth_;
        unsigned int offsetX = borderWidth_;
        if (foc && areButtonsVisible_) {
            buttonWidth = monitor()->buttonWidth();
            labelsWidth -= (3 * buttonWidth) - 3;
        }

        unsigned int labelWidth = size() ? labelsWidth / size() : labelsWidth;

        unsigned int i = 0;
        label_->setWidth(labelWidth);
        for (LClient::iterator it = begin(); it != end(); ) {

            Client *client = *it;
            it++;

            label_->setText(client->name());
            label_->setX(offsetX + i * labelWidth);
            if (it == end()) {
                // FIX for right button panel irretations
                label_->setWidth(
                        width() - label_->x() - 3 * buttonWidth -
                        ((foc && areButtonsVisible_) ? 3 + borderWidth_ : borderWidth_));
            }
            i++;
            if (foc) { // FOCUSSED
                if (client == focused()) {
                    label_->update(theme_->TAB_BACKGROUND_ACTIVE_FOCUSSED,
                                   theme_->TAB_TEXT_ACTIVE_FOCUSSED,
                                   theme_->TAB_SHINE_ACTIVE_FOCUSSED,
                                   theme_->TAB_SHADOW_ACTIVE_FOCUSSED,
                                   true, true);
                }
                else if (client->requestsFocus()) {
                    label_->update(theme_->FOCUSREQ_BACKGROUND,
                                   theme_->FOCUSREQ_TEXT,
                                   theme_->FOCUSREQ_SHINE,
                                   theme_->FOCUSREQ_SHADOW,
                                   true, true);
                }
                else {
                    label_->update(theme_->TAB_BACKGROUND_INACTIVE_FOCUSSED,
                                   theme_->TAB_TEXT_INACTIVE_FOCUSSED,
                                   theme_->TAB_SHINE_INACTIVE_FOCUSSED,
                                   theme_->TAB_SHADOW_INACTIVE_FOCUSSED,
                                   true, true);
                }
            }
            else { // NORMAL
                if (client == focused()) {
                    label_->update(theme_->TAB_BACKGROUND_ACTIVE_NORMAL,
                                   theme_->TAB_TEXT_ACTIVE_NORMAL,
                                   theme_->TAB_SHINE_ACTIVE_NORMAL,
                                   theme_->TAB_SHADOW_ACTIVE_NORMAL,
                                   true, true);
                }
                else if (client->requestsFocus()) {
                    label_->update(theme_->FOCUSREQ_BACKGROUND,
                                   theme_->FOCUSREQ_TEXT,
                                   theme_->FOCUSREQ_SHINE,
                                   theme_->FOCUSREQ_SHADOW,
                                   true, true);
                }
                else {
                    label_->update(theme_->TAB_BACKGROUND_INACTIVE_NORMAL,
                                   theme_->TAB_TEXT_INACTIVE_NORMAL,
                                   theme_->TAB_SHINE_INACTIVE_NORMAL,
                                   theme_->TAB_SHADOW_INACTIVE_NORMAL,
                                   true, true);
                }
            }
        }
        if (!size()) {
            label_->setText("<empty>" + name());
            if (foc) { // FOCUSSED
                label_->update(theme_->TAB_BACKGROUND_ACTIVE_FOCUSSED,
                        theme_->TAB_TEXT_ACTIVE_FOCUSSED,
                        theme_->TAB_SHINE_ACTIVE_FOCUSSED,
                        theme_->TAB_SHADOW_ACTIVE_FOCUSSED,
                        true, true);
            }
            else { // NORMAL
                label_->update(theme_->TAB_BACKGROUND_ACTIVE_NORMAL,
                        theme_->TAB_TEXT_ACTIVE_NORMAL,
                        theme_->TAB_SHINE_ACTIVE_NORMAL,
                        theme_->TAB_SHADOW_ACTIVE_NORMAL,
                        true, true);
            }
        }
    }
    if (isTiled_) {
        unsigned shine, shadow;
        for (LClient::iterator it = begin(); it != end(); it++) {
            Client *client = *it;
            if (foc && client == focused()) {
                shine = theme_->TILED_SHINE_FOCUSSED;
                shadow = theme_->TILED_SHADOW_FOCUSSED;
            }
            else {
                shine = theme_->TILED_SHINE_NORMAL;
                shadow = theme_->TILED_SHADOW_NORMAL;
            }
            Rectangle rect;
            rect.setX(client->x() - 1);
            rect.setY(client->y() - 1);
            rect.setWidth(client->width() + 2);
            rect.setHeight(client->height() + 2);
            Draw::drawRectBorder(frameWindow_, gc_, &rect, shine, shadow);
        }
    }
}

void Frame::resize() {

    label_->setY(borderWidth_);
    XCORE->clearWindow(window());
    XCORE->moveResize(window(), this);

    fitClientArea();
    if (isTiled_ && size() > 1) {
        unsigned int colWidth =
            Util::strToUInt(Util::get(KERNEL->commonSettings(), "frame.colwidth"));

        colWidth = (clientAreaRect_.width() * colWidth) / 100;
        unsigned int stackHeight = clientAreaRect_.height() / (size() - 1);
        unsigned int i = 0;

        for (LClient::iterator it = begin(); it != end(); it++) {
            Client *client = *it;
            if (it == begin()) {
                // left column
                client->copy(&clientAreaRect_);
                client->setX(clientAreaRect_.x() + 1);
                client->setY(clientAreaRect_.y() + 1);
                client->setWidth(colWidth - 2);
                client->setHeight(clientAreaRect_.height() - 2);
            }
            else {
                // right column
                client->setX(clientAreaRect_.x() + colWidth + 1);
                client->setY(i * stackHeight + clientAreaRect_.y() + 1);
                client->setHeight(stackHeight - 2);
                client->setWidth(clientAreaRect_.width() - colWidth - 2);
                i++;
            }
            client->resize();
        }
    }
    else {
        for (LClient::iterator it = begin(); it != end(); it++) {
            Client *client = *it;
            matchClientSize(client);
        }
    }
    illuminate();
}

void Frame::matchClientSize(Client *client)
{
    client->copy(&clientAreaRect_);
    client->resize();
}

void Frame::attach(Client *client) {

    if (client->frame() == this) {
        LOGWARN("frame already attached.");
        return;
    }
    Container<Workspace, LClient, LClient::iterator, Client>
        ::attach(client);
    client->setFrame(this);
    client->prevRectangle()->copy(client);
    client->reparent(window(), clientAreaRect_.x(), clientAreaRect_.y());
    XCORE->sync();
    if (isVisible()) {
        client->show();
    }
    childs()->remove(client);
    childs()->push_front(client);
    resize();
    LOGDEBUG("attached client to frame");
}

Client *Frame::detach(Client *client) {

    assert(size() > 0);
    XCORE->sync();
    LOGDEBUG("detaching clientframe");
    Client *next = Container<Workspace, LClient, LClient::iterator, Client>
                    ::detach(client);
    if (client->isVisible()) {
        client->hide();
    }
    if (size() > 0) {
        assert(next);
    }
    client->reparent(monitor()->rootWindow(), client->x(), client->y());
    client->copy(client->prevRectangle());
    client->setFrame(0);
    if (next) {
        resize();
    }
    return next;
}

void Frame::handleButtonPress(XButtonEvent *event) {

    // TODO: make buttons customizable
    if (event->button == Button1) {

        if (!isFocused()) {
            attached()->focus(focused());
            return;
        }
        else {
            XCORE->raise(window());
        }

        buttonState_ = NONE;
        if (titleBarHeight_ && (cursor_ == Cursors::NORMAL_CURSOR) && size()) {
            LOGDEBUG("normal cursor / switching client");
            int xPosition = event->x;
            unsigned int buttonWidth = titleBarHeight_ + 1;
            if (areButtonsVisible_) {
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

            // handle label click
            xPosition -= borderWidth_;
            unsigned int labelsWidth = width() - 2 * borderWidth_
                - (areButtonsVisible_ ? (3 * buttonWidth) - 2 : 0);
            unsigned int labelWidth = labelsWidth / size();

            unsigned int clientNum = xPosition / labelWidth;
            unsigned int i = 0;

            for (LClient::iterator it = begin(); it != end(); it++) {

                if (i == clientNum) {
                    attached()->focus(*it);
                    return;
                }
                i++;
            }
        }

        if (monitor()->isThingMaximized()) {
            return;
        }

        Direction dir = DOWN;
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

        // Patch applied by Christof Musik
        if (cursor_ != Cursors::NORMAL_CURSOR) {

            if (Split::neighbor(attached()->root(), tree_, dir)) {
                Kernel::instance()->runResizeMode(this, event, dir);
            }
            else if (event->button == Button4) {
                monitor()->focused()->cycleClientPrev();
            } else if (event->button == Button5) {
                monitor()->focused()->cycleClientNext();
            }
        }
    }
}

void Frame::handleButtonRelease(XButtonEvent *event) {

    if (titleBarHeight_ && (event->button == Button1))
    {
        if ((buttonState_ != NONE) && monitor()->isThingMaximized()) {
            monitor()->toggleThingMaximization();
        }
        InvertButton state = buttonState_;
        buttonState_ = NONE;
        switch (state) {
        case MINCLIENT:
            monitor()->detachClient();
            break;
        case DEMAX:
            attached()->toggleClientMode();
            break;
        case CLOSE:
            XCORE->kill(focused()->clientWindow(), focused()->protocols());
            break;
        case NONE:
            break;
        }
    }
 
}

Cursor Frame::cursorForXY(int pointerX, int pointerY) {

    if (borderWidth_ == 0) {
        return Cursors::NORMAL_CURSOR;
    }
    bool left = pointerX < clientAreaRect_.x();
    bool right = pointerX >=
        (int)(clientAreaRect_.x() + clientAreaRect_.width());
    bool up;
    if (titleBarHeight_) {
        up = pointerY <
            (int)(clientAreaRect_.y() - titleBarHeight_ - 1);
    }
    else {
        up = pointerY < clientAreaRect_.y();
    }
    bool down = pointerY >=
        (int)(clientAreaRect_.y() + clientAreaRect_.height());

    if (left && Split::neighbor(attached()->root(), tree_, LEFT)) {
        return Cursors::RESIZE_LEFT_CURSOR;
    }
    else if (right && Split::neighbor(attached()->root(), tree_, RIGHT)) {
        return Cursors::RESIZE_RIGHT_CURSOR;
    }
    else if (up && Split::neighbor(attached()->root(), tree_, UP)) {
        return Cursors::RESIZE_UP_CURSOR;
    }
    else if (down && Split::neighbor(attached()->root(), tree_, DOWN)) {
        return Cursors::RESIZE_DOWN_CURSOR;
    }

    return Cursors::NORMAL_CURSOR;
}

Tree *Frame::tree() const {
    return tree_;
}

void Frame::setTree(Tree *t) {
    tree_ = t;
}

Window Frame::window() {
    return frameWindow_;
}

void Frame::toggleTiled() {
    isTiled_ = !isTiled_;
    resize();
}

void Frame::zoomClient() {
    if (focused() && size() > 1) {
        if (focused() == *begin()) {
            attached()->focus(next());
        }
        Client *client = focused();
        childs()->remove(client);
        childs()->push_front(client);
        resize();
    }
}

