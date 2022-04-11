## Building

We provide two integrated build environments in this repository:

- [Visual Studio](#visual-studio) solution for intuitive Windows development using MSVC++ as compiler
- [GNU Makefiles](#gnu-make-clang) for building with Clang/LLVM on multiple platforms

### Visual Studio

_Folder:_ [/build/vs](https://github.com/neuromore/studio/tree/master/build/vs)

_Requirements:_

- Visual Studio 2019 (default) or 2017 (old)
- Installed VS-Feature: Desktop Development (C/MSVC++ tools/components)
- Installed VS-Feature: Windows 10 SDK (any version, the later the better)

_Steps:_

- Open [Neuromore.sln](https://github.com/neuromore/studio/blob/master/build/vs/Neuromore.sln) in Visual Studio
- For VS2017: Manually downgrade project platformtoolset from v142 (2019) to v141 (2017)
- For VS2017: Manually set a Win 10 SDK version that you have installed
- Build the "Studio" project in release mode or start it directly in debug mode

_Configurations:_

| Name       | Optimized | Debuggable | Logging | Notes                          |
| ---------- | --------- | ---------- | ------- | ------------------------------ |
| Debug      | No        | Yes        | Max     |                                |
| Release    | Yes       | No         | Medium  | Also called DEVELOPMENT        |
| Production | Yes       | No         | Min     | Release + PRODUCTION_BUILD set |

_Build Output Studio:_

| Arch | Folder            |
| ---- | ----------------- |
| x86  | /build/vs/bin/x86 |
| x64  | /build/vs/bin/x64 |

_Notes:_

- Creates portable, stand-alone executable not requiring any special DLLs
- Runs on Windows 7 and above

### GNU Make + Clang

This is our multi-platform build environment based on [GNU Make](https://www.gnu.org/software/make/) and [Clang](https://clang.llvm.org/).

It supports the following platforms:

- Windows
- Linux
- Mac OSX

#### Prerequisites

##### Linux (Ubuntu)

_Get the required build tools:_

```
sudo apt-get install make clang llvm lld
```

_Verify the build tool versions:_

```
make -v
clang++ -v
ar
```

_Get the required development packages of linked libraries:_

```
sudo apt-get install \
 libgstreamer1.0-dev \
 libgstreamer-plugins-base1.0-dev \
 libgstreamer-plugins-good1.0-dev \
 libglib2.0-dev \
 libgl1-mesa-dev \
 libglu1-mesa-dev \
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

##### Mac OSX

_Install these_

```
Xcode
Xcode Command Line Tools
```

_Verify the build tool versions:_

```
make -v
clang++ -v
ar
```

##### Windows

- We recommend to use Visual Studio on Windows, not Clang.
- Visual Studio is still required to build with Clang on Windows (linking VC runtime).
- Requires GNU Make for Windows, Clang for Windows and LLVM for Windows (TODO: Links)

#### Building (all platforms)

_Building third party dependencies:_

```
cd deps/build/make
make
```

_Building Studio:_

```
cd build/make
make
```

_Cleaning third party dependencies:_

```
cd deps/build/make
make clean
```

_Cleaning Studio:_

```
cd build/make
make clean
```

_Building with custom parameters:_

```
cd deps/build/make
make all-x64 -f Makefile.linux -j 4
```

_Notes:_

- Building without parameters will build for your current os, arch and cpu cores count
- Building the dependencies is only required once (or when they change, which doesn't occur often)
- Custom: Replace 'linux' in 'Makefile.linux' by 'windows' or 'osx' to build on Windows or OSX
- Custom: Replace 'x64' by 'x86' to build the 32 Bit version instead of 64 Bit
- Custom: Replace '4' in '-j 4' by the number of cores you want to use for compilation

_Build Output Studio:_

| Arch | Folder              |
| ---- | ------------------- |
| x86  | /build/make/bin/x86 |
| x64  | /build/make/bin/x64 |

_Build Output Dependencies:_

| Arch | Folder                   |
| ---- | ------------------------ |
| x86  | /deps/build/make/lib/x86 |
| x64  | /deps/build/make/lib/x64 |

#### Customizing the Build

- Using preprocessor C/C++ macro defines.
- Set in Config.h files or in build environment.

##### General

| Macro/Define     | Description                                               |
| ---------------- | --------------------------------------------------------- |
| PRODUCTION_BUILD | Tweaks builds for production instead of development usage |

##### Engine Devices

- Configure in Engine [Config.h](https://github.com/neuromore/studio/blob/master/src/Engine/Config.h)
- Enabling these requires proprietary contents from compatible SDK versions at compile/link/run time
- Compile-Time: Headers from SDK go to: /deps/include/some-device/
- Link-Time: Prebuilt proprietary Libs from SDK go to: /deps/prebuilt/win/x86/ (respectively /linux/ or /x64/)
- Run-Time: Prebuilt proprietary DLLs from SDK go to wherever you place and start your Studio.exe from

| Macro/Define                           | Version                                           | Description           |
| -------------------------------------- | ------------------------------------------------- | --------------------- |
| INCLUDE_DEVICE_EMOTIV                  | [master](https://github.com/Emotiv/community-sdk) | Emotiv EEG            |
| INCLUDE_DEVICE_MITSAR                  | ?                                                 | Mitsar EEG            |
| INCLUDE_DEVICE_NEUROSKY_MINDWAVE       | 1.2.0                                             | MindWave EEG          |
| INCLUDE_DEVICE_TOBIIEYEX               | ?                                                 | TobiEyeX Eye Tracking |
| INCLUDE_DEVICE_ADVANCEDBRAINMONITORING | ?                                                 | ABM EEG               |
| INCLUDE_DEVICE_BRAINQUIRY              | ?                                                 | Brainquiry EEG        |
| INCLUDE_DEVICE_ACTICHAMP               | ?                                                 | ActiChamp EEG         |
| INCLUDE_DEVICE_EEMAGINE                | 1.3.19                                            | eego Amplifiers/EEG   |

##### Studio Features

- Configure in Studio [Config.h](https://github.com/neuromore/studio/blob/master/src/Studio/Config.h)

| Macro/Define            | Description                               |
| ----------------------- | ----------------------------------------- |
| BACKEND_LOGGING         | Controls Backend Logging                  |
| OPENCV_SUPPORT          | OpenCV support in Studio                  |
| USE_CRASHREPORTER       | Enables/Disables CrashReporter utility    |
| USE_QTDATAVISUALIZATION | Not supported / WIP                       |
| USE_AUTOUPDATE          | deprecated: controlled autoupdate-feature |
