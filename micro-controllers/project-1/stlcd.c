/**
 * File: stlcd.c
 * Date: 03/12/2018
 *
 * Name: Arnold Jason Chand, Bhupendra Patil, Daniel McKee
 * ECE5530 - Project 1
 */

#include "stlcd.h"

GPIO_TypeDef *GPIOx;

/**
 * @brief Initializes the LCD
 * @param gpio: Set the GPIO port connected to LCD
 */
void stlcd_init(GPIO_TypeDef* gpio)
{
  // Globally assign GPIOx
  GPIOx = gpio;

  // Set PSB, HIGH = Parallel mode
  GPIOx->BSRRL = PSB;

  // RST pin: Low-to-High
  // Reset
  GPIOx->BSRRH = RST;
  Delay(5);

  // Set
  GPIOx->BSRRL = RST;

  // Delay; > 40ms
  Delay(50);

  // Function set 1, 0b 0 0 1 1 X 0 X X
  stlcd_cmd(CMD_FUNCTION_SET);

  // Delay > 100us
  Delay(1);

  // Function set 2, 0b 0 0 1 1 X 0 X X
  stlcd_cmd(CMD_FUNCTION_SET);

  // Delay > 100us
  Delay(1);

  // Display on/off cntrl, 0b 0 0 0 0 1 D C B
  // D = Display on/off
  // C = Cursor on/off
  // B = Blink on/off
  stlcd_cmd(CMD_DISPLAY_CTRL);

  // Delay > 100us
  Delay(1);

  // Clear display
  stlcd_cmd(CMD_CLEAR);

  // Delay > 10ms
  Delay(20);

  // Entry mode set, 0b 0 0 0 0 0 1 (I/D) S
  // I/D = Counter increase/decrease
  // S = Display shift
  stlcd_cmd(CMD_ENTRY_MODE);
}

/**
 * @brief Send a command to the LCD
 * @param STLCD_Cmd: 8-bit command data to send
 */
void stlcd_cmd(char cmd)
{
  // Reset RW, LOW = Write
  GPIOx->BSRRH = RW;

  // Reset RS, LOW = Instruction
  GPIOx->BSRRH = RS;

  // Write instruction to LCD
  GPIOx->ODR = (GPIOx->IDR & 0xFF00) | cmd;

  // Set Enable Pulse
  GPIOx->BSRRL = E;
  Delay(1);
  GPIOx->BSRRH = E;
  Delay(1);
}

/**
 * @brief Write Data to internal RAM
 * @param STLCD_Data: 8-bit char data to write
 */
void stlcd_write(char stlcd_char)
{
  // Reset RW, LOW = Write
  GPIOx->BSRRH = RW;

  // Set RS, HIGH = Data
  GPIOx->BSRRL = RS;

  // Write Data to LCD
  GPIOx->ODR = (GPIOx->IDR & 0xFF00) | stlcd_char;

  // Set Enable Pulse
  GPIOx->BSRRL = E;
  Delay(10);
  GPIOx->BSRRH = E;
  Delay(1);
}

/**
 * @brief Writes a string of char to the LCD.
 * @param str: String of characters.
 */
void stlcd_write_string(char* str)
{
  // Write each char except the null char of string
  for (int i = 0; str[i] != '\0'; i++) {
    stlcd_write(str[i]);
  }
}

unsigned int point_flag = 0;
unsigned int digit_flag = 0;
unsigned int unit_flag = 0;
unsigned int ones_flag = 0;
unsigned int tenth_flag = 0;
unsigned int hundredth_flag = 0;
char unit_value = ' ';

/**
 * @brief Writes a positive number to the LCD.
 * @param num: An unsigned number to write to LCD.
 * @param dec_pont: Number of digits to show after a point, also used as a flag.
 * @param power_sign: Power sign of the number,
 *                    if 0 then 10^(+ve  num)
 *                    if 1 then 10^(-ve num)
 */
