# neuromore studio

## License

[Click here to learn about neuromore Licensing](https://github.com/neuromore/studio/blob/master/neuromore-licensing-info.md)

## Getting started

1. Create a neuromore Cloud account at [https://www.neuromore.com/download](https://www.neuromore.com/download)
2. Download the latest neuromore studio from our github [releases](https://github.com/neuromore/studio/releases) (or build it yourself).
3. Start neuromore studio, sign-in with the created community account, accept the license and you're ready to go.

## Contents

#### Configuration/Cache Folders:

| Platform | Folder                            |
|----------|-----------------------------------|
| WIN      | %LOCALAPPDATA%\neuromore\NMStudio |
| LINUX    | ~/.local/share/neuromore/NMStudio |

### Dependencies

See [this](https://github.com/neuromore/studio/blob/master/deps/README.md) for more information about the third party software included in this repository.

## Building

We provide two integrated build environments in this repository:

* Visual Studio solution for intuitive Windows development using MSVC++ as compiler
* GNU Makefiles for building with Clang/LLVM on multiple platforms

### Visual Studio

*Folder:* [/build/vs](https://github.com/neuromore/studio/tree/master/build/vs)

*Requirements:*
* Visual Studio 2017 (default) or 2019 (new)
* Installed VS-Feature: Desktop Development (C/MSVC++ tools/components)
* Installed VS-Feature: Windows 10 SDK 10.0.17763

*Steps:*
* Open [Neuromore.sln](https://github.com/neuromore/studio/blob/master/build/vs/Neuromore.sln) in Visual Studio
* For VS2019: Select upgrade from v141 (2017) to v142 (2019) as wizard suggests
* Build the "Studio" project in release mode or start it directly in debug mode

*Build Output Studio:*

| Arch | Folder            |
|------|-------------------|
| x86  | /build/vs/bin/x86 |
| x64  | /build/vs/bin/x64 |

*Notes:*

* Creates portable, stand-alone executable not requiring any special DLLs
* Runs on Windows 7 and above

### GNU Make + Clang + LLVM

This is our multi-platform build environment based on [GNU Make](https://www.gnu.org/software/make/), [Clang](https://clang.llvm.org/) and [LLVM](https://llvm.org/).

It supports the following platforms:

* Windows
* Linux
* Mac (Work-In-Progress)

#### General

*Building third party dependencies:*

```
cd deps/build/make
make all-x64 -f Makefile.linux -j 4
```

*Building Studio:*

```
cd build/make
make Studio-x64 -f Makefile.linux -j 4
```

*Cleaning third party dependencies:*

```
cd deps/build/make
make clean -f Makefile.linux -j 4
```

*Cleaning Studio:*

```
cd build/make
make clean -f Makefile.linux -j 4
```

*Notes:*

* Replace 'linux' in 'Makefile.linux' by 'windows' to build on Windows instead of Linux
* Replace 'x64' by 'x86' to build the 32 Bit version instead of 64 Bit
* Replace '4' in '-j 4' by the number of cores you want to use for compilation
* Building the dependencies is only required once (or when they change, which doesn't occur often)

*Build Output Studio:*

| Arch | Folder              |
|------|---------------------|
| x86  | /build/make/bin/x86 |
| x64  | /build/make/bin/x64 |

*Build Output Dependencies:*

| Arch | Folder                   |
|------|--------------------------|
| x86  | /deps/build/make/lib/x86 |
| x64  | /deps/build/make/lib/x64 |

#### Linux (Ubuntu)

*Get the required build tools:*

```
sudo apt-get install make clang llvm
```

*Verify the build tool versions:*

```
make -v
clang++ -v
llvm-ar -version
```

*Get the required development packages of linked libraries:*

```
sudo apt-get install \
 libgl1-mesa-dev \
 libsm-dev \
 libx11-dev \
 libx11-xcb-dev \
 libexpat-dev \
 libxkbcommon-dev \
 libxcb1-dev \
 libxcb-glx0-dev \
 libxcb-icccm4-dev \
 libxcb-image0-dev \
 libxcb-keysyms1-dev \
 libxcb-randr0-dev \
 libxcb-render0-dev \
 libxcb-render-util0-dev \
 libxcb-shape0-dev \
 libxcb-shm0-dev \
 libxcb-sync-dev \
 libxcb-xfixes0-dev \
 libxcb-xinerama0-dev \
 libxcb-xkb-dev \
 libxcb-util-dev
```

#### Windows

* We recommend to use Visual Studio on Windows, not Clang.
* Visual Studio is still required to build with Clang on Windows (linking VC runtime).
* Requires GNU Make for Windows, Clang for Windows and LLVM for Windows (TODO: Links)

### Customizing the Build

TODO: Info about defines for device support/features

## FAQ

### Technical

*Q:* Can I build an offline version of neurmore studio (one that does not require your backend)?
*A*: This is a work in progress. We don't provide an option for that. For now, access to our backend is mandatory.

*Q:* Is the backend code included or available so that I can host it myself?
*A*: No. The backend code and database design is not included in this repository.
