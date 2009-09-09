// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: loader.h 734 2004-09-27 18:15:45Z garbeam $

#ifndef __LOADER_H
#define __LOADER_H

#include "wmi.h"

#include <list>
#include <string>
#include <map>

typedef map<string, string, less<string> > MSettings;

/**
 * Loads and saves settings.
 */
class Loader
{

public:

    /** Constructs a new settings manager instance.  */
    Loader();

    ~Loader();

    /**
     * (Re)loads settings. Returns <code>true</code> on success. 
     * The settings file has following grammar:
     * <key>=<value>
     * whitespaces (' ', '\t') are ignored by default (escape using "...")
     * '\' is escape character for escaping from '"', '=', '#', '\n', '\'
     * @param settings the settings to set
     * @param filename the filename to load from
     * @param suppressWarning suppresses warn messages to stderr
     * @param caseSensitive reads keys case sensitive
     * if file couldn't be loaded
     */
    static bool load(MSettings *settings, string path,
                     bool suppressWarning = false,
                     bool caseSensitive = false);

    /** Saves settings. Returns <code>true</code> on success. */
    static bool saveSettings(MSettings *settings, string path);

    /** Saves content to the file specified by path. */
    static bool saveFile(string path, string content);

    /** Prints all settings to stdout. */
    static void print(MSettings *settings);

};

#endif // __LOADER_H
