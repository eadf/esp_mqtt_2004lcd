/* main.c -- MQTT client example
 *
 * Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of Redis nor the names of its contributors may be used
 * to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "ets_sys.h"
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "stdout/stdout.h"
#include "mem.h"
#include "digoleserial/digoleserial.h"

#define user_procTaskPeriod      1000
static volatile os_timer_t loop_timer;

MQTT_Client mqttClient;
static char clientid[66];
static char buffer[]   = "                    ";
static char topic0[]   = "                    ";
static char topic1[]   = "                    ";
static char topic2[]   = "                    ";
static char topic3[]   = "                    ";
static char topicclr[] = "                    ";

static void loop0(void);
static void loop(void);
static void setup(void);

void ICACHE_FLASH_ATTR
wifiConnectCb(uint8_t status) {

  // Use the hex encoded system_get_chip_id() value as
  // an unique topic (that still fits on the screen)
  os_sprintf(clientid, "/%0x", system_get_chip_id());

  if (status == STATION_GOT_IP) {
    MQTT_Connect(&mqttClient);
  }
}

void ICACHE_FLASH_ATTR
mqttConnectedCb(uint32_t *args) {
  MQTT_Client* client = (MQTT_Client*) args;
  INFO("MQTT: Connected! will use %s as MQTT topic \n", clientid);

  MQTT_Subscribe(client, "/lcd0", 0);
  MQTT_Subscribe(client, "/lcd1", 0);
  MQTT_Subscribe(client, "/lcd2", 0);
  MQTT_Subscribe(client, "/lcd3", 0);
  MQTT_Subscribe(client, "/lcd/clearscreen", 0);
  os_sprintf(topic0, "%s%s", clientid, "/lcd0");
  MQTT_Subscribe(client, topic0, 0);
  os_sprintf(topic1, "%s%s", clientid, "/lcd1");
  MQTT_Subscribe(client, topic1, 0);
  os_sprintf(topic2, "%s%s", clientid, "/lcd2");
  MQTT_Subscribe(client, topic2, 0);
  os_sprintf(topic3, "%s%s", clientid, "/lcd3");
  MQTT_Subscribe(client, topic3, 0);
  os_sprintf(topicclr, "%s%s", clientid, "/clearscreen");
  MQTT_Subscribe(client, topicclr, 0);
}

void ICACHE_FLASH_ATTR
mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len) {

  char *topicBuf = (char*) os_zalloc(topic_len + 1), *dataBuf =
      (char*) os_zalloc(data_len + 1);

  MQTT_Client* client = (MQTT_Client*) args;

  os_memcpy(topicBuf, topic, topic_len);
  topicBuf[topic_len] = 0;

  os_memcpy(dataBuf, data, data_len);
  dataBuf[data_len] = 0;

  INFO("Received topic: %s, data: %s \n", topicBuf, dataBuf);
  if (strcmp(topicBuf, "/lcd0") == 0 || strcmp(topicBuf, topic0) == 0) {
    digoleserial_gotoXY(0,0);
    digoleserial_lcdPrintN(dataBuf, data_len>20?20:data_len);
  } else if (strcmp(topicBuf, "/lcd1") == 0 || strcmp(topicBuf, topic1) == 0 ) {
    digoleserial_gotoXY(0,1);
    digoleserial_lcdPrintN(dataBuf, data_len>20?20:data_len);
  } else if (strcmp(topicBuf, "/lcd2") == 0 || strcmp(topicBuf, topic2) == 0) {
    digoleserial_gotoXY(0,2);
    digoleserial_lcdPrintN(dataBuf, data_len>20?20:data_len);
  } else if (strcmp(topicBuf, "/lcd3") == 0 || strcmp(topicBuf, topic3) == 0) {
    digoleserial_gotoXY(0,3);
    digoleserial_lcdPrintN(dataBuf, data_len>20?20:data_len);
  } else if (strcmp(topicBuf, "/lcd/clearscreen") == 0 || strcmp(topicBuf, topicclr) == 0) {
    digoleserial_lcdClear();
  }
  os_free(topicBuf);
  os_free(dataBuf);
}

/**
 * This is the 'first few seconds' user program loop
 */
