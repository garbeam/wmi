// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#include "shortcut.h"

#include "logger.h"
#include "util.h"
#include "xcore.h"

Shortcut::Shortcut(unsigned long modMask, KeyCode keyCode,
                   Shortcut *next, unsigned int button)
{
    modMask_ = modMask;
    button_ = button;
    keyCode_ = keyCode;
    next_ = next;
}

Shortcut::~Shortcut() {
    if (next_) {
        delete next_;
    }
}


Shortcut *Shortcut::shortcut(string keys) {

    if (keys == "") {
        return 0;
    }

    LOGDEBUG("entered shortcut construction");
    // shortcut handling
    Shortcut *root = 0;
    Shortcut *tmp = 0;
    keys = Util::truncate(keys, ' ');
    unsigned int length = keys.length();
    bool sep;
    string subkeys = "";
    for (int i = 0; i < (int)length; i++) {
        sep = keys[i] == ':';
        if (!sep) {
            subkeys += keys[i];
        }
        else {
            i++; // ignore next ':'
        }

        if (sep || (i == (int)(length - 1))) {
            LOGDEBUG("next shortcut: " + subkeys);
            // if no modifier is given, none is assumed
            if (subkeys.find('+') == string::npos) {
                subkeys = "none+" + subkeys;
            }
            string key = Util::lastToken(subkeys, '+');
            // check if the key sequence contains a Button identifier
            unsigned int button = Util::buttonForString(key);
            Shortcut *shortcut = new Shortcut(
                Util::modMaskForString(subkeys),
                XCORE->stringToKeyCode(key), 0, button);
            if (!root) {
                root = shortcut;
            }
            else {
                tmp->setNext(shortcut);
            }
            tmp = shortcut;
            subkeys = "";
        }
    }
    return root;
}


KeyCode Shortcut::keyCode() {
    return keyCode_;
}

unsigned long Shortcut::modMask() const {
    return modMask_;
}

unsigned int Shortcut::button() const {
    return button_;
}

Shortcut *Shortcut::next() const {
    return next_;
}

void Shortcut::setNext(Shortcut *next) {
    next_ = next;
}

void Shortcut::setKeyCode(KeyCode keyCode) {
    keyCode_ = keyCode;
}

void Shortcut::setModMask(unsigned long modMask) {
    modMask_ = modMask;
}
