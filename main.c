/* Questão 2 - Displays 7-Segmentos Multiplexados */

/* Includes */
#include "stm32f10x.h"
/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
volatile uint8_t TimerState = 0;
volatile uint8_t DisplayState = 0;
uint8_t ProgramError = 0;

const uint16_t TabDigNew[10] = {0b01111110, 0b00001100, 0b10110110, 0b10011110, 0b11001100, 0b11011010,
		0b11111010, 0b00001110, 0b11111110, 0b11011110};

const uint16_t FimdaContagem = {0b10000000};

volatile uint16_t digUnid = 0, digDez = 0;
volatile uint32_t count = 0;

/* Pinagem:
 * PA0-PA7: Displays
 * PA0: (-)
 * PA1: a
 * PA2: b
 * PA3: c
 * PA4: d
 * PA5: e
 * PA6: f
 * PA7: g
 * PB4: Q2 (Unidade)
 * PB6: Q1 (Dezena)
 * PB7-PB9: Leds
 */

/* Private function prototypes */
void Button_Configuration(void);
void Display_Unidade_Configuration(void);
void Leds_Configuration(void);
void SysTick_Configuration(void);
void RTC_Configuration(void);
void Delay(uint16_t k);
void Transistor_Ports(void);

/* Private functions */
void Button_Configuration(void)
{
	/* Botões:
	 * PB0: Altera os estados do timer
	 * PB1: BotUP - incrementa minutos (só ativo na configuração do timer).
	 * PB3: BotDOWN - decrementa minutos (só ativo na configuração do timer).
	*/

	/* Clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Configuração das Portas */
	GPIO_InitTypeDef myButton;

	myButton.GPIO_Mode = GPIO_Mode_IPD;
	myButton.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
	myButton.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &myButton);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);

	/* Linhas EXTI */

	EXTI_InitTypeDef myEXTI;

	myEXTI.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line3;
	myEXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	myEXTI.EXTI_Trigger = EXTI_Trigger_Rising;
	myEXTI.EXTI_LineCmd = ENABLE;
	EXTI_Init(&myEXTI);

	/* Interrupções associadas aos botões - NVIC */
	NVIC_InitTypeDef myIntButton;

	/* PB0 */
	myIntButton.NVIC_IRQChannel = EXTI0_IRQn;
	myIntButton.NVIC_IRQChannelPreemptionPriority = 1;
	myIntButton.NVIC_IRQChannelSubPriority = 1;
	myIntButton.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&myIntButton);

	/* PB1 */
	myIntButton.NVIC_IRQChannel = EXTI1_IRQn;
	myIntButton.NVIC_IRQChannelPreemptionPriority = 2;
	myIntButton.NVIC_IRQChannelSubPriority = 2;
	myIntButton.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&myIntButton);

	/* PB3 */
	myIntButton.NVIC_IRQChannel = EXTI3_IRQn;
	myIntButton.NVIC_IRQChannelPreemptionPriority = 3;
	myIntButton.NVIC_IRQChannelSubPriority = 3;
	myIntButton.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&myIntButton);
}

void Display_Unidade_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef myDisplay;

	myDisplay.GPIO_Mode = GPIO_Mode_Out_PP;
	myDisplay.GPIO_Pin = GPIO_Pin_All;
	myDisplay.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_Init(GPIOA, &myDisplay);
}

void Leds_Configuration(void)
{
	/* Configuração das Portas */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef myLed;

	myLed.GPIO_Mode = GPIO_Mode_Out_PP;
	myLed.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	myLed.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOB, &myLed);
}

void SysTick_Configuration(void)
{
	/* Configurado para interrupções de 1ms */
	SysTick_Config(9000);		/* 1kHz */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

	NVIC_InitTypeDef mySystick;

	mySystick.NVIC_IRQChannel = SysTick_IRQn;
	mySystick.NVIC_IRQChannelPreemptionPriority = 0;
	mySystick.NVIC_IRQChannelSubPriority = 0;
	mySystick.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&mySystick);
}

