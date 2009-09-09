// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
}

#include "expander.h"
#include "util.h"


Expander::Expander(){
    this->key_ = "";
    this->path_ = "";
    this->rehash();
}

bool Expander::rehash(){
    string path_env;
    char *p_c = getenv("PATH");
    if (p_c != NULL) {
        path_env = (string)p_c;
    }
    else{
        return(false);
    }
    paths_ = Util::stringSplit(path_env, ":");
    pathExpands_ = this->read();
    return true;
}

bool Expander::queryString (string s){
    paths_.clear();
    if ((s.find('/') == string::npos) || (Util::trimL(s).substr(0,1) == ".")) {
        //so performing on PATH
        key_ = s;
        pathSearch_=true;
    }
    else{
        //we're using the given string as search-base

        path_ = s.substr(0, s.find_last_of('/')+1);
        key_= s.substr(s.find_last_of('/')+1);

        paths_.insert(path_);
        pathSearch_=false;
    }
    return(true);
}

set<string> Expander::expands (){

    set<string> *target;
    if (pathSearch_) {
        target = &pathExpands_;
    }
    else
    {
        target = &expands_;
        *target = this->read();
    }

    return *target;
}

set<string> Expander::read(){
    set<string> expands;
    DIR *dir;
    dirent *entry;

    for (set<string>::iterator iter = paths_.begin(); iter != paths_.end(); iter++){
        dir=opendir(iter->c_str());
        if (dir != NULL) {
            do {
                entry=readdir(dir);
                if (entry != NULL) {
                    bool expand = false;
                    string filename = (string)(*iter) + "/"
                        + (string)(entry->d_name);

                    /* if we're dealing with a directory, its
                       sufficient to have read-permission (bash
                       acts like this) */

                    struct stat Buf;
                    if (stat(filename.c_str(), &Buf) == 0)
                    {
                        if (S_ISDIR(Buf.st_mode)) {
                            expand = true;
                        }
                    }
                    /* this test for executability uses the real
                       ids (see man 2 access) and thats why may
                       be incorrect if wmi is s(u/g)id'd */

                    if (0 == access(filename.c_str(), X_OK)) {
                        expand = true;
                    }

                    if (expand == true) {
                        expands.insert((string)(entry->d_name));
                    }
                }
            } while (entry != NULL);
            closedir(dir);
        }
    }
    expands.erase(".");
    expands.erase("..");
    return(expands);
}
