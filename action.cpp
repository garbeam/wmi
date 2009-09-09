// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: action.cpp 734 2004-09-27 18:15:45Z garbeam $

extern "C" {
#include <stdlib.h>
#include <string.h>
}

#include "wmi.h"

#include "action.h"
#include "actions.h"
#include "kernel.h"
#include "monitor.h"
#include "prompt.h"
#include "shortcut.h"
#include "validators.h"


Action::Action(string id, ToPerform toPerform,
           IsValid isValid, Type type, char *argument)
{
    id_ = id;
    toPerform_ = toPerform;
    isValid_ = isValid;
    type_ = type;
    argument_ = argument;
    shortcut_ = 0;
    listenOn_ = 0;
}

Action::~Action() {
    if (argument_) {
        free(argument_);
    }
}

void Action::perform() {
    // great HACK to return from maximized frames back to normality
    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    if (focusedMonitor->isThingMaximized() &&
        (toPerform_ != &Actions::cycleClientNext) &&
        (toPerform_ != &Actions::cycleClientPrev) &&
        (toPerform_ != &Actions::toggleBar) &&
        (toPerform_ != &Actions::toggleMaximization))
    {

        focusedMonitor->toggleThingMaximization();
    }

    if (KERNEL->isRecording() &&
        (toPerform_ != &Actions::endChainRecord) &&
        (toPerform_ != &Actions::endScriptRecord) &&
        (toPerform_ != &Actions::inputMode))
    {
         KERNEL->recordedActions()->push_back(
                 new Action(id_, 0, 0, type_, argument_ ? strdup(argument_) : 0));
    }

    Actions actions = *Actions::instance();

    if (isValid()) {
        (actions.*toPerform_)(this, argument_); // call action
    }
}

bool Action::isValid() {
    Validators validators = *Validators::instance();

    return (validators.*isValid_)();
}

Prompt *Action::prompt(unsigned int index) {

    unsigned int i = 0;
    for (LPrompt::iterator it = prompts_.begin();
         it != prompts_.end(); it++)
    {
        if (i == index) {
            return (*it);
        }
        i++;
    }

    // should never occure
    return 0;
}

string Action::id() const {
    return id_;
}

unsigned int Action::promptsCount() const {
    return prompts_.size();
}


IsValid Action::getIsValid() const {
    return isValid_;
}

void Action::setShortcut(Shortcut *shortcut) {
    shortcut_ = shortcut;
    listenOn_ = shortcut;
}

Shortcut *Action::shortcut() const {
    return shortcut_;
}

void Action::setArgument(char *argument) {
    argument_ = argument;
}

char *Action::argument() const {
    return argument_;
}

Action::Type Action::type() const {
    return type_;
}

void Action::setType(Type type) {
    type_ = type;
}


LPrompt *Action::prompts() {
    return &prompts_;
}

ToPerform Action::getToPerform() const {
    return toPerform_;
}

void Action::setListenOn(Shortcut *listenOn) {
    listenOn_ = listenOn;
}

Shortcut *Action::listenOn() const {
    return listenOn_;
}
