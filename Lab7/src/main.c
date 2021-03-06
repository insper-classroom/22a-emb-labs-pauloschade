/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include <asf.h>
#include <string.h>
#include "ili9341.h"
#include "lvgl.h"
#include "touch/touch.h"
#include "wclock.h"

/************************************************************************/
/* LCD / LVGL                                                           */
/************************************************************************/

#define LV_HOR_RES_MAX          (320)
#define LV_VER_RES_MAX          (240)

LV_FONT_DECLARE(dseg70);
LV_FONT_DECLARE(dseg50);
LV_FONT_DECLARE(dseg20);
SemaphoreHandle_t xSemaphoreRTC;


typedef struct  {
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t week;
	uint32_t hour;
	uint32_t minute;
	uint32_t seccond;
} calendar;

/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
static lv_indev_drv_t indev_drv;
static  lv_obj_t * labelBtn1;
static  lv_obj_t * labelBtnMenu;
static  lv_obj_t * labelBtnClock;
static  lv_obj_t * labelBtnUp;
static  lv_obj_t * labelBtnDown;
static lv_obj_t * labelFloor;
static lv_obj_t * labelFloorDecimal;
static lv_obj_t * labelTime;
static lv_obj_t * labelSet;
static lv_obj_t * labelFire;
static lv_obj_t * labelClock;
static lv_obj_t * labelHome;

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

SemaphoreHandle_t xPower;

#define TASK_LCD_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY            (tskIDLE_PRIORITY)
#define TASK_RTC_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_RTC_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);
void lv_termostato(void);

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}


/************************************************************************/
/* HANDLERS RTOS                                                        */
/************************************************************************/
void RTC_Handler(void) {
	uint32_t ul_status = rtc_get_status(RTC);
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	/* Time or date alarm */
	if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
		// o c?digo para irq de alame vem aqui
	}
	
	/* seccond tick */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		xSemaphoreGiveFromISR(xSemaphoreRTC, &xHigherPriorityTaskWoken);
	}

	rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

/************************************************************************/
/* lvgl                                                                 */
/************************************************************************/

lv_obj_t * create_and_align(int x_disp, int y_disp, lv_event_cb_t event_cb, lv_align_t align, int align_to, void * ref) {
	lv_obj_t * newBut = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(newBut, event_cb, LV_EVENT_ALL, NULL);
	align_to ? lv_obj_align_to(newBut, ref, align, x_disp, y_disp) : lv_obj_align(newBut, LV_ALIGN_BOTTOM_LEFT, x_disp, y_disp);
	return newBut;
	
}

// void label_button(lv_obj_t *labelBtn, lv_obj_t * parent, char *text) {
// 		labelBtn = lv_label_create(parent);
// 		lv_label_set_text(labelBtn1, *text);
// 		lv_obj_center(labelBtn);
// }

volatile char power_flag;

static void event_handler(lv_event_t * e) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		power_flag = !power_flag;
		xSemaphoreGiveFromISR(xPower, xHigherPriorityTaskWoken);
	}
}

static void menu_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void clock_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void up_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    char *c;
    int temp;
    if(code == LV_EVENT_CLICKED) {
	    c = lv_label_get_text(labelSet);
	    temp = atoi(c);
	    lv_label_set_text_fmt(labelSet, "%02d", temp + 1);
    }
}

static void down_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    char *c;
    int temp;
    if(code == LV_EVENT_CLICKED) {
	    c = lv_label_get_text(labelSet);
	    temp = atoi(c);
	    lv_label_set_text_fmt(labelSet, "%02d", temp - 1);
    }
}

