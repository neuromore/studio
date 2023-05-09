# Building

We provide two integrated build systems in this repository:

- GNU Makefiles for building with Clang/LLVM on all platforms
- (deprecated) Visual Studio solution for intuitive Windows development using MSVC++ as compiler

## GNU Make + Clang

This is our main multi-platform build environment based on [GNU Make](https://www.gnu.org/software/make/) and [Clang](https://clang.llvm.org/).

It supports the following platforms:

- Windows
- Linux
- Mac OSX

### Prerequisites

#### Linux (Ubuntu)

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
 libxcb-util-dev \
 libpulse-dev \
 libasound2-dev
```

#### Mac OSX

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

#### Windows

- [Download](https://github.com/llvm/llvm-project/releases) and install LLVM.
- [Download](http://gnuwin32.sourceforge.net/packages/make.htm) and install GNU Make.
- [Download](https://visualstudio.microsoft.com/downloads/) install Visual Studio.
- Reboot

During installation, enable the checkbox to add them to your user's path (or all users' path).

_Verify:_

```
make -v
clang++ -v
llvm-ar --version
```

### Building (all platforms)

#### Makefile Parameters

Following Parameters are supported. Defaults will be used if not provided.

| Parameter   | Values                      | Default    |
| ----------- | --------------------------- | ---------- |
| MODE        | debug, release              | debug      |
| BRANDING    | neuromore, ...              | neuromore  |
| TARGET_OS   | win, osx, linux, android    | {host-os}  |
| TARGET_ARCH | x86, x64, arm, arm64        | {host-cpu} |

#### Basic Builds with Defaults

_Building third party dependencies in debug mode for operating system and CPU of the current host:_

```
make Dependencies
```

_Building all neuromore projects in debug mode for operating system and CPU of the current host:_

```
make
```

_Cleaning debug third party dependencies for operating system and CPU of the current host:_

```
make Dependencies-clean
```

_Cleaning all neuromore projects in debug mode for operating system and CPU of the current host:_

```
make clean
```

#### Customized Examples

_Building with customized parameters:_

```
make MODE=release TARGET_OS=win TARGET_ARCH=x86 Dependencies
make MODE=release TARGET_OS=win TARGET_ARCH=x86 Studio
```

#### Build Output Folders

_Example Outputs of Executables:_

| OS  | Arch | Mode    | Folder                               |
| --- | ---- | ------- | ------------------------------------ |
| win | x86  | debug   | /build/make/bin/win-x86/Studio_d.exe |
| osx | x64  | release | /build/make/bin/osx-x64/Studio       |
| ... | ...  | ...     | ...                                  |

_Example Outputs Third Party Libraries:_

| OS  | Arch | Folder                                  |
| --- | ---- | --------------------------------------- |
| win | x86  | /deps/build/make/lib/win-x86/zlib_d.lib |
| osx | x64  | /deps/build/make/lib/osx-x64/zlib.a     |
| ... | ...  | ...                                     |


## Visual Studio (deprecated)

TODO: Update this

_Folder:_ [/build/vs](https://github.com/neuromore/studio/tree/master/build/vs)

_Requirements:_

- Visual Studio 2019 (default) or 2017 (old)
- Installed VS-Feature: Desktop Development (C/MSVC++ tools/components)
- Installed VS-Feature: Windows 10 SDK (any version, the later the better)

_Steps:_

- Open [Neuromore.sln](https://github.com/neuromore/studio/blob/master/Neuromore.sln) in Visual Studio
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


## Customizing the Build

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
| USE_QTDATAVISUALIZATION | Not supported / WIP                       |
| USE_AUTOUPDATE          | deprecated: controlled autoupdate-feature |
