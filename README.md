# MetaSound-Plugins 
This repository contains a collection of custom nodes for UE5 MetaSounds, which I am working toward releasing as a plugin (working title: "Branches").

## Documentation
Development notes:
- [Initial notes](./docs/README.md): information on the first custom node and development process.
- [Quick reference](./docs/quick_reference.md): my personal cheatsheet for creating and troubleshooting custom nodes. 

The nodes available so far should be familiar to most audio folk, but please get in touch if you'd like to know more.

## MetasoundBranches
There are currently four nodes available in the *MetaSoundBranches* plugin for testing, with several more in development:

| Node      | Description                                                  |
|-----------|--------------------------------------------------------------|
| `Dust`    | A randomly timed impulse generator with an audio-rate density control.|
| `SaH`     | An alternative sample and hold unit with an audio-rate trigger. |
| `SaH Bank`| A bank of four identical sample and hold units.              |
| `Slew`    | A slew limiter to smooth out the rise and fall times of an audio signal. |

Upon installing the plugin, these items will appear in *Branches* within the Metasound *Functions* category.

![Signal flow in a MetaSound Source showing a sample and hold node connected to two audio sources, controlling the volume of a white noise generator.](./docs/SaH_demo.png)

## Installation
### Windows
- Clone the repository into your chosen Unreal Engine project's `Plugins` directory.
- Build the project using the [Unreal Automation Tool](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-automation-tool-for-unreal-engine):
-- Navigate to the `Engine\Build\BatchFiles\` folder, and run: 
```PowerShell
.\RunUAT.bat BuildPlugin -plugin="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin" -package="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin"
```

Please see [Learning / Building Plugins](https://dev.epicgames.com/community/learning/tutorials/qz93/unreal-engine-building-plugins) for more information.

---

## Links
- [MetaSounds Reference Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/metasounds-reference-guide-in-unreal-engine)
- [MetaSounds Tutorial videos](https://dev.epicgames.com/community/learning/recommended-community-tutorial/Kw7l/unreal-engine-metasounds) (Matt Spendlove)
- [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart) (Anna Lantz)
