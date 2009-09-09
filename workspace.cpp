// See ../LICENSE.txt for license details.
//
// $Id: workspace.cpp 739 2004-09-28 07:17:12Z garbeam $

extern "C" {
#include <assert.h>
#include <unistd.h>
#include <X11/Xlib.h>
}

#include <map>
#include <sstream>

#include "workspace.h"

#include "binder.h"
#include "clientbar.h"
#include "float.h"
#include "frame.h"
#include "kernel.h"
#include "logger.h"
#include "kernel.h"
#include "monitor.h"
#include "slot.h"
#include "split.h"
#include "statusbar.h"
#include "thing.h"
#include "tree.h"
#include "util.h"
#include "xcore.h"

Workspace::Workspace(Monitor *monitor, unsigned int id, Rectangle *rect)
    : Rectangle(*rect), floatingClients_(monitor), frames_(monitor)
{
    id_ = id;
    root_ = new Tree(0, this);
    isFrameMode_ = true;
    requestsFocus_ = false;
    frameIds_ = 0;
    monitor_ = monitor;

    ostringstream oss;
    oss << monitor->prefix() << ".workspace[" << id_ << "]";
    prefix_ = oss.str().c_str();

    isClientBarVisible_ = (Util::get(KERNEL->sessionSettings(),
                          prefix_ + ".clientbar") == "yes");
    isStatusBarVisible_ = (Util::get(KERNEL->sessionSettings(),
                           prefix_ + ".statusbar") == "yes");
#ifdef SLOT_SUPPORT
    slotTabName_ = Util::get(KERNEL->sessionSettings(), prefix_ + ".slot");
#endif // SLOT_SUPPORT

    name_ = Util::get(KERNEL->sessionSettings(), prefix_ + ".name");

    oss.str("");
    oss << "workspace my dim: " << x() << ", " << y() << ", " << width()
        << ", " << height();
    LOGDEBUG(oss.str());
    XCORE->sync();
}

Workspace::~Workspace() {
    delete root_;
}

void Workspace::focus(Thing *thing, bool raise) {

    if (!thing) {
        XCORE->setInputFocus(PointerRoot);
        illuminate();
        return;
    }

    Client *client = 0;
    Frame *frame = 0;
    if (thing->type() == Thing::CLIENT) {
        client = (Client *)thing;
        if (client->frame()) {
            LOGDEBUG("client has frame");
            frame = client->frame();
            frame->focus(client);
        }
    }
    else {
        frame = (Frame *)thing;
    }

    if (frame) {
        LOGDEBUG("focussing frame");
        if (raise) {
            for (LFrame::iterator it = frames_.begin();
                    it != frames_.end(); it++)
            {
                XCORE->raise(((Frame *)*it)->window());
            }
            XCORE->raise(frame->window());
        }
        frames_.focus(frame);
        client = frame->focused();
    }

    if (client) {
        // grabbing/ungrabbing of the button
        Client *tc = topClient();
        if (tc && tc != client) {
            Binder::instance()->ungrabButtons(tc->clientWindow());
            Binder::instance()->grabButtons(tc->clientWindow(), AnyModifier);
        }

        LOGDEBUG("focussing client");
        client->setRequestsFocus(false);
#ifdef SLOT_SUPPORT
        attached()->slot()->setGrabFocus(false);
#endif

        Binder::instance()->ungrabButtons(client->clientWindow());

        removePushClient(client);
        if (!frame) {
            floatingClients_.focus(client);
        }

        if (raise) {
            XCORE->raise(client->window());
        }
#ifdef SLOT_SUPPORT
        static bool isSlotOverlap =
            Util::get(KERNEL->commonSettings(), "slot.mode") == "overlap";
        if (isSlotOverlap || (isSlotVisible()
            && (isFrameMode_ != (bool)frame))) {
            XCORE->raise(monitor_->slot()->window());
        }
#endif // SLOT_SUPPORT
        XCORE->setInputFocus(client->clientWindow());
    }
    isFrameMode_ = frame;
    illuminate();
}

