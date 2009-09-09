// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: validators.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "validators.h"

#include <map>

#include "action.h"
#include "client.h"
#include "frame.h"
#include "kernel.h"
#include "inputbar.h"
#include "monitor.h"
#include "slot.h"
#include "statusbar.h"
#include "thing.h"
#include "workspace.h"

Validators::Validators() {
}

Validators::~Validators() {
}

bool Validators::validateSelectClient() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    for (LWorkspace::iterator wit = focusedMonitor->begin();
            wit != focusedMonitor->end(); wit++)
    {
        Workspace *workspace = *wit;
 
        if (workspace->floatingClients()->size() ||
            (workspace->focusedFrame() &&
             workspace->focusedFrame()->size()))
        {
            return true;
        }

    }

    return false;
}

bool Validators::validateCycleFrame() {

    return existFrames() && isFrameFocused();

}

bool Validators::validateInputMode() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    return !focusedMonitor->inputBar()->isVisible();
}


bool Validators::isAlwaysPossible() {

    return true;
}

bool Validators::isWorkspaceFocused() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    return focusedMonitor->focused() != 0;
}

bool Validators::existMonitors() {
    return KERNEL->monitors()->size() > 1;
}

bool Validators::existWorkspaces() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    return focusedMonitor->size() > 1;
}

bool Validators::isEmptyWorkspaceFocused() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace->floatingClients()->size() ||
            (workspace->focusedFrame() &&
             workspace->focusedFrame()->size()))
    {
        return false;
    }
    return true;
}

bool Validators::validateDestroyAction() {

    MBindings *bindings = KERNEL->actionBindings();
    for (MBindings::iterator it = bindings->begin();
            it != bindings->end(); it++)
    {
        Action *action = (*it).second;
        if (action->type() != Action::INTERN) {
            return true;
        }
    }
    return false;
}

bool Validators::validateDestroyWorkspace() {

    return isEmptyWorkspaceFocused() && existWorkspaces();
}

bool Validators::existsFrameLeft() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    return workspace->neighborTree(LEFT) != 0;
}

bool Validators::existsFrameRight() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    return workspace->neighborTree(RIGHT) != 0;
}

bool Validators::existsFrameUp() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    return workspace->neighborTree(UP) != 0;
}

bool Validators::existsFrameDown() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    return workspace->neighborTree(DOWN) != 0;
}

bool Validators::validateToggleMode() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace->isFrameMode()) {
        return workspace->floatingClients()->size();
    }
    else {
        return workspace->frames()->size();
    }
}

bool Validators::existFrames() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace) {
        return workspace->frames()->size();
    }

    return false;
}

bool Validators::validateResizeLeft() {

    return isClientFrameFocused() || existsFrameLeft();
}

bool Validators::validateResizeRight() {

    return isClientFrameFocused() || existsFrameRight();
}

bool Validators::validateResizeUp() {

    return isClientFrameFocused() || existsFrameUp();
}

bool Validators::validateResizeDown() {

    return isClientFrameFocused() || existsFrameDown();
}

bool Validators::isFrameOrClientFrameFocused() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace) {
        Thing *thing = workspace->focusedThing();

        if (!thing) {
            return false;
        }

        return thing->hasDecoration();
    }
    return false;

}


bool Validators::isFrameFocused() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace) {
        return workspace->focusedFrame() && workspace->isFrameMode();
    }
    return false;
}


bool Validators::isClientFrameFocused() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (!workspace) {
        return false;
    }

    Client *client = workspace->topClient();

    if (!client) {
        return false;
    }

    return !client->frame();
}

bool Validators::isFloatingClientFocused() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace) {
        Thing *thing = workspace->focusedThing();

        if (!thing) {
            return false;
        }

        return thing->type() != Thing::FRAME;
    }

    return false;
}

bool Validators::isClientFocused() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    return workspace->focusedThing() != 0;

}

bool Validators::existClientsWithinFrame() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace) {
        Thing *thing = workspace->focusedThing();
        if (thing && (thing->type() == Thing::FRAME)) {
            Frame *frame = (Frame *)thing;
            return frame->size() > 1;
        }
    }

    return false;
}

bool Validators::existClients() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    if (workspace->isFrameMode()) {
        Frame *frame = workspace->focusedFrame();
        return frame && (frame->size() > 1);
    }
    else {
        return (workspace->floatingClients()->size() > 1);
    }
}

bool Validators::existDetachedClients() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    return isWorkspaceFocused() &&
        (focusedMonitor->detachedClients()->size() > 1);
}

bool Validators::existsDetachedClient() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    return isWorkspaceFocused() &&
        (focusedMonitor->detachedClients()->size() > 0);
}

bool Validators::validateCancelRecord() {
    return KERNEL->isRecording();
}

bool Validators::validateEndRecord() {
    return KERNEL->isRecording() && (KERNEL->recordedActions()->size() > 0);
}

bool Validators::validateBeginRecord() {
    return !KERNEL->isRecording();
}

#ifdef SLOT_SUPPORT
bool Validators::existsSlotClient() {
    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    return focusedMonitor->slot()->focused()->focused() != 0;
}

bool Validators::existSlotTabs() {
    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    return focusedMonitor->slot()->size() > 0;
}
#endif // SLOT_SUPPORT

bool Validators::validateHookClient() {

#ifdef SLOT_SUPPORT
    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Slot *slot = focusedMonitor->slot();
    Client *client = slot->focused()->focused();
    if (client && slot->isGrabFocus()) {
        return true;
    }
#endif // SLOT_SUPPORT
    return isClientFocused();
}

bool Validators::validateUnhookClient() {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Client *client = 0;
#ifdef SLOT_SUPPORT
    Slot *slot = focusedMonitor->slot();
    client = slot->focused()->focused();
    if (client && (client->hooked() == "")
        && slot->isGrabFocus())
    {
        return true;
    }
#endif // SLOT_SUPPORT
    Workspace *workspace = focusedMonitor->focused();
    client = workspace->topClient();
    return client && (client->hooked() == "");
}

bool Validators::validateDestroyFrame() {
    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    Frame *frame = workspace->focusedFrame();

    return frame && !frame->size();
}
