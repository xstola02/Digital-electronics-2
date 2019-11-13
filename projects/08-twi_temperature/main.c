/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2017-10-27
 * Last update: 2019-11-08
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Scan TWI bus for all connected slave devices and transmit info
 *    to UART.
 */

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>         // itoa() function
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "uart.h"
#include "twi.h"


/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600

/* Variables ---------------------------------------------------------*/


/* Function prototypes -----------------------------------------------*/


/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Test all TWI slave addresses and scan 
 *          connected devices.
 *  Input:  None
 *  Return: None
 */
int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    // TWI
    twi_init();

    /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */

    TIM_config_prescaler(TIM1, TIM_PRESC_256);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put strings to ringbuffer for transmitting via UART.



    // Infinite loop
    for (;;) {
    }

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Update state of TWI Finite
 *         State Machine.
 */
ISR(TIMER1_OVF_vect)
{
    char temp_ch[3];
    char temp_des_ch[2]; 
    char hum_ch[3];
    char hum_des_ch[2]; 
    int8_t hum;
    int8_t hum_des;
    int8_t temp;
    int8_t temp_des;
    twi_start((0x5c << 1)+ TWI_WRITE);    //5c      0b 0101 1100
    twi_write(0x00);
    twi_stop();  

    twi_start((0x5c << 1)+ TWI_READ);
    hum = twi_read_ack();
    hum_des = twi_read_ack();
    temp = twi_read_ack();
    temp_des = twi_read_nack();
    twi_stop();

    itoa(temp, temp_ch, 10);
    itoa(temp_des, temp_des_ch, 10);
    itoa(hum, hum_ch, 10);
    itoa(hum_des, hum_des_ch, 10);

    uart_puts("\n\rHumidity: ");
    uart_puts(hum_ch);
//    uart_puts(".");
//   uart_puts(hum_des_ch);
 //   uart_puts(" percent\r\n");

    uart_puts("\n\rTemperature: ");
    uart_puts(temp_ch);
    //uart_puts(".");
    //uart_puts(temp_des_ch);
//    uart_puts(" deg C\r\n");
}

/**
 *  Brief: Test one TWI address.
 */
