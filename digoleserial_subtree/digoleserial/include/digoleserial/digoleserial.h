#ifndef DIGOLESERIAL_INCLUDE_DIGOLESERIAL_DIGOLESERIAL_H_
#define DIGOLESERIAL_INCLUDE_DIGOLESERIAL_DIGOLESERIAL_H_

#include "c_types.h"

/**
 * clear lcd
 */
void digoleserial_lcdClear(void);

/**
 * set the mode of cursor
 */
void digoleserial_enableCursor(bool cursorOn);

/**
 * set the mode of cursor
 */
void digoleserial_enableBacklight(bool backlightOn);

/**
 * send command directly to the lcd, bypass adapter
 */
void digoleserial_directCommand(uint8_t d);

/**
 * send data directly to the lcd, bypass adapter
 */
void digoleserial_directData(uint8_t d);

/**
 * Allows us to fill the first 8 CGRAM locations
 * with custom characters
 */
void digoleserial_createChar(uint8_t location, uint8_t charmap[]);

/**
 * Writes a single character
 */
void digoleserial_lcdCharacter(uint8_t character);

/**
 * moves the cursor to x=col, y=row
 */
void digoleserial_gotoXY(uint8_t x, uint8_t y);

void digoleserial_lcdHome(void);
/**
 * set baud to max speed: BIT_RATE_115200
 */
void digoleserial_setBaud(void);

/**
 * Writes an optionally null terminated string
 */
void digoleserial_lcdPrintN(uint8_t *buf, uint16_t len);

/**
 * Writes a null terminated string
 */
void digoleserial_lcdPrint(uint8_t *characters);

/**
 * Writes all of the characters in the buffer - including 0x0
 */
void digoleserial_writeCustomChars(uint8_t *buffer, uint16_t len);

/**
 * initiates the lcd. It can only be connected to GPI02 (uart1)
 */
void digoleserial_init(uint8_t col, uint8_t row);
#endif

