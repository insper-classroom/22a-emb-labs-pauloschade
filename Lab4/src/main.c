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
volatile char but_flag;
volatile char but1_flag;
volatile char but2_flag;
volatile char but3_flag;
volatile char flag_rtc_alarm;

/************************************************************************/
/* variables                                                   */
/************************************************************************/


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
	but1_flag = 1;
}

void but2_callback(void)
{
	but2_flag = 1;
}

void but3_callback(void)
{
	but3_flag = 1;
}

/************************************************************************/
/* functions                                                             */
/************************************************************************/
void pin_toggle(Pio *pio, uint32_t mask) {
	pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio,mask);
}

void pisca_led (int n, int t) {
	for (int i=0;i<n;i++){
		pio_clear(LED3_PIO, LED3_IDX_MASK);
		delay_ms(t);
		pio_set(LED3_PIO, LED3_IDX_MASK);
		delay_ms(t);
	}
}
static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
}

void set_alarm_but1() {
	uint32_t current_hour, current_min, current_sec;
	uint32_t current_year, current_month, current_day, current_week;
	rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
	rtc_get_date(RTC, &current_year, &current_month, &current_day, &current_week);
		
	rtc_set_date_alarm(RTC, 1, current_month, 1, current_day);
	rtc_set_time_alarm(RTC, 1, current_hour, 1, current_min, 1, current_sec + 20);
}
/************************************************************************/
/* handlers                                                             */
/************************************************************************/

void TC1_Handler(void) {
	
	volatile uint32_t status = tc_get_status(TC0, 1);
	pin_toggle(LED_PIO, LED_IDX_MASK);
}

void TC2_Handler(void) {

	volatile uint32_t status = tc_get_status(TC0, 2);
	pin_toggle(LED1_PIO, LED1_IDX_MASK);
}

void RTT_Handler(void) {
	uint32_t ul_status;

	ul_status = rtt_get_status(RTT);

	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		RTT_init(4, 16, RTT_MR_ALMIEN);
	}
	
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		pin_toggle(LED2_PIO, LED2_IDX_MASK);
	}

}

void RTC_Handler(void) {
	uint32_t ul_status = rtc_get_status(RTC);
	
	/* seccond tick */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		// o código para irq de segundo vem aqui
	}
	
	/* Time or date alarm */
	if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
		// o código para irq de alame vem aqui
		flag_rtc_alarm = 1;
	}

	rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

/************************************************************************/
/* start                                                                */
/************************************************************************/
void io_init(void)
{
	config_button(BUT_PIO, BUT_IDX_MASK, BUT_PIO_ID, but_callback, 1);
	config_button(BUT1_PIO, BUT1_IDX_MASK, BUT1_PIO_ID, but1_callback, 0);
	config_button(BUT2_PIO, BUT2_IDX_MASK, BUT2_PIO_ID, but2_callback, 1);
	config_button(BUT3_PIO, BUT3_IDX_MASK, BUT3_PIO_ID, but3_callback, 1);
	
	
	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_IDX_MASK, PIO_DEFAULT);
	
	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_IDX_MASK, PIO_DEFAULT);
	
	pmc_enable_periph_clk(LED3_PIO_ID);
	pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_IDX_MASK, PIO_DEFAULT);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();

  // Init OLED
	gfx_mono_ssd1306_init();
	io_init();
	
	//5HZ SAME70 LED
  TC_init(TC0, ID_TC1, 1, 5);
  tc_start(TC0, 1);
	
	//4HZ OLED LED
	TC_init(TC0, ID_TC2, 2, 4);
	tc_start(TC0, 2);
	
	RTT_init(4, 16, RTT_MR_ALMIEN); 


	//RTC
	calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_ALREN);

  /* Insert application code here, after the board has been initialized. */
	while(1) {
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		
		if(flag_rtc_alarm){
			pisca_led(5, 200);
			flag_rtc_alarm = 0;
		}
		
		if(but1_flag) {
			set_alarm_but1();
			but1_flag = 0;
		}
	}
}

// 		for(int i=70;i<=120;i+=2){
//
// 			gfx_mono_draw_rect(i, 5, 2, 10, GFX_PIXEL_SET);
// 			delay_ms(10);
//
// 		}
