// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// Copyright (c) 2003 - 2004 Marcel Manthe <schneegloeckchen at gmx.li>
// See ../LICENSE.txt for license details.
//
// $Id: binder.cpp 734 2004-09-27 18:15:45Z garbeam $

extern "C" {
#include <assert.h>
}

#include <sstream>
#include <string>
#include "wmi.h"

#include "binder.h"

#include "action.h"
#include "box.h"
#include "client.h"
#include "container.h"
#include "cursors.h"
#include "expander.h"
#include "frame.h"
#include "inputbar.h"
#include "kernel.h"
#include "logger.h"
#include "monitor.h"
#include "shortcut.h"
#include "thing.h"
#include "util.h"
#include "workspace.h"
#include "xcore.h"


Binder::Binder() {
    actionBindings_ = KERNEL->actionBindings();
    initLockModifiers();
#ifdef POSIX_REGEX
    temp_pattern_="uninitalized";
    regex_prepare();
#endif
}

Binder::~Binder() {
}

void Binder::grabShortcut(Shortcut *shortcut, Window window) {

    // update keysym definitions of the action
    if (shortcut->button()) {
        XCORE->grabButton(window, shortcut->modMask(),
                          shortcut->button());

        if (shortcut->modMask() == AnyModifier) {
            return;
        }
        if (numLockMask_) {
            XCORE->grabButton(window, shortcut->modMask() | numLockMask_,
                              shortcut->button());
            XCORE->grabButton(window, shortcut->modMask() | numLockMask_
                              | LockMask, shortcut->button());
        }
        if (scrollLockMask_) {
            XCORE->grabButton(window, shortcut->modMask() | scrollLockMask_,
                              shortcut->button());
            XCORE->grabButton(window, shortcut->modMask() | scrollLockMask_
                              | LockMask, shortcut->button());
        }
        if (numLockMask_ && scrollLockMask_) {
            XCORE->grabButton(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_, shortcut->button());
            XCORE->grabButton(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_ | LockMask, shortcut->button());
        }
    }
    else if (shortcut->keyCode()) {
        XCORE->grabKey(window, shortcut->modMask(),
                       shortcut->keyCode());

        if (shortcut->modMask() == AnyModifier) {
            return;
        }
        if (numLockMask_) {
            XCORE->grabKey(window, shortcut->modMask() | numLockMask_,
                              shortcut->keyCode());
            XCORE->grabKey(window, shortcut->modMask() | numLockMask_
                              | LockMask, shortcut->keyCode());
        }
        if (scrollLockMask_) {
            XCORE->grabKey(window, shortcut->modMask() | scrollLockMask_,
                              shortcut->keyCode());
            XCORE->grabKey(window, shortcut->modMask() | scrollLockMask_
                              | LockMask, shortcut->keyCode());
        }
        if (numLockMask_ && scrollLockMask_) {
            XCORE->grabKey(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_, shortcut->keyCode());
            XCORE->grabKey(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_ | LockMask, shortcut->keyCode());
        }
    }
}

void Binder::ungrabShortcut(Shortcut *shortcut, Window window) {

    if (shortcut->button()) {
        XCORE->ungrabButton(window, shortcut->modMask(), shortcut->button());

        if (shortcut->modMask() == AnyModifier) {
            return;
        }
        if (numLockMask_) {
            XCORE->ungrabButton(window, shortcut->modMask() | numLockMask_,
                              shortcut->button());
            XCORE->ungrabButton(window, shortcut->modMask() | numLockMask_
                              | LockMask, shortcut->button());
        }
        if (scrollLockMask_) {
            XCORE->ungrabButton(window, shortcut->modMask() | scrollLockMask_,
                              shortcut->button());
            XCORE->ungrabButton(window, shortcut->modMask() | scrollLockMask_
                              | LockMask, shortcut->button());
        }
        if (numLockMask_ && scrollLockMask_) {
            XCORE->ungrabButton(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_, shortcut->button());
            XCORE->ungrabButton(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_ | LockMask, shortcut->button());
        }
    }
    else {
        XCORE->ungrabKey(window, shortcut->modMask(),
                    shortcut->keyCode());

        if (shortcut->modMask() == AnyModifier) {
            return;
        }
        if (numLockMask_) {
            XCORE->ungrabKey(window, shortcut->modMask() | numLockMask_,
                              shortcut->keyCode());
            XCORE->ungrabKey(window, shortcut->modMask() | numLockMask_
                              | LockMask, shortcut->keyCode());
        }
        if (scrollLockMask_) {
            XCORE->ungrabKey(window, shortcut->modMask() | scrollLockMask_,
                              shortcut->keyCode());
            XCORE->ungrabKey(window, shortcut->modMask() | scrollLockMask_
                              | LockMask, shortcut->keyCode());
        }
        if (numLockMask_ && scrollLockMask_) {
            XCORE->ungrabKey(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_, shortcut->keyCode());
            XCORE->ungrabKey(window, shortcut->modMask() | numLockMask_
                    | scrollLockMask_ | LockMask, shortcut->keyCode());
        }
    }
}

