// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __LAUNCHER_H
#define __LAUNCHER_H

#include <string>

#include "wmi.h"

#include "singleton.h"

// forward declaration
class Action;
class Launcher;

/**
 * Launches external processes and is capable to process chain
 * invocations.
 *
 * Determines recursion in chain invocation.
 */
class Launcher : public Singleton<Launcher>
{

public:

    /** Constructs a new process manager instance.  */
    Launcher();

    ~Launcher();

    /** Execs the given command. */
    void exec(string command);

    /** Execs a chain. */
    void execSeq(Action *caller, string command);
};

#endif // __LAUNCHER_H
