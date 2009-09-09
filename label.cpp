// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: label.cpp 734 2004-09-27 18:15:45Z garbeam $

#include <sstream>

#include "label.h"

#include "draw.h"
#include "font.h"
#include "logger.h"
#include "kernel.h"
#include "monitor.h"
#include "xcore.h"

Label::Label(Monitor *monitor, Window window, Alignment align, GC gc)
    : Rectangle()
{

    LOGDEBUG("initializing label");
    align_ = align;
    font_ = monitor->font();
    monitor_ = monitor;
    gc_ = gc;
    window_ = window;
}

Label::~Label() {
}

unsigned int Label::adjustWidth() {
    setWidth(textWidth());
    return width();
}

unsigned int Label::textWidth() {
    return font_->textWidth(text_) + 2 * font_->height();
}

void Label::update(unsigned long background, unsigned long text,
                unsigned long shine, unsigned long shadow,
                bool shineBorder, bool shadowBorder)

{

    LOGDEBUG("label update");

    // draw background
    XCORE->setForeground(gc_, background);
    XCORE->fillRectangle(window_, gc_, this);

    // draw border if neccessary
    if (shadowBorder) {
        Draw::drawShadowBorder(window_, gc_, this, shadow);
    }

    if (shineBorder) {

        Draw::drawShineBorder(window_, gc_, this, shine);
    }

    // draw text
    if (text_ == "") {
        return; // nothing to draw
    }
    XCORE->setForeground(gc_, text);
    int textWidth = this->textWidth();

    if (textWidth > (int)width()) {
        int matchLength = (int)((text_.length() * width() / textWidth) - 3);
        if (matchLength < 0) {
            matchLength = 0;
        }
        text_ = Util::shortenString(text_, matchLength);
        textWidth = this->textWidth();
    }

    unsigned int fontBaseLine = 0;
    fontBaseLine = font_->ascent();
    unsigned int fontY = y() + height() / 2 - font_->height() / 2 + fontBaseLine;

    unsigned int fontX = 0;
    switch (align_) {
    case LEFT:
        fontX = x() + font_->height();
        break;
    case CENTER:
        fontX = x() + (width() / 2 - (textWidth - 2 * font_->height()) / 2);
        break;
    case RIGHT:
        fontX = x() + width() - textWidth + font_->height();
        break;
    }

   font_->drawText(window_, gc_, fontX, fontY, text_);
}


void Label::setText(string text) {
    text_ = text;
}

string Label::text() const {
    return text_;
}

void Label::setAlignment(const Alignment align) {
    align_ = align;
}
