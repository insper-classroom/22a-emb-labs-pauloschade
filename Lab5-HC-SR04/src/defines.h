#ifndef DEFINES_H_   /* Include guard */
#define DEFINES_H_

//MAIN BUTTON
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX  11
#define BUT_IDX_MASK (1 << BUT_IDX)

//LED
#define LED_PIO   PIOC
#define LED_PIO_ID    ID_PIOC
#define LED_PIO_IDX   8
#define LED_IDX_MASK (1u << LED_PIO_IDX)

//LEDS OLED1
#define LED1_PIO PIOA
#define LED1_PIO_ID	ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_IDX_MASK (1 << LED1_PIO_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID	ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_IDX_MASK (1 << LED2_PIO_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID	ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_IDX_MASK (1 << LED3_PIO_IDX)

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
#define BUT3_IDX_MASK (1 << BUT3_PIO_IDX

//ECHO
#define ECHO_PIO PIOD
#define ECHO_PIO_ID	ID_PIOD
#define ECHO_PIO_IDX 30
#define ECHO_IDX_MASK (1 << ECHO_PIO_IDX)

//
#define TRIG_PIO PIOC
#define TRIG_PIO_ID	ID_PIOC
#define TRIG_PIO_IDX 13
#define TRIG_IDX_MASK (1 << TRIG_PIO_IDX)

#endif // DEFINES_H_