// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: actions.cpp 734 2004-09-27 18:15:45Z garbeam $

extern "C" {
#include <assert.h>
#include <X11/Xlib.h>
}
#include <sstream>
#include <map>

#include "actions.h"

#include "action.h"
#include "binder.h"
#include "client.h"
#include "clientbar.h"
#include "frame.h"
#include "inputbar.h"
#include "launcher.h"
#include "kernel.h"
#include "monitor.h"
#include "logger.h"
#include "expander.h"
#include "prompt.h"
#include "slot.h"
#include "util.h"
#include "validators.h"
#include "workspace.h"

Actions::Actions() {
}

Actions::~Actions() {
}

void Actions::initInternActions(MBindings *actionBindings) {
    Action *action = 0;

    (*actionBindings)["restart"] = new Action("restart", &Actions::restart, &Validators::isAlwaysPossible);
    (*actionBindings)["quit"] = new Action("quit", &Actions::quit, &Validators::isAlwaysPossible);
    (*actionBindings)["save-settings"] = new Action("save-settings", &Actions::saveSettings, &Validators::isAlwaysPossible);

    (*actionBindings)["begin-record"] = new Action("begin-record", &Actions::beginChainRecord, &Validators::validateBeginRecord);

    action = new Action("end-record-chain", &Actions::endChainRecord, &Validators::validateEndRecord);
    action->prompts()->push_back(new Prompt("action name : ", 0));
    (*actionBindings)["end-record-chain"] = action;

    action = new Action("end-record-script", &Actions::endScriptRecord,
                        &Validators::validateEndRecord);
    action->prompts()->push_back(new Prompt("script path : ", 0));
    (*actionBindings)["end-record-script"] = action;

    (*actionBindings)["cancel-record"] = new Action("cancel-record", &Actions::cancelRecord, &Validators::validateCancelRecord);

    action = new Action("exec-term", &Actions::executeTerm,
                        &Validators::isWorkspaceFocused);
    action->prompts()->push_back(new Prompt("command : ", &Binder::queryCommandForPattern));
    (*actionBindings)["exec-term"] = action;

    (*actionBindings)["rehash"] = new Action("rehash", &Actions::rehash, &Validators::isAlwaysPossible);

    action = new Action("exec", &Actions::execute,
                        &Validators::isWorkspaceFocused);
    action->prompts()->push_back(new Prompt("command : ", &Binder::queryCommandForPattern));
    (*actionBindings)["exec"] = action;

    action = new Action("destroy-action", &Actions::destroyAction,
                        &Validators::validateDestroyAction);
    action->prompts()->push_back(new Prompt("action to destroy : ",
                                    &Binder::queryExternChainActionsForPattern));
    (*actionBindings)["destroy-action"] = action;

#ifdef SLOT_SUPPORT
    (*actionBindings)["slot-client"] = new Action("slot-client", &Actions::slotClient, &Validators::isClientFocused);
    (*actionBindings)["unslot-client"] = new Action("unslot-client", &Actions::unslotClient, &Validators::existsSlotClient);
    (*actionBindings)["toggle-slot"] = new Action("toggle-slot", &Actions::toggleSlot, &Validators::isAlwaysPossible);
    (*actionBindings)["cycle-slot-tab-next"] = new Action("cycle-slot-tab-next", &Actions::cycleSlotTabNext, &Validators::existSlotTabs);
    (*actionBindings)["cycle-slot-tab-prev"] = new Action("cycle-slot-tab-prev", &Actions::cycleSlotTabPrev, &Validators::existSlotTabs);
    (*actionBindings)["kill-slot-client"] = new Action("kill-slot-client", &Actions::killSlotClient, &Validators::existsSlotClient);
#endif // SLOT_SUPPORT

    (*actionBindings)["toggle-client-sticky"] = new Action("toggle-client-sticky", &Actions::toggleClientSticky, &Validators::isClientFocused);

    (*actionBindings)["toggle-client-mode"] = new Action("toggle-client-mode", &Actions::toggleClientMode, &Validators::isFrameOrClientFrameFocused);
    (*actionBindings)["grow-left"] = new Action("grow-left", &Actions::growLeft, &Validators::validateResizeLeft);
    (*actionBindings)["grow-right"] = new Action("grow-right", &Actions::growRight, &Validators::validateResizeRight);
    (*actionBindings)["grow-up"] = new Action("grow-up", &Actions::growUp, &Validators::validateResizeUp);
    (*actionBindings)["grow-down"] = new Action("grow-down", &Actions::growDown, &Validators::validateResizeDown);
    (*actionBindings)["shrink-left"] = new Action("shrink-left", &Actions::shrinkLeft, &Validators::validateResizeRight);
    (*actionBindings)["shrink-right"] = new Action("shrink-right", &Actions::shrinkRight, &Validators::validateResizeLeft);
    (*actionBindings)["shrink-up"] = new Action("shrink-up", &Actions::shrinkUp, &Validators::validateResizeDown);
    (*actionBindings)["shrink-down"] = new Action("shrink-down", &Actions::shrinkDown, &Validators::validateResizeUp);
    (*actionBindings)["move-client-left"] = new Action("move-client-left", &Actions::moveClientLeft, &Validators::isFloatingClientFocused);
    (*actionBindings)["move-client-right"] = new Action("move-client-right", &Actions::moveClientRight, &Validators::isFloatingClientFocused);
    (*actionBindings)["move-client-up"] = new Action("move-client-up", &Actions::moveClientUp, &Validators::isFloatingClientFocused);
    (*actionBindings)["move-client-down"] = new Action("move-client-down", &Actions::moveClientDown, &Validators::isFloatingClientFocused);
    (*actionBindings)["show-bars"] = new Action("show-bars", &Actions::showBars, &Validators::isFrameOrClientFrameFocused);
    (*actionBindings)["hide-bars"] = new Action("hide-bars", &Actions::hideBars, &Validators::isFrameOrClientFrameFocused);
    (*actionBindings)["toggle-bar"] = new Action("toggle-bar", &Actions::toggleBar, &Validators::isFrameOrClientFrameFocused);
    (*actionBindings)["show-borders"] = new Action("show-borders", &Actions::showBorders, &Validators::isFrameOrClientFrameFocused);
    (*actionBindings)["hide-borders"] = new Action("hide-borders", &Actions::hideBorders, &Validators::isFrameOrClientFrameFocused);
    (*actionBindings)["toggle-border"] = new Action("toggle-border", &Actions::toggleBorder, &Validators::isFrameOrClientFrameFocused);
    (*actionBindings)["toggle-statusbar"] = new Action("toggle-statusbar", &Actions::toggleStatusBar, &Validators::isAlwaysPossible);
    (*actionBindings)["toggle-clientbar"] = new Action("toggle-clientbar", &Actions::toggleClientBar, &Validators::isAlwaysPossible);
    (*actionBindings)["kill-client"] = new Action("kill-client", &Actions::killClient, &Validators::isClientFocused);
    (*actionBindings)["cycle-client-next"] = new Action("cycle-client-next", &Actions::cycleClientNext, &Validators::existClients);
    (*actionBindings)["cycle-workspace-next"] = new Action("cycle-workspace-next", &Actions::cycleWorkspaceNext, &Validators::existWorkspaces);

    (*actionBindings)["toggle-mode"] = new Action("toggle-mode", &Actions::toggleMode, &Validators::validateToggleMode);

    action = new Action("select-monitor", &Actions::selectMonitor, &Validators::existMonitors);
    action->prompts()->push_back(new Prompt("monitor : ",
                 &Binder::queryMonitorsForPattern));
    (*actionBindings)["select-monitor"] = action;

    action = new Action("select-workspace", &Actions::selectWorkspace, &Validators::existWorkspaces);
    action->prompts()->push_back(new Prompt("workspace : ",
                 &Binder::queryWorkspacesForPattern));
    (*actionBindings)["select-workspace"] = action;

    action = new Action("select-client",
            &Actions::selectClient, &Validators::validateSelectClient);
    action->prompts()->push_back(new Prompt("client : ",
                 &Binder::queryClientsForPattern));
    (*actionBindings)["select-client"] = action;

    action = new Action("select-client-id",
            &Actions::selectClientId, &Validators::validateSelectClient);
    action->prompts()->push_back(new Prompt("client id: ",
                 &Binder::queryClientIdsForPattern));
    (*actionBindings)["select-client-id"] = action;

    (*actionBindings)["hook-client"] = new Action("hook-client", &Actions::hookClient, &Validators::validateHookClient);
    (*actionBindings)["unhook-client"] = new Action("unhook-client", &Actions::unhookClient, &Validators::validateUnhookClient);

    (*actionBindings)["cycle-client-prev"] = new Action("cycle-client-prev", &Actions::cycleClientPrev, &Validators::existClients);
    (*actionBindings)["cycle-workspace-prev"] = new Action("cycle-workspace-prev", &Actions::cycleWorkspacePrev, &Validators::existWorkspaces);
    (*actionBindings)["split-frame-left"] = new Action("split-frame-left", &Actions::splitFrameLeft, &Validators::existClientsWithinFrame);
    (*actionBindings)["split-frame-right"] = new Action("split-frame-right", &Actions::splitFrameRight, &Validators::existClientsWithinFrame);
    (*actionBindings)["split-frame-up"] = new Action("split-frame-up", &Actions::splitFrameUp, &Validators::existClientsWithinFrame);
    (*actionBindings)["split-frame-down"] = new Action("split-frame-down", &Actions::splitFrameDown, &Validators::existClientsWithinFrame);
    (*actionBindings)["join-frame-left"] = new Action("join-frame-left", &Actions::joinFrameLeft, &Validators::existsFrameLeft);
    (*actionBindings)["join-frame-right"] = new Action("join-frame-right", &Actions::joinFrameRight, &Validators::existsFrameRight);
    (*actionBindings)["join-frame-up"] = new Action("join-frame-up", &Actions::joinFrameUp, &Validators::existsFrameUp);
    (*actionBindings)["join-frame-down"] = new Action("join-frame-down", &Actions::joinFrameDown, &Validators::existsFrameDown);
    (*actionBindings)["send-client-left"] = new Action("send-client-left", &Actions::sendClientLeft, &Validators::existsFrameLeft);
    (*actionBindings)["send-client-right"] = new Action("send-client-right", &Actions::sendClientRight, &Validators::existsFrameRight);
    (*actionBindings)["send-client-up"] = new Action("send-client-up", &Actions::sendClientUp, &Validators::existsFrameUp);
    (*actionBindings)["send-client-down"] = new Action("send-client-down", &Actions::sendClientDown, &Validators::existsFrameDown);
    (*actionBindings)["detach-all-clients"] = new Action("detach-all-clients", &Actions::detachAllClients, &Validators::existClients);
    (*actionBindings)["attach-all-clients"] = new Action("attach-all-clients", &Actions::attachAllClients, &Validators::existDetachedClients);
    (*actionBindings)["detach-client"] = new Action("detach-client", &Actions::detachClient, &Validators::isClientFocused);
    (*actionBindings)["attach-last-client"] = new Action("attach-last-client", &Actions::attachLastClient, &Validators::existsDetachedClient);
    (*actionBindings)["inputmode"] = new Action("inputmode", &Actions::inputMode, &Validators::validateInputMode);

    action = new Action("attach-client",
            &Actions::attachClient, &Validators::existDetachedClients);
    action->prompts()->push_back(new Prompt("client : ",
                 &Binder::queryDetachedClientsForPattern));
    (*actionBindings)["attach-client"] = action;

    action = new Action("create-action", &Actions::createAction,
                        &Validators::isAlwaysPossible);
    action->prompts()->push_back(new Prompt("command : ", &Binder::queryCommandForPattern));
    action->prompts()->push_back(new Prompt("action name : ", 0));
    (*actionBindings)["create-action"] = action;

    action = new Action("create-workspace", &Actions::createWorkspace, &Validators::isAlwaysPossible);
    action->prompts()->push_back(new Prompt("workspace name : ", 0));
    (*actionBindings)["create-workspace"] = action;

    (*actionBindings)["destroy-frame"] = new Action("destroy-frame", &Actions::destroyFrame, &Validators::validateDestroyFrame);
    (*actionBindings)["destroy-workspace"] = new Action("destroy-workspace", &Actions::destroyWorkspace, &Validators::validateDestroyWorkspace);

    action = new Action("bind-shortcut", &Actions::bindShortcut,
            &Validators::isAlwaysPossible);
    action->prompts()->push_back(new Prompt("action to bind : ",
                &Binder::queryActionKeysWithoutValidationForPattern));
    action->prompts()->push_back(new Prompt("keys <modifier>+<key> : ", 0));
    (*actionBindings)["bind-shortcut"] = action;

    action = new Action("rename-workspace", &Actions::renameWorkspace, &Validators::isWorkspaceFocused);
    action->prompts()->push_back(new Prompt("new name : ", 0));
    (*actionBindings)["rename-workspace"] = action;

    (*actionBindings)["select-frame-left"] = new Action("select-frame-left", &Actions::selectFrameLeft, &Validators::existsFrameLeft);
    (*actionBindings)["select-frame-right"] = new Action("select-frame-right", &Actions::selectFrameRight, &Validators::existsFrameRight);
    (*actionBindings)["select-frame-up"] = new Action("select-frame-up", &Actions::selectFrameUp, &Validators::existsFrameUp);
    (*actionBindings)["select-frame-down"] = new Action("select-frame-down", &Actions::selectFrameDown, &Validators::existsFrameDown);

    (*actionBindings)["swap-frame-left"] = new Action("swap-frame-left", &Actions::swapFrameLeft, &Validators::existsFrameLeft);
    (*actionBindings)["swap-frame-right"] = new Action("swap-frame-right", &Actions::swapFrameRight, &Validators::existsFrameRight);
    (*actionBindings)["swap-frame-up"] = new Action("swap-frame-up", &Actions::swapFrameUp, &Validators::existsFrameUp);
    (*actionBindings)["swap-frame-down"] = new Action("swap-frame-down", &Actions::swapFrameDown, &Validators::existsFrameDown);

    (*actionBindings)["swap-client-left"] = new Action("swap-client-left", &Actions::swapClientLeft, &Validators::existsFrameLeft);
    (*actionBindings)["swap-client-right"] = new Action("swap-client-right", &Actions::swapClientRight, &Validators::existsFrameRight);
    (*actionBindings)["swap-client-up"] = new Action("swap-client-up", &Actions::swapClientUp, &Validators::existsFrameUp);
    (*actionBindings)["swap-client-down"] = new Action("swap-client-down", &Actions::swapClientDown, &Validators::existsFrameDown);

    (*actionBindings)["toggle-max"] = new Action("toggle-max", &Actions::toggleMaximization, &Validators::isFrameOrClientFrameFocused);

    (*actionBindings)["raise"] = new Action("raise", &Actions::raise, &Validators::isClientFocused);
    (*actionBindings)["lower"] = new Action("lower", &Actions::lower, &Validators::isClientFocused);
    (*actionBindings)["fit-client"] = new Action("fit-client", &Actions::fitClient, &Validators::isClientFocused);
    (*actionBindings)["banish"] = new Action("banish", &Actions::banish, &Validators::isAlwaysPossible);
    (*actionBindings)["toggle-clientbar-mode"] = new Action("toggle-clientbar-mode", &Actions::toggleClientBarMode, &Validators::isAlwaysPossible);
    (*actionBindings)["toggle-sloppy-mode"] = new Action("toggle-sloppy-mode", &Actions::toggleSloppyMode, &Validators::isAlwaysPossible);
    (*actionBindings)["toggle-shortcuts"] = new Action("toggle-shortcuts", &Actions::toggleShortcuts, &Validators::isAlwaysPossible);
    (*actionBindings)["grab-move"] = new Action("grab-move", &Actions::grabMove, &Validators::isClientFrameFocused);
    (*actionBindings)["toggle-tiled"] = new Action("toggle-tiled", &Actions::toggleTiled, &Validators::existClientsWithinFrame);
    (*actionBindings)["zoom-client"] = new Action("zoom-client", &Actions::zoomClient, &Validators::existClientsWithinFrame);

}

