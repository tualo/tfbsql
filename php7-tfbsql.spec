#
# spec file for package php7-tfbsql
#
# Copyright (c) 2018 Chemnitzer Verlag und Druck GmbH u. Co. KG, Chemnitz, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

%define php_name    php7
%define pkg_name    tfbsql
Name:           php7-%{pkg_name}
Version:        20180206
Release:        0
Summary:        FrontBase module for php7
License:        Apache-2.0
Group:          Productivity/Networking/Web/Servers
Url:            https://github.com/tualo/tfbsql
Source:         %{pkg_name}-master.zip
Patch1:         Makefile.patch
BuildRequires:  %{php_name} >= 7
BuildRequires:  %{php_name}-devel >= 7
BuildRequires:  unzip
BuildRequires:  gcc-c++
BuildRequires:	libphpcpp-devel
BuildRequires:	libphpcpp2_0
BuildRequires:	FrontBase-devel
Provides:       php-%{pkg_name} = %{version}
Obsoletes:      php-%{pkg_name} < %{version}
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
%if %{?php_zend_api}0
Requires:       php(api) = %{php_core_api}
Requires:       php(zend-abi) = %{php_zend_api}
%else
%requires_eq    %{php_name}
%endif

%description
tfbsql is a not complete replacement of the missing frontbase module for php7.

%prep
%setup -q -n %{pkg_name}-master
%patch1

%build
make

%install
make install INSTALL_ROOT=%{buildroot}

%files
%defattr(-,root,root,-)
%{_libdir}/%{php_name}/extensions/tfbsql.so
%config(noreplace) %{_sysconfdir}/%{php_name}/conf.d/tfbsql.ini
%doc LICENSE

%changelog