void Workspace::hide() {

    for (LClient::iterator it = attached()->stickyClients()->begin();
         it != attached()->stickyClients()->end(); it++)
    {
        Client *client = *it;
        client->setAttached(0);
        floatingClients_.detach(client);
    }
    for (LClient::iterator it = floatingClients_.begin();
         it != floatingClients_.end(); it++)
    {
        (*it)->hide();
    }

    for (LFrame::iterator it = frames_.begin();
         it != frames_.end(); it++)
    {
        (*it)->hide();
    }
}

void Workspace::show() {

    for (LClient::iterator it = attached()->stickyClients()->begin();
         it != attached()->stickyClients()->end(); it++)
    {
        Client *client = *it;
        client->setAttached(this);
        floatingClients_.attach(client);
    }

    for (LClient::iterator it = floatingClients_.begin();
         it != floatingClients_.end(); it++)
    {
        (*it)->show();
    }

    for (LFrame::iterator it = frames_.begin();
         it != frames_.end(); it++)
    {
        (*it)->show();
    }

    focus(topClient());

    if (isClientBarVisible_) {
        attached()->clientBar()->show();
    }
    else {
        attached()->clientBar()->hide();
    }
    if (isStatusBarVisible_) {
        attached()->statusBar()->show();
    }
    else {
        attached()->statusBar()->hide();
    }
#ifdef SLOT_SUPPORT
    Slot *slot = attached()->slot();
    if (isSlotVisible()) {
        bool doManage = (slotTabName_ != slot->tabName());
        slot->show(slotTabName_);
        if (doManage) {
            slot->manage();
        }
    }
    else {
        slot->hide();
    }
    slotTabName_ = slot->isVisible() ?  slot->tabName() : "";
#endif // SLOT_SUPPORT
}

void Workspace::illuminate() {

    if (attached()->focused() != this) {
        return;
    }

    for (LClient::iterator it = floatingClients_.begin();
         it != floatingClients_.end(); it++)
    {
        (*it)->illuminate();
    }

    LOGDEBUG("floating clients illuminated, going to illuminate frames");
    for (LFrame::iterator it = frames_.begin();
         it != frames_.end(); it++)
    {
        (*it)->illuminate();
    }
    attached()->updateBars();
}

void Workspace::swapFrame(Direction dir) {

    Frame *frame1 = focusedFrame();
    Frame *frame2 = nextFrame(dir);
    if (!frame1 || !frame2) {
        return;
    }

    Tree *tree1 = frame1->tree();
    Tree *tree2 = frame2->tree();

    tree1->setFrame(frame2);
    tree2->setFrame(frame1);
    frame2->setTree(tree1);
    frame1->setTree(tree2);
    Split::adjustSize(tree1, ((dir == LEFT) || (dir == RIGHT)));
    Split::adjustSize(tree2, ((dir == LEFT) || (dir == RIGHT)));
}

void Workspace::swapClient(Direction dir) {

    Frame *frame1 = focusedFrame();
    if (!frame1) {
        return;
    }

    Frame *frame2 = nextFrame(dir);

    if (frame1 && frame2) {

        Client *client1 = frame1->focused();
        Client *client2 = frame2->focused();

        frame1->detach(client1);
        frame2->detach(client2);
        frame2->attach(client1);
        frame1->attach(client2);
        frame1->illuminate();
        frame2->illuminate();
    }
}

Tree *Workspace::neighborTree(Direction dir) {

    if (focusedFrame()) {
        return Split::neighbor(root_, focusedFrame()->tree(), dir);
    }
    return 0;
}

#define EPSILON 3

