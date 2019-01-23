/**
 * File: main.c
 * Date: 03/12/2018
 *
 * Name: Arnold Jason Chand, Bhupendra Patil, Daniel McKee
 * ECE5530 - Project 1
 */

/** Includes ******************************************************************/
#include "main.h"

// Include LCD library
#include "stlcd.h"

/** Variables *****************************************************************/
static __IO uint32_t TimingDelay;

uint16_t square_wave[32][8] = {0};

unsigned int pins = 0;

unsigned int freq = 0;
unsigned int period = 0;
unsigned int pulse_period = 0;
unsigned int duty_cycle = 0;
unsigned int high_pulse_length = 64;

/** TypeDefs ******************************************************************/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
TIM_ICInitTypeDef        TIM_ICInitStruct;
GPIO_InitTypeDef         GPIO_InitStruct;
NVIC_InitTypeDef         NVIC_InitStruct;

/** Function prototypes *******************************************************/
uint16_t decode(uint16_t x);
uint16_t two_pow(uint16_t power);
void draw_wave();
void enable_clocks();
void GPIOB_Init(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void GPIOA_Init(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void TIM2_Config();
void Delay(__IO uint32_t nTime);

/** MAIN PROGRAM **************************************************************/
int main(void)
{
  // Enable the Clocks
  enable_clocks();

  // Configure SysTick, for delay
  if (SysTick_Config(SystemCoreClock / 1000) != 0) while(1);

  // Initialize PA1 as Alternate function
  GPIOA_Init(GPIO_Pin_3, GPIO_Mode_AF);

  // Config TIM2
  TIM2_Config();

  // Set D0-D7, E, RS, RW, RST and PSB as output
  pins = D0 | D1 | D2 | D3 | D4 | D5 | D6 | D7;
  pins |= PSB;
  pins |= RS;
  pins |= RW;
  pins |= E;
  pins |= RST;
  GPIOB_Init(pins, GPIO_Mode_OUT);

  // Initialize LCD connected to GPIOB
  stlcd_init(GPIOB);

  // Clear the GDRAM
  stlcd_clear_gdram();

  while (1)
  {
    // Get high pulse length
    high_pulse_length = (duty_cycle * 128) / 100;

    // Draw the square wave according to the duty cycle
    draw_wave();

    // Output the wave to the LCD
    stlcd_write_gdram(square_wave);

    // Send period to LCD
    // 3rd Line
    stlcd_set_cursor(0, 8);

    // Output frequency period to LCD
    stlcd_write_string("T=");
    stlcd_write_number(period, 0, 1);
    stlcd_write('s');

    stlcd_write(' ');

    // Output Pulse period to LCD
    stlcd_write_string("+W=");
    stlcd_write_number(pulse_period, 0, 1);
    stlcd_write('s');

    // clear extra spaces
    stlcd_write(' ');
    stlcd_write(' ');

    // Send freq and duty cycle data to LCD
    // 4th Line
    stlcd_set_cursor(1, 8);

    // Write frequency value
    stlcd_write_string("F=");
    stlcd_write_number(freq, 1, 0);
    stlcd_write_string("Hz");

    stlcd_write(' ');

    // Write duty cycle
    stlcd_write_string("D=");
    stlcd_write_number(duty_cycle, 0, 0);
    stlcd_write('%');

    // clear extra spaces
    stlcd_write(' ');
    stlcd_write(' ');

    Delay(10);
  }
}

void draw_wave()
{
  uint8_t col_new = 0;
  int i = 0, j = 0;

  // Reset square wave array
  for (i = 0; i < 32; i++) {
    for (j = 0; j < 8; j++) {
      square_wave[i][j] = 0;
    }
  }

  // Draw the horizontal high and low lines
  for (i = 0; i < 8; i++) {

    // If each 16-bit block is available
    if (i < (high_pulse_length / 16)) {

      // Set the wave high line
      square_wave[0][i]  = 0xFFFF;

      // Reset the wave low line
      square_wave[31][i] = 0x0;

    // If at the last valid but not a 16-bit block
    } else if (i == (high_pulse_length / 16)) {

      // decode the value, to shift from LSB to MSB
      // and fill empty bits in between
      square_wave[0][i] = decode(high_pulse_length % 16);

      // invert the obtained high line to get the low line
      // bits that are valid but not full 16-bit block
      square_wave[31][i] = (~(square_wave[0][i]) << 1) + 1;

      // Get the column to set the vertical line
      col_new = i;

    // Else set the high line to be zero
    // and the low line to be filled with 16-bit blocks
    } else {
      square_wave[0][i]  = 0x0;
      square_wave[31][i] = 0xFFFF;
    }
  }

  // Draw the vertical line, at given column: col_new
  for (i = 1; i < 31; i++) {
    square_wave[i][col_new] = two_pow(15 - (high_pulse_length % 16));
  }
}

uint16_t two_pow(uint16_t power)
{
  uint16_t temp = 1;

  for (int i = 0; i < power; i++) {
    temp <<= 1;
  }

  return temp;
}

uint16_t decode(uint16_t x)
{
  uint16_t temp = 0;
  for (int i = 0; i < x + 1; i++) {
    temp >>= 1;
    temp += 0x8000;
  }

  return temp;
}

void enable_clocks()
{
  // GPIOB Clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  // GPIOA Clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // TIM2 Clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  // SYSTICK Clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
}

void GPIOB_Init(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void GPIOA_Init(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void TIM2_Config()
{
  // Connect PA3 to TIM2
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);

  // Interrupt setup for TIM2
  NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStruct);

  // Input Capture on Ch4
  TIM_ICInitStruct.TIM_Channel = TIM_Channel_4;
  TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
  TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStruct.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM_ICInitStruct);

  // Enable TIM2 counter
  TIM_Cmd(TIM2, ENABLE);

  // Setup Ch3 interrupt
  TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0) TimingDelay--;
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while (TimingDelay != 0);
}