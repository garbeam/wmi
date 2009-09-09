// Copyright (c) 2003 - 2004 Anselm R. Garbe <anselmg at t-online.de>
// See ../LICENSE.txt for license details.
//
// $Id: monitor.cpp 734 2004-09-27 18:15:45Z garbeam $

extern "C" {
#include <assert.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

#include <iostream>
#include <sstream>

#include "monitor.h"

#include "action.h"
#include "actions.h"
#include "atoms.h"
#include "binder.h"
#include "box.h"
#include "client.h"
#include "clientbar.h"
#include "cursors.h"
#include "draw.h"
#include "font.h"
#include "frame.h"
#include "inputbar.h"
#include "logger.h"
#include "menu.h"
#include "rectangle.h"
#include "slot.h"
#include "statusbar.h"
#include "theme.h"
#include "thing.h"
#include "util.h"
#include "kernel.h"
#include "workspace.h"
#include "xcore.h"
#include "xfont.h"

Monitor::Monitor(Display *display, unsigned int id)
    : Container<Display, LWorkspace, LWorkspace::iterator, Workspace>(display),
      Rectangle(), detachedClients_(this), stickyClients_(this)
{

    ostringstream oss;
    oss << "monitor[" << id << "]";
    prefix_ = oss.str();
    id_ = id;
    themeSettings_ = KERNEL->themeSettings();
    commonSettings_ = KERNEL->commonSettings();
    sessionSettings_ = KERNEL->sessionSettings();
    setWidth(DisplayWidth(attached(), id_));
    setHeight(DisplayHeight(attached(), id_));
    rootWindow_ = RootWindow(attached(), id_);

    resizeFactor_ = Util::strToUInt(
            Util::get(commonSettings_, "resize-move.factor"));

    theme_ = new Theme();
    maxThing_ = 0;
    clientIds_ = 0;
    fixed_ = 0;

    initFonts();
    LOGDEBUG("font initialized");
    initDisplayString();
    LOGDEBUG("display string '" + displayString_ + "' initialized");
    theme_->initTheme(themeSettings_, XCORE->defaultColormap(id_));
    LOGDEBUG("colors initialized");
    initGC();
    LOGDEBUG("GC initialized");
    initBars();
    LOGDEBUG("bars initialized");
    XCORE->sync();
#ifdef SLOT_SUPPORT
    initSlot();
    LOGDEBUG("slot initialized");
#endif // SLOT_SUPPORT
    initWorkspaces();
    LOGDEBUG("workspaces initialized");

    XCORE->selectEvents(rootWindow_,
                 SubstructureRedirectMask | SubstructureNotifyMask 
                 | PropertyChangeMask);

    KERNEL->installCursor(Cursors::NORMAL_CURSOR, rootWindow_);

    XCORE->sync();
}

void Monitor::initFonts() {

    string fontName = Util::get(KERNEL->themeSettings(), "font");
    font_ = WFont::load(this, fontName);
    if (!font_) {
        // fallback, trying to load "fixed"
        LOGWARN("cannot load font '" + fontName +
                     "', trying loading default font 'fixed'");
        font_ = WFont::load(this, "fixed");
        if (!font_) {
            LOGERROR("cannot load any font", true);
            // exits WMI
        }
    }
    if (font_->height() < MINIMAL_BARHEIGHT) {
        font_->setHeight(MINIMAL_BARHEIGHT);
    }
    if (font_->type() == WFont::XFT) {
        fixed_ = XFont::load(this, "fixed");
        if (!fixed_) {
            LOGERROR("cannot load any font", true);
            // exits WMI
        }
    }
}

void Monitor::initBars() {

    Rectangle rect(0, 0, width(),  titleBarHeight() + 2);

    if (Util::get(commonSettings_, "statusbar.alignment") != "bottom") {
        statusBar_ = new StatusBar(this, &rect);
        inputBar_ = new InputBar(this, &rect);
        rect.setY(height() - rect.height());
        clientBar_ = new ClientBar(this, &rect);
    }
    else {
        clientBar_ = new ClientBar(this, &rect);
        rect.setY(height() - rect.height());
        statusBar_ = new StatusBar(this, &rect);
        inputBar_ = new InputBar(this, &rect);
    }

    rect.setY(height() / 2 - rect.height() / 2);
    box_ = new Box(this, &rect, "");
}

void Monitor::initGC() {

    unsigned long mask = 
                  GCForeground | GCGraphicsExposures
                | GCFunction | GCSubwindowMode | GCLineWidth
                | GCPlaneMask;
    XGCValues gcv;
    gcv.subwindow_mode=IncludeInferiors;
    gcv.function=GXxor;
    gcv.foreground = theme_->FRAME_PSEUDO;
    gcv.fill_style=FillSolid;
    gcv.line_width = Util::strToUInt(
            Util::get(KERNEL->commonSettings(), "border.width"));
    gcv.plane_mask = AllPlanes;
    gcv.graphics_exposures = False;
    if (font_->type() == WFont::NORMAL) {
        gcv.font = ((XFont *)font_)->font()->fid;
        mask |= GCFont;
    }

    pseudoBorderGC_ = XCORE->createGC(rootWindow_, mask, &gcv);

    gcv.function=GXcopy;
    borderGC_ = XCORE->createGC(rootWindow_, mask, &gcv);
}

Monitor::~Monitor() {
    for (MClient::iterator it = clients_.begin();
         it != clients_.end(); it++)
    {
        Client *client = (*it).second;
        clients_.erase(it);
        delete client;
    }
    delete statusBar_;
    delete clientBar_;
#ifdef SLOT_SUPPORT
    delete slot_;
#endif // SLOT_SUPPORT
}

void Monitor::rectForWorkspace(Workspace *workspace) {

    bool isClientBarVisible = workspace->isClientBarVisible();
    bool isStatusBarVisible = workspace->isStatusBarVisible();

    // bar dimensions
    if (Util::get(commonSettings_, "statusbar.alignment") != "bottom") {
        workspace->setY((isStatusBarVisible ? statusBar_->height() : 0));
    }
    else {
        workspace->setY((isClientBarVisible ? clientBar_->height() : 0));
    }
    workspace->setHeight(height()
                         - (isStatusBarVisible ?  statusBar_->height() : 0)
                         - (isClientBarVisible ?  clientBar_->height() : 0));

#ifdef SLOT_SUPPORT
    static bool isSlotOverlap =
        Util::get(commonSettings_, "slot.mode") == "overlap";
    // slot dimensions
    if (workspace->isSlotVisible() && !isSlotOverlap
        && slot_->hasClients())
    {
        if (slot_->alignment() == LEFT) {
            workspace->setX(slot_->width());
        }
        else {
            workspace->setX(x());
        }
        workspace->setWidth(width() - slot_->width());
    }
    else
#endif // SLOT_SUPPORT
    {
        workspace->setX(x());
        workspace->setWidth(width());
    }
}

void Monitor::initWorkspaces() {

    string key = prefix_ + ".workspaces";

    unsigned int workspaceCount =
        Util::strToUInt(Util::get(sessionSettings_, key));
    string id =
        Util::get(KERNEL->sessionSettings(), prefix_ + ".focused");
    unsigned int focusedWSNum = (id != "") ? Util::strToUInt(id) : 0;

    Rectangle rect;
    Workspace *toFocus = 0;
    for (unsigned int i = 0; i < workspaceCount; i++) {
        Workspace *workspace = new Workspace(this, i, &rect);
        matchWorkspace(workspace, LEFT);
        attach(workspace);
        if (i == focusedWSNum) {
            toFocus = workspace;
        }
    }
    LOGDEBUG("workspaces created");
    XCORE->sync();
    focus(toFocus);
}

void Monitor::initDisplayString() {

    ostringstream oss;

    displayString_ = DisplayString(attached());
    unsigned int dot = displayString_.rfind('.');

    LOGDEBUG("initializing display string '" + displayString_ + "'");
    oss << "DISPLAY=";
    LOGDEBUG("ok displayString_");
    if (dot > 0) {
        oss << displayString_.substr(0, dot + 1) << id_;
    }
    else {
        oss << displayString_;
    }

    displayString_ = oss.str();
}

void Monitor::scanWindows() {

    unsigned int numWindows = 0;
    Window *windows;
    XWindowAttributes attr;

    // Queries only the tree of this monitor
    XCORE->queryTree(rootWindow_, &windows, &numWindows);

    ostringstream oss;
    oss << "scanning " << numWindows << " windows";
    LOGDEBUG(oss.str());
    for (unsigned int i = 0; i < numWindows; i++) {
        XCORE->windowAttributes(windows[i], &attr);

        // ignore default windows
        if (attr.override_redirect ||
            menuWindow(windows[i]) ||
            (box_->window() == windows[i]) ||
            barWindow(windows[i]) ||
#ifdef SLOT_SUPPORT
            slotWindow(windows[i]) ||
#endif // SLOT_SUPPORT
            (windows[i] == rootWindow_))
        {
            LOGDEBUG("ignoring scanned window (override redirect)");
            continue;
        }

        if (XCORE->transient(windows[i])) {
            LOGDEBUG("ignoring scanned window (transient)");
            continue;
        }

        // determine client
        Client *client;
        if ((client = clientForWindow(windows[i])) == 0) {
            if (attr.map_state == IsViewable) {
                client = new Client(this, windows[i], &attr);
                addClient(client);
                attachClient(client);
            }
        }
    }
    if (windows) {
        XCORE->free((void *)windows);
    }
}

Thing *Monitor::thingWindow(Window window) {

    for (LWorkspace::iterator it = begin(); it != end(); it++) {
        Workspace *workspace = *it;
        Thing *thing = workspace->thingWindow(window);
        if (thing) {
            return thing;
        }
    }
    return 0;
}


Bar *Monitor::barWindow(Window window) {

    if (statusBar_->window() == window) {
        return statusBar_;
    }
    if (clientBar_->window() == window) {
        return clientBar_;
    }
    if (inputBar_->window() == window) {
        return inputBar_;
    }
    return 0;
}

Menu *Monitor::menuWindow(Window window) {
    Menu *menu = clientBar_->menu();
    if (clientBar_->menu()->window() == window) {
        return menu;
    }

    return 0;
}

#ifdef SLOT_SUPPORT
void Monitor::initSlot() {

    Direction align;
    string valueAlign = Util::get(commonSettings_, "slot.alignment");
    Rectangle rect;

    int offsetX = 0;
    if (valueAlign == "right") {
        align = RIGHT;
        offsetX = width() - 1; // initial offset :)
    }
    else {
        align = LEFT;
    }
    rect = Rectangle(offsetX, titleBarHeight(), 1,
                     height() - 2 * titleBarHeight());

    slot_ = new Slot(this, &rect, align);
}


Slot *Monitor::slotWindow(Window window) {

    if (slot_->window() == window) {
        return slot_;
    }

    return 0;
}

Slot *Monitor::slot() const {
    return slot_;
}

void Monitor::unslotClient() {

    Client *client = slot_->focused()->focused();
    if (client) {
        slot_->detachClient(client);
        client->setMode(Client::FLOAT);
        attachClient(client);
    }
}

void Monitor::slotClient() {

    Client *client = focusedClient();
    if (client) {
        // HACK: removeClient also removes it from clients
        // TODO: thus it would be useful to distinguish removeClient
        //       with detachClient -> overwork around wmi-10
        removeClient(client);
        addClient(client); // we've to add it again, because it was removed
        client->setMode(Client::SLOT);
        attachClient(client);
    }
}

void Monitor::toggleSlot() {

    if (slot_->isVisible()) {
        focused()->setSlotVisible(false);
        slot_->hide();
    }
    else {
        focused()->setSlotVisible(true);
        slot_->show(focused()->slotTabName());
        slot_->manage();
    }

    focused()->setSlotVisible(slot_->isVisible());
}

void Monitor::cycleSlotTabPrev() {
    slot_->focused()->hide();
    slot_->focus(slot_->prev());
    slot_->focused()->show();
    slot_->manage();
}

void Monitor::cycleSlotTabNext() {
    slot_->focused()->hide();
    slot_->focus(slot_->next());
    slot_->focused()->show();
    slot_->manage();
}
#endif // SLOT_SUPPORT

Client *Monitor::clientForWindow(Window window) {

    if (window == 0 || window == rootWindow_) {
        return 0;
    }

    MClient::iterator it = clients_.find((long)window);

    if (it != clients_.end()) {
        return (*it).second;
    }

    // client does not exist
    return 0;
}

void Monitor::addClient(Client *client) {

    clients_[(long)client->clientWindow()] = client;
}

void Monitor::cleanup() {
    while (clients_.size()) {
        MClient::iterator it = clients_.begin();
        Client *client = (*it).second;
        removeClient(client);
        if (!client->transient()) {
            XCORE->addToSaveSet(client->clientWindow());
        }
        delete client;
    }
}

Client *Monitor::focusedClient() {
    return focused()->topClient();
}

void Monitor::removeClient(Client *client) {

    XCORE->sync();
    LOGDEBUG("removing client");

    Workspace *workspace = client->attached();
    if (workspace) {
        workspace->detachClient(client);
        LOGDEBUG("client removed from workspace");
    }
#ifdef SLOT_SUPPORT
    else if (client->mode() == Client::SLOT) {
        LOGDEBUG("removing slot client");
        slot_->detachClient(client);
        matchWorkspace(focused(), slot_->alignment());
    }
#endif
    else if (!client->isVisible()) {
        // seems to be an detached client
        detachedClients_.detach(client);
        XCORE->sync();
        LOGDEBUG("removing detached client");
        statusBar_->illuminate();
    }
    // else already detached
    XCORE->sync();
    clients_.erase((long)client->clientWindow());
    LOGDEBUG("client removed");
}

void Monitor::matchWorkspace(Workspace *workspace, Direction dir) {

    rectForWorkspace(workspace);
    workspace->matchBarNeighbors(dir);
#ifdef SLOT_SUPPORT
    // slot fix
    slot_->setY(workspace->y());
#endif // SLOT_SUPPORT
}

void Monitor::toggleClientBar() {

    if (clientBar_->isVisible()) {
        clientBar_->hide();
    }
    else {
        clientBar_->show();
    }

    focused()->setClientBarVisible(clientBar_->isVisible());
    Direction dir =
        (Util::get(commonSettings_, "statusbar.alignment") == "bottom") ? UP : DOWN;
    matchWorkspace(focused(), dir);
#ifdef SLOT_SUPPORT
    slot_->manage();
#endif // SLOT_SUPPORT
}

void Monitor::toggleStatusBar() {

    if (statusBar_->isVisible()) {
        statusBar_->hide();
    }
    else {
        statusBar_->show();
    }

    focused()->setStatusBarVisible(statusBar_->isVisible());
    Direction dir =
        (Util::get(commonSettings_, "statusbar.alignment") != "bottom") ? UP : DOWN;
    matchWorkspace(focused(), dir);
#ifdef SLOT_SUPPORT
    slot_->manage();
#endif // SLOT_SUPPORT
}

void Monitor::toggleAllClientStates() {

    Workspace *workspace = focused();
    assert(workspace);
    Thing *thing = workspace->focusedThing();

    if (!thing || (thing->type() != Thing::FRAME)) {
        return;
    }
    Frame *frame = (Frame *)thing;

    unsigned int num = frame->size();
    for (unsigned int i = 0; (i < num) && (frame->size() != 0); i++) {
        workspace->changeClientMode(frame->focused(), Client::FLOAT);
    }
    statusBar_->illuminate();
}

void Monitor::attachAllClients() {

    while (detachedClients_.size()) {
        Client *client = *detachedClients_.begin();
        detachedClients_.detach(client);
        attachClient(client);
    }
}

void Monitor::detachAllClients() {

    Workspace *workspace = focused();
    assert(workspace);

    if (workspace->isFrameMode()) {
        Frame *frame = workspace->focusedFrame();
        unsigned int i = frame->size();
        while (i) {
            detachClient(*frame->begin());
            i--;
        }
    }
    else {
        CClient *floatingClients = workspace->floatingClients();
        unsigned int i = floatingClients->size();
        while (i) {
            detachClient(*floatingClients->begin());
            i--;
        }
    }
    statusBar_->illuminate();
}

void Monitor::detachClient(Client *client) {

    if (!client) {
        return;
    }

    Workspace *workspace = client->attached();

    if (workspace) {
        workspace->detachClient(client);
        detachedClients_.attach(client);
    }
}

void Monitor::detachClient() {
    detachClient(focused()->topClient());
}

void Monitor::attachClientByName(string name) {

    for (LClient::iterator it = detachedClients_.begin();
            it != detachedClients_.end(); it++)
    {
        Client *client = *it;
        if (client->name() == name) {
            detachedClients_.detach(client);
            attachClient(client);
            return;
        }
    }
}

void Monitor::attachLastClient() {

    Client *client = detachedClients_.focused();
    if (client) {
        detachedClients_.detach(client);
        attachClient(client);
    }
}

void Monitor::attachClient(Client *client) {

    // first we've to init all necessary info
    client->initICCCM();

    if (client->state() == IconicState) {
        assert(!client->attached());
        detachedClients_.attach(client);
        return;
    }

    // first transient handling
    Client *transient = client->transient();

    if (transient && !transient->attached()
#ifdef SLOT_SUPPORT
        && (transient->mode() != Client::SLOT)
#endif // SLOT_SUPPORT
        )
    {
        // attach also the transient, if some
        detachedClients_.detach(transient);
        attachClient(transient);
    }

#ifdef SLOT_SUPPORT
    if (client->mode() == Client::SLOT) {
        // will be handled by the slot
        LOGDEBUG("new slot client");
        slot_->attachClient(client);
    }
    else
#endif // SLOT_SUPPORT
    {
        Workspace *hooked = workspaceForName(client->hooked());
        if (hooked && contains(hooked)) {
            focus(hooked);
        }
        focused()->attachClient(client);
    }
}

void Monitor::attachDetachedClient(Workspace *workspace, Client *client) {

    focus(workspace);
    detachedClients_.detach(client);
    attachClient(client);
}

void Monitor::createNewWorkspace(string name) {

    static Rectangle rect;
    unsigned int id = size();

    ostringstream key;
    key << prefix_ << ".workspace[" << id << "]";

    (*sessionSettings_)[key.str() + ".name"] = name;
    (*sessionSettings_)[key.str() + ".clientbar"] = "yes";
    (*sessionSettings_)[key.str() + ".statusbar"] = "yes";
#ifdef SLOT_SUPPORT
    (*sessionSettings_)[key.str() + ".slot"] = slot_->tabName();
#endif // SLOT_SUPPORT
    (*sessionSettings_)[key.str() + ".focused"] = "0";

    if (focused()) {
        focused()->hide();
    }
    Workspace *workspace = new Workspace(this , id, this);
    matchWorkspace(workspace, LEFT);
    attach(workspace);
    focused()->show();

    ostringstream value;
    value << size();
    (*sessionSettings_)[prefix_ + ".workspaces"] = value.str();
}

void Monitor::serialize() {

    ostringstream value;
    value << size();
    (*sessionSettings_)[prefix_ + ".workspaces"] = value.str();

    for (LWorkspace::iterator it = begin(); it != end(); it++) {
         Workspace *workspace = *it;
         workspace->serialize();
    }

    value.str("");
    value << focused()->id();
    (*sessionSettings_)[prefix_ + ".focused"] = value.str();
}

void Monitor::focus(Workspace *workspace) {

    workspace->setRequestsFocus(false);
    if (focused() != workspace) {
        focused()->hide();
        LOGDEBUG("focussing workspace");
        XCORE->sync();
        Container<Display, LWorkspace, LWorkspace::iterator,
            Workspace>::focus(workspace);
    }
    focused()->show();
    updateBars();
}

void Monitor::focusClient(Client *client) {

    Workspace *workspace = client->attached();
    if (workspace) {
        focus(workspace);
        workspace->focus(client);
    }
}

void Monitor::focusClientById(string id) {

    for (MClient::iterator it = clients_.begin();
            it != clients_.end(); it++)
    {
        Client *client = (*it).second;

        if (client->id() == id) {
            focusClient(client);
        }
    }
}

void Monitor::focusClientByName(string name) {

    for (MClient::iterator it = clients_.begin();
            it != clients_.end(); it++)
    {
        Client *client = (*it).second;

        if (client->name() == name) {
            focusClient(client);
        }
    }
}

Workspace *Monitor::workspaceForName(string name) {

    for (LWorkspace::iterator it = begin(); it != end(); it++) {
        if ((*it)->name() == name) {
            return (Workspace *)*it;
        }
    }
    return 0;
}

void Monitor::focusWorkspaceNum(unsigned int workspaceNum) {

    unsigned int i = 0;
    for (LWorkspace::iterator it = begin(); it != end(); it++) {
        if (i == workspaceNum) {
            focus(*it);
            return;
        }
        i++;
    }
}

void Monitor::destroyWorkspace(Workspace *workspace) {

    assert(workspace);
    if (size() > 1) {
        LClient *clients = workspace->clients();
        while (clients->size()) {
            Client *client = *clients->begin();
            detachClient(client);
            KERNEL->killClient(client);
        }
        detach(workspace);
        Util::remove(sessionSettings_, workspace->prefix());
        delete workspace;
        focused()->show();
        updateBars();
    }
}

void Monitor::renameWorkspace(Workspace *workspace, string newName) {
    workspace->setName(newName);
    (*sessionSettings_)[workspace->prefix() + ".name"] = newName;
}

void Monitor::updateBars() {

    static string value;

    if (XCORE->textProperty(rootWindow_, Atoms::WMI_STATUSTEXT, &value)) {
        statusBar_->setText(value);
    }
    if (XCORE->textProperty(rootWindow_, Atoms::WMI_METERTEXT, &value)) {
        statusBar_->setMeterText(value);
    }

    LOGDEBUG("update status");
    statusBar_->illuminate();
    clientBar_->illuminate();
    XCORE->sync();
}

void Monitor::toggleThingMaximization() {

    if (!focused()) {
        return;
    }

    if (maxThing_) {

        maxThing_->copy(maxThing_->prevRectangle());
        maxThing_->resize();
        focused()->illuminate();
        maxThing_ = 0; // reset

    }
    else {

        Workspace *workspace = focused();
        maxThing_ = workspace->focusedThing();

        if (!maxThing_) {
            return;
        }

        maxThing_->prevRectangle()->copy(maxThing_);
        maxThing_->setX(x());
        maxThing_->setY(y());
        maxThing_->setWidth(width());
        maxThing_->setHeight(height());
        maxThing_->resize();
        XCORE->raise(maxThing_->window());
        maxThing_->illuminate();
    }
}

void Monitor::illuminateTransRect(Rectangle *rect, unsigned int barHeight)
{
    Draw::drawTransRectangle(rootWindow_, pseudoBorderGC_, rect, barHeight,
                             KERNEL->borderWidth());

    // additional size info
    ostringstream oss;
    oss << "x=" << rect->x()  << ", y=" << rect->y()
        << ": " << rect->width() << "x" << rect->height();

    illuminateTransText(rect, oss.str());
}

unsigned int Monitor::nextClientId() {
    ++clientIds_;
    return clientIds_;
}

void Monitor::illuminateTransText(Rectangle *rect, string text)
{
    WFont *font = (font_->type() == WFont::NORMAL) ?
                    font_ : fixed_;

    // additional size info
    int textWidth = font->textWidth(text);
    unsigned int fontBaseLine = font->ascent();
    unsigned int fontY = rect->y() + rect->height() / 2
                - font->height() / 2 + fontBaseLine;

    unsigned int fontX = rect->x() +
                (rect->width() / 2 - textWidth / 2);
    font->drawText(rootWindow_, pseudoBorderGC_,
                    fontX, fontY, text);
}


unsigned int Monitor::id() const {
    return id_;
}

Window Monitor::rootWindow() const {
    return rootWindow_;
}

string Monitor::displayString() const {
    return displayString_;
}

Theme *Monitor::theme() const {
    return theme_;
}

unsigned int Monitor::resizeFactor() const {
    return resizeFactor_;
}

string Monitor::prefix() const {
    return prefix_;
}

Box *Monitor::box() const {
    return box_;
}

StatusBar *Monitor::statusBar() const {
    return statusBar_;
}

ClientBar *Monitor::clientBar() const {
    return clientBar_;
}

CClient *Monitor::detachedClients() {
    return &detachedClients_;
}

bool Monitor::isThingMaximized() {
    return maxThing_ != 0;
}

GC Monitor::borderGC() {
    return borderGC_;
}

CClient *Monitor::stickyClients() {
    return &stickyClients_;
}

WFont *Monitor::font() const {
    return font_;
}

unsigned int Monitor::titleBarHeight() const {
    return font_->height() + 2;
}

MClient *Monitor::clients() {
    return &clients_;
}

InputBar *Monitor::inputBar() const {
    return inputBar_;
}

void Monitor::banish() {
    XCORE->warpPointer(rootWindow_, width(), height());
}

unsigned int Monitor::buttonWidth() const {
    return font_->height() + 4;
}

void Monitor::hookClient() {

    Client *client = 0;
#ifdef SLOT_SUPPORT
    if (slot_->isGrabFocus() ||
        (!focused()->clients()->size() && slot_->focused()->focused()))
    {
        client = slot_->focused()->focused();
        if (client) {
            client->setHooked(slot_->tabName());
            return;
        }
    }
    else
#endif // SLOT_SUPPORT 
    {
        client = focused()->topClient();
        if (client) {
            client->setHooked(focused()->name());
        }
    }
}

void Monitor::unhookClient() {

    Client *client = 0;
#ifdef SLOT_SUPPORT
    if (slot_->isGrabFocus() ||
        (!focused()->clients()->size() && slot_->focused()->focused()))
    {
        client = slot_->focused()->focused();
        if (client) {
            client->setHooked("");
            return;
        }
    }
    else
#endif // SLOT_SUPPORT
    {
        client = focused()->topClient();
        if (client) {
            client->setHooked("");
        }
    }
}