Frame *Workspace::recentVisitedFrame(Direction dir) {

    if (!focusedFrame()) {
        return 0;
    }
    Frame *focusedFrame = this->focusedFrame();
    Tree *focusedTree = focusedFrame->tree();
    // This algorithm is somewhat magic, because it uses the
    // fact of the state of the globalClientStack_. 
    for (LClient::iterator it = globalClientStack_.begin();
            it != globalClientStack_.end(); it++)
    {
        Client *client = *it;
        Frame *frame = client->frame();
        if (frame && (frame != focusedFrame)) {

            Tree *tree = frame->tree();
            if (Split::isNeighbor(focusedTree, tree, dir)) {
                bool isIgnore = false;
                switch (dir) {
                case LEFT:
                    isIgnore =
                        (int)(tree->x() + tree->width() + EPSILON) <
                        focusedTree->x();
                    isIgnore = isIgnore || (focusedTree->y() > tree->y()) ||
                        (focusedTree->height() < tree->height());
                    break;
                case RIGHT:
                    isIgnore =
                        (int)(focusedTree->x() + focusedTree->width() +
                        EPSILON) < tree->x();
                    isIgnore = isIgnore || (focusedTree->y() > tree->y()) ||
                        (focusedTree->height() < tree->height());
                    break;
                case UP:
                    isIgnore =
                        (int)(tree->y() + tree->height() + EPSILON) <
                        focusedTree->y();
                    isIgnore = isIgnore || (focusedTree->x() > tree->x()) ||
                        (focusedTree->width() < tree->width());
                    break;
                case DOWN:
                    isIgnore =
                        (int)(focusedTree->y() + focusedTree->height() +
                        EPSILON) < tree->y();
                    isIgnore = isIgnore || (focusedTree->x() > tree->x()) ||
                        (focusedTree->width() < tree->width());
                    break;
                default:
                    break;
                }
                if (!isIgnore) {
                    return frame;
                }
            }
        }
    }
    return 0;
}

Frame *Workspace::nextFrame(Direction dir, bool isResize) {

    Tree *neighbor = neighborTree(dir);

    if (neighbor && isResize) { // normal case
        return Split::firstLeaf(neighbor);
    }
    else if (neighbor) {

        Frame *result = Split::firstLeaf(neighbor);
        Frame *recent = recentVisitedFrame(dir);

        if (recent && recent != result && recent != focusedFrame()) {
            result = recent;
        }

        return result;
    }

    return 0;
}

void Workspace::selectFrame(Direction dir) {

    Frame *next = nextFrame(dir, false);

    if (next) {
        focus(next->focused());
    }
}

void Workspace::attachFrame(Frame *frame, Direction dir) {

    Tree *root = root_;

    if (focusedFrame()) {
        root = focusedFrame()->tree();
    }

    Split::attach(root, frame, dir);

    frames_.attach(frame);
    frame->show();
}

Frame *Workspace::newFrame() {

    assert((frames_.size() == 0));
    Frame *frame = new Frame(this, this);
    attachFrame(frame, LEFT); // direction doesn't matter

    return frame;
}

void Workspace::attachClient(Client *client, bool isChangeMode) {

    if (!client->frameWindow() && client->hasDecoration()) {
        LOGDEBUG("creating frame window for client");
        client->createFrame();
    }

    client->setAttached(this);
    if (client->mode() == Client::MAX) {
        // MAX mode is only allowed with decoration
        Frame *frame = 0;
        if (isChangeMode) {
            frame = frameForPoint(
                    client->x() + client->width() / 2,
                    client->y() + client->height() / 2);
        }
        else {
            frame = focusedFrame();
        }
        if (!frame) {
            frame = newFrame();
        }
        frame->attach(client);
        focus(frame->focused());
        return;
    }
    else if (client->mode() == Client::STICKY) {
        attached()->stickyClients()->attach(client);
    }

    for (LClient::iterator it = floatingClients_.begin();
            it != floatingClients_.end(); it++)
    {
        if (*it == client) {
            return;
        }
    }
    floatingClients_.attach(client);
    if (client->frameWindow()) {
        client->reparent(client->frameWindow(), client->x(), client->y());
    }
    XCORE->sync();
    Util::fitInto(client, attached());
    if (!isChangeMode) {
        if (client->isCentered()) {
            Float::center(client, this);
        }
    }
    client->resize();
    client->show();
    focus(client);
}

void Workspace::destroyFrame(Frame *frame) {

    unsigned int i = frame->size();
    while (i) {
        Client *client = *frame->begin();
        attached()->detachClient(client);
        KERNEL->killClient(client);
        i--;
    }
    assert(frame->size() < 1);
    frames_.detach(frame);
    Split::detach(root_, frame->tree());
    delete frame;
    LOGDEBUG("frame destroyed");
}

