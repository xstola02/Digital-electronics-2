/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-23
 * Last update: 2019-11-01
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Analog-to-digital conversion with displaying result on LCD and 
 *    transmitting via UART.
 * 
 * Note:
 *    Peter Fleury's UART library.
 */

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>             // itoa() function
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "lcd.h"
#include "uart.h"

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600

/* Variables ---------------------------------------------------------*/
/* Function prototypes -----------------------------------------------*/
char lcd_user_symbols[16] = {
  0x0C,
  0x0D,
  0x05,
  0x1F,
  0x05,
  0x04,
  0x0A,
  0x0A,
  0x0E,
  0x11,
  0x1B,
  0x11,
  0x15,
  0x0E,
  0x0E,
  0x0E
};
/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Read ADC result and transmit via UART.
 *  Input:  None
 *  Return: None
 */

void tlacitka(int value)
{
    if(value >= 0 && value < 20)
    {
        lcd_gotoxy(11,0);
        lcd_puts("Right");
        uart_puts("Right\r\n");
    }
    else if(value >= 50 && value < 150)
    {
        lcd_gotoxy(7,0);
        lcd_puts("Up");
        uart_puts("Up\r\n");
    }
    else if(value >= 200 && value < 300)
    {
        lcd_gotoxy(6,1);
        lcd_puts("Down");
        uart_puts("Down\r\n");
    }
    else if(value >= 350 && value < 450)
    {
        lcd_gotoxy(0,0);
        lcd_puts("Left");
        uart_puts("Left\r\n");
    }
    else if(value >= 500 && value < 700)
    {
        lcd_gotoxy(10,1);
        lcd_puts("Select");
        uart_puts("Select\r\n");
    }
}

void getchar()
{
uint8_t vstup = uart_getc();

if(vstup != '\0') 
{
    if(vstup == '1') 
    {
        lcd_gotoxy(0,0);
        lcd_putc(0x00);
    }

}
}
void menu()
{
uint8_t vstup = uart_getc();

    if(vstup != '\0') 
    {
        if(vstup == '1') 
        {
            uart_puts("Timer1: ");
            uart_puts("546\r\n");
        }
        else if (vstup == '2')
        {
            uart_puts("Timer1: ");
            uart_puts("reset\r\n");
        }
        else
        {
            uart_puts("Bad command\r\n");
        }
    }
}

int main(void)
{
    // LCD display
    lcd_init(LCD_DISP_ON);
    lcd_puts("LCD testing");

    lcd_command(1<<LCD_CGRAM);
    // Store new characters line by line
    for(int i = 0; i<16; i++)
    {
    lcd_data(lcd_user_symbols[i]);
    }
    lcd_clrscr();
    /* ADC
     * TODO: Configure ADC reference, clock source, enable ADC module, 
     *       and enable conversion complete interrupt */
    ADMUX |= _BV(REFS0);
    ADCSRA |= _BV(ADEN) | _BV(ADIE) | _BV(ADPS0) |_BV(ADPS1) |_BV(ADPS2);
    /* Timer1
     * TODO: Configure Timer1 clock source and enable overflow 
     *       interrupt */
    TIM_config_prescaler(TIM1, TIM_PRESC_64);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    uart_puts ( " \033[3;46;94m " ) ; 
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put string to ringbuffer for transmitting via UART.
    uart_puts("--- Interactive menu ---\r\n");
    uart_puts("1: read current Timer / counter1 value\r\n");
    uart_puts("2: reset Timer / counter1\r\n");
    // Infinite loop
    for (;;) {
        getchar();      // get char 1 - show human with sord
        menu();
    }

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Start ADC conversion.
 */
ISR(TIMER1_OVF_vect)
{
    ADCSRA |= _BV(ADSC);
    
}

/**
 *  Brief: ADC complete interrupt routine. Update LCD and UART 
 *         transmiter.
 */
ISR(ADC_vect)
{
    uint16_t value = 0;
    char uart_string[4];

   lcd_clrscr();

	// Read 10-bit ACD value
    value = ADC;

    itoa(value, uart_string, 10);

    // TODO: Update LCD and UART transmiter
  /*  lcd_gotoxy(0,1);                  //show value of ADC
    lcd_puts(uart_string);
    uart_puts(uart_string);
    uart_puts("\r\n");
*/

    //tlacitka(value); //identify switches
    
}