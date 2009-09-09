// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include <assert.h>
#include <stdlib.h> // getenv stuff
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
}

#include <sstream>

#include "kernel.h"

#include "action.h"
#include "actions.h"
#include "atoms.h"
#include "binder.h"
#include "box.h"
#include "client.h"
#include "clientbar.h"
#include "cursors.h"
#include "container.h"
#include "draw.h"
#include "expander.h"
#include "float.h"
#include "frame.h"
#include "inputbar.h"
#include "launcher.h"
#include "loader.h"
#include "logger.h"
#include "menu.h"
#include "monitor.h"
#include "prompt.h"
#include "shortcut.h"
#include "slot.h"
#include "split.h"
#include "statusbar.h"
#include "thing.h"
#include "util.h"
#include "validators.h"
#include "workspace.h"
#include "xcore.h"

Kernel::Kernel() {
}

void Kernel::initMonitors() {

    unsigned int monitorCount = ScreenCount(display_);

    for (unsigned int i = 0; i < monitorCount; i++) {
        Monitor *monitor = new Monitor(display_, i);
        monitors_->attach(monitor);
    }
}

int Kernel::start(char *argv[], Display *display, MSettings *themeSettings,
                   MSettings *commonSettings, MSettings *actionSettings,
                   MSettings *sessionSettings, bool isFirstRun)
{

    runlevel_ = START;

    // init, order is very important
    display_ = display;
    monitors_ = new CMonitor(this);
    themeSettings_ = themeSettings;
    commonSettings_ = commonSettings;
    actionSettings_ = actionSettings;
    sessionSettings_ = sessionSettings;
    actionBindings_ = new MBindings();
    isRecording_ = false;
    isMenuMode_ = false;
    isSloppyMode_ = true;
    isStackedTabbing_ = Util::get(commonSettings_, "cycle.mode") != "default";
    borderWidth_ = Util::strToUInt(Util::get(commonSettings_, "border.width"));

    LOGDEBUG("--- begin of new WMI session ---");
    XSetErrorHandler(XCore::handleErrors);
    LOGDEBUG("error handler installed");
    XCORE->setDisplay(display_);
    LOGDEBUG("xcore initialized");
    Atoms::initAtoms();
    LOGDEBUG("atoms initialized");
    Cursors::initCursors();
    LOGDEBUG("cursors initialized");
    Actions::instance()->initInternActions(actionBindings_);
    LOGDEBUG("intern actions initialized");
    initActionBindings();
    LOGDEBUG("action bindings initialized");
    initMonitors();
    LOGDEBUG("monitors initialized");
    initKeys();
    LOGDEBUG("keys initialized");
    Launcher::instance()->exec(
            Util::get(themeSettings_, "exec"));
    if (isFirstRun) {
        Actions::instance()->executeTerm(0, "man wmi");
    }
    // Sync all X buffers and wait until all events has been
    // processed
    XCORE->sync();
    initWindows();
    LOGDEBUG("windows initialized");

    defaultPrompt_ = new Prompt(": ", &Binder::queryActionKeysForPattern);

    Action* startupAction = new Action("startup", &Actions::sequence,
                                       &Validators::isAlwaysPossible,
                                       Action::SEQUENCE, 
                                       strdup(Util::get(commonSettings_,
                                                 "startup.chain").c_str()));
    startupAction->perform();
    delete startupAction;
    Expander::instance(); // rehash expander

    // Launch main event loop
    runlevel_ = RUN;
    int result = run();

    serialize();
    LOGDEBUG("current state serialized");
    LOGDEBUG("--- end of WMI session ---");

    cleanup();

    if (runlevel_ == RESTART) {
        saveSettings();
        execvp(argv[0], argv);
        LOGERROR("restart failed", false);
        exit(1);
    }

    return result;
}

Kernel::~Kernel() {
}

Prompt *Kernel::defaultPrompt() const {
    return defaultPrompt_;
}

Window Kernel::defaultRootWindow() {
    return DefaultRootWindow(display_);
}

Window Kernel::rootWindow() {
    return focusedMonitor()->rootWindow();
}

Client *Kernel::focusedClient() {

    assert(focusedMonitor() != 0); // always, while running
    Workspace *focusedWorkspace = focusedMonitor()->focused();
    assert(focusedWorkspace != 0);

    return focusedWorkspace->topClient();
}

