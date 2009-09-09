// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: action.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __ACTION_H
#define __ACTION_H

extern "C" {
#include <X11/Xlib.h>
}

#include <list>
#include <string>
#include "wmi.h"

class Action;
class Actions;
class Prompt;
class Shortcut;
class Validators;

// Method pointer type for actions
typedef void (Actions::*ToPerform)(Action *, const char *);
typedef bool (Validators::*IsValid)();

typedef list<Prompt *> LPrompt;

/** Interface for all actions which will be performed by WMI. */
class Action
{

public:

    enum Type {INTERN, EXTERN, SEQUENCE, UNKNOWN};

    Action(string id, ToPerform toPerform,
           IsValid isValid, Type type = INTERN, char *argument = 0);

    ~Action();

    string id() const;

    unsigned int promptsCount() const;

    /** Performs the action. */
    void perform();

    /** Returns <code>true</code> if the actions is possible. */
    bool isValid();

    /** Returns the is possible method pointer. */
    IsValid getIsValid() const;

    void setShortcut(Shortcut *shortcut);

    Shortcut *shortcut() const;

    void setArgument(char *argument);

    char *argument() const;

    Type type() const;

    void setType(Type type);

    /** Returns prompt for argument at position 'index'. */
    Prompt *prompt(unsigned int index);

    LPrompt *prompts();

    ToPerform getToPerform() const;

    void setListenOn(Shortcut *listenOn);

    Shortcut *listenOn() const;

private:

    string id_;
    ToPerform toPerform_;
    IsValid isValid_;

    // for keyboard bindings
    Shortcut *shortcut_;
    Shortcut *listenOn_;

    // for special stuff
    char *argument_;
    Type type_;
    LPrompt prompts_;

};

#endif // __ACTION_H
