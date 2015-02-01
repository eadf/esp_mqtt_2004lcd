#include "ets_sys.h"
#include "osapi.h"
#include "digoleserial/uart.h"
#include "digoleserial/digoleserial.h"
#include "string.h"

#define UART1   1
#undef DEBUG_DIGOLE

// commands
#define LCD_SETCGRAMADDR 0x40
#define LCD_RETURNHOME 0x02

// UartDev is defined and initialized in rom code.
extern UartDevice UartDev;

static void uart1_tx_string(uint8_t *buf);

/******************************************************************************
 * FunctionName : uart_config
 * Description  : Internal used function
 *                UART1 just used for debug output
 * Parameters   : uart_no, use UART0 or UART1 defined ahead
 * Returns      : NONE
 *******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
digoleserial_uart1_config(void) {
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
  //os_delay_us(10000);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);
  uart_div_modify(UART1, UART_CLK_FREQ / (UartDev.baut_rate));
  WRITE_PERI_REG(UART_CONF0(UART1),
      UartDev.exist_parity | UartDev.parity | (UartDev.stop_bits << UART_STOP_BIT_NUM_S) | (UartDev.data_bits << UART_BIT_NUM_S));
  os_delay_us(10000);
}

static STATUS ICACHE_FLASH_ATTR
uart1_tx_one_char(uint8_t TxChar) {
  while (true) {
    uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(UART1))
        & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);
    if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
      break;
    }
  }

  WRITE_PERI_REG(UART_FIFO(UART1), TxChar);
#ifdef DEBUG_DIGOLE
  if (TxChar>=0x20 && TxChar<=0x7E ) {
    os_printf("%c",TxChar);
  } else if (TxChar==0x0D ) {
    os_printf("0x%02X", TxChar);
    os_printf("\n");
  } else {
    os_printf("0x%02X", TxChar);
  }
#endif
  return OK;
}

static void ICACHE_FLASH_ATTR
uart1_tx_string(uint8_t *buf) {
  for (;*buf;buf++){
    uart1_tx_one_char(*buf);
  }
}

void digoleserial_directCommand(uint8_t d) {
  uart1_tx_string("MCD");
  uart1_tx_one_char(d);
}

void digoleserial_directData(uint8_t d) {
  uart1_tx_string("MDT");
  uart1_tx_one_char(d);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void digoleserial_createChar(uint8_t location, uint8_t charmap[]) {
#ifdef DEBUG_DIGOLE
  os_printf("\n#Creating custom font %d on uart1#\n", location);
#endif
  location &= 0x7; // we only have 8 locations 0-7
  digoleserial_directCommand(LCD_SETCGRAMADDR | (location << 3));
  int i=0;
  for (; i<8; i++) {
    digoleserial_directData(charmap[i]);
  }
  os_delay_us(5000);
#ifdef DEBUG_DIGOLE
  os_printf("\n#Created custom font %d on uart1#\n", location);
#endif
}

void ICACHE_FLASH_ATTR
digoleserial_lcdCharacter(uint8_t c) {
  /*if (c == '\n') {
    //uart1_tx_one_char(0x00);
    uart1_tx_string("TRT\n");
    uart1_tx_string("TT");
  } //else if (c == '\r') {
  *///}
  //else {
    uart1_tx_one_char(c);
  //}
}

void ICACHE_FLASH_ATTR
digoleserial_lcdNString(uint8_t *buf, uint16_t len){
  uint16 i;
  uart1_tx_string("TT");
  for (i = 0; i < len; i++) {
    digoleserial_lcdCharacter(buf[i]);
  }
  uart1_tx_one_char(0x0d);
}

