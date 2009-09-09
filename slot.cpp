// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "slot.h"

#ifdef SLOT_SUPPORT

#include <map>
#include <string>
#include <sstream>

#include "binder.h"
#include "client.h"
#include "cursors.h"
#include "draw.h"
#include "kernel.h"
#include "label.h"
#include "logger.h"
#include "monitor.h"
#include "theme.h"
#include "util.h"
#include "workspace.h"
#include "xcore.h"
#include "xfont.h"

/// slot tab
Tab::Tab(const string name) {
    name_ = name;
    focused_ = 0;
}

Tab::~Tab() {
}

string Tab::name() const {
    return name_;
}

LClient *Tab::clients() {
    return &clients_;
}

void Tab::addClient(Client *client) {
    focused_ = client;
    clients_.push_back(client);
}

void Tab::removeClient(Client *client) {
    clients_.remove(client);
    if (focused_ == client) {
        if (clients_.size()) {
            focused_ = clients_.front();
        }
        else {
            focused_ = 0;
        }
    }
}

bool Tab::isVisible() const {
    return isVisible_;
}

void Tab::show() {
    isVisible_ = true;
    for (LClient::iterator it = clients_.begin();
            it != clients_.end(); it++)
    {
        Client *client = *it;
        client->show();
    }
}

void Tab::hide() {
    isVisible_ = false;
    for (LClient::iterator it = clients_.begin();
            it != clients_.end(); it++)
    {
        Client *client = *it;
        client->hide();
    }
}

Client *Tab::focused() const {
    return focused_;
}

void Tab::focus(Client *client) {
    focused_ = client;
}

/// Slot implementation

Slot::Slot(Monitor *monitor, Rectangle *rect, Direction align)
    : Widget(monitor, rect),
      Container<Monitor, LTab, LTab::iterator, Tab>(monitor)
{
    align_ = align;
    theme_ = this->monitor()->theme();
    isGrabFocus_ = false;

    isSolid_ = (Util::get(KERNEL->commonSettings(), "slot.style") == "solid");
    initTabs();

    label_ = new Label(this->monitor(), window(), Label::CENTER, gc());
    label_->setX(1);
    label_->setY(1);
    label_->setWidth(width() - 2);
    label_->setHeight(this->monitor()->titleBarHeight());
}

Slot::~Slot() {
}

void Slot::initTabs() {

    string tabs = Util::get(KERNEL->commonSettings(), "slot.tabs");

    string name = "";
    for (string::iterator it = tabs.begin(); it != tabs.end(); ) {

        char c = *it;
        it++;
        bool isEndReached = it == tabs.end();

        if ((c == ',') || isEndReached) {
            if (isEndReached) {
                name += c;
            }
            if (name != "") {
                attach(new Tab(name));
                name = "";
            }
        }
        else {
            name += c;
        }
    }

    if (!size()) {
        attach(new Tab("default"));
    }
}

void Slot::show(string tabName) {

    if (isVisible()
        && tabName == this->tabName()
        && hasClients())
    {
        illuminate();
        return;
    }
    isVisible_ = true;
    for (LTab::iterator it = begin(); it != end(); it++) {
        Tab *tab = *it;
        tab->hide();
        if (tab->name() == tabName) {
            focus(tab);
        }
    }
    focused()->show();
    XCORE->show(window());
}

void Slot::hide() {
    isVisible_ = false;
    focused()->hide();
    XCORE->hide(window());
    monitor()->matchWorkspace(monitor()->focused(), align_);
}

Direction Slot::alignment() const {
    return align_;
}

