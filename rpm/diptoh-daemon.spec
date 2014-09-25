#
# diptoh-daemon spec
# (C) kimmoli 2014
#

Name: harbour-diptoh-daemon

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary: Diptoh OtherHalf daemon
Version: 0.2
Release: 1
Group: Qt/Qt
License: LICENSE
URL: https://github.com/kimmoli/diptoh-daemon
Source0: %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5DBus)

Requires:   ambienced

%description
Daemon for Diptoh OtherHalf

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 SPECVERSION=%{version}

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

%files
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/%{name}
%config /etc/systemd/system/%{name}.service
%config /etc/udev/rules.d/95-%{name}.rules
%config(noreplace) %attr(755,root,root) /home/nemo/diptoh/dip*.sh
%config(noreplace) %attr(755,root,root) /home/nemo/diptoh/button*.sh
%{_datadir}/ambience/%{name}
%{_datadir}/ambience/%{name}/%{name}.ambience
%{_datadir}/ambience/%{name}/images/*

%post
#reload udev rules
udevadm control --reload
# if diptoh is connected, start daemon now
if [ -e /sys/devices/platform/toh-core.0/vendor ]; then
 if grep -q 6537 /sys/devices/platform/toh-core.0/vendor ; then
  if grep -q 2 /sys/devices/platform/toh-core.0/product ; then
   systemctl start %{name}.service
  fi
 fi
fi
%_ambience_post

%pre
# In case of update, stop and disable first
if [ "$1" = "2" ]; then
  systemctl stop %{name}.service
  systemctl disable %{name}.service
  udevadm control --reload
fi

%preun
# in case of complete removal, stop and disable
if [ "$1" = "0" ]; then
  systemctl stop %{name}.service
  systemctl disable %{name}.service
  udevadm control --reload
fi
