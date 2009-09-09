// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: main.cpp 734 2004-09-27 18:15:45Z garbeam $

#include "../config.h"

extern "C" {
#include <assert.h>
#include <unistd.h> // getopt stuff
#include <stdlib.h> // getenv stuff
#include <string.h>
#include <X11/Xlib.h>
}

#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include "wmi.h"

#include "loader.h"
#include "util.h"
#include "kernel.h"


static void version() {
    cout << "wmi - window manager improved - " << __WMI_VERSION
         << endl << " (C)opyright 2003 - 2004 by Anselm R. Garbe"
         << endl;
}

static void usage() {
    cout << "usage: " << endl << endl
         << " wmi [-d <display>] [-h] [-v]" << endl
         << endl
         << "  -d display specifies the display to use" << endl
         << "  -h prints out this help" << endl
         << "  -v prints out above version info" << endl;
}

Display *openDisplay(const char *displayName) {

    Display *display = XOpenDisplay(displayName);
    if (!display) {
        if (displayName != 0) {
            cerr << "wmi[E " << Util::timestamp()
                 << "]: cannot open display \'" 
                 << displayName << "\', giving up" << endl;
        }
        else {
            cerr << "wmi[E " << Util::timestamp()
                 << "]: cannot open default display" 
                 << ", giving up" << endl;
        }
        exit(1);
    }

    return display;
}

static bool loadSettings(MSettings *settings, string filename,
                         const char *home, bool suppressWarning = false,
                         bool caseSensitive = false)
{
    // check global conf
    Loader::load(settings, (string)CONFPATH + "/" + filename, true);

    // check local conf
    string localConf = (string)home + "/.wmi/" + filename;
    bool result = Loader::load(settings, localConf,
                               suppressWarning, caseSensitive);
    if (!suppressWarning && !result) {
        cerr << "wmi[W " << Util::timestamp()
            << "]: cannot load settings from '"
            << localConf << "', using defaults" << endl;
    }
    return result;
}

