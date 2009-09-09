// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "inputbar.h"

extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include "action.h"
#include "actions.h"
#include "binder.h"
#include "kernel.h"
#include "label.h"
#include "logger.h"
#include "monitor.h"
#include "prompt.h"
#include "theme.h"
#include "workspace.h"
#include "xcore.h"

InputBar::InputBar(Monitor *monitor, Rectangle *rect)
    : Bar(monitor, rect)
{

    LOGDEBUG("initializing input");
    currAction_ = 0;
    prompt_ = 0;
    promptCounter_ = 0;
    isArgument_ = false;
    partitionBegin_ = selected_ = entryBegin_;
    args_ = "";
    LOGDEBUG("creating input");
}

InputBar::~InputBar() {
}

void InputBar::selectPrevEntry() {

    if (entryBegin_ == entryEnd_) {
        // nothing to select, empty list
        return;
    }

    if (selected_ != partitionBegin_) {
        selected_--;
    }
    else {

        if (partitionBegin_ != entryBegin_) {

            selected_ = --partitionBegin_;
            partitionBegin_ = prevPartitions_.top();
            prevPartitions_.pop();
        }
        else {
            clearPrevPartitionsStack();
        }
    }
}

void InputBar::selectNextEntry() {

    if (entryBegin_ == entryEnd_) {
        // nothing to select, empty list
        return;
    }

    Sstring::iterator tmp = selected_;
    selected_++;

    if (selected_ == entryEnd_) {
        selected_ = partitionBegin_ = entryBegin_;
        clearPrevPartitionsStack();
        return;
    }

    if (tmp == partitionEnd_) {
        prevPartitions_.push(partitionBegin_);
        partitionBegin_ = selected_;
    }
}

void InputBar::illuminate() {

    if (!isVisible()) {
        return;
    }

    drawBorder();
    LOGDEBUG("input update");

    // draw background
    label_->setAlignment(Label::CENTER);
    label_->setText("");
    label_->setX(1);
    label_->setWidth(width() - 2);
    label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                   theme_->BAR_SHINE, theme_->BAR_SHADOW);

    // draw prompt
    label_->setText(prompt_ ? prompt_->prompt() : "");
    label_->adjustWidth();
    label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                   theme_->BAR_SHINE, theme_->BAR_SHADOW);
    label_->setX(label_->x() + label_->width());

    // draw text
    label_->setText(text_);
    label_->adjustWidth();
    label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                   theme_->BAR_SHINE, theme_->BAR_SHADOW);
    label_->setX(label_->x() + label_->width());

    if (partitionBegin_ != entryBegin_) {
        label_->setText("<<");
        label_->adjustWidth();
        label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                       theme_->BAR_SHINE, theme_->BAR_SHADOW);
        label_->setX(label_->x() + label_->width());
    }

    label_->setText(">>");
    unsigned int nextWidth = label_->adjustWidth();

    for (Sstring::iterator it = partitionBegin_; it != entryEnd_; it++)
    {
        string currStr = *it;
        label_->setText(currStr);
        label_->adjustWidth();

        // draw entry
        if (label_->x() + label_->width() < (width() - nextWidth)) {

            if (selected_ != it) {
                label_->update(theme_->LABEL_BACKGROUND_NORMAL,
                               theme_->LABEL_TEXT_NORMAL,
                               theme_->LABEL_SHINE_NORMAL,
                               theme_->LABEL_SHADOW_NORMAL);
            }
            else {
                label_->update(theme_->LABEL_BACKGROUND_FOCUSSED,
                               theme_->LABEL_TEXT_FOCUSSED,
                               theme_->LABEL_SHINE_FOCUSSED,
                               theme_->LABEL_SHADOW_FOCUSSED,
                               true, true);
            }
        }
        else {
            partitionEnd_ = --it;
            label_->setText(">>");
            label_->adjustWidth();
            label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                           theme_->BAR_SHINE, theme_->BAR_SHADOW);
            break;
        }
        label_->setX(label_->x() + label_->width());
    }
}

string InputBar::text(bool onExecEnter) {

    if ((selected_ == entryBegin_) && (entryBegin_ == entryEnd_)) {
        return text_;
    }
    else {
        // If an input begins with '/' character we use also the prefix path
        if (onExecEnter && (text_.length() > 0) && (text_[0] == '/')) {
            text_ = text_.substr(0, text_.find_last_of('/') + 1);
            return text_ + *selected_;
        }
        return *selected_;
    }
}

void InputBar::clearPrevPartitionsStack() {
    while (!prevPartitions_.empty()) {
        prevPartitions_.pop();
    }
}

void InputBar::removeChar() {

    if (!isVisible() || !prompt_) {
        return;
    }

    int textLength = text_.length();
    if (textLength < 1) {
        return;
    }

    if (prompt_->toQuery() && names_.size()>0) {
        unsigned int numNames = names_.size();
        for (; (numNames == names_.size()) && (textLength >= 0);
                textLength--)
        {
#ifdef POSIX_REGEX
		string s = Binder::instance()->popRegexPattern();
		LOGDEBUG("popping from STACK: " + s);
		queryText(s);
		textLength=s.length();
#else
            queryText(text_.substr(0, textLength));
#endif
        }
    }
    else {
        queryText(text_.substr(0, textLength - 1));
    }
    illuminate();
}

void InputBar::queryText(string text) {

    if (!isVisible() || !prompt_) {
        return;
    }

    QueryNamesForPrefix toQuery = prompt_->toQuery();
    names_.clear();

    if (toQuery) {
        text_ = ((*Binder::instance()).*toQuery)(text, &names_);
        entryBegin_ = names_.begin();
        entryEnd_ = names_.end();;
        selected_ = partitionBegin_ = entryBegin_;
        clearPrevPartitionsStack();
    }
    else {
        partitionBegin_ = selected_ = entryBegin_;
        text_ = text;
    }
}

