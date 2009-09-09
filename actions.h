// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: actions.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __ACTIONS_H
#define __ACTIONS_H

#include "singleton.h"

#include <map>
#include "wmi.h"

class Action;

class Bindings;
class Kernel;
class Monitor;

typedef map<string, Action *, less<string> > MBindings;

/**
 * Actions singleton class which delegates end-user interface to the
 * appropriate business logic of WMI.
 */
class Actions : public Singleton<Actions>
{

public:
    Actions();
    ~Actions();

    void initInternActions(MBindings *actionBindings);

    // new macro record stuff
    void beginChainRecord(Action *caller, const char *argument);
    void endChainRecord(Action *caller, const char *argument);
    void endScriptRecord(Action *caller, const char *argument);
    void cancelRecord(Action *caller, const char *argument);

    // intern actions
    void quit(Action *caller, const char *argument);
    void restart(Action *caller, const char *argument);

    void rehash(Action *caller, const char *argument);

    void circulateWindowsUp(Action *caller, const char *argument);

    void toggleClientMode(Action *caller, const char *argument);
    void toggleClientSticky(Action *caller, const char *argument);
    void executeTerm(Action *caller, const char *command);

    void growLeft(Action *caller, const char *argument);
    void growRight(Action *caller, const char *argument);
    void growUp(Action *caller, const char *argument);
    void growDown(Action *caller, const char *argument);

    void shrinkLeft(Action *caller, const char *argument);
    void shrinkRight(Action *caller, const char *argument);
    void shrinkUp(Action *caller, const char *argument);
    void shrinkDown(Action *caller, const char *argument);

    void moveClientLeft(Action *caller, const char *argument);
    void moveClientRight(Action *caller, const char *argument);
    void moveClientUp(Action *caller, const char *argument);
    void moveClientDown(Action *caller, const char *argument);

    void showBars(Action *caller, const char *argument);
    void hideBars(Action *caller, const char *argument);
    void toggleBar(Action *caller, const char *argument);
    void toggleBorder(Action *caller, const char *argument);
    void showBorders(Action *caller, const char *argument);
    void hideBorders(Action *caller, const char *argument);
    void toggleStatusBar(Action *caller, const char *argument);
    void toggleClientBar(Action *caller, const char *argument);
    void inputMode(Action *caller, const char *argument);
    void toggleMode(Action *caller, const char *argument);

    void killClient(Action *caller, const char *argument);

    void cycleClientNext(Action *caller, const char *argument);
    void cycleWorkspaceNext(Action *caller, const char *argument);
    void cycleClientPrev(Action *caller, const char *argument);
    void cycleWorkspacePrev(Action *caller, const char *argument);

    void joinFrameLeft(Action *caller, const char *argument);
    void joinFrameRight(Action *caller, const char *argument);
    void joinFrameUp(Action *caller, const char *argument);
    void joinFrameDown(Action *caller, const char *argument);

    void sendClientLeft(Action *caller, const char *argument);
    void sendClientRight(Action *caller, const char *argument);
    void sendClientUp(Action *caller, const char *argument);
    void sendClientDown(Action *caller, const char *argument);

    void splitFrameLeft(Action *caller, const char *argument);
    void splitFrameRight(Action *caller, const char *argument);
    void splitFrameUp(Action *caller, const char *argument);
    void splitFrameDown(Action *caller, const char *argument);

    void attachClient(Action *caller, const char *argument);
    void attachLastClient(Action *caller, const char *argument);
    void detachClient(Action *caller, const char *argument);
    void detachAllClients(Action *caller, const char *argument);
    void attachAllClients(Action *caller, const char *argument);

    void createAction(Action *caller, const char *argument);
    void createWorkspace(Action *caller, const char *argument);

    void destroyAction(Action *caller, const char *argument);
    void destroyFrame(Action *caller, const char *argument);
    void destroyWorkspace(Action *caller, const char *argument);

    void bindShortcut(Action *caller, const char *argument);

    void renameWorkspace(Action *caller, const char *argument);
    void selectWorkspace(Action *caller, const char *argument);
    void selectMonitor(Action *caller, const char *argument);
    void selectClient(Action *caller, const char *argument);
    void selectClientId(Action *caller, const char *argument);

    void selectFrameLeft(Action *caller, const char *argument);
    void selectFrameRight(Action *caller, const char *argument);
    void selectFrameUp(Action *caller, const char *argument);
    void selectFrameDown(Action *caller, const char *argument);

    void swapFrameLeft(Action *caller, const char *argument);
    void swapFrameRight(Action *caller, const char *argument);
    void swapFrameUp(Action *caller, const char *argument);
    void swapFrameDown(Action *caller, const char *argument);

    void swapClientLeft(Action *caller, const char *argument);
    void swapClientRight(Action *caller, const char *argument);
    void swapClientUp(Action *caller, const char *argument);
    void swapClientDown(Action *caller, const char *argument);

#ifdef SLOT_SUPPORT
    void unslotClient(Action *caller, const char *argument);
    void slotClient(Action *caller, const char *argument);
    void toggleSlot(Action *caller, const char *argument);
    void cycleSlotTabPrev(Action *caller, const char *argument);
    void cycleSlotTabNext(Action *caller, const char *argument);
    void killSlotClient(Action *caller, const char *argument);
#endif // SLOT_SUPPORT

    void toggleMaximization(Action *caller, const char *argument);
    void toggleShortcuts(Action *caller, const char *argument);

    void lower(Action *caller, const char *argument);
    void raise(Action *caller, const char *argument);
    void saveSettings(Action *caller, const char *argument);

    // extern actions only use execute
    void execute(Action *caller, const char *command);

    // map sequences only use sequences
    void sequence(Action *caller, const char *command);

    void hookClient(Action *caller, const char *argument);
    void unhookClient(Action *caller, const char *argument);
    void fitClient(Action *caller, const char *argument);
    void banish(Action *caller, const char *argument);
    void toggleClientBarMode(Action *caller, const char *argument);
    void toggleSloppyMode(Action *caller, const char *argument);
    void grabMove(Action *caller, const char *argument);
    void toggleTiled(Action *caller, const char *argument);
    void zoomClient(Action *caller, const char *argument);
};

#endif // __ACTIONS_H