void ICACHE_FLASH_ATTR
digoleserial_writeCustomChars(uint8_t *buf, uint16_t len){
  uint16 i;
  if (true){
    for (i = 0; i < len; i++) {
      digoleserial_directData(buf[i]);
    }
  } else {
    uart1_tx_string("TT");
    for (i = 0; i < len; i++) {
      if (buf[i] == 0){
        uart1_tx_one_char(0x00);
        digoleserial_directData(buf[i]);
        uart1_tx_string("TT");
      } else {
        digoleserial_lcdCharacter(buf[i]);
      }
    }
  }
  uart1_tx_one_char(0x00);
}


void ICACHE_FLASH_ATTR
digoleserial_gotoXY(uint8_t x, uint8_t y){
  uart1_tx_string("TP");
  uart1_tx_one_char(x);
  uart1_tx_one_char(y);
  os_delay_us(2000);
}

void ICACHE_FLASH_ATTR
digoleserial_lcdString(uint8_t *buf) {
  digoleserial_lcdNString(buf, strlen(buf));
}

void ICACHE_FLASH_ATTR
digoleserial_enableCursor(bool cursorOn){
  uart1_tx_string(cursorOn?"CS1":"CS0");
  //uart1_tx_one_char(0x0);
}

void ICACHE_FLASH_ATTR
digoleserial_enableBacklight(bool backlightOn){
  uart1_tx_string(backlightOn?"BL1":"BL0");
  uart1_tx_one_char(0x0);
}

void ICACHE_FLASH_ATTR
digoleserial_lcdClear(void) {
  //uart1_tx_one_char(0x0);
  uart1_tx_string("CL");
  //uart1_tx_one_char(0x0d);
  //uart1_tx_one_char(0x0);
  os_delay_us(2000);  // this command takes a long time!
}

void ICACHE_FLASH_ATTR
digoleserial_lcdHome(void) {
  digoleserial_directCommand(LCD_RETURNHOME);  // set cursor position to zero
  os_delay_us(2000);  // this command takes a long time!
}

void ICACHE_FLASH_ATTR
digoleserial_setBaud(void){
  if (false) { // setBaud does NOT work!!!

    //digoleserial_lcdNString("Switching to 115200", 18);
    os_printf("\n#Switching to 115200 on uart1#\n");
    //uart1_tx_one_char(0x00);
    digoleserial_lcdNString("", 0); //Sync up
    digoleserial_lcdNString("", 0); //Sync up
    //digoleserial_lcdClear();
    uart1_tx_string("SB5115200\n");
    uart1_tx_one_char(0x0D);
    os_delay_us(100000);
    UartDev.baut_rate = BIT_RATE_115200; //BIT_RATE_115200;
    UartDev.exist_parity = STICK_PARITY_DIS;
    UartDev.stop_bits = ONE_STOP_BIT;
    UartDev.data_bits = EIGHT_BITS;
    digoleserial_uart1_config();
    os_delay_us(20000);
    digoleserial_lcdNString("Switched to 115200", 18);
    os_printf("\n#Switched to 115200 on uart1#\n");
  }
}

void ICACHE_FLASH_ATTR
digoleserial_init(uint8_t col, uint8_t row) {

  UartDev.baut_rate = 9600;
  digoleserial_uart1_config();
  os_delay_us(100000);
  digoleserial_setBaud();
  digoleserial_lcdClear();

  //digoleserial_lcdNString("", 0); //Sync up
  //digoleserial_lcdNString("", 0); //Sync up

  // Sometimes, or rather often actually, the initial sync is problematic
  digoleserial_lcdClear();
  digoleserial_enableCursor(false);
  // set display size
  if (col>4 && row>=1) {
    uart1_tx_string("STCR");
    uart1_tx_one_char(col);
    uart1_tx_one_char(row);
    uart1_tx_one_char(0x80);
    uart1_tx_one_char(0xC0);
    uart1_tx_one_char(0x94);
    uart1_tx_one_char(0xD4);
    uart1_tx_one_char(0x0);
    os_delay_us(10000);
  }

  digoleserial_lcdClear();
  digoleserial_enableCursor(false);
  os_delay_us(10000);
  digoleserial_lcdString("Screen initiated");
}

