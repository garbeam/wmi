// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __SINGLETON_H
#define __SINGLETON_H

/**
 * Basic singleton template used by various controller classes of WMI.
 */
template <class T> class Singleton
{

public:

    static T *instance() {
        if (instance_ == 0) {
            // use default constructor
            instance_ = new T;
        }
        return instance_;
    }

private:

    // the singleton instance
    static T *instance_;
};

template <class T> T* Singleton<T>::instance_ = 0;

#endif // __SINGLETON_H
