// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: menu.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "menu.h"

#include <sstream>

#include "action.h"
#include "clientbar.h"
#include "cursors.h"
#include "draw.h"
#include "font.h"
#include "kernel.h"
#include "label.h"
#include "logger.h"
#include "monitor.h"
#include "theme.h"
#include "util.h"
#include "workspace.h"
#include "xcore.h"
#include "xfont.h"

/// Item class implementation

Item::Item(string name, void *data, Type type) {
    data_ = data;
    type_ = type;

    switch (type_) {
    case WORKSPACE:
        {
            Workspace *workspace = (Workspace *)data;
            Monitor *monitor = workspace->attached();
            if (workspace->clients()->size()
                || monitor->detachedClients()->size()
                || monitor->stickyClients()->size())
            {
                addItem(new Item(name, 0, ROOTITEM));
                addItem(new Item("", 0, SEPARATOR));
                name_ = name + "  >>";
            }
            else {
                name_ = name;
            }
            if (workspace == monitor->focused()) {
                name_ = "* " + name_;
            }
            else {
                name_ = "  " + name_;
            }
        }
        break;
    case DCLIENT:
        name_ = "+ " + name;
        break;
    case ROOTITEM:
        name_ = "<<  " + name;
        break;
    case SEPARATOR:
        name_ = "  -----------------------";
        break;
    default:
        name_ = "  " + name;
        break;
    }
}

Item::~Item() {
    // clean up subitems
    while (items_.size()) {
        Item *item = items_.front();
        items_.erase(items_.begin());
        delete item;
    }
}

LItem *Item::items() {
    return &items_;
}

void Item::addItem(Item *item) {
    item->setParent(this);
    items_.push_back(item);
}

void Item::removeItem(Item *item) {
    item->setParent(0);
    items_.remove(item);
}

Item::Type Item::type() const {
    return type_;
}

string Item::name() const {
    return name_;
}

unsigned int Item::size() {
    return items_.size();
}

Item *Item::parent() const {
    return parent_;
}

void Item::setParent(Item *parent) {
    parent_ = parent;
}

void *Item::data() const {
    return data_;
}

/// Menu class implementation

Menu::Menu(Monitor *monitor)
    : Widget(monitor, new Rectangle(0, 0, 1, 1))
{
    theme_ = this->monitor()->theme();
    root_ = subRoot_ = selected_ = 0;

    label_ = new Label(this->monitor(), window(), Label::LEFT, gc());
    label_->setX(1);
    label_->setY(1);
    label_->setWidth(1);
    label_->setHeight(this->monitor()->font()->height() + 2);

    initActions();
}

Menu::~Menu() {

}

void Menu::initActions() {

    MBindings *actionBindings = KERNEL->actionBindings();
    string actions = Util::get(KERNEL->commonSettings(), "menu.actions");

    string actionName = "";
    bool proceed = actions.size();
    for (string::iterator it = actions.begin(); proceed; it++) {

        if ((*it != ',') && (it != actions.end())) {
            actionName += *it;
        }
        else {
            Action *action = Util::get(actionBindings, actionName);
            if (action) {
                actions_.push_back(action);
            }
            else {
                LOGWARN("unknown action '" + actionName + "' in menu setup");
            }
            actionName = "";
        }
        proceed = it != actions.end();
    }

}

void Menu::updateItemTree() {

    if (root_) {
        delete root_;
    }
    root_ = new Item("", 0, Item::ROOT);
    subRoot_ = root_;

    // now build up the item tree
    for (LAction::iterator it = actions_.begin(); it != actions_.end(); it++) {
        Action *action = *it;
        if (action->isValid()) {
            root_->addItem(new Item(action->id(), action, Item::ACTION));
        }
    }
    if (root_->items()->size()) {
        root_->addItem(new Item("", 0, Item::SEPARATOR));
    }

    // now add all workspaces
    for (LWorkspace::iterator it = monitor()->begin();
         it != monitor()->end(); it++)
    {
        Workspace *workspace = *it;
        Item *wsItem = new Item(workspace->name(), workspace, Item::WORKSPACE);
        LClient *clients = workspace->clients();
        for (LClient::iterator it = clients->begin(); it != clients->end(); it++) {
            Client *client = *it;
            if (client->mode() != Client::STICKY) {
                wsItem->addItem(new Item(client->name(), client, Item::CLIENT));
            }
        }
        CClient *stickyClients = monitor()->stickyClients();
        for (LClient::iterator it = stickyClients->begin();
                it != stickyClients->end(); it++)
        {
            Client *client = *it;
            wsItem->addItem(new Item(client->name(), client, Item::CLIENT));
        }
        CClient *detachedClients = monitor()->detachedClients();
        if (detachedClients->size()) {
            if (workspace->clients()->size() || clients->size()) {
                wsItem->addItem(new Item("", 0, Item::SEPARATOR));
            }
            for (LClient::iterator it = detachedClients->begin();
                    it != detachedClients->end(); it++)
            {
                Client *client = *it;
                wsItem->addItem(new Item(client->name(), client, Item::DCLIENT));
            }
        }
        root_->addItem(wsItem);
    }
}

