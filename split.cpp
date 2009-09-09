// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: split.cpp 734 2004-09-27 18:15:45Z garbeam $

extern "C" {
#include <assert.h>
}

#include <cmath>

#include "split.h"

#include "frame.h"
#include "logger.h"
#include "rectangle.h"
#include "tree.h"
#include "util.h"

Tree *Split::topMostRoot(Tree *first, Tree *last) {

    assert(first);
    assert(last);

    Tree *tmp = first;
    while (tmp->parent()) {
        tmp = tmp->parent();
        tmp->setVisit(tmp->visits() + 1);
    }

    tmp = last;
    while (tmp->parent()) {
        tmp = tmp->parent();
        tmp->setVisit(tmp->visits() + 1);
    }

    // now all tree nodes have been marked, let's
    // search for the first tree node with two visits,
    // this is the topmost root of first an last

    Tree *result = first;
    while (result->visits() < 2) {
        result = result->parent();
    }

    // we got the result, now cleanup on both paths
    tmp = first;
    while (tmp->parent()) {
        tmp = tmp->parent();
        tmp->setVisit(0);
    }

    tmp = last;
    while (tmp->parent()) {
        tmp = tmp->parent();
        tmp->setVisit(0);
    }

    return result;
}

// applied patch by Dr. J. Pfefferl
void Split::resize(Tree *root, Tree *origin,
                   Direction dir, int stepW, int stepH)
{
    int newW, newH;

    if (!origin->frame()) {
        return;
    }

    Tree *neighbor = Split::neighbor(root, origin, dir);
    if (!neighbor|| !neighbor->parent()) {
        return;
    }

    // Normalize stepH/W to positive numbers
    // This is necessary to guarantee that the boundary checks
    // for newW/newH later on work under all circumstances!
    if(0 > stepH || 0 > stepW) {
        stepH *= -1;
        stepW *= -1;
        // Reverse the grow/shrink directions accordingly
        switch(dir) {
        case LEFT:  dir = RIGHT; break;
        case RIGHT: dir = LEFT;  break;
        case UP:    dir = DOWN;  break;
        case DOWN:  dir = UP;    break;
        default: break; // Should not happen
        }
    }

    // neighbor is the nearest neighbor, but this can be
    // a subtree of the top-level neighbor, so we've to
    // determine the top-level neighbor
    Tree *parent = topMostRoot(origin, neighbor);

    // determine the correct root for resizing and if we
    // should grow or shrink the origin
    Tree *growTree, *shrinkTree;
    if ((dir == LEFT) || (dir == UP)) {
        // shrink
        growTree = parent->last();
        shrinkTree = parent->first();
    }
    else {
        // grow
        growTree = parent->first();
        shrinkTree = parent->last();
    }

    switch (dir) {
    case LEFT:
        newW = shrinkTree->width() - stepW;
        if (0 >= newW) {
            return;
        }
        growTree->setX(growTree->x() - stepW);
        growTree->setWidth(growTree->width() + stepW);
        shrinkTree->setWidth(newW);
        break;
    case RIGHT:
        newW = shrinkTree->width() - stepW;
        if (0 >= newW) {
            return;
        }
        growTree->setWidth(growTree->width() + stepW);
        shrinkTree->setX(shrinkTree->x() + stepW);
        shrinkTree->setWidth(newW);
        break;
    case UP:
        newH = shrinkTree->height() - stepH;
        if (0 >= newH) {
            return;
        }
        growTree->setY(growTree->y() - stepH);
        growTree->setHeight(growTree->height() + stepH);
        shrinkTree->setHeight(newH);
        break;
    case DOWN:
        newH = shrinkTree->height() - stepH;
        if(0 >= newH) {
            return;
        }
        growTree->setHeight(growTree->height() + stepH);
        shrinkTree->setY(shrinkTree->y() + stepH);
        shrinkTree->setHeight(newH);
        break;
    default:
        break;
    }
    adjustSize(growTree, ((dir == LEFT) || (dir == RIGHT)));
    adjustSize(shrinkTree, ((dir == LEFT) || (dir == RIGHT)));
}

int Split::distance(Tree *origin, Tree *target) {

    int ox = origin->x() + origin->width() / 2;
    int oy = origin->y() + origin->height() / 2;
    int tx = target->x() + target->width() / 2;
    int ty = target->y() + target->height() / 2;

    return (int)sqrt((double)(((ox - tx) * (ox - tx)) +
                              ((oy - ty) * (oy - ty))));
}

bool Split::isNeighbor(Tree *origin, Tree *target, Direction dir) {

    switch (dir) {
    case LEFT:
        return origin->x() >= (int)(target->x() + target->width());
        break;
    case RIGHT:
        return (int)(origin->x() + origin->width()) <= target->x();
        break;
    case UP:
        return origin->y() >= (int)(target->y() + target->height());
        break;
    case DOWN:
        return (int)(origin->y() + origin->height()) <= target->y();
        break;
    default:
        return false;
        break;
    }
}