void Binder::initKeys(Window window) {

    for (MBindings::iterator it = actionBindings_->begin();
         it != actionBindings_->end(); it++)
    {
        Action *action = (*it).second;
        LOGDEBUG("initKeys: " + action->id());
        if (action->listenOn()) {
            grabShortcut(action->listenOn(), window);
        }
    }
}

string Binder::prettyPrintKeyBindings() {

    ostringstream oss;
    for (MBindings::iterator it = actionBindings_->begin();
         it != actionBindings_->end(); it++)
    {
        Action *action = (*it).second;
        Shortcut *shortcut = action->shortcut();
        Shortcut *tmp = action->shortcut();
        if (shortcut) {
            oss.width(35);
            oss.fill(' ');
            oss << action->id() << " = ";
        }
        while (tmp) {
            oss.width(0);
            if (tmp->button()) {
                oss << Util::stringForModMask(tmp->modMask())
                    << Util::stringForButton(tmp->button());
            }
            else {
                oss << Util::stringForModMask(tmp->modMask())
                    << XCORE->keyCodeToString(tmp->keyCode());
            }
            tmp = tmp->next();
            if (tmp) {
                oss << " :: ";
            }
        }
        if (shortcut) {
            oss << "\n";
        }
    }

    string result = oss.str();
    if (result == "") {
        result = "no key bindings defined\n";
    }
    return result;
}

void Binder::handleButton(Window window, XButtonEvent *event)
{
    LOGDEBUG("handle button press event");
    handleShortcut(window, event->state, 0, event->button);
}

void Binder::handleShortcut(Window window, unsigned long modMask,
                            KeyCode keyCode, unsigned int button)
{
    LOGDEBUG("handle shortcut event");
    LAction grabbedActions;
    // only gets in here for the first part of a shortcut, thus
    // create a list of all shortcuts which are listened for
    for (MBindings::iterator it = actionBindings_->begin();
            it != actionBindings_->end(); it++)
    {
        Action *action = (*it).second;
        Shortcut *shortcut = action->listenOn();

        if (shortcut &&
            (modMask == shortcut->modMask()) &&
            (keyCode == shortcut->keyCode()) &&
            (button == shortcut->button()))
        {
            if (shortcut->next()) {
                action->setListenOn(shortcut->next());
                grabbedActions.push_back(action);
            }
            else {
                grabbedActions.clear();
                LOGDEBUG("handle shortcut performs action '"
                         + action->id() + "'");
                if (action->promptsCount() > 0) {
                    KERNEL->focusedMonitor()->inputBar()->runArgument(action);
                    // action will be performed by argument processing
                }
                else {
                    action->perform();
                }
                break;
            }
        }
    }
    if (grabbedActions.size()) {
        Shortcut shortcut(modMask, keyCode, 0, button);
        // now the magic occurs
        handleShortcutChains(window, &shortcut, &grabbedActions);
    }
    // reset listen on info
    for (MBindings::iterator it = actionBindings_->begin();
            it != actionBindings_->end(); it++)
    {
        Action *action = (*it).second;
        if (action->shortcut()) {
            // already grabbed
            action->setListenOn(action->shortcut());
        }
    }
}

