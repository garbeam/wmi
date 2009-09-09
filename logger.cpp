// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include <stdlib.h>
}

#include <iostream>

#include "logger.h"
#include "util.h"

Logger::Logger() {

}

Logger::~Logger() {

}

void Logger::error(string message, bool fatal) {
    cerr << "wmi[E " << Util::timestamp() << "]: " << message;
    if (fatal) {
        cerr << ", giving up" << endl;
        exit(1);
    }
    cerr << endl;
}

void Logger::warning(string message) {
    cerr << "wmi[W " << Util::timestamp() << "]: " << message << endl;
}

void Logger::debug(string message) {
    cout << "wmi[D " << Util::timestamp() << "]: "
         << message << endl;
}
