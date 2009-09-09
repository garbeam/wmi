// Copyright (c) 2003 - 2009 Anselm R Garbe <anselm@garbe.us>
// See LICENSE for license details.

#ifndef __WMI_H
#define __WMI_H

#include "config.h"

#define LOGERROR(X, Y) Logger::instance()->error(X, Y)
#define LOGWARN(X) Logger::instance()->warning(X)
#define CONFPATH "/etc/wmi"
#define KERNEL Kernel::instance()
#define XCORE XCore::instance()

#ifdef DEBUG
#define LOGDEBUG(X) Logger::instance()->debug(X)
#else
#define LOGDEBUG(X)
#endif

enum Direction {LEFT, RIGHT, UP, DOWN, NORTH_WEST,
                NORTH_EAST, SOUTH_WEST, SOUTH_EAST};

// we use only std namespace
using namespace std;

// Motif bits - see Motif reference manual for further information.
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)

#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)

#define MINIMAL_BARHEIGHT       13
#endif // __WMI_H