void Kernel::killClient(Client *client) {
    if (client) {
        XCORE->kill(client->clientWindow(), client->protocols());
    }
}

void Kernel::updateBars() {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        monitor->statusBar()->illuminate();
        monitor->clientBar()->illuminate();
    }
}

void Kernel::initWindows() {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        monitor->scanWindows();
    }
}

void Kernel::initKeys() {

    XCORE->ungrabKeyboard();
    Binder *bm = Binder::instance();

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        bm->initKeys(monitor->rootWindow());
    }
}

void Kernel::initActionBindings() {

    Action::Type type = Action::UNKNOWN;

    string settingsKey, bind, postfix, spec, keys;
    spec = "";
    Action *action = 0;

    // extract all extern actions and chain sequences
    for (MSettings::iterator it = actionSettings_->begin();
         it != actionSettings_->end(); it++)
    {
        settingsKey = (*it).first;
        if (settingsKey.substr(0, 6) == "extern") {
            type = Action::EXTERN;
        }
        else if (settingsKey.substr(0, 5) == "chain") {
            type = Action::SEQUENCE;
        }
        else {
            type = Action::UNKNOWN;
        }

        if (type != Action::UNKNOWN) { // settingsKey has a valid action prefix
            bind = Util::nthToken(settingsKey, '.', 2);
            LOGDEBUG("found " + bind);
            postfix = Util::nthToken(settingsKey, '.', 3);

            if ((type == Action::EXTERN) && (postfix == "cmd")) {
                spec = (*it).second;
            }
            else if ((type == Action::SEQUENCE) && (postfix == "seq")) {
                spec = (*it).second;
            }

            if (spec != "") {

                if (type == Action::EXTERN) {
                    action = new Action(bind, &Actions::execute,
                                        &Validators::isWorkspaceFocused, type,
                                        (char *)spec.c_str());
                }
                else if (type == Action::SEQUENCE) {

                    // We determine the first sequential action and use
                    // its possibility method (or if it's user defined we
                    // allow sequential performing under all
                    // circumstances).
                    string firstActionKey = Util::nthToken(spec, ',', 1);
                    IsValid isValid = 0;
                    Action *firstAction =
                        Util::get(actionBindings_, firstActionKey);
                    if (firstAction) {
                        isValid = firstAction->getIsValid();
                    }
                    else {
                        isValid = &Validators::isAlwaysPossible;
                    }
                    action = new Action(bind, &Actions::sequence,
                                        isValid, type, (char *)spec.c_str());
                }
            }

            if (action != 0) {

                LOGDEBUG("creating action: " + bind);
                (*actionBindings_)[bind] = action;
                action = 0;
                spec = "";
            }
        }
    }

    // extract all key bindings, if they're defined
    for (MSettings::iterator it = actionSettings_->begin();
         it != actionSettings_->end(); it++)
    {
        settingsKey = (*it).first;
        if (settingsKey.substr(0, 6) == "extern") {
            type = Action::EXTERN;
        }
        else if (settingsKey.substr(0, 6) == "intern") {
            type = Action::INTERN;
        }
        else if (settingsKey.substr(0, 5) == "chain") {
            type = Action::SEQUENCE;
        }
        else {
            type = Action::UNKNOWN;
        }

        if (type != Action::UNKNOWN) {

            bind = Util::nthToken(settingsKey, '.', 2);
            LOGDEBUG("found " + bind);
            postfix = Util::nthToken(settingsKey, '.', 3);

            // settingsKey has a valid action prefix
            if (postfix == "keys") {
                keys = (*it).second;

                if (keys != "") {

                    // fetch action
                    action = Util::get(actionBindings_, bind);
                    if (action) {
                        action->setShortcut(Shortcut::shortcut(keys));
                    }

                    keys = "";
                }
            }
        }
    }
}

bool Kernel::isWMIWindow(Window window) {

    return isRootWindow(window) ||
#ifdef SLOT_SUPPORT
           slotWindow(window) ||
#endif // SLOT_SUPPORT
           menuWindow(window) ||
           barWindow(window) ||
           boxWindow(window) ||
           thingWindow(window);
}

bool Kernel::isRootWindow(Window window) {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        if (monitor->rootWindow() == window) {
            monitors_->focus(monitor);
            return true;
        }
    }

    return false;
}

Menu *Kernel::menuWindow(Window window) {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        Menu *menu = monitor->menuWindow(window); 
        if (menu) {
            monitors_->focus(monitor);
            return menu;
        }
    }

    return 0;
}

