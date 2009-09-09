// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: statusbar.cpp 734 2004-09-27 18:15:45Z garbeam $

extern "C" {
#include <assert.h>
}

#include "statusbar.h"

#include "binder.h"
#include "client.h"
#include "cursors.h"
#include "draw.h"
#include "kernel.h"
#include "inputbar.h"
#include "label.h"
#include "logger.h"
#include "rectangle.h"
#include "workspace.h"
#include "monitor.h"
#include "theme.h"
#include "util.h"
#include "xcore.h"

StatusBar::StatusBar(Monitor *monitor, Rectangle *rect)
    : Bar(monitor, rect)
{
    text_ = "window manager improved - " + string(__WMI_VERSION);
    buttonPressed_ = false;
}

void StatusBar::illuminate() {

    if (!isVisible()) {
        return;
    }

    LOGDEBUG("going to draw border");
    drawBorder();

    unsigned int offsetX = 1;

    label_->setAlignment(Label::CENTER);
    label_->setText("");
    label_->setX(offsetX);
    label_->setWidth(width() - 2);
    label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                   theme_->BAR_SHINE, theme_->BAR_SHADOW);

    updateInputModeButton(&offsetX);
    updateWorkspacePager(&offsetX);
    updateStatus(&offsetX);
    updateMeters(offsetX);

    XCORE->sync();
}

StatusBar::~StatusBar() {
}

void StatusBar::updateInputModeButton(unsigned int *offsetX) {

    if (!isButtonVisible_) {
        return;
    }

    // input mode button
    label_->setX(*offsetX);
    label_->setText("");
    label_->setWidth(monitor()->buttonWidth());
    if (!buttonPressed_) {

        Draw::drawInputModeButton(window(), gc(), label_,
                theme_->BUTTON_BACKGROUND_NORMAL,
                theme_->BUTTON_SHINE_BORDER_NORMAL,
                theme_->BUTTON_SHADOW_BORDER_NORMAL,
                theme_->BUTTON_SHINE_FIGURE_NORMAL,
                theme_->BUTTON_SHADOW_FIGURE_NORMAL);
    }
    else {

        Draw::drawInputModeButton(window(), gc(), label_,
                theme_->BUTTON_BACKGROUND_PRESSED,
                theme_->BUTTON_SHINE_BORDER_PRESSED,
                theme_->BUTTON_SHADOW_BORDER_PRESSED,
                theme_->BUTTON_SHINE_FIGURE_PRESSED,
                theme_->BUTTON_SHADOW_FIGURE_PRESSED);
    }
    *offsetX += monitor()->buttonWidth() + 2;
}

unsigned int StatusBar::calculateWorkspaceWidth() {

    // workspace pager stuff
    Workspace *focusedWorkspace = monitor()->focused();
    assert(focusedWorkspace);

    // first determine maximum workspace label width
    unsigned int workspaceWidth = 0;
    for (LWorkspace::iterator it = monitor()->begin();
         it != monitor()->end(); it++)
    {
        Workspace *workspace = *it;
        label_->setText(workspace->name());
        label_->adjustWidth();
        if (label_->width() > workspaceWidth) {
            workspaceWidth = label_->width();
        }
    }

    return workspaceWidth;
}

void StatusBar::updateWorkspacePager(unsigned int *offsetX) {

    // workspace pager stuff
    Workspace *focusedWorkspace = monitor()->focused();
    assert(focusedWorkspace);
    unsigned int workspaceWidth = calculateWorkspaceWidth();
    unsigned int i = 0;
    label_->setWidth(workspaceWidth);
    for (LWorkspace::iterator it = monitor()->begin();
         it != monitor()->end(); it++)
    {
        Workspace *workspace = *it;
        label_->setX(i * workspaceWidth + *offsetX);
        label_->setText(workspace->name());
        if (workspace == focusedWorkspace) {
            label_->update(theme_->LABEL_BACKGROUND_FOCUSSED,
                           theme_->LABEL_TEXT_FOCUSSED,
                           theme_->LABEL_SHINE_FOCUSSED,
                           theme_->LABEL_SHADOW_FOCUSSED,
                           true, true);
        }
        else if (workspace->requestsFocus()) {
            label_->update(theme_->FOCUSREQ_BACKGROUND,
                           theme_->FOCUSREQ_TEXT,
                           theme_->FOCUSREQ_SHINE,
                           theme_->FOCUSREQ_SHADOW,
                           true, true);
        }
        else {
            label_->update(theme_->LABEL_BACKGROUND_NORMAL,
                           theme_->LABEL_TEXT_NORMAL,
                           theme_->LABEL_SHINE_NORMAL,
                           theme_->LABEL_SHADOW_NORMAL,
                           true, true);
        }
        i++;
    }
    *offsetX += i * workspaceWidth + 2;
}

