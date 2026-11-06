# neuromore Studio

Build neurofeedback, biofeedback and research applications without writing code. Connect a biosensor, design a signal processing pipeline in a visual editor, and use the result to drive a game, a video, sound or your own software. Studio is a C++ and Qt desktop application for Windows, macOS and Linux.

**[Documentation](https://docs.neuromore.com/) · [Download](https://github.com/neuromore/studio/releases/latest) · [Build from source](BUILDING.md) · [Licensing](#licensing)**

![The neuromore Studio classifier editor with live feedback graphs](https://user-images.githubusercontent.com/10089188/152370510-9eb30fc5-eb27-4696-9dc0-1ae25acb3586.gif)

## Get started

1. [Create a free community account](https://account.neuromore.com/#/purchase?state=start&productId=community). Studio has no offline mode, so every build, including a source build, signs in with a neuromore account. The backend code is not in this repository.
2. [Download the latest release](https://github.com/neuromore/studio/releases/latest) for Windows, macOS or Ubuntu and follow [Setup](https://docs.neuromore.com/neuromore-studio/setup/), or [build from source](BUILDING.md).
3. Follow the [getting started tutorial](https://docs.neuromore.com/neuromore-studio/tutorial-getting-started/). No hardware yet? Start with the simulated Test Device.

The docs describe the newest Studio version, which can be ahead of the latest release and of this repository. They mark the differences from the latest release, and [What changed](https://docs.neuromore.com/neuromore-studio/what-changed/) lists them.

## Develop

Studio builds with GNU Make and Clang. Most third-party code, including the parts of Qt 5 that Studio uses, is in `deps`.

- [BUILDING.md](BUILDING.md): prerequisites and build commands.
- [Engine library](https://docs.neuromore.com/integrations/engine-library/): use the processing engine in your own application.

The version of this source is in [src/Studio/Version.h](src/Studio/Version.h). It can differ from the versions the docs compare, so for building this source, follow BUILDING.md.

There is no contribution process yet, so a pull request may not get a review. Before you start work you want merged, open a [discussion](https://github.com/neuromore/studio/discussions).

## Documentation

| You want to | Start here |
| --- | --- |
| Check a device | [Biosensors](https://docs.neuromore.com/neuromore-studio/biosensors/) lists each device with its status and setup |
| Build a protocol | [Classifier tutorial](https://docs.neuromore.com/neuromore-studio/tutorial-classifier/), [Nodes](https://docs.neuromore.com/neuromore-studio/nodes/) |
| Run sessions with a sensor | [Running a session](https://docs.neuromore.com/neuromore-studio/sessions/), [Impedance and contact quality](https://docs.neuromore.com/neuromore-studio/impedance/), [Visualizations](https://docs.neuromore.com/neuromore-studio/visualizations/) |
| Connect other software | [Integrations overview](https://docs.neuromore.com/integrations/overview/), [OSC](https://docs.neuromore.com/neuromore-studio/osc/), [Browser neurofeedback](https://docs.neuromore.com/integrations/browser-neurofeedback/) |
| Solve a problem | [Troubleshooting](https://docs.neuromore.com/neuromore-studio/troubleshooting/), [FAQ](https://docs.neuromore.com/neuromore-studio/faq/) |
| Compare community and commercial options | [Editions](https://docs.neuromore.com/neuromore-studio/editions/) |

## Community and commercial clients

**Community.** This repository is the community version, and GitHub releases are the main public download. Developers can build from source and add device drivers themselves, within the limits in [Source builds](https://docs.neuromore.com/neuromore-studio/editions/#source-builds).

**Commercial clients.** Integrations come through custom builds, and clients can have their own neuromore cloud instance and account. [Contact neuromore](https://neuromore.com/contact/).

## Licensing

See [neuromore licensing](https://neuromore.com/licensing/). The legal texts are in this repository: [neuromore-public-license.md](neuromore-public-license.md), [neuromore-class-exception.md](neuromore-class-exception.md) and [neuromore-approved-interfaces.md](neuromore-approved-interfaces.md). Third-party software is listed in [deps/README.md](deps/README.md).

## Get help

- Questions and ideas: [GitHub Discussions](https://github.com/neuromore/studio/discussions)
- Bugs, including build failures: [GitHub Issues](https://github.com/neuromore/studio/issues)
