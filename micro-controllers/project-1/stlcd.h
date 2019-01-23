/**
 * File: stlcd.h
 * Date: 03/12/2018
 *
 * Name: Arnold Jason Chand, Bhupendra Patil, Daniel McKee
 * ECE5530 - Project 1
 */

#ifndef STLCD_H_
#define STLCD_H_

#include "stm32f4xx.h"

extern void Delay(__IO uint32_t nTime);
extern GPIO_TypeDef *GPIOx;

/** LCD Pins ******************************************************************/
#define D0  GPIO_Pin_0
#define D1  GPIO_Pin_1
#define D2  GPIO_Pin_2
#define D3  GPIO_Pin_3
#define D4  GPIO_Pin_4
#define D5  GPIO_Pin_5
#define D6  GPIO_Pin_6
#define D7  GPIO_Pin_7

// RS, LOW = Data; HIGH = Instruction
#define RS GPIO_Pin_8

// Enable clock pulse
#define E GPIO_Pin_9

// RW, LOW = Write; HIGH = Read
#define RW GPIO_Pin_10

// PSB, LOW = Serial; HIGH = Parallel mode
#define PSB GPIO_Pin_11

// Reset pin, RST
#define RST GPIO_Pin_12

/** Commands ******************************************************************/
#define CMD_CLEAR             0x01
#define CMD_HOME              0x02
#define CMD_ENTRY_MODE        0x06
#define CMD_DISPLAY_CTRL      0x0E
#define CMD_FUNCTION_SET      0x3B
#define CMD_FUNCTION_SET_EXT  0x36

/** CGRAM Defines *************************************************************/
#define CGRAM_SIZE    16
#define CGRAM_ADDR_0  0x40
#define CGRAM_ADDR_1  0x50
#define CGRAM_ADDR_2  0x60
#define CGRAM_ADDR_3  0x70

/** Functions *****************************************************************/
void stlcd_init(GPIO_TypeDef* gpio);
void stlcd_cmd(char cmd);
void stlcd_write(char stlcd_char);
void stlcd_write_string(char* str);
void stlcd_write_number(uint32_t num, uint8_t dec_point, uint8_t power_sign);
void stlcd_set_cursor(uint8_t row, uint8_t col);
void stlcd_clear_gdram();
void stlcd_set_gdram(uint8_t row, uint8_t col, uint16_t data);
void stlcd_write_gdram(uint16_t wave[32][8]);
void stlcd_set_cgram(char cgram_addr, char pattern[CGRAM_SIZE][2]);
void stlcd_write_cgram(char cgram_addr);
void __set_point(uint32_t eng_notation, uint8_t dec_flag);
void __set_unit(uint32_t eng_notation, uint8_t power_sign);

#endif /* STLCD_H_ */

