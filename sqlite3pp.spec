Summary:	sqlite3 C++ wrapper
Name:		sqlite3pp
Version:	1.0.9
Release:	1%{?dist}
BuildArch:	noarch
Group:		Development/Tools
License:	Unknown
URL:		https://github.com/fmidev/sqlite3pp
Source0:	%{name}.tar.gz

%description
sqlite3 C++ wrapper

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n sqlite3pp

%build

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%package devel
Summary: sqlite3 C++ wrapper
Provides:	sqlite3pp-devel

%description devel
sqlite3 C++ wrapper

%files devel
%defattr(-,root,root,0775)
%{_includedir}/sqlite3pp/*

%changelog
* Tue Feb  8 2022 Mika Heiskanen <mika.heiskanen@fmi.fi> - 1.0.9-1
- First packaged version

