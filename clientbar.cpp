// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include <list>
#include <string>
#include <sstream>

#include "clientbar.h"

#include "client.h"
#include "cursors.h"
#include "draw.h"
#include "frame.h"
#include "label.h"
#include "logger.h"
#include "menu.h"
#include "monitor.h"
#include "theme.h"
#include "thing.h"
#include "validators.h"
#include "workspace.h"
#include "xcore.h"

ClientBar::ClientBar(Monitor *monitor, Rectangle *rect)
    : Bar(monitor, rect)
{
    isModeButtonPressed_ = isMenuButtonPressed_ = false;
    mode_ = PAGER;
    menu_ = new Menu(monitor);
}

ClientBar::~ClientBar() {
}

void ClientBar::illuminate() {

    if (!isVisible()) {
        return;
    }

    LOGDEBUG("going to draw border");
    drawBorder();

    label_->setText("");
    label_->setX(1);
    label_->setWidth(width() - 2);
    label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                   theme_->BAR_SHINE, theme_->BAR_SHADOW);

    updateMenuButton();
    if (mode_ == PAGER) {
        updateClientPager();
    }
    else {
        updateClientInfo();
    }
    updateModeButton();
}


void ClientBar::updateClientPager() {

    Workspace *workspace = monitor()->focused();
    CClient *clients = workspace->floatingClients();
    if (clients->size() > 0) {
        unsigned int offsetX = isButtonVisible_ ? monitor()->buttonWidth() + 3 : 1;
        unsigned int clientsWidth = width() - 2;
        if (isButtonVisible_) {
           clientsWidth -= (2 * monitor()->buttonWidth() + 4);
        }
        unsigned int i = 0;
        unsigned int clientWidth = clientsWidth / clients->size();
        label_->setWidth(clientWidth);
        for (LClient::iterator it = clients->begin(); it != clients->end(); ) {
            Client *client = *it;
            it++;

            label_->setX(offsetX + i * clientWidth);
            if (it == clients->end()) {
                // FIX for right button irretations
                label_->setWidth(width() - label_->x()
                        - (isButtonVisible_ ? monitor()->buttonWidth() + 3 : 1));
            }
            unsigned long shine, shadow;
            label_->setText(client->name());
            if (client->isFocused()) {
                shine = theme_->TAB_SHINE_ACTIVE_FOCUSSED;
                shadow = theme_->TAB_SHADOW_ACTIVE_FOCUSSED;
                label_->update(theme_->TAB_BACKGROUND_ACTIVE_FOCUSSED,
                        theme_->TAB_TEXT_ACTIVE_FOCUSSED,
                        shine, shadow, true, true);
            }
            else if (client->requestsFocus()) {
                shine = theme_->FOCUSREQ_SHINE;
                shadow = theme_->FOCUSREQ_SHADOW;
                label_->update(theme_->FOCUSREQ_BACKGROUND,
                               theme_->FOCUSREQ_TEXT,
                               shine, shadow, true, true);
            }
            else {
                shine = theme_->TAB_SHINE_ACTIVE_NORMAL;
                shadow = theme_->TAB_SHADOW_ACTIVE_NORMAL;
                label_->update(theme_->TAB_BACKGROUND_ACTIVE_NORMAL,
                        theme_->TAB_TEXT_ACTIVE_NORMAL,
                        shine, shadow, true, true);
            }

            if (client->mode() == Client::STICKY) {
                Draw::drawStickyNotifier(window(), gc(), label_,
                                         shine, shadow,
                                         label_->textWidth());
            }
            i++;
        }
    }
}

void ClientBar::updateMenuButton() {

    // button stuff
    if (!isButtonVisible_) {
        return;
    }

    label_->setText("");
    label_->setX(1);
    label_->setWidth(monitor()->buttonWidth());
    if (isMenuButtonPressed_) {
        Draw::drawMenuButton(window(), gc(), label_,
                theme_->BUTTON_BACKGROUND_PRESSED,
                theme_->BUTTON_SHINE_BORDER_PRESSED,
                theme_->BUTTON_SHADOW_BORDER_PRESSED,
                theme_->BUTTON_SHINE_FIGURE_PRESSED,
                theme_->BUTTON_SHADOW_FIGURE_PRESSED);
    }
    else {
        Draw::drawMenuButton(window(), gc(), label_,
                theme_->BUTTON_BACKGROUND_NORMAL,
                theme_->BUTTON_SHINE_BORDER_NORMAL,
                theme_->BUTTON_SHADOW_BORDER_NORMAL,
                theme_->BUTTON_SHINE_FIGURE_NORMAL,
                theme_->BUTTON_SHADOW_FIGURE_NORMAL);
    }
}

