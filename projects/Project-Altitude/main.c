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
#include "nokia5110.h"

/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600
#define BMP_adress 0b01110110

/* Variables ---------------------------------------------------------*/


/* Function prototypes -----------------------------------------------*/


/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Test all TWI slave addresses and scan 
 *          connected devices.
 *  Input:  None
 *  Return: None
 */

void BMP_280_configuration(void)
{
    twi_start((BMP_adress << 1) + TWI_WRITE);
    twi_write(0xF4);
    twi_write(0b10110111);
    twi_write(0xF5);
    twi_write(0b00010000);
    twi_stop();
}

int BMP_280_read_pressure(void)
{
    twi_start((BMP_adress << 1) + TWI_WRITE);
    twi_write(0xF7);
    twi_start((BMP_adress << 1) + TWI_READ);
    uint8_t msb_press = twi_read_ack();
    uint8_t lsb_press = twi_read_ack();
    uint8_t xlsb_press = twi_read_nack();
    //twi_stop();
    uint32_t pressure = 0;
    pressure = pressure + msb_press;
    pressure = pressure << 8;
    pressure = pressure + lsb_press;
    pressure = pressure << 8;
    pressure = pressure + xlsb_press;
    pressure = pressure >> 4;
    return pressure;
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
    nokia_lcd_write_string("Pressure:",1);
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

int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    // TWI
    twi_init();

    /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */
    BMP_280_configuration();
    TIM_config_prescaler(TIM1, TIM_PRESC_64);
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
    uint32_t pressure = BMP_280_read_pressure();
   
    int altitude_b = 210;
    int altitude_gps = 225;
    int temperature = 25;

    display_values(pressure, altitude_b, altitude_gps, temperature);
   
   
   
    char pressure_ch[10];





    itoa(pressure, pressure_ch, 10);
    
    uart_puts("\n\rPressure: ");
    uart_puts(pressure_ch);

    
}

/**
 *  Brief: Test one TWI address.
 */
