// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "bar.h"

#include <map>

#include "draw.h"
#include "kernel.h"
#include "label.h"
#include "monitor.h"
#include "theme.h"
#include "util.h"
#include "xcore.h"

Bar::Bar(Monitor *monitor, Rectangle *rect) :
    Widget(monitor, rect)
{
    theme_ = this->monitor()->theme();
    isButtonVisible_ = (Util::get(KERNEL->commonSettings(), ("bar.buttons"))
                        == "yes");
    borderRect_ = Rectangle(0, 0, width(), height());

    label_ = new Label(this->monitor(), window(), Label::CENTER, gc());
    label_->setX(1);
    label_->setY(1);
    label_->setWidth(width() - 2);
    label_->setHeight(height() - 2);
}

Bar::~Bar() {
    delete label_;
}


void Bar::drawBorder() {

    Draw::drawRectBorder(window(), gc(), &borderRect_,
                        theme_->BAR_SHINE, theme_->BAR_SHADOW);

    XCORE->sync();
}
