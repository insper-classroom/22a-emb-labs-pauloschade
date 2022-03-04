#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/************************************************************************/
/* Defines                                                   */
/************************************************************************/

//MAIN BUTTON
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX  11
#define BUT_IDX_MASK (1 << BUT_IDX)

//LEDS OLED1
#define LED_PIO PIOA
#define LED_PIO_ID	ID_PIOA
#define LED_PIO_IDX 0
#define LED_IDX_MASK (1 << LED_PIO_IDX)
//BUTTONS OLED1
#define BUT1_PIO PIOD
#define BUT1_PIO_ID	ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_IDX_MASK (1 << BUT1_PIO_IDX)

/************************************************************************/
/* flags                                                   */
/************************************************************************/
volatile int freq_flag;
volatile int start_counter;
volatile char but_flag;

/************************************************************************/
/* callbacks                                                              */
/************************************************************************/
void but_callback(void)
{
	but_flag = 1;
	
	if(!pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK))
	{
		start_counter = 1;
		return;
	}
	start_counter = 0;
}

void freq_callback(void)
{
	
	start_counter = (pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK)) ? 0 : 1;
	
	freq_flag = 1;
	
	
}

/************************************************************************/
/* funções                                                              */
/************************************************************************/
// pisca led N vez no periodo T
void pisca_led(int n, int t){
	for (int i=0;i<n;i++){
		pio_clear(LED_PIO, LED_IDX_MASK);
		delay_ms(t);
		pio_set(LED_PIO, LED_IDX_MASK);
		delay_ms(t);
	}
}

void decrease(int *p_freq)
{
	if (*p_freq == 0)
	{
		return;
	}
	
	 *p_freq -= 100;
}

void increase(int *p_freq)
{	
	*p_freq += 100;
}


// Inicializa botao SW0 do kit com interrupcao
void io_init(void)
{
	pmc_enable_periph_clk(BUT_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 60);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT_PIO,
	BUT_PIO_ID,
	BUT_IDX_MASK,
	PIO_IT_RISE_EDGE,
	but_callback);

	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
	pio_get_interrupt_status(BUT_PIO);
		
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 3); // Prioridade 3
	
	/************************************************************************/
	/* OLED                                                              */
	/************************************************************************/
	// Configura led
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);

	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT1_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_IDX_MASK, 60);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT1_PIO,
	BUT1_PIO_ID,
	BUT1_IDX_MASK,
	PIO_IT_EDGE,
	freq_callback);

	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4); // Prioridade 4
}

int main (void)
{
	int c;
	char str[128];
	int freq = 400;
	
	board_init();
	sysclk_init();
	io_init();
	delay_init();

  // Init OLED
	gfx_mono_ssd1306_init();
	sprintf(str, "%d", freq);
	gfx_mono_draw_string(str, 50, 16, &sysfont);
  /* Insert application code here, after the board has been initialized. */
	while(1) 
	{
		if(but_flag)
		{
			pisca_led(5, 100000/freq);
			but_flag = 0;
			
		}
		if(freq_flag) {
			if(start_counter) {
				c++;
			}
			else {
				if (c > 10000000) {
					decrease(&freq);
				}
				else {
					increase(&freq);
				}
				c = 0;
				freq_flag = 0;
				gfx_mono_draw_string("     ", 50, 16, &sysfont);
				sprintf(str, "%d", freq);
				gfx_mono_draw_string(str, 50, 16, &sysfont);
			}

		}
	}
}