#ifdef SLOT_SUPPORT
Slot *Kernel::slotWindow(Window window) {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        Slot *slot = monitor->slotWindow(window);
        if (slot) {
            monitors_->focus(monitor);
            return slot;
        }
    }

    return 0;
}
#endif // SLOT_SUPPORT

Thing *Kernel::thingWindow(Window window) {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        Thing *thing = monitor->thingWindow(window);
        if (thing) {
            monitors_->focus(monitor);
            return thing;
        }
    }

    return 0;
}

Box *Kernel::boxWindow(Window window) {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        Box *box = monitor->box();
        if (box->window() == window) {
            return box;
        }
    }
    return 0;
}

Bar *Kernel::barWindow(Window window) {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        Bar *bar = monitor->barWindow(window);
        if (bar) {
            monitors_->focus(monitor);
            return bar;
        }
    }

    return 0;
}

Client *Kernel::clientForWindow(Window window) {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        Client *client = monitor->clientForWindow(window);
        if (client != 0) {
            monitors_->focus(monitor);
            return client;
        }
    }
    return 0;
}

bool Kernel::isInputMode() {
    return focusedMonitor()->inputBar()->isVisible();
}

void Kernel::ungrabShortcutOnAllMonitors(Shortcut *shortcut) {

    Binder *bm = Binder::instance();

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        bm->ungrabShortcut(shortcut, monitor->rootWindow());
    }
}

void Kernel::grabShortcutOnAllMonitors(Shortcut *shortcut) {

    Binder *bm = Binder::instance();

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = (Monitor *)*it;
        bm->grabShortcut(shortcut, monitor->rootWindow());
    }
}

void Kernel::toggleShortcuts() {

    static bool isGrabbed = false;

    isGrabbed = !isGrabbed;
    for (MBindings::iterator it = actionBindings_->begin();
            it != actionBindings_->end(); it++)
    {
        Action *action = (*it).second;
        Shortcut *shortcut = action->listenOn();
        if (shortcut
            && (action->getToPerform() != &Actions::inputMode)
            && (action->getToPerform() != &Actions::toggleShortcuts))
        {
            if (isGrabbed) {
                ungrabShortcutOnAllMonitors(shortcut);
            }
            else {
                grabShortcutOnAllMonitors(shortcut);
            }
        }
    }
}

void Kernel::bindShortcut(string argument) {

    unsigned int argDelim = argument.find_first_of('+');
    if (argDelim == string::npos) {
        return;
    }
    string bind = argument.substr(0, argDelim);
    string keys = argument.substr(argDelim + 1);

    Action *action = Util::get(actionBindings_, bind);

    if (action) {
        Shortcut *oldShortcut = action->shortcut();
        if (oldShortcut) {
            bool doUngrab = true;
            for (MBindings::iterator it = actionBindings_->begin();
                    it != actionBindings_->end(); it++)
            {
                Action *action = (*it).second;
                Shortcut *s = action->listenOn();
                if (s &&
                    (s->modMask() == oldShortcut->modMask()) &&
                    (s->keyCode() == oldShortcut->keyCode()) &&
                    (s->button() == oldShortcut->button()))
                {
                    doUngrab = false;
                    break;
                }
            }
            if (doUngrab) {
                ungrabShortcutOnAllMonitors(oldShortcut);
            }
            delete oldShortcut;
        }

        string prefix;
        switch (action->type()) {
        case Action::INTERN:
            prefix = "intern.";
            break;
        case Action::EXTERN:
            prefix = "extern.";
            break;
        case Action::SEQUENCE:
            prefix = "chain.";
            break;
        case Action::UNKNOWN:
            prefix = "unknown.";
            break;
        }

        if (keys == "") {
            Util::remove(actionSettings_, prefix + bind + ".keys");
            return;
        }
        action->setShortcut(Shortcut::shortcut(keys));

        (*actionSettings_)[prefix + bind + ".keys"] = keys;
        if (action->listenOn()) {
            grabShortcutOnAllMonitors(action->listenOn());
        }
    }
}

void Kernel::serialize() {

    for (LMonitor::iterator it = monitors_->begin();
         it != monitors_->end(); it++)
    {
        Monitor *monitor = *it;
        monitor->serialize();
    }
}

void Kernel::installCursor(Cursor cursor, Window window) {
    XSetWindowAttributes attr;
    attr.cursor = cursor;
    XCORE->setWindowAttributes(window, CWCursor, &attr);
}


