/**
 * File: stm32f4xx_it.c
 * Date: 03/12/2018
 *
 * Name: Arnold Jason Chand, Bhupendra Patil, Daniel McKee
 * ECE5530 - Project 1
 */

/* Includes *******************************************************************/
#include "stm32f4xx_it.h"
#include "main.h"

/** Variables *****************************************************************/
unsigned int capture_status = 0;

unsigned int capture = 0;
unsigned int capture_val1 = 0;
unsigned int capture_val2 = 0;
unsigned int capture_val3 = 0;
unsigned int pulse = 0;

/** Extend variables and functions from main.c ********************************/
extern unsigned int freq;
extern unsigned int period;
extern unsigned int pulse_period;
extern unsigned int duty_cycle;

extern void TimingDelay_Decrement(void);

/** TIM2 Interrupt Handler ****************************************************/
void TIM2_IRQHandler(void)
{
  unsigned int PA3;

  if(TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

    // Read the input signal
    PA3 = GPIOA->IDR & 0x8;

    // Get the first rising edge count
    if (capture_status == 0 && PA3 == 0x8) {

      capture_val1 = TIM2->CCR4;
      capture_status = 1;

    // Get the falling edge count
    } else if (capture_status == 1) {

      capture_val2 = TIM2->CCR4;

      // Compare with the first rising edge count
      capture = (capture_val2 > capture_val1) ?
        (capture_val2 - capture_val1) :
        (capture_val2 < capture_val1) ?
          ((0xFFFFFFFF - capture_val1) + capture_val2) :
           0;

      // compute the pulse width and its period
      pulse = (SystemCoreClock / (2 * capture)) + 1;
      pulse_period = (1000000000 / pulse);

      capture_status = 2;

    // Get the last rising edge count to complete the cycle
    } else if (capture_status == 2) {
      capture_val3 = TIM2->CCR4;

      // Compare with the first rising edge count
      capture = (capture_val3 > capture_val1) ?
        (capture_val3 - capture_val1) :
        (capture_val3 < capture_val1) ?
          ((0xFFFFFFFF - capture_val1) + capture_val3) :
           0;

      // Compute the frequency and its period
      freq = (SystemCoreClock / (2 * capture)) + 1;
      period = (1000000000 / freq);

      // Get new duty cycle
      duty_cycle = (freq * 100) / pulse;

      capture_status = 0;
    }
  }
}

/** SysTick Interrupt Handler *************************************************/
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

