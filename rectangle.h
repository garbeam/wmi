// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

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