void stlcd_write_number(uint32_t num, uint8_t dec_point, uint8_t power_sign)
{
  // power notation
  uint32_t e = 1000000000;

  uint8_t digit = 0;
  uint32_t value = num;

  for (int i = e; i != 0; i /= 10) {
    digit = value / i;

    // If the point flag is not set and decimal point is required
    if (point_flag == 0) {

      // Record the first non-zero digit
      if (digit >= 1 && i > 1 && digit_flag == 0) {
	// Write first digit
	stlcd_write(0x30 | digit);

	// Set the unit and point if it counts
	__set_unit(i, power_sign);
	// Check for decimal point, if needed
	__set_point(i, dec_point);

	// Set digit flag
	digit_flag = 1;
      } else if (digit >= 1 && i == 1 && digit_flag == 0) {
	// Write first digit
	stlcd_write(0x30 | digit);

	// Set the unit and point if it counts
	__set_unit(i, power_sign);
	// Check for decimal point, if needed
	__set_point(i, dec_point);

	// Set digit flag
	digit_flag = 1;
      } else if (digit >= 1 && digit_flag == 1) {
	stlcd_write(0x30 | digit);

	// Set the unit and point if it counts
	__set_unit(i, power_sign);
	// Check for decimal point, if needed
	__set_point(i, dec_point);

      } else if (digit == 0 && digit_flag == 1) {
	stlcd_write(0x30 | digit);

	// Set the unit and point if it counts
	__set_unit(i, power_sign);
	// Check for decimal point, if needed
	__set_point(i, dec_point);
      }

      // If only ones flag is set, then show one more digit
      // after the decimal point
      if (ones_flag == 1 && tenth_flag == 0 &&
	  hundredth_flag == 0 && dec_point == 0) {

	  // Set decimal point
	  dec_point = 1;

	  // Check for decimal point, if needed
	  __set_point(i, dec_point);
      }

    } else if (point_flag == 1 && dec_point != 0) {
      // Write number after point
      stlcd_write(0x30 | digit);
      dec_point--;
    }

    // Update for next digit
    value -= (digit * i);
  }

  // if the unit value is not empty
  if (unit_value != ' ') {
    // Write the unit
    stlcd_write(unit_value);
  }

  // Reset flags and value
  point_flag = 0;
  digit_flag = 0;
  unit_flag = 0;
  unit_value = ' ';
  ones_flag = 0;
  tenth_flag = 0;
  hundredth_flag = 0;
}

/**
 * @brief Sets the cursor position on the LCD
 * @param row: The row of the LCD (Valid number: 0x00 - 0x03)
 * @param col: The column of the LCD (Valid number: 0x00 - 0x0F)
 */
void stlcd_set_cursor(uint8_t row, uint8_t col)
{
  // Get the row
  if      (row == 0) row = 0x80;
  else if (row == 1) row = 0x90;
  else if (row == 2) row = 0xA0;
  else if (row == 3) row = 0xB0;

  // Set cursor to position as long as column is between 0x00 and 0x0F
  if (col <= 0x0F) stlcd_cmd(((char) row) | ((char) col));

  // Else go to the origin
  else stlcd_cmd(0x80);
}

/**
 * @brief Clears all pixels in GDRAM (Takes time to perform full operation)
 */
void stlcd_clear_gdram()
{
  // Enter GDRAM address mode
  stlcd_cmd(CMD_FUNCTION_SET_EXT);
  
  for (int r = 0; r < 64; r++) {
    // Set vertical address
    stlcd_cmd(0x80 | r);
    // Set horizontal address
    stlcd_cmd(0x80 | 0);
    
    for (int c = 0; c < 16; c++) {
      // Clear for every horizontal data
      stlcd_write(0x00);
      stlcd_write(0x00);
    }
  }

  // Enter back to basic mode
  stlcd_cmd(CMD_FUNCTION_SET);
}

/**
 * @brief Sets each pixel of the LCD
 * @param v_addr: Vertical address of GDRAM, Valid range: 0x00 - 0x3F
 * @param h_addr: Horizontal address for GDRAM, Valid range: 0x00 - 0x0F
 * @param data: 2-byte long data
 */
void stlcd_set_gdram(uint8_t v_addr, uint8_t h_addr, uint16_t data)
{
  // Set vertical address
  stlcd_cmd(0x80 | (v_addr & 0x7F));

  // Set horizontal address
  stlcd_cmd(0x80 | (h_addr & 0x0F));

  // Write higher and lower bytes
  stlcd_write((char) ((data & 0xFF00) >> 8));
  stlcd_write((char) (data & 0x00FF));
}

/**
 * @brief Writes a 32 x 8 - 2 byte word to the LCD screen
 * @param wave[32][8]: 2D array data to be written to the
 *                     top half of the screen
 */
void stlcd_write_gdram(uint16_t data[32][8])
{
  // Set extended instruction
  stlcd_cmd(CMD_FUNCTION_SET_EXT);
  
  for (int r = 0; r < 32; r++) {
    // Set vertical address
    stlcd_cmd(0x80 | r);
    // Set horizontal address, auto increment
    stlcd_cmd(0x80 | 0);
    
    for (int c = 0; c < 8; c++) {
      // Write every 2-byte
      stlcd_write((data[r][c] & 0xFF00) >> 8);
      stlcd_write(data[r][c] & 0x00FF);
    }
  }

  // Go back to basic instruction
  stlcd_cmd(CMD_FUNCTION_SET);
}