static MSettings *initThemeSettings() {

    MSettings *themeSettings = new MSettings();

    // init default values
    // new default 24Bit grey color schema
    (*themeSettings)["color.bar.background"] = "#F6F6F6";
    (*themeSettings)["color.bar.text"] = "black";
    (*themeSettings)["color.bar.shine"] = "#999999";
    (*themeSettings)["color.bar.shadow"] = "#999999";
    (*themeSettings)["color.meter.background"] = "#F6F6F6";
    (*themeSettings)["color.meter.figure.high"] = "red";
    (*themeSettings)["color.meter.figure.normal"] = "#253446";
    (*themeSettings)["color.meter.figure.low"] = "green";
    (*themeSettings)["color.meter.border.shine"] = "#7AA4D9";
    (*themeSettings)["color.meter.border.shadow"] = "#7AA4D9";
    (*themeSettings)["color.focusreq.background"] = "#FF4000";
    (*themeSettings)["color.focusreq.text"] = "black";
    (*themeSettings)["color.focusreq.shine"] = "#FFD900";
    (*themeSettings)["color.focusreq.shadow"] = "#FFD900";
    (*themeSettings)["color.tiled.shine.focussed"] = "#FFD900";
    (*themeSettings)["color.tiled.shadow.focussed"] = "#FFD900";
    (*themeSettings)["color.tiled.shine.normal"] = "black";
    (*themeSettings)["color.tiled.shadow.normal"] = "black";
    (*themeSettings)["color.button.background.normal"] = "#3A5D8A";
    (*themeSettings)["color.button.background.pressed"] = "#253446";
    (*themeSettings)["color.button.border.shine.normal"] = "#7AA4D9";
    (*themeSettings)["color.button.border.shine.pressed"] = "#7AA4D9";
    (*themeSettings)["color.button.border.shadow.normal"] = "#7AA4D9";
    (*themeSettings)["color.button.border.shadow.pressed"] = "#7AA4D9";
    (*themeSettings)["color.button.figure.shine.normal"] = "white";
    (*themeSettings)["color.button.figure.shine.pressed"] = "#BDBDBD";
    (*themeSettings)["color.button.figure.shadow.normal"] = "white";
    (*themeSettings)["color.button.figure.shadow.pressed"] = "#BDBDBD";
    (*themeSettings)["color.frame.background.normal"] = "#F6F6F6";
    (*themeSettings)["color.frame.background.focussed"] = "#3E71AA";
    (*themeSettings)["color.frame.pseudo"] = "#D9D9D9";
    (*themeSettings)["color.frame.shine.normal"] = "black";
    (*themeSettings)["color.frame.shine.focussed"] = "#26476A";
    (*themeSettings)["color.frame.shadow.normal"] = "black";
    (*themeSettings)["color.frame.shadow.focussed"] = "#26476A";
    (*themeSettings)["color.label.background.normal"] = "#F6F6F6";
    (*themeSettings)["color.label.background.focussed"] = "#D9D9D9";
    (*themeSettings)["color.label.text.normal"] = "black";
    (*themeSettings)["color.label.text.focussed"] = "black";
    (*themeSettings)["color.label.shine.normal"] = "#999999";
    (*themeSettings)["color.label.shine.focussed"] = "#848484";
    (*themeSettings)["color.label.shadow.normal"] = "#999999";
    (*themeSettings)["color.label.shadow.focussed"] = "#848484";
    (*themeSettings)["color.tab.background.active.normal"] = "#F6F6F6";
    (*themeSettings)["color.tab.background.inactive.normal"] = "#D9D9D9";
    (*themeSettings)["color.tab.background.active.focussed"] = "#3E71AA";
    (*themeSettings)["color.tab.background.inactive.focussed"] = "#F6F6F6";
    (*themeSettings)["color.tab.text.active.normal"] = "#ADADAD";
    (*themeSettings)["color.tab.text.inactive.normal"] = "#ADADAD";
    (*themeSettings)["color.tab.text.active.focussed"] = "white";
    (*themeSettings)["color.tab.text.inactive.focussed"] = "#ADADAD";
    (*themeSettings)["color.tab.shine.active.normal"] = "#F6F6F6";
    (*themeSettings)["color.tab.shine.inactive.normal"] = "#F6F6F6";
    (*themeSettings)["color.tab.shine.active.focussed"] = "#3E71AA";
    (*themeSettings)["color.tab.shine.inactive.focussed"] = "#F6F6F6";
    (*themeSettings)["color.tab.shadow.active.normal"] = "#F6F6F6";
    (*themeSettings)["color.tab.shadow.inactive.normal"] = "#F6F6F6";
    (*themeSettings)["color.tab.shadow.active.focussed"] = "#3E71AA";
    (*themeSettings)["color.tab.shadow.inactive.focussed"] = "#F6F6F6";
    (*themeSettings)["font"] = "fixed";
    (*themeSettings)["exec"] = "xsetroot -solid #7B96AC";

    return themeSettings;
}

static MSettings *initCommonSettings() {

    MSettings *commonSettings = new MSettings();

    // init common settings
    (*commonSettings)["resize-move.factor"] = "10";
    (*commonSettings)["terminal"] = "xterm -e";
    (*commonSettings)["statusbar.alignment"] = "bottom";
#ifdef SLOT_SUPPORT
    (*commonSettings)["slot.alignment"] = "right"; // left
    (*commonSettings)["slot.style"] = "transparent"; // solid
    (*commonSettings)["slot.tabs"] = "default"; // name1,name2,...
    (*commonSettings)["slot.mode"] = "non-overlap"; // overlap
#endif
    (*commonSettings)["bar.buttons"] = "yes"; // no
    (*commonSettings)["frame.buttons"] = "yes"; // no
    (*commonSettings)["frame.colwidth"] = "60";
    (*commonSettings)["frame.autodestroy"] = "yes"; // no
    (*commonSettings)["frame.mode"] = "tabbed"; // tiled
    (*commonSettings)["border.width"] = "3";
    (*commonSettings)["default.client-mode"] = "float";
    (*commonSettings)["default.transient-mode"] = "float";
    (*commonSettings)["default.bar-mode"] = "show"; // hide
    (*commonSettings)["default.border-mode"] = "show"; // hide
    (*commonSettings)["startup.chain"] = "";
    (*commonSettings)["menu.actions"] = "xterm,restart,quit";
    (*commonSettings)["autocompletion.mode"] = "default"; // regex
    (*commonSettings)["cycle.mode"] = "default"; // stackedtabbing

    return commonSettings;
}


