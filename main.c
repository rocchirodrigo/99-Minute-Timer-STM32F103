/* Includes */
#include "stm32f10x.h"
/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
volatile uint8_t TimerState = 0;
volatile uint8_t Toggle = 0;
uint8_t ProgramError = 0;

/* variáveis Granzi (pode excluir que vai continuar funcionando) XXX */
/* Tabela começa com dígito 1, 2,...9, 0 */
const uint16_t TabDig[10] = {0b0111111, 0b0010100, 0b1011011, 0b1011110, 0b1110100, 0b1101110,
		                    0b1101111, 0b0011100, 0b1111111, 0b1111110};
const uint16_t TabDigDez[10] = {0b100111101, 0b000010001, 0b100111010, 0b100011011, 0b000010111, 0b100001111,
        0b100101111, 0b000011001, 0b100111111, 0b100011111};

volatile uint16_t digUnid = 0, digDez = 0;
volatile uint32_t count = 0;

/* end Granzi XXX */

/* Private function prototypes */
void Button_Configuration(void);
void Display_Unidade_Configuration(void);
void Display_Dezena_Configuration(void);
void Leds_Configuration(void);
void SysTick_Configuration(void);
void RTC_Configuration(void);
void Delay(uint16_t k);
/* Private functions */
void Button_Configuration(void)
{
	/* Botões:
	 * PB0: Altera os estados do timer
	 * PB1: BotUP - incrementa minutos (só ativo na configuração do timer).
	 * PB10: BotDOWN - decrementa minutos (só ativo na configuração do timer).
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

	/* PB10 */
	myIntButton.NVIC_IRQChannel = EXTI3_IRQn;
	myIntButton.NVIC_IRQChannelPreemptionPriority = 3;
	myIntButton.NVIC_IRQChannelSubPriority = 3;
	myIntButton.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&myIntButton);
}

void Display_Unidade_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* XXX Configurar ordem! */
	GPIO_InitTypeDef myDisplay;

	myDisplay.GPIO_Mode = GPIO_Mode_Out_PP;
	//myDisplay.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	myDisplay.GPIO_Pin = GPIO_Pin_All;
	myDisplay.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_Init(GPIOA, &myDisplay);
}

void Display_Dezena_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* XXX Configurar ordem! */
	GPIO_InitTypeDef myDisplay;

	myDisplay.GPIO_Mode = GPIO_Mode_Out_PP;
	myDisplay.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
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
	myLed.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	myLed.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOB, &myLed);
}

void SysTick_Configuration(void)
{
	SysTick_Config(9000000);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

	NVIC_InitTypeDef mySystick;

	mySystick.NVIC_IRQChannel = SysTick_IRQn;
	mySystick.NVIC_IRQChannelPreemptionPriority = 1;
	mySystick.NVIC_IRQChannelSubPriority = 1;
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

int main(void)
{
	/* Usar funções já declaradas no escopo! */
	__disable_irq();
	Button_Configuration();
	Display_Unidade_Configuration();
	Leds_Configuration();
	SysTick_Configuration();
	RTC_Configuration();
	__enable_irq();

	/* Infinite loop */
	while (1)
	{
		/* 3 Estados do timer:
		 * 0: Default, para configuração do tempo.
		 * 1: Timer ativo, decrementando
		 * 2: Timer pausado
		 */

		switch (TimerState)
		{
			case 0:
				/* Configuração do Timer. */
				GPIO_ResetBits(GPIOB, GPIO_Pin_7);
				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
				GPIO_ResetBits(GPIOB, GPIO_Pin_9);
				break;

			case 1:
				/* Decrementa o timer. */
				if(count == 0) /* Acabou a contagem */
				{
					GPIO_SetBits(GPIOB, GPIO_Pin_7);
					GPIO_SetBits(GPIOB, GPIO_Pin_8);
					GPIO_ResetBits(GPIOB, GPIO_Pin_9);
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
				GPIO_ResetBits(GPIOB, GPIO_Pin_9);
				break;

			default:
				/* Erro - tratamento de exceção (debug) */
				ProgramError = 99;
				while(1)
				{
				}
				break;
		}

		digUnid = count / 60;
		digDez = count / 600;

		if(digUnid > 9)
		{
			digUnid = digUnid % 10;
		}
		if(digDez > 9)
		{
			digDez = digDez / 10;
		}

		GPIO_Write(GPIOA, (TabDigDez[digDez] << 7 | TabDig[digUnid]));
  }
}