void Slot::illuminate() {

    if (!isVisible() || width() == 1) {
        return;
    }

    if (isSolid_) {
        Rectangle rect(*this);
        rect.setX(0);
        rect.setY(0);
        LOGDEBUG("drawing solid background");
        // draw solid background
        XCORE->setForeground(gc(), theme_->BAR_BACKGROUND);
        XCORE->fillRectangle(window(), gc(), &rect);
        Draw::drawRectBorder(window(), gc(), &rect,
                             theme_->BAR_SHINE, theme_->BAR_BACKGROUND);
    }
    else {
        XCORE->clearWindow(window());
    }

    label_->setWidth(width() - 2);
    unsigned int offsetY = 1;
    unsigned int i = 0;
    for (LTab::iterator it = begin(); it != end(); it++) {
        Tab *tab = *it;
        label_->setY(offsetY + (i * label_->height()));
        label_->setText(tab->name());
        if (tab == focused()) {
            label_->update(theme_->LABEL_BACKGROUND_FOCUSSED,
                       theme_->LABEL_TEXT_FOCUSSED,
                       theme_->LABEL_SHINE_FOCUSSED,
                       theme_->LABEL_SHADOW_FOCUSSED,
                       true, true);
        }
        else {
            label_->update(theme_->LABEL_BACKGROUND_NORMAL,
                           theme_->LABEL_TEXT_NORMAL,
                           theme_->LABEL_SHINE_NORMAL,
                           theme_->LABEL_SHADOW_NORMAL,
                           true, true);
        }
        i++;
    }

    LClient *clients = focused()->clients();
    for (LClient::iterator it = clients->begin();
            it != clients->end(); it++)
    {
        Client *client = *it;
        Rectangle rect(*client);
        rect.setX(rect.x() - 1);
        rect.setY(rect.y() - 1);
        rect.setWidth(rect.width() + 2);
        rect.setHeight(rect.height() + 2);

        if (client == focused()->focused()) {
            Draw::drawRectBorder(window(), gc(), &rect,
                                 theme_->TAB_SHINE_ACTIVE_FOCUSSED,
                                 theme_->TAB_SHADOW_ACTIVE_FOCUSSED);
        }
        else {
            Draw::drawRectBorder(window(), gc(), &rect,
                                 theme_->TAB_SHINE_ACTIVE_NORMAL,
                                 theme_->TAB_SHADOW_ACTIVE_NORMAL);
        }
    }
}

bool Slot::hasClients() {

    for (LTab::iterator it = begin(); it != end(); it++) {
        Tab *tab = *it;
        if (tab->clients()->size()) {
            return true;
        }
    }
    return false;
}

void Slot::manage() {

    if (!isVisible()) {
        return;
    }

    static bool isSlotOverlap =
        Util::get(KERNEL->commonSettings(), "slot.mode") == "overlap";
    unsigned int cHeight = 0;
    unsigned int maxWidth = 1;
    LClient *clients = focused()->clients();
    if (!clients->size() && hasClients()) {
        maxWidth = width();
    }
    for (LClient::iterator it = clients->begin();
            it != clients->end(); it++)
    {
        Client *client = *it;
        cHeight += client->height();
        if (maxWidth < client->width()) {
            maxWidth = client->width() + 2;
        }
    }

    if (align_ == RIGHT) {
        setX(monitor()->width() - maxWidth);
    }
    setWidth(maxWidth);

    // apps are centered
    unsigned int tabPagerHeight = (size() * label_->height()) + 3;
    int offsetY = tabPagerHeight;

    for (LClient::iterator it = clients->begin();
            it != clients->end(); it++)
    {
        Client *client = *it;
        client->setY(offsetY);
        client->setX((width() - client->width()) / 2);
        client->resize();
        offsetY += client->height() + 2;
    }

    if (isSolid_ && !isSlotOverlap) {
        setHeight(monitor()->focused()->height());
    }
    else {
        setHeight(offsetY);
    }
    monitor()->matchWorkspace(monitor()->focused(), align_);
    XCORE->moveResize(window(), this);
    illuminate();
}

Tab *Slot::tabForName(string name) {

    for (LTab::iterator it = begin(); it != end(); it++) {
        Tab *tab = *it;
        if (tab->name() == name) {
            return tab;
        }
    }

    return 0;
}