void Kernel::addClient(Client *client) {

    focusedMonitor()->addClient(client);
}

void Kernel::saveSettings() {

    // actions and session will be safed under any circumstances saved
    // comments will be lost in these files
    const char *home = getenv("HOME");

    Loader::saveSettings(actionSettings_, (string)home +
            "/.wmi/actions.session");
    Loader::saveSettings(sessionSettings_, (string)home +
            "/.wmi/wmi.session");
}

void Kernel::cleanup() {

    for (LMonitor::iterator it = monitors_->begin();
            it != monitors_->end(); it++)
    {
        Monitor *monitor = *it;
        monitor->cleanup();
    }
    XCORE->setInputFocus(PointerRoot);
    Cursors::cleanup(); 
    delete monitors_;
    // TODO: theme, etc.
    XCloseDisplay(display_);
}

//////// EVENT HANDLING //////////////

void Kernel::handleButtonRelease(XButtonEvent *event) {

    event->state &= Binder::instance()->validModMask();

    Bar *bar = barWindow(event->window);
    if (bar) {
        bar->handleButtonRelease(event);
        return;
    }

    Thing *thing = thingWindow(event->window);
    if (thing) {
        thing->handleButtonRelease(event);
    }
}

void Kernel::handleButtonPress(XButtonEvent *event) {

    event->state &= Binder::instance()->validModMask();

    if (event->state != 0) {
        // no AnyModifier, so this seems to be shortcut
        Binder::instance()->handleButton(event->root, event);
        return;
    }

    Menu *menu = menuWindow(event->window);
    if (menu) {
        menu->handleButtonPress(event);
        return;
    }

    Bar *bar = barWindow(event->window);
    if (bar) {
        bar->handleButtonPress(event);
        return;
    }

#ifdef SLOT_SUPPORT
    Slot *slot = slotWindow(event->window);
    if (slot) {
        slot->handleButtonPress(event);
        return;
    }
#endif

    Thing *thing = thingWindow(event->window);

    if (thing) {
        thing->handleButtonPress(event);
    }

    Client *client = clientForWindow(event->window);
    if (client) {
        client->handleButtonPress(event);
    }
}

void Kernel::handleConfigureRequest(XConfigureRequestEvent *event) {

    Client *client = clientForWindow(event->window);

    if (client) {
        client->handleConfigureRequest(event);
    }
    else {
        XWindowChanges wc;

        wc.x = event->x;
        wc.y = event->y;
        wc.width = event->width;
        wc.height = event->height;
        wc.border_width = 0;
        wc.sibling = None;
        wc.stack_mode = Above;
        event->value_mask &= ~CWStackMode;
        event->value_mask |= CWBorderWidth;

        XCORE->configureWindow(event->window, event->value_mask, &wc);
    }
}

void Kernel::handleDestroyNotify(XDestroyWindowEvent *event) {

    Client *client = clientForWindow(event->window);

    if (client) {
        if (event->window == client->clientWindow()) {
            client->setDestroyed(true);
            client->monitor()->removeClient(client);
            delete client;
        }
    }
}

void Kernel::handleExpose(XExposeEvent *event) {

    Bar *bar = barWindow(event->window);
    if (bar) {
        bar->illuminate();
        return;
    }

#ifdef SLOT_SUPPORT
    Slot *slot = slotWindow(event->window);
    if (slot) {
        slot->illuminate();
        return;
    }
#endif // SLOT_SUPPORT

    Thing *thing = thingWindow(event->window);
    if (thing) {
        thing->illuminate();
    }
}

void Kernel::handleKeyPress(XKeyEvent *event) {

    event->state &= Binder::instance()->validModMask();

    if (isInputMode()) {
        focusedMonitor()->inputBar()->handleInput(event);
    }
    else {
        Binder::instance()->handleKey(event->root, event);
        updateBars();
    }
}

