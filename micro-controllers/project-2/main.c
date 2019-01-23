/**
 * File: main.c
 * Date: 04/08/2018
 * 
 * Name: Arnold Jason Chand, Mickey Mcguire
 * ECE5530 - Project 2
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Global Variables */
FlagStatus gBuzzerFlag = RESET;
volatile uint32_t gInpFreqPer = 0;
volatile uint32_t gInpFreq = 0;

// TIM3, OC
uint16_t g30khzPer = 2800;
volatile uint16_t g30khzPulse = 1400;

// TIM4, OC
uint32_t g5_5khzPer = 15272;
uint32_t g3_5khzPer = 24000;

// GPIOB
#define MOTOR_PIN    0
#define SPK_PIN      9
#define ENC_MOD_PIN  10
#define START        GPIO_Pin_11
#define STOP         GPIO_Pin_12

// GPIOC
#define LED_IND GPIO_Pin_0
#define LED_IR  GPIO_Pin_1

/* System variables */
GPIO_InitTypeDef GPIO_IS;
NVIC_InitTypeDef NVIC_IS;
TIM_ICInitTypeDef TIM_ICIS;
TIM_OCInitTypeDef TIM_OCIS;
TIM_TimeBaseInitTypeDef TIM_TBIS;
ADC_InitTypeDef ADC_IS;
DAC_InitTypeDef DAC_IS;

static __IO uint32_t uwTimingDelay;

/* Funtion prototypes */
void task(void);
void bonus1(void);
void bonus2(void);
void bonus3(void);

void systick_init(void);
void pins_config(void);
void tim2_config(void);
void tim3_config(void);
void tim4_config(void);
void adc_config(void);
void dac_config(void);
void encoderModuleCtrl(FunctionalState state);
void buzzerCtrl(FunctionalState state);
void motorCtrl(FunctionalState state);
void motorDutyCtrl(void);
void ledCtrl(uint16_t ledPin, BitAction action);
void ledInd_blink(void);

static void Delay(__IO uint32_t nTime);

int main(void)
{
  task();
  
  // bonus1();
  
  // bonus2();
  
  // bonus3();
}

void task(void)
{
  // Enable delays
  systick_init();
  
  // Configure pins
  pins_config();
  
  // Enable encoder module
  tim2_config();
  encoderModuleCtrl(ENABLE);
  
  // Setup Motor and Speaker control
  tim3_config();
  tim4_config();
  
  while (1) {
//    // wait until START switch goes LOW to HIGH
//    while (GPIO_ReadInputDataBit(GPIOB, START) != Bit_RESET);
//    
//    // Buzzer = ON
//    buzzerCtrl(ENABLE);
//    
//    // Turn on LED 6 times
//    ledIndCtrl();
//    
//    // Disable buzzer
//    buzzerCtrl(DISABLE);
//    // Set flag
//    gBuzzerFlag = SET;
//    
//    // LED_IR = ON
//    GPIOC->BSRRL = LED_IR;
//    
//    // Turn on Motor
//    motorCtrl(ENABLE);
//    
//    // wait until IR (STOP) switch is reset, LOW to HIGH
//    while (GPIO_ReadInputDataBit(GPIOB, STOP) != Bit_RESET) {
//      // Change the motor duty cycle
//      motorDutyCtrl();
//    }
//    
//    motorCtrl(DISABLE);
//    
//    buzzerCtrl(ENABLE);
//    
//    ledIndCtrl();
//    GPIOC->BSRRH = LED_IR;
  }
}

/**
 * Controls the duty cycle of the motor depending on the Encoder Module input.
 */
void motorDutyCtrl(void)
{
  switch (gInpFreq) {
    
    // Set the duty cycle of the Motor
    case 4700:
      g30khzPulse = (g30khzPer * 90) / 100;
      break;
    case 4900:
      g30khzPulse = (g30khzPer * 80) / 100;
      break;
    case 5100:
      g30khzPulse = (g30khzPer * 70) / 100;
      break;
    case 5300:
      g30khzPulse = (g30khzPer * 60) / 100;
      break;
    case 5500:
      g30khzPulse = (g30khzPer * 50) / 100;
      break;
    case 5700:
      g30khzPulse = (g30khzPer * 40) / 100;
      break;
    case 5900:
      g30khzPulse = (g30khzPer * 30) / 100;
      break;
    case 6100:
      g30khzPulse = (g30khzPer * 20) / 100;
      break;
    case 6300:
      g30khzPulse = (g30khzPer * 10) / 100;
      break;
      
    // Set default to 50%
    default:
      break;
  }
}

/**
 * Enables or disables the Timer that controls the Encoder Module
 */
void encoderModuleCtrl(FunctionalState state)
{
  TIM_Cmd(TIM2, state);
}

/**
 * Enables or disables the Timer that controls the Motor
 */
void motorCtrl(FunctionalState state)
{
  TIM_Cmd(TIM3, state);
}

/**
 * Enables or disables the Timer that controls the Speaker
 */
