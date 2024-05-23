This is an archived branch that uses different approach for background transitions together with an attempt for sprite caching.

# VisualU
Plugin for Unreal Engine 5 that brings core functionality for a widget-based visual novel game

# Setup
Build the plugin for the Unreal Engine 5 project (should be also compatible with Unreal Engine 4 after recompiling)

## Build Plugin using Unreal Automation Tool (Windows)
1. Go to ```UE_5.0\Engine\Build\BatchFiles```
2. Type ```powershell``` into File Explorer's address tab
3. In the opened Powershell window, type the following command: ```./RunUAT.bat BuildPlugin -plugin="<path_to_plugin>\VisualU.uplugin" -package="C:\Users\$Env:Username\Desktop\Package"```

## Install Plugin for the project
1. Open project's directory for which you want to install the plugin
2. Create ```Plugins``` folder, than another folder named ```VisualU``` inside of it
3. Go to ```Desktop\Package```
4. Copy everything and paste it to the newly created ```Plugins\VisualU``` folder inside project's directory

# Usage

Check [API documentation!](https://razdvizh.github.io/VisualU/index.html)
Check [VisualU Wiki!](https://github.com/Razdvizh/VisualU/wiki)