void Actions::selectFrameLeft(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->selectFrame(LEFT);
}

void Actions::selectFrameRight(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->selectFrame(RIGHT);
}

void Actions::selectFrameUp(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->selectFrame(UP);
}

void Actions::selectFrameDown(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->selectFrame(DOWN);
}

void Actions::toggleShortcuts(Action *caller, const char *argument) {
    KERNEL->toggleShortcuts();
}

void Actions::restart(Action *caller, const char *argument) {
    KERNEL->restart();
}

void Actions::quit(Action *caller, const char *argument) {
    KERNEL->stop();
}

void Actions::saveSettings(Action *caller, const char *argument) {
    KERNEL->saveSettings();
}

void Actions::execute(Action *caller, const char *command) {

    Launcher::instance()->exec(command);
}

void Actions::executeTerm(Action *caller, const char *command) {
    MSettings *commonSettings = KERNEL->commonSettings();
    string cmd = Util::get(commonSettings, (string)"terminal") + " " + command;

    Launcher::instance()->exec(cmd);
}

void Actions::cycleClientNext(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->cycleClientNext();
}

void Actions::cycleClientPrev(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->cycleClientPrev();
}

void Actions::attachAllClients(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    monitor->attachAllClients();
}

void Actions::detachAllClients(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    monitor->detachAllClients();
}

