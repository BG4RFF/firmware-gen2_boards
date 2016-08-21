/*
 * EGG Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015, 2106.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x.h"
#include "gpio.h"
#include "main.h"

static unsigned int _ms;

void delay_ms (unsigned int ms)
{
  _ms = 1;
  while (ms >= _ms) { ; }
}

void SysTick_Handler(void) // runs every 1ms
{
  // for delay_ms ()
  _ms++;
}

/*
 * Configure the system clock to be 64 MHz (this is the max clock speed possible using the High Speed Internal oscillator)
 */
void SetSysClockTo64(void)
{
  /* Enable Prefetch Buffer */
  FLASH->ACR |= FLASH_ACR_PRFTBE;

  /* Flash 2 wait state */
  FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
  FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

  /* HCLK = SYSCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

  /* PCLK2 = HCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

  /* PCLK1 = HCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

  /*  PLL configuration: PLLCLK = HSI/2 * 16 = 64 MHz (this is the max clock speed possible using HSI) */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
  RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL16);

  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
  }

  /* Select PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
  {
  }
}

void initialize (void)
{
  /* Setup SysTick Timer for 1 millisecond interrupts, also enables Systick and Systick-Interrupt */
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    /* Capture error */
    while (1);
  }

  gpio_init (); // configure pins just after PWM init
  buzzer_init ();
  usart1_init ();
}

int main(void)
{
  initialize ();

  char string [] = "test uart\n";

  while (1)
  {
     delay_ms (1000);
     usart1_send_str (string);
  }
}
