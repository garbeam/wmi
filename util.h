// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __UTIL_H
#define __UTIL_H

extern "C" {
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <sys/stat.h>
}

#include <list>
#include <string>
#include <set>
#include <map>

#include "wmi.h"

class Action;
class Rectangle;

typedef map<string, string, less<string> > MSettings;
typedef map<string, Action *, less<string> > MBindings;

/**
 * Contains various helper methods which are missing in STL library.
 */
class Util
{

public:

    Util();

    ~Util();

    /** Returns n'th Token (substring) of str, delimeted by delim. */
    static string nthToken(string str, char delim, unsigned int n);

    /** Returns last token of str, delimeted by delim. */
    static string lastToken(string str, char delim);

    static string lowerCase(string str);

    /** Returns the canonical path of the given file string. */
    static string canonicalPath(string file);

    /** Returns all dir components of the given path as new string. */
    static string canonicalDir(string path);

    static bool containsWhitespaces(string str);

    /** Returns truncated string without characters given by c or \t. */
    static string truncate(string str, char c);

    /** Returns string with swapped characters (from -> to). */
    static void swapChars(string *str, char from, char to,
                            unsigned int *swapCount = 0);


    /** Shortens this string to n characters plus "..." */
    static string shortenString(string str, unsigned int n);

    static string encodeEscapes(string str);

    static unsigned int numDigits(unsigned int number);

    static unsigned long modMaskForString(string modKeys);
    static string stringForModMask(unsigned long modMask);

    static string stringForButton(unsigned int button);
    static unsigned int buttonForString(string button);

    /** Converts string into char * argument array. */
    static char **arguments(string command);

    /** Returns timestamp of current system time. */
    static string timestamp();

    static unsigned int strToUInt(string str);

    /** Fits client rectangle into host rectangle. */
    static void fitInto(Rectangle *client, Rectangle *host);

    /** Rectangle <code>true</code> if the coord is within the rectangle. */
    static bool isPointWithinRect(unsigned int x, unsigned int y, Rectangle *rect,
                                  Rectangle *offset = 0);

    /** Returns direction for the given string. */
    static Direction dirForString(string str);

    static bool exists(const char *path);

    static set<string> stringSplit(string s, string delimiter);

    /** removes leading blanks */
    static string trimL(string s);

    static Direction reverseDir(Direction dir);

    /**
     * Deletes all elements of a list L, between iterator start and end
     * with type T.
     */
    template <class L, class LTI, class T>
    static void destroy(L *l, LTI start, LTI end) {
        for (LTI it = start; it != end; it++) {
            T t = (T)*it;
            delete t;
        }

        l->erase(start, end);
    }

    /**
     * Returns <code>true</code> if the item is contained in this list.
     */
    template <class L, class LTI, class T>
    static bool contains(L *l, T *item) {
        for (LTI it = l->begin(); it != l->end(); it++) {
            if (item == *it) {
                return true;
            }
        }
        return false;
    }

    static string get(MSettings *settings, string key);
    static void remove(MSettings *settings, string prefix);

    static Action *get(MBindings *bindings, string key);
    static void remove(MBindings *bindings, string prefix);

private:

    /** Fits 1-dimensional c-coordinates directional into h-coordinates. */
    static void fitIntoDirection(int horigin, int hw, int *corigin, int *cw);

};

#endif // __UTIL_H
