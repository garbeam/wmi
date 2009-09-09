// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __EXPANDER_H
#define __EXPANDER_H

#include <string>
#include <set>

#include "singleton.h"

using namespace std;


/**
 * Provides PATH executable expansion.
 */
class Expander : public Singleton<Expander>{
    public:
        Expander();
        bool queryString (string s);
        bool rehash();
        set<string> expands();

    private:
        set<string> read();
        bool pathSearch_;
        string key_;
        string path_;
        set<string> paths_;
        set<string> expands_;
        set<string> pathExpands_;
};

#endif // __EXPANDER_H