unsigned int StatusBar::calculateMetersWidth() {

    unsigned int result = 0;
    if (meterText_ != "") {
        string percentage = "";
        string text = "";
        bool isPercentage = true;
        unsigned int meterWidth = monitor()->buttonWidth() / 2;
        for (string::iterator it = meterText_.begin();
                it != meterText_.end(); )
        {

            char c = *it;
            it++;
            bool isEndreached = it == meterText_.end();
            if (c == '%') {
                isPercentage = false;
            }
            if ((c == ',') || isEndreached) {
                if (isEndreached && (c != '%')) {
                    text += c;
                }
                isPercentage = true;
                result += 2 + meterWidth;
                if (text != "") {
                    label_->setAlignment(Label::LEFT);
                    label_->setText(text);
                    result += label_->textWidth();
                }
                text = "";
                percentage = "";
            }
            else if (isPercentage && (c == '!')) {
                // do nothing here
            }
            else if (c != '%') {
                if (isPercentage) {
                    percentage += c;
                }
                else {
                    text += c;
                }
            }
        }
    }
    return result;
}

void StatusBar::updateMeters(unsigned int offsetX) {

    if (meterText_ != "") {

        string percentage = "";
        string text = "";
        bool isPercentage = true;
        bool isNegotiated = false;
        unsigned int meterWidth = monitor()->buttonWidth() / 2;
        label_->setX(offsetX);
        label_->setWidth(0);
        for (string::iterator it = meterText_.begin();
                it != meterText_.end(); )
        {

            char c = *it;
            it++;
            bool isEndreached = it == meterText_.end();
            if (c == '%') {
                isPercentage = false;
            }
            if ((c == ',') || isEndreached) {
                if (isEndreached && (c != '%')) {
                    text += c;
                }
                label_->setX(label_->x() + label_->width() + 2);
                label_->setWidth(meterWidth);
                int percent = Util::strToUInt(percentage);
                if (percent < 0) {
                    percent = 0;
                }
                else if (percent > 99) {
                    percent = 99;
                }
                Draw::drawMeter(window(), gc(), label_, percent,
                      theme_->METER_BACKGROUND,
                      isNegotiated ? theme_->METER_FIGURE_LOW : theme_->METER_FIGURE_HIGH,
                      theme_->METER_FIGURE_NORMAL,
                      isNegotiated ? theme_->METER_FIGURE_HIGH : theme_->METER_FIGURE_LOW,
                      theme_->METER_BORDER_SHINE,
                      theme_->METER_BORDER_SHADOW);
                if (text != "") {
                    label_->setAlignment(Label::LEFT);
                    label_->setX(label_->x() + label_->width());
                    label_->setText(text);
                    label_->adjustWidth();
                    label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                                   theme_->BAR_SHINE, theme_->BAR_SHADOW);
                }
                text = "";
                percentage = "";
                isPercentage = true;
                isNegotiated = false;
            }
            else if (isPercentage && (c == '!')) {
                isNegotiated = true;
            }
            else if (c != '%') {

                if (isPercentage) {
                    percentage += c;
                }
                else {
                    text += c;
                }
            }
        }
    }
}

void StatusBar::updateStatus(unsigned int *offsetX) {

    unsigned int metersWidth = calculateMetersWidth();

    // user defined status text
    label_->setAlignment(Label::RIGHT);
    label_->setX(*offsetX);
    label_->setWidth(width() - *offsetX - metersWidth - 5);
    label_->setText(text_);
    label_->update(theme_->BAR_BACKGROUND, theme_->BAR_TEXT,
                   theme_->BAR_SHINE, theme_->BAR_SHADOW);

    *offsetX = label_->x() + label_->width() + 2;
}

void StatusBar::handleButtonRelease(XButtonEvent *event) {

    // TODO: make buttons customizable
    if (buttonPressed_) {
        if (event->button == Button1) {
            LOGDEBUG("toggling interactive mode");
            monitor()->inputBar()->runKey(KERNEL->defaultPrompt());
            buttonPressed_ = false;
        }
    }
}

void StatusBar::handleButtonPress(XButtonEvent *event) {

    if ((event->window != window())) {
        return;
    }

    XCORE->raise(window());

    // TODO: make buttons customizable
    if (event->button == Button1) {
        invokeClickedThing(event->x);
    }
    else if (event->button == Button4) {
        monitor()->focus(monitor()->prev());
    }
    else if (event->button == Button5) {
        monitor()->focus(monitor()->next());
    }
}

void StatusBar::handleMotionNotify(XMotionEvent *event) {

    if (buttonPressed_) {
        buttonPressed_ = false;
        illuminate();
    }
}

void StatusBar::invokeClickedThing(int xPosition) {

    if (isButtonVisible_) {
        if (xPosition < (int)monitor()->buttonWidth() + 1) {
            buttonPressed_ = true;
            illuminate();
            return;
        }
        xPosition -= (monitor()->buttonWidth() + 3);
    }

    unsigned int workspaceNum = xPosition / calculateWorkspaceWidth();
    if (workspaceNum < monitor()->size()) {
        monitor()->focusWorkspaceNum(workspaceNum);
    }
    illuminate();
}

void StatusBar::setText(const string text) {
    text_ = text;
}

void StatusBar::setMeterText(const string meterText) {
    meterText_ = meterText;
}
