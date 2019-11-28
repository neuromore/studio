# Building Ubuntu Binary Packages

## Studio

1) Create subfolder in this folder: './studio/usr/local/bin'
2) Copy compiled studio binary into it
3) Ensure package info in './studio/DEBIAN/control' is correct (e.g. increment version/check deps)
4) Run this to create studio.deb:

```
dpkg-deb --build studio
```

5) Installing the created package (-f installs missing dependencies):

```
sudo apt-get install -f ./studio.deb
```

6) Starting neuromore Studio:

```
Studio
```