void bottom_left(void) {
	static lv_style_t style;
	lv_coord_t y_disp = -20;
	lv_coord_t x_disp = 10;
	
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_black());
	lv_style_set_border_width(&style, 0);
	
	
	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, x_disp, y_disp);

	labelBtn1 = lv_label_create(btn1);
	lv_label_set_text(labelBtn1, "[ " LV_SYMBOL_POWER);
	lv_obj_center(labelBtn1);
	lv_obj_add_style(btn1, &style, 0);

	lv_obj_t * btnMenu = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btnMenu, menu_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(btnMenu, btn1, LV_ALIGN_OUT_RIGHT_MID, 0, y_disp/2);

	labelBtnMenu = lv_label_create(btnMenu);
	lv_label_set_text(labelBtnMenu, "| M |");
	lv_obj_center(labelBtnMenu);
	lv_obj_add_style(btnMenu, &style, 0);
	
	lv_obj_t * btnClock = lv_imgbtn_create(lv_scr_act());
	lv_imgbtn_set_src(btnClock, LV_IMGBTN_STATE_RELEASED, NULL, NULL, &wclock);
	lv_obj_add_event_cb(btnClock, clock_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(btnClock, btnMenu, LV_ALIGN_OUT_RIGHT_MID, -95, 55);

	labelBtnClock = lv_label_create(lv_scr_act());
	lv_label_set_text(labelBtnClock, " ]");
	lv_obj_align_to(labelBtnClock, btn1, LV_ALIGN_OUT_RIGHT_MID, 100, 0);
	lv_obj_add_style(btnClock, &style, 0);
	
	lv_obj_t * clockBtn = lv_imgbtn_create(lv_scr_act());
	lv_imgbtn_set_src(clockBtn, LV_IMGBTN_STATE_RELEASED, NULL, NULL, &wclock);
	lv_obj_add_event_cb(clockBtn, clock_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(clockBtn, btnMenu ,LV_ALIGN_OUT_RIGHT_MID, 20, y_disp);
}

void bottom_right(void) {
	static lv_style_t style;
	lv_coord_t y_disp = -20;
	lv_coord_t x_disp = -10;
	
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_black());
	lv_style_set_border_width(&style, 0);
	
	
	lv_obj_t * downBut = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(downBut, down_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(downBut, LV_ALIGN_BOTTOM_RIGHT, x_disp, y_disp);
	
	labelBtnDown = lv_label_create(downBut);
	lv_label_set_text(labelBtnDown, LV_SYMBOL_DOWN " ]");
	lv_obj_center(labelBtnDown);
	lv_obj_add_style(downBut, &style, 0);
	
	lv_obj_t * upBtn = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(upBtn, up_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(upBtn, downBut ,LV_ALIGN_OUT_LEFT_MID, -30, y_disp/2);
	
	labelBtnUp = lv_label_create(upBtn);
	lv_label_set_text(labelBtnUp, "[ " LV_SYMBOL_UP);
	lv_obj_center(labelBtnUp);
	lv_obj_add_style(upBtn, &style, 0);
	
	labelFire = lv_label_create(lv_scr_act());
	lv_label_set_text(labelFire, LV_SYMBOL_CHARGE);
	lv_obj_align_to(labelFire, downBut, LV_ALIGN_OUT_TOP_MID, 10, -40);
	lv_obj_add_style(labelFire, &style, 0);
	
	labelHome = lv_label_create(lv_scr_act());
	lv_label_set_text(labelHome, LV_SYMBOL_HOME);
	lv_obj_align_to(labelHome, upBtn, LV_ALIGN_OUT_TOP_LEFT, -20, -10);
	lv_obj_add_style(labelHome, &style, 0);
}

void center(void) {
	lv_coord_t y_disp_top = 10;
	lv_coord_t x_disp_top = -20;
	
	labelFloor = lv_label_create(lv_scr_act());
	lv_obj_align(labelFloor, LV_ALIGN_LEFT_MID, 35 , -45);
	lv_obj_set_style_text_font(labelFloor, &dseg70, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelFloor, lv_color_white(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelFloor, "%02d", 23);
	
	labelFloorDecimal = lv_label_create(lv_scr_act());
	lv_obj_align_to(labelFloorDecimal, labelFloor ,LV_ALIGN_OUT_RIGHT_MID, -0 , 0);
	lv_obj_set_style_text_font(labelFloorDecimal, &dseg50, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelFloorDecimal, lv_color_white(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelFloorDecimal, ".%d", 4);
	
	labelTime = lv_label_create(lv_scr_act());
	lv_obj_align(labelTime, LV_ALIGN_TOP_RIGHT, x_disp_top , y_disp_top);
	lv_obj_set_style_text_font(labelTime, &dseg20, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelTime, lv_color_white(), LV_STATE_DEFAULT);
	
	labelSet = lv_label_create(lv_scr_act());
	lv_obj_align_to(labelSet, labelTime ,LV_ALIGN_OUT_BOTTOM_MID, -30 , 10);
	lv_obj_set_style_text_font(labelSet, &dseg50, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelSet, lv_color_white(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelSet, "%02d", 22);
}

void lv_termostato(void) {
	bottom_left();
	bottom_right();
	center();
}

void lv_power(void) {
	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_black());
	lv_style_set_border_width(&style, 0);
	
	
	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, 10, -20);

	labelBtn1 = lv_label_create(btn1);
	lv_label_set_text(labelBtn1, LV_SYMBOL_POWER);
	lv_obj_center(labelBtn1);
	lv_obj_add_style(btn1, &style, 0);
}
/************************************************************************/
/* FUNCS                                                                */
/************************************************************************/
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type) {
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.seccond);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 4);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}

