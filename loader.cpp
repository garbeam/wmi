// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include <sys/stat.h>
#include <sys/types.h>
}

#include <fstream>
#include <iostream>
#include <sstream>

#include "loader.h"
#include "util.h"

Loader::Loader() {
}

Loader::~Loader() {
}

bool Loader::load(MSettings *settings, string path,
                  bool suppressWarning, bool caseSensitive)
{

    ifstream is(path.c_str());

    if (!is) {
        if (!suppressWarning) {
            cerr << "wmi[W " << Util::timestamp()
                << "]: cannot open/read configuration file '"
                << path << "', skipping" << endl;
        }
        return false;
    }

    string key = "";
    string value = "";
    string buf = "";
    bool comment = false;
    bool ignoreWhitespaces = true;
    bool escape = false; // provides using {#, ", =, \n} characters
    int c;

    while (!is.eof()) {

        c = is.get();

        switch (c) {
        case ' ':
        case '\t':
            if (!comment && !ignoreWhitespaces) {
                buf += c;
            }
            escape = false;
            break;
        case '"': // toggle ignore whitespace/or escape this character
            if (!comment) {
                if (escape) {
                    buf += c;
                }
                else {
                    ignoreWhitespaces = !ignoreWhitespaces;
                }
            }
            escape = false;
            break;
        case '\\': // escape
            if (!comment) {
                if (escape) {
                    buf += c;
                    escape = false;
                }
                else {
                    escape = true;
                }
            }
            break;
        case '#': // comment
            if (escape) {
                buf += c;
            }
            else {
                comment = true;
            }
            escape = false;
            break;
        case '=':
            if (!comment) {
                if (escape) {
                    buf += c;
                }
                else {
                    key = buf;
                    buf = "";
                }
            }
            escape = false;
            break;
        case '\n':

#define ENDLINE \
            if (!escape) { \
                comment = false; \
                ignoreWhitespaces = true; \
                value = buf; \
                if ((key != "") && (value != "")) { \
                    value = buf; \
                    if (caseSensitive) { \
                        (*settings)[key] = value; \
                    } \
                    else { \
                        (*settings)[Util::lowerCase(key)] = value; \
                    } \
                } \
                buf = key = value = ""; \
            } \
            escape = false; \
            break; \
	    
            ENDLINE
	case EOF :
	    ENDLINE
        default:
            if (!comment) {
                buf += c;
            }
            escape = false;
            break;
        }
    }
    is.close();

    return true;
}

bool Loader::saveSettings(MSettings *settings, string path) {

    ostringstream content; 

    for (MSettings::iterator it = settings->begin();
            it != settings->end(); it++)
    {
        string value = Util::encodeEscapes((*it).second);
        if (Util::containsWhitespaces(value)) {
            content << (*it).first << "=\"" << value << "\"" << endl;
        }
        else {
            content << (*it).first << "=" << value << endl;
        }
    }

    return saveFile(path, content.str());
}

bool Loader::saveFile(string path, string content) {

    string dir = Util::canonicalDir(path);
    if (!Util::exists(dir.c_str())) {

        mode_t mode = umask(0);
        mode = (mode ^ (S_IRWXU | S_IRWXG | S_IRWXO)) | S_IWUSR | S_IXUSR;
        if (mkdir(dir.c_str(), mode) != 0) {
            cerr << "wmi[W " << Util::timestamp()
                << "]: cannot create non-existing directory '"
                << dir << "', skipping" << endl;
        }
    }

    ofstream os(path.c_str());

    if (!os) {
        cerr << "wmi[W " << Util::timestamp()
             << "]: cannot open/write configuration file '"
             << path << "', skipping" << endl;
        return false;
    }

    os << content;
    os.close();

    return true;

}

void Loader::print(MSettings *settings) {

    MSettings::iterator it = settings->begin();
    while (it != settings->end()) {
        cout << (*it).first << "=" << (*it).second << endl;
        it++;
    }
}