void Kernel::handleMapRequest(XMapRequestEvent *event) {

    if (isWMIWindow(event->window)) {
        return;
    }

    XWindowAttributes attr;
    XCORE->windowAttributes(event->window, &attr);

    if (attr.override_redirect) {
        LOGDEBUG("override redirect!");
        return;
    }

    Client *client = clientForWindow(event->window);

    if (!client) {
        client = new Client(focusedMonitor(), event->window, &attr);
        focusedMonitor()->addClient(client);
        LOGDEBUG("client created");
    }
    else if (client->attached()) {
        client->setRequestsFocus(true);
        client->attached()->setRequestsFocus(true);
        return; // we ignore Map Requests from those clients
    }

    ostringstream oss;
    oss << "client dims: " << client->x() << "," <<
        client->y() << "," << client->width() << "," <<
        client->height();
    LOGDEBUG(oss.str());

    /** Initialize names. */
    focusedMonitor()->attachClient(client);
}

void Kernel::handleMotionNotify(XMotionEvent *event) {

    Menu *menu = menuWindow(event->window);
    if (menu) {
        menu->handleMotionNotify(event);
    }

    Bar *bar = barWindow(event->window);
    if (bar) {
        bar->handleMotionNotify(event);
        return;
    }

    Thing *thing = thingWindow(event->window);
    if (thing) {
        if (!thing->isFocused() && isSloppyMode_) {
            Workspace *workspace = focusedMonitor()->focused();
            workspace->focus(thing, false);
        }
        thing->handleMotionNotify(event);
    }

    // focusses the monitor if multihead
    isRootWindow(event->root);
}

void Kernel::handlePropertyNotify(XPropertyEvent *event) {

    if (!isRootWindow(event->window)) {

        Client *client = clientForWindow(event->window);
        ostringstream oss;
        oss << "property atom: " << XCORE->atomName(event->atom);
        LOGDEBUG(oss.str());

        if (client) {
            client->handlePropertyNotify(event);
        }
        return;
    }

    // check if status text has been changed
    if (event->atom == Atoms::WMI_STATUSTEXT ||
        event->atom == Atoms::WMI_METERTEXT)
    {
        focusedMonitor()->updateBars();
        return;
    }
    else if (event->atom == Atoms::WMI_ACTIONCMD) {

        // TODO
        // action[+arg1[...]]
        string actionCmd;
        if (XCORE->textProperty(defaultRootWindow(),
                    Atoms::WMI_ACTIONCMD, &actionCmd))
        {
            string bind, arg;
            unsigned int pos = actionCmd.find_first_of('+');
            if (pos != string::npos) {
                bind = actionCmd.substr(0, pos);
                arg = actionCmd.substr(pos + 1);
            }
            else {
                bind = actionCmd;
            }

            // query action
            Action *action = Util::get(actionBindings_, bind);
            if (action && pos != string::npos) {
                action->setArgument((char *)arg.c_str());
            }

            if (action) {
                if ((action->promptsCount() > 0) && !action->argument()) {
                    focusedMonitor()->inputBar()->runArgument(action);
                    // action will be performed by argument processing
                }
                else {
                    action->perform();
                }
            }

        }
        return;
    }
    else if (event->atom == Atoms::WMI_PRETTYPRINT_REQUEST) {

        XTextProperty property;
        XCORE->stringListToTextProperty(
                Binder::instance()->prettyPrintKeyBindings(), &property);
        XCORE->setTextProperty(display_, defaultRootWindow(),
                &property, Atoms::WMI_PRETTYPRINT_RESPONSE);
    }
}

void Kernel::handleUnmapNotify(XUnmapEvent *event) {
    LOGDEBUG("handle unmap notify");
    Client *client = clientForWindow(event->window);

    if (client) {
        LOGDEBUG("got unmap client");
        client->handleUnmapNotify(event);
    }
}

void Kernel::handleClientMessage(XClientMessageEvent *event) {

    LOGDEBUG("handle client message");
    if (event->message_type == Atoms::WM_CHANGE_STATE) {
        Client *client = clientForWindow(event->window);

        if (client  && client->monitor()
           && (event->format == 32)
           && (event->data.l[0] == IconicState))
        {
            client->monitor()->detachClient(client);
        }
    }
}

