// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

extern "C" {
#include "X11/Xatom.h"
}
#include <sstream>

#include "atoms.h"

#include "logger.h"
#include "kernel.h"
#include "wmi.h"
#include "xcore.h"

Atom Atoms::WM_STATE = 0;
Atom Atoms::WM_CHANGE_STATE = 0;
Atom Atoms::WM_PROTOCOLS = 0;
Atom Atoms::WM_DELETE = 0;
Atom Atoms::WM_TAKE_FOCUS = 0;
Atom Atoms::WM_COLORMAPS = 0;

Atom Atoms::MWM_HINTS = 0;

Atom Atoms::NET_WM_DESKTOP = 0;

Atom Atoms::WMI_ACTIONCMD = 0;
Atom Atoms::WMI_STATUSTEXT = 0;
Atom Atoms::WMI_METERTEXT = 0;
Atom Atoms::WMI_PRETTYPRINT_REQUEST = 0;
Atom Atoms::WMI_PRETTYPRINT_RESPONSE = 0;

bool Atoms::initialized_ = false;

void Atoms::initAtoms()
{
    if (initialized_) {
        return;
    }

    ostringstream oss;

    // ICCCM
    WM_STATE = XCORE->internAtom("WM_STATE");
    oss << "WM_STATE: " << WM_STATE << endl;
    WM_CHANGE_STATE = XCORE->internAtom("WM_CHANGE_STATE");
    oss << "WM_CHANGE_STATE: " << WM_CHANGE_STATE << endl;
    WM_PROTOCOLS = XCORE->internAtom("WM_PROTOCOLS");
    oss << "WM_PROTOCOLS: " << WM_PROTOCOLS << endl;
    WM_DELETE = XCORE->internAtom("WM_DELETE_WINDOW");
    oss << "WM_DELETE_WINDOW: " << WM_DELETE << endl;
    WM_TAKE_FOCUS = XCORE->internAtom("WM_TAKE_FOCUS");
    oss << "WM_TAKE_FOCUS: " << WM_TAKE_FOCUS << endl;
    WM_COLORMAPS = XCORE->internAtom("WM_COLORMAP_WINDOWS");
    oss << "WM_COLORMAP_WINDOWS: " << WM_COLORMAPS << endl;
    oss << "XA_WM_ICON_NAME: " << XA_WM_ICON_NAME << endl;
    oss << "XA_WM_NAME: " << XA_WM_NAME << endl;
    oss << "XA_WM_TRANSIENT_FOR: " << XA_WM_TRANSIENT_FOR;

    LOGDEBUG(oss.str());

    // MOTIF
    MWM_HINTS = XCORE->internAtom("_MOTIF_WM_HINTS");

    // NETWM
    NET_WM_DESKTOP = XCORE->internAtom("_NET_WM_DESKTOP");

    // WMI
    WMI_ACTIONCMD = XCORE->internAtom("_WMI_ACTIONCMD");
    WMI_STATUSTEXT = XCORE->internAtom("_WMI_STATUSTEXT");
    WMI_METERTEXT = XCORE->internAtom("_WMI_METERTEXT");
    WMI_PRETTYPRINT_REQUEST =
        XCORE->internAtom("_WMI_PRETTYPRINT_REQUEST");
    WMI_PRETTYPRINT_RESPONSE =
        XCORE->internAtom("_WMI_PRETTYPRINT_RESPONSE");

    initialized_ = true;
}
