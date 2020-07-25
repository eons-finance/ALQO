
Debian
====================
This directory contains files used to package alqod/alqo-qt
for Debian-based Linux systems. If you compile alqod/alqo-qt yourself, there are some useful files here.

## alqo: URI support ##


alqo-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install alqo-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your alqo-qt binary to `/usr/bin`
and the `../../share/pixmaps/alqo128.png` to `/usr/share/pixmaps`

alqo-qt.protocol (KDE)

