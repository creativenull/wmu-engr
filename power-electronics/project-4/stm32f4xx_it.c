/**
 * File: stm32f4xx_it.c
 * Date: 04/08/2018
 * 
 * Name: Arnold Jason Chand, Faraj Alyami
 * ECE6200 - Project 3
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"

extern volatile uint32_t gDuty;

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_CC2))
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    
    TIM2->CCR2 = gDuty;
  }
}