void Actions::detachClient(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    monitor->detachClient();
}

#ifdef SLOT_SUPPORT
void Actions::unslotClient(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->unslotClient();
}

void Actions::slotClient(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->slotClient();
}

void Actions::toggleSlot(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->toggleSlot();
}

void Actions::cycleSlotTabNext(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    monitor->cycleSlotTabNext();
}

void Actions::cycleSlotTabPrev(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    monitor->cycleSlotTabPrev();
}

void Actions::killSlotClient(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    KERNEL->killClient(monitor->slot()->focused()->focused());
}

#endif // SLOT_SUPPORT

void Actions::attachClient(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();

    monitor->attachClientByName(argument);
}

void Actions::attachLastClient(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();

    monitor->attachLastClient();
}

void Actions::toggleClientSticky(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->toggleClientSticky();
}

void Actions::toggleClientMode(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->toggleClientMode();
}

void Actions::toggleMode(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();

    workspace->toggleMode();
}

void Actions::cycleWorkspaceNext(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->focus(monitor->next());
}

void Actions::cycleWorkspacePrev(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->focus(monitor->prev());
}

void Actions::growLeft(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, LEFT, true);
}

void Actions::growRight(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, RIGHT, true);
}

void Actions::growUp(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, UP, true);
}

void Actions::growDown(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, DOWN, true);
}