/**
 * @brief Sets custom character to the CGRAM
 * @param CGRAM_Addr: CGRAM Address location, only 4 allowed address
 *                    CGRAM_Addr0, CGRAM_Addr1, CGRAM_Addr2 and CGRAM_Addr3
 * @param pattern[][]: Pattern to store to CGRAM, a 2D-array
 */
void stlcd_set_cgram(char cgram_addr, char pattern[CGRAM_SIZE][2])
{
  for (int i = 0; i < CGRAM_SIZE; i++) {
    // Select CGRAM row
    stlcd_cmd(cgram_addr + i);

    // Write pattern
    stlcd_write(pattern[i][0]);
    stlcd_write(pattern[i][1]);
  }
}

/**
 * @brief Prints the custom character stored in CGRAM address
 * @param CGRAM_Addr: Address where the custom character is stored.
 */
void stlcd_write_to_cgram(char cgram_addr)
{
  // Write the custom char at address
  switch (cgram_addr) {

    default:
    case CGRAM_ADDR_0:
      stlcd_write(0x00);
      stlcd_write(0x00);
      break;

    case CGRAM_ADDR_1:
      stlcd_write(0x00);
      stlcd_write(0x02);
      break;

    case CGRAM_ADDR_2:
      stlcd_write(0x00);
      stlcd_write(0x04);
      break;

    case CGRAM_ADDR_3:
      stlcd_write(0x00);
      stlcd_write(0x06);
      break;
  }
}

/**
 * @brief Sets the first point of a scientific notation to a big value.
 * @param eng_notation: Engineering notation value (10, 1000, 1000000, etc).
 * @param dec_flag: To display a '.' to the screen if dec_point != 0.
 */
void __set_point(uint32_t eng_notation, uint8_t dec_flag)
{
  // Add the point and set the flag to not add anymore
  switch (eng_notation) {
    case 1000000000:
      if (dec_flag != 0) stlcd_write('.');
      point_flag = 1;
      break;
    case 1000000:
      if (dec_flag != 0) stlcd_write('.');
      point_flag = 1;
      break;
    case 1000:
      if (dec_flag != 0) stlcd_write('.');
      point_flag = 1;
      break;
    default:
      break;
  }
}

/**
 * @brief Sets the unit to display to the screen instead of remaining numbers.
 * @param eng_notation: Engineering notation value (10, 1000, 1000000, etc).
 * @param power_sign: The sign the notation exponent to be. If argument is 0
 *                    then the exponent is positive else the exponent will be
 *                    negative.
 */
void __set_unit(uint32_t eng_notation, uint8_t power_sign)
{
  // Add a point to specific power
  switch (eng_notation) {

    /**
     * Power of 9
     */
    case 1000000000:
      // Add Giga depending on the power sign
      unit_value = power_sign == 0 ? 'G' : ' ';

      // Set the unit flag
      ones_flag = 1;
      unit_flag = 1;
      break;

    /**
     * Power of 6
     */
    case 1000000:
      // Add Mega or milli depending on the power sign
      unit_value = power_sign == 0 ? 'M' : 'm';

      // Set the unit flag
      unit_flag = 1;
      ones_flag = 1;
      break;
    case 10000000:
      // Add Mega or milli depending on the power sign
      unit_value = power_sign == 0 ? 'M' : 'm';

      // Set the unit flag
      unit_flag = 1;
      tenth_flag = 1;
      break;
    case 100000000:
      // Add Mega or milli depending on the power sign
      unit_value = power_sign == 0 ? 'M' : 'm';

      // Set the unit flag
      unit_flag = 1;
      hundredth_flag = 1;
      break;

    /**
     * Power of 3
     */
    case 1000:
      // Add kilo or micro depending on the power sign
      unit_value = power_sign == 0 ? 'k' : 'u';

      // Set the unit flag
      unit_flag = 1;
      ones_flag = 1;
      break;
    case 10000:
      // Add kilo or micro depending on the power sign
      unit_value = power_sign == 0 ? 'k' : 'u';

      // Set the unit flag
      unit_flag = 1;
      tenth_flag = 1;
      break;
    case 100000:
      // Add kilo or micro depending on the power sign
      unit_value = power_sign == 0 ? 'k' : 'u';

      // Set the unit flag
      unit_flag = 1;
      hundredth_flag = 1;
      break;
    default:
      break;
  }
}

