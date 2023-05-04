%{!?directory:%define directory /usr}

%define buildroot %{_tmppath}/%{name}

Name:          tclgumbo
Summary:       Tcl interface for Gumbo library
Version:       0.2.7
Release:       0
License:       BSD-2-Clause
Group:         Development/Libraries/Tcl
Source:        %{name}-%{version}.tar.gz
URL:           https://github.com/ray2501/tclgumbo 
BuildRequires: autoconf
BuildRequires: make
BuildRequires: tcl-devel >= 8.4
Requires:      tcl >= 8.4
BuildRoot:     %{buildroot}

%description
Tcl interface for Gumbo library.

Google Gumbo is an implementation of the HTML5 parsing algorithm implemented
as a pure C99 library with no outside dependencies.  It's designed to serve
as a building block for other tools and libraries such as linters,
validators, templating languages, and refactoring and analysis tools.

%prep
%setup -q -n %{name}-%{version}

%build
CFLAGS="%optflags" ./configure \
	--prefix=%{directory} \
	--exec-prefix=%{directory} \
	--libdir=%{directory}/%{_lib}
make 

%install
make DESTDIR=%{buildroot} pkglibdir=%{tcl_archdir}/%{name}%{version} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%{tcl_archdir}
