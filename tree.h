// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: tree.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __TREE_H
#define __TREE_H

#include "rectangle.h"

class Frame;
class Tree;

/** Dual-Quad tree with some special WMI related functionality. */
class Tree : public Rectangle {

public:

    Tree(Tree *parent, Rectangle *rect);

    ~Tree();

    Tree *parent() const;

    void setParent(Tree *tree);

    void setFrame(Frame *frame);

    Frame *frame() const;

    Tree *first() const;
    Tree *last() const;

    void setChilds(Tree *first, Tree *last);

    void setHoriz(bool horiz);
    bool isHoriz() const;

    unsigned int visits() const;
    void setVisit(unsigned int visit);

private:

    Tree *first_;
    Tree *last_;
    Tree *parent_;
    Frame *frame_;
    bool isHoriz_;
    unsigned int visits_;
};

#endif // __TREE_H
