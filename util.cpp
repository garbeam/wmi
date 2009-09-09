// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>
}

#include <sstream>
#include <iostream>

#include "util.h"

#include "action.h"

#include "rectangle.h"

Util::Util() {
}

Util::~Util() {
}

string Util::lowerCase(string str) {
    string input = str;

    for (string::iterator it = input.begin(); it != input.end(); it++) {
        *it = tolower(*it);
    }

    return input;
}

bool Util::containsWhitespaces(string str) {

    for (string::iterator it = str.begin(); it != str.end(); it++) {
        if (*it == ' ' || *it == '\t') {
            return true;
        }
    }

    return false;
}

string Util::truncate(string str, char c) {

    string result = "";

    for (string::iterator it = str.begin(); it != str.end(); it++) {
        if ((*it != c) && (*it != '\t')) {
            result += *it;
        }
    }

    return result;
}

string Util::canonicalDir(string path) {
    string dir = canonicalPath(path);
    unsigned int pos = 0;

    pos = dir.find_last_of('/');
    if (pos != string::npos) {
        dir = dir.substr(0, pos);
    }

    return dir;
}

string Util::canonicalPath(string file) {

    string result = "";
    for (string::iterator it = file.begin(); it != file.end(); it++) {

        if (*it == '~') { // found HOME
            char *home = getenv("HOME");
            // Note: if home would be 0, WMI won't have started
            // but who knows what boxes there're around :)
            if (home == 0) {
                cerr << "wmi[E " << timestamp() << "]: "
                     << "Util::canonicalPath() cannot determine your "
                     << "$HOME directory, giving up";
                exit(1);
            }

            result += home;
        }
        else {
            result += *it;
        }
    }
    return result;
}

string Util::encodeEscapes(string str) {
    string encoded = "";

    for (string::iterator it = str.begin(); it != str.end(); it++) {
        switch (*it) {
        case '\\':
            encoded += "\\\\";
            break;
        case '#':
            encoded += "\\#";
            break;
        case '"':
            encoded += "\\\"";
            break;
        case '=':
            encoded += "\\=";
            break;
        default:
            encoded += *it;
            break;
        }
    }

    return encoded;
}

string Util::lastToken(string str, char delim) {

    string result = "";
    unsigned int pos = str.find_last_of(delim);

    if (pos != string::npos) {
        result = str.substr(pos + 1, str.length() - pos);
    }

    return result;
}

string Util::nthToken(string str, char delim, unsigned int n) {

    unsigned int delimCounter = 1;
    string token = "";

    for (string::iterator it = str.begin(); it != str.end(); it++) {

        if (*it == delim) {
            delimCounter++;
            continue;
        }


        if (delimCounter == n) {
            token += *it;
        }
        else if (delimCounter > n) {
            return token; // token ready
        }
    }

    return token;
}

char **Util::arguments(string command) {

    string arg = "";
    list<const char *> args;

    bool proceed = true;
    for (string::iterator it = command.begin(); proceed; it++) {

        proceed = it != command.end();
        if (!proceed || (*it == ' ') || (*it == '\t'))
        {
            if (arg.length() > 0) {
                args.push_back((new string(arg))->c_str());
            }
            arg = "";
        }
        else {
            arg += *it;
        }
    }

    const char **result = new const char *[args.size() + 1];
    unsigned int i = 0;
    for (list<const char *>::iterator it = args.begin();
         it != args.end(); it++, i++)
    {
        result[i] = *it;
    }
    result[i] = 0;

    return (char **)result;
}

unsigned int Util::numDigits(unsigned int number)
{
    unsigned int n = 0;
    do {
        ++n; number /= 10;
    } while (number);

    return n;
}

string Util::timestamp() {
    static time_t utctime;
    static struct tm *todaylocal;
    static char timestamp[16];
    time(&utctime);
    todaylocal = localtime(&utctime);
    strftime(timestamp, 16,"%Y%m%d%H%M%S", todaylocal);

    return (string)timestamp;
}

string Util::stringForButton(unsigned int button) {
    ostringstream oss;

    oss << "Button" << button;
    return oss.str();
}

unsigned int Util::buttonForString(string button) {

    if (button.find("Button1") != string::npos) {
        return Button1;
    }
    else if (button.find("Button2") != string::npos) {
        return Button2;
    }
    else if (button.find("Button3") != string::npos) {
        return Button3;
    }
    else if (button.find("Button4") != string::npos) {
        return Button4;
    }
    else if (button.find("Button5") != string::npos) {
        return Button5;
    }

    return 0;
}

string Util::stringForModMask(unsigned long modMask) {

    string result = "";
    if (modMask & ShiftMask) {
        result += "shift + ";
    }
    if (modMask & LockMask) {
        result += "lock + ";
    }
    if (modMask & ControlMask) {
        result += "ctrl + ";
    }
    if (modMask & Mod1Mask) {
        result += "mod1 + ";
    }
    if (modMask & Mod2Mask) {
        result += "mod2 + ";
    }
    if (modMask & Mod3Mask) {
        result += "mod3 + ";
    }
    if (modMask & Mod4Mask) {
        result += "mod4 + ";
    }
    if (modMask & Mod5Mask) {
        result += "mod5 + ";
    }
    if (!modMask) {
        result += "none + ";
    }
    return result;
}

