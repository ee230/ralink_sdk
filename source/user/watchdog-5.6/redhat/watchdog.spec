Summary:          Software and/or Hardware watchdog daemon
Name:             watchdog
Version:          5.5
Release:          2%{?dist}
License:          GPL+
Group:            System Environment/Daemons

URL:              http://sourceforge.net/projects/watchdog/
Source0:          http://dl.sf.net/watchdog/watchdog-%{version}.tar.gz
Source1:          watchdog.init
Source2:          README.watchdog.ipmi
Source3:          README.Fedora

Patch0:           %{name}-%{version}-cleanup.patch
Patch1:           %{name}-%{version}-cleanup-nfs.patch

BuildRoot:        %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

Requires(post):   /sbin/chkconfig
Requires(postun): /sbin/chkconfig
Requires(post):   /sbin/service
Requires(postun): /sbin/service


%description
The watchdog program can be used as a powerful software watchdog daemon 
or may be alternately used with a hardware watchdog device such as the 
IPMI hardware watchdog driver interface to a resident Baseboard 
Management Controller (BMC).  watchdog periodically writes to /dev/watchdog; 
the interval between writes to /dev/watchdog is configurable through settings 
in the watchdog sysconfig file.  This configuration file is also used to 
set the watchdog to be used as a hardware watchdog instead of its default 
software watchdog operation.  In either case, if the device is open but not 
written to within the configured time period, the watchdog timer expiration 
will trigger a machine reboot. When operating as a software watchdog, the 
ability to reboot will depend on the state of the machine and interrupts.  
When operating as a hardware watchdog, the machine will experience a hard 
reset (or whatever action was configured to be taken upon watchdog timer 
expiration) initiated by the BMC.

 
%prep
%setup -q -n %{name}-%{version}

cp %{SOURCE2} .
cp %{SOURCE3} .

%patch0 -p1 -b .cleanup
%patch1 -p1 -b .cleanup-nfs

mv README README.orig
iconv -f ISO-8859-1 -t UTF-8 < README.orig > README


%build
%configure 
make %{?_smp_mflags}


%install
rm -Rf ${RPM_BUILD_ROOT}
install -d -m0755 ${RPM_BUILD_ROOT}%{_sysconfdir}
make DESTDIR=${RPM_BUILD_ROOT} install
install -Dp -m0644 %{name}.sysconfig ${RPM_BUILD_ROOT}%{_sysconfdir}/sysconfig/watchdog
install -Dp -m0755 %{SOURCE1} ${RPM_BUILD_ROOT}%{_initrddir}/watchdog

%clean
rm -Rf ${RPM_BUILD_ROOT}


%post
if [ $1 -eq 1 ]; then
  /sbin/chkconfig --add %{name}
fi


%preun 
if [ $1 -eq 0 ]; then
  /sbin/service %{name} stop >/dev/null 2>&1
  /sbin/chkconfig --del %{name}
fi


%postun 
if [ $1 -ge 1 ]; then
  /sbin/service %{name} condrestart >/dev/null  2>&1
fi


%files
%defattr(-, root, root, -)
%doc AUTHORS ChangeLog COPYING examples/ IAFA-PACKAGE NEWS README TODO README.watchdog.ipmi README.Fedora
%config(noreplace) %{_sysconfdir}/watchdog.conf
%config(noreplace) %{_sysconfdir}/sysconfig/watchdog
%{_sysconfdir}/rc.d/init.d/watchdog
%{_sbindir}/watchdog
%{_sbindir}/wd_keepalive
%{_mandir}/man5/watchdog.conf.5*
%{_mandir}/man8/watchdog.8*
%{_mandir}/man8/wd_keepalive.8*


%changelog
* Thu Mar  5 2009 Richard W.M. Jones <rjones@redhat.com> - 5.5-2
- Use '-' in defattr line instead of explicit file mode.

* Thu Feb 26 2009 Richard W.M. Jones <rjones@redhat.com> - 5.5-1
- New upstream version 5.5.
- Prepared the package for Fedora review.

* Mon Jun 11  2007 Lon Hohberger <lhh@redhat.com> - 5.3.1-7
- Rebuild for RHEL5 Update 1 - Resolves: 227401

* Wed May 30  2007 Konrad Rzeszutek <konradr@redhat.com> - 5.3.1-6
- Fixed the init script file.

* Tue May 29  2007 Konrad Rzeszutek <konradr@redhat.com> - 5.3.1-5
- Fixed a compile warning in nfsmount_xdr file.

* Wed May 23  2007 Konrad Rzeszutek <konradr@redhat.com> - 5.3.1-4
- Fixed rpmlint warnings.

* Wed May 16  2007 Konrad Rzeszutek <konradr@redhat.com> - 5.3.1-3
- Changes to spec, init script and README file per Carol Hebert recommendation.

* Thu Apr 19  2007 Konrad Rzeszutek <konradr@redhat.com> - 5.3.1-2
- Added README.watchdog.ipmi

* Mon Apr 16  2007 Konrad Rzeszutek <konradr@redhat.com> - 5.3.1-1
- Initial copy. 
