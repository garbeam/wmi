// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// Copyright (c) 2003 - 2004 Marcel Manthe <schneegloeckchen at gmx.li>
// See ../LICENSE.txt for license details.
//
// $Id: binder.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __BINDER_H
#define __BINDER_H

#include "singleton.h"

extern "C" {
#include <X11/Xlib.h>
}

#include <list>
#include <map>
#include <set>

#ifdef POSIX_REGEX
#include <stack>
#include "logger.h"
#include <regex.h>
#endif

#include <string>
#include "wmi.h"

// forward declaration
class Action;
class Binder;
class Group;
class Kernel;
class Shortcut;

typedef set<string> Sstring;
typedef map<string, Action *, less<string> > MBindings;
typedef list<Action *> LAction;

/**
 * Main class of all key- and button-grabbing related logic.
 */
class Binder : public Singleton<Binder>
{

public:

    /** Constructs a new grab manager instance.  */
    Binder();

    ~Binder();

    /** Key initialization for a specific window. */
    void initKeys(Window window);

    /** Key event handler, window is for grabbing. */
    void handleKey(Window window, XKeyEvent *e);

    /** Button event handler, window is for grabbing. */
    void handleButton(Window window, XButtonEvent *e);

    /** Grabs the shortcut. */
    void grabShortcut(Shortcut *shortcut, Window window);

    /** Ungrabs shortcut. */
    void ungrabShortcut(Shortcut *shortcut, Window window);

    /** Grabs all buttons for the modMask of the specific window. */
    void grabButtons(Window window, unsigned long modMask);

    /** Ungrabs all buttons for the specific window. */
    void ungrabButtons(Window window);

    /** Returns pretty print of current key configuration. */
    string prettyPrintKeyBindings();

    /**
     * Queries all actions for the given pattern.
     * Returns absolute equal pattern of <i>all</i> entries.
     * @param pattern contains the pattern.
     * @param set where to put matching action keys.
     */
    string queryActionKeysForPattern(string pattern, Sstring *actionKeys);

    /** Similiar to previous, except actions aren't validated. */
    string queryActionKeysWithoutValidationForPattern(string pattern,
                                                      Sstring *actionKeys);


    /** Similiar to previous, except that internal actions are ignored. */
    string queryExternChainActionsForPattern(
                            string pattern, Sstring *actionKeys);

    /**
     * Queries all executables for the given pattern.
     * Returns absolute equal pattern of <i>all</i> executables.
     * @param pattern contains the pattern.
     * @param set where to put matching executables.
     */
    string queryCommandForPattern(string pattern, Sstring *expands);

    /**
     * Queries all workspace names of the focused monitor.
     * Returns absolute equal pattern of <i>all</i> workspaces.
     * @param pattern contains the pattern.
     * @param set where to put matching names.
     */
    string queryWorkspacesForPattern(string pattern, Sstring *workspaces);

    /**
     * Queries all monitor names.
     * Returns absolute equal pattern of <i>all</i> monitors.
     * @param pattern contains the pattern.
     * @param set where to put matching names.
     */
    string queryMonitorsForPattern(string pattern, Sstring *monitors);

    /**
     * Queries all client names of the focused monitor.
     * Returns absolute equal pattern of <i>all</i> clients.
     * @param pattern contains the pattern.
     * @param set where to put matching names.
     */
    string queryClientsForPattern(string pattern, Sstring *clients);

    /**
     * Queries all detached client names of the focused monitor.
     * Returns absolute equal pattern of <i>all</i> clients.
     * @param pattern contains the pattern.
     * @param set where to put matching names.
     */
    string queryDetachedClientsForPattern(string pattern, Sstring *clients);

    /** Same as above, except instead of client names, client ids. */
    string queryClientIdsForPattern(string pattern, Sstring *clients);

    /**
     * Queries all frame names of the focused workspace.
     * Returns absolute equal pattern of <i>all</i> frame ids.
     * @param pattern contains the pattern.
     * @param set where to put matching names.
     */
    string queryFramesForPattern(string pattern, Sstring *sFrames);

    unsigned int validModMask() const;

#ifdef POSIX_REGEX
    string popRegexPattern(){
        string result;
        if (!regexPattern_.empty()) regexPattern_.pop();
        if (regexPattern_.empty()){
            result = "";
        }else{
            result = regexPattern_.top();
        }

        return result;
    }
        
    void pushRegexPattern(string pattern){
       regexPattern_.push(pattern);

       stack<string> temp = regexPattern_;
       string res;
       while (!temp.empty()){
           res= res + temp.top() + " ---  " ;
           temp.pop();
       }
       LOGDEBUG("STACK: " + res);      
   
    }
    void clearRegexPattern(){
        while (!regexPattern_.empty()) regexPattern_.pop();
    } 
#endif

private:

    /** Helper handler for handleKey and handleButton action. */
    void handleShortcut(Window window, unsigned long modMask,
                        KeyCode keyCode, unsigned int button);

    /** Own key press processing shortcut handler (for postfixed
      shortcuts). */
    void handleShortcutChains(Window window, Shortcut *prefix,
                              LAction *grabbedActions);
    /** Helper for handleShortcutChains. Keyboard should be grabbed. */
    void nextKeystroke(unsigned long *modMask, KeyCode *keyCode);

    /** Emulates key press for client. */
    void emulateKeyPress(unsigned long modMask, KeyCode keyCode);

    /** Inits lock modifier detection (NumLock and ScrollLock). */
    void initLockModifiers();

    /**
     * Returns absolute pattern of the given set, at least pattern.
     * @param offset begin of comparisions within set.
     */
    string absolutePattern(string pattern, Sstring *names, unsigned int offset);

    /**
     * Matches the pattern.
     * @param digest the string to check match.
     * @param pattern the pattern to match.
     * @param strings the container set to insert matched strings.
     * @param patternLength the pattern length.
     */
    void matchPattern(string digest, string pattern, Sstring *strings,
                      unsigned int patternLength);

    void initRegex(string);

#ifdef POSIX_REGEX
    bool regex_first_match(const string& pattern, const string& haystack,
                           int& begin, int& end);
    bool regex_match(const string& pattern, const string& haystack);
    void regex_prepare(){regcomp(&temp_regex_, "", REG_EXTENDED); temp_pattern_="";}
    string temp_pattern_;
    regex_t temp_regex_;
    bool doRegex_;
    stack<string> regexPattern_;
#endif


    MBindings *actionBindings_;

    unsigned int numLockMask_;
    unsigned int scrollLockMask_;
    unsigned int validModMask_;
};

#endif // __BINDER_H
