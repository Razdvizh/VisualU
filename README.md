# VisualU
Plugin for Unreal Engine 5.6 that brings core functionality for a widget-based visual novel narrative.

# Setup
Build the plugin for the Unreal Engine 5.6.x project

## Build Plugin using Unreal Automation Tool (Windows)
1. Go to ```<engine_directory>\Engine\Build\BatchFiles```.
2. Type ```powershell``` into File Explorer's address tab.
3. In the opened Powershell window, type the following command: ```./RunUAT.bat BuildPlugin -plugin="<path_to_plugin>\VisualU.uplugin" -package="C:\Users\$Env:Username\Desktop\Package"```
4. Packaged plugin can be saved to any other folder, Desktop is used for example purposes.

## Install Plugin for the project
1. Open project's directory for which you want to install the plugin.
2. Create ```Plugins``` folder, than another folder named ```VisualU``` inside of it.
3. Go to ```Desktop\Package``` or the folder that was during step 3 of build phase.
4. Copy everything and paste it to the newly created ```Plugins\VisualU``` folder inside project's directory.

# Usage
Check [API documentation!](https://razdvizh.github.io/VisualU/index.html)
Check [VisualU Wiki!](https://github.com/Razdvizh/VisualU/wiki)
Check [Discord Server!](https://discord.gg/9FcxEdgXP7)
