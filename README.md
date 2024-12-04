# MetaSound-Plugins 
This repository contains a collection of custom nodes for Unreal Engine's MetaSounds, which I am working toward releasing as a plugin (working title: "Branches").

## Branches
There are currently thirteen nodes available in the *MetaSoundBranches* plugin for testing, with several more in development:

| Node      | Description                                                  |
|-----------|--------------------------------------------------------------|
| `Bool To Audio`    | Convert a boolean value to an audio signal, with optional rise and fall times.|
| `Clock Divider`    | Divide a trigger into eight density levels.|
| `Crossfade (stereo)`    | A crossfader for stereo signals.|
| `Click`    | Trigger an impulse (unipolar or alternating polarity per impulse).|
| `Dust`    | A randomly timed impulse generator (unipolar or alternating polarity per impulse) with density control and audio-rate modulation.|
| `EDO`    | Generate frequencies for tuning systems using equally divided octaves (float) with a MIDI note input.  Set a reference frequency and reference MIDI note (defaults to A440).|
| `Phase Disperser`     | A chain of allpass filters to soften transients and add that classic laser/slinky-style effect. |
| `Sample And Hold (audio trigger)`     | An alternative sample and hold unit with an audio-rate trigger. |
| `Sample and Hold Bank (audio trigger)`| A bank of four identical sample and hold units with audio-rate triggers.              |
| `Shift Register`| An eight-stage shift register for floats. |
| `Slew`    | A slew limiter to smooth out the rise and fall times of an audio signal. |
| `Tuning`    | Quantize a float value to a custom 12-note tuning, with adjustment per-note. |
| `Width`    | Stereo width adjustment (0-200%), using mid-side processing. |

Upon installing the plugin, these items will appear in the sub-category `Branches` within the Metasound `Functions` category.

![Screenshot of a selection of custom nodes in Metasound, as listed in the table above](./docs/nodes.png)
![Signal flow in a MetaSound Source showing a sample and hold node connected to two audio sources, controlling the volume of a white noise generator.](./docs/SaH_demo.png)

## Installation
- Clone the repository into your chosen Unreal Engine project's `Plugins` directory.
- Build the project using the [Unreal Automation Tool](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-automation-tool-for-unreal-engine):
### Windows
- In PowerShell, navigate to the `Engine\Build\BatchFiles\` folder, and run: 
```PowerShell
.\RunUAT.bat BuildPlugin -plugin="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin" -package="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin"
```
### Mac
- In terminal, navigate to the `Engine/Build/BatchFiles/` folder, and run: 
```Bash
sudo ./RunUAT.sh BuildPlugin -plugin="<path to your project>/Plugins/metasound-plugins/MetasoundBranches.uplugin" -package="<path to your project>/Plugins/metasound-plugins/MetasoundBranches.uplugin"
```

Please see [Learning / Building Plugins](https://dev.epicgames.com/community/learning/tutorials/qz93/unreal-engine-building-plugins) for more information.

## Documentation
Development notes:
- [Initial notes](./docs/README.md): information on the first custom node and development process.
- [Quick reference](./docs/quick_reference.md): some development notes for creating and troubleshooting custom nodes. 

---

## Links
- [MetaSounds Reference Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/metasounds-reference-guide-in-unreal-engine)
- [MetaSounds Tutorial videos](https://dev.epicgames.com/community/learning/recommended-community-tutorial/Kw7l/unreal-engine-metasounds) (Matt Spendlove)
- [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart) (Anna Lantz)