void Binder::handleShortcutChains(Window window, Shortcut *prefix,
                                  LAction *grabbedActions)
{
    string sequence = 
        " " + Util::stringForModMask(prefix->modMask())
            + XCORE->keyCodeToString(prefix->keyCode()) + " ::";
    Box *box = KERNEL->focusedMonitor()->box();
    box->setText(sequence);
    box->show();
    box->illuminate();

    XCORE->grabKeyboard(window);

    bool isDoubleShortcut = false;
    unsigned long modMask;
    KeyCode keyCode;
    Action *action = 0;
    while (grabbedActions->size()) {
        // fetch new key stroke
        nextKeystroke(&modMask, &keyCode);

        if ((prefix->modMask() == modMask) &&
            (prefix->keyCode() == keyCode))
        {
            isDoubleShortcut = true;
            break;
        }
        prefix->setModMask(modMask);
        prefix->setKeyCode(keyCode);
        sequence += " " + Util::stringForModMask(modMask)
                        + XCORE->keyCodeToString(keyCode) + " ::";
        box->setText(sequence);
        box->illuminate();
        for (LAction::iterator it = grabbedActions->begin();
                it != grabbedActions->end(); )
        {
            action = *it;
            Shortcut *shortcut = action->listenOn();
            if (shortcut &&
                (modMask == shortcut->modMask()) &&
                (keyCode == shortcut->keyCode()))
            {
                // iterates next
                it++;
                if (shortcut->next()) {
                    action->setListenOn(shortcut->next());
                }
                else {
                    grabbedActions->clear(); // escape condition
                    break;
                }
            }
            else {
                // iterates next
                it = grabbedActions->erase(it);
            }
            action = 0;
        }
    }
    XCORE->ungrabKeyboard();
    box->hide();
    if (action) {

        LOGDEBUG("handle shortcut performs action '"
                + action->id() + "'");
        if (action->promptsCount() > 0) {
            KERNEL->focusedMonitor()->inputBar()->runArgument(action);
            // action will be performed by argument processing
        }
        else {
            action->perform();
        }
    }
    else if (isDoubleShortcut) {
        emulateKeyPress(modMask, keyCode);
    }
}

void Binder::emulateKeyPress(unsigned long modMask, KeyCode keyCode) {

    Monitor *monitor = KERNEL->focusedMonitor();
    Client *client = monitor->focused()->topClient();

    if (client) {
        XEvent event;
        event.xkey.type = KeyPress;
        event.xkey.time = CurrentTime;
        event.xkey.window = client->clientWindow();
        event.xkey.display = KERNEL->display();
        event.xkey.state = modMask;
        event.xkey.keycode = keyCode;
        XCORE->sendEvent(client->clientWindow(), KeyPressMask, &event);
        event.xkey.type = KeyRelease;
        XCORE->sendEvent(client->clientWindow(), KeyReleaseMask, &event);
        XCORE->sync();
    }
}

void Binder::nextKeystroke(unsigned long *modMask, KeyCode *keyCode) {
    XEvent event;
    KeySym sym;
    *modMask = 0;
    do {
        XCORE->maskEvent(KeyPressMask, &event);
        *modMask |= event.xkey.state & validModMask_;
        *keyCode = event.xkey.keycode;
        sym = XCORE->keyCodeToKeySym(event.xkey.keycode);
    } while (IsModifierKey(sym));
}

void Binder::handleKey(Window window, XKeyEvent *event)
{
    LOGDEBUG("handle key press event");
    handleShortcut(window, event->state, event->keycode, 0);
}

void Binder::grabButtons(Window window, unsigned long modMask)
{
    XCORE->grabButton(window, modMask, AnyModifier);
}

void Binder::ungrabButtons(Window window) {
    XCORE->ungrabButton(window, AnyModifier, AnyButton);
}

#ifdef POSIX_REGEX
bool Binder::regex_first_match(const string& pattern, const string& haystack, int& begin, int& end){
    regmatch_t temp_regmatch;

    begin = 0;
    end = -1 ;
    bool condition = false;
    if (pattern == temp_pattern_ && pattern!="") { condition = true; } else
    {
        condition = (regcomp(&temp_regex_, pattern.c_str(), REG_EXTENDED)==0);
        if (!condition) regex_prepare(); //reinitialize
    }
    if (condition){
        bool result = (regexec(&temp_regex_, haystack.c_str(), 1, &temp_regmatch, 0)==0);
        if (result){
            begin = temp_regmatch.rm_so;
            end = temp_regmatch.rm_eo;
        }
        temp_pattern_ = pattern;
        return result;
    }
    return false;
}

bool Binder::regex_match(const string& pattern, const string& haystack){
    int begin;
    int end;
    return(regex_first_match(pattern, haystack, begin, end));
}
#endif


