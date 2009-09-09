// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: launcher.h 734 2004-09-27 18:15:45Z garbeam $

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
