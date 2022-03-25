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
double freq = (float) 1/(2*0.000058);

/************************************************************************/
/* variables                                                   */
/************************************************************************/


/************************************************************************/
/* prototypes                                                   */
/************************************************************************/
void but1_callback(void);
void echo_callback(void);


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
// 	if (!echo_flag) {
// 		RTT_init(freq, 0, 0);
// 	}
	rtt_start = pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK) ? 1 : 0;
	echo_flag = 1;
}
/************************************************************************/
/* functions                                                             */
/************************************************************************/
void pin_toggle(Pio *pio, uint32_t mask) {
	pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio,mask);
}

static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
	return ul_previous_time;
}

void trig_pulse(){
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

	ul_status = rtt_get_status(RTT);

	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		RTT_init(4, 16, 0);
	}
	
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		pin_toggle(LED2_PIO, LED2_IDX_MASK);
	}

}



/************************************************************************/
/* start                                                                */
/************************************************************************/
void io_init(void)
{
	config_button(BUT1_PIO, BUT1_IDX_MASK, BUT1_PIO_ID, but1_callback, 0); 
	config_button(ECHO_PIO, ECHO_IDX_MASK, ECHO_PIO_ID, echo_callback, 1);
	
	pmc_enable_periph_clk(TRIG_PIO_ID);
	pio_configure(TRIG_PIO, PIO_OUTPUT_1, TRIG_IDX_MASK, PIO_DEFAULT);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();

  // Init OLED
	gfx_mono_ssd1306_init();
	io_init();
	
	
	RTT_init(4, 16, RTT_MR_ALMIEN); 




  /* Insert application code here, after the board has been initialized. */
	while(1) {
		if(but1_flag) {
			trig_pulse();
			but1_flag = 0;
		}
		if(echo_flag) {
			if(rtt_start) {
				RTT_init(freq, 0, 0);
			} else {
				
			}
		}
	}
}

// 		for(int i=70;i<=120;i+=2){
//
// 			gfx_mono_draw_rect(i, 5, 2, 10, GFX_PIXEL_SET);
// 			delay_ms(10);
//
// 		}