int Kernel::run() {
    XEvent event;

    LOGDEBUG("main event loop launched");
    ostringstream oss;
    while (runlevel_ == Kernel::RUN) {

        XCORE->nextEvent(&event);
        switch (event.type) {
        case ButtonPress:
            LOGDEBUG("button press event");
            handleButtonPress(&event.xbutton);
            break;
        case ButtonRelease:
            LOGDEBUG("button release event");
            handleButtonRelease(&event.xbutton);
            break;
        case ClientMessage:
            LOGDEBUG("client message event");
            handleClientMessage(&event.xclient);
            break;
        case ConfigureRequest:
            LOGDEBUG("configure request event");
            handleConfigureRequest(&event.xconfigurerequest);
            break;
        case DestroyNotify:
            LOGDEBUG("destroy window event");
            handleDestroyNotify(&event.xdestroywindow);
            break;
        case Expose:
            LOGDEBUG("expose event");
            handleExpose(&event.xexpose);
            break;
        case KeyPress:
            LOGDEBUG("keypress event");
            handleKeyPress(&event.xkey);
            break;
        case MapRequest:
            LOGDEBUG("map request event");
            handleMapRequest(&event.xmaprequest);
            break;
        case MotionNotify:
            LOGDEBUG("motion event");
            handleMotionNotify(&event.xmotion);
            break;
        case PropertyNotify:
            LOGDEBUG("property event");
            handlePropertyNotify(&event.xproperty);
            break;
        case UnmapNotify:
            LOGDEBUG("unmap event");
            handleUnmapNotify(&event.xunmap);
            break;
        }

    }
    return 0;
}

void Kernel::runResizeMode(Thing *thing, XButtonEvent *buttonEvent,
                           Direction dir, bool resize)
{
    Window window = thing->window();
    if (!window) {
        return;
    }

    Monitor *monitor = thing->monitor();
    Window rootWindow = monitor->rootWindow();
    Thing::Type type = thing->type();
    Workspace *workspace = (type == Thing::CLIENT) ?
        ((Client *)thing)->attached() : ((Frame *)thing)->attached();

    if (!workspace) {
        return;
    }

    int origX, origY, lastX, lastY, newX, newY, dx, dy;
    Rectangle rect(*thing);

    XCORE->translateCoordinates(window, rootWindow,
                                 buttonEvent->x, buttonEvent->y,
                                 &lastX, &lastY);
    newX = newY = 0;
    origX = lastX;
    origY = lastY;

    XCORE->grabPointer(window,
                       ButtonMotionMask | ButtonReleaseMask,
                       buttonEvent->time);

    XEvent event;
    bool done = false, found = false, firstMotion = true;

    XCORE->grabServer();
    while (!done) {
        found = false;

        while (XCORE->checkMaskEvent(
                    ButtonReleaseMask | ButtonMotionMask, &event))
        {
            found = true;

            if (event.type != MotionNotify) {
                break;
            }
        }

        if (!found) {
            usleep(20000);
            continue;
        }

        switch (event.type) {
        case ButtonRelease:
            if (!firstMotion) {
                monitor->illuminateTransRect(&rect, thing->titleBarHeight());

                dx = newX - origX;
                dy = newY - origY;

                if ((dir == LEFT) || (dir == UP)) {
                    // normalization
                    dx *= -1;
                    dy *= -1;
                }

                if (type == Thing::FRAME) {
                    Split::resize(workspace->root(),
                                  ((Frame *)thing)->tree(),
                                  dir, dx, dy);
                }
                else {
                    thing->copy(&rect);
                    thing->resize();
                    thing->illuminate();
                }
                XCORE->sync();
            }
            XCORE->ungrabPointer(event.xbutton.time);
            XCORE->ungrabServer();
            done = true;
            break; // ButtonRelease
        case MotionNotify:
            XCORE->translateCoordinates(event.xmotion.window,
                    rootWindow, event.xmotion.x, event.xmotion.y,
                    &newX, &newY);

            dx = newX - lastX;
            dy = newY - lastY;

            lastX = newX;
            lastY = newY;

            if (firstMotion) {
                firstMotion = false;
            }
            else {
                monitor->illuminateTransRect(&rect, thing->titleBarHeight());
            }

            if (type == Thing::FRAME) {
                switch (dir) {
                case LEFT:
                    rect.setX(rect.x() + dx);
                    rect.setWidth(rect.width() - dx);
                    break;
                case RIGHT:
                    rect.setWidth(rect.width() + dx);
                    break;
                case UP:
                    rect.setY(rect.y() + dy);
                    rect.setHeight(rect.height() - dy);
                    break;
                case DOWN:
                    rect.setHeight(rect.height() + dy);
                    break;
                default:
                    break;
                }
            }
            else {

                if (resize) {
                    Float::resize(&rect, dir, dx, dy);
                }
                else {
                    Float::move(&rect, dx, dy);
                }
            }
            monitor->illuminateTransRect(&rect, thing->titleBarHeight());
            break; // MotionNotify
        }
    }
}

