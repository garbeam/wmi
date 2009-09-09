########################################################################
#
# How to build RPM packages using this wmi.spec:
#
#     a) Make sure you are *NOT* root.
#     b) "make dist"
#     c) "rpmbuild -ta wmi-$(VERSION).tar.gz"
#
# Your RPM build directory must already been set up for your non-root
# user, though:
#
#     r=$HOME/src/rpmbuild
#     mkdir -p $r/BUILD $r/RPMS/i386 $r/SOURCES $r/SPECS $r/SRPMS $r/tmp
#     echo -e "%_topdir $r\n%_tmppath %{_topdir}/tmp" >> ~/.rpmmacros
#
# You should also add something like
#
#     %packager Your Name <my@mail>
#     %vendor Your Name's RPM packages
#
# to ~/.rpmmacros so you can easily see where the generated package
# comes from.
#
########################################################################


Summary: Window Manager Improved, a X11 window manager for hackers
Name: wmi
Version: 10
License: MIT
Release: 2
Group: User Interface/Desktops
URL: http://wmi.berlios.de/
Source: wmi-10.tar.gz
BuildRoot: %{_tmppath}/root-%{name}-%{version}

# Build dependencies are missing on purpose. This spec file is
# intended to build RPM packages from source, regardless of the
# distribution. Therefore, you should be able to build it without RPM
# anyway.
#
# Distributions should roll their own spec file.
#
# Of course, if you know how to write spec files compatible across
# distributions, you're welcome to do so and remove this paragraph.


%description
WMI is a new lightweight window manager for X11, which attempts to
combine the best features of LarsWM, Ion, evilwm and ratpoison into 
one window manager.


%prep
%setup -n wmi-10


%build
%configure
%{__make} %{?_smp_mflags}

# Generate scripts to hook up WMI into gdm, xdm, and Co.
#
# Note: If you want to have these in non-RPM settings, you'll have to
#       create them in some Makefile. I suggest creating a
#       "packaging/" subdirectory and let its Makefile.am handle that.

echo -e "#!/bin/sh\nexec /etc/X11/xdm/Xsession wmi" > hook-gdm

echo -e "#!/bin/sh\nexec %{_bindir}/wmi" > hook-switchdesk

echo -e "[Desktop Entry]\nName=WMI Session\nComment=Use this session to run WMI as your desktop environment\nExec=wmi\nTryExec=wmi\nIcon=\nType=Application" > hook-xsessions


%install
%{__rm} -rf %{buildroot}

# Note: the makeinstall macro doesn't use DESTDIR correctly.
%{__make} DESTDIR=%{buildroot} install

# Install generated scripts to hook up WMI into gdm, xdm, and Co.

install -m 0755 -d %{buildroot}/etc/X11/gdm/Sessions
install -m 0755 hook-gdm %{buildroot}/etc/X11/gdm/Sessions/wmi

install -m 0755 -d %{buildroot}/usr/share/app/switchdesk
install -m 0755 hook-switchdesk %{buildroot}/usr/share/app/switchdesk/Xclients.wmi

install -m 0755 -d %{buildroot}/usr/share/xsessions
install -m 0644 hook-xsessions %{buildroot}/usr/share/xsessions/wmi.desktop


%clean
# It's easier to debug something when we haven't deleted it yet.
##${__rm} -rf ${buildroot}


%files
%defattr(-, root, root, 0755)
%doc AUTHORS ChangeLog BUGS CONTRIB FAQ INSTALL LICENSE.txt NEWS README TODO
%{_bindir}/wmi
%{_bindir}/wmiremote
%{_mandir}/man1/wmi.1*
%{_mandir}/man1/wmiremote.1*
%{_mandir}/man5/actions.conf.5*
%{_mandir}/man5/common.conf.5*
%{_mandir}/man5/session.conf.5*
%{_mandir}/man5/theme.conf.5*
/etc/X11/gdm/Sessions/wmi
/usr/share/app/switchdesk/Xclients.wmi
/usr/share/xsessions/wmi.desktop


%changelog

* Wed Jul 28 2004  <hun@n-dimensional.de> - 9.current-1
- initial version


# Local Variables:
# mode: rpm-spec
# End:
