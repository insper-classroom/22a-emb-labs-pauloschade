#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
#include "helpers.h"
#include "defines.h"

/************************************************************************/
/* flags                                                   */
/************************************************************************/

volatile int start_counter;
volatile char but1_flag;
volatile char echo_flag;
volatile char rtt_start;
int freq = 1/(2*0.000058);
volatile uint32_t rtt_time = 0;

/************************************************************************/
/* variables                                                   */
/************************************************************************/
#define SOUND_SPEED 340.0

/************************************************************************/
/* prototypes                                                   */
/************************************************************************/
void but1_callback(void);
void echo_callback(void);
static uint32_t get_time_rtt();
void io_init(void);

/************************************************************************/
/* callbacks                                                              */
/************************************************************************/
void but1_callback(void)
{
	but1_flag = 1;
}

void echo_callback(void)
{
	if (pio_get(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK)) {
		 RTT_init(freq, 0, 0);
	} else { 
		echo_flag = 1;
		rtt_time = get_time_rtt();
	}
}
/************************************************************************/
/* functions                                                             */
/************************************************************************/
void pin_toggle(Pio *pio, uint32_t mask) {
	pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio,mask);
}

static uint32_t get_time_rtt() {
	return rtt_read_timer_value(RTT);
}

void pulse() {
	pio_set(TRIG_PIO, TRIG_IDX_MASK);
	delay_us(10);
	pio_clear(TRIG_PIO, TRIG_IDX_MASK);
}

void set_alarm_but1();
/************************************************************************/
/* handlers                                                             */
/************************************************************************/

void RTT_Handler(void) {
	uint32_t ul_status;

	/* Get RTT status - ACK */
	ul_status = rtt_get_status(RTT);
	
	/* IRQ due to Time has changed */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		//pin_toggle(TRIG_PIO, TRIG_IDX_MASK);    // BLINK Led
	}

}



/************************************************************************/
/* start                                                                */
/************************************************************************/
void io_init(void)
{
	config_button(BUT1_PIO, BUT1_IDX_MASK, BUT1_PIO_ID, but1_callback, 1, 1); 
	config_button(ECHO_PIO, ECHO_IDX_MASK, ECHO_PIO_ID, echo_callback, 0, 0);
	
	pmc_enable_periph_clk(TRIG_PIO_ID);
	pio_configure(TRIG_PIO, PIO_OUTPUT_1, TRIG_IDX_MASK, PIO_PULLUP);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	
	char str[10];
	float distance;

  // Init OLED
	gfx_mono_ssd1306_init();
	io_init(); 
	gfx_mono_draw_string("init", 0, 16, &sysfont);

  /* Insert application code here, after the board has been initialized. */
	while(1) {
		if(but1_flag) {
			pulse();
			but1_flag = 0;
		}
		if(echo_flag) {
			distance = (SOUND_SPEED*rtt_time)/(2.0 * freq);
			gfx_mono_draw_string("     ", 0, 16, &sysfont);
			sprintf(str, "%f", distance);
			gfx_mono_draw_string(str, 0, 16, &sysfont);
			echo_flag = 0;
		}
	}
}

// 		for(int i=70;i<=120;i+=2){
//
// 			gfx_mono_draw_rect(i, 5, 2, 10, GFX_PIXEL_SET);
// 			delay_ms(10);
//
// 		}
