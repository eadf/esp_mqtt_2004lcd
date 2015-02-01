/*
 * bigint.c
 *  Ported from an arduino library. Can't find the name of the original author.
 */

#include "digoleserial/digoleserial.h"
#include "bigint/bigint.h"
#include "osapi.h"

/*
 Custom characters: I am using custom characters to build large size characters. These characters are
 9 times (3x3) the regular size. When you are far away using the remote control, you might need
 large characters to see what is being displayed.
 */

// Define 8 custom characters

#define B 0xFF  // The byte for a completely filled box
#define A 0x20  // The byte for blank

static uint8_t cc0[8] = {     // Custom Character 0
    0b00000,
    0b00111,
    0b01111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

static uint8_t cc1[8] = {     // Custom Character 1
    0b11100,
    0b11110,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

static uint8_t cc2[8] = {    // Custom Character 2
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000
};

static uint8_t cc3[8] = {    // Custom Character 3
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

static uint8_t cc4[8] = {   // Custom Character 4
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b01111,
    0b00111
};

static uint8_t cc5[8] = {    // Custom Character 5
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000
};

static uint8_t cc6[8] = {    // Custom Character 6
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000
};

static uint8_t cc7[8] = {     // Custom Character 7
    0b00000,
    0b11100,
    0b11110,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

// Array index into parts of big numbers. Each number consists of 9 custom characters
// in a 3x3 matrix. To print a number, you use the array index corresponding to the number
// times 3. For example to print the number 5, you will print bn1[15], bn1[16] and bn1[17]
// for the first row of the large number, and then bn2[15], bn2[16] and bn2[17] and so on.

//                      0      1      2      3      4      5      6      7      8      9
static uint8_t bn1[]={B,2,1, 2,1,A, 2,2,1, 2,2,1, 0,A,B, B,2,2, 0,2,2, 2,2,B, 0,2,1, 0,2,1};
static uint8_t bn2[]={B,A,B, A,B,A, 0,6,5, A,2,1, 5,6,B, 2,2,1, B,6,7, A,0,5, B,6,B, 4,6,B};
static uint8_t bn3[]={4,3,B, 3,B,3, B,3,3, 4,3,B, A,A,B, 4,3,B, 4,3,B, A,B,A, 4,3,B, A,A,B};

void bigint_printDigit(uint8_t xPos, uint8_t digit, uint8_t line);

void ICACHE_FLASH_ATTR
bigint_printDigit(uint8_t xPos, uint8_t digit, uint8_t line) {
  uint8_t buffer[] = "   ";
  digit = digit % 10;
  switch (line) {
  case 0:
    // Line 1 of the one digit number
    buffer[0] = bn1[digit*3];
    buffer[1] = bn1[digit*3+1];
    buffer[2] = bn1[digit*3+2];
    digoleserial_writeCustomChars(buffer,3);
    break;
  case 1:
    // Line 2 of the one-digit number
    buffer[0] = bn2[digit*3];
    buffer[1] = bn2[digit*3+1];
    buffer[2] = bn2[digit*3+2];
    digoleserial_writeCustomChars(buffer,3);
    break;
  case 2:
    // Line 3 of the one-digit number
    buffer[0] = bn3[digit*3];
    buffer[1] = bn3[digit*3+1];
    buffer[2] = bn3[digit*3+2];
    digoleserial_writeCustomChars(buffer,3);
    break;
  }
}

void ICACHE_FLASH_ATTR
bigint_print1Digit(uint8_t xPos, uint16_t number){
  uint8_t number10e0 = (number%10);
  uint8_t i = 0;
  for (i=0; i<3; i++){
    digoleserial_gotoXY(xPos,i);
    digoleserial_lcdNString("",0);
    bigint_printDigit(xPos+4, number10e0, i);
  }
}

void ICACHE_FLASH_ATTR
bigint_print2Digits(uint8_t xPos, uint16_t number){
  uint8_t number10e1 = (number/10);
  uint8_t number10e0 = (number%10);
  uint8_t i = 0;
  for (i=0; i<3; i++){
    digoleserial_gotoXY(xPos,i);
    digoleserial_lcdNString("",0); // required to make the gotoXY work
    bigint_printDigit(xPos, number10e1, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+4, number10e0, i);
  }
}

void ICACHE_FLASH_ATTR
bigint_print3Digits(uint8_t xPos, uint16_t number){
  uint8_t number10e2 = number/100;
  int rest = number%100;
  uint8_t number10e1 = rest/10;
  uint8_t number10e0 = rest%10;

  uint8_t i = 0;
  //uint8_t buffer[] = " ";
  for (i=0; i<3; i++){
    digoleserial_gotoXY(xPos,i);
    digoleserial_lcdNString("",0); // required to make the gotoXY work
    bigint_printDigit(xPos+0, number10e2, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+4, number10e1, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+8, number10e0, i);
  }
}

void ICACHE_FLASH_ATTR
bigint_print4Digits(uint8_t xPos, uint16_t number){
  uint8_t number10e3 = number/1000;
  int rest = number%1000;
  uint8_t number10e2 = rest/100;
  rest = rest%100;
  uint8_t number10e1 = rest/10;
  uint8_t number10e0 = rest%10;

  uint8_t i = 0;
  for (i=0; i<3; i++){
    digoleserial_gotoXY(xPos,i);
    digoleserial_lcdNString("",0); // required to make the gotoXY work
    bigint_printDigit(xPos+0,  number10e3, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+4,  number10e2, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+8,  number10e1, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+12, number10e0, i);
  }
}

void ICACHE_FLASH_ATTR
bigint_print5Digits(uint8_t xPos, uint32_t number) {
  uint8_t number10e4 = number/10000;
  int rest = number%10000;
  uint8_t number10e3 = rest/1000;
  rest = number%1000;
  uint8_t number10e2 = rest/100;
  rest = rest%100;
  uint8_t number10e1 = rest/10;
  uint8_t number10e0 = rest%10;
  uint8_t i = 0;
  for (i=0; i<3; i++){
    digoleserial_gotoXY(xPos,i);
    digoleserial_lcdNString("",0); // required to make the gotoXY work
    bigint_printDigit(xPos+0,  number10e4, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+4,  number10e3, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+8,  number10e2, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+12,  number10e1, i);
    digoleserial_writeCustomChars(" ",1);
    bigint_printDigit(xPos+16, number10e0, i);
  }
}

// Function to send custom characters to the display's RAM
void ICACHE_FLASH_ATTR
bigint_init(void) {
  digoleserial_createChar(0,cc0);  // cc0 becomes uint8_tacter 0
  digoleserial_createChar(1,cc1);  // cc1 becomes uint8_tacter 1
  digoleserial_createChar(2,cc2);  // cc2 becomes uint8_tacter 2
  digoleserial_createChar(3,cc3);  // cc3 becomes uint8_tacter 3
  digoleserial_createChar(4,cc4);  // cc4 becomes uint8_tacter 4
  digoleserial_createChar(5,cc5);  // cc5 becomes uint8_tacter 5
  digoleserial_createChar(6,cc6);  // cc6 becomes uint8_tacter 6
  digoleserial_createChar(7,cc7);  // cc7 becomes uint8_tacter 7
  digoleserial_createChar(0,cc0);  // cc0 becomes uint8_tacter 0  // do char 0 again, it always fails first time
  digoleserial_enableCursor(false);
}
