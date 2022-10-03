# Flight Sensor Firmware/Software Repository

## Overview
- `common`: Shared common source used by multiple projects
- `flight-recorder`:  Python tool to interface to the flight sensor firmware over BLE
- `libs`: external dependencies used by the build
- `nRF5_SDK`: Nordic SDK 17.3
- `projects`: All firmware applications
    - `flash-dev`: project used by flash debugging/development
    - `flight-sensor`: application used to record IMU data and playback saved data over BLE


## Build Instructions

### Downloads
- GNU ARM Embedded Toolchain [here](https://developer.arm.com/downloads/-/gnu-rm) (Version: v10.3.1 10 2021)
- Segger J-Link Drivers [here](https://www.segger.com/downloads/jlink/) (Version V7.58b)
- Nordic Command Line Tools [here](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools/download)
- Visual Studio Code [here](https://code.visualstudio.com/)

<br />

### Installs
**GNU ARM Embedded Toolchain**
- Verify that correct installer version is downloaded
- Run the installer using default options
- Ensure that the install path is here: `C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10`

**Segger J-Link Drivers**
- Run the installer using default options

**Nordic Command Line Tools**
- Run the installer using default options

**Visual Studio Code**
- Run the installer using default options

**Make**
- A copy of the make execute is located in `/libs`. Copy this to your local directory and add to path.
- Run `make --version` to verify that it has been correctly added to your path.

<br />

### Fimware Build Instructions

**Building** 
1. Navigate to a project. Example: `projects/flight-sensor`
2. Run `make setup` to install necessary python packages
3. Run `make` to build the firmware
4. For Softdevice projects, run `make flash_softdevice` to program the softdevice
5. Run `make flash` to program the firmware application

**Config Editor**
1. Run `make sdk_config` to run the config editor

<br />

### VSCode Setup Instructions

**C/C++ Properties**
1. Open the root directory in VS Code
1. Select the Extensions view icon on the Activity bar
1. Search for and install `C/C++`
1. Open the command palette (`CTRL`+`SHIFT`+`P`) and run `>C/C++: Edit Configurations (JSON)` to generate a `c_cpp_properties.json` file.
1. Update `.vscode/c_cpp_properties.json` to have the following settings:
    - `"compilerPath": "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.10/bin/arm-none-eabi-gcc.exe"`
    - `"cStandard": "c11"`
    - `"cppStandard": "c++17"`
    - `"intelliSenseMode": "gcc-arm"`
1. Navigate to a project and run `make vscode`. This will update the `c_cpp_properties.json` file with the correct include paths and defines for the project

**Logging**
1. Under `C:\Program Files (x86)\SEGGER\JLink`, run `JLinkRTTViewer.exe`
1. The log viewer can be buggy, use `F3` to disconnect and `F2` to connect after programming the device 


**Debugger**
1. Select the Extensions view icon on the Activity bar
2. Search for and install `C/C++`
3. Under the `.vscode` directory create a file, `launch.json`
4. Update the file with the following text:
<pre><code>{
    "version": "0.2.0",
    "configurations": [
      {
      "name": "debug",
      "type": "cortex-debug",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "executable": "${workspaceRoot}/projects/flight-sensor/_build/nrf52840_xxaa.out",
      "serverpath": "C:/Program Files (x86)/SEGGER/JLink/JlinkGDBServerCL.exe",
      "armToolchainPath": "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.10/bin",
      "servertype": "jlink",
      "device": "nrf52840_xxaa",
      "interface": "swd",
      "serialNumber": "", //If you have more than one J-Link probe, add the serial number here.
      "jlinkscript":"",
      "svdFile": ""
      }
      ]
}</code></pre>

5. In the `Run` tab, select `Start Debugging` or press `F5` 
