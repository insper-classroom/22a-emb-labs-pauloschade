/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include <asf.h>
#include <string.h>
#include "ili9341.h"
#include "lvgl.h"
#include "touch/touch.h"

/************************************************************************/
/* LCD / LVGL                                                           */
/************************************************************************/

#define LV_HOR_RES_MAX          (320)
#define LV_VER_RES_MAX          (240)

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

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_LCD_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY            (tskIDLE_PRIORITY)

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

static void event_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
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

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void down_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
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
	
	lv_obj_t * btnClock = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btnClock, clock_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(btnClock, btnMenu, LV_ALIGN_OUT_RIGHT_MID, 0, y_disp/2);

	labelBtnClock = lv_label_create(btnClock);
	lv_label_set_text(labelBtnClock, LV_SYMBOL_SETTINGS " ]");
	lv_obj_center(labelBtnClock);
	lv_obj_add_style(btnClock, &style, 0);
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
	lv_obj_add_event_cb(upBtn, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(upBtn, downBut ,LV_ALIGN_OUT_LEFT_MID, -20, y_disp/2);
	
	labelBtnUp = lv_label_create(upBtn);
	lv_label_set_text(labelBtnUp, "[ " LV_SYMBOL_UP);
	lv_obj_center(labelBtnUp);
	lv_obj_add_style(upBtn, &style, 0);
}

void lv_termostato(void) {
	bottom_left();
	bottom_right();
}
/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_lcd(void *pvParameters) {
	int px, py;

	//lv_ex_btn_1();
	lv_termostato();

	for (;;)  {
		lv_tick_inc(50);
		lv_task_handler();
		vTaskDelay(50);
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

	/* Create task to control oled */
	if (xTaskCreate(task_lcd, "LCD", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){ }
}
