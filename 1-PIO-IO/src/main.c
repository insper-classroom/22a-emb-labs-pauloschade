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

// Função de inicialização do uC
void init(void)
{
	sysclk_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	
	//LED
	pmc_enable_periph_clk(LED_PIO_ID);
	//PC8 as output
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0 ,0);
	
	//BUT
	pmc_enable_periph_clk(BUT_PIO_ID);
	pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, 1);
	
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
	pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK, 1);
	//2
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK, 1);
	//3
	pmc_enable_periph_clk(BUT3_PIO_ID);
	pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK, 1);
	
	
	
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int lab1(void)
{
  init();

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while (1)
  {
	  if(!pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK))
	  {
		for (int i = 0; i < 5; i++)
		{
			// 1 on LED Pin
			pio_set(PIOC, LED_PIO_IDX_MASK);
			delay_ms(200);
			// 0 on LED Pin
			pio_clear(PIOC, LED_PIO_IDX_MASK);
			delay_ms(200);
		}
	  } else {
			pio_clear(PIOC, LED_PIO_IDX_MASK);
	  }

  }
  return 0;
}

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
		for (int i = 0; i < 5; i++)
		{
			pio_set(PIOA, LED1_PIO_IDX_MASK);
			delay_ms(200);
			// 0 on LED Pin
			pio_clear(PIOA, LED1_PIO_IDX_MASK);
			delay_ms(200);
		}
	  } else {
			pio_clear(PIOA, LED1_PIO_IDX_MASK);
	  }
	  
	  if(!pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK))
	  {
		for (int i = 0; i < 5; i++)
		{
			pio_set(PIOC, LED2_PIO_IDX_MASK);
			delay_ms(200);
			// 0 on LED Pin
			pio_clear(PIOC, LED2_PIO_IDX_MASK);
			delay_ms(200);
		}
	  } else {
			pio_clear(PIOC, LED2_PIO_IDX_MASK);
	  }
	  
	  if(!pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK))
	  {
		for (int i = 0; i < 5; i++)
		{
			pio_set(PIOB, LED3_PIO_IDX_MASK);
			delay_ms(200);
			// 0 on LED Pin
			pio_clear(PIOB, LED3_PIO_IDX_MASK);
			delay_ms(200);
		}
	  } else {
			pio_clear(PIOB, LED3_PIO_IDX_MASK);
	  }
  }
  return 0;
}