string Binder::absolutePattern(string pattern, Sstring *names,
                                   unsigned int offset)
{

    string result = pattern;
if (!(names->empty())) {

#ifdef POSIX_REGEX
        if (doRegex_) {

            /* Check whether the search-pattern contains a modifier.
               If it does, simply return the pattern itself, otherwise,
               perform an infix-matching-algorithm. */

            string first = *names->begin();
            for (int direction = 0; direction <= 1; direction++){
                bool matches_all = true;
                do {
                    LOGDEBUG("Entering do-while-infix-matching-loop");
                    int begin;
                    int end;

                    if (regex_first_match (result, first, begin, end)){
                        LOGDEBUG("pattern-rule matched first member");
                        int count = end - begin + 1;
                        if (direction == 1) begin--;
                        if ((begin + count  <= (int)first.length()) && (begin >= 0) && (count>0)){
                            pattern=first.substr(begin, count);
                            LOGDEBUG("trying to add a char to pattern");
                            LOGDEBUG(pattern);
                        } else break;
                    }

                    for (Sstring::iterator it = names->begin(); it != names->end(); it++) 
                        if(!regex_match(pattern, (*it))) matches_all = false;
                    if (matches_all) result = pattern;
                } while (matches_all);
            }
            return(result);
        }
#endif

        string firstName = *names->begin();
        string lastName = *names->rbegin();

        unsigned int firstLength = firstName.length();
        unsigned int lastLength = lastName.length();
        for (unsigned int i = offset;
             (i < firstLength) && (i < lastLength); i++)
        {
            if (firstName[i] == lastName[i]) {
                result += firstName[i];
            }
            else {
                break;
            }
        }
    }

    return result; // return absolute pattern
}

void Binder::matchPattern(string digest, string pattern, Sstring *strings,
                          unsigned int patternLength)
{
    bool condition;
    condition = (digest.substr(0, patternLength) == pattern);

#ifdef POSIX_REGEX
    if (doRegex_) {
        condition = regex_match(pattern, digest);
    }
#endif

    if (condition) {
        strings->insert(digest);
    }
}

void Binder::initRegex(string pattern){
#ifdef POSIX_REGEX
    doRegex_ = Util::get(KERNEL->commonSettings(), "autocompletion.mode") == "regex";
#endif
}

string Binder::queryFramesForPattern(string pattern, Sstring *sFrames)
{

    Workspace *workspace = KERNEL->focusedMonitor()->focused();
    CFrame *frames = workspace->frames();
    unsigned int patternLength = pattern.length();
    string lName;
    initRegex(pattern);
    for (LFrame::iterator it = frames->begin(); it != frames->end(); it++) {
        Frame *frame = *it;
        matchPattern(frame->name(), pattern,
                     sFrames, patternLength);
    }
    return absolutePattern(pattern, sFrames, patternLength);
}

string Binder::queryClientIdsForPattern(string pattern, Sstring *clients)
{

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    MClient *clientMap = focusedMonitor->clients();
    unsigned int patternLength = pattern.length();
    initRegex(pattern);
    for (MClient::iterator it = clientMap->begin();
         it != clientMap->end(); it++)
    {
        Client *client = (*it).second;
        if (client->attached()
#ifdef SLOT_SUPPORT
                || client->mode() == Client::SLOT
#endif // SLOT_SUPPORT 
           )
        {
            matchPattern(client->id(), pattern, clients, patternLength);
        }
    }
    return absolutePattern(pattern, clients, pattern.length());
}

string Binder::queryClientsForPattern(string pattern, Sstring *clients)
{

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    MClient *clientMap = focusedMonitor->clients();
    unsigned int patternLength = pattern.length();
    initRegex(pattern);
    for (MClient::iterator it = clientMap->begin();
         it != clientMap->end(); it++)
    {
        Client *client = (*it).second;
        if (client->attached()
#ifdef SLOT_SUPPORT 
                || client->mode() == Client::SLOT
#endif
           )
        {
            matchPattern(client->name(), pattern, clients, patternLength);
        }
    }
    return absolutePattern(pattern, clients, pattern.length());
}

string Binder::queryDetachedClientsForPattern(string pattern, Sstring *clients)
{

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    CClient *detachedClients = focusedMonitor->detachedClients();
    unsigned int patternLength = pattern.length();
    initRegex(pattern);
    for (LClient::iterator it = detachedClients->begin();
         it != detachedClients->end(); it++)
    {
        Client *client = *it;
        matchPattern(client->name(), pattern, clients, patternLength);
    }
    return absolutePattern(pattern, clients, pattern.length());
}

string Binder::queryMonitorsForPattern(string pattern, Sstring *monitors) {

    CMonitor *ms = KERNEL->monitors();
    unsigned int patternLength = pattern.length();
    initRegex(pattern);
    for (LMonitor::iterator it = ms->begin(); it != ms->end(); it++) {
        matchPattern((*it)->displayString(), pattern, monitors, patternLength);
    }

    return absolutePattern(pattern, monitors, patternLength);
}