void buzzerCtrl(FunctionalState state)
{
  TIM_Cmd(TIM4, state);
}

/**
 * Turn on or off the LEDs
 */
void ledCtrl(uint16_t ledPin, BitAction action)
{
  GPIO_WriteBit(GPIOC, ledPin, action);
}

/**
 * Blinks the LED indicator 6 (initial) or 10 (end) times,
 * according to the buzzerFlag
 */
void ledInd_blink(void)
{
  if (!gBuzzerFlag) {
    // 6 times, at 1 blink/s
    for (int i = 0; i < 6; i++) {
      ledCtrl(LED_IND, Bit_SET);
      Delay(500);
      ledCtrl(LED_IND, Bit_RESET);
      Delay(500);
    }
  } else {
    // 10 times, at 2 blinks/s
    for (int i = 0; i < 10; i++) {
      ledCtrl(LED_IND, Bit_SET);
      Delay(250);
      ledCtrl(LED_IND, Bit_RESET);
      Delay(250);
    }
  }
}

void systick_init(void)
{
  // set delay to 1ms
  if (SysTick_Config(SystemCoreClock / 1000)) while (1);
}

void pins_config(void)
{
  // Enable clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  /**
   * Setup Pins
   * PC0, PC1 => OUT
   */
  GPIO_IS.GPIO_Pin = LED_IND | LED_IR;
  GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_IS.GPIO_OType = GPIO_OType_PP;
  GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_IS.GPIO_Speed = GPIO_Speed_100MHz;
  
  GPIO_Init(GPIOC, &GPIO_IS);
  
  /**
   * Setup Pins
   * PB11, PB12 => IN
   */
  GPIO_IS.GPIO_Pin = START | STOP;
  GPIO_IS.GPIO_Mode = GPIO_Mode_IN;
  GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_Init(GPIOB, &GPIO_IS);
  
  /**
   * Setup Pins
   * PB0 => AF (Output) - Motor Pin
   * PB9 => AF (Output) - Speaker Pin
   * PB10 => AF (Input) - Encoder Module Pin
   */
  GPIO_IS.GPIO_Pin = (1 << MOTOR_PIN) | (1 << SPK_PIN) | (1 << ENC_MOD_PIN);
  GPIO_IS.GPIO_Mode = GPIO_Mode_AF;
  GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_Init(GPIOB, &GPIO_IS);
}

void tim2_config(void)
{
  // Enable clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /**
   * Connect pins to TIM2
   * ENC_MOD_PIN => Input (Channel 3)
   */
  GPIO_PinAFConfig(GPIOB, ENC_MOD_PIN, GPIO_AF_TIM2);
  
  /**
   * Setup NVIC for TIM2
   */
  NVIC_IS.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_IS.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_IS.NVIC_IRQChannelSubPriority = 0;
  NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_IS);
  
  /**
   * Setup TIM2 - Encoder Module (INPUT)
   */
  TIM_ICIS.TIM_Channel = TIM_Channel_3;
  TIM_ICIS.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICIS.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICIS.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICIS.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM_ICIS);

  TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
}

void tim3_config(void)
{
  // Enable clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  /**
   * Connect pins to TIM3
   * MOTOR_PIN => Input (Channel 3)
   */
  GPIO_PinAFConfig(GPIOB, MOTOR_PIN, GPIO_AF_TIM3);
  
  /**
   * Setup NVIC for TIM3
   */
  NVIC_IS.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_IS.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_IS.NVIC_IRQChannelSubPriority = 1;
  NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_IS);
  
  /**
   * Setup TIM3 - Motor Control (OUTPUT)
   */
  TIM_TBIS.TIM_Period = g30khzPer;
  TIM_TBIS.TIM_Prescaler = 0;
  TIM_TBIS.TIM_ClockDivision = 0;
  TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM3, &TIM_TBIS);
  
  TIM_OCIS.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCIS.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCIS.TIM_Pulse = g30khzPulse;
  TIM_OCIS.TIM_OutputState = ENABLE;
  
  TIM_OC3Init(TIM3, &TIM_OCIS);
  
  TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
}

void tim4_config(void)
{
  // Enable clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  
  /**
   * Connect pins to TIM4
   * SPK_PIN => Output (Channel 4)
   */
  GPIO_PinAFConfig(GPIOB, SPK_PIN, GPIO_AF_TIM4);
  
  /**
   * Setup NVIC for TIM4
   */
  NVIC_IS.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_IS.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_IS.NVIC_IRQChannelSubPriority = 0;
  NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_IS);
  
  /**
   * Setup TIM4 - Buzzer (OUTPUT)
   */
  TIM_OCIS.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCIS.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCIS.TIM_Pulse = g5_5khzPer / 2;
  TIM_OCIS.TIM_OutputState = ENABLE;
  
  TIM_OC4Init(TIM4, &TIM_OCIS);

  TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);
}

void Delay(__IO uint32_t nTime)
{
  uwTimingDelay = nTime;
  while(uwTimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00) {
    uwTimingDelay--;
  }
}
