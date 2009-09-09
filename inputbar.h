// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __INPUTBAR_H
#define __INPUTBAR_H

#include <set>
#include <stack>
#include <string>
#include "wmi.h"

#include "bar.h"

class Action;
class Monitor;
class Prompt;
class Theme;

typedef set<string> Sstring;
typedef stack<Sstring::iterator> SSstringIter;

/**
 * Represents the input bar and input mode of WMI.
 */
class InputBar: public Bar
{

public:

    InputBar(Monitor *monitor, Rectangle *rect);

    ~InputBar();

    virtual void illuminate();

    virtual void handleMotionNotify(XMotionEvent *event);
    virtual void handleButtonPress(XButtonEvent *event);
    virtual void handleButtonRelease(XButtonEvent *event);

    /** Handler for input key events. */
    void handleInput(XKeyEvent *event);
    void handlePressedReturn();

    void runKey(Prompt *prompt);
    void runArgument(Action *action);

private:

    bool isArgument() const;

    string text(bool onExecEnter = false);

    void queryText(string text);
    void removeChar();
    void clearPrevPartitionsStack();

    void selectNextEntry();
    void selectPrevEntry();

    void escape();

    /** Provides argument processing. */
    Action *currAction_;

    Sstring names_;

    bool isArgument_;
    Sstring::iterator entryBegin_;
    Sstring::iterator entryEnd_;
    Sstring::iterator partitionBegin_;
    Sstring::iterator partitionEnd_;
    Sstring::iterator selected_;
    SSstringIter prevPartitions_;
    Prompt *prompt_;
    string text_;
    string args_;
    unsigned int promptCounter_;
};

#endif // __INPUTBAR_H