void Kernel::beginRecording() {
    isRecording_ = true;
    assert(recordedActions_.size() == 0);
}

void Kernel::endChainRecording(string name) {
    isRecording_ = false;
    if (recordedActions_.size() == 0) {
        return;
    }

    string chain = "";
    Action *firstAction = 0;
    for (LAction::iterator it = recordedActions_.begin();
         it != recordedActions_.end(); )
    {
        Action *action = *it;

        it++;
        if (action->argument()) {
            chain += action->id() + "+" + action->argument();
        }
        else {
            chain += action->id();
        }
        if (it != recordedActions_.end()) {
            chain += ",";
        }

        if (it == recordedActions_.begin()) {
            firstAction = action;
        }
        else {
            delete action;
        }
    }
    // clean up
    recordedActions_.clear();

    IsValid isValid = 0;
    if (firstAction) {
        isValid = firstAction->getIsValid();
    }
    else {
        isValid = &Validators::isAlwaysPossible;
    }
    delete firstAction;

    (*actionSettings_)["chain." + name + ".seq"] = chain;
    Action *action = new Action(name, &Actions::sequence, isValid,
                                Action::SEQUENCE, (char *)chain.c_str());
    (*actionBindings_)[name] = action;
}

void Kernel::cancelRecording() {
    isRecording_ = false;
    if (recordedActions_.size() == 0) {
        return;
    }
    // clean up
    recordedActions_.clear();
}


void Kernel::endScriptRecording(string name) {
    isRecording_ = false;
    if (recordedActions_.size() == 0) {
        return;
    }

    string script = "#!/bin/sh\n";
    for (LAction::iterator it = recordedActions_.begin();
         it != recordedActions_.end(); )
    {
        Action *action = *it;

        it++;
        script += "wmiremote -a \"";
        if (action->argument()) {
            script += action->id() + "+" + action->argument();
        }
        else {
            script += action->id();
        }
        script += "\"\n";

        delete action;
    }
    // clean up
    recordedActions_.clear();


    Loader::saveFile(name, script);
}


MSettings *Kernel::themeSettings() {
    return themeSettings_; 
}

MSettings *Kernel::commonSettings() {
    return commonSettings_; 
}

MSettings *Kernel::actionSettings() {
    return actionSettings_; 
}

MSettings *Kernel::sessionSettings() {
    return sessionSettings_; 
}

Display *Kernel::display() const {
    return display_;
}

Monitor *Kernel::focusedMonitor() const {
    return monitors_->focused();
}

Kernel::Runlevel Kernel::runlevel() const {
    return runlevel_;
}

void Kernel::restart() {
    runlevel_ = RESTART;
}

void Kernel::stop() {
    runlevel_ = STOP;
}

MBindings *Kernel::actionBindings() const {
    return actionBindings_;
}


bool Kernel::isRecording() const {
    return isRecording_;
}

LAction *Kernel::recordedActions() {
    return &recordedActions_;
}

unsigned int Kernel::borderWidth() const {
    return borderWidth_;
}

void Kernel::setMenuMode(bool isMenuMode) {
    isMenuMode_ = isMenuMode;
}

void Kernel::toggleSloppyMode() {
    isSloppyMode_ = !isSloppyMode_;
}

bool Kernel::isStackedTabbing() const {
    return isStackedTabbing_;
}

CMonitor *Kernel::monitors() const {
    return monitors_;
}

void Kernel::selectMonitor(string displayString) {

    for (LMonitor::iterator it = monitors_->begin();
            it != monitors_->end(); it++)
    {
        Monitor *monitor = *it;
        if (monitor->displayString() == displayString) {
            if (monitors_->focused() != monitor) {
                XCORE->warpPointer(monitor->rootWindow(),
                                   monitor->width() / 2,
                                   monitor->height() / 2);
                monitors_->focus(monitor);
                Workspace *ws = monitor->focused();
                ws->focus(ws->topClient());
            }
            return;
        }
    }
}

void Kernel::grabMove() {

    Client *client = focusedClient();
    if (!client || client->frame()) {
        return;
    }
    XCORE->warpPointer(client->window(),
                       client->width() / 2,
                       client->height() / 2);
    XButtonEvent event;
    event.x = client->width() / 2;
    event.y = client->height() / 2;
    event.time = CurrentTime;
    runResizeMode(client, &event, LEFT, false);
}