void ClientBar::updateModeButton() {

    // button stuff
    if (!isButtonVisible_) {
        return;
    }

    Workspace *workspace = monitor()->focused();

    label_->setText("");
    label_->setX(width() - monitor()->buttonWidth() - 1);
    label_->setWidth(monitor()->buttonWidth());
    if (isModeButtonPressed_) {
        if (workspace->isFrameMode()) {
            Draw::drawFloatButton(window(), gc(), label_,
                    theme_->BUTTON_BACKGROUND_PRESSED,
                    theme_->BUTTON_SHINE_BORDER_PRESSED,
                    theme_->BUTTON_SHADOW_BORDER_PRESSED,
                    theme_->BUTTON_SHINE_FIGURE_PRESSED,
                    theme_->BUTTON_SHADOW_FIGURE_PRESSED,
                    workspace->floatingClients()->size());
        }
        else {
            Draw::drawMaxButton(window(), gc(), label_,
                    theme_->BUTTON_BACKGROUND_PRESSED,
                    theme_->BUTTON_SHINE_BORDER_PRESSED,
                    theme_->BUTTON_SHADOW_BORDER_PRESSED,
                    theme_->BUTTON_SHINE_FIGURE_PRESSED,
                    theme_->BUTTON_SHADOW_FIGURE_PRESSED,
                    workspace->focusedFrame() &&
                    workspace->focusedFrame()->size());
        }
    }
    else {
        if (workspace->isFrameMode()) {
            Draw::drawFloatButton(window(), gc(), label_,
                    theme_->BUTTON_BACKGROUND_NORMAL,
                    theme_->BUTTON_SHINE_BORDER_NORMAL,
                    theme_->BUTTON_SHADOW_BORDER_NORMAL,
                    theme_->BUTTON_SHINE_FIGURE_NORMAL,
                    theme_->BUTTON_SHADOW_FIGURE_NORMAL,
                    workspace->floatingClients()->size());
        }
        else {
            Draw::drawMaxButton(window(), gc(), label_,
                    theme_->BUTTON_BACKGROUND_NORMAL,
                    theme_->BUTTON_SHINE_BORDER_NORMAL,
                    theme_->BUTTON_SHADOW_BORDER_NORMAL,
                    theme_->BUTTON_SHINE_FIGURE_NORMAL,
                    theme_->BUTTON_SHADOW_FIGURE_NORMAL,
                    workspace->focusedFrame() &&
                    workspace->focusedFrame()->size());
        }
    }
}

void ClientBar::updateClientInfo() {

    // client info stuff
    Workspace *focusedWorkspace = monitor()->focused();
    Client *client = focusedWorkspace->topClient();
    unsigned int offsetX = isButtonVisible_ ? monitor()->buttonWidth() + 3 : 1;
    unsigned int clientInfoWidth = width() - 2;
    if (isButtonVisible_) {
        clientInfoWidth -= (2 * monitor()->buttonWidth() + 4);
    }
    label_->setX(offsetX);
    label_->setWidth(clientInfoWidth);
    label_->setText(client ? client->name() : "");
    label_->update(theme_->LABEL_BACKGROUND_FOCUSSED,
            theme_->LABEL_TEXT_FOCUSSED,
            theme_->LABEL_SHINE_FOCUSSED,
            theme_->LABEL_SHADOW_FOCUSSED,
            true, true);
    if (client->mode() == Client::STICKY) {
        Draw::drawStickyNotifier(window(), gc(), label_,
                                 theme_->LABEL_SHINE_FOCUSSED,
                                 theme_->LABEL_SHADOW_FOCUSSED,
                                 label_->textWidth());
    }
}

void ClientBar::handleButtonRelease(XButtonEvent *event) {

    if ((event->window != window())) {
        return;
    }

    if (isModeButtonPressed_) {
        isModeButtonPressed_ = false;
        Workspace *ws = monitor()->focused();
        if ((event->button == Button1) &&
            Validators::instance()->validateToggleMode())
        {
            ws->toggleMode();
        }
        illuminate();
    }
    else if (isMenuButtonPressed_) {
        toggleMenuMode();
    }
}

void ClientBar::toggleMenuMode() {

    if (menu_->isVisible()) {
        menu_->hide();
        isMenuButtonPressed_ = false;
    }
    else {
        menu_->show();
    }
    illuminate();
}

void ClientBar::handleButtonPress(XButtonEvent *event) {

    if ((event->window != window())) {
        return;
    }

    XCORE->raise(window());

    // TODO: make buttons customizable
    if (event->button == Button1) {
        invokeClickedThing(event->x);
    }
    else if (event->button == Button4) {
        monitor()->focused()->cycleClientPrev();
    }
    else if (event->button == Button5) {
        monitor()->focused()->cycleClientNext();
    }
}

void ClientBar::handleMotionNotify(XMotionEvent *event) {

    if (isModeButtonPressed_) {
        isModeButtonPressed_ = false;
        illuminate();
    }
}

void ClientBar::invokeClickedThing(int xPosition) {

    if (isButtonVisible_) {
        if (xPosition < (int)(monitor()->buttonWidth() + 1)) {
            isMenuButtonPressed_ = true;
        }
        else if (xPosition > (int)(width() - monitor()->buttonWidth() - 2)) {
            isModeButtonPressed_ = true;
        }
        illuminate();
        if (isModeButtonPressed_ || isMenuButtonPressed_) {
            return;
        }
    }

    if (mode_ != PAGER) {
        illuminate();
        return;
    }

    Workspace *workspace = monitor()->focused();
    unsigned int clientsWidth = width() - 2;
    if (isButtonVisible_) {
        clientsWidth -= (2 * monitor()->buttonWidth() + 4);
        xPosition -= (monitor()->buttonWidth() + 3);
    }
    else {
        xPosition -= 1;
    }

    CClient *clients = workspace->floatingClients();
    if (!clients->size()) {
        return;
    }

    unsigned int clientNum = xPosition / (clientsWidth / clients->size());
    unsigned i = 0;
    for (LClient::iterator it = clients->begin(); it != clients->end(); it++) {
        if (i == clientNum) {
            workspace->focus(*it);
            illuminate();
            return;
        }
        i++;
    }
}

Menu *ClientBar::menu() const {
    return menu_;
}

void ClientBar::setMode(ClientBar::Mode mode) {
    mode_ = mode;
}

ClientBar::Mode ClientBar::mode() const {
    return mode_;
}
