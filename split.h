// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: split.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __SPLIT_H
#define __SPLIT_H

#include "wmi.h"

class Frame;
class Tree;

/**
 * This class contains all necessary splitting algorithm for managing
 * windows in a non-overlapping manner.
 */
class Split {

public:

    static void resize(Tree *root, Tree *origin,
                       Direction dir, int stepW, int stepH);

    static Tree *neighbor(Tree *root, Tree *origin, Direction dir);
    static bool isNeighbor(Tree *origin, Tree *target, Direction dir);

    static void adjustSize(Tree *tree, bool ignoreVert);

    static void detach(Tree *root, Tree *origin);

    static Frame *firstLeaf(Tree *tree);

    static void attach(Tree *root, Frame *frame, Direction dir);

private:

    /** Helper for neighbor stuff. */
    static int distance(Tree *origin, Tree *target);

    /** Helper for resize stuff. */
    static Tree *topMostRoot(Tree *first, Tree *last);

};

#endif // __SPLIT_H
