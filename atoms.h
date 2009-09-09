// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __ATOMS_H
#define __ATOMS_H

extern "C" {
#include "X11/Xlib.h"
}

/**
 * Container of all necessary X atoms used by WMI.
 */
class Atoms
{

public:

    /** Inits all atoms. */
    static void initAtoms();

    /** ICCCM atoms */
    static Atom WM_STATE;
    static Atom WM_CHANGE_STATE;
    static Atom WM_PROTOCOLS;
    static Atom WM_DELETE;
    static Atom WM_TAKE_FOCUS;
    static Atom WM_COLORMAPS;

    /** MOTIF atoms */
    static Atom MWM_HINTS;

    /** NETWM atoms */
    static Atom NET_WM_DESKTOP;

    /** wmi atoms */
    static Atom WMI_ACTIONCMD;
    static Atom WMI_METERTEXT;
    static Atom WMI_STATUSTEXT;
    static Atom WMI_PRETTYPRINT_REQUEST;
    static Atom WMI_PRETTYPRINT_RESPONSE;

private:

    static bool initialized_;
};

#endif // __ATOMS_H
