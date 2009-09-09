// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: rectangle.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "rectangle.h"

Rectangle::Rectangle()
{
    x_ = 0;
    y_ = 0;
    w_ = 0;
    h_ = 0;
}

Rectangle::Rectangle(int x, int y, unsigned int w, unsigned int h) {
    x_ = x;
    y_ = y;
    w_ = w;
    h_ = h;
}

void Rectangle::copy(Rectangle *source) {
    x_ = source->x();
    y_ = source->y();
    w_ = source->width();
    h_ = source->height();
}

bool Rectangle::fitsInto(Rectangle *rect) {
    return ((x_ >= rect->x()) &&
            (y_ >= rect->y()) &&
            (w_ <= rect->width()) &&
            (h_ <= rect->height()) &&
            (x_ + w_ <= rect->x() + rect->width()) &&
            (y_ + h_ <= rect->y() + rect->height()));
}


Rectangle::~Rectangle() {}

int Rectangle::x() const { return x_; }
void Rectangle::setX(int x) { x_ = x; }

int Rectangle::y() const { return y_; }
void Rectangle::setY(int y) { y_ = y; }

unsigned int Rectangle::width() const { return w_; }
void Rectangle::setWidth(unsigned int w) { w_ = w; }

unsigned int Rectangle::height() const { return h_; }
void Rectangle::setHeight(unsigned int h) { h_ = h; }
