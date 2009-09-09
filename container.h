// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: container.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __CONTAINER_H
#define __CONTAINER_H

#include "kernel.h"
#include "util.h" // also includes wmi.h

/**
 * A Container object keeps a parent reference and may contain a list of
 * child objects.
 *
 * A Container is a class template that creates an object that contains a list
 * of other objects and also keeps a reference to a parent object.
 * A container can be configured with for stacked tabbing at compile time.
 *
 * @param P the parent class
 * @param LT the child list class
 * @param LTI the iterator class for the child list 
 * @param T the child class 
 */
template <class P, class LT, class LTI, class T>
    class Container
{
public:

    /**
     * Constructs a container with a parent and a focused child. 
     *
     * @param parent the original parent. 
     * @param child the original focused child which is optional.
     */
    Container(P *parent, T *child = 0) {
        parent_ = parent;
        child_ = child;
    };

    /**
     * The destructor.
     *
     * When the destructor is destoryed it will also destroys 
     * all of the children in the container.
     */
    ~Container() {
        Util::destroy<LT, LTI, T *>
                 (&childs_, childs_.begin(), childs_.end());
    };

    /**
     * Checks if the child is contained in this container.
     *
     * @param child the child to search for.
     */
    bool contains(T *child) {
        return Util::contains<LT, LTI, T>(&childs_, child);
    };

    /** 
     * Get the number of children in this container.
     *
     * @return the the number of children
     */
    unsigned int size() {
        return childs_.size();
    };

    /**
     * Get the list iterator to the first child.
     *
     * @return the list iterator to first child. 
     */
    LTI begin() {
        return childs_.begin();
    };

    /** 
     * Get the list iterator to end of child list. 
     *
     * @return the list iterator to the last child.
     */
    LTI end() {
        return childs_.end();
    };

    /** 
     * Get the parent that the container is attached to. 
     *
     * @return the parent.
     */
    P *attached() const {
        return parent_;
    };

    /** 
     * Attach the container to a new parent. 
     *
     * @param parent the parent. 
     */
    void setAttached(P *parent) {
        parent_ = parent;
    };

    /** 
     * Get the currently focused child in the container. 
     *
     * @return the focused child, if any.
     */
    T *focused() const {
        return child_;
    };

    /** 
     * Gives a child focus. 
     *
     * In normal mode the child simply receives focus.  In stacked
     * tabbing mode the currently focused child will be placed at
     * the end of the list and the newly focused child will be placed
     * at the front.
     *
     * @param child the child to receive focus.
     */
    void focus(T *child) {
        if (child && KERNEL->isStackedTabbing()) {
            if (childs_.size()) {
                T *foc = childs_.front();
                if (foc != child) {
                    childs_.remove(foc);
                    childs_.push_back(foc);
                }
            }
            childs_.remove(child);
            childs_.push_front(child);
        }
        child_ = child;
    };

    /** 
     * Attaches a child to the container. 
     *
     * This attaches a new child to the container.
     * When a child is attached it receives focus.
     * <code>attach()</code> will not add a duplicate child to the container.
     *
     * @param child the child to attach.
     */
    void attach(T *child) {
        if (child && !contains(child)) {
            childs_.push_back(child);
            focus(child);
        }
    };

    /** 
     * Detach a child from the container.
     *
     * Detaches the child from the container.  If the child being detached
     * currently has focus then the focus will switch to the previous child.
     * If this was the last child left in the container than there will be no
     * child with focus. 
     *
     * @param child the child to detach.
     * @return the current child with focus.
     */
    T *detach(T *child) {

        if (child_ == child) {
            // cycle to prev child
            focus(prev());
        }
        childs_.remove(child);

        if (childs_.size() < 1) {
            child_ = 0;
        }
        return child_;
    };

    /**
     * Get the previous child in the list.
     *
     * In stacked tabbing mode this is the last child.  In regular
     * mode this is the child before the currently focused child.
     *
     * @return the previous child.
     */
    T *prev() {

        if (childs_.size() < 1) {
            return 0;
        }
        else if (childs_.size() < 2) {
            return child_;
        }

        if (KERNEL->isStackedTabbing()) {
            return childs_.back();
        }
        else {
            for (LTI it = childs_.begin();
                    it != childs_.end(); it++)
            {
                if (child_ == *it) {
                    if (it != childs_.begin()) {
                        it--;
                        return *it;
                    }
                    else {
                        break;
                    }
                }
            }
            return childs_.back();
        }
    }


    /**
     * Get the next child in the list.
     *
     * In stacked tabbing mode this is the second child in the list.
     * In regular mode it is the child after the currently focused child.
     *
     * @return the next child.
     */
    T *next() {
        if (childs_.size() < 1) {
            return 0;
        }
        else if (childs_.size() < 2) {
            return child_;
        }

        if (KERNEL->isStackedTabbing()) {
            LTI it = childs_.begin();
            it++;
            return *it;
        }
        else {
            for (LTI it = childs_.begin();
                    it != childs_.end(); it++)
            {
                if (child_ == *it) {
                    it++;
                    if (it != childs_.end()) {
                        return *it;
                    }
                    else {
                        break;
                    }
                }
            }
            return childs_.front();
        }
    };

    /** Used for direct manipulation, use with care! */
    LT *childs() {
        return &childs_;
    }


private:

    P *parent_;
    T *child_;
    LT childs_;
};

#endif // __CONTAINER_H
