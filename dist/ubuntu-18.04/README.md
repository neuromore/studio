# Building Ubuntu Binary Packages

## Studio

1. Create subfolder in this folder: './studio/usr/local/bin'
2. Copy compiled studio binary into it
3. Create subfolder './studio/usr/local/share/neuromore
4. Copy Visualizations from previous build into it
5. Ensure package info in './studio/DEBIAN/control' is correct (e.g. increment version/check deps)
6. Run this to create studio.deb:

```
dpkg-deb --build studio
```

7. Installing the created package (-f installs missing dependencies):

```
sudo apt-get install -f ./studio.deb
```

8. Starting neuromore Studio:

```
Studio
```
