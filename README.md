# neuromore Studio

## Your all-in-one biofeedback suite

Create stunning neuro- and biofeedback experiences with ease using neuromore Studio!

![2022_02_03_Demo](https://user-images.githubusercontent.com/10089188/152370510-9eb30fc5-eb27-4696-9dc0-1ae25acb3586.gif)

## Features 

### Plug'n'play - Your no-code IDE for biodata application

The Studio is designed to enable everyone from neuroscientists, therapists and clinicians to neuroenthusiasts to build biofeedback applications without writing code. Drag and drop nodes together and create immersive biofeedback applications at the speed of thought.

### Hardware-agnostic - One platform to interface them all

neuromore Studio provides you with a hardware-agnostic interface that includes medical grade EEG amplifiers, consumer EEG headsets, and heart and GSR sensors. 
Those devices include: 
- Neurosity Crown, Notion, & Notion 2
- Interaxon Muse 2
- OpenBCI Cyton, Daisy, Ganglion
- G.Tec Unicorn (via Brainflow)
- Ant Neuro eego mini 8 and mini 24
- Brainmaster Discovery
- Mitsar	EEG 201
- Polar 10
- All Brainflow devices
- ... and many more

Have a look at the [devices](https://doc.neuromore.com/?cat=0&page=3) section in our docs to see a full list of supported devices. 
We're always interested in extending this list together with our community so if you have a request or want to integrate a device, please reach out and we can find a way to make it work with the Studio.

### State-of-the-art biofeedback

Create immersive end-to-end experiences from protocol to visualization. The Studio comes with 4 pre-built games in which you can control the speed of a car, the weather, screen effects and volume. You can also choose to play videos or audio files and control their brightness and volume for personalized neurofeedback training. 
And if you want to go one step further, you can also make your application control your Unity game, your favorite audio production, or your visualizer of choice via the Open Sound Control protocol. The only limit is our imagination!

### Visualize your data - From data to insights in real-time

Analyze raw and processed EEG and other bio-signals in real-time using various signal views, a 3D LORETA brain representation, power spectograms, and more.

- Live biodata viewer
- 3D LORETA visualization
- Feedback visualizations
- Power spectrograms
- Completely configurable layouts

### Cloud convenience - Analyze session reports from anywhere

Analyze your own sessions or sessions of your patients in the cloud under account.neuromore.com using various graphs and statistics.

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
| OSX      | ~/Library/Containers/com.neuromore.studio/Data/Library/Application Support/neuromore/NMStudio |

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