void RTC_Configuration(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);

	PWR_BackupAccessCmd (ENABLE);
	RCC_BackupResetCmd(ENABLE);
	Delay(100);
	RCC_BackupResetCmd(DISABLE);
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}
	RCC_LSICmd(DISABLE);

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);

	RTC_WaitForSynchro();

	RTC_WaitForLastTask();
	RTC_EnterConfigMode();
	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767);
	RTC_WaitForLastTask();
	RTC_SetCounter(1599168621); // UTC 3/set/20 9h30pm
	RTC_WaitForLastTask();
	RTC_SetAlarm(1599168621 + 1000);
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();
	RTC_ExitConfigMode();
	RTC_WaitForLastTask();

	//BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
	NVIC_InitTypeDef my_IRQ;
	my_IRQ.NVIC_IRQChannel = RTC_IRQn;
	my_IRQ.NVIC_IRQChannelPreemptionPriority = 1;
	my_IRQ.NVIC_IRQChannelSubPriority = 1;
	my_IRQ.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&my_IRQ);
}

void Delay(uint16_t k)
{
	while(k > 0)
	{
		k = k - 1;
	}
}

void Transistor_Ports(void)
{
	/* Configuração das Portas do Transistor */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef myLed;

	myLed.GPIO_Mode = GPIO_Mode_Out_PP;
	myLed.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	myLed.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOB, &myLed);
}

int main(void)
{
	/* Usar funções já declaradas no escopo! */
	__disable_irq();
	Button_Configuration();
	Display_Unidade_Configuration();
	Leds_Configuration();
	Transistor_Ports();
	SysTick_Configuration();
	RTC_Configuration();
	__enable_irq();

	/* Infinite loop */
	while (1)
	{
		/* 3 Estados do timer:
		 * 0: Modo de configuração, botUP e botDOWN ativos.
		 * 1: Timer ativo, decrementando.
		 * 2: Timer pausado.
		 */

		switch (TimerState)
		{
			case 0:
				/* Configuração do Timer. */
				GPIO_ResetBits(GPIOB, GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				break;

			case 1:
				/* Decrementa o timer. */
				if(count == 0) /* Acabou a contagem */
				{
					GPIO_SetBits(GPIOB, GPIO_Pin_7);
					GPIO_SetBits(GPIOB, GPIO_Pin_8);
				}
				else
				{
					/* Contagem ativa, Led Pisca */
					GPIO_SetBits(GPIOB, GPIO_Pin_7);
				}
				break;

			case 2:
				/* Timer Pausado. */
				GPIO_ResetBits(GPIOB, GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				break;

			default:
				break;
		}

		/* Conversão para dígitos da Unidade/Dezena de minutos */
		digUnid = count / 60;
		digDez = count / 600;

		/* Vetor da tabela de binários vai até posição 9 apenas! */
		if(digUnid > 9)
		{
			digUnid = digUnid % 10;
		}
		if(digDez > 9)
		{
			digDez = digDez / 10;
		}

		if(count == 0)
		{
			/* Fim de Contagem, imprime risquinhos */
			GPIO_SetBits(GPIOB, GPIO_Pin_4 |GPIO_Pin_6);
			GPIO_Write(GPIOA, FimdaContagem << 8 | FimdaContagem);
		}
		else
		{
			/* Somente enquanto contagem está ativa imprime os valores nos displays */
			switch (DisplayState)
			{
				case 0:
					GPIO_SetBits(GPIOB, GPIO_Pin_6);
					GPIO_ResetBits(GPIOB, GPIO_Pin_4);
					GPIO_Write(GPIOA, (GPIO_ReadOutputData(GPIOA) & 0xFF00) | TabDigNew[digUnid]);
					break;

				case 1:
					GPIO_SetBits(GPIOB, GPIO_Pin_4);
					GPIO_ResetBits(GPIOB, GPIO_Pin_6);
					GPIO_Write(GPIOA, (GPIO_ReadOutputData(GPIOA) & 0xFF00) | TabDigNew[digDez]);
					break;

				default:
					break;
			}
		}
  }
}
