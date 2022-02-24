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

}

void blink_led(Pio *p_pio, uint32_t ul_mask)
{
	for (int i = 0; i < 5; i++)
	{
		_pio_set(p_pio, ul_mask);
		delay_ms(200);
		// 0 on LED Pin
		_pio_clear(p_pio, ul_mask);
		delay_ms(200);
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
	pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, _PIO_DEFAULT);
	_pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, 1);


	//2
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, _PIO_DEFAULT);
	_pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, 1);


	//3
	pmc_enable_periph_clk(BUT3_PIO_ID);
	pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, _PIO_DEFAULT);
	_pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, 1);
	

	
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
	  if(!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK))
	  {
		blink_led(PIOA, LED1_PIO_IDX_MASK);
	  } else {
			_pio_clear(PIOA, LED1_PIO_IDX_MASK);
	  }
	  
	  if(!pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK))
	  {
		blink_led(PIOC, LED2_PIO_IDX_MASK);
	  } else {
			_pio_clear(PIOC, LED2_PIO_IDX_MASK);
	  }
	  
	  if(!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK))
	  {
		blink_led(PIOB, LED3_PIO_IDX_MASK);
	  } else {
			_pio_clear(PIOB, LED3_PIO_IDX_MASK);
	  }
  }
  return 0;
}
