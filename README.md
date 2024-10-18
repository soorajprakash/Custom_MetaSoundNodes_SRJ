# Metasound-Plugins 
Custom nodes for UE5 Metasound, which I hope to eventually package and release as a plugin (working title "Branches").

## Documentation
- [Initial notes](./docs/README.md): some information on the first custom node and development process.

## MetasoundBranches
There is currently one test node available in the MetasoundBranches plugin:

- `sah`: a standard sample and hold unit with an audio rate trigger.

![Signal flow showing a sample and hold node connected to two sources at audio rate](./docs/SaH_demo.png)

## Installation
Please see [Building Plugins](https://dev.epicgames.com/community/learning/tutorials/qz93/unreal-engine-building-plugins) for more information.
So far, I've been building the plugin by following these steps:
- Clone the repository into the Unreal Engine project's `Plugins` directory.
- Build the project using the Unreal Automation Tool:
-- In `Engine\Build\BatchFiles\` run: 
```PowerShell
- .\RunUAT.bat BuildPlugin -plugin="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin" -package="<path to your project>\Plugins\metasound-plugins\MetasoundBranches.uplugin"
```
