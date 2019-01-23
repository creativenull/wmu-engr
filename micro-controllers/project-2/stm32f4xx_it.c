/**
 * File: stm32f4xx_it.c
 * Date: 04/08/2018
 * 
 * Name: Arnold Jason Chand, Mickey Mcguire
 * ECE5530 - Project 2
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"

// TIM2, IC
extern __IO uint32_t gInpFreqPer;
extern __IO uint32_t gInpFreq;

// TIM3, OC
extern uint16_t g30khzPer;
extern __IO uint16_t g30khzPulse;

// TIM4, OC
extern uint32_t g5_5khzPer;
extern uint32_t g3_5khzPer;

extern void TimingDelay_Decrement(void);
extern FlagStatus gBuzzerFlag;

/** TIM2 Handler ***************************************************************/
static FlagStatus gTIM2Flag = RESET;
static uint32_t gTIM2CaptureCount[2] = {0, 0};
static __IO uint32_t gTIM2Capture = 0;

void TIM2_IRQHandler(void)
{
  /**
   * Channel 3 - Encoder Module (INPUT)
   */
  if(TIM_GetITStatus(TIM2, TIM_IT_CC3)) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    
    if(!gTIM2Flag) {
      // Get the first rising edge count
      gTIM2CaptureCount[0] = TIM_GetCapture3(TIM2);
      gTIM2Flag = SET;
    } else {
      // Get the second rising edge count
      gTIM2CaptureCount[1] = TIM_GetCapture3(TIM2); 
      
      if (gTIM2CaptureCount[1] > gTIM2CaptureCount[0]) {
        gTIM2Capture = (gTIM2CaptureCount[1] - gTIM2CaptureCount[0]); 
      } else if (gTIM2CaptureCount[1] < gTIM2CaptureCount[0]) {
        gTIM2Capture = 
          ((0xFFFFFFFF - gTIM2CaptureCount[0]) + gTIM2CaptureCount[1]); 
      } else {
        gTIM2Capture = 0;
      }
      
      // Compute the frequecy and its period
      gInpFreq = (uint32_t) ((SystemCoreClock / 2) / gTIM2Capture) + 1;
      gTIM2Flag = RESET;
    }
  }
}

/** TIM3 Handler **************************************************************/
void TIM3_IRQHandler(void)
{
  /**
   * Channel 3 - Motor Control (OUTPUT)
   */
  if (TIM_GetITStatus(TIM3, TIM_IT_CC3)) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    
    // Set the PWM duty cycle according to encoder module input
    TIM_SetCompare3(TIM3, g30khzPulse);
  }
}

/** TIM4 Handler **************************************************************/
static __IO uint16_t gTIM4Capture = 0;
static FlagStatus gTIM4Edge5_5 = RESET;
static FlagStatus gTIM4Edge3_5 = RESET;

void TIM4_IRQHandler(void)
{
  /**
   * Channel 4 - Buzzer (OUTPUT)
   */
  if (TIM_GetITStatus(TIM4, TIM_IT_CC4)) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
    
    gTIM4Capture = TIM_GetCapture4(TIM4);
    
    if (!gBuzzerFlag) {
      // 5.5 kHz
      if (!gTIM4Edge5_5) {
        TIM_SetCompare4(TIM4, gTIM4Capture + (g5_5khzPer / 2));
        gTIM4Edge5_5 = SET;
      } else {
        TIM_SetCompare4(TIM4, gTIM4Capture + (g5_5khzPer / 2));
        gTIM4Edge5_5 = RESET;
      }
    } else {
      // 3.5 Khz
      if (!gTIM4Edge3_5) {
        TIM_SetCompare4(TIM4, gTIM4Capture + (g3_5khzPer / 2));
        gTIM4Edge3_5 = SET;
      } else {
        TIM_SetCompare4(TIM4, gTIM4Capture + (g3_5khzPer / 2));
        gTIM4Edge3_5 = RESET;
      }
    }
  }
}

/** SysTick Handler ***********************************************************/
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}