# MetaSound-Branches 
This repository contains a collection of custom nodes for Unreal Engine's MetaSounds, downloadable as the *Branches* plugin for Windows and Mac.

![Screenshot of a selection of custom nodes in Metasound.](./docs/svg/Nodes.svg)

## Nodes
There are currently eighteen nodes available in the *MetaSound Branches* plugin for testing, with several more in development:

| Node | Category | Description |
|------|-----------|-------------|
| [`Bool To Audio`](https://matthewscharles.github.io/metasound-branches/BoolToAudio.html) | Conversions | Convert a boolean value to an audio signal, with optional rise and fall times. |
| [`Clock Divider`](https://matthewscharles.github.io/metasound-branches/ClockDivider.html) | Triggers | Divide a trigger into eight density levels. |
| [`Dust (Audio)`](https://matthewscharles.github.io/metasound-branches/Dust(Audio).html) | Generators | Generate randomly timed impulses (unipolar or alternating polarity per impulse) with density control and audio-rate modulation. |
| [`Dust (Trigger)`](https://matthewscharles.github.io/metasound-branches/Dust(Trigger).html) | Generators | Generate randomly timed impulses (unipolar or alternating polarity per impulse) with density control and audio-rate modulation. |
| [`Edge`](https://matthewscharles.github.io/metasound-branches/Edge.html) | Envelopes | Detect upward and downward changes in an input audio signal, with optional debounce. |
| [`EDO`](https://matthewscharles.github.io/metasound-branches/EDO.html) | Tuning | Generate frequencies for tuning systems using equally divided octaves (float) with a MIDI note input. Set a reference frequency and reference MIDI note (defaults to A440). |
| [`Impulse`](https://matthewscharles.github.io/metasound-branches/Impulse.html) | Generators | Trigger a one-sample impulse (unipolar or alternating polarity per impulse). |
| [`Phase Disperser`](https://matthewscharles.github.io/metasound-branches/PhaseDisperser.html) | Filters | A chain of allpass filters acting as a phase disperser to soften transients. |
| [`Shift Register`](https://matthewscharles.github.io/metasound-branches/ShiftRegister.html) | Modulation | An eight-stage shift register for floats. |
| [`Slew (Audio)`](https://matthewscharles.github.io/metasound-branches/Slew(Audio).html) | Filters | A slew rate limiter to smooth out the rise and fall times of an audio signal. |
| [`Slew (Float)`](https://matthewscharles.github.io/metasound-branches/Slew(Float).html) | Filters | A slew limiter to smooth out the rise and fall times of a float value. |
| [`Stereo Balance`](https://matthewscharles.github.io/metasound-branches/StereoBalance.html) | Spatialization | Adjust the balance of a stereo signal. |
| [`Stereo Crossfade`](https://matthewscharles.github.io/metasound-branches/StereoCrossfade.html) | Envelopes | Crossfade between two stereo signals. |
| [`Stereo Gain`](https://matthewscharles.github.io/metasound-branches/StereoGain.html) | Mix | Adjust gain for a stereo signal. |
| [`Stereo Inverter`](https://matthewscharles.github.io/metasound-branches/StereoInverter.html) | Spatialization | Invert and/or swap stereo channels. |
| [`Stereo Width`](https://matthewscharles.github.io/metasound-branches/StereoWidth.html) | Spatialization | Stereo width adjustment (0-200%), using mid-side processing. |
| [`Tuning`](https://matthewscharles.github.io/metasound-branches/Tuning.html) | Tuning | Quantize a float value to a custom 12-note tuning, with adjustment in cents per-note. |
| [`Zero Crossing`](https://matthewscharles.github.io/metasound-branches/ZeroCrossing.html) | Envelopes | Detect zero crossings in an input audio signal, with optional debounce. |

Upon installing the plugin, these items will appear in the sub-category `Branches` within the Metasound `Functions` category.

## Installation

*Please note that this is an experimental version and the node names are subject to change.  Connections may need to be re-drawn upon upgrading to the next version.*

### Downloadable binaries
- Download for Windows and Mac here: [Releases](https://github.com/matthewscharles/metasound-branches/releases/)
- Extract the contents of the zip file into the plugins folder of your project or engine as preferred.

### Building from source
- Clone the repository into your chosen Unreal Engine project's `Plugins` directory.
- Build the project using the [Unreal Automation Tool](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-automation-tool-for-unreal-engine):

#### Windows
- In PowerShell, navigate to the `Engine\Build\BatchFiles\` folder, and run: 
```PowerShell
.\RunUAT.bat BuildPlugin -plugin="<path to your project>\Plugins\metasound-branches\MetasoundBranches.uplugin" -package="<path to your project>\Plugins\metasound-branches\"
```
#### Mac
- In terminal, navigate to the `Engine/Build/BatchFiles/` folder, and run: 
```Bash
sudo ./RunUAT.sh BuildPlugin -plugin="<path to your project>/Plugins/metasound-branches/MetasoundBranches.uplugin" -package="<path to your project>/Plugins/metasound-branches/"
```

Please see [Epic Games: Building Plugins](https://dev.epicgames.com/community/learning/tutorials/qz93/unreal-engine-building-plugins) for more information.

## Documentation
- [MetaSound Branches manual](https://matthewscharles.github.io/metasound-branches/): currently a list of nodes with inlets and outlets, with more coming soon.
- [Examples](https://github.com/matthewscharles/MetaSound-Examples): work in progress -- UAsset files available for download without any written explanation.
- [Demonstration Video](https://www.youtube.com/watch?v=HgudP5fzDWM): demo patch and examples.
### Development notes
- [Initial notes](./docs/development_notes.md): information on the first custom node and development process (now a little out of date).
- [Quick reference](./docs/quick_reference.md): some of my notes for creating and troubleshooting custom nodes. 

---

## Links
- [MetaSounds Reference Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/metasounds-reference-guide-in-unreal-engine)
- [MetaSounds Tutorial videos](https://dev.epicgames.com/community/learning/recommended-community-tutorial/Kw7l/unreal-engine-metasounds) (Matt Spendlove)
- [Writing a Pitch Shift MetaSound Node](https://dev.epicgames.com/community/learning/tutorials/KJWk/writing-a-pitch-shift-metasound-node) (Aaron McLeran)
- [Creating MetaSound Nodes in C++ Quickstart](https://dev.epicgames.com/community/learning/tutorials/ry7p/unreal-engine-creating-metasound-nodes-in-c-quickstart) (Anna Lantz)