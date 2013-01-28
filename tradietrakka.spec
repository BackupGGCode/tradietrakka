

Name:		tradietrakka
Version:	0.1
Release:	%mkrel	2
Summary:	Simple business tool for Tradesmen to keep track of invoices, tax etc.
Source0:	http://tradietrakka.googlecode.com/files/tradietrakka-0.1.tar.xz

URL:		http://tradietrakka.googlecode.com/

Group:		Applications/Office
License:	GPLv3
BuildRequires:	libqt4-devel

%description
TradieTrakka is a very simple, easy to use program aimed at Tradesmen running
a small business. It makes life easy by making it trivial to Quote, Invoice
and Generate BAS (Australian GST reporting)

%prep
%setup -q -n %{name}

%build
%qmake_qt4
%make

%install
%makeinstall_std INSTALL_ROOT=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%{_bindir}/TradieTrakka

%changelog
* Sun Jan 15 2012 Steven Tucker <tuxta2@gmail.com> 0.5mga2

 Sun Jan 15 2012 Steven Tucker <tuxta2@gmail.com> 0.5mga2
 
 Specfile written for Mageia
