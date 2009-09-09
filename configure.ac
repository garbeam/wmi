AC_INIT(src/main.cpp)
AC_PREREQ(2.52)
AM_INIT_AUTOMAKE(wmi, 10, anselmg@t-online.de)
# next release: AM_INIT_AUTOMAKE(wmi, 10, anselmg@t-online.de)
AM_CONFIG_HEADER(config.h)
# following two AC_CHECK_HEADERS have been provided by Markus Lindorfer
# for Solaris compatibility
AC_CHECK_HEADERS([assert.h ctype.h stdio.h stdlib.h sys/select.h sys/stat.h sys/types.h sys/wait.h time.h unistd.h X11/cursorfont.h X11/keysym.h X11/Xatom.h X11/X.h X11/Xlib.h X11/Xmd.h X11/Xproto.h])
AC_CHECK_HEADERS([X11/Xresource.h X11/Xutil.h],[],[],
[#ifdef HAVE_X11_XLIB_H
 #  include <X11/Xlib.h>
 #endif
])

AC_LANG_CPLUSPLUS

AC_PROG_CXX
AC_PROG_INSTALL
AC_HEADER_STDC

# check X headers and libs
AC_PATH_X
AC_PATH_XTRA

test x$no_x = "xyes" && AC_MSG_ERROR([WMI requires the X Window
        System libraries and headers.])

test x$x_includes = "x" && x_includes="/usr/include"
test x$x_libraries = "x" && x_libraries="/usr/lib"

CFLAGS=""
CXXFLAGS="-Os $CXXFLAGS $X_CFLAGS"
LIBS="$LIBS $X_LIBS $X_PRE_LIBS"

# Check for required functions in -lX11
AC_CHECK_LIB(X11, XOpenDisplay,
          LIBS="$LIBS -lX11", 
          AC_MSG_ERROR([Could not find XOpenDisplay in -lX11.])
)

LIBS="$LIBS $X_EXTRA_LIBS"

# Add extra flags if in debug mode
AC_ARG_WITH(debug, [  --with-debug            enable debug support])

if test "x$with_debug" != "x"; then
    AC_DEFINE(DEBUG, 1, [Define if wmi is to be build with debug information.])
    CXXFLAGS="$X_CFLAGS -g -ggdb -O0 -Wall -Wpointer-arith -Wcast-align -Winline"
fi

# Add extra flags for posix regex feature
AC_ARG_WITH(, [  --with-posix-regex  enable posix regex])

if test "x$with_posix_regex" != "x"; then
    AC_CHECK_HEADERS([regex.h])
    AC_DEFINE(POSIX_REGEX, 1, [Define if wmi is to be build with posix
            regex.])
fi

# Add extra flags for posix regex feature
AC_ARG_WITH(, [  --with-posix-regex  enable posix regex])

if test "x$with_posix_regex" != "x"; then
    AC_CHECK_HEADERS([regex.h])
    AC_DEFINE(POSIX_REGEX, 1, [Define if wmi is to be build with posix
            regex.])
fi


# Add extra flags for diamond box
AC_ARG_WITH(diamond, [  --without-diamond       disable diamond box whily move/resize])

if test "x$with_diamond" = "x"; then
    AC_DEFINE(DIAMOND_BOX, 1, [Define if wmi is to be build with diamond box.])
fi

# Add extra flags for slot
AC_ARG_WITH(slot, [  --without-slot          disable slot])

if test "x$with_slot" = "x"; then
    AC_DEFINE(SLOT_SUPPORT, 1, [Define if wmi is to be build with slot support.])
fi

# Add extra flags for Xft mode checking
# Xft support, we ignore UTF-8 since WMI is not ready for i18n
TMPLIBS=$LIBS
TMPCXXFLAGS=$CXXFLAGS
AC_MSG_CHECKING([whether to have Xft support])
AM_PATH_XFT(yes,
    XFT=true,
    XFT=false
)

XFT_LINK=false

if test "x$XFT" = "xtrue" ; then
    AC_TRY_LINK([
            #include <X11/Xft/Xft.h>
        ], [ XftFontClose(0, 0); return 1; ], 
    [
        XFT_LINK=true
    ])
fi

# Add extra flags for Xft feature
AC_ARG_WITH(xft, [  --without-xft           disable Xft support])

if test "x$with_xft" != "x"; then
#restore old values
    LIBS=$TMPLIBS
    CXXFLAGS=$TMPCXXFLAGS
else
    if test "x$XFT_LINK" = "xtrue" ; then
        AC_DEFINE(XFT_SUPPORT, 1, antialias support)
    fi
fi

LDFLAGS="$LDFLAGS $LIBS"

# print summary
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
AC_MSG_RESULT([Using '$prefix' for installation.])
AC_MSG_RESULT([Using '$CXX' for C++ compiler.])
AC_MSG_RESULT([Building with '$CXXFLAGS' for C++ compiler flags.])
AC_MSG_RESULT([Building with '$LDFLAGS' for linker flags.])
AC_MSG_RESULT([])

AC_OUTPUT([Makefile version.h src/Makefile man/Makefile])
