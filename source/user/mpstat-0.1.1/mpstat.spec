# This file was automagically generated with spec-gen 
# By Joseph Acosta 
Summary: Utility for monitoring CPU status on MultiProcessor systems
Name: mpstat
Version: 0.1.0
Release: 1
URL: http://www.mindspring.com/~joeja/programs.html
Copyright: GPL
Group: Utilities/System
BuildRoot: /var/tmp/mpstat-root
Source: mpstat-0.1.0.tar.gz
Packager: Joseph Acosta
ExclusiveOS: Linux
%description
mpstat is a utitlity for Linux to help in the monitoring of SMP machines.
Currently it displays percent usr/sys CPU, percent idle, and interupts split between CPU's and major and minor faults.
%prep
%setup 
%build
patch -p1 < mpstat.rpm.patch
make
%install
make install
%clean
rm -rf /var/tmp/mpstat-root
%files
/usr/bin/mpstat
%doc README
%doc FAQ.txt
%changelog