static void ICACHE_FLASH_ATTR
loop0(void) {
  static uint8_t iterations = 0;
  if (iterations<=4 ) {
    digoleserial_lcdClear();
    digoleserial_gotoXY(0,0);
    digoleserial_lcdPrint("Digole serial driver");
    digoleserial_gotoXY(4,1);
    digoleserial_lcdPrint("for esp8266");
    digoleserial_gotoXY(2,2);
    digoleserial_lcdPrint("github.com/eadf/  ");
    digoleserial_gotoXY(0,3);
    digoleserial_lcdPrint("esp8266_digoleserial");

    // restart loop0 timer
    os_timer_disarm(&loop_timer);
    os_timer_setfn(&loop_timer, (os_timer_func_t *) loop0, NULL);
    os_timer_arm(&loop_timer, 1000, 0);
  } else if (iterations<=8 ) {
    digoleserial_lcdClear();
    digoleserial_gotoXY(0,0);
    digoleserial_lcdPrint("   Wireless MQTT    ");
    digoleserial_gotoXY(0,1);
    digoleserial_lcdPrint("    for esp8266     ");
    digoleserial_gotoXY(0,2);
    digoleserial_lcdPrint(" github.com/tuanpmt ");
    digoleserial_gotoXY(0,3);
    digoleserial_lcdPrint("    /esp_mqtt       ");

    // restart loop0 timer
    os_timer_disarm(&loop_timer);
    os_timer_setfn(&loop_timer, (os_timer_func_t *) loop0, NULL);
    os_timer_arm(&loop_timer, 1000, 0);
  } else {
    digoleserial_lcdClear();
    loop();
    digoleserial_gotoXY(0,0);
    digoleserial_lcdPrint(clientid);
    digoleserial_lcdPrint("/lcd0");
    digoleserial_gotoXY(0,1);
    digoleserial_lcdPrint(clientid);
    digoleserial_lcdPrint("/lcd1");
    digoleserial_gotoXY(0,2);
    digoleserial_lcdPrint(clientid);
    digoleserial_lcdPrint("/lcd2");
    digoleserial_gotoXY(0,3);
    digoleserial_lcdPrint(clientid);
    digoleserial_lcdPrint("/lcd3");

    // Start loop timer, repeating this time
    os_timer_disarm(&loop_timer);
    os_timer_setfn(&loop_timer, (os_timer_func_t *) loop, NULL);
    os_timer_arm(&loop_timer, 1000, 1);
  }
  iterations += 1;
}

/**
 * This is the main user program loop
 */
static void ICACHE_FLASH_ATTR
loop(void) {
  static uint8_t iterations = 0;

  // toggle the heartbeat symbol
  digoleserial_gotoXY(19,3);
  os_sprintf(buffer,"%c",iterations&1?0b10100001:0b11011111);
  digoleserial_lcdPrint(buffer);
  iterations += 1;
}

/**
 * Setup program. When user_init runs the debug printouts will not always
 * show on the serial console. So i run the inits in here, 2 seconds later.
 */
static void ICACHE_FLASH_ATTR
setup(void) {

  CFG_Load();

  MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);
  MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);

  MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
  MQTT_OnConnected(&mqttClient, mqttConnectedCb);
  MQTT_OnData(&mqttClient, mqttDataCb);

  WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);

  // setup lcd
  digoleserial_init(20,4);
  digoleserial_lcdClear();
  digoleserial_enableCursor(false);
  // Start loop timer
  os_timer_disarm(&loop_timer);
  os_timer_setfn(&loop_timer, (os_timer_func_t *) loop0, NULL);
  os_timer_arm(&loop_timer, 1000, 0);
}

void ICACHE_FLASH_ATTR
user_init(void) {

  // Make uart0 work with just the TX pin. Baud:115200,n,8,1
  // The RX pin is now free for GPIO use.
  stdout_init();

  // Run setup() 2 seconds from now
  os_timer_disarm(&loop_timer);
  os_timer_setfn(&loop_timer, (os_timer_func_t *) setup, NULL);
  os_timer_arm(&loop_timer, 3000, 0);

  INFO("\nSystem started ...\n");
}
