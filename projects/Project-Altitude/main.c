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
#include <stdlib.h>
#include <stdio.h>         // itoa() function
#include <avr/io.h>
#include <math.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "uart.h"
#include "twi.h"
#include "nokia5110.h"
#include "bmp280.h"


/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 115200
#define BMP_adress 0b01110110
#define UART_BUFLEN 10
/* Variables ---------------------------------------------------------*/

/* Function prototypes -----------------------------------------------*/


/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Test all TWI slave addresses and scan 
 *          connected devices.
 *  Input:  None
 *  Return: None
 */

int calc_alt(double pressure, double press_ref)
{
    double cit = log10(pressure/press_ref) * press_ref;
    double jmen = log10(2.718281) * 1.225 * 9.98;
    double alt = -cit / jmen;
    return alt;
}
void display_values(int pressure, int altitude_b, int altitude_gps, int temperature)
 {
    int altitude_dif = altitude_gps - altitude_b;

    char pressure_ch[10];
    char altitude_b_ch[10];
    char altitude_gps_ch[10];
    char altitude_dif_ch[10];
    char temperature_ch[10];

    itoa(pressure, pressure_ch, 10);
    itoa(altitude_b, altitude_b_ch, 10);
    itoa(altitude_gps, altitude_gps_ch, 10);
    itoa(altitude_dif, altitude_dif_ch, 10);
    itoa(temperature, temperature_ch, 10);
    
    nokia_lcd_clear();
    nokia_lcd_write_string("Pres:",1);
    nokia_lcd_write_string(pressure_ch,1);
    nokia_lcd_set_cursor(0, 10);
    nokia_lcd_write_string("Alt BMP:", 1);
    nokia_lcd_write_string(altitude_b_ch,1);
    nokia_lcd_set_cursor(0, 20);
    nokia_lcd_write_string("Alt GPS:", 1);
    nokia_lcd_write_string(altitude_gps_ch,1);
    nokia_lcd_set_cursor(0, 30);
    nokia_lcd_write_string("Difference:", 1);
    nokia_lcd_write_string(altitude_dif_ch,1);
    nokia_lcd_set_cursor(0, 40);
    nokia_lcd_write_string("Temperature:", 1);
    nokia_lcd_write_string(temperature_ch,1);
    nokia_lcd_render();
 }
void uart_print(char *name, long val)
{
	char debug_buffer[UART_BUFLEN];

        uart_puts(name);
        uart_puts(" = ");

        ltoa((val), debug_buffer, UART_BUFLEN);
        uart_puts(debug_buffer);
        uart_puts("\n");
}
int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    uart_puts("ok");
    // TWI
    bmp280_init();
    uart_puts("ok");
    /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */
    
    TIM_config_prescaler(TIM1, TIM_PRESC_256);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
   
    nokia_lcd_init();
    nokia_lcd_clear();
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
    
    double press_ref = 1013;
	bmp280_measure();
	double	pressure = (bmp280_getpressure()/100+32);
    double altitude_b = calc_alt(pressure, press_ref);
    int altitude_gps = 225;
    int temperature = bmp280_gettemperature()/100;

    display_values(pressure, altitude_b, altitude_gps, temperature);

  
}
