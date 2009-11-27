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
	@rm -f wmi wmiremote ${OBJ_wmi} ${OBJ_wmiremote} wmi-${WMI_VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p wmi-${WMI_VERSION}
	@cp -R AUTHORS CONTRIB ChangeLog FAQ INSTALL LICENSE Makefile \
		config.mk examples man *.cpp *.h wmi-${WMI_VERSION}
	@tar -cf wmi-${WMI_VERSION}.tar wmi-${WMI_VERSION}
	@gzip wmi-${WMI_VERSION}.tar
	@rm -rf wmi-${WMI_VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f wmi ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/wmi
	@cp -f wmiremote ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/wmiremote
	@echo installing manual pages to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@cp -f man/*.1 ${DESTDIR}${MANPREFIX}/man1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/wmi.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/wmiremote.1
	@echo installing manual pages to ${DESTDIR}${MANPREFIX}/man5
	@mkdir -p ${DESTDIR}${MANPREFIX}/man5
	@cp -f man/*.5 ${DESTDIR}${MANPREFIX}/man5
	@chmod 644 ${DESTDIR}${MANPREFIX}/man5/actions.conf.5
	@chmod 644 ${DESTDIR}${MANPREFIX}/man5/common.conf.5
	@chmod 644 ${DESTDIR}${MANPREFIX}/man5/session.conf.5
	@chmod 644 ${DESTDIR}${MANPREFIX}/man5/theme.conf.5

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/wmi
	@rm -f ${DESTDIR}${PREFIX}/bin/wmiremote
	@echo removing manual pages from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/wmi.1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/wmi.1
	@echo removing manual pages from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man5/actions.conf.5
	@rm -f ${DESTDIR}${MANPREFIX}/man5/common.conf.5
	@rm -f ${DESTDIR}${MANPREFIX}/man5/session.conf.5
	@rm -f ${DESTDIR}${MANPREFIX}/man5/theme.conf.5

.PHONY: all options clean dist install uninstall
