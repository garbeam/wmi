// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: rectangle.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __RECTANGLE_H
#define __RECTANGLE_H

/**
 * Basic rectangle used by all viewable items of WMI.
 */
class Rectangle
{

public:

    Rectangle();

    Rectangle(int x, int y, unsigned int w, unsigned int h);

    ~Rectangle();

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);

    unsigned int width() const;
    void setWidth(unsigned int w);

    unsigned int height() const;
    void setHeight(unsigned int h);

    void copy(Rectangle *source);

    bool fitsInto(Rectangle *rect);

private:

    int x_, y_;
    unsigned int w_, h_;
};

#endif // __RECTANGLE_H