unsigned long Util::modMaskForString(string modKeys)
{
    unsigned long modMask = 0;
    if (modKeys.find("shift") != string::npos) {
        modMask |= ShiftMask;
    }
    if (modKeys.find("lock") != string::npos) {
        modMask |= LockMask;
    }
    if ((modKeys.find("control") != string::npos) ||
        (modKeys.find("ctrl") != string::npos)) {
        modMask |= ControlMask;
    }
    if ((modKeys.find("alt") != string::npos) ||
        (modKeys.find("mod1") != string::npos)) {
        modMask |= Mod1Mask;
    }
    if (modKeys.find("mod2") != string::npos) {
        modMask |= Mod2Mask;
    }
    if (modKeys.find("mod3") != string::npos) {
        modMask |= Mod3Mask;
    }
    if (modKeys.find("mod4") != string::npos) {
        modMask |= Mod4Mask;
    }
    if (modKeys.find("mod5") != string::npos) {
        modMask |= Mod5Mask;
    }
    return modMask;
}

unsigned int Util::strToUInt(string str) {

    return atoi(str.c_str());
}

void Util::fitIntoDirection(int horigin, int hw, int *corigin, int *cw)
{
    int horigin1 = horigin;
    int horigin2 = horigin + hw;
    int corigin1 = *corigin;
    int corigin2 = *corigin + *cw;

    if ((corigin1 < horigin1) && (corigin2 <= horigin2)) {
        *corigin = horigin;
        if (*corigin + *cw > horigin2) {
            *cw = hw;
        }
    }
    else if ((corigin1 < horigin1) && (corigin2 > horigin2)) {
        *corigin = horigin;
        *cw = hw;
    }
    else if ((corigin1 >= horigin1) && (corigin2 > horigin2)) {
        *cw = horigin2 - *corigin;
    }
}


void Util::fitInto(Rectangle *client, Rectangle *host) {

    int cx = client->x();
    int cw = client->width();

    fitIntoDirection(host->x(), host->width(), &cx, &cw);
    client->setX(cx);
    client->setWidth(cw);

    int cy = client->y();
    int ch = client->height();

    fitIntoDirection(host->y(), host->height(), &cy, &ch);
    client->setY(cy);
    client->setHeight(ch);
}

Direction Util::dirForString(string str) {

    string dirStr;

    dirStr = lowerCase(str);

    if (dirStr == "left") {
        return LEFT;
    }
    else if (dirStr == "right") {
        return RIGHT;
    }
    else if (dirStr == "up") {
        return UP;
    }
    else {
        return DOWN;
    }
}

bool Util::exists(const char *path) {

    return access(path, F_OK) == 0;
}

string Util::shortenString(string str, unsigned int n) {

    return str.substr(0, n) + "...";
}


set<string> Util::stringSplit(string s, string delimiter){
    set<string> res;
    unsigned int pos = 0;
    while (pos != string::npos){
        pos = s.find_first_of(delimiter);
        res.insert(s.substr(0, pos));
        s.erase(0, pos + 1);
    }
    return(res);
}

string Util::trimL(string s){
    return s.substr(s.find_first_not_of(' '));
}


bool Util::isPointWithinRect(unsigned int x, unsigned int y, Rectangle *rect,
                        Rectangle *offset)
{
    unsigned int offsetX = rect->x();
    unsigned int offsetY = rect->y();
    if (offset) {
        offsetX += offset->x();
        offsetY += offset->y();
    }

    return ((offsetX <= x) && (offsetY <= y) &&
            (offsetX + rect->width() >= x) &&
            (offsetY + rect->height() >= y));
}


void Util::swapChars(string *str, char from, char to,
                       unsigned int *swapCount)
{
    for (string::iterator it = str->begin(); it != str->end(); it++) {
        if (*it == from) {
            *it = to;
            if (swapCount) {
                (*swapCount)++;
            }
        }
    }
}

Direction Util::reverseDir(Direction dir) {
    switch (dir) {
    case LEFT:
        return RIGHT;
        break;
    case RIGHT:
        return LEFT;
        break;
    case UP:
        return DOWN;
        break;
    case DOWN:
        return UP;
        break;
    case NORTH_WEST:
        return SOUTH_EAST;
        break;
    case NORTH_EAST:
        return SOUTH_WEST;
        break;
    case SOUTH_WEST:
        return NORTH_EAST;
        break;
    case SOUTH_EAST:
        return NORTH_WEST;
        break;
    }
    return LEFT;
}

string Util::get(MSettings *settings, string key) {
    MSettings::iterator it = settings->find(key);
    return (it != settings->end()) ? (*it).second : "";
}

void Util::remove(MSettings *settings, string prefix) {

    unsigned int length = prefix.length();
    for (MSettings::iterator it = settings->begin();
            it != settings->end(); it++)
    {
        string key = (*it).first;
        if (key.length() >= length) {
            if (key.substr(0, length) == prefix) {
                settings->erase(it);
            }
        }
    }
}

Action *Util::get(MBindings *bindings, string key) {
    MBindings::iterator it = bindings->find(key);
    return (it != bindings->end()) ? (*it).second : 0;
}

void Util::remove(MBindings *bindings, string prefix) {

    unsigned int length = prefix.length();
    for (MBindings::iterator it = bindings->begin();
            it != bindings->end(); it++)
    {
        string key = (*it).first;
        if (key.length() >= length) {
            if (key.substr(0, length) == prefix) {
                bindings->erase(it);
            }
        }
    }
}
