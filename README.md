# Metasound-Plugins 
This repo contains some custom nodes for UE5 Metasounds, which I hope to eventually release as a plugin (working title "Branches").

## Documentation
- [Initial notes](./docs/README.md): information on the first custom node and development process.

## MetasoundBranches
There are currently two test nodes available in the MetasoundBranches plugin, with several more in development:

| Node      | Description                                                  |
|-----------|--------------------------------------------------------------|
| `Dust`    | A randomly timed impulse generator with an audio-rate density control.|
| `SaH`     | An alternative sample and hold unit with an audio-rate trigger. |
| `SaH Bank`| A bank of four identical sample and hold units.              |

Upon installing the plugin, these items will appear in a *Branches* subfolder within the Metasound *Functions* category.

![Signal flow showing a sample and hold node connected to two sources at audio rate](./docs/SaH_demo.png)

## Installation
- Clone the repository into the Unreal Engine project's `Plugins` directory.
- Build the project using the Unreal Automation Tool:
-- Navigate to the `Engine\Build\BatchFiles\` folder, and run: 
```PowerShell
.\RunUAT.bat BuildPlugin -plugin="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin" -package="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin"
```

Please see [Learning / Building Plugins](https://dev.epicgames.com/community/learning/tutorials/qz93/unreal-engine-building-plugins) for more information.
---
## Links
- [Metasounds Tutorial videos](https://dev.epicgames.com/community/learning/recommended-community-tutorial/Kw7l/unreal-engine-metasounds)