void Workspace::sendClient(Direction dir) {

    Frame *from = focusedFrame();
    Frame *to = nextFrame(dir);

    if (from && to) {
        Client *client = from->focused();
        if (!from->detach(client) &&
            (Util::get(KERNEL->commonSettings(), "frame.autodestroy") == "yes"))
        {
            destroyFrame(from);
        }
        client->initICCCM();
        to->attach(client);
        focus(to->focused());
    }
}

void Workspace::joinFrame(Direction dir) {

    Frame *from = focusedFrame();
    Frame *to = nextFrame(dir);

    if (from && to) {

        for (unsigned int i = from->size(); i > 0; i--) {
            Client *client = from->focused();
            if (from->detach(client) == 0) {
                destroyFrame(from);
            }
            client->initICCCM();
            to->attach(client);
        }
        focus(to->focused());
    }
}

void Workspace::splitFrame(Direction dir) {

    Frame *frame = focusedFrame();
    if (frame && frame->size() > 1) {
        Frame *splitFrame = new Frame(this, frame);
        LOGDEBUG("split frame");
        Client *client = frame->focused();
        frame->detach(client);
        attachFrame(splitFrame, dir);
        splitFrame->attach(client);
        focus(splitFrame->focused());
    }
}

void Workspace::resize(Thing *thing, Direction dir, bool grow) {

    switch (thing->type()) {
    case Thing::FRAME:
        resizeFrame(dir, grow);
        break;
    case Thing::CLIENT:
        if (((Client *)thing)->hasDecoration()) {
            resizeClient(dir, grow);
        }
        break;
    }
}

void Workspace::resizeFrame(Direction dir, bool grow) {

    Monitor *monitor = attached();
    int stepW = width() / 100 * monitor->resizeFactor();
    int stepH = height() / 100 * monitor->resizeFactor();

    if (!grow) {
        stepW *= -1;
        stepH *= -1;
        dir = Util::reverseDir(dir);
    }

    Split::resize(root_, focusedFrame()->tree(),
                  dir, stepW, stepH);
}

void Workspace::detachClient(Client *client) {

    // we need to remove all references to a sticky client on *all*
    // other workspaces first
    if (client->mode() == Client::STICKY) {
        for (LWorkspace::iterator it = attached()->begin();
                it != attached()->end(); it++)
        {
            Workspace *workspace = *it;
            if (workspace == this) {
                continue;
            }
            workspace->removeClient(client);
        }
        attached()->stickyClients()->detach(client);
    }

    Frame *frame = client->frame();
    if (frame) {
        if (!frame->detach(client) &&
            (Util::get(KERNEL->commonSettings(), "frame.autodestroy") == "yes"))
        {
            destroyFrame(frame);
        }
    }
    else {
        floatingClients_.detach(client);
        if (client->isVisible()) {
            client->hide();
        }
        client->reparent(attached()->rootWindow(),
                         client->x(), client->y());
    }
    LOGDEBUG("detached from containers");

    removeClient(client);

    client->setAttached(0);

    assert(client != topClient());
    if (topClient()) {
        focus(topClient());
    }
    else {
        illuminate();
    }
    LOGDEBUG("final stuff of detaching client");
}

Frame *Workspace::focusedFrame() const {
    return frames_.focused();
}

Thing *Workspace::thingWindow(Window window) {

    for (LClient::iterator it = floatingClients_.begin();
         it != floatingClients_.end(); it++)
    {
        Client *client = *it;
        if (client->frameWindow() == window) {
            return client;
        }
    }
    for (LFrame::iterator it = frames_.begin();
         it != frames_.end(); it++)
    {
        Frame *frame = *it;
        if (frame->window() == window) {
            return frame;
        }
    }
    return 0;
}

void Workspace::matchBarNeighbors(Direction dir) {

    // TODO: take care of dir
    // commits current workspace rectangle to root_ tree map element
    root_->copy(this);
    Split::adjustSize(root_, ((dir == LEFT) || (dir == RIGHT)));
}

