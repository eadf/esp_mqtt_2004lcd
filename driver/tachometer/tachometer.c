/*
 * tachometer.c
 *
 *  Created on: Jan 26, 2015
 *      Author: eadf
 */
#include "osapi.h"
#include "ets_sys.h"
#include "gpio.h"
#include "mem.h"
#include "tachometer/tachometer.h"
#include "easygpio/easygpio.h"
#include "os_type.h"

#define TACHOMETER_POLL_TIME 500 // 500ms

static volatile uint32_t   tachometer_timeStamp = 0;
static volatile uint32_t   tachometer_pulses = 0;
static volatile uint32_t   tachometer_sample = 0;
static volatile os_timer_t tachometer_timer;
static uint8_t tachometer_pin = 0;

// forward declarations
static void tachometer_disableInterrupt(void);
static void tachometer_enableInterrupt(void);
static void tachometer_intr_handler(int8_t key);
static void tachometer_timerFunc(void);

static void
tachometer_disableInterrupt(void) {
  gpio_pin_intr_state_set(GPIO_ID_PIN(tachometer_pin), GPIO_PIN_INTR_DISABLE);
}

static void
tachometer_enableInterrupt(void) {
  gpio_pin_intr_state_set(GPIO_ID_PIN(tachometer_pin), GPIO_PIN_INTR_POSEDGE);
}

static void
tachometer_intr_handler(int8_t key) {
  uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
  //clear interrupt status
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(tachometer_pin));
  tachometer_pulses+=1;
}

uint32_t ICACHE_FLASH_ATTR
tachometer_getSample(void) {
   return tachometer_sample;
}

void ICACHE_FLASH_ATTR
tachometer_timerFunc(void) {
  static uint16 counter = 0;

  // save the state as 'atomic' as possible
  //tachometer_disableInterrupt();
  uint32_t now = system_get_time();
  uint32_t prevTimeStamp = tachometer_timeStamp;
  uint32_t pulses = tachometer_pulses;
  tachometer_timeStamp = now;
  tachometer_pulses = 0;
  //tachometer_enableInterrupt();

  int32 period = now - prevTimeStamp;
  bool aBit = GPIO_INPUT_GET(tachometer_pin);
  if (period>0){
    tachometer_sample = (1000000.0*(float)pulses)/(float)period;
    if (false && (counter%3 == 0)) {
      // print this every 4:th iteration
      os_printf("Tachometer: pulses: %d period:%d us ", pulses, period);
      os_printf("pinValue:%c tachometer_sample=%d\n",aBit?'1':'0', tachometer_sample);
    }
  }
  counter += 1;
}

void ICACHE_FLASH_ATTR
tachometer_init(uint8_t ioPin) {
  tachometer_pin = ioPin;

  //Disarm timer
  os_timer_disarm(&tachometer_timer);
  os_timer_setfn(&tachometer_timer, (os_timer_func_t *) tachometer_timerFunc, NULL);

  if (easygpio_attachInterrupt(tachometer_pin, EASYGPIO_NOPULL, tachometer_intr_handler)) {
    // start the poll/sample timer
    os_timer_arm(&tachometer_timer, TACHOMETER_POLL_TIME, 1);
    tachometer_enableInterrupt();
  } else {
    os_printf("tachometer_init failed to set interrupt\n");
  }
}