void Actions::shrinkLeft(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, LEFT, false);
}

void Actions::shrinkRight(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, RIGHT, false);
}

void Actions::shrinkUp(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, UP, false);
}

void Actions::shrinkDown(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Thing *thing = workspace->focusedThing();
    assert(thing);

    workspace->resize(thing, DOWN, false);
}

void Actions::moveClientLeft(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->moveClient(LEFT);
    }
}

void Actions::moveClientRight(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->moveClient(RIGHT);
    }
}

void Actions::moveClientUp(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->moveClient(UP);
    }
}

void Actions::moveClientDown(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->moveClient(DOWN);
    }
}

void Actions::sendClientLeft(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->sendClient(LEFT);
    }
}

void Actions::sendClientRight(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->sendClient(RIGHT);
    }
}

void Actions::sendClientUp(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->sendClient(UP);
    }
}

void Actions::sendClientDown(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Client *client = monitor->focusedClient();
    if (client) {
        workspace->sendClient(DOWN);
    }
}

void Actions::joinFrameLeft(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->joinFrame(LEFT);
}

void Actions::joinFrameRight(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->joinFrame(RIGHT);
}

void Actions::joinFrameUp(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->joinFrame(UP);
}

void Actions::joinFrameDown(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->joinFrame(DOWN);
}

