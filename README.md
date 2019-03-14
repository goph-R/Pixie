Pixie
=====
The Open Source Image Manager.

![Screenshot](doc/images/screenshot.png?raw=true "Screenshot")

### Install on Windows:

[Download the Win32 installer here](https://github.com/goph-R/Pixie/releases/download/0.1/PixieInstall_0.1.exe)

### Compile on Ubuntu (18.04)

Install qtcreator:

```
sudo apt install build-essentials
sudo apt install qtcreator
```

Then create the make file and run make:

```
qmake -makefile -o Makefile pixie.pro
make
```

The Pixie executable should have been created so run it:

```
./pixie
```
