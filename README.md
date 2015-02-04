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

###Required:

esp_iot_sdk_v0.9.4_14_12_19 ( v0.9.5 breaks everything )

Actually, i have not tested this with v0.9.5. I tested a clean sdk 0.9.5 install with one of the basic examples (could have been blinky). It compiled and uploaded fine but the esp had a infinite crash loop with some message about "MEM CHK FAIL" on the console. So i threw the whole sdk out. I will try upgrading the sdk again once [mqtt](https://github.com/tuanpmt/esp_mqtt) upgrades to 0.9.5+.
