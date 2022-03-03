/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)

//LED
#define LED_PIO   PIOC  
// #      
#define LED_PIO_ID    ID_PIOC
#define LED_PIO_IDX   8    
#define LED_PIO_IDX_MASK (1u << LED_PIO_IDX)

//BUT
#define BUT_PIO		PIOA
#define BUT_PIO_ID	ID_PIOA
#define BUT_PIO_IDX 11
#define BUT_PIO_IDX_MASK (1 << BUT_PIO_IDX)

//LEDS OLED1
#define LED1_PIO PIOA
#define LED1_PIO_ID	ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1 << LED1_PIO_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID	ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1 << LED2_PIO_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID	ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_PIO_IDX_MASK (1 << LED3_PIO_IDX)

//BUTTONS OLED1
#define BUT1_PIO PIOD
#define BUT1_PIO_ID	ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_PIO_IDX_MASK (1 << BUT1_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID	ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1 << BUT2_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID	ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_PIO_IDX_MASK (1 << BUT3_PIO_IDX)


/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/
void _pio_set(Pio *p_pio, const uint32_t ul_mask);
void _pio_clear(Pio *p_pio, const uint32_t ul_mask);
void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable);
void _pio_set_input(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute);
void _pio_set_output(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level, const uint32_t ul_multidrive_enable, const uint32_t ul_pull_up_enable);
uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type, const uint32_t ul_mask);
void _delay_ms(int ms);
void blink_led(Pio *p_pio, uint32_t ul_mask);
void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void _pio_set(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio -> PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask,
	const uint32_t ul_pull_up_enable) 
{
	if (ul_pull_up_enable)
	{
		p_pio -> PIO_PUER = ul_mask;
		return;
	}
	p_pio -> PIO_PUDR = ul_mask;
}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask,
        const uint32_t ul_attribute)
{
	_pio_pull_up(p_pio, ul_mask, ul_attribute & _PIO_PULLUP);
	
	/* Configure pin as input */
	//Disable output
	p_pio->PIO_ODR = ul_mask;
	//Pin controlled by PIOc
	p_pio->PIO_PER = ul_mask;
	
	// Activate IFER to enable filtering
	if (ul_attribute & (_PIO_DEGLITCH | _PIO_DEBOUNCE)) {
		p_pio->PIO_IFER = ul_mask;
	} else {
		p_pio->PIO_IFDR = ul_mask;
		return;
	}
	
	//IFSCDR disables register, therefore enables Deglitch
	if (ul_attribute & PIO_DEGLITCH) 
	{
		p_pio->PIO_IFSCDR = ul_mask;
		return;
	}
	
	// If register is enabled, Debounce is activated
	if (ul_attribute & PIO_DEBOUNCE) 
	{
		p_pio->PIO_IFSCER = ul_mask;
	}
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask,
		const uint32_t ul_default_level,
		const uint32_t ul_multidrive_enable,
		const uint32_t ul_pull_up_enable)
{
	_pio_pull_up(p_pio, ul_mask, ul_pull_up_enable);
	
	/* Configure pin as output */
	//Enable output
	p_pio->PIO_OER = ul_mask;
	//Pin controlled by PIOc
	p_pio->PIO_PER = ul_mask;
	
	/* Set default value */
	if (ul_default_level) {
		_pio_set(p_pio, ul_mask);
	}
	else {
		_pio_clear(p_pio, ul_mask);
	}
	
	/* Enable multi-drive if necessary */
	if (ul_multidrive_enable) 
	{
		p_pio->PIO_MDER = ul_mask;
		return;
	}
	p_pio->PIO_MDDR = ul_mask;
}

uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type,
const uint32_t ul_mask)
{
	uint32_t IorO;

	if ((ul_type == PIO_OUTPUT_0)) {
		IorO = p_pio->PIO_ODSR;
	} else {
		IorO = p_pio->PIO_PDSR;
	}

	if ((IorO & ul_mask) == 0) {
		return 0;
	}	
	return 1;
}

void _delay_ms(int ms)
{
	for (int i = 0; i < ms * 150000; i++)
	{
		asm("nop");
	}
}

void blink_led(Pio *p_pio, uint32_t ul_mask)
{
	for (int i = 0; i < 5; i++)
	{
		_pio_set(p_pio, ul_mask);
		_delay_ms(200);
		// 0 on LED Pin
		_pio_clear(p_pio, ul_mask);
		_delay_ms(200);
	}
}

// Função de inicialização do uC
void init(void)
{
	sysclk_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	
	//OLED
	
	//LEDS
	//1
	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0 ,0);
	//2
	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0 ,0);
	//3
	pmc_enable_periph_clk(LED3_PIO_ID);
	pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0 ,0);
	
	//BUTTONS
	//1
	pmc_enable_periph_clk(BUT1_PIO_ID);
	_pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);



	//2
	pmc_enable_periph_clk(BUT2_PIO_ID);
	_pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);


	//3
	pmc_enable_periph_clk(BUT3_PIO_ID);
	_pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.

int main(void)
{
  init();

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while (1)
  {
	  //Press button 1
	  if(!_pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK))
	  {
		blink_led(PIOA, LED1_PIO_IDX_MASK);
	  } else {
			_pio_clear(PIOA, LED1_PIO_IDX_MASK);
	  }
	  
	  if(!_pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK))
	  {
		blink_led(PIOC, LED2_PIO_IDX_MASK);
	  } else {
			_pio_clear(PIOC, LED2_PIO_IDX_MASK);
	  }
	  
	  if(!_pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK))
	  {
		blink_led(PIOB, LED3_PIO_IDX_MASK);
	  } else {
			_pio_clear(PIOB, LED3_PIO_IDX_MASK);
	  }
  }
  return 0;
}
