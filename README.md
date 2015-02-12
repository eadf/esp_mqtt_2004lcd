# esp_mqtt_2004lcd

Display mqtt messages on a 20x4 LCD.

I'm pulling in two git subtree projects : [tuanpmt's exellent mqtt project: esp_mqtt](https://github.com/tuanpmt/esp_mqtt) and my lcd project [  esp8266_digoleserial](https://github.com/eadf/esp8266_digoleserial) to build this project.

To use the LCD you will need an esp with GPIO2 available. Digoleserial uses UART1 so the GPIO2 pin is hardwired.

##Usage
To use this mqtt enabled LCD you can send text messages to these topics:
* /lcd0
* /lcd1
* /lcd2
* /lcd3

Representing each row of the display.

You can also send messages to a topic that is unique to each esp, look at the debug console for more info.

###Building and installing:

First you need to install the sdk and the easy way of doing that is to use [esp_open_sdk.](https://github.com/pfalcon/esp-open-sdk)

You can put that anywhere you like (/opt/local/esp-open-sdk, /esptools etc etc)

Then you could create a small setenv.sh file, containing the location of your newly compiled sdk and other platform specific info;
```
export SDK_BASE=/opt/local/esp-open-sdk/sdk
export PATH=${SDK_BASE}/../xtensa-lx106-elf/bin:${PATH}
export ESPPORT=/dev/ttyO0  
```
(or setup your IDE to do the same)

In the root of this project create a soft link Makefile -> Makefile.[mac,linux]

To make a clean build, flash and connect to the esp console you just do this in a shell:
```
source setenv.sh # This is only needed once per session
make clean && make test
```

You won't be needing esptool, my makefiles only uses esptool.py (provided by esp-open-sdk)

I have tested this with sdk v0.9.5 and v0.9.4 (linux & mac makefile)
