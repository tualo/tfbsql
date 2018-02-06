%global php_extdir  %(php-config --extension-dir 2>/dev/null || echo "undefined")
%global commit 40-CHARACTER-HASH-VALUE
%global gittag GIT-TAG
%global shortcommit %(c=%{commit}; echo ${c:0:7})

Summary:      tualo frontbase for php7
Name:         tfbsql
Version:      1.0.0
Release:      1%{?dist}
Packager:     thomas.hoffmann@tualo.de
Copyright:    Apache License Version 2.0
Group:        Development/Languages

Source:       https://github.com/tualo/%{name}/archive/%{gittag}/%{name}-%{version}.tar.gz

BuildRoot:    /var/tmp/%{name}-buildroot

%description
tualo frontbase for php7

%prep

%setup
%autosetup -n %{name}-%{gittag}


%build
make

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc ChangeLog LICENSE
%config(noreplace) %{_sysconfdir}/php.d/%{name}.ini
%{php_extdir}/%{name}.so

%changelog
* Mon Feb 05 2018 Thomas Hoffmann <thomas.hoffmann@tualo.de> 1.0.0
- initial project