Tree *Split::neighbor(Tree *root, Tree *origin, Direction dir) {

    if (root == 0) {
        return 0;
    }

    Tree *result1 = neighbor(root->first(), origin, dir);
    if (!result1 && root->first()) {
        result1 = root->first();
    }

    Tree *result2 = neighbor(root->last(), origin, dir);
    if (!result2 && root->last()) {
        result2 = root->last();
    }

    bool res1 = result1 && (result1 != origin)
                && isNeighbor(origin, result1, dir);
    bool res2 = result2 && (result2 != origin)
                && isNeighbor(origin, result2, dir);

    if (res1 && res2) {

        if (distance(origin, result1)
            < distance(origin, result2))
        {
            return result1;
        }
        else {
            return result2;
        }
    }
    else if (res1) {
        return result1;
    }
    else if (res2) {
        return result2;
    }

    return 0;
}

void Split::adjustSize(Tree *tree, bool ignoreVert) {

    Tree *first = tree->first();
    Tree *last = tree->last();
    if (first) {
        assert(last);
        if (tree->isHoriz()) {
            // horizontal split
            if (ignoreVert) {
                first->copy(tree);
                first->setWidth(tree->width() / 2);
                last->copy(first);
                last->setX(first->x() + first->width());
            }
            else {
                // beware horiz order
                first->setY(tree->y());
                first->setHeight(tree->height());
                last->setY(tree->y());
                last->setHeight(tree->height());
            }
        }
        else {
            // vertical split
            if (!ignoreVert) {
                first->copy(tree);
                first->setHeight(tree->height() / 2);
                last->copy(first);
                last->setY(first->y() + first->height());
            }
            else {
                // beware vert order
                first->setX(tree->x());
                first->setWidth(tree->width());
                last->setX(tree->x());
                last->setWidth(tree->width());
            }
        }
        adjustSize(first, ignoreVert);
        adjustSize(last, ignoreVert);
    }
    else {
        Frame *frame = tree->frame();
        if (frame) {
            frame->copy(tree);
            frame->resize();
        }
    }
}

void Split::detach(Tree *root, Tree *origin) {

    Tree *parent = origin->parent();

    if (parent) {
        Tree *child = (parent->first() != origin)
                      ? parent->first() : parent->last();
        delete origin;
        assert(child);
        if (child->first()) {
            assert(child->last());
            assert(!child->frame());
            parent->setChilds(child->first(), child->last());
            parent->setHoriz(child->isHoriz());
            adjustSize(parent, parent->isHoriz());
            delete child;
        }
        else {
            Frame *frame = child->frame();
            assert(frame);
            parent->setFrame(frame);
            parent->setChilds(0, 0);
            frame->resize();
            delete child;
        }
    }
    else {
        assert(origin == root);
        root->setFrame(0);
    }
}

Frame *Split::firstLeaf(Tree *tree) {

    if (tree->frame()) {
        return tree->frame();
    }
    if (tree->first() && tree->first()->frame()) {
        return tree->first()->frame();
    }
    if (tree->last() && tree->last()->frame()) {
        return tree->last()->frame();
    }
    if (tree->first()) {
        return firstLeaf(tree->first());
    }
    else if (tree->last()) {
        return firstLeaf(tree->last());
    }
    return 0;
}

void Split::attach(Tree *root, Frame *frame, Direction dir) {

    Rectangle rect;
    assert(root->first() == 0);

    Frame *oldFrame = root->frame();
    if (!oldFrame) {
        root->setFrame(frame);
    }
    else {
        root->setFrame(0);
        rect.copy(root);

        if ((dir == LEFT) || (dir == RIGHT)) {
            root->setHoriz(true);
            rect.setWidth(rect.width() / 2);
        }
        else {
            root->setHoriz(false);
            rect.setHeight(rect.height() / 2);
        }

        Tree *first = new Tree(root, &rect);
        Tree *last;
        if ((dir == LEFT) || (dir == UP)) {
            first->setFrame(frame);
            if (dir ==  LEFT) {
                rect.setX(rect.x() + rect.width());
            }
            else { // UP
                rect.setY(rect.y() + rect.height());
            }
            last = new Tree(root, &rect);
            last->setFrame(oldFrame);
        }
        else { // RIGHT || DOWN
            first->setFrame(oldFrame);
            if (dir ==  RIGHT) {
                rect.setX(rect.x() + rect.width());
            }
            else { // DOWN
                rect.setY(rect.y() + rect.height());
            }
            last = new Tree(root, &rect);
            last->setFrame(frame);
        }
        root->setChilds(first, last);
        oldFrame->resize();
    }
    frame->resize();
}
