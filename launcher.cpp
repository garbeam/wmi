// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <X11/Xlib.h>
}

#include <map>

#include "launcher.h"

#include "action.h"
#include "logger.h"
#include "kernel.h"
#include "monitor.h"

Launcher::Launcher() {}
Launcher::~Launcher() {}

void Launcher::exec(string command) {

    Monitor *focusedMonitor = Kernel::instance()->focusedMonitor();

    if (fork() == 0) {
        if (fork() == 0) {
            setsid();

            close(ConnectionNumber(Kernel::instance()->display()));

            putenv((char *)focusedMonitor->displayString().c_str());

            char **cmdArgs = Util::arguments(command);

            LOGDEBUG("executing '" + command + "'");
            execvp(cmdArgs[0], cmdArgs);
            LOGERROR("cannot execute '" + command + "'", true);
            // exits WMI
        }
        exit(0);
    }
    wait(0);
}

void Launcher::execSeq(Action *caller, string command)  {

    MBindings *actionBindings = KERNEL->actionBindings();

    string cmd = command;
    string actionName = "";
    string arguments = "";

    LOGDEBUG("sequence: got: " + cmd);
    bool proceed = true;
    for (string::iterator it = cmd.begin(); proceed; it++) {

        proceed = it != cmd.end();
        if (!proceed || (*it == ',') ) { // proceed action
            if (caller->id() == actionName) { // recursion detected
                Logger::instance()->warning(
                        "sequence '" + actionName +
                        "' was called recursivly, stopped");
                return;

            }
            // argument fetching
            unsigned int argCount = 0;
            unsigned int argDelim = actionName.find_first_of('+');

            if (argDelim != string::npos) {
                argCount++; // at least 1 argument
                arguments = actionName.substr(argDelim + 1);
                LOGDEBUG("sequence: got arguments: " + arguments);
                actionName = actionName.substr(0, argDelim);
                LOGDEBUG("sequence: found arguments: " + arguments);
                LOGDEBUG("sequence: for action: " + actionName);
            }

            Action *action = Util::get(actionBindings, actionName);
            if (!action) {
                LOGWARN("unknown action '" + actionName +
                        "' cannot be performed sequentially");
                return;
            }
            if (action->promptsCount() != argCount) {
                LOGWARN("action '" + actionName +
                        "' cannot be performed sequentially, because "
                        "to much or less given arguments.");
                return;
            }
            if (!action->isValid()) {
                LOGWARN("action '" + actionName +
                        "' cannot be performed because it"
                        " won't have any effect");
                return;
            }
            if (argCount) {
                action->setArgument((char *)arguments.c_str());
            }
            action->perform();
            actionName = "";
        }
        else {
            actionName += *it;
        }
    }
}
