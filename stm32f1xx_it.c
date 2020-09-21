/**
  ******************************************************************************
  * @file    stm32f1xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"

/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t TimerState;
extern uint32_t count;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                          */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_md.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */

void RTC_IRQHandler(void)
{
	/* RTC Configurado para 1seg - decrementa Timer (quando em estado ativo XXX conferir!)*/
	if(TimerState == 1)
	{

		if(count > 0)
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_8, GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8)^1);
			count = count - 1;
		}
		else
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
		}
	}
	RTC_ClearITPendingBit(RTC_IT_ALR | RTC_IT_OW | RTC_IT_SEC);
}

void EXTI0_IRQHandler(void)
{
	/* Botão do Estado do Timer:
	 * 0: setup
	 * 1: timer ativo (decrementa)
	 * 2: timer inativo
	 * Sequência de estados: 0 --> 1 --> 2 --> 1 --> 2...
	 */
	switch (TimerState)
	{
		case 0:
			TimerState = 1;
			break;
		case 1:
			TimerState = 2;
			break;
		case 2:
			TimerState = 1;
			break;
		default:
			/* Tratamento de exceção (desnecessário?) */
			break;
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void)
{
	/* BotUP: Incrementa o número de minutos do timer */
	if(TimerState == 0)
	{
		if(count < (99 * 60))
		{
			count = count + 60;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

void EXTI3_IRQHandler(void)
{
	/* BotDOWN: Decrementa o número de minutos do timer */
	if(TimerState == 0)
	{
		if(count > 60)
		{
			count = count - 60;
		}
		GPIO_SetBits(GPIOB, GPIO_Pin_9);
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
