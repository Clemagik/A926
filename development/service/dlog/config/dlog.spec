Name:       dlog
Summary:    Logging service
Version:    0.4.1
Release:    15
Group:      System/Libraries
License:    Apache License, Version 2.0
Source0:    %{name}-%{version}.tar.gz
Source101:  packaging/dlogutil.manifest
Source102:  packaging/libdlog.manifest
Source201:  packaging/dlog.conf.in
Source202:  packaging/dlog_logger.conf.in
Source203:  packaging/dlog_logger.conf-micro.in
Source204:  packaging/dlog_logger.conf-micro-debug.in
Source301:  packaging/dlog_logger.service
BuildRequires: autoconf
BuildRequires: automake
BuildRequires: libtool
BuildRequires: pkgconfig(libsystemd-journal)
BuildRequires: pkgconfig(capi-base-common)
Requires(post): coreutils

%description
dlog API library

%package -n libdlog
Summary:    Logging service dlog API
Group:      Development/Libraries
Requires(post): smack-utils

%description -n libdlog
dlog API library

%package -n libdlog-devel
Summary:    Logging service dlog API
Group:      Development/Libraries
Requires:   lib%{name} = %{?epoch:%{epoch}:}%{version}-%{release}

%description -n libdlog-devel
dlog API library


%package -n dlogutil
Summary:    print log data to the screen
Group:      Development/Libraries
Requires:   lib%{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Requires(post): /usr/bin/systemctl
Requires(postun): /usr/bin/systemctl
Requires(preun): /usr/bin/systemctl

%description -n dlogutil
Utilities for print log data

%prep
%setup -q

%build
cp %{SOURCE101} .
cp %{SOURCE102} .
%autogen --disable-static
%configure --disable-static \
%if 0%{?tizen_build_binary_release_type_daily}
			--enable-fatal_on \
%endif
%if 0%{?tizen_build_binary_release_type_eng}
			--enable-engineer_mode \
%endif
%if 0%{?sec_build_binary_debug_enable}
			--enable-debug_enable \
%endif
			--without-systemd-journal
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/usr/bin/
cp %{_builddir}/%{name}-%{version}/scripts/dlogctrl %{buildroot}/usr/bin/dlogctrl

mkdir -p %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants/
install -m 0644 %SOURCE301 %{buildroot}%{_libdir}/systemd/system/

ln -s ../dlog_logger.service %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants/dlog_logger.service

mkdir -p %{buildroot}/usr/share/license
cp LICENSE.Apache-2.0 %{buildroot}/usr/share/license/%{name}
cp LICENSE.Apache-2.0 %{buildroot}/usr/share/license/libdlog
cp LICENSE.Apache-2.0 %{buildroot}/usr/share/license/dlogutil

mkdir -p %{buildroot}/opt/etc
cp %SOURCE201 %{buildroot}/opt/etc/dlog.conf

# default set log output to external files
cp %SOURCE202 %{buildroot}/opt/etc/dlog_logger.conf

%preun -n dlogutil

%post -n dlogutil
systemctl daemon-reload

%postun -n dlogutil
systemctl daemon-reload

%post -n libdlog
/sbin/ldconfig

%postun -n libdlog
/sbin/ldconfig

%files
/usr/share/license/%{name}

%files  -n dlogutil
%manifest dlogutil.manifest
/usr/share/license/dlogutil
%attr(755,root,app_logging) %{_bindir}/dlog_logger
%attr(755,root,app_logging) %{_bindir}/dlogutil
%attr(755,root,app_logging) %{_bindir}/dlogctrl
%attr(755,root,app_logging) /opt/etc/dlog_logger.conf
%{_libdir}/systemd/system/dlog_logger.service
%{_libdir}/systemd/system/multi-user.target.wants/dlog_logger.service


%files  -n libdlog
%manifest libdlog.manifest
/usr/share/license/libdlog
%{_libdir}/libdlog.so.0
%{_libdir}/libdlog.so.0.0.0
%attr(664,root,app_logging) /opt/etc/dlog.conf

%files -n libdlog-devel
%{_includedir}/dlog/dlog.h
%{_libdir}/pkgconfig/dlog.pc
%{_libdir}/libdlog.so