static MSettings *initActionSettings() {

    MSettings *actionSettings = new MSettings();

    // intern action bindings
    (*actionSettings)["intern.quit.keys"] = "ctrl+alt+q";
    (*actionSettings)["intern.grow-left.keys"] = "alt+r::h";
    (*actionSettings)["intern.grow-right.keys"] = "alt+r::l";
    (*actionSettings)["intern.grow-up.keys"] = "alt+r::k";
    (*actionSettings)["intern.grow-down.keys"] = "alt+r::j";
    (*actionSettings)["intern.shrink-left.keys"] = "shift+alt+r::h";
    (*actionSettings)["intern.shrink-right.keys"] = "shift+alt+r::l";
    (*actionSettings)["intern.shrink-up.keys"] = "shift+alt+r::k";
    (*actionSettings)["intern.shrink-down.keys"] = "shift+alt+r::j";
    (*actionSettings)["intern.move-client-left.keys"] = "shift+alt+h";
    (*actionSettings)["intern.move-client-right.keys"] = "shift+alt+l";
    (*actionSettings)["intern.move-client-up.keys"] = "shift+alt+k";
    (*actionSettings)["intern.move-client-down.keys"] = "shift+alt+j";
    (*actionSettings)["intern.select-frame-left.keys"] = "alt+h";
    (*actionSettings)["intern.select-frame-right.keys"] = "alt+l";
    (*actionSettings)["intern.select-frame-up.keys"] = "alt+k";
    (*actionSettings)["intern.select-frame-down.keys"] = "alt+j";
    (*actionSettings)["intern.kill-client.keys"] = "ctrl+alt+c";
    (*actionSettings)["intern.cycle-client-next.keys"] = "alt+Tab";
    (*actionSettings)["intern.cycle-client-prev.keys"] = "shift+alt+Tab";
    (*actionSettings)["intern.cycle-workspace-next.keys"] = "ctrl+alt+Right";
    (*actionSettings)["intern.cycle-workspace-prev.keys"] = "ctrl+alt+Left";
    (*actionSettings)["intern.split-frame-left.keys"] = "alt+s::h";
    (*actionSettings)["intern.split-frame-right.keys"] = "alt+s::l";
    (*actionSettings)["intern.split-frame-up.keys"] = "alt+s::k";
    (*actionSettings)["intern.split-frame-down.keys"] = "alt+s::j";
    (*actionSettings)["intern.join-frame-left.keys"] = "shift+alt+s::h";
    (*actionSettings)["intern.join-frame-right.keys"] = "shift+alt+s::l";
    (*actionSettings)["intern.join-frame-up.keys"] = "shift+alt+s::k";
    (*actionSettings)["intern.join-frame-down.keys"] = "shift+alt+s::j";
    (*actionSettings)["intern.detach-client.keys"] = "alt+d";
    (*actionSettings)["intern.attach-last-client.keys"] = "alt+a";
    (*actionSettings)["intern.toggle-client-mode.keys"] = "alt+m";
    (*actionSettings)["intern.toggle-mode.keys"] = "alt+f";
    (*actionSettings)["intern.inputmode.keys"] = "alt+i";
    (*actionSettings)["intern.exec.keys"] = "alt+e";
    (*actionSettings)["intern.grab-move.keys"] = "mod1+Button1";
    (*actionSettings)["intern.zoom-client.keys"] = "mod1+z";

    // sample extern commands
    (*actionSettings)["extern.xterm.cmd"] = "xterm";
    (*actionSettings)["extern.xterm.keys"] = "alt+x";

    // sample chains
    (*actionSettings)["chain.grow-full.seq"] =
        "grow-left,grow-right,grow-up,grow-down";
    (*actionSettings)["chain.grow-full.keys"] = "alt+r::f";
    (*actionSettings)["chain.shrink-full.seq"] =
        "shrink-left,shrink-right,shrink-up,shrink-down";
    (*actionSettings)["chain.shrink-full.keys"] = "shift+alt+r::f";

    return actionSettings;
}

