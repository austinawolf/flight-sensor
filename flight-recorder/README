# Flight Recorder Tool

## Build Instructions

### Downloads
- Python `3.9+` [here](https://www.python.org/downloads/release/python-3912/)
- PyCharm Community Editor [here](https://www.jetbrains.com/pycharm/download/#section=windows) 

<br />

### Installs
**Python**
1. Run the python 3.9+ installer downloaded earlier using default settings
 - On first page make sure `Add Python 3.9 to PATH` is checked
 - Click Install

**PyCharm**
- Run the installer using default options

<br />

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
        - In the Interpreter drop down menu `C:\Users\<username>\.virtualenvs\flitht-recorder\scripts\python.exe` should be selected
        - Click `OK` in all open windows

**BLE Central**
1. Connect a `PCA10056` Nordic Dev Board to the PC using the main USB port
2. On the power switch select `VDD`
3. Navigate to `flight-sensor/flight-recorder/image`
4. Run `nrfjprog --eraseall --program connectivity_4.1.4_usb_with_s132_5.1.0.hex --verify` to program the Nordic connectivity image to the device
5. Connect the USB cable to the side USB port labeled `nRF USB`
6. In `Device Manager` on your PC, verify `Nordic Semiconductor DFU` appears

<br />

### Data Collection

**Stream**
- In the project window, right click on `flight-recorder/stream` and select `Run "stream"`

**Record**
- In the project window, right click on `flight-recorder/record` and select `Run "record"`

**Playback**
- In the project window, right click on `flight-recorder/playback` and select `Run "playback"`

**Calibrate**
- In the project window, right click on `flight-recorder/calibrate` and select `Run "calibrate"`
