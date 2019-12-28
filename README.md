# OSS evolution of classic nx technology - OpenNX CE

All these years, the classic nx was not as dead as it seemed ;)
It is used in production and develops as fast as it can.

I am very grateful to the developers of the [ArcticaProject/nx-libs](https://github.com/ArcticaProject/nx-libs) for maintaining backward
compatibility and the opportunity to use their libraries instead
of self-assembly.

Compared to the original opennx new features added by community:

- Building with wxWidgets 3.0.4

- For win32 building with modified !M NX-libs uder cygwin. Xming not used.
Sources of these libs may be found at [old git repo](http://git.etersoft.ru/people/dimbor/packages/?p=NXClient-W32.git)

- CUPS client mode: client's system CUPS used directlty
This need to provide username/pass on each printer (like on win-resources);

- Pulseaudio sound: direct, tunnelled, with or without resampling.
On Windows pulseaudio6 binarys are included;

- Localization of windows sharenames;

- On Windows original fonts from nxclient are included.

- ...

It worked with our own [freenx-server](https://github.com/dimbor-ru/freenx-server)
And with classic nxserver 3.x.x compatibility remained.

Debian package home-maded for Devuan ASCII now. There is a suspicion that
under Debian Stretch everything will be fine.

Windows installer can be downloaded at [unixforum.org](https://unixforum.org/up/nxman/OpenNXCE-1.0.0-r15.5.exe)

dimbor. 2019.