void Actions::splitFrameLeft(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->splitFrame(LEFT);
}

void Actions::splitFrameRight(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->splitFrame(RIGHT);
}

void Actions::splitFrameUp(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->splitFrame(UP);
}

void Actions::splitFrameDown(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->splitFrame(DOWN);
}

void Actions::showBorders(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->toggleBorders(true);
}

void Actions::hideBorders(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->toggleBorders(false);
}

void Actions::toggleBorder(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Thing *thing = workspace->focusedThing();
    if (thing) {
        if (thing->borderWidth()) {
            thing->setBorderWidth(0);
        }
        else {
            thing->setBorderWidth(KERNEL->borderWidth());
        }
        thing->resize();
    }
}

void Actions::showBars(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->toggleBars(true);
}

void Actions::hideBars(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    workspace->toggleBars(false);
}

void Actions::toggleBar(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    assert(workspace);

    Thing *thing = workspace->focusedThing();
    if (thing) {
        thing->setTitleBarHeight(thing->titleBarHeight() ? 
                                 0 : monitor->titleBarHeight());
        thing->resize();
    }
}

void Actions::toggleClientBar(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->toggleClientBar();
}

void Actions::toggleStatusBar(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->toggleStatusBar();
}

void Actions::inputMode(Action *caller, const char *argument) {

    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->inputBar()->runKey(KERNEL->defaultPrompt());
}

