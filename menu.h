// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: menu.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __MENU_H
#define __MENU_H

#include "wmi.h"

extern "C" {
#include <X11/Xlib.h>
}

#include <list>
#include <string>

#include "widget.h"

class Action;
class Client;
class Item;
class Label;
class Monitor;
class Theme;

typedef list<Action *> LAction;
typedef list<Item *> LItem;

/**
 * Items for menu.
 */
class Item {

public:

    // The type of this items is related to the data_ pointer which must
    // of following type if the specific type has been choosen:
    //   ACTION -> Action *
    //   CLIENT -> Client * (attached client)
    //   DCLIENT -> Client * (detached client)
    //   WORKSPACE -> Workspace *
    //   ROOT -> 0
    //   ROOTITEM -> 0
    //   SEPARATOR -> 0
    enum Type {ACTION, CLIENT, DCLIENT, WORKSPACE, ROOT,
               ROOTITEM, SEPARATOR};

    Item(string name, void *data, Type type);
    ~Item();

    LItem *items();

    void addItem(Item *item);
    void removeItem(Item *item);

    Type type() const;
    string name() const;
    Item *parent() const;
    void setParent(Item *parent);
    void *data() const;

    unsigned int size();

private:

    string name_;
    void *data_;
    LItem items_;
    Type type_;
    Item *parent_;

};


/**
 * Basic menu for action, cut'n'paste and re-attach stuff.
 */
class Menu : public Widget {

public:

    Menu(Monitor *monitor);

    ~Menu();

    void manage();
    void updateItemTree();

    /** called on Expose by Kernel. */
    void illuminate();

    void handleButtonPress(XButtonEvent *event);
    void handleMotionNotify(XMotionEvent *event);

    virtual void show();
    virtual void hide();

private:

    void initActions();

    void escape();

    Theme *theme_;
    Label *label_;
    LAction actions_;
    Item *root_;
    Item *subRoot_;
    Item *selected_;
};

#endif // __MENU_H