string Binder::queryWorkspacesForPattern(string pattern,
        Sstring *workspaces)
{

    Monitor *focusedMonitor = KERNEL->focusedMonitor();
    unsigned int patternLength = pattern.length();
    string wsName;
    initRegex(pattern);
    for (LWorkspace::iterator it = focusedMonitor->begin();
         it != focusedMonitor->end(); it++)
    {
        matchPattern((*it)->name(), pattern, workspaces, patternLength);
    }

    return absolutePattern(pattern, workspaces, patternLength);
}

string Binder::queryExternChainActionsForPattern(
        string pattern, Sstring *actionKeys)
{
    unsigned int patternLength = pattern.length();
    string actionKey;
    Action *action;
    initRegex(pattern);
    for (MBindings::iterator it = actionBindings_->begin();
         it != actionBindings_->end(); it++)
    {
        actionKey = (*it).first;
        action = (*it).second;
        if (action->type() == Action::INTERN) {
            continue;
        }
        matchPattern(actionKey, pattern, actionKeys, patternLength);
    }

    return absolutePattern(pattern, actionKeys, patternLength);
}

string Binder::queryActionKeysWithoutValidationForPattern(
        string pattern, Sstring *actionKeys)
{
    unsigned int patternLength = pattern.length();
    string actionKey;
    initRegex(pattern);
    for (MBindings::iterator it = actionBindings_->begin();
         it != actionBindings_->end(); it++)
    {
        actionKey = (*it).first;
        matchPattern(actionKey, pattern, actionKeys, patternLength);
    }

    return absolutePattern(pattern, actionKeys, patternLength);
}

string Binder::queryActionKeysForPattern(string pattern, Sstring *actionKeys) {

    unsigned int patternLength = pattern.length();
    string actionKey;
    initRegex(pattern);
    for (MBindings::iterator it = actionBindings_->begin();
         it != actionBindings_->end(); it++)
    {
        Action *action = (*it).second;

        if (action->isValid()) {
            actionKey = (*it).first;
            matchPattern(actionKey, pattern, actionKeys, patternLength);
        }
    }

    return absolutePattern(pattern, actionKeys, patternLength);
}

string Binder::queryCommandForPattern(string pattern, Sstring *expands) {

    Expander::instance()->queryString(pattern);
    Sstring exp = Expander::instance()->expands();
    initRegex(pattern);


    unsigned int offset  = pattern.find_last_of('/');
    if (offset == string::npos) offset=0; else offset++; 

#ifdef POSIX_REGEX
    doRegex_ = doRegex_ && (offset == 0);
#endif

    for (Sstring::iterator it = exp.begin();
        it != exp.end(); it++)
    {
    matchPattern((string)(*it), pattern.substr(offset), expands, pattern.length()-offset);
    }

    return absolutePattern(pattern, expands, pattern.length() -
            pattern.find_last_of('/') - 1);
}

#define NUM_MASKS 8

void Binder::initLockModifiers() {
    XModifierKeymap *modmap;
    KeyCode numLock, scrollLock;

    // preinit
    numLockMask_ = scrollLockMask_ = 0;

    // init modifier map
    modmap = XCORE->modifierMapping();

    // modifier masks we know
    static int masks[NUM_MASKS] = {
        ShiftMask, LockMask, ControlMask, Mod1Mask,
        Mod2Mask, Mod3Mask, Mod4Mask, Mod5Mask
    };

    // temporary keysyms of evil keys
    numLock = XCORE->stringToKeyCode("Num_Lock");
    scrollLock = XCORE->stringToKeyCode("Scroll_Lock");

    if (modmap && modmap->max_keypermod > 0) {
        unsigned int max = NUM_MASKS * modmap->max_keypermod;
        for (unsigned int i = 0; i < max; i++) {
            if (numLock && (modmap->modifiermap[i] == numLock)) {
                numLockMask_ = masks[i / modmap->max_keypermod];
            }
            else if (scrollLock && (modmap->modifiermap[i] == scrollLock)) {
                scrollLockMask_ = masks[i / modmap->max_keypermod];
            }
        }
    }

    XCORE->freeModifierMapping(modmap);
    validModMask_ = 255 & ~(numLockMask_ | scrollLockMask_ | LockMask);
}

unsigned int Binder::validModMask() const {
    return validModMask_;
}
