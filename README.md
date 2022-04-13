# neuromore Studio

Build state-of-the-art neurofeedback applications with ease using neuromore Studio!

Our node-based IDE supports a wide variety of biosensors like EEG headsets (e.g. OpenBCI boards, Interaxon’s Muse and devices supported by the Brainflow API), heart rate monitors, and bluetooth devices that you can effortlessly connect to your applications.

Create audio or visual experiences within neuromore Studio or make your application control your Unity game, your favorite audio production, or your visualizer of choice using the Open Sound Control protocol. The only limit is our imagination!

![2022_02_03_Demo](https://user-images.githubusercontent.com/10089188/152370510-9eb30fc5-eb27-4696-9dc0-1ae25acb3586.gif)

## License

[Click here to learn about neuromore Licensing](https://github.com/neuromore/studio/blob/master/neuromore-licensing-info.md)

## Getting started (all platforms)

1. Create a neuromore Cloud account at [https://account.neuromore.com/#/purchase?state=start&productId=community](https://account.neuromore.com/#/purchase?state=start&productId=community)
2. Download the latest neuromore studio from our github [releases](https://github.com/neuromore/studio/releases) (or build it yourself).
3. Start neuromore studio, sign-in with the created community account, accept the license and you're ready to go.
4. Check out the wiki for specific device information or on examples on how to get started https://github.com/neuromore/studio/wiki
5. Watch the tutorials for more info [https://www.youtube.com/channel/UCAOU6SsvwCwC30hJaFLhWgw](https://www.youtube.com/channel/UCAOU6SsvwCwC30hJaFLhWgw)

## Minimum System Requirements

| What     | Minimum                                               | Recommended |
| -------- | ----------------------------------------------------- | ----------- |
| OS       | Windows 10, Ubuntu 20.04, OSX 10.15                   | TODO        |
| CPU      | Dual-Core released past ~2010 (with SSE4.2, AVX, ...) | TODO        |
| RAM      | 4GB                                                   | TODO        |
| GPU      | OpenGL 2.x compatible (TODO: more details, e.g. MEM)  | TODO        |

## Configuration/Cache Folders

| Platform | Folder                                           |
| -------- | ------------------------------------------------ |
| WIN      | %LOCALAPPDATA%\neuromore\NMStudio                |
| LINUX    | ~/.local/share/neuromore/NMStudio                |
| OSX      | ~/Library/Application Support/neuromore/NMStudio |

## Building

See [this](https://github.com/neuromore/studio/blob/master/BUILDING.md) for more information about how to compile neuromore Studio.

## Dependencies

See [this](https://github.com/neuromore/studio/blob/master/deps/README.md) for more information about the third party software included in this repository.

## FAQ

### Technical

_Q:_ Can I build an offline version of neurmore studio (one that does not require your backend)?
_A_: This is a work in progress. We don't provide an option for that. For now, access to our backend is mandatory.

_Q:_ Is the backend code included or available so that I can host it myself?
_A_: No. The backend code and database design is not included in this repository.

_Q:_ How do I reset my password?
_A:_ https://account.neuromore.com/#/login

## Related Projects

- [Run the muse-2 in neuromore](https://github.com/naxocaballero/muse2-neuromore) - https://github.com/naxocaballero/muse2-neuromore - thanks to [@naxocaballero](https://github.com/naxocaballero)
- [There's also an LSL to OSC converter](https://github.com/ViacheslavBobrov/Muse_Neuromore) by @ViacheslavBobrov that works with the muse.
- [BrainFlow configuration](https://github.com/brainflow-dev/brainflow/blob/master/docs/SupportedBoards.rst) by @Andrey1994
