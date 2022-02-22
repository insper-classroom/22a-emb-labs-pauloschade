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
