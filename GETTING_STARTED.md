# Getting Started
- [Setting up the environment](#setting-up-the-environment)
- [Compiling](#compiling)
- [Running and developing the ZSWatch SW without the actual ZSWatch HW](#running-and-developing-the-zswatch-sw-without-the-actual-zswatch-hw)
  * [Native Posix](#1-native-posix)
  * [Native Posix + dev kit dongle](#2-native-posix--dev-kit-dongle)
  * [nRF5340 dev kit](#3-nrf5340-dev-kit)
- [Getting Gadgetbridge setup](#getting-gadgetbridge-setup)
  * [Pairing](#pairing)
  * [Weather](#weather)

If you have received or built a ZSWatch there are a few things you need to know before starting.

1. On the dock v1 Rev 1 the watch connector is rotated 180 degrees. Meaning you need to connect the watch 180 degree rotated (see image below).
2. Be careful when connecting the watch to the dock.
    - Check orientation.
    - Check that the pins are not "offsetted".
<div align="center">

<img src=".github/dock_connect.jpg" width="50%"/>
<br>
<sub>
  Note the usage for the dock v1 Rev1.
</sub>
</div>


## Setting up the environment
Download and install the tools needed for flashing.
- [SEGGER J-Link](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack)
- [nRF Commond line tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools/download)

Two options, either set up toolchain and everything by following [Zephyr Getting Started Guide](https://docs.zephyrproject.org/3.2.0/develop/getting_started/index.html) or you can use the in my opinion easier approch by using the [Nordic Toolchain manager](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started/assistant.html). 
Everything works with both Zephyr and with nRF Connect (Nordic Semi. Zephyr fork). If you are new to Zephyr I suggest installing using Nordic Toolchain manager together with the nRF Connect VSCode plugin as I think that is a bit easier.

*Tested with both*
- Zephyr 3.4.0
- nRF Connect SDK 2.4.0

## Cloning source code
Make sure you have enough space and clone the source code:
```
git clone git@github.com:jakkra/ZSWatch.git
git submodule update --init --recursive
```

## Building and Flashing

There to approach to deal with Zephyr based projects:
- [Using the nRF Connect extension for VSCode](#using-the-nrf-connect-extension-for-vscode)
- [Using the command line](#using-the-command-line)

### Using the nRF Connect extension for VSCode
To be able to build, flash and debug via VSCode please install [nRF Connect for VS Code Extension Pack](https://marketplace.visualstudio.com/items?itemName=nordic-semiconductor.nrf-connect-extension-pack).   
[Here](https://nrfconnect.github.io/vscode-nrf-connect/get_started/build_app_ncs.html) you can also find a manual on how to deal with the nRF Connect extension.
Follow the steps below to open and build the ZSWatch application:
- Press "Open an existing application" under the "WELCOME" field of nRF Connect extension. 
- Create new build configuration by clicking on "No build configuration" field in "APPLICATIONS" window. 
- Choose `zswatch_nrf5340_cpuapp` as the board and nRF Connect SDK 2.4.0.
- Set revision to 1 or 2 depending on what version of ZSWatch is used. If your watch is built before Aug. 1 2023 it's revision 1. Revision 2 adds external flash.
- Press "Add fragment" under the "Kconfig fragments" field, here choose either debug (for developing) or release (for daily use).
- Press "Build configuration" button.


### Using the command line
The second option is to use a command line to build and flash Zephyr applications. [Here](https://nrfconnect.github.io/vscode-nrf-connect/get_started/build_app_ncs.html) you can find some more information.   
Example of building for ZSWatch board:
```
west build --board zswatch_nrf5340_cpuapp@1 -- -DOVERLAY_CONFIG="boards/{debug/release}.conf"
west flash
```

__NOTE (Zephyr only)__
<br>
Since the nrf5340 is a dual core microcontroller where the second core is designed to serve the BLE stack, the second image needs to be flashed for BLE operation.    
If you are building with Zephyr you need in addition manually compile and flash the `zephyr/samples/bluetooth/hci_rpmsg` sample and flash that to the NET core. With nRF Connect this is done automatically thanks to the `child_image/hci_rpmsg.conf`. For convenience I have also uploaded a pre-compiled [hex image for NET CPU](app/child_image/GENERATED_CP_NETWORK_merged_domains.hex) if you don't want to recompile it yourself. Flash it using following:
<br>
`nrfjprog -f NRF53 --coprocessor CP_NETWORK --program app/child_image/GENERATED_CP_NETWORK_merged_domains.hex --chiperase`

To build the NET core image:
Command line: 
- Navigate to `zephyr/samples/bluetooth/hci_rpmsg`
- Build using `west build --board zswatch_nrf5340_cpunet@1 -- -DBOARD_ROOT=<ZSWatch absolute path>/app  -DOVERLAY_CONFIG=nrf5340_cpunet_df-bt_ll_sw_split.conf`
- `west flash`
- This only needs to be done once, unless you do a full erase or recover of the nRF5340, which you typically don't do.

VScode:
- Add `zephyr/samples/bluetooth/hci_rpmsg` as an application.
- Select `zswatch_nrf5340_cpunet` as board (VSCode should pick this one up automatically if you added the ZSWatch application earlier).
- Set revision to 1 or 2 depending on what version of ZSWatch is used. If your watch is built before Aug. 1 2023 it's revision 1. Revision 2 adds external flash.
- Press `Add Fragment` under the "Kconfig fragments" field and select the `nrf5340_cpunet_df-bt_ll_sw_split.conf`
- Done, press `Build Configuration`.

## Running and developing the ZSWatch SW without the actual ZSWatch HW
Depending on preference and available hardware, three options can be chosen:
1. [Native Posix](#1-native-posix)
2. [Native Posix + dev kit dongle](#2-native-posix--dev-kit-dongle)
3. [nRF5340 dev kit](#3-nrf5340-dev-kit)

### 1. Native Posix
This option applicable if you host computer hardware have build-in bluetooth module and your host machine is Linux. This option does not require any hardware at all sine Zephyr support BlueZ([details](https://docs.zephyrproject.org/latest/connectivity/bluetooth/bluetooth-tools.html)) and also can emulate display peripheral.

#### Preparation
The "Display driver" emulator need to be installed ([Learn more](https://docs.zephyrproject.org/latest/boards/posix/native_posix/doc/index.html#peripherals) about nativ_posix peripherals):
```
sudo apt-get install pkg-config libsdl2-dev:i386
export PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig
```
To execute ZSWatch application on native posix fist make sure that you have [required](https://docs.zephyrproject.org/latest/connectivity/bluetooth/bluetooth-tools.html#using-bluez-with-zephyr) version of linux kernel and BlueZ.    
Find a HCI index on your host using: `sudo hcitool dev` command and try to execute some zephyr samples located in *<Zephyr base>/zephyr/samples/bluetooth/...*. Example:
```
cd <Zephyr base>/zephyr/samples/bluetooth/peripheral_hr
west build -b native_posix
sudo btmgmt --index 0 power off
sudo ./build/zephyr/zephyr.exe --bt-dev=hci0
```
If everything was successful you will see the message "Advertising successfully started" in the logs and you will be able to connect to the device from your phone using e.g. [nRF Connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=nl&pli=1) application.


#### Running ZSWatch app
To build ZSWatch application for native posix simply run:
```
cd <ZSWatch path>/app
west build -b native_posix
sudo btmgmt --index <hci index> power off
sudo ./build/zephyr/zephyr.exe --bt-dev=hci<hci index>
```

__Tips:__
1. If you want to be able to debug: `sudo gdb -ex=r --args build/zephyr/zephyr.exe --bt-dev=hci<hci index>`
2. If you want to scale up the SDL window (4x) apply the patch in `app/zephyr_patches/sdl_upscale.patch`

https://github.com/jakkra/ZSWatch/assets/4318648/3b3e4831-a217-45a9-8b90-7b48cea7647e


### 2. Native Posix + dev-kit dongle
In case there is no built-in Bluetooth module on the host computer, an external nRF dev kit can be used as a BLE module. In fact, any external BLE module that supports the HCI interface can be used. In doing so, the application will run on the host machine and communicate with BLE controller over hci_usb/hci_uart depending on the hardware you have.

#### Preparation
Compile and flash the *zephyr/samples/bluetooth/hci_usb* application with following additions to prj.conf:
```
CONFIG_BT_EXT_ADV=y
CONFIG_BT_PER_ADV=y
CONFIG_BT_PER_ADV_SYNC=y
CONFIG_BT_PER_ADV_SYNC_MAX=2
```
**NOTE:** If hci_uart is used, a new HCI port must be attached, follow this [guide](https://docs.zephyrproject.org/latest/samples/bluetooth/hci_uart/README.html#using-the-controller-with-qemu-and-native-posix). Alternatively in case of using hci_usb you don't need to attach new HCI port, just physically connect USB to nRF USB port.

Make sure that new hci device appear using: `sudo hcitool dev`

Next follow the [Preparation](#preparation) to install the "Display driver" emulator and the [Running ZSWatch app](#running-zswatch-app) instruction to execute the application.


### 3. nRF5340 dev kit
This is possible, what you need is a [nRF5340-DK](https://www.digikey.se/en/products/detail/nordic-semiconductor-asa/NRF5340-DK/13544603) (or EVK-NORA-B1) and a breakout of the screen I use [https://www.waveshare.com/1.28inch-touch-lcd.htm](https://www.waveshare.com/1.28inch-touch-lcd.htm).
<br>
You may also add _any_ of the sensors on the ZSWatch, Sparkfun for example have them all:<br>
[BMI270](https://www.sparkfun.com/products/17353)
[BME688](https://www.sparkfun.com/products/19096)
[BMP581](https://www.sparkfun.com/products/20170)
[MAX30101](https://www.sparkfun.com/products/16474)
[LIS2MDL](https://www.sparkfun.com/products/19851)

When using the nRF5340-DK all you need to do is to replace `zswatch_nrf5340_cpuapp` with `nrf5340dk_nrf5340_cpuapp` as the board in the compiling instructions above. You may also need to tweak the pin assignment in [app/boards/nrf5340dk_nrf5340_cpuapp.overlay](app/boards/nrf5340dk_nrf5340_cpuapp.overlay) for your needs.

## Getting Gadgetbridge setup
Install the Android app [GadgetBridge](https://codeberg.org/Freeyourgadget) or [from Play Store here](https://play.google.com/store/apps/details?id=com.espruino.gadgetbridge.banglejs&hl=en_US)
- In Gadgetbridge press plus button to add ZSWatch
- Enable "Discover unsupported devices" option and set "Scanning intensity" to maximum in "Discover and pair options"
- It will scan and you should see a device called ZSWatch, long press it.
- Select in the dropdown Bangle.js as the device.

### Pairing
To get communication with your phone working you need to pair ZSWatch.
- In ZSWatch go to Settings -> Bluetooth -> Enable pairing.
- Now go **reconnect** to the watch from Gadgetbridge app.
- You should now be paired and a popup should be seen on ZSWatch.

### Weather
To get weather working follow the instructions [here](https://codeberg.org/Freeyourgadget/Gadgetbridge/wiki/Weather).