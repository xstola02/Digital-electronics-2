/*
 * ---------------------------------------------------------------------
 * Author:      Hana Stolarova,  Vladimir Lahoda
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2019-11-26
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *   Project for our seminar DE2. It messures hight (altitude) based on pressure data and 
 *   compares it to GPS data.
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
#define UART_BAUD_RATE 9600
#define BMP_adress 0b01110110
#define UART_BUFLEN 10
/* Variables ---------------------------------------------------------*/
char gps[70];
/* Function prototypes -----------------------------------------------*/


/* Functions ---------------------------------------------------------*/



void display_values(int pressure, int altitude_b, int altitude_gps, double temperature) //Function fordisplay values 
 {
    int altitude_dif = abs(altitude_gps - altitude_b);

    char pressure_ch[10];           // declaration of char fields so they can be displayed on Nokia display
    char altitude_b_ch[10];
    char altitude_gps_ch[10];
    char altitude_dif_ch[10];
    char temperature_ch[10];

    itoa(pressure, pressure_ch, 10);           // conversion from int data to string   
    itoa(altitude_b/100, altitude_b_ch, 10);
    itoa(altitude_gps/100, altitude_gps_ch, 10);
    itoa(altitude_dif/100, altitude_dif_ch, 10);
    itoa(temperature, temperature_ch, 10);
    
    nokia_lcd_clear();                          //clear display
    nokia_lcd_write_string("Pres: ",1);          //display values of char fields - pressure, altitude, temperature, GPS coordinates
    nokia_lcd_write_string(pressure_ch,1);
    nokia_lcd_write_string("hPa",1);
    nokia_lcd_set_cursor(0, 10);
    nokia_lcd_write_string("BMP: ", 1);
    nokia_lcd_write_string(altitude_b_ch,1);
    nokia_lcd_write_string(" m",1);
    nokia_lcd_set_cursor(0, 20);
    nokia_lcd_write_string("GPS: ", 1);
    nokia_lcd_write_string(altitude_gps_ch,1);
    nokia_lcd_write_string(" m",1);
    nokia_lcd_set_cursor(0, 30);
    nokia_lcd_write_string("Diff: ", 1);
    nokia_lcd_write_string(altitude_dif_ch,1);
    nokia_lcd_write_string(" m",1);
    nokia_lcd_set_cursor(0, 40);
    nokia_lcd_write_string("Temp: ", 1);
    nokia_lcd_write_string(temperature_ch,1);
    nokia_lcd_write_string(" C",1);
    nokia_lcd_render();
 }

/*
*               function we are not using in our project, 
*               but when we tried to make the project work from the beginning,
*               it wasnt working properly without this particular function 
*               now the project is working as it should without this function, but just in case we left it here
void uart_print(char *name, long val)   
{
	char debug_buffer[UART_BUFLEN];

        uart_puts(name);
        uart_puts(" = ");

        ltoa((val), debug_buffer, UART_BUFLEN);
        uart_puts(debug_buffer);
        uart_puts("\n");
}*/

int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    //uart_puts("ok");
    // Initialisation BME 280
    bmp280_init();
    //uart_puts("ok");
    
    // Timer1
    TIM_config_prescaler(TIM1, TIM_PRESC_256);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    // Display init
    nokia_lcd_init();
    nokia_lcd_clear();
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    for (;;) {
        
  /*          for(uint8_t i = 0; i < 68; i++)
            {
                gps[i] = uart_getc();
            }
        
        nokia_lcd_clear();
        nokia_lcd_set_cursor(0,0);
        nokia_lcd_write_string(gps, 1);
        nokia_lcd_render();
  */   }

    // Will never reach this
    return (0);
}


    // Timer1 overflow interrupt routine.
 
ISR(TIMER1_OVF_vect)
{
    
    double press_ref = 1020;        //Select pressure reference - we want to add a function which will display a choice of pressure reference
	bmp280_measure();               // Request for download actual data from BME registers
	double pressure = (bmp280_getpressure()/100+32);
    double temperature = 1 ;//bmp280_gettemperature()/100;
    uint64_t altitude_b = ((press_ref*100*(16000+(64*temperature))+pressure*100*(16000+(64*temperature)))/(100*(press_ref - pressure)));
    int altitude_gps = 28900;


    
    display_values(pressure, altitude_b, altitude_gps, temperature);

  
}
