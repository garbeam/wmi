# wmi - window manager improved
# See LICENSE file for copyright and license details.

include config.mk

SRC_wmi = action.cpp \
	actions.cpp \
	atoms.cpp \
	bar.cpp \
	binder.cpp \
	box.cpp \
	clientbar.cpp \
	client.cpp \
	cursors.cpp \
	draw.cpp \
	expander.cpp \
	float.cpp \
	font.cpp \
	frame.cpp \
	inputbar.cpp \
	kernel.cpp \
	label.cpp \
	launcher.cpp \
	loader.cpp \
	logger.cpp \
	main.cpp \
	menu.cpp \
	monitor.cpp \
	prompt.cpp \
	rectangle.cpp \
	shortcut.cpp \
	slot.cpp \
	split.cpp \
	statusbar.cpp \
	theme.cpp \
	thing.cpp \
	tree.cpp \
	util.cpp \
	validators.cpp \
	widget.cpp \
	workspace.cpp \
	xcore.cpp \
	xfont.cpp \
	xftfont.cpp

SRC_wmiremote = wmiremote.cpp

OBJ_wmi = ${SRC_wmi:.cpp=.o}
OBJ_wmiremote = ${SRC_wmiremote:.cpp=.o}

all: options wmi wmiremote

options:
	@echo wmi build options:
	@echo "CXXFLAGS = ${CXXFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CXX      = ${CXX}"

.cpp.o:
	@echo CXX $<
	@${CXX} -c ${CXXFLAGS} $<

${OBJ_wmi}: config.mk

${OBJ_wmiremote}: config.mk

wmi: ${OBJ_wmi}
	@echo CXX -o $@
	@${CXX} -o $@ ${OBJ_wmi} ${LDFLAGS}

wmiremote: ${OBJ_wmiremote}
	@echo CC -o $@
	@${CXX} -o $@ ${OBJ_wmiremote} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f wmi wmiremote ${OBJ_wmi} ${OBJ_wmiremote} wmi-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p wmi-${VERSION}
	@cp -R LICENSE Makefile README config.def.h config.mk \
		wmi.1 ${SRC} wmi-${VERSION}
	@tar -cf wmi-${VERSION}.tar wmi-${VERSION}
	@gzip wmi-${VERSION}.tar
	@rm -rf wmi-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f wmi ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/wmi
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < wmi.1 > ${DESTDIR}${MANPREFIX}/man1/wmi.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/wmi.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/wmi
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/wmi.1

.PHONY: all options clean dist install uninstall