static MSettings *initSessionSettings(Display *display)
{
    MSettings *sessionSettings = new MSettings();

    unsigned int monitorCount = ScreenCount(display);

    ostringstream prefix;

    for (unsigned int i = 0; i < monitorCount; i++) {
        prefix.str("");
        prefix << "monitor[" << i << "]";

        (*sessionSettings)[prefix.str() + ".workspaces"] = "2";
        (*sessionSettings)[prefix.str() + ".focused"] = "0";
        (*sessionSettings)[prefix.str() + ".workspace[0].name"] = "workspace 1";
        (*sessionSettings)[prefix.str() + ".workspace[0].clientbar"] = "yes";
        (*sessionSettings)[prefix.str() + ".workspace[0].statusbar"] = "yes";
        (*sessionSettings)[prefix.str() + ".workspace[1].name"] = "workspace 2";
        (*sessionSettings)[prefix.str() + ".workspace[1].clientbar"] = "yes";
        (*sessionSettings)[prefix.str() + ".workspace[1].statusbar"] = "yes";
#ifdef SLOT_SUPPORT
        (*sessionSettings)[prefix.str() + ".workspace[0].slot"] = "default";
        (*sessionSettings)[prefix.str() + ".workspace[1].slot"] = "default";
#endif
    }

    return sessionSettings;
}

////////////////////////////////////////////////////////////
// Here WMI's life begins
int main(int argc, char *argv[])
{
    string *displayName = 0;
    int c;
    unsigned int size;

    // command line args
    while ((c = getopt(argc, argv, "vhd:")) != -1) {
        switch (c) {
        case 'd':
            size = strlen(optarg);
            displayName = new string(optarg);
            break;
        case 'v':
            version();
            exit(0);
            break;
        case '?':
        case 'h':
            usage();
            exit(1);
            break;
        }
    }

    const char *home = getenv("HOME");
    if (!home) {
        cerr << "wmi[E " << Util::timestamp()
            << "]: cannot determine your $HOME directory,"
            << " giving up" << endl;
        exit(1);
    }

    Display *display = openDisplay((displayName != 0) ?
                                   displayName->c_str() : 0);

    // settings
    MSettings *themeSettings = initThemeSettings();
    bool existsThemeConf = loadSettings(themeSettings, "theme.conf", home);
    MSettings *commonSettings = initCommonSettings();
    bool existsCommonConf = loadSettings(commonSettings, "common.conf", home);
    MSettings *actionSettings = initActionSettings();
    loadSettings(actionSettings, "actions.session", home);
    loadSettings(actionSettings, "actions.conf", home, true);
    MSettings *sessionSettings = initSessionSettings(display);
    loadSettings(sessionSettings, "wmi.session", home, false, true);
    loadSettings(sessionSettings, "session.conf", home, true, true);

    // only one Kernel instance is needed
    int result = Kernel::instance()->start(argv, display,
            themeSettings, commonSettings, actionSettings,
            sessionSettings, !existsCommonConf);

    // Check if we should safe all current settings to the rc file
    if (result == 0) {
        if (!existsThemeConf) {
            Loader::saveSettings(themeSettings, (string)home +
                    "/.wmi/theme.conf");
        }
        if (!existsCommonConf) {
            Loader::saveSettings(commonSettings, (string)home +
                    "/.wmi/common.conf");
        }
        Kernel::instance()->saveSettings();
    }

    return result;
}
