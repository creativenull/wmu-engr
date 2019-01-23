/**
 * File: main.c
 * Date: 04/08/2018
 * 
 * Name: Arnold Jason Chand, Faraj Alyami
 * ECE6200 - Project 3
 *
 * Pin Description:
 * PC0  -> ADC (Output Voltage V2+)
 * PB3  -> TIM2 OC2 (PWM Output)
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
GPIO_InitTypeDef GPIO_IS;
ADC_InitTypeDef ADC_IS;
NVIC_InitTypeDef NVIC_IS;
TIM_OCInitTypeDef TIM_OCIS;
TIM_TimeBaseInitTypeDef TIM_TBIS;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;

static const uint32_t           g100khzPer = 840;
volatile uint32_t               gDuty = 42;

static uint16_t                 gVoltage = 0;
static uint16_t                 gDigital = 0;

static const uint32_t           gVref = 12;
static const float              gKp = 10;
static const float              gKi = 0.1;
static int32_t                  gErr = 0;
static int32_t                  gSum = 0;
static int32_t                  gP = 0;
static int32_t                  gI = 0;
static int32_t                  gPI = 0;

/* Private function prototypes -----------------------------------------------*/
static void adc_cfg(void);
static void tim_cfg(void);

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  tim_cfg();
  adc_cfg();
  
  while (1) {
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) {
      ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
      
      // Read the analog voltage
      gDigital = ADC_GetConversionValue(ADC1);
      gVoltage = (gDigital * 35) / 4095;
      
      // Calculate error
      gErr = gVref - gVoltage;
      
      gP = (int32_t) (gErr * gKp);
      
      gI = (int32_t) (gSum * gKi);
      
      gPI = gP + gI;
      
      gSum += gErr;
      if (gSum > 5500) {
        gSum = 5500;
      }
      
      if (gPI > 0 && gPI < 840) {
        gDuty = gPI;
      }
    }
  }
}

static void adc_cfg(void)
{
  // Enable clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /**
   * Setup Pins
   */
  GPIO_IS.GPIO_Pin = GPIO_Pin_0;
  GPIO_IS.GPIO_Mode = GPIO_Mode_AN;
  GPIO_IS.GPIO_OType = GPIO_OType_PP;
  GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_IS.GPIO_Speed = GPIO_Speed_100MHz;
  
  GPIO_Init(GPIOC, &GPIO_IS);
  
  // Setup ADC1
  ADC_IS.ADC_Resolution = ADC_Resolution_12b;
  ADC_IS.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_IS.ADC_ScanConvMode = DISABLE;
  ADC_IS.ADC_ContinuousConvMode = ENABLE;
  ADC_IS.ADC_NbrOfConversion = 1;

  ADC_Init(ADC1, &ADC_IS);
  
  // Enable ADC1
  ADC_Cmd(ADC1, ENABLE);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
  
  ADC_SoftwareStartConv(ADC1);
}

static void tim_cfg(void)
{
  // Enable clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /**
   * Setup Pins
   */
  GPIO_IS.GPIO_Pin = GPIO_Pin_3;
  GPIO_IS.GPIO_Mode = GPIO_Mode_AF;
  GPIO_IS.GPIO_OType = GPIO_OType_PP;
  GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_IS.GPIO_Speed = GPIO_Speed_100MHz;
  
  GPIO_Init(GPIOB, &GPIO_IS);
  
  // Connect Pin to AF
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);
  
  /**
   * NVIC
   */
  NVIC_IS.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
  NVIC_IS.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_IS.NVIC_IRQChannelSubPriority = 0;
  
  NVIC_Init(&NVIC_IS);
  
  /**
   * Time Base
   */
  TIM_TBIS.TIM_Period = g100khzPer;
  TIM_TBIS.TIM_Prescaler = 0;
  TIM_TBIS.TIM_ClockDivision = 0;
  TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM2, &TIM_TBIS);
  
  /**
   * PWM
   */
  TIM_OCIS.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCIS.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCIS.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCIS.TIM_Pulse = gDuty;
  
  TIM_OC2Init(TIM2, &TIM_OCIS);
  
  // Enable TIM2
  TIM_Cmd(TIM2, ENABLE);
  
  // Enable ISR
  TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
}