void Workspace::lower() {

    Thing *thing = focusedThing();

    if (!thing) {
        return;
    }

    XCORE->lower(thing->window());
}

void Workspace::raise() {

    Client *client = topClient();

    if (!client) {
        return;
    }

    focus(client);
}

void Workspace::changeClientMode(Client *client, Client::Mode mode) {
    assert(client);
    detachClient(client);
    client->setMode(mode);
    attachClient(client, true);
}

void Workspace::toggleClientMode() {
    Client *client = topClient();
    if (client) {
        changeClientMode(client, (client->mode() == Client::MAX) ?
                                 Client::FLOAT : Client::MAX);
    }
}

void Workspace::toggleClientSticky() {
    Client *client = topClient();
    if (client) {
        if (client->mode() != Client::STICKY) {
            changeClientMode(client, Client::STICKY);
        }
        else {
            changeClientMode(client, Client::FLOAT);
        }
    }
}

void Workspace::resizeClient(Direction dir, bool grow) {

    Client *client = topClient();
    if (!client || !client->hasDecoration()) {
        return;
    }

    Monitor *monitor = attached();

    int stepW = width() / 100 * monitor->resizeFactor();
    int stepH = height() / 100 * monitor->resizeFactor();

    if ((dir == LEFT) || (dir == UP)) {
        stepW *= -1;
        stepH *= -1;
    }

    Float::resize(client, grow ? dir : Util::reverseDir(dir),
                  stepW, stepH);

    client->resize();
}

void Workspace::moveClient(Direction dir) {

    Client *client = topClient();
    if (!client) {
        return;
    }

    Monitor *monitor = attached();

    // review: correct
    int stepW = width() / 100 * monitor->resizeFactor();
    int stepH = height() / 100 * monitor->resizeFactor();

    if ((dir == LEFT) || (dir == UP)) {
        stepW *= -1;
        stepH *= -1;
    }

    if ((dir == LEFT) || (dir == RIGHT)) {
        Float::move(client, stepW, 0);
    }
    else {
        Float::move(client, 0, stepH);
    }

    client->resize();
}


void Workspace::serialize() {

    (*KERNEL->sessionSettings())[prefix_ + ".name"] = name_;
    (*KERNEL->sessionSettings())[prefix_ + ".clientbar"] =
        isClientBarVisible_ ? "yes" : "no";
    (*KERNEL->sessionSettings())[prefix_ + ".statusbar"] =
        isStatusBarVisible_ ? "yes" : "no";
#ifdef SLOT_SUPPORT
    (*KERNEL->sessionSettings())[prefix_ + ".slot"] = slotTabName_;
#endif // SLOT_SUPPORT
}

Frame *Workspace::frameForPoint(unsigned int x, unsigned int y) {

    for (LFrame::iterator it = frames_.begin();
         it != frames_.end(); it++)
    {
        Frame *frame = *it;
        if (Util::isPointWithinRect(x, y, frame, this)) {
            return frame;
        }
    }
    return focusedFrame();
}

void Workspace::toggleBorders(bool visible) {

    for (LFrame::iterator it = frames_.begin();
            it != frames_.end(); it++)
    {
        Frame *frame = *it;
        frame->setBorderWidth(visible ? KERNEL->borderWidth() : 0);
        frame->resize();
    }
    for (LClient::iterator it = floatingClients_.begin();
            it != floatingClients_.end(); it++)
    {
        Client *client = *it;
        if (client->hasDecoration()) {
            client->setBorderWidth(visible ? KERNEL->borderWidth() : 0);
            client->resize();
        }
    }
}

void Workspace::toggleBars(bool visible) {

    for (LFrame::iterator it = frames_.begin();
            it != frames_.end(); it++)
    {
        Frame *frame = *it;
        frame->setTitleBarHeight(visible ? attached()->titleBarHeight() : 0);
        frame->resize();
    }
    for (LClient::iterator it = floatingClients_.begin();
            it != floatingClients_.end(); it++)
    {
        Client *client = *it;
        if (client->hasDecoration()) {
            client->setTitleBarHeight(visible ? attached()->titleBarHeight() : 0);
            client->resize();
        }
    }
}