void update_timer() {
	uint32_t current_hour, current_min, current_sec;
	rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
	lv_label_set_text_fmt(labelTime, "%d:%d:%d", current_hour, current_min, current_sec);
}
/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_lcd(void *pvParameters) {
	int px, py;

	//lv_ex_btn_1();
	lv_termostato();

	for (;;)  {
		if (xSemaphoreTake(xPower,0)) {
			if(power_flag) {
				lv_obj_clean(lv_scr_act());
				lv_power();
			} else {
				lv_obj_clean(lv_scr_act());
				lv_termostato();
			}
		}
		lv_tick_inc(50);
		lv_task_handler();
		vTaskDelay(50);
	}
}

static void task_rtc(void *pvParameters) {
	calendar rtc_initial = {0, 0, 0, 0, 0, 0 ,0};
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_SECEN);
	for(;;) {
		if (xSemaphoreTake(xSemaphoreRTC, 0)) {
			update_timer();
		}
	}
}

/************************************************************************/
/* configs                                                              */
/************************************************************************/

static void configure_lcd(void) {
	/**LCD pin configure on SPI*/
	pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);  //
	pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
	pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
	pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
	pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
	pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
	
	ili9341_init();
	ili9341_backlight_on();
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
	};

	/* Configure console UART. */
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

/************************************************************************/
/* port lvgl                                                            */
/************************************************************************/

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
	ili9341_set_top_left_limit(area->x1, area->y1);   ili9341_set_bottom_right_limit(area->x2, area->y2);
	ili9341_copy_pixels_to_screen(color_p,  (area->x2 + 1 - area->x1) * (area->y2 + 1 - area->y1));
	
	/* IMPORTANT!!!
	* Inform the graphics library that you are ready with the flushing*/
	lv_disp_flush_ready(disp_drv);
}

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
	int px, py, pressed;
	
	if (readPoint(&px, &py))
		data->state = LV_INDEV_STATE_PRESSED;
	else
		data->state = LV_INDEV_STATE_RELEASED; 
	
	data->point.x = px;
	data->point.y = py;
}

void configure_lvgl(void) {
	lv_init();
	lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);
	
	lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;      /*Set the horizontal resolution in pixels*/
	disp_drv.ver_res = LV_VER_RES_MAX;      /*Set the vertical resolution in pixels*/

	lv_disp_t * disp;
	disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
	
	/* Init input on LVGL */
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_input_read;
	lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/
int main(void) {
	/* board and sys init */
	board_init();
	sysclk_init();
	configure_console();

	/* LCd, touch and lvgl init*/
	configure_lcd();
	configure_touch();
	configure_lvgl();
	
	xSemaphoreRTC = xSemaphoreCreateBinary();
	xPower = xSemaphoreCreateBinary();

	/* Create task to control oled */
	if (xTaskCreate(task_lcd, "LCD", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}
	
	if (xTaskCreate(task_rtc, "RTC", TASK_RTC_STACK_SIZE, NULL, TASK_RTC_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create rtc task\r\n");
	}
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){ }
}
