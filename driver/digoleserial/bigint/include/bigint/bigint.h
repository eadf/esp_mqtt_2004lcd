/*
 * bigint.h
 *  Ported from an arduino library. Can't find the name of the original author.
 */

#ifndef INCLUDE_BIGINT_H_
#define INCLUDE_BIGINT_H_

void bigint_init(void);
void bigint_print1Digit(uint8_t xPos, uint16_t digit);
void bigint_print2Digits(uint8_t xPos, uint16_t digit);
void bigint_print3Digits(uint8_t xPos, uint16_t digit);
void bigint_print4Digits(uint8_t xPos, uint16_t digit);
void bigint_print5Digits(uint8_t xPos, uint32_t number);

#endif /* INCLUDE_BIGINT_H_ */