void Actions::killClient(Action *caller, const char *argument) {

    KERNEL->killClient(KERNEL->focusedClient());
}

void Actions::sequence(Action *caller, const char *command) {

    Launcher::instance()->execSeq(caller, command);
}

void Actions::createAction(Action *caller, const char *argument) {

    string arg = argument;
    LOGDEBUG("arg: " + arg);
    unsigned int argDelim = arg.find_last_of('+');
    if (argDelim == string::npos) {
        return;
    }
    string cmd = arg.substr(0, argDelim);
    string bind = arg.substr(argDelim + 1);

    if ((bind.length() < 1) || (cmd.length() < 1)) {
        return;
    }
    Action *action = new Action(bind, &Actions::execute,
                                &Validators::isWorkspaceFocused, Action::EXTERN,
                                (char *)cmd.c_str());
    (*KERNEL->actionBindings())[bind] = action;
    (*KERNEL->actionSettings())["extern." + action->id() + ".cmd"] = cmd;
}

void Actions::createWorkspace(Action *caller, const char *argument) {

    assert(argument != 0);

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    focusedMonitor->createNewWorkspace(argument);
}


void Actions::bindShortcut(Action *caller, const char *argument) {

    KERNEL->bindShortcut(argument);
}

// TODO: ungrab potentially keys before removing
void Actions::destroyAction(Action *caller, const char *argument) {

    Action *action = Util::get(KERNEL->actionBindings(), argument);
    if (action && (action->type() != Action::INTERN)) {

        Util::remove(KERNEL->actionBindings(), argument);
        string prefix = (action->type() == Action::EXTERN) ? "extern." :
            "chain.";
        prefix += argument;
        Util::remove(KERNEL->actionSettings(), prefix);
    }
}