void Workspace::cycleClientNext() {

    if (isFrameMode_) {
        if (focusedFrame()) {
            focus(focusedFrame()->next());
        }
    }
    else {
        focus(floatingClients_.next());
    }
}

void Workspace::cycleClientPrev() {

    if (isFrameMode_) {
        if (focusedFrame()) {
            focus(focusedFrame()->prev());
        }
    }
    else {
        focus(floatingClients_.prev());
    }
}

void Workspace::toggleMode() {
    isFrameMode_ = !isFrameMode_;
    if (isFrameMode_) {
        if (focusedFrame()) {
            focus(focusedFrame()->focused());
        }
        else {
            focus(0);
        }
    }
    else {
        for (LClient::iterator it = floatingClients_.begin();
             it != floatingClients_.end(); it++)
        {
            XCORE->raise(((Client *)*it)->window());
        }
        focus(floatingClients_.focused());
    }
}

void Workspace::removePushClient(Client *client) {
    removeClient(client);
    pushClient(client);
}

void Workspace::removeClient(Client *client) {
    if (stackContainsClient(client)) {
        globalClientStack_.remove(client);
    }
}

void Workspace::pushClient(Client *client) {
    if (!stackContainsClient(client)) {
        globalClientStack_.push_front(client);
    }
}

bool Workspace::stackContainsClient(Client *client) {
    for (LClient::iterator it = globalClientStack_.begin();
            it != globalClientStack_.end(); it++)
    {
        if (client == *it) {
            return true;
        }
    }
    return false;
}

Client *Workspace::topClient() {

#ifdef SLOT_SUPPORT
    if (isSlotVisible() && attached()->slot()->isGrabFocus()) {
        // if the slot grabs the focus currently, this workspace
        // has temprarily no focussed topClient - so just return
        // 0.
        return 0;
    }
#endif
    if (globalClientStack_.size()) {
        return globalClientStack_.front();
    }
    return 0;
}

unsigned int Workspace::nextFrameId() {
    ++frameIds_;
    return frameIds_;
}

unsigned int Workspace::id() const {
    return id_;
}

void Workspace::setName(string name) {
    name_ = name;
}

string Workspace::name() const {
    return name_;
}

string Workspace::prefix() const {
    return prefix_;
}

Tree* Workspace::root() const {
    return root_;
}

bool Workspace::isFrameMode() const {
    return isFrameMode_;
}

void Workspace::setClientBarVisible(bool visible) {
    isClientBarVisible_ = visible;
}

void Workspace::setStatusBarVisible(bool visible) {
    isStatusBarVisible_ = visible;
}

#ifdef SLOT_SUPPORT
void Workspace::setSlotVisible(bool visible) {
    slotTabName_ = visible ?
        attached()->slot()->tabName() : "";
}

bool Workspace::isSlotVisible() const {
    return slotTabName_ != "";
}

string Workspace::slotTabName() const {
    return slotTabName_;
}
#endif // SLOT_SUPPORT

Thing *Workspace::focusedThing() {

    Client *client = topClient();
    return (client && client->frame()) ? (Thing *)client->frame() :
        (Thing *)client;
}

void Workspace::fitClient() {

    Client *client = topClient();
    if (client) {
        Util::fitInto(client, this);
        client->resize();
    }
}

bool Workspace::isClientBarVisible() const {
    return isClientBarVisible_;
}

bool Workspace::isStatusBarVisible() const {
    return isStatusBarVisible_;
}

Monitor *Workspace::attached() const {
    return monitor_;
}

CClient *Workspace::floatingClients() {
    return &floatingClients_;
}

CFrame *Workspace::frames() {
    return &frames_;
}

bool Workspace::requestsFocus() const {
    return requestsFocus_;
}

void Workspace::setRequestsFocus(bool requestsFocus) {
    requestsFocus_ = requestsFocus;
}

LClient *Workspace::clients() {
    return &globalClientStack_;
}

