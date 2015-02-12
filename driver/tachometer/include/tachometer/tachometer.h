/*
 * tachometer.h
 *
 *  Created on: Jan 26, 2015
 *      Author: eadf
 */

#ifndef TACHOMETER_INCLUDE_TACHOMETER_TACHOMETER_H_
#define TACHOMETER_INCLUDE_TACHOMETER_TACHOMETER_H_


uint32_t tachometer_getSample(void);
void tachometer_init(uint8_t ioPin);

#endif /* TACHOMETER_INCLUDE_TACHOMETER_TACHOMETER_H_ */