void Actions::destroyWorkspace(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    assert(monitor != 0);

    monitor->destroyWorkspace(monitor->focused());
}

void Actions::selectMonitor(Action *caller, const char *argument) {

    KERNEL->selectMonitor(argument);
}

void Actions::selectWorkspace(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->workspaceForName(argument);
    if (workspace) {
        focusedMonitor->focus(workspace);
    }
}

void Actions::selectClientId(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    focusedMonitor->focusClientById(argument);
}

void Actions::selectClient(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    focusedMonitor->focusClientByName(argument);
}

void Actions::renameWorkspace(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    assert(focusedMonitor != 0);

    focusedMonitor->renameWorkspace(focusedMonitor->focused(),
                                    argument);
}

void Actions::swapFrameLeft(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapFrame(LEFT);
}

void Actions::swapFrameRight(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapFrame(RIGHT);
}

void Actions::swapFrameUp(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapFrame(UP);
}

void Actions::swapFrameDown(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapFrame(DOWN);
}

void Actions::swapClientLeft(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapClient(LEFT);
}

void Actions::swapClientRight(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapClient(RIGHT);
}

void Actions::swapClientUp(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapClient(UP);
}

void Actions::swapClientDown(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();
    assert(workspace);

    workspace->swapClient(DOWN);
}


void Actions::toggleMaximization(Action *caller, const char *argument) {
    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    focusedMonitor->toggleThingMaximization();
}

void Actions::toggleClientBarMode(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    ClientBar *clientBar = focusedMonitor->clientBar();

    clientBar->setMode((clientBar->mode() == ClientBar::PAGER) ?
                        ClientBar::CLIENTINFO : ClientBar::PAGER);
    clientBar->illuminate();
}

void Actions::fitClient(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    workspace->fitClient();
}

void Actions::lower(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    workspace->lower();
}

void Actions::raise(Action *caller, const char *argument) {

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    Workspace *workspace = focusedMonitor->focused();

    workspace->raise();
}

void Actions::rehash(Action *caller, const char *argument) {

    Expander::instance()->rehash();
}

void Actions::beginChainRecord(Action *caller, const char *argument) {
    KERNEL->beginRecording();
}

void Actions::endChainRecord(Action *caller, const char *argument) {
    KERNEL->endChainRecording(argument);
}

void Actions::endScriptRecord(Action *caller, const char *argument) {
    KERNEL->endScriptRecording(argument);
}

void Actions::cancelRecord(Action *caller, const char *argument) {
    KERNEL->cancelRecording();
}

void Actions::banish(Action *caller, const char *argument) {
    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    focusedMonitor->banish();
}

void Actions::unhookClient(Action *caller, const char *argument) {
    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    focusedMonitor->unhookClient();
}

void Actions::hookClient(Action *caller, const char *argument) {
    Monitor *focusedMonitor = KERNEL->focusedMonitor();

    focusedMonitor->hookClient();
}

void Actions::toggleSloppyMode(Action *caller, const char *argument) {
    KERNEL->toggleSloppyMode();
}

void Actions::grabMove(Action *caller, const char *argument) {
    KERNEL->grabMove();
}

void Actions::zoomClient(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused(); 
    Frame *frame = workspace->focusedFrame();

    if (frame) {
        frame->zoomClient();
    }
}

void Actions::toggleTiled(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();
    Frame *frame = workspace->focusedFrame();
    if (frame) {
        frame->toggleTiled();
    }
}

void Actions::destroyFrame(Action *caller, const char *argument) {
    Monitor *monitor = KERNEL->focusedMonitor();
    Workspace *workspace = monitor->focused();

    if (workspace->focusedFrame()) {
        workspace->destroyFrame(workspace->focusedFrame());
    }
}
