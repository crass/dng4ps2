Summary: Program for convertion RAW files from hacked Canon cameras into DNG fromat
Name: dng4ps2
Version: __VERSION__
Release: __RELEASE__
Group: Photo
License: GPL
Packager: Denis Artyomov  [denis.artyomov@gmail.com]
URL: http://dng4ps2.chat.ru
BuildRoot: %{_tmppath}/dng4ps2-root

%description
Program for convertion RAW files from hacked Canon cameras into DNG fromat

%files
%defattr(-,root,root)
/usr/local/dng4ps2/
%{_datadir}/applications/dng4ps2.desktop

%install
mkdir -p -m 555 %{buildroot}/usr/local/dng4ps2
mkdir -p -m 555 %{buildroot}/usr/local/dng4ps2/langs

install -m 555 /root/rpms/dng4ps2/dng4ps-2 %{buildroot}/usr/local/dng4ps2/
install -m 444 /root/rpms/dng4ps2/langs/* %{buildroot}/usr/local/dng4ps2/langs/

cat >dng4ps2.desktop << 'EOF'
[Desktop Entry]
Name=dng4ps2
Categories=Graphics;Photography;
Exec=/usr/local/dng4ps2/dng4ps-2
Icon=/usr/local/dng4ps2/dng4ps2.png
Terminal=false
Type=Application
EOF

install -d -m0755 %{buildroot}%{_datadir}/applications/
desktop-file-install --dir %{buildroot}%{_datadir}/applications dng4ps2.desktop

%post
update-desktop-database

%clean
rm -rf %{buildroot}