void Slot::attachClient(Client *client) {

    if (width() < client->width()) {
        setWidth(client->width());
    }

    Binder::instance()->ungrabButtons(client->clientWindow());
    Binder::instance()->grabButtons(client->clientWindow(), AnyModifier);

    if (client->hooked() != "") {
        Tab *tab = tabForName(client->hooked());
        if (tab && (tab != focused())) {
            focused()->hide();
            focus(tab);
            tab->show();
        }
    }

    focused()->addClient(client);
    client->reparent(window(), 0, 0);

    if (isVisible()) {
        client->show();
    }
    manage();
}

void Slot::detachClient(Client *client) {

    for (LTab::iterator it = begin(); it != end(); it++) {
        Tab *tab = *it;
        LClient *clients = tab->clients();
        bool found = false;
        for (LClient::iterator cit = clients->begin();
             cit != clients->end(); cit++)
        {
            found = (client == *cit);
            if (found) {
                break;
            }
        }
        if (found) {
            tab->removeClient(client);
            break;
        }
    }
    if (!client->isDestroyed()) {
        if (client->isVisible()) {
            client->hide();
        }
        client->reparent(monitor()->rootWindow(), 0, 0);
    }
    manage();
}

void Slot::handleButtonPress(XButtonEvent *event) {

    if (event->button == Button1) {
        unsigned int tabPagerHeight = (size() * label_->height()) + 3;

        XCORE->raise(window());
        if (event->y < (int)tabPagerHeight) {
            unsigned int selTab = event->y / label_->height();
            ostringstream oss;
            oss << "select tab #" << selTab << " of " << size() << " tabs ";
            LOGDEBUG(oss.str());
            unsigned int i = 0;
            for (LTab::iterator it = begin(); it != end(); it++) {
                if (selTab == i) {
                    if (focused() != *it) {
                        focused()->hide();
                        focus(*it);
                        focused()->show();
                        manage();
                    }
                    return;
                }
                i++;
            }
        }
    }
    else if (event->button == Button4) {
        monitor()->cycleSlotTabPrev();
    }
    else if (event->button == Button5) {
        monitor()->cycleSlotTabNext();
    }
}

void Slot::handleMotionNotify(XMotionEvent *event) {
}

string Slot::tabName() {
    return focused()->name();
}

bool Slot::isGrabFocus() const {
    return isGrabFocus_;
}

void Slot::setGrabFocus(bool grabFocus) {
    Client *client = focused()->focused();
    if (isGrabFocus_ && !grabFocus) {
        // workspace grabs back the focus
        if (client) {
            Binder::instance()->ungrabButtons(client->clientWindow());
            Binder::instance()->grabButtons(client->clientWindow(), AnyModifier);
        }
    }
    isGrabFocus_ = grabFocus;
}

void Slot::focusClient(Client *client) {

    for (LTab::iterator it = begin(); it != end(); it++) {
        Tab *tab = *it;
        LClient *clients = tab->clients();
        for (LClient::iterator cit = clients->begin();
                cit != clients->end(); cit++)
        {
            if (*cit == client) {
                if (tab != focused()) {
                    focused()->hide();
                    focus(tab);
                    tab->show();
                    manage();
                }
                Client *old = focused()->focused();
                if (old != client) {
                    Binder::instance()->ungrabButtons(old->clientWindow());
                    Binder::instance()->grabButtons(old->clientWindow(),
                                                    AnyModifier);
                }
                Client *tc = monitor()->focused()->topClient();
                isGrabFocus_ = true;
                if (tc) {
                    // unfocus topClient() of workspace, because the
                    // slot grabs focus control now
                    Binder::instance()->ungrabButtons(tc->clientWindow());
                    Binder::instance()->grabButtons(tc->clientWindow(),
                                                    AnyModifier);
                    monitor()->focused()->illuminate();
                }
                focused()->focus(client);
                Binder::instance()->ungrabButtons(client->clientWindow());
                XCORE->setInputFocus(client->clientWindow());
                illuminate();
                return;
            }
        }
    }
}
#endif // SLOT_SUPPORT