void InputBar::handleInput(XKeyEvent *event) {

    static const char *home = getenv("HOME");
    static bool isNext = false;
    char buffer[32];
    int count;

    if (!isVisible()) {
        return;
    }
    static KeySym sym;
    // patch contributed by Marcin Pawlik
    sym = XCORE->lookupNextKeySym(event, &count, buffer);
    if (IsFunctionKey(sym) || IsKeypadKey(sym) ||
            IsMiscFunctionKey(sym) || IsPFKey(sym) ||
            IsPrivateKeypadKey(sym))
    {
        return; // ignore
    }

    bool reset = false;
    if (event->state & ControlMask) {
        switch (sym) {
        case XK_H:
        case XK_h:
            sym = XK_BackSpace;
            break;
        case XK_J:
        case XK_j:
            sym = XK_Return;
            break;
        case XK_G:
        case XK_g:
            sym = XK_Escape;
            break;
        case XK_U:
        case XK_u:
            reset = true;
            break;
        }
    }

    switch (sym) {
    case XK_BackSpace:
        removeChar();
        break;
    case XK_Right:
        selectNextEntry();
        break;
    case XK_Left:
        selectPrevEntry();
        break;
    case XK_Return:
        handlePressedReturn();
        break;
    case XK_Escape:
        escape();
        break;
    case XK_Num_Lock:
        break;
    default:
        if (reset) {
            text_ = "";
            queryText(text_);
        }
        else if ((count == 1) && !iscntrl(buffer[0])) {
            buffer[count] = 0;
            string text = "";
            if (home && (buffer[0] == '~')) {
                text = text_ + home;
            }
            else {
                text = text_ + buffer;
            }
            queryText(text);
            text_ = text;
        }
        else if (sym == XK_Tab) {
            string text = text_;
            if (!isNext) {
                queryText(text_);
            }
            isNext = (text == text_);
            if (isNext) {
                selectNextEntry();
            }
        }
#ifdef POSIX_REGEX
	if (text_=="") {Binder::instance()->clearRegexPattern();}else{
	if (!names_.empty()) Binder::instance()->pushRegexPattern(text_);}
	
#endif
        break;
    }
    if (sym != XK_Tab) {
        isNext = false;
    }
    illuminate();
}

void InputBar::escape() {

    promptCounter_ = 0;
    args_ = "";
    prompt_ = 0;
    currAction_ = 0;
#ifdef POSIX_REGEX
    Binder::instance()->clearRegexPattern();
#endif
    XCORE->ungrabKeyboard();
    hide();
}

void InputBar::handlePressedReturn() {

    string actionKey;
    bool isDirectInvocation = false;

    LOGDEBUG("fetching selection");
    actionKey = text();
    unsigned int pos = actionKey.find_first_of('+');
    if (!isArgument_ && (pos != string::npos)) {
        args_ = actionKey.substr(pos + 1);
        actionKey = actionKey.substr(0, pos);
        isDirectInvocation = true;
    }
    LOGDEBUG("selection fetched");
    if (isArgument_ || (actionKey != "") || isDirectInvocation) {

        if (!isArgument_ || isDirectInvocation)
        { // fetch action
            LOGDEBUG("fetching action: " + actionKey );
            currAction_ = Util::get(KERNEL->actionBindings(), actionKey);
        }

        if (currAction_) {

            if (!isArgument_ &&
                (currAction_->promptsCount() > 0) && !isDirectInvocation)
            {
                // set prompt of first argument
                prompt_ = currAction_->prompt(0);
                isArgument_ = true;
                queryText("");
            }
            else if (isArgument_) {

                // fetch args
                if (args_.length() > 0) {
                    args_ += '+';
                }
                args_ += text((currAction_->getToPerform() == &Actions::execute) ||
                              (currAction_->getToPerform() == &Actions::executeTerm));

                promptCounter_++;
                if (promptCounter_ < currAction_->promptsCount()) {
                    prompt_ = currAction_->prompt(promptCounter_);
                }
                queryText(""); // keyboard is already grabbed
            }
            illuminate();

            if ((promptCounter_ == currAction_->promptsCount()) ||
                    isDirectInvocation)
            {
                LOGDEBUG("going to perform action");
                if (isArgument_ || isDirectInvocation) {
                    LOGDEBUG("got args: " + args_);
                    currAction_->setArgument((char *)args_.c_str());
                }
                // otherwise default invocation
                currAction_->perform();
                escape();
            }
            else {
                return;
            }
        }
    }
}

void InputBar::runKey(Prompt *prompt) {
    if (!isVisible()) {
        show();
    }
    XCORE->grabKeyboard(window());
    currAction_ = 0;
    prompt_ = prompt;
    isArgument_ = false;
    queryText("");
    illuminate();
}

void InputBar::runArgument(Action *action) {
    if (!action->isValid()) {
        return;
    }
    if (!isVisible()) {
        show();
    }
    XCORE->grabKeyboard(window());
    currAction_ = action;
    // set prompt of first argument
    prompt_ = currAction_->prompt(0);
    isArgument_ = true;
    queryText("");
    illuminate();
}

void InputBar::handleMotionNotify(XMotionEvent *event) {
    // nothing todo
}

void InputBar::handleButtonPress(XButtonEvent *event) {
    escape();
}

void InputBar::handleButtonRelease(XButtonEvent *event) {
    // nothing todo
}

bool InputBar::isArgument() const {
    return isArgument_;
}
