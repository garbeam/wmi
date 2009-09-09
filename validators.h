// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __VALIDATORS_H
#define __VALIDATORS_H

#include "wmi.h"

#include "singleton.h"

class Kernel;
class Monitor;

class Validators;

/** Note: existance checks are qualified only on focused stuff. */
class Validators : public Singleton<Validators>
{

public:

    Validators();
    ~Validators();

    // validation methods
    bool isAlwaysPossible();

    /**
     * Return <code>true</code> if a client can be unhooked.
     */
    bool validateUnhookClient();

    /**
     * Return <code>true</code> if a client can be unhooked.
     */
    bool validateHookClient();

    /**
     * Returns <code>true</code> if a frame is focussed and there're
     * more frames.
     */
    bool validateCycleFrame();

    /** Returns <code>true</code> if the wmi kernel has recorded actions. */
    bool validateEndRecord();

    /** Returns <code>true</code> if the wmi is recording actions. */
    bool validateCancelRecord();

    /** Returns <code>true</code> if the wmi kernel is not recording actions. */
    bool validateBeginRecord();

    /** Returns <code>true</code> if a workspace exists. */
    bool isWorkspaceFocused();

    /** Returns <code>true</code> if at least two workspaces exist. */
    bool existMonitors();

    /** Returns <code>true</code> if at least two workspaces exist. */
    bool existWorkspaces();

    bool isEmptyWorkspaceFocused();
    bool validateDestroyWorkspace();
    bool validateDestroyFrame();

    bool validateDestroyAction();

    /** Validates directional grow/shrink actions. */
    bool validateResizeLeft();
    bool validateResizeRight();
    bool validateResizeUp();
    bool validateResizeDown();

    /** Returns <code>true</code> if a frame into direction exist. */
    bool existsFrameLeft();
    bool existsFrameRight();
    bool existsFrameUp();
    bool existsFrameDown();

    /** Returns <code>true</code> if a frame is focused. */
    bool isFrameFocused();

    /** Returns <code>true</code> if a frame or client exist. */
    bool isFrameOrClientFrameFocused();

    /** Returns <code>true</code> if at least two frames exist. */
    bool existFrames();

    /** Returns <code>true</code> if toggleMode is valid. */
    bool validateToggleMode();

    /** Returns <code>true</code> if a client frame exists. */
    bool isClientFrameFocused();

    /** Returns <code>true</code> if a client exists. */
    bool isClientFocused();

    /** Returns <code>true</code> if a floating client exists. */
    bool isFloatingClientFocused();

    /** Returns <code>true</code> if at least two clients exists. */
    bool existClients();

    /** Returns <code>true</code> if at least two clients exists. */
    bool existClientsWithinFrame();

    bool existsDetachedClient();
    bool existDetachedClients();

    /** Returns <code>true</code> if input mode is not active. */
    bool validateInputMode();

    /**
     * Returns <code>true</code> if any client exists on the
     * focused monitor.
     */ 
    bool validateSelectClient();
#ifdef SLOT_SUPPORT
    bool existsSlotClient();
    bool existSlotTabs();
#endif // SLOT_SUPPORT

};

#endif // __VALIDATORS_H
