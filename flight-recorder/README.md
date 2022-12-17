# Flight Recorder Tool

## Build Instructions

### Downloads
- Python `3.9+` [here](https://www.python.org/downloads/release/python-3912/)
- PyCharm Community Editor [here](https://www.jetbrains.com/pycharm/download/#section=windows)

### Installs
**Python**
1. Run the python 3.9+ installer downloaded earlier using default settings
 - On first page make sure `Add Python 3.9 to PATH` is checked
 - Click Install

**PyCharm**
- Run the installer using default options

### Setup

**PyCharm** 
1. Run Pycharm
1. Open the directory: `flight-sensor/flight-recorder`
1. Update the interpreter:
    - Navigate to File > Settings
    - Under Project: `flight-recorder > Python Interpreter`:
        - From the Python Interpreter drop down menu, select `Show All...`
        - Click `+`
        - Select `Existing Environment`
        - In the Interpreter drop down menu `C:\Users\<username>\.virtualenvs\flight-recorder\scripts\python.exe` should be selected
        - Click `OK` in all open windows

**BLE Central**
1. Connect a `PCA10056` Nordic Dev Board to the PC using the main USB port
2. On the power switch select `VDD`
3. Navigate to `flight-sensor/flight-recorder/image`
4. Run `nrfjprog --eraseall` to erase the device
5. Run `nrfjprog --program connectivity_4.1.4_usb_with_s132_5.1.0.hex --verify` to program the Nordic connectivity image to the device
6. Connect the USB cable to the side USB port labeled `nRF USB`
7. In `Device Manager` on your PC, verify `Nordic Semiconductor DFU` appears

<br />

### Data Collection

**Stream**: connects to the sensor and record live orientation samples to a session file.

**Record**: commands the sensor to start saving orientation samples to local flash. Use playback to retrieve samples. 

**Playback**: streams previously recorded data

**Calibrate**: executes the calibration routine (about 15 seconds). Keep the device flat throughout the calibration.

### Analysis

**Sensor Fusion**: Plots euler angles of last stream/playback session

### Visualization

**Display**: starts a stream and displays live animation

