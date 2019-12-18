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
#include <string.h>
#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "uart.h"
#include "twi.h"
#include "nokia5110.h"
#include "bmp280.h"
#include "GPIO.h"


/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600
#define BMP_adress 0b01110110
//#define UART_BUFLEN 100
/* Variables ---------------------------------------------------------*/
char gps[100];
//char* gps_1[70];
char *gps_2[10];
double alti_gps;
char alti[10];
char* id;
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
    itoa(altitude_gps, altitude_gps_ch, 10);
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

int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    //uart_puts("ok");
    // Initialisation BMP 280
    bmp280_init();
    //uart_puts("ok");
    // Timer1
    TIM_config_prescaler(TIM1, TIM_PRESC_1024);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    // Display init
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_set_cursor(10,0);             // Displayed logo for 2s
    nokia_lcd_write_string("ALTI", 3);
    nokia_lcd_set_cursor(2,25);
    nokia_lcd_write_string("meter", 3);
    nokia_lcd_render();
    _delay_ms(2000);

    nokia_lcd_clear();
    nokia_lcd_write_string("Pres: ",1);
    nokia_lcd_set_cursor(0, 10);
    nokia_lcd_write_string("BMP: ", 1);
    nokia_lcd_set_cursor(0, 20);
    nokia_lcd_write_string("GPS: ", 1);
    nokia_lcd_set_cursor(0, 30);
    nokia_lcd_write_string("Diff: ", 1);
    nokia_lcd_set_cursor(0, 40);
    nokia_lcd_write_string("Temp: ", 1);
    nokia_lcd_render();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    for (;;) {
        while (!(UCSR0A & (1<<RXC0)))       // Recomended code from datasheet (Atmega 328P) - If there are unread data in the buffer, a condition occurs
        {
            if(uart_getc() == '$')          // If first char is '$' (ID char for PA6H message)
            {
                for(uint8_t i = 0; i < 99; i++)     // Print message from Buffer to string gps
                {
                    gps[i] = uart_getc();
                }
                gps[99] = '\0';             // Print "\0" to end of message
            }
            
            if (strstr(gps,"GPGGA") != NULL)    // Search key "GPGGA" in received message
            {
                char* gps_1 = gps;
                int u = 0;
                while((id = strtok_r(gps_1,",",&gps_1)))    //Divides string into several strings, when he reads ","
                {
                    gps_2[u] = id; 
                    u++; 
                }
                alti_gps = atof(gps_2[9]);                    //Choses the only string we need from the bunch and assigns it to variable alti_gps
            }
        }
    }

    // Will never reach this
    return (0);
}


    // Timer1 overflow interrupt routine.
 
ISR(TIMER1_OVF_vect)
{
    double press_ref = 1026;        //Select pressure reference - we want to add a function which will display a choice of pressure reference
    bmp280_measure();               // Request for download actual data from BME registers
    double pressure = (bmp280_getpressure()/100+32);
    double temperature = bmp280_gettemperature()/100;                 //for testing we chose temperature -1degrese for accurate temperature outside 
    uint64_t altitude_b = abs(((press_ref*100*(16000+(64*temperature))+pressure*100*(16000+(64*temperature)))/(100*(press_ref - pressure))));
                                        // equation for restatement pressure to altitude with temperature compansation
    
    display_values(pressure, altitude_b, alti_gps, temperature);

}
