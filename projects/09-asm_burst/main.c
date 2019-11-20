/***********************************************************************
 * 
 * Implementation of LFSR-based (Linear Feedback Shift Register) 
 * pseudo-random generator in AVR assembly.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-2019 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>         // itoa() function
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "uart.h"
#include "GPIO.h"

/* Typedef -----------------------------------------------------------*/
typedef enum {
    IDLE_STATE = 1,
    RAND4_STATE,
    RAND8_STATE,
    UART_STATE
} state_t;

/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600

/* Variables ---------------------------------------------------------*/
state_t current_state = IDLE_STATE;

/* Function prototypes -----------------------------------------------*/
void fsm_random(void);
extern uint8_t rand4_asm(uint8_t current);
extern uint8_t rand8_asm(uint8_t current);

/* Functions ---------------------------------------------------------*/
/* Call assembly functions to generate psudo-random values. */
/* Main --------------------------------------------------------------*/
/* Generate a sequence of LFSR preudo-random values using 4- and 8-bit
 * structure. */
int main(void)
{
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU)); // 8N1

    /* Timer1
     * TODO: Enable overflow interrupt every 33 msec. */
    TIM_config_prescaler(TIM1, TIM_PRESC_8);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
    sei();
    uart_puts("\r\n---LFSR pseudo-random generator---\r\n");

    GPIO_config_output (&DDRB, PB5);

    for (;;) {
       pulses(); 
    }

    return (0);
}

/* Interrupts --------------------------------------------------------*/
/* Timer1 overflow interrupt routine.
 * Update state of the FSM. */
ISR(TIMER1_OVF_vect)
{
    pulses();
}