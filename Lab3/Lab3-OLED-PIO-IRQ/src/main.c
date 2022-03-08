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

#define BUT2_PIO PIOC
#define BUT2_PIO_ID	ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_IDX_MASK (1 << BUT2_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID	ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_IDX_MASK (1 << BUT3_PIO_IDX)

/************************************************************************/
/* flags                                                   */
/************************************************************************/
volatile int freq_flag;
volatile int start_counter;
volatile char but_flag;
volatile char decrease_flag;

/************************************************************************/
/* prototypes                                                   */
/************************************************************************/
void but_callback(void);
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);

void increase(int *p_freq);
void decrease(int *p_freq);
void pisca_led(int n, int t);

void config_button(Pio *p_pio, const uint32_t ul_mask, uint32_t ul_id, void (*p_handler) (uint32_t, uint32_t), int it_rise);
void io_init(void);

/************************************************************************/
/* callbacks                                                              */
/************************************************************************/
void but_callback(void)
{
	but_flag = 1;
}

void but1_callback(void)
{
	start_counter = (pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK)) ? 0 : 1;
	freq_flag = 1;
}

void but2_callback(void)
{
	but_flag = 1;
}

void but3_callback(void)
{
	decrease_flag = 1;
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

void config_button(Pio *p_pio, const uint32_t ul_mask, uint32_t ul_id, void (*p_handler) (uint32_t, uint32_t), int it_rise) {
	pmc_enable_periph_clk(ul_id);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(p_pio, PIO_INPUT, ul_mask, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(p_pio, ul_mask, 60);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	uint32_t but_attr = (it_rise ? PIO_IT_RISE_EDGE : PIO_IT_EDGE);
	pio_handler_set(p_pio,
	ul_id,
	ul_mask,
	but_attr,
	p_handler);

	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(p_pio, ul_mask);
	pio_get_interrupt_status(p_pio);
	
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(ul_id);
	NVIC_SetPriority(ul_id, 4); // Prioridade 4
}
// Inicializa botao SW0 do kit com interrupcao

void io_init(void)
{
	config_button(BUT_PIO, BUT_IDX_MASK, BUT_PIO_ID, but_callback, 1);
	config_button(BUT1_PIO, BUT1_IDX_MASK, BUT1_PIO_ID, but1_callback, 0);
	config_button(BUT2_PIO, BUT2_IDX_MASK, BUT2_PIO_ID, but2_callback, 1);
	config_button(BUT3_PIO, BUT3_IDX_MASK, BUT3_PIO_ID, but3_callback, 1);
	
	
  pmc_enable_periph_clk(LED_PIO_ID);
  pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);	
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
		
		if(decrease_flag)
		{
			decrease(&freq);
			gfx_mono_draw_string("     ", 50, 16, &sysfont);
			sprintf(str, "%d", freq);
			gfx_mono_draw_string(str, 50, 16, &sysfont);
			decrease_flag = 0;
		}
		
		if(freq_flag) {
			if(start_counter) {
				c++;
			}
			else {
				if (c > 8000000) {
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
