// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: tree.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "tree.h"

#include "frame.h"

Tree::Tree(Tree *parent, Rectangle *rect)
    : Rectangle(*rect)
{
    parent_ = parent;
    frame_ = 0;
    first_ = last_ = 0;
    visits_ = 0;
}

Tree::~Tree() {
}

Tree *Tree::parent() const {
    return parent_;
}

void Tree::setParent(Tree *tree) {
    parent_ = tree;
}

void Tree::setFrame(Frame *frame) {
    frame_ = frame;
    if (frame_) {
        frame_->setTree(this);
        frame_->copy(this);
    }
}

Frame *Tree::frame() const {
    return frame_;
}

Tree *Tree::first() const {
    return first_;
}

Tree *Tree::last() const {
    return last_;
}

void Tree::setChilds(Tree *first, Tree *last) {
    first_ = first;
    last_ = last;
    if (first_ && last_) {
        first_->setParent(this);
        last_->setParent(this);
    }
}

void Tree::setHoriz(bool horiz) {
    isHoriz_ = horiz;
}

bool Tree::isHoriz() const {
    return isHoriz_;
}

unsigned int Tree::visits() const {
    return visits_;
}

void Tree::setVisit(unsigned int visit) {
    visits_ = visit;
}
