// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __PROMPT_H
#define __PROMPT_H

#include <set>
#include <string>
#include "wmi.h"

class Binder;

typedef set<string> Sstring;
typedef string (Binder::*QueryNamesForPrefix)
            (string prefix, Sstring *nameSet);

/**
 * Represents a prompt in input mode. A prompt may also contain a
 * query method pointer for dynamic menu support.
 */
class Prompt {

public:

    Prompt(string prompt, QueryNamesForPrefix toQuery);

    string prompt() const;

    QueryNamesForPrefix toQuery() const;

private:

    string prompt_;
    QueryNamesForPrefix toQuery_;
};

#endif // __PROMPT_H