void Menu::show() {
    KERNEL->setMenuMode(true);
    isVisible_ = true;
    XCORE->showRaised(window());
    updateItemTree();
    manage();
}

void Menu::hide() {
    KERNEL->setMenuMode(false);
    isVisible_ = false;
    XCORE->hide(window());
}

void Menu::illuminate() {

    Rectangle rect(*this);
    rect.setX(0);
    rect.setY(0);
    LOGDEBUG("drawing solid background");
    // draw solid background
    XCORE->raise(window());
    XCORE->setForeground(gc(), theme_->BAR_BACKGROUND);
    XCORE->fillRectangle(window(), gc(), &rect);
    LItem *items = subRoot_->items();

    unsigned int offsetY = 1;
    unsigned int i = 0;
    for (LItem::iterator it = items->begin(); it != items->end(); it++) {
        Item *item = *it;
        label_->setY(offsetY + i * label_->height());
        label_->setText(item->name());
        if ((item == selected_) && (item->type() != Item::SEPARATOR)) {
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
                           theme_->LABEL_SHADOW_NORMAL);
        }
        i++;
    }

    Draw::drawRectBorder(window(), gc(), &rect,
            theme_->BAR_SHINE, theme_->BAR_SHADOW);
}

void Menu::manage() {

    if (!isVisible()) {
        return;
    }

    unsigned int maxWidth = 0;
    LItem *items = subRoot_->items();
    for (LItem::iterator it = items->begin(); it != items->end(); it++) {
        Item *item = *it;
        label_->setText(item->name());
        if (label_->textWidth() > maxWidth) {
            maxWidth = label_->textWidth() + 10;
        }
    }
    label_->setWidth(maxWidth);
    setWidth(maxWidth + 2);
    setHeight(items->size() * label_->height() + 2);

    if (monitor()->clientBar()->y()) {
        setY(monitor()->height() - monitor()->clientBar()->height() - height());
    }
    else {
        setY(monitor()->clientBar()->height());
    }

    // time to illuminate menu
    resize();
    illuminate();
}

void Menu::handleButtonPress(XButtonEvent *event) {
    XCORE->raise(window());
    if (selected_) {
        switch (selected_->type()) {
        case Item::WORKSPACE:
            {
                Workspace *workspace = (Workspace *)selected_->data();
                if (event->button == Button1) {
                    if (selected_->items()->size()) {
                        subRoot_ = selected_;
                        manage();
                    }
                    else { // focus workspace
                        monitor()->focus(workspace);
                        escape();
                    }
                }
                else if (event->button == Button3) {
                    monitor()->focus(workspace);
                    updateItemTree();
                    manage();
                }
            }
            break;
        case Item::ROOTITEM:
            subRoot_ = subRoot_->parent();
            manage();
            break;
        case Item::ACTION:
            {
                Action *action = (Action *)selected_->data();
                if (action->isValid()) {
                    action->perform();
                    escape();
                }
            }
            break;
        case Item::CLIENT:
            {
                Workspace *workspace = (Workspace *)subRoot_->data();
                Client *client = (Client *)selected_->data();
                if (event->button == Button3) {
                    monitor()->detachClient(client);
                    if (monitor()->focused() != workspace) {
                        monitor()->attachLastClient();
                    }
                }
                else {
                    monitor()->focus(workspace);
                    workspace->focus(client);
                    if (event->button == Button2) {
                        XCORE->warpPointer(client->clientWindow(),
                                           client->width() / 2,
                                           client->height() / 2);
                    }
                }
                escape();
            }
            break;
        case Item::DCLIENT:
            {
                Client *client = (Client *)selected_->data();
                monitor()->attachDetachedClient((Workspace *)subRoot_->data(),
                                               client);
                if (event->button == Button2) {
                    XCORE->warpPointer(client->clientWindow(),
                                       client->x() + client->width() / 2,
                                       client->y() + client->height() / 2);
                }
                escape();
            }
            break;
        default:
            break;
        }
    }
}

void Menu::escape() {
    monitor()->clientBar()->toggleMenuMode();
}

void Menu::handleMotionNotify(XMotionEvent *event) {

    unsigned int selItem = event->y / label_->height();
    ostringstream oss;
    oss << "eventY=" << event->y << " selItem=" << selItem;
    LOGDEBUG(oss.str());

    unsigned int i = 0;
    LItem *items = subRoot_->items();
    for (LItem::iterator it = items->begin(); it != items->end(); it++) {
        Item *item = *it;
        if (selItem == i) {
            if (item == selected_) {
                LOGDEBUG("no redraw");
                return;
            }
            selected_ = item;
            illuminate();
            return;
        }
        i++;
    }
    selected_ = 0;
}
