# -D MUST pass in _version and _release, and SHOULD pass in dist.

Summary: UUID module for Varnish Cache
Name: vmod-uuid
Version: %{_version}
Release: %{_release}%{?dist}
License: BSD
Group: System Environment/Daemons
URL: https://github.com/otto-de/libvmod-uuid
Source0: %{name}-%{version}.tar.gz

# varnish from varnish5 at packagecloud
Requires: varnish >= 5.2.0
Requires: uuid

BuildRequires: varnish-devel >= 5.2.0
BuildRequires: uuid-devel
BuildRequires: pkgconfig
BuildRequires: make
BuildRequires: gcc
BuildRequires: python-docutils >= 0.6

# git builds
#BuildRequires: automake
#BuildRequires: autoconf
#BuildRequires: autoconf-archive
#BuildRequires: libtool

Provides: vmod-uuid, vmod-uuid-debuginfo

%description
UUID Varnish vmod used to generate a uuid, including versions 1, 3, 4
and 5 as specified in RFC 4122. See the RFC for details about the
various versions.

%prep
%setup -q -n %{name}-%{version}

%build

# if this were a git build
# ./autogen.sh

%configure

make

%check

make check

%install

make install DESTDIR=%{buildroot}

# Only use the version-specific docdir created by %doc below
rm -rf %{buildroot}%{_docdir}

# None of these for fedora/epel
find %{buildroot}/%{_libdir}/ -name '*.la' -exec rm -f {} ';'
find %{buildroot}/%{_libdir}/ -name '*.a' -exec rm -f {} ';'

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_libdir}/varnish*/vmods/
%{_mandir}/man3/*.3*
%doc README.rst COPYING LICENSE

%post
/sbin/ldconfig

%changelog
